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
#include <phymod/phymod_util.h>
#include <phymod/phymod_dispatch.h>
#include <phymod/chip/bcmi_tscbh_fe_gen2_xgxs_defs.h>
#include <phymod/chip/tscbh_fe_gen2.h>
#include "tscbh/tier1/tbhmod.h"
#include "tscbh_fe_gen2/tier1/tbhmod_fe_gen2.h"
#include "tscbh_fe_gen2/tier1/tbhmod_fe_gen2_sc_lkup_table.h"
#include "blackhawk7_l2p2/tier2/blackhawk7_l2p2_ucode.h"
#include "blackhawk7_l2p2/tier2/blackhawk7_l2p2_ucode.h"
#include "blackhawk7_l2p2/tier1/blackhawk7_l2p2_cfg_seq.h"
#include "blackhawk7_l2p2/tier1/blackhawk7_l2p2_enum.h"
#include "blackhawk7_l2p2/tier1/blackhawk7_l2p2_common.h"
#include "blackhawk7_l2p2/tier1/blackhawk7_l2p2_interface.h"
#include "blackhawk7_l2p2/tier1/blackhawk7_l2p2_dependencies.h"
#include "blackhawk7_l2p2/tier1/blackhawk7_l2p2_internal.h"
#include "blackhawk7_l2p2/tier1/public/blackhawk7_api_uc_vars_rdwr_defns_public.h"
#include "blackhawk7_l2p2/tier1/blackhawk7_l2p2_access.h"

#define TSCBH_FE_GEN2_SERDES_ID        0x2f /* 0x9008 Main0_serdesID - Serdes ID Register */
#define TSCBH_FE_GEN2_ID0              0x600d
#define TSCBH_FE_GEN2_ID1              0x8770


/* this file contains the initial code based on gen2,
   needs to be updated according to FE GEN2's changelist.
*/

#define SPEED_ID_INDEX_100G_4_LANE_CL73_KR4     0x6
#define SPEED_ID_INDEX_100G_4_LANE_CL73_CR4     0x7

#define PMD_OSR_2P5                    0x3

/* PMD reg access */
#define IS_PMD_ACCESS(addr)       ((addr >> 16) & 0x1)

int tscbh_fe_gen2_core_identify(const phymod_core_access_t* core,
                                uint32_t core_id,
                                uint32_t* is_identified)
{
    PHYID2r_t id2;
    PHYID3r_t id3;
    MAIN0_SERDESIDr_t serdesid;
    int ioerr = 0;

    *is_identified = 0;
    ioerr += READ_PHYID2r(&core->access, &id2);
    ioerr += READ_PHYID3r(&core->access, &id3);

    if (PHYID2r_REGID1f_GET(id2) == TSCBH_FE_GEN2_ID0 &&
       (PHYID3r_REGID2f_GET(id3) == TSCBH_FE_GEN2_ID1)) {
        /* PHY IDs match - now check PCS model */
        ioerr += READ_MAIN0_SERDESIDr(&core->access, &serdesid);
        if ((MAIN0_SERDESIDr_MODEL_NUMBERf_GET(serdesid)) == TSCBH_FE_GEN2_SERDES_ID) {
            *is_identified = 1;
        }
    }
    return ioerr ? PHYMOD_E_IO : PHYMOD_E_NONE;
}

int tscbh_fe_gen2_core_info_get(const phymod_core_access_t* core, phymod_core_info_t* info)
{
    int rv = 0;
    MAIN0_SERDESIDr_t serdes_id;
    char core_name[15] = "TscbhFeGen2";
    PHYID2r_t id2;
    PHYID3r_t id3;
    const phymod_access_t *pm_acc = &core->access;

    rv = READ_MAIN0_SERDESIDr(&core->access, &serdes_id);

    info->serdes_id = MAIN0_SERDESIDr_GET(serdes_id);
    PHYMOD_IF_ERR_RETURN
        (phymod_core_name_get(core, MAIN0_SERDESIDr_GET(serdes_id), core_name, info));
    info->serdes_id = MAIN0_SERDESIDr_GET(serdes_id);
    info->core_version = phymodCoreVersionTscbhFeGen2;

    PHYMOD_IF_ERR_RETURN(READ_PHYID2r(pm_acc, &id2));
    PHYMOD_IF_ERR_RETURN(READ_PHYID3r(pm_acc, &id3));

    info->phy_id0 = (uint16_t) id2.v[0];
    info->phy_id1 = (uint16_t) id3.v[0];

    return rv;
}

/*
 * set lane swapping for core
 */

int tscbh_fe_gen2_core_lane_map_set(const phymod_core_access_t* core,
                                    const phymod_lane_map_t* lane_map)
{
    phymod_core_access_t  core_copy;
    uint32_t lane, pcs_tx_swap = 0, pcs_rx_swap = 0/*, pmd_index = 0*/;
    uint8_t pmd_tx_addr[8], pmd_rx_addr[8];
    uint8_t physical_tx_lane = 0, physical_rx_lane = 0, pmd_index = 0;

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
    PHYMOD_IF_ERR_RETURN
        (tbhmod_pcs_tx_lane_swap(&core_copy.access, pcs_tx_swap));
    PHYMOD_IF_ERR_RETURN
        (tbhmod_pcs_rx_lane_swap(&core_copy.access, pcs_rx_swap));

    /* PMD lane mapping */
    for (lane = 0; lane < TSCBH_NOF_LANES_IN_CORE; lane++){
        physical_tx_lane = lane_map->lane_map_tx[lane];
        physical_rx_lane = lane_map->lane_map_rx[lane];
        /* default TX/RX_LANE_ADDR_0 = 0x0, TX_RX_LANE_ADDR_1 = 0x1 */
        pmd_tx_addr[physical_tx_lane] = (physical_tx_lane % 2 == 0)? 0 : 1;
        pmd_rx_addr[physical_rx_lane] = (physical_rx_lane % 2 == 0)? 0 : 1;
        /* logical lane is mapped to different physical lane TX and RX */
        if (physical_tx_lane != physical_rx_lane) {
            /* update with TX_LANE_ADDR_0 = 0x1 or TX_LANE_ADDR_1 = 0x0 */
            /* RX_LANE_ADDR_0/1 stays at default */
            pmd_tx_addr[physical_tx_lane] = (physical_tx_lane % 2 == 0)? 1 : 0;
        }
    }

    /* PMD lane map is needed when Rx & Tx are not mapped to the same logical lane*/
    for (pmd_index = 0; pmd_index < BLACKHAWK7_L2P2_NOF_INSTANCE; pmd_index++) {
        for (lane = 0; lane < TSCBH_NOF_LANES_IN_CORE; lane++) {
            /* find logical lanes that are mapped to physical lane 0,2,4,6 */
            if ((pmd_index * 2) == (lane_map->lane_map_tx[lane])) {
                core_copy.access.lane_mask = 0x1 << lane;
                PHYMOD_IF_ERR_RETURN
                   (blackhawk7_l2p2_map_lanes(&core_copy.access,
                       BLACKHAWK7_L2P2_NOF_LANES_IN_CORE,
                       &(pmd_tx_addr[pmd_index*2]), &(pmd_rx_addr[pmd_index*2]) ));
                break;
            }
        }
    }

    return PHYMOD_E_NONE;
}

int tscbh_fe_gen2_core_lane_map_get(const phymod_core_access_t* core,
                                    phymod_lane_map_t* lane_map)
{
    phymod_core_access_t  core_copy;
    uint32_t lane, pcs_tx_swap = 0, pcs_rx_swap = 0;

    PHYMOD_MEMCPY(&core_copy, core, sizeof(core_copy));
    core_copy.access.lane_mask = 0x1;

    PHYMOD_IF_ERR_RETURN
        (tbhmod_fe_gen2_pcs_tx_lane_swap_get(&core_copy.access, &pcs_tx_swap));
    PHYMOD_IF_ERR_RETURN
        (tbhmod_fe_gen2_pcs_rx_lane_swap_get(&core_copy.access, &pcs_rx_swap));

    for (lane = 0; lane < TSCBH_NOF_LANES_IN_CORE; lane++){
        /*decode each lane as four bits*/
        lane_map->lane_map_tx[(pcs_tx_swap >> (lane*4)) & 0xf] = lane;
        lane_map->lane_map_rx[(pcs_rx_swap >> (lane*4)) & 0xf] = lane;
    }

    return PHYMOD_E_NONE;
}

static
int _tscbh_fe_gen2_phy_firmware_lane_config_set(const phymod_phy_access_t* phy, phymod_firmware_lane_config_t fw_config)
{
    uint32_t is_warm_boot;
    struct blackhawk7_l2p2_uc_lane_config_st serdes_firmware_config;
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
        serdes_firmware_config.field.force_er               = fw_config.ForceExtenedReach;
        serdes_firmware_config.field.force_nr               = fw_config.ForceNormalReach;
        serdes_firmware_config.field.force_nrz_mode         = fw_config.ForceNRZMode;
        serdes_firmware_config.field.force_pam4_mode        = fw_config.ForcePAM4Mode;
        serdes_firmware_config.field.lp_has_prec_en         = fw_config.LpPrecoderEnabled;

        PHYMOD_IF_ERR_RETURN(PHYMOD_IS_WRITE_DISABLED(&phy->access, &is_warm_boot));

        if (!is_warm_boot) {
            PHYMOD_IF_ERR_RETURN(blackhawk7_l2p2_lane_soft_reset_get(&phy_copy.access, &rst_status));
            if (!rst_status) PHYMOD_IF_ERR_RETURN (blackhawk7_l2p2_lane_soft_reset(&phy_copy.access, 1));
            PHYMOD_IF_ERR_RETURN(blackhawk7_l2p2_set_uc_lane_cfg(&phy_copy.access, serdes_firmware_config));
            if (!rst_status) PHYMOD_IF_ERR_RETURN (blackhawk7_l2p2_lane_soft_reset(&phy_copy.access, 0));
        }
    }

    return PHYMOD_E_NONE;
}

int tscbh_fe_gen2_phy_firmware_lane_config_set(const phymod_phy_access_t* phy,
                                               phymod_firmware_lane_config_t fw_config)
{
    phymod_phy_access_t phy_copy;
    uint32_t lane_reset, pcs_lane_enable, port_lane_mask;
    int start_lane, num_lane, port_start_lane, port_num_lane;

    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));
    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(&phy->access, &start_lane, &num_lane));

    /*get the start lane of the port lane mask */
    PHYMOD_IF_ERR_RETURN
        (tbhmod_port_start_lane_get(&phy_copy.access, &port_start_lane, &port_num_lane));
    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_mask_get(port_start_lane, port_num_lane, &port_lane_mask));

    /*first check if lane is in reset */
    PHYMOD_IF_ERR_RETURN
        (blackhawk7_l2p2_lane_soft_reset_get(&phy_copy.access, &lane_reset));

    /*next check if PCS lane is in reset */
    phy_copy.access.lane_mask = 1 << port_start_lane;
    PHYMOD_IF_ERR_RETURN
        (tbhmod_enable_get(&phy_copy.access, &pcs_lane_enable));

    /* disable pcs lane if pcs lane not in rset */
    if (pcs_lane_enable) {
        phy_copy.access.lane_mask = 1 << port_start_lane;
        PHYMOD_IF_ERR_RETURN
            (tbhmod_disable_set(&phy_copy.access));
        /* add the pcs disable SW WAR */
        phy_copy.access.lane_mask = port_lane_mask;
        PHYMOD_IF_ERR_RETURN
            (tbhmod_fe_gen2_pcs_reset_sw_war(&phy_copy.access));
    }

    /* if lane is not in reset, then reset the lane first */
    if (!lane_reset) {
        PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));
        PHYMOD_IF_ERR_RETURN
            (blackhawk7_l2p2_lane_soft_reset(&phy_copy.access, 1));
    }

    PHYMOD_IF_ERR_RETURN
         (_tscbh_fe_gen2_phy_firmware_lane_config_set(phy, fw_config));

    if (!lane_reset) {
        PHYMOD_IF_ERR_RETURN
            (blackhawk7_l2p2_lane_soft_reset(&phy_copy.access, 0));
    }

    /* re-enable pcs lane if pcs lane not in rset */
    if (pcs_lane_enable) {
        phy_copy.access.lane_mask = 1 << port_start_lane;
        PHYMOD_IF_ERR_RETURN
            (tbhmod_enable_set(&phy_copy.access));
    }

    return PHYMOD_E_NONE;
}

/* load tscbh fw. the fw_loader parameter is valid just for external fw load*/
STATIC
int _tscbh_fe_gen2_core_firmware_load(const phymod_core_access_t* core, const phymod_core_init_config_t* init_config)
{
    phymod_core_access_t  core_copy;
    unsigned int blackhawk_ucode_len;
    unsigned char *blackhawk_ucode;

    PHYMOD_MEMCPY(&core_copy, core, sizeof(core_copy));

    blackhawk_ucode_len = BLACKHAWK7_V1L2P2_UCODE_IMAGE_SIZE;
    blackhawk_ucode = blackhawk7_l2p2_ucode_get();

    switch(init_config->firmware_load_method){
    case phymodFirmwareLoadMethodInternal:
        PHYMOD_IF_ERR_RETURN(blackhawk7_l2p2_ucode_load(&core_copy.access, blackhawk_ucode, blackhawk_ucode_len));
        break;
    case phymodFirmwareLoadMethodExternal:
        PHYMOD_NULL_CHECK(init_config->firmware_loader);
        PHYMOD_IF_ERR_RETURN(blackhawk7_l2p2_ucode_init(&core_copy.access));
        PHYMOD_IF_ERR_RETURN
            (blackhawk7_l2p2_pram_firmware_enable(&core_copy.access, 1, 0));
        PHYMOD_IF_ERR_RETURN(init_config->firmware_loader(core, blackhawk_ucode_len, blackhawk_ucode));
        PHYMOD_IF_ERR_RETURN
            (blackhawk7_l2p2_pram_firmware_enable(&core_copy.access, 0, 0));
        break;
    case phymodFirmwareLoadMethodNone:
        break;
    default:
        PHYMOD_RETURN_WITH_ERR(PHYMOD_E_CONFIG, (_PHYMOD_MSG("illegal fw load method %u"), init_config->firmware_load_method));
    }

    return PHYMOD_E_NONE;

}

int tscbh_fe_gen2_phy_firmware_lane_config_get(const phymod_phy_access_t* phy,
                                               phymod_firmware_lane_config_t* fw_config)
{
    struct blackhawk7_l2p2_uc_lane_config_st lane_config;
    phymod_phy_access_t pm_phy_copy;

    PHYMOD_MEMCPY(&pm_phy_copy, phy, sizeof(pm_phy_copy));

    PHYMOD_MEMSET(&lane_config, 0x0, sizeof(lane_config));
    PHYMOD_MEMSET(fw_config, 0, sizeof(*fw_config));

    PHYMOD_IF_ERR_RETURN
        (blackhawk7_l2p2_get_uc_lane_cfg(&pm_phy_copy.access, &lane_config));

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

int tscbh_fe_gen2_phy_polarity_set(const phymod_phy_access_t* phy, const phymod_polarity_t* polarity)
{
    phymod_phy_access_t phy_copy, pm_phy_copy;
    int i, start_lane, num_lane;
    uint32_t lane_reset, pcs_lane_enable;
    uint32_t tx_polarity, rx_polarity;

    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));
    PHYMOD_MEMCPY(&pm_phy_copy, phy, sizeof(pm_phy_copy));

    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(&phy->access, &start_lane, &num_lane));

    /*first check if lane is in reset */
    PHYMOD_IF_ERR_RETURN
        (blackhawk7_l2p2_lane_soft_reset_get(&pm_phy_copy.access, &lane_reset));

    /*next check if PCS lane is in reset */
    PHYMOD_IF_ERR_RETURN
        (tbhmod_enable_get(&pm_phy_copy.access, &pcs_lane_enable));

    /* disable pcs lane if pcs lane was enabled */
    if (pcs_lane_enable) {
        pm_phy_copy.access.lane_mask = 1 << start_lane;
        PHYMOD_IF_ERR_RETURN
            (tbhmod_disable_set(&pm_phy_copy.access));
        /* add the pcs disable SW WAR */
        PHYMOD_IF_ERR_RETURN
            (tbhmod_fe_gen2_pcs_reset_sw_war(&phy->access));
    }

    /* if lane is not in reset, then reset the lane first */
    if (!lane_reset) {
        PHYMOD_MEMCPY(&pm_phy_copy, phy, sizeof(pm_phy_copy));
        PHYMOD_IF_ERR_RETURN
            (blackhawk7_l2p2_lane_soft_reset(&pm_phy_copy.access, 1));
    }

    /* Set the tx_rx_polarity */
    for (i = 0; i < num_lane; i++) {
        if (!PHYMOD_LANEPBMP_MEMBER(phy->access.lane_mask, start_lane + i)) {
            continue;
        }
        phy_copy.access.lane_mask = 0x1 << (i + start_lane);
        tx_polarity = ((polarity->tx_polarity >> i) & 0x1);
        rx_polarity = ((polarity->rx_polarity >> i) & 0x1);
        PHYMOD_IF_ERR_RETURN
            (blackhawk7_l2p2_tx_rx_polarity_set(&phy_copy.access, tx_polarity, rx_polarity));
    }

    /* release the ln dp reset */
    if (!lane_reset) {
        PHYMOD_MEMCPY(&pm_phy_copy, phy, sizeof(pm_phy_copy));
        PHYMOD_IF_ERR_RETURN
            (blackhawk7_l2p2_lane_soft_reset(&pm_phy_copy.access, 0));
    }

    /* re-enable pcs lane if pcs lane was enabled*/
    if (pcs_lane_enable) {
        pm_phy_copy.access.lane_mask = 1 << start_lane;
        PHYMOD_IF_ERR_RETURN
            (tbhmod_enable_set(&pm_phy_copy.access));
    }

    return PHYMOD_E_NONE;
}

