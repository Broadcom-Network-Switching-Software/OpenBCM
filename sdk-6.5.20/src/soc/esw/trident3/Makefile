# $Id: Makefile,v 1.1.2.1 2012/11/18 00:50:37 wyuan Exp $
#
# This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
# 
# Copyright 2007-2020 Broadcom Inc. All rights reserved.
# Makefile for Trident3 SOC driver Library
#

LOCALDIR = src/soc/esw/trident3

include ${SDK}/make/Make.config

SUBDIR_TRIDENT3_FLEXPORT = flexport

subdirs := $(SUBDIR_TRIDENT3_FLEXPORT)

lib = libsoc_trident3

include ${SDK}/make/Make.subdirs

include ${SDK}/make/Make.lib
include ${SDK}/make/Make.depend

