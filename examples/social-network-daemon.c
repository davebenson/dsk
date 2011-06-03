#include "../dsk.h"
#include "generated/socnet.pb-c.h"
#include "../gskrbtreemacros.h"
#include "../gsklistmacros.h"
#include <string.h>
#include <stdio.h>

typedef Socnet__User User;
typedef Socnet__Blather Blather;
typedef Socnet__Feed Feed;

/* --- globals --- */
static DskTable *name_to_user;
static DskTable *userid_to_user;
static DskTable *word_userid_to_feed;
static DskTable *blatherid_to_blather;
static uint64_t *next_user_id;          /* mmapped */
static uint64_t *next_blather_id;          /* mmapped */

/* --- user methods & caching --- */
typedef struct _UserCacheNode UserCacheNode;
struct _UserCacheNode
{
  User *user;
  UserCacheNode *byid_left, *byid_right, *byid_parent;
  UserCacheNode *byname_left, *byname_right, *byname_parent;
  uint8_t byid_is_red, byname_is_red;

  UserCacheNode *older, *newer;
};
UserCacheNode *usercache_byid_tree = NULL, *usercache_byname_tree = NULL;
#define USERCACHE_BYID_GET_IS_RED(u)  (u)->byid_is_red
#define USERCACHE_BYID_SET_IS_RED(u,v)  (u)->byid_is_red=v
#define USERCACHE_BYID_CMP(a,b, rv) rv = (a->user->id < b->user->id) ? -1 \
                                       : (a->user->id > b->user->id) ? 1 : 0
#define GET_USERCACHE_BYID_TREE() \
  usercache_byid_tree, UserCacheNode *, USERCACHE_BYID_GET_IS_RED, \
  USERCACHE_BYID_SET_IS_RED, byid_parent, byid_left, byid_right, \
  USERCACHE_BYID_CMP

#define USERCACHE_BYNAME_GET_IS_RED(u)  (u)->byname_is_red
#define USERCACHE_BYNAME_SET_IS_RED(u,v)  (u)->byname_is_red=v
#define USERCACHE_BYNAME_CMP(a,b, rv) rv = strcmp(a->user->name,b->user->name)
#define GET_USERCACHE_BYNAME_TREE() \
  usercache_byname_tree, UserCacheNode *, USERCACHE_BYNAME_GET_IS_RED, \
  USERCACHE_BYNAME_SET_IS_RED, byname_parent, byname_left, byname_right, \
  USERCACHE_BYNAME_CMP

UserCacheNode *usercache_oldest, *usercache_newest;
#define GET_USER_CACHE_LIST() \
  UserCacheNode *, usercache_oldest, usercache_newest, older, newer
unsigned usercache_size = 0;
unsigned usercache_max_size = 64;

static void
add_user_to_cache (User *user)
{
  UserCacheNode *ucn = dsk_malloc (sizeof (UserCacheNode));
  ucn->user = user;
  UserCacheNode *conflict;
  GSK_RBTREE_INSERT (GET_USERCACHE_BYID_TREE (), ucn, conflict);
  dsk_assert (conflict == NULL);
  GSK_RBTREE_INSERT (GET_USERCACHE_BYNAME_TREE (), ucn, conflict);
  dsk_assert (conflict == NULL);
  GSK_LIST_APPEND (GET_USER_CACHE_LIST (), ucn);
  usercache_size += 1;
  if (usercache_size > usercache_max_size)
    {
      /* drop oldest */
      UserCacheNode *kill = usercache_oldest;
      GSK_LIST_REMOVE_FIRST (GET_USER_CACHE_LIST ());
      GSK_RBTREE_REMOVE (GET_USERCACHE_BYID_TREE (), kill);
      GSK_RBTREE_REMOVE (GET_USERCACHE_BYNAME_TREE (), kill);
      socnet__user__free_unpacked (kill->user, NULL);
      usercache_size -= 1;
    }
}

