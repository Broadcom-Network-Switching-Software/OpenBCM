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
#include <phymod/phymod_dispatch.h>
#include <phymod/chip/bcmi_tscbh_xgxs_defs.h>
#include <phymod/chip/tscbh.h>
#include "tscbh/tier1/tbhmod.h"
#include "tscbh/tier1/tbhmod_sc_lkup_table.h"
#include "tscbh/tier1/tbhmod_1588_lkup_table.h"
#include "tscbh/tier1/tbhPCSRegEnums.h"
#include "blackhawk/tier1/blackhawk_cfg_seq.h"
#include "blackhawk/tier1/blackhawk_tsc_enum.h"
#include "blackhawk/tier1/blackhawk_tsc_common.h"
#include "blackhawk/tier1/blackhawk_tsc_interface.h"
#include "blackhawk/tier1/blackhawk_tsc_dependencies.h"
#include "blackhawk/tier1/blackhawk_tsc_internal.h"
#include "blackhawk/tier1/public/blackhawk_api_uc_vars_rdwr_defns_public.h"
#include "blackhawk/tier1/blackhawk_tsc_access.h"


extern unsigned char blackhawk_ucode_rev0[];
extern unsigned int blackhawk_ucode_len_rev0;
extern unsigned short blackhawk_ucode_crc_rev0;
extern unsigned short blackhawk_ucode_stack_size_rev0;

extern unsigned char blackhawk_ucode_rev1[];
extern unsigned int blackhawk_ucode_len_rev1;
extern unsigned short blackhawk_ucode_crc_rev1;
extern unsigned short blackhawk_ucode_stack_size_rev1;

extern uint32_t spd_id_entry_26[TSCBH_SPEED_ID_TABLE_SIZE][TSCBH_SPEED_ID_ENTRY_SIZE];
extern uint32_t spd_id_entry_25[TSCBH_SPEED_ID_TABLE_SIZE][TSCBH_SPEED_ID_ENTRY_SIZE];
extern uint32_t spd_id_entry_20[TSCBH_SPEED_ID_TABLE_SIZE][TSCBH_SPEED_ID_ENTRY_SIZE];
extern uint32_t am_table_entry[TSCBH_AM_TABLE_SIZE][TSCBH_AM_ENTRY_SIZE];
extern uint32_t um_table_entry[TSCBH_UM_TABLE_SIZE][TSCBH_UM_ENTRY_SIZE];
extern uint32_t speed_priority_mapping_table[TSCBH_SPEED_PRIORITY_MAPPING_TABLE_SIZE][TSCBH_SPEED_PRIORITY_MAPPING_ENTRY_SIZE];
extern uint32_t spd_id_entry_100g_4lane_no_fec_26[TSCBH_SPEED_ID_ENTRY_SIZE];
extern uint32_t spd_id_entry_100g_4lane_no_fec_25[TSCBH_SPEED_ID_ENTRY_SIZE];

extern const ts_table_entry ts_table_rx_sop[TBHMOD_SPEED_MODE_COUNT];
extern const ts_table_entry ts_table_tx_sop[TBHMOD_SPEED_MODE_COUNT];

#define TSCBH_SERDES_ID           0x25 /* 0x9008 Main0_serdesID - Serdes ID Register */
#define SPEED_ID_INDEX_100G_4_LANE_CL73_KR4     0x6
#define SPEED_ID_INDEX_100G_4_LANE_CL73_CR4     0x7

int tscbh_core_identify(const phymod_core_access_t* core, uint32_t core_id, uint32_t* is_identified)
{
    PHYID2r_t id2;
    PHYID3r_t id3;
    MAIN0_SERDESIDr_t serdesid;
    int ioerr = 0;

    *is_identified = 0;
    ioerr += READ_PHYID2r(&core->access, &id2);
    ioerr += READ_PHYID3r(&core->access, &id3);

    if (PHYID2r_REGID1f_GET(id2) == TSCBH_ID0 &&
       (PHYID3r_REGID2f_GET(id3) == TSCBH_ID1)) {
        /* PHY IDs match - now check PCS model */
        ioerr += READ_MAIN0_SERDESIDr(&core->access, &serdesid);
        if ( (MAIN0_SERDESIDr_MODEL_NUMBERf_GET(serdesid)) == TSCBH_SERDES_ID)  {
            *is_identified = 1;
        }
    }
    return ioerr ? PHYMOD_E_IO : PHYMOD_E_NONE;
}


int tscbh_core_info_get(const phymod_core_access_t* core, phymod_core_info_t* info)
{
    int rv = 0;
    MAIN0_SERDESIDr_t serdes_id;
    char core_name[15] = "Tscbh16";
    PHYID2r_t id2;
    PHYID3r_t id3;
    const phymod_access_t *pm_acc = &core->access;

    rv = READ_MAIN0_SERDESIDr(&core->access, &serdes_id);

    info->serdes_id = MAIN0_SERDESIDr_GET(serdes_id);
    PHYMOD_IF_ERR_RETURN
        (phymod_core_name_get(core, MAIN0_SERDESIDr_GET(serdes_id), core_name, info));
    info->serdes_id = MAIN0_SERDESIDr_GET(serdes_id);
    info->core_version = phymodCoreVersionTscbh16;

    PHYMOD_IF_ERR_RETURN(READ_PHYID2r(pm_acc, &id2));
    PHYMOD_IF_ERR_RETURN(READ_PHYID3r(pm_acc, &id3));

    info->phy_id0 = (uint16_t) id2.v[0];
    info->phy_id1 = (uint16_t) id3.v[0];

    return rv;
}

/*
 * get the core lane mapping
 * in format, lane_map_tx/rx[logical lane] = physical lane
 */
int tscbh_core_lane_map_get(const phymod_core_access_t* core, phymod_lane_map_t* lane_map)
{
    phymod_core_access_t  core_copy;
    uint32_t tx_lane_map, rx_lane_map;
    int i = 0;

    PHYMOD_MEMCPY(&core_copy, core, sizeof(core_copy));
    core_copy.access.lane_mask = 0x1;

    PHYMOD_IF_ERR_RETURN
        (blackhawk_pmd_lane_map_get(&core_copy.access, &tx_lane_map, &rx_lane_map));

    /*next get the lane map into serdes spi format */
    for (i = 0; i < TSCBH_NOF_LANES_IN_CORE; i++) {
        lane_map->lane_map_tx[tx_lane_map >> (4 * i) & 0xf] = i;
        lane_map->lane_map_rx[rx_lane_map >> (4 * i) & 0xf] = i;
    }

    return PHYMOD_E_NONE;
}


int tscbh_core_reset_set(const phymod_core_access_t* core, phymod_reset_mode_t reset_mode, phymod_reset_direction_t direction)
{
    return PHYMOD_E_NONE;

}

int tscbh_core_reset_get(const phymod_core_access_t* core, phymod_reset_mode_t reset_mode, phymod_reset_direction_t* direction)
{
    return PHYMOD_E_NONE;

}

int tscbh_phy_tx_lane_control_set(const phymod_phy_access_t* phy, phymod_phy_tx_lane_control_t tx_control)
{
    phymod_firmware_lane_config_t fw_lane_config;
    phymod_phy_access_t phy_copy;

    PHYMOD_IF_ERR_RETURN (tscbh_phy_firmware_lane_config_get(phy, &fw_lane_config));
    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));

    switch (tx_control) {
        case phymodTxTrafficDisable:
            PHYMOD_IF_ERR_RETURN(tbhmod_tx_lane_control(&phy_copy.access, 0, TBHMOD_TX_LANE_TRAFFIC_DISABLE));
            break;
        case phymodTxTrafficEnable:
            /* whenever the second inpout which is enable is set, then the thrid parameter is do not care */
            PHYMOD_IF_ERR_RETURN(tbhmod_tx_lane_control(&phy_copy.access, 1, TBHMOD_TX_LANE_ILLEGAL));
            break;
        case phymodTxReset:
            PHYMOD_IF_ERR_RETURN(tbhmod_tx_lane_control(&phy_copy.access, 0, TBHMOD_TX_LANE_RESET));
            break;
        case phymodTxElectricalIdleEnable:
            if (fw_lane_config.LaneConfigFromPCS == 0) {
                PHYMOD_IF_ERR_RETURN(blackhawk_electrical_idle_set(&phy_copy.access, 1));
            } else {
                return PHYMOD_E_PARAM;
            }
            break;
        case phymodTxElectricalIdleDisable:
            PHYMOD_IF_ERR_RETURN(blackhawk_electrical_idle_set(&phy_copy.access, 0));
            break;
        case phymodTxSquelchOn:
            PHYMOD_IF_ERR_RETURN(blackhawk_tsc_tx_disable(&phy_copy.access, 1));
            break;
        case phymodTxSquelchOff:
            PHYMOD_IF_ERR_RETURN(blackhawk_tsc_tx_disable(&phy_copy.access, 0));
            break;
        default:
            return PHYMOD_E_PARAM;
    }
    return PHYMOD_E_NONE;

}


int tscbh_phy_tx_lane_control_get(const phymod_phy_access_t* phy, phymod_phy_tx_lane_control_t *tx_control)
{
    uint8_t tx_disable;
    int reset, tx_lane;
    uint32_t lb_enable;
    phymod_phy_access_t pm_phy_copy;
    int start_lane, num_lane;

    PHYMOD_MEMCPY(&pm_phy_copy, phy, sizeof(pm_phy_copy));
    /* next program the tx fir taps and driver current based on the input */
    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(&phy->access, &start_lane, &num_lane));

    pm_phy_copy.access.lane_mask = 0x1 << start_lane;

    PHYMOD_IF_ERR_RETURN(blackhawk_tsc_tx_disable_get(&pm_phy_copy.access, &tx_disable));

    /* next check if PMD loopback is on */
    if (tx_disable) {
        PHYMOD_IF_ERR_RETURN(blackhawk_tsc_dig_lpbk_get(&pm_phy_copy.access, &lb_enable));
        if (lb_enable) tx_disable = 0;
    }

    if(tx_disable) {
        *tx_control = phymodTxSquelchOn;
    } else {
        PHYMOD_IF_ERR_RETURN(tbhmod_tx_lane_control_get(&pm_phy_copy.access, &reset, &tx_lane));
        if (!reset) {
            *tx_control = phymodTxReset;
        } else if (!tx_lane) {
            *tx_control = phymodTxTrafficDisable;
        } else {
            *tx_control = phymodTxTrafficEnable;
        }
    }
    return PHYMOD_E_NONE;
}

/*Rx control*/
int tscbh_phy_rx_lane_control_set(const phymod_phy_access_t* phy, phymod_phy_rx_lane_control_t rx_control)
{
    phymod_phy_access_t pm_phy_copy;

    PHYMOD_MEMCPY(&pm_phy_copy, phy, sizeof(pm_phy_copy));

    switch (rx_control) {
        case phymodRxReset:
            PHYMOD_IF_ERR_RETURN(tbhmod_rx_lane_control(&pm_phy_copy.access, 0));
            break;
        case phymodRxSquelchOn:
            PHYMOD_IF_ERR_RETURN(blackhawk_pmd_force_signal_detect(&pm_phy_copy.access, 1, 0));
            break;
        case phymodRxSquelchOff:
            PHYMOD_IF_ERR_RETURN(blackhawk_pmd_force_signal_detect(&pm_phy_copy.access, 0, 0));
            break;
        default:
            return PHYMOD_E_PARAM;
    }

    return PHYMOD_E_NONE;
}

int tscbh_phy_rx_lane_control_get(const phymod_phy_access_t* phy, phymod_phy_rx_lane_control_t* rx_control)
{
    int reset, rx_squelch_enable;
    uint32_t lb_enable;
    uint8_t force_en, force_val;
    phymod_phy_access_t pm_phy_copy;

    PHYMOD_MEMCPY(&pm_phy_copy, phy, sizeof(pm_phy_copy));

    /* first get the force enabled bit and forced value */
    PHYMOD_IF_ERR_RETURN(blackhawk_pmd_force_signal_detect_get(&pm_phy_copy.access, &force_en, &force_val));

    if (force_en & (!force_val)) {
        rx_squelch_enable = 1;
    } else {
        rx_squelch_enable = 0;
    }

    /* next check if PMD loopback is on */
    if (rx_squelch_enable) {
        PHYMOD_IF_ERR_RETURN(blackhawk_tsc_dig_lpbk_get(&pm_phy_copy.access, &lb_enable));
        if (lb_enable) rx_squelch_enable = 0;
    }
    if(rx_squelch_enable) {
        *rx_control = phymodRxSquelchOn;
    } else {
        PHYMOD_IF_ERR_RETURN(tbhmod_rx_lane_control_get(&pm_phy_copy.access, &reset));
        if (reset == 0) {
            *rx_control = phymodRxReset;
        } else {
            *rx_control = phymodRxSquelchOff;
        }
    }
    return PHYMOD_E_NONE;

}

/* load tscbh fw. the fw_loader parameter is valid just for external fw load*/
STATIC
int _tscbh_core_firmware_load(const phymod_core_access_t* core, const phymod_core_init_config_t* init_config)
{
    phymod_core_access_t  core_copy;
    phymod_phy_access_t phy_access;
    unsigned int blackhawk_ucode_len;
    unsigned char *blackhawk_ucode;
    uint32_t rev_num = 0;

    PHYMOD_MEMCPY(&core_copy, core, sizeof(core_copy));
    TSCBH_CORE_TO_PHY_ACCESS(&phy_access, core);

    phy_access.access.lane_mask = 0x1;
    PHYMOD_IF_ERR_RETURN
        (tbhmod_revid_get(&phy_access.access, &rev_num));
    if (rev_num == TBHMOD_REV_NUM_0) {
        blackhawk_ucode = blackhawk_ucode_rev0;
        blackhawk_ucode_len = blackhawk_ucode_len_rev0;
    } else {
        blackhawk_ucode = blackhawk_ucode_rev1;
        blackhawk_ucode_len = blackhawk_ucode_len_rev1;
    }

    switch(init_config->firmware_load_method){
    case phymodFirmwareLoadMethodInternal:
        PHYMOD_IF_ERR_RETURN(blackhawk_tsc_ucode_mdio_load(&core_copy.access, blackhawk_ucode, blackhawk_ucode_len));
        break;
    case phymodFirmwareLoadMethodExternal:
        PHYMOD_NULL_CHECK(init_config->firmware_loader);
        PHYMOD_IF_ERR_RETURN(blackhawk_tsc_ucode_init(&core_copy.access));
        PHYMOD_IF_ERR_RETURN
            (blackhawk_pram_firmware_enable(&core_copy.access, 1, 0));
        PHYMOD_IF_ERR_RETURN(init_config->firmware_loader(core, blackhawk_ucode_len, blackhawk_ucode));
        PHYMOD_IF_ERR_RETURN
            (blackhawk_pram_firmware_enable(&core_copy.access, 0, 0));
        break;
    case phymodFirmwareLoadMethodNone:
        break;
    default:
        PHYMOD_RETURN_WITH_ERR(PHYMOD_E_CONFIG, (_PHYMOD_MSG("illegal fw load method %u"), init_config->firmware_load_method));
    }

    return PHYMOD_E_NONE;

}

int tscbh_phy_firmware_lane_config_get(const phymod_phy_access_t* phy, phymod_firmware_lane_config_t* fw_config)
{
    struct blackhawk_tsc_uc_lane_config_st lane_config;
    phymod_phy_access_t pm_phy_copy;

    PHYMOD_MEMCPY(&pm_phy_copy, phy, sizeof(pm_phy_copy));

    PHYMOD_MEMSET(&lane_config, 0x0, sizeof(lane_config));
    PHYMOD_MEMSET(fw_config, 0, sizeof(*fw_config));

    PHYMOD_IF_ERR_RETURN
        (blackhawk_tsc_get_uc_lane_cfg(&pm_phy_copy.access, &lane_config));

    fw_config->LaneConfigFromPCS     = lane_config.field.lane_cfg_from_pcs;
    fw_config->AnEnabled             = lane_config.field.an_enabled;
    fw_config->DfeOn                 = lane_config.field.dfe_on;
    fw_config->LpDfeOn               = lane_config.field.dfe_lp_mode;
    fw_config->ForceBrDfe            = lane_config.field.force_brdfe_on;
    fw_config->MediaType             = lane_config.field.media_type;
    fw_config->UnreliableLos         = lane_config.field.unreliable_los;
    fw_config->Cl72AutoPolEn         = lane_config.field.cl72_auto_polarity_en;
    fw_config->ScramblingDisable     = lane_config.field.scrambling_dis;
    fw_config->Cl72RestTO            = lane_config.field.cl72_restart_timeout_en;
    fw_config->ForceExtenedReach     = lane_config.field.force_es;
    fw_config->ForceNormalReach      = lane_config.field.force_ns;
    fw_config->LpPrecoderEnabled     = lane_config.field.lp_has_prec_en;
    fw_config->ForcePAM4Mode         = lane_config.field.force_pam4_mode;
    fw_config->ForceNRZMode          = lane_config.field.force_nrz_mode;

    return PHYMOD_E_NONE;
}

int tscbh_phy_tx_set(const phymod_phy_access_t* phy, const phymod_tx_t* tx)
{
    phymod_phy_access_t phy_copy;
    int start_lane, num_lane, i, port_start_lane, port_num_lane;
    enum blackhawk_tsc_txfir_tap_enable_enum enable_taps = NRZ_6TAP;
    uint32_t lane_reset, pcs_lane_enable, port_lane_mask = 0;

    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(&phy->access, &start_lane, &num_lane));
    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));

    /*get the start lane of the port lane mask */
    PHYMOD_IF_ERR_RETURN
        (tbhmod_port_start_lane_get(&phy_copy.access, &port_start_lane, &port_num_lane));

    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_mask_get(port_start_lane, port_num_lane, &port_lane_mask));

    phy_copy.access.lane_mask = 1 << port_start_lane;

    /*next check if PCS lane is in reset */
    PHYMOD_IF_ERR_RETURN
        (tbhmod_enable_get(&phy_copy.access, &pcs_lane_enable));

    /*first check if lane is in reset */
    phy_copy.access.lane_mask = 1 << start_lane;
    PHYMOD_IF_ERR_RETURN
        (blackhawk_lane_soft_reset_get(&phy_copy.access, &lane_reset));

    /* disable pcs lane if pcs lane not in rset */
    if (pcs_lane_enable) {
        phy_copy.access.lane_mask = 1 << port_start_lane;
        PHYMOD_IF_ERR_RETURN
            (tbhmod_disable_set(&phy_copy.access));

        /* add the pcs disable SW WAR */
        phy_copy.access.lane_mask = port_lane_mask;
        PHYMOD_IF_ERR_RETURN
            (tbhmod_pcs_reset_sw_war(&phy_copy.access));
    }
    if (!lane_reset) {
        PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));
        PHYMOD_IF_ERR_RETURN
            (blackhawk_lane_soft_reset(&phy_copy.access, 1));
    }

    for (i = 0; i < num_lane; i++) {
        if (!PHYMOD_LANEPBMP_MEMBER(phy->access.lane_mask, start_lane + i)) {
            continue;
        }
        phy_copy.access.lane_mask = 1 << (start_lane + i);
        /*next check 3 tap mode or 6 tap mode */
        if (tx->tap_mode == phymodTxTapMode3Tap) {
            if (tx->sig_method == phymodSignallingMethodNRZ) {
                enable_taps= NRZ_LP_3TAP;
            } else {
                enable_taps= PAM4_LP_3TAP;
            }
            PHYMOD_IF_ERR_RETURN
                (blackhawk_tsc_apply_txfir_cfg(&phy_copy.access,
                                                 enable_taps,
                                                 0,
                                                 tx->pre,
                                                 tx->main,
                                                 tx->post,
                                                 0,
                                                 0));
        } else {
            if (tx->sig_method == phymodSignallingMethodNRZ) {
                enable_taps= NRZ_6TAP;
            } else {
                enable_taps= PAM4_6TAP;
            }
            PHYMOD_IF_ERR_RETURN
                (blackhawk_tsc_apply_txfir_cfg(&phy_copy.access,
                                                 enable_taps,
                                                 tx->pre2,
                                                 tx->pre,
                                                 tx->main,
                                                 tx->post,
                                                 tx->post2,
                                                 tx->post3));
        }
    }

    if (!lane_reset) {
        PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));
        PHYMOD_IF_ERR_RETURN
            (blackhawk_lane_soft_reset(&phy_copy.access, 0));
    }

    /* re-enable pcs lane if pcs lane not in rset */
    if (pcs_lane_enable) {
        phy_copy.access.lane_mask = 1 << port_start_lane;
        PHYMOD_IF_ERR_RETURN
            (tbhmod_enable_set(&phy_copy.access));
    }
    return PHYMOD_E_NONE;
}

int tscbh_phy_media_type_tx_get(const phymod_phy_access_t* phy, phymod_media_typed_t media, phymod_tx_t* tx)
{

    return PHYMOD_E_NONE;

}


/*
 * set lane swapping for core
 */

int tscbh_core_lane_map_set(const phymod_core_access_t* core, const phymod_lane_map_t* lane_map)
{
    phymod_core_access_t  core_copy;
    uint32_t lane, pcs_tx_swap = 0, pcs_rx_swap = 0;
    uint8_t pmd_tx_addr[8], pmd_rx_addr[8];

    if (lane_map->num_of_lanes != TSCBH_NOF_LANES_IN_CORE){
        return PHYMOD_E_CONFIG;
    }

    PHYMOD_MEMCPY(&core_copy, core, sizeof(core_copy));
    core_copy.access.lane_mask = 0x1;

    for (lane = 0; lane < TSCBH_NOF_LANES_IN_CORE; lane++){
        if ((lane_map->lane_map_tx[lane] >= TSCBH_NOF_LANES_IN_CORE)||
             (lane_map->lane_map_rx[lane] >= TSCBH_NOF_LANES_IN_CORE)){
            return PHYMOD_E_CONFIG;
        }
        /*encode each lane as four bits*/
        pcs_tx_swap += lane_map->lane_map_tx[lane]<<(lane*4);
        pcs_rx_swap += lane_map->lane_map_rx[lane]<<(lane*4);
    }
    /* PMD lane addr is based on PCS logical to physical mapping*/
    for (lane = 0; lane < TSCBH_NOF_LANES_IN_CORE; lane++){
        pmd_tx_addr[((pcs_tx_swap >> (lane*4)) & 0xf)] = lane;
        pmd_rx_addr[((pcs_rx_swap >> (lane*4)) & 0xf)] = lane;
    }

    PHYMOD_IF_ERR_RETURN
        (tbhmod_pcs_tx_lane_swap(&core_copy.access, pcs_tx_swap));
    PHYMOD_IF_ERR_RETURN
        (tbhmod_pcs_rx_lane_swap(&core_copy.access, pcs_rx_swap));
    PHYMOD_IF_ERR_RETURN
        (blackhawk_tsc_map_lanes(&core_copy.access, TSCBH_NOF_LANES_IN_CORE, pmd_tx_addr, pmd_rx_addr));

    return PHYMOD_E_NONE;
}

#if 0
STATIC
int _tscbh_speed_config_get(uint32_t speed, uint32_t *pll_multiplier, uint32_t *is_pam4, uint32_t *osr_mode)
{
    return PHYMOD_E_NONE;
}
#endif

STATIC
int _tscbh_phy_firmware_lane_config_set(const phymod_phy_access_t* phy, phymod_firmware_lane_config_t fw_config)
{
    uint32_t is_warm_boot;
    struct blackhawk_tsc_uc_lane_config_st serdes_firmware_config;
    phymod_phy_access_t phy_copy;
    int start_lane, num_lane, i;
    uint32_t rst_status;

    PHYMOD_MEMSET(&serdes_firmware_config, 0x0, sizeof(serdes_firmware_config));
    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(&phy->access, &start_lane, &num_lane));
    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));

    for (i = 0; i < num_lane; i++) {
        if (!PHYMOD_LANEPBMP_MEMBER(phy->access.lane_mask, start_lane + i)) {
            continue;
        }
        phy_copy.access.lane_mask = 1 << (start_lane + i);
        serdes_firmware_config.field.lane_cfg_from_pcs      = fw_config.LaneConfigFromPCS;
        serdes_firmware_config.field.an_enabled             = fw_config.AnEnabled;
        serdes_firmware_config.field.dfe_on                 = fw_config.DfeOn;
        serdes_firmware_config.field.force_brdfe_on         = fw_config.ForceBrDfe;
        /* serdes_firmware_config.field.cl72_emulation_en = fw_config.Cl72Enable; */
        serdes_firmware_config.field.scrambling_dis         = fw_config.ScramblingDisable;
        serdes_firmware_config.field.unreliable_los         = fw_config.UnreliableLos;
        serdes_firmware_config.field.media_type             = fw_config.MediaType;
        serdes_firmware_config.field.dfe_lp_mode            = fw_config.LpDfeOn;
        serdes_firmware_config.field.cl72_auto_polarity_en  = fw_config.Cl72AutoPolEn;
        serdes_firmware_config.field.cl72_restart_timeout_en = fw_config.Cl72RestTO;
        serdes_firmware_config.field.force_es               = fw_config.ForceExtenedReach;
        serdes_firmware_config.field.force_ns               = fw_config.ForceNormalReach;
        serdes_firmware_config.field.force_nrz_mode         = fw_config.ForceNRZMode;
        serdes_firmware_config.field.force_pam4_mode        = fw_config.ForcePAM4Mode;
        serdes_firmware_config.field.lp_has_prec_en         = fw_config.LpPrecoderEnabled;

        PHYMOD_IF_ERR_RETURN(PHYMOD_IS_WRITE_DISABLED(&phy->access, &is_warm_boot));

        if (!is_warm_boot) {
            PHYMOD_IF_ERR_RETURN(blackhawk_lane_soft_reset_get(&phy_copy.access, &rst_status));
            if (!rst_status) PHYMOD_IF_ERR_RETURN (blackhawk_lane_soft_reset(&phy_copy.access, 1));
            PHYMOD_IF_ERR_RETURN(blackhawk_tsc_set_uc_lane_cfg(&phy_copy.access, serdes_firmware_config));
            if (!rst_status) PHYMOD_IF_ERR_RETURN (blackhawk_lane_soft_reset(&phy_copy.access, 0));
        }
    }

    return PHYMOD_E_NONE;
}

