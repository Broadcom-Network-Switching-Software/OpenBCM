/*
 *         
 * 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <phymod/phymod_system.h>
#include <phymod/phymod_diagnostics.h>
#include <phymod/phymod_config.h>
#include <phymod/phymod_util.h>
#include <phymod/phymod_config.h>
#include <phymod/chip/eagle_diagnostics.h>

#include "../../qtce/tier1/qmod_enum_defines.h" 
#include "../../qtce/tier1/qmod.h" 
#include "../../eagle/tier1/eagle_tsc_common.h" 
#include "../../eagle/tier1/eagle_tsc_interface.h" 
#include "../../eagle/tier1/eagle_cfg_seq.h"
#include "../../eagle/tier1/eagle_tsc_debug_functions.h"

/* extern err_code_t eagle_tsc_read_event_log( const phymod_access_t *pa, uint8_t *trace_mem,enum event_log_display_mode_enum display_mode); */

#ifdef PHYMOD_QTCE_SUPPORT

/*phymod, internal enum mappings*/
STATIC
int _qtce_prbs_poly_phymod_to_eagle(phymod_prbs_poly_t phymod_poly, enum srds_prbs_polynomial_enum *eagle_poly)
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
        PHYMOD_RETURN_WITH_ERR(PHYMOD_E_PARAM, (_PHYMOD_MSG("unsupported poly for qtce %u"), phymod_poly));
    }
    return PHYMOD_E_NONE;
}

STATIC
int _qtce_prbs_poly_qtce_to_phymod(eagle_prbs_polynomial_type_t tsce_poly, phymod_prbs_poly_t *phymod_poly)
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

#ifdef PHYMOD_TO_QTCE_OS_MODE_TRANS
STATIC
int _qtce_os_mode_phymod_to_qtce(phymod_osr_mode_t phymod_os_mode, qmod_os_mode_type *tsce_os_mode)
{
    switch(phymod_os_mode){
    case phymodOversampleMode1:
        *tsce_os_mode = QMOD_PMA_OS_MODE_1;
        break;
    case phymodOversampleMode2:
        *tsce_os_mode = QMOD_PMA_OS_MODE_2;
        break;
    case phymodOversampleMode3:
        *tsce_os_mode = QMOD_PMA_OS_MODE_3;
        break;
    case phymodOversampleMode3P3:
        *tsce_os_mode = QMOD_PMA_OS_MODE_3_3;
        break;
    case phymodOversampleMode4:
        *tsce_os_mode = QMOD_PMA_OS_MODE_4;
        break;
    case phymodOversampleMode5:
        *tsce_os_mode = QMOD_PMA_OS_MODE_5;
        break;
    case phymodOversampleMode8:
        *tsce_os_mode = QMOD_PMA_OS_MODE_8;
        break;
    case phymodOversampleMode8P25:
        *tsce_os_mode = QMOD_PMA_OS_MODE_8_25;
        break;
    case phymodOversampleMode10:
        *tsce_os_mode = QMOD_PMA_OS_MODE_10;
        break;
    default:
        PHYMOD_RETURN_WITH_ERR(PHYMOD_E_PARAM, (_PHYMOD_MSG("unsupported over sample mode for qtce %u"), phymod_os_mode));
    }
    return PHYMOD_E_NONE;
}
#endif

STATIC
int _qtce_os_mode_qtce_to_phymod(qmod_os_mode_type tsce_os_mode, phymod_osr_mode_t *phymod_os_mode)
{
    switch(tsce_os_mode){
    case QMOD_PMA_OS_MODE_1:
        *phymod_os_mode = phymodOversampleMode1;
        break;
    case QMOD_PMA_OS_MODE_2:
        *phymod_os_mode = phymodOversampleMode2;
        break;
    case QMOD_PMA_OS_MODE_3:
        *phymod_os_mode = phymodOversampleMode3;
        break;
    case QMOD_PMA_OS_MODE_3_3:
        *phymod_os_mode = phymodOversampleMode3P3;
        break;
    case QMOD_PMA_OS_MODE_4:
        *phymod_os_mode = phymodOversampleMode4;
        break;
    case QMOD_PMA_OS_MODE_5:
        *phymod_os_mode = phymodOversampleMode5;
        break;
    case QMOD_PMA_OS_MODE_8:
        *phymod_os_mode = phymodOversampleMode8;
        break;
    case QMOD_PMA_OS_MODE_8_25:
        *phymod_os_mode = phymodOversampleMode8P25;
        break;
    case QMOD_PMA_OS_MODE_10:
        *phymod_os_mode = phymodOversampleMode10;
        break;
    default:
        PHYMOD_RETURN_WITH_ERR(PHYMOD_E_INTERNAL, (_PHYMOD_MSG("uknown os mode %u"), tsce_os_mode));
    }
    return PHYMOD_E_NONE;
}

