#
# This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
# 
# Copyright 2007-2020 Broadcom Inc. All rights reserved.
#
# BCMLM top-level makefile
#

# Default location for all SDK libraries
include $(SDK)/make/libpath.mk

# Default include path
BCMLM_INCLUDE_PATH = -I$(BCMLM)/include

# Support libraries
BCMLM_INCLUDE_PATH += \
	-I$(SHR)/include \
	-I$(BSL)/include \
	-I$(SAL)/include \
	-I$(BCMIMM)/include \
	-I$(BCMDRD)/include \
	-I$(BCMLTD)/include \
	-I$(BCMBD)/include \
	-I$(BCMMGMT)/include \
	-I$(BCMEVM)/include \
	-I$(BCMLT)/include \
	-I$(BCMLRD)/include \
	-I$(BCMLTM)/include \
	-I$(BCMPC)/include \
	-I$(BCMEVM)/include \
	-I$(BCMISSU)/include

# Import preprocessor flags avoiding include duplicates
TMP_SDK_CPPFLAGS := $(filter-out $(BCMLM_INCLUDE_PATH),$(SDK_CPPFLAGS))
TMP_SDK_CPPFLAGS += $(BCMLM_INCLUDE_PATH)
override SDK_CPPFLAGS := $(TMP_SDK_CPPFLAGS)
export SDK_CPPFLAGS

include $(SDK)/make/config.mk

include $(BCMLM)/targets.mk

.PHONY: all $(CLEAN_TARGETS) $(BCMLM_TARGETS)

ifneq (,$(filter $(MAKECMDGOALS),$(CLEAN_TARGETS)))
SDK_CLEAN = $(MAKECMDGOALS)
endif

all:
	$(MAKE) -C yaml
	$(MAKE) bcmlmlibs

bcmlmlibs: $(BCMLM_TARGETS)

$(BCMLM_TARGETS):
	@${ECHO} Processing BCMLM subdirectory $@
	$(MAKE) -C $@ $(SDK_CLEAN)

$(CLEAN_TARGETS):: $(BCMLM_TARGETS)

# Common developer targets
include $(SDK)/make/devtools.mk
