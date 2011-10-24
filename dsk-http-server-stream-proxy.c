#include "dsk.h"

dsk_boolean
dsk_http_server_stream_respond_proxy
                               (DskHttpServerStreamTransfer *transfer,
                                DskHttpServerStreamProxyOptions *options,
                                DskError **error)
{
  DskUrlScheme scheme = DSK_URL_SCHEME_UNKNOWN;
  unsigned scheme_len;
  dsk_assert (!transfer->responded);

  if (!dsk_url_scheme_parse (transfer->request->path, &scheme_len,
                             &scheme, error))
    return DSK_FALSE;


  switch (scheme)
    {
    case DSK_URL_SCHEME_HTTP:
      {
        DskHttpClientRequestOptions req_options;
        if (options->request_options)
          req_options = *(options->request_options);
        else
          {
            static DskHttpClientRequestOptions ro = DSK_HTTP_CLIENT_REQUEST_OPTIONS_DEFAULT;
            req_options = ro;

            /* handle Via-like headers */
            ...
          }

        if (req_options.url == NULL
         && req_options.host == NULL
         && req_options.path == NULL)
          req_options.url = transfer->request->path;

        req_options.funcs = &proxy_client_request_funcs;
        req_options.func_data = ...;
        req_options.destroy = ...;

        /* make client request */
        if (!dsk_http_client_request (client, &req_options,
                                      transfer->post_data,
                                      &error))
          {
            ...
          }
        break;
      }
    case DSK_URL_SCHEME_FILE:
      dsk_set_error (error, "file: URLs not allowed by HTTP proxy");
      return DSK_FALSE;
    default:
      dsk_set_error (error, "unsupported url scheme '%s'",
                     dsk_url_scheme_name (scheme));
      return DSK_FALSE;
    }
  return DSK_TRUE;
}
