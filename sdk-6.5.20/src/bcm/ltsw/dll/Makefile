# $Id:$
# This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
# 
# Copyright 2007-2020 Broadcom Inc. All rights reserved.
#
# Makefile for BCM driver Library
#

include ${SDK}/make/Make.config

ifndef BCM_LTSW
BCM_LTSW = $(SDK)/src/bcm/ltsw
endif

BCM_LTSW_DLL = $(BCM_LTSW)/dll
BCM_LTSW_YAML = $(BCM_LTSW)/yaml

# Environmental variables
export ISSU_DB = $(BCM_LTSW_DLL)/db
export SDK_VERSION = $(SDK_VER)
export START_SDK_VER = $(SDK_VER_START)

ifeq ($(SDK_VERSION),)
VERSION_EXT =
DB_DIR = $(BCM_LTSW_DLL)/db/current
else
VERSION_EXT = -v $(SDK_VERSION)
DB_DIR = $(BCM_LTSW_DLL)/db/ver_$(SDK_VERSION)
endif

ifeq ($(START_SDK_VER),)
START_VER_EXT =
else
START_VER_EXT = -s $(START_SDK_VER)
endif

RM_DIRECTORY = $(sort $(dir $(wildcard $(BCM_LTSW_DLL)/db/*/*)))
ISSU_DB_GEN = $(SDKLT)/bcmha/scripts/issu_struct_db.py

ifneq (,$(findstring clean,$(MAKECMDGOALS)))
SDK_CLEAN = clean
endif

BCMISSU_DLL_TARGETS = main db

.PHONY: all clean $(BCMISSU_DLL_TARGETS) db_generate bcmissulibs

all: db_generate
	$(MAKE) bcmissulibs

bcmissulibs: $(BCMISSU_DLL_TARGETS)

$(BCMISSU_DLL_TARGETS):
	$(MAKE) -C $@ $(SDK_CLEAN)

db_generate:
	@${ECHO} Processing script $@
	python $(ISSU_DB_GEN) $(VERSION_EXT) $(START_VER_EXT)

clean:: $(BCMISSU_DLL_TARGETS)
