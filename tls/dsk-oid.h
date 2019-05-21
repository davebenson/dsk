
/* Object ID */
typedef struct DskOID DskOID;
struct DskOID
{
  uint32_t n_subids;
  uint32_t subids[];
};

extern const uint32_t dskoid_heap[];

