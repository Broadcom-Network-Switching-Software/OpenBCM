#
# This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
# 
# Copyright 2007-2020 Broadcom Inc. All rights reserved.
#
# SAL default targets.
#
# This file may be included from the application makefile as well.
#

SAL_DEFAULT_TARGETS = libc os

ifndef SAL_TARGETS
SAL_TARGETS = $(SAL_DEFAULT_TARGETS)
endif

ifneq (,$(SAL_EXCLUDE_TARGETS))
SAL_TARGETS := $(filter-out $(SAL_EXCLUDE_TARGETS),$(SAL_TARGETS))
endif

SAL_LIBNAMES = $(addprefix libsal,$(SAL_TARGETS))

# Default suffixes for object files
include $(SDK)/make/suffix.mk

SAL_LIBS = $(addsuffix .$(SDK_LIBSUFFIX),$(SAL_LIBNAMES))
