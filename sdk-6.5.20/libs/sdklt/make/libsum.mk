#
# This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
# 
# Copyright 2007-2020 Broadcom Inc. All rights reserved.
#
# SDK library source checksum.
#
# This file is being used as a top-level makefile, i.e. it is not
# intended for inclusion.
#
# Setting SDK_GEN_CHECKSUM=0 will turn off this feature, but this will
# also disable warm boot support.
#

all: libsum

# Get SDK_BLKDDIR and SDK_LIBSUM
include $(SDK)/make/config.mk

# Get SDK_DEFAULT_TARGETS
include $(SDK)/make/targets.mk

# Skip Linux and application components
LIBSUM_TARGETS := $(filter-out bcma bcmlu,$(SDK_DEFAULT_TARGETS))

# Tool for generating component checksum files
LIBSUM := $(SDK)/make/libsum.sh

libsum: clean $(SDK_BLDDIR)/.tree
ifneq (0,$(SDK_GEN_CHECKSUM))
	@$(ECHO) "Generating library checksum"
	$(LIBSUM) $(SDK_BLDDIR) $(SDK_LIBSUM) $(LIBSUM_TARGETS)
endif

.PHONY: all libsum

$(CLEAN_TARGETS)::
	$(RM) $(SDK_BLDDIR)/*sum
