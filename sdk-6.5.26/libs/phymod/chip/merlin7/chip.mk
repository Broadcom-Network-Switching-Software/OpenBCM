#
# This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
# 
# Copyright 2007-2022 Broadcom Inc. All rights reserved.
#

ifeq ($(PHYMOD_CHIPS),)
MAKE_PHYMOD_CHIPS += MERLIN7
endif
ifneq ($(filter MERLIN7,$(MAKE_PHYMOD_CHIPS)),)
VPATH += chip/merlin7 chip/merlin7/tier2 chip/merlin7/tier1
VSRCS += $(wildcard chip/merlin7/*.c)
VSRCS += $(wildcard chip/merlin7/tier2/*.c)
VSRCS += $(wildcard chip/merlin7/tier1/*.c)
endif
