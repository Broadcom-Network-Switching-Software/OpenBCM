#
# This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
# 
# Copyright 2007-2020 Broadcom Inc. All rights reserved.
#
# Tool chain and platform settings for compiling the SDK for a single
# simulated BCM56780 device on the local host machine.
#

# Tools prefix
CROSS_COMPILE =

# Build tools
CC = $(CROSS_COMPILE)gcc
LD = $(CROSS_COMPILE)ld
AR = $(CROSS_COMPILE)ar

# PCI bus configuration flags to avoid software byte-swap
CPPFLAGS += -DSYS_BE_PIO=0 -DSYS_BE_PACKET=0 -DSYS_BE_OTHER=0

# Build for BCM56780 only
CPPFLAGS += -DBCMDRD_CONFIG_INCLUDE_CHIP_DEFAULT=0 \
	    -DBCMDRD_CONFIG_INCLUDE_BCM56780_A0=1

# Hardware interface (see $SDK/bcma/sys/probe directory)
export SYSTEM_INTERFACE = xgssim

# Default open source target build
OPENSRC_BUILD ?= $(shell uname -m)

include $(SDK)/appl/make/yaml_native.mk
