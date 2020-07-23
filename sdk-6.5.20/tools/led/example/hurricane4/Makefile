#
# This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
# 
# Copyright 2007-2020 Broadcom Inc. All rights reserved.
#
#
# Copyright: (c) 2012 Broadcom Corp.
# All Rights Reserved.
#
ifeq (${SDK},)
# {
$(error environment variable SDK not set)
# }
endif

Q := @
ARM_TOOLCHAINS = arm9uClibctoolchain
-include $(SDK)/make/Makefile.linux-northstar-2_6

BIN2HEX = ${SDK}/tools/bin2hex.pl
INCDIR = -I${SDK}/include
LDSCRIPT = $(SDK)/tools/led/cmicx/custom_led.lds
SRCS := $(wildcard *.c)
HEXOBJS := $(patsubst %.c,%.hex,$(SRCS))

all: clean $(HEXOBJS)

%.hex: %.c
	$Qecho "Compile " $<
	$Qarm-linux-gcc -O1 -g ${INCDIR} -mthumb -mcpu=cortex-m0 -c -o $(patsubst %.c,%.o,$<) $<
	$Qarm-linux-ld -T $(LDSCRIPT) -Bstatic -nostdlib --entry 0 -EL -Map $(patsubst %.c,%.map,$<) -o $(patsubst %.c,%.elf,$<) $(patsubst %.c,%.o,$<)
	$Qarm-linux-objcopy -O binary $(patsubst %.c,%.elf,$<) $(patsubst %.c,%.bin,$<)
	$Q$(BIN2HEX) $(patsubst %.c,%.bin,$<) $@
	$Qsed -i 's/0x//g' $@
	$Qsed -i 's/,//g' $@
	$Qsed -i 's/^[\ ]\+//g' $@


clean:
	$Qrm -rf *.elf *.o *.map *.bin *.hex

