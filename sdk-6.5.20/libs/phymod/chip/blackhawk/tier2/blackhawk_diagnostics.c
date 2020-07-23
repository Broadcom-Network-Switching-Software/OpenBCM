
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
#include <phymod/phymod_diag.h>
#include <phymod/phymod_diagnostics_dispatch.h>
#include <phymod/chip/blackhawk.h>
#include <phymod/chip/blackhawk_diagnostics.h>
#include "blackhawk/tier1/blackhawk_cfg_seq.h"
#include "blackhawk/tier1/common/srds_api_enum.h"
#include "blackhawk/tier1/blackhawk_tsc_enum.h"
#include "blackhawk/tier1/blackhawk_tsc_common.h"
#include "blackhawk/tier1/blackhawk_tsc_interface.h"
#include "blackhawk/tier1/blackhawk_tsc_dependencies.h"
#include "blackhawk/tier1/blackhawk_tsc_internal.h"
#include "blackhawk/tier1/public/blackhawk_api_uc_vars_rdwr_defns_public.h"
#include "blackhawk/tier1/blackhawk_tsc_access.h"
#include "blackhawk/tier1/blackhawk_tsc_types.h"

#ifdef PHYMOD_LINKCAT_BLACKHAWK_SUPPORT
#include "include/LinkCAT_lib.h"
#endif

#define PATTERN_MAX_LENGTH 240
#define BLACKHAWK_PRBS_MAX_ERROR_COUNT 0x7FFFFFFF
#ifdef PHYMOD_BLACKHAWK_SUPPORT


/*phymod, internal enum mappings*/
STATIC
int _blackhawk_prbs_poly_phymod_to_blackhawk(phymod_prbs_poly_t phymod_poly, enum srds_prbs_polynomial_enum *blackhawk_poly)
{
    switch(phymod_poly){
    case phymodPrbsPoly7:
        *blackhawk_poly = PRBS_7;
        break;
    case phymodPrbsPoly9:
        *blackhawk_poly = PRBS_9;
        break;
    case phymodPrbsPoly11:
        *blackhawk_poly = PRBS_11;
        break;
    case phymodPrbsPoly15:
        *blackhawk_poly = PRBS_15;
        break;
    case phymodPrbsPoly23:
        *blackhawk_poly = PRBS_23;
        break;
    case phymodPrbsPoly31:
        *blackhawk_poly = PRBS_31;
        break;
    case phymodPrbsPoly58:
        *blackhawk_poly = PRBS_58;
        break;
    case phymodPrbsPoly49:
        *blackhawk_poly = PRBS_49;
        break;
    case phymodPrbsPoly10:
        *blackhawk_poly = PRBS_10;
        break;
    case phymodPrbsPoly20:
        *blackhawk_poly = PRBS_20;
        break;
    case phymodPrbsPoly13:
        *blackhawk_poly = PRBS_13;
        break;
    default:
        PHYMOD_RETURN_WITH_ERR(PHYMOD_E_PARAM, (_PHYMOD_MSG("unsupported poly for blackhawk %u"), phymod_poly));
    }
    return PHYMOD_E_NONE;
}

STATIC
int _blackhawk_prbs_poly_blackhawk_to_phymod(enum srds_prbs_polynomial_enum  *blackhawk_poly, phymod_prbs_poly_t *phymod_poly)
{
    switch(*blackhawk_poly){
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
    case PRBS_49:
        *phymod_poly = phymodPrbsPoly49;
        break;
    case PRBS_10:
        *phymod_poly = phymodPrbsPoly10;
        break;
    case PRBS_20:
        *phymod_poly = phymodPrbsPoly20;
        break;
    case PRBS_13:
        *phymod_poly = phymodPrbsPoly13;
        break;
    default:
        PHYMOD_RETURN_WITH_ERR(PHYMOD_E_INTERNAL, (_PHYMOD_MSG("uknown poly %u"), *blackhawk_poly));
    }
    return PHYMOD_E_NONE;
}

int blackhawk_phy_rx_slicer_position_set(const phymod_phy_access_t* phy, uint32_t flags, const phymod_slicer_position_t* position)
{


    /* Not supported */
    PHYMOD_DEBUG_ERROR(("blackhawk_phy_rx_slicer_position_set function is NOT SUPPORTED!!\n"));


    return PHYMOD_E_NONE;

}

int blackhawk_phy_rx_slicer_position_get(const phymod_phy_access_t* phy, uint32_t flags, phymod_slicer_position_t* position)
{


    /* Not supported */
    PHYMOD_DEBUG_ERROR(("blackhawk_phy_rx_slicer_position_get function is NOT SUPPORTED!!\n"));


    return PHYMOD_E_NONE;

}


int blackhawk_phy_rx_slicer_position_max_get(const phymod_phy_access_t* phy, uint32_t flags, const phymod_slicer_position_t* position_min, const phymod_slicer_position_t* position_max)
{


    /* Not supported */
    PHYMOD_DEBUG_ERROR(("blackhawk_phy_rx_slicer_position_max_get function is NOT SUPPORTED!!\n"));


    return PHYMOD_E_NONE;

}


int blackhawk_phy_prbs_config_set(const phymod_phy_access_t* phy, uint32_t flags , const phymod_prbs_t* prbs)
{
    enum srds_prbs_polynomial_enum blackhawk_poly;
    phymod_phy_access_t phy_copy;
    int start_lane, num_lane, i;
    phymod_firmware_lane_config_t fw_config;

    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(&phy->access, &start_lane, &num_lane));
    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));

    /* next to see if the port is PAM4 or not */
    PHYMOD_IF_ERR_RETURN
        (blackhawk_phy_firmware_lane_config_get(phy, &fw_config));

    PHYMOD_IF_ERR_RETURN(_blackhawk_prbs_poly_phymod_to_blackhawk(prbs->poly, &blackhawk_poly));
    /*first check which direction */
    for (i = 0; i < num_lane; i++) {
        if (!PHYMOD_LANEPBMP_MEMBER(phy->access.lane_mask, start_lane + i)) {
            continue;
        }
        phy_copy.access.lane_mask = 1 << (start_lane + i);
        if (PHYMOD_PRBS_DIRECTION_RX_GET(flags)) {
            /* if PAM4 speed, use mode 2 */
            if (fw_config.ForcePAM4Mode) {
                PHYMOD_IF_ERR_RETURN
                    (blackhawk_tsc_config_rx_prbs(&phy_copy.access, blackhawk_poly,PRBS_INITIAL_SEED_NO_HYSTERESIS,  prbs->invert));
            } else {
                PHYMOD_IF_ERR_RETURN
                    (blackhawk_tsc_config_rx_prbs(&phy_copy.access, blackhawk_poly,PRBS_INITIAL_SEED_HYSTERESIS,  prbs->invert));
            }
        } else if (PHYMOD_PRBS_DIRECTION_TX_GET(flags)) {
            PHYMOD_IF_ERR_RETURN
                (blackhawk_tsc_config_tx_prbs(&phy_copy.access, blackhawk_poly, prbs->invert));
        } else {
            /* if PAM4 speed, use mode 2 */
            if (fw_config.ForcePAM4Mode) {
                PHYMOD_IF_ERR_RETURN
                    (blackhawk_tsc_config_rx_prbs(&phy_copy.access, blackhawk_poly,PRBS_INITIAL_SEED_NO_HYSTERESIS,  prbs->invert));
            } else {
                PHYMOD_IF_ERR_RETURN
                    (blackhawk_tsc_config_rx_prbs(&phy_copy.access, blackhawk_poly,PRBS_INITIAL_SEED_HYSTERESIS,  prbs->invert));
            }
            PHYMOD_IF_ERR_RETURN
                (blackhawk_tsc_config_tx_prbs(&phy_copy.access, blackhawk_poly, prbs->invert));
        }
    }
    return PHYMOD_E_NONE;
}

