#include <arpa/inet.h>                  /* htonl and htons */
#include <stdlib.h>
#include <string.h>
#include <stdio.h>                      /* TMP: debugging */
#include "dsk-rbtree-macros.h"
#include "dsk-common.h"
#include "dsk-object.h"
#include "dsk-error.h"
#include "dsk-dns-protocol.h"

#define MAX_PIECES              64

/* TODO: provide citation */
#define MAX_DOMAIN_NAME_LENGTH  1024

/* Used for binary packing/unpacking */
typedef struct _DskDnsHeader DskDnsHeader;
struct _DskDnsHeader
{
  unsigned qid:16;

#if BYTE_ORDER == BIG_ENDIAN
  unsigned qr:1;
  unsigned opcode:4;
  unsigned aa:1;
  unsigned tc:1;
  unsigned rd:1;

  unsigned ra:1;
  unsigned unused:3;
  unsigned rcode:4;
#else
  unsigned rd:1;
  unsigned tc:1;
  unsigned aa:1;
  unsigned opcode:4;
  unsigned qr:1;

  unsigned rcode:4;
  unsigned unused:3;
  unsigned ra:1;
#endif

  unsigned qdcount:16;
  unsigned ancount:16;
  unsigned nscount:16;
  unsigned arcount:16;
};


/* === parsing a binary message === */

/* Returns the name's length, including NUL */
static dsk_boolean
gather_name_length (unsigned       length,
                    const uint8_t *data,
                    unsigned      *used_inout,
                    unsigned      *sublen_out,
                    DskError     **error)
{
  unsigned used = *used_inout;
  unsigned at = used;
  unsigned rv = 0;
  dsk_boolean has_followed_pointer = DSK_FALSE;
  unsigned n_pieces = 0;
  for (;;)
    {
      if (at == length)
        {
          dsk_set_error (error, "truncated at beginning of name");
          return DSK_FALSE;
        }
      if (data[at] != 0 && (data[at] & 0xc0) == 0)
        {
          if (++n_pieces > MAX_PIECES)
            {
              dsk_set_error (error, "too many components in message (or circular loop)");
              return DSK_FALSE;
            }
          if (at + data[at] + 1 > length)
            {
              dsk_set_error (error, "string of length %u truncated", data[at]);
              return DSK_FALSE;
            }
          rv += data[at] + 1;
          at += data[at] + 1;
        }
      else if (data[at] == 0)
        {
          if (!has_followed_pointer)
            *used_inout = at + 1;
          *sublen_out = rv ? rv : 1;
          return DSK_TRUE;
        }
      else if ((data[at] & 0xc0) != 0xc0)
        {
          dsk_set_error (error, "invalid bytes in dns string");
          return DSK_FALSE;
        }
      else if (at + 1 == length)
        {
          dsk_set_error (error, "string truncated in middle of pointer");
          return DSK_FALSE;
        }
      else
        {
          unsigned new_offset = ((data[at] & ~0xc0) << 8) | data[at+1];
          if (!has_followed_pointer)
            {
              *used_inout = at + 2;
              has_followed_pointer = DSK_TRUE;
            }
          if (new_offset >= length)
            {
              dsk_set_error (error, "pointer out-of-bounds unpacking dns");
              return DSK_FALSE;
            }
          at = new_offset;
        }
    }
}


static dsk_boolean
gather_name_length_resource_record (unsigned       length,
                                    const uint8_t *data,
                                    unsigned      *used_inout, 
                                    unsigned      *str_space_inout,
                                    DskError     **error)
{
  const char *code;
  uint8_t header[10];
  unsigned sublen;
  DskDnsResourceRecordType type;
  unsigned rdlength;
  //const uint8_t *rddata;
  unsigned init_used;

  /* owner */
  if (!gather_name_length (length, data, used_inout, &sublen, error))
    return DSK_FALSE;
  *str_space_inout += sublen;
  if (*used_inout + 10 > length)
    {
      dsk_set_error (error, "truncated resource-record");
      return DSK_FALSE;
    }
  memcpy (header, data + *used_inout, 10);
  *used_inout += 10;
  type = ((uint16_t)header[0] << 8) | ((uint16_t)header[1] << 0);
  rdlength = ((uint16_t)header[8] << 8)  | ((uint16_t)header[9] << 0);
  if (*used_inout + rdlength > length)
    {
      dsk_set_error (error, "truncated resource-data");
      return DSK_FALSE;
    }
  //rddata = data + *used_inout;
  switch (type)
    {
    case DSK_DNS_RR_HOST_ADDRESS:       code = "d"; break;
    case DSK_DNS_RR_HOST_ADDRESS_IPV6:  code = "dddd"; break;
    case DSK_DNS_RR_NAME_SERVER:        code = "n"; break;
    case DSK_DNS_RR_CANONICAL_NAME:     code = "n"; break;
    case DSK_DNS_RR_POINTER:            code = "n"; break;
    case DSK_DNS_RR_MAIL_EXCHANGE:      code = "bbn"; break;
    case DSK_DNS_RR_HOST_INFO:          code = "ss"; break;
    case DSK_DNS_RR_START_OF_AUTHORITY: code = "nnddddd"; break;
    case DSK_DNS_RR_TEXT:               code = "ss"; break;
    case DSK_DNS_RR_WILDCARD:           code = ""; break;
    case DSK_DNS_RR_WELL_KNOWN_SERVICE:
    case DSK_DNS_RR_ZONE_TRANSFER:
    case DSK_DNS_RR_ZONE_MAILB:
      dsk_set_error (error, "unimplemented resource-record type %u", type);
      return DSK_FALSE;
    default:
      dsk_set_error (error, "unknown resource-record type %u", type);
      return DSK_FALSE;
    }

  init_used = *used_inout;
  while (*code)
    {
      switch (*code)
        {
        case 'b':
          if (*used_inout == length)
            goto truncated;
          *used_inout += 1;
        case 'd':
          if (*used_inout + 4 > length)
            goto truncated;
          *used_inout += 4;
          break;
        case 's':
          {
            unsigned c;
            if (*used_inout == length)
              goto truncated;
            c = data[*used_inout];
            if (c + 1 + *used_inout > length)
              goto truncated;
            *used_inout += c + 1;
            *str_space_inout += c + 1;
            break;
          }
        case 'n':
          if (!gather_name_length (length, data, used_inout, &sublen, error))
            return DSK_FALSE;
          *str_space_inout += sublen;
          break;
        default:
          dsk_assert_not_reached ();
        }
      code++;
    }
  if (init_used + rdlength != *used_inout)
    {
      dsk_set_error (error, "mismatch between parsed size %u and stated rdlength %u", 
                     *used_inout - init_used, rdlength);
      return DSK_FALSE;
    }
  return DSK_TRUE;
truncated:
  dsk_set_error (error, "data truncated in resource-record of type %u", type);
  return DSK_FALSE;
}

