#
# This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
# 
# Copyright 2007-2020 Broadcom Inc. All rights reserved.
#
# BCMPC top-level makefile
#

# Default location for all SDK libraries
include $(SDK)/make/libpath.mk

# Default include path
BCMPC_INCLUDE_PATH = -I$(BCMPC)/include

# Support libraries
BCMPC_INCLUDE_PATH += \
	-I$(SHR)/include \
	-I$(BSL)/include \
	-I$(SAL)/include \
	-I$(BCMIMM)/include \
	-I$(BCMDRD)/include \
	-I$(BCMLTD)/include \
	-I$(BCMLTM)/include \
	-I$(BCMMGMT)/include \
	-I$(BCMBD)/include \
	-I$(BCMLRD)/include \
	-I$(BCMPMAC)/include \
	-I$(PHYMOD)/include \
	-I$(BCMEVM)/include \
	-I$(BCMISSU)/include \
	-I$(BCMCFG)/include

# Import preprocessor flags avoiding include duplicates
TMP_SDK_CPPFLAGS := $(filter-out $(BCMPC_INCLUDE_PATH),$(SDK_CPPFLAGS))
TMP_SDK_CPPFLAGS += $(BCMPC_INCLUDE_PATH)
override SDK_CPPFLAGS := $(TMP_SDK_CPPFLAGS)
export SDK_CPPFLAGS

include $(SDK)/make/config.mk

include $(BCMPC)/targets.mk

.PHONY: all $(CLEAN_TARGETS) $(BCMPC_TARGETS)

ifneq (,$(filter $(MAKECMDGOALS),$(CLEAN_TARGETS)))
SDK_CLEAN = $(MAKECMDGOALS)
endif

all:
	$(MAKE) -C yaml
	$(MAKE) bcmpclibs

bcmpclibs: $(BCMPC_TARGETS)

$(BCMPC_TARGETS):
	@${ECHO} Processing BCMPC subdirectory $@
	$(MAKE) -C $@ $(SDK_CLEAN)

$(CLEAN_TARGETS):: $(BCMPC_TARGETS)

# Common developer targets
include $(SDK)/make/devtools.mk
