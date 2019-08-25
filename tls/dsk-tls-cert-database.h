
typedef struct DskTlsCertDatabaseClass DskTlsCertDatabaseClass;
typedef struct DskTlsCertDatabase DskTlsCertDatabase;

struct DskTlsCertDatabaseClass
{
  DskClassObject base_instance;

  // NOTE: caller must unref the returned cert (if non-NULL)
  DskTlsX509Certificate *(*lookup)(DskTlsCertDatabase *db,
                                   const DskTlsX509Name *name);
  bool (*reload)(DskTlsCertDatabase *db,
                 DskError **error);
};

struct DskTlsCertDatabase
{
  DskObject base_instance;
};


DskTlsCertDatabase *
dsk_tls_cert_database_from_directory (const char *dir_name,
                                      DskError  **error);

DskTlsCertDatabase *
dsk_tls_cert_database_from_pem_file  (const char *filename,
                                      DskError  **error);


DskTlsCertDatabase *
dsk_tls_cert_database_from_memory    (size_t      n_certs,
                                      DskTlsX509Certificate **certs,
                                      DskError  **error);

DskTlsCertDatabase *
dsk_tls_cert_database_global         (void);

//
// Used to notify us to reread the cacert file
// for the global database.
//
// (Users providing a DskTlsCertDatabase
// will have to do their own updates).
//
void
dsk_tls_cert_database_global_reload  (void);

//
// Walks-up the certificate chain,
// validating each cert,
// and stopping (with success) if any
// cert matches one in our database.
//
// This can fail because:
//   (1) no ca-cert can be found for any cert in the chain.
//   (2) signature verification failed.
//   (3) format errors in the database (for lazily loaded databases).
//
// This is the workhorse of the authentication procedure,
// also known as the Public-Key Infrastructure.
//
bool
dsk_tls_cert_database_validate_cert  (DskTlsCertDatabase *db,
                                      const DskTlsX509Certificate *cert,
                                      DskError  **error);

