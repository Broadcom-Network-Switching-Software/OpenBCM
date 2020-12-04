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
#include <phymod/phymod_config.h>
#include <phymod/phymod_diagnostics.h>
#include <phymod/phymod_diagnostics_dispatch.h>
#include <phymod/chip/eagle_diagnostics.h>

#include "../tier1/tqmod_cfg_seq.h"
#include "../../tsce/tier1/temod_enum_defines.h"
#include "../../tsce/tier1/temod.h"
#include "../../tsce/tier1/tePCSRegEnums.h"
#include "../../eagle/tier1/eagle_cfg_seq.h"
#include "../../eagle/tier1/eagle_tsc_interface.h"
#include "../../eagle/tier1/eagle_tsc_dependencies.h"
#include "../../eagle/tier1/eagle_tsc_debug_functions.h"

#ifdef PHYMOD_QSGMIIE_SUPPORT

int qsgmiie_phy_rx_slicer_position_set(const phymod_phy_access_t* phy, uint32_t flags, const phymod_slicer_position_t* position)
{
        
    
    /* Place your code here */

        
    return PHYMOD_E_NONE;
    
}

int qsgmiie_phy_rx_slicer_position_get(const phymod_phy_access_t* phy, uint32_t flags, phymod_slicer_position_t* position)
{
        
    
    /* Place your code here */

        
    return PHYMOD_E_NONE;
    
}


int qsgmiie_phy_rx_slicer_position_max_get(const phymod_phy_access_t* phy, uint32_t flags, const phymod_slicer_position_t* position_min, const phymod_slicer_position_t* position_max)
{
        
    
    /* Place your code here */

        
    return PHYMOD_E_NONE;
    
}

STATIC
int _qsgmiie_prbs_poly_phymod_to_eagle(phymod_prbs_poly_t phymod_poly, enum srds_prbs_polynomial_enum *eagle_poly)
{
    switch(phymod_poly){
    case phymodPrbsPoly7:
        *eagle_poly = PRBS_7;
        break;
    case phymodPrbsPoly9:
        *eagle_poly = PRBS_9;
        break;
    case phymodPrbsPoly11:
        *eagle_poly = PRBS_11;
        break;
    case phymodPrbsPoly15:
        *eagle_poly = PRBS_15;
        break;
    case phymodPrbsPoly23:
        *eagle_poly = PRBS_23;
        break;
    case phymodPrbsPoly31:
        *eagle_poly = PRBS_31;
        break;
    case phymodPrbsPoly58:
        *eagle_poly = PRBS_58;
        break;
    default:
        PHYMOD_RETURN_WITH_ERR(PHYMOD_E_PARAM, (_PHYMOD_MSG("unsupported poly for tsce %u"), phymod_poly));
    }
    return PHYMOD_E_NONE;
}

STATIC
int _qsgmiie_prbs_poly_tsce_to_phymod(eagle_prbs_polynomial_type_t tsce_poly, phymod_prbs_poly_t *phymod_poly)
{
    switch(tsce_poly){
    case EAGLE_PRBS_POLYNOMIAL_7:
        *phymod_poly = phymodPrbsPoly7;
        break;
    case EAGLE_PRBS_POLYNOMIAL_9:
        *phymod_poly = phymodPrbsPoly9;
        break;
    case EAGLE_PRBS_POLYNOMIAL_11:
        *phymod_poly = phymodPrbsPoly11;
        break;
    case EAGLE_PRBS_POLYNOMIAL_15:
        *phymod_poly = phymodPrbsPoly15;
        break;
    case EAGLE_PRBS_POLYNOMIAL_23:
        *phymod_poly = phymodPrbsPoly23;
        break;
    case EAGLE_PRBS_POLYNOMIAL_31:
        *phymod_poly = phymodPrbsPoly31;
        break;
    case EAGLE_PRBS_POLYNOMIAL_58:
        *phymod_poly = phymodPrbsPoly58;
        break;
    default:
        PHYMOD_RETURN_WITH_ERR(PHYMOD_E_INTERNAL, (_PHYMOD_MSG("uknown poly %u"), tsce_poly));
    }
    return PHYMOD_E_NONE;
}


