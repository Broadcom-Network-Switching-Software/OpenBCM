#
# $Id: Makefile,v 1.4 2012/03/08 10:43:54 assafz Exp $
# This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
# 
# Copyright 2007-2020 Broadcom Inc. All rights reserved.
#


LOCALDIR = src/bcm/dnx/oam

include ${SDK}/make/Make.config

#
# Take the subdir 
#

ifneq (,$(findstring -DBCM_DNX2_SUPPORT,$(CFGFLAGS)))
subdirs_oamp_v1 =	\
    oamp_v1
endif


ifneq (,$(findstring -DBCM_DNX2_SUPPORT,$(CFGFLAGS)))
subdirs +=  ${subdirs_oamp_v1}
endif

include ${SDK}/make/Make.subdirs

lib = libbcm_dnxoam

include ${SDK}/make/Make.lib

include ${SDK}/make/Make.depend

