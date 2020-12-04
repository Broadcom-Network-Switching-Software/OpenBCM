#
# This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
# 
# Copyright 2007-2020 Broadcom Inc. All rights reserved.
#
# LTD default targets.
#
# This file may be included from the application makefile as well.
#

LTD_DEFAULT_TARGETS = table conf chip bitop

ifndef LTD_TARGETS
LTD_TARGETS = $(LTD_DEFAULT_TARGETS)
endif

LTD_LIBNAMES = $(addprefix libbcmltd,$(LTD_TARGETS))

# Default suffixes for object files
include $(SDK)/make/suffix.mk

BCMLTD_LIBS = $(addsuffix .$(SDK_LIBSUFFIX),$(LTD_LIBNAMES))
