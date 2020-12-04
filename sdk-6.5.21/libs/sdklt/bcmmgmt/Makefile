#
# This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
# 
# Copyright 2007-2020 Broadcom Inc. All rights reserved.
#
# BCMMGMT top-level makefile
#

# Default location for all SDK libraries
include $(SDK)/make/libpath.mk

# Default include path
BCMMGMT_INCLUDE_PATH = -I$(BCMMGMT)/include -I.

# Support libraries
BCMMGMT_INCLUDE_PATH += \
	-I$(BCMLT)/include \
	-I$(BCMTRM)/include \
	-I$(BCMDRD)/include \
	-I$(BCMLRD)/include \
	-I$(BCMBD)/include \
	-I$(BCMLTD)/include \
	-I$(BCMMGMT)/include \
	-I$(BCMLM)/include \
	-I$(BCMCFG)/include \
	-I$(SHR)/include \
	-I$(BCMLTM)/include \
	-I$(BCMPTM)/include \
	-I$(BCMDI)/include \
	-I$(BCMFP)/include \
	-I$(BCMIMM)/include \
	-I$(BCMPKT)/include \
	-I$(BCMECMP)/include \
	-I$(BCMLTP)/include \
	-I$(BCMPC)/include \
	-I$(BCMTM)/include \
	-I$(BCMEVM)/include \
	-I$(BCMCTH)/include \
	-I$(BCMISSU)/include \
	-I$(BCMSEC)/include \
	-I$(BCMHA)/include \
	-I$(BSL)/include \
	-I$(SAL)/include

# Import preprocessor flags avoiding include duplicates
TMP_SDK_CPPFLAGS := $(filter-out $(BCMMGMT_INCLUDE_PATH),$(SDK_CPPFLAGS))
TMP_SDK_CPPFLAGS += $(BCMMGMT_INCLUDE_PATH)
override SDK_CPPFLAGS := $(TMP_SDK_CPPFLAGS)
export SDK_CPPFLAGS

include $(SDK)/make/config.mk

include $(BCMMGMT)/targets.mk

.PHONY: all $(CLEAN_TARGETS) $(BCMMGMT_TARGETS)

ifneq (,$(filter $(MAKECMDGOALS),$(CLEAN_TARGETS)))
SDK_CLEAN = $(MAKECMDGOALS)
endif

all:
	$(MAKE) bcmmgmtlibs

bcmmgmtlibs: $(BCMMGMT_TARGETS)

$(BCMMGMT_TARGETS):
	@${ECHO} Processing BCMMGMT subdirectory $@
	$(MAKE) -C $@ $(SDK_CLEAN)

$(CLEAN_TARGETS):: $(BCMMGMT_TARGETS)

# Common developer targets
include $(SDK)/make/devtools.mk
