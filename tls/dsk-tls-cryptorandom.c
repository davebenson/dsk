#include "../dsk.h"
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>

void dsk_get_cryptorandom_data (size_t length,
                                uint8_t *data)
{
  static int fd = -1;
  if (fd < 0)
    {
      fd = open("/dev/urandom", O_RDONLY);
      if (fd < 0)
        dsk_die("error reading /dev/urandom: %s", strerror (errno));
    }
  while (length > 0)
    {
      ssize_t nread = read (fd, data, length);
      if (nread < 0)
        {
          if (errno == EINTR)
            continue;
          dsk_die ("error reading random data: %s", strerror (errno));
        }
      length -= nread;
      data += nread;
    }
}
