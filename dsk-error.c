#include <stdio.h>
#include <string.h>
#include "dsk.h"

static void dsk_error_finalize (DskError *error)
{
  while (error->data_list)
    {
      DskErrorData *d = error->data_list;
      error->data_list = d->next;
      if (d->type->clear)
        d->type->clear (d->type, d + 1);
      dsk_free (d);
    }
  dsk_free (error->message);
}

DSK_OBJECT_CLASS_DEFINE_CACHE_DATA(DskError);
DskErrorClass dsk_error_class =
{
  DSK_OBJECT_CLASS_DEFINE(DskError, &dsk_object_class,
                          NULL,
                          dsk_error_finalize)
};


DskError *dsk_error_new        (const char *format,
                                ...)
{
  va_list args;
  DskError *rv;
  va_start (args, format);
  rv = dsk_error_new_valist (format, args);
  va_end (args);
  return rv;
}

DskError *dsk_error_new_valist (const char *format,
                                va_list     args)
{
  char buf[512];
  DskError *rv;
  vsnprintf (buf, sizeof (buf), format, args);
  buf[sizeof(buf) - 1] = 0;
  rv = dsk_object_new (&dsk_error_class);
  rv->message = dsk_strdup (buf);
  return rv;
}

DskError *dsk_error_new_literal(const char *message)
{
  DskError *rv = dsk_object_new (&dsk_error_class);
  rv->message = dsk_strdup (message);
  return rv;
}

void dsk_set_error (DskError **error,
                    const char *format,
                    ...)
{
  va_list args;
  if (error == NULL)
    return;

  DskError *new_error;

  va_start (args, format);
  new_error = dsk_error_new_valist (format, args);
  va_end (args);

  if (*error != NULL)
    {
      dsk_warning ("attempting to override error in dsk_set_error.  old error:\n"
                   "   %s\n"
                   "New error:\n"
                   "   %s\n",
                   (*error)->message,
                   new_error->message);
      dsk_error_unref (new_error);
    }
  else
    *error = new_error;
}

DskError *dsk_error_ref        (DskError   *error)
{
  dsk_assert (dsk_object_is_a (error, &dsk_error_class));
  return (DskError *) dsk_object_ref (error);
}

void      dsk_error_unref      (DskError   *error)
{
  dsk_assert (dsk_object_is_a (error, &dsk_error_class));
  dsk_object_unref (error);
}
void      dsk_add_error_prefix (DskError   **error,
                                const char  *format,
                                ...)
{
  char buf[512];
  va_list args;
  char *new_message;
  if (error == NULL)
    return;
  dsk_assert (*error != NULL);
  va_start (args, format);
  vsnprintf (buf, sizeof (buf), format, args);
  va_end (args);
  buf[sizeof(buf) - 1] = 0;
  new_message = dsk_malloc (strlen (buf) + strlen ((*error)->message) + 2 + 1);
  strcpy (new_message, buf);
  strcat (new_message, ": ");
  strcat (new_message, (*error)->message);
  dsk_free ((*error)->message);
  (*error)->message = new_message;
}

void      dsk_add_error_suffix (DskError   **error,
                                const char  *format,
                                ...)
{
  char buf[512];
  va_list args;
  char *new_message;
  if (error == NULL)
    return;
  dsk_assert (*error != NULL);
  va_start (args, format);
  vsnprintf (buf, sizeof (buf), format, args);
  va_end (args);
  buf[sizeof(buf) - 1] = 0;
  new_message = dsk_malloc (strlen (buf) + strlen ((*error)->message) + 1);
  strcpy (new_message, (*error)->message);
  strcat (new_message, buf);
  dsk_free ((*error)->message);
  (*error)->message = new_message;
}

void dsk_error_to_buffer (DskError *error,
		          DskBuffer *out)
{
  dsk_buffer_append_string (out, "ERROR: ");
  dsk_buffer_append_string (out, error->message);
  for (DskErrorData *at = error->data_list; at != NULL; at = at->next)
    at->type->to_string(at->type, at + 1, out);
}

char * dsk_error_to_string (DskError *error)
{
  DskBuffer buf = DSK_BUFFER_INIT;
  dsk_error_to_buffer (error, &buf);
  return dsk_buffer_empty_to_string (&buf);
}

