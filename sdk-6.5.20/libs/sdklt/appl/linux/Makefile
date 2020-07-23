#
# This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
# 
# Copyright 2007-2020 Broadcom Inc. All rights reserved.
#
# Build SDK Linux kernel modules.
#
# This make file requires a pointer to a make file with the relevant
# definitions for tool chain and Linux kernel sources.
#
# Please refer to $(SDK)/appl/make for examples.
#

ifndef SDK
nosdk:; @echo 'The $$SDK environment variable is not set'; exit 1
else

# BDE module
BDE = $(SDK)/linux/bde

# KNET module
KNET = $(SDK)/linux/knet

# KNET Callback module
KNETCB = $(SDK)/linux/knetcb

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

INCLUDES += -I$(CURDIR)

# We want our own includes before any system includes
CPPFLAGS := $(INCLUDES) $(CPPFLAGS) $(ADD_CPPFLAGS)
CFLAGS += $(ADD_CFLAGS)
LDFLAGS += $(ADD_LDFLAGS)

# Support quiet make
ifneq (,$(findstring s,$(MAKEFLAGS)))
export ARFLAGS = rc
export Q = @
quiet = quiet_
KBUILD_VERBOSE = 1
endif

# Export toolchain
export CC
export AR
export LD
export TOOLSDIR
export CROSS_COMPILE
export KDIR

# Check for Perl support
ifdef NO_PERL
export SDK_PERL   = n/a
endif

# Configure Linux kernel module build
BDE_BLDDIR = $(BLDDIR)/lkm/bde
KNET_BLDDIR = $(BLDDIR)/lkm/knet
KNETCB_BLDDIR = $(BLDDIR)/lkm/knetcb
export LKM_CPPFLAGS = $(CPPFLAGS)
export LKM_CFLAGS = $(CFLAGS)
export LKM_LDFLAGS = $(LDFLAGS)
export ADD_CPPFLAGS

.PHONY: all bde knet knetcb

all: bde knet knetcb

bde: $(BLDDIR)/.tree $(DSTDIR)/.tree
	$(MAKE) -C $(BDE) quiet=$(quiet) \
		LKM_BLDDIR=$(BDE_BLDDIR) KBUILD_VERBOSE=$(KBUILD_VERBOSE)
	(cd $(DSTDIR); ln -s -f $(BDE_BLDDIR)/*.ko ./)

knet: $(BLDDIR)/.tree  $(DSTDIR)/.tree bde
	$(MAKE) -C $(KNET) quiet=$(quiet) \
		LKM_BLDDIR=$(KNET_BLDDIR) KBUILD_VERBOSE=$(KBUILD_VERBOSE) \
		KBUILD_EXTRA_SYMBOLS=$(BDE_BLDDIR)/Module.symvers
	(cd $(DSTDIR); ln -s -f $(KNET_BLDDIR)/*.ko ./)

knetcb: $(BLDDIR)/.tree  $(DSTDIR)/.tree bde knet
	$(MAKE) -C $(KNETCB) quiet=$(quiet) \
		LKM_BLDDIR=$(KNETCB_BLDDIR) KBUILD_VERBOSE=$(KBUILD_VERBOSE) \
		KBUILD_EXTRA_SYMBOLS=$(KNET_BLDDIR)/Module.symvers
	(cd $(DSTDIR); ln -s -f $(KNETCB_BLDDIR)/*.ko ./)

$(BLDDIR)/.tree:
	@$(ECHO) 'Creating target directory $(BLDDIR)'
	@mkdir -p $(BLDDIR)
	@$(ECHO) "Target directory $(BLDDIR) created" > $@

ifneq ($(DSTDIR),$(BLDDIR))
$(DSTDIR)/.tree:
	@echo 'Creating target directory $(DSTDIR)'
	@mkdir -p $(DSTDIR)
	@echo "Target directory $(BLDDIR) created" > $@
endif

clean:
	$(MAKE) -C $(BDE) LKM_BLDDIR=$(BDE_BLDDIR) $@
	$(MAKE) -C $(KNET) LKM_BLDDIR=$(KNET_BLDDIR) $@
	$(MAKE) -C $(KNETCB) LKM_BLDDIR=$(KNETCB_BLDDIR) $@
	rm -f $(DSTDIR)/*.ko

endif # TARGET_PLATFORM

distclean:
	$(MAKE) -C $(BDE) LKM_BLDDIR=$(BDE_BLDDIR) $@
	$(MAKE) -C $(KNET) LKM_BLDDIR=$(KNET_BLDDIR) $@
	$(MAKE) -C $(KNETCB) LKM_BLDDIR=$(KNETCB_BLDDIR) $@

cleanall:: clean distclean

.PHONY: help clean cleanall distclean

endif # defined $SDK
