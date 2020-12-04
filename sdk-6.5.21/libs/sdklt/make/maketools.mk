#
# This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
# 
# Copyright 2007-2020 Broadcom Inc. All rights reserved.
#
# SDK make tools
#

# Compiler command for generating dependencies
SDK_DEPEND ?= $(CC) -M -MP -P $(CPPFLAGS) $< 

# Perl is required for portable build tools
ifndef SDK_PERL
SDK_PERL = perl
endif

ifeq (n/a,$(SDK_PERL))
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
MKTOOL  = $(SDK_PERL) $(SDK)/make/mktool.pl
RM      = $(MKTOOL) -rm
MKDIR   = $(MKTOOL) -md
FOREACH = $(MKTOOL) -foreach
CP      = $(MKTOOL) -cp
MAKEDEP = $(MKTOOL) -dep 
ECHO    = $(MKTOOL) -echo
PRINTF  = $(MKTOOL) -printf
MKBEEP  = $(MKTOOL) -beep
endif
