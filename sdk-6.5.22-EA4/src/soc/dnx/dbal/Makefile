#
# $Id: Makefile,v 1.4 Broadcom SDK $
# This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
# 
# Copyright 2007-2021 Broadcom Inc. All rights reserved.
#


LOCALDIR = src/soc/dnx/dbal

include ${SDK}/make/Make.config

subdirs := auto_generated

ifdef DNX2_CHIPS
# {
subdirs += dbal_init
# }		   
endif
   

INCDIR += -I${SDK}/include/soc/dnx

include ${SDK}/make/Make.subdirs

lib = libsoc_dnxdbal

include ${SDK}/make/Make.lib

include ${SDK}/make/Make.depend

