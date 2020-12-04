#
# This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
# 
# Copyright 2007-2020 Broadcom Inc. All rights reserved.
#
# BCMLM default targets.
#
# This file may be included from the application makefile as well.
#

BCMLM_DEFAULT_TARGETS = main chip hmi

ifndef BCMLM_TARGETS
BCMLM_TARGETS = $(BCMLM_DEFAULT_TARGETS)
endif

ifneq (,$(BCMLM_EXCLUDE_TARGETS))
BCMLM_TARGETS := $(filter-out $(BCMLM_EXCLUDE_TARGETS),$(BCMLM_TARGETS))
endif

BCMLM_LIBNAMES = $(addprefix libbcmlm,$(BCMLM_TARGETS))

# Default suffixes for object files
include $(SDK)/make/suffix.mk

BCMLM_LIBS = $(addsuffix .$(SDK_LIBSUFFIX),$(BCMLM_LIBNAMES))
