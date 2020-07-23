#
# This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
# 
# Copyright 2007-2020 Broadcom Inc. All rights reserved.
#
# Tool chain and platform settings for cross-compiling the SDK for the
# Broadcom XLR CPU board.
#
# In order to build for the XLR platform you must provide the
# following variables:
#
# TOOLCHAIN_DIR - Tool chain base directory.
# KDIR - Path to the XLR Linux kernel sources.
#

# Broadcom settings for TOOLCHAIN_DIR and KDIR
-include $(SDK)/INTERNAL/appl/make/xlr_linux_broadcom.mk

# Too chain directories
TOOLCHAIN_BIN_DIR = $(TOOLCHAIN_DIR)/bin
LIBRARY_PATH = $(TOOLCHAIN_DIR)/lib:$(TOOLCHAIN_DIR)/lib64
TARGET_ARCHITECTURE = x86_64-fedora-linux-gnu

# Add tools to system PATH
TOOL_PATH := $(TOOLCHAIN_BIN_DIR):$(TOOL_PATH)
override PATH := $(TOOL_PATH):$(PATH)

# Required for building application libraries
export TOOLCHAIN_BIN_DIR LIBRARY_PATH

# Tools prefix
CROSS_COMPILE = $(TARGET_ARCHITECTURE)-

# Build tools
CC = $(CROSS_COMPILE)gcc
LD = $(CROSS_COMPILE)ld
AR = $(CROSS_COMPILE)ar

# Architecture is required for building kernel modules
export ARCH=x86_64

# PCI bus configuration flags to avoid software byte-swap
CPPFLAGS += -DSYS_BE_PIO=0 -DSYS_BE_PACKET=0 -DSYS_BE_OTHER=0

# Use optimized register access
CPPFLAGS += -DBCMDRD_CONFIG_MEMMAP_DIRECT=1

# Hardware interface (see $SDK/bcma/sys/probe directory)
export SYSTEM_INTERFACE = ngbde

# Default open source target build
OPENSRC_BUILD ?= fed21-x86_64
