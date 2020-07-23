#
# This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
# 
# Copyright 2007-2020 Broadcom Inc. All rights reserved.
#
# LRD default targets.
#
# This file may be included from the application makefile as well.
#

LRD_DEFAULT_TARGETS = main client table map init chip probe

ifndef LRD_TARGETS
LRD_TARGETS = $(LRD_DEFAULT_TARGETS)
endif

LRD_LIBNAMES = $(addprefix libbcmlrd,$(LRD_TARGETS))

# Default suffixes for object files
include $(SDK)/make/suffix.mk

BCMLRD_LIBS = $(addsuffix .$(SDK_LIBSUFFIX),$(LRD_LIBNAMES))
