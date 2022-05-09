/*
 *
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 *
 */

#include <phymod/phymod.h>
#include <phymod/phymod_system.h>
#include <phymod/phymod.h>
#include <phymod/phymod_dispatch.h>
#include <phymod/phymod_util.h>

#ifdef PHYMOD_TSCBH_SUB10G_SUPPORT

#include <phymod/chip/tscbh_sub10g.h>
#include <phymod/chip/bcmi_tscbh_sub10g_xgxs_defs.h>

#include "blackhawk7_l8p2/tier2/blackhawk7_l8p2_ucode.h"
#include "blackhawk7_l8p2/tier1/blackhawk7_l8p2_cfg_seq.h"
#include "blackhawk7_l8p2/tier1/blackhawk7_l8p2_enum.h"
#include "blackhawk7_l8p2/tier1/blackhawk7_l8p2_common.h"
#include "blackhawk7_l8p2/tier1/blackhawk7_l8p2_interface.h"
#include "blackhawk7_l8p2/tier1/blackhawk7_l8p2_dependencies.h"
#include "blackhawk7_l8p2/tier1/blackhawk7_l8p2_internal.h"
#include "blackhawk7_l8p2/tier1/public/blackhawk7_api_uc_vars_rdwr_defns_public.h"
#include "blackhawk7_l8p2/tier1/blackhawk7_l8p2_access.h"

#include <tscbh_sub10g/tier1/tbhmod_sub10g.h>


#define TSCBH_SUB10G_ID0        0x600d
#define TSCBH_SUB10G_ID1        0x8770
#define TSCBH_SUB10G_SERDES_ID    0x31 /* 0x900e Main0_serdesID - Serdes ID Register */


int tscbh_sub10g_core_identify(const phymod_core_access_t* core, uint32_t core_id, uint32_t* is_identified)
{
    PHYID2r_t id2;
    PHYID3r_t id3;
    TSCE_MAIN0_SERDESIDr_t serdesid;
    int ioerr = 0;

    *is_identified = 0;
    ioerr += READ_PHYID2r(&core->access, &id2);
    ioerr += READ_PHYID3r(&core->access, &id3);

    if (PHYID2r_REGID1f_GET(id2) == TSCBH_SUB10G_ID0 &&
       (PHYID3r_REGID2f_GET(id3) == TSCBH_SUB10G_ID1)) {
        /* PHY IDs match - now check PCS model */
        ioerr += READ_TSCE_MAIN0_SERDESIDr(&core->access, &serdesid);
        if ((TSCE_MAIN0_SERDESIDr_MODEL_NUMBERf_GET(serdesid)) == TSCBH_SUB10G_SERDES_ID)  {
            *is_identified = 1;
        }
    }
    return ioerr ? PHYMOD_E_IO : PHYMOD_E_NONE;
}




int tscbh_sub10g_core_info_get(const phymod_core_access_t* core, phymod_core_info_t* info)
{
    int rv = 0;
    TSCE_MAIN0_SERDESIDr_t serdes_id;
    char core_name[15] = "Tscbh_sub10g";
    PHYID2r_t id2;
    PHYID3r_t id3;
    const phymod_access_t *pm_acc = &core->access;

    rv = READ_TSCE_MAIN0_SERDESIDr(&core->access, &serdes_id);

    info->serdes_id = TSCE_MAIN0_SERDESIDr_GET(serdes_id);
    PHYMOD_IF_ERR_RETURN
        (phymod_core_name_get(core, TSCE_MAIN0_SERDESIDr_GET(serdes_id), core_name, info));
    info->serdes_id = TSCE_MAIN0_SERDESIDr_GET(serdes_id);
    info->core_version = phymodCoreVersionTscbh_sub10g;

    PHYMOD_IF_ERR_RETURN(READ_PHYID2r(pm_acc, &id2));
    PHYMOD_IF_ERR_RETURN(READ_PHYID3r(pm_acc, &id3));

    info->phy_id0 = (uint16_t) id2.v[0];
    info->phy_id1 = (uint16_t) id3.v[0];

    return rv;
}



/*
 * set lane swapping for core
 */

int tscbh_sub10g_core_lane_map_set(const phymod_core_access_t* core, const phymod_lane_map_t* lane_map)
{
    phymod_core_access_t  core_copy;
    uint32_t lane, pcs_tx_swap = 0, pcs_rx_swap = 0;
    uint32_t pcs_tx_swap_pm1 = 0, pcs_rx_swap_pm1 = 0;
    uint8_t pmd_tx_addr[8], pmd_rx_addr[8];

    if (lane_map->num_of_lanes != TSCBH_SUB10G_NOF_LANES_IN_CORE) {
        return PHYMOD_E_CONFIG;
    }

    PHYMOD_MEMCPY(&core_copy, core, sizeof(core_copy));

    /*
        * XLPORT0 will connect to BH7 lane 0-3.
        * XLPORT1 will connect to BH7 lane 4-7.
        * Lane swap is supported on a Quad basis.
        */

    for (lane = 0; lane < TSCBH_SUB10G_NOF_LANES_IN_CORE; lane++) {
        if (lane < 4) {
            if ((lane_map->lane_map_tx[lane] >= 4) ||
                (lane_map->lane_map_rx[lane] >= 4)) {
                return PHYMOD_E_CONFIG;
            }
        } else {
            if ((lane_map->lane_map_tx[lane] >= 8) ||
                (lane_map->lane_map_tx[lane] < 4) ||
                (lane_map->lane_map_rx[lane] >= 8) ||
                (lane_map->lane_map_rx[lane] <4)) {
                return PHYMOD_E_CONFIG;
            }
        }
        /*encode each lane as four bits*/
        pcs_tx_swap += lane_map->lane_map_tx[lane]<<(lane*4);
        pcs_rx_swap += lane_map->lane_map_rx[lane]<<(lane*4);
        if (lane >= 4) {
            pcs_tx_swap_pm1 += (lane_map->lane_map_tx[lane] - 4) << ((lane - 4) * 4);
            pcs_rx_swap_pm1 += (lane_map->lane_map_rx[lane] - 4) << ((lane - 4) * 4);
        }
    }
    /* PMD lane addr is based on PCS logical to physical mapping*/
    for (lane = 0; lane < TSCBH_SUB10G_NOF_LANES_IN_CORE; lane++){
        pmd_tx_addr[((pcs_tx_swap >> (lane*4)) & 0xf)] = lane;
        pmd_rx_addr[((pcs_rx_swap >> (lane*4)) & 0xf)] = lane;
    }

    /* config lane map on pm4x10_0*/
    core_copy.access.lane_mask = 0x1;
    PHYMOD_IF_ERR_RETURN
        (temod2pll_pcs_rx_lane_swap(&core_copy.access, pcs_rx_swap & 0xffff));
    PHYMOD_IF_ERR_RETURN
        (temod2pll_pcs_tx_lane_swap(&core_copy.access, pcs_tx_swap & 0xffff));

    /* config lane map on pm4x10_1*/
    core_copy.access.lane_mask = 0x10;
        PHYMOD_IF_ERR_RETURN
        (temod2pll_pcs_rx_lane_swap(&core_copy.access, pcs_rx_swap_pm1));
    PHYMOD_IF_ERR_RETURN
        (temod2pll_pcs_tx_lane_swap(&core_copy.access, pcs_tx_swap_pm1));

    PHYMOD_IF_ERR_RETURN
        (blackhawk7_l8p2_map_lanes(&core_copy.access, TSCBH_SUB10G_NOF_LANES_IN_CORE, pmd_tx_addr, pmd_rx_addr));

    return PHYMOD_E_NONE;
}


int tscbh_sub10g_core_lane_map_get(const phymod_core_access_t* core, phymod_lane_map_t* lane_map)
{
    phymod_core_access_t  core_copy;
    uint32_t tx_lane_map, rx_lane_map;
    int i = 0;

    PHYMOD_MEMCPY(&core_copy, core, sizeof(core_copy));
    core_copy.access.lane_mask = 0x1;

    PHYMOD_IF_ERR_RETURN
        (blackhawk7_l8p2_pmd_lane_map_get(&core_copy.access, &tx_lane_map, &rx_lane_map));

    /*next get the lane map into serdes spi format */
    lane_map->num_of_lanes = TSCBH_SUB10G_NOF_LANES_IN_CORE;
    for (i = 0; i < TSCBH_SUB10G_NOF_LANES_IN_CORE; i++) {
        lane_map->lane_map_tx[tx_lane_map >> (4 * i) & 0xf] = i;
        lane_map->lane_map_rx[rx_lane_map >> (4 * i) & 0xf] = i;
    }

    return PHYMOD_E_NONE;
}

STATIC int
_tscbh_sub10g_phy_lane_swap_validate(const phymod_phy_access_t* phy)
{
    int i;
    phymod_lane_map_t lane_map;
    phymod_core_access_t core;

    PHYMOD_MEMCPY(&core, phy, sizeof(phymod_core_access_t));

    PHYMOD_IF_ERR_RETURN
        (tscbh_sub10g_core_lane_map_get(&core, &lane_map));

    for (i = 0; i < TSCBH_SUB10G_NOF_LANES_IN_CORE; i++) {
         if (!PHYMOD_LANEPBMP_MEMBER(phy->access.lane_mask, i)) {
            continue;
         }

         if (lane_map.lane_map_tx[i] != lane_map.lane_map_rx[i]) {
             PHYMOD_DEBUG_WARN(("Warning: core_addr 0x%x, TX lane %d is mapped to %d, while RX lane %d is mapped to %d!\n",
                                core.access.addr, i, lane_map.lane_map_tx[i], i, lane_map.lane_map_rx[i]));
             return TRUE;
         }
    }

    return FALSE;
}


int tscbh_sub10g_core_reset_set(const phymod_core_access_t* core, phymod_reset_mode_t reset_mode, phymod_reset_direction_t direction)
{

    /* Place your code here */


    return PHYMOD_E_NONE;

}

int tscbh_sub10g_core_reset_get(const phymod_core_access_t* core, phymod_reset_mode_t reset_mode, phymod_reset_direction_t* direction)
{

    /* Place your code here */


    return PHYMOD_E_NONE;

}

static
int _tscbh_sub10g_phy_firmware_lane_config_set(const phymod_phy_access_t* phy, phymod_firmware_lane_config_t fw_config)
{
    uint32_t is_warm_boot;
    struct blackhawk7_l8p2_uc_lane_config_st serdes_firmware_config;
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
            PHYMOD_IF_ERR_RETURN(blackhawk7_l8p2_lane_soft_reset_get(&phy_copy.access, &rst_status));
            if (!rst_status) PHYMOD_IF_ERR_RETURN (blackhawk7_l8p2_lane_soft_reset(&phy_copy.access, 1));
            PHYMOD_IF_ERR_RETURN(blackhawk7_l8p2_set_uc_lane_cfg(&phy_copy.access, serdes_firmware_config));
            if (!rst_status) PHYMOD_IF_ERR_RETURN (blackhawk7_l8p2_lane_soft_reset(&phy_copy.access, 0));
        }
    }

    return PHYMOD_E_NONE;
}


int tscbh_sub10g_phy_firmware_lane_config_set(const phymod_phy_access_t* phy, phymod_firmware_lane_config_t fw_lane_config)
{
    phymod_phy_access_t phy_copy;
    uint32_t pcs_lane_enable;
    int port_start_lane, port_num_lane;

    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));

    /*get the start lane of the port lane mask */
    PHYMOD_IF_ERR_RETURN
        (tbhmod_sub10g_port_start_lane_get(&phy_copy.access, &port_start_lane, &port_num_lane));

    /*check if PCS lane is in reset */
    phy_copy.access.lane_mask = 1 << port_start_lane;
    PHYMOD_IF_ERR_RETURN
        (temod2pll_disable_get(&phy_copy.access, &pcs_lane_enable));

    /* disable pcs lane if pcs lane not in rset */
    if (pcs_lane_enable) {
        phy_copy.access.lane_mask = 1 << port_start_lane;
        PHYMOD_IF_ERR_RETURN
            (temod2pll_disable_set(&phy_copy.access));
    }

    PHYMOD_IF_ERR_RETURN
         (_tscbh_sub10g_phy_firmware_lane_config_set(phy, fw_lane_config));

    /* re-enable pcs lane if pcs lane not in rset */
    if (pcs_lane_enable) {
        phy_copy.access.lane_mask = 1 << port_start_lane;
        PHYMOD_IF_ERR_RETURN
            (tbhmod_sub10g_enable_set(&phy_copy.access));
    }

    return PHYMOD_E_NONE;
}



int tscbh_sub10g_phy_firmware_lane_config_get(const phymod_phy_access_t* phy, phymod_firmware_lane_config_t* fw_lane_config)
{
    struct blackhawk7_l8p2_uc_lane_config_st lane_config;
    phymod_phy_access_t pm_phy_copy;

    PHYMOD_MEMCPY(&pm_phy_copy, phy, sizeof(pm_phy_copy));

    PHYMOD_MEMSET(&lane_config, 0x0, sizeof(lane_config));
    PHYMOD_MEMSET(fw_lane_config, 0, sizeof(*fw_lane_config));

    PHYMOD_IF_ERR_RETURN
        (blackhawk7_l8p2_get_uc_lane_cfg(&pm_phy_copy.access, &lane_config));

    fw_lane_config->LaneConfigFromPCS     = lane_config.field.lane_cfg_from_pcs;
    fw_lane_config->AnEnabled             = lane_config.field.an_enabled;
    fw_lane_config->DfeOn                 = lane_config.field.dfe_on;
    fw_lane_config->LpDfeOn               = lane_config.field.dfe_lp_mode;
    fw_lane_config->ForceBrDfe            = lane_config.field.force_brdfe_on;
    fw_lane_config->MediaType             = lane_config.field.media_type;
    fw_lane_config->UnreliableLos         = lane_config.field.unreliable_los;
    fw_lane_config->Cl72AutoPolEn         = lane_config.field.cl72_auto_polarity_en;
    fw_lane_config->ScramblingDisable     = lane_config.field.scrambling_dis;
    fw_lane_config->Cl72RestTO            = lane_config.field.cl72_restart_timeout_en;
    fw_lane_config->ForceExtenedReach     = lane_config.field.force_er;
    fw_lane_config->ForceNormalReach      = lane_config.field.force_nr;
    fw_lane_config->LpPrecoderEnabled     = lane_config.field.lp_has_prec_en;
    fw_lane_config->ForcePAM4Mode         = lane_config.field.force_pam4_mode;
    fw_lane_config->ForceNRZMode          = lane_config.field.force_nrz_mode;

    return PHYMOD_E_NONE;
}



int tscbh_sub10g_phy_rx_restart(const phymod_phy_access_t* phy)
{

    /* Place your code here */


    return PHYMOD_E_NONE;

}


int tscbh_sub10g_phy_polarity_set(const phymod_phy_access_t* phy, const phymod_polarity_t* polarity)
{
    phymod_phy_access_t phy_copy;
    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));

    PHYMOD_IF_ERR_RETURN
        (blackhawk7_l8p2_tx_rx_polarity_set(&phy_copy.access, polarity->tx_polarity, polarity->rx_polarity));

    return PHYMOD_E_NONE;

}

int tscbh_sub10g_phy_polarity_get(const phymod_phy_access_t* phy, phymod_polarity_t* polarity)
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
            (blackhawk7_l8p2_tx_rx_polarity_get(&phy_copy.access, &temp_pol.tx_polarity, &temp_pol.rx_polarity));
        polarity->tx_polarity |= ((temp_pol.tx_polarity & 0x1) << i);
        polarity->rx_polarity |= ((temp_pol.rx_polarity & 0x1) << i);
    }

    return PHYMOD_E_NONE;

}


