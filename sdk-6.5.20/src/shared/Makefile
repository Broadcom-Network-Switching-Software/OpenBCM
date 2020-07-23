# $Id: Makefile,v 1.1 2004/02/24 07:46:47 csm Exp $
# This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
# 
# Copyright 2007-2020 Broadcom Inc. All rights reserved.
#
# Makefile for Shared Library
#

LOCALDIR = src/shared

include ${SDK}/make/Make.config

$(warning INCLUDE ${SDK}/MAKE/MAKE.CONFIG)


#
# For both DPP or DNX, we need to take library xml. However, it is
# NOT taken for 'kernel' compilation (Note that this is not relevant
# for DNX which does not support 'kernel')
#
# For DNX, we do NOT take 'swstate'. In the future, e shall use a
# specifically adapted 'swstate'
#
# Once we have more than 1 family we use SAND_CHIPS
#

ifdef SAND_CHIPS
# { 
subdirs = shrextend utilex dbx
ifdef DPP_CHIPS
# {
subdirs += swstate
# }
endif
ifdef INCLUDE_DNX_LIBS
# {
ifndef DPP_CHIPS
# {
subdirs += swstate
# }
endif
# }
endif
ifeq (,$(findstring -DNO_FILEIO,$(CFGFLAGS)))
# {
	subdirs += libxml
# }
endif
# }
else
# {
ifdef SHR_EXTEND
# {
subdirs += shrextend
# }
endif
# }
endif

subdirs += binfs

include ${SDK}/make/Make.subdirs

lib = libshared
include ${SDK}/make/Make.lib

include ${SDK}/make/Make.depend
