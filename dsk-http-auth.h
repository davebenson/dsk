/* Low-level Authentification Handling,
 * this should be independent of the other HTTP code.
 */

/* --- basic --- */
/* NOTE: NUL is not included in the length,
   and is not added by encode(). */
unsigned dsk_http_auth_basic_get_length (const char *username,
                                         const char *password);
void     dsk_http_auth_basic_encode     (const char *username,
                                         const char *password,
                                         char       *encoded_data);


/* --- digest --- */
DskHttpAuthDigest *dsk_http_auth_digest_new (char     **kv_pairs,
                                             DskError **error);
void               dsk_http_auth_digest_set_userpass (DskHttpAuthDigest *digest,
                                                      const char        *username,
                                                      const char        *password);
...

/* --- oauth --- */
...