int tscbh_sub10g_phy_tx_set(const phymod_phy_access_t* phy, const phymod_tx_t* tx)
{
    phymod_phy_access_t phy_copy;
    int start_lane, num_lane, i, port_start_lane, port_num_lane, rv = PHYMOD_E_NONE;
    uint32_t lane_reset, pcs_lane_enable;

    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(&phy->access, &start_lane, &num_lane));
    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));

    /*get the start lane of the port lane mask */
    PHYMOD_IF_ERR_RETURN
        (tbhmod_sub10g_port_start_lane_get(&phy_copy.access, &port_start_lane, &port_num_lane));

    phy_copy.access.lane_mask = 1 << port_start_lane;

    /*next check if PCS lane is in reset */
    PHYMOD_IF_ERR_RETURN
        (temod2pll_disable_get(&phy_copy.access, &pcs_lane_enable));

    /*first check if lane is in reset */
    phy_copy.access.lane_mask = 1 << start_lane;
    PHYMOD_IF_ERR_RETURN
        (blackhawk7_l8p2_lane_soft_reset_get(&phy_copy.access, &lane_reset));

    /* disable pcs lane if pcs lane not in rset */
    if (pcs_lane_enable) {
        phy_copy.access.lane_mask = 1 << port_start_lane;
        PHYMOD_IF_ERR_RETURN
            (temod2pll_disable_set(&phy_copy.access));
    }
    if (!lane_reset) {
        PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));
        PHYMOD_IF_ERR_RETURN
            (blackhawk7_l8p2_lane_soft_reset(&phy_copy.access, 1));
    }

    if (tx->sig_method != phymodSignallingMethodNRZ) {
        PHYMOD_DEBUG_ERROR(("ERROR :: only support SignallingMethodNRZ \n"));
        return PHYMOD_E_PARAM;
    }

    for (i = 0; i < num_lane; i++) {
        if (!PHYMOD_LANEPBMP_MEMBER(phy->access.lane_mask, start_lane + i)) {
            continue;
        }
        phy_copy.access.lane_mask = 1 << (start_lane + i);
        /*next check 3 tap mode or 6 tap mode */
        if (tx->tap_mode == phymodTxTapMode3Tap) {
            /* rv will be returned at the end of function */
            rv |= blackhawk7_l8p2_apply_txfir_cfg(&phy_copy.access,
                                                 BLACKHAWK7_L8P2_NRZ_LP_3TAP,
                                                 0,
                                                 tx->pre,
                                                 tx->main,
                                                 tx->post,
                                                 0,
                                                 0);
        } else {
            /* rv will be returned at the end of function */
            rv |= blackhawk7_l8p2_apply_txfir_cfg(&phy_copy.access,
                                                 BLACKHAWK7_L8P2_NRZ_6TAP,
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
            (blackhawk7_l8p2_lane_soft_reset(&phy_copy.access, 0));
    }

    /* re-enable pcs lane if pcs lane not in rset */
    if (pcs_lane_enable) {
        phy_copy.access.lane_mask = 1 << port_start_lane;
        PHYMOD_IF_ERR_RETURN
            (tbhmod_sub10g_enable_set(&phy_copy.access));
    }
    return rv;

}

int tscbh_sub10g_phy_tx_get(const phymod_phy_access_t* phy, phymod_tx_t* tx)
{
    uint8_t pmd_tx_tap_mode;
    int16_t val;
    phymod_phy_access_t phy_copy;
    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));

    /* read current tx tap mode */
    PHYMOD_IF_ERR_RETURN
        (blackhawk7_l8p2_tx_tap_mode_get(&phy_copy.access, &pmd_tx_tap_mode));

    if (pmd_tx_tap_mode == 0) {
        /* 3 tap mode */
        tx->tap_mode = phymodTxTapMode3Tap;
    } else {
        tx->tap_mode = phymodTxTapMode6Tap;
    }
    tx->sig_method = phymodSignallingMethodNRZ;

    /*next check 3 tap mode or 6 tap mode */
    if (tx->tap_mode == phymodTxTapMode3Tap) {
        PHYMOD_IF_ERR_RETURN
            (blackhawk7_l8p2_read_tx_afe(&phy_copy.access, BLACKHAWK7_L8P2_TX_AFE_TAP0, &val));
            tx->pre = val;
        PHYMOD_IF_ERR_RETURN
            (blackhawk7_l8p2_read_tx_afe(&phy_copy.access, BLACKHAWK7_L8P2_TX_AFE_TAP1, &val));
            tx->main = val;
        PHYMOD_IF_ERR_RETURN
            (blackhawk7_l8p2_read_tx_afe(&phy_copy.access, BLACKHAWK7_L8P2_TX_AFE_TAP2, &val));
            tx->post = val;
            tx->pre2 = 0;
            tx->post2 = 0;
            tx->post3 = 0;
    } else {
        PHYMOD_IF_ERR_RETURN
            (blackhawk7_l8p2_read_tx_afe(&phy_copy.access, BLACKHAWK7_L8P2_TX_AFE_TAP0, &val));
            tx->pre2 = val;
        PHYMOD_IF_ERR_RETURN
            (blackhawk7_l8p2_read_tx_afe(&phy_copy.access, BLACKHAWK7_L8P2_TX_AFE_TAP1, &val));
            tx->pre = val;
        PHYMOD_IF_ERR_RETURN
            (blackhawk7_l8p2_read_tx_afe(&phy_copy.access, BLACKHAWK7_L8P2_TX_AFE_TAP2, &val));
            tx->main = val;
        PHYMOD_IF_ERR_RETURN
            (blackhawk7_l8p2_read_tx_afe(&phy_copy.access, BLACKHAWK7_L8P2_TX_AFE_TAP3, &val));
            tx->post = val;
        PHYMOD_IF_ERR_RETURN
            (blackhawk7_l8p2_read_tx_afe(&phy_copy.access, BLACKHAWK7_L8P2_TX_AFE_TAP4, &val));
            tx->post2 = val;
        PHYMOD_IF_ERR_RETURN
            (blackhawk7_l8p2_read_tx_afe(&phy_copy.access, BLACKHAWK7_L8P2_TX_AFE_TAP5, &val));
            tx->post3 = val;
    }

    return PHYMOD_E_NONE;

}


int tscbh_sub10g_phy_media_type_tx_get(const phymod_phy_access_t* phy, phymod_media_typed_t media, phymod_tx_t* tx)
{

    /* Place your code here */


    return PHYMOD_E_NONE;

}


int tscbh_sub10g_phy_reset_set(const phymod_phy_access_t* phy, const phymod_phy_reset_t* reset)
{

    /* Place your code here */


    return PHYMOD_E_NONE;

}

int tscbh_sub10g_phy_reset_get(const phymod_phy_access_t* phy, phymod_phy_reset_t* reset)
{

    /* Place your code here */


    return PHYMOD_E_NONE;

}


int tscbh_sub10g_phy_power_set(const phymod_phy_access_t* phy, const phymod_phy_power_t* power)
{

    /* Place your code here */


    return PHYMOD_E_NONE;

}

int tscbh_sub10g_phy_power_get(const phymod_phy_access_t* phy, phymod_phy_power_t* power)
{

    /* Place your code here */


    return PHYMOD_E_NONE;

}


int tscbh_sub10g_phy_hg2_codec_control_set(const phymod_phy_access_t* phy, phymod_phy_hg2_codec_t hg2_codec)
{
    temod2pll_hg2_codec_t local_copy;
    switch (hg2_codec) {
        case phymodBcmHG2CodecOff:
            local_copy = TEMOD2PLL_HG2_CODEC_OFF;
            break;
        case phymodBcmHG2CodecOnWith8ByteIPG:
            local_copy = TEMOD2PLL_HG2_CODEC_ON_8BYTE_IPG;
            break;
        case phymodBcmHG2CodecOnWith9ByteIPG:
            local_copy = TEMOD2PLL_HG2_CODEC_ON_9BYTE_IPG;
            break;
        default:
            local_copy = TEMOD2PLL_HG2_CODEC_OFF;
            break;
        }
    PHYMOD_IF_ERR_RETURN(temod2pll_hg2_codec_set(&phy->access, local_copy));
    return PHYMOD_E_NONE;
}

int tscbh_sub10g_phy_hg2_codec_control_get(const phymod_phy_access_t* phy, phymod_phy_hg2_codec_t* hg2_codec)
{
    temod2pll_hg2_codec_t local_copy;

    PHYMOD_IF_ERR_RETURN(temod2pll_hg2_codec_get(&phy->access, &local_copy));

    switch (local_copy) {
        case TEMOD2PLL_HG2_CODEC_OFF:
            *hg2_codec = phymodBcmHG2CodecOff;
            break;
        case TEMOD2PLL_HG2_CODEC_ON_8BYTE_IPG:
            *hg2_codec = phymodBcmHG2CodecOnWith8ByteIPG;
            break;
        case TEMOD2PLL_HG2_CODEC_ON_9BYTE_IPG:
            *hg2_codec = phymodBcmHG2CodecOnWith9ByteIPG;
            break;
        default:
            *hg2_codec = phymodBcmHG2CodecOff;
            break;
        }
    return PHYMOD_E_NONE;
}


int tscbh_sub10g_phy_tx_lane_control_set(const phymod_phy_access_t* phy, phymod_phy_tx_lane_control_t tx_control)
{
    phymod_phy_access_t pm_phy_copy;
    PHYMOD_MEMCPY(&pm_phy_copy, phy, sizeof(pm_phy_copy));

    switch (tx_control) {
    case phymodTxTrafficDisable:
        PHYMOD_IF_ERR_RETURN(temod2pll_tx_lane_control_set(&pm_phy_copy.access, TEMOD2PLL_TX_LANE_TRAFFIC_DISABLE));
        break;
    case phymodTxTrafficEnable:
        PHYMOD_IF_ERR_RETURN(temod2pll_tx_lane_control_set(&pm_phy_copy.access, TEMOD2PLL_TX_LANE_TRAFFIC_ENABLE));
        break;
    case phymodTxReset:
        PHYMOD_IF_ERR_RETURN(temod2pll_tx_lane_control_set(&pm_phy_copy.access, TEMOD2PLL_TX_LANE_RESET));
        break;
    case phymodTxSquelchOn:
        PHYMOD_IF_ERR_RETURN(blackhawk7_l8p2_tx_disable(&pm_phy_copy.access, 1));
        break;
    case phymodTxSquelchOff:
        PHYMOD_IF_ERR_RETURN(blackhawk7_l8p2_tx_disable(&pm_phy_copy.access, 0));
        break;
    default:
        break;
    }
    return PHYMOD_E_NONE;

}

