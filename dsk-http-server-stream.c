#include <stdio.h>              /* for snprintf() */
#include <string.h>
#include "dsk.h"
#include "dsk-http-internals.h"
#include "dsk-list-macros.h"

#define GET_XFER_QUEUE(server_stream) \
  DskHttpServerStreamTransfer *,      \
  (server_stream)->first_transfer,    \
  (server_stream)->last_transfer,     \
  next

/* warn if a content-body is provided for HEAD;
   or if not provided for any other verb. */
#define DEBUG_ODD_VERB_USE              1

/* print message on error */
#define DEBUG_PRINT_ERRORS              1

static void
handle_post_data_finalize (void *data)
{
  DskHttpServerStreamTransfer *xfer = data;
  dsk_assert (xfer->post_data != NULL);
  xfer->post_data = NULL;
}
static void start_transfer (DskHttpServerStreamTransfer *transfer);
static void
free_transfer (DskHttpServerStreamTransfer *xfer)
{
  if (xfer->request)
    dsk_object_unref (xfer->request);
  if (xfer->response)
    dsk_object_unref (xfer->response);
  if (xfer->content_readable_trap)
    {
      //if (xfer->blocked_content)          /* not necessary */
        //dsk_hook_trap_unblock (xfer->content_readable_trap);
      dsk_hook_trap_destroy (xfer->content_readable_trap);
      xfer->content_readable_trap = NULL;
    }
  if (xfer->content)
    {
      dsk_octet_source_shutdown (xfer->content);
      dsk_object_unref (xfer->content);
    }
  if (xfer->post_data)
    {
      dsk_object_untrap_finalize (DSK_OBJECT (xfer->post_data),
                                  handle_post_data_finalize,
                                  xfer);
      xfer->post_data = NULL;
    }
  dsk_free (xfer);
}

static void
do_shutdown (DskHttpServerStream *ss,
             DskError            *e)
{
  DskError *error = e;
  while (ss->first_transfer != NULL)
    {
      DskHttpServerStreamTransfer *xfer = ss->first_transfer;
      ss->first_transfer = xfer->next;
      if (xfer->funcs != NULL)
        {
          if (xfer->funcs->error_notify != NULL)
            {
              if (error == NULL)
                error = dsk_error_new ("HTTP server shut down before transfer complete");
              xfer->funcs->error_notify (xfer, error);
            }
          if (xfer->funcs->destroy != NULL)
            xfer->funcs->destroy (xfer);
        }
      if (xfer->returned && !xfer->responded)
        {
          xfer->failed = DSK_TRUE;
          continue;                     /* do not continue destroying the xfer */
        }
      free_transfer (xfer);
    }
  if (e == NULL && error != NULL)
    dsk_error_unref (error);
  dsk_hook_set_idle_notify (&ss->request_available, DSK_FALSE);
  ss->last_transfer = NULL;
  ss->read_transfer = NULL;
  ss->next_request = NULL;
  if (ss->read_trap)
    {
      dsk_hook_trap_destroy (ss->read_trap);
      ss->read_trap = NULL;
    }
  if (ss->write_trap)
    {
      dsk_hook_trap_destroy (ss->write_trap);
      ss->write_trap = NULL;
    }
  if (ss->source)
    {
      dsk_octet_source_shutdown (ss->source);
      dsk_object_unref (ss->source);
      ss->source = NULL;
    }
  if (ss->sink)
    {
      dsk_octet_sink_shutdown (ss->sink);
      dsk_object_unref (ss->sink);
      ss->sink = NULL;
    }
}

static void
do_deferred_shutdown (DskHttpServerStream *ss)
{
  ss->deferred_shutdown = DSK_TRUE;
  if (ss->outgoing_data.size == 0)
    do_shutdown (ss, NULL);
}

static void
server_set_error (DskHttpServerStream *server,
                  const char          *format,
                  ...)
{
  DskError *e;
  va_list args;
  va_start (args, format);
  e = dsk_error_new_valist (format, args);
#if DEBUG_PRINT_ERRORS
  dsk_warning ("server_set_error: %s", e->message);
#endif
  va_end (args);
  do_shutdown (server, e);
  dsk_error_unref (e);
}

static void
done_reading_post_data (DskHttpServerStream *ss)
{
  DskHttpServerStreamTransfer *xfer = ss->read_transfer;
  dsk_assert (xfer != NULL);
  dsk_assert (xfer->next == NULL);
  if (xfer->request->connection_close)
    ss->no_more_transfers = DSK_TRUE;

  if (xfer->post_data != NULL)
    dsk_memory_source_done_adding (xfer->post_data);
  xfer->read_state = DSK_HTTP_SERVER_STREAM_READ_DONE;
  if (xfer->funcs != NULL && xfer->funcs->post_data_complete != NULL)
    xfer->funcs->post_data_complete (xfer);
  if (ss->wait_for_content_complete 
   && ss->next_request == NULL)
    {
      ss->next_request = xfer;

      /* notify that we have a header */
      dsk_hook_set_idle_notify (&ss->request_available, DSK_TRUE);
    }
  ss->read_transfer = NULL;
}

