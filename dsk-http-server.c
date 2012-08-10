#include <string.h>
#include <stdio.h>
#include <stdlib.h>

/* These headers are exclusively to call open(2)/fstat(2):
   they are used for serving a file and could easily be moved
   to dsk-http-server-stream -- except maybe for error handling */
   
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include "dsk.h"

#include "dsk-list-macros.h"


/* === Request Matching Infrastructure === */
/* a compiled test of a header */
typedef struct _MatchTester MatchTester;
typedef struct _MatchTesterStandard MatchTesterStandard;
typedef struct _RealServerRequest RealServerRequest;

typedef void (*FunctionPointer)();

struct _MatchTester
{
  dsk_boolean (*test)(MatchTester *tester,
                      DskHttpServerRequest *request);
  void        (*destroy) (MatchTester *tester);
};

typedef struct
{
  DskHttpServerMatchType match_type;
  DskPattern *test;
} MatchTypeAndPattern;
struct _MatchTesterStandard
{
  MatchTester base;
  unsigned n_types;
  MatchTypeAndPattern types[1];
};

struct _MatchTesterVerb
{
  MatchTester base;
  DskHttpVerb verb;
};

/* an uncompiled list of match information */
typedef struct _MatchTestList MatchTestList;
struct _MatchTestList
{
  DskHttpServerMatchType match_type;
  char *pattern_str;
  MatchTestList *prev;
};

typedef struct _ServerStream ServerStream;
struct _ServerStream
{
  DskHttpServerBindInfo *bind_info;
  DskHttpServerStream *http_stream;
  DskHookTrap *trap;            /* http_stream->request_available trap */
  ServerStream *prev, *next;
  DskIpAddress ip_address;
  unsigned ip_port;
};

/* info about a listening port */
struct _DskHttpServerBindInfo
{
  DskHttpServer *server;
  DskHttpServerBindInfo *next;
  DskHttpServerStreamOptions server_stream_options;
  DskOctetListener *listener;

  RealServerRequest *first_request, *last_request;
  ServerStream *first_stream, *last_stream;

  dsk_boolean is_local;
  unsigned bind_port;
  char *bind_local_path;
};

#define GET_BIND_INFO_STREAM_LIST(bind_info) \
  ServerStream *, (bind_info)->first_stream, (bind_info)->last_stream, \
  prev, next

typedef struct _Handler Handler;
typedef enum
{
  HANDLER_TYPE_STREAMING_POST_DATA,
  HANDLER_TYPE_CGI,
  HANDLER_TYPE_WEBSOCKET
} HandlerType;

struct _Handler
{
  HandlerType handler_type;
  FunctionPointer handler;
  void *handler_data;
  DskHookDestroy handler_destroy;
  Handler *next;
};


typedef struct _MatchTestNode MatchTestNode;
struct _MatchTestNode
{
  dsk_boolean under_construction;
  MatchTestList *test_list;     /* only while under_construction */
    
  MatchTester *tester;  /* created on demand, freed when test_list modded */

  MatchTestNode *parent;
  MatchTestNode *first_child, *last_child;
  MatchTestNode *prev_sibling, *next_sibling;

  Handler *first_handler, *last_handler;
};
#define GET_HANDLER_QUEUE(match_test_node)      \
  Handler*,                                     \
  (match_test_node)->first_handler,             \
  (match_test_node)->last_handler,              \
  next

struct _DskHttpServer
{
  DskObject base_instance;
  MatchTestNode top;
  MatchTestNode *current;
  DskHttpServerStreamOptions server_stream_options;
  DskHttpServerBindInfo *bind_infos;
};

static void
dsk_http_server_init (DskHttpServer *server)
{
  static DskHttpServerStreamOptions sso = DSK_HTTP_SERVER_STREAM_OPTIONS_INIT;
  server->top.under_construction = DSK_TRUE;
  server->server_stream_options = sso;
  server->current = &server->top;
}

static void dsk_http_server_finalize (DskHttpServer *server);

typedef struct _DskHttpServerClass DskHttpServerClass;
struct _DskHttpServerClass
{
  DskObjectClass base_class;
};

DSK_OBJECT_CLASS_DEFINE_CACHE_DATA(DskHttpServer);
static DskHttpServerClass dsk_http_server_class =
{
  DSK_OBJECT_CLASS_DEFINE (DskHttpServer, &dsk_object_class,
                           dsk_http_server_init,
                           dsk_http_server_finalize)
};

DskHttpServer *
dsk_http_server_new (void)
{
  return dsk_object_new (&dsk_http_server_class);
}

void dsk_http_server_add_match                 (DskHttpServer        *server,
                                                DskHttpServerMatchType type,
                                                const char           *pattern)
{
  MatchTestList *new_node = dsk_malloc (sizeof (MatchTestList) + strlen (pattern) + 1);
  new_node->pattern_str = (char*)(new_node + 1);
  strcpy (new_node->pattern_str, pattern);
  new_node->match_type = type;
  new_node->prev = server->current->test_list;
  server->current->test_list = new_node;
  if (server->current->tester != NULL)
    {
      server->current->tester->destroy (server->current->tester);
      server->current->tester = NULL;
    }
}

void dsk_http_server_match_save                (DskHttpServer        *server)
{
  MatchTestNode *node = DSK_NEW0 (MatchTestNode);
  node->under_construction = DSK_TRUE;

  node->parent = server->current;

  /* add to sibling list of current */
  node->prev_sibling = server->current->last_child;
  node->next_sibling = NULL;
  if (server->current->first_child == NULL)
    server->current->first_child = node;
  else
    server->current->last_child->next_sibling = node;
  server->current->last_child = node;

  server->current = node;
}

