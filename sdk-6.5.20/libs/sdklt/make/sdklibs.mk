#
# This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
# 
# Copyright 2007-2020 Broadcom Inc. All rights reserved.
#
# SDK build targets.
#
# This file should be included in the main application makefile to
# allow building the sdklibs target.
#
# By default all SDK libraries are built, but this can be overridden
# using the SDK_TARGETS variable, for example to only build the SAL
# and BSL libraries:
#
# all: sdklibs
# SDK_TARGETS = sal bsl
# include $(SDK)/make/sdklibs.mk
#

# Basic make tools
include $(SDK)/make/maketools.mk

# Targets within this file should never become automatic targets
sdklibs_help:
	@$(ECHO) "Your makefile targets must precede this file"

.PHONY: sdklibs_help

# Default location for all SDK libraries
include $(SDK)/make/libpath.mk

# Get SDK_TARGETS
include $(SDK)/make/targets.mk

include $(SDK)/make/clean.mk

# This is our main target
sdklibs: sdklibs_opts sdklibs_prep
	$(MAKE) sdklibs_bld

-include $(SDK)/make/chipdef.mk

sdklibs_prep: $(SDK_CHIPDEF)

# Generate file with current build options
sdklibs_opts:
	$(MAKE) -f $(SDK)/make/options.mk

# This target build the component libraries
sdklibs_bld: $(SDK_TARGETS)

$(CLEAN_TARGETS):: sdklibs

.PHONY: sdklibs sdklibs_prep sdklibs_bld $(SDK_TARGETS)

#
# IMPORTANT:
# The following sections must be kept in (link) dependency order.
#

# Configure BCMA library
ifneq (,$(filter bcma,$(SDK_TARGETS)))
# BCMA_LIBS is defined in $(BCMA)/targets.mk
include $(BCMA)/targets.mk
SDK_LIBS += $(BCMA_LIBS)
SDK_INCLUDES += -I$(BCMA)/include
bcma:
	$(MAKE) -C $(BCMA) $(SDK_CLEAN)
endif

# Configure BCMMGMT library
ifneq (,$(filter bcmmgmt,$(SDK_TARGETS)))
# BCMMGMT_LIBS is defined in $(BCMMGMT)/targets.mk
include $(BCMMGMT)/targets.mk
SDK_LIBS += $(BCMMGMT_LIBS)
SDK_INCLUDES += -I$(BCMMGMT)/include
bcmmgmt:
	$(MAKE) -C $(BCMMGMT) $(SDK_CLEAN)
endif

# Configure BCMLU library
ifneq (,$(filter bcmlu,$(SDK_TARGETS)))
# BCMLU_LIBS is defined in $(BCMLU)/targets.mk
include $(BCMLU)/targets.mk
SDK_LIBS += $(BCMLU_LIBS)
SDK_INCLUDES += -I$(BCMLU)/include
bcmlu:
	$(MAKE) -C $(BCMLU) $(SDK_CLEAN)
endif

# Configure BCMLT library
ifneq (,$(filter bcmlt,$(SDK_TARGETS)))
# BCMLT_LIBS is defined in $(BCMLT)/targets.mk
include $(BCMLT)/targets.mk

SDK_LIBS += $(BCMLT_LIBS)
SDK_INCLUDES += -I$(BCMLT)/include
bcmlt:
	$(MAKE) -C $(BCMLT) $(SDK_CLEAN)
endif

# Configure BCMLTP library
ifneq (,$(filter bcmltp,$(SDK_TARGETS)))
# BCMLTP_LIBS is defined in $(BCMLTP)/targets.mk
include $(BCMLTP)/targets.mk
SDK_LIBS += $(BCMLTP_LIBS)
SDK_INCLUDES += -I$(BCMLTP)/include
bcmltp:
	$(MAKE) -C $(BCMLTP) $(SDK_CLEAN)
endif

