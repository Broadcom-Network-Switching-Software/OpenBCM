
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

#include <phymod/chip/falcon_diagnostics.h>

#include "../../tscf/tier1/tefmod_enum_defines.h" 
#include "../../falcon/tier1/falcon_cfg_seq.h"
#include "../../falcon/tier1/falcon_tsc_enum.h"
#include "../../falcon/tier1/falcon_tsc_common.h"
#include "../../falcon/tier1/falcon_tsc_interface.h"
#include "../../falcon/tier1/falcon_tsc_dependencies.h"
#include "../../falcon/tier1/falcon_tsc_debug_functions.h"


#define PATTERN_MAX_LENGTH 240
#ifdef PHYMOD_FALCON_SUPPORT


/*phymod, internal enum mappings*/
STATIC
int _falcon_prbs_poly_phymod_to_falcon(phymod_prbs_poly_t phymod_poly, enum srds_prbs_polynomial_enum *falcon_poly)
{
    switch(phymod_poly){
    case phymodPrbsPoly7:
        *falcon_poly = PRBS_7;
        break;
    case phymodPrbsPoly9:
        *falcon_poly = PRBS_9;
        break;
    case phymodPrbsPoly11:
        *falcon_poly = PRBS_11;
        break;
    case phymodPrbsPoly15:
        *falcon_poly = PRBS_15;
        break;
    case phymodPrbsPoly23:
        *falcon_poly = PRBS_23;
        break;
    case phymodPrbsPoly31:
        *falcon_poly = PRBS_31;
        break;
    case phymodPrbsPoly58:
        *falcon_poly = PRBS_58;
        break;
    default:
        PHYMOD_RETURN_WITH_ERR(PHYMOD_E_PARAM, (_PHYMOD_MSG("unsupported poly for tscf %u"), phymod_poly));
    }
    return PHYMOD_E_NONE;
}

STATIC
int _falcon_prbs_poly_tscf_to_phymod(falcon_prbs_polynomial_type_t tscf_poly, phymod_prbs_poly_t *phymod_poly)
{
    switch(tscf_poly){
    case FALCON_PRBS_POLYNOMIAL_7:
        *phymod_poly = phymodPrbsPoly7;
        break;
    case FALCON_PRBS_POLYNOMIAL_9:
        *phymod_poly = phymodPrbsPoly9;
        break;
    case FALCON_PRBS_POLYNOMIAL_11:
        *phymod_poly = phymodPrbsPoly11;
        break;
    case FALCON_PRBS_POLYNOMIAL_15:
        *phymod_poly = phymodPrbsPoly15;
        break;
    case FALCON_PRBS_POLYNOMIAL_23:
        *phymod_poly = phymodPrbsPoly23;
        break;
    case FALCON_PRBS_POLYNOMIAL_31:
        *phymod_poly = phymodPrbsPoly31;
        break;
    case FALCON_PRBS_POLYNOMIAL_58:
        *phymod_poly = phymodPrbsPoly58;
        break;
    default:
        PHYMOD_RETURN_WITH_ERR(PHYMOD_E_INTERNAL, (_PHYMOD_MSG("uknown poly %u"), tscf_poly));
    }
    return PHYMOD_E_NONE;
}

int falcon_phy_rx_slicer_position_set(const phymod_phy_access_t* phy, uint32_t flags, const phymod_slicer_position_t* position)
{
        
    
    /* Not supported */
    PHYMOD_DEBUG_ERROR(("falcon_phy_rx_slicer_position_set function is NOT SUPPORTED!!\n"));

        
    return PHYMOD_E_NONE;
    
}

int falcon_phy_rx_slicer_position_get(const phymod_phy_access_t* phy, uint32_t flags, phymod_slicer_position_t* position)
{
        
    
    /* Not supported */
    PHYMOD_DEBUG_ERROR(("falcon_phy_rx_slicer_position_get function is NOT SUPPORTED!!\n"));

        
    return PHYMOD_E_NONE;
    
}


int falcon_phy_rx_slicer_position_max_get(const phymod_phy_access_t* phy, uint32_t flags, const phymod_slicer_position_t* position_min, const phymod_slicer_position_t* position_max)
{
        
    
    /* Not supported */
    PHYMOD_DEBUG_ERROR(("falcon_phy_rx_slicer_position_max_get function is NOT SUPPORTED!!\n"));

        
    return PHYMOD_E_NONE;
    
}


