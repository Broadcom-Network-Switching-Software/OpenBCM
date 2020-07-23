# -*- Makefile -*-
#
# Linux KNET Callback module.
#
# This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
# 
# Copyright 2007-2020 Broadcom Inc. All rights reserved.
#

include Kbuild

ifeq ($(KERNELRELEASE),)

MOD_NAME = linux_ngknetcb

include $(SDK)/make/lkm.mk

endif

.PHONY: distclean

distclean:
