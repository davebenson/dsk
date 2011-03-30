/* examples/generated/wikiformats.h -- generated by dsk-make-xml-binding */
/* DO NOT HAND EDIT - changes will be lost */


#ifndef DSK_H__INCLUDED
#include <dsk/dsk.h>
#endif
#include <stddef.h>
typedef struct _Wikiformats__Contributor Wikiformats__Contributor;
typedef struct _Wikiformats__Revision Wikiformats__Revision;
typedef struct _Wikiformats__Page Wikiformats__Page;


/* Enums for Unions and Enumerations */


/* Structures and Unions */
struct _Wikiformats__Contributor
{
  char* username;
  int id;
};
#define wikiformats__contributor__type ((DskXmlBindingType*)(&wikiformats__contributor__descriptor))
DskXml    * wikiformats__contributor__to_xml
                      (const Wikiformats__Contributor *source,
                       DskError **error);
dsk_boolean wikiformats__contributor__parse
                      (DskXml *source,
                       Wikiformats__Contributor *dest,
                       DskError **error);
void        wikiformats__contributor__clear (Wikiformats__Contributor *to_clear);
struct _Wikiformats__Revision
{
  int id;
  char* timestamp;
  dsk_boolean has_contributor;
  Wikiformats__Contributor contributor;
  char* minor;
  char* comment;
  char* text;
};
#define wikiformats__revision__type ((DskXmlBindingType*)(&wikiformats__revision__descriptor))
DskXml    * wikiformats__revision__to_xml
                      (const Wikiformats__Revision *source,
                       DskError **error);
dsk_boolean wikiformats__revision__parse
                      (DskXml *source,
                       Wikiformats__Revision *dest,
                       DskError **error);
void        wikiformats__revision__clear (Wikiformats__Revision *to_clear);
struct _Wikiformats__Page
{
  char* title;
  int id;
  char* redirect;
  Wikiformats__Revision revision;
};
#define wikiformats__page__type ((DskXmlBindingType*)(&wikiformats__page__descriptor))
DskXml    * wikiformats__page__to_xml
                      (const Wikiformats__Page *source,
                       DskError **error);
dsk_boolean wikiformats__page__parse
                      (DskXml *source,
                       Wikiformats__Page *dest,
                       DskError **error);
void        wikiformats__page__clear (Wikiformats__Page *to_clear);


/* Namespace Declaration */
#define wikiformats__namespace ((DskXmlBindingNamespace*)(&wikiformats__descriptor))




/* Private */
extern const DskXmlBindingNamespace wikiformats__descriptor;
extern const DskXmlBindingTypeStruct wikiformats__contributor__descriptor;
extern const DskXmlBindingTypeStruct wikiformats__revision__descriptor;
extern const DskXmlBindingTypeStruct wikiformats__page__descriptor;