int tscbh_phy_firmware_lane_config_set(const phymod_phy_access_t* phy, phymod_firmware_lane_config_t fw_config)
{
    phymod_phy_access_t phy_copy;
    uint32_t lane_reset, pcs_lane_enable;
    int start_lane, num_lane;

    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));
    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(&phy->access, &start_lane, &num_lane));

    /*first check if lane is in reset */
    PHYMOD_IF_ERR_RETURN
        (blackhawk_lane_soft_reset_get(&phy_copy.access, &lane_reset));

    /*next check if PCS lane is in reset */
    PHYMOD_IF_ERR_RETURN
        (tbhmod_enable_get(&phy_copy.access, &pcs_lane_enable));

    /* disable pcs lane if pcs lane not in rset */
    if (pcs_lane_enable) {
        phy_copy.access.lane_mask = 1 << start_lane;
        PHYMOD_IF_ERR_RETURN
            (tbhmod_disable_set(&phy_copy.access));

        /* add the pcs disable SW WAR */
        PHYMOD_IF_ERR_RETURN
            (tbhmod_pcs_reset_sw_war(&phy->access));
    }

    /* if lane is not in reset, then reset the lane first */
    if (!lane_reset) {
        PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));
        PHYMOD_IF_ERR_RETURN
            (blackhawk_lane_soft_reset(&phy_copy.access, 1));
    }

    PHYMOD_IF_ERR_RETURN
         (_tscbh_phy_firmware_lane_config_set(phy, fw_config));

    if (!lane_reset) {
        PHYMOD_IF_ERR_RETURN
            (blackhawk_lane_soft_reset(&phy_copy.access, 0));
    }

    /* re-enable pcs lane if pcs lane not in rset */
    if (pcs_lane_enable) {
        phy_copy.access.lane_mask = 1 << start_lane;
        PHYMOD_IF_ERR_RETURN
            (tbhmod_enable_set(&phy_copy.access));
    }

    return PHYMOD_E_NONE;
}


/* reset rx sequencer
 * flags - unused parameter
 */
int tscbh_phy_rx_restart(const phymod_phy_access_t* phy)
{

    return PHYMOD_E_NONE;
}


int tscbh_phy_polarity_set(const phymod_phy_access_t* phy, const phymod_polarity_t* polarity)
{
    phymod_phy_access_t phy_copy;
    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));

    PHYMOD_IF_ERR_RETURN
        (blackhawk_tx_rx_polarity_set(&phy_copy.access, polarity->tx_polarity, polarity->rx_polarity));

    return PHYMOD_E_NONE;
}


int tscbh_phy_polarity_get(const phymod_phy_access_t* phy, phymod_polarity_t* polarity)
{
    int start_lane, num_lane, i;
    phymod_polarity_t temp_pol;
    phymod_phy_access_t phy_copy;
    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));

    /* figure out the lane num and start_lane based on the input */
    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(&phy->access, &start_lane, &num_lane));

    polarity->tx_polarity = 0;
    polarity->rx_polarity = 0;
    for (i = 0; i < num_lane; i++) {
        if (!PHYMOD_LANEPBMP_MEMBER(phy->access.lane_mask, start_lane + i)) {
            continue;
        }
        phy_copy.access.lane_mask = 0x1 << (i + start_lane);
        temp_pol.tx_polarity = 0;
        temp_pol.rx_polarity = 0;
        PHYMOD_IF_ERR_RETURN
            (blackhawk_tx_rx_polarity_get(&phy_copy.access, &temp_pol.tx_polarity, &temp_pol.rx_polarity));
        polarity->tx_polarity |= ((temp_pol.tx_polarity & 0x1) << i);
        polarity->rx_polarity |= ((temp_pol.rx_polarity & 0x1) << i);
    }

    return PHYMOD_E_NONE;
}

int tscbh_phy_tx_get(const phymod_phy_access_t* phy, phymod_tx_t* tx)
{
    uint8_t pmd_tx_tap_mode;
    uint16_t tx_tap_nrz_mode = 0;
    int16_t val;
    phymod_phy_access_t phy_copy;
    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));

    /* read current tx tap mode */
    PHYMOD_IF_ERR_RETURN
        (blackhawk_tsc_tx_tap_mode_get(&phy_copy.access, &pmd_tx_tap_mode));

    /*read current tx NRZ mode control info */
    PHYMOD_IF_ERR_RETURN
        (blackhawk_tsc_tx_nrz_mode_get(&phy_copy.access, &tx_tap_nrz_mode));

    if (pmd_tx_tap_mode == 0) {
        /* 3 tap mode */
        tx->tap_mode = phymodTxTapMode3Tap;
    } else {
        tx->tap_mode = phymodTxTapMode6Tap;
    }

    if (tx_tap_nrz_mode) {
        tx->sig_method = phymodSignallingMethodNRZ;
    } else {
        tx->sig_method = phymodSignallingMethodPAM4;
    }

    /*next check 3 tap mode or 6 tap mode */
    if (tx->tap_mode == phymodTxTapMode3Tap) {
        PHYMOD_IF_ERR_RETURN
            (blackhawk_tsc_read_tx_afe(&phy_copy.access, TX_AFE_TAP0, &val));
            tx->pre = val;
        PHYMOD_IF_ERR_RETURN
            (blackhawk_tsc_read_tx_afe(&phy_copy.access, TX_AFE_TAP1, &val));
            tx->main = val;
        PHYMOD_IF_ERR_RETURN
            (blackhawk_tsc_read_tx_afe(&phy_copy.access, TX_AFE_TAP2, &val));
            tx->post = val;
            tx->pre2 = 0;
            tx->post2 = 0;
            tx->post3 = 0;
    } else {
        PHYMOD_IF_ERR_RETURN
            (blackhawk_tsc_read_tx_afe(&phy_copy.access, TX_AFE_TAP0, &val));
            tx->pre2 = val;
        PHYMOD_IF_ERR_RETURN
            (blackhawk_tsc_read_tx_afe(&phy_copy.access, TX_AFE_TAP1, &val));
            tx->pre = val;
        PHYMOD_IF_ERR_RETURN
            (blackhawk_tsc_read_tx_afe(&phy_copy.access, TX_AFE_TAP2, &val));
            tx->main = val;
        PHYMOD_IF_ERR_RETURN
            (blackhawk_tsc_read_tx_afe(&phy_copy.access, TX_AFE_TAP3, &val));
            tx->post = val;
        PHYMOD_IF_ERR_RETURN
            (blackhawk_tsc_read_tx_afe(&phy_copy.access, TX_AFE_TAP4, &val));
            tx->post2 = val;
        PHYMOD_IF_ERR_RETURN
            (blackhawk_tsc_read_tx_afe(&phy_copy.access, TX_AFE_TAP5, &val));
            tx->post3 = val;
    }

    return PHYMOD_E_NONE;
}



int tscbh_phy_tx_override_set(const phymod_phy_access_t* phy, const phymod_tx_override_t* tx_override)
{
    phymod_phy_access_t phy_copy;
    int start_lane, num_lane, i;

    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(&phy->access, &start_lane, &num_lane));
    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));

    PHYMOD_IF_ERR_RETURN
        (blackhawk_lane_soft_reset(&phy_copy.access, 1));

    for (i = 0; i < num_lane; i++) {
        if (!PHYMOD_LANEPBMP_MEMBER(phy->access.lane_mask, start_lane + i)) {
            continue;
        }
        phy_copy.access.lane_mask = 1 << (start_lane + i);
        PHYMOD_IF_ERR_RETURN
             (blackhawk_tsc_tx_pi_freq_override(&phy_copy.access,
                                                 tx_override->phase_interpolator.enable,
                                                 tx_override->phase_interpolator.value));
    }

    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));
    PHYMOD_IF_ERR_RETURN
        (blackhawk_lane_soft_reset(&phy_copy.access, 0));

    return PHYMOD_E_NONE;
}

int tscbh_phy_tx_override_get(const phymod_phy_access_t* phy, phymod_tx_override_t* tx_override)
{
    phymod_phy_access_t phy_copy;
    int16_t temp_value;
    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));

    PHYMOD_IF_ERR_RETURN
        (blackhawk_tx_pi_control_get(&phy_copy.access, &temp_value));

    tx_override->phase_interpolator.value = (int32_t) temp_value;
    return PHYMOD_E_NONE;
}

int tscbh_phy_reset_set(const phymod_phy_access_t* phy, const phymod_phy_reset_t* reset)
{

    return PHYMOD_E_UNAVAIL;
}


int tscbh_phy_reset_get(const phymod_phy_access_t* phy, phymod_phy_reset_t* reset)
{

    return PHYMOD_E_UNAVAIL;

}


int tscbh_phy_power_set(const phymod_phy_access_t* phy, const phymod_phy_power_t* power)
{
    return PHYMOD_E_UNAVAIL;
}

int tscbh_phy_power_get(const phymod_phy_access_t* phy, phymod_phy_power_t* power)
{
    return PHYMOD_E_UNAVAIL;
}

/* This function based on num_lane, data_rate and fec_type
 * assign force speed SW speed_id.
 */
STATIC
int _tscbh_phy_speed_id_set(int num_lane,
                            uint32_t data_rate,
                            phymod_fec_type_t fec_type,
                            tbhmod_spd_intfc_type_t* spd_intf)
{
    if (num_lane == 1) {
        switch (data_rate) {
            case 10000:
                if (fec_type == phymod_fec_None) {
                    *spd_intf = TBHMOD_SPD_10000_XFI;
                } else if (fec_type == phymod_fec_CL74) {
                    *spd_intf = TBHMOD_SPD_10G_FEC_BASE_R_KR1_CR1;
                } else {
                    return PHYMOD_E_UNAVAIL;
                }
                break;
            case 12000:
                *spd_intf = TBHMOD_SPD_12P5G_BRCM_KR1;
                break;
            case 20000:
                if (fec_type == phymod_fec_None) {
                    *spd_intf = TBHMOD_SPD_20000_XFI;
                } else if (fec_type == phymod_fec_CL74) {
                    *spd_intf = TBHMOD_SPD_20G_FEC_BASE_R_KR1_CR1;
                } else {
                    return PHYMOD_E_UNAVAIL;
                }
                break;
            case 25000:
                if (fec_type == phymod_fec_None) {
                    *spd_intf = TBHMOD_SPD_25000_XFI;
                } else if (fec_type == phymod_fec_CL74) {
                    *spd_intf = TBHMOD_SPD_25G_FEC_BASE_R_KR1_CR1;
                } else if (fec_type == phymod_fec_CL91) {
                    *spd_intf = TBHMOD_SPD_25G_FEC_RS_FEC_KR1_CR1;
                } else {
                    return PHYMOD_E_UNAVAIL;
                }
                break;
            case 50000:
                if (fec_type == phymod_fec_CL91) {
                    *spd_intf = TBHMOD_SPD_50G_BRCM_FEC_528_CR1_KR1;
                } else if (fec_type == phymod_fec_RS544) {
                    *spd_intf = TBHMOD_SPD_50G_IEEE_KR1_CR1;
                } else if (fec_type == phymod_fec_RS272) {
                    *spd_intf = TBHMOD_SPD_50G_BRCM_FEC_272_KR1_CR1;
                } else {
                    return PHYMOD_E_UNAVAIL;
                }
                break;
            default:
                return PHYMOD_E_UNAVAIL;
        }
    } else if (num_lane == 2) {
        switch (data_rate) {
            case 40000:
                if (fec_type == phymod_fec_None) {
                    *spd_intf = TBHMOD_SPD_40G_MLD_X2;
                } else {
                    return PHYMOD_E_UNAVAIL;
                }
                break;
            case 50000:
                if (fec_type == phymod_fec_None) {
                    *spd_intf = TBHMOD_SPD_50G_MLD_X2;
                } else if (fec_type == phymod_fec_CL91) {
                    *spd_intf = TBHMOD_SPD_50G_MLD_FEC_528_X2;
                } else if (fec_type == phymod_fec_RS544) {
                    *spd_intf = TBHMOD_SPD_50G_BRCM_FEC_544_CR2_KR2;
                } else {
                    return PHYMOD_E_UNAVAIL;
                }
                break;
            case 100000:
                if (fec_type == phymod_fec_None) {
                    *spd_intf = TBHMOD_SPD_100G_BRCM_NOFEC_KR2_CR2;
                } else if (fec_type == phymod_fec_CL91) {
                    *spd_intf = TBHMOD_SPD_100G_BRCM_FEC_528_KR2_CR2;
                } else if (fec_type == phymod_fec_RS544) {
                    *spd_intf = TBHMOD_SPD_100G_IEEE_KR2_CR2;
                } else if (fec_type == phymod_fec_RS272) {
                    *spd_intf = TBHMOD_SPD_100G_BRCM_FEC_272_KR2_CR2;
                } else {
                    return PHYMOD_E_UNAVAIL;
                }
                break;
            default:
                return PHYMOD_E_UNAVAIL;
        }
    } else if (num_lane == 4) {
        switch (data_rate) {
            case 40000:
                if (fec_type == phymod_fec_None) {
                    *spd_intf = TBHMOD_SPD_40G_MLD_X4;
                } else if (fec_type == phymod_fec_CL74) {
                    *spd_intf = TBHMOD_SPD_40G_FEC_BASE_R_KR4_CR4;
                } else {
                    return PHYMOD_E_UNAVAIL;
                }
                break;
            case 100000:
                if (fec_type == phymod_fec_None) {
                    *spd_intf = TBHMOD_SPD_100G_MLD_NO_FEC_X4;
                } else if (fec_type == phymod_fec_CL91) {
                    *spd_intf = TBHMOD_SPD_100G_MLD_X4;
                } else if (fec_type == phymod_fec_RS544) {
                    *spd_intf = TBHMOD_SPD_100G_BRCM_FEC_544_1XN_KR4_CR4;
                } else {
                    return PHYMOD_E_UNAVAIL;
                }
                break;
            case 200000:
                if (fec_type == phymod_fec_None) {
                    *spd_intf = TBHMOD_SPD_200G_BRCM_NO_FEC_KR4_CR4;
                } else if (fec_type == phymod_fec_RS544) {
                    *spd_intf = TBHMOD_SPD_200G_BRCM_FEC_544_1XN_KR4_CR4;
                } else if (fec_type == phymod_fec_RS544_2XN) {
                    *spd_intf = TBHMOD_SPD_200G_IEEE_FEC_544_2XN_KR4_CR4;
                } else if (fec_type == phymod_fec_RS272) {
                    *spd_intf = TBHMOD_SPD_200G_BRCM_FEC_272_1XN_KR4_CR4;
                } else {
                    return PHYMOD_E_UNAVAIL;
                }
                break;
            default:
                return PHYMOD_E_UNAVAIL;
        }
    } else if (num_lane == 8) {
        if (data_rate == 400000 && fec_type == phymod_fec_RS544_2XN) {
            *spd_intf = TBHMOD_SPD_400G_BRCM_FEC_544_2XN_X8;
        } else {
            return PHYMOD_E_UNAVAIL;
        }
    } else {
        return PHYMOD_E_UNAVAIL;
    }

    return PHYMOD_E_NONE;
}

int tscbh_phy_speed_config_set(const phymod_phy_access_t* phy,
                                   const phymod_phy_speed_config_t* speed_config,
                                   const phymod_phy_pll_state_t* old_pll_state,
                                   phymod_phy_pll_state_t* new_pll_state)
{
    uint32_t ovco_is_pwrdn, tvco_is_pwrdn = 0;
    phymod_phy_access_t pm_phy_copy;
    uint32_t lane_mask_backup;
    uint32_t  tvco_pll_index, ovco_pll_index, tvco_pll_div = 0, ovco_pll_div = 0, request_pll_div = 0, pll_index = 0;
    uint32_t loss_in_db;
    int i, start_lane, num_lane, mapped_speed_id, ilkn_set;
    phymod_firmware_lane_config_t firmware_lane_config;
    phymod_firmware_core_config_t firmware_core_config;
    tbhmod_spd_intfc_type_t spd_intf = 0;
    tbhmod_refclk_t ref_clk;
    uint32_t is_pam4, osr_mode;

    firmware_lane_config = speed_config->pmd_lane_config;
    /*first make sure that tvco pll index is valid */
    if (phy->access.tvco_pll_index > 1) {
        PHYMOD_DEBUG_ERROR(("Unsupported tvco index\n"));
        return PHYMOD_E_UNAVAIL;
    }

    tvco_pll_index = phy->access.tvco_pll_index;
    ovco_pll_index = tvco_pll_index ? 0 : 1;

    PHYMOD_MEMSET(&firmware_core_config, 0x0, sizeof(firmware_core_config));

    /* Copy the PLL state */
    *new_pll_state = *old_pll_state;

    PHYMOD_MEMCPY(&pm_phy_copy, phy, sizeof(pm_phy_copy));
    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(&phy->access, &start_lane, &num_lane));
    lane_mask_backup = phy->access.lane_mask;

    /* Hold the pcs lane reset */
    pm_phy_copy.access.lane_mask = 1 << start_lane;
    PHYMOD_IF_ERR_RETURN
        (tbhmod_disable_set(&pm_phy_copy.access));

    /* add the pcs disable SW WAR */
    PHYMOD_IF_ERR_RETURN
        (tbhmod_pcs_reset_sw_war(&phy->access));

    /* write this port forced speed id entry */
    PHYMOD_IF_ERR_RETURN
      (tbhmod_set_sc_speed(&pm_phy_copy.access, TSCBH_FORCED_SPEED_ID_OFFSET + start_lane, 0));

    /*Hold the per lane PMD soft reset bit*/
    pm_phy_copy.access.lane_mask = lane_mask_backup;
    PHYMOD_IF_ERR_RETURN
        (blackhawk_lane_soft_reset(&pm_phy_copy.access, 1));

    /* first check if current lane are in ILKN mode */
    PHYMOD_IF_ERR_RETURN
        (tbhmod_pcs_ilkn_chk(&pm_phy_copy.access, &ilkn_set));

    /* if previous config is ILKN and reqeust config is ethernet
    need to clear some ILKN config */
    if (ilkn_set && !PHYMOD_DEVICE_OP_MODE_PCS_BYPASS_GET(phy->device_op_mode)) {
    /* Remove pmd_tx_disable_pin_dis it may be asserted because of ILKn */
        for (i = 0; i < num_lane; i++) {
            if (!PHYMOD_LANEPBMP_MEMBER(phy->access.lane_mask, start_lane + i)) {
                continue;
            }
            pm_phy_copy.access.lane_mask = 1 << (start_lane + i);
            PHYMOD_IF_ERR_RETURN
                (blackhawk_pmd_tx_disable_pin_dis_set(&pm_phy_copy.access, 0));
        }
        pm_phy_copy.access.lane_mask = lane_mask_backup;
        /*disable PCS ilkn mode */
        PHYMOD_IF_ERR_RETURN
            (tbhmod_pcs_ilkn_enable(&pm_phy_copy.access, 0));
      /* previous config is ethernet and request is ILKN mode */
    } else if (!ilkn_set && PHYMOD_DEVICE_OP_MODE_PCS_BYPASS_GET(phy->device_op_mode)) {
        for (i = 0; i < num_lane; i++) {
            if (!PHYMOD_LANEPBMP_MEMBER(phy->access.lane_mask, start_lane + i)) {
                continue;
            }
            pm_phy_copy.access.lane_mask = 1 << (start_lane + i);
            PHYMOD_IF_ERR_RETURN
                (blackhawk_pmd_tx_disable_pin_dis_set(&pm_phy_copy.access, 1));
        }
        /*enable PCS ilkn mode */
        for (i = 0; i < num_lane; i++) {
            if (!PHYMOD_LANEPBMP_MEMBER(phy->access.lane_mask, start_lane + i)) {
                continue;
            }
            pm_phy_copy.access.lane_mask = 1 << (start_lane + i);
            PHYMOD_IF_ERR_RETURN
                (tbhmod_pcs_ilkn_enable(&pm_phy_copy.access, 1));
       }
    }

    /*only update the port mode for ethernet port */
    if (!PHYMOD_DEVICE_OP_MODE_PCS_BYPASS_GET(phy->device_op_mode)) {
        PHYMOD_IF_ERR_RETURN
            (tbhmod_update_port_mode(&pm_phy_copy.access));
    }

    /*for ethernet speed mode config set */
    if (!PHYMOD_DEVICE_OP_MODE_PCS_BYPASS_GET(phy->device_op_mode)) {
        PHYMOD_IF_ERR_RETURN(_tscbh_phy_speed_id_set(num_lane, speed_config->data_rate,
                                                     speed_config->fec_type, &spd_intf));

        PHYMOD_IF_ERR_RETURN
            (tbhmod_get_mapped_speed(spd_intf, &mapped_speed_id));

        /* set the rs fec CW properly */
        if ((speed_config->fec_type == phymod_fec_RS544) ||
            (speed_config->fec_type == phymod_fec_RS544_2XN)) {
            if (start_lane < 4) {
                pm_phy_copy.access.lane_mask = 1 << 0;
            } else {
                pm_phy_copy.access.lane_mask = 1 << 4;
            }
            PHYMOD_IF_ERR_RETURN
                (tbhmod_rsfec_cw_type(&pm_phy_copy.access, 0, 0));
        } else if ((speed_config->fec_type == phymod_fec_RS272) ||
                   (speed_config->fec_type == phymod_fec_RS272_2XN)) {
            if (start_lane < 4) {
                pm_phy_copy.access.lane_mask = 1 << 0;
            } else {
                pm_phy_copy.access.lane_mask = 1 << 4;
            }
            PHYMOD_IF_ERR_RETURN
                (tbhmod_rsfec_cw_type(&pm_phy_copy.access, 1, 0));
        }
    }

    /* Check if ovco is power down */
    pm_phy_copy.access.pll_idx = ovco_pll_index;
    PHYMOD_IF_ERR_RETURN
        (blackhawk_tsc_pll_pwrdn_get(&pm_phy_copy.access, &ovco_is_pwrdn));

    /* if ovco is NOT pwoer down, then get the ovco div*/
    if (!ovco_is_pwrdn) {
        PHYMOD_IF_ERR_RETURN
            (blackhawk_tsc_INTERNAL_read_pll_div(&pm_phy_copy.access, &ovco_pll_div));
    }

    /* next check if pcs bypass mode, yes, then need to check tvco is power down */
    if (PHYMOD_DEVICE_OP_MODE_PCS_BYPASS_GET(phy->device_op_mode)) {
        pm_phy_copy.access.pll_idx = tvco_pll_index;
        PHYMOD_IF_ERR_RETURN
            (blackhawk_tsc_pll_pwrdn_get(&pm_phy_copy.access, &tvco_is_pwrdn));
    }

    if (!tvco_is_pwrdn) {
        /* Get TVCO because it's not allowed to change during speed set */
        pm_phy_copy.access.pll_idx = tvco_pll_index;
        PHYMOD_IF_ERR_RETURN
            (blackhawk_tsc_INTERNAL_read_pll_div(&pm_phy_copy.access, &tvco_pll_div));
    }

    /* based on the current TVCO PLL div, decide which copy of speed id entry to load */
    /* and this step only applies to (non)ilkn port*/
    if (!PHYMOD_DEVICE_OP_MODE_PCS_BYPASS_GET(phy->device_op_mode)) {
        /* first set the lane mask to be 0x1 */
        pm_phy_copy.access.lane_mask = 1 << 0;
        if ((tvco_pll_div == TBHMOD_PLL_MODE_DIV_170) || (tvco_pll_div == TBHMOD_PLL_MODE_DIV_85))  {
            /* then load 26G TVCO speed id entry */
            PHYMOD_IF_ERR_RETURN
                (phymod_mem_write(&pm_phy_copy.access, phymodMemSpeedIdTable, TSCBH_FORCED_SPEED_ID_OFFSET + start_lane, &spd_id_entry_26[mapped_speed_id][0]));
        } else if ((tvco_pll_div == (uint32_t)TBHMOD_PLL_MODE_DIV_165) || (tvco_pll_div == (uint32_t)TBHMOD_PLL_MODE_DIV_82P5)) {
            /* then load 25G TVCO speed id entry */
            PHYMOD_IF_ERR_RETURN
                (phymod_mem_write(&pm_phy_copy.access, phymodMemSpeedIdTable, TSCBH_FORCED_SPEED_ID_OFFSET + start_lane, &spd_id_entry_25[mapped_speed_id][0]));
        } else {
            /* then load 20G TVCO speed id entry */
            PHYMOD_IF_ERR_RETURN
                (phymod_mem_write(&pm_phy_copy.access, phymodMemSpeedIdTable, TSCBH_FORCED_SPEED_ID_OFFSET + start_lane, &spd_id_entry_20[mapped_speed_id][0]));
        }
    }

    /* Check the request speed VCO */
    pm_phy_copy.access.lane_mask = 1 << start_lane;
    PHYMOD_IF_ERR_RETURN
        (tbhmod_refclk_get(&pm_phy_copy.access, &ref_clk));

    /* Get requested PLL */
    /*for ethernet speed mode config set */
    if (!PHYMOD_DEVICE_OP_MODE_PCS_BYPASS_GET(phy->device_op_mode)) {
        PHYMOD_IF_ERR_RETURN
            (tbhmod_plldiv_lkup_get(&pm_phy_copy.access, mapped_speed_id, ref_clk, &request_pll_div));
    } else {
        /*first check the ref clock */
        int ref_clk_is_156p25 = 0;
        if (ref_clk == TBHMOD_REF_CLK_156P25MHZ) {
            ref_clk_is_156p25 = 1;
        }
        PHYMOD_IF_ERR_RETURN
        (blackhawk_speed_config_get(speed_config->data_rate, ref_clk_is_156p25, &request_pll_div, &is_pam4, &osr_mode));
    }

    if ((ovco_pll_div == request_pll_div) || (tvco_pll_div == request_pll_div)) {
        /* First check if pll0 is active and the new speed can be
           supported with existing VCO */
        pll_index = (ovco_pll_div == request_pll_div)? ovco_pll_index : tvco_pll_index;
    } else {
        /*this speed request can not be configured */
        PHYMOD_DEBUG_ERROR(("ERROR :: this speed can not be configured \n"));
        return PHYMOD_E_CONFIG;
    }

    /* choose the right pll index for the port */
    for (i = 0; i < num_lane; i++) {
        pm_phy_copy.access.lane_mask = 1 << (start_lane + i);
        if (!PHYMOD_LANEPBMP_MEMBER(phy->access.lane_mask, start_lane + i)) {
            continue;
        }
        PHYMOD_IF_ERR_RETURN
            (blackhawk_lane_pll_selection_set(&pm_phy_copy.access, pll_index));
    }

    pm_phy_copy.access.lane_mask = lane_mask_backup;
    /* Program OS mode */
    if (!PHYMOD_DEVICE_OP_MODE_PCS_BYPASS_GET(phy->device_op_mode)) {
        PHYMOD_IF_ERR_RETURN
            (tbhmod_pmd_osmode_set(&pm_phy_copy.access, mapped_speed_id, ref_clk));
    } else {
        for (i = 0; i < num_lane; i++) {
            pm_phy_copy.access.lane_mask = 1 << (start_lane + i);
            if (!PHYMOD_LANEPBMP_MEMBER(phy->access.lane_mask, start_lane + i)) {
                continue;
            }
            PHYMOD_IF_ERR_RETURN
                (blackhawk_osr_mode_set(&pm_phy_copy.access, osr_mode));
        }
    }

    /*next need to set certain firmware lane config to be zero*/
    firmware_lane_config.LaneConfigFromPCS = 0;
    firmware_lane_config.AnEnabled = 0;

   for (i = 0; i < num_lane; i++) {
        pm_phy_copy.access.lane_mask = 0x1 << (start_lane + i);
        if (!PHYMOD_LANEPBMP_MEMBER(phy->access.lane_mask, start_lane + i)) {
            continue;
        }
        PHYMOD_IF_ERR_RETURN
             (_tscbh_phy_firmware_lane_config_set(&pm_phy_copy, firmware_lane_config));
    }

    /* if the PAM4 mode, need to program the channel loss. In NRZ mode it is zeroed. */
    loss_in_db = firmware_lane_config.ForcePAM4Mode? speed_config->PAM4_channel_loss : 0;
    for (i = 0; i < num_lane; i++) {
        pm_phy_copy.access.lane_mask = 0x1 << (start_lane + i);
        if (!PHYMOD_LANEPBMP_MEMBER(phy->access.lane_mask, start_lane + i)) {
            continue;
        }
        PHYMOD_IF_ERR_RETURN
            (blackhawk_channel_loss_set(&pm_phy_copy.access, loss_in_db));
    }

    /* next need to enable/disable link training based on the input */
    if (speed_config->linkTraining) {
        PHYMOD_IF_ERR_RETURN
            (tscbh_phy_cl72_set(phy, speed_config->linkTraining));
    } else {
        /* disable cl72 and avoid overwriting the value from above _firmware_lane_config_set */
        for (i = 0; i < num_lane; i++) {
            pm_phy_copy.access.lane_mask = 1 << (start_lane + i);
            if (!PHYMOD_LANEPBMP_MEMBER(phy->access.lane_mask, start_lane + i)) {
                continue;
            }
            PHYMOD_IF_ERR_RETURN
                (blackhawk_clause72_control(&pm_phy_copy.access, speed_config->linkTraining));
        }
    }

    /*release the lne soft reset bit*/
    PHYMOD_MEMCPY(&pm_phy_copy, phy, sizeof(pm_phy_copy));
    PHYMOD_IF_ERR_RETURN
        (blackhawk_lane_soft_reset(&pm_phy_copy.access, 0));

    /* Release the pcs lane reset */
    pm_phy_copy.access.lane_mask = 1 << start_lane;
    PHYMOD_IF_ERR_RETURN
        (tbhmod_enable_set(&pm_phy_copy.access));

    /* first clear the current lane _mask from both */
    new_pll_state->pll1_lanes_bitmap &= ~(phy->access.lane_mask);
    new_pll_state->pll0_lanes_bitmap &= ~(phy->access.lane_mask);

    /* need to update the pll_state */
    if (pll_index) {
        new_pll_state->pll1_lanes_bitmap |= phy->access.lane_mask;
    } else {
        new_pll_state->pll0_lanes_bitmap |= phy->access.lane_mask;
    }

    return PHYMOD_E_NONE;
}

