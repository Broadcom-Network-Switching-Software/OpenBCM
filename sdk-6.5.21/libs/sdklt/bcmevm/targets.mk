#
# This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
# 
# Copyright 2007-2020 Broadcom Inc. All rights reserved.
#
# BCMEVM default targets.
#
# This file may be included from the application makefile as well.
#

BCMEVM_DEFAULT_TARGETS = main 

ifndef BCMEVM_TARGETS
BCMEVM_TARGETS = $(BCMEVM_DEFAULT_TARGETS)
endif

ifneq (,$(BCMEVM_EXCLUDE_TARGETS))
BCMEVM_TARGETS := $(filter-out $(BCMEVM_EXCLUDE_TARGETS),$(BCMEVM_TARGETS))
endif

BCMEVM_LIBNAMES = $(addprefix libbcmevm,$(BCMEVM_TARGETS))

# Default suffixes for object files
include $(SDK)/make/suffix.mk

BCMEVM_LIBS = $(addsuffix .$(SDK_LIBSUFFIX),$(BCMEVM_LIBNAMES))
