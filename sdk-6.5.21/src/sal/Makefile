# $Id: Makefile,v 1.4 2007/08/16 18:46:17 miyarn Exp $
# This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
# 
# Copyright 2007-2020 Broadcom Inc. All rights reserved.
#
# Makefile for SOC SAL support
#
LOCALDIR = src/sal

include ${SDK}/make/Make.config

ifdef SAL_CORE_OVERRIDE
subdirs=${SAL_CORE_OVERRIDE}
else
subdirs=core
endif

# The sal_appl is not valid for all platforms
ifndef NO_SAL_APPL
subdirs += appl
endif

include ${SDK}/make/Make.subdirs

include ${SDK}/make/Make.depend
