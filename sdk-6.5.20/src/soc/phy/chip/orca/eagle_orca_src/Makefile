#
# $Id: Makefile,v 1.1 2016/08/26 22:14:22 raphy Exp $
# This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
# 
# Copyright 2007-2020 Broadcom Inc. All rights reserved.
#
# Makefile for phy/chip/orca/eagle_orca_src subdirectory
#
LOCALDIR = src/soc/phy/chip/orca/eagle_orca_src

include ${SDK}/make/Make.config

CFGFLAGS+=  -UCOMPILER_HAS_DOUBLE

LSRCS    =\
            eagle_phy_1_functions.c eagle_phy_1_dependencies.c\

OBJS := $(LSRCS:.c=.o)

lib = libsoc_orca
include ${SDK}/make/Make.lib

include ${SDK}/make/Make.depend
