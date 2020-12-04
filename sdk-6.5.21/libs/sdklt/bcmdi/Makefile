#
# This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
# 
# Copyright 2007-2020 Broadcom Inc. All rights reserved.
#
# BCMDI top-level makefile
#

# Default location for all SDK libraries
include $(SDK)/make/libpath.mk

# Default include path
BCMDI_INCLUDE_PATH = -I$(BCMDI)/include

# Support libraries
BCMDI_INCLUDE_PATH += \
	-I$(BCMPTM)/include \
	-I$(BCMBD)/include \
	-I$(BCMPC)/include \
	-I$(BCMDRD)/include \
	-I$(BCMLTA)/include \
	-I$(BCMLRD)/include \
	-I$(BCMLTD)/include \
	-I$(BCMMGMT)/include \
	-I$(BCMIMM)/include \
	-I$(BCMISSU)/include \
	-I$(BCMCFG)/include \
	-I$(SHR)/include \
	-I$(BSL)/include \
	-I$(SAL)/include

# Import preprocessor flags avoiding include duplicates
TMP_SDK_CPPFLAGS := $(filter-out $(BCMDI_INCLUDE_PATH),$(SDK_CPPFLAGS))
TMP_SDK_CPPFLAGS += $(BCMDI_INCLUDE_PATH)
override SDK_CPPFLAGS := $(TMP_SDK_CPPFLAGS)
export SDK_CPPFLAGS

include $(SDK)/make/config.mk

include $(BCMDI)/targets.mk

.PHONY: all $(CLEAN_TARGETS) $(BCMDI_TARGETS)

ifneq (,$(filter $(MAKECMDGOALS),$(CLEAN_TARGETS)))
SDK_CLEAN = $(MAKECMDGOALS)
endif

all:
	$(MAKE) -C yaml
	$(MAKE) bcmdilibs

bcmdilibs: $(BCMDI_TARGETS)

$(BCMDI_TARGETS):
	@${ECHO} Processing BCMDI subdirectory $@
	$(MAKE) -C $@ $(SDK_CLEAN)

$(CLEAN_TARGETS):: $(BCMDI_TARGETS)
	$(MAKE) -C yaml $@

# Common developer targets
include $(SDK)/make/devtools.mk