int falcon_phy_prbs_config_set(const phymod_phy_access_t* phy, uint32_t flags , const phymod_prbs_t* prbs)
{
    enum srds_prbs_polynomial_enum falcon_poly;
    phymod_phy_access_t phy_copy;
    int start_lane, num_lane, i;

    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(&phy->access, &start_lane, &num_lane));
    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));

    PHYMOD_IF_ERR_RETURN(_falcon_prbs_poly_phymod_to_falcon(prbs->poly, &falcon_poly));
    /*first check which direction */
    for (i = 0; i < num_lane; i++) {
        if (!PHYMOD_LANEPBMP_MEMBER(phy->access.lane_mask, start_lane + i)) {
            continue;
        }
        phy_copy.access.lane_mask = 1 << (start_lane + i);
        if (PHYMOD_PRBS_DIRECTION_RX_GET(flags)) {
            PHYMOD_IF_ERR_RETURN
                (falcon_tsc_config_rx_prbs(&phy_copy.access, falcon_poly, PRBS_INITIAL_SEED_HYSTERESIS,  prbs->invert));
        } else if (PHYMOD_PRBS_DIRECTION_TX_GET(flags)) {
            PHYMOD_IF_ERR_RETURN
                (falcon_tsc_config_tx_prbs(&phy_copy.access, falcon_poly, prbs->invert));
        } else {
            PHYMOD_IF_ERR_RETURN
                (falcon_tsc_config_rx_prbs(&phy_copy.access, falcon_poly, PRBS_INITIAL_SEED_HYSTERESIS,  prbs->invert));
            PHYMOD_IF_ERR_RETURN
                (falcon_tsc_config_tx_prbs(&phy_copy.access, falcon_poly, prbs->invert));
        }
    }
    return PHYMOD_E_NONE;
}

int falcon_phy_prbs_config_get(const phymod_phy_access_t* phy, uint32_t flags , phymod_prbs_t* prbs)
{
    phymod_prbs_t config_tmp;
    falcon_prbs_polynomial_type_t tscf_poly;

    if (PHYMOD_PRBS_DIRECTION_TX_GET(flags)) {
        PHYMOD_IF_ERR_RETURN(falcon_prbs_tx_inv_data_get(&phy->access, &config_tmp.invert));
        PHYMOD_IF_ERR_RETURN(falcon_prbs_tx_poly_get(&phy->access, &tscf_poly));
        PHYMOD_IF_ERR_RETURN(_falcon_prbs_poly_tscf_to_phymod(tscf_poly, &config_tmp.poly));
        prbs->invert = config_tmp.invert;
        prbs->poly = config_tmp.poly;
    } else if (PHYMOD_PRBS_DIRECTION_RX_GET(flags)) {
        PHYMOD_IF_ERR_RETURN(falcon_prbs_rx_inv_data_get(&phy->access, &config_tmp.invert));
        PHYMOD_IF_ERR_RETURN(falcon_prbs_rx_poly_get(&phy->access, &tscf_poly));
        PHYMOD_IF_ERR_RETURN(_falcon_prbs_poly_tscf_to_phymod(tscf_poly, &config_tmp.poly));
        prbs->invert = config_tmp.invert;
        prbs->poly = config_tmp.poly;
    } else {
        PHYMOD_IF_ERR_RETURN(falcon_prbs_tx_inv_data_get(&phy->access, &config_tmp.invert));
        PHYMOD_IF_ERR_RETURN(falcon_prbs_tx_poly_get(&phy->access, &tscf_poly));
        PHYMOD_IF_ERR_RETURN(_falcon_prbs_poly_tscf_to_phymod(tscf_poly, &config_tmp.poly));
        prbs->invert = config_tmp.invert;
        prbs->poly = config_tmp.poly;
    }
    return PHYMOD_E_NONE;
}


int falcon_phy_prbs_enable_set(const phymod_phy_access_t* phy, uint32_t flags , uint32_t enable)
{
    phymod_phy_access_t phy_copy;
    int start_lane, num_lane, i;

    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(&phy->access, &start_lane, &num_lane));
    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));

    /*first check which direction */
    for (i = 0; i < num_lane; i++) {
        if (!PHYMOD_LANEPBMP_MEMBER(phy->access.lane_mask, start_lane + i)) {
            continue;
        }
        phy_copy.access.lane_mask = 1 << (start_lane + i);
        if (PHYMOD_PRBS_DIRECTION_TX_GET(flags)) {
            PHYMOD_IF_ERR_RETURN(falcon_tsc_tx_prbs_en(&phy_copy.access, enable));
        } else if (PHYMOD_PRBS_DIRECTION_RX_GET(flags)) {
            PHYMOD_IF_ERR_RETURN(falcon_tsc_rx_prbs_en(&phy_copy.access, enable));
        } else {
            PHYMOD_IF_ERR_RETURN(falcon_tsc_tx_prbs_en(&phy_copy.access, enable));
            PHYMOD_IF_ERR_RETURN(falcon_tsc_rx_prbs_en(&phy_copy.access, enable));
        }
    }
    return PHYMOD_E_NONE;
}

int falcon_phy_prbs_enable_get(const phymod_phy_access_t* phy, uint32_t flags , uint32_t* enable)
{
    uint32_t enable_tmp;
    if (PHYMOD_PRBS_DIRECTION_TX_GET(flags)) {
        PHYMOD_IF_ERR_RETURN(falcon_prbs_tx_enable_get(&phy->access, &enable_tmp));
        *enable = enable_tmp;
    } else if (PHYMOD_PRBS_DIRECTION_RX_GET(flags)) {
        PHYMOD_IF_ERR_RETURN(falcon_prbs_rx_enable_get(&phy->access, &enable_tmp));
        *enable = enable_tmp;
    } else {
        PHYMOD_IF_ERR_RETURN(falcon_prbs_tx_enable_get(&phy->access, &enable_tmp));
        *enable = enable_tmp;
        PHYMOD_IF_ERR_RETURN(falcon_prbs_rx_enable_get(&phy->access, &enable_tmp));
        *enable &= enable_tmp;
    }

    return PHYMOD_E_NONE;
    
}


