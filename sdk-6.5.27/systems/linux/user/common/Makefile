# -*- Makefile -*-
# $Id: Makefile,v 1.83 Broadcom SDK $
# This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
# 
# Copyright 2007-2022 Broadcom Inc. All rights reserved.
#

# Select a random backoff time (1-20 secs) for retrying 'ln -sf' operations
BACKOFF := $$$$(( (RANDOM % 20) + 1 ))

SDK :=$(shell if [ -n "$$SDK" ] ; then\
	echo $$SDK;\
	else\
	cd $(dir $(lastword $(MAKEFILE_LIST))); while /usr/bin/test ! -e RELEASE ; do \
	dir=`cd ../;pwd`;			\
	if [ "$$dir" = "/" ] ; then		\
	   echo Cannot find SDK in $(lastword $(MAKEFILE_LIST)) 1>&2;		\
	   exit 1;				\
	fi ;					\
	cd $$dir;				\
	done ;					\
	pwd;					\
	fi)

ifeq ($(SDK),)
$(error Please run this in a tree)
endif

export SDK

override-target=unix-user
ifeq ($(LINUX_MAKE_SHARED_LIB),1)
libext = so.$(SHAREDLIBVER)
else
libext = a
endif

ifeq (,$(platform))
$(error Internal error: platform variable is not set)
endif

include $(SDK)/make/Make.config
LOCALDIR=systems/linux/user/common
include $(SDK)/make/Make.depend
kernel-override=linux-$(platform)
ifdef BLDCONFIG
KERN_BLDROOT=${SDK}/${SDKBUILD}/${BLDCONFIG}/$(kernel-override)${all_suffix}$(bldroot_suffix)
else
KERN_BLDROOT=${SDK}/${SDKBUILD}/$(kernel-override)${all_suffix}$(bldroot_suffix)
endif

ifeq (,$(kernel_version))
kernel_version=2_4
endif

ifneq ($(kernel_version),2_4)
KOBJ=ko
else
KOBJ=o
endif

ifneq (,$(TOOLS_DIR))
override PATH := $(TOOLS_DIR):$(PATH)
endif

ifeq ($(DEST_DIR),)
DEST_DIR=${BLDDIR}
endif

ifeq ($(BCM_LIBDIR),)
BCM_LIBDIR := $(LIBDIR)/lib
endif

BCM_LOCAL := bcm.$(targetplat)$(all_suffix)
BCM := $(DEST_DIR)/$(BCM_LOCAL)

ifeq ($(LINUX_MAKE_SHARED_LIB),1)
EDKHOST_LIB_LOCAL := libedkhost.so
export EDKHLIBSO=y
else
EDKHOST_LIB_LOCAL := libedkhost.a
endif
EDKHOST_LIB := $(DEST_DIR)/$(EDKHOST_LIB_LOCAL)

KERNEL_BDE_LOCAL := linux-kernel-bde.$(KOBJ)
KERNEL_BDE := $(DEST_DIR)/$(KERNEL_BDE_LOCAL)

USER_BDE_LOCAL := linux-user-bde.$(KOBJ)
USER_BDE := $(DEST_DIR)/$(USER_BDE_LOCAL)

KNET_CB_LOCAL := linux-knet-cb.$(KOBJ)
KNET_CB := $(DEST_DIR)/$(KNET_CB_LOCAL)

BCM_KNET_LOCAL := linux-bcm-knet.$(KOBJ)
BCM_KNET := $(DEST_DIR)/$(BCM_KNET_LOCAL)

BCM_PTP_CLOCK_LOCAL := linux-bcm-ptp-clock.$(KOBJ)
BCM_PTP_CLOCK := $(DEST_DIR)/$(BCM_PTP_CLOCK_LOCAL)

NETSERVE_LOCAL := netserve
NETSERVE := $(DEST_DIR)/$(NETSERVE_LOCAL)

KERNEL_NGBDE_LOCAL := linux_ngbde.$(KOBJ)
KERNEL_NGBDE := $(DEST_DIR)/$(KERNEL_NGBDE_LOCAL)

KERNEL_NGKNET_LOCAL := linux_ngknet.$(KOBJ)
KERNEL_NGKNET := $(DEST_DIR)/$(KERNEL_NGKNET_LOCAL)

KERNEL_NGKNETCB_LOCAL := linux_ngknetcb.$(KOBJ)
KERNEL_NGKNETCB := $(DEST_DIR)/$(KERNEL_NGKNETCB_LOCAL)

KERNEL_NGEDK_LOCAL := linux_ngedk.$(KOBJ)
KERNEL_NGEDK := $(DEST_DIR)/$(KERNEL_NGEDK_LOCAL)

export LOCAL_TARGET_PATH := $(realpath ..)/$(platform)

ifneq (,$(findstring BCM_COMPILE_OPENNSA,$(CFLAGS)))
LIB_NAME := libopennsa
else
LIB_NAME := libsdk
endif

LIBSDK :=
ifneq (,$(findstring BCM_COMPILE_STATIC_LIB,$(CFLAGS)))
LIBSDK += $(LIB_NAME).a
endif
ifneq (,$(findstring BCM_COMPILE_SHARED_LIB,$(CFLAGS)))
LIBSDK += $(LIB_NAME).so
endif

# Kernel modules
KERNEL_TARGETS := $(KERNEL_BDE) $(USER_BDE)
LOCAL_KERNEL_TARGETS := $(patsubst %,$(realpath ..)/$(platform)/%,$(KERNEL_BDE_LOCAL) $(USER_BDE_LOCAL))

# Main BCM application
BCM_TARGETS := $(BCM)
LOCAL_BCM_TARGETS := $(BCM_LOCAL) $(BCM_LOCAL).dbg
ifeq ($(DNX_FAST_MODE),1)
# We need ASE target as first dependacie to force it start compiling as fast as possible as it takes most of the time
ASE_PREREQ :=
ifneq (,$(findstring libtcl_broadcom.$(libext),$(DIAG_LIBS)))
ifndef NO_ASE
ASE_LOCAL_FILE := src/appl/$(TCLVER)/broadcom/sdk_wrap
ASE_PREREQ := $(BLDROOT)/$(ASE_LOCAL_FILE).o
endif
endif
ifeq ($(filter -g, $(CFLAGS)),)
LOCAL_BCM_TARGETS := $(filter-out $(BCM_LOCAL).dbg, $(LOCAL_BCM_TARGETS))
endif
endif
LOCAL_BCM_TARGETS := $(patsubst %,$(realpath ..)/$(platform)/%,$(LOCAL_BCM_TARGETS))

# EDK-Host build root
ifdef BUILD_EDK
ifeq (, $(EDKHOST))
$(error EDKHOST Path not set)
endif

