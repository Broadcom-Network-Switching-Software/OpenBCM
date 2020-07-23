#
# $Id:$
# This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
# 
# Copyright 2007-2020 Broadcom Inc. All rights reserved.
# 
# Makefile for SOC swstate subdirectory
#
LOCALDIR = src/soc/dnx/swstate/auto_generated

include ${SDK}/make/Make.config
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

lib = libsoc_dnxswstate_auto_generated

include ${SDK}/make/Make.lib
include ${SDK}/make/Make.depend
