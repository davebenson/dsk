#ifndef DSK_H__INCLUDED
#define DSK_H__INCLUDED

/* Univerisally needed functions and typedefs (like the int## types) */
#include "dsk-common.h"

/* Parsing and formatting ints in endian-specific ways. */
#include "dsk-endian.h"

/* File-descriptors and ways to manipulate them. */
#include "dsk-fd.h"

/* DskDispatch is an event-loop */
#include "dsk-dispatch.h"

/* Shorthands for using the default DskDispatch. */
#include "dsk-main.h"

/* Base-class for the object system. */
#include "dsk-object.h"

/* Base-class for the error system. */
#include "dsk-error.h"

/* DskMemPoolFixed is for a pool of fixed-length allocations that can
   be allocated and freed (which means returning them to a free-list
   that will be returned at next alloc.)

   DskMemPool is a alloc-only pool that can be freed all-at-once,
   which is a typical situation at the end of a function impl. */
#include "dsk-mem-pool.h"

/* DskHook is a lightweight notification system. */
#include "dsk-hook.h"

/* DskBuffer: Binary-Data queue. */
#include "dsk-buffer.h"

/* Input/output stream halves. */
#include "dsk-octet-io.h"

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

#include "dsk-octet-listener.h"

#include "dsk-network-interface-list.h"

#include "dsk-dns-protocol.h"

#include "dsk-client-stream.h"
#include "dsk-octet-fd.h"
#include "dsk-octet-listener-socket.h"

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

#include "dsk-http-protocol.h"
#include "dsk-cgi.h"
#include "dsk-websocket.h"
#include "dsk-http-client-auth.h"
#include "dsk-http-client-stream.h"
#include "dsk-http-client.h"
#include "dsk-http-server-stream.h"
#include "dsk-http-server-stream-proxy.h"
#include "dsk-http-server.h"
#include "dsk-mime-multipart.h"
#include "dsk-mime.h"

#include "dsk-xml.h"
#include "dsk-xml-parser.h"
#include "dsk-xml-binding.h"
#include "dsk-xmlrpc.h"

#include "dsk-json.h"

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

#include "dsk-cmdline.h"

#include "dsk-daemon.h"

#include "dsk-checksum.h"

#include "dsk-cleanup.h"

#include "dsk-config.h"
#include "dsk-logger.h"

#include "dsk-ssl.h"
#include "dsk-ssl-listener.h"

#include "dsk-https.h"

#include "dsk-table.h"
#include "dsk-table-file.h"

#ifdef DSK_INCLUDE_TS0
#include "dsk-ts0.h"
#endif


#undef _dsk_inline_assert

#endif
