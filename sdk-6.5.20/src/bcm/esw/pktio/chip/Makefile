# $Id: $
# This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
# 
# Copyright 2007-2020 Broadcom Inc. All rights reserved.
#
# Makefile for streamlined packet I/O drivers
#

include ${SDK}/make/Make.config

# Chips with drivers available here
CHIPS = $(shell echo $(sort $(patsubst bcm%/,%,$(dir $(wildcard bcm*/)))) | tr a-z A-Z)

# Filter out chips that are not enabled in this build
SUBDIR_CHIPS =
$(foreach v,${CHIPS},$(if $(BCM_$v),$(eval SUBDIR_CHIPS+=bcm$v),))
subdirs = $(shell echo ${SUBDIR_CHIPS} | tr A-Z a-z)

include ${SDK}/make/Make.subdirs
