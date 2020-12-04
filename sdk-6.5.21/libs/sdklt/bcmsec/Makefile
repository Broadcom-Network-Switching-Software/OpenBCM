#
# This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
# 
# Copyright 2007-2020 Broadcom Inc. All rights reserved.
#
# BCMSEC top-level makefile
#

# Default location for all SDK libraries
include $(SDK)/make/libpath.mk

# Default include path
BCMSEC_INCLUDE_PATH = -I$(BCMSEC)/include

# Support libraries
BCMSEC_INCLUDE_PATH += \
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
	-I$(BCMCFG)/include \
	-I$(BCMPTM)/include \
	-I$(BCMPC)/include \
	-I$(BCMISSU)/include

# Import preprocessor flags avoiding include duplicates
TMP_SDK_CPPFLAGS := $(filter-out $(BCMSEC_INCLUDE_PATH),$(SDK_CPPFLAGS))
TMP_SDK_CPPFLAGS += $(BCMSEC_INCLUDE_PATH)
override SDK_CPPFLAGS := $(TMP_SDK_CPPFLAGS)
export SDK_CPPFLAGS

include $(SDK)/make/config.mk

include $(BCMSEC)/targets.mk

.PHONY: all $(CLEAN_TARGETS) $(BCMSEC_TARGETS)

ifneq (,$(filter $(MAKECMDGOALS),$(CLEAN_TARGETS)))
SDK_CLEAN = $(MAKECMDGOALS)
endif

all:
	$(MAKE) -C yaml
	$(MAKE) bcmseclibs

bcmseclibs: $(BCMSEC_TARGETS)

$(BCMSEC_TARGETS):
	@${ECHO} Processing BCMSEC subdirectory $@
	$(MAKE) -C $@ $(SDK_CLEAN)

$(CLEAN_TARGETS):: $(BCMSEC_TARGETS)
	$(MAKE) -C yaml $@

# Common developer targets
include $(SDK)/make/devtools.mk
