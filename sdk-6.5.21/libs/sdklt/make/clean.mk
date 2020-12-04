#
# This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
# 
# Copyright 2007-2020 Broadcom Inc. All rights reserved.
#
# SDK cleaning rules
#

ifndef CLEAN_TARGETS
CLEAN_TARGETS = clean cleanall cleanlibs distclean
endif

ifndef SDK_CLEAN
ifneq (,$(filter $(MAKECMDGOALS),$(CLEAN_TARGETS)))
export SDK_CLEAN = $(MAKECMDGOALS)
endif
endif

.PHONY: $(CLEAN_TARGETS)
