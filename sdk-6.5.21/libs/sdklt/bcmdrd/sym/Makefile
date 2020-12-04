#
# This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
# 
# Copyright 2007-2020 Broadcom Inc. All rights reserved.
#

include $(SDK)/make/makeutils.mk

# Set default library name
include $(SDK)/make/libname.mk

#
# If SDK_CHIPS is defined, then exclude the symbol table of any chip
# which is not part of this list.
#
CSRCS := $(subst chip/,,$(wildcard chip/*.c))
ifdef SDK_CHIPS
# Create space-separated lowercase version of chip list
SDK_CHIPS_SPC := $(call spc_sep,$(SDK_CHIPS))
SDK_CHIPS_LC := $(call chip_lc,$(SDK_CHIPS_SPC))
# Create list of symbol table source files
CSRCS := $(filter $(addsuffix _sym.c,$(SDK_CHIPS_LC)),$(CSRCS))
endif
VSRCS += $(addprefix chip/,$(CSRCS))

VPATH += chip
VSRCS += $(wildcard *.c)

LSRCS = $(sort $(notdir $(VSRCS)))
LOBJS = $(addsuffix .$(OBJSUFFIX), $(basename $(LSRCS)))
BOBJS = $(addprefix $(BLDDIR)/,$(LOBJS))

# Generic library make rules
include $(SDK)/make/lib.mk