int blackhawk_phy_prbs_config_get(const phymod_phy_access_t* phy, uint32_t flags , phymod_prbs_t* prbs)
{
    phymod_prbs_t config_tmp;
    enum srds_prbs_polynomial_enum blackhawk_poly;
    enum srds_prbs_checker_mode_enum prbs_checker_mode;
    phymod_phy_access_t phy_copy;
    uint8_t invert;

    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));


    if (PHYMOD_PRBS_DIRECTION_TX_GET(flags)) {
        PHYMOD_IF_ERR_RETURN(blackhawk_tsc_get_tx_prbs_config(&phy_copy.access, &blackhawk_poly, &invert));
        config_tmp.invert = invert;
        PHYMOD_IF_ERR_RETURN(_blackhawk_prbs_poly_blackhawk_to_phymod(&blackhawk_poly, &config_tmp.poly));
        prbs->invert = config_tmp.invert;
        prbs->poly = config_tmp.poly;
    } else if (PHYMOD_PRBS_DIRECTION_RX_GET(flags)) {
        PHYMOD_IF_ERR_RETURN(blackhawk_tsc_get_rx_prbs_config(&phy_copy.access,
                                                                &blackhawk_poly,
                                                                &prbs_checker_mode,
                                                                &invert));
        config_tmp.invert = invert;
        PHYMOD_IF_ERR_RETURN(_blackhawk_prbs_poly_blackhawk_to_phymod(&blackhawk_poly, &config_tmp.poly));
        prbs->invert = config_tmp.invert;
        prbs->poly = config_tmp.poly;
    } else {
        PHYMOD_IF_ERR_RETURN(blackhawk_tsc_get_tx_prbs_config(&phy_copy.access, &blackhawk_poly,  &invert));
        config_tmp.invert = invert;
        PHYMOD_IF_ERR_RETURN(_blackhawk_prbs_poly_blackhawk_to_phymod(&blackhawk_poly, &config_tmp.poly));
        prbs->invert = config_tmp.invert;
        prbs->poly = config_tmp.poly;
    }
    return PHYMOD_E_NONE;
}

int blackhawk_phy_prbs_enable_set(const phymod_phy_access_t* phy, uint32_t flags , uint32_t enable)
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
            PHYMOD_IF_ERR_RETURN(blackhawk_tsc_tx_prbs_en(&phy_copy.access, enable));
        } else if (PHYMOD_PRBS_DIRECTION_RX_GET(flags)) {
            PHYMOD_IF_ERR_RETURN(blackhawk_tsc_rx_prbs_en(&phy_copy.access, enable));
        } else {
            PHYMOD_IF_ERR_RETURN(blackhawk_tsc_tx_prbs_en(&phy_copy.access, enable));
            PHYMOD_IF_ERR_RETURN(blackhawk_tsc_rx_prbs_en(&phy_copy.access, enable));
        }
    }
    return PHYMOD_E_NONE;
}

int blackhawk_phy_prbs_enable_get(const phymod_phy_access_t* phy, uint32_t flags , uint32_t* enable)
{
    uint8_t enable_tmp;
    phymod_phy_access_t phy_copy;

    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));

    if (PHYMOD_PRBS_DIRECTION_TX_GET(flags)) {
        PHYMOD_IF_ERR_RETURN(blackhawk_tsc_get_tx_prbs_en(&phy_copy.access, &enable_tmp));
        *enable = enable_tmp;
    } else if (PHYMOD_PRBS_DIRECTION_RX_GET(flags)) {
        PHYMOD_IF_ERR_RETURN(blackhawk_tsc_get_rx_prbs_en(&phy_copy.access, &enable_tmp));
        *enable = enable_tmp;
    } else {
        PHYMOD_IF_ERR_RETURN(blackhawk_tsc_get_tx_prbs_en(&phy_copy.access, &enable_tmp));
        *enable = enable_tmp;
        PHYMOD_IF_ERR_RETURN(blackhawk_tsc_get_rx_prbs_en(&phy_copy.access, &enable_tmp));
        *enable &= enable_tmp;
    }

    return PHYMOD_E_NONE;

}


int blackhawk_phy_prbs_status_get(const phymod_phy_access_t* phy, uint32_t flags, phymod_prbs_status_t* prbs_status)
{
    uint8_t status = 0, status_ori = 0;
    uint32_t prbs_err_count = 0, prbs_err_count_temp = 0;
    int i, start_lane, num_lane;
    phymod_phy_access_t phy_copy;
    enum srds_prbs_polynomial_enum blackhawk_poly;
    enum srds_prbs_checker_mode_enum prbs_checker_mode;
    uint8_t invert, chk_en;

    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));
    /* next figure out the lane num and start_lane based on the input */
    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(&phy->access, &start_lane, &num_lane));

    prbs_status->error_count = 0;
    prbs_status->prbs_lock_loss = 0;
    prbs_status->prbs_lock = 1;

    for (i = 0; i < num_lane; i++) {
        if (!PHYMOD_LANEPBMP_MEMBER(phy->access.lane_mask, start_lane + i)) {
            continue;
        }
        phy_copy.access.lane_mask = 0x1 << (i + start_lane);

        PHYMOD_IF_ERR_RETURN(blackhawk_tsc_get_rx_prbs_config(&phy_copy.access,
                                                              &blackhawk_poly,
                                                              &prbs_checker_mode,
                                                              &invert));
        if (prbs_checker_mode == PRBS_INITIAL_SEED_HYSTERESIS) {
            PHYMOD_IF_ERR_RETURN(blackhawk_tsc_prbs_chk_lock_state(&phy_copy.access, &status));
            if (status) {
                /*next check the lost of lock and error count */
                status = 0;
                PHYMOD_IF_ERR_RETURN
                        (blackhawk_tsc_prbs_err_count_state(&phy_copy.access, &prbs_err_count, &status));
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
        } else if (prbs_checker_mode == PRBS_INITIAL_SEED_NO_HYSTERESIS) {
            /* When PRBS_CHK_MODE is 2, PRBS checker will declare out-of-lock and
             * resync only if PRBS checker is re-enabled or CDR loses lock.
             * As a result, for this checker mode, we need to toggle the prbs_chk_en
             * bit to determine whether current PRBS is locked or not.
             */
            PHYMOD_IF_ERR_RETURN(blackhawk_tsc_get_rx_prbs_en(&phy_copy.access, &chk_en));
            if (!chk_en) {
                    PHYMOD_DEBUG_VERBOSE((" Lane :: %d PRBS not locked\n", i ));
                    prbs_status->prbs_lock = 0;
            } else {
                /* 1. Read current PRBS error count. If we confirm PRBS is locked and
                 * this value is not saturated, this will be the error count returned
                 * by this function.
                 */
                PHYMOD_IF_ERR_RETURN
                        (blackhawk_tsc_prbs_err_count_state(&phy_copy.access, &prbs_err_count, &status_ori));
                /* 2. Toggle prbs_chk_en bit. Read the PRBS counter to clear
                 * the counter and lock_loss status. Then check PRBS lock status.
                 */
                PHYMOD_IF_ERR_RETURN(blackhawk_tsc_rx_prbs_en(&phy_copy.access, 0));
                PHYMOD_IF_ERR_RETURN(blackhawk_tsc_rx_prbs_en(&phy_copy.access, 1));
                PHYMOD_IF_ERR_RETURN
                        (blackhawk_tsc_prbs_err_count_state(&phy_copy.access, &prbs_err_count_temp, &status));
                PHYMOD_IF_ERR_RETURN(blackhawk_tsc_prbs_chk_lock_state(&phy_copy.access, &status));
                /* 3. If current PRBS is locked while in step 1 we have counter
                 * value saturated or status_ori == 1 (PRBS lost lock),
                 * it means PRBS loses lock in between.
                 * Otherwise, PRBS is always locked and we take the counter
                 * value in step 1.
                 *
                 * If current PRBS is not locked, return prbs_lock = 0.
                 */
                if (status) {
                    if ((prbs_err_count == BLACKHAWK_PRBS_MAX_ERROR_COUNT) || status_ori) {
                        prbs_status->prbs_lock_loss = 1;
                    } else {
                        prbs_status->error_count += prbs_err_count;
                    }
                } else {
                    PHYMOD_DEBUG_VERBOSE((" Lane :: %d PRBS not locked\n", i ));
                    prbs_status->prbs_lock = 0;
                }
            }
        } else {
            return PHYMOD_E_UNAVAIL;
        }
    }

    return PHYMOD_E_NONE;

}


