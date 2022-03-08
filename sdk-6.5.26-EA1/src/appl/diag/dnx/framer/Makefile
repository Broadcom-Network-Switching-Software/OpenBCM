#
# $Id: Makefile,v 1.4 Broadcom SDK $
# This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
# 
# Copyright 2007-2022 Broadcom Inc. All rights reserved.
#


LOCALDIR = src/appl/diag/dnx/framer

include ${SDK}/make/Make.config

ifeq (${BUILD_FLEXE_DBG},1)
INCDIR += -I${SDK}/libs/flexe_core/dbg/top
endif

include ${SDK}/make/Make.subdirs

lib = libdiag_dnxflexe

include ${SDK}/make/Make.lib

include ${SDK}/make/Make.depend

