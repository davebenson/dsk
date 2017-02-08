#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <alloca.h>
#include "dsk.h"
#include "dsk-xml-binding-internals.h"
#include "dsk-rbtree-macros.h"
#define DANG_SIZEOF_SIZE_T 8
#include "dsk-qsort-macro.h"
#define DSK_STRUCT_MEMBER_P(struct_p, struct_offset)   \
    ((void*) ((char*) (struct_p) + (long) (struct_offset)))
#define DSK_STRUCT_MEMBER(member_type, struct_p, struct_offset)   \
    (*(member_type*) DSK_STRUCT_MEMBER_P ((struct_p), (struct_offset)))

typedef struct _NamespaceNode NamespaceNode;
struct _NamespaceNode
{
  DskXmlBindingNamespace *ns;
  NamespaceNode *left, *right, *parent;
  dsk_boolean is_red;
};
#define COMPARE_NAMESPACE_NODES(a,b, rv) rv = strcmp (a->ns->name, b->ns->name)
#define NAMESPACE_TREE(binding) \
  binding->namespace_tree, NamespaceNode*, \
  DSK_STD_GET_IS_RED, DSK_STD_SET_IS_RED, \
  parent, left, right, \
  COMPARE_NAMESPACE_NODES

#define COMPARE_STRING_TO_NAMESPACE_INFO(a, b, rv) rv = strcmp (a, b->ns->name)

typedef struct _SearchPath SearchPath;
struct _SearchPath
{
  char *path;
  unsigned path_len;
  char *separator;
  unsigned sep_len;
};
struct _DskXmlBinding
{
  NamespaceNode *namespace_tree;

  unsigned n_search_paths;
  SearchPath *search_paths;
};

DskXmlBinding *dsk_xml_binding_new (void)
{
  DskXmlBinding *rv = DSK_NEW (DskXmlBinding);
  rv->namespace_tree = NULL;
  rv->n_search_paths = 0;
  rv->search_paths = NULL;
  return rv;
}

static void
free_ns_tree_recursve (NamespaceNode *n)
{
  if (n == NULL)
    return;
  dsk_xml_binding_namespace_unref (n->ns);
  free_ns_tree_recursve (n->left);
  free_ns_tree_recursve (n->right);
  dsk_free (n);
}

void           dsk_xml_binding_free (DskXmlBinding *binding)
{
  unsigned i;
  free_ns_tree_recursve (binding->namespace_tree);
  for (i = 0; i < binding->n_search_paths; i++)
    {
      dsk_free (binding->search_paths[i].path);
      dsk_free (binding->search_paths[i].separator);
    }
  dsk_free (binding->search_paths);
  dsk_free (binding);
}

void           dsk_xml_binding_add_searchpath (DskXmlBinding *binding,
                                               const char    *path,
                                               const char    *ns_separator)
{
  binding->search_paths = dsk_realloc (binding->search_paths,
                                       (binding->n_search_paths+1) * sizeof (SearchPath));
  binding->search_paths[binding->n_search_paths].path = dsk_strdup (path);
  binding->search_paths[binding->n_search_paths].separator = dsk_strdup (ns_separator);
  binding->search_paths[binding->n_search_paths].path_len = strlen (path);
  binding->search_paths[binding->n_search_paths].sep_len = strlen (ns_separator);
  binding->n_search_paths++;
}

DskXmlBindingNamespace*
dsk_xml_binding_try_ns         (DskXmlBinding *binding,
                                const char    *name)
{
  NamespaceNode *ns;
  DSK_RBTREE_LOOKUP_COMPARATOR (NAMESPACE_TREE (binding),
                                name, COMPARE_STRING_TO_NAMESPACE_INFO,
                                ns);
  return ns ? ns->ns : NULL;
}

DskXmlBindingNamespace*
dsk_xml_binding_get_ns         (DskXmlBinding *binding,
                                const char    *name,
                                DskError     **error)
{
  DskXmlBindingNamespace *ns = dsk_xml_binding_try_ns (binding, name);
  unsigned n_dots, n_non_dots;
  unsigned i;
  const char *at;
  if (ns != NULL)
    return ns;

  n_dots = n_non_dots = 0;
  for (at = name; *at; at++)
    if (*at == '.')
      n_dots++;
    else
      n_non_dots++;

  /* look along searchpath */
  for (i = 0; i < binding->n_search_paths; i++)
    {
      unsigned length = binding->search_paths[i].path_len
                   + 1
                   + n_non_dots
                   + n_dots * binding->search_paths[i].sep_len
                   + 1;
      char *path = dsk_malloc (length);
      char *out = path + binding->search_paths[i].path_len;
      memcpy (path, binding->search_paths[i].path,
              binding->search_paths[i].path_len);
      *out++ = '/';
      for (at = name; *at; at++)
        if (*at == '.')
          {
            memcpy (out, binding->search_paths[i].separator,
                    binding->search_paths[i].sep_len);
            out += binding->search_paths[i].sep_len;
          }
        else
          *out++ = *at;
      *out = 0;

      /* try opening file */
      if (dsk_file_test_exists (path))
        {
          uint8_t *contents = dsk_file_get_contents (path, NULL, error);
          DskXmlBindingNamespace *real_ns;
          NamespaceNode *node;
          NamespaceNode *conflict;
          if (contents == NULL)
            {
              dsk_free (path);
              return NULL;
            }
          real_ns = _dsk_xml_binding_parse_ns_str (binding, path, name,
                                                   (char*) contents, error);
          if (real_ns == NULL)
            {
              dsk_free (path);
              dsk_free (contents);
              return NULL;
            }
          dsk_free (path);

          /* add to tree */
          node = DSK_NEW (NamespaceNode);
          node->ns = real_ns;
          DSK_RBTREE_INSERT (NAMESPACE_TREE (binding), node, conflict);
          dsk_assert (conflict == NULL);
          dsk_free (contents);
          return real_ns;
        }
      dsk_free (path);
    }

  dsk_set_error (error, "namespace %s could not be found on search-path", name);
  return NULL;
}

