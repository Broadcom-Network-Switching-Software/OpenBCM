#
# This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
# 
# Copyright 2007-2020 Broadcom Inc. All rights reserved.
#
# BCMTRM default targets.
#
# This file may be included from the application makefile as well.
#

BCMTRM_DEFAULT_TARGETS = main 

ifndef BCMTRM_TARGETS
BCMTRM_TARGETS = $(BCMTRM_DEFAULT_TARGETS)
endif

ifneq (,$(BCMTRM_EXCLUDE_TARGETS))
BCMTRM_TARGETS := $(filter-out $(BCMTRM_EXCLUDE_TARGETS),$(BCMTRM_TARGETS))
endif

BCMTRM_LIBNAMES = $(addprefix libbcmtrm,$(BCMTRM_TARGETS))

# Default suffixes for object files
include $(SDK)/make/suffix.mk

BCMTRM_LIBS = $(addsuffix .$(SDK_LIBSUFFIX),$(BCMTRM_LIBNAMES))
