#include <alloca.h>
#include <string.h>
#include "dsk.h"

typedef struct _Component Component;
struct _Component
{
  const char *start;
  unsigned len;
};

dsk_boolean dsk_path_normalize_inplace (char *path_inout)
{
  if (path_inout[0] == 0)
    return DSK_FALSE;

  /* Split into pieces */
  unsigned n_slash = 0;
  const char *in = path_inout;
  while (*in)
    if (*in++ == DSK_DIR_SEPARATOR)
      n_slash++;
  Component *comps = alloca (sizeof (Component) * (n_slash+1));
  unsigned n_comps = 0;
  in = path_inout;
  while (*in)
    {
      while (*in == DSK_DIR_SEPARATOR)
        in++;
      if (*in == '\0')
        break;
      unsigned len = 1;
      while (in[len] && in[len] != DSK_DIR_SEPARATOR)
        len++;
      comps[n_comps].start = in;
      comps[n_comps].len = len;
      n_comps++;
      in += len;
    }

  /* Handle . and .. */
  {
    unsigned o = 0;
    unsigned i = 0;
    for (i = 0; i < n_comps; i++)
      if (comps[i].len == 1 && comps[i].start[0] == '.')
        continue;
      else if (comps[i].len == 2 && comps[i].start[0] == '.' && comps[i].start[1] == '.')
        {
          if (o)
            o--;
        }
      else
        comps[o++] = comps[i];
    n_comps = o;
  }

  char *out = path_inout;
  if (*out == DSK_DIR_SEPARATOR)
    out++;
  if (n_comps)
    {
      memmove (out, comps[0].start, comps[0].len);
      out += comps[0].len;
      unsigned i;
      for (i = 1; i < n_comps; i++)
        {
          *out++ = DSK_DIR_SEPARATOR;
          memmove (out, comps[i].start, comps[i].len);
          out += comps[i].len;
        }
    }
  *out = 0;
  return DSK_TRUE;
}