/* --- namespaces --- */
DskXmlBindingNamespace *
dsk_xml_binding_namespace_new (const char *name)
{
  DskXmlBindingNamespace *rv = DSK_NEW (DskXmlBindingNamespace);
  rv->is_static = DSK_FALSE;
  rv->name = dsk_strdup (name);
  rv->n_types = 0;
  rv->types = NULL;
  rv->ref_count = 1;
  rv->types_sorted_by_name = NULL;
  return rv;
}
void dsk_xml_binding_namespace_unref (DskXmlBindingNamespace *ns)
{
  if (!ns->is_static)
    {
      if (--(ns->ref_count) == 0)
        {
          unsigned i;
          for (i = 0; i < ns->n_types; i++)
            dsk_xml_binding_type_unref (ns->types[i]);
          dsk_free (ns->types);
          dsk_free (ns->types_sorted_by_name);
          dsk_free (ns->name);
          dsk_free (ns);
        }
    }
}
DskXmlBindingType *
dsk_xml_binding_namespace_lookup (DskXmlBindingNamespace *ns,
                                  const char             *name)
{
  unsigned start = 0, count = ns->n_types;
  const unsigned *sorted = ns->types_sorted_by_name;
  while (count > 0)
    {
      unsigned mid = start + count / 2;
      unsigned mid_index = sorted ? sorted[mid] : mid;
      DskXmlBindingType *mid_type = ns->types[mid_index];
      int rv = strcmp (mid_type->name, name);
      if (rv < 0)
        {
          unsigned new_start = mid + 1;
          count -= new_start - start;
          start = new_start;
        }
      else if (rv > 0)
        {
          count /= 2;
        }
      else
        return mid_type;
    }
  return NULL;
}

/* --- fundamental types --- */
#define CHECK_IS_TEXT() \
  if (to_parse->type != DSK_XML_TEXT) \
    { \
      dsk_set_error (error, "expected string XML node for type %s, got <%s>", \
                     type->name, to_parse->str); \
      return DSK_FALSE; \
    }
static dsk_boolean
xml_binding_parse__int(DskXmlBindingType *type,
                       DskXml            *to_parse,
		       void              *out,
		       DskError         **error)
{
  char *end;
  CHECK_IS_TEXT ();
  * (int *) out = strtol (to_parse->str, &end, 10);
  if (end == to_parse->str || *end != 0)
    {
      dsk_set_error (error, "error parsing int from XML node");
      return DSK_FALSE;
    }
  return DSK_TRUE;
}

static DskXml   *
xml_binding_to_xml__int(DskXmlBindingType *type,
                        const void        *data,
		        DskError         **error)
{
  char buf[64];
  DSK_UNUSED (type); DSK_UNUSED (error);
  snprintf (buf, sizeof (buf), "%d", * (int *) data);
  return dsk_xml_text_new (buf);
}

DskXmlBindingType dsk_xml_binding_type_int =
{
  DSK_TRUE,    /* is_fundamental */
  DSK_TRUE,    /* is_static */
  DSK_FALSE,   /* is_struct */
  DSK_FALSE,   /* is_union */
  0,           /* ref_count */

  sizeof (int),
  DSK_ALIGNOF_INT,
  &dsk_xml_binding_namespace_builtin,
  "int",       /* name */
  "int",       /* ctypename */
  xml_binding_parse__int,
  xml_binding_to_xml__int,
  NULL,        /* no clear */
  NULL         /* finalize_type */
};

static dsk_boolean
xml_binding_parse__uint(DskXmlBindingType *type,
                        DskXml            *to_parse,
		        void              *out,
		        DskError         **error)
{
  char *end;
  CHECK_IS_TEXT ();
  * (int *) out = strtoul (to_parse->str, &end, 10);
  if (end == to_parse->str || *end != 0)
    {
      dsk_set_error (error, "error parsing uint from XML node");
      return DSK_FALSE;
    }
  return DSK_TRUE;
}

static DskXml   *
xml_binding_to_xml__uint(DskXmlBindingType *type,
                         const void        *data,
		         DskError         **error)
{
  char buf[64];
  DSK_UNUSED (type); DSK_UNUSED (error);
  snprintf (buf, sizeof (buf), "%u", * (unsigned int *) data);
  return dsk_xml_text_new (buf);
}

DskXmlBindingType dsk_xml_binding_type_uint = 
{
  DSK_TRUE,    /* is_fundamental */
  DSK_TRUE,    /* is_static */
  DSK_FALSE,   /* is_struct */
  DSK_FALSE,   /* is_union */
  0,           /* ref_count */

  sizeof (unsigned int),
  DSK_ALIGNOF_INT,

  &dsk_xml_binding_namespace_builtin,
  "uint",      /* name */
  "uint",      /* ctypename */
  xml_binding_parse__uint,
  xml_binding_to_xml__uint,
  NULL,        /* no clear */
  NULL         /* finalize_type */
};

static dsk_boolean
xml_binding_parse__float(DskXmlBindingType *type,
                        DskXml            *to_parse,
		        void              *out,
		        DskError         **error)
{
  char *end;
  CHECK_IS_TEXT ();
  * (float *) out = strtod (to_parse->str, &end);
  if (end == to_parse->str || *end != 0)
    {
      dsk_set_error (error, "error parsing float from XML node");
      return DSK_FALSE;
    }
  return DSK_TRUE;
}

static DskXml   *
xml_binding_to_xml__float(DskXmlBindingType *type,
                          const void        *data,
		          DskError         **error)
{
  char buf[64];
  DSK_UNUSED (type); DSK_UNUSED (error);
  snprintf (buf, sizeof (buf), "%.6f", * (float *) data);
  return dsk_xml_text_new (buf);
}

DskXmlBindingType dsk_xml_binding_type_float = 
{
  DSK_TRUE,    /* is_fundamental */
  DSK_TRUE,    /* is_static */
  DSK_FALSE,   /* is_struct */
  DSK_FALSE,   /* is_union */
  0,           /* ref_count */

  sizeof (float),
  DSK_ALIGNOF_FLOAT,

  &dsk_xml_binding_namespace_builtin,
  "float",      /* name */
  "float",      /* ctypename */
  xml_binding_parse__float,
  xml_binding_to_xml__float,
  NULL,        /* no clear */
  NULL         /* finalize_type */
};

