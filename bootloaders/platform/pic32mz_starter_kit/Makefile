#Microchip 32MZ starter kit with Secure Boot.

BINPREFIX=xc32-

CC=${BINPREFIX}gcc
CXX=${BINPREFIX}g++
LD=${BINPREFIX}ld
AS=${BINPREFIX}as
BIN2HEX=${BINPREFIX}bin2hex

TOPDIR=../..

NAME=Microchip_Eth_Starter_Kit_prplSecureBoot
MCU=32MZ2048EFM144
CRT=$(TOPDIR)/arch/mips/pic32mz/boot.S
BOARD_CFLAGS=-DPIC32MZ -DPIC32_SRS_SET_COUNT=8 \
                                     -I$(TOPDIR) -I$(TOPDIR)/arch/mips/pic32mz/include -I$(TOPDIR)/platform/pic32mz_starter_kit/include/ -I$(TOPDIR)/lib \
                                     -I$(TOPDIR)/lib/libsal/ -I$(TOPDIR)/lib/axtls-lite/include/

OBJS = $(TOPDIR)/bootloader.o \
       $(TOPDIR)/platform/pic32mz_starter_kit/board/board.o \
       $(TOPDIR)/platform/pic32mz_starter_kit/board/uart.o \
       $(TOPDIR)/lib/libsal/sal_rsa.o \
       $(TOPDIR)/lib/libsal/sal_sha256.o \
       $(TOPDIR)/lib/axtls-lite/src/bigint.o \
       $(TOPDIR)/lib/axtls-lite/src/sha256.o \
       $(TOPDIR)/lib/axtls-lite/src/rsa.o \
       $(TOPDIR)/lib/axtls-lite/src/malloc.o \
       $(TOPDIR)/lib/axtls-lite/src/pic32mz-libc.o

CFLAGS=-mprocessor=${MCU} -DBOARD_MICROCHIP_ETH_START_KIT -DSECURE_BOOT -DAXTLS_LITE -DBARE_METAL ${BOARD_CFLAGS} -mno-float
ASFLAGS=-Wa,--gdwarf-2 -mprocessor=${MCU} ${BOARD_CFLAGS} -mno-float
LDFLAGS=-mprocessor=${MCU} -nostdlib -nostartfiles ${BOARD_CFLAGS} -mno-float -T $(TOPDIR)/arch/mips/pic32mz/pic32mz.ld -Wl,--gc-sections -ffunction-sections -fdata-sections

all:
	${MAKE} compile

compile: ${NAME}.hex

${NAME}.hex: ${NAME}.elf
	${BIN2HEX} -a $?

${NAME}.elf: ${OBJS}
	${CC} ${LDFLAGS} -o $@ ${CRT} $?

clean:
	rm -f *.elf *.hex
	find $(TOPDIR) -type f -name '*.o' -delete

