#
# This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
# 
# Copyright 2007-2022 Broadcom Inc. All rights reserved.
#

ifeq ($(PHYMOD_CHIPS),)
MAKE_PHYMOD_CHIPS += MADURA
endif
ifneq ($(filter MADURA,$(MAKE_PHYMOD_CHIPS)),)
VPATH += chip/madura chip/madura/tier2 chip/madura/tier1 chip/madura/tier1/madura_serdes/falcon_madura_src  chip/madura/tier1/madura_serdes/falcon2_madura_src
VSRCS += $(wildcard chip/madura/*.c)
VSRCS += $(wildcard chip/madura/tier2/*.c)
VSRCS += $(wildcard chip/madura/tier1/*.c)
VSRCS += $(wildcard chip/madura/tier1/madura_serdes/falcon_madura_src/falcon_madura_functions.c)
VSRCS += $(wildcard chip/madura/tier1/madura_serdes/falcon_madura_src/falcon_madura_dependencies.c)
VSRCS += $(wildcard chip/madura/tier1/madura_serdes/falcon2_madura_src/falcon2_madura_functions.c)
VSRCS += $(wildcard chip/madura/tier1/madura_serdes/falcon2_madura_src/falcon2_madura_dependencies.c)
endif
