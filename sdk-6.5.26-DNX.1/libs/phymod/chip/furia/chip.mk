#
# This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
# 
# Copyright 2007-2022 Broadcom Inc. All rights reserved.
#

ifeq ($(PHYMOD_CHIPS),)
MAKE_PHYMOD_CHIPS += FURIA
endif
ifneq ($(filter FURIA,$(MAKE_PHYMOD_CHIPS)),)
VPATH += chip/furia chip/furia/tier2 chip/furia/tier1 chip/furia/tier1/falcon_furia_src chip/furia_82212 chip/furia_82212/tier2
VSRCS += $(wildcard chip/furia/*.c)
VSRCS += $(wildcard chip/furia/tier2/*.c)
VSRCS += $(wildcard chip/furia/tier1/*.c)
VSRCS += $(wildcard chip/furia/tier1/falcon_furia_src/falcon_furia_functions.c)
VSRCS += $(wildcard chip/furia/tier1/falcon_furia_src/falcon_furia_dependencies.c)

VSRCS += $(wildcard chip/furia_82212/*.c)
VSRCS += $(wildcard chip/furia_82212/tier2/*.c)
endif
