/* TODO: 
   - from_json() should take the object as an argument, return boolean

 */

#include <stdio.h>
#include <string.h>
#include "../dsk.h"
#include "../gskrbtreemacros.h"

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

typedef struct _Containment Containment;
struct _Containment
{
  JsonType *container;
  JsonType *contained;
  JsonType *contained_via;
  unsigned iteration;
  Containment *next_in_contained;
};

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

  Containment *contained_type_list;
};

#define JSON_TYPE_TREE_COMPARE(a,b,rv) rv = strcmp(a->name, b->name)
#define JSON_TYPE_TREE_KEY_COMPARE(a,b,rv) rv = strcmp(a, b->name)
#define GET_JSON_TYPE_TREE() \
  tree_top, JsonType *, GSK_STD_GET_IS_RED, GSK_STD_SET_IS_RED, \
  parent, left, right, \
  JSON_TYPE_TREE_COMPARE
JsonType *tree_top;

static JsonType *
force_type_by_name (const char *name)
{
  JsonType *type;
  GSK_RBTREE_LOOKUP_COMPARATOR(GET_JSON_TYPE_TREE(), name, JSON_TYPE_TREE_KEY_COMPARE, type);
  if (type != NULL)
    return type;
  type = dsk_malloc0 (sizeof (JsonType));
  type->name = dsk_strdup (name);
  type->metatype = JSON_METATYPE_STUB;
  type->cc_name = dsk_codegen_mixedcase_normalize (type->name);
  type->uc_name = dsk_codegen_mixedcase_to_uppercase (type->cc_name);
  type->lc_name = dsk_codegen_mixedcase_to_lowercase (type->cc_name);
  JsonType *conflict;
  GSK_RBTREE_INSERT (GET_JSON_TYPE_TREE (), type, conflict);
  dsk_assert (conflict == NULL);
  return type;
}

