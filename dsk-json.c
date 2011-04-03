#include <string.h>
#include <stdlib.h>
#include "dsk.h"
#include "gskqsortmacro.h"

DskJsonValue *dsk_json_value_new_null   (void)
{
  DskJsonValue *rv = dsk_malloc (sizeof (DskJsonValue));
  rv->type = DSK_JSON_VALUE_NULL;
  return rv;
}
DskJsonValue *dsk_json_value_new_boolean(dsk_boolean    value)
{
  DskJsonValue *rv = dsk_malloc (sizeof (DskJsonValue));
  rv->type = DSK_JSON_VALUE_BOOLEAN;
  rv->value.v_boolean = value;
  return rv;
}
/* takes ownership of subvalues, but not of the members array */
DskJsonValue *dsk_json_value_new_object (unsigned       n_members,
                                         DskJsonMember *members)
{
  unsigned total_str_len = 0;
  unsigned i;
  char *str_heap;
  DskJsonValue *rv;
  DskJsonMember **smembers;
  for (i = 0; i < n_members; i++)
    total_str_len += strlen (members[i].name) + 1;
  rv = dsk_malloc (sizeof (DskJsonValue)
                   + sizeof (DskJsonMember) * n_members
                   + sizeof (DskJsonMember*) * n_members
                   + total_str_len);
  rv->type = DSK_JSON_VALUE_OBJECT;
  rv->value.v_object.n_members = n_members;
  rv->value.v_object.members = (DskJsonMember *) (rv + 1);
  smembers = (DskJsonMember **) (rv->value.v_object.members + n_members);
  rv->value.v_object.members_sorted_by_name = smembers;
  str_heap = (char*)(smembers + n_members);
  for (i = 0; i < n_members; i++)
    {
      rv->value.v_object.members[i].value = members[i].value;
      rv->value.v_object.members[i].name = str_heap;
      strcpy (str_heap, members[i].name);
      str_heap = strchr (str_heap, 0) + 1;
      smembers[i] = rv->value.v_object.members + i;
    }
#define COMPARE_MEMBERS(a,b, rv)   rv = strcmp (a->name, b->name)
  GSK_QSORT (smembers, DskJsonMember *, n_members, COMPARE_MEMBERS);
#undef COMPARE_MEMBERS
  return rv;
}

DskJsonMember *
dsk_json_object_get_member (DskJsonValue *value,
                            const char   *name)
{
  unsigned start = 0, count;
  dsk_return_val_if_fail (value->type == DSK_JSON_VALUE_OBJECT, "get_member called on non-object json type", NULL);
  count = value->value.v_object.n_members;
  while (count > 0)
    {
      unsigned mid = start + count / 2;
      DskJsonMember *member = value->value.v_object.members_sorted_by_name[mid];
      int cmp = strcmp (name, member->name);
      if (cmp < 0)
        {
          count /= 2;
        }
      else if (cmp > 0)
        {
          count = (start + count) - (mid + 1);
          start = mid + 1;
        }
      else
        return member;
    }
  return NULL;
}

DskJsonValue *
dsk_json_object_get_value  (DskJsonValue *value,
                            const char   *name)
{
  DskJsonMember *member = dsk_json_object_get_member (value, name);
  return member == NULL ? NULL : member->value;
}

/* takes ownership of subvalues, but not of the values array */
DskJsonValue *dsk_json_value_new_array  (unsigned       n_values,
                                         DskJsonValue **values)
{
  DskJsonValue *rv;
  rv = dsk_malloc (sizeof (DskJsonValue)
                   + sizeof (DskJsonValue *) * n_values);
  rv->type = DSK_JSON_VALUE_ARRAY;
  rv->value.v_array.n_values = n_values;
  rv->value.v_array.values = (DskJsonValue **) (rv + 1);
  memcpy (rv->value.v_array.values, values, sizeof (DskJsonValue*) * n_values);
  return rv;
}
DskJsonValue *dsk_json_value_new_string (unsigned       n_bytes,
                                         char          *str)
{
  DskJsonValue *rv;
  rv = dsk_malloc (sizeof (DskJsonValue) + n_bytes + 1);
  rv->type = DSK_JSON_VALUE_STRING;
  rv->value.v_string.length = n_bytes;
  rv->value.v_string.str = (char*)(rv+1);
  memcpy (rv->value.v_string.str, str, n_bytes);
  rv->value.v_string.str[n_bytes] = 0;
  return rv;
}

DskJsonValue *dsk_json_value_new_number (double         value)
{
  DskJsonValue *rv;
  rv = dsk_malloc (sizeof (DskJsonValue));
  rv->type = DSK_JSON_VALUE_NUMBER;
  rv->value.v_number = value;
  return rv;
}

void          dsk_json_value_free       (DskJsonValue  *value)
{
  unsigned i;
  if (value->type == DSK_JSON_VALUE_OBJECT)
    for (i = 0; i < value->value.v_object.n_members; i++)
      dsk_json_value_free (value->value.v_object.members[i].value);
  else if (value->type == DSK_JSON_VALUE_ARRAY)
    for (i = 0; i < value->value.v_array.n_values; i++)
      dsk_json_value_free (value->value.v_array.values[i]);
  dsk_free (value);
}

