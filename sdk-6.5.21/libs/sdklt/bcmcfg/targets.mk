#
# This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
# 
# Copyright 2007-2020 Broadcom Inc. All rights reserved.
#
# CFG default targets.
#
# This file may be included from the application makefile as well.
#

CFG_DEFAULT_TARGETS = main init ha yaml lt handler reader config comp swcomp

ifndef CFG_TARGETS
CFG_TARGETS = $(CFG_DEFAULT_TARGETS)
endif

CFG_LIBNAMES = $(addprefix libbcmcfg,$(CFG_TARGETS))

# Default suffixes for object files
include $(SDK)/make/suffix.mk

BCMCFG_LIBS = $(addsuffix .$(SDK_LIBSUFFIX),$(CFG_LIBNAMES))
