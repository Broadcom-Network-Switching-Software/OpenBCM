# $Id: Makefile,v 1.5 Broadcom SDK $
# This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
# 
# Copyright 2007-2022 Broadcom Inc. All rights reserved.
# 

include ${SDK}/make/Make.config

export TARGET
export BLDDIR
export LIBDIR

ifneq (,$(findstring -DBCM_DFE_SUPPORT,$(CFGFLAGS)))
subdirs+=interrupts
endif

include ${SDK}/make/Make.subdirs
