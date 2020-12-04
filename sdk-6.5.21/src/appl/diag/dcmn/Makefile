#
# This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
# 
# Copyright 2007-2020 Broadcom Inc. All rights reserved.
# $Id: Makefile,v 1.10 2013/08/01 14:52:57 msteinbe Exp $
LOCALDIR = src/appl/diag/dcmn

include ${SDK}/make/Make.config

INCDIR += -I${SDK}/include/shared

lib = libdiag_dcmn

CFGFLAGS += -std=gnu99 -D_GNU_SOURCE

OBJS  = \
	utils_board_general.o \
	cmdlist.o \
	counter.o \
	port.o \
	mem.o \
	reg.o \
	soc.o \
	srcell.o \
	init.o \
	counters_data.o \
	rate_calc.o \
	diag_egq.o \
	diag.o \
	diag_oam.o \
	ddr.o \
	gport.o \
	dcmn_patch_database.o \
	fabric.o \
	cosq.o \
	init_deinit.o \
	diag_buffers.o \
	diag_mmu.o \
	rx.o \
	diag_signals_dump.o \
	diag_fc.o

ifdef DIAG_IPSEC
OBJS  += \
	diag_ipsec.o
endif
ifdef DPP_CHIPS
OBJS += \
	diag_alloc.o \
	diag_field.o \
	diag_pp.o \
	credit_request_thresholds.o \
	diag_lb.o
ifeq (,$(findstring -DNO_FILEIO,$(CFGFLAGS)))
OBJS += \
	diag_pp_data.o \
	export.o
endif
ifdef BCM_88675_A0
OBJS += \
	parser.o
endif
endif


ifeq (gto-2_6, $(platform))
OBJS  += bsp_cards_consts.o
endif

BOBJS = $(addprefix ${BLDDIR}/, ${OBJS})

include ${SDK}/make/Make.subdirs
include ${SDK}/make/Make.lib
include ${SDK}/make/Make.depend