int tscbh_fe_gen2_phy_polarity_get(const phymod_phy_access_t* phy, phymod_polarity_t* polarity)
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
            (blackhawk7_l2p2_tx_rx_polarity_get(&phy_copy.access, &temp_pol.tx_polarity, &temp_pol.rx_polarity));
        polarity->tx_polarity |= ((temp_pol.tx_polarity & 0x1) << i);
        polarity->rx_polarity |= ((temp_pol.rx_polarity & 0x1) << i);
    }

    return PHYMOD_E_NONE;
}

int tscbh_fe_gen2_phy_tx_set(const phymod_phy_access_t* phy, const phymod_tx_t* tx)
{
    phymod_phy_access_t phy_copy;
    int start_lane, num_lane, i, port_start_lane, port_num_lane, rv = PHYMOD_E_NONE;
    enum blackhawk7_l2p2_txfir_tap_enable_enum enable_taps = BLACKHAWK7_L2P2_NRZ_6TAP;
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
        (blackhawk7_l2p2_lane_soft_reset_get(&phy_copy.access, &lane_reset));

    /* disable pcs lane if pcs lane not in rset */
    if (pcs_lane_enable) {
        phy_copy.access.lane_mask = 1 << port_start_lane;
        PHYMOD_IF_ERR_RETURN
            (tbhmod_disable_set(&phy_copy.access));
        /* add the pcs disable SW WAR */
        phy_copy.access.lane_mask = port_lane_mask;
        PHYMOD_IF_ERR_RETURN
            (tbhmod_fe_gen2_pcs_reset_sw_war(&phy_copy.access));
    }
    if (!lane_reset) {
        PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));
        PHYMOD_IF_ERR_RETURN
            (blackhawk7_l2p2_lane_soft_reset(&phy_copy.access, 1));
    }

    for (i = 0; i < num_lane; i++) {
        if (!PHYMOD_LANEPBMP_MEMBER(phy->access.lane_mask, start_lane + i)) {
            continue;
        }
        phy_copy.access.lane_mask = 1 << (start_lane + i);
        /*next check 3 tap mode or 6 tap mode */
        if (tx->tap_mode == phymodTxTapMode3Tap) {
            if (tx->sig_method == phymodSignallingMethodNRZ) {
                enable_taps= BLACKHAWK7_L2P2_NRZ_LP_3TAP;
            } else {
                enable_taps= BLACKHAWK7_L2P2_PAM4_LP_3TAP;
            }
            /* rv will be returned at the end of function */
            rv |= blackhawk7_l2p2_apply_txfir_cfg(&phy_copy.access,
                                                 enable_taps,
                                                 0,
                                                 tx->pre,
                                                 tx->main,
                                                 tx->post,
                                                 0,
                                                 0);
        } else {
            if (tx->sig_method == phymodSignallingMethodNRZ) {
                enable_taps= BLACKHAWK7_L2P2_NRZ_6TAP;
            } else {
                enable_taps= BLACKHAWK7_L2P2_PAM4_6TAP;
            }
            /* rv will be returned at the end of function */
            rv |= blackhawk7_l2p2_apply_txfir_cfg(&phy_copy.access,
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
            (blackhawk7_l2p2_lane_soft_reset(&phy_copy.access, 0));
    }

    /* re-enable pcs lane if pcs lane not in rset */
    if (pcs_lane_enable) {
        phy_copy.access.lane_mask = 1 << port_start_lane;
        PHYMOD_IF_ERR_RETURN
            (tbhmod_enable_set(&phy_copy.access));
    }
    return rv;
}

int tscbh_fe_gen2_phy_tx_get(const phymod_phy_access_t* phy, phymod_tx_t* tx)
{
    uint8_t pmd_tx_tap_mode;
    uint16_t tx_tap_nrz_mode = 0;
    int16_t val;
    phymod_phy_access_t phy_copy;
    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));

    /* read current tx tap mode */
    PHYMOD_IF_ERR_RETURN
        (blackhawk7_l2p2_tx_tap_mode_get(&phy_copy.access, &pmd_tx_tap_mode));

    /*read current tx NRZ mode control info */
    PHYMOD_IF_ERR_RETURN
        (blackhawk7_l2p2_tx_nrz_mode_get(&phy_copy.access, &tx_tap_nrz_mode));

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
            (blackhawk7_l2p2_read_tx_afe(&phy_copy.access, BLACKHAWK7_L2P2_TX_AFE_TAP0, &val));
            tx->pre = val;
        PHYMOD_IF_ERR_RETURN
            (blackhawk7_l2p2_read_tx_afe(&phy_copy.access, BLACKHAWK7_L2P2_TX_AFE_TAP1, &val));
            tx->main = val;
        PHYMOD_IF_ERR_RETURN
            (blackhawk7_l2p2_read_tx_afe(&phy_copy.access, BLACKHAWK7_L2P2_TX_AFE_TAP2, &val));
            tx->post = val;
            tx->pre2 = 0;
            tx->post2 = 0;
            tx->post3 = 0;
    } else {
        PHYMOD_IF_ERR_RETURN
            (blackhawk7_l2p2_read_tx_afe(&phy_copy.access, BLACKHAWK7_L2P2_TX_AFE_TAP0, &val));
            tx->pre2 = val;
        PHYMOD_IF_ERR_RETURN
            (blackhawk7_l2p2_read_tx_afe(&phy_copy.access, BLACKHAWK7_L2P2_TX_AFE_TAP1, &val));
            tx->pre = val;
        PHYMOD_IF_ERR_RETURN
            (blackhawk7_l2p2_read_tx_afe(&phy_copy.access, BLACKHAWK7_L2P2_TX_AFE_TAP2, &val));
            tx->main = val;
        PHYMOD_IF_ERR_RETURN
            (blackhawk7_l2p2_read_tx_afe(&phy_copy.access, BLACKHAWK7_L2P2_TX_AFE_TAP3, &val));
            tx->post = val;
        PHYMOD_IF_ERR_RETURN
            (blackhawk7_l2p2_read_tx_afe(&phy_copy.access, BLACKHAWK7_L2P2_TX_AFE_TAP4, &val));
            tx->post2 = val;
        PHYMOD_IF_ERR_RETURN
            (blackhawk7_l2p2_read_tx_afe(&phy_copy.access, BLACKHAWK7_L2P2_TX_AFE_TAP5, &val));
            tx->post3 = val;
    }

    return PHYMOD_E_NONE;

}

int tscbh_fe_gen2_phy_media_type_tx_get(const phymod_phy_access_t* phy,
                                        phymod_media_typed_t media, phymod_tx_t* tx)
{
    return PHYMOD_E_NONE;
}

int tscbh_fe_gen2_phy_tx_lane_control_set(const phymod_phy_access_t* phy,
                                          phymod_phy_tx_lane_control_t tx_control)
{
    phymod_firmware_lane_config_t fw_lane_config;
    phymod_phy_access_t phy_copy;

    PHYMOD_IF_ERR_RETURN (tscbh_fe_gen2_phy_firmware_lane_config_get(phy, &fw_lane_config));
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
                PHYMOD_IF_ERR_RETURN(blackhawk7_l2p2_electrical_idle_set(&phy_copy.access, 1));
            } else {
                return PHYMOD_E_PARAM;
            }
            break;
        case phymodTxElectricalIdleDisable:
            PHYMOD_IF_ERR_RETURN(blackhawk7_l2p2_electrical_idle_set(&phy_copy.access, 0));
            break;
        case phymodTxSquelchOn:
            PHYMOD_IF_ERR_RETURN(blackhawk7_l2p2_tx_disable(&phy_copy.access, 1));
            break;
        case phymodTxSquelchOff:
            PHYMOD_IF_ERR_RETURN(blackhawk7_l2p2_tx_disable(&phy_copy.access, 0));
            break;
        default:
            return PHYMOD_E_PARAM;
    }

    return PHYMOD_E_NONE;
}

int tscbh_fe_gen2_phy_tx_lane_control_get(const phymod_phy_access_t* phy,
                                          phymod_phy_tx_lane_control_t *tx_control)
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

    PHYMOD_IF_ERR_RETURN(blackhawk7_l2p2_tx_disable_get(&pm_phy_copy.access, &tx_disable));

    /* next check if PMD loopback is on */
    if (tx_disable) {
        PHYMOD_IF_ERR_RETURN(blackhawk7_l2p2_dig_lpbk_get(&pm_phy_copy.access, &lb_enable));
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
int tscbh_fe_gen2_phy_rx_lane_control_set(const phymod_phy_access_t* phy,
                                          phymod_phy_rx_lane_control_t rx_control)
{
    phymod_phy_access_t pm_phy_copy;

    PHYMOD_MEMCPY(&pm_phy_copy, phy, sizeof(pm_phy_copy));

    switch (rx_control) {
        case phymodRxReset:
            PHYMOD_IF_ERR_RETURN(tbhmod_rx_lane_control(&pm_phy_copy.access, 0));
            break;
        case phymodRxSquelchOn:
            PHYMOD_IF_ERR_RETURN(blackhawk7_l2p2_pmd_force_signal_detect(&pm_phy_copy.access, 1, 0));
            break;
        case phymodRxSquelchOff:
            PHYMOD_IF_ERR_RETURN(blackhawk7_l2p2_pmd_force_signal_detect(&pm_phy_copy.access, 0, 0));
            break;
        default:
            return PHYMOD_E_PARAM;
    }

    return PHYMOD_E_NONE;
}

int tscbh_fe_gen2_phy_rx_lane_control_get(const phymod_phy_access_t* phy,
                                          phymod_phy_rx_lane_control_t* rx_control)
{
    int reset, rx_squelch_enable;
    uint32_t lb_enable;
    uint8_t force_en, force_val;
    phymod_phy_access_t pm_phy_copy;

    PHYMOD_MEMCPY(&pm_phy_copy, phy, sizeof(pm_phy_copy));

    /* first get the force enabled bit and forced value */
    PHYMOD_IF_ERR_RETURN(blackhawk7_l2p2_pmd_force_signal_detect_get(&pm_phy_copy.access, &force_en, &force_val));

    if (force_en & (!force_val)) {
        rx_squelch_enable = 1;
    } else {
        rx_squelch_enable = 0;
    }

    /* next check if PMD loopback is on */
    if (rx_squelch_enable) {
        PHYMOD_IF_ERR_RETURN(blackhawk7_l2p2_dig_lpbk_get(&pm_phy_copy.access, &lb_enable));
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

/*
 * Enables/Disables 100G 4 lane FEC override
 *
 * value: 0x1    Enable for NOFEC 100G AN
 * register 0xc05e sc_x4_control_SW_spare1 lane 0 copy  is used
 * for this purpose
 */
int tscbh_fe_gen2_phy_fec_override_set(const phymod_phy_access_t* phy, uint32_t enable)
{
    phymod_phy_access_t phy_copy;
    uint32_t* tscbh_fe_gen2_spd_id_entry;
    uint32_t* tscbh_fe_gen2_spd_id_entry_100g_4lane_no_fec;

    tscbh_fe_gen2_spd_id_entry = tscbh_fe_gen2_spd_id_entry_get();
    tscbh_fe_gen2_spd_id_entry_100g_4lane_no_fec = tscbh_fe_gen2_spd_id_entry_100g_4lane_no_fec_get();

    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));
    phy_copy.access.lane_mask = 1;

    if ((enable ==  0) || (enable == 1)) {
         PHYMOD_IF_ERR_RETURN(tbhmod_fe_gen2_fec_override_set(&phy_copy.access, enable));
    } else {
     PHYMOD_DEBUG_ERROR(("ERROR :: Supported input values: 1 to set FEC override, 0 to disable FEC override\n"));
    }

    /* first set the lane mask to be 0x1 */
    phy_copy.access.lane_mask = 1 << 0;
        /* then load speed id entry for the 100G 4 lane speed */
    if (enable) {
        PHYMOD_IF_ERR_RETURN
            (phymod_mem_write(&phy_copy.access, phymodMemSpeedIdTable,
                              SPEED_ID_INDEX_100G_4_LANE_CL73_KR4,
                              tscbh_fe_gen2_spd_id_entry_100g_4lane_no_fec));
        PHYMOD_IF_ERR_RETURN
            (phymod_mem_write(&phy_copy.access, phymodMemSpeedIdTable,
                              SPEED_ID_INDEX_100G_4_LANE_CL73_CR4,
                              tscbh_fe_gen2_spd_id_entry_100g_4lane_no_fec));
    } else {
        PHYMOD_IF_ERR_RETURN
            (phymod_mem_write(&phy_copy.access, phymodMemSpeedIdTable,
                              SPEED_ID_INDEX_100G_4_LANE_CL73_KR4,
                              (tscbh_fe_gen2_spd_id_entry + SPEED_ID_INDEX_100G_4_LANE_CL73_KR4 * TSCBH_FE_GEN2_SPEED_ID_ENTRY_SIZE)));
        PHYMOD_IF_ERR_RETURN
            (phymod_mem_write(&phy_copy.access, phymodMemSpeedIdTable,
                              SPEED_ID_INDEX_100G_4_LANE_CL73_CR4,
                              (tscbh_fe_gen2_spd_id_entry + SPEED_ID_INDEX_100G_4_LANE_CL73_CR4 * TSCBH_FE_GEN2_SPEED_ID_ENTRY_SIZE)));
    }

    return PHYMOD_E_NONE;
}

int tscbh_fe_gen2_phy_fec_override_get(const phymod_phy_access_t* phy, uint32_t* enable)
{
    phymod_phy_access_t phy_copy;

    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));
    phy_copy.access.lane_mask = 1;
    PHYMOD_IF_ERR_RETURN(tbhmod_fe_gen2_fec_override_get(&phy_copy.access, enable));

    return PHYMOD_E_NONE;
}

