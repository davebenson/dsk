#include <string.h>
#include <stdio.h>
#include "../dsk.h"

typedef struct _RequestData RequestData;
struct _RequestData
{
  DskMemorySource *source;
  DskMemorySink *sink;
  DskHttpResponse *response_header;
  DskError *error;
  DskWebsocket *websocket;
  dsk_boolean content_complete;
  DskBuffer content;
  dsk_boolean destroyed;
};
static dsk_boolean cmdline_verbose = DSK_FALSE;
static dsk_boolean cmdline_print_error_messages = DSK_FALSE;
#define REQUEST_DATA_DEFAULT { NULL, NULL, NULL, NULL, NULL,    \
                               DSK_FALSE,       /* content_complete */ \
                               DSK_BUFFER_INIT, \
                               DSK_FALSE,       /* destroyed */ \
                             }

static void
request_data__handle_response (DskHttpClientStreamTransfer *xfer)
{
  RequestData *rd = xfer->user_data;
  rd->response_header = dsk_object_ref (xfer->response);
}
static void
request_data__handle_content_complete (DskHttpClientStreamTransfer *xfer)
{
  RequestData *rd = xfer->user_data;
  if (xfer->content != NULL)
    dsk_buffer_drain (&rd->content, &xfer->content->buffer);
  rd->content_complete = DSK_TRUE;
}
static void
request_data__handle_error (DskHttpClientStreamTransfer *xfer)
{
  RequestData *rd = xfer->user_data;
  if (rd->error == NULL)
    rd->error = dsk_error_ref (xfer->owner->latest_error);
}
static void
request_data__destroy (DskHttpClientStreamTransfer *xfer)
{
  RequestData *rd = xfer->user_data;
  rd->destroyed = DSK_TRUE;
}

static void
request_data__handle_response_websocket (DskHttpClientStreamTransfer *xfer)
{
  RequestData *rd = xfer->user_data;
  rd->websocket = dsk_object_ref (xfer->websocket);
  rd->response_header = dsk_object_ref (xfer->response);
}

static void
request_data_clear (RequestData *rd)
{
  dsk_object_unref (rd->source);
  dsk_object_unref (rd->sink);
  if (rd->error)
    dsk_error_unref (rd->error);
  if (rd->response_header)
    dsk_object_unref (rd->response_header);
  if (rd->websocket)
    dsk_object_unref (rd->websocket);
  dsk_buffer_reset (&rd->content);
}

static dsk_boolean
is_http_request_complete (DskBuffer *buf,
                          unsigned  *length_out)
{
  char *slab = dsk_malloc (buf->size + 1);
  const char *a, *b;

  dsk_buffer_peek (buf, buf->size, slab);
  slab[buf->size] = 0;

  a = strstr (slab, "\n\n");
  b = strstr (slab, "\n\r\n");
  if (a == NULL && b == NULL)
    {
      dsk_free (slab);
      return DSK_FALSE;
    }
  if (length_out)
    {
      if (a == NULL)
        *length_out = b - slab + 3;
      else if (b == NULL)
        *length_out = a - slab + 2;
      else if (a < b)
        *length_out = a - slab + 2;
      else 
        *length_out = b - slab + 3;
    }

  dsk_free (slab);
  return DSK_TRUE;
}

static void
test_simple (dsk_boolean byte_by_byte)
{
  static const char *response_content_versions[] =  {
                                "HTTP/1.1 200 OK\r\n"
                                "Date: Mon, 17 May 2010 22:50:08 GMT\r\n"
                                "Content-Type: text/plain\r\n"
                                "Content-Length: 7\r\n"
                                "Connection: close\r\n"
                                "\r\n"
                                "hi mom\n",

                                "HTTP/1.1 200 OK\n"
                                "Date: Mon, 17 May 2010 22:50:08 GMT\n"
                                "Content-Type: text/plain\n"
                                "Content-Length: 7\n"
                                "Connection: close\n"
                                "\n"
                                "hi mom\n",

                                "HTTP/1.1 200 OK\r\n"
                                "Date:\r\n"
                                "     Mon, 17 May 2010 22:50:08 GMT\r\n"
                                "Content-Type:\r\n"
                                "     text/plain\r\n"
                                "Content-Length:\r\n"
                                "     7\r\n"
                                "Connection:\r\n"
                                "     close\r\n"
                                "\r\n"
                                "hi mom\n",

                                "HTTP/1.1 100 Continue\r\n"
                                "\r\n"
                                "HTTP/1.1 200 OK\r\n"
                                "Date: Mon, 17 May 2010 22:50:08 GMT\r\n"
                                "Content-Type: text/plain\r\n"
                                "Content-Length: 7\r\n"
                                "Connection: close\r\n"
                                "\r\n"
                                "hi mom\n",
                        };
  unsigned iter;

  for (iter = 0; iter < DSK_N_ELEMENTS (response_content_versions); iter++)
    {
      DskHttpClientStream *stream;
      DskHttpClientStreamOptions options = DSK_HTTP_CLIENT_STREAM_OPTIONS_INIT;
      RequestData request_data = REQUEST_DATA_DEFAULT;
      DskHttpRequestOptions req_options = DSK_HTTP_REQUEST_OPTIONS_INIT;
      DskHttpClientStreamRequestOptions cr_options = DSK_HTTP_CLIENT_STREAM_REQUEST_OPTIONS_INIT;
      DskHttpClientStreamTransfer *xfer;
      DskHttpClientStreamFuncs request_funcs_0;
      const char *content;
      DskError *error = NULL;
      fprintf (stderr, ".");
      memset (&request_funcs_0, 0, sizeof (request_funcs_0));
      request_funcs_0.handle_response = request_data__handle_response;
      request_funcs_0.handle_content_complete = request_data__handle_content_complete;
      request_funcs_0.destroy = request_data__destroy;
      request_data.source = dsk_memory_source_new ();
      request_data.sink = dsk_memory_sink_new ();
      request_data.sink->max_buffer_size = 100000000;
      stream = dsk_http_client_stream_new (DSK_OCTET_SINK (request_data.sink),
                                           DSK_OCTET_SOURCE (request_data.source),
                                           &options);
      req_options.host = "localhost";
      req_options.full_path = "/hello.txt";
      cr_options.request_options = &req_options;
      cr_options.funcs = &request_funcs_0;
      cr_options.user_data = &request_data;
      xfer = dsk_http_client_stream_request (stream, &cr_options, &error);
      if (xfer == NULL)
        dsk_die ("dsk_http_client_stream_request failed: %s", error->message);

      /* read data from sink */
      while (!is_http_request_complete (&request_data.sink->buffer, NULL))
        dsk_main_run_once ();

      content = response_content_versions[iter];

      /* write response */
      if (byte_by_byte)
        {
          const char *at = content;
          while (*at)
            {
              dsk_buffer_append_byte (&request_data.source->buffer, *at++);
              dsk_memory_source_added_data (request_data.source);
              while (request_data.source->buffer.size)
                dsk_main_run_once ();
            }
        }
      else
        {
          dsk_buffer_append_string (&request_data.source->buffer, content);
          dsk_memory_source_added_data (request_data.source);
        }

      while (request_data.response_header == NULL)
        dsk_main_run_once ();
      dsk_assert (request_data.response_header->http_major_version == 1);
      dsk_assert (request_data.response_header->http_minor_version == 1);
      dsk_assert (request_data.response_header->content_length == 7);
      dsk_assert (!request_data.response_header->transfer_encoding_chunked);
      dsk_assert (request_data.response_header->connection_close);
      while (!request_data.content_complete)
        dsk_main_run_once ();

      dsk_assert (request_data.content.size == 7);
      {
        char buf[7];
        dsk_buffer_peek (&request_data.content, 7, buf);
        dsk_assert (memcmp (buf, "hi mom\n", 7) == 0);
      }

      dsk_object_unref (stream);
      request_data_clear (&request_data);
    }
}

static void test_simple_bigwrite() { test_simple(DSK_FALSE); }
static void test_simple_bytebybyte() { test_simple(DSK_TRUE); }

