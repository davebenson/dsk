//
// Implement the "record layer" of TLS.
//
// This has actually been fairly
// consistent for a long time and so
// back-compatibility can be
// implemented outside of the core protocol
// implementation.
//
#include "../dsk.h"
#include "dsk-tls-private.h"
#include <string.h>
#include <alloca.h>

static void
dsk_tls_base_connection_finalize (DskTlsBaseConnection *conn)
{
  assert (conn->handshake == NULL); // taken care of by subclass finalize
  if (conn->underlying)
    {
      if (conn->write_trap != NULL)
        dsk_hook_trap_destroy (conn->write_trap);
      if (conn->read_trap != NULL)
        dsk_hook_trap_destroy (conn->read_trap);
      dsk_object_unref (conn->underlying);
    }
  dsk_free (conn->record_buffer);
}

//
//  Handle a 
//
static DskTlsBaseHandshakeMessageResponseCode
dsk_tls_base_connection_handle_handshake_record
                                      (DskTlsBaseConnection *connection,
                                       size_t                length,
                                       const uint8_t        *data,
                                       DskError            **error)
{
  DskTlsBaseHandshake *hs_info = connection->handshake;
  DskTlsBaseConnectionClass *clss = DSK_TLS_BASE_CONNECTION_GET_CLASS (connection);
  switch (connection->handshake_message_state)
    {
    state_INIT:
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
          return DSK_TLS_BASE_HANDSHAKE_MESSAGE_RESPONSE_OK;
        }
    case DSK_TLS_BASE_HMS_HEADER:
      if (length + connection->hm_header_length >= 4)
        {
          unsigned xfer = 4 - connection->hm_header_length;
          memcpy (connection->hm_header + connection->hm_header_length,
                  data,
                  xfer);
          length -= xfer;
          data += xfer;
          connection->handshake_message_state = DSK_TLS_BASE_HMS_GATHERING_BODY;
          connection->hm_length = dsk_uint24be_parse (connection->hm_header+1);
          connection->cur_hm_length = 0;
          connection->hm_data = dsk_mem_pool_alloc (&hs_info->mem_pool, connection->hm_length);
          goto state_GATHERING_BODY;
        }
      else
        {
          memcpy (connection->hm_header + connection->hm_header_length,
                  data, length);
          connection->hm_header_length += length;
          return true;
        }
    state_GATHERING_BODY:
    case DSK_TLS_BASE_HMS_GATHERING_BODY:
      if (length + connection->cur_hm_length >= connection->hm_length)
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
                    hm,
                    error
                  ))
            {
            case DSK_TLS_BASE_HANDSHAKE_MESSAGE_RESPONSE_OK:
              connection->handshake_message_state = DSK_TLS_BASE_HMS_INIT;
              goto state_INIT;

            case DSK_TLS_BASE_HANDSHAKE_MESSAGE_RESPONSE_FAILED:
              return DSK_TLS_BASE_HANDSHAKE_MESSAGE_RESPONSE_FAILED;
            case DSK_TLS_BASE_HANDSHAKE_MESSAGE_RESPONSE_SUSPEND:
              connection->handshake_message_state = DSK_TLS_BASE_HMS_SUSPENDED;
              dsk_hook_trap_block (connection->read_trap);

              // This will be undone in _unsuspend().
              dsk_object_ref (connection);
              return DSK_TLS_BASE_HANDSHAKE_MESSAGE_RESPONSE_SUSPEND;
            }
        }
      else
        {
          memcpy (connection->hm_data + connection->cur_hm_length,
                  data, length);
          connection->cur_hm_length += length;
          return DSK_TLS_BASE_HANDSHAKE_MESSAGE_RESPONSE_OK;
        }
    case DSK_TLS_BASE_HMS_SUSPENDED:
      assert(false);    // record-layer should be suspected too
    }

//  DskTlsHandshakeMessageType message_type = data[0];
//  unsigned hs_length = dsk_uint24be_parse (
//  uint8_t *hs_data = dsk_mem_pool_alloc_unaligned (&hs_info->mem_pool, length);
//  memcpy (hs_data, data, length);
//  DskTlsHandshakeMessage *message;
//  message = dsk_tls_handshake_message_parse (message_type, 
//                                             length, hs_data,
//                                             &hs_info->mem_pool,
//                                             error);
//  if (message == NULL)
//    {
//      return DSK_TLS_BASE_HANDSHAKE_MESSAGE_RESPONSE_FAILED;
//    }
//  return dsk_tls_server_connection_handle_message (c, message, error);
}

