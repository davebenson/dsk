#include "dsk.h"

/* --- decoder --- */
typedef struct _DskXmlEscaperClass DskXmlEscaperClass;
struct _DskXmlEscaperClass
{
  DskOctetFilterClass base_class;
};
typedef struct _DskXmlEscaper DskXmlEscaper;
struct _DskXmlEscaper
{
  DskOctetFilter base_instance;
};

#define dsk_xml_escaper_init NULL
#define dsk_xml_escaper_finalize NULL

static dsk_boolean
dsk_xml_escaper_process (DskOctetFilter *filter,
                        DskBuffer      *out,
                        unsigned        in_length,
                        const uint8_t  *in_data,
                        DskError      **error)
{
  DSK_UNUSED (filter);
  DSK_UNUSED (error);
  while (in_length > 0)
    {
      unsigned n = 0;
      while (n < in_length && in_data[n] != '<' && in_data[n] != '>'
             && in_data[n] != '&')
        n++;
      if (n)
        dsk_buffer_append (out, n, in_data);
      in_data += n;
      in_length -= n;
      while (in_length > 0)
        if (*in_data == '<')
          {
            in_data++;
            in_length--;
            dsk_buffer_append (out, 4, "&lt;");
          }
        else if (*in_data == '>')
          {
            in_data++;
            in_length--;
            dsk_buffer_append (out, 4, "&gt;");
          }
        else if (*in_data == '&')
          {
            in_data++;
            in_length--;
            dsk_buffer_append (out, 5, "&amp;");
          }
        else
          break;
    }
  return DSK_TRUE;
}
#define dsk_xml_escaper_finish NULL
DSK_OCTET_FILTER_SUBCLASS_DEFINE(static, DskXmlEscaper, dsk_xml_escaper);
DskOctetFilter *dsk_xml_escaper_new            (void)
{
  return dsk_object_new (&dsk_xml_escaper_class);
}

DskOctetFilter *dsk_xml_unescaper_new  (void);
