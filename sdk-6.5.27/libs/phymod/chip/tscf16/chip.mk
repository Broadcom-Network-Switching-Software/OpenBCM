#
# This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
# 
# Copyright 2007-2022 Broadcom Inc. All rights reserved.
#

ifeq ($(PHYMOD_CHIPS),)
MAKE_PHYMOD_CHIPS += TSCF16
endif
ifneq ($(filter TSCF16,$(MAKE_PHYMOD_CHIPS)),)
VPATH += chip/tscf16 chip/tscf16/tier2 chip/tscf16/tier1
VSRCS += $(wildcard chip/tscf16/*.c)
VSRCS += $(wildcard chip/tscf16/tier2/*.c)
VSRCS += $(wildcard chip/tscf16/tier1/*.c)
endif
