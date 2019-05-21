#include "../dsk.h"

rsassa_pkcs1_sign (DskRSAKeyPair *kp,
                   size_t         message_len,
                   const uint8_t *message_data,
                   uint8_t       *signature);
                   
RSASSA-PKCS1
