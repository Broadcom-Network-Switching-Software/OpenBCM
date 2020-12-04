/*
 *         
 * 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <phymod/phymod_diagnostics.h>
#include <phymod/phymod_diagnostics_dispatch.h>


#ifdef PHYMOD_FURIA_SUPPORT

#include "../../furia/tier1/furia_types.h"
#include "../../furia/tier1/furia_cfg_seq.h"
/**   Convert PHYMOD poly to falcon poly type     
 *    This function converts PHYMOD polynomial type to falcon polynomial type  
 *
 *    @param phymod_poly        phymod polynomial type
 *    @param falcon_poly        falcon polynomial type
 *
 *    @return PHYMOD_E_NONE     successful function execution 
 */
STATIC
int _furia_prbs_poly_phymod_to_falcon(phymod_prbs_poly_t phymod_poly,
                                      enum srds_prbs_polynomial_enum *falcon_poly)
{
    switch(phymod_poly) {
        case phymodPrbsPoly7:
            *falcon_poly= PRBS7;
        break;
        case phymodPrbsPoly9:
            *falcon_poly= PRBS9;
        break;
        case phymodPrbsPoly11:
            *falcon_poly= PRBS11;
        break;
        case phymodPrbsPoly15:
            *falcon_poly= PRBS15;
        break;
        case phymodPrbsPoly23:
            *falcon_poly= PRBS23;
        break;
        case phymodPrbsPoly31:
            *falcon_poly= PRBS31;
        break;
        case phymodPrbsPoly58:
            *falcon_poly= PRBS58;
        break;
        default:
            PHYMOD_RETURN_WITH_ERR
                (PHYMOD_E_PARAM,\
                 (_PHYMOD_MSG("unsupported poly for furia")));
        break;
    }
    return PHYMOD_E_NONE;
}

/**   Convert falcon poly to PHYMOD poly type     
 *    This function converts falcon polynomial type to PHYMOD polynomial type  
 *
 *    @param falcon_poly        falcon polynomial type
 *    @param phymod_poly        phymod polynomial type
 *
 *    @return PHYMOD_E_NONE     successful function execution 
 */
STATIC
int _furia_prbs_poly_falcon_to_phymod(enum srds_prbs_polynomial_enum falcon_poly,
                                      phymod_prbs_poly_t *phymod_poly)
{
    switch(falcon_poly) {
        case PRBS7:
            *phymod_poly = phymodPrbsPoly7;
        break;
        case PRBS9:
            *phymod_poly = phymodPrbsPoly9;
        break;
        case PRBS11:
            *phymod_poly = phymodPrbsPoly11;
        break;
        case PRBS15:
            *phymod_poly = phymodPrbsPoly15;
        break;
        case PRBS23:
            *phymod_poly = phymodPrbsPoly23;
        break;
        case PRBS31:
            *phymod_poly = phymodPrbsPoly31;
        break;
        case PRBS58:
            *phymod_poly = phymodPrbsPoly58;
        break;
        default:
            PHYMOD_RETURN_WITH_ERR
                (PHYMOD_E_INTERNAL,\
                 (_PHYMOD_MSG("uknown poly")));
        break;
    }
    return PHYMOD_E_NONE;
}

/**   Set PRBS configuration 
 *    This function configures PRBS generator and checker with user provided
 *    PRBS configuration such as type of polynomial, invert data 
 *
 *    @param phy                Pointer to phymod phy access structure 
 *    @param flags              Currently unused and reserved for future use
 *    @param prbs               Pointer to phymod prbs structure which has
 *                              user provide polynomial and invert data info 
 *
 *    @return PHYMOD_E_NONE     successful function execution 
 */
int furia_phy_prbs_config_set(const phymod_phy_access_t* phy, uint32_t flags,
                              const phymod_prbs_t* prbs)
{
    enum srds_prbs_polynomial_enum falcon_poly;
    PHYMOD_IF_ERR_RETURN
        (_furia_prbs_poly_phymod_to_falcon(prbs->poly, &falcon_poly));
    PHYMOD_IF_ERR_RETURN
        (furia_prbs_config_set(&phy->access, flags, falcon_poly, prbs->invert));
    return PHYMOD_E_NONE;
}

/**   Get PRBS configuration 
 *    This function retrieves PRBS generator and checker configuration (type
 *    of polynomial and invert data from furia device 
 *
 *    @param phy                Pointer to phymod phy access structure 
 *    @param flags              Currently unused and reserved for future use
 *    @param prbs               Pointer to phymod prbs structure 
 *                              which will be populated with polynomial
 *                              and invert data info 
 *
 *    @return PHYMOD_E_NONE     successful function execution 
 */
int furia_phy_prbs_config_get(const phymod_phy_access_t* phy, uint32_t flags,
                              phymod_prbs_t* prbs)
{
    phymod_prbs_t config_tmp;
    enum srds_prbs_polynomial_enum falcon_poly;
    PHYMOD_IF_ERR_RETURN
        (furia_prbs_config_get(&phy->access, flags, &falcon_poly, &config_tmp.invert));
    PHYMOD_IF_ERR_RETURN
        (_furia_prbs_poly_falcon_to_phymod(falcon_poly, &config_tmp.poly));
    prbs->invert = config_tmp.invert;
    prbs->poly = config_tmp.poly;
    return PHYMOD_E_NONE;
}