static const char *
parse_domain_name     (unsigned              length,
                       const uint8_t        *data,
                       unsigned             *used_inout,
                       char                **str_heap_at,
                       DskError            **error)
{
  const char *rv = *str_heap_at;
  while (*used_inout < length
      && data[*used_inout] != 0
      && (data[*used_inout] & 0xc0) == 0)
    {
      unsigned part_len = data[*used_inout];
      *used_inout += 1;
      dsk_assert (*used_inout + part_len < length);
      if (rv < *str_heap_at)
        {
          **str_heap_at = '.';
          *str_heap_at += 1;
        }
      memcpy (*str_heap_at, data + *used_inout, part_len);
      *str_heap_at += part_len;
      *used_inout += part_len;
    }
  if (*used_inout == length)
    {
      /* truncated */
      dsk_set_error (error, "truncated in domain name");
      return DSK_FALSE;
    }
  if (data[*used_inout] == 0)
    {
      *used_inout += 1;
    }
  else
    {
      unsigned at = (data[*used_inout] & ~0xc0) << 8
                  | (data[*used_inout+1]);
      *used_inout += 2;
      while (data[at] != 0)
        {
          if ((data[at] & 0xc0) == 0xc0)
            {
              /* pointer to new location */
              const uint8_t *addr = data + at;
              at = ((addr[0] & ~0xc0) << 8) | (addr[1]);
            }
          else
            {
              /* new length-prefixed name component */
              unsigned part_len = data[at++];
              dsk_assert (at + part_len < length);
              if (*str_heap_at > rv)
                {
                  **str_heap_at = '.';
                  *str_heap_at += 1;
                }
              memcpy (*str_heap_at, data + at, part_len);
              *str_heap_at += part_len;
              at += part_len;
            }
        }
    }

  **str_heap_at = 0;
  *str_heap_at += 1;
  return rv;
}

static const char *
parse_length_prefixed_string (const uint8_t *data,
                              unsigned      *used_inout,
                              char         **extra_space_inout)
{
  unsigned part_len = data[*used_inout];
  char *rv;
  *used_inout += 1;
  rv = *extra_space_inout;
  memcpy (rv, data + *used_inout, part_len);
  rv[part_len] = 0;
  *extra_space_inout += (part_len + 1);
  return rv;
}

static dsk_boolean
parse_question (unsigned          length,
                const uint8_t    *data,
                unsigned         *used_inout,
                DskDnsQuestion   *question,
                char            **str_heap_at,
                DskError        **error)
{
  const char *name;
  uint16_t array[2];
  name = parse_domain_name (length, data, used_inout, str_heap_at, error);
  if (*used_inout + 4 > length)
    {
      dsk_set_error (error, "data truncated in question");
      return DSK_FALSE;
    }
  memcpy (array, data + *used_inout, 4);
  *used_inout += 4;
  question->name = name;
  question->query_type = htons (array[0]);
  question->query_class = htons (array[1]);
  /* TODO: validate query_type and query_class */
  return DSK_TRUE;
}

