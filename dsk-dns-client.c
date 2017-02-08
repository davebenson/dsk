#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "dsk.h"
#include "dsk-rbtree-macros.h"
#include "dsk-list-macros.h"

#define MAX_CNAMES                 16
#define TIME_WAIT_TO_RECYCLE_ID    60
#define DSK_DNS_MAX_NAMELEN        1024         /* TODO: look it up! */


#define NO_EXPIRE_TIME  ((unsigned)(-1))

static unsigned retry_schedule[] = { 1000, 2000, 3000, 4000 };

typedef struct _LookupData LookupData;
struct _LookupData
{
  DskDnsLookupFunc callback;
  void *callback_data;
};


typedef struct _Waiter Waiter;
struct _Waiter
{
  DskDnsCacheEntryFunc func;
  void *data;
  Waiter *next;
};

struct _DskDnsCacheEntryJob
{
  DskDnsCacheEntry *owner;

  /* users waiting on this job to finish */
  Waiter *waiters;

  unsigned ns_index;
  uint8_t attempt;
  uint8_t waiting_to_send;
  DskDispatchTimer *timer;

  /* the question, stored as binary */
  unsigned message_len;
  uint8_t *message;

  DskDnsCacheEntryJob *prev_waiting, *next_waiting;
};



typedef struct _NameserverInfo NameserverInfo;
struct _NameserverInfo
{
  DskIpAddress address;
  unsigned n_requests;
  unsigned n_responses;
};
static void
nameserver_info_init (NameserverInfo *init,
                      const DskIpAddress *addr)
{
  init->address = *addr;
  init->n_requests = init->n_responses = 0;
}


/* TODO: plugable random number generator.  or mersenne twister import */
static unsigned
random_int (unsigned n)
{
  dsk_assert (n > 0);
  if (n == 1)
    return 0;
  else
    return rand () / (RAND_MAX / n + 1);
}

static void
handle_cache_entry_lookup (DskDnsCacheEntry *entry,
                           void             *data)
{
  LookupData *lookup_data = data;
  DskDnsLookupResult result;
  switch (entry->type)
    {
    case DSK_DNS_CACHE_ENTRY_IN_PROGRESS:
      dsk_assert_not_reached ();
    case DSK_DNS_CACHE_ENTRY_ERROR:
      result.type = DSK_DNS_LOOKUP_RESULT_BAD_RESPONSE;
      result.addr = NULL;
      /* CONSIDER: adding cname chain somewhere */
      result.message = entry->info.error.error->message;
      lookup_data->callback (&result, lookup_data->callback_data);
      dsk_free (lookup_data);
      return;

    case DSK_DNS_CACHE_ENTRY_NEGATIVE:
      result.type = DSK_DNS_LOOKUP_RESULT_NOT_FOUND;
      result.addr = NULL;
      result.message = "not found";
      /* CONSIDER: adding cname chain somewhere */
      lookup_data->callback (&result, lookup_data->callback_data);
      dsk_free (lookup_data);
      return;
    case DSK_DNS_CACHE_ENTRY_CNAME:
      dsk_assert_not_reached ();
    case DSK_DNS_CACHE_ENTRY_ADDR:
      result.type = DSK_DNS_LOOKUP_RESULT_FOUND;
      result.addr = entry->info.addr.addresses
                  + random_int (entry->info.addr.n);
      result.message = NULL;
      lookup_data->callback (&result, lookup_data->callback_data);
      dsk_free (lookup_data);
      return;
    }
}

void    dsk_dns_lookup (const char       *name,
                        dsk_boolean       is_ipv6,
                        DskDnsLookupFunc  callback,
                        void             *callback_data)
{
  LookupData *lookup_data = DSK_NEW (LookupData);
  lookup_data->callback = callback;
  lookup_data->callback_data = callback_data;
  dsk_dns_lookup_cache_entry (name, is_ipv6,
                              DSK_DNS_LOOKUP_USE_SEARCHPATH |
                              DSK_DNS_LOOKUP_FOLLOW_CNAMES,
                              handle_cache_entry_lookup, lookup_data);
} 

/* --- globals --- */
static DskUdpSocket *dns_udp_socket = NULL;
static DskHookTrap *dns_udp_socket_trap = NULL;
static DskDnsCacheEntry *dns_cache = NULL;
static DskDnsCacheEntry *expiration_tree = NULL;
static unsigned next_nameserver_index = 0;
static DskDnsCacheEntryJob *first_waiting_to_send = NULL;
static DskDnsCacheEntryJob *last_waiting_to_send = NULL;

/* --- configuration --- */
static unsigned n_resolv_conf_ns = 0;
static NameserverInfo *resolv_conf_ns = NULL;
static unsigned n_resolv_conf_search_paths = 0;
static char **resolv_conf_search_paths = NULL;
static unsigned max_resolv_conf_searchpath_len = 0;
static DskDnsCacheEntry *etc_hosts_tree = NULL;
static DskDnsConfigFlags config_flags = DSK_DNS_CONFIG_FLAGS_INIT;
static dsk_boolean dns_initialized = DSK_FALSE;

#define CACHE_ENTRY_NAME_IS_RED(n)  n->name_type_is_red
#define CACHE_ENTRY_NAME_SET_IS_RED(n,v)  n->name_type_is_red=v
#define COMPARE_DNS_CACHE_ENTRY_BY_NAME_TYPE(a,b,rv) \
  rv = strcmp ((a)->name, (b)->name); \
  if (rv == 0) \
    { \
      if (a->is_ipv6 && !b->is_ipv6) rv = -1; \
      else if (!a->is_ipv6 && b->is_ipv6) rv = -1; \
    }
#define GET_ETC_HOSTS_TREE() \
  etc_hosts_tree, DskDnsCacheEntry *, CACHE_ENTRY_NAME_IS_RED, \
  CACHE_ENTRY_NAME_SET_IS_RED, \
  name_type_parent, name_type_left, name_type_right, \
  COMPARE_DNS_CACHE_ENTRY_BY_NAME_TYPE
