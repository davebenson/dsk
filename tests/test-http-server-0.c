#include <sys/stat.h>
#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "../dsk.h"

static dsk_boolean cmdline_verbose = DSK_FALSE;

static void
respond_text_string (DskHttpServerRequest *request,
                     void                 *func_data)
{
  DskHttpServerResponseOptions response_options = DSK_HTTP_SERVER_RESPONSE_OPTIONS_INIT;
  const char *str = (const char *) func_data;
  unsigned length = strlen (str);
  DskMemorySource *source = dsk_memory_source_new ();
  dsk_buffer_append (&source->buffer, length, func_data);
  dsk_memory_source_added_data (source);
  dsk_memory_source_done_adding (source);
  response_options.source = (DskOctetSource *) source;
  response_options.content_length = length;             /* optional */
  dsk_http_server_request_respond (request, &response_options);
  dsk_object_unref (source);
}

static void
respond_sum (DskHttpServerRequest *request,
                     void                 *func_data)
{
  DskHttpServerResponseOptions response_options = DSK_HTTP_SERVER_RESPONSE_OPTIONS_INIT;
  int a = 0, b = 0;
  char sum_str[100];
  DskCgiVariable *var;
  DSK_UNUSED (func_data);
  var = dsk_http_server_request_lookup_cgi (request, "a");
  if (var)
    a = atoi (var->value);
  var = dsk_http_server_request_lookup_cgi (request, "b");
  if (var)
    b = atoi (var->value);
  snprintf (sum_str, sizeof (sum_str), "%d\n", a+b);
  response_options.content_length = strlen (sum_str);
  response_options.content_body = (uint8_t *) sum_str;
  dsk_http_server_request_respond (request, &response_options);
}
static void
internal_redirect (DskHttpServerRequest *request,
                     void                 *func_data)
{
  DskCgiVariable *path = dsk_http_server_request_lookup_cgi (request, "path");
  DSK_UNUSED (func_data);
  if (path == NULL)
    dsk_http_server_request_respond_error (request, 400,
                                           "missing path= cgi var");
  else
    dsk_http_server_request_internal_redirect (request, path->value);
}
static void
do_internal_redirect_timer_func (void *data)
{
  internal_redirect (data, NULL);
}
static void
internal_redirect_short_sleep (DskHttpServerRequest *request,
                               void                 *func_data)
{
  DSK_UNUSED (func_data);
  dsk_main_add_timer_millis (20, do_internal_redirect_timer_func, request);
}

typedef struct _SimpleRequestInfo SimpleRequestInfo;
struct _SimpleRequestInfo
{
  dsk_boolean done;
  dsk_boolean failed;
  DskBuffer content_buffer;
  DskHttpStatus status_code;
};
#define SIMPLE_REQUEST_INFO_INIT { DSK_FALSE, DSK_FALSE, DSK_BUFFER_INIT, 0 }

static void simple__handle_response (DskHttpClientStreamTransfer *xfer)
{
  SimpleRequestInfo *sri = xfer->user_data;
  sri->status_code = xfer->response->status_code;
}
static void simple__handle_content_complete (DskHttpClientStreamTransfer *xfer)
{
  SimpleRequestInfo *sri = xfer->user_data;
  DskError *error = NULL;
  switch (dsk_octet_source_read_buffer (DSK_OCTET_SOURCE (xfer->content),
                                        &sri->content_buffer,
                                        &error))
    {
    case DSK_IO_RESULT_SUCCESS:
    case DSK_IO_RESULT_AGAIN:
    case DSK_IO_RESULT_EOF:
      break;
    default:
      dsk_die ("dsk_octet_source_read_buffer failed: %s", error->message);
    }
}
static void simple__handle_error (DskHttpClientStreamTransfer *xfer)
{
  SimpleRequestInfo *sri = xfer->user_data;
  sri->failed = DSK_TRUE;
}
static void simple__handle_destroy (DskHttpClientStreamTransfer *xfer)
{
  SimpleRequestInfo *sri = xfer->user_data;
  sri->done = DSK_TRUE;
}

