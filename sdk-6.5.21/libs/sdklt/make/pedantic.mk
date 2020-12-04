#
# This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
# 
# Copyright 2007-2020 Broadcom Inc. All rights reserved.
#
# Pedantic build target.
#
# This makefile is intended for sanity builds at the component level, e.g.:
#
#   cd $SDK/bcmdrd
#   make pedantic
#

include $(SDK)/make/pedantic_gcc.mk

pedantic:
	$(MAKE) SDK_CFLAGS="$(PEDANTIC_GCC_FLAGS)" \
		SDK_BLDDIR=`pwd`/build/pedantic

clean::
	-rm -rf `pwd`/build/pedantic

.PHONY: pedantic
