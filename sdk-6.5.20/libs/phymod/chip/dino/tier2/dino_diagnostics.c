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

#ifdef PHYMOD_DINO_SUPPORT
#include "../tier1/dino_cfg_seq.h"
#include "../tier1/dino_diag_seq.h"
#include "../tier1/dino_serdes/common/srds_api_enum.h"

int _dino_phymod_prbs_poly_to_serdes_prbs_poly(uint16_t phymod_poly, uint16_t *serdes_poly)
{
    switch (phymod_poly) {
        case phymodPrbsPoly7:
            *serdes_poly = PRBS_7;
        break;
        case phymodPrbsPoly9:
            *serdes_poly = PRBS_9;
        break;
        case phymodPrbsPoly11:
            *serdes_poly = PRBS_11;
        break;
        case phymodPrbsPoly15:
            *serdes_poly = PRBS_15;
        break;
        case phymodPrbsPoly23:
            *serdes_poly = PRBS_23;
        break;
        case phymodPrbsPoly31:
            *serdes_poly = PRBS_31;
        break;
        case phymodPrbsPoly58:
            *serdes_poly = PRBS_58;
        break;
        default: 
            return PHYMOD_E_PARAM;
    }

    return PHYMOD_E_NONE;
}
int _dino_serdes_prbs_poly_to_phymod_prbs_poly(uint16_t serdes_poly, phymod_prbs_poly_t *phymod_poly)
{
    switch (serdes_poly) {
        case PRBS_7:
            *phymod_poly = phymodPrbsPoly7;
        break; 
        case PRBS_9:
            *phymod_poly = phymodPrbsPoly9;
        break; 
        case PRBS_11:
            *phymod_poly = phymodPrbsPoly11;
        break; 
        case PRBS_15:
            *phymod_poly = phymodPrbsPoly15;
        break; 
        case PRBS_23:
            *phymod_poly = phymodPrbsPoly23;
        break; 
        case PRBS_31:
            *phymod_poly = phymodPrbsPoly31;
        break; 
        case PRBS_58:
            *phymod_poly = phymodPrbsPoly58;
        break; 
        default: 
            return PHYMOD_E_PARAM;
    }

    return PHYMOD_E_NONE;
}

int dino_phy_prbs_config_set(const phymod_phy_access_t* phy, uint32_t flags , const phymod_prbs_t* prbs)
{
    uint16_t lane = 0;
    uint16_t lane_mask = 0; 
    uint16_t if_side = 0;
    int num_lanes = 0;
    uint32_t chip_id = 0;
    uint32_t rev_id = 0;
    const phymod_access_t *pa = &phy->access;

    lane_mask = PHYMOD_ACC_LANE_MASK(pa);
    DINO_GET_INTF_SIDE(phy, if_side);
    PHYMOD_IF_ERR_RETURN (dino_get_chipid(&phy->access, &chip_id, &rev_id));
    DINO_GET_NUM_LANES(chip_id, num_lanes);

    for (lane = 0; lane < num_lanes ; lane ++) {
        if (lane_mask & (1 << lane)) {
            PHYMOD_IF_ERR_RETURN (
                _dino_set_slice_reg (pa, DINO_SLICE_UNICAST, if_side, lane));
            PHYMOD_IF_ERR_RETURN(
                _dino_phy_prbs_config_set(pa, flags, if_side, lane, prbs)); 
        }
    }
    PHYMOD_IF_ERR_RETURN (
        _dino_set_slice_reg (pa, DINO_SLICE_RESET, DINO_IF_LINE, 0));

    return PHYMOD_E_NONE;
}