int blackhawk_phy_pattern_config_set(const phymod_phy_access_t* phy, const phymod_pattern_t* pattern)
{
    int i,j = 0, bit;
    char patt[PATTERN_MAX_LENGTH+1];
    phymod_phy_access_t phy_copy;

    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));

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
    /* coverity[divide_by_zero : FALSE] */
    PHYMOD_IF_ERR_RETURN
            (blackhawk_tsc_config_shared_tx_pattern (&phy_copy.access,
				(uint8_t) pattern->pattern_len, (const char *) patt));
    return PHYMOD_E_NONE;

}

int blackhawk_phy_pattern_config_get(const phymod_phy_access_t* phy, phymod_pattern_t* pattern)
{
    phymod_phy_access_t phy_copy;

    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));
    PHYMOD_IF_ERR_RETURN
        (blackhawk_tsc_config_shared_tx_pattern_idx_get(&phy_copy.access,
                                  &pattern->pattern_len,
                                  pattern->pattern));

    return PHYMOD_E_NONE;

}


int blackhawk_phy_pattern_enable_set(const phymod_phy_access_t* phy, uint32_t enable, const phymod_pattern_t* pattern)
{
    phymod_phy_access_t phy_copy;

    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));
    PHYMOD_IF_ERR_RETURN
        (blackhawk_tsc_tx_shared_patt_gen_en(&phy_copy.access, (uint8_t) enable, (uint8_t)pattern->pattern_len));
    return PHYMOD_E_NONE;

}

int blackhawk_phy_pattern_enable_get(const phymod_phy_access_t* phy, uint32_t* enable)
{
    phymod_phy_access_t phy_copy;
    uint8_t enable_8;

    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));
    PHYMOD_IF_ERR_RETURN
       (blackhawk_tsc_tx_shared_patt_gen_en_get(&phy_copy.access, &enable_8));

    *enable = enable_8;
    return PHYMOD_E_NONE;

}


int blackhawk_core_diagnostics_get(const phymod_core_access_t* core, phymod_core_diagnostics_t* diag)
{

    return PHYMOD_E_NONE;
}


int blackhawk_phy_diagnostics_get(const phymod_phy_access_t* phy, phymod_phy_diagnostics_t* diag)
{
    unsigned char rx_lock;
    int osr_mode;
    phymod_phy_access_t phy_copy;

    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));
    phymod_diag_eyescan_t_init(&diag->eyescan);
    phymod_diag_slicer_offset_t_init(&diag->slicer_offset);

    PHYMOD_IF_ERR_RETURN(blackhawk_tsc_pmd_lock_status(&phy_copy.access, &rx_lock));
    diag->rx_lock = (uint32_t ) rx_lock;
    PHYMOD_IF_ERR_RETURN(blackhawk_osr_mode_get(&phy_copy.access, &osr_mode));
    switch(osr_mode) {
        case 0: diag->osr_mode = phymodOversampleMode1; break;
        case 1: diag->osr_mode = phymodOversampleMode2; break;
        case 2: diag->osr_mode = phymodOversampleMode4; break;
        case 5: diag->osr_mode = phymodOversampleMode8; break;
        case 8: diag->osr_mode = phymodOversampleMode16P5; break;
        case 11: diag->osr_mode = phymodOversampleMode8P25; break;
        case 12: diag->osr_mode = phymodOversampleMode20P625; break;
        default:
            PHYMOD_RETURN_WITH_ERR(PHYMOD_E_INTERNAL, (_PHYMOD_MSG("unsupported OS mode %d"), osr_mode));
    }
    PHYMOD_IF_ERR_RETURN(blackhawk_tsc_signal_detect(&phy_copy.access, &diag->signal_detect));

    return PHYMOD_E_NONE;
}

