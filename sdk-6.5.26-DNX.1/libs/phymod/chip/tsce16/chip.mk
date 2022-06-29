#
# This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
# 
# Copyright 2007-2022 Broadcom Inc. All rights reserved.
#

ifeq ($(PHYMOD_CHIPS),)
MAKE_PHYMOD_CHIPS += TSCE16
endif
ifneq ($(filter TSCE16,$(MAKE_PHYMOD_CHIPS)),)
VPATH += chip/tsce16 chip/tsce16/tier2 chip/tsce16/tier1
VSRCS += $(wildcard chip/tsce16/*.c)
VSRCS += $(wildcard chip/tsce16/tier2/*.c)
VSRCS += $(wildcard chip/tsce16/tier1/*.c)
endif
