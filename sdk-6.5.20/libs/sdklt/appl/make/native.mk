#
# This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
# 
# Copyright 2007-2020 Broadcom Inc. All rights reserved.
#
# Tool chain and platform settings for compiling tools on the local
# host machine.
#

# Tools prefix
CROSS_COMPILE =

# Build tools
CC = $(CROSS_COMPILE)gcc
LD = $(CROSS_COMPILE)ld
AR = $(CROSS_COMPILE)ar

# Default open source target build
OPENSRC_BUILD ?= $(shell uname -m)

include $(SDK)/appl/make/yaml_native.mk
