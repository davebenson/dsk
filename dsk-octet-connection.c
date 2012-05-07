#include "dsk.h"
#include "debug.h"

static DskOctetConnectionOptions default_options = DSK_OCTET_CONNECTION_OPTIONS_INIT;

#if DSK_ENABLE_DEBUGGING
dsk_boolean dsk_debug_connections;
#endif

static dsk_boolean
handle_source_readable (void       *object,
                        void       *callback_data)
{
  DskOctetSource *source = object;
  DskOctetConnection *conn = callback_data;
  DskError *error = NULL;
  dsk_assert (conn->source == source);

  switch (dsk_octet_source_read_buffer (source, &conn->buffer, &error))
    {
    case DSK_IO_RESULT_SUCCESS:
      break;
    case DSK_IO_RESULT_AGAIN:
      return DSK_TRUE;
    case DSK_IO_RESULT_EOF:            /* only for read operations */
      goto done_reading;
    case DSK_IO_RESULT_ERROR:
      if (conn->shutdown_on_write_error)
        dsk_octet_source_shutdown (source);
      goto done_reading;
    }
  if (dsk_debug_connections)
    dsk_warning ("handle_source_readable: buffer size now %u: write_trap->block_count=%u",conn->buffer.size,conn->write_trap->block_count);
  if (conn->buffer.size > 0
   && conn->write_trap->block_count == 1)
    dsk_hook_trap_unblock (conn->write_trap);
  if (conn->buffer.size >= conn->max_buffer)
    dsk_hook_trap_block (conn->read_trap);
  return DSK_TRUE;

done_reading:
  {
    DskHookTrap *read_trap = conn->read_trap;
    dsk_object_unref (conn->source);
    conn->source = NULL;
    conn->read_trap = NULL;
    if (conn->buffer.size == 0)
      {
        DskHookTrap *write_trap = conn->write_trap;
        if (write_trap)
          {
            conn->write_trap = NULL;
            if (write_trap->block_count > 0)
              dsk_hook_trap_unblock (write_trap);
            dsk_hook_trap_destroy (write_trap);

            if (conn->sink)
              dsk_octet_sink_shutdown (conn->sink);
          }
      }
    dsk_hook_trap_destroy (read_trap);
  }
  return DSK_FALSE;
}
static dsk_boolean
handle_sink_writable (void *object, void *callback_data)
{
  DskOctetConnection *conn = callback_data;
  DskOctetSink *sink = object;
  DskError *error = NULL;
  dsk_assert (conn->sink == sink);
  switch (dsk_octet_sink_write_buffer (sink, &conn->buffer, &error))
    {
    case DSK_IO_RESULT_SUCCESS:
    case DSK_IO_RESULT_AGAIN:
      break;
    case DSK_IO_RESULT_EOF:
      dsk_assert_not_reached ();
    case DSK_IO_RESULT_ERROR:
      goto got_error;
    }
  if (conn->buffer.size == 0
   /* && conn->write_trap != NULL */
   && conn->write_trap->block_count == 0)
    dsk_hook_trap_block (conn->write_trap);
  if (conn->read_trap != NULL
   && conn->read_trap->block_count > 0
   && conn->buffer.size < conn->max_buffer)
    dsk_hook_trap_unblock (conn->read_trap);
  return DSK_TRUE;

got_error:
  if (conn->shutdown_on_write_error)
    {
      if (conn->read_trap)
        {
          if (conn->read_trap->block_count > 0)
            dsk_hook_trap_unblock (conn->read_trap);
          dsk_hook_trap_destroy (conn->read_trap);
          conn->read_trap = NULL;
        }
      if (conn->source != NULL)
        {
          DskOctetSource *source = conn->source;
          conn->source = NULL;
          dsk_octet_source_shutdown (source);
          dsk_object_unref (source);
        }
      dsk_octet_sink_shutdown (sink);
    }
  conn->sink = NULL;
  dsk_object_unref (sink);
  return DSK_FALSE;
}

