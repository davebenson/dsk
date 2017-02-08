typedef struct _DskXmlFilename DskXmlFilename;
typedef struct _DskXml DskXml;

typedef enum
{
  DSK_XML_ELEMENT,
  DSK_XML_TEXT,
  DSK_XML_COMMENT
} DskXmlType;

struct _DskXmlFilename
{
  unsigned ref_count;
  char *filename;
};

struct _DskXml
{
  /* all fields public and read-only */

  unsigned line_no;
  DskXmlFilename *filename;
  DskXmlType type;
  unsigned ref_count;
  char *str;		/* name for ELEMENT, text for COMMENT and TEXT */
  char **attrs;		/* key-value pairs */
  unsigned n_children;
  DskXml **children;
};

DskXml *dsk_xml_ref   (DskXml *xml);
void    dsk_xml_unref (DskXml *xml);

/* --- TODO: add wad of constructors --- */
DskXml *dsk_xml_text_new_len (unsigned length,
                              const char *data);
DskXml *dsk_xml_text_new     (const char *str);

DskXml *dsk_xml_new_take_1   (const char *name,
                              DskXml     *child);
DskXml *dsk_xml_new_take_2   (const char *name,
                              DskXml     *child1,
                              DskXml     *child2);
DskXml *dsk_xml_text_child_new (const char *name,
                                const char *contents);

/* Create an empty xml node */
DskXml *dsk_xml_new_empty    (const char *name);

/* NOTE: we do NOT take the actual array 'children', just every element
   (every XML node) of it. */
DskXml *dsk_xml_new_take_n   (const char *name,
                              unsigned    n_children,
                              DskXml    **children);

DskXml *dsk_xml_new_take_list (const char *name,
                               DskXml     *first_or_null,
                               ...);

extern DskXml dsk_xml_empty_text_global;
#define dsk_xml_empty_text   (&dsk_xml_empty_text_global)

/* --- comments (not recommended much) --- */
DskXml *dsk_xml_comment_new_len (unsigned length,
                                 const char *text);


/* --- xml lookup functions --- */
const char *dsk_xml_find_attr (const DskXml *xml,
                               const char   *name);

dsk_boolean dsk_xml_is_whitespace (const DskXml *xml);
char *dsk_xml_get_all_text (const DskXml *xml);
dsk_boolean dsk_xml_is_element (const DskXml *xml, const char *name);
DskXml *dsk_xml_find_solo_child (DskXml *, DskError **error);

/* --- simple (namespace free) serialization --- */
void dsk_xml_write_to_buffer (const DskXml *xml, DskBuffer *buffer);

/* FOR INTERNAL USE ONLY: create an xml node from a packed set of
   attributes, and a set of children, which we will take ownership of.
   We will do text-node compacting.
 */
DskXml *_dsk_xml_new_elt_parse (unsigned n_attrs,
                                unsigned name_kv_space,
                                const char *name_and_attrs,
                                unsigned n_children,
                                DskXml **children,
                                dsk_boolean condense_text_nodes);
void _dsk_xml_set_position (DskXml *xml,
                            DskXmlFilename *filename,
                            unsigned line_no);
