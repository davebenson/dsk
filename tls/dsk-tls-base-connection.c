
typedef struct DskTlsBaseHandshake DskTlsBaseHandshake;
struct DskTlsBaseHandshake
{
  DSK_TLS_BASE_HANDSHAKE_MEMBERS(DskTlsBaseConnection);
};

static void
dsk_tls_server_connection_finalize (DskTlsServerConnection *conn)
{
  ....
}

static bool
dsk_tls_base_connection_handle_record (DskTlsBaseConnection *connection,
                                       DskTlsRecordContentType content_type,
                                       size_t                length,
                                       const uint8_t        *data,
                                       DskError            **error)
{
  switch (content_type)
    {
      case DSK_TLS_RECORD_CONTENT_TYPE_HANDSHAKE:
        {
          DskTlsBaseHandshake *hs_info = connection->handshake;
          switch (connection->handshake_message_state)
            {
            case DSK_TLS_BASE_HMS_INIT:
              if (length >= 4)
                {
                  memcpy (connection->hm_header, data, 4);
                  length -= 4;
                  data += 4;
                  connection->handshake_message_state = DSK_TLS_BASE_HMS_GATHERING_BODY;
                  connection->hm_length = dsk_uint24be_parse (connection->hm_header+1);
                  connection->cur_hm_length = 0;
                  connection->hm_data = dsk_mem_pool_alloc (&hs_info->mem_pool, connection->hm_length);
                  goto state_GATHERING_BODY;
                }
              else
                {
                  memcpy (connection->hm_header, data, length);
                  connection->hm_header_length = length;
                  connection->handshake_message_state = DSK_TLS_BASE_HMS_HEADER;
                  return true;
                }
            case DSK_TLS_BASE_HMS_HEADER:
              if (length + connection->hm_header_length >= 4)
                {
                  unsigned xfer = 4 - connection->hm_header_length;
                  memcpy (connection->hm_header + connection->hm_header_length,
                          data, xref);
                  length -= xref;
                  data += xref;
                  connection->handshake_message_state = DSK_TLS_BASE_HMS_GATHERING_BODY;
                  connection->hm_length = dsk_uint24be_parse (connection->hm_header+1);
                  connection->cur_hm_length = 0;
                  connection->hm_data = dsk_mem_pool_alloc (...);
                  goto state_GATHERING_BODY;
                }
              else
                {
                  memcpy (connection->hm_header + connection->hm_header_length,
                          data, length);
                  connection->hm_header_length + length;
                  return true;
                }
            state_GATHERING_BODY:
            case DSK_TLS_BASE_HMS_GATHERING_BODY:
              if (length + connection->cur_hm_length >= connection->hm_header)
                {
                  DskTlsHandshakeMessage *hm =
                    dsk_tls_handshake_message_parse (
                       connection->hm_header[0],
                       connection->hm_length,
                       connection->hm_data,
                       &hs_info->mem_pool,
                       error
                    );
                  if (hm == NULL)
                    {
                      return false;
                    }
                  switch (clss->handle_handshake_message (
                            connection,

                          ))
                    {
                    case DSK_TLS_BASE_HANDSHAKE_MESSAGE_RESPONSE_OK:
                      ...
                    case DSK_TLS_BASE_HANDSHAKE_MESSAGE_RESPONSE_FAILED:
                      ...
                    case DSK_TLS_BASE_HANDSHAKE_MESSAGE_RESPONSE_SUSPEND:
                      ...
                    }
                }
              else
                {
                  memcpy (connection->hm_data + connection->cur_hm_length,
                          data, length);
                  connection->cur_hm_length += length;
                  return true;
                }
            case DSK_TLS_BASE_HMS_SUSPENDED:
              ...
            }

          DskTlsServerHandshake *hs_info = c->handshake;
          DskTlsHandshakeMessageType message_type = data[0];
          unsigned hs_length = 
          uint8_t *hs_data = dsk_mem_pool_alloc_unaligned (&hs_info->mem_pool, length);
          memcpy (hs_data, data, length);
          DskTlsHandshakeMessage *message;
          message = dsk_tls_handshake_message_parse (message_type, 
                                                     length, hs_data,
                                                     &hs_info->mem_pool,
                                                     error);
          if (message == NULL)
            {
              return DSK_TLS_BASE_HANDSHAKE_MESSAGE_RESPONSE_FAILED;
            }
          return dsk_tls_server_connection_handle_message (c, message, error);
        }
      case DSK_TLS_RECORD_CONTENT_TYPE_ALERT:
        if (length != 2)
          {
            ...
          }
        DskTlsAlertLevel level = data[0];
        DskTlsAlertDescription desc = data[1];
        switch (level)
          {
          case DSK_TLS_ALERT_LEVEL_WARNING:
            ...
          case DSK_TLS_ALERT_LEVEL_FATAL:
            ...
          }
        ... bad level
        return false;

      case DSK_TLS_RECORD_CONTENT_TYPE_APPLICATION_DATA:
        if (connection->handshake_message_state != DSK_TLS_BASE_HMS_INIT)
          {
            ... application data may not be interleaved with handshake data
            return false;
          }
        ... pass to application layer
        return true;
      default:
        *error = dsk_error_new ("unexpected TLS record-content-type for server");
        return DSK_TLS_BASE_HANDSHAKE_MESSAGE_RESPONSE_FAILED;
    }
}

