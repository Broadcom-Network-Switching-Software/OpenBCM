#
# This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
# 
# Copyright 2007-2020 Broadcom Inc. All rights reserved.
#
# BCMLTM default targets.
#
# This file may be included from the application makefile as well.
#

BCMLTM_DEFAULT_TARGETS = main md ha ee_stage fa_stage db wb tree lta info mp stats state util

ifndef BCMLTM_TARGETS
BCMLTM_TARGETS = $(BCMLTM_DEFAULT_TARGETS)
endif

ifneq (,$(BCMLTM_EXCLUDE_TARGETS))
BCMLTM_TARGETS := $(filter-out $(BCMLTM_EXCLUDE_TARGETS),$(BCMLTM_TARGETS))
endif

BCMLTM_LIBNAMES = $(addprefix libbcmltm,$(BCMLTM_TARGETS))

# Default suffixes for object files
include $(SDK)/make/suffix.mk

BCMLTM_LIBS = $(addsuffix .$(SDK_LIBSUFFIX),$(BCMLTM_LIBNAMES))
