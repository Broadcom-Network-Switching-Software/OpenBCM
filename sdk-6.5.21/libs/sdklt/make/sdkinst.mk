#
# This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
# 
# Copyright 2007-2020 Broadcom Inc. All rights reserved.
#
# Build consolidated static and shared libraries from individual
# component libraries.
#
# Output will be placed in the directory $(SDK_DSTDIR).
#
# Note that this make file will work only on Linux systems.
#

# Library names
LIBNAME = libsdklt
SDK_STATIC_LIB = $(LIBNAME).a
SDK_SHARED_LIB = $(LIBNAME).so

# Default build directory
ifndef SDK_BLDDIR
SDK_BLDDIR = $(SDK)/build
endif

# Temporary MRI file for building single static library
MRI = $(SDK_BLDDIR)/sdklt.mri

# Temporary directory for building shared library
TMPDIR = $(SDK_BLDDIR)/$(LIBNAME).tmp

# The Linux-specific libraries are organized differently than the SDK
# core libraries, so we handle them separately.
SDK_LINUX_TARGETS = bcmlu
SDK_INCS = $(filter-out $(SDK_LINUX_TARGETS),$(SDK_TARGETS))
LINUX_INCS = $(SDK_LINUX_TARGETS)

# Default installation base directory
ifndef SDK_DSTDIR
SDK_DSTDIR = $(SDK_BLDDIR)
endif

# Installation directories for libraries and header files.
SDK_LIBDIR = $(SDK_DSTDIR)/lib
SDK_INCDIR = $(SDK_DSTDIR)/include/sdklt

LIBSTATIC = $(SDK_LIBDIR)/$(SDK_STATIC_LIB)
LIBSHARED = $(SDK_LIBDIR)/$(SDK_SHARED_LIB)
LIBHEADER = $(SDK_INCDIR)/.sdklt

# Need to build sdklibs first for proper parallel build support
all:
	$(MAKE) sdklibs
	$(MAKE) install

install: $(LIBSTATIC) $(LIBSHARED) $(LIBHEADER)

# Make rules to build library collection (defines SDK_BLIBS)
include $(SDK)/make/sdklibs.mk

# Build static library from individual component libraries
$(LIBSTATIC): $(SDK_BLIBS)
	$(Q)echo "Building static library $(SDK_STATIC_LIB) ..."
	$(Q)(echo create $(SDK_STATIC_LIB); \
	     for l in $(SDK_LIBS); do \
	         echo addlib $$l; \
	     done; \
	     echo save; \
	     echo end) > $(MRI)
	$(Q)(cd $(SDK_BLDDIR); \
	     rm -f $(SDK_STATIC_LIB); \
	     $(AR) -M < $(MRI) || exit 1; \
	     rm -f $(MRI))
	$(Q)mkdir -p $(SDK_LIBDIR)
	$(Q)mv $(SDK_BLDDIR)/$(SDK_STATIC_LIB) $@

$(LIBSHARED): $(LIBSTATIC)
ifeq (,$(findstring fPIC,$(SDK_CFLAGS)))
	$(Q)echo "Skip building shared library (no -fPIC option in CFLAGS)"
else
	$(Q)echo "Building shared library $(SDK_SHARED_LIB) ..."
	$(Q)mkdir -p $(TMPDIR)
	$(Q)(cd $(TMPDIR); \
	     ar -x $(SDK_LIBDIR)/$(SDK_STATIC_LIB); \
	     $(CC) -shared $(SDK_CFLAGS) -o $(SDK_SHARED_LIB) *.o || exit 1; \
	     mv $(TMPDIR)/$(SDK_SHARED_LIB) $@; \
	     rm -rf $(TMPDIR))
endif

$(LIBHEADER): $(SDK_BLIBS)
	$(Q)echo "Building include directory ..."
	$(Q)rm -rf $(SDK_INCDIR)
	$(Q)mkdir -p $(SDK_INCDIR)
	$(Q)for l in $(SDK_INCS); do \
	        cp -dR $(SDK)/$$l/include/* $(SDK_INCDIR); \
	    done;
	$(Q)for l in $(LINUX_INCS); do \
	        cp -dR $(SDK)/linux/$$l/include/* $(SDK_INCDIR); \
	    done;
	$(Q)touch $@

clean::
	$(Q)rm -rf $(SDK_INCDIR)
	$(Q)rm -rf $(SDK_LIBDIR)

include $(SDK)/make/clean.mk

$(CLEAN_TARGETS):: sdklibs

cleanall:: clean

.PHONY: all install
