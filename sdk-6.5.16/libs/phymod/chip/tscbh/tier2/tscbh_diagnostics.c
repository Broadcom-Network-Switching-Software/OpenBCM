
/*
 *
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2019 Broadcom Inc. All rights reserved.
 */

#include <phymod/phymod.h>
#include <phymod/phymod_system.h>
#include <phymod/phymod_util.h>
#include <phymod/phymod_config.h>
#include <phymod/phymod_diagnostics.h>
#include <phymod/phymod_diagnostics_dispatch.h>
#include <phymod/phymod_acc.h>
#include <phymod/chip/tscbh.h>
#include <phymod/chip/tscbh_diagnostics.h>

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
#include "tscbh/tier1/tbhmod.h"


#define PATTERN_MAX_LENGTH 240
#ifdef PHYMOD_BLACKHAWK_SUPPORT


/*phymod, internal enum mappings*/
STATIC
int _tscbh_prbs_poly_phymod_to_blackhawk(phymod_prbs_poly_t phymod_poly, enum srds_prbs_polynomial_enum *blackhawk_poly)
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
        PHYMOD_RETURN_WITH_ERR(PHYMOD_E_PARAM, (_PHYMOD_MSG("unsupported poly for tscf %u"), phymod_poly));
    }
    return PHYMOD_E_NONE;
}

STATIC
int _tscbh_prbs_poly_tscbh_to_phymod(enum srds_prbs_polynomial_enum  *blackhawk_poly, phymod_prbs_poly_t *phymod_poly)
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


int tscbh_phy_rx_slicer_position_set(const phymod_phy_access_t* phy, uint32_t flags, const phymod_slicer_position_t* position)
{
    /* Not supported */
    PHYMOD_DEBUG_ERROR(("tscbh_phy_rx_slicer_position_set function is NOT SUPPORTED!!\n"));

    return PHYMOD_E_NONE;
}

int tscbh_phy_rx_slicer_position_get(const phymod_phy_access_t* phy, uint32_t flags, phymod_slicer_position_t* position)
{
    /* Not supported */
    PHYMOD_DEBUG_ERROR(("tscbh_phy_rx_slicer_position_get function is NOT SUPPORTED!!\n"));

    return PHYMOD_E_NONE;
}


int tscbh_phy_rx_slicer_position_max_get(const phymod_phy_access_t* phy, uint32_t flags, const phymod_slicer_position_t* position_min, const phymod_slicer_position_t* position_max)
{
    /* Not supported */
    PHYMOD_DEBUG_ERROR(("tscbh_phy_rx_slicer_position_max_get function is NOT SUPPORTED!!\n"));

    return PHYMOD_E_NONE;
}


int tscbh_phy_prbs_config_set(const phymod_phy_access_t* phy, uint32_t flags , const phymod_prbs_t* prbs)
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
        (tscbh_phy_firmware_lane_config_get(phy, &fw_config));

    PHYMOD_IF_ERR_RETURN(_tscbh_prbs_poly_phymod_to_blackhawk(prbs->poly, &blackhawk_poly));
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

int tscbh_phy_prbs_config_get(const phymod_phy_access_t* phy, uint32_t flags , phymod_prbs_t* prbs)
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
        PHYMOD_IF_ERR_RETURN(_tscbh_prbs_poly_tscbh_to_phymod(&blackhawk_poly, &config_tmp.poly));
        prbs->invert = config_tmp.invert;
        prbs->poly = config_tmp.poly;
    } else if (PHYMOD_PRBS_DIRECTION_RX_GET(flags)) {
        PHYMOD_IF_ERR_RETURN(blackhawk_tsc_get_rx_prbs_config(&phy_copy.access,
                                                                &blackhawk_poly,
                                                                &prbs_checker_mode,
                                                                &invert));
        config_tmp.invert = invert;
        PHYMOD_IF_ERR_RETURN(_tscbh_prbs_poly_tscbh_to_phymod(&blackhawk_poly, &config_tmp.poly));
        prbs->invert = config_tmp.invert;
        prbs->poly = config_tmp.poly;
    } else {
        PHYMOD_IF_ERR_RETURN(blackhawk_tsc_get_tx_prbs_config(&phy_copy.access, &blackhawk_poly,  &invert));
        config_tmp.invert = invert;
        PHYMOD_IF_ERR_RETURN(_tscbh_prbs_poly_tscbh_to_phymod(&blackhawk_poly, &config_tmp.poly));
        prbs->invert = config_tmp.invert;
        prbs->poly = config_tmp.poly;
    }
    return PHYMOD_E_NONE;
}

