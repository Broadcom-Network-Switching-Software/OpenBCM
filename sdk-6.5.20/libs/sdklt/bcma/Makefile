#
# This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
# 
# Copyright 2007-2020 Broadcom Inc. All rights reserved.
#
# BCMA top-level makefile
#

# Default location for all SDK libraries
include $(SDK)/make/libpath.mk

# Default include path
BCMA_INCLUDE_PATH = -I$(BCMA)/include

# Support libraries
BCMA_INCLUDE_PATH += \
	-I$(BCMLU)/include \
	-I$(SIM)/include \
	-I$(BCMLT)/include \
	-I$(BCMMGMT)/include \
	-I$(BCMTRM)/include \
	-I$(BCMLTM)/include \
	-I$(BCMLTD)/include \
	-I$(BCMLRD)/include \
	-I$(BCMCFG)/include \
	-I$(BCMPTM)/include \
	-I$(BCMPMAC)/include \
	-I$(BCMBD)/include \
	-I$(BCMPC)/include \
	-I$(BCMTM)/include \
	-I$(BCMHA)/include \
	-I$(BCMDRD)/include \
	-I$(BCMDI)/include \
	-I$(SHR)/include \
	-I$(BSL)/include \
	-I$(SAL)/include \
	-I$(BCMPKT)/include \
	-I$(PHYMOD)/include \
	-I$(PCIEPHY)/include

# Import preprocessor flags avoiding include duplicates
TMP_SDK_CPPFLAGS := $(filter-out $(BCMA_INCLUDE_PATH),$(SDK_CPPFLAGS))
TMP_SDK_CPPFLAGS += $(BCMA_INCLUDE_PATH)
override SDK_CPPFLAGS := $(TMP_SDK_CPPFLAGS)
export SDK_CPPFLAGS

include $(SDK)/make/config.mk

include $(BCMA)/targets.mk

.PHONY: all $(CLEAN_TARGETS) $(BCMA_TARGETS)

ifneq (,$(filter $(MAKECMDGOALS),$(CLEAN_TARGETS)))
SDK_CLEAN = $(MAKECMDGOALS)
endif

all:
	$(MAKE) bcmalibs

bcmalibs: $(BCMA_TARGETS)

$(BCMA_TARGETS):
	@${ECHO} Processing BCMA subdirectory $@
	$(MAKE) -C $@ $(SDK_CLEAN)

$(CLEAN_TARGETS):: $(BCMA_TARGETS)

# Common developer targets
include $(SDK)/make/devtools.mk
