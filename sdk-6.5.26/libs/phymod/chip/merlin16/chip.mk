#
# This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
# 
# Copyright 2007-2022 Broadcom Inc. All rights reserved.
#

ifeq ($(PHYMOD_CHIPS),)
MAKE_PHYMOD_CHIPS += MERLIN16
endif
ifneq ($(filter MERLIN16,$(MAKE_PHYMOD_CHIPS)),)
VPATH += chip/merlin16 chip/merlin16/tier2 chip/merlin16/tier1
VSRCS += $(wildcard chip/merlin16/*.c)
VSRCS += $(wildcard chip/merlin16/tier2/*.c)
VSRCS += $(wildcard chip/merlin16/tier1/*.c)
endif
