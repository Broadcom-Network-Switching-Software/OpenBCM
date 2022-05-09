#
# This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
# 
# Copyright 2007-2022 Broadcom Inc. All rights reserved.
#

ifeq ($(PHYMOD_CHIPS),)
MAKE_PHYMOD_CHIPS += QTCE16
endif
ifneq ($(filter QTCE16,$(MAKE_PHYMOD_CHIPS)),)
VPATH += chip/qtce16 chip/qtce16/tier2 chip/qtce16/tier1
VSRCS += $(wildcard chip/qtce16/*.c)
VSRCS += $(wildcard chip/qtce16/tier2/*.c)
VSRCS += $(wildcard chip/qtce16/tier1/*.c)
endif