static void
test_transfer_encoding_chunked (void)
{
  unsigned iter;
  for (iter = 0; iter < 3; iter++)
    {
      DskHttpClientStream *stream;
      DskHttpClientStreamOptions options = DSK_HTTP_CLIENT_STREAM_OPTIONS_INIT;
      RequestData request_data = REQUEST_DATA_DEFAULT;
      DskHttpRequestOptions req_options = DSK_HTTP_REQUEST_OPTIONS_INIT;
      DskHttpClientStreamTransfer *xfer;
      DskHttpClientStreamFuncs request_funcs_0;
      DskError *error = NULL;
      const char *content = NULL;
      DskHttpClientStreamRequestOptions cr_options = DSK_HTTP_CLIENT_STREAM_REQUEST_OPTIONS_INIT;
      unsigned pass;
      switch (iter)
        {
        case 0:
          content = "HTTP/1.1 200 OK\r\n"
                    "Date: Mon, 17 May 2010 22:50:08 GMT\r\n"
                    "Content-Type: text/plain\r\n"
                    "Transfer-Encoding: chunked\r\n"
                    "\r\n"
                    "7\r\nhi mom\n\r\n"
                    "0\r\n"
                    "\r\n";             /* no trailer */
          break;
        case 1:
          content = "HTTP/1.1 200 OK\r\n"
                    "Date: Mon, 17 May 2010 22:50:08 GMT\r\n"
                    "Content-Type: text/plain\r\n"
                    "Transfer-Encoding: chunked\r\n"
                    "\r\n"
                    "7; random extension\r\nhi mom\n\r\n"
                    "0; another extension\r\n"
                    "\r\n";             /* no trailer */
          break;
        case 2:
          content = "HTTP/1.1 200 OK\r\n"
                    "Date: Mon, 17 May 2010 22:50:08 GMT\r\n"
                    "Content-Type: text/plain\r\n"
                    "Transfer-Encoding: chunked\r\n"
                    "\r\n"
                    "7\r\nhi mom\n\r\n"
                    "0\r\n"
                    "X-Information: whatever\r\n"              /* trailer */
                    "\r\n";
          break;
        }
      memset (&request_funcs_0, 0, sizeof (request_funcs_0));
      request_funcs_0.handle_response = request_data__handle_response;
      request_funcs_0.handle_content_complete = request_data__handle_content_complete;
      request_funcs_0.destroy = request_data__destroy;
      request_data.source = dsk_memory_source_new ();
      request_data.sink = dsk_memory_sink_new ();
      request_data.sink->max_buffer_size = 100000000;
      stream = dsk_http_client_stream_new (DSK_OCTET_SINK (request_data.sink),
                                           DSK_OCTET_SOURCE (request_data.source),
                                           &options);
      req_options.host = "localhost";
      req_options.full_path = "/hello.txt";
      cr_options.request_options = &req_options;
      cr_options.funcs = &request_funcs_0;
      cr_options.user_data = &request_data;

      /* write response */
      for (pass = 0; pass < 2; pass++)
        {
          fprintf (stderr, ".");
          xfer = dsk_http_client_stream_request (stream, &cr_options, &error);
          DSK_UNUSED (xfer);

          /* read data from sink */
          while (!is_http_request_complete (&request_data.sink->buffer, NULL))
            dsk_main_run_once ();

          switch (pass)
            {
            case 0:
              dsk_buffer_append_string (&request_data.source->buffer, content);
              dsk_memory_source_added_data (request_data.source);
              break;
            case 1:
              {
                const char *at = content;
                while (*at)
                  {
                    dsk_buffer_append_byte (&request_data.source->buffer, *at++);
                    dsk_memory_source_added_data (request_data.source);
                    while (request_data.source->buffer.size)
                      dsk_main_run_once ();
                  }
              }
              break;
            }

          while (request_data.response_header == NULL)
            dsk_main_run_once ();

          while (request_data.response_header == NULL)
            dsk_main_run_once ();
          dsk_assert (request_data.response_header->http_major_version == 1);
          dsk_assert (request_data.response_header->http_minor_version == 1);
          dsk_assert (request_data.response_header->content_length == -1LL);
          dsk_assert (request_data.response_header->transfer_encoding_chunked);
          dsk_assert (!request_data.response_header->connection_close);
          while (!request_data.content_complete)
            dsk_main_run_once ();

          dsk_assert (request_data.content.size == 7);
          {
            char buf[7];
            dsk_buffer_peek (&request_data.content, 7, buf);
            dsk_assert (memcmp (buf, "hi mom\n", 7) == 0);
          }
          dsk_buffer_reset (&request_data.content);

          request_data.content_complete = 0;
          request_data.destroyed = 0;
          dsk_object_unref (request_data.response_header);
          request_data.response_header = NULL;
          dsk_buffer_reset (&request_data.sink->buffer);
        }
      dsk_object_unref (stream);
      request_data_clear (&request_data);
    }
}

static void
test_simple_post (void)
{
  DskHttpClientStream *stream;
  DskHttpClientStreamOptions options = DSK_HTTP_CLIENT_STREAM_OPTIONS_INIT;
  RequestData request_data = REQUEST_DATA_DEFAULT;
  DskHttpRequestOptions req_options = DSK_HTTP_REQUEST_OPTIONS_INIT;
  DskHttpClientStreamRequestOptions cr_options = DSK_HTTP_CLIENT_STREAM_REQUEST_OPTIONS_INIT;
  DskHttpClientStreamTransfer *xfer;
  DskHttpClientStreamFuncs request_funcs_0;
  DskError *error = NULL;
  DskMemorySource *post_data;
  const char *post_data_str = "this is some POST data\n";
  unsigned len;
  memset (&request_funcs_0, 0, sizeof (request_funcs_0));
  request_funcs_0.handle_response = request_data__handle_response;
  request_funcs_0.handle_content_complete = request_data__handle_content_complete;
  request_funcs_0.destroy = request_data__destroy;
  request_data.source = dsk_memory_source_new ();
  request_data.sink = dsk_memory_sink_new ();
  request_data.sink->max_buffer_size = 100000000;
  stream = dsk_http_client_stream_new (DSK_OCTET_SINK (request_data.sink),
                                       DSK_OCTET_SOURCE (request_data.source),
                                       &options);
  req_options.verb = DSK_HTTP_VERB_POST;
  req_options.host = "localhost";
  req_options.full_path = "/hello.txt";
  req_options.content_length = strlen (post_data_str);
  post_data = dsk_memory_source_new ();
  dsk_buffer_append_string (&post_data->buffer, post_data_str);
  dsk_memory_source_done_adding (post_data);
  cr_options.request_options = &req_options;
  cr_options.funcs = &request_funcs_0;
  cr_options.user_data = &request_data;
  cr_options.post_data = DSK_OCTET_SOURCE (post_data);
  cr_options.post_data_length = req_options.content_length;
  xfer = dsk_http_client_stream_request (stream, &cr_options, &error);
  if (xfer == NULL)
    dsk_die ("error creating request: %s", error->message);
  dsk_object_unref (post_data);

  /* read data from sink; pluck off POST Data */
  while (!is_http_request_complete (&request_data.sink->buffer, &len))
    dsk_main_run_once ();
  dsk_buffer_discard (&request_data.sink->buffer, len);
  while (request_data.sink->buffer.size < strlen (post_data_str))
    dsk_main_run_once ();
  dsk_assert (request_data.sink->buffer.size == strlen (post_data_str));
  {
    char slab[1000];
    dsk_buffer_read (&request_data.sink->buffer, 1000, slab);
    dsk_assert (memcmp (slab, post_data_str, strlen (post_data_str)) == 0);
  }

  /* write response */
  {
  static const char *content = 
                            "HTTP/1.1 200 OK\r\n"
                            "Date: Mon, 17 May 2010 22:50:08 GMT\r\n"
                            "Content-Type: text/plain\r\n"
                            "Content-Length: 7\r\n"
                            "Connection: close\r\n"
                            "\r\n"
                            "hi mom\n";
  dsk_buffer_append_string (&request_data.source->buffer, content);
  dsk_memory_source_added_data (request_data.source);
  }

  while (request_data.response_header == NULL)
    dsk_main_run_once ();
  dsk_assert (request_data.response_header->http_major_version == 1);
  dsk_assert (request_data.response_header->http_minor_version == 1);
  dsk_assert (request_data.response_header->content_length == 7);
  dsk_assert (!request_data.response_header->transfer_encoding_chunked);
  dsk_assert (request_data.response_header->connection_close);
  while (!request_data.content_complete)
    dsk_main_run_once ();

  dsk_assert (request_data.content.size == 7);
  {
    char buf[7];
    dsk_buffer_peek (&request_data.content, 7, buf);
    dsk_assert (memcmp (buf, "hi mom\n", 7) == 0);
  }

  dsk_object_unref (stream);
  request_data_clear (&request_data);
}

typedef struct
{
  DskMemorySource *memory_source;
  const char *str;
} PostInfo;

static void
write_post_data (void *pi)
{
  PostInfo *post_info = pi;

  dsk_buffer_append_string (&post_info->memory_source->buffer,
                            post_info->str);
  dsk_memory_source_added_data (post_info->memory_source);
  dsk_memory_source_done_adding (post_info->memory_source);
  dsk_object_unref (post_info->memory_source);
}
static void
write_post_data_1by1 (void *pi)
{
  PostInfo *post_info = pi;
  if (*(post_info->str) == 0)
    {
      dsk_memory_source_done_adding (post_info->memory_source);
      dsk_object_unref (post_info->memory_source);
      return;
    }

  dsk_buffer_append_byte (&post_info->memory_source->buffer,
                          *post_info->str);
  post_info->str += 1;
  dsk_memory_source_added_data (post_info->memory_source);
  dsk_main_add_idle (write_post_data_1by1, post_info);
}

static void
test_transfer_encoding_chunked_post (void)
{
  unsigned iter;
  for (iter = 0; iter < 4; iter++)
    {
      DskHttpClientStream *stream;
      DskHttpClientStreamOptions options = DSK_HTTP_CLIENT_STREAM_OPTIONS_INIT;
      DskHttpClientStreamRequestOptions cr_options = DSK_HTTP_CLIENT_STREAM_REQUEST_OPTIONS_INIT;
      DskHttpRequestOptions req_options = DSK_HTTP_REQUEST_OPTIONS_INIT;
      DskHttpClientStreamTransfer *xfer;
      DskHttpClientStreamFuncs request_funcs_0;
      DskError *error = NULL;
      RequestData request_data = REQUEST_DATA_DEFAULT;
      PostInfo post_info;
      const char *post_data_str = "this is some POST data\n";
      DskMemorySource *post_data;
      unsigned len;
      const char *expected;
      memset (&request_funcs_0, 0, sizeof (request_funcs_0));
      request_funcs_0.handle_response = request_data__handle_response;
      request_funcs_0.handle_content_complete = request_data__handle_content_complete;
      request_funcs_0.destroy = request_data__destroy;
      request_data.source = dsk_memory_source_new ();
      request_data.sink = dsk_memory_sink_new ();
      request_data.sink->max_buffer_size = 100000000;
      stream = dsk_http_client_stream_new (DSK_OCTET_SINK (request_data.sink),
                                           DSK_OCTET_SOURCE (request_data.source),
                                           &options);
      req_options.verb = DSK_HTTP_VERB_POST;
      req_options.host = "localhost";
      req_options.full_path = "/hello.txt";
      post_data = dsk_memory_source_new ();
      fprintf (stderr, ".");
      if (iter == 0)
        {
          dsk_buffer_append_string (&post_data->buffer, post_data_str);
          dsk_memory_source_added_data (post_data);
          dsk_memory_source_done_adding (post_data);
        }
      cr_options.request_options = &req_options;
      cr_options.post_data = DSK_OCTET_SOURCE (post_data);
      cr_options.funcs = &request_funcs_0;
      cr_options.user_data = &request_data;
      xfer = dsk_http_client_stream_request (stream, &cr_options, &error);
      DSK_UNUSED (xfer);
      if (iter == 0)
        dsk_object_unref (post_data);
      else if (iter == 1)
        {
          dsk_buffer_append_string (&post_data->buffer, post_data_str);
          dsk_memory_source_added_data (post_data);
          dsk_memory_source_done_adding (post_data);
          dsk_object_unref (post_data);
        }
      else if (iter == 2)
        {
          post_info.str = post_data_str;
          post_info.memory_source = post_data;
          dsk_main_add_idle (write_post_data, &post_info);
        }
      else if (iter == 3)
        {
          post_info.str = post_data_str;
          post_info.memory_source = post_data;
          dsk_main_add_idle (write_post_data_1by1, &post_info);
        }

      /* read data from sink; pluck off POST Data */
      while (!is_http_request_complete (&request_data.sink->buffer, &len))
        dsk_main_run_once ();
      dsk_buffer_discard (&request_data.sink->buffer, len);
      expected = iter == 3
                   ? "1\r\nt\r\n" /* a bunch of 1-byte chunks */
                     "1\r\nh\r\n"
                     "1\r\ni\r\n"
                     "1\r\ns\r\n"
                     "1\r\n \r\n"
                     "1\r\ni\r\n"
                     "1\r\ns\r\n"
                     "1\r\n \r\n"
                     "1\r\ns\r\n"
                     "1\r\no\r\n"
                     "1\r\nm\r\n"
                     "1\r\ne\r\n"
                     "1\r\n \r\n"
                     "1\r\nP\r\n"
                     "1\r\nO\r\n"
                     "1\r\nS\r\n"
                     "1\r\nT\r\n"
                     "1\r\n \r\n"
                     "1\r\nd\r\n"
                     "1\r\na\r\n"
                     "1\r\nt\r\n"
                     "1\r\na\r\n"
                     "1\r\n\n\r\n"
                     "0\r\n\r\n"
                   : "17\r\nthis is some POST data\n\r\n0\r\n\r\n";
      while (request_data.sink->buffer.size < strlen (expected))
        dsk_main_run_once ();
      dsk_assert (request_data.sink->buffer.size == strlen (expected));
      {
        char slab[1000];
        dsk_buffer_read (&request_data.sink->buffer, 1000, slab);
        dsk_assert (memcmp (slab, expected, strlen (expected)) == 0);
      }

      /* write response */
      {
      static const char *content = 
                                "HTTP/1.1 200 OK\r\n"
                                "Date: Mon, 17 May 2010 22:50:08 GMT\r\n"
                                "Content-Type: text/plain\r\n"
                                "Content-Length: 7\r\n"
                                "Connection: close\r\n"
                                "\r\n"
                                "hi mom\n";
      dsk_buffer_append_string (&request_data.source->buffer, content);
      dsk_memory_source_added_data (request_data.source);
      }

      while (request_data.response_header == NULL)
        dsk_main_run_once ();
      dsk_assert (request_data.response_header->http_major_version == 1);
      dsk_assert (request_data.response_header->http_minor_version == 1);
      dsk_assert (request_data.response_header->content_length == 7);
      dsk_assert (!request_data.response_header->transfer_encoding_chunked);
      dsk_assert (request_data.response_header->connection_close);
      while (!request_data.content_complete)
        dsk_main_run_once ();

      dsk_assert (request_data.content.size == 7);
      {
        char buf[7];
        dsk_buffer_peek (&request_data.content, 7, buf);
        dsk_assert (memcmp (buf, "hi mom\n", 7) == 0);
      }

      dsk_object_unref (stream);
      request_data_clear (&request_data);
    }
}