STATIC
int _tscbh_speed_table_entry_to_speed_config_get(const phymod_phy_access_t* phy,
                                        spd_id_tbl_entry_t* speed_config_entry,
                                        phymod_phy_speed_config_t* speed_config)
{
    uint32_t pll_div, refclk_in_hz, data_rate_lane;
    uint32_t pll_index;
    int osr_mode;
    tbhmod_refclk_t ref_clk;
    uint32_t vco_rate;
    phymod_phy_access_t phy_copy;
    phymod_firmware_lane_config_t firmware_lane_config;
    int num_lane;

    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));

    switch (speed_config_entry->num_lanes) {
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

    /*first figure out which pll the current port is using */
    PHYMOD_IF_ERR_RETURN
        (blackhawk_lane_pll_selection_get(&phy_copy.access, &pll_index));

    phy_copy.access.pll_idx = pll_index;

    PHYMOD_IF_ERR_RETURN
        (tscbh_phy_firmware_lane_config_get(phy, &firmware_lane_config));

    /* get the PLL div from HW */
    PHYMOD_IF_ERR_RETURN
        (blackhawk_tsc_INTERNAL_read_pll_div(&phy_copy.access, &pll_div));

    PHYMOD_IF_ERR_RETURN
        (tbhmod_refclk_get(&phy_copy.access, &ref_clk));

    if (ref_clk == TBHMOD_REF_CLK_312P5MHZ) {
        refclk_in_hz = 312500000;
    } else {
        refclk_in_hz = 156250000;
    }
    PHYMOD_IF_ERR_RETURN
        (blackhawk_tsc_get_vco_from_refclk_div(&phy_copy.access, refclk_in_hz, pll_div, &vco_rate, 0));

    PHYMOD_IF_ERR_RETURN
        (blackhawk_osr_mode_get(&phy_copy.access, &osr_mode));

    /*
     * data_rate_lane = vco_rate / 66 * 64.
     * For 25G VCO case, round down the data_rate_lane to 24000 by convention.
     */
    if (vco_rate == 20625000) {
        data_rate_lane = 20000;
    } else if (vco_rate == 25000000) {
        data_rate_lane = 24000;
    } else {
        data_rate_lane = 25000;
    }

    /* next check if PAM4 mode enabled */
    if (firmware_lane_config.ForcePAM4Mode) {
        data_rate_lane = data_rate_lane  << 1;
    } else {
        /* to get the over sample value */
        data_rate_lane = data_rate_lane >> osr_mode;
    }

    speed_config->data_rate = data_rate_lane * num_lane;

    PHYMOD_IF_ERR_RETURN
        (tbhmod_fec_arch_decode_get(speed_config_entry->fec_arch, &(speed_config->fec_type)));

    return PHYMOD_E_NONE;
}


STATIC
int _tscbh_speed_id_to_speed_config_get(const phymod_phy_access_t* phy,
                                        int speed_id,
                                        int num_lane,
                                        phymod_phy_speed_config_t* speed_config)
{
    uint32_t pll_div, refclk_in_hz, data_rate_lane;
    uint32_t pll_index;
    int osr_mode;
    tbhmod_refclk_t ref_clk;
    uint32_t vco_rate;
    phymod_phy_access_t phy_copy;
    phymod_firmware_lane_config_t firmware_lane_config;
    spd_id_tbl_entry_t speed_config_entry;
    uint32_t packed_entry[5];

    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));

    /*first figure out which pll the current port is using */
    PHYMOD_IF_ERR_RETURN
        (blackhawk_lane_pll_selection_get(&phy_copy.access, &pll_index));

    phy_copy.access.pll_idx = pll_index;

    PHYMOD_IF_ERR_RETURN
        (tscbh_phy_firmware_lane_config_get(phy, &firmware_lane_config));

    /* get the PLL div from HW */
    PHYMOD_IF_ERR_RETURN
        (blackhawk_tsc_INTERNAL_read_pll_div(&phy_copy.access, &pll_div));

    PHYMOD_IF_ERR_RETURN
        (tbhmod_refclk_get(&phy_copy.access, &ref_clk));

    if (ref_clk == TBHMOD_REF_CLK_312P5MHZ) {
        refclk_in_hz = 312500000;
    } else {
        refclk_in_hz = 156250000;
    }
    PHYMOD_IF_ERR_RETURN
        (blackhawk_tsc_get_vco_from_refclk_div(&phy_copy.access, refclk_in_hz, pll_div, &vco_rate, 0));

    PHYMOD_IF_ERR_RETURN
        (blackhawk_osr_mode_get(&phy_copy.access, &osr_mode));
    /*
     * data_rate_lane = vco_rate / 66 * 64.
     * For 25G VCO case, round down the data_rate_lane to 24000 by convention.
     */
    if (vco_rate == 20625000) {
        data_rate_lane = 20000;
    } else if (vco_rate == 25000000) {
        data_rate_lane = 24000;
    } else {
        data_rate_lane = 25000;
    }

    /* next check if PAM4 mode enabled */
    if (firmware_lane_config.ForcePAM4Mode) {
        data_rate_lane = data_rate_lane  << 1;
    } else {
        /* to get the over sample value */
        data_rate_lane = data_rate_lane >> osr_mode;
    }

    speed_config->data_rate = data_rate_lane * num_lane;

    PHYMOD_IF_ERR_RETURN
        (phymod_mem_read(&phy_copy.access, phymodMemSpeedIdTable, speed_id, packed_entry));

    spd_ctrl_unpack_spd_id_tbl_entry(packed_entry, &speed_config_entry);

    PHYMOD_IF_ERR_RETURN
        (tbhmod_fec_arch_decode_get(speed_config_entry.fec_arch, &(speed_config->fec_type)));

    return PHYMOD_E_NONE;
}

int tscbh_phy_speed_config_get(const phymod_phy_access_t* phy, phymod_phy_speed_config_t* speed_config)
{
    uint32_t cl72_enable;
    phymod_phy_access_t phy_copy;
    phymod_firmware_lane_config_t firmware_lane_config;
    int start_lane, num_lane, speed_id;
    uint32_t packed_entry[20];
    spd_id_tbl_entry_t speed_config_entry;
    int an_en, an_done, osr_mode;
    uint32_t pll_div, vco_freq_khz, pll_index, lane_mask_backup;
    tbhmod_refclk_t ref_clk;

    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));
    lane_mask_backup = phy->access.lane_mask;

    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(&phy->access, &start_lane, &num_lane));

    PHYMOD_IF_ERR_RETURN
        (tscbh_phy_firmware_lane_config_get(phy, &firmware_lane_config));

    speed_config->pmd_lane_config.AnEnabled          = firmware_lane_config.AnEnabled;
    speed_config->pmd_lane_config.Cl72AutoPolEn      = firmware_lane_config.Cl72AutoPolEn;
    speed_config->pmd_lane_config.Cl72RestTO         = firmware_lane_config.Cl72RestTO;
    speed_config->pmd_lane_config.DfeOn              = firmware_lane_config.DfeOn;
    speed_config->pmd_lane_config.ForceBrDfe         = firmware_lane_config.ForceBrDfe;
    speed_config->pmd_lane_config.ForceExtenedReach  = firmware_lane_config.ForceExtenedReach;
    speed_config->pmd_lane_config.ForceNormalReach   = firmware_lane_config.ForceNormalReach;
    speed_config->pmd_lane_config.ForceNRZMode       = firmware_lane_config.ForceNRZMode;
    speed_config->pmd_lane_config.ForcePAM4Mode      = firmware_lane_config.ForcePAM4Mode;
    speed_config->pmd_lane_config.LaneConfigFromPCS  = firmware_lane_config.LaneConfigFromPCS;
    speed_config->pmd_lane_config.LpDfeOn            = firmware_lane_config.LpDfeOn;
    speed_config->pmd_lane_config.LpPrecoderEnabled  = firmware_lane_config.LpPrecoderEnabled;
    speed_config->pmd_lane_config.MediaType          = firmware_lane_config.MediaType;
    speed_config->pmd_lane_config.ScramblingDisable  = firmware_lane_config.ScramblingDisable;
    speed_config->pmd_lane_config.UnreliableLos      = firmware_lane_config.UnreliableLos;

    /* for ethernet port */
    if (!PHYMOD_DEVICE_OP_MODE_PCS_BYPASS_GET(phy->device_op_mode)) {
        /* first read speed id from resolved status */
        PHYMOD_IF_ERR_RETURN
            (tbhmod_speed_id_get(&phy_copy.access, &speed_id));

        /* next check check if AN enabled */
        PHYMOD_IF_ERR_RETURN
           (tbhmod_autoneg_status_get(&phy_copy.access, &an_en, &an_done));

        /* first read the speed entry and then decode the speed and FEC type */
        phy_copy.access.lane_mask = 1 << 0;
        PHYMOD_IF_ERR_RETURN
            (phymod_mem_read(&phy_copy.access, phymodMemSpeedIdTable, speed_id, packed_entry));

        PHYMOD_MEMSET(&speed_config_entry,0,sizeof(speed_config_entry));
        /*decode speed entry */
        spd_ctrl_unpack_spd_id_tbl_entry(packed_entry, &speed_config_entry);


        PHYMOD_IF_ERR_RETURN
            (_tscbh_speed_table_entry_to_speed_config_get(phy, &speed_config_entry, speed_config));

        /* if autoneg enabled, needs to update the FEC_ARCH based on the An resolved status */
        /* however for 100G 4 lane NO FEC/with FEC, will rely on the speed id entry since the AN
        resolved status is not correct */
        if ((an_en && an_done) && (speed_id != SPEED_ID_INDEX_100G_4_LANE_CL73_KR4) &&
            (speed_id != SPEED_ID_INDEX_100G_4_LANE_CL73_CR4)) {
            uint8_t fec_arch;
            phy_copy.access.lane_mask = 0x1 << start_lane;
            PHYMOD_IF_ERR_RETURN
                (tbhmod_autoneg_fec_status_get(&phy_copy.access, &fec_arch));
            PHYMOD_IF_ERR_RETURN
                (tbhmod_fec_arch_decode_get(fec_arch, &(speed_config->fec_type)));
        }
    } else {
        /*first figure out which pll the current port is using */
        PHYMOD_IF_ERR_RETURN
            (blackhawk_lane_pll_selection_get(&phy_copy.access, &pll_index));

        phy_copy.access.pll_idx = pll_index;
         /* get the PLL div from HW */
        PHYMOD_IF_ERR_RETURN
            (blackhawk_tsc_INTERNAL_read_pll_div(&phy_copy.access, &pll_div));

        phy_copy.access.lane_mask = 0x1;
        /* next read what's the ref clock */
        PHYMOD_IF_ERR_RETURN
           (tbhmod_refclk_get(&phy_copy.access, &ref_clk));

        if (ref_clk == TBHMOD_REF_CLK_156P25MHZ) {
            PHYMOD_IF_ERR_RETURN
                (blackhawk_tsc_get_vco_from_refclk_div(&phy_copy.access, REF_CLOCK_156P25, pll_div, &vco_freq_khz, 0));
        } else {
            PHYMOD_IF_ERR_RETURN
                (blackhawk_tsc_get_vco_from_refclk_div(&phy_copy.access, REF_CLOCK_312P5, pll_div, &vco_freq_khz, 0));
        }

        phy_copy.access.lane_mask = lane_mask_backup;

        PHYMOD_IF_ERR_RETURN
            (blackhawk_osr_mode_get(&phy_copy.access, &osr_mode));
        /* next check if PAM4 mode enabled */
        if (firmware_lane_config.ForcePAM4Mode) {
            speed_config->data_rate = (vco_freq_khz  << 1) / 1000;
        } else {
            /* to get the over sample value */
            if (osr_mode == 0) {
                speed_config->data_rate = (vco_freq_khz) / 1000;
            } else if(osr_mode == 1) {
                speed_config->data_rate = (vco_freq_khz  >> 1) / 1000;
            } else if (osr_mode == 2) {
                speed_config->data_rate = (vco_freq_khz  >> 2) / 1000;
            }
        }
    }

    /* next get the cl72 enable status */
    phy_copy.access.lane_mask = 0x1 << start_lane;
    PHYMOD_IF_ERR_RETURN
        (blackhawk_clause72_control_get(&phy_copy.access, &cl72_enable));
    speed_config->linkTraining = cl72_enable;

    return PHYMOD_E_NONE;
}


int tscbh_phy_cl72_set(const phymod_phy_access_t* phy, uint32_t cl72_en)
{
    struct blackhawk_tsc_uc_lane_config_st serdes_firmware_config;
    phymod_firmware_lane_config_t firmware_lane_config;
    int start_lane, num_lane, i, precoder_en;
    uint32_t lane_reset, pcs_lane_enable;
    phymod_phy_access_t pm_phy_copy;

    PHYMOD_MEMCPY(&pm_phy_copy, phy, sizeof(pm_phy_copy));
    PHYMOD_IF_ERR_RETURN(blackhawk_tsc_get_uc_lane_cfg(&pm_phy_copy.access, &serdes_firmware_config));

    if ((serdes_firmware_config.field.dfe_on == 0) && cl72_en) {
      PHYMOD_DEBUG_ERROR(("ERROR :: DFE is off : Can not start CL72/CL93 with no DFE\n"));
      return PHYMOD_E_CONFIG;
    }

    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(&phy->access, &start_lane, &num_lane));

    /*first check if lane is in reset */
    PHYMOD_IF_ERR_RETURN
        (blackhawk_lane_soft_reset_get(&pm_phy_copy.access, &lane_reset));

    /*next check if PCS lane is in reset */
    PHYMOD_IF_ERR_RETURN
        (tbhmod_enable_get(&pm_phy_copy.access, &pcs_lane_enable));

    /* disable pcs lane if pcs lane not in rset */
    if (pcs_lane_enable) {
        pm_phy_copy.access.lane_mask = 1 << start_lane;
        PHYMOD_IF_ERR_RETURN
            (tbhmod_disable_set(&pm_phy_copy.access));
        /* add the pcs disable SW WAR */
        PHYMOD_IF_ERR_RETURN
            (tbhmod_pcs_reset_sw_war(&phy->access));
    }

    /* if lane is not in reset, then reset the lane first */
    if (!lane_reset) {
        PHYMOD_MEMCPY(&pm_phy_copy, phy, sizeof(pm_phy_copy));
        PHYMOD_IF_ERR_RETURN
            (blackhawk_lane_soft_reset(&pm_phy_copy.access, 1));
    }

    /* next need to clear both force ER and NR config on the firmware lane config side
    if link training enable is set */
    if (cl72_en) {
        PHYMOD_IF_ERR_RETURN
            (tscbh_phy_firmware_lane_config_get(phy, &firmware_lane_config));

        firmware_lane_config.ForceNormalReach = 0;
        firmware_lane_config.ForceExtenedReach = 0;

         PHYMOD_IF_ERR_RETURN
            (_tscbh_phy_firmware_lane_config_set(phy, firmware_lane_config));
    } else {
        /* disable Tx pre-coding and set Rx in NR mode */
        for (i = 0; i < num_lane; i++) {
            pm_phy_copy.access.lane_mask = 1 << (start_lane + i);
            precoder_en = 0;
            PHYMOD_IF_ERR_RETURN
                (tscbh_phy_tx_pam4_precoder_enable_get(&pm_phy_copy, &precoder_en));
            if (precoder_en) {
                PHYMOD_IF_ERR_RETURN
                    (tscbh_phy_tx_pam4_precoder_enable_set(&pm_phy_copy, 0));
            }
            PHYMOD_IF_ERR_RETURN
                (tscbh_phy_firmware_lane_config_get(&pm_phy_copy, &firmware_lane_config));
            if (firmware_lane_config.ForcePAM4Mode) {
                firmware_lane_config.ForceNormalReach = 1;
                firmware_lane_config.ForceExtenedReach = 0;
                PHYMOD_IF_ERR_RETURN
                    (_tscbh_phy_firmware_lane_config_set(&pm_phy_copy, firmware_lane_config));
            }
        }
    }
    for (i = 0; i < num_lane; i++) {
        pm_phy_copy.access.lane_mask = 1 << (start_lane + i);
        if (!PHYMOD_LANEPBMP_MEMBER(phy->access.lane_mask, start_lane + i)) {
            continue;
        }
        PHYMOD_IF_ERR_RETURN
            (blackhawk_clause72_control(&pm_phy_copy.access, cl72_en));
    }

    /* release the ln dp reset */
    if (!lane_reset) {
        PHYMOD_MEMCPY(&pm_phy_copy, phy, sizeof(pm_phy_copy));
        PHYMOD_IF_ERR_RETURN
            (blackhawk_lane_soft_reset(&pm_phy_copy.access, 0));
    }

    /* re-enable pcs lane if pcs lane not in rset */
    if (pcs_lane_enable) {
        pm_phy_copy.access.lane_mask = 1 << start_lane;
        PHYMOD_IF_ERR_RETURN
            (tbhmod_enable_set(&pm_phy_copy.access));
    }

    return PHYMOD_E_NONE;
}

int tscbh_phy_cl72_get(const phymod_phy_access_t* phy, uint32_t* cl72_en)
{
    phymod_phy_access_t pm_phy_copy;

    PHYMOD_MEMCPY(&pm_phy_copy, phy, sizeof(pm_phy_copy));

    PHYMOD_IF_ERR_RETURN
        (blackhawk_clause72_control_get(&pm_phy_copy.access, cl72_en));

    return PHYMOD_E_NONE;
}


int tscbh_phy_cl72_status_get(const phymod_phy_access_t* phy, phymod_cl72_status_t* status)
{
    int i;
    uint32_t tmp_status;
    int start_lane, num_lane;
    phymod_phy_access_t phy_copy;
    int an_en, an_done, speed_id;
    spd_id_tbl_entry_t speed_config_entry;
    uint32_t packed_entry[5];

    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));
    status->locked = 1;

    /* next figure out the lane num and start_lane based on the input */
    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(&phy->access, &start_lane, &num_lane));
    /* check check if AN enabled */
    PHYMOD_IF_ERR_RETURN
       (tbhmod_autoneg_status_get(&phy_copy.access, &an_en, &an_done));

    if (an_en && an_done) {
        PHYMOD_IF_ERR_RETURN
            (tbhmod_speed_id_get(&phy_copy.access, &speed_id));
        PHYMOD_IF_ERR_RETURN
            (phymod_mem_read(&phy_copy.access, phymodMemSpeedIdTable, speed_id, packed_entry));
        spd_ctrl_unpack_spd_id_tbl_entry(packed_entry, &speed_config_entry);
        /* Update num_lane and lane_mask for AN port */
        num_lane = 1 << speed_config_entry.num_lanes;
        /* Update lane_mask */
        phy_copy.access.lane_mask = 0x0;
        for (i = 0; i < num_lane; i++) {
            phy_copy.access.lane_mask |= 0x1 << (i + start_lane);
        }
    }

    for (i = 0; i < num_lane; i++) {
        if (!PHYMOD_LANEPBMP_MEMBER(phy->access.lane_mask, start_lane + i)) {
            continue;
        }
        phy_copy.access.lane_mask = 0x1 << (i + start_lane);
        tmp_status = 1;
        PHYMOD_IF_ERR_RETURN(blackhawk_pmd_cl72_receiver_status(&phy_copy.access, &tmp_status));
        if (tmp_status == 0) {
            status->locked = 0;
            return PHYMOD_E_NONE;
        }
    }

    return PHYMOD_E_NONE;
}

int tscbh_phy_loopback_set(const phymod_phy_access_t* phy, phymod_loopback_mode_t loopback, uint32_t enable)
{
    int i;
    int start_lane, num_lane;
    phymod_phy_access_t phy_copy;

    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));

    /* next figure out the lane num and start_lane based on the input */
    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(&phy->access, &start_lane, &num_lane));

    switch (loopback) {
    case phymodLoopbackGlobal :
    case phymodLoopbackGlobalPMD :
        if (enable) {
            phy_copy.access.lane_mask = 1 << start_lane;
            PHYMOD_IF_ERR_RETURN
                (tbhmod_disable_set(&phy_copy.access));
            /*first squelch rx */
            for (i = 0; i < num_lane; i++) {
                if (!PHYMOD_LANEPBMP_MEMBER(phy->access.lane_mask, start_lane + i)) {
                    continue;
                }
                phy_copy.access.lane_mask = 0x1 << (i + start_lane);
                PHYMOD_IF_ERR_RETURN(blackhawk_pmd_force_signal_detect(&phy_copy.access,  (int) enable, (int) 0));
            }
            phy_copy.access.lane_mask = 1 << start_lane;
            PHYMOD_IF_ERR_RETURN
                (tbhmod_enable_set(&phy_copy.access));
        }
        for (i = 0; i < num_lane; i++) {
            if (!PHYMOD_LANEPBMP_MEMBER(phy->access.lane_mask, start_lane + i)) {
                continue;
            }
            phy_copy.access.lane_mask = 0x1 << (i + start_lane);
            PHYMOD_IF_ERR_RETURN(blackhawk_tsc_tx_disable(&phy_copy.access, enable));
            PHYMOD_IF_ERR_RETURN(blackhawk_tsc_dig_lpbk(&phy_copy.access, (uint8_t) enable));
        }
        if (!enable) {
            for (i = 0; i < num_lane; i++) {
                if (!PHYMOD_LANEPBMP_MEMBER(phy->access.lane_mask, start_lane + i)) {
                    continue;
                }
                phy_copy.access.lane_mask = 0x1 << (i + start_lane);
                PHYMOD_IF_ERR_RETURN(blackhawk_pmd_force_signal_detect(&phy_copy.access,  (int) enable, (int) 0));
            }
        }
        break;
    case phymodLoopbackRemotePMD :
        for (i = 0; i < num_lane; i++) {
            if (!PHYMOD_LANEPBMP_MEMBER(phy->access.lane_mask, start_lane + i)) {
                continue;
            }
            phy_copy.access.lane_mask = 0x1 << (i + start_lane);
            PHYMOD_IF_ERR_RETURN(blackhawk_tsc_rmt_lpbk(&phy_copy.access, (uint8_t)enable));
        }
        break;
    case phymodLoopbackRemotePCS :
    default :
        PHYMOD_RETURN_WITH_ERR(PHYMOD_E_UNAVAIL,
                               (_PHYMOD_MSG("This mode is not supported\n")));
        break;
    }
    return PHYMOD_E_NONE;
}

