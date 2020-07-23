#
# This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
# 
# Copyright 2007-2020 Broadcom Inc. All rights reserved.
#
# TRM top-level makefile
#

# Default location for all SDK libraries
include $(SDK)/make/libpath.mk

# Default include path
BCMTRM_INCLUDE_PATH = -I$(BCMTRM)/include -I.

# Support libraries
BCMTRM_INCLUDE_PATH += \
	-I$(BCMLT)/include \
	-I$(BCMDRD)/include \
	-I$(BCMLRD)/include \
	-I$(BCMLTD)/include \
	-I$(BCMMGMT)/include \
	-I$(BCMEVM)/include \
	-I$(SHR)/include \
	-I$(BCMLTM)/include \
	-I$(BCMCFG)/include \
	-I$(BCMPTM)/include \
	-I$(BCMISSU)/include \
	-I$(BSL)/include \
	-I$(SAL)/include

# Import preprocessor flags avoiding include duplicates
TMP_SDK_CPPFLAGS := $(filter-out $(BCMTRM_INCLUDE_PATH),$(SDK_CPPFLAGS))
TMP_SDK_CPPFLAGS += $(BCMTRM_INCLUDE_PATH)
override SDK_CPPFLAGS := $(TMP_SDK_CPPFLAGS)
export SDK_CPPFLAGS

include $(SDK)/make/config.mk

include $(BCMTRM)/targets.mk

.PHONY: all $(CLEAN_TARGETS) $(BCMTRM_TARGETS)

ifneq (,$(filter $(MAKECMDGOALS),$(CLEAN_TARGETS)))
SDK_CLEAN = $(MAKECMDGOALS)
endif

all:
	$(MAKE) -C yaml
	$(MAKE) bcmtrmlibs

bcmtrmlibs: $(BCMTRM_TARGETS)

$(BCMTRM_TARGETS):
	@${ECHO} Processing BCMTRM subdirectory $@
	$(MAKE) -C $@ $(SDK_CLEAN)

$(CLEAN_TARGETS):: $(BCMTRM_TARGETS)
	$(MAKE) -C yaml $@

# Common developer targets
include $(SDK)/make/devtools.mk