static bool
handle_underlying_readable (DskStream *underlying,
                            DskTlsBaseConnection *conn)
{
  RecordHeaderParseResult header_result;
  DskTlsBaseConnectionClass *clss = DSK_TLS_BASE_CONNECTION_GET_CLASS (conn);

  assert (conn->underlying == underlying);

  // Perform raw read.
  dsk_stream_read_buffer (underlying, &conn->incoming_raw);
//
//try_parse_record_header:
//  header_result = dsk_tls_parse_record_header (&conn->incoming_raw);
//  switch (header_result.code)
//    {
//    case PARSE_RESULT_CODE_OK:
//      break;
//    case PARSE_RESULT_CODE_ERROR:
//      {
//        dsk_tls_connection_fail (conn, header_result.error);
//        dsk_error_unref (header_result.error);
//        return;
//      }
//    case PARSE_RESULT_CODE_TOO_SHORT:
//      return true;
//    }
//
//  dsk_buffer_discard (&conn->incoming_raw, header_result.header_length);
//
//  ensure_record_data_capacity (conn, header_result.payload_length);
//  uint8_t *record = conn->records_plaintext + conn->records_plaintext_lenth;
//  dsk_buffer_read (&conn->incoming_raw, header_result.payload_length, record);
//
//  if (conn->cipher_suite_read_instance != NULL)
//    {
//      ...
//    }
//  else
//    {
//      conn->records_plaintext_lenth += header_result.payload_length;
//    }
//
//  const uint8_t *parse_at = conn->records_plaintext;
//  const uint8_t *end_parse = conn->records_plaintext + conn->records_plaintext_lenth;
//  switch (header_result.content_type)
//    {
//    case DSK_TLS_RECORD_CONTENT_TYPE_ALERT:
//      {
//        ...
//        break;
//      }
//    case DSK_TLS_RECORD_CONTENT_TYPE_HANDSHAKE:
//      {
//        DskMemPool *pool = clss->peek_handshake_mempool (base);
//        while (parse_at + 4 <= end_parse)
//          {
//            uint32_t len = dsk_uint24be_parse (parse_at + 1);
//            if (parse_at + 4 + len > 0)
//              {
//                ... need more data
//              }
//            uint8_t *msgbuf = dsk_mem_pool_alloc (pool, len);
//            memcpy (msgbuf, parse_at + 4, len);
//            DskTlsHandshakeMessage *handshake = dsk_tls_handshake_parse (*parse_at, len, msgbuf, pool, &error);
//            if (handshake == NULL)
//              {
//                dsk_tls_connection_fail (conn, error);
//                dsk_error_unref (error);
//                return false;
//              }
//            else
//              {
//                switch (clss->handle_handshake_message (conn, handshake, &error))
//                  {
//                    case DSK_TLS_BASE_RECORD_RESPONSE_OK:
//                      ..
//                    case DSK_TLS_BASE_RECORD_RESPONSE_SUSPEND:
//                      ..
//                    case DSK_TLS_BASE_RECORD_RESPONSE_FAILED:
//                      dsk_tls_connection_fail (conn, err);
//                      dsk_error_unref (err);
//                      dsk_tls_handshake_unref (shake);
//                      return false;
//                  }
//              }
//          }
//        break;
//      }
//
//    case DSK_TLS_RECORD_CONTENT_TYPE_APPLICATION_DATA:
//      {
//        // unencrypt and validate data
//        ...
//
//        // 
//        ...
//      }
//
//    case DSK_TLS_RECORD_CONTENT_TYPE_CHANGE_CIPHER_SPEC:
//      {
//        //...
//      }
//      break;
//
//    case DSK_TLS_RECORD_CONTENT_TYPE_HEARTBEAT:
//      {
//        ...
//      }
//      break;
//
//    default:
//      error = dsk_error_new ("unknown ContentType 0x%02x", 
//                             header_result.content_type);
//      dsk_tls_connection_fail (conn, err);
//      dsk_error_unref (err);
//      return false;
//    }
//
//  goto try_parse_record_header;
//}

