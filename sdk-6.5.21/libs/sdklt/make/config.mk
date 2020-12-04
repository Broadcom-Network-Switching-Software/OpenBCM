#
# This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
# 
# Copyright 2007-2020 Broadcom Inc. All rights reserved.
#
# SDK make rules and definitions
#

#
# Provide reasonable defaults for configuration variables
#

# Default build directory
ifndef SDK_BLDDIR
SDK_BLDDIR = $(SDK)/build
endif

# Location to build objects in
SDK_OBJDIR = $(SDK_BLDDIR)/obj
override BLDDIR := $(SDK_OBJDIR)

# Location to place libraries
SDK_LIBDIR = $(SDK_BLDDIR)
LIBDIR := $(SDK_LIBDIR)

# Option to retrieve compiler version
ifndef SDK_CC_VERFLAGS
SDK_CC_VERFLAGS := -v
endif
CC_VERFLAGS = $(SDK_CC_VERFLAGS);

# Default suffixes for object files
include $(SDK)/make/suffix.mk

# Set options for partial build support
include $(SDK)/make/partial.mk

# Name of SDK source file checksum file
SDK_LIBSUM := sdk.sum

#
# Set up compiler options, etc.
#
# The SDK_EXTRA_CPPFLAGS and SDK_EXTRA_CFLAGS are intended for command
# line overrides and should not be used in any make files.
#

# Import preprocessor flags
override CPPFLAGS = $(SDK_CPPFLAGS) $(SDK_EXTRA_CPPFLAGS)

# Import compiler flags
override CFLAGS = $(SDK_CFLAGS) $(SDK_EXTRA_CFLAGS)


#
# Define standard targets, etc.
#

ifdef LOCALDIR
override BLDDIR := $(BLDDIR)/$(LOCALDIR)
endif

ifndef LSRCS
LSRCS = $(wildcard *.c *.cpp *.cc)
endif
ifndef LOBJS
LOBJS = $(addsuffix .$(OBJSUFFIX), $(basename $(LSRCS)))
endif
ifndef BOBJS
BOBJS = $(addprefix $(BLDDIR)/,$(LOBJS))
endif

# Use SDK_QUIET=1 to control printing of compilation lines.
ifdef SDK_QUIET
Q = @
endif

#
# Define rules for creating object directories
#

.PRECIOUS: $(BLDDIR)/.tree

%/.tree:
	@$(ECHO) 'Creating build directory $(dir $@)'
	$(Q)$(MKDIR) $(dir $@)
	@$(ECHO) "Build Directory for $(LOCALDIR) created" > $@

#
# Configure build tools
#
include $(SDK)/make/maketools.mk

#
# Cleaning targets
#
include $(SDK)/make/clean.mk
