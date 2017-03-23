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

#ifndef __SAL_CRYPTO__
#define __SAL_CRYPTO__

#ifdef AXTLS_LITE
#include <axtls-lite/os_int.h>
#include <axtls-lite/os_port.h>
#include <axtls-lite/crypto.h>
typedef SHA256_CTX sha256_t;
typedef RSA_CTX rsa_t;
#else
#error "No cryptograpic library"
#endif /* AXTLS_LITE */

void sha256_init(sha256_t *sha256);

void sha256_update(sha256_t *sha256, const unsigned char *msg, int len);

void sha256_final(sha256_t *sha256, unsigned char *digest);

/* Create a new public key from modulus/exponent
 */
void rsa_pub_key_new(rsa_t **rsa, const unsigned char *modulus, int mod_len,
	                            const unsigned char *pub_exp, int pub_len);

/* Decrypt a message using a public key
 */
int rsa_decrypt(const rsa_t *rsa, const unsigned char *signature, 
                            unsigned char *digest, int len);

#endif /* __SAL_CRYPTO__ */