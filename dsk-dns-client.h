

typedef struct _DskDnsLookupResult DskDnsLookupResult;
typedef struct _DskDnsEntry DskDnsEntry;

typedef enum
{
  DSK_DNS_LOOKUP_RESULT_FOUND,
  DSK_DNS_LOOKUP_RESULT_NOT_FOUND,
  DSK_DNS_LOOKUP_RESULT_TIMEOUT,
  DSK_DNS_LOOKUP_RESULT_BAD_RESPONSE
} DskDnsLookupResultType;

struct _DskDnsLookupResult
{
  DskDnsLookupResultType type;
  DskIpAddress *addr;          /* if found */
  const char *message;          /* for all other types */
};

typedef void (*DskDnsLookupFunc) (DskDnsLookupResult *result,
                                  void               *callback_data);

void    dsk_dns_lookup (const char       *name,
                        dsk_boolean       is_ipv6,
                        DskDnsLookupFunc  callback,
                        void             *callback_data);


/* non-blocking lookups only hit the cache */
typedef enum
{
  DSK_DNS_LOOKUP_NONBLOCKING_NOT_FOUND,
  DSK_DNS_LOOKUP_NONBLOCKING_MUST_BLOCK,
  DSK_DNS_LOOKUP_NONBLOCKING_FOUND,
  DSK_DNS_LOOKUP_NONBLOCKING_ERROR
} DskDnsLookupNonblockingResult;


/* NB: 'name' must be normalized !!!! */
DskDnsLookupNonblockingResult
       dsk_dns_lookup_nonblocking (const char *name,
                                   DskIpAddress *out,
                                   dsk_boolean    is_ipv6,
                                   DskError     **error);



typedef enum
{
  DSK_DNS_CACHE_ENTRY_IN_PROGRESS,
  DSK_DNS_CACHE_ENTRY_ERROR,
  DSK_DNS_CACHE_ENTRY_NEGATIVE,
  DSK_DNS_CACHE_ENTRY_CNAME,
  DSK_DNS_CACHE_ENTRY_ADDR,
} DskDnsCacheEntryType;

typedef struct _DskDnsCacheEntryJob DskDnsCacheEntryJob;
typedef struct _DskDnsCacheEntry DskDnsCacheEntry;
struct _DskDnsCacheEntry
{
  char *name;
  dsk_boolean is_ipv6;
  unsigned expire_time;
  DskDnsCacheEntryType type;
  union {
    DskDnsCacheEntryJob *in_progress;
    struct {
      DskError *error;
    } error;
    char *cname;
    struct { unsigned n; DskIpAddress *addresses; unsigned i; } addr;
  } info;

  DskDnsCacheEntry *expire_left, *expire_right, *expire_parent;
  dsk_boolean expire_is_red;
  DskDnsCacheEntry *name_type_left, *name_type_right, *name_type_parent;
  dsk_boolean name_type_is_red;
};

typedef enum
{
  DSK_DNS_LOOKUP_USE_SEARCHPATH = (1<<0),
  DSK_DNS_LOOKUP_FOLLOW_CNAMES = (1<<1)
} DskDnsLookupFlags;

typedef void (*DskDnsCacheEntryFunc) (DskDnsCacheEntry *entry,
                                      void             *callback_data);
void              dsk_dns_lookup_cache_entry (const char       *name,
                                              dsk_boolean       is_ipv6,
                                              DskDnsLookupFlags flags,
                                              DskDnsCacheEntryFunc callback,
                                              void             *callback_data);


typedef enum
{
  DSK_DNS_CONFIG_USE_RESOLV_CONF_SEARCHPATH = (1<<0),
  DSK_DNS_CONFIG_USE_RESOLV_CONF_NS = (1<<1),
  DSK_DNS_CONFIG_USE_ETC_HOSTS = (1<<2)
} DskDnsConfigFlags;
#define DSK_DNS_CONFIG_FLAGS_INIT \
  (DSK_DNS_CONFIG_USE_RESOLV_CONF_SEARCHPATH| \
   DSK_DNS_CONFIG_USE_RESOLV_CONF_NS| \
   DSK_DNS_CONFIG_USE_ETC_HOSTS)
void dsk_dns_client_config (DskDnsConfigFlags flags);
void dsk_dns_client_add_nameserver (DskIpAddress *addr);
void dsk_dns_config_dump (void);

