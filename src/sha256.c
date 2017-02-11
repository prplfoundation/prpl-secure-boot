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
#include <stdlib.h>
#include <openssl/sha.h>
#include "crypt.h"

int sha256_compute(char* path, unsigned char hash[])
{
    SHA256_CTX sha256;
    FILE *file;
    char* buffer;
    const int buffer_size = 32768;
    int bytes_read = 0;

    file = fopen(path, "rb");
    if(file == NULL) {
        perror("fopen");
        return -1;
    }

    SHA256_Init(&sha256);

    buffer = malloc(buffer_size);
    if(buffer == NULL) {
        perror("malloc");
        return -1;
    }

    /* read from 0x00 to 0X0F */
    bytes_read = fread(buffer, 1, 0x10, file);
    SHA256_Update(&sha256, buffer, bytes_read);

    /* skip firmware header area */
    fread(buffer, 1, (0x200-0x10), file);

    while((bytes_read = fread(buffer, 1, buffer_size, file))) {
        SHA256_Update(&sha256, buffer, bytes_read);
    }

    SHA256_Final(hash, &sha256);

    fclose(file);
    free(buffer);

    return 0;
}
