#
# This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
# 
# Copyright 2007-2022 Broadcom Inc. All rights reserved.
#

ifeq ($(PHYMOD_CHIPS),)
MAKE_PHYMOD_CHIPS += PHY8806X
endif
ifneq ($(filter PHY8806X,$(MAKE_PHYMOD_CHIPS)),)
VPATH += chip/phy8806x chip/phy8806x/tier2 chip/phy8806x/tier1
VSRCS += $(filter-out chip/phy8806x/phy8806x_pe_api.c,  $(wildcard chip/phy8806x/*.c))
VSRCS += $(wildcard chip/phy8806x/tier2/*.c)
VSRCS += $(wildcard chip/phy8806x/tier1/*.c)
endif
