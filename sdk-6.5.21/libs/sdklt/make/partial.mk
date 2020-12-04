#
# This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
# 
# Copyright 2007-2020 Broadcom Inc. All rights reserved.
#
# SDK partial build support
#

include $(SDK)/make/makeutils.mk

#
# If SDK_CHIPS is defined, then exclude any chip which is not part of
# this list. Note that bond-option chips must be added via SDK_SKUS
# separately if needed.
#
ifdef SDK_CHIPS
# Create space-separated uppercase version of chip list
SDK_CHIPS_SPC := $(call spc_sep,$(SDK_CHIPS) $(SDK_SKUS))
SDK_CHIPS_UC := $(call chip_uc,$(SDK_CHIPS_SPC))
endif

#
# If SDK_VARIANTS is defined, then exclude any chip variant which is
# not part of this list.
#
ifdef SDK_VARIANTS
# Create space-separated uppercase version of chip variant list
SDK_VARIANTS_SPC := $(call spc_sep,$(SDK_VARIANTS))
SDK_VARIANTS_UC := $(call var_uc,$(SDK_VARIANTS_SPC))
endif

ifdef SDK_CHIPS
CHIP_CPPFLAGS := CHIP_DEFAULT=0 $(addsuffix =1,$(SDK_CHIPS_UC))
CHIP_CPPFLAGS := $(addprefix -DBCMDRD_CONFIG_INCLUDE_,$(CHIP_CPPFLAGS))
TMP_CPPFLAGS := $(filter-out $(CHIP_CPPFLAGS),$(SDK_CPPFLAGS))
override SDK_CPPFLAGS := $(TMP_CPPFLAGS) $(CHIP_CPPFLAGS)
endif

ifdef SDK_VARIANTS
VAR_CPPFLAGS := VARIANT_DEFAULT=0
VAR_CPPFLAGS += $(foreach C,$(SDK_CHIPS_UC),$(addprefix $(C)_,$(addsuffix =1,$(SDK_VARIANTS_UC))))
VAR_CPPFLAGS := $(addprefix -DBCMLTD_CONFIG_INCLUDE_,$(VAR_CPPFLAGS))
TMP_CPPFLAGS := $(filter-out $(VAR_CPPFLAGS),$(SDK_CPPFLAGS))
override SDK_CPPFLAGS := $(TMP_CPPFLAGS) $(VAR_CPPFLAGS)
endif
