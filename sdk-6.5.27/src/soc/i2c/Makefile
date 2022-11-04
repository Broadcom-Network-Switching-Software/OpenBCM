#
#   $Id: Makefile,v 1.2.648.1 Broadcom SDK $
# 
# 
# This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
# 
# Copyright 2007-2022 Broadcom Inc. All rights reserved.
#
# Makefile for i2c subdirectory
#
CFLAGS += -g
LOCALDIR = src/soc/i2c

include ${SDK}/make/Make.config

lib = libsoc_i2c
ifeq (,$(findstring -DBCM_ESW_SUPPORT,$(CFGFLAGS)))
ifeq (,$(findstring -DBCM_PETRA_SUPPORT,$(CFGFLAGS)))
OBJS = cpu.o
BOBJS = $(addprefix ${BLDDIR}/, ${OBJS})
endif
endif

include ${SDK}/make/Make.lib

include ${SDK}/make/Make.depend
