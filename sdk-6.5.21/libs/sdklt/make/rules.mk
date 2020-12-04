#
# This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
# 
# Copyright 2007-2020 Broadcom Inc. All rights reserved.
#
# SDK default make rules. These can optionally be overridden by
# letting SDK_MAKE_RULES point to a different rules file.
#

ifdef SDK_MAKE_RULES

include $(SDK_MAKE_RULES)

else

#
# Add build option dependency if present
#
BLDOPTS := $(SDK_BLDDIR)/.options
ifneq (,$(wildcard $(BLDOPTS)))
BLDOPTS_DEP = $(BLDOPTS)
endif

#
# Using abspath ensures that the __FILE__ macro supplies the full path
#
$(BLDDIR)/%.$(OBJSUFFIX): %.c $(BLDDIR)/.tree $(BLDOPTS_DEP)
	@$(ECHO) 'Compiling $(LOCALDIR)/$<'
	$(Q)$(CC) $(CPPFLAGS) $(CFLAGS) -c $(abspath $<) -o $@

$(BLDDIR)/%.$(OBJSUFFIX): %.cpp $(BLDDIR)/.tree $(BLDOPTS_DEP)
	@$(ECHO) 'Compiling $(LOCALDIR)/$<'
	$(Q)$(CC) $(CPPFLAGS) $(CFLAGS) -c $(abspath $<) -o $@

$(BLDDIR)/%.$(OBJSUFFIX): %.cc $(BLDDIR)/.tree $(BLDOPTS_DEP)
	@$(ECHO) 'Compiling $(LOCALDIR)/$<'
	$(Q)$(CC) $(CPPFLAGS) $(CFLAGS) -c $(abspath $<) -o $@


#
# Default to normal mode or chunk mode based on size of BOBJS
#
ifeq ($(SDK_MAKE_LIBRARY_MODE),)

ifneq ($(word 600,$(BOBJS)),)
SDK_MAKE_LIBRARY_MODE=chunk
else
SDK_MAKE_LIBRARY_MODE=normal
endif

endif


#
# Normal Library Builds. 
# All object files archived at once. 
#
ifeq ($(SDK_MAKE_LIBRARY_MODE),normal)

$(LIBDIR)/$(LIBNAME).$(LIBSUFFIX): $(BOBJS) $(BLDDIR)/.tree 
	@$(ECHO) 'Building library $(LIBNAME)...'
	$(Q)$(AR) $(ARFLAGS) $@ $(BOBJS)

endif


# Serialized make. No limit on number of files. 

ifeq ($(SDK_MAKE_LIBRARY_MODE),serial)

#
# This mode can be selected when the OBJECT list is too large
# to archive as a single execution due to command-line constraints. 
#
# This occurs when the file paths are very long and the object
# count is very high. 
#
# Instead of archiving all files at once, we use the built in
# auto-archive target for each object file. 
#
# These rules cannot be executing in parallel because they would
# cause concurrent access to the target library file, which will
# corrupt it. As a result, this mode is split into two different
# invocations:
#	The first stage allows parallel builds of all object files
#	The second stage disables parallelism and archives the objects
#
#
# This mode is very SLOW due to serial archive and the obviously large
# number of files which need to be archived. 
# 

ifndef SDK_MAKE_LIBRARY_SERIAL_SECOND_STAGE

#
# This is the first stage -- build all object files needed by the library
# These are the prequisites. Then invoke the second stage to archive them. 
#
$(LIBDIR)/$(LIBNAME).$(LIBSUFFIX): $(BOBJS) $(BLDDIR)/.tree 
	$(MAKE) $(LIBDIR)/$(LIBNAME).$(LIBSUFFIX) SDK_MAKE_LIBRARY_SERIAL_SECOND_STAGE=1 SDK_MAKE_LIBRARY_MODE=serial

else

#
# This is the second stage -- disable parallelism and archive the resulting files. 
#

.NOTPARALLEL:

# Builtin archive target
LIBBUILD = $(LIBDIR)/$(LIBNAME).$(LIBSUFFIX)($(BOBJS))

# Custom archive rule
.o.$(LIBSUFFIX):
	$(Q)$(AR) $(ARFLAGS) $@ $<

# Archive the library
$(LIBDIR)/$(LIBNAME).$(LIBSUFFIX): $(LIBBUILD)
	@$(ECHO) 'Serial built library $(notdir $@)'

endif
endif


#
# Chunking Library Mode
# 
# This is hideous but fast. It also works in parallel builds. 
# Just don't look at it. Ever. 
#
# You don't want to enable this for every library. 
#

