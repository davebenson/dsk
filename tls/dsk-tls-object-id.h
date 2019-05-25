
/* Object ID */
#define DSK_TLS_OBJECT_ID_SIZEOF_SUBID sizeof(uint32_t)
typedef struct DskTlsObjectID DskTlsObjectID;
struct DskTlsObjectID
{
  uint32_t n_subids;
  uint32_t subids[];
};

extern const uint32_t dsk_tls_object_id_heap[];


DSK_INLINE_FUNC size_t dsk_tls_object_id_size_by_n_subids (unsigned n_subidentifiers);

DSK_INLINE_FUNC bool dsk_tls_object_ids_equal (const DskTlsObjectID *a, 
                                               const DskTlsObjectID *b);


#if DSK_CAN_INLINE || defined(DSK_IMPLEMENT_INLINES)
DSK_INLINE_FUNC size_t
dsk_tls_object_id_size_by_n_subids (unsigned n_subidentifiers)
{
  return sizeof(DskTlsObjectID) + sizeof(uint32_t) * n_subidentifiers;
}
DSK_INLINE_FUNC bool
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
#endif
