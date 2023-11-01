#ifndef ENCODE_HANDLER
#define ENCODE_HANDLER

#include <openssl/evp.h>
#include <stdbool.h>

bool encodePsw(const char *password, unsigned char *hash);

#endif