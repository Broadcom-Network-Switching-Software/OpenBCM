#
# This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
# 
# Copyright 2007-2020 Broadcom Inc. All rights reserved.
#
# BCMDI default targets.
#
# This file may be included from the application makefile as well.
#

BCMDI_DEFAULT_TARGETS = main chip util

ifndef BCMDI_TARGETS
BCMDI_TARGETS = $(BCMDI_DEFAULT_TARGETS)
endif

ifneq (,$(BCMDI_EXCLUDE_TARGETS))
BCMDI_TARGETS := $(filter-out $(BCMDI_EXCLUDE_TARGETS),$(BCMDI_TARGETS))
endif

BCMDI_LIBNAMES = $(addprefix libbcmdi,$(BCMDI_TARGETS))

# Default suffixes for object files
include $(SDK)/make/suffix.mk

BCMDI_LIBS = $(addsuffix .$(SDK_LIBSUFFIX),$(BCMDI_LIBNAMES))
