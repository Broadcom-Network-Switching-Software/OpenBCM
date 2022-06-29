#
# This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
# 
# Copyright 2007-2022 Broadcom Inc. All rights reserved.
#

ifeq ($(PHYMOD_CHIPS),)
MAKE_PHYMOD_CHIPS += FALCON16_V1L4P1
endif
ifneq ($(filter FALCON16_V1L4P1,$(MAKE_PHYMOD_CHIPS)),)
VPATH += chip/falcon16_v1l4p1 chip/falcon16_v1l4p1/tier2 chip/falcon16_v1l4p1/tier1 chip/falcon16_v1l4p1/tier1/src
VSRCS += $(wildcard chip/falcon16_v1l4p1/*.c)
VSRCS += $(wildcard chip/falcon16_v1l4p1/tier2/*.c)
VSRCS += $(wildcard chip/falcon16_v1l4p1/tier1/*.c)
VSRCS += $(wildcard chip/falcon16_v1l4p1/tier1/src/*.c)
endif