static dsk_boolean
match_tester_standard_test (MatchTester *tester,
                            DskHttpServerRequest *request)
{
  MatchTesterStandard *std = (MatchTesterStandard *) tester;
  unsigned i;
  for (i = 0; i < std->n_types; i++)
    {
      const char *test_str = NULL;
      char tmp_buf[64];
      switch (std->types[i].match_type)
        {
        case DSK_HTTP_SERVER_MATCH_PATH:
          test_str = request->request_header->path;
          break;
        case DSK_HTTP_SERVER_MATCH_HOST:
          test_str = request->request_header->host;
          break;
        case DSK_HTTP_SERVER_MATCH_USER_AGENT:
          test_str = request->request_header->user_agent;
          break;
        case DSK_HTTP_SERVER_MATCH_BIND_PORT:
          if (!request->bind_info->is_local)
            {
              snprintf (tmp_buf, sizeof (tmp_buf), "%u", 
                        request->bind_info->bind_port);
              test_str = tmp_buf;
            }
          break;
        case DSK_HTTP_SERVER_MATCH_BIND_PATH:
          if (!request->bind_info->is_local)
            test_str = request->bind_info->bind_local_path;
          break;
        case DSK_HTTP_SERVER_MATCH_VERB:
          test_str = dsk_http_verb_name (request->request_header->verb);
          break;
        }
      if (test_str == NULL)
        return DSK_FALSE;
      if (!dsk_pattern_match (std->types[i].test, test_str))
        return DSK_FALSE;
    }
  return DSK_TRUE;
}

void
dsk_http_server_add_match_verb            (DskHttpServer        *server,
                                           DskHttpVerb           verb)
{
  dsk_http_server_add_match (server,
                             DSK_HTTP_SERVER_MATCH_VERB,
                             dsk_http_verb_name (verb));
}

static void
match_tester_standard_destroy (MatchTester *tester)
{
  MatchTesterStandard *std = (MatchTesterStandard *) tester;
  unsigned i;
  for (i = 0; i < std->n_types; i++)
    dsk_pattern_free (std->types[i].test);
  dsk_free (std);
}

static MatchTester *
create_tester (MatchTestList *list)
{
  unsigned max_run_length, n_unique_types;
  MatchTestList *at;
  MatchTesterStandard *tester;
  DskPatternEntry *patterns;
  if (list == NULL)
    return NULL;
#define GET_MATCH_TEST_LIST(list) MatchTestList*, list, prev
#define COMPARE_MATCH_TEST_LIST_BY_TEST_TYPE(a,b,rv) \
  rv = a->match_type < b->match_type ? -1 : a->match_type > b->match_type ? 1 : 0;
  DSK_STACK_SORT (GET_MATCH_TEST_LIST (list),
                  COMPARE_MATCH_TEST_LIST_BY_TEST_TYPE);
#undef COMPARE_MATCH_TEST_LIST_BY_TEST_TYPE
#undef GET_MATCH_TEST_LIST

  /* Count the number of unique types,
     and the max count of any given type. */
  max_run_length = 0;
  n_unique_types = 0;
  for (at = list; at; )
    {
      MatchTestList *run_end = at->prev;
      unsigned run_length = 1;
      while (run_end && run_end->match_type == at->match_type)
        {
          run_length++;
          run_end = run_end->prev;
        }
      if (run_length > max_run_length)
        max_run_length = run_length;
      n_unique_types++;
      at = run_end;
    }

  tester = dsk_malloc (sizeof (MatchTesterStandard)
                       + sizeof (MatchTypeAndPattern) * (n_unique_types-1));
  tester->base.test = match_tester_standard_test;
  tester->base.destroy = match_tester_standard_destroy;
  tester->n_types = n_unique_types;
  patterns = alloca (sizeof (DskPatternEntry) * max_run_length);
  n_unique_types = 0;
  for (at = list; at; )
    {
      MatchTestList *run_end = at;
      unsigned run_length = 0;
      DskError *error = NULL;
      while (run_end && run_end->match_type == at->match_type)
        {
          patterns[run_length].pattern = run_end->pattern_str;
          patterns[run_length].result = tester;
          run_length++;
          run_end = run_end->prev;
        }
      tester->types[n_unique_types].test = dsk_pattern_compile (run_length, patterns, &error);
      if (tester->types[n_unique_types].test == NULL)
        {
#if 0
          if (server->config_error_trap)
            server->config_error_trap (error, server->config_error_trap_data);
#endif
          dsk_warning ("error compiling pattern for match-type %d, %s",
                       at->match_type, error->message);
          dsk_error_unref (error);
          return NULL;
        }
      tester->types[n_unique_types].match_type = at->match_type;
      n_unique_types++;
      at = run_end;
    }
  return (MatchTester *) tester;
}

void dsk_http_server_match_restore             (DskHttpServer        *server)
{
  dsk_return_if_fail (server->current != &server->top, "cannot 'restore' after last frame");

  server->current->under_construction = DSK_FALSE;

  /* Create tester object, if needed */
  if (server->current->tester == NULL)
    server->current->tester = create_tester (server->current->test_list);

  /* Free the test information */
  while (server->current->test_list != NULL)
    {
      MatchTestList *next = server->current->test_list->prev;
      dsk_free (server->current->test_list);
      server->current->test_list = next;
    }

  /* Now back to parent context */
  server->current = server->current->parent;
}


static Handler *
add_handler_generic (DskHttpServer *server,
                     HandlerType handler_type,
                     FunctionPointer handler_func,
                     void *handler_data,
                     DskHookDestroy handler_destroy)
{
  Handler *handler = DSK_NEW (Handler);
  handler->handler_type = handler_type;
  handler->handler = handler_func;
  handler->handler_data = handler_data;
  handler->handler_destroy = handler_destroy;
  handler->next = NULL;

  /* Add to handler list */
  DSK_QUEUE_ENQUEUE (GET_HANDLER_QUEUE (server->current), handler);

  return handler;
}

void
dsk_http_server_register_streaming_post_handler (DskHttpServer *server,
                                                 DskHttpServerStreamingPostFunc func,
                                                 void *func_data,
                                                 DskHookDestroy destroy)
{
  add_handler_generic (server, HANDLER_TYPE_STREAMING_POST_DATA,
                       (FunctionPointer) func, func_data, destroy);
}

void
dsk_http_server_register_cgi_handler (DskHttpServer *server,
                                      DskHttpServerCgiFunc func,
                                      void *func_data,
                                      DskHookDestroy destroy)
{
  add_handler_generic (server, HANDLER_TYPE_CGI,
                       (FunctionPointer) func, func_data, destroy);
}