static dsk_boolean
parse_resource_record (unsigned              length,
                       const uint8_t        *data,
                       unsigned             *used_inout,
                       DskDnsResourceRecord *rr,
                       char                **str_heap_at,
                       DskError            **error)
{
  uint8_t header[10];
  uint16_t type;
  uint16_t class;
  uint32_t ttl;
  //uint16_t rdlength;
  rr->owner = parse_domain_name (length, data, used_inout, str_heap_at, error);
  if (rr->owner == NULL)
    return DSK_FALSE;
  if (*used_inout + 10 > length)
    {
      dsk_set_error (error, "data truncated in resource-record header");
      return DSK_FALSE;
    }
  memcpy (header, data + *used_inout, 10);
  *used_inout += 10;
  type     = ((uint16_t)header[0] << 8)  | ((uint16_t)header[1] << 0);
  class    = ((uint16_t)header[2] << 8)  | ((uint16_t)header[3] << 0);
  ttl      = ((uint32_t)header[4] << 24) | ((uint32_t)header[5] << 16)
           | ((uint32_t)header[6] << 8)  | ((uint32_t)header[7] << 0);
  //rdlength = ((uint16_t)header[8] << 8)  | ((uint16_t)header[9] << 0);
  rr->type = type;
  rr->class_code = class;
  rr->time_to_live = ttl;
  switch (type)
    {
    case DSK_DNS_RR_HOST_ADDRESS:
      memcpy (rr->rdata.a.ip_address, data + *used_inout, 4);
      *used_inout += 4;
      break;
    case DSK_DNS_RR_HOST_ADDRESS_IPV6:
      memcpy (rr->rdata.aaaa.address, data + *used_inout, 16);
      *used_inout += 16;
      break;
    case DSK_DNS_RR_NAME_SERVER:
    case DSK_DNS_RR_CANONICAL_NAME:
    case DSK_DNS_RR_POINTER:
      rr->rdata.domain_name = parse_domain_name (length, data, used_inout, str_heap_at, error);
      if (rr->rdata.domain_name == NULL)
        return DSK_FALSE;
      break;
    case DSK_DNS_RR_HOST_INFO:
      rr->rdata.hinfo.cpu = parse_length_prefixed_string (data, used_inout, str_heap_at);
      rr->rdata.hinfo.os = parse_length_prefixed_string (data, used_inout, str_heap_at);
      break;
    case DSK_DNS_RR_MAIL_EXCHANGE:
      {
        uint16_t pv;
        memcpy (&pv, data + *used_inout, 2);
        rr->rdata.mx.preference_value = htons (pv);
        *used_inout += 2;
      }
      if ((rr->rdata.mx.mail_exchange_host_name = parse_domain_name (length, data, used_inout, str_heap_at, error)) == NULL)
       return DSK_FALSE;
      break;
    case DSK_DNS_RR_START_OF_AUTHORITY:
      if ((rr->rdata.soa.mname = parse_domain_name (length, data, used_inout, str_heap_at, error)) == NULL
       || (rr->rdata.soa.rname = parse_domain_name (length, data, used_inout, str_heap_at, error)) == NULL)
       return DSK_FALSE;
      {
        uint32_t intervals[5];
        memcpy (intervals, data + *used_inout, 20);
	rr->rdata.soa.serial = htonl (intervals[0]);
	rr->rdata.soa.refresh_time = htonl (intervals[1]);
	rr->rdata.soa.retry_time = htonl (intervals[2]);
	rr->rdata.soa.expire_time = htonl (intervals[3]);
	rr->rdata.soa.minimum_time = htonl (intervals[4]);
        *used_inout += 20;
      }
      break;
    case DSK_DNS_RR_TEXT:
      rr->rdata.txt = parse_length_prefixed_string (data, used_inout, str_heap_at);
      break;
    default:
      dsk_set_error (error, "invalid type %u of resource-record", type);
      return DSK_FALSE;
    }
  return DSK_TRUE;
}

static dsk_boolean
validate_opcode (DskDnsOpcode opcode)
{
  return opcode <= 5;
}
static dsk_boolean
validate_rcode (DskDnsRcode rcode)
{
  return rcode <= 10;
}

DskDnsMessage *
dsk_dns_message_parse (unsigned       length,
                       const uint8_t *data,
                       DskError     **error)
{
  DskDnsHeader header;
  unsigned i;
  unsigned total_rr;
  unsigned str_space = 0;
  unsigned used;
  DskDnsMessage *message;
  char *str_heap_at;
  if (length < 12)
    {
      dsk_set_error (error, "dns packet too short (<12 bytes)");
      return NULL;
    }

  dsk_assert (sizeof (header) == 12);
  memcpy (&header, data, 12);

#if BYTE_ORDER == LITTLE_ENDIAN
  header.qid = htons (header.qid);
  header.qdcount = htons (header.qdcount);
  header.ancount = htons (header.ancount);
  header.nscount = htons (header.nscount);
  header.arcount = htons (header.arcount);
#endif
  if (!validate_rcode (header.rcode))
    {
      dsk_set_error (error, "dns message had invalid 'rcode'");
      return NULL;
    }
  if (!validate_opcode (header.opcode))
    {
      dsk_set_error (error, "dns message had invalid 'opcode'");
      return NULL;
    }

  /* obtain a list of offsets to strings.
     each name may lead to N offsets,
     but that list should be exhaustive and unique,
     b/c strings can only appear in places we recognize.
     distinguish the offset that are used from those that aren't. */
  used = 12;
  for (i = 0; i < header.qdcount; i++)
    {
      unsigned sublen;
      if (!gather_name_length (length, data, &used, &sublen, error))
        goto cleanup;
      str_space += sublen;
      used += 4;
    }
  total_rr = header.ancount + header.nscount + header.arcount;
  for (i = 0; i < total_rr; i++)
    {
      if (!gather_name_length_resource_record (length, data,
                                               &used, &str_space, error))
        goto cleanup;
    }

  /* allocate space for the message */
  message = dsk_malloc (sizeof (DskDnsMessage)
                        + sizeof (DskDnsQuestion) * header.qdcount
                        + sizeof (DskDnsResourceRecord) * total_rr
                        + str_space);
  message->id = header.qid;
  message->is_query = header.qr ^ 1;
  message->is_authoritative = header.aa;
  message->is_truncated = header.tc;
  message->recursion_desired = header.rd;
  message->recursion_available = header.ra;
  message->n_questions = header.qdcount;
  message->questions = (DskDnsQuestion *) (message + 1);
  message->n_answer_rr = header.ancount;
  message->answer_rr = (DskDnsResourceRecord *) (message->questions + message->n_questions);
  message->n_authority_rr = header.nscount;
  message->authority_rr = message->answer_rr + message->n_answer_rr;
  message->n_additional_rr = header.arcount;
  message->additional_rr = message->authority_rr + message->n_authority_rr;
  message->rcode = header.rcode;
  message->opcode = header.opcode;

  str_heap_at = (char*) (message->additional_rr + message->n_additional_rr);

  /* parse the four sections */
  used = 12;
  for (i = 0; i < header.qdcount; i++)
    if (!parse_question (length, data, &used,
                         &message->questions[i],
                         &str_heap_at,
                         error))
      goto cleanup;
  for (i = 0; i < total_rr; i++)
    if (!parse_resource_record (length, data, &used,
                                &message->answer_rr[i],
                                &str_heap_at,
                                error))
      goto cleanup;

  return message;

cleanup:
  return NULL;
}