int tscbh_phy_loopback_get(const phymod_phy_access_t* phy, phymod_loopback_mode_t loopback, uint32_t* enable)
{
    int start_lane, num_lane;
    phymod_phy_access_t phy_copy;

    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));

    /*next figure out the lane num and start_lane based on the input*/
    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(&phy->access, &start_lane, &num_lane));

    switch (loopback) {
    case phymodLoopbackGlobal :
    case phymodLoopbackGlobalPMD :
        PHYMOD_IF_ERR_RETURN(blackhawk_tsc_dig_lpbk_get(&phy_copy.access, enable));
        break;
    case phymodLoopbackRemotePMD :
        PHYMOD_IF_ERR_RETURN(blackhawk_tsc_rmt_lpbk_get(&phy_copy.access, enable));
        break;
    case phymodLoopbackRemotePCS :
    default :
        return PHYMOD_E_UNAVAIL;
    }
    return PHYMOD_E_NONE;
}

/* Core initialization
 * (PASS1)
 * 1.  De-assert PMD core and PMD lane reset
 * 2.  Set heartbeat for comclk
 * 3.  Micro code load and verify
 * 4.  Start CRC Calculation (opt)
 * (PASS2)
 * 5.  Configure PMD lane mapping and PCS lane swap
 * 6.  De-assert micro reset
 * 7.  Wait for uc_active = 1
 * 8.  Initialize software information table for the micro
 * 9.  Config PMD polarity
 * 10. AFE/PLL configuration
 * 11. Set core_from_pcs_config
 * 12. Program AN default timer
 * 13. Load sd_id_table, am_table and um_table into TSC memory
 * 14. Release core DP soft reset
 */
STATIC
int _tscbh_core_init_pass1(const phymod_core_access_t* core, const phymod_core_init_config_t* init_config, const phymod_core_status_t* core_status)
{
    int rv, lane;
    phymod_phy_access_t phy_access;
    phymod_core_access_t  core_copy;
    uint32_t uc_enable = 0, rev_num;
    phymod_polarity_t tmp_pol;
    ucode_info_t ucode;
    unsigned int blackhawk_ucode_len;
    unsigned char *blackhawk_ucode;

    if (init_config->firmware_load_method == phymodFirmwareLoadMethodNone) {
        return PHYMOD_E_NONE;
    }

    TSCBH_CORE_TO_PHY_ACCESS(&phy_access, core);
    PHYMOD_MEMCPY(&core_copy, core, sizeof(core_copy));
    core_copy.access.lane_mask = 0x1;

    PHYMOD_MEMSET(&tmp_pol, 0x0, sizeof(tmp_pol));

    /* 1. De-assert PMD core power and core data path reset */
    PHYMOD_IF_ERR_RETURN
        (tbhmod_pmd_reset_seq(&core_copy.access, core_status->pmd_active));

    core_copy.access.pll_idx = 1;
    PHYMOD_IF_ERR_RETURN
        (blackhawk_tsc_core_dp_reset(&core_copy.access, 1));
    core_copy.access.pll_idx = 0;
    PHYMOD_IF_ERR_RETURN
        (blackhawk_tsc_core_dp_reset(&core_copy.access, 1));

    /* De-assert PMD lane reset */
    for (lane = 0; lane < TSCBH_NOF_LANES_IN_CORE; lane++) {
        phy_access.access.lane_mask = 1 << lane;
        PHYMOD_IF_ERR_RETURN
          (tbhmod_pmd_x4_reset(&phy_access.access));
    }

    PHYMOD_IF_ERR_RETURN
        (blackhawk_uc_active_get(&phy_access.access, &uc_enable));
    if (uc_enable) return PHYMOD_E_NONE;

    /* 2. Set the heart beat, default is 156.25M */
    if (init_config->interface.ref_clock != phymodRefClk156Mhz) {
        PHYMOD_IF_ERR_RETURN
            (blackhawk_refclk_set(&core_copy.access, init_config->interface.ref_clock));
        PHYMOD_IF_ERR_RETURN
            (tbhmod_refclk_set(&phy_access.access, TBHMOD_REF_CLK_312P5MHZ));
    }

    /* 3. set the micro stack size */
    phy_access.access.lane_mask = 0x1;
    PHYMOD_IF_ERR_RETURN
        (tbhmod_revid_get(&phy_access.access, &rev_num));
    if (rev_num == TBHMOD_REV_NUM_0) {
        ucode.stack_size = blackhawk_ucode_stack_size_rev0;
        ucode.ucode_size = blackhawk_ucode_len_rev0;
        blackhawk_ucode_len = blackhawk_ucode_len_rev0;
        blackhawk_ucode = blackhawk_ucode_rev0;
    } else {
        ucode.stack_size = blackhawk_ucode_stack_size_rev1;
        ucode.ucode_size = blackhawk_ucode_len_rev1;
        blackhawk_ucode_len = blackhawk_ucode_len_rev1;
        blackhawk_ucode = blackhawk_ucode_rev1;
    }

    PHYMOD_IF_ERR_RETURN
    (blackhawk_tsc_uc_reset_with_info(&core_copy.access , 1, ucode));

    /* 4. Micro code load and verify */
    rv = _tscbh_core_firmware_load(&core_copy, init_config);
    if (rv != PHYMOD_E_NONE) {
        PHYMOD_DEBUG_ERROR(("devad 0x%"PRIx32" lane 0x%"PRIx32": UC firmware-load failed\n", core->access.addr, core->access.lane_mask));
        PHYMOD_IF_ERR_RETURN(rv);
    }

     /* need to check if the ucode load is correct or not */
    if (init_config->firmware_load_method != phymodFirmwareLoadMethodNone) {
        /* Read-back uCode from Program RAM and verify against ucode_image.
         * The comparison is 4-byte at a time, which is time-consuming.
         * Thus, this code is only needed for debug purpose.
         */
        if (PHYMOD_CORE_INIT_F_FIRMWARE_LOAD_VERIFY_GET(init_config)) {
            rv = blackhawk_tsc_ucode_load_verify(&core_copy.access, (uint8_t *) blackhawk_ucode, blackhawk_ucode_len);
            if (rv != PHYMOD_E_NONE) {
                PHYMOD_DEBUG_ERROR(("devad 0x%x lane 0x%x: UC load-verify failed\n", core->access.addr, core->access.lane_mask));
                PHYMOD_IF_ERR_RETURN(rv);
            }
        }
    }

    return PHYMOD_E_NONE;
}


STATIC
int _tscbh_core_init_pass2(const phymod_core_access_t* core, const phymod_core_init_config_t* init_config, const phymod_core_status_t* core_status)
{
    phymod_phy_access_t phy_access, phy_access_copy;
    phymod_core_access_t  core_copy;
    enum blackhawk_tsc_pll_refclk_enum refclk;
    phymod_polarity_t tmp_pol;
    int lane, pll_index;
    uint32_t speed_id_load_size, am_table_load_size, um_table_load_size;
    uint32_t tvco_rate, i, tvco_pll_index, rev_num;

    TSCBH_CORE_TO_PHY_ACCESS(&phy_access, core);
    phy_access_copy = phy_access;
    PHYMOD_MEMCPY(&core_copy, core, sizeof(core_copy));
    core_copy.access.lane_mask = 0x1;
    phy_access_copy = phy_access;
    phy_access_copy.access = core->access;
    phy_access_copy.access.lane_mask = 0x1;
    phy_access_copy.type = core->type;
    tvco_pll_index = core->access.tvco_pll_index;

    /* 5. Configure PMD lane mapping and PCS lane swap */
    PHYMOD_IF_ERR_RETURN
        (tscbh_core_lane_map_set(&core_copy, &init_config->lane_map));

    if (init_config->interface.ref_clock == phymodRefClk156Mhz) {
        refclk = BLACKHAWK_TSC_PLL_REFCLK_156P25MHZ;
        /* first check tvco index */
        if (tvco_pll_index == 1) {
            PHYMOD_IF_ERR_RETURN(
                 tbhmod_pll_to_vco_get(TBHMOD_REF_CLK_156P25MHZ, init_config->pll1_div_init_value, &tvco_rate));
        } else if (tvco_pll_index == 0) {
            PHYMOD_IF_ERR_RETURN(
                 tbhmod_pll_to_vco_get(TBHMOD_REF_CLK_156P25MHZ, init_config->pll0_div_init_value, &tvco_rate));
        } else {
            PHYMOD_DEBUG_ERROR(("Unsupported tvco index\n"));
            return PHYMOD_E_UNAVAIL;
        }
    } else if (init_config->interface.ref_clock == phymodRefClk312Mhz) {
        refclk = BLACKHAWK_TSC_PLL_REFCLK_312P5MHZ;
        /* first check tvco index */
        if (tvco_pll_index == 1) {
            PHYMOD_IF_ERR_RETURN(
                 tbhmod_pll_to_vco_get(TBHMOD_REF_CLK_312P5MHZ, init_config->pll1_div_init_value, &tvco_rate));
        } else if (tvco_pll_index == 0) {
            PHYMOD_IF_ERR_RETURN(
                 tbhmod_pll_to_vco_get(TBHMOD_REF_CLK_312P5MHZ, init_config->pll0_div_init_value, &tvco_rate));
        } else {
            PHYMOD_DEBUG_ERROR(("Unsupported tvco index\n"));
            return PHYMOD_E_UNAVAIL;
        }
    } else {
        PHYMOD_DEBUG_ERROR(("Unsupported reference clock.\n"));
        return PHYMOD_E_UNAVAIL;
    }

    /*next need to load speed id table and AM table */
    speed_id_load_size = TSCBH_SPEED_ID_TABLE_SIZE > TSCBH_HW_SPEED_ID_TABLE_SIZE ? TSCBH_HW_SPEED_ID_TABLE_SIZE : TSCBH_SPEED_ID_TABLE_SIZE;
    am_table_load_size = TSCBH_AM_TABLE_SIZE > TSCBH_HW_AM_TABLE_SIZE ? TSCBH_HW_AM_TABLE_SIZE : TSCBH_AM_TABLE_SIZE;
    um_table_load_size = TSCBH_UM_TABLE_SIZE > TSCBH_HW_UM_TABLE_SIZE ? TSCBH_HW_UM_TABLE_SIZE : TSCBH_UM_TABLE_SIZE;

    if (tvco_rate == TBHMOD_VCO_26G) {
        for (i = 0; i < speed_id_load_size; i++) {
            PHYMOD_IF_ERR_RETURN
                (phymod_mem_write(&core_copy.access, phymodMemSpeedIdTable, i, &spd_id_entry_26[i][0]));
        }
    } else if (tvco_rate == TBHMOD_VCO_25G) {
        for (i = 0; i < speed_id_load_size; i++) {
            PHYMOD_IF_ERR_RETURN
                (phymod_mem_write(&core_copy.access, phymodMemSpeedIdTable, i, &spd_id_entry_25[i][0]));
        }
    } else if (tvco_rate == TBHMOD_VCO_20G) {
        for (i = 0; i < speed_id_load_size; i++) {
            PHYMOD_IF_ERR_RETURN
                (phymod_mem_write(&core_copy.access, phymodMemSpeedIdTable, i, &spd_id_entry_20[i][0]));
        }
    }

    for (i = 0; i < am_table_load_size; i++) {
        PHYMOD_IF_ERR_RETURN
            (phymod_mem_write(&core_copy.access, phymodMemAMTable, i,  &am_table_entry[i][0]));
    }

    for (i = 0; i < um_table_load_size; i++) {
        PHYMOD_IF_ERR_RETURN
            (phymod_mem_write(&core_copy.access, phymodMemUMTable, i,  &um_table_entry[i][0]));
    }

    /*need to update speed_priority_mapping_table with correct speed id */
    PHYMOD_IF_ERR_RETURN
        (phymod_mem_write(&core_copy.access, phymodMemSpeedPriorityMapTable, 0,  &speed_priority_mapping_table[0][0]));

    if (init_config->firmware_load_method != phymodFirmwareLoadMethodNone) {
        ucode_info_t ucode;
        PHYMOD_IF_ERR_RETURN
            (tbhmod_revid_get(&phy_access.access, &rev_num));
        if (rev_num == TBHMOD_REV_NUM_0) {
            ucode.stack_size = blackhawk_ucode_stack_size_rev0;
            ucode.ucode_size = blackhawk_ucode_len_rev0;
            ucode.crc_value  = blackhawk_ucode_crc_rev0;
        } else {
            ucode.stack_size = blackhawk_ucode_stack_size_rev1;
            ucode.ucode_size = blackhawk_ucode_len_rev1;
            ucode.crc_value  = blackhawk_ucode_crc_rev1;
        }

        /* per PLP, uc will set this bit, so SW does not need to do this anymore */
        /* PHYMOD_IF_ERR_RETURN
            (blackhawk_uc_active_set(&core_copy.access ,1)); */

        /* 6. Release uc reset */
        PHYMOD_IF_ERR_RETURN
        (blackhawk_tsc_uc_reset_with_info(&core_copy.access , 0, ucode));

        /* 7. Wait for uc_active = 1 */
        PHYMOD_IF_ERR_RETURN
            (blackhawk_tsc_wait_uc_active(&phy_access.access));

        for (lane = 0; lane < TSCBH_NOF_LANES_IN_CORE; lane++) {
            phy_access_copy.access.lane_mask = 1 << lane;
            PHYMOD_IF_ERR_RETURN
                (blackhawk_pmd_ln_h_rstb_pkill_override(&phy_access_copy.access, 0x1));
        }

        /* 8. Initialize software information table for the macro */
        PHYMOD_IF_ERR_RETURN
            (blackhawk_tsc_init_blackhawk_tsc_info(&core_copy.access));

        /* check the FW crc checksum error */
        if (!PHYMOD_CORE_INIT_F_BYPASS_CRC_CHECK_GET(init_config)) {
            PHYMOD_IF_ERR_RETURN
                (blackhawk_tsc_check_ucode_crc(&core_copy.access, ucode.crc_value, 200));
        }

        /* release pmd lane hard reset */
        for (lane = 0; lane < TSCBH_NOF_LANES_IN_CORE; lane++) {
            phy_access_copy.access.lane_mask = 1 << lane;
            PHYMOD_IF_ERR_RETURN
                (blackhawk_pmd_ln_h_rstb_pkill_override(&phy_access_copy.access, 0x0));
        }
    } else {
        PHYMOD_IF_ERR_RETURN
            (blackhawk_tsc_firmware_load_none_init_blackhawk_tsc_info(&core_copy.access));
    }

    /* 9. RX/TX polarity configuration */
    for (lane = 0; lane < TSCBH_NOF_LANES_IN_CORE; lane++) {
        phy_access_copy.access.lane_mask = 1 << lane;
        tmp_pol.tx_polarity = (init_config->polarity_map.tx_polarity) >> lane & 0x1;
        tmp_pol.rx_polarity = (init_config->polarity_map.rx_polarity) >> lane & 0x1;
        PHYMOD_IF_ERR_RETURN
            (tscbh_phy_polarity_set(&phy_access_copy, &tmp_pol));
        /* clear the tmp vairiable */
        PHYMOD_MEMSET(&tmp_pol, 0x0, sizeof(tmp_pol));
    }

    /* 10. AFE/PLL configuration */
    for (pll_index = 0; pll_index < 2; pll_index++) {
        core_copy.access.pll_idx = pll_index;
        PHYMOD_IF_ERR_RETURN
            (blackhawk_afe_pll_reg_set(&core_copy.access, &init_config->afe_pll));
    }

    /* PLL_DIV config for both PLL0 and PLL1 */
    if (init_config->pll0_div_init_value != (uint32_t)phymod_TSCBH_PLL_DIVNONE) {
        core_copy.access.pll_idx = 0;
        PHYMOD_IF_ERR_RETURN
            (blackhawk_tsc_configure_pll_refclk_div(&core_copy.access,
                                                      refclk,
                                                      init_config->pll0_div_init_value));
    }

    if (init_config->pll1_div_init_value != (uint32_t)phymod_TSCBH_PLL_DIVNONE) {
        core_copy.access.pll_idx = 1;
        PHYMOD_IF_ERR_RETURN
            (blackhawk_tsc_configure_pll_refclk_div(&core_copy.access,
                                                      refclk,
                                                      init_config->pll1_div_init_value));
    }

    /* 12. Program AN default timer  for both MMP0 and MMP1*/
    core_copy.access.lane_mask = 0x1;
    core_copy.access.pll_idx = 0;
    PHYMOD_IF_ERR_RETURN
        (tbhmod_set_an_timers(&core_copy.access, init_config->interface.ref_clock, NULL));

    /* set the PMD debug level to be 2 as default */
    core_copy.access.lane_mask = 0xff;
    PHYMOD_IF_ERR_RETURN
        (blackhawk_tsc_set_usr_ctrl_lane_event_log_level(&core_copy.access, 2));

    core_copy.access.lane_mask = 0x10;
    core_copy.access.pll_idx = 0;
    PHYMOD_IF_ERR_RETURN
        (tbhmod_set_an_timers(&core_copy.access, init_config->interface.ref_clock, NULL));

    /* disable base-r FEC 1bit and 2-bit err interrupt */
    PHYMOD_IF_ERR_RETURN
        (tbhmod_ecc_error_intr_enable_set(&phy_access_copy.access, phymodIntrEccBaseRFEC, 0));

    /* 13. Release core DP soft reset for both PLLs */
    core_copy.access.lane_mask = 0x1;
    core_copy.access.pll_idx = 0;
    /* next need to config PMD micro clock source, chip defualt is PLL0 and SW driver will overwrite the
    default to use PLL1 */
    PHYMOD_IF_ERR_RETURN
        (blackhawk_micro_clk_source_select(&core_copy.access, 1));
    PHYMOD_IF_ERR_RETURN
        (blackhawk_tsc_core_dp_reset(&core_copy.access, 0));
    core_copy.access.pll_idx = 1;
    PHYMOD_IF_ERR_RETURN
        (blackhawk_tsc_core_dp_reset(&core_copy.access, 0));

   return PHYMOD_E_NONE;

}

int tscbh_core_init(const phymod_core_access_t* core, const phymod_core_init_config_t* init_config, const phymod_core_status_t* core_status)
{
    if ( (!PHYMOD_CORE_INIT_F_EXECUTE_PASS1_GET(init_config) &&
          !PHYMOD_CORE_INIT_F_EXECUTE_PASS2_GET(init_config)) ||
        PHYMOD_CORE_INIT_F_EXECUTE_PASS1_GET(init_config)) {
        PHYMOD_IF_ERR_RETURN
            (_tscbh_core_init_pass1(core, init_config, core_status));

        if (PHYMOD_CORE_INIT_F_EXECUTE_PASS1_GET(init_config)) {
            return PHYMOD_E_NONE;
        }
    }

    if ( (!PHYMOD_CORE_INIT_F_EXECUTE_PASS1_GET(init_config) &&
          !PHYMOD_CORE_INIT_F_EXECUTE_PASS2_GET(init_config)) ||
        PHYMOD_CORE_INIT_F_EXECUTE_PASS2_GET(init_config)) {
        PHYMOD_IF_ERR_RETURN
            (_tscbh_core_init_pass2(core, init_config, core_status));
    }

    return PHYMOD_E_NONE;
}

int tscbh_phy_init(const phymod_phy_access_t* phy, const phymod_phy_init_config_t* init_config)
{
    /*int pll_restart = 0;*/
    const phymod_access_t *pm_acc = &phy->access;
    phymod_phy_access_t pm_phy_copy;
    int start_lane, num_lane, i;
    phymod_firmware_lane_config_t firmware_lane_config;
    uint32_t rev_num, pll_power_down = 0;
    PHYMOD_MEMCPY(&pm_phy_copy, phy, sizeof(pm_phy_copy));
    PHYMOD_MEMSET(&firmware_lane_config, 0x0, sizeof(firmware_lane_config));

    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(pm_acc, &start_lane, &num_lane));
    /* per lane based reset release */
    PHYMOD_IF_ERR_RETURN
        (tbhmod_pmd_x4_reset(&pm_phy_copy.access));

    /* Put PMD lane into soft reset */
    PHYMOD_IF_ERR_RETURN
        (blackhawk_lane_soft_reset(&pm_phy_copy.access, 1));

    /* next get version number */
    pm_phy_copy.access.lane_mask = 0x1;
    PHYMOD_IF_ERR_RETURN
        (tbhmod_revid_get(&pm_phy_copy.access, &rev_num));

    for (i = 0; i < num_lane; i++) {
        if (!PHYMOD_LANEPBMP_MEMBER(phy->access.lane_mask, start_lane + i)) {
            continue;
        }
        pm_phy_copy.access.lane_mask = 0x1 << (i + start_lane);
        /* set tx parameters */
        PHYMOD_IF_ERR_RETURN
            (tscbh_phy_tx_set(&pm_phy_copy, &init_config->tx[i]));
    }

    /* clearing all the lane config */
    PHYMOD_MEMSET(&firmware_lane_config, 0x0, sizeof(firmware_lane_config));
    for (i = 0; i < num_lane; i++) {
        if (!PHYMOD_LANEPBMP_MEMBER(phy->access.lane_mask, start_lane + i)) {
            continue;
        }
        pm_phy_copy.access.lane_mask = 0x1 << (i + start_lane);
        PHYMOD_IF_ERR_RETURN
             (_tscbh_phy_firmware_lane_config_set(&pm_phy_copy, firmware_lane_config));
        /* FROM PMD team:
         * The best initial analog calibration is achieved using the information
         * from the PMD RX PRBS checker. Using the FW register value of 3 enables
         * a continuous version that does not rely on PRBS traffic anymore.
         * the value of 3 o will apply to A0 part only, and for B0 the value will be 0
        */
        if (rev_num == TBHMOD_REV_NUM_0) {
            PHYMOD_IF_ERR_RETURN
                 (blackhawk_tsc_lane_cfg_fwapi_data1_set(&pm_phy_copy.access, 3));
        } else {
            PHYMOD_IF_ERR_RETURN
                 (blackhawk_tsc_lane_cfg_fwapi_data1_set(&pm_phy_copy.access, 0));
        }
    }

    PHYMOD_MEMCPY(&pm_phy_copy, phy, sizeof(pm_phy_copy));
    PHYMOD_IF_ERR_RETURN
        (tbhmod_rx_lane_control(&pm_phy_copy.access, 1));
    PHYMOD_IF_ERR_RETURN
        (tbhmod_tx_lane_control(&pm_phy_copy.access, 1, 0));         /* TX_LANE_CONTROL */

    /* make sure that power up PLL is locked */
    pm_phy_copy.access.pll_idx = 0;
    pm_phy_copy.access.lane_mask = 1 << 0;
    PHYMOD_IF_ERR_RETURN
        (blackhawk_tsc_pll_pwrdn_get(&pm_phy_copy.access, &pll_power_down));

    /* need to check pll0 lock if not power up */
    /* put the check here is to save on boot up time */
    if (!pll_power_down) {
        uint32_t cnt = 0, pll_lock = 0;
        cnt = 0;
        while (cnt < 500) {
            PHYMOD_IF_ERR_RETURN(blackhawk_tsc_pll_lock_get(&pm_phy_copy.access, &pll_lock));
            cnt = cnt + 1;
            if (pll_lock) {
                break;
            } else {
                if(cnt == 500) {
                    PHYMOD_DEBUG_ERROR(("WARNING :: core 0x%x PLL0 is not locked within 5 milli second \n", pm_phy_copy.access.addr));
                    break;
                }
            }
            PHYMOD_USLEEP(10);
        }
    }

    pm_phy_copy.access.pll_idx = 1;
    pm_phy_copy.access.lane_mask = 1 << 0;
    PHYMOD_IF_ERR_RETURN
        (blackhawk_tsc_pll_pwrdn_get(&pm_phy_copy.access, &pll_power_down));

    /* need to check pll1 lock if not power up */
    if (!pll_power_down) {
        uint32_t cnt = 0, pll_lock = 0;
        cnt = 0;
        while (cnt < 500) {
            PHYMOD_IF_ERR_RETURN(blackhawk_tsc_pll_lock_get(&pm_phy_copy.access, &pll_lock));
            cnt = cnt + 1;
            if (pll_lock) {
                break;
            } else {
                if(cnt == 500) {
                    PHYMOD_DEBUG_ERROR(("WARNING :: core 0x%x PLL1 is not locked within 5 milli second \n", pm_phy_copy.access.addr));
                    break;
                }
            }
            PHYMOD_USLEEP(10);
        }
    }

    return PHYMOD_E_NONE;
}


/* this function gives the PMD_RX_LOCK_STATUS */
int tscbh_phy_link_status_get(const phymod_phy_access_t* phy, uint32_t* link_status)
{
    phymod_phy_access_t pm_phy_copy;
    PHYMOD_MEMCPY(&pm_phy_copy, phy, sizeof(pm_phy_copy));

    PHYMOD_IF_ERR_RETURN(tbhmod_get_pcs_latched_link_status(&pm_phy_copy.access, link_status));
    return PHYMOD_E_NONE;
}


