#ifndef DSK_H__INCLUDED
#define DSK_H__INCLUDED

#define DSK_ASM_NONE        0
#define DSK_ASM_AMD64       1

#include "dsk-config.h"

/* Univerisally needed functions and typedefs (like the int## types) */
#include "dsk-common.h"

/* Parsing and formatting ints in endian-specific ways. */
#include "dsk-endian.h"

/* Break down and build up floating point numbers
 * from their constituent parts. */
#include "dsk-floating-point.h"

/* File-descriptors and ways to manipulate them. */
#include "dsk-fd.h"

/* DskDispatch is an event-loop */
#include "dsk-dispatch.h"

/* Shorthands for using the default DskDispatch. */
#include "dsk-main.h"

/* DskMemPoolFixed is for a pool of fixed-length allocations that can
   be allocated and freed (which means returning them to a free-list
   that will be returned at next alloc.)

   DskMemPool is a alloc-only pool that can be freed all-at-once,
   which is a typical situation at the end of a function impl. */
#include "dsk-mem-pool.h"

/* Base-class for the object system. */
#include "dsk-object.h"

/* Base-class for the error system. */
#include "dsk-error.h"

/* DskHook is a lightweight notification system. */
#include "dsk-hook.h"

/* DskBuffer: Binary-Data queue. */
#include "dsk-buffer.h"

/* DskFlatBuffer: Binary-Data queue implemented with a single buffer. */
#include "dsk-flat-buffer.h"

/* Input/output stream halves. */
#include "dsk-stream.h"
#include "dsk-sync-filter.h"

/* Input from memory, and output to memory. */
#include "dsk-memory.h"

/* DskIpAddress: IPv4 or IPv6 address. */
#include "dsk-ip-address.h"

/* DskEthernetAddress: a MAC address. */
#include "dsk-ethernet-address.h"

/* Simple DNS Client. */
#include "dsk-dns-client.h"

/* Our limited threading support */
#include "dsk-thread-pool.h"

#include "dsk-stream-listener.h"

#include "dsk-network-interface-list.h"

#include "dsk-dns-protocol.h"

#include "dsk-client-stream.h"
#include "dsk-fd-stream.h"
#include "dsk-socket-stream-listener.h"

#include "dsk-udp-socket.h"

#include "dsk-print.h"

#include "dsk-codegen.h"

#include "dsk-date.h"

#include "dsk-file-util.h"
#include "dsk-path.h"
#include "dsk-dir.h"

#include "dsk-pattern.h"

#include "dsk-url.h"

#include "dsk-rand.h"

#include "dsk-cgi.h"
#include "dsk-mime-multipart.h"
#include "dsk-mime.h"

#include "dsk-xml.h"
#include "dsk-xml-parser.h"
#include "dsk-xml-binding.h"

#include "json/dsk-json.h"
#include "json/dsk-json-callback-parser.h"

#include "dsk-ctoken.h"

#include "dsk-zlib.h"
#include "dsk-bz2lib.h"
#include "dsk-octet-filter-misc.h"
#include "dsk-base64.h"

#include "dsk-unicode.h"
#include "dsk-ascii.h"
#include "dsk-utf8.h"
#include "dsk-utf16.h"
#include "dsk-strv.h"
#include "dsk-html-entities.h"

#include "dsk-cmdline.h"

#include "dsk-daemon.h"

#include "dsk-checksum.h"

#include "dsk-cleanup.h"

#include "dsk-logger.h"

/* --- crypto --- */
#include "tls/dsk-aes.h"
#include "tls/dsk-chacha20.h"
#include "tls/dsk-block-cipher.h"
#include "tls/dsk-aead-gcm.h"
#include "tls/dsk-aead-ccm.h"
#include "tls/dsk-curve25519.h"
#include "tls/dsk-curve448.h"
#include "tls/dsk-hmac.h"
#include "tls/dsk-hkdf.h"
#include "tls/dsk-tls-object-id.h"
#include "tls/dsk-tls-object-ids.h"
#include "tls/dsk-asn1.h"
#include "tls/dsk-tls-cryptorandom.h"
#include "tls/dsk-tls-enums.h"
#include "tls/dsk-tls-error.h"
#include "tls/dsk-tls-key-pair.h"
#include "tls/dsk-tls-x509.h"
#include "tls/dsk-tls-bignum.h"
#include "tls/dsk-tls-ffdhe.h"
#include "tls/dsk-tls-cipher-suite.h"
#include "tls/dsk-tls-key-schedule.h"
#include "tls/dsk-tls-protocol.h"
#include "tls/dsk-tls-key-share.h"
#include "tls/dsk-tls-signature.h"
#include "tls/dsk-tls-ec-prime.h"
#include "tls/dsk-tls-base-connection.h"
#include "tls/dsk-tls-client-connection.h"
#include "tls/dsk-tls-server-connection.h"
#include "tls/dsk-tls-oid-mappings.h"

// TODO: remove!
#include "dsk-ts0.h"


#undef _dsk_inline_assert

#endif