EDKHLIB ?= y
ifneq ("$(EDKHLIB)", "y")
  ifneq ("$(EDKHLIB)", "n")
    $(error EDKHLIB can be y or n)
  endif
endif

# Say no if YAML support needs to be disabled
EDKH_YAMLLIB ?= y

EDKHOST_BUILDROOT=$(SDK)/$(SDKBUILD)/edk-host/$(TARGET_MACHINE)
EDKELF_BUILDROOT=$(SDK)/$(SDKBUILD)/edk/$(TARGET_MACHINE)
EDKELF_TOOLCHAIN_PATH=/projects/ntsw-tools/toolchains/x86_64/gcc-arm-none-eabi-9-2019-q4-major
EDKELF_CROSS_COMPILE=arm-none-eabi-

ifeq ("$(EDKHLIB)", "y")
  # Default open source root directory
  ifdef OPENSRC_BUILD
  OPENSRC ?= /projects/ntsw-sw/opensrc/build/$(OPENSRC_BUILD)
  endif

  ifdef OPENSRC
  YAML ?= $(wildcard $(OPENSRC)/yaml)
  endif

  ifneq ($(YAML),)
  # Only export YAML if the library exists
  export YAML

  ifeq ($(OPENSRC_LIB32),)
  YAML_LDFLAGS += -L$(YAML)/lib64 -Xlinker -R$(YAML)/lib64
  endif
  YAML_LDFLAGS += -L$(YAML)/lib -Xlinker -R$(YAML)/lib
  YAML_LDLIBS  += -lyaml
  endif # YAML

  ifeq ("$(EDKH_YAMLLIB)", "y")
  LDFLAGS += $(YAML_LDFLAGS) $(YAML_LDLIBS)
  endif

  EDK_LDFLAGS += -T $(EDKHOST)/arch/$(ARCH)/edk-host-image.lds
  ADD_TO_CFLAGS += -DEDKHLIB
else
  LOCAL_BCM_TARGETS += $(patsubst %,$(LOCAL_TARGET_PATH)/%,$(EDKHOST_LIBEXEC_LOCAL))
endif
else
  # No EDK
  EDKHOST :=
endif

EDKHOST_LIBEXEC_LOCAL := edk-host.elf
EDKHOST_LIBEXEC := $(DEST_DIR)/$(EDKHOST_LIBEXEC_LOCAL)
ifeq ("$(EDKELFFW)", "y")
ifeq (, $(EDKELF))
$(error EDKELF Path not set)
endif

ifeq ("$(EDK_DEFCONFIG_DEVICE)", "jericho2")
EDKELFFW_LIBEXEC_LOCAL=bcm88690_cli_debug.elf
endif
ifeq ("$(EDK_DEFCONFIG_DEVICE)", "j2c")
EDKELFFW_LIBEXEC_LOCAL=bcm88800_cli_debug.elf
endif
ifeq ("$(EDK_DEFCONFIG_DEVICE)", "q2a")
EDKELFFW_LIBEXEC_LOCAL=bcm88480_cli_debug.elf
endif
ifeq ("$(EDK_DEFCONFIG_DEVICE)", "j2x")
EDKELFFW_LIBEXEC_LOCAL=bcm88830_cli_debug.elf
endif
EDKELFFW_LIBEXEC=$(DEST_DIR)/$(EDKELFFW_LIBEXEC_LOCAL)
LOCAL_BCM_TARGETS += $(patsubst %,$(LOCAL_TARGET_PATH)/%,$(EDKELFFW_LIBEXEC_LOCAL))
endif

# Additional user mode applications
USER_TARGETS := $(NETSERVE)
LOCAL_USER_TARGETS := $(patsubst %,$(realpath ..)/$(platform)/%,$(NETSERVE_LOCAL))

ifneq ($(LIBSDK),)
# Libraries to be released
LOCAL_LIB_TARGETS := $(patsubst %,$(LIBDIR)/%,$(LIBSDK))
endif

ifeq (,$(findstring DELIVER,$(MAKECMDGOALS)))

# Allow override of default make target
ifeq (,$(DEFAULT_MAKE_TARGET))
.DEFAULT_GOAL := all
else
.DEFAULT_GOAL := $(DEFAULT_MAKE_TARGET)
endif

ifdef BUILD_SDKLT
KERNEL_TARGETS += $(KERNEL_NGBDE)
LOCAL_KERNEL_TARGETS += $(patsubst %,$(LOCAL_TARGET_PATH)/%,$(KERNEL_NGBDE_LOCAL))
ifdef BUILD_EDK
KERNEL_TARGETS += $(KERNEL_NGEDK)
LOCAL_KERNEL_TARGETS += $(patsubst %,$(LOCAL_TARGET_PATH)/%,$(KERNEL_NGEDK_LOCAL))
endif
endif

# Kernel network support
ifdef BUILD_KNET



KERNEL_TARGETS += $(BCM_KNET)
LOCAL_KERNEL_TARGETS += $(patsubst %,$(realpath ..)/$(platform)/%,$(BCM_KNET_LOCAL))


ifdef BUILD_KNET_CB
ifneq (,$(filter $(BCM_KNET),$(KERNEL_TARGETS)))
KERNEL_TARGETS += $(KNET_CB)
LOCAL_KERNEL_TARGETS += $(patsubst %,$(realpath ..)/$(platform)/%,$(KNET_CB_LOCAL))
endif
endif

ifdef BUILD_SDKLT
KERNEL_TARGETS += $(KERNEL_NGKNET)
LOCAL_KERNEL_TARGETS += $(patsubst %,$(realpath ..)/$(platform)/%,$(KERNEL_NGKNET_LOCAL))
ifdef BUILD_KNET_CB
KERNEL_TARGETS += $(KERNEL_NGKNETCB)
LOCAL_KERNEL_TARGETS += $(patsubst %,$(realpath ..)/$(platform)/%,$(KERNEL_NGKNETCB_LOCAL))
endif
endif

ADD_TO_KCFLAGS += -I$(SDK)/systems/linux/kernel/modules/include
COND_KNET_LIBS = libuser.$(libext)

endif # BUILD_KNET


# KnetSync support
ifdef BUILD_KNETSYNC

KERNEL_TARGETS += $(BCM_PTP_CLOCK)
LOCAL_KERNEL_TARGETS += $(patsubst %,$(realpath ..)/$(platform)/%,$(BCM_PTP_CLOCK_LOCAL))

endif # BUILD_KNETSYNC