int tscbh_sub10g_phy_tx_lane_control_get(const phymod_phy_access_t* phy, phymod_phy_tx_lane_control_t* tx_control)
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

    PHYMOD_IF_ERR_RETURN(blackhawk7_l8p2_tx_disable_get(&pm_phy_copy.access, &tx_disable));

    /* next check if PMD loopback is on */
    if (tx_disable) {
        PHYMOD_IF_ERR_RETURN(blackhawk7_l8p2_dig_lpbk_get(&pm_phy_copy.access, &lb_enable));
        if (lb_enable) tx_disable = 0;
    }

    if(tx_disable) {
        *tx_control = phymodTxSquelchOn;
    } else {
        PHYMOD_IF_ERR_RETURN(temod2pll_tx_lane_control_get(&pm_phy_copy.access, &reset, &tx_lane));
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


int tscbh_sub10g_phy_rx_lane_control_set(const phymod_phy_access_t* phy, phymod_phy_rx_lane_control_t rx_control)
{
    phymod_phy_access_t pm_phy_copy;

    PHYMOD_MEMCPY(&pm_phy_copy, phy, sizeof(pm_phy_copy));

    switch (rx_control) {
        case phymodRxReset:
            PHYMOD_IF_ERR_RETURN(temod2pll_rx_lane_control_set(&phy->access, 1));
            break;
        case phymodRxSquelchOn:
            PHYMOD_IF_ERR_RETURN(blackhawk7_l8p2_pmd_force_signal_detect(&pm_phy_copy.access, 1, 0));
            break;
        case phymodRxSquelchOff:
            PHYMOD_IF_ERR_RETURN(blackhawk7_l8p2_pmd_force_signal_detect(&pm_phy_copy.access, 0, 0));
            break;
        default:
            return PHYMOD_E_PARAM;
    }

    return PHYMOD_E_NONE;

}

int tscbh_sub10g_phy_rx_lane_control_get(const phymod_phy_access_t* phy, phymod_phy_rx_lane_control_t* rx_control)
{
    int reset, rx_squelch_enable;
    uint32_t lb_enable;
    uint8_t force_en, force_val;
    phymod_phy_access_t pm_phy_copy;

    PHYMOD_MEMCPY(&pm_phy_copy, phy, sizeof(pm_phy_copy));

    /* first get the force enabled bit and forced value */
    PHYMOD_IF_ERR_RETURN(blackhawk7_l8p2_pmd_force_signal_detect_get(&pm_phy_copy.access, &force_en, &force_val));

    if (force_en & (!force_val)) {
        rx_squelch_enable = 1;
    } else {
        rx_squelch_enable = 0;
    }

    /* next check if PMD loopback is on */
    if (rx_squelch_enable) {
        PHYMOD_IF_ERR_RETURN(blackhawk7_l8p2_dig_lpbk_get(&pm_phy_copy.access, &lb_enable));
        if (lb_enable) rx_squelch_enable = 0;
    }
    if(rx_squelch_enable) {
        *rx_control = phymodRxSquelchOn;
    } else {
        PHYMOD_IF_ERR_RETURN(temod2pll_rx_lane_control_get(&pm_phy_copy.access, &reset));
        if (reset == 0) {
            *rx_control = phymodRxReset;
        } else {
            *rx_control = phymodRxSquelchOff;
        }
    }
    return PHYMOD_E_NONE;
}


int tscbh_sub10g_phy_fec_enable_set(const phymod_phy_access_t* phy, uint32_t enable)
{
    PHYMOD_IF_ERR_RETURN(temod2pll_fecmode_set(&phy->access, enable));
    return PHYMOD_E_NONE;
}

int tscbh_sub10g_phy_fec_enable_get(const phymod_phy_access_t* phy, uint32_t* enable)
{
    PHYMOD_IF_ERR_RETURN(temod2pll_fecmode_get(&phy->access, enable));
    return PHYMOD_E_NONE;
}


int tscbh_sub10g_phy_autoneg_oui_set(const phymod_phy_access_t* phy, phymod_autoneg_oui_t an_oui)
{
    temod2pll_an_oui_t oui;

    oui.oui                    = an_oui.oui;
    oui.oui_override_hpam_adv  = an_oui.oui_override_hpam_adv;
    oui.oui_override_hpam_det  = an_oui.oui_override_hpam_det;
    oui.oui_override_bam73_adv = an_oui.oui_override_bam73_adv;
    oui.oui_override_bam73_det = an_oui.oui_override_bam73_det;
    oui.oui_override_bam37_adv = an_oui.oui_override_bam37_adv;
    oui.oui_override_bam37_det = an_oui.oui_override_bam37_det;

    PHYMOD_IF_ERR_RETURN(temod2pll_an_oui_set(&phy->access, oui));

    return PHYMOD_E_NONE;
}

int tscbh_sub10g_phy_autoneg_oui_get(const phymod_phy_access_t* phy, phymod_autoneg_oui_t* an_oui)
{
    temod2pll_an_oui_t oui;

    PHYMOD_IF_ERR_RETURN(temod2pll_an_oui_get(&phy->access, &oui));

    an_oui->oui_override_hpam_adv  = oui.oui_override_hpam_adv;
    an_oui->oui_override_hpam_det  = oui.oui_override_hpam_det;
    an_oui->oui_override_bam73_adv = oui.oui_override_bam73_adv;
    an_oui->oui_override_bam73_det = oui.oui_override_bam73_det;
    an_oui->oui_override_bam37_adv = oui.oui_override_bam37_adv;
    an_oui->oui_override_bam37_det = oui.oui_override_bam37_det;

    return PHYMOD_E_NONE;
}


int tscbh_sub10g_phy_eee_set(const phymod_phy_access_t* phy, uint32_t enable)
{
    uint32_t lpi_bypass;
    int rv = PHYMOD_E_NONE ;
    lpi_bypass = PHYMOD_LPI_BYPASS_GET(enable);
    enable &= 0x1;
    if (lpi_bypass) {
        rv = temod2pll_eee_control_set(&phy->access,enable);
    } else {
        return PHYMOD_E_UNAVAIL;
    }
    return rv;
}

int tscbh_sub10g_phy_eee_get(const phymod_phy_access_t* phy, uint32_t* enable)
{
    if (PHYMOD_LPI_BYPASS_GET(*enable)) {
        PHYMOD_IF_ERR_RETURN(temod2pll_eee_control_get(&phy->access, enable));
        PHYMOD_LPI_BYPASS_SET(*enable);
    } else {
        return PHYMOD_E_UNAVAIL;
    }
    return PHYMOD_E_NONE;
}


int tscbh_sub10g_phy_interface_config_set(const phymod_phy_access_t* phy, uint32_t flags, const phymod_phy_inf_config_t* config)
{
    uint16_t new_speed_vec=0;
    temod2pll_spd_intfc_type spd_intf = TEMOD2PLL_SPD_ILLEGAL;
    phymod_phy_access_t pm_phy_copy;
    int start_lane=0, num_lane, i;
    uint32_t os_dfeon=0;
    uint32_t scrambling_dis=0;
    uint32_t os_mode = 0;
    int cl72_allowed=0, cl72_req=0, cl72 = 0;
    uint32_t  tvco_pll_index, ovco_pll_index, tvco_pll_div = 0, ovco_pll_div = 0, request_pll_div = 0, pll_index = 0;
    uint32_t ovco_is_pwrdn, tvco_is_pwrdn = 0;

    /* sc_table_entry exp_entry; RAVI */
    phymod_firmware_lane_config_t firmware_lane_config;

    PHYMOD_MEMCPY(&pm_phy_copy, phy, sizeof(pm_phy_copy));
    PHYMOD_MEMSET(&firmware_lane_config, 0x0, sizeof(firmware_lane_config));

    /*first make sure that tvco pll index is valid */
    if (phy->access.tvco_pll_index > 1) {
        PHYMOD_DEBUG_ERROR(("Unsupported tvco index\n"));
        return PHYMOD_E_UNAVAIL;
    }

    tvco_pll_index = phy->access.tvco_pll_index;
    ovco_pll_index = tvco_pll_index ? 0 : 1;

    firmware_lane_config.MediaType = 0;

    /* next program the tx fir taps and driver current based on the input */
    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(&phy->access, &start_lane, &num_lane));

    /* reset pcs */
    PHYMOD_IF_ERR_RETURN
        (temod2pll_disable_set(&phy->access));

    /* Hold the per lne soft reset bit */
    for (i = 0; i < num_lane; i++) {
        if (!PHYMOD_LANEPBMP_MEMBER(phy->access.lane_mask, start_lane + i)) {
            continue;
        }
        pm_phy_copy.access.lane_mask = 1 << (start_lane + i);
        PHYMOD_IF_ERR_RETURN
            (blackhawk7_l8p2_lane_soft_reset(&pm_phy_copy.access, 1));
    }
    /* deassert pmd_tx_disable_pin_dis if it is set by ILKn */
    /* remove pmd_tx_disable_pin_dis it may be asserted because of ILKn */
    if (config->interface_type != phymodInterfaceBypass) {
        for (i = 0; i < num_lane; i++) {
            if (!PHYMOD_LANEPBMP_MEMBER(phy->access.lane_mask, start_lane + i)) {
                continue;
            }
            pm_phy_copy.access.lane_mask = 1 << (start_lane + i);
            PHYMOD_IF_ERR_RETURN
                (blackhawk7_l8p2_pmd_tx_disable_pin_dis_set(&pm_phy_copy.access, 0));
        }
    }

    pm_phy_copy.access.lane_mask = 0x1 << start_lane;
     PHYMOD_IF_ERR_RETURN
        (tscbh_sub10g_phy_firmware_lane_config_get(&pm_phy_copy, &firmware_lane_config));

    /* make sure that an and config from pcs is off */
    firmware_lane_config.AnEnabled = 0;
    firmware_lane_config.LaneConfigFromPCS = 0;

    /* and also make sure the cl72 restart timeout is enabled */
    firmware_lane_config.Cl72RestTO = 1;
    firmware_lane_config.Cl72AutoPolEn = 0;
    if (PHYMOD_INTF_MODES_FIBER_GET(config)) {
        firmware_lane_config.MediaType = phymodFirmwareMediaTypeOptics;
    } else if (PHYMOD_INTF_MODES_COPPER_GET(config)) {
        firmware_lane_config.MediaType = phymodFirmwareMediaTypeCopperCable;
    } else {
        firmware_lane_config.MediaType = phymodFirmwareMediaTypePcbTraceBackPlane;
    }

    /* find the speed */
    if (config->interface_type == phymodInterfaceXFI) {
        firmware_lane_config.MediaType = phymodFirmwareMediaTypePcbTraceBackPlane;
    } else if(config->interface_type == phymodInterfaceSFI) {
        firmware_lane_config.MediaType = phymodFirmwareMediaTypeOptics;
    } else if(config->interface_type == phymodInterfaceSFPDAC) {
        firmware_lane_config.MediaType = phymodFirmwareMediaTypeCopperCable;
    } else if(config->interface_type == phymodInterface1000X) {
        firmware_lane_config.MediaType = phymodFirmwareMediaTypeOptics;
    } else if(config->interface_type == phymodInterfaceSGMII) {
        firmware_lane_config.MediaType = phymodFirmwareMediaTypePcbTraceBackPlane;
    } else if(config->interface_type == phymodInterfaceLR) {
        firmware_lane_config.MediaType = phymodFirmwareMediaTypeOptics;
    } else if(config->interface_type == phymodInterfaceSR) {
        firmware_lane_config.MediaType = phymodFirmwareMediaTypeOptics;
    }

    if (config->interface_type != phymodInterfaceBypass) {
        PHYMOD_IF_ERR_RETURN
            (tbhmod_sub10g_update_port_mode(&phy->access));
    }

    spd_intf = TEMOD2PLL_SPD_10_X1_SGMII; /* to prevent undefinded TEMOD2PLL_SPD_ILLEGAL accessing tables */

    switch(config->data_rate) {
    case 10:
        spd_intf = TEMOD2PLL_SPD_10_SGMII;
        break;
    case 100:
        spd_intf = TEMOD2PLL_SPD_100_SGMII;
        break;
    case 1000:
        spd_intf = TEMOD2PLL_SPD_1000_SGMII;
        break;
    case 2500:
        spd_intf = TEMOD2PLL_SPD_2500_X1;
        break;
    case 5000:
        spd_intf = TEMOD2PLL_SPD_5000_XFI;
        break;
    case 10000:
        if ((config->interface_type == phymodInterfaceKR ||
            config->interface_type == phymodInterfaceCR)) {
            cl72_allowed = 1 ;
        }
        spd_intf = TEMOD2PLL_SPD_10000_XFI;
        break;
    case 40000:
        cl72_allowed = 1 ;
        spd_intf = TEMOD2PLL_SPD_40G_XLAUI;
        if (config->interface_type == phymodInterfaceXLAUI) {
            firmware_lane_config.MediaType = phymodFirmwareMediaTypePcbTraceBackPlane;
        } else if(config->interface_type == phymodInterfaceSR4) {
            cl72_allowed = 0;
            firmware_lane_config.MediaType = phymodFirmwareMediaTypeOptics ;
        } else if(config->interface_type == phymodInterfaceCR4) {
            firmware_lane_config.MediaType = phymodFirmwareMediaTypeCopperCable ;
        } else if (config->interface_type == phymodInterfaceKR4) {
            firmware_lane_config.MediaType = phymodFirmwareMediaTypePcbTraceBackPlane;
        } else {
            firmware_lane_config.MediaType = phymodFirmwareMediaTypePcbTraceBackPlane ;
        }
        break;
    default:
        PHYMOD_RETURN_WITH_ERR(PHYMOD_E_CONFIG,
            (_PHYMOD_MSG("this speed %d is not supported"),
              config->data_rate));
        break;
    }

    if(PHYMOD_INTF_MODES_HIGIG_GET(config)) {
       PHYMOD_IF_ERR_RETURN
          (temod2pll_encode_set(&phy->access, spd_intf, 1));
       PHYMOD_IF_ERR_RETURN
          (temod2pll_decode_set(&phy->access, spd_intf, 1));
    } else {
       PHYMOD_IF_ERR_RETURN
          (temod2pll_encode_set(&phy->access, spd_intf, 0));
       PHYMOD_IF_ERR_RETURN
          (temod2pll_decode_set(&phy->access, spd_intf, 0));
    }

    /* Check if ovco is power down */
    pm_phy_copy.access.pll_idx = ovco_pll_index;
    PHYMOD_IF_ERR_RETURN
        (blackhawk7_l8p2_pll_pwrdn_get(&pm_phy_copy.access, &ovco_is_pwrdn));

    /* if ovco is NOT pwoer down, then get the ovco div*/
    if (!ovco_is_pwrdn) {
        PHYMOD_IF_ERR_RETURN
            (blackhawk7_l8p2_INTERNAL_read_pll_div(&pm_phy_copy.access, &ovco_pll_div));
    }

    /* next check if pcs bypass mode, yes, then need to check tvco is power down */
    if (config->interface_type != phymodInterfaceBypass) {
        pm_phy_copy.access.pll_idx = tvco_pll_index;
        PHYMOD_IF_ERR_RETURN
            (blackhawk7_l8p2_pll_pwrdn_get(&pm_phy_copy.access, &tvco_is_pwrdn));
    }

    if (!tvco_is_pwrdn) {
        /* Get TVCO because it's not allowed to change during speed set */
        pm_phy_copy.access.pll_idx = tvco_pll_index;
        PHYMOD_IF_ERR_RETURN
            (blackhawk7_l8p2_INTERNAL_read_pll_div(&pm_phy_copy.access, &tvco_pll_div));
    }

    /* Get requested PLL */
    /*for ethernet speed mode config set */
    if (config->interface_type != phymodInterfaceBypass) {
        PHYMOD_IF_ERR_RETURN
            (tbhmod_sub10g_plldiv_lkup_get(&phy->access, spd_intf, config->ref_clock, &request_pll_div, &new_speed_vec));
        /* the speed is not supported */
        if (request_pll_div == TBHMOD_SUB10G_PLL_MODE_DIV_ZERO) {
            PHYMOD_RETURN_WITH_ERR(PHYMOD_E_CONFIG,
                                   (_PHYMOD_MSG("this speed %d is not supported"),
                                     config->data_rate));
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
                (blackhawk7_l8p2_lane_pll_selection_set(&pm_phy_copy.access, pll_index));
        }
    }

    PHYMOD_IF_ERR_RETURN
        (tbhmod_sub10g_osmode_lkup_get(&phy->access, spd_intf, &os_mode));
    for (i = 0; i < num_lane; i++) {
        pm_phy_copy.access.lane_mask = 1 << (start_lane + i);
        if (!PHYMOD_LANEPBMP_MEMBER(phy->access.lane_mask, start_lane + i)) {
            continue;
        }
        PHYMOD_IF_ERR_RETURN
            (blackhawk7_l8p2_osr_mode_set(&pm_phy_copy.access, os_mode));
    }

    /* cl72  control*/
    cl72_req = (flags & (PHYMOD_INTF_F_CL72_REQUESTED_BY_CNFG | PHYMOD_INTF_F_CL72_REQUESTED_BY_API)) ? 1 : 0;
    if (cl72_req & cl72_allowed) {
        cl72 = 1;
    } else {
        cl72 = 0;
    }
    for (i = 0; i < num_lane; i++) {
        pm_phy_copy.access.lane_mask = 1 << (start_lane + i);
        if (!PHYMOD_LANEPBMP_MEMBER(phy->access.lane_mask, start_lane + i)) {
            continue;
        }
        PHYMOD_IF_ERR_RETURN
            (blackhawk7_l8p2_clause72_control(&pm_phy_copy.access, cl72));
    }

    if(config->interface_type != phymodInterfaceBypass) {
        PHYMOD_IF_ERR_RETURN
            (temod2pll_set_spd_intf(&phy->access, spd_intf, 1));
    }

    PHYMOD_IF_ERR_RETURN
        (tbhmod_sub10g_osdfe_on_lkup_get(&phy->access, spd_intf, &os_dfeon));
    PHYMOD_IF_ERR_RETURN
        (tbhmod_sub10g_scrambling_dis_lkup_get(&phy->access, spd_intf, &scrambling_dis));
    firmware_lane_config.DfeOn = os_dfeon;
    firmware_lane_config.ScramblingDisable = scrambling_dis;
    firmware_lane_config.ForceNRZMode = 1;
    firmware_lane_config.ForcePAM4Mode = 0;
    for (i = 0; i < num_lane; i++) {
        if (!PHYMOD_LANEPBMP_MEMBER(phy->access.lane_mask, start_lane + i)) {
            continue;
        }
        pm_phy_copy.access.lane_mask = 0x1 << (start_lane + i);
        PHYMOD_IF_ERR_RETURN
             (_tscbh_sub10g_phy_firmware_lane_config_set(&pm_phy_copy, firmware_lane_config));
    }

    /* release the per lne soft reset bit */
    for (i = 0; i < num_lane; i++) {
        if (!PHYMOD_LANEPBMP_MEMBER(phy->access.lane_mask, start_lane + i)) {
            continue;
        }
        pm_phy_copy.access.lane_mask = 1 << (start_lane + i);
        PHYMOD_IF_ERR_RETURN
            (blackhawk7_l8p2_lane_soft_reset(&pm_phy_copy.access, 0));
    }

    /* Release the pcs lane reset only if the port is enabled */
    pm_phy_copy.access.lane_mask = 1 << start_lane;
    PHYMOD_IF_ERR_RETURN
        (tbhmod_sub10g_enable_set(&pm_phy_copy.access));

    return PHYMOD_E_NONE;

}


