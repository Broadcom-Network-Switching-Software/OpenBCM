#
# $Id: Makefile,v 1.4 Exp $
# This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
# 
# Copyright 2007-2020 Broadcom Inc. All rights reserved.
#

#this check is done since not all customers receives aacl feature, 
#only when aacl_mngr.h exists in the code APIs are defined
ifneq (,$(wildcard ./aacl_mngr.h))
CFGFLAGS += -DDNX_AACL_OPERATIONAL
endif

LOCALDIR = src/bcm/dnx/aacl

include ${SDK}/make/Make.config

include ${SDK}/make/Make.subdirs

lib = libbcm_dnxaacl

include ${SDK}/make/Make.lib

include ${SDK}/make/Make.depend
