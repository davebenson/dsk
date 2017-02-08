/* invariant:  if a buffer.size==0, then first_frag/last_frag == NULL.
   corollary:  if a buffer.size==0, then the buffer is using no memory. */

typedef struct _DskBuffer DskBuffer;
typedef struct _DskBufferFragment DskBufferFragment;

struct _DskBufferFragment
{
  DskBufferFragment    *next;
  uint8_t              *buf;
  unsigned              buf_max_size;	/* allocation size of buf */
  unsigned              buf_start;	/* offset in buf of valid data */
  unsigned              buf_length;	/* length of valid data in buf; != 0 */
  
  dsk_boolean           is_foreign;
  DskDestroyNotify      destroy;
  void                 *destroy_data;
};

struct _DskBuffer
{
  unsigned              size;

  DskBufferFragment    *first_frag;
  DskBufferFragment    *last_frag;
};

#define DSK_BUFFER_INIT		{ 0, NULL, NULL }


void     dsk_buffer_init                (DskBuffer       *buffer);

unsigned dsk_buffer_read                (DskBuffer    *buffer,
                                         unsigned      max_length,
                                         void         *data);
unsigned dsk_buffer_peek                (const DskBuffer* buffer,
                                         unsigned      max_length,
                                         void         *data);
int      dsk_buffer_discard             (DskBuffer    *buffer,
                                         unsigned      max_discard);
char    *dsk_buffer_read_line           (DskBuffer    *buffer);

char    *dsk_buffer_parse_string0       (DskBuffer    *buffer);
                        /* Returns first char of buffer, or -1. */
int      dsk_buffer_peek_byte           (const DskBuffer *buffer);
int      dsk_buffer_read_byte           (DskBuffer    *buffer);

uint8_t  dsk_buffer_byte_at             (DskBuffer    *buffer,
                                         unsigned      index);
uint8_t  dsk_buffer_last_byte           (DskBuffer    *buffer);
/* 
 * Appending to the buffer.
 */
void     dsk_buffer_append              (DskBuffer    *buffer, 
                                         unsigned      length,
                                         const void   *data);

DSK_INLINE_FUNC void dsk_buffer_append_small(DskBuffer    *buffer, 
                                         unsigned      length,
                                         const void   *data);
void     dsk_buffer_append_string       (DskBuffer    *buffer, 
                                         const char   *string);
DSK_INLINE_FUNC void dsk_buffer_append_byte(DskBuffer    *buffer, 
                                         uint8_t       byte);
void      dsk_buffer_append_byte_f      (DskBuffer    *buffer, 
                                         uint8_t       byte);
void     dsk_buffer_append_repeated_byte(DskBuffer    *buffer, 
                                         size_t        count,
                                         uint8_t       byte);
#define dsk_buffer_append_zeros(buffer, count) \
  dsk_buffer_append_repeated_byte ((buffer), 0, (count))


void     dsk_buffer_append_string0      (DskBuffer    *buffer,
                                         const char   *string);

void     dsk_buffer_append_foreign      (DskBuffer    *buffer,
					 unsigned      length,
                                         const void   *data,
					 DskDestroyNotify destroy,
					 void         *destroy_data);

void     dsk_buffer_printf              (DskBuffer    *buffer,
					 const char   *format,
					 ...) DSK_GNUC_PRINTF(2,3);
void     dsk_buffer_vprintf             (DskBuffer    *buffer,
					 const char   *format,
					 va_list       args);

uint8_t  dsk_buffer_get_last_byte       (DskBuffer    *buffer);
uint8_t  dsk_buffer_get_byte_at         (DskBuffer    *buffer,
                                         size_t        idx);


/* --- appending data that will be filled in later --- */
typedef struct {
  DskBuffer *buffer;
  DskBufferFragment *fragment;
  unsigned offset;
  unsigned length;
} DskBufferPlaceholder;

void     dsk_buffer_append_placeholder  (DskBuffer    *buffer,
                                         unsigned      length,
                                         DskBufferPlaceholder *out);
void     dsk_buffer_placeholder_set     (DskBufferPlaceholder *placeholder,
                                         const void       *data);

/* --- buffer-to-buffer transfers --- */
/* Take all the contents from src and append
 * them to dst, leaving src empty.
 */
unsigned dsk_buffer_drain               (DskBuffer    *dst,
                                         DskBuffer    *src);

