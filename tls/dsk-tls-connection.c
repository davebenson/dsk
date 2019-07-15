#include "../dsk.h"
#include "dsk-tls-private.h"
#include <stdlib.h>
#include <string.h>

//
// Supported stuff.
//
static DskTlsSignatureScheme global_sig_schemes[] = {
  DSK_TLS_SIGNATURE_SCHEME_RSA_PKCS1_SHA256,
  DSK_TLS_SIGNATURE_SCHEME_RSA_PKCS1_SHA384,
  DSK_TLS_SIGNATURE_SCHEME_RSA_PKCS1_SHA512,
  DSK_TLS_SIGNATURE_SCHEME_ECDSA_SECP256R1_SHA256,
  DSK_TLS_SIGNATURE_SCHEME_ECDSA_SECP384R1_SHA384,
  DSK_TLS_SIGNATURE_SCHEME_ECDSA_SECP521R1_SHA512,
  DSK_TLS_SIGNATURE_SCHEME_RSA_PSS_RSAE_SHA256,
  DSK_TLS_SIGNATURE_SCHEME_RSA_PSS_RSAE_SHA384,
  DSK_TLS_SIGNATURE_SCHEME_RSA_PSS_RSAE_SHA512,
  DSK_TLS_SIGNATURE_SCHEME_ED25519,
  DSK_TLS_SIGNATURE_SCHEME_ED448,
  DSK_TLS_SIGNATURE_SCHEME_RSASSA_PSS_PSS_SHA256,
  DSK_TLS_SIGNATURE_SCHEME_RSASSA_PSS_PSS_SHA384,
  DSK_TLS_SIGNATURE_SCHEME_RSASSA_PSS_PSS_SHA512
};

//
// Allocate memory and objects from the handshke-pool.
//
#define HS_ALLOC(hs_info, type) \
  ((type *) dsk_mem_pool_alloc (&((hs_info)->mem_pool), sizeof(type)))
#define HS_ALLOC0(hs_info, type) \
  ((type *) dsk_mem_pool_alloc (&((hs_info)->mem_pool), sizeof(type)))
#define HS_ALLOC_ARRAY(hs_info, count, type) \
  ((type *) dsk_mem_pool_alloc (&((hs_info)->mem_pool), sizeof(type) * (count)))
#define HS_ALLOC_ARRAY0(hs_info, count, type) \
  ((type *) dsk_mem_pool_alloc (&((hs_info)->mem_pool), sizeof(type) * (count)))

static bool
handle_certificate_verify  (DskTlsConnection *conn,
                            DskTlsHandshakeMessage  *shake,
                            DskError        **error)
{
  if (conn->state != ...)
    {
      ...
    }
  ...
}
static bool
handle_finished            (DskTlsConnection *conn,
                            DskTlsHandshakeMessage  *shake,
                            DskError        **error)
{
  if (conn->state != ...)
    {
      ...
    }
  ...
}


static bool
handle_handshake_message (DskTlsConnection *conn,
                          DskTlsHandshakeMessage  *shake,
                          DskError        **error)
{
  switch (shake->type)
    {
    case DSK_TLS_HANDSHAKE_MESSAGE_TYPE_CLIENT_HELLO:
      return handle_client_hello (conn, shake, error);
    case DSK_TLS_HANDSHAKE_MESSAGE_TYPE_SERVER_HELLO:
      if (is_retry_request (shake))
        return handle_hello_retry_request (conn, shake, error);
      else
        return handle_server_hello (conn, shake, error);
    case DSK_TLS_HANDSHAKE_MESSAGE_TYPE_NEW_SESSION_TICKET:
      return handle_new_session_ticket (conn, shake, error);
    case DSK_TLS_HANDSHAKE_MESSAGE_TYPE_END_OF_EARLY_DATA:
      return handle_end_of_early_data (conn, shake, error);
    case DSK_TLS_HANDSHAKE_MESSAGE_TYPE_ENCRYPTED_EXTENSIONS:
      return handle_encrypted_extensions (conn, shake, error);
    case DSK_TLS_HANDSHAKE_MESSAGE_TYPE_CERTIFICATE:
      return handle_certificate (conn, shake, error);
    case DSK_TLS_HANDSHAKE_MESSAGE_TYPE_CERTIFICATE_REQUEST:
      return handle_certificate_request (conn, shake, error);
    case DSK_TLS_HANDSHAKE_MESSAGE_TYPE_CERTIFICATE_VERIFY:
      return handle_certificate_verify (conn, shake, error);
    case DSK_TLS_HANDSHAKE_MESSAGE_TYPE_FINISHED:
      return handle_finished (conn, shake, error);
    case DSK_TLS_HANDSHAKE_MESSAGE_TYPE_KEY_UPDATE:
      *error = ...
      return handle_certificate_key_update (conn, shake, error);
    default:
      assert(0);
    }
  return true;
}

DskTlsConnection *
dsk_tls_connection_client_new (DskTlsClientContext *context,
                               DskStream           *underlying,
                               DskError           **error)
{
  DskTlsConnection *conn = dsk_object_new (&dsk_tls_connection_class);
  conn->underlying = dsk_object_ref (underlying);
  conn->context = dsk_object_ref (context);
  conn->handshake_info = dsk_new0 (DskTlsHandshakeNegotiation);

  conn->state = DSK_TLS_CONNECTION_CLIENT_WAIT_SERVER_HELLO;
  if (!send_client_hello_flight (conn->handshake_info,
                                 error))
    {
      ...
    }

  dsk_hook_trap (...);  read
  dsk_hook_trap (..);   write
  return conn;
}

DskTlsConnection *
dsk_tls_connection_server_new (DskTlsServerContext *context,
                               DskStream           *underlying,
                               DskError           **error)
{
  DskTlsConnection *conn = dsk_object_new (&dsk_tls_connection_class);
  conn->underlying = dsk_object_ref (underlying);
  conn->context = dsk_object_ref (context);
  conn->handshake_info = dsk_new0 (DskTlsHandshakeNegotiation);
  conn->state = DSK_TLS_CONNECTION_SERVER_START;
  dsk_hook_trap (...);  read
  return conn;
}