int falcon_phy_prbs_status_get(const phymod_phy_access_t* phy, uint32_t flags, phymod_prbs_status_t* prbs_status)
{
    uint8_t status = 0;
    uint32_t prbs_err_count = 0;

    PHYMOD_IF_ERR_RETURN(falcon_tsc_prbs_chk_lock_state(&phy->access, &status));
    if (status) {
        prbs_status->prbs_lock = 1;
        /*next check the lost of lock and error count */
        status = 0;
        PHYMOD_IF_ERR_RETURN
            (falcon_tsc_prbs_err_count_state(&phy->access, &prbs_err_count, &status));
        if (status) {
        /*temp lost of lock */
            prbs_status->prbs_lock_loss = 1;
        } else {
            prbs_status->prbs_lock_loss = 0;
            prbs_status->error_count = prbs_err_count;
        }
    } else {
        prbs_status->prbs_lock = 0;
    }

    return PHYMOD_E_NONE;
    
}


int falcon_phy_pattern_config_set(const phymod_phy_access_t* phy, const phymod_pattern_t* pattern)
{
    int i,j = 0, bit;
    char patt[PATTERN_MAX_LENGTH+1];
    for (i=0; i< PATTERN_MAX_SIZE; i++)
    {
      for (j=0;j<32 && i*32+j <= PATTERN_MAX_LENGTH; j++)
      {
          if (i*32+j == (int)pattern->pattern_len) {
              break;
          }
          bit = pattern->pattern[i] >> j & 00000001;
          switch (bit) {
          case (1):
              patt[i*32+j] = '1';
              break;
          default:
              patt[i*32+j] = '0';
              break;
          }
      }
      if (i*32+j == (int)pattern->pattern_len && i*32+j <= PATTERN_MAX_LENGTH) {
          /* coverity[overrun-local] */
          patt[i*32+j] = '\0';
          break;
      }
    }
    PHYMOD_IF_ERR_RETURN
            (falcon_tsc_config_shared_tx_pattern_idx_set(&phy->access,
                                  &pattern->pattern_len ));
    PHYMOD_IF_ERR_RETURN
            (falcon_tsc_config_shared_tx_pattern (&phy->access,
                                (uint8_t) pattern->pattern_len, (const char *) patt));
        
    return PHYMOD_E_NONE;
    
}

int falcon_phy_pattern_config_get(const phymod_phy_access_t* phy, phymod_pattern_t* pattern)
{
        
    PHYMOD_IF_ERR_RETURN
        (falcon_tsc_config_shared_tx_pattern_idx_get(&phy->access,
                                  &pattern->pattern_len, 
                                  pattern->pattern));
        
    return PHYMOD_E_NONE;
    
}


int falcon_phy_pattern_enable_set(const phymod_phy_access_t* phy, uint32_t enable, const phymod_pattern_t* pattern)
{
    PHYMOD_IF_ERR_RETURN
        (falcon_tsc_tx_shared_patt_gen_en(&phy->access, (uint8_t) enable, (uint8_t)pattern->pattern_len));
    return PHYMOD_E_NONE;
    
}

int falcon_phy_pattern_enable_get(const phymod_phy_access_t* phy, uint32_t* enable)
{
    uint8_t enable_8;
    PHYMOD_IF_ERR_RETURN
       (falcon_tsc_tx_shared_patt_gen_en_get(&phy->access, &enable_8));
    *enable = (uint32_t) enable_8;
    return PHYMOD_E_NONE;
    
}


int falcon_core_diagnostics_get(const phymod_core_access_t* core, phymod_core_diagnostics_t* diag)
{
    int16_t temperature;

    falcon_tsc_read_die_temperature(&core->access, &temperature);
    diag->temperature = temperature;

    falcon_tsc_read_pll_range(&core->access, &diag->pll_range);

    return PHYMOD_E_NONE;
}


int falcon_phy_diagnostics_get(const phymod_phy_access_t* phy, phymod_phy_diagnostics_t* diag)
{
        
    unsigned char rx_lock;
    int osr_mode;
    phymod_diag_eyescan_t_init(&diag->eyescan);
    phymod_diag_slicer_offset_t_init(&diag->slicer_offset);

    PHYMOD_IF_ERR_RETURN(falcon_tsc_pmd_lock_status(&phy->access, &rx_lock));
    diag->rx_lock = (uint32_t ) rx_lock;
    PHYMOD_IF_ERR_RETURN(falcon_osr_mode_get(&phy->access, &osr_mode));
    switch(osr_mode) {
        case 0: diag->osr_mode = phymodOversampleMode1; break;
        case 1: diag->osr_mode = phymodOversampleMode2; break;
        case 2: diag->osr_mode = phymodOversampleMode4; break;
        case 5: diag->osr_mode = phymodOversampleMode8; break;
        case 8: diag->osr_mode = phymodOversampleMode16P5; break;
        case 12: diag->osr_mode = phymodOversampleMode20P625; break;
        default: 
            PHYMOD_RETURN_WITH_ERR(PHYMOD_E_INTERNAL, (_PHYMOD_MSG("unsupported OS mode %d"), osr_mode));
    }
    PHYMOD_IF_ERR_RETURN(falcon_tsc_signal_detect(&phy->access, &diag->signal_detect));

    return PHYMOD_E_NONE;
}

