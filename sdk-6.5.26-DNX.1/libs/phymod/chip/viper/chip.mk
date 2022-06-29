#
# This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
# 
# Copyright 2007-2022 Broadcom Inc. All rights reserved.
#

ifeq ($(PHYMOD_CHIPS),)
MAKE_PHYMOD_CHIPS += VIPER
endif
ifneq ($(filter VIPER,$(MAKE_PHYMOD_CHIPS)),)
VPATH += chip/viper chip/viper/tier2 chip/viper/tier1
VSRCS += $(wildcard chip/viper/*.c)
VSRCS += $(wildcard chip/viper/tier2/*.c)
VSRCS += $(wildcard chip/viper/tier1/*.c)
endif
