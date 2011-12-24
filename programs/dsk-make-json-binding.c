/* TODO: 
   - from_json() should take the object as an argument, return boolean
   - use dsk_print() instead of stdio
 */

#include <stdio.h>
#include <string.h>
#include "../dsk.h"
#include "../dsk-rbtree-macros.h"

#define IS_TOKEN  DSK_CTOKEN_IS


typedef struct _JsonMember JsonMember;
typedef struct _JsonType JsonType;

char *cmdline_output = NULL;
char *cmdline_input = NULL;

/* if all the following, the default is blank.  */
char *namespace_func_prefix;       // eg "foo__"
char *namespace_struct_prefix;     // eg "Foo_"
char *namespace_enum_prefix;       // eg "FOO__"

typedef enum
{
  JSON_METATYPE_INT,
  JSON_METATYPE_BOOLEAN,
  JSON_METATYPE_NUMBER,
  JSON_METATYPE_STRING,
  JSON_METATYPE_JSON,
  JSON_METATYPE_UNION,
  JSON_METATYPE_STRUCT,
  JSON_METATYPE_STUB
} JsonMetatype;

typedef enum
{
  JSON_MEMBER_TYPE_DEFAULT,
  JSON_MEMBER_TYPE_MAP,
  JSON_MEMBER_TYPE_ARRAY,
  JSON_MEMBER_TYPE_OPTIONAL
} JsonMemberType;

#if 0
typedef struct _Containment Containment;
struct _Containment
{
  JsonType *container;
  JsonType *contained;
  JsonType *contained_via;
  unsigned iteration;
  Containment *next_in_contained;
};
#endif

struct _JsonMember
{
  char *name;
  JsonMemberType member_type;
  JsonType *value_type;

  char *uc_name;                        /* only needed for unions */
};

struct _JsonType
{
  char *name;
  JsonMetatype metatype;

  char *c_name;                 /* name for use in C code */
  char *uc_name;                /* upper-case, like for constructing enums */
  char *lc_name;                /* lower-case, for constructing function names */
  char *cc_name;                /* mixed-case, w/o namespace */

  unsigned n_members;		/* for union, struct */
  JsonMember *members;

  JsonType *left, *right, *parent;
  dsk_boolean is_red;

  dsk_boolean used_as_map_member;
  unsigned def_line_no;

  // Used while computing the order in which to present types.
  dsk_boolean in_ordered_set, computing_order;
  JsonType *tmp_container;
};

#define JSON_TYPE_TREE_COMPARE(a,b,rv) rv = strcmp(a->name, b->name)
#define JSON_TYPE_TREE_KEY_COMPARE(a,b,rv) rv = strcmp(a, b->name)
#define GET_JSON_TYPE_TREE() \
  tree_top, JsonType *, DSK_STD_GET_IS_RED, DSK_STD_SET_IS_RED, \
  parent, left, right, \
  JSON_TYPE_TREE_COMPARE
JsonType *tree_top = NULL;
unsigned n_types = 0;
JsonType **types;
unsigned n_types_ordered = 0;

static JsonType *
force_type_by_name (const char *name, const char *cname)
{
  JsonType *type;
  DSK_RBTREE_LOOKUP_COMPARATOR(GET_JSON_TYPE_TREE(), name, JSON_TYPE_TREE_KEY_COMPARE, type);
  if (type != NULL)
    return type;
  type = dsk_malloc0 (sizeof (JsonType));
  type->name = dsk_strdup (name);
  type->metatype = JSON_METATYPE_STUB;
  type->cc_name = dsk_codegen_mixedcase_normalize (type->name);
  type->uc_name = dsk_codegen_mixedcase_to_uppercase (type->cc_name);
  type->lc_name = dsk_codegen_mixedcase_to_lowercase (type->cc_name);
  if (cname)
    type->c_name = dsk_strdup (cname);
  else
    type->c_name = dsk_strdup_printf ("%s%s", namespace_struct_prefix, type->cc_name);
  JsonType *conflict;
  DSK_RBTREE_INSERT (GET_JSON_TYPE_TREE (), type, conflict);
  dsk_assert (conflict == NULL);
  n_types++;
  return type;
}

