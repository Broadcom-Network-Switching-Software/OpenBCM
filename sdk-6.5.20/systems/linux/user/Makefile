# -*- Makefile -*-
# $Id: Makefile,v 1.11 2007/10/06 01:23:18 sanjayg Exp $
# This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
# 
# Copyright 2007-2020 Broadcom Inc. All rights reserved.
#
# Makefile for StrataXGS SDK and diagnostics
#

include $(SDK)/make/Make.config

ifeq ($(HOSTTYPE),i386-linux)
subdirs= bmw jag nsx nsx64 raptor nsx-2_6 bmw-2_6 jag-2_6 raptor-2_6 nsx_wrl-2_6 gto-2_6
endif

include $(SDK)/make/Make.subdirs

include $(SDK)/make/Make.depend
