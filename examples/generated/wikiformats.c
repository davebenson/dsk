/* examples/generated/wikiformats.c -- generated by dsk-make-xml-binding */
/* DO NOT HAND EDIT - changes will be lost */


#include "wikiformats.h"


/* === Anonymous Structures Inside Unions === */


/* Structures and Unions */
static const DskXmlBindingStructMember wikiformats__contributor__members[] =
{
  {
    DSK_XML_BINDING_REQUIRED,
    "username",
    dsk_xml_binding__string__type,
    0,
    offsetof (Wikiformats__Contributor, username)
  },
  {
    DSK_XML_BINDING_REQUIRED,
    "id",
    dsk_xml_binding__int__type,
    0,
    offsetof (Wikiformats__Contributor, id)
  },
};
static const DskXmlBindingStructMember wikiformats__revision__members[] =
{
  {
    DSK_XML_BINDING_REQUIRED,
    "id",
    dsk_xml_binding__int__type,
    0,
    offsetof (Wikiformats__Revision, id)
  },
  {
    DSK_XML_BINDING_REQUIRED,
    "timestamp",
    dsk_xml_binding__string__type,
    0,
    offsetof (Wikiformats__Revision, timestamp)
  },
  {
    DSK_XML_BINDING_OPTIONAL,
    "contributor",
    wikiformats__contributor__type,
    offsetof (Wikiformats__Revision, has_contributor),
    offsetof (Wikiformats__Revision, contributor)
  },
  {
    DSK_XML_BINDING_REQUIRED,
    "minor",
    dsk_xml_binding__string__type,
    0,
    offsetof (Wikiformats__Revision, minor)
  },
  {
    DSK_XML_BINDING_REQUIRED,
    "comment",
    dsk_xml_binding__string__type,
    0,
    offsetof (Wikiformats__Revision, comment)
  },
  {
    DSK_XML_BINDING_REQUIRED,
    "text",
    dsk_xml_binding__string__type,
    0,
    offsetof (Wikiformats__Revision, text)
  },
};
static const DskXmlBindingStructMember wikiformats__page__members[] =
{
  {
    DSK_XML_BINDING_REQUIRED,
    "title",
    dsk_xml_binding__string__type,
    0,
    offsetof (Wikiformats__Page, title)
  },
  {
    DSK_XML_BINDING_REQUIRED,
    "id",
    dsk_xml_binding__int__type,
    0,
    offsetof (Wikiformats__Page, id)
  },
  {
    DSK_XML_BINDING_REQUIRED,
    "redirect",
    dsk_xml_binding__string__type,
    0,
    offsetof (Wikiformats__Page, redirect)
  },
  {
    DSK_XML_BINDING_REQUIRED,
    "revision",
    wikiformats__revision__type,
    0,
    offsetof (Wikiformats__Page, revision)
  },
};