static void
parse_member_list (JsonMetatype metatype,
                   const char *name,
                   const char *contents,
                   unsigned    n_tokens,
                   DskCToken  *tokens,
                   unsigned   *n_members_out,
                   JsonMember**members_out)
{
  /* parse type */
  unsigned n_members = 0;
  JsonMember *members = NULL;
  const char *metatype_name = metatype == JSON_METATYPE_STRUCT ? "struct" :"union";
  while (n_tokens > 0)
    {
      if (n_tokens < 3)
        {
          dsk_die ("file too short in %s %s, at %s:%u",
                   metatype_name, name,
                   cmdline_input, tokens[n_tokens-1].start_line);
        }
      char *type_name = dsk_strndup (tokens[0].n_bytes,
                                     contents + tokens[0].start_byte);
      if (tokens[0].type != DSK_CTOKEN_TYPE_BAREWORD)
        {
          dsk_die ("expected type-name, got '%s', in %s %s, at %s:%u",
                   type_name,
                   metatype_name, name,
                   cmdline_input, tokens[0].start_line);
        }
      JsonMember member;
      if (strcmp (type_name, "void") == 0)
        {
          if (metatype == JSON_METATYPE_UNION)
            {
              member.value_type = NULL;
            }
          else
            {
              dsk_die ("expected type-name, got 'void', in %s %s, at %s:%u",
                       metatype_name, name,
                       cmdline_input, tokens[0].start_line);
            }
        }
      else
        member.value_type = force_type_by_name (type_name, NULL);
      unsigned at;
      if (metatype == JSON_METATYPE_STRUCT)
        {
          if (tokens[1].type == DSK_CTOKEN_TYPE_BRACE)
            {
              if (tokens[1].n_children != 0)
                dsk_die ("expected {} to be empty, in %s %s, at %s:%u",
                         metatype_name, name,
                         cmdline_input, tokens[1].start_line);
              at = 2;
              member.member_type = JSON_MEMBER_TYPE_MAP;
              member.value_type->used_as_map_member = 1;
            }
          else if (tokens[1].type == DSK_CTOKEN_TYPE_BRACKET)
            {
              if (tokens[1].n_children != 0)
                dsk_die ("expected [] to be empty, in %s %s, at %s:%u",
                         metatype_name, name,
                         cmdline_input, tokens[1].start_line);
              at = 2;
              member.member_type = JSON_MEMBER_TYPE_ARRAY;
            }
          else if (tokens[1].type == DSK_CTOKEN_TYPE_OPERATOR
               &&  tokens[1].n_bytes == 1
               &&  contents[tokens[1].start_byte] == '?')
            {
              at = 2;
              member.member_type = JSON_MEMBER_TYPE_OPTIONAL;
            }
          else
            {
              at = 1;
              member.member_type = JSON_MEMBER_TYPE_DEFAULT;
            }
        }
      else
        {
          at = 1;
          member.member_type = JSON_MEMBER_TYPE_DEFAULT;
        }
      if (tokens[at].type != DSK_CTOKEN_TYPE_BAREWORD)
        {
          dsk_die ("unexpected token after member's type: %.*s, in %s %s, at %s:%u",
                   tokens[1].n_bytes, contents + tokens[1].start_byte,
                   metatype_name, name,
                   cmdline_input, tokens[1].start_line);
        }
      member.name = dsk_strndup (tokens[at].n_bytes,
                                 contents + tokens[at].start_byte);
      at++;
      if (at == n_tokens)
        {
          dsk_die ("file too short in %s %s, at %s:%u",
                   metatype_name, name,
                   cmdline_input, tokens[at-1].start_line);
        }
      if (tokens[at].type != DSK_CTOKEN_TYPE_OPERATOR
       || tokens[at].n_bytes != 1
       || contents[tokens[at].start_byte] != ';')
        {
          dsk_die ("expected ';' got '%.*s', in %s %s, at %s:%u",
                   tokens[at].n_bytes, contents + tokens[at].start_byte,
                   metatype_name, name,
                   cmdline_input, tokens[at].start_line);
        }
      at++;

      members = dsk_realloc (members, sizeof (JsonMember) * (n_members+1));
      members[n_members++] = member;

      n_tokens -= at;
      tokens += at;
    }
  if (metatype == JSON_METATYPE_UNION)
    {
      unsigned m;
      for (m = 0; m < n_members; m++)
        {
          members[m].uc_name = dsk_strdup (members[m].name);
          dsk_ascii_strup (members[m].uc_name);
        }
    }

  *n_members_out = n_members;
  *members_out = members;
}

#if 0
static dsk_boolean
add_containment (JsonType *container,
                 JsonType *contained,
                 JsonType *via,
                 unsigned  iteration)
{
  Containment *c;
  for (c = container->contained_type_list; c; c = c->next_in_contained)
    if (c->contained == contained)
      return DSK_FALSE;
  c = dsk_malloc (sizeof (Containment));
  c->next_in_contained = container->contained_type_list;
  container->contained_type_list = c;
  c->container = container;
  c->contained = contained;
  c->contained_via = via;
  c->iteration = iteration;
  return DSK_TRUE;
}
#endif


static dsk_boolean
foreach_recursive (JsonType *at, dsk_boolean (*func)(JsonType *cb))
{
  if (at == NULL)
    return DSK_FALSE;
  dsk_boolean rv = foreach_recursive (at->left, func);
  if (func (at))
    rv = DSK_TRUE;
  if (foreach_recursive (at->right, func))
    rv = DSK_TRUE;
  return rv;
}
static dsk_boolean
foreach (dsk_boolean (*func)(JsonType *cb))
{
  return foreach_recursive (tree_top, func);
}

static dsk_boolean
check_for_stubs (JsonType *type)
{
  if (type->metatype == JSON_METATYPE_STUB)
    dsk_die ("type %s used but not defined", type->name);
  return DSK_FALSE;
}

#if 0
static dsk_boolean
init_containments (JsonType *type)
{
  unsigned i;
  for (i = 0; i < type->n_members; i++)
    {
      if (type->members[i].member_type == JSON_MEMBER_TYPE_DEFAULT)
        {
          add_containment (type,
                           type->members[i].value_type,
                           type->members[i].value_type,
                           0);
        }
    }
  return DSK_FALSE;
}

unsigned expand_containments_iteration = 0;
static dsk_boolean
expand_containments (JsonType *type)
{
  Containment *c1, *c2;
  dsk_boolean rv = DSK_FALSE;
  for (c1 = type->contained_type_list; c1; c1 = c1->next_in_contained)
    for (c2 = c1->contained->contained_type_list;
         c2 && c2->iteration == expand_containments_iteration;
         c2 = c2->next_in_contained)
      if (add_containment (type,
                           c2->contained,
                           c1->contained,
                           expand_containments_iteration + 1))
        rv = DSK_TRUE;
  return rv;
}
static dsk_boolean
check_for_self_containment (JsonType *type)
{
  Containment *c;
  for (c = type->contained_type_list; c; c = c->next_in_contained)
    if (c->contained == type)
      {
        /* Find containment chain */
        ...

        dsk_die ("type %s contains itself (via %s)", type->name, chain);
      }
  return DSK_FALSE;
}
#endif

