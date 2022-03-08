# $Id: Makefile,v 1.6 Broadcom SDK $
# This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
# 
# Copyright 2007-2022 Broadcom Inc. All rights reserved.
# 

include ${SDK}/make/Make.config

export TARGET
export BLDDIR
export LIBDIR

ifdef DNX_CHIPS
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


