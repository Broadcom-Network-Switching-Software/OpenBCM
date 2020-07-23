#
#
# This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
# 
# Copyright 2007-2020 Broadcom Inc. All rights reserved.
#

LOCALDIR = src/shared/utilex

include ${SDK}/make/Make.config

INCDIR += -I${SDK}/include/shared

lib = libshared_utilex
#
# Extend C language standard to C99 (plus some GNU goodies)
#
CFGFLAGS += -std=gnu99

include ${SDK}/make/Make.lib
include ${SDK}/make/Make.depend