int qsgmiie_phy_prbs_config_set(const phymod_phy_access_t* phy,  uint32_t flags, const phymod_prbs_t* prbs)
{
        
    phymod_phy_access_t phy_copy;
    int start_lane, num_lane;

    enum srds_prbs_polynomial_enum eagle_poly;

    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));
 
    /*next figure out the lane num and start_lane based on the input*/
    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(&phy->access, &start_lane, &num_lane));
    phy_copy.access.lane_mask = 0x1 << (start_lane / 4);

    PHYMOD_IF_ERR_RETURN(_qsgmiie_prbs_poly_phymod_to_eagle(prbs->poly, &eagle_poly));
    /*first check which direction */
    if (PHYMOD_PRBS_DIRECTION_RX_GET(flags)) {
        PHYMOD_IF_ERR_RETURN
            (eagle_tsc_config_rx_prbs(&phy_copy.access, eagle_poly, PRBS_INITIAL_SEED_HYSTERESIS,  prbs->invert));
    } else if (PHYMOD_PRBS_DIRECTION_TX_GET(flags)) {
        PHYMOD_IF_ERR_RETURN
            (eagle_tsc_config_tx_prbs(&phy_copy.access, eagle_poly, prbs->invert));
    } else {
        PHYMOD_IF_ERR_RETURN
            (eagle_tsc_config_rx_prbs(&phy_copy.access, eagle_poly, PRBS_INITIAL_SEED_HYSTERESIS,  prbs->invert));
        PHYMOD_IF_ERR_RETURN
            (eagle_tsc_config_tx_prbs(&phy_copy.access, eagle_poly, prbs->invert));
    }
        
    return PHYMOD_E_NONE;
    
}


int qsgmiie_phy_prbs_config_get(const phymod_phy_access_t* phy, uint32_t flags, phymod_prbs_t* prbs)
{
    phymod_prbs_t config_tmp;
    phymod_phy_access_t phy_copy;
    int start_lane, num_lane;
    eagle_prbs_polynomial_type_t tsce_poly;

    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));

    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(&phy->access, &start_lane, &num_lane));
    phy_copy.access.lane_mask = 0x1 << (start_lane / 4);

    if (PHYMOD_PRBS_DIRECTION_TX_GET(flags)) {
        PHYMOD_IF_ERR_RETURN(eagle_prbs_tx_inv_data_get(&phy_copy.access, &config_tmp.invert));
        PHYMOD_IF_ERR_RETURN(eagle_prbs_tx_poly_get(&phy_copy.access, &tsce_poly));
        PHYMOD_IF_ERR_RETURN(_qsgmiie_prbs_poly_tsce_to_phymod(tsce_poly, &config_tmp.poly));
        prbs->invert = config_tmp.invert;
        prbs->poly = config_tmp.poly;
    } else if (PHYMOD_PRBS_DIRECTION_RX_GET(flags)) {
        PHYMOD_IF_ERR_RETURN(eagle_prbs_rx_inv_data_get(&phy_copy.access, &config_tmp.invert));
        PHYMOD_IF_ERR_RETURN(eagle_prbs_rx_poly_get(&phy_copy.access, &tsce_poly));
        PHYMOD_IF_ERR_RETURN(_qsgmiie_prbs_poly_tsce_to_phymod(tsce_poly, &config_tmp.poly));
        prbs->invert = config_tmp.invert;
        prbs->poly = config_tmp.poly;
    } else {
        PHYMOD_IF_ERR_RETURN(eagle_prbs_tx_inv_data_get(&phy_copy.access, &config_tmp.invert));
        PHYMOD_IF_ERR_RETURN(eagle_prbs_tx_poly_get(&phy_copy.access, &tsce_poly));
        PHYMOD_IF_ERR_RETURN(_qsgmiie_prbs_poly_tsce_to_phymod(tsce_poly, &config_tmp.poly));
        prbs->invert = config_tmp.invert;
        prbs->poly = config_tmp.poly;
    }
        
    return PHYMOD_E_NONE;
    
}


