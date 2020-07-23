#
# This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
# 
# Copyright 2007-2020 Broadcom Inc. All rights reserved.
#
# SDK make rules for libraries
#

include $(SDK)/make/config.mk

ifeq (1,$(LIBSUM))

# Create source file checksum instead of building library
include $(SDK)/make/libsum.mk

else

include $(SDK)/make/rules.mk

BLIBNAME = $(LIBDIR)/$(LIBNAME).$(LIBSUFFIX)

.SECONDARY:: $(BOBJS)

all:: $(BLDDIR)/.tree $(BLIBNAME)

ifneq ($(SDK_MAKE_LIBRARY_MODE),chunk)

# Normal cleaning

clean:: cleanlibs
	@$(ECHO) Cleaning objects for $(notdir $(BLIBNAME))
	$(Q)$(RM) $(BLDDIR)/.tree $(BOBJS)

else

# Chunk cleaning (required if large number of objects)

clean:: cleanlibs
	@$(ECHO) Chunk cleaning objects for $(notdir $(BLIBNAME))
ifneq ($(BOBJ_00),)
	$(Q)$(RM) $(BOBJ_00)
endif
ifneq ($(BOBJ_01),)
	$(Q)$(RM) $(BOBJ_01)
endif
ifneq ($(BOBJ_02),)
	$(Q)$(RM) $(BOBJ_02)
endif
ifneq ($(BOBJ_03),)
	$(Q)$(RM) $(BOBJ_03)
endif
ifneq ($(BOBJ_04),)
	$(Q)$(RM) $(BOBJ_04)
endif
ifneq ($(BOBJ_05),)
	$(Q)$(RM) $(BOBJ_05)
endif
ifneq ($(BOBJ_06),)
	$(Q)$(RM) $(BOBJ_06)
endif
ifneq ($(BOBJ_07),)
	$(Q)$(RM) $(BOBJ_07)
endif
ifneq ($(BOBJ_08),)
	$(Q)$(RM) $(BOBJ_08)
endif
ifneq ($(BOBJ_09),)
	$(Q)$(RM) $(BOBJ_09)
endif
ifneq ($(BOBJ_10),)
	$(Q)$(RM) $(BOBJ_10)
endif
ifneq ($(BOBJ_11),)
	$(Q)$(RM) $(BOBJ_11)
endif
ifneq ($(BOBJ_12),)
	$(Q)$(RM) $(BOBJ_12)
endif
ifneq ($(BOBJ_13),)
	$(Q)$(RM) $(BOBJ_13)
endif
ifneq ($(BOBJ_14),)
	$(Q)$(RM) $(BOBJ_14)
endif
ifneq ($(BOBJ_15),)
	$(Q)$(RM) $(BOBJ_15)
endif
ifneq ($(BOBJ_16),)
	$(Q)$(RM) $(BOBJ_16)
endif
ifneq ($(BOBJ_17),)
	$(Q)$(RM) $(BOBJ_17)
endif
ifneq ($(BOBJ_18),)
	$(Q)$(RM) $(BOBJ_18)
endif
ifneq ($(BOBJ_19),)
	$(Q)$(RM) $(BOBJ_19)
endif
ifneq ($(BOBJ_20),)
	$(Q)$(RM) $(BOBJ_20)
endif
ifneq ($(BOBJ_21),)
	$(Q)$(RM) $(BOBJ_21)
endif
ifneq ($(BOBJ_22),)
	$(Q)$(RM) $(BOBJ_22)
endif
ifneq ($(BOBJ_23),)
	$(Q)$(RM) $(BOBJ_23)
endif
ifneq ($(BOBJ_24),)
	$(Q)$(RM) $(BOBJ_24)
endif
ifneq ($(BOBJ_25),)
	$(Q)$(RM) $(BOBJ_25)
endif
ifneq ($(BOBJ_26),)
	$(Q)$(RM) $(BOBJ_26)
endif
ifneq ($(BOBJ_27),)
	$(Q)$(RM) $(BOBJ_27)
endif
ifneq ($(BOBJ_28),)
	$(Q)$(RM) $(BOBJ_28)
endif
ifneq ($(BOBJ_29),)
	$(Q)$(RM) $(BOBJ_29)
endif
ifneq ($(BOBJ_30),)
	$(Q)$(RM) $(BOBJ_30)
endif
ifneq ($(BOBJ_31),)
	$(Q)$(RM) $(BOBJ_31)
endif
ifneq ($(BOBJ_32),)
	$(Q)$(RM) $(BOBJ_32)
endif
ifneq ($(BOBJ_33),)
	$(Q)$(RM) $(BOBJ_33)
endif
ifneq ($(BOBJ_34),)
	$(Q)$(RM) $(BOBJ_34)
endif
ifneq ($(BOBJ_35),)
	$(Q)$(RM) $(BOBJ_35)
endif
ifneq ($(BOBJ_36),)
	$(Q)$(RM) $(BOBJ_36)
