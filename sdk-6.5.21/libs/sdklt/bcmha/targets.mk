#
# This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
# 
# Copyright 2007-2020 Broadcom Inc. All rights reserved.
#
# BCMHA default targets.
#
# This file may be included from the application makefile as well.
#

BCMHA_DEFAULT_TARGETS = mem

ifndef BCMHA_TARGETS
BCMHA_TARGETS = $(BCMHA_DEFAULT_TARGETS)
endif

ifneq (,$(BCMHA_EXCLUDE_TARGETS))
BCMHA_TARGETS := $(filter-out $(BCMHA_EXCLUDE_TARGETS),$(BCMHA_TARGETS))
endif

BCMHA_LIBNAMES = $(addprefix libbcmha,$(BCMHA_TARGETS))

# Default suffixes for object files
include $(SDK)/make/suffix.mk

BCMHA_LIBS = $(addsuffix .$(SDK_LIBSUFFIX),$(BCMHA_LIBNAMES))