int falcon_phy_pmd_info_dump(const phymod_phy_access_t* phy, const char* type)
{
    int start_lane, num_lane;
    int tmp_lane_mask;
    phymod_phy_access_t phy_copy;
    struct falcon_tsc_detailed_lane_status_st lane_st[4];
    int i = 0;
    int j = 0;

    uint32_t cmd_type;

    uint8_t trace_mem[768];

    if (!type) {
        cmd_type = (uint32_t)TEFMOD_DIAG_DSC;
    } else if ((!PHYMOD_STRCMP(type, "ber")) ||
               (!PHYMOD_STRCMP(type, "Ber")) ||
               (!PHYMOD_STRCMP(type, "BER"))) {
        cmd_type = (uint32_t)TEFMOD_DIAG_BER;
    } else if ((!PHYMOD_STRCMP(type, "config")) ||
               (!PHYMOD_STRCMP(type, "Config")) ||
               (!PHYMOD_STRCMP(type, "CONFIG"))) {
        cmd_type = (uint32_t)TEFMOD_DIAG_CFG;
    } else if ((!PHYMOD_STRCMP(type, "cl72")) ||
               (!PHYMOD_STRCMP(type, "Cl72")) ||
               (!PHYMOD_STRCMP(type, "CL72"))) {
        cmd_type = (uint32_t)TEFMOD_DIAG_CL72;
    } else if ((!PHYMOD_STRCMP(type, "debug")) ||
               (!PHYMOD_STRCMP(type, "Debug")) ||
               (!PHYMOD_STRCMP(type, "DEBUG"))) {
        cmd_type = (uint32_t)TEFMOD_DIAG_DEBUG;
    } else if ((!PHYMOD_STRCMP(type, "state")) ||
               (!PHYMOD_STRCMP(type, "State")) ||
               (!PHYMOD_STRCMP(type, "STATE"))) {
        cmd_type = (uint32_t)TEFMOD_DIAG_STATE;
    } else if ((!PHYMOD_STRCMP(type, "verbose")) ||
               (!PHYMOD_STRCMP(type, "Verbose")) ||
               (!PHYMOD_STRCMP(type, "VERBOSE"))) {
        cmd_type = (uint32_t)TEFMOD_DIAG_ALL;
    } else if (!PHYMOD_STRCMP(type, "STD")) {
        cmd_type = (uint32_t)TEFMOD_DIAG_DSC_STD;
    } else {
        cmd_type = (uint32_t)TEFMOD_DIAG_STATE;
    }

    PHYMOD_DEBUG_ERROR((" %s:%d type = %d laneMask  = 0x%X\n", __func__, __LINE__, cmd_type, phy->access.lane_mask));

    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));
 
    /*next figure out the lane num and start_lane based on the input*/
    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(&phy->access, &start_lane, &num_lane));

    if (cmd_type == TEFMOD_DIAG_DSC) {
       PHYMOD_IF_ERR_RETURN
           (falcon_tsc_display_core_state(&phy_copy.access));
       PHYMOD_IF_ERR_RETURN
           (falcon_tsc_display_lane_state_hdr(&phy_copy.access)); 
       for (i = 0; i < 4; i++) {
            phy_copy.access.lane_mask = 0x1 << i ;
            PHYMOD_IF_ERR_RETURN
                (falcon_tsc_display_lane_state(&phy_copy.access));
       }
    } else if (cmd_type == TEFMOD_DIAG_DSC_STD) {

            PHYMOD_DIAG_OUT(("    +--------------------------------------------------------------------+\n"));
            PHYMOD_DIAG_OUT(("    | DSC Phy: 0x%03x lane_mask: 0x%02x                                 |\n", phy->access.addr, phy->access.lane_mask));
            PHYMOD_DIAG_OUT(("    +--------------------------------------------------------------------+\n"));
            PHYMOD_IF_ERR_RETURN
                (falcon_tsc_display_core_state(&phy_copy.access));
             PHYMOD_IF_ERR_RETURN
                (falcon_tsc_display_lane_state_hdr(&phy_copy.access));
             PHYMOD_IF_ERR_RETURN
                (falcon_tsc_display_lane_state(&phy_copy.access));
             tmp_lane_mask = phy_copy.access.lane_mask;
             for (j = 0; j < 4; j++) {
               phy_copy.access.lane_mask = 0x1 << j;
               PHYMOD_IF_ERR_RETURN
                  (falcon_tsc_log_full_pmd_state(&phy_copy.access, &lane_st[j]));
             }
             phy_copy.access.lane_mask = tmp_lane_mask;
             PHYMOD_IF_ERR_RETURN
                (falcon_tsc_disp_full_pmd_state(&phy_copy.access, lane_st, 4));
             PHYMOD_IF_ERR_RETURN
                (falcon_tsc_read_event_log((&phy_copy.access), trace_mem, 2));
                 
    } else {
        for (i = 0; i < num_lane; i++) {
            phy_copy.access.lane_mask = 0x1 << (i + start_lane);
            
            switch (cmd_type) {
            case TEFMOD_DIAG_CFG:
                PHYMOD_DEBUG_ERROR((" %s:%d type = CFG\n", __func__, __LINE__));
                if(i==0) {
                    tmp_lane_mask = phy_copy.access.lane_mask;
                    phy_copy.access.lane_mask = 1;
                    PHYMOD_IF_ERR_RETURN
                        (falcon_tsc_display_core_config(&phy_copy.access));
                    phy_copy.access.lane_mask = tmp_lane_mask;
                }
                PHYMOD_IF_ERR_RETURN
                    (falcon_tsc_display_lane_config(&phy_copy.access));
                break;
                
            case TEFMOD_DIAG_CL72:
                PHYMOD_DEBUG_ERROR((" %s:%d type = CL72\n", __func__, __LINE__));
                PHYMOD_IF_ERR_RETURN
                    (falcon_tsc_display_cl93n72_status(&phy_copy.access));
                break;
                
            case TEFMOD_DIAG_DEBUG:
                PHYMOD_DEBUG_ERROR((" %s:%d type = DBG\n", __func__, __LINE__));
                PHYMOD_IF_ERR_RETURN
                    (falcon_tsc_display_lane_debug_status(&phy_copy.access));
                break;
                
            case TEFMOD_DIAG_BER:
                PHYMOD_DEBUG_ERROR((" %s:%d type = BER\n", __func__, __LINE__));
                break;
                
            /*
             * COVERITY
             *
             * TEFMOD_DIAG_ALL branch involve information in TEFMOD_DIAG_STATE branch
             */
            /* coverity[unterminated_case] */
            case TEFMOD_DIAG_ALL:
                PHYMOD_DEBUG_ERROR((" %s:%d type = CFG\n", __func__, __LINE__));
                if(i==0) {
                    tmp_lane_mask = phy_copy.access.lane_mask;
                    phy_copy.access.lane_mask = 1;
                    PHYMOD_IF_ERR_RETURN
                        (falcon_tsc_display_core_config(&phy_copy.access));
                    phy_copy.access.lane_mask = tmp_lane_mask;
                }
                PHYMOD_IF_ERR_RETURN
                    (falcon_tsc_display_lane_config(&phy_copy.access));

                PHYMOD_DEBUG_ERROR((" %s:%d type = CL72\n", __func__, __LINE__));
                PHYMOD_IF_ERR_RETURN
                    (falcon_tsc_display_cl93n72_status(&phy_copy.access));

                PHYMOD_DEBUG_ERROR((" %s:%d type = DBG\n", __func__, __LINE__));
                PHYMOD_IF_ERR_RETURN
                    (falcon_tsc_display_lane_debug_status(&phy_copy.access));

            case TEFMOD_DIAG_STATE:
            default:
                PHYMOD_DEBUG_ERROR((" %s:%d type = DEF\n", __func__, __LINE__));
                PHYMOD_IF_ERR_RETURN
                    (falcon_tsc_display_core_state(&phy_copy.access));
                PHYMOD_IF_ERR_RETURN
                    (falcon_tsc_display_lane_state_hdr(&phy_copy.access));
                PHYMOD_IF_ERR_RETURN
                    (falcon_tsc_display_lane_state(&phy_copy.access));
                tmp_lane_mask = phy_copy.access.lane_mask;
                for (j = 0; j < 4; j++) {
                     phy_copy.access.lane_mask = 0x1 << j;
                     PHYMOD_IF_ERR_RETURN
                         (falcon_tsc_log_full_pmd_state(&phy_copy.access, &lane_st[j]));
                 }
                phy_copy.access.lane_mask = tmp_lane_mask;
                PHYMOD_IF_ERR_RETURN
                    (falcon_tsc_disp_full_pmd_state(&phy_copy.access, lane_st, 4));
                PHYMOD_IF_ERR_RETURN
                    (falcon_tsc_read_event_log((&phy_copy.access), trace_mem, 2));
                break;
            }
        }
    }
    return PHYMOD_E_NONE;
}

