# $Id: Makefile,v 1.16 2013/02/12 16:38:40 hamilton Exp $
# This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
# 
# Copyright 2007-2020 Broadcom Inc. All rights reserved.

SDK = $(CURDIR)
export SDK

include ${SDK}/make/Make.config

subdirs=src include


ifdef BUILD_KAPS
subdirs += libs/kaps
endif

ifdef BUILD_PHYMOD
subdirs += libs/phymod
endif

ifdef BUILD_PCIEPHY
subdirs += libs/pciephy
endif

ifdef BUILD_FLEXE
subdirs += libs/flexe_core
endif

include ${SDK}/make/Make.subdirs

ifdef BUILD_BCMISSU_DLL
include ${SDK}/make/Make.issudll
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
