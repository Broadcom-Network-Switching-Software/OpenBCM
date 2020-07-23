#
# This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
# 
# Copyright 2007-2020 Broadcom Inc. All rights reserved.
#
# SDKLT nplsim application.
#
# This make file builds the SDKLT library and a small application on
# top to exercise the logical table (LT) API on an NPL simulator.
#

ifndef SDK
nosdk:; @echo 'The $$SDK environment variable is not set'; exit 1
else

# Default location to look for platform-specific make files
ifndef TARGET_PLATFORM_DIR
TARGET_PLATFORM_DIR = $(SDK)/appl/make
endif

# Get build configuration
TARGET_PLATFORM = native_nplsim
include $(TARGET_PLATFORM_DIR)/$(TARGET_PLATFORM).mk

# This is where we put the build objects
ifndef BLDDIR
BLDDIR = $(CURDIR)/build/$(TARGET_PLATFORM)
endif

# This is where we deliver the executable
ifndef DSTDIR
DSTDIR = $(BLDDIR)
endif

# Export toolchain
export CC = gcc
export AR = ar

# Turn on warnings and debug symbols
CFLAGS += -Wall -Werror
CFLAGS += -g

# Always compile for shared library
CFLAGS += -fPIC

# Add default POSIX Threads flags
CFLAGS += -pthread

# Default YAML configuration
include $(SDK)/appl/make/yaml.mk
LDFLAGS += $(YAML_LDFLAGS)
EXTRA_LIBS += $(YAML_LDLIBS)

# We want our own includes before any system includes
CPPFLAGS := -I$(CURDIR) $(CPPFLAGS) $(ADD_CPPFLAGS)
CFLAGS += $(ADD_CFLAGS)
LDFLAGS += $(ADD_LDFLAGS)
EXTRA_LIBS += $(ADD_LDLIBS)

# Support quiet make (make -s)
ifneq (,$(findstring s,$(MAKEFLAGS)))
export ARFLAGS = rc
export Q=@
endif

# Export toolchain
export CC
export AR
export TOOLSDIR
export CROSS_COMPILE

# Build the SDK library from here
SDKLIB = $(SDK)/appl/sdklib

# Place the library build objects here
SDK_BLDDIR = $(SDKLIB)/build/$(TARGET_PLATFORM)

# Install SDK header files here
SDK_INCDIR = $(SDK_BLDDIR)/include/sdklt

# Install SDK library files here
SDK_LIBDIR = $(SDK_BLDDIR)/lib

# Main target
all: sdk
	$(MAKE) nplsim

#
# Configure main application build to incorporate the SDK include
# directories.
#
INCLUDES += -I$(SDK_INCDIR)
OBJS =	main.o version.o
BOBJS = $(addprefix $(BLDDIR)/,$(OBJS))

# Name and destination of executable
PRG = $(DSTDIR)/sdklt$(DSTSUFFIX)

.PHONY: all sdk nplsim

# Build SDK
sdk:
	$(MAKE) -C $(SDKLIB) TARGET_PLATFORM=$(TARGET_PLATFORM) \
		CPPFLAGS="$(CPPFLAGS)" CFLAGS="$(CFLAGS)"

nplsim: $(PRG)

# Link application objects
LDOBJS = $(BOBJS)

# Link SDK static library
LDLIBS += $(SDK_LIBDIR)/libsdklt.a

# Non-SDK libraries
LDLIBS += $(OPENSRC_LIBS) $(EXTRA_LIBS) $(BM_LIBS) -pthread -lrt -ldl -lm

$(PRG): $(BOBJS) $(DSTDIR)/.tree
	@echo 'Linking target application $@'
	$(CC) $(LDFLAGS) $(LDOBJS) $(LDLIBS) -o $@

# Dynamic build information
VERSION_CPPFLAGS += -DVERSION_INFO="$(shell echo \\\"`cat $(SDK)/RELEASE`\\\")"
VERSION_CPPFLAGS += -DDATE_INFO="$(shell echo \\\"`date -R`\\\")"
ifdef SCM_INFO_CMD
SCM_INFO = $(shell echo `$(SCM_INFO_CMD)`)
VERSION_CPPFLAGS += -DSCM_INFO="\"$(SCM_INFO)\""
endif

$(BLDDIR)/version.o: version.c $(BLDDIR)/.tree FORCE
	@echo 'Building application module $@ (forced)'
	$(CC) $(INCLUDES) $(CPPFLAGS) $(VERSION_CPPFLAGS) $(CFLAGS) -c $< -o $@
FORCE:

$(BLDDIR)/%.o: %.c $(BLDDIR)/.tree
	@echo 'Building application module $@'
	$(CC) $(INCLUDES) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

$(BLDDIR)/.tree:
	@echo 'Creating build directory $(BLDDIR)'
	@mkdir -p $(BLDDIR)
	@echo "Build directory $(BLDDIR) created" > $@

ifneq ($(DSTDIR),$(BLDDIR))
$(DSTDIR)/.tree:
	@echo 'Creating target directory $(DSTDIR)'
	@mkdir -p $(DSTDIR)
	@echo "Target directory $(BLDDIR) created" > $@
endif

clean::
	$(MAKE) -C $(SDKLIB) TARGET_PLATFORM=$(TARGET_PLATFORM) clean
	rm -f $(PRG) $(BOBJS)

cleanall:: clean

# This target can be used to clean the SDK YAML components without
# rebuilding the entire library.
cleanyaml:
	$(MAKE) -C $(SDKLIB) TARGET_PLATFORM=$(TARGET_PLATFORM) cleanyaml

cleanall:: clean

.PHONY: help clean cleanall cleanyaml

endif # defined $SDK