int dino_phy_prbs_config_get(const phymod_phy_access_t* phy, uint32_t flags , phymod_prbs_t* prbs)
{
    uint16_t lane = 0;
    uint16_t lane_mask = 0;
    uint16_t if_side = 0;
    uint16_t serdes_poly = 0;
    uint16_t   chkr_mode = 0;
    int num_lanes = 0;
    uint32_t chip_id = 0;
    uint32_t rev_id = 0;
    const phymod_access_t *pa = &phy->access;

    lane_mask = PHYMOD_ACC_LANE_MASK(pa);
    DINO_GET_INTF_SIDE(phy, if_side);
    PHYMOD_IF_ERR_RETURN (dino_get_chipid(&phy->access, &chip_id, &rev_id));
    DINO_GET_NUM_LANES(chip_id, num_lanes);

    for (lane = 0; lane < num_lanes; lane ++) {
        if ((lane_mask & (1 << lane))) {
            PHYMOD_IF_ERR_RETURN (
                _dino_set_slice_reg (pa, DINO_SLICE_UNICAST, if_side, lane));
            PHYMOD_IF_ERR_RETURN(
                _dino_phy_prbs_config_get(pa, flags, if_side, lane, prbs, &serdes_poly, &chkr_mode)); 
            break;
        }
    }
    PHYMOD_IF_ERR_RETURN (
        _dino_set_slice_reg (pa, DINO_SLICE_RESET, DINO_IF_LINE, 0));
    PHYMOD_DEBUG_VERBOSE(("Checker POLY:%d\n", serdes_poly));
    PHYMOD_DEBUG_VERBOSE(("Checker mode:%d\n", chkr_mode));
    PHYMOD_IF_ERR_RETURN (
        _dino_serdes_prbs_poly_to_phymod_prbs_poly(serdes_poly, &prbs->poly));

    return PHYMOD_E_NONE;
}

int dino_phy_prbs_enable_set(const phymod_phy_access_t* phy, uint32_t flags , uint32_t enable)
{
    uint16_t lane = 0;
    uint16_t lane_mask = 0;
    uint16_t if_side = 0;
    int num_lanes = 0;
    uint32_t chip_id = 0;
    uint32_t rev_id = 0;
    const phymod_access_t *pa = &phy->access;

    lane_mask = PHYMOD_ACC_LANE_MASK(pa);
    DINO_GET_INTF_SIDE(phy, if_side);
    PHYMOD_IF_ERR_RETURN (dino_get_chipid(&phy->access, &chip_id, &rev_id));
    DINO_GET_NUM_LANES(chip_id, num_lanes);

    for (lane = 0; lane < num_lanes; lane ++) {
        if (lane_mask & (1 << lane)) {
             PHYMOD_IF_ERR_RETURN (
                 _dino_set_slice_reg (pa, DINO_SLICE_UNICAST, if_side, lane));
             PHYMOD_IF_ERR_RETURN(
                 _dino_phy_prbs_enable_set(pa, flags, if_side, lane, enable));
        }
    }
    PHYMOD_IF_ERR_RETURN (
        _dino_set_slice_reg (pa, DINO_SLICE_RESET, DINO_IF_LINE, 0));

    return PHYMOD_E_NONE;
}

int dino_phy_prbs_enable_get(const phymod_phy_access_t* phy, uint32_t flags , uint32_t* enable)
{
    uint16_t lane = 0;
    uint16_t lane_mask = 0;
    uint16_t if_side = 0;
    int num_lanes = 0;
    uint32_t chip_id = 0;
    uint32_t rev_id = 0;
    const phymod_access_t *pa = &phy->access;

    lane_mask = PHYMOD_ACC_LANE_MASK(pa);
    DINO_GET_INTF_SIDE(phy, if_side);
    PHYMOD_IF_ERR_RETURN (dino_get_chipid(&phy->access, &chip_id, &rev_id));
    DINO_GET_NUM_LANES(chip_id, num_lanes);

    for (lane = 0; lane < num_lanes; lane ++) {
        if ((lane_mask & (1 << lane))) {
            PHYMOD_IF_ERR_RETURN (
                _dino_set_slice_reg (pa, DINO_SLICE_UNICAST, if_side, lane));
            PHYMOD_IF_ERR_RETURN(
                _dino_phy_prbs_enable_get(pa, flags, if_side, lane, enable));
            break;
        }
    }
    PHYMOD_IF_ERR_RETURN (
        _dino_set_slice_reg (pa, DINO_SLICE_RESET, DINO_IF_LINE, 0));

    return PHYMOD_E_NONE;
}

