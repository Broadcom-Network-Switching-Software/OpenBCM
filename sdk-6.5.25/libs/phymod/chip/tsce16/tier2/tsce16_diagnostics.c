/*
 *         
 * 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2021 Broadcom Inc. All rights reserved.
 *     
 */
#ifndef __KERNEL__
#include <stdlib.h>
#endif
#include <phymod/phymod.h>
#include <phymod/phymod_system.h>
#include <phymod/phymod_diagnostics.h>
#include <phymod/phymod_diagnostics_dispatch.h>
#include <phymod/phymod_util.h>
#include "tsce16/tier1/temod16_enum_defines.h"
#include "tsce16/tier1/temod16.h"
#include "tsce16/tier1/te16PCSRegEnums.h"
#include "merlin16/tier1/merlin16_diag.h"
#include "merlin16/tier1/merlin16_debug_functions.h"
#include "merlin16/tier1/common/srds_api_enum.h"
#include "merlin16/tier1/merlin16_prbs.h"
#include "merlin16/tier1/merlin16_config.h"
#include <phymod/chip/merlin16_diagnostics.h>
#include <phymod/chip/tsce16.h>

#ifdef PHYMOD_TSCE16_SUPPORT

#define TSCE16_NOF_LANES_IN_CORE (4)

int tsce16_phy_pmd_info_dump(const phymod_phy_access_t* phy, const char* type)
{
    int start_lane, num_lane;
    int tmp_lane_mask;
    phymod_phy_access_t phy_copy;

    /* struct merlin16_detailed_lane_status_st lane_st[4]; */
    int i = 0;
    uint32_t cmd_type;
    int is_in_reset = 0;
    int log_level = 1;

    if (!type) {
        cmd_type = (uint32_t)TEMOD16_DIAG_DSC;
    } else if ((!PHYMOD_STRCMP(type, "ber")) ||
               (!PHYMOD_STRCMP(type, "Ber")) ||
               (!PHYMOD_STRCMP(type, "BER"))) {
        cmd_type = (uint32_t)TEMOD16_DIAG_BER;
    } else if ((!PHYMOD_STRCMP(type, "config")) ||
               (!PHYMOD_STRCMP(type, "Config")) ||
               (!PHYMOD_STRCMP(type, "CONFIG"))) {
        cmd_type = (uint32_t)TEMOD16_DIAG_CFG;
    } else if ((!PHYMOD_STRCMP(type, "cl72")) ||
               (!PHYMOD_STRCMP(type, "Cl72")) ||
               (!PHYMOD_STRCMP(type, "CL72"))) {
        cmd_type = (uint32_t)TEMOD16_DIAG_CL72;
    } else if ((!PHYMOD_STRCMP(type, "debug")) ||
               (!PHYMOD_STRCMP(type, "Debug")) ||
               (!PHYMOD_STRCMP(type, "DEBUG"))) {
        cmd_type = (uint32_t)TEMOD16_DIAG_DEBUG;
    } else if ((!PHYMOD_STRCMP(type, "state")) ||
               (!PHYMOD_STRCMP(type, "State")) ||
               (!PHYMOD_STRCMP(type, "STATE"))) {
        cmd_type = (uint32_t)TEMOD16_DIAG_STATE;
    } else if ((!PHYMOD_STRNCMP(type, "state-", 6)) ||
               (!PHYMOD_STRNCMP(type, "State-", 6)) ||
               (!PHYMOD_STRNCMP(type, "STATE-", 6))) {
        cmd_type = (uint32_t)TEMOD16_DIAG_STATE;
        log_level = PHYMOD_STRTOUL((type + 6), NULL, 0);
    } else if ((!PHYMOD_STRCMP(type, "verbose")) ||
               (!PHYMOD_STRCMP(type, "Verbose")) ||
               (!PHYMOD_STRCMP(type, "VERBOSE"))) {
        cmd_type = (uint32_t)TEMOD16_DIAG_ALL;
    } else if ((!PHYMOD_STRCMP(type, "std")) ||
               (!PHYMOD_STRCMP(type, "Std")) ||
               (!PHYMOD_STRCMP(type, "STD"))) {
        cmd_type = (uint32_t)TEMOD16_DIAG_DSC_STD;
    } else if ((!PHYMOD_STRNCMP(type, "std-", 4)) ||
               (!PHYMOD_STRNCMP(type, "Std-", 4)) ||
               (!PHYMOD_STRNCMP(type, "STD-", 4))) {
        cmd_type = (uint32_t)TEMOD16_DIAG_DSC_STD;
        log_level = PHYMOD_STRTOUL((type + 4), NULL, 0);
    } else {
        cmd_type = (uint32_t)TEMOD16_DIAG_STATE;
    }

    PHYMOD_DEBUG_ERROR((" %s:%d type = %d laneMask  = 0x%X\n", __func__, __LINE__, cmd_type, phy->access.lane_mask));

    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));
 
    /*next figure out the lane num and start_lane based on the input*/
    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(&phy->access, &start_lane, &num_lane));

    if (cmd_type == TEMOD16_DIAG_DSC) {
        PHYMOD_IF_ERR_RETURN
            (merlin16_display_core_state(&phy_copy.access));
        PHYMOD_IF_ERR_RETURN
            (merlin16_display_lane_state_hdr());
        for (i = 0; i < 4; i++) {
            phy_copy.access.lane_mask = 0x1 << i ;
            PHYMOD_IF_ERR_RETURN
                (temod16_pmd_x4_reset_get(&phy_copy.access, &is_in_reset));
            if (is_in_reset) {
                PHYMOD_DIAG_OUT(("    - skip lane=%0d\n", i));
                continue;
            }
            PHYMOD_IF_ERR_RETURN
                (merlin16_display_lane_state(&phy_copy.access));
       }
    } else if (cmd_type == TEMOD16_DIAG_DSC_STD) {
        PHYMOD_DIAG_OUT(("    +--------------------------------------------------------------------+\n"));
        PHYMOD_DIAG_OUT(("    | DSC Phy: 0x%03x lane_mask: 0x%02x                                  |\n", phy->access.addr, phy->access.lane_mask));
        PHYMOD_DIAG_OUT(("    +--------------------------------------------------------------------+\n"));
        PHYMOD_IF_ERR_RETURN
            (merlin16_display_core_state(&phy_copy.access));
        PHYMOD_IF_ERR_RETURN
            (merlin16_display_lane_state_hdr());
        for (i = 0; i < num_lane; i++) {
            phy_copy.access.lane_mask = 0x1 << (i + start_lane);
            PHYMOD_IF_ERR_RETURN
                (temod16_pmd_x4_reset_get(&phy_copy.access, &is_in_reset));
            if (is_in_reset) {
                PHYMOD_DIAG_OUT(("    - skip lane=%0d\n", (i + start_lane)));
                continue;
            }
            PHYMOD_IF_ERR_RETURN
                (merlin16_display_lane_state(&phy_copy.access));
        }
        PHYMOD_IF_ERR_RETURN
            (merlin16_set_usr_ctrl_lane_event_log_level(&phy_copy.access, log_level));
        PHYMOD_IF_ERR_RETURN
            (merlin16_read_event_log(&phy_copy.access));
    } else {
        for (i = 0; i < num_lane; i++) {
            phy_copy.access.lane_mask = 0x1 << (i + start_lane);
            
            switch (cmd_type) {
            case TEMOD16_DIAG_CFG:
                PHYMOD_DEBUG_ERROR((" %s:%d type = CFG\n", __func__, __LINE__));
                if (i==0) {
                    tmp_lane_mask = phy_copy.access.lane_mask;
                    phy_copy.access.lane_mask = 1;
                    PHYMOD_IF_ERR_RETURN
                        (merlin16_display_core_config(&phy_copy.access));
                    phy_copy.access.lane_mask = tmp_lane_mask;
                }
                PHYMOD_IF_ERR_RETURN
                    (merlin16_display_lane_config(&phy_copy.access));
                break;
            case TEMOD16_DIAG_CL72:
                PHYMOD_DEBUG_ERROR((" %s:%d type = CL72\n", __func__, __LINE__));
                PHYMOD_IF_ERR_RETURN
                    (merlin16_display_cl72_status(&phy_copy.access));
                break;
            case TEMOD16_DIAG_DEBUG:
                PHYMOD_DEBUG_ERROR((" %s:%d type = DBG\n", __func__, __LINE__));
                PHYMOD_IF_ERR_RETURN
                    (merlin16_display_lane_debug_status(&phy_copy.access));
                break;
            case TEMOD16_DIAG_BER:
                PHYMOD_DEBUG_ERROR((" %s:%d type = BER\n", __func__, __LINE__));
                break;
            case TEMOD16_DIAG_ALL:
                PHYMOD_DEBUG_ERROR((" %s:%d type = CFG\n", __func__, __LINE__));
                if (i==0) {
                    tmp_lane_mask = phy_copy.access.lane_mask;
                    phy_copy.access.lane_mask = 1;
                    PHYMOD_IF_ERR_RETURN
                        (merlin16_display_core_config(&phy_copy.access));
                    phy_copy.access.lane_mask = tmp_lane_mask;
                }
                PHYMOD_IF_ERR_RETURN
                    (merlin16_display_lane_config(&phy_copy.access));

                PHYMOD_DEBUG_ERROR((" %s:%d type = CL72\n", __func__, __LINE__));
                PHYMOD_IF_ERR_RETURN
                    (merlin16_display_cl72_status(&phy_copy.access));

                PHYMOD_DEBUG_ERROR((" %s:%d type = DBG\n", __func__, __LINE__));
                PHYMOD_IF_ERR_RETURN
                    (merlin16_display_lane_debug_status(&phy_copy.access));
                break;
            case TEMOD16_DIAG_STATE:
            default:
                PHYMOD_DEBUG_ERROR((" %s:%d type = DEF\n", __func__, __LINE__));
                 PHYMOD_IF_ERR_RETURN
                    (merlin16_display_core_state_hdr());
                 PHYMOD_IF_ERR_RETURN
                    (merlin16_display_core_state_line(&phy_copy.access));
                PHYMOD_IF_ERR_RETURN
                    (merlin16_display_core_state(&phy_copy.access));
                PHYMOD_IF_ERR_RETURN
                    (merlin16_display_lane_state_hdr());
                PHYMOD_IF_ERR_RETURN
                    (merlin16_display_lane_state(&phy_copy.access));
                PHYMOD_IF_ERR_RETURN
                    (merlin16_set_usr_ctrl_lane_event_log_level(&phy_copy.access, log_level));
                PHYMOD_IF_ERR_RETURN
                    (merlin16_read_event_log(&phy_copy.access));
                break;
            }
        }
    }
    return PHYMOD_E_NONE;
}

