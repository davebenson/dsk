
typedef enum
{
  DSK_DNS_SECTION_QUESTION      = 0x01,
  DSK_DNS_SECTION_ANSWER        = 0x02,
  DSK_DNS_SECTION_AUTHORITY     = 0x04,
  DSK_DNS_SECTION_ADDITIONAL    = 0x08,

  DSK_DNS_SECTION_ALL           = 0x0f
} DskDnsSectionCode;


typedef enum
{
  DSK_DNS_CLASS_IN      = 1,
  DSK_DNS_CLASS_ANY     = 255
} DskDnsClassCode;


/* DskDnsResourceRecordType: AKA RTYPE: 
 *       Types of `RR's or `ResourceRecord's (values match RFC 1035, 3.2.2)
 */
typedef enum
{
  /* An `A' record:  the ip address of a host. */
  DSK_DNS_RR_HOST_ADDRESS = 1,

  /* A `NS' record:  the authoritative name server for the domain */
  DSK_DNS_RR_NAME_SERVER = 2,

  /* A `CNAME' record:  indicate another name for an alias. */
  DSK_DNS_RR_CANONICAL_NAME = 5,

  /* A `HINFO' record: identifies the CPU and OS used by a host */
  DSK_DNS_RR_HOST_INFO = 13,

  /* A `MX' record */
  DSK_DNS_RR_MAIL_EXCHANGE = 15,

  /* A `PTR' record:  a pointer to another part of the domain name space */
  DSK_DNS_RR_POINTER = 12,

  /* A `SOA' record:  identifies the start of a zone of authority [???] */
  DSK_DNS_RR_START_OF_AUTHORITY = 6,

  /* A `TXT' record:  miscellaneous text */
  DSK_DNS_RR_TEXT = 16,

  /* A `WKS' record:  description of a well-known service */
  DSK_DNS_RR_WELL_KNOWN_SERVICE = 11,

  /* A `AAAA' record: for IPv6 (see RFC 1886) */
  DSK_DNS_RR_HOST_ADDRESS_IPV6 = 28,

  /* --- only allowed for queries --- */

  /* A `AXFR' record: `special zone transfer QTYPE' */
  DSK_DNS_RR_ZONE_TRANSFER = 252,

  /* A `MAILB' record: matches all mail box related RRs (e.g. MB and MG). */
  DSK_DNS_RR_ZONE_MAILB = 253,

  /* A `*' record:  matches anything. */
  DSK_DNS_RR_WILDCARD = 255

} DskDnsResourceRecordType;


typedef enum
{
  DSK_DNS_OP_QUERY  = 0,
  DSK_DNS_OP_IQUERY = 1,
  DSK_DNS_OP_STATUS = 2,
  DSK_DNS_OP_NOTIFY = 4,
  DSK_DNS_OP_UPDATE = 5,
} DskDnsOpcode;

typedef struct _DskDnsQuestion DskDnsQuestion;
struct _DskDnsQuestion
{
  /* The domain name for which information is being requested. */
  const char *name;

  /* The type of query we are asking. */
  DskDnsResourceRecordType  query_type;

  /* The domain where the query applies. */
  DskDnsClassCode query_class;
};


typedef enum
{
  DSK_DNS_RCODE_NOERROR    = 0,
  DSK_DNS_RCODE_FORMERR    = 1,
  DSK_DNS_RCODE_SERVFAIL   = 2,
  DSK_DNS_RCODE_NXDOMAIN   = 3,
  DSK_DNS_RCODE_NOTIMP     = 4,
  DSK_DNS_RCODE_REFUSED    = 5,
  DSK_DNS_RCODE_YXDOMAIN   = 6,
  DSK_DNS_RCODE_YXRRSET    = 7,
  DSK_DNS_RCODE_NXRRSET    = 8,
  DSK_DNS_RCODE_NOTAUTH    = 9,
  DSK_DNS_RCODE_NOTZONE    = 10,
} DskDnsRcode;

