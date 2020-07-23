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
#include <phymod/chip/tscbh_gen3.h>
#include <phymod/chip/bcmi_tscbh_gen3_xgxs_defs.h>
#include "tscbh_gen3/tier1/tbhmod_gen3.h"
#include "tscbh_gen3/tier1/tbhmod_gen3_sc_lkup_table.h"
#include "tscbh_gen3/tier1/tbhmod_gen3_1588_lkup_table.h"
#include "blackhawk7_v1l8p1/tier1/blackhawk7_v1l8p1_cfg_seq.h"
#include "blackhawk7_v1l8p1/tier1/blackhawk7_v1l8p1_enum.h"
#include "blackhawk7_v1l8p1/tier1/blackhawk7_v1l8p1_common.h"
#include "blackhawk7_v1l8p1/tier1/blackhawk7_v1l8p1_interface.h"
#include "blackhawk7_v1l8p1/tier1/blackhawk7_v1l8p1_dependencies.h"
#include "blackhawk7_v1l8p1/tier1/blackhawk7_v1l8p1_internal.h"
#include "blackhawk7_v1l8p1/tier1/public/blackhawk7_api_uc_vars_rdwr_defns_public.h"
#include "blackhawk7_v1l8p1/tier1/blackhawk7_v1l8p1_access.h"


extern unsigned char blackhawk7_v1l8p1_ucode[];
extern unsigned int blackhawk7_v1l8p1_ucode_len;
extern unsigned short blackhawk7_v1l8p1_ucode_crc;
extern unsigned short blackhawk7_v1l8p1_ucode_stack_size;

extern uint32_t tscbh_gen3_spd_id_entry_26[TSCBH_GEN3_SPEED_ID_TABLE_SIZE][TSCBH_GEN3_SPEED_ID_ENTRY_SIZE];
extern uint32_t tscbh_gen3_spd_id_entry_25[TSCBH_GEN3_SPEED_ID_TABLE_SIZE][TSCBH_GEN3_SPEED_ID_ENTRY_SIZE];
extern uint32_t tscbh_gen3_spd_id_entry_20[TSCBH_GEN3_SPEED_ID_TABLE_SIZE][TSCBH_GEN3_SPEED_ID_ENTRY_SIZE];
extern uint32_t tscbh_gen3_am_table_entry[TSCBH_GEN3_AM_TABLE_SIZE][TSCBH_GEN3_AM_ENTRY_SIZE];
extern uint32_t tscbh_gen3_um_table_entry[TSCBH_GEN3_UM_TABLE_SIZE][TSCBH_GEN3_UM_ENTRY_SIZE];
extern uint32_t tscbh_gen3_speed_priority_mapping_table[TSCBH_GEN3_SPEED_PRIORITY_MAPPING_TABLE_SIZE][TSCBH_GEN3_SPEED_PRIORITY_MAPPING_ENTRY_SIZE];
extern uint32_t tscbh_gen3_spd_id_entry_26_gsh[TSCBH_GEN3_SPEED_ID_TABLE_SIZE][TSCBH_GEN3_SPEED_ID_ENTRY_SIZE];
extern uint32_t tscbh_gen3_spd_id_entry_25_gsh[TSCBH_GEN3_SPEED_ID_TABLE_SIZE][TSCBH_GEN3_SPEED_ID_ENTRY_SIZE];
extern uint32_t tscbh_gen3_spd_id_entry_20_gsh[TSCBH_GEN3_SPEED_ID_TABLE_SIZE][TSCBH_GEN3_SPEED_ID_ENTRY_SIZE];

extern const tbhmod_gen3_ts_table_entry tbhmod_gen3_ts_table_tx_sop[TBHMOD_GEN3_SPEED_MODE_COUNT];
extern const tbhmod_gen3_ts_table_entry tbhmod_gen3_ts_table_rx_sop[TBHMOD_GEN3_SPEED_MODE_COUNT];
extern const tbhmod_gen3_ts_table_entry tbhmod_gen3_ts_table_tx_sfd[TBHMOD_GEN3_SPEED_MODE_COUNT];
extern const tbhmod_gen3_ts_table_entry tbhmod_gen3_ts_table_rx_sfd[TBHMOD_GEN3_SPEED_MODE_COUNT];

#define TSCBH_GEN3_SERDES_ID           0x27 /* 0x9008 Main0_serdesID - Serdes ID Register */
#define TSCBH_GEN3_PMD_OSR_2P5         0x3


int tscbh_gen3_core_identify(const phymod_core_access_t* core, uint32_t core_id, uint32_t* is_identified)
{
    PHYID2r_t id2;
    PHYID3r_t id3;
    MAIN0_SERDESIDr_t serdesid;
    int ioerr = 0;

    *is_identified = 0;
    ioerr += READ_PHYID2r(&core->access, &id2);
    ioerr += READ_PHYID3r(&core->access, &id3);

    if (PHYID2r_REGID1f_GET(id2) == TSCBH_GEN3_ID0 &&
       (PHYID3r_REGID2f_GET(id3) == TSCBH_GEN3_ID1)) {
        /* PHY IDs match - now check PCS model */
        ioerr += READ_MAIN0_SERDESIDr(&core->access, &serdesid);
        if ( (MAIN0_SERDESIDr_MODEL_NUMBERf_GET(serdesid)) == TSCBH_GEN3_SERDES_ID)  {
            *is_identified = 1;
        }
    }
    return ioerr ? PHYMOD_E_IO : PHYMOD_E_NONE;
}


int tscbh_gen3_core_info_get(const phymod_core_access_t* core, phymod_core_info_t* info)
{
    int rv = 0;
    MAIN0_SERDESIDr_t serdes_id;
    char core_name[15] = "TscbhGen3";
    PHYID2r_t id2;
    PHYID3r_t id3;
    const phymod_access_t *pm_acc = &core->access;

    rv = READ_MAIN0_SERDESIDr(&core->access, &serdes_id);

    info->serdes_id = MAIN0_SERDESIDr_GET(serdes_id);
    PHYMOD_IF_ERR_RETURN
        (phymod_core_name_get(core, MAIN0_SERDESIDr_GET(serdes_id), core_name, info));
    info->serdes_id = MAIN0_SERDESIDr_GET(serdes_id);
    info->core_version = phymodCoreVersionTscbhGen3;

    PHYMOD_IF_ERR_RETURN(READ_PHYID2r(pm_acc, &id2));
    PHYMOD_IF_ERR_RETURN(READ_PHYID3r(pm_acc, &id3));

    info->phy_id0 = (uint16_t) id2.v[0];
    info->phy_id1 = (uint16_t) id3.v[0];

    return rv;
}

int tscbh_gen3_phy_firmware_lane_config_get(const phymod_phy_access_t* phy, phymod_firmware_lane_config_t* fw_config)
{
    struct blackhawk7_v1l8p1_uc_lane_config_st lane_config;
    phymod_phy_access_t pm_phy_copy;

    PHYMOD_MEMCPY(&pm_phy_copy, phy, sizeof(pm_phy_copy));

    PHYMOD_MEMSET(&lane_config, 0x0, sizeof(lane_config));
    PHYMOD_MEMSET(fw_config, 0, sizeof(*fw_config));

    PHYMOD_IF_ERR_RETURN
        (blackhawk7_v1l8p1_get_uc_lane_cfg(&pm_phy_copy.access, &lane_config));

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
    fw_config->ForceExtenedReach     = lane_config.field.force_er;
    fw_config->ForceNormalReach      = lane_config.field.force_nr;
    fw_config->LpPrecoderEnabled     = lane_config.field.lp_has_prec_en;
    fw_config->ForcePAM4Mode         = lane_config.field.force_pam4_mode;
    fw_config->ForceNRZMode          = lane_config.field.force_nrz_mode;

    return PHYMOD_E_NONE;
}


int tscbh_gen3_phy_tx_lane_control_set(const phymod_phy_access_t* phy, phymod_phy_tx_lane_control_t tx_control)
{
    phymod_firmware_lane_config_t fw_lane_config;
    phymod_phy_access_t phy_copy;

    PHYMOD_IF_ERR_RETURN (tscbh_gen3_phy_firmware_lane_config_get(phy, &fw_lane_config));
    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));

    switch (tx_control) {
        case phymodTxElectricalIdleEnable:
            if (fw_lane_config.LaneConfigFromPCS == 0) {
                PHYMOD_IF_ERR_RETURN(blackhawk7_v1l8p1_electrical_idle_set(&phy_copy.access, 1));
            } else {
                return PHYMOD_E_PARAM;
            }
            break;
        case phymodTxElectricalIdleDisable:
            PHYMOD_IF_ERR_RETURN(blackhawk7_v1l8p1_electrical_idle_set(&phy_copy.access, 0));
            break;
        case phymodTxSquelchOn:
            PHYMOD_IF_ERR_RETURN(blackhawk7_v1l8p1_tx_disable(&phy_copy.access, 1));
            break;
        case phymodTxSquelchOff:
            PHYMOD_IF_ERR_RETURN(blackhawk7_v1l8p1_tx_disable(&phy_copy.access, 0));
            break;
        default:
            return PHYMOD_E_PARAM;
    }
    return PHYMOD_E_NONE;
}


int tscbh_gen3_phy_tx_lane_control_get(const phymod_phy_access_t* phy, phymod_phy_tx_lane_control_t *tx_control)
{
    uint8_t tx_disable;
    uint32_t lb_enable;
    phymod_phy_access_t pm_phy_copy;
    int start_lane, num_lane;

    PHYMOD_MEMCPY(&pm_phy_copy, phy, sizeof(pm_phy_copy));
    /* next program the tx fir taps and driver current based on the input */
    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(&phy->access, &start_lane, &num_lane));

    pm_phy_copy.access.lane_mask = 0x1 << start_lane;

    PHYMOD_IF_ERR_RETURN(blackhawk7_v1l8p1_tx_disable_get(&pm_phy_copy.access, &tx_disable));

    /* next check if PMD loopback is on */
    if (tx_disable) {
        PHYMOD_IF_ERR_RETURN(blackhawk7_v1l8p1_dig_lpbk_get(&pm_phy_copy.access, &lb_enable));
        if (lb_enable) tx_disable = 0;
    }

    if (tx_disable) {
        *tx_control = phymodTxSquelchOn;
    } else {
        *tx_control = phymodTxSquelchOff;
    }

    return PHYMOD_E_NONE;
}

/*Rx control*/
int tscbh_gen3_phy_rx_lane_control_set(const phymod_phy_access_t* phy, phymod_phy_rx_lane_control_t rx_control)
{
    phymod_phy_access_t pm_phy_copy;

    PHYMOD_MEMCPY(&pm_phy_copy, phy, sizeof(pm_phy_copy));

    switch (rx_control) {
        case phymodRxSquelchOn:
            PHYMOD_IF_ERR_RETURN(blackhawk7_v1l8p1_pmd_force_signal_detect(&pm_phy_copy.access, 1, 0));
            break;
        case phymodRxSquelchOff:
            PHYMOD_IF_ERR_RETURN(blackhawk7_v1l8p1_pmd_force_signal_detect(&pm_phy_copy.access, 0, 0));
            break;
        default:
            return PHYMOD_E_PARAM;
    }

    return PHYMOD_E_NONE;
}

int tscbh_gen3_phy_rx_lane_control_get(const phymod_phy_access_t* phy, phymod_phy_rx_lane_control_t* rx_control)
{
    int rx_squelch_enable;
    uint32_t lb_enable;
    uint8_t force_en, force_val;
    phymod_phy_access_t pm_phy_copy;

    PHYMOD_MEMCPY(&pm_phy_copy, phy, sizeof(pm_phy_copy));

    /* first get the force enabled bit and forced value */
    PHYMOD_IF_ERR_RETURN(blackhawk7_v1l8p1_pmd_force_signal_detect_get(&pm_phy_copy.access, &force_en, &force_val));

    if (force_en & (!force_val)) {
        rx_squelch_enable = 1;
    } else {
        rx_squelch_enable = 0;
    }

    /* next check if PMD loopback is on */
    if (rx_squelch_enable) {
        PHYMOD_IF_ERR_RETURN(blackhawk7_v1l8p1_dig_lpbk_get(&pm_phy_copy.access, &lb_enable));
        if (lb_enable) rx_squelch_enable = 0;
    }
    if(rx_squelch_enable) {
        *rx_control = phymodRxSquelchOn;
    } else {
        *rx_control = phymodRxSquelchOff;
    }
    return PHYMOD_E_NONE;
}


STATIC
int _tscbh_gen3_core_firmware_load(const phymod_core_access_t* core, const phymod_core_init_config_t* init_config)
{
    phymod_core_access_t  core_copy;
    phymod_phy_access_t phy_access;
    unsigned int blackhawk_ucode_len;
    unsigned char *blackhawk_ucode;

    PHYMOD_MEMCPY(&core_copy, core, sizeof(core_copy));
    TSCBH_GEN3_CORE_TO_PHY_ACCESS(&phy_access, core);

    phy_access.access.lane_mask = 0x1;
    blackhawk_ucode = blackhawk7_v1l8p1_ucode;
    blackhawk_ucode_len = blackhawk7_v1l8p1_ucode_len;

    switch(init_config->firmware_load_method){
    case phymodFirmwareLoadMethodInternal:
        PHYMOD_IF_ERR_RETURN(blackhawk7_v1l8p1_ucode_load(&core_copy.access, blackhawk_ucode, blackhawk_ucode_len));
        break;
    case phymodFirmwareLoadMethodExternal:
        PHYMOD_NULL_CHECK(init_config->firmware_loader);
        PHYMOD_IF_ERR_RETURN(blackhawk7_v1l8p1_ucode_init(&core_copy.access));
        PHYMOD_IF_ERR_RETURN
            (blackhawk7_v1l8p1_pram_firmware_enable(&core_copy.access, 1, 0));
        PHYMOD_IF_ERR_RETURN(init_config->firmware_loader(core, blackhawk_ucode_len, blackhawk_ucode));
        PHYMOD_IF_ERR_RETURN
            (blackhawk7_v1l8p1_pram_firmware_enable(&core_copy.access, 0, 0));
        break;
    case phymodFirmwareLoadMethodNone:
        break;
    default:
        PHYMOD_RETURN_WITH_ERR(PHYMOD_E_CONFIG, (_PHYMOD_MSG("illegal fw load method %u"), init_config->firmware_load_method));
    }

    return PHYMOD_E_NONE;
}


static int _tscbh_gen3_100g_2lane_sw_workaround(const phymod_phy_access_t* phy)
{
    phymod_phy_access_t phy_copy;
    int start_lane, num_lane;

    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(&phy->access, &start_lane, &num_lane));
    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));
    /* enable PMD lane override */
    phy_copy.access.lane_mask = 1 << start_lane;

    /* Hold the pcs lane reset */
    PHYMOD_IF_ERR_RETURN
        (tbhmod_gen3_disable_set(&phy_copy.access));

    /* write this port forced speed id entry , always use 0, which is 10G SFI */
    PHYMOD_IF_ERR_RETURN
      (tbhmod_gen3_set_sc_speed(&phy_copy.access, 0, 0));

    /* next update the port_mode */
    phy_copy.access.lane_mask = 1 << start_lane;
    PHYMOD_IF_ERR_RETURN
        (tbhmod_gen3_update_port_mode(&phy_copy.access));

    /* add rx lock override */
    PHYMOD_IF_ERR_RETURN
        (tbhmod_gen3_pmd_rx_lock_override_enable(&phy_copy.access, 1));
    /* clear state machine state */
    PHYMOD_IF_ERR_RETURN
        (tbhmod_gen3_read_sc_fsm_status(&phy_copy.access));
    PHYMOD_IF_ERR_RETURN
        (tbhmod_gen3_read_sc_done(&phy_copy.access));
    PHYMOD_IF_ERR_RETURN
        (tbhmod_gen3_enable_set(&phy_copy.access));
    PHYMOD_IF_ERR_RETURN
        (tbhmod_gen3_polling_for_sc_done(&phy_copy.access));
    /* disable pcs again */
    PHYMOD_IF_ERR_RETURN
        (tbhmod_gen3_disable_set(&phy_copy.access));
    PHYMOD_USLEEP(10);
    /* disable rx lock override */
    PHYMOD_IF_ERR_RETURN
        (tbhmod_gen3_pmd_rx_lock_override_enable(&phy_copy.access, 0));

    return PHYMOD_E_NONE;
}

int tscbh_gen3_phy_tx_set(const phymod_phy_access_t* phy, const phymod_tx_t* tx)
{
    phymod_phy_access_t phy_copy;
    int start_lane, num_lane, i, port_start_lane, port_num_lane, rv = PHYMOD_E_NONE;
    enum blackhawk7_v1l8p1_txfir_tap_enable_enum enable_taps = BLACKHAWK7_V1L8P1_NRZ_6TAP;
    uint32_t lane_reset, pcs_lane_enable, port_lane_mask = 0;

    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(&phy->access, &start_lane, &num_lane));
    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));

    /*get the start lane of the port lane mask */
    PHYMOD_IF_ERR_RETURN
        (tbhmod_gen3_port_start_lane_get(&phy_copy.access, &port_start_lane, &port_num_lane));

    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_mask_get(port_start_lane, port_num_lane, &port_lane_mask));

    phy_copy.access.lane_mask = 1 << port_start_lane;

    /*next check if PCS lane is in reset */
    PHYMOD_IF_ERR_RETURN
        (tbhmod_gen3_enable_get(&phy_copy.access, &pcs_lane_enable));

    /*first check if lane is in reset */
    phy_copy.access.lane_mask = 1 << start_lane;
    PHYMOD_IF_ERR_RETURN
        (blackhawk7_v1l8p1_lane_soft_reset_get(&phy_copy.access, &lane_reset));

    /* disable pcs lane if pcs lane not in rset */
    if (pcs_lane_enable) {
        phy_copy.access.lane_mask = 1 << port_start_lane;
        PHYMOD_IF_ERR_RETURN
            (tbhmod_gen3_disable_set(&phy_copy.access));
        /* add the pcs disable SW WAR */
        phy_copy.access.lane_mask = port_lane_mask;
        PHYMOD_IF_ERR_RETURN
            (tbhmod_gen3_pcs_reset_sw_war(&phy_copy.access));
    }
    if (!lane_reset) {
        PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));
        PHYMOD_IF_ERR_RETURN
            (blackhawk7_v1l8p1_lane_soft_reset(&phy_copy.access, 1));
    }

    for (i = 0; i < num_lane; i++) {
        if (!PHYMOD_LANEPBMP_MEMBER(phy->access.lane_mask, start_lane + i)) {
            continue;
        }
        phy_copy.access.lane_mask = 1 << (start_lane + i);
        /*next check 3 tap mode or 6 tap mode */
        if (tx->tap_mode == phymodTxTapMode3Tap) {
            if (tx->sig_method == phymodSignallingMethodNRZ) {
                enable_taps= BLACKHAWK7_V1L8P1_NRZ_LP_3TAP;
            } else {
                enable_taps= BLACKHAWK7_V1L8P1_PAM4_LP_3TAP;
            }
            rv |= blackhawk7_v1l8p1_apply_txfir_cfg(&phy_copy.access,
                                                 enable_taps,
                                                 0,
                                                 tx->pre,
                                                 tx->main,
                                                 tx->post,
                                                 0,
                                                 0);
        } else {
            if (tx->sig_method == phymodSignallingMethodNRZ) {
                enable_taps= BLACKHAWK7_V1L8P1_NRZ_6TAP;
            } else {
                enable_taps= BLACKHAWK7_V1L8P1_PAM4_6TAP;
            }
            rv |= blackhawk7_v1l8p1_apply_txfir_cfg(&phy_copy.access,
                                                 enable_taps,
                                                 tx->pre2,
                                                 tx->pre,
                                                 tx->main,
                                                 tx->post,
                                                 tx->post2,
                                                 tx->post3);
        }
    }

    if (!lane_reset) {
        PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));
        PHYMOD_IF_ERR_RETURN
            (blackhawk7_v1l8p1_lane_soft_reset(&phy_copy.access, 0));
    }

    /* re-enable pcs lane if pcs lane not in rset */
    if (pcs_lane_enable) {
        phy_copy.access.lane_mask = 1 << port_start_lane;
        PHYMOD_IF_ERR_RETURN
            (tbhmod_gen3_enable_set(&phy_copy.access));
    }
    return rv;
}

int tscbh_gen3_phy_media_type_tx_get(const phymod_phy_access_t* phy, phymod_media_typed_t media, phymod_tx_t* tx)
{
    return PHYMOD_E_NONE;
}

