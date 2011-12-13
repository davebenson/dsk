typedef struct _DskSslContextOptions DskSslContextOptions;
typedef struct _DskSslStreamOptions DskSslStreamOptions;
typedef struct _DskSslContext DskSslContext;
typedef struct _DskSslStream DskSslStream;

struct _DskSslContextOptions
{
  const char     *cert_filename;
  const char     *key_filename;
  const char     *password;
};

DskSslContext    *dsk_ssl_context_new   (DskSslContextOptions *options,
                                         DskError            **error);
  
struct _DskSslStreamOptions
{
  dsk_boolean     is_client;
  DskSslContext  *context;
  DskOctetSink   *sink;
  DskOctetSource *source;
};

dsk_boolean  dsk_ssl_stream_new          (DskSslStreamOptions   *options,
                                          DskSslStream         **stream_out,
					  DskOctetSink         **sink_out,
                                          DskOctetSource       **source_out,
					  DskError             **error);
