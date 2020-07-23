#
# This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
# 
# Copyright 2007-2020 Broadcom Inc. All rights reserved.
#
# Helper makefile for defining default LIBNAME and LOCALDIR variables
# for library sub-components.
#
# Both of the above mentioned variables should be constructed from the
# name of the current directory and its parent.
#

# Extract the parent directory name
PPATH := $(dir $(CURDIR))
MAINLIB := $(notdir $(PPATH:%/=%))

# Extract the current directory name
SUBLIB = $(notdir $(CURDIR))

LOCALDIR = $(MAINLIB)/$(SUBLIB)
LIBNAME = lib$(MAINLIB)$(SUBLIB)