static DskHttpClientStreamFuncs simple_client_stream_funcs =
{
  simple__handle_response,
  simple__handle_content_complete,
  simple__handle_error,
  simple__handle_destroy
};

static DskHttpClientStreamTransfer *
do_request (DskHttpClientStream *http_client_stream,
            DskHttpClientStreamRequestOptions *request_options,
            int mode)
{
  DskError *error = NULL;
  DskHttpClientStreamTransfer *xfer;
  char *to_free = NULL;
  if (cmdline_verbose)
    dsk_warning ("do_request: path=%s, mode=%d",
                 request_options->request_options->path,
                 mode);
  switch (mode)
    {
    case 0:
      break;
    case 1:
    case 2:
      {
        const char *in = request_options->request_options->path;
        DskBuffer path = DSK_BUFFER_INIT;
        DskOctetFilter *urlenc;
        if (mode == 1)
          dsk_buffer_append_string (&path, "/internal-redirect?path=");
        else
          dsk_buffer_append_string (&path, "/sleep-internal-redirect?path=");
        urlenc = dsk_url_encoder_new ();
        dsk_octet_filter_process (urlenc, &path, strlen (in), (uint8_t*)in, NULL);
        dsk_octet_filter_finish (urlenc, &path, NULL);
        dsk_object_unref (urlenc);
        to_free = dsk_buffer_empty_to_string (&path);
        request_options->request_options->path = to_free;
        break;
      }
    }

  xfer = dsk_http_client_stream_request (http_client_stream,
                                         request_options, &error);
  if (xfer == NULL)
    dsk_die ("error creating client-stream request: %s", error->message);
  dsk_free (to_free);
  return xfer;
}

