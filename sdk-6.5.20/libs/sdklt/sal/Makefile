#
# This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
# 
# Copyright 2007-2020 Broadcom Inc. All rights reserved.
#
# SAL top-level makefile
#

# Default location for all SDK libraries
include $(SDK)/make/libpath.mk

# Default include path
SAL_INCLUDE_PATH = -I$(SAL)/include

# Import preprocessor flags avoiding include duplicates
TMP_SDK_CPPFLAGS := $(filter-out $(SAL_INCLUDE_PATH),$(SDK_CPPFLAGS))
TMP_SDK_CPPFLAGS += $(SAL_INCLUDE_PATH)
TMP_SDK_CPPFLAGS += $(SAL_CONFIG_FLAGS)
override SDK_CPPFLAGS := $(TMP_SDK_CPPFLAGS)
export SDK_CPPFLAGS

include $(SDK)/make/config.mk

include $(SAL)/targets.mk

.PHONY: all $(CLEAN_TARGETS) $(SAL_TARGETS)

ifneq (,$(filter $(MAKECMDGOALS),$(CLEAN_TARGETS)))
SDK_CLEAN = $(MAKECMDGOALS)
endif

all:
	$(MAKE) sallibs

sallibs: $(SAL_TARGETS)

$(SAL_TARGETS):
	@${ECHO} Processing SAL subdirectory $@
	$(MAKE) -C $@ $(SDK_CLEAN)

$(CLEAN_TARGETS):: $(SAL_TARGETS)

# Common developer targets
include $(SDK)/make/devtools.mk