/* === dsk_dns_message_serialize === */
typedef struct _StrTreeNode StrTreeNode;
struct _StrTreeNode
{
  unsigned offset;
  const char *str;

  /* Tree for the next word */
  StrTreeNode *subtree;

  /* child nodes */
  StrTreeNode *left, *right, *parent;
  unsigned is_red;
};

static dsk_boolean
validate_name (const char *domain_name)
{
  unsigned n_non_dot = 0;
  if (*domain_name == '.')
    ++domain_name;
  while (*domain_name == 0)
    {
      if (*domain_name == '.')
        {
          if (n_non_dot == 0)
            return DSK_FALSE;
          n_non_dot = 0;
        }
      else
        {
          if (!('a' <= *domain_name && *domain_name <= 'z')
               && *domain_name != '-' && *domain_name != '_')
            return DSK_FALSE;
          domain_name++;
          n_non_dot++;
          if (n_non_dot > 63)
            return DSK_FALSE;
        }
    }
  return DSK_TRUE;
}

static dsk_boolean
validate_question (DskDnsQuestion *question)
{
  if (!validate_name (question->name))
    return DSK_FALSE;
  /* TODO: consider validating class/rr-type */
  return DSK_TRUE;
}

static dsk_boolean
validate_resource_record (DskDnsResourceRecord *rr)
{
  if (!validate_name (rr->owner))
    return DSK_FALSE;
  switch (rr->type)
    {
    case DSK_DNS_RR_NAME_SERVER:
    case DSK_DNS_RR_CANONICAL_NAME:
    case DSK_DNS_RR_POINTER:
      return validate_name (rr->rdata.domain_name);
    case DSK_DNS_RR_HOST_INFO:
      return rr->rdata.hinfo.cpu != NULL
          && strlen (rr->rdata.hinfo.cpu) <= 255
          && rr->rdata.hinfo.os != NULL
          && strlen (rr->rdata.hinfo.os) <= 255;
    case DSK_DNS_RR_MAIL_EXCHANGE:
      return validate_name (rr->rdata.mx.mail_exchange_host_name);
    case DSK_DNS_RR_START_OF_AUTHORITY:
      return validate_name (rr->rdata.soa.mname)
          && validate_name (rr->rdata.soa.rname);
    case DSK_DNS_RR_TEXT:
    case DSK_DNS_RR_HOST_ADDRESS:
    case DSK_DNS_RR_HOST_ADDRESS_IPV6:
      return DSK_TRUE;
    case DSK_DNS_RR_WELL_KNOWN_SERVICE:
    default:
      return DSK_FALSE;
    }
}

static unsigned get_name_n_components (const char *str)
{
  unsigned rv = 0;
  if (*str == '.')
    str++;
  while (*str)
    {
      if (*str == '.')
        {
          if (str[1] == 0)
            break;
          rv++;
        }
      str++;
    }
  return rv;
}
static unsigned
get_question_n_components (DskDnsQuestion *question)
{
  return get_name_n_components (question->name);
}
static unsigned
get_rr_n_components (DskDnsResourceRecord *rr)
{
  unsigned rv = get_name_n_components (rr->owner);
  switch (rr->type)
    {
    case DSK_DNS_RR_NAME_SERVER:
    case DSK_DNS_RR_CANONICAL_NAME:
    case DSK_DNS_RR_POINTER:
      rv += get_name_n_components (rr->rdata.domain_name);
      break;
    case DSK_DNS_RR_MAIL_EXCHANGE:
      rv += get_name_n_components (rr->rdata.mx.mail_exchange_host_name);
      break;
    case DSK_DNS_RR_START_OF_AUTHORITY:
      rv += get_name_n_components (rr->rdata.soa.mname);
      break;
    default:
      break;
    }
  return rv;
}

