#
# This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
# 
# Copyright 2007-2020 Broadcom Inc. All rights reserved.
#

include $(SDK)/make/makeutils.mk

include $(SDK)/make/libname.mk

ifndef SYSTEM_INTERFACE
SYSTEM_INTERFACE = xgssim
endif

# Optionally allow multiple probe interfaces
PROBE_INTERFACES := $(SYSTEM_INTERFACE) $(EXTRA_SYSTEM_INTERFACES)
PROBE_INTERFACES_SPC := $(sort $(call spc_sep,$(PROBE_INTERFACES)))
PROBE_INTERFACES_UC := $(call var_uc,$(PROBE_INTERFACES_SPC))

# Define system interface(s)
PROBE_CPPFLAGS := $(addprefix -DBCMA_SYS_PROBE_,$(PROBE_INTERFACES_UC))

# Never auto-create simulated devices when multiple probe interfaces
ifneq (,$(EXTRA_SYSTEM_INTERFACES))
PROBE_CPPFLAGS += -DBCMA_SYS_SIM_OVERRIDE_ONLY=1
endif

override SDK_CPPFLAGS := $(SDK_CPPFLAGS) $(PROBE_CPPFLAGS)
export SDK_CPPFLAGS

ifndef BCMA_SYS_SUBDIRS
BCMA_SYS_SUBDIRS := conf_examples conf probe
endif

# Set base directories
SYS_SUBDIRS := $(BCMA_SYS_SUBDIRS)

# Add appropriate probe system directories
ifneq (,$(filter probe,$(SYS_SUBDIRS)))
SYS_SUBDIRS += $(addprefix probe/,$(PROBE_INTERFACES_SPC))
endif

SUBDIRS := $(SYS_SUBDIRS)

include $(SDK)/make/subdir.mk

include $(SDK)/make/lib.mk
