#
# This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
# 
# Copyright 2007-2020 Broadcom Inc. All rights reserved.
#
# LTP default targets.
#
# This file may be included from the application makefile as well.
#

LTP_DEFAULT_TARGETS = main player

ifndef LTP_TARGETS
LTP_TARGETS = $(LTP_DEFAULT_TARGETS)
endif

LTP_LIBNAMES = $(addprefix libbcmltp,$(LTP_TARGETS))

# Default suffixes for object files
include $(SDK)/make/suffix.mk

BCMLTP_LIBS = $(addsuffix .$(SDK_LIBSUFFIX),$(LTP_LIBNAMES))
