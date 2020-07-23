#
# $Id: Makefile,v 1.1 2011/05/06 22:14:22 raphy Exp $
# This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
# 
# Copyright 2007-2020 Broadcom Inc. All rights reserved.
#
# Makefile for phy/fcmap/src subdirectory
#
LOCALDIR = src/soc/phy/fcmap/src

include ${SDK}/make/Make.config

CFGFLAGS += -DBCM84756_A0
CFGFLAGS += -DBCM84756_C0
CFGFLAGS += -DBCM88060_A0
CFGFLAGS += -DBCM88061_A0

LSRCS    =\
           bfcmap_io.c bfcmap.c \
           bfcmap_ident.c bfcmap_stat.c bfcmap_vlan.c \
           bfcmap84756_a0.c bfcmap84756_a0_fw.c \
           bfcmap88060_a0.c

ifndef BUILD_MACSEC
LSRCS  += io_mmi.c io_mmi1.c mmi_cmn.c
LSRCS  += bcm84756_ucode.c
endif

lib = libsoc_fcmap
include ${SDK}/make/Make.lib

include ${SDK}/make/Make.depend
