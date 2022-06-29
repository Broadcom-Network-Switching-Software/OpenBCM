#
# This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
# 
# Copyright 2007-2022 Broadcom Inc. All rights reserved.
#

ifeq ($(PHYMOD_CHIPS),)
MAKE_PHYMOD_CHIPS += TSCE_DPLL
endif
ifneq ($(filter TSCE_DPLL,$(MAKE_PHYMOD_CHIPS)),)
VPATH += chip/tsce_dpll chip/tsce_dpll/tier2 chip/tsce_dpll/tier1
VSRCS += $(wildcard chip/tsce_dpll/*.c)
VSRCS += $(wildcard chip/tsce_dpll/tier2/*.c)
VSRCS += $(wildcard chip/tsce_dpll/tier1/*.c)
endif