ifeq ($(NO_LOCAL_TARGETS),)
LOCAL_TARGETS := $(LOCAL_KERNEL_TARGETS) $(LOCAL_BCM_TARGETS) $(LOCAL_USER_TARGETS)
all_targets := $(LOCAL_TARGETS)
else
LOCAL_TARGETS :=
all_targets := $(KERNEL_TARGETS) $(BCM_TARGETS) $(USER_TARGETS)
endif

ifneq ($(LIBSDK),)
all_targets += $(LOCAL_LIB_TARGETS)
endif



export BCM_LIB_PETRA


# Build all targets
all:: $(BLDDIR)/.tree $(all_targets)

ifeq ($(NO_LOCAL_TARGETS),)
# Build kernel modules only
mod: $(LOCAL_KERNEL_TARGETS)

# Build main BCM application only
bcm: $(LOCAL_BCM_TARGETS)

# Build additional user mode applications only
user: $(LOCAL_USER_TARGETS)

define LOCAL_TARGET_DEF
$(1) : $(DEST_DIR)/$(notdir $(1))
	@for i in {1..10}; do echo "Symlinking $1..." ; ln -sf $$? $$@ && break || sleep ${BACKOFF} ; done
endef
endif

# Target specific
ADD_TO_CFLAGS += -DLINUX_SAL_DMA_ALLOC_OVERRIDE

# User BDE libraries
ADD_TO_KCFLAGS += -I$(SDK)/systems/bde/linux/include

CFLAGS += $(ADD_TO_CFLAGS) $(ADD_TO_KCFLAGS)

ifdef ESW_CHIPS
# We already include Make.target with ESW_DIAG_LIBS, filter-out libplibde
ESW_DIAG_LIBS := $(filter-out libplibde.$(libext), $(ESW_DIAG_LIBS))
endif

ifdef LTSW_CHIPS
LT_DIAG_LIBS = libdiag_lt.$(libext)
ifneq (,$(findstring -DBCM_LTSW_SUPPORT,$(CFGFLAGS)))
LTSW_DIAG_LIBS = \
    libdiag_ltsw.$(libext)
endif

ifneq (,$(findstring -DBCM_CTSW_SUPPORT,$(CFGFLAGS)))
LTSW_CNA_DIAG_LIBS = \
	libdiag_ctsw.$(libext)
endif
endif

ifndef NO_SAL_APPL
COND_LIB_SAL_APPL=libsal_appl.$(libext)
COND_LIB_SAL_APPL_PLATFORM=libsal_appl_plat.$(libext)
endif

ifeq ($(BCM_SIM_PATH_SUPPORT),1)
COND_LIB_PLI_BDE = libplibde.$(libext)
PLI = pli
endif

DIAG_LIBS = \
        libdiag.$(libext)       \
        ${ESW_DIAG_LIBS} \
        ${LT_DIAG_LIBS} \
        ${LTSW_DIAG_LIBS} \
        ${CTSW_DIAG_LIBS} \
        ${SAND_DIAG_LIBS} \
	$(COND_LIB_EAV_APPL) \
	${COND_LIB_PHYMOD_DIAG} \
	${COND_LIB_PORTMOD_DIAG} \
        ${COND_LIB_TEST} \
	$(COND_LIB_TCL) \
	$(APPL_LIBS) \
	$(COND_LIB_EDITLINE) \
	$(COND_LIB_WBDM) \
	$(COND_LIB_SMB) \
	$(COND_LIB_PHY_DIAGNOSTICS) \
	$(COND_KNET_LIBS) \
	$(COND_LIB_SAL_APPL) \
	$(COND_LIB_PLI_BDE) \
	liblubde.$(libext)


ifneq (,$(findstring -DBCM_EXT_DPLL_DRV_SUPPORT,$(CFGFLAGS)))
    EXTDPLL_DRV_LIB = libextdpll_drv.$(libext)
endif

ACL_LIB = $(COND_LIB_ACL)
BCM_LIBS = $(DIAG_LIBS) $(DRV_LIBS) $(ACL_LIB) $(COND_LIB_ETH_DRV) $(COND_LIB_SAL_APPL_PLATFORM) $(EXTDPLL_DRV_LIB)
BCM_LIBS_BLD = $(addprefix $(BLDDIR)/,$(BCM_LIBS))

BCM_LIBS_BLD += ${EXTERNAL_LIBS}
ifdef BUILD_EDK
ifeq ("$(EDKHLIB)", "y")
ifdef BUILD_SDKLT
BCM_LIBS_BLD += $(EDKHOST)/chip/built-in.o $(EDKHOST)/apps/built-in.o
else
EDK_LIBS_BLD := $(EDKHOST_BUILDROOT)/$(EDKHOST_LIB_LOCAL) $(EDKHOST_BUILDROOT)/chip/built-in.o $(EDKHOST_BUILDROOT)/apps/built-in.o
BCM_LIBS_BLD += $(EDK_LIBS_BLD)
endif
endif
endif

ifeq ($(NO_MULTIPLE_DEFINITION),1)
# { #
BCM_LIBS_NO_PLAT = $(DIAG_LIBS) $(DRV_LIBS) $(ACL_LIB) $(COND_LIB_ETH_DRV) $(EXTDPLL_DRV_LIB)
BCM_LIBS_NO_PLAT_BLD += $(addprefix $(BLDDIR)/,$(BCM_LIBS_NO_PLAT))
BCM_LIBS_NO_PLAT_BLD += ${EXTERNAL_LIBS}
#
# Prepare a 'unique sorted' list of all libraries. This is for the
# hunt for multiple definitoins (of the same symbol).
# See NO_MULTIPLE_DEFINITION below.
#
SORTED_BCM_LIBS_BLD = $(sort $(BCM_LIBS_NO_PLAT_BLD))
#
# Compile after extracting ALL symbols from each library so that
# multiple definitions of the same symbol will result in an error message.
# Note that libsal_appl_plat.a is taken out of the 'whole archive' scope.
#
BCM_LIBS_FOR_LINKER = $(addprefix $(BLDDIR)/,$(COND_LIB_SAL_APPL_PLATFORM)) -Wl,--whole-archive $(SORTED_BCM_LIBS_BLD)  -Wl,--no-whole-archive
# } #
else
# { #
BCM_LIBS_FOR_LINKER = $(BCM_LIBS_BLD)
# } #
endif

# Kernel network support
ifdef BUILD_KNET

knet_subdirs = shared

knet_subdirs += bcm-knet


ifdef BUILD_KNET_CB
knet_subdirs += knet-cb
endif

endif # BUILD_KNET

ifeq (1,$(BUILD_SDKLT))

# Default open source root directory
ifdef OPENSRC_BUILD
OPENSRC ?= /projects/ntsw-sw/opensrc/build/$(OPENSRC_BUILD)
endif

