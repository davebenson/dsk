
#define DSK_HTTP2_FRAME_HEADER_LENGTH   9

typedef struct DskHttp2Header DskHttp2Header;
struct DskHttp2Header
{
  const char *key;
  const char *value;
  DskHttp2HeaderCode code;
};
  

typedef struct DskHttp2Stream DskHttp2Stream;
typedef struct DskHttp2StreamClass DskHttp2StreamClass;

struct DskHttp2StreamClass
{
  DskOctetStreamClass base_class;
};


struct DskHttp2Stream
{
  DskOctetStream base_instance;
  uint32_t stream_id;
  unsigned n_request_headers;
  DskHttp2Header **request_headers;
  unsigned n_response_headers;
  DskHttp2Header **response_headers;
  DskHttp2StreamCallbacks *callbacks;
  void *callback_data;
  DskHttp2Stream *next_in_hash;
  DskHttp2Connection *connection;

  unsigned request_headers_alloced;
  unsigned response_headers_alloced;
};

typedef struct {
  size_t n_stream_bins;
  DskHttp2Stream **streams_by_bin;
  size_t n_streams;
  DskHttp2Server *server;               // if it's a server-connection
  uint8_t http_major_version;
  uint8_t http_minor_version;
} DskHttp2Connection;

typedef struct DskHttp2Server DskHttp2Server;
struct DskHttp2ServerCallbacks
{
  void (*connection_started) (DskHttp2Server     *server,
                              DskHttp2Connection *connection,
                              void               *callback_data);
};


struct DskHttp2ServerOptions
{
  /* If NULL, the listener will be created by the constructor from
   * the subsequent options.
   */
  DskOctetListener *listener;

  /* Build listener from these arguments */
  int port;
  bool use_ssl;
  DskSslListenerOptions *ssl_options;
  DskOctetListenerSocketOptions *non_ssl_options;

  /* Callbacks */
  DskHttp2ServerCallbacks *callbacks;
  void                    *callback_data;
};

DskHttp2Server *dsk_http2_server_new (DskSslListenerOptions   *options,
                                      DskError               **error);


DskHttp2Connection    *dsk_http2_client_connection_new (DskHttp2ClientConnectionOptions *options);

/* --- connection API --- */
void                   dsk_http2_connection_go_away    (DskHttp2Connection *conn);
DskHttp2Stream        *dsk_http2_connection_create_stream(DskHttp2Connection *connection);

/* --- stream APIs --- */
void                   dsk_http2_stream_add_headers    (DskHttp2Stream     *stream,
                                                        unsigned            n_headers,
                                                        DskHttp2Header     *headers,
                                                        bool                done_adding);
void                   dsk_http2_stream_trap_events    (DskHttp2Stream     *stream,
                                                        DskHttp2StreamCallbacks *callbacks,
                                                        void               *callback_data);
