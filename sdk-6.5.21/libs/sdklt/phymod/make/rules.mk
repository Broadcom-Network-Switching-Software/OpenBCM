# $Id$
# This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
# 
# Copyright 2007-2020 Broadcom Inc. All rights reserved.
#
# PHYMOD default make rules. These can optionally be overridden by
# letting PHYMOD_MAKE_RULES point to a different rules file.
#

ifdef PHYMOD_MAKE_RULES

include $(PHYMOD_MAKE_RULES)

else

targetlibsoname = $(LIBNAME).so.${SHAREDLIBVER}
targetlibrealname = ${targetlibsoname}
targetlibso = ${LIBDIR}/${targetlibrealname}

$(BLDDIR)/%.$(OBJSUFFIX): %.c $(BLDDIR)/.tree
	@$(ECHO) 'Compiling $(LOCALDIR)/$<'
	$(Q)$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

$(LIBDIR)/$(LIBNAME).$(LIBSUFFIX): $(BOBJS) $(BLDDIR)/.tree 
	@$(ECHO) 'Building library $(LIBNAME)...'
	$(Q)$(AR) $(ARFLAGS) $@ $(BOBJS)
ifeq ($(LINUX_MAKE_SHARED_LIB),1)
	$(CC) -shared -Wl,-soname,${targetlibsoname} -o ${targetlibso} ${BOBJS} -lc -lnsl -lpthread -lm -lrt
endif # LINUX_MAKE_SHARED_LIB #
endif
