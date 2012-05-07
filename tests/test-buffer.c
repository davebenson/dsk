#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "../dsk.h"

void random_slice(DskBuffer* buf)
{
  DskBuffer tmpbuf;
  char *copy, *copy_at;
  unsigned orig_size = buf->size;
  dsk_buffer_init (&tmpbuf);
  copy = dsk_malloc (buf->size);
  copy_at = copy;
  while (1)
    {
      int r;
      r = (rand () % 16384) + 1;
      r = dsk_buffer_read (buf, r, copy_at);
      dsk_buffer_append (&tmpbuf, r, copy_at);
      if (r == 0)
	break;
    }
  dsk_assert (copy_at == copy + orig_size);
  dsk_assert (buf->size == 0);
  dsk_assert (tmpbuf.size == orig_size);
  copy_at = dsk_malloc (orig_size);
  dsk_assert (dsk_buffer_read (&tmpbuf, orig_size, copy_at) == orig_size);
  dsk_assert (dsk_buffer_read (&tmpbuf, orig_size, copy_at) == 0);
  dsk_assert (memcmp (copy, copy_at, orig_size) == 0);
  dsk_free (copy);
  dsk_free (copy_at);
}

void count(DskBuffer* buf, int start, int end)
{
  char b[1024];
  while (start <= end)
    {
      sprintf (b, "%d\n", start++);
      dsk_buffer_append (buf, strlen (b), b);
    }
}

void decount(DskBuffer* buf, int start, int end)
{
  char b[1024];
  while (start <= end)
    {
      char *rv;
      sprintf (b, "%d", start++);
      rv = dsk_buffer_read_line (buf);
      dsk_assert (rv != NULL);
      dsk_assert (strcmp (b, rv) == 0);
      dsk_free (rv);
    }
}

/* Return whether 'buffer' begins with 'str'; remove it if so. */
static dsk_boolean
try_initial_remove (DskBuffer *buffer, const char *str)
{
  unsigned len = strlen (str);
  if (buffer->size < len)
    return DSK_FALSE;
  char *tmp = dsk_malloc (len);
  dsk_buffer_read (buffer, len, tmp);
  dsk_boolean rv = memcmp (str, tmp, len) == 0;
  dsk_free (tmp);
  return rv;
}

static char *generate_str (unsigned min_length, unsigned max_length)
{
  unsigned len = dsk_random_int_range (min_length, max_length);
  char *rv = dsk_malloc (len + 1);
  unsigned i;
  for (i = 0; i < len; i++)
    rv[i] = dsk_random_int_range (0, 26)["abcdefghijklmnopqrstuvwxyz"];
  rv[i] = 0;
  return rv;
}

