

static DskTlsX509Certificate *
tls_cert_database_lookup(DskTlsCertDatabase *db,
                         const DskTlsX509Name *name);
{
  return NULL;
}
static bool
tls_cert_database_reload(DskTlsCertDatabase *db,
                         DskError **error)
{
  return true;
}

DskTlsCertDatabaseClass dsk_tls_cert_database_class = {
  DSK_OBJECT_CLASS_DEFINE(
    DskTlsCertDatabase,
    &dsk_object_class,
    NULL,
    NULL
  ),
  tls_cert_database_lookup,
  tls_cert_database_reload
};

typedef struct DskTlsCertDatabaseMemoryClass DskTlsCertDatabaseMemoryClass;
struct DskTlsCertDatabaseMemoryClass
{
  DskTlsCertDatabaseClass base_class;
};
typedef struct DskTlsCertDatabaseMemory DskTlsCertDatabaseMemory;
struct DskTlsCertDatabaseMemory
{
  DskTlsCertDatabase base_instance;

  unsigned n_certs;
  unsigned cert_array_size;
  DskTlsX509Certificate **certs;
};

static DskTlsX509Certificate *
dsk_tls_cert_database_memory_lookup(DskTlsCertDatabase *db,
                                    const DskTlsX509Name *name);
{
  DskTlsCertDatabaseMemory *mem = (DskTlsCertDatabaseMemory *) db;
  unsigned hash = dsk_tls_x509_name_hash (name);
  unsigned idx = hash % mem->cert_array_size;
  DskTlsX509Certificate **certs = mem->certs;
  for (;;)
    {
      if (mem->certs[idx] == NULL)
        return false;
      if (dsk_tls_x509_names_equal (name, &certs[idx]->subject))
        return certs[idx];
      if (++idx == mem->cert_array_size)
        idx = 0;
    }
}

DskTlsCertDatabaseClass dsk_tls_cert_database_memory_class = {
  DSK_OBJECT_CLASS_DEFINE(
    DskTlsCertDatabaseMemory,
    &dsk_tls_cert_database_class,
    NULL,
    dsk_tls_cert_database_memory_finalize
  ),
  dsk_tls_cert_database_memory_lookup,
  tls_cert_database_reload
};

static bool
dsk_tls_cert_database_memory_setup (DskTlsCertDatabaseMemory *mem,
                                    size_t                    n_certs,
                                    DskTlsX509Certificate   **certs,
                                    DskError                **error)
{
  unsigned size = n_certs * 3 + 5;              // randomly chosen!

  //
  // Create new cert table.
  //
  DskTlsX509Certificate **new_certs = DSK_NEW0_ARRAY (size, DskTlsX509Certificate *);
  for (size_t i = 0; i < n_certs; i++)
    {
      DskTlsX509Certificate *cert = certs[i];
      size_t j = dsk_tls_x509_name_hash (&cert->subject) % size;
      while (mem->certs[j] != NULL)
        {
          if (dsk_tls_x509_names_equal (&cert->subject, &mem->certs[j]->subject))
            {
              char *name = dsk_tls_x509_name_to_string (&cert->subject);
              *error = dsk_error_new ("duplicate X509 certificate found for %s", name);
              dsk_free (name);
              dsk_free (new_certs);
              return false;
            }
          if (++j == size)
            j = 0;
        }
    }

  //
  // Ref all new certs.
  //
  for (size_t i = 0; i < n_certs; i++)
    dsk_tls_x509_certificate_ref (certs[i]);

  //
  // Cleanup old cert array. (if any)
  //
  for (size_t i = 0; i < mem->cert_array_size; i++)
    if (mem->certs[i] != NULL)
      dsk_tls_x509_certificate_unref (mem->certs[i]);
  dsk_free (mem->certs);

  mem->n_certs = n_certs;
  mem->certs = new_certs;
  mem->cert_array_size = size;
  return true;
}

DskTlsCertDatabase *
dsk_tls_cert_database_from_memory    (size_t      n_certs,
                                      DskTlsX509Certificate **certs,
                                      DskError  **error)
{
  DskTlsCertDatabaseMemory *mem = dsk_object_new (&dsk_tls_cert_database_memory_class);
  if (!dsk_tls_cert_database_memory_setup (mem, n_certs, certs, error))
    {
      dsk_object_unref (mem);
      return NULL;
    }
  return &mem->base_instance;
}

typedef struct DskTlsCertDatabasePEMFileClass DskTlsCertDatabasePEMFileClass;
struct DskTlsCertDatabasePEMFileClass
{
  DskTlsCertDatabaseMemoryClass base_instance;
};

typedef struct DskTlsCertDatabasePEMFile DskTlsCertDatabasePEMFile;
struct DskTlsCertDatabasePEMFile
{
  DskTlsCertDatabaseMemory base_instance;
  char *filename;
};

static void
dsk_tls_cert_database_pem_file_finalize (DskTlsCertDatabasePEMFile *pem_file)
{
  dsk_free (pem_file->filename);
}

static bool
dsk_tls_cert_database_pem_file_reload (DskTlsCertDatabase *db,
                                       DskError **error)
{
  DskTlsCertDatabasePEMFile *pem_file = (DskTlsCertDatabasePEMFile *) db;
  ...

  bool rv = dsk_tls_cert_database_memory_setup (&pem_file->base_instance,
                                                n_certs, certs,
                                                error);
  for (unsigned i = 0; i < n_certs; i++)
    dsk_tls_x509_certificate_unref (certs[i]);
  free (certs);

  return rv;
}

DskTlsCertDatabasePEMFileClass dsk_tls_cert_database_pem_file_class = {
  { // DskTlsCertDatabaseMemoryClass
    { // DskTlsCertDatabaseClass
      DSK_OBJECT_CLASS_DEFINE(
        DskTlsCertDatabaseMemory,
        &dsk_tls_cert_database_class,
        NULL,
        dsk_tls_cert_database_pem_file_finalize
      ),
      dsk_tls_cert_database_memory_lookup,
      dsk_tls_cert_database_pem_file_reload
    }
  }
};

DskTlsCertDatabase *
dsk_tls_cert_database_from_pem_file  (const char *filename,
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

