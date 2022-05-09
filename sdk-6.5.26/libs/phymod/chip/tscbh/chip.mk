#
# This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
# 
# Copyright 2007-2022 Broadcom Inc. All rights reserved.
#

ifeq ($(PHYMOD_CHIPS),)
MAKE_PHYMOD_CHIPS += TSCBH
endif
ifneq ($(filter TSCBH,$(MAKE_PHYMOD_CHIPS)),)
VPATH += chip/tscbh chip/tscbh/tier2 chip/tscbh/tier1
VSRCS += $(wildcard chip/tscbh/*.c)
VSRCS += $(wildcard chip/tscbh/tier2/*.c)
VSRCS += $(wildcard chip/tscbh/tier1/*.c)
endif
