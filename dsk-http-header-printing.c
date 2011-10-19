#include <stdio.h>
#include <string.h>
#include "dsk.h"

static void
append_space_http_version (DskBuffer *buffer,
                           unsigned   major,
                           unsigned   minor,
                           dsk_boolean skip_space)
{
  /* example: ' http/1.0' */
  char buf[] = { ' ', 'H', 'T', 'T', 'P', '/', '0' + major, '.', '0' + minor };
  dsk_buffer_append (buffer, 9 - skip_space, buf + skip_space);
}

static void
append_newline (DskBuffer *buffer)
{
  dsk_buffer_append (buffer, 2, "\r\n");
}

static void
append_content_type (DskBuffer *buffer,
                     const char *content_type)
{
  const char *subtype = strchr (content_type, '/');
  const char *charset = NULL;
  if (subtype != NULL)
    {
      subtype++;
      charset = strchr (subtype, '/');
      if (charset)
        charset++;
    }
  if (charset != NULL)
    {
      dsk_buffer_append (buffer, charset - 1 - content_type, content_type);
      dsk_buffer_append_string (buffer, "; charset=");
      dsk_buffer_append_string (buffer, charset);
    }
  else
    dsk_buffer_append_string (buffer, content_type);
}

static void
append_date (DskBuffer *buffer, dsk_time_t time)
{
  DskDate date;
  char buf[DSK_DATE_MAX_LENGTH];
  dsk_unixtime_to_date (time, &date);
  dsk_date_print_rfc822 (&date, buf);
  dsk_buffer_append_string (buffer, buf);
}
static void
append_md5 (DskBuffer *buffer, const uint8_t *md5)
{
  unsigned i;
  for (i = 0; i < 16; i++)
    {
      char buf[3];
      snprintf (buf, sizeof (buf), "%02x", md5[i]);
      dsk_buffer_append (buffer, 2, buf);
    }
}
static void
maybe_add_simple_header (DskBuffer *buffer,
                         const char *header,
                         const char *value)
{
  if (value)
    {
      dsk_buffer_append_string (buffer, header);
      dsk_buffer_append (buffer, 2, ": ");
      dsk_buffer_append_string (buffer, value);
      append_newline (buffer);
    }
}

static void
append_unparsed_headers (DskBuffer *buffer,
                         unsigned   n,
                         const DskHttpHeaderMisc *misc)
{
  unsigned i;
  for (i = 0; i < n; i++)
    {
      dsk_buffer_append_string (buffer, misc[i].key);
      dsk_buffer_append (buffer, 2, ": ");
      dsk_buffer_append_string (buffer, misc[i].value);
      append_newline (buffer);
    }
}

void             dsk_http_request_print_buffer  (DskHttpRequest *request,
                                                 DskBuffer *buffer)
{
  dsk_buffer_append_string (buffer, dsk_http_verb_name (request->verb));
  dsk_buffer_append_byte (buffer, ' ');
  dsk_buffer_append_string (buffer, request->path);
  append_space_http_version (buffer,
                             request->http_minor_version,
                             request->http_major_version,
                             DSK_FALSE);
  append_newline (buffer);
  if (request->transfer_encoding_chunked)
    dsk_buffer_append_string (buffer, "Transfer-Encoding: chunked\r\n");
  if (request->has_date)
    {
      dsk_buffer_append_string (buffer, "Date: ");
      append_date (buffer, request->date);
    }
  if (request->connection_close)
    dsk_buffer_append_string (buffer, "Connection: close\r\n");
  else if (request->connection_upgrade)
    dsk_buffer_append_string (buffer, "Connection: upgrade\r\n");
  if (request->content_encoding_gzip)
    dsk_buffer_append_string (buffer, "Content-Encoding: gzip\r\n");
  if (request->supports_content_encoding_gzip)
    dsk_buffer_append_string (buffer, "Accept-Encoding: gzip\r\n");
  if (request->content_type)
    {
      dsk_buffer_append_string (buffer, "Content-Type: ");
      append_content_type (buffer, request->content_type);
      append_newline (buffer);
    }
  if (request->content_length != -1LL)
    {
      char buf[20];
      snprintf (buf, sizeof (buf), "%llu", (unsigned long long) request->content_length);
      dsk_buffer_append_string (buffer, "Content-Length: ");
      dsk_buffer_append_string (buffer, buf);
      append_newline (buffer);
    }
  maybe_add_simple_header (buffer, "Host", request->host);
  maybe_add_simple_header (buffer, "User-Agent", request->user_agent);
  maybe_add_simple_header (buffer, "Referer", request->referrer);

  append_unparsed_headers (buffer,
                           request->n_unparsed_headers,
                           request->unparsed_headers);
}

static void
append_status (DskBuffer *buffer,
               DskHttpStatus status)
{
  char buf[20];
  snprintf (buf, sizeof (buf), "%u ", status);
  dsk_buffer_append_string (buffer, buf);
  dsk_buffer_append_string (buffer, dsk_http_status_get_message (status));
}
void
dsk_http_response_print_buffer  (DskHttpResponse *response,
                                 DskBuffer *buffer)
{
  append_space_http_version (buffer,
                             response->http_major_version,
                             response->http_minor_version,
                             DSK_TRUE);
  dsk_buffer_append_byte (buffer, ' ');
  append_status(buffer, response->status_code);
  append_newline (buffer);

  if (response->transfer_encoding_chunked)
    dsk_buffer_append_string (buffer, "Transfer-Encoding: chunked\r\n");
  if (response->connection_close)
    dsk_buffer_append_string (buffer, "Connection: close\r\n");
  if (response->accept_ranges)
    dsk_buffer_append_string (buffer, "Accept-Ranges: bytes\r\n");
  if (response->content_type)
    {
      dsk_buffer_append_string (buffer, "Content-Type: ");
      append_content_type (buffer, response->content_type);
      append_newline (buffer);
    }
  if (response->content_length != -1LL)
    {
      char buf[20];
      snprintf (buf, sizeof (buf), "%llu", (unsigned long long) response->content_length);
      dsk_buffer_append_string (buffer, "Content-Length: ");
      dsk_buffer_append_string (buffer, buf);
      append_newline (buffer);
    }

  if (response->has_date)
    {
      dsk_buffer_append_string (buffer, "Date: ");
      append_date (buffer, response->date);
    }
  if (response->has_expires)
    {
      dsk_buffer_append_string (buffer, "Expires: ");
      append_date (buffer, response->expires);
    }
  if (response->has_last_modified)
    {
      dsk_buffer_append_string (buffer, "Last-Modified: ");
      append_date (buffer, response->expires);
    }
  if (response->content_encoding_gzip)
    dsk_buffer_append_string (buffer, "Content-Encoding: gzip\r\n");
  if (response->has_md5sum)
    {
      dsk_buffer_append_string (buffer, "Content-MD5: ");
      append_md5 (buffer, response->md5sum);
      append_newline (buffer);
    }

  /* The `Location' to redirect to. */
  maybe_add_simple_header (buffer, "Location", response->location);
  maybe_add_simple_header (buffer, "Server", response->server);

  append_unparsed_headers (buffer,
                           response->n_unparsed_headers,
                           response->unparsed_headers);
}
