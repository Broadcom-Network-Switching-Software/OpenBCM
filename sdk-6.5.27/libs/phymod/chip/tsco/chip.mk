#
# This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
# 
# Copyright 2007-2022 Broadcom Inc. All rights reserved.
#

ifeq ($(PHYMOD_CHIPS),)
MAKE_PHYMOD_CHIPS += TSCO
endif
ifneq ($(filter TSCO,$(MAKE_PHYMOD_CHIPS)),)
VPATH += chip/tsco chip/tsco/tier2 chip/tsco/tier1
VSRCS += $(wildcard chip/tsco/*.c)
VSRCS += $(wildcard chip/tsco/tier2/*.c)
VSRCS += $(wildcard chip/tsco/tier1/*.c)
endif
