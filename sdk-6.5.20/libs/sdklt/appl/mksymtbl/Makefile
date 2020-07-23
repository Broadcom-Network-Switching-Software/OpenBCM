#
# This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
# 
# Copyright 2007-2020 Broadcom Inc. All rights reserved.
#
# SDKLT mksymtbl application.
#
# This make file builds the a subset of the SDKLT library and a small
# application on top to generate the physical device information to
# YAML data.
#

ifndef SDK
nosdk:; @echo 'The $$SDK environment variable is not set'; exit 1
else

# This is where we put the build objects
ifndef BLDDIR
BLDDIR = $(CURDIR)/build
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
export CC=gcc
export AR=ar

#
# Configure SDK library build to use same build options and
# directories as the application itself.
#
export SDK_BLDDIR = $(BLDDIR)
export SDK_CPPFLAGS = $(CPPFLAGS)
export SDK_CFLAGS = $(CFLAGS)

# Only build what is necessary
export SDK_TARGETS = bcmdrd shr bsl sal
export SHR_TARGETS = pb cht

# Main target
all: sdklibs
	$(MAKE) main

# SDK make targets, including the sdklibs target
include $(SDK)/make/sdklibs.mk

#
# Configure main application build to incorporate the SDK include
# directories.
#
INCLUDES += $(SDK_INCLUDES)
OBJS =	main.o version.o
BOBJS = $(addprefix $(BLDDIR)/,$(OBJS))

# Name and destination of executable
PRG = $(DSTDIR)/mksymtbl$(DSTSUFFIX)

.PHONY: all main

main: $(PRG)

# Link application objects
LDOBJS = $(BOBJS) $(SDK_BLIBS)

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
VERSION_CPPFLAGS += -DYEAR_INFO="$(shell echo \\\"`date +%Y`\\\")"

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
	rm -f $(PRG) $(BOBJS)

cleanall:: clean

.PHONY: help clean cleanall

endif # defined $SDK
