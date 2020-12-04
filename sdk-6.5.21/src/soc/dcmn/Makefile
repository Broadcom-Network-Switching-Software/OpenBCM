#
# $Id$
# This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
# 
# Copyright 2007-2020 Broadcom Inc. All rights reserved.
#
# Makefile for dpp subdirectory
#

LOCALDIR = src/soc/dcmn

include ${SDK}/make/Make.config
#
# If this is compilation for SLK-ALL then optimize for code
# size. This is added to temporarily overcome the code size
# issue on SLK compiler.
#
ifeq ($(findstring OPTIMIZE_CODE,$(MAKE_CUSTOM_SLK_CODE_SIZE)),OPTIMIZE_CODE)
CFGFLAGS += -Os
endif

export CFGFLAGS

include ${SDK}/make/Make.subdirs

lib = libsoc_dcmn

include ${SDK}/make/Make.lib

include ${SDK}/make/Make.depend