int blackhawk_phy_pmd_info_dump(const phymod_phy_access_t* phy, const char* type)
{
    int start_lane, num_lane;
    uint32_t i, j, tmp_lane_mask;
    phymod_phy_access_t phy_copy;

    phymod_diag_type_t cmd_type;
    srds_info_t  *tsc_info_ptr = NULL;

    if (!type) {
        cmd_type = phymod_diag_DSC;
    } else if ((!PHYMOD_STRCMP(type, "ber")) ||
               (!PHYMOD_STRCMP(type, "Ber")) ||
               (!PHYMOD_STRCMP(type, "BER"))) {
        cmd_type = phymod_diag_BER;
    } else if ((!PHYMOD_STRCMP(type, "config")) ||
               (!PHYMOD_STRCMP(type, "Config")) ||
               (!PHYMOD_STRCMP(type, "CONFIG"))) {
        cmd_type = phymod_diag_CFG;
    } else if ((!PHYMOD_STRCMP(type, "cl72")) ||
               (!PHYMOD_STRCMP(type, "Cl72")) ||
               (!PHYMOD_STRCMP(type, "CL72"))) {
        cmd_type = phymod_diag_CL72;
    } else if ((!PHYMOD_STRCMP(type, "debug")) ||
               (!PHYMOD_STRCMP(type, "Debug")) ||
               (!PHYMOD_STRCMP(type, "DEBUG"))) {
        cmd_type = phymod_diag_DEBUG;
    } else if ((!PHYMOD_STRCMP(type, "state")) ||
               (!PHYMOD_STRCMP(type, "State")) ||
               (!PHYMOD_STRCMP(type, "STATE"))) {
        cmd_type = phymod_diag_STATE;
    } else if ((!PHYMOD_STRCMP(type, "state_eye")) ||
            (!PHYMOD_STRCMP(type, "State_Eye")) ||
            (!PHYMOD_STRCMP(type, "STATE_EYE"))) {
        cmd_type = phymod_diag_STATE_EYE;
    }else if ((!PHYMOD_STRCMP(type, "verbose")) ||
               (!PHYMOD_STRCMP(type, "Verbose")) ||
               (!PHYMOD_STRCMP(type, "VERBOSE"))) {
        cmd_type = phymod_diag_ALL;
    } else if (!PHYMOD_STRCMP(type, "STD")) {
        cmd_type = phymod_diag_DSC_STD;
    } else {
        cmd_type = phymod_diag_STATE;
    }

    PHYMOD_DEBUG_ERROR((" %s:%d type = %d laneMask  = 0x%X\n", __func__, __LINE__, cmd_type, phy->access.lane_mask));

    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));

    /* Make sure information table is initialized */
    tsc_info_ptr = blackhawk_tsc_INTERNAL_get_blackhawk_tsc_info_ptr(&phy_copy.access);
    if (tsc_info_ptr->signature == 0) {
     PHYMOD_IF_ERR_RETURN
            (blackhawk_tsc_init_blackhawk_tsc_info(&phy_copy.access));
    }
    /*next figure out the lane num and start_lane based on the input*/
    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(&phy->access, &start_lane, &num_lane));

    if (cmd_type == phymod_diag_DSC) {
       for (i = start_lane; i < (uint32_t)start_lane + num_lane; i++) {
            if (!PHYMOD_LANEPBMP_MEMBER(phy->access.lane_mask, i)) {
                continue;
            }
            phy_copy.access.lane_mask = 0x1 << i ;
            PHYMOD_IF_ERR_RETURN
                (blackhawk_tsc_display_diag_data(&phy_copy.access, SRDS_DIAG_CORE));
       }
    } else if (cmd_type == phymod_diag_DSC_STD) {

            PHYMOD_DIAG_OUT(("    +--------------------------------------------------------------------+\n"));
            PHYMOD_DIAG_OUT(("    | DSC Phy: 0x%03x lane_mask: 0x%02x                                 |\n", phy->access.addr, phy->access.lane_mask));
            PHYMOD_DIAG_OUT(("    +--------------------------------------------------------------------+\n"));
             tmp_lane_mask = phy_copy.access.lane_mask;
             for (j = 0; j < 8; j++) {
               phy_copy.access.lane_mask = 0x1 << j;
               PHYMOD_IF_ERR_RETURN
                  (blackhawk_tsc_display_diag_data(&phy_copy.access, SRDS_DIAG_CORE | SRDS_DIAG_LANE | SRDS_DIAG_EVENT));
             }

    } else if (cmd_type == phymod_diag_STATE_EYE) {
            PHYMOD_DEBUG_ERROR((" %s:%d type = DEF\n", __func__, __LINE__));
            for (j = 0; j < 8; j++) {
                 phy_copy.access.lane_mask = 0x1 << j;
                 PHYMOD_IF_ERR_RETURN
                    (blackhawk_tsc_display_diag_data(&phy_copy.access, SRDS_DIAG_CORE | SRDS_DIAG_LANE | SRDS_DIAG_EVENT | SRDS_DIAG_REG_LANE | SRDS_DIAG_REG_CORE | SRDS_DIAG_UC_LANE | SRDS_DIAG_UC_CORE | SRDS_DIAG_EYE));
             }
    } else {
        for (i = 0; i < (uint32_t)num_lane; i++) {
            if (!PHYMOD_LANEPBMP_MEMBER(phy->access.lane_mask, start_lane + i)) {
                continue;
            }
            phy_copy.access.lane_mask = 1 << (start_lane + i);

            switch (cmd_type) {
            case phymod_diag_CFG:
                PHYMOD_DEBUG_ERROR((" %s:%d type = CFG\n", __func__, __LINE__));
                if(i==0) {
                    tmp_lane_mask = phy_copy.access.lane_mask;
                    phy_copy.access.lane_mask = 1;
                    PHYMOD_IF_ERR_RETURN
                        (blackhawk_tsc_display_core_config(&phy_copy.access));
                    phy_copy.access.lane_mask = tmp_lane_mask;
                }
                PHYMOD_IF_ERR_RETURN
                    (blackhawk_tsc_display_lane_config(&phy_copy.access));
                break;

            case phymod_diag_DEBUG:
                PHYMOD_DEBUG_ERROR((" %s:%d type = DBG\n", __func__, __LINE__));
                PHYMOD_IF_ERR_RETURN
                    (blackhawk_tsc_display_lane_debug_status(&phy_copy.access));
                break;

            case phymod_diag_BER:
                PHYMOD_DEBUG_ERROR((" %s:%d type = BER\n", __func__, __LINE__));
                break;

            /*
             * COVERITY
             *
             * TEFMOD_DIAG_ALL branch involve information in TEFMOD_DIAG_STATE branch
             */
            /* coverity[unterminated_case] */
            case phymod_diag_ALL:
                PHYMOD_DEBUG_ERROR((" %s:%d type = CFG\n", __func__, __LINE__));
                if(i==0) {
                    tmp_lane_mask = phy_copy.access.lane_mask;
                    phy_copy.access.lane_mask = 1;
                    PHYMOD_IF_ERR_RETURN
                        (blackhawk_tsc_display_core_config(&phy_copy.access));
                    phy_copy.access.lane_mask = tmp_lane_mask;
                }
                PHYMOD_IF_ERR_RETURN
                    (blackhawk_tsc_display_lane_config(&phy_copy.access));

                PHYMOD_DEBUG_ERROR((" %s:%d type = DBG\n", __func__, __LINE__));
                PHYMOD_IF_ERR_RETURN
                    (blackhawk_tsc_display_lane_debug_status(&phy_copy.access));
                break;
            case phymod_diag_STATE:
            default:
                PHYMOD_DEBUG_ERROR((" %s:%d type = DEF\n", __func__, __LINE__));
                for (j = 0; j < 8; j++) {
                     phy_copy.access.lane_mask = 0x1 << j;
                     if (j==0){
                         PHYMOD_IF_ERR_RETURN
                            (blackhawk_tsc_display_diag_data(&phy_copy.access, SRDS_DIAG_CORE | SRDS_DIAG_LANE | SRDS_DIAG_EVENT | SRDS_DIAG_REG_LANE | SRDS_DIAG_REG_CORE | SRDS_DIAG_UC_LANE | SRDS_DIAG_UC_CORE));
                     } else {
                         PHYMOD_IF_ERR_RETURN
                            (blackhawk_tsc_display_diag_data(&phy_copy.access, SRDS_DIAG_LANE | SRDS_DIAG_EVENT | SRDS_DIAG_REG_LANE | SRDS_DIAG_UC_LANE ));
                     }
                 }
                break;
            }
        }
    }
    return PHYMOD_E_NONE;
}

