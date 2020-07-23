#
# $Id: Makefile,v 1.6 2013/05/07 13:17:24 nirf Exp $
# This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
# 
# Copyright 2007-2020 Broadcom Inc. All rights reserved.
#
# Makefile for phy subdirectory
#
LOCALDIR = src/soc/phy

include ${SDK}/make/Make.config

-include wcmod/src/subdir.mk
-include tscmod/src/subdir.mk
-include phy8806x/src/subdir.mk
ifneq (,$(findstring -DINCLUDE_PLP_IMACSEC,$(CFGFLAGS)))
-include imacsec/subdir.mk
endif
include ${SDK}/make/Make.subdirs

lib = libsoc_phy

include ${SDK}/make/Make.lib

include ${SDK}/make/Make.depend
