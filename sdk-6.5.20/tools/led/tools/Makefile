# $Id: Makefile,v 1.1 2004/02/24 07:46:21 csm Exp $
# This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
# 
# Copyright 2007-2020 Broadcom Inc. All rights reserved.
#
# Build LED processor tools
#
.KEEP_STATE:

CC = gcc
RM = rm -f

CFLAGS = -g -Wall -I.

LED_TOOLS      = ledasm leddasm ledsim

all: $(LED_TOOLS)

remake: clobber all

ledasm: ledasm.c ledasm.h
	$(CC) $(CFLAGS) -o ledasm ledasm.c

leddasm: leddasm.c ledasm.h leddasmcore.o
	$(CC) $(CFLAGS) -o leddasm leddasmcore.o leddasm.c

ledsim: ledsim.c ledasm.h leddasmcore.o
	$(CC) $(CFLAGS) -o ledsim leddasmcore.o ledsim.c

leddasmcore.o: leddasmcore.c ledasm.h
	$(CC) $(CFLAGS) -c leddasmcore.c

clean:
	$(RM) *.o

clobber: clean
	$(RM) $(LED_TOOLS)