//
// Handle a decrypted record (or unencrypted, initially).
//
static DskTlsBaseHandshakeMessageResponseCode
dsk_tls_base_connection_handle_record (DskTlsBaseConnection *connection,
                                       DskTlsRecordContentType content_type,
                                       size_t                length,
                                       const uint8_t        *data,
                                       DskError            **error)
{
  DskTlsBaseConnectionClass *clss = DSK_TLS_BASE_CONNECTION_GET_CLASS (connection);
  switch (content_type)
    {
      case DSK_TLS_RECORD_CONTENT_TYPE_HANDSHAKE:
        return dsk_tls_base_connection_handle_handshake_record
                                         (connection, length, data, error);
          
      case DSK_TLS_RECORD_CONTENT_TYPE_ALERT:
        if (length != 2)
          {
            *error = dsk_error_new ("malformed alert message: wrong length");
            return DSK_TLS_BASE_HANDSHAKE_MESSAGE_RESPONSE_FAILED;
          }
        DskTlsAlertLevel level = data[0];
        DskTlsAlertDescription desc = data[1];
        switch (level)
          {
          case DSK_TLS_ALERT_LEVEL_WARNING:
            if (!clss->warning_alert_received (connection, desc, error))
              return DSK_TLS_BASE_HANDSHAKE_MESSAGE_RESPONSE_FAILED;
            return DSK_TLS_BASE_HANDSHAKE_MESSAGE_RESPONSE_OK;
          case DSK_TLS_ALERT_LEVEL_FATAL:
            clss->fatal_alert_received (connection, desc);
            *error = dsk_error_new ("fatal: %s",
                                    dsk_tls_alert_description_name (desc));
            return DSK_TLS_BASE_HANDSHAKE_MESSAGE_RESPONSE_FAILED;
          default:
            *error = dsk_error_new ("invalid alert level");
            return DSK_TLS_BASE_HANDSHAKE_MESSAGE_RESPONSE_FAILED;
          }

      case DSK_TLS_RECORD_CONTENT_TYPE_APPLICATION_DATA:
        if (connection->handshake_message_state != DSK_TLS_BASE_HMS_INIT)
          {
            *error = dsk_error_new ("application data may not be interleaved with handshake data");
            DSK_ERROR_SET_TLS (*error, FATAL, UNEXPECTED_MESSAGE);
            return DSK_TLS_BASE_HANDSHAKE_MESSAGE_RESPONSE_FAILED;
          }
        dsk_buffer_append (&connection->incoming_plaintext,
                           length, data);
        return DSK_TLS_BASE_HANDSHAKE_MESSAGE_RESPONSE_OK;
      default:
        *error = dsk_error_new ("unexpected TLS record-content-type for server");
        return DSK_TLS_BASE_HANDSHAKE_MESSAGE_RESPONSE_FAILED;
    }
}

#define RECORD_HEADER_LENGTH 5

//
// Frame and optionally decrypt record until we get to
// a partial record or are suspended.
//
// Per RFC 8446, we should transmit TlsPlaintext records directly
// until encryption is engaged, then send TLSCiphertext records.
// These objects have the same structure:
//
//    struct {
//        ContentType type;
//        ProtocolVersion legacy_record_version;
//        uint16 length;
//        opaque fragment[TLSPlaintext.length];
//    } TLSPlaintext;
//
//    struct {
//        ContentType opaque_type = application_data; /* 23 */
//        ProtocolVersion legacy_record_version = 0x0303; /* TLS v1.2 */
//        uint16 length;
//        opaque encrypted_record[TLSCiphertext.length];
//    } TLSCiphertext;
//
//
// Once decrypted, encrypted_record encodes a TLSInnerPlaintext:
//
//    struct {
//        opaque content[TLSPlaintext.length];
//        ContentType type;
//        uint8 zeros[length_of_padding];
//    } TLSInnerPlaintext;
//
// Whether the underlying data is a TLSInnerPlaintext or TLSPlaintext
// will be "abstracted" away by the time we get
// to dsk_tls_base_connection_handle_record().
//


