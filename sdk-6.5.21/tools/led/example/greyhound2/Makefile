# $Id: Makefile,v 1.41 2014/07/01 03:31:11 rihuang Exp $
# This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
# 
# Copyright 2007-2020 Broadcom Inc. All rights reserved.
#
# Compile sample LED driver programs
#
#
.KEEP_STATE:

.SUFFIXES:	.asm .hex

ASM = ../../tools/ledasm

SRCS = $(wildcard *.asm)
OBJS = $(patsubst %.asm,%.hex,$(SRCS))

all: $(OBJS)

remake: clobber all

%.hex: %.asm
	$(ASM) $*

clean:
	rm -f *.lst

clobber: clean
	rm -f *.hex
	rm -f *.c
