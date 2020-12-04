# $Id$
# This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
# 
# Copyright 2007-2020 Broadcom Inc. All rights reserved.
#
# Default rule to build dependencies.  This builds an x.d file for each
# x.c file that describes the dependencies.  We then conditionally include
# the generated .d files.
#

ifneq (n/a,$(PCIEPHY_DEPEND))

#
# If making 'clean', do not include any .d files.  If they are included,
# gmake intrinsically tries to remake them all.
#

ifeq (,$(findstring clean,$(MAKECMDGOALS)))

DEPS_SED = \
	cp $(BLDDIR)/$*.d $(BLDDIR)/$*.P;\
	$(ECHO) >> $(BLDDIR)/$*.P;\
	sed -e 's/\#.*//' -e 's/^[^:]*: *//' \
	-e 's/ *\\$$//' -e '/^$$/ d' -e 's/$$/ :/' \
	-e '/^ .$$/d' \
	< $(BLDDIR)/$*.d >> $(BLDDIR)/$*.P; \
	$(RM) -f $(BLDDIR)/$*.d $(BLDDIR)/$*.tmp

#
# Attempt to build the depend files.  If it fails, the depend file is
# removed so that it is not included in later builds.
#
$(BLDDIR)/%.P : %.c $(BLDDIR)/.tree
	@$(ECHO) Dependencies for $(LOCALDIR)/$<
ifdef MAKEDEP
	@$(MAKEDEP) "$@" "$(BLDDIR)/$*.d" "$(PCIEPHY_DEPEND)"
else
	@$(PRINTF) '$$(BLDDIR)/' > $(BLDDIR)/$*.d
	@$(PCIEPHY_DEPEND) >> $(BLDDIR)/$*.d
endif
	$(DEPS_SED)

ifneq ($(strip $(LSRCS)),)
ifneq (,$(findstring .$(OBJSUFFIX),$(MAKECMDGOALS)))
-include $(addprefix $(BLDDIR)/,$(MAKECMDGOALS:.$(OBJSUFFIX)=.P))
else
-include $(addprefix $(BLDDIR)/,$(addsuffix .P,$(basename $(LSRCS))))
endif
endif

endif	# !CLEANING

clean::
	@$(ECHO) Cleaning dependencies for $(LOCALDIR)
	@$(RM) $(BOBJS:.$(OBJSUFFIX)=.P)

endif