/*
 * set lane swapping for core
 */

int tscbh_gen3_core_lane_map_set(const phymod_core_access_t* core, const phymod_lane_map_t* lane_map)
{
    phymod_core_access_t  core_copy;
    uint32_t lane, pcs_tx_swap = 0, pcs_rx_swap = 0;
    uint8_t pmd_tx_addr[8], pmd_rx_addr[8];

    if (lane_map->num_of_lanes != TSCBH_GEN3_NOF_LANES_IN_CORE){
        return PHYMOD_E_CONFIG;
    }

    PHYMOD_MEMCPY(&core_copy, core, sizeof(core_copy));
    core_copy.access.lane_mask = 0x1;

    for (lane = 0; lane < TSCBH_GEN3_NOF_LANES_IN_CORE; lane++){
        if ((lane_map->lane_map_tx[lane] >= TSCBH_GEN3_NOF_LANES_IN_CORE)||
             (lane_map->lane_map_rx[lane] >= TSCBH_GEN3_NOF_LANES_IN_CORE)){
            return PHYMOD_E_CONFIG;
        }
        /*encode each lane as four bits*/
        pcs_tx_swap += lane_map->lane_map_tx[lane]<<(lane*4);
        pcs_rx_swap += lane_map->lane_map_rx[lane]<<(lane*4);
    }
    /* PMD lane addr is based on PCS logical to physical mapping*/
    for (lane = 0; lane < TSCBH_GEN3_NOF_LANES_IN_CORE; lane++){
        pmd_tx_addr[((pcs_tx_swap >> (lane*4)) & 0xf)] = lane;
        pmd_rx_addr[((pcs_rx_swap >> (lane*4)) & 0xf)] = lane;
    }

    PHYMOD_IF_ERR_RETURN
        (tbhmod_gen3_pcs_tx_lane_swap(&core_copy.access, pcs_tx_swap));
    PHYMOD_IF_ERR_RETURN
        (tbhmod_gen3_pcs_rx_lane_swap(&core_copy.access, pcs_rx_swap));

    COMPILER_REFERENCE(pmd_tx_addr);
    COMPILER_REFERENCE(pmd_rx_addr);
    PHYMOD_IF_ERR_RETURN
        (blackhawk7_v1l8p1_map_lanes(&core_copy.access, TSCBH_GEN3_NOF_LANES_IN_CORE, pmd_tx_addr, pmd_rx_addr));

    return PHYMOD_E_NONE;
}

int tscbh_gen3_core_lane_map_get(const phymod_core_access_t* core, phymod_lane_map_t* lane_map) 
{
    phymod_core_access_t  core_copy;
    uint32_t tx_lane_map, rx_lane_map;
    int i = 0;

    PHYMOD_MEMCPY(&core_copy, core, sizeof(core_copy));
    core_copy.access.lane_mask = 0x1;

    PHYMOD_IF_ERR_RETURN
        (blackhawk7_v1l8p1_pmd_lane_map_get(&core_copy.access, &tx_lane_map, &rx_lane_map));

    lane_map->num_of_lanes = TSCBH_GEN3_NOF_LANES_IN_CORE;

    /*next get the lane map into serdes spi format */
    for (i = 0; i < TSCBH_GEN3_NOF_LANES_IN_CORE; i++) {
        lane_map->lane_map_tx[tx_lane_map >> (4 * i) & 0xf] = i;
        lane_map->lane_map_rx[rx_lane_map >> (4 * i) & 0xf] = i;
    }

    return PHYMOD_E_NONE;
}

static int _tscbh_gen3_phy_firmware_lane_config_set(const phymod_phy_access_t* phy, phymod_firmware_lane_config_t fw_config)
{
    uint32_t is_warm_boot;
    struct blackhawk7_v1l8p1_uc_lane_config_st serdes_firmware_config;
    phymod_phy_access_t phy_copy;
    int start_lane, num_lane, i;

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
        serdes_firmware_config.field.force_er               = fw_config.ForceExtenedReach;
        serdes_firmware_config.field.force_nr               = fw_config.ForceNormalReach;
        serdes_firmware_config.field.force_nrz_mode         = fw_config.ForceNRZMode;
        serdes_firmware_config.field.force_pam4_mode        = fw_config.ForcePAM4Mode;
        serdes_firmware_config.field.lp_has_prec_en         = fw_config.LpPrecoderEnabled;

        PHYMOD_IF_ERR_RETURN(PHYMOD_IS_WRITE_DISABLED(&phy->access, &is_warm_boot));

        if (!is_warm_boot) {
            PHYMOD_IF_ERR_RETURN(blackhawk7_v1l8p1_set_uc_lane_cfg(&phy_copy.access, serdes_firmware_config));
        }
    }

    return PHYMOD_E_NONE;
}

int tscbh_gen3_phy_firmware_lane_config_set(const phymod_phy_access_t* phy, phymod_firmware_lane_config_t fw_config)
{
    phymod_phy_access_t phy_copy;
    uint32_t lane_reset, pcs_lane_enable, port_lane_mask = 0;
    int start_lane, num_lane, port_start_lane, port_num_lane;

    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));
    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(&phy->access, &start_lane, &num_lane));

    /*get the start lane of the port lane mask */
    PHYMOD_IF_ERR_RETURN
        (tbhmod_gen3_port_start_lane_get(&phy_copy.access, &port_start_lane, &port_num_lane));

    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_mask_get(port_start_lane, port_num_lane, &port_lane_mask));

    /*first check if lane is in reset */
    PHYMOD_IF_ERR_RETURN
        (blackhawk7_v1l8p1_lane_soft_reset_get(&phy_copy.access, &lane_reset));

    /*next check if PCS lane is in reset */
    phy_copy.access.lane_mask = 1 << port_start_lane;
    PHYMOD_IF_ERR_RETURN
        (tbhmod_gen3_enable_get(&phy_copy.access, &pcs_lane_enable));

    /* disable pcs lane if pcs lane not in rset */
    if (pcs_lane_enable) {
        PHYMOD_IF_ERR_RETURN
            (tbhmod_gen3_disable_set(&phy_copy.access));
        /* add the pcs disable SW WAR */
        phy_copy.access.lane_mask = port_lane_mask;
        PHYMOD_IF_ERR_RETURN
            (tbhmod_gen3_pcs_reset_sw_war(&phy_copy.access));
    }

    /* if lane is not in reset, then reset the lane first */
    if (!lane_reset) {
        PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));
        PHYMOD_IF_ERR_RETURN
            (blackhawk7_v1l8p1_lane_soft_reset(&phy_copy.access, 1));
    }

    PHYMOD_IF_ERR_RETURN
         (_tscbh_gen3_phy_firmware_lane_config_set(phy, fw_config));

    if (!lane_reset) {
        PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));
        PHYMOD_IF_ERR_RETURN
            (blackhawk7_v1l8p1_lane_soft_reset(&phy_copy.access, 0));
    }

    /* re-enable pcs lane if pcs lane not in rset */
    if (pcs_lane_enable) {
        phy_copy.access.lane_mask = 1 << port_start_lane;
        PHYMOD_IF_ERR_RETURN
            (tbhmod_gen3_enable_set(&phy_copy.access));
    }

    return PHYMOD_E_NONE;
}


int tscbh_gen3_phy_polarity_set(const phymod_phy_access_t* phy, const phymod_polarity_t* polarity)
{
    phymod_phy_access_t phy_copy;
    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));

    PHYMOD_IF_ERR_RETURN
        (blackhawk7_v1l8p1_tx_rx_polarity_set(&phy_copy.access, polarity->tx_polarity, polarity->rx_polarity));

    return PHYMOD_E_NONE;
}


int tscbh_gen3_phy_polarity_get(const phymod_phy_access_t* phy, phymod_polarity_t* polarity)
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
            (blackhawk7_v1l8p1_tx_rx_polarity_get(&phy_copy.access, &temp_pol.tx_polarity, &temp_pol.rx_polarity));
        polarity->tx_polarity |= ((temp_pol.tx_polarity & 0x1) << i);
        polarity->rx_polarity |= ((temp_pol.rx_polarity & 0x1) << i);
    }

    return PHYMOD_E_NONE;
}

int tscbh_gen3_port_enable_set(const phymod_phy_access_t* phy, uint32_t enable)
{
    phymod_phy_access_t phy_copy;
    uint32_t pcs_enable, lpbk_enable;
    int start_lane, num_lane, port_an_enable, port_enable;

    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));

    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(&phy->access, &start_lane, &num_lane));

    /* first read port an enable bit */
    PHYMOD_IF_ERR_RETURN
        (tbhmod_gen3_port_an_mode_enable_get(&phy_copy.access, &port_an_enable));

    /* next read current port enable bit */
    PHYMOD_IF_ERR_RETURN
        (tbhmod_gen3_port_enable_get(&phy_copy.access, &port_enable));

    /*next check if PCS lane is in reset */
    PHYMOD_IF_ERR_RETURN
        (tbhmod_gen3_enable_get(&phy_copy.access, &pcs_enable));

   /* Check if PMD loopback is enable */
    PHYMOD_IF_ERR_RETURN
        (tscbh_gen3_phy_loopback_get(phy, phymodLoopbackGlobalPMD, &lpbk_enable));

    if ((port_an_enable) || (port_enable && !pcs_enable)) {
        /* cuurent port is in An mode mode */
        if (enable == 1) {
            /* next release both tx/rx squelch */
            PHYMOD_IF_ERR_RETURN(blackhawk7_v1l8p1_tx_disable(&phy_copy.access, 0));
            PHYMOD_IF_ERR_RETURN(blackhawk7_v1l8p1_pmd_force_signal_detect(&phy_copy.access, 0, 0));
            phy_copy.access.lane_mask = 1 << start_lane;
        }
        PHYMOD_IF_ERR_RETURN
            (tbhmod_gen3_port_cl73_enable_set(&phy_copy.access, enable));
        /*check if enable ==0 */
        if (!enable) {
            /* next set both tx/rx squelch */
            PHYMOD_IF_ERR_RETURN(blackhawk7_v1l8p1_tx_disable(&phy_copy.access, 1));
            PHYMOD_IF_ERR_RETURN(blackhawk7_v1l8p1_pmd_force_signal_detect(&phy_copy.access, 1, 0));
        }

    } else {
        /* cuurent port is in forced speed mode */
        if (enable == 1) {
            /* next release both tx/rx squelch */
            PHYMOD_IF_ERR_RETURN(blackhawk7_v1l8p1_tx_disable(&phy_copy.access, 0));
            /*
             * PMD release (APIS - A006_02, ucode -D004_08) forces sigdet on when enabling
             * dig_Lpbk for BHK7, which causes link down when disabling then enabling the port.
             * To fix this issue, force sigdet on when port is enabled and PMD loopback is set.
             */
            if (lpbk_enable) {
                PHYMOD_IF_ERR_RETURN(blackhawk7_v1l8p1_pmd_force_signal_detect(&phy_copy.access, 1, 1));
            } else {
                PHYMOD_IF_ERR_RETURN(blackhawk7_v1l8p1_pmd_force_signal_detect(&phy_copy.access, 0, 0));
            }
            phy_copy.access.lane_mask = 1 << start_lane;
            /* enable speed control bit */
            PHYMOD_IF_ERR_RETURN
                (tbhmod_gen3_enable_set(&phy_copy.access));
        } else if (enable == 0) {
            /* disable speed control bit */
            PHYMOD_IF_ERR_RETURN
                (tbhmod_gen3_disable_set(&phy_copy.access));
            /* next set both tx/rx squelch */
            PHYMOD_IF_ERR_RETURN(blackhawk7_v1l8p1_tx_disable(&phy_copy.access, 1));
            PHYMOD_IF_ERR_RETURN(blackhawk7_v1l8p1_pmd_force_signal_detect(&phy_copy.access, 1, 0));
        }
    }

    /* next set port enable bit */
    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));
    PHYMOD_IF_ERR_RETURN
        (tbhmod_gen3_port_enable_set(&phy_copy.access, enable));


    return PHYMOD_E_NONE;
}

int tscbh_gen3_port_enable_get(const phymod_phy_access_t* phy, uint32_t* enable)
{
    phymod_phy_access_t phy_copy;
    int temp_enable;

    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));

    /* first read port an enable bit */
    PHYMOD_IF_ERR_RETURN
        (tbhmod_gen3_port_enable_get(&phy_copy.access, &temp_enable));

    *enable = (uint32_t) temp_enable;

    return PHYMOD_E_NONE;
}



int tscbh_gen3_phy_tx_get(const phymod_phy_access_t* phy, phymod_tx_t* tx)
{
    uint8_t pmd_tx_tap_mode;
    uint16_t tx_tap_nrz_mode = 0;
    int16_t val;
    phymod_phy_access_t phy_copy;
    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));

    /* read current tx tap mode */
    PHYMOD_IF_ERR_RETURN
        (blackhawk7_v1l8p1_tx_tap_mode_get(&phy_copy.access, &pmd_tx_tap_mode));

    /*read current tx NRZ mode control info */
    PHYMOD_IF_ERR_RETURN
        (blackhawk7_v1l8p1_tx_nrz_mode_get(&phy_copy.access, &tx_tap_nrz_mode));

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
            (blackhawk7_v1l8p1_read_tx_afe(&phy_copy.access, BLACKHAWK7_V1L8P1_TX_AFE_TAP0, &val));
            tx->pre = val;
        PHYMOD_IF_ERR_RETURN
            (blackhawk7_v1l8p1_read_tx_afe(&phy_copy.access, BLACKHAWK7_V1L8P1_TX_AFE_TAP1, &val));
            tx->main = val;
        PHYMOD_IF_ERR_RETURN
            (blackhawk7_v1l8p1_read_tx_afe(&phy_copy.access, BLACKHAWK7_V1L8P1_TX_AFE_TAP2, &val));
            tx->post = val;
            tx->pre2 = 0;
            tx->post2 = 0;
            tx->post3 = 0;
    } else {
        PHYMOD_IF_ERR_RETURN
            (blackhawk7_v1l8p1_read_tx_afe(&phy_copy.access, BLACKHAWK7_V1L8P1_TX_AFE_TAP0, &val));
            tx->pre2 = val;
        PHYMOD_IF_ERR_RETURN
            (blackhawk7_v1l8p1_read_tx_afe(&phy_copy.access, BLACKHAWK7_V1L8P1_TX_AFE_TAP1, &val));
            tx->pre = val;
        PHYMOD_IF_ERR_RETURN
            (blackhawk7_v1l8p1_read_tx_afe(&phy_copy.access, BLACKHAWK7_V1L8P1_TX_AFE_TAP2, &val));
            tx->main = val;
        PHYMOD_IF_ERR_RETURN
            (blackhawk7_v1l8p1_read_tx_afe(&phy_copy.access, BLACKHAWK7_V1L8P1_TX_AFE_TAP3, &val));
            tx->post = val;
        PHYMOD_IF_ERR_RETURN
            (blackhawk7_v1l8p1_read_tx_afe(&phy_copy.access, BLACKHAWK7_V1L8P1_TX_AFE_TAP4, &val));
            tx->post2 = val;
        PHYMOD_IF_ERR_RETURN
            (blackhawk7_v1l8p1_read_tx_afe(&phy_copy.access, BLACKHAWK7_V1L8P1_TX_AFE_TAP5, &val));
            tx->post3 = val;
    }

    return PHYMOD_E_NONE;
}

/* This function based on num_lane, data_rate and fec_type
 * assign force speed SW speed_id.
 */

STATIC
int _tscbh_gen3_phy_speed_id_set(int num_lane,
                            uint32_t data_rate,
                            phymod_fec_type_t fec_type,
                            tbhmod_gen3_spd_intfc_type_t* spd_intf)
{
    if (num_lane == 1) {
        switch (data_rate) {
            case 10000:
                if (fec_type == phymod_fec_None) {
                    *spd_intf = TBHMOD_GEN3_SPD_10G_IEEE_KR1;
                } else {
                    return PHYMOD_E_UNAVAIL;
                }
                break;
            case 20000:
                if (fec_type == phymod_fec_None) {
                    *spd_intf = TBHMOD_GEN3_SPD_20G_BRCM_KR1;
                } else {
                    return PHYMOD_E_UNAVAIL;
                }
                break;
            case 25000:
                if (fec_type == phymod_fec_None) {
                    *spd_intf = TBHMOD_GEN3_SPD_25G_BRCM_NO_FEC_KR1_CR1;
                } else if (fec_type == phymod_fec_CL91) {
                    *spd_intf = TBHMOD_GEN3_SPD_25G_BRCM_FEC_528_KR1_CR1;
                } else {
                    return PHYMOD_E_UNAVAIL;
                }
                break;
            case 50000:
                if (fec_type == phymod_fec_CL91) {
                    *spd_intf = TBHMOD_GEN3_SPD_50G_BRCM_FEC_528_CR1_KR1;
                } else if (fec_type == phymod_fec_RS544) {
                    *spd_intf = TBHMOD_GEN3_SPD_50G_IEEE_KR1_CR1;
                } else if (fec_type == phymod_fec_RS272) {
                    *spd_intf = TBHMOD_GEN3_SPD_50G_BRCM_FEC_272_CR1_KR1;
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
                    *spd_intf = TBHMOD_GEN3_SPD_40G_BRCM_CR2;
                } else {
                    return PHYMOD_E_UNAVAIL;
                }
                break;
            case 50000:
                if (fec_type == phymod_fec_None) {
                    *spd_intf = TBHMOD_GEN3_SPD_50G_BRCM_CR2_KR2_NO_FEC;
                } else if (fec_type == phymod_fec_CL91) {
                    *spd_intf = TBHMOD_GEN3_SPD_50G_BRCM_CR2_KR2_RS_FEC;
                } else if (fec_type == phymod_fec_RS544) {
                    *spd_intf = TBHMOD_GEN3_SPD_50G_BRCM_FEC_544_CR2_KR2;
                } else {
                    return PHYMOD_E_UNAVAIL;
                }
                break;
            case 100000:
                if (fec_type == phymod_fec_None) {
                    *spd_intf = TBHMOD_GEN3_SPD_100G_BRCM_NO_FEC_KR2_CR2;
                } else if (fec_type == phymod_fec_CL91) {
                    *spd_intf = TBHMOD_GEN3_SPD_100G_BRCM_FEC_528_KR2_CR2;
                } else if (fec_type == phymod_fec_RS544) {
                    *spd_intf = TBHMOD_GEN3_SPD_100G_IEEE_KR2_CR2;
                } else if (fec_type == phymod_fec_RS272) {
                    *spd_intf = TBHMOD_GEN3_SPD_100G_BRCM_FEC_272_CR2_KR2;
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
                    *spd_intf = TBHMOD_GEN3_SPD_40G_IEEE_KR4;
                } else {
                    return PHYMOD_E_UNAVAIL;
                }
                break;
            case 100000:
                if (fec_type == phymod_fec_None) {
                    *spd_intf = TBHMOD_GEN3_SPD_100G_BRCM_NO_FEC_X4;
                } else if (fec_type == phymod_fec_CL91) {
                    *spd_intf = TBHMOD_GEN3_SPD_100G_IEEE_KR4;
                } else if (fec_type == phymod_fec_RS544) {
                    *spd_intf = TBHMOD_GEN3_SPD_100G_BRCM_KR4_CR4;
                } else {
                    return PHYMOD_E_UNAVAIL;
                }
                break;
            case 200000:
                if (fec_type == phymod_fec_None) {
                    *spd_intf = TBHMOD_GEN3_SPD_200G_BRCM_NO_FEC_KR4_CR4;
                } else if (fec_type == phymod_fec_RS544) {
                    *spd_intf = TBHMOD_GEN3_SPD_200G_BRCM_KR4_CR4;
                } else if (fec_type == phymod_fec_RS544_2XN) {
                    *spd_intf = TBHMOD_GEN3_SPD_200G_IEEE_KR4_CR4;
                } else if (fec_type == phymod_fec_RS272) {
                    *spd_intf = TBHMOD_GEN3_SPD_200G_BRCM_FEC_272_N4;
                } else if (fec_type == phymod_fec_RS272_2XN) {
                    *spd_intf = TBHMOD_GEN3_SPD_200G_BRCM_FEC_272_CR4_KR4;
                } else {
                    return PHYMOD_E_UNAVAIL;
                }
                break;
            default:
                return PHYMOD_E_UNAVAIL;
        }
    } else if (num_lane == 8) {
        if (data_rate == 400000 && fec_type == phymod_fec_RS544_2XN) {
            *spd_intf = TBHMOD_GEN3_SPD_400G_BRCM_FEC_KR8_CR8;
        } else if (data_rate == 400000 && fec_type == phymod_fec_RS272_2XN) {
            *spd_intf = TBHMOD_GEN3_SPD_400G_BRCM_FEC_272_N8;
        } else if (data_rate == 200000 && fec_type == phymod_fec_RS544_2XN) {
            *spd_intf = TBHMOD_GEN3_SPD_200G_BRCM_FEC_544_CR8_KR8;
        } else {
            return PHYMOD_E_UNAVAIL;
        }
    } else {
        return PHYMOD_E_UNAVAIL;
    }

    return PHYMOD_E_NONE;
}