void
dsk_http_server_register_websocket_handler      (DskHttpServer *server,
                                                 DskHttpServerWebsocketFunc func,
                                                 void          *func_data,
                                                 DskHookDestroy destroy)
{
  (void) add_handler_generic (server, HANDLER_TYPE_WEBSOCKET,
                              (FunctionPointer) func, func_data, destroy);
}

/* === Handling Requests === */
typedef enum
{
  REQUEST_HANDLING_INIT,
  REQUEST_HANDLING_INVOKING,
  REQUEST_HANDLING_NEED_POST_DATA,
  REQUEST_HANDLING_BLOCKED_ERROR,
  REQUEST_HANDLING_BLOCKED_PASS,
  REQUEST_HANDLING_BLOCKED_INTERNAL_REDIRECT,
  REQUEST_HANDLING_WAITING,     /* for a respond() type function */
  REQUEST_HANDLING_GOT_RESPONSE_WHILE_INVOKING,
  REQUEST_HANDLING_GOT_RESPONSE
} RequestHandlingState;

struct _RealServerRequest
{
  DskHttpServerRequest request;
  MatchTestNode *node;
  Handler *handler;

  RealServerRequest *prev, *next;

  /* For exponential resized of arrays */
  unsigned cgi_variables_alloced;

  /* are we currently invoking the handler's function */
  RequestHandlingState state;

  /* For BLOCKED_ERROR state */
  char *blocked_error;
  DskHttpStatus blocked_error_status;

  DskBuffer post_data_buffer;

  /* We free the request when !waiting_for_response && got_transfer_destroy
     && !invoking_handler */
  unsigned waiting_for_response : 1;
  unsigned got_transfer_destroy : 1;
  unsigned invoking_handler : 1;

};

static void
maybe_free_real_server_request (RealServerRequest *rreq)
{
  ////dsk_warning ("maybe_free_real_server_request: %p: waiting_for_response=%u, got_transfer_destroy=%u, invoking_handler=%u", rreq, rreq->waiting_for_response, rreq->got_transfer_destroy, rreq->invoking_handler);
  if (!rreq->waiting_for_response
   && rreq->got_transfer_destroy
   && !rreq->invoking_handler)
    {
      dsk_object_unref (rreq->request.request_header);
      if (rreq->request.cgi_variables_computed)
        {
          unsigned i;
          for (i = 0; i < rreq->request.n_cgi_variables; i++)
            dsk_cgi_variable_clear (rreq->request.cgi_variables + i);
          dsk_free (rreq->request.cgi_variables);
        }
      dsk_free (rreq);
    }
}

static dsk_boolean find_first_match (RealServerRequest *rreq);
static dsk_boolean advance_to_next_handler (RealServerRequest *info);
static void invoke_handler (RealServerRequest *info);

/* An error response without the flag handling -- for use in other "respond" functions */
static void
respond_error (DskHttpServerRequest *request,
               DskHttpStatus         status,
               const char           *message)
{
  /* Configure response */
  RealServerRequest *rreq = (RealServerRequest *) request;
  DskHttpServerStreamResponseOptions resp_options = DSK_HTTP_SERVER_STREAM_RESPONSE_OPTIONS_INIT;
  DskHttpResponseOptions header_options = DSK_HTTP_RESPONSE_OPTIONS_INIT;
  DskBuffer content_buffer = DSK_BUFFER_INIT;
  DskPrint *print;
  uint8_t *content_data;
  DskError *error = NULL;

  resp_options.header_options = &header_options;
  header_options.status_code = status;
  header_options.content_type = "text/html/UTF-8";

  /* compute content */
  print = dsk_print_new_buffer (&content_buffer);
  dsk_print_set_string (print, "status_message", dsk_http_status_get_message (status));
  dsk_print_set_uint (print, "status_code", status);
  dsk_print_set_string (print, "message", message);
  dsk_print_set_filtered_string (print, "html_escaped_message", message,
                                 dsk_xml_escaper_new ());
  dsk_print (print, "<html>"
                    "<head><title>Error $status_code: $status_message</title></head>"
                    "<body>"
                    "<h1>Error $status_code: $status_message</h1>"
                    "<p><pre>\n"
                    "$html_escaped_message\n"
                    "</pre></p>"
                    "</body>"
                    "</html>");
  dsk_print_free (print);
  content_data = dsk_malloc (content_buffer.size);
  resp_options.content_body = content_data;
  resp_options.content_length = content_buffer.size;
  dsk_buffer_read (&content_buffer, content_buffer.size, content_data);

  /* Respond */
  if (!dsk_http_server_stream_respond (request->transfer, &resp_options, &error))
    {
      dsk_warning ("error responding with error (original error='%s'; second error='%s')",
                   message, error->message);
      dsk_error_unref (error);
    }
  dsk_free (content_data);

  /* Free request */
  maybe_free_real_server_request (rreq);
}

