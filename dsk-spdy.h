

/* --- one TCP connection corresponds to a SPDY session; 
       this is the raw framing layer --- */
       
typedef struct _DskSpdySession DskSpdySession;
struct _DskSpdySession
{
  DskObject       base_instance;

  /* underlying transport */
  DskOctetStream *stream;
  DskOctetSource *source;
  DskOctetSink   *sink;

  DskBuffer       incoming;
  DskBuffer       outgoing;

  DskHook         new_stream;
  DskHook         error_hook;
  DskError       *error;

  uint32_t        next_stream_id;
  uint32_t        next_ping_id;

  DskSpdyStream  *streams_by_id;
  DskSpdyStream  *first_unclaimed_stream, *last_unclaimed_stream;
};
#define DSK_SPDY_SESSION_IS_CLIENT(session) ((session)->next_stream_id & 1)

DskSpdySession*dsk_spdy_session_new                   (dsk_boolean     is_client_side,
                                                       DskOctetStream *stream,
                                                       DskOctetSource *source,
                                                       DskOctetSink   *sink,
						       DskBuffer      *initial_data,
						       DskError      **error);

DskSpdySession*dsk_spdy_session_new_client_tcp        (const char     *name,
                                                       DskIpAddress   *address,
						       unsigned        port,
						       DskError      **error);

dsk_boolean    dsk_spdy_session_get_stream            (DskSpdySession *session,
                                                       uint32_t       *stream_id_out,
                                                       DskOctetStream**stream_out,
                                                       DskOctetSource**source_out,
                                                       DskOctetSink  **sink_out);

dsk_boolean    dsk_spdy_session_make_stream           (DskSpdySession *session,
                                                       uint32_t       *stream_id_out,
						       dsk_boolean     unidirectional,
                                                       DskOctetStream**stream_out,
                                                       DskOctetSource**source_out,
                                                       DskOctetSink  **sink_out);

