#
# This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
# 
# Copyright 2007-2022 Broadcom Inc. All rights reserved.
#

ifeq ($(PHYMOD_CHIPS),)
MAKE_PHYMOD_CHIPS += FALCON_DPLL
endif
ifneq ($(filter FALCON_DPLL,$(MAKE_PHYMOD_CHIPS)),)
VPATH += chip/falcon_dpll chip/falcon_dpll/tier2 chip/falcon_dpll/tier1
VSRCS += $(wildcard chip/falcon_dpll/*.c)
VSRCS += $(wildcard chip/falcon_dpll/tier2/*.c)
VSRCS += $(wildcard chip/falcon_dpll/tier1/*.c)
endif