static dsk_boolean
handle_post_data_buffer_low (DskMemorySource *source,
                             DskHttpServerStreamTransfer *xfer)
{
  DSK_UNUSED (source);
  dsk_hook_trap_unblock (xfer->owner->read_trap);
  xfer->buffer_low_trap = NULL;
  return DSK_FALSE;
}

static dsk_boolean
transfer_post_content (DskHttpServerStreamTransfer *xfer,
                       unsigned                     amount)
{
  if (xfer->post_data != NULL)
    {
      size_t size;
      dsk_buffer_transfer (&xfer->post_data->buffer,
                           &xfer->owner->incoming_data,
                           amount);
      dsk_memory_source_added_data (xfer->post_data);

      size = xfer->post_data->buffer.size;
      if (size > xfer->owner->max_post_data_size
       || (size > xfer->post_data->buffer_low_amount
           && xfer->owner->wait_for_content_complete))
        {
          /* error POST data too large */
          server_set_error (xfer->owner, "POST data too large (at least %u bytes)",
                            (unsigned) size);
          return DSK_FALSE;
        }
      else if (size > xfer->post_data->buffer_low_amount)
        {
          /* block readable and stop processing */
          dsk_hook_trap_block (xfer->owner->read_trap);
          xfer->buffer_low_trap = dsk_hook_trap (&xfer->post_data->buffer_low,
                                                 (DskHookFunc) handle_post_data_buffer_low,
                                                 xfer, NULL);
        }
    }
  else
    dsk_buffer_discard (&xfer->owner->incoming_data, amount);
  return DSK_TRUE;
}

