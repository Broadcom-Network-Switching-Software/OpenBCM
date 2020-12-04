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


#ifdef PHYMOD_QUADRA28_SUPPORT

#include <phymod/chip/quadra28.h>

/* To be defined */
#include "../tier1/quadra28_types.h"
#include "../tier1/quadra28_cfg_seq.h"
#include "../tier1/quadra28_reg_access.h"
#include "../tier1/bcmi_quadra28_defs.h"


/**   Convert PHYMOD poly to falcon poly type
 *    This function converts PHYMOD polynomial type to falcon polynomial type
 *
 *    @param phymod_poly        phymod polynomial type
 *    @param falcon_poly        falcon polynomial type
 *
 *    @return PHYMOD_E_NONE     successful function execution
 */
STATIC
int _quadra28_prbs_poly_phymod_to_merlin(phymod_prbs_poly_t phymod_poly,
                                      enum srds_prbs_polynomial_enum *merlin_poly)
{
    switch(phymod_poly) {
        case phymodPrbsPoly7:
            *merlin_poly= SRDS_PRBS_7;
        break;
        case phymodPrbsPoly9:
            *merlin_poly= SRDS_PRBS_9;
        break;
        case phymodPrbsPoly11:
            *merlin_poly= SRDS_PRBS_11;
        break;
        case phymodPrbsPoly15:
            *merlin_poly= SRDS_PRBS_15;
        break;
        case phymodPrbsPoly23:
            *merlin_poly= SRDS_PRBS_23;
        break;
        case phymodPrbsPoly31:
            *merlin_poly= SRDS_PRBS_31;
        break;
        case phymodPrbsPoly58:
            *merlin_poly= SRDS_PRBS_58;
        break;
        default:
            PHYMOD_RETURN_WITH_ERR
                (PHYMOD_E_PARAM,\
                 (_PHYMOD_MSG("unsupported poly for q28")));
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
int _quadra28_prbs_poly_merlin_to_phymod(enum srds_prbs_polynomial_enum merlin_poly,
                                      phymod_prbs_poly_t *phymod_poly)
{
    switch(merlin_poly) {
        case SRDS_PRBS_7:
            *phymod_poly = phymodPrbsPoly7;
        break;
        case SRDS_PRBS_9:
            *phymod_poly = phymodPrbsPoly9;
        break;
        case SRDS_PRBS_11:
            *phymod_poly = phymodPrbsPoly11;
        break;
        case SRDS_PRBS_15:
            *phymod_poly = phymodPrbsPoly15;
        break;
        case SRDS_PRBS_23:
            *phymod_poly = phymodPrbsPoly23;
        break;
        case SRDS_PRBS_31:
            *phymod_poly = phymodPrbsPoly31;
        break;
        case SRDS_PRBS_58:
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

/** PRBS Polynomial Enum */
int quadra28_phy_prbs_config_set(const phymod_phy_access_t* phy, uint32_t flags , const phymod_prbs_t* prbs)
{
    enum srds_prbs_polynomial_enum merlin_poly;

    PHYMOD_IF_ERR_RETURN
        (_quadra28_prbs_poly_phymod_to_merlin(prbs->poly, &merlin_poly));
    PHYMOD_IF_ERR_RETURN(
        quara28_prbs_config_set(phy,
                        flags,
                        merlin_poly,
                        prbs->invert));
    return PHYMOD_E_NONE;

}

int quadra28_phy_prbs_config_get(const phymod_phy_access_t* phy, uint32_t flags , phymod_prbs_t* prbs)
{
    enum srds_prbs_polynomial_enum merlin_poly;

    PHYMOD_IF_ERR_RETURN(
        quadra28_prbs_config_get(phy,
                        flags,
                        &merlin_poly, &prbs->invert));
    PHYMOD_IF_ERR_RETURN
        (_quadra28_prbs_poly_merlin_to_phymod(merlin_poly, &prbs->poly));

    return PHYMOD_E_NONE;

}


int quadra28_phy_prbs_enable_set(const phymod_phy_access_t* phy, uint32_t flags , uint32_t enable)
{

    PHYMOD_IF_ERR_RETURN(
        quadra28_prbs_enable_set(phy, flags, enable));

    return PHYMOD_E_NONE;

}

int quadra28_phy_prbs_enable_get(const phymod_phy_access_t* phy, uint32_t flags , uint32_t* enable)
{
    PHYMOD_IF_ERR_RETURN(
        quadra28_prbs_enable_get(phy, flags, enable));

    return PHYMOD_E_NONE;

}


int quadra28_phy_prbs_status_get(const phymod_phy_access_t* phy, uint32_t flags, phymod_prbs_status_t* prbs_status)
{
    PHYMOD_IF_ERR_RETURN(
        quadra28_prbs_status_get(phy,
                             &prbs_status->prbs_lock,
                             &prbs_status->prbs_lock_loss,
                             &prbs_status->error_count));

    return PHYMOD_E_NONE;

}


int quadra28_phy_diagnostics_get(const phymod_phy_access_t* phy, phymod_phy_diagnostics_t* diag)
{
    PHYMOD_IF_ERR_RETURN(
      _quadra28_phy_diagnostics_get(phy, diag));
    return PHYMOD_E_NONE;

}


int quadra28_phy_eyescan_run(const phymod_phy_access_t* phy, uint32_t flags, phymod_eyescan_mode_t mode, const phymod_phy_eyescan_options_t* eyescan_options)
{
    /* Added condition to solve printing eye trice from PHYMOD layer*/
    if (PHYMOD_EYESCAN_F_DONE_GET(flags)) {
        switch(mode) {
        case phymodEyescanModeFast:
          return quadra28_display_eye_scan(phy);
          break;
        case phymodEyescanModeBERProj:
          return quadra28_ber_proj(phy, eyescan_options);
          break;
        default:
          return quadra28_display_eye_scan(phy);
          break;
        }

    } else {
        return PHYMOD_E_NONE;
    }

}

int quadra28_phy_link_mon_enable_set(const phymod_phy_access_t* phy, phymod_link_monitor_mode_t link_mon_mode, uint32_t enable)
{
    uint32_t retime_get = 0;

    PHYMOD_IF_ERR_RETURN(
            _quadra28_phy_retimer_enable_get(&phy->access, &retime_get));
    if (retime_get != 1) {
        return PHYMOD_E_CONFIG;
    }
    return PHYMOD_E_NONE;
}

int quadra28_phy_link_mon_enable_get(const phymod_phy_access_t* phy, phymod_link_monitor_mode_t link_mon_mode, uint32_t* enable)
{
    return _quadra28_phy_retimer_enable_get(&phy->access, enable);
}


int quadra28_phy_link_mon_status_get(const phymod_phy_access_t* phy, uint32_t* lock_status, uint32_t* lock_lost_lh, uint32_t* error_count)
{
    uint32_t acc_flags = 0;
    int pkg_side = 0;
    int num_lanes = QUADRA28_MAX_LANE;
    int lane_map = 0;
    int lane_index = 0;
    BCMI_QUADRA28_SIDE_SELECTIONr_t side_sel_reg_val;
    BCMI_QUADRA28_PCS_STATUS1r_t pcs_status1_reg_val;
    BCMI_QUADRA28_TENGBASE_R_PCS_STATUS_2r_t   pcs_status2_reg_val;
    BCMI_QUADRA28_ALARM_RAW_STATUS_1r_t   alarm_raw_status;
    phymod_access_t acc_cpy;
    int pmd_mode = 0;
    uint32_t speed = 0;
    uint32_t retimer_get = 0;

    PHYMOD_MEMSET(&side_sel_reg_val, 0, sizeof(BCMI_QUADRA28_SIDE_SELECTIONr_t));
    PHYMOD_MEMSET(&pcs_status1_reg_val, 0, sizeof(BCMI_QUADRA28_PCS_STATUS1r_t));
    PHYMOD_MEMCPY(&acc_cpy, &phy->access, sizeof(phymod_access_t));

    Q28_GET_PMD_MODE(&acc_cpy, pmd_mode);
    Q28_10G_PHY_ID_WORK_AROUND(speed, acc_cpy);

    acc_flags = PHYMOD_ACC_FLAGS(&acc_cpy);
    QUADRA28_GET_IF_SIDE(acc_flags, pkg_side);
    lane_map = PHYMOD_ACC_LANE_MASK(&acc_cpy);

    QUADRA28_SELECT_SIDE(&acc_cpy,side_sel_reg_val,pkg_side);
    PHYMOD_IF_ERR_RETURN(
            _quadra28_phy_retimer_enable_get(&phy->access, &retimer_get));
    if(retimer_get) {
        *lock_status = 1;
        *lock_lost_lh = 1;
        *error_count = 0xffff;
        if (pmd_mode == Q28_SINGLE_PMD) {
            for(lane_index = 0; lane_index < num_lanes; lane_index++) {
                if (((lane_map >> lane_index) & 1) == 0x1) {
                    /* select a lane */
                    PHYMOD_IF_ERR_RETURN(quadra28_channel_select(&acc_cpy, lane_index));
                    PHYMOD_IF_ERR_RETURN(BCMI_QUADRA28_READ_PCS_STATUS1r(&acc_cpy, &pcs_status1_reg_val));
                    *lock_status &= BCMI_QUADRA28_PCS_STATUS1r_PCS_RECEIVE_LINK_STATUSf_GET(pcs_status1_reg_val);
                }
            }
        } else {
            PHYMOD_IF_ERR_RETURN(BCMI_QUADRA28_READ_PCS_STATUS1r(&acc_cpy, &pcs_status1_reg_val));
            *lock_status &= BCMI_QUADRA28_PCS_STATUS1r_PCS_RECEIVE_LINK_STATUSf_GET(pcs_status1_reg_val);
        }
        PHYMOD_IF_ERR_RETURN(BCMI_QUADRA28_READ_TENGBASE_R_PCS_STATUS_2r(&acc_cpy, &pcs_status2_reg_val));
        *error_count &= BCMI_QUADRA28_TENGBASE_R_PCS_STATUS_2r_R64_ERRBLKCNTf_GET(pcs_status2_reg_val);
         PHYMOD_IF_ERR_RETURN(BCMI_QUADRA28_READ_ALARM_RAW_STATUS_1r(&acc_cpy,&alarm_raw_status));
        if(pkg_side == Q28_INTF_SYS) {
           *lock_lost_lh &= BCMI_QUADRA28_ALARM_RAW_STATUS_1r_RAW_STATUS_PMD_RX_LOCK_LOST_STATUS_SYSf_GET(alarm_raw_status);
        } else {
           *lock_lost_lh &= BCMI_QUADRA28_ALARM_RAW_STATUS_1r_RAW_STATUS_PMD_RX_LOCK_LOST_STATUS_LINEf_GET(alarm_raw_status);
        }
    } else {
        PHYMOD_DEBUG_ERROR((" PCS monitor is supported when retimer mode is enabled \n"));
        return PHYMOD_E_CONFIG;
    }
    /* restore Q28_INTF_LINE side */
    QUADRA28_RESET_SIDE_BCST_CTRL(&acc_cpy,side_sel_reg_val,Q28_INTF_LINE);
    return PHYMOD_E_NONE;

}

int quadra28_phy_pmd_info_dump(const phymod_phy_access_t* phy,const char* type)
{
    PHYMOD_IF_ERR_RETURN(
          _quadra28_phy_status_dump(phy));
    return PHYMOD_E_NONE;

}
#endif /* PHYMOD_QUADRA28_SUPPORT */