/* === One of these functions should be called by any handler === */
void dsk_http_server_request_respond          (DskHttpServerRequest *request,
                                               DskHttpServerResponseOptions *options)
{
  RealServerRequest *rreq = (RealServerRequest *) request;
  DskHttpServerStreamResponseOptions soptions = DSK_HTTP_SERVER_STREAM_RESPONSE_OPTIONS_INIT;
  DskHttpResponseOptions header_options = DSK_HTTP_RESPONSE_OPTIONS_INIT;
  dsk_boolean must_unref_content_stream = DSK_FALSE;
  DskError *error = NULL;

  dsk_assert (rreq->state == REQUEST_HANDLING_INVOKING
          ||  rreq->state == REQUEST_HANDLING_WAITING);
  dsk_assert (rreq->waiting_for_response);


  /* Transform options */
  soptions.header_options = &header_options;
  
  soptions.content_length = options->content_length;
  soptions.content_body = options->content_body;
  soptions.content_stream = options->source;
  if (options->source_filename)
    {
      /* XXX: need a function for this */
      int fd = open (options->source_filename, O_RDONLY);
      struct stat stat_buf;
      DskOctetStreamFdSource *fdsource;
      if (fd < 0)
        {
          /* construct error message */
          int open_errno = errno;
          DskBuffer content_buffer = DSK_BUFFER_INIT;
          DskPrint *print = dsk_print_new_buffer (&content_buffer);
          char *error_message;
          dsk_print_set_string (print, "filename", options->source_filename);
          dsk_print_set_string (print, "error_message", strerror (open_errno));
          dsk_print (print, "error opening $filename: $error_message");
          dsk_print_free (print);
          error_message = dsk_buffer_empty_to_string (&content_buffer);

          /* serve response */
          dsk_http_server_request_respond_error (request, 404, error_message);
          dsk_free (error_message);
          return;
        }
      if (fstat (fd, &stat_buf) < 0)
        {
          dsk_warning ("error calling fstat: %s", strerror (errno));
        }
      else
        {
          /* try to setup content-length */
          if (S_ISREG (stat_buf.st_mode))
            soptions.content_length = stat_buf.st_size;
        }
      if (!dsk_octet_stream_new_fd (fd,
                                    DSK_FILE_DESCRIPTOR_IS_READABLE
                                    |DSK_FILE_DESCRIPTOR_IS_NOT_WRITABLE,
                                    NULL,        /* do not need stream */
                                    &fdsource,
                                    NULL,        /* no sink */
                                    &error))
        {
          dsk_add_error_prefix (&error, "opening %s", options->source_filename);
          dsk_http_server_request_respond_error (request, 404, error->message);
          dsk_error_unref (error);
          close (fd);
          return;
        }
      soptions.content_stream = (DskOctetSource *) fdsource;
      must_unref_content_stream = DSK_TRUE;
    }
  else if (options->source_buffer != NULL)
    {
      DskMemorySource *source = dsk_memory_source_new ();
      soptions.content_length = options->source_buffer->size;
      dsk_buffer_drain (&source->buffer, options->source_buffer);
      dsk_memory_source_added_data (source);
      dsk_memory_source_done_adding (source);
      soptions.content_stream = (DskOctetSource *) source;
      must_unref_content_stream = DSK_TRUE;
    }
  header_options.content_type = options->content_type;
  header_options.content_main_type = options->content_main_type;
  header_options.content_sub_type = options->content_sub_type;
  header_options.content_charset = options->content_charset;
  header_options.date = dsk_dispatch_default ()->last_dispatch_secs;

  /* Do lower-level response */
  rreq->waiting_for_response = DSK_FALSE;
  if (!dsk_http_server_stream_respond (request->transfer, &soptions, &error))
    {
      dsk_warning ("dsk_http_server_stream_respond failed: %s", error->message);
      //dsk_http_server_request_respond_error (request, 500, error->message);
      dsk_error_unref (error);
      if (must_unref_content_stream)
        dsk_object_unref (soptions.content_stream);
      return;
    }

  /* State transitions */
  if (rreq->state == REQUEST_HANDLING_INVOKING)
    rreq->state = REQUEST_HANDLING_GOT_RESPONSE_WHILE_INVOKING;
  else
    {
      /* free RealServerRequest */
      rreq->state = REQUEST_HANDLING_GOT_RESPONSE;
      maybe_free_real_server_request (rreq);
    }

  if (must_unref_content_stream)
    dsk_object_unref (soptions.content_stream);
}

/* Like the public function, but also works in INIT state */
static void
_dsk_http_server_request_respond_error (DskHttpServerRequest *request,
                                        DskHttpStatus         status,
                                        const char           *message)
{
  RealServerRequest *rreq = (RealServerRequest *) request;
  dsk_assert (rreq->state == REQUEST_HANDLING_WAITING
          ||  rreq->state == REQUEST_HANDLING_INVOKING
          ||  rreq->state == REQUEST_HANDLING_INIT);
  if (rreq->state != REQUEST_HANDLING_INIT)
    {
      dsk_assert (rreq->waiting_for_response);
      rreq->waiting_for_response = DSK_FALSE;
    }
  if (rreq->state == REQUEST_HANDLING_INVOKING)
    {
      rreq->state = REQUEST_HANDLING_BLOCKED_ERROR;
      rreq->blocked_error = dsk_strdup (message);
      rreq->blocked_error_status = status;
    }
  else
    {
      respond_error (request, status, message);
    }
}

void dsk_http_server_request_respond_error    (DskHttpServerRequest *request,
                                               DskHttpStatus         status,
                                               const char           *message)
{
  RealServerRequest *rreq = (RealServerRequest *) request;
  dsk_assert (rreq->state != REQUEST_HANDLING_INIT);
  _dsk_http_server_request_respond_error (request, status, message);
}

void dsk_http_server_request_redirect         (DskHttpServerRequest *request,
                                               DskHttpStatus         status,
                                               const char           *location)
{
  DskHttpServerStreamResponseOptions resp_options = DSK_HTTP_SERVER_STREAM_RESPONSE_OPTIONS_INIT;
  DskHttpResponseOptions header_options = DSK_HTTP_RESPONSE_OPTIONS_INIT;
  RealServerRequest *rreq = (RealServerRequest *) request;
  DskBuffer content_buffer = DSK_BUFFER_INIT;
  DskPrint *print;
  uint8_t *content_data;
  DskError *error = NULL;

  dsk_assert (rreq->waiting_for_response);
  if (!(300 <= status && status <= 399))
    {
      dsk_warning ("bad redirect");
      dsk_http_server_request_respond_error (request, 500,
                                             "got non-3XX code for redirect");
      return;
    }
  rreq->waiting_for_response = DSK_FALSE;
  resp_options.header_options = &header_options;
  header_options.status_code = status;
  header_options.location = (char*)location;

  /* use dsk_print to format content body */
  print = dsk_print_new_buffer (&content_buffer);
  dsk_print_set_uint (print, "status_code", status);
  dsk_print_set_string (print, "location", location);
  dsk_print_set_filtered_string (print, "location_html_escaped", location,
                                 dsk_xml_escaper_new ());
  dsk_print (print, "<html><head><title>See $location_html_escaped</title></head>"
             "<body>Please go <a href=\"$location\">here</a></body>"
             "</html>\n");
  dsk_print_free (print);

  resp_options.content_length = content_buffer.size;
  content_data = dsk_malloc (content_buffer.size);
  dsk_buffer_read (&content_buffer, content_buffer.size, content_data);
  resp_options.content_body = content_data;
  header_options.content_type = "text/html/UTF-8";

  if (!dsk_http_server_stream_respond (request->transfer, &resp_options, &error))
    {
      dsk_warning ("error making redirect respond: %s", error->message);
      dsk_error_unref (error);
    }
  dsk_free (content_data);
  maybe_free_real_server_request (rreq);
}

