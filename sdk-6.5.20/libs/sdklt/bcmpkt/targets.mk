#
# This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
# 
# Copyright 2007-2020 Broadcom Inc. All rights reserved.
#
# BCMPKT default targets.
#
# This file may be included from the application makefile as well.
#

-include $(SDK)/bcmpkt/hdr.mk
BCMPKT_DEFAULT_TARGETS = trace unet
BCMPKT_DEFAULT_TARGETS += $(BCMPKT_HDR_TARGETS)
BCMPKT_DEFAULT_TARGETS += net socket knet dev buf txpmd rxpmd lbpmd chip sym

ifndef BCMPKT_TARGETS
BCMPKT_TARGETS = $(BCMPKT_DEFAULT_TARGETS)
endif

ifneq (,$(BCMPKT_EXCLUDE_TARGETS))
BCMPKT_TARGETS := $(filter-out $(BCMPKT_EXCLUDE_TARGETS),$(BCMPKT_TARGETS))
endif

BCMPKT_LIBNAMES = $(addprefix libbcmpkt,$(BCMPKT_TARGETS))

# Default suffixes for object files
include $(SDK)/make/suffix.mk

BCMPKT_LIBS = $(addsuffix .$(SDK_LIBSUFFIX),$(BCMPKT_LIBNAMES))
