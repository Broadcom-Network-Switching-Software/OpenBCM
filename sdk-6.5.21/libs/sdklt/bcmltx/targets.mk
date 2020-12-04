#
# This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
# 
# Copyright 2007-2020 Broadcom Inc. All rights reserved.
#
# LTX default targets.
#
# This file may be included from the application makefile as well.
#

LTX_DEFAULT_TARGETS = \
	bcml3 \
	bcml2 \
	bcmecn \
	bcmlb \
	bcmmirror \
	bcmudf \
	bcmtnl \
	bcmtm \
	bcmdlb \
	bcmev \
	bcmport \
	bcmagm \
	bcmvlan \
	bcmldh \
	bcmtrunk \
	bcmctreflex \
	bcmctr \
	bcmflexdigest \
	bcmmon \
	bcmsec \
	general

ifndef LTX_TARGETS
LTX_TARGETS = $(LTX_DEFAULT_TARGETS)
endif

LTX_LIBNAMES = $(addprefix libbcmltx,$(LTX_TARGETS))

# Default suffixes for object files
include $(SDK)/make/suffix.mk

BCMLTX_LIBS = $(addsuffix .$(SDK_LIBSUFFIX),$(LTX_LIBNAMES))
