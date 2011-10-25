#include "dsk.h"

char *dsk_ethernet_address_to_string (const DskEthernetAddress *address)
{
  return dsk_strdup_printf ("%02x:%02x:%02x:%02x:%02x:%02x",
                            address->address[0],
                            address->address[1],
                            address->address[2],
                            address->address[3],
                            address->address[4],
                            address->address[5]);
}

