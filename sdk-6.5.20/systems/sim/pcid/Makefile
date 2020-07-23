# $Id: Makefile,v 1.27 2012/04/18 08:06:11 srajesh Exp $
# This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
# 
# Copyright 2007-2020 Broadcom Inc. All rights reserved.
#

LOCALDIR = systems/sim/pcid

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

include ${SDK}/make/Make.config

ifeq ($(INSTALL_DIR),)
INSTALL_DIR=.
endif

ifndef SIM_STANDALONE_ONLY
#for SIM_STANDALONE_ONLY only libpcid.a should be compiled
ifdef DPP_CHIPS
LOCALDIR=systems/sim/pcid
subdirs := ../dpp/ChipSim
include ${SDK}/make/Make.subdirs
endif

ifdef DFE_CHIPS
LOCALDIR=systems/sim/pcid
subdirs := ../dpp/ChipSim
include ${SDK}/make/Make.subdirs
endif
endif
#
# To use Purify, add a line here similar to the following:
#
#CC=/net/igor/export/home/pure/purify-4.2-solaris2/purify gcc

#
# Define all targets to build
#

PCID = ${INSTALL_DIR}/pcid.${targetplat}${all_suffix}
PCID_LIB = ${LIBDIR}/libpcid.a

all_targets = ${PCID} ${PCID_LIB}

#
# Define targets to be built by default.
#
build_targets = ${all_targets}

#
# Libraries
#
#DIAG_LIBS = \
#	${COND_LIB_EDITLINE} \
#	libsal_appl.a libsal_appl_plat.a libplibde.a





PCID_LIBS = ${EXTRA_TEST_LIBS} ${DIAG_LIBS} ${APPL_LIBS} ${DRV_LIBS}
PCID_LIBS_BLD = $(addprefix ${BLDROOT}/,${PCID_LIBS})

ifdef DPP_CHIPS
PCID_LIB_CHIPSIM =  ${LIBDIR}/libchipsim_sim.a
PCID_LIBS_BLD += ${PCID_LIB_CHIPSIM} 
endif

ifdef DPP_DFE
PCID_LIB_CHIPSIM =  ${LIBDIR}/libchipsim_sim.a
PCID_LIBS_BLD += ${PCID_LIB_CHIPSIM} 
endif

#ifdef BUILD_MACSEC
PCID_LIBS_BLD += ${EXTERNAL_LIBS}
#endif

export IPROC_NO_ATL = 1
CFGFLAGS += -DIPROC_NO_ATL

all:: ${BLDDIR}/.tree ${build_targets}



# Libraries

.PHONY: _bcm_libraries

_bcm_libraries:
ifndef NO_BCM
	$(MAKE) -C ${SDK}
ifdef BUILD_PHYMOD
	$(MAKE) -C $(SDK)/libs/phymod
endif
endif

# Broadcom Command Monitor

# Build platform_defines.c to compile in config.bcm
#
ifndef NO_SAL_APPL
PLATFORM_DEFINES_OBJ=${BLDDIR}/platform_defines.o
${BLDDIR}/platform_defines.c: $(BLDDIR)/.tree ${SDK}/rc/config.bcm
	$(PERL) ${SDK}/tools/bcm2c_linux.pl ${SDK}/rc/config.bcm >$@
${BLDDIR}/platform_defines.o: ${BLDDIR}/platform_defines.c
	${CC} ${CFLAGS} -c $< -o $@
ifndef NO_VERSION
	touch ${SDK}/rc/config.bcm
endif
else
PLATFORM_DEFINES_OBJ=
endif

# PCI Daemon (chip simulator)
ifdef SIM_STANDALONE_ONLY
#for SIM_STANDALONE_ONLY only libpcid.a should be compiled
${PCID}:

else

LINKER := $(CC)
ifeq ($(C_CPP_CODE_MIX), TRUE)
LINKER := $(CXX)
endif

${PCID}: _bde _bcm_libraries ${BOBJS} ${BLDDIR}/version.o \
         ${PLATFORM_DEFINES_OBJ}
	@rm -f ${PCID}	# Prevent core dump if currently running
	${LINKER} -o ${PCID} ${BOBJS} ${BLDDIR}/version.o \
	${PLATFORM_DEFINES_OBJ} ${LDFLAGS} \
        -Wl,--start-group ${PCID_LIBS_BLD} -Wl,--end-group
endif
# This list has the .c files except for chipsim.c and pcidappl.c
# Update it when new files are added so the AV simulator works.
PCID_SRCS = \
	cmicx_sim.c cmicx_sim_schan.c cmicx_sim_sbus_dma.c cmicx_sim_util.c \
	cmicx_sim_packet_dma.c datum.c cmicsim.c pli.c util.c dma.c  physim.c\
	ism.c \
	l2x.c l3x.c socintf.c proc_req.c packet.c vlan.c \
    mpls.c fpem.c tunnel.c
PCID_OBJS = $(addsuffix .o, $(basename ${PCID_SRCS}))
PCID_BOBJS = $(addprefix ${BLDDIR}/,${PCID_OBJS})

${PCID_LIB}: ${PCID_BOBJS}
	${RM} $@
	$(AR) ${ARFLAGS} $@ $(sort ${PCID_BOBJS})

$(BLDDIR)/%.P: $(BLDDIR)/.tree
	$(info $@)

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

# Clean

clean:: ${BLDDIR}/.tree 
	rm -f ${all_targets}
	rm -f ${BOBJS}
	rm -f ${LIBDIR}/libpcid.a
	rm -f ${BLDDIR}/version.c ${BLDDIR}/version.o
	rm -f ${BLDDIR}/platform_defines.c ${BLDDIR}/platform_defines.o
ifndef NO_BCM
	$(MAKE) -C ${SDK} clean
ifdef BUILD_PHYMOD
	$(MAKE) -C $(SDK)/libs/phymod clean
endif
endif
	$(MAKE) -C ${SDK}/systems/bde/pli clean

distclean::
	$(RM) $(BLDROOT)
	$(RM) $(DEST_DIR)

.PHONY: _bde
_bde:
ifndef NO_BDE
	$(MAKE) -C ${SDK}/systems/bde/pli
endif