int tsce16_phy_pcs_info_dump(const phymod_phy_access_t* phy, const char* type)
{
    phymod_phy_access_t phy_copy;

    PHYMOD_MEMCPY        (&phy_copy, phy, sizeof(phy_copy));
    PHYMOD_IF_ERR_RETURN (temod16_diag_disp(&phy_copy.access, type));

    return PHYMOD_E_NONE;
}

STATIC
int _tsce16_prbs_poly_phymod_to_merlin16(phymod_prbs_poly_t phymod_poly, enum srds_prbs_polynomial_enum *merlin16_poly)
{
    switch(phymod_poly){
    case phymodPrbsPoly7:
        *merlin16_poly = PRBS_7;
        break;
    case phymodPrbsPoly9:
        *merlin16_poly = PRBS_9;
        break;
    case phymodPrbsPoly11:
        *merlin16_poly = PRBS_11;
        break;
    case phymodPrbsPoly15:
        *merlin16_poly = PRBS_15;
        break;
    case phymodPrbsPoly23:
        *merlin16_poly = PRBS_23;
        break;
    case phymodPrbsPoly31:
        *merlin16_poly = PRBS_31;
        break;
    case phymodPrbsPoly58:
        *merlin16_poly = PRBS_58;
        break;
    default:
        PHYMOD_RETURN_WITH_ERR(PHYMOD_E_PARAM, (_PHYMOD_MSG("unsupported poly for tsce16 %u"), phymod_poly));
    }
    return PHYMOD_E_NONE;
}

