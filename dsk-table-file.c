#include "dsk.h"


/* --- Generic TableFileInterface code --- */


/* --- Standard TableFileInterface implementation --- */

/* the TableFileInterface interface constructor itself */
typedef struct _TableFileStdInterface TableFileStdInterface;
struct _TableFileStdInterface
{
  DskTableFileInterface base_iface;
  DskTableFileCompressor *compressor;
};

  DskTableFileWriter *(*new_writer) (DskTableFileInterface   *iface,
                                     const char              *openat_dir,
                                     int                      openat_fd,
                                     const char              *base_filename,
                                     DskError               **error);
  DskTableReader     *(*new_reader) (DskTableFileInterface   *iface,
                                     const char              *openat_dir,
                                     int                      openat_fd,
                                     const char              *base_filename,
                                     DskError               **error);
  DskTableFileSeeker *(*new_seeker) (DskTableFileInterface   *iface,
                                     const char              *openat_dir,
                                     int                      openat_fd,
                                     const char              *base_filename,
                                     DskError               **error);
  dsk_boolean         (*delete_file)(DskTableFileInterface   *iface,
                                     const char              *openat_dir,
                                     int                      openat_fd,
                                     const char              *base_filename,
                                     DskError               **error);
  void                (*destroy)    (DskTableFileInterface   *iface);


DskTableFileInterface *
dsk_table_file_interface_new (DskTableFileCompressor *compressor,
                              unsigned                
{
  TableFileStdInterface *iface = dsk_malloc (sizeof (TableFileStdInterface));
  iface->base_iface.new_reader = std__new_reader;
  iface->base_iface.new_writer = std__new_writer;
  iface->base_iface.new_seeker = std__new_seeker;
  iface->base_iface.delete_file = std__delete_file;
  iface->base_iface.destroy = std__destroy;
  iface->compressor = compressor;
}