int qsgmiie_phy_prbs_enable_set(const phymod_phy_access_t* phy, uint32_t flags, uint32_t enable)
{
        
    phymod_phy_access_t phy_copy;
    int start_lane, num_lane;


    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));
 
    /*next figure out the lane num and start_lane based on the input*/
    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(&phy->access, &start_lane, &num_lane));
    phy_copy.access.lane_mask = 0x1 << (start_lane / 4);

    if (PHYMOD_PRBS_DIRECTION_TX_GET(flags)) {
        PHYMOD_IF_ERR_RETURN(eagle_tsc_tx_prbs_en(&phy_copy.access, enable));
    } else if (PHYMOD_PRBS_DIRECTION_RX_GET(flags)) {
        PHYMOD_IF_ERR_RETURN(eagle_tsc_rx_prbs_en(&phy_copy.access, enable));
    } else {
        PHYMOD_IF_ERR_RETURN(eagle_tsc_tx_prbs_en(&phy_copy.access, enable));
        PHYMOD_IF_ERR_RETURN(eagle_tsc_rx_prbs_en(&phy_copy.access, enable));
    }
        
    return PHYMOD_E_NONE;
    
}


int qsgmiie_phy_prbs_enable_get(const phymod_phy_access_t* phy, uint32_t flags, uint32_t* enable)
{
    phymod_phy_access_t phy_copy;
    int start_lane, num_lane;
    uint32_t enable_tmp;

    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));

    /*next figure out the lane num and start_lane based on the input*/
    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(&phy->access, &start_lane, &num_lane));
    phy_copy.access.lane_mask = 0x1 << (start_lane / 4);

    if (PHYMOD_PRBS_DIRECTION_TX_GET(flags)) {
        PHYMOD_IF_ERR_RETURN(eagle_prbs_tx_enable_get(&phy_copy.access, &enable_tmp));
        *enable = enable_tmp;
    } else if (PHYMOD_PRBS_DIRECTION_RX_GET(flags)) {
        PHYMOD_IF_ERR_RETURN(eagle_prbs_rx_enable_get(&phy_copy.access, &enable_tmp));
        *enable = enable_tmp;
    } else {
        PHYMOD_IF_ERR_RETURN(eagle_prbs_tx_enable_get(&phy_copy.access, &enable_tmp));
        *enable = enable_tmp;
        PHYMOD_IF_ERR_RETURN(eagle_prbs_rx_enable_get(&phy_copy.access, &enable_tmp));
        *enable &= enable_tmp;
    }


        
    return PHYMOD_E_NONE;
    
}


int qsgmiie_phy_prbs_status_get(const phymod_phy_access_t* phy, uint32_t flags, phymod_prbs_status_t* prbs_status)
{
        
    uint8_t status = 0;
    uint32_t prbs_err_count = 0;
    int start_lane, num_lane;
    phymod_phy_access_t phy_copy;

    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));
    /* next figure out the lane num and start_lane based on the input */
    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(&phy->access, &start_lane, &num_lane));
    
    prbs_status->prbs_lock = 1;
    prbs_status->error_count = 0;
    prbs_status->prbs_lock_loss = 0;

    phy_copy.access.lane_mask = 0x1 << (start_lane / 4);
    PHYMOD_IF_ERR_RETURN(eagle_tsc_prbs_chk_lock_state(&phy_copy.access, &status));
    if (status) {
        /*next check the lost of lock and error count */
        status = 0;
        PHYMOD_IF_ERR_RETURN
            (eagle_tsc_prbs_err_count_state(&phy_copy.access, &prbs_err_count, &status));
        PHYMOD_DEBUG_VERBOSE((" Lane :: %d PRBS Error count :: %d lock_loss=%0d\n", (start_lane / 4), prbs_err_count, status));
        if (status) {
            /*temp lost of lock */
            prbs_status->prbs_lock_loss = 1;
        } else {
            prbs_status->error_count += prbs_err_count;
        }
    } else {
        PHYMOD_DEBUG_VERBOSE((" Lane :: %d PRBS not locked\n", (start_lane / 4)));
        prbs_status->prbs_lock = 0;
        return PHYMOD_E_NONE;
    }
    return PHYMOD_E_NONE;
}