STATIC
int _tsce16_prbs_poly_merlin16_to_phymod(enum srds_prbs_polynomial_enum merlin16_poly, phymod_prbs_poly_t *phymod_poly)
{
    switch(merlin16_poly){
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
        PHYMOD_RETURN_WITH_ERR(PHYMOD_E_INTERNAL, (_PHYMOD_MSG("uknown poly %u"), merlin16_poly));
    }
    return PHYMOD_E_NONE;
}

int tsce16_phy_prbs_config_set(const phymod_phy_access_t* phy, uint32_t flags , const phymod_prbs_t* prbs)
{
    enum  srds_prbs_polynomial_enum merlin16_poly;
    int i, start_lane, num_lane;
    phymod_phy_access_t phy_copy;
    PHYMOD_IF_ERR_RETURN(_tsce16_prbs_poly_phymod_to_merlin16(prbs->poly, &merlin16_poly));

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
                    (merlin16_config_rx_prbs(&phy_copy.access, merlin16_poly, PRBS_INITIAL_SEED_HYSTERESIS,  prbs->invert));
        }
    } else if (PHYMOD_PRBS_DIRECTION_TX_GET(flags)) {
        for (i = 0; i < num_lane; i++) {
            phy_copy.access.lane_mask = 0x1 << (i + start_lane);
            if (!PHYMOD_LANEPBMP_MEMBER(phy->access.lane_mask, start_lane + i)) {
                continue;
            }
            PHYMOD_IF_ERR_RETURN
                    (merlin16_config_tx_prbs(&phy_copy.access, merlin16_poly, prbs->invert));
        }
    } else {
        for (i = 0; i < num_lane; i++) {
            phy_copy.access.lane_mask = 0x1 << (i + start_lane);
            if (!PHYMOD_LANEPBMP_MEMBER(phy->access.lane_mask, start_lane + i)) {
                continue;
            }
            PHYMOD_IF_ERR_RETURN
                    (merlin16_config_rx_prbs(&phy_copy.access, merlin16_poly, PRBS_INITIAL_SEED_HYSTERESIS,  prbs->invert));
            PHYMOD_IF_ERR_RETURN
                    (merlin16_config_tx_prbs(&phy_copy.access, merlin16_poly, prbs->invert));
        }
    }

    return PHYMOD_E_NONE;
}

