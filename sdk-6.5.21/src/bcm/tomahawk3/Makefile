# $Id: Makefile,v 1.113 2013/02/07 06:57:24 chsindhu Exp $
# This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
# 
# Copyright 2007-2020 Broadcom Inc. All rights reserved.
#
# Makefile for BCM driver Library
#

LOCALDIR = src/bcm/tomahawk3

export BCM_HIDE_DISPATCHABLE=1

include ${SDK}/make/Make.config

subdirs := $(SUBDIR_NONE)

include ${SDK}/make/Make.subdirs

lib = libbcm_th3new
include ${SDK}/make/Make.lib

include ${SDK}/make/Make.depend