int main(int argc, char** argv)
{

  DskBuffer gskbuffer;
  char buf[1024];
  char *str;

  dsk_cmdline_init ("test dsk-buffer code", "test DskBuffer", NULL, 0);
  dsk_cmdline_process_args (&argc, &argv);

  dsk_buffer_init (&gskbuffer);
  dsk_assert (gskbuffer.size == 0);
  dsk_buffer_append (&gskbuffer, 5, "hello");
  dsk_assert (gskbuffer.size == 5);
  dsk_assert (dsk_buffer_read (&gskbuffer, sizeof (buf), buf) == 5);
  dsk_assert (memcmp (buf, "hello", 5) == 0);
  dsk_assert (gskbuffer.size == 0);
  dsk_buffer_clear (&gskbuffer);

  dsk_buffer_init (&gskbuffer);
  count (&gskbuffer, 1, 100000);
  decount (&gskbuffer, 1, 100000);
  dsk_assert (gskbuffer.size == 0);
  dsk_buffer_clear (&gskbuffer);

  dsk_buffer_init (&gskbuffer);
  dsk_buffer_append_string (&gskbuffer, "hello\na\nb");
  str = dsk_buffer_read_line (&gskbuffer);
  dsk_assert (str);
  dsk_assert (strcmp (str, "hello") == 0);
  dsk_free (str);
  str = dsk_buffer_read_line (&gskbuffer);
  dsk_assert (str);
  dsk_assert (strcmp (str, "a") == 0);
  dsk_free (str);
  dsk_assert (gskbuffer.size == 1);
  dsk_assert (dsk_buffer_read_line (&gskbuffer) == NULL);
  dsk_buffer_append_byte (&gskbuffer, '\n');
  str = dsk_buffer_read_line (&gskbuffer);
  dsk_assert (str);
  dsk_assert (strcmp (str, "b") == 0);
  dsk_free (str);
  dsk_assert (gskbuffer.size == 0);
  dsk_buffer_clear (&gskbuffer);

  dsk_buffer_init (&gskbuffer);
  dsk_buffer_append (&gskbuffer, 5, "hello");
  dsk_buffer_append_foreign (&gskbuffer, 4, "test", NULL, NULL);
  dsk_buffer_append (&gskbuffer, 5, "hello");
  dsk_assert (gskbuffer.size == 14);
  dsk_assert (dsk_buffer_read (&gskbuffer, sizeof (buf), buf) == 14);
  dsk_assert (memcmp (buf, "hellotesthello", 14) == 0);
  dsk_assert (gskbuffer.size == 0);

  /* Test that the foreign data really is not being stored in the DskBuffer */
  {
    char test_str[5];
    strcpy (test_str, "test");
    dsk_buffer_init (&gskbuffer);
    dsk_buffer_append (&gskbuffer, 5, "hello");
    dsk_buffer_append_foreign (&gskbuffer, 4, test_str, NULL, NULL);
    dsk_buffer_append (&gskbuffer, 5, "hello");
    dsk_assert (gskbuffer.size == 14);
    dsk_assert (dsk_buffer_peek (&gskbuffer, sizeof (buf), buf) == 14);
    dsk_assert (memcmp (buf, "hellotesthello", 14) == 0);
    test_str[1] = '3';
    dsk_assert (gskbuffer.size == 14);
    dsk_assert (dsk_buffer_read (&gskbuffer, sizeof (buf), buf) == 14);
    dsk_assert (memcmp (buf, "hellot3sthello", 14) == 0);
    dsk_buffer_clear (&gskbuffer);
  }

  /* Test str_index_of */
  {
    DskBuffer buffer = DSK_BUFFER_INIT;
    dsk_buffer_append_foreign (&buffer, 3, "abc", NULL, NULL);
    dsk_buffer_append_foreign (&buffer, 3, "def", NULL, NULL);
    dsk_buffer_append_foreign (&buffer, 3, "gad", NULL, NULL);
#if 0
    dsk_assert (dsk_buffer_str_index_of (&buffer, "cdefg") == 2);
    dsk_assert (dsk_buffer_str_index_of (&buffer, "ad") == 7);
    dsk_assert (dsk_buffer_str_index_of (&buffer, "ab") == 0);
    dsk_assert (dsk_buffer_str_index_of (&buffer, "a") == 0);
    dsk_assert (dsk_buffer_str_index_of (&buffer, "g") == 6);
#endif
    dsk_buffer_clear (&buffer);
  }

  static const char *before_strs[] = {
    "",
    "foo",
    NULL, NULL
  };
  before_strs[2] = generate_str (100, 1000);
  before_strs[3] = generate_str (10000, 100000);
  static const char *placeholder_strs[] = {
    "",
    "bar",
    NULL, NULL, NULL
  };
  placeholder_strs[2] = generate_str (100, 1000);
  placeholder_strs[3] = generate_str (10000, 100000);
  placeholder_strs[4] = generate_str (100000, 1000000);
  static const char *after_strs[] = {
    "",
    "foo",
    NULL, NULL
  };
  after_strs[2] = generate_str (100, 1000);
  after_strs[3] = generate_str (10000, 100000);
  unsigned bi, pi, ai;
  for (bi = 0; bi < DSK_N_ELEMENTS (before_strs); bi++)
    for (pi = 0; pi < DSK_N_ELEMENTS (placeholder_strs); pi++)
      for (ai = 0; ai < DSK_N_ELEMENTS (after_strs); ai++)
        {
          DskBuffer buffer = DSK_BUFFER_INIT;
          const char *pi_str = placeholder_strs[pi];
          DskBufferPlaceholder placeholder;
          dsk_buffer_append_string (&buffer, before_strs[bi]);
          dsk_buffer_append_placeholder (&buffer, strlen (pi_str), &placeholder);
          dsk_buffer_append_string (&buffer, after_strs[ai]);
          dsk_buffer_placeholder_set (&placeholder, pi_str);
          dsk_assert (try_initial_remove (&buffer, before_strs[bi]));
          dsk_assert (try_initial_remove (&buffer, pi_str));
          dsk_assert (try_initial_remove (&buffer, after_strs[ai]));
          dsk_assert (buffer.size == 0);
        }

  return 0;
}