int tscbh_fe_gen2_phy_cl72_set(const phymod_phy_access_t* phy, uint32_t cl72_en)
{
    struct blackhawk7_l2p2_uc_lane_config_st serdes_firmware_config;
    phymod_firmware_lane_config_t firmware_lane_config;
    int start_lane, num_lane, i, precoder_en;
    uint32_t lane_reset, pcs_lane_enable;
    phymod_phy_access_t pm_phy_copy;

    PHYMOD_MEMCPY(&pm_phy_copy, phy, sizeof(pm_phy_copy));
    PHYMOD_IF_ERR_RETURN(blackhawk7_l2p2_get_uc_lane_cfg(&pm_phy_copy.access, &serdes_firmware_config));

    if ((serdes_firmware_config.field.dfe_on == 0) && cl72_en) {
      PHYMOD_DEBUG_ERROR(("ERROR :: DFE is off : Can not start CL72/CL93 with no DFE\n"));
      return PHYMOD_E_CONFIG;
    }

    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(&phy->access, &start_lane, &num_lane));

    /*first check if lane is in reset */
    PHYMOD_IF_ERR_RETURN
        (blackhawk7_l2p2_lane_soft_reset_get(&pm_phy_copy.access, &lane_reset));

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
            (tbhmod_fe_gen2_pcs_reset_sw_war(&phy->access));
    }

    /* if lane is not in reset, then reset the lane first */
    if (!lane_reset) {
        PHYMOD_MEMCPY(&pm_phy_copy, phy, sizeof(pm_phy_copy));
        PHYMOD_IF_ERR_RETURN
            (blackhawk7_l2p2_lane_soft_reset(&pm_phy_copy.access, 1));
    }

    /* next need to clear both force ER and NR config on the firmware lane config side
    if link training enable is set */
    if (cl72_en) {
        PHYMOD_IF_ERR_RETURN
            (tscbh_fe_gen2_phy_firmware_lane_config_get(phy, &firmware_lane_config));

        firmware_lane_config.ForceNormalReach = 0;
        firmware_lane_config.ForceExtenedReach = 0;

         PHYMOD_IF_ERR_RETURN
            (_tscbh_fe_gen2_phy_firmware_lane_config_set(phy, firmware_lane_config));
    } else {
        /* disable Tx pre-coding and set Rx in NR mode */
        for (i = 0; i < num_lane; i++) {
            pm_phy_copy.access.lane_mask = 1 << (start_lane + i);
            precoder_en = 0;
            PHYMOD_IF_ERR_RETURN
                (tscbh_fe_gen2_phy_tx_pam4_precoder_enable_get(&pm_phy_copy, &precoder_en));
            if (precoder_en) {
                PHYMOD_IF_ERR_RETURN
                    (tscbh_fe_gen2_phy_tx_pam4_precoder_enable_set(&pm_phy_copy, 0));
            }
            PHYMOD_IF_ERR_RETURN
                (tscbh_fe_gen2_phy_firmware_lane_config_get(&pm_phy_copy, &firmware_lane_config));
            if (firmware_lane_config.ForcePAM4Mode) {
                firmware_lane_config.ForceNormalReach = 1;
                firmware_lane_config.ForceExtenedReach = 0;
                PHYMOD_IF_ERR_RETURN
                    (_tscbh_fe_gen2_phy_firmware_lane_config_set(&pm_phy_copy, firmware_lane_config));
            }
        }
    }
    for (i = 0; i < num_lane; i++) {
        pm_phy_copy.access.lane_mask = 1 << (start_lane + i);
        if (!PHYMOD_LANEPBMP_MEMBER(phy->access.lane_mask, start_lane + i)) {
            continue;
        }
        PHYMOD_IF_ERR_RETURN
            (blackhawk7_l2p2_clause72_control(&pm_phy_copy.access, cl72_en));
    }

    /* release the ln dp reset */
    if (!lane_reset) {
        PHYMOD_MEMCPY(&pm_phy_copy, phy, sizeof(pm_phy_copy));
        PHYMOD_IF_ERR_RETURN
            (blackhawk7_l2p2_lane_soft_reset(&pm_phy_copy.access, 0));
    }

    /* re-enable pcs lane if pcs lane not in rset */
    if (pcs_lane_enable) {
        pm_phy_copy.access.lane_mask = 1 << start_lane;
        PHYMOD_IF_ERR_RETURN
            (tbhmod_enable_set(&pm_phy_copy.access));
    }

    return PHYMOD_E_NONE;
}

int tscbh_fe_gen2_phy_cl72_get(const phymod_phy_access_t* phy, uint32_t* cl72_en)
{
    phymod_phy_access_t pm_phy_copy;

    PHYMOD_MEMCPY(&pm_phy_copy, phy, sizeof(pm_phy_copy));

    PHYMOD_IF_ERR_RETURN
        (blackhawk7_l2p2_clause72_control_get(&pm_phy_copy.access, cl72_en));

    return PHYMOD_E_NONE;
}

int tscbh_fe_gen2_phy_cl72_status_get(const phymod_phy_access_t* phy, phymod_cl72_status_t* status)
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
        PHYMOD_IF_ERR_RETURN(blackhawk7_l2p2_pmd_cl72_receiver_status(&phy_copy.access, &tmp_status));
        if (tmp_status == 0) {
            status->locked = 0;
            return PHYMOD_E_NONE;
        }
    }

    return PHYMOD_E_NONE;
}

