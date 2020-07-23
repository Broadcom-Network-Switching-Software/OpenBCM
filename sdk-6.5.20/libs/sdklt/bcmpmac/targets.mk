#
# This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
# 
# Copyright 2007-2020 Broadcom Inc. All rights reserved.
#
# BCM PortMAC default targets.
#
# This file may be included from the application makefile as well.
#

BCMPMAC_DEFAULT_TARGETS = chip util

ifndef BCMPMAC_TARGETS
BCMPMAC_TARGETS = $(BCMPMAC_DEFAULT_TARGETS)
endif

BCMPMAC_LIBNAMES = $(addprefix libbcmpmac,$(BCMPMAC_TARGETS))

# Default suffixes for object files
include $(SDK)/make/suffix.mk

BCMPMAC_LIBS = $(addsuffix .$(SDK_LIBSUFFIX),$(BCMPMAC_LIBNAMES))
