# 
# This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
# 
# Copyright 2007-2019 Broadcom Inc. All rights reserved.
#
# PCIEG3 make rules and definitions
#

#
# Provide reasonable defaults for configuration variables
#

# Default build directory
ifndef PCIEG3_BLDDIR
PCIEG3_BLDDIR = $(PCIEG3)/build
endif

# Location to build objects in
ifndef PCIEG3_OBJDIR
PCIEG3_OBJDIR = $(PCIEG3_BLDDIR)/obj
endif
override BLDDIR := $(PCIEG3_OBJDIR)

# Location to place libraries
ifndef PCIEG3_LIBDIR
PCIEG3_LIBDIR = $(PCIEG3_BLDDIR)
endif
LIBDIR := $(PCIEG3_LIBDIR)

# Option to retrieve compiler version
ifndef PCIEG3_CC_VERFLAGS
PCIEG3_CC_VERFLAGS := -v
endif
CC_VERFLAGS = $(PCIEG3_CC_VERFLAGS);

# Default suffix for object files
ifndef PCIEG3_OBJSUFFIX
PCIEG3_OBJSUFFIX = o
endif
OBJSUFFIX = $(PCIEG3_OBJSUFFIX)

# Default suffix for library files
ifndef PCIEG3_LIBSUFFIX
PCIEG3_LIBSUFFIX = a
endif
LIBSUFFIX = $(PCIEG3_LIBSUFFIX)

#
# Set up compiler options, etc.
#

# Default include path
PCIEG3_INCLUDE_PATH = -I$(PCIEG3)/include
PCIEG3_INCLUDE_PATH += -I$(PCIEG3)/merlin16/include
INCDIR += $(PCIEG3_INCLUDE_PATH)
# Import preprocessor flags avoiding include duplicates
TMP_PCIEG3_CPPFLAGS := $(filter-out $(PCIEG3_INCLUDE_PATH),$(PCIEG3_CPPFLAGS))

TMP_PCIEG3_CPPFLAGS += $(PCIEG3_DSYM_CPPFLAGS)
export PCIEG3_DSYM_CPPFLAGS

override CPPFLAGS = $(TMP_PCIEG3_CPPFLAGS) $(PCIEG3_INCLUDE_PATH)


# Import compiler flags
override CFLAGS = $(PCIEG3_CFLAGS)




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

# Use PCIEG3_QUIET=1 to control printing of compilation lines.
ifdef PCIEG3_QUIET
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
include $(PCIEG3)/make/maketools.mk