static dsk_boolean
xml_binding_parse__double(DskXmlBindingType *type,
                        DskXml            *to_parse,
		        void              *out,
		        DskError         **error)
{
  char *end;
  CHECK_IS_TEXT ();
  * (double *) out = strtod (to_parse->str, &end);
  if (end == to_parse->str || *end != 0)
    {
      dsk_set_error (error, "error parsing double from XML node");
      return DSK_FALSE;
    }
  return DSK_TRUE;
}

static DskXml   *
xml_binding_to_xml__double(DskXmlBindingType *type,
                          const void        *data,
		          DskError         **error)
{
  char buf[64];
  DSK_UNUSED (type); DSK_UNUSED (error);
  snprintf (buf, sizeof (buf), "%.16f", * (double *) data);
  return dsk_xml_text_new (buf);
}

DskXmlBindingType dsk_xml_binding_type_double = 
{
  DSK_TRUE,    /* is_fundamental */
  DSK_TRUE,    /* is_static */
  DSK_FALSE,   /* is_struct */
  DSK_FALSE,   /* is_union */
  0,           /* ref_count */

  sizeof (double),
  DSK_ALIGNOF_DOUBLE,

  &dsk_xml_binding_namespace_builtin,
  "double",    /* name */
  "double",    /* ctypename */
  xml_binding_parse__double,
  xml_binding_to_xml__double,
  NULL,        /* no clear */
  NULL         /* finalize_type */
};

static dsk_boolean
xml_binding_parse__string(DskXmlBindingType *type,
                          DskXml            *to_parse,
		          void              *out,
		          DskError         **error)
{
  CHECK_IS_TEXT ();
  * (char **) out = dsk_strdup (to_parse->str);
  return DSK_TRUE;
}

static DskXml   *
xml_binding_to_xml__string(DskXmlBindingType *type,
                           const void        *data,
		           DskError         **error)
{
  const char *str = * (char **) data;
  DSK_UNUSED (type); DSK_UNUSED (error);
  return dsk_xml_text_new (str ? str : "");
}
static void
clear__string(DskXmlBindingType *type,
              void              *data)
{
  DSK_UNUSED (type);
  dsk_free (* (char **) data);
}

DskXmlBindingType dsk_xml_binding_type_string = 
{
  DSK_TRUE,    /* is_fundamental */
  DSK_TRUE,    /* is_static */
  DSK_FALSE,   /* is_struct */
  DSK_FALSE,   /* is_union */
  0,           /* ref_count */

  sizeof (char *),
  DSK_ALIGNOF_POINTER,

  &dsk_xml_binding_namespace_builtin,
  "string",    /* name */
  "char*",    /* ctypename */
  xml_binding_parse__string,
  xml_binding_to_xml__string,
  clear__string,
  NULL         /* finalize_type */
};
static dsk_boolean xml_is_whitespace (DskXml *xml)
{
  const char *at;
  if (xml->type == DSK_XML_ELEMENT)
    return DSK_FALSE;
  at = xml->str;
  while (*at)
    {
      if (!dsk_ascii_isspace (*at))
        return DSK_FALSE;
      at++;
    }
  return DSK_TRUE;
}

static dsk_boolean
is_value_bearing_node (DskXml *child, DskError **error)
{
  unsigned i;
  unsigned n_nonws = 0;
  dsk_assert (child->type == DSK_XML_ELEMENT);
  if (child->n_children <= 1)
    return DSK_TRUE;
  for (i = 0; i < child->n_children; i++)
    if (!xml_is_whitespace (child->children[i]))
      n_nonws++;
  if (n_nonws > 1)
    {
      dsk_set_error (error, "<%s> contains multiple values", child->str);
      return DSK_FALSE;
    }
  return DSK_TRUE;
}
static DskXml *
get_value_from_value_bearing_node (DskXml *child)
{
  if (child->n_children == 0)
    return dsk_xml_empty_text;
  else if (child->n_children == 1)
    return child->children[0];
  else
    {
      unsigned i;
      for (i = 0; i < child->n_children; i++)
        if (!xml_is_whitespace (child->children[i]))
          return child->children[i];
    }
  return NULL;  /* can't happen since is_value_bearing_node() returned TRUE */
}

static DskXmlBindingType *builtin_namespace_types[] =
{
  /* NOTE: THESE MUST BE SORTED BY NAME! */
  &dsk_xml_binding_type_double,
  &dsk_xml_binding_type_float,
  &dsk_xml_binding_type_int,
  &dsk_xml_binding_type_string,
  &dsk_xml_binding_type_uint,
};
DskXmlBindingNamespace dsk_xml_binding_namespace_builtin =
{
  DSK_TRUE,                     /* is_static */
  NULL,                         /* name */
  DSK_N_ELEMENTS (builtin_namespace_types),
  builtin_namespace_types,
  0,                            /* ref_count */
  NULL                          /* types_sorted_by_name */
};

