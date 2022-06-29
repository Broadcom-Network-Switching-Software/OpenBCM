#
# This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
# 
# Copyright 2007-2022 Broadcom Inc. All rights reserved.
#

ifeq ($(PHYMOD_CHIPS),)
MAKE_PHYMOD_CHIPS += DINO
endif
ifneq ($(filter DINO,$(MAKE_PHYMOD_CHIPS)),)
VPATH += chip/dino chip/dino/tier2 chip/dino/tier1 chip/dino/tier1/dino_serdes/falcon2_dino_src chip/dino/tier1/dino_serdes/merlin_dino_src
VSRCS += $(wildcard chip/dino/*.c)
VSRCS += $(wildcard chip/dino/tier2/*.c)
VSRCS += $(wildcard chip/dino/tier1/*.c)
VSRCS += $(wildcard chip/dino/tier1/dino_serdes/falcon2_dino_src/falcon2_dino_functions.c)
VSRCS += $(wildcard chip/dino/tier1/dino_serdes/falcon2_dino_src/falcon2_dino_dependencies.c)
VSRCS += $(wildcard chip/dino/tier1/dino_serdes/merlin_dino_src/merlin_dino_functions.c)
VSRCS += $(wildcard chip/dino/tier1/dino_serdes/merlin_dino_src/merlin_dino_dependencies.c)
endif