STATIC
int _tscbh_sub10g_speed_id_interface_config_get(const phymod_phy_access_t* phy, int speed_id,
                                                phymod_phy_inf_config_t* config, uint16_t an_enable,
                                                phymod_firmware_lane_config_t *lane_config)
{
    switch (speed_id) {
    case 0x1:
        config->data_rate = 10;
        config->interface_type = phymodInterfaceSGMII;
        break;
    case 0x2:
        config->data_rate = 100;
        config->interface_type = phymodInterfaceSGMII;
        break;
    case 0x3:
    {
        if (lane_config->MediaType == phymodFirmwareMediaTypeOptics) {
            config->interface_type = phymodInterface1000X;
        } else {
            config->interface_type = phymodInterfaceSGMII;
        }
        config->data_rate = 1000;
        break;
    }
    case 0x4:
        config->data_rate = 1000;
        config->interface_type = phymodInterfaceCX;
        break;
    case 0x5:
        config->data_rate = 1000;
        config->interface_type = phymodInterfaceKX;
        break;
    case 0x6:
    {
        if (an_enable) {
            config->interface_type = phymodInterfaceKX;
        } else {
            if (lane_config->MediaType == phymodFirmwareMediaTypeOptics) {
                config->interface_type = phymodInterface1000X;
            } else {
                config->interface_type = phymodInterfaceSGMII;
            }
        }
        config->data_rate = 2500;
        break;
    }
    case 0x1c:
    {
        config->data_rate = 10000;
        config->interface_type = phymodInterfaceKR;
        if(!an_enable) {
            if (lane_config->MediaType == phymodFirmwareMediaTypeOptics) {
                config->interface_type = phymodInterfaceSFI ;
            } else if (lane_config->MediaType == phymodFirmwareMediaTypeCopperCable) {
                config->interface_type = phymodInterfaceCR ;
            } else {
              config->interface_type = phymodInterfaceXFI ;
            }
        }
        break;
    }
    case 0x21:  /* digital_operationSpeeds_SPEED_40G_KR4 */
        {
            config->data_rate = 40000;
            if (lane_config->MediaType == phymodFirmwareMediaTypeOptics) {
                config->interface_type = phymodInterfaceSR4;
            } else if (lane_config->MediaType == phymodFirmwareMediaTypeCopperCable) {
                config->interface_type = phymodInterfaceCR4;
            } else if (lane_config->MediaType == phymodFirmwareMediaTypePcbTraceBackPlane) {
                config->interface_type = phymodInterfaceKR4;
            } else {
                config->interface_type = phymodInterfaceXLAUI;
            }
            break;
        }
    case 0x31:
        config->data_rate = 5000;
        config->interface_type = phymodInterfaceKR;
        break;
    case 0x35:
        config->data_rate = 10;
        config->interface_type = phymodInterfaceSGMII;
        break;
    case 0x36:
        config->data_rate = 100;
        config->interface_type = phymodInterfaceSGMII;
        break;
    case 0x37:
    {
        if (lane_config->MediaType == phymodFirmwareMediaTypeOptics) {
            config->interface_type = phymodInterface1000X;
        } else {
            config->interface_type = phymodInterfaceSGMII;
        }
        config->data_rate = 1000;
        break;
    }
    case 0x38:
    {
        if (lane_config->MediaType == phymodFirmwareMediaTypeOptics) {
            config->interface_type = phymodInterface1000X;
        } else {
            config->interface_type = phymodInterfaceSGMII;
        }
        config->data_rate = 2500;
        break;
    }
    default:
        config->data_rate = 0;
        config->interface_type = phymodInterfaceSGMII;
        break;
    }
    return PHYMOD_E_NONE;
}

int tscbh_sub10g_phy_interface_config_get(const phymod_phy_access_t* phy, uint32_t flags, phymod_ref_clk_t ref_clock, phymod_phy_inf_config_t* config)
{
    int speed_id;
    phymod_firmware_lane_config_t firmware_lane_config;
    phymod_phy_access_t pm_phy_copy;
    int start_lane, num_lane;
    temod2pll_an_control_t an_control;
    int an_complete = 0;

    config->ref_clock = ref_clock;
    PHYMOD_MEMCPY(&pm_phy_copy, phy, sizeof(pm_phy_copy));
    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(&phy->access, &start_lane, &num_lane));

    PHYMOD_IF_ERR_RETURN
        (temod2pll_speed_id_get(&phy->access, &speed_id));

    pm_phy_copy.access.lane_mask = 0x1 << start_lane;

    PHYMOD_MEMSET(&an_control, 0x0,  sizeof(temod2pll_an_control_t));
    PHYMOD_IF_ERR_RETURN
        (temod2pll_autoneg_control_get(&pm_phy_copy.access, &an_control, &an_complete));

    PHYMOD_IF_ERR_RETURN
        (tscbh_sub10g_phy_firmware_lane_config_get(&pm_phy_copy, &firmware_lane_config));

    PHYMOD_IF_ERR_RETURN
        (_tscbh_sub10g_speed_id_interface_config_get(phy, speed_id, config, an_control.enable, &firmware_lane_config));

    if (firmware_lane_config.MediaType == phymodFirmwareMediaTypeOptics) {
        PHYMOD_INTF_MODES_COPPER_CLR(config);
        PHYMOD_INTF_MODES_BACKPLANE_CLR(config);
        PHYMOD_INTF_MODES_FIBER_SET(config);
    } else if (firmware_lane_config.MediaType == phymodFirmwareMediaTypeCopperCable) {
        PHYMOD_INTF_MODES_FIBER_CLR(config);
        PHYMOD_INTF_MODES_BACKPLANE_CLR(config);
        PHYMOD_INTF_MODES_COPPER_SET(config);
    } else {
        PHYMOD_INTF_MODES_FIBER_CLR(config);
        PHYMOD_INTF_MODES_COPPER_CLR(config);
        PHYMOD_INTF_MODES_BACKPLANE_SET(config);
    }

    return PHYMOD_E_NONE;
}


int tscbh_sub10g_phy_cl72_set(const phymod_phy_access_t* phy, uint32_t cl72_en)
{
    struct blackhawk7_l8p2_uc_lane_config_st serdes_firmware_config;
    phymod_firmware_lane_config_t firmware_lane_config;
    int start_lane, num_lane, i;
    uint32_t lane_reset, pcs_lane_enable;
    phymod_phy_access_t pm_phy_copy;

    PHYMOD_MEMCPY(&pm_phy_copy, phy, sizeof(pm_phy_copy));
    PHYMOD_IF_ERR_RETURN(blackhawk7_l8p2_get_uc_lane_cfg(&pm_phy_copy.access, &serdes_firmware_config));

    if ((serdes_firmware_config.field.dfe_on == 0) && cl72_en) {
      PHYMOD_DEBUG_ERROR(("ERROR :: DFE is off : Can not start CL72/CL93 with no DFE\n"));
      return PHYMOD_E_CONFIG;
    }

    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(&phy->access, &start_lane, &num_lane));

    /*first check if lane is in reset */
    PHYMOD_IF_ERR_RETURN
        (blackhawk7_l8p2_lane_soft_reset_get(&pm_phy_copy.access, &lane_reset));

    /*next check if PCS lane is in reset */
    PHYMOD_IF_ERR_RETURN
        (temod2pll_disable_get(&pm_phy_copy.access, &pcs_lane_enable));

    /* disable pcs lane if pcs lane not in rset */
    if (pcs_lane_enable) {
        PHYMOD_IF_ERR_RETURN
            (temod2pll_disable_set(&pm_phy_copy.access));
    }

    /* if lane is not in reset, then reset the lane first */
    if (!lane_reset) {
        PHYMOD_MEMCPY(&pm_phy_copy, phy, sizeof(pm_phy_copy));
        PHYMOD_IF_ERR_RETURN
            (blackhawk7_l8p2_lane_soft_reset(&pm_phy_copy.access, 1));
    }

    /* next need to clear both force ER and NR config on the firmware lane config side
    if link training enable is set */
    if (cl72_en) {
        PHYMOD_IF_ERR_RETURN
            (tscbh_sub10g_phy_firmware_lane_config_get(phy, &firmware_lane_config));

        firmware_lane_config.ForceNormalReach = 0;
        firmware_lane_config.ForceExtenedReach = 0;

         PHYMOD_IF_ERR_RETURN
            (_tscbh_sub10g_phy_firmware_lane_config_set(phy, firmware_lane_config));
    }
    for (i = 0; i < num_lane; i++) {
        pm_phy_copy.access.lane_mask = 1 << (start_lane + i);
        if (!PHYMOD_LANEPBMP_MEMBER(phy->access.lane_mask, start_lane + i)) {
            continue;
        }
        PHYMOD_IF_ERR_RETURN
            (blackhawk7_l8p2_clause72_control(&pm_phy_copy.access, cl72_en));
    }

    /* release the ln dp reset */
    if (!lane_reset) {
        PHYMOD_MEMCPY(&pm_phy_copy, phy, sizeof(pm_phy_copy));
        PHYMOD_IF_ERR_RETURN
            (blackhawk7_l8p2_lane_soft_reset(&pm_phy_copy.access, 0));
    }

    /* re-enable pcs lane if pcs lane not in rset */
    if (pcs_lane_enable) {
        pm_phy_copy.access.lane_mask = 1 << start_lane;
        PHYMOD_IF_ERR_RETURN
            (tbhmod_sub10g_enable_set(&pm_phy_copy.access));
    }

    return PHYMOD_E_NONE;
}


int tscbh_sub10g_phy_cl72_get(const phymod_phy_access_t* phy, uint32_t* cl72_en)
{
    phymod_phy_access_t pm_phy_copy;

    PHYMOD_MEMCPY(&pm_phy_copy, phy, sizeof(pm_phy_copy));

    PHYMOD_IF_ERR_RETURN
        (blackhawk7_l8p2_clause72_control_get(&pm_phy_copy.access, cl72_en));

    return PHYMOD_E_NONE;
}


int tscbh_sub10g_phy_cl72_status_get(const phymod_phy_access_t* phy, phymod_cl72_status_t* status)
{
    int i;
    uint32_t tmp_status;
    int start_lane, num_lane;
    phymod_phy_access_t phy_copy;
    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));
    status->locked = 1;

    /* next figure out the lane num and start_lane based on the input */
    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(&phy->access, &start_lane, &num_lane));

    for (i = 0; i < num_lane; i++) {
        if (!PHYMOD_LANEPBMP_MEMBER(phy->access.lane_mask, start_lane + i)) {
            continue;
        }
        phy_copy.access.lane_mask = 0x1 << (i + start_lane);
        tmp_status = 1;
        PHYMOD_IF_ERR_RETURN(blackhawk7_l8p2_pmd_cl72_receiver_status(&phy_copy.access, &tmp_status));
        if (tmp_status == 0) {
            status->locked = 0;
            return PHYMOD_E_NONE;
        }
    }

    return PHYMOD_E_NONE;
}


int tscbh_sub10g_phy_autoneg_ability_set(const phymod_phy_access_t* phy, const phymod_autoneg_ability_t* an_ability)
{
    temod2pll_an_ability_t value;
    int start_lane, num_lane;
    phymod_phy_access_t phy_copy;

    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(&phy->access, &start_lane, &num_lane));

    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));
    phy_copy.access.lane_mask = 0x1 << start_lane;

    PHYMOD_MEMSET(&value, 0x0, sizeof(value));

    value.cl73_adv.an_cl72 = an_ability->an_cl72;
    if (PHYMOD_AN_FEC_CL74_GET(an_ability->an_fec)) {
        value.cl73_adv.an_fec = TEMOD2PLL_FEC_CL74_SUPRTD_REQSTD;
    } else if (PHYMOD_AN_FEC_OFF_GET(an_ability->an_fec)) {
        value.cl73_adv.an_fec = TEMOD2PLL_FEC_SUPRTD_NOT_REQSTD;
    }

    /* check if sgmii or not */
    if (PHYMOD_AN_CAP_SGMII_GET(an_ability)) {
        switch (an_ability->sgmii_speed) {
        case phymod_CL37_SGMII_10M:
            value.cl37_adv.cl37_sgmii_speed = TEMOD2PLL_CL37_SGMII_10M;
            break;
        case phymod_CL37_SGMII_100M:
            value.cl37_adv.cl37_sgmii_speed = TEMOD2PLL_CL37_SGMII_100M;
            break;
        case phymod_CL37_SGMII_1000M:
            value.cl37_adv.cl37_sgmii_speed = TEMOD2PLL_CL37_SGMII_1000M;
            break;
        default:
            value.cl37_adv.cl37_sgmii_speed = TEMOD2PLL_CL37_SGMII_1000M;
            break;
        }
    }
    /* next check pause */
    if (PHYMOD_AN_CAP_SYMM_PAUSE_GET(an_ability) && !PHYMOD_AN_CAP_ASYM_PAUSE_GET(an_ability)) {
        value.cl37_adv.an_pause = TEMOD2PLL_SYMM_PAUSE;
        value.cl73_adv.an_pause = TEMOD2PLL_SYMM_PAUSE;
    }
    if (PHYMOD_AN_CAP_ASYM_PAUSE_GET(an_ability) && !PHYMOD_AN_CAP_SYMM_PAUSE_GET(an_ability)) {
        value.cl37_adv.an_pause = TEMOD2PLL_ASYM_PAUSE;
        value.cl73_adv.an_pause = TEMOD2PLL_ASYM_PAUSE;
    }
    if (PHYMOD_AN_CAP_ASYM_PAUSE_GET(an_ability) && PHYMOD_AN_CAP_SYMM_PAUSE_GET(an_ability)) {
        value.cl37_adv.an_pause = TEMOD2PLL_ASYM_SYMM_PAUSE;
        value.cl73_adv.an_pause = TEMOD2PLL_ASYM_SYMM_PAUSE;
    }

    /* check cl73 ability */
    if (PHYMOD_AN_CAP_1G_KX_GET(an_ability->an_cap))
        value.cl73_adv.an_base_speed |= 1 << TEMOD2PLL_CL73_1000BASE_KX;
    if (PHYMOD_AN_CAP_10G_KR_GET(an_ability->an_cap))
        value.cl73_adv.an_base_speed |= 1 << TEMOD2PLL_CL73_10GBASE_KR;
    if (PHYMOD_AN_CAP_2P5G_X_GET(an_ability->an_cap))
        value.cl73_adv.an_base_speed |= 1 << TEMOD2PLL_CL73_2p5GBASE_X;
    if (PHYMOD_AN_CAP_5G_KR1_GET(an_ability->an_cap))
        value.cl73_adv.an_base_speed |= 1 << TEMOD2PLL_CL73_5GBASE_KR1;
    if (PHYMOD_AN_CAP_40G_KR4_GET(an_ability->an_cap))
        value.cl73_adv.an_base_speed |= 1 << TEMOD2PLL_CL73_40GBASE_KR4;
    if (PHYMOD_AN_CAP_40G_CR4_GET(an_ability->an_cap))
        value.cl73_adv.an_base_speed |= 1 << TEMOD2PLL_CL73_40GBASE_CR4;

    PHYMOD_IF_ERR_RETURN
        (temod2pll_autoneg_set(&phy_copy.access, &value));
    return PHYMOD_E_NONE;

}


