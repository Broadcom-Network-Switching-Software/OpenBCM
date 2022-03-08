FEATURE_LIST := CINT CPU_I2C MEM_SCAN EDITLINE BCM_SAL_PROFILE CUSTOMER TEST INTR APIMODE 
FEATURE_LIST += L3 AUTOCOMPLETE

ifneq ($(COMPILE_OPENNSA_SUPPORT),1)
# Remove RCPU from OpenNSA to avoid segmentation fault in OpenNSA example (shared with XGS).
    FEATURE_LIST += RCPU
endif

BCM_PTL_SPT=1

ifeq ($(NO_MULTIPLE_DEFINITION),1)
# -fcommon :
# In C code, this option controls the placement of global variables defined without an initializer, known as
# tentative definitions in the C standard.
# For -fno-common, the compiler places uninitialized global variables in the BSS section of the object file.
# This inhibits the merging of tentative definitions by the linker so you get a multiple-definition error if
# the same variable is accidentally defined in more than one compilation unit.
# -fcommon is mainly useful to enable legacy code to link without errors.
  CFGFLAGS += -fno-common
endif

# In order to keep backward compatability for J2 gen compilation, we keep using ALL_DNX_CHIPS for DNX2
# and map ALL_DNX_CHIPS to ALL_DNX2_CHIPS in Make.config
ifneq ($(EXCLUDE_DNX2),1)
    ALL_DNX_CHIPS = 1
    FEATURE_LIST += PTP
endif
# In order to keep backward compatability for Ramon compilation, we keep using ALL_DNXF_CHIPS for DNXF1
# and map ALL_DNXF_CHIPS to ALL_DNXF1_CHIPS in Make.config
ifneq ($(EXCLUDE_DNXF1),1)
    ALL_DNXF_CHIPS = 1
endif
#ALL_DPP_CHIPS = 1
#ALL_DFE_CHIPS = 1
override SBX_CHIPS=

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

ifeq ($(THREAD_SANITIZER_IS_ACTIVE),1)
  # These flags are needed for Thread Sanitizer
  CFGFLAGS += -DTHREAD_SANITIZER_IS_ACTIVE=1
  CFGFLAGS += -fsanitize=thread
  # Add linker options but only at the last stage. Intermediate links, as
  # used for Linux modules use a linker which does not recognize thread sanitizer
  # static library.
  # See linux-user-common.mk
  EXT_LDFLAGS  = -fsanitize=thread -static-libtsan
  LINK_STATIC := 0
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
    #
    # This is required for running DVAPI so that the test can exit normally
    # (otherwise, no unit is attached and DVAPI environment complains.
    #
    CFGFLAGS += -DINIT_ON_SECOND_LOOP=1
endif

CFGFLAGS += -D__DUNE_LINUX_BCM_CPU_PCIE__
CFGFLAGS += -D_SIMPLE_MEMORY_ALLOCATION_=9
CFGFLAGS += -DUSE_LINUX_BDE_MMAP=1
ifneq ($(NO_WB),1)
# {
  CFGFLAGS += -DBCM_WARM_BOOT_SUPPORT
  CFGFLAGS += -DBCM_WARM_BOOT_SUPPORT_SW_DUMP
# }
endif
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
  BCM_EXTRA_CC_CFLAGS += -fPIC
  CFGFLAGS += -DBCM_COMPILE_SHARED_LIB
  LINK_STATIC := 0
endif

ifeq ($(COMPILE_OPENNSA_SUPPORT),1)
  CFGFLAGS += -DBCM_COMPILE_STATIC_LIB
  CFGFLAGS += -DBCM_COMPILE_SHARED_LIB
  CFGFLAGS += -DBCM_COMPILE_OPENNSA
  BCM_EXTRA_CC_CFLAGS += -fPIC
endif

ifeq ($(BDE_USE_MSI),1)
  CFGFLAGS += -DBDE_USE_MSI
endif
ifeq ($(BDE_USE_MSIX),1)
  CFGFLAGS += -DBDE_USE_MSIX
endif
ifeq ($(BCM_INSTANCE_SUPPORT),1)
   CFGFLAGS += -DBCM_INSTANCE_SUPPORT
endif

ifneq ($(USE_CLANG),1)
# {
CFGFLAGS += -Werror=all

CFGFLAGS += -Werror=maybe-uninitialized
# }
else
# {
CFGFLAGS += -Werror

CFGFLAGS += -Wno-shift-count-overflow
CFGFLAGS += -Wno-logical-not-parentheses
CFGFLAGS += -Wno-format-security
CFGFLAGS += -Wno-address-of-packed-member
CFGFLAGS += -Wno-enum-conversion
CFGFLAGS += -Wno-shift-negative-value
# }
endif

CFGFLAGS += -Werror=sizeof-array-argument

#Flags to prevent gcc 8.1.0 new warnings to appear as errors.
#(All these flags have been removed so as to activate the maximal types of
#warning)
#
#DEBUG_CFLAGS= -Wno-error=stringop-overflow
#DEBUG_CFLAGS= -Wno-misleading-indentation

#LINK_STATIC := 0

ifneq ($(EXCLUDE_VENDORS),1)
endif

ifeq ($(USE_CLANG),1)
# {
ifeq ($(APPLY_MSAN),1)
# {
    #
    # Note that on MSAN manual, they recommend optimization (-O1 or higher)
    # to make execution time more 'reasonable'.
    #
    CFLAGS += -O1
    LINK_STATIC := 0
    EXTRA_USER_CFLAGS += -fsanitize=memory -fPIE -fno-omit-frame-pointer -fno-optimize-sibling-calls -fsanitize-memory-track-origins -fsanitize-memory-use-after-dtor -fsanitize-recover=all
    EXTRA_USER_LDFLAGS += -fsanitize=memory -fPIE -pie -fno-omit-frame-pointer -fno-optimize-sibling-calls -fsanitize-memory-track-origins -fsanitize-memory-use-after-dtor -fsanitize-recover=all
# }
endif
  #
  # This version of CLANG seems to be having a problem with 'inline'.
  #
  CFGFLAGS += -DINLINE=
  #
  # For CLANG, we do apply -Werror above since we aim at setting all
  # warnings to be treated as errors. If they are all cleaned up, then
  # we may leave the original BCM_CFLAGS (See make/Makefile.unix-user)
  # Otherwise, warnings, which are silenced, are reactivated by '-Werror=all'
  # in BCM_CFLAGS.
  #
  BCM_CFLAGS   = -fno-strict-aliasing
  #
  # Use CLANG 6.0.1 since it is, currently, the only version that has MSAN installed
  # for it (on rhel7). CLANG compiler is selected from rhel7.
  #
  export LLVMVER=6.0.1
  CC=/tools/oss/packages/x86_64-rhel7/llvm/6.0.1/bin/clang
# }
endif
#
# The two lines below are not required for the operation of this make. They
# are here only to clearly display the version of make
#
MAK=/tools/bin/make
$(info $(shell MAKEVER=$(MAKEVER) $(MAK) -v))