int tscbh_gen3_phy_speed_config_set(const phymod_phy_access_t* phy,
                                   const phymod_phy_speed_config_t* speed_config,
                                   const phymod_phy_pll_state_t* old_pll_state,
                                   phymod_phy_pll_state_t* new_pll_state)
{
    phymod_phy_access_t pm_phy_copy;
    uint32_t lane_mask_backup;
    uint32_t tvco_pll_div = TBHMOD_GEN3_PLL_MODE_DIV_170, request_pll_div = 0;
    int i, start_lane, num_lane, mapped_speed_id;
    phymod_firmware_lane_config_t firmware_lane_config;
    phymod_firmware_core_config_t firmware_core_config;
    tbhmod_gen3_spd_intfc_type_t spd_intf = 0;
    tbhmod_gen3_refclk_t ref_clk;
    uint32_t osr_2p5_is_required = 0;
    int port_enable;

    firmware_lane_config = speed_config->pmd_lane_config;
    /*first make sure that tvco pll index is valid */
    if (phy->access.tvco_pll_index > 0) {
        PHYMOD_DEBUG_ERROR(("Unsupported tvco index\n"));
        return PHYMOD_E_UNAVAIL;
    }

    PHYMOD_MEMSET(&firmware_core_config, 0x0, sizeof(firmware_core_config));

    /* Copy the PLL state */
    *new_pll_state = *old_pll_state;

    PHYMOD_MEMCPY(&pm_phy_copy, phy, sizeof(pm_phy_copy));
    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(&phy->access, &start_lane, &num_lane));
    lane_mask_backup = phy->access.lane_mask;

    /* get current port enable bit */
    PHYMOD_IF_ERR_RETURN
        (tbhmod_gen3_port_enable_get(&pm_phy_copy.access, &port_enable));

    /* then clear the port an mode enable bit */
    PHYMOD_IF_ERR_RETURN
        (tbhmod_gen3_port_an_mode_enable_set(&pm_phy_copy.access, 0));

    /* Hold the pcs lane reset */
    pm_phy_copy.access.lane_mask = 1 << start_lane;
    PHYMOD_IF_ERR_RETURN
        (tbhmod_gen3_disable_set(&pm_phy_copy.access));

    /* add the pcs disable SW WAR */
    PHYMOD_IF_ERR_RETURN
        (tbhmod_gen3_pcs_reset_sw_war(&phy->access));

    /* write this port forced speed id entry */
    PHYMOD_IF_ERR_RETURN
      (tbhmod_gen3_set_sc_speed(&pm_phy_copy.access, TSCBH_GEN3_FORCED_SPEED_ID_OFFSET + start_lane, 0));

    /*Hold the per lane PMD soft reset bit*/
    pm_phy_copy.access.lane_mask = lane_mask_backup;
    PHYMOD_IF_ERR_RETURN
        (blackhawk7_v1l8p1_lane_soft_reset(&pm_phy_copy.access, 1));

    /*only update the port mode for ethernet port */
    PHYMOD_IF_ERR_RETURN
        (tbhmod_gen3_update_port_mode(&pm_phy_copy.access));

    /*for ethernet speed mode config set */
    PHYMOD_IF_ERR_RETURN(_tscbh_gen3_phy_speed_id_set(num_lane, speed_config->data_rate,
                                                 speed_config->fec_type, &spd_intf));

    PHYMOD_IF_ERR_RETURN
        (tbhmod_gen3_get_mapped_speed(spd_intf, &mapped_speed_id));

    /* set the rs fec CW properly */
    if ((speed_config->fec_type == phymod_fec_RS544) ||
        (speed_config->fec_type == phymod_fec_RS544_2XN)) {
        if (start_lane < 4) {
            pm_phy_copy.access.lane_mask = 1 << 0;
        } else {
            pm_phy_copy.access.lane_mask = 1 << 4;
        }
        PHYMOD_IF_ERR_RETURN
            (tbhmod_gen3_rsfec_cw_type(&pm_phy_copy.access, 0, 0));
        /* need to set MPP1 for 8 lane port */
        if (num_lane == 8) {
            pm_phy_copy.access.lane_mask = 1 << 4;
            PHYMOD_IF_ERR_RETURN
                (tbhmod_gen3_rsfec_cw_type(&pm_phy_copy.access, 0, 0));
        }
    } else if ((speed_config->fec_type == phymod_fec_RS272) ||
               (speed_config->fec_type == phymod_fec_RS272_2XN)) {
        if (start_lane < 4) {
            pm_phy_copy.access.lane_mask = 1 << 0;
        } else {
            pm_phy_copy.access.lane_mask = 1 << 4;
        }
        PHYMOD_IF_ERR_RETURN
            (tbhmod_gen3_rsfec_cw_type(&pm_phy_copy.access, 1, 0));
        /* need to set MPP1 for 8 lane port */
        if (num_lane == 8) {
            pm_phy_copy.access.lane_mask = 1 << 4;
            PHYMOD_IF_ERR_RETURN
                (tbhmod_gen3_rsfec_cw_type(&pm_phy_copy.access, 1, 0));
        }
    }

    /* Get TVCO because it's not allowed to change during speed set */
    pm_phy_copy.access.pll_idx = 0;
    PHYMOD_IF_ERR_RETURN
        (blackhawk7_v1l8p1_INTERNAL_read_pll_div(&pm_phy_copy.access, &tvco_pll_div));

    /* based on the current TVCO PLL div, decide which copy of speed id entry to load */
    /* and this step only applies to (non)ilkn port*/
    /* first set the lane mask to be 0x1 */
    pm_phy_copy.access.lane_mask = 1 << 0;
    if ((tvco_pll_div == (uint32_t) TBHMOD_GEN3_PLL_MODE_DIV_170) || (tvco_pll_div == (uint32_t) TBHMOD_GEN3_PLL_MODE_DIV_85))  {
        /* then load 26G TVCO speed id entry */
        PHYMOD_IF_ERR_RETURN
            (phymod_mem_write(&pm_phy_copy.access, phymodMemSpeedIdTable, TSCBH_GEN3_FORCED_SPEED_ID_OFFSET + start_lane, &tscbh_gen3_spd_id_entry_26[mapped_speed_id][0]));
    } else if ((tvco_pll_div == (uint32_t) TBHMOD_GEN3_PLL_MODE_DIV_165) || (tvco_pll_div == (uint32_t)TBHMOD_GEN3_PLL_MODE_DIV_82P5)) {
        /* then load 25G TVCO speed id entry */
        PHYMOD_IF_ERR_RETURN
            (phymod_mem_write(&pm_phy_copy.access, phymodMemSpeedIdTable, TSCBH_GEN3_FORCED_SPEED_ID_OFFSET + start_lane, &tscbh_gen3_spd_id_entry_25[mapped_speed_id][0]));
    } else {
        /* then load 20G TVCO speed id entry */
        PHYMOD_IF_ERR_RETURN
            (phymod_mem_write(&pm_phy_copy.access, phymodMemSpeedIdTable, TSCBH_GEN3_FORCED_SPEED_ID_OFFSET + start_lane, &tscbh_gen3_spd_id_entry_20[mapped_speed_id][0]));
    }

    /* Check the request speed VCO */
    pm_phy_copy.access.lane_mask = 1 << start_lane;
    PHYMOD_IF_ERR_RETURN
        (tbhmod_gen3_refclk_get(&pm_phy_copy.access, &ref_clk));

    /* Get requested PLL */
    /*for ethernet speed mode config set */
    PHYMOD_IF_ERR_RETURN
        (tbhmod_gen3_plldiv_lkup_get(&pm_phy_copy.access, mapped_speed_id, ref_clk, &request_pll_div));

    /* for the 10G single lane and 40G 4 lane case, either 20G or 25G VCO can work */
    /* first check if requested 20G VCO is present in the current PLL config, if NOT, need
     to try 25G VCO present or not */
    if (request_pll_div != tvco_pll_div) {
        if  (((int)tvco_pll_div == TBHMOD_GEN3_PLL_MODE_DIV_165) ||
             ((int)tvco_pll_div == TBHMOD_GEN3_PLL_MODE_DIV_82P5)) {
            request_pll_div = tvco_pll_div;
            osr_2p5_is_required = 1;
        }
    }

    /* next check if request PLL div is the same as the current tvco pll div */
    if (tvco_pll_div != request_pll_div) {
        /*this speed request can not be configured */
        PHYMOD_DEBUG_ERROR(("ERROR :: this speed can not be configured \n"));
        return PHYMOD_E_CONFIG;
    }

    pm_phy_copy.access.lane_mask = lane_mask_backup;
    /* Program OS mode */
    /* need to check if osr 2.5 is required */
    if (osr_2p5_is_required) {
        PHYMOD_IF_ERR_RETURN
            (blackhawk7_v1l8p1_osr_mode_set(&pm_phy_copy.access, TBHMOD_GEN3_OS_MODE_2p5));
    } else {
        PHYMOD_IF_ERR_RETURN
            (tbhmod_gen3_pmd_osmode_set(&pm_phy_copy.access, mapped_speed_id, ref_clk));
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
             (_tscbh_gen3_phy_firmware_lane_config_set(&pm_phy_copy, firmware_lane_config));
    }

    /* next need to enable/disable link training based on the input */
    if (speed_config->linkTraining) {
        PHYMOD_IF_ERR_RETURN
            (tscbh_gen3_phy_cl72_set(phy, speed_config->linkTraining));
    } else {
        /* disable cl72 and avoid overwriting the value from above _firmware_lane_config_set */
        for (i = 0; i < num_lane; i++) {
            pm_phy_copy.access.lane_mask = 1 << (start_lane + i);
            if (!PHYMOD_LANEPBMP_MEMBER(phy->access.lane_mask, start_lane + i)) {
                continue;
            }
            PHYMOD_IF_ERR_RETURN
                (blackhawk7_v1l8p1_clause72_control(&pm_phy_copy.access, speed_config->linkTraining));
        }
    }

    /*release the lne soft reset bit*/
    PHYMOD_MEMCPY(&pm_phy_copy, phy, sizeof(pm_phy_copy));
    PHYMOD_IF_ERR_RETURN
        (blackhawk7_v1l8p1_lane_soft_reset(&pm_phy_copy.access, 0));

    /* Release the pcs lane reset */
    if (port_enable) {
        pm_phy_copy.access.lane_mask = 1 << start_lane;
        PHYMOD_IF_ERR_RETURN
            (tbhmod_gen3_enable_set(&pm_phy_copy.access));
    }

    return PHYMOD_E_NONE;
}

STATIC
int _tscbh_gen3_speed_table_entry_to_speed_config_get(const phymod_phy_access_t *phy,
                                        tbhmod_gen3_spd_id_tbl_entry_t *speed_config_entry,
                                        phymod_phy_speed_config_t *speed_config)
{
    uint32_t pll_div, refclk_in_hz, data_rate_lane;
    int osr_mode = 0;
    tbhmod_gen3_refclk_t ref_clk;
    uint32_t vco_rate = 20625000;
    phymod_phy_access_t phy_copy;
    phymod_firmware_lane_config_t firmware_lane_config = {0};
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

    phy_copy.access.pll_idx = 0;
    PHYMOD_MEMSET(&firmware_lane_config,0,sizeof(firmware_lane_config));
    PHYMOD_IF_ERR_RETURN
        (tscbh_gen3_phy_firmware_lane_config_get(phy, &firmware_lane_config));

    /* get the PLL div from HW */
    PHYMOD_IF_ERR_RETURN
        (blackhawk7_v1l8p1_INTERNAL_read_pll_div(&phy_copy.access, &pll_div));

    PHYMOD_IF_ERR_RETURN
        (tbhmod_gen3_refclk_get(&phy_copy.access, &ref_clk));

    if (ref_clk == TBHMOD_GEN3_REF_CLK_312P5MHZ) {
        refclk_in_hz = 312500000;
    } else {
        refclk_in_hz = 156250000;
    }
    PHYMOD_IF_ERR_RETURN
        (blackhawk7_v1l8p1_get_vco_from_refclk_div(&phy_copy.access, refclk_in_hz, pll_div, &vco_rate, 0));

    PHYMOD_IF_ERR_RETURN
        (blackhawk7_v1l8p1_osr_mode_get(&phy_copy.access, &osr_mode));

    if (vco_rate != 20625000) {
        data_rate_lane = 25000;
    } else {
        data_rate_lane = 20000;
    }

    /* next check if PAM4 mode enabled */
    if (firmware_lane_config.ForcePAM4Mode) {
        data_rate_lane = data_rate_lane  << 1;
    } else {
        /* to get the over sample value */
        /* first check if osr 2.5 is used or not */
        if (osr_mode == TSCBH_GEN3_PMD_OSR_2P5) {
            data_rate_lane = 10000;
        } else {
            data_rate_lane = data_rate_lane >> osr_mode;
        }
    }

    speed_config->data_rate = data_rate_lane * num_lane;

    PHYMOD_IF_ERR_RETURN
        (tbhmod_gen3_fec_arch_decode_get(speed_config_entry->fec_arch, &(speed_config->fec_type)));

    return PHYMOD_E_NONE;
}

int tscbh_gen3_phy_speed_config_get(const phymod_phy_access_t* phy, phymod_phy_speed_config_t* speed_config)
{
    uint32_t cl72_enable = 0;
    phymod_phy_access_t phy_copy;
    phymod_firmware_lane_config_t firmware_lane_config = {0};
    int start_lane, num_lane, speed_id;
    uint32_t packed_entry[20];
    tbhmod_gen3_spd_id_tbl_entry_t speed_config_entry;
    int an_en, an_done /*, osr_mode */;

    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));

    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(&phy->access, &start_lane, &num_lane));
    PHYMOD_MEMSET(&firmware_lane_config,0,sizeof(firmware_lane_config));
    PHYMOD_IF_ERR_RETURN
        (tscbh_gen3_phy_firmware_lane_config_get(phy, &firmware_lane_config));

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
            (tbhmod_gen3_speed_id_get(&phy_copy.access, &speed_id));

        /* next check check if AN enabled */
        PHYMOD_IF_ERR_RETURN
           (tbhmod_gen3_autoneg_status_get(&phy_copy.access, &an_en, &an_done));

        /* first read the speed entry and then decode the speed and FEC type */
        phy_copy.access.lane_mask = 1 << 0;
        PHYMOD_IF_ERR_RETURN
            (phymod_mem_read(&phy_copy.access, phymodMemSpeedIdTable, speed_id, packed_entry));

        /*decode speed entry */
        tbhmod_gen3_spd_ctrl_unpack_spd_id_tbl_entry(packed_entry, &speed_config_entry);


        PHYMOD_IF_ERR_RETURN
            (_tscbh_gen3_speed_table_entry_to_speed_config_get(phy, &speed_config_entry, speed_config));

        /* if autoneg enabled, needs to update the FEC_ARCH based on the An resolved status */
        if (an_en && an_done) {
            uint8_t fec_arch;
            phy_copy.access.lane_mask = 0x1 << start_lane;
            PHYMOD_IF_ERR_RETURN
                (tbhmod_gen3_autoneg_fec_status_get(&phy_copy.access, &fec_arch));
            PHYMOD_IF_ERR_RETURN
                (tbhmod_gen3_fec_arch_decode_get(fec_arch, &(speed_config->fec_type)));
        }
    }

    /* next get the cl72 enable status */
    phy_copy.access.lane_mask = 0x1 << start_lane;
    PHYMOD_IF_ERR_RETURN
        (blackhawk7_v1l8p1_clause72_control_get(&phy_copy.access, &cl72_enable));
    speed_config->linkTraining = cl72_enable;

    return PHYMOD_E_NONE;
}


int tscbh_gen3_phy_cl72_set(const phymod_phy_access_t* phy, uint32_t cl72_en)
{
    struct blackhawk7_v1l8p1_uc_lane_config_st serdes_firmware_config;
    phymod_firmware_lane_config_t firmware_lane_config;
    int start_lane, num_lane, i, precoder_en;
    uint32_t lane_reset, pcs_lane_enable;
    phymod_phy_access_t pm_phy_copy;

    PHYMOD_MEMCPY(&pm_phy_copy, phy, sizeof(pm_phy_copy));
    PHYMOD_IF_ERR_RETURN(blackhawk7_v1l8p1_get_uc_lane_cfg(&pm_phy_copy.access, &serdes_firmware_config));

    if ((serdes_firmware_config.field.dfe_on == 0) && cl72_en) {
      PHYMOD_DEBUG_ERROR(("ERROR :: DFE is off : Can not start CL72/CL93 with no DFE\n"));
      return PHYMOD_E_CONFIG;
    }

    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(&phy->access, &start_lane, &num_lane));

    /*first check if lane is in reset */
    PHYMOD_IF_ERR_RETURN
        (blackhawk7_v1l8p1_lane_soft_reset_get(&pm_phy_copy.access, &lane_reset));

    /*next check if PCS lane is in reset */
    PHYMOD_IF_ERR_RETURN
        (tbhmod_gen3_enable_get(&pm_phy_copy.access, &pcs_lane_enable));

    /* disable pcs lane if pcs lane not in rset */
    if (pcs_lane_enable) {
        pm_phy_copy.access.lane_mask = 1 << start_lane;
        PHYMOD_IF_ERR_RETURN
            (tbhmod_gen3_disable_set(&pm_phy_copy.access));
        /* add the pcs disable SW WAR */
        PHYMOD_IF_ERR_RETURN
            (tbhmod_gen3_pcs_reset_sw_war(&phy->access));
    }

    /* if lane is not in reset, then reset the lane first */
    if (!lane_reset) {
        PHYMOD_MEMCPY(&pm_phy_copy, phy, sizeof(pm_phy_copy));
        PHYMOD_IF_ERR_RETURN
            (blackhawk7_v1l8p1_lane_soft_reset(&pm_phy_copy.access, 1));
    }

    /* next need to clear both force ER and NR config on the firmware lane config side
    if link training enable is set */
    if (cl72_en) {
        PHYMOD_IF_ERR_RETURN
            (tscbh_gen3_phy_firmware_lane_config_get(phy, &firmware_lane_config));

        firmware_lane_config.ForceNormalReach = 0;
        firmware_lane_config.ForceExtenedReach = 0;

         PHYMOD_IF_ERR_RETURN
            (_tscbh_gen3_phy_firmware_lane_config_set(phy, firmware_lane_config));
    } else {
        /* disable Tx pre-coding and set Rx in NR mode */
        for (i = 0; i < num_lane; i++) {
            pm_phy_copy.access.lane_mask = 1 << (start_lane + i);
            precoder_en = 0;
            PHYMOD_IF_ERR_RETURN
                (tscbh_gen3_phy_tx_pam4_precoder_enable_get(&pm_phy_copy, &precoder_en));
            if (precoder_en) {
                PHYMOD_IF_ERR_RETURN
                    (tscbh_gen3_phy_tx_pam4_precoder_enable_set(&pm_phy_copy, 0));
            }
            PHYMOD_IF_ERR_RETURN
                (tscbh_gen3_phy_firmware_lane_config_get(&pm_phy_copy, &firmware_lane_config));
            if (firmware_lane_config.ForcePAM4Mode) {
                firmware_lane_config.ForceNormalReach = 1;
                firmware_lane_config.ForceExtenedReach = 0;
                PHYMOD_IF_ERR_RETURN
                    (_tscbh_gen3_phy_firmware_lane_config_set(&pm_phy_copy, firmware_lane_config));
            }
        }
    }
    for (i = 0; i < num_lane; i++) {
        pm_phy_copy.access.lane_mask = 1 << (start_lane + i);
        if (!PHYMOD_LANEPBMP_MEMBER(phy->access.lane_mask, start_lane + i)) {
            continue;
        }
        PHYMOD_IF_ERR_RETURN
            (blackhawk7_v1l8p1_clause72_control(&pm_phy_copy.access, cl72_en));
    }

    /* release the ln dp reset */
    if (!lane_reset) {
        PHYMOD_MEMCPY(&pm_phy_copy, phy, sizeof(pm_phy_copy));
        PHYMOD_IF_ERR_RETURN
            (blackhawk7_v1l8p1_lane_soft_reset(&pm_phy_copy.access, 0));
    }

    /* re-enable pcs lane if pcs lane not in rset */
    if (pcs_lane_enable) {
        pm_phy_copy.access.lane_mask = 1 << start_lane;
        PHYMOD_IF_ERR_RETURN
            (tbhmod_gen3_enable_set(&pm_phy_copy.access));
    }

    return PHYMOD_E_NONE;
}

