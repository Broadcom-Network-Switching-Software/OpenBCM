# $Id:$
# This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
# 
# Copyright 2007-2020 Broadcom Inc. All rights reserved.
#
# Makefile for BCM driver Library
#

LOCALDIR = src/bcm/ltsw/yaml

include ${SDK}/make/Make.config

# Destination for generated header files
HA_DSTDIR = $(SDK)/include/bcm_int/ltsw/generated

# Environmental variables
export ISSU_DB = $(SDK)/src/bcm/ltsw/dll/db
export SDK_VERSION = $(SDK_VER)

# Output files
HA_YAMLS = $(wildcard *.yml)
HA_LHDRS = $(patsubst %.yml,%.h,$(HA_YAMLS))
HA_BHDRS = $(addprefix $(HA_DSTDIR)/,$(HA_LHDRS))

# Tools
PARSER = $(SDKLT)/bcmha/scripts/ha_yml_parser.py
HA_PARSE = python $(PARSER)

all:: $(HA_DSTDIR)/.tree $(HA_BHDRS)

$(HA_DSTDIR)/.tree:
	@$(ECHO) 'Creating include directory $(dir $@)'
	$(Q)$(MKDIR) $(dir $@)
	@$(ECHO) "Include directory $(dir $@) created" > $@

$(HA_DSTDIR)/%.h: %.yml $(HA_DSTDIR)/.tree FORCE
	@echo Generating HA header files for LTSW: $@
	$(HA_PARSE) -g $< -o $@
FORCE:

clean_d::
#ifdef QUIET
#	@$(ECHO) Cleaning $(HA_DSTDIR)
#endif
#	rm -rf $(HA_DSTDIR)

clean:: clean_d
