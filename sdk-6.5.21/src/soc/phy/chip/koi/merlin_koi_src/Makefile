#
# $Id: Makefile,v 1.1 2011/05/06 22:14:22 raphy Exp $
# This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
# 
# Copyright 2007-2020 Broadcom Inc. All rights reserved.
#
# Makefile for phy/chip/koi/merlin_koi_src subdirectory
#
LOCALDIR = src/soc/phy/chip/koi/merlin_koi_src

include ${SDK}/make/Make.config

CFGFLAGS+=  -UCOMPILER_HAS_DOUBLE

LSRCS    =\
            merlin_koi_functions.c merlin_koi_dependencies.c\

OBJS := $(LSRCS:.c=.o)

lib = libsoc_koi
include ${SDK}/make/Make.lib

include ${SDK}/make/Make.depend