int tscbh_phy_rx_pmd_locked_get(const phymod_phy_access_t* phy, uint32_t* pmd_lock)
{
    int start_lane, num_lane, i;
    phymod_phy_access_t pm_phy_copy;
    uint8_t tmp_lock;

    PHYMOD_MEMCPY(&pm_phy_copy, phy, sizeof(pm_phy_copy));
    *pmd_lock = 1;

    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(&phy->access, &start_lane, &num_lane));

    for (i = 0; i < num_lane; i++) {
        pm_phy_copy.access.lane_mask = 1 << (start_lane + i);
        if (!PHYMOD_LANEPBMP_MEMBER(phy->access.lane_mask, start_lane + i)) {
            continue;
        }
        PHYMOD_IF_ERR_RETURN
            (blackhawk_tsc_pmd_lock_status(&pm_phy_copy.access, &tmp_lock));
        *pmd_lock &= (uint32_t) tmp_lock;
    }
    return PHYMOD_E_NONE;

}

/* this function gives the PMD_RX_LOCK_STATUS */
int tscbh_phy_rx_signal_detect_get(const phymod_phy_access_t* phy, uint32_t* signal_detect)
{
    int start_lane, num_lane, i;
    phymod_phy_access_t pm_phy_copy;
    uint32_t tmp_detect;

    PHYMOD_MEMCPY(&pm_phy_copy, phy, sizeof(pm_phy_copy));
    *signal_detect = 1;

    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(&phy->access, &start_lane, &num_lane));

    for (i = 0; i < num_lane; i++) {
        pm_phy_copy.access.lane_mask = 1 << (start_lane + i);
        if (!PHYMOD_LANEPBMP_MEMBER(phy->access.lane_mask, start_lane + i)) {
            continue;
        }
        PHYMOD_IF_ERR_RETURN
            (blackhawk_tsc_signal_detect(&pm_phy_copy.access, &tmp_detect));
        *signal_detect &= tmp_detect;
    }
    return PHYMOD_E_NONE;

}

int tscbh_phy_reg_read(const phymod_phy_access_t* phy, uint32_t reg_addr, uint32_t* val)
{
    PHYMOD_IF_ERR_RETURN(phymod_tscbh_iblk_read(&phy->access, reg_addr, val));
    return PHYMOD_E_NONE;
}


int tscbh_phy_reg_write(const phymod_phy_access_t* phy, uint32_t reg_addr, uint32_t val)
{
    PHYMOD_IF_ERR_RETURN(phymod_tscbh_iblk_write(&phy->access, reg_addr, val));
    return PHYMOD_E_NONE;
}

/* get default tx taps*/
int tscbh_phy_tx_taps_default_get(const phymod_phy_access_t* phy, phymod_phy_signalling_method_t mode, phymod_tx_t* tx)
{
    /*always default to 6-taps mode */
    tx->tap_mode = phymodTxTapMode6Tap;
    tx->sig_method = mode;
    if (mode == phymodSignallingMethodNRZ) {
        tx->pre2 = 0;
        tx->pre = 0;
        tx->main = 127;
        tx->post = 0;
        tx->post2 = 0;
        tx->post3 = 0;
    } else {
        tx->pre2 = 0;
        tx->pre = -24;
        tx->main = 132;
        tx->post = -12;
        tx->post2 = 0;
        tx->post3 = 0;
    }

    return PHYMOD_E_NONE;
}

/* get default tx taps*/
int tscbh_phy_lane_config_default_get(const phymod_phy_access_t* phy, phymod_phy_signalling_method_t mode, phymod_firmware_lane_config_t* lane_config)
{
    /* default always assume backplane as the medium type and with dfe on */
    if (mode == phymodSignallingMethodNRZ) {
        lane_config->ForceNRZMode = 1;
        lane_config->ForcePAM4Mode = 0;
        lane_config->ForceNormalReach  = 0;
    } else {
        lane_config->ForceNRZMode = 0;
        lane_config->ForcePAM4Mode = 1;
        lane_config->ForceNormalReach  = 1;
    }
    lane_config->LaneConfigFromPCS = 0;
    lane_config->AnEnabled = 0;
    lane_config->DfeOn = 1;
    lane_config->LpDfeOn = 0;
    lane_config->ForceBrDfe = 0;
    lane_config->MediaType = 0;
    lane_config->ScramblingDisable = 0;
    lane_config->Cl72AutoPolEn = 0;
    lane_config->Cl72RestTO    = 0;
    lane_config->ForceExtenedReach = 0;
    lane_config->LpPrecoderEnabled = 0;
    lane_config->UnreliableLos = 0;

    return PHYMOD_E_NONE;
}

int tscbh_phy_pll_multiplier_get(const phymod_phy_access_t* phy, uint32_t* core_vco_pll_multiplier)
{
    phymod_phy_access_t pm_phy_copy;
    PHYMOD_MEMCPY(&pm_phy_copy, phy, sizeof(pm_phy_copy));

    PHYMOD_IF_ERR_RETURN(blackhawk_tsc_INTERNAL_read_pll_div(&pm_phy_copy.access,  core_vco_pll_multiplier));
    return PHYMOD_E_NONE;
}

int tscbh_phy_firmware_load_info_get(const phymod_phy_access_t* phy, phymod_firmware_load_info_t* info)
{
    uint32_t rev_num;
    phymod_phy_access_t phy_copy;

    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));
    phy_copy.access.lane_mask = 0x1;

    PHYMOD_IF_ERR_RETURN
        (tbhmod_revid_get(&(phy_copy.access), &rev_num));
    if (rev_num == TBHMOD_REV_NUM_0) {
        info->ucode_ptr = &blackhawk_ucode_rev0[0];
        info->ucode_len = blackhawk_ucode_len_rev0;
    } else {
        info->ucode_ptr = &blackhawk_ucode_rev1[0];
        info->ucode_len = blackhawk_ucode_len_rev1;
    }
    return PHYMOD_E_NONE;
}

STATIC
int _tscbh_phy_autoneg_ability_to_vco_get(const phymod_autoneg_advert_abilities_t* an_advert_abilities,
                                          uint16_t* request_vco)
{
    phymod_autoneg_advert_ability_t* an_ability;
    uint32_t i;

    an_ability = an_advert_abilities->autoneg_abilities;
    /* We do not validate each abilities here since they are supposed to be validated in portmod. */
    for (i = 0; i < an_advert_abilities->num_abilities; i++) {
        switch (an_ability[i].speed) {
            case 10000:
                /* CL73-10G-1lane */
                *request_vco |= TBHMOD_VCO_20G;
                break;
            case 20000:
                /* CL73BAM-20G-2lanes */
                *request_vco |= TBHMOD_VCO_20G;
                break;
            case 25000:
                /* CL73-1lane; CL73BAM-1lane; MSA-1lane */
                *request_vco |= TBHMOD_VCO_25G;
                break;
            case 40000:
                /* CL73-40G-4lanes */
                /* CL73BAM-40G-2lanes */
                *request_vco |= TBHMOD_VCO_20G;
                break;
            case 50000:
                if (an_ability[i].fec == phymod_fec_RS544) {
                    /* CL73-50G-1lane */
                    /* CL73BAM-50G-2lanes-RS544 */
                    *request_vco |= TBHMOD_VCO_26G;
                } else {
                    /* CL73BAM-50G-1lane*/
                    /* CL73BAM-50G-2lanes-Nofec/CL74/RS528 */
                    /* MSA-50G-2lanes */
                    *request_vco |= TBHMOD_VCO_25G;
                }
                break;
            case 100000:
                if (an_ability[i].fec == phymod_fec_RS544) {
                    /* CL73-100G-2lanes */
                    /* CL73BAM-100G-4lanes-RS544 */
                    *request_vco |= TBHMOD_VCO_26G;
                } else {
                    /* CL73-100G-4lanes */
                    /* CL73BAM-100G-2lanes-RS528 */
                    /* CL73BAM-100G-2lanes-Nofec */
                    /* CL73BAM-100G-4lanes-Nofec */
                    *request_vco |= TBHMOD_VCO_25G;
                }
                break;
            case 200000:
                if ((an_ability[i].fec == phymod_fec_RS544)
                    || (an_ability[i].fec == phymod_fec_RS544_2XN)) {
                    /* CL73-200G-4lanes-RS544_2XN */
                    /* CL73BAM-200G-4lanes-RS544_1XN */
                    *request_vco |= TBHMOD_VCO_26G;
                } else {
                    /* CL73BAM-200G-4lanes-Nofec */
                    *request_vco |= TBHMOD_VCO_25G;
                }
                break;
            default:
                break;
        }

    }

    return PHYMOD_E_NONE;
}

int tscbh_phy_autoneg_advert_ability_set(const phymod_phy_access_t* phy,
                                         const phymod_autoneg_advert_abilities_t* an_advert_abilities,
                                         const phymod_phy_pll_state_t* old_pll_adv_state,
                                         phymod_phy_pll_state_t* new_pll_adv_state)
{
    uint16_t request_vco = 0;
    int start_lane, num_lane, i = 0, requested_pll_num;
    phymod_phy_access_t phy_copy;
    tbhmod_refclk_t ref_clk;
    uint32_t request_pll_div[3], ovco_is_pwrdn = 0;
    uint32_t  ovco_pll_div = 0, tvco_pll_div = 0, pll_index = 0;
    uint8_t ovco_pll_index;
    uint32_t cnt, pll_lock = 0;

    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(&phy->access, &start_lane, &num_lane));

    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));
    phy_copy.access.lane_mask = 0x1 << start_lane;

    ovco_pll_index = phy->access.tvco_pll_index ? 0 : 1;

    /* Program local advert abilitiy registers */
    PHYMOD_IF_ERR_RETURN
        (tbhmod_autoneg_ability_set(&phy_copy.access, an_advert_abilities));

    PHYMOD_IF_ERR_RETURN
        (_tscbh_phy_autoneg_ability_to_vco_get(an_advert_abilities, &request_vco));

    PHYMOD_IF_ERR_RETURN
        (tbhmod_refclk_get(&phy_copy.access, &ref_clk));
    /* Check the request speed VCO */
    if (request_vco & TBHMOD_VCO_20G) {
        PHYMOD_IF_ERR_RETURN
            (tbhmod_vco_to_pll_lkup(TBHMOD_VCO_20G, ref_clk, &request_pll_div[i]));
        i++;
    }
    if (request_vco & TBHMOD_VCO_25G) {
        PHYMOD_IF_ERR_RETURN
            (tbhmod_vco_to_pll_lkup(TBHMOD_VCO_25G, ref_clk, &request_pll_div[i]));
        i++;
    }
    if (request_vco & TBHMOD_VCO_26G) {
        PHYMOD_IF_ERR_RETURN
            (tbhmod_vco_to_pll_lkup(TBHMOD_VCO_26G, ref_clk, &request_pll_div[i]));
        i++;
    }
    if (i > 2) {
        /* More than 2 vcos are needed */
        return PHYMOD_E_PARAM;
    }
    requested_pll_num = i - 1;

    /* Get OVCO info */
    phy_copy.access.pll_idx = ovco_pll_index;
    PHYMOD_IF_ERR_RETURN
        (blackhawk_tsc_pll_pwrdn_get(&phy_copy.access, &ovco_is_pwrdn));
    if (!ovco_is_pwrdn) {
        PHYMOD_IF_ERR_RETURN
            (blackhawk_tsc_INTERNAL_read_pll_div(&phy_copy.access, &ovco_pll_div));
    }

    /* Get TVCO info */
    phy_copy.access.pll_idx = phy->access.tvco_pll_index;
    PHYMOD_IF_ERR_RETURN
        (blackhawk_tsc_INTERNAL_read_pll_div(&phy_copy.access, &tvco_pll_div));

    /* Copy existing PLL lane bitmap */
    *new_pll_adv_state = *old_pll_adv_state;

    /* Clear current lane_map from both PLL lane bitmap */
    new_pll_adv_state->pll1_lanes_bitmap &= ~(phy->access.lane_mask);
    new_pll_adv_state->pll0_lanes_bitmap &= ~(phy->access.lane_mask);

    for (i = requested_pll_num; i >= 0; i--) {
        if (request_pll_div[i] == ovco_pll_div) {
            /* if ovco is using PLL 0 */
            if (!ovco_pll_index) {
                new_pll_adv_state->pll0_lanes_bitmap |= phy->access.lane_mask;
            } else {
                new_pll_adv_state->pll1_lanes_bitmap |= phy->access.lane_mask;
            }
        } else if (request_pll_div[i] == tvco_pll_div) {
            /* if ovco is using PLL 0 */
            if (!ovco_pll_index) {
                new_pll_adv_state->pll1_lanes_bitmap |= phy->access.lane_mask;
            } else {
                new_pll_adv_state->pll0_lanes_bitmap |= phy->access.lane_mask;
            }
        } else {
            /* Need VCO change */
            pll_index = ovco_pll_index;
            phy_copy.access.pll_idx = pll_index;

            if (ovco_is_pwrdn) {
                /* Power up PLL0 */
                PHYMOD_IF_ERR_RETURN
                    (blackhawk_tsc_core_pwrdn(&phy_copy.access, PWR_ON));
            }
            /*toggle core dp reset */
            phy_copy.access.lane_mask = 0x1;
            PHYMOD_IF_ERR_RETURN
                (blackhawk_tsc_core_dp_reset(&phy_copy.access, 1));

            /*config the PLL to the requested VCO */
            if (ref_clk == TBHMOD_REF_CLK_312P5MHZ) {
                PHYMOD_IF_ERR_RETURN
                    (blackhawk_tsc_configure_pll_refclk_div(&phy_copy.access,
                                                            BLACKHAWK_TSC_PLL_REFCLK_312P5MHZ,
                                                            request_pll_div[i]));
            } else {
                PHYMOD_IF_ERR_RETURN
                    (blackhawk_tsc_configure_pll_refclk_div(&phy_copy.access,
                                                            BLACKHAWK_TSC_PLL_REFCLK_156P25MHZ,
                                                            request_pll_div[i]));
            }
            /* release core soft reset */
            PHYMOD_IF_ERR_RETURN
                (blackhawk_tsc_core_dp_reset(&phy_copy.access, 0));
            ovco_pll_div = request_pll_div[i];
            if (!ovco_pll_index) {
                new_pll_adv_state->pll0_lanes_bitmap |= phy->access.lane_mask;
            } else {
                new_pll_adv_state->pll1_lanes_bitmap |= phy->access.lane_mask;
            }
            /* need to wait for the PLL lock */
            cnt = 0;
            while (cnt < 500) {
                PHYMOD_IF_ERR_RETURN(blackhawk_tsc_pll_lock_get(&phy_copy.access, &pll_lock));
                cnt = cnt + 1;
                if(pll_lock) {
                    break;
                } else {
                    if(cnt == 500) {
                        PHYMOD_DEBUG_ERROR(("WARNING :: core 0x%x PLL Index %d is not locked within 5 milli second \n", phy_copy.access.addr, pll_index));
                        break;
                    }
                }
                PHYMOD_USLEEP(10);
            }

        }
    }

    return PHYMOD_E_NONE;
}

int tscbh_phy_autoneg_advert_ability_get(const phymod_phy_access_t* phy,
                                         phymod_autoneg_advert_abilities_t* an_advert_abilities)
{
    phymod_phy_access_t phy_copy;
    int start_lane, num_lane;
    uint32_t i;
    phymod_firmware_lane_config_t firmware_lane_config;
    uint32_t fec_override = 0;

    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(&phy->access, &start_lane, &num_lane));
    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));
    phy_copy.access.lane_mask = 0x1 << start_lane;

    PHYMOD_IF_ERR_RETURN
        (tbhmod_autoneg_ability_get(&phy_copy.access, an_advert_abilities));

    /* Get Medium type from fw_lane_config */
    PHYMOD_IF_ERR_RETURN
        (tscbh_phy_firmware_lane_config_get(phy, &firmware_lane_config));

    for (i = 0; i < an_advert_abilities->num_abilities; i++) {
        an_advert_abilities->autoneg_abilities[i].medium = firmware_lane_config.MediaType;
    }
    /* check if FEC override bit is set  */
    PHYMOD_IF_ERR_RETURN
        (tscbh_phy_fec_override_get(phy, &fec_override));
    if (fec_override == 1) {
        for (i = 0; i < an_advert_abilities->num_abilities; i++) {
            if ((an_advert_abilities->autoneg_abilities[i].speed == 100000) &&
                (an_advert_abilities->autoneg_abilities[i].resolved_num_lanes == 4) &&
                (an_advert_abilities->autoneg_abilities[i].fec == phymod_fec_CL91) &&
                (an_advert_abilities->autoneg_abilities[i].an_mode == phymod_AN_MODE_CL73)) {
                an_advert_abilities->autoneg_abilities[i].fec = phymod_fec_None;
            }
        }
    }

    return PHYMOD_E_NONE;
}

int tscbh_phy_autoneg_remote_advert_ability_get(const phymod_phy_access_t* phy,
                                                phymod_autoneg_advert_abilities_t* an_advert_abilities)
{
    phymod_phy_access_t phy_copy;
    int start_lane, num_lane, is_copper = 0;
    uint32_t i, fec_override = 0;

    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(&phy->access, &start_lane, &num_lane));
    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));
    phy_copy.access.lane_mask = 0x1 << start_lane;

    PHYMOD_IF_ERR_RETURN
        (tbhmod_autoneg_remote_ability_get(&phy_copy.access, an_advert_abilities));

    for (i = 0; i < an_advert_abilities->num_abilities; i++) {
        if (an_advert_abilities->autoneg_abilities[i].medium == phymodFirmwareMediaTypeCopperCable) {
            is_copper = 1;
            break;
        }
    }

    for (i = 0; i < an_advert_abilities->num_abilities; i++) {
        if (is_copper) {
            an_advert_abilities->autoneg_abilities[i].medium = phymodFirmwareMediaTypeCopperCable;
        } else {
            an_advert_abilities->autoneg_abilities[i].medium = phymodFirmwareMediaTypePcbTraceBackPlane;
        }
    }

    /* check if FEC override bit is set  */
    PHYMOD_IF_ERR_RETURN
        (tscbh_phy_fec_override_get(phy, &fec_override));
    if (fec_override == 1) {
        for (i = 0; i < an_advert_abilities->num_abilities; i++) {
            if ((an_advert_abilities->autoneg_abilities[i].speed == 100000) &&
                (an_advert_abilities->autoneg_abilities[i].resolved_num_lanes == 4) &&
                (an_advert_abilities->autoneg_abilities[i].fec == phymod_fec_CL91) &&
                (an_advert_abilities->autoneg_abilities[i].an_mode == phymod_AN_MODE_CL73)) {
                an_advert_abilities->autoneg_abilities[i].fec = phymod_fec_None;
            }
        }
    }

    return PHYMOD_E_NONE;
}

int tscbh_phy_autoneg_set(const phymod_phy_access_t* phy, const phymod_autoneg_control_t* an)
{
    int num_lane_adv_encoded, mapped_speed_id;
    int start_lane, num_lane;
    int i, do_lane_config_set;
    uint32_t pll_1_div, vco_rate, refclk_in_hz;
    phymod_firmware_lane_config_t firmware_lane_config;
    tbhmod_an_control_t an_control;
    phymod_phy_access_t phy_copy;
    tbhmod_refclk_t ref_clk;
    tbhmod_spd_intfc_type_t spd_intf = 0;

    PHYMOD_MEMSET(&firmware_lane_config, 0x0, sizeof(firmware_lane_config));

    PHYMOD_MEMSET(&an_control, 0x0, sizeof(an_control));
    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(&phy->access, &start_lane, &num_lane));

    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));
    phy_copy.access.lane_mask = 0x1 << start_lane;

    if (an->enable) {
        PHYMOD_IF_ERR_RETURN(tbhmod_disable_set(&phy_copy.access));
        /* adjust FEC arch for RS544 */
        if (PHYMOD_AN_F_FEC_RS272_CLR_GET(an)) {
            PHYMOD_IF_ERR_RETURN(tbhmod_rsfec_cw_type(&phy_copy.access, 0, 0));
        }

        /*next choose TVCO as the PLL selelction for all the lanes*/
        for (i = 0; i < num_lane; i++) {
            phy_copy.access.lane_mask = 1 << (start_lane + i);
            PHYMOD_IF_ERR_RETURN
                (blackhawk_lane_pll_selection_set(&phy_copy.access, phy->access.tvco_pll_index));
        }
    }

    switch (an->num_lane_adv) {
        case 1:
            num_lane_adv_encoded = 0;
            break;
        case 2:
            num_lane_adv_encoded = 1;
            break;
        case 4:
            num_lane_adv_encoded = 2;
            break;
        case 8:
            num_lane_adv_encoded = 3;
            break;
        default:
            return PHYMOD_E_PARAM;
    }

    an_control.num_lane_adv = num_lane_adv_encoded;
    an_control.enable       = an->enable;
    switch (an->an_mode) {
        case phymod_AN_MODE_CL73:
            an_control.an_type = TBHMOD_AN_MODE_CL73;
            break;
        case phymod_AN_MODE_CL73BAM:
            an_control.an_type = TBHMOD_AN_MODE_CL73_BAM;
            break;
        case phymod_AN_MODE_CL73_MSA:
            an_control.an_type = TBHMOD_AN_MODE_CL73_MSA;
            break;
        default:
            return PHYMOD_E_PARAM;
            break;
    }

    /* SW WAR for 400G AN */
    /* if AN is enabled, first needs to disable timer */
    if (num_lane == 8) {
        if (an->enable) {
            PHYMOD_IF_ERR_RETURN
                (tbhmod_400g_autoneg_timer_disable(&phy_copy.access, 1));
        } else {
            PHYMOD_IF_ERR_RETURN
                (tbhmod_400g_autoneg_timer_disable(&phy_copy.access, 0));
        }
    }

    if (an->enable) {
        /* Set AN port mode */
        PHYMOD_IF_ERR_RETURN
            (tbhmod_set_an_port_mode(&phy_copy.access, start_lane));

        /* Get TVCO rate (PLL1 for now) */
        PHYMOD_IF_ERR_RETURN
            (tbhmod_refclk_get(&phy_copy.access, &ref_clk));

        if (ref_clk == TBHMOD_REF_CLK_312P5MHZ) {
            refclk_in_hz = 312500000;
        } else {
            refclk_in_hz = 156250000;
        }

        /* next read current TVCO pll divider*/
        phy_copy.access.pll_idx = phy->access.tvco_pll_index;
        PHYMOD_IF_ERR_RETURN
            (blackhawk_tsc_INTERNAL_read_pll_div(&phy_copy.access, &pll_1_div));

        PHYMOD_IF_ERR_RETURN
            (blackhawk_tsc_get_vco_from_refclk_div(&phy_copy.access, refclk_in_hz, pll_1_div, &vco_rate, 0));

        if (vco_rate == 20625000) {
            /* load 20G VCO spd_id */
            spd_intf = TBHMOD_SPD_CL73_20G;
        } else if (vco_rate == 25781250) {
            /* load 25G VCO spd_id */
            spd_intf = TBHMOD_SPD_CL73_25G;
        } else if (vco_rate == 26562500) {
            /* load 26G VCO spd_id */
            spd_intf = TBHMOD_SPD_CL73_26G;
        } else {
            return PHYMOD_E_PARAM;
        }

        PHYMOD_IF_ERR_RETURN
            (tbhmod_get_mapped_speed(spd_intf, &mapped_speed_id));

        phy_copy.access.lane_mask = 0x1 << start_lane;

        /* Load 1G speed ID */
        PHYMOD_IF_ERR_RETURN
            (tbhmod_set_sc_speed(&phy_copy.access, mapped_speed_id, 0));
    }

    do_lane_config_set = 0;
    if (an->enable) {
        /* make sure the firmware config is set to an enabled */
        PHYMOD_IF_ERR_RETURN
            (tscbh_phy_firmware_lane_config_get(&phy_copy, &firmware_lane_config));
        /* make sure the firmware config is set to an enabled */
        if (firmware_lane_config.AnEnabled != 1) {
          firmware_lane_config.AnEnabled = 1;
          do_lane_config_set = 1;
        }
        if (firmware_lane_config.LaneConfigFromPCS != 1) {
          firmware_lane_config.LaneConfigFromPCS = 1;
          do_lane_config_set = 1;
        }
        firmware_lane_config.Cl72RestTO = 0;
        firmware_lane_config.ForceNormalReach = 0;
        firmware_lane_config.ForceExtenedReach = 0;
        firmware_lane_config.ForceNRZMode= 0;
        firmware_lane_config.ForcePAM4Mode = 0;
    }

    if (do_lane_config_set) {
        for (i = 0; i < num_lane; i++) {
            if (!PHYMOD_LANEPBMP_MEMBER(phy->access.lane_mask, start_lane + i)) {
                continue;
            }
            phy_copy.access.lane_mask = 0x1 << (i + start_lane);
            PHYMOD_IF_ERR_RETURN
                (blackhawk_lane_soft_reset(&phy_copy.access, 1));
        }
        PHYMOD_USLEEP(1000);
        for (i = 0; i < num_lane; i++) {
            if (!PHYMOD_LANEPBMP_MEMBER(phy->access.lane_mask, start_lane + i)) {
                continue;
            }
            phy_copy.access.lane_mask = 0x1 << (i + start_lane);
            PHYMOD_IF_ERR_RETURN
                (_tscbh_phy_firmware_lane_config_set(&phy_copy, firmware_lane_config));
        }
        for (i = 0; i < num_lane; i++) {
            if (!PHYMOD_LANEPBMP_MEMBER(phy->access.lane_mask, start_lane + i)) {
                continue;
            }
            phy_copy.access.lane_mask = 0x1 << (i + start_lane);
            PHYMOD_IF_ERR_RETURN
                (blackhawk_lane_soft_reset(&phy_copy.access, 0));
        }
    }

    phy_copy.access.lane_mask = 0x1 << start_lane;

    PHYMOD_IF_ERR_RETURN
        (tbhmod_autoneg_control(&phy_copy.access, &an_control));