/*diagnotics functions*/

int qtce_phy_rx_slicer_position_set(const phymod_phy_access_t* phy, uint32_t flags, const phymod_slicer_position_t* position)
{
        
        
    
    /* Place your code here */

        
    return PHYMOD_E_NONE;
        
    
}

int qtce_phy_rx_slicer_position_get(const phymod_phy_access_t* phy, uint32_t flags, phymod_slicer_position_t* position)
{
        
        
    
    /* Place your code here */

        
    return PHYMOD_E_NONE;
        
    
}


int qtce_phy_rx_slicer_position_max_get(const phymod_phy_access_t* phy, uint32_t flags, const phymod_slicer_position_t* position_min, const phymod_slicer_position_t* position_max)
{
        
        
    
    /* Place your code here */

        
    return PHYMOD_E_NONE;
        
    
}

int qtce_phy_prbs_config_set(const phymod_phy_access_t* phy, uint32_t flags , const phymod_prbs_t* prbs)
{
    phymod_phy_access_t phy_copy;
    int start_lane, num_lane, lane_id, sub_port;
    int i = 0;

    enum srds_prbs_polynomial_enum eagle_poly;

    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));
 
    /*next figure out the lane num and start_lane based on the input*/
    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(&phy->access, &start_lane, &num_lane));
    PHYMOD_IF_ERR_RETURN
        (qmod_lane_info(&phy->access, &lane_id, &sub_port));
    start_lane = lane_id;

    PHYMOD_IF_ERR_RETURN(_qtce_prbs_poly_phymod_to_eagle(prbs->poly, &eagle_poly));
    /*first check which direction */
    if (PHYMOD_PRBS_DIRECTION_RX_GET(flags)) {
        for (i = 0; i < num_lane; i++) {
            phy_copy.access.lane_mask = 0x1 << (i + start_lane);
            PHYMOD_IF_ERR_RETURN
                (eagle_tsc_config_rx_prbs(&phy_copy.access, eagle_poly, PRBS_INITIAL_SEED_HYSTERESIS,  prbs->invert));
        }
    } else if (PHYMOD_PRBS_DIRECTION_TX_GET(flags)) {
        for (i = 0; i < num_lane; i++) {
            phy_copy.access.lane_mask = 0x1 << (i + start_lane);
            PHYMOD_IF_ERR_RETURN
                (eagle_tsc_config_tx_prbs(&phy_copy.access, eagle_poly, prbs->invert));
        }
    } else {
        for (i = 0; i < num_lane; i++) {
            phy_copy.access.lane_mask = 0x1 << (i + start_lane);
            PHYMOD_IF_ERR_RETURN
                (eagle_tsc_config_rx_prbs(&phy_copy.access, eagle_poly, PRBS_INITIAL_SEED_HYSTERESIS,  prbs->invert));
            PHYMOD_IF_ERR_RETURN
                (eagle_tsc_config_tx_prbs(&phy_copy.access, eagle_poly, prbs->invert));
        }
    }         
    return PHYMOD_E_NONE;
}