static void _blackhawk_diag_uc_reg_dump(phymod_access_t *pa)
{
    err_code_t errc;

    COMPILER_REFERENCE(errc);

    PHYMOD_DIAG_OUT(("+-------------------------------------------------+\n"));
    PHYMOD_DIAG_OUT(("|    MICRO CODE USR CTRL CONFIGURATION REGISTERS  |\n"));
    PHYMOD_DIAG_OUT(("+-------------------------------------------------+\n"));
    PHYMOD_DIAG_OUT(("|    config_word              [0x00]: 0x%04X      |\n",    blackhawk_tsc_rdwl_uc_var(pa,&errc,0x0)));
    PHYMOD_DIAG_OUT(("|    usr_misc_ctrl_word       [0x04]: 0x%04X      |\n",    blackhawk_tsc_rdwl_uc_var(pa,&errc,0x4)));
    PHYMOD_DIAG_OUT(("|    retune_after_restart     [0x06]: 0x%04X      |\n",    blackhawk_tsc_rdbl_uc_var(pa,&errc,0x6)));
    PHYMOD_DIAG_OUT(("|    clk90_offset_adjust      [0x07]: 0x%04X      |\n",    blackhawk_tsc_rdbls_uc_var(pa,&errc,0x7)));
    PHYMOD_DIAG_OUT(("|    clk90_offset_override    [0x08]: 0x%04X      |\n",    blackhawk_tsc_rdbl_uc_var(pa,&errc,0x8)));
    PHYMOD_DIAG_OUT(("|    lane_event_log_level     [0x09]: 0x%04X      |\n",    blackhawk_tsc_rdbl_uc_var(pa,&errc,0x9)));
    PHYMOD_DIAG_OUT(("|    pam4_chn_loss            [0x0A]: 0x%04X      |\n",    blackhawk_tsc_rdbl_uc_var(pa,&errc,0xa)));
    PHYMOD_DIAG_OUT(("|    cl93n72_frc_byte         [0x0B]: 0x%04X      |\n",    blackhawk_tsc_rdbl_uc_var(pa,&errc,0xb)));
    PHYMOD_DIAG_OUT(("|    disable_startup          [0x0C]: 0x%04X      |\n",    blackhawk_tsc_rdwl_uc_var(pa,&errc,0xc)));
    PHYMOD_DIAG_OUT(("|    disable_steady_state     [0x0E]: 0x%04X      |\n",    blackhawk_tsc_rdwl_uc_var(pa,&errc,0xe)));
    PHYMOD_DIAG_OUT(("|    disable_startup_dfe      [0x10]: 0x%04X      |\n",    blackhawk_tsc_rdbl_uc_var(pa,&errc,0x10)));
    PHYMOD_DIAG_OUT(("|    disable_steady_state_dfe [0x11]: 0x%04X      |\n",    blackhawk_tsc_rdbl_uc_var(pa,&errc,0x11)));
    PHYMOD_DIAG_OUT(("+-------------------------------------------------+\n"));
    PHYMOD_DIAG_OUT(("|         MICRO CODE USER STATUS REGISTERS        |\n"));
    PHYMOD_DIAG_OUT(("+-------------------------------------------------+\n"));
    PHYMOD_DIAG_OUT(("|    restart_counter          [0x12]: 0x%04X     |\n",    blackhawk_tsc_rdbl_uc_var(pa,&errc,0x12)));
    PHYMOD_DIAG_OUT(("|    reset_counter            [0x13]: 0x%04X     |\n",    blackhawk_tsc_rdbl_uc_var(pa,&errc,0x13)));
    PHYMOD_DIAG_OUT(("|    pmd_lock_counter         [0x14]: 0x%04X     |\n",    blackhawk_tsc_rdbl_uc_var(pa,&errc,0x14)));
    PHYMOD_DIAG_OUT(("|    heye_left                [0x15]: 0x%04X     |\n",    blackhawk_tsc_rdbl_uc_var(pa,&errc,0x15)));
    PHYMOD_DIAG_OUT(("|    heye_right               [0x16]: 0x%04X     |\n",    blackhawk_tsc_rdbl_uc_var(pa,&errc,0x16)));
    PHYMOD_DIAG_OUT(("|    veye_upper               [0x17]: 0x%04X     |\n",    blackhawk_tsc_rdbl_uc_var(pa,&errc,0x17)));
    PHYMOD_DIAG_OUT(("|    veye_lower               [0x18]: 0x%04X     |\n",    blackhawk_tsc_rdbl_uc_var(pa,&errc,0x18)));
    PHYMOD_DIAG_OUT(("|    micro_stopped            [0x19]: 0x%04X     |\n",    blackhawk_tsc_rdbl_uc_var(pa,&errc,0x19)));
    PHYMOD_DIAG_OUT(("|    link_time                [0x1C]: 0x%04X     |\n",    blackhawk_tsc_rdwl_uc_var(pa,&errc,0x1c)));
    PHYMOD_DIAG_OUT(("+-------------------------------------------------+\n"));
    PHYMOD_DIAG_OUT(("|            MICRO CODE MISC REGISTERS            |\n"));
    PHYMOD_DIAG_OUT(("+-------------------------------------------------+\n"));
    PHYMOD_DIAG_OUT(("|    usr_diag_status          [0x1E]: 0x%04X     |\n",    blackhawk_tsc_rdwl_uc_var(pa,&errc,0x1e)));
    PHYMOD_DIAG_OUT(("|    usr_diag_rd_ptr          [0x20]: 0x%04X     |\n",    blackhawk_tsc_rdbl_uc_var(pa,&errc,0x20)));
    PHYMOD_DIAG_OUT(("|    usr_diag_mode            [0x21]: 0x%04X     |\n",    blackhawk_tsc_rdbl_uc_var(pa,&errc,0x21)));
    PHYMOD_DIAG_OUT(("|    usr_main_tap_est         [0x22]: 0x%04X     |\n",    blackhawk_tsc_rdwls_uc_var(pa,&errc,0x22)));
    PHYMOD_DIAG_OUT(("|    usr_sts_phase_hoffset    [0x24]: 0x%04X     |\n",    blackhawk_tsc_rdbls_uc_var(pa,&errc,0x24)));
    PHYMOD_DIAG_OUT(("|    usr_diag_wr_ptr          [0x25]: 0x%04X     |\n",    blackhawk_tsc_rdbl_uc_var(pa,&errc,0x25)));
    PHYMOD_DIAG_OUT(("|    status_byte              [0x26]: 0x%04X     |\n",    blackhawk_tsc_rdbl_uc_var(pa,&errc,0x26)));
    PHYMOD_DIAG_OUT(("+-------------------------------------------------+\n"));
}

STATIC int blackhawk_diagnostics_eyescan_run_uc(const phymod_phy_access_t* phy, uint32_t flags)
{
    int                 rc = PHYMOD_E_NONE;
    int                 j ;
    phymod_phy_access_t phy_copy;

    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));

    if(PHYMOD_EYESCAN_F_PROCESS_GET(flags)) {
        for(j=0; j< PHYMOD_CONFIG_MAX_LANES_PER_CORE; j++) { /* Loop for all lanes. */
            if ((phy->access.lane_mask & (1<<j))==0) continue;

            phy_copy.access.lane_mask = (phy->access.lane_mask & (1<<j));

            PHYMOD_DIAG_OUT(("\n\n\n"));
            PHYMOD_DIAG_OUT(("    +--------------------------------------------------------------------+\n"));
            PHYMOD_DIAG_OUT(("    | EYESCAN Phy: 0x%03x lane_mask: 0x%02x                                 |\n", phy_copy.access.addr, phy_copy.access.lane_mask));
            PHYMOD_DIAG_OUT(("    +--------------------------------------------------------------------+\n"));

            rc  = blackhawk_tsc_display_eye_scan(&(phy_copy.access));
            if(rc != PHYMOD_E_NONE) {
                _blackhawk_diag_uc_reg_dump(&(phy_copy.access));
                PHYMOD_IF_ERR_RETURN(rc);
            }
        }
    }
    return PHYMOD_E_NONE;
}