/* for now comment out, need to revisit */
#if 0
    if (!an->enable) {
        tbhmod_enable_set(&phy_copy.access);
    }
#endif

    return PHYMOD_E_NONE;
}

int tscbh_phy_autoneg_get(const phymod_phy_access_t* phy, phymod_autoneg_control_t* an, uint32_t* an_done)
{
    tbhmod_an_control_t an_control;
    phymod_phy_access_t phy_copy;
    int start_lane, num_lane;
    int an_complete = 0;

    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(&phy->access, &start_lane, &num_lane));

    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));
    phy_copy.access.lane_mask = 0x1 << start_lane;

    PHYMOD_MEMSET(&an_control, 0x0,  sizeof(tbhmod_an_control_t));
    PHYMOD_IF_ERR_RETURN
        (tbhmod_autoneg_control_get(&phy_copy.access, &an_control, &an_complete));

    if (an_control.enable) {
        an->enable = 1;
        *an_done = an_complete;
    } else {
        an->enable = 0;
    }

    switch (an_control.num_lane_adv) {
        case 0:
            an->num_lane_adv = 1;
            break;
        case 1:
            an->num_lane_adv = 2;
            break;
        case 2:
            an->num_lane_adv = 4;
            break;
        case 3:
            an->num_lane_adv = 8;
            break;
        default:
            an->num_lane_adv = 0;
            break;
    }

    switch (an_control.an_type) {
        case TBHMOD_AN_MODE_CL73:
            an->an_mode = phymod_AN_MODE_CL73;
            break;
        case TBHMOD_AN_MODE_CL73_BAM:
            an->an_mode = phymod_AN_MODE_CL73BAM;
            break;
        case TBHMOD_AN_MODE_MSA:
            an->an_mode = phymod_AN_MODE_MSA;
            break;
        case TBHMOD_AN_MODE_CL73_MSA:
            an->an_mode = phymod_AN_MODE_CL73_MSA;
            break;
        default:
            an->an_mode = phymod_AN_MODE_NONE;
            break;
    }

    return PHYMOD_E_NONE;
}

int tscbh_phy_autoneg_status_get(const phymod_phy_access_t* phy, phymod_autoneg_status_t* status)
{
    int an_en, an_done;
    phymod_phy_speed_config_t speed_config;
    phymod_phy_access_t phy_copy;
    int start_lane, num_lane, speed_id;
    uint32_t packed_entry[5];
    spd_id_tbl_entry_t speed_config_entry;

    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(&phy->access, &start_lane, &num_lane));

    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));
    phy_copy.access.lane_mask = 0x1 << start_lane;

    PHYMOD_IF_ERR_RETURN
       (tbhmod_autoneg_status_get(&phy_copy.access, &an_en, &an_done));

    PHYMOD_IF_ERR_RETURN
       (tscbh_phy_speed_config_get(phy, &speed_config));

    if (an_en && an_done) {
        uint32_t an_resolved_mode;
        /* if an resolves and link up */
        PHYMOD_IF_ERR_RETURN
            (tbhmod_speed_id_get(&phy_copy.access, &speed_id));
        /*read the speed id entry and get the num_lane info */
        phy_copy.access.lane_mask = 1 << start_lane;
        PHYMOD_IF_ERR_RETURN
            (phymod_mem_read(&phy_copy.access, phymodMemSpeedIdTable, speed_id, packed_entry));
        spd_ctrl_unpack_spd_id_tbl_entry(packed_entry, &speed_config_entry);
        num_lane = 1 << speed_config_entry.num_lanes;
        /* read the AN final resolved port mode */
        PHYMOD_IF_ERR_RETURN
            (tbhmod_resolved_port_mode_get(&phy_copy.access, &an_resolved_mode));
        status->resolved_port_mode = an_resolved_mode;
    }

    status->enabled   = an_en;
    status->locked    = an_done;
    status->data_rate = speed_config.data_rate;
    status->resolved_num_lane = num_lane;

    return PHYMOD_E_NONE;
}

int tscbh_phy_pll_reconfig(const phymod_phy_access_t* phy,
                           uint8_t pll_index,
                           uint32_t pll_div,
                           phymod_ref_clk_t ref_clk1)
{
    phymod_phy_access_t pm_phy_copy;
    tbhmod_refclk_t ref_clk;
    /*phymod_tscbh_pll_multiplier_t pll_0, pll_1; */
    uint32_t tvco_rate = 0, speed_id_load_size, i, pll_is_pwrdn;
    uint8_t tvco_pll_index, tvco_reconfig = 0;
    enum blackhawk_tsc_pll_refclk_enum refclk;
    uint32_t cnt = 0, pll_lock = 0;

    PHYMOD_MEMCPY(&pm_phy_copy, phy, sizeof(pm_phy_copy));
    tvco_pll_index = phy->access.tvco_pll_index;

    if (pll_index > 1) {
        PHYMOD_DEBUG_ERROR(("Unsupported PLL index\n"));
        return PHYMOD_E_UNAVAIL;
    }

    tvco_reconfig = (tvco_pll_index == pll_index) ? 1 : 0;

    pm_phy_copy.access.lane_mask = 1 << 0;
    /* first needs to read the ref clock from main reg*/
    PHYMOD_IF_ERR_RETURN
        (tbhmod_refclk_get(&pm_phy_copy.access, &ref_clk));

    if (ref_clk == TBHMOD_REF_CLK_156P25MHZ) {
        refclk = BLACKHAWK_TSC_PLL_REFCLK_156P25MHZ;
    } else if (ref_clk == TBHMOD_REF_CLK_312P5MHZ) {
        refclk = BLACKHAWK_TSC_PLL_REFCLK_312P5MHZ;
    } else {
        PHYMOD_DEBUG_ERROR(("Unsupported reference clock.\n"));
        return PHYMOD_E_UNAVAIL;
    }

    if (tvco_reconfig) {
        PHYMOD_IF_ERR_RETURN
            (tbhmod_pll_to_vco_get(ref_clk, pll_div, &tvco_rate));
    }

    /* next disable pcs datapath only if TVCO re-config*/
    if (tvco_reconfig) {
        PHYMOD_MEMCPY(&pm_phy_copy, phy, sizeof(pm_phy_copy));
        PHYMOD_IF_ERR_RETURN
            (tbhmod_disable_set(&pm_phy_copy.access));
        /* add the pcs disable SW WAR */
        PHYMOD_IF_ERR_RETURN
            (tbhmod_pcs_reset_sw_war(&phy->access));
    }

    /*only need to load speed id table for tvco re-config */
    /*then reload the speed id table based on the new tvco */
    if (tvco_reconfig) {
        speed_id_load_size = TSCBH_SPEED_ID_TABLE_SIZE > TSCBH_HW_SPEED_ID_TABLE_SIZE ? TSCBH_HW_SPEED_ID_TABLE_SIZE : TSCBH_SPEED_ID_TABLE_SIZE;
        if (tvco_rate == TBHMOD_VCO_26G) {
            for (i = 0; i < speed_id_load_size; i++) {
                PHYMOD_IF_ERR_RETURN
                    (phymod_mem_write(&pm_phy_copy.access, phymodMemSpeedIdTable, i, &spd_id_entry_26[i][0]));
            }
        } else if (tvco_rate == TBHMOD_VCO_25G) {
            for (i = 0; i < speed_id_load_size; i++) {
                PHYMOD_IF_ERR_RETURN
                    (phymod_mem_write(&pm_phy_copy.access, phymodMemSpeedIdTable, i, &spd_id_entry_25[i][0]));
            }
        } else if (tvco_rate == TBHMOD_VCO_20G) {
            for (i = 0; i < speed_id_load_size; i++) {
                PHYMOD_IF_ERR_RETURN
                    (phymod_mem_write(&pm_phy_copy.access, phymodMemSpeedIdTable, i, &spd_id_entry_20[i][0]));
            }
        }
    }

    /*next check if the PLL is power down or not */
    pm_phy_copy.access.pll_idx = pll_index;
    pm_phy_copy.access.lane_mask = 1 << 0;
    PHYMOD_IF_ERR_RETURN
        (blackhawk_tsc_pll_pwrdn_get(&pm_phy_copy.access, &pll_is_pwrdn));

    /* if PLL is power down, need to power up first */
    if (pll_is_pwrdn) {
        /* Power up ovco if it's power down */
        PHYMOD_IF_ERR_RETURN
            (blackhawk_tsc_core_pwrdn(&pm_phy_copy.access, PWR_ON));
    }

    PHYMOD_IF_ERR_RETURN
        (blackhawk_tsc_core_dp_reset(&pm_phy_copy.access, 1));

    /*next re-config pll divider */
    PHYMOD_IF_ERR_RETURN
        (blackhawk_tsc_configure_pll_refclk_div(&pm_phy_copy.access,
                                                  refclk,
                                                  pll_div));

    PHYMOD_IF_ERR_RETURN
        (blackhawk_tsc_core_dp_reset(&pm_phy_copy.access, 0));

    /* need to wait for the PLL lock */
    cnt = 0;
    while (cnt < 500) {
        PHYMOD_IF_ERR_RETURN(blackhawk_tsc_pll_lock_get(&pm_phy_copy.access, &pll_lock));
        cnt = cnt + 1;
        if(pll_lock) {
            break;
        } else {
            if(cnt == 500) {
                PHYMOD_DEBUG_ERROR(("WARNING :: core 0x%x PLL Index %d is not locked within 5 milli second \n", pm_phy_copy.access.addr, pll_index));
                break;
            }
        }
        PHYMOD_USLEEP(10);
    }

    return PHYMOD_E_NONE;
}

int tscbh_phy_tx_pam4_precoder_enable_set(const phymod_phy_access_t* phy, int enable)
{
    phymod_phy_access_t phy_copy;
    int start_lane, num_lane, i;
    uint32_t lane_reset, pcs_lane_enable;

    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(&phy->access, &start_lane, &num_lane));
    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));

    /*first check if lane is in reset */
    PHYMOD_IF_ERR_RETURN
        (blackhawk_lane_soft_reset_get(&phy_copy.access, &lane_reset));

    /*next check if PCS lane is in reset */
    PHYMOD_IF_ERR_RETURN
        (tbhmod_enable_get(&phy_copy.access, &pcs_lane_enable));

    /* disable pcs lane if pcs lane not in rset */
    if (pcs_lane_enable) {
        phy_copy.access.lane_mask = 1 << start_lane;
        PHYMOD_IF_ERR_RETURN
            (tbhmod_disable_set(&phy_copy.access));

        /* add the pcs disable SW WAR */
        PHYMOD_IF_ERR_RETURN
            (tbhmod_pcs_reset_sw_war(&phy->access));
    }

    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));
    /* if lane is not in reset, then reset the lane first */
    if (!lane_reset) {
        PHYMOD_IF_ERR_RETURN
            (blackhawk_lane_soft_reset(&phy_copy.access, 1));
    }

    for (i = 0; i < num_lane; i++) {
        if (!PHYMOD_LANEPBMP_MEMBER(phy->access.lane_mask, start_lane + i)) {
            continue;
        }
        phy_copy.access.lane_mask = 1 << (start_lane + i);
        PHYMOD_IF_ERR_RETURN
            (blackhawk_tsc_tx_pam4_precoder_enable_set(&phy_copy.access, enable));
    }

    /* release the ln dp reset */
    if (!lane_reset) {
        PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));
        PHYMOD_IF_ERR_RETURN
            (blackhawk_lane_soft_reset(&phy_copy.access, 0));
    }

    /* re-enable pcs lane if pcs lane not in rset */
    if (pcs_lane_enable) {
        phy_copy.access.lane_mask = 1 << start_lane;
        PHYMOD_IF_ERR_RETURN
            (tbhmod_enable_set(&phy_copy.access));
    }

    return PHYMOD_E_NONE;
}

int tscbh_phy_tx_pam4_precoder_enable_get(const phymod_phy_access_t* phy, int *enable)
{
    phymod_phy_access_t pm_phy_copy;

    PHYMOD_MEMCPY(&pm_phy_copy, phy, sizeof(pm_phy_copy));
    PHYMOD_IF_ERR_RETURN
        (blackhawk_tsc_tx_pam4_precoder_enable_get(&pm_phy_copy.access, enable));
    return PHYMOD_E_NONE;
}

/*Set/Get timesync enable*/
int tscbh_timesync_enable_set(const phymod_phy_access_t* phy, uint32_t flags, uint32_t enable)
{
    phymod_phy_access_t phy_copy;
    int start_lane, num_lane, is_sfd = 0;
    uint32_t pcs_lane_enable;
    uint32_t fclk_div_mode = 0;

    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));
    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(&phy->access, &start_lane, &num_lane));

    phy_copy.access.lane_mask = 0x1 << start_lane;

    /* RX timestamping control */
    if (PHYMOD_TIMESYNC_ENABLE_F_RX_GET(flags)) {
        PHYMOD_IF_ERR_RETURN
            (tbhmod_pcs_rx_ts_en(&phy_copy.access, enable, is_sfd));
    }

    /* Core related control:
     *     1. Enable fclk on PMD, with default div_mode.
     */
    if (PHYMOD_TIMESYNC_ENABLE_F_CORE_GET(flags)) {
        phy_copy.access.pll_idx = phy_copy.access.tvco_pll_index;
        PHYMOD_IF_ERR_RETURN
            (blackhawk_tsc_clk4sync_enable_set(&phy_copy.access, enable, fclk_div_mode));
    }

    /* One-Step Timestamp Pipeline */
    if (PHYMOD_TIMESYNC_ENABLE_F_ONE_STEP_PIPELINE_GET(flags)){
        /* check if PCS lane is in reset */
        PHYMOD_IF_ERR_RETURN
            (tbhmod_enable_get(&phy_copy.access, &pcs_lane_enable));

        /* disable pcs lane if pcs lane not in rset */
        if (pcs_lane_enable) {
            PHYMOD_IF_ERR_RETURN
                (tbhmod_disable_set(&phy_copy.access));

            /* add the pcs disable SW WAR */
            PHYMOD_IF_ERR_RETURN
                (tbhmod_pcs_reset_sw_war(&phy->access));
        }

        PHYMOD_IF_ERR_RETURN
            (tbhmod_osts_pipeline(&phy_copy.access, enable));

        /* re-enable pcs lane if pcs lane not in rset */
        if (pcs_lane_enable) {
            PHYMOD_IF_ERR_RETURN
                (tbhmod_enable_set(&phy_copy.access));
        }
    }

    return PHYMOD_E_NONE;
}

/* Only one flag can be served each time.
 */
int tscbh_timesync_enable_get(const phymod_phy_access_t* phy, uint32_t flags, uint32_t* enable)
{
    phymod_phy_access_t phy_copy;
    uint32_t fclk_div_mode;

    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));

    if (PHYMOD_TIMESYNC_ENABLE_F_RX_GET(flags)) {
        PHYMOD_IF_ERR_RETURN
            (tbhmod_pcs_rx_ts_en_get(&phy_copy.access, enable));
    }

    if (PHYMOD_TIMESYNC_ENABLE_F_ONE_STEP_PIPELINE_GET(flags)){
        PHYMOD_IF_ERR_RETURN
            (tbhmod_osts_pipeline_get(&phy_copy.access, enable));
    }

    if (PHYMOD_TIMESYNC_ENABLE_F_CORE_GET(flags)) {
        phy_copy.access.pll_idx = phy_copy.access.tvco_pll_index;
        PHYMOD_IF_ERR_RETURN
            (blackhawk_tsc_clk4sync_enable_get(&phy_copy.access, enable, &fclk_div_mode));
    }

    return PHYMOD_E_NONE;
}

/* Set timesync offset */
int tscbh_timesync_offset_set(const phymod_core_access_t* core, uint32_t ts_offset)
{
    uint32_t sub_ns_offset=0;
    phymod_core_access_t core_copy;
    PHYMOD_MEMCPY(&core_copy, core, sizeof(core_copy));

    PHYMOD_IF_ERR_RETURN
        (tbhmod_pcs_1588_ts_offset_set(&core_copy.access, ts_offset, sub_ns_offset));

    return PHYMOD_E_NONE;
}

/*Set timesync adjust*/
int tscbh_timesync_adjust_set(const phymod_phy_access_t* phy, uint32_t flags, uint32_t ts_am_norm_mode)
{
    int start_lane, num_lane, i, is_pam4, is_valid = 0;
    uint8_t fec_arch;
    int speed_id, mapped_speed_id;
    int an_en, an_done;
    phymod_phy_access_t phy_copy;
    spd_id_tbl_entry_t speed_config_entry;
    uint32_t packed_entry[5];
    phymod_phy_speed_config_t speed_config;
    phymod_firmware_lane_config_t firmware_lane_config;
    tbhmod_spd_intfc_type_t spd_intf = 0;
    phymod_mem_type_t tx_mem,rx_mem;
    int ts_table_index;
    int osr_mode;
    ts_table_entry *ts_tx_entry = NULL, *ts_rx_entry = NULL;
    ts_table_entry ts_update_table;
    uint32_t psll_entry[TBHMOD_TS_TABLE_SIZE * TBHMOD_TS_PSLL_BASED_ENTRY_SIZE];
    uint32_t psll_entry_size, ts_table_size;
    uint32_t pll_div, pll_index;
    tbhmod_refclk_t ref_clk;
    uint32_t tvco, current_vco;
    int mem_offset;
    phymod_interrupt_type_t intr_type = phymodIntrNone;
    tbhmod_intr_status_t intr_status;

    if (ts_am_norm_mode == 0x2) {
        PHYMOD_DEBUG_ERROR(("TSCBH does not supported Latestlane Mode.\n"));
        return PHYMOD_E_UNAVAIL;
    }

    /* Here starts the sequence to enable Timestamping based on the current speed config */
    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));
    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(&phy->access, &start_lane, &num_lane));
    PHYMOD_IF_ERR_RETURN
       (tbhmod_autoneg_status_get(&phy_copy.access, &an_en, &an_done));
    PHYMOD_IF_ERR_RETURN
       (tscbh_phy_firmware_lane_config_get(phy, &firmware_lane_config));

    is_pam4 = firmware_lane_config.ForcePAM4Mode;

    if (start_lane < 4) {
        /* MPP0 */
        mem_offset = start_lane * TBHMOD_TS_DEFAULT_TABLE_SIZE;
    } else {
        /* MPP1 */
        mem_offset = (start_lane - 4) * TBHMOD_TS_DEFAULT_TABLE_SIZE;
    }

    /* 1. Find default 1588 Table */

    /* 1.1 Get current speed id */
    PHYMOD_IF_ERR_RETURN
        (tbhmod_speed_id_get(&phy_copy.access, &speed_id));

    PHYMOD_IF_ERR_RETURN
        (phymod_mem_read(&phy_copy.access, phymodMemSpeedIdTable, speed_id, packed_entry));
    spd_ctrl_unpack_spd_id_tbl_entry(packed_entry, &speed_config_entry);
    /* 1.2 Update num_lane and lane_mask for AN port */
    if (an_en && an_done) {
        num_lane = 1 << speed_config_entry.num_lanes;
        /* Update lane_mask */
        phy_copy.access.lane_mask = 0x0;
        for (i = 0; i < num_lane; i++) {
            phy_copy.access.lane_mask |= 0x1 << (i + start_lane);
        }
    }
    /* 1.3 Get FEC type */
    PHYMOD_IF_ERR_RETURN
        (_tscbh_speed_id_to_speed_config_get(phy, speed_id, num_lane, &speed_config));

    if (an_en && an_done) {
        PHYMOD_IF_ERR_RETURN
            (tbhmod_autoneg_fec_status_get(&phy_copy.access, &fec_arch));
        PHYMOD_IF_ERR_RETURN
            (tbhmod_fec_arch_decode_get(fec_arch, &(speed_config.fec_type)));
    }

    /* 10G-CL74 does not support 1588 */
    if ((speed_config.data_rate == 10000) && (speed_config.fec_type == phymod_fec_CL74)) {
        PHYMOD_DEBUG_ERROR(("1588 is not supported in current speed config.\n"));
        return PHYMOD_E_UNAVAIL;
    }

    /* 1.4 Get mapped speed id */
    if (speed_id <= 0x25) {
        /* AN speed IDs */
        mapped_speed_id = speed_id;
    } else {
        /* Customized speed ID, need extra mapping. */
        /* Only applys to FS */
        PHYMOD_IF_ERR_RETURN(_tscbh_phy_speed_id_set(num_lane, speed_config.data_rate,
                                                     speed_config.fec_type, &spd_intf));
        PHYMOD_IF_ERR_RETURN
            (tbhmod_get_mapped_speed(spd_intf, &mapped_speed_id));
    }

    /* 1.5 Get the table index and table size of the 1588 table */
    PHYMOD_IF_ERR_RETURN
       (tbhmod_1588_table_index_get(mapped_speed_id, speed_config.fec_type, &ts_table_index, &ts_table_size));

    if (ts_table_index == -1) {
        PHYMOD_DEBUG_ERROR(("1588 is not supported in current speed config.\n"));
        return PHYMOD_E_UNAVAIL;
    }

    psll_entry_size = ts_table_size * TBHMOD_TS_PSLL_BASED_ENTRY_SIZE;

    /* 1.6 Find the 1588 table */
    ts_tx_entry = (ts_table_entry*) &(ts_table_tx_sop[ts_table_index]);
    ts_rx_entry = (ts_table_entry*) &(ts_table_rx_sop[ts_table_index]);

    if (num_lane == 8) {
        tx_mem = phymodMemTxLkup1588400G;
        rx_mem = phymodMemRxLkup1588400G;
        intr_type = phymodIntrEccRx1588400g;
    } else if (start_lane < 4) {
        tx_mem = phymodMemTxLkup1588Mpp0;
        rx_mem = phymodMemRxLkup1588Mpp0;
    } else {
        tx_mem = phymodMemTxLkup1588Mpp1;
        rx_mem = phymodMemRxLkup1588Mpp1;
    }
    /* Get OS mode */
    PHYMOD_IF_ERR_RETURN
        (blackhawk_osr_mode_get(&phy_copy.access, &osr_mode));

    PHYMOD_IF_ERR_RETURN
        (tbhmod_refclk_get(&phy_copy.access, &ref_clk));

    /* Get current used VCO */
     PHYMOD_IF_ERR_RETURN
        (blackhawk_lane_pll_selection_get(&phy_copy.access, &pll_index));
    phy_copy.access.pll_idx = pll_index;
    PHYMOD_IF_ERR_RETURN
        (blackhawk_tsc_INTERNAL_read_pll_div(&phy_copy.access, &pll_div));
    PHYMOD_IF_ERR_RETURN
        (tbhmod_pll_to_vco_get(ref_clk, pll_div, &current_vco));
    /* Get TVCO */
    phy_copy.access.pll_idx = phy_copy.access.tvco_pll_index;
    PHYMOD_IF_ERR_RETURN
        (blackhawk_tsc_INTERNAL_read_pll_div(&phy_copy.access, &pll_div));
    PHYMOD_IF_ERR_RETURN
        (tbhmod_pll_to_vco_get(ref_clk, pll_div, &tvco));

    /* 2. Program UI */
    /* Assume clk4sync_div always 0 */
    PHYMOD_IF_ERR_RETURN
        (tbhmod_pcs_set_1588_ui(&phy_copy.access, current_vco, tvco, osr_mode, 0, is_pam4));

    /* 3. Program PMD lantency */
    PHYMOD_IF_ERR_RETURN
        (tbhmod_1588_pmd_latency(&phy_copy.access, current_vco, osr_mode, is_pam4));

    /* 4. Enable rx deskew
     *    Need low to high transition to trigger HW recording the current status.
    */
    if (ts_am_norm_mode == 0x1) {
        PHYMOD_IF_ERR_RETURN
            (tbhmod_pcs_rx_deskew_en(&phy_copy.access, 0));
        PHYMOD_USLEEP(10);
        PHYMOD_IF_ERR_RETURN
            (tbhmod_pcs_rx_deskew_en(&phy_copy.access, 1));
    }

    /* 5. Load 1588 TX table */
    /* Different MPPs have different memory space.
     * Different logical ports have different memory offset.
     */
    for (i = 0; i < (int)ts_table_size; i++) {
        PHYMOD_IF_ERR_RETURN
            (phymod_mem_write(&phy_copy.access, tx_mem, (i + mem_offset), &(*ts_tx_entry)[i][0]));
    }

    /* 6. Load 1588 RX table */
    if (ts_am_norm_mode == 0x1) {
        /* RX tbl need to update deskew before writing to mem */
        /* 6.1 Check for deskew valid */
        for (i = 0; i < 1000; i++) {
            PHYMOD_USLEEP(10);
            PHYMOD_IF_ERR_RETURN
                (tbhmod_pcs_ts_deskew_valid(&phy_copy.access, speed_config_entry.bit_mux_mode, &is_valid));
            if (is_valid) {
                break;
            }
        }
        if (!is_valid) {
            return PHYMOD_E_TIMEOUT;
        }

        /* 6.2 Update deskew to 1588 table */
        /* 6.2.1 Translate RX table to psuedo logical lane(PSLL) based array */
        for (i = 0; i < (int)ts_table_size; i++) {
            PHYMOD_IF_ERR_RETURN
                (tbhmod_tbl_entry_to_psll_entry_map(&(*ts_rx_entry)[i][0], &psll_entry[i * TBHMOD_TS_PSLL_BASED_ENTRY_SIZE]));
        }

        /* 6.2.2 Calculate deskew and update time value for each PSLL */
        PHYMOD_IF_ERR_RETURN
            (tbhmod_pcs_mod_rx_1588_tbl_val(&phy_copy.access, speed_config_entry.bit_mux_mode,
                                            current_vco, osr_mode, is_pam4, psll_entry_size, psll_entry));

        /* 6.2.3 Translate PSLL based array back to 1588 entry format */
        for (i = 0; i < (int)ts_table_size; i++) {
            PHYMOD_IF_ERR_RETURN
                (tbhmod_psll_entry_to_tbl_entry_map(&psll_entry[i * TBHMOD_TS_PSLL_BASED_ENTRY_SIZE], &ts_update_table[i][0]));
        }

        /* 2-stage SW WAR for RX 1588 memory accessing:
         * Stage 1: Before accessing RX 1588 memory.
         * Disable corresponding RX 1588 memory ECC interrupt.
         */
        if (intr_type == phymodIntrEccRx1588400g) {
            PHYMOD_IF_ERR_RETURN
                (tbhmod_ecc_error_intr_enable_set(&phy_copy.access, intr_type, 0));
        } else {
            /* RX 1588 ECC MPP0 is used for even memory, and MPP1 is for odd memory.
             * So for non-400G case, we need to disable both MPP0 and MPP1 RX 1588 ECC interrupts.
             */
            PHYMOD_IF_ERR_RETURN
                (tbhmod_ecc_error_intr_enable_set(&phy_copy.access, phymodIntrEccRx1588Mpp0, 0));
            PHYMOD_IF_ERR_RETURN
                (tbhmod_ecc_error_intr_enable_set(&phy_copy.access, phymodIntrEccRx1588Mpp1, 0));
        }
        /* 6.3 Write the updated RX table to memroy */
        for (i = 0; i < (int)ts_table_size; i++) {
            PHYMOD_IF_ERR_RETURN
                (phymod_mem_write(&phy_copy.access, rx_mem, (i + mem_offset), &ts_update_table[i][0]));
        }
    } else {
        /* 2-stage SW WAR for RX 1588 memory accessing:
         * Stage 1: Before accessing RX 1588 memory.
         * Disable corresponding RX 1588 memory ECC interrupt.
         */
        if (intr_type == phymodIntrEccRx1588400g) {
            PHYMOD_IF_ERR_RETURN
                (tbhmod_ecc_error_intr_enable_set(&phy_copy.access, intr_type, 0));
        } else {
            /* RX 1588 ECC MPP0 is used for even memory, and MPP1 is for odd memory.
             * So for non-400G case, we need to disable both MPP0 and MPP1 RX 1588 ECC interrupts.
             */
            PHYMOD_IF_ERR_RETURN
                (tbhmod_ecc_error_intr_enable_set(&phy_copy.access, phymodIntrEccRx1588Mpp0, 0));
            PHYMOD_IF_ERR_RETURN
                (tbhmod_ecc_error_intr_enable_set(&phy_copy.access, phymodIntrEccRx1588Mpp1, 0));
        }

        /* If deskew update is not required, load default tables */
        for (i = 0; i < (int)ts_table_size; i++) {
            PHYMOD_IF_ERR_RETURN
                (phymod_mem_write(&phy_copy.access, rx_mem, (i + mem_offset), &(*ts_rx_entry)[i][0]));
        }
    }

    /* 2-stage SW WAR for RX 1588 memory accessing:
     * Stage 2: After accessing RX 1588 memory.
     * Read the ECC status.
     *      If error address is within the range being written, ignore it.
     *      If error address is outside the modification range, print warning.
     *      ECC interrupt will be flagged during next HW access, which will invoke
     *      interrupt handler to handle the true ECC error.
     * Re-enable ECC interrupt.
     */
    if (intr_type == phymodIntrEccRx1588400g) {
        intr_status.type = intr_type;
        PHYMOD_IF_ERR_RETURN
            (tbhmod_intr_status_get(&phy_copy.access, &intr_status));
        if (intr_status.is_1b_err || intr_status.is_2b_err) {
            if ((intr_status.err_addr < mem_offset) ||
                (intr_status.err_addr >= (int)(mem_offset + ts_table_size))) {
                if (intr_status.is_2b_err) {
                    PHYMOD_DEBUG_ERROR(("WARNING :: core 0x%x detects 2-bit RX 1588 400G ECC error at address 0x%x.\n",
                                        phy_copy.access.addr, intr_status.err_addr));
                } else {
                    PHYMOD_DEBUG_ERROR(("WARNING :: core 0x%x detects 1-bit RX 1588 400G ECC error at address 0x%x.\n",
                                        phy_copy.access.addr, intr_status.err_addr));
                }
            }
        }
    } else {
        /* ECC status within the physical even 1588 RX MPP memory. */
        intr_status.type = phymodIntrEccRx1588Mpp0;
        PHYMOD_IF_ERR_RETURN
            (tbhmod_intr_status_get(&phy_copy.access, &intr_status));
        if (intr_status.is_1b_err || intr_status.is_2b_err) {
            if (((intr_status.err_addr * 2) < mem_offset) ||
                ((intr_status.err_addr * 2) >= (int)(mem_offset + ts_table_size))) {
                if (intr_status.is_2b_err) {
                    PHYMOD_DEBUG_ERROR(("WARNING :: core 0x%x detects 2-bit RX 1588 MPP0 ECC error at address 0x%x.\n",
                                        phy_copy.access.addr, intr_status.err_addr));
                } else {
                    PHYMOD_DEBUG_ERROR(("WARNING :: core 0x%x detects 1-bit RX 1588 MPP0 ECC error at address 0x%x.\n",
                                        phy_copy.access.addr, intr_status.err_addr));
                }
            }
        }
        /* ECC status within the physical odd 1588 RX MPP memory. */
        intr_status.type = phymodIntrEccRx1588Mpp1;
        PHYMOD_IF_ERR_RETURN
            (tbhmod_intr_status_get(&phy_copy.access, &intr_status));
        if (intr_status.is_1b_err || intr_status.is_2b_err) {
            if (((intr_status.err_addr * 2 + 1) < mem_offset) ||
                ((intr_status.err_addr * 2 + 1) >= (int)(mem_offset + ts_table_size))) {
                if (intr_status.is_2b_err) {
                    PHYMOD_DEBUG_ERROR(("WARNING :: core 0x%x detects 2-bit RX 1588 MPP1 ECC error at address 0x%x.\n",
                                        phy_copy.access.addr, intr_status.err_addr));
                } else {
                    PHYMOD_DEBUG_ERROR(("WARNING :: core 0x%x detects 1-bit RX 1588 MPP1 ECC error at address 0x%x.\n",
                                        phy_copy.access.addr, intr_status.err_addr));
                }
            }
        }
    }
    if (intr_type == phymodIntrEccRx1588400g) {
        PHYMOD_IF_ERR_RETURN
            (tbhmod_ecc_error_intr_enable_set(&phy_copy.access, intr_type, 1));
    } else {
        PHYMOD_IF_ERR_RETURN
            (tbhmod_ecc_error_intr_enable_set(&phy_copy.access, phymodIntrEccRx1588Mpp0, 1));
        PHYMOD_IF_ERR_RETURN
            (tbhmod_ecc_error_intr_enable_set(&phy_copy.access, phymodIntrEccRx1588Mpp1, 1));
    }
    /* 7. Enable SFD/SOP timestamping on tx and rx */
    /* In Gen1, only applys to 10G, 20G and 25G. */
    PHYMOD_IF_ERR_RETURN
        (tbhmod_pcs_set_1588_xgmii(&phy_copy.access, 0, current_vco, osr_mode));

    return PHYMOD_E_NONE;
}


