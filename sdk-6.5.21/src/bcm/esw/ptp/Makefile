# $Id: Makefile,v 1.3 2012/01/21 00:23:04 reldeeb Exp $
# This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
# 
# Copyright 2007-2020 Broadcom Inc. All rights reserved.
#
# Makefile for BCM driver Library
#

LOCALDIR = src/bcm/esw/ptp

include ${SDK}/make/Make.config

ifneq (,$(findstring -DBCM_KSCPU_SUPPORT,$(CFGFLAGS)))
CFLAGS += -DPTP_KEYSTONE_STACK
endif
#CFLAGS += -DPTP_RX_CALLBACK_DEBUG

lib = libptp_esw

include ${SDK}/make/Make.lib
include ${SDK}/make/Make.depend

