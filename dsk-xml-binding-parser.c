#include <string.h>
#include <stdlib.h>

/* type_decl -> STRUCT name LBRACE member_list RBRACE
	      | UNION name opt_extends LBRACE case_list RBRACE
              | ENUM name LBRACE enum_value_list RBRACE
              | SEMICOLON ;
   type_decls -> type_decl type_decls | ;
   name -> BAREWORD ;
   opt_name -> BAREWORD | ;
   member -> dotted_bareword(type_ref) quantity BAREWORD(name) SEMICOLON
   member_list = member member_list | ;
   quantity = '!' | '?' | '*' | '+' ;
   case -> BAREWORD(label) COLON BAREWORD(type) SEMICOLON
         | BAREWORD(label) COLON LBRACE member_list RBRACE SEMICOLON ;
   case_list = case case_list | ;
   namespace_decl -> NAMESPACE DOTTED_BAREWORD SEMICOLON
   use_statement -> USE DOTTED_BAREWORD opt_as_clause SEMICOLON
   opt_as_clause -> AS BAREWORD | ;
   file -> namespace_decl use_statements type_decls
 */

#include "dsk.h"
#include "dsk-xml-binding-internals.h"

typedef enum
{
  /* reserved words */
  TOKEN_STRUCT,
  TOKEN_UNION,
  TOKEN_CASE,
  TOKEN_AS,
  TOKEN_USE,
  TOKEN_NAMESPACE,
  TOKEN_EXCLAMATION_POINT,
  TOKEN_QUESTION_MARK,
  TOKEN_ASTERISK,
  TOKEN_PLUS,
  TOKEN_COLON,

  /* naked word, not a reserved word */
  TOKEN_BAREWORD,

  /* punctuation */
  TOKEN_LBRACE,
  TOKEN_RBRACE,
  TOKEN_DOT,
  TOKEN_SEMICOLON,

} TokenType;
#define token_type_is_word(tt)   ((tt) <= TOKEN_BAREWORD)

#define LBRACE_STR  "{"
#define RBRACE_STR  "}"

typedef struct Token
{
  TokenType type;
  unsigned start, length;
  unsigned line_no;
} Token;

#define TOKENIZE_INIT_TOKEN_COUNT 32