STATIC
int _tscbh_flexport_sw_workaround(const phymod_phy_access_t* phy)
{
    phymod_phy_access_t phy_copy;
    int start_lane, num_lane, mapped_speed_id;
    uint32_t pll_div, pll_index;
    uint32_t packed_entry[5];
    tbhmod_spd_intfc_type_t spd_intf = 0;

    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(&phy->access, &start_lane, &num_lane));
    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));
    /* enable PMD lane override */
    phy_copy.access.lane_mask = 1 << start_lane;

    /* Hold the pcs lane reset */
    PHYMOD_IF_ERR_RETURN
        (tbhmod_disable_set(&phy_copy.access));

    /* get PLL index */
    PHYMOD_IF_ERR_RETURN
        (blackhawk_lane_pll_selection_get(&phy_copy.access, &pll_index));

    /* get the PLL div */
    phy_copy.access.lane_mask = 0x1;
    phy_copy.access.pll_idx = pll_index;
    PHYMOD_IF_ERR_RETURN
        (blackhawk_tsc_INTERNAL_read_pll_div(&phy_copy.access, &pll_div));

    /* for 26G VCO, use speed id 5
       for 25G vco, use speed id 2
       for 20G vco, use speed id 0 */
    if ((pll_div == TBHMOD_PLL_MODE_DIV_170) || (pll_div == TBHMOD_PLL_MODE_DIV_85))  {
        spd_intf = TBHMOD_SPD_50G_IEEE_KR1_CR1;
    } else if ((pll_div == (uint32_t)TBHMOD_PLL_MODE_DIV_165) || (pll_div == (uint32_t)TBHMOD_PLL_MODE_DIV_82P5)) {
        spd_intf = TBHMOD_SPD_25000_XFI;
    } else {
        spd_intf = TBHMOD_SPD_10000_XFI;
    }

    PHYMOD_IF_ERR_RETURN
        (tbhmod_get_mapped_speed(spd_intf, &mapped_speed_id));

    /*next read the speed id entry and then copy to the right forced speed */
    phy_copy.access.lane_mask = 1 << 0;
    PHYMOD_IF_ERR_RETURN
        (phymod_mem_read(&phy_copy.access, phymodMemSpeedIdTable, mapped_speed_id, packed_entry));
    PHYMOD_IF_ERR_RETURN
        (phymod_mem_write(&phy_copy.access, phymodMemSpeedIdTable, TSCBH_FORCED_SPEED_ID_OFFSET + start_lane, packed_entry));

    /* next update the port_mode */
    phy_copy.access.lane_mask = 1 << start_lane;
    PHYMOD_IF_ERR_RETURN
        (tbhmod_update_port_mode(&phy_copy.access));

    /* add rx lock override */
    PHYMOD_IF_ERR_RETURN
        (tbhmod_pmd_rx_lock_override_enable(&phy_copy.access, 1));
    /* clear state machine state */
    PHYMOD_IF_ERR_RETURN
        (tbhmod_read_sc_fsm_status(&phy_copy.access));
    PHYMOD_IF_ERR_RETURN
        (tbhmod_read_sc_done(&phy_copy.access));
    PHYMOD_IF_ERR_RETURN
        (tbhmod_enable_set(&phy_copy.access));
    PHYMOD_IF_ERR_RETURN
        (tbhmod_polling_for_sc_done(&phy_copy.access));
    /* disable pcs again */
    PHYMOD_IF_ERR_RETURN
        (tbhmod_disable_set(&phy_copy.access));
    PHYMOD_USLEEP(10000);
    /* disable rx lock override */
    PHYMOD_IF_ERR_RETURN
        (tbhmod_pmd_rx_lock_override_enable(&phy_copy.access, 0));

    return PHYMOD_E_NONE;
}

int tscbh_phy_pcs_enable_set(const phymod_phy_access_t* phy, uint32_t enable)
{
    phymod_phy_access_t phy_copy;
    int start_lane, num_lane;

    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(&phy->access, &start_lane, &num_lane));
    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));

    if (enable == 1) {
        phy_copy.access.lane_mask = 1 << start_lane;
        PHYMOD_IF_ERR_RETURN
            (tbhmod_enable_set(&phy_copy.access));
    } else if (enable == 0) {
        PHYMOD_IF_ERR_RETURN
            (tbhmod_disable_set(&phy_copy.access));
    } else {
        /* this is the SW WAR for the 16nm flexport HW issue */
        PHYMOD_IF_ERR_RETURN
            (_tscbh_flexport_sw_workaround(phy));
    }

    return PHYMOD_E_NONE;
}

int tscbh_phy_synce_clk_ctrl_set(const phymod_phy_access_t* phy,
                                 phymod_synce_clk_ctrl_t cfg)
{
    phymod_phy_access_t phy_copy;
    uint32_t current_pll_index, pll_div, sdm_val;
    int osr_mode;

    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));

    PHYMOD_IF_ERR_RETURN
         (tbhmod_synce_mode_set(&phy_copy.access, cfg.stg0_mode, cfg.stg1_mode));

    /* next check if SDM mode, if yes, needs to figure out the SDM value based on the current */
    if ((cfg.stg0_mode == 0x2) && (cfg.stg1_mode == 0x0)) {
        /* vco the port is using */
        /* first get the PLL index */
        PHYMOD_IF_ERR_RETURN
            (blackhawk_lane_pll_selection_get(&phy_copy.access, &current_pll_index));

        /* next get the OSR the port is current using */
        PHYMOD_IF_ERR_RETURN
            (blackhawk_osr_mode_get(&phy_copy.access, &osr_mode));

        /* next get the VCO the port is current using */
        phy_copy.access.pll_idx = current_pll_index;
        phy_copy.access.lane_mask = 0x1;
        PHYMOD_IF_ERR_RETURN
            (blackhawk_tsc_INTERNAL_read_pll_div(&phy_copy.access, &pll_div));

        /* next based on the VCO value and osr mode, set the SDM value properly */
        /* first check 26G VCO */
        if ((pll_div == TBHMOD_PLL_MODE_DIV_170) || (pll_div == TBHMOD_PLL_MODE_DIV_85))  {
            sdm_val = TSCBH_SYNCE_SDM_DIVISOR_26G_VCO;
        } else if ((pll_div == (uint32_t)TBHMOD_PLL_MODE_DIV_165) || (pll_div == (uint32_t)TBHMOD_PLL_MODE_DIV_82P5)) {
            sdm_val = TSCBH_SYNCE_SDM_DIVISOR_25GP78125_VCO;
        } else if ((pll_div == TBHMOD_PLL_MODE_DIV_132) || (pll_div == TBHMOD_PLL_MODE_DIV_66)) {
            sdm_val = TSCBH_SYNCE_SDM_DIVISOR_20G_VCO;
        } else if ((pll_div == TBHMOD_PLL_MODE_DIV_160) || (pll_div == TBHMOD_PLL_MODE_DIV_80)) {
            sdm_val = TSCBH_SYNCE_SDM_DIVISOR_25G_VCO;
        } else {
            PHYMOD_DEBUG_ERROR(("Unsupported VCOs\n"));
            return PHYMOD_E_UNAVAIL;
        }

        /* need to adjust the SDM value based on OSR mode */
        if (osr_mode == 1) {
            sdm_val >>= 1;
        }

        /* next configure the SDM value */
        PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));
        PHYMOD_IF_ERR_RETURN
            (tbhmod_synce_clk_ctrl_set(&phy_copy.access, sdm_val));
	}

    return PHYMOD_E_NONE;
}

int tscbh_phy_synce_clk_ctrl_get(const phymod_phy_access_t* phy,
                                 phymod_synce_clk_ctrl_t *cfg)
{
    phymod_phy_access_t phy_copy;

    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));

    PHYMOD_IF_ERR_RETURN
        (tbhmod_synce_mode_get(&phy_copy.access, &(cfg->stg0_mode), &(cfg->stg1_mode)));

    PHYMOD_IF_ERR_RETURN
        (tbhmod_synce_clk_ctrl_get(&phy_copy.access, &(cfg->sdm_val)));

    return PHYMOD_E_NONE;
}

int tscbh_phy_rx_ppm_get(const phymod_phy_access_t* phy, int16_t* rx_ppm)
{
    int start_lane, num_lane;
    phymod_phy_access_t pm_phy_copy;

    PHYMOD_MEMCPY(&pm_phy_copy, phy, sizeof(pm_phy_copy));

    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(&phy->access, &start_lane, &num_lane));

    pm_phy_copy.access.lane_mask = 1 << start_lane;
    PHYMOD_IF_ERR_RETURN
        (blackhawk_tsc_rx_ppm(&pm_phy_copy.access, rx_ppm));
    return PHYMOD_E_NONE;
}

/* This function will handle PCS ECC interrupts.
 * 1. Clear interrupt status.
 * 2. Re-load the table entry if error is in UM, AM table.
 * 3. Re-load the 1588 tables and SPEED_ID table entry for 1b error.
 * 4. Return is_handled = 0 if 2b-error happens and Phymod can not recover.
 */
int tscbh_intr_handler(const phymod_phy_access_t* phy,
                       phymod_interrupt_type_t type,
                       uint32_t* is_handled)
{
    phymod_phy_access_t phy_copy;
    tbhmod_intr_status_t intr_status;
    uint32_t speed_id_table_entry[TSCBH_SPEED_ID_ENTRY_SIZE];
    uint32_t ts_table_entry_local[TBHMOD_TS_ENTRY_SIZE];
    int index;

    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));
    intr_status.type = type;

    /* Get and clear interrupt status */
    PHYMOD_IF_ERR_RETURN
        (tbhmod_intr_status_get(&phy_copy.access, &intr_status));

    phy_copy.access.lane_mask = 0x1;

    if (intr_status.is_2b_err) {
        *is_handled = 0;
    } else {
        *is_handled = 1;
    }

    switch (type) {
        case phymodIntrEccAMTable:
            if (intr_status.is_2b_err || intr_status.is_1b_err) {
                index = intr_status.err_addr;
                if (index >= TSCBH_HW_AM_TABLE_SIZE) {
                    return PHYMOD_E_FAIL;
                }
                /* Reload AM table entry with either 1b or 2b ECC error. */
                /* am_table_entry table size equals to TSCBH_HW_AM_TABLE_SIZE,
                * so no need to do extra boundrary check.
                */
                PHYMOD_IF_ERR_RETURN
                    (phymod_mem_write(&phy_copy.access, phymodMemAMTable, index,  &am_table_entry[index][0]));
                *is_handled = 1;
            }
            break;
        case phymodIntrEccSpeedTable:
            if (intr_status.is_1b_err) {
                index = intr_status.err_addr;
                if (index >= TSCBH_HW_SPEED_ID_TABLE_SIZE) {
                    return PHYMOD_E_FAIL;
                }
                PHYMOD_IF_ERR_RETURN
                    (phymod_mem_read(&phy_copy.access, phymodMemSpeedIdTable, index, speed_id_table_entry));
                PHYMOD_IF_ERR_RETURN
                    (phymod_mem_write(&phy_copy.access, phymodMemSpeedIdTable, index, speed_id_table_entry));
            }
            break;
        case phymodIntrEccUMTable:
            if (intr_status.is_2b_err || intr_status.is_1b_err) {
                index = intr_status.err_addr;
                if (index >= TSCBH_HW_UM_TABLE_SIZE) {
                    return PHYMOD_E_FAIL;
                }
                PHYMOD_IF_ERR_RETURN
                    (phymod_mem_write(&phy_copy.access, phymodMemUMTable, index,  &um_table_entry[index][0]));
                *is_handled = 1;
            }

            break;
        case phymodIntrEccRx1588400g:
            if (intr_status.is_1b_err) {
                index = intr_status.err_addr;
                if (index >= TBHMOD_TS_DEFAULT_TABLE_SIZE) {
                    return PHYMOD_E_FAIL;
                }
                PHYMOD_IF_ERR_RETURN
                    (phymod_mem_read(&phy_copy.access, phymodMemRxLkup1588400G, index, ts_table_entry_local));
                PHYMOD_IF_ERR_RETURN
                    (phymod_mem_write(&phy_copy.access, phymodMemRxLkup1588400G, index, ts_table_entry_local));
            }
            break;
        case phymodIntrEccRx1588Mpp1:
            if (intr_status.is_1b_err) {
                index = intr_status.err_addr;
                if (index >= TBHMOD_TS_MPP_MEM_SIZE) {
                    return PHYMOD_E_FAIL;
                }
                PHYMOD_IF_ERR_RETURN
                    (phymod_mem_read(&phy_copy.access, phymodMemRxLkup1588Mpp1,
                                     index, ts_table_entry_local));
                PHYMOD_IF_ERR_RETURN
                    (phymod_mem_write(&phy_copy.access, phymodMemRxLkup1588Mpp1,
                                      index, ts_table_entry_local));
            }
            break;
        case phymodIntrEccRx1588Mpp0:
            if (intr_status.is_1b_err) {
                index = intr_status.err_addr;
                if (index >= TBHMOD_TS_MPP_MEM_SIZE) {
                    return PHYMOD_E_FAIL;
                }
                PHYMOD_IF_ERR_RETURN
                    (phymod_mem_read(&phy_copy.access, phymodMemRxLkup1588Mpp0,
                                     index, ts_table_entry_local));
                PHYMOD_IF_ERR_RETURN
                    (phymod_mem_write(&phy_copy.access, phymodMemRxLkup1588Mpp0,
                                      index, ts_table_entry_local));
            }
            break;
        case phymodIntrEccTx1588400g:
            if (intr_status.is_1b_err) {
                index = intr_status.err_addr;
                if (index >= TBHMOD_TS_DEFAULT_TABLE_SIZE) {
                    return PHYMOD_E_FAIL;
                }
                PHYMOD_IF_ERR_RETURN
                    (phymod_mem_read(&phy_copy.access, phymodMemTxLkup1588400G, index, ts_table_entry_local));
                PHYMOD_IF_ERR_RETURN
                    (phymod_mem_write(&phy_copy.access, phymodMemTxLkup1588400G, index, ts_table_entry_local));
            }
            break;
        case phymodIntrEccTx1588Mpp1:
            if (intr_status.is_1b_err) {
                index = intr_status.err_addr;
                if (index >= TBHMOD_TS_MPP_MEM_SIZE) {
                    return PHYMOD_E_FAIL;
                }
                PHYMOD_IF_ERR_RETURN
                    (phymod_mem_read(&phy_copy.access, phymodMemTxLkup1588Mpp1,
                                     index, ts_table_entry_local));
                PHYMOD_IF_ERR_RETURN
                    (phymod_mem_write(&phy_copy.access, phymodMemTxLkup1588Mpp1,
                                      index, ts_table_entry_local));
            }
            break;
        case phymodIntrEccTx1588Mpp0:
            if (intr_status.is_1b_err) {
                index = intr_status.err_addr;
                if (index >= TBHMOD_TS_MPP_MEM_SIZE) {
                    return PHYMOD_E_FAIL;
                }
                PHYMOD_IF_ERR_RETURN
                    (phymod_mem_read(&phy_copy.access, phymodMemTxLkup1588Mpp0,
                                     index, ts_table_entry_local));
                PHYMOD_IF_ERR_RETURN
                    (phymod_mem_write(&phy_copy.access, phymodMemTxLkup1588Mpp0,
                                      index, ts_table_entry_local));
            }
            break;
        default:
            break;
    }

    return PHYMOD_E_NONE;
}

int tscbh_timesync_tx_info_get(const phymod_phy_access_t* phy, phymod_ts_fifo_status_t* ts_tx_info)
{
    phymod_phy_access_t phy_copy;
    tbhmod_ts_tx_info_t local_ts_tx_info;

    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));
    PHYMOD_MEMSET(&local_ts_tx_info, 0, sizeof(tbhmod_ts_tx_info_t));

    PHYMOD_IF_ERR_RETURN(tbhmod_1588_tx_info_get(&phy_copy.access, &local_ts_tx_info));

    ts_tx_info->ts_in_fifo_lo = ((uint32_t)(local_ts_tx_info.ts_val_mid << 16)) | ((uint32_t)local_ts_tx_info.ts_val_lo);
    ts_tx_info->ts_in_fifo_hi = (uint32_t)local_ts_tx_info.ts_val_hi;
    ts_tx_info->ts_seq_id = (uint32_t)local_ts_tx_info.ts_seq_id;
    ts_tx_info->ts_sub_nanosec = (uint32_t)local_ts_tx_info.ts_sub_nanosec;

    return PHYMOD_E_NONE;
}

