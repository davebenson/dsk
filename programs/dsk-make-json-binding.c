

/* if all the following, the default is blank.  */
char *namespace_func_prefix;       // eg "foo__"
char *namespace_struct_prefix;     // eg "Foo_"
har *namespace_enum_prefix;       // eg "FOO__"

typedef enum
{
  JSON_METATYPE_INT,
  JSON_METATYPE_BOOLEAN,
  JSON_METATYPE_NUMBER,
  JSON_METATYPE_STRING,
  JSON_METATYPE_UNION,
  JSON_METATYPE_STRUCT,
  JSON_METATYPE_STUB
} JsonMetatype

typedef enum
{
  JSON_MEMBER_TYPE_DEFAULT,
  JSON_MEMBER_TYPE_MAP,
  JSON_MEMBER_TYPE_ARRAY,
  JSON_MEMBER_TYPE_OPTIONAL
} JsonMemberType;

struct _JsonMember
{
  char *name;
  JsonMemberType member_Type;
  JsonType *value_type;
};
  JSON_METATYPE_ARRAY,
  JSON_METATYPE_MAP,

struct _JsonType
{
  char *name;
  JsonMetatype metatype;

  unsigned n_members;		/* for union, struct */
  JsonMember *members;

  JsonType *subtype;		/* for array, map, optional */

  JsonType *left, *right, *parent;
  dsk_boolean is_red;
};

static void
parse_type (unsigned n_tokens,
            DskCToken *tokens,
            unsigned *n_used_out,
            JsonType **type_out)
{
  ...
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
  parse_type (n_tokens, tokens, &tokens_used, &type);
  if (n_tokens < tokens_used + 2)
    {
      ...
    }
  if (tokens[tokens_used].type != DSK_CTOKEN_TYPE_BAREWORD)
    {
      ...
    }
  while (...)
    {
      /* handle arrays and assoc-arrays */
      ...
    }
  if (tokens_used < n_tokens
      && IS_TOKEN (tokens + tokens_used, OPERATOR, "?"))
    {
      /* optional */
      ...
    }

  /* name */
  ...

  /* check semicolon */
  ...

          parse_member_list (metatype, name, contents,
                             token->children[at+2].n_children,
                             token->children[at+2].children,
                             &n_members, &members);

int main(int argc, char **argv)
{
  DskCTokenScannerConfig config = DSK_CTOKEN_SCANNER_CONFIG_DEFAULT;
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
  if (content == NULL)
    dsk_die ("error reading input file: %s", error->message);
  config.error_filename = cmdline_input;

  DskCToken *token;
  token = dsk_ctoken_scan_str (contents, contents + len, &config, &error);
  if (token == NULL)
    dsk_die ("error scanning ctokens: %s", error->message);
  
  at = 0;
  if (is_token_bareword (token->children + at, "namespace"))
    {
      ...
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
}
