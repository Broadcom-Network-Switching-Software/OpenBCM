#
# This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
# 
# Copyright 2007-2020 Broadcom Inc. All rights reserved.
#
# BCMPC default targets.
#
# This file may be included from the application makefile as well.
#

BCMPC_DEFAULT_TARGETS = main chip lth imm core pmgr_drv pm_drv phy common db hmi

ifndef BCMPC_TARGETS
BCMPC_TARGETS = $(BCMPC_DEFAULT_TARGETS)
endif

ifneq (,$(BCMPC_EXCLUDE_TARGETS))
BCMPC_TARGETS := $(filter-out $(BCMPC_EXCLUDE_TARGETS),$(BCMPC_TARGETS))
endif

BCMPC_LIBNAMES = $(addprefix libbcmpc,$(BCMPC_TARGETS))

# Default suffixes for object files
include $(SDK)/make/suffix.mk

BCMPC_LIBS = $(addsuffix .$(SDK_LIBSUFFIX),$(BCMPC_LIBNAMES))
