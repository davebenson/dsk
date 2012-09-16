#include <time.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include "../dsk.h"

static DskDir *location;
static dsk_boolean cmdline_verbose = DSK_FALSE;
static dsk_boolean cmdline_slow = DSK_FALSE;
static dsk_boolean cmdline_keep_testdir = DSK_FALSE;



static void
test_simple_write_read (void)
{
  DskError *error = NULL;
  DskTableFileInterface *iface = &dsk_table_file_interface_trivial;

  DskTableFileWriter *writer = iface->new_writer (iface, location, "base", &error);
  DskTableReader *reader;
  if (writer == NULL)
    dsk_die ("%s", error->message);
  if (!writer->write (writer, 1, (uint8_t*) "a", 1, (uint8_t*) "A", &error)
   || !writer->write (writer, 1, (uint8_t*) "b", 1, (uint8_t*) "B", &error)
   || !writer->write (writer, 1, (uint8_t*) "c", 1, (uint8_t*) "C", &error))
    dsk_die ("error writing: %s", error->message);
  if (!writer->close (writer, &error))
    dsk_die ("error closing writer: %s", error->message);
  writer->destroy (writer);

  reader = iface->new_reader (iface, location, "base", &error);
  if (reader == NULL)
    dsk_die ("error creating reader: %s", error->message);
  dsk_assert (!reader->at_eof);
  dsk_assert (reader->key_length == 1);
  dsk_assert (reader->key_data[0] == 'a');
  dsk_assert (reader->value_length == 1);
  dsk_assert (reader->value_data[0] == 'A');
  if (!reader->advance (reader, &error))
    dsk_die ("error advancing reader: %s", error->message);
  dsk_assert (!reader->at_eof);
  dsk_assert (reader->key_length == 1);
  dsk_assert (reader->key_data[0] == 'b');
  dsk_assert (reader->value_length == 1);
  dsk_assert (reader->value_data[0] == 'B');
  if (!reader->advance (reader, &error))
    dsk_die ("error advancing reader: %s", error->message);
  dsk_assert (!reader->at_eof);
  dsk_assert (reader->key_length == 1);
  dsk_assert (reader->key_data[0] == 'c');
  dsk_assert (reader->value_length == 1);
  dsk_assert (reader->value_data[0] == 'C');
  if (!reader->advance (reader, &error))
    dsk_die ("error advancing reader: %s", error->message);
  dsk_assert (reader->at_eof);
  reader->destroy (reader);
}


/* ---- Infrastructure (etc) for stress-testing of
        DskTableWriter/Reader performing the identity --- */
/* A_n is 10^n characters. */
#define A_1       "AAAAAAAAAA"
#define A_2       A_1 A_1 A_1 A_1 A_1 A_1 A_1 A_1 A_1 A_1
#define A_3       A_2 A_2 A_2 A_2 A_2 A_2 A_2 A_2 A_2 A_2
#define A_4       A_3 A_3 A_3 A_3 A_3 A_3 A_3 A_3 A_3 A_3
#define A_5       A_4 A_4 A_4 A_4 A_4 A_4 A_4 A_4 A_4 A_4
#define A_6       A_4 A_4 A_4 A_4 A_4 A_4 A_4 A_4 A_4 A_4

#define B_1       "BBBBBBBBBB"
#define B_2       B_1 B_1 B_1 B_1 B_1 B_1 B_1 B_1 B_1 B_1
#define B_3       B_2 B_2 B_2 B_2 B_2 B_2 B_2 B_2 B_2 B_2
#define B_4       B_3 B_3 B_3 B_3 B_3 B_3 B_3 B_3 B_3 B_3
#define B_5       B_4 B_4 B_4 B_4 B_4 B_4 B_4 B_4 B_4 B_4
#define B_6       B_4 B_4 B_4 B_4 B_4 B_4 B_4 B_4 B_4 B_4
typedef struct
{
  const char *key;
  const char *value;
} TestEntry;

