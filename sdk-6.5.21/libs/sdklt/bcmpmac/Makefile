#
# This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
# 
# Copyright 2007-2020 Broadcom Inc. All rights reserved.
#
# BCM PortMac top-level makefile
#

# Default location for all SDK libraries
include $(SDK)/make/libpath.mk

# Default include path
BCMPMAC_INCLUDE_PATH = -I$(BCMPMAC)/include

# Support libraries
BCMPMAC_INCLUDE_PATH += \
	-I$(BSL)/include \
	-I$(SAL)/include \
	-I$(SHR)/include

# Import preprocessor flags avoiding include duplicates
TMP_SDK_CPPFLAGS := $(filter-out $(BCMPMAC_INCLUDE_PATH),$(SDK_CPPFLAGS))
TMP_SDK_CPPFLAGS += $(BCMPMAC_INCLUDE_PATH)
override SDK_CPPFLAGS := $(TMP_SDK_CPPFLAGS)
export SDK_CPPFLAGS

include $(SDK)/make/config.mk

include $(BCMPMAC)/targets.mk

.PHONY: all $(CLEAN_TARGETS) $(BCMPMAC_TARGETS)

ifneq (,$(filter $(MAKECMDGOALS),$(CLEAN_TARGETS)))
SDK_CLEAN = $(MAKECMDGOALS)
endif

all:
	$(MAKE) bcmpmaclibs

bcmpmaclibs: $(BCMPMAC_TARGETS)

$(BCMPMAC_TARGETS):
	@${ECHO} Processing BCMPMAC subdirectory $@
	$(MAKE) -C $@ $(SDK_CLEAN)

$(CLEAN_TARGETS):: $(BCMPMAC_TARGETS)

# Common developer targets
include $(SDK)/make/devtools.mk
