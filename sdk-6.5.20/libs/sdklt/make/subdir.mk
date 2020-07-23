#
# This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
# 
# Copyright 2007-2020 Broadcom Inc. All rights reserved.
#
# Helper make file for building sub-directories within a library
# sub-component.
#
# By default we include source files four levels deep.
#

ifndef SUBDIRS

include $(SDK)/make/makeutils.mk

# By default we scan three levels of sub-directories
SUBDIRS := $(sort $(dir $(wildcard */*/ */*/*/ */*/*/*/)))

# Check if chip directory is secondary level
ifneq (,$(wildcard chip/*/))
CSD := chip/
endif

# Check if variants are in a dedicated subdirectory
ifneq (,$(wildcard $(CSD)bcm*/variant/*/))
VSD := variant/
endif

# Create list of all chip directories
ALL_CHIPDIRS += $(sort $(dir $(wildcard $(CSD)bcm*/ $(CSD)bcm*/*/ $(CSD)bcm*/$(VSD)*/)))

#
# If SDK_CHIPS is defined, then exclude any chip directory which is
# not part of this list.
#
ifdef SDK_CHIPS
# Create space-separated lowercase version of chip list
SDK_CHIPS_SPC := $(call spc_sep,$(SDK_CHIPS))
SDK_CHIPS_LC := $(call chip_lc,$(SDK_CHIPS_SPC))
endif

#
# If SDK_VARIANTS is defined, then exclude any chip variant directory
# which is not part of this list.
#
# Subdirectories within each individual chip directory are treated as
# a variant of that chip by default. To treat such subdirectories as
# ordinary subdirectories, they must be listed as pseudo-variants in
# the chip/Makefile for a given component.
#
ifdef SDK_VARIANTS
# Create space-separated lowercase version of chip variant list
SDK_VARIANTS_SPC := $(call spc_sep,$(SDK_VARIANTS) $(SDK_PSEUDO_VARIANTS))
SDK_VARIANTS_LC := $(call var_lc,$(SDK_VARIANTS_SPC))
endif

ifdef SDK_CHIPS
# Create list of requested chip directories
SDK_CHIPDIRS := $(addsuffix /,$(SDK_CHIPS_LC))
SDK_CHIPDIRS := $(addprefix $(CSD),$(SDK_CHIPDIRS))
# Add variant sub-directories (if any)
VAR_WC = $(addsuffix */*/,$(SDK_CHIPDIRS))
ifdef SDK_VARIANTS
# Create wildcard from specified chips and variants
VAR_WC = $(foreach V,$(SDK_VARIANTS_LC),$(foreach C,$(SDK_CHIPDIRS),$(C)$(VSD)$(V)/))
endif
else
# If SDK_VARIANTS is defined, but SDK_CHIPS is not.
ifdef SDK_VARIANTS
# Create wildcard from all chips and specified variants
SDK_CHIPDIRS := $(sort $(dir $(wildcard $(CSD)bcm*/)))
VAR_WC = $(foreach V,$(SDK_VARIANTS_LC),$(CSD)bcm*/$(VSD)$(V)/)
endif
endif

# Identify variant directories to be included in partial build
ifneq (,$(VAR_WC))
ifneq (,$(VSD))
VAR_WC += $(addsuffix */,$(SDK_CHIPDIRS))
endif
SDK_CHIPVARS := $(dir $(wildcard $(VAR_WC)))
endif

# All chip/variant directories to be included in partial build
SDK_INCDIRS := $(SDK_CHIPDIRS) $(SDK_CHIPVARS)

# Filter directories if partial build
ifneq (,$(strip $(SDK_INCDIRS)))
# Create list of chip directories to filter out
SKIPDIRS := $(filter-out $(SDK_INCDIRS),$(ALL_CHIPDIRS))
# Perform filtering of sub-directories
FILTERED_SUBDIRS := $(filter-out $(SKIPDIRS),$(SUBDIRS))
SUBDIRS := $(FILTERED_SUBDIRS)
endif

endif

VPATH += $(SUBDIRS)
VSRCS += $(wildcard $(addsuffix /*.c,$(VPATH)))
VSRCS += $(wildcard $(addsuffix /*.cpp,$(VPATH)))
VSRCS += $(wildcard $(addsuffix /*.cc,$(VPATH)))
VSRCS += $(wildcard *.c *.cpp *.cc)

LSRCS = $(sort $(notdir $(VSRCS)))
LOBJS = $(addsuffix .$(OBJSUFFIX), $(basename $(LSRCS)))
BOBJS = $(addprefix $(BLDDIR)/,$(LOBJS))

#
# Debug target
#
ifeq (1,$(SUBDIRS_DEBUG))

# Suppress dependency generation
SDK_DEPEND=n/a

subdirs_debug:
	echo "$(LOCALDIR) SUBDIRS: $(SUBDIRS)"
ifeq (1,$(VSRCS_DEBUG))
	echo "$(LOCALDIR) VSRCS: $(VSRCS)"
endif

.PHONY: subdirs_debug

endif
