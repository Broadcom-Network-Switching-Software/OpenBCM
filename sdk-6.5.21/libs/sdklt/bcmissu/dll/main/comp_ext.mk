#
# This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
# 
# Copyright 2007-2020 Broadcom Inc. All rights reserved.
#
# Components handlers
#

IMM_COMP_SRC = $(SDK)/bcmimm/issu_dll
BCMLTM_COMP_SRC = $(SDK)/bcmltm/issu_dll
BCMPTM_COMP_SRC = $(SDK)/bcmptm/issu_dll

VPATH += $(IMM_COMP_SRC) $(BCMLTM_COMP_SRC) + $(BCMPTM_COMP_SRC)

COMP_VSRCS += $(wildcard $(IMM_COMP_SRC)/*.c)
COMP_VSRCS += $(wildcard $(BCMLTM_COMP_SRC)/*.c)
COMP_VSRCS += $(wildcard $(BCMPTM_COMP_SRC)/*.c)

COMP_LSRCS = $(notdir $(COMP_VSRCS))
COMP_LOBJS = $(addsuffix .o, $(basename $(COMP_LSRCS)))
BOBJS += $(addprefix $(BLDDIR)/,$(COMP_LOBJS))
VSRCS += $(COMP_VSRCS)

SDK_CPPFLAGS += -I$(SDK)/bcmimm/include -I$(SDK)/bcmltm/include -I$(SDK)/bcmptm/include
