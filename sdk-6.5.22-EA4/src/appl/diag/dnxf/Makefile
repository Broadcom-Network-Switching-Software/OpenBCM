#
# This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
# 
# Copyright 2007-2021 Broadcom Inc. All rights reserved.
# $Id: Makefile,v 1.11 Broadcom SDK $
LOCALDIR = src/appl/diag/dnxf

include ${SDK}/make/Make.config

CFGFLAGS += -std=gnu99
export CFGFLAGS

lib = libdiag_dnxf

subdirs+= swstate

include ${SDK}/make/Make.subdirs
include ${SDK}/make/Make.lib
include ${SDK}/make/Make.depend