static dsk_boolean
handle_source_readable (DskOctetSource *source,
                      DskHttpServerStream *ss)
{
  DskError *error = NULL;
  DskHttpServerStreamTransfer *xfer;
  switch (dsk_octet_source_read_buffer (source, &ss->incoming_data, &error))
    {
    case DSK_IO_RESULT_SUCCESS:
      break;
    case DSK_IO_RESULT_AGAIN:
      return DSK_TRUE;
    case DSK_IO_RESULT_EOF:
      do_shutdown (ss, NULL);
      goto return_false;
    case DSK_IO_RESULT_ERROR:
      do_shutdown (ss, error);
      dsk_error_unref (error);
      goto return_false;
    }
restart_processing:
  if (ss->incoming_data.size == 0)
    return DSK_TRUE;
  if (ss->read_transfer == NULL)
    {
      if (ss->no_more_transfers)
        {
          dsk_buffer_reset (&ss->incoming_data);
          ss->read_trap = NULL;
          return DSK_FALSE;
        }

      /* new transfer */
      xfer = DSK_NEW0 (DskHttpServerStreamTransfer);
      xfer->owner = ss;
      xfer->read_state = DSK_HTTP_SERVER_STREAM_READ_NEED_HEADER;
      DSK_QUEUE_ENQUEUE (GET_XFER_QUEUE (ss), xfer);
      ss->read_transfer = xfer;
    }
  else
    xfer = ss->read_transfer;
  switch (xfer->read_state)
    {
    case DSK_HTTP_SERVER_STREAM_READ_NEED_HEADER:
      {
        unsigned start = xfer->read_info.need_header.checked;
        if (start == ss->incoming_data.size)
          break;

        if (!_dsk_http_scan_for_end_of_header (&ss->incoming_data, 
                                          &xfer->read_info.need_header.checked,
                                          DSK_FALSE))
          {
            /* check for max header size */
            if (xfer->read_info.need_header.checked > ss->max_header_size)
              {
                server_set_error (ss, "header too long (at least %u bytes)",
                                  (unsigned)xfer->read_info.need_header.checked);
                goto return_false;
              }
            goto return_true;
          }
        else
          {
            /* parse header */
            unsigned header_len = xfer->read_info.need_header.checked;
            dsk_boolean has_content;
            DskHttpRequest *request;
            dsk_boolean empty_content_body;
            request = dsk_http_request_parse_buffer (&ss->incoming_data, header_len, &error);
            if (request == NULL)
              {
                server_set_error (ss, "HTTP server: error parsing HTTP request from client: %s",
                                  error->message);
                goto return_false;
              }
            xfer->request = request;
            dsk_buffer_discard (&ss->incoming_data, header_len);
            if (request->is_websocket_request)
              {
                xfer->read_state = DSK_HTTP_SERVER_STREAM_READ_WEBSOCKET;
                goto restart_processing;
              }
            has_content = request->verb == DSK_HTTP_VERB_PUT
                       || request->verb == DSK_HTTP_VERB_POST;

            if (has_content)
              {
                xfer->post_data = dsk_memory_source_new ();
                if (ss->wait_for_content_complete)
                  xfer->post_data->buffer_low_amount = ss->max_post_data_size;
                dsk_object_trap_finalize (DSK_OBJECT (xfer->post_data),
                                          handle_post_data_finalize,
                                          xfer);
              }

            empty_content_body = !has_content
                               || (request->content_length == 0LL
                                   && !request->transfer_encoding_chunked);

            if (empty_content_body)
              {
                xfer->read_state = DSK_HTTP_SERVER_STREAM_READ_DONE;
                if (xfer->post_data != NULL)
                  dsk_memory_source_done_adding (xfer->post_data);
                dsk_assert (xfer->next == NULL);
                ss->read_transfer = NULL;
                if (xfer->request->connection_close)
                  ss->no_more_transfers = DSK_TRUE;
              }
            else
              {
                if (request->transfer_encoding_chunked)
                  {
                    xfer->read_state = DSK_HTTP_SERVER_STREAM_READ_IN_XFER_CHUNKED_HEADER;
                    xfer->read_info.in_xfer_chunk.remaining = 0;
                  }
                else if (request->content_length != -1LL)
                  {
                    xfer->read_state = DSK_HTTP_SERVER_STREAM_READ_IN_POST;
                    xfer->read_info.in_post.remaining = request->content_length;
                  }
                else
                  {
                    server_set_error (ss, "need Content-Length or Transfer-Encoding chunked for %s data",
                                      dsk_http_verb_name (request->verb));
                    goto return_false;
                  }
              }
            if (empty_content_body || !ss->wait_for_content_complete)
              {
                if (ss->next_request == NULL)
                  ss->next_request = xfer;

                /* notify that we have a header */
                dsk_hook_set_idle_notify (&ss->request_available, DSK_TRUE);
              }
          }
        goto restart_processing;
      }
    case DSK_HTTP_SERVER_STREAM_READ_IN_POST:
      {
        unsigned amount = ss->incoming_data.size;
        if (amount > xfer->read_info.in_post.remaining)
          amount = xfer->read_info.in_post.remaining;

        if (xfer->post_data != NULL)
          dsk_buffer_transfer (&xfer->post_data->buffer, &ss->incoming_data, amount);
        else
          dsk_buffer_discard (&ss->incoming_data, amount);
        xfer->read_info.in_post.remaining -= amount;
        if (xfer->read_info.in_post.remaining == 0)
          {
            /* done reading post data */
            done_reading_post_data (ss);
            goto restart_processing;
          }
        goto return_true;
      }
    case DSK_HTTP_SERVER_STREAM_READ_IN_XFER_CHUNKED_HEADER:
    in_xfer_chunk_header:
      {
        int c;
        while ((c=dsk_buffer_read_byte (&ss->incoming_data)) != -1)
          {
            if (dsk_ascii_isxdigit (c))
              {
                xfer->read_info.in_xfer_chunk.remaining <<= 4;
                xfer->read_info.in_xfer_chunk.remaining += dsk_ascii_xdigit_value (c);
              }
            else if (c == '\n')
              {
                /* switch state */
                if (xfer->read_info.in_xfer_chunk.remaining == 0)
                  {
                    xfer->read_state = DSK_HTTP_SERVER_STREAM_READ_XFER_CHUNK_TRAILER;
                    goto in_trailer;
                  }
                else
                  {
                    xfer->read_state = DSK_HTTP_SERVER_STREAM_READ_IN_XFER_CHUNK;
                    goto in_xfer_chunk;
                  }
              }
            else if (dsk_ascii_isspace (c))
              {
                /* do nothing */
              }
            else if (c == ';')
              {
                xfer->read_state = DSK_HTTP_SERVER_STREAM_READ_IN_XFER_CHUNKED_HEADER_EXTENSION;
                goto in_xfer_chunked_header_extension;
              }
            else
              {
                server_set_error (ss, "unexpected character %s in chunked POST data",
                                  dsk_ascii_byte_name (c));
                goto return_false;
              }
          }
        goto return_true;
      }
    case DSK_HTTP_SERVER_STREAM_READ_IN_XFER_CHUNKED_HEADER_EXTENSION:
    in_xfer_chunked_header_extension:
      {
        int c;
        while ((c=dsk_buffer_read_byte (&ss->incoming_data)) != -1)
          {
            if (c == '\n')
              {
                /* switch state */
                xfer->read_state = DSK_HTTP_SERVER_STREAM_READ_IN_XFER_CHUNK;
                goto in_xfer_chunk;
              }
            else
              {
                /* do nothing */
              }
          }
        goto return_true;
      }
    case DSK_HTTP_SERVER_STREAM_READ_IN_XFER_CHUNK:
    in_xfer_chunk:
      {
        unsigned amount = ss->incoming_data.size;
        if (amount > xfer->read_info.in_xfer_chunk.remaining)
          amount = xfer->read_info.in_xfer_chunk.remaining;
        if (!transfer_post_content (xfer, amount))
          {
            goto return_false;
          }
        xfer->read_info.in_xfer_chunk.remaining -= amount;
        if (xfer->read_info.in_xfer_chunk.remaining == 0)
          {
            xfer->read_state = DSK_HTTP_SERVER_STREAM_READ_AFTER_XFER_CHUNK;
            goto after_xfer_chunk;
          }
        goto return_true;
      }
    case DSK_HTTP_SERVER_STREAM_READ_AFTER_XFER_CHUNK:
    after_xfer_chunk:
      {
        for (;;)
          {
            int c = dsk_buffer_read_byte (&ss->incoming_data);
            if (c == '\r')
              continue;
            if (c == '\n')
              {
                xfer->read_state = DSK_HTTP_SERVER_STREAM_READ_IN_XFER_CHUNKED_HEADER;
                xfer->read_info.in_xfer_chunk.remaining = 0;
                goto in_xfer_chunk_header;
              }
            if (c == -1)
              goto return_true;
            server_set_error (ss,
                              "unexpected char %s after chunk",
                              dsk_ascii_byte_name (c));
            goto return_false;
          }
      }
      break;
    case DSK_HTTP_SERVER_STREAM_READ_XFER_CHUNK_TRAILER:
    in_trailer:
      /* We are looking for the end of content similar to the end
         of an http-header: two consecutive newlines. */
      if (!_dsk_http_scan_for_end_of_header (&ss->incoming_data, 
                              &xfer->read_info.in_xfer_chunk_trailer.checked,
                                             DSK_TRUE))
        {
          goto return_true;
        }
      dsk_buffer_discard (&ss->incoming_data,
                          xfer->read_info.in_xfer_chunk_trailer.checked);
      done_reading_post_data (ss);
      xfer = NULL;            /* reduce chances of bugs */
      goto restart_processing;
#if 0
    case DSK_HTTP_SERVER_STREAM_READ_XFER_CHUNK_FINAL_NEWLINE:
      {
        int c;
        while ((c = dsk_buffer_peek_byte (&ss->incoming_data)) != -1)
          {
            if (c == '\r' || c == ' ')
              {
                dsk_buffer_discard (&ss->incoming_data, 1);
                continue;
              }
            if (c == '\n')
              {
                dsk_buffer_discard (&ss->incoming_data, 1);
                done_reading_post_data (ss);
                xfer = NULL;            /* reduce chances of bugs */
                goto restart_processing;
              }
            //if (!ss->strict_keepalive)
              {
                done_reading_post_data (ss);
                xfer = NULL;            /* reduce chances of bugs */
                goto restart_processing;
              }
            //else
              //{
                //server_set_error (ss,
                                  //"bad character after transfer-encoding: chunked; expected newline");
                //goto return_false;
              //}
          }
        goto return_true;
      }
#endif
    case DSK_HTTP_SERVER_STREAM_READ_DONE:
      dsk_assert_not_reached ();
      break;
    case DSK_HTTP_SERVER_STREAM_READ_WEBSOCKET:
      if (ss->incoming_data.size < 8)
        goto return_true;
      if (ss->first_transfer == xfer)
        {
          if (ss->next_request == NULL)
            ss->next_request = xfer;
          dsk_hook_set_idle_notify (&ss->request_available, DSK_TRUE);
        }
      break;
    }

return_true:
  /* In the client, we handled EOF at the end;
     such behavior isn't really appropriate for the server.
     (And I'm not sure it makes sense for the client either) */
  return DSK_TRUE;

return_false:
  ss->read_trap = NULL;
  return DSK_FALSE;
}