ifdef OPENSRC
YAML ?= $(wildcard $(OPENSRC)/yaml)
endif

ifneq ($(YAML),)
# Only export YAML if the library exists
export YAML

ifeq ($(OPENSRC_LIB32),)
YAML_LDFLAGS += -L$(YAML)/lib64 -Xlinker -R$(YAML)/lib64
endif
YAML_LDFLAGS += -L$(YAML)/lib -Xlinker -R$(YAML)/lib
YAML_LDLIBS  += -lyaml

LDFLAGS += $(YAML_LDFLAGS) $(YAML_LDLIBS)
else
$(error environment variable YAML not set for yaml library path!)
endif # YAML

ifeq (1,$(BUILD_ISSU))
ISSU_LDFLAGS  = -L$(BCM_LIBDIR) -Xlinker -R$(BCM_LIBDIR)
ISSU_LDFLAGS += -Wl,--export-dynamic
LDFLAGS += $(ISSU_LDFLAGS)
endif

endif # BUILD_SDKLT

ifeq (1,$(BUILD_SDKLT))

# SDKLT_BDE module
SDKLT_BDE = $(SDKLT)/linux/bde
# SDKLT_KNET module
SDKLT_KNET = $(SDKLT)/linux/knet
# SDKLT_KNETCB module
SDKLT_KNETCB = $(SDKLT)/linux/knetcb
# SDKLT_EDK module
SDKLT_EDK = $(SDKLT)/linux/edk
# SDKLT_LKM_BLDROOT
SDKLT_LKM_BLDROOT = $(KERN_BLDROOT)/sdklt

BDE_BLDDIR = $(SDKLT_LKM_BLDROOT)/lkm/bde
KNET_BLDDIR = $(SDKLT_LKM_BLDROOT)/lkm/knet
KNETCB_BLDDIR = $(SDKLT_LKM_BLDROOT)/lkm/knetcb
EDK_BLDDIR = $(SDKLT_LKM_BLDROOT)/lkm/edk

export LKM_CPPFLAGS = $(LTSW_LKM_ADD_CPPFLAGS)
export LKM_CFLAGS = $(LTSW_LKM_ADD_CFLAGS)
export LKM_LDFLAGS = $(LTSW_LKM_ADD_LDFLAGS)

# Support quiet make
ifneq (,$(findstring s,$(MAKEFLAGS)))
quiet = quiet_
KBUILD_VERBOSE = 1
endif

endif # BUILD_SDKLT

ifneq (,$(GO_SUPPORT))
include $(SDK)/make/Make.gotest
endif


# Optionally check elfutils version
ELFUTILS_VERBOSE = 1
include $(SDK)/make/Make.elfutils

# KnetSync Support
ifdef BUILD_KNETSYNC
knetsync_subdirs = bcm-ptp-clock
endif # BUILD_KNETSYNC

KERNEL_MODULES_LIST := kernel_linux kernel_linux_user

kernel_linux: $(KERN_BLDROOT)/.tree
ifdef IPROC_BUILD
ifneq ($(wildcard $(SDK)/tools/diffconfig.pl),)
	$(PERL) $(SDK)/tools/diffconfig.pl \
                $(SDK)/systems/linux/user/$(platform)/linux-$(platform).config $(KERNDIR)/.config
endif
endif
	$(MAKE) -C $(SDK)/systems/bde/linux/kernel kernel_version=$(kernel_version) OPT_CFLAGS="$(ADD_TO_KCFLAGS)"

kernel_linux_user: kernel_linux
	$(MAKE) -C $(SDK)/systems/bde/linux/user/kernel kernel_version=$(kernel_version) OPT_CFLAGS="$(ADD_TO_KCFLAGS)"