endif
ifneq ($(BOBJ_37),)
	$(Q)$(RM) $(BOBJ_37)
endif
ifneq ($(BOBJ_38),)
	$(Q)$(RM) $(BOBJ_38)
endif
ifneq ($(BOBJ_39),)
	$(Q)$(RM) $(BOBJ_39)
endif
ifneq ($(BOBJ_40),)
	$(Q)$(RM) $(BOBJ_40)
endif
ifneq ($(BOBJ_41),)
	$(Q)$(RM) $(BOBJ_41)
endif
ifneq ($(BOBJ_42),)
	$(Q)$(RM) $(BOBJ_42)
endif
ifneq ($(BOBJ_43),)
	$(Q)$(RM) $(BOBJ_43)
endif
ifneq ($(BOBJ_44),)
	$(Q)$(RM) $(BOBJ_44)
endif
ifneq ($(BOBJ_45),)
	$(Q)$(RM) $(BOBJ_45)
endif
ifneq ($(BOBJ_46),)
	$(Q)$(RM) $(BOBJ_46)
endif
ifneq ($(BOBJ_47),)
	$(Q)$(RM) $(BOBJ_47)
endif
ifneq ($(BOBJ_48),)
	$(Q)$(RM) $(BOBJ_48)
endif
ifneq ($(BOBJ_49),)
	$(Q)$(RM) $(BOBJ_49)
endif
ifneq ($(BOBJ_50),)
	$(Q)$(RM) $(BOBJ_50)
endif
ifneq ($(BOBJ_51),)
	$(Q)$(RM) $(BOBJ_51)
endif
ifneq ($(BOBJ_52),)
	$(Q)$(RM) $(BOBJ_52)
endif
ifneq ($(BOBJ_53),)
	$(Q)$(RM) $(BOBJ_53)
endif
ifneq ($(BOBJ_54),)
	$(Q)$(RM) $(BOBJ_54)
endif
ifneq ($(BOBJ_55),)
	$(Q)$(RM) $(BOBJ_55)
endif
ifneq ($(BOBJ_56),)
	$(Q)$(RM) $(BOBJ_56)
endif
ifneq ($(BOBJ_57),)
	$(Q)$(RM) $(BOBJ_57)
endif
ifneq ($(BOBJ_58),)
	$(Q)$(RM) $(BOBJ_58)
endif
ifneq ($(BOBJ_59),)
	$(Q)$(RM) $(BOBJ_59)
endif
ifneq ($(BOBJ_60),)
	$(Q)$(RM) $(BOBJ_60)
endif
ifneq ($(BOBJ_61),)
	$(Q)$(RM) $(BOBJ_61)
endif
ifneq ($(BOBJ_62),)
	$(Q)$(RM) $(BOBJ_62)
endif
ifneq ($(BOBJ_63),)
	$(Q)$(RM) $(BOBJ_63)
endif
ifneq ($(BOBJ_64),)
	$(Q)$(RM) $(BOBJ_64)
endif
ifneq ($(BOBJ_65),)
	$(Q)$(RM) $(BOBJ_65)
endif
ifneq ($(BOBJ_66),)
	$(Q)$(RM) $(BOBJ_66)
endif
ifneq ($(BOBJ_67),)
	$(Q)$(RM) $(BOBJ_67)
endif
ifneq ($(BOBJ_68),)
	$(Q)$(RM) $(BOBJ_68)
endif
ifneq ($(BOBJ_69),)
	$(Q)$(RM) $(BOBJ_69)
endif
ifneq ($(BOBJ_70),)
	$(Q)$(RM) $(BOBJ_70)
endif
ifneq ($(BOBJ_71),)
	$(Q)$(RM) $(BOBJ_71)
endif
ifneq ($(BOBJ_72),)
	$(Q)$(RM) $(BOBJ_72)
endif
ifneq ($(BOBJ_73),)
	$(Q)$(RM) $(BOBJ_73)
endif
ifneq ($(BOBJ_74),)
	$(Q)$(RM) $(BOBJ_74)
endif
ifneq ($(BOBJ_75),)
	$(Q)$(RM) $(BOBJ_75)
endif
ifneq ($(BOBJ_76),)
	$(Q)$(RM) $(BOBJ_76)
endif
ifneq ($(BOBJ_77),)
	$(Q)$(RM) $(BOBJ_77)
endif
ifneq ($(BOBJ_78),)
	$(Q)$(RM) $(BOBJ_78)
endif
ifneq ($(BOBJ_79),)
	$(Q)$(RM) $(BOBJ_79)
endif
ifneq ($(BOBJ_80),)
	$(Q)$(RM) $(BOBJ_80)
endif
	$(Q)$(RM) $(BLDDIR)/.tree $(BLIBNAME)
# End of chunk library mode
endif

cleanlibs::
	@$(ECHO) Cleaning $(notdir $(BLIBNAME))
	$(Q)$(RM) $(BLIBNAME)

cleanall:: clean

include $(SDK)/make/depend.mk

endif # LIBSUM
