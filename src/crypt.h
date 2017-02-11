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

#ifndef _CRYPT_MODULE_
#define _CRYPT_MODULE_

#include <openssl/rsa.h>

RSA *rsa_read_private_key(char *path);

RSA *rsa_read_public_key(char *path);

RSA *rsa_extract_public_key(RSA *privkey);

int rsa_encrypt(unsigned char *data, int len, unsigned char *signature, RSA *privkey);

int rsa_decrypt(unsigned char *signature, int len, unsigned char *digest, RSA *pubkey);

int sha256_compute(char* path, unsigned char hash[]);

#endif /* _CRYPT_MODULE_ */
