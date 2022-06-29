#
# This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
# 
# Copyright 2007-2022 Broadcom Inc. All rights reserved.
#

ifeq ($(PHYMOD_CHIPS),)
MAKE_PHYMOD_CHIPS += QUADRA28
endif
ifneq ($(filter QUADRA28,$(MAKE_PHYMOD_CHIPS)),)
VPATH += chip/quadra28 chip/quadra28/tier2 chip/quadra28/tier1 chip/quadra28/tier1/quadra28_serdes
VSRCS += $(wildcard chip/quadra28/*.c)
VSRCS += $(wildcard chip/quadra28/tier2/*.c)
VSRCS += $(wildcard chip/quadra28/tier1/*.c)
VSRCS += $(wildcard chip/quadra28/tier1/quadra28_serdes/merlin_quadra28_functions.c)
VSRCS += $(wildcard chip/quadra28/tier1/quadra28_serdes/merlin_quadra28_dependencies.c)
endif
