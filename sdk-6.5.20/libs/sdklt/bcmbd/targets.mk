#
# This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
# 
# Copyright 2007-2020 Broadcom Inc. All rights reserved.
#
# BCMBD default targets.
#
# This file may be included from the application makefile as well.
#

BCMBD_DEFAULT_TARGETS = main \
			chip \
			hmi \
			pcie \
			qspi \
			i2c \
			miim \
			gpio \
			ts \
			pvt \
			led \
			mcs \
			m0ssq \
			fwm \
			dma \
			intr \
			shared

ifndef BCMBD_TARGETS
BCMBD_TARGETS = $(BCMBD_DEFAULT_TARGETS)
endif

ifneq (,$(BCMBD_EXCLUDE_TARGETS))
BCMBD_TARGETS := $(filter-out $(BCMBD_EXCLUDE_TARGETS),$(BCMBD_TARGETS))
endif

BCMBD_LIBNAMES = $(addprefix libbcmbd,$(BCMBD_TARGETS))

# Default suffixes for object files
include $(SDK)/make/suffix.mk

BCMBD_LIBS = $(addsuffix .$(SDK_LIBSUFFIX),$(BCMBD_LIBNAMES))
