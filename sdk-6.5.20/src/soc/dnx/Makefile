#
# $Id: Makefile,v 1.4 2012/03/08 10:43:54 assafz Exp $
# This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
# 
# Copyright 2007-2020 Broadcom Inc. All rights reserved.
#

include ${SDK}/make/Make.config

CFGFLAGS += -std=gnu99
#
# If this is compilation for SLK-ALL then optimize for code
# size. This is added to temporarily overcome the code size
# issue on SLK compiler.
#
ifeq ($(findstring OPTIMIZE_CODE,$(MAKE_CUSTOM_SLK_CODE_SIZE)),OPTIMIZE_CODE)
CFGFLAGS += -Os
endif

export CFGFLAGS

subdirs =	adapter	\
			dbal	\
			dnx_data\
			drv	\
			field/tcam_access	\
			pemladrv	\
			pll \
			swstate	\
			recovery	\
			kbp  \
			phy  \
			linkctrl  \
			utils  \
			arr  \
			intr \
			intr/auto_generated

#jericho2 dnx PEMLA compilation
ifeq ($(PEMLA_BUILD),1)
PEMLA_Q2AB0_PATH=${SDK}/tools/dnx/pemla/qumran2a_bo
PEMLA_Q2AA0_PATH=${SDK}/tools/dnx/pemla/qumran2a_a0
PEMLA_JR2A0_PATH=${SDK}/tools/dnx/pemla/jericho2_a0
PEMLA_JR2B0_PATH=${SDK}/tools/dnx/pemla/jericho2_b0
PEMLA_J2CA0_PATH=${SDK}/tools/dnx/pemla/jericho2c_a0
PEMLA_J2PA0_PATH=${SDK}/tools/dnx/pemla/jericho2p_a0
# PEMLA_J2PMA0_PATH=${SDK}/tools/dnx/pemla/jericho2pm_a0
ifneq ($(REAL_SDK),)
PEMLA_Q2AB0_PATH=${REAL_SDK}/tools/dnx/pemla/qumran2a_b0
PEMLA_Q2AA0_PATH=${REAL_SDK}/tools/dnx/pemla/qumran2a_a0
PEMLA_JR2A0_PATH=${REAL_SDK}/tools/dnx/pemla/jericho2_a0
PEMLA_JR2B0_PATH=${REAL_SDK}/tools/dnx/pemla/jericho2_b0
PEMLA_J2CA0_PATH=${REAL_SDK}/tools/dnx/pemla/jericho2c_a0
PEMLA_J2PA0_PATH=${REAL_SDK}/tools/dnx/pemla/jericho2p_a0
# PEMLA_J2PMA0_PATH=${REAL_SDK}/tools/dnx/pemla/jericho2pm_a0
endif
#Build ucode if pemla folder exists
ifneq ($(wildcard $(PEMLA_JR2A0_PATH)),)
subdirs += $(PEMLA_JR2A0_PATH)
endif
ifneq ($(wildcard $(PEMLA_JR2B0_PATH)),)
subdirs += $(PEMLA_JR2B0_PATH)
endif
ifneq ($(wildcard $(PEMLA_J2CA0_PATH)),)
subdirs += $(PEMLA_J2CA0_PATH)
endif
ifneq ($(wildcard $(PEMLA_Q2AB0_PATH)),)
subdirs += $(PEMLA_Q2AB0_PATH)
endif
ifneq ($(wildcard $(PEMLA_Q2AA0_PATH)),)
subdirs += $(PEMLA_Q2AA0_PATH)
endif
ifneq ($(wildcard $(PEMLA_J2PA0_PATH)),)
subdirs += $(PEMLA_J2PA0_PATH)
endif
ifneq ($(wildcard $(PEMLA_J2PMA0_PATH)),)
subdirs += $(PEMLA_J2PMA0_PATH)
endif
endif

INCDIR += -I${SDK}/include/shared

lib = libsoc_dnx

include ${SDK}/make/Make.lib
include ${SDK}/make/Make.subdirs
