# $Id: Makefile,v 1.3 2007/03/06 20:58:46 sanjayg Exp $
# This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
# 
# Copyright 2007-2020 Broadcom Inc. All rights reserved.
#
# Makefile for eth driver
#

LOCALDIR = systems/drv/eth

include ${SDK}/make/Make.config

ifeq (${targetbase},posix)
CFLAGS += -DLINUX
endif

ifeq (${targetbase},unix)
CFLAGS += -DLINUX
endif

ifeq (${targetbase},linux)
CFLAGS += -DLINUX -DKERNEL
endif

ifeq (${targetbase},vxworks)
CFLAGS += -DVXWORKS
endif

lib = libeth
include ${SDK}/make/Make.lib

include ${SDK}/make/Make.depend