ifeq ($(SDK_MAKE_LIBRARY_MODE),chunk)

BOBJ_00 = $(wordlist    1, 250, $(BOBJS))
BOBJ_01 = $(wordlist  251, 500, $(BOBJS))
BOBJ_02 = $(wordlist  501, 750, $(BOBJS))
BOBJ_03 = $(wordlist  751,1000, $(BOBJS))
BOBJ_04 = $(wordlist 1001,1250, $(BOBJS))
BOBJ_05 = $(wordlist 1251,1500, $(BOBJS))
BOBJ_06 = $(wordlist 1501,1750, $(BOBJS))
BOBJ_07 = $(wordlist 1751,2000, $(BOBJS))
BOBJ_08 = $(wordlist 2001,2250, $(BOBJS))
BOBJ_09 = $(wordlist 2251,2500, $(BOBJS))
BOBJ_10 = $(wordlist 2501,2750, $(BOBJS))
BOBJ_11 = $(wordlist 2751,3000, $(BOBJS))
BOBJ_12 = $(wordlist 3001,3250, $(BOBJS))
BOBJ_13 = $(wordlist 3251,3500, $(BOBJS))
BOBJ_14 = $(wordlist 3501,3750, $(BOBJS))
BOBJ_15 = $(wordlist 3751,4000, $(BOBJS))
BOBJ_16 = $(wordlist 4001,4250, $(BOBJS))
BOBJ_17 = $(wordlist 4251,4500, $(BOBJS))
BOBJ_18 = $(wordlist 4501,4750, $(BOBJS))
BOBJ_19 = $(wordlist 4751,5000, $(BOBJS))
BOBJ_20 = $(wordlist 5001,5250, $(BOBJS))
BOBJ_21 = $(wordlist 5251,5500, $(BOBJS))
BOBJ_22 = $(wordlist 5501,5750, $(BOBJS))
BOBJ_23 = $(wordlist 5751,6000, $(BOBJS))
BOBJ_24 = $(wordlist 6001,6250, $(BOBJS))
BOBJ_25 = $(wordlist 6251,6500, $(BOBJS))
BOBJ_26 = $(wordlist 6501,6750, $(BOBJS))
BOBJ_27 = $(wordlist 6751,7000, $(BOBJS))
BOBJ_28 = $(wordlist 7001,7250, $(BOBJS))
BOBJ_29 = $(wordlist 7251,7500, $(BOBJS))
BOBJ_30 = $(wordlist 7501,7750, $(BOBJS))
BOBJ_31 = $(wordlist 7751,8000, $(BOBJS))
BOBJ_32 = $(wordlist 8001,8250, $(BOBJS))
BOBJ_33 = $(wordlist 8251,8500, $(BOBJS))
BOBJ_34 = $(wordlist 8501,8750, $(BOBJS))
BOBJ_35 = $(wordlist 8751,9000, $(BOBJS))
BOBJ_36 = $(wordlist 9001,9250, $(BOBJS))
BOBJ_37 = $(wordlist 9251,9500, $(BOBJS))
BOBJ_38 = $(wordlist 9501,9750, $(BOBJS))
BOBJ_39 = $(wordlist 9751,10000, $(BOBJS))
BOBJ_40 = $(wordlist 10001,10250, $(BOBJS))
BOBJ_41 = $(wordlist 10251,10500, $(BOBJS))
BOBJ_42 = $(wordlist 10501,10750, $(BOBJS))
BOBJ_44 = $(wordlist 10751,11000, $(BOBJS))
BOBJ_45 = $(wordlist 11001,11250, $(BOBJS))
BOBJ_46 = $(wordlist 11251,11500, $(BOBJS))
BOBJ_47 = $(wordlist 11501,11750, $(BOBJS))
BOBJ_48 = $(wordlist 11751,12000, $(BOBJS))
BOBJ_49 = $(wordlist 12001,12250, $(BOBJS))
BOBJ_50 = $(wordlist 12251,12500, $(BOBJS))
BOBJ_51 = $(wordlist 12501,12750, $(BOBJS))
BOBJ_52 = $(wordlist 12751,13000, $(BOBJS))
BOBJ_53 = $(wordlist 13001,13250, $(BOBJS))
BOBJ_54 = $(wordlist 13251,13500, $(BOBJS))
BOBJ_55 = $(wordlist 13501,13750, $(BOBJS))
BOBJ_56 = $(wordlist 13751,14000, $(BOBJS))
BOBJ_57 = $(wordlist 14001,14250, $(BOBJS))
BOBJ_58 = $(wordlist 14251,14500, $(BOBJS))
BOBJ_59 = $(wordlist 14501,14750, $(BOBJS))
BOBJ_60 = $(wordlist 14751,15000, $(BOBJS))
BOBJ_61 = $(wordlist 15001,15250, $(BOBJS))
BOBJ_62 = $(wordlist 15251,15500, $(BOBJS))
BOBJ_63 = $(wordlist 15501,15750, $(BOBJS))
BOBJ_64 = $(wordlist 15751,16000, $(BOBJS))
BOBJ_65 = $(wordlist 16001,16250, $(BOBJS))
BOBJ_66 = $(wordlist 16251,16500, $(BOBJS))
BOBJ_67 = $(wordlist 16501,16750, $(BOBJS))
BOBJ_68 = $(wordlist 16751,17000, $(BOBJS))
BOBJ_69 = $(wordlist 17001,17250, $(BOBJS))
BOBJ_70 = $(wordlist 17251,17500, $(BOBJS))
BOBJ_71 = $(wordlist 17501,17750, $(BOBJS))
BOBJ_72 = $(wordlist 17751,18000, $(BOBJS))
BOBJ_73 = $(wordlist 18001,18250, $(BOBJS))
BOBJ_74 = $(wordlist 18251,18500, $(BOBJS))
BOBJ_75 = $(wordlist 18501,18750, $(BOBJS))
BOBJ_76 = $(wordlist 18751,19000, $(BOBJS))
BOBJ_77 = $(wordlist 19001,19250, $(BOBJS))
BOBJ_78 = $(wordlist 19251,19500, $(BOBJS))
BOBJ_79 = $(wordlist 19501,19750, $(BOBJS))
BOBJ_80 = $(wordlist 19751,20000, $(BOBJS))

