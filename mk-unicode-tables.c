// ftp://ftp.unicode.org/Public/3.0-Update/UnicodeData-3.0.0.html
#include <stdio.h>
#include "dsk-priority-queue-macros.h"

struct _SimpleHuffmanTreeNode
{
  SimpleHuffmanTreeNodeType type;
  union {
    SimpleHuffmanTreeNodeType *children[2];
    unsigned leaf;
  } info;

  unsigned freq;

  // used by tree serializer
  unsigned node_index;
};

#define PQ_COMPARE(a,b, rv) \
  rv = a < b ? -1 : a > b ? 1 : 0
#define GET_PQ() \
  SimpleHuffmanTreeNode *, n_huffman_nodes, huffman_node_tree, PQ_COMPARE

#define ALLOC_NODE()   DSK_NEW (SimpleHuffmanTreeNode)

unsigned n_huffman_nodes = 0;
unsigned huffman_nodes_alloced = 0;
SimpleHuffmanTreeNode **huffman_node_tree;

static void
make_tree (size_t n_values, unsigned *value_freqs);
{
  size_t tree_size = n_values * 2 - 1;
  SimpleHuffmanTreeNode *tree = DSK_NEW_ARRAY (SimpleHuffmanTreeNode, tree_size);
  SimpleHuffmanTreeNode *previous_alloc = tree + tree_size;
  SimpleHuffmanTreeNode **pq = DSK_NEW_ARRAY (SimpleHuffmanTreeNode *, n_values);
  size_t pq_size = 0;
  for (i = 0; i < n_values; i++)
    {
      SimpleHuffmanTreeNode *leaf = --previous_alloc;
      leaf->type = SIMPLE_HUFFMAN_TREE_LEAF;
      leaf->info.leaf = i;
      leaf->freq = value_freqs[i];
      add_node (pq_size, pq, leaf);
      ++pq_size;
    }
  while (pq_size > 1)
    {
      SimpleHuffmanTreeNode *a = pq[0];
      DSK_PRIORITY_QUEUE_REMOVE_MIN (GET_PQ());
      --pq_size;

      SimpleHuffmanTreeNode *b = pq[0];
      DSK_PRIORITY_QUEUE_REMOVE_MIN (GET_PQ());
      --pq_size;

      SimpleHuffmanTreeNode *out = --previous_alloc;
      out->type = SIMPLE_HUFFMAN_TREE_NODE;
      out->info.node.children[0] = a;
      out->info.node.children[1] = b;
      out->freq = a->freq + b->freq;
    }
  assert (previous_alloc == tree);
  return tree;
}

struct _HuffmanTree
{
  size_t tree_size;
  int *tree;

  char *string_heap;
};

struct LinePiece
{
  size_t len;
  const char *str;
};

enum PieceIndex
{
  PIECE_CODE = 0,
  PIECE_NAME = 1,
  PIECE_CATEGORY = 2,
  PIECE_COMBINING = 3,
  PIECE_BIDI = 4,
  PIECE_DECOMPOSED = 5,
  PIECE_DIGIT_VALUE = 6,
  PIECE_NUMERIC_VALUE = 7,
  PIECE_MIRRORED = 8,
  PIECE_UNKNOWN = 9,  ///           N
  PIECE_OLD_NAME = 10,
  PIECE_COMMENT = 11,
  PIECE_UPPERCASE = 12,
  PIECE_LOWERCASE = 13,
  PIECE_TITLECASE = 14,
};

int main(int argc, char **argv)
{
  char linebuf[1024];
  struct LinePiece pieces[15];

  while (fgets (linebuf, sizeof (linebuf), stdin) != NULL)
    {
      char *newline = strchr (linebuf, '\n');
      *newline = ':';
      char *at = linebuf;
      while (n_pieces < 15)
        {
          pieces[n_pieces].str = at;
          char *colon = strchr (at, ':');
          pieces[n_pieces].len = colon - at;
          n_pieces++;

          at = colon + 1;
        }
    }
}


