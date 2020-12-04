# -*- Makefile -*-
# $Id$
# This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
# 
# Copyright 2007-2020 Broadcom Inc. All rights reserved.
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

BCM_LOCAL := demo_opennsa_init.$(targetplat)$(all_suffix)
BCM := $(DEST_DIR)/$(BCM_LOCAL)

KERNEL_BDE_LOCAL := linux-kernel-bde.$(KOBJ)
KERNEL_BDE := $(DEST_DIR)/$(KERNEL_BDE_LOCAL)

USER_BDE_LOCAL := linux-user-bde.$(KOBJ)
USER_BDE := $(DEST_DIR)/$(USER_BDE_LOCAL)

KNET_CB_LOCAL := linux-knet-cb.$(KOBJ)
KNET_CB := $(DEST_DIR)/$(KNET_CB_LOCAL)

BCM_KNET_LOCAL := linux-bcm-knet.$(KOBJ)
BCM_KNET := $(DEST_DIR)/$(BCM_KNET_LOCAL)

KERNEL_NGBDE_LOCAL := linux_ngbde.$(KOBJ)
KERNEL_NGBDE := $(DEST_DIR)/$(KERNEL_NGBDE_LOCAL)

KERNEL_NGKNET_LOCAL := linux_ngknet.$(KOBJ)
KERNEL_NGKNET := $(DEST_DIR)/$(KERNEL_NGKNET_LOCAL)

OPENNSA = libopennsa.a libopennsa.so

LOCAL_TARGET_PATH := $(realpath ..)/$(platform)

# Kernel modules
KERNEL_TARGETS := $(KERNEL_BDE) $(USER_BDE)
LOCAL_KERNEL_TARGETS := $(patsubst %,$(realpath ..)/$(platform)/%,$(KERNEL_BDE_LOCAL) $(USER_BDE_LOCAL))

# Main BCM application
BCM_TARGETS := $(BCM)
LOCAL_BCM_TARGETS := $(patsubst %,$(realpath ..)/$(platform)/%,$(BCM_LOCAL) $(BCM_LOCAL).dbg)

# Libraries to be released
LIB_TARGETS := $(patsubst %,$(DEST_DIR)/%,$(OPENNSA))
LOCAL_LIB_TARGETS := $(patsubst %,$(realpath ..)/$(platform)/%,$(OPENNSA))

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
endif

# Kernel network support
ifdef BUILD_KNET

ifdef ESW_CHIPS
KERNEL_TARGETS += $(BCM_KNET)
LOCAL_KERNEL_TARGETS += $(patsubst %,$(realpath ..)/$(platform)/%,$(BCM_KNET_LOCAL))
endif

ifdef BUILD_KNET_CB
KERNEL_TARGETS += $(KNET_CB)
LOCAL_KERNEL_TARGETS += $(patsubst %,$(realpath ..)/$(platform)/%,$(KNET_CB_LOCAL))
endif

ifdef BUILD_SDKLT
KERNEL_TARGETS += $(KERNEL_NGKNET)
LOCAL_KERNEL_TARGETS += $(patsubst %,$(realpath ..)/$(platform)/%,$(KERNEL_NGKNET_LOCAL))
endif

ADD_TO_KCFLAGS += -I$(SDK)/systems/linux/kernel/modules/include
COND_KNET_LIBS = libuser.$(libext)

endif # BUILD_KNET

ifeq ($(NO_LOCAL_TARGETS),)
LOCAL_TARGETS := $(LOCAL_KERNEL_TARGETS) $(LOCAL_BCM_TARGETS) $(LOCAL_LIB_TARGETS)
all_targets := $(LOCAL_TARGETS)
else
LOCAL_TARGETS :=
all_targets := $(KERNEL_TARGETS) $(BCM_TARGETS) $(LIB_TARGETS)
endif



export BCM_LIB_PETRA

# Build all targets
all: $(BLDDIR)/.tree $(all_targets)

libopennsa: user_libs $(LOCAL_LIB_TARGETS)

ifeq ($(NO_LOCAL_TARGETS),)
# Build kernel modules only
mod: $(LOCAL_KERNEL_TARGETS)

# Build main BCM application only
bcm: $(LOCAL_BCM_TARGETS)

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
ESW_DIAG_LIBS = libdiag_esw.$(libext) \
                libtsl3uc.$(libext) \
                libtsl3mc.$(libext) \
                libtsmpls.$(libext) \
                libtsmmu.$(libext) \
                libtsoam.$(libext) \
                libtsniv.$(libext) \
                libtsriot.$(libext) \
                libtsvlan.$(libext) \
                libtsvxlan.$(libext) \
                libtsfflow.$(libext) \
                libtsbasic.$(libext) \
                libtsifp.$(libext) \
                libtsvfp.$(libext) \
                libtsefp.$(libext) \
                libtscos.$(libext) \
                libtsloadbalance.$(libext) \
                libtsdgm.$(libext) \
                libtstcb.$(libext) \
                libtspstat.$(libext) \
                libtsphy.$(libext) \
                libtsledup.$(libext) \
                libtscmic.$(libext) \
                libtsvisibility.$(libext) \
                libtswarmboot.$(libext) \
                libtechsupport.$(libext)
