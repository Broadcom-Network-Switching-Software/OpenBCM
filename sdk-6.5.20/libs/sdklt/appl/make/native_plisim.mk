#
# This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
# 
# Copyright 2007-2020 Broadcom Inc. All rights reserved.
#
# Tool chain and platform settings for compiling the SDK for a
# PLI-based simulator like BCMSIM.
#

# Tools prefix
CROSS_COMPILE =

# Build tools
CC = $(CROSS_COMPILE)gcc
LD = $(CROSS_COMPILE)ld
AR = $(CROSS_COMPILE)ar

# PCI bus configuration flags to avoid software byte-swap
CPPFLAGS += -DSYS_BE_PIO=0 -DSYS_BE_PACKET=0 -DSYS_BE_OTHER=0

# Hardware interface (see $SDK/bcma/sys/probe directory)
export SYSTEM_INTERFACE = plisim

# Default open source target build
OPENSRC_BUILD ?= $(shell uname -m)

include $(SDK)/appl/make/yaml_native.mk