ifneq ($(word 20001,$(BOBJS)),)
$(error Too many files in chunk library build $(LIBNAME). Add more chunks @ $$SDK/make/rules.mk)
endif

$(LIBDIR)/%.$(LIBSUFFIX): $(BOBJS) $(BLDDIR)/.tree
	@$(ECHO) 'Chunk Building library $(LIBNAME)...'
ifneq ($(BOBJ_00),)
	$(Q)$(AR) $(ARFLAGS) $@ $(BOBJ_00)
endif
ifneq ($(BOBJ_01),)
	$(Q)$(AR) $(ARFLAGS) $@ $(BOBJ_01)
endif
ifneq ($(BOBJ_02),)
	$(Q)$(AR) $(ARFLAGS) $@ $(BOBJ_02)
endif
ifneq ($(BOBJ_03),)
	$(Q)$(AR) $(ARFLAGS) $@ $(BOBJ_03)
endif
ifneq ($(BOBJ_04),)
	$(Q)$(AR) $(ARFLAGS) $@ $(BOBJ_04)
endif
ifneq ($(BOBJ_05),)
	$(Q)$(AR) $(ARFLAGS) $@ $(BOBJ_05)
endif
ifneq ($(BOBJ_06),)
	$(Q)$(AR) $(ARFLAGS) $@ $(BOBJ_06)
endif
ifneq ($(BOBJ_07),)
	$(Q)$(AR) $(ARFLAGS) $@ $(BOBJ_07)
endif
ifneq ($(BOBJ_08),)
	$(Q)$(AR) $(ARFLAGS) $@ $(BOBJ_08)
endif
ifneq ($(BOBJ_09),)
	$(Q)$(AR) $(ARFLAGS) $@ $(BOBJ_09)
endif
ifneq ($(BOBJ_10),)
	$(Q)$(AR) $(ARFLAGS) $@ $(BOBJ_10)
endif
ifneq ($(BOBJ_11),)
	$(Q)$(AR) $(ARFLAGS) $@ $(BOBJ_11)
endif
ifneq ($(BOBJ_12),)
	$(Q)$(AR) $(ARFLAGS) $@ $(BOBJ_12)
endif
ifneq ($(BOBJ_13),)
	$(Q)$(AR) $(ARFLAGS) $@ $(BOBJ_13)
endif
ifneq ($(BOBJ_14),)
	$(Q)$(AR) $(ARFLAGS) $@ $(BOBJ_14)
endif
ifneq ($(BOBJ_15),)
	$(Q)$(AR) $(ARFLAGS) $@ $(BOBJ_15)
