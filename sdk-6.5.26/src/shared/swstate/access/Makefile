#
# $Id:$
# This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
# 
# Copyright 2007-2022 Broadcom Inc. All rights reserved.
# 
# Makefile for SOC swstate access subdirectory
#
LOCALDIR = src/shared/swstate/access

# enable optimization & disable debugging for this lib
DEBUG_OPTIMIZE = TRUE
ifneq ($(DEBUG_SYMBOLS),TRUE)
# {
DEBUG_CFLAGS += -g0
# }
endif
include ${SDK}/make/Make.config

lib = libshared_swstate_access

include ${SDK}/make/Make.lib
include ${SDK}/make/Make.depend
