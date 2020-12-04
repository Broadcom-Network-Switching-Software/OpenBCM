#
# This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
# 
# Copyright 2007-2020 Broadcom Inc. All rights reserved.
# $Id: Makefile,v 1.11 2013/01/14 12:24:25 assaf Exp $
LOCALDIR = src/appl/reference/dnx/minicontroller

include ${SDK}/make/Make.config

CFGFLAGS += -std=gnu99
export CFGFLAGS

lib = libreference_dnx_minicon

subdirs += armor

include ${SDK}/make/Make.subdirs
include ${SDK}/make/Make.lib
include ${SDK}/make/Make.depend