ifdef BUILD_SDKLT
KERNEL_MODULES_LIST += kernel_sdklt
kernel_sdklt: kernel_linux
	$(MAKE) -C $(SDKLT_BDE) SDK=$(SDKLT) quiet=$(quiet) LKM_BLDDIR=$(BDE_BLDDIR) KBUILD_VERBOSE=$(KBUILD_VERBOSE)
	(cd $(KERN_BLDROOT); ln -s -f $(BDE_BLDDIR)/*.ko ./)
ifdef BUILD_EDK
KERNEL_MODULES_LIST += kernel_edk
kernel_edk: kernel_sdklt
	$(MAKE) -C $(SDKLT_EDK) SDK=$(SDKLT) quiet=$(quiet) LKM_BLDDIR=$(EDK_BLDDIR) KBUILD_VERBOSE=$(KBUILD_VERBOSE)\
        KBUILD_EXTRA_SYMBOLS=$(BDE_BLDDIR)/Module.symvers
	(cd $(KERN_BLDROOT); ln -s -f $(EDK_BLDDIR)/*.ko ./)
endif	
endif

ifdef BUILD_KNET
KERNEL_MODULES_LIST += kernel_knet
kernel_knet: kernel_linux
	$(MAKE) -C $(SDK)/systems/linux/kernel/modules kernel_version=$(kernel_version) OPT_CFLAGS="$(ADD_TO_KCFLAGS)" \
		subdirs="$(knet_subdirs)" override-target=linux-$(platform)
ifdef BUILD_SDKLT
KERNEL_MODULES_LIST += kernel_knet_sdklt
kernel_knet_sdklt: kernel_sdklt
	$(MAKE) -C $(SDKLT_KNET) SDK=$(SDKLT) quiet=$(quiet) LKM_BLDDIR=$(KNET_BLDDIR) KBUILD_VERBOSE=$(KBUILD_VERBOSE) \
		KBUILD_EXTRA_SYMBOLS=$(BDE_BLDDIR)/Module.symvers
	(cd $(KERN_BLDROOT); ln -s -f $(KNET_BLDDIR)/*.ko ./)
ifdef BUILD_KNET_CB
KERNEL_MODULES_LIST += kernel_knet_cb
kernel_knet_cb: kernel_knet_sdklt
	$(MAKE) -C $(SDKLT_KNETCB) SDK=$(SDKLT) quiet=$(quiet) LKM_BLDDIR=$(KNETCB_BLDDIR) KBUILD_VERBOSE=$(KBUILD_VERBOSE) \
		KBUILD_EXTRA_SYMBOLS=$(KNET_BLDDIR)/Module.symvers
	(cd $(KERN_BLDROOT); ln -s -f $(KNETCB_BLDDIR)/*.ko ./)
endif
endif
endif

ifdef BUILD_KNETSYNC
KERNEL_MODULES_LIST += kernel_knet_sync
kernel_knet_sync: kernel_knet
	$(MAKE) -C $(SDK)/systems/linux/kernel/modules kernel_version=$(kernel_version) OPT_CFLAGS="$(ADD_TO_KCFLAGS)" \
		subdirs="$(knetsync_subdirs)" override-target=linux-$(platform)
endif

.PHONY: $(KERNEL_MODULES_LIST)
kernel_modules: elfutils $(BSL_ENUM_HYBRID) $(KERNEL_MODULES_LIST)


user_libs: $(PRE_AUTO_GENERATED) $(BSL_ENUM_HYBRID)
ifdef CUSTOMER_SOURCE_DIR
	$(MAKE) -C $(CUSTOMER_SOURCE_DIR) CFLAGS="$(CFLAGS)"
endif
ifdef BUILD_SDKLT
	$(MAKE) -C $(SDKLT) SDK=$(SDKLT) SDK_VERSION=$(SDKLT_VER) sdklibs
ifdef BUILD_ISSU
	$(MAKE) issu
else
	mkdir -p $(SDK_VHDR_PATH)
	@echo "const char *bcmsdk_sw_ver = \"$(SDK_VER)\";" > $(SDK_VHDR)
	$(MAKE) -C $(SDKLT)/bcmissu/dll SDK=$(SDKLT) SDK_VERSION=$(SDKLT_VER) issu_db
endif
	$(MAKE) -C $(SDKLT) SDK=$(SDKLT) SDK_VERSION=$(SDKLT_VER) install
ifneq (,$(findstring -fPIC,$(SDK_CFLAGS)))
	@echo "Creating SDKLT shared library for HSDK ..."
	ln -sf $(BCM_LIBDIR)/libsdklt.so $(BCM_LIBDIR)/libsdklt.$(libext)
endif
	@echo "Creating the excluded SDKLT headers for HSDK ..."
	@for t in $(SDK_EXCLUDE_TARGETS); do \
	    rm -rf $(SDK_DSTDIR)/include/sdklt/$$t; \
	    cp -dR $(SDKLT)/$$t/include/* $(SDK_DSTDIR)/include/sdklt; \
	done;
endif
ifneq ($(DNX_FAST_MODE),1)
	$(MAKE) -C $(SDK)/src CFLAGS="$(ADD_TO_CFLAGS)"
endif
	$(MAKE) -C $(SDK)/systems/drv CFLAGS="$(CFLAGS)"
ifdef BUILD_MACSEC
	@${ECHO} "Building MACSEC libraries ...."
	$(MAKE) -C ${MACSEC_HOME} MACSEC_CC="${CC}" MACSEC_LD="${LD}" MACSEC_AR="$(AR)" MACSEC_CFLAGS="${MACSEC_BUILD_FLAG}" MACSEC_TARGET="${targetbase}/${targetplat}/${platform}"
endif

#
# Build ISSU shared libraries.
#
ifdef BUILD_ISSU
issu:
ifeq (,$(SDK_CLEAN))
	$(MAKE) -C $(SDKLT) SDK=$(SDKLT) SDK_VERSION=$(SDKLT_VER) START_SDK_VER=$(SDKLT_VER_START) $@
	$(MAKE) -C $(SDK) $@
else
	$(MAKE) -C $(SDKLT) SDK=$(SDKLT) SDK_CLEAN=$(SDK_CLEAN) SDK_VERSION=$(SDKLT_VER) bcmissudll
	$(MAKE) -C $(SDK) SDK_CLEAN=$(SDK_CLEAN) issu_clean
endif
endif # BUILD_ISSU

# Broadcom Command Monitor

# Build platform_defines.c to compile in config.bcm
#
ifndef NO_SAL_APPL
PLATFORM_DEFINES_OBJ=${BLDDIR}/platform_defines.o
${BLDDIR}/platform_defines.c: ${SDK}/rc/config.bcm
	$(PERL) ${SDK}/tools/bcm2c_linux.pl ${SDK}/rc/config.bcm >$@
${BLDDIR}/platform_defines.o: ${BLDDIR}/platform_defines.c
	${CC} ${CFLAGS} -c $< -o $@
else
PLATFORM_DEFINES_OBJ=
endif

BCM_LIBS_DEPS := $(filter %.$(libext), $(BCM_LIBS_BLD))
BCM_TARGET = $(BCM).dbg
ifeq ($(DNX_FAST_MODE),1)
ifeq ($(filter -g, $(CFLAGS)),)
BCM_TARGET = $(BCM)
endif
BCM_LIBS_DEPS := $(BCM_LIBS_DEPS) | user_libs
else
$(BCM_LIBS_DEPS): | user_libs
endif
pli:
	$(MAKE) -C ${SDK}/systems/bde/pli OPT_CFLAGS="$(ADD_TO_CFLAGS)"


ifneq (,$(findstring BCM_COMPILE_STATIC_LIB,$(CFLAGS)))
ifeq (,$(findstring BCM_COMPILE_OPENNSA,$(CFLAGS)))
LINK_LIBS := $(LIBDIR)/$(LIB_NAME).a
endif
$(LIBDIR)/$(LIB_NAME).a: $(BCM_LIBS_DEPS) $(BLDDIR)/socdiag.o $(BLDDIR)/version.o ${PLATFORM_DEFINES_OBJ}
	(echo create $@; for i in $(sort $(BCM_LIBS_BLD)); do echo addlib $$i; done; echo save) | $(AR) -M
	$(AR) cr $@ $(BLDDIR)/socdiag.o $(BLDDIR)/version.o ${PLATFORM_DEFINES_OBJ}
endif

ifneq (,$(findstring BCM_COMPILE_SHARED_LIB,$(CFLAGS)))
ifeq (,$(findstring BCM_COMPILE_OPENNSA,$(CFLAGS)))
LINK_LIBS := $(LIBDIR)/$(LIB_NAME).so
$(LIBDIR)/$(LIB_NAME).so: $(BLDDIR)/socdiag.o $(BLDDIR)/version.o ${PLATFORM_DEFINES_OBJ} $(BCM_LIBS_DEPS)
	$(CC) -shared -Wl,--allow-multiple-definition -Wl,--whole-archive $^ -Wl,--no-whole-archive -o $@ $(LIBS)
else
$(LIBDIR)/$(LIB_NAME).so: $(BCM_LIBS_DEPS)
	$(CC)  -shared -o  $@ $(CFLAGS) -Wl,--whole-archive $^ -Wl,--no-whole-archive -Wl,--version-script=${SDK}/systems/linux/user/wrx-opennsa/libsdk_export.map $(LIBS)
	$(STRIP) --strip-all --discard-all $(LIBDIR)/$(LIB_NAME).so
endif
endif

LINKER := $(CC)
ifeq ($(C_CPP_CODE_MIX), TRUE)
LINKER := $(CXX)
endif

ifeq ($(CREATE_MAP),1)
    #
    # This will result in the creation of a 'map' file along with the SDK object file.
    # See tools/dnx/make/systems/linux-user-common.mk
    #
    MAPFLAGS = -Xlinker -Map=$@.map
else
    MAPFLAGS =
endif


ifeq ($(CREATE_UNUSED_LIST),1)
#
# (The options 'CFGFLAGS += -ffunction-sections -fdata-sections' in
# compilation makes sure each procedure and each global gets its own
# section name).
# The option '--gc-sections' results in removing sections which are
# not used (e.g., '.rodata').
# The option '--print-gc-sections' prints details of those sections
# It is printed provided the control flag DISABLE_UNUSED_LIST is NOT set
# to '1'
#
LDFLAGS += -Wl,--gc-sections
ifneq ($(DISABLE_UNUSED_LIST),1)
LDFLAGS += -Wl,--print-gc-sections
endif
endif

ifneq ($(LINK_LIBS),)
# { #
$(BCM_TARGET): $(LINK_LIBS) $(EDKELFFW_LIBEXEC) ${PLI}
	mkdir -p $(@D)
	@rm -f $@	# Prevent core dump if currently running
	@echo Linking $@ # This is needed for error parsing
	$(LINKER) $(CFLAGS) -o $@ \
	-Wl,--start-group $(LINK_LIBS) -Wl,--end-group  $(EXT_LDFLAGS) $(LDFLAGS) $(MAPFLAGS)
# } #
else
$(BCM_TARGET): $(EDKELFFW_LIBEXEC) $(BLDDIR)/socdiag.o $(BLDDIR)/version.o ${PLATFORM_DEFINES_OBJ} ${PLI} $(BCM_LIBS_DEPS)
	@rm -f $@	# Prevent core dump if currently running
ifeq (1,$(LINK_STATIC))
	@echo Linking $@ # This is needed for error parsing
else
	@echo Linking $@ dynamically # This is needed for error parsing
endif
	$(LINKER) $(CFLAGS) -o $@ $(BLDDIR)/socdiag.o $(BLDDIR)/version.o \
	-Wl,--start-group  ${PLATFORM_DEFINES_OBJ} $(BCM_LIBS_FOR_LINKER) -Wl,--end-group  $(EXT_LDFLAGS) $(LDFLAGS) $(EDK_LDFLAGS) $(MAPFLAGS)
# } #
endif

ifeq ($(BCM_TARGET),$(BCM).dbg)
$(BCM): $(BCM_TARGET)
	mkdir -p $(@D)
	$(OBJCOPY) --strip-debug $< $@
endif

ifeq ("$(EDKELFFW)", "y")
$(EDKELFFW_LIBEXEC): $(EDKELF_BUILDROOT)/$(EDKELFFW_LIBEXEC_LOCAL)
	mkdir -p $(@D)
	/projects/ntsw-tools/toolchains/x86_64/gcc-arm-none-eabi-9-2019-q4-major/bin/arm-none-eabi-objcopy --strip-debug $(EDKELF_BUILDROOT)/$(EDKELFFW_LIBEXEC_LOCAL) $@    
else
$(EDKHOST_LIBEXEC): user_libs
	mkdir -p $(@D)
	$(OBJCOPY) --strip-debug $(EDKHOST_BUILDROOT)/$(EDKHOST_LIBEXEC_LOCAL) $@
endif

$(KERNEL_BDE): $(KERN_BLDROOT)/linux-kernel-bde.$(KOBJ)
	mkdir -p $(@D)
	$(OBJCOPY) --strip-debug $< $@

$(USER_BDE): $(KERN_BLDROOT)/linux-user-bde.$(KOBJ)
	$(OBJCOPY) --strip-debug $< $@

$(KNET_CB): $(KERN_BLDROOT)/$(KNET_CB_LOCAL)
	$(OBJCOPY) --strip-debug $< $@

$(BCM_KNET): $(KERN_BLDROOT)/$(BCM_KNET_LOCAL)
	$(OBJCOPY) --strip-debug $< $@

$(KERNEL_NGBDE): $(KERN_BLDROOT)/linux_ngbde.$(KOBJ)
	$(OBJCOPY) --strip-debug $< $@

$(KERNEL_NGKNET): $(KERN_BLDROOT)/linux_ngknet.$(KOBJ)
	$(OBJCOPY) --strip-debug $< $@

$(KERNEL_NGKNETCB): $(KERN_BLDROOT)/linux_ngknetcb.$(KOBJ)
	$(OBJCOPY) --strip-debug $< $@

$(KERNEL_NGEDK): $(KERN_BLDROOT)/linux_ngedk.$(KOBJ)
	$(OBJCOPY) --strip-debug $< $@

$(BCM_PTP_CLOCK): $(KERN_BLDROOT)/$(BCM_PTP_CLOCK_LOCAL)
	$(OBJCOPY) --strip-debug $< $@

$(KERN_BLDROOT)/%.$(KOBJ): kernel_modules
	@echo Force execution of kernel_modules target > /dev/null

$(NETSERVE): $(BLDDIR)/netserve.o
	mkdir -p $(@D)
	$(CC) $(CFLAGS) -o $@ $(BLDDIR)/netserve.o $(LDFLAGS) -lutil
	$(OBJCOPY) --strip-debug $@

ifeq ($(NO_LOCAL_TARGETS),)
$(foreach targ,$(LOCAL_TARGETS),$(eval $(call LOCAL_TARGET_DEF,$(targ))))
endif

# Build Version
$(BLDDIR)/version.c: $(BLDDIR)/socdiag.o user_libs
	$(PERL) $(SDK)/tools/version.pl > $@
$(BLDDIR)/version.o: $(BLDDIR)/version.c
	$(CC) $(CFLAGS) -c $< -o $@

go:
ifeq ($(GO_SUPPORT),)
	@echo "Building Go Not Supported"
else
	# Rename main in socdiag.o.  Go has its own main, so this resolves the conflict.
	$(OBJCOPY) --redefine-sym main=main_app  $(BLDDIR)/socdiag.o $(BLDDIR)/socdiag_go.o
	$(MAKE) go-swig
	@echo "Building Go"
	$(MAKE) go-image
endif

clean::
	$(MAKE) -C $(SDK)/systems/bde/linux/kernel $@
	$(MAKE) -C $(SDK)/systems/bde/linux/user/kernel $@
	$(MAKE) -C $(SDK)/systems/bde/linux/user CFLAGS="$(CFLAGS)" $@
	$(MAKE) -C $(SDK)/systems/drv $@
	$(MAKE) -C $(SDK)/systems/drv/eth $@
	$(MAKE) -C $(SDK)/src CFLAGS="$(CFLAGS)" $@
ifdef BUILD_EDK
ifndef BUILD_SDKLT
	$(MAKE) -C $(EDKHOST) V=0 O=$(EDKHOST_BUILDROOT) $@
endif
endif
ifdef BUILD_SDKLT
	$(MAKE) -C $(SDKLT) SDK=$(SDKLT) $@
ifdef BUILD_ISSU
	$(MAKE) -C $(SDKLT) SDK=$(SDKLT) SDK_CLEAN=$@ SDK_VERSION=$(SDKLT_VER) bcmissudll
	$(MAKE) -C $(SDK) SDK_CLEAN=$@ issu_clean
else
	$(RM) $(SDK_VHDR_PATH)
endif
endif
ifdef BUILD_PHYMOD
	$(MAKE) -C $(SDK)/libs/phymod LINUX_MAKE_SHARED_LIB=${LINUX_MAKE_SHARED_LIB} SHAREDLIBVER=${SHAREDLIBVER} $@
endif
ifdef BUILD_PCIEPHY
	$(MAKE) -C $(SDK)/libs/pciephy LINUX_MAKE_SHARED_LIB=${LINUX_MAKE_SHARED_LIB} SHAREDLIBVER=${SHAREDLIBVER} $@
endif
ifdef BUILD_KAPS
ifeq ($(shell test -f $(SDK)/libs/kaps/Makefile; echo $$?),0)
	$(MAKE) -C $(SDK)/libs/kaps $@
endif
endif
ifdef BUILD_KAPS_JR1
ifeq ($(shell test -f $(SDK)/libs/kaps_jr1/Makefile; echo $$?),0)
	$(MAKE) -C $(SDK)/libs/kaps_jr1 $@
endif
endif
ifdef BUILD_FLEXE
	$(MAKE) -C $(SDK)/libs/flexe_core $@
endif
ifdef BUILD_FRAMER
	$(MAKE) -C $(SDK)/libs/framer $@
endif
ifdef BUILD_AAPL
	$(MAKE) -C $(SDK)/libs/aapl $@
endif
	$(MAKE) -C $(SDK)/systems/linux/kernel/user/shared $@
	$(MAKE) -C $(SDK)/systems/linux/kernel/modules \
		subdirs="$(knet_subdirs)" \
		override-target=linux-$(platform) $@
	$(RM) $(BLDDIR)/version.c $(BLDDIR)/version.o
	$(RM) $(BLDDIR)/platform_defines.c $(BLDDIR)/platform_defines.o
	$(RM) $(BLDDIR)/socdiag.o
	$(RM) $(KERNEL_BDE) $(USER_BDE) $(BCM) $(BCM).dbg
	$(RM) $(BCM_KNET)
	$(RM) $(BCM_PTP_CLOCK)
ifdef BUILD_SDKLT
	$(RM) $(KERNEL_NGBDE) $(KERNEL_NGKNET) $(KERNEL_NGKNETCB) $(KERNEL_NGEDK)
endif
	$(RM) $(KERN_BLDROOT)/linux-kernel-bde.$(KOBJ)
	$(RM) $(KERN_BLDROOT)/linux-user-bde.$(KOBJ)
	$(RM) $(KERN_BLDROOT)/linux-bcm-knet.$(KOBJ)
	$(RM) $(KERN_BLDROOT)/linux-bcm-ptp-clock.$(KOBJ)
ifdef BUILD_SDKLT
	$(RM) $(KERN_BLDROOT)/$(KERNEL_NGBDE_LOCAL)
	$(RM) $(KERN_BLDROOT)/$(KERNEL_NGKNET_LOCAL)
	$(RM) $(KERN_BLDROOT)/$(KERNEL_NGKNETCB_LOCAL)
	$(RM) $(KERN_BLDROOT)/$(KERNEL_NGEDK_LOCAL)
	$(RM) $(KERN_BLDROOT)/sdklt
	$(RM) $(BLDDIR)/include
	$(RM) $(BLDDIR)/lib
	$(RM) $(BLDDIR)/sdklt
endif
	$(RM) $(BLDDIR)/netserve.o
	$(RM) $(NETSERVE) $(LOCAL_TARGETS)
ifdef BUILD_EXT_SERVO
	@${ECHO} "Clean External servo library ....$(CPU_PLATFORM)"
	$(MAKE) -C ${EXT_SERVO_BUILD_HOME} TARGET=linux.$(CPU_PLATFORM) clean
endif
ifdef BUILD_XFLOW_MACSEC
	$(MAKE) -C ${XFLOW_MACSEC_HOME} XFLOW_MACSEC_CC="${CC}" XFLOW_MACSEC_LD="${LD}" XFLOW_MACSEC_BLDDIR="${XFLOW_MACSEC_BLDDIR}" XFLOW_MACSEC_LIBDIR="${BCM_LIB_XFLOW_MACSEC_ROOT}" \
		XFLOW_MACSEC_TARGET="${targetbase}/$(targetplat)/${platform}" clean
endif
ifneq ($(GO_SUPPORT),)
	$(MAKE) go-clean
endif

distclean::
	$(RM) $(BLDROOT)
	$(RM) $(KERN_BLDROOT)
	$(RM) $(DEST_DIR)
ifdef BUILD_EDK
	$(MAKE) -C $(EDKHOST) V=0 $@
	$(RM) $(EDKHOST_BUILDROOT)
endif

.PHONY: variable FORCE

#
# Echo variable values used for configuration
# usage: make VAR=CC variable
#
variable::
	@echo $($(VAR))

else # ifeq (,$(findstring DELIVER,$(MAKECMDGOALS)))

ifndef DELIVERY
$(error DELIVERY required)
endif

DELIVERABLES := $(KERNEL_BDE) $(USER_BDE) $(BCM) $(NETSERVE)

ifneq ($(filter -g, $(CFLAGS)),)
DELIVERABLES += $(BCM).dbg
endif 
# 1= source
define DELIVERY_RULE
_DEL_TARG += $(DELIVERY)/$(notdir $(1))
$(DELIVERY)/$(notdir $(1)) : $(1)
	@mkdir -p $(DELIVERY)
ifeq ($(filter $(notdir $(BCM)) $(notdir $(BCM).dbg),$(notdir $1)),)
	$(OBJCOPY) --strip-debug $$< $$@
else
	$(CP) -p $$< $$@
endif
endef

$(foreach f,$(DELIVERABLES),$(eval $(call DELIVERY_RULE,$(f))))

DELIVER: $(_DEL_TARG)

endif # ifeq (,$(findstring DELIVER,$(MAKECMDGOALS)))

# Make.config defines remaining phony targets
.PHONY: build kernel_modules user_libs pli DELIVER mod bcm user issu go



EXTRA_CONFIG_MAKE_FLAGS = CFLAGS="$(ADD_TO_CFLAGS)"

# $(error LIBS = $(BCM_LIBS_BLD))
#

ifdef BUILD_EDK
ifndef BUILD_SDKLT
.PHONY: edk_targets
edk_targets:
	$(MAKE) -C $(EDKHOST) $(TARGET_MACHINE)_$(EDK_DEFCONFIG_DEVICE)_defconfig Q=$(Q) O=$(EDKHOST_BUILDROOT) TOOLCHAIN_BASE_DIR=$(TOOLCHAIN_BASE_DIR) CROSS_COMPILE=$(CROSS_COMPILE)
	$(MAKE) -C $(EDKHOST) EDKHLIB=$(EDKHLIB) Q=$(Q) O=$(EDKHOST_BUILDROOT) TOOLCHAIN_BASE_DIR=$(TOOLCHAIN_BASE_DIR) CROSS_COMPILE=$(CROSS_COMPILE)

ifeq ("$(EDKELFFW)", "y")
$(EDKELF_BUILDROOT)/$(EDKELFFW_LIBEXEC_LOCAL): FORCE
	$(MAKE) -C $(EDKELF) $(EDK_DEFCONFIG_DEVICE)_cli_defconfig Q=$(Q) O=$(EDKELF_BUILDROOT) TOOLCHAIN_PATH=$(EDKELF_TOOLCHAIN_PATH) CROSS_COMPILE=$(EDKELF_CROSS_COMPILE)
	$(MAKE) -C $(EDKELF) Q=$(Q) O=$(EDKELF_BUILDROOT) TOOLCHAIN_BASE_DIR=$(TOOLCHAIN_BASE_DIR) TOOLCHAIN_PATH=$(EDKELF_TOOLCHAIN_PATH) CROSS_COMPILE=$(EDKELF_CROSS_COMPILE)
endif

$(EDK_LIBS_BLD): edk_targets
endif
endif

$(LIBDIR)/liblubde.%: FORCE
	$(MAKE) -C $(SDK)/systems/bde/linux/user CFLAGS="$(CFLAGS)"

ifdef BUILD_EXT_SERVO
$(EXT_SERVO_LIB): FORCE
	@${ECHO} "Building External servo library .... $(CPU_PLATFORM)"
	$(MAKE) -C ${EXT_SERVO_BUILD_HOME} TARGET=linux.$(CPU_PLATFORM) CPU_TARGET=$(CPU_PLATFORM) USER_ARFLAGS=rcvs CLOCK_TYPE=BC NUMBER_OF_STACK_INSTANCES=10 DCO_DEVICE=softwareDco TIMESTAMP_DEVICE=softwareTimestamping
endif

ifdef BUILD_XFLOW_MACSEC
.PHONY: xflow_macsec_libs
XFLOW_MACSEC_DEPS :=
ifdef BUILD_SDKLT
XFLOW_MACSEC_DEPS := user_libs
endif
xflow_macsec_libs: $(XFLOW_MACSEC_DEPS) FORCE
	@${ECHO} "Building XFLOW_MACSEC libraries ...."
	$(MAKE) -C ${XFLOW_MACSEC_HOME} XFLOW_MACSEC_BLDDIR="${XFLOW_MACSEC_BLDDIR}" XFLOW_MACSEC_LIBDIR="${BCM_LIB_XFLOW_MACSEC_ROOT}" XFLOW_MACSEC_CC="${CC}" XFLOW_MACSEC_LD="${LD}" XFLOW_MACSEC_AR="$(AR)" XFLOW_MACSEC_CFLAGS="${XFLOW_MACSEC_BUILD_FLAG}" XFLOW_MACSEC_TARGET="${targetbase}/${targetplat}/${platform}" CFLAGS="$(CFLAGS)"

$(BCM_LIB_XFLOW_MACSEC): xflow_macsec_libs
endif

ifdef BUILD_KNET
$(LIBDIR)/libuser.$(libext): FORCE
	$(MAKE) -C $(SDK)/systems/linux/kernel/user/shared CFLAGS="$(CFLAGS)"
endif

ifdef BUILD_PHYMOD
$(LIBDIR)/libphymod.$(libext): FORCE
	$(MAKE) -C $(SDK)/libs/phymod LINUX_MAKE_SHARED_LIB=${LINUX_MAKE_SHARED_LIB} SHAREDLIBVER=${SHAREDLIBVER}
endif

ifdef BUILD_PCIEPHY
$(LIBDIR)/libpciephy.$(libext): FORCE
	$(MAKE) -C $(SDK)/libs/pciephy LINUX_MAKE_SHARED_LIB=${LINUX_MAKE_SHARED_LIB} SHAREDLIBVER=${SHAREDLIBVER}
endif

ifdef BUILD_FLEXE
$(LIBDIR)/libflexe_core_std.$(libext): FORCE
	$(MAKE) -C $(SDK)/libs/flexe_core/std FLEXE_CFLAGS="$(ADD_TO_CFLAGS)" CFLAGS="$(CFLAGS)"
endif

ifdef BUILD_FLEXE_DBG
$(LIBDIR)/libflexe_core_dbg.$(libext): FORCE
	$(MAKE) -C $(SDK)/libs/flexe_core/dbg FLEXE_CFLAGS="$(ADD_TO_CFLAGS)" CFLAGS="$(CFLAGS)"
endif

ifdef BUILD_FRAMER
$(LIBDIR)/$(FRAMER_LIB_NAME).$(libext): FORCE
	$(MAKE) -C $(SDK)/libs/framer
endif

ifdef BUILD_AAPL
$(LIBDIR)/libaapl.$(libext): FORCE
	$(MAKE) -C $(SDK)/libs/aapl
endif

ifdef BUILD_PHY_BRIDGE
$(LIBDIR)/libphy_bridge.$(libext): FORCE
	$(MAKE) -C $(SDK)/libs/phy_bridge
endif

ifdef BUILD_KAPS
ifeq ($(shell test -f $(SDK)/libs/kaps/Makefile; echo $$?),0)
$(LIBDIR)/libkaps.$(libext): FORCE
	$(MAKE) -C $(SDK)/libs/kaps
endif
endif

ifdef BUILD_KAPS_JR1
ifeq ($(shell test -f $(SDK)/libs/kaps_jr1/Makefile; echo $$?),0)
$(LIBDIR)/libkaps_jr1.$(libext): FORCE
	$(MAKE) -C $(SDK)/libs/kaps_jr1
endif
endif

ifeq ($(DNX_FAST_MODE),1)
-include $(SDK)/tools/dnx/make/non_recursive_common.mk
endif
