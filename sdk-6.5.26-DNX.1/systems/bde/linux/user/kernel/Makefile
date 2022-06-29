# -*- Makefile -*-
# $Id: Makefile,v 1.13 Broadcom SDK $
# This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
# 
# Copyright 2007-2022 Broadcom Inc. All rights reserved.
#
LOCALDIR = systems/bde/linux/user/kernel

# Make sure we build for the kernel if this is a user-mode build
ifneq ($(platform), )
override-target=linux-$(platform)
endif

include $(SDK)/make/Make.config

LIBS = $(LIBDIR)/libkern.a

KERNEL_MODULE_DIR = kernel_module
THIS_MOD_NAME := linux-user-bde
MODULE = $(LIBDIR)/$(THIS_MOD_NAME).o
KMODULE = $(LIBDIR)/$(THIS_MOD_NAME).ko
build: kernel_libs module $(KMODULE)

ifeq (1,$(NO_PRECOMPILED_MODULE))
# {
#
# For 'NO_PRECOMPILED_MODULE', the first kernel compilation is done
# in systems/bde/linux/user/kernel (and not in systems/bde/linux/user/kernel/kernel_module,
# which is not even created).
# KBUILD_EXTRA_SYMBOLS exports the symbols created on the first module compilation to
# all other module compilations.
#
KBUILD_EXTRA_SYMBOLS := ${BLDDIR}/../../kernel/Module.symvers
export KBUILD_EXTRA_SYMBOLS
#
# These are the objects which need to be compiled, in the kernel, to
# created the module object file.
#
SRCS_COMPOSING = $(THIS_MOD_NAME).c ../../../../linux/kernel/modules/shared/gmodule.c
OBJECTS_COMPOSING = "$(THIS_MOD_NAME)-file.o gmodule.o"
#
# Note that for NO_PRECOMPILED_MODULE, the subdirectory 'systems/bde/linux/user/kernel/kernel_module'
# is not created and all action is done in systems/bde/linux/user/kernel
#
module: $(KMODULE)

#
# Note that for NO_PRECOMPILED_MODULE, the subdirectory 'systems/bde/linux/kernel/kernel_module'
# is not created and all action is done in systems/bde/linux/kernel
#
$(KMODULE): $(SRCS_COMPOSING)
	rm -fr $(BLDDIR)/$(KERNEL_MODULE_DIR)
	rm -f $(BLDDIR)/*.c
	rm -f $(BLDDIR)/*.o
	rm -f $(BLDDIR)/*.ko
	cp ${SDK}/make/Makefile.linux-kmodule $(BLDDIR)/Makefile
	cp ./*.c $(BLDDIR)/
	cp ./*.h $(BLDDIR)/
	cp ../../../../linux/kernel/modules/shared/*.c $(BLDDIR)/
	cat ${KBUILD_EXTRA_SYMBOLS} > $(BLDDIR)/Module.symvers
	#
	# We change here the name of the 'base' file since, it seems, the module-creation
	# mechanism does not add symbols when the name of one of the files is the same as
	# the name of the module itself.
	#
	mv $(BLDDIR)/$(THIS_MOD_NAME).c $(BLDDIR)/$(THIS_MOD_NAME)-file.c
	MOD_OBJS=$(OBJECTS_COMPOSING) MOD_NAME=$(THIS_MOD_NAME) $(MAKE) -C $(BLDDIR) $(THIS_MOD_NAME).ko LOC_BLDDIR=$(BLDDIR) LOC_SRCDIR=$(PWD)
# }
else
# {
KBUILD_EXTRA_SYMBOLS := ${BLDDIR}/../../kernel/kernel_module/Module.symvers
export KBUILD_EXTRA_SYMBOLS

module: kernel_libs $(MODULE)

#
# This is for the compilation of the 'precompiled' object. We keep it
# here for compatibility.
#
$(MODULE): $(BLDDIR)/.tree $(BOBJS)
	$(LD) $(MODULE_LDFLAGS) -r -d $(BOBJS) $(LIBS) -o $@

$(KMODULE): $(MODULE)
	rm -fr $(BLDDIR)/$(KERNEL_MODULE_DIR)
	mkdir $(BLDDIR)/$(KERNEL_MODULE_DIR)
	cp ${SDK}/make/Makefile.linux-kmodule $(BLDDIR)/$(KERNEL_MODULE_DIR)/Makefile
	cat ${KBUILD_EXTRA_SYMBOLS} > $(BLDDIR)/$(KERNEL_MODULE_DIR)/Module.symvers
	MOD_NAME=$(THIS_MOD_NAME) $(MAKE) -C $(BLDDIR)/$(KERNEL_MODULE_DIR) $(THIS_MOD_NAME).ko
# }
endif

kernel_libs:
	$(MAKE) -C $(SDK)/systems/linux/kernel/modules/shared

include $(SDK)/make/Make.depend

# Make.depend is before clean:: so that Make.depend's clean:: runs first.

clean::
	$(MAKE) -C $(SDK)/systems/linux/kernel/modules/shared $@
	$(RM) $(BOBJS) $(MODULE)
	$(RM) $(BLDDIR)/$(KERNEL_MODULE_DIR)

distclean::

.PHONY: build kernel_libs
