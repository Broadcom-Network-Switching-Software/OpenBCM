#
# $Id: Makefile,v 1.3 2012/02/27 15:20:18 omri Exp $
#
# This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
# 
# Copyright 2007-2020 Broadcom Inc. All rights reserved.
#

LOCALDIR = src/soc/dpp/ARAD/

#
# We disable 'uninitialized' warnings since they come from the
# obfuscated file arad_pp_frwrd_ipv4_lpm_mngr.c
#
ifneq (,$(findstring -DGTS_COMPILATION,$(CFGFLAGS)))
CFGFLAGS += -Wno-uninitialized -Wno-maybe-uninitialized
endif

include ${SDK}/make/Make.config

lib = libbcm_dpp_arad_pp

include ${SDK}/make/Make.lib
include ${SDK}/make/Make.depend
