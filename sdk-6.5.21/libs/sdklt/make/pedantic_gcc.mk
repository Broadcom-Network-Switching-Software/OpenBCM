#
# This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
# 
# Copyright 2007-2020 Broadcom Inc. All rights reserved.
#
# Default pedantic build flags for gcc.
#

PEDANTIC_GCC_FLAGS += -pedantic -std=c99
PEDANTIC_GCC_FLAGS += -Wsign-compare -Wundef -Wunused -Wshadow
PEDANTIC_GCC_FLAGS += -Wstrict-prototypes -Wmissing-prototypes
PEDANTIC_GCC_FLAGS += -Wdeclaration-after-statement
