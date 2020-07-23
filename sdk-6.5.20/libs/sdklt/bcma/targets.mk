#
# This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
# 
# Copyright 2007-2020 Broadcom Inc. All rights reserved.
#
# BCMA default targets.
#
# This file may be included from the application makefile as well.
#

BCMA_DEFAULT_TARGETS = \
	sys \
	bcmlt \
	bcmtrm \
	bcmmgmt \
	bcmpkt \
	cint \
	bcmpc \
	bcmbd \
	bcmdrd \
	bcmdi \
	bcmptm \
	ha \
	mcs \
	fwm \
	board \
	i2c \
	pcie \
	flash \
	tecsup \
	test \
	hmon \
	sal \
	bsl \
	clirlc \
	cli \
	env \
	editline \
	io \
    hchk

ifndef BCMA_TARGETS
BCMA_TARGETS = $(BCMA_DEFAULT_TARGETS)
endif

ifneq (,$(BCMA_EXCLUDE_TARGETS))
BCMA_TARGETS := $(filter-out $(BCMA_EXCLUDE_TARGETS),$(BCMA_TARGETS))
endif

BCMA_AVAILABLE_TARGETS := $(notdir $(wildcard $(SDK)/bcma/*))
BCMA_TARGETS := $(filter $(BCMA_AVAILABLE_TARGETS),$(BCMA_TARGETS))

ifneq (,$(findstring SAL_NO_STDC,$(SDK_CPPFLAGS)))
BCMA_TARGETS := $(filter-out cint,$(BCMA_TARGETS))
endif

ifneq (,$(findstring cint,$(BCMA_TARGETS)))
export BCMA_CINT_ENABLE=1
endif

BCMA_LIBNAMES = $(addprefix libbcma,$(BCMA_TARGETS))

# Default suffixes for object files
include $(SDK)/make/suffix.mk

BCMA_LIBS = $(addsuffix .$(SDK_LIBSUFFIX),$(BCMA_LIBNAMES))