int tscbh_fe_gen2_phy_autoneg_set(const phymod_phy_access_t* phy, const phymod_autoneg_control_t* an)
{
    int num_lane_adv_encoded, mapped_speed_id;
    int start_lane, num_lane;
    int i, do_lane_config_set;
    uint32_t pll_1_div, vco_rate, refclk_in_hz, pll_index = 0;
    phymod_firmware_lane_config_t firmware_lane_config;
    tbhmod_an_control_t an_control;
    phymod_phy_access_t phy_copy;
    tbhmod_refclk_t ref_clk;
    tbhmod_fe_gen2_spd_intfc_type_t spd_intf = 0;

    PHYMOD_MEMSET(&firmware_lane_config, 0x0, sizeof(firmware_lane_config));

    PHYMOD_MEMSET(&an_control, 0x0, sizeof(an_control));
    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(&phy->access, &start_lane, &num_lane));

    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));

    /* first set the port an mode enable bit */
    PHYMOD_IF_ERR_RETURN
        (tbhmod_port_an_mode_enable_set(&phy_copy.access, an->enable));

    phy_copy.access.lane_mask = 0x1 << start_lane;

    if (an->enable) {
        PHYMOD_IF_ERR_RETURN(tbhmod_disable_set(&phy_copy.access));
        /* adjust FEC arch for RS544/RS272 */
        if (PHYMOD_AN_F_FEC_RS272_CLR_GET(an)) {
            if (start_lane < 4) {
                phy_copy.access.lane_mask = 0x1;
                PHYMOD_IF_ERR_RETURN(tbhmod_rsfec_cw_type(&phy_copy.access, 0, 0));
            }
            if ((start_lane >= 4) || (num_lane == 8)) {
                phy_copy.access.lane_mask = 0x1 << 4;
                PHYMOD_IF_ERR_RETURN(tbhmod_rsfec_cw_type(&phy_copy.access, 0, 0));
            }
        } else if (PHYMOD_AN_F_FEC_RS272_REQ_GET(an)) {
            if (start_lane < 4) {
                phy_copy.access.lane_mask = 0x1;
                PHYMOD_IF_ERR_RETURN(tbhmod_rsfec_cw_type(&phy_copy.access, 1, 0));
            }
            if ((start_lane >= 4) || (num_lane == 8)) {
                phy_copy.access.lane_mask = 0x1 << 4;
                PHYMOD_IF_ERR_RETURN(tbhmod_rsfec_cw_type(&phy_copy.access, 1, 0));
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

        /* get PLL index */
        phy_copy.access.lane_mask = 0x1 << start_lane;
        PHYMOD_IF_ERR_RETURN
            (blackhawk7_l2p2_lane_tx_pll_selection_get(&phy_copy.access, &pll_index));
        /* next read current pll divider*/
        phy_copy.access.pll_idx = pll_index;
        PHYMOD_IF_ERR_RETURN
            (blackhawk7_l2p2_INTERNAL_read_pll_div(&phy_copy.access, &pll_1_div));

        PHYMOD_IF_ERR_RETURN
            (blackhawk7_l2p2_get_vco_from_refclk_div(&phy_copy.access, refclk_in_hz, pll_1_div, &vco_rate, 0));

        if (vco_rate == 20625000) {
            /* load 20G VCO spd_id */
            spd_intf = TBHMOD_FE_GEN2_SPD_CL73_IEEE_20GVCO;
        } else if (vco_rate == 25781250) {
            /* load 25G VCO spd_id */
            spd_intf = TBHMOD_FE_GEN2_SPD_CL73_IEEE_25GVCO;
        } else if (vco_rate == 26562500) {
            /* load 26G VCO spd_id */
            spd_intf = TBHMOD_FE_GEN2_SPD_CL73_IEEE_26GVCO;
        } else {
            return PHYMOD_E_PARAM;
        }

        PHYMOD_IF_ERR_RETURN
            (tbhmod_fe_gen2_get_mapped_speed(spd_intf, &mapped_speed_id));

        phy_copy.access.lane_mask = 0x1 << start_lane;

        /* Load 1G speed ID */
        PHYMOD_IF_ERR_RETURN
            (tbhmod_set_sc_speed(&phy_copy.access, mapped_speed_id, 0));
    } else {
        /* Disable Tx PAM4 pre-coding. It might be enabled by AN link training. */
        for (i = 0; i < num_lane; i++) {
            if (!PHYMOD_LANEPBMP_MEMBER(phy->access.lane_mask, start_lane + i)) {
                continue;
            }
            phy_copy.access.lane_mask = 0x1 << (i + start_lane);
            PHYMOD_IF_ERR_RETURN
                (blackhawk7_l2p2_tx_pam4_precoder_enable_set(&phy_copy.access, 0));
        }
    }

    do_lane_config_set = 0;
    if (an->enable) {
        /* make sure the firmware config is set to an enabled */
        PHYMOD_IF_ERR_RETURN
            (tscbh_fe_gen2_phy_firmware_lane_config_get(&phy_copy, &firmware_lane_config));
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
    } else {
        /* make sure the firmware config is set to an disabled */
        PHYMOD_IF_ERR_RETURN
            (tscbh_fe_gen2_phy_firmware_lane_config_get(&phy_copy, &firmware_lane_config));

        firmware_lane_config.AnEnabled = 0;
        firmware_lane_config.LaneConfigFromPCS = 0;
        do_lane_config_set = 1;
    }

    if (do_lane_config_set) {
        for (i = 0; i < num_lane; i++) {
            if (!PHYMOD_LANEPBMP_MEMBER(phy->access.lane_mask, start_lane + i)) {
                continue;
            }
            phy_copy.access.lane_mask = 0x1 << (i + start_lane);
            PHYMOD_IF_ERR_RETURN
                (blackhawk7_l2p2_lane_soft_reset(&phy_copy.access, 1));
        }
        PHYMOD_USLEEP(1000);
        for (i = 0; i < num_lane; i++) {
            if (!PHYMOD_LANEPBMP_MEMBER(phy->access.lane_mask, start_lane + i)) {
                continue;
            }
            phy_copy.access.lane_mask = 0x1 << (i + start_lane);
            PHYMOD_IF_ERR_RETURN
                (_tscbh_fe_gen2_phy_firmware_lane_config_set(&phy_copy, firmware_lane_config));
        }
        for (i = 0; i < num_lane; i++) {
            if (!PHYMOD_LANEPBMP_MEMBER(phy->access.lane_mask, start_lane + i)) {
                continue;
            }
            phy_copy.access.lane_mask = 0x1 << (i + start_lane);
            PHYMOD_IF_ERR_RETURN
                (blackhawk7_l2p2_lane_soft_reset(&phy_copy.access, 0));
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

int tscbh_fe_gen2_phy_autoneg_get(const phymod_phy_access_t* phy, phymod_autoneg_control_t* an, uint32_t* an_done)
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

int tscbh_fe_gen2_phy_autoneg_status_get(const phymod_phy_access_t* phy, phymod_autoneg_status_t* status)
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
       (tscbh_fe_gen2_phy_speed_config_get(phy, &speed_config));

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
 * 9.  Set core_from_pcs_config
 * 10. AFE/PLL configuration
 * 11. Program AN default timer
 * 12. Release core DP soft reset
 */
STATIC
int _tscbh_fe_gen2_core_init_pass1(const phymod_core_access_t* core, const phymod_core_init_config_t* init_config, const phymod_core_status_t* core_status)
{
    int rv, lane, pmd_index = 0;
    phymod_phy_access_t phy_access;
    phymod_core_access_t  core_copy;
    uint32_t uc_enable = 1, temp_val = 0;
    ucode_info_t ucode;
    /* need to fix this later */
    unsigned int blackhawk7_ucode_len;
    unsigned char *blackhawk7_ucode;
    uint32_t am_table_load_size, um_table_load_size, speed_id_load_size, i;
    uint32_t *tscbh_fe_gen2_am_table_entry;
    uint32_t *tscbh_fe_gen2_um_table_entry;
    uint32_t *tscbh_fe_gen2_speed_priority_mapping_table;
    uint32_t* tscbh_fe_gen2_spd_id_entry;

    tscbh_fe_gen2_am_table_entry = tscbh_fe_gen2_am_table_entry_get();
    tscbh_fe_gen2_um_table_entry = tscbh_fe_gen2_um_table_entry_get();
    tscbh_fe_gen2_speed_priority_mapping_table = tscbh_fe_gen2_speed_priority_mapping_table_get();
    tscbh_fe_gen2_spd_id_entry = tscbh_fe_gen2_spd_id_entry_get();

    TSCBH_CORE_TO_PHY_ACCESS(&phy_access, core);
    PHYMOD_MEMCPY(&core_copy, core, sizeof(core_copy));
    core_copy.access.lane_mask = 0x1;

    /* 1. De-assert PMD core power and core data path reset */
    PHYMOD_IF_ERR_RETURN
        (tbhmod_fe_gen2_pmd_reset_seq(&core_copy.access, 0));

    for (pmd_index = 0; pmd_index < BLACKHAWK7_L2P2_NOF_INSTANCE; pmd_index++) {
        /* with chip default lane mapping,
           using logical lane 0,2,4,6 for accessing PMD instance 0,1,2,3 */
        core_copy.access.lane_mask = 0x1 << (pmd_index * 2);
        core_copy.access.pll_idx = 1;
        PHYMOD_IF_ERR_RETURN
            (blackhawk7_l2p2_core_dp_reset(&core_copy.access, 1));
        core_copy.access.pll_idx = 0;
        PHYMOD_IF_ERR_RETURN
            (blackhawk7_l2p2_core_dp_reset(&core_copy.access, 1));
    }

    /* De-assert PMD lane reset */
    for (lane = 0; lane < TSCBH_NOF_LANES_IN_CORE; lane++) {
        phy_access.access.lane_mask = 1 << lane;
        PHYMOD_IF_ERR_RETURN
          (tbhmod_pmd_x4_reset(&phy_access.access));
    }
    /* check uc_enable for all 4 PMD instances */
    for (pmd_index = 0; pmd_index < BLACKHAWK7_L2P2_NOF_INSTANCE; pmd_index++) {
        phy_access.access.lane_mask = 0x1 << (pmd_index * 2);
        PHYMOD_IF_ERR_RETURN
            (blackhawk7_l2p2_uc_active_get(&phy_access.access, &temp_val));
        uc_enable &= temp_val;
    }
    if (uc_enable) return PHYMOD_E_NONE;

    /* 2. Set the heart beat, default is 156.25M */
    if (init_config->interface.ref_clock != phymodRefClk156Mhz) {
        for (pmd_index = 0; pmd_index < BLACKHAWK7_L2P2_NOF_INSTANCE; pmd_index++) {
            core_copy.access.lane_mask = 0x1 << (pmd_index * 2);
            PHYMOD_IF_ERR_RETURN
                (blackhawk7_l2p2_refclk_set(&core_copy.access, init_config->interface.ref_clock));
        }
        PHYMOD_IF_ERR_RETURN
            (tbhmod_refclk_set(&phy_access.access, TBHMOD_REF_CLK_312P5MHZ));
    }

    /* set the micro stack size */
    phy_access.access.lane_mask = 0x1;
    PHYMOD_MEMSET(&ucode, 0, sizeof(ucode));
    ucode.stack_size = BLACKHAWK7_V1L2P2_UCODE_STACK_SIZE;
    ucode.ucode_size = BLACKHAWK7_V1L2P2_UCODE_IMAGE_SIZE;
    ucode.crc_value  = BLACKHAWK7_V1L2P2_UCODE_IMAGE_CRC;
    blackhawk7_ucode = blackhawk7_l2p2_ucode_get();
    blackhawk7_ucode_len = ucode.ucode_size;

    /* load ucode for all 4 PMD instances */
    for (pmd_index = 0; pmd_index < BLACKHAWK7_L2P2_NOF_INSTANCE; pmd_index++) {
        core_copy.access.lane_mask = 0x1 << (pmd_index * 2);
        PHYMOD_IF_ERR_RETURN
        (blackhawk7_l2p2_uc_reset_with_info(&core_copy.access , 1, ucode));

        /* 3. Micro code load and verify */
        rv = _tscbh_fe_gen2_core_firmware_load(&core_copy, init_config);
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
                rv = blackhawk7_l2p2_ucode_load_verify(&core_copy.access, (uint8_t *) blackhawk7_ucode, blackhawk7_ucode_len);
                if (rv != PHYMOD_E_NONE) {
                    PHYMOD_DEBUG_ERROR(("devad 0x%x lane 0x%x: UC load-verify failed\n", core->access.addr, core->access.lane_mask));
                    PHYMOD_IF_ERR_RETURN(rv);
                }
            }
        }
    }

    PHYMOD_MEMCPY(&core_copy, core, sizeof(core_copy));
    core_copy.access.lane_mask = 0x1;
    am_table_load_size = TSCBH_FE_GEN2_AM_TABLE_SIZE > TSCBH_HW_AM_TABLE_SIZE ? TSCBH_HW_AM_TABLE_SIZE : TSCBH_FE_GEN2_AM_TABLE_SIZE;
    um_table_load_size = TSCBH_FE_GEN2_UM_TABLE_SIZE > TSCBH_HW_UM_TABLE_SIZE ? TSCBH_HW_UM_TABLE_SIZE : TSCBH_FE_GEN2_UM_TABLE_SIZE;
    speed_id_load_size = TSCBH_FE_GEN2_SPEED_ID_TABLE_SIZE > TSCBH_FE_GEN2_HW_SPEED_ID_TABLE_SIZE ? TSCBH_FE_GEN2_HW_SPEED_ID_TABLE_SIZE : TSCBH_FE_GEN2_SPEED_ID_TABLE_SIZE;
    for (i = 0; i < am_table_load_size; i++) {
        PHYMOD_IF_ERR_RETURN
            (phymod_mem_write(&core_copy.access, phymodMemAMTable, i,
                              (tscbh_fe_gen2_am_table_entry + i * TSCBH_FE_GEN2_AM_ENTRY_SIZE)));
    }

    for (i = 0; i < um_table_load_size; i++) {
        PHYMOD_IF_ERR_RETURN
            (phymod_mem_write(&core_copy.access, phymodMemUMTable, i,
                              (tscbh_fe_gen2_um_table_entry + i * TSCBH_FE_GEN2_UM_ENTRY_SIZE)));
    }

    /* load speed id table here: there is only one speed table in J2X */
    for (i = 0; i < speed_id_load_size; i++) {
        PHYMOD_IF_ERR_RETURN
            (phymod_mem_write(&core_copy.access, phymodMemSpeedIdTable, i,
                              (tscbh_fe_gen2_spd_id_entry + i * TSCBH_FE_GEN2_SPEED_ID_ENTRY_SIZE)));
    }

    /*need to update speed_priority_mapping_table with correct speed id */
    PHYMOD_IF_ERR_RETURN
        (phymod_mem_write(&core_copy.access, phymodMemSpeedPriorityMapTable, 0,
                          tscbh_fe_gen2_speed_priority_mapping_table));

    return PHYMOD_E_NONE;
}

STATIC
int _tscbh_fe_gen2_core_init_pass2(const phymod_core_access_t* core, const phymod_core_init_config_t* init_config, const phymod_core_status_t* core_status)
{
    phymod_phy_access_t phy_access, phy_access_copy;
    phymod_core_access_t  core_copy;
    enum blackhawk7_l2p2_pll_refclk_enum refclk;
    phymod_polarity_t tmp_pol;
    int lane, pmd_index = 0, pmd_lane = 0;
    uint32_t pll_index = 0;

    TSCBH_CORE_TO_PHY_ACCESS(&phy_access, core);
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
        (tscbh_fe_gen2_core_lane_map_set(&core_copy, &init_config->lane_map));

    if (init_config->interface.ref_clock == phymodRefClk156Mhz) {
        refclk = BLACKHAWK7_L2P2_PLL_REFCLK_156P25MHZ;
    } else if (init_config->interface.ref_clock == phymodRefClk312Mhz) {
        refclk = BLACKHAWK7_L2P2_PLL_REFCLK_312P5MHZ;
    } else {
        PHYMOD_DEBUG_ERROR(("Unsupported reference clock.\n"));
        return PHYMOD_E_UNAVAIL;
    }

    if (init_config->firmware_load_method != phymodFirmwareLoadMethodNone) {
        ucode_info_t ucode;

        PHYMOD_MEMSET(&ucode, 0, sizeof(ucode));
        ucode.stack_size = BLACKHAWK7_V1L2P2_UCODE_STACK_SIZE;
        ucode.ucode_size = BLACKHAWK7_V1L2P2_UCODE_IMAGE_SIZE;
        ucode.crc_value  = BLACKHAWK7_V1L2P2_UCODE_IMAGE_CRC;

        /* 5. Release uc reset */
        for (pmd_index = 0; pmd_index < BLACKHAWK7_L2P2_NOF_INSTANCE; pmd_index++) {
            for (lane = 0; lane < TSCBH_NOF_LANES_IN_CORE; lane++) {
                /* find logical lanes that are mapped to physical lane 0,2,4,6 */
                if ((pmd_index * 2) == (init_config->lane_map.lane_map_tx[lane])) {
                    core_copy.access.lane_mask = 0x1 << lane;
                    PHYMOD_IF_ERR_RETURN
                    (blackhawk7_l2p2_uc_reset_with_info(&core_copy.access , 0, ucode));
                    break;
                }
            }
        }

        /* 6. Wait for uc_active = 1 */
        for (pmd_index = 0; pmd_index < BLACKHAWK7_L2P2_NOF_INSTANCE; pmd_index++) {
            for (lane = 0; lane < TSCBH_NOF_LANES_IN_CORE; lane++) {
                /* find logical lanes that are mapped to physical lane 0,2,4,6 */
                if ((pmd_index * 2) == (init_config->lane_map.lane_map_tx[lane])) {
                    core_copy.access.lane_mask = 0x1 << lane;
                    PHYMOD_IF_ERR_RETURN
                        (blackhawk7_l2p2_wait_uc_active(&core_copy.access));
                    break;
                }
            }
        }

        for (lane = 0; lane < TSCBH_NOF_LANES_IN_CORE; lane++) {
            phy_access_copy.access.lane_mask = 1 << lane;
            PHYMOD_IF_ERR_RETURN
                (blackhawk7_l2p2_pmd_ln_h_rstb_pkill_override(&phy_access_copy.access, 0x1));
        }

        for (pmd_index = 0; pmd_index < BLACKHAWK7_L2P2_NOF_INSTANCE; pmd_index++) {
            for (lane = 0; lane < TSCBH_NOF_LANES_IN_CORE; lane++) {
                /* find logical lanes that are mapped to physical lane 0,2,4,6 */
                if ((pmd_index * 2) == (init_config->lane_map.lane_map_tx[lane])) {
                    core_copy.access.lane_mask = 0x1 << lane;
                    /* 7. Initialize software information table for the macro */
                    PHYMOD_IF_ERR_RETURN
                        (blackhawk7_l2p2_init_blackhawk7_l2p2_info(&core_copy.access));

                    /* check the FW crc checksum error */
                    if (!PHYMOD_CORE_INIT_F_BYPASS_CRC_CHECK_GET(init_config)) {
                        PHYMOD_IF_ERR_RETURN
                            (blackhawk7_l2p2_ucode_crc_verify(&core_copy.access, ucode.ucode_size, ucode.crc_value));
                    }
                    break;
                }
            }
        }

        /* release pmd lane hard reset */
        for (lane = 0; lane < TSCBH_NOF_LANES_IN_CORE; lane++) {
            phy_access_copy.access.lane_mask = 1 << lane;
            PHYMOD_IF_ERR_RETURN
                (blackhawk7_l2p2_pmd_ln_h_rstb_pkill_override(&phy_access_copy.access, 0x0));
        }
    }

    /* 8. RX/TX polarity configuration */
    for (lane = 0; lane < TSCBH_NOF_LANES_IN_CORE; lane++) {
        phy_access_copy.access.lane_mask = 1 << lane;
        tmp_pol.tx_polarity = (init_config->polarity_map.tx_polarity) >> lane & 0x1;
        tmp_pol.rx_polarity = (init_config->polarity_map.rx_polarity) >> lane & 0x1;
    PHYMOD_IF_ERR_RETURN
        (blackhawk7_l2p2_tx_rx_polarity_set(&phy_access_copy.access, tmp_pol.tx_polarity, tmp_pol.rx_polarity));
        /* clear the tmp vairiable */
        PHYMOD_MEMSET(&tmp_pol, 0x0, sizeof(tmp_pol));
    }

    for (pmd_index = 0; pmd_index < BLACKHAWK7_L2P2_NOF_INSTANCE; pmd_index++) {
        for (lane = 0; lane < TSCBH_NOF_LANES_IN_CORE; lane++) {
            /* find logical lanes that are mapped to physical lane 0,2,4,6 */
            if ((pmd_index * 2) == (init_config->lane_map.lane_map_tx[lane])) {
                core_copy.access.lane_mask = 0x1 << lane;
                /* 9. Enable PCS clock block: not needed */

                /* 10a. PLL_DIV config for both PLL0 and PLL1 */
                if (init_config->pll0_div_init_value != (uint32_t)phymod_TSCBH_PLL_DIVNONE) {
                    core_copy.access.pll_idx = 0;
                    if (init_config->pll0_div_init_value == TBHMOD_PLL_MODE_DIV_132 ||
                        init_config->pll0_div_init_value == TBHMOD_PLL_MODE_DIV_66) {
                        PHYMOD_IF_ERR_RETURN
                            (blackhawk7_l2p2_configure_20P625G_pll_div(&core_copy.access,
                                                                       init_config->pll0_div_init_value));
                    } else {
                        PHYMOD_IF_ERR_RETURN
                            (blackhawk7_l2p2_configure_pll_refclk_div(&core_copy.access,
                                                                      refclk,
                                                                      init_config->pll0_div_init_value));
                    }
                }

                if (init_config->pll1_div_init_value != (uint32_t)phymod_TSCBH_PLL_DIVNONE) {
                    core_copy.access.pll_idx = 1;
                    if (init_config->pll1_div_init_value == TBHMOD_PLL_MODE_DIV_132 ||
                        init_config->pll1_div_init_value == TBHMOD_PLL_MODE_DIV_66) {
                        PHYMOD_IF_ERR_RETURN
                            (blackhawk7_l2p2_configure_20P625G_pll_div(&core_copy.access,
                                                                       init_config->pll1_div_init_value));
                    } else {
                        PHYMOD_IF_ERR_RETURN
                            (blackhawk7_l2p2_configure_pll_refclk_div(&core_copy.access,
                                                                      refclk,
                                                                      init_config->pll1_div_init_value));
                    }
                }
                break;
            }
        }
    }
    /* set default pll selection if flexible PLL selection is not required */
    if (!PHYMOD_DEVICE_OP_MODE_FLEXIBLE_PLL_SELECT_GET(phy_access_copy.device_op_mode)) {
        /* 10b. choose the default pll index for each lane */
        for (lane = 0; lane < TSCBH_NOF_LANES_IN_CORE; lane++) {
            phy_access_copy.access.lane_mask = 1 << lane;
            /* mapped tx pmd lane */
            pmd_lane = init_config->lane_map.lane_map_tx[lane];
            /* a dedicated PLL is selected for Tx lane,
               Physical Tx0 selecs PLL0 and Tx1 selects PLL1 */
            pll_index = pmd_lane % 2;
            PHYMOD_IF_ERR_RETURN
                (blackhawk7_l2p2_lane_tx_pll_selection_set(&phy_access_copy.access, pll_index));
            PHYMOD_IF_ERR_RETURN
                (blackhawk7_l2p2_lane_rx_pll_selection_set(&phy_access_copy.access, pll_index));
        }
    }

    /* 11. Program AN default timer for both MMP0 and MMP1*/
    PHYMOD_MEMCPY(&core_copy, core, sizeof(core_copy));
    core_copy.access.lane_mask = 0x1;
    core_copy.access.pll_idx = 0;
    PHYMOD_IF_ERR_RETURN
        (tbhmod_set_an_timers(&core_copy.access, init_config->interface.ref_clock, NULL));

    core_copy.access.lane_mask = 0x10;
    core_copy.access.pll_idx = 0;
    PHYMOD_IF_ERR_RETURN
        (tbhmod_set_an_timers(&core_copy.access, init_config->interface.ref_clock, NULL));

    /* disable base-r FEC 1bit and 2-bit err interrupt */
    PHYMOD_IF_ERR_RETURN
        (tbhmod_ecc_error_intr_enable_set(&phy_access_copy.access, phymodIntrEccBaseRFEC, 0));

    /* set the PMD debug level to be 2 as default */
    for (lane = 0; lane < TSCBH_NOF_LANES_IN_CORE; lane++) {
        phy_access_copy.access.lane_mask = 1 << lane;
        PHYMOD_IF_ERR_RETURN
            (blackhawk7_l2p2_set_usr_ctrl_lane_event_log_level(&phy_access_copy.access, 2));
    }

    /* 12. Release core DP soft reset for both PLLs */
    for (pmd_index = 0; pmd_index < BLACKHAWK7_L2P2_NOF_INSTANCE; pmd_index++) {
        for (lane = 0; lane < TSCBH_NOF_LANES_IN_CORE; lane++) {
            /* find logical lanes that are mapped to physical lane 0,2,4,6 */
            if ((pmd_index * 2) == (init_config->lane_map.lane_map_tx[lane])) {
                core_copy.access.lane_mask = 0x1 << lane;
                core_copy.access.pll_idx = 0;
                PHYMOD_IF_ERR_RETURN
                    (blackhawk7_l2p2_core_dp_reset(&core_copy.access, 0));
                core_copy.access.pll_idx = 1;
                PHYMOD_IF_ERR_RETURN
                    (blackhawk7_l2p2_core_dp_reset(&core_copy.access, 0));
                break;
            }
        }
    }

    return PHYMOD_E_NONE;
}

int tscbh_fe_gen2_core_init(const phymod_core_access_t* core, const phymod_core_init_config_t* init_config, const phymod_core_status_t* core_status)
{
    if ( (!PHYMOD_CORE_INIT_F_EXECUTE_PASS1_GET(init_config) &&
          !PHYMOD_CORE_INIT_F_EXECUTE_PASS2_GET(init_config)) ||
        PHYMOD_CORE_INIT_F_EXECUTE_PASS1_GET(init_config)) {
        PHYMOD_IF_ERR_RETURN
            (_tscbh_fe_gen2_core_init_pass1(core, init_config, core_status));

        if (PHYMOD_CORE_INIT_F_EXECUTE_PASS1_GET(init_config)) {
            return PHYMOD_E_NONE;
        }
    }

    if ( (!PHYMOD_CORE_INIT_F_EXECUTE_PASS1_GET(init_config) &&
          !PHYMOD_CORE_INIT_F_EXECUTE_PASS2_GET(init_config)) ||
        PHYMOD_CORE_INIT_F_EXECUTE_PASS2_GET(init_config)) {
        PHYMOD_IF_ERR_RETURN
            (_tscbh_fe_gen2_core_init_pass2(core, init_config, core_status));
    }

    return PHYMOD_E_NONE;
}

int tscbh_fe_gen2_phy_pll_multiplier_get(const phymod_phy_access_t* phy, uint32_t* core_vco_pll_multiplier)
{
    phymod_phy_access_t pm_phy_copy;
    PHYMOD_MEMCPY(&pm_phy_copy, phy, sizeof(pm_phy_copy));

    PHYMOD_IF_ERR_RETURN(blackhawk7_l2p2_INTERNAL_read_pll_div(&pm_phy_copy.access,  core_vco_pll_multiplier));

    return PHYMOD_E_NONE;
}

int tscbh_fe_gen2_phy_init(const phymod_phy_access_t* phy, const phymod_phy_init_config_t* init_config)
{
    /*int pll_restart = 0;*/
    const phymod_access_t *pm_acc = &phy->access;
    phymod_phy_access_t pm_phy_copy;
    int start_lane, num_lane, i;
    phymod_firmware_lane_config_t firmware_lane_config;
    uint32_t pll_power_down = 0, pll_index = 0;
    enum blackhawk7_l2p2_txfir_tap_enable_enum enable_taps = BLACKHAWK7_L2P2_NRZ_6TAP;

    PHYMOD_MEMCPY(&pm_phy_copy, phy, sizeof(pm_phy_copy));
    PHYMOD_MEMSET(&firmware_lane_config, 0x0, sizeof(firmware_lane_config));

    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(pm_acc, &start_lane, &num_lane));

    /* per lane based reset release */
    if (PHYMOD_DEVICE_OP_MODE_PCS_BYPASS_GET(phy->device_op_mode)) {
        for (i = 0; i < num_lane; i++) {
            if (!PHYMOD_LANEPBMP_MEMBER(phy->access.lane_mask, start_lane + i)) {
                continue;
            }
            pm_phy_copy.access.lane_mask = 0x1 << (i + start_lane);
            PHYMOD_IF_ERR_RETURN
                (tbhmod_pmd_x4_reset(&pm_phy_copy.access));
        }
    } else {
        PHYMOD_IF_ERR_RETURN
            (tbhmod_pmd_x4_reset(&pm_phy_copy.access));
    }

    PHYMOD_MEMCPY(&pm_phy_copy, phy, sizeof(pm_phy_copy));
    /* Put PMD lane into soft reset */
    PHYMOD_IF_ERR_RETURN
        (blackhawk7_l2p2_lane_soft_reset(&pm_phy_copy.access, 1));

    for (i = 0; i < num_lane; i++) {
        if (!PHYMOD_LANEPBMP_MEMBER(phy->access.lane_mask, start_lane + i)) {
            continue;
        }
        pm_phy_copy.access.lane_mask = 0x1 << (i + start_lane);
        /*next check 3 tap mode or 6 tap mode */
        if (init_config->tx[i].tap_mode == phymodTxTapMode3Tap) {
            if (init_config->tx[i].sig_method == phymodSignallingMethodNRZ) {
                enable_taps= BLACKHAWK7_L2P2_NRZ_LP_3TAP;
            } else {
                enable_taps= BLACKHAWK7_L2P2_PAM4_LP_3TAP;
            }
            PHYMOD_IF_ERR_RETURN
                (blackhawk7_l2p2_apply_txfir_cfg(&pm_phy_copy.access,
                                                 enable_taps,
                                                 0,
                                                 init_config->tx[i].pre,
                                                 init_config->tx[i].main,
                                                 init_config->tx[i].post,
                                                 0,
                                                 0));
        } else {
            if (init_config->tx[i].sig_method == phymodSignallingMethodNRZ) {
                enable_taps= BLACKHAWK7_L2P2_NRZ_6TAP;
            } else {
                enable_taps= BLACKHAWK7_L2P2_PAM4_6TAP;
            }
            PHYMOD_IF_ERR_RETURN
                (blackhawk7_l2p2_apply_txfir_cfg(&pm_phy_copy.access,
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
             (_tscbh_fe_gen2_phy_firmware_lane_config_set(&pm_phy_copy, firmware_lane_config));
    }

    PHYMOD_MEMCPY(&pm_phy_copy, phy, sizeof(pm_phy_copy));
    if (PHYMOD_DEVICE_OP_MODE_PCS_BYPASS_GET(phy->device_op_mode)) {
        for (i = 0; i < num_lane; i++) {
             if (!PHYMOD_LANEPBMP_MEMBER(phy->access.lane_mask, start_lane + i)) {
                 continue;
             }
             pm_phy_copy.access.lane_mask = 0x1 << (i + start_lane);
             PHYMOD_IF_ERR_RETURN
                 (tbhmod_rx_lane_control(&pm_phy_copy.access, 1));
             PHYMOD_IF_ERR_RETURN
                 (tbhmod_tx_lane_control(&pm_phy_copy.access, 1, 0));         /* TX_LANE_CONTROL */
         }
    } else {
        PHYMOD_IF_ERR_RETURN
            (tbhmod_rx_lane_control(&pm_phy_copy.access, 1));
        PHYMOD_IF_ERR_RETURN
            (tbhmod_tx_lane_control(&pm_phy_copy.access, 1, 0));         /* TX_LANE_CONTROL */
    }

    PHYMOD_MEMCPY(&pm_phy_copy, phy, sizeof(pm_phy_copy));
    /* check PLL lock for each logical lane */
    for (i = 0; i < num_lane; i++) {
        if (!PHYMOD_LANEPBMP_MEMBER(phy->access.lane_mask, start_lane + i)) {
            continue;
        }
        pm_phy_copy.access.lane_mask = 0x1 << (i + start_lane);
        /* get PLL index */
        PHYMOD_IF_ERR_RETURN
            (blackhawk7_l2p2_lane_tx_pll_selection_get(&pm_phy_copy.access, &pll_index));
        /* make sure that power up PLL is locked */
        pm_phy_copy.access.pll_idx = pll_index;
        PHYMOD_IF_ERR_RETURN
            (blackhawk7_l2p2_pll_pwrdn_get(&pm_phy_copy.access, &pll_power_down));

        /* need to check pll lock if not power up */
        /* put the check here is to save on boot up time */
        if (!pll_power_down) {
            uint32_t cnt = 0, pll_lock = 0;
            while (cnt < 1000) {
                PHYMOD_IF_ERR_RETURN(blackhawk7_l2p2_pll_lock_get(&pm_phy_copy.access, &pll_lock));
                cnt = cnt + 1;
                if (pll_lock) {
                    break;
                } else {
                    if (cnt == 1000) {
                        PHYMOD_DEBUG_ERROR(("WARNING :: core 0x%x PLL is not locked within 10 milli second \n", pm_phy_copy.access.addr));
                        break;
                    }
                }
                PHYMOD_USLEEP(10);
            }
        }
    }

    /* 100G_2LANE_SW_WAR is not applicable for pm8x50fe gen2 */

    return PHYMOD_E_NONE;
}

STATIC int
_tscbh_fe_gen2_phy_lane_swap_validate(const phymod_phy_access_t* phy)
{
    int i;
    phymod_lane_map_t lane_map;
    phymod_core_access_t core;

    PHYMOD_MEMCPY(&core, phy, sizeof(phymod_core_access_t));

    PHYMOD_IF_ERR_RETURN
        (tscbh_fe_gen2_core_lane_map_get(&core, &lane_map));

    for (i = 0; i < TSCBH_NOF_LANES_IN_CORE; i++) {
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

int tscbh_fe_gen2_phy_loopback_set(const phymod_phy_access_t* phy, phymod_loopback_mode_t loopback, uint32_t enable)
{
    int i;
    int start_lane, num_lane;
    uint32_t port_enable;
    phymod_phy_access_t phy_copy;

    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));

    /* next figure out the lane num and start_lane based on the input */
    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(&phy->access, &start_lane, &num_lane));

    if (enable && (num_lane != TSCBH_NOF_LANES_IN_CORE)) {
        if (_tscbh_fe_gen2_phy_lane_swap_validate(phy)) {
            PHYMOD_DEBUG_WARN(("Warning: Digital and remote loopback will not operate as expected!\n"));
        }
    }

    switch (loopback) {
    case phymodLoopbackGlobal :
    case phymodLoopbackGlobalPMD :
        if (enable) {
            phy_copy.access.lane_mask = 1 << start_lane;
            PHYMOD_IF_ERR_RETURN
                (tbhmod_disable_set(&phy_copy.access));
            for (i = 0; i < num_lane; i++) {
                if (!PHYMOD_LANEPBMP_MEMBER(phy->access.lane_mask, start_lane + i)) {
                    continue;
                }
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
            PHYMOD_IF_ERR_RETURN(blackhawk7_l2p2_tx_disable(&phy_copy.access, enable));
            PHYMOD_IF_ERR_RETURN(blackhawk7_l2p2_dig_lpbk(&phy_copy.access, (uint8_t) enable));
        }
        if (!enable) {
            for (i = 0; i < num_lane; i++) {
                if (!PHYMOD_LANEPBMP_MEMBER(phy->access.lane_mask, start_lane + i)) {
                    continue;
                }
            }
        }
        break;
    case phymodLoopbackRemotePMD :
        /* get current port enable bit */
        PHYMOD_IF_ERR_RETURN
            (tbhmod_port_enable_get(&phy_copy.access, &port_enable));
        if (port_enable) {
            phy_copy.access.lane_mask = 1 << start_lane;
            PHYMOD_IF_ERR_RETURN
                (tbhmod_disable_set(&phy_copy.access));
        }

        for (i = 0; i < num_lane; i++) {
            if (!PHYMOD_LANEPBMP_MEMBER(phy->access.lane_mask, start_lane + i)) {
                continue;
            }
            phy_copy.access.lane_mask = 0x1 << (i + start_lane);
            PHYMOD_IF_ERR_RETURN(blackhawk7_l2p2_rmt_lpbk(&phy_copy.access, (uint8_t)enable));
        }
        /* Release the pcs lane reset if port is enabled */
        if (port_enable) {
            phy_copy.access.lane_mask = 1 << start_lane;
            PHYMOD_IF_ERR_RETURN
                (tbhmod_enable_set(&phy_copy.access));
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

int tscbh_fe_gen2_phy_loopback_get(const phymod_phy_access_t* phy, phymod_loopback_mode_t loopback, uint32_t* enable)
{
    phymod_phy_access_t phy_copy;

    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));

    switch (loopback) {
    case phymodLoopbackGlobal :
    case phymodLoopbackGlobalPMD :
        PHYMOD_IF_ERR_RETURN(blackhawk7_l2p2_dig_lpbk_get(&phy_copy.access, enable));
        break;
    case phymodLoopbackRemotePMD :
        PHYMOD_IF_ERR_RETURN(blackhawk7_l2p2_rmt_lpbk_get(&phy_copy.access, enable));
        break;
    case phymodLoopbackRemotePCS :
    default :
        return PHYMOD_E_UNAVAIL;
    }

    return PHYMOD_E_NONE;
}

/* this function gives the rx pmd lock STATUS */
int tscbh_fe_gen2_phy_rx_pmd_locked_get(const phymod_phy_access_t* phy, uint32_t* pmd_lock)
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
            (blackhawk7_l2p2_pmd_lock_status(&pm_phy_copy.access, &tmp_lock));
        *pmd_lock &= (uint32_t) tmp_lock;
    }

    return PHYMOD_E_NONE;
}

/* this function gives the rx signal detection STATUS */
int tscbh_fe_gen2_phy_rx_signal_detect_get(const phymod_phy_access_t* phy, uint32_t* signal_detect)
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
            (blackhawk7_l2p2_signal_detect(&pm_phy_copy.access, &tmp_detect));
        *signal_detect &= tmp_detect;
    }

    return PHYMOD_E_NONE;
}

/*Set/Get timesync enable*/
int tscbh_fe_gen2_timesync_enable_set(const phymod_phy_access_t* phy, uint32_t flags, uint32_t enable)
{
    return PHYMOD_E_NONE;
}

/* Only one flag can be served each time.
 */
int tscbh_fe_gen2_timesync_enable_get(const phymod_phy_access_t* phy, uint32_t flags, uint32_t* enable)
{
    return PHYMOD_E_NONE;
}

/*Set timesync adjust*/
int tscbh_fe_gen2_timesync_adjust_set(const phymod_phy_access_t* phy,
                                      uint32_t flags,
                                      const phymod_timesync_adjust_config_info_t *config)
{
    return PHYMOD_E_NONE;
}

/* This function based on num_lane, data_rate and fec_type
 * assign force speed SW speed_id.
 */
STATIC
int _tscbh_fe_gen2_phy_speed_id_set(int num_lane,
                                    uint32_t data_rate,
                                    phymod_fec_type_t fec_type,
                                    tbhmod_fe_gen2_spd_intfc_type_t* spd_intf)
{
    if (num_lane == 1) {
        switch (data_rate) {
            case 10000:
                if (fec_type == phymod_fec_None) {
                    *spd_intf = TBHMOD_FE_GEN2_SPD_10G_IEEE_KR1;
                } else if (fec_type == phymod_fec_CL74) {
                    *spd_intf = TBHMOD_FE_GEN2_SPD_10G_IEEE_FEC_BASE_R_N1;
                } else {
                    return PHYMOD_E_UNAVAIL;
                }
                break;

            case 12000:
                *spd_intf = TBHMOD_FE_GEN2_SPD_12P12G_BRCM_KR1;
                break;

            case 20000:
                if (fec_type == phymod_fec_None) {
                    *spd_intf = TBHMOD_FE_GEN2_SPD_20G_BRCM_CR1;
                } else if (fec_type == phymod_fec_CL74) {
                    *spd_intf = TBHMOD_FE_GEN2_SPD_20G_BRCM_FEC_BASE_R_N1;
                } else {
                    return PHYMOD_E_UNAVAIL;
                }
                break;
            case 25000:
                if (fec_type == phymod_fec_None) {
                    *spd_intf = TBHMOD_FE_GEN2_SPD_25G_BRCM_NO_FEC_KR1_CR1;
                } else if (fec_type == phymod_fec_CL74) {
                    *spd_intf = TBHMOD_FE_GEN2_SPD_25G_BRCM_FEC_BASE_R_KR1_CR1;
                } else if (fec_type == phymod_fec_CL91) {
                    *spd_intf = TBHMOD_FE_GEN2_SPD_25G_BRCM_FEC_528_KR1_CR1;
                } else {
                    return PHYMOD_E_UNAVAIL;
                }
                break;
            case 50000:
                if (fec_type == phymod_fec_CL91) {
                    *spd_intf = TBHMOD_FE_GEN2_SPD_50G_BRCM_FEC_528_CR1_KR1;
                } else if (fec_type == phymod_fec_RS544) {
                    *spd_intf = TBHMOD_FE_GEN2_SPD_50G_IEEE_KR1_CR1;
                } else if (fec_type == phymod_fec_RS272) {
                    *spd_intf = TBHMOD_FE_GEN2_SPD_50G_BRCM_FEC_272_CR1_KR1;
                } else if (fec_type == phymod_fec_None) {
                    *spd_intf = TBHMOD_FE_GEN2_SPD_50G_BRCM_NO_FEC_CR1_KR1;
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
                    *spd_intf = TBHMOD_FE_GEN2_SPD_40G_BRCM_CR2;
                } else {
                    return PHYMOD_E_UNAVAIL;
                }
                break;
            case 50000:
                if (fec_type == phymod_fec_None) {
                    *spd_intf = TBHMOD_FE_GEN2_SPD_50G_BRCM_CR2_KR2_NO_FEC;
                } else if (fec_type == phymod_fec_CL91) {
                    *spd_intf = TBHMOD_FE_GEN2_SPD_50G_BRCM_CR2_KR2_RS_FEC;
                } else if (fec_type == phymod_fec_RS544) {
                    *spd_intf = TBHMOD_FE_GEN2_SPD_50G_BRCM_FEC_544_CR2_KR2;
                } else {
                    return PHYMOD_E_UNAVAIL;
                }
                break;
            case 100000:
                if (fec_type == phymod_fec_None) {
                    *spd_intf = TBHMOD_FE_GEN2_SPD_100G_BRCM_NO_FEC_KR2_CR2;
                } else if (fec_type == phymod_fec_CL91) {
                    *spd_intf = TBHMOD_FE_GEN2_SPD_100G_BRCM_FEC_528_KR2_CR2;
                } else if (fec_type == phymod_fec_RS544) {
                    *spd_intf = TBHMOD_FE_GEN2_SPD_100G_IEEE_KR2_CR2;
                } else if (fec_type == phymod_fec_RS272) {
                    *spd_intf = TBHMOD_FE_GEN2_SPD_100G_BRCM_FEC_272_CR2_KR2;
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
                    *spd_intf = TBHMOD_FE_GEN2_SPD_40G_IEEE_KR4;
                } else if (fec_type == phymod_fec_CL74) {
                    *spd_intf = TBHMOD_FE_GEN2_SPD_40G_IEEE_FEC_BASE_R_N4;
                } else {
                    return PHYMOD_E_UNAVAIL;
                }
                break;
            case 100000:
                if (fec_type == phymod_fec_None) {
                    *spd_intf = TBHMOD_FE_GEN2_SPD_100G_BRCM_NO_FEC_X4;
                } else if (fec_type == phymod_fec_CL91) {
                    *spd_intf = TBHMOD_FE_GEN2_SPD_100G_IEEE_KR4;
                } else if (fec_type == phymod_fec_RS544) {
                    *spd_intf = TBHMOD_FE_GEN2_SPD_100G_BRCM_KR4_CR4;
                } else {
                    return PHYMOD_E_UNAVAIL;
                }
                break;
            case 200000:
                if (fec_type == phymod_fec_None) {
                    *spd_intf = TBHMOD_FE_GEN2_SPD_200G_BRCM_NO_FEC_KR4_CR4;
                } else if (fec_type == phymod_fec_RS544) {
                    *spd_intf = TBHMOD_FE_GEN2_SPD_200G_BRCM_KR4_CR4;
                } else if (fec_type == phymod_fec_RS544_2XN) {
                    *spd_intf = TBHMOD_FE_GEN2_SPD_200G_IEEE_KR4_CR4;
                } else if (fec_type == phymod_fec_RS272) {
                    *spd_intf = TBHMOD_FE_GEN2_SPD_200G_BRCM_FEC_272_N4;
                } else if (fec_type == phymod_fec_RS272_2XN) {
                    *spd_intf = TBHMOD_FE_GEN2_SPD_200G_BRCM_FEC_272_CR4_KR4;
                } else {
                    return PHYMOD_E_UNAVAIL;
                }
                break;
            default:
                return PHYMOD_E_UNAVAIL;
        }
    } else if (num_lane == 8) {
        if (data_rate == 400000 && fec_type == phymod_fec_RS544_2XN) {
            *spd_intf = TBHMOD_FE_GEN2_SPD_400G_BRCM_FEC_KR8_CR8;
        }else if (data_rate == 400000 && fec_type == phymod_fec_RS272_2XN) {
            *spd_intf = TBHMOD_FE_GEN2_SPD_400G_BRCM_FEC_272_N8;
        } else {
            return PHYMOD_E_UNAVAIL;
        }
    } else {
        return PHYMOD_E_UNAVAIL;
    }

    return PHYMOD_E_NONE;
}

int tscbh_fe_gen2_phy_speed_config_set(const phymod_phy_access_t* phy,
                                       const phymod_phy_speed_config_t* speed_config,
                                       const phymod_phy_pll_state_t* old_pll_state,
                                       phymod_phy_pll_state_t* new_pll_state)
{
    phymod_phy_access_t pm_phy_copy;
    uint32_t lane_mask_backup;
    uint32_t pll_index = 0, request_pll_div = 0, pll0_div = 0, pll1_div = 0;
    uint32_t loss_in_db;
    int i, start_lane, num_lane, mapped_speed_id, ilkn_set;
    phymod_firmware_lane_config_t firmware_lane_config;
    phymod_firmware_core_config_t firmware_core_config;
    tbhmod_fe_gen2_spd_intfc_type_t spd_intf = 0;
    tbhmod_refclk_t ref_clk;
    uint32_t is_pam4, osr_mode, osr_2p5_is_required = 0, port_enable;
    uint32_t *tscbh_fe_gen2_spd_id_entry;
    uint8_t is_otn_mode;

    tscbh_fe_gen2_spd_id_entry = tscbh_fe_gen2_spd_id_entry_get();

    firmware_lane_config = speed_config->pmd_lane_config;

    PHYMOD_MEMSET(&firmware_core_config, 0x0, sizeof(firmware_core_config));

    /* Copy the PLL state */
    *new_pll_state = *old_pll_state;

    PHYMOD_MEMCPY(&pm_phy_copy, phy, sizeof(pm_phy_copy));
    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(&phy->access, &start_lane, &num_lane));
    lane_mask_backup = phy->access.lane_mask;

    /* get current port enable bit */
    PHYMOD_IF_ERR_RETURN
        (tbhmod_port_enable_get(&pm_phy_copy.access, &port_enable));

    if (!PHYMOD_DEVICE_OP_MODE_PCS_BYPASS_GET(phy->device_op_mode)) {
        /* then clear the port an mode enable bit */
        PHYMOD_IF_ERR_RETURN
            (tbhmod_port_an_mode_enable_set(&pm_phy_copy.access, 0));

        /* Hold the pcs lane reset */
        pm_phy_copy.access.lane_mask = 1 << start_lane;
        PHYMOD_IF_ERR_RETURN
            (tbhmod_disable_set(&pm_phy_copy.access));

        
        /* add the pcs disable SW WAR */
        PHYMOD_IF_ERR_RETURN
            (tbhmod_fe_gen2_pcs_reset_sw_war(&phy->access));

        /* write this port forced speed id entry */
        PHYMOD_IF_ERR_RETURN
          (tbhmod_set_sc_speed(&pm_phy_copy.access, TSCBH_FORCED_SPEED_ID_OFFSET + start_lane, 0));
    }

    /*Hold the per lane PMD soft reset bit*/
    pm_phy_copy.access.lane_mask = lane_mask_backup;
    PHYMOD_IF_ERR_RETURN
        (blackhawk7_l2p2_lane_soft_reset(&pm_phy_copy.access, 1));

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
                (blackhawk7_l2p2_pmd_tx_disable_pin_dis_set(&pm_phy_copy.access, 0));
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
                (blackhawk7_l2p2_pmd_tx_disable_pin_dis_set(&pm_phy_copy.access, 1));
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
        PHYMOD_IF_ERR_RETURN(_tscbh_fe_gen2_phy_speed_id_set(num_lane, speed_config->data_rate,
                                                     speed_config->fec_type, &spd_intf));

        PHYMOD_IF_ERR_RETURN
            (tbhmod_fe_gen2_get_mapped_speed(spd_intf, &mapped_speed_id));

        /* set the rs fec CW properly */
        if ((speed_config->fec_type == phymod_fec_RS544) ||
            (speed_config->fec_type == phymod_fec_RS544_2XN)) {
            if (start_lane < 4) {
                pm_phy_copy.access.lane_mask = 1 << 0;
                PHYMOD_IF_ERR_RETURN
                    (tbhmod_rsfec_cw_type(&pm_phy_copy.access, 0, 0));
            }
            if ((start_lane >= 4) || (num_lane == 8)) {
                pm_phy_copy.access.lane_mask = 1 << 4;
                PHYMOD_IF_ERR_RETURN
                    (tbhmod_rsfec_cw_type(&pm_phy_copy.access, 0, 0));
            }
        } else if ((speed_config->fec_type == phymod_fec_RS272) ||
                   (speed_config->fec_type == phymod_fec_RS272_2XN)) {
            if (start_lane < 4) {
                pm_phy_copy.access.lane_mask = 1 << 0;
                PHYMOD_IF_ERR_RETURN
                    (tbhmod_rsfec_cw_type(&pm_phy_copy.access, 1, 0));
            }
            if ((start_lane >= 4) || (num_lane == 8)) {
                pm_phy_copy.access.lane_mask = 1 << 4;
                PHYMOD_IF_ERR_RETURN
                    (tbhmod_rsfec_cw_type(&pm_phy_copy.access, 1, 0));
            }
        }
    }

    /* load speed id entry for (non)ilkn port*/
    if (!PHYMOD_DEVICE_OP_MODE_PCS_BYPASS_GET(phy->device_op_mode)) {
        /* first set the lane mask to be 0x1 */
        pm_phy_copy.access.lane_mask = 1 << 0;
        PHYMOD_IF_ERR_RETURN
            (phymod_mem_write(&pm_phy_copy.access, phymodMemSpeedIdTable, TSCBH_FORCED_SPEED_ID_OFFSET + start_lane,
                              (tscbh_fe_gen2_spd_id_entry + mapped_speed_id * TSCBH_FE_GEN2_SPEED_ID_ENTRY_SIZE)));
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

        /* for the 10G single lane and 40G 4 lane case, either 20G or 25G VCO can work */
        /* if request 20G VCO, then use 25G VCO */
        if (((speed_config->data_rate == 10000) && (num_lane == 1)) ||
            ((speed_config->data_rate == 40000) && (num_lane == 4))) {
            if (request_pll_div == TBHMOD_PLL_MODE_DIV_132) {
                request_pll_div = TBHMOD_PLL_MODE_DIV_165;
                osr_2p5_is_required = 1;
            } else if (request_pll_div == TBHMOD_PLL_MODE_DIV_66) {
                request_pll_div = TBHMOD_PLL_MODE_DIV_82P5;
                osr_2p5_is_required = 1;
            }
        }
    } else {
        /*first check the ref clock */
        int ref_clk_is_156p25 = 0;
        if (ref_clk == TBHMOD_REF_CLK_156P25MHZ) {
            ref_clk_is_156p25 = 1;
        }
        if (PHYMOD_SPEED_CONFIG_IN_OTN_MODE_GET(speed_config)) {
            is_otn_mode = 1;
        } else {
            is_otn_mode = 0;
        }
        PHYMOD_IF_ERR_RETURN
        (blackhawk7_l2p2_speed_config_get(speed_config->data_rate, ref_clk_is_156p25, &request_pll_div, &is_pam4, &osr_mode, is_otn_mode));
        if ((speed_config->data_rate == 10312) ||
            (speed_config->data_rate == 41250)) {
            if (request_pll_div == TBHMOD_PLL_MODE_DIV_132) {
                request_pll_div = TBHMOD_PLL_MODE_DIV_165;
                osr_2p5_is_required = 1;
            } else if (request_pll_div == TBHMOD_PLL_MODE_DIV_66) {
                request_pll_div = TBHMOD_PLL_MODE_DIV_82P5;
                osr_2p5_is_required = 1;
            }
        }
    }

    /* for flex pll selection mode: choose the right pll index for the port */
    if (PHYMOD_DEVICE_OP_MODE_FLEXIBLE_PLL_SELECT_GET(phy->device_op_mode)) {
        /* get current pll0 div */
        pm_phy_copy.access.pll_idx = 0;
        PHYMOD_IF_ERR_RETURN
            (blackhawk7_l2p2_INTERNAL_read_pll_div(&pm_phy_copy.access, &pll0_div));
        /* get current pll1 div */
        pm_phy_copy.access.pll_idx = 1;
        PHYMOD_IF_ERR_RETURN
            (blackhawk7_l2p2_INTERNAL_read_pll_div(&pm_phy_copy.access, &pll1_div));

        if ((pll0_div == request_pll_div) || (pll1_div == request_pll_div)) {
            /* check if the new speed can be supported with existing VCO */
            pll_index = (pll0_div == request_pll_div)? 0 : 1;
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
                (blackhawk7_l2p2_lane_tx_pll_selection_set(&pm_phy_copy.access, pll_index));
            PHYMOD_IF_ERR_RETURN
                (blackhawk7_l2p2_lane_rx_pll_selection_set(&pm_phy_copy.access, pll_index));
        }
    }

    pm_phy_copy.access.lane_mask = lane_mask_backup;
    /* Program OS mode */
    if (!PHYMOD_DEVICE_OP_MODE_PCS_BYPASS_GET(phy->device_op_mode)) {
        /* need to check if osr 2.5 is required */
        if (osr_2p5_is_required) {
            PHYMOD_IF_ERR_RETURN
                (blackhawk7_l2p2_osr_mode_set(&pm_phy_copy.access, PMD_OSR_2P5));
        } else {
            for (i = 0; i < num_lane; i++) {
                pm_phy_copy.access.lane_mask = 1 << (start_lane + i);
                if (!PHYMOD_LANEPBMP_MEMBER(phy->access.lane_mask, start_lane + i)) {
                    continue;
                }
                PHYMOD_IF_ERR_RETURN
                    (tbhmod_fe_gen2_pmd_osmode_set(&pm_phy_copy.access, mapped_speed_id, ref_clk));
            }
        }
    } else {
        if (osr_2p5_is_required) {
            PHYMOD_IF_ERR_RETURN
                (blackhawk7_l2p2_osr_mode_set(&pm_phy_copy.access, PMD_OSR_2P5));
        } else {
            PHYMOD_IF_ERR_RETURN
                (blackhawk7_l2p2_osr_mode_set(&pm_phy_copy.access, osr_mode));
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
             (_tscbh_fe_gen2_phy_firmware_lane_config_set(&pm_phy_copy, firmware_lane_config));
    }

    /* if the PAM4 mode, need to program the channel loss. In NRZ mode it is zeroed. */
    loss_in_db = firmware_lane_config.ForcePAM4Mode? speed_config->PAM4_channel_loss : 0;
    for (i = 0; i < num_lane; i++) {
        pm_phy_copy.access.lane_mask = 0x1 << (start_lane + i);
        if (!PHYMOD_LANEPBMP_MEMBER(phy->access.lane_mask, start_lane + i)) {
            continue;
        }
        PHYMOD_IF_ERR_RETURN
            (blackhawk7_l2p2_channel_loss_set(&pm_phy_copy.access, loss_in_db));
    }

    /* next need to enable/disable link training based on the input */
    if (speed_config->linkTraining) {
        PHYMOD_IF_ERR_RETURN
            (tscbh_fe_gen2_phy_cl72_set(phy, speed_config->linkTraining));
    } else {
        /* disable cl72 and avoid overwriting the value from above _firmware_lane_config_set */
        for (i = 0; i < num_lane; i++) {
            pm_phy_copy.access.lane_mask = 1 << (start_lane + i);
            if (!PHYMOD_LANEPBMP_MEMBER(phy->access.lane_mask, start_lane + i)) {
                continue;
            }
            PHYMOD_IF_ERR_RETURN
                (blackhawk7_l2p2_clause72_control(&pm_phy_copy.access, speed_config->linkTraining));
        }
    }

    /*release the lne soft reset bit*/
    PHYMOD_MEMCPY(&pm_phy_copy, phy, sizeof(pm_phy_copy));
    PHYMOD_IF_ERR_RETURN
        (blackhawk7_l2p2_lane_soft_reset(&pm_phy_copy.access, 0));

    /* Release the pcs lane reset only if the port is enabled */
    if (!PHYMOD_DEVICE_OP_MODE_PCS_BYPASS_GET(phy->device_op_mode)){
        if (port_enable) {
            pm_phy_copy.access.lane_mask = 1 << start_lane;
            PHYMOD_IF_ERR_RETURN
                (tbhmod_enable_set(&pm_phy_copy.access));
        }
    }

    /* first clear the current lane _mask from both */
    new_pll_state->pll1_lanes_bitmap &= ~(phy->access.lane_mask);
    new_pll_state->pll0_lanes_bitmap &= ~(phy->access.lane_mask);
    /* need to update the pll_state */
    for (i = 0; i < num_lane; i++) {
        if (!PHYMOD_LANEPBMP_MEMBER(phy->access.lane_mask, start_lane + i)) {
            continue;
        }
        pm_phy_copy.access.lane_mask = 1 << (start_lane + i);
        PHYMOD_IF_ERR_RETURN
            (blackhawk7_l2p2_lane_tx_pll_selection_get(&pm_phy_copy.access, &pll_index));
        if (pll_index) {
            new_pll_state->pll1_lanes_bitmap |= pm_phy_copy.access.lane_mask;
        } else {
            new_pll_state->pll0_lanes_bitmap |= pm_phy_copy.access.lane_mask;
        }
    }

    return PHYMOD_E_NONE;
}