static void
append_cgi_variable  (DskHttpServerRequest *request,
                      DskCgiVariable        *cgi)
{
  RealServerRequest *rreq = (RealServerRequest *) request;
  if (request->n_cgi_variables == rreq->cgi_variables_alloced)
    {
      unsigned new_alloced = rreq->cgi_variables_alloced ? rreq->cgi_variables_alloced * 2 : 4;
      request->cgi_variables = dsk_realloc (request->cgi_variables, new_alloced * sizeof (DskCgiVariable));
      rreq->cgi_variables_alloced = new_alloced;
    }
  request->cgi_variables[request->n_cgi_variables++] = *cgi;
}

static void
respond_no_handler_found (DskHttpServerRequest *request)
{
  RealServerRequest *rreq = (RealServerRequest *) request;
  rreq->waiting_for_response = DSK_TRUE;  //HACK
  _dsk_http_server_request_respond_error (request, 404,
                                          "no handlers matched your request");
}

static void
add_get_cgi_variables (RealServerRequest *rreq)
{
  const char *qm;
  qm = strchr (rreq->request.request_header->path, '?');
  if (qm)
    {
      /* compute GET variables (this may be called with an initial
       * query, or via an internal redirect). */
      size_t n_variables;
      DskCgiVariable *variables;
      DskError *error = NULL;
      if (!dsk_cgi_parse_query_string (qm, &n_variables, &variables, &error))
        {
          dsk_warning ("error parsing query string CGI variables: %s", error->message);
          dsk_error_unref (error);
          return;
        }
      else
        {
          unsigned i;
          for (i = 0; i < n_variables; i++)
            append_cgi_variable (&rreq->request, &variables[i]);
          dsk_free (variables);
        }
    }
}

static void
add_post_cgi_variables (RealServerRequest *rreq)
{
  unsigned post_data_len = rreq->request.raw_post_data_size;
  const uint8_t *post_data = rreq->request.raw_post_data;
  size_t n_variables;
  DskCgiVariable *variables;
  DskError *error = NULL;
  if (!dsk_cgi_parse_post_data (rreq->request.request_header->content_type,
                                rreq->request.request_header->content_type_kv_pairs,
                                post_data_len, post_data,
                                &n_variables, &variables, &error))
    {
      dsk_warning ("error parsing query string CGI variables: %s", error->message);
      dsk_error_unref (error);
      return;
    }
  else
    {
      unsigned i;
      for (i = 0; i < n_variables; i++)
        append_cgi_variable (&rreq->request, &variables[i]);
      dsk_free (variables);
    }
}

void dsk_http_server_request_internal_redirect(DskHttpServerRequest *request,
                                               const char           *new_path)
{
  RealServerRequest *rreq = (RealServerRequest *) request;
  DskHttpRequestOptions req_options;
  DskHttpRequest *old_header;
  dsk_assert (rreq->waiting_for_response);
  rreq->waiting_for_response = DSK_FALSE;
  if (rreq->state == REQUEST_HANDLING_WAITING)
    rreq->state = REQUEST_HANDLING_INIT;
  else
    dsk_assert (rreq->state == REQUEST_HANDLING_INVOKING);

  /* Create the new HTTP request correspond to this redirect */
  old_header = request->request_header;
  dsk_http_request_init_options (old_header, &req_options);
  req_options.full_path = (char*) new_path;
  request->request_header = dsk_http_request_new (&req_options, NULL);
  dsk_object_unref (old_header);

  /* Do we want to handle CGI variables? */
  if (request->cgi_variables_computed)
    {
      unsigned o, i;

      /* remove any existing GET variables */
      for (i = o = 0; i < request->n_cgi_variables; i++)
        if (request->cgi_variables[i].is_get)
          dsk_cgi_variable_clear (&request->cgi_variables[i]);
        else
          request->cgi_variables[o++] = request->cgi_variables[i];
      request->n_cgi_variables = o;

      add_get_cgi_variables (rreq);
    }

  if (!find_first_match (rreq))
    {
      respond_no_handler_found (request);
      return;
    }


  dsk_assert (rreq->handler != NULL);
  if (rreq->state == REQUEST_HANDLING_INVOKING)
    rreq->state = REQUEST_HANDLING_BLOCKED_INTERNAL_REDIRECT;
  else
    invoke_handler (rreq);
}

void dsk_http_server_request_pass             (DskHttpServerRequest *request)
{
  RealServerRequest *rreq = (RealServerRequest *) request;
  dsk_assert (rreq->waiting_for_response);
  rreq->waiting_for_response = DSK_FALSE;
  if (rreq->state == REQUEST_HANDLING_INVOKING)
    {
      rreq->state = REQUEST_HANDLING_BLOCKED_PASS;
    }
  else
    {
      if (!advance_to_next_handler (rreq))
        {
          respond_no_handler_found (request);
          return;
        }
      dsk_assert (rreq->handler != NULL);
      invoke_handler (rreq);
    }
}


static dsk_boolean
match_test_node_matches    (MatchTestNode *node,
                            DskHttpServerRequest *request)
{
  if (node->tester == NULL)
    return DSK_TRUE;
  else
    return node->tester->test (node->tester, request);
}

static MatchTestNode *
find_first_match_recursive (MatchTestNode *node,
                            DskHttpServerRequest *request)
{
  if (match_test_node_matches (node, request))
    {
      MatchTestNode *child;
      for (child = node->first_child; child; child = child->next_sibling)
        {
          MatchTestNode *rv = find_first_match_recursive (child, request);
          if (rv != NULL)
            return rv;
        }
      if (node->first_handler != NULL)
        return node;
    }
  return NULL;
}

