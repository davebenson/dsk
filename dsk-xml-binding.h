
typedef struct _DskXmlBindingType DskXmlBindingType;
typedef struct _DskXmlBinding DskXmlBinding;
typedef struct _DskXmlBindingStructMember DskXmlBindingStructMember;
typedef struct _DskXmlBindingTypeStruct DskXmlBindingTypeStruct;
typedef struct _DskXmlBindingUnionCase DskXmlBindingUnionCase;
typedef struct _DskXmlBindingTypeUnion DskXmlBindingTypeUnion;
typedef struct _DskXmlBindingNamespace DskXmlBindingNamespace;

typedef enum
{
  DSK_XML_BINDING_REQUIRED,
  DSK_XML_BINDING_OPTIONAL,
  DSK_XML_BINDING_REPEATED,
  DSK_XML_BINDING_REQUIRED_REPEATED
} DskXmlBindingQuantity;


struct _DskXmlBindingType
{
  unsigned is_fundamental : 1;
  unsigned is_static : 1;
  unsigned is_struct : 1;
  unsigned is_union : 1;

  unsigned ref_count;           /* only used if !is_static */

  unsigned sizeof_instance;
  unsigned alignof_instance;
  DskXmlBindingNamespace *ns;
  char *name;
  char *ctypename;              /* usually NULL */

  /* virtual functions */
  dsk_boolean (*parse)(DskXmlBindingType *type,
                       DskXml            *to_parse,
		       void              *out,
		       DskError         **error);
  DskXml   *  (*to_xml)(DskXmlBindingType *type,
                        const void        *data,
		        DskError         **error);
  void        (*clear) (DskXmlBindingType *type,
		        void              *out);

  /* to be invoked by dsk_xml_binding_unref(); only used on non-static types */
  void        (*finalize_type)(DskXmlBindingType *);
};


struct _DskXmlBindingNamespace
{
  dsk_boolean is_static;
  char *name;
  unsigned n_types;
  DskXmlBindingType **types;
  unsigned ref_count;

  unsigned *types_sorted_by_name;
};

DskXmlBindingNamespace *dsk_xml_binding_namespace_new (const char *name);
void dsk_xml_binding_namespace_unref (DskXmlBindingNamespace *ns);

DskXmlBinding *dsk_xml_binding_new (void);
void           dsk_xml_binding_add_searchpath (DskXmlBinding *binding,
                                               const char    *path,
                                               const char    *ns_separator);
DskXmlBindingNamespace*
               dsk_xml_binding_get_ns         (DskXmlBinding *binding,
                                               const char    *name,
                                               DskError     **error);
DskXmlBindingNamespace*
               dsk_xml_binding_try_ns         (DskXmlBinding *binding,
                                               const char    *name);
DskXmlBindingType *dsk_xml_binding_namespace_lookup (DskXmlBindingNamespace *,
                                                     const char *name);
void           dsk_xml_binding_free (DskXmlBinding *);

struct _DskXmlBindingStructMember
{
  DskXmlBindingQuantity quantity;
  char *name;
  DskXmlBindingType *type;
  unsigned quantifier_offset;
  unsigned offset;
};
struct _DskXmlBindingTypeStruct
{
  DskXmlBindingType base_type;
  unsigned n_members;
  DskXmlBindingStructMember *members;
  unsigned *members_sorted_by_name;
};
DskXmlBindingTypeStruct *dsk_xml_binding_type_struct_new (DskXmlBindingNamespace *ns,
                                                 const char        *struct_name,
                                                 unsigned           n_members,
                                   const DskXmlBindingStructMember *members,
                                                 DskError         **error);

int dsk_xml_binding_type_struct_lookup_member (DskXmlBindingTypeStruct *type,
                                               const char              *name);

typedef unsigned int DskXmlBindingTypeUnionTag;
struct _DskXmlBindingUnionCase
{
  char *name;
  dsk_boolean elide_struct_outer_tag;
  DskXmlBindingType *type;
};
struct _DskXmlBindingTypeUnion
{
  DskXmlBindingType base_type;
  unsigned variant_offset;
  unsigned n_cases;
  DskXmlBindingUnionCase *cases;
  unsigned *cases_sorted_by_name;
};

DskXmlBindingTypeUnion *dsk_xml_binding_type_union_new (DskXmlBindingNamespace *ns,
                                                 const char        *union_name,
                                                 unsigned           n_cases,
                                   const DskXmlBindingUnionCase *cases,
                                                 DskError         **error);

int dsk_xml_binding_type_union_lookup_case (DskXmlBindingTypeUnion *type,
                                            const char              *name);
int dsk_xml_binding_type_union_lookup_case_by_tag (DskXmlBindingTypeUnion *type,
                                            DskXmlBindingTypeUnionTag tag);

DskXmlBindingType * dsk_xml_binding_type_ref (DskXmlBindingType *type);
void dsk_xml_binding_type_unref (DskXmlBindingType *type);

/* --- fundamental types --- */
extern DskXmlBindingType dsk_xml_binding_type_int;
extern DskXmlBindingType dsk_xml_binding_type_uint;
extern DskXmlBindingType dsk_xml_binding_type_float;
extern DskXmlBindingType dsk_xml_binding_type_double;
extern DskXmlBindingType dsk_xml_binding_type_string;
extern DskXmlBindingNamespace dsk_xml_binding_namespace_builtin;

#define dsk_xml_binding__int__type     (&dsk_xml_binding_type_int)
#define dsk_xml_binding__uint__type    (&dsk_xml_binding_type_uint)
#define dsk_xml_binding__float__type   (&dsk_xml_binding_type_float)
#define dsk_xml_binding__double__type  (&dsk_xml_binding_type_double)
#define dsk_xml_binding__string__type  (&dsk_xml_binding_type_string)

/* --- for generated code --- */
dsk_boolean dsk_xml_binding_struct_parse (DskXmlBindingType *type,
		                          DskXml            *to_parse,
		                          void              *out,
		                          DskError         **error);
DskXml  *   dsk_xml_binding_struct_to_xml(DskXmlBindingType *type,
		                          const void        *data,
		                          DskError         **error);
void        dsk_xml_binding_struct_clear (DskXmlBindingType *type,
		                          void              *out);
dsk_boolean dsk_xml_binding_union_parse  (DskXmlBindingType *type,
		                          DskXml            *to_parse,
		                          void              *out,
		                          DskError         **error);
DskXml  *   dsk_xml_binding_union_to_xml (DskXmlBindingType *type,
		                          const void        *data,
		                          DskError         **error);
void        dsk_xml_binding_union_clear  (DskXmlBindingType *type,
		                          void              *out);
