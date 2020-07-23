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
#include <phymod/phymod_util.h>
#include "../tier1/viper_inc.h"

#ifdef PHYMOD_VIPER_SUPPORT

/*phymod, internal enum mappings*/
STATIC
int _viper_prbs_poly_phymod_to_viper(phymod_prbs_poly_t phymod_poly, viper_prbs_poly_t *viper_poly)
{
    switch(phymod_poly){
    case phymodPrbsPoly7:
        *viper_poly = VIPER_PRBS_POLYNOMIAL_7;
        break;
    case phymodPrbsPoly15:
        *viper_poly = VIPER_PRBS_POLYNOMIAL_15;
        break;
    case phymodPrbsPoly23:
        *viper_poly = VIPER_PRBS_POLYNOMIAL_23;
        break;
    case phymodPrbsPoly31:
        *viper_poly = VIPER_PRBS_POLYNOMIAL_31;
        break;
    case phymodPrbsPoly9:
        *viper_poly = VIPER_PRBS_POLYNOMIAL_9;
        break;
    case phymodPrbsPoly10:
        *viper_poly = VIPER_PRBS_POLYNOMIAL_10;
        break;
    case phymodPrbsPoly11:
        *viper_poly = VIPER_PRBS_POLYNOMIAL_11;
        break;
    default:
        PHYMOD_RETURN_WITH_ERR(PHYMOD_E_PARAM, (_PHYMOD_MSG("unsupported poly for viper %u"), phymod_poly));
    }
    return PHYMOD_E_NONE;
}

STATIC
int _viper_prbs_poly_viper_to_phymod(viper_prbs_poly_t viper_poly, phymod_prbs_poly_t *phymod_poly)
{
    switch(viper_poly){
    case VIPER_PRBS_POLYNOMIAL_7:
        *phymod_poly = phymodPrbsPoly7;
        break;
    case VIPER_PRBS_POLYNOMIAL_15:
        *phymod_poly = phymodPrbsPoly15;
        break;
    case VIPER_PRBS_POLYNOMIAL_23:
        *phymod_poly = phymodPrbsPoly23;
        break;
    case VIPER_PRBS_POLYNOMIAL_31:
        *phymod_poly = phymodPrbsPoly31;
         break;
    case VIPER_PRBS_POLYNOMIAL_9:
        *phymod_poly = phymodPrbsPoly9;
        break;
    case VIPER_PRBS_POLYNOMIAL_10:
        *phymod_poly = phymodPrbsPoly10;
        break;
    case VIPER_PRBS_POLYNOMIAL_11:
        *phymod_poly = phymodPrbsPoly11;
        break;
    default:
        PHYMOD_RETURN_WITH_ERR(PHYMOD_E_INTERNAL, (_PHYMOD_MSG("uknown poly %u"), viper_poly));
    }
    return PHYMOD_E_NONE;
}



int viper_phy_rx_slicer_position_set(const phymod_phy_access_t* phy, uint32_t flags, const phymod_slicer_position_t* position)
{
        
    
    /* Place your code here */

        
    return PHYMOD_E_NONE;
    
}

int viper_phy_rx_slicer_position_get(const phymod_phy_access_t* phy, uint32_t flags, phymod_slicer_position_t* position)
{
        
    
    /* Place your code here */

        
    return PHYMOD_E_NONE;
    
}


int viper_phy_rx_slicer_position_max_get(const phymod_phy_access_t* phy, uint32_t flags, const phymod_slicer_position_t* position_min, const phymod_slicer_position_t* position_max)
{
        
    
    /* Place your code here */

        
    return PHYMOD_E_NONE;
    
}


int viper_phy_prbs_config_set(const phymod_phy_access_t* phy, uint32_t flags , const phymod_prbs_t* prbs)
{
    phymod_phy_access_t phy_copy;
    viper_prbs_poly_t viper_poly;
    int start_lane, num_lane;
    int i = 0;

    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));

    /*next figure out the lane num and start_lane based on the input*/
    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(&phy->access, &start_lane, &num_lane));

    PHYMOD_IF_ERR_RETURN(_viper_prbs_poly_phymod_to_viper(prbs->poly, &viper_poly));

    for (i = 0; i < num_lane; i++) {
         phy_copy.access.lane_mask = 0x1 << (i + start_lane);
         PHYMOD_IF_ERR_RETURN(viper_prbs_lane_inv_data_set(&phy_copy.access, num_lane, prbs->invert));
         PHYMOD_IF_ERR_RETURN(viper_prbs_lane_poly_set(&phy_copy.access, num_lane, viper_poly));
    }

    return PHYMOD_E_NONE;
    
}

