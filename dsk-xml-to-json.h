/* TODO: support attributes */

typedef struct _DskXmlToJsonConverter DskXmlToJsonConverter;
struct _DskXmlToJsonConverter
{
...
};


DskXmlToJsonConverter *dsk_xml_to_json_converter_new (DskXmlToJsonConverterFlags);

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

DskJsonValue   *dsk_xml_to_json_convert              (DskXmlToJsonConverter *,
                                                      const DskXml  *xml,
						      DskError     **error);
