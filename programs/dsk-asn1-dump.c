#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "../dsk.h"

/* configuration */
static bool verbose = false;
static const char *input_filename = NULL;

int main(int argc, char **argv)
{
  dsk_cmdline_init ("ASN.1 dump",
                    "Dump ASN1 to stdout.\n",
                    NULL,
                    0);
  dsk_cmdline_permit_extra_arguments (true);
  dsk_cmdline_add_string ("input", "Input ASN.1 file", "FILENAME", DSK_CMDLINE_MANDATORY, &input_filename);
  dsk_cmdline_add_boolean ("verbose", "Print extra messages", NULL, 0, &verbose);
  dsk_cmdline_process_args (&argc, &argv);

  DskError *error = NULL;
  size_t asn1_size;
  uint8_t *asn1_data = dsk_file_get_contents (input_filename, &asn1_size, &error);
  if (asn1_data == NULL)
    dsk_die ("error reading input file: %s", error->message);
  DskMemPool pool = DSK_MEM_POOL_STATIC_INIT;
  size_t used;
  while (asn1_size > 0)
    {
      DskASN1Value *value = dsk_asn1_value_parse_der (asn1_size, asn1_data, &used, &pool, &error);
      if (value == NULL)
        dsk_die ("error parsing ASN.1 from %s: %s", input_filename, error->message);
        
      DskBuffer buffer = DSK_BUFFER_INIT;
      dsk_asn1_value_dump_to_buffer (value, &buffer);
      dsk_buffer_writev (&buffer, STDOUT_FILENO);

      asn1_size -= used;
      asn1_data += used;
      printf("asn1_size = %u\n",(unsigned)asn1_size);
      if (asn1_size > 0)
        printf("\n-------------------------\n\n");
    }
  return 0;
}