int tscbh_sub10g_phy_autoneg_ability_get(const phymod_phy_access_t* phy, phymod_autoneg_ability_t* an_ability_get_type)
{
    temod2pll_an_ability_t value;
    phymod_phy_access_t phy_copy;
    int start_lane, num_lane;
    temod2pll_an_control_t an_control;
    int an_complete = 0;
    int an_fec = 0;

    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(&phy->access, &start_lane, &num_lane));
    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));
    phy_copy.access.lane_mask = 0x1 << start_lane;
    PHYMOD_MEMSET(&value, 0x0, sizeof(value));

    PHYMOD_IF_ERR_RETURN
        (temod2pll_autoneg_local_ability_get(&phy_copy.access, &value));
    PHYMOD_IF_ERR_RETURN
        (temod2pll_autoneg_control_get(&phy_copy.access, &an_control, &an_complete));
    an_ability_get_type->an_cl72 = value.cl73_adv.an_cl72;
    an_ability_get_type->an_fec = 0;
    an_fec = value.cl73_adv.an_fec;
    if (an_fec & TEMOD2PLL_FEC_CL74_SUPRTD_REQSTD) {
        PHYMOD_AN_FEC_CL74_SET(an_ability_get_type->an_fec);
    } else {
        PHYMOD_AN_FEC_OFF_SET(an_ability_get_type->an_fec);
    }

    if ((value.cl37_adv.an_pause == TEMOD2PLL_ASYM_PAUSE) || (value.cl73_adv.an_pause == TEMOD2PLL_ASYM_PAUSE)) {
        PHYMOD_AN_CAP_ASYM_PAUSE_SET(an_ability_get_type);
    } else if ((value.cl37_adv.an_pause == TEMOD2PLL_SYMM_PAUSE) || (value.cl73_adv.an_pause == TEMOD2PLL_SYMM_PAUSE)) {
        PHYMOD_AN_CAP_SYMM_PAUSE_SET(an_ability_get_type);
    } else if ((value.cl37_adv.an_pause == TEMOD2PLL_ASYM_SYMM_PAUSE) || (value.cl73_adv.an_pause == TEMOD2PLL_ASYM_SYMM_PAUSE)) {
        PHYMOD_AN_CAP_ASYM_PAUSE_SET(an_ability_get_type);
        PHYMOD_AN_CAP_SYMM_PAUSE_SET(an_ability_get_type);
    }

    /* get AN Clause */
    switch (an_control.an_type) {
        case TEMOD2PLL_AN_MODE_CL73:
            PHYMOD_AN_CAP_CL73_SET(an_ability_get_type);
            break;
        case TEMOD2PLL_AN_MODE_CL37:
            PHYMOD_AN_CAP_CL37_SET(an_ability_get_type);
            break;
        case TEMOD2PLL_AN_MODE_SGMII:
            PHYMOD_AN_CAP_SGMII_SET(an_ability_get_type);
            break;
        default:
            break;
    }

    /* get the cl37 sgmii speed */
    switch (value.cl37_adv.cl37_sgmii_speed) {
    case TEMOD2PLL_CL37_SGMII_10M:
        an_ability_get_type->sgmii_speed = phymod_CL37_SGMII_10M;
        break;
    case TEMOD2PLL_CL37_SGMII_100M:
        an_ability_get_type->sgmii_speed = phymod_CL37_SGMII_100M;
        break;
    case TEMOD2PLL_CL37_SGMII_1000M:
        an_ability_get_type->sgmii_speed = phymod_CL37_SGMII_1000M;
        break;
    default:
        break;
    }
    /* first check cl73 ability */
    if (value.cl73_adv.an_base_speed & 1 << TEMOD2PLL_CL73_40GBASE_CR4)
        PHYMOD_AN_CAP_40G_CR4_SET(an_ability_get_type->an_cap);
    if (value.cl73_adv.an_base_speed & 1 << TEMOD2PLL_CL73_40GBASE_KR4)
        PHYMOD_AN_CAP_40G_KR4_SET(an_ability_get_type->an_cap);
    if (value.cl73_adv.an_base_speed & 1 << TEMOD2PLL_CL73_10GBASE_KR)
        PHYMOD_AN_CAP_10G_KR_SET(an_ability_get_type->an_cap);
    if (value.cl73_adv.an_base_speed & 1 << TEMOD2PLL_CL73_1000BASE_KX)
        PHYMOD_AN_CAP_1G_KX_SET(an_ability_get_type->an_cap);
    if (value.cl73_adv.an_base_speed & 1 << TEMOD2PLL_CL73_2p5GBASE_X)
        PHYMOD_AN_CAP_2P5G_X_SET(an_ability_get_type->an_cap);
    if (value.cl73_adv.an_base_speed & 1 << TEMOD2PLL_CL73_5GBASE_KR1)
        PHYMOD_AN_CAP_5G_KR1_SET(an_ability_get_type->an_cap);

    return PHYMOD_E_NONE;
}



int tscbh_sub10g_phy_autoneg_remote_ability_get(const phymod_phy_access_t* phy, phymod_autoneg_ability_t* an_ability_get_type)
{

    temod2pll_an_ability_t value;
    temod2pll_an_control_t an_control;
    phymod_phy_access_t phy_copy;
    int an_complete = 0;
    int start_lane, num_lane;
    int an_fec;

    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(&phy->access, &start_lane, &num_lane));
    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));
    phy_copy.access.lane_mask = 0x1 << start_lane;
    PHYMOD_MEMSET(&value, 0x0, sizeof(value));
    PHYMOD_MEMSET(&an_control, 0x0, sizeof(an_control));

    PHYMOD_IF_ERR_RETURN
        (temod2pll_autoneg_remote_ability_get(&phy_copy.access, &value));
    an_ability_get_type->an_cl72 = value.cl73_adv.an_cl72;
    an_fec = value.cl73_adv.an_fec;
    if (an_fec == TEMOD2PLL_FEC_CL74_SUPRTD_REQSTD) {
        PHYMOD_AN_FEC_CL74_SET(an_ability_get_type->an_fec);
    } else {
        PHYMOD_AN_FEC_OFF_SET(an_ability_get_type->an_fec);
    }
    PHYMOD_IF_ERR_RETURN
        (temod2pll_autoneg_control_get(&phy_copy.access, &an_control, &an_complete));

    if (an_control.an_type == TEMOD2PLL_AN_MODE_CL73) {
        if (value.cl73_adv.an_pause == TEMOD2PLL_ASYM_PAUSE) {
            PHYMOD_AN_CAP_ASYM_PAUSE_SET(an_ability_get_type);
        } else if (value.cl73_adv.an_pause == TEMOD2PLL_SYMM_PAUSE) {
            PHYMOD_AN_CAP_SYMM_PAUSE_SET(an_ability_get_type);
        } else if (value.cl73_adv.an_pause == TEMOD2PLL_ASYM_SYMM_PAUSE) {
            PHYMOD_AN_CAP_ASYM_PAUSE_SET(an_ability_get_type);
            PHYMOD_AN_CAP_SYMM_PAUSE_SET(an_ability_get_type);
        }
    } else {
      if (value.cl37_adv.an_pause == TEMOD2PLL_ASYM_PAUSE) {
          PHYMOD_AN_CAP_ASYM_PAUSE_SET(an_ability_get_type);
      } else if (value.cl37_adv.an_pause == TEMOD2PLL_SYMM_PAUSE) {
          PHYMOD_AN_CAP_SYMM_PAUSE_SET(an_ability_get_type);
      } else if (value.cl37_adv.an_pause == TEMOD2PLL_ASYM_SYMM_PAUSE) {
          PHYMOD_AN_CAP_ASYM_PAUSE_SET(an_ability_get_type);
          PHYMOD_AN_CAP_SYMM_PAUSE_SET(an_ability_get_type);
      }
    }

    /* get AN Clause */
    switch (an_control.an_type) {
        case TEMOD2PLL_AN_MODE_CL73:
            PHYMOD_AN_CAP_CL73_SET(an_ability_get_type);
            break;
        case TEMOD2PLL_AN_MODE_CL37:
            PHYMOD_AN_CAP_CL37_SET(an_ability_get_type);
            break;
        case TEMOD2PLL_AN_MODE_SGMII:
            PHYMOD_AN_CAP_SGMII_SET(an_ability_get_type);
            break;
        default:
            break;
     }

    /* get the cl37 sgmii speed */
    switch (value.cl37_adv.cl37_sgmii_speed) {
    case TEMOD2PLL_CL37_SGMII_10M:
        an_ability_get_type->sgmii_speed = phymod_CL37_SGMII_10M;
        break;
    case TEMOD2PLL_CL37_SGMII_100M:
        an_ability_get_type->sgmii_speed = phymod_CL37_SGMII_100M;
        break;
    case TEMOD2PLL_CL37_SGMII_1000M:
        an_ability_get_type->sgmii_speed = phymod_CL37_SGMII_1000M;
        break;
    default:
        break;
    }

    /* check cl73 ability */
    if (value.cl73_adv.an_base_speed &  1 << TEMOD2PLL_CL73_100GBASE_CR10)
        PHYMOD_AN_CAP_100G_CR10_SET(an_ability_get_type->an_cap);
    if (value.cl73_adv.an_base_speed & 1 << TEMOD2PLL_CL73_40GBASE_CR4)
        PHYMOD_AN_CAP_40G_CR4_SET(an_ability_get_type->an_cap);
    if (value.cl73_adv.an_base_speed & 1 << TEMOD2PLL_CL73_40GBASE_KR4)
        PHYMOD_AN_CAP_40G_KR4_SET(an_ability_get_type->an_cap);
    if (value.cl73_adv.an_base_speed & 1 << TEMOD2PLL_CL73_10GBASE_KR)
        PHYMOD_AN_CAP_10G_KR_SET(an_ability_get_type->an_cap);
    if (value.cl73_adv.an_base_speed & 1 << TEMOD2PLL_CL73_10GBASE_KX4)
        PHYMOD_AN_CAP_10G_KX4_SET(an_ability_get_type->an_cap);
    if (value.cl73_adv.an_base_speed & 1 << TEMOD2PLL_CL73_1000BASE_KX)
        PHYMOD_AN_CAP_1G_KX_SET(an_ability_get_type->an_cap);
    if (value.cl73_adv.an_base_speed & 1 << TEMOD2PLL_CL73_2p5GBASE_X)
        PHYMOD_AN_CAP_2P5G_X_SET(an_ability_get_type->an_cap);
    if (value.cl73_adv.an_base_speed & 1 << TEMOD2PLL_CL73_5GBASE_KR1)
        PHYMOD_AN_CAP_5G_KR1_SET(an_ability_get_type->an_cap);

    return PHYMOD_E_NONE;
}



int tscbh_sub10g_phy_autoneg_set(const phymod_phy_access_t* phy, const phymod_autoneg_control_t* an)
{
    int num_lane_adv_encoded;
    phymod_firmware_lane_config_t firmware_lane_config;
    int start_lane, num_lane, i ;
    phymod_phy_access_t phy_copy;
    temod2pll_an_control_t an_control;

    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(&phy->access, &start_lane, &num_lane));

    PHYMOD_MEMSET(&firmware_lane_config, 0x0, sizeof(firmware_lane_config));

    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));
    phy_copy.access.lane_mask = 0x1 << start_lane;

    /*only support cl73/sgmii/cl37*/
    switch (an->num_lane_adv) {
        case 1:
            num_lane_adv_encoded = 0;
            break;
        case 4:
            num_lane_adv_encoded = 2;
        default:
            return PHYMOD_E_PARAM;
    }

    /*KX pd is not supported*/
    an_control.pd_kx_en = 0;
    an_control.pd_kx4_en = 0;
    an_control.pd_2p5g_en = 0; /*for now disable*/
    an_control.num_lane_adv = num_lane_adv_encoded;
    an_control.enable       = an->enable;
    an_control.an_property_type = 0x0;   /* for now disable */
    switch (an->an_mode) {
    case phymod_AN_MODE_CL73:
        an_control.an_type = TEMOD2PLL_AN_MODE_CL73;
        break;
    case phymod_AN_MODE_CL37:
        an_control.an_type = TEMOD2PLL_AN_MODE_CL37;
        break;
    case phymod_AN_MODE_SGMII:
        an_control.an_type = TEMOD2PLL_AN_MODE_SGMII;
        break;
    default:
        return PHYMOD_E_PARAM;
        break;
    }

    PHYMOD_IF_ERR_RETURN
        (temod2pll_disable_set(&phy->access));

    for (i = 0; i < num_lane; i++) {
        if (!PHYMOD_LANEPBMP_MEMBER(phy->access.lane_mask, start_lane + i)) {
            continue;
        }
        phy_copy.access.lane_mask = 0x1 << (i + start_lane);
        PHYMOD_IF_ERR_RETURN
            (blackhawk7_l8p2_lane_soft_reset(&phy_copy.access, 1));
    }

     /*next choose TVCO as the PLL selelction for all the lanes*/
    if (an->enable) {
        for (i = 0; i < num_lane; i++) {
            phy_copy.access.lane_mask = 1 << (start_lane + i);
            PHYMOD_IF_ERR_RETURN
                (blackhawk7_l8p2_lane_pll_selection_set(&phy_copy.access, phy->access.tvco_pll_index));
        }
    }

    /* make sure the firmware config is set to an eenabled */
    PHYMOD_IF_ERR_RETURN
        (tscbh_sub10g_phy_firmware_lane_config_get(&phy_copy, &firmware_lane_config));


    if (an->enable) {
        firmware_lane_config.AnEnabled = 1;
        firmware_lane_config.LaneConfigFromPCS = 1;
        firmware_lane_config.Cl72RestTO = 0;
        firmware_lane_config.ForceNormalReach = 0;
        firmware_lane_config.ForceExtenedReach = 0;
        firmware_lane_config.ForceNRZMode= 0;
        firmware_lane_config.ForcePAM4Mode = 0;
    } else {
        firmware_lane_config.AnEnabled = 0;
        firmware_lane_config.LaneConfigFromPCS = 0;
        firmware_lane_config.Cl72RestTO = 1;
    }

    for (i = 0; i < num_lane; i++) {
        if (!PHYMOD_LANEPBMP_MEMBER(phy->access.lane_mask, start_lane + i)) {
            continue;
        }
        phy_copy.access.lane_mask = 0x1 << (i + start_lane);
        PHYMOD_IF_ERR_RETURN
            (_tscbh_sub10g_phy_firmware_lane_config_set(&phy_copy, firmware_lane_config));
    }

    for (i = 0; i < num_lane; i++) {
        if (!PHYMOD_LANEPBMP_MEMBER(phy->access.lane_mask, start_lane + i)) {
            continue;
        }
        phy_copy.access.lane_mask = 0x1 << (i + start_lane);
        PHYMOD_IF_ERR_RETURN
            (blackhawk7_l8p2_lane_soft_reset(&phy_copy.access, 0));
    }

    if (an->enable) {
        PHYMOD_IF_ERR_RETURN
            (tbhmod_sub10g_set_an_port_mode(&phy_copy.access, 1,
                                            num_lane_adv_encoded,
                                            start_lane, 0));
    }

    phy_copy.access.lane_mask = 0x1 << start_lane;
    PHYMOD_IF_ERR_RETURN
        (tbhmod_sub10g_enable_set(&phy_copy.access));

    PHYMOD_IF_ERR_RETURN
        (temod2pll_autoneg_control(&phy_copy.access, &an_control));

    return PHYMOD_E_NONE;
}


int tscbh_sub10g_phy_autoneg_get(const phymod_phy_access_t* phy, phymod_autoneg_control_t* an, uint32_t* an_done)
{
    temod2pll_an_control_t an_control;
    phymod_phy_access_t phy_copy;
    int start_lane, num_lane;
    int an_complete = 0;

    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(&phy->access, &start_lane, &num_lane));

    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));
    phy_copy.access.lane_mask = 0x1 << start_lane;

    PHYMOD_MEMSET(&an_control, 0x0,  sizeof(temod2pll_an_control_t));
    PHYMOD_IF_ERR_RETURN
        (temod2pll_autoneg_control_get(&phy_copy.access, &an_control, &an_complete));

    if (an_control.enable) {
        an->enable = 1;
        *an_done = an_complete;
    } else {
        an->enable = 0;
        *an_done   = 0;
    }
    switch (an_control.num_lane_adv) {
        case 0:
            an->num_lane_adv = 1;
            break;
        default:
            an->num_lane_adv = 0;
            break;
    }
    switch (an_control.an_type) {
        case TEMOD2PLL_AN_MODE_CL73:
            an->an_mode = phymod_AN_MODE_CL73;
            break;
        case TEMOD2PLL_AN_MODE_CL37:
            an->an_mode = phymod_AN_MODE_CL37;
            break;
        case TEMOD2PLL_AN_MODE_SGMII:
            an->an_mode = phymod_AN_MODE_SGMII;
            break;
        default:
            an->an_mode = phymod_AN_MODE_NONE;
            break;
    }

    return PHYMOD_E_NONE;
}