static dsk_boolean
find_first_match (RealServerRequest *rreq)
{
  DskHttpServerRequest *request = &rreq->request;
  MatchTestNode *node;
  node = find_first_match_recursive (&request->server->top, request);
  if (node == NULL)
    return DSK_FALSE;
  rreq->node = node;
  rreq->handler = node->first_handler;
  return DSK_TRUE;
}

static MatchTestNode *
find_next_match  (DskHttpServer  *server,
                  MatchTestNode  *node,
                  DskHttpServerRequest *request)
{
  MatchTestNode *at;
  DSK_UNUSED (server);
  while (node)
    {
      for (at = node->next_sibling; at; at = at->next_sibling)
        {
          MatchTestNode *rv = find_first_match_recursive (at, request);
          if (rv)
            return rv;
        }
      node = node->parent;
      if (node->first_handler != NULL)
        return node;
    }
  return NULL;
}

static dsk_boolean
advance_to_next_handler (RealServerRequest *info)
{
  if (info->handler->next != NULL)
    info->handler = info->handler->next;
  else
    {
      info->node = find_next_match (info->request.bind_info->server,
                                    info->node,
                                    &info->request);
      if (info->node == NULL)
        return DSK_FALSE;
      info->handler = info->node->first_handler;
    }
  return DSK_TRUE;
}

static void
compute_cgi_variables (RealServerRequest *rreq)
{
  DskHttpServerRequest *request = &rreq->request;
  dsk_assert (request->n_cgi_variables == 0);

  add_get_cgi_variables (rreq);
  if (request->request_header->verb == DSK_HTTP_VERB_POST
   || request->request_header->verb == DSK_HTTP_VERB_PUT)
    {
      dsk_assert (request->has_raw_post_data);
      add_post_cgi_variables (rreq);
    }
  request->cgi_variables_computed = DSK_TRUE;
}

static dsk_boolean
begin_computing_cgi_variables (RealServerRequest *rreq)
{
  DskHttpVerb verb = rreq->request.request_header->verb;

  /* Not a POST or PUT request, therefore we don't expect CGI variables */
  if (verb != DSK_HTTP_VERB_PUT && verb != DSK_HTTP_VERB_POST)
    compute_cgi_variables (rreq);
  else if (rreq->request.has_raw_post_data)
    compute_cgi_variables (rreq);
  else
    return DSK_FALSE;
  return DSK_TRUE;
}

static void
invoke_handler (RealServerRequest *rreq)
{
  DskHttpRequest *req_header = rreq->request.transfer->request;
  dsk_boolean is_websocket = req_header->is_websocket_request;
restart:
  /* Do the actual handler invocation,
     with 'in_handler' acting as a reentrancy guard,
     so that dsk_http_server_request_respond_*()
     calls during handler invocation are processed in a loop
     (see 'if (rreq->in_handler_got_result)' below) instead
     of recursing to cause a very large stack. */
  dsk_assert (rreq->state == REQUEST_HANDLING_INIT
           || rreq->state == REQUEST_HANDLING_WAITING
           || rreq->state == REQUEST_HANDLING_NEED_POST_DATA);
  dsk_assert (!rreq->invoking_handler);
  dsk_assert (!rreq->waiting_for_response);

  rreq->state = REQUEST_HANDLING_INVOKING;
  rreq->invoking_handler = DSK_TRUE;
  if (is_websocket)
    { 
      if (rreq->handler->handler_type == HANDLER_TYPE_WEBSOCKET)
        {
          DskHttpServerWebsocketFunc func;
          const char *value = dsk_http_request_get (req_header, "Sec-WebSocket-Protocol");
          char **protocols;
          func = (DskHttpServerWebsocketFunc) rreq->handler->handler;

          /* split up the list of protocols */
          protocols = dsk_utf8_split_on_whitespace (value);

          /* create response header */
          func (&rreq->request, protocols, rreq->handler->handler_data);

          dsk_strv_free (protocols);
        }
      else
        rreq->state = REQUEST_HANDLING_BLOCKED_PASS;
    }
  else
    switch (rreq->handler->handler_type)
      {
      case HANDLER_TYPE_STREAMING_POST_DATA:
        {
          /* Invoke handler immediately */
          DskHttpServerStreamingPostFunc func = (DskHttpServerStreamingPostFunc) rreq->handler->handler;
          DskOctetSource *post_data = (DskOctetSource *) rreq->request.transfer->post_data;
          rreq->waiting_for_response = DSK_TRUE;
          func (&rreq->request, post_data, rreq->handler->handler_data);
        }
        break;
      case HANDLER_TYPE_CGI:
        {
          if (!rreq->request.cgi_variables_computed)
            {
              if (!begin_computing_cgi_variables (rreq))
                {
                  rreq->state = REQUEST_HANDLING_NEED_POST_DATA;
                  break;
                }
            }

          if (rreq->request.cgi_variables_computed)
            {
              /* Invoke handler immediately */
              DskHttpServerCgiFunc func = (DskHttpServerCgiFunc) rreq->handler->handler;
              rreq->waiting_for_response = DSK_TRUE;
              func (&rreq->request, rreq->handler->handler_data);
            }
          else
            {
              /* Handler will be invoked once cgi variables are obtained 
                 (ie after waiting for POST data) */
            }
        }
        break;
      case HANDLER_TYPE_WEBSOCKET:
        /* websocket requests handled above */
        rreq->state = REQUEST_HANDLING_BLOCKED_PASS;
      }
  rreq->invoking_handler = DSK_FALSE;
  switch (rreq->state)
    {
    case REQUEST_HANDLING_INIT:
      dsk_assert_not_reached ();
    case REQUEST_HANDLING_INVOKING:
      rreq->state = REQUEST_HANDLING_WAITING;
      break;
    case REQUEST_HANDLING_NEED_POST_DATA:
      dsk_assert_not_reached ();
    case REQUEST_HANDLING_BLOCKED_ERROR:
      {
        /* this will free the handler */
        char *msg = rreq->blocked_error;
        rreq->blocked_error = NULL;
        respond_error (&rreq->request, rreq->blocked_error_status, msg);
        dsk_free (msg);
        return;
      }
    case REQUEST_HANDLING_BLOCKED_PASS:
      rreq->state = REQUEST_HANDLING_INIT;
      if (!advance_to_next_handler (rreq))
        {
          respond_no_handler_found (&rreq->request);
          return;
        }
      else
        goto restart;
    case REQUEST_HANDLING_BLOCKED_INTERNAL_REDIRECT:
      rreq->state = REQUEST_HANDLING_INIT;
      goto restart;
    case REQUEST_HANDLING_WAITING:
    case REQUEST_HANDLING_GOT_RESPONSE:
      dsk_assert_not_reached ();
    case REQUEST_HANDLING_GOT_RESPONSE_WHILE_INVOKING:
      maybe_free_real_server_request (rreq);
      return;
    }
}

