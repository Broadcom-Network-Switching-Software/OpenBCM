#
# This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
# 
# Copyright 2007-2022 Broadcom Inc. All rights reserved.
#

ifeq ($(PHYMOD_CHIPS),)
MAKE_PHYMOD_CHIPS += BLACKHAWK
endif
ifneq ($(filter BLACKHAWK,$(MAKE_PHYMOD_CHIPS)),)
VPATH += chip/blackhawk chip/blackhawk/tier2 chip/blackhawk/tier1
VSRCS += $(wildcard chip/blackhawk/*.c)
VSRCS += $(wildcard chip/blackhawk/tier2/*.c)
VSRCS += $(wildcard chip/blackhawk/tier1/*.c)
VSRCS += $(wildcard chip/blackhawk/tier1/src/*.c)
endif
