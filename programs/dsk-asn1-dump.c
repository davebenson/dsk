#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "../dsk.h"

/* configuration */
static dsk_boolean verbose = DSK_FALSE;
static const char *input_filename = NULL;

int main(int argc, char **argv)
{
  dsk_cmdline_init ("ASN.1 dump",
                    "Dump ASN1 to stdout.\n",
                    NULL,
                    0);
  dsk_cmdline_permit_extra_arguments (DSK_TRUE);
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
  DskASN1Value *value = dsk_asn1_value_parse_der (asn1_size, asn1_data, &used, &pool, &error);
  if (value == NULL)
    dsk_die ("error parsing ASN.1 from %s: %s", input_filename, error->message);
    
  DskBuffer buffer = DSK_BUFFER_INIT;
  dsk_asn1_value_dump_to_buffer (value, &buffer);
  dsk_buffer_writev (&buffer, STDOUT_FILENO);
  return 0;
}
