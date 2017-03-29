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

#ifndef _BOOTLOADER_H_
#define _BOOTLOADER_H_

#include <p32xxxx.h>

typedef char int8;
typedef unsigned char uint8;
typedef short int16;
typedef unsigned short uint16;
typedef int int32;
typedef unsigned int uint32;
typedef int intptr;
typedef unsigned int uintptr;

typedef unsigned char bool;
typedef unsigned char byte;
typedef unsigned int uint;

#include "board.h"

#pragma pack(push,2)
typedef struct {
    unsigned char header_version;       // Version number of this header
    unsigned int flash_start_addr;      // Address to program flash start
    unsigned int user_app_addr;         // Address to user application (bootloader will jump to it after valitation)
    unsigned int firmware_start_addr;   // Firmware start address in flash
    unsigned int firmware_length;       // Firmware length
    unsigned char hash[32];             // SHA256 hash computed for firmware image
    unsigned char signature[256];       // hash encrypted with RSA private key
} firmware_header_t;
#pragma pack(pop)

firmware_header_t * firmware_read_header(uint32 base_addr);

#if 1
#define INFO(X,...) _printf(X, ##__VA_ARGS__)
#else
#define INFO(X,...) do{}while(0)
#endif

/* Part of this code was originally written by Rich Testardi (rich@testardi.com);
 * please preserve this reference. */
typedef void __attribute__((far, noreturn)) (* FNUSERAPP)(void);
#define check_has_firmware ((*((unsigned long *) USER_APP_ADDR)) != ((unsigned long) -1))
extern uint32 _skip_ram_space_addr;
extern uint32 _skip_ram_space_end_adder;
extern uint32 _RAM_SKIP_SIZE;

#define offsetBaseAddrInfo  (0x008ul)
#define offsetHeaderInfo    (0x010ul)

#endif  /* _BOOTLOADER_H_ */
