#
# This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
# 
# Copyright 2007-2020 Broadcom Inc. All rights reserved.
#
# Umbrella for various SDK developer tools.
#

# Pedantic build
include $(SDK)/make/pedantic.mk

# Standard documentation targets
include $(SDK)/make/doc.mk

# Internal tools
-include $(SDK)/INTERNAL/appl/make/devtools.mk
