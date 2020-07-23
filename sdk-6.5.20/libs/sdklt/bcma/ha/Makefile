#
# This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
# 
# Copyright 2007-2020 Broadcom Inc. All rights reserved.
#

include $(SDK)/make/libname.mk

ifndef HA_MEM
HA_MEM = shm
endif

ifeq ($(HA_MEM),shm)
override SDK_CPPFLAGS := $(SDK_CPPFLAGS) -D_XOPEN_SOURCE=600
export SDK_CPPFLAGS
endif

SUBDIRS := cmd mem/$(HA_MEM)
include $(SDK)/make/subdir.mk

include $(SDK)/make/lib.mk
