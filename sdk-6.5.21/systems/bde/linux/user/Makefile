# -*- Makefile -*-
# $Id: Makefile,v 1.4 2004/03/05 21:26:08 mlarsen Exp $
# This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
# 
# Copyright 2007-2020 Broadcom Inc. All rights reserved.
#
# Makefile for Linux user BDE
#
LOCALDIR = systems/bde/linux/user

CFLAGS += -I../include -I${SDK}/systems/linux/kernel/modules/include

include ${SDK}/make/Make.config

# need to add vpath sources
VPATH = ../shared

# Add the srcs to be found by vpath
LSRCS += mpool.c
LHDRS += mpool.h
LOBJS = $(addsuffix .o, $(basename ${LSRCS}))
BOBJS = $(addprefix ${BLDDIR}/,${LOBJS})

lib = liblubde

include ${SDK}/make/Make.lib

include ${SDK}/make/Make.depend
