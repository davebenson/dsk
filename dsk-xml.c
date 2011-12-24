#include <stdlib.h>
#include <string.h>
#include "dsk.h"

DskXml *dsk_xml_ref   (DskXml *xml)
{
  ++(xml->ref_count);
  return xml;
}

static inline void
dsk_xml_filename_unref (DskXmlFilename *filename)
{
  if (--(filename->ref_count) == 0)
    dsk_free (filename);
}

void    dsk_xml_unref (DskXml *xml)
{
restart:
  if (--(xml->ref_count) == 0)
    {
      if (xml->filename)
        dsk_xml_filename_unref (xml->filename);
      if (xml->type == DSK_XML_ELEMENT)
        {
          unsigned n = xml->n_children;
          DskXml *tail = NULL;
          unsigned i;
          for (i = 0; i < n; i++)
            {
              if (xml->children[i]->ref_count > 1)
                --(xml->children[i]->ref_count);
              else if (tail)
                dsk_xml_unref (xml->children[i]);
              else
                tail = xml->children[i];
            }
          if (tail)
            {
              dsk_free (xml);
              xml = tail;
              goto restart;
            }
        }
      dsk_free (xml);
    }
}

/* --- TODO: add wad of constructors --- */
DskXml *dsk_xml_text_new_len (unsigned length,
                              const char *data)
{
  DskXml *xml = dsk_malloc (length + 1 + sizeof (DskXml));
  memcpy (xml + 1, data, length);
  xml->str = (char*) (xml + 1);
  xml->str[length] = 0;
  xml->filename = NULL;
  xml->line_no = 0;
  xml->type = DSK_XML_TEXT;
  xml->ref_count = 1;
  return xml;
}
DskXml *dsk_xml_text_new     (const char *data)
{
  return dsk_xml_text_new_len (strlen (data), data);
}
DskXml dsk_xml_empty_text_global = {
  0,             /* line_no */
  NULL,          /* filename */
  DSK_XML_TEXT,  /* type */
  1,             /* ref_count */
  "",            /* str */
  NULL,          /* attrs */
  0,             /* n_children */
  NULL,          /* children */
};

DskXml *dsk_xml_comment_new_len (unsigned length,
                                 const char *text)
{
  DskXml *rv = dsk_xml_text_new_len (length, text);
  rv->type = DSK_XML_COMMENT;
  return rv;
}
static DskXml *condense_text_nodes (unsigned n, DskXml **xml)
{
  unsigned length = 0;
  char *at;
  unsigned i;
  DskXml *rv;
  dsk_assert (n > 1);
  for (i = 0; i < n; i++)
    length += strlen (xml[i]->str);
  rv = dsk_malloc (sizeof (DskXml) + length + 1);
  rv->str = (char*)(rv+1);
  rv->filename = NULL;
  rv->line_no = 0;
  rv->type = DSK_XML_TEXT;
  rv->ref_count = 1;
  at = rv->str;
  for (i = 0; i < n; i++)
    {
      at = dsk_stpcpy (at, xml[i]->str);
      dsk_xml_unref (xml[i]);
    }
  return rv;
}

DskXml *dsk_xml_new_empty   (const char *name)
{
  return _dsk_xml_new_elt_parse (0, strlen (name) + 1, name,
                                 0, NULL, DSK_FALSE);
}

DskXml *
dsk_xml_text_child_new (const char *name,
                        const char *contents)
{
  return dsk_xml_new_take_1 (name, dsk_xml_text_new (contents));
}

DskXml *dsk_xml_new_take_1   (const char *name,
                              DskXml     *child)
{
  return _dsk_xml_new_elt_parse (0, strlen (name) + 1, name,
                                 1, &child,
                                 DSK_FALSE);
}

DskXml *dsk_xml_new_take_n   (const char *name,
                              unsigned    n_children,
                              DskXml    **children)
{
  return _dsk_xml_new_elt_parse (0, strlen (name) + 1, name,
                                 n_children, children,
                                 DSK_TRUE);
}
DskXml *dsk_xml_new_take_list (const char *name,
                               DskXml     *first_or_null,
                               ...)
{
  va_list args;
  unsigned count = 0;
  DskXml *at;
  DskXml **children;

  va_start (args, first_or_null);
  for (at = first_or_null; at != NULL; at = va_arg (args, DskXml *))
    count++;
  va_end (args);

  children = alloca (sizeof (DskXml *) * count);
  count = 0;
  va_start (args, first_or_null);
  for (at = first_or_null; at != NULL; at = va_arg (args, DskXml *))
    children[count++] = at;
  va_end (args);

  return dsk_xml_new_take_n (name, count, children);
}

