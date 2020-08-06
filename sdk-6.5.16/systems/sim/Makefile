# 
# This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
# 
# Copyright 2007-2020 Broadcom Inc. All rights reserved.
#

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

LOCALDIR=systems/sim

uname := $(shell uname -s)
ifeq ($(uname),Linux)
 ifeq ($(shell uname -m),x86_64)
  override-target=unix-linux-64
 else
  override-target=unix-linux
 endif
endif
ifeq ($(uname),SunOS)
 override-target=unix-solaris
endif
ifeq ($(uname),NetBSD)
 override-target=unix-netbsd
endif

ifdef NO_SAL_APPL
export NO_SAL_APPL
ifndef BLDCONFIG
BLDCONFIG=nsa
endif
export BLDCONFIG
endif

MFLAGS = 

export PCID_BUILD=1
export IPROC_NO_ATL = 1
CFGFLAGS += -DIPROC_NO_ATL

# Turn on direct schan to accelerate simulation in
# BCMSIM and AV environments.
CFGFLAGS += -DPLISIM_DIRECT_SCHAN

export CFGFLAGS

include ${SDK}/make/Make.config

#
# To use Purify, add a line here similar to the following:
#
#CC=/net/igor/export/home/pure/purify-4.2-solaris2/purify gcc

#
# Define all targets to build
# pcid is built first so you can start it before bcm links :-)
#
ifeq ($(DEST_DIR),)
DEST_DIR=${BLDDIR}
endif

PCID_LOCAL :=pcid.sim${all_suffix}
PCID = ${DEST_DIR}/pcid.${targetplat}${all_suffix}
L2SWITCH = ${DEST_DIR}/l2switch.${targetplat}${all_suffix}

# This defines where main is found
BCM_LOCAL = bcm.sim${all_suffix}
ifdef NO_SAL_APPL
MAIN_LIB=${BLDDIR}/socdiag-nsa.o
NSA=.nsa
BCM = ${DEST_DIR}/bcm.${targetplat}${all_suffix}${NSA}
all_targets := ${BCM}
ifeq ($(NO_LOCAL_TARGETS),)
all_targets += $(BCM_LOCAL)
endif
else
MAIN_LIB=${BLDDIR}/socdiag.o
BCM = ${DEST_DIR}/bcm.${targetplat}${all_suffix}${NSA}
all_targets := ${BCM} ${PCID}
ifeq ($(NO_LOCAL_TARGETS),)
all_targets += $(BCM_LOCAL) $(PCID_LOCAL)
endif
endif



# MAIN_LIB=${LIBDIR}/libdisctest.a

ifdef DPP_CHIPS
  subdirs = dpp/ChipSim
  include ${SDK}/make/Make.subdirs
endif

#
# Define targets to be built by default.
#
build_targets = ${all_targets}

#
# Libraries
#

BCM_LIBS = ${APPL_LIBS} ${DIAG_LIBS} ${DRV_LIBS}
BCM_LIBS_BLD = $(addprefix ${LIBDIR}/,${BCM_LIBS})

ifdef BUILD_XFLOW_MACSEC
BCM_LIBS_BLD += ${BCM_LIB_XFLOW_MACSEC}
else
ifdef BUILD_MACSEC
BCM_LIBS_BLD += ${BCM_LIB_MACSEC}
endif
endif

ifdef DPP_CHIPS
BCM_LIB_CHIPSIM = ${LIBDIR}/libchipsim_sim.${libext}
BCM_LIBS_BLD += ${BCM_LIB_CHIPSIM}
endif

PCID_LIBS = ${DRV_LIBS}
PCID_LIBS_BLD = $(addprefix ${LIBDIR}/,${PCID_LIBS})

L2SWITCH_LIBS = ${DRV_LIBS}
L2SWITCH_LIBS_BLD = $(addprefix ${LIBDIR}/,${L2SWITCH_LIBS})

ifeq (,$(findstring DELIVER,$(MAKECMDGOALS)))
all:: ${BLDDIR}/.tree ${build_targets} 

#
# Simplified targets to build a target for current platform
#
.PHONY:	bcm pcid l2switch

bcm.${override-target} bcm.${targetplat}${all_suffix}: ${BCM}

pcid.${override-target} pcid.${targetplat}${all_suffix}: ${PCID}

l2switch l2switch.${targetplat}${all_suffix}: ${L2SWITCH}


$(BCM_LOCAL) : $(BCM)
	cp $? $@


$(PCID_LOCAL) : $(PCID)
	cp $? $@

# Libraries

.PHONY: _bcm_libraries

_bcm_libraries:
ifndef NO_BCM
ifdef BUILD_PHYMOD
	$(MAKE) -C ${SDK}/libs/phymod
endif
	$(MAKE) -C ${SDK}
endif

ifdef BUILD_XFLOW_MACSEC
	$Q$(ECHO) "Building XFLOW_MACSEC libraries ...."
	$Q$(ECHO) "${targetbase}, $(targetplat), ${platform}"
	$(MAKE) -C ${XFLOW_MACSEC_HOME} XFLOW_MACSEC_CC="${CC}" XFLOW_MACSEC_LD="${LD}" XFLOW_MACSEC_CFLAGS="${XFLOW_MACSEC_BUILD_FLAG}" XFLOW_MACSEC_TARGET="${targetbase}/$(targetplat)/${platform}"
