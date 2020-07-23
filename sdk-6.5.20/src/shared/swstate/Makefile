#
# $Id:$
# This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
# 
# Copyright 2007-2020 Broadcom Inc. All rights reserved.
# 
# Makefile for SOC swstate subdirectory
#
LOCALDIR = src/shared/swstate

include ${SDK}/make/Make.config
#
# Take the subdirs 'access layout diagnostic' if either DPP or DNX are defined
#
subdirs = 
ifdef DPP_CHIPS  
  subdirs += access layout diagnostic infrastructure utils
endif

ifdef INCLUDE_DNX_LIBS  
  subdirs += utils
endif

include ${SDK}/make/Make.subdirs

lib = libshared_swstate

include ${SDK}/make/Make.lib
include ${SDK}/make/Make.depend