#define GET_CACHE_BY_NAME_TREE() \
  dns_cache, DskDnsCacheEntry *, CACHE_ENTRY_NAME_IS_RED, \
  CACHE_ENTRY_NAME_SET_IS_RED, \
  name_type_parent, name_type_left, name_type_right, \
  COMPARE_DNS_CACHE_ENTRY_BY_NAME_TYPE

#define CACHE_ENTRY_EXPIRE_IS_RED(n)  n->name_type_is_red
#define CACHE_ENTRY_EXPIRE_SET_IS_RED(n,v)  n->name_type_is_red=v
#define COMPARE_DNS_CACHE_ENTRY_BY_EXPIRE(a,b,rv) \
  rv = a->expire_time < b->expire_time ? -1 \
     : a->expire_time > b->expire_time ? 1 \
     : a < b ? -1 \
     : a > b ? 1 \
     : 0
#define GET_EXPIRATION_TREE() \
  expiration_tree, DskDnsCacheEntry *, CACHE_ENTRY_EXPIRE_IS_RED, \
  CACHE_ENTRY_EXPIRE_SET_IS_RED, \
  name_type_parent, name_type_left, name_type_right, \
  COMPARE_DNS_CACHE_ENTRY_BY_EXPIRE
  
/* --- configuration functions --- */
void dsk_dns_client_config (DskDnsConfigFlags flags)
{
  config_flags = flags;
}
void dsk_dns_client_add_nameserver (DskIpAddress *addr)
{
  unsigned N = n_resolv_conf_ns;
  resolv_conf_ns = DSK_RENEW (NameserverInfo, resolv_conf_ns, N);
  n_resolv_conf_ns = N + 1;
  resolv_conf_ns[N].address = *addr;
  resolv_conf_ns[N].n_requests = 0;
  resolv_conf_ns[N].n_responses = 0;
}
void dsk_dns_config_dump (void)
{
  unsigned i;
  for (i = 0; i < n_resolv_conf_ns; i++)
    {
      char *str = dsk_ip_address_to_string (&resolv_conf_ns[i].address);
      printf ("nameserver %u: %s\n", i, str);
      dsk_free (str);
    }
  for (i = 0; i < n_resolv_conf_search_paths; i++)
    printf ("searchpath %u: %s\n", i, resolv_conf_search_paths[i]);
}

/* --- handling system files (resolv.conf and hosts) --- */
static void
job_notify_waiters_and_free (DskDnsCacheEntryJob *job)
{
  /* notify waiters */
  while (job->waiters)
    {
      Waiter *w = job->waiters;
      job->waiters = w->next;

      w->func (job->owner, w->data);
      dsk_free (w);
    }

  /* free job */
  dsk_free (job->message);
  dsk_free (job);
}

#define MAX_ANSWER_RR  256
static void
handle_message (DskDnsMessage *message)
{
  unsigned n_answer = message->n_answer_rr;
  unsigned i, j;
  if (n_answer > MAX_ANSWER_RR)
    n_answer = MAX_ANSWER_RR;
  for (i = 0; i < message->n_questions; i++)
    {
      DskDnsQuestion *q = message->questions + i;
      DskDnsResourceRecord *a = message->answer_rr;
      char answers_that_match[MAX_ANSWER_RR];
      unsigned n_matches = 0;
      int cname_index = -1;
      dsk_boolean is_ipv6;
      DskDnsCacheEntry dummy, *entry;
      DskDnsCacheEntryJob *job;
      if (q->query_type == DSK_DNS_RR_HOST_ADDRESS)
        is_ipv6 = DSK_FALSE;
      else if (q->query_type == DSK_DNS_RR_HOST_ADDRESS_IPV6)
        is_ipv6 = DSK_TRUE;
      else
        {
          dsk_warning ("DNS system: received packet with unexpected question type %u", q->query_type);
          continue;
        }

      dummy.name = (char *) q->name;
      dummy.is_ipv6 = is_ipv6;
      DSK_RBTREE_LOOKUP (GET_CACHE_BY_NAME_TREE (), &dummy, entry);
      if (entry == NULL)
        {
          dsk_warning ("DNS system: unexpected packet pertaining to %s",
                       q->name);
          continue;
        }
      if (entry->type != DSK_DNS_CACHE_ENTRY_IN_PROGRESS)
        continue;
      job = entry->info.in_progress;
      memset (answers_that_match, 0, n_answer);
      n_matches = 0;
      for (j = 0; j < n_answer; j++, a++)
        if (strcmp (q->name, a->owner) == 0)
          {
            if ((a->type == DSK_DNS_RR_HOST_ADDRESS && !is_ipv6)
             || (a->type == DSK_DNS_RR_HOST_ADDRESS_IPV6 && is_ipv6))
              {
                n_matches++;
                answers_that_match[j] = DSK_TRUE;
              }
            else if (a->type == DSK_DNS_RR_CANONICAL_NAME)
              {
                cname_index = j;
              }
            else
              {
                /* not expected, but we ignore it */
              }
          }
      if (cname_index >= 0)
        {
          /* cname result */
          entry->type = DSK_DNS_CACHE_ENTRY_CNAME;
          entry->info.cname = dsk_strdup (message->answer_rr[cname_index].rdata.domain_name);
        }
      else if (n_matches == 0)
        {
          /* negative result */
          entry->type = DSK_DNS_CACHE_ENTRY_NEGATIVE;
        }
      else
        {
          /* positive result (an actual address) */
          DskIpAddress *addrs = DSK_NEW_ARRAY (n_matches, DskIpAddress);
          unsigned ai = 0;
          entry->type = DSK_DNS_CACHE_ENTRY_ADDR;
          entry->info.addr.n = n_matches;
          entry->info.addr.addresses = addrs;
          for (j = 0; j < n_answer; j++)
            if (answers_that_match[j])
              {
                addrs[ai].type = is_ipv6 ? DSK_IP_ADDRESS_IPV6 : DSK_IP_ADDRESS_IPV4;

                /* NOTE: we assume aaaa.address and a.ip_address
                   alias!  This is fine though- we comment appropriately in
                   dsk-dns-protocol.h */
                memcpy (addrs[ai].address,
                        message->answer_rr[j].rdata.aaaa.address,
                        is_ipv6 ? 16 : 4);
                ai++;
              }
        }

      /* destroy job */
      dsk_dispatch_remove_timer (job->timer);
      job_notify_waiters_and_free (job);
    }
}