endif
ifneq ($(BOBJ_16),)
	$(Q)$(AR) $(ARFLAGS) $@ $(BOBJ_16)
endif
ifneq ($(BOBJ_17),)
	$(Q)$(AR) $(ARFLAGS) $@ $(BOBJ_17)
endif
ifneq ($(BOBJ_18),)
	$(Q)$(AR) $(ARFLAGS) $@ $(BOBJ_18)
endif
ifneq ($(BOBJ_19),)
	$(Q)$(AR) $(ARFLAGS) $@ $(BOBJ_19)
endif
ifneq ($(BOBJ_20),)
	$(Q)$(AR) $(ARFLAGS) $@ $(BOBJ_20)
endif
ifneq ($(BOBJ_21),)
	$(Q)$(AR) $(ARFLAGS) $@ $(BOBJ_21)
endif
ifneq ($(BOBJ_22),)
	$(Q)$(AR) $(ARFLAGS) $@ $(BOBJ_22)
endif
ifneq ($(BOBJ_23),)
	$(Q)$(AR) $(ARFLAGS) $@ $(BOBJ_23)
endif
ifneq ($(BOBJ_24),)
	$(Q)$(AR) $(ARFLAGS) $@ $(BOBJ_24)
endif
ifneq ($(BOBJ_25),)
	$(Q)$(AR) $(ARFLAGS) $@ $(BOBJ_25)
endif
ifneq ($(BOBJ_26),)
	$(Q)$(AR) $(ARFLAGS) $@ $(BOBJ_26)
endif
ifneq ($(BOBJ_27),)
	$(Q)$(AR) $(ARFLAGS) $@ $(BOBJ_27)
endif
ifneq ($(BOBJ_28),)
	$(Q)$(AR) $(ARFLAGS) $@ $(BOBJ_28)
endif
ifneq ($(BOBJ_29),)
	$(Q)$(AR) $(ARFLAGS) $@ $(BOBJ_29)
endif
ifneq ($(BOBJ_30),)
	$(Q)$(AR) $(ARFLAGS) $@ $(BOBJ_30)
endif
ifneq ($(BOBJ_31),)
	$(Q)$(AR) $(ARFLAGS) $@ $(BOBJ_31)
endif
ifneq ($(BOBJ_32),)
	$(Q)$(AR) $(ARFLAGS) $@ $(BOBJ_32)
endif
ifneq ($(BOBJ_33),)
	$(Q)$(AR) $(ARFLAGS) $@ $(BOBJ_33)
endif
ifneq ($(BOBJ_34),)
	$(Q)$(AR) $(ARFLAGS) $@ $(BOBJ_34)
endif
ifneq ($(BOBJ_35),)
	$(Q)$(AR) $(ARFLAGS) $@ $(BOBJ_35)
endif
ifneq ($(BOBJ_36),)
	$(Q)$(AR) $(ARFLAGS) $@ $(BOBJ_36)
endif
ifneq ($(BOBJ_37),)
	$(Q)$(AR) $(ARFLAGS) $@ $(BOBJ_37)
endif
ifneq ($(BOBJ_38),)
	$(Q)$(AR) $(ARFLAGS) $@ $(BOBJ_38)
endif
ifneq ($(BOBJ_39),)
	$(Q)$(AR) $(ARFLAGS) $@ $(BOBJ_39)
endif
ifneq ($(BOBJ_40),)
	$(Q)$(AR) $(ARFLAGS) $@ $(BOBJ_40)
endif
ifneq ($(BOBJ_41),)
	$(Q)$(AR) $(ARFLAGS) $@ $(BOBJ_41)
endif
ifneq ($(BOBJ_42),)
	$(Q)$(AR) $(ARFLAGS) $@ $(BOBJ_42)
endif
ifneq ($(BOBJ_43),)
	$(Q)$(AR) $(ARFLAGS) $@ $(BOBJ_43)
endif
ifneq ($(BOBJ_44),)
	$(Q)$(AR) $(ARFLAGS) $@ $(BOBJ_44)
endif
ifneq ($(BOBJ_45),)
	$(Q)$(AR) $(ARFLAGS) $@ $(BOBJ_45)
endif
ifneq ($(BOBJ_46),)
	$(Q)$(AR) $(ARFLAGS) $@ $(BOBJ_46)
endif
ifneq ($(BOBJ_47),)
	$(Q)$(AR) $(ARFLAGS) $@ $(BOBJ_47)
endif
ifneq ($(BOBJ_48),)
	$(Q)$(AR) $(ARFLAGS) $@ $(BOBJ_48)