# Configure BCMTRM library
ifneq (,$(filter bcmtrm,$(SDK_TARGETS)))
# BCMTRM_LIBS is defined in $(BCMTRM)/targets.mk
include $(BCMTRM)/targets.mk
SDK_LIBS += $(BCMTRM_LIBS)
SDK_INCLUDES += -I$(BCMTRM)/include
bcmtrm:
	$(MAKE) -C $(BCMTRM) $(SDK_CLEAN)
endif

# Configure BCMDI library
ifneq (,$(filter bcmdi,$(SDK_TARGETS)))
# BCMDI_LIBS is defined in $(BCMDI)/targets.mk
include $(BCMDI)/targets.mk
SDK_LIBS += $(BCMDI_LIBS)
SDK_INCLUDES += -I$(BCMDI)/include
bcmdi:
	$(MAKE) -C $(BCMDI) $(SDK_CLEAN)
endif


# Configure BCMLTM library
ifneq (,$(filter bcmltm,$(SDK_TARGETS)))
# BCMLTM_LIBS is defined in $(BCMLTM)/targets.mk
include $(BCMLTM)/targets.mk
SDK_LIBS += $(BCMLTM_LIBS)
SDK_INCLUDES += -I$(BCMLTM)/include
bcmltm:
	$(MAKE) -C $(BCMLTM) $(SDK_CLEAN)
endif

# Configure BCMLTA library
ifneq (,$(filter bcmlta,$(SDK_TARGETS)))
# BCMLTA_LIBS is defined in $(BCMLTA)/targets.mk
include $(BCMLTA)/targets.mk
SDK_LIBS += $(BCMLTA_LIBS)
SDK_INCLUDES += -I$(BCMLTA)/include
bcmlta:
	$(MAKE) -C $(BCMLTA) $(SDK_CLEAN)
endif

# Configure BCMLTX library
ifneq (,$(filter bcmltx,$(SDK_TARGETS)))
# BCMLTX_LIBS is defined in $(BCMLTX)/targets.mk
include $(BCMLTX)/targets.mk
SDK_LIBS += $(BCMLTX_LIBS)
SDK_INCLUDES += -I$(BCMLTX)/include
bcmltx:
	$(MAKE) -C $(BCMLTX) $(SDK_CLEAN)
endif

# Configure BCMFP library
ifneq (,$(filter bcmfp,$(SDK_TARGETS)))
# BCMFP_LIBS is defined in $(BCMFP)/targets.mk
include $(BCMFP)/targets.mk
SDK_LIBS += $(BCMFP_LIBS)
SDK_INCLUDES += -I$(BCMFP)/include
bcmfp:
	$(MAKE) -C $(BCMFP) $(SDK_CLEAN)
endif

# Configure BCMSEC library
ifneq (,$(filter bcmsec,$(SDK_TARGETS)))
# BCMSEC_LIBS is defined in $(BCMSEC)/targets.mk
include $(BCMSEC)/targets.mk
SDK_LIBS += $(BCMSEC_LIBS)
SDK_INCLUDES += -I$(BCMSEC)/include
bcmsec:
	$(MAKE) -C $(BCMSEC) $(SDK_CLEAN)
endif

# Configure BCMTM library
ifneq (,$(filter bcmtm,$(SDK_TARGETS)))
# BCMTM_LIBS is defined in $(BCMTM)/targets.mk
include $(BCMTM)/targets.mk
SDK_LIBS += $(BCMTM_LIBS)
SDK_INCLUDES += -I$(BCMTM)/include
bcmtm:
	$(MAKE) -C $(BCMTM) $(SDK_CLEAN)
endif

# Configure BCMCTH library
ifneq (,$(filter bcmcth,$(SDK_TARGETS)))
# BCMCTH_LIBS is defined in $(BCMCTH)/targets.mk
include $(BCMCTH)/targets.mk
SDK_LIBS += $(BCMCTH_LIBS)
SDK_INCLUDES += -I$(BCMCTH)/include
bcmcth:
	$(MAKE) -C $(BCMCTH) $(SDK_CLEAN)