/**   Enable PRBS 
 *    This function enables PRBS checker and generator of furia device 
 *
 *    @param phy                Pointer to phymod phy access structure 
 *    @param flags              Currently unused and reserved for future use
 *    @param enable             Enable or Disable supplied by user 
 *
 *    @return PHYMOD_E_NONE     successful function execution 
 */
int furia_phy_prbs_enable_set(const phymod_phy_access_t* phy, uint32_t flags,
                              uint32_t enable)
{
    PHYMOD_IF_ERR_RETURN
        (furia_prbs_enable_set(&phy->access, flags, enable));
    return PHYMOD_E_NONE;   
}

/**   PRBS Enable get 
 *    This function retreives the status of PRBS checker and generator of 
 *    furia device i.e.., whether or not enabled 
 *
 *    @param phy                Pointer to phymod phy access structure 
 *    @param flags              Currently unused and reserved for future use
 *    @param enable             Enable or Disable read from the device 
 *
 *    @return PHYMOD_E_NONE     successful function execution 
 */
int furia_phy_prbs_enable_get(const phymod_phy_access_t* phy, uint32_t flags,
                              uint32_t *enable)
{
    PHYMOD_IF_ERR_RETURN
        (furia_prbs_enable_get(&phy->access, flags, enable));
    return PHYMOD_E_NONE;
}

/**   Get PRBS lock and error status 
 *    This function retreives the status of PRBS lock, loss of lock and
 *    PRBS error count of the furia device 
 *
 *    @param phy                Pointer to phymod phy access structure 
 *    @param flags              Currently unused and reserved for future use
 *    @param prbs_status        PRBS status structure will be populated with 
 *                              PRBS lock, loss of lock and error count 
 *
 *    @return PHYMOD_E_NONE     successful function execution 
 */
int furia_phy_prbs_status_get(const phymod_phy_access_t* phy, uint32_t flags,
                              phymod_prbs_status_t* prbs_status)
{
    PHYMOD_IF_ERR_RETURN
        (furia_prbs_status_get(&phy->access,
                                    &prbs_status->prbs_lock,
                                    &prbs_status->prbs_lock_loss,
                                    &prbs_status->error_count)); 
    return PHYMOD_E_NONE;
}

/**   Display eyescan  
 *    This function used to display eye diagram from given lane 
 *
 *    @param phy                Pointer to phymod phy access structure 
 *
 *    @return PHYMOD_E_NONE     successful function execution 
 */
int furia_phy_eyescan_run( const phymod_phy_access_t *phy, uint32_t flags,
                           phymod_eyescan_mode_t mode, const phymod_phy_eyescan_options_t* eyescan_options)
{
    /* Added condition to solve printing eye trice from PHYMOD layer*/
    if (PHYMOD_EYESCAN_F_DONE_GET(flags)) {
        switch(mode) {
        case phymodEyescanModeFast:
          return furia_display_eye_scan(&phy->access);
          break;
        case phymodEyescanModeBERProj:
          return furia_ber_proj(&phy->access, eyescan_options);
          break;
        default:
          return furia_display_eye_scan(&phy->access);
          break;
        }
        
    } else {
        return PHYMOD_E_NONE;
    }
}
/**   Diagnostics get 
 *    This function used to get the diagnostics of a given lane 
 *
 *    @param phy                Pointer to phymod phy access structure
 *    @param diag               Diagnostics attributes 
 *
 *    @return PHYMOD_E_NONE     successful function execution 
 */
int furia_phy_diagnostics_get(const phymod_phy_access_t* phy, phymod_phy_diagnostics_t* diag)
{
    return _furia_phy_diagnostics_get(&phy->access, diag);
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
int furia_phy_link_mon_enable_set(const phymod_phy_access_t* phy, phymod_link_monitor_mode_t fcpcs_chkr_mode, uint32_t enable)
{
    return furia_fc_pcs_chkr_enable_set(&phy->access, fcpcs_chkr_mode, enable);
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
int furia_phy_link_mon_enable_get(const phymod_phy_access_t* phy, phymod_link_monitor_mode_t fcpcs_chkr_mode, uint32_t *enable)
{
    return furia_fc_pcs_chkr_enable_get(&phy->access, fcpcs_chkr_mode, enable);
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
int furia_phy_link_mon_status_get(const phymod_phy_access_t* phy, uint32_t* lock_status, uint32_t* lock_lost_lh, uint32_t* error_count)
{
    return furia_fc_pcs_chkr_status_get(&phy->access, lock_status, lock_lost_lh, error_count);
}

int furia_phy_pmd_info_dump(const phymod_phy_access_t* phy, const char* type)
{
    return _furia_phy_status_dump(&phy->access);
}
#endif /* PHYMOD_FURIA_SUPPORT */