/* structures */
static dsk_boolean
parse_struct_ignore_outer_tag  (DskXmlBindingType *type,
                                DskXml            *to_parse,
                                void              *out,
                                DskError         **error)
{
  DskXmlBindingTypeStruct *s = (DskXmlBindingTypeStruct *) type;
  unsigned *counts;
  unsigned *member_index;
  unsigned i;
  if (to_parse->type != DSK_XML_ELEMENT)
    {
      dsk_set_error (error, "cannot parse structure from string");
      return DSK_FALSE;
    }
  counts = alloca (s->n_members * sizeof (unsigned));
  for (i = 0; i < s->n_members; i++)
    counts[i] = 0;
  member_index = alloca (to_parse->n_children * sizeof (unsigned));
  for (i = 0; i < to_parse->n_children; i++)
    if (to_parse->children[i]->type == DSK_XML_ELEMENT)
      {
        const char *mem_name = to_parse->children[i]->str;
        int mem_no = dsk_xml_binding_type_struct_lookup_member (s, mem_name);
        if (mem_no < 0)
          {
            dsk_set_error (error, "no member %s found", mem_name);
            return DSK_FALSE;
          }
        member_index[i] = mem_no;
        counts[mem_no] += 1;

        if (!is_value_bearing_node (to_parse->children[i], error))
          return DSK_FALSE;
      }
  for (i = 0; i < s->n_members; i++)
    switch (s->members[i].quantity)
      {
      case DSK_XML_BINDING_REQUIRED:
        if (counts[i] == 0)
          {
            dsk_set_error (error, "required member %s not found",
                           s->members[i].name);
            return DSK_FALSE;
          }
        else if (counts[i] > 1)
          {
            dsk_set_error (error, "required member %s specified more than once",
                           s->members[i].name);
            return DSK_FALSE;
          }
        break;
      case DSK_XML_BINDING_OPTIONAL:
        if (counts[i] > 1)
          {
            dsk_set_error (error, "optional member %s specified more than once",
                           s->members[i].name);
            return DSK_FALSE;
          }
        DSK_STRUCT_MEMBER (unsigned, out,
                           s->members[i].quantifier_offset) = counts[i];
        break;
      case DSK_XML_BINDING_REQUIRED_REPEATED:
        if (counts[i] == 0)
          {
            dsk_set_error (error, "repeated required member %s not found",
                           s->members[i].name);
            return DSK_FALSE;
          }
        break;
      case DSK_XML_BINDING_REPEATED:
        break;
      }
  for (i = 0; i < s->n_members; i++)
    switch (s->members[i].quantity)
      {
      case DSK_XML_BINDING_REQUIRED:
        break;
      case DSK_XML_BINDING_OPTIONAL:
        DSK_STRUCT_MEMBER (dsk_boolean, out,
                           s->members[i].quantifier_offset) = counts[i];
        break;
      case DSK_XML_BINDING_REQUIRED_REPEATED:
      case DSK_XML_BINDING_REPEATED:
        DSK_STRUCT_MEMBER (void *, out, s->members[i].offset)
          = dsk_malloc (s->members[i].type->sizeof_instance * counts[i]);
        DSK_STRUCT_MEMBER (dsk_boolean, out,
                           s->members[i].quantifier_offset) = 0;
        break;
      }
  for (i = 0; i < to_parse->n_children; i++)
    if (to_parse->children[i]->type == DSK_XML_ELEMENT)
      {
        unsigned index = member_index[i];
        DskXmlBindingStructMember *member = s->members + index;
        DskXml *value_xml = get_value_from_value_bearing_node (to_parse->children[i]);
        switch (member->quantity)
          {
          case DSK_XML_BINDING_REQUIRED:
          case DSK_XML_BINDING_OPTIONAL:
            if (!member->type->parse (member->type, value_xml,
                                      DSK_STRUCT_MEMBER_P (out, member->offset),
                                      error))
              {
                /* TO CONSIDER: add error info */
                goto error_cleanup;
              }
            break;
          case DSK_XML_BINDING_REPEATED:
          case DSK_XML_BINDING_REQUIRED_REPEATED:
            {
              unsigned *p_idx = DSK_STRUCT_MEMBER_P (out, member->quantifier_offset);
              unsigned idx = *p_idx;
              char *slab = DSK_STRUCT_MEMBER (void *, out, member->offset);
              if (!member->type->parse (member->type, value_xml,
                                        slab + member->type->sizeof_instance * idx,
                                        error))
                {
                  /* TO CONSIDER: add error info */
                  goto error_cleanup;
                }
              *p_idx += 1;
            }
            break;
          }
      }
  return DSK_TRUE;

error_cleanup:
  {
    unsigned j, k;
    for (j = 0; j < i; j++)
      if (to_parse->children[j]->type == DSK_XML_ELEMENT
       && (s->members[member_index[j]].quantity == DSK_XML_BINDING_REQUIRED
        || s->members[member_index[j]].quantity == DSK_XML_BINDING_OPTIONAL))
        {
          DskXmlBindingType *mtype = s->members[member_index[j]].type;
          if (mtype->clear != NULL)
            mtype->clear (mtype, DSK_STRUCT_MEMBER_P (out, s->members[member_index[j]].offset));
        }
    for (j = 0; j < s->n_members; j++)
      if (s->members[j].quantity == DSK_XML_BINDING_REPEATED
       || s->members[j].quantity == DSK_XML_BINDING_REQUIRED_REPEATED)
        {
          unsigned count = DSK_STRUCT_MEMBER (unsigned, out, s->members[j].quantifier_offset);
          char *slab = DSK_STRUCT_MEMBER (char *, out, s->members[j].offset);
          DskXmlBindingType *mtype = s->members[j].type;
          if (mtype->clear != NULL)
            for (k = 0; k < count; k++)
              mtype->clear (mtype, slab + mtype->sizeof_instance * k);
          dsk_free (slab);
        }
  }
  return DSK_FALSE;
}

dsk_boolean dsk_xml_binding_struct_parse (DskXmlBindingType *type,
		                          DskXml            *to_parse,
		                          void              *out,
		                          DskError         **error)
{
  if (!dsk_xml_is_element (to_parse, type->name))
    {
      if (to_parse->type == DSK_XML_ELEMENT)
        dsk_set_error (error, "expected element of type %s.%s, got <%s>",
                       type->ns->name, type->name, to_parse->str);
      else
        dsk_set_error (error, "expected element of type %s.%s, got text",
                       type->ns->name, type->name);
      return DSK_FALSE;
    }

  return parse_struct_ignore_outer_tag (type, to_parse, out, error);
}

