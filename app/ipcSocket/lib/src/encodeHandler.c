#include "encodeHandler.h"
#include <string.h>

bool encodePsw(const char *password, unsigned char *hash)
{
    EVP_MD_CTX *mdctx;
    const EVP_MD *md;
    unsigned int md_len;

    md = EVP_sha256(); // Use the SHA-256 algorithm

    if (!(mdctx = EVP_MD_CTX_new())) {
        return false;
    }

    if (1 != EVP_DigestInit_ex(mdctx, md, NULL)) {
        EVP_MD_CTX_free(mdctx);
        return false;
    }

    if (1 != EVP_DigestUpdate(mdctx, password, strlen(password))) {
        EVP_MD_CTX_free(mdctx);
        return false;
    }

    if (1 != EVP_DigestFinal_ex(mdctx, hash, &md_len)) {
        EVP_MD_CTX_free(mdctx);
        return false;
    }

    EVP_MD_CTX_free(mdctx);
    return true;
}