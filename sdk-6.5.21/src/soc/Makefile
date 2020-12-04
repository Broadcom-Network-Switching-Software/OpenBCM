# $Id: Makefile,v 1.23 2012/09/21 14:07:38 schacher Exp $
# This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
# 
# Copyright 2007-2020 Broadcom Inc. All rights reserved.
#
# Makefile for SOC driver Library
#
LOCALDIR = src/soc

CFGFLAGS += -DSOURCE_CHECK_LIBSOC

include ${SDK}/make/Make.config

subdirs = phy common shared

ifdef ESW_CHIPS
subdirs += mcm
endif

ifdef ESW_CHIPS
subdirs += esw
endif

ifdef LTSW_CHIPS
subdirs += ltsw
endif

ifdef KBP_DEVICE
subdirs += kbp
endif

#
# The 'dnx' directory is reserved for JR2 family (JR2 and up)
# The 'dnxf' directory is reserved for Ramon family (Ramon and up)
# 
ifdef INCLUDE_DNX_LIBS
subdirs += dnx
subdirs += mcm
subdirs += dnxc
endif

ifdef DNXF_CHIPS
subdirs += dnxf
subdirs += mcm
subdirs += dnxc
endif

ifdef SAND_CHIPS
subdirs += sand
endif

ifdef DPP_CHIPS
subdirs += dpp
subdirs += mcm
subdirs += dcmn
subdirs += hwstate
endif

ifdef DFE_CHIPS
subdirs += dfe
subdirs += mcm
subdirs += dcmn
subdirs += hwstate
endif

ifdef PORTMOD_SUPPORT
ifdef PORTMOD_STAND_ALONE
subdirs += mcm
endif
subdirs+= portmod
endif

ifdef CPRIMOD_SUPPORT
subdirs+= cprimod
endif


ifdef COND_LIB_SOC_I2C
subdirs += i2c
endif

ifdef BCM_56440_A0
ifdef COND_LIB_CES
subdirs += ces
endif
endif

ifdef FEATURE_FCMAP
subdirs += phy/fcmap/src
endif

subdirs += phy/chip/koi/merlin_koi_src
subdirs += phy/chip/orca/eagle_orca_src

ifdef IMACSEC_HOME
subdirs += ${IMACSEC_HOME}
endif

include ${SDK}/make/Make.subdirs

lib = libsoc
include ${SDK}/make/Make.lib

include ${SDK}/make/Make.depend
