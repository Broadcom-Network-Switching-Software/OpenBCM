#
# This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
# 
# Copyright 2007-2020 Broadcom Inc. All rights reserved.
#
# SIM default targets.
#
# This file may be included from the application makefile as well.
#

SIM_DEFAULT_TARGETS = pli chelem

ifndef SIM_TARGETS
SIM_TARGETS = $(SIM_DEFAULT_TARGETS)
endif

ifneq (,$(SIM_EXCLUDE_TARGETS))
SIM_TARGETS := $(filter-out $(SIM_EXCLUDE_TARGETS),$(SIM_TARGETS))
endif

SIM_AVAILABLE_TARGETS := $(notdir $(wildcard $(SDK)/sim/*))
SIM_TARGETS := $(filter $(SIM_AVAILABLE_TARGETS),$(SIM_TARGETS))

SIM_LIBNAMES = $(addprefix libsim,$(SIM_TARGETS))

# Default suffixes for object files
include $(SDK)/make/suffix.mk

SIM_LIBS = $(addsuffix .$(SDK_LIBSUFFIX),$(SIM_LIBNAMES))
