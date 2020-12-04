# $Id: Makefile,v 1.6 2007/03/15 02:23:45 sanjayg Exp $
# This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
# 
# Copyright 2007-2020 Broadcom Inc. All rights reserved.
#
# Makefile for SOC drivers
#
LOCALDIR = systems/drv

include ${SDK}/make/Make.config

#
# For each "base_target", define a variable "${target-base}-drivers" that
# contains the drivers that need to be built for that target.
#
# Current targets are: vxworks (VxWorks) and unix (Solaris)
#
ifneq (,$(findstring -DINCLUDE_DRIVERS,$(CFGFLAGS)))
vxworks-drivers	= socend
unix-drivers   	=
endif

ifneq (,$(findstring -DINCLUDE_BCM570X,$(CFGFLAGS)))
subdirs := bcm570x
endif

ifneq (,$(findstring -DINCLUDE_RCPU,$(CFGFLAGS)))
subdirs += eth
endif

ifneq (,$(subdirs))
include ${SDK}/make/Make.subdirs
endif


#
# Get list into one variable.
#
target-drivers	= $($(targetbase)-drivers)

#
# Need to override sources and binaries to only use local
#
LSRCS := $(foreach drv, $(target-drivers), $(filter $(drv).%,$(LSRCS)))
LOBJS := $(foreach drv, $(target-drivers), $(filter $(drv).%,$(LOBJS)))
BOBJS := $(addprefix ${BLDDIR}/,${LOBJS})

lib = libdrivers
include ${SDK}/make/Make.lib

include ${SDK}/make/Make.depend
