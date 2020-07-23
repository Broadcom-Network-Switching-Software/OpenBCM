#
# This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
# 
# Copyright 2007-2020 Broadcom Inc. All rights reserved.
#
# BLT default targets.
#
# This file may be included from the application makefile as well.
#

BCMLT_DEFAULT_TARGETS = main 

ifndef BCMLT_TARGETS
BCMLT_TARGETS = $(BCMLT_DEFAULT_TARGETS)
endif

ifneq (,$(BCMLT_EXCLUDE_TARGETS))
BCMLT_TARGETS := $(filter-out $(BCMLT_EXCLUDE_TARGETS),$(BCMLT_TARGETS))
endif

BCMLT_LIBNAMES = $(addprefix libbcmlt,$(BCMLT_TARGETS))

# Default suffixes for object files
include $(SDK)/make/suffix.mk

BCMLT_LIBS = $(addsuffix .$(SDK_LIBSUFFIX),$(BCMLT_LIBNAMES))
