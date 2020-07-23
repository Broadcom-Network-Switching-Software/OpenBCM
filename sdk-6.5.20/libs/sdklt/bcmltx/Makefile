#
# This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
# 
# Copyright 2007-2020 Broadcom Inc. All rights reserved.
#
# LTX top-level makefile
#

# Default location for all SDK libraries
include $(SDK)/make/libpath.mk

# Default include path
LTX_INCLUDE_PATH = -I$(BCMLTX)/include

# Support libraries
LTX_INCLUDE_PATH += \
	-I$(SHR)/include \
	-I$(BCMCFG)/include \
	-I$(BCMLTD)/include \
	-I$(BCMLRD)/include \
	-I$(BCMDRD)/include \
	-I$(BCMTM)/include \
	-I$(BCMPC)/include \
	-I$(BCMPTM)/include \
	-I$(BSL)/include \
	-I$(SAL)/include \
	-I$(BCMTM)/include \
	-I$(BCMCTH)/include \
	-I$(BCMIMM)/include \
	-I$(BCMBD)/include \
	-I$(BCMSEC)/include \

# Import preprocessor flags avoiding include duplicates
TMP_SDK_CPPFLAGS := $(filter-out $(LTX_INCLUDE_PATH),$(SDK_CPPFLAGS))
TMP_SDK_CPPFLAGS += $(LTX_INCLUDE_PATH)
override SDK_CPPFLAGS := $(TMP_SDK_CPPFLAGS)
export SDK_CPPFLAGS

include $(SDK)/make/config.mk

include $(BCMLTX)/targets.mk

.PHONY: all $(CLEAN_TARGETS) $(LTX_TARGETS)

ifneq (,$(filter $(MAKECMDGOALS),$(CLEAN_TARGETS)))
SDK_CLEAN = $(MAKECMDGOALS)
endif

all:
	$(MAKE) ltxlibs

ltxlibs: $(LTX_TARGETS)

$(LTX_TARGETS):
	@${ECHO} Processing BCMLTX subdirectory $@
	$(MAKE) -C $@ $(SDK_CLEAN)

$(CLEAN_TARGETS):: $(LTX_TARGETS)

# Common developer targets
include $(SDK)/make/devtools.mk