int dino_phy_prbs_status_get(const phymod_phy_access_t* phy, uint32_t flags, phymod_prbs_status_t* prbs_status)
{
    uint16_t lane = 0;
    uint16_t lane_mask = 0;
    uint16_t if_side = 0;
    int num_lanes = 0;
    uint32_t chip_id = 0;
    uint32_t rev_id = 0;
    const phymod_access_t *pa = &phy->access;

    lane_mask = PHYMOD_ACC_LANE_MASK(pa);
    DINO_GET_INTF_SIDE(phy, if_side);
    PHYMOD_IF_ERR_RETURN (dino_get_chipid(&phy->access, &chip_id, &rev_id));
    DINO_GET_NUM_LANES(chip_id, num_lanes);

    prbs_status->prbs_lock = 1;
    prbs_status->prbs_lock_loss = 1;
    prbs_status->error_count = 0;

    for (lane = 0; lane < num_lanes; lane ++) {
        if ((lane_mask & (1 << lane))) {
            PHYMOD_IF_ERR_RETURN (
                _dino_set_slice_reg (pa, DINO_SLICE_UNICAST, if_side, lane));
            PHYMOD_IF_ERR_RETURN(
                _dino_phy_prbs_status_get(pa, flags, if_side, lane, prbs_status));
        }
    }
    PHYMOD_IF_ERR_RETURN (
        _dino_set_slice_reg (pa, DINO_SLICE_RESET, DINO_IF_LINE, 0));

    return PHYMOD_E_NONE; 
}

int dino_core_diagnostics_get(const phymod_core_access_t* core, phymod_core_diagnostics_t* diag)
{
    phymod_phy_access_t phy;
    const phymod_access_t *pa = &core->access;
    uint16_t lane_mask = 0;
    uint16_t if_side = 0; 
    uint16_t lane = 0;
    int num_lanes = 0;
    uint32_t chip_id = 0;
    uint32_t rev_id = 0;

    PHYMOD_MEMCPY(&phy, core, sizeof(phymod_core_access_t));
    lane_mask = PHYMOD_ACC_LANE_MASK(pa);
    DINO_GET_INTF_SIDE((&phy), if_side);
    PHYMOD_IF_ERR_RETURN (dino_get_chipid(&core->access, &chip_id, &rev_id));
    DINO_GET_NUM_LANES(chip_id, num_lanes);

    for (lane = 0; lane < num_lanes; lane ++) {
        if ((lane_mask & (1 << lane))) {
            PHYMOD_IF_ERR_RETURN (
                _dino_set_slice_reg (pa, DINO_SLICE_UNICAST, if_side, lane));
            PHYMOD_IF_ERR_RETURN(
                _dino_core_diagnostics_get(pa, if_side, lane, diag));
            break;
        }
    }
    PHYMOD_IF_ERR_RETURN (
        _dino_set_slice_reg (pa, DINO_SLICE_RESET, DINO_IF_LINE, 0));

    return PHYMOD_E_NONE;
}

int dino_phy_diagnostics_get(const phymod_phy_access_t* phy, phymod_phy_diagnostics_t* diag)
{
    uint16_t lane = 0;
    uint16_t lane_mask = 0;
    uint16_t if_side = 0;
    int num_lanes = 0;
    uint32_t chip_id = 0;
    uint32_t rev_id = 0;
    const phymod_access_t *pa = &phy->access;

    lane_mask = PHYMOD_ACC_LANE_MASK(pa);
    DINO_GET_INTF_SIDE(phy, if_side);
    PHYMOD_IF_ERR_RETURN (dino_get_chipid(&phy->access, &chip_id, &rev_id));
    DINO_GET_NUM_LANES(chip_id, num_lanes);

    PHYMOD_DEBUG_VERBOSE(("**********************************************\n"));
    PHYMOD_DEBUG_VERBOSE(("******* PHY status dump for PHY ID:%d ********\n", pa->addr));
    PHYMOD_DEBUG_VERBOSE(("**********************************************\n"));
    PHYMOD_DEBUG_VERBOSE(("**** PHY status dump for interface side:%d ****\n", if_side));
    PHYMOD_DEBUG_VERBOSE(("***********************************************\n"));

    for (lane = 0; lane < num_lanes; lane ++) {
        if ((lane_mask & (1 << lane))) {
            PHYMOD_IF_ERR_RETURN (
                _dino_set_slice_reg (pa, DINO_SLICE_UNICAST, if_side, lane));
            PHYMOD_IF_ERR_RETURN(
                _dino_phy_diagnostics_get(pa, if_side, lane, diag));
        }
    }
    PHYMOD_IF_ERR_RETURN (
        _dino_set_slice_reg (pa, DINO_SLICE_RESET, DINO_IF_LINE, 0));

    return PHYMOD_E_NONE;
}