/* test keepalive under a variety of request and response types */
static void
test_keepalive_full (dsk_boolean add_postcontent_newlines)
{
  static const char *patterns[] = 
    /* Legend:  Query, get
                Post query
                Transfer-encoding chunked POST request
                Response, content-length
                Chunked response, transfer-encoding chunked */
    { "QRQRQRQRQRQRQRQRQRQRQRQRQRQRQRQRQRQRQRQRQRQRQRQRQRQR",
      "QQRRQQRRQQRRQQRRQQRRQQRRQQRRQQRRQQRRQQRRQQRRQQRRQQRR",
      "QQQRRRQQQRRRQQQRRRQQQRRRQQQRRRQQQRRRQQQRRRQQQRRR",
      "QQQRCRQQQRCRQQQCRCQQQCCCQQQRRRQQQCCCQQQRCRQQQCCR" };
  RequestData request_data_default = REQUEST_DATA_DEFAULT;
  RequestData request_data_array[100]; /* max length of pattern string ! */
  DskHttpClientStreamFuncs request_funcs_0;
  unsigned iter;
  dsk_boolean debug = DSK_FALSE;
  memset (&request_funcs_0, 0, sizeof (request_funcs_0));
  request_funcs_0.handle_response = request_data__handle_response;
  request_funcs_0.handle_content_complete = request_data__handle_content_complete;
  request_funcs_0.destroy = request_data__destroy;
  for (iter = 0; iter < DSK_N_ELEMENTS (patterns); iter++)
    {
      const char *at = patterns[iter];
      unsigned n_requests = 0, n_responses = 0;
      DskHttpClientStream *stream;
      DskMemorySource *source;
      DskMemorySink *sink;
      DskHttpClientStreamOptions options = DSK_HTTP_CLIENT_STREAM_OPTIONS_INIT;
      DskError *error = NULL;

      source = dsk_memory_source_new ();
      sink = dsk_memory_sink_new ();
      sink->max_buffer_size = 100000000;
      stream = dsk_http_client_stream_new (DSK_OCTET_SINK (sink),
                                           DSK_OCTET_SOURCE (source),
                                           &options);
      request_data_default.source = source;
      request_data_default.sink = sink;
      if (debug)
        fprintf (stderr, "[");
      else
        fprintf (stderr, ".");
      while (*at)
        {
          RequestData *rd;
          dsk_boolean is_query = (*at == 'Q' || *at == 'P' || *at == 'T');
          dsk_boolean is_response = (*at == 'R' || *at == 'C');
          dsk_assert (is_query || is_response);
          if (debug)
            fprintf(stderr, "%c", *at);
          if (is_query)
            {
              rd = request_data_array + n_requests;
              dsk_assert (n_requests < DSK_N_ELEMENTS (request_data_array));
              *rd = request_data_default;
              dsk_object_ref (rd->sink);
              dsk_object_ref (rd->source);      /* undone by clear() */
            }
          else
            {
              rd = request_data_array + n_responses;
              dsk_assert (n_responses < n_requests);
            }
          switch (*at)
            {
              /* === queries === */
            case 'Q':
              {
                DskHttpRequestOptions req_options = DSK_HTTP_REQUEST_OPTIONS_INIT;
                DskHttpClientStreamRequestOptions cr_options = DSK_HTTP_CLIENT_STREAM_REQUEST_OPTIONS_INIT;
                DskHttpClientStreamTransfer *xfer;
                req_options.verb = DSK_HTTP_VERB_GET;
                req_options.host = "localhost";
                req_options.full_path = "/hello.txt";
                cr_options.request_options = &req_options;
                cr_options.funcs = &request_funcs_0;
                cr_options.user_data = rd;
                xfer = dsk_http_client_stream_request (stream, &cr_options, &error);
                dsk_assert (xfer != NULL);
                (void) xfer;            /* hmm */
              }
              break;
            case 'P':
              {
                DskHttpRequestOptions req_options = DSK_HTTP_REQUEST_OPTIONS_INIT;
                DskHttpClientStreamRequestOptions cr_options = DSK_HTTP_CLIENT_STREAM_REQUEST_OPTIONS_INIT;
                DskHttpClientStreamTransfer *xfer;
                const char *pd_str = "this is post data\n";
                DskMemorySource *post_data;
                req_options.verb = DSK_HTTP_VERB_POST;
                req_options.host = "localhost";
                req_options.full_path = "/hello.txt";
                req_options.content_length = strlen (pd_str);
                cr_options.request_options = &req_options;
                cr_options.funcs = &request_funcs_0;
                cr_options.user_data = rd;
                post_data = dsk_memory_source_new ();
                cr_options.post_data = DSK_OCTET_SOURCE (post_data);
                dsk_buffer_append_string (&post_data->buffer, pd_str);
                xfer = dsk_http_client_stream_request (stream, &cr_options, &error);
                dsk_assert (xfer != NULL);
                dsk_memory_source_added_data (post_data);
                dsk_memory_source_done_adding (post_data);
                dsk_object_unref (post_data);
              }
              break;
            case 'T':
              {
                DskHttpRequestOptions req_options = DSK_HTTP_REQUEST_OPTIONS_INIT;
                DskHttpClientStreamRequestOptions cr_options = DSK_HTTP_CLIENT_STREAM_REQUEST_OPTIONS_INIT;
                DskHttpClientStreamTransfer *xfer;
                const char *pd_str = "this is post data\n";
                DskMemorySource *post_data;
                req_options.verb = DSK_HTTP_VERB_POST;
                req_options.host = "localhost";
                req_options.full_path = "/hello.txt";
                cr_options.request_options = &req_options;
                cr_options.funcs = &request_funcs_0;
                post_data = dsk_memory_source_new ();
                cr_options.post_data = DSK_OCTET_SOURCE (post_data);
                cr_options.user_data = rd;
                dsk_buffer_append_string (&post_data->buffer, pd_str);
                xfer = dsk_http_client_stream_request (stream, &cr_options, &error);
                dsk_assert (xfer != NULL);
                dsk_memory_source_added_data (post_data);
                dsk_memory_source_done_adding (post_data);
              }
              break;

              /* === responses === */
            case 'R':
              {
                char content_buf[8];
                /* write response to client stream */
                dsk_buffer_append_string (&source->buffer,
                                          "HTTP/1.1 200 Ok\r\n"
                                          "Content-Length: 8\r\n"
                                          "\r\n"
                                          "hi mom!\n"  /* 8 bytes */
                                         );
                if (add_postcontent_newlines)
                  dsk_buffer_append_string (&source->buffer, "\n");
                dsk_memory_source_added_data (source);

                /* handle response from DskHttpClientStream */
                while (!rd->destroyed)
                  dsk_main_run_once ();
                dsk_assert (rd->response_header != NULL);
                dsk_assert (rd->response_header->content_length == 8LL);
                dsk_assert (!rd->response_header->transfer_encoding_chunked);
                dsk_assert (rd->response_header->http_major_version == 1);
                dsk_assert (rd->response_header->http_minor_version == 1);
                dsk_assert (rd->response_header->status_code == 200);
                dsk_assert (rd->content_complete);
                dsk_assert (rd->content.size == 8);
                dsk_buffer_read (&rd->content, 8, content_buf);
                dsk_assert (memcmp (content_buf, "hi mom!\n", 8) == 0);
              }
              break;
            case 'C':
              {
                char content_buf[8];
                /* write response to client stream */
                dsk_buffer_append_string (&source->buffer,
                                          "HTTP/1.1 200 Ok\r\n"
                                          "Transfer-Encoding: chunked\r\n"
                                          "\r\n"
                                          "3\r\nhi \r\n"
                                          "5\r\nmom!\n\r\n"
                                          "0\r\n\r\n"
                                         );
                if (add_postcontent_newlines)
                  dsk_buffer_append_string (&source->buffer, "\r\n");
                dsk_memory_source_added_data (source);

                /* handle response from DskHttpClientStream */
                while (!rd->destroyed)
                  dsk_main_run_once ();
                dsk_assert (rd->response_header != NULL);
                dsk_assert (rd->response_header->content_length == -1LL);
                dsk_assert (rd->response_header->transfer_encoding_chunked);
                dsk_assert (rd->response_header->http_major_version == 1);
                dsk_assert (rd->response_header->http_minor_version == 1);
                dsk_assert (rd->response_header->status_code == 200);
                dsk_assert (rd->content_complete);
                dsk_assert (rd->content.size == 8);
                dsk_buffer_read (&rd->content, 8, content_buf);
                dsk_assert (memcmp (content_buf, "hi mom!\n", 8) == 0);
                
              }
              break;
            default:
              dsk_assert_not_reached ();
            }
          if (is_query)
            n_requests++;
          else
            {
              n_responses++;
              request_data_clear (rd);
            }
          at++;
        }
      if (debug)
        fprintf(stderr, "]");
      dsk_object_unref (stream);
      dsk_object_unref (source);
      dsk_object_unref (sink);
    }
}
static void
test_keepalive (void)
{
  test_keepalive_full (DSK_FALSE);
}
static void
test_keepalive_old_broken_clients (void)
{
  test_keepalive_full (DSK_TRUE);
}