static dsk_boolean
handle_dns_udp_socket_readable (void *socket,
                                void *user_data)
{
  DSK_UNUSED (user_data);
  for (;;)
    {
      DskIpAddress addr;
      unsigned port;
      unsigned length;
      uint8_t *data;
      DskError *error = NULL;
      DskDnsMessage *message;
next_packet:
      switch (dsk_udp_socket_receive (socket,
                                      &addr, &port, &length, &data,
                                      &error))
        {
        case DSK_IO_RESULT_SUCCESS:
          /* verify address matches id */
          {
            unsigned id;
            if (length < 12)
              {
                dsk_warning ("DNS system: message far too short");
                dsk_free (data);
                goto next_packet;
              }
            id = (data[0] << 8) | (data[1]);
            if (id >= n_resolv_conf_ns)
              {
                dsk_warning ("DNS system: received packet with bad ID");
                dsk_free (data);
                goto next_packet;
              }

            /* ip-based security :/ */
            if (!dsk_ip_addresses_equal (&addr, &resolv_conf_ns[id].address))
              {
                dsk_warning ("DNS system: packet source address incorrect");
                dsk_free (data);
                goto next_packet;
              }
            message = dsk_dns_message_parse (length, data, &error);
            if (message == NULL)
              {
                dsk_warning ("DNS system: error parsing message: %s",
                             error->message);
                dsk_free (data);
                dsk_error_unref (error);
                error = NULL;
                goto next_packet;
              }
            handle_message (message);
            dsk_free (message);
            dsk_free (data);
            break;
          }
        case DSK_IO_RESULT_AGAIN:
        case DSK_IO_RESULT_EOF:      /* shouldn't happen, try like AGAIN */
          return DSK_TRUE;
        case DSK_IO_RESULT_ERROR:
          dsk_warning ("DNS: receiving from UDP socket: %s", error->message);
          dsk_error_unref (error);
          return DSK_TRUE;              /* um, could induce busy loop */
        }
    }
}

