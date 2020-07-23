#
# This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
# 
# Copyright 2007-2020 Broadcom Inc. All rights reserved.
#
FEATURE_LIST := CINT ATPTRANS_SOCKET L3 CPU_I2C I2C MEM_SCAN EDITLINE CHASSIS TEST BCM_SAL_PROFILE RCPU INTR APIMODE PTP CUSTOMER MSTP DUNE_UI AUTOCOMPLETE

BCM_PTL_SPT=1

# In order to keep backward compatability for J2 gen compilation, we keep using ALL_DNX_CHIPS for DNX2 and map ALL_DNX_CHIPS to ALL_DNX2_CHIPS in Make.config
ifneq ($(EXCLUDE_DNX2),1)
ALL_DNX_CHIPS = 1
endif
ifneq ($(EXCLUDE_DNXF),1)
    ALL_DNXF_CHIPS = 1
endif
#ALL_DPP_CHIPS = 1
#ALL_DFE_CHIPS = 1
override SBX_CHIPS=

VENDOR_LIST = BROADCOM

ifeq ($(VALGRIND_SUPPORT),1)
  VALGRIND_DIR = /projects/NTSW_SW_USRS/common/tools/valgrind-3.11.0
  CFGFLAGS += -DUSE_EXTERNAL_MEM_CHECKING=1
  CFGFLAGS += -DUSE_VALGRIND_CLIENT_REQUESTS -I$(VALGRIND_DIR) -I$(VALGRIND_DIR)/include
  CFGFLAGS += -DVALGRIND_SUPPORT=1
  #
  # Indicate we want to use our string utilities (such as strcmp) and
  # not the standard C-runtime-library. 
  # This may be required because, for some compilers, the runtime-library accesses
  # memory in resolution of 8 bytes, which collides with Valgrind.
  #
  CFGFLAGS += -DUSE_CUSTOM_STR_UTILS=1
endif

ifeq ($(ALLOC_DEBUG_SUPPORT),1)
    #
    # To only display memory consumption, comment the following two lines
    #
    CFGFLAGS += -DAGGRESSIVE_ALLOC_DEBUG_TESTING=1
    #
    # To display some info on sal alloc/free during initialization, set
    # DISPLAY_MEM_USAGE
    # to a non-zero value
    #
    CFGFLAGS += -DDISPLAY_MEM_USAGE=1
endif

CFGFLAGS += -D__DUNE_LINUX_BCM_CPU_PCIE__
CFGFLAGS += -D_SIMPLE_MEMORY_ALLOCATION_=9
CFGFLAGS += -DUSE_LINUX_BDE_MMAP=1
CFGFLAGS += -DBCM_WARM_BOOT_SUPPORT
CFGFLAGS += -DBCM_WARM_BOOT_API_TEST
CFGFLAGS += -DBCM_WARM_BOOT_SUPPORT_SW_DUMP
CFGFLAGS += -DSERDES_API_FLOATING_POINT
CFGFLAGS += -DBCM_EASY_RELOAD_WB_COMPAT_SUPPORT
CFGFLAGS += -DBCM_CONTROL_API_TRACKING
CFGFLAGS += -DDNX_INTERNAL
CFGFLAGS += -DSOC_PCI_DEBUG
ifeq ($(STATIC_LIBRARY_SUPPORT),1)
  CFGFLAGS += -DBCM_COMPILE_STATIC_LIB
endif

# SHARED_LIBRARY_SUPPORT will enable the shared library creation and compile bcm user with it.
# To create a shared lib it will add PIC compilation flag and will change the BINUTILSVER version to 2.27
ifeq ($(SHARED_LIBRARY_SUPPORT),1)
  CFGFLAGS += -DBCM_COMPILE_SHARED_LIB
  LINK_STATIC := 0
endif

ifeq ($(COMPILE_OPENNSA_SUPPORT),1)
  CFGFLAGS += -DBCM_COMPILE_STATIC_LIB
  CFGFLAGS += -DBCM_COMPILE_SHARED_LIB
  CFGFLAGS += -DBCM_COMPILE_OPENNSA
endif

CFGFLAGS += -Werror=all

#Flags to prevent gcc 8.1.0 new warnings to appear as errors.
#(All these flags have been removed so as to activate the maximal types of
#warning)
#
#DEBUG_CFLAGS= -Wno-error=stringop-overflow
#DEBUG_CFLAGS= -Wno-misleading-indentation

#LINK_STATIC := 0

ifneq ($(EXCLUDE_VENDORS),1)
    VENDOR_LIST=BROADCOM DNX
endif


#
# In YDC environment makefile for linux-kernel-bde.ko and linux-user-bde.ko requires
# make version 3.8.1
#
MAKEFILE_FULLPATH=/tools/oss/packages/x86_64-rhel6/make/3.81/bin/make
ifneq ("$(wildcard $(MAKEFILE_FULLPATH))","")
    export MAKE=$(MAKEFILE_FULLPATH)
endif
#
# The three lines below are not required for the operation of this make. They
# are here only to clearly display the version of make
#
export MAKEVER=3.81
MAK=/tools/bin/make
$(info $(shell MAKEVER=$(MAKEVER) $(MAK) -v))