int viper_phy_prbs_config_get(const phymod_phy_access_t* phy, uint32_t flags , phymod_prbs_t* prbs)
{
    phymod_phy_access_t phy_copy;
    phymod_prbs_t config_tmp;
    viper_prbs_poly_t viper_poly;
    int start_lane, num_lane;
    int i = 0;

    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));

    /*next figure out the lane num and start_lane based on the input*/
    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(&phy->access, &start_lane, &num_lane));

    for (i = 0; i < num_lane; i++) {
        PHYMOD_IF_ERR_RETURN(viper_prbs_lane_inv_data_get(&phy_copy.access, num_lane ,&config_tmp.invert));
        PHYMOD_IF_ERR_RETURN(viper_prbs_lane_poly_get(&phy_copy.access, num_lane, &viper_poly));
        PHYMOD_IF_ERR_RETURN(_viper_prbs_poly_viper_to_phymod(viper_poly, &config_tmp.poly));
        prbs->invert = config_tmp.invert;
        prbs->poly = config_tmp.poly;
    }
    return PHYMOD_E_NONE;
    
}


int viper_phy_prbs_enable_set(const phymod_phy_access_t* phy, uint32_t flags , uint32_t enable)
{
    phymod_phy_access_t phy_copy;
    int start_lane, num_lane;
    int i = 0;

    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));

    /*next figure out the lane num and start_lane based on the input*/
    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(&phy->access, &start_lane, &num_lane));

    for (i = 0; i < num_lane; i++) {
        PHYMOD_IF_ERR_RETURN(viper_prbs_enable_set(&phy_copy.access, num_lane, enable));
    }
    return PHYMOD_E_NONE;
    
}

int viper_phy_prbs_enable_get(const phymod_phy_access_t* phy, uint32_t flags , uint32_t* enable)
{
    phymod_phy_access_t phy_copy;
    uint32_t enable_tmp;
    int start_lane, num_lane;
    int i = 0;

    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));

    /*next figure out the lane num and start_lane based on the input*/
    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(&phy->access, &start_lane, &num_lane));

    for (i = 0; i < num_lane; i++) {
        PHYMOD_IF_ERR_RETURN(viper_prbs_enable_get(&phy_copy.access, num_lane, &enable_tmp));
        *enable = enable_tmp;
    }
    return PHYMOD_E_NONE;
    
}


int viper_phy_prbs_status_get(const phymod_phy_access_t* phy, uint32_t flags, phymod_prbs_status_t* prbs_status)
{
    viper_prbs_status_t status;
    int i, start_lane, num_lane;
    phymod_phy_access_t phy_copy;

    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));
    /* next figure out the lane num and start_lane based on the input */
    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(&phy->access, &start_lane, &num_lane));

    for (i = 0; i < num_lane; i++) {
        phy_copy.access.lane_mask = 0x1 << (i + start_lane);
        PHYMOD_IF_ERR_RETURN(viper_prbs_status_get(&phy_copy.access, &status));
        prbs_status->prbs_lock = status.prbs_lock; 
        prbs_status->prbs_lock_loss = status.prbs_lock_loss; 
        prbs_status->error_count = status.error_count; 
    }
 
    return PHYMOD_E_NONE;
}


int viper_phy_pattern_config_set(const phymod_phy_access_t* phy, const phymod_pattern_t* pattern)
{
        
    
    /* Place your code here */

        
    return PHYMOD_E_NONE;
    
}

int viper_phy_pattern_config_get(const phymod_phy_access_t* phy, phymod_pattern_t* pattern)
{
        
    
    /* Place your code here */

        
    return PHYMOD_E_NONE;
    
}


int viper_phy_pattern_enable_set(const phymod_phy_access_t* phy, uint32_t enable, const phymod_pattern_t* pattern)
{
        
    
    /* Place your code here */

        
    return PHYMOD_E_NONE;
    
}

int viper_phy_pattern_enable_get(const phymod_phy_access_t* phy, uint32_t* enable)
{
        
    
    /* Place your code here */

        
    return PHYMOD_E_NONE;
    
}


int viper_core_diagnostics_get(const phymod_core_access_t* core, phymod_core_diagnostics_t* diag)
{
        
    
    /* Place your code here */

        
    return PHYMOD_E_NONE;
    
}


int viper_phy_diagnostics_get(const phymod_phy_access_t* phy, phymod_phy_diagnostics_t* diag)
{
        
    
    /* Place your code here */

        
    return PHYMOD_E_NONE;
    
}


int viper_phy_pmd_info_dump(const phymod_phy_access_t* phy, const char* type)
{
    return PHYMOD_E_NONE;

}


int viper_phy_pcs_info_dump(const phymod_phy_access_t* phy, const char* type)
{

    phymod_phy_access_t phy_copy;

    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));
    PHYMOD_IF_ERR_RETURN(viper_diag_disp(&phy_copy.access, (char*)type));

    return PHYMOD_E_NONE;

}


#endif /* PHYMOD_VIPER_SUPPORT */