int tsce16_phy_prbs_config_get(const phymod_phy_access_t* phy, uint32_t flags , phymod_prbs_t* prbs)
{
    enum srds_prbs_polynomial_enum merlin16_poly;
    enum srds_prbs_checker_mode_enum merlin16_checker_mode;
    uint8_t invert;

    if (PHYMOD_PRBS_DIRECTION_TX_GET(flags)) {
        PHYMOD_IF_ERR_RETURN(merlin16_get_tx_prbs_config(&phy->access, &merlin16_poly, &invert)); 
    } else if (PHYMOD_PRBS_DIRECTION_RX_GET(flags)) {
        PHYMOD_IF_ERR_RETURN(merlin16_get_rx_prbs_config(&phy->access, &merlin16_poly, &merlin16_checker_mode, &invert));
    } else {
        PHYMOD_IF_ERR_RETURN(merlin16_get_tx_prbs_config(&phy->access, &merlin16_poly, &invert));
    }

    prbs->invert = (uint32_t) invert;
    PHYMOD_IF_ERR_RETURN(_tsce16_prbs_poly_merlin16_to_phymod(merlin16_poly, &(prbs->poly)));

    return PHYMOD_E_NONE;
}


int tsce16_phy_prbs_enable_set(const phymod_phy_access_t* phy, uint32_t flags , uint32_t enable)
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
            PHYMOD_IF_ERR_RETURN(merlin16_tx_prbs_en(&phy_copy.access, enable));
        }
    } else if (PHYMOD_PRBS_DIRECTION_RX_GET(flags)) {
        for (i = 0; i < num_lane; i++) {
            if (!PHYMOD_LANEPBMP_MEMBER(phy->access.lane_mask, start_lane + i)) {
                continue;
            }
            phy_copy.access.lane_mask = 0x1 << (i + start_lane);
            PHYMOD_IF_ERR_RETURN(merlin16_rx_prbs_en(&phy_copy.access, enable));
        }
    } else {
        for (i = 0; i < num_lane; i++) {
            if (!PHYMOD_LANEPBMP_MEMBER(phy->access.lane_mask, start_lane + i)) {
                continue;
            }
            phy_copy.access.lane_mask = 0x1 << (i + start_lane);
            PHYMOD_IF_ERR_RETURN(merlin16_tx_prbs_en(&phy_copy.access, enable));
            PHYMOD_IF_ERR_RETURN(merlin16_rx_prbs_en(&phy_copy.access, enable));
        }
    }

    return PHYMOD_E_NONE;
}

