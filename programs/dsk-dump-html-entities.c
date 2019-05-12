#include "../dsk.h"
#include <stdio.h>

int main()
{
#define PRINT_ENTITY(shortname) \
  printf("%s: %s (0x%06x)\n", DSK_HTML_ENTITY_UTF8_##shortname, #shortname, DSK_HTML_ENTITY_UNICODE_##shortname);
  DSK_FOREACH_HTML_ENTITY(PRINT_ENTITY)
  return 0;
}
