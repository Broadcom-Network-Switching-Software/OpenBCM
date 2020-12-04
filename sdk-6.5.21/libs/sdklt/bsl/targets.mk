#
# This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
# 
# Copyright 2007-2020 Broadcom Inc. All rights reserved.
#
# BSL default targets.
#
# This file may be included from the application makefile as well.
#

BSL_DEFAULT_TARGETS = core

ifndef BSL_TARGETS
BSL_TARGETS = $(BSL_DEFAULT_TARGETS)
endif

ifneq (,$(BSL_EXCLUDE_TARGETS))
BSL_TARGETS := $(filter-out $(BSL_EXCLUDE_TARGETS),$(BSL_TARGETS))
endif

BSL_LIBNAMES = $(addprefix libbsl,$(BSL_TARGETS))

# Default suffixes for object files
include $(SDK)/make/suffix.mk

BSL_LIBS = $(addsuffix .$(SDK_LIBSUFFIX),$(BSL_LIBNAMES))
