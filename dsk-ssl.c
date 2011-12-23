#include <string.h>
#include "dsk.h"
#include <openssl/ssl.h>
#include <openssl/err.h>

#define DSK_SSL_CONTEXT(context) DSK_OBJECT_CAST(DskSslContext, context, &dsk_ssl_context_class)
#define DSK_SSL_STREAM(stream)   DSK_OBJECT_CAST(DskSslStream, stream, &dsk_ssl_stream_class)
#define DSK_SSL_SINK(sink)       DSK_OBJECT_CAST(DskSslSink, sink, &dsk_ssl_sink_class)
#define DSK_SSL_SOURCE(source)   DSK_OBJECT_CAST(DskSslSource, source, &dsk_ssl_source_class)

/* --- SSL context --- */
typedef struct _DskSslContextClass DskSslContextClass;
struct _DskSslContextClass
{
  DskObjectClass base_class;
};

struct _DskSslContext
{
  DskObject base_instance;
  SSL_CTX *ctx;
  char *password;
};

static void
dsk_ssl_context_finalize (DskSslContext *context)
{
  if (context->ctx)
    SSL_CTX_free (context->ctx);
  dsk_free (context->password);
}

DSK_OBJECT_CLASS_DEFINE_CACHE_DATA(DskSslContext);
static DskSslContextClass dsk_ssl_context_class =
{
  DSK_OBJECT_CLASS_DEFINE(DskSslContext, &dsk_object_class, NULL,
                          dsk_ssl_context_finalize)
};

static int
set_password_cb (char *buf, int size, int rwflag, void *userdata)
{
  DskSslContext *ctx = userdata;
  DSK_UNUSED (rwflag);
  strncpy (buf, ctx->password, size);
  return strlen (ctx->password);
}

DskSslContext    *
dsk_ssl_context_new   (DskSslContextOptions *options,
                       DskError            **error)
{
  const SSL_METHOD *method = SSLv3_method ();
  SSL_CTX *ctx = SSL_CTX_new (method);
  DskSslContext *rv = dsk_object_new (&dsk_ssl_context_class);
  rv->ctx = ctx;
  if (options->password)
    {
      rv->password = dsk_strdup (options->password);
      SSL_CTX_set_default_passwd_cb (ctx, set_password_cb);
      SSL_CTX_set_default_passwd_cb_userdata (ctx, rv);
    }
  if (options->cert_filename)
    {
      if (SSL_CTX_use_certificate_file (ctx, options->cert_filename,
                                        SSL_FILETYPE_PEM) != 1)
        {
          dsk_set_error (error, "error using certificate file %s",
                         options->cert_filename);
          dsk_object_unref (rv);
          return NULL;
        }
    }
  if (options->key_filename)
    {
      if (SSL_CTX_use_PrivateKey_file (ctx, options->key_filename, SSL_FILETYPE_PEM) != 1)
        {
          dsk_set_error (error, "error using key file %s",
                         options->key_filename);
          dsk_object_unref (rv);
          return NULL;
        }
    }
  return rv;
}

/* --- SSL stream --- */

typedef struct _DskSslStreamClass DskSslStreamClass;
typedef struct _DskSslSinkClass DskSslSinkClass;
typedef struct _DskSslSink DskSslSink;
typedef struct _DskSslSourceClass DskSslSourceClass;
typedef struct _DskSslSource DskSslSource;

struct _DskSslStreamClass
{
  DskOctetStreamClass base_class;
};
struct _DskSslStream
{
  DskOctetStream base_instance;
  DskOctetSink *underlying_sink;
  DskOctetSource *underlying_source;
  DskSslContext *context;
  SSL *ssl;
  DskHookTrap *underlying_read;
  DskHookTrap *underlying_write;

  unsigned is_client : 1;
  unsigned handshaking : 1;
  unsigned read_needed_to_handshake : 1;
  unsigned write_needed_to_handshake : 1;
  unsigned read_needed_to_write : 1;
  unsigned write_needed_to_read : 1;
};

struct _DskSslSinkClass
{
  DskOctetSinkClass base_class;
};
struct _DskSslSink
{
  DskOctetSink base_instance;
};

