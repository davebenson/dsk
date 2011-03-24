/* DskHttpClientAuth:
   This handles authentification with a wide variety of client needs.

   The simplest, most common case is adding an unconditional user/password
   combo to the request:  that can be done by calling set_info().

   All other users must use dsk_http_client_auth_add_callback()
   which allows for generic handler matching.  It also supports blocking.
   You must take one of four possible action:
     - respond           -- gives a username and password
     - respond_error     -- problem getting username/password
     - respond_na        -- this handler does not apply to the request
     - respond_cancelled -- you trapped cancellation, and the request cancelled
 */

typedef struct _DskHttpClientAuth DskHttpClientAuth;

DskHttpClientAuth *
dsk_http_client_auth_new         (void);


#if 0           /* is this kinda api necessary???? */
/* 'scheme', 'key_value_pairs' and 'caseless_key_value_pairs'
 * are used to match the authentification request.
 * This is overly elaborate -- in most cases you'll have
 * have a username and password, and passing in NULL is appropriate.
 *
 * 'username' and 'password' are applied as needed.
 */
void
dsk_http_client_auth_add_password(DskHttpAuthAgent *agent,
                                  const char       *scheme,
                                  char            **key_value_pairs,
                                  char            **caseless_key_value_pairs,
                                  const char       *username,
                                  const char       *password);
#endif

void
dsk_http_client_auth_set_info    (DskHttpClientAuth *agent,
                                  const char       *username,
                                  const char       *password);

void
dsk_http_client_auth_add_preemptive_basic (DskHttpClientAuth *agent,
                                           const char *realm,
                                           const char *username,
                                           const char *password);


void dsk_http_client_auth_enable_caching  (DskHttpClientAuth *agent);

/* --- dsk_http_client_auth_add_callback() --- */
/* - this allows for custom response handlers,
     including asking an actual human.  Its API is elaborate to 
     allow an arbitrary delay between the request and when
     the respond is given.
 */
typedef struct _DskHttpClientAuthRequest DskHttpClientAuthRequest;
struct _DskHttpClientAuthRequest
{
  /* request info */
  char **key_value_pairs;

  /*< private data follows >*/
  DskHttpClientAuth *agent;
  //... state
};
typedef void (*DskHttpClientAuthHandler) (DskHttpClientAuthRequest *request,
                                          void       *handler_data);


/* Each time the handler is invoked, it must eventually to be
 * followed up with a call to one of
 * dsk_http_client_auth_request_{respond,respond_fail,respond_na}.
 */
void
dsk_http_client_auth_add_callback(DskHttpClientAuth        *agent,
                                  DskHttpClientAuthHandler handler,
                                  void                    *handler_data,
                                  DskDestroyNotify         handler_destroy);


/* --- These functions are only for use during or after a handler registered
       with dsk_http_client_auth_add_callback() is invoked --- */
void
dsk_http_client_auth_request_respond   (DskHttpClientAuthRequest *request,
                                        const char               *username,
                                        const char               *password);

void
dsk_http_client_auth_request_respond_fail (DskHttpClientAuthRequest *request,
                                           const char               *message);

void
dsk_http_client_auth_request_respond_na   (DskHttpClientAuthRequest *request);


/* cancellation handling API */
void dsk_http_client_auth_request_trap_cancellation
                         (DskHttpClientAuthRequest *request,
                          DskHttpClientAuthHandler cancellation_handler);

void
dsk_http_client_auth_request_respond_cancelled (DskHttpClientAuthRequest*);



