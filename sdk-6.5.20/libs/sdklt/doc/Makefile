#
# This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
# 
# Copyright 2007-2020 Broadcom Inc. All rights reserved.
#
# DOC top-level makefile
#

include $(SDK)/make/config.mk

# Documentation directories with figures for the user guide
UGFIG_DIRS += $(SDK)/bcmha/DOC
UGFIG_DIRS += $(SDK)/bcmpkt/DOC

userguide: ugfigs
	$(MAKE) DOXY_EXT=.userguide doc

ugfigs: $(UGFIG_DIRS)

$(UGFIG_DIRS):
	$(MAKE) DOXY_EXT=.userguide DOCDIR=$@ figs

# Standard documentation targets
include $(SDK)/make/doc.mk

.PHONY: userguide ugfigs $(UGFIG_DIRS)
