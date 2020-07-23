#
# This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
# 
# Copyright 2007-2020 Broadcom Inc. All rights reserved.
#
# FP default targets.
#
# This file may be included from the application makefile as well.
#

BCMFP_DEFAULT_TARGETS = main em_ft em ingress vlan egress handler chip common algorithms

ifndef BCMFP_TARGETS
BCMFP_TARGETS = $(BCMFP_DEFAULT_TARGETS)
endif

ifneq (,$(BCMFP_EXCLUDE_TARGETS))
BCMFP_TARGETS := $(filter-out $(BCMFP_EXCLUDE_TARGETS),$(BCMFP_TARGETS))
endif

BCMFP_LIBNAMES = $(addprefix libbcmfp,$(BCMFP_TARGETS))

# Default suffixes for object files
include $(SDK)/make/suffix.mk

BCMFP_LIBS = $(addsuffix .$(SDK_LIBSUFFIX),$(BCMFP_LIBNAMES))