int tscbh_gen3_phy_cl72_get(const phymod_phy_access_t* phy, uint32_t* cl72_en)
{
    phymod_phy_access_t pm_phy_copy;

    PHYMOD_MEMCPY(&pm_phy_copy, phy, sizeof(pm_phy_copy));

    PHYMOD_IF_ERR_RETURN
        (blackhawk7_v1l8p1_clause72_control_get(&pm_phy_copy.access, cl72_en));

    return PHYMOD_E_NONE;
}


int tscbh_gen3_phy_cl72_status_get(const phymod_phy_access_t* phy, phymod_cl72_status_t* status)
{
    int i;
    uint32_t tmp_status;
    int start_lane, num_lane;
    phymod_phy_access_t phy_copy;
    int an_en, an_done, speed_id;
    tbhmod_gen3_spd_id_tbl_entry_t speed_config_entry;
    uint32_t packed_entry[5];

    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));
    status->locked = 1;

    /* next figure out the lane num and start_lane based on the input */
    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(&phy->access, &start_lane, &num_lane));
    /* check check if AN enabled */
    PHYMOD_IF_ERR_RETURN
       (tbhmod_gen3_autoneg_status_get(&phy_copy.access, &an_en, &an_done));

    if (an_en && an_done) {
        PHYMOD_IF_ERR_RETURN
            (tbhmod_gen3_speed_id_get(&phy_copy.access, &speed_id));
        PHYMOD_IF_ERR_RETURN
            (phymod_mem_read(&phy_copy.access, phymodMemSpeedIdTable, speed_id, packed_entry));
        tbhmod_gen3_spd_ctrl_unpack_spd_id_tbl_entry(packed_entry, &speed_config_entry);
        /* Update num_lane for AN port */
        num_lane = 1 << speed_config_entry.num_lanes;
    }

    for (i = 0; i < num_lane; i++) {
        if (!PHYMOD_LANEPBMP_MEMBER(phy->access.lane_mask, start_lane + i)) {
            continue;
        }
        phy_copy.access.lane_mask = 0x1 << (i + start_lane);
        tmp_status = 1;
        PHYMOD_IF_ERR_RETURN(blackhawk7_v1l8p1_pmd_cl72_receiver_status(&phy_copy.access, &tmp_status));
        if (tmp_status == 0) {
            status->locked = 0;
            return PHYMOD_E_NONE;
        }
    }

    return PHYMOD_E_NONE;
}

int tscbh_gen3_phy_loopback_set(const phymod_phy_access_t* phy, phymod_loopback_mode_t loopback, uint32_t enable)
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
                (tbhmod_gen3_disable_set(&phy_copy.access));
            /*first squelch rx */
            for (i = 0; i < num_lane; i++) {
                if (!PHYMOD_LANEPBMP_MEMBER(phy->access.lane_mask, start_lane + i)) {
                    continue;
                }
                phy_copy.access.lane_mask = 0x1 << (i + start_lane);
                PHYMOD_IF_ERR_RETURN(blackhawk7_v1l8p1_pmd_force_signal_detect(&phy_copy.access,  (int) enable, (int) 0));
            }
            phy_copy.access.lane_mask = 1 << start_lane;
            PHYMOD_IF_ERR_RETURN
                (tbhmod_gen3_enable_set(&phy_copy.access));
        }
        for (i = 0; i < num_lane; i++) {
            if (!PHYMOD_LANEPBMP_MEMBER(phy->access.lane_mask, start_lane + i)) {
                continue;
            }
            phy_copy.access.lane_mask = 0x1 << (i + start_lane);
            PHYMOD_IF_ERR_RETURN(blackhawk7_v1l8p1_tx_disable(&phy_copy.access, enable));
            PHYMOD_IF_ERR_RETURN(blackhawk7_v1l8p1_dig_lpbk(&phy_copy.access, (uint8_t) enable));
        }
        if (!enable) {
            for (i = 0; i < num_lane; i++) {
                if (!PHYMOD_LANEPBMP_MEMBER(phy->access.lane_mask, start_lane + i)) {
                    continue;
                }
                phy_copy.access.lane_mask = 0x1 << (i + start_lane);
                PHYMOD_IF_ERR_RETURN(blackhawk7_v1l8p1_pmd_force_signal_detect(&phy_copy.access,  (int) enable, (int) 0));
            }
        }
        break;
    case phymodLoopbackRemotePMD :
        for (i = 0; i < num_lane; i++) {
            if (!PHYMOD_LANEPBMP_MEMBER(phy->access.lane_mask, start_lane + i)) {
                continue;
            }
            phy_copy.access.lane_mask = 0x1 << (i + start_lane);
            PHYMOD_IF_ERR_RETURN(blackhawk7_v1l8p1_rmt_lpbk(&phy_copy.access, (uint8_t)enable));
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

int tscbh_gen3_phy_loopback_get(const phymod_phy_access_t* phy, phymod_loopback_mode_t loopback, uint32_t* enable)
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
        PHYMOD_IF_ERR_RETURN(blackhawk7_v1l8p1_dig_lpbk_get(&phy_copy.access, enable));
        break;
    case phymodLoopbackRemotePMD :
        PHYMOD_IF_ERR_RETURN(blackhawk7_v1l8p1_rmt_lpbk_get(&phy_copy.access, enable));
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
 * (PASS2)
 * 4.  Configure PMD lane mapping and PCS lane swap
 * 5.  De-assert micro reset
 * 6.  Wait for uc_active = 1
 * 7.  Initialize software information table for the micro
 * 8.  Config PMD polarity
 * 9. AFE/PLL configuration
 * 10. Set core_from_pcs_config
 * 11. Program AN default timer
 * 12. Load sd_id_table, am_table and um_table into TSC memory
 * 13. Release core DP soft reset
 */
STATIC
int _tscbh_gen3_core_init_pass1(const phymod_core_access_t* core, const phymod_core_init_config_t* init_config, const phymod_core_status_t* core_status)
{
    int  rv, lane;
    phymod_phy_access_t phy_access;
    phymod_core_access_t  core_copy;
    /* need to fix this later */
    uint32_t uc_enable = 0;
    ucode_info_t ucode;
    unsigned int blackhawk7_ucode_len;
    unsigned char *blackhawk7_ucode;
    uint32_t i, am_table_load_size, um_table_load_size;

    TSCBH_GEN3_CORE_TO_PHY_ACCESS(&phy_access, core);
    PHYMOD_MEMCPY(&core_copy, core, sizeof(core_copy));
    core_copy.access.lane_mask = 0x1;

    /* 1. De-assert PMD core power and core data path reset */
    PHYMOD_IF_ERR_RETURN
        (tbhmod_gen3_pmd_reset_seq(&core_copy.access));

    core_copy.access.pll_idx = 0;
    PHYMOD_IF_ERR_RETURN
        (blackhawk7_v1l8p1_core_dp_reset(&core_copy.access, 1));

    /* De-assert PMD lane reset */
    for (lane = 0; lane < TSCBH_GEN3_NOF_LANES_IN_CORE; lane++) {
        phy_access.access.lane_mask = 1 << lane;
        PHYMOD_IF_ERR_RETURN
          (tbhmod_gen3_pmd_x4_reset(&phy_access.access));
    }

    if (!PHYMOD_CORE_INIT_F_SERDES_FW_BCAST_GET(init_config)) {
        PHYMOD_IF_ERR_RETURN
            (blackhawk7_v1l8p1_uc_active_get(&phy_access.access, &uc_enable));
        if (uc_enable) return PHYMOD_E_NONE;
    }

    /* 2. Set the heart beat, default is 156.25M */
    if (init_config->interface.ref_clock != phymodRefClk156Mhz) {
        PHYMOD_IF_ERR_RETURN
            (blackhawk7_v1l8p1_refclk_set(&core_copy.access, init_config->interface.ref_clock));
        PHYMOD_IF_ERR_RETURN
            (tbhmod_gen3_refclk_set(&phy_access.access, TBHMOD_GEN3_REF_CLK_312P5MHZ));
    }

    /* set the micro stack size */
    phy_access.access.lane_mask = 0x1;
    ucode.stack_size = blackhawk7_v1l8p1_ucode_stack_size;
    ucode.ucode_size = blackhawk7_v1l8p1_ucode_len;
    blackhawk7_ucode_len = blackhawk7_v1l8p1_ucode_len;
    blackhawk7_ucode = blackhawk7_v1l8p1_ucode;

    PHYMOD_IF_ERR_RETURN
    (blackhawk7_v1l8p1_uc_reset_with_info(&core_copy.access , 1, ucode));

    /* 3. Micro code load and verify */
    rv = _tscbh_gen3_core_firmware_load(&core_copy, init_config);
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
            rv = blackhawk7_v1l8p1_ucode_load_verify(&core_copy.access, (uint8_t *) blackhawk7_ucode, blackhawk7_ucode_len);
            if (rv != PHYMOD_E_NONE) {
                PHYMOD_DEBUG_ERROR(("devad 0x%x lane 0x%x: UC load-verify failed\n", core->access.addr, core->access.lane_mask));
                PHYMOD_IF_ERR_RETURN(rv);
            }
        }
    }

    am_table_load_size = TSCBH_GEN3_AM_TABLE_SIZE > TSCBH_GEN3_HW_AM_TABLE_SIZE ? TSCBH_GEN3_HW_AM_TABLE_SIZE : TSCBH_GEN3_AM_TABLE_SIZE;
    um_table_load_size = TSCBH_GEN3_UM_TABLE_SIZE > TSCBH_GEN3_HW_UM_TABLE_SIZE ? TSCBH_GEN3_HW_UM_TABLE_SIZE : TSCBH_GEN3_UM_TABLE_SIZE;

    for (i = 0; i < am_table_load_size; i++) {
        PHYMOD_IF_ERR_RETURN
            (phymod_mem_write(&core_copy.access, phymodMemAMTable, i,  &tscbh_gen3_am_table_entry[i][0]));
    }

    for (i = 0; i < um_table_load_size; i++) {
        PHYMOD_IF_ERR_RETURN
            (phymod_mem_write(&core_copy.access, phymodMemUMTable, i,  &tscbh_gen3_um_table_entry[i][0]));
    }

    /*need to update speed_priority_mapping_table with correct speed id */
    PHYMOD_IF_ERR_RETURN
        (phymod_mem_write(&core_copy.access, phymodMemSpeedPriorityMapTable, 0,  &tscbh_gen3_speed_priority_mapping_table[0][0]));

    /* Program AN default timer  for both MMP0 and MMP1*/
    core_copy.access.lane_mask = 0x1;
    core_copy.access.pll_idx = 0;
    PHYMOD_IF_ERR_RETURN
        (tbhmod_gen3_set_an_timers(&core_copy.access, init_config->interface.ref_clock, NULL));
    /* enable FEC COBRA as default */
    PHYMOD_IF_ERR_RETURN
        (tbhmod_gen3_fec_cobra_enable(&core_copy.access, 1));

    core_copy.access.lane_mask = 0x10;
    core_copy.access.pll_idx = 0;
    PHYMOD_IF_ERR_RETURN
        (tbhmod_gen3_set_an_timers(&core_copy.access, init_config->interface.ref_clock, NULL));
    /* enable FEC COBRA as default */
    PHYMOD_IF_ERR_RETURN
        (tbhmod_gen3_fec_cobra_enable(&core_copy.access, 1));

    return PHYMOD_E_NONE;
}


STATIC
int _tscbh_gen3_core_init_pass2(const phymod_core_access_t* core, const phymod_core_init_config_t* init_config, const phymod_core_status_t* core_status)
{
    phymod_phy_access_t phy_access, phy_access_copy;
    phymod_core_access_t  core_copy;
    enum blackhawk7_v1l8p1_pll_refclk_enum refclk;
    phymod_polarity_t tmp_pol;
    int lane;
    uint32_t speed_id_load_size;
    uint32_t tvco_rate, i, fclk_div_mode = 1;

    TSCBH_GEN3_CORE_TO_PHY_ACCESS(&phy_access, core);
    phy_access_copy = phy_access;
    PHYMOD_MEMCPY(&core_copy, core, sizeof(core_copy));
    core_copy.access.lane_mask = 0x1;
    phy_access_copy = phy_access;
    phy_access_copy.access = core->access;
    phy_access_copy.access.lane_mask = 0x1;
    phy_access_copy.type = core->type;
    PHYMOD_MEMSET(&tmp_pol, 0x0, sizeof(tmp_pol));

    /* 4. Configure PMD lane mapping and PCS lane swap */
    PHYMOD_IF_ERR_RETURN
        (tscbh_gen3_core_lane_map_set(&core_copy, &init_config->lane_map));

    if (init_config->interface.ref_clock == phymodRefClk156Mhz) {
        refclk = BLACKHAWK7_V1L8P1_PLL_REFCLK_156P25MHZ;
        PHYMOD_IF_ERR_RETURN
            (tbhmod_gen3_pll_to_vco_get(TBHMOD_GEN3_REF_CLK_156P25MHZ, init_config->pll0_div_init_value, &tvco_rate));
    } else if (init_config->interface.ref_clock == phymodRefClk312Mhz) {
        refclk = BLACKHAWK7_V1L8P1_PLL_REFCLK_312P5MHZ;
        PHYMOD_IF_ERR_RETURN
            (tbhmod_gen3_pll_to_vco_get(TBHMOD_GEN3_REF_CLK_312P5MHZ, init_config->pll0_div_init_value, &tvco_rate));
    } else {
        PHYMOD_DEBUG_ERROR(("Unsupported reference clock.\n"));
        return PHYMOD_E_UNAVAIL;
    }

    /*next need to load speed id table and AM table */
    speed_id_load_size = TSCBH_GEN3_SPEED_ID_TABLE_SIZE > TSCBH_GEN3_HW_SPEED_ID_TABLE_SIZE ? TSCBH_GEN3_HW_SPEED_ID_TABLE_SIZE : TSCBH_GEN3_SPEED_ID_TABLE_SIZE;

     if (tvco_rate == TBHMOD_GEN3_VCO_26G) {
        for (i = 0; i < speed_id_load_size; i++) {
            PHYMOD_IF_ERR_RETURN
                (phymod_mem_write(&core_copy.access, phymodMemSpeedIdTable, i, &tscbh_gen3_spd_id_entry_26[i][0]));
        }
    } else if (tvco_rate == TBHMOD_GEN3_VCO_25G) {
        for (i = 0; i < speed_id_load_size; i++) {
            PHYMOD_IF_ERR_RETURN
                (phymod_mem_write(&core_copy.access, phymodMemSpeedIdTable, i, &tscbh_gen3_spd_id_entry_25[i][0]));
        }
    } else if (tvco_rate == TBHMOD_GEN3_VCO_20G) {
        for (i = 0; i < speed_id_load_size; i++) {
            PHYMOD_IF_ERR_RETURN
                (phymod_mem_write(&core_copy.access, phymodMemSpeedIdTable, i, &tscbh_gen3_spd_id_entry_20[i][0]));
        }
    }

    if (init_config->firmware_load_method != phymodFirmwareLoadMethodNone) {
        ucode_info_t ucode;
        ucode.stack_size = blackhawk7_v1l8p1_ucode_stack_size;
        ucode.ucode_size = blackhawk7_v1l8p1_ucode_len;
        ucode.crc_value  = blackhawk7_v1l8p1_ucode_crc;

        /* 6. Release uc reset */
        PHYMOD_IF_ERR_RETURN
        (blackhawk7_v1l8p1_uc_reset_with_info(&core_copy.access , 0, ucode));

        /* 7. Wait for uc_active = 1 */
        PHYMOD_IF_ERR_RETURN
            (blackhawk7_v1l8p1_wait_uc_active(&phy_access.access));

        for (lane = 0; lane < TSCBH_GEN3_NOF_LANES_IN_CORE; lane++) {
            phy_access_copy.access.lane_mask = 1 << lane;
            PHYMOD_IF_ERR_RETURN
                (blackhawk7_v1l8p1_pmd_ln_h_rstb_pkill_override(&phy_access_copy.access, 0x1));
        }

        /* 8. Initialize software information table for the macro */
        PHYMOD_IF_ERR_RETURN
            (blackhawk7_v1l8p1_init_blackhawk7_v1l8p1_info(&core_copy.access));

        /* check the FW crc checksum error */
        if (!PHYMOD_CORE_INIT_F_BYPASS_CRC_CHECK_GET(init_config)) {
            PHYMOD_IF_ERR_RETURN
                (blackhawk7_v1l8p1_ucode_crc_verify(&core_copy.access, ucode.ucode_size, ucode.crc_value));
        }

        /* release pmd lane hard reset */
        for (lane = 0; lane < TSCBH_GEN3_NOF_LANES_IN_CORE; lane++) {
            phy_access_copy.access.lane_mask = 1 << lane;
            PHYMOD_IF_ERR_RETURN
                (blackhawk7_v1l8p1_pmd_ln_h_rstb_pkill_override(&phy_access_copy.access, 0x0));
        }
    } else {

        /* This function will be implemented if DNX needs. */
        /*PHYMOD_IF_ERR_RETURN
            (blackhawk7_v1l8p1_firmware_load_none_init_blackhawk7_v1l8p1_info(&core_copy.access));
        */
    }

    /* 9. RX/TX polarity configuration */
    for (lane = 0; lane < TSCBH_GEN3_NOF_LANES_IN_CORE; lane++) {
        phy_access_copy.access.lane_mask = 1 << lane;
        tmp_pol.tx_polarity = (init_config->polarity_map.tx_polarity) >> lane & 0x1;
        tmp_pol.rx_polarity = (init_config->polarity_map.rx_polarity) >> lane & 0x1;
        PHYMOD_IF_ERR_RETURN
            (tscbh_gen3_phy_polarity_set(&phy_access_copy, &tmp_pol));
        /* clear the tmp vairiable */
        PHYMOD_MEMSET(&tmp_pol, 0x0, sizeof(tmp_pol));
    }

    /* Enable PCS clock block */
    core_copy.access.pll_idx = 0;
    PHYMOD_IF_ERR_RETURN
        (tbhmod_gen3_pcs_clk_blk_en(&core_copy.access, 1));

    if ((int) (init_config->pll0_div_init_value) != phymod_TSCBH_PLL_DIVNONE) {
        core_copy.access.pll_idx = 0;
        PHYMOD_IF_ERR_RETURN
            (blackhawk7_v1l8p1_configure_pll_refclk_div(&core_copy.access,
                                                      refclk,
                                                      init_config->pll0_div_init_value));
    }

    /* Set FCLK period. */
    PHYMOD_IF_ERR_RETURN
        (tbhmod_gen3_set_fclk_period(&core_copy.access, tvco_rate, fclk_div_mode));

    /* et the PMD debug level to be 2 as default */
    for (lane = 0; lane < TSCBH_GEN3_NOF_LANES_IN_CORE; lane++) {
        phy_access_copy.access.lane_mask = 1 << lane;
        PHYMOD_IF_ERR_RETURN
            (blackhawk7_v1l8p1_set_usr_ctrl_lane_event_log_level(&phy_access_copy.access, 2));
    }

    /* 14. Release core DP soft reset for PLL0 */
    core_copy.access.lane_mask = 0x1;
    core_copy.access.pll_idx = 0;
    PHYMOD_IF_ERR_RETURN
        (blackhawk7_v1l8p1_core_dp_reset(&core_copy.access, 0));

    return PHYMOD_E_NONE;

}

int tscbh_gen3_core_init(const phymod_core_access_t* core, const phymod_core_init_config_t* init_config, const phymod_core_status_t* core_status)
{
    if ( (!PHYMOD_CORE_INIT_F_EXECUTE_PASS1_GET(init_config) &&
          !PHYMOD_CORE_INIT_F_EXECUTE_PASS2_GET(init_config)) ||
        PHYMOD_CORE_INIT_F_EXECUTE_PASS1_GET(init_config)) {
        PHYMOD_IF_ERR_RETURN
            (_tscbh_gen3_core_init_pass1(core, init_config, core_status));

        if (PHYMOD_CORE_INIT_F_EXECUTE_PASS1_GET(init_config)) {
            return PHYMOD_E_NONE;
        }
    }

    if ( (!PHYMOD_CORE_INIT_F_EXECUTE_PASS1_GET(init_config) &&
          !PHYMOD_CORE_INIT_F_EXECUTE_PASS2_GET(init_config)) ||
        PHYMOD_CORE_INIT_F_EXECUTE_PASS2_GET(init_config)) {
        PHYMOD_IF_ERR_RETURN
            (_tscbh_gen3_core_init_pass2(core, init_config, core_status));
    }

    return PHYMOD_E_NONE;
}

