#
# This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
# 
# Copyright 2007-2020 Broadcom Inc. All rights reserved.
#
# BCMECMP top-level makefile.
#

# Default location for all SDK libraries
include $(SDK)/make/libpath.mk

# Default include path
BCMECMP_INCLUDE_PATH = -I$(BCMECMP)/include

# Support libraries
BCMECMP_INCLUDE_PATH += \
	-I$(SHR)/include \
	-I$(BCMLTD)/include \
	-I$(BCMMGMT)/include \
	-I$(BCMDRD)/include \
	-I$(BCMBD)/include \
	-I$(BCMLRD)/include \
	-I$(BCMPTM)/include \
	-I$(BCMLTM)/include \
	-I$(BCMCFG)/include \
	-I$(BSL)/include \
	-I$(BCMIMM)/include \
	-I$(BCMISSU)/include \
	-I$(SAL)/include

# Import preprocessor flags avoiding include duplicates
TMP_SDK_CPPFLAGS := $(filter-out $(BCMECMP_INCLUDE_PATH),$(SDK_CPPFLAGS))
TMP_SDK_CPPFLAGS += $(BCMECMP_INCLUDE_PATH)
override SDK_CPPFLAGS := $(TMP_SDK_CPPFLAGS)
export SDK_CPPFLAGS

include $(SDK)/make/config.mk

include $(BCMECMP)/targets.mk

.PHONY: all $(CLEAN_TARGETS) $(BCMECMP_TARGETS)

ifneq (,$(filter $(MAKECMDGOALS),$(CLEAN_TARGETS)))
SDK_CLEAN = $(MAKECMDGOALS)
endif

all:
	$(MAKE) -C yaml
	$(MAKE) bcmecmplibs

bcmecmplibs: $(BCMECMP_TARGETS)

$(BCMECMP_TARGETS):
	@${ECHO} Processing BCMECMP subdirectory $@
	$(MAKE) -C $@ $(SDK_CLEAN)

$(CLEAN_TARGETS):: $(BCMECMP_TARGETS)
	$(MAKE) -C yaml $@

# Common developer targets
include $(SDK)/make/devtools.mk