static dsk_boolean
struct_members_to_xml        (DskXmlBindingType *type,
                              const void        *strct,
                              unsigned          *n_nodes_out,
                              DskXml          ***nodes_out,
                              DskError         **error)
{
  DskXmlBindingTypeStruct *s = (DskXmlBindingTypeStruct *) type;
  DskXml **children;
  unsigned n_children = 0;
  unsigned i;
  DskXml *subxml;
  for (i = 0; i < s->n_members; i++)
    switch (s->members[i].quantity)
      {
      case DSK_XML_BINDING_REQUIRED:
        n_children++;
        break;
      case DSK_XML_BINDING_OPTIONAL:
        if (DSK_STRUCT_MEMBER (dsk_boolean, strct, s->members[i].quantifier_offset))
          n_children++;
        break;
      case DSK_XML_BINDING_REPEATED:
      case DSK_XML_BINDING_REQUIRED_REPEATED:
        n_children += DSK_STRUCT_MEMBER (unsigned, strct, s->members[i].quantifier_offset);
        break;
      }
  *n_nodes_out = n_children;
  *nodes_out = DSK_NEW_ARRAY (n_children, DskXml *);
  children = *nodes_out;
  n_children = 0;
  for (i = 0; i < s->n_members; i++)
    switch (s->members[i].quantity)
      {
      case DSK_XML_BINDING_REQUIRED:
        subxml = s->members[i].type->to_xml (s->members[i].type,
                                             DSK_STRUCT_MEMBER_P (strct, s->members[i].offset),
                                             error);
        if (subxml == NULL)
          goto error_cleanup;
        children[n_children++] = dsk_xml_new_take_1 (s->members[i].name, subxml);
        break;
      case DSK_XML_BINDING_OPTIONAL:
        if (DSK_STRUCT_MEMBER (dsk_boolean, strct, s->members[i].quantifier_offset))
          {
            subxml = s->members[i].type->to_xml (s->members[i].type,
                                                 DSK_STRUCT_MEMBER_P (strct, s->members[i].offset),
                                                 error);
            if (subxml == NULL)
              goto error_cleanup;
            children[n_children++] = dsk_xml_new_take_1 (s->members[i].name, subxml);
          }
        break;
      case DSK_XML_BINDING_REPEATED:
      case DSK_XML_BINDING_REQUIRED_REPEATED:
        {
          unsigned j, n;
          char *array = DSK_STRUCT_MEMBER (char *, strct, s->members[i].offset);
          n = DSK_STRUCT_MEMBER (unsigned, strct, s->members[i].quantifier_offset);
          for (j = 0; j < n; j++)
            {
              subxml = s->members[i].type->to_xml (s->members[i].type,
                                                   array,
                                                   error);
              children[n_children++] = dsk_xml_new_take_1 (s->members[i].name, subxml);
              array += s->members[i].type->sizeof_instance;
            }

            break;
          }
      }
  dsk_assert (n_children == *n_nodes_out);
  return DSK_TRUE;
error_cleanup:
  for (i = 0; i < n_children; i++)
    dsk_xml_unref (children[i]);
  dsk_free (children);
  return DSK_FALSE;
}
DskXml  *   dsk_xml_binding_struct_to_xml(DskXmlBindingType *type,
		                          const void        *data,
		                          DskError         **error)
{
  unsigned n_children;
  DskXml **children;
  DskXml *rv;
  if (!struct_members_to_xml (type, data, &n_children, &children, error))
    return NULL;
  rv = dsk_xml_new_take_n (type->name, n_children, children);
  dsk_free (children);
  return rv;

}

static void
clear_struct_members (DskXmlBindingType *type,
                      void              *strct)
{
  DskXmlBindingTypeStruct *s = (DskXmlBindingTypeStruct *) type;
  unsigned i;
  for (i = 0; i < s->n_members; i++)
    {
      DskXmlBindingType *mtype = s->members[i].type;
      void *mem = DSK_STRUCT_MEMBER_P (strct, s->members[i].offset);
      switch (s->members[i].quantity)
        {
        case DSK_XML_BINDING_REQUIRED:
          if (mtype->clear != NULL)
            mtype->clear (mtype, mem);
          break;
        case DSK_XML_BINDING_OPTIONAL:
          if (mtype->clear != NULL
           && DSK_STRUCT_MEMBER (dsk_boolean, strct, s->members[i].quantifier_offset))
            mtype->clear (mtype, mem);
          break;
        case DSK_XML_BINDING_REPEATED:
        case DSK_XML_BINDING_REQUIRED_REPEATED:
          {
            if (mtype->clear != NULL)
              {
                char *arr = * (char **) mem;
                unsigned j, n = DSK_STRUCT_MEMBER (unsigned, strct, s->members[i].quantifier_offset);
                for (j = 0; j < n; j++)
                  {
                    mtype->clear (mtype, arr);
                    arr += mtype->sizeof_instance;
                  }
              }
            dsk_free (* (char **) mem);
          }
          break;
        }
    }
}



void        dsk_xml_binding_struct_clear (DskXmlBindingType *type,
		                          void              *data)
{
  clear_struct_members (type, data);
}

static void
add_type_to_namespace (DskXmlBindingNamespace *ns,
                       DskXmlBindingType      *type)
{
  dsk_assert (!ns->is_static);
  if ((ns->n_types & (ns->n_types-1)) == 0)
    {
      unsigned new_size = ns->n_types ? ns->n_types * 2 : 1;
      ns->types = dsk_realloc (ns->types, new_size * sizeof (DskXmlBindingType*));
      ns->types_sorted_by_name = dsk_realloc (ns->types_sorted_by_name, new_size * sizeof (unsigned));
    }
#if 0
  unsigned insert_at_start = 0;
  unsigned insert_at_n = ns->n_types;
  while (insert_at_n > 0)
    {
      unsigned mid = insert_at_start + insert_at_n / 2;
      int rv = strcmp (type->name, ns->types[mid]->name);
      if (rv < 0)
        insert_at_n = mid - insert_at_start;
      else if (rv > 0)
        {
          unsigned new_start = mid + 1;
          unsigned new_n = insert_at_start + insert_at_n - new_start;
          insert_at_start = new_start;
          insert_at_n = new_n;
        }
      else
        {
          dsk_warning ("duplicate type %s in namespace", type->name);
          return;
        }
    }
  if (insert_at_start > 0
      && strcmp (type->name, ns->types[insert_at_start-1]->name) < 0)
    insert_at_start--;
  memmove (ns->types + insert_at_start,
           ns->types + insert_at_start + 1,
           (ns->n_types - insert_at_start) * sizeof (DskXmlBindingType*));
#endif
  ns->types_sorted_by_name[ns->n_types] = ns->n_types;
  ns->types[ns->n_types++] = type;

#define COMPARE_TYPES_BY_NAME(a,b, rv) \
  rv = strcmp (ns->types[a]->name, ns->types[b]->name)
  DSK_QSORT (ns->types_sorted_by_name, unsigned, ns->n_types,
             COMPARE_TYPES_BY_NAME);
#undef COMPARE_TYPES_BY_NAME
}

