# $Id: Makefile,v 1.16 Broadcom SDK $
# This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
# 
# Copyright 2007-2022 Broadcom Inc. All rights reserved.

SDK = $(CURDIR)
export SDK

include ${SDK}/make/Make.config


ifneq ($(DNX_FAST_MODE),1)
subdirs=src include
endif

ifeq ($(shell test -f $(SDK)/libs/kaps/Makefile; echo $$?),0)
ifdef BUILD_KAPS
subdirs += libs/kaps
endif
endif

ifeq ($(shell test -f $(SDK)/libs/kaps_jr1/Makefile; echo $$?),0)
ifdef BUILD_KAPS_JR1
subdirs += libs/kaps_jr1
endif
endif

ifdef BUILD_PHYMOD
subdirs += libs/phymod
endif

ifdef BUILD_PCIEPHY
subdirs += libs/pciephy
endif

ifdef BUILD_AAPL
subdirs += libs/aapl
endif

ifdef BUILD_PHY_BRIDGE
subdirs += libs/phy_bridge
endif

ifdef BUILD_FLEXE
subdirs += libs/flexe_core/std
endif

ifdef BUILD_FLEXE_DBG
subdirs += libs/flexe_core/dbg
endif

ifdef BUILD_XFLOW_MACSEC
ifeq ($(shell test -f $(SDK)/libs/xflow_macsec/Makefile; echo $$?),0)
subdirs += libs/xflow_macsec
endif
endif

ifdef BUILD_FRAMER
subdirs += libs/framer
endif

include ${SDK}/make/Make.subdirs

ifdef BUILD_ISSU
include ${SDK}/make/Make.issudll
endif

ifeq ($(DNX_FAST_MODE),1)
-include $(SDK)/tools/dnx/make/custom.mk
endif

tree: ${BLDROOT}/.tree

#
# Provide TAGS rule to allow ctags/etags to be built at any level in the
# tree.
#
tags::
	$(RM) tags
	$(FOREACH) -find . ".*?\.[c|h]" "-pBuilding tags for: ##" \
		"${CTAGS} -a -o ${SDK}/tags ##"

etags::
	$(MAKE) CTAGS=${ETAGS} tags
	$(FOREACH) -find . ".*?\.tcl" "-pBuilding tags for: ##" \
		"${ETAGS} -a --lang=none --regex='/proc[ \t]+\([^ \t]+\)/\1/' -o ${SDK}/tags ##"

ifdef FAST
  include ${SDK}/make/Make.fast
endif
