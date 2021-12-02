#
# $Id: Makefile,v 1.4 Broadcom SDK $
# This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
# 
# Copyright 2007-2021 Broadcom Inc. All rights reserved.
#


LOCALDIR = src/bcm/dnx/oam

include ${SDK}/make/Make.config

#
# Take the subdir 
#

ifdef DNX2_CHIPS
# {
subdirs_oamp_v1 =	\
    oamp_v1
# }
endif


ifdef DNX2_CHIPS
# {
subdirs +=  ${subdirs_oamp_v1}
# }
endif

include ${SDK}/make/Make.subdirs

lib = libbcm_dnxoam

include ${SDK}/make/Make.lib

include ${SDK}/make/Make.depend