static TestEntry testdata__small_keys_0[] = {
  { "a", "b" },
  { "b", "B" },
  { "c", "C" },
  { "d", "D" },
};
static TestEntry testdata__small_keys_1[] = {
  { "a", "b" },
  { "b", "B" },
  { "c", "C" },
  { "d", "D" },
  { "f", "F" },
  { "g", "G" },
  { "d", "H" },
};
static TestEntry testdata__small_keys_2[] = {
  { "aa", "b" },
  { "bb", "B" },
  { "cc", "C" },
  { "dd", "D" },
  { "ff", "F" },
  { "gg", "G" },
  { "dd", "H" },
};
static TestEntry testdata__small_keys_3[] = {
  { "aaaaaa", "b" },
  { "bbbbbb", "B" },
  { "cccccc", "C" },
  { "dddddd", "D" },
  { "ffffff", "F" },
  { "gggggg", "G" },
  { "dddddd", "H" },
};
static TestEntry testdata__small_keys_4[] = {
  { "aaaaqa", "bbbbbbbbb" },
  { "bbbbb", "BBBBBBBB" },
  { "ccccc", "CCCCCCCC" },
  { "ddddd", "DDDDDDDDD" },
  { "fffff", "FFFFFFFF" },
  { "ggggg", "GGGGGGGG" },
  { "ddddd", "HHHHHHHH" },
};
static TestEntry testdata__various_keys[] = {
  { A_1, "bbbbbbbbb" },
  { A_2, "BBBBBBBB" },
  { A_4, "CCCCCCCC" },
  { A_3, "DDDDDDDDD" },
  { A_4, "FFFFFFFF" },
  { A_6, "GGGGGGGG" },
  { A_5, "HHHHHHHH" },
};
static TestEntry testdata__various_values[] = {
  { "aaaaqa", A_1 },
  { "bbbbb",  A_2 },
  { "ccccc",  A_2 },
  { "ddddd",  A_3 },
  { "fffff",  A_4 },
  { "ggggg",  A_6 },
  { "ddddd",  A_5 },
};
static TestEntry testdata__various_keys_values_0[] = {
  { A_1, A_1 },
  { A_3, A_2 },
  { A_1, A_2 },
  { A_2, A_3 },
  { A_3, A_4 },
  { A_4, A_6 },
  { A_5, A_5 },
};
static TestEntry testdata__various_keys_values_1[] = {
  { B_1, A_1 },
  { A_3, B_2 },
  { B_1, B_2 },
  { A_2, B_3 },
  { B_3, A_4 },
  { B_4, A_6 },
  { A_5, B_5 },
};