static void
parse_member_list (const char *metatype_name,
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
      member.value_type = force_type_by_name (type_name);
      unsigned at;
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
  *n_members_out = n_members;
  *members_out = members;
}

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
static dsk_boolean
expand_containments (JsonType *type,
                               unsigned iteration)
{
  Containment *c1, *c2;
  dsk_boolean rv = DSK_FALSE;
  for (c1 = type->contained_type_list; c1; c1 = c1->next_in_contained)
    for (c2 = c1->contained->contained_type_list; c2 && c2->iteration == iteration; c2 = c2->next_in_contained)
      if (add_containment (type,
                           c2->contained,
                           c1->contained,
                           iteration + 1))
        rv = DSK_TRUE;
  return rv;
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
      fprintf (fp, "%.*s%s = dsk_json_value_new_number (%s);\n",
               indent, "", output_name, input_name);
      break;
    case JSON_METATYPE_BOOLEAN:
      fprintf (fp, "%.*s%s = dsk_json_value_new_boolean (%s);\n",
               indent, "", output_name, input_name);
      break;
    case JSON_METATYPE_STRING:
      fprintf (fp, "%.*s%s = dsk_json_value_new_string (strlen (%s), %s);\n",
               indent, "", output_name, input_name, input_name);
      break;
    case JSON_METATYPE_JSON:
      fprintf (fp, "%.*s%s = dsk_json_value_copy (%s);\n",
               indent, "", output_name, input_name);
      break;

      /* STRUCT and UNION are handled by other generated functions */
    case JSON_METATYPE_STRUCT:
    case JSON_METATYPE_UNION:
      fprintf (fp, "%.*s%s = %s%s__to_json (&(%s);\n",
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
      fprintf (fp, "%.*sif (%s->type != %s)\n"
                   "%.*s  {\n"
                   "%.*s    dsk_set_error (error, \"not a %s\");\n"
                   "%.*s    return NULL;\n"
                   "%.*s  }\n",
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
      fprintf (fp, "%.*s%s = %s->v_number;\n",
               indent, "", output_name, input_name);
      break;
    case JSON_METATYPE_BOOLEAN:
      fprintf (fp, "%.*s%s = %s->v_boolean;\n",
               indent, "", output_name, input_name);
      break;
    case JSON_METATYPE_STRING:
      fprintf (fp, "%.*s%s = %s->v_string.str;\n",
               indent, "", output_name, input_name);
      break;

      /* STRUCT and UNION are handled by other generated functions */
    case JSON_METATYPE_STRUCT:
    case JSON_METATYPE_UNION:
      fprintf (fp, "%.*sif (!%s%s__from_json (mem_pool, %s, &%s, error))\n"
                   "%.*s  return DSK_FALSE;\n",         /*TODO: add_prefix to error */
               indent, "", namespace_func_prefix, type->lc_name, input_name, output_name,
               indent, "");
      break;
    case JSON_METATYPE_JSON:
      fprintf (fp, "%.*s%s = %s;\n", 
               indent, "", output_name, input_name);
      break;
    case JSON_METATYPE_STUB:
      dsk_assert_not_reached ();

    }
}

int main(int argc, char **argv)
{
  DskCTokenScannerConfig config = DSK_CTOKEN_SCANNER_CONFIG_DEFAULT;
  unsigned i, j;
  dsk_cmdline_init ("generate JSON bindings",
                    "Take a file with ...",
                    NULL,
                    0);
  dsk_cmdline_add_string ("input", "input specification file", "FILENAME",
                          DSK_CMDLINE_MANDATORY, &cmdline_input);
  dsk_cmdline_add_string ("output", "output basename file", "BASENAME",
                          DSK_CMDLINE_MANDATORY, &cmdline_output);
  dsk_cmdline_process_args (&argc, &argv);

  DskError *error = NULL;
  size_t len;
  char *contents = dsk_file_get_contents (cmdline_input, &len, &error);
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
    fund_type = force_type_by_name ("int");
    fund_type->metatype = JSON_METATYPE_INT;
    fund_type->c_name = "int";
    fund_type = force_type_by_name ("boolean");
    fund_type->metatype = JSON_METATYPE_BOOLEAN;
    fund_type->c_name = "dsk_boolean";
    fund_type = force_type_by_name ("number");
    fund_type->metatype = JSON_METATYPE_NUMBER;
    fund_type->c_name = "double";
    fund_type = force_type_by_name ("string");
    fund_type->metatype = JSON_METATYPE_STRING;
    fund_type->c_name = "char *";
  }
  
  unsigned at = 0;
  if (IS_TOKEN (token->children + at, BAREWORD, "namespace"))
    {
      if (at + 1 == token->n_children)
        {
          ...
        }
      if (token->children[at+1].type != DSK_CTOKEN_TYPE_BAREWORD)
        {
          ...
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
                  ...
                }
              if (token->children[at+1].type != DSK_CTOKEN_TYPE_BAREWORD)
                {
                  ...
                }
              at += 2;
            }
          else
            {
              dsk_die (...);
            }
        }
      if (at == token->n_children)
        {
          ...
        }
      at++;             /* skip semicolon */

      /* handle namespaces */
      ...
    }

  char *hname = NULL, *cname = NULL;

  if (use_stdout)
    fp = stdout;
  else
    {
      hname = dsk_strdup_printf ("%s.h", cmdline_output);
      fp = fopen (fname, "w");
      if (fp == NULL)
        dsk_die ("error creating %s: %s", cname, strerror (errno));
    }

  while (at < token->n_children)
    {
      if (IS_TOKEN (token->children + at, BAREWORD, "struct")
       || IS_TOKEN (token->children + at, BAREWORD, "union"))
        {
          dsk_boolean is_struct = contents[token->children[at].start_byte] == 's';
          const char *metatype = is_struct ? "struct" : "union";
          if (at + 2 >= token->n_children)
            dsk_die ("premature EOF after '%s'", metatype);
          if (token->children[at+1].type != DSK_CTOKEN_TYPE_BAREWORD)
            dsk_die ("expected name after '%s' %s:%u",
                     metatype, cmdline_input, token->children[at+1].start_line);
          char *name = dsk_strndup (token->children[at+1].n_bytes, 
                               contents + token->children[at+1].start_byte);
          if (token->children[at+1].type != DSK_CTOKEN_TYPE_BRACE)
            dsk_die ("expected '{' after '%s %.*s' at %s:%u",
                     metatype, name,
                     cmdline_input, token->children[at+2].start_line);

          parse_member_list (metatype, name, contents,
                             token->children[at+2].n_children,
                             token->children[at+2].children,
                             &n_members, &members);

          /* create stub */
          new_type = force_type_by_name (name);
          if (new_type->metatype != JSON_METATYPE_STUB)
            dsk_die ("type '%s' duplicate defined (compare lines %u and %u of %s)",
                     name,
                     new_type->def_line_no,
                     token->children[at].start_line,
                     cmdline_input);
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
  n_types = 0;
  types = dsk_malloc (sizeof (JsonType*) * n_types);
  if (type_tree)
    {
      foreach (check_for_undefined_types);
      foreach (init_containments);
      while (foreach (expand_containments))
        {}
      foreach (check_for_self_containment);
      foreach (compute_impl_order);
    }

  /* --- Create header --- */

  fname = dsk_strdup_printf ("%s.h", cmdline_output);
  fp = fopen (fname, "w");

  /* write header */
  if (include_dsk)
    fprintf (fp, "#include <dsk.h>\n\n");

  /* write typedefs */
  for (i = 0; i < n_types; i++)
    {
      if (!types[i]->is_fundamental)
        {
          fprintf (fp, "typedef struct %s %s;",
                   types[i]->cname, types[i]->cname);
        }
      if (types[i]->used_as_map_member)
        {
          fprintf (fp, "typedef struct %s%s_MapElement %s%s_MapElement;",
                   namespace_struct_prefix, types[i]->cc_name,
                   namespace_struct_prefix, types[i]->cc_name);
        }
    }
  fprintf (fp, "\n");

  /* write structures and function decls */
  for (i = 0; i < n_types; i++)
    {
      if (types[i]->used_as_map_member)
        {
          fprintf (fp, "struct %s%s_MapElement\n{\n"
                       "  char *name;\n"
                       "  %s *value;\n",
                       namespace_struct_prefix, types[i]->cc_name,
                       types[i]->cname);
        }
      if (types[i]->metatype == JSON_METATYPE_STRUCT)
        {
          fprintf (fp, "struct %s\n{\n", 
                   types[i]->cname);
          for (j = 0; j < types[i]->n_members; j++)
            switch (types[i]->members[j])
              {
              case JSON_MEMBER_TYPE_DEFAULT:
                fprintf (fp, "  %s %s;\n",
                         types[i]->members[j]->type->cname,
                         types[i]->members[j]->name);
                break;
              case JSON_MEMBER_TYPE_OPTIONAL:
                fprintf (fp, "  %s *%s;\n",
                         types[i]->members[j]->type->cname,
                         types[i]->members[j]->name);
                break;
              case JSON_MEMBER_TYPE_ARRAY:
                fprintf (fp, "  unsigned n_%s;\n"
                             "  %s *%s;\n",
                         types[i]->members[j]->name,
                         types[i]->members[j]->type->cname,
                         types[i]->members[j]->name);
                break;
              case JSON_MEMBER_TYPE_MAP:
                fprintf (fp, "  unsigned n_%s;\n"
                             "  %s%s *%s;\n",
                         types[i]->members[j]->name,
                         namespace_struct_prefix, types[i]->members[j]->cc_name,
                         types[i]->members[j]->name);
                break;
              }
            }
          fprintf (fp, "};\n\n");
        }
      else if (types[i]->metatype == JSON_METATYPE_UNION)
        {
          fprintf (fp, "typedef enum\n{\n");
          for (i = 0; i < types[i]->n_members; i++)
            {
              fprintf (fp, "  %s%s__%s,\n",
                       namespace_enum_prefix,
                       types[i]->uc_name,
                       types[i]->members[j].uc_name);
            }
          fprintf (fp, "} %s%s__Type;\n\n",
                   namespace_struct_prefix, types[i]->name);
          fprintf (fp, "struct %s\n{\n");
          fprintf (fp, "  %s%s__Type type;\n",
                   namespace_enum_prefix,
                   types[i]->uc_name);
          for (i = 0; i < types[i]->n_members; i++)
            {
              if (types[i]->members[i].type != NULL)
                fprintf (fp, "  %s %s;\n",
                         types[i]->members[j].type->c_name,
                         types[i]->members[j].name);
            }
          fprintf (fp, "};\n\n");
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
                   namespace_func_prefix, type->lcname,
                   type->cname,
                   namespace_func_prefix, type->lcname,
                   type->cname);
    }

  if (!use_stdout && !one_file)
    {
      fclose (fp);
    }


  /* --- Create implementation --- */
  if (!use_stdout && !one_file)
    {
      cname = dsk_strdup_printf ("%s.c", cmdline_output);
      fp = fopen (fname, "w");
      if (fp == NULL)
        dsk_die ("error creating %s: %s", cname, strerror (errno));

      /* write includes */
      fprintf (fp, "#include \"%s\"\n", hname);
    }

  /* === write from_json() implementations === */
  for (i = 0; i < n_types; i++)
    {
      if (types[i]->metatype != JSON_METATYPE_UNION
       && types[i]->metatype != JSON_METATYPE_STRUCT)
        continue;
      fprintf (fp, "%s *%s%s__from_json\n"
                   "         (DskMemPool *mem_pool,\n"
                   "          DskJsonValue *json,\n"
                   "          DskError  **error)\n"
                   "{\n",
                   type->cname,
                   namespace_func_prefix, type->lcname);
      switch (types[i]->metatype)
        {
        case JSON_METATYPE_UNION:
          fprintf (fp, "  if (json->type != DSK_JSON_VALUE_OBJECT)\n"
                       "    {\n"
                       "      dsk_set_error (error, \"union %%s from json: not an object\", \"%s\");\n"
                       "      return NULL;\n"
                       "    }\n"
                       "  if (json->v_object.n_members == 1)\n"
                       "    {\n"
                       "      dsk_set_error (error, \"union %%s from json: did not have just one member\", \"%s\");\n"
                       "      return NULL;\n"
                       "    }\n"
                       "  switch (json->v_object.members[0].name)\n"
                       "    {\n",
                       type->cname,
                       type->cname);
          uint8_t *members_used = dsk_malloc0 (types[i]->n_members);
          for (j = 0; j < types[i]->n_members; j++)
            if (!members_used[j])
              {
                fprintf (fp, "    case '%c':\n", types[i]->members[j].name[0]);
                for (k = j; k < types[i]->n_members; k++)
                  if (types->members[j].name[0] == types->members[k].name[0])
                    {
                      fprintf (fp, "      if (strcmp (json->v_object.members[0].name, \"%s\") == 0)\n"
                                   "        {\n"
                                   "          %s *rv = dsk_mem_pool_alloc (mem_pool, sizeof (%s));\n"
                                   "          rv->type = %s%s__%s;\n"
                               types->members[k].name,
                               types->cname,
                               namespace_enum_prefix, types[i]->ucname,
                               types[i]->members[k].uc_name);
                      char *n = dsk_strdup_printf ("rv->info.%s", types[i]->members[k].name);
                      implement_from_json_by_type (fp, types[i]->members[k].type,
                                                   10, n);
                      dsk_free (n);
                      fprintf (fp, "          return rv;\n"
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
                       "                 json->v_object.members[0].name)\n"
                       "  return NULL;\n",
                       types[i]->cname);
          break;
        case JSON_METATYPE_STRUCT:
          fprintf (fp, "  if (json->type != DSK_JSON_VALUE_OBJECT)\n"
                       "    {\n"
                       "      dsk_set_error (error, \"union %%s from json: not an object\", \"%s\");\n"
                       "      return NULL;\n"
                       "    }\n",
                       type->cname);
          for (j = 0; j < types[i]->n_members; j++)
            switch (types[i]->members[j].member_type)
              {
              case JSON_MEMBER_TYPE_DEFAULT:
                {
                  char *v = dsk_strdup_printf ("value->%s", types[i]->members[j].name);
                  fprintf (fp, "  {\n"
                               "    DskJsonValue *subvalue = dsk_json_object_get_value (json, \"%s\");\n"
                               "    if (subvalue == NULL)\n"
                               "      {\n"
                               "        dsk_set_error (error, \"could not get %%s in %%s\",\n"
                               "                       \"%s\", \"%s\");\n"
                               "        return NULL;\n"
                               "      }\n",
                               types[i]->members[j].name,

                  implement_from_json_by_type (fp, 2, types[i]->members[j].type, "subvalue", v);
                  dsk_free (v);
                }
                break;
              case JSON_MEMBER_TYPE_OPTIONAL:
                ...
              case JSON_MEMBER_TYPE_ARRAY:
                ...
              case JSON_MEMBER_TYPE_MAP:
                ...
              default:
                dsk_assert_not_reached ();
              }
          break;
        }
      fprintf (fp, "}\n\n");
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
                   namespace_func_prefix, type->lcname,
                   type->cname);
      switch (types[i]->metatype)
        {
        case JSON_METATYPE_UNION:
          fprintf (fp, "  DskJsonMember member;\n"
                       "  switch (value->type)\n"
                       "    {\n");
          for (j = 0; j < types[i]->n_members; j++)
            {
              fprintf (fp, "    case %s%s__%s:\n"
                           "      member.name = \"%s\";\n"
                       namespace_enum_prefix,
                       types[i]->uc_name,
                       types[i]->members[j].uc_name,
                       types[i]->members[j].name);
              implement_to_json_by_type (fp, types[i]->members[j].type,
                                         6,
                                         "member.type");
              fprintf (fp, "      return dsk_json_value_new_object (1, &member);\n");
            }
          fprintf (fp, "    default:\n"
                       "      dsk_assert_not_reached ();\n");
          break;
        case JSON_METATYPE_STRUCT:
          fprintf (fp, "  unsigned n_members = 0;\n"
                       "  DskJsonMember members[%u];\n",
                       types[i]->n_members);
          for (j = 0; j < types[i]->n_members; j++)
            switch (types[i]->members[j].member_type)
              {
              case JSON_MEMBER_TYPE_DEFAULT:
                fprintf (fp, "    members[n_members].name = \"%s\";\n",
                         types[i]->members[j].name);
                implement_to_json_by_type (fp, types[i]->members[j].type,
                                           4,
                                           "members[n_members].value");
                fprintf (fp, "    n_members++;\n");
                break;
              case JSON_MEMBER_TYPE_MAP:
                fprintf (fp, "    members[n_members].name = \"%s\";\n",
                         types[i]->members[j].name);
                fprintf (fp, "    {\n"
                             "      DskJsonMember *submembers = dsk_malloc (sizeof (DskJsonMember) * value->n_%s);\n"
                             "      unsigned idx;\n"
                             "      for (idx = 0; idx < value->n_%s; idx++)\n"
                             "        {\n"
                             "          submembers[idx].name = value->%s[idx].name;\n",
                         types[i]->members[j].name,
                         types[i]->members[j].name,
                         types[i]->members[j].name);
                implement_to_json_by_type (fp, types[i]->members[j].type,
                                           10, "submembers[idx].value");
                fprintf (fp, "        }\n"
                             "      members[n_members].value = dsk_json_value_new_array (value->n_%s, submembers);\n"
                             "    }\n"
                             "  n_members++;\n",
                         types[i]->members[j].name);
                break;
              case JSON_MEMBER_TYPE_ARRAY:
                fprintf (fp, "    {\n"
                             "      DskJsonValue **v = dsk_malloc (sizeof (DskJsonValue*) * value->n_%s);\n"
                             "      unsigned idx;\n"
                             "      for (idx = 0; idx < value->n_%s; idx++)\n"
                             "        {\n",
                         types[i]->members[j].name,
                         types[i]->members[j].name);
                char *v = dsk_strdup_printf ("value->%s",
                                             types[i]->members[j].name);
                implement_to_json_by_type (fp, types[i]->members[j].type,
                                           10,
                                           v, "v[idx]");
                dsk_free (v);
                fprintf (fp, "        }\n"
                             "    }\n"
                             "    members[n_members].name = \"%s\";\n"
                             "    members[n_members].value = dsk_json_value_new_array (n_%s, %s);\n"
                             "    n_members++;\n",
                             types[i]->members[j].name,
                             types[i]->members[j].name,
                             types[i]->members[j].name);
                break;
              case JSON_MEMBER_TYPE_OPTIONAL:
                fprintf (fp, "    if (value->%s != NULL)\n"
                             "      {\n"
                             "        members[n_members].name = \"%s\";\n",
                             types[i]->members[j].name,
                             types[i]->members[j].name);
                char *v = dsk_strdup_printf ("(*(value->%s))",
                                             types[i]->members[j].name);
                implement_to_json_by_type (fp, types[i]->members[j].type,
                                           8, v, "members[n_members].value");
                dsk_free (v);
                fprintf (fp, "        n_members++;\n");
                fprintf (fp, "      }\n");
                break;
              }
          fprintf (fp, "  return dsk_json_value_new_object (n_members, members);\n");
          break;
        }
      fprintf (fp, "}\n\n");
    }

  if (!use_stdout)
    {
      fclose (fp);
    }

  return 0;
}
