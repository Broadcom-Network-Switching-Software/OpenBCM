#
# This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
# 
# Copyright 2007-2020 Broadcom Inc. All rights reserved.
#
# Helper make file for building library sub-components.
#

# Set default library name
include $(SDK)/make/libname.mk

# Include source files in  sub-directories.
include $(SDK)/make/subdir.mk

# Generic library make rules
include $(SDK)/make/lib.mk