static dsk_boolean
dsk_dns_try_init (DskError **error)
{
  char buf[2048];
  FILE *fp;
  DskDnsCacheEntry *conflict;
  unsigned lineno;

  dsk_assert (!dns_initialized);

  /* parse /etc/hosts */
  fp = fopen ("/etc/hosts", "r");
  if (fp == NULL)
    {
      dsk_set_error (error, "error opening %s: %s",
                     "/etc/hosts", strerror (errno));
      return DSK_FALSE;
    }
  lineno = 0;
  while (fgets (buf, sizeof (buf), fp) != NULL)
    {
      char *at = buf;
      const char *ip;
      const char *name;
      DskIpAddress addr;
      DskDnsCacheEntry *host_entry;
      ++lineno;
      DSK_ASCII_SKIP_SPACE (at);
      if (*at == '#')
        continue;
      if (*at == 0)
        continue;
      ip = at;
      DSK_ASCII_SKIP_NONSPACE (at);
      *at++ = 0;
      DSK_ASCII_SKIP_SPACE (at);
      name = at;
      DSK_ASCII_SKIP_NONSPACE (at);
      *at = 0;
      if (*ip == 0 || *name == 0)
        {
          dsk_warning ("parsing /etc/hosts line %u: expected ip/name pair",
                       lineno);
          continue;
        }
      if (!dsk_ip_address_parse_numeric (ip, &addr))
        {
          dsk_warning ("parsing /etc/hosts line %u: error parsing ip address",
                       lineno);
          continue;
        }
      host_entry = dsk_malloc (sizeof (DskDnsCacheEntry)
                               + strlen (name) + 1);

      host_entry->info.addr.addresses = DSK_NEW (DskIpAddress);
      host_entry->info.addr.addresses[0] = addr;
      host_entry->name = (char*)(host_entry + 1);
      strcpy (host_entry->name, name);
      for (at = host_entry->name; *at; at++)
        if (dsk_ascii_isupper (*at))
          *at += ('a' - 'A');

      host_entry->is_ipv6 = addr.type == DSK_IP_ADDRESS_IPV6;
      host_entry->expire_time = NO_EXPIRE_TIME;
      host_entry->type = DSK_DNS_CACHE_ENTRY_ADDR;
      host_entry->info.addr.n = 1;
      host_entry->info.addr.i = 0;
      host_entry->info.addr.addresses[0] = addr;
retry:
      DSK_RBTREE_INSERT (GET_ETC_HOSTS_TREE (), host_entry, conflict);
      if (conflict != NULL)
        {
          DSK_RBTREE_REMOVE (GET_ETC_HOSTS_TREE (), conflict);
          goto retry;
        }
    }
  fclose (fp);

  /* parse /etc/resolv.conf */
  fp = fopen ("/etc/resolv.conf", "r");
  if (fp == NULL)
    {
      dsk_set_error (error, "error opening %s: %s",
                     "/etc/resolv.conf", strerror (errno));
      return DSK_FALSE;
    }
  lineno = 0;
  while (fgets (buf, sizeof (buf), fp) != NULL)
    {
      char *at = buf;
      char *command, *arg;
      ++lineno;
      while (*at && dsk_ascii_isspace (*at))
        at++;
      if (*at == '#')
        continue;
      DSK_ASCII_SKIP_SPACE (at);
      if (*at == '#')
        continue;
      command = at;
      while (*at && !dsk_ascii_isspace (*at))
        {
          if ('A' <= *at && *at <= 'Z')
            *at += ('a' - 'Z');
          at++;
        }
      *at++ = 0;
      DSK_ASCII_SKIP_SPACE (at);
      arg = at;
      DSK_ASCII_SKIP_NONSPACE (at);
      *at = 0;
      if (strcmp (command, "search") == 0)
        {
          const char *in;
          char *out;
          dsk_boolean dot_allowed;
          unsigned i;

          /* Add a searchpath to the list. */

          /* normalize argument (lowercase; check syntax) */
          in = out = arg;
          dot_allowed = DSK_FALSE;
          while (*in)
            {
              if (*in == '.')
                {
                  if (dot_allowed)
                    {
                      *out++ = '.';
                      dot_allowed = DSK_FALSE;
                    }
                }
              else if ('A' <= *in && *in <= 'Z')
                {
                  dot_allowed = DSK_TRUE;
                  *out++ = *in + ('a' - 'A');
                }
              else if (('0' <= *in && *in <= '9')
                    || ('a' <= *in && *in <= 'z')
                    || (*in == '-')
                    || (*in == '_'))
                {
                  dot_allowed = DSK_TRUE;
                  *out++ = *in;
                }
              else
                {
                  dsk_warning ("disallowed character '%c' in searchpath in /etc/resolv.conf line %u", *in, lineno);
                  goto next;
                }
              in++;
            }
          *out = 0;

          /* remove trailing dot, if it exists */
          if (!dot_allowed && out > arg)
            *(out-1) = 0;

          if (*arg == 0)
            {
              dsk_warning ("empty searchpath entry in /etc/resolv.conf (line %u)", lineno);
              goto next;
            }

          /* add if not already in set */
          for (i = 0; i < n_resolv_conf_search_paths; i++)
            if (strcmp (arg, resolv_conf_search_paths[i]) == 0)
              {
                dsk_warning ("match: searchpath[%u] = %s", i, resolv_conf_search_paths[i]);
                break;
              }
          if (i < n_resolv_conf_search_paths)
            {
              dsk_warning ("searchpath '%s' appears twice in /etc/resolv.conf (line %u)",
                           arg, lineno);
            }
          else
            {
              unsigned length = strlen (arg);
              resolv_conf_search_paths = DSK_RENEW (char *,
                                            resolv_conf_search_paths,
                                            n_resolv_conf_search_paths + 1);
              if (max_resolv_conf_searchpath_len < length)
                max_resolv_conf_searchpath_len = length;
              resolv_conf_search_paths[n_resolv_conf_search_paths++] = dsk_strdup (arg);
            }
        }
      else if (strcmp (command, "nameserver") == 0)
        {
          /* add nameserver */
          DskIpAddress addr;
          unsigned i;
          if (!dsk_ip_address_parse_numeric (arg, &addr))
            {
              dsk_warning ("in /etc/resolv.conf, line %u: error parsing ip address", lineno);
              goto next;
            }
          for (i = 0; i < n_resolv_conf_ns; i++)
            if (dsk_ip_addresses_equal (&resolv_conf_ns[i].address, &addr))
              break;
          if (i < n_resolv_conf_ns)
            {
              dsk_warning ("in /etc/resolv.conf, line %u: nameserver %s already exists", lineno, arg);
            }
          else
            {
              resolv_conf_ns = DSK_RENEW (NameserverInfo, resolv_conf_ns,
                                          n_resolv_conf_ns + 1);
              nameserver_info_init (&resolv_conf_ns[n_resolv_conf_ns++], &addr);
            }
        }
      else
        {
          dsk_warning ("unknown command '%s' in /etc/resolv.conf line %u",
                       command, lineno);
        }
next:
      ;
    }
  fclose (fp);
  if (n_resolv_conf_ns == 0)
    {
      dsk_set_error (error, "no nameservers given: cannot resolve names");
      return DSK_FALSE;
    }

  /* make UDP socket for queries */
  dns_udp_socket = dsk_udp_socket_new (DSK_FALSE, error);
  if (dns_udp_socket == NULL)
    {
      dsk_add_error_prefix (error, "initializing dns client");
      return DSK_FALSE;
    }
  dsk_hook_trap (&dns_udp_socket->readable,
                 handle_dns_udp_socket_readable,
                 NULL, NULL);


  dns_initialized = DSK_TRUE;
  return DSK_TRUE;
}

#define MAYBE_DNS_INIT_RETURN(error, error_rv)         \
  do {                                                 \
    if (!dns_initialized && !dsk_dns_try_init (error)) \
      return error_rv;                                 \
  }while(0)

/* --- expunging old records --- */
static unsigned expunge_block_count = 0;
static dsk_boolean blocked_expunge = DSK_FALSE;
static void
expunge_old_cache_entries (void)
{
  dsk_time_t cur_time;
  if (expunge_block_count)
    {
      blocked_expunge = DSK_TRUE;
      return;
    }
  cur_time = dsk_get_current_time ();
  while (expiration_tree)
    {
      DskDnsCacheEntry *oldest;
      DSK_RBTREE_FIRST (GET_EXPIRATION_TREE (), oldest);
      if (oldest->expire_time > cur_time)
        break;

      /* free oldest */
      DSK_RBTREE_REMOVE (GET_CACHE_BY_NAME_TREE (), oldest);
      DSK_RBTREE_REMOVE (GET_EXPIRATION_TREE (), oldest);
      switch (oldest->type)
        {
        case DSK_DNS_CACHE_ENTRY_IN_PROGRESS:
          /* IN_PROGRESS cache-entries don't expire -- they time out --
             in the sense that expiration happens a fixed time after
             a reponse. timing-out occurs after all the retries have failed. */
          dsk_assert_not_reached ();

        case DSK_DNS_CACHE_ENTRY_ERROR:
          dsk_error_unref (oldest->info.error.error);
          break;
        case DSK_DNS_CACHE_ENTRY_NEGATIVE:
          break;
        case DSK_DNS_CACHE_ENTRY_CNAME:
          dsk_free (oldest->info.cname);
          break;
        case DSK_DNS_CACHE_ENTRY_ADDR:
          dsk_free (oldest->info.addr.addresses);
          break;
        }
    }
}