#if 0
DskXmlBindingTypeStruct *
dsk_xml_binding_type_struct_new (DskXmlBindingNamespace *ns,
                            const char        *struct_name,
                            unsigned           n_members,
                            const DskXmlBindingStructMember *members)
{
  DskXmlBindingTypeStruct *rv;
  unsigned tail_space = n_members * sizeof (DskXmlBindingStructMember);
  unsigned i;
  dsk_warning ("dsk_xml_binding_struct_new: ns=%p, struct_name=%s", ns, struct_name);
  for (i = 0; i < n_members; i++)
    tail_space += strlen (members[i].name) + 1;
  tail_space += strlen (struct_name);
  rv = dsk_malloc (sizeof (DskXmlBindingTypeStruct) + tail_space);
  rv->base_type.is_fundamental = 0;
  rv->base_type.is_static = 0;
  rv->base_type.is_struct = 1;
  rv->base_type.is_union = 0;
  rv->base_type.sizeof_instance = sizeof (void*);
  rv->base_type.alignof_instance = sizeof (void*);
  rv->base_type.ns = ns;
  rv->base_type.parse = dsk_xml_binding_struct_parse;
  rv->base_type.to_xml = dsk_xml_binding_struct_to_xml;
  rv->base_type.clear = dsk_xml_binding_struct_clear;

  rv->n_members = n_members;
  rv->members = (DskXmlBindingStructMember*)(rv + 1);
  char *str_at;
  str_at = (char*)(rv->members + n_members);
  rv->base_type.name = str_at;
  str_at = stpcpy (str_at, struct_name) + 1;
  for (i = 0; i < n_members; i++)
    {
      rv->members[i].name = str_at;
      str_at = stpcpy (str_at, members[i].name) + 1;
    }
  if (ns != NULL)
    add_type_to_namespace (ns, &rv->base_type);
  return rv;
}
#endif

static dsk_boolean
is_empty_element (DskXml *xml)
{
  if (xml->type != DSK_XML_ELEMENT)
    return DSK_FALSE;
  if (xml->n_children == 0)
    return DSK_TRUE;
  if (xml->n_children > 1)
    return DSK_FALSE;
  return dsk_xml_is_whitespace (xml->children[0]);
}

int
dsk_xml_binding_type_struct_lookup_member (DskXmlBindingTypeStruct *s,
                                           const char              *name)
{
  unsigned start = 0, count = s->n_members;
  while (count > 0)
    {
      unsigned mid = start + count / 2;
      int rv = strcmp (s->members[s->members_sorted_by_name[mid]].name, name);
      if (rv < 0)
        {
          unsigned new_start = mid + 1;
          count -= new_start - start;
          start = new_start;
        }
      else if (rv > 0)
        {
          count /= 2;
        }
      else
        return s->members_sorted_by_name[mid];
    }
  return -1;
}

static void
struct__finalize_type (DskXmlBindingType *type)
{
  DskXmlBindingTypeStruct *s = (DskXmlBindingTypeStruct *) type;
  unsigned i;
  dsk_free (s->members_sorted_by_name);
  for (i = 0; i < s->n_members; i++)
    dsk_xml_binding_type_unref (s->members[i].type);
}

DskXmlBindingTypeStruct *
dsk_xml_binding_type_struct_new (DskXmlBindingNamespace *ns,
                                 const char             *name,
                                 unsigned                n_members,
                                 const DskXmlBindingStructMember *members,
                                 DskError              **error)
{
  unsigned max_align = DSK_ALIGNOF_STRUCTURE;
  unsigned i;
  unsigned str_size = name ? (strlen (name) + 1) : 0;
  DskXmlBindingTypeStruct *rv;
  char *str_at;
  unsigned offset = 0;
  for (i = 0; i < n_members; i++)
    str_size += strlen (members[i].name) + 1;
  rv = dsk_malloc0 (sizeof (DskXmlBindingTypeStruct)
                    + sizeof (DskXmlBindingStructMember) * n_members
                    + str_size);
  rv->n_members = n_members;
  rv->members = (DskXmlBindingStructMember*)(rv + 1);
  str_at = (char*)(rv->members + n_members);
  if (name)
    {
      rv->base_type.name = str_at;
      str_at = dsk_stpcpy (str_at, name) + 1;
    }
  rv->base_type.is_struct = 1;
  rv->base_type.ref_count = 1;
  rv->base_type.ns = ns;
  rv->base_type.parse = dsk_xml_binding_struct_parse;
  rv->base_type.to_xml = dsk_xml_binding_struct_to_xml;
  rv->base_type.clear = NULL;
  rv->base_type.finalize_type = struct__finalize_type;
  for (i = 0; i < n_members; i++)
    {
      DskXmlBindingType *mtype = members[i].type;
      rv->members[i].quantity = members[i].quantity;
      rv->members[i].name = str_at;
      str_at = dsk_stpcpy (str_at, members[i].name) + 1;
      rv->members[i].type = dsk_xml_binding_type_ref (mtype);
      if (mtype->clear
       || members[i].quantity == DSK_XML_BINDING_REPEATED
       || members[i].quantity == DSK_XML_BINDING_REQUIRED_REPEATED)
        rv->base_type.clear = dsk_xml_binding_struct_clear;
      switch (members[i].quantity)
        {
        case DSK_XML_BINDING_REQUIRED:
          rv->members[i].quantifier_offset = -1;
          offset = DSK_ALIGN (offset, mtype->alignof_instance);
          rv->members[i].offset = offset;
          offset += mtype->sizeof_instance;
          if (max_align < mtype->alignof_instance)
            max_align = mtype->alignof_instance;
          break;

        case DSK_XML_BINDING_OPTIONAL:
          offset = DSK_ALIGN (offset, DSK_ALIGNOF_INT);
          rv->members[i].quantifier_offset = offset;
          offset += sizeof (dsk_boolean);
          offset = DSK_ALIGN (offset, mtype->alignof_instance);
          rv->members[i].offset = offset;
          offset += mtype->sizeof_instance;
          if (max_align < DSK_ALIGNOF_INT)
            max_align = DSK_ALIGNOF_INT;
          if (max_align < mtype->alignof_instance)
            max_align = mtype->alignof_instance;
          break;

        case DSK_XML_BINDING_REPEATED:
        case DSK_XML_BINDING_REQUIRED_REPEATED:
          offset = DSK_ALIGN (offset, DSK_ALIGNOF_INT);
          rv->members[i].quantifier_offset = offset;
          offset += sizeof (unsigned);
          offset = DSK_ALIGN (offset, DSK_ALIGNOF_POINTER);
          rv->members[i].offset = offset;
          offset += sizeof (void*);
          if (max_align < DSK_ALIGNOF_INT)
            max_align = DSK_ALIGNOF_INT;
          if (max_align < DSK_ALIGNOF_POINTER)
            max_align = DSK_ALIGNOF_POINTER;
          break;
        default:
          assert (0);
        }
    }
  offset = DSK_ALIGN (offset, max_align);
  rv->base_type.sizeof_instance = offset;
  rv->base_type.alignof_instance = max_align;

  rv->members_sorted_by_name = DSK_NEW_ARRAY (n_members, unsigned);
  for (i = 0; i < n_members; i++)
    rv->members_sorted_by_name[i] = i;
#define COMPARE_MEMBERS_BY_NAME(a,b, rv) rv = strcmp (members[a].name, members[b].name)
  DSK_QSORT (rv->members_sorted_by_name, unsigned, n_members,
             COMPARE_MEMBERS_BY_NAME);
#undef COMPARE_MEMBERS_BY_NAME
  for (i = 1; i < n_members; i++)
    if (strcmp (members[rv->members_sorted_by_name[i-1]].name,
                members[rv->members_sorted_by_name[i]].name) == 0)
      {
        dsk_set_error (error, "duplicate member '%s' in struct",
                       members[rv->members_sorted_by_name[i]].name);
        dsk_xml_binding_type_unref (&rv->base_type);
        return NULL;
      }
  if (ns != NULL)
    add_type_to_namespace (ns, &rv->base_type);
  return rv;
}

