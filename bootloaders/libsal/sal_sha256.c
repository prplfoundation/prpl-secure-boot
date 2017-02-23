/*
Copyright (c) 2017, prpl Foundation

Permission to use, copy, modify, and/or distribute this software for any purpose with or without 
fee is hereby granted, provided that the above copyright notice and this permission notice appear 
in all copies.

THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE
INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE 
FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM 
LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, 
ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.

This code was written by Marcelo Veiga Neves at Embedded System Group (GSE) at PUCRS/Brazil.

*/

#include "sal_crypto.h"

#ifdef AXTLS_LITE

void sha256_init(sha256_t *sha256)
{
    SHA256_Init(sha256);
}

void sha256_update(sha256_t *sha256, const unsigned char *msg, int len)
{
    SHA256_Update(sha256, msg, len);
}

void sha256_final(sha256_t *sha256, unsigned char *digest)
{
    SHA256_Final(digest, sha256);
}

#else /* ! AXTLS_LITE */

void sha256_init(sha256_t *sha256)
{
    return;
}

void sha256_update(sha256_t *sha256, const unsigned char *msg, int len)
{
    return;
}

void sha256_final(sha256_t *sha256, unsigned char *digest)
{
    return;
}

#endif /* AXTLS_LITE */