static unsigned
get_max_str_nodes (DskDnsMessage *message)
{
  unsigned max_str_nodes = 1;
  unsigned i;
  for (i = 0; i < message->n_questions; i++)
    max_str_nodes += get_question_n_components (message->questions + i);
  for (i = 0; i < message->n_answer_rr; i++)
    max_str_nodes += get_rr_n_components (message->answer_rr + i);
  for (i = 0; i < message->n_authority_rr; i++)
    max_str_nodes += get_rr_n_components (message->authority_rr + i);
  for (i = 0; i < message->n_additional_rr; i++)
    max_str_nodes += get_rr_n_components (message->additional_rr + i);
  return max_str_nodes;
}

static int
compare_dot_terminated_strs (const char *a,
                             const char *b)
{
#define IS_END_CHAR(c)  ((c) == 0 || (c) == '.')
  char ca, cb;
  while (!IS_END_CHAR (*a) && !IS_END_CHAR (*b))
    {
      if (*a < *b) return -1;
      else if (*a > *b) return 1;
      a++, b++;
    }
  ca = (*a == '.') ? 0 : *a;
  cb = (*b == '.') ? 0 : *b;
  return (ca < cb) ? -1 : (ca > cb) ? 1 : 0;
#undef IS_END_CHAR
}

#define COMPARE_STR_TREE_NODES(a,b, rv) \
         rv = compare_dot_terminated_strs (a->str, b->str)
#define COMPARE_STR_TO_TREE_NODE(a,b, rv) \
         rv = compare_dot_terminated_strs (a, b->str)

#define STR_NODE_GET_TREE(top) \
  (top), StrTreeNode*,  \
  DSK_STD_GET_IS_RED, DSK_STD_SET_IS_RED, \
  parent, left, right, \
  COMPARE_STR_TREE_NODES

static unsigned 
get_name_size (const char     *name,
               StrTreeNode   **p_top,
               StrTreeNode   **p_heap)
{
  const char *end = strchr (name, 0);
  StrTreeNode **p_at = p_top;
  dsk_boolean is_first = DSK_TRUE;
  dsk_boolean use_ptr = DSK_FALSE;
  unsigned str_size = 0;
  if (end == name)
    return 1;
  for (;;)
    {
      const char *cstart;
      StrTreeNode *child;
      while (end >= name && end[-1] == '.')
        end--;
      if (end == name)
        break;
      cstart = end;
      while (cstart > name && cstart[-1] != '.')
        cstart--;
      if (*p_at != NULL)
        /* lookup child with this name */
        DSK_RBTREE_LOOKUP_COMPARATOR (STR_NODE_GET_TREE (*p_at),
                                      cstart, COMPARE_STR_TO_TREE_NODE,
                                      child);
      else
        child = NULL;
      if (is_first)
        {
          use_ptr = (child != NULL);
          is_first = DSK_FALSE;
        }
      if (child == NULL)
        {
          StrTreeNode *conflict;
          /* create new node */
          child = *p_heap;
          *p_heap += 1;
          memset (child, 0, sizeof (StrTreeNode));
          child->str = cstart;

          /* insert node */
          DSK_RBTREE_INSERT (STR_NODE_GET_TREE (*p_at), 
                             child, conflict);
          dsk_assert (conflict == NULL);

          /* increase size */
          str_size += (end - cstart) + 1;
        }
      p_at = &child->subtree;

      if (cstart == name)
        break;
      end = cstart - 1;
    }
  return str_size + (use_ptr ? 2 : 1);
}

static unsigned 
get_question_size (DskDnsQuestion *question,
                   StrTreeNode   **p_top,
                   StrTreeNode   **p_heap)
{
  unsigned rv = get_name_size (question->name, p_top, p_heap) + 4;
  return rv;
}
static unsigned
get_rr_size (DskDnsResourceRecord *rr,
             StrTreeNode         **p_top,
             StrTreeNode         **p_heap)
{
  unsigned rv = get_name_size (rr->owner, p_top, p_heap) + 10;
  switch (rr->type)
    {
    case DSK_DNS_RR_HOST_ADDRESS:
      rv += 4;
      break;
    case DSK_DNS_RR_NAME_SERVER:
    case DSK_DNS_RR_CANONICAL_NAME:
    case DSK_DNS_RR_POINTER:
      rv += get_name_size (rr->rdata.domain_name, p_top, p_heap);
      break;
    case DSK_DNS_RR_HOST_INFO:
      rv += 4;
      break;
    case DSK_DNS_RR_MAIL_EXCHANGE:
      rv += get_name_size (rr->rdata.mx.mail_exchange_host_name, p_top, p_heap);
      rv += 2;
      break;
    case DSK_DNS_RR_START_OF_AUTHORITY:
      rv += get_name_size (rr->rdata.soa.mname, p_top, p_heap);
      rv += get_name_size (rr->rdata.soa.rname, p_top, p_heap);
      rv += 20;
      break;
    case DSK_DNS_RR_TEXT:
      rv += strlen (rr->rdata.txt) + 1;
      break;
    case DSK_DNS_RR_HOST_ADDRESS_IPV6:
      rv += 16;
      break;
    default:
      dsk_assert_not_reached ();
    }
  return rv;
}

