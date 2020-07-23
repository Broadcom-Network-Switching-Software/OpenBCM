#
# This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
# 
# Copyright 2007-2020 Broadcom Inc. All rights reserved.
#
# Build Linux kernel modules.
#

ifndef SDK
nosdk:; @echo 'The $$SDK environment variable is not set'; exit 1
else

# Default location to look for platform-specific make files
ifndef TARGET_PLATFORM_DIR
TARGET_PLATFORM_DIR = $(SDK)/appl/make
endif

ifndef TARGET_PLATFORM

help:
	@echo
	@echo Please specify a target platform, e.g.:
	@echo
	@echo make TARGET_PLATFORM=xlr_linux $(MAKECMDGOALS)
	@echo

clean: help

else

# Get build configuration for selected target
include $(TARGET_PLATFORM_DIR)/$(TARGET_PLATFORM).mk

# SDK make includes and tools
include $(SDK)/make/maketools.mk

# Default build directory
ifndef BLDDIR
BLDDIR = $(CURDIR)/build/$(TARGET_PLATFORM)
endif

# Default delivery directory
ifndef DSTDIR
DSTDIR = $(BLDDIR)
endif

# Configure build flags
override CPPFLAGS := $(CPPFLAGS) $(ADD_CPPFLAGS)
override CFLAGS := -fPIC $(filter-out -fPIC,$(CFLAGS)) $(ADD_CFLAGS)

# Support quiet make
ifneq (,$(findstring s,$(MAKEFLAGS)))
export ARFLAGS = rc
export Q = @
endif

# Export toolchain
export CC
export AR
export LD
export TOOLSDIR
export CROSS_COMPILE
export KDIR

# Configure SDK libraries
export SDK_BLDDIR = $(BLDDIR)
export SDK_DSTDIR = $(DSTDIR)
export SDK_CPPFLAGS = $(CPPFLAGS)
export SDK_CFLAGS = $(CFLAGS)

.PHONY: all issu

all: yaml_check
	$(MAKE) -C $(SDK)

# Include the yaml_check target
include $(SDK)/appl/make/yaml_check.mk

issu:
	$(MAKE) -C $(SDK) issu

clean cleanall distclean:
	$(MAKE) -C $(SDK) $@

# This target can be used to clean the SDK YAML components without
# rebuilding the entire library.
cleanyaml:
	$(RM) -f $(BLDDIR)/libbcmcfgyaml*

endif # TARGET_PLATFORM

.PHONY: help clean cleanall distclean cleanyaml

endif # defined $SDK