/* NOTE: we create a sentinel token at the end just for the line-number. */
static Token *
tokenize (const char *filename,
          const char *str,
          unsigned *n_tokens_out,
          DskError **error)
{
  unsigned n = 0;
  Token *rv = DSK_NEW_ARRAY (TOKENIZE_INIT_TOKEN_COUNT, Token);
  unsigned line_no = 1;
  const char *at = str;
#define ADD_TOKEN(type_, start_, len_) \
  do { \
    if (n >= TOKENIZE_INIT_TOKEN_COUNT && ((n) & (n-1)) == 0) \
      rv = dsk_realloc (rv, sizeof (Token) * n * 2); \
    rv[n].type = type_; rv[n].start = start_; rv[n].length = len_; rv[n].line_no = line_no; \
    n++; \
  } while(0)
#define ADD_TOKEN_ADVANCE(type_, len_) \
    do { ADD_TOKEN (type_, at - str, (len_)); at += (len_); } while(0)

  while (*at)
    {
      if (('a' <= *at && *at <= 'z')
       || ('A' <= *at && *at <= 'Z')
       || *at == '_')
        {
          const char *end = at + 1;
          TokenType type;
          while (('a' <= *end && *end <= 'z')
              || ('A' <= *end && *end <= 'Z')
              || ('0' <= *end && *end <= '9')
              || *end == '_')
            end++;
          type = TOKEN_BAREWORD;
          switch (*at)
            {
            case 's':
              if (strncmp (at, "struct", 6) == 0 && (end-at) == 6)
                type = TOKEN_STRUCT;
              break;
            case 'c':
              if (strncmp (at, "case", 4) == 0 && (end-at) == 4)
                type = TOKEN_CASE;
              break;
            case 'a':
              if (strncmp (at, "as", 2) == 0 && (end-at) == 2)
                type = TOKEN_AS;
              break;
            case 'u':
              if (strncmp (at, "use", 3) == 0 && (end-at) == 3)
                type = TOKEN_USE;
              if (strncmp (at, "union", 5) == 0 && (end-at) == 5)
                type = TOKEN_UNION;
              break;
            case 'n':
              if (strncmp (at, "namespace", 9) == 0 && (end-at) == 9)
                type = TOKEN_NAMESPACE;
              break;
            }
          ADD_TOKEN_ADVANCE (type, end - at);
        }
      else if (*at == '.')
        ADD_TOKEN_ADVANCE (TOKEN_DOT, 1);
      else if (*at == ';')
        ADD_TOKEN_ADVANCE (TOKEN_SEMICOLON, 1);
      else if (*at == '{')
        ADD_TOKEN_ADVANCE (TOKEN_LBRACE, 1);
      else if (*at == '}')
        ADD_TOKEN_ADVANCE (TOKEN_RBRACE, 1);
      else if (*at == '!')
        ADD_TOKEN_ADVANCE (TOKEN_EXCLAMATION_POINT, 1);
      else if (*at == '?')
        ADD_TOKEN_ADVANCE (TOKEN_QUESTION_MARK, 1);
      else if (*at == '*')
        ADD_TOKEN_ADVANCE (TOKEN_ASTERISK, 1);
      else if (*at == '+')
        ADD_TOKEN_ADVANCE (TOKEN_PLUS, 1);
      else if (*at == ':')
        ADD_TOKEN_ADVANCE (TOKEN_COLON, 1);
      else if (*at == ' ' || *at == '\t')
        at++;
      else if (*at == '\n')
        {
          at++;
          line_no++;
        }
      else
        {
          dsk_set_error (error, "unexpected character %s (%s:%u)",
                         dsk_ascii_byte_name (*at),
                         filename, line_no);
          dsk_free (rv);
          return NULL;
        }
    }

  /* place sentinel */
  ADD_TOKEN_ADVANCE (0, 0);
  n--;

  *n_tokens_out = n;
  return rv;
}

typedef struct _UseStatement UseStatement;
typedef struct
{
  /* the name-space currently under construction */
  DskXmlBindingNamespace *ns;

  const char *str;
  unsigned    n_tokens;
  Token      *tokens;
  const char *filename;         /* for error reporting */
  DskXmlBinding *binding;

  unsigned       n_types;
  DskXmlBindingType **types;
  unsigned types_alloced;

  unsigned n_use_statements;
  UseStatement *use_statements;
} ParseContext;

typedef struct 
{
  DskXmlBindingType *type;
  unsigned line_no;
  unsigned orig_index;
} NewTypeInfo;

struct _UseStatement
{
  DskXmlBindingNamespace *ns;
  char *as;
};

static char *
make_token_string (ParseContext *context,
                   unsigned      idx)
{
  char *rv;
  dsk_assert (idx < context->n_tokens);
  rv = dsk_malloc (context->tokens[idx].length + 1);
  memcpy (rv, context->str + context->tokens[idx].start,
          context->tokens[idx].length);
  rv[context->tokens[idx].length] = 0;
  return rv;
}

