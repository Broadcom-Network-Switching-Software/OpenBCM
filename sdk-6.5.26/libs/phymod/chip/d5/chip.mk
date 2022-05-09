#
# This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
# 
# Copyright 2007-2022 Broadcom Inc. All rights reserved.
#

ifeq ($(PHYMOD_CHIPS),)
MAKE_PHYMOD_CHIPS += D5
endif
ifneq ($(filter D5,$(MAKE_PHYMOD_CHIPS)),)
VPATH += chip/d5 chip/d5/tier2 chip/d5/tier1
VSRCS += $(wildcard chip/d5/*.c)
VSRCS += $(wildcard chip/d5/tier2/*.c)
VSRCS += $(wildcard chip/d5/tier1/*.c)
endif
