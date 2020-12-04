#
# This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
# 
# Copyright 2007-2020 Broadcom Inc. All rights reserved.
#
# LTP top-level makefile
#

# Default location for all SDK libraries
include $(SDK)/make/libpath.mk

# Default include path
LTP_INCLUDE_PATH = -I$(BCMLTP)/include

# Support libraries
LTP_INCLUDE_PATH += \
	-I$(SHR)/include \
	-I$(BCMLT)/include \
	-I$(BCMTRM)/include \
	-I$(BCMLTM)/include \
	-I$(BCMPTM)/include \
	-I$(BCMCFG)/include \
	-I$(BCMLRD)/include \
	-I$(BCMLTD)/include \
	-I$(BCMDRD)/include \
	-I$(BCMMGMT)/include \
	-I$(BSL)/include \
	-I$(SAL)/include

# Import preprocessor flags avoiding include duplicates
TMP_SDK_CPPFLAGS := $(filter-out $(LTP_INCLUDE_PATH),$(SDK_CPPFLAGS))
TMP_SDK_CPPFLAGS += $(LTP_INCLUDE_PATH)
override SDK_CPPFLAGS := $(TMP_SDK_CPPFLAGS)
export SDK_CPPFLAGS

include $(SDK)/make/config.mk

include $(BCMLTP)/targets.mk

.PHONY: all $(CLEAN_TARGETS) $(LTP_TARGETS)

ifneq (,$(filter $(MAKECMDGOALS),$(CLEAN_TARGETS)))
SDK_CLEAN = $(MAKECMDGOALS)
endif

all:
	$(MAKE) ltplibs

ltplibs: $(LTP_TARGETS)

$(LTP_TARGETS):
	@${ECHO} Processing BCMLTP subdirectory $@
	$(MAKE) -C $@ $(SDK_CLEAN)

$(CLEAN_TARGETS):: $(LTP_TARGETS)

# Common developer targets
include $(SDK)/make/devtools.mk