STATIC
int _tscbh_fe_gen2_speed_table_entry_to_speed_config_get(const phymod_phy_access_t* phy,
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
        (blackhawk7_l2p2_lane_tx_pll_selection_get(&phy_copy.access, &pll_index));

    phy_copy.access.pll_idx = pll_index;

    PHYMOD_IF_ERR_RETURN
        (tscbh_fe_gen2_phy_firmware_lane_config_get(phy, &firmware_lane_config));

    /* get the PLL div from HW */
    PHYMOD_IF_ERR_RETURN
        (blackhawk7_l2p2_INTERNAL_read_pll_div(&phy_copy.access, &pll_div));

    PHYMOD_IF_ERR_RETURN
        (tbhmod_refclk_get(&phy_copy.access, &ref_clk));

    if (ref_clk == TBHMOD_REF_CLK_312P5MHZ) {
        refclk_in_hz = 312500000;
    } else {
        refclk_in_hz = 156250000;
    }
    PHYMOD_IF_ERR_RETURN
        (blackhawk7_l2p2_get_vco_from_refclk_div(&phy_copy.access, refclk_in_hz, pll_div, &vco_rate, 0));

    PHYMOD_IF_ERR_RETURN
        (blackhawk7_l2p2_osr_mode_get(&phy_copy.access, &osr_mode));

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
        /* first check if osr 2.5 is used or not */
        if (osr_mode == PMD_OSR_2P5) {
            data_rate_lane = 10000;
        } else {
            data_rate_lane = data_rate_lane >> osr_mode;
        }
    }

    speed_config->data_rate = data_rate_lane * num_lane;

    PHYMOD_IF_ERR_RETURN
        (tbhmod_fec_arch_decode_get(speed_config_entry->fec_arch, &(speed_config->fec_type)));

    return PHYMOD_E_NONE;
}

