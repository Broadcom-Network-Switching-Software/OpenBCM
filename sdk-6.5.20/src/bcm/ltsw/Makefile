# $Id:$
# This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
# 
# Copyright 2007-2020 Broadcom Inc. All rights reserved.
#
# Makefile for BCM driver Library
#

LOCALDIR = src/bcm/ltsw

export BCM_HIDE_DISPATCHABLE=1

SUBDIR_YAML = yaml

SUBDIR_MBCM = mbcm

include ${SDK}/make/Make.config

ifneq (,$(LTSW_INCLUDE_CHIPS))
SUBDIR_CHIP = chip
endif

ifeq (1,$(LTSW_INCLUDE_XGS))
SUBDIR_XGS = xgs
endif
ifeq (1,$(LTSW_INCLUDE_XFS))
SUBDIR_XFS = xfs
endif

ifeq (1,$(LTSW_INCLUDE_TM))
SUBDIR_TM = tm
endif

subdirs := \
	$(SUBDIR_YAML) \
	$(SUBDIR_MBCM) \
	$(SUBDIR_CHIP) \
	$(SUBDIR_XGS) \
	$(SUBDIR_XFS) \
	$(SUBDIR_TM) \
	$(SUBDIR_NONE)

include ${SDK}/make/Make.subdirs

lib = libbcm_ltsw
include ${SDK}/make/Make.lib

include ${SDK}/make/Make.depend