/* --- low-level ---*/
/* MAY or MAY NOT set error.
   Unset if we just need to block; set if there was an error
   (too many cnames is the only possible error).

   name must be canonicalized. */
DskDnsCacheEntry *
dsk_dns_lookup_nonblocking_entry (const char    *name,
                                  dsk_boolean    is_ipv6,
                                  DskError     **error)
{
  DskDnsCacheEntry ce;
  DskDnsCacheEntry *entry;
  MAYBE_DNS_INIT_RETURN (error, NULL);
  ce.name = (char*) name;
  ce.is_ipv6 = is_ipv6 ? 1 : 0;
  DSK_RBTREE_LOOKUP (GET_CACHE_BY_NAME_TREE (), &ce, entry);
  return entry;
}
DskDnsLookupNonblockingResult
dsk_dns_lookup_nonblocking (const char *name,
                           DskIpAddress *out,
                           dsk_boolean    is_ipv6,
                           DskError     **error)
{
  unsigned n_cnames = 0;
  MAYBE_DNS_INIT_RETURN (error, DSK_DNS_LOOKUP_NONBLOCKING_ERROR);
  do
    {
      DskDnsCacheEntry ce;
      DskDnsCacheEntry *entry;
      ce.name = (char*) name;
      ce.is_ipv6 = is_ipv6 ? 1 : 0;
      DSK_RBTREE_LOOKUP (GET_CACHE_BY_NAME_TREE (), &ce, entry);
      if (entry == NULL)
        return DSK_DNS_LOOKUP_NONBLOCKING_MUST_BLOCK;
      switch (entry->type)
        {
	case DSK_DNS_CACHE_ENTRY_IN_PROGRESS:
          return DSK_DNS_LOOKUP_NONBLOCKING_MUST_BLOCK;
        case DSK_DNS_CACHE_ENTRY_ERROR:
          dsk_set_error (error, "looking up %s: %s",
                         name,
                         entry->info.error.error->message);
          return DSK_DNS_LOOKUP_NONBLOCKING_ERROR;
	case DSK_DNS_CACHE_ENTRY_NEGATIVE:
          return DSK_DNS_LOOKUP_NONBLOCKING_NOT_FOUND;
	case DSK_DNS_CACHE_ENTRY_CNAME:
          name = entry->info.cname;
	  break;
	case DSK_DNS_CACHE_ENTRY_ADDR:
          *out = entry->info.addr.addresses[entry->info.addr.i];
          if (++entry->info.addr.i == entry->info.addr.n)
            entry->info.addr.i = 0;
          return DSK_DNS_LOOKUP_NONBLOCKING_FOUND;
	default:
	  dsk_assert_not_reached ();
	}
    }
  while (n_cnames < MAX_CNAMES);
  dsk_set_error (error, "looking up %s: too many cnames",
		 name);
  return DSK_DNS_LOOKUP_NONBLOCKING_ERROR;
} 

static DskIOResult
job_send_message (DskDnsCacheEntryJob *job, DskError **error)
{
  return dsk_udp_socket_send_to_ip (dns_udp_socket,
                                    &resolv_conf_ns[job->ns_index].address,
                                    DSK_DNS_PORT,
                                    job->message_len, job->message,
                                    error);
}

static void clear_waiting_to_send_flag (DskDnsCacheEntryJob *job);
static void raise_waiting_to_send_flag (DskDnsCacheEntryJob *job);

static dsk_boolean
handle_socket_writable (void)
{
  while (first_waiting_to_send != NULL)
    {
      DskDnsCacheEntryJob *job = first_waiting_to_send;
      DskError *error = NULL;
      clear_waiting_to_send_flag (job);
      switch (job_send_message (job, &error))
        {
        case DSK_IO_RESULT_SUCCESS:
          /* wait for message or timeout */
          break;

        case DSK_IO_RESULT_AGAIN:
          raise_waiting_to_send_flag (job);
          return DSK_FALSE; /* do not run this trap again -
                               we have a new one */

        case DSK_IO_RESULT_EOF:
          dsk_assert_not_reached ();
          break;
        case DSK_IO_RESULT_ERROR:
          /* Treat this like a timeout */
          dsk_warning ("error sending UDP for DNS: %s", error->message);
          dsk_error_unref (error);
          break;

        default:
          dsk_assert_not_reached ();
        }
    }
  return DSK_FALSE;
}

#define GET_WAITING_TO_SEND_LIST() \
  DskDnsCacheEntryJob *, first_waiting_to_send, last_waiting_to_send, \
  prev_waiting, next_waiting
static void
raise_waiting_to_send_flag (DskDnsCacheEntryJob *job)
{
  if (!job->waiting_to_send)
    {
      if (first_waiting_to_send == NULL)
        {
          dns_udp_socket_trap
            = dsk_hook_trap (&dns_udp_socket->writable,
                             (DskHookFunc) handle_socket_writable,
                             NULL, NULL);
        }
      DSK_LIST_APPEND (GET_WAITING_TO_SEND_LIST (), job);
      job->waiting_to_send = DSK_TRUE;
    }
}
static void
clear_waiting_to_send_flag (DskDnsCacheEntryJob *job)
{
  if (job->waiting_to_send)
    {
      DSK_LIST_REMOVE (GET_WAITING_TO_SEND_LIST (), job);
      job->waiting_to_send = DSK_FALSE;
      if (first_waiting_to_send == NULL)
        {
          dsk_hook_trap_destroy (dns_udp_socket_trap);
          dns_udp_socket_trap = NULL;
        }
    }
}

