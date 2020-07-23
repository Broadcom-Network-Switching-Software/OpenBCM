#
# This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
# 
# Copyright 2007-2020 Broadcom Inc. All rights reserved.
#

LOCALDIR = src/bcm/ltsw/chip/bcm56780_a0

include ${SDK}/make/Make.config

lib = libltsw_bcm56780_a0

VPATH = $(subst /,,$(wildcard */))
VSRCS = $(foreach vp,$(VPATH),$(wildcard $(vp)/*.c))


LSRCS = $(VSRCS)
LSRCS += $(wildcard *.c)

OBJS   :=  $(addsuffix .o, $(basename $(LSRCS)))

ASRCS   :=  $(addprefix $(curr_dir)/,$(notdir $(LSRCS)))

include ${SDK}/make/Make.lib
include ${SDK}/make/Make.depend

