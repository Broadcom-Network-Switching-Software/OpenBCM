#
# This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
# 
# Copyright 2007-2020 Broadcom Inc. All rights reserved.
#
# Default SDK build targets.
#

#
# To avoid confusion, the list below should be in the same order as
# the libraries are listed in sdklibs.mk.
#
SDK_DEFAULT_TARGETS = \
	bcma \
	bcmmgmt \
	bcmlu \
	bcmlt \
	bcmltp \
	bcmtrm \
	bcmdi \
	bcmltm \
	bcmlta \
	bcmltx \
	bcmfp \
	bcmsec \
	bcmtm \
	bcmcth \
	bcmecmp \
	bcmpkt \
	bcmcnet \
	bcmptm \
	bcmlm \
	bcmpc \
	bcmbd \
	bcmimm \
	bcmcfg \
	bcmissu \
	bcmlrd \
	bcmpmac \
	phymod \
	pciephy \
	bcmevm \
	bcmltd \
	bcmha \
	bcmdrd \
	sim \
	shr \
	sal \
	bsl

ifeq (,$(SDK_TARGETS))
SDK_TARGETS = $(SDK_DEFAULT_TARGETS)
endif

ifneq (,$(SDK_EXCLUDE_TARGETS))
SDK_TARGETS := $(filter-out $(SDK_EXCLUDE_TARGETS),$(SDK_TARGETS))
endif
