#
# $Id$
# This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
# 
# Copyright 2007-2022 Broadcom Inc. All rights reserved.
#
# Makefile for dpp subdirectory
#

LOCALDIR = src/soc/dnxc

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

subdirs = dnxc_data swstate

ifdef USE_NEW_ACCESS
subdirs += access
endif

include ${SDK}/make/Make.subdirs

lib = libsoc_dnxc

include ${SDK}/make/Make.lib

include ${SDK}/make/Make.depend
