# $Id: Makefile,v 1.6 2012/09/13 16:17:34 siadous Exp $
# This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
# 
# Copyright 2007-2020 Broadcom Inc. All rights reserved.
# 

include ${SDK}/make/Make.config

export TARGET
export BLDDIR
export LIBDIR

ifdef INCLUDE_DNX_LIBS
subdirs += dnxc
subdirs += dnx
endif

ifdef DNXF_CHIPS
subdirs += dnxc
subdirs += dnxf
endif

ifdef SAND_CHIPS
subdirs += sand
endif

include ${SDK}/make/Make.subdirs


