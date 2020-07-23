#
# $Id: Makefile,v 1.9 2012/09/10 01:47:11 srajesh Exp $
# This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
# 
# Copyright 2007-2020 Broadcom Inc. All rights reserved.
#
# Makefile for esw subdirectory
#

LOCALDIR = src/soc/esw

include ${SDK}/make/Make.config

ifdef BCM_56640_A0
SUBDIR_TRIUMPH3 = triumph3
endif

ifdef BCM_56850_A0
SUBDIR_TDM = tdm
SUBDIR_TRIUMPH3 = triumph3
SUBDIR_TRIDENT2 = trident2
endif

ifdef BCM_56860_A0
SUBDIR_TDM = tdm
SUBDIR_TRIUMPH3 = triumph3
SUBDIR_TRIDENT2 = trident2
SUBDIR_TRIDENT2P = trident2p
endif

ifdef BCM_56340_A0
SUBDIR_TRIUMPH3 = triumph3
SUBDIR_TRIDENT2 = trident2
endif

ifdef BCM_56450_A0
SUBDIR_KATANA2 = katana2
SUBDIR_TRIUMPH3 = triumph3
endif

ifdef BCM_53400_A0
SUBDIR_GHREYHOUND = greyhound
endif

ifdef BCM_56960_A0
ifneq (,$(findstring BCM_56960_A0,$(ESW_CHIP_NAMES)))
SUBDIR_TDM = tdm
SUBDIR_TRIUMPH3 = triumph3
SUBDIR_TRIDENT2 = trident2
SUBDIR_TOMAHAWK = tomahawk
endif
endif

ifdef BCM_56965_A0
SUBDIR_TDM = tdm
SUBDIR_TRIUMPH3 = triumph3
SUBDIR_TRIDENT2 = trident2
SUBDIR_TOMAHAWK = tomahawk
endif

ifdef BCM_56160_A0
SUBDIR_GHREYHOUND = greyhound
SUBDIR_HURRICANE3 = hurricane3
endif

ifdef BCM_53540_A0
SUBDIR_GHREYHOUND = greyhound
SUBDIR_HURRICANE3 = hurricane3
endif

ifdef BCM_53570_A0
SUBDIR_TDM = tdm
SUBDIR_GHREYHOUND = greyhound
SUBDIR_HURRICANE3 = hurricane3
SUBDIR_GREYHOUND2 = greyhound2
endif

ifdef BCM_56070_A0
SUBDIR_TDM = tdm
SUBDIR_GHREYHOUND = greyhound
SUBDIR_HURRICANE3 = hurricane3
SUBDIR_GREYHOUND2 = greyhound2
SUBDIR_FIRELIGHT = firelight
endif

ifdef BCM_56260_A0
SUBDIR_SABER2 = saber2
endif

ifdef BCM_56970_A0
SUBDIR_TDM = tdm
SUBDIR_TRIUMPH3 = triumph3
SUBDIR_TRIDENT2 = trident2
SUBDIR_TOMAHAWK = tomahawk
SUBDIR_TOMAHAWK2 = tomahawk2
SUBDIR_FLEXPORT = flexport
endif

ifdef BCM_56270_A0
SUBDIR_METROLITE = metrolite
endif

ifdef BCM_56670_A0
SUBDIR_TDM = tdm
SUBDIR_MONTEREY =monterey 
SUBDIR_APACHE = apache
SUBDIR_TRIUMPH3 = triumph3
SUBDIR_TRIDENT2 = trident2
SUBDIR_TRIDENT2P = trident2p
endif

ifdef BCM_56560_A0
SUBDIR_TDM = tdm
SUBDIR_APACHE = apache
SUBDIR_TRIUMPH3 = triumph3
SUBDIR_TRIDENT2 = trident2
SUBDIR_TRIDENT2P = trident2p
endif

ifdef BCM_56980_A0
SUBDIR_TRIUMPH3 = triumph3
SUBDIR_TRIDENT2 = trident2
SUBDIR_TOMAHAWK = tomahawk
SUBDIR_TOMAHAWK3 = tomahawk3
# TH2 dependencies in TH3
SUBDIR_TDM = tdm
SUBDIR_TOMAHAWK2 = tomahawk2
SUBDIR_FLEXPORT = flexport
endif

ifdef BCM_56870_A0
SUBDIR_TDM = tdm
SUBDIR_TRIUMPH3 = triumph3
SUBDIR_TRIDENT2 = trident2
SUBDIR_TOMAHAWK = tomahawk
SUBDIR_TRIDENT3 = trident3
SUBDIR_TRIDENT2P = trident2p
SUBDIR_FLEXPORT = flexport
endif

ifdef BCM_56370_A0
SUBDIR_TDM = tdm
SUBDIR_TRIUMPH3 = triumph3
SUBDIR_TRIDENT2 = trident2
SUBDIR_TOMAHAWK = tomahawk
SUBDIR_TRIDENT3 = trident3
SUBDIR_HELIX5 = helix5
SUBDIR_TRIDENT2P = trident2p
SUBDIR_FLEXPORT = flexport
endif

ifdef BCM_56275_A0
SUBDIR_TDM = tdm
SUBDIR_TRIUMPH3 = triumph3
SUBDIR_TRIDENT2 = trident2
SUBDIR_TOMAHAWK = tomahawk
SUBDIR_TRIDENT3 = trident3
SUBDIR_HELIX5 = helix5
SUBDIR_HURRICANE4 = hurricane4
SUBDIR_TRIDENT2P = trident2p
SUBDIR_FLEXPORT = flexport
endif

ifdef BCM_56470_A0
SUBDIR_TDM = tdm
SUBDIR_TRIUMPH3 = triumph3
SUBDIR_TRIDENT2 = trident2
SUBDIR_TOMAHAWK = tomahawk
SUBDIR_TRIDENT3 = trident3
SUBDIR_HELIX5 = helix5
SUBDIR_HURRICANE4 = hurricane4
SUBDIR_FIREBOLT6 = firebolt6
SUBDIR_TRIDENT2P = trident2p
SUBDIR_FLEXPORT = flexport
endif

ifdef BCM_56770_A0
SUBDIR_TDM = tdm
SUBDIR_TRIUMPH3 = triumph3
SUBDIR_TRIDENT2 = trident2
SUBDIR_TOMAHAWK = tomahawk
SUBDIR_TRIDENT3 = trident3
SUBDIR_MAVERICK2 = maverick2
SUBDIR_TRIDENT2P = trident2p
SUBDIR_FLEXPORT = flexport
endif

subdirs := $(SUBDIR_TRIUMPH3) $(SUBDIR_TRIDENT2) $(SUBDIR_KATANA2) $(SUBDIR_TOMAHAWK) $(SUBDIR_GHREYHOUND) $(SUBDIR_TRIDENT2P) $(SUBDIR_TDM) $(SUBDIR_ALPM) $(SUBDIR_SABER2) $(SUBDIR_HURRICANE3) $(SUBDIR_APACHE) $(SUBDIR_METROLITE) $(SUBDIR_TOMAHAWK2) $(SUBDIR_FLEXPORT) $(SUBDIR_GREYHOUND2) $(SUBDIR_FIRELIGHT) $(SUBDIR_TRIDENT3) $(SUBDIR_HELIX5) $(SUBDIR_HURRICANE4) $(SUBDIR_FIREBOLT6) $(SUBDIR_MAVERICK2) $(SUBDIR_TOMAHAWK3) $(SUBDIR_MONTEREY)

include ${SDK}/make/Make.subdirs

lib = libsoc_esw
include ${SDK}/make/Make.lib

include ${SDK}/make/Make.depend