static void
stream_transfer_handle_error_notify       (DskHttpServerStreamTransfer *transfer,
                                           DskError                    *error)
{
  RealServerRequest *rreq = transfer->func_data;
  dsk_warning ("error in stream-transfer: %s [request %s]",
               error->message,
               rreq->request.request_header->path);
}

static void
stream_transfer_handle_post_data_complete (DskHttpServerStreamTransfer *transfer)
{
  RealServerRequest *rreq = transfer->func_data;

  rreq->request.has_raw_post_data = DSK_TRUE;
  rreq->request.raw_post_data_size = rreq->post_data_buffer.size;
  rreq->request.raw_post_data = dsk_malloc (rreq->post_data_buffer.size);
  dsk_buffer_read (&rreq->post_data_buffer,
                   rreq->post_data_buffer.size,
                   rreq->request.raw_post_data);

  if (rreq->state == REQUEST_HANDLING_NEED_POST_DATA)
    invoke_handler (rreq);
}

static void
stream_transfer_handle_post_data_failed   (DskHttpServerStreamTransfer *transfer)
{
  DSK_UNUSED (transfer);
#if 0
  RealServerRequest *rreq = transfer->func_data;
  if (rreq->state == REQUEST_HANDLING_NEED_POST_DATA)
    {
    }
#endif
}

static void
stream_transfer_handle_destroy            (DskHttpServerStreamTransfer *transfer)
{
  RealServerRequest *rreq = transfer->func_data;
  rreq->got_transfer_destroy = DSK_TRUE;
  maybe_free_real_server_request (rreq);
}


static DskHttpServerStreamFuncs stream_transfer_handlers =
{
  stream_transfer_handle_error_notify,
  stream_transfer_handle_post_data_complete,
  stream_transfer_handle_post_data_failed,
  stream_transfer_handle_destroy
};

static dsk_boolean
handle_http_server_request_available (DskHttpServerStream   *stream,
                                      ServerStream          *sstream)
{
  RealServerRequest *rreq;
  DskHttpServerStreamTransfer *xfer;
  xfer = dsk_http_server_stream_get_request (stream);
  if (xfer == NULL)
    return DSK_TRUE;

  rreq = DSK_NEW0 (RealServerRequest);
  rreq->request.server = sstream->bind_info->server;
  rreq->request.request_header = dsk_object_ref (xfer->request);
  rreq->request.transfer = xfer;
  rreq->request.bind_info = sstream->bind_info;
  rreq->request.client_ip_address = sstream->ip_address;
  rreq->request.client_ip_port = sstream->ip_port;
  dsk_http_server_stream_transfer_set_funcs (xfer, &stream_transfer_handlers, rreq);

  if (find_first_match (rreq))
    invoke_handler (rreq);
  else
    {
      respond_no_handler_found (&rreq->request);
    }
  return DSK_TRUE;
}

static void
http_server_stream_destroyed (ServerStream *sstream)
{
  if (sstream->bind_info != NULL)
    {
      DskHttpServerBindInfo *bind_info = sstream->bind_info;
      DSK_LIST_REMOVE (GET_BIND_INFO_STREAM_LIST (bind_info), sstream);
    }
  if (sstream->http_stream)
    dsk_object_unref (sstream->http_stream);
  dsk_free (sstream);
}

static dsk_boolean
handle_listener_ready (DskOctetListener *listener,
                       DskHttpServerBindInfo         *bind_info)
{
  DskOctetSource *source;
  DskOctetSink *sink;
  DskError *error = NULL;
  DskHttpServerStream *http_stream;
  ServerStream *sstream;
  switch (dsk_octet_listener_accept (listener, NULL, &source, &sink, &error))
    {
    case DSK_IO_RESULT_ERROR:
      //
      dsk_warning ("octet-listener failed to accept connectino: %s",
                   error->message);
      dsk_error_unref (error);
      return DSK_TRUE;
    case DSK_IO_RESULT_AGAIN:
      return DSK_TRUE;
    case DSK_IO_RESULT_SUCCESS:
      break;
    case DSK_IO_RESULT_EOF:
      dsk_warning ("octet-listener: accept returned EOF?");
      return DSK_FALSE;
    }

  http_stream = dsk_http_server_stream_new (sink, source,
                                            &bind_info->server_stream_options);
  sstream = DSK_NEW (ServerStream);
  sstream->bind_info = bind_info;

  DskFileDescriptor fd = -1;
  if (dsk_object_is_a (source, &dsk_octet_stream_fd_source_class)
   && source->stream != NULL)
    fd = DSK_OCTET_STREAM_FD (source->stream)->fd;
  if (fd < 0
   || dsk_getpeername (fd, &sstream->ip_address, &sstream->ip_port))
    {
      memset (&sstream->ip_address, 0, sizeof (DskIpAddress));
      sstream->ip_port = 0;
    }
  DSK_LIST_APPEND (GET_BIND_INFO_STREAM_LIST (bind_info), sstream);
  sstream->http_stream = http_stream;
  sstream->trap = dsk_hook_trap (&http_stream->request_available,
                            (DskHookFunc) handle_http_server_request_available,
                            sstream,
                            (DskHookDestroy) http_server_stream_destroyed);
  dsk_object_unref (sink);
  dsk_object_unref (source);
  return DSK_TRUE;
}