int tscbh_phy_pcs_lane_swap_adjust(const phymod_phy_access_t* phy, uint32_t active_lane_map, uint32_t original_tx_lane_map, uint32_t original_rx_lane_map)
{
    phymod_phy_access_t phy_copy;
    int start_lane, num_lane, i, disable_lane[2]= {0,0}, disable_lane_num = 0;
    int tx_physical_lane, tx_logical_lane, rx_physical_lane, rx_logical_lane;
    uint32_t new_tx_lane_map, new_rx_lane_map, swap_mask, tmp_tx_lane_map, tmp_rx_lane_map;

    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(&phy->access, &start_lane, &num_lane));
    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));

    /* next based on active_lane_map, figure out the disable lane num  and index*/
    for (i = 0; i < num_lane; i++)
    {
        if(active_lane_map & (1 << i))
        {
            continue;
        } else {
            disable_lane[disable_lane_num] = i + start_lane;
            disable_lane_num++;
        }
    }

    /* next check if disable_lane_num 0 */
    if (disable_lane_num == 0) {
        new_tx_lane_map = original_tx_lane_map;
        new_rx_lane_map = original_rx_lane_map;

        /* next check if 4 lane or 8 lane port */
        if (num_lane == 8) {
            PHYMOD_IF_ERR_RETURN
                (tbhmod_pcs_tx_lane_swap(&phy_copy.access, new_tx_lane_map));
            PHYMOD_IF_ERR_RETURN
                (tbhmod_pcs_rx_lane_swap(&phy_copy.access, new_rx_lane_map));
        } else { /* 4 lane port */
            for (i = 0; i < num_lane; i++) {
                tx_logical_lane = i + start_lane;
                rx_logical_lane = i + start_lane;
                tx_physical_lane = (original_tx_lane_map & (0xf << (tx_logical_lane * 4)) ) >> (tx_logical_lane * 4);
                rx_physical_lane = (original_rx_lane_map & (0xf << (rx_logical_lane * 4)) ) >> (rx_logical_lane * 4);
                tx_physical_lane &= 0xf;
                rx_physical_lane &= 0xf;
                PHYMOD_IF_ERR_RETURN
                    (tbhmod_pcs_single_tx_lane_swap_set(&phy_copy.access, tx_physical_lane, tx_logical_lane));
                PHYMOD_IF_ERR_RETURN
                    (tbhmod_pcs_single_rx_lane_swap_set(&phy_copy.access, rx_physical_lane, rx_logical_lane));
            }
        }
    } else  {
        if (num_lane == 4) {
            /* first check if disable lane is the last, if yes, then need to adjust */
            if ((disable_lane[0] == 3) || (disable_lane[0] == 7)) {
                return PHYMOD_E_NONE;
            } else if (disable_lane[0] < 4) {
                /* mpp0 */
                tx_logical_lane = disable_lane[0];
                rx_logical_lane = disable_lane[0];
                tx_physical_lane = (original_tx_lane_map & (0xf << (3 * 4)) & 0xffff) >> (3 * 4);
                rx_physical_lane = (original_rx_lane_map & (0xf << (3 * 4)) & 0xffff) >> (3 * 4);
                tx_physical_lane &= 0xf;
                rx_physical_lane &= 0xf;
                PHYMOD_IF_ERR_RETURN
                    (tbhmod_pcs_single_tx_lane_swap_set(&phy_copy.access, tx_physical_lane, tx_logical_lane));
                PHYMOD_IF_ERR_RETURN
                    (tbhmod_pcs_single_rx_lane_swap_set(&phy_copy.access, rx_physical_lane, rx_logical_lane));
                /* mpp0 */
                tx_logical_lane = 3;
                rx_logical_lane = 3;
                tx_physical_lane = (original_tx_lane_map & (0xf << (disable_lane[0] * 4)) & 0xffff) >> (disable_lane[0] * 4);
                rx_physical_lane = (original_rx_lane_map & (0xf << (disable_lane[0] * 4)) & 0xffff) >> (disable_lane[0] * 4);
                tx_physical_lane &= 0xf;
                rx_physical_lane &= 0xf;
                PHYMOD_IF_ERR_RETURN
                    (tbhmod_pcs_single_tx_lane_swap_set(&phy_copy.access, tx_physical_lane, tx_logical_lane));
                PHYMOD_IF_ERR_RETURN
                    (tbhmod_pcs_single_rx_lane_swap_set(&phy_copy.access, rx_physical_lane, rx_logical_lane));
            } else {
                /* mpp1 */
                tx_logical_lane = disable_lane[0];
                rx_logical_lane = disable_lane[0];
                tx_physical_lane = (original_tx_lane_map & (0xf << (7 * 4)) & 0xffff0000) >> (7 * 4);
                rx_physical_lane = (original_rx_lane_map & (0xf << (7 * 4)) & 0xffff0000) >> (7 * 4);
                tx_physical_lane &= 0xf;
                rx_physical_lane &= 0xf;
                PHYMOD_IF_ERR_RETURN
                    (tbhmod_pcs_single_tx_lane_swap_set(&phy_copy.access, tx_physical_lane, tx_logical_lane));
                PHYMOD_IF_ERR_RETURN
                    (tbhmod_pcs_single_rx_lane_swap_set(&phy_copy.access, rx_physical_lane, rx_logical_lane));
                /* mpp1 */
                tx_logical_lane = 7;
                rx_logical_lane = 7;
                tx_physical_lane = (original_tx_lane_map & (0xf << (disable_lane[0] * 4)) & 0xffff0000) >> (disable_lane[0] * 4);
                rx_physical_lane = (original_rx_lane_map & (0xf << (disable_lane[0] * 4)) & 0xffff0000) >> (disable_lane[0] * 4);
                tx_physical_lane &= 0xf;
                rx_physical_lane &= 0xf;
                PHYMOD_IF_ERR_RETURN
                    (tbhmod_pcs_single_tx_lane_swap_set(&phy_copy.access, tx_physical_lane, tx_logical_lane));
                PHYMOD_IF_ERR_RETURN
                    (tbhmod_pcs_single_rx_lane_swap_set(&phy_copy.access, rx_physical_lane, rx_logical_lane));
            }
        } else if (num_lane == 8) {
            /* first check if disable lane is the last, if yes, then no need to adjust */
            if (disable_lane_num == 1) {
                /* no need to do pcs adjust */
                if (disable_lane[0] == 7) {
                    return PHYMOD_E_NONE;
                } else {
                    new_tx_lane_map = (original_tx_lane_map & (0xf << (7 * 4)) & 0xffffffff) >> (4 * (7 - disable_lane[0]));
                    new_tx_lane_map |= (original_tx_lane_map & (0xf << (disable_lane[0] * 4)) & 0xffffffff) << (4 * (7 - disable_lane[0]));
                    new_rx_lane_map = (original_rx_lane_map & (0xf << (7 * 4)) & 0xffffffff) >> (4 * (7 - disable_lane[0]));
                    new_rx_lane_map |= (original_rx_lane_map & (0xf << (disable_lane[0] * 4)) & 0xffffffff) << (4 * (7 - disable_lane[0]));
                    swap_mask = ~(0xf << (4 * 7) | 0xf << (4 * disable_lane[0]));
                    new_tx_lane_map |=  (original_tx_lane_map & swap_mask);
                    new_rx_lane_map |=  (original_rx_lane_map & swap_mask);
                }
            } else { /*2 lane disabled */
                if ((disable_lane[0] == 6)  && (disable_lane[1] == 7)) {
                    /* no need to do pcs adjust */
                    return PHYMOD_E_NONE;
                } else if (disable_lane[1] != 6) {
                    new_tx_lane_map = (original_tx_lane_map & (0xf << (6 * 4)) & 0xffffffff) >> (4 * (6 - disable_lane[0]));
                    new_tx_lane_map |= (original_tx_lane_map & (0xf << (disable_lane[0] * 4)) & 0xffffffff) << (4 * (6 - disable_lane[0]));
                    tmp_tx_lane_map = (original_tx_lane_map & (0xf << (7 * 4)) & 0xffffffff) >> (4 * (7 - disable_lane[1]));
                    tmp_tx_lane_map |= (original_tx_lane_map & (0xf << (disable_lane[1] * 4)) & 0xffffffff) <<  (4 * (7 - disable_lane[1]));
                    new_tx_lane_map |= tmp_tx_lane_map;
                    new_rx_lane_map = (original_rx_lane_map & (0xf << (6 * 4)) & 0xffffffff) >> (4 * (6 - disable_lane[0]));
                    new_rx_lane_map |= (original_rx_lane_map & (0xf << (disable_lane[0] * 4)) & 0xffffffff) << (4 * (6 - disable_lane[0]));
                    tmp_rx_lane_map = (original_rx_lane_map & (0xf << (7 * 4)) & 0xffffffff) >> (4 * (7 - disable_lane[1]));
                    tmp_rx_lane_map |= (original_rx_lane_map & (0xf << (disable_lane[1] * 4)) & 0xffffffff) << (4 * (7 - disable_lane[1]));
                    new_rx_lane_map |= tmp_rx_lane_map;
                    swap_mask = ~(0xff << (4 * 6) | 0xf << (4 * disable_lane[0]) | 0xf << (4 * disable_lane[1]));
                    new_tx_lane_map |=  (original_tx_lane_map & swap_mask);
                    new_rx_lane_map |=  (original_rx_lane_map & swap_mask);
                } else {
                    tmp_tx_lane_map = (original_tx_lane_map & (0xf << (7 * 4)) & 0xffffffff) >> (4 * (7 - disable_lane[0]));
                    tmp_tx_lane_map |= (original_tx_lane_map & (0xf << (disable_lane[0] * 4)) & 0xffffffff) <<  (4 * (7 - disable_lane[0]));
                    new_tx_lane_map = tmp_tx_lane_map;
                    tmp_rx_lane_map = (original_rx_lane_map & (0xf << (7 * 4)) & 0xffffffff) >> (4 * (7 - disable_lane[0]));
                    tmp_rx_lane_map |= (original_rx_lane_map & (0xf << (disable_lane[0] * 4)) & 0xffffffff) << (4 * (7 - disable_lane[0]));
                    new_rx_lane_map = tmp_rx_lane_map;
                    swap_mask = ~(0xf << (4 * 7) | 0xf << (4 * disable_lane[0]));
                    new_tx_lane_map |=  (original_tx_lane_map & swap_mask);
                    new_rx_lane_map |=  (original_rx_lane_map & swap_mask);
                }
            }

            PHYMOD_IF_ERR_RETURN
                (tbhmod_pcs_tx_lane_swap(&phy_copy.access, new_tx_lane_map));
            PHYMOD_IF_ERR_RETURN
                (tbhmod_pcs_rx_lane_swap(&phy_copy.access, new_rx_lane_map));
        }
    }
    return PHYMOD_E_NONE;
}

int tscbh_phy_load_speed_id_entry(const phymod_phy_access_t* phy, uint32_t speed, uint32_t num_lane, phymod_fec_type_t fec_type)
{
    phymod_phy_access_t phy_copy;
    int start_lane, local_num_lane, mapped_speed_id;
    tbhmod_spd_intfc_type_t spd_intf;
    uint32_t tvco_pll_div;

    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(&phy->access, &start_lane, &local_num_lane));
    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));

    /* first check the speed */
    switch (speed) {
    case 150000:
        spd_intf = TBHMOD_SPD_150G_FEC_544_2XN_N3;
        break;
    case 350000:
        spd_intf = TBHMOD_SPD_350G_FEC_544_2XN_N7;
        break;
    case 300000:
        spd_intf = TBHMOD_SPD_300G_FEC_544_2XN_N6;
        break;
    case 400000:
        spd_intf = TBHMOD_SPD_400G_BRCM_FEC_544_2XN_X8;
        break;
    case 200000:
        if (fec_type == phymod_fec_RS544_2XN) {
            spd_intf = TBHMOD_SPD_200G_IEEE_FEC_544_2XN_KR4_CR4;
        } else if (fec_type == phymod_fec_RS544) {
            spd_intf = TBHMOD_SPD_200G_BRCM_FEC_544_1XN_KR4_CR4;
        } else if (fec_type == phymod_fec_RS272) {
            spd_intf = TBHMOD_SPD_200G_BRCM_FEC_272_1XN_KR4_CR4;
        } else {
            PHYMOD_DEBUG_ERROR(("Unsupported 200G fec type\n"));
            return PHYMOD_E_UNAVAIL;
        }
        break;
    default:
        PHYMOD_DEBUG_ERROR(("Unsupported speed for speed id load function \n"));
        return PHYMOD_E_UNAVAIL;
    }

    PHYMOD_IF_ERR_RETURN
        (tbhmod_get_mapped_speed(spd_intf, &mapped_speed_id));

    /* Get TVCO because it's not allowed to change during speed set */
    phy_copy.access.pll_idx = phy->access.tvco_pll_index;

    PHYMOD_IF_ERR_RETURN
        (blackhawk_tsc_INTERNAL_read_pll_div(&phy_copy.access, &tvco_pll_div));

    /* based on the current TVCO PLL div, decide which copy of speed id entry to load */
    /* first set the lane mask to be 0x1 */
    phy_copy.access.lane_mask = 1 << 0;
    if ((tvco_pll_div == TBHMOD_PLL_MODE_DIV_170) || (tvco_pll_div == TBHMOD_PLL_MODE_DIV_85))  {
        /* then load 26G TVCO speed id entry */
        PHYMOD_IF_ERR_RETURN
            (phymod_mem_write(&phy_copy.access, phymodMemSpeedIdTable, TSCBH_FORCED_SPEED_ID_OFFSET + start_lane, &spd_id_entry_26[mapped_speed_id][0]));
    } else if ((tvco_pll_div == (uint32_t)TBHMOD_PLL_MODE_DIV_165) || (tvco_pll_div == (uint32_t)TBHMOD_PLL_MODE_DIV_82P5)) {
        /* then load 25G TVCO speed id entry */
        PHYMOD_IF_ERR_RETURN
            (phymod_mem_write(&phy_copy.access, phymodMemSpeedIdTable, TSCBH_FORCED_SPEED_ID_OFFSET + start_lane, &spd_id_entry_25[mapped_speed_id][0]));
    } else {
        /* then load 20G TVCO speed id entry */
        PHYMOD_IF_ERR_RETURN
            (phymod_mem_write(&phy_copy.access, phymodMemSpeedIdTable, TSCBH_FORCED_SPEED_ID_OFFSET + start_lane, &spd_id_entry_20[mapped_speed_id][0]));
    }

    return PHYMOD_E_NONE;
}

int tscbh_phy_pmd_override_enable_set(const phymod_phy_access_t* phy,
                                   phymod_override_type_t pmd_override_type,
                                   uint32_t override_enable,
                                   uint32_t override_val)
{
    phymod_phy_access_t phy_copy;
    int start_lane, num_lane;

    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(&phy->access, &start_lane, &num_lane));
    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));

    PHYMOD_IF_ERR_RETURN
        (tbhmod_pmd_override_enable_set(&phy_copy.access, pmd_override_type, override_enable, override_val));

    return PHYMOD_E_NONE;
}

int tscbh_phy_pll_powerdown_get(const phymod_phy_access_t* phy, uint32_t pll_index, uint32_t* powerdown)
{
    phymod_phy_access_t phy_copy;

    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));

    phy_copy.access.pll_idx = pll_index;
    phy_copy.access.lane_mask = 1 << 0;
    PHYMOD_IF_ERR_RETURN
        (blackhawk_tsc_pll_pwrdn_get(&phy_copy.access, powerdown));

    return PHYMOD_E_NONE;
}

int tscbh_phy_codec_mode_set(const phymod_phy_access_t* phy, phymod_phy_codec_mode_t codec_type)
{
    phymod_phy_access_t phy_copy;
    spd_id_tbl_entry_t speed_config_entry;
    uint32_t packed_entry[5];
    int start_lane, num_lane;
    uint32_t lane_reset, pcs_lane_enable;

    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(&phy->access, &start_lane, &num_lane));
    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));

    /*first check if lane is in reset */
    PHYMOD_IF_ERR_RETURN
        (blackhawk_lane_soft_reset_get(&phy_copy.access, &lane_reset));

    /*next check if PCS lane is in reset */
    PHYMOD_IF_ERR_RETURN
        (tbhmod_enable_get(&phy_copy.access, &pcs_lane_enable));

    /* disable pcs lane if pcs lane not in rset */
    if (pcs_lane_enable) {
        phy_copy.access.lane_mask = 1 << start_lane;
        PHYMOD_IF_ERR_RETURN
            (tbhmod_disable_set(&phy_copy.access));

        /* add the pcs disable SW WAR */
        PHYMOD_IF_ERR_RETURN
            (tbhmod_pcs_reset_sw_war(&phy->access));
    }

    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));
    /* if lane is not in reset, then reset the lane first */
    if (!lane_reset) {
        PHYMOD_IF_ERR_RETURN
            (blackhawk_lane_soft_reset(&phy_copy.access, 1));
    }

    /* first read the current speed id entry for this port */
    PHYMOD_IF_ERR_RETURN
        (phymod_mem_read(&phy_copy.access, phymodMemSpeedIdTable, TSCBH_FORCED_SPEED_ID_OFFSET + start_lane, packed_entry));

    spd_ctrl_unpack_spd_id_tbl_entry(packed_entry, &speed_config_entry);

    /* update the codec field */
    speed_config_entry.codec_mode = codec_type;

    /* next pack the speed config entry into 5 word format */
    spd_ctrl_pack_spd_id_tbl_entry(&speed_config_entry, &packed_entry[0]);

    /* write back to the speed id HW table */
    PHYMOD_IF_ERR_RETURN
        (phymod_mem_write(&phy_copy.access, phymodMemSpeedIdTable, TSCBH_FORCED_SPEED_ID_OFFSET + start_lane, &packed_entry[0]));

    /* release the ln dp reset */
    if (!lane_reset) {
        PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));
        PHYMOD_IF_ERR_RETURN
            (blackhawk_lane_soft_reset(&phy_copy.access, 0));
    }

    /* re-enable pcs lane if pcs lane not in rset */
    if (pcs_lane_enable) {
        phy_copy.access.lane_mask = 1 << start_lane;
        PHYMOD_IF_ERR_RETURN
            (tbhmod_enable_set(&phy_copy.access));
    }

    return PHYMOD_E_NONE;
}

int tscbh_phy_codec_mode_get(const phymod_phy_access_t* phy, phymod_phy_codec_mode_t* codec_type)
{
    phymod_phy_access_t phy_copy;
    spd_id_tbl_entry_t speed_config_entry;
    uint32_t packed_entry[5];
    int start_lane, num_lane, speed_id;

    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(&phy->access, &start_lane, &num_lane));
    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));

    phy_copy.access.lane_mask = 1 << start_lane;

    /* first read speed id from resolved status */
    PHYMOD_IF_ERR_RETURN
        (tbhmod_speed_id_get(&phy_copy.access, &speed_id));

    /* first read the current speed id entry for this port */
    PHYMOD_IF_ERR_RETURN
        (phymod_mem_read(&phy_copy.access, phymodMemSpeedIdTable, speed_id, packed_entry));

    spd_ctrl_unpack_spd_id_tbl_entry(packed_entry, &speed_config_entry);

    /* the codeec type */
    *codec_type = speed_config_entry.codec_mode;

    return PHYMOD_E_NONE;

}

int tscbh_phy_fec_bypass_indication_set(const phymod_phy_access_t* phy,
                                        uint32_t enable)
{
    int start_lane, num_lane;
    phymod_phy_access_t phy_copy;
    uint32_t lane_reset, pcs_lane_enable;

    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));

    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(&phy->access, &start_lane, &num_lane));

    /*first check if lane is in reset */
    PHYMOD_IF_ERR_RETURN
        (blackhawk_lane_soft_reset_get(&phy_copy.access, &lane_reset));

    /*next check if PCS lane is in reset */
    PHYMOD_IF_ERR_RETURN
        (tbhmod_enable_get(&phy_copy.access, &pcs_lane_enable));

    /* disable pcs lane if pcs lane not in rset */
    if (pcs_lane_enable) {
        phy_copy.access.lane_mask = 1 << start_lane;
        PHYMOD_IF_ERR_RETURN
            (tbhmod_disable_set(&phy_copy.access));

        /* add the pcs disable SW WAR */
        PHYMOD_IF_ERR_RETURN
            (tbhmod_pcs_reset_sw_war(&phy->access));
    }

    /* if lane is not in reset, then reset the lane first */
    if (!lane_reset) {
        PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));
        PHYMOD_IF_ERR_RETURN
            (blackhawk_lane_soft_reset(&phy_copy.access, 1));
    }

    phy_copy.access.lane_mask = 1 << start_lane;
    PHYMOD_IF_ERR_RETURN(
      tbhmod_fec_bypass_indication_set(&phy_copy.access, enable));

    /* release the lane soft reset bit */
    if (!lane_reset) {
        PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));
        PHYMOD_IF_ERR_RETURN
            (blackhawk_lane_soft_reset(&phy_copy.access, 0));
    }

    /* re-enable pcs lane if pcs lane not in rset */
    if (pcs_lane_enable) {
        phy_copy.access.lane_mask = 1 << start_lane;
        PHYMOD_IF_ERR_RETURN
            (tbhmod_enable_set(&phy_copy.access));
    }

    return PHYMOD_E_NONE;
}

int tscbh_phy_fec_bypass_indication_get(const phymod_phy_access_t* phy,
                                        uint32_t *enable)
{
    int start_lane, num_lane;
    phymod_phy_access_t phy_copy;

    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));

    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(&phy->access, &start_lane, &num_lane));

    phy_copy.access.lane_mask = 1 << start_lane;

    PHYMOD_IF_ERR_RETURN(
      tbhmod_fec_bypass_indication_get(&phy_copy.access, enable));

    return PHYMOD_E_NONE;
}

int tscbh_phy_rs_fec_rxp_get(const phymod_phy_access_t* phy, uint32_t* hi_ser_lh, uint32_t* hi_ser_live)
{
    int start_lane, num_lane;
    phymod_phy_access_t phy_copy;

    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));

    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(&phy->access, &start_lane, &num_lane));

    phy_copy.access.lane_mask = 1 << start_lane;

    PHYMOD_IF_ERR_RETURN
        (tbhmod_rs_fec_hi_ser_get(&phy_copy.access, hi_ser_lh, hi_ser_live));

    return PHYMOD_E_NONE;
}

/*
 * Enables/Disables 100G 4 lane FEC override
 *
 * value: 0x1    Enable for NOFEC 100G AN
 * register 0xc05e sc_x4_control_SW_spare1 lane 0 copy  is used
 * for this purpose
 */
int tscbh_phy_fec_override_set(const phymod_phy_access_t* phy, uint32_t enable)
{
    phymod_phy_access_t phy_copy;
    uint32_t tvco_pll_div;

    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));
    phy_copy.access.lane_mask = 1;

    if ((enable ==  0) || (enable == 1)) {
         PHYMOD_IF_ERR_RETURN(tbhmod_fec_override_set(&phy_copy.access, enable));
    } else {
     PHYMOD_DEBUG_ERROR(("ERROR :: Supported input values: 1 to set FEC override, 0 to disable FEC override\n"));
    }

    /* need to load the speed id table properly*/
    /* Get TVCO because it's not allowed to change during speed set */
    phy_copy.access.pll_idx = phy->access.tvco_pll_index;

    PHYMOD_IF_ERR_RETURN
        (blackhawk_tsc_INTERNAL_read_pll_div(&phy_copy.access, &tvco_pll_div));

    /* based on the current TVCO PLL div, decide which copy of speed id entry to load */
    /* first set the lane mask to be 0x1 */
    phy_copy.access.lane_mask = 1 << 0;
    if ((tvco_pll_div == TBHMOD_PLL_MODE_DIV_170) || (tvco_pll_div == TBHMOD_PLL_MODE_DIV_85))  {
        /* then load 26G TVCO speed id entry for the 100G 4 lane speed */
        if (enable) {
            PHYMOD_IF_ERR_RETURN
                (phymod_mem_write(&phy_copy.access, phymodMemSpeedIdTable, SPEED_ID_INDEX_100G_4_LANE_CL73_KR4, &spd_id_entry_100g_4lane_no_fec_26[0]));
            PHYMOD_IF_ERR_RETURN
                (phymod_mem_write(&phy_copy.access, phymodMemSpeedIdTable, SPEED_ID_INDEX_100G_4_LANE_CL73_CR4, &spd_id_entry_100g_4lane_no_fec_26[0]));
        } else {
            PHYMOD_IF_ERR_RETURN
                (phymod_mem_write(&phy_copy.access, phymodMemSpeedIdTable, SPEED_ID_INDEX_100G_4_LANE_CL73_KR4, &spd_id_entry_26[SPEED_ID_INDEX_100G_4_LANE_CL73_KR4][0]));
            PHYMOD_IF_ERR_RETURN
                (phymod_mem_write(&phy_copy.access, phymodMemSpeedIdTable, SPEED_ID_INDEX_100G_4_LANE_CL73_CR4, &spd_id_entry_26[SPEED_ID_INDEX_100G_4_LANE_CL73_CR4][0]));
        }
    } else if ((tvco_pll_div == (uint32_t)TBHMOD_PLL_MODE_DIV_165) || (tvco_pll_div == (uint32_t)TBHMOD_PLL_MODE_DIV_82P5)) {
        if (enable) {
            PHYMOD_IF_ERR_RETURN
                (phymod_mem_write(&phy_copy.access, phymodMemSpeedIdTable, SPEED_ID_INDEX_100G_4_LANE_CL73_KR4, &spd_id_entry_100g_4lane_no_fec_25[0]));
            PHYMOD_IF_ERR_RETURN
                (phymod_mem_write(&phy_copy.access, phymodMemSpeedIdTable, SPEED_ID_INDEX_100G_4_LANE_CL73_CR4, &spd_id_entry_100g_4lane_no_fec_25[0]));
        } else {
            PHYMOD_IF_ERR_RETURN
                (phymod_mem_write(&phy_copy.access, phymodMemSpeedIdTable, SPEED_ID_INDEX_100G_4_LANE_CL73_KR4, &spd_id_entry_25[SPEED_ID_INDEX_100G_4_LANE_CL73_KR4][0]));
            PHYMOD_IF_ERR_RETURN
                (phymod_mem_write(&phy_copy.access, phymodMemSpeedIdTable, SPEED_ID_INDEX_100G_4_LANE_CL73_CR4, &spd_id_entry_25[SPEED_ID_INDEX_100G_4_LANE_CL73_CR4][0]));
        }
    }
    return PHYMOD_E_NONE;
}

int tscbh_phy_fec_override_get(const phymod_phy_access_t* phy, uint32_t* enable)
{
    phymod_phy_access_t phy_copy;

    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));
    phy_copy.access.lane_mask = 1;
    PHYMOD_IF_ERR_RETURN(tbhmod_fec_override_get(&phy_copy.access, enable));

    return PHYMOD_E_NONE;

}

int tscbh_phy_interrupt_enable_set(const phymod_phy_access_t* phy,
                                   phymod_interrupt_type_t intr_type,
                                   uint32_t enable)
{
    phymod_phy_access_t phy_copy;

    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));
    PHYMOD_IF_ERR_RETURN(tbhmod_ecc_error_intr_enable_set(&phy_copy.access,
                                                          intr_type,  enable));
    return PHYMOD_E_NONE;
}



int tscbh_phy_interrupt_enable_get(const phymod_phy_access_t* phy,
                                   phymod_interrupt_type_t intr_type,
                                   uint32_t* enable)
{
    phymod_phy_access_t phy_copy;

    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));
    PHYMOD_IF_ERR_RETURN(tbhmod_ecc_error_intr_enable_get(&phy_copy.access,
                                                          intr_type, enable));
    return PHYMOD_E_NONE;
}

int tscbh_phy_fec_three_cw_bad_state_get(const phymod_phy_access_t* phy, uint32_t *state)
{
    int start_lane, num_lane;
    phymod_phy_access_t phy_copy;

    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));

    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(&phy->access, &start_lane, &num_lane));

    phy_copy.access.lane_mask = 1 << start_lane;

    PHYMOD_IF_ERR_RETURN
        (tbhmod_fec_three_cw_bad_state_get(&phy_copy.access, state));

    return PHYMOD_E_NONE;
}
