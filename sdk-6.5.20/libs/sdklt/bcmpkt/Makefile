#
# This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
# 
# Copyright 2007-2020 Broadcom Inc. All rights reserved.
#
# BCMPKT top-level makefile
#

# Default location for all SDK libraries
include $(SDK)/make/libpath.mk

# Default include path
BCMPKT_INCLUDE_PATH = -I$(BCMPKT)/include

# Support libraries
BCMPKT_INCLUDE_PATH += \
	-I$(SHR)/include \
	-I$(BSL)/include \
	-I$(BCMDRD)/include \
	-I$(BCMBD)/include \
	-I$(SAL)/include \
	-I$(BCMCNET)/include \
	-I$(BCMPTM)/include \
	-I$(BCMLTD)/include \
	-I$(BCMPC)/include \
	-I$(BCMCTH)/include \
	-I$(BCMLRD)/include

# Import preprocessor flags avoiding include duplicates
TMP_SDK_CPPFLAGS := $(filter-out $(BCMPKT_INCLUDE_PATH),$(SDK_CPPFLAGS))
TMP_SDK_CPPFLAGS += $(BCMPKT_INCLUDE_PATH)
override SDK_CPPFLAGS := $(TMP_SDK_CPPFLAGS)
export SDK_CPPFLAGS

include $(SDK)/make/config.mk

include $(BCMPKT)/targets.mk

.PHONY: all $(CLEAN_TARGETS) $(BCMPKT_TARGETS)

ifneq (,$(filter $(MAKECMDGOALS),$(CLEAN_TARGETS)))
SDK_CLEAN = $(MAKECMDGOALS)
endif

all:
	$(MAKE) bcmpktlibs

bcmpktlibs: $(BCMPKT_TARGETS)

$(BCMPKT_TARGETS):
	@${ECHO} Processing BCMPKT subdirectory $@
	$(MAKE) -C $@ $(SDK_CLEAN)

$(CLEAN_TARGETS):: $(BCMPKT_TARGETS)

# Common developer targets
include $(SDK)/make/devtools.mk
