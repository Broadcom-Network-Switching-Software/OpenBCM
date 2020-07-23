#
# This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
# 
# Copyright 2007-2020 Broadcom Inc. All rights reserved.
# $Id: Makefile,v 1.11 2016/06/14 11:11:11 mfrank Exp $
LOCALDIR = src/appl/diag/dnxf

include ${SDK}/make/Make.config

CFGFLAGS += -std=gnu99
export CFGFLAGS

lib = libdiag_dnxf

subdirs+= swstate

include ${SDK}/make/Make.subdirs
include ${SDK}/make/Make.lib
include ${SDK}/make/Make.depend