static char *
parse_dotted_bareword (ParseContext *context,
                       unsigned      start,
                       unsigned     *tokens_used_out,
                       DskError    **error)
{
  unsigned comps;
  if (start >= context->n_tokens)
    {
      dsk_set_error (error, "end-of-file expecting bareword (%s:%u)",
                     context->filename, context->tokens[context->n_tokens].line_no);
      return NULL;
    }
  if (!token_type_is_word (context->tokens[start].type))
    {
      dsk_set_error (error, "expected bareword, got '%.*s' (%s:%u)",
                     (int) context->tokens[start].length,
                     context->str + context->tokens[start].start,
                     context->filename,
                     context->tokens[start].line_no);
      return NULL;
    }
  comps = 1;
  for (;;)
    {
      if (start + comps * 2 - 1 == context->n_tokens)
        goto done;
      if (context->tokens[start + comps * 2 - 1].type != TOKEN_DOT)
        goto done;
      if (start + comps * 2 == context->n_tokens
       || !token_type_is_word (context->tokens[start + comps * 2].type))
        {
          dsk_set_error (error, "expected bareword after '.', got '%.*s' (%s:%u)",
                         (int) context->tokens[start + comps*2].length,
                         context->str + context->tokens[start + comps*2].start,
                         context->filename,
                         context->tokens[start + comps*2].line_no);
          return NULL;
        }
      comps++;
    }

done:
  *tokens_used_out = comps * 2 - 1;
  {
    unsigned alloc_len = 0, i;
    char *rv, *at;
    for (i = 0; i < comps; i++)
      alloc_len += context->tokens[start + i * 2].length + 1;
    rv = dsk_malloc (alloc_len);
    at = rv;
    for (i = 0; i < comps; i++)
      {
        memcpy (at, context->str + context->tokens[start + i*2].start,
                context->tokens[start + i*2].length);
        at += context->tokens[start + i*2].length;
        *at++ = '.';
      }
    --at;
    *at = 0;
    return rv;
  }
}

static DskXmlBindingType *
dotted_bareword_to_type (ParseContext *context,
                         const char   *dotted_type_name,
                         unsigned      at,
                         DskError    **error)
{
  if (strchr (dotted_type_name, '.') == NULL)
    {
      /* just a name:  first lookup in current namespace,
         then look it up in 'use' statements without 'as' clauses. */
      unsigned i;
      DskXmlBindingType *rv = NULL;
      for (i = 0; i < context->n_types; i++)
        if (strcmp (dotted_type_name, context->types[i]->name) == 0)
          return context->types[i];
      rv = dsk_xml_binding_namespace_lookup (context->ns, dotted_type_name);
      if (rv)
        return rv;
      for (i = 0; i < context->n_use_statements; i++)
        if (context->use_statements[i].as == NULL)
          {
            DskXmlBindingNamespace *as_ns = context->use_statements[i].ns;
            rv = dsk_xml_binding_namespace_lookup (as_ns, dotted_type_name);
            if (rv != NULL)
              return rv;
          }
      dsk_set_error (error, "type '%s' not found (%s:%u)", dotted_type_name,
                     context->filename, context->tokens[at].line_no);
      return NULL;
    }
  else
    {
      const char *last_dot = strrchr (dotted_type_name, '.');
      unsigned ns_i;
      DskXmlBindingType *rv;
      if (strchr (dotted_type_name, '.') != last_dot)
        {
          dsk_set_error (error, "namespace qualifier must be a simple bareword (got %.*s) (%s:%u)",
                         (int)(last_dot - dotted_type_name), dotted_type_name,
                         context->filename, context->tokens[at].line_no);
          return NULL;
        }

      /* look up namespace in 'as'-qualified 'use'-statements */
      for (ns_i = 0; ns_i < context->n_use_statements; ns_i++)
        if (context->use_statements[ns_i].as != NULL
         && memcmp (context->use_statements[ns_i].as,
                    dotted_type_name, last_dot - dotted_type_name) == 0)
          break;
      if (ns_i == context->n_use_statements)
        {
          dsk_set_error (error, "namespace qualifier (got %.*s) not found in as-clause of any use-statement (%s:%u)",
                         (int)(last_dot - dotted_type_name), dotted_type_name,
                         context->filename, context->tokens[at].line_no);
          return NULL;
        }
      rv = dsk_xml_binding_namespace_lookup (context->use_statements[ns_i].ns,
                                             last_dot + 1);
      if (rv == NULL)
        {
          dsk_set_error (error, "no %s found in namespace %s (%s:%u)",
                         last_dot+1, context->use_statements[ns_i].ns->name,
                         context->filename, context->tokens[at].line_no);
          return NULL;
        }
      else
        return rv;
    }
}
static DskXmlBindingType *
parse_type (ParseContext *context,
            unsigned     *at_inout,
            DskError    **error)
{
  unsigned used;
  unsigned start = *at_inout;
  DskXmlBindingType *type;
  char *bw = parse_dotted_bareword (context, start, &used, error);
  if (bw == NULL)
    return NULL;
  type = dotted_bareword_to_type (context, bw, start, error);
  dsk_free (bw);
  *at_inout += used;
  return type;
}
static int
find_matching_rbrace (ParseContext *context,
                      unsigned      at)
{
  unsigned lbrace_depth = 1;
  at++;
  while (at < context->n_tokens)
    {
      if (context->tokens[at].type == TOKEN_LBRACE)
        lbrace_depth++;
      else if (context->tokens[at].type == TOKEN_RBRACE)
        {
          if (lbrace_depth == 1)
            return at;
          lbrace_depth--;
        }
      at++;
    }
  return -1;
}

