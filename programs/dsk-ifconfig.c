#include "../dsk.h"
#include <stdio.h>

int main(int argc, char **argv)
{
  DskNetworkInterfaceList *list;
  unsigned i;
  dsk_cmdline_init ("dump network interfaces", "this mimics ifconfig with no args", NULL, 0);
  dsk_cmdline_process_args (&argc, &argv);

  list = dsk_network_interface_list_peek_global ();
  for (i = 0; i < list->n_interfaces; i++)
    {
      printf ("%s:\n", list->interfaces[i].ifname);
      if (list->interfaces[i].is_loopback)
        printf ("\tloopback\n");
      if (list->interfaces[i].supports_multicast)
        printf ("\tsupports_multicast\n");
      if (list->interfaces[i].is_promiscuous)
        printf ("\tis_promiscuous\n");
      if (list->interfaces[i].is_up)
        {
          char *str = dsk_ip_address_to_string (&list->interfaces[i].address);
          printf ("\tup: %s\n", str);
          dsk_free (str);
        }
      if (list->interfaces[i].is_p2p)
        {
          char *str = dsk_ip_address_to_string (&list->interfaces[i].p2p_address);
          printf ("\tp2p: %s\n", str);
          dsk_free (str);
        }
      if (list->interfaces[i].supports_broadcast)
        {
          char *str = dsk_ip_address_to_string (&list->interfaces[i].broadcast);
          printf ("\tbroadcast: %s\n", str);
          dsk_free (str);
        }
      if (list->interfaces[i].has_hw_address)
        {
          char *str = dsk_ethernet_address_to_string (&list->interfaces[i].hw_address);
          printf ("\thardware: %s\n", str);
          dsk_free (str);
        }
      if (i + 1 < list->n_interfaces)
        printf ("\n");
    }
  if (i == 0)
    printf ("No interfaces found.\n");
  return 0;
}

