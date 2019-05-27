#include <alloca.h>
#include <string.h>
#include "dsk.h"

typedef struct _DskSyncFilterChainClass DskSyncFilterChainClass;
typedef struct _DskSyncFilterChain DskSyncFilterChain;

struct _DskSyncFilterChainClass
{
  DskSyncFilterClass base_class;
};

struct _DskSyncFilterChain
{
  DskSyncFilter base_instance;
  unsigned n_filters;           /* n_filters >= 2 */
  DskSyncFilter **filters;
};

static bool
dsk_sync_filter_chain_process (DskSyncFilter *filter,
                                DskBuffer      *out,
                                unsigned        in_length,
                                const uint8_t  *in_data,
                                DskError      **error)
{
  DskBuffer b1 = DSK_BUFFER_INIT;
  DskBuffer b2 = DSK_BUFFER_INIT;
  DskSyncFilterChain *chain = (DskSyncFilterChain *) filter;
  unsigned i;

  /* first filter */
  if (!dsk_sync_filter_process (chain->filters[0], &b1, in_length, in_data,
                                 error))
    return false;

  /* middle filters */
  for (i = 1; i + 1 < chain->n_filters; i++)
    {
      DskBuffer *in_buf = i%2 == 1  ? &b1 : &b2;
      DskBuffer *out_buf = i%2 == 1 ? &b2 : &b1;
      if (!dsk_sync_filter_process_buffer (chain->filters[i], out_buf,
                                            in_buf->size, in_buf,
                                            true, error))
        return false;
    }

  /* last filter */
  {
    DskBuffer *in_buf = i%2 == 1  ? &b1 : &b2;
    if (!dsk_sync_filter_process_buffer (chain->filters[i], out,
                                          in_buf->size, in_buf,
                                          true, error))
      return false;
  }
  return true;
}

static bool
dsk_sync_filter_chain_finish  (DskSyncFilter *filter,
                                DskBuffer      *out,
                                DskError      **error)
{
  unsigned i, j;
  DskSyncFilterChain *chain = (DskSyncFilterChain *) filter;
  for (i = 0; i < chain->n_filters; i++)
    {
      DskBuffer b1 = DSK_BUFFER_INIT;
      DskBuffer b2 = DSK_BUFFER_INIT;
      if (!dsk_sync_filter_finish (chain->filters[i],
                                    (i+1 == chain->n_filters) ? out : &b1,
                                    error))
        return false;
      for (j = i + 1; b1.size && j < chain->n_filters; j++)
        {
          DskBuffer swap;
          if (!dsk_sync_filter_process_buffer (chain->filters[j],
                                                (j+1) == chain->n_filters ? out : &b2,
                                                b1.size, &b1, true, error))
            return false;
          swap = b1;
          b1 = b2;
          b2 = swap;
        }
    }
  return true;
}
static void
dsk_sync_filter_chain_finalize (DskSyncFilterChain *chain)
{
  unsigned i;
  for (i = 0; i < chain->n_filters; i++)
    dsk_object_unref (chain->filters[i]);
  dsk_free (chain->filters);
}
#define dsk_sync_filter_chain_init NULL
DSK_SYNC_FILTER_SUBCLASS_DEFINE(static, DskSyncFilterChain, dsk_sync_filter_chain);

DskSyncFilter *
dsk_sync_filter_chain_new_take    (unsigned         n_filters,
                                    DskSyncFilter **filters)
{
  if (n_filters == 0)
    return dsk_sync_filter_identity_new ();
  else if (n_filters == 1)
    return filters[0];
  else
    {
      DskSyncFilterChain *chain = dsk_object_new (&dsk_sync_filter_chain_class);
      chain->n_filters = n_filters;
      chain->filters = DSK_NEW_ARRAY (n_filters, DskSyncFilter *);
      memcpy (chain->filters, filters, sizeof (DskSyncFilter*) * n_filters);
      return DSK_SYNC_FILTER (chain);
    }
}

DskSyncFilter *
dsk_sync_filter_chain_new_take_list(DskSyncFilter *first, ...)
{
  va_list args;
  unsigned n_filters;
  DskSyncFilter **filters;

  if (first)
    {
      n_filters = 1;
      va_start (args, first);
      while (va_arg (args, DskSyncFilter *) != NULL)
        n_filters++;
      va_end (args);
    }
  else
    n_filters = 0;

  filters = alloca (sizeof (DskSyncFilter *) * n_filters);
  if (first)
    {
      DskSyncFilter *filter;
      filters[0] = first;
      n_filters = 1;
      va_start (args, first);
      while ((filter=va_arg (args, DskSyncFilter *)) != NULL)
        filters[n_filters++] = filter;
      va_end (args);
    }
  return dsk_sync_filter_chain_new_take (n_filters, filters);
}