/* unions */
dsk_boolean dsk_xml_binding_union_parse  (DskXmlBindingType *type,
		                          DskXml            *to_parse,
		                          void              *out,
		                          DskError         **error)
{
  DskXmlBindingTypeUnion *u = (DskXmlBindingTypeUnion *) type;
  int case_i;
  void *union_data = NULL;
  DskXmlBindingType *case_type;
  if (to_parse->type != DSK_XML_ELEMENT)
    {
      dsk_set_error (error, "cannot parse union from string");
      goto error_maybe_add_union_name;
    }
  case_i = dsk_xml_binding_type_union_lookup_case (u, to_parse->str);
  if (case_i < 0)
    {
      dsk_set_error (error, "bad case '%s' in union",
                     to_parse->str);
      goto error_maybe_add_union_name;
    }
  union_data = out;
  case_type = u->cases[case_i].type;
  if (case_type == NULL)
    {
      if (!is_empty_element (to_parse))
        {
          dsk_set_error (error, "case %s has no data", to_parse->str);
          goto error_maybe_add_union_name;
        }
    }
  else if (u->cases[case_i].elide_struct_outer_tag)
    {
      dsk_assert (case_type->is_struct);
      if (!parse_struct_ignore_outer_tag (case_type, to_parse,
                                          ((char*) out) + u->variant_offset,
                                          error))
        goto error_maybe_add_union_name;
    }
  else
    {
      /* find solo child */
      DskXml *subxml = dsk_xml_find_solo_child (to_parse, error);
      if (subxml == NULL)
        goto error_maybe_add_union_name;

      if (!case_type->parse (case_type, subxml,
                             (char*) union_data + u->variant_offset,
                             error))
        {
          goto error_maybe_add_union_name;
        }
    }
  * (DskXmlBindingTypeUnionTag *) union_data = case_i;
  return DSK_TRUE;

error_maybe_add_union_name:
  if (type->name != NULL)
    dsk_add_error_prefix (error, "in %s.%s", type->ns->name, type->name);
  dsk_free (union_data);
  return DSK_FALSE;
}

DskXml  *
dsk_xml_binding_union_to_xml (DskXmlBindingType *type,
                              const void        *data,
                              DskError         **error)
{
  DskXmlBindingTypeUnionTag tag;
  DskXmlBindingTypeUnion *u = (DskXmlBindingTypeUnion *) type;
  void *variant_data = (char *)data + u->variant_offset;
  tag = * (DskXmlBindingTypeUnionTag *) data;
  if (tag >= u->n_cases)
    {
      dsk_set_error (error, "unknown tag %u in union, while converting to XML", tag);
      goto error_maybe_add_union_name;
    }
  if (u->cases[tag].type == NULL)
    {
      return dsk_xml_new_empty (u->cases[tag].name);
    }
  else if (u->cases[tag].elide_struct_outer_tag)
    {
      DskXml **subnodes;
      unsigned n_subnodes;
      DskXml *rv;
      if (!struct_members_to_xml (u->cases[tag].type,
                                  variant_data,
                                  &n_subnodes, &subnodes, error))
        goto error_maybe_add_union_name;
      rv = dsk_xml_new_take_n (u->cases[tag].name, n_subnodes, subnodes);
      dsk_free (subnodes);
      return rv;
    }
  else
    {
      DskXmlBindingType *subtype = u->cases[tag].type;
      DskXml *subxml = subtype->to_xml (subtype, variant_data, error);
      if (subxml == NULL)
        goto error_maybe_add_union_name;
      return dsk_xml_new_take_1 (u->cases[tag].name, subxml);
    }

error_maybe_add_union_name:
  if (type->name)
    dsk_add_error_prefix (error, "in union %s.%s", type->ns->name, type->name);
  return DSK_FALSE;
}