int tscbh_fe_gen2_phy_speed_config_get(const phymod_phy_access_t* phy,
                                       phymod_phy_speed_config_t* speed_config)
{
    uint32_t cl72_enable;
    phymod_phy_access_t phy_copy;
    phymod_firmware_lane_config_t firmware_lane_config;
    int start_lane, num_lane, speed_id;
    uint32_t packed_entry[20] = {0};
    spd_id_tbl_entry_t speed_config_entry;
    int an_en, an_done, osr_mode;
    uint32_t pll_div, vco_freq_khz, pll_index, lane_mask_backup;
    tbhmod_refclk_t ref_clk;

    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));
    lane_mask_backup = phy->access.lane_mask;

    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(&phy->access, &start_lane, &num_lane));

    PHYMOD_IF_ERR_RETURN
        (tscbh_fe_gen2_phy_firmware_lane_config_get(phy, &firmware_lane_config));

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

        /*decode speed entry */
        spd_ctrl_unpack_spd_id_tbl_entry(packed_entry, &speed_config_entry);

        PHYMOD_IF_ERR_RETURN
            (_tscbh_fe_gen2_speed_table_entry_to_speed_config_get(phy, &speed_config_entry, speed_config));

        /* if autoneg enabled, needs to update the FEC_ARCH based on the An resolved status */

        if (an_en && an_done) {
            uint8_t fec_arch;
            uint32_t fec_align_live = 1;
            phy_copy.access.lane_mask = 0x1 << start_lane;

            PHYMOD_IF_ERR_RETURN
                (tbhmod_autoneg_fec_status_get(&phy_copy.access, &fec_arch));
            PHYMOD_IF_ERR_RETURN
                (tbhmod_fec_arch_decode_get(fec_arch, &(speed_config->fec_type)));

            /* For 100G 4 lane NO FEC the AN resolved status is not correct 
             * Hence check if FEC align indeed happened to distinguish NO FEC case*/
            if ((speed_id == SPEED_ID_INDEX_100G_4_LANE_CL73_KR4) ||
                (speed_id == SPEED_ID_INDEX_100G_4_LANE_CL73_CR4)) {
                PHYMOD_IF_ERR_RETURN
                    (tbhmod_fe_gen2_fec_align_status_get(&phy_copy.access, &fec_align_live));
                if (!fec_align_live) {
                    /* Case of 100G 4 LANE with NO FEC */
                    speed_config->fec_type = phymod_fec_None;
                }
            }
        }
    } else {
        /*first figure out which pll the starting lane is using */
        phy_copy.access.lane_mask = 0x1 << start_lane;
        PHYMOD_IF_ERR_RETURN
            (blackhawk7_l2p2_lane_tx_pll_selection_get(&phy_copy.access, &pll_index));

        phy_copy.access.pll_idx = pll_index;
         /* get the PLL div from HW */
        PHYMOD_IF_ERR_RETURN
            (blackhawk7_l2p2_INTERNAL_read_pll_div(&phy_copy.access, &pll_div));

        phy_copy.access.lane_mask = 0x1;
        /* next read what's the ref clock */
        PHYMOD_IF_ERR_RETURN
           (tbhmod_refclk_get(&phy_copy.access, &ref_clk));

        phy_copy.access.lane_mask = 0x1 << start_lane;
        if (ref_clk == TBHMOD_REF_CLK_156P25MHZ) {
            PHYMOD_IF_ERR_RETURN
                (blackhawk7_l2p2_get_vco_from_refclk_div(&phy_copy.access, REF_CLOCK_156P25, pll_div, &vco_freq_khz, 0));
        } else {
            PHYMOD_IF_ERR_RETURN
                (blackhawk7_l2p2_get_vco_from_refclk_div(&phy_copy.access, REF_CLOCK_312P5, pll_div, &vco_freq_khz, 0));
        }

        phy_copy.access.lane_mask = lane_mask_backup;

        PHYMOD_IF_ERR_RETURN
            (blackhawk7_l2p2_osr_mode_get(&phy_copy.access, &osr_mode));
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
            } else if (osr_mode == PMD_OSR_2P5) {
                speed_config->data_rate = vco_freq_khz / 2500;
            }
        }
    }

    /* next get the cl72 enable status */
    phy_copy.access.lane_mask = 0x1 << start_lane;
    PHYMOD_IF_ERR_RETURN
        (blackhawk7_l2p2_clause72_control_get(&phy_copy.access, &cl72_enable));
    speed_config->linkTraining = cl72_enable;

    return PHYMOD_E_NONE;
}