struct TestDataset
{
  const char *name;
  unsigned n_entries;
  TestEntry *entries;
  uint64_t to_write;
};
static struct TestDataset test_datasets[] =
{
#define WRITE_DATASET(static_array, to_write) \
   {#static_array, DSK_N_ELEMENTS (static_array), (static_array), (to_write)}
#define WRITE_SMALL_DATASETS(to_write) \
   WRITE_DATASET (testdata__small_keys_0, to_write), \
   WRITE_DATASET (testdata__small_keys_1, to_write), \
   WRITE_DATASET (testdata__small_keys_2, to_write), \
   WRITE_DATASET (testdata__small_keys_3, to_write), \
   WRITE_DATASET (testdata__small_keys_4, to_write)
#define WRITE_LARGE_DATASETS(to_write) \
   WRITE_DATASET (testdata__various_keys, to_write), \
   WRITE_DATASET (testdata__various_values, to_write), \
   WRITE_DATASET (testdata__various_keys_values_0, to_write), \
   WRITE_DATASET (testdata__various_keys_values_1, to_write)
#define WRITE_ALL_DATASETS(to_write) \
   WRITE_SMALL_DATASETS(to_write), \
   WRITE_LARGE_DATASETS(to_write)

  WRITE_ALL_DATASETS (0),
  WRITE_ALL_DATASETS (1),
  WRITE_ALL_DATASETS (2),
  WRITE_ALL_DATASETS (3),
  WRITE_ALL_DATASETS (4),
  WRITE_ALL_DATASETS (5),
  WRITE_ALL_DATASETS (6),
  WRITE_ALL_DATASETS (7),
  WRITE_ALL_DATASETS (8),
  WRITE_ALL_DATASETS (11),
  WRITE_ALL_DATASETS (16),
  WRITE_ALL_DATASETS (19),
  WRITE_ALL_DATASETS (23),
  WRITE_ALL_DATASETS (32),
  WRITE_ALL_DATASETS (127),
  WRITE_ALL_DATASETS (128),
  WRITE_ALL_DATASETS (129),
  WRITE_ALL_DATASETS (255),
  WRITE_ALL_DATASETS (256),
  WRITE_ALL_DATASETS (257),
  WRITE_ALL_DATASETS (511),
  WRITE_ALL_DATASETS (512),
  WRITE_ALL_DATASETS (513),
  WRITE_ALL_DATASETS (1023),
  WRITE_ALL_DATASETS (1024),
  WRITE_ALL_DATASETS (1025),

  WRITE_SMALL_DATASETS (2023),
  WRITE_SMALL_DATASETS (2024),
  WRITE_SMALL_DATASETS (2025),
  WRITE_SMALL_DATASETS (2048),
  WRITE_SMALL_DATASETS (4095),
  WRITE_SMALL_DATASETS (4096),
  WRITE_SMALL_DATASETS (4097),
  WRITE_SMALL_DATASETS (65535),
  WRITE_SMALL_DATASETS (65536),
  WRITE_SMALL_DATASETS (65537),

  WRITE_SMALL_DATASETS (1024*1024-2),
  WRITE_SMALL_DATASETS (1024*1024-1),
  WRITE_SMALL_DATASETS (1024*1024),
  WRITE_SMALL_DATASETS (1024*1024+1),
  WRITE_SMALL_DATASETS (1024*1024+2),
};
static struct TestDataset test_slow_datasets[] =
{
  WRITE_LARGE_DATASETS (2023),
  WRITE_LARGE_DATASETS (2024),
  WRITE_LARGE_DATASETS (2025),
  WRITE_LARGE_DATASETS (2048),
  WRITE_LARGE_DATASETS (4095),
  WRITE_LARGE_DATASETS (4096),
  WRITE_LARGE_DATASETS (4097),
  WRITE_LARGE_DATASETS (65535),
  WRITE_LARGE_DATASETS (65536),
  WRITE_LARGE_DATASETS (65537),
};


static void
test_various_read_write_1 (const char *name,
                           unsigned   n_entries,
                           TestEntry *entries,
                           uint64_t   n_write)
{
  DskError *error = NULL;
  DskTableFileInterface *iface = &dsk_table_file_interface_trivial;
  uint64_t big_i;               /* index from 0..n_write-1 */
  uint32_t small_i;             /* index from 0..n_entries-1 */
  DskTableFileWriter *writer;
  DskTableReader *reader;

  if (cmdline_verbose)
    fprintf (stderr, "running dataset %s [%llu]\n", name, (unsigned long long) n_write);
  else
    fprintf (stderr, ".");

  writer = iface->new_writer (iface, location, "base", &error);
  if (writer == NULL)
    dsk_die ("%s", error->message);
  for (big_i = small_i = 0; big_i < n_write; big_i++)
    {
      TestEntry *e = entries + small_i++;
      if (small_i == n_entries)
        small_i = 0;
      if (!writer->write (writer,
                          strlen (e->key), (uint8_t*) e->key,
                          strlen (e->value), (uint8_t*) e->value,
                          &error))
        dsk_die ("error writing: %s", error->message);
    }
  if (!writer->close (writer, &error))
    dsk_die ("error closing writer: %s", error->message);
  writer->destroy (writer);

  reader = iface->new_reader (iface, location, "base", &error);
  if (reader == NULL)
    dsk_die ("error creating reader: %s", error->message);
  small_i = 0;
  for (big_i = 0; big_i < n_write; big_i++)
    {
      TestEntry *e = entries + small_i++;
      if (small_i == n_entries)
        small_i = 0;

      dsk_assert (!reader->at_eof);
      dsk_assert (reader->key_length == strlen (e->key));
      dsk_assert (reader->value_length == strlen (e->value));
      dsk_assert (memcmp (reader->key_data, e->key, reader->key_length) == 0);
      dsk_assert (memcmp (reader->value_data, e->value, reader->value_length) == 0);

      if (!reader->advance (reader, &error))
        dsk_die ("error reading file: %s", error->message);
      if (big_i + 1 == n_write)
        break;
    }
  dsk_assert (reader->at_eof);
  reader->destroy (reader);
}
static void
test_various_read_write (void)
{
  unsigned i;
  for (i = 0; i < DSK_N_ELEMENTS (test_datasets); i++)
    test_various_read_write_1 (test_datasets[i].name,
                               test_datasets[i].n_entries,
                               test_datasets[i].entries,
                               test_datasets[i].to_write);
  if (cmdline_slow)
    for (i = 0; i < DSK_N_ELEMENTS (test_slow_datasets); i++)
      test_various_read_write_1 (test_slow_datasets[i].name,
                                 test_slow_datasets[i].n_entries,
                                 test_slow_datasets[i].entries,
                                 test_slow_datasets[i].to_write);
}

static int
str_test_func (unsigned len,
               const uint8_t *data,
               void *func_data)
{
  unsigned func_data_len = strlen (func_data);
  int rv = memcmp (data, func_data, DSK_MIN (len, func_data_len));
  //dsk_warning ("comparing test key %s with %.*s", (char*)func_data, len, data);
  if (rv == 0)
    rv = (len < func_data_len) ? -1 : (len > func_data_len) ? 1 : 0;
  return rv;
}

static void
test_various_write_seek_1 (const char *name,
                           unsigned    n_entries,
                           TestEntry  *entries,
                           unsigned    n_negative,
                           TestEntry  *neg_entries)
{
  DskError *error = NULL;
  unsigned i;
  DskTableFileInterface *iface = &dsk_table_file_interface_trivial;
  DskTableFileWriter *writer;
  DskTableFileSeeker *seeker;

  if (cmdline_verbose)
    fprintf (stderr, "running dataset %s [%u]\n", name, n_entries);
  else
    fprintf (stderr, ".");

  writer = iface->new_writer (iface, location, "base", &error);
  if (writer == NULL)
    dsk_die ("%s", error->message);
  for (i = 0; i < n_entries; i++)
    {
      TestEntry *e = entries + i;
      if (!writer->write (writer,
                          strlen (e->key), (uint8_t*) e->key,
                          strlen (e->value), (uint8_t*) e->value,
                          &error))
        dsk_die ("error writing: %s", error->message);
    }
  if (!writer->close (writer, &error))
    dsk_die ("error closing writer: %s", error->message);
  writer->destroy (writer);

  /* --- now test seeker --- */

  /* pick the step size. */
  {
  static unsigned prime_table[] = { 29, 31, 37, 41, 43, 47, 53, 59, 61, 67 };
  unsigned *p_ptr = prime_table + DSK_N_ELEMENTS (prime_table) - 1;
  unsigned max_test, n_test, test_i, step;
  while (n_entries % *p_ptr == 0)
    p_ptr--;
  step = *p_ptr % n_entries;

  /* create seeker */
  seeker = iface->new_seeker (iface, location, "base", &error);
  if (seeker == NULL)
    dsk_die ("error creating seeker from newly finished writer: %s",
             error->message);

  max_test = cmdline_slow ? 100000 : 1000;
  n_test = DSK_MIN (n_entries, max_test);
  test_i = step;
  for (i = 0; i < n_test; i++)
    {
      unsigned key_len, value_len;
      const uint8_t *key_data, *value_data;
      /* do the seek */
      if (!seeker->find (seeker,
                         str_test_func,
                         (void*) entries[test_i].key,
                         DSK_TABLE_FILE_FIND_ANY,
                         &key_len, &key_data,
                         &value_len, &value_data,
                         &error))
        {
          if (error)
            dsk_die ("error doing find that should have succeeded: %s",
                     error->message);
          else
            dsk_die ("not found doing find that should have succeeded");
        }
#if 0
      dsk_warning ("test=%s, got result %.*s", entries[test_i].key,
                   (int) key_len, key_data);
#endif
      dsk_assert (key_len == strlen (entries[test_i].key));
      dsk_assert (value_len == strlen (entries[test_i].value));
      dsk_assert (memcmp (key_data, entries[test_i].key, key_len) == 0);
      dsk_assert (memcmp (value_data, entries[test_i].value, value_len) == 0);

      /* advance test_i */
      test_i += step;
      if (test_i >= n_entries)
        test_i -= n_entries;
    }

  /* do negative tests */
  for (i = 0; i < n_negative; i++)
    {
      unsigned key_len, value_len;
      const uint8_t *key_data, *value_data;
      /* do the seek */
      if (!seeker->find (seeker,
                         str_test_func,
                         (void*) neg_entries[i].key,
                         DSK_TABLE_FILE_FIND_ANY,
                         &key_len, &key_data,
                         &value_len, &value_data,
                         &error))
        {
          if (error)
            dsk_die ("error doing find that should have returned nothing: %s",
                     error->message);
        }
      else if (key_len == strlen (neg_entries[i].key)
               && memcmp (key_data, neg_entries[i].key, key_len) == 0)
        {
          dsk_die ("found result when none expected");
        }
    }


  seeker->destroy (seeker);
  }
}

static TestEntry write_seek__odd_letters_to_cap[] = {
  { "a", "A" }, { "c", "C" }, { "e", "E" }, { "g", "G" },
  { "i", "I" }, { "k", "K" }, { "m", "M" }, { "o", "O" },
  { "q", "Q" }, { "s", "S" }, { "u", "U" }, { "w", "W" },
  { "y", "Y" },
};
static TestEntry write_seek__even_letters_to_cap[] = {
  { "b", "B" }, { "d", "D" }, { "f", "F" }, { "h", "H" },
  { "j", "J" }, { "l", "L" }, { "n", "N" }, { "p", "P" },
  { "r", "R" }, { "t", "T" }, { "v", "V" }, { "x", "X" },
  { "z", "Z" },
};
static TestEntry write_seek__dict_head_to_dict_tail[] = {
{"","weal's"}, {"A","weals"}, {"A's","wealth"}, {"AOL","wealth's"},
{"AOL's","wealthier"}, {"Aachen","wealthiest"}, {"Aachen's","wealthiness"},
{"Aaliyah","wealthiness's"}, {"Aaliyah's","wealthy"}, {"Aaron","wean"},
{"Aaron's","weaned"}, {"Abbas","weaning"}, {"Abbasid","weans"},
{"Abbasid's","weapon"}, {"Abbott","weapon's"}, {"Abbott's","weaponless"},
{"Abby","weaponry"}, {"Abby's","weaponry's"}, {"Abdul","weapons"},
{"Abdul's","wear"}, {"Abe","wearable"}, {"Abe's","wearer"},
{"Abel","wearer's"}, {"Abel's","wearers"}, {"Abelard","wearied"},
{"Abelard's","wearier"}, {"Abelson","wearies"}, {"Abelson's","weariest"},
{"Aberdeen","wearily"}, {"Aberdeen's","weariness"}, {"Abernathy","weariness's"},
{"Abernathy's","wearing"}, {"Abidjan","wearisome"}, {"Abidjan's","wears"},
{"Abigail","weary"}, {"Abigail's","wearying"}, {"Abilene","weasel"},
{"Abilene's","weasel's"}, {"Abner","weaseled"}, {"Abner's","weaseling"},
{"Abraham","weasels"}, {"Abraham's","weather"}, {"Abram","weather's"},
{"Abram's","weathercock"}, {"Abrams","weathercock's"},
{"Absalom","weathercocked"}, {"Absalom's","weathercocking"},
{"Abuja","weathercocks"}, {"Abyssinia","weathered"},
{"Abyssinia's","weathering"}, {"Abyssinian","weathering's"},
{"Ac","weatherize"}, {"Ac's","weatherized"}, {"Acadia","weatherizes"},
{"Acadia's","weatherizing"}, {"Acapulco","weatherman"},
{"Acapulco's","weatherman's"}, {"Accra","weathermen"},
{"Accra's","weatherproof"}, {"Acevedo","weatherproofed"},
{"Acevedo's","weatherproofing"}, {"Achaean","weatherproofs"},
{"Achaean's","weathers"}, {"Achebe","weave"}, {"Achebe's","weaved"},
{"Achernar","weaver"}, {"Achernar's","weaver's"}, {"Acheson","weavers"},
{"Acheson's","weaves"}, {"Achilles","weaving"}, {"Aconcagua","web"},
{"Aconcagua's","web's"}, {"Acosta","webbed"}, {"Acosta's","webbing"},
{"Acropolis","webbing's"}, {"Acropolis's","webs"}, {"Acrux","website"},
{"Acrux's","websites"}, {"Actaeon","wed"}, {"Actaeon's","wedded"},
{"Acton","wedder"}, {"Acton's","wedding"}, {"Acts","wedding's"},
{"Acuff","weddings"}, {"Acuff's","wedge"}, {"Ada","wedge's"},
{"Ada's","wedged"}, {"Adam","wedges"}, {"Adam's","wedging"},
{"Adams","wedlock"}, {"Adan","wedlock's"}, {"Adan's","weds"},
{"Adana","wee"}, {"Adana's","weed"}, {"Adar","weed's"},
{"Adar's","weeded"}, {"Addams","weeder"}, {"Adderley","weeder's"},
{"Adderley's","weeders"}, {"Addie","weedier"}, {"Addie's","weediest"},
{"Addison","weeding"}, {"Addison's","weeds"}, {"Adela","weedy"},
{"Adela's","weeing"}, {"Adelaide","week"}, {"Adelaide's","week's"},
{"Adele","weekday"}, {"Adele's","weekday's"}, {"Adeline","weekdays"},
{"Adeline's","weekend"}, {"Aden","weekend's"}, {"Aden's","weekended"},
{"Adenauer","weekending"}, {"Adenauer's","weekends"}, {"Adhara","weeklies"},
{"Adhara's","weekly"}, {"Adidas","weeknight"}, {"Adidas's","weeknight's"},
{"Adirondack","weeknights"}, {"Adirondack's","weeks"}, {"Adirondacks","weep"},
{"Adkins","weeper"}, {"Adkins's","weeper's"}, {"Adler","weepers"},
{"Adler's","weepier"}, {"Adolf","weepies"}, {"Adolf's","weepiest"},
{"Adolfo","weeping"}, {"Adolfo's","weepings"}, {"Adolph","weeps"},
{"Adolph's","weepy"}, {"Adonis","weer"}, {"Adonis's","wees"},
{"Adonises","weest"}, {"Adrian","weevil"}, {"Adrian's","weevil's"},
{"Adriana","weevils"}, {"Adriana's","weft"}, {"Adriatic","weft's"},
{"Adrienne","wefted"}, {"Adrienne's","wefting"}, {"Advent","wefts"},
{"Advent's","weigh"}, {"Adventist","weighed"}, {"Adventist's","weighing"},
{"Advents","weighs"}, {"Advil","weight"}, {"Advil's","weight's"},
{"Aegean","weighted"}, {"Aelfric","weightier"}, {"Aelfric's","weightiest"},
{"Aeneas","weightiness"}, {"Aeneid","weightiness's"}, {"Aeneid's","weighting"},
{"Aeolus","weighting's"}, {"Aeolus's","weightless"}, {"Aeroflot","weightlessness"},
{"Aeroflot's","weightlessness's"}, {"Aeschylus","weightlifter"},
{"Aeschylus's","weightlifters"}, {"Aesculapius","weightlifting"},
{"Aesculapius's","weightlifting's"}, {"Aesop","weights"}, {"Aesop's","weighty"},
{"Afghan","weir"}, {"Afghan's","weir's"}, {"Afghanistan","weird"},
{"Afghanistan's","weirded"}, {"Afghans","weirder"}, {"Africa","weirdest"},
{"Africa's","weirding"}, {"African","weirdly"}, {"Africans","weirdness"},
{"Afrikaans","weirdness's"}, {"Afrikaans's","weirdo"}, {"Afrikaner","weirdo's"},
{"Afrikaner's","weirdos"}, {"Afrikaners","weirds"}, {"Afro","weired"},
{"Afro's","weiring"}, {"Afrocentrism","weirs"}, {"Afros","welcome"},
{"Ag","welcomed"}, {"Ag's","welcomes"}, {"Agamemnon","welcoming"},
{"Agamemnon's","weld"}, {"Agassi","welded"}, {"Agassi's","welder"},
{"Agassiz","welder's"}, {"Agassiz's","welders"}, {"Agatha","welding"},
{"Agatha's","welds"}, {"Aggie","welfare"}, {"Aggie's","welfare's"},
{"Aglaia","welkin"}, {"Aglaia's","welkin's"}, {"Agnes","well"},
{"Agnes's","welled"}, {"Agnew","welling"}};

static TestEntry write_seek__nonwords[] = {
 {"AAX",""}, {"AaX",""}, {"AbX",""}, {"AcX",""}, {"AdX",""}, {"AeX",""},
 {"AfX",""}, {"AgX",""}, {"AzX",""},
};

static struct {
  const char *name;
  unsigned n_entries;
  TestEntry *entries;
  unsigned n_neg_entries;
  TestEntry *neg_entries;
} test_seek_datasets[] =
{
#define WRITE_ENTRY(name, pos, neg) \
  { name, DSK_N_ELEMENTS(pos), pos, DSK_N_ELEMENTS(neg), neg }
  WRITE_ENTRY ("odd/even", write_seek__odd_letters_to_cap, write_seek__even_letters_to_cap),
  WRITE_ENTRY ("even/odd", write_seek__even_letters_to_cap, write_seek__odd_letters_to_cap),
  WRITE_ENTRY ("dict list 1", write_seek__dict_head_to_dict_tail, write_seek__nonwords),
#undef WRITE_ENTRY
};


static void
test_simple_write_seek (void)
{
  unsigned i;
  for (i = 0; i < DSK_N_ELEMENTS (test_seek_datasets); i++)
    test_various_write_seek_1 (test_seek_datasets[i].name,
                               test_seek_datasets[i].n_entries,
                               test_seek_datasets[i].entries,
                               test_seek_datasets[i].n_neg_entries,
                               test_seek_datasets[i].neg_entries);
}


static struct 
{
  const char *name;
  void (*test)(void);
} tests[] =
{
  { "simple writing/reading", test_simple_write_read },
  { "simple write/seek testing", test_simple_write_seek },
  { "extensive write/read testing", test_various_read_write },
  //{ "extensive write/seek testing", test_various_write_seek },
};

int main(int argc, char **argv)
{
  unsigned i;
  char test_dir_buf[256];
  DskError *error = NULL;

  dsk_cmdline_init ("test table internals (the 'file' abstraction)",
                    "Test Table Internals",
                    NULL, 0);
  dsk_cmdline_add_boolean ("verbose", "extra logging", NULL, 0,
                           &cmdline_verbose);
  dsk_cmdline_add_boolean ("slow", "run tests that are fairly slow", NULL, 0,
                           &cmdline_slow);
  dsk_cmdline_add_boolean ("keep-testdir", "do not delete working directory", NULL, 0,
                           &cmdline_keep_testdir);
  dsk_cmdline_process_args (&argc, &argv);

  snprintf (test_dir_buf, sizeof (test_dir_buf),
            "test-table-file-%u-%u", (unsigned)time(NULL), (unsigned)getpid());
  location = dsk_dir_new (NULL, test_dir_buf, DSK_DIR_NEW_MAYBE_CREATE, &error);
  if (location == NULL)
    dsk_die ("error making directory: %s", error->message);

  for (i = 0; i < DSK_N_ELEMENTS (tests); i++)
    {
      fprintf (stderr, "Test: %s... ", tests[i].name);
      tests[i].test ();
      fprintf (stderr, " done.\n");
    }

  if (cmdline_keep_testdir)
    {
      fprintf (stderr,
               "test-table-file: keep-testdir: preserving test directory %s\n",
               dsk_dir_get_str (location));
    }
  else
    {
      DskError *error = NULL;
      if (!dsk_remove_dir_recursive (dsk_dir_get_str (location), &error))
        dsk_die ("error removing directory %s: %s",
                 dsk_dir_get_str (location), error->message);
    }
  dsk_cleanup ();
  return 0;
}