STATIC int blackhawk_diagnostics_eye_margin_proj( const phymod_phy_access_t* phy, uint32_t flags,
                                        const phymod_phy_eyescan_options_t* eyescan_options)
{
#ifdef SERDES_API_FLOATING_POINT
  USR_DOUBLE data_rate, data_rate_in_Mhz;
  phymod_phy_access_t phy_copy;
  int start_lane, num_lane, i, found=0;

  if(PHYMOD_EYESCAN_F_PROCESS_GET(flags)) {
     PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(&phy->access, &start_lane, &num_lane));
     PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));
     for (i = 0; i < num_lane; i++) {
         phy_copy.access.lane_mask = 1 << (start_lane + i);

         if (found == 0) {
             data_rate = eyescan_options->linerate_in_khz * 1000.0;
             data_rate_in_Mhz = data_rate / 1000.0 / 1000.0;;
             found = 1;
         }
         if(num_lane > 1) {
             PHYMOD_DIAG_OUT((" l=%0d, baud rate  = %fMB/s \n", i, data_rate_in_Mhz));
         } else {
             PHYMOD_DIAG_OUT((" baud rate       = %fMB/s \n", data_rate_in_Mhz));
         }
         PHYMOD_IF_ERR_RETURN(blackhawk_tsc_eye_margin_proj(&phy_copy.access, data_rate, eyescan_options->ber_proj_scan_mode,
                                    eyescan_options->ber_proj_timer_cnt, eyescan_options->ber_proj_err_cnt));
     }
  }
#else
      PHYMOD_RETURN_WITH_ERR(PHYMOD_E_INTERNAL, (_PHYMOD_MSG("BER Proj is supported with SERDES_API_FLOATING_POINT only\n")));
#endif

  return PHYMOD_E_NONE;
}

int blackhawk_phy_eyescan_run(const phymod_phy_access_t* phy,
                           uint32_t flags,
                           phymod_eyescan_mode_t mode,
                           const phymod_phy_eyescan_options_t* eyescan_options)
{
    uint8_t  pmd_rx_lock=0;
    phymod_phy_access_t phy_copy;

    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));

    PHYMOD_IF_ERR_RETURN
      (blackhawk_tsc_pmd_lock_status(&phy_copy.access, &pmd_rx_lock));

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
            return blackhawk_diagnostics_eyescan_run_uc(phy, flags);
        case phymodEyescanModeBERProj:
            return blackhawk_diagnostics_eye_margin_proj(phy, flags, eyescan_options);
        default:
            PHYMOD_RETURN_WITH_ERR(PHYMOD_E_PARAM, (_PHYMOD_MSG("unsupported eyescan mode %u"), mode));
    }

  return PHYMOD_E_NONE;
}

int blackhawk_phy_PAM4_tx_pattern_enable_set(const phymod_phy_access_t* phy, phymod_PAM4_tx_pattern_t pattern_type, uint32_t enable)
{
    phymod_phy_access_t phy_copy;

    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));

    switch (pattern_type) {
    case phymod_PAM4TxPattern_JP03B:
        PHYMOD_IF_ERR_RETURN
            (blackhawk_tsc_config_tx_jp03b_pattern(&phy_copy.access, (uint8_t) enable));
        break;
    case phymod_PAM4TxPattern_Linear:
        PHYMOD_IF_ERR_RETURN
            (blackhawk_tsc_config_tx_linearity_pattern(&phy_copy.access, (uint8_t) enable));
        break;
    default:
        PHYMOD_RETURN_WITH_ERR(PHYMOD_E_PARAM, (_PHYMOD_MSG("unsupported PAM4 tx pattern  %u"), pattern_type));
    }
    return PHYMOD_E_NONE;

}

int blackhawk_phy_PAM4_tx_pattern_enable_get(const phymod_phy_access_t* phy, phymod_PAM4_tx_pattern_t pattern_type, uint32_t* enable)
{
    phymod_phy_access_t phy_copy;

    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));

    PHYMOD_IF_ERR_RETURN
        (blackhawk_tsc_pam4_tx_pattern_enable_get(&phy_copy.access, pattern_type, enable));
    return PHYMOD_E_NONE;

}