int tscbh_gen3_phy_init(const phymod_phy_access_t* phy, const phymod_phy_init_config_t* init_config)
{
    /*int pll_restart = 0;*/
    const phymod_access_t *pm_acc = &phy->access;
    phymod_phy_access_t pm_phy_copy;
    int start_lane, num_lane, i;
    phymod_firmware_lane_config_t firmware_lane_config;
    uint32_t pll_power_down = 0;
    enum blackhawk7_v1l8p1_txfir_tap_enable_enum enable_taps = BLACKHAWK7_V1L8P1_NRZ_6TAP;

    PHYMOD_MEMCPY(&pm_phy_copy, phy, sizeof(pm_phy_copy));
    PHYMOD_MEMSET(&firmware_lane_config, 0x0, sizeof(firmware_lane_config));

    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(pm_acc, &start_lane, &num_lane));
    /* per lane based reset release */
    PHYMOD_IF_ERR_RETURN
        (tbhmod_gen3_pmd_x4_reset(&pm_phy_copy.access));

    /* Put PMD lane into soft reset */
    PHYMOD_IF_ERR_RETURN
        (blackhawk7_v1l8p1_lane_soft_reset(&pm_phy_copy.access, 1));

    for (i = 0; i < num_lane; i++) {
        if (!PHYMOD_LANEPBMP_MEMBER(phy->access.lane_mask, start_lane + i)) {
            continue;
        }
        pm_phy_copy.access.lane_mask = 1 << (start_lane + i);
        /*next check 3 tap mode or 6 tap mode */
        if (init_config->tx[i].tap_mode == phymodTxTapMode3Tap) {
            if (init_config->tx[i].sig_method == phymodSignallingMethodNRZ) {
                enable_taps= BLACKHAWK7_V1L8P1_NRZ_LP_3TAP;
            } else {
                enable_taps= BLACKHAWK7_V1L8P1_PAM4_LP_3TAP;
            }
            PHYMOD_IF_ERR_RETURN
                (blackhawk7_v1l8p1_apply_txfir_cfg(&pm_phy_copy.access,
                                                 enable_taps,
                                                 0,
                                                 init_config->tx[i].pre,
                                                 init_config->tx[i].main,
                                                 init_config->tx[i].post,
                                                 0,
                                                 0));
        } else {
            if (init_config->tx[i].sig_method == phymodSignallingMethodNRZ) {
                enable_taps= BLACKHAWK7_V1L8P1_NRZ_6TAP;
            } else {
                enable_taps= BLACKHAWK7_V1L8P1_PAM4_6TAP;
            }
            PHYMOD_IF_ERR_RETURN
                (blackhawk7_v1l8p1_apply_txfir_cfg(&pm_phy_copy.access,
                                                 enable_taps,
                                                 init_config->tx[i].pre2,
                                                 init_config->tx[i].pre,
                                                 init_config->tx[i].main,
                                                 init_config->tx[i].post,
                                                 init_config->tx[i].post2,
                                                 init_config->tx[i].post3));
        }
    }

    /* clearing all the lane config */
    PHYMOD_MEMSET(&firmware_lane_config, 0x0, sizeof(firmware_lane_config));
    for (i = 0; i < num_lane; i++) {
        if (!PHYMOD_LANEPBMP_MEMBER(phy->access.lane_mask, start_lane + i)) {
            continue;
        }
        pm_phy_copy.access.lane_mask = 0x1 << (i + start_lane);
        PHYMOD_IF_ERR_RETURN
             (_tscbh_gen3_phy_firmware_lane_config_set(&pm_phy_copy, firmware_lane_config));
        /* FROM PMD team:
         * The best initial analog calibration is achieved using the information
         * from the PMD RX PRBS checker. Using the FW register value of 3 enables
         * a continuous version that does not rely on PRBS traffic anymore.
         */
        PHYMOD_IF_ERR_RETURN
             (blackhawk7_v1l8p1_lane_cfg_fwapi_data1_set(&pm_phy_copy.access, 3));
    }

    /* make sure that power up PLL is locked */
    pm_phy_copy.access.pll_idx = 0;
    pm_phy_copy.access.lane_mask = 1 << 0;
    PHYMOD_IF_ERR_RETURN
        (blackhawk7_v1l8p1_pll_pwrdn_get(&pm_phy_copy.access, &pll_power_down));

    /* need to check pll0 lock if not power up */
    /* put the check here is to save on boot up time */
    if (!pll_power_down) {
        uint32_t cnt = 0, pll_lock = 0;
        cnt = 0;
        while (cnt < 1000) {
            PHYMOD_IF_ERR_RETURN(blackhawk7_v1l8p1_pll_lock_get(&pm_phy_copy.access, &pll_lock));
            cnt = cnt + 1;
            if (pll_lock) {
                break;
            } else {
                if(cnt == 1000) {
                    PHYMOD_DEBUG_ERROR(("WARNING :: core 0x%x PLL0 is not locked within 5 milli second \n", pm_phy_copy.access.addr));
                    break;
                }
            }
            PHYMOD_USLEEP(10);
        }
    }

    if (!PHYMOD_PHY_INIT_F_SW_WAR_100G_2LANE_IS_DONE_GET(init_config) && (num_lane != 8)) {
        /* this SW WAR is required for lane 0 land lane 4 */
        pm_phy_copy.access.lane_mask = 0x1;
        /* release PMD lane into soft reset */
        PHYMOD_IF_ERR_RETURN
            (blackhawk7_v1l8p1_lane_soft_reset(&pm_phy_copy.access, 0));
        pm_phy_copy.access.lane_mask = 0x10;
        /* release PMD lane into soft reset */
        PHYMOD_IF_ERR_RETURN
            (blackhawk7_v1l8p1_lane_soft_reset(&pm_phy_copy.access, 0));

        /* add the flexport workaround for the 100G 2 lane AM  issue */
        PHYMOD_IF_ERR_RETURN
            (_tscbh_gen3_100g_2lane_sw_workaround(&pm_phy_copy));
        pm_phy_copy.access.lane_mask = 0x1;
        PHYMOD_IF_ERR_RETURN
            (_tscbh_gen3_100g_2lane_sw_workaround(&pm_phy_copy));

        /* Put PMD lane into soft reset */
        PHYMOD_IF_ERR_RETURN
            (blackhawk7_v1l8p1_lane_soft_reset(&pm_phy_copy.access, 1));
        pm_phy_copy.access.lane_mask = 0x10;
        PHYMOD_IF_ERR_RETURN
            (blackhawk7_v1l8p1_lane_soft_reset(&pm_phy_copy.access, 1));
    }

    return PHYMOD_E_NONE;
}

int tscbh_gen3_phy_link_status_get(const phymod_phy_access_t* phy, uint32_t* link_status)
{
    phymod_phy_access_t pm_phy_copy;
    PHYMOD_MEMCPY(&pm_phy_copy, phy, sizeof(pm_phy_copy));

    PHYMOD_IF_ERR_RETURN(tbhmod_gen3_get_pcs_latched_link_status(&pm_phy_copy.access, link_status));
    return PHYMOD_E_NONE;
}


/* this function gives the rx pmd lock STATUS */
int tscbh_gen3_phy_rx_pmd_locked_get(const phymod_phy_access_t* phy, uint32_t* pmd_lock)
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
            (blackhawk7_v1l8p1_pmd_lock_status(&pm_phy_copy.access, &tmp_lock));
        *pmd_lock &= (uint32_t) tmp_lock;
    }
    return PHYMOD_E_NONE;

}

/* this function gives the rx signal detection STATUS */
int tscbh_gen3_phy_rx_signal_detect_get(const phymod_phy_access_t* phy, uint32_t* signal_detect)
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
            (blackhawk7_v1l8p1_signal_detect(&pm_phy_copy.access, &tmp_detect));
        *signal_detect &= tmp_detect;
    }
    return PHYMOD_E_NONE;

}

/* get default tx taps*/
int tscbh_gen3_phy_tx_taps_default_get(const phymod_phy_access_t* phy, phymod_phy_signalling_method_t mode, phymod_tx_t* tx)
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

int tscbh_gen3_phy_reg_read(const phymod_phy_access_t* phy, uint32_t reg_addr, uint32_t* val)
{
    PHYMOD_IF_ERR_RETURN(phymod_tscbh_iblk_read(&phy->access, reg_addr, val));
    return PHYMOD_E_NONE;
}


int tscbh_gen3_phy_reg_write(const phymod_phy_access_t* phy, uint32_t reg_addr, uint32_t val)
{
    PHYMOD_IF_ERR_RETURN(phymod_tscbh_iblk_write(&phy->access, reg_addr, val));
    return PHYMOD_E_NONE;
}


/* get default pmd lane config*/
int tscbh_gen3_phy_lane_config_default_get(const phymod_phy_access_t* phy, phymod_phy_signalling_method_t mode, phymod_firmware_lane_config_t* lane_config)
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
    lane_config->Cl72RestTO = 0;
    lane_config->ForceExtenedReach = 0;
    lane_config->LpPrecoderEnabled = 0;
    lane_config->UnreliableLos = 0;

    return PHYMOD_E_NONE;
}

int tscbh_gen3_phy_pll_multiplier_get(const phymod_phy_access_t* phy, uint32_t* core_vco_pll_multiplier)
{
    phymod_phy_access_t pm_phy_copy;
    PHYMOD_MEMCPY(&pm_phy_copy, phy, sizeof(pm_phy_copy));

    PHYMOD_IF_ERR_RETURN(blackhawk7_v1l8p1_INTERNAL_read_pll_div(&pm_phy_copy.access,  core_vco_pll_multiplier));
    return PHYMOD_E_NONE;
}

int tscbh_gen3_phy_firmware_load_info_get(const phymod_phy_access_t* phy, phymod_firmware_load_info_t* info)
{
    return PHYMOD_E_NONE;
}

static int _tscbh_gen3_phy_autoneg_ability_to_vco_get(const phymod_phy_access_t* phy,
                                               const phymod_autoneg_advert_abilities_t* an_advert_abilities,
                                               uint16_t* request_vco)
{
    phymod_autoneg_advert_ability_t* an_ability;
    uint32_t i, abilility_40G_10G = 0;
    phymod_phy_access_t phy_copy;

    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));

    an_ability = an_advert_abilities->autoneg_abilities;
    /* We do not validate each abilities here since they are supposed to be validated in portmod. */
    for (i = 0; i < an_advert_abilities->num_abilities; i++) {
        switch (an_ability[i].speed) {
            case 10000:
                /* CL73-10G-1lane */
                abilility_40G_10G = 1;
                break;
            case 20000:
                /* CL73BAM-20G-lanes */
                *request_vco |= TBHMOD_GEN3_VCO_20G;
                break;
            case 25000:
                /* CL73-1lane; CL73BAM-1lane; MSA-1lane */
                *request_vco |= TBHMOD_GEN3_VCO_25G;
                break;
            case 40000:
                /* CL73-40G-4lanes */
                /* CL73BAM-40G-2lanes */
                if (an_ability[i].resolved_num_lanes == 2) {
                    *request_vco |= TBHMOD_GEN3_VCO_20G;
                } else {
                    abilility_40G_10G = 1;
                }
                break;
            case 50000:
                if ((an_ability[i].fec == phymod_fec_RS544) ||
                     (an_ability[i].fec == phymod_fec_RS272)) {
                    /* CL73-50G-1lane RS544/RS272 */
                    /* CL73BAM-50G-2lanes-RS544 */
                    *request_vco |= TBHMOD_GEN3_VCO_26G;
                } else {
                    /* CL73BAM-50G-1lane*/
                    /* CL73BAM-50G-2lanes-Nofec/CL74/RS528 */
                    /* MSA-50G-2lanes */
                    *request_vco |= TBHMOD_GEN3_VCO_25G;
                }
                break;
            case 100000:
                if ((an_ability[i].fec == phymod_fec_RS544) ||
                    (an_ability[i].fec == phymod_fec_RS272))  {
                    /* CL73-100G-2lanes */
                    /* CL73 BAM 100G- 2lane_RS272 */
                    /* CL73BAM-100G-4lanes-RS544 */
                    *request_vco |= TBHMOD_GEN3_VCO_26G;
                } else {
                    /* CL73-100G-4lanes */
                    /* CL73BAM-100G-2lanes-RS528 */
                    /* CL73BAM-100G-2lanes-Nofec */
                    /* CL73BAM-100G-4lanes-Nofec */
                    *request_vco |= TBHMOD_GEN3_VCO_25G;
                }
                break;
            case 200000:
                if ((an_ability[i].fec == phymod_fec_RS544)
                    || (an_ability[i].fec == phymod_fec_RS544_2XN)
                    || (an_ability[i].fec == phymod_fec_RS272_2XN)) {
                    /* CL73-200G-4lanes-RS544_2XN */
                    /* CL73BAM-200G-4lanes-RS544_1XN */
                    /* CL73-BAM-4 lanes-RS272_2XN*/
                    *request_vco |= TBHMOD_GEN3_VCO_26G;
                } else {
                    /* CL73BAM-200G-4lanes-Nofec */
                    *request_vco |= TBHMOD_GEN3_VCO_25G;
                }
                break;
            case 400000:
                    *request_vco |= TBHMOD_GEN3_VCO_26G;
                break;
            default:
                break;
        }

    }

    /* need to check if 10G 1 lane or 40G 4 lane speed prsent */
    if (abilility_40G_10G) {
        /*if there is no 25G nor 20G VCO prsent, then 25G VCO needs to be added */
        if (!(*request_vco & TBHMOD_GEN3_VCO_25G) && !(*request_vco & TBHMOD_GEN3_VCO_20G)) {
            /* first check the current TVCO*/
            uint32_t  tvco_pll_div = 0;

            phy_copy.access.lane_mask = 0x1;
            phy_copy.access.pll_idx = 0;
            PHYMOD_IF_ERR_RETURN
                (blackhawk7_v1l8p1_INTERNAL_read_pll_div(&phy_copy.access, &tvco_pll_div));

            /* first check TVCO 25G or 20G  not */
            if ( ((int)tvco_pll_div == TBHMOD_GEN3_PLL_MODE_DIV_165) ||
                 ((int)tvco_pll_div == TBHMOD_GEN3_PLL_MODE_DIV_82P5)) {
                *request_vco |= TBHMOD_GEN3_VCO_25G;
            } else if ( ((int)tvco_pll_div == TBHMOD_GEN3_PLL_MODE_DIV_132) ||
                 ((int)tvco_pll_div == TBHMOD_GEN3_PLL_MODE_DIV_66)) {
                *request_vco |= TBHMOD_GEN3_VCO_20G;
            } else {
                *request_vco |= TBHMOD_GEN3_VCO_25G;
            }
        }
    }

    return PHYMOD_E_NONE;
}

int tscbh_gen3_phy_autoneg_advert_ability_set(const phymod_phy_access_t* phy,
                                         const phymod_autoneg_advert_abilities_t* an_advert_abilities,
                                         const phymod_phy_pll_state_t* old_pll_adv_state,
                                         phymod_phy_pll_state_t* new_pll_adv_state)
{
    uint16_t request_vco = 0;
    int start_lane, num_lane, i = 0;
    phymod_phy_access_t phy_copy;
    tbhmod_gen3_refclk_t ref_clk;
    uint32_t request_pll_div[3] = {0, 0, 0}, tvco_pll_div = 0, pll_lock, cnt;

    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(&phy->access, &start_lane, &num_lane));

    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));
    phy_copy.access.lane_mask = 0x1 << start_lane;

    PHYMOD_IF_ERR_RETURN
        (_tscbh_gen3_phy_autoneg_ability_to_vco_get(phy, an_advert_abilities, &request_vco));

    PHYMOD_IF_ERR_RETURN
        (tbhmod_gen3_refclk_get(&phy_copy.access, &ref_clk));

    /* Check the request speed VCO */
    if (request_vco & TBHMOD_GEN3_VCO_20G) {
        PHYMOD_IF_ERR_RETURN
            (tbhmod_gen3_vco_to_pll_lkup(TBHMOD_GEN3_VCO_20G, ref_clk, &request_pll_div[i]));
        i++;
    }
    if (request_vco & TBHMOD_GEN3_VCO_25G) {
        PHYMOD_IF_ERR_RETURN
            (tbhmod_gen3_vco_to_pll_lkup(TBHMOD_GEN3_VCO_25G, ref_clk, &request_pll_div[i]));
        i++;
    }
    if (request_vco & TBHMOD_GEN3_VCO_26G) {
        PHYMOD_IF_ERR_RETURN
            (tbhmod_gen3_vco_to_pll_lkup(TBHMOD_GEN3_VCO_26G, ref_clk, &request_pll_div[i]));
        i++;
    }
    if (i > 1) {
        /* More than 1 vcos are needed */
        return PHYMOD_E_PARAM;
    }

    /* Program local advert abilitiy registers */
    PHYMOD_IF_ERR_RETURN
        (tbhmod_gen3_autoneg_ability_set(&phy_copy.access, an_advert_abilities));

    /* next need to check if need to change TVCO */
    /* Get TVCO info */
    phy_copy.access.pll_idx = 0;
    PHYMOD_IF_ERR_RETURN
        (blackhawk7_v1l8p1_INTERNAL_read_pll_div(&phy_copy.access, &tvco_pll_div));

    if ((i == 1) && (request_pll_div[0] != tvco_pll_div)) {
        /*toggle core dp reset */
        phy_copy.access.lane_mask = 0x1;
        PHYMOD_IF_ERR_RETURN
            (blackhawk7_v1l8p1_core_dp_reset(&phy_copy.access, 1));

        /*config the PLL to the requested VCO */
        if (ref_clk == TBHMOD_GEN3_REF_CLK_312P5MHZ) {
            PHYMOD_IF_ERR_RETURN
                (blackhawk7_v1l8p1_configure_pll_refclk_div(&phy_copy.access,
                                                        BLACKHAWK7_V1L8P1_PLL_REFCLK_312P5MHZ,
                                                        request_pll_div[0]));
        } else {
            PHYMOD_IF_ERR_RETURN
                (blackhawk7_v1l8p1_configure_pll_refclk_div(&phy_copy.access,
                                                        BLACKHAWK7_V1L8P1_PLL_REFCLK_156P25MHZ,
                                                        request_pll_div[0]));
        }
        /* release core soft reset */
        PHYMOD_IF_ERR_RETURN
            (blackhawk7_v1l8p1_core_dp_reset(&phy_copy.access, 0));
        /* need to wait for the PLL lock */
        cnt = 0;
        while (cnt < 1000) {
            PHYMOD_IF_ERR_RETURN(blackhawk7_v1l8p1_pll_lock_get(&phy_copy.access, &pll_lock));
            cnt = cnt + 1;
            if (pll_lock) {
                break;
            } else {
                if (cnt == 1000) {
                    PHYMOD_DEBUG_ERROR(("WARNING :: core 0x%x PLL is not locked within 10 milli second \n", phy_copy.access.addr));
                    break;
                }
            }
            PHYMOD_USLEEP(10);
        }
    }
    return PHYMOD_E_NONE;
}

int tscbh_gen3_phy_autoneg_advert_ability_get(const phymod_phy_access_t* phy,
                                         phymod_autoneg_advert_abilities_t* an_advert_abilities)
{
    phymod_phy_access_t phy_copy;
    int start_lane, num_lane;
    uint32_t i;
    phymod_firmware_lane_config_t firmware_lane_config;

    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(&phy->access, &start_lane, &num_lane));
    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));
    phy_copy.access.lane_mask = 0x1 << start_lane;

    PHYMOD_IF_ERR_RETURN
        (tbhmod_gen3_autoneg_ability_get(&phy_copy.access, an_advert_abilities));

    /* Get Medium type from fw_lane_config */
    PHYMOD_IF_ERR_RETURN
        (tscbh_gen3_phy_firmware_lane_config_get(phy, &firmware_lane_config));

    for (i = 0; i < an_advert_abilities->num_abilities; i++) {
        an_advert_abilities->autoneg_abilities[i].medium = firmware_lane_config.MediaType;
    }

    return PHYMOD_E_NONE;
}

