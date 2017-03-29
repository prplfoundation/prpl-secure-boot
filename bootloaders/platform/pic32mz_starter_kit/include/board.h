#ifndef _BOARD_H_
#define _BOARD_H_

/* Red */
#define LED1_ENABLE()       (TRISHCLR = (1 << 0))
#define LED1_DISABLE()      (TRISHSET = (1 << 0))
#define LED1_TOGGLE()       (LATHINV  = (1 << 0))
#define LED1_ON()           (LATHSET  = (1 << 0))
#define LED1_OFF()          (LATHCLR  = (1 << 0))

/* Yellow */
#define LED2_ENABLE()       (TRISHCLR = (1 << 1))
#define LED2_DISABLE()      (TRISHSET = (1 << 1))
#define LED2_TOGGLE()       (LATHINV =  (1 << 1))
#define LED2_ON()           (LATHSET =  (1 << 1))
#define LED2_OFF()          (LATHCLR =  (1 << 1))

/* Green */
#define LED3_ENABLE()       (TRISHCLR = (1 << 2))
#define LED3_DISABLE()      (TRISHSET = (1 << 2))
#define LED3_TOGGLE()       (LATHINV =  (1 << 2))
#define LED3_ON()           (LATHSET =  (1 << 2))
#define LED3_OFF()          (LATHCLR =  (1 << 2))

#define FLASH_START             0x9D000000
#define USER_APP_ADDR           0x9D001000

#undef _CONFIG_VALID_
#include "config.h"

#ifndef _CONFIG_VALID_
    #error    Board/CPU combination not defined
#endif

#define board_halt()  asm("SDBBP 0");

#endif /* _BOARD_H_ */

