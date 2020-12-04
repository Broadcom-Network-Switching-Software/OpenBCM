#
# This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
# 
# Copyright 2007-2020 Broadcom Inc. All rights reserved.
#
# BCMLU default targets.
#
# This file may be included from the application makefile as well.
#

BCMLU_DEFAULT_TARGETS =  bde bcmpkt knet

ifndef BCMLU_TARGETS
BCMLU_TARGETS = $(BCMLU_DEFAULT_TARGETS)
endif

BCMLU_LIBNAMES = $(addprefix libbcmlu,$(BCMLU_TARGETS))

# Default suffixes for object files
include $(SDK)/make/suffix.mk

BCMLU_LIBS = $(addsuffix .$(SDK_LIBSUFFIX),$(BCMLU_LIBNAMES))
