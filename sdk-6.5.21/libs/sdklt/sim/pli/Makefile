#
# This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
# 
# Copyright 2007-2020 Broadcom Inc. All rights reserved.
#

include $(SDK)/make/libname.mk

ifndef SIM_PLI_SYSINTF
SIM_PLI_SYSINTF = posix
endif

ifeq ($(SIM_PLI_SYSINTF),posix)
# Prevent errors in pedantic C99 build
PLI_CPPFLAGS += -D_POSIX_C_SOURCE=200112L
TMP_SDK_CPPFLAGS := $(filter-out $(PLI_CPPFLAGS),$(SDK_CPPFLAGS))
TMP_SDK_CPPFLAGS += $(PLI_CPPFLAGS)
override SDK_CPPFLAGS := $(TMP_SDK_CPPFLAGS)
endif

SUBDIRS := $(SIM_PLI_SYSINTF)
include $(SDK)/make/subdir.mk

include $(SDK)/make/lib.mk
