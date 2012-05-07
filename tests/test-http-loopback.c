#include <stdio.h>
#include "../dsk.h"

typedef struct {
  DskHttpResponse *response;
  DskBuffer content;
  DskError *error;
  dsk_boolean content_complete;
  dsk_boolean destroyed;
} ClientRequestData;
#define CLIENT_REQUEST_DATA_DEFAULT \
{ \
  NULL, \
  DSK_BUFFER_INIT, \
  NULL, \
  DSK_FALSE, \
  DSK_FALSE \
}
static void client_handle_response (DskHttpClientStreamTransfer *xfer)
{
  ClientRequestData *crd = xfer->user_data;
  crd->response = dsk_object_ref (xfer->response);
  /* XXX: we need a function to do this to adjust hooks */
  xfer->content->buffer_low_amount = 10000000;           /* no max buffer size */
}
static void client_content_complete (DskHttpClientStreamTransfer *xfer)
{
  ClientRequestData *crd = xfer->user_data;
  dsk_octet_source_read_buffer (DSK_OCTET_SOURCE (xfer->content),
                                &crd->content, NULL);
  crd->content_complete = DSK_TRUE;
}
static void client_handle_error (DskHttpClientStreamTransfer *xfer)
{
  ClientRequestData *crd = xfer->user_data;
  if (crd->error == NULL)
    crd->error = dsk_object_ref (xfer->owner->latest_error);
}
static void client_destroy (DskHttpClientStreamTransfer *xfer)
{
  ClientRequestData *crd = xfer->user_data;
  crd->destroyed = DSK_TRUE;
}

static void
client_request_data_clear (ClientRequestData *crd)
{
  if (crd->response)
    dsk_object_unref (crd->response);
  dsk_buffer_reset (&crd->content);
  if (crd->error)
    dsk_error_unref (crd->error);
}

static DskHttpClientStreamFuncs client_request_funcs =
{
  client_handle_response,
  client_content_complete,
  client_handle_error,
  client_destroy,
};


static void
test_simple_connection_close (void)
{
  DskOctetSource *source1, *source2;
  DskOctetSink *sink1, *sink2;
  DskHttpClientStreamOptions client_options = DSK_HTTP_CLIENT_STREAM_OPTIONS_INIT;
  DskHttpServerStreamOptions server_options = DSK_HTTP_SERVER_STREAM_OPTIONS_INIT;
  ClientRequestData client_request_data = CLIENT_REQUEST_DATA_DEFAULT;
  DskHttpClientStreamTransfer *client_xfer;
  DskHttpServerStreamTransfer *server_xfer;
  DskHttpRequestOptions request_options = DSK_HTTP_REQUEST_OPTIONS_INIT;
  DskHttpClientStreamRequestOptions cr_options = DSK_HTTP_CLIENT_STREAM_REQUEST_OPTIONS_INIT;
  DskError *error = NULL;
  DskHttpClientStream *client;
  DskHttpServerStream *server;
  dsk_octet_pipe_new (0, &sink1, &source1);
  dsk_octet_pipe_new (0, &sink2, &source2);

  client = dsk_http_client_stream_new (sink1, source2, &client_options);
  server = dsk_http_server_stream_new (sink2, source1, &server_options);
  dsk_object_unref (sink1);
  dsk_object_unref (sink2);
  dsk_object_unref (source1);
  dsk_object_unref (source2);
  request_options.full_path = "/";
  request_options.host = "localhost";
  cr_options.request_options = &request_options;
  cr_options.funcs = &client_request_funcs;
  cr_options.user_data = &client_request_data;
  client_xfer = dsk_http_client_stream_request (client, &cr_options, &error);
  dsk_assert (client_xfer != NULL);
  /* ALTERNATE: wait for request_available hook */
  while ((server_xfer=dsk_http_server_stream_get_request (server)) == NULL)
    dsk_main_run_once ();

  /* server respond */
  {
  DskHttpServerStreamResponseOptions response_options = DSK_HTTP_SERVER_STREAM_RESPONSE_OPTIONS_INIT;
  DskHttpResponseOptions response_hdr_options = DSK_HTTP_RESPONSE_OPTIONS_INIT;
  response_options.header_options = &response_hdr_options;
  response_options.content_length = 7;
  response_options.content_body = (const void *) "hi mom\n";
  if (!dsk_http_server_stream_respond (server_xfer, &response_options, &error))
    dsk_die ("dsk_http_server_respond failed: %s", error->message);
  }

  /* client wait for response complete */
  while (!client_request_data.content_complete)
    dsk_main_run_once ();

  /* cleanup */
  dsk_object_unref (client);
  dsk_object_unref (server);
  client_request_data_clear (&client_request_data);
}

static struct 
{
  const char *name;
  void (*test)(void);
} tests[] =
{
  { "simple connection-close", test_simple_connection_close },
};
int main(int argc, char **argv)
{
  unsigned i;

  dsk_cmdline_init ("test HTTP client and server connected with pipes",
                    "Run a number of tests in which the HTTP client/server",
                    NULL, 0);

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
