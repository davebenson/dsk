/* TODO: handle big-endian machines (search for ENDIANNESS ISSUE) */
#include <alloca.h>
#include "../dsk.h"
#include "generated/socnet.pb-c.h"
#include "../gskrbtreemacros.h"
#include "../dsk-list-macros.h"
#include "../gskqsortmacro.h"
#include <string.h>
#include <stdio.h>
#include <time.h>

typedef Socnet__User User;
typedef Socnet__Blather Blather;
typedef Socnet__Feed Feed;

/* --- globals --- */
static DskTable *name_to_user;
static DskTable *userid_to_user;
static DskTable *userid_word_to_feed;
static DskTable *blatherid_to_blather;
static uint64_t *next_user_id;          /* mmapped */
static uint64_t *next_blather_id;          /* mmapped */


/* --- sanity limits --- */
#define MAX_TERMS_IN_QUERY                      16


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
#define USER_CACHE_BYID_GET_IS_RED(u)  (u)->byid_is_red
#define USER_CACHE_BYID_SET_IS_RED(u,v)  (u)->byid_is_red=v
#define USER_CACHE_BYID_CMP(a,b, rv) rv = (a->user->id < b->user->id) ? -1 \
                                       : (a->user->id > b->user->id) ? 1 : 0
#define GET_USER_CACHE_BYID_TREE() \
  usercache_byid_tree, UserCacheNode *, USER_CACHE_BYID_GET_IS_RED, \
  USER_CACHE_BYID_SET_IS_RED, byid_parent, byid_left, byid_right, \
  USER_CACHE_BYID_CMP

