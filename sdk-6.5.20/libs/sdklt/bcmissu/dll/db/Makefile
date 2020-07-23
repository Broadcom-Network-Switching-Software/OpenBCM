#
# This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
# 
# Copyright 2007-2020 Broadcom Inc. All rights reserved.
#

include $(SDK)/make/libname.mk

# There could be multiple versions sub directories within the ISSU DB
# but we want to build only those that are between the start and current
# version.
$(eval SUBDIRS := $(shell python $(SDK)/bcmha/scripts/get_dir_list.py -v $(SDK_VERSION) -s $(START_SDK_VER)))
SUBDIRS += base
VPATH += $(SUBDIRS)
VSRCS += $(wildcard $(addsuffix /*.c,$(VPATH)))
VSRCS += $(wildcard *.c *.cpp *.cc)
LSRCS = $(sort $(notdir $(VSRCS)))
LOBJS = $(addsuffix .$(OBJSUFFIX), $(basename $(LSRCS)))
BOBJS = $(addprefix $(BLDDIR)/,$(LOBJS))

include $(SDK)/make/lib.mk

#include $(SDK)/make/sublib.mk
