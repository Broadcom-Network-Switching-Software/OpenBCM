# $Id: Makefile,v 1.23 Broadcom SDK $
# This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
# 
# Copyright 2007-2021 Broadcom Inc. All rights reserved.
#
# Makefile for SOC driver Library
#
include ${SDK}/make/Make.config

CFGFLAGS += -std=gnu99
#
# If this is compilation for SLK-ALL then optimize for code
# size. This is added to temporarily overcome the code size
# issue on SLK compiler.
#
ifeq ($(findstring OPTIMIZE_CODE,$(MAKE_CUSTOM_CODE_SIZE)),OPTIMIZE_CODE)
CFGFLAGS += -Os
endif

export CFGFLAGS

subdirs = cmn swstate dnxf_data


ifdef DNXF1_CHIPS
# {
subdirs+=ramon
# }
endif


include ${SDK}/make/Make.subdirs
