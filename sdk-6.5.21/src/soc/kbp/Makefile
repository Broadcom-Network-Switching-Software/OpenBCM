# $Id: Makefile,v 1.5.40.1 2013/05/10 04:08:39 kirankum Exp $
# This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
# 
# Copyright 2007-2020 Broadcom Inc. All rights reserved.

LOCALDIR = src/soc/kbp

include ${SDK}/make/Make.config

ifneq (KBP_ALG,$(findstring KBP_ALG,$(KBP_DEVICE)))
subdirs += common
endif

ifeq (KBP_11K,$(findstring KBP_11K,$(KBP_DEVICE)))
subdirs += nlm2
endif

ifeq (KBP_12K,$(findstring KBP_12K,$(KBP_DEVICE)))
subdirs += nlm3
endif

ifeq (KBP_ALG,$(findstring KBP_ALG,$(KBP_DEVICE)))
subdirs += alg_kbp
endif

include ${SDK}/make/Make.subdirs
include ${SDK}/make/Make.depend