static dsk_boolean
compute_type_order (JsonType *type)
{
  if (type->in_ordered_set)
    return DSK_FALSE;
  if (type->computing_order)
    {
      DskBuffer list = DSK_BUFFER_INIT;
      dsk_buffer_append_string (&list, type->name);
      JsonType *at = type;
      do
        {
          at = at->tmp_container;
          dsk_buffer_printf (&list, " is contained in '%s'", type->name);
        }
      while (at != type);
      dsk_die ("cannot fully order types in file: %s",
               dsk_buffer_empty_to_string (&list));
    }
  type->computing_order = DSK_TRUE;
  unsigned i;
  for (i = 0; i < type->n_members; i++)
    if (type->members[i].member_type == JSON_MEMBER_TYPE_DEFAULT
     && type->members[i].value_type != NULL)
      {
        type->members[i].value_type->tmp_container = type;
        compute_type_order (type->members[i].value_type);
      }
  dsk_warning ("types[%u] = %s", n_types_ordered, type->name);
  types[n_types_ordered++] = type;
  type->computing_order = DSK_FALSE;
  type->in_ordered_set = DSK_TRUE;
  return DSK_FALSE;
}



static void
implement_to_json_by_type   (FILE *fp,
                             JsonType *type,
                             unsigned indent,
                             const char *input_name,
                             const char *output_name)
{
  switch (type->metatype)
    {
    case JSON_METATYPE_INT:
    case JSON_METATYPE_NUMBER:
      fprintf (fp, "%*s%s = dsk_json_value_new_number (%s);\n",
               indent, "", output_name, input_name);
      break;
    case JSON_METATYPE_BOOLEAN:
      fprintf (fp, "%*s%s = dsk_json_value_new_boolean (%s);\n",
               indent, "", output_name, input_name);
      break;
    case JSON_METATYPE_STRING:
      fprintf (fp, "%*s%s = dsk_json_value_new_string (strlen (%s), %s);\n",
               indent, "", output_name, input_name, input_name);
      break;
    case JSON_METATYPE_JSON:
      fprintf (fp, "%*s%s = dsk_json_value_copy (%s);\n",
               indent, "", output_name, input_name);
      break;

      /* STRUCT and UNION are handled by other generated functions */
    case JSON_METATYPE_STRUCT:
    case JSON_METATYPE_UNION:
      fprintf (fp, "%*s%s = %s%s__to_json (&(%s));\n",
               indent, "", output_name,
               namespace_func_prefix, type->lc_name, input_name);
      break;
    case JSON_METATYPE_STUB:
      dsk_assert_not_reached ();

    }
}

static void
implement_from_json_by_type (FILE *fp,
                             JsonType *type,
                             unsigned indent,
                             const char *input_name,
                             const char *output_name)
{
  const char *req_type_enumname, *req_type_shortname;
  switch (type->metatype)
    {
    case JSON_METATYPE_INT:
    case JSON_METATYPE_NUMBER:
      req_type_enumname = "DSK_JSON_VALUE_NUMBER";
      req_type_shortname = "number";
      break;
    case JSON_METATYPE_STRING:
      req_type_enumname = "DSK_JSON_VALUE_STRING";
      req_type_shortname = "string";
      break;
    case JSON_METATYPE_BOOLEAN:
      req_type_enumname = "DSK_JSON_VALUE_BOOLEAN";
      req_type_shortname = "boolean";
      break;
    case JSON_METATYPE_STRUCT:
    case JSON_METATYPE_UNION:
      req_type_enumname = "DSK_JSON_VALUE_OBJECT";
      req_type_shortname = "object";
      break;
    case JSON_METATYPE_JSON:
      req_type_enumname = NULL;
      req_type_shortname = NULL;
      break;
    case JSON_METATYPE_STUB:
      dsk_assert_not_reached ();
    }

  if (req_type_shortname)
    {
      fprintf (fp, "%*sif (%s->type != %s)\n"
                   "%*s  {\n"
                   "%*s    dsk_set_error (error, \"not a %s\");\n"
                   "%*s    return DSK_FALSE;\n"
                   "%*s  }\n",
                   indent,"", input_name, req_type_enumname,
                   indent,"",
                   indent,"", req_type_shortname,
                   indent,"",
                   indent,"");
    }

  switch (type->metatype)
    {
    case JSON_METATYPE_INT:
    case JSON_METATYPE_NUMBER:
      fprintf (fp, "%*s%s = %s->v_number.value;\n",
               indent, "", output_name, input_name);
      break;
    case JSON_METATYPE_BOOLEAN:
      fprintf (fp, "%*s%s = %s->v_boolean.value;\n",
               indent, "", output_name, input_name);
      break;
    case JSON_METATYPE_STRING:
      fprintf (fp, "%*s%s = %s->v_string.str;\n",
               indent, "", output_name, input_name);
      break;

      /* STRUCT and UNION are handled by other generated functions */
    case JSON_METATYPE_STRUCT:
    case JSON_METATYPE_UNION:
      fprintf (fp, "%*sif (!%s%s__from_json (mem_pool, %s, &%s, error))\n"
                   "%*s  return DSK_FALSE;\n",         /*TODO: add_prefix to error */
               indent, "", namespace_func_prefix, type->lc_name, input_name, output_name,
               indent, "");
      break;
    case JSON_METATYPE_JSON:
      fprintf (fp, "%*s%s = %s;\n", 
               indent, "", output_name, input_name);
      break;
    case JSON_METATYPE_STUB:
      dsk_assert_not_reached ();

    }
}