int tscbh_phy_prbs_enable_set(const phymod_phy_access_t* phy, uint32_t flags , uint32_t enable)
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

int tscbh_phy_prbs_enable_get(const phymod_phy_access_t* phy, uint32_t flags , uint32_t* enable)
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


int tscbh_phy_prbs_status_get(const phymod_phy_access_t* phy, uint32_t flags, phymod_prbs_status_t* prbs_status)
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
    }
    return PHYMOD_E_NONE;
}

#if 0
int tscbh_phy_pattern_config_set(const phymod_phy_access_t* phy, const phymod_pattern_t* pattern)
{
    phymod_phy_access_t phy_copy;

    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));

    PHYMOD_IF_ERR_RETURN
            (blackhawk_tsc_config_shared_tx_pattern (&phy_copy.access,
				(uint8_t) pattern->pattern_len, (const char *) pattern->pattern));
    return PHYMOD_E_NONE;
}

int tscbh_phy_pattern_config_get(const phymod_phy_access_t* phy, phymod_pattern_t* pattern)
{
    phymod_phy_access_t phy_copy;

    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));
    PHYMOD_IF_ERR_RETURN
        (blackhawk_tsc_config_shared_tx_pattern_idx_get(&phy_copy.access,
                                  &pattern->pattern_len,
                                  pattern->pattern));
    return PHYMOD_E_NONE;
}


int tscbh_phy_pattern_enable_set(const phymod_phy_access_t* phy, uint32_t enable, const phymod_pattern_t* pattern)
{
    phymod_phy_access_t phy_copy;

    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));
    PHYMOD_IF_ERR_RETURN
        (blackhawk_tsc_tx_shared_patt_gen_en(&phy_copy.access, (uint8_t) enable, (uint8_t)pattern->pattern_len));
    return PHYMOD_E_NONE;
}

int tscbh_phy_pattern_enable_get(const phymod_phy_access_t* phy, uint32_t* enable)
{
    phymod_phy_access_t phy_copy;

    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));
    PHYMOD_IF_ERR_RETURN
       (blackhawk_tsc_tx_shared_patt_gen_en_get(&phy_copy.access, (uint8_t *) enable));
    return PHYMOD_E_NONE;
}

#endif

int tscbh_phy_fec_cl91_correctable_counter_get(const phymod_phy_access_t* phy, uint32_t* count)
{
    phymod_phy_access_t phy_copy;

    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));
    PHYMOD_IF_ERR_RETURN
       (tbhmod_fec_correctable_counter_get(&phy_copy.access, count));
    return PHYMOD_E_NONE;
}

int tscbh_phy_fec_cl91_uncorrectable_counter_get(const phymod_phy_access_t* phy, uint32_t* count)
{
    phymod_phy_access_t phy_copy;

    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));
    PHYMOD_IF_ERR_RETURN
       (tbhmod_fec_uncorrectable_counter_get(&phy_copy.access, count));
    return PHYMOD_E_NONE;

}


int tscbh_core_diagnostics_get(const phymod_core_access_t* core, phymod_core_diagnostics_t* diag)
{

    return PHYMOD_E_NONE;
}


int tscbh_phy_diagnostics_get(const phymod_phy_access_t* phy, phymod_phy_diagnostics_t* diag)
{

    return PHYMOD_E_NONE;
}

static void _tscbh_diag_uc_reg_dump(phymod_access_t *pa)
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

STATIC int tscbh_diagnostics_eyescan_run_uc(const phymod_phy_access_t* phy, uint32_t flags)
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
                _tscbh_diag_uc_reg_dump(&(phy_copy.access));
                PHYMOD_IF_ERR_RETURN(rc);
            }
        }
    }
    return PHYMOD_E_NONE;
}

