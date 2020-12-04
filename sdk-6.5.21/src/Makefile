# $Id: Makefile,v 1.18 2011/08/30 07:13:28 mlarsen Exp $
# This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
# 
# Copyright 2007-2020 Broadcom Inc. All rights reserved.
#
# Makefile for Switch on a Chip (SOC) driver and diagnostics
#

include ${SDK}/make/Make.config

export TARGET
export BLDDIR
export LIBDIR


subdirs = soc shared sal bcm appl

ifdef COND_LIB_BCMX
subdirs += bcmx
endif

ifdef COND_LIB_CUSTOMER
subdirs += customer
endif

ifdef COND_LIB_EXAMPLES
subdirs += examples
endif

ifdef COND_LIB_BOARD
subdirs += board
endif

ifdef FLAT_TREE
subdirs += flat
endif

override MAKEFLAGS += TARGET=$(TARGET)

include ${SDK}/make/Make.subdirs