int tsce16_phy_prbs_enable_get(const phymod_phy_access_t* phy, uint32_t flags , uint32_t* enable)
{
    uint8_t enable_tmp;

    if (PHYMOD_PRBS_DIRECTION_TX_GET(flags)) {
        PHYMOD_IF_ERR_RETURN(merlin16_get_tx_prbs_en(&phy->access, &enable_tmp));
        *enable = enable_tmp;
    } else if (PHYMOD_PRBS_DIRECTION_RX_GET(flags)) {
        PHYMOD_IF_ERR_RETURN(merlin16_get_rx_prbs_en(&phy->access, &enable_tmp));
        *enable = enable_tmp;
    } else {
        PHYMOD_IF_ERR_RETURN(merlin16_get_tx_prbs_en(&phy->access, &enable_tmp));
        *enable = enable_tmp;
        PHYMOD_IF_ERR_RETURN(merlin16_get_rx_prbs_en(&phy->access, &enable_tmp));
        *enable &= enable_tmp;
    }

    return PHYMOD_E_NONE;
}


int tsce16_phy_prbs_status_get(const phymod_phy_access_t* phy, uint32_t flags, phymod_prbs_status_t* prbs_status)
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
        PHYMOD_IF_ERR_RETURN(merlin16_prbs_chk_lock_state(&phy_copy.access, &status));
        if (status) {
            /*next check the lost of lock and error count */
            status = 0;
            PHYMOD_IF_ERR_RETURN
                (merlin16_prbs_err_count_state(&phy_copy.access, &prbs_err_count, &status));
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
            /* return PHYMOD_E_NONE; */
        }
    }

    return PHYMOD_E_NONE;
}

int tsce16_phy_fec_error_inject_set(const phymod_phy_access_t* phy,
                                    uint16_t error_control_map,
                                    phymod_fec_error_mask_t bit_error_mask)
{
    phymod_phy_access_t phy_copy;
    int start_lane, num_lane;
    uint16_t valid_error_mask = 0xf;
    uint32_t fec_enable = 0;
    temod16_fec_error_mask_t fec_error_mask;

    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));
    fec_error_mask.error_mask_bit_31_0 = bit_error_mask.error_mask_bit_31_0;
    fec_error_mask.error_mask_bit_63_32 = bit_error_mask.error_mask_bit_63_32;
    fec_error_mask.error_mask_bit_79_64 = bit_error_mask.error_mask_bit_79_64;

    PHYMOD_IF_ERR_RETURN(temod16_fecmode_get(&phy->access, &fec_enable));
    if (!fec_enable) {
        PHYMOD_DEBUG_ERROR((" %s:%d fec is NOT enabled!\n", __func__, __LINE__));
        return PHYMOD_E_CONFIG;
    }

    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(&phy->access, &start_lane, &num_lane));

    /* valid error_control_map(bit mask):
     * single lane port     :  [0:0]
     * dual lane port       :  [1:0]
     * 4-lane port          :  [3:0]
     * all lanes on the port:  0xff
     *
     * If the error_control_map is not 0xff, needs to translate it to lane_mask.
     * Otherwise, use the lane_mask directly.
     */
    if (error_control_map != 0xff) {
        switch (num_lane) {
            case 0x1:
                valid_error_mask = 0x1;
                break;
            case 0x2:
                valid_error_mask = 0x3;
                phy_copy.access.lane_mask = (error_control_map & 0x3) << start_lane;
                break;
            case 0x4:
                valid_error_mask = phy->access.lane_mask;
                phy_copy.access.lane_mask = error_control_map & 0xf;
                break;
            default:
                return PHYMOD_E_CONFIG;
        }
        if (error_control_map & (~valid_error_mask)) {
            PHYMOD_DEBUG_ERROR((" %s:%d error_control_map is wrong! valid mask range 0x%x\n", __func__, __LINE__, valid_error_mask));
            return PHYMOD_E_CONFIG;
        }
    }

    PHYMOD_IF_ERR_RETURN(temod16_fec_error_inject_set(&phy_copy.access, error_control_map, fec_error_mask));

    return PHYMOD_E_NONE;
}

