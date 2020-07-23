#
# This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
# 
# Copyright 2007-2020 Broadcom Inc. All rights reserved.
#
# BCMCNET top-level makefile
#

# Default location for all SDK libraries
include $(SDK)/make/libpath.mk

# Default include path
BCMCNET_INCLUDE_PATH = -I$(BCMCNET)/include


# Support libraries
BCMCNET_INCLUDE_PATH += \
	-I$(SHR)/include \
	-I$(BSL)/include \
	-I$(SAL)/include \
	-I$(BCMBD)/include \
	-I$(BCMDRD)/include \
	-I$(BCMLTD)/include \
	-I$(BCMLRD)/include \
	-I$(BCMPKT)/include

# Import preprocessor flags avoiding include duplicates
TMP_SDK_CPPFLAGS := $(filter-out $(BCMCNET_INCLUDE_PATH),$(SDK_CPPFLAGS))
TMP_SDK_CPPFLAGS += $(BCMCNET_INCLUDE_PATH)
override SDK_CPPFLAGS := $(TMP_SDK_CPPFLAGS)
export SDK_CPPFLAGS

include $(SDK)/make/config.mk

include $(BCMCNET)/targets.mk

.PHONY: all $(CLEAN_TARGETS) $(BCMCNET_TARGETS)

ifneq (,$(filter $(MAKECMDGOALS),$(CLEAN_TARGETS)))
SDK_CLEAN = $(MAKECMDGOALS)
endif

all:
	$(MAKE) bcmcnetlibs

bcmcnetlibs: $(BCMCNET_TARGETS)

$(BCMCNET_TARGETS):
	@${ECHO} Processing BCMCNET subdirectory $@
	$(MAKE) -C $@ $(SDK_CLEAN)

$(CLEAN_TARGETS):: $(BCMCNET_TARGETS)

# Common developer targets
include $(SDK)/make/devtools.mk
