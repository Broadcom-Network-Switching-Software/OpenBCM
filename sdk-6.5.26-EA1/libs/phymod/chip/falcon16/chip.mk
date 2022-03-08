#
# This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
# 
# Copyright 2007-2022 Broadcom Inc. All rights reserved.
#

ifeq ($(PHYMOD_CHIPS),)
MAKE_PHYMOD_CHIPS += FALCON16
endif
ifneq ($(filter FALCON16,$(MAKE_PHYMOD_CHIPS)),)
VPATH += chip/falcon16 chip/falcon16/tier2 chip/falcon16/tier1 chip/falcon16/tier1/src
VSRCS += $(wildcard chip/falcon16/*.c)
VSRCS += $(wildcard chip/falcon16/tier2/*.c)
VSRCS += $(wildcard chip/falcon16/tier1/*.c)
VSRCS += $(wildcard chip/falcon16/tier1/src/*.c)
endif