endif

# Configure BCMECMP library
ifneq (,$(filter bcmecmp,$(SDK_TARGETS)))
# BCMECMP_LIBS is defined in $(BCMECMP)/targets.mk
include $(BCMECMP)/targets.mk
SDK_LIBS += $(BCMECMP_LIBS)
SDK_INCLUDES += -I$(BCMECMP)/include
bcmecmp:
	$(MAKE) -C $(BCMECMP) $(SDK_CLEAN)
endif

# Configure BCMPKT library
ifneq (,$(filter bcmpkt,$(SDK_TARGETS)))
# BCMPKT_LIBS is defined in $(BCMPKT)/targets.mk
include $(BCMPKT)/targets.mk
SDK_LIBS += $(BCMPKT_LIBS)
SDK_INCLUDES += -I$(BCMPKT)/include
bcmpkt:
	$(MAKE) -C $(BCMPKT) $(SDK_CLEAN)
endif

# Configure BCMCNET library
ifneq (,$(filter bcmcnet,$(SDK_TARGETS)))
# BCMCNET_LIBS is defined in $(BCMCNET)/targets.mk
include $(BCMCNET)/targets.mk
SDK_LIBS += $(BCMCNET_LIBS)
SDK_INCLUDES += -I$(BCMCNET)/include
bcmcnet:
	$(MAKE) -C $(BCMCNET) $(SDK_CLEAN)
endif

# Configure BCMPTM library
ifneq (,$(filter bcmptm,$(SDK_TARGETS)))
# BCMPTM_LIBS is defined in $(BCMPTM)/targets.mk
include $(BCMPTM)/targets.mk
SDK_LIBS += $(BCMPTM_LIBS)
SDK_INCLUDES += -I$(BCMPTM)/include
bcmptm:
	$(MAKE) -C $(BCMPTM) $(SDK_CLEAN)
endif

# Configure BCMLM library
ifneq (,$(filter bcmlm,$(SDK_TARGETS)))
# BCMLM_LIBS is defined in $(BCMLM)/targets.mk
include $(BCMLM)/targets.mk
SDK_LIBS += $(BCMLM_LIBS)
SDK_INCLUDES += -I$(BCMLM)/include
bcmlm:
	$(MAKE) -C $(BCMLM) $(SDK_CLEAN)
endif

# Configure BCMPC library
ifneq (,$(filter bcmpc,$(SDK_TARGETS)))
# BCMPC_LIBS is defined in $(BCMPC)/targets.mk
include $(BCMPC)/targets.mk
SDK_LIBS += $(BCMPC_LIBS)
SDK_INCLUDES += -I$(BCMPC)/include
bcmpc:
	$(MAKE) -C $(BCMPC) $(SDK_CLEAN)
endif

# Configure BCMBD library
ifneq (,$(filter bcmbd,$(SDK_TARGETS)))
# BCMBD_LIBS is defined in $(BCMBD)/targets.mk
include $(BCMBD)/targets.mk
SDK_LIBS += $(BCMBD_LIBS)
SDK_INCLUDES += -I$(BCMBD)/include
bcmbd:
	$(MAKE) -C $(BCMBD) $(SDK_CLEAN)
endif

# Configure BCMIMM library
ifneq (,$(filter bcmimm,$(SDK_TARGETS)))
include $(BCMIMM)/targets.mk
SDK_LIBS += $(BCMIMM_LIBS)
SDK_INCLUDES += -I$(BCMIMM)/include
bcmimm:
	$(MAKE) -C $(BCMIMM) $(SDK_CLEAN)
endif

# Configure BCMCFG library
ifneq (,$(filter bcmcfg,$(SDK_TARGETS)))
# BCMCFG_LIBS is defined in $(BCMCFG)/targets.mk
include $(BCMCFG)/targets.mk
SDK_LIBS += $(BCMCFG_LIBS)
SDK_INCLUDES += -I$(BCMCFG)/include
bcmcfg:
	$(MAKE) -C $(BCMCFG) $(SDK_CLEAN)