ESW_DIAG_LIBS += libtsetrap.$(libext)
ESW_DIAG_LIBS += libtsint.$(libext)
ESW_DIAG_LIBS += libtshistogram.$(libext)
ESW_DIAG_LIBS += libtsalpm.$(libext)
ESW_DIAG_LIBS += libtscancun.$(libext)
endif

ifdef LTSW_CHIPS
LTSW_DIAG_LIBS = \
    libdiag_ltsw.$(libext) \
    libdiag_ltswissu.$(libext)
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
        ${LTSW_DIAG_LIBS} \
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
	$(COND_LIB_SAL_APPL_PLATFORM) \
	$(COND_LIB_PLI_BDE) \
	liblubde.$(libext)


ACL_LIB = $(COND_LIB_ACL)
BCM_LIBS = $(DIAG_LIBS) $(DRV_LIBS) $(ACL_LIB) $(COND_LIB_ETH_DRV)
BCM_LIBS_BLD = $(addprefix $(BLDDIR)/,$(BCM_LIBS))

BCM_LIBS_BLD += ${EXTERNAL_LIBS}

# Kernel network support
ifdef BUILD_KNET

knet_subdirs =

ifdef ESW_CHIPS
knet_subdirs += bcm-knet
endif

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
endif # YAML

endif # BUILD_SDKLT

ifeq (1,$(BUILD_SDKLT))

# SDKLT_BDE module
SDKLT_BDE = $(SDKLT)/linux/bde
# SDKLT_KNET module
SDKLT_KNET = $(SDKLT)/linux/knet
# SDKLT_LKM_BLDROOT
SDKLT_LKM_BLDROOT = $(KERN_BLDROOT)/sdklt

BDE_BLDDIR = $(SDKLT_LKM_BLDROOT)/lkm/bde
KNET_BLDDIR = $(SDKLT_LKM_BLDROOT)/lkm/knet

export LKM_CPPFLAGS = $(LTSW_LKM_ADD_CPPFLAGS)
export LKM_CFLAGS = $(LTSW_LKM_ADD_CFLAGS)
export LKM_LDFLAGS = $(LTSW_LKM_ADD_LDFLAGS)

# Support quiet make
ifneq (,$(findstring s,$(MAKEFLAGS)))
quiet = quiet_
KBUILD_VERBOSE = 1
endif

endif # BUILD_SDKLT


.PHONY: $(BSL_ENUM_HYBRID)

# Optionally check elfutils version
ELFUTILS_VERBOSE = 1
include $(SDK)/make/Make.elfutils

kernel_modules: elfutils $(BSL_ENUM_HYBRID)
ifdef IPROC_BUILD
ifneq ($(wildcard $(SDK)/tools/diffconfig.pl),)
	$(PERL) $(SDK)/tools/diffconfig.pl \
                $(SDK)/systems/linux/user/$(platform)/linux-$(platform).config $(KERNDIR)/.config
endif
endif
	$(MAKE) -C $(SDK)/systems/linux/kernel/modules kernel_version=$(kernel_version) OPT_CFLAGS="$(ADD_TO_KCFLAGS)" \
                subdirs="shared" override-target=linux-$(platform); \
	$(MAKE) -C $(SDK)/systems/bde/linux/kernel kernel_version=$(kernel_version) OPT_CFLAGS="$(ADD_TO_KCFLAGS)"; \
	$(MAKE) -C $(SDK)/systems/bde/linux/user/kernel kernel_version=$(kernel_version) OPT_CFLAGS="$(ADD_TO_KCFLAGS)"
