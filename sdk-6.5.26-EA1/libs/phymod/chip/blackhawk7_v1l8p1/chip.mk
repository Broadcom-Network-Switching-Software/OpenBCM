#
# This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
# 
# Copyright 2007-2022 Broadcom Inc. All rights reserved.
#

ifeq ($(PHYMOD_CHIPS),)
MAKE_PHYMOD_CHIPS += NULL
endif
# BLACKHAWK7_V1L8P1 chip support
ifeq ($(PHYMOD_CHIPS),)
MAKE_PHYMOD_CHIPS += BLACKHAWK7_V1L8P1
endif
ifneq ($(filter BLACKHAWK7_V1L8P1,$(MAKE_PHYMOD_CHIPS)),)
VPATH += chip/blackhawk7_v1l8p1 chip/blackhawk7_v1l8p1/tier2 chip/blackhawk7_v1l8p1/tier1
VSRCS += $(wildcard chip/blackhawk7_v1l8p1/*.c)
VSRCS += $(wildcard chip/blackhawk7_v1l8p1/tier2/*.c)
VSRCS += $(wildcard chip/blackhawk7_v1l8p1/tier1/*.c)
endif