STATIC
int _blackhawk_phy_post_fec_ber_proj(const phymod_phy_access_t* phy, const phymod_phy_ber_proj_options_t* options)
{
    phymod_phy_access_t phy_copy;
    uint8_t default_hist_errcnt_thresh = 1;
    uint8_t default_prbs_errcnt_thresh;
    uint8_t hist_errcnt_thresh = 0, prbs_errcnt_thresh = 0;
    int timeout_s;
    blackhawk_tsc_prbs_err_analyzer_errcnt_st err_analyzer;
    int start_lane, num_lane, i;
#ifdef SERDES_API_FLOATING_POINT
    USR_DOUBLE ber;
    blackhawk_tsc_prbs_err_analyzer_report_st err_analyzer_rep;
    uint16_t x = 0, y = 0, z = 0;
#endif /* SERDES_API_FLOATING_POINT */

    PHYMOD_IF_ERR_RETURN(phymod_util_lane_config_get(&phy->access, &start_lane, &num_lane));
    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));

    switch (options->ber_proj_phase) {
        case PHYMOD_BER_PROJ_PHASE_F_PRE:
            /* This is pre-config stage. Only needed when
             * options->ber_proj_hist_errcnt_thresh == 0.
             */
            if (options->ber_proj_fec_size == 0) {
                /* If user use unsupported FEC type, Portmod will set fec_size to 0.
                 * So here we check whether fec_size equals to 0. If so, return error.
                 */
                PHYMOD_RETURN_WITH_ERR(PHYMOD_E_PARAM,
                                       (_PHYMOD_MSG("Unsupported FEC type for Post FEC BER Projection.")));
            }

            default_prbs_errcnt_thresh = default_hist_errcnt_thresh + 8;
            for (i = 0; i < num_lane; i++) {
                phy_copy.access.lane_mask = 1 << (start_lane + i);
                /* Configure PRBS Error Analyzer */
                PHYMOD_IF_ERR_RETURN
                    (blackhawk_tsc_prbs_error_analyzer_config(&phy_copy.access,
                                                              options->ber_proj_fec_size,
                                                              default_prbs_errcnt_thresh,
                                                              default_hist_errcnt_thresh));
            }
            break;
        case PHYMOD_BER_PROJ_PHASE_F_CONFIG:
            if (options->ber_proj_fec_size == 0) {
                /* If user use unsupported FEC type, Portmod will set fec_size to 0.
                 * So here we check whether fec_size equals to 0. If so, return error.
                 */
                PHYMOD_RETURN_WITH_ERR(PHYMOD_E_PARAM, (_PHYMOD_MSG("Unsupported FEC type for Post FEC BER Projection.")));
            }
            /* PLP team recommend 5% proj_timeout value as the timeout_s to get optimized thershold */
            /* 99 is used to generate CEIL function of 5% of timeout_s */
            timeout_s = (options->ber_proj_timeout_s * 5 + 99)/ 100;

            for (i = 0; i < num_lane; i++) {
                phy_copy.access.lane_mask = 1 << (start_lane + i);
                if (options->without_print != 1) {
                    PHYMOD_DIAG_OUT(("\nConfigure PRBS Error Analyzer: Phy: 0x%03x, Lane: 0x%01x", phy->access.addr, start_lane+i));
                }
                if ((options->ber_proj_hist_errcnt_thresh == 0)) {
                    /* Use optimized Hist_Errcnt_Threshold */
                    /* Initialize err cnt */
                    PHYMOD_MEMSET(&err_analyzer, 0, sizeof(blackhawk_tsc_prbs_err_analyzer_errcnt_st));
                    /* Get the error count */
                    PHYMOD_IF_ERR_RETURN
                        (blackhawk_tsc_prbs_error_analyzer_accumulate_err_count(&phy_copy.access, &err_analyzer));
                    if (options->without_print != 1) {
                        PHYMOD_DIAG_OUT(("\n------------------------------------------------------------------------------------------------\n"));
                        /* Get the optimized hist errcnt threshold. */
                        PHYMOD_IF_ERR_RETURN
                            (blackhawk_tsc_optimize_hist_errcnt_thresh(&phy_copy.access,
                                                                       &err_analyzer,
                                                                       timeout_s,
                                                                       options->ber_proj_timeout_s));
                        PHYMOD_DIAG_OUT(("------------------------------------------------------------------------------------------------\n"));
                    } else {
                        /* Get the optimized hist errcnt threshold. */
                        PHYMOD_IF_ERR_RETURN
                            (blackhawk_tsc_optimize_hist_errcnt_thresh_get(&phy_copy.access,
                                                                   &err_analyzer,
                                                                   timeout_s,
                                                                   options->ber_proj_timeout_s));
                    }
                    /* Get the optimized hist errcnt threshold. */
                    hist_errcnt_thresh = err_analyzer.hist_errcnt_thresh;
                } else if (options->ber_proj_hist_errcnt_thresh > 7) {
                    PHYMOD_RETURN_WITH_ERR(PHYMOD_E_PARAM, (_PHYMOD_MSG("hist_errcnt_threshold is out of range. Valid range is [1,7].")));
                } else {
                    hist_errcnt_thresh = options->ber_proj_hist_errcnt_thresh;
                }
                prbs_errcnt_thresh = hist_errcnt_thresh + 8;
                PHYMOD_IF_ERR_RETURN
                    (blackhawk_tsc_prbs_error_analyzer_config(&phy_copy.access,
                                                              options->ber_proj_fec_size,
                                                              prbs_errcnt_thresh,
                                                              hist_errcnt_thresh));
                if (options->without_print != 1) {
                    PHYMOD_IF_ERR_RETURN
                        (blackhawk_tsc_display_prbs_error_analyzer_config(&phy_copy.access, prbs_errcnt_thresh, hist_errcnt_thresh, options->ber_proj_timeout_s));
                } else {
                    uint16_t fec_size = 0;
                    PHYMOD_IF_ERR_RETURN
                        (blackhawk_tsc_get_prbs_error_analyzer_config(&phy_copy.access, &fec_size, &prbs_errcnt_thresh, &hist_errcnt_thresh));
                }
            }
            break;
        case PHYMOD_BER_PROJ_PHASE_F_START:
            for (i = 0; i < num_lane; i++) {
                phy_copy.access.lane_mask = 1 << (start_lane + i);
                PHYMOD_IF_ERR_RETURN(blackhawk_tsc_error_analyzer_status_clear(&phy_copy.access));
            }
            break;
        case PHYMOD_BER_PROJ_PHASE_F_COLLECT:
            for (i = 0; i < num_lane; i++) {
                phy_copy.access.lane_mask = 1 << (start_lane + i);
                PHYMOD_MEMSET(&err_analyzer, 0, sizeof(blackhawk_tsc_prbs_err_analyzer_errcnt_st));
                PHYMOD_MEMCPY(err_analyzer.prbs_errcnt, options->ber_proj_prbs_errcnt[i].prbs_errcnt, sizeof(err_analyzer.prbs_errcnt));
                PHYMOD_IF_ERR_RETURN(
                    blackhawk_tsc_prbs_error_analyzer_accumulate_err_count(&phy_copy.access, &err_analyzer));
                PHYMOD_MEMCPY(options->ber_proj_prbs_errcnt[i].prbs_errcnt, err_analyzer.prbs_errcnt, sizeof(err_analyzer.prbs_errcnt));
            }
            break;
        case PHYMOD_BER_PROJ_PHASE_F_CAL:
            for (i = 0; i < num_lane; i++) {
                phy_copy.access.lane_mask = 1 << (start_lane + i);
                PHYMOD_MEMSET(&err_analyzer, 0, sizeof(blackhawk_tsc_prbs_err_analyzer_errcnt_st));
                if ((options->ber_proj_hist_errcnt_thresh == 0)) {
                    PHYMOD_IF_ERR_RETURN(
                        blackhawk_tsc_prbs_error_analyzer_accumulate_err_count(&phy_copy.access, &err_analyzer));
                } else {
                    err_analyzer.hist_errcnt_thresh = options->ber_proj_hist_errcnt_thresh;
                    err_analyzer.prbs_errcnt_thresh = options->ber_proj_hist_errcnt_thresh + 8;
                }
                PHYMOD_MEMCPY(err_analyzer.prbs_errcnt, options->ber_proj_prbs_errcnt[i].prbs_errcnt, sizeof(err_analyzer.prbs_errcnt));
               if (options->without_print != 1) {
                    PHYMOD_DIAG_OUT(("\nPost-FEC BER: Phy: 0x%03x, Lane: 0x%01x", phy->access.addr, start_lane+i));
                    PHYMOD_IF_ERR_RETURN(blackhawk_tsc_display_prbs_error_analyzer_err_count(&phy_copy.access, err_analyzer));
                    PHYMOD_IF_ERR_RETURN(
                        blackhawk_tsc_prbs_error_analyzer_compute_proj(&phy_copy.access, err_analyzer, options->ber_proj_timeout_s));
                }
#ifdef SERDES_API_FLOATING_POINT
                else {
                    PHYMOD_IF_ERR_RETURN(
                        blackhawk_tsc_prbs_error_analyzer_report_proj(&phy_copy.access, err_analyzer, options->ber_proj_timeout_s, &err_analyzer_rep));
                    /* save ber x.y e-z  in uint32: bit 31-bit 24 : x value, bit23 - bit16: y value, bit15-bit 0: z value */
                    /* for example, if the BER is 4.3e-9, then x=4, y=3 and z=9 and The expected returned value will be 0x04030009*/
                    if (err_analyzer_rep.delta_n >=2) {
                        z = 0; x = 0, y = 0;
                        ber = err_analyzer_rep.proj_ber;
                        while(((int)ber) <=0 ) {
                            ber = ber * 10;
                            z++;
                        }
                        x = (uint16_t)ber;
                        /* keep 2 digits for y */
                        y = (ber - (USR_DOUBLE)x)*100 + 0.5;
                        /* printf("\n  BER = %0.2e\n\n",err_analyzer_rep.proj_ber); */
                        options->ber_proj_prbs_errcnt[i].prbs_proj_ber = x << 24 | y << 16 | z;
                    }
                }
#else
                options->ber_proj_prbs_errcnt[i].prbs_proj_ber = 0;
#endif /* SERDES_API_FLOATING_POINT */
                options->ber_proj_prbs_errcnt[i].start_hist_errcnt_thresh = err_analyzer.hist_errcnt_thresh;
                options->ber_proj_prbs_errcnt[i].max_errcnt_thresh = err_analyzer.prbs_errcnt_thresh;
            }
            break;
        default:
            return PHYMOD_E_PARAM;
    }
    return PHYMOD_E_NONE;
}

/* BER Projection*/
int blackhawk_phy_ber_proj(const phymod_phy_access_t* phy, phymod_ber_proj_mode_t mode, const phymod_phy_ber_proj_options_t* options)
{
    switch (mode) {
        case phymodBERProjModePostFEC:
            PHYMOD_IF_ERR_RETURN(_blackhawk_phy_post_fec_ber_proj(phy, options));
            break;
        default:
            PHYMOD_RETURN_WITH_ERR(PHYMOD_E_PARAM, (_PHYMOD_MSG("unsupported BER PROJECTION mode  %u"), mode));
    }
    return PHYMOD_E_NONE;
}