STATIC err_code_t falcon_phy_meas_lowber_eye (const phymod_access_t *pa, 
                                     const phymod_phy_eyescan_options_t* eyescan_options,
                                     uint32_t *buffer) 
{
    struct falcon_tsc_eyescan_options_st e_options;

    e_options.linerate_in_khz = eyescan_options->linerate_in_khz;
    e_options.timeout_in_milliseconds = eyescan_options->timeout_in_milliseconds;
    e_options.horz_max = eyescan_options->horz_max;
    e_options.horz_min = eyescan_options->horz_min;
    e_options.hstep = eyescan_options->hstep;
    e_options.vert_max = eyescan_options->vert_max;
    e_options.vert_min = eyescan_options->vert_min;
    e_options.vstep = eyescan_options->vstep;
    e_options.mode = eyescan_options->mode;

    return (falcon_tsc_meas_lowber_eye(pa, e_options, buffer));
}

STATIC err_code_t falcon_phy_display_lowber_eye (const phymod_access_t *pa, 
                                        const phymod_phy_eyescan_options_t* eyescan_options,
                                        uint32_t *buffer) 
{
    struct falcon_tsc_eyescan_options_st e_options;

    e_options.linerate_in_khz = eyescan_options->linerate_in_khz;
    e_options.timeout_in_milliseconds = eyescan_options->timeout_in_milliseconds;
    e_options.horz_max = eyescan_options->horz_max;
    e_options.horz_min = eyescan_options->horz_min;
    e_options.hstep = eyescan_options->hstep;
    e_options.vert_max = eyescan_options->vert_max*2;
    e_options.vert_min = eyescan_options->vert_min*2;
    e_options.vstep = eyescan_options->vstep*2;
    e_options.mode = eyescan_options->mode;

    return(falcon_tsc_display_lowber_eye (pa, e_options, buffer)); 
} 

