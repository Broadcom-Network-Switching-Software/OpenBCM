#
# This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
# 
# Copyright 2007-2020 Broadcom Inc. All rights reserved.
#

LOCALDIR = src/bcm/ltsw/dll/main

include ${SDK}/make/Make.config

CFGFLAGS += -fPIC

lib = libltsw_dllmain

VPATH = $(SDKLT)/bcmissu/dll/main
VSRCS = $(wildcard $(VPATH)/*.c)

LSRCS = $(notdir $(VSRCS))
LOBJS = $(addsuffix .o, $(basename $(LSRCS)))
BOBJS = $(addprefix $(BLDDIR)/,$(LOBJS))

include ${SDK}/make/Make.lib
include ${SDK}/make/Make.depend
