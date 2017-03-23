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

#include <pic32mz.h>
#include <board.h>


void board_led_init()
{
    LED1_ENABLE();
    LED2_ENABLE();
    LED3_ENABLE();
}

void board_init()
{
    /* Init board leds */
    board_led_init();

    /* Turn on all led for 1 second at the startup */
    LED1_TOGGLE();
    LED2_TOGGLE();
    LED3_TOGGLE();
    udelay(1000000);
    LED1_TOGGLE();
    LED2_TOGGLE();
    LED3_TOGGLE();
    udelay(1000000);

    /* Configure UART for debug messages */
    init_uart(115200, 9600, F_CPU);
}

void board_cleanup()
{
    LED1_OFF();
    LED2_OFF();
    LED3_OFF();

    LED1_DISABLE();
    LED2_DISABLE();
    LED3_DISABLE();
}
