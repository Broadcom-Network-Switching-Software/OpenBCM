#
# This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
# 
# Copyright 2007-2020 Broadcom Inc. All rights reserved.
#
# BCMISSU top-level makefile
#

# Default location for all SDK libraries
include $(SDK)/make/libpath.mk

# Default include path
BCMISSU_INCLUDE_PATH = -I$(BCMISSU)/include -I./

# Support libraries
BCMISSU_INCLUDE_PATH += \
	-I$(BCMMGMT)/include \
	-I$(BCMDRD)/include \
	-I$(BCMLRD)/include \
	-I$(BCMLTD)/include \
	-I$(BSL)/include \
	-I$(SHR)/include \
	-I$(SAL)/include


# Import preprocessor flags avoiding include duplicates
TMP_SDK_CPPFLAGS := $(filter-out $(BCMISSU_INCLUDE_PATH),$(SDK_CPPFLAGS))
TMP_SDK_CPPFLAGS += $(BCMISSU_INCLUDE_PATH)
override SDK_CPPFLAGS := $(TMP_SDK_CPPFLAGS)
export SDK_CPPFLAGS

include $(SDK)/make/config.mk

include $(BCMISSU)/targets.mk

.PHONY: all libsum FORCE $(CLEAN_TARGETS) doc $(BCMISSU_TARGETS)

ifneq (,$(filter $(MAKECMDGOALS),$(CLEAN_TARGETS)))
SDK_CLEAN = $(MAKECMDGOALS)
endif

# Tools for libsum header file
GDIFF = diff -q -N

all: libsum
	$(MAKE) bcmissumainlibs

bcmissumainlibs: $(BCMISSU_TARGETS)

$(BCMISSU_TARGETS):
	@${ECHO} Processing BCMISSU subdirectory $@
	$(MAKE) -C $@ $(SDK_CLEAN)

$(CLEAN_TARGETS):: $(BCMISSU_TARGETS)
	$(MAKE) -C dll $(SDK_CLEAN)

# Target for generating library checksum header file
LIBSUM_DIR := $(BCMISSU)/include/bcmissu/generated
LIBSUM_HDR := $(LIBSUM_DIR)/bcmissu_libsum.h
LIBSUM_FILE := $(SDK_BLDDIR)/$(SDK_LIBSUM)
LIBSUM_TMP := $(SDK_BLDDIR)/libsum.tmp
LIBSUM_STR = $(shell cat $(LIBSUM_FILE) 2>/dev/null)

libsum: $(LIBSUM_HDR)

$(LIBSUM_HDR): $(LIBSUM_FILE)
	@$(ECHO) Generating ISSU checksum file
	$(MKDIR) $(LIBSUM_DIR)
	$(ECHO) "#define BCMISSU_LIBSUM_STR \"$(LIBSUM_STR)\""  > $(LIBSUM_TMP)
	@if ! $(GDIFF) $(LIBSUM_TMP) $@ >/dev/null 2>&1; then \
		echo "Installing $(notdir $@)"; \
		mv -f $(LIBSUM_TMP) $@; fi
	rm -f $(LIBSUM_TMP)

$(LIBSUM_FILE): $(SDK_BLDDIR)/.tree FORCE
	$(MAKE) -f $(SDK)/make/libsum.mk

FORCE:

cleanall::
	$(RM) $(LIBSUM_DIR)/*
	$(MAKE) -f $(SDK)/make/libsum.mk $@

# Common developer targets
include $(SDK)/make/devtools.mk
