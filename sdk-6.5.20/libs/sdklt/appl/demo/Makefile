#
# This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
# 
# Copyright 2007-2020 Broadcom Inc. All rights reserved.
#
# SDKLT demo application.
#
# This make file builds the SDKLT library and a small application on
# top to exercise the logical table (LT) API for switch device
# management.
#
# By default the make file builds the SDK library, the required Linux
# kernel modules and the demo application. Once the SDK library has
# been built, the kernel modules and the application can be built
# separately using the 'kmod' and 'demo' make targets.
#

ifndef SDK
nosdk:; @echo 'The $$SDK environment variable is not set'; exit 1
else

# Default location to look for platform-specific make files
ifndef TARGET_PLATFORM_DIR
TARGET_PLATFORM_DIR = $(SDK)/appl/make
endif

TARGET_PLATFORM_MAKEFILES += $(wildcard $(TARGET_PLATFORM_DIR)/*sim.mk)
TARGET_PLATFORM_MAKEFILES += $(wildcard $(TARGET_PLATFORM_DIR)/*linux.mk)
TARGET_PLATFORMS = $(basename $(notdir $(TARGET_PLATFORM_MAKEFILES)))

ifndef TARGET_PLATFORM

help:
	@echo
	@echo Broadcom SDKLT demo application makefile.
	@echo
	@echo Please specify a target platform, e.g.:
	@echo
	@echo make TARGET_PLATFORM=native_thsim $(MAKECMDGOALS)
	@echo
	@echo Available target platforms:
	@echo $(TARGET_PLATFORMS)
	@echo

clean: help

else

# Get build configuration for selected target
include $(TARGET_PLATFORM_DIR)/$(TARGET_PLATFORM).mk

# Customize build directory name
ifndef BUILD
BUILD = build
endif

# This is where we put the build objects
ifndef BLDDIR
BLDDIR = $(CURDIR)/$(BUILD)/$(TARGET_PLATFORM)
endif

# This is where we deliver the executable
ifndef DSTDIR
DSTDIR = $(BLDDIR)
endif

# Turn on warnings and debug symbols
CFLAGS += -Wall -Werror
CFLAGS += -g

# Always compile for shared library
CFLAGS += -fPIC

# Add default POSIX Threads flags
CFLAGS += -pthread

# Default optimization
ifndef OPT_CFLAGS
OPT_CFLAGS = -O2
endif

# Add optimization flags
CFLAGS += $(OPT_CFLAGS)

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

# Build the Linux kernel modules from here
SDKMOD = $(SDK)/appl/linux

# Place the library build objects here
export SDK_BLDDIR = $(SDKLIB)/$(BUILD)/$(TARGET_PLATFORM)

# Install SDK header files here
SDK_INCDIR = $(SDK_BLDDIR)/include/sdklt

# Install SDK library files here
SDK_LIBDIR = $(SDK_BLDDIR)/lib

#
# Set ISSU=1 to finalize the ISSU DB and build the ISSU DLL.
# The following environment variables must defined:
#
#   SDK_VERSION     (set to contenst of $(SDK)/RELEASE by default)
#   START_SDK_VER   (no default value)
#
ifeq ($(ISSU),1)
ifeq ($(SDK_VERSION),)
export SDK_VERSION=$(shell echo `cat $(SDK)/RELEASE`)
endif
ifndef START_SDK_VER
export START_SDK_VER=$(SDK_VERSION)
endif
endif

# Main target
all: sdk kmod
	$(MAKE) demo
ifeq ($(ISSU),1)
	$(MAKE) -C $(SDK) issu
endif

#
# Configure main application build to incorporate the SDK include
# directories.
#
INCLUDES += -I$(SDK_INCDIR)
OBJS =	main.o version.o
BOBJS = $(addprefix $(BLDDIR)/,$(OBJS))

# Name and destination of executable
PRG = $(DSTDIR)/sdklt$(DSTSUFFIX)

.PHONY: all sdk kmod demo

# Build SDK
sdk:
	$(MAKE) -C $(SDKLIB) TARGET_PLATFORM=$(TARGET_PLATFORM) \
		CPP_FLAGS="$(CPPFLAGS)" CFLAGS="$(CFLAGS)"

# Build Linux kernel modules
kmod:
ifdef KDIR
	$(MAKE) -C $(SDKMOD) TARGET_PLATFORM=$(TARGET_PLATFORM) \
		DSTDIR=$(DSTDIR)
endif

demo: $(PRG)

# Link application objects
LDOBJS = $(BOBJS)

# Link SDK static library
LDLIBS += $(SDK_LIBDIR)/libsdklt.a

# Non-SDK libraries
LDLIBS += $(OPENSRC_LIBS) $(EXTRA_LIBS) -pthread -lrt -ldl -lm
LDLIBS += -Wl,--export-dynamic

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

endif # TARGET_PLATFORM

cleanall:: clean

.PHONY: help clean cleanall cleanyaml

endif # defined $SDK
