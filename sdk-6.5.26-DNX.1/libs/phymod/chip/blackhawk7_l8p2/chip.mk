#
# This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
# 
# Copyright 2007-2022 Broadcom Inc. All rights reserved.
#

ifeq ($(PHYMOD_CHIPS),)
MAKE_PHYMOD_CHIPS += BLACKHAWK7_L8P2
endif
ifneq ($(filter BLACKHAWK7_L8P2,$(MAKE_PHYMOD_CHIPS)),)
VPATH += chip/blackhawk7_l8p2 chip/blackhawk7_l8p2/tier2 chip/blackhawk7_l8p2/tier1
VSRCS += $(wildcard chip//blackhawk7_l8p2/*.c)
VSRCS += $(wildcard chip//blackhawk7_l8p2/tier2/*.c)
VSRCS += $(wildcard chip//blackhawk7_l8p2/tier1/*.c)
endif