/* Like `drain', but only transfers some of the data. */
unsigned dsk_buffer_transfer            (DskBuffer    *dst,
                                         DskBuffer    *src,
					 unsigned      max_transfer);

/* --- file-descriptor mucking --- */
int      dsk_buffer_writev              (DskBuffer       *read_from,
                                         int              fd);
int      dsk_buffer_writev_len          (DskBuffer *read_from,
		                         int              fd,
		                         unsigned         max_bytes);
/* returns TRUE iff all the data was written.  'read_from' is blank. */
dsk_boolean dsk_buffer_write_all_to_fd  (DskBuffer       *read_from,
                                         int              fd,
                                         DskError       **error);
int      dsk_buffer_readv               (DskBuffer       *write_to,
                                         int              fd);

/* --- deallocating memory used by buffer --- */

/* This deallocates memory used by the buffer-- you are responsible
 * for the allocation and deallocation of the DskBuffer itself. */
void     dsk_buffer_clear               (DskBuffer    *to_destroy);

/* Same as calling clear/init */
void     dsk_buffer_reset               (DskBuffer    *to_reset);

/* Return a string and clear the buffer. */
char *dsk_buffer_empty_to_string (DskBuffer *buffer);

/* --- iterating through the buffer --- */
/* 'frag_offset_out' is the offset of the returned fragment in the whole
   buffer. */
DskBufferFragment *dsk_buffer_find_fragment (DskBuffer   *buffer,
                                             unsigned     offset,
                                             unsigned    *frag_offset_out);

/* Free all unused buffer fragments. */
void     _dsk_buffer_cleanup_recycling_bin ();

typedef enum {
  DSK_BUFFER_DUMP_DRAIN = (1<<0),
  DSK_BUFFER_DUMP_NO_DRAIN = (1<<1),
  DSK_BUFFER_DUMP_FATAL_ERRORS = (1<<2),
  DSK_BUFFER_DUMP_LEAVE_PARTIAL = (1<<3),
  DSK_BUFFER_DUMP_NO_CREATE_DIRS = (1<<4),
  DSK_BUFFER_DUMP_EXECUTABLE = (1<<5),
} DskBufferDumpFlags;

dsk_boolean dsk_buffer_dump (DskBuffer          *buffer,
                             const char         *filename,
                             DskBufferDumpFlags  flags,
                             DskError          **error);
                          

/* misc */
int dsk_buffer_index_of(DskBuffer *buffer, char char_to_find);

unsigned dsk_buffer_fragment_peek (DskBufferFragment *fragment,
                                   unsigned           offset,
                                   unsigned           length,
                                   void              *buf);
dsk_boolean dsk_buffer_fragment_advance (DskBufferFragment **frag_inout,
                                         unsigned           *offset_inout,
                                         unsigned            skip);

/* HACKS */
/* NOTE: the buffer is INVALID after this call, since no empty
   fragments are allowed.  You MUST deal with this if you do 
   not actually add data to the buffer */
void dsk_buffer_append_empty_fragment (DskBuffer *buffer);

void dsk_buffer_maybe_remove_empty_fragment (DskBuffer *buffer);

/* a way to delete the fragment from dsk_buffer_append_empty_fragment() */
void dsk_buffer_fragment_free (DskBufferFragment *fragment);


#if DSK_CAN_INLINE || defined(DSK_IMPLEMENT_INLINES)
DSK_INLINE_FUNC void dsk_buffer_append_small(DskBuffer    *buffer, 
                                         unsigned      length,
                                         const void   *data)
{
  DskBufferFragment *f = buffer->last_frag;
  if (f != NULL
   && !f->is_foreign
   && f->buf_start + f->buf_length + length <= f->buf_max_size)
    {
      uint8_t *dst = f->buf + (f->buf_start + f->buf_length);
      const uint8_t *src = data;
      f->buf_length += length;
      buffer->size += length;
      while (length--)
        *dst++ = *src++;
    }
  else
    dsk_buffer_append (buffer, length, data);
}
DSK_INLINE_FUNC void dsk_buffer_append_byte(DskBuffer    *buffer, 
                                            uint8_t       byte)
{
  DskBufferFragment *f = buffer->last_frag;
  if (f != NULL
   && !f->is_foreign
   && f->buf_start + f->buf_length < f->buf_max_size)
    {
      f->buf[f->buf_start + f->buf_length] = byte;
      ++(f->buf_length);
      buffer->size += 1;
    }
  else
    dsk_buffer_append (buffer, 1, &byte);
}

#endif
