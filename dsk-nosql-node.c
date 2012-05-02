/* Implement the server portion of a "redis"-clone node.
 */

/* All objects come with a snapshot object and a pile of updates. */

/* Format:
      uint32   - hash code
      varint32 - length of key
      bytes    - key
      varint32 - (length of object) - 1, or 0 for no-object
                 (just updates-- this comes up if we are restoring from backup)
      varint64 - time in usecs since epoch
      bytes    - object
      varint32 - n_updates
      for each update:
        varint64 - time in usecs since last time (either object time, or prior update entry)
	varint32 - update length
	bytes    - update data
 */

/* So, to define an object in this system the methods are:
       update[] coalesce_updates(update[])
       object_update(object, update[])
 */

/* So here's the types and operations we provide.
   Note that these are only modifiers.

  LIST operations:
    LPUSH(string), RPUSH(string)    -- add to left or right side of list
    LPOP(), RPOP()                  -- remove from left or right side of list
    LREMOVE(string), RREMOVE(string)-- remove first/last matching element
    LTRIM(integer), RTRIM(integer)  -- cut list to max size
  SET operations:
    INSERT(string)
    REMOVE(string)
    RENAME(string, string)
  HASH operations:
    INSERT(string,string)
    REMOVE(string)
    REMOVE_IF(string,string)
    RENAME(string,string)
    SET_IF(string,string,string)
    SET_IF_EXISTS(string,string)
    SET_IF_NOT_EXISTS(string,string)
    APPEND(string,string)
  SORTED_SET operations:
    INSERT(string, double)
    REMOVE(string)
    SET_SCORE(string, double)

  BIG_SET
  BIG_HASH
  BIG_SORTED_SET
 */

