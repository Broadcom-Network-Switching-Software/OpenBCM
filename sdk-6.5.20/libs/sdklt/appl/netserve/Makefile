#
# This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
# 
# Copyright 2007-2020 Broadcom Inc. All rights reserved.
#
# SDK netserve application.
#
# This make file builds the SDK netserve application, which is a small
# program that redirects standard I/O to a network socket.
#
# Example invokations are provided as special targets at the bottom of
# this file.
#

ifndef SDK
nosdk:; @echo 'The $$SDK environment variable is not set'; exit 1
else

# Default location to look for platform-specific make files
ifndef TARGET_PLATFORM_DIR
TARGET_PLATFORM_DIR = $(SDK)/appl/make
endif

ifeq ($(TARGET_PLATFORM)$(CROSS_COMPILE),)

help:
	@echo
	@echo Broadcom SDKLT netserve application makefile.
	@echo
	@echo Please specify a target platform, e.g.:
	@echo
	@echo make TARGET_PLATFORM=native $(MAKECMDGOALS)
	@echo make TARGET_PLATFORM=xlr_linux $(MAKECMDGOALS)
	@echo

clean: help

else

# Get build configuration for selected target
ifdef TARGET_PLATFORM
include $(TARGET_PLATFORM_DIR)/$(TARGET_PLATFORM).mk
endif

# This is where we put the build objects
ifndef BLDDIR
BLDDIR = $(CURDIR)/build/$(TARGET_PLATFORM)
endif

# This is where we deliver the executable
ifndef DSTDIR
DSTDIR = $(BLDDIR)
endif

# Turn on warnings and debug symbols
CFLAGS += -Wall -Werror
CFLAGS += -g

# Default optimization
ifndef OPT_CFLAGS
OPT_CFLAGS = -O2
endif

# Add optimization flags
CFLAGS += $(OPT_CFLAGS)

# Allow extra flags from command line
CPPFLAGS += $(ADD_CPPFLAGS)
CFLAGS += $(ADD_CFLAGS)
LDFLAGS += $(ADD_LDFLAGS)

# Configure application
OBJS =	netserve.o
BOBJS = $(addprefix $(BLDDIR)/,$(OBJS))

# Name and destination of executable
PRG = $(DSTDIR)/netserve$(DSTSUFFIX)

.PHONY: main clean

main: $(PRG)

# POSIX Threads and Real-time C libraries
LDLIBS = $(OPENSRC_LDLIBS) -lutil -lpthread

$(PRG): $(BOBJS) $(DSTDIR)/.tree
	@echo 'Linking target application $@'
	$(CC) $(LDFLAGS) $(BOBJS) $(LDLIBS) -o $@

$(BLDDIR)/%.o: %.c $(BLDDIR)/.tree
	@echo 'Building application module $@'
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

$(BLDDIR)/.tree:
	@echo 'Creating build directory $(BLDDIR)'
	@mkdir -p $(BLDDIR)
	@echo "Build directory $(BLDLDIR) created" > $@

ifneq ($(DSTDIR),$(BLDDIR))
$(DSTDIR)/.tree:
	@echo 'Creating target directory $(DSTDIR)'
	@mkdir -p $(DSTDIR)
	@echo "Target directory $(BLDLDIR) created" > $@
endif

clean::
	rm -f $(PRG) $(BOBJS)

endif # TARGET_PLATFORM

cleanall:: clean

.PHONY: help clean cleanall

endif # defined $SDK