struct _DskSslSourceClass
{
  DskOctetSourceClass base_class;
};
struct _DskSslSource
{
  DskOctetSource base_instance;
};
DSK_OBJECT_CLASS_DEFINE_CACHE_DATA(DskSslStream);
static void        dsk_ssl_stream_finalize (DskSslStream     *stream)
{
  dsk_assert (stream->ssl);
  SSL_free (stream->ssl);
  stream->ssl = NULL;
  if (stream->underlying_read)
    dsk_hook_trap_free (stream->underlying_read);
  if (stream->underlying_write)
    dsk_hook_trap_free (stream->underlying_write);
  if (stream->underlying_sink)
    dsk_object_unref (stream->underlying_sink);
  if (stream->underlying_source)
    dsk_object_unref (stream->underlying_source);
  dsk_object_unref (stream->context);
}

static void
dsk_ssl_stream_shutdown (DskSslStream *stream)
{
  if (stream->underlying_read)
    {
      dsk_hook_trap_free (stream->underlying_read);
      stream->underlying_read = NULL;
    }
  if (stream->underlying_write)
    {
      dsk_hook_trap_free (stream->underlying_write);
      stream->underlying_write = NULL;
    }
  if (stream->underlying_sink)
    {
      dsk_octet_sink_shutdown (stream->underlying_sink);
      stream->underlying_sink = NULL;
    }
  if (stream->underlying_source)
    {
      dsk_octet_source_shutdown (stream->underlying_source);
      stream->underlying_source = NULL;
    }
  if (stream->base_instance.sink)
    dsk_octet_sink_detach (stream->base_instance.sink);
  if (stream->base_instance.source)
    dsk_octet_source_detach (stream->base_instance.source);
  if (stream->ssl)
    {
      SSL_free (stream->ssl);
      stream->ssl = NULL;
    }
}

static DskSslStreamClass dsk_ssl_stream_class =
{
  {
    DSK_OBJECT_CLASS_DEFINE(DskSslStream, &dsk_octet_stream_class,
                            NULL,
                            dsk_ssl_stream_finalize)
  }
};

static void dsk_ssl_sink_init (DskSslSink *sink);
static DskIOResult dsk_ssl_sink_write    (DskOctetSink   *sink,
                                          unsigned        max_len,
                                          const void     *data_out,
                                          unsigned       *n_written_out,
                                          DskError      **error);
static void        dsk_ssl_sink_shutdown (DskOctetSink   *sink);

DSK_OBJECT_CLASS_DEFINE_CACHE_DATA(DskSslSink);
static DskSslSinkClass dsk_ssl_sink_class =
{
  {
    DSK_OBJECT_CLASS_DEFINE(DskSslSink, &dsk_octet_sink_class,
                            dsk_ssl_sink_init, NULL),
    dsk_ssl_sink_write,
    NULL,               /* write_buffer */
    dsk_ssl_sink_shutdown
  }
};

static DskIOResult dsk_ssl_source_read     (DskOctetSource   *source,
                                            unsigned        max_len,
                                            void           *data,
                                            unsigned       *n_written_out,
                                            DskError      **error);
static void        dsk_ssl_source_shutdown (DskOctetSource   *source);
static void        dsk_ssl_source_init     (DskSslSource *source);

DSK_OBJECT_CLASS_DEFINE_CACHE_DATA(DskSslSource);
static DskSslSourceClass dsk_ssl_source_class =
{
  {
    DSK_OBJECT_CLASS_DEFINE(DskSslSource, &dsk_octet_source_class,
                            dsk_ssl_source_init, NULL),
    dsk_ssl_source_read,
    NULL,               /* read_buffer */
    dsk_ssl_source_shutdown
  }
};


static dsk_boolean handle_underlying_readable (DskOctetSource *underlying,
                                               DskSslStream   *stream);
static void        handle_underlying_read_destroy (DskSslStream *stream);
static dsk_boolean handle_underlying_writable (DskOctetSink   *underlying,
                                               DskSslStream   *stream);