else
ifdef BUILD_MACSEC
	$Q$(ECHO) "Building MACSEC libraries ...."
	$(MAKE) -C ${MACSEC_HOME} MACSEC_CC="${CC}" MACSEC_LD="${LD}" MACSEC_CFLAGS="${MACSEC_BUILD_FLAG}" MACSEC_TARGET="${targetbase}/$(targetplat)/${platform}"
endif
endif



# Broadcom Command Monitor

# Build platform_defines.c to compile in config.bcm
#
ifndef NO_SAL_APPL
PLATFORM_DEFINES_OBJ=${BLDDIR}/platform_defines.o

${BLDDIR}/platform_defines.c: $(BLDDIR)/.tree ${SDK}/rc/config.bcm
	+$(PERL) ${SDK}/tools/bcm2c_linux.pl ${SDK}/rc/config.bcm >$@

${BLDDIR}/platform_defines.o: ${BLDDIR}/platform_defines.c
	+${CC} ${CFLAGS} -c $< -o $@
ifndef NO_VERSION
	touch ${SDK}/rc/config.bcm
endif
else
PLATFORM_DEFINES_OBJ=
endif

${BCM}: ${MAIN_LIB} _bde _bcm_libraries ${BLDDIR}/version.o \
        ${PLATFORM_DEFINES_OBJ}
	mkdir -p $(@D)
	@rm -f $@	# Prevent core dump if currently running
	${CC} -o $@ ${MAIN_LIB} ${BLDDIR}/version.o \
        ${PLATFORM_DEFINES_OBJ} ${LDFLAGS} \
        -Wl,--start-group ${BCM_LIBS_BLD} -Wl,--end-group

# PCI Daemon (chip simulator)

pcid.${override-target}.sim: ${PCID}
	${RM} -f pcid.sim
	${LN} -s ${PCID} pcid.sim

${PCID}: _bcm_libraries _bde
	mkdir -p $(@D)
	$(MAKE) -C ${SDK}/systems/sim/pcid \
		PCID=${PCID}

# Example L2 Switch Application

${L2SWITCH}: _bcm_libraries _bde
	mkdir -p $(@D)
	$(MAKE) -C ${SDK}/systems/sim/l2switch \
		INSTALL_DIR=$(DEST_DIR) ${L2SWITCH}





#
# Build version generator.  If NO_VERSION=1 is specified in Make.local
# or on command line, then use the existing version.c.  If that doesn't
# exist, copy the default one from config.
#
# Otherwise, force the generation of a new version.c using the perl script.
#
ifdef NO_VERSION
${BLDDIR}/version.c: ${BLDDIR}/.tree
	$(CP) ${SDK}/make/version.c $@
else
.PHONY: _force
${BLDDIR}/version.c: ${MAIN_LIB} _bde _bcm_libraries
	$(MKVERS) > $@
endif
${BLDDIR}/version.o: ${BLDDIR}/version.c
	${CC} ${CFLAGS} -c $< -o $@

# Make.depend is before clean:: so that Make.depend's clean:: runs first.

include ${SDK}/make/Make.depend

else
ifndef DELIVERY
$(error DELIVERY required)
endif

DELIVERABLES := ${all_targets}

# 1= source
define DELIVERY_RULE
_DEL_TARG += $(DELIVERY)/$(firstword $(subst -, ,$(notdir $(1))))
$(DELIVERY)/$(firstword $(subst -, ,$(notdir $(1)))) : $(1)
	@mkdir -p $(DELIVERY)
	\cp $$< $$@
endef

$(foreach f,$(DELIVERABLES),$(eval $(call DELIVERY_RULE,$(f))))

DELIVER: $(_DEL_TARG)
endif

# Clean
clean:: ${BLDDIR}/.tree
	$(RM) $(BLDDIR)/version.c $(BLDDIR)/version.o
	$(RM) $(BLDDIR)/platform_defines.c $(BLDDIR)/platform_defines.o
	$(RM) ${all_targets}
	$(RM) ${BOBJS}
	# Cleaning in PCID cleans BCM libs, BDE, etc.
ifdef BUILD_PHYMOD
	$(MAKE) -C ${SDK}/libs/phymod clean
endif
	$(MAKE) -C ${SDK}/systems/sim/pcid \
		INSTALL_DIR=$(BLDDIR) clean
	$(MAKE) -C ${SDK}/systems/sim/l2switch \
		INSTALL_DIR=$(BLDDIR) clean
ifdef BUILD_XFLOW_MACSEC
	$(MAKE) -C ${XFLOW_MACSEC_HOME} XFLOW_MACSEC_CC="${CC}" XFLOW_MACSEC_LD="${LD}" \
		XFLOW_MACSEC_TARGET="${targetbase}/$(targetplat)/${platform}" clean
endif

distclean::
	$(RM) $(BLDROOT)
	$(RM) $(DEST_DIR)

.PHONY: _bde
_bde:
	$(MAKE) -C ${SDK}/systems/bde/pli

# two libraries this binary needs
# point to them to allow parallel make processes
$(SDK)/build/%/libdiscover.a:
	$(MAKE) -C $(SDK)/src/appl/discover

$(SDK)/build/%/libstktask.a:
	$(MAKE) -C $(SDK)/src/appl/stktask



