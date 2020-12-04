# $Id: Makefile,v 1.17 2011/09/29 21:30:10 gururaj Exp $
# This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
# 
# Copyright 2007-2020 Broadcom Inc. All rights reserved.
#
# Makefile for BCM5 driver Library
#

LOCALDIR = src/bcm

CFGFLAGS += -DSOURCE_CHECK_LIBBCM

include ${SDK}/make/Make.config

subdirs = common
ifneq (,$(findstring -DBCM_TOMAHAWK3_SUPPORT,$(CFGFLAGS)))
subdirs+=tomahawk3
endif
$(error, $subdirs)
ifneq (,$(findstring -DBCM_ESW_SUPPORT,$(CFGFLAGS)))
subdirs+=esw
endif
ifneq (,$(findstring -DBCM_LTSW_SUPPORT,$(CFGFLAGS)))
subdirs+=ltsw
endif
ifneq (,$(findstring -DBCM_SHADOW_SUPPORT,$(CFGFLAGS)))
subdirs+=shadow
endif
ifneq (,$(findstring -DBCM_RPC_SUPPORT,$(CFGFLAGS)))
subdirs+=rpc
subdirs+=compat
endif
ifneq (,$(findstring -DBCM_PETRA_SUPPORT,$(CFGFLAGS)))
subdirs+=dpp
endif
ifneq (,$(findstring -DBCM_DFE_SUPPORT,$(CFGFLAGS)))
subdirs+=dfe
endif
ifneq (,$(findstring -DBCM_DNX_SUPPORT,$(CFGFLAGS)))
subdirs+=dnx
endif
ifneq (,$(findstring -DBCM_DNXF_SUPPORT,$(CFGFLAGS)))
subdirs+=dnxf
endif

include ${SDK}/make/Make.subdirs

lib = libbcm
include ${SDK}/make/Make.lib

include ${SDK}/make/Make.depend

