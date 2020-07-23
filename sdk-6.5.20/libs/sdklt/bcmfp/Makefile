#
# This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
# 
# Copyright 2007-2020 Broadcom Inc. All rights reserved.
#
# BCMFP top-level makefile
#

# Default location for all SDK libraries
include $(SDK)/make/libpath.mk

# Default include path
BCMFP_INCLUDE_PATH = -I$(BCMFP)/include

# Support libraries
BCMFP_INCLUDE_PATH += \
	-I$(BCMBD)/include \
	-I$(BCMDRD)/include \
	-I$(BCMLTD)/include \
	-I$(BCMLRD)/include \
	-I$(BCMLTM)/include \
	-I$(BCMIMM)/include \
	-I$(BCMMGMT)/include \
	-I$(BCMEVM)/include \
	-I$(BCMPTM)/include \
	-I$(BCMCTH)/include \
	-I$(BCMCFG)/include \
	-I$(BCMDI)/include \
	-I$(SHR)/include \
	-I$(BSL)/include \
	-I$(BCMISSU)/include \
	-I$(SAL)/include

# Import preprocessor flags avoiding include duplicates
TMP_SDK_CPPFLAGS := $(filter-out $(BCMFP_INCLUDE_PATH),$(SDK_CPPFLAGS))
TMP_SDK_CPPFLAGS += $(BCMFP_INCLUDE_PATH)
override SDK_CPPFLAGS := $(TMP_SDK_CPPFLAGS)
export SDK_CPPFLAGS

include $(SDK)/make/config.mk

include $(BCMFP)/targets.mk

.PHONY: all $(CLEAN_TARGETS) $(BCMFP_TARGETS)

ifneq (,$(filter $(MAKECMDGOALS),$(CLEAN_TARGETS)))
SDK_CLEAN = $(MAKECMDGOALS)
endif

all:
	$(MAKE) -C yaml
	$(MAKE) bcmfplibs

bcmfplibs: $(BCMFP_TARGETS)

$(BCMFP_TARGETS):
	@${ECHO} Processing BCMFP subdirectory $@
	$(MAKE) -C $@ $(SDK_CLEAN)

$(CLEAN_TARGETS):: $(BCMFP_TARGETS)
	$(MAKE) -C yaml $@


# Common developer targets
include $(SDK)/make/devtools.mk