static void _falcon_diag_uc_reg_dump(const phymod_access_t *pa)
{
    err_code_t errc;

    COMPILER_REFERENCE(errc);

    PHYMOD_DIAG_OUT(("+-------------------------------------------------+\n"));
    PHYMOD_DIAG_OUT(("|    MICRO CODE USR CTRL CONFIGURATION REGISTERS  |\n"));
    PHYMOD_DIAG_OUT(("+-------------------------------------------------+\n"));
    PHYMOD_DIAG_OUT(("|    config_word              [0x00]: 0x%04X      |\n",    falcon_tsc_rdwl_uc_var(pa,&errc,0x0)));
    PHYMOD_DIAG_OUT(("|    retune_after_restart     [0x02]: 0x%04X      |\n",    falcon_tsc_rdbl_uc_var(pa,&errc,0x2)));
    PHYMOD_DIAG_OUT(("|    clk90_offset_adjust      [0x03]: 0x%04X      |\n",    falcon_tsc_rdbl_uc_var(pa,&errc,0x3)));
    PHYMOD_DIAG_OUT(("|    clk90_offset_override    [0x04]: 0x%04X      |\n",    falcon_tsc_rdbl_uc_var(pa,&errc,0x4)));
    PHYMOD_DIAG_OUT(("|    lane_event_log_level     [0x05]: 0x%04X      |\n",    falcon_tsc_rdbl_uc_var(pa,&errc,0x5)));
    PHYMOD_DIAG_OUT(("|    disable_startup          [0x06]: 0x%04X      |\n",    falcon_tsc_rdbl_uc_var(pa,&errc,0x6)));
    PHYMOD_DIAG_OUT(("|    disable_startup_dfe      [0x07]: 0x%04X      |\n",    falcon_tsc_rdbl_uc_var(pa,&errc,0x7)));
    PHYMOD_DIAG_OUT(("|    disable_steady_state     [0x08]: 0x%04X      |\n",    falcon_tsc_rdbl_uc_var(pa,&errc,0x8)));
    PHYMOD_DIAG_OUT(("|    disable_steady_state_dfe [0x09]: 0x%04X      |\n",    falcon_tsc_rdbl_uc_var(pa,&errc,0x9)));
    PHYMOD_DIAG_OUT(("+-------------------------------------------------+\n"));
    PHYMOD_DIAG_OUT(("|         MICRO CODE USER STATUS REGISTERS        |\n"));
    PHYMOD_DIAG_OUT(("+-------------------------------------------------+\n"));
    PHYMOD_DIAG_OUT(("|    restart_counter           [0x0A]: 0x%04X     |\n",    falcon_tsc_rdbl_uc_var(pa,&errc,0xa)));
    PHYMOD_DIAG_OUT(("|    reset_counter             [0x0B]: 0x%04X     |\n",    falcon_tsc_rdbl_uc_var(pa,&errc,0xb)));
    PHYMOD_DIAG_OUT(("|    pmd_lock_counter          [0x0C]: 0x%04X     |\n",    falcon_tsc_rdbl_uc_var(pa,&errc,0xc)));
    PHYMOD_DIAG_OUT(("|    heye_left                 [0x0D]: 0x%04X     |\n",    falcon_tsc_rdbl_uc_var(pa,&errc,0xd)));
    PHYMOD_DIAG_OUT(("|    heye_left_lsbyte          [0x0E]: 0x%04X     |\n",    falcon_tsc_rdbl_uc_var(pa,&errc,0xe)));
    PHYMOD_DIAG_OUT(("|    heye_right                [0x0F]: 0x%04X     |\n",    falcon_tsc_rdbl_uc_var(pa,&errc,0xf)));
    PHYMOD_DIAG_OUT(("|    heye_right_lsbyte         [0x10]: 0x%04X     |\n",    falcon_tsc_rdbl_uc_var(pa,&errc,0x10)));
    PHYMOD_DIAG_OUT(("|    veye_upper                [0x11]: 0x%04X     |\n",    falcon_tsc_rdbl_uc_var(pa,&errc,0x11)));
    PHYMOD_DIAG_OUT(("|    veye_upper_lsbyte         [0x12]: 0x%04X     |\n",    falcon_tsc_rdbl_uc_var(pa,&errc,0x12)));
    PHYMOD_DIAG_OUT(("|    veye_lower                [0x13]: 0x%04X     |\n",    falcon_tsc_rdbl_uc_var(pa,&errc,0x13)));
    PHYMOD_DIAG_OUT(("|    veye_lower_lsbyte         [0x14]: 0x%04X     |\n",    falcon_tsc_rdbl_uc_var(pa,&errc,0x14)));
    PHYMOD_DIAG_OUT(("|    micro_stopped             [0x15]: 0x%04X     |\n",    falcon_tsc_rdbl_uc_var(pa,&errc,0x15)));
    PHYMOD_DIAG_OUT(("|    link_time                 [0x16]: 0x%04X     |\n",    falcon_tsc_rdwl_uc_var(pa,&errc,0x16)));
    PHYMOD_DIAG_OUT(("+-------------------------------------------------+\n"));
    PHYMOD_DIAG_OUT(("|            MICRO CODE MISC REGISTERS            |\n"));
    PHYMOD_DIAG_OUT(("+-------------------------------------------------+\n"));
    PHYMOD_DIAG_OUT(("|    usr_diag_status           [0x18]: 0x%04X     |\n",    falcon_tsc_rdwl_uc_var(pa,&errc,0x18)));
    PHYMOD_DIAG_OUT(("|    usr_diag_rd_ptr           [0x1A]: 0x%04X     |\n",    falcon_tsc_rdbl_uc_var(pa,&errc,0x1a)));
    PHYMOD_DIAG_OUT(("|    usr_diag_mode             [0x1B]: 0x%04X     |\n",    falcon_tsc_rdbl_uc_var(pa,&errc,0x1b)));
    PHYMOD_DIAG_OUT(("|    usr_var_msb               [0x1C]: 0x%04X     |\n",    falcon_tsc_rdwl_uc_var(pa,&errc,0x1c)));
    PHYMOD_DIAG_OUT(("|    usr_var_lsb               [0x1E]: 0x%04X     |\n",    falcon_tsc_rdwl_uc_var(pa,&errc,0x1e)));
    PHYMOD_DIAG_OUT(("+-------------------------------------------------+\n"));
}