int tscbh_gen3_phy_autoneg_remote_advert_ability_get(const phymod_phy_access_t* phy,
                                                phymod_autoneg_advert_abilities_t* an_advert_abilities)
{
    phymod_phy_access_t phy_copy;
    int start_lane, num_lane, is_copper = 0;
    uint32_t i;

    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(&phy->access, &start_lane, &num_lane));
    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));
    phy_copy.access.lane_mask = 0x1 << start_lane;

    PHYMOD_IF_ERR_RETURN
        (tbhmod_gen3_autoneg_remote_ability_get(&phy_copy.access, an_advert_abilities));

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

    return PHYMOD_E_NONE;
}

int tscbh_gen3_phy_autoneg_set(const phymod_phy_access_t* phy, const phymod_autoneg_control_t* an)
{
    int num_lane_adv_encoded, mapped_speed_id;
    int start_lane, num_lane;
    int i, do_lane_config_set;
    uint32_t pll_div, vco_rate, refclk_in_hz;
    phymod_firmware_lane_config_t firmware_lane_config;
    tbhmod_gen3_an_control_t an_control;
    phymod_phy_access_t phy_copy;
    tbhmod_gen3_refclk_t ref_clk;
    tbhmod_gen3_spd_intfc_type_t spd_intf = 0;

    PHYMOD_MEMSET(&firmware_lane_config, 0x0, sizeof(firmware_lane_config));

    PHYMOD_MEMSET(&an_control, 0x0, sizeof(an_control));
    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(&phy->access, &start_lane, &num_lane));

    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));
    /* first set the port an mode enable bit */
    PHYMOD_IF_ERR_RETURN
        (tbhmod_gen3_port_an_mode_enable_set(&phy_copy.access, (int) an->enable));

    phy_copy.access.lane_mask = 0x1 << start_lane;

    if (an->enable) {
        PHYMOD_IF_ERR_RETURN(tbhmod_gen3_disable_set(&phy_copy.access));
        /* adjust FEC arch for RS544/RS272 */
        if (PHYMOD_AN_F_FEC_RS272_CLR_GET(an)) {
            if (start_lane < 4) {
                phy_copy.access.lane_mask = 0x1;
                PHYMOD_IF_ERR_RETURN(tbhmod_gen3_rsfec_cw_type(&phy_copy.access, 0, 0));
            }
            if ((start_lane >= 4) || (num_lane == 8)) {
                phy_copy.access.lane_mask = 0x1 << 4;
                PHYMOD_IF_ERR_RETURN(tbhmod_gen3_rsfec_cw_type(&phy_copy.access, 0, 0));
            }
        } else if (PHYMOD_AN_F_FEC_RS272_REQ_GET(an)) {
            if (start_lane < 4) {
                phy_copy.access.lane_mask = 0x1;
                PHYMOD_IF_ERR_RETURN(tbhmod_gen3_rsfec_cw_type(&phy_copy.access, 1, 0));
            }
            if ((start_lane >= 4) || (num_lane == 8)) {
                phy_copy.access.lane_mask = 0x1 << 4;
                PHYMOD_IF_ERR_RETURN(tbhmod_gen3_rsfec_cw_type(&phy_copy.access, 1, 0));
            }
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
            an_control.an_type = TBHMOD_GEN3_AN_MODE_CL73;
            break;
        case phymod_AN_MODE_CL73BAM:
            an_control.an_type = TBHMOD_GEN3_AN_MODE_CL73_BAM;
            break;
        case phymod_AN_MODE_CL73_MSA:
            an_control.an_type = TBHMOD_GEN3_AN_MODE_CL73_MSA;
            break;
        default:
            return PHYMOD_E_PARAM;
            break;
    }

    if (an->enable) {
        PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));
        /* Set AN port mode */
        PHYMOD_IF_ERR_RETURN
            (tbhmod_gen3_set_an_port_mode(&phy_copy.access, start_lane));

        /* Get TVCO rate (PLL1 for now) */
        PHYMOD_IF_ERR_RETURN
            (tbhmod_gen3_refclk_get(&phy_copy.access, &ref_clk));

        if (ref_clk == TBHMOD_GEN3_REF_CLK_312P5MHZ) {
            refclk_in_hz = 312500000;
        } else {
            refclk_in_hz = 156250000;
        }

        /* next read current TVCO pll divider*/
        phy_copy.access.pll_idx = 0;
        PHYMOD_IF_ERR_RETURN
            (blackhawk7_v1l8p1_INTERNAL_read_pll_div(&phy_copy.access, &pll_div));

        PHYMOD_IF_ERR_RETURN
            (blackhawk7_v1l8p1_get_vco_from_refclk_div(&phy_copy.access, refclk_in_hz, pll_div, &vco_rate, 0));

        if (vco_rate == 20625000) {
            /* load 20G VCO spd_id */
            spd_intf = TBHMOD_GEN3_SPD_CL73_IEEE_20G;
        } else if (vco_rate == 25781250) {
            /* load 25G VCO spd_id */
            spd_intf = TBHMOD_GEN3_SPD_CL73_IEEE_25G;
        } else if (vco_rate == 26562500) {
            /* load 26G VCO spd_id */
            spd_intf = TBHMOD_GEN3_SPD_CL73_IEEE_26G;
        } else {
            return PHYMOD_E_PARAM;
        }

        PHYMOD_IF_ERR_RETURN
            (tbhmod_gen3_get_mapped_speed(spd_intf, &mapped_speed_id));

        phy_copy.access.lane_mask = 0x1 << start_lane;

        /* Load 1G speed ID */
        PHYMOD_IF_ERR_RETURN
            (tbhmod_gen3_set_sc_speed(&phy_copy.access, mapped_speed_id, 0));
    }

    do_lane_config_set = 0;
    if (an->enable) {
        /* make sure the firmware config is set to an enabled */
        PHYMOD_IF_ERR_RETURN
            (tscbh_gen3_phy_firmware_lane_config_get(&phy_copy, &firmware_lane_config));
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
                (blackhawk7_v1l8p1_lane_soft_reset(&phy_copy.access, 1));
        }
        PHYMOD_USLEEP(1000);
        for (i = 0; i < num_lane; i++) {
            if (!PHYMOD_LANEPBMP_MEMBER(phy->access.lane_mask, start_lane + i)) {
                continue;
            }
            phy_copy.access.lane_mask = 0x1 << (i + start_lane);
            PHYMOD_IF_ERR_RETURN
                (_tscbh_gen3_phy_firmware_lane_config_set(&phy_copy, firmware_lane_config));
        }
        for (i = 0; i < num_lane; i++) {
            if (!PHYMOD_LANEPBMP_MEMBER(phy->access.lane_mask, start_lane + i)) {
                continue;
            }
            phy_copy.access.lane_mask = 0x1 << (i + start_lane);
            PHYMOD_IF_ERR_RETURN
                (blackhawk7_v1l8p1_lane_soft_reset(&phy_copy.access, 0));
        }
    }

    phy_copy.access.lane_mask = 0x1 << start_lane;

    PHYMOD_IF_ERR_RETURN
        (tbhmod_gen3_autoneg_control(&phy_copy.access, &an_control));

    return PHYMOD_E_NONE;
}

int tscbh_gen3_phy_autoneg_get(const phymod_phy_access_t* phy, phymod_autoneg_control_t* an, uint32_t* an_done)
{
    tbhmod_gen3_an_control_t an_control;
    phymod_phy_access_t phy_copy;
    int start_lane, num_lane;
    int an_complete = 0;

    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(&phy->access, &start_lane, &num_lane));

    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));
    phy_copy.access.lane_mask = 0x1 << start_lane;

    PHYMOD_MEMSET(&an_control, 0x0,  sizeof(tbhmod_gen3_an_control_t));
    PHYMOD_IF_ERR_RETURN
        (tbhmod_gen3_autoneg_control_get(&phy_copy.access, &an_control, &an_complete));

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
        case TBHMOD_GEN3_AN_MODE_CL73:
            an->an_mode = phymod_AN_MODE_CL73;
            break;
        case TBHMOD_GEN3_AN_MODE_CL73_BAM:
            an->an_mode = phymod_AN_MODE_CL73BAM;
            break;
        case TBHMOD_GEN3_AN_MODE_MSA:
            an->an_mode = phymod_AN_MODE_MSA;
            break;
        case TBHMOD_GEN3_AN_MODE_CL73_MSA:
            an->an_mode = phymod_AN_MODE_CL73_MSA;
            break;
        default:
            an->an_mode = phymod_AN_MODE_NONE;
            break;
    }

    return PHYMOD_E_NONE;
}

int tscbh_gen3_phy_autoneg_status_get(const phymod_phy_access_t* phy, phymod_autoneg_status_t* status)
{
    int an_en, an_done;
    phymod_phy_speed_config_t speed_config;
    phymod_phy_access_t phy_copy;
    int start_lane, num_lane, speed_id;
    uint32_t packed_entry[5];
    tbhmod_gen3_spd_id_tbl_entry_t speed_config_entry;

    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(&phy->access, &start_lane, &num_lane));

    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));
    phy_copy.access.lane_mask = 0x1 << start_lane;

    PHYMOD_IF_ERR_RETURN
       (tbhmod_gen3_autoneg_status_get(&phy_copy.access, &an_en, &an_done));

    PHYMOD_IF_ERR_RETURN
       (tscbh_gen3_phy_speed_config_get(phy, &speed_config));

    if (an_en && an_done) {
        uint32_t an_resolved_mode;
        /* if an resolves and link up */
        PHYMOD_IF_ERR_RETURN
            (tbhmod_gen3_speed_id_get(&phy_copy.access, &speed_id));
        /*read the speed id entry and get the num_lane info */
        phy_copy.access.lane_mask = 1 << start_lane;
        PHYMOD_IF_ERR_RETURN
            (phymod_mem_read(&phy_copy.access, phymodMemSpeedIdTable, speed_id, packed_entry));
        tbhmod_gen3_spd_ctrl_unpack_spd_id_tbl_entry(packed_entry, &speed_config_entry);
        num_lane = 1 << speed_config_entry.num_lanes;
        /* read the AN final resolved port mode */
        PHYMOD_IF_ERR_RETURN
            (tbhmod_gen3_resolved_port_mode_get(&phy_copy.access, &an_resolved_mode));
        status->resolved_port_mode = an_resolved_mode;
    }

    status->enabled   = an_en;
    status->locked    = an_done;
    status->data_rate = speed_config.data_rate;
    status->resolved_num_lane = num_lane;

    return PHYMOD_E_NONE;
}

int tscbh_gen3_phy_pll_reconfig(const phymod_phy_access_t* phy,
                                uint8_t pll_index,
                                uint32_t pll_div,
                                phymod_ref_clk_t ref_clk1)
{
    phymod_phy_access_t pm_phy_copy;
    tbhmod_gen3_refclk_t ref_clk;
    uint32_t tvco_rate = 0, speed_id_load_size, i, pll_is_pwrdn;
    enum blackhawk7_v1l8p1_pll_refclk_enum refclk;
    uint32_t cnt = 0, pll_lock = 0, fclk_div_mode = 1;

    PHYMOD_MEMCPY(&pm_phy_copy, phy, sizeof(pm_phy_copy));

    if (pll_index > 0) {
        PHYMOD_DEBUG_ERROR(("Unsupported PLL index\n"));
        return PHYMOD_E_UNAVAIL;
    }

    pm_phy_copy.access.lane_mask = 1 << 0;
    /* first needs to read the ref clock from main reg*/
    PHYMOD_IF_ERR_RETURN
        (tbhmod_gen3_refclk_get(&pm_phy_copy.access, &ref_clk));

    if (ref_clk == TBHMOD_GEN3_REF_CLK_156P25MHZ) {
        refclk = BLACKHAWK7_V1L8P1_PLL_REFCLK_156P25MHZ;
    } else if (ref_clk == TBHMOD_GEN3_REF_CLK_312P5MHZ) {
        refclk = BLACKHAWK7_V1L8P1_PLL_REFCLK_312P5MHZ;
    } else {
        PHYMOD_DEBUG_ERROR(("Unsupported reference clock.\n"));
        return PHYMOD_E_UNAVAIL;
    }

    PHYMOD_IF_ERR_RETURN
        (tbhmod_gen3_pll_to_vco_get(ref_clk, pll_div, &tvco_rate));

    /* next disable pcs datapath only if TVCO re-config*/
    PHYMOD_MEMCPY(&pm_phy_copy, phy, sizeof(pm_phy_copy));
    PHYMOD_IF_ERR_RETURN
        (tbhmod_gen3_disable_set(&pm_phy_copy.access));
    /* add the pcs disable SW WAR */
    PHYMOD_IF_ERR_RETURN
        (tbhmod_gen3_pcs_reset_sw_war(&phy->access));

    /*only need to load speed id table for tvco re-config */
    /*then reload the speed id table based on the new tvco */
    speed_id_load_size = TSCBH_GEN3_SPEED_ID_TABLE_SIZE > TSCBH_GEN3_HW_SPEED_ID_TABLE_SIZE ? TSCBH_GEN3_HW_SPEED_ID_TABLE_SIZE : TSCBH_GEN3_SPEED_ID_TABLE_SIZE;
    if (tvco_rate == TBHMOD_GEN3_VCO_26G) {
        for (i = 0; i < speed_id_load_size; i++) {
            PHYMOD_IF_ERR_RETURN
                (phymod_mem_write(&pm_phy_copy.access, phymodMemSpeedIdTable, i, &tscbh_gen3_spd_id_entry_26[i][0]));
        }
    } else if (tvco_rate == TBHMOD_GEN3_VCO_25G) {
        for (i = 0; i < speed_id_load_size; i++) {
            PHYMOD_IF_ERR_RETURN
                (phymod_mem_write(&pm_phy_copy.access, phymodMemSpeedIdTable, i, &tscbh_gen3_spd_id_entry_25[i][0]));
        }
    } else if (tvco_rate == TBHMOD_GEN3_VCO_20G) {
        for (i = 0; i < speed_id_load_size; i++) {
            PHYMOD_IF_ERR_RETURN
                (phymod_mem_write(&pm_phy_copy.access, phymodMemSpeedIdTable, i, &tscbh_gen3_spd_id_entry_20[i][0]));
        }
    }

    /*next check if the PLL is power down or not */
    pm_phy_copy.access.pll_idx = 0;
    pm_phy_copy.access.lane_mask = 1 << 0;
    PHYMOD_IF_ERR_RETURN
        (blackhawk7_v1l8p1_pll_pwrdn_get(&pm_phy_copy.access, &pll_is_pwrdn));

    /* if PLL is power down, need to power up first */
    if (pll_is_pwrdn) {
        /* Power up ovco if it's power down */
        PHYMOD_IF_ERR_RETURN
            (blackhawk7_v1l8p1_core_pwrdn(&pm_phy_copy.access, PWR_ON));
    }

    PHYMOD_IF_ERR_RETURN
        (blackhawk7_v1l8p1_core_dp_reset(&pm_phy_copy.access, 1));

    /*next re-config pll divider */
    PHYMOD_IF_ERR_RETURN
        (blackhawk7_v1l8p1_configure_pll_refclk_div(&pm_phy_copy.access,
                                                  refclk,
                                                  pll_div));

    PHYMOD_IF_ERR_RETURN
        (blackhawk7_v1l8p1_core_dp_reset(&pm_phy_copy.access, 0));

    /* Configure FCLK period. */
    PHYMOD_IF_ERR_RETURN
        (tbhmod_gen3_set_fclk_period(&pm_phy_copy.access, tvco_rate, fclk_div_mode));

    /* need to wait for the PLL lock */
    cnt = 0;
    while (cnt < 1000) {
        PHYMOD_IF_ERR_RETURN(blackhawk7_v1l8p1_pll_lock_get(&pm_phy_copy.access, &pll_lock));
        cnt = cnt + 1;
        if (pll_lock) {
            break;
        } else {
            if (cnt == 1000) {
                PHYMOD_DEBUG_ERROR(("WARNING :: core 0x%x PLL Index %d is not locked within 10 milli second \n", pm_phy_copy.access.addr, pll_index));
                break;
            }
        }
        PHYMOD_USLEEP(10);
    }

    return PHYMOD_E_NONE;
}

int tscbh_gen3_phy_tx_pam4_precoder_enable_set(const phymod_phy_access_t* phy, int enable)
{
    phymod_phy_access_t phy_copy;
    int start_lane, num_lane, i;
    uint32_t lane_reset, pcs_lane_enable;

    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(&phy->access, &start_lane, &num_lane));
    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));

    /*first check if lane is in reset */
    PHYMOD_IF_ERR_RETURN
        (blackhawk7_v1l8p1_lane_soft_reset_get(&phy_copy.access, &lane_reset));

    /*next check if PCS lane is in reset */
    PHYMOD_IF_ERR_RETURN
        (tbhmod_gen3_enable_get(&phy_copy.access, &pcs_lane_enable));

    /* disable pcs lane if pcs lane not in rset */
    if (pcs_lane_enable) {
        phy_copy.access.lane_mask = 1 << start_lane;
        PHYMOD_IF_ERR_RETURN
            (tbhmod_gen3_disable_set(&phy_copy.access));
        /* add the pcs disable SW WAR */
        PHYMOD_IF_ERR_RETURN
            (tbhmod_gen3_pcs_reset_sw_war(&phy->access));
    }

    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));
    /* if lane is not in reset, then reset the lane first */
    if (!lane_reset) {
        PHYMOD_IF_ERR_RETURN
            (blackhawk7_v1l8p1_lane_soft_reset(&phy_copy.access, 1));
    }

    for (i = 0; i < num_lane; i++) {
        if (!PHYMOD_LANEPBMP_MEMBER(phy->access.lane_mask, start_lane + i)) {
            continue;
        }
        phy_copy.access.lane_mask = 1 << (start_lane + i);
        PHYMOD_IF_ERR_RETURN
            (blackhawk7_v1l8p1_tx_pam4_precoder_enable_set(&phy_copy.access, enable));
    }

    /* release the ln dp reset */
    if (!lane_reset) {
        PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));
        PHYMOD_IF_ERR_RETURN
            (blackhawk7_v1l8p1_lane_soft_reset(&phy_copy.access, 0));
    }

    /* re-enable pcs lane if pcs lane not in rset */
    if (pcs_lane_enable) {
        phy_copy.access.lane_mask = 1 << start_lane;
        PHYMOD_IF_ERR_RETURN
            (tbhmod_gen3_enable_set(&phy_copy.access));
    }

    return PHYMOD_E_NONE;
}

int tscbh_gen3_phy_tx_pam4_precoder_enable_get(const phymod_phy_access_t* phy, int *enable)
{
    phymod_phy_access_t pm_phy_copy;

    PHYMOD_MEMCPY(&pm_phy_copy, phy, sizeof(pm_phy_copy));
    PHYMOD_IF_ERR_RETURN
        (blackhawk7_v1l8p1_tx_pam4_precoder_enable_get(&pm_phy_copy.access, enable));
    return PHYMOD_E_NONE;
}

/*Set/Get timesync enable*/
int tscbh_gen3_timesync_enable_set(const phymod_phy_access_t* phy, uint32_t flags, uint32_t enable)
{
    phymod_phy_access_t phy_copy;
    int start_lane, num_lane;
    uint32_t pcs_lane_enable;
    uint32_t fclk_div_mode = 1;

    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));
    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(&phy->access, &start_lane, &num_lane));

    phy_copy.access.lane_mask = 0x1 << start_lane;

    /* RX timestamping control */
    if (PHYMOD_TIMESYNC_ENABLE_F_RX_GET(flags)) {
        PHYMOD_IF_ERR_RETURN
            (tbhmod_gen3_pcs_rx_ts_en(&phy_copy.access, enable));
    }

    /* Core related control:
     * 1. Enable FCLK on PMD, with div_mode = 8T.
     */
    if (PHYMOD_TIMESYNC_ENABLE_F_CORE_GET(flags)) {
        PHYMOD_IF_ERR_RETURN
            (blackhawk7_v1l8p1_clk4sync_enable_set(&phy_copy.access, enable, fclk_div_mode));
    }

    /* 2. One-Step Timestmap Pipeline. */
    if (PHYMOD_TIMESYNC_ENABLE_F_ONE_STEP_PIPELINE_GET(flags)) {
        /* Check if PCS lane is in reset. */
        PHYMOD_IF_ERR_RETURN
            (tbhmod_gen3_enable_get(&phy_copy.access, &pcs_lane_enable));
        /* Disable PCS lane if PCS lane is not in reset. */
        if (pcs_lane_enable) {
            PHYMOD_IF_ERR_RETURN
                (tbhmod_gen3_disable_set(&phy_copy.access));
            /* Add PCS disable SW WAR. */
            PHYMOD_IF_ERR_RETURN
                (tbhmod_gen3_pcs_reset_sw_war(&phy->access));
        }

        PHYMOD_IF_ERR_RETURN
            (tbhmod_gen3_osts_pipeline(&phy_copy.access, enable));

        /* Re-enable PCS lane if PCS lane not in reset. */
        if (pcs_lane_enable) {
            PHYMOD_IF_ERR_RETURN
                (tbhmod_gen3_enable_set(&phy_copy.access));
        }
    }

    return PHYMOD_E_NONE;
}

