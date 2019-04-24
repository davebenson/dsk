
struct DskTlsContext
{
...
};


DskTlsContext    *dsk_tls_context_new   (DskTlsContextOptions     *options,
                                         DskError                **error);
DskTlsContext    *dsk_tls_context_ref   (DskTlsContext            *context);
void              dsk_tls_context_unref (DskTlsContext            *context);


/* --- more complicated construction --- */
DskTslContextFactory *
dsk_tls_context_factory_new (...);

DskTslContext *
dsk_tls_context_factory_create_context (...);

DskTslContext *
dsk_tls_context_factory_convert_to_context (...);

DskTslContext *
dsk_tls_context_factory_destroy (DskTslContextFactory *factory);
