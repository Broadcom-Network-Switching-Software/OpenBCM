#
# This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
# 
# Copyright 2007-2022 Broadcom Inc. All rights reserved.
#

ifeq ($(PHYMOD_CHIPS),)
MAKE_PHYMOD_CHIPS += TSCO_DPLL
endif
ifneq ($(filter TSCO_DPLL,$(MAKE_PHYMOD_CHIPS)),)
VPATH += chip/tsco_dpll chip/tsco_dpll/tier2 chip/tsco_dpll/tier1
VSRCS += $(wildcard chip/tsco_dpll/*.c)
VSRCS += $(wildcard chip/tsco_dpll/tier2/*.c)
VSRCS += $(wildcard chip/tsco_dpll/tier1/*.c)
endif
