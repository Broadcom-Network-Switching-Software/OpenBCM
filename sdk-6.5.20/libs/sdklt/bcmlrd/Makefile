#
# This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
# 
# Copyright 2007-2020 Broadcom Inc. All rights reserved.
#
# LRD top-level makefile
#

# Default location for all SDK libraries
include $(SDK)/make/libpath.mk

# Default include path
LRD_INCLUDE_PATH = -I$(BCMLRD)/include

# Support libraries
LRD_INCLUDE_PATH += \
	-I$(BCMDRD)/include \
	-I$(BCMMGMT)/include \
	-I$(BCMLTD)/include \
	-I$(SHR)/include \
	-I$(BSL)/include \
	-I$(SAL)/include

# Import preprocessor flags avoiding include duplicates
TMP_SDK_CPPFLAGS := $(filter-out $(LRD_INCLUDE_PATH),$(SDK_CPPFLAGS))
TMP_SDK_CPPFLAGS += $(LRD_INCLUDE_PATH)
override SDK_CPPFLAGS := $(TMP_SDK_CPPFLAGS)
export SDK_CPPFLAGS

include $(SDK)/make/config.mk

include $(BCMLRD)/targets.mk

.PHONY: all $(CLEAN_TARGETS) doc $(LRD_TARGETS)

ifneq (,$(filter $(MAKECMDGOALS),$(CLEAN_TARGETS)))
SDK_CLEAN = $(MAKECMDGOALS)
endif

all:
	$(MAKE) lrdlibs

lrdlibs: $(LRD_TARGETS)

$(LRD_TARGETS):
	@${ECHO} Processing BCMLRD subdirectory $@
	$(MAKE) -C $@ $(SDK_CLEAN)

$(CLEAN_TARGETS):: $(LRD_TARGETS)

# Common developer targets
include $(SDK)/make/devtools.mk
