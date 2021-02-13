# $Id: Makefile,v 1.2 Broadcom SDK $
# This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
# 
# Copyright 2007-2021 Broadcom Inc. All rights reserved.
#
# Makefile for BCM driver Library
#

LOCALDIR = src/bcm/dpp

include ${SDK}/make/Make.config

# Extend C language standard to C99 (plus some GNU goodies)
CFGFLAGS += -std=gnu99
#
# If this is compilation for SLK-ALL then optimize for code
# size. This is added to temporarily overcome the code size
# issue on SLK compiler.
#
ifeq ($(findstring OPTIMIZE_CODE,$(MAKE_CUSTOM_SLK_CODE_SIZE)),OPTIMIZE_CODE)
CFGFLAGS += -Os
endif

export CFGFLAGS

ifneq (,$(findstring -DINCLUDE_PTP,$(CFGFLAGS)))
subdirs += ptp
endif

include ${SDK}/make/Make.subdirs

lib = libbcm_dpp

include ${SDK}/make/Make.lib

include ${SDK}/make/Make.depend
