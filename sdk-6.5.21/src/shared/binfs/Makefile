# $Id: Makefile,v 1.1 2004/02/24 07:46:47 csm Exp $
# This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
# 
# Copyright 2007-2020 Broadcom Inc. All rights reserved.
#
# Makefile for Shared Library
#

LOCALDIR = src/shared/binfs

include ${SDK}/make/Make.config

ifeq ($(BINFS_UPDATE),TRUE)
$(warning Update binfs data .... $(shell python $(SDK)/tools/genbinfs.py > /dev/null))
endif

OBJS = binfs.o binfs_file_list.o

BOBJS = $(addprefix ${BLDDIR}/, ${OBJS})

lib = libshared_binfs
include ${SDK}/make/Make.lib
include ${SDK}/make/Make.depend