DskHttpServerStream *
dsk_http_server_stream_new     (DskOctetSink        *sink,
                                DskOctetSource      *source,
                                DskHttpServerStreamOptions *options)
{
  DskHttpServerStream *ss = dsk_object_new (&dsk_http_server_stream_class);
  ss->sink = dsk_object_ref (sink);
  ss->source = dsk_object_ref (source);
  ss->max_header_size = options->max_header_size;
  ss->max_pipelined_requests = options->max_pipelined_requests;
  ss->max_post_data_size = options->max_post_data_size;
  ss->max_outgoing_buffer_size = options->max_outgoing_buffer_size;
  ss->wait_for_content_complete = options->wait_for_content_complete ? 1 : 0;
  ss->read_trap = dsk_hook_trap (&source->readable_hook,
                                 (DskHookFunc) handle_source_readable,
                                 ss, NULL);
  return ss;
}

static dsk_boolean
is_ready_to_return (DskHttpServerStream *stream,
                    DskHttpServerStreamTransfer *xfer)
{
  if (xfer->request == NULL)
    return DSK_FALSE;
  if (xfer->read_state == DSK_HTTP_SERVER_STREAM_READ_DONE)
    return DSK_TRUE;
  if (!stream->wait_for_content_complete)
    return DSK_TRUE;
  return DSK_FALSE;
}

