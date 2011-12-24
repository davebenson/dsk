#include <stdio.h>
#include <stdlib.h>
#include "../dsk.h"

/* configuration */
static dsk_boolean use_ipv6 = DSK_FALSE;
static dsk_boolean no_links = DSK_FALSE;
static char *nameserver = NULL;
static dsk_boolean verbose = DSK_FALSE;
static unsigned max_concurrent = 1;
static dsk_boolean fatal_errors = DSK_FALSE;

/* state */
static unsigned n_running = 0;
static dsk_boolean exit_status = 0;

static void
handle_dns_result  (DskDnsLookupResult *result,
                    void               *callback_data)
{
  const char *name = callback_data;
  char *str;
  switch (result->type)
    {
    case DSK_DNS_LOOKUP_RESULT_FOUND:
      str = dsk_ip_address_to_string (result->addr);
      printf ("%s: %s\n", name, str);
      dsk_free (str);
      break;
    case DSK_DNS_LOOKUP_RESULT_NOT_FOUND:
      printf ("%s: not found\n", name);
      break;
    case DSK_DNS_LOOKUP_RESULT_TIMEOUT:
      printf ("%s: timeout\n", name);
      break;
    case DSK_DNS_LOOKUP_RESULT_BAD_RESPONSE:
      printf ("%s: bad response: %s\n", name, result->message);
      break;
    default:
      dsk_assert_not_reached ();
    }
  if (result->type != DSK_DNS_LOOKUP_RESULT_FOUND)
    {
      exit_status = 1;
      if (fatal_errors)
        exit (1);
    }
  --n_running;
}

int main(int argc, char **argv)
{
  DskDnsConfigFlags cfg_flags = DSK_DNS_CONFIG_FLAGS_INIT;
  dsk_boolean no_searchpath = DSK_FALSE;
  int i;
  dsk_cmdline_init ("perform DNS lookups",
                    "Perform DNS lookups with Dsk DNS client.\n",
                    "HOSTNAMES...",
                    0);
  dsk_cmdline_permit_extra_arguments (DSK_TRUE);
  dsk_cmdline_add_boolean ("ipv6", "Lookup names in the IPv6 namespace", NULL, 0, &use_ipv6);
  dsk_cmdline_add_boolean ("ipv4", "Lookup names in the IPv4 namespace", NULL, DSK_CMDLINE_REVERSED, &use_ipv6);
  dsk_cmdline_add_boolean ("cname", "Return CNAME or POINTER records if they arise", NULL, 0, &no_links);
  dsk_cmdline_add_string ("nameserver", "Specify the nameserver to use", "IP", 0, &nameserver);
  dsk_cmdline_add_boolean ("verbose", "Print extra messages", NULL, 0, &verbose);
  dsk_cmdline_add_boolean ("no-searchpath", "Do not use /etc/resolv.conf's searchpath", NULL, 0, &no_searchpath);
  dsk_cmdline_add_boolean ("fatal-errors", "Exit on first error", NULL, 0, &fatal_errors);
  dsk_cmdline_process_args (&argc, &argv);

  if (no_searchpath)
    cfg_flags &= ~DSK_DNS_CONFIG_USE_RESOLV_CONF_SEARCHPATH;

  if (nameserver == NULL)
    {
      /* just use default config */
    }
  else
    {
      DskIpAddress addr;
      cfg_flags &= ~DSK_DNS_CONFIG_USE_RESOLV_CONF_NS;
      if (!dsk_ip_address_parse_numeric (nameserver, &addr))
        dsk_error ("error parsing nameserver address (must be numeric)");
      dsk_dns_client_add_nameserver (&addr);
    }
  dsk_dns_client_config (cfg_flags);
  if (verbose)
    {
      dsk_dns_config_dump ();
    }

  if (argc == 1)
    dsk_error ("expected name to resolve");
  for (i = 1; i < argc; i++)
    {
      n_running++;
      dsk_dns_lookup (argv[i],
                      use_ipv6,
                      handle_dns_result,
                      argv[i]);
      while (n_running >= max_concurrent)
        dsk_main_run_once ();
    }
  dsk_dispatch_destroy_default ();
  return exit_status;
}