int dino_phy_eyescan_run (const phymod_phy_access_t* phy, uint32_t flags, phymod_eyescan_mode_t mode, const phymod_phy_eyescan_options_t* eyescan_options)
{
    uint16_t lane = 0;
    uint16_t lane_mask = 0;
    uint16_t if_side = 0;
    int num_lanes = 0;
    uint32_t chip_id = 0;
    uint32_t rev_id = 0;
    const phymod_access_t *pa = &phy->access;

    lane_mask = PHYMOD_ACC_LANE_MASK(pa);
    DINO_GET_INTF_SIDE(phy, if_side);
    PHYMOD_IF_ERR_RETURN (dino_get_chipid(&phy->access, &chip_id, &rev_id));
    DINO_GET_NUM_LANES(chip_id, num_lanes);

    PHYMOD_DEBUG_VERBOSE(("**********************************************\n"));
    PHYMOD_DEBUG_VERBOSE(("******* PHY status dump for PHY ID:%d ********\n", pa->addr));
    PHYMOD_DEBUG_VERBOSE(("**********************************************\n"));
    PHYMOD_DEBUG_VERBOSE(("**** PHY status dump for interface side:%d ****\n", if_side));
    PHYMOD_DEBUG_VERBOSE(("***********************************************\n"));

    for (lane = 0; lane < num_lanes; lane ++) {
        if ((lane_mask & (1 << lane))) {
            PHYMOD_IF_ERR_RETURN (
                _dino_set_slice_reg (pa, DINO_SLICE_UNICAST, if_side, lane));
            if (PHYMOD_EYESCAN_F_DONE_GET(flags)) {
                switch(mode) {
                case phymodEyescanModeFast:
                    PHYMOD_IF_ERR_RETURN (
                       _dino_phy_display_eyescan(pa, if_side, lane));
                 break;
                case phymodEyescanModeBERProj:
                    PHYMOD_IF_ERR_RETURN (
                      _dino_ber_proj(phy, if_side, lane, eyescan_options));
                 break;
                default:
                    PHYMOD_IF_ERR_RETURN (
                        _dino_phy_display_eyescan(pa, if_side, lane));
                  break;
                }
            } else {
                return PHYMOD_E_NONE;
            }
        }
    }
    PHYMOD_IF_ERR_RETURN (
        _dino_set_slice_reg (pa, DINO_SLICE_RESET, DINO_IF_LINE, 0));
   
    return PHYMOD_E_NONE;
}