DskHttpServerStreamTransfer *
dsk_http_server_stream_get_request (DskHttpServerStream *stream)
{
  DskHttpServerStreamTransfer *rv = stream->next_request;
  if (rv != NULL)
    {
      if (rv->request->is_websocket_request
       && stream->first_transfer != rv)
        {
          return NULL;
        }
      rv->returned = DSK_TRUE;
      stream->next_request = rv->next;

      if (stream->next_request != NULL
       && !is_ready_to_return (stream, stream->next_request))
        stream->next_request = NULL;

        /* If the ref-count of the post-data gets to 0,
           we will see the finalizer handler get called,
           which will in turn set post_data to NULL;
           once post_data is NULL, we discard any further POST data. */
      if (rv->post_data != NULL)
        dsk_main_add_idle (dsk_object_unref_f, rv->post_data);
    }
  if (stream->next_request == NULL)
    dsk_hook_set_idle_notify (&stream->request_available, DSK_FALSE);
  return rv;
}

void
dsk_http_server_stream_transfer_set_funcs (DskHttpServerStreamTransfer      *transfer,
                                  DskHttpServerStreamFuncs         *funcs,
                                  void                             *func_data)
{
  transfer->funcs = funcs;
  transfer->func_data = func_data;
}

static dsk_boolean
check_header       (DskHttpServerStreamTransfer *transfer,
                    DskHttpServerStreamResponseOptions *options,
                    DskError                   **error)
{
  DskHttpResponse *header = options->header;
  DSK_UNUSED (transfer);
  if (options->content_stream != NULL)
    {
      int64_t length = dsk_octet_source_get_length (options->content_stream);
      if (length >= 0LL)
        {
          if (options->content_length >= 0LL && length != options->content_length)
            {
              dsk_set_error (error, "Content-Length mismatch (stream v response options)");
              return DSK_FALSE;
            }
          if (header->content_length >= 0LL && length != header->content_length)
            {
              dsk_set_error (error, "Content-Length mismatch (stream v header)");
              return DSK_FALSE;
            }
          header->content_length = length;
        }
    }
  if (options->content_length >= 0)
    {
      if (header->content_length >= 0LL && options->content_length != header->content_length)
        {
          dsk_set_error (error, "Content-Length mismatch (response options v header)");
          return DSK_FALSE;
        }
      header->content_length = options->content_length;
    }
  return DSK_TRUE;
}

static DskHttpResponse *
construct_response (DskHttpServerStreamTransfer *transfer,
                    DskHttpServerStreamResponseOptions *options,
                    DskError **error)
{
  DskHttpResponseOptions hdr = *(options->header_options);
  DskHttpResponse *rv;
  if (options->content_stream != NULL)
    {
      int64_t length = dsk_octet_source_get_length (options->content_stream);
      if (length >= 0LL)
        {
          if (options->content_length >= 0LL && length != options->content_length)
            {
              dsk_set_error (error, "Content-Length mismatch (stream v response options)");
              return NULL;
            }
          if (hdr.content_length >= 0LL && length != hdr.content_length)
            {
              dsk_set_error (error, "Content-Length mismatch (stream v header options)");
              return NULL;
            }
          hdr.content_length = length;
        }
    }
  if (options->content_length >= 0)
    {
      if (hdr.content_length >= 0LL && options->content_length != hdr.content_length)
        {
          dsk_set_error (error, "Content-Length mismatch (response options v header options)");
          return NULL;
        }
      hdr.content_length = options->content_length;
    }
  if (hdr.request == NULL)
    hdr.request = transfer->request;
  rv = dsk_http_response_new (&hdr, error);
  /* cleanup? */
  return rv;
}