/* fast BER Projection get*/
int blackhawk_phy_fast_ber_proj_get(const phymod_phy_access_t* phy, uint32_t* ber_proj_data)
{
    struct ber_data_st ber_data_local;
    int start_lane, num_lane;
    phymod_phy_access_t phy_copy;

    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));

    /* next figure out the lane num and start_lane based on the input */
    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(&phy->access, &start_lane, &num_lane));

    phy_copy.access.lane_mask = 0x1 << start_lane;

    /* collect prbs in 100 ms */
    PHYMOD_IF_ERR_RETURN
        (blackhawk_tsc_INTERNAL_get_BER_data(&phy_copy.access, 100, &ber_data_local, 0));

    /* first check if PRBS enabled or prbs lost lock */
    if ((ber_data_local.prbs_chk_en == 0) || (ber_data_local.lcklost == 1)) {
        *ber_proj_data = 0xffffffff;
    } else {
        uint16_t x = 0,y = 0,z = 0,division;

        if (COMPILER_64_GE(ber_data_local.num_errs, ber_data_local.num_bits)) {
            x = 1;
            y = 0;
            z = 0;
        } else {
            uint64_t tmp_num_errs, tmp_num_bits, tmp_div;
            while (1) {
                /*
                 * division = (uint16_t)(((ber_data_local.num_errs<<1) + ber_data_local.num_bits)/(ber_data_local.num_bits<<1));
                 */
                COMPILER_64_COPY(tmp_num_errs, ber_data_local.num_errs);
                COMPILER_64_COPY(tmp_num_bits, ber_data_local.num_bits);

                /* first check if the  number of error is 0 or the error is too small */
                if (COMPILER_64_IS_ZERO(tmp_num_errs) || (z > 40))  {
                    /* the prbs error count is too small to have a meaningful estimate, return 0 */
                    *ber_proj_data = 0;
                    return PHYMOD_E_NONE;
                }
                /* ber_data_local.num_errs << 1 */
                COMPILER_64_SHL(tmp_num_errs, 1);

                /* (ber_data_local.num_errs << 1) + ber_data_local.num_bits */
                COMPILER_64_ADD_64(tmp_num_errs, ber_data_local.num_bits);

                /* ber_data_local.num_bits << 1 */
                COMPILER_64_SHL(tmp_num_bits, 1);

                COMPILER_64_COPY(tmp_div, tmp_num_errs);
                COMPILER_64_UDIV_64(tmp_div, tmp_num_bits);
                division = (uint16_t)(COMPILER_64_LO(tmp_div));

                if (division >= 10) break;
                /*
                 * ber_data_local.num_errs = ber_data_local.num_errs*10;
                 */
                COMPILER_64_UMUL_32(ber_data_local.num_errs, (uint32_t) 10);
                z = z + 1;
            }
            if(division >= 100) {
                division = division / 10;
                z = z - 1;
            }
            x = division / 10;
            y = division - 10 * x;
            z = z - 1;
        }
        *ber_proj_data = x << 24 | y << 16 | z;
    }

    return PHYMOD_E_NONE;
}

int blackhawk_phy_pmd_lane_diag_debug_level_set(const phymod_phy_access_t* phy, uint32_t level)
{
    phymod_phy_access_t phy_copy;
    int start_lane, num_lane, i;

    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));
    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(&phy->access, &start_lane, &num_lane));

    for (i = 0; i < num_lane; i++) {
        phy_copy.access.lane_mask = 1 << (start_lane + i);
        if (!PHYMOD_LANEPBMP_MEMBER(phy->access.lane_mask, start_lane + i)) {
            continue;
        }
        PHYMOD_IF_ERR_RETURN
            (blackhawk_tsc_set_usr_ctrl_lane_event_log_level(&phy_copy.access, level));
    }

    return PHYMOD_E_NONE;
}

int blackhawk_phy_pmd_lane_diag_debug_level_get(const phymod_phy_access_t* phy, uint32_t *level)
{
    phymod_phy_access_t phy_copy;
    uint8_t level_temp;

    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));
    PHYMOD_IF_ERR_RETURN
        (blackhawk_tsc_get_usr_ctrl_lane_event_log_level(&phy_copy.access, &level_temp));

    *level = level_temp;
    return PHYMOD_E_NONE;
}

int blackhawk_phy_linkcat(const phymod_phy_access_t* phy, phymod_linkCAT_op_mode_t mode) {

#ifdef PHYMOD_LINKCAT_BLACKHAWK_SUPPORT
    linkcat_config linkcat_config;
    linkcat_returns linkcat_status;
    phymod_access_t phy_acc;

    PHYMOD_MEMCPY(&phy_acc, &phy->access, sizeof(phymod_access_t));

    linkcat_status.insertion_loss = 0.0;
    linkcat_status.fit_factor = 0.0;
    linkcat_config.mode = mode;
    linkcat_config.portaddress = phy->access.addr;
    linkcat_config.dir_name = NULL;
    linkcat_config.file_prefix = NULL;

    return(blackhawk_tsc_LinkCat(&phy_acc, &linkcat_config, &linkcat_status));
#else
    return PHYMOD_E_UNAVAIL;
#endif /* PHYMOD_LINKCAT_BLACKHAWK_SUPPORT */

}

int blackhawk_phy_pmd_usr_misc_ctrl_set(const phymod_phy_access_t* phy,
                                        phymod_phy_pmd_usr_misc_ctrl_t mode,
                                        uint32_t value)
{
    phymod_phy_access_t phy_copy;
    int start_lane, num_lane, i;
    uint32_t bit_position = 0;

    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));
    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(&phy->access, &start_lane, &num_lane));

    switch (mode) {
        case phymodMiscCtrlAutoNegForceER:
            bit_position = BLACKHAWK_USR_MISC_CTRL_FORCE_ER;
            break;
        case phymodMiscCtrlAutoNegForceNR:
            bit_position = BLACKHAWK_USR_MISC_CTRL_FORCE_NR;
            break;
        default:
            break;
    }

    for (i = 0; i < num_lane; i++) {
        phy_copy.access.lane_mask = 1 << (start_lane + i);
        if (!PHYMOD_LANEPBMP_MEMBER(phy->access.lane_mask, start_lane + i)) {
            continue;
        }
        PHYMOD_IF_ERR_RETURN
            (blackhawk_tsc_usr_misc_ctrl_set(&phy_copy.access,
                                             bit_position, value));
    }

    return PHYMOD_E_NONE;
}

int blackhawk_phy_pmd_usr_misc_ctrl_get(const phymod_phy_access_t* phy,
                                        phymod_phy_pmd_usr_misc_ctrl_t mode,
                                        uint32_t *value)
{
    phymod_phy_access_t phy_copy;
    uint32_t bit_position = 0;

    switch (mode) {
        case phymodMiscCtrlAutoNegForceER:
            bit_position = BLACKHAWK_USR_MISC_CTRL_FORCE_ER;
            break;
        case phymodMiscCtrlAutoNegForceNR:
            bit_position = BLACKHAWK_USR_MISC_CTRL_FORCE_NR;
            break;
        default:
            break;
    }

    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));
    PHYMOD_IF_ERR_RETURN
        (blackhawk_tsc_usr_misc_ctrl_get(&phy_copy.access,
                                         bit_position, (uint8_t *)value));

    return PHYMOD_E_NONE;
}

#endif /* PHYMOD_BLACKHAWK_SUPPORT */
