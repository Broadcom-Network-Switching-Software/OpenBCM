#
# This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
# 
# Copyright 2007-2020 Broadcom Inc. All rights reserved.
# $Id: Makefile,v 1.11 2013/01/14 12:24:25 assaf Exp $
LOCALDIR = src/appl/diag/dnx

include ${SDK}/make/Make.config

#macros used in this library need C99 support
CFGFLAGS += -std=gnu99 -D_GNU_SOURCE

ifndef ESW_CHIPS
CFGFLAGS += -D_SOC_MEMACC_H -D_SOC_MEMREGS_H -D_SOC_ALLFIELDS_H -D_SOC_INTENUM_H -D_SOC_DRV_H
export CFGFLAGS
endif

OBJS  = \
	diag_dnx_cmdlist.o \
	diag_dnx_visibility.o \
	diag_dnx_tdm.o \
	diag_dnx_legacy.o

ifneq (,$(findstring -DADAPTER_SERVER_MODE,$(CFGFLAGS)))
subdirs+= adapter
endif
subdirs+= allocation_mngr
subdirs+= arr
subdirs+= crps
subdirs+= dbal
subdirs+= diag
subdirs+= dram
subdirs+= error_recovery
subdirs+= export
subdirs+= field
subdirs+= gpm
subdirs+= reset
subdirs+= l2
subdirs+= vlan
subdirs+= l3
subdirs+= mdb
subdirs+= meter
subdirs+= oam
subdirs+= port
subdirs+= stg
subdirs+= stif
subdirs+= tm
subdirs+= trap
subdirs+= tx
subdirs+= nif
subdirs+= swstate
subdirs+= fabric
subdirs+= kbp
subdirs+= pp
subdirs+= time_analyzer
subdirs+= knet
subdirs+= srv6
subdirs+= lif
subdirs+= sec
ifeq (FLEXE,$(findstring FLEXE,$(FEATURE_LIST)))
subdirs+= flexe
endif
subdirs+= utils

lib = libdiag_dnx

include ${SDK}/make/Make.subdirs
include ${SDK}/make/Make.lib
include ${SDK}/make/Make.depend
