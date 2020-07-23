#
# This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
# 
# Copyright 2007-2020 Broadcom Inc. All rights reserved.
#
# BCMTM default targets.
#
# This file may be included from the application makefile as well.
#

BCMTM_DEFAULT_TARGETS = main lth chip shared tdm util

ifndef BCMTM_TARGETS
BCMTM_TARGETS = $(BCMTM_DEFAULT_TARGETS)
endif

BCMTM_LIBNAMES = $(addprefix libbcmtm,$(BCMTM_TARGETS))

# Default suffixes for object files
include $(SDK)/make/suffix.mk

BCMTM_LIBS = $(addsuffix .$(SDK_LIBSUFFIX),$(BCMTM_LIBNAMES))
