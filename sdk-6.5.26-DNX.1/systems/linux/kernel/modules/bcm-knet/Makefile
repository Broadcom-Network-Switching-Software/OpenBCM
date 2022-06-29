# -*- Makefile -*-
# $Id: Makefile,v 1.3 Broadcom SDK $
# This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
# 
# Copyright 2007-2022 Broadcom Inc. All rights reserved.
#
LOCALDIR = systems/linux/kernel/modules/bcm-knet

include ${SDK}/make/Make.config

LIBS = $(LIBDIR)/libkern.a

KERNEL_MODULE_DIR = kernel_module

THIS_MOD_NAME := linux-bcm-knet
MODULE = $(LIBDIR)/$(THIS_MOD_NAME).o
KMODULE = $(LIBDIR)/$(THIS_MOD_NAME).ko

# BCM Network Device

ifeq (1,$(NO_PRECOMPILED_MODULE))
# {
#
# For 'NO_PRECOMPILED_MODULE', the first kernel compilation is done
# in systems/linux/kernel/modules/bcm-knet (and not in systems/linux/kernel/modules/bcm-knet/kernel_module,
# which is not even created).
# KBUILD_EXTRA_SYMBOLS exports the symbols created on the first module compilation to
# all other module compilations.
#
KBUILD_EXTRA_SYMBOLS := ${BLDDIR}/../../../../bde/linux/kernel/Module.symvers
export KBUILD_EXTRA_SYMBOLS
#
# These are the objects which need to be compiled, in the kernel, to
# created the module object file.
#
SRCS_COMPOSING = bcm-knet.c ../shared/gmodule.c
OBJECTS_COMPOSING = "bcm-knet.o gmodule.o"
#
# Note that for NO_PRECOMPILED_MODULE, the subdirectory  'systems/linux/kernel/modules/bcm-knet/kernel_module'
# is not created and all action is done in systems/linux/kernel/modules/bcm-knet
#

build: $(KMODULE)

$(KMODULE): $(SRCS_COMPOSING)
	rm -fr $(BLDDIR)/$(KERNEL_MODULE_DIR)
	rm -f $(BLDDIR)/*.c
	rm -f $(BLDDIR)/*.o
	rm -f $(BLDDIR)/*.ko
	cp ${SDK}/make/Makefile.linux-kmodule $(BLDDIR)/Makefile
	cp ./*.c $(BLDDIR)/
	cp ../shared/*.c $(BLDDIR)/
	cat ${KBUILD_EXTRA_SYMBOLS} > $(BLDDIR)/Module.symvers
	MOD_OBJS=$(OBJECTS_COMPOSING) MOD_NAME=$(THIS_MOD_NAME) $(MAKE) -C $(BLDDIR) $(THIS_MOD_NAME).ko LOC_BLDDIR=$(BLDDIR) LOC_SRCDIR=$(PWD)
# }
else
# {
KBUILD_EXTRA_SYMBOLS := ${BLDDIR}/../../../../bde/linux/kernel/kernel_module/Module.symvers
export KBUILD_EXTRA_SYMBOLS

build: $(MODULE) $(KMODULE)
#
# This is for the compilation of the 'precompiled' object. We keep it
# here for compatibility.
#
$(MODULE): $(BLDDIR)/.tree $(BOBJS) $(LIBS)
	$(LD) $(MODULE_LDFLAGS) -r -d $(BOBJS) $(LIBS) -o $@

$(KMODULE): $(MODULE)
	rm -fr $(BLDDIR)/$(KERNEL_MODULE_DIR)
	mkdir $(BLDDIR)/$(KERNEL_MODULE_DIR)
	cp ${SDK}/make/Makefile.linux-kmodule $(BLDDIR)/$(KERNEL_MODULE_DIR)/Makefile
	cat ${KBUILD_EXTRA_SYMBOLS} > $(BLDDIR)/$(KERNEL_MODULE_DIR)/Module.symvers
	MOD_NAME=$(THIS_MOD_NAME) $(MAKE) -C $(BLDDIR)/$(KERNEL_MODULE_DIR) $(THIS_MOD_NAME).ko
# }
endif

# Make.depend is before clean:: so that Make.depend's clean:: runs first.

include ${SDK}/make/Make.depend

clean::
	$(RM) $(BLDDIR)/version.c $(BLDDIR)/version.o
	$(RM) $(BOBJS) $(MODULE)

.PHONY: build

