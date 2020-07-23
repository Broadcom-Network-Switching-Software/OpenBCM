#
# This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
# 
# Copyright 2007-2020 Broadcom Inc. All rights reserved.
#
# Rules to rebuild all objects if build options change.
#
# This file is being used as a top-level makefile, i.e. it is not
# intended for inclusion.
#
# Setting SDK_AUTO_REBUILD=0 will turn off this feature.
#

# Get SDK_BLKDDIR
include $(SDK)/make/config.mk

# Build option to save and compare
BLD_OPT = $(sort $(CPPFLAGS)) $(sort $(CFLAGS))

# File name must match the object dependencies in rules.mk
BLD_FILE = $(SDK_BLDDIR)/.options

# Update build options file if build options changed
BLD_CMD = \
	echo "Reconfiguring build options"; \
	echo "$(BLD_OPT)" > $(BLD_FILE); \
	rm -f $(SDK_BLDDIR)/lib*chip.$(LIBSUFFIX) \
	rm -f $(SDK_BLDDIR)/libbcmdrdsym.$(LIBSUFFIX)

build_options: $(SDK_BLDDIR)/.tree
ifneq (0,$(SDK_AUTO_REBUILD))
	echo "$(BLD_OPT)" | cmp -s - $(BLD_FILE) || ($(BLD_CMD))
endif

.PHONY: build_options
