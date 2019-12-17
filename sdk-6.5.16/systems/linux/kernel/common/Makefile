# -*- Makefile -*-
# 
# This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
# 
# Copyright 2007-2019 Broadcom Inc. All rights reserved.

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

user-override=unix-user

ifeq (,$(platform))
$(error Makefile: error: Internal error: platform variable is not set)
else

override-target=linux-$(platform)
include $(SDK)/make/Make.config
endif

ifeq (,$(kernel_version))
kernel_version=2_4
endif

ifneq ($(kernel_version),2_4)
KOBJ=ko
else
KOBJ=o
endif

BCM_PROXY=$(DEST_DIR)/bcm.user.proxy

lmsc_platform=$(platform)

ifeq (2.6.24.4, $(KERN_VER))
lmsc_platform=$(platform).2.6.24
endif

ifeq (2.6.25, $(KERN_VER))
lmsc_platform=$(platform).2.6.25
endif

ifeq (2.6.27.18, $(KERN_VER))
lmsc_platform=$(platform).2.6.27
endif

ifeq (3.0,$(WRS_LINUX_VERSION))
lmsc_platform=$(platform)-wr30
endif

ifeq ($(DESTDIR),)
DESTDIR=${BLDDIR}
endif

# 1 = basename
define KERNEL_RULE
$(DESTDIR)/$(1) : kernel_modules
$(DEST_DIR)/$(1) : $(DESTDIR)/$(1) | $(DEST_DIR)
	$(OBJCOPY) --strip-debug $(DESTDIR)/$(1) $$@
endef


LOCAL_DESTDIR :=$(SDK)/systems/linux/kernel/$(platform)

BCM_PROXY_LOCAL :=$(LOCAL_DESTDIR)/bcm.user.proxy
KERNEL_BDE_LOCAL :=$(LOCAL_DESTDIR)/linux-kernel-bde.$(KOBJ)
KERNEL_CORE_LOCAL :=$(LOCAL_DESTDIR)/linux-bcm-core.$(KOBJ)
KERNEL_PROXY_LOCAL :=$(LOCAL_DESTDIR)/linux-uk-proxy.$(KOBJ)
KERNEL_DIAG_FULL_LOCAL :=$(LOCAL_DESTDIR)/linux-bcm-diag-full.$(KOBJ)
KERNEL_NET_LOCAL :=$(LOCAL_DESTDIR)/linux-bcm-net.$(KOBJ)
KERNEL_DIAG_LOCAL :=$(LOCAL_DESTDIR)/linux-bcm-diag.$(KOBJ)

KERNEL_BDE :=$(DEST_DIR)/linux-kernel-bde.$(KOBJ)
KERNEL_CORE:=$(DEST_DIR)/linux-bcm-core.$(KOBJ)
KERNEL_PROXY :=$(DEST_DIR)/linux-uk-proxy.$(KOBJ)
KERNEL_DIAG_FULL :=$(DEST_DIR)/linux-bcm-diag-full.$(KOBJ)
KERNEL_NET:=$(DEST_DIR)/linux-bcm-net.$(KOBJ)
KERNEL_DIAG:=$(DEST_DIR)/linux-bcm-diag.$(KOBJ)

_KERNEL_BDE:=$(DESTDIR)/linux-kernel-bde.$(KOBJ)
_KERNEL_CORE:=$(DESTDIR)/linux-bcm-core.$(KOBJ)
_KERNEL_PROXY:=$(DESTDIR)/linux-uk-proxy.$(KOBJ)
_KERNEL_DIAG_FULL:=$(DESTDIR)/linux-bcm-diag-full.$(KOBJ)
_KERNEL_NET:=$(DESTDIR)/linux-bcm-net.$(KOBJ)
_KERNEL_DIAG:=$(DESTDIR)/linux-bcm-diag.$(KOBJ)

_KERNEL_TARGETS:=$(_KERNEL_BDE) $(_KERNEL_CORE) $(_KERNEL_PROXY) \
               $(_KERNEL_DIAG_FULL) $(_KERNEL_NET) $(_KERNEL_DIAG)



# Modules must be built in load order to ensure correct module versioning
KERNEL_TARGETS:=$(KERNEL_BDE) $(KERNEL_CORE) $(KERNEL_PROXY) \
               $(KERNEL_DIAG_FULL) $(KERNEL_NET) $(KERNEL_DIAG)

ifeq ($(NO_LOCAL_TARGETS),)
LOCAL_TARGETS :=$(KERNEL_BDE_LOCAL) $(KERNEL_CORE_LOCAL) $(KERNEL_PROXY_LOCAL) \
	$(KERNEL_DIAG_FULL_LOCAL) $(KERNEL_NET_LOCAL) $(KERNEL_DIAG_LOCAL)
KERNEL_TARGETS += $(LOCAL_TARGETS)

# Rule to make local target
define LOCAL_TARGET_DEF
$(1) : $(DESTDIR)/$(notdir $(1))
	@for i in {1..10}; do echo "Symlinking $1..." ; ln -sf $(DEST_DIR)/$(notdir $(1))  $$@ && break || sleep ${BACKOFF} ; done
endef
endif

ifeq (,$(findstring DELIVER,$(MAKECMDGOALS)))
all: $(KERNEL_TARGETS) user_apps link_check $(if $(NO_LOCAL_TARGETS),,$(BCM_PROXY_LOCAL)) $(EXTRA_LIBRARY)

$(foreach t,$(notdir $(_KERNEL_TARGETS)),$(eval $(call KERNEL_RULE,$(t))))

