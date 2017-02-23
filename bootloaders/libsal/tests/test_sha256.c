#include <stdio.h>
#include <string.h>
#include "sal_crypto.h"

void print_hex(char *prefix, unsigned char buffer[], int len)
{
    int i = 0;

    printf("%s", prefix);

    for(i = 0; i < len; i++)
    {
        printf("%02x", buffer[i]);
    }

    printf(" (%d)\n", len);
}

int main()
{
    sha256_t sha256;
    unsigned char digest[32];
    unsigned char text[] = "abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq";
    unsigned char hash[] = {
        0x24, 0x8d, 0x6a, 0x61, 0xd2, 0x06, 0x38, 0xb8, 0xe5, 0xc0, 0x26, 0x93, 0x0c, 0x3e, 0x60, 0x39,
        0xa3, 0x3c, 0xe4, 0x59, 0x64, 0xff, 0x21, 0x67, 0xf6, 0xec, 0xed, 0xd4, 0x19, 0xdb, 0x06, 0xc1
    };

    sha256_init(&sha256);
    sha256_update(&sha256, text, strlen((char*)text));
    sha256_final(&sha256, digest);

    print_hex("digest: ", digest, 32);
    print_hex("hash: ", hash, 32);

    if (memcmp(hash, digest, 32) != 0) {
        printf("DO NOT MATCH!\n");
        return 1;
    } else {
        printf("MATCH!\n");
    }

    return 0;
}