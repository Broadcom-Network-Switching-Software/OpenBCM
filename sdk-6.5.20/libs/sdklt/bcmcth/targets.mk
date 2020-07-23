#
# This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
# 
# Copyright 2007-2020 Broadcom Inc. All rights reserved.
#
# BCMCTH default targets.
# This file may be included from the application makefile as well.
#

BCMCTH_DEFAULT_TARGETS = \
	main \
	agm \
	ctr \
	dlb \
	ecn \
	l2 \
	l3 \
	lb \
	ldh \
	mirror \
	mon \
	trunk \
	port \
	vlan \
	tnl \
	meter \
	flexdigest \
        flex_qos \
	udf \
	oam \
	ts \
    pkt \
	device \
	chip \
	shared

ifndef BCMCTH_TARGETS
BCMCTH_TARGETS = $(BCMCTH_DEFAULT_TARGETS)
endif

BCMCTH_LIBNAMES = $(addprefix libbcmcth,$(BCMCTH_TARGETS))

# Default suffixes for object files
include $(SDK)/make/suffix.mk

BCMCTH_LIBS = $(addsuffix .$(SDK_LIBSUFFIX),$(BCMCTH_LIBNAMES))