/* Descriptors */
struct Wikiformats__Contributor__AlignmentTest {
  char a;
  Wikiformats__Contributor b;
};
const unsigned wikiformats__contributor__members_sorted_by_name[] =
{
  1, /* members[1] = "id" */
  0, /* members[0] = "username" */
};
const DskXmlBindingTypeStruct wikiformats__contributor__descriptor =
{
  {
    DSK_FALSE,    /* !is_fundamental */
    DSK_TRUE,     /* is_static */
    DSK_TRUE,    /* is_struct */
    DSK_FALSE,     /* is_union */
    0,            /* ref_count */
    sizeof (Wikiformats__Contributor),
    offsetof (struct Wikiformats__Contributor__AlignmentTest, b),
    wikiformats__namespace,
    "Contributor",
    NULL,    /* ctypename==typename */
    dsk_xml_binding_struct_parse,
    dsk_xml_binding_struct_to_xml,
    dsk_xml_binding_struct_clear,
    NULL           /* finalize_type */
  },
  2,
  (DskXmlBindingStructMember *) wikiformats__contributor__members,
  (unsigned *) wikiformats__contributor__members_sorted_by_name
};
struct Wikiformats__Revision__AlignmentTest {
  char a;
  Wikiformats__Revision b;
};
const unsigned wikiformats__revision__members_sorted_by_name[] =
{
  4, /* members[4] = "comment" */
  2, /* members[2] = "contributor" */
  0, /* members[0] = "id" */
  3, /* members[3] = "minor" */
  5, /* members[5] = "text" */
  1, /* members[1] = "timestamp" */
};
const DskXmlBindingTypeStruct wikiformats__revision__descriptor =
{
  {
    DSK_FALSE,    /* !is_fundamental */
    DSK_TRUE,     /* is_static */
    DSK_TRUE,    /* is_struct */
    DSK_FALSE,     /* is_union */
    0,            /* ref_count */
    sizeof (Wikiformats__Revision),
    offsetof (struct Wikiformats__Revision__AlignmentTest, b),
    wikiformats__namespace,
    "Revision",
    NULL,    /* ctypename==typename */
    dsk_xml_binding_struct_parse,
    dsk_xml_binding_struct_to_xml,
    dsk_xml_binding_struct_clear,
    NULL           /* finalize_type */
  },
  6,
  (DskXmlBindingStructMember *) wikiformats__revision__members,
  (unsigned *) wikiformats__revision__members_sorted_by_name
};
struct Wikiformats__Page__AlignmentTest {
  char a;
  Wikiformats__Page b;
};
const unsigned wikiformats__page__members_sorted_by_name[] =
{
  1, /* members[1] = "id" */
  2, /* members[2] = "redirect" */
  3, /* members[3] = "revision" */
  0, /* members[0] = "title" */
};
const DskXmlBindingTypeStruct wikiformats__page__descriptor =
{
  {
    DSK_FALSE,    /* !is_fundamental */
    DSK_TRUE,     /* is_static */
    DSK_TRUE,    /* is_struct */
    DSK_FALSE,     /* is_union */
    0,            /* ref_count */
    sizeof (Wikiformats__Page),
    offsetof (struct Wikiformats__Page__AlignmentTest, b),
    wikiformats__namespace,
    "Page",
    NULL,    /* ctypename==typename */
    dsk_xml_binding_struct_parse,
    dsk_xml_binding_struct_to_xml,
    dsk_xml_binding_struct_clear,
    NULL           /* finalize_type */
  },
  4,
  (DskXmlBindingStructMember *) wikiformats__page__members,
  (unsigned *) wikiformats__page__members_sorted_by_name
};


/* The Namespace */
static const unsigned wikiformats__type_sorted_by_name[] =
{
  0,   /* types[0] = Contributor */
  2,   /* types[2] = Page */
  1,   /* types[1] = Revision */
};
static const DskXmlBindingType *wikiformats__type_array[] =
{
  wikiformats__contributor__type,
  wikiformats__revision__type,
  wikiformats__page__type,
};

const DskXmlBindingNamespace wikiformats__descriptor =
{
  DSK_TRUE,              /* is_static */
  "wikiformats",
  3,               /* n_types */
  (DskXmlBindingType **) wikiformats__type_array,
  0,                     /* ref_count */
  wikiformats__type_sorted_by_name
};
DskXml    * wikiformats__contributor__to_xml
                      (const Wikiformats__Contributor *source,
                       DskError **error)
{
  return dsk_xml_binding_struct_to_xml (wikiformats__contributor__type, source, error);
}
dsk_boolean wikiformats__contributor__parse
                      (DskXml *source,
                       Wikiformats__Contributor *dest,
                       DskError **error)
{
  return dsk_xml_binding_struct_parse (wikiformats__contributor__type, source, dest, error);
}
void        wikiformats__contributor__clear (Wikiformats__Contributor *to_clear)
{
  dsk_xml_binding_struct_clear (wikiformats__contributor__type, to_clear);
}
DskXml    * wikiformats__revision__to_xml
                      (const Wikiformats__Revision *source,
                       DskError **error)
{
  return dsk_xml_binding_struct_to_xml (wikiformats__revision__type, source, error);
}
dsk_boolean wikiformats__revision__parse
                      (DskXml *source,
                       Wikiformats__Revision *dest,
                       DskError **error)
{
  return dsk_xml_binding_struct_parse (wikiformats__revision__type, source, dest, error);
}
void        wikiformats__revision__clear (Wikiformats__Revision *to_clear)
{
  dsk_xml_binding_struct_clear (wikiformats__revision__type, to_clear);
}
DskXml    * wikiformats__page__to_xml
                      (const Wikiformats__Page *source,
                       DskError **error)
{
  return dsk_xml_binding_struct_to_xml (wikiformats__page__type, source, error);
}
dsk_boolean wikiformats__page__parse
                      (DskXml *source,
                       Wikiformats__Page *dest,
                       DskError **error)
{
  return dsk_xml_binding_struct_parse (wikiformats__page__type, source, dest, error);
}
void        wikiformats__page__clear (Wikiformats__Page *to_clear)
{
  dsk_xml_binding_struct_clear (wikiformats__page__type, to_clear);
}
