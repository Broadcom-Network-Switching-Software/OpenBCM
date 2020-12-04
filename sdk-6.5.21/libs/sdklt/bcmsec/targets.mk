#
# This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
# 
# Copyright 2007-2020 Broadcom Inc. All rights reserved.
#
# BCMSEC default targets.
#
# This file may be included from the application makefile as well.
#

BCMSEC_DEFAULT_TARGETS = main lth chip imm util

ifndef BCMSEC_TARGETS
BCMSEC_TARGETS = $(BCMSEC_DEFAULT_TARGETS)
endif

ifneq (,$(BCMSEC_EXCLUDE_TARGETS))
BCMSEC_TARGETS := $(filter-out $(BCMSEC_EXCLUDE_TARGETS),$(BCMSEC_TARGETS))
endif

BCMSEC_LIBNAMES = $(addprefix libbcmsec,$(BCMSEC_TARGETS))

# Default suffixes for object files
include $(SDK)/make/suffix.mk

BCMSEC_LIBS = $(addsuffix .$(SDK_LIBSUFFIX),$(BCMSEC_LIBNAMES))