static dsk_boolean
handle_sink_writable (DskOctetSink *sink,
                      DskHttpServerStream *stream)
{
  DskError *error = NULL;
  switch (dsk_octet_sink_write_buffer (sink, &stream->outgoing_data, &error))
    {
    case DSK_IO_RESULT_SUCCESS:
    case DSK_IO_RESULT_AGAIN:
      if (stream->outgoing_data.size <= stream->max_outgoing_buffer_size
       && stream->first_transfer != NULL
       && stream->first_transfer->blocked_content)
        {
          stream->first_transfer->blocked_content = DSK_FALSE;
          dsk_hook_trap_unblock (stream->first_transfer->content_readable_trap);
        }
      if (stream->outgoing_data.size == 0)
        {
          stream->write_trap = NULL;
          if (stream->deferred_shutdown)
            do_shutdown (stream, NULL);
          return DSK_FALSE;
        }
      return DSK_TRUE;
    case DSK_IO_RESULT_EOF:
      error = dsk_error_new ("unexpect EOF writing");
      /* fallthrough */
    case DSK_IO_RESULT_ERROR:
      {
        stream->write_trap = NULL;
        server_set_error (stream, "error writing to transport: %s", error->message);
        return DSK_FALSE;
      }
    }
  dsk_return_val_if_reached (NULL, DSK_FALSE);
}

static dsk_boolean
handle_content_readable (DskOctetSource *content,
                         DskHttpServerStreamTransfer *xfer)
{
  DskBuffer tmp_buf = DSK_BUFFER_INIT;
  DskBuffer *out = &xfer->owner->outgoing_data;
  DskBuffer *buf;
  DskError *error = NULL;
  dsk_boolean got_eof = DSK_FALSE;
  if (xfer->response->transfer_encoding_chunked)
    buf = &tmp_buf;
  else
    buf = out;
  switch (dsk_octet_source_read_buffer (content, buf, &error))
    {
    case DSK_IO_RESULT_SUCCESS:
      break;
    case DSK_IO_RESULT_AGAIN:
      return DSK_TRUE;
    case DSK_IO_RESULT_ERROR:
      do_shutdown (xfer->owner, error);
      dsk_error_unref (error);
      return DSK_FALSE;
    case DSK_IO_RESULT_EOF:
      got_eof = DSK_TRUE;
      break;
    }
  if (xfer->response->transfer_encoding_chunked)
    {
      /* write xfer chunked header */
      char hdr_buf[32];
      if (!got_eof && buf->size == 0)
        return DSK_TRUE;
      snprintf (hdr_buf, sizeof (hdr_buf), "%x\r\n", (unsigned) buf->size);
      dsk_buffer_append_string (out, hdr_buf);
      dsk_buffer_drain (out, buf);
      dsk_buffer_append (out, 2, "\r\n");
    }
  else
    {
      /* nothing to do */
    }
  if (xfer->owner->write_trap == NULL && out->size > 0)
    {
      xfer->owner->write_trap = dsk_hook_trap (&xfer->owner->sink->writable_hook,
                                               (DskHookFunc) handle_sink_writable,
                                               xfer->owner,
                                               NULL);
    }
  if (xfer->owner->outgoing_data.size > xfer->owner->max_outgoing_buffer_size && !got_eof)
    {
      xfer->blocked_content = 1;
      dsk_hook_trap_block (xfer->content_readable_trap);
    }
  if (got_eof)
    {
      dsk_boolean close = xfer->response->connection_close;
      DskHttpServerStream *ss = xfer->owner;

      /* destroy transfer */
      dsk_assert (xfer == ss->first_transfer);
      ss->first_transfer = xfer->next;
      if (xfer->next == NULL)
        ss->last_transfer = NULL;
      if (xfer->funcs != NULL
       && xfer->funcs->destroy != NULL)
        xfer->funcs->destroy (xfer);
      free_transfer (xfer);

      if (close)
        do_deferred_shutdown (ss);
      else if (ss->first_transfer != NULL)
        { 
          /* dump header / trap content / trap writable */
          if (ss->first_transfer->responded)
            start_transfer (ss->first_transfer);
          else if (ss->first_transfer->request->is_websocket_request)
            dsk_hook_set_idle_notify (&ss->request_available, DSK_TRUE);
        }

      return DSK_FALSE;
    }
  return DSK_TRUE;
}

static void
start_transfer (DskHttpServerStreamTransfer *transfer)
{
  DskHttpServerStream *ss = transfer->owner;
  /* shove header into outgoing buffer */
  dsk_http_response_print_buffer (transfer->response,
                                  &ss->outgoing_data);
  dsk_buffer_append (&ss->outgoing_data, 2, "\r\n");

  /* ensure we have a write-trap */
  if (ss->write_trap == NULL)
    {
      ss->write_trap = dsk_hook_trap (&ss->sink->writable_hook,
                                                   (DskHookFunc) handle_sink_writable,
                                                   transfer->owner,
                                                   NULL);
    }

  /* trap content readable */
  if (transfer->content)
    {
      transfer->content_readable_trap = dsk_hook_trap (&transfer->content->readable_hook,
                                                   (DskHookFunc) handle_content_readable,
                                                   transfer,
                                                   NULL);

      /* XXX: this is actually a good implementation of kickstarting
         the reading process rather than waiting for the hook to idle-notify,
         as it might. However, it's a rather creative use of dsk_hook_notify() */
      /* XXX: Maybe an impl of dsk_hook_trap_notify() would be safer? */
      dsk_hook_notify (&transfer->content->readable_hook);
    }
  else
    {
      if (transfer->response->connection_close)
        {
          do_deferred_shutdown (transfer->owner);
        }
    }
}