static void        handle_underlying_write_destroy (DskSslStream *stream);
static void
dsk_ssl_stream_update_traps (DskSslStream *stream)
{
  dsk_boolean read_trap = DSK_FALSE, write_trap = DSK_FALSE;
  if (stream->base_instance.sink == NULL
   || stream->base_instance.source == NULL)
    return;
  if (stream->underlying_sink == NULL
   || stream->underlying_source == NULL)
    return;
  if (stream->handshaking)
    {
      if (stream->read_needed_to_handshake)
        read_trap = DSK_TRUE;
      else if (stream->write_needed_to_handshake)
        write_trap = DSK_TRUE;
    }
  else
    {
      dsk_boolean w = dsk_hook_is_trapped (&stream->base_instance.sink->writable_hook);
      dsk_boolean r = dsk_hook_is_trapped (&stream->base_instance.source->readable_hook);
      if (w)
        {
          if (stream->read_needed_to_write)
            read_trap = DSK_TRUE;
          else
            write_trap = DSK_TRUE;
        }
      if (r)
        {
          if (stream->write_needed_to_read)
            write_trap = DSK_TRUE;
          else
            read_trap = DSK_TRUE;
        }
    }
  if (read_trap && stream->underlying_read == NULL)
    stream->underlying_read = dsk_hook_trap (&stream->base_instance.source->readable_hook,
                                             (DskHookFunc) handle_underlying_readable,
                                             stream,
                                             (DskHookDestroy) handle_underlying_read_destroy);
  else if (!read_trap && stream->underlying_read != NULL)
    {
      dsk_hook_trap_free (stream->underlying_read);
      stream->underlying_read = NULL;
    }
  if (write_trap && stream->underlying_write == NULL)
    stream->underlying_write = dsk_hook_trap (&stream->base_instance.sink->writable_hook,
                                              (DskHookFunc) handle_underlying_writable,
                                              stream,
                                              (DskHookDestroy) handle_underlying_write_destroy);
  else if (!write_trap && stream->underlying_write != NULL)
    {
      /* NOTE: dsk_hook_trap_free is intended to NOT call destroy() */
      dsk_hook_trap_free (stream->underlying_write);
      stream->underlying_write = NULL;
    }
}

static dsk_boolean
do_handshake (DskSslStream *stream_ssl, DskError **error)
{
  int rv;
  //DEBUG (stream_ssl, ("do_handshake[client=%u]: start", stream_ssl->is_client));
  rv = SSL_do_handshake (stream_ssl->ssl);
  if (rv <= 0)
    {
      int error_code = SSL_get_error (stream_ssl->ssl, rv);
      unsigned long l = ERR_peek_error();
      switch (error_code)
	{
	case SSL_ERROR_NONE:
	  stream_ssl->handshaking = 0;
          dsk_ssl_stream_update_traps (stream_ssl);
	  break;
	case SSL_ERROR_SYSCALL:
          dsk_set_error (error, "error with underlying stream");
          return DSK_FALSE;
          break;
	case SSL_ERROR_WANT_READ:
          stream_ssl->read_needed_to_handshake = 1;
          stream_ssl->write_needed_to_handshake = 0;
          dsk_ssl_stream_update_traps (stream_ssl);
	  break;
	case SSL_ERROR_WANT_WRITE:
          stream_ssl->read_needed_to_handshake = 0;
          stream_ssl->write_needed_to_handshake = 1;
          dsk_ssl_stream_update_traps (stream_ssl);
	  break;
	default:
	  {
	    dsk_set_error (error,
			 "error doing-handshake on SSL socket: %s: %s [code=%08lx (%lu)] [rv=%d]",
			 ERR_func_error_string(l),
			 ERR_reason_error_string(l),
			 l, l, error_code);
	    return DSK_FALSE;
	  }
	}
    }
  else
    {
      stream_ssl->handshaking = 0;
      dsk_ssl_stream_update_traps (stream_ssl);
    }
  return DSK_TRUE;
}


