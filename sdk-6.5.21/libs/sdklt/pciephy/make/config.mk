# $Id$
# This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
# 
# Copyright 2007-2020 Broadcom Inc. All rights reserved.
#
# PCIEPHY make rules and definitions
#

#
# Provide reasonable defaults for configuration variables
#

# Default build directory
ifndef PCIEPHY_BLDDIR
PCIEPHY_BLDDIR = $(PCIEPHY)/build
endif

# Location to build objects in
ifndef PCIEPHY_OBJDIR
PCIEPHY_OBJDIR = $(PCIEPHY_BLDDIR)/obj
endif
override BLDDIR := $(PCIEPHY_OBJDIR)

# Location to place libraries
ifndef PCIEPHY_LIBDIR
PCIEPHY_LIBDIR = $(PCIEPHY_BLDDIR)
endif
LIBDIR := $(PCIEPHY_LIBDIR)

# Option to retrieve compiler version
ifndef PCIEPHY_CC_VERFLAGS
PCIEPHY_CC_VERFLAGS := -v
endif
CC_VERFLAGS = $(PCIEPHY_CC_VERFLAGS);

# Default suffix for object files
ifndef PCIEPHY_OBJSUFFIX
PCIEPHY_OBJSUFFIX = o
endif
OBJSUFFIX = $(PCIEPHY_OBJSUFFIX)

# Default suffix for library files
ifndef PCIEPHY_LIBSUFFIX
PCIEPHY_LIBSUFFIX = a
endif
LIBSUFFIX = $(PCIEPHY_LIBSUFFIX)

#
# Set up compiler options, etc.
#

# Default include path
PCIEPHY_INCLUDE_PATH = -I$(PCIEPHY)/include
PCIEPHY_INCLUDE_PATH += -I$(PCIEPHY)/chip
INCDIR += $(PCIEPHY_INCLUDE_PATH)
# Import preprocessor flags avoiding include duplicates
TMP_PCIEPHY_CPPFLAGS := $(filter-out $(PCIEPHY_INCLUDE_PATH),$(PCIEPHY_CPPFLAGS))

TMP_PCIEPHY_CPPFLAGS += $(PCIEPHY_DSYM_CPPFLAGS)
export PCIEPHY_DSYM_CPPFLAGS

override CPPFLAGS = $(TMP_PCIEPHY_CPPFLAGS) $(PCIEPHY_INCLUDE_PATH)


# Import compiler flags
override CFLAGS = $(PCIEPHY_CFLAGS)




#
# Define standard targets, etc.
#

ifdef LOCALDIR
override BLDDIR := $(BLDDIR)/$(LOCALDIR)
endif

ifndef LSRCS
LSRCS = $(wildcard *.c)
endif
ifndef LOBJS
LOBJS = $(addsuffix .$(OBJSUFFIX), $(basename $(LSRCS)))
endif
ifndef BOBJS
BOBJS = $(addprefix $(BLDDIR)/,$(LOBJS))
endif

# Use PCIEPHY_QUIET=1 to control printing of compilation lines.
ifdef PCIEPHY_QUIET
Q = @
endif

#
# Define rules for creating object directories
#
#
#	mkdir -p $(dir $@)
.PRECIOUS: $(BLDDIR)/.tree

%/.tree:
	@$(ECHO) 'Creating build directory $(dir $@)'
	$(Q)$(MKDIR) $(dir $@)
	@$(ECHO) "Build Directory for $(LOCALDIR) created" > $@
#
# Configure build tools
#
include $(PCIEPHY)/make/maketools.mk