static void
pack_message_header (DskDnsMessage *message,
                     uint8_t       *out)
{
  DskDnsHeader header;
  header.qid = htons (message->id);
  header.qr = 1 ^ message->is_query;
  header.opcode = message->opcode;
  header.aa = message->is_authoritative;
  header.tc = 0;        //message->is_truncated;
  header.rd = message->recursion_desired;
  header.ra = message->recursion_available;
  header.unused = 0;
  header.rcode = message->rcode;
  header.qdcount = htons (message->n_questions);
  header.ancount = htons (message->n_answer_rr);
  header.nscount = htons (message->n_authority_rr);
  header.arcount = htons (message->n_additional_rr);
  dsk_assert (sizeof (DskDnsHeader) == 12);
  memcpy (out, &header, 12);
}
static void
write_pointer (uint8_t **data_inout,
               unsigned offset)
{
  /* write pointer */
  uint8_t bytes[2];
  bytes[0] = 0xc0 | (offset >> 8);
  bytes[1] = offset;
  memcpy (*data_inout, bytes, 2);
  *data_inout += 2;
}

static void
pack_domain_name  (const char     *name,
                   uint8_t        *data_start,      /* for computing offsets */
                   uint8_t       **data_inout,
                   StrTreeNode    *top)
{
  StrTreeNode *up = NULL;
  const char *end;

  if (name[0] == '.')
    name++;
  if (name[0] == 0)
    {
      **data_inout = 0;
      *data_inout += 1;
      return;
    }
  end = strchr (name, 0);
  if (end > name && end[-1] == '.')
    end--;

  /* scan up tree until we find one with offset==0;
     or until we run out of components */
  {
  const char *beg = NULL;
  StrTreeNode *cur = NULL;
  while (name < end)
    {
      beg = end;
      while (beg > name && beg[-1] != '.')
        beg--;
      DSK_RBTREE_LOOKUP_COMPARATOR (STR_NODE_GET_TREE (top),
                                    beg, COMPARE_STR_TO_TREE_NODE,
                                    cur);
      dsk_assert (cur != NULL);
      if (cur->offset == 0)
        break;
      end = beg;
      if (end > name)
        end--;
      up = top;
      top = cur->subtree;
    }
  

  if (name < end)
    {
      /* write remaining strings -- first calculate the size
         then start at the end. */
      unsigned packed_str_size = (end + 1) - name;
      uint8_t *at = *data_inout + packed_str_size;
      while (end != name)
        {
          at -= (end-beg);
          memcpy (at, beg, end - beg);
          at--;
          *at = (end-beg);

          cur->offset = (at - data_start);
          top = cur->subtree;

          end = beg;
          if (end > name)
            {
              end--;          /* skip . */
              beg = end;
              while (beg > name && beg[-1] != '.')
                beg--;
              DSK_RBTREE_LOOKUP_COMPARATOR (STR_NODE_GET_TREE (top),
                                    beg, COMPARE_STR_TO_TREE_NODE,
                                    cur);
              dsk_assert (cur != NULL);
            }
        }
      dsk_assert (at == *data_inout);
      *data_inout += packed_str_size;

      if (up != NULL)
        write_pointer (data_inout, up->offset);
      else
        {
          /* write 0 */
          **data_inout = 0;
          *data_inout += 1;
        }
      return;
    }
  }
  write_pointer (data_inout, up->offset);
}

static void
pack_question (DskDnsQuestion *question,
               uint8_t        *data_start,      /* for computing offsets */
               uint8_t       **data_inout,
               StrTreeNode    *top)
{
  uint16_t qarray[2];
  pack_domain_name (question->name, data_start, data_inout, top);
  qarray[0] = htons (question->query_type);
  qarray[1] = htons (question->query_class);
  memcpy (*data_inout, qarray, 4);
  *data_inout += 4;
}

static void
pack_len_prefixed_string (const char *str,
                          uint8_t   **data_inout)
{
  unsigned length = strlen (str);
  **data_inout = length;
  *data_inout += 1;
  memcpy (*data_inout, str, length);
  *data_inout += length;
}

