#
# This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
# 
# Copyright 2007-2022 Broadcom Inc. All rights reserved.
#

ifeq ($(PHYMOD_CHIPS),)
MAKE_PHYMOD_CHIPS += TSCE7
endif
ifneq ($(filter TSCE7,$(MAKE_PHYMOD_CHIPS)),)
VPATH += chip/tsce7 chip/tsce7/tier2
VSRCS += $(wildcard chip/tsce7/*.c)
VSRCS += $(wildcard chip/tsce7/tier2/*.c)
endif
