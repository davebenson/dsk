#include "../dsk.h"
#include <string.h>

int main()
{

  char **tmp;
  tmp = dsk_strsplit("a,b,c,d,e,f", ",");
  assert(dsk_strv_length (tmp) == 6);
  assert(strcmp(tmp[0], "a") == 0);
  assert(strcmp(tmp[1], "b") == 0);
  assert(strcmp(tmp[2], "c") == 0);
  assert(strcmp(tmp[3], "d") == 0);
  assert(strcmp(tmp[4], "e") == 0);
  assert(strcmp(tmp[5], "f") == 0);
  dsk_strv_free (tmp);

  tmp = dsk_strsplit("abcdef", ",");
  assert(dsk_strv_length (tmp) == 1);
  assert(strcmp(tmp[0], "abcdef") == 0);
  dsk_strv_free (tmp);

  return 0;
}