int qtce_phy_prbs_config_get(const phymod_phy_access_t* phy, uint32_t flags , phymod_prbs_t* prbs)
{
    phymod_prbs_t config_tmp;
    eagle_prbs_polynomial_type_t tsce_poly;
    phymod_phy_access_t phy_copy;
    int lane_id, sub_port;

    PHYMOD_IF_ERR_RETURN
        (qmod_lane_info(&phy->access, &lane_id, &sub_port));
    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));
    /*There is only single lane mode in QTCE*/
    phy_copy.access.lane_mask = 0x1 << lane_id;

    if (PHYMOD_PRBS_DIRECTION_TX_GET(flags)) {
        PHYMOD_IF_ERR_RETURN(eagle_prbs_tx_inv_data_get(&phy_copy.access, &config_tmp.invert));
        PHYMOD_IF_ERR_RETURN(eagle_prbs_tx_poly_get(&phy_copy.access, &tsce_poly));
        PHYMOD_IF_ERR_RETURN(_qtce_prbs_poly_qtce_to_phymod(tsce_poly, &config_tmp.poly));
        prbs->invert = config_tmp.invert;
        prbs->poly = config_tmp.poly;
    } else if (PHYMOD_PRBS_DIRECTION_RX_GET(flags)) {
        PHYMOD_IF_ERR_RETURN(eagle_prbs_rx_inv_data_get(&phy_copy.access, &config_tmp.invert));
        PHYMOD_IF_ERR_RETURN(eagle_prbs_rx_poly_get(&phy_copy.access, &tsce_poly));
        PHYMOD_IF_ERR_RETURN(_qtce_prbs_poly_qtce_to_phymod(tsce_poly, &config_tmp.poly));
        prbs->invert = config_tmp.invert;
        prbs->poly = config_tmp.poly;
    } else {
        PHYMOD_IF_ERR_RETURN(eagle_prbs_tx_inv_data_get(&phy_copy.access, &config_tmp.invert));
        PHYMOD_IF_ERR_RETURN(eagle_prbs_tx_poly_get(&phy_copy.access, &tsce_poly));
        PHYMOD_IF_ERR_RETURN(_qtce_prbs_poly_qtce_to_phymod(tsce_poly, &config_tmp.poly));
        prbs->invert = config_tmp.invert;
        prbs->poly = config_tmp.poly;
    }
    return PHYMOD_E_NONE;
}



int qtce_phy_prbs_enable_set(const phymod_phy_access_t* phy, uint32_t flags , uint32_t enable)
{
    phymod_phy_access_t phy_copy;
    int start_lane, num_lane, lane_id, sub_port;
    int i = 0;


    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));
 
    /*next figure out the lane num and start_lane based on the input*/
    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(&phy->access, &start_lane, &num_lane));
    PHYMOD_IF_ERR_RETURN
        (qmod_lane_info(&phy->access, &lane_id, &sub_port));
    start_lane = lane_id;

    if (PHYMOD_PRBS_DIRECTION_TX_GET(flags)) {
        for (i = 0; i < num_lane; i++) {
            phy_copy.access.lane_mask = 0x1 << (i + start_lane);
            PHYMOD_IF_ERR_RETURN(eagle_tsc_tx_prbs_en(&phy_copy.access, enable));
        }
    } else if (PHYMOD_PRBS_DIRECTION_RX_GET(flags)) {
        for (i = 0; i < num_lane; i++) {
            phy_copy.access.lane_mask = 0x1 << (i + start_lane);
            PHYMOD_IF_ERR_RETURN(eagle_tsc_rx_prbs_en(&phy_copy.access, enable));
        }
    } else {
        for (i = 0; i < num_lane; i++) {
            phy_copy.access.lane_mask = 0x1 << (i + start_lane);
            PHYMOD_IF_ERR_RETURN(eagle_tsc_tx_prbs_en(&phy_copy.access, enable));
            PHYMOD_IF_ERR_RETURN(eagle_tsc_rx_prbs_en(&phy_copy.access, enable));
        }
    }
    return PHYMOD_E_NONE;   
}

int qtce_phy_prbs_enable_get(const phymod_phy_access_t* phy, uint32_t flags , uint32_t *enable)
{
    uint32_t enable_tmp;
    phymod_phy_access_t phy_copy;
    int lane_id, sub_port;

    PHYMOD_IF_ERR_RETURN
        (qmod_lane_info(&phy->access, &lane_id, &sub_port));
    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));
    /*There is only single lane mode in QTCE*/
    phy_copy.access.lane_mask = 0x1 << lane_id;

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


