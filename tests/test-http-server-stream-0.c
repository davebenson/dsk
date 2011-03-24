#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "../dsk.h"

static dsk_boolean cmdline_verbose = DSK_FALSE;

static dsk_boolean set_boolean_true (void *obj, void *pbool)
{
  DSK_UNUSED (obj);
  * (dsk_boolean *) pbool = DSK_TRUE;
  return DSK_FALSE;
}

static unsigned bbb_rem;
static const uint8_t *bbb_at;
static void
add_byte_to_memory_source (void *data)
{
  DskMemorySource *csource = DSK_MEMORY_SOURCE (data);
  dsk_buffer_append_byte (&csource->buffer, *bbb_at++);
  bbb_rem--;
  if (bbb_rem > 0)
    dsk_main_add_idle (add_byte_to_memory_source, csource);
  dsk_memory_source_added_data (csource);
}

static void
test_simple (void)
{
  static const char *headers[] =
    {
      "GET / HTTP/1.1\r\n"
      "Host: localhost\r\n"
      "Connection: close\r\n"
      "\r\n"
      ,
      "GET / HTTP/1.0\r\n"
      "Host: localhost\r\n"
      "\r\n"
    };
  static const char *header_blurbs[] = { "1.1/close", "1.0/default" };
  static const char *iter_blurbs[] = { "byte-by-byte", "one-chunk" };
  static const char *response_blurbs[] = { "data block", "buffer-stream", "byte-by-byte content" };
  unsigned header_i, iter, resp_iter;

  for (header_i = 0; header_i < DSK_N_ELEMENTS (headers); header_i++)
    for (iter = 0; iter < 2; iter++)
      for (resp_iter = 0; resp_iter < 3; resp_iter++)
        {
          const char *hdr = headers[header_i];
          unsigned hdr_len = strlen (hdr);
          DskError *error = NULL;
          DskMemorySource *csource = dsk_memory_source_new ();
          DskMemorySink *csink = dsk_memory_sink_new ();
          DskHttpServerStream *stream;
          DskHttpServerStreamOptions server_opts 
            = DSK_HTTP_SERVER_STREAM_OPTIONS_DEFAULT;
          DskHttpResponseOptions resp_opts
            = DSK_HTTP_RESPONSE_OPTIONS_DEFAULT;
          DskHttpServerStreamResponseOptions stream_resp_opts
            = DSK_HTTP_SERVER_STREAM_RESPONSE_OPTIONS_DEFAULT;
          dsk_boolean got_notify;
          DskHttpServerStreamTransfer *xfer;
          DskMemorySource *content_source = NULL;
          char *line;
          csink->max_buffer_size = 128*1024;
          if (cmdline_verbose)
            dsk_warning ("request style: %s; writing data mode: %s; response style: %s",
                         header_blurbs[header_i], iter_blurbs[iter], response_blurbs[resp_iter]);
          else
            fprintf (stderr, ".");
          stream = dsk_http_server_stream_new (DSK_OCTET_SINK (csink),
                                               DSK_OCTET_SOURCE (csource),
                                               &server_opts);
          if (iter == 0)
            {
              /* Feed data to server in one bite */
              dsk_buffer_append (&csource->buffer, hdr_len, hdr);
              dsk_memory_source_added_data (csource);
              //dsk_memory_source_done_adding (csource);
            }
          else
            {
              /* Feed data to server byte-by-byte */
              unsigned rem = hdr_len;
              const char *at = hdr;
              while (rem--)
                {
                  dsk_buffer_append_byte (&csource->buffer, *at++);
                  dsk_memory_source_added_data (csource);
                  while (csource->buffer.size > 0)
                    dsk_main_run_once ();
                }
              //dsk_memory_source_done_adding (csource);
            }
          /* wait til we receive the request */
          got_notify = DSK_FALSE;
          dsk_hook_trap (&stream->request_available,
                         set_boolean_true,
                         &got_notify,
                         NULL);
          while (!got_notify)
            dsk_main_run_once ();
          xfer = dsk_http_server_stream_get_request (stream);
          dsk_assert (xfer != NULL);
          dsk_assert (dsk_http_server_stream_get_request (stream) == NULL);

          switch (header_i)
            {
            case 0:
              dsk_assert (xfer->request->connection_close);
              dsk_assert (xfer->request->http_major_version == 1);
              dsk_assert (xfer->request->http_minor_version == 1);
              dsk_assert (!xfer->request->transfer_encoding_chunked);
              dsk_assert (xfer->request->content_length == -1LL);
              break;
            case 1:
              dsk_assert (xfer->request->http_major_version == 1);
              dsk_assert (xfer->request->http_minor_version == 0);
              dsk_assert (!xfer->request->transfer_encoding_chunked);
              dsk_assert (xfer->request->content_length == -1LL);
              break;
            }


          /* send a response */
          switch (resp_iter)
            {
            case 0:
              stream_resp_opts.header_options = &resp_opts;
              stream_resp_opts.content_length = 7;
              stream_resp_opts.content_body = (const uint8_t *) "hi mom\n";
              break;
            case 1:
              /* streaming data, no content-length;
                 relies on Connection-Close */
              stream_resp_opts.header_options = &resp_opts;

              /* setting content_source will cause us to write the 
                 content in byte-by-byte below */
              content_source = dsk_memory_source_new ();
              stream_resp_opts.content_stream = DSK_OCTET_SOURCE (content_source);
              break;
            case 2:
              /* streaming data, no content-length;
                 relies on Connection-Close */
              stream_resp_opts.header_options = &resp_opts;
              stream_resp_opts.content_length = 7;

              /* setting content_source will cause us to write the 
                 content in byte-by-byte below */
              content_source = dsk_memory_source_new ();
              stream_resp_opts.content_stream = DSK_OCTET_SOURCE (content_source);
              break;
            }
          if (!dsk_http_server_stream_respond (xfer, &stream_resp_opts, &error))
            dsk_die ("error responding to request: %s", error->message);

          /* We retain content_source only so we can
             add the content byte-by-byte */
          if (content_source != NULL)
            {
              bbb_at = (const uint8_t *) "hi mom\n";
              bbb_rem = 7;
              dsk_main_add_idle (add_byte_to_memory_source, content_source);
              while (bbb_rem > 0)
                dsk_main_run_once ();
              dsk_memory_source_done_adding (content_source);
              dsk_object_unref (content_source);
              content_source = NULL;
            }

          /* read until EOF from sink */
          while (!csink->got_shutdown)
            dsk_main_run_once ();

          /* analyse response (header+body) */
          line = dsk_buffer_read_line (&csink->buffer);
          dsk_assert (line != NULL);
          dsk_assert (strncmp (line, "HTTP/1.", 7) == 0);
          dsk_assert (line[7] == '0' || line[7] == '1');
          dsk_assert (line[8] == ' ');
          dsk_assert (strncmp (line+9, "200", 3) == 0);
          dsk_assert (line[12] == 0 || dsk_ascii_isspace (line[12]));
          dsk_free (line);
          while ((line=dsk_buffer_read_line (&csink->buffer)) != NULL)
            {
              if (line[0] == '\r' || line[0] == 0)
                {
                  dsk_free (line);
                  break;
                }
              /* TODO: process header line? */
              dsk_free (line);
            }
          dsk_assert (line != NULL);
          line = dsk_buffer_read_line (&csink->buffer);
          dsk_assert (strcmp (line, "hi mom") == 0);
          dsk_free (line);
          dsk_assert (csink->buffer.size == 0);

          /* cleanup */
          dsk_object_unref (csource);
          dsk_object_unref (csink);
          dsk_object_unref (stream);
        }
}

