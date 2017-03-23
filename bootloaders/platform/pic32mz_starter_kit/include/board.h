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

// core timer ticks per second is always 1/2 the CPU ticks, so devide by 2 to get ticks per second
// divide by another 1000 (or 2* 1000) to get ticks per millisecond
// we add the 1000 to round up or down on a faction of a millisecond
#define CORE_TIMER_TICKS_PER_MILLISECOND    ((F_CPU + 1000) / 2000)


#endif /* _BOARD_H_ */

