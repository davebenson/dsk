generated-data: dsk-hpack-huffman-decode-generated.inc dsk-hpack-huffman-encode-generated.inc

dsk-hpack-huffman-decode-generated.inc: dsk-hpack-make-decode-tree
	./dsk-hpack-make-decode-tree > dsk-hpack-huffman-decode-generated.inc
dsk-hpack-huffman-encode-generated.inc: dsk-hpack-make-encode-table
	./dsk-hpack-make-encode-table > dsk-hpack-huffman-encode-generated.inc

dsk-hpack-make-decode-tree: dsk-hpack-make-tree.c
	$(CC) -o dsk-hpack-make-decode-tree -DGENERATE_DECODE_TABLES dsk-hpack-make-tree.c

dsk-hpack-make-encode-table: dsk-hpack-make-tree.c
	$(CC) -o dsk-hpack-make-encode-table -DGENERATE_ENCODE_TABLES dsk-hpack-make-tree.c