ifeq ($(NO_LOCAL_TARGETS),)
$(foreach t,$(LOCAL_TARGETS),$(eval $(call LOCAL_TARGET_DEF,$(t))))
endif

ifeq (1,$(KS_HIGHMEM))
lmsc_platform:=$(lmsc_platform)-highmem
endif

$(DEST_DIR):
	mkdir -p $@

kernel_modules:
ifdef BUILD_PHYMOD
	$(MAKE) -C $(SDK)/libs/phymod
endif
ifdef BUILD_PCIEG3
	$(MAKE) -C $(SDK)/libs/pcieg3
endif
	$(MAKE) -C $(SDK)/src
	$(MAKE) -C $(SDK)/systems/drv
ifdef BUILD_XFLOW_MACSEC
	@$(ECHO) "Building XFLOW_MACSEC libraries ...."
	$(MAKE) -C ${XFLOW_MACSEC_HOME} XFLOW_MACSEC_CC="${CC}" XFLOW_MACSEC_LD="${LD}" XFLOW_MACSEC_AR="$(AR)" XFLOW_MACSEC_CFLAGS="${XFLOW_MACSEC_BUILD_FLAG}" XFLOW_MACSEC_TARGET="${targetbase}/$(targetplat)/${platform}" all
else
ifdef BUILD_MACSEC
	@$(ECHO) "Building MACSEC libraries ...."
	$(MAKE) -C ${MACSEC_HOME} MACSEC_CC="${CC}" MACSEC_LD="${LD}" MACSEC_AR="$(AR)" MACSEC_CFLAGS="${MACSEC_BUILD_FLAG}" MACSEC_TARGET="${targetbase}/$(targetplat)/${platform}" all
endif
endif
	$(MAKE) -C $(SDK)/systems/bde/linux/kernel
	$(MAKE) -C $(SDK)/systems/linux/kernel/modules kernel_version=$(kernel_version)

$(BCM_PROXY_LOCAL) : $(BCM_PROXY)
	rm -f $@
	ln -sf $? $@


$(BCM_PROXY) : user_apps

user_apps:
	$(MAKE) -C $(SDK)/src/sal/appl
	$(MAKE) -C $(SDK)/systems/linux/kernel/user/bcm-diag-proxy \
		 BCM_PROXY=$(BCM_PROXY) \
		 override-target=$(user-override) \
		 bldroot_suffix=/$(override-target)

#
# Define the set of allowable link dependencies. 
# All configurations in the LINK_CHECKS variable
# must be clean. 
#
LINK_CHECKS = $(KERNEL_BDE)
LINK_CHECKS += $(KERNEL_PROXY)
LINK_CHECKS += $(KERNEL_BDE),$(KERNEL_CORE)
LINK_CHECKS += $(KERNEL_BDE),$(KERNEL_CORE),$(KERNEL_NET)
LINK_CHECKS += $(KERNEL_BDE),$(KERNEL_CORE),$(KERNEL_PROXY),$(KERNEL_DIAG)
LINK_CHECKS += $(KERNEL_BDE),$(KERNEL_PROXY),$(KERNEL_DIAG_FULL)

link_check: kernel_modules
	$(PERL) $(SDK)/tools/lmsc.pl ferror=0 kernel=$(lmsc_platform) $(LINK_CHECKS)

clean:
	$(MAKE) -C $(SDK)/src $@
	$(MAKE) -C $(SDK)/systems/drv $@
	$(MAKE) -C $(SDK)/systems/drv/eth $@
ifdef BUILD_PHYMOD
	$(MAKE) -C $(SDK)/libs/phymod $@
endif
ifdef BUILD_PCIEG3
	$(MAKE) -C $(SDK)/libs/pcieg3 $@
endif
	$(MAKE) -C $(SDK)/systems/linux/kernel/modules $@
	$(MAKE) -C $(SDK)/systems/bde/linux/kernel $@
	$(MAKE) -C $(SDK)/systems/linux/kernel/user/bcm-diag-proxy \
		 BCM_PROXY=$(BCM_PROXY) \
		 override-target=$(user-override) \
		 bldroot_suffix=/$(override-target) $@
	$(RM) $(_KERNEL_TARGETS) $(KERNEL_TARGETS) $(LOCAL_TARGETS)
ifdef BUILD_XFLOW_MACSEC
	$(MAKE) -C ${XFLOW_MACSEC_HOME} XFLOW_MACSEC_TARGET="${targetbase}/$(targetplat)/${platform}" clean
else
ifdef BUILD_MACSEC
	$(MAKE) -C ${MACSEC_HOME} MACSEC_TARGET="${targetbase}/$(targetplat)/${platform}" clean
endif
endif

distclean:
	$(RM) $(BLDROOT)
	$(RM) $(DEST_DIR)

.PHONY: variable user_apps

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

DELIVERABLES := $(KERNEL_TARGETS) $(BCM_PROXY)

# 1= source
define DELIVERY_RULE
_DEL_TARG += $(DELIVERY)/$(notdir $(1))
$(DELIVERY)/$(notdir $(1)) : $(1)
	@mkdir -p $(DELIVERY)
	$(OBJCOPY) --strip-debug $$< $$@
endef

$(foreach f,$(DELIVERABLES),$(eval $(call DELIVERY_RULE,$(f))))

DELIVER: $(_DEL_TARG)
endif

# Make.config defines remaining phony targets
.PHONY: check kernel_modules user_apps kernel_targets DELIVER


