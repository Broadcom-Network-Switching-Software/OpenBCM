# $Id$
# This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
# 
# Copyright 2007-2020 Broadcom Inc. All rights reserved.
#
# PCIEPHY make rules for libraries
#

include $(PCIEPHY)/make/config.mk

include $(PCIEPHY)/make/rules.mk

BLIBNAME = $(LIBDIR)/$(LIBNAME).$(LIBSUFFIX)

.SECONDARY:: $(BOBJS)

all:: $(BLDDIR)/.tree $(BLIBNAME)

clean::
	@$(ECHO) Cleaning objects for $(notdir $(BLIBNAME))
	$(Q)$(RM) $(BLDDIR)/.tree $(BOBJS) $(BLIBNAME)

include $(PCIEPHY)/make/depend.mk
