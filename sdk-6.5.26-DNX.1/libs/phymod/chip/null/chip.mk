#
# This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
# 
# Copyright 2007-2022 Broadcom Inc. All rights reserved.
#

ifeq ($(PHYMOD_CHIPS),)
MAKE_PHYMOD_CHIPS += NULL
endif
ifneq ($(filter NULL,$(MAKE_PHYMOD_CHIPS)),)
VPATH += chip/null
VSRCS += $(wildcard chip/null/*.c)
endif