static void
test_simple_http_server (void)
{
  DskHttpServer *server = dsk_http_server_new ();
  DskError *error = NULL;

  dsk_http_server_match_save (server);
  dsk_http_server_add_match (server, DSK_HTTP_SERVER_MATCH_PATH, "/hello.txt");
  dsk_http_server_register_cgi_handler (server,
                                        respond_text_string,
                                        "hello\n",
                                        NULL);
  dsk_http_server_match_restore (server);

  dsk_http_server_match_save (server);
  dsk_http_server_add_match (server, DSK_HTTP_SERVER_MATCH_PATH, "/add\\?.*");
  dsk_http_server_register_cgi_handler (server,
                                        respond_sum,
                                        NULL,
                                        NULL);
  dsk_http_server_match_restore (server);


  dsk_http_server_match_save (server);
  dsk_http_server_add_match (server, DSK_HTTP_SERVER_MATCH_PATH, "/internal-redirect\\?.*");
  dsk_http_server_register_cgi_handler (server,
                                        internal_redirect,
                                        NULL,
                                        NULL);
  dsk_http_server_match_restore (server);

  dsk_http_server_match_save (server);
  dsk_http_server_add_match (server, DSK_HTTP_SERVER_MATCH_PATH, "/sleep-internal-redirect\\?.*");
  dsk_http_server_register_cgi_handler (server,
                                        internal_redirect_short_sleep,
                                        NULL,
                                        NULL);
  dsk_http_server_match_restore (server);

  if (!dsk_http_server_bind_local (server, "tests/sockets/http-server.socket",
                                   &error))
    dsk_die ("error binding: %s", error->message);

  /* use a client-stream to fetch hello.txt */
  {
  DskClientStreamOptions cs_options = DSK_CLIENT_STREAM_OPTIONS_INIT;
  DskHttpClientStreamOptions http_client_stream_options = DSK_HTTP_CLIENT_STREAM_OPTIONS_INIT;
  DskHttpClientStreamRequestOptions request_options = DSK_HTTP_CLIENT_STREAM_REQUEST_OPTIONS_INIT;
  DskHttpRequestOptions req_options = DSK_HTTP_REQUEST_OPTIONS_INIT;
  int mode;
  request_options.request_options = &req_options;
  cs_options.path = "tests/sockets/http-server.socket";

  for (mode = 0; mode < 3; mode++)
    {
      /* mode 0: simple path
         mode 1: internal redirect
         mode 2: internal redirect after sleep */

      /* Allocate a testing http_client_stream */
      DskHttpClientStream *http_client_stream;
      DskHttpClientStreamTransfer *xfer;
      char content_buf[6];
      {
        DskOctetSink *client_sink;
        DskOctetSource *client_source;
        if (!dsk_client_stream_new (&cs_options, NULL, &client_sink, &client_source,
                                    &error))
          dsk_die ("error creating client-stream");
        http_client_stream = dsk_http_client_stream_new (client_sink, client_source,
                                                         &http_client_stream_options);
        dsk_object_unref (client_sink);
        dsk_object_unref (client_source);
      }
      {
      SimpleRequestInfo sri = SIMPLE_REQUEST_INFO_INIT;
      req_options.path = "/hello.txt";
      request_options.funcs = &simple_client_stream_funcs;
      request_options.user_data = &sri;
      xfer = do_request (http_client_stream, &request_options, mode);
      while (!sri.done)
        dsk_main_run_once ();
      dsk_assert (sri.content_buffer.size == 6);
      dsk_buffer_read (&sri.content_buffer, 6, content_buf);
      dsk_assert (memcmp (content_buf, "hello\n", 6) == 0);
      dsk_assert (sri.status_code == 200);
      dsk_assert (!sri.failed);
      dsk_buffer_clear (&sri.content_buffer);
      }

      /* use a client-stream to fetch anything else to get a 404 */
      {
      SimpleRequestInfo sri = SIMPLE_REQUEST_INFO_INIT;
      req_options.path = "/does-not-exist";
      request_options.funcs = &simple_client_stream_funcs;
      request_options.user_data = &sri;
      xfer = do_request (http_client_stream, &request_options, mode);
      if (xfer == NULL)
        dsk_die ("error creating client-stream request: %s", error->message);
      while (!sri.done)
        dsk_main_run_once ();
      dsk_assert (sri.status_code == 404);
      dsk_assert (!sri.failed);
      dsk_buffer_clear (&sri.content_buffer);
      }

      /* use a client-stream to fetch /add else to get a 404 */
      {
      SimpleRequestInfo sri = SIMPLE_REQUEST_INFO_INIT;
      char *str;
      req_options.path = "/add?a=10&b=32";
      request_options.funcs = &simple_client_stream_funcs;
      request_options.user_data = &sri;
      xfer = do_request (http_client_stream, &request_options, mode);
      if (xfer == NULL)
        dsk_die ("error creating client-stream request: %s", error->message);
      while (!sri.done)
        dsk_main_run_once ();
      dsk_assert (sri.status_code == 200);
      dsk_assert (!sri.failed);
      str = dsk_buffer_empty_to_string (&sri.content_buffer);
      dsk_ascii_strchomp (str);
      dsk_assert (strcmp (str, "42") == 0);
      dsk_free (str);
      }
      dsk_object_unref (http_client_stream);
    }
  }

  dsk_object_unref (server);
}

static struct 
{
  const char *name;
  void (*test)(void);
} tests[] =
{
  { "simple http server", test_simple_http_server },
};

int main(int argc, char **argv)
{
  unsigned i;

  dsk_cmdline_init ("test HTTP server",
                    "Test the high-level HTTP server interface",
                    NULL, 0);
  dsk_cmdline_add_boolean ("verbose", "extra logging", NULL, 0,
                           &cmdline_verbose);
  dsk_cmdline_process_args (&argc, &argv);

  // HACK: we need the sockets directory to exist!
  mkdir ("tests/sockets", 0755);

  for (i = 0; i < DSK_N_ELEMENTS (tests); i++)
    {
      fprintf (stderr, "Test: %s... ", tests[i].name);
      tests[i].test ();
      fprintf (stderr, " done.\n");
    }
  dsk_cleanup ();
  return 0;
}