endif

# Configure BCMISSU library
ifneq (,$(filter bcmissu,$(SDK_TARGETS)))
# BCMLT_LIBS is defined in $(BCMISSU)/targets.mk
include $(BCMISSU)/targets.mk

SDK_LIBS += $(BCMISSU_LIBS)
SDK_INCLUDES += -I$(BCMISSU)/include
bcmissu:
	$(MAKE) -C $(BCMISSU) $(SDK_CLEAN)
endif

# Configure BCMLRD library
ifneq (,$(filter bcmlrd,$(SDK_TARGETS)))
# BCMLRD_LIBS is defined in $(BCMLRD)/targets.mk
include $(BCMLRD)/targets.mk
SDK_LIBS += $(BCMLRD_LIBS)
SDK_INCLUDES += -I$(BCMLRD)/include
bcmlrd:
	$(MAKE) -C $(BCMLRD) $(SDK_CLEAN)
endif

# Configure BCMPMAC library
ifneq (,$(filter bcmpmac,$(SDK_TARGETS)))
# BCMPMAC_LIBS is defined in $(BCMPMAC)/targets.mk
include $(BCMPMAC)/targets.mk
SDK_LIBS += $(BCMPMAC_LIBS)
SDK_INCLUDES += -I$(BCMPMAC)/include
bcmpmac:
	$(MAKE) -C $(BCMPMAC) $(SDK_CLEAN)
endif

# Configure PHYMOD library
ifneq (,$(filter phymod,$(SDK_TARGETS)))
include $(SDK)/make/phymod.mk
PHYMOD_CPPFLAGS += $(SDK_CPPFLAGS) -I$(BSL)/include -I$(SAL)/include -I$(SHR)/include
export PHYMOD_CPPFLAGS
export PHYMOD_CFLAGS = $(SDK_CFLAGS)
export PHYMOD_BLDDIR = $(SDK_BLDDIR)
include $(SDK)/make/suffix.mk
export PHYMOD_LIBSUFFIX = $(SDK_LIBSUFFIX)
SDK_LIBS += $(addsuffix .$(PHYMOD_LIBSUFFIX),libphymod)
SDK_INCLUDES += -I$(PHYMOD)/include
phymod:
ifeq (,$(filter $(SDK_CLEAN),$(filter-out clean,$(CLEAN_TARGETS))))
	$(MAKE) -C $(PHYMOD) PHYMOD_CHIPS=$(PHYMOD_CHIPS) $(SDK_CLEAN)
endif
endif

# Configure PCIEPHY library
ifneq (,$(filter pciephy,$(SDK_TARGETS)))
PCIEPHY_CPPFLAGS += $(SDK_CPPFLAGS) -I$(BSL)/include -I$(SAL)/include -I$(SHR)/include
export PCIEPHY_CPPFLAGS
export PCIEPHY_CFLAGS = $(SDK_CFLAGS)
export PCIEPHY_BLDDIR = $(SDK_BLDDIR)
include $(SDK)/make/suffix.mk
export PCIEPHY_LIBSUFFIX = $(SDK_LIBSUFFIX)
SDK_LIBS += $(addsuffix .$(PCIEPHY_LIBSUFFIX),libpciephy)
SDK_INCLUDES += -I$(PCIEPHY)/include
pciephy:
ifeq (,$(filter $(SDK_CLEAN),$(filter-out clean,$(CLEAN_TARGETS))))
	$(MAKE) -C $(PCIEPHY) PCIEPHY_CHIPS=$(PCIEPHY_CHIPS) $(SDK_CLEAN)
endif
endif