void
dsk_xml_binding_union_clear  (DskXmlBindingType *type,
                              void              *data)
{
  DskXmlBindingTypeUnionTag tag;
  DskXmlBindingTypeUnion *u = (DskXmlBindingTypeUnion *) type;
  void *variant_data = (char *)data + u->variant_offset;
  DskXmlBindingType *case_type;
  tag = * (DskXmlBindingTypeUnionTag *) data;
  dsk_assert (tag < u->n_cases);
  case_type = u->cases[tag].type;
  if (case_type == NULL)
    {
      /* nothing to do */
    }
  else if (u->cases[tag].elide_struct_outer_tag)
    clear_struct_members (case_type, variant_data);
  else if (case_type->clear != NULL)
    case_type->clear (case_type, variant_data);
}
int
dsk_xml_binding_type_union_lookup_case (DskXmlBindingTypeUnion *u,
                                        const char             *name)
{
  unsigned start = 0, count = u->n_cases;
  while (count > 0)
    {
      unsigned mid = start + count / 2;
      int rv = strcmp (u->cases[u->cases_sorted_by_name[mid]].name, name);
      if (rv < 0)
        {
          unsigned new_start = mid + 1;
          count -= new_start - start;
          start = new_start;
        }
      else if (rv > 0)
        {
          count /= 2;
        }
      else
        return u->cases_sorted_by_name[mid];
    }
  return -1;
}

static void
union__finalize_type (DskXmlBindingType *type)
{
  DskXmlBindingTypeUnion *u = (DskXmlBindingTypeUnion *) type;
  unsigned i;
  for (i = 0; i < u->n_cases; i++)
    if (u->cases[i].type)
      dsk_xml_binding_type_unref (u->cases[i].type);
  dsk_free (u->cases_sorted_by_name);
}

DskXmlBindingTypeUnion *
dsk_xml_binding_type_union_new (DskXmlBindingNamespace *ns,
                                const char        *name,
                                unsigned           n_cases,
                                const DskXmlBindingUnionCase *cases,
                                DskError         **error)
{
  unsigned alignof_variant = 1;
  unsigned sizeof_variant = 0;
  unsigned str_size = name ? (strlen (name) + 1) : 0;
  unsigned i;
  DskXmlBindingTypeUnion *rv;
  char *str_at;
  for (i = 0; i < n_cases; i++)
    str_size += strlen (cases[i].name) + 1;
  rv = dsk_malloc0 (sizeof (DskXmlBindingTypeUnion)
                    + sizeof (DskXmlBindingUnionCase) * n_cases
                    + str_size);
  rv->n_cases = n_cases;
  rv->cases = (DskXmlBindingUnionCase*)(rv+1);
  rv->base_type.ns = ns;
  str_at = (char*)(rv->cases + n_cases);
  if (name)
    {
      rv->base_type.name = str_at;
      str_at = dsk_stpcpy (str_at, name) + 1;
    }

  for (i = 0; i < n_cases; i++)
    {
      rv->cases[i].elide_struct_outer_tag = cases[i].elide_struct_outer_tag;
      rv->cases[i].type = cases[i].type;
      rv->cases[i].name = str_at;
      str_at = dsk_stpcpy (str_at, cases[i].name) + 1;
      if (rv->cases[i].type)
        {
          if (cases[i].type->sizeof_instance > sizeof_variant)
            sizeof_variant = cases[i].type->sizeof_instance;
          if (cases[i].type->alignof_instance > alignof_variant)
            alignof_variant = cases[i].type->alignof_instance;
        }
    }
  rv->variant_offset = DSK_ALIGN (sizeof (DskXmlBindingTypeUnionTag), alignof_variant);
  rv->base_type.is_union = 1;
  rv->base_type.ref_count = 1;
  rv->base_type.ns = ns;
  rv->base_type.parse = dsk_xml_binding_union_parse;
  rv->base_type.to_xml = dsk_xml_binding_union_to_xml;
  rv->base_type.clear = NULL;
  rv->base_type.finalize_type = union__finalize_type;
  rv->base_type.sizeof_instance = DSK_ALIGN (rv->variant_offset + sizeof_variant, alignof_variant);
  rv->base_type.alignof_instance = (DSK_ALIGNOF_INT > alignof_variant) ? DSK_ALIGNOF_INT : alignof_variant;

  rv->cases_sorted_by_name = DSK_NEW_ARRAY (n_cases, unsigned);
  for (i = 0; i < n_cases; i++)
    rv->cases_sorted_by_name[i] = i;
#define COMPARE_CASES_BY_NAME(a,b, rv) rv = strcmp (cases[a].name, cases[b].name)
  DSK_QSORT (rv->cases_sorted_by_name, unsigned, n_cases,
             COMPARE_CASES_BY_NAME);
  for (i = 1; i < n_cases; i++)
    if (strcmp (cases[rv->cases_sorted_by_name[i-1]].name,
                cases[rv->cases_sorted_by_name[i]].name) == 0)
      {
        dsk_set_error (error, "duplicate case-name %s in union",
                       cases[rv->cases_sorted_by_name[i]].name);
        dsk_free (rv->cases_sorted_by_name);
        dsk_free (rv);
        return NULL;
      }
  for (i = 0; i < n_cases; i++)
    if (cases[i].type)
      {
        dsk_xml_binding_type_ref (cases[i].type);
        if (cases[i].type->clear)
          rv->base_type.clear = dsk_xml_binding_union_clear;
      }
  if (ns != NULL)
    add_type_to_namespace (ns, &rv->base_type);
  return rv;
}


void
dsk_xml_binding_type_unref (DskXmlBindingType *type)
{
  if (type->is_static)
    return;
  if (--(type->ref_count) == 0)
    {
      if (type->finalize_type != NULL)
        type->finalize_type (type);
      dsk_free (type);
    }
}

DskXmlBindingType *
dsk_xml_binding_type_ref (DskXmlBindingType *type)
{
  if (type->is_static)
    return type;
  ++(type->ref_count);
  return type;
}
