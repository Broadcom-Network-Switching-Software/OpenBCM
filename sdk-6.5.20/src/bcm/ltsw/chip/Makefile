# $Id:$
# This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
# 
# Copyright 2007-2020 Broadcom Inc. All rights reserved.
#
# Makefile for BCM driver Library
#

include ${SDK}/make/Make.config

ifneq (,$(LTSW_INCLUDE_CHIPS))
CHIP_NAMES = $(strip $(shell echo $(LTSW_INCLUDE_CHIPS) | tr A-Z a-z))
FILE_NAMES = $(shell ls)
SUBDIR_CHIPS = $(filter $(FILE_NAMES), $(CHIP_NAMES))

ifndef BCM_56990_A0
SUB_CHIPS := bcm56990_b0 bcm56996_a0
ifneq ($(filter $(SUB_CHIPS), $(CHIP_NAMES)),)
SUBDIR_CHIPS += bcm56990_a0
endif
endif
endif

subdirs := $(SUBDIR_CHIPS)

include ${SDK}/make/Make.subdirs