static void
sink_hook_destroyed (void *data)
{
  DskOctetConnection *conn = data;
  if (dsk_debug_connections)
    dsk_warning ("sink_hook_destroyed: conn=%p[%s]; write_trap=%p",
                 conn, ((DskObject*)conn)->object_class->name,
                 conn->write_trap);
  conn->write_trap = NULL;
  if (conn->sink != NULL)
    {
      DskOctetSink *sink = conn->sink;
      conn->sink = NULL;
      dsk_object_unref (sink);
    }
  dsk_object_unref (conn);
}
static void
source_hook_destroyed (void *data)
{
  DskOctetConnection *conn = data;
  if (dsk_debug_connections)
    dsk_warning ("source_hook_destroyed: conn=%p", conn);
  conn->read_trap = NULL;
  if (conn->source)
    {
      DskOctetSource *source = conn->source;
      conn->source = NULL;
      dsk_object_unref (source);
    }
  dsk_object_unref (conn);
}

DskOctetConnection *
dsk_octet_connection_new (DskOctetSource *source,
                          DskOctetSink   *sink,
                          DskOctetConnectionOptions *opt)
{
  DskOctetConnection *connection;
  dsk_assert (dsk_object_is_a (source, &dsk_octet_source_class));
  dsk_assert (dsk_object_is_a (sink, &dsk_octet_sink_class));
  if (opt == NULL)
    opt = &default_options;
  connection = dsk_object_new (&dsk_octet_connection_class);
  connection->source = dsk_object_ref (source);
  connection->sink = dsk_object_ref (sink);
  connection->max_buffer = opt->max_buffer;
  connection->shutdown_on_read_error = opt->shutdown_on_read_error;
  connection->shutdown_on_write_error = opt->shutdown_on_write_error;
  connection->read_trap = dsk_hook_trap (&(source->readable_hook),
                                         handle_source_readable,
                                         dsk_object_ref (connection),
                                         source_hook_destroyed);
  connection->write_trap = dsk_hook_trap (&(sink->writable_hook),
                                          handle_sink_writable,
                                          dsk_object_ref (connection),
                                          sink_hook_destroyed);
  dsk_hook_trap_block (connection->write_trap);
  return connection;
}

void
dsk_octet_connection_shutdown (DskOctetConnection *conn)
{
  dsk_object_ref (conn);
  if (conn->read_trap)
    {
      dsk_hook_trap_destroy (conn->read_trap);
      conn->read_trap = NULL;
    }
  if (conn->write_trap)
    {
      dsk_hook_trap_destroy (conn->write_trap);
      conn->write_trap = NULL;
    }
  if (conn->sink)
    {
      dsk_octet_sink_shutdown (conn->sink);
      dsk_object_unref (conn->sink);
      conn->sink = NULL;
    }
  if (conn->source)
    {
      dsk_octet_source_shutdown (conn->source);
      dsk_object_unref (conn->source);
      conn->source = NULL;
    }
  dsk_object_unref (conn);
}

void
dsk_octet_connection_disconnect (DskOctetConnection *conn)
{
  dsk_object_ref (conn);
  if (conn->read_trap)
    {
      dsk_hook_trap_destroy (conn->read_trap);
      conn->read_trap = NULL;
    }
  if (conn->write_trap)
    {
      dsk_hook_trap_destroy (conn->read_trap);
      conn->write_trap = NULL;
    }
  if (conn->sink)
    {
      dsk_object_unref (conn->sink);
      conn->sink = NULL;
    }
  if (conn->source)
    {
      dsk_object_unref (conn->source);
      conn->source = NULL;
    }
  dsk_object_unref (conn);
}

static void
dsk_octet_connection_finalize (DskOctetConnection *conn)
{
  dsk_assert (conn->source == NULL);
  dsk_assert (conn->sink == NULL);
  if (conn->last_error)
    dsk_error_unref (conn->last_error);
  dsk_buffer_clear (&conn->buffer);
}

DSK_OBJECT_CLASS_DEFINE_CACHE_DATA(DskOctetConnection);
const DskOctetConnectionClass dsk_octet_connection_class =
{
  DSK_OBJECT_CLASS_DEFINE (DskOctetConnection,
                           &dsk_object_class,
                           NULL,
                           dsk_octet_connection_finalize)
};