int tscbh_fe_gen2_phy_firmware_load_info_get(const phymod_phy_access_t* phy,
                                             phymod_firmware_load_info_t* info)
{
    info->ucode_len = BLACKHAWK7_V1L2P2_UCODE_IMAGE_SIZE;
    info->ucode_ptr = blackhawk7_l2p2_ucode_get();

    return PHYMOD_E_NONE;
}


int tscbh_fe_gen2_phy_autoneg_advert_ability_set(const phymod_phy_access_t* phy,
                                         const phymod_autoneg_advert_abilities_t* an_advert_abilities,
                                         const phymod_phy_pll_state_t* old_pll_adv_state,
                                         phymod_phy_pll_state_t* new_pll_adv_state)
{
    int start_lane, num_lane;
    phymod_phy_access_t phy_copy;

    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(&phy->access, &start_lane, &num_lane));

    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));
    phy_copy.access.lane_mask = 0x1 << start_lane;

    /* Program local advert abilitiy registers */
    PHYMOD_IF_ERR_RETURN
        (tbhmod_autoneg_ability_set(&phy_copy.access, an_advert_abilities));

    /* Copy existing PLL lane bitmap: unused */
    *new_pll_adv_state = *old_pll_adv_state;

    return PHYMOD_E_NONE;
}

int tscbh_fe_gen2_phy_autoneg_advert_ability_get(const phymod_phy_access_t* phy,
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
        (tbhmod_autoneg_ability_get(&phy_copy.access, an_advert_abilities));

    /* Get Medium type from fw_lane_config */
    PHYMOD_IF_ERR_RETURN
        (tscbh_fe_gen2_phy_firmware_lane_config_get(phy, &firmware_lane_config));

    for (i = 0; i < an_advert_abilities->num_abilities; i++) {
        an_advert_abilities->autoneg_abilities[i].medium = firmware_lane_config.MediaType;
    }

    return PHYMOD_E_NONE;
}

int tscbh_fe_gen2_phy_autoneg_remote_advert_ability_get(const phymod_phy_access_t* phy,
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

    return PHYMOD_E_NONE;
}

int tscbh_fe_gen2_phy_tx_pam4_precoder_enable_set(const phymod_phy_access_t* phy, int enable)
{
    phymod_phy_access_t phy_copy;
    int start_lane, num_lane, i;
    uint32_t lane_reset, pcs_lane_enable;

    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(&phy->access, &start_lane, &num_lane));
    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));

    /*first check if lane is in reset */
    PHYMOD_IF_ERR_RETURN
        (blackhawk7_l2p2_lane_soft_reset_get(&phy_copy.access, &lane_reset));

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
            (tbhmod_fe_gen2_pcs_reset_sw_war(&phy->access));
    }

    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));
    /* if lane is not in reset, then reset the lane first */
    if (!lane_reset) {
        PHYMOD_IF_ERR_RETURN
            (blackhawk7_l2p2_lane_soft_reset(&phy_copy.access, 1));
    }

    for (i = 0; i < num_lane; i++) {
        if (!PHYMOD_LANEPBMP_MEMBER(phy->access.lane_mask, start_lane + i)) {
            continue;
        }
        phy_copy.access.lane_mask = 1 << (start_lane + i);
        PHYMOD_IF_ERR_RETURN
            (blackhawk7_l2p2_tx_pam4_precoder_enable_set(&phy_copy.access, enable));
    }

    /* release the ln dp reset */
    if (!lane_reset) {
        PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));
        PHYMOD_IF_ERR_RETURN
            (blackhawk7_l2p2_lane_soft_reset(&phy_copy.access, 0));
    }

    /* re-enable pcs lane if pcs lane not in rset */
    if (pcs_lane_enable) {
        phy_copy.access.lane_mask = 1 << start_lane;
        PHYMOD_IF_ERR_RETURN
            (tbhmod_enable_set(&phy_copy.access));
    }

    return PHYMOD_E_NONE;
}

int tscbh_fe_gen2_phy_tx_pam4_precoder_enable_get(const phymod_phy_access_t* phy, int *enable)
{
    phymod_phy_access_t pm_phy_copy;

    PHYMOD_MEMCPY(&pm_phy_copy, phy, sizeof(pm_phy_copy));
    PHYMOD_IF_ERR_RETURN
        (blackhawk7_l2p2_tx_pam4_precoder_enable_get(&pm_phy_copy.access, enable));

    return PHYMOD_E_NONE;
}

int tscbh_fe_gen2_phy_pll_reconfig(const phymod_phy_access_t* phy,
                                   uint8_t pll_index,
                                   uint32_t pll_div,
                                   phymod_ref_clk_t ref_clk1)
{
    phymod_phy_access_t pm_phy_copy;
    tbhmod_refclk_t ref_clk;
    /*phymod_tscbh_pll_multiplier_t pll_0, pll_1; */
    uint32_t i, pll_is_pwrdn, default_pll_index = 0;
    enum blackhawk7_l2p2_pll_refclk_enum refclk;
    uint32_t cnt = 0, pll_lock = 0, current_pll_div = 0;
    int start_lane, num_lane;

    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(&phy->access, &start_lane, &num_lane));
    PHYMOD_MEMCPY(&pm_phy_copy, phy, sizeof(pm_phy_copy));

    if (pll_index > 1) {
        PHYMOD_DEBUG_ERROR(("Unsupported PLL index\n"));
        return PHYMOD_E_UNAVAIL;
    }

    pm_phy_copy.access.lane_mask = 1 << 0;
    /* first needs to read the ref clock from main reg*/
    PHYMOD_IF_ERR_RETURN
        (tbhmod_refclk_get(&pm_phy_copy.access, &ref_clk));

    if (ref_clk == TBHMOD_REF_CLK_156P25MHZ) {
        refclk = BLACKHAWK7_L2P2_PLL_REFCLK_156P25MHZ;
    } else if (ref_clk == TBHMOD_REF_CLK_312P5MHZ) {
        refclk = BLACKHAWK7_L2P2_PLL_REFCLK_312P5MHZ;
    } else {
        PHYMOD_DEBUG_ERROR(("Unsupported reference clock.\n"));
        return PHYMOD_E_UNAVAIL;
    }

    /*next check if the PLL is power down or not */
    for (i = 0; i < num_lane; i++) {
        if (!PHYMOD_LANEPBMP_MEMBER(phy->access.lane_mask, start_lane + i)) {
            continue;
        }
        pm_phy_copy.access.lane_mask = 1 << (start_lane + i);

        if (PHYMOD_DEVICE_OP_MODE_FLEXIBLE_PLL_SELECT_GET(phy->device_op_mode)) {
            /* flex pll selection mode: pll_index is passed down from portmod */
            pm_phy_copy.access.pll_idx = pll_index;
        } else {
            /* fixed pll mode: read default pll_index for current logical lane */
            PHYMOD_IF_ERR_RETURN
                (blackhawk7_l2p2_lane_tx_pll_selection_get(&pm_phy_copy.access, &default_pll_index));
            pm_phy_copy.access.pll_idx = default_pll_index;
        }
        PHYMOD_IF_ERR_RETURN
            (blackhawk7_l2p2_pll_pwrdn_get(&pm_phy_copy.access, &pll_is_pwrdn));

        /* if PLL is power down, need to power up first */
        if (pll_is_pwrdn) {
            /* Power up PLL if it's power down */
            PHYMOD_IF_ERR_RETURN
                (blackhawk7_l2p2_core_pwrdn(&pm_phy_copy.access, PWR_ON));
        }

        /*
         * Delay 300ms as PLL not lock Workaround
         */
        PHYMOD_USLEEP(300000);
        /* get current pll_div */
        PHYMOD_IF_ERR_RETURN
            (blackhawk7_l2p2_INTERNAL_read_pll_div(&pm_phy_copy.access, &current_pll_div));
        /* skip pll reconfig if current_pll_div is same as required pll_div */
        if (current_pll_div == pll_div) {
            continue;
        }
        /*next re-config pll divider */
        PHYMOD_IF_ERR_RETURN
            (blackhawk7_l2p2_core_dp_reset(&pm_phy_copy.access, 1));
        PHYMOD_IF_ERR_RETURN
            (blackhawk7_l2p2_configure_pll_refclk_div(&pm_phy_copy.access,
                                                      refclk,
                                                      pll_div));
        PHYMOD_IF_ERR_RETURN
            (blackhawk7_l2p2_core_dp_reset(&pm_phy_copy.access, 0));

        /* need to wait for the PLL lock */
        cnt = 0;
        while (cnt < 1000) {
            PHYMOD_IF_ERR_RETURN(blackhawk7_l2p2_pll_lock_get(&pm_phy_copy.access, &pll_lock));
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
    }

    return PHYMOD_E_NONE;
}

int tscbh_fe_gen2_phy_synce_clk_ctrl_set(const phymod_phy_access_t* phy,
                                         phymod_synce_clk_ctrl_t cfg)
{
    phymod_phy_access_t phy_copy;
    uint32_t current_pll_index, sdm_val;
    int osr_mode, start_lane, num_lane;
    int32_t pll_div;

    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(&phy->access, &start_lane, &num_lane));
    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));

    PHYMOD_IF_ERR_RETURN
         (tbhmod_synce_mode_set(&phy_copy.access, cfg.stg0_mode, cfg.stg1_mode));

    /* next check if SDM mode, if yes, needs to figure out the SDM value based on the current */
    if ((cfg.stg0_mode == 0x2) && (cfg.stg1_mode == 0x0)) {
        /* vco the port is using */
        /* first get the PLL index of the starting lane */
        phy_copy.access.lane_mask = 1 << start_lane;
        PHYMOD_IF_ERR_RETURN
            (blackhawk7_l2p2_lane_tx_pll_selection_get(&phy_copy.access, &current_pll_index));

        /* next get the OSR the port is current using */
        PHYMOD_IF_ERR_RETURN
            (blackhawk7_l2p2_osr_mode_get(&phy_copy.access, &osr_mode));

        /* next get the VCO the port is current using */
        phy_copy.access.pll_idx = current_pll_index;
        phy_copy.access.lane_mask = 1 << start_lane;
        PHYMOD_IF_ERR_RETURN
            (blackhawk7_l2p2_INTERNAL_read_pll_div(&phy_copy.access, (uint32_t *)&pll_div));

        /* next based on the VCO value and osr mode, set the SDM value properly */
        /* first check 26G VCO */
        if ((pll_div == TBHMOD_PLL_MODE_DIV_170) || (pll_div == TBHMOD_PLL_MODE_DIV_85))  {
            sdm_val = TSCBH_SYNCE_SDM_DIVISOR_26G_VCO;
        } else if ((pll_div == TBHMOD_PLL_MODE_DIV_165) || (pll_div == TBHMOD_PLL_MODE_DIV_82P5)) {
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
            /* this is oversample by 2 case */
            sdm_val >>= 1;
        } else if (osr_mode == 3) {
            /* this is oversample by 2.5  case */
            sdm_val = (sdm_val << 1) / 5;
        }

        /* next configure the SDM value */
        PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));
        PHYMOD_IF_ERR_RETURN
            (tbhmod_synce_clk_ctrl_set(&phy_copy.access, sdm_val));
    }

    return PHYMOD_E_NONE;
}

int tscbh_fe_gen2_phy_synce_clk_ctrl_get(const phymod_phy_access_t* phy,
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

int tscbh_fe_gen2_phy_pcs_enable_set(const phymod_phy_access_t* phy, uint32_t enable)
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
    }

    return PHYMOD_E_NONE;
}