int qtce_phy_prbs_status_get(const phymod_phy_access_t* phy, uint32_t flags, phymod_prbs_status_t* prbs_status)
{
    uint8_t status = 0;
    uint32_t prbs_err_count = 0;
    int i, start_lane, num_lane, lane_id, sub_port;
    phymod_phy_access_t phy_copy;

    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));
    /* next figure out the lane num and start_lane based on the input */
    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(&phy->access, &start_lane, &num_lane));
    PHYMOD_IF_ERR_RETURN
        (qmod_lane_info(&phy->access, &lane_id, &sub_port));
    start_lane = lane_id;
    
    prbs_status->prbs_lock = 1;
    prbs_status->error_count = 0;
    prbs_status->prbs_lock_loss = 0;

    for (i = 0; i < num_lane; i++) {
        phy_copy.access.lane_mask = 0x1 << (i + start_lane);
        PHYMOD_IF_ERR_RETURN(eagle_tsc_prbs_chk_lock_state(&phy_copy.access, &status));
        if (status) {
            /*next check the lost of lock and error count */
            status = 0;
            PHYMOD_IF_ERR_RETURN
                (eagle_tsc_prbs_err_count_state(&phy_copy.access, &prbs_err_count, &status));
            PHYMOD_DEBUG_VERBOSE((" Lane :: %d PRBS Error count :: %d lock_loss=%0d\n", i, prbs_err_count, status));
            if (status) {
                /*temp lost of lock */
                prbs_status->prbs_lock_loss = 1;
            } else {
                prbs_status->error_count += prbs_err_count;
            }
        } else {
            PHYMOD_DEBUG_VERBOSE((" Lane :: %d PRBS not locked\n", i ));
            prbs_status->prbs_lock = 0;
            return PHYMOD_E_NONE;
        }
    }
    return PHYMOD_E_NONE;
}


int qtce_phy_pattern_config_set(const phymod_phy_access_t* phy, const phymod_pattern_t* pattern, uint32_t* mode_sel)
{
    return PHYMOD_E_NONE;
}

int qtce_phy_pattern_config_get(const phymod_phy_access_t* phy, phymod_pattern_t* pattern)
{
    /* Place your code here */

        
    return PHYMOD_E_NONE;
}

int qtce_phy_pattern_enable_set(const phymod_phy_access_t* phy, uint32_t enable, uint32_t mode_sel)
{
    return PHYMOD_E_NONE;
}

int qtce_phy_pattern_enable_get(const phymod_phy_access_t* phy, uint32_t* enable)
{
        
        
    
    /* Place your code here */

        
    return PHYMOD_E_NONE;
}

int qtce_core_diagnostics_get(const phymod_core_access_t* core, phymod_core_diagnostics_t* diag)
{
        
    
    /* Place your code here */

        
    return PHYMOD_E_NONE;
    
}

int qtce_phy_diagnostics_get(const phymod_phy_access_t* phy, phymod_phy_diagnostics_t* diag)
{
    phymod_phy_access_t phy_copy;
    int lane_id, sub_port;
    qmod_os_mode_type os_mode = QMOD_PMA_OS_MODE_1;
    phymod_diag_eyescan_t_init(&diag->eyescan);
    phymod_diag_slicer_offset_t_init(&diag->slicer_offset);
    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));
    PHYMOD_IF_ERR_RETURN
        (qmod_lane_info(&phy->access, &lane_id, &sub_port));
    /*There is only single lane mode in QTCE*/
    phy_copy.access.lane_mask = 0x1 << lane_id;

    PHYMOD_IF_ERR_RETURN(qmod_pmd_lock_get(&phy_copy.access, &diag->rx_lock));
    
    PHYMOD_IF_ERR_RETURN(_qtce_os_mode_qtce_to_phymod(os_mode, &diag->osr_mode));
        
    return PHYMOD_E_NONE;
    
}

