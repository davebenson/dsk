
typedef struct _DskTableFileWriter DskTableFileWriter;
typedef struct _DskTableFileSeeker DskTableFileSeeker;
typedef struct _DskTableFileCompressor DskTableFileCompressor;

typedef enum
{
  DSK_TABLE_FILE_WRITER_FINISHED,
  DSK_TABLE_FILE_WRITER_FINISHING,
  DSK_TABLE_FILE_WRITER_FINISH_FAILED
} DskTableFileWriterFinish;

struct _DskTableFileWriter
{
  dsk_boolean (*write)  (DskTableFileWriter *writer,
                         unsigned            key_length,
                         const uint8_t      *key_data,
                         unsigned            value_length,
                         const uint8_t      *value_data,
                         DskError          **error);
  DskTableFileWriterFinish (*finish) 
                        (DskTableFileWriter *writer,
                         DskError          **error);
  dsk_boolean (*close)  (DskTableFileWriter *writer,
                         DskError          **error);
  void        (*destroy)(DskTableFileWriter *writer);
};

/* Returns:
     -1 if the key is before the range we are searching for.
      0 if the key is in the range we are searching for.
     +1 if the key is after the range we are searching for.
 */
typedef int  (*DskTableSeekerFindFunc) (unsigned           key_len,
                                        const uint8_t     *key_data,
                                        void              *user_data);
typedef enum
{
  DSK_TABLE_FILE_FIND_FIRST,
  DSK_TABLE_FILE_FIND_ANY,
  DSK_TABLE_FILE_FIND_LAST
} DskTableFileFindMode;

struct _DskTableFileSeeker
{
  dsk_boolean (*find)      (DskTableFileSeeker    *seeker,
                            DskTableSeekerFindFunc func,
                            void                  *func_data,
                            DskTableFileFindMode   mode,
                            unsigned              *key_len_out,
                            const uint8_t        **key_data_out,
                            unsigned              *value_len_out,
                            const uint8_t        **value_data_out,
                            DskError             **error);
 
 
  DskTableReader *
             (*find_reader)(DskTableFileSeeker    *seeker,
                            DskTableSeekerFindFunc func,
                            void                  *func_data,
                            DskError             **error);
 
  dsk_boolean (*index)     (DskTableFileSeeker    *seeker,
                            uint64_t               index,
                            unsigned              *key_len_out,
                            const void           **key_data_out,
                            unsigned              *value_len_out,
                            const void           **value_data_out,
                            DskError             **error);
 
  DskTableReader *
            (*index_reader)(DskTableFileSeeker    *seeker,
                            uint64_t               index,
                            DskError             **error);
 

  void         (*destroy)  (DskTableFileSeeker    *seeker);

};


struct _DskTableFileInterface
{
  DskTableFileWriter *(*new_writer) (DskTableFileInterface   *iface,
                                     DskDir                  *dir,
                                     const char              *base_filename,
                                     DskError               **error);
  DskTableReader     *(*new_reader) (DskTableFileInterface   *iface,
                                     DskDir                  *dir,
                                     const char              *base_filename,
                                     DskError               **error);
  DskTableFileSeeker *(*new_seeker) (DskTableFileInterface   *iface,
                                     DskDir                  *dir,
                                     const char              *base_filename,
                                     DskError               **error);
  dsk_boolean         (*delete_file)(DskTableFileInterface   *iface,
                                     DskDir                  *dir,
                                     const char              *base_filename,
                                     DskError               **error);
  void                (*destroy)    (DskTableFileInterface   *iface);
};

  
extern DskTableFileInterface dsk_table_file_interface_default;

typedef struct _DskTableFileNewOptions DskTableFileNewOptions;
struct _DskTableFileNewOptions
{
  DskTableFileCompressor *compressor;

  /* As we get entries into the file, we group them into units of size
     n_compress and write the key, the offset of the compressed blob,
     and various sizes into the biggest index.  for each smaller index,
     we only write one entry in the small index for every index_ratio
     entries in the next larger index.  */

  /* number of entries to compress into one unit */
  unsigned n_compress;

  /* size ratio between indexes */
  unsigned index_ratio;

  /* enable a trivial kind of pre-compression:
     if two consecutive records have a common prefix,
     only store the length of that prefix.  a big win for
     long URLs and the like. */
  dsk_boolean prefix_compress;
};
#define DSK_TABLE_FILE_NEW_OPTIONS_INIT              \
{                                                       \
  &dsk_table_file_compressor_gzip[3],                   \
  64,                           /* n_compress */        \
  256,                          /* index_ratio */       \
  DSK_TRUE                      /* prefix_compress */   \
}

DskTableFileInterface *dsk_table_file_interface_new (DskTableFileNewOptions *options);

extern DskTableFileInterface dsk_table_file_interface_trivial;

struct _DskTableFileCompressor
{
  /* Fails iff the output buffer is too short. */
  dsk_boolean (*compress)   (DskTableFileCompressor *compressor,
                             unsigned                in_len,
                             const uint8_t          *in_data,
                             unsigned               *out_len_inout,
                             uint8_t                *out_data);

  /* Fails on corrupt data, or if the output buffer is too small. */
  dsk_boolean (*decompress) (DskTableFileCompressor *compressor,
                             unsigned                in_len,
                             const uint8_t          *in_data,
                             unsigned               *out_len_inout,
                             uint8_t                *out_data);
};

extern DskTableFileCompressor dsk_table_file_compressor_lzo;
extern DskTableFileCompressor dsk_table_file_compressor_gzip[10];
/*extern DskTableFileCompressor dsk_table_file_compressor_bzip2[10];*/