int qsgmiie_phy_pattern_config_set(const phymod_phy_access_t* phy, const phymod_pattern_t* pattern)
{
        
    
    /* Place your code here */

        
    return PHYMOD_E_NONE;
    
}

int qsgmiie_phy_pattern_config_get(const phymod_phy_access_t* phy, phymod_pattern_t* pattern)
{
        
    
    /* Place your code here */

        
    return PHYMOD_E_NONE;
    
}


int qsgmiie_phy_pattern_enable_set(const phymod_phy_access_t* phy, uint32_t enable)
{
        
    
    /* Place your code here */

        
    return PHYMOD_E_NONE;
    
}

int qsgmiie_phy_pattern_enable_get(const phymod_phy_access_t* phy, uint32_t* enable)
{
        
    
    /* Place your code here */

        
    return PHYMOD_E_NONE;
    
}


int qsgmiie_core_diagnostics_get(const phymod_core_access_t* core, phymod_core_diagnostics_t* diag)
{
        
    
    /* Place your code here */

        
    return PHYMOD_E_NONE;
    
}


int qsgmiie_phy_diagnostics_get(const phymod_phy_access_t* phy, phymod_phy_diagnostics_t* diag)
{
        
    unsigned char rx_lock;
    int osr_mode;
    phymod_diag_eyescan_t_init(&diag->eyescan);
    phymod_diag_slicer_offset_t_init(&diag->slicer_offset);

    PHYMOD_IF_ERR_RETURN(eagle_tsc_pmd_lock_status(&phy->access, &rx_lock));
    diag->rx_lock = (uint32_t ) rx_lock;
    PHYMOD_IF_ERR_RETURN(eagle_osr_mode_get(&phy->access, &osr_mode));
    switch(osr_mode) {
        case 0: diag->osr_mode = phymodOversampleMode1; break;
        case 1: diag->osr_mode = phymodOversampleMode2; break;
        case 2: diag->osr_mode = phymodOversampleMode3; break;
        case 3: diag->osr_mode = phymodOversampleMode3P3; break;
        case 4: diag->osr_mode = phymodOversampleMode4; break;
        case 5: diag->osr_mode = phymodOversampleMode5; break;
        case 6: diag->osr_mode = phymodOversampleMode7P5; break;
        case 7: diag->osr_mode = phymodOversampleMode8; break;
        case 8: diag->osr_mode = phymodOversampleMode8P25; break;
        case 9: diag->osr_mode = phymodOversampleMode10; break;
        default:
            PHYMOD_RETURN_WITH_ERR(PHYMOD_E_INTERNAL, (_PHYMOD_MSG("unsupported OS mode %d"), osr_mode));
    }
    PHYMOD_IF_ERR_RETURN(eagle_tsc_signal_detect(&phy->access, &diag->signal_detect));

    return PHYMOD_E_NONE;
    
}