static void
pack_resource_record (DskDnsResourceRecord *rr,
                      uint8_t        *data_start, /* for computing offsets */
                      uint8_t       **data_inout,
                      StrTreeNode    *top)
{
  uint8_t *generic;
  unsigned rdata_len;
  uint16_t data[5];

  pack_domain_name (rr->owner, data_start, data_inout, top);

  /* reserve space for generic part of resource-record */
  generic = *data_inout;
  *data_inout += 10;

  /* pack type-specific rdata */
  switch (rr->type)
    {
    case DSK_DNS_RR_HOST_ADDRESS:
      memcpy (*data_inout, rr->rdata.a.ip_address, 4);
      *data_inout += 4;
      break;
    case DSK_DNS_RR_HOST_ADDRESS_IPV6:
      memcpy (*data_inout, rr->rdata.aaaa.address, 16);
      *data_inout += 16;
      break;
    case DSK_DNS_RR_NAME_SERVER:
    case DSK_DNS_RR_CANONICAL_NAME:
    case DSK_DNS_RR_POINTER:
      pack_domain_name (rr->rdata.domain_name, data_start, data_inout, top);
      break;
    case DSK_DNS_RR_MAIL_EXCHANGE:
      {
        uint16_t pv_be = htons (rr->rdata.mx.preference_value);
        memcpy (*data_inout, &pv_be, 2);
        *data_inout += 2;
      }
      pack_domain_name (rr->rdata.mx.mail_exchange_host_name, data_start, data_inout, top);
      break;
    case DSK_DNS_RR_HOST_INFO:
      pack_len_prefixed_string (rr->rdata.hinfo.cpu, data_inout);
      pack_len_prefixed_string (rr->rdata.hinfo.os, data_inout);
      break;
    case DSK_DNS_RR_START_OF_AUTHORITY:
      pack_domain_name (rr->rdata.soa.mname, data_start, data_inout, top);
      pack_domain_name (rr->rdata.soa.rname, data_start, data_inout, top);
      {
        uint32_t intervals[5];
	intervals[0] = htonl (rr->rdata.soa.serial);
	intervals[1] = htonl (rr->rdata.soa.refresh_time);
	intervals[2] = htonl (rr->rdata.soa.retry_time);
	intervals[3] = htonl (rr->rdata.soa.expire_time);
	intervals[4] = htonl (rr->rdata.soa.minimum_time);
        memcpy (*data_inout, intervals, 20);
        *data_inout += 20;
      }
      break;
    case DSK_DNS_RR_TEXT:
      pack_len_prefixed_string (rr->rdata.txt, data_inout);
      break;
    default:
      /* This should not happen, because validate_resource_record()
         returned TRUE, */
      dsk_assert_not_reached ();
    }

  /* write generic resource-code info */
  rdata_len = *data_inout - generic;
  data[0] = htons (rr->type);
  data[1] = htons (DSK_DNS_CLASS_IN);
  data[2] = htons (rr->time_to_live >> 16);
  data[3] = htons (rr->time_to_live);
  data[4] = htons (rdata_len);
  memcpy (generic, data, 10);
}

uint8_t *
dsk_dns_message_serialize (DskDnsMessage *message,
                           unsigned      *length_out)
{
  unsigned max_str_nodes;
  StrTreeNode *nodes;
  StrTreeNode *nodes_at;                /* next node to us */
  StrTreeNode *top = NULL;
  unsigned i;
  unsigned size;
  uint8_t *rv, *at;

  /* check message contents:
     - string bounds
     - opcode / errcode validity
     - lowercased domain-names (?)
     - n_questions, etc must be less than 1<<16
   */
  if (message->n_questions > 0xffff
   || message->n_answer_rr > 0xffff
   || message->n_authority_rr > 0xffff
   || message->n_additional_rr > 0xffff)
    return NULL;
  if (!validate_rcode (message->rcode)
   || !validate_opcode (message->opcode))
    return NULL;
  for (i = 0; i < message->n_questions; i++)
    if (!validate_question (message->questions + i))
      return NULL;
  for (i = 0; i < message->n_answer_rr; i++)
    if (!validate_resource_record (message->answer_rr + i))
      return NULL;
  for (i = 0; i < message->n_authority_rr; i++)
    if (!validate_resource_record (message->authority_rr + i))
      return NULL;
  for (i = 0; i < message->n_additional_rr; i++)
    if (!validate_resource_record (message->additional_rr + i))
      return NULL;

  max_str_nodes = get_max_str_nodes (message);

  /* scan through figuring out how long the packed data will be. */
  nodes = alloca (sizeof (StrTreeNode) * max_str_nodes);
  nodes_at = nodes;
  size = 12;
  for (i = 0; i < message->n_questions; i++)
    size += get_question_size (message->questions + i, &top, &nodes_at);
  for (i = 0; i < message->n_answer_rr; i++)
    size += get_rr_size (message->answer_rr + i, &top, &nodes_at);
  for (i = 0; i < message->n_authority_rr; i++)
    size += get_rr_size (message->authority_rr + i, &top, &nodes_at);
  for (i = 0; i < message->n_additional_rr; i++)
    size += get_rr_size (message->additional_rr + i, &top, &nodes_at);
  dsk_assert ((unsigned)(nodes_at - nodes) <= max_str_nodes);

  /* pack the message */
  rv = dsk_malloc (size);
  at = rv;
  pack_message_header (message, at);
  at += sizeof (DskDnsHeader);
  for (i = 0; i < message->n_questions; i++)
    pack_question (message->questions + i, rv, &at, top);
  for (i = 0; i < message->n_answer_rr; i++)
    pack_resource_record (message->answer_rr + i, rv, &at, top);
  for (i = 0; i < message->n_authority_rr; i++)
    pack_resource_record (message->authority_rr + i, rv, &at, top);
  for (i = 0; i < message->n_additional_rr; i++)
    pack_resource_record (message->additional_rr + i, rv, &at, top);
  dsk_assert ((unsigned)(at - rv) == size);
  *length_out = size;

  return rv;
}


/* --- debugging --- */
static const char *
dsk_dns_resource_record_type_name (DskDnsResourceRecordType type)
{

  switch (type)
    {
#define WRITE_CASE(shortname)  case DSK_DNS_RR_##shortname: return #shortname
    WRITE_CASE (HOST_ADDRESS);
    WRITE_CASE (NAME_SERVER);
    WRITE_CASE (CANONICAL_NAME);
    WRITE_CASE (HOST_INFO);
    WRITE_CASE (MAIL_EXCHANGE);
    WRITE_CASE (POINTER);
    WRITE_CASE (START_OF_AUTHORITY);
    WRITE_CASE (TEXT);
    WRITE_CASE (WELL_KNOWN_SERVICE);
    WRITE_CASE (HOST_ADDRESS_IPV6);
    WRITE_CASE (ZONE_TRANSFER);
    WRITE_CASE (ZONE_MAILB);
    WRITE_CASE (WILDCARD);
    default: return "*unknown-rr-type*";
#undef WRITE_CASE
    }
}

