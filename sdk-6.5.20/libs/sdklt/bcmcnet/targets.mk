#
# This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
# 
# Copyright 2007-2020 Broadcom Inc. All rights reserved.
#
# BCMCNET default targets.
#
# This file may be included from the application makefile as well.
#

BCMCNET_DEFAULT_TARGETS = main chip hmi

ifndef BCMCNET_TARGETS
BCMCNET_TARGETS = $(BCMCNET_DEFAULT_TARGETS)
endif

ifneq (,$(BCMCNET_EXCLUDE_TARGETS))
BCMCNET_TARGETS := $(filter-out $(BCMCNET_EXCLUDE_TARGETS),$(BCMCNET_TARGETS))
endif

BCMCNET_LIBNAMES = $(addprefix libbcmcnet,$(BCMCNET_DEFAULT_TARGETS))

# Default suffixes for object files
include $(SDK)/make/suffix.mk

BCMCNET_LIBS = $(addsuffix .$(SDK_LIBSUFFIX),$(BCMCNET_LIBNAMES))
