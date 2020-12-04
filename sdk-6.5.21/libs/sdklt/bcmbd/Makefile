#
# This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
# 
# Copyright 2007-2020 Broadcom Inc. All rights reserved.
#
# BCMBD top-level makefile
#

# Default location for all SDK libraries
include $(SDK)/make/libpath.mk

# Default include path
BCMBD_INCLUDE_PATH = -I$(BCMBD)/include

# Support libraries
BCMBD_INCLUDE_PATH += \
	-I$(BCMLTD)/include \
	-I$(BCMLRD)/include \
	-I$(BCMDRD)/include \
	-I$(BCMCFG)/include \
	-I$(BCMMGMT)/include \
	-I$(SHR)/include \
	-I$(BSL)/include \
	-I$(SAL)/include

# Import preprocessor flags avoiding include duplicates
TMP_SDK_CPPFLAGS := $(filter-out $(BCMBD_INCLUDE_PATH),$(SDK_CPPFLAGS))
TMP_SDK_CPPFLAGS += $(BCMBD_INCLUDE_PATH)
override SDK_CPPFLAGS := $(TMP_SDK_CPPFLAGS)
export SDK_CPPFLAGS

include $(SDK)/make/config.mk

include $(BCMBD)/targets.mk

.PHONY: all $(CLEAN_TARGETS) $(BCMBD_TARGETS)

ifneq (,$(filter $(MAKECMDGOALS),$(CLEAN_TARGETS)))
SDK_CLEAN = $(MAKECMDGOALS)
endif

all:
	$(MAKE) bcmbdlibs

bcmbdlibs: $(BCMBD_TARGETS)

$(BCMBD_TARGETS):
	@${ECHO} Processing BCMBD subdirectory $@
	$(MAKE) -C $@ $(SDK_CLEAN)

$(CLEAN_TARGETS):: $(BCMBD_TARGETS)

# Common developer targets
include $(SDK)/make/devtools.mk
