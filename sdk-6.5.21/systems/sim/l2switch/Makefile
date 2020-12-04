# $Id: Makefile,v 1.6 2010/08/24 22:47:44 sparks Exp $
# This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
# 
# Copyright 2007-2020 Broadcom Inc. All rights reserved.
#

LOCALDIR = systems/sim/l2switch

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

ifeq ($(INSTALL_DIR),)
INSTALL_DIR=.
endif

include ${SDK}/make/Make.config

#
# To use Purify, add a line here similar to the following:
#
#CC=/net/igor/export/home/pure/purify-4.2-solaris2/purify gcc

#
# Define all targets to build
#

L2SWITCH = ${INSTALL_DIR}/l2switch.${targetplat}${all_suffix}
L2SWITCH_LIB = ${LIBDIR}/libl2switch.a

all_targets = ${L2SWITCH} ${L2SWITCH_LIB}

#
# Define targets to be built by default.
#
build_targets = ${all_targets}

#
# Libraries
#

#DIAG_LIBS = \
#	${COND_LIB_EDITLINE} \
#	libdiag.a libtest.a \
#	${COND_LIB_TCL} \
#	libsal_appl.a libsal_appl_plat.a libplibde.a

L2SWITCH_LIBS =  ${APPL_LIBS} ${DRV_LIBS} ${DIAG_LIBS}
L2SWITCH_LIBS_BLD = $(addprefix ${BLDROOT}/,${L2SWITCH_LIBS})


all:: ${BLDDIR}/.tree ${build_targets}

# Libraries

.PHONY: _bcm_libraries

_bcm_libraries:
ifndef NO_BCM
	$(MAKE) -C ${SDK}
endif

# Example L2 Switch Application

${L2SWITCH}: _bde _bcm_libraries ${BOBJS} ${BLDDIR}/version.o
	@rm -f ${L2SWITCH}	# Prevent core dump if currently running
	${CC} -o ${L2SWITCH} ${BOBJS} ${BLDDIR}/version.o \
		${L2SWITCH_LIBS_BLD} \
		${L2SWITCH_LIBS_BLD} \
		${L2SWITCH_LIBS_BLD} \
		${LDFLAGS}

L2SWITCH_SRCS = \
	l2switch.c
L2SWITCH_OBJS = $(addsuffix .o, $(basename ${L2SWITCH_SRCS}))
L2SWITCH_BOBJS = $(addprefix ${BLDDIR}/,${L2SWITCH_OBJS})

${L2SWITCH_LIB}: ${L2SWITCH_BOBJS}
	${RM} $@
	$Q$(AR) ${ARFLAGS} $@ $(sort ${L2SWITCH_BOBJS})

# Build Version

${BLDDIR}/version.c: ${BLDDIR}/.tree
	$(MKVERS) > $@
${BLDDIR}/version.o: ${BLDDIR}/version.c
	${CC} ${CFLAGS} -c $< -o $@

# Make.depend is before clean:: so that Make.depend's clean:: runs first.

include ${SDK}/make/Make.depend

# Clean

clean:: ${BLDDIR}/.tree 
	rm -f ${all_targets}
	rm -f ${BOBJS}
	rm -f ${LIBDIR}/libl2switch.a
	rm -f ${BLDDIR}/version.c ${BLDDIR}/version.o
ifndef NO_BCM
	$(MAKE) -C ${SDK} clean
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
