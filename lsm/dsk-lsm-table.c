struct DskLsmTableWriter {
  int fd;
  uint8_t *mmapped;
  ... pointers into mmapped
};
struct DskLsmTableMergeJob {
  DskLsmTableFile *prev, *next;
  DskLsmTableWriter writer;
};

struct DskLsmTableFile {
  // read-only
  int fd;
  uint8_t *mmapped;

  // This file may be merging with previous or next
  // file, but never both at the same time.
  DskLsmTableMergeJob *job;

  DskLsmTableFile *prev, *next;
};

struct DskLsmInMemoryEntry {
  unsigned key_len;
  DskLsmBuffer kv;

  unsigned is_red : 1;
  unsigned is_free : 1;
  DskLsmInMemoryEntry *left, *right, *parent;
};

/* NOTE: this requires 'tree' to be a parameter/variable. */
#define RBTREE_COMPARE_INMEMORY(a,b,rv) \
  rv = tree->compare((a),(b),tree->compare_data)

#define GET_IN_MEMORY_TREE() \
  tree->(in_memory_table).tree,
  DskLsmInMemoryEntry *,
  GET_ENTRY_IS_RED, SET_ENTRY_IS_RED,
  parent, left, right,
  RBTREE_COMPARE_INMEMORY


struct DskLsmInMemoryTree {
  DskLsmInMemoryTree *tree;
  DskLsmInMemoryEntry *free_list;
};

#define TABLE_GET_FILE_LIST(table) \
  DskLsmTableFile *,               \
  (table)->first, (table)->last,   \
  prev, next

struct DskLsmTable {
  DskObject base_instance;

  DskLsmInMemoryTree in_memory;

  DskLsmTableFile *first, *last;
};

