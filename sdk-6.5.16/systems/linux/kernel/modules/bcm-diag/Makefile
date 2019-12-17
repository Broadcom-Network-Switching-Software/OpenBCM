# -*- Makefile -*-
# 
# This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
# 
# Copyright 2007-2019 Broadcom Inc. All rights reserved.
#
LOCALDIR = systems/linux/kernel/modules/bcm-diag

include ${SDK}/make/Make.config

DIAG = ${COND_LIB_BCMX_DIAG} libdiag.a


ifdef ESW_CHIPS
DIAG += libdiag_esw.a \
        libtsl3uc.a \
        libtsbasic.a \
        libtsl3mc.a \
        libtsmpls.a \
        libtsmmu.a \
        libtsoam.a \
        libtsniv.a \
        libtsriot.a \
        libtsvlan.a \
        libtsvxlan.a \
        libtsfflow.a \
        libtsifp.a \
        libtsvfp.a \
        libtsefp.a \
        libtscos.a \
        libtsloadbalance.a \
        libtsdgm.a \
        libtstcb.a \
        libtspstat.a \
        libtsphy.a \
        libtsledup.a \
        libtscmic.a \
        libtsvisibility.a \
        libtswarmboot.a \
        libtechsupport.a
DIAG += libtsetrap.a
DIAG += libtsint.a
DIAG += libtshistogram.a
DIAG += libtsalpm.a
DIAG += libtscancun.a
endif

ifdef DPP_CHIPS
DIAG += libdiag_dpp.a
endif

ifdef DFE_CHIPS
DIAG += libdiag_dfe.a libdiag_dcmn.a
else
ifdef DPP_CHIPS
DIAG += libdiag_dcmn.a
endif
endif

ifdef PORTMOD_SUPPORT
DIAG += libdiag_portmod.a
endif

ifndef NO_SAL_APPL
COND_LIB_SAL_APPL=libsal_appl.a
COND_LIB_SAL_APPL_PLATFORM=libsal_appl_plat.a
endif

DIAG_LIBS = \
	${DIAG}\
	${COND_LIB_PHYMOD_DIAG} \
	${COND_LIB_PORTMOD_DIAG} \
	libkern.a \
	${COND_LIB_SAL_APPL} \
	${COND_LIB_EDITLINE} \
	${COND_LIB_DIAG_APS} \
	${COND_LIB_SAL_APPL_PLATFORM}

BCM_LIBS = $(DIAG_LIBS) $(COND_LIB_ETH_DRV)
BCM_LIBS_BLD = $(addprefix $(LIBDIR)/,$(BCM_LIBS))
ifndef BUILD_MACSEC
BCM_LIBS_BLD += ${EXTERNAL_LIBS}
endif

ifeq ($(kernel_version),2_4)
MODULE = $(LIBDIR)/linux-bcm-diag.o
else
KERNEL_MODULE_DIR = kernel_module

THIS_MOD_NAME := linux-bcm-diag
MODULE = $(LIBDIR)/$(THIS_MOD_NAME).o
KMODULE = $(LIBDIR)/$(THIS_MOD_NAME).ko

build: $(MODULE) $(KMODULE)
endif

KBUILD_EXTRA_SYMBOLS := ${BLDDIR}/../bcm-core/kernel_module/Module.symvers
KBUILD_EXTRA_SYMBOLS += ${BLDDIR}/../uk-proxy/kernel_module/Module.symvers

# BCM Diag Shell

$(MODULE): $(BLDDIR)/.tree $(BOBJS) $(BCM_LIBS_BLD)
	$(LD) $(MODULE_LDFLAGS) -r -d $(BOBJS) --start-group $(BCM_LIBS_BLD) --end-group -o $@
ifneq ($(kernel_version),2_4)
$(KMODULE): $(MODULE)
	rm -fr $(BLDDIR)/$(KERNEL_MODULE_DIR)
	mkdir $(BLDDIR)/$(KERNEL_MODULE_DIR)
	cp ${SDK}/make/Makefile.linux-kmodule $(BLDDIR)/$(KERNEL_MODULE_DIR)/Makefile
	cat ${KBUILD_EXTRA_SYMBOLS} > $(BLDDIR)/$(KERNEL_MODULE_DIR)/Module.symvers
	MOD_NAME=$(THIS_MOD_NAME) $(MAKE) -C $(BLDDIR)/$(KERNEL_MODULE_DIR) $(THIS_MOD_NAME).ko
endif

# Make.depend is before clean:: so that Make.depend's clean:: runs first.

include ${SDK}/make/Make.depend

clean::
	$(RM) $(BLDDIR)/version.c $(BLDDIR)/version.o
	$(RM) $(BOBJS) $(MODULE)

ifneq ($(kernel_version),2_4)
.PHONY: build
endif
