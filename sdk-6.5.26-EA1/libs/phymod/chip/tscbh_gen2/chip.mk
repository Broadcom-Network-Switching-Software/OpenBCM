#
# This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
# 
# Copyright 2007-2022 Broadcom Inc. All rights reserved.
#

ifeq ($(PHYMOD_CHIPS),)
MAKE_PHYMOD_CHIPS += TSCBH_GEN2
endif
ifneq ($(filter TSCBH_GEN2,$(MAKE_PHYMOD_CHIPS)),)
VPATH += chip/tscbh_gen2 chip/tscbh_gen2/tier2 chip/tscbh_gen2/tier1
VSRCS += $(wildcard chip/tscbh_gen2/*.c)
VSRCS += $(wildcard chip/tscbh_gen2/tier2/*.c)
VSRCS += $(wildcard chip/tscbh_gen2/tier1/*.c)
endif