#define USER_CACHE_BYNAME_GET_IS_RED(u)  (u)->byname_is_red
#define USER_CACHE_BYNAME_SET_IS_RED(u,v)  (u)->byname_is_red=v
#define USER_CACHE_BYNAME_CMP(a,b, rv) rv = strcmp(a->user->name,b->user->name)
#define GET_USER_CACHE_BYNAME_TREE() \
  usercache_byname_tree, UserCacheNode *, USER_CACHE_BYNAME_GET_IS_RED, \
  USER_CACHE_BYNAME_SET_IS_RED, byname_parent, byname_left, byname_right, \
  USER_CACHE_BYNAME_CMP

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
  GSK_RBTREE_INSERT (GET_USER_CACHE_BYID_TREE (), ucn, conflict);
  dsk_assert (conflict == NULL);
  GSK_RBTREE_INSERT (GET_USER_CACHE_BYNAME_TREE (), ucn, conflict);
  dsk_assert (conflict == NULL);
  GSK_LIST_APPEND (GET_USER_CACHE_LIST (), ucn);
  usercache_size += 1;
  if (usercache_size > usercache_max_size)
    {
      /* drop oldest */
      UserCacheNode *kill = usercache_oldest;
      GSK_LIST_REMOVE_FIRST (GET_USER_CACHE_LIST ());
      GSK_RBTREE_REMOVE (GET_USER_CACHE_BYID_TREE (), kill);
      GSK_RBTREE_REMOVE (GET_USER_CACHE_BYNAME_TREE (), kill);
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
  GSK_RBTREE_LOOKUP_COMPARATOR (GET_USER_CACHE_BYID_TREE (), userid, BYID_CMP,
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
  GSK_RBTREE_LOOKUP_COMPARATOR (GET_USER_CACHE_BYNAME_TREE (), name, BYNAME_CMP,
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

static void
write_blather (Blather *blather)
{
  /* serialize */
  unsigned blather_len = socnet__blather__get_packed_size (blather);
  uint8_t *blather_data = dsk_malloc (blather_len);
  socnet__blather__pack (blather, blather_data);

  /* add to table */
  dsk_table_insert (blatherid_to_blather,
                    8, (const uint8_t *) (&blather->id),  /* ENDIANNESS ISSUE */
                    blather_len, blather_data,
                    NULL);

  dsk_free (blather_data);
}

typedef struct _FeedCacheNode FeedCacheNode;
struct _FeedCacheNode
{
  uint64_t user_id;
  char *term;
  Feed *feed;

  FeedCacheNode *left, *right, *parent;
  uint8_t is_red;

  unsigned lock_count;

  FeedCacheNode *older, *newer;
};
FeedCacheNode *feedcache_tree = NULL;
#define FEED_CACHE_GET_IS_RED(u)  (u)->is_red
#define FEED_CACHE_SET_IS_RED(u,v)  (u)->is_red=v
#define FEED_CACHE_CMP(a,b, rv) rv = (a->user_id < b->user_id) ? -1 \
                                       : (a->user_id > b->user_id) ? 1 \
                                       : strcmp(a->term, b->term)
 
#define GET_FEED_CACHE_TREE() \
  feedcache_tree, FeedCacheNode *, FEED_CACHE_GET_IS_RED, \
  FEED_CACHE_SET_IS_RED, parent, left, right, \
  FEED_CACHE_CMP


FeedCacheNode *feedcache_oldest, *feedcache_newest;
#define GET_FEED_CACHE_LIST() \
  FeedCacheNode *, feedcache_oldest, feedcache_newest, older, newer
unsigned feedcache_size = 0;
unsigned feedcache_max_size = 512;

static FeedCacheNode *
lock_term_userid_feed (uint64_t    user_id,
                       const char *term)
{

  FeedCacheNode tmp;
  FeedCacheNode *node;
  DskError *error = NULL;
  tmp.user_id = user_id;
  tmp.term = (char*) term;
  GSK_RBTREE_LOOKUP (GET_FEED_CACHE_TREE(), &tmp, node);
  if (node != NULL)
    {
      if (node->lock_count == 0)
        GSK_LIST_REMOVE (GET_FEED_CACHE_LIST (), node);
      node->lock_count += 1;
    }
  else
    {
      unsigned term_len = strlen (term);
      unsigned key_len = 8 + term_len;
      uint8_t *key_data = alloca (key_len);
      unsigned value_len;
      const uint8_t *value_data;

      node = dsk_malloc (sizeof (FeedCacheNode) + term_len + 1);
      node->term = (char*)(node+1);
      strcpy (node->term, term);
      node->user_id = user_id;
      memcpy (key_data, &user_id, 8);           /* ENDIANNESS ISSUE */
      memcpy (key_data + 8, term, term_len);
      if (!dsk_table_lookup (userid_word_to_feed, 
                             key_len, key_data,
                             &value_len, &value_data,
                             &error))
        {
          if (error)
            {
              dsk_warning ("error looking up feed for user %llu, keyword %s",
                           user_id, term);
              dsk_error_unref (error);
              error = NULL;
            }
          node->feed = NULL;
        }
      node->feed = socnet__feed__unpack (NULL, value_len, value_data);
      node->lock_count = 1;
      FeedCacheNode *conflict;
      GSK_RBTREE_INSERT (GET_FEED_CACHE_TREE (), node, conflict);
      dsk_assert (conflict == NULL);
    }
  return node;
}

/* --- blather cache --- */
typedef struct _BlatherCacheNode BlatherCacheNode;
struct _BlatherCacheNode
{
  Blather *blather;

  BlatherCacheNode *left, *right, *parent;
  uint8_t is_red;

  unsigned lock_count;

  BlatherCacheNode *older, *newer;
};
BlatherCacheNode *blathercache_tree = NULL;
#define BLATHER_CACHE_GET_IS_RED(u)  (u)->is_red
#define BLATHER_CACHE_SET_IS_RED(u,v)  (u)->is_red=v
#define BLATHER_CACHE_CMP(a,b, rv) rv = (a->blather->id < b->blather->id) ? -1 \
                                       : (a->blather->id > b->blather->id) ? 1 \
                                       : 0
 
#define GET_BLATHER_CACHE_TREE() \
  blathercache_tree, BlatherCacheNode *, BLATHER_CACHE_GET_IS_RED, \
  BLATHER_CACHE_SET_IS_RED, parent, left, right, \
  BLATHER_CACHE_CMP


BlatherCacheNode *blathercache_oldest, *blathercache_newest;
#define GET_BLATHER_CACHE_LIST() \
  BlatherCacheNode *, blathercache_oldest, blathercache_newest, older, newer
unsigned blathercache_size = 0;
unsigned blathercache_max_size = 512;

static Blather *
lock_blather (uint64_t blather_id)
{
  BlatherCacheNode *node;
#define BYID_CMP(bid, bcn, rv)  rv = bid < bcn->blather->id ? -1 : bid > bcn->blather->id ? 1 : 0
  GSK_RBTREE_LOOKUP_COMPARATOR (GET_BLATHER_CACHE_TREE(), blather_id, BYID_CMP, node);
#undef BYID_CMP
  if (node != NULL)
    {
      if (node->lock_count == 0)
        GSK_LIST_REMOVE (GET_BLATHER_CACHE_LIST (), node);
      node->lock_count += 1;
    }
  else
    {
      unsigned key_len = 8;
      uint8_t *key_data = (uint8_t *) &blather_id; /* ENDIANNESS ISSUE */
      unsigned value_len;
      const uint8_t *value_data;
      DskError *error = NULL;

      if (!dsk_table_lookup (userid_word_to_feed, 
                             key_len, key_data,
                             &value_len, &value_data,
                             &error))
        {
          if (error)
            {
              dsk_warning ("error looking up blather %llu", blather_id);
              dsk_error_unref (error);
              error = NULL;
            }
          return NULL;
        }
      Blather *blather = socnet__blather__unpack (NULL, value_len, value_data);
      node = dsk_malloc (sizeof (BlatherCacheNode));
      node->blather = blather;
      node->lock_count = 1;
      BlatherCacheNode *conflict;
      GSK_RBTREE_INSERT (GET_BLATHER_CACHE_TREE (), node, conflict);
      dsk_assert (conflict == NULL);
    }
  return node->blather;
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
  user->id = *next_user_id;
  *next_user_id += 1;

  /* respond with user-id */
  DskHttpServerResponseOptions options = DSK_HTTP_SERVER_RESPONSE_OPTIONS_DEFAULT;
  options.content_type = "text/plain";
  char buf[256];
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
  User *user;
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
  DskBuffer buffer = DSK_BUFFER_INIT;
  DskHttpServerResponseOptions resp_options = DSK_HTTP_SERVER_RESPONSE_OPTIONS_DEFAULT;
  dsk_buffer_append_string (&buffer, "{\"user\":\"");

  /* quote name (ADD DskBuffer function!!!) */
  {
    const char *at = user->name;
    while (*at)
      {
        if (*at == '"' || *at == '\\')
          dsk_buffer_append_byte (&buffer, '\\');
        dsk_buffer_append_byte (&buffer, *at);
        at++;
      }
  }
  
  dsk_buffer_printf (&buffer, ",\"id\":%llu, friends:[", user->id);
  unsigned i;
  for (i = 0; i + 1 < user->n_friends; i++)
    dsk_buffer_printf (&buffer, "%llu,", user->friends[i]);
  if (i < user->n_friends)
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
  User *users[2];
  const char *id_cgis[2] = {"a_id", "b_id"};
  const char *name_cgis[2] = {"a_name", "b_name"};
  unsigned i;
  for (i = 0; i < 2; i++)
    {
      const char *str;
      if ((str=get_cgi_var (request, id_cgis[i], DSK_FALSE)) != NULL)
        {
          uint64_t id = strtoull (str, NULL, 10);
          users[i] = lookup_user_by_id (id);
          if (users[i] == NULL)
            {
              dsk_http_server_request_respond_error (request,
                                                     DSK_HTTP_STATUS_NOT_FOUND,
                                                     "user id not found");
              return;
            }
        }
      else if ((str=get_cgi_var (request, name_cgis[i], DSK_TRUE)) != NULL)
        {
          users[i] = lookup_user_by_name (str);
          if (users[i] == NULL)
            {
              dsk_http_server_request_respond_error (request,
                                                     DSK_HTTP_STATUS_NOT_FOUND,
                                                     "user name not found");
              return;
            }
        }
      else
        return;
    }

  /* find out if they are already friends with eachother. */
  dsk_boolean a_friendswith_b, b_friendswith_a;
  for (i = 0; i < users[0]->n_friends; i++)
    if (users[0]->friends[i] == users[1]->id)
      break;
  a_friendswith_b = (i < users[0]->n_friends);
  for (i = 0; i < users[1]->n_friends; i++)
    if (users[1]->friends[i] == users[0]->id)
      break;
  b_friendswith_a = (i < users[1]->n_friends);

  if (a_friendswith_b && b_friendswith_a)
    {
      /* error: already friends */
      dsk_http_server_request_respond_error (request,
                                             DSK_HTTP_STATUS_NOT_FOUND,
                                             "already friends");
      return;
    }
  dsk_warn_if_fail(!a_friendswith_b, "friendship in this network is symmetric");
  dsk_warn_if_fail(!b_friendswith_a, "friendship in this network is symmetric");

  if (!a_friendswith_b)
    {
      /* insert user */
      users[0]->friends = dsk_realloc (users[0]->friends, 8 * (users[0]->n_friends+1));
      users[0]->friends[users[0]->n_friends++] = users[1]->id;
      write_user (users[0]);
    }
  if (!b_friendswith_a)
    {
      /* insert user */
      users[1]->friends = dsk_realloc (users[1]->friends, 8 * (users[1]->n_friends+1));
      users[1]->friends[users[1]->n_friends++] = users[0]->id;
      write_user (users[1]);
    }

  /* respond vapidly, but successfully */
  DskHttpServerResponseOptions options = DSK_HTTP_SERVER_RESPONSE_OPTIONS_DEFAULT;
  options.content_type = "text/plain";
  options.content_body = "";
  options.content_length = 0;
  dsk_http_server_respond (request, &options);
}

#define MAX_WORD_SIZE                1024

typedef void (*TokenCallback) (const char *token,
                               void       *callback_data);

typedef struct _BlatherToken BlatherToken;
struct _BlatherToken
{
  char *normalized;

  /* location w/i the original message (byte offsets) */
  unsigned start, length;
};


static BlatherToken *
tokenize_blather (const char   *message,
                  DskMemPool   *pool,
                  unsigned     *n_tokens_out)
{
  /* TODO: handle non-ascii */
  unsigned rv_alloced = 16;
  BlatherToken *rv = dsk_malloc (sizeof (BlatherToken) * rv_alloced);
  unsigned n_rv = 0;
  const char *message_begin = message;
  while (*message)
    {
      if (dsk_ascii_is_alnum (*message))
        {
          const char *start = message;
          ++message;
          while (dsk_ascii_isalnum (*message))
            message++;
          if (message - start < MAX_WORD_SIZE)
            {
              const char *in = start;
              char *word = dsk_mem_pool_alloc_unaligned (pool, message - start + 1);
              char *out = word;
              while (in < message)
                {
                  if (dsk_ascii_isupper (*in))
                    *out = *in + ('a' - 'A');
                  else
                    *out = *in;
                  out++;
                  in++;
                }
              *out = 0;
              if (rv_alloced == n_rv)
                {
                  rv = dsk_realloc (rv, rv_alloced * 2 * sizeof (char**));
                  rv_alloced *= 2;
                }
              rv[n_rv].normalized = word;
              rv[n_rv].start = start - message_begin;
              rv[n_rv].length = in - start;
              n_rv++;
            }
        }
      else
        message++;
    }
  *n_tokens_out = n_rv;
  return rv;
}


static void
handle_message_token (const char *word,
                      uint64_t    blather_id,
                      User       *user)
{
  DskError *error = NULL;
  unsigned word_len = strlen (word);
  uint8_t buf[MAX_WORD_SIZE + 8];
  uint8_t feed_packed[256];
  unsigned feed_packed_len;
  Feed feed = SOCNET__FEED__INIT;
  unsigned i;
  char mem_pool_slab[1024];
  feed.n_blatherids = 1;
  feed.blatherids = &blather_id;
  feed_packed_len = socnet__feed__pack (&feed, feed_packed);

  memcpy (buf + 8, word, word_len);
  for (i = 0; i < user->n_friends; i++)
    {
      memcpy (buf, user->friends + i, 8);
      if (!dsk_table_add (userid_word_to_feed,
                          word_len + 8, buf, feed_packed_len, feed_packed,
                          &error))
        {
          dsk_warning ("index table-add: %s", error->message);
          dsk_error_unref (error);
          error = NULL;
        }
    }
  memcpy (buf, user->friends + i, 8);
  if (!dsk_table_add (userid_word_to_feed,
                      word_len + 8, buf, feed_packed_len, feed_packed,
                      &error))
    {
      dsk_warning ("index table-add: %s", error->message);
      dsk_error_unref (error);
      error = NULL;
    }
}

static void
cgi_handler__blather        (DskHttpServerRequest *request,
                             void                 *func_data)
{
  const char *author_id_str, *text;
  author_id_str = get_cgi_var (request, "authorid", DSK_TRUE);
  if (author_id_str == NULL)
    return;
  text = get_cgi_var (request, "text", DSK_TRUE);
  if (text == NULL)
    return;

  uint64_t author_id;
  author_id = strtoull (author_id_str, NULL, 10);
  User *author;
  author = lookup_user_by_id (author_id);
  if (author == NULL)
    {
      dsk_http_server_request_respond_error (request,
                                             DSK_HTTP_STATUS_NOT_FOUND,
                                             "author not found");
      return;
    }

  /* add message to message index and assign ID */
  Blather blather = SOCNET__BLATHER__INIT;
  blather.id = *next_blather_id;
  *next_blather_id += 1;
  blather.author_id = strtoull (author, NULL, 10);
  blather.text = (char*) text;
  blather.timestamp = time (NULL);
  write_blather (&blather);

  /* break message into words (maybe some bigrams);
   * always include empty string;
   * add all friend/word pairs to index */
  char mem_pool_slab[1024];
  DskMemPool mem_pool;
  dsk_mem_pool_init_buf (&mem_pool, sizeof (mem_pool_slab), mem_pool_slab);
  unsigned n_tokens;
  BlatherToken *tokens = tokenize_blather (text, &mem_pool, &n_tokens);
#define COMPARE_STRINGS(a,b,rv) rv = strcmp (a.normalized,b.normalized)
  GSK_QSORT (tokens, BlatherToken, n_tokens, COMPARE_STRINGS);
#undef COMPARE_STRINGS
  unsigned i;
  if (n_tokens > 1)
    {
      unsigned o = 0;
      for (i = 1; i < n_tokens; i++)
        {
          if (strcmp (tokens[o].normalized, tokens[i].normalized) != 0)
            tokens[++o] = tokens[i];
        }
      n_tokens = o + 1;
    }
  handle_message_token ("", blather.id, author);
  for (i = 0; i < n_tokens; i++)
    handle_message_token (tokens[i].normalized, blather.id, author);

  /* respond with blather id */
  char resp_buf[64];
  snprintf (resp_buf, sizeof (resp_buf), "%llu", blather.id);

  DskHttpServerResponseOptions options = DSK_HTTP_SERVER_RESPONSE_OPTIONS_DEFAULT;
  options.content_type = "text/plain";
  options.content_body = resp_buf;
  options.content_length = strlen (resp_buf);
  dsk_http_server_respond (request, &options);
  dsk_free (tokens);
  dsk_mem_pool_clear (&mem_pool);
}

typedef struct _ScoredBlather ScoredBlather;
struct _ScoredBlather
{
  double score;
  Blather *blather;
  DskJsonValue *listing;
};

typedef struct _QueryPart QueryPart;
struct _QueryPart
{
  char *normalized;             /* lowercased, no punctuation */
  uint64_t total_occurances;
  Socnet__WordInfo *word_info;  /* may be NULL if unknown */
};

typedef struct _Query Query;
struct _Query
{
  unsigned n_parts;
  QueryPart *parts;
};

static dsk_boolean
score_blather (Blather       *blather,
               double        *score_out,
               DskJsonValue **json_out,
               Query         *query)
{
  /* find words */
  char mem_pool_slab[2048];
  DskMemPool mem_pool;
  BlatherToken *words;
  unsigned n_tokens;
  dsk_mem_pool_init_buf (&mem_pool, sizeof (mem_pool_slab), mem_pool_slab);
  words = tokenize_blather (blather->text, &mem_pool, &n_tokens);
  if (words == NULL)
    return DSK_FALSE;

  /* tf/idf */
  ...

  /* compute boldfaced blather */
  ...

  /* make json */
  ...
}

static void
cgi_handler__search    (DskHttpServerRequest *request,
                        void                 *func_data)
{
  const char *user_id_str, *text;
  user_id_str = get_cgi_var (request, "userid", DSK_TRUE);
  if (user_id_str == NULL)
    return;
  text = get_cgi_var (request, "text", DSK_TRUE);
  if (text == NULL)
    return;
  User *user = lookup_user_by_id (strtoull (user_id_str, NULL, 10));
  if (user == NULL)
    {
      dsk_http_server_request_respond_error (request,
                                             DSK_HTTP_STATUS_NOT_FOUND,
                                             "user id not found");
      return;
    }
  DskMemPool mem_pool;
  uint8_t slab[1024];
  dsk_mem_pool_init_buf (&mem_pool, sizeof (slab), slab);
  unsigned n_terms;
  BlatherToken *terms = tokenize_blather (text, &mem_pool, &n_terms);
  unsigned i;

  /* lookup all terms */
  FeedCacheNode **term_feeds = alloca (sizeof(FeedCacheNode*) * n_terms);
  unsigned total_n_blatherids = 0;
  for (i = 0; i < n_terms; i++)
    {
      term_feeds[i] = lock_term_userid_feed (user->id, terms[i]);
      if (term_feeds[i]->feed != NULL)
        total_n_blatherids += term_feeds[i]->feed->n_blatherids;
    }
  uint64_t *all_blatherids = dsk_malloc (total_n_blatherids * sizeof(uint64_t));
  unsigned at = 0;
  for (i = 0; i < n_terms; i++)
    if (term_feeds[i]->feed != NULL)
      {
        unsigned N = term_feeds[i]->feed->n_blatherids;
        memcpy (all_blatherids + at,
                term_feeds[i]->feed->blatherids,
                N * sizeof (uint64_t));
        at += N;
      }


  /* Sort descending the blatherids */
#define COMPARE_UINT64_DESC(a,b,rv) rv = a<b ? +1 : a>b ? -1 : 0;
  GSK_QSORT (all_blatherids, uint64_t, at, COMPARE_UINT64_DESC);
#undef COMPARE_UINT64_DESC

  /* unique blatherids */
  if (at > 0)
    {
      unsigned o = 0;
      for (i = 1; i < at; i++)
        if (all_blatherids[o] != all_blatherids[i])
          all_blatherids[++o] = all_blatherids[i];
      at = o + 1;
    }

  total_n_blatherids = at;
  ScoredBlather *scored_blathers = dsk_malloc (sizeof (ScoredBlather) * at);
  at = 0;
  unsigned n_scored = 0;

  /* lookup blathers until we have enough */
  DskBuffer response = DSK_BUFFER_INIT;
  unsigned n_blathers_scored;
  for (n_blathers_scored = 0; n_blathers_scored < at; n_blathers_scored++)
    {
      Blather *blather = lock_blather (all_blatherids[n_blathers_scored]);
      if (blather == NULL)
        continue;
      if (!score_blather (blather, &score, &json_node, query))
        {
          unlock_blather (blather);
        }
      else
        {
          scored_blathers[n_scored].score = score;
          scored_blathers[n_scored].blather = blather;
          scored_blathers[n_scored].json_node = json_node;
          n_scored++;
        }

      if (n_scored > n_desired && i > min_examine)
        {
          sort_scored_blathers_desc (n_scored, scored_blathers);
          // TODO: dedup
          if (n_scored > n_desired)
            {
              n_blathers_scored++;
              goto found_results;
            }
        }
    }
  sort_scored_blathers_desc (n_scored, scored_blathers);
found_results:
  DskJsonMember members[2];
  unsigned n_members = 0;
  if (debug)
    {
      DskJsonMember debmems[10];
      unsigned n_debmems = 0;
      DskJsonValue **terminfos = alloca (sizeof (DskJsonMember) * n_terms);
      debmems[n_debmems].name = "blathers_from_all_terms";
      debmems[n_debmems].value = dsk_json_value_new_number (orig_total_n_blathers);
      n_debmems++;
      debmems[n_debmems].name = "unique_blathers_from_all_terms";
      debmems[n_debmems].value = dsk_json_value_new_number (total_n_blathers);
      n_debmems++;
      debmems[n_debmems].name = "blathers_scored";
      debmems[n_debmems].value = dsk_json_value_new_number (n_blathers_scored);
      n_debmems++;
      for (i = 0; i < n_terms; i++)
        {
          DskJsonMember terminfo[3];
          terminfo[0].name = "term";
          terminfo[0].value = dsk_json_value_new_string (term_feeds[i].term);
          terminfo[1].name = "found";
          terminfo[1].value = dsk_json_value_new_boolean (term_feeds[i].feed != NULL);
          terminfo[2].name = "n";
          terminfo[2].value = dsk_json_value_new_number (term_feeds[i].feed ? term_feeds[i].feed->n_blatherids : 0);
          terminfos[i] = dsk_json_value_new_object (3, terminfo);
        }
      debmems[n_debmems].name = "term_infos";
      debmems[n_debmems].value = dsk_json_value_new_array (n_terms, terminfos);
      n_debmems++;
      members[n_members].name = "debug";
      members[n_members].value = dsk_json_value_new_object (n_debmems, debmems);
      n_members++;
    }
  {
    /* render json array */
    unsigned n = DSK_MIN (n_desired, n_scored);
    DskJsonValue **listings = dsk_malloc (sizeof(DskJsonValue*) * n);
    for (i = 0; i < n; i++)
      listings[i] = scored_blathers[i].json_node;
    members[n_members].name = "results";
    members[n_members].value = dsk_json_value_new_array (n, listings);
    n_members++;
    dsk_free (listings);
  }

  /* Create total result object. */
  DskJsonValue *value = dsk_json_value_new_object (n_members, members);
  dsk_json_value_to_buffer (value, &buffer);
  dsk_json_value_free (value);
  dsk_buffer_append_byte (&buffer, '\n');

  /* cleanup */
  for (i = 0; i < n_terms; i++)
    unlock_term_userid_feed (term_feeds[i]);
  for (i = 0; i < n_scored; i++)
    unlock_blather (scored_blathers[i].blather);
  dsk_free (scored_blathers);

  dsk_free (terms);
  dsk_mem_pool_clear (&mem_pool);
  dsk_free (all_blatherids);

  /* do HTTP response */
  DskHttpResponseOptions resp_options = DSK_HTTP_RESPONSE_OPTIONS_DEFAULT;
  resp_options.content_type = "application/json";
  resp_options.source_buffer = &buffer;
  dsk_http_server_respond (request, &options);
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
          dsk_error ("%s could not be accessed: %s",
                     state_dir, strerror (errno));
        }
      if (!dsk_mkdir_recursive (state_dir, 0777, &error))
        {
          dsk_error ("%s could not be make: %s",
                     state_dir, error->message);
        }
    }
  else if (!S_ISDIR (stat_buf.st_mode))
    {
      dsk_error ("%s was not a directory", state_dir);
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
