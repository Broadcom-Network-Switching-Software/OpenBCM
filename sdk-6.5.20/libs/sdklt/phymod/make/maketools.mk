# $Id$
# This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
# 
# Copyright 2007-2020 Broadcom Inc. All rights reserved.
#
# PHYMOD make tools
#

# Compiler command for generating dependencies
PHYMOD_DEPEND ?= $(CC) -M -P $(CPPFLAGS) $< 

# Perl is required for portable build tools
ifndef PHYMOD_PERL
PHYMOD_PERL = perl
endif

ifeq (n/a,$(PHYMOD_PERL))
#
# If perl is not available, try building using standard UNIX utilities
#
RM      = rm -f
MKDIR   = mkdir -p
CP      = cp -d
ECHO    = echo
PRINTF  = printf
else
#
# If perl is available, use portable build tools
#
MKTOOL  = $(PHYMOD_PERL) $(PHYMOD)/make/mktool.pl
RM      = $(MKTOOL) -rm
MKDIR   = $(MKTOOL) -md
FOREACH = $(MKTOOL) -foreach
CP      = $(MKTOOL) -cp
MAKEDEP = $(MKTOOL) -dep 
ECHO    = $(MKTOOL) -echo
PRINTF  = $(MKTOOL) -echo -n
MKBEEP  = $(MKTOOL) -beep
endif