static DskError *
test_bad_response (const char *response)
{
  DskHttpClientStream *stream;
  DskHttpClientStreamOptions options = DSK_HTTP_CLIENT_STREAM_OPTIONS_INIT;
  DskHttpClientStreamRequestOptions cr_options = DSK_HTTP_CLIENT_STREAM_REQUEST_OPTIONS_INIT;
  RequestData request_data = REQUEST_DATA_DEFAULT;
  DskHttpRequestOptions req_options = DSK_HTTP_REQUEST_OPTIONS_INIT;
  DskHttpClientStreamFuncs request_funcs_0;
  DskError *error = NULL;
  memset (&request_funcs_0, 0, sizeof (request_funcs_0));
  request_funcs_0.handle_response = request_data__handle_response;
  request_funcs_0.handle_content_complete = request_data__handle_content_complete;
  request_funcs_0.handle_error = request_data__handle_error;
  request_funcs_0.destroy = request_data__destroy;
  request_data.source = dsk_memory_source_new ();
  request_data.sink = dsk_memory_sink_new ();
  request_data.sink->max_buffer_size = 100000000;
  options.print_warnings = DSK_FALSE;
  stream = dsk_http_client_stream_new (DSK_OCTET_SINK (request_data.sink),
                                       DSK_OCTET_SOURCE (request_data.source),
                                       &options);
  req_options.full_path = "/hello.txt";
  cr_options.request_options = &req_options;
  cr_options.user_data = &request_data;
  cr_options.funcs = &request_funcs_0;
  dsk_http_client_stream_request (stream, &cr_options, NULL);
  dsk_buffer_append_string (&request_data.source->buffer, response);
  dsk_memory_source_added_data (request_data.source);
  dsk_memory_source_done_adding (request_data.source);
  while (!request_data.destroyed)
    dsk_main_run_once ();
  dsk_assert (request_data.error != NULL);
  error = dsk_error_ref (request_data.error);
  request_data_clear (&request_data);
  dsk_object_unref (stream);
  return error;
}

static void
test_bad_responses (void)
{
  DskError *e;

  fprintf (stderr, ".");
  e = test_bad_response ("HTTP/1.1 qwerty\r\n"
                         "Content-Length: 7\r\n"
                         "\r\n"
                         "hi mom\n");
  if (cmdline_print_error_messages)
    fprintf (stderr, "bad nonnumeric status code error: %s\n", e->message);
  dsk_error_unref (e);

  fprintf (stderr, ".");
  e = test_bad_response ("HTTP/1.1 999\r\n"
                         "Content-Length: 7\r\n"
                         "\r\n"
                         "hi mom\n");
  if (cmdline_print_error_messages)
    fprintf (stderr, "bad numeric status code error: %s\n", e->message);
  dsk_error_unref (e);

  fprintf (stderr, ".");
  e = test_bad_response ("HTTP/1.1 200 Ok\r\n"
                         "Transfer-Encoding: chunked\r\n"
                         "\r\n"
                         "7x\r\nhi mom\n0\r\n\r\n");
  if (cmdline_print_error_messages)
    fprintf (stderr, "bad chunk header: %s\n", e->message);
  dsk_error_unref (e);

  fprintf (stderr, ".");
  e = test_bad_response ("HTTP/1.1 200 Ok\r\n"
                         "Transfer-Encoding: chunked\r\n"
                         "\r\n"
                         "7\r\nhi mom\n0x\r\n\r\n");
  if (cmdline_print_error_messages)
    fprintf (stderr, "bad final chunk header: %s\n", e->message);
  dsk_error_unref (e);
}

static void
test_gzip_download (void)
{
  dsk_boolean byte_by_byte;
  static const char response0[] =
      "HTTP/1.1 200 OK\r\n"
      "Date: Mon, 17 May 2010 22:50:08 GMT\r\n"
      "Content-Type: text/plain\r\n"
      "Content-Encoding: gzip\r\n"
      "Content-Length: 27\r\n"
      "Connection: close\r\n"
      "\r\n"
      "\37\213\b\0\0\0\0\0\0\3\313\310T\310\315\317\345\2\0]\363Nq\a\0\0\0";

  static const char response1[] =
      "HTTP/1.1 200 OK\n"
      "Date: Mon, 17 May 2010 22:50:08 GMT\n"
      "Content-Type: text/plain\n"
      "Content-Length: 27\n"
      "Content-Encoding: gzip\r\n"
      "Connection: close\n"
      "\n"
      "\37\213\b\0\0\0\0\0\0\3\313\310T\310\315\317\345\2\0]\363Nq\a\0\0\0";
  static const char response2[] =
      "HTTP/1.1 200 OK\n"
      "Date: Mon, 17 May 2010 22:50:08 GMT\n"
      "Content-Type: text/plain\n"
      "Content-Encoding: gzip\r\n"
      "Transfer-Encoding: chunked\r\n"
      "\n"
      "1b\r\n"
       "\37\213\b\0\0\0\0\0\0\3\313\310T\310\315\317\345\2\0]\363Nq\a\0\0\0\r\n"
      "0\r\n\r\n";
  static const char response3[] =
      "HTTP/1.1 200 OK\n"
      "Date: Mon, 17 May 2010 22:50:08 GMT\n"
      "Content-Type: text/plain\n"
      "Content-Encoding: gzip\r\n"
      "Transfer-Encoding: chunked\r\n"
      "\n"
      "1\r\n\37\r\n"
      "1\r\n\213\r\n"
      "1\r\n\b\r\n"
      "1\r\n\0\r\n"
      "1\r\n\0\r\n"
      "1\r\n\0\r\n"
      "1\r\n\0\r\n"
      "1\r\n\0\r\n"
      "1\r\n\0\r\n"
      "1\r\n\3\r\n"
      "1\r\n\313\r\n"
      "1\r\n\310\r\n"
      "1\r\nT\r\n"
      "1\r\n\310\r\n"
      "1\r\n\315\r\n"
      "1\r\n\317\r\n"
      "1\r\n\345\r\n"
      "1\r\n\2\r\n"
      "1\r\n\0\r\n"
      "1\r\n]\r\n"
      "1\r\n\363\r\n"
      "1\r\nN\r\n"
      "1\r\nq\r\n"
      "1\r\n\a\r\n"
      "1\r\n\0\r\n"
      "1\r\n\0\r\n"
      "1\r\n\0\r\n"
      "0\r\n\r\n";

  static struct {
    unsigned length;
    const char *data;
    /* the sizeof(static_arr) includes the terminal NUL, so subtract 1 */
#define MK_ENTRY(static_arr) { sizeof(static_arr)-1, static_arr }
  } response_content_versions[] = {
    MK_ENTRY(response0),
    MK_ENTRY(response1),
    MK_ENTRY(response2),
    MK_ENTRY(response3)
  };
#undef MK_ENTRY
  for (byte_by_byte = 0; byte_by_byte <= 1; byte_by_byte++)
    {
      unsigned iter;

      for (iter = 0; iter < DSK_N_ELEMENTS (response_content_versions); iter++)
        {
          DskHttpClientStream *stream;
          DskHttpClientStreamOptions options = DSK_HTTP_CLIENT_STREAM_OPTIONS_INIT;
          RequestData request_data = REQUEST_DATA_DEFAULT;
          DskHttpRequestOptions req_options = DSK_HTTP_REQUEST_OPTIONS_INIT;
          DskHttpClientStreamRequestOptions cr_options = DSK_HTTP_CLIENT_STREAM_REQUEST_OPTIONS_INIT;
          DskHttpClientStreamTransfer *xfer;
          DskHttpClientStreamFuncs request_funcs_0;
          DskError *error = NULL;
          fprintf (stderr, ".");
          memset (&request_funcs_0, 0, sizeof (request_funcs_0));
          request_funcs_0.handle_response = request_data__handle_response;
          request_funcs_0.handle_content_complete = request_data__handle_content_complete;
          request_funcs_0.destroy = request_data__destroy;
          request_data.source = dsk_memory_source_new ();
          request_data.sink = dsk_memory_sink_new ();
          request_data.sink->max_buffer_size = 100000000;
          stream = dsk_http_client_stream_new (DSK_OCTET_SINK (request_data.sink),
                                               DSK_OCTET_SOURCE (request_data.source),
                                               &options);
          req_options.host = "localhost";
          req_options.full_path = "/hello.txt";
          cr_options.request_options = &req_options;
          cr_options.funcs = &request_funcs_0;
          cr_options.user_data = &request_data;
          xfer = dsk_http_client_stream_request (stream, &cr_options, &error);
          if (xfer == NULL)
            dsk_die ("dsk_http_client_stream_request failed: %s", error->message);

          /* read data from sink */
          while (!is_http_request_complete (&request_data.sink->buffer, NULL))
            dsk_main_run_once ();

          {
          const char *content = response_content_versions[iter].data;
          unsigned len = response_content_versions[iter].length;

          /* write response */
          if (byte_by_byte)
            {
              const char *at = content;
              unsigned rem = len;
              while (rem > 0)
                {
                  dsk_buffer_append_byte (&request_data.source->buffer, *at++);
                  dsk_memory_source_added_data (request_data.source);
                  while (request_data.source->buffer.size)
                    dsk_main_run_once ();
                  rem--;
                }
            }
          else
            {
              dsk_buffer_append (&request_data.source->buffer, len, content);
              dsk_memory_source_added_data (request_data.source);
            }
          }

          while (request_data.response_header == NULL)
            dsk_main_run_once ();
          dsk_assert (request_data.response_header->http_major_version == 1);
          dsk_assert (request_data.response_header->http_minor_version == 1);
          if (iter < 2)
            {
              dsk_assert (!request_data.response_header->transfer_encoding_chunked);
              dsk_assert (request_data.response_header->connection_close);
            }
          else
            {
              dsk_assert (request_data.response_header->transfer_encoding_chunked);
              dsk_assert (!request_data.response_header->connection_close);
            }
          while (!request_data.content_complete)
            dsk_main_run_once ();

          dsk_assert (request_data.content.size == 7);
          {
            char buf[7];
            dsk_buffer_peek (&request_data.content, 7, buf);
            dsk_assert (memcmp (buf, "hi mom\n", 7) == 0);
          }

          dsk_object_unref (stream);
          request_data_clear (&request_data);
        }
    }
}

