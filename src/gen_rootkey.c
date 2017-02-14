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
#include "crypt.h"

void write_public_key(RSA *rsa_pub)
{
    int i;
    unsigned char modulus[1024];
    unsigned int mod_len;
    unsigned char expo[1024];
    unsigned int expo_len;


    mod_len = BN_bn2bin(rsa_pub->n, modulus);
    expo_len = BN_bn2bin(rsa_pub->e, expo);

    printf("/*\
\n\
    Copyright (c) 2017, prpl Foundation\n\
\n\
    Permission to use, copy, modify, and/or distribute this software for any purpose with or without\n\
    fee is hereby granted, provided that the above copyright notice and this permission notice appear\n\
    in all copies.\n\
\n\
    THE SOFTWARE IS PROVIDED \"AS IS\" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE\n\
    INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE\n\
    FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM\n\
    LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,\n\
    ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.\n\
\n\n\
    This file was generated automatically by the gen_rootkey tool - do not edit\n\
*/\n\n");


    printf("#ifndef RSA_PUBLIC_KEY\n");
    printf("#define RSA_PUBLIC_KEY\n\n");

    printf("unsigned char rsa_modulus[] = {\n");
    for (i=0;i<mod_len;i++) {
        if (i % 16 == 0)
            printf("    ");
        printf("0x%02x", modulus[i]);
        if (i!=mod_len-1)
            printf(", ");
        if ((i+1) % 16 == 0)
            printf("\n");
    }
    printf("};\n");

    printf("unsigned int rsa_mod_len = %d;\n\n", mod_len);

    printf("unsigned char rsa_expo[] = {");
    for (i=0;i<expo_len;i++) {
        printf("0x%02x", expo[i]);
        if (i!=expo_len-1)
            printf(", ");
    }
    printf("};\n");

    printf("unsigned int rsa_expo_len = %d;\n\n", expo_len);

    printf("#endif\n");
}

int main(int argc, char *argv[])
{
    RSA *rsa_pub;
    
    if (argc != 2) {
        printf("Usage: %s public_key.pem\n", argv[0]);
        exit(1);
    }

    rsa_pub = rsa_read_public_key(argv[1]);
    if (rsa_pub == NULL) {
        printf("Cannot load public key.\n");
        exit(1);
    }

    write_public_key(rsa_pub);

    return 0;
}
