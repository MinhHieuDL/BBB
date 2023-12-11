#ifndef ENCODE_HANDLER
#define ENCODE_HANDLER

#include <stdbool.h>

bool encodePsw(const char *password, unsigned char *hash);
unsigned int getHashSize();

#endif