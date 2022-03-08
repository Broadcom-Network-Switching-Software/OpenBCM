#
# This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
# 
# Copyright 2007-2022 Broadcom Inc. All rights reserved.
#

ifeq ($(PHYMOD_CHIPS),)
MAKE_PHYMOD_CHIPS += HURACAN
endif
ifneq ($(filter HURACAN,$(MAKE_PHYMOD_CHIPS)),)
VPATH += chip/huracan chip/huracan/tier2 chip/huracan/tier1
VSRCS += $(wildcard chip/huracan/*.c)
VSRCS += $(wildcard chip/huracan/tier2/*.c)
VSRCS += $(wildcard chip/huracan/tier1/*.c)
endif
