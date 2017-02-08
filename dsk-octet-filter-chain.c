#include <alloca.h>
#include <string.h>
#include "dsk.h"

typedef struct _DskOctetFilterChainClass DskOctetFilterChainClass;
typedef struct _DskOctetFilterChain DskOctetFilterChain;

struct _DskOctetFilterChainClass
{
  DskOctetFilterClass base_class;
};

struct _DskOctetFilterChain
{
  DskOctetFilter base_instance;
  unsigned n_filters;           /* n_filters >= 2 */
  DskOctetFilter **filters;
};

static dsk_boolean
dsk_octet_filter_chain_process (DskOctetFilter *filter,
                                DskBuffer      *out,
                                unsigned        in_length,
                                const uint8_t  *in_data,
                                DskError      **error)
{
  DskBuffer b1 = DSK_BUFFER_INIT;
  DskBuffer b2 = DSK_BUFFER_INIT;
  DskOctetFilterChain *chain = (DskOctetFilterChain *) filter;
  unsigned i;

  /* first filter */
  if (!dsk_octet_filter_process (chain->filters[0], &b1, in_length, in_data,
                                 error))
    return DSK_FALSE;

  /* middle filters */
  for (i = 1; i + 1 < chain->n_filters; i++)
    {
      DskBuffer *in_buf = i%2 == 1  ? &b1 : &b2;
      DskBuffer *out_buf = i%2 == 1 ? &b2 : &b1;
      if (!dsk_octet_filter_process_buffer (chain->filters[i], out_buf,
                                            in_buf->size, in_buf,
                                            DSK_TRUE, error))
        return DSK_FALSE;
    }

  /* last filter */
  {
    DskBuffer *in_buf = i%2 == 1  ? &b1 : &b2;
    if (!dsk_octet_filter_process_buffer (chain->filters[i], out,
                                          in_buf->size, in_buf,
                                          DSK_TRUE, error))
      return DSK_FALSE;
  }
  return DSK_TRUE;
}

static dsk_boolean
dsk_octet_filter_chain_finish  (DskOctetFilter *filter,
                                DskBuffer      *out,
                                DskError      **error)
{
  unsigned i, j;
  DskOctetFilterChain *chain = (DskOctetFilterChain *) filter;
  for (i = 0; i < chain->n_filters; i++)
    {
      DskBuffer b1 = DSK_BUFFER_INIT;
      DskBuffer b2 = DSK_BUFFER_INIT;
      if (!dsk_octet_filter_finish (chain->filters[i],
                                    (i+1 == chain->n_filters) ? out : &b1,
                                    error))
        return DSK_FALSE;
      for (j = i + 1; b1.size && j < chain->n_filters; j++)
        {
          DskBuffer swap;
          if (!dsk_octet_filter_process_buffer (chain->filters[j],
                                                (j+1) == chain->n_filters ? out : &b2,
                                                b1.size, &b1, DSK_TRUE, error))
            return DSK_FALSE;
          swap = b1;
          b1 = b2;
          b2 = swap;
        }
    }
  return DSK_TRUE;
}
static void
dsk_octet_filter_chain_finalize (DskOctetFilterChain *chain)
{
  unsigned i;
  for (i = 0; i < chain->n_filters; i++)
    dsk_object_unref (chain->filters[i]);
  dsk_free (chain->filters);
}
#define dsk_octet_filter_chain_init NULL
DSK_OCTET_FILTER_SUBCLASS_DEFINE(static, DskOctetFilterChain, dsk_octet_filter_chain);

DskOctetFilter *
dsk_octet_filter_chain_new_take    (unsigned         n_filters,
                                    DskOctetFilter **filters)
{
  if (n_filters == 0)
    return dsk_octet_filter_identity_new ();
  else if (n_filters == 1)
    return filters[0];
  else
    {
      DskOctetFilterChain *chain = dsk_object_new (&dsk_octet_filter_chain_class);
      chain->n_filters = n_filters;
      chain->filters = DSK_NEW_ARRAY (n_filters, DskOctetFilter *);
      memcpy (chain->filters, filters, sizeof (DskOctetFilter*) * n_filters);
      return DSK_OCTET_FILTER (chain);
    }
}

DskOctetFilter *
dsk_octet_filter_chain_new_take_list(DskOctetFilter *first, ...)
{
  va_list args;
  unsigned n_filters;
  DskOctetFilter **filters;

  if (first)
    {
      n_filters = 1;
      va_start (args, first);
      while (va_arg (args, DskOctetFilter *) != NULL)
        n_filters++;
      va_end (args);
    }
  else
    n_filters = 0;

  filters = alloca (sizeof (DskOctetFilter *) * n_filters);
  if (first)
    {
      DskOctetFilter *filter;
      filters[0] = first;
      n_filters = 1;
      va_start (args, first);
      while ((filter=va_arg (args, DskOctetFilter *)) != NULL)
        filters[n_filters++] = filter;
      va_end (args);
    }
  return dsk_octet_filter_chain_new_take (n_filters, filters);
}