int tscbh_sub10g_phy_autoneg_restart_set(const phymod_phy_access_t* phy, const phymod_autoneg_control_t* an)
{

    /* Place your code here */


    return PHYMOD_E_NONE;

}


int tscbh_sub10g_phy_autoneg_status_get(const phymod_phy_access_t* phy, phymod_autoneg_status_t* status)
{

    /* Place your code here */


    return PHYMOD_E_NONE;

}

/* load tscbh fw. the fw_loader parameter is valid just for external fw load*/
STATIC
int _tscbh_sub10g_core_firmware_load(const phymod_core_access_t* core, const phymod_core_init_config_t* init_config)
{
    phymod_core_access_t  core_copy;
    phymod_phy_access_t phy_access;
    unsigned int blackhawk_ucode_len;
    unsigned char *blackhawk_ucode;

    PHYMOD_MEMCPY(&core_copy, core, sizeof(core_copy));
    TSCBH_SUB10G_CORE_TO_PHY_ACCESS(&phy_access, core);

    phy_access.access.lane_mask = 0x1;
    blackhawk_ucode_len = BLACKHAWK7_V2L8P2_UCODE_IMAGE_SIZE;
    blackhawk_ucode = blackhawk7_l8p2_ucode_get(0, 0);

    switch (init_config->firmware_load_method) {
    case phymodFirmwareLoadMethodInternal:
        PHYMOD_IF_ERR_RETURN(blackhawk7_l8p2_ucode_load(&core_copy.access, blackhawk_ucode, blackhawk_ucode_len));
        break;
    case phymodFirmwareLoadMethodExternal:
        PHYMOD_NULL_CHECK(init_config->firmware_loader);
        PHYMOD_IF_ERR_RETURN(blackhawk7_l8p2_ucode_init(&core_copy.access));
        PHYMOD_IF_ERR_RETURN
            (blackhawk7_l8p2_pram_firmware_enable(&core_copy.access, 1, 0));
        PHYMOD_IF_ERR_RETURN(init_config->firmware_loader(core, blackhawk_ucode_len, blackhawk_ucode));
        PHYMOD_IF_ERR_RETURN
            (blackhawk7_l8p2_pram_firmware_enable(&core_copy.access, 0, 0));
        break;
    case phymodFirmwareLoadMethodNone:
        break;
    default:
        PHYMOD_RETURN_WITH_ERR(PHYMOD_E_CONFIG, (_PHYMOD_MSG("illegal fw load method %u"), init_config->firmware_load_method));
    }

    return PHYMOD_E_NONE;

}

STATIC
int _tscbh_sub10g_core_init_pass1(const phymod_core_access_t* core,
                                  const phymod_core_init_config_t* init_config,
                                  const phymod_core_status_t* core_status)
{
    int rv, lane;
    phymod_phy_access_t phy_access;
    phymod_core_access_t  core_copy;
    uint32_t uc_enable = 0;
    ucode_info_t ucode;
    unsigned int blackhawk7_ucode_len;
    unsigned char *blackhawk7_ucode;

    TSCBH_SUB10G_CORE_TO_PHY_ACCESS(&phy_access, core);
    PHYMOD_MEMCPY(&core_copy, core, sizeof(core_copy));
    core_copy.access.lane_mask = 0x1;

    /* 1. De-assert PMD core power and core data path reset */
    PHYMOD_IF_ERR_RETURN
        (tbhmod_sub10g_pmd_reset_seq(&core_copy.access, 0));

    core_copy.access.pll_idx = 1;
    PHYMOD_IF_ERR_RETURN
        (blackhawk7_l8p2_core_dp_reset(&core_copy.access, 1));
    core_copy.access.pll_idx = 0;
    PHYMOD_IF_ERR_RETURN
        (blackhawk7_l8p2_core_dp_reset(&core_copy.access, 1));

    /* De-assert PMD lane reset */
    for (lane = 0; lane < TSCBH_SUB10G_NOF_LANES_IN_CORE; lane++) {
        phy_access.access.lane_mask = 1 << lane;
        PHYMOD_IF_ERR_RETURN
            (temod2pll_pmd_x4_reset(&phy_access.access));
    }

    phy_access.access.lane_mask = 0x1;
    if (!PHYMOD_CORE_INIT_F_SERDES_FW_BCAST_GET(init_config)) {
        PHYMOD_IF_ERR_RETURN
            (blackhawk7_l8p2_uc_active_get(&phy_access.access, &uc_enable));
        if (uc_enable) return PHYMOD_E_NONE;
    }

    /* 2. Set the heart beat, default is 156.25M */
    if (init_config->interface.ref_clock == phymodRefClk156Mhz) {
        PHYMOD_IF_ERR_RETURN
            (blackhawk7_l8p2_refclk_set(&core_copy.access, phymodRefClk156Mhz));
    } else {
        PHYMOD_DEBUG_ERROR(("Unsupported reference clock.\n"));
        return PHYMOD_E_UNAVAIL;
    }

    /* set the micro stack size */
    phy_access.access.lane_mask = 0x1;
    PHYMOD_MEMSET(&ucode, 0, sizeof(ucode));
    ucode.stack_size = BLACKHAWK7_V2L8P2_UCODE_STACK_SIZE;
    ucode.ucode_size = BLACKHAWK7_V2L8P2_UCODE_IMAGE_SIZE;
    ucode.crc_value  = BLACKHAWK7_V2L8P2_UCODE_IMAGE_CRC;
    blackhawk7_ucode = blackhawk7_l8p2_ucode_get(0, 0);
    blackhawk7_ucode_len = ucode.ucode_size;

    PHYMOD_IF_ERR_RETURN
        (blackhawk7_l8p2_uc_reset_with_info(&core_copy.access , 1, ucode));

    /* 3. Micro code load and verify */
    rv = _tscbh_sub10g_core_firmware_load(&core_copy, init_config);
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
            rv = blackhawk7_l8p2_ucode_load_verify(&core_copy.access, (uint8_t *) blackhawk7_ucode, blackhawk7_ucode_len);
            if (rv != PHYMOD_E_NONE) {
                PHYMOD_DEBUG_ERROR(("devad 0x%x lane 0x%x: UC load-verify failed\n", core->access.addr, core->access.lane_mask));
                PHYMOD_IF_ERR_RETURN(rv);
            }
        }
    }

    return PHYMOD_E_NONE;
}


STATIC
int _tscbh_sub10g_core_init_pass2(const phymod_core_access_t* core,
                                  const phymod_core_init_config_t* init_config,
                                  const phymod_core_status_t* core_status)
{
    phymod_phy_access_t phy_access, phy_access_copy;
    phymod_core_access_t  core_copy;
    int lane, pll_num, pmd_config_bypass = 0;
    enum blackhawk7_l8p2_pll_refclk_enum refclk;
    phymod_polarity_t tmp_pol;

    TSCBH_SUB10G_CORE_TO_PHY_ACCESS(&phy_access, core);
    PHYMOD_MEMCPY(&core_copy, core, sizeof(core_copy));

    /* first check if PMD config needs to be skipped */
    if (PHYMOD_CORE_INIT_F_BYPASS_PMD_CONFIGURATION_GET(init_config)) {
        pmd_config_bypass = 1;
        core_copy.access.lane_mask = 0x10;
        phy_access_copy = phy_access;
        phy_access_copy.access = core->access;
        phy_access_copy.access.lane_mask = 0x10;
        phy_access_copy.type = core->type;
    } else {
        core_copy.access.lane_mask = 0x1;
        phy_access_copy = phy_access;
        phy_access_copy.access = core->access;
        phy_access_copy.access.lane_mask = 0x1;
        phy_access_copy.type = core->type;
    }

    /* 4. Configure PMD lane mapping and PCS lane swap */
    PHYMOD_IF_ERR_RETURN
        (tscbh_sub10g_core_lane_map_set(&core_copy, &init_config->lane_map));

    /* Set the heart beat on pcs, default is 156.25M  */
    if (init_config->interface.ref_clock == phymodRefClk156Mhz) {
        PHYMOD_IF_ERR_RETURN
            (tbhmod_sub10g_refclk_set(&core_copy.access, phymodRefClk156Mhz));
    } else {
        PHYMOD_DEBUG_ERROR(("Unsupported reference clock.\n"));
        return PHYMOD_E_UNAVAIL;
    }

    if (!pmd_config_bypass) {
        if (init_config->firmware_load_method != phymodFirmwareLoadMethodNone) {
            ucode_info_t ucode;

            PHYMOD_MEMSET(&ucode, 0, sizeof(ucode));
            ucode.stack_size = BLACKHAWK7_V2L8P2_UCODE_STACK_SIZE;
            ucode.ucode_size = BLACKHAWK7_V2L8P2_UCODE_IMAGE_SIZE;
            ucode.crc_value  = BLACKHAWK7_V2L8P2_UCODE_IMAGE_CRC;

            /* 5. Release uc reset */
            PHYMOD_IF_ERR_RETURN
                (blackhawk7_l8p2_uc_reset_with_info(&core_copy.access , 0, ucode));

            /* 6. Wait for uc_active = 1 */
            PHYMOD_IF_ERR_RETURN
                (blackhawk7_l8p2_wait_uc_active(&phy_access_copy.access));

            for (lane = 0; lane < TSCBH_SUB10G_NOF_LANES_IN_CORE; lane++) {
                phy_access_copy.access.lane_mask = 1 << lane;
                PHYMOD_IF_ERR_RETURN
                    (blackhawk7_l8p2_pmd_ln_h_rstb_pkill_override(&phy_access_copy.access, 0x1));
            }

            /* 7. Initialize software information table for the macro */
            PHYMOD_IF_ERR_RETURN
                (blackhawk7_l8p2_init_blackhawk7_l8p2_info(&core_copy.access));

            /* check the FW crc checksum error */
            if (!PHYMOD_CORE_INIT_F_BYPASS_CRC_CHECK_GET(init_config)) {
                PHYMOD_IF_ERR_RETURN
                    (blackhawk7_l8p2_ucode_crc_verify(&core_copy.access, ucode.ucode_size, ucode.crc_value));
            }

            /* release pmd lane hard reset */
            for (lane = 0; lane < TSCBH_SUB10G_NOF_LANES_IN_CORE; lane++) {
                phy_access_copy.access.lane_mask = 1 << lane;
                PHYMOD_IF_ERR_RETURN
                    (blackhawk7_l8p2_pmd_ln_h_rstb_pkill_override(&phy_access_copy.access, 0x0));
            }
        }

        /* 8. RX/TX polarity configuration */
        for (lane = 0; lane < TSCBH_SUB10G_NOF_LANES_IN_CORE; lane++) {
            phy_access_copy.access.lane_mask = 1 << lane;
            tmp_pol.tx_polarity = (init_config->polarity_map.tx_polarity) >> lane & 0x1;
            tmp_pol.rx_polarity = (init_config->polarity_map.rx_polarity) >> lane & 0x1;
            PHYMOD_IF_ERR_RETURN
                (tscbh_sub10g_phy_polarity_set(&phy_access_copy, &tmp_pol));
            /* clear the tmp vairiable */
            PHYMOD_MEMSET(&tmp_pol, 0x0, sizeof(tmp_pol));
        }

        /* 9. Enable PCS clock block */
        for (pll_num = 0; pll_num < 2; pll_num++) {
            core_copy.access.pll_idx = pll_num;
            PHYMOD_IF_ERR_RETURN
                (tbhmod_sub10g_pcs_clk_blk_en(&core_copy.access, 1));
        }
        /* 10. PLL_DIV config for both PLL0 and PLL1 */
        refclk = BLACKHAWK7_L8P2_PLL_REFCLK_156P25MHZ;
        if (init_config->pll0_div_init_value != (uint32_t)phymod_TSCBH_PLL_DIVNONE) {
            core_copy.access.pll_idx = 0;
            if (init_config->pll0_div_init_value == TBHMOD_SUB10G_PLL_MODE_DIV_132 ||
                init_config->pll0_div_init_value == TBHMOD_SUB10G_PLL_MODE_DIV_66) {
                PHYMOD_IF_ERR_RETURN
                    (blackhawk7_l8p2_configure_20P625G_pll_div(&core_copy.access,
                                                               init_config->pll0_div_init_value));
            } else {
                PHYMOD_IF_ERR_RETURN
                    (blackhawk7_l8p2_configure_pll_refclk_div(&core_copy.access,
                                                              refclk,
                                                              init_config->pll0_div_init_value));
            }
        }

        if (init_config->pll1_div_init_value != (uint32_t)phymod_TSCBH_PLL_DIVNONE) {
            core_copy.access.pll_idx = 1;
            if (init_config->pll1_div_init_value == TBHMOD_SUB10G_PLL_MODE_DIV_132 ||
                init_config->pll1_div_init_value == TBHMOD_SUB10G_PLL_MODE_DIV_66) {
                PHYMOD_IF_ERR_RETURN
                    (blackhawk7_l8p2_configure_20P625G_pll_div(&core_copy.access,
                                                               init_config->pll1_div_init_value));
            } else {
                PHYMOD_IF_ERR_RETURN
                    (blackhawk7_l8p2_configure_pll_refclk_div(&core_copy.access,
                                                              refclk,
                                                              init_config->pll1_div_init_value));
            }
        }

        /* set the PMD debug level to be 2 as default */
        for (lane = 0; lane < TSCBH_SUB10G_NOF_LANES_IN_CORE; lane++) {
            phy_access_copy.access.lane_mask = 1 << lane;
            PHYMOD_IF_ERR_RETURN
                (blackhawk7_l8p2_set_usr_ctrl_lane_event_log_level(&phy_access_copy.access, 2));
        }
    }

    PHYMOD_IF_ERR_RETURN
        (temod2pll_autoneg_timer_init(&core_copy.access));

    /* mld has 2 reg copies */
    if (pmd_config_bypass) {
        phy_access_copy.access.lane_mask = 0xf0;
    } else {
        phy_access_copy.access.lane_mask = 0xf;
    }
    PHYMOD_IF_ERR_RETURN
        (temod2pll_mld_am_timers_set(&phy_access_copy.access));

    if (pmd_config_bypass) {
        phy_access_copy.access.lane_mask = 0x10;
    } else {
        phy_access_copy.access.lane_mask = 0x1;
    }
    PHYMOD_IF_ERR_RETURN
        (temod2pll_master_port_num_set(&core_copy.access, 0));
    PHYMOD_IF_ERR_RETURN
        (temod2pll_cl48_lfrfli_init(&core_copy.access));

    PHYMOD_IF_ERR_RETURN
        (temod2pll_cl74_chng_default (&core_copy.access));

    if (!pmd_config_bypass) {
        /* 11. Release core DP soft reset for both PLLs */
        core_copy.access.lane_mask = 0x1;
        core_copy.access.pll_idx = 0;
        PHYMOD_IF_ERR_RETURN
            (blackhawk7_l8p2_core_dp_reset(&core_copy.access, 0));
        core_copy.access.pll_idx = 1;
        PHYMOD_IF_ERR_RETURN
            (blackhawk7_l8p2_core_dp_reset(&core_copy.access, 0));
    }

    return PHYMOD_E_NONE;
}
int tscbh_sub10g_core_init(const phymod_core_access_t* core, const phymod_core_init_config_t* init_config, const phymod_core_status_t* core_status)
{
    if ( (!PHYMOD_CORE_INIT_F_EXECUTE_PASS1_GET(init_config) &&
          !PHYMOD_CORE_INIT_F_EXECUTE_PASS2_GET(init_config)) ||
        PHYMOD_CORE_INIT_F_EXECUTE_PASS1_GET(init_config)) {
        PHYMOD_IF_ERR_RETURN
            (_tscbh_sub10g_core_init_pass1(core, init_config, core_status));

        if (PHYMOD_CORE_INIT_F_EXECUTE_PASS1_GET(init_config)) {
            return PHYMOD_E_NONE;
        }
    }

    if ( (!PHYMOD_CORE_INIT_F_EXECUTE_PASS1_GET(init_config) &&
          !PHYMOD_CORE_INIT_F_EXECUTE_PASS2_GET(init_config)) ||
        PHYMOD_CORE_INIT_F_EXECUTE_PASS2_GET(init_config)) {
        PHYMOD_IF_ERR_RETURN
            (_tscbh_sub10g_core_init_pass2(core, init_config, core_status));
    }

    return PHYMOD_E_NONE;

}