static dsk_boolean
parse_member_list (ParseContext *context,
                   unsigned      lbrace_index,
                   unsigned     *n_tokens_used_out,
                   unsigned     *n_members_out,
                   DskXmlBindingStructMember **members_out,
                   DskError    **error)
{
  unsigned at = lbrace_index;
  int matching_rbrace;
  int members_n_tokens;
  unsigned max_members;
  unsigned n_members;
  DskXmlBindingStructMember *members;
  if (context->tokens[at].type != TOKEN_LBRACE)
    {
      dsk_set_error (error, "expected '"LBRACE_STR"' after struct NAME (%s:%u)",
                     context->filename, context->tokens[at].line_no);
      return DSK_FALSE;
    }

  /* maximum possible members a set of type/name pairs */
  matching_rbrace = find_matching_rbrace (context, lbrace_index);
  if (matching_rbrace < 0)
    {
      dsk_set_error (error, "missing '"RBRACE_STR"' for '"LBRACE_STR"' at %s:%u",
                     context->filename, context->tokens[lbrace_index].line_no);
      return DSK_FALSE;
    }
  members_n_tokens = matching_rbrace - (lbrace_index+1);
  max_members = members_n_tokens / 2;

  ++at;         /* skip left-brace */

  /* parse each member */
  n_members = 0;
  members = DSK_NEW_ARRAY (max_members, DskXmlBindingStructMember);
  while (at < context->n_tokens
      && context->tokens[at].type != TOKEN_RBRACE)
    {
      DskXmlBindingStructMember member;
      if (context->tokens[at].type == TOKEN_SEMICOLON)
        {
          at++;
          continue;
        }

      /* parse type */
      member.type = parse_type (context, &at, error);
      if (member.type == NULL)
        goto got_error;

      /* parse quantity characters: * ? ! + */
      if (at == context->n_tokens)
        {
          dsk_set_error (error, "too few tokens for structure member (%s:%u)",
                         context->filename, context->tokens[at].line_no);
          goto got_error;
        }
      switch (context->tokens[at].type)
        {
        case TOKEN_EXCLAMATION_POINT:
          member.quantity = DSK_XML_BINDING_REQUIRED;
          break;
        case TOKEN_QUESTION_MARK:
          member.quantity = DSK_XML_BINDING_OPTIONAL;
          break;
        case TOKEN_ASTERISK:
          member.quantity = DSK_XML_BINDING_REPEATED;
          break;
        case TOKEN_PLUS:
          member.quantity = DSK_XML_BINDING_REQUIRED_REPEATED;
          break;
        default:
          dsk_set_error (error, "expected '+', '!', '*' or '?', got %.*s at (%s:%u)",
                         (int)context->tokens[at].length,
                         context->str + context->tokens[at].start,
                         context->filename,
                         context->tokens[at].line_no);
          goto got_error;
        }
      at++;

      /* parse name */
      if (token_type_is_word (context->tokens[at].type))
        {
          unsigned length = context->tokens[at].length;
          char *name = dsk_malloc (length + 1);
          member.name = name;
          memcpy (name, context->str + context->tokens[at].start, length);
          name[length] = 0;
        }
      else
        {
          dsk_set_error (error, "expected indentifier at %s:%u (got '%.*s')",
                         context->filename, context->tokens[at].line_no,
                         (int)context->tokens[at].length,
                         context->str + context->tokens[at].start);
          goto got_error;
        }
      at++;

      /* add member */
      members[n_members++] = member;
    }
  if (at == context->n_tokens)
    {
      dsk_set_error (error, "unexpected EOF, looking for end of structure members starting %s:%u",
                     context->filename, context->tokens[lbrace_index].line_no);
      goto got_error;
    }
  *n_tokens_used_out = at + 1 - lbrace_index;
  *members_out = members;
  *n_members_out = n_members;
  return DSK_TRUE;

got_error:
  {
    unsigned i;
    for (i = 0; i < n_members; i++)
      dsk_free ((void*)(members[i].name));
  }
  dsk_free (members);
  return DSK_FALSE;
}

