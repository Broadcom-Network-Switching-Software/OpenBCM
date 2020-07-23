#
# This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
# 
# Copyright 2007-2020 Broadcom Inc. All rights reserved.
#
# SDK default object suffixes
#

# Default suffix for object files
ifndef SDK_OBJSUFFIX
SDK_OBJSUFFIX = o
endif
OBJSUFFIX = $(SDK_OBJSUFFIX)

# Default suffix for library files
ifndef SDK_LIBSUFFIX
SDK_LIBSUFFIX = a
endif
LIBSUFFIX = $(SDK_LIBSUFFIX)