/* this is gzipped data from the string "this is post data\n" */
static const char this_is_post_data__gzipped[] = 
  "\37\213\b\0\0\0\0\0\2\3+\311\310,V\0\242\202\374\342\22\205\224\304\222D.\0\255M[:\22\0\0\0";

static void
test_pre_gzipped_post_data_0 (void)
{
  DskHttpClientStream *stream;
  DskHttpClientStreamOptions options = DSK_HTTP_CLIENT_STREAM_OPTIONS_INIT;
  RequestData request_data = REQUEST_DATA_DEFAULT;
  DskHttpRequestOptions req_options = DSK_HTTP_REQUEST_OPTIONS_INIT;
  DskHttpClientStreamRequestOptions cr_options = DSK_HTTP_CLIENT_STREAM_REQUEST_OPTIONS_INIT;
  DskHttpClientStreamTransfer *xfer;
  DskHttpClientStreamFuncs request_funcs_0;
  DskError *error = NULL;
  unsigned req_hdr_len;
  unsigned gzipped_post_len = sizeof (this_is_post_data__gzipped) - 1;
  fprintf (stderr, ".");
  memset (&request_funcs_0, 0, sizeof (request_funcs_0));
  request_funcs_0.handle_response = request_data__handle_response;
  request_funcs_0.handle_content_complete = request_data__handle_content_complete;
  request_funcs_0.destroy = request_data__destroy;
  request_data.source = dsk_memory_source_new ();
  request_data.sink = dsk_memory_sink_new ();
  request_data.sink->max_buffer_size = 100000000;
  stream = dsk_http_client_stream_new (DSK_OCTET_SINK (request_data.sink),
                                       DSK_OCTET_SOURCE (request_data.source),
                                       &options);
  req_options.host = "localhost";
  req_options.full_path = "/hello.txt";
  req_options.verb = DSK_HTTP_VERB_POST;
  cr_options.request_options = &req_options;
  cr_options.funcs = &request_funcs_0;
  cr_options.user_data = &request_data;
  cr_options.post_data_length = gzipped_post_len;
  cr_options.post_data_slab = (void*) this_is_post_data__gzipped;
  cr_options.post_data_is_gzipped = DSK_TRUE;
  xfer = dsk_http_client_stream_request (stream, &cr_options, &error);
  if (xfer == NULL)
    dsk_die ("dsk_http_client_stream_request failed: %s", error->message);

  /* read data from sink */
  while (!is_http_request_complete (&request_data.sink->buffer, &req_hdr_len))
    dsk_main_run_once ();

  /* verify header is ok */
  {
    char *hdr = dsk_malloc (req_hdr_len+1);
    char buf[256];
    dsk_buffer_read (&request_data.sink->buffer, req_hdr_len, hdr);
    hdr[req_hdr_len] = 0;
    snprintf (buf, sizeof(buf), "Content-Length: %u", gzipped_post_len);
    dsk_assert (strstr (hdr, buf) != NULL);
    dsk_assert (dsk_ascii_isspace (strstr (hdr, buf)[strlen(buf)]));
    dsk_assert (strstr (hdr, "Content-Encoding: gzip") != NULL);
    dsk_free (hdr);
  }

  /* read/verify POST data */
  {
    char *data = dsk_malloc (gzipped_post_len);
    dsk_assert (request_data.sink->buffer.size == gzipped_post_len);
    dsk_buffer_read (&request_data.sink->buffer, gzipped_post_len, data);
    dsk_assert (memcmp (this_is_post_data__gzipped, data, gzipped_post_len) == 0);
    dsk_free (data);
  }

  /* write response */
  dsk_buffer_append_string (&request_data.source->buffer,
                            "HTTP/1.0 200 Success\r\n"
                            "Content-Length: 7\r\n"
                            "Connection: close\r\n"
                            "\r\n"
                            "hi mom\n");
  dsk_memory_source_added_data (request_data.source);

  while (request_data.response_header == NULL)
    dsk_main_run_once ();
  dsk_assert (request_data.response_header->http_major_version == 1);
  dsk_assert (request_data.response_header->http_minor_version == 0);
  dsk_assert (!request_data.response_header->transfer_encoding_chunked);
  dsk_assert (request_data.response_header->connection_close);
  while (!request_data.content_complete)
    dsk_main_run_once ();

  dsk_assert (request_data.content.size == 7);
  {
    char buf[7];
    dsk_buffer_peek (&request_data.content, 7, buf);
    dsk_assert (memcmp (buf, "hi mom\n", 7) == 0);
  }

  dsk_object_unref (stream);
  request_data_clear (&request_data);
}

static void
test_pre_gzipped_post_data_1 (void)
{
  DskHttpClientStream *stream;
  DskHttpClientStreamOptions options = DSK_HTTP_CLIENT_STREAM_OPTIONS_INIT;
  RequestData request_data = REQUEST_DATA_DEFAULT;
  DskHttpRequestOptions req_options = DSK_HTTP_REQUEST_OPTIONS_INIT;
  DskHttpClientStreamRequestOptions cr_options = DSK_HTTP_CLIENT_STREAM_REQUEST_OPTIONS_INIT;
  DskHttpClientStreamTransfer *xfer;
  DskHttpClientStreamFuncs request_funcs_0;
  DskError *error = NULL;
  unsigned gzipped_post_len = sizeof (this_is_post_data__gzipped) - 1;
  char buf[64];
  int index;
  DskMemorySource *post_data;
  unsigned req_hdr_len;
  unsigned n_written, n_read;

  fprintf (stderr, ".");
  memset (&request_funcs_0, 0, sizeof (request_funcs_0));
  request_funcs_0.handle_response = request_data__handle_response;
  request_funcs_0.handle_content_complete = request_data__handle_content_complete;
  request_funcs_0.destroy = request_data__destroy;
  request_data.source = dsk_memory_source_new ();
  request_data.sink = dsk_memory_sink_new ();
  request_data.sink->max_buffer_size = 100000000;
  stream = dsk_http_client_stream_new (DSK_OCTET_SINK (request_data.sink),
                                       DSK_OCTET_SOURCE (request_data.source),
                                       &options);
  req_options.host = "localhost";
  req_options.full_path = "/hello.txt";
  req_options.verb = DSK_HTTP_VERB_POST;
  cr_options.request_options = &req_options;
  cr_options.funcs = &request_funcs_0;
  cr_options.user_data = &request_data;
  post_data = dsk_memory_source_new ();
  cr_options.post_data = DSK_OCTET_SOURCE (post_data);
  cr_options.post_data_is_gzipped = DSK_TRUE;
  xfer = dsk_http_client_stream_request (stream, &cr_options, &error);
  if (xfer == NULL)
    dsk_die ("dsk_http_client_stream_request failed: %s", error->message);

  /* read data from sink */
  while (!is_http_request_complete (&request_data.sink->buffer, &req_hdr_len))
    dsk_main_run_once ();

  /* verify header is ok */
  {
    char *hdr = dsk_malloc (req_hdr_len+1);
    dsk_buffer_read (&request_data.sink->buffer, req_hdr_len, hdr);
    hdr[req_hdr_len] = 0;
    dsk_assert (strstr (hdr, "Transfer-Encoding: chunked") != NULL);
    dsk_assert (strstr (hdr, "Content-Encoding: gzip") != NULL);
    dsk_free (hdr);
  }

  for (n_written = 0; n_written < gzipped_post_len; n_written++)
    {
      dsk_buffer_append_byte (&post_data->buffer,
                              this_is_post_data__gzipped[n_written]);
      dsk_memory_source_added_data (post_data);
      while (post_data->buffer.size)
        dsk_main_run_once ();
    }
  dsk_memory_source_done_adding (post_data);
  dsk_object_unref (post_data);

  /* handle eof marker?? */
  dsk_main_run_once ();

  /* read/verify POST data */
  for (n_read = 0; n_read < gzipped_post_len; n_read++)
    {
      /* read chunk header */
      index = dsk_buffer_index_of (&request_data.sink->buffer, '\n');
      dsk_assert (index >= 0);
      dsk_assert (index < (int)sizeof (buf));
      dsk_buffer_read (&request_data.sink->buffer, index + 1, buf);
      buf[index] = 0;
      dsk_assert (buf[0] == '1');
      dsk_assert (buf[1] == 0 || buf[1] == '\r');

      /* read chunk data */
      dsk_assert (request_data.sink->buffer.size != 0);
      dsk_assert ((uint8_t)dsk_buffer_read_byte (&request_data.sink->buffer)
               == (uint8_t)this_is_post_data__gzipped[n_read]);
      index = dsk_buffer_index_of (&request_data.sink->buffer, '\n');
      dsk_assert (index >= 0);
      dsk_assert (index <= 2);
      dsk_buffer_discard (&request_data.sink->buffer, index + 1);
    }
  /* read chunked-data end */
  index = dsk_buffer_index_of (&request_data.sink->buffer, '\n');
  dsk_assert (index >= 0);
  dsk_assert (index < (int)sizeof (buf));
  dsk_buffer_read (&request_data.sink->buffer, index + 1, buf);
  buf[index] = 0;
  dsk_assert (buf[0] == '0');
  dsk_assert (buf[1] == 0 || buf[1] == '\r');
  index = dsk_buffer_index_of (&request_data.sink->buffer, '\n');
  dsk_assert (index >= 0);
  dsk_buffer_discard (&request_data.sink->buffer, index+1);

  /* buffer should now be empty */
  dsk_assert (request_data.sink->buffer.size == 0);

  /* write response */
  dsk_buffer_append_string (&request_data.source->buffer,
                            "HTTP/1.1 200 Success\r\n"
                            "Content-Length: 7\r\n"
                            "Connection: close\r\n"
                            "\r\n"
                            "hi mom\n");
  dsk_memory_source_added_data (request_data.source);

  while (request_data.response_header == NULL)
    dsk_main_run_once ();
  dsk_assert (request_data.response_header->http_major_version == 1);
  dsk_assert (request_data.response_header->http_minor_version == 1);
  dsk_assert (!request_data.response_header->transfer_encoding_chunked);
  dsk_assert (request_data.response_header->connection_close);
  while (!request_data.content_complete)
    dsk_main_run_once ();

  dsk_assert (request_data.content.size == 7);
  {
    char buf[7];
    dsk_buffer_peek (&request_data.content, 7, buf);
    dsk_assert (memcmp (buf, "hi mom\n", 7) == 0);
  }

  dsk_object_unref (stream);
  request_data_clear (&request_data);
}

