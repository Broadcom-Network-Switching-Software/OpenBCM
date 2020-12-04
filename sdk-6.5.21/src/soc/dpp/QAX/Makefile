#
# $Id: Makefile Exp $
#
# This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
# 
# Copyright 2007-2020 Broadcom Inc. All rights reserved.
#

LOCALDIR = src/soc/dpp/QAX/

include ${SDK}/make/Make.config

subdirs := QAX_PP

ifdef BCM_88270_A0  

ifndef LINUX_MAKE_KERNEL
subdirs += PEM
endif

endif

include ${SDK}/make/Make.subdirs

lib = libsoc_dpp_qax

include ${SDK}/make/Make.lib
include ${SDK}/make/Make.depend