typedef struct _DskDnsResourceRecord DskDnsResourceRecord;
struct _DskDnsResourceRecord
{
  DskDnsResourceRecordType  type;
  const char               *owner;     /* where the resource_record is found */
  uint32_t                  time_to_live;
  DskDnsClassCode           class_code;
  DskDnsRcode               result_code;
  DskDnsOpcode              opcode;

  /* rdata: record type specific data */
  union
  {
    /* For DSK_DNS_RR_HOST_ADDRESS (and IPV6 variant)
       and DSK_DNS_CLASS_INTERNET.  We will make sure that
       aaaa.address and ip_address alias!  So you may
       use aaaa.address == a.ip_address, to save code */
    struct { uint8_t ip_address[4]; } a;
    struct { uint8_t address[16]; } aaaa;

		/* unsupported */
    /* For DSK_DNS_RR_HOST_ADDRESS and DSK_DNS_CLASS_CHAOS */
    struct
    {
      const char *chaos_name;
      uint16_t chaos_address;
    } a_chaos;

    /* For DSK_DNS_RR_CNAME, DSK_DNS_RR_POINTER, DSK_DNS_RR_NAME_SERVER */
    const char *domain_name;

    /* For DSK_DNS_RR_MAIL_EXCHANGE */
    struct
    {
      unsigned preference_value; /* "lower is better" */

      const char *mail_exchange_host_name;
    } mx;

    /* For DSK_DNS_RR_TEXT */
    const char *txt;

    /* For DSK_DNS_RR_HOST_INFO */
    struct
    {
      const char *cpu;
      const char *os;
    } hinfo;


    /* SOA: Start Of a zone of Authority.
     *
     * Comments cut-n-pasted from RFC 1035, 3.3.13.
     */
    struct
    {
      /* The domain-name of the name server that was the
	 original or primary source of data for this zone. */
      const char *mname;

      /* specifies the mailbox of the
	 person responsible for this zone. */
      const char *rname;

      /* The unsigned 32 bit version number of the original copy
	 of the zone.  Zone transfers preserve this value.  This
	 value wraps and should be compared using sequence space
	 arithmetic. */
      uint32_t serial;

      /* A 32 bit time interval before the zone should be
	 refreshed. (cf 1034, 4.3.5) [in seconds] */
      uint32_t refresh_time;

      /* A 32 bit time interval that should elapse before a
	 failed refresh should be retried. [in seconds] */
      uint32_t retry_time;

      /* A 32 bit time value that specifies the upper limit on
	 the time interval that can elapse before the zone is no
	 longer authoritative. [in seconds] */
      uint32_t expire_time;

      /* The unsigned 32 bit minimum TTL field that should be
	 exported with any RR from this zone. [in seconds] */
      uint32_t minimum_time;
    } soa;

  } rdata;
};


typedef struct _DskDnsMessage DskDnsMessage;
struct _DskDnsMessage
{
  unsigned n_questions;
  DskDnsQuestion *questions;
  unsigned n_answer_rr;
  DskDnsResourceRecord *answer_rr;
  unsigned n_authority_rr;
  DskDnsResourceRecord *authority_rr;
  unsigned n_additional_rr;
  DskDnsResourceRecord *additional_rr;

  uint16_t id;     /* used by requestor to match queries and replies */

  /* Is this a query or a response? */
  uint16_t is_query : 1;

  uint16_t is_authoritative : 1;
  uint16_t is_truncated : 1;

  /* [Responses only] the `RA bit': whether the server is willing to provide
   *                                recursive services. (cf 1034, 4.3.1)
   */
  uint16_t recursion_available : 1;

  /* [Queries only] the `RD bit': whether the requester wants recursive
   *                              service for this queries. (cf 1034, 4.3.1)
   */
  uint16_t recursion_desired : 1;

  uint8_t /*DskDnsOpcode*/ opcode;
  uint8_t /*DskDnsRcode*/ rcode;

};

#define DSK_DNS_PORT 53

DskDnsMessage *dsk_dns_message_parse     (unsigned       length,
                                          const uint8_t *data,
                                          DskError     **error);
uint8_t *      dsk_dns_message_serialize (DskDnsMessage *message,
                                          unsigned      *length_out);


/* diagnostics */
void dsk_dns_message_dump (DskDnsMessage *);
