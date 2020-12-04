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
#include <phymod/phymod_util.h>
#include <phymod/phymod_diagnostics.h>
#include <phymod/phymod_diagnostics_dispatch.h>
#include <phymod/chip/merlin7_diagnostics.h>
#include "merlin7/tier1/merlin7_tsc_config.h"
#include "merlin7/tier1/merlin7_tsc_diag.h"
#include "merlin7/tier1/merlin7_tsc_debug_functions.h"
#include "merlin7/tier1/common/srds_api_enum.h"
#include "merlin7/tier1/merlin7_tsc_prbs.h"

#ifdef PHYMOD_MERLIN7_SUPPORT

STATIC
int _tsce7_prbs_poly_phymod_to_merlin7(phymod_prbs_poly_t phymod_poly, enum srds_prbs_polynomial_enum *merlin7_poly)
{
    switch(phymod_poly){
    case phymodPrbsPoly7:
        *merlin7_poly = PRBS_7;
        break;
    case phymodPrbsPoly9:
        *merlin7_poly = PRBS_9;
        break;
    case phymodPrbsPoly11:
        *merlin7_poly = PRBS_11;
        break;
    case phymodPrbsPoly15:
        *merlin7_poly = PRBS_15;
        break;
    case phymodPrbsPoly23:
        *merlin7_poly = PRBS_23;
        break;
    case phymodPrbsPoly31:
        *merlin7_poly = PRBS_31;
        break;
    case phymodPrbsPoly58:
        *merlin7_poly = PRBS_58;
        break;
    default:
        PHYMOD_RETURN_WITH_ERR(PHYMOD_E_PARAM, (_PHYMOD_MSG("unsupported poly for tsce7 %u"), phymod_poly));
    }
    return PHYMOD_E_NONE;
}

STATIC
int _tsce7_prbs_poly_merlin7_to_phymod(enum srds_prbs_polynomial_enum merlin7_poly, phymod_prbs_poly_t *phymod_poly)
{
    switch(merlin7_poly){
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
        PHYMOD_RETURN_WITH_ERR(PHYMOD_E_INTERNAL, (_PHYMOD_MSG("uknown poly %u"), merlin7_poly));
    }
    return PHYMOD_E_NONE;
}

int merlin7_phy_prbs_config_set(const phymod_phy_access_t* phy,
                                uint32_t flags,
                                const phymod_prbs_t* prbs)
{
    enum  srds_prbs_polynomial_enum merlin7_poly;
    int i, start_lane, num_lane;
    phymod_phy_access_t phy_copy;
    PHYMOD_IF_ERR_RETURN(_tsce7_prbs_poly_phymod_to_merlin7(prbs->poly, &merlin7_poly));

    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));

    /* next figure out the lane num and start_lane based on the input */
    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(&phy->access, &start_lane, &num_lane));

    /*first check which direction */
    if (PHYMOD_PRBS_DIRECTION_RX_GET(flags)) {
        for (i = 0; i < num_lane; i++) {
            if (!PHYMOD_LANEPBMP_MEMBER(phy->access.lane_mask, start_lane + i)) {
                continue;
            }
            phy_copy.access.lane_mask = 0x1 << (i + start_lane);
            PHYMOD_IF_ERR_RETURN
                (merlin7_tsc_config_rx_prbs(&phy_copy.access,
                                            merlin7_poly,
                                            PRBS_INITIAL_SEED_HYSTERESIS,
                                            prbs->invert));
        }
    } else if (PHYMOD_PRBS_DIRECTION_TX_GET(flags)) {
        for (i = 0; i < num_lane; i++) {
            phy_copy.access.lane_mask = 0x1 << (i + start_lane);
            if (!PHYMOD_LANEPBMP_MEMBER(phy->access.lane_mask, start_lane + i)) {
                continue;
            }
            PHYMOD_IF_ERR_RETURN
                (merlin7_tsc_config_tx_prbs(&phy_copy.access,
                                            merlin7_poly,
                                            prbs->invert));
        }
    } else {
        for (i = 0; i < num_lane; i++) {
            phy_copy.access.lane_mask = 0x1 << (i + start_lane);
            if (!PHYMOD_LANEPBMP_MEMBER(phy->access.lane_mask, start_lane + i)) {
                continue;
            }
            PHYMOD_IF_ERR_RETURN
                (merlin7_tsc_config_rx_prbs(&phy_copy.access,
                                            merlin7_poly,
                                            PRBS_INITIAL_SEED_HYSTERESIS,
                                            prbs->invert));
            PHYMOD_IF_ERR_RETURN
                (merlin7_tsc_config_tx_prbs(&phy_copy.access,
                                            merlin7_poly,
                                            prbs->invert));
        }
    }

    return PHYMOD_E_NONE;
}