int main(int argc, char **argv)
{
  DskCTokenScannerConfig config = DSK_CTOKEN_SCANNER_CONFIG_INIT;
  unsigned i, j;
  dsk_boolean use_stdout = DSK_FALSE;
  dsk_boolean one_file = DSK_FALSE;
  dsk_boolean include_dsk = DSK_TRUE;
  dsk_cmdline_init ("generate JSON bindings",
                    "Take a file with ...",
                    NULL,
                    0);
  dsk_cmdline_add_string ("input", "input specification file", "FILENAME",
                          DSK_CMDLINE_MANDATORY, &cmdline_input);
  dsk_cmdline_add_string ("output", "output basename file", "BASENAME",
                          DSK_CMDLINE_MANDATORY, &cmdline_output);
  dsk_cmdline_add_boolean ("stdout", "output generated code to a standard-output", NULL,
                          0, &use_stdout);
  dsk_cmdline_add_boolean ("one-file", "output generated code to a single file", NULL,
                          0, &one_file);
  dsk_cmdline_add_boolean ("no-include-dsk", "do not output #include for DSK", NULL,
                          DSK_CMDLINE_REVERSED, &include_dsk);
  dsk_cmdline_process_args (&argc, &argv);

  DskError *error = NULL;
  size_t len;
  char *contents = (char *)dsk_file_get_contents (cmdline_input, &len, &error);
  if (contents == NULL)
    dsk_die ("error reading input file: %s", error->message);
  config.error_filename = cmdline_input;

  DskCToken *token;
  token = dsk_ctoken_scan_str (contents, contents + len, &config, &error);
  if (token == NULL)
    dsk_die ("error scanning ctokens: %s", error->message);

  /* add fundamental types */
  {
    JsonType *fund_type;
    fund_type = force_type_by_name ("int", "int");
    fund_type->metatype = JSON_METATYPE_INT;
    fund_type->c_name = "int";
    fund_type = force_type_by_name ("boolean", "dsk_boolean");
    fund_type->metatype = JSON_METATYPE_BOOLEAN;
    fund_type->c_name = "dsk_boolean";
    fund_type = force_type_by_name ("number", "double");
    fund_type->metatype = JSON_METATYPE_NUMBER;
    fund_type->c_name = "double";
    fund_type = force_type_by_name ("string", "char *");
    fund_type->metatype = JSON_METATYPE_STRING;
    fund_type->c_name = "char *";
  }
  
  unsigned at = 0;
  if (IS_TOKEN (token->children + at, BAREWORD, "namespace"))
    {
      if (at + 1 == token->n_children)
        {
          dsk_die ("premature end-of-file after namespace %s:%u",
                   cmdline_input, token->children[token->n_children-1].start_line);
        }
      if (token->children[at+1].type != DSK_CTOKEN_TYPE_BAREWORD)
        {
          dsk_die ("expected bareword after namespace %s:%u",
                   cmdline_input, token->children[token->n_children-1].start_line);
        }
      unsigned first_bareword = at + 1;
      at += 2;
      while (at < token->n_children)
        {
          if (IS_TOKEN (token->children + at, OPERATOR, ";"))
            break;
          if (IS_TOKEN (token->children + at, OPERATOR, "."))
            {
              if (at + 1 == token->n_children)
                {
                  dsk_die ("unexpected EOF in namespace after '.': %s:%u",
                   cmdline_input, token->children[at+1].start_line);
                }
              if (token->children[at+1].type != DSK_CTOKEN_TYPE_BAREWORD)
                {
                  dsk_die ("expected bareword in namespace after '.': %s:%u",
                   cmdline_input, token->children[at+1].start_line);
                }
              at += 2;
            }
          else
            {
              dsk_die ("expected . and got '%.*s': %s:%u",
                       token->children[at].n_bytes,
                       token->children[at].start_char,
                       cmdline_input,
                       token->children[at].start_line);
            }
        }
      if (at == token->n_children)
        {
          dsk_die ("unexpected EOF in namespace declaration: %s:%u",
           cmdline_input, token->children[at-1].start_line);
        }
      at++;             /* skip semicolon */

      /* handle namespaces */
      DskBuffer lc_ns = DSK_BUFFER_INIT,
                uc_ns = DSK_BUFFER_INIT,
                mixed_ns = DSK_BUFFER_INIT;
      unsigned tmp;
      for (tmp = first_bareword; tmp < at; tmp += 2)
        {
          if (tmp > first_bareword)
            {
              dsk_buffer_append (&lc_ns, 2, "__");
              dsk_buffer_append (&uc_ns, 2, "__");
              dsk_buffer_append (&mixed_ns, 1, "_");
            }
          char *sub;
          const char *subname = dsk_ctoken_force_str (token->children + tmp);

          sub = dsk_codegen_lowercase_to_mixedcase (subname);
          dsk_buffer_append_string (&mixed_ns, sub);
          dsk_free (sub);

          dsk_buffer_append_string (&lc_ns, subname);

          sub = dsk_strdup (subname);
          dsk_ascii_strup (sub);
          dsk_buffer_append_string (&uc_ns, sub);
          dsk_free (sub);
        }
      dsk_buffer_append (&lc_ns, 2, "__");
      dsk_buffer_append (&uc_ns, 2, "__");
      dsk_buffer_append (&mixed_ns, 1, "_");
      namespace_struct_prefix = dsk_buffer_empty_to_string (&mixed_ns);
      namespace_func_prefix = dsk_buffer_empty_to_string (&lc_ns);
      namespace_enum_prefix = dsk_buffer_empty_to_string (&uc_ns);
    }

  char *hname = NULL, *cname = NULL;

  FILE *fp;
  if (use_stdout)
    fp = stdout;
  else
    {
      hname = dsk_strdup_printf ("%s.h", cmdline_output);
      fp = fopen (hname, "w");
      if (fp == NULL)
        dsk_die ("error creating %s: %s", cname, strerror (errno));
    }

  while (at < token->n_children)
    {
      if (IS_TOKEN (token->children + at, BAREWORD, "struct")
       || IS_TOKEN (token->children + at, BAREWORD, "union"))
        {
          dsk_boolean is_struct = contents[token->children[at].start_byte] == 's';
          const char *metatype_name = is_struct ? "struct" : "union";
          JsonMetatype metatype = is_struct ? JSON_METATYPE_STRUCT : JSON_METATYPE_UNION;
          if (at + 2 >= token->n_children)
            dsk_die ("premature EOF after '%s'", metatype_name);
          if (token->children[at+1].type != DSK_CTOKEN_TYPE_BAREWORD)
            dsk_die ("expected name after '%s' %s:%u",
                     metatype_name, cmdline_input, token->children[at+1].start_line);
          char *name = dsk_strndup (token->children[at+1].n_bytes, 
                               contents + token->children[at+1].start_byte);
          if (token->children[at+2].type != DSK_CTOKEN_TYPE_BRACE)
            dsk_die ("expected '{' after '%s %s' at %s:%u",
                     metatype_name, name,
                     cmdline_input, token->children[at+2].start_line);

          unsigned n_members;
          JsonMember *members;
          parse_member_list (metatype, name, contents,
                             token->children[at+2].n_children,
                             token->children[at+2].children,
                             &n_members, &members);

          /* create stub */
          JsonType *new_type = force_type_by_name (name, NULL);
          if (new_type->metatype != JSON_METATYPE_STUB)
            dsk_die ("type '%s' duplicate defined (compare lines %u and %u of %s)",
                     name,
                     new_type->def_line_no,
                     token->children[at].start_line,
                     cmdline_input);
          new_type->def_line_no = token->children[at].start_line;
          new_type->metatype = is_struct ? JSON_METATYPE_STRUCT : JSON_METATYPE_UNION;
          new_type->n_members = n_members;
          new_type->members = members;
          at += 3;
        }
      else if (IS_TOKEN (token->children + at, OPERATOR, ";"))
        {
          at++;
        }
      else
        {
          dsk_die ("unexpected token '%.*s' at %s:%u",
                   token->children[at].n_bytes, 
                   contents + token->children[at].start_byte,
                   cmdline_input,
                   token->children[at].start_line);
        }
    }

  /* resolve containment relationship; sort types by dependency */
  types = dsk_malloc (sizeof (JsonType*) * n_types);
  foreach (check_for_stubs);
  foreach (compute_type_order);

  /* --- Create header --- */

  hname = dsk_strdup_printf ("%s.h", cmdline_output);
  fp = fopen (hname, "w");

  /* write header */
  if (include_dsk)
    fprintf (fp, "#include <dsk.h>\n\n");

  /* write typedefs */
  for (i = 0; i < n_types; i++)
    {
      if (types[i]->metatype == JSON_METATYPE_UNION
       || types[i]->metatype == JSON_METATYPE_STRUCT)
        {
          fprintf (fp, "typedef struct %s %s;\n",
                   types[i]->c_name, types[i]->c_name);
        }
      if (types[i]->used_as_map_member)
        {
          fprintf (fp, "typedef struct %s%s_MapElement %s%s_MapElement;\n",
                   namespace_struct_prefix, types[i]->cc_name,
                   namespace_struct_prefix, types[i]->cc_name);
        }
    }
  fprintf (fp, "\n");

  /* write structures and function decls */
  for (i = 0; i < n_types; i++)
    {
      dsk_warning ("render decl for type %u/%u: %s", i,n_types,types[i]->name);
      if (types[i]->used_as_map_member)
        {
          fprintf (fp, "struct %s%s_MapElement\n{\n"
                       "  char *name;\n"
                       "  %s *value;\n"
                       "};\n",
                       namespace_struct_prefix, types[i]->cc_name,
                       types[i]->c_name);
        }
      if (types[i]->metatype == JSON_METATYPE_STRUCT)
        {
          fprintf (fp, "struct %s\n{\n", 
                   types[i]->c_name);
          for (j = 0; j < types[i]->n_members; j++)
            switch (types[i]->members[j].member_type)
              {
              case JSON_MEMBER_TYPE_DEFAULT:
                fprintf (fp, "  %s %s;\n",
                         types[i]->members[j].value_type->c_name,
                         types[i]->members[j].name);
                break;
              case JSON_MEMBER_TYPE_OPTIONAL:
                fprintf (fp, "  %s *%s;\n",
                         types[i]->members[j].value_type->c_name,
                         types[i]->members[j].name);
                break;
              case JSON_MEMBER_TYPE_ARRAY:
                fprintf (fp, "  unsigned n_%s;\n"
                             "  %s *%s;\n",
                         types[i]->members[j].name,
                         types[i]->members[j].value_type->c_name,
                         types[i]->members[j].name);
                break;
              case JSON_MEMBER_TYPE_MAP:
                fprintf (fp, "  unsigned n_%s;\n"
                             "  %s%s_MapElement *%s;\n",
                         types[i]->members[j].name,
                         namespace_struct_prefix, types[i]->members[j].value_type->cc_name,
                         types[i]->members[j].name);
                break;
              }
          fprintf (fp, "};\n\n");
        }
      else if (types[i]->metatype == JSON_METATYPE_UNION)
        {
          fprintf (fp, "typedef enum\n{\n");
          unsigned m;
          for (m = 0; m < types[i]->n_members; m++)
            {
              fprintf (fp, "  %s%s__%s,\n",
                       namespace_enum_prefix,
                       types[i]->uc_name,
                       types[i]->members[m].uc_name);
            }
          fprintf (fp, "} %s__Type;\n\n",
                   types[i]->c_name);
          fprintf (fp, "struct %s\n{\n", types[i]->c_name);
          fprintf (fp, "  %s__Type type;\n"
                       "  union {\n",
                   types[i]->c_name);
          for (m = 0; m < types[i]->n_members; m++)
            {
              if (types[i]->members[m].value_type != NULL)
                fprintf (fp, "    %s %s;\n",
                         types[i]->members[m].value_type->c_name,
                         types[i]->members[m].name);
            }
          fprintf (fp, "  } info;\n"
                       "};\n\n");
        }
      else
        continue;
      fprintf (fp, "dsk_boolean %s%s__from_json\n"
                   "         (DskMemPool *mem_pool,\n"
                   "          DskJsonValue    *json,\n"
                   "          %s         *out,\n"
                   "          DskError  **error);\n"
                   "DskJsonValue *%s%s__to_json\n"
                   "         (const %s *value);\n",
                   namespace_func_prefix, types[i]->lc_name,
                   types[i]->c_name,
                   namespace_func_prefix, types[i]->lc_name,
                   types[i]->c_name);
    }

  if (!use_stdout && !one_file)
    {
      fclose (fp);
    }


  /* --- Create implementation --- */
  if (!use_stdout && !one_file)
    {
      cname = dsk_strdup_printf ("%s.c", cmdline_output);
      fp = fopen (cname, "w");
      if (fp == NULL)
        dsk_die ("error creating %s: %s", cname, strerror (errno));

      /* write includes */
      fprintf (fp, "#include \"%s\"\n", hname);
    }
  fprintf (fp, "#include <string.h>\n");

  /* === write from_json() implementations === */
  for (i = 0; i < n_types; i++)
    {
      if (types[i]->metatype != JSON_METATYPE_UNION
       && types[i]->metatype != JSON_METATYPE_STRUCT)
        continue;
      fprintf (fp, "dsk_boolean %s%s__from_json\n"
                   "         (DskMemPool *mem_pool,\n"
                   "          DskJsonValue *json,\n"
                   "          %s *out,\n"
                   "          DskError  **error)\n"
                   "{\n",
                   namespace_func_prefix, types[i]->lc_name,
                   types[i]->c_name);
      switch (types[i]->metatype)
        {
        case JSON_METATYPE_UNION:
          fprintf (fp, "  if (json->type != DSK_JSON_VALUE_OBJECT)\n"
                       "    {\n"
                       "      dsk_set_error (error, \"union %%s from json: not an object\", \"%s\");\n"
                       "      return DSK_FALSE;\n"
                       "    }\n"
                       "  if (json->v_object.n_members == 1)\n"
                       "    {\n"
                       "      dsk_set_error (error, \"union %%s from json: did not have just one member\", \"%s\");\n"
                       "      return DSK_FALSE;\n"
                       "    }\n"
                       "  switch (json->v_object.members[0].name[0])\n"
                       "    {\n",
                       types[i]->c_name,
                       types[i]->c_name);
          uint8_t *members_used = dsk_malloc0 (types[i]->n_members);
          for (j = 0; j < types[i]->n_members; j++)
            if (!members_used[j])
              {
                fprintf (fp, "    case '%c':\n", types[i]->members[j].name[0]);
                unsigned k;
                for (k = j; k < types[i]->n_members; k++)
                  if (types[i]->members[j].name[0] == types[i]->members[k].name[0])
                    {
                      fprintf (fp,   "      if (strcmp (json->v_object.members[0].name, \"%s\") == 0)\n"
                                     "        {\n",
                               types[i]->members[k].name);
                      JsonType *mem_type = types[i]->members[k].value_type;
                      fprintf (fp,   "          out->type = %s%s__%s;\n",
                               namespace_enum_prefix, types[i]->uc_name,
                               types[i]->members[k].uc_name);
                      if (mem_type)
                        {
                          char *n = dsk_strdup_printf ("out->info.%s", types[i]->members[k].name);
                          implement_from_json_by_type (fp, mem_type,
                                                       10, "json->v_object.members[0].value", n);
                          dsk_free (n);
                        }
                      fprintf (fp,   "          return DSK_TRUE;\n"
                                     "        }\n");
                      members_used[k] = 1;
                    }
                fprintf (fp, "      break;\n");
              }
          dsk_free (members_used);
          fprintf (fp, "    default:\n"
                       "      break;\n"
                       "    }\n"
                       "  dsk_set_error (error, \"from_json -> %s: bad type '%%s'\",\n"
                       "                 json->v_object.members[0].name);\n"
                       "  return DSK_FALSE;\n"
                       "}\n",
                       types[i]->c_name);
          break;
        case JSON_METATYPE_STRUCT:
          fprintf (fp, "  if (json->type != DSK_JSON_VALUE_OBJECT)\n"
                       "    {\n"
                       "      dsk_set_error (error, \"union %%s from json: not an object\", \"%s\");\n"
                       "      return DSK_FALSE;\n"
                       "    }\n",
                       types[i]->c_name);
          for (j = 0; j < types[i]->n_members; j++)
            switch (types[i]->members[j].member_type)
              {
              case JSON_MEMBER_TYPE_DEFAULT:
                {
                  char *v = dsk_strdup_printf ("out->%s", types[i]->members[j].name);
                  fprintf (fp, "  {\n"
                               "    DskJsonValue *subvalue = dsk_json_object_get_value (json, \"%s\");\n"
                               "    if (subvalue == NULL)\n"
                               "      {\n"
                               "        dsk_set_error (error, \"could not get %%s in %%s\",\n"
                               "                       \"%s\", \"%s\");\n"
                               "        return DSK_FALSE;\n"
                               "      }\n",
                               types[i]->members[j].name,
                               types[i]->members[j].name,
                               types[i]->c_name);
                  implement_from_json_by_type (fp, types[i]->members[j].value_type,
                                               4, "subvalue", v);
                  fprintf (fp, "  }\n");
                  dsk_free (v);
                }
                break;
              case JSON_MEMBER_TYPE_OPTIONAL:
                {
                  char *v = dsk_strdup_printf ("out->%s", types[i]->members[j].name);
                  fprintf (fp, "  {\n"
                               "    DskJsonValue *subvalue = dsk_json_object_get_value (json, \"%s\");\n"
                               "    if (subvalue == NULL)\n"
                               "      %s = NULL;\n"
                               "    else\n"
                               "      {\n"
                               "        %s = dsk_mem_pool_alloc (mem_pool, sizeof (%s));\n",
                               types[i]->members[j].name,
                               v, v, types[i]->members[j].value_type->c_name);
                  char *vv = dsk_strdup_printf ("(*(%s))", v);
                  implement_from_json_by_type (fp, types[i]->members[j].value_type,
                                               8, "subvalue", vv);
                  fprintf (fp, "      }\n"
                               "  }\n");
                  dsk_free (vv);
                  dsk_free (v);
                }
                break;
              case JSON_MEMBER_TYPE_ARRAY:
                {
                  fprintf (fp, "  {\n"
                               "    DskJsonValue *subvalue = dsk_json_object_get_value (json, \"%s\");\n"
                               "    if (subvalue == NULL)\n"
                               "      {\n"
                               "        out->n_%s = 0;\n"
                               "        out->%s = NULL;\n"
                               "      }\n"
                               "    else if (subvalue->type != DSK_JSON_VALUE_ARRAY)\n"
                               "      {\n"
                               "        dsk_set_error (error, \"member %%s of %%s is not an array\",\n"
                               "                       \"%s\", \"%s\");\n"
                               "        return DSK_FALSE;\n"
                               "      }\n"
                               "    else\n"
                               "      {\n"
                               "        unsigned i;\n"
                               "        out->n_%s = subvalue->v_array.n_values;\n"
                               "        out->%s = dsk_mem_pool_alloc (mem_pool, out->n_%s * sizeof (%s));\n"
                               "        for (i = 0; i < out->n_%s; i++)\n"
                               "          {\n",
                               types[i]->members[j].name,
                               types[i]->members[j].name,
                               types[i]->members[j].name,
                               types[i]->members[j].name,
                               types[i]->c_name,
                               types[i]->members[j].name,
                               types[i]->members[j].name, types[i]->members[j].name, types[i]->members[j].value_type->c_name,
                               types[i]->members[j].name);
                  char *v = dsk_strdup_printf ("out->%s[i]", types[i]->members[j].name);
                  implement_from_json_by_type (fp, types[i]->members[j].value_type,
                                               12, "subvalue->v_array.values[i]",
                                               v);
                  dsk_free (v);
                  fprintf (fp, "          }\n"
                               "      }\n"
                               "  }\n");
                }
                break;
              case JSON_MEMBER_TYPE_MAP:
                {
                  fprintf (fp, "  {\n"
                               "    DskJsonValue *subvalue = dsk_json_object_get_value (json, \"%s\");\n"
                               "    if (subvalue == NULL)\n"
                               "      {\n"
                               "        out->n_%s = 0;\n"
                               "        out->%s = NULL;\n"
                               "      }\n"
                               "    else if (subvalue->type != DSK_JSON_VALUE_OBJECT)\n"
                               "      {\n"
                               "        dsk_set_error (error, \"member %%s of %%s is not an object\",\n"
                               "                       \"%s\", \"%s\");\n"
                               "        return DSK_FALSE;\n"
                               "      }\n"
                               "    else\n"
                               "      {\n"
                               "        unsigned i;\n"
                               "        out->n_%s = subvalue->v_object.n_members;\n"
                               "        out->%s = dsk_mem_pool_alloc (mem_pool, sizeof (%s%s_MapElement) * out->n_%s);\n"
                               "        %s *values = dsk_mem_pool_alloc (mem_pool, sizeof (%s) * out->n_%s);\n"
                               "        for (i = 0; i < out->n_%s; i++)\n"
                               "          {\n"
                               "            DskJsonMember *mi = subvalue->v_object.members_sorted_by_name[i];\n"
                               "            out->%s[i].name = mi->name;\n"
                               "            out->%s[i].value = values + i;\n",
                               types[i]->members[j].name,
                               types[i]->members[j].name,
                               types[i]->members[j].name,
                               types[i]->members[j].name, types[i]->c_name,
                               types[i]->members[j].name,
                               types[i]->members[j].name, namespace_struct_prefix, types[i]->members[j].value_type->cc_name, types[i]->members[j].name,
                               types[i]->members[j].value_type->c_name, types[i]->members[j].value_type->c_name, types[i]->members[j].name,
                               types[i]->members[j].name,
                               types[i]->members[j].name,
                               types[i]->members[j].name);
                  implement_from_json_by_type (fp, types[i]->members[j].value_type,
                                               12, "mi->value",
                                               "values[i]");
                  fprintf (fp, "          }\n"
                               "      }\n"
                               "  }\n");
                  break;
                }
              default:
                dsk_assert_not_reached ();
              }
            fprintf (fp, "  return DSK_TRUE;\n"
                         "}\n\n");
          break;
        default:
          dsk_assert_not_reached ();
        }
    }

  /* === write to_json() implementations === */
  for (i = 0; i < n_types; i++)
    {
      if (types[i]->metatype != JSON_METATYPE_UNION
       && types[i]->metatype != JSON_METATYPE_STRUCT)
        continue;
      fprintf (fp, "DskJsonValue *%s%s__to_json\n"
                   "         (const %s *value)\n"
                   "{\n",
                   namespace_func_prefix, types[i]->lc_name,
                   types[i]->c_name);
      switch (types[i]->metatype)
        {
        case JSON_METATYPE_UNION:
          fprintf (fp, "  DskJsonMember member;\n"
                       "  switch (value->type)\n"
                       "    {\n");
          for (j = 0; j < types[i]->n_members; j++)
            {
              fprintf (fp, "    case %s%s__%s:\n"
                           "      member.name = \"%s\";\n",
                       namespace_enum_prefix, types[i]->uc_name, types[i]->members[j].uc_name,
                       types[i]->members[j].name);
              if (types[i]->members[j].value_type)
                {
                  char *in = dsk_strdup_printf ("value->info.%s", types[i]->members[j].name); 
                  implement_to_json_by_type (fp, types[i]->members[j].value_type,
                                             6, in, "member.value");
                  dsk_free (in);
                }
              else
                fprintf (fp, "      member.value = dsk_json_value_new_null ();\n");
              fprintf (fp, "      return dsk_json_value_new_object (1, &member);\n");
            }
          fprintf (fp, "    }\n"
                       "  return NULL;\n");
          break;
        case JSON_METATYPE_STRUCT:
          fprintf (fp, "  unsigned n_members = 0;\n"
                       "  DskJsonMember members[%u];\n",
                       types[i]->n_members);
          for (j = 0; j < types[i]->n_members; j++)
            switch (types[i]->members[j].member_type)
              {
              case JSON_MEMBER_TYPE_DEFAULT:
                fprintf (fp, "  members[n_members].name = \"%s\";\n",
                         types[i]->members[j].name);
                {
                  char *v = dsk_strdup_printf ("value->%s", types[i]->members[j].name);
                  implement_to_json_by_type (fp, types[i]->members[j].value_type,
                                             2,
                                             v, "members[n_members].value");
                  fprintf (fp, "  n_members++;\n");
                  dsk_free (v);
                }
                break;
              case JSON_MEMBER_TYPE_MAP:
                fprintf (fp, "  members[n_members].name = \"%s\";\n",
                         types[i]->members[j].name);
                fprintf (fp, "  {\n"
                             "    DskJsonMember *submembers = dsk_malloc (sizeof (DskJsonMember) * value->n_%s);\n"
                             "    unsigned idx;\n"
                             "    for (idx = 0; idx < value->n_%s; idx++)\n"
                             "      {\n"
                             "        submembers[idx].name = value->%s[idx].name;\n",
                         types[i]->members[j].name,
                         types[i]->members[j].name,
                         types[i]->members[j].name);
                {
                  char *v = dsk_strdup_printf ("(*(value->%s[idx].value))", types[i]->members[j].name);
                  implement_to_json_by_type (fp, types[i]->members[j].value_type,
                                             8, v, "submembers[idx].value");
                  dsk_free (v);
                }
                fprintf (fp, "      }\n"
                             "    members[n_members].value = dsk_json_value_new_object (value->n_%s, submembers);\n"
                             "  }\n"
                             "  n_members++;\n",
                         types[i]->members[j].name);
                break;
              case JSON_MEMBER_TYPE_ARRAY:
                fprintf (fp, "  {\n"
                             "    DskJsonValue **v = dsk_malloc (sizeof (DskJsonValue*) * value->n_%s);\n"
                             "    unsigned idx;\n"
                             "    for (idx = 0; idx < value->n_%s; idx++)\n"
                             "      {\n",
                         types[i]->members[j].name,
                         types[i]->members[j].name);
                char *v = dsk_strdup_printf ("value->%s[idx]",
                                             types[i]->members[j].name);
                implement_to_json_by_type (fp, types[i]->members[j].value_type,
                                           8,
                                           v, "v[idx]");
                dsk_free (v);
                fprintf (fp, "      }\n"
                             "    members[n_members].name = \"%s\";\n"
                             "    members[n_members].value = dsk_json_value_new_array (value->n_%s, v);\n"
                             "    dsk_free (v);\n"
                             "    n_members++;\n"
                             "  }\n",
                             types[i]->members[j].name,
                             types[i]->members[j].name);
                break;
              case JSON_MEMBER_TYPE_OPTIONAL:
                fprintf (fp, "  if (value->%s != NULL)\n"
                             "    {\n"
                             "      members[n_members].name = \"%s\";\n",
                             types[i]->members[j].name,
                             types[i]->members[j].name);
                {
                  char *v = dsk_strdup_printf ("(*(value->%s))",
                                               types[i]->members[j].name);
                  implement_to_json_by_type (fp, types[i]->members[j].value_type,
                                             8, v, "members[n_members].value");
                  dsk_free (v);
                }
                fprintf (fp, "      n_members++;\n");
                fprintf (fp, "    }\n");
                break;
              }
          fprintf (fp, "  return dsk_json_value_new_object (n_members, members);\n");
          break;
        default:
          dsk_assert_not_reached ();
        }
      fprintf (fp, "}\n\n");
    }

  if (!use_stdout)
    {
      fclose (fp);
    }

  return 0;
}
