/*
 *         
 * 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <phymod/phymod.h>
#include <phymod/phymod_diagnostics.h>
#include <phymod/phymod_diagnostics_dispatch.h>

#ifdef PHYMOD_SESTO_SUPPORT
#include "../tier1/sesto_cfg_seq.h"
#include "../tier1/sesto_diag_seq.h"

int sesto_phy_rx_slicer_position_set(const phymod_phy_access_t* phy, uint32_t flags, const phymod_slicer_position_t* position)
{
        
    
    /* Place your code here */

        
    return PHYMOD_E_UNAVAIL;
    
}

int sesto_phy_rx_slicer_position_get(const phymod_phy_access_t* phy, uint32_t flags, phymod_slicer_position_t* position)
{
        
    
    /* Place your code here */

        
    return PHYMOD_E_UNAVAIL;
    
}


int sesto_phy_rx_slicer_position_max_get(const phymod_phy_access_t* phy, uint32_t flags, const phymod_slicer_position_t* position_min, const phymod_slicer_position_t* position_max)
{
        
    
    /* Place your code here */

        
    return PHYMOD_E_UNAVAIL;
    
}


int sesto_phy_prbs_config_set(const phymod_phy_access_t* phy, uint32_t flags , const phymod_prbs_t* prbs)
{
    return (_sesto_phy_prbs_config_set (phy, flags, prbs)); 
}

int sesto_phy_prbs_config_get(const phymod_phy_access_t* phy, uint32_t flags , phymod_prbs_t* prbs)
{
    return (_sesto_phy_prbs_config_get(phy, flags, prbs)); 
}


int sesto_phy_prbs_enable_set(const phymod_phy_access_t* phy, uint32_t flags , uint32_t enable)
{
    return _sesto_phy_prbs_enable_set(phy, flags, enable);
}

int sesto_phy_prbs_enable_get(const phymod_phy_access_t* phy, uint32_t flags , uint32_t* enable)
{
    return _sesto_phy_prbs_enable_get(phy, flags, enable);
}


int sesto_phy_prbs_status_get(const phymod_phy_access_t* phy, uint32_t flags, phymod_prbs_status_t* prbs_status)
{
    return _sesto_phy_prbs_status_get(phy, flags, prbs_status);
}


int sesto_phy_pattern_config_set(const phymod_phy_access_t* phy, const phymod_pattern_t* pattern)
{
        
    
    /* Place your code here */

        
    return PHYMOD_E_UNAVAIL;
    
}

int sesto_phy_pattern_config_get(const phymod_phy_access_t* phy, phymod_pattern_t* pattern)
{
        
    
    /* Place your code here */

        
    return PHYMOD_E_UNAVAIL;
    
}


int sesto_phy_pattern_enable_set(const phymod_phy_access_t* phy, uint32_t enable, const phymod_pattern_t* pattern)
{
        
    
    /* Place your code here */

        
    return PHYMOD_E_UNAVAIL;
    
}

int sesto_phy_pattern_enable_get(const phymod_phy_access_t* phy, uint32_t* enable)
{
        
    
    /* Place your code here */

        
    return PHYMOD_E_UNAVAIL;
    
}


int sesto_core_diagnostics_get(const phymod_core_access_t* core, phymod_core_diagnostics_t* diag)
{
    return _sesto_core_diagnostics_get(core, diag);
}


int sesto_phy_diagnostics_get(const phymod_phy_access_t* phy, phymod_phy_diagnostics_t* diag)
{
    return _sesto_phy_diagnostics_get(phy, diag);
}

int sesto_phy_eyescan_run (const phymod_phy_access_t* phy, uint32_t flags, phymod_eyescan_mode_t mode, const phymod_phy_eyescan_options_t* eyescan_options)
{
    
    if (PHYMOD_EYESCAN_F_DONE_GET(flags)) {
        switch(mode) {
        case phymodEyescanModeFast:
          return _sesto_phy_display_eyescan(phy);
        case phymodEyescanModeBERProj:
          return _sesto_ber_proj(phy, eyescan_options);
        default:
          return _sesto_phy_display_eyescan(phy);
          break;
        }
    } else {
        return PHYMOD_E_NONE;
    }

}

/**  FC/PCS checker enable set
 *   This function is used to enable or disable FC/PCS checker
 *
 *    @param phy                Pointer to phymod phy access structure
 *    @param fcpcs_chkr_mode    FC/PCS checker mode
 *    @param enable             Enable or disable
 *        1 -  enable
 *        0 -  disable
 *
 *    @return PHYMOD_E_NONE     successful function execution
 */
int sesto_phy_link_mon_enable_set(const phymod_phy_access_t* phy, phymod_link_monitor_mode_t fcpcs_chkr_mode, uint32_t enable)
{
    return _sesto_pcs_link_monitor_enable_set(phy, enable);
}
/**  FC/PCS checker enable get
 *   This function is used to check whether or not FC/PCS checker enabled
 *
 *    @param phy                Pointer to phymod phy access structure
 *    @param fcpcs_chkr_mode    FC/PCS checker mode
 *    @param enable             Enable or disable
 *        1 -  enable
 *        0 -  disable
 *
 *    @return PHYMOD_E_NONE     successful function execution
 */
int sesto_phy_link_mon_enable_get(const phymod_phy_access_t* phy, phymod_link_monitor_mode_t fcpcs_chkr_mode, uint32_t *enable)
{
    return _sesto_pcs_link_monitor_enable_get(phy, enable);
}
/**  FC/PCS checker status get
 *   This function is used to lock, loss of lock, error count status of the FC/PCS checker
 *
 *    @param phy                Pointer to phymod phy access structure
 *    @param lock_status        Live lock status
 *        1 - Locked
 *        0 - Not locked
 *    @param lock_lost_lh       Loss of lock
 *        1 -  Loss of lock happened
 *        0 -  No Loss of lock happended
 *    @param error_count        Error count
 *
 *    @return PHYMOD_E_NONE     successful function execution
 */
int sesto_phy_link_mon_status_get(const phymod_phy_access_t* phy, uint32_t* lock_status, uint32_t* lock_lost_lh, uint32_t* error_count)
{
    return _sesto_get_pcs_link_status(phy, lock_status, lock_lost_lh, error_count);
}

int sesto_phy_pmd_info_dump(const phymod_phy_access_t* phy, const char* type)
{
    return _sesto_phy_status_dump(phy);
}

#endif /* PHYMOD_SESTO_SUPPORT */
