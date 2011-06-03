#include "../dsk-table.h"
#include "../dsk-http-server.h"

/* FORMATS ---
  Generally:
    - 40-bit ints are used for user-ids

  Specific formats:
    - User -  (the value for both the by-id and by-name tables)
      - 1-byte version number (0 for now)
      - 40-bit id
      - NUL-terminated name
    - User -  (the value for both the by-id and by-name tables)

struct _User
{
  uint64_t id;
  char *name;
  unsigned n_friends;
  uint64_t *friends;
};

struct _Message
{
  uint64_t id;
  uint64_t author;
  char *text;
};

/* --- globals --- */
static DskTable *name_to_user;
static DskTable *userid_to_user;
static DskTable *word_userid_to_messageid;
static uint64_t *next_user_id;          /* mmapped */

/* --- utility functions --- */
static User *
parse_user (unsigned len, const uint8_t *data)
{
  ...
}
static User *
lookup_user_by_id (uint64_t userid)
{
  ...
}
static User *
lookup_user_by_name (const char *name)
{
  ...
}

/* NOTE: takes ownership of 'user' (for possible caching) */
static void
write_user (User *user)
{
  /* serialize */
  ...

  /* add to "by-id" table */
  ...

  /* add to "by-name" table */
  ...
}


/* --- CGI handlers --- */

/* cgi helpers */

/* 'mandatory' means we respond with an error
   if the CGI variable isn't available. */
static const char *
get_cgi_var (DskHttpServerRequest *request,
             const char           *variable_name,
             dsk_boolean           mandatory)
{
  for (i = 0; i < request->n_cgi_variables; i++)
    if (strcmp (request->cgi_variables[i].name, variable_name) == 0)
      return request->cgi_variables[i].value;
  if (mandatory)
    {
      unsigned len = strlen (variable_name) + 100;
      char *msg = dsk_malloc (len);
      snprintf (msg, len, "no CGI variable %s found", variable_name);
      dsk_http_server_request_respond_error (request,
                                             DSK_HTTP_STATUS_BAD_REQUEST, msg);
      dsk_free (msg);
    }
  return NULL;
}

static void
cgi_handler__add_user (DskHttpServerRequest *request,
                       void                 *func_data)
{
  const char *name = get_cgi_var (request, "name", DSK_TRUE);
  if (name == NULL)
    return;

  /* lookup username: fail if user already exists */
  User *user = lookup_user_by_name (name);
  if (user != NULL)
    {
      ...
    }

  /* XXX: should be atomic: (allocate user-id, and the two inserts) */

  user = dsk_malloc (sizeof (User) + strlen (name) + 1);
  user->name = (char *) (user + 1);
  strcpy (user->name, name);
  user->n_friends = 0;
  user->friends = NULL;


  /* allocate user-id */
  user->user_id = *next_user_id;
  *next_user_id += 1;

  /* respond with user-id */
  DskHttpServerResponseOptions options = DSK_HTTP_SERVER_RESPONSE_OPTIONS_DEFAULT;
  options.content_type = "text/plain";
  snprintf (buf, sizeof (buf), "%llu", user->id);
  options.content_body = buf;
  options.content_length = strlen (buf);
  dsk_http_server_respond (request, &options);

  write_user (user);            /* takes ownership of user */
}

static void
cgi_handler__lookup_user (DskHttpServerRequest *request,
                          void                 *func_data)
{
  /* 
  /* lookup username: fail if user already exists */
  const char *name = get_cgi_var (request, "name", DSK_TRUE);
  if (name == NULL)
    return;
  ...
}

static void
cgi_handler__add_connection (DskHttpServerRequest *request,
                             void                 *func_data)
{
  /* ensure both userids are valid */
  User *a,*b;
  ...

  /* find out if they are already friends with eachother. */
  dsk_boolean a_friendswith_b, b_friendswith_a;
  ...

  if (a_friendswith_b && b_friendswith_a)
    {
      /* error: already friends */
      ...
      return;
    }
  dsk_warn_if_fail(!a_friendswith_b, "friendship in this network is symmetric");
  dsk_warn_if_fail(!b_friendswith_a, "friendship in this network is symmetric");

  if (!a_friendswith_b)
    {
      /* insert user */
      ...
    }
  if (!b_friendswith_a)
    {
      /* insert user */
      ...
    }

  /* respond vapidly, but successfully */
  DskHttpServerResponseOptions options = DSK_HTTP_SERVER_RESPONSE_OPTIONS_DEFAULT;
  options.content_type = "text/plain";
  options.content_body = "";
  options.content_length = 0;
  dsk_http_server_respond (request, &options);
}

static void
cgi_handler__add_connection (DskHttpServerRequest *request,
                             void                 *func_data)
{
  /* ensure both userids are valid */
  User *a,*b;
  ...

  /* find out if they are already friends with eachother. */
  dsk_boolean a_friendswith_b, b_friendswith_a;
  ...

  if (a_friendswith_b && b_friendswith_a)
    {
      /* error: already friends */
      ...
      return;
    }
  dsk_warn_if_fail(!a_friendswith_b, "friendship in this network is symmetric");
  dsk_warn_if_fail(!b_friendswith_a, "friendship in this network is symmetric");

  if (!a_friendswith_b)
    {
      /* insert user */
      ...
    }
  if (!b_friendswith_a)
    {
      /* insert user */
      ...
    }

  /* respond vapidly, but successfully */
  ...
}

static void
cgi_handler__blather        (DskHttpServerRequest *request,
                             void                 *func_data)
{
  /* add message to message index and assign ID */
  ...

  /* break message into words (maybe some bigrams) */
  ...

  /* add all friend/word pairs to index */
  ...

  /* respond with message id */
  ...
}

static void
cgi_handler__search    (DskHttpServerRequest *request,
                        void                 *func_data)
{
  ...
}

int
main (int argc, char **argv)
{
  unsigned port;
  dsk_boolean bind_any = DSK_FALSE;
  DskIpAddress bind_address = DSK_IP_ADDRESS_DEFAULT;
  dsk_cmdline_init ("run a social network search engine",
                    "implement an HTTP server with entry points\n"
                    "suitable for a social network: create users,\n"
                    "create connections, and add messages.\n",
                    NULL,
                    0);
  dsk_cmdline_add_uint ("port", "port to listen on", "PORT",
                        DSK_CMDLINE_MANDATORY, &port);
  dsk_cmdline_add_boolean ("bind-any", "accept connections from any host", NULL,
                        0, &bind_any);
  dsk_cmdline_process_args (&argc, &argv);

  server = dsk_http_server_new ();

  static struct {
    const char *pattern;
    DskHttpServerCgiFunc func;
  } handlers[] = {
    { "/add-user\\?.*", cgi_handler__add_user },
    { "/add-connection\\?.*", cgi_handler__add_connection },
    { "/blather\\?.*", cgi_handler__blather },
  };

  for (i = 0; i < DSK_N_ELEMENTS (handlers); i++)
    {
      dsk_http_server_match_save (server);
      dsk_http_server_add_match (server, DSK_HTTP_SERVER_MATCH_PATH,
                                 handlers[i].pattern);
      dsk_http_server_register_cgi_handler (server, handlers[i].func,
                                            NULL, NULL);
      dsk_http_server_match_restore (server);
    }

  if (!bind_any)
    dsk_ip_address_localhost (&bind_address);

  if (!dsk_http_server_bind_tcp (server, &bind_address, port, &error))
    dsk_die ("error binding to port %u: %s", port, error->message);

  return dsk_main_run ();
}