static char *
read_line_blocking (DskBuffer *buffer)
{
  char *rv;
  while ((rv=dsk_buffer_read_line (buffer)) == NULL)
    dsk_main_run_once ();
  return rv;
}

static void
test_response_keepalive (void)
{
  static const char *headers[] =
    {
      "GET / HTTP/1.1\r\n"
      "Host: localhost\r\n"
      "\r\n"
    };
  static const char *header_blurbs[] = { "1.1/close", "1.0/default" };
  static const char *iter_blurbs[] = { "byte-by-byte", "one-chunk" };
  static const char *response_blurbs[] = { "data block", "buffer-stream", "byte-by-byte content" };
  unsigned header_i, iter, resp_iter, rep;

  for (header_i = 0; header_i < DSK_N_ELEMENTS (headers); header_i++)
    for (iter = 0; iter < 2; iter++)
      for (resp_iter = 0; resp_iter < 3; resp_iter++)
        {
          const char *hdr = headers[header_i];
          unsigned hdr_len = strlen (hdr);
          DskError *error = NULL;
          DskMemorySource *csource = dsk_memory_source_new ();
          DskMemorySink *csink = dsk_memory_sink_new ();
          DskHttpServerStream *stream;
          DskHttpServerStreamOptions server_opts 
            = DSK_HTTP_SERVER_STREAM_OPTIONS_DEFAULT;
          DskHttpResponseOptions resp_opts
            = DSK_HTTP_RESPONSE_OPTIONS_DEFAULT;
          DskHttpServerStreamResponseOptions stream_resp_opts
            = DSK_HTTP_SERVER_STREAM_RESPONSE_OPTIONS_DEFAULT;
          csink->max_buffer_size = 128*1024;
          if (cmdline_verbose)
            dsk_warning ("request style: %s; writing data mode: %s; response style: %s",
                         header_blurbs[header_i], iter_blurbs[iter], response_blurbs[resp_iter]);
          else
            fprintf (stderr, ".");
          stream = dsk_http_server_stream_new (DSK_OCTET_SINK (csink),
                                               DSK_OCTET_SOURCE (csource),
                                               &server_opts);
          for (rep = 0; rep < 20; rep++)
            {
              dsk_boolean got_notify;
              DskHttpServerStreamTransfer *xfer;
              DskMemorySource *content_source = NULL;
              char *line;
              dsk_boolean in_header;
              dsk_boolean chunked;
              int content_length;

              if (cmdline_verbose)
                fprintf (stderr, ".");
              if (iter == 0)
                {
                  /* Feed data to server in one bite */
                  dsk_buffer_append (&csource->buffer, hdr_len, hdr);
                  dsk_memory_source_added_data (csource);
                  //dsk_memory_source_done_adding (csource);
                }
              else
                {
                  /* Feed data to server byte-by-byte */
                  unsigned rem = hdr_len;
                  const char *at = hdr;
                  while (rem--)
                    {
                      dsk_buffer_append_byte (&csource->buffer, *at++);
                      dsk_memory_source_added_data (csource);
                      while (csource->buffer.size > 0)
                        dsk_main_run_once ();
                    }
                  //dsk_memory_source_done_adding (csource);
                }
              /* wait til we receive the request */
              got_notify = DSK_FALSE;
              dsk_hook_trap (&stream->request_available,
                             set_boolean_true,
                             &got_notify,
                             NULL);
              while (!got_notify)
                dsk_main_run_once ();
              xfer = dsk_http_server_stream_get_request (stream);
              dsk_assert (xfer != NULL);
              dsk_assert (dsk_http_server_stream_get_request (stream) == NULL);

              switch (header_i)
                {
                case 0:
                  dsk_assert (!xfer->request->connection_close);
                  dsk_assert (xfer->request->http_major_version == 1);
                  dsk_assert (xfer->request->http_minor_version == 1);
                  dsk_assert (!xfer->request->transfer_encoding_chunked);
                  dsk_assert (xfer->request->content_length == -1LL);
                  break;
                }

              /* send a response */
              switch (resp_iter)
                {
                case 0:
                  stream_resp_opts.header_options = &resp_opts;
                  stream_resp_opts.content_length = 7;
                  stream_resp_opts.content_body = (const uint8_t *) "hi mom\n";
                  break;
                case 1:
                  /* streaming data, no content-length;
                     relies on Transfer-Encoding: chunked */
                  stream_resp_opts.header_options = &resp_opts;

                  /* setting content_source will cause us to write the 
                     content in byte-by-byte below */
                  content_source = dsk_memory_source_new ();
                  stream_resp_opts.content_stream = DSK_OCTET_SOURCE (content_source);
                  break;
                case 2:
                  /* streaming data, content-length;
                     can use Content-Length header with keepalive */
                  stream_resp_opts.header_options = &resp_opts;
                  stream_resp_opts.content_length = 7;

                  /* setting content_source will cause us to write the 
                     content in byte-by-byte below */
                  content_source = dsk_memory_source_new ();
                  stream_resp_opts.content_stream = DSK_OCTET_SOURCE (content_source);
                  break;
                }
              if (!dsk_http_server_stream_respond (xfer, &stream_resp_opts, &error))
                dsk_die ("error responding to request: %s", error->message);

              /* We retain content_source only so we can
                 add the content byte-by-byte */
              if (content_source != NULL)
                {
                  bbb_at = (const uint8_t *) "hi mom\n";
                  bbb_rem = 7;
                  dsk_main_add_idle (add_byte_to_memory_source, content_source);
                  while (bbb_rem > 0)
                    dsk_main_run_once ();
                  dsk_memory_source_done_adding (content_source);
                  dsk_object_unref (content_source);
                  content_source = NULL;
                }

              /* analyse response (header+body) */
              line = read_line_blocking (&csink->buffer);
              dsk_assert (line != NULL);
              dsk_assert (strncmp (line, "HTTP/1.", 7) == 0);
              dsk_assert (line[7] == '0' || line[7] == '1');
              dsk_assert (line[8] == ' ');
              dsk_assert (strncmp (line+9, "200", 3) == 0);
              dsk_assert (line[12] == 0 || dsk_ascii_isspace (line[12]));
              dsk_free (line);
              in_header = DSK_TRUE;
              chunked = DSK_FALSE;
              content_length = -1;
              while (in_header)
                {
                  char *cr;
                  line = read_line_blocking (&csink->buffer);
                  cr = strchr (line, '\r');
                  if (cr)
                    *cr = 0;
                  if (line[0] == 0)
                    {
                      in_header = DSK_FALSE;
                    }
                  else if (strcmp (line, "Transfer-Encoding: chunked") == 0)
                    {
                      chunked = DSK_TRUE;
                    }
                  else if (strncmp (line, "Content-Length:", 15) == 0)
                    {
                      char *at = line + 15;
                      while (dsk_ascii_isspace (*at))
                        at++;
                      dsk_assert (dsk_ascii_isdigit (*at));
                      content_length = strtoul (at, NULL, 10);
                    }
                  dsk_free (line);
                }
              dsk_assert (chunked || content_length != -1);
              dsk_assert (!chunked || content_length == -1);
              if (chunked)
                {
                  DskBuffer content = DSK_BUFFER_STATIC_INIT;
                  for (;;)
                    {
                      unsigned chunk_len;
                      line = read_line_blocking (&csink->buffer);
                      dsk_assert (dsk_ascii_isxdigit (*line));
                      chunk_len = strtoul (line, NULL, 16);
                      dsk_free (line);
                      while (csink->buffer.size < chunk_len)
                        dsk_main_run_once ();
                      dsk_buffer_transfer (&content, &csink->buffer, chunk_len);
                      line = read_line_blocking (&csink->buffer);
                      /* should be blank */
                      dsk_free (line);
                      if (chunk_len == 0)
                        {
                          break;
                        }
                    }
                  line = dsk_buffer_read_line (&content);
                  dsk_assert (strcmp (line, "hi mom") == 0);
                  dsk_free (line);
                }
              else
                {
                  line = read_line_blocking (&csink->buffer);
                  dsk_assert (strcmp (line, "hi mom") == 0);
                  dsk_free (line);
                  dsk_assert (csink->buffer.size == 0);
                }
            }

          /* cleanup */
          dsk_object_unref (csource);
          dsk_object_unref (csink);
          dsk_object_unref (stream);
        }
}
static void
test_pipelining (void)
{
  static const char *headers[] =
    {
      "GET / HTTP/1.1\r\n"
      "Host: localhost\r\n"
      "\r\n"
      "GET / HTTP/1.1\r\n"
      "Host: localhost\r\n"
      "\r\n"
    };
  static const char *header_blurbs[] = { "1.1" };
  static const char *iter_blurbs[] = { "byte-by-byte", "one-chunk" };
  static const char *response_blurbs[] = { "data block", "buffer-stream", "byte-by-byte content" };
  unsigned header_i, iter, resp_iter;

  for (header_i = 0; header_i < DSK_N_ELEMENTS (headers); header_i++)
    for (iter = 0; iter < 2; iter++)
      for (resp_iter = 0; resp_iter < 3; resp_iter++)
        {
          const char *hdr = headers[header_i];
          unsigned hdr_len = strlen (hdr);
          DskError *error = NULL;
          DskMemorySource *csource = dsk_memory_source_new ();
          DskMemorySink *csink = dsk_memory_sink_new ();
          DskHttpServerStream *stream;
          DskHttpServerStreamOptions server_opts 
            = DSK_HTTP_SERVER_STREAM_OPTIONS_DEFAULT;
          DskHttpResponseOptions resp_opts
            = DSK_HTTP_RESPONSE_OPTIONS_DEFAULT;
          DskHttpServerStreamResponseOptions stream_resp_opts
            = DSK_HTTP_SERVER_STREAM_RESPONSE_OPTIONS_DEFAULT;
          dsk_boolean got_notify;
          DskHttpServerStreamTransfer *xfer;
          unsigned reqno;
          csink->max_buffer_size = 128*1024;
          if (cmdline_verbose)
            dsk_warning ("request style: %s; writing data mode: %s; response style: %s",
                         header_blurbs[header_i], iter_blurbs[iter], response_blurbs[resp_iter]);
          else
            fprintf (stderr, ".");
          stream = dsk_http_server_stream_new (DSK_OCTET_SINK (csink),
                                               DSK_OCTET_SOURCE (csource),
                                               &server_opts);
          if (iter == 0)
            {
              /* Feed data to server in one bite */
              dsk_buffer_append (&csource->buffer, hdr_len, hdr);
              dsk_memory_source_added_data (csource);
              //dsk_memory_source_done_adding (csource);
            }
          else
            {
              /* Feed data to server byte-by-byte */
              unsigned rem = hdr_len;
              const char *at = hdr;
              while (rem--)
                {
                  dsk_buffer_append_byte (&csource->buffer, *at++);
                  dsk_memory_source_added_data (csource);
                  while (csource->buffer.size > 0)
                    dsk_main_run_once ();
                }
              //dsk_memory_source_done_adding (csource);
            }
          /* wait til we receive the request */
          got_notify = DSK_FALSE;
          dsk_hook_trap (&stream->request_available,
                         set_boolean_true,
                         &got_notify,
                         NULL);
          while (!got_notify)
            dsk_main_run_once ();
          for (reqno = 0; reqno < 2; reqno++)
            {
              DskMemorySource *content_source = NULL;
              xfer = dsk_http_server_stream_get_request (stream);
              dsk_assert (xfer != NULL);

              switch (header_i)
                {
                case 0:
                  dsk_assert (xfer->request->http_major_version == 1);
                  dsk_assert (xfer->request->http_minor_version == 1);
                  dsk_assert (!xfer->request->transfer_encoding_chunked);
                  dsk_assert (xfer->request->content_length == -1LL);
                  break;
                }


              /* send a response */
              switch (resp_iter)
                {
                case 0:
                  stream_resp_opts.header_options = &resp_opts;
                  stream_resp_opts.content_length = 7;
                  stream_resp_opts.content_body = (const uint8_t *) "hi mom\n";
                  break;
                case 1:
                case 2:
                  /* streaming data, transfer-encoding chunked */
                  stream_resp_opts.header_options = &resp_opts;

                  /* setting content_source will cause us to write the 
                     content in byte-by-byte below */
                  content_source = dsk_memory_source_new ();
                  stream_resp_opts.content_stream = DSK_OCTET_SOURCE (content_source);
                  break;
                }
              if (reqno == 1)
                resp_opts.connection_close = DSK_TRUE;
              if (!dsk_http_server_stream_respond (xfer, &stream_resp_opts, &error))
                dsk_die ("error responding to request: %s", error->message);

              /* We retain content_source only so we can
                 add the content byte-by-byte */
              switch (resp_iter)
                {
                case 1:
                  bbb_at = (const uint8_t *) "hi mom\n";
                  bbb_rem = 7;
                  dsk_main_add_idle (add_byte_to_memory_source, content_source);
                  while (bbb_rem > 0)
                    dsk_main_run_once ();
                  dsk_memory_source_done_adding (content_source);
                  dsk_object_unref (content_source);
                  content_source = NULL;
                  break;
                case 2:
                  bbb_at = (const uint8_t *) "hi mom\n";
                  bbb_rem = 7;
                  dsk_buffer_append_string (&content_source->buffer, "hi mom\n");
                  dsk_memory_source_added_data (content_source);
                  dsk_memory_source_done_adding (content_source);
                  dsk_object_unref (content_source);
                  content_source = NULL;
                  break;
                }
              dsk_assert (content_source == NULL);
            }
          dsk_assert (dsk_http_server_stream_get_request (stream) == NULL);

          /* read until EOF from sink */
          while (!csink->got_shutdown)
            dsk_main_run_once ();

          /* analyse response (header+body) */
          for (reqno = 0; reqno < 2; reqno++)
            {
              char *line;
              dsk_boolean chunked = DSK_FALSE;
              line = dsk_buffer_read_line (&csink->buffer);
              dsk_assert (line != NULL);
              dsk_assert (strncmp (line, "HTTP/1.", 7) == 0);
              dsk_assert (line[7] == '0' || line[7] == '1');
              dsk_assert (line[8] == ' ');
              dsk_assert (strncmp (line+9, "200", 3) == 0);
              dsk_assert (line[12] == 0 || dsk_ascii_isspace (line[12]));
              dsk_free (line);
              while ((line=dsk_buffer_read_line (&csink->buffer)) != NULL)
                {
                  if (line[0] == '\r' || line[0] == 0)
                    {
                      dsk_free (line);
                      break;
                    }
                  if (strncasecmp (line, "transfer-encoding: chunked", 26) == 0)
                    {
                      chunked = DSK_TRUE;
                    }

                  /* TODO: process header line? */
                  dsk_free (line);
                }
              dsk_assert (line != NULL);
              if (chunked)
                {
                  DskBuffer tmp = DSK_BUFFER_STATIC_INIT;
                  char buf[7];
                  for (;;)
                    {
                      unsigned len;
                      line = dsk_buffer_read_line (&csink->buffer);
                      len = strtoul (line, NULL, 16);
                      dsk_free (line);
                      if (len == 0)
                        {
                          /* trailer */
                          line = dsk_buffer_read_line (&csink->buffer);
                          dsk_assert (line != NULL);
                          dsk_free (line);
                          break;
                        }
                      else
                        {
                          dsk_buffer_transfer (&tmp, &csink->buffer, len);
                          line = dsk_buffer_read_line (&csink->buffer);
                          dsk_assert (line != NULL);
                          dsk_free (line);
                        }
                    }
                  dsk_assert (tmp.size == 7);
                  dsk_buffer_read (&tmp, 7, buf);
                  dsk_assert (memcmp (buf, "hi mom\n", 7) == 0);
                }
              else
                {
                  line = dsk_buffer_read_line (&csink->buffer);
                  dsk_assert (strcmp (line, "hi mom") == 0);
                  dsk_free (line);
                }
            }
          dsk_assert (csink->buffer.size == 0);

          /* cleanup */
          dsk_object_unref (csource);
          dsk_object_unref (csink);
          dsk_object_unref (stream);
        }
}


