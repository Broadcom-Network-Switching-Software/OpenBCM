#
# This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
# 
# Copyright 2007-2022 Broadcom Inc. All rights reserved.
#

ifeq ($(PHYMOD_CHIPS),)
MAKE_PHYMOD_CHIPS += QTCE
endif
ifneq ($(filter QTCE,$(MAKE_PHYMOD_CHIPS)),)
VPATH += chip/qtce chip/qtce/tier2 chip/qtce/tier1
VSRCS += $(wildcard chip/qtce/*.c)
VSRCS += $(wildcard chip/qtce/tier2/*.c)
VSRCS += $(wildcard chip/qtce/tier1/*.c)
endif