static void
handle_timer_expired (void        *data)
{
  DskDnsCacheEntryJob *job = data;
  DskError *error = NULL;
  (void) data;

  clear_waiting_to_send_flag (job);
  job->timer = NULL;

  /* is this the last attempt? */
  if (job->attempt + 1 == DSK_N_ELEMENTS (retry_schedule))
    {
      /* Setup cache-entry */
      DskDnsCacheEntry *owner = job->owner;
      DskDnsCacheEntry *conflict;
      owner->type = DSK_DNS_CACHE_ENTRY_ERROR;
      owner->info.error.error = dsk_error_new ("timed out waiting for response");
      owner->expire_time = dsk_get_current_time () + 1;
      DSK_RBTREE_INSERT (GET_EXPIRATION_TREE (), owner, conflict);
      dsk_assert (conflict == NULL);

      job_notify_waiters_and_free (job);
      return;
    }

  /* adjust timer */
  job->attempt += 1;
  job->timer = dsk_main_add_timer_millis (retry_schedule[job->attempt],
                                          handle_timer_expired, job);

  /* try a different nameserver */
  job->ns_index += 1;
  if (job->ns_index == n_resolv_conf_ns)
    job->ns_index = 0;

  /* resend message */
  switch (job_send_message (job, &error))
    {
    case DSK_IO_RESULT_SUCCESS:
      /* wait for message or timeout */
      break;

    case DSK_IO_RESULT_AGAIN:
      raise_waiting_to_send_flag (job);
      break;

    case DSK_IO_RESULT_EOF:
      dsk_assert_not_reached ();
      break;
    case DSK_IO_RESULT_ERROR:
      /* Treat this like a timeout */
      dsk_warning ("error sending UDP for DNS: %s", error->message);
      dsk_error_unref (error);
      break;

    default:
      dsk_assert_not_reached ();
    }
}

static void
begin_dns_request (DskDnsCacheEntry *entry)
{
  /* pick dns server */
  DskDnsCacheEntryJob *job;
  unsigned dns_index = next_nameserver_index++;
  DskDnsMessage message;
  DskDnsQuestion question;
  DskError *error = NULL;

  if (next_nameserver_index == n_resolv_conf_ns)
    next_nameserver_index = 0;

  /* create job */
  entry->info.in_progress = job = DSK_NEW (DskDnsCacheEntryJob);
  job->owner = entry;
  job->ns_index = dns_index;
  job->waiters = NULL;
  job->attempt = 0;
  job->timer = NULL;
  job->message = NULL;
  job->waiting_to_send = DSK_FALSE;

  /* send dns question to nameserver */
  memset (&message, 0, sizeof (message));
  message.n_questions = 1;
  message.questions = &question;
  message.id = dns_index;
  message.is_query = 1;
  message.recursion_desired = 1;
  message.opcode = DSK_DNS_OP_QUERY;
  question.name = entry->name;
  question.query_type = entry->is_ipv6 ? DSK_DNS_RR_HOST_ADDRESS_IPV6 : DSK_DNS_RR_HOST_ADDRESS;
  question.query_class = DSK_DNS_CLASS_IN;
  job->message = dsk_dns_message_serialize (&message, &job->message_len);
  job->timer = dsk_dispatch_add_timer_millis (dsk_dispatch_default (),
                                              retry_schedule[0],
                                              handle_timer_expired,
                                              job);
  switch (job_send_message (job, &error))
    {
    case DSK_IO_RESULT_SUCCESS:

      /* make timeout timer */
      break;
    case DSK_IO_RESULT_AGAIN:
      raise_waiting_to_send_flag (job);
      break;
    case DSK_IO_RESULT_EOF:
      dsk_assert_not_reached ();
      break;
    case DSK_IO_RESULT_ERROR:
      dsk_warning ("error sending UDP for DNS: %s", error->message);
      dsk_error_unref (error);

      /* Proceed to create fail timer; we will handle this like a timeout */
      break;
    default:
      dsk_assert_not_reached ();
    }
}


static void
lookup_without_searchpath_or_cnames (const char       *normalized_name,
                                     dsk_boolean       is_ipv6,
                                     DskDnsCacheEntryFunc callback,
                                     void             *callback_data)
{
  DskDnsCacheEntry ce;
  DskDnsCacheEntry *entry;
  ce.name = (char*) normalized_name;
  ce.is_ipv6 = is_ipv6;


  /* lookup in /etc/hosts if enabled */
  if (config_flags & DSK_DNS_CONFIG_USE_ETC_HOSTS)
    {
      DSK_RBTREE_LOOKUP (GET_ETC_HOSTS_TREE (), &ce, entry);
      if (entry != NULL)
        {
          callback (entry, callback_data);
          return;
        }
    }

  /* lookup in cache */
  DSK_RBTREE_LOOKUP (GET_CACHE_BY_NAME_TREE (), &ce, entry);

  /* --- do actual dns request --- */

  /* create new IN_PROGRESS entry */
  if (entry == NULL)
    {
      DskDnsCacheEntry *conflict;
      entry = dsk_malloc (sizeof (DskDnsCacheEntry) + strlen (normalized_name) + 1);
      entry->name = strcpy ((char*)(entry+1), normalized_name);
      entry->is_ipv6 = is_ipv6;
      entry->expire_time = NO_EXPIRE_TIME;
      entry->type = DSK_DNS_CACHE_ENTRY_IN_PROGRESS;

      DSK_RBTREE_INSERT (GET_CACHE_BY_NAME_TREE (), entry, conflict);
      dsk_assert (conflict == NULL);
      begin_dns_request (entry);

      expunge_old_cache_entries ();
    }
  if (entry->type == DSK_DNS_CACHE_ENTRY_IN_PROGRESS)
    {
      /* This happens in an existing pending DNS lookup,
         as well as when a new CacheEntry is created. */

      /* add to watch list */
      Waiter *waiter;
      waiter = DSK_NEW (Waiter);
      waiter->func = callback;
      waiter->data = callback_data;
      waiter->next = entry->info.in_progress->waiters;
      entry->info.in_progress->waiters = waiter;
    }
  else
    {
      ++expunge_block_count;
      (*callback) (entry, callback_data);
      if (--expunge_block_count == 0 && blocked_expunge)
        {
          blocked_expunge = DSK_FALSE;
          expunge_old_cache_entries ();
        }
      return;
    }
}

typedef struct _CnameInfo CnameInfo;
struct _CnameInfo
{
  dsk_boolean is_ipv6;
  unsigned n_cnames;
  DskDnsCacheEntryFunc callback;
  void *callback_data;
  unsigned expire_time;
};

