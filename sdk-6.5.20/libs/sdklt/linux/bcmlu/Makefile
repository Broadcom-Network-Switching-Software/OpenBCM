#
# This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
# 
# Copyright 2007-2020 Broadcom Inc. All rights reserved.
#
# BCMLU top-level makefile
#

# Default location for all SDK libraries
include $(SDK)/make/libpath.mk

# Default include path
BCMLU_INCLUDE_PATH = -I$(BCMLU)/include -I$(SDK)/linux/include -I$(SDK)/linux/include/bcmpkt

# Support libraries
BCMLU_INCLUDE_PATH += \
	-I$(SHR)/include \
	-I$(BSL)/include \
	-I$(SAL)/include \
	-I$(BCMDRD)/include \
	-I$(BCMPKT)/include \
	-I$(BCMCNET)/include \
	-I$(BCMLTD)/include \
	-I$(BCMLRD)/include \
	-I$(SDK)/linux/knet/include

# Import preprocessor flags avoiding include duplicates
TMP_SDK_CPPFLAGS := $(filter-out $(BCMLU_INCLUDE_PATH),$(SDK_CPPFLAGS))
TMP_SDK_CPPFLAGS += $(BCMLU_INCLUDE_PATH)
# Support pedantic build
TMP_SDK_CPPFLAGS += -D_GNU_SOURCE=1
override SDK_CPPFLAGS := $(TMP_SDK_CPPFLAGS)

export SDK_CPPFLAGS

include $(SDK)/make/config.mk

include $(BCMLU)/targets.mk

.PHONY: all $(CLEAN_TARGETS) $(BCMLU_TARGETS)

ifneq (,$(filter $(MAKECMDGOALS),$(CLEAN_TARGETS)))
SDK_CLEAN = $(MAKECMDGOALS)
endif

all:
	$(MAKE) bcmlulibs

bcmlulibs: $(BCMLU_TARGETS)

$(BCMLU_TARGETS):
	@${ECHO} Processing BCMLU subdirectory $@
	$(MAKE) -C $@ $(SDK_CLEAN)

$(CLEAN_TARGETS):: $(BCMLU_TARGETS)

# Common developer targets
include $(SDK)/make/devtools.mk
