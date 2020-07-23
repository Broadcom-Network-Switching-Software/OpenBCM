#
# This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
# 
# Copyright 2007-2020 Broadcom Inc. All rights reserved.
#
# BCMPTM top-level makefile
#

# Default location for all SDK libraries
include $(SDK)/make/libpath.mk

# Default include path
BCMPTM_INCLUDE_PATH = -I$(BCMPTM)/include

# Support libraries
BCMPTM_INCLUDE_PATH += \
	-I$(BCMBD)/include \
	-I$(BCMDRD)/include \
	-I$(BCMLTD)/include \
	-I$(BCMLRD)/include \
	-I$(BCMLTM)/include \
	-I$(BCMMGMT)/include \
	-I$(BCMISSU)/include \
	-I$(BCMPC)/include \
	-I$(SHR)/include \
	-I$(BSL)/include \
	-I$(SAL)/include \
	-I$(BCMCFG)/include \
	-I$(BCMLRD)/include \
	-I$(BCMIMM)/include \
	-I$(BCMEVM)/include \
	-I$(BCMDI)/include  \
	-I$(BCMSEC)/include  \
	-I$(BCMPTM)/chip

# Import preprocessor flags avoiding include duplicates
TMP_SDK_CPPFLAGS := $(filter-out $(BCMPTM_INCLUDE_PATH),$(SDK_CPPFLAGS))
TMP_SDK_CPPFLAGS += $(BCMPTM_INCLUDE_PATH)
override SDK_CPPFLAGS := $(TMP_SDK_CPPFLAGS)
export SDK_CPPFLAGS

include $(SDK)/make/config.mk

include $(BCMPTM)/targets.mk

.PHONY: all $(CLEAN_TARGETS) $(BCMPTM_TARGETS)

ifneq (,$(filter $(MAKECMDGOALS),$(CLEAN_TARGETS)))
SDK_CLEAN = $(MAKECMDGOALS)
endif

all:
	$(MAKE) -C yaml
	$(MAKE) bcmptmlibs

bcmptmlibs: $(BCMPTM_TARGETS)

$(BCMPTM_TARGETS):
	@${ECHO} Processing BCMPTM subdirectory $@
	$(MAKE) -C $@ $(SDK_CLEAN)

$(CLEAN_TARGETS):: $(BCMPTM_TARGETS)
	$(MAKE) -C yaml $@

# Common developer targets
include $(SDK)/make/devtools.mk
