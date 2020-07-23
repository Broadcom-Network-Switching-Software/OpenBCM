#
# This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
# 
# Copyright 2007-2020 Broadcom Inc. All rights reserved.
#
# BCMLT top-level makefile
#

# Default location for all SDK libraries
include $(SDK)/make/libpath.mk

# Default include path
BCMLT_INCLUDE_PATH = -I$(BCMLT)/include

# Support libraries
BCMLT_INCLUDE_PATH += \
	-I$(BCMTRM)/include \
	-I$(BCMLTM)/include \
	-I$(SHR)/include \
	-I$(BSL)/include \
	-I$(BCMDRD)/include \
	-I$(BCMLTA)/include \
	-I$(BCMLRD)/include \
	-I$(BCMCFG)/include \
	-I$(BCMLTD)/include \
	-I$(BCMMGMT)/include \
	-I$(SAL)/include


# Import preprocessor flags avoiding include duplicates
TMP_SDK_CPPFLAGS := $(filter-out $(BCMLT_INCLUDE_PATH),$(SDK_CPPFLAGS))
TMP_SDK_CPPFLAGS += $(BCMLT_INCLUDE_PATH)
override SDK_CPPFLAGS := $(TMP_SDK_CPPFLAGS)
export SDK_CPPFLAGS

include $(SDK)/make/config.mk

include $(BCMLT)/targets.mk

.PHONY: all $(CLEAN_TARGETS) $(BCMLT_TARGETS)

ifneq (,$(filter $(MAKECMDGOALS),$(CLEAN_TARGETS)))
SDK_CLEAN = $(MAKECMDGOALS)
endif

all:
	$(MAKE) bcmltlibs

bcmltlibs: $(BCMLT_TARGETS)

$(BCMLT_TARGETS):
	@${ECHO} Processing BCMLT subdirectory $@
	$(MAKE) -C $@ $(SDK_CLEAN)

$(CLEAN_TARGETS):: $(BCMLT_TARGETS)

# Common developer targets
include $(SDK)/make/devtools.mk
