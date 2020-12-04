#
#
# This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
# 
# Copyright 2007-2020 Broadcom Inc. All rights reserved.
#

LOCALDIR = src/shared/dbx

include ${SDK}/make/Make.config

CFGFLAGS += -std=gnu99

INCDIR += -I${SDK}/include/shared

lib = libshared_dbx

OBJS  = \
	dbx_file.o

ifeq (,$(findstring -DNO_FILEIO,$(CFGFLAGS)))
OBJS += dbx_pre_compiled_xml_parser_internal.o
OBJS += dbx_xml.o
endif
BOBJS = $(addprefix ${BLDDIR}/, ${OBJS})

include ${SDK}/make/Make.subdirs
include ${SDK}/make/Make.lib
include ${SDK}/make/Make.depend