int merlin7_phy_prbs_config_get(const phymod_phy_access_t* phy,
                                uint32_t flags,
                                phymod_prbs_t* prbs)
{
    enum srds_prbs_polynomial_enum merlin7_poly;
    enum srds_prbs_checker_mode_enum merlin7_checker_mode;
    uint8_t invert;

    if (PHYMOD_PRBS_DIRECTION_TX_GET(flags)) {
        PHYMOD_IF_ERR_RETURN
            (merlin7_tsc_get_tx_prbs_config(&phy->access,
                                            &merlin7_poly,
                                            &invert));
    } else if (PHYMOD_PRBS_DIRECTION_RX_GET(flags)) {
        PHYMOD_IF_ERR_RETURN
            (merlin7_tsc_get_rx_prbs_config(&phy->access,
                                            &merlin7_poly,
                                            &merlin7_checker_mode,
                                            &invert));
    } else {
        PHYMOD_IF_ERR_RETURN
            (merlin7_tsc_get_tx_prbs_config(&phy->access,
                                            &merlin7_poly,
                                            &invert));
    }

    prbs->invert = (uint32_t) invert;
    PHYMOD_IF_ERR_RETURN
        (_tsce7_prbs_poly_merlin7_to_phymod(merlin7_poly, &(prbs->poly)));

    return PHYMOD_E_NONE;
}

int merlin7_phy_prbs_enable_set(const phymod_phy_access_t* phy,
                                uint32_t flags,
                                uint32_t enable)
{
    int i, start_lane, num_lane;
    phymod_phy_access_t phy_copy;

    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));
    /* next figure out the lane num and start_lane based on the input */
    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(&phy->access, &start_lane, &num_lane));

    if (PHYMOD_PRBS_DIRECTION_TX_GET(flags)) {
        for (i = 0; i < num_lane; i++) {
            if (!PHYMOD_LANEPBMP_MEMBER(phy->access.lane_mask, start_lane + i)) {
                continue;
            }
            phy_copy.access.lane_mask = 0x1 << (i + start_lane);
            PHYMOD_IF_ERR_RETURN
                (merlin7_tsc_tx_prbs_en(&phy_copy.access, enable));
        }
    } else if (PHYMOD_PRBS_DIRECTION_RX_GET(flags)) {
        for (i = 0; i < num_lane; i++) {
            if (!PHYMOD_LANEPBMP_MEMBER(phy->access.lane_mask, start_lane + i)) {
                continue;
            }
            phy_copy.access.lane_mask = 0x1 << (i + start_lane);
            PHYMOD_IF_ERR_RETURN
                (merlin7_tsc_rx_prbs_en(&phy_copy.access, enable));
        }
    } else {
        for (i = 0; i < num_lane; i++) {
            if (!PHYMOD_LANEPBMP_MEMBER(phy->access.lane_mask, start_lane + i)) {
                continue;
            }
            phy_copy.access.lane_mask = 0x1 << (i + start_lane);
            PHYMOD_IF_ERR_RETURN
                (merlin7_tsc_tx_prbs_en(&phy_copy.access, enable));
            PHYMOD_IF_ERR_RETURN
                (merlin7_tsc_rx_prbs_en(&phy_copy.access, enable));
        }
    }

    return PHYMOD_E_NONE;
}

int merlin7_phy_prbs_enable_get(const phymod_phy_access_t* phy,
                                uint32_t flags,
                                uint32_t* enable)
{
    uint8_t enable_tmp;

    if (PHYMOD_PRBS_DIRECTION_TX_GET(flags)) {
        PHYMOD_IF_ERR_RETURN
            (merlin7_tsc_get_tx_prbs_en(&phy->access, &enable_tmp));
        *enable = enable_tmp;
    } else if (PHYMOD_PRBS_DIRECTION_RX_GET(flags)) {
        PHYMOD_IF_ERR_RETURN
            (merlin7_tsc_get_rx_prbs_en(&phy->access, &enable_tmp));
        *enable = enable_tmp;
    } else {
        PHYMOD_IF_ERR_RETURN
            (merlin7_tsc_get_tx_prbs_en(&phy->access, &enable_tmp));
        *enable = enable_tmp;
        PHYMOD_IF_ERR_RETURN
            (merlin7_tsc_get_rx_prbs_en(&phy->access, &enable_tmp));
        *enable &= enable_tmp;
    }

    return PHYMOD_E_NONE;
}

int merlin7_phy_prbs_status_get(const phymod_phy_access_t* phy,
                                uint32_t flags,
                                phymod_prbs_status_t* prbs_status)
{
    uint8_t status = 0;
    uint32_t prbs_err_count = 0;
    int i, start_lane, num_lane;
    phymod_phy_access_t phy_copy;

    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));
    /* next figure out the lane num and start_lane based on the input */
    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(&phy->access, &start_lane, &num_lane));

    prbs_status->prbs_lock = 0;
    prbs_status->error_count = 0;
    prbs_status->prbs_lock_loss = 0;
    prbs_status->prbs_lock = 1;

    for (i = 0; i < num_lane; i++) {
        if (!PHYMOD_LANEPBMP_MEMBER(phy->access.lane_mask, start_lane + i)) {
            continue;
        }
        phy_copy.access.lane_mask = 0x1 << (i + start_lane);
        PHYMOD_IF_ERR_RETURN
            (merlin7_tsc_prbs_chk_lock_state(&phy_copy.access, &status));
        if (status) {
            /*next check the lost of lock and error count */
            status = 0;
            PHYMOD_IF_ERR_RETURN
                (merlin7_tsc_prbs_err_count_state(&phy_copy.access,
                                                  &prbs_err_count,
                                                  &status));
            PHYMOD_DEBUG_VERBOSE((" Lane :: %d PRBS Error count :: %d\n", i, prbs_err_count));
            if (status) {
                /*temp lost of lock */
                prbs_status->prbs_lock_loss = 1;
            } else {
                prbs_status->error_count += prbs_err_count;
            }
        } else {
            PHYMOD_DEBUG_VERBOSE((" Lane :: %d PRBS not locked\n", i ));
            prbs_status->prbs_lock = 0;
        }
    }

    return PHYMOD_E_NONE;
}

