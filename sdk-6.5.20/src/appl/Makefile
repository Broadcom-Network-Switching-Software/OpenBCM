# $Id: Makefile,v 1.26 2012/10/22 21:45:11 dgoeke Exp $
# This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
# 
# Copyright 2007-2020 Broadcom Inc. All rights reserved.
# 

include ${SDK}/make/Make.config

export TARGET
export BLDDIR
export LIBDIR


ifdef ESW_CHIPS
subdirs += diag
endif

ifdef LTSW_CHIPS
subdirs += diag
endif

ifdef DPP_CHIPS
subdirs += diag
subdirs += dpp
subdirs += dcmn
subdirs += reference
endif

ifdef DFE_CHIPS
subdirs += diag
subdirs += dfe
subdirs += dcmn
subdirs += reference
endif

ifdef INCLUDE_DNX_LIBS
subdirs += diag
subdirs += reference
endif

ifdef DNXF_CHIPS
subdirs += diag
subdirs += reference
endif

ifneq (,$(findstring -DINCLUDE_CTEST,$(CFGFLAGS)))
subdirs += ctest
endif

ifdef COND_LIB_PIONEER
subdirs += pioneer_host
endif

ifdef COND_LIB_TEST
subdirs += test
endif

ifdef COND_LIB_TCL
subdirs += ${TCLVER}
endif

ifdef COND_LIB_CPUDB
subdirs += cpudb
endif

ifdef COND_LIB_CPUTRANS
subdirs += cputrans
endif

ifdef COND_LIB_DISCOVER
subdirs += discover
endif

ifdef COND_LIB_STKTASK
subdirs += stktask
endif

ifdef COND_LIB_AEDEV
subdirs += aedev
endif

ifdef COND_LIB_CINT
subdirs += cint
endif

ifdef COND_LIB_C_UNIT
subdirs += c_unit
endif

ifdef PORTMOD_STAND_ALONE
subdirs += diag
subdirs += portmod
endif

ifdef PE_SDK
subdirs += pelib
endif


include ${SDK}/make/Make.subdirs
