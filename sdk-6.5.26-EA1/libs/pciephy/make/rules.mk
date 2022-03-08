# $Id$
# This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
# 
# Copyright 2007-2022 Broadcom Inc. All rights reserved.
#
# PCIEPHY default make rules. These can optionally be overridden by
# letting PCIEPHY_MAKE_RULES point to a different rules file.
#

ifdef PCIEPHY_MAKE_RULES

include $(PCIEPHY_MAKE_RULES)

else

targetlibsoname = $(LIBNAME).so.${SHAREDLIBVER}
targetlibrealname = ${targetlibsoname}
targetlibso = ${LIBDIR}/${targetlibrealname}

#
# By default we will try to build the dependency files via the main
# compilation command, so we just need to include the depedency files
# if present.
#
DEPFLAGS := -MMD -MP -P

$(BLDDIR)/%.$(OBJSUFFIX): %.c $(BLDDIR)/.tree
	@$(ECHO) 'Compiling $(LOCALDIR)/$<'
	$(Q)$(CC) $(DEPFLAGS) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

$(LIBDIR)/$(LIBNAME).$(LIBSUFFIX): $(BOBJS) $(BLDDIR)/.tree
	@$(ECHO) 'Building library $(LIBNAME)...'
	$(Q)$(AR) $(ARFLAGS) $@ $(BOBJS)
ifeq ($(LINUX_MAKE_SHARED_LIB),1)
	$(CC) -shared -Wl,-soname,${targetlibsoname} -o ${targetlibso} \
	      ${BOBJS} -lc -lnsl -lpthread -lm -lrt
endif # LINUX_MAKE_SHARED_LIB #

-include $(BOBJS:.$(OBJSUFFIX)=.d)

clean::
	@$(ECHO) Cleaning dependencies for $(LOCALDIR)
	@$(RM) $(BOBJS:.$(OBJSUFFIX)=.d)

#
# Define rules for creating object directories
#
.PRECIOUS: $(BLDDIR)/.tree

%/.tree:
	@$(ECHO) 'Creating build directory $(dir $@)'
	$(Q)$(MKDIR) $(dir $@)
	@$(ECHO) "Build Directory for $(LOCALDIR) created" > $@
#
# Configure build tools
#
include $(PCIEPHY)/make/maketools.mk

endif # PCIEPHY_MAKE_RULES
