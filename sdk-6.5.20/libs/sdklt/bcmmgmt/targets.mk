#
# This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
# 
# Copyright 2007-2020 Broadcom Inc. All rights reserved.
#
# BCMMGMT default targets.
#
# This file may be included from the application makefile as well.
#

# SDK_TARGETS =
# include $(SDK)/make/sdklibs.mk

BCMMGMT_DEFAULT_TARGETS = main

ifndef BCMMGMT_TARGETS
BCMMGMT_TARGETS = $(BCMMGMT_DEFAULT_TARGETS)
endif

ifneq (,$(BCMMGMT_EXCLUDE_TARGETS))
BCMMGMT_TARGETS := $(filter-out $(BCMMGMT_EXCLUDE_TARGETS),$(BCMMGMT_TARGETS))
endif

BCMMGMT_LIBNAMES = $(addprefix libbcmmgmt,$(BCMMGMT_TARGETS))

# Default suffixes for object files
include $(SDK)/make/suffix.mk

BCMMGMT_LIBS = $(addsuffix .$(SDK_LIBSUFFIX),$(BCMMGMT_LIBNAMES))