static dsk_boolean
handle_underlying_readable (DskOctetSource *underlying,
                            DskSslStream   *stream)
{
  dsk_assert (stream->underlying_source == underlying);
  if (stream->handshaking)
    {
      DskError *error = NULL;
      if (!do_handshake (stream, &error))
        {
          dsk_octet_stream_set_error (DSK_OCTET_STREAM (stream), error);
          dsk_error_unref (error);
          return DSK_FALSE;
        }
      return DSK_TRUE;
    }
  else if (stream->read_needed_to_write)
    dsk_hook_notify (&stream->base_instance.sink->writable_hook);
  else
    dsk_hook_notify (&stream->base_instance.source->readable_hook);
  return DSK_TRUE;
}
static void
handle_underlying_read_destroy (DskSslStream *stream)
{
  stream->underlying_read = NULL;
  dsk_ssl_stream_shutdown (stream);
}
static void
handle_underlying_write_destroy (DskSslStream *stream)
{
  stream->underlying_write = NULL;
  dsk_ssl_stream_shutdown (stream);
}

static dsk_boolean
handle_underlying_writable (DskOctetSink *underlying,
                            DskSslStream   *stream)
{
  dsk_assert (stream->underlying_sink == underlying);
  if (stream->handshaking)
    {
      DskError *error = NULL;
      if (!do_handshake (stream, &error))
        {
          dsk_octet_stream_set_error (DSK_OCTET_STREAM (stream), error);
          dsk_error_unref (error);
          return DSK_FALSE;
        }
      return DSK_TRUE;
    }
  else if (stream->write_needed_to_read)
    dsk_hook_notify (&stream->base_instance.source->readable_hook);
  else
    dsk_hook_notify (&stream->base_instance.sink->writable_hook);
  return DSK_TRUE;
}

static int 
bio_dsk_bwrite (BIO *bio, const char *out, int length)
{
  DskSslStream *stream = DSK_SSL_STREAM (bio->ptr);
  DskError *error = NULL;
  unsigned n_written;
  BIO_clear_retry_flags (bio);
  switch (dsk_octet_sink_write (stream->underlying_sink,
                                length, out, &n_written,
                                &error))
    {
    case DSK_IO_RESULT_SUCCESS:
      return n_written;
    case DSK_IO_RESULT_EOF:
      return 0;
    case DSK_IO_RESULT_AGAIN:
      BIO_set_retry_write (bio);
      return -1;
    case DSK_IO_RESULT_ERROR:
      dsk_octet_stream_set_error (DSK_OCTET_STREAM (stream), error);
      dsk_error_unref (error);
      break;
    }
  errno = EINVAL;                   /* ugh! */
  return -1;

}

static int 
bio_dsk_bread (BIO *bio, char *in, int max_length)
{
  DskSslStream *stream = DSK_SSL_STREAM (bio->ptr);
  DskError *error = NULL;
  unsigned n_read;
  BIO_clear_retry_flags (bio);
  switch (dsk_octet_source_read (stream->underlying_source,
                                 max_length, in, &n_read,
                                 &error))
    {
    case DSK_IO_RESULT_SUCCESS:
      return n_read;
    case DSK_IO_RESULT_EOF:
      return 0;
    case DSK_IO_RESULT_AGAIN:
      BIO_set_retry_read (bio);
      return -1;
    case DSK_IO_RESULT_ERROR:
      dsk_octet_stream_set_error (DSK_OCTET_STREAM (stream), error);
      dsk_error_unref (error);
      break;
    }

  errno = EINVAL;                   /* ugh! */
  return -1;

}

static long 
bio_dsk_ctrl (BIO  *bio,
              int   cmd,
              long  num,
              void *ptr)
{
  DskSslStream *stream = DSK_SSL_STREAM (bio->ptr);
  DSK_UNUSED (stream);
  DSK_UNUSED (num);
  DSK_UNUSED (ptr);

  //DEBUG_BIO("bio_dsk_ctrl: called with cmd=%d", cmd);

  switch (cmd)
    {
    case BIO_CTRL_DUP:
    case BIO_CTRL_FLUSH:
    case BIO_CTRL_PENDING:
    case BIO_CTRL_WPENDING:
      return 1;
    }

  /* -1 seems more appropriate, but this is
     what bss_fd returns when it doesn't know the cmd. */
  return 0;
}

static int 
bio_dsk_create (BIO *bio)
{
  DSK_UNUSED (bio);
  // DEBUG_BIO("bio_dsk_create (%p)", bio);
  return 1;
}