STATIC int falcon_diagnostics_eyescan_run_uc(const phymod_phy_access_t* phy, uint32_t flags)   
{
    int                 ii, rc = PHYMOD_E_NONE;
    uint32_t            stripe[64];
    uint16_t            status;
    int                 j ;
    phymod_phy_access_t phy_copy;
    
  PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));

  for(j=0; j< PHYMOD_CONFIG_MAX_LANES_PER_CORE; j++) { /* Loop for all lanes. */
    if ((phy->access.lane_mask & (1<<j))==0) continue;

    phy_copy.access.lane_mask = (phy->access.lane_mask & (1<<j));

    if(PHYMOD_EYESCAN_F_ENABLE_GET(flags)) {
        PHYMOD_IF_ERR_RETURN(falcon_tsc_meas_eye_scan_start(&(phy_copy.access), 0));

        if(!PHYMOD_EYESCAN_F_ENABLE_DONT_WAIT_GET(flags)) {
            PHYMOD_USLEEP(100000);
        }
    }

    if(PHYMOD_EYESCAN_F_PROCESS_GET(flags)) {

        for (ii = 62; ii >= -62; ii=ii-2) { 
            if (ii == 62) {
                PHYMOD_DIAG_OUT(("\n\n\n"));
                PHYMOD_DIAG_OUT(("    +--------------------------------------------------------------------+\n"));
                PHYMOD_DIAG_OUT(("    | EYESCAN Phy: 0x%03x lane_mask: 0x%02x                                 |\n", phy_copy.access.addr, phy_copy.access.lane_mask));
                PHYMOD_DIAG_OUT(("    +--------------------------------------------------------------------+\n"));
                /*display eyescan header*/
                PHYMOD_IF_ERR_RETURN(falcon_tsc_display_eye_scan_header(&(phy_copy.access), 1));
            }

            rc  = falcon_tsc_read_eye_scan_stripe(&(phy_copy.access), stripe, &status);
            if(rc != PHYMOD_E_NONE) {
                _falcon_diag_uc_reg_dump(&(phy_copy.access));
                PHYMOD_IF_ERR_RETURN(rc);
            }
            PHYMOD_IF_ERR_RETURN(falcon_tsc_display_eye_scan_stripe(&(phy_copy.access), ii, &stripe[0]));

            PHYMOD_DIAG_OUT(("\n"));
        }

        if (rc == PHYMOD_E_NONE) {
            PHYMOD_IF_ERR_RETURN(falcon_tsc_display_eye_scan_footer(&(phy_copy.access), 1));
            PHYMOD_DIAG_OUT(("\n"));
        }
    }
    
    if(PHYMOD_EYESCAN_F_DONE_GET(flags)) {
        PHYMOD_IF_ERR_RETURN(falcon_tsc_meas_eye_scan_done(&(phy_copy.access)));
    }
  }
    return PHYMOD_E_NONE;
}