int qsgmiie_phy_pmd_info_dump(const phymod_phy_access_t* phy, const char* type)
{
    int start_lane, num_lane;
    phymod_phy_access_t phy_copy;
    int i = 0;
    uint32_t cmd_type;
    uint8_t trace_mem[768];

    if (!type) {
        cmd_type = (uint32_t)TEMOD_DIAG_DSC;
    } else if (!PHYMOD_STRCMP(type, "ber")) {
        cmd_type = (uint32_t)TEMOD_DIAG_BER;
    } else if (!PHYMOD_STRCMP(type, "config")) {
        cmd_type = (uint32_t)TEMOD_DIAG_CFG;
    } else if (!PHYMOD_STRCMP(type, "CL72") || !PHYMOD_STRCMP(type, "cl72")) {
        cmd_type = (uint32_t)TEMOD_DIAG_CL72;
    } else if (!PHYMOD_STRCMP(type, "debug")) {
        cmd_type = (uint32_t)TEMOD_DIAG_DEBUG;
    } else if (!PHYMOD_STRCMP(type, "state")) {
        cmd_type = (uint32_t)TEMOD_DIAG_STATE;
    } else {
        cmd_type = (uint32_t)TEMOD_DIAG_DSC;
    }

    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));
 
    /*next figure out the lane num and start_lane based on the input*/
    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(&phy->access, &start_lane, &num_lane));

    if (cmd_type == TEMOD_DIAG_DSC) {
        phy_copy.access.lane_mask = 0x1 << (start_lane/4);
        PHYMOD_IF_ERR_RETURN
            (eagle_tsc_display_core_state(&phy_copy.access));
        PHYMOD_IF_ERR_RETURN
            (eagle_tsc_display_lane_state_hdr(&phy_copy.access));
        PHYMOD_IF_ERR_RETURN
            (eagle_tsc_display_lane_state(&phy_copy.access));
    } else {
        for (i = 0; i < num_lane; i++) {
            phy_copy.access.lane_mask = 0x1 << (i + start_lane);

            switch(cmd_type) {
            case TEMOD_DIAG_CFG:
                PHYMOD_IF_ERR_RETURN
                    (eagle_tsc_display_core_config(&phy_copy.access));
                 PHYMOD_IF_ERR_RETURN
                     (eagle_tsc_display_lane_config(&phy_copy.access));
                 break;
                 
            case TEMOD_DIAG_CL72:
                PHYMOD_IF_ERR_RETURN
                    (eagle_tsc_display_cl72_status(&phy_copy.access));
                break;
                
            case TEMOD_DIAG_DEBUG:
                PHYMOD_IF_ERR_RETURN
                    (eagle_tsc_display_lane_debug_status(&phy_copy.access));
                break;

            case TEMOD_DIAG_BER:
                break;

            case TEMOD_DIAG_STATE:
            default:
                 PHYMOD_IF_ERR_RETURN
                    (eagle_tsc_display_core_state_hdr(&phy_copy.access));
                 PHYMOD_IF_ERR_RETURN
                    (eagle_tsc_display_core_state_line(&phy_copy.access));
                PHYMOD_IF_ERR_RETURN
                    (eagle_tsc_display_core_state(&phy_copy.access));
                PHYMOD_IF_ERR_RETURN
                    (eagle_tsc_display_lane_state_hdr(&phy_copy.access));
                PHYMOD_IF_ERR_RETURN
                    (eagle_tsc_display_lane_state(&phy_copy.access));
                PHYMOD_IF_ERR_RETURN
                    (eagle_tsc_read_event_log((&phy_copy.access), trace_mem, 2));
                break;
            }
        }
    }
    return PHYMOD_E_NONE;
}

int qsgmiie_phy_eyescan_run(const phymod_phy_access_t* phy,
                                       uint32_t flags,
                                       phymod_eyescan_mode_t mode,
                                       const phymod_phy_eyescan_options_t* eyescan_options)
{
    phymod_phy_access_t phy_copy;
    int start_lane, num_lane;

    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));

    /*next figure out the lane num and start_lane based on the input*/
    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(&phy->access, &start_lane, &num_lane));
    phy_copy.access.lane_mask = 0x1 << (start_lane / 4);

    PHYMOD_IF_ERR_RETURN
        (eagle_phy_eyescan_run(&phy_copy, flags, mode, eyescan_options));

    return PHYMOD_E_NONE;
}

#endif /* PHYMOD_QSGMIIE_SUPPORT */