/* Only one flag can be served each time.
 */
int tscbh_gen3_timesync_enable_get(const phymod_phy_access_t* phy, uint32_t flags, uint32_t* enable)
{
    phymod_phy_access_t phy_copy;
    uint32_t fclk_div_mode;

    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));

    if (PHYMOD_TIMESYNC_ENABLE_F_RX_GET(flags)) {
        PHYMOD_IF_ERR_RETURN
            (tbhmod_gen3_pcs_rx_ts_en_get(&phy_copy.access, enable));
    } else if (PHYMOD_TIMESYNC_ENABLE_F_ONE_STEP_PIPELINE_GET(flags)) {
        PHYMOD_IF_ERR_RETURN
            (tbhmod_gen3_osts_pipeline_get(&phy_copy.access, enable));
    } else if (PHYMOD_TIMESYNC_ENABLE_F_CORE_GET(flags)) {
        PHYMOD_IF_ERR_RETURN
            (blackhawk7_v1l8p1_clk4sync_enable_get(&phy_copy.access, enable, &fclk_div_mode));
    }

    return PHYMOD_E_NONE;
}

/*Set timesync adjust*/
int tscbh_gen3_timesync_adjust_set(const phymod_phy_access_t* phy, uint32_t flags, uint32_t ts_am_norm_mode)
{
    int start_lane, num_lane, i, is_pam4, is_sfd, reduced_preamble_mode, is_valid = 0;
    uint8_t fec_arch;
    int speed_id, mapped_speed_id;
    phymod_firmware_lane_config_t firmware_lane_config;
    int an_en, an_done, normalize_to_latest = 0;
    phymod_phy_access_t phy_copy;
    tbhmod_gen3_spd_id_tbl_entry_t speed_config_entry;
    uint32_t packed_entry[5];
    phymod_phy_speed_config_t speed_config;
    tbhmod_gen3_spd_intfc_type_t spd_intf;
    phymod_mem_type_t tx_mem, rx_mem;
    int ts_table_index;
    int osr_mode;
    tbhmod_gen3_ts_table_entry *ts_tx_entry = NULL, *ts_rx_entry = NULL;
    uint32_t ts_table_size;
    uint32_t pll_div;
    tbhmod_gen3_refclk_t ref_clk;
    uint32_t vco;
    int rx_mem_offset, tx_mem_offset;
    int tx_max_skew;
    uint32_t clk4sync_en, clk4sync_div;
    uint32_t lane_mask_backup;
    uint32_t rx_max_skew, rx_min_skew, rx_dsl_sel, rx_psll_sel;
    uint32_t skew_per_vl[20];

    if (ts_am_norm_mode == 0x2) {
        normalize_to_latest = 1;
    }

    if (PHYMOD_TIMESYNC_F_SOP_GET(flags)) {
        is_sfd = 0;
    } else {
        is_sfd = 1;
    }

    if (PHYMOD_TIMESYNC_F_REDUCED_PREAMBLE_MODE_GET(flags)) {
        reduced_preamble_mode = 1;
    } else {
        reduced_preamble_mode = 0;
    }

    PHYMOD_MEMSET(&firmware_lane_config,0,sizeof(firmware_lane_config));
    PHYMOD_IF_ERR_RETURN
        (tscbh_gen3_phy_firmware_lane_config_get(phy, &firmware_lane_config));
    if (firmware_lane_config.ForcePAM4Mode) {
        is_pam4 = 1;
    } else {
        is_pam4 = 0;
    }

    /* Save the lane mask for the logical port. */
    lane_mask_backup = phy->access.lane_mask;

    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));
    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(&phy->access, &start_lane, &num_lane));

    /* Here starts the sequence to enable Timestamping based on the current speed config. */

    /* Gen3 has a unified TX memory for both MPPs. */
    tx_mem_offset = (start_lane >> 1) * TBHMOD_GEN3_TS_DEFAULT_TABLE_SIZE;
    tx_mem = phymodMemTxLkup1588Mpp0;
    if (start_lane < 4) {
        /* MPP0 */
        rx_mem_offset = (start_lane >> 1) * TBHMOD_GEN3_TS_DEFAULT_TABLE_SIZE;
        rx_mem = phymodMemRxLkup1588Mpp0;
    } else {
        /* MPP1. */
        rx_mem_offset = ((start_lane - 4) >> 1) * TBHMOD_GEN3_TS_DEFAULT_TABLE_SIZE;
        rx_mem = phymodMemRxLkup1588Mpp1;
    }

    /* 1. Find default 1588 Table. */

    phy_copy.access.lane_mask = 0x1 << start_lane;
    /* 1.1 Get current speed id. */
    PHYMOD_IF_ERR_RETURN
        (tbhmod_gen3_speed_id_get(&phy_copy.access, &speed_id));

    PHYMOD_IF_ERR_RETURN
        (phymod_mem_read(&phy_copy.access, phymodMemSpeedIdTable, speed_id, packed_entry));

    tbhmod_gen3_spd_ctrl_unpack_spd_id_tbl_entry(packed_entry, &speed_config_entry);

    /* 1.2 Update num_lane and lane mask for AN port. */
    PHYMOD_IF_ERR_RETURN
        (tbhmod_gen3_autoneg_status_get(&phy_copy.access, &an_en, &an_done));
    if (an_en && an_done) {
        num_lane = 1 << speed_config_entry.num_lanes;
        /* Updaet lane_mask */
        lane_mask_backup = 0x0;
        for (i = 0; i < num_lane; i++) {
            lane_mask_backup |= 0x1 << (i + start_lane);
        }
    }

    /* 1.3 Get FEC type. */
    PHYMOD_IF_ERR_RETURN
        (_tscbh_gen3_speed_table_entry_to_speed_config_get(phy, &speed_config_entry, &speed_config));
    if (an_en && an_done) {
        PHYMOD_IF_ERR_RETURN
            (tbhmod_gen3_autoneg_fec_status_get(&phy_copy.access, &fec_arch));
        PHYMOD_IF_ERR_RETURN
            (tbhmod_gen3_fec_arch_decode_get(fec_arch, &(speed_config.fec_type)));
    }

    /* 1.4 Get mapped speed id. */
    if (speed_id <= 0x25) {
        /* AN speed IDs. */
        mapped_speed_id = speed_id;
    } else {
        /* Force speed speed IDs. */
        PHYMOD_IF_ERR_RETURN
            (_tscbh_gen3_phy_speed_id_set(num_lane, speed_config.data_rate,
                                       speed_config.fec_type, &spd_intf));
        PHYMOD_IF_ERR_RETURN
            (tbhmod_gen3_get_mapped_speed(spd_intf, &mapped_speed_id));
    }

    /* 1.5 Get the table index and table size of the 1588 table. */
    PHYMOD_IF_ERR_RETURN
        (tbhmod_gen3_1588_table_index_get(mapped_speed_id, speed_config.fec_type,
                                          &ts_table_index, &ts_table_size));

    if (ts_table_index == -1) {
        PHYMOD_DEBUG_ERROR(("1588 is not supported in current speed config.\n"));
        return PHYMOD_E_UNAVAIL;
    }

    /* 1.6 Find the 1588 table. */
    if (is_sfd) {
        ts_tx_entry = (tbhmod_gen3_ts_table_entry*) &(tbhmod_gen3_ts_table_tx_sfd[ts_table_index]);
        ts_rx_entry = (tbhmod_gen3_ts_table_entry*) &(tbhmod_gen3_ts_table_rx_sfd[ts_table_index]);
    } else {
        ts_tx_entry = (tbhmod_gen3_ts_table_entry*) &(tbhmod_gen3_ts_table_tx_sop[ts_table_index]);
        ts_rx_entry = (tbhmod_gen3_ts_table_entry*) &(tbhmod_gen3_ts_table_rx_sop[ts_table_index]);
    }

    /* Get OS mode. */
    PHYMOD_IF_ERR_RETURN
        (blackhawk7_v1l8p1_osr_mode_get(&phy_copy.access, &osr_mode));

    PHYMOD_IF_ERR_RETURN
        (tbhmod_gen3_refclk_get(&phy_copy.access, &ref_clk));

    /* Get current VCO. */
    PHYMOD_IF_ERR_RETURN
        (blackhawk7_v1l8p1_INTERNAL_read_pll_div(&phy_copy.access, &pll_div));
    PHYMOD_IF_ERR_RETURN
        (tbhmod_gen3_pll_to_vco_get(ref_clk, pll_div, &vco));

    /* 2. Load 1588 TX table. */
    phy_copy.access.lane_mask = 0x1;
    for (i = 0; i < (int) ts_table_size; i++) {
        PHYMOD_IF_ERR_RETURN
            (phymod_mem_write(&phy_copy.access, tx_mem, (i + tx_mem_offset), &(*ts_tx_entry)[i][0]));
    }

    /* 3. Load default 1588 RX table. */
    for (i = 0; i < (int)ts_table_size; i++) {
        PHYMOD_IF_ERR_RETURN
            (phymod_mem_write(&phy_copy.access, rx_mem, (i + rx_mem_offset), &(*ts_rx_entry)[i][0]));
    }

    phy_copy.access.lane_mask = 0x1 << start_lane;
    /* 4. Proram UI. */
    PHYMOD_IF_ERR_RETURN
        (blackhawk7_v1l8p1_clk4sync_enable_get(&phy_copy.access, &clk4sync_en, &clk4sync_div));

    /* Both UI and PMD latency are PMD lane based,
     * so the lane mask need to be the entire logical port's lane_mask.
     */
    phy_copy.access.lane_mask = lane_mask_backup;
    PHYMOD_IF_ERR_RETURN
        (tbhmod_gen3_pcs_set_1588_ui(&phy_copy.access, vco, osr_mode, is_pam4));

    /* 5. Program PMD latency. */
    PHYMOD_IF_ERR_RETURN
        (tbhmod_gen3_tx_pmd_latency_set(&phy_copy.access, vco, osr_mode, is_pam4));

    /* 6. Update deskew for TX and RX. */
    if (ts_am_norm_mode) {
        /* 6.1 Enable rx, tx deskew record.
         *     Need low-to-hight transition to trigger HW recording the current status.
         */
        phy_copy.access.lane_mask = 0x1 << start_lane;
        PHYMOD_IF_ERR_RETURN
            (tbhmod_gen3_pcs_rx_deskew_en(&phy_copy.access, 0));
        phy_copy.access.lane_mask = lane_mask_backup;
        PHYMOD_IF_ERR_RETURN
            (tbhmod_gen3_pcs_set_tx_lane_skew_capture(&phy_copy.access, 1));
        PHYMOD_USLEEP(5);
        PHYMOD_IF_ERR_RETURN
            (tbhmod_gen3_pcs_set_tx_lane_skew_capture(&phy_copy.access, 0));
        phy_copy.access.lane_mask = 0x1 << start_lane;
        PHYMOD_IF_ERR_RETURN
            (tbhmod_gen3_pcs_rx_deskew_en(&phy_copy.access, 1));

        /* 6.2 Update tx skew. */
        phy_copy.access.lane_mask = lane_mask_backup;
        PHYMOD_IF_ERR_RETURN
            (tbhmod_gen3_pcs_measure_tx_lane_skew(&phy_copy.access, vco,
                                                  osr_mode, is_pam4,
                                                  normalize_to_latest, &tx_max_skew));

        /* 6.3 Check for rx deskew valid. */
        for (i = 0; i < 1000; i++) {
            PHYMOD_USLEEP(10);
            PHYMOD_IF_ERR_RETURN
                (tbhmod_gen3_chk_rx_ts_deskew_valid(&phy_copy.access,
                                                    speed_config_entry.bit_mux_mode,
                                                    &is_valid));
            if (is_valid) {
                break;
            }
        }
        if (!is_valid) {
            return PHYMOD_E_TIMEOUT;
        }

        /* 6.4 Update RX deksew. */
        PHYMOD_IF_ERR_RETURN
            (tbhmod_gen3_mod_rx_1588_tbl_val(&phy_copy.access, speed_config_entry.bit_mux_mode,
                                             vco, osr_mode, is_pam4, normalize_to_latest,
                                             &rx_max_skew, &rx_min_skew, skew_per_vl,
                                             &rx_dsl_sel, &rx_psll_sel));
    }

    /* 7. Set SFD/SOP and byte offset. */
    phy_copy.access.lane_mask = 0x1 << start_lane;
    PHYMOD_IF_ERR_RETURN
        (tbhmod_gen3_pcs_ts_config(&phy_copy.access, is_sfd, reduced_preamble_mode));

    return PHYMOD_E_NONE;
}

int tscbh_gen3_phy_pcs_lane_swap_adjust(const phymod_phy_access_t* phy, uint32_t active_lane_map, uint32_t original_tx_lane_map, uint32_t original_rx_lane_map)
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
                (tbhmod_gen3_pcs_tx_lane_swap(&phy_copy.access, new_tx_lane_map));
            PHYMOD_IF_ERR_RETURN
                (tbhmod_gen3_pcs_rx_lane_swap(&phy_copy.access, new_rx_lane_map));
        } else { /* 4 lane port */
            for (i = 0; i < num_lane; i++) {
                tx_logical_lane = i + start_lane;
                rx_logical_lane = i + start_lane;
                tx_physical_lane = (original_tx_lane_map & (0xf << (tx_logical_lane * 4)) ) >> (tx_logical_lane * 4);
                rx_physical_lane = (original_rx_lane_map & (0xf << (rx_logical_lane * 4)) ) >> (rx_logical_lane * 4);
                tx_physical_lane &= 0xf;
                rx_physical_lane &= 0xf;
                PHYMOD_IF_ERR_RETURN
                    (tbhmod_gen3_pcs_single_tx_lane_swap_set(&phy_copy.access, tx_physical_lane, tx_logical_lane));
                PHYMOD_IF_ERR_RETURN
                    (tbhmod_gen3_pcs_single_rx_lane_swap_set(&phy_copy.access, rx_physical_lane, rx_logical_lane));
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
                    (tbhmod_gen3_pcs_single_tx_lane_swap_set(&phy_copy.access, tx_physical_lane, tx_logical_lane));
                PHYMOD_IF_ERR_RETURN
                    (tbhmod_gen3_pcs_single_rx_lane_swap_set(&phy_copy.access, rx_physical_lane, rx_logical_lane));
                /* mpp0 */
                tx_logical_lane = 3;
                rx_logical_lane = 3;
                tx_physical_lane = (original_tx_lane_map & (0xf << (disable_lane[0] * 4)) & 0xffff) >> (disable_lane[0] * 4);
                rx_physical_lane = (original_rx_lane_map & (0xf << (disable_lane[0] * 4)) & 0xffff) >> (disable_lane[0] * 4);
                tx_physical_lane &= 0xf;
                rx_physical_lane &= 0xf;
                PHYMOD_IF_ERR_RETURN
                    (tbhmod_gen3_pcs_single_tx_lane_swap_set(&phy_copy.access, tx_physical_lane, tx_logical_lane));
                PHYMOD_IF_ERR_RETURN
                    (tbhmod_gen3_pcs_single_rx_lane_swap_set(&phy_copy.access, rx_physical_lane, rx_logical_lane));
            } else {
                /* mpp1 */
                tx_logical_lane = disable_lane[0];
                rx_logical_lane = disable_lane[0];
                tx_physical_lane = (original_tx_lane_map & (0xf << (7 * 4)) & 0xffff0000) >> (7 * 4);
                rx_physical_lane = (original_rx_lane_map & (0xf << (7 * 4)) & 0xffff0000) >> (7 * 4);
                tx_physical_lane &= 0xf;
                rx_physical_lane &= 0xf;
                PHYMOD_IF_ERR_RETURN
                    (tbhmod_gen3_pcs_single_tx_lane_swap_set(&phy_copy.access, tx_physical_lane, tx_logical_lane));
                PHYMOD_IF_ERR_RETURN
                    (tbhmod_gen3_pcs_single_rx_lane_swap_set(&phy_copy.access, rx_physical_lane, rx_logical_lane));
                /* mpp1 */
                tx_logical_lane = 7;
                rx_logical_lane = 7;
                tx_physical_lane = (original_tx_lane_map & (0xf << (disable_lane[0] * 4)) & 0xffff0000) >> (disable_lane[0] * 4);
                rx_physical_lane = (original_rx_lane_map & (0xf << (disable_lane[0] * 4)) & 0xffff0000) >> (disable_lane[0] * 4);
                tx_physical_lane &= 0xf;
                rx_physical_lane &= 0xf;
                PHYMOD_IF_ERR_RETURN
                    (tbhmod_gen3_pcs_single_tx_lane_swap_set(&phy_copy.access, tx_physical_lane, tx_logical_lane));
                PHYMOD_IF_ERR_RETURN
                    (tbhmod_gen3_pcs_single_rx_lane_swap_set(&phy_copy.access, rx_physical_lane, rx_logical_lane));
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
                (tbhmod_gen3_pcs_tx_lane_swap(&phy_copy.access, new_tx_lane_map));
            PHYMOD_IF_ERR_RETURN
                (tbhmod_gen3_pcs_rx_lane_swap(&phy_copy.access, new_rx_lane_map));
        }
    }
    return PHYMOD_E_NONE;
}


int tscbh_gen3_phy_rx_ppm_get(const phymod_phy_access_t* phy, int16_t* rx_ppm)
{
    int start_lane, num_lane;
    phymod_phy_access_t pm_phy_copy;

    PHYMOD_MEMCPY(&pm_phy_copy, phy, sizeof(pm_phy_copy));

    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(&phy->access, &start_lane, &num_lane));

    pm_phy_copy.access.lane_mask = 1 << start_lane;
    PHYMOD_IF_ERR_RETURN
        (blackhawk7_v1l8p1_rx_ppm(&pm_phy_copy.access, rx_ppm));
    return PHYMOD_E_NONE;
}

