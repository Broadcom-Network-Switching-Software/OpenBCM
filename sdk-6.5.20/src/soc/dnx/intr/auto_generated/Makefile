#
# $Id: Makefile,v 1.4 2012/03/08 10:43:54 assafz Exp $
# This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
# 
# Copyright 2007-2020 Broadcom Inc. All rights reserved.
#
# Makefile for dnx/dnx_data/auto_generated subdirectory
#

LOCALDIR = src/soc/dnx/intr/auto_generated

include ${SDK}/make/Make.config
subdirs := 
ifdef BCM_88690_A0 
subdirs += jr2 
endif
ifdef BCM_88800_A0 
subdirs += j2c 
endif
ifdef BCM_88480_A0
subdirs += q2a
endif
ifdef BCM_88850_A0
subdirs += j2p
endif

#
# If this is compilation for SLK then optimize this library for code
# size. Use PARTIAL_OPT_CODE for doing only partial code size optimization
#
ifeq ($(findstring PARTIAL_OPT_CODE,$(MAKE_CUSTOM_SLK_CODE_SIZE)),PARTIAL_OPT_CODE)
CFGFLAGS += -Os
endif
ifeq ($(findstring OPTIMIZE_CODE,$(MAKE_CUSTOM_SLK_CODE_SIZE)),OPTIMIZE_CODE)
CFGFLAGS += -Os
endif

include ${SDK}/make/Make.subdirs

lib = libsoc_dnxintrag

include ${SDK}/make/Make.lib

include ${SDK}/make/Make.depend

