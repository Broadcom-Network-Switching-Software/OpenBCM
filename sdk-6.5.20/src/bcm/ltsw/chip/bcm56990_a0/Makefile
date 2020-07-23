#
# This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
# 
# Copyright 2007-2020 Broadcom Inc. All rights reserved.
#

LOCALDIR = src/bcm/ltsw/chip/bcm56990_a0

include ${SDK}/make/Make.config

SUBDIRS =

ifdef BCM_56990_A0
SUBDIRS += bcm56990_a0
endif

ifdef BCM_56990_B0
SUBDIRS += bcm56990_b0 bcm5699x_shr
endif

ifdef BCM_56996_A0
SUBDIRS += bcm56996_a0 bcm5699x_shr
endif

VPATH = $(SUBDIRS)
LSRCS += $(foreach sd, $(SUBDIRS), $(wildcard $(sd)/*.c))
OBJS   :=  $(addsuffix .o, $(basename $(LSRCS)))

ASRCS   :=  $(addprefix $(curr_dir)/,$(LSRCS))


lib = libltsw_bcm56990_a0

include ${SDK}/make/Make.lib
include ${SDK}/make/Make.depend

ifdef BCM_56990_B0
$(shell ln -s -f $(LIBDIR)/$(lib).a $(LIBDIR)/libltsw_bcm56990_b0.a)
endif

ifdef BCM_56996_A0
$(shell ln -s -f $(LIBDIR)/$(lib).a $(LIBDIR)/libltsw_bcm56996_a0.a)
endif
