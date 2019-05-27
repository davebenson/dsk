#include "dsk.h"

typedef struct _DskStreamSourceFilterClass DskStreamSourceFilterClass;
typedef struct _DskStreamSourceFilter DskStreamSourceFilter;

struct _DskStreamSourceFilterClass
{
  DskStreamClass base_class;
};

struct _DskStreamSourceFilter
{
  DskStream base_instance;
  DskBuffer buffer;
  DskSyncFilter *filter;
  DskStream *sub;
  DskHookTrap *read_trap;
};

static bool
read_into_buffer (DskStreamSourceFilter *filter,
                  DskError            **error)
{
  DskBuffer tmp = DSK_BUFFER_INIT;
  switch (dsk_stream_read_buffer (filter->sub, &tmp, error))
    {
    case DSK_IO_RESULT_SUCCESS:
      break;
    case DSK_IO_RESULT_ERROR:
      return false;
    case DSK_IO_RESULT_EOF:
      {
        if (filter->read_trap)
          {
            dsk_hook_trap_destroy (filter->read_trap);
            filter->read_trap = NULL;
          }
        dsk_object_unref (filter->sub);
        filter->sub = NULL;

        if (!dsk_sync_filter_finish (filter->filter, &filter->buffer, error))
          return false;
        dsk_object_unref (filter->filter);
        filter->filter = NULL;
        return true;
      }
    case DSK_IO_RESULT_AGAIN:
      return true;
    }
  if (!dsk_sync_filter_process_buffer (filter->filter, &filter->buffer,
                                        tmp.size, &tmp, true, error))
    {
      dsk_buffer_clear (&tmp);
      return false;
    }
  return true;
}

static bool
notify_filter_readable (DskStream *sub,
                        DskStreamSourceFilter *sf)
{
  DSK_UNUSED (sub);
  dsk_hook_notify (&sf->base_instance.readable_hook);
  if (!dsk_hook_is_trapped (&sf->base_instance.readable_hook)
   || sf->filter == NULL)
    {
      sf->read_trap = NULL;
      return false;
    }
  return true;
}


static DskIOResult
dsk_stream_source_filter_read (DskStream *source,
                              unsigned        max_len,
                              void           *data_out,
                              unsigned       *bytes_read_out,
                              DskError      **error)
{
  DskStreamSourceFilter *sf = (DskStreamSourceFilter *) source;
  if (sf->buffer.size == 0)
    {
      if (sf->filter == NULL)
        return DSK_IO_RESULT_EOF;
      if (!read_into_buffer (sf, error))
        return DSK_IO_RESULT_ERROR;
      if (sf->buffer.size == 0)
        return sf->filter == NULL ? DSK_IO_RESULT_EOF : DSK_IO_RESULT_AGAIN;
    }
  *bytes_read_out = dsk_buffer_read (&sf->buffer, max_len, data_out);
  dsk_hook_set_idle_notify (&source->readable_hook, sf->buffer.size > 0 || sf->filter == NULL);
  if (sf->buffer.size == 0
   && sf->sub != NULL
   && sf->read_trap == NULL
   && dsk_hook_is_trapped (&source->readable_hook))
    sf->read_trap = dsk_hook_trap (&sf->sub->readable_hook,
                                   (DskHookFunc) notify_filter_readable,
                                   sf, NULL);
  return DSK_IO_RESULT_SUCCESS;
}

static DskIOResult
dsk_stream_source_filter_read_buffer (DskStream *source,
                                     DskBuffer      *read_buffer,
                                     DskError      **error)
{
  DskStreamSourceFilter *sf = (DskStreamSourceFilter *) source;
  if (sf->buffer.size == 0)
    {
      if (sf->filter == NULL)
        return DSK_IO_RESULT_EOF;
      /* XXX: results in extra copy sometimes */
      if (!read_into_buffer (sf, error))
        return DSK_IO_RESULT_ERROR;
      if (sf->buffer.size == 0)
        return sf->filter == NULL ? DSK_IO_RESULT_EOF : DSK_IO_RESULT_AGAIN;
    }
  dsk_buffer_drain (read_buffer, &sf->buffer);
  dsk_hook_set_idle_notify (&source->readable_hook, sf->filter == NULL);
  if (sf->sub != NULL
   && sf->read_trap == NULL
   && dsk_hook_is_trapped (&source->readable_hook))
    sf->read_trap = dsk_hook_trap (&sf->sub->readable_hook,
                                   (DskHookFunc) notify_filter_readable,
                                   sf, NULL);
  return DSK_IO_RESULT_SUCCESS;
}

static void       
dsk_stream_source_filter_shutdown_read  (DskStream *source)
{
  DskStreamSourceFilter *sf = (DskStreamSourceFilter *) source;
  if (sf->read_trap)
    {
      dsk_hook_trap_destroy (sf->read_trap);
      sf->read_trap = NULL;
    }
  if (sf->sub)
    {
      dsk_stream_shutdown_read (sf->sub);
      dsk_object_unref (sf->sub);
      sf->sub = NULL;
    }
  if (sf->filter)
    {
      dsk_object_unref (sf->filter);
      sf->filter = NULL;
    }
  dsk_buffer_reset (&sf->buffer);
  dsk_hook_set_idle_notify (&source->readable_hook, true);
}

static void
dsk_stream_source_filter_finalize (DskStreamSourceFilter *sf)
{
  if (sf->read_trap != NULL)
    dsk_hook_trap_destroy (sf->read_trap);
  if (sf->sub != NULL)
    dsk_object_unref (sf->sub);
  if (sf->filter != NULL)
    dsk_object_unref (sf->filter);
  dsk_buffer_clear (&sf->buffer);
}

DSK_OBJECT_CLASS_DEFINE_CACHE_DATA (DskStreamSourceFilter);
static DskStreamSourceFilterClass dsk_stream_source_filter_class =
{
  { DSK_OBJECT_CLASS_DEFINE (DskStreamSourceFilter, &dsk_stream_source_filter_class, 
                             NULL, dsk_stream_source_filter_finalize),
    dsk_stream_source_filter_read,
    dsk_stream_source_filter_read_buffer,
    dsk_stream_source_filter_shutdown_read,
    NULL,
    NULL,
    NULL,
  }
};

static void
set_poll   (void       *object,
            bool is_trapped)
{
  DskStreamSourceFilter *sf = (DskStreamSourceFilter *) object;
  if (sf->sub == NULL)
    return;
  if (is_trapped)
    {
      if (sf->read_trap == NULL)
        sf->read_trap = dsk_hook_trap (&sf->sub->readable_hook,
                                       (DskHookFunc) notify_filter_readable,
                                       sf, NULL);
    }
  else
    {
      if (sf->read_trap != NULL)
        {
          dsk_hook_trap_destroy (sf->read_trap);
          sf->read_trap = NULL;
        }
    }
}

static DskHookFuncs poll_funcs = 
{
  (DskHookObjectFunc) dsk_object_ref_f,
  (DskHookObjectFunc) dsk_object_unref_f,
  (DskHookSetPoll) set_poll
};


DskStream *dsk_sync_filter_source (DskStream *source,
                                         DskSyncFilter *filter)
{
  DskStreamSourceFilter *sf = dsk_object_new (&dsk_stream_source_filter_class);
  sf->sub = dsk_object_ref (source);
  sf->filter = dsk_object_ref (filter);
  dsk_hook_set_funcs (&sf->base_instance.readable_hook, &poll_funcs);
  return DSK_STREAM (sf);
}
