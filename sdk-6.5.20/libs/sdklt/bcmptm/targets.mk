#
# This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
# 
# Copyright 2007-2020 Broadcom Inc. All rights reserved.
#
# PTM default targets.
#
# This file may be included from the application makefile as well.
#

BCMPTM_DEFAULT_TARGETS = \
	main \
	cth \
	mreq \
	rm_tcam \
	rm_hash \
	rm_alpm \
	ireq \
	cci \
	ser \
	cmdproc \
	wal \
	ptcache \
	cth_be \
	chip \
	misc

ifndef BCMPTM_TARGETS
BCMPTM_TARGETS = $(BCMPTM_DEFAULT_TARGETS)
endif

ifneq (,$(BCMPTM_EXCLUDE_TARGETS))
BCMPTM_TARGETS := $(filter-out $(BCMPTM_EXCLUDE_TARGETS),$(BCMPTM_TARGETS))
endif

BCMPTM_LIBNAMES = $(addprefix libbcmptm,$(BCMPTM_TARGETS))

# Default suffixes for object files
include $(SDK)/make/suffix.mk

BCMPTM_LIBS = $(addsuffix .$(SDK_LIBSUFFIX),$(BCMPTM_LIBNAMES))
