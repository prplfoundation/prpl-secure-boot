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

#include <bootloader.h>

#define UMODE_PDSEL_8NPAR 0x0000
#define UMODE_ON 0x8000
#define USTA_URXEN 0x00001000
#define USTA_UTXEN 0x00000400
#define USTA_UTXBF 0x00000200
#define BRG_BAUD(fr,bd) ((((fr)/8 + (bd)) / (bd) / 2) - 1)

/** 
 * @brief Configures UART2 as stdout and UART6 as alternative. 
 * @param baudrate_u2 UART2 baudrate.
 * @param baudrate_u6 UART6 baudrate.
 * @param sysclk System clock. 
 */
void init_uart(unsigned int baudrate_u2, unsigned int baudrate_u6, unsigned int sysclk){

    U2RXR = 1;  /* RPG6 to UART2 RX */
    RPB14R = 2; /* RPB14R to UART2 TX */

    U2STA = 0;
    U2BRG = ((int)( ((sysclk/2) / (16*baudrate_u2)) -1)) + 1;    
    U2MODE = UMODE_PDSEL_8NPAR |            /* 8-bit data, no parity */
        UMODE_ON;                       /* UART Enable */
        U2STASET = USTA_URXEN | USTA_UTXEN;     /* RX / TX Enable */
    
    U6RXR = 3; /* RPD0 to UART6 RX (pin 11) */
    RPF2R = 4; /* RPF2 to UART6 TX (pin 12)*/

    U6STA = 0;
    U6BRG = ((int)( ((sysclk/2) / (16*baudrate_u6)) -1)) + 1;    
    U6MODE = UMODE_PDSEL_8NPAR |            /* 8-bit data, no parity */
        UMODE_ON;                       /* UART Enable */
    U6STASET = USTA_URXEN | USTA_UTXEN;     /* RX / TX Enable */
}

/** 
 * @brief Write char to UART2. 
 * @param c Character to be writed. 
 */
void _putchar(char c){   
    while(U2STA&USTA_UTXBF);
    U2TXREG = c;   
}


int _puts(const char *str){
    while(*str){
        if(*str == '\n')
            _putchar('\r');
        _putchar(*str++);
    }
  return 0;
}