#
# This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
# 
# Copyright 2007-2022 Broadcom Inc. All rights reserved.
#

ifeq ($(PHYMOD_CHIPS),)
MAKE_PHYMOD_CHIPS += SESTO
endif
ifneq ($(filter SESTO,$(MAKE_PHYMOD_CHIPS)),)
VPATH += chip/sesto chip/sesto/tier2 chip/sesto/tier1 chip/sesto/tier1/sesto_serdes/falcon_furia_sesto_src chip/sesto/tier1/sesto_serdes/merlin_sesto_src
VSRCS += $(wildcard chip/sesto/*.c)
VSRCS += $(wildcard chip/sesto/tier2/*.c)
VSRCS += $(wildcard chip/sesto/tier1/*.c)
VSRCS += $(wildcard chip/sesto/tier1/sesto_serdes/falcon_furia_sesto_src/falcon_furia_sesto_functions.c)
VSRCS += $(wildcard chip/sesto/tier1/sesto_serdes/falcon_furia_sesto_src/falcon_furia_sesto_dependencies.c)
VSRCS += $(wildcard chip/sesto/tier1/sesto_serdes/merlin_sesto_src/merlin_sesto_functions.c)
VSRCS += $(wildcard chip/sesto/tier1/sesto_serdes/merlin_sesto_src/merlin_sesto_dependencies.c)
endif