endif
ifneq ($(BOBJ_49),)
	$(Q)$(AR) $(ARFLAGS) $@ $(BOBJ_49)
endif
ifneq ($(BOBJ_50),)
	$(Q)$(AR) $(ARFLAGS) $@ $(BOBJ_50)
endif
ifneq ($(BOBJ_51),)
	$(Q)$(AR) $(ARFLAGS) $@ $(BOBJ_51)
endif
ifneq ($(BOBJ_52),)
	$(Q)$(AR) $(ARFLAGS) $@ $(BOBJ_52)
endif
ifneq ($(BOBJ_53),)
	$(Q)$(AR) $(ARFLAGS) $@ $(BOBJ_53)
endif
ifneq ($(BOBJ_54),)
	$(Q)$(AR) $(ARFLAGS) $@ $(BOBJ_54)
endif
ifneq ($(BOBJ_55),)
	$(Q)$(AR) $(ARFLAGS) $@ $(BOBJ_55)
endif
ifneq ($(BOBJ_56),)
	$(Q)$(AR) $(ARFLAGS) $@ $(BOBJ_56)
endif
ifneq ($(BOBJ_57),)
	$(Q)$(AR) $(ARFLAGS) $@ $(BOBJ_57)
endif
ifneq ($(BOBJ_58),)
	$(Q)$(AR) $(ARFLAGS) $@ $(BOBJ_58)
endif
ifneq ($(BOBJ_59),)
	$(Q)$(AR) $(ARFLAGS) $@ $(BOBJ_59)
endif
ifneq ($(BOBJ_60),)
	$(Q)$(AR) $(ARFLAGS) $@ $(BOBJ_60)
endif
ifneq ($(BOBJ_61),)
	$(Q)$(AR) $(ARFLAGS) $@ $(BOBJ_61)
endif
ifneq ($(BOBJ_62),)
	$(Q)$(AR) $(ARFLAGS) $@ $(BOBJ_62)
endif
ifneq ($(BOBJ_63),)
	$(Q)$(AR) $(ARFLAGS) $@ $(BOBJ_63)
endif
ifneq ($(BOBJ_64),)
	$(Q)$(AR) $(ARFLAGS) $@ $(BOBJ_64)
endif
ifneq ($(BOBJ_65),)
	$(Q)$(AR) $(ARFLAGS) $@ $(BOBJ_65)
endif
ifneq ($(BOBJ_66),)
	$(Q)$(AR) $(ARFLAGS) $@ $(BOBJ_66)
endif
ifneq ($(BOBJ_67),)
	$(Q)$(AR) $(ARFLAGS) $@ $(BOBJ_67)
endif
ifneq ($(BOBJ_68),)
	$(Q)$(AR) $(ARFLAGS) $@ $(BOBJ_68)
endif
ifneq ($(BOBJ_69),)
	$(Q)$(AR) $(ARFLAGS) $@ $(BOBJ_69)
endif
ifneq ($(BOBJ_70),)
	$(Q)$(AR) $(ARFLAGS) $@ $(BOBJ_70)
endif
ifneq ($(BOBJ_71),)
	$(Q)$(AR) $(ARFLAGS) $@ $(BOBJ_71)
endif
ifneq ($(BOBJ_72),)
	$(Q)$(AR) $(ARFLAGS) $@ $(BOBJ_72)
endif
ifneq ($(BOBJ_73),)
	$(Q)$(AR) $(ARFLAGS) $@ $(BOBJ_73)
endif
ifneq ($(BOBJ_74),)
	$(Q)$(AR) $(ARFLAGS) $@ $(BOBJ_74)
endif
ifneq ($(BOBJ_75),)
	$(Q)$(AR) $(ARFLAGS) $@ $(BOBJ_75)
endif
ifneq ($(BOBJ_76),)
	$(Q)$(AR) $(ARFLAGS) $@ $(BOBJ_76)
endif
ifneq ($(BOBJ_77),)
	$(Q)$(AR) $(ARFLAGS) $@ $(BOBJ_77)
endif
ifneq ($(BOBJ_78),)
	$(Q)$(AR) $(ARFLAGS) $@ $(BOBJ_78)
endif
ifneq ($(BOBJ_79),)
	$(Q)$(AR) $(ARFLAGS) $@ $(BOBJ_79)
endif
ifneq ($(BOBJ_80),)
	$(Q)$(AR) $(ARFLAGS) $@ $(BOBJ_80)
endif


# End of chunk library mode
endif
endif
