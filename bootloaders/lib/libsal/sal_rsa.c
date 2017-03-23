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

void rsa_pub_key_new(rsa_t **rsa, const unsigned char *modulus, int mod_len,
                                  const unsigned char *pub_exp, int pub_len)
{
    RSA_pub_key_new(rsa, modulus, mod_len, pub_exp, pub_len);
}

int rsa_decrypt(const rsa_t *rsa, const unsigned char *signature, 
                            unsigned char *digest, int len)
{
    return RSA_decrypt(rsa, signature, digest, 1024, 0);
}

#else /* Not AXTLS_LITE */

void rsa_pub_key_new(rsa_t **rsa, const unsigned char *modulus, int mod_len,
                                  const unsigned char *pub_exp, int pub_len)
{
    return;
}

int rsa_decrypt(const rsa_t *rsa, const unsigned char *signature, 
                            unsigned char *digest, int len)
{
    return -1;
}

#endif /* AXTLS_LITE */
