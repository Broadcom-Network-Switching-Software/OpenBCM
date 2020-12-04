/*
 *         
 * 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.  
 */
#ifndef __FURIA_82212_HELPER_H__
#define __FURIA_82212_HELPER_H__

/* 
 * Includes
 */
#include "../../furia/tier1/furia_pkg_cfg.h"

#define FURIA_ID0       0xae02   
#define FURIA_ID1       0x5230  
#define FURIA_82212_MAX_DIE  2
#define FURIA_82212_MAX_LANE 12
#define FURIA_82212_MAX_DIE_LANE 6
#define FURIA_82212_BCAST_LANEMASK 0xFFF

extern const FURIA_PKG_LANE_CFG_t glb_lanes_82212[];
#define FURIA_82212_GET_DIE_NO(idx) glb_lanes_82212[idx].die_addr

#define FURIA_82212_CALL_PHYDRV(_PACC,_LIDX,_FUN)       \
    _PACC.access.addr &= ~(1);                          \
    _PACC.access.addr |= FURIA_82212_GET_DIE_NO(_LIDX); \
    _PACC.access.lane_mask = (1 << _LIDX);              \
    PHYMOD_IF_ERR_RETURN(_FUN);                         \
    _PACC.access.lane_mask = 0;

#endif
