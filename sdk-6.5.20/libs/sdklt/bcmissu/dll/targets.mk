#
# This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
# 
# Copyright 2007-2020 Broadcom Inc. All rights reserved.
#
# BLTISSU default targets.
#
# This file may be included from the application makefile as well.
#

BCMISSU_DEFAULT_DLL_TARGETS = main db

ifndef BCMISSU_DLL_TARGETS
BCMISSU_DLL_TARGETS = $(BCMISSU_DEFAULT_DLL_TARGETS)
endif

BCMISSU_DLL_LIBNAMES = $(addprefix libdll,$(BCMISSU_DLL_TARGETS))

# Default suffixes for object files
include $(SDK)/make/suffix.mk

BCMISSU_DLL_LIBS = $(addsuffix .$(SDK_LIBSUFFIX),$(BCMISSU_DLL_LIBNAMES))