static void
handle_cname_callback (DskDnsCacheEntry *entry,
                       void             *callback_data)
{
  CnameInfo *ci = callback_data;
  switch (entry->type)
    {
    case DSK_DNS_CACHE_ENTRY_IN_PROGRESS:
      dsk_assert_not_reached ();
    case DSK_DNS_CACHE_ENTRY_ERROR:
    case DSK_DNS_CACHE_ENTRY_NEGATIVE:
    case DSK_DNS_CACHE_ENTRY_ADDR:
      ci->callback (entry, ci->callback_data);
      dsk_free (ci);
      break;
    case DSK_DNS_CACHE_ENTRY_CNAME:
      if (ci->n_cnames >= MAX_CNAMES)
        {
          DskDnsCacheEntry ce;
          ce.type = DSK_DNS_CACHE_ENTRY_ERROR;
          ce.expire_time = ci->expire_time;
          ce.info.error.error = dsk_error_new ("too many cnames (at %s -> %s)",
                                               entry->name,
                                               entry->info.cname);
          ci->callback (&ce, ci->callback_data);
          dsk_error_unref (ce.info.error.error);
          dsk_free (ci);
          break;
        }
      ci->n_cnames += 1;
      if (entry->expire_time < ci->expire_time)
        ci->expire_time = entry->expire_time;
      lookup_without_searchpath_or_cnames (entry->info.cname,
                                           ci->is_ipv6, handle_cname_callback,
                                           ci);
      break;
    default:
      dsk_assert_not_reached ();
    }
}


static void
lookup_without_searchpath (const char       *normalized_name,
                           dsk_boolean       is_ipv6,
                           DskDnsLookupFlags flags,
                           DskDnsCacheEntryFunc callback,
                           void             *callback_data)
{
  if (flags & DSK_DNS_LOOKUP_FOLLOW_CNAMES)
    {
      CnameInfo *ci = DSK_NEW (CnameInfo);
      ci->is_ipv6 = is_ipv6;
      ci->n_cnames = 0;
      ci->callback = callback;
      ci->callback_data = callback_data;
      ci->expire_time = 0xffffffff;
      lookup_without_searchpath_or_cnames (normalized_name,
                                           is_ipv6, handle_cname_callback,
                                           ci);
    }
  else
    lookup_without_searchpath_or_cnames (normalized_name,
                                         is_ipv6, callback,
                                         callback_data);
}

typedef struct _SearchpathStatus SearchpathStatus;
struct _SearchpathStatus
{
  unsigned searchpath_index;
  unsigned n_cnames;
  char *name;
  char *end_of_name;
  unsigned expire_time;
  unsigned in_progress : 1;
  unsigned not_found_while_in_progress : 1;
  unsigned found_while_in_progress : 1;
  unsigned is_ipv6 : 1;
  DskDnsCacheEntryFunc callback;
  void *callback_data;
  DskDnsLookupFlags lookup_flags;
};
static void
handle_searchpath_entry_lookup (DskDnsCacheEntry *entry,
                                void             *callback_data)
{
  SearchpathStatus *status = callback_data;
  switch (entry->type)
    {
    case DSK_DNS_CACHE_ENTRY_IN_PROGRESS:
      dsk_assert_not_reached ();
    case DSK_DNS_CACHE_ENTRY_ERROR:
      status->callback (entry, status->callback_data);
      if (status->in_progress)
        status->found_while_in_progress = DSK_TRUE;
      else
        {
          dsk_free (status->name);
          dsk_free (status);
          return;
        }
      break;
    case DSK_DNS_CACHE_ENTRY_NEGATIVE:
      if (status->expire_time > entry->expire_time)
        status->expire_time = entry->expire_time;
      if (status->in_progress)
        status->not_found_while_in_progress = DSK_TRUE;
      else
        {
next_search_path:
          /* go to next searchpath, or not found */
          ++(status->searchpath_index);
          if (status->searchpath_index <= n_resolv_conf_search_paths)
            {
              /* lookup with the given searchpath */
              status->in_progress = DSK_TRUE;
              status->found_while_in_progress = DSK_FALSE;
              status->not_found_while_in_progress = DSK_FALSE;
              if (status->searchpath_index == n_resolv_conf_search_paths)
                /* no searchpath */
                *(status->end_of_name) = 0;
              else
                /* searchpath */
                strcpy (status->end_of_name,
                        resolv_conf_search_paths[status->searchpath_index]);
              lookup_without_searchpath (status->name, status->is_ipv6,
                                         status->lookup_flags,
                                         handle_searchpath_entry_lookup,
                                         status);
              status->in_progress = DSK_FALSE;
              if (status->found_while_in_progress)
                {
                  dsk_free (status->name);
                  dsk_free (status);
                  return;
                }
              else if (status->not_found_while_in_progress)
                {
                  goto next_search_path;
                }
              else
                {
                  /* we must block, waiting for the query to return; the
                     work will resume at that point. */
                }
            }
          else
            {
              /* not found */
              DskDnsCacheEntry fake;
              fake.expire_time = status->expire_time;
              fake.name = status->name;
              fake.type = DSK_DNS_CACHE_ENTRY_NEGATIVE;
              fake.is_ipv6 = status->is_ipv6;
              status->callback (&fake, status->callback_data);
              dsk_free (status->name);
              dsk_free (status);
            }
          return;
        }
      break;
    case DSK_DNS_CACHE_ENTRY_CNAME:
    case DSK_DNS_CACHE_ENTRY_ADDR:
      status->callback (entry, status->callback_data);
      if (status->in_progress)
        status->found_while_in_progress = 1;
      else
        {
          dsk_free (status->name);
          dsk_free (status);
        }
      return;
    }
}

/* NOTE: we call with 'name' taken from another cache entry when resolving cnames.
   SO this must copy the string BEFORE it ousts anything from its cache. */
