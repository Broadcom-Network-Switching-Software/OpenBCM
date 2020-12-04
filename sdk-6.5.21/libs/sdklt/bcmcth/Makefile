#
# This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
# 
# Copyright 2007-2020 Broadcom Inc. All rights reserved.
#
# BCMCTH top-level makefile
#

# Default location for all SDK libraries
include $(SDK)/make/libpath.mk

# Default include path
BCMCTH_INCLUDE_PATH = -I$(BCMCTH)/include

# Support libraries
BCMCTH_INCLUDE_PATH += \
	-I$(BCMMGMT)/include \
	-I$(BCMLTD)/include \
	-I$(BCMLRD)/include \
	-I$(BCMPTM)/include \
	-I$(BCMLTM)/include \
	-I$(BCMEVM)/include \
	-I$(BCMIMM)/include \
	-I$(BCMBD)/include \
	-I$(BCMDRD)/include \
	-I$(BCMCFG)/include \
	-I$(BCMPC)/include \
	-I$(SHR)/include \
	-I$(BSL)/include \
	-I$(BCMISSU)/include \
	-I$(BCMTM)/include \
	-I$(BCMPKT)/include \
	-I$(SAL)/include

# Import preprocessor flags avoiding include duplicates
TMP_SDK_CPPFLAGS := $(filter-out $(BCMCTH_INCLUDE_PATH),$(SDK_CPPFLAGS))
TMP_SDK_CPPFLAGS += $(BCMCTH_INCLUDE_PATH)
override SDK_CPPFLAGS := $(TMP_SDK_CPPFLAGS)
export SDK_CPPFLAGS

include $(SDK)/make/config.mk

include $(BCMCTH)/targets.mk

.PHONY: all $(CLEAN_TARGETS) $(BCMCTH_TARGETS)

ifneq (,$(filter $(MAKECMDGOALS),$(CLEAN_TARGETS)))
SDK_CLEAN = $(MAKECMDGOALS)
endif

all:
	$(MAKE) -C yaml
	$(MAKE) bcmcthlibs

bcmcthlibs: $(BCMCTH_TARGETS)

$(BCMCTH_TARGETS):
	@${ECHO} Processing BCMCTH subdirectory $@
	$(MAKE) -C $@ $(SDK_CLEAN)

$(CLEAN_TARGETS):: $(BCMCTH_TARGETS)
	$(MAKE) -C yaml $@

# Common developer targets
include $(SDK)/make/devtools.mk
