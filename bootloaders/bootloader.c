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

/* Include parameters for the configuration memory area */
#define CONFIGURATION_MEMORY

#include <bootloader.h>
#include <sal_crypto.h>
#include <rootkey.h>

static FNUSERAPP firmware_start = (FNUSERAPP) USER_APP_ADDR;

void udelay(uint32_t usec)
{
    uint32_t now = _CP0_GET_COUNT();
    uint32_t final = now + usec * (F_CPU / 1000000) / 2;

    for (;;) {
        now = _CP0_GET_COUNT();
        if ((int32_t) (now - final) >= 0) break;
    }
}

int main()
{
    /* Board initialization */
    board_init();

    INFO("===========================================================\n");
    INFO("prplSecureBoot %s [%s, %s]\n", "v0.1.0", __DATE__, __TIME__);
    INFO("Copyright (c) 2017, prpl Foundation\n");
    INFO("===========================================================\n");

    /* Verify firmware */
    if (firmware_verify() == 0) {
        /* Everithing is good, just jump to user app */
        firmware_start();
    }

    /* No valid firmware image was found, stop! */
    board_halt();

    return 0;
}


void print_hex(char *prefix, unsigned char buffer[], int len)
{
    int i = 0;

    INFO("%s", prefix);

    for(i = 0; i < len; i++)
    {
        INFO("%02x", buffer[i]);
    }

    INFO(" (%d)\n", len);
}

void firmware_print_header(firmware_header_t *header)
{
    INFO("prplSecureBoot Firmware Header:\n");
    INFO("Header version:          0x%02x\n", header->header_version);
    INFO("Flash start address:     0x%02x\n", header->flash_start_addr);
    INFO("Execution address:       0x%02x\n", header->user_app_addr);
    INFO("Firmware start address:  0x%02x\n", header->firmware_start_addr);
    INFO("Firmware length:         0x%02x (%d)\n", header->firmware_length, header->firmware_length);
    print_hex("Firmware hash:           ", header->hash, 32);
    print_hex("Firmware signature:      ", header->signature, 256);
    //printf("Header size: %d\n", (int)sizeof(hdr));
}

firmware_header_t * firmware_read_header(uint32 base_addr)
{
    firmware_header_t *header;
    uint32 addr = base_addr + offsetHeaderInfo;

    if (check_has_firmware == 0) {
        // Firmware not found in flash
        return NULL;
    }

    header = (firmware_header_t *) addr;
    if(header->header_version == 0x01 && header->flash_start_addr == FLASH_START) {
        // Firmware header found!
        return header;
    }

    return NULL;
}

int compute_hash(firmware_header_t *header, unsigned char *hash)
{
    int i;
    sha256_t sha256;
    unsigned int addr = FLASH_START;
    unsigned int end = FLASH_START + 0x8000;

    unsigned int data[4];

    sha256_init(&sha256);

    /* read from 0x00 to 0x0F */
    sha256_update(&sha256, (unsigned char*)addr, (unsigned long)0x10);

    /* skip firmware header area */
    addr += 0x200;

    while (addr < end) {
        sha256_update(&sha256, (unsigned char*)addr, (unsigned long)0x10);
        addr += 0x10;
    }

    sha256_final(&sha256, hash);

    for (i = 0; i < 32; i++) {
        if (hash[i] != header->hash[i]) {
            return 1;
        }
    }

    return 0;
}

int verify_signature(firmware_header_t *header, unsigned char *hash)
{
    int i, len;
    rsa_t *rsa = NULL;
    unsigned char digest[1024];

    rsa_pub_key_new(&rsa, rsa_modulus, rsa_mod_len, rsa_expo, rsa_expo_len);

    len = rsa_decrypt(rsa, header->signature, digest, 1024);
    if (len < 32) {
        return -1;
    }

    for (i = 0; i < 32; i++) {
        if (hash[i] != digest[i]) {
            return 1;
        }
    }

    return 0;
}

int firmware_verify(void)
{
    int is_secure = 0;
    firmware_header_t *header;

    LED1_OFF();
    LED2_OFF();
    LED3_OFF();

    header = firmware_read_header(FLASH_START);
    if (header != NULL) {
        unsigned char hash[32];
        firmware_print_header(header);
        INFO("Verifying hash integrity... ");
        if(compute_hash(header, hash) == 0) {
            INFO("OK.\n");
            INFO("Verifying signature authenticity... ");
            if(verify_signature(header, hash) == 0) {
                // firmware image was successfully verified
                INFO("OK.\n");
                is_secure = 1;
            } else {
                INFO("NOK.\n");
                // decrypted hash doesn't match with the computed one
                LED1_ON(); // RED ON
            }
        } else {
            INFO("NOK.\n");
            // computed hash doesn't match
            LED2_ON(); // YELLOW ON
        }
    } else {
        INFO("No firmware image found.\n");
    }

    board_cleanup();

    if (is_secure == 0) {
        INFO("Invalid firmware.\n");
        udelay(1000);
        return 1;
    }

    INFO("Starting firmware...\n");
    udelay(1000);

    return 0;
}
