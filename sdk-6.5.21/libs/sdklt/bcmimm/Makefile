#
# This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
# 
# Copyright 2007-2020 Broadcom Inc. All rights reserved.
#
# INMEM top-level makefile
#

# Default location for all SDK libraries
include $(SDK)/make/libpath.mk

# Default include path
INMEM_INCLUDE_PATH = -I$(BCMIMM)/include

# Support libraries
INMEM_INCLUDE_PATH += \
	-I$(SHR)/include \
	-I$(BCMLTD)/include \
	-I$(BCMLRD)/include \
	-I$(BCMDRD)/include \
	-I$(BCMMGMT)/include \
	-I$(BCMISSU)/include \
	-I$(BCMLTM)/include \
	-I$(BCMEVM)/include \
	-I$(BCMCFG)/include \
	-I$(BSL)/include \
	-I$(SAL)/include

# Import preprocessor flags avoiding include duplicates
TMP_SDK_CPPFLAGS := $(filter-out $(INMEM_INCLUDE_PATH),$(SDK_CPPFLAGS))
TMP_SDK_CPPFLAGS += $(INMEM_INCLUDE_PATH) -DBCMLTA_SUPPORTS_TRANS
override SDK_CPPFLAGS := $(TMP_SDK_CPPFLAGS)
export SDK_CPPFLAGS

include $(SDK)/make/config.mk

include $(BCMIMM)/targets.mk

.PHONY: all $(CLEAN_TARGETS) $(BCMIMM_TARGETS)

ifneq (,$(filter $(MAKECMDGOALS),$(CLEAN_TARGETS)))
SDK_CLEAN = $(MAKECMDGOALS)
endif

all:
	$(MAKE) -C yaml
	$(MAKE) immlibs

immlibs: $(BCMIMM_TARGETS)

$(BCMIMM_TARGETS):
	@${ECHO} Processing BCMIMM subdirectory $@
	$(MAKE) -C $@ $(SDK_CLEAN)

$(CLEAN_TARGETS):: $(BCMIMM_TARGETS)
	$(MAKE) -C yaml $@

# Common developer targets
include $(SDK)/make/devtools.mk
