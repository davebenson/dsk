//
// This API is intended to be pretty similar
// to DskBuffer, with the note that direct buffer
// access is easy and recommended.
//
/* invariant:  allocation <= start
 *                        <= start+size
 *                        <= allocation+allocation_size.
 */
typedef struct DskFlatBuffer DskFlatBuffer;
struct DskFlatBuffer
{
  unsigned   allocation_size;
  uint8_t   *allocation;
  bool       must_free_allocation;

  uint8_t   *start;
  size_t     size;
};

#define DSK_FLAT_BUFFER_INIT		{ 0, NULL, NULL, NULL }


void     dsk_flat_buffer_init           (DskFlatBuffer       *buffer);

//
// Generally, it's better to just access 'start' and 'size'
// rather than using read/peek 
// (which have to copy the data into 'data').
//
// The following method signatures
// are just copied from DskBuffer.
// They are here mostly to ease porting.
//
unsigned dsk_flat_buffer_read          (DskFlatBuffer    *buffer,
                                        unsigned          max_length,
                                        void             *data);
unsigned dsk_flat_buffer_peek          (const DskFlatBuffer* buffer,
                                        unsigned          max_length,
                                        void             *data);
int      dsk_flat_buffer_discard       (DskFlatBuffer    *buffer,
                                        unsigned          max_discard);
char    *dsk_flat_buffer_read_line      (DskFlatBuffer    *buffer);

char    *dsk_flat_buffer_parse_string0  (DskFlatBuffer    *buffer);
                        /* Returns first char of buffer, or -1. */
int      dsk_flat_buffer_peek_byte      (const DskFlatBuffer *buffer);
int      dsk_flat_buffer_read_byte      (DskFlatBuffer    *buffer);

uint8_t  dsk_flat_buffer_byte_at        (DskFlatBuffer    *buffer,
                                         unsigned      index);
uint8_t  dsk_flat_buffer_last_byte      (DskFlatBuffer    *buffer);

/* 
 * Appending to the buffer.
 */
void     dsk_flat_buffer_append         (DskFlatBuffer    *buffer, 
                                         unsigned          length,
                                         const void       *data);
void     dsk_flat_buffer_append_string  (DskFlatBuffer    *buffer, 
                                         const char       *string);
void     dsk_flat_buffer_append_byte(DskFlatBuffer    *buffer, 
                                         uint8_t           byte);
void     dsk_flat_buffer_append_byte_f (DskFlatBuffer    *buffer, 
                                         uint8_t           byte);
void     dsk_flat_buffer_append_repeated_byte(DskFlatBuffer    *buffer, 
                                         size_t            count,
                                         uint8_t           byte);
#define dsk_flat_buffer_append_zeros(buffer, count) \
  dsk_flat_buffer_append_repeated_byte ((buffer), 0, (count))


void     dsk_flat_buffer_append_string0 (DskFlatBuffer    *buffer,
                                         const char   *string);

void     dsk_flat_buffer_append_printf  (DskFlatBuffer    *buffer,
					 const char   *format,
					 ...) DSK_GNUC_PRINTF(2,3);
void     dsk_flat_buffer_append_vprintf (DskFlatBuffer    *buffer,
					 const char   *format,
					 va_list       args);
void     dsk_flat_buffer_set_printf     (DskFlatBuffer    *buffer,
					 const char   *format,
					 ...) DSK_GNUC_PRINTF(2,3);
void     dsk_flat_buffer_set_vprintf    (DskFlatBuffer    *buffer,
					 const char   *format,
					 va_list       args);

// returns 'start'; data is NOT preserved.
uint8_t *dsk_flat_buffer_set_size       (DskFlatBuffer    *buffer,
					 size_t            size);

DSK_INLINE uint8_t
         dsk_flat_buffer_get_last_byte  (DskFlatBuffer    *buffer);
DSK_INLINE uint8_t
         dsk_flat_buffer_get_byte_at    (DskFlatBuffer    *buffer,
                                         size_t        idx);



/* --- file-descriptor mucking --- */
/* returns TRUE iff all the data was written.  'read_from' is blank. */
bool     dsk_flat_buffer_write_all_to_fd(DskFlatBuffer   *read_from,
                                         int              fd,
                                         DskError       **error);

/* --- deallocating memory used by buffer --- */

/* This deallocates memory used by the buffer-- you are responsible
 * for the allocation and deallocation of the DskFlatBuffer itself. */
void     dsk_flat_buffer_clear               (DskFlatBuffer *to_destroy);

/* Same as calling clear/init */
void     dsk_flat_buffer_reset               (DskFlatBuffer *to_reset);

/* Return a string and clear the buffer. */
char    *dsk_flat_buffer_empty_to_string     (DskFlatBuffer *buffer);

/* --- writing to a file --- */
bool     dsk_flat_buffer_dump                (DskFlatBuffer *buffer,
                                              const char    *filename,
                                              DskBufferDumpFlags  flags,
                                              DskError     **error);
                          

/* misc */
int      dsk_flat_buffer_index_of(DskFlatBuffer *buffer, char char_to_find);


DSK_INLINE bool dsk_flat_buffer_check_invariants (DskFlatBuffer *fb)
{
  return fb->allocation <= fb->start
      && fb->start <= (fb->start + fb->size)
      && (fb->start + fb->size) <= (fb->allocation+fb->allocation_size);
}

//
// Inline Functions
//