static bool
handle_complete_records (DskTlsBaseConnection *conn)
{
restart:
  if (conn->incoming_raw.size < RECORD_HEADER_LENGTH)
    return true;
  uint8_t outer_header[RECORD_HEADER_LENGTH];
  dsk_buffer_peek (&conn->incoming_raw, RECORD_HEADER_LENGTH, outer_header);
  DskTlsRecordContentType outer_rctype = outer_header[0];
  unsigned fragment_len = dsk_uint16be_parse (outer_header + 3);
  if (conn->incoming_raw.size < RECORD_HEADER_LENGTH + fragment_len)
    return true;                    // need more data
  dsk_buffer_discard (&conn->incoming_raw, RECORD_HEADER_LENGTH);
  size_t needed = outer_rctype == DSK_TLS_RECORD_CONTENT_TYPE_APPLICATION_DATA
                ? (fragment_len * 2) : fragment_len;
  if (conn->record_buffer_length < needed)
    {
      if (conn->record_buffer != NULL)
        dsk_free (conn->record_buffer);
      conn->record_buffer = dsk_malloc (needed);
      conn->record_buffer_length = needed;
    }
  uint8_t *fragment = conn->record_buffer;
  dsk_buffer_read (&conn->incoming_raw, fragment_len, fragment);
  unsigned record_length;
  uint8_t *record_data;
  DskTlsRecordContentType content_type;
  if (conn->cipher_suite_read_instance != NULL)
    {
      //
      // Compute associated_data; called additional_data in the spec.
      // See RFC 8446 Section 5.2:
      //     additional_data = TLSCiphertext.opaque_type ||
      //                       TLSCiphertext.legacy_record_version ||
      //                       TLSCiphertext.length
      // That is, it's outer_header.
      //

      //
      // the initialization-vector 'iv' is called in the nonce,
      // is computed: (Section 5.3)
      //     1.  The 64-bit record sequence number is encoded in network byte
      //         order and padded to the left with zeros to iv_length.
      //     2.  The padded sequence number is XORed with either the static
      //         client_write_iv or server_write_iv (depending on the role).
      //
      unsigned iv_len = conn->cipher_suite->iv_size;
      uint8_t *iv = alloca (iv_len);
      memcpy (iv, conn->read_iv, iv_len);
      uint8_t seq[8];
      dsk_uint64be_pack (conn->read_seqno, seq);
      conn->read_seqno += 1;

      for (unsigned i = 0; i < 8; i++)
        iv[iv_len - 8 + i] ^= seq[i];

      unsigned plaintext_len = fragment_len - conn->cipher_suite->auth_tag_size;
      if (!conn->cipher_suite->decrypt
               (conn->cipher_suite_read_instance,
                plaintext_len, fragment,
                RECORD_HEADER_LENGTH, outer_header, // associated_data
                iv,
                fragment + fragment_len,
                fragment + plaintext_len))
        {
          DskError *error = dsk_error_new ("AEAD decryption failure");
          dsk_tls_base_connection_fail (conn, error);
          return false;
        }

      //
      // Parse inner-message header
      //
      record_data = fragment + fragment_len;
      unsigned n_zeroes = 0;
      for (n_zeroes = 0; n_zeroes < plaintext_len; n_zeroes++)
        if (record_data[plaintext_len - 1 - n_zeroes] != 0)
          break;
      content_type = record_data[plaintext_len - 1 - n_zeroes];
      record_length = plaintext_len - n_zeroes - 1;
    }
  else
    {
      //
      // unprotected record
      //
      record_length = fragment_len;
      record_data = fragment;
      content_type = outer_rctype;
    }

  DskError *error = NULL;
  switch (dsk_tls_base_connection_handle_record
                (conn, content_type,
                 record_length, record_data,
                 &error))
    {
    case DSK_TLS_BASE_HANDSHAKE_MESSAGE_RESPONSE_OK:
      goto restart;

    case DSK_TLS_BASE_HANDSHAKE_MESSAGE_RESPONSE_FAILED:
      dsk_tls_base_connection_fail (conn, error);
      dsk_error_unref (error);
      return false;

    case DSK_TLS_BASE_HANDSHAKE_MESSAGE_RESPONSE_SUSPEND:
      assert (conn->handshake_message_state == DSK_TLS_BASE_HMS_SUSPENDED);
      return true;
    }
}

static bool
handle_underlying_readable (DskStream *underlying,
                            DskTlsBaseConnection *conn)
{
  //DskTlsRecordHeaderParseResult header_result;
  //DskTlsBaseConnectionClass *clss = DSK_TLS_BASE_CONNECTION_GET_CLASS (conn);

  assert (conn->underlying == underlying);

  // Perform raw read.
  size_t old_size = conn->incoming_raw.size;
  DskError *error = NULL;
  switch (dsk_stream_read_buffer (underlying, &conn->incoming_raw, &error))
    {
    case DSK_IO_RESULT_SUCCESS:
      if (conn->incoming_raw.size == old_size)
        return true;
      return true;
    case DSK_IO_RESULT_EOF:
      if (!conn->expecting_eof)
        {
          DskError *error = dsk_error_new ("unexpected EOF");
          dsk_tls_base_connection_fail (conn, error);
          dsk_error_unref (error);
          conn->read_trap = NULL;
          return false;
        }
      conn->read_trap = NULL;
      return false;
        
    case DSK_IO_RESULT_AGAIN:
      return true;
    case DSK_IO_RESULT_ERROR:
      dsk_tls_base_connection_fail (conn, error);
      dsk_error_unref (error);
      conn->read_trap = NULL;
      return false;
    }

  //
  // TODO: assert blocked (may just be a re-entrant invocation tho?;
  // or maybe we should never get here when state==SUSPENDED?)
  //
  if (conn->handshake_message_state == DSK_TLS_BASE_HMS_SUSPENDED)
    {
      return true;
    }

  if (!handle_complete_records (conn))
    {
      conn->read_trap = NULL;
      return false;
    }
  return true;
}