static User *
lookup_user_by_id (uint64_t userid)
{
  UserCacheNode *cache_node;
  DskError *error = NULL;
#define BYID_CMP(id_, ucn, rv)  rv = id_ < ucn->user->id ? -1 \
                                   : id_ > ucn->user->id ? +1 : 0
  GSK_RBTREE_LOOKUP_COMPARATOR (GET_USERCACHE_BYID_TREE (), userid, BYID_CMP,
                                cache_node);
#undef BYID_CMP
  if (cache_node != NULL)
    {
      touch_user (cache_node);
      return cache_node->user;
    }

  unsigned userid_len = 8;
  uint8_t *userid_data = (uint8_t *) (&userid); /* ENDIANNESS ISSUE */
  unsigned user_len;
  const uint8_t *user_data;
  if (!dsk_table_lookup (userid_to_user,
                         userid_len, userid_data,
                         &user_len, &user_data,
                         &error))
    {
      if (error)
        {
          dsk_warning ("dsk_table_lookup (userid_to_user) failed: %s",
                       error->message);
          dsk_error_unref (error);
          error = NULL;
        }
      return NULL;
    }

  User *user = socnet__user__unpack (NULL, user_len, user_data);
  if (user == NULL)
    return NULL;
  if (user->id != userid)
    {
      dsk_warning ("unpacking returned user failed: user->id(%llu) != userid(%llu)",
                   user->id, userid);
      socnet__user__free_unpacked (user, NULL);
      return NULL;
    }

  add_user_to_cache (user);

  return user;
}
static User *
lookup_user_by_name (const char *name)
{
  UserCacheNode *cache_node;
#define BYNAME_CMP(nme, ucn, rv)  rv = strcmp (nme, ucn->user->name)
  GSK_RBTREE_LOOKUP_COMPARATOR (GET_USERCACHE_BYNAME_TREE (), name, BYNAME_CMP,
                                cache_node);
#undef BYNAME_CMP
  if (cache_node != NULL)
    {
      touch_user (cache_node);
      return cache_node->user;
    }

  unsigned user_len;
  const uint8_t *user_data;
  DskError *error = NULL;
  if (!dsk_table_lookup (name_to_user,
                         strlen (name), (const uint8_t *) name,
                         &user_len, &user_data,
                         &error))
    {
      if (error)
        {
          dsk_warning ("dsk_table_lookup (name_to_user) failed: %s",
                       error->message);
          dsk_error_unref (error);
          error = NULL;
        }
      return NULL;
    }

  User *user = socnet__user__unpack (NULL, user_len, user_data);
  if (user == NULL)
    return NULL;
  if (strcmp (user->name, name) != 0)
    {
      dsk_warning ("unpacking returned user failed: user->name(%s) != name(%s)",
                   user->name, name);
      socnet__user__free_unpacked (user, NULL);
      return NULL;
    }
  add_user_to_cache (user);
  return user;
}