int tsce16_phy_fec_error_inject_get(const phymod_phy_access_t* phy,
                                    uint16_t* error_control_map,
                                    phymod_fec_error_mask_t* bit_error_mask)
{
    temod16_fec_error_mask_t fec_error_mask;
    uint16_t control_map = 0;
    int start_lane, num_lane;

    PHYMOD_IF_ERR_RETURN(temod16_fec_error_inject_get(&phy->access, &control_map, &fec_error_mask));

    bit_error_mask->error_mask_bit_31_0 = fec_error_mask.error_mask_bit_31_0;
    bit_error_mask->error_mask_bit_63_32 = fec_error_mask.error_mask_bit_63_32;
    bit_error_mask->error_mask_bit_79_64 = fec_error_mask.error_mask_bit_79_64;

    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(&phy->access, &start_lane, &num_lane));

      switch (num_lane) {
         case 0x1:
             *error_control_map = (control_map >> start_lane) & 0x1;
             break;
         case 0x2:
             *error_control_map = (control_map >> start_lane) & 0x3;
             break;
         case 0x4:
             *error_control_map = control_map & 0xf;
             break;
         default:
             return PHYMOD_E_CONFIG;
     }

    return PHYMOD_E_NONE;
}


int tsce16_phy_fec_correctable_counter_get(const phymod_phy_access_t* phy, uint32_t* count)
{
    uint32_t lane_count;
    uint32_t sum = 0;
    phymod_phy_access_t phy_copy;
    int start_lane, num_lane;
    int i = 0;

    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));
    /*next figure out the lane num and start_lane based on the input*/
    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(&phy->access, &start_lane, &num_lane));

    /* Add counts of all the lanes of the port */
    for (i = 0; i < num_lane; i++) {
        if (!PHYMOD_LANEPBMP_MEMBER(phy->access.lane_mask, start_lane + i)) {
            continue;
        }
        phy_copy.access.lane_mask = 0x1 << (i + start_lane);
        PHYMOD_IF_ERR_RETURN(temod16_fec_correctable_counter_get(&phy_copy.access, &lane_count));
        /* Check overflow */
        if (lane_count > (0xffffffff - sum)) {
            sum = 0xffffffff;
            break;
        } else {
            sum += lane_count;
        }
    }

    *count = sum;

    return PHYMOD_E_NONE;
}

int tsce16_phy_fec_uncorrectable_counter_get(const phymod_phy_access_t* phy, uint32_t* count)
{
    uint32_t lane_count;
    uint32_t sum = 0;
    phymod_phy_access_t phy_copy;
    int start_lane, num_lane;
    int i = 0;

    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));
    /*next figure out the lane num and start_lane based on the input*/
    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(&phy->access, &start_lane, &num_lane));

    /* Add counts of all the lanes of the port */
    for (i = 0; i < num_lane; i++) {
        if (!PHYMOD_LANEPBMP_MEMBER(phy->access.lane_mask, start_lane + i)) {
            continue;
        }
        phy_copy.access.lane_mask = 0x1 << (i + start_lane);
        PHYMOD_IF_ERR_RETURN(temod16_fec_uncorrectable_counter_get(&phy_copy.access, &lane_count));
        /* Check overflow */
        if (lane_count > 0xffffffff - sum) {
            sum = 0xffffffff;
            break;
        } else {
            sum += lane_count;
        }
    }

    *count = sum;

    return PHYMOD_E_NONE;
}

int tsce16_phy_stat_get(const phymod_phy_access_t* phy, phymod_phy_stat_t* stat)
{
    phymod_phy_access_t phy_copy;
    temod16_encoder_type_t encode_mode;
    int i = 0;

    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));

    for (i = 0; i < TSCE16_NOF_LANES_IN_CORE; i++) {
        if (!PHYMOD_LANEPBMP_MEMBER(phy->access.lane_mask, i)) {
            continue;
        }

        phy_copy.access.lane_mask = 0x1 << i;

        PHYMOD_IF_ERR_RETURN(temod16_encode_mode_get(&phy_copy.access, &encode_mode));
        if (encode_mode == TEMOD16_ENCODER_CL49) {
            /* CL49 has no BIP count */
            stat->pcs_bip_err_count[i] = 0;
            PHYMOD_IF_ERR_RETURN(temod16_cl49_ber_counter_get(&phy_copy.access, &stat->pcs_ber_count[i]));
        } else if (encode_mode == TEMOD16_ENCODER_CL82) {
            PHYMOD_IF_ERR_RETURN(temod16_bip_error_counter_get(&phy_copy.access, &stat->pcs_bip_err_count[i]));
            PHYMOD_IF_ERR_RETURN(temod16_cl82_ber_counter_get(&phy_copy.access, &stat->pcs_ber_count[i]));
        } else {
            stat->pcs_bip_err_count[i] = 0;
            stat->pcs_ber_count[i]     = 0;
        }
    }

    return PHYMOD_E_NONE;
}