# Configure BCMEVM library
ifneq (,$(filter bcmevm,$(SDK_TARGETS)))
include $(BCMEVM)/targets.mk
SDK_LIBS += $(BCMEVM_LIBS)
SDK_INCLUDES += -I$(BCMEVM)/include
bcmevm:
	$(MAKE) -C $(BCMEVM) $(SDK_CLEAN)
endif

# Configure BCMLTD library
ifneq (,$(filter bcmltd,$(SDK_TARGETS)))
# BCMLTD_LIBS is defined in $(BCMLTD)/targets.mk
include $(BCMLTD)/targets.mk
SDK_LIBS += $(BCMLTD_LIBS)
SDK_INCLUDES += -I$(BCMLTD)/include
bcmltd:
	$(MAKE) -C $(BCMLTD) $(SDK_CLEAN)
endif

# Configure BCMHA library
ifneq (,$(filter bcmha,$(SDK_TARGETS)))
# BCMHA_LIBS is defined in $(BCMHA)/targets.mk
include $(BCMHA)/targets.mk
SDK_LIBS += $(BCMHA_LIBS)
SDK_INCLUDES += -I$(BCMHA)/include
bcmha:
	$(MAKE) -C $(BCMHA) $(SDK_CLEAN)
endif

# Configure BCMDRD library
ifneq (,$(filter bcmdrd,$(SDK_TARGETS)))
# BCMDRD_LIBS is defined in $(BCMDRD)/targets.mk
include $(BCMDRD)/targets.mk
SDK_LIBS += $(BCMDRD_LIBS)
SDK_INCLUDES += -I$(BCMDRD)/include
bcmdrd:
	$(MAKE) -C $(BCMDRD) $(SDK_CLEAN)
endif

# Configure SIM library
ifneq (,$(filter sim,$(SDK_TARGETS)))
# SIM_LIBS is defined in $(SIM)/targets.mk
include $(SIM)/targets.mk
SDK_LIBS += $(SIM_LIBS)
SDK_INCLUDES += -I$(SIM)/include
sim:
	$(MAKE) -C $(SIM) $(SDK_CLEAN)
endif

# Configure SHR library
ifneq (,$(filter shr,$(SDK_TARGETS)))
# SHR_LIBS is defined in $(SHR)/targets.mk
include $(SHR)/targets.mk
SDK_LIBS += $(SHR_LIBS)
SDK_INCLUDES += -I$(SHR)/include
shr:
	$(MAKE) -C $(SHR) $(SDK_CLEAN)
endif

# Configure SAL library
ifneq (,$(filter sal,$(SDK_TARGETS)))
# SAL_LIBS is defined in $(SAL)/targets.mk
include $(SAL)/targets.mk
SDK_LIBS += $(SAL_LIBS)
SDK_INCLUDES += -I$(SAL)/include
sal:
	$(MAKE) -C $(SAL) $(SDK_CLEAN)
endif

# Configure BSL library
ifneq (,$(filter bsl,$(SDK_TARGETS)))
# BSL_LIBS is defined in $(BSL)/targets.mk
include $(BSL)/targets.mk
SDK_LIBS += $(BSL_LIBS)
SDK_INCLUDES += -I$(BSL)/include
bsl:
	$(MAKE) -C $(BSL) $(SDK_CLEAN)
endif

# Targets with full build path
ifdef SDK_BLDDIR
SDK_BLIBS = $(addprefix $(SDK_BLDDIR)/,$(SDK_LIBS))
endif

# Add target to ensure that both lists are kept in the same order
LLST = $(subst linux,,$(subst /,,$(basename $(dir $(subst -I$(SDK),,$(SDK_INCLUDES))))))
TLST = $(SDK_DEFAULT_TARGETS)

sdklibs_check:
ifneq ("$(TLST)","$(LLST)")
	@$(ECHO) "Error: SDK component lists do not match" 1>&2
	@$(ECHO) "Target list: $(TLST)" 1>&2
	@$(ECHO) "Link order:  $(LLST)" 1>&2
	@exit -1;
endif

.PHONY: sdklibs_check
