#
# This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
# 
# Copyright 2007-2022 Broadcom Inc. All rights reserved.
#

ifeq ($(PHYMOD_CHIPS),)
MAKE_PHYMOD_CHIPS += EAGLE_DPLL
endif
ifneq ($(filter EAGLE_DPLL,$(MAKE_PHYMOD_CHIPS)),)
VPATH += chip/eagle_dpll chip/eagle_dpll/tier2 chip/eagle_dpll/tier1
VSRCS += $(wildcard chip/eagle_dpll/*.c)
VSRCS += $(wildcard chip/eagle_dpll/tier2/*.c)
VSRCS += $(wildcard chip/eagle_dpll/tier1/*.c)
endif