ifdef BUILD_SDKLT
	$(MAKE) -C $(SDKLT_BDE) SDK=$(SDKLT) quiet=$(quiet) LKM_BLDDIR=$(BDE_BLDDIR) KBUILD_VERBOSE=$(KBUILD_VERBOSE)
	(cd $(KERN_BLDROOT); ln -s -f $(BDE_BLDDIR)/*.ko ./)
endif
ifdef BUILD_KNET
	$(MAKE) -C $(SDK)/systems/linux/kernel/modules kernel_version=$(kernel_version) OPT_CFLAGS="$(ADD_TO_KCFLAGS)" \
		subdirs="$(knet_subdirs)" override-target=linux-$(platform)
ifdef BUILD_SDKLT
	$(MAKE) -C $(SDKLT_KNET) SDK=$(SDKLT) quiet=$(quiet) LKM_BLDDIR=$(KNET_BLDDIR) KBUILD_VERBOSE=$(KBUILD_VERBOSE) \
		KBUILD_EXTRA_SYMBOLS=$(BDE_BLDDIR)/Module.symvers
	(cd $(KERN_BLDROOT); ln -s -f $(KNET_BLDDIR)/*.ko ./)
endif
endif

user_libs: $(BSL_ENUM_HYBRID)
ifdef CUSTOMER_SOURCE_DIR
	$(MAKE) -C $(CUSTOMER_SOURCE_DIR) CFLAGS="$(CFLAGS)"
endif
	$(MAKE) -C $(SDK)/systems/bde/linux/user CFLAGS="$(CFLAGS)"
ifdef BUILD_SDKLT
	$(MAKE) -C $(SDKLT) SDK=$(SDKLT) SDK_VERSION=$(SDKLT_VER)
	$(MAKE) -C $(SDKLT)/bcmissu/dll SDK=$(SDKLT) SDK_VERSION=$(SDKLT_VER) issu_db
endif
ifdef BUILD_PHYMOD
	$(MAKE) -C $(SDK)/libs/phymod LINUX_MAKE_SHARED_LIB=${LINUX_MAKE_SHARED_LIB} SHAREDLIBVER=${SHAREDLIBVER}
endif
ifdef BUILD_PCIEPHY
	$(MAKE) -C $(SDK)/libs/pciephy
endif
	$(MAKE) -C $(SDK)/src CFLAGS="$(ADD_TO_CFLAGS)"
	$(MAKE) -C $(SDK)/systems/drv CFLAGS="$(CFLAGS)"
ifdef BUILD_KNET
	$(MAKE) -C $(SDK)/systems/linux/kernel/user/shared CFLAGS="$(CFLAGS)"
endif
ifdef BUILD_XFLOW_MACSEC
	@${ECHO} "Building XFLOW_MACSEC libraries ...."
	$(MAKE) -C ${XFLOW_MACSEC_HOME} XFLOW_MACSEC_CC="${CC}" XFLOW_MACSEC_LD="${LD}" XFLOW_MACSEC_AR="$(AR)" XFLOW_MACSEC_CFLAGS="${XFLOW_MACSEC_BUILD_FLAG}" XFLOW_MACSEC_TARGET="${targetbase}/${targetplat}/${platform}" CFLAGS="$(CFLAGS)"
else
ifdef BUILD_MACSEC
	@${ECHO} "Building MACSEC libraries ...."
	$(MAKE) -C ${MACSEC_HOME} MACSEC_CC="${CC}" MACSEC_LD="${LD}" MACSEC_AR="$(AR)" MACSEC_CFLAGS="${MACSEC_BUILD_FLAG}" MACSEC_TARGET="${targetbase}/${targetplat}/${platform}"
endif
endif
ifdef BUILD_EXT_SERVO
	@${ECHO} "Building External servo library .... $(CPU_PLATFORM)"
	$(MAKE) -C ${EXT_SERVO_BUILD_HOME} TARGET=linux.$(CPU_PLATFORM) CPU_TARGET=$(CPU_PLATFORM) USER_ARFLAGS=rcvs CLOCK_TYPE=BC NUMBER_OF_STACK_INSTANCES=10 DCO_DEVICE=softwareDco TIMESTAMP_DEVICE=softwareTimestamping
endif

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

pli:
	$(MAKE) -C ${SDK}/systems/bde/pli OPT_CFLAGS="$(ADD_TO_CFLAGS)"

$(DEST_DIR):
	mkdir --parents --verbose $@

$(DEST_DIR)/libopennsa.a: $(BCM_LIBS_BLD) $(DEST_DIR)
	(echo create $@; for i in $(sort $(BCM_LIBS_BLD)); do echo addlib $$i; done; echo save) | ar -M

$(DEST_DIR)/libopennsa.so: $(BCM_LIBS_BLD) $(DEST_DIR)
	gcc -shared -Wl,--whole-archive $(sort $(BCM_LIBS_BLD)) -Wl,--no-whole-archive -o $@

$(BCM): $(BCM).dbg
	mkdir -p $(@D)
	$(OBJCOPY) --strip-debug $< $@

LINKER := $(CC)
ifeq ($(C_CPP_CODE_MIX), TRUE)
LINKER := $(CXX)
endif

$(BCM).dbg: user_libs $(PLI) $(BLDDIR)/demo_opennsa_init.o $(BLDDIR)/version.o \
	${PLATFORM_DEFINES_OBJ}
	@rm -f $@	# Prevent core dump if currently running
ifeq (1,$(LINK_STATIC))
	@echo Linking $@ # This is needed for error parsing
else
	@echo Linking $@ dynamically # This is needed for error parsing
endif
	$(LINKER) $(CFLAGS) -o $@ $(BLDDIR)/demo_opennsa_init.o $(BLDDIR)/version.o \
	-Wl,--start-group ${PLATFORM_DEFINES_OBJ} $(BCM_LIBS_BLD)  -Wl,--end-group $(LDFLAGS)

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

$(KERN_BLDROOT)/%.$(KOBJ): kernel_modules
	@echo Force execution of kernel_modules target > /dev/null

ifeq ($(NO_LOCAL_TARGETS),)
$(foreach targ,$(LOCAL_TARGETS),$(eval $(call LOCAL_TARGET_DEF,$(targ))))
endif

# Build Version
$(BLDDIR)/version.c: $(BLDDIR)/demo_opennsa_init.o user_libs
	$(PERL) $(SDK)/tools/version.pl > $@
$(BLDDIR)/version.o: $(BLDDIR)/version.c
	$(CC) $(CFLAGS) -c $< -o $@

clean::
	$(MAKE) -C $(SDK)/systems/bde/linux/kernel $@
	$(MAKE) -C $(SDK)/systems/bde/linux/user/kernel $@
	$(MAKE) -C $(SDK)/systems/bde/linux/user CFLAGS="$(CFLAGS)" $@
	$(MAKE) -C $(SDK)/systems/drv $@
	$(MAKE) -C $(SDK)/systems/drv/eth $@
	$(MAKE) -C $(SDK)/src CFLAGS="$(CFLAGS)" $@
ifdef BUILD_SDKLT
	$(MAKE) -C $(SDKLT) SDK=$(SDKLT) $@
endif
ifdef BUILD_PHYMOD
	$(MAKE) -C $(SDK)/libs/phymod LINUX_MAKE_SHARED_LIB=${LINUX_MAKE_SHARED_LIB} SHAREDLIBVER=${SHAREDLIBVER} $@
endif
ifdef BUILD_PCIEPHY
	$(MAKE) -C $(SDK)/libs/pciephy $@
endif
	$(MAKE) -C $(SDK)/systems/linux/kernel/user/shared $@
	$(MAKE) -C $(SDK)/systems/linux/kernel/modules \
		subdirs="$(knet_subdirs)" \
		override-target=linux-$(platform) $@
	$(RM) $(BLDDIR)/version.c $(BLDDIR)/version.o
	$(RM) $(BLDDIR)/platform_defines.c $(BLDDIR)/platform_defines.o
	$(RM) $(BLDDIR)/demo_opennsa_init.o
	$(RM) $(KERNEL_BDE) $(USER_BDE) $(BCM) $(BCM).dbg
	$(RM) $(BCM_KNET)
ifdef BUILD_SDKLT
	$(RM) $(KERNEL_NGBDE) $(KERNEL_NGKNET)
endif
	$(RM) $(KERN_BLDROOT)/linux-kernel-bde.$(KOBJ)
	$(RM) $(KERN_BLDROOT)/linux-user-bde.$(KOBJ)
	$(RM) $(KERN_BLDROOT)/linux-bcm-knet.$(KOBJ)
ifdef BUILD_SDKLT
	$(RM) $(KERN_BLDROOT)/$(KERNEL_NGBDE_LOCAL)
	$(RM) $(KERN_BLDROOT)/$(KERNEL_NGKNET_LOCAL)
	$(RM) $(KERN_BLDROOT)/sdklt
	$(RM) $(BLDDIR)/include
	$(RM) $(BLDDIR)/lib
	$(RM) $(BLDDIR)/sdklt
endif
ifdef BUILD_EXT_SERVO
	@${ECHO} "Clean External servo library ....$(CPU_PLATFORM)"
	$(MAKE) -C ${EXT_SERVO_BUILD_HOME} TARGET=linux.$(CPU_PLATFORM) clean
endif
ifdef BUILD_XFLOW_MACSEC
	$(MAKE) -C ${XFLOW_MACSEC_HOME} XFLOW_MACSEC_CC="${CC}" XFLOW_MACSEC_LD="${LD}" \
		XFLOW_MACSEC_TARGET="${targetbase}/$(targetplat)/${platform}" clean
endif

distclean::
	$(RM) $(BLDROOT)
	$(RM) $(KERN_BLDROOT)
	$(RM) $(DEST_DIR)

.PHONY: variable

#
# Echo variable values used for configuration
# usage: make VAR=CC variable
#
variable::
	@echo $($(VAR))

else
ifndef DELIVERY
$(error DELIVERY required)
endif

DELIVERABLES := $(KERNEL_BDE) $(USER_BDE) $(BCM) $(BCM).dbg

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
endif

# Make.config defines remaining phony targets
.PHONY: build kernel_modules user_libs pli DELIVER mod bcm user issu


