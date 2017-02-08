#include <stdio.h>
#include <string.h>
#include "dsk-common.h"
#include "dsk-object.h"
#include "dsk-error.h"

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

void       *dsk_error_force_data (DskError   *error,
                                  DskErrorDataType *data_type,
                                  dsk_boolean *created_out)
{
  char *data = dsk_error_find_data (error, data_type);
  if (data)
    {
      if (created_out)
        *created_out = DSK_FALSE;
      return data;
    }
  if (created_out)
    *created_out = DSK_TRUE;
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

DskErrorDataType dsk_error_data_type_errno = {
  "errno",
  sizeof(int),
  NULL
};
void        dsk_error_set_errno  (DskError   *error,
                                  int         error_no)
{
  * (int *) dsk_error_force_data (error, &dsk_error_data_type_errno, NULL) = error_no;
}

dsk_boolean dsk_error_get_errno  (DskError   *error,
                                  int        *error_no_out)
{
  int *e = dsk_error_find_data (error, &dsk_error_data_type_errno);
  if (e)
    {
      *error_no_out = *e;
      return DSK_TRUE;
    }
  return DSK_FALSE;
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