void
dsk_dns_lookup_cache_entry (const char       *name,
                            dsk_boolean       is_ipv6,
                            DskDnsLookupFlags flags,
                            DskDnsCacheEntryFunc callback,
                            void             *callback_data)
{
  DskDnsCacheEntry ce;
  DskError *error = NULL;
  char normalized_name[DSK_DNS_MAX_NAMELEN + 1];
  const char *in = name;
  char *out = normalized_name;
  dsk_boolean last_was_dot = DSK_TRUE;          /* to inhibit initial '.'s */
  dsk_boolean ends_with_dot = DSK_FALSE;

  /* initialize */
  if (!dns_initialized && !dsk_dns_try_init (&error))
    {
      DskDnsCacheEntry entry;
      entry.name = (char*) normalized_name;
      entry.is_ipv6 = is_ipv6;
      entry.expire_time = 0;
      entry.type = DSK_DNS_CACHE_ENTRY_ERROR;
      entry.info.error.error = error;
      callback (&entry, callback_data);
      dsk_warning ("error initializing dns subsystem: %s", error->message);
      dsk_error_unref (error);
      return;
    }

  /* ensure is_ipv6 is 0 or 1 */
  if (is_ipv6)
    is_ipv6 = DSK_TRUE;

  /* normalize name */
  while (*in)
    {
      if (*in == '.')
        {
          if (!last_was_dot)
            {
              *out++ = '.';
              if (out == normalized_name + DSK_DNS_MAX_NAMELEN)
                goto name_too_long;
              last_was_dot = DSK_TRUE;
            }
        }
      else
        {
          last_was_dot = DSK_FALSE;
          if (('0' <= *in && *in <= '9')
            || ('a' <= *in && *in <= 'z')
            || *in == '-' || *in == '_')
            *out++ = *in;
          else if ('A' <= *in && *in <= 'Z')
            *out++ = *in + ('a' - 'A');
          else
            {
              ce.name = (char*)name;
              ce.is_ipv6 = is_ipv6;
              ce.type = DSK_DNS_CACHE_ENTRY_ERROR;
              ce.info.error.error = dsk_error_new ("illegal char in domain-name");
              callback (&ce, callback_data);
              dsk_error_unref (ce.info.error.error);
              return;
            }
          if (out == normalized_name + DSK_DNS_MAX_NAMELEN)
            goto name_too_long;
        }
      in++;
    }
  *out = 0;
  if (normalized_name[0] == 0)
    {
      ce.name = (char*)name;
      ce.is_ipv6 = is_ipv6;
      ce.type = DSK_DNS_CACHE_ENTRY_ERROR;
      ce.info.error.error = dsk_error_new ("empty domain name cannot be looked up");
      callback (&ce, callback_data);
      return;
    }
  if (last_was_dot)
    {
      --out;
      *out = 0;
      ends_with_dot = DSK_TRUE;
    }
  /* ensure dns system is ready */
  if (!dns_initialized && !dsk_dns_try_init (&error))
    {
      ce.name = (char*) name;
      ce.is_ipv6 = is_ipv6;
      ce.expire_time = NO_EXPIRE_TIME;
      ce.type = DSK_DNS_CACHE_ENTRY_ERROR;
      ce.info.error.error = error;
      callback (&ce, callback_data);
      dsk_error_unref (error);
      return;
    }

  if (ends_with_dot || n_resolv_conf_search_paths == 0
      || (config_flags & DSK_DNS_CONFIG_USE_RESOLV_CONF_SEARCHPATH) == 0
      || (flags & DSK_DNS_LOOKUP_USE_SEARCHPATH) == 0)
    {
      lookup_without_searchpath (normalized_name, is_ipv6, flags, callback, callback_data);
      return;
    }
  else
    {
      /* iterate through searchpath, eventually trying "no search path" */
      SearchpathStatus *status = DSK_NEW (SearchpathStatus);
      status->searchpath_index = 0;
      status->name = normalized_name;
      status->expire_time = 0xffffffff;
      status->callback = callback;
      status->callback_data = callback_data;
      status->is_ipv6 = is_ipv6;
      status->lookup_flags = flags;

      while (status->searchpath_index <= n_resolv_conf_search_paths)
        {
          unsigned j = status->searchpath_index;
          /* construct full name */
          if (status->searchpath_index == n_resolv_conf_search_paths)
            *out = 0;
          else
            {
              *out = '.';
              strcpy (out + 1, resolv_conf_search_paths[j]);
            }

          /* make request without searchpath */
          status->in_progress = DSK_TRUE;
          status->not_found_while_in_progress = DSK_FALSE;
          status->found_while_in_progress = DSK_FALSE;
          lookup_without_searchpath (normalized_name, is_ipv6, flags,
                                     handle_searchpath_entry_lookup, status);
          status->in_progress = DSK_FALSE;
          if (status->not_found_while_in_progress)
            {
              /* next searchpath */
              status->searchpath_index += 1;
            }
          else if (status->found_while_in_progress)
            {
              /* free status object */
              dsk_free (status);
              return;
            }
          else
            {
              unsigned name_len = out - normalized_name;
              status->name = dsk_malloc (name_len
                                         + 1 + max_resolv_conf_searchpath_len
                                         + 1);
              strcpy (status->name, normalized_name);
              status->end_of_name = status->name + name_len;

              return;           /* wait for callback */
            }
        }

      /* not found */
      {
        ce.type = DSK_DNS_CACHE_ENTRY_NEGATIVE;
        ce.name = normalized_name;
        ce.is_ipv6 = is_ipv6;
        ce.expire_time = status->expire_time;
        callback (&ce, callback_data);
        dsk_free (status);
      }
    }
  return;


name_too_long:
  {
    ce.type = DSK_DNS_CACHE_ENTRY_ERROR;
    ce.name = (char*) name;
    ce.is_ipv6 = is_ipv6;
    ce.expire_time = 0xffffffff;
    ce.info.error.error = dsk_error_new ("name too long in DNS lookup");
    callback (&ce, callback_data);
    dsk_error_unref (ce.info.error.error);
  }
}