STATIC
int tsce16_diagnostics_eye_margin_proj(
                        const phymod_phy_access_t* phy,
                        uint32_t flags,
                        const phymod_phy_eyescan_options_t* eyescan_options)
{
#ifdef SERDES_API_FLOATING_POINT
  USR_DOUBLE data_rate_in_Hz;
  phymod_phy_access_t phy_copy;
  int start_lane, num_lane, i, found=0;
  phymod_ref_clk_t ref_clk;
  phymod_phy_inf_config_t cfg;


  if(PHYMOD_EYESCAN_F_PROCESS_GET(flags)) {
     PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(&phy->access, &start_lane, &num_lane));
     PHYMOD_IF_ERR_RETURN
            (temod16_refclk_get(&phy->access, &ref_clk));
     PHYMOD_IF_ERR_RETURN
        (phymod_phy_inf_config_t_init(&cfg));

     PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));
     for (i = 0; i < num_lane; i++) {
         phy_copy.access.lane_mask = 1 << (start_lane + i);

         if (found == 0) {
             PHYMOD_IF_ERR_RETURN
                (tsce16_phy_interface_config_get(phy, 0, ref_clk, &cfg));
             data_rate_in_Hz = cfg.data_rate * 1000 * 1000;
             found = 1;
         }

         PHYMOD_IF_ERR_RETURN
             (merlin16_eye_margin_proj(&phy_copy.access,
                            data_rate_in_Hz,
                            eyescan_options->ber_proj_scan_mode,
                            eyescan_options->ber_proj_timer_cnt,
                            eyescan_options->ber_proj_err_cnt));
     }
  }
#else
  PHYMOD_RETURN_WITH_ERR(PHYMOD_E_INTERNAL, (_PHYMOD_MSG("BER Proj is supported with SERDES_API_FLOATING_POINT only\n")));
#endif

  return PHYMOD_E_NONE;
}

int tsce16_phy_eyescan_run(const phymod_phy_access_t* phy,
                           uint32_t flags,
                           phymod_eyescan_mode_t mode,
                           const phymod_phy_eyescan_options_t* eyescan_options)
{
    uint8_t  pmd_rx_lock=0;
    phymod_phy_access_t phy_copy;

    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));

    PHYMOD_IF_ERR_RETURN
      (merlin16_pmd_lock_status(&phy_copy.access, &pmd_rx_lock));

    if(pmd_rx_lock == 0) {
      PHYMOD_RETURN_WITH_ERR(PHYMOD_E_INTERNAL,
             (_PHYMOD_MSG("Can not get eyescan when pmd_rx is not locked\n")));
    }

    /* If stage isn't set - perform all stages*/
    if(!PHYMOD_EYESCAN_F_ENABLE_GET(flags)
       && !PHYMOD_EYESCAN_F_PROCESS_GET(flags)
       && !PHYMOD_EYESCAN_F_DONE_GET(flags))
    {
        PHYMOD_EYESCAN_F_ENABLE_SET(flags);
        PHYMOD_EYESCAN_F_PROCESS_SET(flags);
        PHYMOD_EYESCAN_F_DONE_SET(flags);
    }

    /* mode phymodEyescanModeBERProj gives BER projection */
    switch(mode) {
        case phymodEyescanModeFast:
            return merlin16_phy_eyescan_run(phy, flags, mode, eyescan_options);
        case phymodEyescanModeBERProj:
            return tsce16_diagnostics_eye_margin_proj(phy, flags,
                                                      eyescan_options);
        default:
            PHYMOD_RETURN_WITH_ERR(PHYMOD_E_PARAM,
                         (_PHYMOD_MSG("unsupported eyescan mode %u"), mode));
    }

  return PHYMOD_E_NONE;
}

#endif /* PHYMOD_TSCE16_SUPPORT */
