/* Miscellaneous MIME Support Functions */

typedef struct _DskMimeKeyValueInplace DskMimeKeyValueInplace;
struct _DskMimeKeyValueInplace
{
  const char *key_start, *key_end;
  const char *value_start, *value_end;
};
dsk_boolean dsk_mime_key_values_scan (const char *str,
                                      unsigned    max_kv,
                                      DskMimeKeyValueInplace *kv,
                                      unsigned   *n_kv_out,
                                      DskError  **error);

/* See RFC 2183: Content-Disposition header type;  i guess;
   these fields are far from exhaustive;
   they are the ones needed by form processing (i'm
   not sure even 'id' is used) */
typedef struct _DskMimeContentDisposition DskMimeContentDisposition;
struct _DskMimeContentDisposition
{
  dsk_boolean is_inline;
  const char *filename_start;
  const char *filename_end;
  const char *name_start;
  const char *name_end;
  const char *id_start;
  const char *id_end;
};

dsk_boolean dsk_parse_mime_content_disposition_header (const char *line,
                                                       DskMimeContentDisposition *out,
                                                       DskError **error);
