#
# $Id$
# This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
# 
# Copyright 2007-2022 Broadcom Inc. All rights reserved.
#

ifndef PHYMOD_LOCALDIR
PHYMOD_LOCALDIR = phymod
endif

LOCALDIR := $(PHYMOD_LOCALDIR)

ifndef PHYMOD
PHYMOD = $(CURDIR)
endif

include ${PHYMOD}/make/config.mk

# Core library
VPATH = acc core diag
VSRCS = $(wildcard acc/*.c) $(wildcard core/*.c) $(wildcard diag/*.c)

# Symbolic debug
VPATH += sym sym/chip
VSRCS += $(wildcard sym/*.c) $(wildcard sym/chip/*.c)

# PHY simulators
VPATH += sim sim/chip
VSRCS += $(wildcard sim/*.c) $(wildcard sim/chip/*.c)

# PHY util
VPATH += util 
VSRCS += $(wildcard util/*.c) 

#
# The PHYMOD_CHIPS make variable can be used to compile the sources
# for only one chip (or a subset of chips).
#
# Please see $PHYMOD/examples/libonly/Makefile for examples
#
ifneq ($(PHYMOD_CHIPS),)
MAKE_PHYMOD_CHIPS := $(PHYMOD_CHIPS)
endif

# Include chip sources
-include $(wildcard ${PHYMOD}/chip/*/chip.mk)

LSRCS = $(notdir $(VSRCS))
LOBJS = $(addsuffix .o, $(basename $(LSRCS)))
BOBJS = $(addprefix $(BLDDIR)/,$(LOBJS))

LIBNAME = libphymod

include ${PHYMOD}/make/lib.mk
