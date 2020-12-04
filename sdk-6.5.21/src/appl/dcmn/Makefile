# $Id: Makefile,v 1.3 2012/10/28 07:42:37 assaf Exp $
# This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
# 
# Copyright 2007-2020 Broadcom Inc. All rights reserved.
# 

include ${SDK}/make/Make.config

export TARGET
export BLDDIR
export LIBDIR

ifeq (INTR,$(findstring INTR,$(FEATURE_LIST)))
# {
subdirs+=rx_los interrupts
# }
else
# {
#
# If INTR is not included, we still need 'rx_los' for DPP (KBP).
# This only happens on adapter
#
ifneq (,$(findstring -DADAPTER_SERVER_MODE,$(CFGFLAGS)))
# {
subdirs += rx_los
# }
endif
# }
endif




include ${SDK}/make/Make.subdirs