static void
test_bad_gzip (void)
{
  dsk_boolean byte_by_byte;
  static const char response0[] =
      "HTTP/1.1 200 OK\r\n"
      "Date: Mon, 17 May 2010 22:50:08 GMT\r\n"
      "Content-Type: text/plain\r\n"
      "Content-Encoding: gzip\r\n"
      "Content-Length: 27\r\n"
      "Connection: close\r\n"
      "\r\n"
      "\32\213\b\0\0\1\0\0\0\3\313\310T\311\315\317\345\2\0]\363Nq\a\0\0\0";

  static const char response1[] =
      "HTTP/1.1 200 OK\n"
      "Date: Mon, 17 May 2010 22:50:08 GMT\n"
      "Content-Type: text/plain\n"
      "Content-Length: 27\n"
      "Content-Encoding: gzip\r\n"
      "Connection: close\n"
      "\n"
      "\32\213\b\0\0\2\0\0\0\3\313\310T\311\315\317\345\2\0]\363Nq\a\0\0\0";
  static const char response2[] =
      "HTTP/1.1 200 OK\n"
      "Date: Mon, 17 May 2010 22:50:08 GMT\n"
      "Content-Type: text/plain\n"
      "Content-Encoding: gzip\r\n"
      "Transfer-Encoding: chunked\r\n"
      "\n"
      "1b\r\n"
       "\31\213\b\0\0\0\0\0\0\4\313\310T\310\315\317\345\2\0]\363Nq\a\0\0\0\r\n"
      "0\r\n\r\n";
  static const char response3[] =
      "HTTP/1.1 200 OK\n"
      "Date: Mon, 17 May 2010 22:50:08 GMT\n"
      "Content-Type: text/plain\n"
      "Content-Encoding: gzip\r\n"
      "Transfer-Encoding: chunked\r\n"
      "\n"
      "1\r\n\32\r\n"
      "1\r\n\213\r\n"
      "1\r\n\b\r\n"
      "1\r\n\0\r\n"
      "1\r\n\0\r\n"
      "1\r\n\0\r\n"
      "1\r\n\0\r\n"
      "1\r\n\1\r\n"
      "1\r\n\0\r\n"
      "1\r\n\3\r\n"
      "1\r\n\314\r\n"
      "1\r\n\310\r\n"
      "1\r\nT\r\n"
      "1\r\n\310\r\n"
      "1\r\n\315\r\n"
      "1\r\n\317\r\n"
      "1\r\n\345\r\n"
      "1\r\n\2\r\n"
      "1\r\n\0\r\n"
      "1\r\n]\r\n"
      "1\r\n\363\r\n"
      "1\r\nN\r\n"
      "1\r\nq\r\n"
      "1\r\n\a\r\n"
      "1\r\n\0\r\n"
      "1\r\n\0\r\n"
      "1\r\n\0\r\n"
      "0\r\n\r\n";

  static struct {
    unsigned length;
    const char *data;
    /* the sizeof(static_arr) includes the terminal NUL, so subtract 1 */
#define MK_ENTRY(static_arr) { sizeof(static_arr)-1, static_arr }
  } response_content_versions[] = {
    MK_ENTRY(response0),
    MK_ENTRY(response1),
    MK_ENTRY(response2),
    MK_ENTRY(response3)
  };
#undef MK_ENTRY
  for (byte_by_byte = 0; byte_by_byte <= 1; byte_by_byte++)
    {
      unsigned iter;

      for (iter = 0; iter < DSK_N_ELEMENTS (response_content_versions); iter++)
        {
          DskHttpClientStream *stream;
          DskHttpClientStreamOptions options = DSK_HTTP_CLIENT_STREAM_OPTIONS_INIT;
          RequestData request_data = REQUEST_DATA_DEFAULT;
          DskHttpRequestOptions req_options = DSK_HTTP_REQUEST_OPTIONS_INIT;
          DskHttpClientStreamRequestOptions cr_options = DSK_HTTP_CLIENT_STREAM_REQUEST_OPTIONS_INIT;
          DskHttpClientStreamTransfer *xfer;
          DskHttpClientStreamFuncs request_funcs_0;
          DskError *error = NULL;
          fprintf (stderr, ".");
          memset (&request_funcs_0, 0, sizeof (request_funcs_0));
          request_funcs_0.handle_response = request_data__handle_response;
          request_funcs_0.handle_content_complete = request_data__handle_content_complete;
          request_funcs_0.handle_error = request_data__handle_error;
          request_funcs_0.destroy = request_data__destroy;
          request_data.source = dsk_memory_source_new ();
          request_data.sink = dsk_memory_sink_new ();
          request_data.sink->max_buffer_size = 100000000;
          options.print_warnings = DSK_FALSE;
          stream = dsk_http_client_stream_new (DSK_OCTET_SINK (request_data.sink),
                                               DSK_OCTET_SOURCE (request_data.source),
                                               &options);
          req_options.host = "localhost";
          req_options.full_path = "/hello.txt";
          cr_options.request_options = &req_options;
          cr_options.funcs = &request_funcs_0;
          cr_options.user_data = &request_data;
          xfer = dsk_http_client_stream_request (stream, &cr_options, &error);
          if (xfer == NULL)
            dsk_die ("dsk_http_client_stream_request failed: %s", error->message);

          /* read data from sink */
          while (!is_http_request_complete (&request_data.sink->buffer, NULL))
            dsk_main_run_once ();

          {
          const char *content = response_content_versions[iter].data;
          unsigned len = response_content_versions[iter].length;

          /* write response */
          if (byte_by_byte)
            {
              const char *at = content;
              unsigned rem = len;
              while (rem > 0 && request_data.error == NULL)
                {
                  dsk_buffer_append_byte (&request_data.source->buffer, *at++);
                  dsk_memory_source_added_data (request_data.source);
                  while (request_data.source->buffer.size
                      && request_data.error == NULL)
                    dsk_main_run_once ();
                  rem--;
                }
            }
          else
            {
              dsk_buffer_append (&request_data.source->buffer, len, content);
              dsk_memory_source_added_data (request_data.source);
            }
          }

          while (request_data.response_header == NULL)
            dsk_main_run_once ();
          dsk_assert (request_data.response_header->http_major_version == 1);
          dsk_assert (request_data.response_header->http_minor_version == 1);
          if (iter < 2)
            {
              dsk_assert (!request_data.response_header->transfer_encoding_chunked);
              dsk_assert (request_data.response_header->connection_close);
            }
          else
            {
              dsk_assert (request_data.response_header->transfer_encoding_chunked);
              dsk_assert (!request_data.response_header->connection_close);
            }
          while (request_data.error == NULL)
            dsk_main_run_once ();

          dsk_object_unref (stream);
          request_data_clear (&request_data);
        }
    }
}

static void
test_transport_errors_reading (void)
{
  static const char *response_content_versions[4] =  {
                                "HTT",

                                "HTTP/1.1 200 OK\r\n",

                                "HTTP/1.1 200 OK\r\n"
                                "Date: Mon, 17 May 2010 22:50:08 GMT\r\n"
                                "Content-Type: text/plain\r\n"
                                "Content-Length: 7\r\n"
                                "Connection: close\r\n"
                                "\r\n"
                                "hi \n",

                                "HTTP/1.1 200 OK\n"
                                "Date: Mon, 17 May 2010 22:50:08 GMT\n"
                                "Content-Type: text/plain\n"
                                "Transfer-Encoding: chunked\n"
                                "\n"
                                "6\r\nhi mom\r\n",

                        };
  unsigned iter;

  for (iter = 0; iter < DSK_N_ELEMENTS (response_content_versions); iter++)
    {
      DskHttpClientStream *stream;
      DskHttpClientStreamOptions options = DSK_HTTP_CLIENT_STREAM_OPTIONS_INIT;
      RequestData request_data = REQUEST_DATA_DEFAULT;
      DskHttpRequestOptions req_options = DSK_HTTP_REQUEST_OPTIONS_INIT;
      DskHttpClientStreamRequestOptions cr_options = DSK_HTTP_CLIENT_STREAM_REQUEST_OPTIONS_INIT;
      DskHttpClientStreamTransfer *xfer;
      DskHttpClientStreamFuncs request_funcs_0;
      DskError *error = NULL;
      fprintf (stderr, ".");
      memset (&request_funcs_0, 0, sizeof (request_funcs_0));
      request_funcs_0.handle_response = request_data__handle_response;
      request_funcs_0.handle_content_complete = request_data__handle_content_complete;
      request_funcs_0.handle_error = request_data__handle_error;
      request_funcs_0.destroy = request_data__destroy;
      request_data.source = dsk_memory_source_new ();
      request_data.sink = dsk_memory_sink_new ();
      request_data.sink->max_buffer_size = 100000000;
      options.print_warnings = DSK_FALSE;
      stream = dsk_http_client_stream_new (DSK_OCTET_SINK (request_data.sink),
                                           DSK_OCTET_SOURCE (request_data.source),
                                           &options);
      req_options.host = "localhost";
      req_options.full_path = "/hello.txt";
      cr_options.request_options = &req_options;
      cr_options.funcs = &request_funcs_0;
      cr_options.user_data = &request_data;
      xfer = dsk_http_client_stream_request (stream, &cr_options, &error);
      if (xfer == NULL)
        dsk_die ("dsk_http_client_stream_request failed: %s", error->message);

      /* read data from sink */
      while (!is_http_request_complete (&request_data.sink->buffer, NULL))
        dsk_main_run_once ();

      {
      const char *content = response_content_versions[iter];

      /* write response */
      dsk_buffer_append_string (&request_data.source->buffer, content);
      dsk_memory_source_added_data (request_data.source);
      dsk_memory_source_take_error (request_data.source,
                                    dsk_error_new ("simulated test error"));
      }

      while (!request_data.destroyed)
        dsk_main_run_once ();

      dsk_assert (request_data.error != NULL);

      dsk_object_unref (stream);
      request_data_clear (&request_data);
    }
}

