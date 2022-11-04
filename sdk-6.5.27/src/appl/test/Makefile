# $Id: Makefile,v 1.3 Broadcom SDK $
# This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
# 
# Copyright 2007-2022 Broadcom Inc. All rights reserved.
#
# Makefile for Broadcom Command Monitor (BCM) diagnostic tests
#

LOCALDIR = src/appl/test

CFGFLAGS += -DSOURCE_CHECK_LIBAPPL

include ${SDK}/make/Make.config

subdirs =
ifdef DNX_CHIPS
subdirs += dnx
endif
ifdef DNXF_CHIPS
subdirs += dnxf
endif
ifdef SAND_CHIPS
subdirs += sand
endif
ifdef DPP_CHIPS
subdirs += dpp
endif

include ${SDK}/make/Make.subdirs

lib = libtest
include ${SDK}/make/Make.lib

include ${SDK}/make/Make.depend
