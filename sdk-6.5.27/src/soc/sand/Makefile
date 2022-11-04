#
# $Id$
# This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
# 
# Copyright 2007-2022 Broadcom Inc. All rights reserved.
#
# Makefile for dfe subdirectory
#

LOCALDIR = src/soc/sand

include ${SDK}/make/Make.config

subdirs = auto_generated shrextend

include ${SDK}/make/Make.subdirs

#macros used in this library need C99 support
CFGFLAGS += -std=gnu99 -D_GNU_SOURCE

lib = libsoc_sand

OBJS  = \
	ddrc16_regs.o \
	ddrc16.o \
	shmoo_combo16.o \
	ha_shmem.o \
	sand_access_init.o \
	sand_aux_access.o \
	sand_intr_corr_act_func.o \
	sand_cpu2tap.o \
	sand_mem.o \
	sand_pparse.o \
	sand_ser_correction.o \
	sand_signals.o \
    dbx_pre_compiled_devices.o

ifneq (,$(findstring -DNO_FILEIO,$(CFGFLAGS)))
OBJS  += \
	sand_signals_static.o
else
ifneq (,$(findstring -DSTATIC_SIGNAL_TEST,$(CFGFLAGS)))
OBJS  += \
	sand_signals_static.o
else
OBJS  += \
	sand_signals_xml.o
endif
endif

ifneq (,$(findstring -DADAPTER_SERVER_MODE,$(CFGFLAGS)))
OBJS  += \
	sand_signals_adapter.o
endif

BOBJS = $(addprefix ${BLDDIR}/, ${OBJS})

include ${SDK}/make/Make.lib
include ${SDK}/make/Make.depend