static const char *
dsk_dns_class_name (DskDnsClassCode code)
{

  switch (code)
    {
#define WRITE_CASE(shortname)  case DSK_DNS_CLASS_##shortname: return #shortname
    WRITE_CASE (IN);
    WRITE_CASE (ANY);
    default: return "*unknown-class*";
#undef WRITE_CASE
    }
}
static const char *
dsk_dns_rcode_name (DskDnsRcode code)
{

  switch (code)
    {
#define WRITE_CASE(shortname)  case DSK_DNS_RCODE_##shortname: return #shortname
    WRITE_CASE (NOERROR);
    WRITE_CASE (FORMERR);
    WRITE_CASE (SERVFAIL);
    WRITE_CASE (NXDOMAIN);
    WRITE_CASE (NOTIMP);
    WRITE_CASE (REFUSED);
    WRITE_CASE (YXDOMAIN);
    WRITE_CASE (YXRRSET);
    WRITE_CASE (NXRRSET);
    WRITE_CASE (NOTAUTH);
    WRITE_CASE (NOTZONE);
    default: return "*unknown-rcode*";
#undef WRITE_CASE
    }
}
static const char *
dsk_dns_opcode_name (DskDnsOpcode code)
{

  switch (code)
    {
#define WRITE_CASE(shortname)  case DSK_DNS_OP_##shortname: return #shortname
    WRITE_CASE (QUERY);
    WRITE_CASE (IQUERY);
    WRITE_CASE (STATUS);
    WRITE_CASE (NOTIFY);
    WRITE_CASE (UPDATE);
    default: return "*unknown-opcode*";
#undef WRITE_CASE
    }
}

#include <stdio.h>
static void 
print_rr (DskDnsResourceRecord *rr)
{
  unsigned i;
  printf ("  %s  %s", rr->owner, dsk_dns_resource_record_type_name (rr->type));
  switch (rr->type)
    {
    case DSK_DNS_RR_HOST_ADDRESS:
      printf (" %u.%u.%u.%u\n",
              rr->rdata.a.ip_address[0],
              rr->rdata.a.ip_address[1],
              rr->rdata.a.ip_address[2],
              rr->rdata.a.ip_address[3]);
      break;
    case DSK_DNS_RR_CANONICAL_NAME:
    case DSK_DNS_RR_POINTER:
    case DSK_DNS_RR_NAME_SERVER:
      printf (" %s\n", rr->rdata.domain_name);
      break;

    case DSK_DNS_RR_MAIL_EXCHANGE:
      printf (" %u %s\n", rr->rdata.mx.preference_value, rr->rdata.mx.mail_exchange_host_name);
      break;

    case DSK_DNS_RR_TEXT:
      printf (" %s\n", rr->rdata.txt);
      break;
    case DSK_DNS_RR_HOST_INFO:
      printf (" %s   %s\n", rr->rdata.hinfo.cpu,rr->rdata.hinfo.os);
      break;


    case DSK_DNS_RR_START_OF_AUTHORITY:
      printf (" %s %s %u %u %u %u %u\n",
              rr->rdata.soa.mname,
              rr->rdata.soa.rname,
              rr->rdata.soa.serial,
              rr->rdata.soa.refresh_time,
              rr->rdata.soa.retry_time,
              rr->rdata.soa.expire_time,
              rr->rdata.soa.minimum_time);
      break;
    case DSK_DNS_RR_HOST_ADDRESS_IPV6:
      printf (" %x", rr->rdata.aaaa.address[0]);
      for (i = 1; i < 16; i++)
        printf (":%x", rr->rdata.aaaa.address[i]);
      printf ("\n");
    default:
      dsk_assert_not_reached ();
    }
}

void
dsk_dns_message_dump (DskDnsMessage *message)
{
  unsigned i;
  printf ("id: %u\n"
          "is_query: %u\n"
          "is_authoritative: %u\n"
          "is_truncated: %u\n"
          "recursion_available: %u\n"
          "recursion_desired: %u\n"
          "result code: %s\n"
          "opcode: %s\n",
          message->id,
          message->is_query,
          message->is_authoritative,
          message->is_truncated,
          message->recursion_available,
          message->recursion_desired,
          dsk_dns_rcode_name (message->rcode),
          dsk_dns_opcode_name (message->opcode));
  printf ("QUESTIONS:\n");
  for (i = 0; i < message->n_questions; i++)
    {
      printf ("  name: %s\n"
              "  type: %s\n"
              "  class: %s\n",
              message->questions[i].name,
              dsk_dns_resource_record_type_name (message->questions[i].query_type),
              dsk_dns_class_name (message->questions[i].query_class));
    }
  printf ("ANSWERS:\n");
  for (i = 0; i < message->n_answer_rr; i++)
    print_rr (message->answer_rr + i);
  printf ("AUTHORITY:\n");
  for (i = 0; i < message->n_authority_rr; i++)
    print_rr (message->authority_rr + i);
  printf ("ADDITIONAL:\n");
  for (i = 0; i < message->n_additional_rr; i++)
    print_rr (message->additional_rr + i);
}
