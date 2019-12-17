# 
# This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
# 
# Copyright 2007-2019 Broadcom Inc. All rights reserved.

SDK = $(CURDIR)
export SDK

include ${SDK}/make/Make.config

subdirs=src include

ifdef BUILD_PHYMOD
subdirs += libs/phymod
endif

ifdef BUILD_PCIEG3
subdirs += libs/pcieg3
endif

include ${SDK}/make/Make.subdirs

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
