#
# This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
# 
# Copyright 2007-2022 Broadcom Inc. All rights reserved.
#

ifeq ($(PHYMOD_CHIPS),)
MAKE_PHYMOD_CHIPS += TSCBH_SUB10G
endif
ifneq ($(filter TSCBH_SUB10G,$(MAKE_PHYMOD_CHIPS)),)
VPATH += chip/tscbh_sub10g chip/tscbh_sub10g/tier2 chip/tscbh_sub10g/tier1
VSRCS += $(wildcard chip/tscbh_sub10g/*.c)
VSRCS += $(wildcard chip/tscbh_sub10g/tier2/*.c)
VSRCS += $(wildcard chip/tscbh_sub10g/tier1/*.c)
endif
