#
# This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
# 
# Copyright 2007-2020 Broadcom Inc. All rights reserved.
#

LOCALDIR = src/bcm/ltsw/dll/db

include ${SDK}/make/Make.config

CFGFLAGS += -fPIC

lib = libltsw_dlldb

VPATH = $(subst /,,$(wildcard */))
VSRCS = $(foreach vp,$(VPATH),$(wildcard $(vp)/*.c))

LSRCS = $(notdir $(VSRCS))
LOBJS = $(addsuffix .o, $(basename $(LSRCS)))
BOBJS = $(addprefix $(BLDDIR)/,$(LOBJS))

include ${SDK}/make/Make.lib
include ${SDK}/make/Make.depend

clean::
ifneq (,$(VPATH))
	$Q$(RM) $(addprefix $(SDK)/$(LOCALDIR)/,$(VPATH))
endif