static dsk_boolean
parse_case_list (ParseContext           *context,
                 unsigned                lbrace_index,
                 unsigned               *n_tokens_used_out,
                 unsigned               *n_cases_out,
                 DskXmlBindingUnionCase**cases_out,
                 DskError              **error)
{
  unsigned at = lbrace_index;
  int matching_rbrace;
  unsigned cases_tokens;
  unsigned max_cases;
  unsigned n_cases = 0;
  DskXmlBindingUnionCase *cases;
  if (context->tokens[at].type != TOKEN_LBRACE)
    {
      dsk_set_error (error, "expected '"LBRACE_STR"' after union NAME (%s:%u)",
                     context->filename, context->tokens[at].line_no);
      return DSK_FALSE;
    }
  at++;

  matching_rbrace = find_matching_rbrace (context, lbrace_index);
  if (matching_rbrace < 0)
    {
      dsk_set_error (error, "missing '"RBRACE_STR"' for '"LBRACE_STR"' at %s:%u",
                     context->filename, context->tokens[lbrace_index].line_no);
      return DSK_FALSE;
    }
  cases_tokens = matching_rbrace - (lbrace_index+1);
  max_cases = cases_tokens / 2;
  cases = DSK_NEW_ARRAY (max_cases, DskXmlBindingUnionCase);

  while (at < context->n_tokens
      && context->tokens[at].type != TOKEN_RBRACE)
    {
      DskXmlBindingUnionCase cas;
      if (context->tokens[at].type == TOKEN_SEMICOLON)
        {
          at++;
          continue;
        }
      if (at + 2 >= context->n_tokens)
        {
          dsk_set_error (error, "too few tokens in case (%s:%u)",
                         context->filename, context->tokens[at].line_no);
          goto got_error;
        }
      if (!token_type_is_word (context->tokens[at].type))
        {
          dsk_set_error (error, "expected bareword for case label, got %.*s (%s:%u)",
                         (int) context->tokens[at].length,
                         context->str + context->tokens[at].start,
                         context->filename,
                         context->tokens[at].line_no);
          goto got_error;
        }
      if (context->tokens[at+1].type != TOKEN_COLON)
        {
          dsk_set_error (error, "expected ':' for case label, got %.*s (%s:%u)",
                         (int) context->tokens[at+1].length,
                         context->str + context->tokens[at+1].start,
                         context->filename,
                         context->tokens[at+1].line_no);
          goto got_error;
        }
      cas.name = dsk_malloc (context->tokens[at].length + 1);
      memcpy (cas.name, context->str + context->tokens[at].start,
              context->tokens[at].length);
      cas.name[context->tokens[at].length] = 0;
      cas.elide_struct_outer_tag = DSK_FALSE;
      if (at + 2 == context->n_tokens
       || context->tokens[at+2].type == TOKEN_SEMICOLON)
        {
          /* void case */
          cas.type = NULL;
        }
      else if (context->tokens[at+2].type == TOKEN_LBRACE)
        {
          /* parse member list into anon structure */
          unsigned tokens_used;
          unsigned n_members;
          DskXmlBindingStructMember *members;
          DskXmlBindingTypeStruct *stype;
          if (!parse_member_list (context, at + 2, &tokens_used,
                                  &n_members, &members, error))
            {
              dsk_add_error_prefix (error, "parsing case %s", cas.name);
              dsk_free (cas.name);
              goto got_error;
            }
          stype = dsk_xml_binding_type_struct_new (NULL, NULL,
                                                   n_members, members,
                                                   error);
          cas.type = (DskXmlBindingType*) stype;
          cas.elide_struct_outer_tag = DSK_TRUE;
          if (cas.type == NULL)
            {
              dsk_add_error_prefix (error, "parsing case %s", cas.name);
              dsk_free (cas.name);
              goto got_error;
            }
          at += 2 + tokens_used;
        }
      else
        {
          /* parse type */
          at += 2;
          cas.type = parse_type (context, &at, error);
          if (cas.type == NULL)
            goto got_error;
        }
      cases[n_cases++] = cas;
    }
  if (at == context->n_tokens)
    {
      dsk_set_error (error, "missing '"RBRACE_STR"' for '"LBRACE_STR"' at %s:%u",
                     context->filename, context->tokens[lbrace_index].line_no);
      goto got_error;
    }
  *n_tokens_used_out = at + 1 - lbrace_index;
  *n_cases_out = n_cases;
  *cases_out = cases;
  return DSK_TRUE;

got_error:
  dsk_free (cases);
  return DSK_FALSE;
}

