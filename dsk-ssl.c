#include "dsk.h"


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
DSK_OBJECT_CLASS_DEFINE(DskSslContext, &dsk_object_class, NULL,
                        dsk_ssl_context_finalize);


DskSslContext    *
dsk_ssl_context_new   (DskSslContextOptions *options)
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
          ...
        }
    }
  if (options->key_filename)
    {
      if (SSL_CTX_use_PrivateKey_file (ctx, options->key_filename, SSL_FILETYPE_PEM) != 1)
        {
          ...
          dsk_object_unref (rv);
          return NULL;
        }
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
  DskBuffer incoming, outgoing;
};

struct _DskSslSinkClass
{
  DskOctetStreamClass base_class;
};
struct _DskSslSink
{
  DskOctetSink base_instance;
};

struct _DskSslSourceClass
{
  DskOctetStreamClass base_class;
};
struct _DskSslSource
{
  DskOctetSource base_instance;
};

static int 
bio_dsk_bwrite (BIO *bio, const char *out, int length)
{
  DskSslStream *stream = DSK_SSL_STREAM (bio->ptr);
  DEBUG_BIO("bio_dsk_bwrite: writing %d bytes to read-buffer of backend", length);
  dsk_buffer_append (&stream->outgoing, length, out);
  ensure_has_write_trap (stream);
  return length;
}

static int 
bio_dsk_bread (BIO *bio, char *in, int max_length)
{
  DskSslStream *stream = DSK_SSL_STREAM (bio->ptr);
  DEBUG_BIO("bio_dsk_bread: read %u bytes of %d bytes from backend write buffer", length, max_length);
  int nread = dsk_buffer_read (&stream->incoming, max_length, in);
  check_read_trap (stream);
  return nread;
}

static long 
bio_dsk_ctrl (BIO  *bio,
              int   cmd,
              long  num,
              void *ptr)
{
  DskSslStream *stream = DSK_SSL_STREAM (bio->ptr);

  DEBUG_BIO("bio_dsk_ctrl: called with cmd=%d", cmd);

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
  DEBUG_BIO("bio_dsk_create (%p)", bio);
  return TRUE;
}

static int 
bio_dsk_destroy (BIO *bio)
{
  DskSslStream *stream = DSK_SSL_STREAM (bio->ptr);
  DEBUG_BIO("bio_dsk_destroy (%p)", bio);
  ...
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

static gboolean
do_handshake (GskStreamSsl *stream_ssl, SSL* ssl, GError **error)
{
  int rv;
  DEBUG (stream_ssl, ("do_handshake[client=%u]: start", stream_ssl->is_client));
  rv = SSL_do_handshake (ssl);
  if (rv <= 0)
    {
      int error_code = SSL_get_error (ssl, rv);
      gulong l = ERR_peek_error();
      switch (error_code)
	{
	case SSL_ERROR_NONE:
	  stream_ssl->doing_handshake = 0;
	  set_backend_flags_raw_to_underlying (stream_ssl);
	  //g_message ("DONE HANDSHAKE (is-client=%u)", stream_ssl->is_client);
	  break;
	case SSL_ERROR_SYSCALL:
	case SSL_ERROR_WANT_READ:
	  set_backend_flags_raw (stream_ssl, FALSE, TRUE);
	  //g_message("do-handshake:want-read");
	  break;
	case SSL_ERROR_WANT_WRITE:
	  //g_message("do-handshake:want-write");
	  set_backend_flags_raw (stream_ssl, TRUE, FALSE);
	  break;
	default:
	  {
	    g_set_error (error,
			 GSK_G_ERROR_DOMAIN,
			 GSK_ERROR_BAD_FORMAT,
			 _("error doing-handshake on SSL socket: %s: %s [code=%08lx (%lu)] [rv=%d]"),
			 ERR_func_error_string(l),
			 ERR_reason_error_string(l),
			 l, l, error_code);
	    return FALSE;
	  }
	}
    }
  else
    {
      stream_ssl->doing_handshake = 0;
      set_backend_flags_raw_to_underlying (stream_ssl);
    }
  return TRUE;
}



static DskSslSinkClass dsk_ssl_sink_class =
{
  {
    DSK_OBJECT_CLASS_DEFINE(DskSslSink, &dsk_octet_sink_class,
                            NULL, dsk_ssl_sink_finalize),
    dsk_ssl_sink_write,
    NULL                /* write_buffer */
  }
};

static DskSslSourceClass dsk_ssl_source_class =
{
  {
    DSK_OBJECT_CLASS_DEFINE(DskSslSink, &dsk_octet_source_class,
                            NULL, dsk_ssl_source_finalize),
    dsk_ssl_source_read,
    NULL                /* read_buffer */
  }
};

dsk_boolean
dsk_ssl_stream_new         (DskStreamSslOptions   *options,
                            DskSslStream         **stream_out
                            DskOctetSink         **sink_out,
                            DskOctetSource       **source_out,
                            DskError             **error)
{
  BIO *bio;
  DskSslSink *sink;
  DskSslSource *source;

  if (sink_out == NULL || source_out == NULL)
    {
      dsk_set_error (error, "dsk_ssl_stream_new: sink/source");
      return DSK_FALSE;
    }

  sink = dsk_object_new (&dsk_ssl_sink_class);
  source = dsk_object_new (&dsk_ssl_source_class);

  stream->bio = BIO_new (&bio_method__ssl_underlying_stream);
  stream->bio->ptr = stream;
  stream->bio->init = TRUE;		/// HMM...
  stream->ssl = SSL_new (options->context->ctx);
  stream->context = dsk_object_ref (options->context);
  SSL_set_bio (stream->ssl, stream->bio, stream->bio);
  stream->base_instance.sink = sink;            /* does not own */
  stream->base_instance.source = source;        /* does not own */
  stream->is_client = options->is_client ? 1 : 0;
  stream->is_handshaking = DSK_TRUE;
  sink->base_instance.stream = dsk_object_ref (stream);
  source->base_instance.stream = dsk_object_ref (stream);

  *sink_out = DSK_OCTET_SINK (sink);
  *source_out = DSK_OCTET_SOURCE (source);

  if (stream_out != NULL)
    *stream_out = stream;
  else
    dsk_object_unref (stream);
  return DSK_TRUE;
}