STATIC int tscbh_diagnostics_eye_margin_proj( const phymod_phy_access_t* phy, uint32_t flags,
                                        const phymod_phy_eyescan_options_t* eyescan_options)
{
#ifdef SERDES_API_FLOATING_POINT
  int osr_mode;
  USR_DOUBLE data_rate, data_rate_in_Mhz;
  phymod_phy_access_t phy_copy;
  int start_lane, num_lane, i, found=0;
  phymod_phy_speed_config_t speed_config;

  struct   blackhawk_tsc_uc_core_config_st core_cfg;
  if(PHYMOD_EYESCAN_F_PROCESS_GET(flags)) {
     PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(&phy->access, &start_lane, &num_lane));
     PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));
     for (i = 0; i < num_lane; i++) {
         phy_copy.access.lane_mask = 1 << (start_lane + i);

         if (found == 0) {
             PHYMOD_IF_ERR_RETURN(blackhawk_tsc_get_uc_core_config(&(phy_copy.access), &core_cfg));
             PHYMOD_IF_ERR_RETURN(blackhawk_osr_mode_get(&phy_copy.access, &osr_mode));
             PHYMOD_IF_ERR_RETURN(tscbh_phy_speed_config_get(phy, &speed_config));
             data_rate = speed_config.data_rate * 1000.0 * 1000.0;
             data_rate = data_rate * 66 / 64 / num_lane;
             data_rate_in_Mhz= data_rate / 1000.0 / 1000.0;
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

int tscbh_phy_eyescan_run(const phymod_phy_access_t* phy,
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
            return tscbh_diagnostics_eyescan_run_uc(phy, flags);
        case phymodEyescanModeBERProj:
            return tscbh_diagnostics_eye_margin_proj(phy, flags, eyescan_options);
        default:
            PHYMOD_RETURN_WITH_ERR(PHYMOD_E_PARAM, (_PHYMOD_MSG("unsupported eyescan mode %u"), mode));
    }

  return PHYMOD_E_NONE;
}

/* Get RS FEC symbol error count.*/
int tscbh_phy_rsfec_symbol_error_counter_get(const phymod_phy_access_t* phy,
                                             int max_count,
                                             int* actual_count,
                                             uint32_t* error_count)
{
    phymod_phy_access_t phy_copy;
    int start_lane, num_lane, speed_id, i;
    spd_id_tbl_entry_t speed_config_entry;
    uint32_t packed_entry[5];

    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));
    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(&phy->access, &start_lane, &num_lane));

    phy_copy.access.lane_mask = 0x1 << start_lane;
    /* first read speed id from resolved status */
    PHYMOD_IF_ERR_RETURN
        (tbhmod_speed_id_get(&phy_copy.access, &speed_id));

    /* first read the speed entry and then decode the speed and FEC type */
    phy_copy.access.lane_mask = 1 << 0;
    PHYMOD_IF_ERR_RETURN
        (phymod_mem_read(&phy_copy.access, phymodMemSpeedIdTable, speed_id, packed_entry));

    /*decode speed entry */
    spd_ctrl_unpack_spd_id_tbl_entry(packed_entry, &speed_config_entry);

    switch (speed_config_entry.num_lanes) {
        case 0: num_lane = 1;
            break;
        case 1: num_lane = 2;
            break;
        case 2: num_lane = 4;
            break;
        case 3: num_lane = 8;
            break;
        case 4: num_lane = 3;
            break;
        case 5: num_lane = 6;
            break;
        case 6: num_lane = 7;
            break;
        default:
            PHYMOD_DEBUG_ERROR(("Unsupported number of lane \n"));
            return PHYMOD_E_UNAVAIL;
    }

    /* Update lane mask. During AN, lane mask might change. */
    phy_copy.access.lane_mask = 0;
    for (i = 0; i < num_lane; i ++) {
        phy_copy.access.lane_mask |= 1 << (start_lane + i);
    }

    PHYMOD_IF_ERR_RETURN
        (tbhmod_rsfec_symbol_error_counter_get(&phy_copy.access,
                                               speed_config_entry.bit_mux_mode,
                                               max_count,
                                               actual_count,
                                               error_count));

    return PHYMOD_E_NONE;
}
#endif /* PHYMOD_BLACKHAWK_SUPPORT */
