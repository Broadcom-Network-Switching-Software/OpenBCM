#
# This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
# 
# Copyright 2007-2020 Broadcom Inc. All rights reserved.
#
# CFG default targets.
#
# This file may be included from the application makefile as well.
#

BCMIMM_DEFAULT_TARGETS = main handler backend

ifndef BCMIMM_TARGETS
BCMIMM_TARGETS = $(BCMIMM_DEFAULT_TARGETS)
endif

BCMIMM_LIBNAMES = $(addprefix libbcmimm,$(BCMIMM_TARGETS))

# Default suffixes for object files
include $(SDK)/make/suffix.mk

BCMIMM_LIBS = $(addsuffix .$(SDK_LIBSUFFIX),$(BCMIMM_LIBNAMES))