static void
test_random_response (void)
{
  unsigned iter;
  unsigned r = 5003;
  for (iter = 0; iter < 6400; iter++)
    {
      DskHttpClientStream *stream;
      DskHttpClientStreamOptions options = DSK_HTTP_CLIENT_STREAM_OPTIONS_INIT;
      RequestData request_data = REQUEST_DATA_DEFAULT;
      DskHttpRequestOptions req_options = DSK_HTTP_REQUEST_OPTIONS_INIT;
      DskHttpClientStreamRequestOptions cr_options = DSK_HTTP_CLIENT_STREAM_REQUEST_OPTIONS_INIT;
      unsigned i;
      DskHttpClientStreamTransfer *xfer;
      DskHttpClientStreamFuncs request_funcs_0;
      DskError *error = NULL;
      char rstr[1000];
      if (iter % 100 == 0)
        fprintf (stderr, ".");
      memset (&request_funcs_0, 0, sizeof (request_funcs_0));
      request_funcs_0.handle_response = request_data__handle_response;
      request_funcs_0.handle_content_complete = request_data__handle_content_complete;
      request_funcs_0.handle_error = request_data__handle_error;
      request_funcs_0.destroy = request_data__destroy;
      request_data.source = dsk_memory_source_new ();
      request_data.sink = dsk_memory_sink_new ();
      request_data.sink->max_buffer_size = 100000000;
      options.print_warnings = DSK_FALSE;
      stream = dsk_http_client_stream_new (DSK_OCTET_SINK (request_data.sink),
                                           DSK_OCTET_SOURCE (request_data.source),
                                           &options);
      req_options.host = "localhost";
      req_options.full_path = "/hello.txt";
      cr_options.request_options = &req_options;
      cr_options.funcs = &request_funcs_0;
      cr_options.user_data = &request_data;
      xfer = dsk_http_client_stream_request (stream, &cr_options, &error);
      DSK_UNUSED (xfer);

      while (!is_http_request_complete (&request_data.sink->buffer, NULL))
        dsk_main_run_once ();

      for (i = 0; i < sizeof(rstr); i++)
        {
          rstr[i] = r % 100252051;
          r *= 33;
          r += 8;
        }
      dsk_buffer_append (&request_data.source->buffer, sizeof (rstr), rstr);
      dsk_buffer_append_string (&request_data.source->buffer, "\r\n\r\n");
      dsk_memory_source_added_data (request_data.source);
      while (request_data.error == NULL)
        dsk_main_run_once ();
      dsk_object_unref (stream);
      request_data_clear (&request_data);

    }
}

/* === HEAD request tests === */

static void
test_head_simple (dsk_boolean byte_by_byte)
{
  static const char *response_content_versions[] =  {
                                "HTTP/1.1 200 OK\r\n"
                                "Date: Mon, 17 May 2010 22:50:08 GMT\r\n"
                                "Content-Type: text/plain\r\n"
                                "Content-Length: 7\r\n"
                                "Connection: close\r\n"
                                "\r\n",

                                "HTTP/1.1 200 OK\n"
                                "Date: Mon, 17 May 2010 22:50:08 GMT\n"
                                "Content-Type: text/plain\n"
                                "Content-Length: 7\n"
                                "Connection: close\n"
                                "\n",

                                "HTTP/1.1 200 OK\r\n"
                                "Date:\r\n"
                                "     Mon, 17 May 2010 22:50:08 GMT\r\n"
                                "Content-Type:\r\n"
                                "     text/plain\r\n"
                                "Content-Length:\r\n"
                                "     7\r\n"
                                "Connection:\r\n"
                                "     close\r\n"
                                "\r\n",

                  /* XXX: is 100 Continue allowed in response to HEAD? */
                                "HTTP/1.1 100 Continue\r\n"
                                "\r\n"
                                "HTTP/1.1 200 OK\r\n"
                                "Date: Mon, 17 May 2010 22:50:08 GMT\r\n"
                                "Content-Type: text/plain\r\n"
                                "Content-Length: 7\r\n"
                                "Connection: close\r\n"
                                "\r\n",
                        };
  unsigned iter;

  for (iter = 0; iter < DSK_N_ELEMENTS (response_content_versions); iter++)
    {
      DskHttpClientStream *stream;
      DskHttpClientStreamOptions options = DSK_HTTP_CLIENT_STREAM_OPTIONS_INIT;
      RequestData request_data = REQUEST_DATA_DEFAULT;
      DskHttpRequestOptions req_options = DSK_HTTP_REQUEST_OPTIONS_INIT;
      DskHttpClientStreamRequestOptions cr_options = DSK_HTTP_CLIENT_STREAM_REQUEST_OPTIONS_INIT;
      DskHttpClientStreamTransfer *xfer;
      DskHttpClientStreamFuncs request_funcs_0;
      DskError *error = NULL;
      fprintf (stderr, ".");
      memset (&request_funcs_0, 0, sizeof (request_funcs_0));
      request_funcs_0.handle_response = request_data__handle_response;
      request_funcs_0.handle_content_complete = NULL;
      request_funcs_0.destroy = request_data__destroy;
      request_data.source = dsk_memory_source_new ();
      request_data.sink = dsk_memory_sink_new ();
      request_data.sink->max_buffer_size = 100000000;
      stream = dsk_http_client_stream_new (DSK_OCTET_SINK (request_data.sink),
                                           DSK_OCTET_SOURCE (request_data.source),
                                           &options);
      req_options.host = "localhost";
      req_options.full_path = "/hello.txt";
      req_options.verb = DSK_HTTP_VERB_HEAD;
      cr_options.request_options = &req_options;
      cr_options.funcs = &request_funcs_0;
      cr_options.user_data = &request_data;
      xfer = dsk_http_client_stream_request (stream, &cr_options, &error);
      if (xfer == NULL)
        dsk_die ("dsk_http_client_stream_request failed: %s", error->message);

      /* read data from sink */
      while (!is_http_request_complete (&request_data.sink->buffer, NULL))
        dsk_main_run_once ();

      {
      const char *content = response_content_versions[iter];

      /* write response */
      if (byte_by_byte)
        {
          const char *at = content;
          while (*at)
            {
              dsk_buffer_append_byte (&request_data.source->buffer, *at++);
              dsk_memory_source_added_data (request_data.source);
              while (request_data.source->buffer.size)
                dsk_main_run_once ();
            }
        }
      else
        {
          dsk_buffer_append_string (&request_data.source->buffer, content);
          dsk_memory_source_added_data (request_data.source);
        }
      }

      while (request_data.response_header == NULL)
        dsk_main_run_once ();
      dsk_assert (request_data.response_header->http_major_version == 1);
      dsk_assert (request_data.response_header->http_minor_version == 1);
      dsk_assert (request_data.response_header->content_length == 7);
      dsk_assert (!request_data.response_header->transfer_encoding_chunked);
      dsk_assert (request_data.response_header->connection_close);

      dsk_object_unref (stream);
      request_data_clear (&request_data);
    }
}

static void test_head_simple_bigwrite() { test_head_simple(DSK_FALSE); }
static void test_head_simple_bytebybyte() { test_head_simple(DSK_TRUE); }

static void
test_head_transfer_encoding_chunked (void)
{
  unsigned iter;
  for (iter = 0; iter < 3; iter++)
    {
      DskHttpClientStream *stream;
      DskHttpClientStreamOptions options = DSK_HTTP_CLIENT_STREAM_OPTIONS_INIT;
      RequestData request_data = REQUEST_DATA_DEFAULT;
      DskHttpRequestOptions req_options = DSK_HTTP_REQUEST_OPTIONS_INIT;
      DskHttpClientStreamTransfer *xfer;
      DskHttpClientStreamFuncs request_funcs_0;
      DskError *error = NULL;
      const char *content;
      DskHttpClientStreamRequestOptions cr_options = DSK_HTTP_CLIENT_STREAM_REQUEST_OPTIONS_INIT;
      unsigned pass;
      switch (iter)
        {
        case 0:
          content = "HTTP/1.1 200 OK\r\n"
                    "Date: Mon, 17 May 2010 22:50:08 GMT\r\n"
                    "Content-Type: text/plain\r\n"
                    "Transfer-Encoding: chunked\r\n"
                    "\r\n";
          break;
        case 1:
          content = "HTTP/1.1 200 OK\r\n"
                    "Date: Mon, 17 May 2010 22:50:08 GMT\r\n"
                    "Content-Type: text/plain\r\n"
                    "Transfer-Encoding: chunked\r\n"
                    "\r\n";
          break;
        case 2:
          content = "HTTP/1.1 200 OK\r\n"
                    "Date: Mon, 17 May 2010 22:50:08 GMT\r\n"
                    "Content-Type: text/plain\r\n"
                    "Transfer-Encoding: chunked\r\n"
                    "\r\n";
          break;
        }
      memset (&request_funcs_0, 0, sizeof (request_funcs_0));
      request_funcs_0.handle_response = request_data__handle_response;
      request_funcs_0.handle_content_complete = request_data__handle_content_complete;
      request_funcs_0.destroy = request_data__destroy;
      request_data.source = dsk_memory_source_new ();
      request_data.sink = dsk_memory_sink_new ();
      request_data.sink->max_buffer_size = 100000000;
      stream = dsk_http_client_stream_new (DSK_OCTET_SINK (request_data.sink),
                                           DSK_OCTET_SOURCE (request_data.source),
                                           &options);
      req_options.verb = DSK_HTTP_VERB_HEAD;
      req_options.host = "localhost";
      req_options.full_path = "/hello.txt";
      cr_options.request_options = &req_options;
      cr_options.funcs = &request_funcs_0;
      cr_options.user_data = &request_data;

      /* write response */
      for (pass = 0; pass < 2; pass++)
        {
          fprintf (stderr, ".");
          xfer = dsk_http_client_stream_request (stream, &cr_options, &error);
          DSK_UNUSED (xfer);

          /* read data from sink */
          while (!is_http_request_complete (&request_data.sink->buffer, NULL))
            dsk_main_run_once ();

          switch (pass)
            {
            case 0:
              dsk_buffer_append_string (&request_data.source->buffer, content);
              dsk_memory_source_added_data (request_data.source);
              break;
            case 1:
              {
                const char *at = content;
                while (*at)
                  {
                    dsk_buffer_append_byte (&request_data.source->buffer, *at++);
                    dsk_memory_source_added_data (request_data.source);
                    while (request_data.source->buffer.size)
                      dsk_main_run_once ();
                  }
              }
              break;
            }

          while (request_data.response_header == NULL)
            dsk_main_run_once ();

          while (request_data.response_header == NULL)
            dsk_main_run_once ();
          dsk_assert (request_data.response_header->http_major_version == 1);
          dsk_assert (request_data.response_header->http_minor_version == 1);
          dsk_assert (request_data.response_header->content_length == -1LL);
          dsk_assert (request_data.response_header->transfer_encoding_chunked);
          dsk_assert (!request_data.response_header->connection_close);
          while (!request_data.content_complete)
            dsk_main_run_once ();

          dsk_assert (request_data.content.size == 0);

          request_data.content_complete = 0;
          request_data.destroyed = 0;
          dsk_object_unref (request_data.response_header);
          request_data.response_header = NULL;
          dsk_buffer_reset (&request_data.sink->buffer);
        }
      dsk_object_unref (stream);
      request_data_clear (&request_data);
    }
}