int tscbh_sub10g_phy_pll_multiplier_get(const phymod_phy_access_t* phy, uint32_t* core_vco_pll_multiplier)
{
    phymod_phy_access_t pm_phy_copy;
    PHYMOD_MEMCPY(&pm_phy_copy, phy, sizeof(pm_phy_copy));

    PHYMOD_IF_ERR_RETURN(blackhawk7_l8p2_INTERNAL_read_pll_div(&pm_phy_copy.access, core_vco_pll_multiplier));
    return PHYMOD_E_NONE;
}


int tscbh_sub10g_phy_init(const phymod_phy_access_t* phy, const phymod_phy_init_config_t* init_config)
{
    /*int pll_restart = 0;*/
    const phymod_access_t *pm_acc = &phy->access;
    phymod_phy_access_t pm_phy_copy;
    int start_lane, num_lane, i;
    phymod_firmware_lane_config_t firmware_lane_config;
    uint32_t pll_power_down = 0;

    PHYMOD_MEMCPY(&pm_phy_copy, phy, sizeof(pm_phy_copy));
    PHYMOD_MEMSET(&firmware_lane_config, 0x0, sizeof(firmware_lane_config));

    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(pm_acc, &start_lane, &num_lane));
    /* per lane based reset release */
    PHYMOD_IF_ERR_RETURN
        (temod2pll_pmd_x4_reset(&pm_phy_copy.access));

    /* Put PMD lane into soft reset */
    PHYMOD_IF_ERR_RETURN
        (blackhawk7_l8p2_lane_soft_reset(&pm_phy_copy.access, 1));

    for (i = 0; i < num_lane; i++) {
        if (!PHYMOD_LANEPBMP_MEMBER(phy->access.lane_mask, start_lane + i)) {
            continue;
        }
        pm_phy_copy.access.lane_mask = 0x1 << (i + start_lane);
        /*next check 3 tap mode or 6 tap mode */
        if (init_config->tx[i].tap_mode == phymodTxTapMode3Tap) {
            if (init_config->tx[i].sig_method != phymodSignallingMethodNRZ) {
                PHYMOD_DEBUG_ERROR(("Unsupported signalling method.\n"));
                return PHYMOD_E_UNAVAIL;
            }

            PHYMOD_IF_ERR_RETURN
                (blackhawk7_l8p2_apply_txfir_cfg(&pm_phy_copy.access,
                                                 BLACKHAWK7_L8P2_NRZ_LP_3TAP,
                                                 0,
                                                 init_config->tx[i].pre,
                                                 init_config->tx[i].main,
                                                 init_config->tx[i].post,
                                                 0,
                                                 0));
        } else {
            if (init_config->tx[i].sig_method != phymodSignallingMethodNRZ) {
                PHYMOD_DEBUG_ERROR(("Unsupported signalling method.\n"));
                return PHYMOD_E_UNAVAIL;
            }
            PHYMOD_IF_ERR_RETURN
                (blackhawk7_l8p2_apply_txfir_cfg(&pm_phy_copy.access,
                                                 BLACKHAWK7_L8P2_NRZ_6TAP,
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
             (_tscbh_sub10g_phy_firmware_lane_config_set(&pm_phy_copy, firmware_lane_config));
    }

    /* Enable tx pcs interface delay mode */
    for (i = 0; i < num_lane; i++) {
        if (!PHYMOD_LANEPBMP_MEMBER(phy->access.lane_mask, start_lane + i)) {
            continue;
        }
        pm_phy_copy.access.lane_mask = 0x1 << (i + start_lane);
        PHYMOD_IF_ERR_RETURN
            (blackhawk7_l8p2_tx_pcs_intf_dly_mode_en(&pm_phy_copy.access, 1));
    }

    PHYMOD_MEMCPY(&pm_phy_copy, phy, sizeof(pm_phy_copy));
    PHYMOD_IF_ERR_RETURN
        (tbhmod_sub10g_update_port_mode(&pm_phy_copy.access));

    PHYMOD_IF_ERR_RETURN
        (temod2pll_rx_lane_control_set(&pm_phy_copy.access, 1));
    PHYMOD_IF_ERR_RETURN
        (temod2pll_tx_lane_control_set(&pm_phy_copy.access,
                                       TEMOD2PLL_TX_LANE_RESET_TRAFFIC_ENABLE));

    /* make sure that power up PLL is locked */
    pm_phy_copy.access.pll_idx = 0;
    pm_phy_copy.access.lane_mask = 1 << 0;
    PHYMOD_IF_ERR_RETURN
        (blackhawk7_l8p2_pll_pwrdn_get(&pm_phy_copy.access, &pll_power_down));

    /* need to check pll0 lock if not power up */
    /* put the check here is to save on boot up time */
    if (!pll_power_down) {
        uint32_t cnt = 0, pll_lock = 0;
        while (cnt < 1000) {
            PHYMOD_IF_ERR_RETURN(blackhawk7_l8p2_pll_lock_get(&pm_phy_copy.access, &pll_lock));
            cnt = cnt + 1;
            if (pll_lock) {
                break;
            } else {
                if (cnt == 1000) {
                    PHYMOD_DEBUG_ERROR(("WARNING :: core 0x%x PLL0 is not locked within 20 milli second \n", pm_phy_copy.access.addr));
                    break;
                }
            }
            PHYMOD_USLEEP(20);
        }
    }

    pm_phy_copy.access.pll_idx = 1;
    pm_phy_copy.access.lane_mask = 1 << 0;
    PHYMOD_IF_ERR_RETURN
        (blackhawk7_l8p2_pll_pwrdn_get(&pm_phy_copy.access, &pll_power_down));

    /* need to check pll1 lock if not power up */
    if (!pll_power_down) {
        uint32_t cnt = 0, pll_lock = 0;
        while (cnt < 1000) {
            PHYMOD_IF_ERR_RETURN(blackhawk7_l8p2_pll_lock_get(&pm_phy_copy.access, &pll_lock));
            cnt = cnt + 1;
            if (pll_lock) {
                break;
            } else {
                if (cnt == 1000) {
                    PHYMOD_DEBUG_ERROR(("WARNING :: core 0x%x PLL1 is not locked within 20 milli second \n", pm_phy_copy.access.addr));
                    break;
                }
            }
            PHYMOD_USLEEP(20);
        }
    }

    return PHYMOD_E_NONE;
}



int tscbh_sub10g_phy_loopback_set(const phymod_phy_access_t* phy, phymod_loopback_mode_t loopback, uint32_t enable)
{
    int i;
    int start_lane, num_lane;
    uint32_t port_enable;
    phymod_phy_access_t phy_copy;
    uint32_t cl72_en;

    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));

    /* next figure out the lane num and start_lane based on the input */
    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(&phy->access, &start_lane, &num_lane));

    if ((loopback != phymodLoopbackGlobal) &&
        (enable && (num_lane != TSCBH_SUB10G_NOF_LANES_IN_CORE))) {
        if (_tscbh_sub10g_phy_lane_swap_validate(phy)) {
            PHYMOD_DEBUG_WARN(("Warning: Digital and remote loopback will not operate as expected!\n"));
        }
    }

    switch (loopback) {
    case phymodLoopbackGlobal :
        PHYMOD_IF_ERR_RETURN
            (tscbh_sub10g_phy_cl72_get(&phy_copy, &cl72_en));
        if ((cl72_en == 1) && (enable == 1)) {
             PHYMOD_DEBUG_ERROR(("adr=%0"PRIx32",lane 0x%x: Error! pcs gloop not supported with cl72 enabled\n",
                                 phy_copy.access.addr, start_lane));
             break;
        }
        PHYMOD_IF_ERR_RETURN
            (tbhmod_sub10g_tx_loopback_control(&phy_copy.access, enable, start_lane, num_lane));
        break;
    case phymodLoopbackGlobalPMD :
        for (i = 0; i < num_lane; i++) {
            if (!PHYMOD_LANEPBMP_MEMBER(phy->access.lane_mask, start_lane + i)) {
                continue;
            }
            phy_copy.access.lane_mask = 0x1 << (i + start_lane);
            PHYMOD_IF_ERR_RETURN(blackhawk7_l8p2_tx_disable(&phy_copy.access, enable));
            PHYMOD_IF_ERR_RETURN(blackhawk7_l8p2_dig_lpbk(&phy_copy.access, (uint8_t) enable));
        }
        break;
    case phymodLoopbackRemotePMD :
        /* get current port enable bit */
        PHYMOD_IF_ERR_RETURN
            (temod2pll_disable_get(&phy_copy.access, &port_enable));
        if (port_enable) {
            phy_copy.access.lane_mask = 1 << start_lane;
            PHYMOD_IF_ERR_RETURN
                (temod2pll_disable_set(&phy_copy.access));
        }

        for (i = 0; i < num_lane; i++) {
            if (!PHYMOD_LANEPBMP_MEMBER(phy->access.lane_mask, start_lane + i)) {
                continue;
            }
            phy_copy.access.lane_mask = 0x1 << (i + start_lane);
            PHYMOD_IF_ERR_RETURN(blackhawk7_l8p2_rmt_lpbk(&phy_copy.access, (uint8_t)enable));
        }
        /* Release the pcs lane reset if port is enabled */
        if (port_enable) {
            phy_copy.access.lane_mask = 1 << start_lane;
            PHYMOD_IF_ERR_RETURN
                (tbhmod_sub10g_enable_set(&phy_copy.access));
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


int tscbh_sub10g_phy_loopback_get(const phymod_phy_access_t* phy, phymod_loopback_mode_t loopback, uint32_t* enable)
{
    int start_lane, num_lane;
    phymod_phy_access_t phy_copy;
    uint32_t enable_core;

    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));

    /*next figure out the lane num and start_lane based on the input*/
    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(&phy->access, &start_lane, &num_lane));

    switch (loopback) {
    case phymodLoopbackGlobal :
        PHYMOD_IF_ERR_RETURN(temod2pll_tx_loopback_get(&phy_copy.access, &enable_core));
        start_lane = (start_lane >= 4) ? (start_lane - 4) : start_lane;
        *enable = (enable_core >> start_lane) & 0x1;
        break;
    case phymodLoopbackGlobalPMD :
        PHYMOD_IF_ERR_RETURN(blackhawk7_l8p2_dig_lpbk_get(&phy_copy.access, enable));
        break;
    case phymodLoopbackRemotePMD :
        PHYMOD_IF_ERR_RETURN(blackhawk7_l8p2_rmt_lpbk_get(&phy_copy.access, enable));
        break;
    case phymodLoopbackRemotePCS :
    default :
        return PHYMOD_E_UNAVAIL;
    }
    return PHYMOD_E_NONE;
}


int tscbh_sub10g_phy_rx_pmd_locked_get(const phymod_phy_access_t* phy, uint32_t* pmd_lock)
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
            (blackhawk7_l8p2_pmd_lock_status(&pm_phy_copy.access, &tmp_lock));
        *pmd_lock &= (uint32_t) tmp_lock;
    }
    return PHYMOD_E_NONE;

}


int tscbh_sub10g_phy_rx_signal_detect_get(const phymod_phy_access_t* phy, uint32_t* signal_detect)
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
            (blackhawk7_l8p2_signal_detect(&pm_phy_copy.access, &tmp_detect));
        *signal_detect &= tmp_detect;
    }
    return PHYMOD_E_NONE;

}


int tscbh_sub10g_phy_link_status_get(const phymod_phy_access_t* phy, uint32_t* link_status)
{
    phymod_phy_access_t phy_copy;

    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));

    PHYMOD_IF_ERR_RETURN
        (temod2pll_get_pcs_latched_link_status(&phy_copy.access, link_status));
    return PHYMOD_E_NONE;
}


int tscbh_sub10g_phy_reg_read(const phymod_phy_access_t* phy, uint32_t reg_addr, uint32_t* val)
{
    /*pmd register read*/
    if (((reg_addr >> 16) & 0x1f) == 1) {
        PHYMOD_IF_ERR_RETURN(phymod_tscbh_iblk_read(&phy->access, reg_addr, val));
    } else {
        /*pcs register read*/
        PHYMOD_IF_ERR_RETURN(phymod_tsc_iblk_read(&phy->access, reg_addr, val));
    }
    return PHYMOD_E_NONE;
}


int tscbh_sub10g_phy_reg_write(const phymod_phy_access_t* phy, uint32_t reg_addr, uint32_t val)
{
    /*pmd register write*/
    if (((reg_addr >> 16) & 0x1f) == 1) {
        PHYMOD_IF_ERR_RETURN(phymod_tscbh_iblk_write(&phy->access, reg_addr, val));
    } else {
        /*pcs register write*/
        PHYMOD_IF_ERR_RETURN(phymod_tsc_iblk_write(&phy->access, reg_addr, val));
    }
    return PHYMOD_E_NONE;

}

int tscbh_sub10g_timesync_enable_set(const phymod_phy_access_t* phy, uint32_t flags, uint32_t enable)
{

    /* Place your code here */


    return PHYMOD_E_NONE;

}

int tscbh_sub10g_timesync_enable_get(const phymod_phy_access_t* phy, uint32_t flags, uint32_t* enable)
{

    /* Place your code here */


    return PHYMOD_E_NONE;

}


int tscbh_sub10g_timesync_offset_set(const phymod_core_access_t* core, uint32_t ts_offset)
{

    /* Place your code here */


    return PHYMOD_E_NONE;

}

int tscbh_sub10g_timesync_offset_get(const phymod_core_access_t* core, uint32_t* ts_offset)
{

    /* Place your code here */


    return PHYMOD_E_NONE;

}


int tscbh_sub10g_timesync_adjust_set(const phymod_phy_access_t* phy, uint32_t flags,
                                     const phymod_timesync_adjust_config_info_t* config_info)
{

    /* Place your code here */


    return PHYMOD_E_NONE;

}


