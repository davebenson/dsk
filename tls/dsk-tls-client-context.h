
/* --- DskTlsClientSessionDatabase --- */
struct DskTlsClientSessionDatabaseClass {
  DskObjectClass base_class;
  void (*lookup)(DskTlsClientSessionDatabase *database,
                 DskTlsClientSessionLookupResult *result);
};
struct DskTlsClientSessionDatabase {
  DskObject base_instance;
};

struct DskTlsClientSessionLookupResult {
  void (*found_sessions)(DskTlsClientSessionLookupResult *result,
                         size_t n_sessions_found,
                         const DskTlsClientSessionInfo *sessions,
                         const uint8_t *state_length);
  void (*session_not_found)(DskTlsClientSessionLookupResult *result);
};

typedef void (*DskTlsClientLookupSessionsFunc)(DskTlsClientConnection *conn,
                                              void *client_lookup_session_data);
void dsk_tls_client_connection_found_sessions (DskTlsClientConnection *conn,
void dsk_tls_client_connection_session_not_found  (DskTlsClientConnection *conn);


typedef struct DskTlsClientContextOptions DskTlsClientContextOptions;
struct DskTlsClientContextOptions
{
  size_t n_certificates;
  DskTlsKeyPair **certificates;

  size_t n_application_layer_protocols;
  const char **application_layer_protocols;
  bool application_layer_protocol_negotiation_required;

  bool support_early_data;              // requires Pre-Shared Key

  // A comma-sep list of key-shares whose
  // public/private keys should be computed
  // in the initial ClientHello.
  const char *offered_key_share_groups;

  DskTlsClientSessionDatabase *lookup_sessions;

  DskTlsCertDatabase *cacert_database;

  size_t n_certificate_authorities;
  DskTlsX509DistinguishedName *certificate_authorities;

  const char *server_name;

  bool allow_self_signed;
  DskTlsX509Certificate *pinned_cert;

  DskTlsClientVerifyServerCertFunc verify_server_cert_func;
  void *verify_server_cert_data;

  DskTlsClientWarnHandler client_warn_handler;
  void *client_warn_data;
  DskDestroyNotify client_warn_destroy;

  DskTlsClientFatalHandler fatal_handler;
  void *fatal_handler_data;
  DskDestroyNotify fatal_handler_destroy;
};
#define DSK_TLS_CLIENT_CONTEXT_OPTIONS_INIT (DskTlsClientContextOptions){ \
        .n_certificates = 0, \
}

DskTlsClientContext *dsk_tls_client_context_new   (DskTlsClientContextOptions  *options,
                                                   DskError                   **error);
DskTlsClientContext *dsk_tls_client_context_ref   (DskTlsClientContext         *context);
void                 dsk_tls_client_context_unref (DskTlsClientContext         *context);