static void
test_simple_websocket (void)
{
  unsigned iter;
  for (iter = 0; iter < 1; iter++)
    {
      DskHttpClientStream *stream;
      DskHttpClientStreamOptions options = DSK_HTTP_CLIENT_STREAM_OPTIONS_INIT;
      DskHttpRequestOptions req_options = DSK_HTTP_REQUEST_OPTIONS_INIT;
      DskHttpClientStreamTransfer *xfer;
      DskHttpClientStreamFuncs request_funcs_websocket;
      DskError *error = NULL;
      const char *content;
      DskHttpClientStreamRequestOptions cr_options = DSK_HTTP_CLIENT_STREAM_REQUEST_OPTIONS_INIT;
      unsigned pass;
      switch (iter)
        {
        case 0:
          content = "HTTP/1.1 101 Websocket Protocol Handshake\r\n"
                    "Date: Mon, 17 May 2010 22:50:08 GMT\r\n"
                    "Upgrade: WebSocket\r\n"
                    "Connection: Upgrade\r\n"
                    "Sec-WebSocket-Origin: http://example.com\r\n"
                    "Sec-WebSocket-Location: ws://example.com/demo\r\n"
                    "Sec-WebSocket-Protocol: sample\r\n"
                    "\r\n";
          break;
        }
      memset (&request_funcs_websocket, 0, sizeof (request_funcs_websocket));
      request_funcs_websocket.handle_response = request_data__handle_response_websocket;
      request_funcs_websocket.handle_content_complete = NULL;
      request_funcs_websocket.destroy = request_data__destroy;
      req_options.host = "localhost";
      req_options.full_path = "/hello.txt";
      cr_options.request_options = &req_options;
      cr_options.funcs = &request_funcs_websocket;
      cr_options.is_websocket_request = DSK_TRUE;
      cr_options.websocket_protocols = "myproto";

      /* write response */
      for (pass = 0; pass < 2; pass++)
        {
          uint8_t response_handshake[16];
          DskHttpRequest *req;
          const char *key1 = NULL, *key2 = NULL;
          uint8_t key3[8];
          RequestData request_data = REQUEST_DATA_DEFAULT;
          request_data.source = dsk_memory_source_new ();
          request_data.sink = dsk_memory_sink_new ();
          request_data.sink->max_buffer_size = 100000000;
          cr_options.user_data = &request_data;
          stream = dsk_http_client_stream_new (DSK_OCTET_SINK (request_data.sink),
                                               DSK_OCTET_SOURCE (request_data.source),
                                               &options);

          fprintf (stderr, ".");
          xfer = dsk_http_client_stream_request (stream, &cr_options, &error);
          if (xfer == NULL)
            dsk_die ("error making websocket request: %s", error->message);

          /* read data from sink */
          unsigned request_len;
          while (!is_http_request_complete (&request_data.sink->buffer,
                                            &request_len))
            dsk_main_run_once ();

          /* parse request; extract key1 and key2 from request header */
          {
            unsigned uh;
            req = dsk_http_request_parse_buffer (&request_data.sink->buffer,
                                                                 request_len,
                                                                 &error);
            dsk_assert (req != NULL);

            /* hunt down key1 and key2 (and verify their length) */
            for (uh = 0; uh < req->n_unparsed_headers; uh++)
              if (dsk_ascii_strcasecmp (req->unparsed_headers[uh].key,
                                        "Sec-WebSocket-Key1") == 0)
                key1 = req->unparsed_headers[uh].value;
              else if (dsk_ascii_strcasecmp (req->unparsed_headers[uh].key,
                                             "Sec-WebSocket-Key2") == 0)
                key2 = req->unparsed_headers[uh].value;
            dsk_assert (key1 != NULL);
            dsk_assert (key1 != NULL);
          }

          /* read out key3 */
          dsk_buffer_discard (&request_data.sink->buffer, request_len);
          dsk_memory_sink_drained (request_data.sink);
          while (request_data.sink->buffer.size < 8)
            dsk_main_run_once ();
          dsk_buffer_read (&request_data.sink->buffer, 8, key3);

          /* compute websocket handshake response */
          if (!_dsk_websocket_compute_response (key1, key2, key3,
                                                response_handshake,
                                                &error))
            dsk_assert_not_reached ();

          dsk_object_unref (req);
          req = NULL;
          key1 = key2 = NULL;

          switch (pass)
            {
            case 0:
              dsk_buffer_append_string (&request_data.source->buffer, content);
              dsk_buffer_append (&request_data.source->buffer, 16, response_handshake);
              dsk_memory_source_added_data (request_data.source);
              break;
            case 1:
              {
                unsigned part;
                for (part = 0; part < 2; part++)
                  {
                    const char *at = content;
                    unsigned rem;
                    if (part == 0)
                      {
                        at = content;
                        rem = strlen (content);
                      }
                    else
                      {
                        const uint8_t *handshake = response_handshake;
                        at = (const char *) handshake;
                        rem = 16;
                      }
                    while (rem--)
                      {
                        dsk_buffer_append_byte (&request_data.source->buffer, *at++);
                        dsk_memory_source_added_data (request_data.source);
                        while (request_data.source->buffer.size)
                          dsk_main_run_once ();
                      }
                  }
              }
              break;
            }

          while (request_data.response_header == NULL)
            dsk_main_run_once ();
          dsk_assert (request_data.response_header->http_major_version == 1);
          dsk_assert (request_data.response_header->http_minor_version == 1);
          dsk_assert (request_data.response_header->content_length == -1LL);
          dsk_assert (request_data.response_header->connection_upgrade);
          dsk_assert (request_data.websocket != NULL);

          request_data.content_complete = 0;
          request_data.destroyed = 0;
          dsk_object_unref (request_data.response_header);
          request_data.response_header = NULL;
          dsk_buffer_reset (&request_data.sink->buffer);

          /* play with websocket */
          dsk_warning ("TODO: need more websocket testing");

          dsk_object_unref (stream);
          request_data_clear (&request_data);
        }
    }
}

static struct 
{
  const char *name;
  void (*test)(void);
} tests[] =
{
  { "simple connection-close", test_simple_bigwrite },
  { "simple connection-close byte-by-byte", test_simple_bytebybyte },
  { "simple POST", test_simple_post },
  { "transfer-encoding chunked content", test_transfer_encoding_chunked },
  { "transfer-encoding chunked POST", test_transfer_encoding_chunked_post },
  { "pipelining and keepalive", test_keepalive },
  { "pipelining and keepalive, on old HTTP servers", test_keepalive_old_broken_clients },
  { "bad responses", test_bad_responses },
  { "gzip uncompression", test_gzip_download },
  { "pre-gzipped POST data (slab)", test_pre_gzipped_post_data_0 },
  { "pre-gzipped POST data (chunked)", test_pre_gzipped_post_data_1 },
  { "bad gzipped data", test_bad_gzip },
  { "transport errors reading", test_transport_errors_reading },
  { "random-data response test", test_random_response },

  /* a subset of the above tests done with HEAD instead of GET */
  { "HEAD simple connection-close", test_head_simple_bigwrite },
  { "HEAD simple connection-close byte-by-byte", test_head_simple_bytebybyte },
  { "HEAD transfer-encoding chunked content", test_head_transfer_encoding_chunked },
  //{ "HEAD pipelining and keepalive", test_head_keepalive },
  //{ "HEAD pipelining and keepalive, on old HTTP servers", test_head_keepalive_old_broken_clients },
  //{ "HEAD bad responses", test_head_bad_responses },
  //{ "HEAD gzip uncompression", test_head_gzip_download },
  { "simple websocket", test_simple_websocket },
};

int main(int argc, char **argv)
{
  unsigned i;

  dsk_cmdline_init ("test HTTP client stream",
                    "Test the low-level HTTP client interface",
                    NULL, 0);
  dsk_cmdline_add_boolean ("verbose", "extra logging", NULL, 0,
                           &cmdline_verbose);
  dsk_cmdline_add_boolean ("print-error", "print error messages for bad-response tests", NULL, 0,
                           &cmdline_print_error_messages);
  dsk_cmdline_process_args (&argc, &argv);

  for (i = 0; i < DSK_N_ELEMENTS (tests); i++)
    {
      fprintf (stderr, "Test: %s... ", tests[i].name);
      tests[i].test ();
      fprintf (stderr, " done.\n");
    }
  dsk_cleanup ();
  return 0;
}
