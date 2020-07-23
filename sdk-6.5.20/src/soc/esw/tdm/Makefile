#
# $Id: Makefile,v 1.9 2012/09/10 01:47:11 srajesh Exp $
# This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
# 
# Copyright 2007-2020 Broadcom Inc. All rights reserved.
#
# Makefile for esw subdirectory
#

LOCALDIR = src/soc/esw/tdm

include ${SDK}/make/Make.config

ifdef BCM_56850_A0
SUBDIR_TDM_CORE = core
SUBDIR_TDM_TRIDENT2P = trident2p
endif

ifdef BCM_56860_A0
SUBDIR_TDM_CORE = core
SUBDIR_TDM_TRIDENT2P = trident2p
endif

ifdef BCM_56560_A0
SUBDIR_TDM_CORE = core
SUBDIR_TDM_APACHE = apache
SUBDIR_TDM_TRIDENT2P = trident2p
endif

ifdef BCM_56960_A0
ifneq (,$(findstring BCM_56960_A0,$(ESW_CHIP_NAMES)))
SUBDIR_TDM_CORE = core
SUBDIR_TDM_TRIDENT2P = trident2p
SUBDIR_TDM_TOMAHAWK = tomahawk
endif
endif

ifdef BCM_56670_A0
SUBDIR_TDM_CORE = core
SUBDIR_TDM_MONTEREY = monterey
SUBDIR_TDM_APACHE = apache
SUBDIR_TDM_TRIDENT2P = trident2p
endif

ifdef BCM_56870_A0
SUBDIR_TDM_CORE = core
SUBDIR_TDM_TRIDENT3 = trident3
SUBDIR_TDM_TRIDENT2P = trident2p
SUBDIR_TDM_TOMAHAWK = tomahawk
endif

ifdef BCM_56370_A0
SUBDIR_TDM_CORE = core
SUBDIR_TDM_HELIX5 = helix5
SUBDIR_TDM_TRIDENT3 = trident3
SUBDIR_TDM_TRIDENT2P = trident2p
SUBDIR_TDM_TOMAHAWK = tomahawk
endif

ifdef BCM_56470_A0
SUBDIR_TDM_CORE = core
SUBDIR_TDM_HELIX5 = helix5
SUBDIR_TDM_HURRICANE4 = hurricane4
SUBDIR_TDM_FIREBOLT6 = firebolt6
SUBDIR_TDM_TRIDENT3 = trident3
SUBDIR_TDM_TRIDENT2P = trident2p
SUBDIR_TDM_TOMAHAWK = tomahawk
endif

ifdef BCM_56275_A0
SUBDIR_TDM_CORE = core
SUBDIR_TDM_HELIX5 = helix5
SUBDIR_TDM_HURRICANE4 = hurricane4
SUBDIR_TDM_TRIDENT3 = trident3
SUBDIR_TDM_TRIDENT2P = trident2p
SUBDIR_TDM_TOMAHAWK = tomahawk
endif

ifdef BCM_56965_A0
SUBDIR_TDM_CORE = core
SUBDIR_TDM_TRIDENT2P = trident2p
SUBDIR_TDM_TOMAHAWK = tomahawk
endif

ifdef BCM_56970_A0
SUBDIR_TDM_CORE = core
SUBDIR_TDM_TRIDENT2P = trident2p
SUBDIR_TDM_TOMAHAWK = tomahawk
SUBDIR_TDM_TOMAHAWK2 = tomahawk2
endif

ifdef BCM_56770_A0
SUBDIR_TDM_CORE = core
SUBDIR_TDM_MAVERICK2 = maverick2
SUBDIR_TDM_TRIDENT3 = trident3
SUBDIR_TDM_TRIDENT2P = trident2p
SUBDIR_TDM_TOMAHAWK = tomahawk
endif

ifdef BCM_53570_A0
SUBDIR_TDM_CORE = core
SUBDIR_TDM_GREYHOUND2 = greyhound2
endif

ifdef BCM_56070_A0
SUBDIR_TDM_CORE = core
SUBDIR_TDM_GREYHOUND2 = greyhound2
SUBDIR_TDM_FIRELIGHT = firelight
endif

ifdef BCM_56980_A0
#TH2 dependencies in TH3
SUBDIR_TDM_CORE = core
SUBDIR_TDM_TRIDENT2P = trident2p
SUBDIR_TDM_TOMAHAWK = tomahawk
SUBDIR_TDM_TOMAHAWK2 = tomahawk2
endif

subdirs := $(SUBDIR_TDM_CORE) $(SUBDIR_TDM_TRIDENT2P) $(SUBDIR_TDM_TRIDENT2) $(SUBDIR_TDM_TOMAHAWK) $(SUBDIR_TDM_APACHE) $(SUBDIR_TDM_TOMAHAWK2) $(SUBDIR_TDM_TRIDENT3)  $(SUBDIR_TDM_HELIX5) $(SUBDIR_TDM_FIREBOLT6) $(SUBDIR_TDM_HURRICANE4)  $(SUBDIR_TDM_MAVERICK2) $(SUBDIR_TDM_GREYHOUND2) $(SUBDIR_TDM_FIRELIGHT) $(SUBDIR_TDM_MONTEREY)

include ${SDK}/make/Make.subdirs

lib = libsoc_tdm
include ${SDK}/make/Make.lib

include ${SDK}/make/Make.depend


