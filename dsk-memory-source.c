#include "dsk.h"

static void
dsk_memory_source_init (DskMemorySource *source)
{
  dsk_hook_init (&source->buffer_low, source);
}

static void
dsk_memory_source_finalize (DskMemorySource *source)
{
  if (source->error)
    dsk_error_unref (source->error);
  dsk_buffer_clear (&source->buffer);
  dsk_hook_clear (&source->buffer_low);
}

static DskIOResult 
dsk_memory_source_read        (DskOctetSource *source,
                               unsigned        max_len,
                               void           *data_out,
                               unsigned       *bytes_read_out,
                               DskError      **error)
{
  DskMemorySource *msource = DSK_MEMORY_SOURCE (source);
  if (msource->error != NULL)
    {
      *error = msource->error;
      msource->error = NULL;
      return DSK_IO_RESULT_ERROR;
    }
  *bytes_read_out = dsk_buffer_read (&msource->buffer, max_len, data_out);
  if (!msource->done_adding)
    {
      if (msource->buffer.size <= msource->buffer_low_amount)
        dsk_hook_set_idle_notify (&msource->buffer_low, DSK_TRUE);
      if (msource->buffer.size == 0 && msource->error == NULL)
        dsk_hook_set_idle_notify (&source->readable_hook, DSK_FALSE);
    }
  if (*bytes_read_out == 0)
    {
      if (msource->error != NULL)
        {
          *error = msource->error;
          msource->error = NULL;
          return DSK_IO_RESULT_ERROR;
        }
      else if (msource->done_adding)
        return DSK_IO_RESULT_EOF;
      else
        return DSK_IO_RESULT_AGAIN;
    }
  else
    return DSK_IO_RESULT_SUCCESS;
}

static DskIOResult
dsk_memory_source_read_buffer (DskOctetSource *source,
                               DskBuffer      *read_buffer,
                               DskError      **error)
{
  DskMemorySource *msource = DSK_MEMORY_SOURCE (source);
  DskIOResult rv;
  if (msource->error != NULL)
    {
      *error = msource->error;
      msource->error = NULL;
      return DSK_IO_RESULT_ERROR;
    }
  if (msource->buffer.size == 0 && msource->done_adding && msource->error == NULL)
    return DSK_IO_RESULT_EOF;
  if (dsk_buffer_drain (read_buffer, &msource->buffer) == 0)
    {
    rv = DSK_IO_RESULT_AGAIN;
    }
  else
    rv = DSK_IO_RESULT_SUCCESS;
  if (!msource->done_adding)
    {
      dsk_hook_set_idle_notify (&msource->buffer_low, DSK_TRUE);
      dsk_hook_set_idle_notify (&source->readable_hook, DSK_FALSE);
    }
  return rv;
}
static void
dsk_memory_source_shutdown (DskOctetSource *source)
{
  DskMemorySource *msource = DSK_MEMORY_SOURCE (source);
  msource->got_shutdown = DSK_TRUE;
}

DSK_OBJECT_CLASS_DEFINE_CACHE_DATA (DskMemorySource);
DskMemorySourceClass dsk_memory_source_class =
{
  {
    DSK_OBJECT_CLASS_DEFINE(DskMemorySource,
                            &dsk_octet_source_class,
                            dsk_memory_source_init,
                            dsk_memory_source_finalize),
    dsk_memory_source_read,
    dsk_memory_source_read_buffer,
    dsk_memory_source_shutdown
  }
};

void dsk_memory_source_done_adding (DskMemorySource *source)
{
  source->done_adding = DSK_TRUE;
  dsk_hook_set_idle_notify (&source->buffer_low, DSK_FALSE);
  dsk_hook_set_idle_notify (&DSK_OCTET_SOURCE (source)->readable_hook, DSK_TRUE);
}

void dsk_memory_source_added_data  (DskMemorySource *source)
{
  dsk_assert (!source->done_adding);
  if (source->buffer.size != 0)
    dsk_hook_set_idle_notify (&DSK_OCTET_SOURCE (source)->readable_hook, DSK_TRUE);
  if (source->buffer.size > source->buffer_low_amount)
    dsk_hook_set_idle_notify (&source->buffer_low, DSK_FALSE);
}

void
dsk_memory_source_take_error (DskMemorySource *source,
                              DskError        *error)
{
  if (source->error)
    dsk_error_unref (source->error);
  source->error = error;
  if (source->error != NULL)
    dsk_hook_set_idle_notify (&DSK_OCTET_SOURCE (source)->readable_hook, DSK_TRUE);
}
