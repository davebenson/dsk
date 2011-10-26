#include <stdio.h>
#include "../dsk.h"

static const char *hostname;
static dsk_boolean is_ipv6 = DSK_FALSE;
static dsk_boolean cname = DSK_FALSE;
static dsk_boolean use_searchpath = DSK_TRUE;

static void
handle_cache_entry (DskDnsCacheEntry *entry,
                    void             *callback_data)
{
  int exit_status = 0;
  DSK_UNUSED (callback_data);
  switch (entry->type)
    {
    case DSK_DNS_CACHE_ENTRY_IN_PROGRESS:
      dsk_assert_not_reached ();
      break;
    case DSK_DNS_CACHE_ENTRY_ERROR:
      printf ("error resolving %s: %s\n",
              entry->name,
              entry->info.error.error->message);
      exit_status = 1;
      break;

    case DSK_DNS_CACHE_ENTRY_NEGATIVE:
      printf ("%s: NOT FOUND\n", entry->name);
      exit_status = 1;
      break;

    case DSK_DNS_CACHE_ENTRY_CNAME:
      printf ("%s -> %s", entry->name, entry->info.cname);
      break;

    case DSK_DNS_CACHE_ENTRY_ADDR:
      {
        unsigned i;
        for (i = 0; i < entry->info.addr.n; i++)
          {
            DskIpAddress *addr = entry->info.addr.addresses + i;
            char *str = dsk_ip_address_to_string (addr);
            printf ("%s\n", str);
            dsk_free (str);
          }
      }
      break;
    }
  dsk_main_exit (exit_status);
}

int main(int argc, char **argv)
{
  dsk_cmdline_init ("lookup a name in DNS",
                    "Resolve a hostname using DNS",
                    "HOSTNAME",
                    0);
  dsk_cmdline_add_boolean ("no-searchpath",
                           "suppress use of searchpath",
                           NULL,
                           DSK_CMDLINE_REVERSED,
                           &use_searchpath);
  dsk_cmdline_add_boolean ("ipv6",
                           "try IPv6 resolution",
                           NULL,
                           0,
                           &is_ipv6);
  dsk_cmdline_add_boolean ("cname",
                           "return CNAME information instead of an address",
                           NULL,
                           0,
                           &cname);
  dsk_cmdline_permit_extra_arguments (DSK_TRUE);
  dsk_cmdline_process_args (&argc, &argv);
  if (argc > 2)
    dsk_die ("too many arguments");
  else if (argc < 2)
    dsk_die ("missing HOSTNAME");
  hostname = argv[1];
  dsk_dns_lookup_cache_entry (hostname, is_ipv6,
                              (cname ? 0 : DSK_DNS_LOOKUP_FOLLOW_CNAMES)
                              | (use_searchpath ? DSK_DNS_LOOKUP_USE_SEARCHPATH : 0),
                              handle_cache_entry, NULL);
  return dsk_main_run ();
}
