#
# $Id: Makefile,v 1.4 2012/03/08 10:43:54 assafz Exp $
# This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
# 
# Copyright 2007-2020 Broadcom Inc. All rights reserved.
#


LOCALDIR = src/bcm/dnx/algo

include ${SDK}/make/Make.config

#subdirs = algo
subdirs += res_mngr
subdirs += template_mngr
subdirs += l2
subdirs += l3
subdirs += lif_mngr
subdirs += port
subdirs += rx
subdirs += crps
subdirs += ipq
subdirs += egq
subdirs += sch
subdirs += cal
subdirs += failover
subdirs += multicast
subdirs += field
subdirs += port_pp
subdirs += sat
subdirs += stg
subdirs += qos
subdirs += rate
subdirs += bfd
subdirs += consistent_hashing
subdirs += mpls
subdirs += vlan
subdirs += trunk
subdirs += oam
subdirs += oamp
subdirs += ecgm
subdirs += lane_map
subdirs += tunnel
subdirs += stat_pp
subdirs += ptp
subdirs += ppmc
subdirs += instru
subdirs += synce
subdirs += flexe_general
subdirs += swstate
subdirs += flexe_core

include ${SDK}/make/Make.subdirs

lib = libbcm_dnxalgo

include ${SDK}/make/Make.lib

include ${SDK}/make/Make.depend

