
static bool
handle_underlying_readable (DskStream *underlying,
                            DskTlsBaseConnection *conn)
{
  RecordHeaderParseResult header_result;
  DskTlsBaseConnectionClass *clss = DSK_TLS_BASE_CONNECTION_GET_CLASS (conn);

  assert (conn->underlying == underlying);

  // Perform raw read.
  dsk_stream_read_buffer (underlying, &conn->incoming_raw);

try_parse_record_header:
  header_result = dsk_tls_parse_record_header (&conn->incoming_raw);
  switch (header_result.code)
    {
    case PARSE_RESULT_CODE_OK:
      break;
    case PARSE_RESULT_CODE_ERROR:
      {
        dsk_tls_connection_fail (conn, header_result.error);
        dsk_error_unref (header_result.error);
        return;
      }
    case PARSE_RESULT_CODE_TOO_SHORT:
      return true;
    }

  dsk_buffer_discard (&conn->incoming_raw, header_result.header_length);

  ensure_record_data_capacity (conn, header_result.payload_length);
  uint8_t *record = conn->records_plaintext + conn->records_plaintext_lenth;
  dsk_buffer_read (&conn->incoming_raw, header_result.payload_length, record);

  if (conn->cipher_suite_read_instance != NULL)
    {
      ...
    }
  else
    {
      conn->records_plaintext_lenth += header_result.payload_length;
    }

  const uint8_t *parse_at = conn->records_plaintext;
  const uint8_t *end_parse = conn->records_plaintext + conn->records_plaintext_lenth;
  switch (header_result.content_type)
    {
    case DSK_TLS_RECORD_CONTENT_TYPE_ALERT:
      {
        ...
        break;
      }
    case DSK_TLS_RECORD_CONTENT_TYPE_HANDSHAKE:
      {
        DskMemPool *pool = clss->peek_handshake_mempool (base);
        while (parse_at + 4 <= end_parse)
          {
            uint32_t len = dsk_uint24be_parse (parse_at + 1);
            if (parse_at + 4 + len > 0)
              {
                ... need more data
              }
            uint8_t *msgbuf = dsk_mem_pool_alloc (pool, len);
            memcpy (msgbuf, parse_at + 4, len);
            DskTlsHandshakeMessage *handshake = dsk_tls_handshake_parse (*parse_at, len, msgbuf, pool, &error);
            if (handshake == NULL)
              {
                dsk_tls_connection_fail (conn, error);
                dsk_error_unref (error);
                return false;
              }
            else
              {
                switch (clss->handle_handshake_message (conn, handshake, &error))
                  {
                    case DSK_TLS_BASE_RECORD_RESPONSE_OK:
                      ..
                    case DSK_TLS_BASE_RECORD_RESPONSE_SUSPEND:
                      ..
                    case DSK_TLS_BASE_RECORD_RESPONSE_FAILED:
                      dsk_tls_connection_fail (conn, err);
                      dsk_error_unref (err);
                      dsk_tls_handshake_unref (shake);
                      return false;
                  }
              }
          }
        break;
      }

    case DSK_TLS_RECORD_CONTENT_TYPE_APPLICATION_DATA:
      {
        // unencrypt and validate data
        ...

        // 
        ...
      }

    case DSK_TLS_RECORD_CONTENT_TYPE_CHANGE_CIPHER_SPEC:
      {
        //...
      }
      break;

    case DSK_TLS_RECORD_CONTENT_TYPE_HEARTBEAT:
      {
        ...
      }
      break;

    default:
      error = dsk_error_new ("unknown ContentType 0x%02x", 
                             header_result.content_type);
      dsk_tls_connection_fail (conn, err);
      dsk_error_unref (err);
      return false;
    }

  goto try_parse_record_header;
}

