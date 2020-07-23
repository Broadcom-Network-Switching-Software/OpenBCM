# -*- Makefile -*-
# $Id: Makefile,v 1.2 2006/06/30 20:20:15 korchakd Exp $
# This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
# 
# Copyright 2007-2020 Broadcom Inc. All rights reserved.
#
# Makefile for BCM rpc driver Library
#

LOCALDIR = src/bcm/common

include ${SDK}/make/Make.config

ifneq (,$(findstring -DINCLUDE_PTP,$(CFGFLAGS)))
SUBDIR_PTP = ptp
endif

subdirs := \
	$(SUBDIR_PTP) \
	$(SUBDIR_NONE)

include ${SDK}/make/Make.subdirs

lib = libbcm_common
include ${SDK}/make/Make.lib

include ${SDK}/make/Make.depend