STATIC int falcon_diagnostics_eyescan_run_lowber( 
    const phymod_phy_access_t* phy,
    uint32_t flags,
    const phymod_phy_eyescan_options_t* eyescan_options
    )   
{
    uint32_t *buffer;
    int buffer_size = 64*64;
    int rv = PHYMOD_E_NONE;

    buffer = PHYMOD_MALLOC(sizeof(uint32_t) * buffer_size, "buffer");
    if (NULL == buffer) {
        return PHYMOD_E_MEMORY;
    }
    PHYMOD_MEMSET(buffer, 0, sizeof(uint32_t) * buffer_size);

    /*enable eyescan*/
    if(PHYMOD_EYESCAN_F_PROCESS_GET(flags)) {

        rv = falcon_phy_meas_lowber_eye (&(phy->access), eyescan_options, buffer);
        if (rv < 0) {
            goto cleanup;
        }
        rv = falcon_phy_display_lowber_eye (&(phy->access), eyescan_options, buffer);
        if (rv < 0) {
            goto cleanup;
        }
    }
    if (buffer != NULL) {
        PHYMOD_FREE(buffer);
    }

    if(PHYMOD_EYESCAN_F_DONE_GET(flags)) {
        PHYMOD_IF_ERR_RETURN(falcon_tsc_pmd_uc_cmd(&(phy->access), CMD_CAPTURE_BER_END, 0, 2000));
    }

    return PHYMOD_E_NONE;

cleanup:
    if (buffer != NULL) {
        PHYMOD_FREE(buffer);
    }

    return rv;
}

STATIC int falcon_diagnostics_eye_margin_proj( const phymod_phy_access_t* phy, uint32_t flags,
                                        const phymod_phy_eyescan_options_t* eyescan_options) {
#ifdef SERDES_API_FLOATING_POINT
  int osr_mode;
  USR_DOUBLE data_rate, data_rate_in_Mhz;
  phymod_phy_access_t phy_copy;
  int start_lane, num_lane, i, found=0;

  struct   falcon_tsc_uc_core_config_st core_cfg;
  if(PHYMOD_EYESCAN_F_PROCESS_GET(flags)) {
     PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(&phy->access, &start_lane, &num_lane));
     PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));
     for (i = 0; i < num_lane; i++) {
         phy_copy.access.lane_mask = 1 << (start_lane + i);

         if (found == 0) {
             PHYMOD_IF_ERR_RETURN(falcon_tsc_get_uc_core_config (&(phy_copy.access), &core_cfg));
             PHYMOD_IF_ERR_RETURN(falcon_osr_mode_get(&phy_copy.access, &osr_mode));
             data_rate = (core_cfg.vco_rate_in_Mhz * 1000.0 * 1000.0) / (1<<osr_mode);
             data_rate_in_Mhz = (core_cfg.vco_rate_in_Mhz) / (1<<osr_mode);
             found = 1;
         }
         if(num_lane > 1) {
             PHYMOD_DIAG_OUT((" l=%0d, data rate  = %fMHz\n", i, data_rate_in_Mhz));
         } else {
             PHYMOD_DIAG_OUT((" data rate       = %fMHz\n", data_rate_in_Mhz));
         }
         falcon_tsc_eye_margin_proj(&phy_copy.access, data_rate, eyescan_options->ber_proj_scan_mode,
                                    eyescan_options->ber_proj_timer_cnt, eyescan_options->ber_proj_err_cnt);
     }
  }
#else
      PHYMOD_RETURN_WITH_ERR(PHYMOD_E_INTERNAL, (_PHYMOD_MSG("BER Proj is supported with SERDES_API_FLOATING_POINT only\n")));
#endif
  return PHYMOD_E_NONE;
}

int falcon_phy_eyescan_run(const phymod_phy_access_t* phy, 
                           uint32_t flags, 
                           phymod_eyescan_mode_t mode, 
                           const phymod_phy_eyescan_options_t* eyescan_options)
{
    uint8_t  pmd_rx_lock=0;

    PHYMOD_IF_ERR_RETURN
      (falcon_tsc_pmd_lock_status(&phy->access, &pmd_rx_lock));
    if(pmd_rx_lock == 0) {
      PHYMOD_RETURN_WITH_ERR(PHYMOD_E_INTERNAL, (_PHYMOD_MSG("Can not get eyescan when pmd_rx is not locked\n")));
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
            return falcon_diagnostics_eyescan_run_uc(phy, flags);
        case phymodEyescanModeLowBER:
            return falcon_diagnostics_eyescan_run_lowber(phy, flags, eyescan_options);
        case phymodEyescanModeBERProj:
            return falcon_diagnostics_eye_margin_proj(phy, flags, eyescan_options);
        default:
            PHYMOD_RETURN_WITH_ERR(PHYMOD_E_PARAM, (_PHYMOD_MSG("unsupported eyescan mode %u"), mode));
    }
}


#endif /* PHYMOD_FALCON_SUPPORT */
