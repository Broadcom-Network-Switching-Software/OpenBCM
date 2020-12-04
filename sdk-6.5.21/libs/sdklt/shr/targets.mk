#
# This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
# 
# Copyright 2007-2020 Broadcom Inc. All rights reserved.
#
# SHR default targets.
#
# This file may be included from the application makefile as well.
#

SHR_DEFAULT_TARGETS = \
	avl \
	cht \
	hash \
	sysm \
	fmm \
	lmem_mgr \
	thread \
	timeout \
	ha \
	mpool \
	itbm \
	error \
	crc \
	bitop \
	util \
	atomic \
	pb \
	aidxres

ifndef SHR_TARGETS
SHR_TARGETS = $(SHR_DEFAULT_TARGETS)
endif

ifneq (,$(SHR_EXCLUDE_TARGETS))
SHR_TARGETS := $(filter-out $(SHR_EXCLUDE_TARGETS),$(SHR_TARGETS))
endif

SHR_LIBNAMES = $(addprefix libshr,$(SHR_TARGETS))

# Default suffixes for object files
include $(SDK)/make/suffix.mk

SHR_LIBS = $(addsuffix .$(SDK_LIBSUFFIX),$(SHR_LIBNAMES))