/* TODO: mark no_more_transfers as needed */
dsk_boolean
dsk_http_server_stream_respond (DskHttpServerStreamTransfer *transfer,
                                DskHttpServerStreamResponseOptions *options,
                                DskError **error)
{
  DskHttpResponse *header = NULL;
  dsk_assert (transfer->returned);
  dsk_assert (!transfer->responded);

  transfer->responded = DSK_TRUE;

  /* transfer has failed (transport error or protocol error);
     silently destroy it; options is allowed to be NULL in this case. */
  if (transfer->failed)
    {
      dsk_object_unref (transfer->request);
      if (transfer->post_data)
        {
          dsk_object_untrap_finalize (DSK_OBJECT (transfer->post_data),
                                      handle_post_data_finalize,
                                      transfer);
          dsk_object_unref (transfer->post_data);
          transfer->post_data = NULL;
        }
      dsk_free (transfer);
      if (options != NULL)
        {
          dsk_set_error (error, "transfer has failed before you responded");
          return DSK_FALSE;
        }
      return DSK_TRUE;
    }
  if (options->header != NULL)
    {
      if (!check_header (transfer, options, error))
        goto invalid_arguments;
      header = dsk_object_ref (options->header);
    }
  else if (options->header_options != NULL)
    {
      header = construct_response (transfer, options, error);
      if (header == NULL)
        goto invalid_arguments;
    }
  else
    goto invalid_arguments;
  if (dsk_http_has_response_body (transfer->request->verb, header->status_code))
    {
      DskOctetSource *source;
      if (options->content_stream != NULL)
        source = dsk_object_ref (options->content_stream);
      else if (header->content_length == 0
           ||  options->content_body != NULL)
        {
          DskMemorySource *msource = dsk_memory_source_new ();
          dsk_buffer_append (&msource->buffer, header->content_length, options->content_body);
          dsk_memory_source_added_data (msource);
          dsk_memory_source_done_adding (msource);
          source = DSK_OCTET_SOURCE (msource);
        }
      else
        {
          dsk_set_error (error, "content required");
          goto invalid_arguments;
        }
      transfer->content = source;
    }
  else if (options->content_stream != NULL)
    {
      /* what to do: ignore or drain or error? */
      dsk_octet_source_shutdown (options->content_stream);
#if DEBUG_ODD_VERB_USE
      if (transfer->request->verb == DSK_HTTP_VERB_HEAD)
        dsk_warning ("content-stream provided for HEAD request");
      else
        dsk_warning ("content-stream provided for %u response", header->status_code);
#endif
    }

  /* internal server error: set-up header/source appropriately */
  if (0)
    {
invalid_arguments:
      {
        DskHttpResponseOptions opts = DSK_HTTP_RESPONSE_OPTIONS_INIT;
        DskMemorySource *msource = dsk_memory_source_new ();
        DskError *e = NULL;
        opts.request = transfer->request;
        opts.status_code = 500;
        opts.content_type = "text/plain";
        dsk_warning ("dsk_http_server_stream_respond failed: %s", (*error)->message);
        dsk_buffer_append_string (&msource->buffer,
                                  "dsk_http_server_stream_respond failed: ");
        if (error)
          dsk_buffer_append_string (&msource->buffer, (*error)->message);
        else
          dsk_buffer_append_string (&msource->buffer,
                                    "no error message available");
        dsk_buffer_append_byte (&msource->buffer, '\n');
        dsk_memory_source_added_data (msource);
        dsk_memory_source_done_adding (msource);
        opts.content_length = msource->buffer.size;
        if (header)
          dsk_object_unref (header);
        header = dsk_http_response_new (&opts, &e);
        if (header == NULL)
          dsk_die ("constructing internal server error: failed: %s", e->message);
        dsk_assert (header != NULL);
        transfer->content = DSK_OCTET_SOURCE (msource);
      }
    }

  transfer->response = header;
  if (header->connection_close)
    transfer->owner->no_more_transfers = DSK_TRUE;

  if (transfer->owner->first_transfer == transfer)
    {
      start_transfer (transfer);
    }
  else
    {
      /* waiting for another transfer to finish writing */
    }
  return DSK_TRUE;

}


