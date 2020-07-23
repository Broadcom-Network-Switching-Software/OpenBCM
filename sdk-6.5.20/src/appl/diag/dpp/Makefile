#
# This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
# 
# Copyright 2007-2020 Broadcom Inc. All rights reserved.
# $Id: Makefile,v 1.11 2013/01/14 12:24:25 assaf Exp $
LOCALDIR = src/appl/diag/dpp

include ${SDK}/make/Make.config

INCDIR += -I${SDK}/include/soc/dpp


CFLAGS += -DCINT_CONFIG_INCLUDE_SDK_SAL=1
CFLAGS += -DCINT_CONFIG_INCLUDE_STUBS=0

lib = libdiag_dpp


OBJS  = \
	l2.o \
	l3.o \
	vlan.o \
	utils_gen_err_ssf.o \
	txrx.o \
	dpp.o \
	fabric.o \
	stg.o \
	flexible_mode_kbp.o\
	test_oam.o \
	test_bfd.o

ifeq (KBP,$(findstring KBP,$(FEATURE_LIST)))
ifneq (,$(findstring KBP_12K, $(KBP_DEVICE)))
OBJS += kbp.o
endif
endif

ifeq (KBP,$(findstring KBP,$(FEATURE_LIST)))
ifeq (KBP_ALG,$(findstring KBP_ALG, $(KBP_DEVICE)))
OBJS += kbp.o
endif
endif

ifeq (KNET,$(findstring KNET,$(FEATURE_LIST)))
OBJS += knetctrl.o
endif

ifeq (DUNE_UI,$(findstring DUNE_UI,$(FEATURE_LIST)))
OBJS += utils_char_queue_ssf.o \
	utils_tid_task.o \
	utils_string.o 	\
	utils_d_os_ssf.o
endif

ifeq (gto-2_6, $(platform))
OBJS  += \
        utils_arad_card.o
endif

ifeq (wrx-3_7, $(platform))
OBJS  += \
        utils_arad_card.o
endif

ifeq (wrx-opennsa, $(platform))
OBJS  += \
        utils_arad_card.o
endif

ifeq (slk, $(platform))
OBJS  += \
        utils_arad_card.o
endif

ifeq (slk_be, $(platform))
OBJS  += \
        utils_arad_card.o
endif

ifeq (x86-64-fc28, $(platform))
OBJS  += \
        utils_arad_card.o
endif

BOBJS = $(addprefix ${BLDDIR}/, ${OBJS})

include ${SDK}/make/Make.subdirs
include ${SDK}/make/Make.lib
include ${SDK}/make/Make.depend