int dino_phy_pmd_info_dump(const phymod_phy_access_t* phy, const char* type)
{
    uint16_t lane = 0;
    uint16_t lane_mask = 0;
    uint16_t if_side = 0;
    int num_lanes = 0;
    uint32_t chip_id = 0;
    uint32_t rev_id = 0;
    const phymod_access_t *pa = &phy->access;

    lane_mask = PHYMOD_ACC_LANE_MASK(pa);
    DINO_GET_INTF_SIDE(phy, if_side);
    PHYMOD_IF_ERR_RETURN (dino_get_chipid(&phy->access, &chip_id, &rev_id));
    DINO_GET_NUM_LANES(chip_id, num_lanes);

    PHYMOD_DEBUG_VERBOSE(("**********************************************\n"));
    PHYMOD_DEBUG_VERBOSE(("******* PHY status dump for PHY ID:%d ********\n", pa->addr));
    PHYMOD_DEBUG_VERBOSE(("**********************************************\n"));
    PHYMOD_DEBUG_VERBOSE(("**** PHY status dump for interface side:%d ****\n", if_side));
    PHYMOD_DEBUG_VERBOSE(("***********************************************\n"));

    /* For core status dump */
    for (lane = 0; lane < num_lanes; lane ++) {
        if ((lane_mask & (1 << lane))) {
            PHYMOD_IF_ERR_RETURN (
                _dino_set_slice_reg (pa, DINO_SLICE_UNICAST, if_side, lane));
            PHYMOD_IF_ERR_RETURN(
                _dino_phy_status_dump(pa, if_side, lane));
        }
    }
    PHYMOD_IF_ERR_RETURN (
        _dino_set_slice_reg (pa, DINO_SLICE_RESET, DINO_IF_LINE, 0));

    return PHYMOD_E_NONE;
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
int dino_phy_link_mon_enable_set(const phymod_phy_access_t* phy, phymod_link_monitor_mode_t fcpcs_chkr_mode, uint32_t enable)
{
    uint16_t lane = 0;
    uint16_t lane_mask = 0;
    uint16_t if_side = 0;
    int num_lanes = 0;
    uint32_t chip_id = 0;
    uint32_t rev_id = 0;
    const phymod_access_t *pa = &phy->access;

    lane_mask = PHYMOD_ACC_LANE_MASK(pa);
    DINO_GET_INTF_SIDE(phy, if_side);
    PHYMOD_IF_ERR_RETURN (dino_get_chipid(&phy->access, &chip_id, &rev_id));
    DINO_GET_NUM_LANES(chip_id, num_lanes);

    for (lane = 0; lane < num_lanes; lane ++) {
        if ((lane_mask & (1 << lane))) {
            PHYMOD_IF_ERR_RETURN (
                _dino_set_slice_reg (pa, DINO_SLICE_UNICAST, if_side, lane));
            PHYMOD_IF_ERR_RETURN(
                _dino_phy_link_mon_enable_set(pa, enable));
        }
    }

    PHYMOD_IF_ERR_RETURN (
        _dino_set_slice_reg (pa, DINO_SLICE_RESET, DINO_IF_LINE, 0));

    return PHYMOD_E_NONE;
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
int dino_phy_link_mon_enable_get(const phymod_phy_access_t* phy, phymod_link_monitor_mode_t fcpcs_chkr_mode, uint32_t *enable)
{
    uint16_t lane = 0;
    uint16_t lane_mask = 0;
    uint16_t if_side = 0;
    int num_lanes = 0;
    uint32_t chip_id = 0;
    uint32_t rev_id = 0;
    const phymod_access_t *pa = &phy->access;

    lane_mask = PHYMOD_ACC_LANE_MASK(pa);
    DINO_GET_INTF_SIDE(phy, if_side);
    PHYMOD_IF_ERR_RETURN (dino_get_chipid(&phy->access, &chip_id, &rev_id));
    DINO_GET_NUM_LANES(chip_id, num_lanes);
    *enable = 1;

    for (lane = 0; lane < num_lanes; lane ++) {
        if ((lane_mask & (1 << lane))) {
            PHYMOD_IF_ERR_RETURN (
                _dino_set_slice_reg (pa, DINO_SLICE_UNICAST, if_side, lane));
            PHYMOD_IF_ERR_RETURN (
                _dino_phy_link_mon_enable_get(pa, enable));
            break;
        }
    }
    PHYMOD_IF_ERR_RETURN (
        _dino_set_slice_reg (pa, DINO_SLICE_RESET, DINO_IF_LINE, 0));

    return PHYMOD_E_NONE;
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
int dino_phy_link_mon_status_get(const phymod_phy_access_t* phy, uint32_t* lock_status, uint32_t* lock_lost_lh, uint32_t* error_count)
{
    uint16_t lane = 0;
    uint16_t lane_mask = 0;
    uint16_t if_side = 0;
    int num_lanes = 0;
    uint32_t chip_id = 0;
    uint32_t rev_id = 0;
    uint32_t enable = 0;
    const phymod_access_t *pa = &phy->access;

    lane_mask = PHYMOD_ACC_LANE_MASK(pa);
    DINO_GET_INTF_SIDE(phy, if_side);
    PHYMOD_IF_ERR_RETURN (dino_get_chipid(&phy->access, &chip_id, &rev_id));
    DINO_GET_NUM_LANES(chip_id, num_lanes);
    
    *lock_status = 1;
    *lock_lost_lh = 1;
    *error_count = 0;
    enable = 1;

    for (lane = 0; lane < num_lanes; lane ++) {
        if ((lane_mask & (1 << lane))) {
            PHYMOD_IF_ERR_RETURN (
                _dino_set_slice_reg (pa, DINO_SLICE_UNICAST, if_side, lane));
            PHYMOD_IF_ERR_RETURN (
                _dino_phy_link_mon_enable_get(pa, &enable));
            if (enable) {
                PHYMOD_IF_ERR_RETURN (
                    _dino_phy_link_mon_status_get(pa, lock_status, lock_lost_lh, error_count));
            } else {
                *lock_status &= 0;
                *lock_lost_lh &= 1;
                *error_count = 0;
            }
        }
    }
    PHYMOD_IF_ERR_RETURN (
        _dino_set_slice_reg (pa, DINO_SLICE_RESET, DINO_IF_LINE, 0));

    return PHYMOD_E_NONE;
}
#endif /* PHYMOD_DINO_SUPPORT */
