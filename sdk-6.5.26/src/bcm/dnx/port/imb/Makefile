#
# $Id: Makefile,v 1.4 Broadcom SDK $
# This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
# 
# Copyright 2007-2022 Broadcom Inc. All rights reserved.
#


LOCALDIR = src/bcm/dnx/port/imb

include ${SDK}/make/Make.config

#subdirs = imb 

include ${SDK}/make/Make.subdirs

lib = libbcm_dnxport_imb

CFGFLAGS += -std=gnu99 -D_GNU_SOURCE

OBJS  = \
	imb_utils.o \
	imb.o \
	imb_common.o \
	imb_dispatch.o \
	imb_enums.o \
	imb_diag.o \
	imb_ethu.o \
	imb_ethu_internal.o \
	imb_fabric.o \
	imb_fabric_dispatch.o

ifdef DNX2_CHIPS
OBJS  += \
	imb_cdu.o \
	imb_cdu_shr.o \
	imb_cdu_internal.o \
	imb_cdu_dispatch.o \
	imb_cdu_shr_dispatch.o \
	imb_clu.o \
	imb_clu_internal.o \
	imb_clu_dispatch.o \
        imb_framer.o \
        imb_framer_internal.o \
        imb_framer_dispatch.o \
	imb_feu.o \
	imb_feu_phy.o \
	imb_feu_phy_dispatch.o \
	imb_ilu.o \
	imb_ilu_internal.o \
	imb_ilu_dispatch.o \
	imb_ile.o \
	imb_ile_internal.o \
	imb_ile_dispatch.o \
	imb_otnu_phy.o \
	imb_otnu_phy_dispatch.o 

endif


BOBJS = $(addprefix ${BLDDIR}/, ${OBJS})

include ${SDK}/make/Make.lib

include ${SDK}/make/Make.depend

