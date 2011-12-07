
/* TODO: support attributes */

typedef struct _DskXmlToJsonConverter DskXmlToJsonConverter;

typedef enum
{
  PATH_HINT_NONE,
  PATH_HINT_SUPPRESS = (1<<0),
  PATH_HINT_NUMBER = (1<<1),
  PATH_HINT_BOOLEAN = (1<<2),
  PATH_HINT_STRING = (1<<3),
  PATH_HINT_ARRAY = (1<<4)
} PathHint;

struct _Node
{
  PathHint hint;

  ExactComponent *exact_tree;

  unsigned n_patterns;
  PatternComponent *patterns;

  Node *doublestars;
};

struct _ExactComponent
{
  Node node;
  char *path;
  ExactPath *left,*right,*parent;
};

struct _PatternComponent
{
  Node node;
  char *pattern_string;
  DskPattern *pattern;
};

struct _Path
{
  dsk_boolean 
  char *component;

  /* a tree sorted by name */
  Path *exact_subpaths;

  unsigned n_pattern;
};

struct _DskXmlToJsonConverter
{
  /* Maximum number of double-star path entries
     that need simultaneous tracking. */
  unsigned max_doublestars;

  unsigned max_othernodes;

  Node top_node;
};


DskXmlToJsonConverter *
dsk_xml_to_json_converter_new (DskXmlToJsonConverterFlags flags)
{
  ...
}

/* The PATH is an argument to many functions below.
 * It resembles an XPath, but it's much more restricted.
 * 
 * Each path component is separated by "/".
 * "**" is a special path component
 * that means "matching any number or nesting levels";
 * otherwise each path is a DskPattern-compatible string.
 *
 * "**" 

 * Optimization note:  "**" in the middle of an expression can be
 * expensive - exponentially expensive on the number of "**"'s you have
 * in the middle of your path - but the constant-time expense is
 * (relatively) big.
 */

static void
do_hint (DskXmlToJsonConverter *converter,
         const char            *path,
         PathHint               hint)
{
  char **pieces = dsk_strsplit (path, "/");
  Node *at = &converter->top_node;
  for (i = 0; pieces[i]; i++)
    if (pieces[i][0] == 0)
      {
        /* ignore empty components */
      }
    else if (is_literal (pieces[i]))
      {
        /* add/lookup exact tree */
        ...
      }
    else if (strcmp (pieces[i], "**") == 0)
      {
        if (at->doublestars == NULL)
          {
            ...
          }
        at = at->doublestars;
      }
    else
      {
        /* must be a pattern */
        for (j = 0; j < at->n_patterns; j++)
          if (strcmp (at->patterns[j].pattern_string, pieces[i]) == 0)
            break;
        if (j == at->n_patterns)
          {
            ...
          }
        at = &at->patterns[j].node
      }
  if (at->hint != PATH_HINT_NONE)
    {
      dsk_warning ("hint already given for path %s", path);
    }
  at->hint = hint;
}

void dsk_xml_to_json_converter_hint_array (DskXmlToJsonConverter *,
                                           const char *path);
void dsk_xml_to_json_converter_hint_unused(DskXmlToJsonConverter *,
                                           const char *path);
void dsk_xml_to_json_converter_hint_string(DskXmlToJsonConverter *,
                                           const char *path);
void dsk_xml_to_json_converter_hint_number(DskXmlToJsonConverter *,
                                           const char *path);
void dsk_xml_to_json_converter_hint_boolean(DskXmlToJsonConverter *,
                                            const char *path);


#define NEWA(type, count) ((type*)(alloca(sizeof(type) * (count))))

DskJsonValue *xml_to_json_recursive (DskXmlToJsonConverter *converter,
                                     unsigned               n_nodes,
                                     Node                 **nodes,
                                     unsigned               n_doublestars,
                                     Node                 **doublestars,
                                     DskXml                *xml,
                                     DskError             **error)
{
  unsigned i;
  unsigned n_members = 0;
  DskJsonMember *members = NEWA (DskJsonMember, xml->n_children);
  unsigned n_array_members = 0;
  DskJsonMember *array_members = NEWA (DskJsonMember, xml->n_children);
  dsk_assert (xml->type == DSK_XML_ELEMENT);
  for (i = 0; i < xml->n_children; i++)
    if (xml->children[i]->type == DSK_XML_ELEMENT)
      {
        const char *c = xml->children[i]->str;
        const char *subtext = NULL;
        unsigned n_subnodes = 0;
        unsigned n_new_doublestar_nodes = 0;

        if (xml->children[i]->n_children == 0)
          subtext = "";
        else (xml->children[i]->n_children == 1
           && xml->children[i]->children[0]->type == DSK_XML_TEXT)
          subtext = xml->children[i]->children[0]->str;

        PathHint hint = PATH_HINT_NONE;

        /* find hint */
        for (j = 0; j < n_nodes; j++)
          {
            ExactPath *sub = lookup_exact (nodes[i], c);
            if (sub)
              {
                if (sub->hint != PATH_HINT_NONE)
                  hint |= sub->hint;
              }

            /* try patterns */
            ...

            /* try double-stars */
            ...
          }
        DskJsonValue *sub = NULL;
        if (hint & PATH_HINT_SUPPRESS)
          {
            /* do nothing */
          }
        else if (hint & PATH_HINT_NUMBER)
          {
            if (subtext == NULL)
              {
                if (!converter->suppress_errors)
                  {
                    dsk_set_error (error,
                           "expected number, got structured data, in <%s>", c);
                    goto error_cleanup;
                  }
              }
            else
              {
                double value = strtod (subtext, &end);
                if (subtext == end || *end != 0)
                  {
                    ...
                  }
                sub = dsk_json_value_new_number (value);
              }
          }
        else if (hint & PATH_HINT_BOOLEAN)
          {
            ...
          }
        else if ((hint & PATH_HINT_STRING) || subtext != NULL)
          {
            /* string */
            if (subtext == NULL)
              {
                if (!converter->suppress_errors)
                  {
                    dsk_set_error ("expected string, got structured data in <%s>", c);
                    goto error_cleanup;
                  }
              }
            else
              sub = dsk_json_value_new_string (strlen (subtext), subtext);
          }
        else
          {
            /* subobject: recurse */
            ...
          }
        if (sub != NULL)
          {
            if (hint & PATH_HINT_ARRAY)
              {
                array_members[n_array_members].name = c;
                array_members[n_array_members].value = sub;
                n_array_members++;
              }
            else
              {
                members[n_members].name = c;
                members[n_members].value = sub;
                n_members++;
              }
          }
      }

  /* consolidate array members */
  ...

  return dsk_json_value_new_object (n_members, members);
}


DskJsonValue   *
dsk_xml_to_json_convert              (DskXmlToJsonConverter *converter,
                                      const DskXml  *xml,
                                      DskError     **error)
{
  ...
}