static int 
bio_dsk_destroy (BIO *bio)
{
  DskSslStream *stream = DSK_SSL_STREAM (bio->ptr);
  //DEBUG_BIO("bio_dsk_destroy (%p)", bio);
  DSK_UNUSED (stream);
  return 1;
}

static BIO_METHOD bio_method__ssl_underlying_stream =
{
  22,			/* type:  this is quite a hack */
  "DSK-BIO-Underlying",	/* name */
  bio_dsk_bwrite,	/* bwrite */
  bio_dsk_bread,	/* bread */
  NULL,			/* bputs */
  NULL,			/* bgets */
  bio_dsk_ctrl,	        /* ctrl */
  bio_dsk_create,	/* create */
  bio_dsk_destroy,	/* destroy */
  NULL			/* callback_ctrl */
};

static DskIOResult
dsk_ssl_sink_write (DskOctetSink *sink,
                    unsigned        max_len,
                    const void     *data_out,
                    unsigned       *n_written_out,
                    DskError      **error)
{
  DskSslStream *stream = DSK_SSL_STREAM (sink->stream);
  if (stream->handshaking)
    return DSK_IO_RESULT_AGAIN;
  int rv = SSL_write (stream->ssl, data_out, max_len);
  if (rv > 0)
    {
      *n_written_out = rv;
      return DSK_IO_RESULT_SUCCESS;
    }
  if (rv == 0)
    {
      //dsk_set_error (error, "connection closed");
      return DSK_IO_RESULT_EOF;
    }
  stream->read_needed_to_write = 0;
  switch (rv)
    {
      switch (SSL_get_error (stream->ssl, rv))
	{
	case SSL_ERROR_WANT_READ:
	  stream->read_needed_to_write = 1;
          dsk_ssl_stream_update_traps (stream);
	  return DSK_IO_RESULT_AGAIN;
	case SSL_ERROR_WANT_WRITE:
          dsk_ssl_stream_update_traps (stream);
	  return DSK_IO_RESULT_AGAIN;
	case SSL_ERROR_SYSCALL:
	  dsk_set_error (error, "Gsk-BIO interface had problems writing");
	  break;
	case SSL_ERROR_NONE:
	  dsk_set_error (error, "error writing to ssl stream, but error code set to none");
	  break;
	default:
	  {
	    unsigned long l;
	    l = ERR_peek_error();
	    dsk_set_error (error,
			   "error writing to ssl stream [in the '%s' library]: %s: %s [is-client=%d]",
			   ERR_lib_error_string(l),
			   ERR_func_error_string(l),
			   ERR_reason_error_string(l),
			   stream->is_client);
	    break;
	  }
	}
    }
  return DSK_IO_RESULT_ERROR;
}
static void        dsk_ssl_sink_shutdown (DskOctetSink   *sink)
{
  DskSslStream *stream = DSK_SSL_STREAM (sink->stream);
  dsk_ssl_stream_shutdown (stream);
}

static DskIOResult dsk_ssl_source_read     (DskOctetSource   *source,
                                            unsigned        max_len,
                                            void           *data,
                                            unsigned       *n_read_out,
                                            DskError      **error)
{
  DskSslStream *stream = DSK_SSL_STREAM (source->stream);
  if (stream->handshaking)
    return DSK_IO_RESULT_AGAIN;
  int rv = SSL_read (stream->ssl, data, max_len);
  if (rv > 0)
    {
      *n_read_out = rv;
      return DSK_IO_RESULT_SUCCESS;
    }
  if (rv == 0)
    {
      //dsk_set_error (error, "connection closed");
      return DSK_IO_RESULT_EOF;
    }
  stream->write_needed_to_read = 0;
  switch (rv)
    {
      switch (SSL_get_error (stream->ssl, rv))
	{
	case SSL_ERROR_WANT_READ:
	  return DSK_IO_RESULT_AGAIN;
	case SSL_ERROR_WANT_WRITE:
	  stream->write_needed_to_read = 1;
	  return DSK_IO_RESULT_AGAIN;
	case SSL_ERROR_SYSCALL:
	  dsk_set_error (error, "Gsk-BIO interface had problems reading");
	  break;
	case SSL_ERROR_NONE:
	  dsk_set_error (error, "error reading from ssl stream, but error code set to none");
	  break;
	default:
	  {
	    unsigned long l;
	    l = ERR_peek_error();
	    dsk_set_error (error,
			   "error reading from ssl stream [in the '%s' library]: %s: %s [is-client=%d]",
			   ERR_lib_error_string(l),
			   ERR_func_error_string(l),
			   ERR_reason_error_string(l),
			   stream->is_client);
	    break;
	  }
	}
    }
  return DSK_IO_RESULT_ERROR;
}
static void        dsk_ssl_source_shutdown (DskOctetSource   *source)
{
  DskSslStream *stream = DSK_SSL_STREAM (source->stream);
  dsk_ssl_stream_shutdown (stream);
}