static void
free_cases (unsigned n_cases, DskXmlBindingUnionCase *cases)
{
  unsigned i;
  for (i = 0; i < n_cases; i++)
    dsk_free (cases[i].name);
  dsk_free (cases);
}

static int compare_new_type_info_by_name (const void *a, const void *b)
{
  const NewTypeInfo *A = a;
  const NewTypeInfo *B = b;
  return strcmp (A->type->name, B->type->name);
}

static dsk_boolean
parse_file (ParseContext *context,
            DskError  **error)
{
  unsigned n_tokens = context->n_tokens;
  Token *tokens = context->tokens;
  unsigned n_tokens_used;
  char *ns_name;
  unsigned at;
  unsigned n_ns_types = 0;
  NewTypeInfo *ns_type_info = NULL;

  if (n_tokens == 0 || tokens[0].type != TOKEN_NAMESPACE)
    {
      dsk_set_error (error,
                     "file must begin with 'namespace' (%s:%u)",
                     context->filename, tokens[0].line_no);
      return DSK_FALSE;
    }

  ns_name = parse_dotted_bareword (context, 1, &n_tokens_used, error);
  if (ns_name == NULL)
    return DSK_FALSE;
  at = 1 + n_tokens_used;
  if (at == n_tokens || tokens[at].type != TOKEN_SEMICOLON)
    {
      dsk_set_error (error,
                     "expected ; after namespace %s (%s:%u)",
                     ns_name, context->filename, tokens[at].line_no);
      dsk_free (ns_name);
      return DSK_FALSE;
    }
  at++;         /* skip semicolon */


  /* parse 'use' statements */
  while (at < n_tokens)
    {
      if (tokens[at].type == TOKEN_USE)
        {
          char *use;
          UseStatement stmt;
          unsigned use_at = at;
          use = parse_dotted_bareword (context, at + 1, &n_tokens_used, error);
          if (use == NULL)
            goto error_cleanup;
          at += 1 + n_tokens_used;
          stmt.ns = dsk_xml_binding_get_ns (context->binding, use, error);
          if (stmt.ns == NULL)
            {
              /* append suffix */
              dsk_add_error_prefix (error, "in %s, line %u:\n\t",
                                    context->filename, 
                                    context->tokens[use_at].line_no);
              dsk_free (use);
              goto error_cleanup;
            }
          if (at < n_tokens && tokens[at].type == TOKEN_AS)
            {
              if (at + 1 == n_tokens || !token_type_is_word (tokens[at+1].type))
                {
                  dsk_set_error (error,
                                 "error parsing shortname from 'as' clause (%s:%u)",
                                 context->filename, tokens[at+1].line_no);
                  dsk_free (use);
                  goto error_cleanup;
                }
              stmt.as = make_token_string (context, at + 1);
              at += 2;
            }
          else
            stmt.as = NULL;
          if (at >= n_tokens || tokens[at].type != TOKEN_SEMICOLON)
            {
              /* missing semicolon */
              dsk_set_error (error,
                             "expected ; after use %s (%s:%u)",
                             use, context->filename,
                             tokens[at].line_no);
              dsk_free (use);
              goto error_cleanup;
            }

          at++;         /* skip ; */
          dsk_free (use);

          /* store into namespace array */
          if ((context->n_use_statements & (context->n_use_statements-1)) == 0)
            {
              /* resize */
              unsigned new_size = context->n_use_statements ? context->n_use_statements*2 : 1;
              unsigned new_size_bytes = new_size * sizeof (UseStatement);
              context->use_statements = dsk_realloc (context->use_statements, new_size_bytes);
            }
          context->use_statements[context->n_use_statements++] = stmt;
        }
      else if (tokens[at].type != TOKEN_STRUCT
            && tokens[at].type != TOKEN_UNION)
        {
          dsk_set_error (error,
                         "expected 'use', 'struct' or 'union', got '%.*s' (%s:%u)",
                         (int) tokens[at].length,
                         context->str + tokens[at].start,
                         context->filename, tokens[at].line_no);
          return DSK_FALSE;
        }
      else
        break;          /* fall-through to struct/union handling */
    }

  /* parse type-decls */
  while (at < n_tokens)
    {
      dsk_boolean has_type_info = DSK_FALSE;
      NewTypeInfo type_info;
      type_info.line_no = tokens[at].line_no;
      type_info.orig_index = n_ns_types;

      if (tokens[at].type == TOKEN_STRUCT)
        {
          char *struct_name;
          unsigned n_members;
          DskXmlBindingStructMember *members;
          DskXmlBindingTypeStruct *stype;
          if (at + 1 == n_tokens || !token_type_is_word (tokens[at+1].type))
            {
              dsk_set_error (error, "error parsing name of struct (%s:%u)",
                             context->filename, tokens[at+1].line_no);
              goto error_cleanup;
            }
          if (!parse_member_list (context, at + 2, &n_tokens_used,
                                  &n_members, &members, error))
            goto error_cleanup;
          struct_name = make_token_string (context, at+1);
          stype = dsk_xml_binding_type_struct_new (context->ns,
                                                  struct_name,
                                                  n_members, members,
                                                  error);
          dsk_free (struct_name);
          {
            unsigned m;
            for (m = 0; m < n_members; m++)
              dsk_free (members[m].name);
            dsk_free (members);
          }

          if (stype == NULL)
            goto error_cleanup;

          /* add to array of types */
          has_type_info = DSK_TRUE;
          type_info.type = (DskXmlBindingType *) stype;
          at += 2 + n_tokens_used;
        }
      else if (tokens[at].type == TOKEN_UNION)
        {
          char *union_name;
          unsigned n_cases;
          DskXmlBindingUnionCase *cases;
          DskXmlBindingTypeUnion *utype;
          DskXmlBindingType *type;
          if (at + 1 == n_tokens || !token_type_is_word (tokens[at+1].type))
            {
              dsk_set_error (error, "error parsing name of union (%s:%u)",
                             context->filename, tokens[at+1].line_no);
              goto error_cleanup;
            }
          if (!parse_case_list (context, at + 2, &n_tokens_used,
                                &n_cases, &cases, error))
            {
              goto error_cleanup;
            }
          union_name = make_token_string (context, at+1);
          utype = dsk_xml_binding_type_union_new (context->ns, union_name, n_cases, cases, error);
          type = (DskXmlBindingType *) utype;
          dsk_free (union_name);
          free_cases (n_cases, cases);

          if (type == NULL)
            goto error_cleanup;

          /* add to array of types */
          type_info.type = type;
          has_type_info = DSK_TRUE;
          at += 2 + n_tokens_used;
        }
      else if (tokens[at].type == TOKEN_SEMICOLON)
        {
          at++;
        }
      else
        {
          dsk_set_error (error,
                         "unexpected token '%.*s' (%s:%u) - expected struct or union",
                         (int) tokens[at].length,
                         context->str + tokens[at].start,
                         context->filename, tokens[at].line_no);
          goto error_cleanup;
        }

      /* Append type-info to array, if needed */
      if (has_type_info)
        {
          if ((n_ns_types & (n_ns_types-1)) == 0)
            {
              /* resize */
              unsigned new_size = n_ns_types ? n_ns_types*2 : 1;
              unsigned new_size_bytes = new_size * sizeof (NewTypeInfo);
              ns_type_info = dsk_realloc (ns_type_info, new_size_bytes);
            }
          ns_type_info[n_ns_types++] = type_info;
        }
    }

  qsort (ns_type_info, n_ns_types, sizeof (NewTypeInfo),
         compare_new_type_info_by_name);

  {
    unsigned i;
    for (i = 1; i < n_ns_types; i++)
      if (strcmp (ns_type_info[i-1].type->name, ns_type_info[i].type->name) == 0)
        {
          dsk_set_error (error, "namespace %s defined type %s twice (at least) (%s:%u and %s:%u)",
                         ns_name, ns_type_info[i].type->name,
                         context->filename, ns_type_info[i-1].line_no,
                         context->filename, ns_type_info[i].line_no);
          goto error_cleanup;
        }
  }
  dsk_free (ns_type_info);
  dsk_free (ns_name);
  return DSK_TRUE;

error_cleanup:
  {
    unsigned i;
    dsk_free (ns_name);
    /* note: the various ns_type_info[i].type are owned by the namespace */
    dsk_free (ns_type_info);
    for (i = 0; i < context->n_use_statements; i++)
      dsk_free (context->use_statements[i].as);
    dsk_free (context->use_statements);
    context->n_use_statements = 0;
    context->use_statements = NULL;
  }
  return DSK_FALSE;
}

DskXmlBindingNamespace *
_dsk_xml_binding_parse_ns_str (DskXmlBinding *binding, 
                               const char    *filename,
                               const char    *namespace_name,
                               const char    *contents,
                               DskError     **error)
{
  ParseContext context;
  unsigned i;
  memset (&context, 0, sizeof (context));

  context.tokens = tokenize (filename, contents, &context.n_tokens, error);
  if (context.tokens == NULL)
    return NULL;
  context.binding = binding;
  context.ns = dsk_xml_binding_namespace_new (namespace_name);
  context.str = contents;
  context.filename = filename;
  context.n_use_statements = 1;
  context.use_statements = DSK_NEW (UseStatement);
  context.use_statements[0].ns = &dsk_xml_binding_namespace_builtin;
  context.use_statements[0].as = NULL;

  if (!parse_file (&context, error))
    {
      dsk_xml_binding_namespace_unref (context.ns);
      context.ns = NULL;
    }
  dsk_free (context.tokens);
  for (i = 0; i < context.n_use_statements; i++)
    dsk_free (context.use_statements[i].as);
  dsk_free (context.use_statements);
  return context.ns;
}