static void
test_simple_post (void)
{
  static const char *headers[] =
    {
      "POST / HTTP/1.1\r\n"
      "Host: localhost\r\n"
      "Connection: close\r\n"
      "Content-Length: 7\r\n"
      "\r\n"
      "hi mom\n"
      ,
      "POST / HTTP/1.0\r\n"
      "Host: localhost\r\n"
      "Content-Length: 7\r\n"
      "\r\n"
      "hi mom\n"
      ,
      "POST / HTTP/1.0\r\n"
      "Host: localhost\r\n"
      "Transfer-Encoding: chunked\r\n"
      "\r\n"
      "7\r\nhi mom\n\r\n0\r\n\r\n"
      ,
      "POST / HTTP/1.0\r\n"
      "Host: localhost\r\n"
      "Transfer-Encoding: chunked\r\n"
      "\r\n"
      "1\r\nh\r\n"
      "1\r\ni\r\n"
      "1\r\n \r\n"
      "1\r\nm\r\n"
      "1\r\no\r\n"
      "1\r\nm\r\n"
      "1\r\n\n\r\n"
      "0\r\n\r\n"
    };
  static const char *header_blurbs[] = { "1.1/close", "1.0/default" };
  static const char *iter_blurbs[] = { "byte-by-byte", "one-chunk" };
  unsigned header_i, iter;

  for (header_i = 0; header_i < DSK_N_ELEMENTS (headers); header_i++)
    for (iter = 0; iter < 2; iter++)
      {
        const char *hdr = headers[header_i];
        unsigned hdr_len = strlen (hdr);
        DskError *error = NULL;
        DskMemorySource *csource = dsk_memory_source_new ();
        DskMemorySink *csink = dsk_memory_sink_new ();
        DskHttpServerStream *stream;
        DskHttpServerStreamOptions server_opts 
          = DSK_HTTP_SERVER_STREAM_OPTIONS_DEFAULT;
        DskHttpResponseOptions resp_opts
          = DSK_HTTP_RESPONSE_OPTIONS_DEFAULT;
        DskHttpServerStreamResponseOptions stream_resp_opts
          = DSK_HTTP_SERVER_STREAM_RESPONSE_OPTIONS_DEFAULT;
        dsk_boolean got_notify;
        DskHttpServerStreamTransfer *xfer;
        DskOctetSource *post_data;
        dsk_boolean done;
        DskBuffer buffer = DSK_BUFFER_STATIC_INIT;
        char *line;
        csink->max_buffer_size = 128*1024;
        if (cmdline_verbose)
          dsk_warning ("request style: %s; writing data mode: %s",
                       header_blurbs[header_i], iter_blurbs[iter]);
        else
          fprintf (stderr, ".");
        stream = dsk_http_server_stream_new (DSK_OCTET_SINK (csink),
                                             DSK_OCTET_SOURCE (csource),
                                             &server_opts);
        if (iter == 0)
          {
            /* Feed data to server in one bite */
            dsk_buffer_append (&csource->buffer, hdr_len, hdr);
            dsk_memory_source_added_data (csource);
            //dsk_memory_source_done_adding (csource);
          }
        else
          {
            /* Feed data to server byte-by-byte */
            unsigned rem = hdr_len;
            const char *at = hdr;
            while (rem--)
              {
                dsk_buffer_append_byte (&csource->buffer, *at++);
                dsk_memory_source_added_data (csource);
                while (csource->buffer.size > 0)
                  dsk_main_run_once ();
              }
            //dsk_memory_source_done_adding (csource);
          }
        /* wait til we receive the request */
        got_notify = DSK_FALSE;
        dsk_hook_trap (&stream->request_available,
                       set_boolean_true,
                       &got_notify,
                       NULL);
        while (!got_notify)
          dsk_main_run_once ();
        xfer = dsk_http_server_stream_get_request (stream);
        dsk_assert (xfer != NULL);
        dsk_assert (dsk_http_server_stream_get_request (stream) == NULL);

        switch (header_i)
          {
          case 0:
            dsk_assert (xfer->request->verb == DSK_HTTP_VERB_POST);
            dsk_assert (xfer->request->connection_close);
            dsk_assert (xfer->request->http_major_version == 1);
            dsk_assert (xfer->request->http_minor_version == 1);
            dsk_assert (!xfer->request->transfer_encoding_chunked);
            dsk_assert (xfer->request->content_length == 7);
            break;
          case 1:
            dsk_assert (xfer->request->verb == DSK_HTTP_VERB_POST);
            dsk_assert (xfer->request->http_major_version == 1);
            dsk_assert (xfer->request->http_minor_version == 0);
            dsk_assert (!xfer->request->transfer_encoding_chunked);
            dsk_assert (xfer->request->content_length == 7);
            break;
          }

        post_data = dsk_object_ref (xfer->post_data);
        done = DSK_FALSE;
        while (!done)
          {
            switch (dsk_octet_source_read_buffer (post_data, &buffer, &error))
              {
              case DSK_IO_RESULT_SUCCESS:
                break;
              case DSK_IO_RESULT_AGAIN:
                dsk_main_run_once ();
                break;
              case DSK_IO_RESULT_ERROR:
                dsk_die ("error reading post-data: %s", error->message);
              case DSK_IO_RESULT_EOF:
                done = DSK_TRUE;
                break;
              }
          }
        dsk_assert (buffer.size == 7);
        {
          char buf[7];
          dsk_buffer_read (&buffer, 7, buf);
          dsk_assert (memcmp (buf, "hi mom\n", 7) == 0);
        }

        /* send a response */
        stream_resp_opts.header_options = &resp_opts;
        stream_resp_opts.content_length = 7;
        stream_resp_opts.content_body = (const uint8_t *) "yo mom\n";
        if (!dsk_http_server_stream_respond (xfer, &stream_resp_opts, &error))
          dsk_die ("error responding to request: %s", error->message);

        /* read until EOF from sink */
        while (!csink->got_shutdown)
          dsk_main_run_once ();

        /* analyse response (header+body) */
        line = dsk_buffer_read_line (&csink->buffer);
        dsk_assert (line != NULL);
        dsk_assert (strncmp (line, "HTTP/1.", 7) == 0);
        dsk_assert (line[7] == '0' || line[7] == '1');
        dsk_assert (line[8] == ' ');
        dsk_assert (strncmp (line+9, "200", 3) == 0);
        dsk_assert (line[12] == 0 || dsk_ascii_isspace (line[12]));
        dsk_free (line);
        while ((line=dsk_buffer_read_line (&csink->buffer)) != NULL)
          {
            if (line[0] == '\r' || line[0] == 0)
              {
                dsk_free (line);
                break;
              }
            /* TODO: process header line? */
            dsk_free (line);
          }
        dsk_assert (line != NULL);
        line = dsk_buffer_read_line (&csink->buffer);
        dsk_assert (strcmp (line, "yo mom") == 0);
        dsk_free (line);
        dsk_assert (csink->buffer.size == 0);

        /* cleanup */
        dsk_object_unref (csource);
        dsk_object_unref (csink);
        dsk_object_unref (stream);
      }
}

static struct 
{
  const char *name;
  void (*test)(void);
} tests[] =
{
  { "simple connection-close", test_simple },
  { "simple response keepalive", test_response_keepalive },
  { "pipelining test", test_pipelining },
  { "simple POST", test_simple_post },
  //{ "transfer-encoding chunked content", test_transfer_encoding_chunked },
  //{ "transfer-encoding chunked POST", test_transfer_encoding_chunked_post },
  //{ "content-encoding gzip", test_content_encoding_gzip },
};

int main(int argc, char **argv)
{
  unsigned i;

  dsk_cmdline_init ("test HTTP server stream",
                    "Test the low-level HTTP server interface",
                    NULL, 0);
  dsk_cmdline_add_boolean ("verbose", "extra logging", NULL, 0,
                           &cmdline_verbose);
  dsk_cmdline_process_args (&argc, &argv);

  for (i = 0; i < DSK_N_ELEMENTS (tests); i++)
    {
      fprintf (stderr, "Test: %s... ", tests[i].name);
      if (cmdline_verbose)
        fprintf (stderr, "\n");
      tests[i].test ();
      fprintf (stderr, " done.\n");
    }
  dsk_cleanup ();
  return 0;
}
