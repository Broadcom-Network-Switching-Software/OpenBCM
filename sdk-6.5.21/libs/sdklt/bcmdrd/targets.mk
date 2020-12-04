#
# This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
# 
# Copyright 2007-2020 Broadcom Inc. All rights reserved.
#
# BCMDRD default targets.
#
# This file may be included from the application makefile as well.
#

BCMDRD_DEFAULT_TARGETS = hal dev chip sym util

ifndef BCMDRD_TARGETS
BCMDRD_TARGETS = $(BCMDRD_DEFAULT_TARGETS)
endif

ifneq (,$(BCMDRD_EXCLUDE_TARGETS))
BCMDRD_TARGETS := $(filter-out $(BCMDRD_EXCLUDE_TARGETS),$(BCMDRD_TARGETS))
endif

BCMDRD_LIBNAMES = $(addprefix libbcmdrd,$(BCMDRD_TARGETS))

# Default suffixes for object files
include $(SDK)/make/suffix.mk

BCMDRD_LIBS = $(addsuffix .$(SDK_LIBSUFFIX),$(BCMDRD_LIBNAMES))
