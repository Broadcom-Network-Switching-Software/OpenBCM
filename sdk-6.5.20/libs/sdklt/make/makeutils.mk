#
# This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
# 
# Copyright 2007-2020 Broadcom Inc. All rights reserved.
#
# Various make utility functions.
#

ifneq (1,MAKEUTILS)
MAKEUTILS := 1

# Change comma-separated list to space-separated list
comma = ,
empty =
space = $(empty) $(empty)
spc_sep = $(subst $(comma),$(space),$1)

# Convert chip name to uppercase
chip_uc = $(subst a,A,$(subst b,B,$(subst c,C,$(subst m,M,$1))))

# Convert chip name to lowercase
chip_lc = $(subst A,a,$(subst B,b,$(subst C,c,$(subst M,m,$1))))

# Convert chip variant name to uppercase
var_uc = $(shell echo $1 | tr a-z A-Z)

# Convert chip variant name to lowercase
var_lc = $(shell echo $1 | tr A-Z a-z)

endif
