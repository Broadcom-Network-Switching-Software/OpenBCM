# $Id: Makefile,v 1.41 2013/06/14 03:31:11 honi Exp $
# This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
# 
# Copyright 2007-2020 Broadcom Inc. All rights reserved.
#
# Compile sample LED driver programs.
#
# Notes:
#	The Merlin runs the same programs as White Knight (wk5670, wk5690)
#
.KEEP_STATE:

.SUFFIXES:	.asm .hex

ASM = ../tools/ledasm

OBJS = \
	ex1.hex \
	ex2.hex \
	ex3.hex \
	ex4.hex \
	ex5.hex \
	testasm.hex \
	sdk5605.hex \
	pong5605.hex \
	pong-wk.hex \
	sdk5670.hex \
	sdk5690.hex \
	p48.hex \
	knigget.hex \
	wk5670.hex \
	wk5690.hex \
	merlin5670.hex \
	merlin5690.hex \
	lm5690.hex \
	galahad.hex \
	lancelot.hex \
	k12-5690.hex \
	twolynx.hex \
	herculynx.hex \
	mirror.hex \
	sdk5665.hex \
	sdk5650.hex \
	guenevere5670.hex \
	guenevere5673.hex \
	sdk5675.hex \
	tuc24_ref.hex \
	lm48p5695.hex \
	lm48p5695_10.hex \
	sdk56504.hex \
	sdk56504ref.hex \
	lm48p56504.hex \
	bcm953300p24ref.hex \
	bcm956314p24ref.hex \
	lm48p56504_50.hex \
	poe48p56504.hex \
	sdk56102.hex \
	felix48.hex \
	lm12pcx456501.hex \
	sdk56601.hex \
	sdk56602.hex \
	sdk56800.hex \
	sdk56800c.hex \
	sdk56580.hex \
	lm12p56802.hex \
	sdk56018.hex \
	sdk56214.hex \
	sdk56680.hex \
	sdk56685.hex \
	sdk56820.hex \
	sdk56524.hex \
	sdk56624.hex \
	sdk56626.hex \
	sdk56628.hex \
	sdk56629.hex \
	sdk56634.hex \
	sdk56636.hex \
	sdk56638.hex \
	sdk56639.hex \
	bcm956820r24xg.hex \
	bcm956024p48ref.hex \
	bcm956024r50t.hex \
	generic8led.hex \
	sdk56840.hex \
	sdk56643ge.hex \
	sdk56640xe.hex \
	sdk56640hsp.hex \
	sdk56648hsp.hex \
	magnum.hex \
	sdk88650.hex \
	sdk88670.hex \
	sdk88470.hex \
	sdk88270.hex \
	sdk88680.hex \
	sdk56960.hex \
	sdk56270.hex \
	sdk56560.hex \
	sdk56960_mt2.hex \
	sdk8806x.hex \
        bcm953547k.hex\
	sdk56670.hex 
  

all: $(OBJS)

remake: clobber all

%.hex: %.asm
	$(ASM) $*

clean:
	rm -f *.lst

clobber: clean
	rm -f *.hex
	rm -f *.c
