#
# This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
# 
# Copyright 2007-2020 Broadcom Inc. All rights reserved.
#
# BCMISSU DLL top-level makefile
#

# Default location for all SDK libraries
include $(SDK)/make/libpath.mk

# Default include path
BCMISSU_INCLUDE_PATH = -I$(BCMISSU)/include -I./

BCMISSU_INCLUDE_PATH += $(COMP_INC_PATH)
# Support libraries
BCMISSU_INCLUDE_PATH += \
	-I$(BCMMGMT)/include \
	-I$(BCMLRD)/include \
	-I$(BCMLTD)/include \
	-I$(BCMBD)/include \
	-I$(BCMDRD)/include \
	-I$(BSL)/include \
	-I$(SHR)/include \
	-I$(SAL)/include


# Import preprocessor flags avoiding include duplicates
TMP_SDK_CPPFLAGS := $(filter-out $(BCMISSU_INCLUDE_PATH),$(SDK_CPPFLAGS))
TMP_SDK_CPPFLAGS += $(BCMISSU_INCLUDE_PATH)
TMP_SDK_CPPFLAGS += -fPIC
override SDK_CPPFLAGS := $(TMP_SDK_CPPFLAGS)
export SDK_CPPFLAGS
CFLAGS += -fPIC

BCMISSU_DLL = $(BCMISSU)/dll

include $(SDK)/make/config.mk

include $(BCMISSU_DLL)/targets.mk

ifeq ($(SDK_VERSION),)
VERSION_EXT =
DB_DIR = $(BCMISSU_DLL)/db/current
else
VERSION_EXT = -v $(SDK_VERSION)
DB_DIR = $(BCMISSU_DLL)/db/ver_$(SDK_VERSION)
endif

ifeq ($(START_SDK_VER),)
START_VER_EXT =
else
START_VER_EXT = -s $(START_SDK_VER)
endif

RM_DIRECTORY = $(sort $(dir $(wildcard $(BCMISSU)/db/*/*)))
ISSU_DB_GEN = $(BCMHA)/scripts/issu_struct_db.py
ISSU_LTID_GEN = $(BCMHA)/scripts/ltid_gen.py
PYTHON = python

.PHONY: all $(CLEAN_TARGETS) $(BCMISSU_DLL_TARGETS) db_generate bcmissulibs

ifneq (,$(filter $(MAKECMDGOALS),$(CLEAN_TARGETS)))
SDK_CLEAN = $(MAKECMDGOALS)
endif

all: db_generate
	$(MAKE) bcmissulibs

bcmissulibs: $(BCMISSU_DLL_TARGETS)

$(BCMISSU_DLL_TARGETS):
	@${ECHO} Processing BCMISSU subdirectory $@
	$(MAKE) -C $@ $(SDK_CLEAN) SDK_VERSION=$(SDK_VERSION) START_SDK_VER=$(START_SDK_VER)

# Use this target to build the ISSU DB for this version.
# Need to define the environment variables SDK_VERSION and START_SDK_VER.
# Both environment variables must be composed via alphanumeric
# and alpha characters which might be separated by dot.
# Example 3.2.17a. The version names will be automatically sorted (which will
# determine the order of the versions). For that reason, special characters
# can not be used and the user must understad that version 3.2.17a is prior
# to 3.2.17b.

include $(SDK)/make/makeutils.mk

ifneq ($(SDK_CHIPS),)
SDK_CHIPS_SD := $(call spc_sep,$(SDK_CHIPS))
INCLUDE_DEVICES = $(addprefix -c ,$(SDK_CHIPS_SD))
endif

ifneq ($(SDK_VARIANTS),)
SDK_VARIANTS_SD := $(call spc_sep,$(SDK_VARIANTS))
INCLUDE_VARIANTS = $(addprefix -t ,$(SDK_VARIANTS_SD))
endif

issu_db:
	@${ECHO} Processing script $@
	$(PYTHON) $(ISSU_LTID_GEN) -v $(SDK_VERSION) $(INCLUDE_DEVICES) $(INCLUDE_VARIANTS)

db_generate: issu_db
	@${ECHO} Processing script $@
	$(PYTHON) $(ISSU_LTID_GEN) -f -s $(START_SDK_VER) -v $(SDK_VERSION)
	$(PYTHON) $(ISSU_DB_GEN) $(VERSION_EXT) $(START_VER_EXT)

cleanall::
	@${ECHO} ISSU cleaning dir $(DB_DIR)
	rm -f $(DB_DIR)/*.h
	rm -f $(DB_DIR)/*.c

clean::
	@${ECHO} ISSU cleaning libraries
	rm -f $(SDK_LIBDIR)/libdll*.a
	rm -f $(SDK_LIBDIR)/lib/*
	rm -f $(SDK_OBJDIR)/dll/db/*
	rm -f $(SDK_OBJDIR)/dll/main/*

# Common developer targets
include $(SDK)/make/devtools.mk