DskXml *_dsk_xml_new_elt_parse (unsigned n_attrs,
                                unsigned name_kv_space,
                                const char *name_and_attrs,
                                unsigned n_children,
                                DskXml **children,
                                dsk_boolean condense_text)
{
  DskXml *rv;
  unsigned i;
  char *at;
  if (condense_text && n_children > 1)
    {
      for (i = 0; i < n_children - 1; i++)
        if (children[i]->type == DSK_XML_TEXT
         && children[i+1]->type == DSK_XML_TEXT)
          break;
      if (i < n_children - 1)
        {
          unsigned o = i;
          while (i < n_children)
            {
              /* Condense more than one text node together */
              unsigned start_run = i;
              i += 2;
              while (i < n_children && children[i]->type == DSK_XML_TEXT)
                i++;
              children[o++] = condense_text_nodes (i - start_run,
                                                   children + start_run);

              /* Passthrough uncondensable items */
              while (i < n_children
                   && (children[i]->type != DSK_XML_TEXT
                    || i == n_children-1
                    || children[i+1]->type != DSK_XML_TEXT))
                children[o++] = children[i++];
            }
          n_children = o;
        }
    }
  rv = dsk_malloc (sizeof (char *) * (n_attrs * 2 + 1)
                           + sizeof (DskXml *) * n_children
                           + name_kv_space
                           + sizeof (DskXml));
  rv->line_no = 0;
  rv->filename = NULL;
  rv->type = DSK_XML_ELEMENT;
  rv->ref_count = 1;
  rv->n_children = n_children;
  rv->attrs = (char **) (rv + 1);
  rv->children = (DskXml **) (rv->attrs + n_attrs * 2 + 1);
  rv->str = (char*) (rv->children + n_children);
  memcpy (rv->str, name_and_attrs, name_kv_space);
  at = strchr (rv->str, 0) + 1;
  for (i = 0; i < 2*n_attrs; i++)
    {
      rv->attrs[i] = at;
      at = strchr (at, 0) + 1;
    }
  rv->attrs[i] = NULL;
  memcpy (rv->children, children, sizeof(DskXml *) * n_children);
  return rv;
}

void _dsk_xml_set_position (DskXml *xml,
                            DskXmlFilename *filename,
                            unsigned line_no)
{
  dsk_assert (xml->filename == NULL);
  xml->filename = filename;
  filename->ref_count++;
  xml->line_no = line_no;
}

const char *dsk_xml_find_attr (const DskXml *xml,
                               const char *name)
{
  unsigned i;
  if (xml->type == DSK_XML_ELEMENT)
    for (i = 0; xml->attrs[2*i] != NULL; i++)
      if (strcmp (xml->attrs[2*i], name) == 0)
        return xml->attrs[2*i+1];
  return NULL;
}

static void
get_all_text_to_buffer (const DskXml *xml,
                        DskBuffer *out)
{
  if (xml->type == DSK_XML_TEXT)
    dsk_buffer_append_string (out, xml->str);
  else if (xml->type == DSK_XML_ELEMENT)
    {
      unsigned i;
      for (i = 0; i < xml->n_children; i++)
        get_all_text_to_buffer (xml->children[i], out);
    }
}

char *dsk_xml_get_all_text (const DskXml *xml)
{
  DskBuffer buffer = DSK_BUFFER_INIT;
  char *rv;
  get_all_text_to_buffer (xml, &buffer);
  rv = dsk_malloc (buffer.size + 1);
  rv[buffer.size] = 0;
  dsk_buffer_read (&buffer, buffer.size, rv);
  return rv;
}

dsk_boolean
dsk_xml_is_element (const DskXml *xml, const char *name)
{
  return xml->type == DSK_XML_ELEMENT && strcmp (xml->str, name) == 0;
}
dsk_boolean
dsk_xml_is_whitespace (const DskXml *xml)
{
  const char *at;
  if (xml->type != DSK_XML_TEXT)
    return DSK_TRUE;
  at = xml->str;
  dsk_utf8_skip_whitespace (&at);
  return (*at == 0);
}
DskXml *
dsk_xml_find_solo_child (DskXml *xml,
                         DskError **error)
{
  if (xml->type != DSK_XML_ELEMENT)
    {
      dsk_set_error (error, "text or comment node has no children");
      return NULL;
    }
  switch (xml->n_children)
    {
    case 0: return dsk_xml_empty_text;
    case 1: return xml->children[0];
    case 2: if (dsk_xml_is_whitespace (xml->children[0]))
              return xml->children[1];
            if (dsk_xml_is_whitespace (xml->children[1]))
              return xml->children[0];
            break;
    case 3: if (dsk_xml_is_whitespace (xml->children[0])
             && dsk_xml_is_whitespace (xml->children[2]))
              return xml->children[1];
            break;
    }
  dsk_set_error (error, "multiple children under <%s>", xml->str);
  return NULL;
}