/* NOTE: takes ownership of 'user' (for possible caching) */
static void
write_user (User *user)
{
  /* serialize */
  unsigned user_len = socnet__user__get_packed_size (user);
  uint8_t *user_data = dsk_malloc (user_len);
  socnet__user__pack (user, user_data);

  /* add to tables */
  dsk_table_insert (name_to_user,
                    strlen (user->name), (const uint8_t *) user->name,
                    user_len, user_data,
                    NULL);
  dsk_table_insert (userid_to_user,
                    8, (const uint8_t *) (&user->id),  /* ENDIANNESS ISSUE */
                    user_len, user_data,
                    NULL);

  dsk_free (user_data);
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
  unsigned i;
  for (i = 0; i < request->n_cgi_variables; i++)
    if (strcmp (request->cgi_variables[i].key, variable_name) == 0)
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
      dsk_http_server_request_respond_error (request,
                                             DSK_HTTP_STATUS_NOT_ACCEPTABLE,
                                             "user already existed");
      return;
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
  /* lookup username: fail if user already exists */
  const char *name = get_cgi_var (request, "name", DSK_FALSE);
  const char *id = NULL;
  if (name == NULL)
    {
      id = get_cgi_var (request, "id", DSK_TRUE);
      if (id == NULL)
        return;
    }
  if (name)
    user = lookup_user_by_name (name);
  else
    {
      uint64_t I = strtoull (id, NULL, 10);
      user = lookup_user_by_id (I);
    }
  if (user == NULL)
    {
      dsk_http_server_request_respond_error (request,
                                             DSK_HTTP_STATUS_NOT_FOUND,
                                             "user not found");
      return;
    }

  /* return JSON */
  DskBuffer buffer = DSK_BUFFER_STATIC_INIT;
  DskHttpServerResponseOptions resp_options = DSK_HTTP_SERVER_RESPONSE_OPTIONS_DEFAULT;
  dsk_buffer_append_string (&buffer, "{\"user\":\"");

  /* quote name */
  ...
  
  dsk_buffer_printf (&buffer, ",\"id\":%llu, friends:[", user->id);
  for (i = 0; i + 1 < users->n_friends; i++)
    dsk_buffer_printf (&buffer, "%llu,", user->friends[i]);
  if (i < users->n_friends)
    dsk_buffer_printf (&buffer, "%llu", user->friends[i]);
  dsk_buffer_printf (&buffer, "]}\n");
  resp_options.source_buffer = &buffer;
  resp_options.content_type = "application/json";
  dsk_http_server_respond (request, &resp_options);
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
cgi_handler__blather        (DskHttpServerRequest *request,
                             void                 *func_data)
{
  /* add message to message index and assign ID */
  ...

  /* break message into words (maybe some bigrams);
     always include empty string */
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

static DskTable *
table_new_or_die (DskTableConfig *config)
{
  DskError *error = NULL;
  DskTable *rv = dsk_table_new (config, &error);
  if (rv == NULL)
    dsk_error ("table creation failed (%s): %s", config->dir, error->message);
  return rv;
}

int
main (int argc, char **argv)
{
  unsigned port;
  dsk_boolean bind_any = DSK_FALSE;
  DskIpAddress bind_address = DSK_IP_ADDRESS_DEFAULT;
  const char *state_dir;
  dsk_cmdline_init ("run a social network search engine",
                    "implement an HTTP server with entry points\n"
                    "suitable for a social network: create users,\n"
                    "create connections, and add messages.\n",
                    NULL,
                    0);
  dsk_cmdline_add_uint ("port", "port to listen on", "PORT",
                        DSK_CMDLINE_MANDATORY, &port);
  dsk_cmdline_add_string ("state-dir", "directory to write state to", "DIR",
                          DSK_CMDLINE_MANDATORY, &state_dir);
  dsk_cmdline_add_boolean ("bind-any", "accept connections from any host", NULL,
                        0, &bind_any);
  dsk_cmdline_process_args (&argc, &argv);

  if (state_dir[0] == 0)
    dsk_die ("state-dir must not be empty");

  /* create tables */
  if (stat (state_dir, &stat_buf) < 0)
    {
      if (errno != ENOENT)
        {
          ...
        }
      if (!dsk_mkdir_recursive (dir, 0777, &error))
        {
          ...
        }
    }
  else if (!S_ISDIR (stat_buf.st_mode))
    {
      ...
    }

  /* create / open tables */
  dir_buf = dsk_malloc (strlen (state_dir) + 128);
  post_dir = dsk_stpcpy (dir_buf, state_dir);
  if (*(post_dir-1) != '/')
    *post_dir++ = '/';

  config.dir = dir_buf;

  strcpy (post_dir, "userid-to-user");
  userid_to_user = table_new_or_die (&config);

  strcpy (post_dir, "name-to-user");
  name_to_user = table_new_or_die (&config);

  strcpy (post_dir, "blatherid-to-blather");
  blatherid_to_blather = table_new_or_die (&config);

  strcpy (post_dir, "word-userid-to-feed");
  config.dir = dir_buf;
  config.merge = merge__feed;
  word_userid_to_feed = table_new_or_die (&config);
  config.merge = NULL;

  server = dsk_http_server_new ();

  static struct {
    const char *pattern;
    DskHttpServerCgiFunc func;
  } handlers[] = {
    { "/add-user\\?.*", cgi_handler__add_user },
    { "/add-connection\\?.*", cgi_handler__add_connection },
    { "/lookup-user\\?.*", cgi_handler__lookup_user },
    { "/blather\\?.*", cgi_handler__blather },
    { "/search\\?.*", cgi_handler__search },
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
