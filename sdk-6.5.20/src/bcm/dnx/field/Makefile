#
# $Id: Makefile,v 1.4 2012/03/08 10:43:54 assafz Exp $
# This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
# 
# Copyright 2007-2020 Broadcom Inc. All rights reserved.
#


LOCALDIR = src/bcm/dnx/field

include ${SDK}/make/Make.config

INCDIR += -I${SDK}/include/bcm/dnx/field

subdirs += tcam map

include ${SDK}/make/Make.subdirs

lib = libbcm_dnxfield

include ${SDK}/make/Make.lib

include ${SDK}/make/Make.depend