int qtce_phy_pmd_info_dump(const phymod_phy_access_t* phy, const char *type)
{
    int start_lane, num_lane, lane_id, sub_port;
    phymod_phy_access_t phy_copy;
    int i = 0;
    uint32_t cmd_type;
    uint8_t trace_mem[768];
    int is_in_reset = 0;

    if (!type) {
        cmd_type = (uint32_t)QMOD_DIAG_DSC;
    } else if (!PHYMOD_STRCMP(type, "ber")) {
        cmd_type = (uint32_t)QMOD_DIAG_BER;
    } else if (!PHYMOD_STRCMP(type, "config")) {
        cmd_type = (uint32_t)QMOD_DIAG_CFG;
    } else if (!PHYMOD_STRCMP(type, "CL72") || !PHYMOD_STRCMP(type, "cl72")) {
        cmd_type = (uint32_t)QMOD_DIAG_CL72;
    } else if (!PHYMOD_STRCMP(type, "debug")) {
        cmd_type = (uint32_t)QMOD_DIAG_DEBUG;
    } else if (!PHYMOD_STRCMP(type, "state")) {
        cmd_type = (uint32_t)QMOD_DIAG_STATE;
    } else {
        cmd_type = (uint32_t)QMOD_DIAG_DSC;
    }

    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));
 
    /*next figure out the lane num and start_lane based on the input*/
    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(&phy->access, &start_lane, &num_lane));
    PHYMOD_IF_ERR_RETURN
        (qmod_lane_info(&phy->access, &lane_id, &sub_port));
    start_lane = lane_id;

    if (cmd_type == QMOD_DIAG_DSC) {
        PHYMOD_IF_ERR_RETURN
            (eagle_tsc_display_core_state(&phy_copy.access));
        PHYMOD_IF_ERR_RETURN
            (eagle_tsc_display_lane_state_hdr(&phy_copy.access));
        for (i = 0; i < 4; i++) {
            phy_copy.access.lane_mask = 0x1 << i ;
            PHYMOD_IF_ERR_RETURN
                    (qmod_pmd_x4_reset_get(&phy_copy.access, &is_in_reset));
            if (is_in_reset) {
                PHYMOD_DIAG_OUT(("    - skip lane=%0d\n", i));
                continue;
            }
            PHYMOD_IF_ERR_RETURN
                (eagle_tsc_display_lane_state(&phy_copy.access));
        }
    } else {
        for (i = 0; i < num_lane; i++) {
            phy_copy.access.lane_mask = 0x1 << (i + start_lane);

            switch (cmd_type) {
            case QMOD_DIAG_CFG:
                PHYMOD_IF_ERR_RETURN
                    (eagle_tsc_display_core_config(&phy_copy.access));
                 PHYMOD_IF_ERR_RETURN
                     (eagle_tsc_display_lane_config(&phy_copy.access));
                 break;
                 
            case QMOD_DIAG_CL72:
                PHYMOD_IF_ERR_RETURN
                    (eagle_tsc_display_cl72_status(&phy_copy.access));
                break;
                
            case QMOD_DIAG_DEBUG:
                PHYMOD_IF_ERR_RETURN
                    (eagle_tsc_display_lane_debug_status(&phy_copy.access));
                break;

            case QMOD_DIAG_BER:
                break;

            case QMOD_DIAG_STATE:
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

int qtce_phy_pcs_info_dump(const phymod_phy_access_t* phy, const char* type)
{
    phymod_phy_access_t phy_copy;

    PHYMOD_MEMCPY        (&phy_copy, phy, sizeof(phy_copy));
    PHYMOD_IF_ERR_RETURN (qmod_diag_disp(&phy_copy.access, type));

    return PHYMOD_E_NONE;
}


int qtce_phy_eyescan_run(const phymod_phy_access_t* phy, uint32_t flags, phymod_eyescan_mode_t mode, const phymod_phy_eyescan_options_t* eyescan_options)
{
    int lane_id, sub_port;
    phymod_phy_access_t phy_copy;

    PHYMOD_IF_ERR_RETURN
        (qmod_lane_info(&phy->access, &lane_id, &sub_port));

    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));
    phy_copy.access.lane_mask = 0x1 << lane_id;

    PHYMOD_IF_ERR_RETURN (eagle_phy_eyescan_run(&phy_copy, flags, mode, eyescan_options));

    return PHYMOD_E_NONE;
}

#endif /* PHYMOD_QTCE_SUPPORT */
