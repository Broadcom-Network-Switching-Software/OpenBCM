#
# $Id$
# This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
# 
# Copyright 2007-2020 Broadcom Inc. All rights reserved.
#
LOCALDIR = libs/pciephy

ifndef PCIEPHY
PCIEPHY = $(CURDIR)
endif

include ${PCIEPHY}/make/config.mk

# Core library
VPATH += diag
VSRCS += $(wildcard diag/*.c)

# Merlin16 pciephy support
VPATH += chip/merlin16 chip/merlin16/tier1
VSRCS += $(wildcard chip/merlin16/*.c)
VSRCS += $(wildcard chip/merlin16/tier1/*.c)

# Merlin7 pciephy support
VPATH += chip/merlin7 chip/merlin7/tier1
VSRCS += $(wildcard chip/merlin7/*.c)
VSRCS += $(wildcard chip/merlin7/tier1/*.c)

LSRCS = $(notdir $(VSRCS))
LOBJS = $(addsuffix .o, $(basename $(LSRCS)))
BOBJS = $(addprefix $(BLDDIR)/,$(LOBJS))

LIBNAME = libpciephy
include ${PCIEPHY}/make/lib.mk

include ${PCIEPHY}/make/depend.mk
