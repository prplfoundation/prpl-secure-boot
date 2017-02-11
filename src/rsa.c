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

#include <stdio.h>
#include <openssl/rsa.h>
#include <openssl/pem.h>
#include <openssl/err.h>


RSA *rsa_read_private_key(char *path)
{
    FILE *keyfile;
    RSA *rsa;

    keyfile = fopen(path, "r");
    if (keyfile == NULL) {
        perror("fopen");
        return NULL;
    }

    rsa = PEM_read_RSAPrivateKey(keyfile, NULL, NULL, NULL);

    fclose(keyfile);

    return rsa;
}


RSA *rsa_read_public_key(char *path)
{
    FILE *keyfile;
    RSA *rsa;

    keyfile = fopen(path, "r");
    if (keyfile == NULL) {
        perror("fopen");
        return NULL;
    }

    rsa = PEM_read_RSA_PUBKEY(keyfile, NULL, NULL, NULL);

    fclose(keyfile);

    return rsa;
}


RSA *rsa_extract_public_key(RSA *privkey)
{
    BIO *rsa_pub_bio = NULL;
    RSA *pubkey = NULL;

    rsa_pub_bio = BIO_new(BIO_s_mem());
    if (PEM_write_bio_RSAPublicKey(rsa_pub_bio, privkey) < 0) {
        BIO_free(rsa_pub_bio);
        return NULL;
    }

    pubkey = RSA_new();
    if (PEM_read_bio_RSAPublicKey(rsa_pub_bio, &pubkey, NULL, NULL) < 0) {
        BIO_free(rsa_pub_bio);
        RSA_free(pubkey);
        return NULL;
    }

    BIO_free(rsa_pub_bio);

    return pubkey;
}


int rsa_encrypt(unsigned char *data, int len, unsigned char *signature, RSA *privkey)
{
    int result;

    result = RSA_private_encrypt(len, data, signature, privkey, RSA_PKCS1_PADDING);

    return result;
}


int rsa_decrypt(unsigned char *signature, int len, unsigned char *digest, RSA *pubkey)
{
    int result;

    result = RSA_public_decrypt(len, signature, digest, pubkey, RSA_PKCS1_PADDING);

    return result;
}
