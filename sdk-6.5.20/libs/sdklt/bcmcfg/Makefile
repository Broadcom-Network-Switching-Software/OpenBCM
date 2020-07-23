#
# This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
# 
# Copyright 2007-2020 Broadcom Inc. All rights reserved.
#
# CFG top-level makefile
#

# Default location for all SDK libraries
include $(SDK)/make/libpath.mk

# Default include path
CFG_INCLUDE_PATH = -I$(BCMCFG)/include

# Support libraries
CFG_INCLUDE_PATH += \
	-I$(SHR)/include \
	-I$(BCMLTD)/include \
	-I$(BCMDRD)/include \
	-I$(BCMLRD)/include \
	-I$(BCMISSU)/include \
	-I$(BCMMGMT)/include \
	-I$(BSL)/include \
	-I$(SAL)/include

ifdef YAML
SDK_CPPFLAGS += -I$(YAML)/include
endif

# Import preprocessor flags avoiding include duplicates
TMP_SDK_CPPFLAGS := $(filter-out $(CFG_INCLUDE_PATH),$(SDK_CPPFLAGS))
TMP_SDK_CPPFLAGS += $(CFG_INCLUDE_PATH)
override SDK_CPPFLAGS := $(TMP_SDK_CPPFLAGS)
export SDK_CPPFLAGS

include $(SDK)/make/config.mk

include $(BCMCFG)/targets.mk

.PHONY: all $(CLEAN_TARGETS) $(CFG_TARGETS)

ifneq (,$(filter $(MAKECMDGOALS),$(CLEAN_TARGETS)))
SDK_CLEAN = $(MAKECMDGOALS)
endif

all:
	$(MAKE) -C issu
	$(MAKE) cfglibs

cfglibs: $(CFG_TARGETS)

$(CFG_TARGETS):
	@${ECHO} Processing BCMCFG subdirectory $@
	$(MAKE) -C $@ $(SDK_CLEAN)

$(CLEAN_TARGETS):: $(CFG_TARGETS)
	$(MAKE) -C issu $@

# Common developer targets
include $(SDK)/make/devtools.mk
