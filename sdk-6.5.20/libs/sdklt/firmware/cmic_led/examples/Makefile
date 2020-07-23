#
# This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
# 
# Copyright 2007-2020 Broadcom Inc. All rights reserved.
#
# CMIC LED firmware makefile.
#
# The default target is to generate the LED firmware binary files.
# All source code is under this folder.
# Type "make" to generate LED binary files.
# Type "make clean" to clean all LED binaries and compiler outputs.
#
Q := @

#
# Toolchain setup for compiling CMICd/CMICm LED code.
#
ASM  := ../tools/ledasm/ledasm
HEX2BIN := python ../tools/hex2bin.py
FWPACK := ../../tools/fwpack

#
# CMICx LED uses Broadcom XLDK ARM toolchain by default.
#
ifdef SDK
-include $(SDK)/INTERNAL/appl/make/toolchain/ns_arm_uclibc.mk
else
$(error SDK variable is not defined.)
endif

ifndef CROSS_COMPILE
CROSS_COMPILE = arm-linux-
endif

ifndef CC
CC = $(CROSS_COMPILE)gcc
endif

ifndef LD
LD = $(CROSS_COMPILE)ld
endif

ifndef OBJCOPY
OBJCOPY = $(CROSS_COMPILE)objcopy
endif

#
# CMICd/CMICm LED code and object files.
#
SRCS = $(wildcard *.asm)
OBJS = $(patsubst %.asm,%.fpkg,$(SRCS))
RMOBJS = $(patsubst %.asm,%.lst,$(SRCS))
RMOBJS += $(patsubst %.asm,%.hex,$(SRCS))
RMOBJS += $(patsubst %.asm,%.c,$(SRCS))
RMOBJS += $(patsubst %.asm,%.bin,$(SRCS))

#
# CMICx LED code and object file.
#

# CMICX supported chips default.
# Parsing source tree to know CMICX supported devices.
CMICX_DEVS_DEF ?= $(addprefix BCM, $(shell egrep -R "bcmbd_cmicx_led.h" $(SDK)/bcmbd/chip/ | \
                    awk '{print $$2} ' FS="$(SDK)/bcmbd/chip/bcm" | \
                    awk '{print $$1}' FS="_" ORS=" "))


CMICX_SRCS += cmicx_customer_led.c cmicx_customer_led.h cmicx_customer_led.lds
OBJS += cmicx_customer_led.fpkg

# Add removable objects.
RMOBJS += cmicx_customer_led.elf
RMOBJS += cmicx_customer_led.o
RMOBJS += cmicx_customer_led.map
RMOBJS += cmicx_customer_led.bin
RMOBJS += $(OBJS)

# Default target.
all: $(ASM) $(OBJS)

$(ASM):
	$(Q)make -C ../tools/ledasm

# Create a cmicx_customer_led.yml, if there is no existing one.
cmicx_customer_led.yml:
	$(Q)echo "Generating default $@ ...."
	$(Q)cat $@.template | sed -e "s/- <devid>//" > .$@.tmp1
	$(Q)for id in $(CMICX_DEVS_DEF) ; do \
		cat .$@.tmp1 | \
		sed -e "s/supported-chips:/supported-chips:\n                        - $$id/" > \
		.$@.tmp2; \
		mv .$@.tmp2 .$@.tmp1; \
	done
	$(Q)cat .$@.tmp1 | sed -e "s/\s\+$$//" > $@;
	$(Q)rm .$@.tmp1;

cmicx_customer_led.bin: $(CMICX_SRCS)
	$(Q)echo "Compiling  $< ...."
ifeq ($(findstring arm,$(CC)),)
	$(Q)echo "GNU ARM Embedded toolchain is not ready."
	$(Q)echo "Please contact Broadcom for XLDK ARM toolchain."
else
	$(Q)$(CC) -O1 -g -mthumb -mcpu=cortex-m0 -c  -o cmicx_customer_led.o $<
	$(Q)$(LD) -T cmicx_customer_led.lds -Bstatic -nostdlib --entry 0 -EL \
            -Map cmicx_customer_led.map \
            -o cmicx_customer_led.elf cmicx_customer_led.o
	$(Q)$(OBJCOPY) -O binary cmicx_customer_led.elf $@
endif

%.bin: %.asm
	$(Q)echo "Compiling $< ...."
	$(Q)$(ASM) $*
	$(Q)$(HEX2BIN) $*.hex $@

%.fpkg: %.bin %.yml
	$(Q)echo "Packaging $@ ...."
	$(Q)$(FWPACK) $*.yml $*.bin $@

clean:
	$(Q)rm -f $(RMOBJS)

distclean:
	$(Q)rm -f cmicx_customer_led.yml
	$(Q)make clean
