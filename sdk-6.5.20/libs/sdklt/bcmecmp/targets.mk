#
# This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
# 
# Copyright 2007-2020 Broadcom Inc. All rights reserved.
#
# BCMECMP default targets.
#

BCMECMP_DEFAULT_TARGETS = main imm chip grpmgr shared util

ifndef BCMECMP_TARGETS
BCMECMP_TARGETS = $(BCMECMP_DEFAULT_TARGETS)
endif

BCMECMP_LIBNAMES = $(addprefix libbcmecmp,$(BCMECMP_TARGETS))

# Default suffixes for object files
include $(SDK)/make/suffix.mk

BCMECMP_LIBS = $(addsuffix .$(SDK_LIBSUFFIX),$(BCMECMP_LIBNAMES))