void       *dsk_error_force_data (DskError   *error,
                                  DskErrorDataType *data_type,
                                  bool *created_out)
{
  char *data = dsk_error_find_data (error, data_type);
  if (data)
    {
      if (created_out)
        *created_out = false;
      return data;
    }
  if (created_out)
    *created_out = true;
  DskErrorData *at = dsk_malloc (sizeof (DskErrorData) + data_type->size);
  at->type = data_type;
  at->next = error->data_list;
  error->data_list = at;
  return at + 1;
}

void       *dsk_error_find_data  (DskError   *error,
                                  DskErrorDataType *data_type)
{
  DskErrorData *at;
  for (at = error->data_list; at; at = at->next)
    if (at->type == data_type)
      return at + 1;
  return NULL;
}

bool dsk_error_remove_data       (DskError *error,
                                  DskErrorDataType *data_type)
{
  DskErrorData **p_at;
  for (p_at = &error->data_list; *p_at != NULL; p_at = &((*p_at)->next))
    if ((*p_at)->type == data_type)
      {
	DskErrorData *to_kill = *p_at;
        *p_at = to_kill->next;

	if (data_type->clear)
          (*data_type->clear)(data_type, to_kill);
	dsk_free (to_kill);
	return true;
      }
  return false;
}

static void
dsk_error_data_type_errno__to_string (DskErrorDataType *type,
		                      const void *data,
				      DskBuffer *buffer)
{
  DSK_UNUSED (type);
  int e = * (const int *) data;
  dsk_buffer_printf (buffer, "[errno: %d %s]", e, strerror(e));
}

DskErrorDataType dsk_error_data_type_errno = {
  "errno",
  sizeof(int),
  NULL,
  NULL,
  dsk_error_data_type_errno__to_string
};
void        dsk_error_set_errno  (DskError   *error,
                                  int         error_no)
{
  * (int *) dsk_error_force_data (error, &dsk_error_data_type_errno, NULL) = error_no;
}

bool dsk_error_get_errno  (DskError   *error,
                                  int        *error_no_out)
{
  int *e = dsk_error_find_data (error, &dsk_error_data_type_errno);
  if (e)
    {
      *error_no_out = *e;
      return true;
    }
  return false;
}

static void clear_error (DskErrorDataType *type, void *data)
{
  DSK_UNUSED(type);

  DskError **p_err = data;
  if (*p_err == NULL)
    return;

  dsk_error_unref(*p_err);
  *p_err = NULL;
}

static void copy_error(DskErrorDataType *type,
                       void *dst_data,
		       const void *src_data)
{
  DSK_UNUSED(type);

  DskError *dst = * (DskError **) dst_data;
  DskError *src = * (DskError **) src_data;
  if (src != NULL)
    dsk_error_ref (src);
  if (dst != NULL)
    dsk_error_unref (dst);
  *(DskError **) dst = src;
}

static void dsk_error_data_type_cause__to_string
                      (DskErrorDataType *type,
		       const void *data,
		       DskBuffer *buffer)
{
  char *e = dsk_error_to_string (* (DskError **) data);
  DSK_UNUSED (type);
  dsk_buffer_append_byte (buffer, '[');
  dsk_buffer_append (buffer, strlen (e), e);
  dsk_free (e);
  dsk_buffer_append_byte (buffer, ']');
}

DskErrorDataType dsk_error_data_type_cause = {
  "cause",
  sizeof(DskError*),
  clear_error,
  copy_error,
  dsk_error_data_type_cause__to_string
};

void        dsk_error_set_cause  (DskError   *error,
                                  DskError   *cause)
{
  if (!cause)
    {
       dsk_error_remove_data(error, &dsk_error_data_type_cause);
       return;
    }

  DskError **err = (DskError **) dsk_error_force_data (error, &dsk_error_data_type_cause, NULL);
  dsk_error_ref (cause);
  if (*err)
    dsk_error_unref (*err);
  *err = cause;
}

DskError *dsk_error_get_cause  (DskError   *error)
{
  DskError **e = dsk_error_find_data (error, &dsk_error_data_type_cause);
  if (e == NULL)
    return NULL;
  return *e;
}

void        dsk_propagate_error  (DskError  **dst,
                                  DskError   *src)
{
  dsk_assert (src != NULL);
  if (dst)
    {
      dsk_assert (*dst == NULL);
      *dst = src;
    }
  else
    dsk_error_unref (src);
}

