#
# This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
# 
# Copyright 2007-2022 Broadcom Inc. All rights reserved.
#

ifeq ($(PHYMOD_CHIPS),)
MAKE_PHYMOD_CHIPS += OSPREY7_V2L8P2
endif
ifneq ($(filter OSPREY7_V2L8P2,$(MAKE_PHYMOD_CHIPS)),)
VPATH += chip/osprey7_v2l8p2 chip/osprey7_v2l8p2/tier2 chip/osprey7_v2l8p2/tier1
VSRCS += $(wildcard chip/osprey7_v2l8p2/*.c)
VSRCS += $(wildcard chip/osprey7_v2l8p2/tier2/*.c)
VSRCS += $(wildcard chip/osprey7_v2l8p2/tier1/*.c)
endif
