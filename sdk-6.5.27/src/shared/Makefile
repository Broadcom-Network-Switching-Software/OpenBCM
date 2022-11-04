# $Id: Makefile,v 1.1 Broadcom SDK $
# This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
# 
# Copyright 2007-2022 Broadcom Inc. All rights reserved.
#
# Makefile for Shared Library
#

LOCALDIR = src/shared

include ${SDK}/make/Make.config

$(warning INCLUDE ${SDK}/MAKE/MAKE.CONFIG)


subdirs =
#
# For both DPP or DNX, we need to take library xml. However, it is
# NOT taken for 'kernel' compilation (Note that this is not relevant
# for DNX which does not support 'kernel')
#
# For DNX, we do NOT take 'swstate'. We use a specifically adapted 'swstate'
#
ifdef SAND_CHIPS
# { 
ifdef DPP_CHIPS
# {
subdirs += swstate
# }
endif
# }
endif

#
# For SAND, ESW (and whoever LIBXML is defined for, on FEATURE_LIST), we
# need to take the various 'shared' libraries. This includes library xml.
# However, it is NOT taken for 'kernel' compilation.
#
ifneq (,$(findstring LIBXML,$(FEATURE_LIST)))
# {
subdirs += utilex dbx
ifeq (,$(findstring -DNO_FILEIO,$(CFGFLAGS)))
# {
  subdirs += libxml
# }
endif
# }
endif

subdirs += binfs


include ${SDK}/make/Make.subdirs

lib = libshared
include ${SDK}/make/Make.lib

include ${SDK}/make/Make.depend
