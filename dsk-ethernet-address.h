
typedef struct _DskEthernetAddress DskEthernetAddress;
struct _DskEthernetAddress
{
  uint8_t address[6];
};

char *dsk_ethernet_address_to_string (const DskEthernetAddress *);