static void
bind_info_destroyed (DskHttpServerBindInfo *bind_info)
{
  DskHttpServerBindInfo **p;
  for (p = &bind_info->server->bind_infos;
       *p != NULL;
       p = &((*p)->next))
    if (*p == bind_info)
      {
        *p = bind_info->next;
        dsk_object_unref (bind_info->listener);
        if (bind_info->is_local)
          dsk_free (bind_info->bind_local_path);
        dsk_free (bind_info);
        return;
      }
}

static DskHttpServerBindInfo *
do_bind (DskHttpServer *server,
         const DskOctetListenerSocketOptions *options,
         DskError **error)
{
  DskOctetListener *listener = dsk_octet_listener_socket_new (options, error);
  DskHttpServerBindInfo *bind_info;
  if (listener == NULL)
    return NULL;
  bind_info = DSK_NEW (DskHttpServerBindInfo);
  bind_info->listener = listener;
  bind_info->server = server;
  bind_info->next = server->bind_infos;
  bind_info->server_stream_options = server->server_stream_options;
  bind_info->first_request = bind_info->last_request = NULL;
  bind_info->first_stream = bind_info->last_stream = NULL;
  server->bind_infos = bind_info;

  dsk_hook_trap (&listener->incoming,
                 (DskHookFunc) handle_listener_ready,
                 bind_info,
                 (DskHookDestroy) bind_info_destroyed);

  return bind_info;
}

dsk_boolean dsk_http_server_bind_tcp           (DskHttpServer        *server,
                                                DskIpAddress         *bind_addr,
                                                unsigned              port,
                                                DskError            **error)
{
  DskOctetListenerSocketOptions listener_opts = DSK_OCTET_LISTENER_SOCKET_OPTIONS_INIT;
  DskHttpServerBindInfo *bind_info;
  listener_opts.is_local = DSK_FALSE;
  if (bind_addr)
    listener_opts.bind_address = *bind_addr;
  listener_opts.bind_port = port;
  bind_info = do_bind (server, &listener_opts, error);
  if (bind_info == NULL)
    return DSK_FALSE;
  bind_info->is_local = DSK_FALSE;
  bind_info->bind_port = port;
  return DSK_TRUE;
}


dsk_boolean dsk_http_server_bind_local         (DskHttpServer        *server,
                                                const char           *path,
                                                DskError            **error)
{
  DskOctetListenerSocketOptions listener_opts = DSK_OCTET_LISTENER_SOCKET_OPTIONS_INIT;
  DskHttpServerBindInfo *bind_info;
  listener_opts.is_local = DSK_TRUE;
  listener_opts.local_path = path;
  bind_info = do_bind (server, &listener_opts, error);
  if (bind_info == NULL)
    return DSK_FALSE;
  bind_info->is_local = DSK_TRUE;
  bind_info->bind_local_path = dsk_strdup (path);
  return DSK_TRUE;
}


/* --- clean up --- */
static void
destruct_match_node (MatchTestNode *node)
{

  /* free handlers */
  while (node->first_handler != NULL)
    {
      Handler *kill = node->first_handler;
      node->first_handler = kill->next;
      if (kill->handler_destroy != NULL)
        kill->handler_destroy (kill->handler_data);
      dsk_free (kill);
    }

  /* free tests */
  while (node->test_list)
    {
      MatchTestList *kill = node->test_list;
      node->test_list = kill->prev;
      dsk_free (kill);
    }
  if (node->tester && node->tester->destroy)
    node->tester->destroy (node->tester);

  /* free children */
  while (node->first_child != NULL)
    {
      MatchTestNode *kill = node->first_child;
      node->first_child = kill->next_sibling;
      destruct_match_node (kill);
      dsk_free (kill);
    }
}
static void dsk_http_server_finalize (DskHttpServer *server)
{
  /* free binding sites and open requests */
  while (server->bind_infos != NULL)
    {
      DskHttpServerBindInfo *bind_info = server->bind_infos;
      server->bind_infos = bind_info->next;

      /* Detach all active requests */
      while (bind_info->first_request)
        {
          RealServerRequest *kill = bind_info->first_request;
          bind_info->first_request = kill->next;
          kill->request.server = NULL;
          kill->request.bind_info = NULL;
          kill->prev = kill->next = NULL;
        }
      bind_info->last_request = NULL;

      /* Free all streams associated with the binding site */
      while (bind_info->first_stream != NULL)
        {
          ServerStream *ss = bind_info->first_stream;
          DskHttpServerStream *http_stream = ss->http_stream;
          DSK_LIST_REMOVE_FIRST (GET_BIND_INFO_STREAM_LIST (bind_info));
          ss->bind_info = NULL;
          ss->http_stream = NULL;
          dsk_http_server_stream_shutdown (http_stream);
          dsk_hook_trap_destroy (ss->trap);
          dsk_object_unref (http_stream);
        }

      dsk_object_unref (bind_info->listener);
      if (bind_info->is_local)
        dsk_free (bind_info->bind_local_path);
      dsk_free (bind_info);
    }

  /* free matching infrastructure */
  destruct_match_node (&server->top);
}

dsk_boolean
dsk_http_server_request_respond_websocket(DskHttpServerRequest *request,
                                         const char           *protocol,
                                         DskWebsocket        **sock_out)
{
  RealServerRequest *rreq = (RealServerRequest *) request;
  dsk_boolean rv;
  rv = dsk_http_server_stream_respond_websocket (request->transfer,
                                                 protocol,
                                                 sock_out);
  maybe_free_real_server_request (rreq);
  return rv;
}

DskCgiVariable *dsk_http_server_request_lookup_cgi (DskHttpServerRequest *request,
                                               const char           *name)
{
  unsigned i;

  /* Return nothing if variables not already computed.
     We cannot compute them here, b/c we may need to block waiting
     for POST data */
  if (!request->cgi_variables_computed)
    return NULL;

  /* todo: sorted table or hash or something */
  for (i = 0; i < request->n_cgi_variables; i++)
    if (request->cgi_variables[i].key != NULL
     && strcmp (request->cgi_variables[i].key, name) == 0)
      return request->cgi_variables + i;
  return NULL;
}