static void
dsk_ssl_sink_set_poll (void *object,
                       dsk_boolean is_trapped)
{
  DskOctetSink *sink = DSK_OCTET_SINK (object);
  dsk_assert (dsk_hook_is_trapped (&sink->writable_hook) == is_trapped);
  dsk_ssl_stream_update_traps (DSK_SSL_STREAM (sink->stream));
}

static void
dsk_ssl_sink_init (DskSslSink *sink)
{
  static DskHookFuncs sink_hook_funcs = {
    (DskHookObjectFunc) dsk_object_ref_f,
    (DskHookObjectFunc) dsk_object_unref_f,
    dsk_ssl_sink_set_poll
  };
  dsk_hook_set_funcs (&sink->base_instance.writable_hook, &sink_hook_funcs);
}

static void
dsk_ssl_source_set_poll (void *object,
                       dsk_boolean is_trapped)
{
  DskOctetSource *source = DSK_OCTET_SOURCE (object);
  dsk_assert (dsk_hook_is_trapped (&source->readable_hook) == is_trapped);
  dsk_ssl_stream_update_traps (DSK_SSL_STREAM (source->stream));
}

static void
dsk_ssl_source_init (DskSslSource *source)
{
  static DskHookFuncs source_hook_funcs = {
    (DskHookObjectFunc) dsk_object_ref_f,
    (DskHookObjectFunc) dsk_object_unref_f,
    dsk_ssl_source_set_poll
  };
  dsk_hook_set_funcs (&source->base_instance.readable_hook, &source_hook_funcs);
}

dsk_boolean
dsk_ssl_stream_new         (DskSslStreamOptions   *options,
                            DskSslStream         **stream_out,
                            DskOctetSource       **source_out,
                            DskOctetSink         **sink_out,
                            DskError             **error)
{
  DskSslStream *stream;
  DskSslSink *sink;
  DskSslSource *source;

  if (sink_out == NULL || source_out == NULL)
    {
      dsk_set_error (error, "dsk_ssl_stream_new: sink/source");
      return DSK_FALSE;
    }

  sink = dsk_object_new (&dsk_ssl_sink_class);
  source = dsk_object_new (&dsk_ssl_source_class);
  stream = dsk_object_new (&dsk_ssl_stream_class);

  BIO *bio;
  bio = BIO_new (&bio_method__ssl_underlying_stream);
  bio->ptr = stream;
  bio->init = 1;		/// HMM...
  stream->ssl = SSL_new (options->context->ctx);
  stream->context = dsk_object_ref (options->context);
  SSL_set_bio (stream->ssl, bio, bio);
  stream->base_instance.sink = DSK_OCTET_SINK (sink);        /* does not own */
  stream->base_instance.source = DSK_OCTET_SOURCE (source); /* does not own */
  stream->is_client = options->is_client ? 1 : 0;
  stream->handshaking = DSK_TRUE;
  sink->base_instance.stream = dsk_object_ref (stream);
  source->base_instance.stream = dsk_object_ref (stream);

  if (stream->is_client)
    SSL_set_connect_state (stream->ssl);
  else
    SSL_set_accept_state (stream->ssl);

  *sink_out = DSK_OCTET_SINK (sink);
  *source_out = DSK_OCTET_SOURCE (source);

  if (stream_out != NULL)
    *stream_out = stream;
  else
    dsk_object_unref (stream);
  return DSK_TRUE;
}