int tscbh_sub10g_phy_tx_taps_default_get(const phymod_phy_access_t* phy, phymod_phy_signalling_method_t mode, phymod_tx_t* tx)
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
        PHYMOD_DEBUG_ERROR(("ERROR :: only support SignallingMethodNRZ \n"));
        return PHYMOD_E_UNAVAIL;
    }

    return PHYMOD_E_NONE;
}


int tscbh_sub10g_phy_lane_config_default_get(const phymod_phy_access_t* phy, phymod_phy_signalling_method_t mode, phymod_firmware_lane_config_t* lane_config)
{
    /* default always assume backplane as the medium type and with dfe on */
    if (mode == phymodSignallingMethodNRZ) {
        lane_config->ForceNRZMode = 1;
        lane_config->ForcePAM4Mode = 0;
        lane_config->ForceNormalReach  = 0;
    } else {
        PHYMOD_DEBUG_ERROR(("ERROR :: only support SignallingMethodNRZ \n"));
        return PHYMOD_E_UNAVAIL;
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


int tscbh_sub10g_phy_firmware_load_info_get(const phymod_phy_access_t* phy, phymod_firmware_load_info_t* info)
{
    phymod_phy_access_t pm_phy_copy;
    uint8_t ver_id, afe_ver_a0 = 0;

    PHYMOD_MEMCPY(&pm_phy_copy, phy, sizeof(pm_phy_copy));

    PHYMOD_IF_ERR_RETURN(blackhawk7_l8p2_afe_version_get(&pm_phy_copy.access, &ver_id));
    if (ver_id == AMS_VERSION_A0) {
       afe_ver_a0 = 1;
       info->ucode_len = BLACKHAWK7_V1L8P2_UCODE_IMAGE_SIZE;
    } else {
       info->ucode_len = BLACKHAWK7_V2L8P2_UCODE_IMAGE_SIZE;
    }
    info->ucode_ptr = blackhawk7_l8p2_ucode_get(afe_ver_a0, 0);

    return PHYMOD_E_NONE;
}


int tscbh_sub10g_phy_pll_reconfig(const phymod_phy_access_t* phy, uint8_t pll_index, uint32_t pll_div, phymod_ref_clk_t ref_clk)
{
    phymod_phy_access_t pm_phy_copy;
    uint32_t pll_is_pwrdn;
    uint8_t tvco_pll_index, tvco_reconfig = 0;
    enum blackhawk7_l8p2_pll_refclk_enum refclk;
    uint32_t cnt = 0, pll_lock = 0;

    PHYMOD_MEMCPY(&pm_phy_copy, phy, sizeof(pm_phy_copy));
    tvco_pll_index = phy->access.tvco_pll_index;

    if (pll_index > 1) {
        PHYMOD_DEBUG_ERROR(("Unsupported PLL index\n"));
        return PHYMOD_E_UNAVAIL;
    }

    tvco_reconfig = (tvco_pll_index == pll_index) ? 1 : 0;

    if (ref_clk == phymodRefClk156Mhz) {
        refclk = BLACKHAWK7_L8P2_PLL_REFCLK_156P25MHZ;
    } else {
        PHYMOD_DEBUG_ERROR(("ERROR :: refclk should be 156.25M \n"));
        return PHYMOD_E_UNAVAIL;
    }

    /* next disable pcs datapath only if TVCO re-config*/
    if (tvco_reconfig) {
        PHYMOD_MEMCPY(&pm_phy_copy, phy, sizeof(pm_phy_copy));
        pm_phy_copy.access.lane_mask = 0xf0;
        PHYMOD_IF_ERR_RETURN
            (temod2pll_disable_set(&pm_phy_copy.access));
        pm_phy_copy.access.lane_mask = 0xf;
        PHYMOD_IF_ERR_RETURN
            (temod2pll_disable_set(&pm_phy_copy.access));
        PHYMOD_MEMCPY(&pm_phy_copy, phy, sizeof(pm_phy_copy));
    }

    /*next check if the PLL is power down or not */
    pm_phy_copy.access.pll_idx = pll_index;
    pm_phy_copy.access.lane_mask = 1 << 0;
    PHYMOD_IF_ERR_RETURN
        (blackhawk7_l8p2_pll_pwrdn_get(&pm_phy_copy.access, &pll_is_pwrdn));

    /* if PLL is power down, need to power up first */
    if (pll_is_pwrdn) {
        /* Power up ovco if it's power down */
        PHYMOD_IF_ERR_RETURN
            (blackhawk7_l8p2_core_pwrdn(&pm_phy_copy.access, PWR_ON));
    }

    /* Delay 300ms as PLL not lock Workaround */
    PHYMOD_USLEEP(300000);
    /*next re-config pll divider */
    if (pll_div == TBHMOD_SUB10G_PLL_MODE_DIV_132 ||
        pll_div == TBHMOD_SUB10G_PLL_MODE_DIV_66) {
        /* Corresponds to 20G VCO */
        PHYMOD_IF_ERR_RETURN
            (blackhawk7_l8p2_configure_20P625G_pll_div(&pm_phy_copy.access,
                                                       pll_div));
    } else {
        PHYMOD_IF_ERR_RETURN
            (blackhawk7_l8p2_core_dp_reset(&pm_phy_copy.access, 1));
        PHYMOD_IF_ERR_RETURN
            (blackhawk7_l8p2_configure_pll_refclk_div(&pm_phy_copy.access,
                                                      refclk,
                                                      pll_div));
        PHYMOD_IF_ERR_RETURN
            (blackhawk7_l8p2_core_dp_reset(&pm_phy_copy.access, 0));
    }

    /* need to wait for the PLL lock */
    cnt = 0;
    while (cnt < 1000) {
        PHYMOD_IF_ERR_RETURN(blackhawk7_l8p2_pll_lock_get(&pm_phy_copy.access, &pll_lock));
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



int tscbh_sub10g_phy_synce_clk_ctrl_set(const phymod_phy_access_t* phy, phymod_synce_clk_ctrl_t cfg)
{
    phymod_phy_access_t phy_copy;
    int start_lane, num_lane;
    uint32_t current_pll_index, sdm_val;
    int osr_mode;
    int32_t pll_div;

    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(&phy->access, &start_lane, &num_lane));

    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));
    if (start_lane >= 4) {
        start_lane -= 4;
    }
    phy_copy.access.lane_mask = 0x1 << start_lane;

    PHYMOD_IF_ERR_RETURN
        (temod2pll_synce_stg0_mode_set(&phy_copy.access, cfg.stg0_mode));

    PHYMOD_IF_ERR_RETURN
        (temod2pll_synce_stg1_mode_set(&phy_copy.access, cfg.stg1_mode));

    if ((cfg.stg0_mode == 0x2) && (cfg.stg1_mode == 0x0)) {
        /* vco the port is using */
        /* first get the PLL index */
        PHYMOD_IF_ERR_RETURN
            (blackhawk7_l8p2_lane_pll_selection_get(&phy_copy.access, &current_pll_index));

        /* next get the OSR the port is current using */
        PHYMOD_IF_ERR_RETURN
            (blackhawk7_l8p2_osr_mode_get(&phy_copy.access, &osr_mode));

        /* next get the VCO the port is current using */
        phy_copy.access.pll_idx = current_pll_index;
        phy_copy.access.lane_mask = 0x1;
        PHYMOD_IF_ERR_RETURN
            (blackhawk7_l8p2_INTERNAL_read_pll_div(&phy_copy.access, (uint32_t *)&pll_div));

        /* next based on the VCO value and osr mode, set the SDM value properly */
       if ((pll_div == TBHMOD_SUB10G_PLL_MODE_DIV_132) || (pll_div == TBHMOD_SUB10G_PLL_MODE_DIV_66)) {
            switch (osr_mode) {
                case TSCBH_SUB10G_OS_MODE_2:
                    sdm_val = TSCBH_SUB10G_SYNCE_SDM_DIVISOR_10G_PER_LN;
                    break;
                case  TSCBH_SUB10G_OS_MODE_4:
                    sdm_val = TSCBH_SUB10G_SYNCE_SDM_DIVISOR_5G_PER_LN;
                    break;
                case TSCBH_SUB10G_OS_MODE_16p5:
                    sdm_val = TSCBH_SUB10G_SYNCE_SDM_DIVISOR_1G_PER_LN;
                    break;
                default:
                    PHYMOD_DEBUG_ERROR(("Unsupported osmode\n"));
                    return PHYMOD_E_UNAVAIL;
            }
        } else if ((pll_div == TBHMOD_SUB10G_PLL_MODE_DIV_160) || (pll_div == TBHMOD_SUB10G_PLL_MODE_DIV_80)) {
            switch (osr_mode) {
                case TSCBH_SUB10G_OS_MODE_8:
                    sdm_val = TSCBH_SUB10G_SYNCE_SDM_DIVISOR_2P5G_PER_LN;
                    break;
                default:
                    PHYMOD_DEBUG_ERROR(("Unsupported osmode\n"));
                    return PHYMOD_E_UNAVAIL;
            }
        } else {
            PHYMOD_DEBUG_ERROR(("Unsupported VCOs\n"));
            return PHYMOD_E_UNAVAIL;
        }

        /* next configure the SDM value */
        PHYMOD_IF_ERR_RETURN
            (temod2pll_synce_clk_ctrl_set(&phy_copy.access, sdm_val));
    }

    return PHYMOD_E_NONE;

}

int tscbh_sub10g_phy_synce_clk_ctrl_get(const phymod_phy_access_t* phy, phymod_synce_clk_ctrl_t* cfg)
{
    phymod_phy_access_t phy_copy;
    int start_lane, num_lane;

    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(&phy->access, &start_lane, &num_lane));
    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));
    if (start_lane >= 4) {
        start_lane -= 4;
    }
    phy_copy.access.lane_mask = 0x1 << start_lane;

    PHYMOD_IF_ERR_RETURN
        (temod2pll_synce_stg0_mode_get(&phy_copy.access, &(cfg->stg0_mode)));

    PHYMOD_IF_ERR_RETURN
        (temod2pll_synce_stg1_mode_get(&phy_copy.access, &(cfg->stg1_mode)));

    PHYMOD_IF_ERR_RETURN
        (temod2pll_synce_clk_ctrl_get(&phy_copy.access, &(cfg->sdm_val)));

    return PHYMOD_E_NONE;
}


int tscbh_sub10g_phy_rx_ppm_get(const phymod_phy_access_t* phy, int16_t* rx_ppm)
{
    int start_lane, num_lane;
    phymod_phy_access_t pm_phy_copy;

    PHYMOD_MEMCPY(&pm_phy_copy, phy, sizeof(pm_phy_copy));

    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(&phy->access, &start_lane, &num_lane));

    pm_phy_copy.access.lane_mask = 1 << start_lane;
    PHYMOD_IF_ERR_RETURN
        (blackhawk7_l8p2_rx_ppm(&pm_phy_copy.access, rx_ppm));
    return PHYMOD_E_NONE;
}


int tscbh_sub10g_timesync_tx_info_get(const phymod_phy_access_t* phy, phymod_ts_fifo_status_t* ts_tx_info)
{

    /* Place your code here */


    return PHYMOD_E_NONE;

}


int tscbh_sub10g_phy_pll_powerdown_get(const phymod_phy_access_t* phy, uint32_t pll_index, uint32_t* powerdown)
{
    phymod_phy_access_t phy_copy;

    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));

    phy_copy.access.pll_idx = pll_index;
    phy_copy.access.lane_mask = 1 << 0;
    PHYMOD_IF_ERR_RETURN
        (blackhawk7_l8p2_pll_pwrdn_get(&phy_copy.access, powerdown));

    return PHYMOD_E_NONE;
}

static
int _tscbh_sub10g_phy_power_mode_map(phymod_power_mode_t power_mode, enum srds_core_pwrdn_mode_enum *pwrdn_mode)
{
    switch (power_mode) {
    case phymodSerdesPWR_ON:
        *pwrdn_mode = PWR_ON;
        break;
    case phymodSerdesPWRDN:
        *pwrdn_mode = PWRDN;
        break;
    case phymodSerdesPWRDN_DEEP:
        *pwrdn_mode = PWRDN_DEEP;
        break;
    case phymodSerdesPWRDN_TX:
        *pwrdn_mode = PWRDN_TX;
        break;
    case phymodSerdesPWRDN_RX:
        *pwrdn_mode = PWRDN_RX;
        break;
    case phymodSerdesPWROFF_DEEP:
        *pwrdn_mode = PWROFF_DEEP;
        break;
    case phymodSerdesPWRDN_TX_DEEP:
        *pwrdn_mode = PWRDN_TX_DEEP;
        break;
    case phymodSerdesPWRDN_RX_DEEP:
        *pwrdn_mode = PWRDN_RX_DEEP;
        break;
    case phymodSerdesPWR_ON_TX:
        *pwrdn_mode = PWR_ON_TX;
        break;
    case phymodSerdesPWR_ON_RX:
        *pwrdn_mode = PWR_ON_RX;
        break;
    default:
        return PHYMOD_E_PARAM;
    }

    return PHYMOD_E_NONE;
}
int tscbh_sub10g_phy_serdes_power_set(const phymod_phy_access_t* phy, phymod_power_option_t power_option, phymod_power_mode_t power_mode)
{
    phymod_phy_access_t phy_copy;
    int start_lane, num_lane, i;
    enum srds_core_pwrdn_mode_enum pwrdn_mode;

    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(&phy->access, &start_lane, &num_lane));
    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));

    PHYMOD_IF_ERR_RETURN
        (_tscbh_sub10g_phy_power_mode_map(power_mode, &pwrdn_mode));
    switch (power_option) {
        case phymodSerdesOptionLane:
            for( i = 0; i < num_lane; i++) {
                if (!PHYMOD_LANEPBMP_MEMBER(phy->access.lane_mask, start_lane + i)) {
                    continue;
                }
                phy_copy.access.lane_mask = 1 << (start_lane + i);
                PHYMOD_IF_ERR_RETURN
                    (blackhawk7_l8p2_lane_pwrdn(&phy_copy.access, pwrdn_mode));
            }
            PHYMOD_IF_ERR_RETURN
                (blackhawk7_l8p2_lane_soft_reset(&phy_copy.access, 0));
            break;
        case phymodSerdesOptionCore:
            phy_copy.access.lane_mask = 0x1;
            PHYMOD_IF_ERR_RETURN
                (blackhawk7_l8p2_core_pwrdn(&phy_copy.access, pwrdn_mode));
            if (power_mode == phymodSerdesPWR_ON) {
                PHYMOD_IF_ERR_RETURN
                    (blackhawk7_l8p2_core_dp_reset(&phy_copy.access, 0));
            }
            if (power_mode == phymodSerdesPWRDN_DEEP) {
                /*clear the pll select after core power down*/
                for (i = 0; i < TSCBH_SUB10G_NOF_LANES_IN_CORE; i++) {
                    phy_copy.access.lane_mask = 1 << i;
                    PHYMOD_IF_ERR_RETURN
                        (blackhawk7_l8p2_lane_pll_selection_set(&phy_copy.access,
                                                                phy->access.tvco_pll_index));
                }
            }
            break;
        case phymodSerdesOptionAll:
            PHYMOD_IF_ERR_RETURN
                (blackhawk7_l8p2_pwrdn_all(&phy_copy.access, pwrdn_mode));
            break;
        default:
            PHYMOD_RETURN_WITH_ERR(PHYMOD_E_UNAVAIL,
                               (_PHYMOD_MSG("This power option is not supported\n")));
            break;
    }

    return PHYMOD_E_NONE;
}


#endif /* PHYMOD_TSCBH_SUB10G_SUPPORT */
