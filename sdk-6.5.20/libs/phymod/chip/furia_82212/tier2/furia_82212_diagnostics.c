/*
 *         
 * 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <phymod/phymod.h>
#include <phymod/phymod_system.h>
#include <phymod/phymod_diagnostics.h>
#include <phymod/phymod_diagnostics_dispatch.h>

#ifdef PHYMOD_FURIA_SUPPORT

#include <phymod/chip/furia_diagnostics.h>
#include "furia_82212_helper.h"

int furia_82212_phy_prbs_config_set(const phymod_phy_access_t* phy, uint32_t flags , const phymod_prbs_t* prbs)
{
    phymod_phy_access_t phy_cpy;
    uint16_t lane_idx = 0;
    PHYMOD_MEMSET(&phy_cpy, 0, sizeof(phymod_phy_access_t));
    PHYMOD_MEMCPY(&phy_cpy, phy, sizeof(phymod_phy_access_t));
   
    for (lane_idx = 0; lane_idx < FURIA_82212_MAX_LANE; lane_idx ++) {
        if (phy->access.lane_mask & (1 << lane_idx)) {
            FURIA_82212_CALL_PHYDRV(phy_cpy, lane_idx,
                  furia_phy_prbs_config_set(&phy_cpy, flags, prbs));
        }
    }
    return PHYMOD_E_NONE;
    
}

int furia_82212_phy_prbs_config_get(const phymod_phy_access_t* phy, uint32_t flags , phymod_prbs_t* prbs)
{
    phymod_phy_access_t phy_cpy;
    uint16_t lane_idx = 0;
    PHYMOD_MEMSET(&phy_cpy, 0, sizeof(phymod_phy_access_t));
    PHYMOD_MEMCPY(&phy_cpy, phy, sizeof(phymod_phy_access_t));
   
    for (lane_idx = 0; lane_idx < FURIA_82212_MAX_LANE; lane_idx ++) {
        if (phy->access.lane_mask & (1 << lane_idx)) {
            FURIA_82212_CALL_PHYDRV(phy_cpy, lane_idx,
                  furia_phy_prbs_config_get(&phy_cpy, flags, prbs));
            break;
        }
    }
    return PHYMOD_E_NONE;
    
}


int furia_82212_phy_prbs_enable_set(const phymod_phy_access_t* phy, uint32_t flags , uint32_t enable)
{
    phymod_phy_access_t phy_cpy;
    uint16_t lane_idx = 0;
    PHYMOD_MEMSET(&phy_cpy, 0, sizeof(phymod_phy_access_t));
    PHYMOD_MEMCPY(&phy_cpy, phy, sizeof(phymod_phy_access_t));
   
    for (lane_idx = 0; lane_idx < FURIA_82212_MAX_LANE; lane_idx ++) {
        if (phy->access.lane_mask & (1 << lane_idx)) {
            FURIA_82212_CALL_PHYDRV(phy_cpy, lane_idx,
                  furia_phy_prbs_enable_set(&phy_cpy, flags, enable));
        }
    }
    return PHYMOD_E_NONE;
}

int furia_82212_phy_prbs_enable_get(const phymod_phy_access_t* phy, uint32_t flags , uint32_t* enable)
{
    phymod_phy_access_t phy_cpy;
    uint16_t lane_idx = 0;
    PHYMOD_MEMSET(&phy_cpy, 0, sizeof(phymod_phy_access_t));
    PHYMOD_MEMCPY(&phy_cpy, phy, sizeof(phymod_phy_access_t));
   
    for (lane_idx = 0; lane_idx < FURIA_82212_MAX_LANE; lane_idx ++) {
        if (phy->access.lane_mask & (1 << lane_idx)) {
            FURIA_82212_CALL_PHYDRV(phy_cpy, lane_idx,
                  furia_phy_prbs_enable_get(&phy_cpy, flags, enable));
            break;
        }
    }
        
    return PHYMOD_E_NONE;
    
}


int furia_82212_phy_prbs_status_get(const phymod_phy_access_t* phy, uint32_t flags, phymod_prbs_status_t* prbs_status)
{
    phymod_prbs_status_t status;
    phymod_phy_access_t phy_cpy;
    uint16_t lane_idx = 0;
    PHYMOD_MEMSET(&phy_cpy, 0, sizeof(phymod_phy_access_t));
    PHYMOD_MEMCPY(&phy_cpy, phy, sizeof(phymod_phy_access_t));
    prbs_status->prbs_lock = 1;
    prbs_status->prbs_lock_loss = 1;
    prbs_status->error_count = 0;

    for (lane_idx = 0; lane_idx < FURIA_82212_MAX_LANE; lane_idx ++) {
        if (phy->access.lane_mask & (1 << lane_idx)) {
            FURIA_82212_CALL_PHYDRV(phy_cpy, lane_idx,
                  furia_phy_prbs_status_get(&phy_cpy, flags, &status));
            prbs_status->prbs_lock &= status.prbs_lock;
            prbs_status->prbs_lock_loss &= status.prbs_lock_loss ;
            prbs_status->error_count |= status.error_count;
        }
    }
       
    return PHYMOD_E_NONE;
    
}


int furia_82212_phy_diagnostics_get(const phymod_phy_access_t* phy, phymod_phy_diagnostics_t* diag)
{
    phymod_phy_access_t phy_cpy;
    uint16_t lane_idx = 0;
    PHYMOD_MEMSET(&phy_cpy, 0, sizeof(phymod_phy_access_t));
    PHYMOD_MEMCPY(&phy_cpy, phy, sizeof(phymod_phy_access_t));
   
    for (lane_idx = 0; lane_idx < FURIA_82212_MAX_LANE; lane_idx ++) {
        if (phy->access.lane_mask & (1 << lane_idx)) {
            FURIA_82212_CALL_PHYDRV(phy_cpy, lane_idx,
                  furia_phy_diagnostics_get(&phy_cpy, diag));
            break;
        }
    }
       
    return PHYMOD_E_NONE;
}


int furia_82212_phy_eyescan_run(const phymod_phy_access_t* phy, uint32_t flags, phymod_eyescan_mode_t mode, const phymod_phy_eyescan_options_t* eyescan_options)
{
    phymod_phy_access_t phy_cpy;
    uint16_t lane_idx = 0;
    PHYMOD_MEMSET(&phy_cpy, 0, sizeof(phymod_phy_access_t));
    PHYMOD_MEMCPY(&phy_cpy, phy, sizeof(phymod_phy_access_t));
   
    for (lane_idx = 0; lane_idx < FURIA_82212_MAX_LANE; lane_idx ++) {
        if (phy->access.lane_mask & (1 << lane_idx)) {
            FURIA_82212_CALL_PHYDRV(phy_cpy, lane_idx,
                  furia_phy_eyescan_run(&phy_cpy, flags, mode, eyescan_options));
        }
    }
       
    return PHYMOD_E_NONE;
    
}


int furia_82212_phy_link_mon_enable_set(const phymod_phy_access_t* phy, phymod_link_monitor_mode_t link_mon_mode, uint32_t enable)
{
    phymod_phy_access_t phy_cpy;
    uint16_t lane_idx = 0;
    PHYMOD_MEMSET(&phy_cpy, 0, sizeof(phymod_phy_access_t));
    PHYMOD_MEMCPY(&phy_cpy, phy, sizeof(phymod_phy_access_t));
   
    for (lane_idx = 0; lane_idx < FURIA_82212_MAX_LANE; lane_idx ++) {
        if (phy->access.lane_mask & (1 << lane_idx)) {
            FURIA_82212_CALL_PHYDRV(phy_cpy, lane_idx,
                  furia_phy_link_mon_enable_set(&phy_cpy, link_mon_mode, enable));
        }
    }
       
    return PHYMOD_E_NONE;
    
}

int furia_82212_phy_link_mon_enable_get(const phymod_phy_access_t* phy, phymod_link_monitor_mode_t link_mon_mode, uint32_t* enable)
{
    phymod_phy_access_t phy_cpy;
    uint16_t lane_idx = 0;
    PHYMOD_MEMSET(&phy_cpy, 0, sizeof(phymod_phy_access_t));
    PHYMOD_MEMCPY(&phy_cpy, phy, sizeof(phymod_phy_access_t));
   
    for (lane_idx = 0; lane_idx < FURIA_82212_MAX_LANE; lane_idx ++) {
        if (phy->access.lane_mask & (1 << lane_idx)) {
            FURIA_82212_CALL_PHYDRV(phy_cpy, lane_idx,
                  furia_phy_link_mon_enable_get(&phy_cpy, link_mon_mode, enable));
            break;
        }
    }
        
    return PHYMOD_E_NONE;
    
}


int furia_82212_phy_link_mon_status_get(const phymod_phy_access_t* phy, uint32_t* lock_status, uint32_t* lock_lost_lh, uint32_t* error_count)
{
    phymod_phy_access_t phy_cpy;
    uint16_t lane_idx = 0;
    uint32_t lock= 1, lock_lost = 1, err_cnt = 0;
    PHYMOD_MEMSET(&phy_cpy, 0, sizeof(phymod_phy_access_t));
    PHYMOD_MEMCPY(&phy_cpy, phy, sizeof(phymod_phy_access_t));
    
    *lock_status = 0xFFFF;
    *lock_lost_lh = 0xFFFF;
    for (lane_idx = 0; lane_idx < FURIA_82212_MAX_LANE; lane_idx ++) {
        if (phy->access.lane_mask & (1 << lane_idx)) {
            FURIA_82212_CALL_PHYDRV(phy_cpy, lane_idx,
                  furia_phy_link_mon_status_get(&phy_cpy, &lock, &lock_lost, &err_cnt));
            *lock_status &= lock;
            *lock_lost_lh &= lock_lost;
            *error_count |= err_cnt;
        }
    }
       
        
    return PHYMOD_E_NONE;
    
}


#endif /* PHYMOD_FURIA_SUPPORT */