STATIC int merlin7_diagnostics_eyescan_run_uc(const phymod_phy_access_t* phy, uint32_t flags)
{
    int                 rc = PHYMOD_E_NONE;
    uint32_t            stripe[64];
    uint16_t            status;
    int                 i,ii;
    phymod_phy_access_t phy_copy;
    struct merlin7_tsc_uc_core_config_st core_cfg;

    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));

    /* do not run eyescan for 10Gx4 case */
    if ((PHYMOD_EYESCAN_F_ENABLE_GET(flags)) ||
        (PHYMOD_EYESCAN_F_PROCESS_GET(flags))) {
        PHYMOD_IF_ERR_RETURN(merlin7_tsc_get_uc_core_config(&(phy_copy.access), &core_cfg));
        if ((core_cfg.vco_rate_in_Mhz == 12500) && (phy->access.lane_mask == 0xf)) {
            PHYMOD_RETURN_WITH_ERR(PHYMOD_E_FAIL, (_PHYMOD_MSG("eyescan is not supported")));
        }
    }

    for (i = 0; i < PHYMOD_CONFIG_MAX_LANES_PER_CORE; i++) { /* Loop for all lanes. */
        if ((phy->access.lane_mask & (1 << i)) == 0) continue;

        phy_copy.access.lane_mask = (phy->access.lane_mask & (1 << i));

        if (PHYMOD_EYESCAN_F_ENABLE_GET(flags)) {
            rc = merlin7_tsc_meas_eye_scan_start(&(phy_copy.access), EYE_SCAN_HORIZ);
            if (rc != PHYMOD_E_NONE) {
                PHYMOD_IF_ERR_RETURN(merlin7_tsc_meas_eye_scan_done(&(phy_copy.access)));
            }
            if (!PHYMOD_EYESCAN_F_ENABLE_DONT_WAIT_GET(flags)) {
                PHYMOD_USLEEP(100000);
            }
        }

        if (PHYMOD_EYESCAN_F_PROCESS_GET(flags)) {
            for (ii = 31; ii >= -31; ii = ii-1) {
                if (ii == 31) {
                    PHYMOD_DIAG_OUT(("\n\n\n"));
                    PHYMOD_DIAG_OUT(("    +--------------------------------------------------------------------+\n"));
                    PHYMOD_DIAG_OUT(("    | EYESCAN Phy: 0x%03x lane_mask: 0x%02x                                 |\n", phy_copy.access.addr, phy_copy.access.lane_mask));
                    PHYMOD_DIAG_OUT(("    +--------------------------------------------------------------------+\n"));
                    /*display eyescan header*/
                    PHYMOD_IF_ERR_RETURN
                        (merlin7_tsc_display_eye_scan_header(&phy_copy.access,1));
                }

                rc  = merlin7_tsc_read_eye_scan_stripe(&phy_copy.access,
                                                       stripe,
                                                       &status);
                if (rc != PHYMOD_E_NONE) {
                    PHYMOD_IF_ERR_RETURN(rc);
                }
                PHYMOD_IF_ERR_RETURN
                    (merlin7_tsc_display_eye_scan_stripe(&phy_copy.access,
                                                         ii,
                                                         &stripe[0]));

                PHYMOD_DIAG_OUT(("\n"));
            }

            if (rc == PHYMOD_E_NONE) {
                PHYMOD_IF_ERR_RETURN
                    (merlin7_tsc_display_eye_scan_footer(&phy_copy.access,1));
                PHYMOD_DIAG_OUT(("\n"));
            }
        }

        if (PHYMOD_EYESCAN_F_DONE_GET(flags)) {
            PHYMOD_IF_ERR_RETURN(merlin7_tsc_meas_eye_scan_done(&(phy_copy.access)));
        }
    }

    return PHYMOD_E_NONE;
}

int merlin7_phy_eyescan_run(const phymod_phy_access_t* phy,
                            uint32_t flags,
                            phymod_eyescan_mode_t mode,
                            const phymod_phy_eyescan_options_t* eyescan_options)
{
    switch(mode) {
        case phymodEyescanModeFast:
             return merlin7_diagnostics_eyescan_run_uc(phy, flags);
        default:
            PHYMOD_RETURN_WITH_ERR(PHYMOD_E_PARAM, (_PHYMOD_MSG("unsupported eyescan mode %u"), mode));
    }

    return PHYMOD_E_NONE;

}
#endif /* PHYMOD_MERLIN7_SUPPORT */
