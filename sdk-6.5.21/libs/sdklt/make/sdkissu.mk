#
# This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
# 
# Copyright 2007-2020 Broadcom Inc. All rights reserved.
#
# Build shared ISSU library.
#
# Objects will be placed in the directory $(SDK_BLDDIR).
# Library will be placed in the directory $(ISSU_LIBDIR).
#
# Note that this make file will work only on Linux systems.
#

ifneq ($(SDK_VERSION),)
ISSU_LIB_VER = $(SDK_VERSION)
endif

# Library names
ISSU_LIBNAME = libsdkissu
ISSU_STATIC_LIB = $(ISSU_LIBNAME).a
ISSU_SONAME = $(ISSU_LIBNAME).so
ISSU_SHARED_LIB = $(ISSU_SONAME).$(ISSU_LIB_VER)

# Default build directory
ifeq (,$(SDK_BLDDIR))
SDK_BLDDIR = $(SDK)/build
endif

# Default installation base directory
ifeq (,$(SDK_DSTDIR))
SDK_DSTDIR = $(SDK_BLDDIR)
endif

# Temporary MRI file for building single static library
ISSU_MRI = $(SDK_BLDDIR)/sdkissu.mri

# Temporary directory for building shared library
ISSU_TMPDIR = $(SDK_BLDDIR)/$(ISSU_LIBNAME).tmp

# Installation directory for library.
ISSU_LIBDIR = $(SDK_DSTDIR)/lib

ISSU_LIBSTATIC = $(ISSU_LIBDIR)/$(ISSU_STATIC_LIB)
ISSU_LIBSHARED = $(ISSU_LIBDIR)/$(ISSU_SHARED_LIB)

ifndef BCMISSU
include $(SDK)/make/libpath.mk
endif

BCMISSU_DLL = $(BCMISSU)/dll

# Need to build issulibs first for proper parallel build support
issu:
	@echo Building ISSU DLL - $(BCMISSU_DLL)
	$(MAKE) bcmissudll
	$(MAKE) issuinst

include $(BCMISSU_DLL)/targets.mk

cleanall:: bcmissudll

bcmissudll:
	@echo Building ISSUDLL from top - $(BCMISSU)
	$(MAKE) -C $(BCMISSU_DLL) $(SDK_CLEAN) \
		SDK_DSTDIR=$(SDK_DSTDIR) SDK_BLDDIR=$(SDK_BLDDIR)

# Targets with full build path
ISSU_BLIBS = $(addprefix $(SDK_BLDDIR)/,$(BCMISSU_DLL_LIBS))

issuinst: $(ISSU_LIBSTATIC) $(ISSU_LIBSHARED)

# Build static library from individual component libraries
$(ISSU_LIBSTATIC): $(ISSU_BLIBS)
	$(Q)echo "Building static library $(ISSU_STATIC_LIB) ..."
	$(Q)(echo create $(ISSU_STATIC_LIB); \
	     for l in $(BCMISSU_DLL_LIBS); do \
	         echo addlib $$l; \
	     done; \
	     echo save; \
	     echo end) > $(ISSU_MRI)
	$(Q)(cd $(SDK_BLDDIR); \
	     rm -f $(ISSU_STATIC_LIB); \
	     $(AR) -M < $(ISSU_MRI); \
	     rm -f $(ISSU_MRI))
	$(Q)mkdir -p $(ISSU_LIBDIR)
	$(Q)mv $(SDK_BLDDIR)/$(ISSU_STATIC_LIB) $@

$(ISSU_LIBSHARED): $(ISSU_LIBSTATIC)
	$(Q)echo "Building shared ISSU library $(ISSU_SHARED_LIB) ..."
	$(Q)mkdir -p $(ISSU_TMPDIR)
	$(Q)(cd $(ISSU_TMPDIR); \
	     ar -x $(ISSU_LIBDIR)/$(ISSU_STATIC_LIB); \
	     $(CC) -shared $(SDK_CFLAGS) -Wl,-soname,$(ISSU_SONAME) \
	           -o $(ISSU_SHARED_LIB) *.o -lc; \
	     mv $(ISSU_TMPDIR)/$(ISSU_SHARED_LIB) $@; \
	     rm -rf $(ISSU_TMPDIR))

.PHONY: issu bcmissudll issuinst