int tscbh_fe_gen2_phy_rx_ppm_get(const phymod_phy_access_t* phy, int16_t* rx_ppm)
{
    int start_lane, num_lane;
    phymod_phy_access_t pm_phy_copy;

    PHYMOD_MEMCPY(&pm_phy_copy, phy, sizeof(pm_phy_copy));

    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(&phy->access, &start_lane, &num_lane));

    pm_phy_copy.access.lane_mask = 1 << start_lane;
    PHYMOD_IF_ERR_RETURN
        (blackhawk7_l2p2_rx_ppm(&pm_phy_copy.access, rx_ppm));

    return PHYMOD_E_NONE;
}

int tscbh_fe_gen2_timesync_tx_info_get(const phymod_phy_access_t* phy,
                                       phymod_ts_fifo_status_t* ts_tx_info)
{

    return PHYMOD_E_NONE;
}

int tscbh_fe_gen2_phy_pll_powerdown_get(const phymod_phy_access_t* phy,
                                        uint32_t pll_index, uint32_t* powerdown)
{
    phymod_phy_access_t phy_copy;
    int start_lane, num_lane;
    uint32_t i, default_pll_index = 0, pll_is_pwrdn = 0;

    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(&phy->access, &start_lane, &num_lane));

    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));

    *powerdown = 0;
    /* loop through each logical lane */
    for (i = 0; i < num_lane; i++) {
        if (!PHYMOD_LANEPBMP_MEMBER(phy->access.lane_mask, start_lane + i)) {
            continue;
        }
        phy_copy.access.lane_mask = 1 << (start_lane + i);
        if (PHYMOD_DEVICE_OP_MODE_FLEXIBLE_PLL_SELECT_GET(phy->device_op_mode)) {
            /* flex pll selection mode: pll_index is passed down from portmod */
            phy_copy.access.pll_idx = pll_index;
        } else {
            /* read default pll_index for current logical lane */
            PHYMOD_IF_ERR_RETURN
                (blackhawk7_l2p2_lane_tx_pll_selection_get(&phy_copy.access, &default_pll_index));
            phy_copy.access.pll_idx = default_pll_index;
        }
        /* check if PLL is powered down */
        PHYMOD_IF_ERR_RETURN
            (blackhawk7_l2p2_pll_pwrdn_get(&phy_copy.access, &pll_is_pwrdn));
        /* if any decidated PLL is powered down, then *powerdown is set to 1 */
        *powerdown |= pll_is_pwrdn;
    }

    return PHYMOD_E_NONE;
}

int tscbh_fe_gen2_phy_fec_bypass_indication_set(const phymod_phy_access_t* phy, uint32_t enable)
{
    int start_lane, num_lane;
    phymod_phy_access_t phy_copy;
    uint32_t lane_reset, pcs_lane_enable;

    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));

    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(&phy->access, &start_lane, &num_lane));

    /*first check if lane is in reset */
    PHYMOD_IF_ERR_RETURN
        (blackhawk7_l2p2_lane_soft_reset_get(&phy_copy.access, &lane_reset));

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
            (blackhawk7_l2p2_lane_soft_reset(&phy_copy.access, 1));
    }

    phy_copy.access.lane_mask = 1 << start_lane;
    PHYMOD_IF_ERR_RETURN(
      tbhmod_fec_bypass_indication_set(&phy_copy.access, enable));

    /* release the lane soft reset bit */
    if (!lane_reset) {
        PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));
        PHYMOD_IF_ERR_RETURN
            (blackhawk7_l2p2_lane_soft_reset(&phy_copy.access, 0));
    }

    /* re-enable pcs lane if pcs lane not in rset */
    if (pcs_lane_enable) {
        phy_copy.access.lane_mask = 1 << start_lane;
        PHYMOD_IF_ERR_RETURN
            (tbhmod_enable_set(&phy_copy.access));
    }

    return PHYMOD_E_NONE;
}

int tscbh_fe_gen2_phy_fec_bypass_indication_get(const phymod_phy_access_t* phy, uint32_t *enable)
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

int tscbh_fe_gen2_phy_fec_corruption_period_set(const phymod_phy_access_t* phy, uint32_t period)
{
    phymod_phy_access_t phy_copy;

    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));

    /*The unit is 15us in register, the unit is 1ms in API*/
    PHYMOD_IF_ERR_RETURN
        (tbhmod_fe_gen2_fec_corruption_period_set(&phy_copy.access, ((period * 1000) / 15)));

    return PHYMOD_E_NONE;
}

int tscbh_fe_gen2_phy_fec_corruption_period_get(const phymod_phy_access_t* phy, uint32_t *period)
{
    int start_lane, num_lane;
    phymod_phy_access_t phy_copy;
    uint32_t period_15_us;

    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));

    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(&phy->access, &start_lane, &num_lane));

    phy_copy.access.lane_mask = 1 << start_lane;

    /*The unit is 15us in register, the unit is 1ms in API*/
    PHYMOD_IF_ERR_RETURN
        (tbhmod_fe_gen2_fec_corruption_period_get(&phy_copy.access, &period_15_us));
    *period = ((period_15_us * 15) % 1000) ? ((period_15_us * 15) / 1000 + 1) : ((period_15_us * 15) / 1000);

    return PHYMOD_E_NONE;
}

int tscbh_fe_gen2_phy_rx_pmd_lock_counter_get(const phymod_phy_access_t* phy, uint32_t* counter)
{
    return PHYMOD_E_NONE;
}

int tscbh_fe_gen2_phy_fec_three_cw_bad_state_get(const phymod_phy_access_t* phy, uint32_t *state)
{
    int start_lane, num_lane;
    phymod_phy_access_t phy_copy;

    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));

    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(&phy->access, &start_lane, &num_lane));

    phy_copy.access.lane_mask = 1 << start_lane;

    PHYMOD_IF_ERR_RETURN
        (tbhmod_fe_gen2_fec_three_cw_bad_state_get(&phy_copy.access, state));

    return PHYMOD_E_NONE;
}


int tscbh_fe_gen2_port_enable_set(const phymod_phy_access_t* phy, uint32_t enable)
{
    phymod_phy_access_t phy_copy;
    uint32_t port_an_enable, port_enable, pcs_enable;
    int i, start_lane, num_lane;

    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));

    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(&phy->access, &start_lane, &num_lane));

    if (!PHYMOD_DEVICE_OP_MODE_PCS_BYPASS_GET(phy->device_op_mode)) {
        /* first read port an enable bit */
        PHYMOD_IF_ERR_RETURN
            (tbhmod_port_an_mode_enable_get(&phy_copy.access, &port_an_enable));

        /* next read current port enable bit */
        PHYMOD_IF_ERR_RETURN
            (tbhmod_port_enable_get(&phy_copy.access, &port_enable));

        /*next check if PCS lane is in reset */
        PHYMOD_IF_ERR_RETURN
            (tbhmod_enable_get(&phy_copy.access, &pcs_enable));

        if ((port_an_enable) || (port_enable && !pcs_enable)) {
            /* current port is in An mode mode */
            if (enable == 1) {
                /* next release both tx/rx squelch */
                PHYMOD_IF_ERR_RETURN(blackhawk7_l2p2_tx_disable(&phy_copy.access, 0));
                PHYMOD_IF_ERR_RETURN(blackhawk7_l2p2_pmd_force_signal_detect(&phy_copy.access, 0, 0));
                phy_copy.access.lane_mask = 1 << start_lane;
            }
            PHYMOD_IF_ERR_RETURN
                (tbhmod_port_cl73_enable_set(&phy_copy.access, enable));
            /*check if enable ==0 */
            if (!enable) {
                PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));
                /* next set both tx/rx squelch */
                PHYMOD_IF_ERR_RETURN(blackhawk7_l2p2_tx_disable(&phy_copy.access, 1));
                PHYMOD_IF_ERR_RETURN(blackhawk7_l2p2_pmd_force_signal_detect(&phy_copy.access, 1, 0));
            }
        } else {
            /* cuurent port is in forced speed mode */
            if (enable == 1) {
                /* next release both tx/rx squelch */
                PHYMOD_IF_ERR_RETURN(blackhawk7_l2p2_tx_disable(&phy_copy.access, 0));
                PHYMOD_IF_ERR_RETURN(blackhawk7_l2p2_pmd_force_signal_detect(&phy_copy.access, 0, 0));

                phy_copy.access.lane_mask = 1 << start_lane;
                /* enable speed control bit */
                PHYMOD_IF_ERR_RETURN
                    (tbhmod_enable_set(&phy_copy.access));
            } else if (enable == 0) {
                /* disable speed control bit */
                PHYMOD_IF_ERR_RETURN
                    (tbhmod_disable_set(&phy_copy.access));
                /* next set both tx/rx squelch */
                PHYMOD_IF_ERR_RETURN(blackhawk7_l2p2_tx_disable(&phy_copy.access, 1));
                PHYMOD_IF_ERR_RETURN(blackhawk7_l2p2_pmd_force_signal_detect(&phy_copy.access, 1, 0));
            }
        }

        /* next set port enable bit */
        PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));
        PHYMOD_IF_ERR_RETURN
            (tbhmod_port_enable_set(&phy_copy.access, enable));
    } else {
        for (i = 0; i < num_lane; i++) {
            if (!PHYMOD_LANEPBMP_MEMBER(phy->access.lane_mask, start_lane + i)) {
                continue;
            }
            phy_copy.access.lane_mask = 0x1 << (start_lane + i);
            PHYMOD_IF_ERR_RETURN(blackhawk7_l2p2_tx_disable(&phy_copy.access, enable ? 0 : 1));
            PHYMOD_IF_ERR_RETURN(blackhawk7_l2p2_pmd_force_signal_detect(&phy_copy.access, enable ? 0 : 1, 0));
            /* next set port enable bit */
            PHYMOD_IF_ERR_RETURN
                (tbhmod_port_enable_set(&phy_copy.access, enable));
        }
    }

    return PHYMOD_E_NONE;
}


int tscbh_fe_gen2_port_enable_get(const phymod_phy_access_t* phy,
                                  uint32_t* enable)
{
    phymod_phy_access_t phy_copy;

    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));

    /* first read port an enable bit */
    PHYMOD_IF_ERR_RETURN
        (tbhmod_port_enable_get(&phy_copy.access, enable));

    return PHYMOD_E_NONE;
}

int tscbh_fe_gen2_phy_txpi_config_set(const phymod_phy_access_t* phy,
                                      const phymod_txpi_config_t* config)
{

    return PHYMOD_E_NONE;
}

int tscbh_fe_gen2_phy_txpi_config_get(const phymod_phy_access_t* phy,
                                      phymod_txpi_config_t* config)
{

    return PHYMOD_E_NONE;
}

/* set FEC Degraded SER configurations*/
/*! 
 * phymod_phy_fec_degraded_ser_set
 *
 * @brief  Set/Get FEC Degraded SER configurations 
 *
 * @param [in]  phy             - phy access information
 * @param [in]  config          - 
 */
int tscbh_fe_gen2_phy_fec_degraded_ser_set(const phymod_phy_access_t* phy,
                                           phymod_fec_degraded_ser_config_t config)
{
    phymod_phy_access_t phy_copy;
    int start_lane, num_lane;

    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(&phy->access, &start_lane, &num_lane));
    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));
    /* set MPP0 */
    if (start_lane < 4) {
        phy_copy.access.lane_mask = 0x1;
        PHYMOD_IF_ERR_RETURN
            (tbhmod_fe_gen2_fec_degraded_ctrl_enable_set(&phy_copy.access, config.enable));
        PHYMOD_IF_ERR_RETURN
            (tbhmod_fe_gen2_fec_degraded_ctrl_activate_threshold_set(&phy_copy.access, config.activate_threshold));
        PHYMOD_IF_ERR_RETURN
            (tbhmod_fe_gen2_fec_degraded_ctrl_deactivate_threshold_set(&phy_copy.access, config.deactivate_threshold));
        PHYMOD_IF_ERR_RETURN
            (tbhmod_fe_gen2_fec_degraded_ctrl_interval_set(&phy_copy.access, config.interval));
    }
    /* set MPP1 */
    if ((start_lane >= 4) || (num_lane == 8)) {
        phy_copy.access.lane_mask = 0x1 << 4;
        PHYMOD_IF_ERR_RETURN
            (tbhmod_fe_gen2_fec_degraded_ctrl_enable_set(&phy_copy.access, config.enable));
        PHYMOD_IF_ERR_RETURN
            (tbhmod_fe_gen2_fec_degraded_ctrl_activate_threshold_set(&phy_copy.access, config.activate_threshold));
        PHYMOD_IF_ERR_RETURN
            (tbhmod_fe_gen2_fec_degraded_ctrl_deactivate_threshold_set(&phy_copy.access, config.deactivate_threshold));
        PHYMOD_IF_ERR_RETURN
            (tbhmod_fe_gen2_fec_degraded_ctrl_interval_set(&phy_copy.access, config.interval));
    }

    return PHYMOD_E_NONE;
}

/*! 
 * phymod_phy_fec_degraded_ser_get
 *
 * @brief  Set/Get FEC Degraded SER configurations 
 *
 * @param [in]  phy             - phy access information
 * @param [out]  config          - 
 */
int tscbh_fe_gen2_phy_fec_degraded_ser_get(const phymod_phy_access_t* phy,
                                           phymod_fec_degraded_ser_config_t* config)
{
    phymod_phy_access_t phy_copy;
    int start_lane, num_lane;

    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(&phy->access, &start_lane, &num_lane));

    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));
    /* get current settings */
    phy_copy.access.lane_mask = 1 << start_lane;
    PHYMOD_IF_ERR_RETURN
        (tbhmod_fe_gen2_fec_degraded_ctrl_enable_get(&phy_copy.access, &(config->enable)));
    PHYMOD_IF_ERR_RETURN
        (tbhmod_fe_gen2_fec_degraded_ctrl_activate_threshold_get(&phy_copy.access, &(config->activate_threshold)));
    PHYMOD_IF_ERR_RETURN
        (tbhmod_fe_gen2_fec_degraded_ctrl_deactivate_threshold_get(&phy_copy.access, &(config->deactivate_threshold)));
    PHYMOD_IF_ERR_RETURN
        (tbhmod_fe_gen2_fec_degraded_ctrl_interval_get(&phy_copy.access, &(config->interval)));

    return PHYMOD_E_NONE;
}

/*! 
 * phymod_phy_fec_degraded_ser_status_get
 *
 * @brief  get FEC Degraded SER status 
 *
 * @param [in]  phy             - phy access information
 * @param [out]  status          - 
 */
int tscbh_fe_gen2_phy_fec_degraded_ser_status_get(const phymod_phy_access_t* phy,
                                                  phymod_fec_degraded_ser_status_t* status)
{
    phymod_phy_access_t phy_copy;
    int start_lane, num_lane;

    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(&phy->access, &start_lane, &num_lane));

    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));
    /* get status */
    phy_copy.access.lane_mask = 1 << start_lane;
    PHYMOD_IF_ERR_RETURN
        (tbhmod_fe_gen2_fec_degraded_status_get(&phy_copy.access, status));

    return PHYMOD_E_NONE;
}

int tscbh_fe_gen2_phy_reg_read(const phymod_phy_access_t* phy, uint32_t reg_addr, uint32_t* val)
{
    PHYMOD_IF_ERR_RETURN(phymod_tscbh_iblk_read(&phy->access, reg_addr, val));
    return PHYMOD_E_NONE;
}


int tscbh_fe_gen2_phy_reg_write(const phymod_phy_access_t* phy, uint32_t reg_addr, uint32_t val)
{
    phymod_phy_access_t phy_copy;
    int start_lane, num_lane, i;

    if (IS_PMD_ACCESS(reg_addr)) {
        /* get the start lane and num lanes of the port lane mask */
        PHYMOD_IF_ERR_RETURN
            (phymod_util_lane_config_get(&phy->access, &start_lane, &num_lane));
        PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));
        /* per lane PMD access */
        for (i = 0; i < num_lane; i++) {
            if (!PHYMOD_LANEPBMP_MEMBER(phy->access.lane_mask, start_lane + i)) {
                continue;
            }
            phy_copy.access.lane_mask = 1 << (start_lane + i);
            PHYMOD_IF_ERR_RETURN(phymod_tscbh_iblk_write(&phy_copy.access, reg_addr, val));
        }
    } else {
        PHYMOD_IF_ERR_RETURN(phymod_tscbh_iblk_write(&phy->access, reg_addr, val));
    }
    return PHYMOD_E_NONE;
}

