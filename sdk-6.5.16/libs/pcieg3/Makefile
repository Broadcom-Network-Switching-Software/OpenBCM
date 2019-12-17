#
# 
# This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
# 
# Copyright 2007-2019 Broadcom Inc. All rights reserved.
#
LOCALDIR = libs/pcieg3

ifndef PCIEG3
PCIEG3 = $(CURDIR)
endif

include ${PCIEG3}/make/config.mk

# merlin16 pcieg3 support
VPATH += diag
VPATH += merlin16
VPATH += merlin16/src
VSRCS += $(wildcard merlin16/src/*.c) $(wildcard diag/*.c)

LSRCS = $(notdir $(VSRCS))
LOBJS = $(addsuffix .o, $(basename $(LSRCS)))
BOBJS = $(addprefix $(BLDDIR)/,$(LOBJS))

LIBNAME = libpcieg3
include ${PCIEG3}/make/lib.mk

include ${PCIEG3}/make/depend.mk
