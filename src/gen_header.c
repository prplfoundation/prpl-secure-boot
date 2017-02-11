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
#include <string.h>

#include <openssl/rsa.h>
#include <openssl/pem.h>
#include <openssl/err.h>

#include "crypt.h"
#include "header.h"

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

void print_header(struct firmware_header hdr)
{
    printf("\nprplSecureBoot Firmware Header:\n");
    printf("Header version:          0x%02x\n", hdr.header_version);
    printf("Flash start address:     0x%02x\n", hdr.flash_start_addr);
    printf("Execution address:       0x%02x\n", hdr.user_app_addr);
    printf("Firmware start address:  0x%02x\n", hdr.firmware_start_addr);
    printf("Firmware length:         0x%02x (%d)\n", hdr.firmware_length, hdr.firmware_length);
    print_hex("Firmware hash:           ", hdr.hash, 32);
    print_hex("Firmware signature:      ", hdr.signature, 256);
    printf("Header size: %d\n", (int)sizeof(hdr));
}



int write_header(char* path, struct firmware_header hdr)
{
    FILE *old, *new;
    char signed_file[512];
    int bytes_read = 0;
    const int buffer_size = 32768;
    char *buffer;

    sprintf(signed_file, "%s.signed", path);

    old = fopen(path, "rb");
    if(old == NULL) {
        perror("fopen");
        return -1;
    }

    new = fopen(signed_file, "wb");
    if(new == NULL) {
        perror("fopen");
        return -1;
    }

    buffer = malloc(buffer_size);
    if(buffer == NULL) {
        perror("malloc");
        return -1;
    }

    /* read from 0x00 to 0X0F */
    bytes_read = fread(buffer, 1, 0x10, old);
    fwrite(buffer, 1, bytes_read, new);

    /* firmware header */
    fread(buffer, 1, sizeof(hdr), old);
    fwrite(&hdr, 1, sizeof(hdr), new);

    while((bytes_read = fread(buffer, 1, buffer_size, old))) {
        fwrite(buffer, 1, bytes_read, new);
    }

    fclose(old);
    fclose(new);

    printf("Firmware written to %s\n", signed_file);

    return 0;
}  



int main(int argc, char *argv[])
{
    RSA *rsa_pri;
    RSA *rsa_pub;;
    unsigned char sha256_hash[65];
    unsigned char signature[1024];
    unsigned char digest[1024];
    struct firmware_header header;
    int len;
    
    if (argc != 3) {
        printf("Usage: %s private_key.pem firmware.bin\n", argv[0]);
        exit(1);
    }

    rsa_pri = rsa_read_private_key(argv[1]);
    if (rsa_pri == NULL) {
        printf("Cannot load private key.\n");
        exit(1);
    }

    rsa_pub = rsa_extract_public_key(rsa_pri);
    if (rsa_pub == NULL) {
        printf("Cannot extract public key.\n");
        exit(1);
    }

    if(sha256_compute(argv[2], sha256_hash) < 0) {
        printf("Cannot compute firmware hash.\n");
        exit(1);
    }

    //print_hex("hash: ", sha256_hash, SHA256_DIGEST_LENGTH);

    len = rsa_encrypt(sha256_hash, SHA256_DIGEST_LENGTH, signature, rsa_pri);
    if (len < 0) {
        printf("Cannot generate firmware signature.\n");
        exit(1);
    }

    //print_hex("signature: ", signature, len);

    len = rsa_decrypt(signature, len, digest, rsa_pub);
    if (len < 0) {
        printf("Failed to decrypt firmware signature.\n");
        exit(1);
    }
    
    //print_hex("digest: ", digest, len);

    if (memcmp(sha256_hash, digest, len) != 0) {
        printf("Failed to verify firmware signature.\n");
        exit(1);
    }

    memset(&header, 0, sizeof(struct firmware_header));
    header.header_version = 0x1;                  // Version number of this header
    header.flash_start_addr = FLASH_START;        // Address to program flash start
    header.user_app_addr = USER_APP_ADDR;         // Address to user application (bootloader will jump to it after valitation)
    header.firmware_start_addr = FIRMWARE_START;  // Firmware start address in flash
    header.firmware_length = FIRMWARE_LEN;        // Firmware length
    memcpy(header.hash, sha256_hash, 32);         // SHA256 hash computed for firmware image
    memcpy(header.signature, signature, 256);     // hash encrypted with RSA private key  

    print_header(header);

    write_header(argv[2], header);

    return 0;
}
