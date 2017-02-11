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

#ifndef __HEADER__
#define __HEADER__

#define FLASH_START             0x9D000000
#define USER_APP_ADDR           0x9D001000
#define FIRMWARE_START          0x9D0001f0
#define FIRMWARE_LEN            (0x8000-0x1f0)

#pragma pack(push,2)
struct firmware_header {
    unsigned char header_version;       // Version number of this header
    unsigned int flash_start_addr;      // Address to program flash start
    unsigned int user_app_addr;         // Address to user application (bootloader will jump to it after valitation)
    unsigned int firmware_start_addr;   // Firmware start address in flash
    unsigned int firmware_length;       // Firmware length
    unsigned char hash[32];             // SHA256 hash computed for firmware image
    unsigned char signature[256];       // hash encrypted with RSA private key  
};
#pragma pack(pop)

#endif /* __HEADER__ */
