#
# This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
# 
# Copyright 2007-2020 Broadcom Inc. All rights reserved.
#
# BCMEVM top-level makefile
#

# Default location for all SDK libraries
include $(SDK)/make/libpath.mk

# Default include path
BCMEVM_INCLUDE_PATH = -I$(BCMEVM)/include

# Support libraries
BCMEVM_INCLUDE_PATH += \
	-I$(BCMDRD)/include \
	-I$(BCMLRD)/include \
	-I$(BCMLTD)/include \
	-I$(SHR)/include \
	-I$(BSL)/include \
	-I$(BCMMGMT)/include \
	-I$(BCMLTM)/include \
	-I$(SAL)/include

# Import preprocessor flags avoiding include duplicates
TMP_SDK_CPPFLAGS := $(filter-out $(BCMEVM_INCLUDE_PATH),$(SDK_CPPFLAGS))
TMP_SDK_CPPFLAGS += $(BCMEVM_INCLUDE_PATH)
override SDK_CPPFLAGS := $(TMP_SDK_CPPFLAGS)
export SDK_CPPFLAGS

include $(SDK)/make/config.mk

include $(BCMEVM)/targets.mk

.PHONY: all $(CLEAN_TARGETS) $(BCMEVM_TARGETS)

ifneq (,$(filter $(MAKECMDGOALS),$(CLEAN_TARGETS)))
SDK_CLEAN = $(MAKECMDGOALS)
endif

all:
	$(MAKE) bcmevmlibs

bcmevmlibs: $(BCMEVM_TARGETS)

$(BCMEVM_TARGETS):
	@${ECHO} Processing BCMEVM subdirectory $@
	$(MAKE) -C $@ $(SDK_CLEAN)

$(CLEAN_TARGETS):: $(BCMEVM_TARGETS)

# Common developer targets
include $(SDK)/make/devtools.mk
