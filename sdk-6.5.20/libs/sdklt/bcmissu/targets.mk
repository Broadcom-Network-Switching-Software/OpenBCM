#
# This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
# 
# Copyright 2007-2020 Broadcom Inc. All rights reserved.
#
# BLTISSU default targets.
#
# This file may be included from the application makefile as well.
#

BCMISSU_DEFAULT_TARGETS = main

ifndef BCMISSU_TARGETS
BCMISSU_TARGETS = $(BCMISSU_DEFAULT_TARGETS)
endif

BCMISSU_LIBNAMES = $(addprefix libbcmissu,$(BCMISSU_TARGETS))

# Default suffixes for object files
include $(SDK)/make/suffix.mk

BCMISSU_LIBS = $(addsuffix .$(SDK_LIBSUFFIX),$(BCMISSU_LIBNAMES))
