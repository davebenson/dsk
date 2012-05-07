#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include "../dsk.h"

static void
handle_fd (DskPattern *pattern,
           const char *prefix_filename,
           unsigned    buffer_size,
           int         fd)
{
  DskBuffer in = DSK_BUFFER_INIT;
  DskBuffer out = DSK_BUFFER_INIT;
  for (;;)
    {
      int rv = dsk_buffer_readv (&in, fd);
      if (rv == 0)
        break;
      char *line;
      while ((line = dsk_buffer_read_line (&in)) != NULL)
        {
          if (dsk_pattern_match (pattern, line))
            {
              if (prefix_filename)
                {
                  dsk_buffer_append_string (&out, prefix_filename);
                  dsk_buffer_append_string (&out, ": ");
                }
              dsk_buffer_append_string (&out, line);
              dsk_buffer_append_byte (&out, '\n');

              while (out.size > buffer_size)
                dsk_buffer_writev (&out, STDOUT_FILENO);
            }
          dsk_free (line);
        }
    }
  while (out.size > 0)
    dsk_buffer_writev (&out, STDOUT_FILENO);
}

int main(int argc, char **argv)
{
  DskPattern *pattern;
  DskError *error = NULL;
  dsk_cmdline_init ("grep-like tool",
                    "Implement 'grep' using DskPattern",
		    "PATTERN [FILES...]",
		    DSK_CMDLINE_PERMIT_ARGUMENTS);

  dsk_cmdline_process_args (&argc, &argv);

  if (argc < 2)
    dsk_die ("missing pattern");
  DskPatternEntry entry = { argv[1], "nonnull" };
  pattern = dsk_pattern_compile (1, &entry, &error);
  if (pattern == NULL)
    dsk_die ("error compiling pattern: %s", error->message);

  unsigned buffer_size = isatty (STDOUT_FILENO) ? 0 : 8192;
  
  if (argc == 2)
    handle_fd (pattern, NULL, buffer_size, 0);
  else
    {
      int i;
      for (i = 2; i < argc; i++)
        {
          int fd = open (argv[i], O_RDONLY);
          if (fd < 0)
            dsk_die ("error opening %s: %s", argv[i], strerror (errno));
          handle_fd (pattern, argc == 3 ? NULL : argv[i], buffer_size, fd);
          close (fd);
        }
    }
  return 0;
}


