#
# This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
# 
# Copyright 2007-2020 Broadcom Inc. All rights reserved.
#
# Helper make file for building HA header files.
#

# Get main library name
include $(SDK)/make/libname.mk

# Default location for all SDK libraries
include $(SDK)/make/libpath.mk

# Common build tools
include $(SDK)/make/maketools.mk

# Destination for generated header files
HA_DSTDIR = ../include/$(MAINLIB)/generated

# Input files
HA_YMLS = $(wildcard *.yml)

# Output files
HA_LHDRS = $(HA_YMLS:.yml=.h)
HA_BHDRS = $(addprefix $(HA_DSTDIR)/,$(HA_LHDRS))

# Tools
PARSER = $(BCMHA)/scripts/ha_yml_parser.py
HA_PARSE = python $(PARSER)

all: $(HA_DSTDIR)/.tree $(HA_BHDRS)

.PRECIOUS: $(HA_DSTDIR)/.tree

$(HA_DSTDIR)/.tree:
	@$(ECHO) 'Creating include directory $(dir $@)'
	$(Q)$(MKDIR) $(dir $@)
	@$(ECHO) "Include directory $(dir $@) created" > $@

$(HA_DSTDIR)/%.h: %.yml $(HA_DSTDIR)/.tree FORCE
	@echo Generating HA header file $(notdir $@)
	$(HA_PARSE) -g $< -o $@

# In addition to the HA header file, the HA parser also generates the
# ISSU database files, which means that the make rule above does not
# cover all output files, and therefore we use a forced target to
# ensure the rule is run every time. However, since the HA parser will
# only update target files that have changed, the rule will not
# trigger any unwanted downstream make dependencies.
FORCE:

include $(SDK)/make/clean.mk

distclean::
	$(Q)$(RM) $(HA_DSTDIR)/.tree $(HA_BHDRS) $(HA_DSTDIR)

cleanall:: distclean

$(CLEAN_TARGETS)::

.PHONY: all
