#
# This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
# 
# Copyright 2007-2020 Broadcom Inc. All rights reserved.
#
# LTD top-level makefile
#

# Default location for all SDK libraries
include $(SDK)/make/libpath.mk

# Default include path
LTD_INCLUDE_PATH = -I$(BCMLTD)/include

# Support libraries
LTD_INCLUDE_PATH += \
	-I$(BCMDRD)/include \
	-I$(SHR)/include \
	-I$(BSL)/include \
	-I$(SAL)/include

# Import preprocessor flags avoiding include duplicates
TMP_SDK_CPPFLAGS := $(filter-out $(LTD_INCLUDE_PATH),$(SDK_CPPFLAGS))
TMP_SDK_CPPFLAGS += $(LTD_INCLUDE_PATH)
override SDK_CPPFLAGS := $(TMP_SDK_CPPFLAGS)
export SDK_CPPFLAGS

include $(SDK)/make/config.mk

include $(BCMLTD)/targets.mk

.PHONY: all $(CLEAN_TARGETS) $(LTD_TARGETS)

ifneq (,$(filter $(MAKECMDGOALS),$(CLEAN_TARGETS)))
SDK_CLEAN = $(MAKECMDGOALS)
endif

all:
	$(MAKE) ltdlibs

ltdlibs: $(LTD_TARGETS)

$(LTD_TARGETS):
	@${ECHO} Processing BCMLTD subdirectory $@
	$(MAKE) -C $@ $(SDK_CLEAN)

$(CLEAN_TARGETS):: $(LTD_TARGETS)

# Common developer targets
include $(SDK)/make/devtools.mk
