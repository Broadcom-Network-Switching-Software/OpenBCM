# $Id: Makefile,v 1.6 2012/09/13 16:17:34 siadous Exp $
# This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
# 
# Copyright 2007-2020 Broadcom Inc. All rights reserved.
# 

include ${SDK}/make/Make.config

export TARGET
export BLDDIR
export LIBDIR

subdirs += sweep 
ifeq (DUNE_UI,$(findstring DUNE_UI,$(FEATURE_LIST)))
subdirs += UserInterface
endif

ifneq (,$(findstring -DBCM_PETRA_SUPPORT,$(CFGFLAGS)))
ifeq (INTR,$(findstring INTR,$(FEATURE_LIST)))
subdirs += interrupts
endif
endif


ifdef DPP_CHIPS
subdirs += FecAllocation
subdirs += FecPerformance
subdirs += fabric_fgl
endif


include ${SDK}/make/Make.subdirs


