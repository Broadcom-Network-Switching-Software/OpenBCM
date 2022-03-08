#
# This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
# 
# Copyright 2007-2022 Broadcom Inc. All rights reserved.
#

ifeq ($(PHYMOD_CHIPS),)
MAKE_PHYMOD_CHIPS += TSCBH_FLEXE
endif
ifneq ($(filter TSCBH_FLEXE,$(MAKE_PHYMOD_CHIPS)),)
VPATH += chip/tscbh_flexe chip/tscbh_flexe/tier2 chip/tscbh_flexe/tier1
VSRCS += $(wildcard chip/tscbh_flexe/*.c)
VSRCS += $(wildcard chip/tscbh_flexe/tier2/*.c)
VSRCS += $(wildcard chip/tscbh_flexe/tier1/*.c)
endif
