
/* Object ID */
#define DSK_TLS_OBJECT_ID_SIZEOF_SUBID sizeof(uint32_t)
typedef struct DskTlsObjectID DskTlsObjectID;
struct DskTlsObjectID
{
  uint32_t n_subids;
  uint32_t subids[];
};
#define DSK_TLS_OBJECT_ID_MAX_SUBID_STRING_SIZE       11                        /* 1,000,000,000 = 10 digits, plus '.' */

extern const uint32_t dsk_tls_object_id_heap[];


DSK_INLINE size_t dsk_tls_object_id_size_by_n_subids (unsigned n_subidentifiers);

DSK_INLINE bool dsk_tls_object_ids_equal (const DskTlsObjectID *a, 
                                               const DskTlsObjectID *b);
char *dsk_tls_object_id_to_string(const DskTlsObjectID *oid);


DSK_INLINE size_t
dsk_tls_object_id_size_by_n_subids (unsigned n_subidentifiers)
{
  return sizeof(DskTlsObjectID) + sizeof(uint32_t) * n_subidentifiers;
}
DSK_INLINE bool
dsk_tls_object_ids_equal (const DskTlsObjectID *a, 
                          const DskTlsObjectID *b)
{
  if (a->n_subids != b->n_subids)
    return false;
  size_t N = a->n_subids;
  for (unsigned i = 0; i < N; i++)
    if (a->subids[i] != b->subids[i])
      return false;
  return true;
}
