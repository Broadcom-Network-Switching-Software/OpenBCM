#
# This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
# 
# Copyright 2007-2020 Broadcom Inc. All rights reserved.
#
# TM top-level makefile
#

# Default location for all SDK libraries
include $(SDK)/make/libpath.mk

# Default include path
BCMTM_INCLUDE_PATH = -I$(BCMTM)/include


# Support libraries
BCMTM_INCLUDE_PATH += \
	-I$(SHR)/include \
	-I$(BCMLT)/include \
	-I$(BCMLTD)/include \
	-I$(BSL)/include \
	-I$(SAL)/include \
	-I$(BCMBD)/include \
	-I$(BCMDRD)/include \
	-I$(BCMPTM)/include \
	-I$(BCMLTM)/include \
	-I$(BCMLRD)/include \
	-I$(BCMMGMT)/include \
	-I$(BCMPC)/include \
	-I$(BCMPMAC)/include \
	-I$(PHYMOD)/include \
	-I$(BCMCFG)/include \
	-I$(BCMIMM)/include \
	-I$(BCMCTH)/include \
	-I$(BCMISSU)/include

# Import preprocessor flags avoiding include duplicates
TMP_SDK_CPPFLAGS := $(filter-out $(BCMTM_INCLUDE_PATH),$(SDK_CPPFLAGS))
TMP_SDK_CPPFLAGS += $(BCMTM_INCLUDE_PATH)
override SDK_CPPFLAGS := $(TMP_SDK_CPPFLAGS)
export SDK_CPPFLAGS

include $(SDK)/make/config.mk

include $(BCMTM)/targets.mk

.PHONY: all $(CLEAN_TARGETS) $(BCMTM_TARGETS)

ifneq (,$(filter $(MAKECMDGOALS),$(CLEAN_TARGETS)))
SDK_CLEAN = $(MAKECMDGOALS)
endif

all:
	$(MAKE) -C yaml
	$(MAKE) bcmtmlibs

bcmtmlibs: $(BCMTM_TARGETS)

$(BCMTM_TARGETS):
	@${ECHO} Processing BCMTM subdirectory $@
	$(MAKE) -C $@ $(SDK_CLEAN)

$(CLEAN_TARGETS):: $(BCMTM_TARGETS)
	$(MAKE) -C yaml $@

# Common developer targets
include $(SDK)/make/devtools.mk