void
dsk_tls_base_connection_send_key_update (DskTlsBaseConnection *conn,
                                         bool request_key_update)
{
  // Update write-key.
  dsk_tls_update_key_inplace (conn->cipher_suite->hash_type,
                              conn->client_application_traffic_secret);
  conn->cipher_suite->init (conn->cipher_suite_write_instance,
                            true,            // for encryption
                            conn->client_application_traffic_secret);

  DskTlsHandshakeMessage msg;
  memset (&msg, 0, sizeof (msg));
  msg.type = DSK_TLS_HANDSHAKE_MESSAGE_TYPE_KEY_UPDATE;
  msg.is_outgoing = true;
  msg.key_update.update_requested = request_key_update;

  DskBuffer buf = DSK_BUFFER_INIT;
  dsk_tls_handshake_to_buffer (&msg, &buf);
  uint8_t *msg_data = alloca (buf.size);
  size_t msg_len = buf.size;
  dsk_tls_base_connection_send_handshake_msgdata (conn, msg_len, msg_data);
}

bool dsk_tls_base_connection_init_underlying (DskTlsBaseConnection *conn,
                                              DskStream *underlying,
                                              DskError **error)
{
  DSK_UNUSED (error);
  assert (conn->underlying == NULL);
  assert (underlying != NULL);
  conn->underlying = dsk_object_ref (underlying);
  conn->read_trap = dsk_hook_trap (&underlying->readable_hook,
                                   (DskHookFunc) handle_underlying_readable,
                                   conn,
                                   NULL);
  return true;
}
void
dsk_tls_base_handshake_get_transcript_hash (DskTlsBaseHandshake *hs_info,
                                            uint8_t *transcript_hash)
{
  const DskChecksumType *csum_type = hs_info->conn->cipher_suite->hash_type;

  // Create a copy of the transcript_instance.
  void *instance = alloca (csum_type->instance_size);
  memcpy (
    instance,
    hs_info->transcript_hash_instance,
    csum_type->instance_size
  );

  // Get the transcript_hash from the instance.
  csum_type->end (instance, transcript_hash);
}

DskTlsHandshakeMessage *
dsk_tls_base_handshake_create_outgoing_handshake
                         (DskTlsBaseHandshake       *hs_info,
                          DskTlsHandshakeMessageType   type,
                          unsigned                     max_extensions)

{
  DskTlsHandshakeMessage *hs = dsk_mem_pool_alloc0 (&hs_info->mem_pool, sizeof (DskTlsHandshakeMessage));
  hs->type = type;
  hs->is_outgoing = true;

  hs->n_extensions = 0;
  hs->max_extensions = max_extensions;
  hs->extensions = dsk_mem_pool_alloc (&hs_info->mem_pool, max_extensions * sizeof( DskTlsExtension *));

  return hs;
}

void dsk_tls_base_connection_unsuspend   (DskTlsBaseConnection *connection)
{
  assert (connection->handshake_message_state == DSK_TLS_BASE_HMS_SUSPENDED);
  DskTlsBaseConnectionClass *cclass = DSK_TLS_BASE_CONNECTION_GET_CLASS (connection);
  DskError *error = NULL;
  switch (cclass->handle_unsuspend (connection, &error))
    {
    case DSK_TLS_BASE_HANDSHAKE_MESSAGE_RESPONSE_OK:
      dsk_hook_trap_block (connection->read_trap);
      connection->handshake_message_state = DSK_TLS_BASE_HMS_INIT;
      if (!handle_complete_records(connection))
        dsk_warning ("handle_complete_records returned false");
      break;
    case DSK_TLS_BASE_HANDSHAKE_MESSAGE_RESPONSE_FAILED:
      connection->handshake_message_state = DSK_TLS_BASE_HMS_INIT;
      dsk_tls_base_connection_fail (connection, error);
      break;
    case DSK_TLS_BASE_HANDSHAKE_MESSAGE_RESPONSE_SUSPEND:
      break;
    }

  // undoes ref in handle_message_record().
  dsk_object_unref (connection);
}
