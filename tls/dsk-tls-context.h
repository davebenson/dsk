
typedef struct DskTlsClientContextOptions DskTlsClientContextOptions;
typedef struct DskTlsServerContextOptions DskTlsServerContextOptions;
typedef struct DskTlsContextOptions DskTlsContextOptions;

struct DskTlsContextOptions
{
  // A comma-sep list of groups
  // to NOT support.  The actual
  // list of groups will be found
  // by computing the difference
  // from the set of groups we literally support
  // (as in, have the code for) and
  // of course the peer must support the same group.
  const char *unsupported_groups;

  size_t n_certificates;
  DskTlsCertificate *certificates;

  // Setup for server/client specific options.
  DskTlsServerContextOptions *server_options;
  DskTlsClientContextOptions *client_options;
};

struct DskTlsClientContextOptions
{
  // A comma-sep list of key-shares whose
  // public/private keys should be computed
  // before the initial handshake.
  const char *offered_key_share_groups;

};
struct DskTlsServerContextOptions
{
  bool allow_pre_shared_keys;
};



typedef struct DskTlsContext DskTlsContext;

DskTlsContext    *dsk_tls_context_new   (DskTlsContextOptions     *options,
                                         DskError                **error);
DskTlsContext    *dsk_tls_context_ref   (DskTlsContext            *context);
void              dsk_tls_context_unref (DskTlsContext            *context);