dsk_boolean
dsk_http_server_stream_respond_websocket
                               (DskHttpServerStreamTransfer *transfer,
                                const char *protocol,
                                DskWebsocket **websocket_out)
{
  DskHttpResponseOptions resp_options = DSK_HTTP_RESPONSE_OPTIONS_INIT;
  DskHttpResponse *response;
  DskHttpServerStream *stream = transfer->owner;
  const char *origin = dsk_http_request_get (transfer->request, "Origin");
  const char *host = dsk_http_request_get (transfer->request, "Host");
  const char *key1 = dsk_http_request_get (transfer->request, "Sec-Websocket-Key1");
  const char *key2 = dsk_http_request_get (transfer->request, "Sec-Websocket-Key2");
  char *location = NULL;
  DskHttpHeaderMisc misc[5];
  unsigned n_misc = 0;
  const char *msg = NULL;
  DskError *error = NULL;

  if (host == NULL)
    {
      msg = "missing Host";
      goto handle_error;
    }
  if (key1 == NULL || key2 == NULL)
    {
      msg = "missing Sec-Websocket-Key1 or -Key2";
      goto handle_error;
    }

  misc[n_misc].key = "Upgrade";
  misc[n_misc].value = "WebSocket";
  n_misc++;

  if (origin)
    {
      misc[n_misc].key = "Sec-Websocket-Origin";
      misc[n_misc].value = (char*) origin;
      n_misc++;
    }

  {
    unsigned loc_len = 5 + strlen (host) + strlen (transfer->request->path)
                     + 1;
    location = dsk_malloc (loc_len);
    strcpy (location, "ws://");
    dsk_stpcpy (dsk_stpcpy (location + 5, host), transfer->request->path);

    misc[n_misc].key = "Sec-Websocket-Location";
    misc[n_misc].value = location;
    n_misc++;
  }
  
  if (protocol)
    {
      misc[n_misc].key = "Sec-Websocket-Protocol";
      misc[n_misc].value = (char*) protocol;
      n_misc++;
    }

  resp_options.status_code = DSK_HTTP_STATUS_SWITCHING_PROTOCOLS;
  resp_options.connection_upgrade = 1;
  resp_options.n_unparsed_headers = n_misc;
  resp_options.unparsed_misc_headers = misc;
  response = dsk_http_response_new (&resp_options, NULL);
  dsk_assert (response != NULL);
  dsk_free (location);
  location = NULL;

  /* create memory source + sink */
  if (stream->read_trap)
    {
      DskHookTrap *trap = stream->read_trap;
      stream->read_trap = NULL;
      dsk_hook_trap_destroy (trap);
    }
  if (stream->write_trap)
    {
      DskHookTrap *trap = stream->write_trap;
      stream->write_trap = NULL;
      dsk_hook_trap_destroy (trap);
    }

  /* compute websocket response */
  uint8_t key3[8];
  uint8_t ws_response[16];
  if (dsk_buffer_read (&stream->incoming_data, 8, key3) != 8)
    dsk_assert_not_reached ();
  if (!_dsk_websocket_compute_response (key1, key2, key3, ws_response, &error))
    {
      msg = error->message;
      goto handle_error;
    }

  DskWebsocket *websocket;
  websocket = dsk_object_new (&dsk_websocket_class);
  dsk_http_response_print_buffer (response, &websocket->outgoing);
  dsk_buffer_append (&websocket->outgoing, 2, "\r\n");
  dsk_buffer_append (&websocket->outgoing, 16, ws_response);

  _dsk_websocket_server_init (websocket, stream->source, stream->sink);
  stream->sink = NULL;
  stream->source = NULL;
  *websocket_out = websocket;
  return DSK_TRUE;

handle_error:
  do_deferred_shutdown (stream);
  dsk_warning ("invalid handshake in websocket: %s", msg);
  if (error)
    dsk_error_unref (error);
  *websocket_out = NULL;
  return DSK_FALSE;
}


static void
dsk_http_server_stream_init (DskHttpServerStream *stream)
{
  dsk_hook_init (&stream->request_available, stream);
}
static void
dsk_http_server_stream_finalize (DskHttpServerStream *stream)
{
  do_shutdown (stream, NULL);
  dsk_buffer_clear (&stream->incoming_data);
  dsk_buffer_clear (&stream->outgoing_data);
  if (!stream->request_available.is_cleared)
    dsk_hook_clear (&stream->request_available);
}

void
dsk_http_server_stream_shutdown (DskHttpServerStream *stream)
{
  do_shutdown (stream, NULL);
}

DSK_OBJECT_CLASS_DEFINE_CACHE_DATA(DskHttpServerStream);
DskHttpServerStreamClass dsk_http_server_stream_class =
{
  DSK_OBJECT_CLASS_DEFINE (DskHttpServerStream,
                           &dsk_object_class,
                           dsk_http_server_stream_init,
                           dsk_http_server_stream_finalize)
};