int tscbh_gen3_phy_load_speed_id_entry(const phymod_phy_access_t* phy, uint32_t speed, uint32_t num_lane, phymod_fec_type_t fec_type)
{
    phymod_phy_access_t phy_copy;
    int start_lane, local_num_lane, mapped_speed_id;
    tbhmod_gen3_spd_intfc_type_t spd_intf;
    uint32_t tvco_pll_div;

    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(&phy->access, &start_lane, &local_num_lane));
    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));

    /* first check the speed */
    switch (speed) {
    case 150000:
        spd_intf = TBHMOD_GEN3_SPD_150G_FEC_544_2XN_N3;
        break;
    case 350000:
        spd_intf = TBHMOD_GEN3_SPD_350G_FEC_544_2XN_N7;
        break;
    case 300000:
        spd_intf = TBHMOD_GEN3_SPD_300G_FEC_544_2XN_N6;
        break;
    case 400000:
        if (fec_type == phymod_fec_RS544_2XN) {
            spd_intf = TBHMOD_GEN3_SPD_400G_BRCM_FEC_KR8_CR8;
        } else if (fec_type == phymod_fec_RS272_2XN) {
            spd_intf = TBHMOD_GEN3_SPD_400G_BRCM_FEC_272_N8;
        } else {
            PHYMOD_DEBUG_ERROR(("Unsupported 200G fec type\n"));
            return PHYMOD_E_UNAVAIL;
        }
        break;
    case 200000:
        if (fec_type == phymod_fec_RS544_2XN) {
            spd_intf = TBHMOD_GEN3_SPD_200G_IEEE_KR4_CR4;
        } else if (fec_type == phymod_fec_RS544) {
            spd_intf = TBHMOD_GEN3_SPD_200G_BRCM_KR4_CR4;
        } else if (fec_type == phymod_fec_RS272) {
            spd_intf = TBHMOD_GEN3_SPD_200G_BRCM_FEC_272_N4;
        } else if (fec_type == phymod_fec_RS272_2XN) {
            spd_intf = TBHMOD_GEN3_SPD_200G_BRCM_FEC_272_CR4_KR4;
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
        (tbhmod_gen3_get_mapped_speed(spd_intf, &mapped_speed_id));

    /* Get TVCO because it's not allowed to change during speed set */
    phy_copy.access.pll_idx = 0;

    PHYMOD_IF_ERR_RETURN
        (blackhawk7_v1l8p1_INTERNAL_read_pll_div(&phy_copy.access, &tvco_pll_div));

    /* based on the current TVCO PLL div, decide which copy of speed id entry to load */
    /* first set the lane mask to be 0x1 */
    phy_copy.access.lane_mask = 1 << 0;
    if (((int) tvco_pll_div == TBHMOD_GEN3_PLL_MODE_DIV_170) || ((int) tvco_pll_div == TBHMOD_GEN3_PLL_MODE_DIV_85))  {
        /* then load 26G TVCO speed id entry */
        PHYMOD_IF_ERR_RETURN
            (phymod_mem_write(&phy_copy.access, phymodMemSpeedIdTable, TSCBH_GEN3_FORCED_SPEED_ID_OFFSET + start_lane, &tscbh_gen3_spd_id_entry_26[mapped_speed_id][0]));
    } else if (((int)tvco_pll_div == TBHMOD_GEN3_PLL_MODE_DIV_165) || ((int) tvco_pll_div == TBHMOD_GEN3_PLL_MODE_DIV_82P5)) {
        /* then load 25G TVCO speed id entry */
        PHYMOD_IF_ERR_RETURN
            (phymod_mem_write(&phy_copy.access, phymodMemSpeedIdTable, TSCBH_GEN3_FORCED_SPEED_ID_OFFSET + start_lane, &tscbh_gen3_spd_id_entry_25[mapped_speed_id][0]));
    } else {
        /* then load 20G TVCO speed id entry */
        PHYMOD_IF_ERR_RETURN
            (phymod_mem_write(&phy_copy.access, phymodMemSpeedIdTable, TSCBH_GEN3_FORCED_SPEED_ID_OFFSET + start_lane, &tscbh_gen3_spd_id_entry_20[mapped_speed_id][0]));
    }

    return PHYMOD_E_NONE;
}

int tscbh_gen3_phy_pll_powerdown_get(const phymod_phy_access_t* phy, uint32_t pll_index, uint32_t* powerdown)
{
    phymod_phy_access_t phy_copy;

    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));

    phy_copy.access.pll_idx = 0;
    phy_copy.access.lane_mask = 1 << 0;
    PHYMOD_IF_ERR_RETURN
        (blackhawk7_v1l8p1_pll_pwrdn_get(&phy_copy.access, powerdown));

    return PHYMOD_E_NONE;
}

int tscbh_gen3_timesync_tx_info_get(const phymod_phy_access_t* phy, phymod_ts_fifo_status_t* ts_tx_info)
{
    phymod_phy_access_t phy_copy;
    int start_lane, num_lane, port_index;
    uint32_t ts_info[3];

    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));

    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(&phy->access, &start_lane, &num_lane));
    phy_copy.access.lane_mask = 0x1;
    port_index = start_lane >> 1;

    PHYMOD_IF_ERR_RETURN
        (phymod_mem_read(&phy_copy.access, phymodMemTxTwostep1588Ts, port_index, ts_info));
    PHYMOD_IF_ERR_RETURN
        (tbhmod_gen3_tx_ts_info_unpack_tx_ts_tbl_entry(ts_info, ts_tx_info));

    return PHYMOD_E_NONE;
}

int tscbh_gen3_phy_autoneg_speed_id_table_reload(const phymod_phy_access_t* phy, uint32_t gsh_header_enable)
{
    phymod_phy_access_t phy_copy;
    uint32_t tvco_pll_div, i;

    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));

    /*first read the current TVCO rate */
    phy_copy.access.pll_idx = 0;

    PHYMOD_IF_ERR_RETURN
        (blackhawk7_v1l8p1_INTERNAL_read_pll_div(&phy_copy.access, &tvco_pll_div));

    /* based on the current TVCO PLL div, decide which copy of speed id entry to load */
    /* first set the lane mask to be 0x1 */
    phy_copy.access.lane_mask = 1 << 0;
    /* need to reload autoneg speed id table based on gsh enable or not */
    if (((int) tvco_pll_div == TBHMOD_GEN3_PLL_MODE_DIV_170) || ((int) tvco_pll_div == TBHMOD_GEN3_PLL_MODE_DIV_85))  {
        /* then load 26G TVCO speed id entry */
        for (i = 0; i < TSCBH_GEN3_AUTONEG_SPEED_ID_COUNT; i++) {
            if (gsh_header_enable) {
                PHYMOD_IF_ERR_RETURN
                    (phymod_mem_write(&phy_copy.access, phymodMemSpeedIdTable, i, &tscbh_gen3_spd_id_entry_26_gsh[i][0]));
            } else {
                PHYMOD_IF_ERR_RETURN
                    (phymod_mem_write(&phy_copy.access, phymodMemSpeedIdTable, i, &tscbh_gen3_spd_id_entry_26[i][0]));
            }
        }
    } else if (((int) tvco_pll_div == TBHMOD_GEN3_PLL_MODE_DIV_165) || ((int) tvco_pll_div == TBHMOD_GEN3_PLL_MODE_DIV_82P5)) {
        /* then load 25G TVCO speed id entry */
        for (i = 0; i < TSCBH_GEN3_AUTONEG_SPEED_ID_COUNT; i++) {
            if (gsh_header_enable) {
                PHYMOD_IF_ERR_RETURN
                    (phymod_mem_write(&phy_copy.access, phymodMemSpeedIdTable, i, &tscbh_gen3_spd_id_entry_25_gsh[i][0]));
            } else {
                PHYMOD_IF_ERR_RETURN
                    (phymod_mem_write(&phy_copy.access, phymodMemSpeedIdTable, i, &tscbh_gen3_spd_id_entry_25[i][0]));
            }
        }
    } else {
        /* then load 20G TVCO speed id entry */
        for (i = 0; i < TSCBH_GEN3_AUTONEG_SPEED_ID_COUNT; i++) {
            if (gsh_header_enable) {
                PHYMOD_IF_ERR_RETURN
                    (phymod_mem_write(&phy_copy.access, phymodMemSpeedIdTable, i, &tscbh_gen3_spd_id_entry_20_gsh[i][0]));
            } else {
                PHYMOD_IF_ERR_RETURN
                    (phymod_mem_write(&phy_copy.access, phymodMemSpeedIdTable, i, &tscbh_gen3_spd_id_entry_20[i][0]));
            }
        }
    }
    return PHYMOD_E_NONE;
}

int tscbh_gen3_phy_pcs_enable_set(const phymod_phy_access_t* phy, uint32_t enable)
{
    phymod_phy_access_t phy_copy;
    int start_lane, num_lane;

    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(&phy->access, &start_lane, &num_lane));
    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));

    if (enable == 1) {
        phy_copy.access.lane_mask = 1 << start_lane;
        PHYMOD_IF_ERR_RETURN
            (tbhmod_gen3_enable_set(&phy_copy.access));
    } else if (enable == 0) {
        PHYMOD_IF_ERR_RETURN
            (tbhmod_gen3_disable_set(&phy_copy.access));
    }

    return PHYMOD_E_NONE;
}


int tscbh_gen3_phy_synce_clk_ctrl_set(const phymod_phy_access_t* phy,
                                 phymod_synce_clk_ctrl_t cfg)
{
    return PHYMOD_E_NONE;
}

int tscbh_gen3_phy_synce_clk_ctrl_get(const phymod_phy_access_t* phy,
                                 phymod_synce_clk_ctrl_t *cfg)
{
    return PHYMOD_E_NONE;
}

int tscbh_gen3_intr_handler(const phymod_phy_access_t* phy,
                       phymod_interrupt_type_t type,
                       uint32_t* is_handled)
{
    phymod_phy_access_t phy_copy;
    tbhmod_gen3_intr_status_t intr_status;
    uint32_t speed_id_table_entry[TSCBH_GEN3_SPEED_ID_ENTRY_SIZE];
    uint32_t ts_table_entry[TBHMOD_GEN3_TS_ENTRY_SIZE];
    int index;

    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));
    intr_status.type = type;

    /* Get and clear interrupt status. */
    PHYMOD_IF_ERR_RETURN
        (tbhmod_gen3_intr_status_get(&phy_copy.access, &intr_status));

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
                if (index >= TSCBH_GEN3_HW_AM_TABLE_SIZE) {
                    return PHYMOD_E_FAIL;
                }
                PHYMOD_IF_ERR_RETURN
                    (phymod_mem_write(&phy_copy.access, phymodMemAMTable, index,
                                      &tscbh_gen3_am_table_entry[index][0]));
                *is_handled = 1;
            }
            break;
        case phymodIntrEccSpeedTable:
            if (intr_status.is_1b_err) {
                index = intr_status.err_addr;
                if (index >= TSCBH_GEN3_HW_SPEED_ID_TABLE_SIZE) {
                    return PHYMOD_E_FAIL;
                }
                PHYMOD_IF_ERR_RETURN
                    (phymod_mem_read(&phy_copy.access, phymodMemSpeedIdTable,
                                     index, speed_id_table_entry));
                PHYMOD_IF_ERR_RETURN
                    (phymod_mem_write(&phy_copy.access, phymodMemSpeedIdTable,
                                      index, speed_id_table_entry));
            }
            break;
        case phymodIntrEccUMTable:
            if (intr_status.is_2b_err || intr_status.is_1b_err) {
                index = intr_status.err_addr;
                if (index >= TSCBH_GEN3_HW_UM_TABLE_SIZE) {
                    return PHYMOD_E_FAIL;
                }
                PHYMOD_IF_ERR_RETURN
                    (phymod_mem_write(&phy_copy.access, phymodMemUMTable, index,
                                      &tscbh_gen3_um_table_entry[index][0]));
                *is_handled = 1;
            }
            break;
        case phymodIntrEccRx1588Mpp1:
            if (intr_status.is_1b_err) {
                index = intr_status.err_addr;
                if (index >= TBHMOD_GEN3_TS_RX_MPP_MEM_SIZE) {
                    return PHYMOD_E_FAIL;
                }
                PHYMOD_IF_ERR_RETURN
                    (phymod_mem_read(&phy_copy.access, phymodMemRxLkup1588Mpp1,
                                     index, ts_table_entry));
                PHYMOD_IF_ERR_RETURN
                    (phymod_mem_write(&phy_copy.access, phymodMemRxLkup1588Mpp1,
                                      index, ts_table_entry));
            }
            break;
        case phymodIntrEccRx1588Mpp0:
            if (intr_status.is_1b_err) {
                index = intr_status.err_addr;
                if (index >= TBHMOD_GEN3_TS_RX_MPP_MEM_SIZE) {
                    return PHYMOD_E_FAIL;
                }
                PHYMOD_IF_ERR_RETURN
                    (phymod_mem_read(&phy_copy.access, phymodMemRxLkup1588Mpp0,
                                     index, ts_table_entry));
                PHYMOD_IF_ERR_RETURN
                    (phymod_mem_write(&phy_copy.access, phymodMemRxLkup1588Mpp0,
                                      index, ts_table_entry));
            }
            break;
        case phymodIntrEccTx1588Mpp0:
            /*
             * Tscbh_gen3 only has one TX 1588 memory.
             * Here we use phymodIntrEccTx1588Mpp0 to represent
             * TX 1588 ECC error type.
             * phymodIntrEccTx1588Mpp1 will not be handled in gen3 driver.
             */
            if (intr_status.is_1b_err) {
                index = intr_status.err_addr;
                if (index >= TBHMOD_GEN3_TS_TX_MEM_SIZE) {
                    return PHYMOD_E_FAIL;
                }
                PHYMOD_IF_ERR_RETURN
                    (phymod_mem_read(&phy_copy.access, phymodMemTxLkup1588Mpp0,
                                     index, ts_table_entry));
                PHYMOD_IF_ERR_RETURN
                    (phymod_mem_write(&phy_copy.access, phymodMemTxLkup1588Mpp0,
                                      index, ts_table_entry));
            }
            break;
        default:
            break;
    }

    return PHYMOD_E_NONE;
}

int tscbh_gen3_phy_codec_mode_set(const phymod_phy_access_t* phy, phymod_phy_codec_mode_t codec_type)
{
    phymod_phy_access_t phy_copy;
    tbhmod_gen3_spd_id_tbl_entry_t speed_config_entry;
    uint32_t packed_entry[5];
    int start_lane, num_lane;
    uint32_t lane_reset, pcs_lane_enable;

    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(&phy->access, &start_lane, &num_lane));
    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));

    /*first check if lane is in reset */
    PHYMOD_IF_ERR_RETURN
        (blackhawk7_v1l8p1_lane_soft_reset_get(&phy_copy.access, &lane_reset));

    /*next check if PCS lane is in reset */
    PHYMOD_IF_ERR_RETURN
        (tbhmod_gen3_enable_get(&phy_copy.access, &pcs_lane_enable));

    /* disable pcs lane if pcs lane not in rset */
    if (pcs_lane_enable) {
        phy_copy.access.lane_mask = 1 << start_lane;
        PHYMOD_IF_ERR_RETURN
            (tbhmod_gen3_disable_set(&phy_copy.access));

        /* add the pcs disable SW WAR */
        PHYMOD_IF_ERR_RETURN
            (tbhmod_gen3_pcs_reset_sw_war(&phy->access));
    }

    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));
    /* if lane is not in reset, then reset the lane first */
    if (!lane_reset) {
        PHYMOD_IF_ERR_RETURN
            (blackhawk7_v1l8p1_lane_soft_reset(&phy_copy.access, 1));
    }

    /* first read the current speed id entry for this port */
    PHYMOD_IF_ERR_RETURN
        (phymod_mem_read(&phy_copy.access, phymodMemSpeedIdTable, TSCBH_GEN3_FORCED_SPEED_ID_OFFSET + start_lane, packed_entry));

    tbhmod_gen3_spd_ctrl_unpack_spd_id_tbl_entry(packed_entry, &speed_config_entry);

    /* update the codec field */
    speed_config_entry.codec_mode = codec_type;

    /* next pack the speed config entry into 5 word format */
    tbhmod_gen3_spd_ctrl_pack_spd_id_tbl_entry(&speed_config_entry, &packed_entry[0]);

    /* write back to the speed id HW table */
    PHYMOD_IF_ERR_RETURN
        (phymod_mem_write(&phy_copy.access, phymodMemSpeedIdTable, TSCBH_GEN3_FORCED_SPEED_ID_OFFSET + start_lane, &packed_entry[0]));

    /* release the ln dp reset */
    if (!lane_reset) {
        PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));
        PHYMOD_IF_ERR_RETURN
            (blackhawk7_v1l8p1_lane_soft_reset(&phy_copy.access, 0));
    }

    /* re-enable pcs lane if pcs lane not in rset */
    if (pcs_lane_enable) {
        phy_copy.access.lane_mask = 1 << start_lane;
        PHYMOD_IF_ERR_RETURN
            (tbhmod_gen3_enable_set(&phy_copy.access));
    }

    return PHYMOD_E_NONE;
}


int tscbh_gen3_phy_codec_mode_get(const phymod_phy_access_t* phy, phymod_phy_codec_mode_t* codec_type)
{
    phymod_phy_access_t phy_copy;
    tbhmod_gen3_spd_id_tbl_entry_t speed_config_entry;
    uint32_t packed_entry[5];
    int start_lane, num_lane, speed_id;

    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(&phy->access, &start_lane, &num_lane));
    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));

    phy_copy.access.lane_mask = 1 << start_lane;

    /* first read speed id from resolved status */
    PHYMOD_IF_ERR_RETURN
        (tbhmod_gen3_speed_id_get(&phy_copy.access, &speed_id));

    /* first read the current speed id entry for this port */
    PHYMOD_IF_ERR_RETURN
        (phymod_mem_read(&phy_copy.access, phymodMemSpeedIdTable, speed_id, packed_entry));

    tbhmod_gen3_spd_ctrl_unpack_spd_id_tbl_entry(packed_entry, &speed_config_entry);

    /* the codeec type */
    *codec_type = speed_config_entry.codec_mode;

    return PHYMOD_E_NONE;
}

int tscbh_gen3_phy_fec_bypass_indication_set(const phymod_phy_access_t* phy,
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
        (blackhawk7_v1l8p1_lane_soft_reset_get(&phy_copy.access, &lane_reset));

    /*next check if PCS lane is in reset */
    PHYMOD_IF_ERR_RETURN
        (tbhmod_gen3_enable_get(&phy_copy.access, &pcs_lane_enable));

    /* disable pcs lane if pcs lane not in rset */
    if (pcs_lane_enable) {
        phy_copy.access.lane_mask = 1 << start_lane;
        PHYMOD_IF_ERR_RETURN
            (tbhmod_gen3_disable_set(&phy_copy.access));

        /* add the pcs disable SW WAR */
        PHYMOD_IF_ERR_RETURN
            (tbhmod_gen3_pcs_reset_sw_war(&phy->access));
    }

    /* if lane is not in reset, then reset the lane first */
    if (!lane_reset) {
        PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));
        PHYMOD_IF_ERR_RETURN
            (blackhawk7_v1l8p1_lane_soft_reset(&phy_copy.access, 1));
    }

    phy_copy.access.lane_mask = 1 << start_lane;
    PHYMOD_IF_ERR_RETURN
      (tbhmod_gen3_fec_bypass_indication_set(&phy_copy.access, enable));

    /* release the lane soft reset bit */
    if (!lane_reset) {
        PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));
        PHYMOD_IF_ERR_RETURN
            (blackhawk7_v1l8p1_lane_soft_reset(&phy_copy.access, 0));
    }

    /* re-enable pcs lane if pcs lane not in rset */
    if (pcs_lane_enable) {
        phy_copy.access.lane_mask = 1 << start_lane;
        PHYMOD_IF_ERR_RETURN
            (tbhmod_gen3_enable_set(&phy_copy.access));
    }

    return PHYMOD_E_NONE;
}


int tscbh_gen3_phy_fec_bypass_indication_get(const phymod_phy_access_t* phy,
                                        uint32_t *enable)
{
    int start_lane, num_lane;
    phymod_phy_access_t phy_copy;

    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));

    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(&phy->access, &start_lane, &num_lane));

    phy_copy.access.lane_mask = 1 << start_lane;

    PHYMOD_IF_ERR_RETURN
      (tbhmod_gen3_fec_bypass_indication_get(&phy_copy.access, enable));

    return PHYMOD_E_NONE;
}


int tscbh_gen3_phy_rs_fec_rxp_get(const phymod_phy_access_t* phy, uint32_t* hi_ser_lh, uint32_t* hi_ser_live)
{
    int start_lane, num_lane;
    phymod_phy_access_t phy_copy;

    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));

    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(&phy->access, &start_lane, &num_lane));

    phy_copy.access.lane_mask = 1 << start_lane;

    PHYMOD_IF_ERR_RETURN
        (tbhmod_gen3_rs_fec_hi_ser_get(&phy_copy.access, hi_ser_lh, hi_ser_live));

    return PHYMOD_E_NONE;
}

int tscbh_gen3_phy_pmd_override_enable_set(const phymod_phy_access_t* phy,
                                           phymod_override_type_t pmd_override_type,
                                           uint32_t override_enable,
                                           uint32_t override_val)
{
    phymod_phy_access_t phy_copy;

    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));
    PHYMOD_IF_ERR_RETURN
        (tbhmod_gen3_pmd_override_enable_set(&phy_copy.access, pmd_override_type, override_enable, override_val));

    return PHYMOD_E_NONE;
}

int tscbh_gen3_phy_interrupt_enable_set(const phymod_phy_access_t* phy,
                                        phymod_interrupt_type_t intr_type,
                                        uint32_t enable)
{
    phymod_phy_access_t phy_copy;

    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));
    PHYMOD_IF_ERR_RETURN(tbhmod_gen3_interrupt_enable_set(&phy_copy.access,
                                                          intr_type,  enable));
    return PHYMOD_E_NONE;
}



int tscbh_gen3_phy_interrupt_enable_get(const phymod_phy_access_t* phy,
                                        phymod_interrupt_type_t intr_type,
                                        uint32_t* enable)
{
    phymod_phy_access_t phy_copy;

    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));
    PHYMOD_IF_ERR_RETURN(tbhmod_gen3_interrupt_enable_get(&phy_copy.access,
                                                          intr_type, enable));
    return PHYMOD_E_NONE;
}

