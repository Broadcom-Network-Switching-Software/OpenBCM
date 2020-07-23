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
#include <phymod/phymod_debug.h>
#include <phymod/phymod_util.h>
#include <phymod/chip/bcmi_qtce_xgxs_defs.h>
#include <phymod/chip/eagle_common.h>

#include "../../qtce/tier1/qePCSRegEnums.h"
#include "../../qtce/tier1/qmod_enum_defines.h"
#include "../../qtce/tier1/qmod_device.h"
#include "../../qtce/tier1/qmod.h"
#include "../../qtce/tier1/qmod_defines.h"
#include "../../qtce/tier1/qmod_sc_lkup_table.h"
#include "../../qtce/tier1/phy_tsc_iblk.h"
#include "../../eagle/tier1/eagle_cfg_seq.h"
#include "../../eagle/tier1/eagle_tsc_common.h" 
#include "../../eagle/tier1/eagle_tsc_interface.h" 
#include "../../eagle/tier1/eagle_tsc_dependencies.h" 

#ifdef PHYMOD_QTCE_SUPPORT

#define QTCE_ID0        0x600d
#define QTCE_ID1        0x8770
#define QTCE_REV_MASK   0x0

#define QTCE4_MODEL     0x15
#define EAGLE_MODEL     0x1a

#define QTCE_NOF_DFES (5)
#define QTCE_NOF_LANES_IN_CORE (4) 
#define QTCE_LANE_SWAP_LANE_MASK (0x3)
#define QTCE_PHY_ALL_LANES (0xf)
#define QTCE_CORE_TO_PHY_ACCESS(_phy_access, _core_access) \
    do{\
        PHYMOD_MEMCPY(&(_phy_access)->access, &(_core_access)->access, sizeof((_phy_access)->access));\
        (_phy_access)->type = (_core_access)->type; \
        (_phy_access)->access.lane_mask = QTCE_PHY_ALL_LANES; \
    }while(0)

#define QTCE_PMD_CRC_UCODE  1
/* uController's firmware */
extern unsigned char qtce_ucode[];
extern unsigned short qtce_ucode_ver;
extern unsigned short qtce_ucode_crc;
extern unsigned short qtce_ucode_len;

typedef int (*sequncer_control_f)(const phymod_access_t* core, uint32_t enable);
typedef int (*rx_DFE_tap_control_set_f)(const phymod_access_t* phy, uint32_t val);

STATIC
int _qtce_phy_firmware_lane_config_set(const phymod_phy_access_t* phy, phymod_firmware_lane_config_t fw_config)
{
    struct eagle_tsc_uc_lane_config_st serdes_firmware_config;
    phymod_phy_access_t phy_copy;
    int start_lane, num_lane, i;
    /* uint32_t rst_status; */
    uint32_t is_warm_boot;

    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(&phy->access, &start_lane, &num_lane));
    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));

    for (i = 0; i < num_lane; i++) {
        phy_copy.access.lane_mask = 1 << (start_lane + i);
        serdes_firmware_config.field.lane_cfg_from_pcs = fw_config.LaneConfigFromPCS;
        serdes_firmware_config.field.an_enabled        = fw_config.AnEnabled;
        serdes_firmware_config.field.dfe_on            = fw_config.DfeOn; 
        serdes_firmware_config.field.force_brdfe_on    = fw_config.ForceBrDfe;
        /* serdes_firmware_config.field.cl72_emulation_en = fw_config.Cl72Enable; */
        serdes_firmware_config.field.scrambling_dis    = fw_config.ScramblingDisable;
        serdes_firmware_config.field.unreliable_los    = fw_config.UnreliableLos;
        serdes_firmware_config.field.media_type        = fw_config.MediaType; 

        PHYMOD_IF_ERR_RETURN(PHYMOD_IS_WRITE_DISABLED(&phy_copy.access, &is_warm_boot));

		
        if(!is_warm_boot) {
            /* PHYMOD_IF_ERR_RETURN(eagle_lane_soft_reset_read(&phy_copy.access, &rst_status)); */
            /* if(rst_status) */ PHYMOD_IF_ERR_RETURN (eagle_lane_soft_reset_release(&phy_copy.access, 0));
            PHYMOD_USLEEP(100); 
            PHYMOD_IF_ERR_RETURN(eagle_tsc_set_uc_lane_cfg(&phy_copy.access, serdes_firmware_config));
            /* if(rst_status) */ PHYMOD_IF_ERR_RETURN (eagle_lane_soft_reset_release(&phy_copy.access, 1));
        }
    }
    return PHYMOD_E_NONE;
}


int qtce_core_identify(const phymod_core_access_t* core, uint32_t core_id, uint32_t* is_identified)
{
    int ioerr = 0;
    const phymod_access_t *pm_acc = &core->access;
    PHYID2r_t id2;
    PHYID3r_t id3;
    MAIN_SERDESIDr_t serdesid;
    /* DIG_REVID0r_t revid; */
    uint32_t model;
    int rv ;
    *is_identified = 0;

    if(core_id == 0){
        ioerr += READ_PHYID2r(pm_acc, &id2);
        ioerr += READ_PHYID3r(pm_acc, &id3);
    }
    else{
        PHYID2r_SET(id2, ((core_id >> 16) & 0xffff));
        PHYID3r_SET(id3, core_id & 0xffff);
    }

    if (PHYID2r_GET(id2) == QTCE_ID0 &&
        (PHYID3r_GET(id3) &= ~QTCE_REV_MASK) == QTCE_ID1) {
        /* PHY IDs match - now check PCS model */
        ioerr += READ_MAIN_SERDESIDr(pm_acc, &serdesid);
        model = MAIN_SERDESIDr_MODEL_NUMBERf_GET(serdesid);
        if (model == QTCE4_MODEL)  {
            /* PCS model matches - now check PMD model */
            /* for now bypass the pmd register rev check */
            /* ioerr += READ_DIG_REVID0r(pm_acc, &revid);
            if (DIG_REVID0r_REVID_MODELf_GET(revid) == EAGLE_MODEL) */ {
                *is_identified = 1;
            }
        }
    }
    rv = ioerr ? PHYMOD_E_IO : PHYMOD_E_NONE;
    return rv ;
}

int qtce_core_info_get(const phymod_core_access_t* phy, phymod_core_info_t* info)
{
    uint32_t serdes_id;
    char core_name[15];
    PHYID2r_t id2;
    PHYID3r_t id3;
    const phymod_access_t *pm_acc = &phy->access;

    PHYMOD_IF_ERR_RETURN
        (qmod_revid_read(&phy->access, &serdes_id));
    PHYMOD_IF_ERR_RETURN
        (phymod_core_name_get(phy, serdes_id, core_name, info));
    info->serdes_id = serdes_id;

    if ((serdes_id & 0x3f) == QTCE4_MODEL) {
        info->core_version = phymodCoreVersionQtceA0;
        PHYMOD_STRNCPY(core_name, "QtceA0", PHYMOD_STRLEN("QtceA0")+1); 
    } else {
        info->core_version = phymodCoreVersionTsce12A0;
        PHYMOD_STRNCPY(core_name, "Tsce4", PHYMOD_STRLEN("Tsce4")+1);
    }
    PHYMOD_IF_ERR_RETURN
        (phymod_core_name_get(phy, serdes_id, core_name, info));

    PHYMOD_IF_ERR_RETURN(READ_PHYID2r(pm_acc, &id2));
    PHYMOD_IF_ERR_RETURN(READ_PHYID3r(pm_acc, &id3));

    info->phy_id0 = (uint16_t) id2.v[0];
    info->phy_id1 = (uint16_t) id3.v[0];
    return PHYMOD_E_NONE;
}

/*
 * Write 1 to pmd_x4_override.rx_lock_ovrd & pmd_x4_override.signal_detect_ovrd
 * fields of all 4 ports at core init time.
 * This function is used to solve the following issues:
 * 1.rx_lock and signal_ok indications from PMD are not swapped correctly.(SDK-132662)
 * 2.GH2 requires the Rx clock before we put the unimac port into MAC loopback.(SDK-115895)
 */
STATIC
int _qtce_core_lane_override_set(const phymod_core_access_t* core)
{
   uint32_t lane;
   phymod_phy_access_t phy_copy;

   QTCE_CORE_TO_PHY_ACCESS(&phy_copy, core);
   for(lane = 0 ; lane < QTCE_NOF_LANES_IN_CORE ; lane++){
       phy_copy.access.lane_mask = 1 << lane;
       PHYMOD_IF_ERR_RETURN
           (qmod_pmd_lane_override(&phy_copy.access, QMOD_PMD_LANE_SIGNAL_DETECT_OVRD));
       PHYMOD_IF_ERR_RETURN
           (qmod_pmd_lane_override(&phy_copy.access, QMOD_PMD_LANE_RX_LOCK_OVRD));
   }

   return PHYMOD_E_NONE;
}

/* 
 * set lane swapping for core 
 * The tx swap is composed of PCS swap and after that the PMD swap. 
 * The rx swap is composed just by PCS swap
 *
 * lane_map_tx and lane_map_rx[lane=logic_lane] are logic-lane base.
 * pcs_swap and register is logic_lane base.
 * but pmd_tx_map and addr_index_swap (and registers) are physical lane base 
 */
 
int qtce_core_lane_map_set(const phymod_core_access_t* core, const phymod_lane_map_t* lane_map)
{
    uint32_t pcs_swap = 0 , pmd_swap = 0, lane;
    uint32_t addr_index_swap = 0, pmd_tx_map =0;


    if(lane_map->num_of_lanes != QTCE_NOF_LANES_IN_CORE){
        return PHYMOD_E_CONFIG;
    }
    for( lane = 0 ; lane < QTCE_NOF_LANES_IN_CORE ; lane++){
        if(lane_map->lane_map_rx[lane] >= QTCE_NOF_LANES_IN_CORE){
            return PHYMOD_E_CONFIG;
        }
        /* encode each lane as four bits */
        /* pcs_map[lane] = rx_map[lane] */
        pcs_swap += lane_map->lane_map_rx[lane]<<(lane*4);
    }

    for( lane = 0 ; lane < QTCE_NOF_LANES_IN_CORE ; lane++){
        if(lane_map->lane_map_tx[lane] >= QTCE_NOF_LANES_IN_CORE){
            return PHYMOD_E_CONFIG;
        }
        /* encode each lane as four bits. pmd_swap is logic base */
        /* considering the pcs lane swap: pmd_map[pcs_map[lane]] = tx_map[lane] */
        pmd_swap += lane_map->lane_map_tx[lane]<<(lane_map->lane_map_rx[lane]*4);
    }

    for( lane = 0 ; lane < QTCE_NOF_LANES_IN_CORE ; lane++){
        addr_index_swap |= (lane << 4*((pcs_swap >> lane*4) & 0xf)) ;
        pmd_tx_map      |= (lane << 4*((pmd_swap >> lane*4) & 0xf)) ;
    }
    PHYMOD_IF_ERR_RETURN(qmod_pcs_lane_swap(&core->access, pcs_swap));

    PHYMOD_IF_ERR_RETURN
        (qmod_pmd_addr_lane_swap(&core->access, addr_index_swap));
    PHYMOD_IF_ERR_RETURN
        (qmod_pmd_lane_swap_tx(&core->access, pmd_tx_map));

    return PHYMOD_E_NONE;
}


int qtce_core_lane_map_get(const phymod_core_access_t* core, phymod_lane_map_t* lane_map)
{
    uint32_t pmd_swap = 0 , pcs_swap = 0, lane; 
    PHYMOD_IF_ERR_RETURN(qmod_pcs_lane_swap_get(&core->access, &pcs_swap));
    PHYMOD_IF_ERR_RETURN(qmod_pmd_lane_swap_tx_get(&core->access, &pmd_swap));
    for( lane = 0 ; lane < QTCE_NOF_LANES_IN_CORE ; lane++){
        /* deccode each lane from four bits */
        lane_map->lane_map_rx[lane] = (pcs_swap>>(lane*4)) & QTCE_LANE_SWAP_LANE_MASK;
        /* considering the pcs lane swap: tx_map[lane] = pmd_map[pcs_map[lane]] */
        lane_map->lane_map_tx[lane] = (pmd_swap>>(lane_map->lane_map_rx[lane]*4)) & QTCE_LANE_SWAP_LANE_MASK;
    }
    lane_map->num_of_lanes = QTCE_NOF_LANES_IN_CORE;   
    return PHYMOD_E_NONE;
}


int qtce_core_reset_set(const phymod_core_access_t* core, phymod_reset_mode_t reset_mode, phymod_reset_direction_t direction)
{
    
    /* Place your code here */

        
    return PHYMOD_E_NONE;
}


int qtce_core_reset_get(const phymod_core_access_t* core, phymod_reset_mode_t reset_mode, phymod_reset_direction_t* direction)
{       
    return PHYMOD_E_UNAVAIL;
}

int qtce_core_firmware_info_get(const phymod_core_access_t* core, phymod_core_firmware_info_t* fw_info)
{
    
    /* 
     * It's O.K to use this code as is since the firmware CRC is already checked at 
     * at the time we load it.
     */
    fw_info->fw_crc = qtce_ucode_crc;
    fw_info->fw_version = qtce_ucode_ver;
    return PHYMOD_E_NONE;
}

/* load qtce fw. the fw_loader parameter is valid just for external fw load */
STATIC
int _qtce_core_firmware_load(const phymod_core_access_t* core, phymod_firmware_load_method_t load_method, phymod_firmware_loader_f fw_loader)
{
#ifndef QTCE_PMD_CRC_UCODE
    phymod_core_firmware_info_t actual_fw;
#endif


    switch(load_method){
    case phymodFirmwareLoadMethodInternal:
        PHYMOD_IF_ERR_RETURN(eagle_tsc_ucode_mdio_load(&core->access, qtce_ucode, qtce_ucode_len));
        break;
    case phymodFirmwareLoadMethodExternal:
        PHYMOD_NULL_CHECK(fw_loader); 

        eagle_pram_flop_set(&core->access, 0x0);

        PHYMOD_IF_ERR_RETURN(eagle_tsc_ucode_init(&core->access));

        PHYMOD_IF_ERR_RETURN
            (qmod_pram_abl_enable_set(&core->access,1));
        PHYMOD_IF_ERR_RETURN
            (eagle_pram_firmware_enable(&core->access, 1));

        PHYMOD_IF_ERR_RETURN(fw_loader(core, qtce_ucode_len, qtce_ucode)); 

        PHYMOD_IF_ERR_RETURN
            (eagle_pram_firmware_enable(&core->access, 0));
        PHYMOD_IF_ERR_RETURN
            (qmod_pram_abl_enable_set(&core->access,0));
        break;
    case phymodFirmwareLoadMethodNone:
        break;
    default:
        PHYMOD_RETURN_WITH_ERR(PHYMOD_E_CONFIG, (_PHYMOD_MSG("illegal fw load method %u"), load_method));
    }
    return PHYMOD_E_NONE;
}

int qtce_phy_firmware_core_config_set(const phymod_phy_access_t* phy, phymod_firmware_core_config_t fw_config)
{
    struct eagle_tsc_uc_core_config_st serdes_firmware_core_config;
    PHYMOD_MEMSET(&serdes_firmware_core_config, 0, sizeof(serdes_firmware_core_config));
    serdes_firmware_core_config.field.core_cfg_from_pcs = fw_config.CoreConfigFromPCS;
    serdes_firmware_core_config.field.vco_rate = fw_config.VcoRate; 
    PHYMOD_IF_ERR_RETURN(eagle_tsc_set_uc_core_config(&phy->access, serdes_firmware_core_config));
    return PHYMOD_E_NONE;
}


int qtce_phy_firmware_core_config_get(const phymod_phy_access_t* phy, phymod_firmware_core_config_t* fw_config)
{
    struct eagle_tsc_uc_core_config_st serdes_firmware_core_config;
    PHYMOD_IF_ERR_RETURN(eagle_tsc_get_uc_core_config(&phy->access, &serdes_firmware_core_config));
    PHYMOD_MEMSET(fw_config, 0, sizeof(*fw_config));
    fw_config->CoreConfigFromPCS = serdes_firmware_core_config.field.core_cfg_from_pcs;
    fw_config->VcoRate = serdes_firmware_core_config.field.vco_rate;
    return PHYMOD_E_NONE; 
}


int qtce_phy_firmware_lane_config_set(const phymod_phy_access_t* phy, phymod_firmware_lane_config_t fw_config)
{
    phymod_phy_access_t phy_copy;
    int start_lane, num_lane, i, lane_id, sub_port;

    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(&phy->access, &start_lane, &num_lane));
    PHYMOD_IF_ERR_RETURN
        (qmod_lane_info(&phy->access, &lane_id, &sub_port));
    start_lane = lane_id;
    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));

    /*Hold the per lne soft reset bit*/
    for (i = 0; i < num_lane; i++) {
        phy_copy.access.lane_mask = 1 << (start_lane + i);
        PHYMOD_IF_ERR_RETURN
            (eagle_lane_soft_reset_release(&phy_copy.access, 0));
    }

    PHYMOD_USLEEP(500);
    for (i = 0; i < num_lane; i++) {
        phy_copy.access.lane_mask = 1 << (start_lane + i);
        PHYMOD_IF_ERR_RETURN
             (_qtce_phy_firmware_lane_config_set(&phy_copy, fw_config));
    }
    /*Hold the per lne soft reset bit*/
    for (i = 0; i < num_lane; i++) {
        phy_copy.access.lane_mask = 1 << (start_lane + i);
        PHYMOD_IF_ERR_RETURN
            (eagle_lane_soft_reset_release(&phy_copy.access, 1));
    }

    PHYMOD_IF_ERR_RETURN
        (qmod_trigger_speed_change(&phy->access));

    return PHYMOD_E_NONE;
}


int qtce_phy_firmware_lane_config_get(const phymod_phy_access_t* phy, phymod_firmware_lane_config_t* fw_config)
{
    struct eagle_tsc_uc_lane_config_st serdes_firmware_config;
    phymod_phy_access_t phy_copy;
    int lane_id, sub_port;

    PHYMOD_IF_ERR_RETURN
        (qmod_lane_info(&phy->access, &lane_id, &sub_port));
    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));
    phy_copy.access.lane_mask = 1 << lane_id;

    PHYMOD_IF_ERR_RETURN(eagle_tsc_get_uc_lane_cfg(&phy_copy.access, &serdes_firmware_config));
    PHYMOD_MEMSET(fw_config, 0, sizeof(*fw_config));
    fw_config->LaneConfigFromPCS = serdes_firmware_config.field.lane_cfg_from_pcs;
    fw_config->AnEnabled         = serdes_firmware_config.field.an_enabled;
    fw_config->DfeOn             = serdes_firmware_config.field.dfe_on;
    fw_config->ForceBrDfe        = serdes_firmware_config.field.force_brdfe_on;
    fw_config->Cl72AutoPolEn        = serdes_firmware_config.field.cl72_auto_polarity_en;
    fw_config->Cl72RestTO      = serdes_firmware_config.field.cl72_restart_timeout_en;
    fw_config->ScramblingDisable = serdes_firmware_config.field.scrambling_dis;
    fw_config->UnreliableLos     = serdes_firmware_config.field.unreliable_los;
    fw_config->MediaType         = serdes_firmware_config.field.media_type;
     
    return PHYMOD_E_NONE; 
}

/* reset pll sequencer
 * flags - unused parameter
 */
int qtce_core_pll_sequencer_restart(const phymod_core_access_t* core, uint32_t flags, phymod_sequencer_operation_t operation)
{
    switch (operation) {
        case phymodSeqOpStop:
            PHYMOD_IF_ERR_RETURN
                (qmod_pll_sequencer_control(&core->access, 0));
        break;
        case phymodSeqOpStart:
            PHYMOD_IF_ERR_RETURN
                (qmod_pll_sequencer_control(&core->access, 1));
            
            /* PHYMOD_IF_ERR_RETURN
                (qtce_pll_lock_wait(&core->access, 250000)); */
        break;
        case phymodSeqOpRestart:
            PHYMOD_IF_ERR_RETURN
                (qmod_pll_sequencer_control(&core->access, 0));
            
            PHYMOD_IF_ERR_RETURN
                (qmod_pll_sequencer_control(&core->access, 1));
            
            /* PHYMOD_IF_ERR_RETURN
                (qtce_pll_lock_wait(&core->access, 250000)); */
        break;
        default:
            return PHYMOD_E_UNAVAIL;
        break;
    }     
    return PHYMOD_E_NONE;
}

int qtce_core_wait_event(const phymod_core_access_t* core, phymod_core_event_t event, uint32_t timeout)
{
    switch(event){
    case phymodCoreEventPllLock: 
        /* PHYMOD_IF_ERR_RETURN(qtce_pll_lock_wait(&core->access, timeout)); */
        break;
    default:
        PHYMOD_RETURN_WITH_ERR(PHYMOD_E_CONFIG, (_PHYMOD_MSG("illegal wait event %u"), event));
    }
    return PHYMOD_E_NONE;
}

int qtce_phy_pll_multiplier_get(const phymod_phy_access_t* core, uint32_t* pll_multiplier)
{
    return PHYMOD_E_NONE;
}


/* reset rx sequencer 
 * flags - unused parameter
 */
int qtce_phy_rx_restart(const phymod_phy_access_t* phy)
{
    phymod_phy_access_t phy_copy;
    int lane_id, sub_port;

    PHYMOD_IF_ERR_RETURN
        (qmod_lane_info(&phy->access, &lane_id, &sub_port));
    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));
    phy_copy.access.lane_mask = 1 << lane_id;

    PHYMOD_IF_ERR_RETURN(eagle_tsc_rx_restart(&phy_copy.access, 1));
    return PHYMOD_E_NONE;
}


int qtce_phy_polarity_set(const phymod_phy_access_t* phy, const phymod_polarity_t* polarity)
{
    phymod_phy_access_t phy_copy;
    int lane_id, sub_port;

    PHYMOD_IF_ERR_RETURN
        (qmod_lane_info(&phy->access, &lane_id, &sub_port));
    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));
    phy_copy.access.lane_mask = 1 << lane_id;

    PHYMOD_IF_ERR_RETURN
        (qmod_tx_rx_polarity_set(&phy_copy.access, polarity->tx_polarity, polarity->rx_polarity));
    return PHYMOD_E_NONE;
}


int qtce_phy_polarity_get(const phymod_phy_access_t* phy, phymod_polarity_t* polarity)
{
    phymod_phy_access_t phy_copy;
    int lane_id, sub_port;

    PHYMOD_IF_ERR_RETURN
        (qmod_lane_info(&phy->access, &lane_id, &sub_port));
    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));
    phy_copy.access.lane_mask = 1 << lane_id;

    PHYMOD_IF_ERR_RETURN
        (qmod_tx_rx_polarity_get(&phy_copy.access, &polarity->tx_polarity, &polarity->rx_polarity));
    return PHYMOD_E_NONE;
}

int _qtce_phy_tx_set(phymod_phy_access_t* phy, const phymod_tx_t* tx)
{
    PHYMOD_IF_ERR_RETURN
        (eagle_tsc_write_tx_afe(&phy->access, TX_AFE_PRE, (int8_t)tx->pre));
    PHYMOD_IF_ERR_RETURN
        (eagle_tsc_write_tx_afe(&phy->access, TX_AFE_MAIN, (int8_t)tx->main));
    PHYMOD_IF_ERR_RETURN
        (eagle_tsc_write_tx_afe(&phy->access, TX_AFE_POST1, (int8_t)tx->post));
    PHYMOD_IF_ERR_RETURN
        (eagle_tsc_write_tx_afe(&phy->access, TX_AFE_POST2, (int8_t)tx->post2));
    PHYMOD_IF_ERR_RETURN
        (eagle_tsc_write_tx_afe(&phy->access, TX_AFE_POST3, (int8_t)tx->post3));
    PHYMOD_IF_ERR_RETURN
        (eagle_tsc_write_tx_afe(&phy->access, TX_AFE_AMP,  (int8_t)tx->amp));

    return PHYMOD_E_NONE;
}

int qtce_phy_tx_set(const phymod_phy_access_t* phy, const phymod_tx_t* tx)
{
    phymod_phy_access_t phy_copy;
    int lane_id, sub_port;

    PHYMOD_IF_ERR_RETURN
        (qmod_lane_info(&phy->access, &lane_id, &sub_port));
    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));
    phy_copy.access.lane_mask = 1 << lane_id;

    PHYMOD_IF_ERR_RETURN(_qtce_phy_tx_set(&phy_copy, tx));

    return PHYMOD_E_NONE;
}

int qtce_phy_tx_get(const phymod_phy_access_t* phy, phymod_tx_t* tx)
{
    phymod_phy_access_t phy_copy;
    int lane_id, sub_port;
    int8_t value = 0;

    PHYMOD_IF_ERR_RETURN
        (qmod_lane_info(&phy->access, &lane_id, &sub_port));
    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));
    phy_copy.access.lane_mask = 1 << lane_id;

    PHYMOD_IF_ERR_RETURN
        (eagle_tsc_read_tx_afe(&phy_copy.access, TX_AFE_PRE, &value));
    tx->pre = value;
    PHYMOD_IF_ERR_RETURN
        (eagle_tsc_read_tx_afe(&phy_copy.access, TX_AFE_MAIN, &value));
    tx->main = value;
    PHYMOD_IF_ERR_RETURN
        (eagle_tsc_read_tx_afe(&phy_copy.access, TX_AFE_POST1, &value));
    tx->post = value;
    PHYMOD_IF_ERR_RETURN
        (eagle_tsc_read_tx_afe(&phy_copy.access, TX_AFE_POST2, &value));
    tx->post2 = value;
    PHYMOD_IF_ERR_RETURN
        (eagle_tsc_read_tx_afe(&phy_copy.access, TX_AFE_POST3, &value));
    tx->post3 = value;
    PHYMOD_IF_ERR_RETURN
        (eagle_tsc_read_tx_afe(&phy_copy.access, TX_AFE_AMP, &value));
    tx->amp = value;

    return PHYMOD_E_NONE;
}

int qtce_phy_tx_override_set(const phymod_phy_access_t* phy, const phymod_tx_override_t* tx_override)
{
    phymod_phy_access_t phy_copy;
    int lane_id, sub_port;

    PHYMOD_IF_ERR_RETURN
        (qmod_lane_info(&phy->access, &lane_id, &sub_port));
    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));
    phy_copy.access.lane_mask = 1 << lane_id;

    PHYMOD_IF_ERR_RETURN
        (eagle_tsc_tx_pi_freq_override(&phy_copy.access,
                                    tx_override->phase_interpolator.enable,
                                    tx_override->phase_interpolator.value));    
    return PHYMOD_E_NONE;
}

int qtce_phy_tx_override_get(const phymod_phy_access_t* phy, phymod_tx_override_t* tx_override)
{
/*
    PHYMOD_IF_ERR_RETURN
        (qtce_tx_pi_control_get(&phy->access, &tx_override->phase_interpolator.value));
*/
    return PHYMOD_E_NONE;
}


int qtce_phy_rx_set(const phymod_phy_access_t* phy, const phymod_rx_t* rx)
{
    uint32_t i;
    phymod_phy_access_t phy_copy;
    int lane_id, sub_port;

    PHYMOD_IF_ERR_RETURN
        (qmod_lane_info(&phy->access, &lane_id, &sub_port));
    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));
    phy_copy.access.lane_mask = 1 << lane_id;

    /* params check */
    if((rx->num_of_dfe_taps == 0) || (rx->num_of_dfe_taps < QTCE_NOF_DFES)){
        PHYMOD_RETURN_WITH_ERR(PHYMOD_E_CONFIG, (_PHYMOD_MSG("illegal number of DFEs to set %u"), rx->num_of_dfe_taps));
    }

    /*vga set*/
    if (rx->vga.enable) {
        /* first stop the rx adaption */
        PHYMOD_IF_ERR_RETURN(eagle_tsc_stop_rx_adaptation(&phy_copy.access, 1));
        PHYMOD_IF_ERR_RETURN(eagle_tsc_write_rx_afe(&phy_copy.access, RX_AFE_VGA, rx->vga.value));
    } else {
        PHYMOD_IF_ERR_RETURN(eagle_tsc_stop_rx_adaptation(&phy_copy.access, 0));
    }
    
    /* dfe set */
    for (i = 0 ; i < rx->num_of_dfe_taps ; i++){
        if(rx->dfe[i].enable){
            PHYMOD_IF_ERR_RETURN(eagle_tsc_stop_rx_adaptation(&phy_copy.access, 1));
            switch (i) {
                case 0:
                    PHYMOD_IF_ERR_RETURN(eagle_tsc_write_rx_afe(&phy_copy.access, RX_AFE_DFE1, rx->dfe[i].value));
                    break;
                case 1:
                    PHYMOD_IF_ERR_RETURN(eagle_tsc_write_rx_afe(&phy_copy.access, RX_AFE_DFE2, rx->dfe[i].value));
                    break;
                case 2:
                    PHYMOD_IF_ERR_RETURN(eagle_tsc_write_rx_afe(&phy_copy.access, RX_AFE_DFE3, rx->dfe[i].value));
                    break;
                case 3:
                    PHYMOD_IF_ERR_RETURN(eagle_tsc_write_rx_afe(&phy_copy.access, RX_AFE_DFE4, rx->dfe[i].value));
                    break;
                case 4:
                    PHYMOD_IF_ERR_RETURN(eagle_tsc_write_rx_afe(&phy_copy.access, RX_AFE_DFE5, rx->dfe[i].value));
                    break;
                default:
                    return PHYMOD_E_PARAM;
            }
        } else {
            PHYMOD_IF_ERR_RETURN(eagle_tsc_stop_rx_adaptation(&phy_copy.access, 0));
        }
    }
     
    /* peaking filter set */
    if(rx->peaking_filter.enable){
        /* first stop the rx adaption */
        PHYMOD_IF_ERR_RETURN(eagle_tsc_stop_rx_adaptation(&phy_copy.access, 1));
        PHYMOD_IF_ERR_RETURN(eagle_tsc_write_rx_afe(&phy_copy.access, RX_AFE_PF, rx->peaking_filter.value));
    } else {
        PHYMOD_IF_ERR_RETURN(eagle_tsc_stop_rx_adaptation(&phy_copy.access, 0));
    }
    
    if(rx->low_freq_peaking_filter.enable){
        /* first stop the rx adaption */
        PHYMOD_IF_ERR_RETURN(eagle_tsc_stop_rx_adaptation(&phy_copy.access, 1));
        PHYMOD_IF_ERR_RETURN(eagle_tsc_write_rx_afe(&phy_copy.access, RX_AFE_PF2, rx->low_freq_peaking_filter.value));
    } else {
        PHYMOD_IF_ERR_RETURN(eagle_tsc_stop_rx_adaptation(&phy_copy.access, 0));
    }
    return PHYMOD_E_NONE;
}


int qtce_phy_rx_get(const phymod_phy_access_t* phy, phymod_rx_t* rx)
{
    phymod_phy_access_t phy_copy;
    int lane_id, sub_port;
    int8_t tmpData;

    PHYMOD_IF_ERR_RETURN
        (qmod_lane_info(&phy->access, &lane_id, &sub_port));
    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));
    phy_copy.access.lane_mask = 1 << lane_id;

    PHYMOD_IF_ERR_RETURN
        (eagle_tsc_read_rx_afe(&phy_copy.access, RX_AFE_PF, &tmpData));
    rx->peaking_filter.value = tmpData;
    PHYMOD_IF_ERR_RETURN
        (eagle_tsc_read_rx_afe(&phy_copy.access, RX_AFE_PF2, &tmpData));
    rx->low_freq_peaking_filter.value = tmpData;
    PHYMOD_IF_ERR_RETURN
        (eagle_tsc_read_rx_afe(&phy_copy.access, RX_AFE_VGA, &tmpData));
    rx->vga.value = tmpData;
    PHYMOD_IF_ERR_RETURN
        (eagle_tsc_read_rx_afe(&phy_copy.access, RX_AFE_DFE1, &tmpData));
    rx->dfe[0].value = tmpData;
    PHYMOD_IF_ERR_RETURN
        (eagle_tsc_read_rx_afe(&phy_copy.access, RX_AFE_DFE2, &tmpData));
    rx->dfe[1].value = tmpData;
    PHYMOD_IF_ERR_RETURN
        (eagle_tsc_read_rx_afe(&phy_copy.access, RX_AFE_DFE3, &tmpData));
    rx->dfe[2].value = tmpData;
    PHYMOD_IF_ERR_RETURN
        (eagle_tsc_read_rx_afe(&phy_copy.access, RX_AFE_DFE4, &tmpData));
    rx->dfe[3].value = tmpData;
    PHYMOD_IF_ERR_RETURN
        (eagle_tsc_read_rx_afe(&phy_copy.access, RX_AFE_DFE5, &tmpData));
    rx->dfe[4].value = tmpData;

    rx->num_of_dfe_taps = 5;
    rx->dfe[0].enable = 1;
    rx->dfe[1].enable = 1;
    rx->dfe[2].enable = 1;
    rx->dfe[3].enable = 1;
    rx->dfe[4].enable = 1;
    rx->vga.enable = 1;
    rx->low_freq_peaking_filter.enable = 1;
    rx->peaking_filter.enable = 1;
    return PHYMOD_E_NONE;
}


int qtce_phy_reset_set(const phymod_phy_access_t* phy, const phymod_phy_reset_t* reset)
{
        
        
    
    /* Place your code here */

        
    return PHYMOD_E_NONE;
        
    
}


int qtce_phy_reset_get(const phymod_phy_access_t* phy, phymod_phy_reset_t* reset)
{
        
        
    
    /* Place your code here */

        
    return PHYMOD_E_NONE;
        
    
}


int qtce_phy_power_set(const phymod_phy_access_t* phy, const phymod_phy_power_t* power)
{
    phymod_phy_access_t pm_phy_copy;
    int start_lane, num_lane, lane_id, sub_port;
    
    PHYMOD_MEMCPY(&pm_phy_copy, phy, sizeof(pm_phy_copy));
    /* next program the tx fir taps and driver current based on the input */
    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(&phy->access, &start_lane, &num_lane));

    PHYMOD_IF_ERR_RETURN
        (qmod_lane_info(&phy->access, &lane_id, &sub_port));

    start_lane = lane_id ;
    pm_phy_copy.access.lane_mask = 0x1 << start_lane;


    if ((power->tx == phymodPowerOff) && (power->rx == phymodPowerOff)) {        
        PHYMOD_IF_ERR_RETURN(qmod_port_enable_set(&pm_phy_copy.access, 0));
    }
    if ((power->tx == phymodPowerOn) && (power->rx == phymodPowerOn)) {
        PHYMOD_IF_ERR_RETURN(qmod_port_enable_set(&pm_phy_copy.access, 1));
    }

    if ((power->tx == phymodPowerOff) && (power->rx == phymodPowerNoChange)) {
        /* disable tx on the PMD side */
        PHYMOD_IF_ERR_RETURN(eagle_tsc_tx_disable(&pm_phy_copy.access, 1));
    }
    if ((power->tx == phymodPowerOn) && (power->rx == phymodPowerNoChange)) {
        /* enable tx on the PMD side */
        PHYMOD_IF_ERR_RETURN(eagle_tsc_tx_disable(&pm_phy_copy.access, 0));
    }
    if ((power->tx == phymodPowerNoChange) && (power->rx == phymodPowerOff)) {
        /* disable rx on the PMD side */
        PHYMOD_IF_ERR_RETURN(qmod_rx_squelch_set(&pm_phy_copy.access, 1));
    }
    if ((power->tx == phymodPowerNoChange) && (power->rx == phymodPowerOn)) {
        /* enable rx on the PMD side */
        PHYMOD_IF_ERR_RETURN(qmod_rx_squelch_set(&pm_phy_copy.access, 0));
    }
    return PHYMOD_E_NONE; 
}

int qtce_phy_power_get(const phymod_phy_access_t* phy, phymod_phy_power_t* power)
{
    int enable;
    uint32_t lb_enable;
    phymod_phy_access_t pm_phy_copy;
    int start_lane, num_lane, lane_id, sub_port;

    PHYMOD_MEMCPY(&pm_phy_copy, phy, sizeof(pm_phy_copy));
    /* next program the tx fir taps and driver current based on the input */
    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(&phy->access, &start_lane, &num_lane));
    
    PHYMOD_IF_ERR_RETURN
        (qmod_lane_info(&phy->access, &lane_id, &sub_port));
    
    start_lane = lane_id ;
    pm_phy_copy.access.lane_mask = 0x1 << start_lane;

    PHYMOD_IF_ERR_RETURN(qmod_rx_squelch_get(&pm_phy_copy.access, &enable));

    /* next check if PMD loopback is on */ 
    if (enable) {                           
        PHYMOD_IF_ERR_RETURN(eagle_pmd_loopback_get(&pm_phy_copy.access, &lb_enable));
        if (lb_enable) enable = 0;
    }

    power->rx = (enable == 1)? phymodPowerOff: phymodPowerOn;
    /* Commented the following line. Because if in PMD loopback mode, we squelch the
           xmit, and we should still see the correct port status */
    /* PHYMOD_IF_ERR_RETURN(qtce_tx_squelch_get(&pm_phy_copy.access, &enable)); */
    power->tx = (enable == 1)? phymodPowerOff: phymodPowerOn;
    return PHYMOD_E_NONE;
}

int qtce_phy_tx_lane_control_set(const phymod_phy_access_t* phy, phymod_phy_tx_lane_control_t tx_control)
{   
    phymod_phy_access_t pm_phy_copy;
    int start_lane, num_lane, lane_id, sub_port;
    int qmode = 0, subports = 0, sq_value = 0;

    PHYMOD_MEMCPY(&pm_phy_copy, phy, sizeof(pm_phy_copy));
    /* next program the tx fir taps and driver current based on the input */
    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(&phy->access, &start_lane, &num_lane));

    PHYMOD_IF_ERR_RETURN
        (qmod_lane_info(&phy->access, &lane_id, &sub_port));

    start_lane = lane_id ;  
    pm_phy_copy.access.lane_mask = 0x1 << start_lane;

    if (PHYMOD_ACC_F_QMODE_GET(&phy->access)) {
        qmode = 1;
        PHYMOD_IF_ERR_RETURN(qmod_port_state_get(&pm_phy_copy.access, QMOD_PORT_STATE_CONFIGED, &subports));
    }

    switch (tx_control) {
    case phymodTxTrafficDisable:
        PHYMOD_IF_ERR_RETURN(qmod_tx_lane_control_set(&pm_phy_copy.access, QMOD_TX_LANE_TRAFFIC_DISABLE));
        break;
    case phymodTxTrafficEnable:
        PHYMOD_IF_ERR_RETURN(qmod_tx_lane_control_set(&pm_phy_copy.access, QMOD_TX_LANE_TRAFFIC_ENABLE));
        break;
    case phymodTxReset:
        PHYMOD_IF_ERR_RETURN(qmod_tx_lane_control_set(&pm_phy_copy.access, QMOD_TX_LANE_RESET));
        break;
    case phymodTxSquelchOn:
        PHYMOD_IF_ERR_RETURN(qmod_port_state_set(&pm_phy_copy.access, QMOD_PORT_STATE_TX_SQUELCH, sub_port, 1));
        PHYMOD_IF_ERR_RETURN(qmod_port_state_get(&pm_phy_copy.access, QMOD_PORT_STATE_TX_SQUELCH, &sq_value));
        if ((qmode == 0) || (sq_value == subports)) { 
            /* not qmode or all subports want to squelch*/
            PHYMOD_IF_ERR_RETURN(qmod_tx_squelch_set(&pm_phy_copy.access, 1));
        }
        break;
    case phymodTxSquelchOff:
        PHYMOD_IF_ERR_RETURN(qmod_port_state_set(&pm_phy_copy.access, QMOD_PORT_STATE_TX_SQUELCH, sub_port, 0));
        PHYMOD_IF_ERR_RETURN(qmod_tx_squelch_set(&pm_phy_copy.access, 0));
        break;
    default:
        break;
    }
    return PHYMOD_E_NONE;
}

int qtce_phy_tx_lane_control_get(const phymod_phy_access_t* phy, phymod_phy_tx_lane_control_t* tx_control)
{   
    int enable, reset, tx_lane;
    uint32_t lb_enable;
    phymod_phy_access_t pm_phy_copy;
    int start_lane, num_lane, lane_id, sub_port;
    int sq_value;

    PHYMOD_MEMCPY(&pm_phy_copy, phy, sizeof(pm_phy_copy));
    /* next program the tx fir taps and driver current based on the input */
    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(&phy->access, &start_lane, &num_lane));
    
    PHYMOD_IF_ERR_RETURN
        (qmod_lane_info(&phy->access, &lane_id, &sub_port));
    start_lane = lane_id ;
    pm_phy_copy.access.lane_mask = 0x1 << start_lane;

    PHYMOD_IF_ERR_RETURN(qmod_port_state_get(&pm_phy_copy.access, QMOD_PORT_STATE_TX_SQUELCH, &sq_value));
    enable = sq_value & (1 << sub_port);

    /* next check if PMD loopback is on */ 
    if (enable) {                           
        PHYMOD_IF_ERR_RETURN(eagle_pmd_loopback_get(&pm_phy_copy.access, &lb_enable));
        if (lb_enable) enable = 0;
    }

    if(enable) {
        *tx_control = phymodTxSquelchOn;
    } else {
        PHYMOD_IF_ERR_RETURN(qmod_tx_lane_control_get(&pm_phy_copy.access, &reset, &tx_lane));
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


int qtce_phy_rx_lane_control_set(const phymod_phy_access_t* phy, phymod_phy_rx_lane_control_t rx_control)
{   
    phymod_phy_access_t pm_phy_copy;
    int start_lane, num_lane, lane_id, sub_port;
    int qmode = 0, subports = 0, sq_value = 0;

    PHYMOD_MEMCPY(&pm_phy_copy, phy, sizeof(pm_phy_copy));
    /* next program the tx fir taps and driver current based on the input */
    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(&phy->access, &start_lane, &num_lane));

    PHYMOD_IF_ERR_RETURN
        (qmod_lane_info(&phy->access, &lane_id, &sub_port));

    start_lane = lane_id ;  
    pm_phy_copy.access.lane_mask = 0x1 << start_lane;

    if (PHYMOD_ACC_F_QMODE_GET(&phy->access)) {
        qmode = 1;
        PHYMOD_IF_ERR_RETURN(qmod_port_state_get(&pm_phy_copy.access, QMOD_PORT_STATE_CONFIGED, &subports));
    }
    switch (rx_control) {
    case phymodRxReset:
        PHYMOD_IF_ERR_RETURN(qmod_rx_lane_control_set(&pm_phy_copy.access, 0));
        break;
    case phymodRxSquelchOn:
        PHYMOD_IF_ERR_RETURN(qmod_port_state_set(&pm_phy_copy.access, QMOD_PORT_STATE_RX_SQUELCH, sub_port, 1));
        PHYMOD_IF_ERR_RETURN(qmod_port_state_get(&pm_phy_copy.access, QMOD_PORT_STATE_RX_SQUELCH, &sq_value));
        if ((qmode == 0) || (sq_value == subports)) {  
            /* not qmode or all subports want to squelch */
            PHYMOD_IF_ERR_RETURN(qmod_rx_squelch_set(&pm_phy_copy.access, 1));    
        }
        break;
    case phymodRxSquelchOff:
        PHYMOD_IF_ERR_RETURN(qmod_port_state_set(&pm_phy_copy.access, QMOD_PORT_STATE_RX_SQUELCH, sub_port, 0));
        PHYMOD_IF_ERR_RETURN(qmod_rx_squelch_set(&pm_phy_copy.access, 0));
        break;
    default:
        break;
    }
    return PHYMOD_E_NONE;
}

int qtce_phy_rx_lane_control_get(const phymod_phy_access_t* phy, phymod_phy_rx_lane_control_t* rx_control)
{   
    int enable, reset;
    uint32_t lb_enable;
    phymod_phy_access_t pm_phy_copy;
    int start_lane, num_lane, lane_id, sub_port;
    int sq_value;

    PHYMOD_MEMCPY(&pm_phy_copy, phy, sizeof(pm_phy_copy));
    /* next program the tx fir taps and driver current based on the input */
    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(&phy->access, &start_lane, &num_lane));

    PHYMOD_IF_ERR_RETURN
        (qmod_lane_info(&phy->access, &lane_id, &sub_port));

    start_lane = lane_id ;
    pm_phy_copy.access.lane_mask = 0x1 << start_lane;

    PHYMOD_IF_ERR_RETURN(qmod_port_state_get(&pm_phy_copy.access, QMOD_PORT_STATE_RX_SQUELCH, &sq_value));
    enable = sq_value & (1 << sub_port);

    /* next check if PMD loopback is on */ 
    if (enable) {                           
        PHYMOD_IF_ERR_RETURN(eagle_pmd_loopback_get(&pm_phy_copy.access, &lb_enable));
        if (lb_enable) enable = 0;
    }
    if(enable) {
        *rx_control = phymodRxSquelchOn;
    } else {
        PHYMOD_IF_ERR_RETURN(qmod_rx_lane_control_get(&pm_phy_copy.access, &reset));
        if (reset == 0) {
            *rx_control = phymodRxReset;
        } else {
            *rx_control = phymodRxSquelchOff;
        }
    }
    return PHYMOD_E_NONE;
}

int qtce_phy_tx_disable_set(const phymod_phy_access_t* phy, uint32_t tx_disable)
{       
    return PHYMOD_E_NONE;
}


int qtce_phy_tx_disable_get(const phymod_phy_access_t* phy, uint32_t* tx_disable)
{
    return PHYMOD_E_NONE;
}

int qtce_phy_fec_enable_set(const phymod_phy_access_t* phy, uint32_t enable)
{
    return PHYMOD_E_UNAVAIL;

}

int qtce_phy_fec_enable_get(const phymod_phy_access_t* phy, uint32_t* enable)
{
    return PHYMOD_E_UNAVAIL;

}

int _qtce_pll_multiplier_get(uint32_t pll_div, uint32_t *pll_multiplier)
{
    switch (pll_div) {
    case 0x0:
        *pll_multiplier = 46;
        break;
    case 0x1:
        *pll_multiplier = 72;
        break;
    case QMOD_PLL_MODE_DIV_40:
        *pll_multiplier = 40;
        break;
    case QMOD_PLL_MODE_DIV_42:
        *pll_multiplier = 42;
        break;
    case QMOD_PLL_MODE_DIV_48:
        *pll_multiplier = 48;
        break;
    case 0x5:
        *pll_multiplier = 50;
        break;
    case QMOD_PLL_MODE_DIV_52:
        *pll_multiplier = 52;
        break;
    case QMOD_PLL_MODE_DIV_54:
        *pll_multiplier = 54;
        break;
    case QMOD_PLL_MODE_DIV_60:
        *pll_multiplier = 60;
        break;
    case QMOD_PLL_MODE_DIV_64:
        *pll_multiplier = 64;
        break;
    case QMOD_PLL_MODE_DIV_66:
        *pll_multiplier = 66;
        break;
    case 0xb:
        *pll_multiplier = 68;
        break;
    case QMOD_PLL_MODE_DIV_70:
        *pll_multiplier = 70;
        break;
    case QMOD_PLL_MODE_DIV_80:
        *pll_multiplier = 80;
        break;
    case QMOD_PLL_MODE_DIV_92:
        *pll_multiplier = 92;
        break;
    case 0xf:
        *pll_multiplier = 100;
        break;
    default:
        *pll_multiplier = 66;
        break;
    }
    return PHYMOD_E_NONE;
}

/* to modify the sub-port speed */
STATIC
int _qtce_qsgmii_interface_config_set(const phymod_phy_access_t* phy, uint32_t flags, const phymod_phy_inf_config_t* config)
{
    qmod_spd_intfc_type spd_intf = QMOD_SPD_ILLEGAL, old_spd_intf;
    qmod_spd_intfc_type qmod_spd_intf = QMOD_SPD_ILLEGAL;
    phymod_phy_access_t pm_phy_copy;
    int      start_lane, lane_id, sub_port, num_lane ;

    PHYMOD_MEMCPY(&pm_phy_copy, phy, sizeof(pm_phy_copy));

    /* next program the tx fir taps and driver current based on the input */
    /* get num_lane only in QTC */
    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(&phy->access, &start_lane, &num_lane));

    PHYMOD_IF_ERR_RETURN
        (qmod_lane_info(&phy->access, &lane_id, &sub_port));

    start_lane = lane_id ;  
    pm_phy_copy.access.lane_mask = 0x1 << start_lane;

    spd_intf = QMOD_SPD_1000_SGMII; /* to prevent undefinded QMOD_SPD_ILLEGAL accessing tables */

    switch(config->data_rate) {
    case 10:
        if(config->pll_divider_req==40) {
            spd_intf = QMOD_SPD_10_SGMII;
        } else {
           /* spd_intf = QMOD_SPD_10_X1_SGMII; */
            spd_intf = QMOD_SPD_10_SGMII;
        }
        break;
    case 100:
        if(config->pll_divider_req==40) {
            spd_intf = QMOD_SPD_100_SGMII;
        } else {
            /*spd_intf = QMOD_SPD_100_X1_SGMII; */
            spd_intf = QMOD_SPD_100_SGMII;
        }
        break;
    case 1000:
        if(config->pll_divider_req==40) {
            spd_intf = QMOD_SPD_1000_SGMII;
        } else {
            /*spd_intf = QMOD_SPD_1000_X1_SGMII; */
            spd_intf = QMOD_SPD_1000_SGMII;
        }
        break;
    default:
        spd_intf = QMOD_SPD_ILLEGAL;
        break;
    }

    PHYMOD_IF_ERR_RETURN
        (qmod_get_qport_spd(&pm_phy_copy.access, sub_port, &old_spd_intf));
    
    qmod_spd_intf = spd_intf;
    spd_intf = QMOD_SPD_4000;

    if (old_spd_intf != QMOD_SPD_1000_SGMII) {
        PHYMOD_IF_ERR_RETURN
            (qmod_set_qport_spd(&pm_phy_copy.access, sub_port, QMOD_SPD_1000_SGMII));
    }
    
    if (qmod_spd_intf != QMOD_SPD_1000_SGMII) {
        PHYMOD_USLEEP(200);
        PHYMOD_IF_ERR_RETURN
            (qmod_set_qport_spd(&pm_phy_copy.access, sub_port, qmod_spd_intf));
    }

    return PHYMOD_E_NONE;

}

int qtce_phy_interface_config_set(const phymod_phy_access_t* phy, uint32_t flags, const phymod_phy_inf_config_t* config)
{  
    uint32_t current_pll_div=0;
    uint32_t new_pll_div=0;
    uint16_t new_speed_vec=0;
    qmod_spd_intfc_type spd_intf = QMOD_SPD_ILLEGAL;
    qmod_spd_intfc_type qmod_spd_intf = QMOD_SPD_ILLEGAL;
    phymod_phy_access_t pm_phy_copy;
    int start_lane, num_lane, i, pll_switch = 0;
    uint32_t u_os_mode = 0;
    int lane_bkup; 
    uint32_t pll_multiplier, vco_rate;
    int      lane_id, sub_port ;
    uint32_t sc_enable = 0;

    /* sc_table_entry exp_entry; RAVI */
    phymod_firmware_lane_config_t firmware_lane_config;  
    phymod_firmware_core_config_t firmware_core_config;

    PHYMOD_MEMCPY(&pm_phy_copy, phy, sizeof(pm_phy_copy));

    firmware_lane_config.MediaType = 0;

    /* next program the tx fir taps and driver current based on the input */
    /* get num_lane only in QTC */
    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(&phy->access, &start_lane, &num_lane));

    PHYMOD_IF_ERR_RETURN
        (qmod_lane_info(&phy->access, &lane_id, &sub_port));

    start_lane = lane_id ;  
    pm_phy_copy.access.lane_mask = 0x1 << start_lane;
    
    /* special call path for QSGMII FS mode for port not repeated calls on the same lane */
    if (PHYMOD_ACC_F_QMODE_GET(&phy->access)) {
        qmod_speedchange_get(&pm_phy_copy.access, &sc_enable) ;
        if (sc_enable) {
            return _qtce_qsgmii_interface_config_set(phy, flags, config);
        }
    }

    PHYMOD_IF_ERR_RETURN
        (qmod_reset(&pm_phy_copy.access)); 
      
    /* Hold the per lane soft reset bit */
    for (i = 0; i < num_lane; i++) {
        pm_phy_copy.access.lane_mask = 1 << (start_lane + i);
        PHYMOD_IF_ERR_RETURN
            (eagle_lane_soft_reset_release(&pm_phy_copy.access, 0));
    }

    PHYMOD_USLEEP(500);
    pm_phy_copy.access.lane_mask = 0x1 << start_lane;
     PHYMOD_IF_ERR_RETURN
        (qtce_phy_firmware_lane_config_get(phy, &firmware_lane_config));

    /* make sure that an and config from pcs is off */
    firmware_core_config.CoreConfigFromPCS = 0;
    firmware_lane_config.AnEnabled = 0;
    firmware_lane_config.LaneConfigFromPCS = 0;
    if (PHYMOD_INTF_MODES_FIBER_GET(config)) {
        firmware_lane_config.MediaType = phymodFirmwareMediaTypeOptics;
    }     

    PHYMOD_IF_ERR_RETURN
        (qmod_update_port_mode(&phy->access, (int *) &pll_switch));

    spd_intf = QMOD_SPD_1000_SGMII; /* to prevent undefinded QMOD_SPD_ILLEGAL accessing tables */

    /* find the speed */
    if (config->interface_type == phymodInterfaceXFI) {
        firmware_lane_config.MediaType = phymodFirmwareMediaTypePcbTraceBackPlane;
    } else if(config->interface_type == phymodInterfaceSFI) {
        firmware_lane_config.MediaType = phymodFirmwareMediaTypeOptics;
    }
    switch(config->data_rate) {
    case 10:
        if(config->pll_divider_req==40) {
            spd_intf = QMOD_SPD_10_SGMII;
        } else {
           /* spd_intf = QMOD_SPD_10_X1_SGMII; */
            spd_intf = QMOD_SPD_10_SGMII;
        }
        break;
    case 100:
        if(config->pll_divider_req==40) {
            spd_intf = QMOD_SPD_100_SGMII;
        } else {
            /*spd_intf = QMOD_SPD_100_X1_SGMII; */
            spd_intf = QMOD_SPD_100_SGMII;
        }
        break;
    case 1000:
        if(config->pll_divider_req==40) {
            spd_intf = QMOD_SPD_1000_SGMII;
        } else {
            /*spd_intf = QMOD_SPD_1000_X1_SGMII; */
            spd_intf = QMOD_SPD_1000_SGMII;
        }
        break;
    case 2500:
        if(config->pll_divider_req==40) {
            spd_intf = QMOD_SPD_2500;
        } else {
            /*spd_intf = QMOD_SPD_2500_X1; */
            spd_intf = QMOD_SPD_2500;
        }
        break;
    case 5000:
    case 10500:
    case 11000:
    case 12000:
    case 13000:
    case 15750:
    case 16000:
    case 20000:
    case 21000:
    case 3000:
    case 35000:
    case 40000:
    case 42000:
    case 100000:
    case 106000:
    case 107000:
    case 120000:
    case 127000:
        spd_intf = QMOD_SPD_ILLEGAL;
        break;
    default:
        break;
    }

    if(config->data_rate==10||config->data_rate==100|| config->data_rate==1000) {
      if (config->interface_type == phymodInterfaceSGMII) {
        firmware_lane_config.MediaType =phymodFirmwareMediaTypePcbTraceBackPlane;
      }
      if (config->interface_type == phymodInterface1000X) {
        firmware_lane_config.MediaType = phymodFirmwareMediaTypeOptics;
      }
    }

    PHYMOD_IF_ERR_RETURN
        (qmod_get_plldiv(&phy->access, &current_pll_div));

    /* next check if we are in qsgmii mode */
    if (PHYMOD_ACC_F_QMODE_GET(&phy->access)) {
        qmod_spd_intf = spd_intf;
        spd_intf = QMOD_SPD_4000;

        PHYMOD_IF_ERR_RETURN
            (qmod_set_qport_spd(&pm_phy_copy.access, sub_port, QMOD_SPD_1000_SGMII));

        if (qmod_spd_intf != QMOD_SPD_1000_SGMII) {
            PHYMOD_USLEEP(200);
            PHYMOD_IF_ERR_RETURN
                (qmod_set_qport_spd(&pm_phy_copy.access, sub_port, qmod_spd_intf));
        }
    }          
    
    PHYMOD_IF_ERR_RETURN
        (qmod_plldiv_lkup_get(&pm_phy_copy.access, spd_intf, &new_pll_div, &new_speed_vec));

    
    PHYMOD_IF_ERR_RETURN
        (qmod_pmd_osmode_set(&pm_phy_copy.access, spd_intf, u_os_mode));

    /* if pll change is enabled. new_pll_div is the reg vector value */
    if((current_pll_div != new_pll_div) && (PHYMOD_INTF_F_DONT_TURN_OFF_PLL & flags)){
        PHYMOD_RETURN_WITH_ERR(PHYMOD_E_CONFIG, 
                               (_PHYMOD_MSG("pll has to change for speed_set from %u to %u but DONT_TURN_OFF_PLL flag is enabled"),
                                 current_pll_div, new_pll_div));
    }
    /* pll switch is required and expected */
    if((current_pll_div != new_pll_div) && !(PHYMOD_INTF_F_DONT_TURN_OFF_PLL & flags)) {
        /* phymod_access_t tmp_phy_access; */
    
        /* Change in PLL, so reset all the ports first. Check for TSC12 */
        lane_bkup = pm_phy_copy.access.lane_mask;
        pm_phy_copy.access.lane_mask = 0xf;
        qmod_disable_set(&pm_phy_copy.access);
        pm_phy_copy.access.lane_mask = lane_bkup;

        /* tmp_phy_access = phy->access; */

        PHYMOD_IF_ERR_RETURN
            (eagle_core_soft_reset_release(&pm_phy_copy.access, 0));

        /* release the uc reset */
        PHYMOD_IF_ERR_RETURN
            (eagle_uc_reset(&pm_phy_copy.access ,1)); 

        /* set the PLL divider */
        PHYMOD_IF_ERR_RETURN
            (eagle_pll_mode_set(&pm_phy_copy.access, new_pll_div));
        PHYMOD_IF_ERR_RETURN
            (_qtce_pll_multiplier_get(new_pll_div, &pll_multiplier));
        /* update the VCO rate properly */
        switch (config->ref_clock) {
            case phymodRefClk156Mhz:
                vco_rate = pll_multiplier * 156 + pll_multiplier * 25 / 100;
                break;
            case phymodRefClk125Mhz:
                vco_rate = pll_multiplier * 125;
                break;
            default:
                vco_rate = pll_multiplier * 156 + pll_multiplier * 25 / 100;
                break;
        }
        firmware_core_config.VcoRate = (vco_rate - 5750) / 250 + 1;                 

        /* change the  master port num to the current caller port */
        PHYMOD_IF_ERR_RETURN
            (qmod_master_port_num_set(&pm_phy_copy.access, start_lane));

        PHYMOD_IF_ERR_RETURN
            (qmod_pll_reset_enable_set(&pm_phy_copy.access, 1));

        /* update the firmware config properly */
        PHYMOD_IF_ERR_RETURN
            (qtce_phy_firmware_core_config_set(&pm_phy_copy, firmware_core_config));
        PHYMOD_IF_ERR_RETURN
            (eagle_core_soft_reset_release(&pm_phy_copy.access, 1));
    }


    for (i = 0; i < num_lane; i++) {
        pm_phy_copy.access.lane_mask = 0x1 << (start_lane + i);
        PHYMOD_IF_ERR_RETURN
             (_qtce_phy_firmware_lane_config_set(&pm_phy_copy, firmware_lane_config));
    }
    
    /* release the per lne soft reset bit */
    for (i = 0; i < num_lane; i++) {
        pm_phy_copy.access.lane_mask = 1 << (start_lane + i);
        PHYMOD_IF_ERR_RETURN
            (eagle_lane_soft_reset_release(&pm_phy_copy.access, 1));
    }

    if (config->interface_type == phymodInterfaceBypass) {
      return PHYMOD_E_FAIL;
    } else {
     /*  if(flags & PHYMOD_INTF_F_SET_SPD_NO_TRIGGER) {
          (qmod_set_spd_intf(&pm_phy_copy.access, spd_intf, 1));
      } else { */
      PHYMOD_IF_ERR_RETURN
          (qmod_set_spd_intf(&pm_phy_copy.access, spd_intf, 0));
      /* } */
    }
    PHYMOD_USLEEP(10000);
    /* TX Params are set outside this func */
    /* change TX parameters if enabled */
    /* if((PHYMOD_IF_F_DONT_OVERIDE_TX_PARAMS & flags) == 0)
       PHYMOD_IF_ERR_RETURN
       (qtce_phy_media_type_tx_get(phy, phymodMediaTypeMid, &tx_params));
       PHYMOD_IF_ERR_RETURN
       (qtce_phy_tx_set(phy, &tx_params));
    */



    return PHYMOD_E_NONE;
}

int _qtce_speed_id_interface_config_get(const phymod_phy_access_t* phy, int speed_id, 
                                        phymod_phy_inf_config_t* config, uint16_t an_enable, 
                                        phymod_firmware_lane_config_t *lane_config)
{
    int ilkn_set;
    int osr_mode;
    uint32_t vco_rate;
    uint32_t pll_multiplier;
    uint32_t current_pll_div=0;
    int u_os_mode = 0xff;
    uint32_t actual_osr, actual_osr_rem;
    phymod_osr_mode_t osr_mode_enum;

    PHYMOD_IF_ERR_RETURN
        (qmod_get_plldiv(&phy->access, &current_pll_div));

    ilkn_set = 0;
    /* coverity[dead_error_condition] */
    if(ilkn_set) {
        /* coverity[dead_error_begin] */
        config->interface_type = phymodInterfaceBypass;
        PHYMOD_IF_ERR_RETURN
            (_qtce_pll_multiplier_get(current_pll_div, &pll_multiplier));
        PHYMOD_IF_ERR_RETURN
            (eagle_osr_mode_get(&phy->access, &osr_mode));

        switch (config->ref_clock) {
            case phymodRefClk156Mhz:
                vco_rate = pll_multiplier * 156 + pll_multiplier * 25 / 100;
                break;
            case phymodRefClk125Mhz:
                vco_rate = pll_multiplier * 125;
                break;
            default:
                vco_rate = pll_multiplier * 156 + pll_multiplier * 25 / 100;
                break;
        }

        PHYMOD_IF_ERR_RETURN(eagle_osr_mode_to_enum(osr_mode, &osr_mode_enum));
        PHYMOD_IF_ERR_RETURN(phymod_osr_mode_to_actual_os(osr_mode_enum, &actual_osr, &actual_osr_rem));
        config->data_rate = vco_rate/actual_osr;
    } else {

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
          config->data_rate = 2500;
          if (lane_config->MediaType == phymodFirmwareMediaTypeOptics) {
              config->interface_type = phymodInterface1000X;
          } else {
              config->interface_type = phymodInterfaceSGMII;
          }
          break;
      case 0x7:
          config->data_rate = 5000;
          config->interface_type = phymodInterfaceSR;
          break;
      case 0x8:
          config->data_rate = 10000;
          config->interface_type = phymodInterfaceCX4;
          break;
      case 0x9:
          config->data_rate = 10000;
          config->interface_type = phymodInterfaceKX4;
          break;
      case 0xa:
      case 0xb:
      case 0xc:
      case 0xd:
      case 0xe:
      case 0xf:
          PHYMOD_IF_ERR_RETURN
             (qmod_pmd_osmode_get(&phy->access,  &u_os_mode));
          if(u_os_mode == QMOD_PMA_OS_MODE_2) {
            config->data_rate = 5000;
            config->interface_type = phymodInterfaceRXAUI;
          } else {
            config->data_rate = 10000;
            config->interface_type = phymodInterfaceCR2;
          }
          break;
      case 0x31:
          config->data_rate = 5000;
          config->interface_type = phymodInterfaceKR;
          break;
      case 0x32:
      case 0x35:
          config->data_rate = 10;
          config->interface_type = phymodInterfaceSGMII;
          break;
      case 0x36:
          config->data_rate = 100;
          config->interface_type = phymodInterfaceSGMII;
          break;
      case 0x37:
          config->data_rate = 1000;
          config->interface_type = phymodInterfaceSGMII;
          break;
/* this is qsgmii mode */
      case 0x3c:
          config->data_rate = 1000;
          config->interface_type = phymodInterfaceQSGMII;
          break;
      case 0x38:
      case 0x10:
      case 0x11:
      case 0x12:
      case 0x13:
      case 0x14:
      case 0x15:
      case 0x16:
      case 0x17:
      case 0x18:
      case 0x19:
      case 0x1a:
      case 0x1b:    /* digital_operationSpeeds_SPEED_40G_X4; BRCM */
      case 0x1c:
      case 0x1d:
      case 0x1e:
      case 0x1f:
      case 0x20:
      case 0x21:  /* digital_operationSpeeds_SPEED_40G_KR4 */
      case 0x22:
      case 0x23:  /* digital_operationSpeeds_SPEED_42G_X4; atcually MLD */
      case 0x24:
      case 0x25:
      case 0x26:
      case 0x27:
      default:
          config->data_rate = 0;
          config->interface_type = phymodInterfaceSGMII;
          break;
      }
    }
    return PHYMOD_E_NONE;
}


/* flags- unused parameter */
int qtce_phy_interface_config_get(const phymod_phy_access_t* phy, uint32_t flags, phymod_ref_clk_t ref_clock, phymod_phy_inf_config_t* config)
{
    int speed_id;
    phymod_firmware_lane_config_t firmware_lane_config;  
    phymod_phy_access_t pm_phy_copy;
    int start_lane, num_lane;
    qmod_an_control_t an_control;
    int an_complete = 0;
    int      lane_id, sub_port ;

    config->ref_clock = ref_clock;
    PHYMOD_MEMCPY(&pm_phy_copy, phy, sizeof(pm_phy_copy));
    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(&phy->access, &start_lane, &num_lane));

    PHYMOD_IF_ERR_RETURN
        (qmod_lane_info(&phy->access, &lane_id, &sub_port));
    start_lane = lane_id ;
    
    pm_phy_copy.access.lane_mask = 0x1 << start_lane;
    PHYMOD_IF_ERR_RETURN
        (qmod_speed_id_get(&pm_phy_copy.access, &speed_id, sub_port));

    PHYMOD_MEMSET(&an_control, 0x0,  sizeof(qmod_an_control_t));
    PHYMOD_IF_ERR_RETURN
        (qmod_autoneg_control_get(&pm_phy_copy.access, sub_port, &an_control, &an_complete));

    PHYMOD_IF_ERR_RETURN
        (qtce_phy_firmware_lane_config_get(phy, &firmware_lane_config));
    
    PHYMOD_IF_ERR_RETURN
        (_qtce_speed_id_interface_config_get(&pm_phy_copy, speed_id, config, an_control.enable, &firmware_lane_config));

    if (firmware_lane_config.MediaType == phymodFirmwareMediaTypeOptics) {
        PHYMOD_INTF_MODES_FIBER_SET(config);
    } else {
        PHYMOD_INTF_MODES_FIBER_CLR(config);
    }

    return PHYMOD_E_NONE;
}


int qtce_phy_cl72_set(const phymod_phy_access_t* phy, uint32_t cl72_en)
{
  return PHYMOD_E_FAIL;
}

int qtce_phy_cl72_get(const phymod_phy_access_t* phy, uint32_t* cl72_en)
{
  return PHYMOD_E_FAIL;
}

int qtce_phy_cl72_status_get(const phymod_phy_access_t* phy, phymod_cl72_status_t* status)
{
  return PHYMOD_E_FAIL;
}

int qtce_phy_autoneg_ability_set(const phymod_phy_access_t* phy, const phymod_autoneg_ability_t* an_ability)
{
    qmod_an_ability_t value;
    int start_lane, num_lane;
    phymod_phy_access_t phy_copy;
    int      lane_id, sub_port ;

    /* next program the tx fir taps and driver current based on the input */
    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(&phy->access, &start_lane, &num_lane));

    PHYMOD_IF_ERR_RETURN
        (qmod_lane_info(&phy->access, &lane_id, &sub_port));

    start_lane = lane_id ;  

    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));
    phy_copy.access.lane_mask = 0x1 << start_lane;

    PHYMOD_MEMSET(&value, 0x0, sizeof(value));

    /* check if sgmii  or not */
    if (PHYMOD_AN_CAP_SGMII_GET(an_ability)) {
        switch (an_ability->sgmii_speed) {
        case phymod_CL37_SGMII_10M:
            value.cl37_adv.cl37_sgmii_speed = QMOD_CL37_SGMII_10M;
            break;
        case phymod_CL37_SGMII_100M:
            value.cl37_adv.cl37_sgmii_speed = QMOD_CL37_SGMII_100M;
            break;
        case phymod_CL37_SGMII_1000M:
            value.cl37_adv.cl37_sgmii_speed = QMOD_CL37_SGMII_1000M;
            break;
        default:
            value.cl37_adv.cl37_sgmii_speed = QMOD_CL37_SGMII_1000M;
            break;
        }
        if( PHYMOD_AN_CAP_HALF_DUPLEX_GET(an_ability)) {
            value.cl37_adv.an_duplex = QMOD_AN_HALF_DUPLEX_SET ;
        }
    }
    /* next check pause */
    if (PHYMOD_AN_CAP_SYMM_PAUSE_GET(an_ability) && !PHYMOD_AN_CAP_ASYM_PAUSE_GET(an_ability)) {
        value.cl37_adv.an_pause = QMOD_SYMM_PAUSE;
    }
    if (PHYMOD_AN_CAP_ASYM_PAUSE_GET(an_ability) && !PHYMOD_AN_CAP_SYMM_PAUSE_GET(an_ability)) {
        value.cl37_adv.an_pause = QMOD_ASYM_PAUSE;
    }
    if (PHYMOD_AN_CAP_ASYM_PAUSE_GET(an_ability) && PHYMOD_AN_CAP_SYMM_PAUSE_GET(an_ability)) {
        value.cl37_adv.an_pause = QMOD_ASYM_SYMM_PAUSE;
    }

    /* check cl37 and cl37 bam ability */
    if (PHYMOD_AN_CAP_CL37BAM_GET(an_ability)) {
        value.cl37_adv.an_bam_speed |= 1 << QMOD_CL37_BAM_2p5GBASE_X;
        PHYMOD_IF_ERR_RETURN
            (qmod_an_cl37_bam_abilities(&phy_copy.access, &value, sub_port));
    } else { 
        PHYMOD_IF_ERR_RETURN
            (qmod_autoneg_cl37_base_abilities(&phy_copy.access, &value, sub_port));
    }


 /*   PHYMOD_IF_ERR_RETURN
        (qmod_autoneg_set(&phy_copy.access, &value, sub_port));*/

    return PHYMOD_E_NONE;
}

int qtce_phy_autoneg_ability_get(const phymod_phy_access_t* phy, phymod_autoneg_ability_t* an_ability_get_type){
    qmod_an_ability_t value;
    phymod_phy_access_t phy_copy;
    int      lane_id, sub_port ;


    PHYMOD_IF_ERR_RETURN
        (qmod_lane_info(&phy->access, &lane_id, &sub_port));

    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));
    phy_copy.access.lane_mask = 0x1 << lane_id;
    PHYMOD_MEMSET(&value, 0x0, sizeof(value));
     
    PHYMOD_IF_ERR_RETURN
        (qmod_autoneg_local_ability_get(&phy_copy.access, &value, sub_port));

    if(value.cl37_adv.an_type != QMOD_AN_MODE_NONE) {
        if (value.cl37_adv.an_pause == QMOD_ASYM_PAUSE) {
            PHYMOD_AN_CAP_ASYM_PAUSE_SET(an_ability_get_type);
        } else if (value.cl37_adv.an_pause == QMOD_SYMM_PAUSE) {
            PHYMOD_AN_CAP_SYMM_PAUSE_SET(an_ability_get_type);
        } else if (value.cl37_adv.an_pause == QMOD_ASYM_SYMM_PAUSE) {
            PHYMOD_AN_CAP_ASYM_PAUSE_SET(an_ability_get_type);
            PHYMOD_AN_CAP_SYMM_PAUSE_SET(an_ability_get_type);
        }
        if(value.cl37_adv.an_type & QMOD_AN_MODE_CL37) {
            PHYMOD_AN_CAP_CL37_SET(an_ability_get_type) ;
            PHYMOD_AN_CAP_HALF_DUPLEX_CLR(an_ability_get_type) ;
        }
        if(value.cl37_adv.an_type & QMOD_AN_MODE_SGMII) {
            PHYMOD_AN_CAP_SGMII_SET(an_ability_get_type) ;
            /* get the cl37 sgmii speed */
            switch (value.cl37_adv.cl37_sgmii_speed) {
            case QMOD_CL37_SGMII_10M:
                an_ability_get_type->sgmii_speed = phymod_CL37_SGMII_10M;
                break;
            case QMOD_CL37_SGMII_100M:
                an_ability_get_type->sgmii_speed = phymod_CL37_SGMII_100M;
                break;
            case QMOD_CL37_SGMII_1000M:
                an_ability_get_type->sgmii_speed = phymod_CL37_SGMII_1000M;
                break;
            default:
                break;
            }
            if(value.cl37_adv.an_duplex & QMOD_AN_HALF_DUPLEX_SET ) { 
                PHYMOD_AN_CAP_HALF_DUPLEX_SET(an_ability_get_type) ;
            } else {
                PHYMOD_AN_CAP_HALF_DUPLEX_CLR(an_ability_get_type) ;
            }
        }
        if(value.cl37_adv.an_type & QMOD_AN_MODE_CL37BAM) {
            /* check cl37 bam ability */
            if (value.cl37_adv.an_bam_speed & 1 << QMOD_CL37_BAM_2p5GBASE_X) 
                PHYMOD_BAM_CL37_CAP_2P5G_SET(an_ability_get_type->cl37bam_cap);
        }
    }
    return PHYMOD_E_NONE;
}

int qtce_phy_autoneg_remote_ability_get(const phymod_phy_access_t* phy, phymod_autoneg_ability_t* an_ability_get_type)
{

    qmod_an_ability_t value;
    phymod_phy_access_t phy_copy;
    int      lane_id, sub_port ;

    PHYMOD_IF_ERR_RETURN
        (qmod_lane_info(&phy->access, &lane_id, &sub_port));

    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));
    phy_copy.access.lane_mask = 0x1 << lane_id;
    PHYMOD_MEMSET(&value, 0x0, sizeof(value));
     
    PHYMOD_IF_ERR_RETURN
        (qmod_autoneg_remote_ability_get(&phy_copy.access, &value, sub_port));

    if (value.cl37_adv.an_pause == QMOD_ASYM_PAUSE) {
        PHYMOD_AN_CAP_ASYM_PAUSE_SET(an_ability_get_type);
    } else if (value.cl37_adv.an_pause == QMOD_SYMM_PAUSE) {
        PHYMOD_AN_CAP_SYMM_PAUSE_SET(an_ability_get_type);
    } else if (value.cl37_adv.an_pause == QMOD_ASYM_SYMM_PAUSE) {
        PHYMOD_AN_CAP_ASYM_PAUSE_SET(an_ability_get_type);
        PHYMOD_AN_CAP_SYMM_PAUSE_SET(an_ability_get_type);
    }
    if(value.cl37_adv.an_type != QMOD_AN_MODE_NONE) {     
       if(value.cl37_adv.an_type & QMOD_AN_MODE_CL37) {
            PHYMOD_AN_CAP_CL37_SET(an_ability_get_type) ;
            PHYMOD_AN_CAP_HALF_DUPLEX_CLR(an_ability_get_type) ;
       }
        if(value.cl37_adv.an_type & QMOD_AN_MODE_SGMII) {
            PHYMOD_AN_CAP_SGMII_SET(an_ability_get_type) ;
            /* get the cl37 sgmii speed */
            switch (value.cl37_adv.cl37_sgmii_speed) {
            case QMOD_CL37_SGMII_10M:
                an_ability_get_type->sgmii_speed = phymod_CL37_SGMII_10M;
                break;
            case QMOD_CL37_SGMII_100M:
                an_ability_get_type->sgmii_speed = phymod_CL37_SGMII_100M;
                break;
            case QMOD_CL37_SGMII_1000M:
                an_ability_get_type->sgmii_speed = phymod_CL37_SGMII_1000M;
                break;
            default:
                break;
            }
        }
        /* check cl37 bam ability */
        if (value.cl37_adv.an_bam_speed & 1 << QMOD_CL37_BAM_2p5GBASE_X) 
            PHYMOD_BAM_CL37_CAP_2P5G_SET(an_ability_get_type->cl37bam_cap);
    }
    return PHYMOD_E_NONE;
}

STATIC
int _qtc_qsgmii_autoneg_set(const phymod_phy_access_t* phy, const phymod_autoneg_control_t* an)
{
    int num_lane_adv_encoded;
    int start_lane, num_lane, lane_id, sub_port;
    phymod_phy_access_t phy_copy;
    qmod_an_control_t an_control;
    
    
    /* next program the tx fir taps and driver current based on the input */
    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(&phy->access, &start_lane, &num_lane));

    PHYMOD_IF_ERR_RETURN
        (qmod_lane_info(&phy->access, &lane_id, &sub_port));

    start_lane = lane_id ;

    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));
    phy_copy.access.lane_mask = 0x1 << start_lane;

    num_lane_adv_encoded = 0;  /* one lane */
    an_control.pd_kx4_en = 0;  /* for now  disable */
    an_control.pd_kx_en = 0;   /* for now disable */
    an_control.num_lane_adv = num_lane_adv_encoded;
    an_control.enable       = an->enable;
    an_control.an_property_type = 0x0;   /* for now disable */

    switch (an->an_mode) {
    case phymod_AN_MODE_CL37:
        an_control.an_type = QMOD_AN_MODE_CL37;
        break;
    case phymod_AN_MODE_CL37BAM:
        { 
            
            an_control.an_type = QMOD_AN_MODE_CL37BAM;
            /* PHYMOD_IF_ERR_RETURN
                (qmod_an_cl37_bam_abilities(&phy_copy.access, &value, sub_port)); */
            break;
        }
    case phymod_AN_MODE_SGMII:
        an_control.an_type = QMOD_AN_MODE_SGMII;
        break;
    default:
        an_control.an_type = QMOD_AN_MODE_CL37;
        break;
    }

    phy_copy.access.lane_mask = 0x1 << start_lane;

    PHYMOD_IF_ERR_RETURN
        (qmod_autoneg_control(&phy_copy.access, &an_control, sub_port));
    
    return PHYMOD_E_NONE;

}

int qtce_phy_autoneg_set(const phymod_phy_access_t* phy, const phymod_autoneg_control_t* an)
{
    int num_lane_adv_encoded;
    phymod_firmware_lane_config_t firmware_lane_config; 
    phymod_firmware_core_config_t firmware_core_config_tmp;
    int start_lane, num_lane, i, lane_id, sub_port;
    phymod_phy_access_t phy_copy;
    qmod_an_control_t an_control;
    int ctrl_port ;

    if (PHYMOD_ACC_F_QMODE_GET(&phy->access)) {
        return _qtc_qsgmii_autoneg_set(phy, an);
    }

    /* next program the tx fir taps and driver current based on the input */
    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(&phy->access, &start_lane, &num_lane));

    PHYMOD_IF_ERR_RETURN
        (qmod_lane_info(&phy->access, &lane_id, &sub_port));

    start_lane = lane_id ;  
    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));
    phy_copy.access.lane_mask = 0x1 << start_lane;

    /* make sure the firmware config is set to an eenabled */
    PHYMOD_IF_ERR_RETURN
        (qtce_phy_firmware_core_config_get(&phy_copy, &firmware_core_config_tmp));


    switch (an->num_lane_adv) {
        case 1:
            num_lane_adv_encoded = 0;
            break;
        case 2:
            num_lane_adv_encoded = 1;
            break;
        case 4:
            num_lane_adv_encoded = 0;
            break;
        case 10:
            num_lane_adv_encoded = 3;
            break;
        default:
            return PHYMOD_E_PARAM;
    }

    an_control.pd_kx4_en = 0;  /* for now  disable */
    an_control.pd_kx_en = 0;   /* for now disable */
    an_control.num_lane_adv = num_lane_adv_encoded;
    an_control.enable       = an->enable;
    an_control.an_property_type = 0x0;   /* for now disable */  
    switch (an->an_mode) {
    case phymod_AN_MODE_CL37:
        an_control.an_type = QMOD_AN_MODE_CL37;
        break;
    case phymod_AN_MODE_CL37BAM:
        { 
            
            an_control.an_type = QMOD_AN_MODE_CL37BAM;
            /* PHYMOD_IF_ERR_RETURN
                (qmod_an_cl37_bam_abilities(&phy_copy.access, &value, sub_port)); */
            break;
        }
    case phymod_AN_MODE_SGMII:
        an_control.an_type = QMOD_AN_MODE_SGMII;
        break;
    default:
        an_control.an_type = QMOD_AN_MODE_CL37;
        break;
    }

    /* put pcs into reset */
      PHYMOD_IF_ERR_RETURN
            (qmod_reset(&phy_copy.access)); 


    for (i = 0; i < num_lane; i++) {
        phy_copy.access.lane_mask = 0x1 << (i + start_lane);
        PHYMOD_IF_ERR_RETURN
            (eagle_lane_soft_reset_release(&phy_copy.access, 0));
       }

    PHYMOD_USLEEP(500);
     
    /* make sure the firmware config is set to an eenabled */
    PHYMOD_IF_ERR_RETURN
        (qtce_phy_firmware_lane_config_get(phy, &firmware_lane_config));
    if (an->enable) {
        firmware_lane_config.AnEnabled = 1;
        firmware_lane_config.LaneConfigFromPCS = 1;
        firmware_core_config_tmp.CoreConfigFromPCS = 1;
    } else {
        firmware_lane_config.AnEnabled = 0;
        firmware_lane_config.LaneConfigFromPCS = 0;
        firmware_core_config_tmp.CoreConfigFromPCS = 0;
    }


    for (i = 0; i < num_lane; i++) {
        phy_copy.access.lane_mask = 0x1 << (i + start_lane);
        PHYMOD_IF_ERR_RETURN
            (_qtce_phy_firmware_lane_config_set(&phy_copy, firmware_lane_config));
    }

    /* Release core soft reset */
/*
    if ((an->num_lane_adv == 4) || (an->an_mode == phymod_AN_MODE_CL37BAM)) {
      phy_copy.access.lane_mask = 0x1 << start_lane;
      PHYMOD_IF_ERR_RETURN
        (eagle_core_soft_reset_release(&phy_copy.access, 1));
    }
*/

    for (i = 0; i < num_lane; i++) {
        phy_copy.access.lane_mask = 0x1 << (i + start_lane);
        PHYMOD_IF_ERR_RETURN
            (eagle_lane_soft_reset_release(&phy_copy.access, 1));
    }
    

    PHYMOD_USLEEP(1000); 

    PHYMOD_IF_ERR_RETURN
        (qmod_trigger_speed_change(&phy_copy.access));  


    phy_copy.access.lane_mask = 0x1 << start_lane;
    if (an->enable) {
        ctrl_port = 0 ;
        if((an->an_mode == phymod_AN_MODE_CL37BAM) ||  (an->num_lane_adv == 4 )) 
           ctrl_port = 1 ;
        PHYMOD_IF_ERR_RETURN
            (qmod_set_an_port_mode(&phy_copy.access, num_lane_adv_encoded, start_lane, ctrl_port));
    } else {
        ctrl_port =0 ;
        PHYMOD_IF_ERR_RETURN
            (qmod_set_an_port_mode(&phy_copy.access, num_lane_adv_encoded, start_lane, ctrl_port));
    }
    PHYMOD_IF_ERR_RETURN
        (qmod_autoneg_control(&phy_copy.access, &an_control, sub_port));
    
    return PHYMOD_E_NONE;
    
}

int qtce_phy_autoneg_get(const phymod_phy_access_t* phy, phymod_autoneg_control_t* an, uint32_t* an_done)
{
    qmod_an_control_t an_control;
    phymod_phy_access_t phy_copy;
    int start_lane, num_lane, lane_id, sub_port; 
    int an_complete = 0;
    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));

    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(&phy->access, &start_lane, &num_lane));

    PHYMOD_IF_ERR_RETURN
        (qmod_lane_info(&phy->access, &lane_id, &sub_port));

    start_lane = lane_id ;  
    phy_copy.access.lane_mask = 0x1 << start_lane;

    PHYMOD_MEMSET(&an_control, 0x0,  sizeof(qmod_an_control_t));
    PHYMOD_IF_ERR_RETURN
        (qmod_autoneg_control_get(&phy_copy.access,  sub_port, &an_control, &an_complete));
    
    if (an_control.enable) {
        an->enable = 1;
        *an_done = an_complete; 
    } else {
        an->enable = 0;
    }                

    return PHYMOD_E_NONE;
}


int qtce_phy_autoneg_status_get(const phymod_phy_access_t* phy, phymod_autoneg_status_t* status)
{
    /* Place your code here */

        
    return PHYMOD_E_NONE;
}


int qtce_core_init(const phymod_core_access_t* core, const phymod_core_init_config_t* init_config, const phymod_core_status_t* core_status)
{
    phymod_phy_access_t phy_access, phy_access_copy;
    phymod_core_access_t  core_copy;
    phymod_firmware_core_config_t  firmware_core_config_tmp;
    uint32_t  uc_active = 0;
#if 0
    qmod_an_init_t an; 
#endif
    QTCE_CORE_TO_PHY_ACCESS(&phy_access, core);
    phy_access_copy = phy_access;
    PHYMOD_MEMCPY(&core_copy, core, sizeof(core_copy));
    core_copy.access.lane_mask = 0x1;
        phy_access_copy = phy_access;
        phy_access_copy.access = core->access;
        phy_access_copy.access.lane_mask = 0x1;
        phy_access_copy.type = core->type;

    PHYMOD_IF_ERR_RETURN(eagle_uc_active_get(&core_copy.access, &uc_active));
    if(uc_active) {
        return(PHYMOD_E_NONE);
    }

    PHYMOD_IF_ERR_RETURN
        (qmod_pmd_reset_seq(&core_copy.access, core_status->pmd_active));

    /* need to set the heart beat default is for 156.25M */
    if (init_config->interface.ref_clock == phymodRefClk125Mhz) {
        PHYMOD_IF_ERR_RETURN
            (qmod_refclk_set(&core_copy.access, QMODREFCLK125MHZ)) ;
    } else {
        PHYMOD_IF_ERR_RETURN
            (qmod_refclk_set(&core_copy.access, QMODREFCLK156MHZ)) ;
    }
  
    if (_qtce_core_firmware_load(&core_copy, init_config->firmware_load_method, init_config->firmware_loader)) {
        PHYMOD_DEBUG_ERROR(("devad 0x%x lane 0x%x: UC firmware-load failed\n", core->access.addr, core->access.lane_mask));  
        PHYMOD_IF_ERR_RETURN (PHYMOD_E_INIT);
    }
#ifndef QTCE_PMD_CRC_UCODE
    /* next we need to check if the load is correct or not */
    if (eagle_tsc_ucode_load_verify(&core_copy.access, (uint8_t *) &qtce_ucode, qtce_ucode_len)) {
        PHYMOD_DEBUG_ERROR(("devad 0x%x lane 0x%x: UC load-verify failed\n", core->access.addr, core->access.lane_mask));  
        PHYMOD_IF_ERR_RETURN (PHYMOD_E_INIT);
    }
#endif /* QTCE_PMD_CRC_UCODE */

    PHYMOD_IF_ERR_RETURN
        (eagle_pmd_ln_h_rstb_pkill_override( &phy_access_copy.access, 0x1));

    /* next we need to set the uc active and release uc */
    PHYMOD_IF_ERR_RETURN
        (eagle_uc_active_set(&core_copy.access ,1)); 
    /* release the uc reset */
    PHYMOD_IF_ERR_RETURN
        (eagle_uc_reset(&core_copy.access ,1)); 
    /* we need to wait at least 10ms for the uc to settle */
    PHYMOD_USLEEP(10000);

    if(PHYMOD_CORE_INIT_F_FIRMWARE_LOAD_VERIFY_GET(init_config)) {
#ifndef QTCE_PMD_CRC_UCODE
        /* poll the ready bit in 10 ms */
        eagle_tsc_poll_uc_dsc_ready_for_cmd_equals_1(&phy_access_copy.access, 1);
#else
        PHYMOD_IF_ERR_RETURN(
                eagle_tsc_ucode_crc_verify( &core_copy.access, qtce_ucode_len,qtce_ucode_crc));
#endif /* QTCE_PMD_CRC_UCODE */
    }

    PHYMOD_IF_ERR_RETURN(
        eagle_pmd_ln_h_rstb_pkill_override( &phy_access_copy.access, 0x0));

    PHYMOD_IF_ERR_RETURN
        (eagle_core_soft_reset_release(&core_copy.access, 0));

    /* plldiv CONFIG */
    if (PHYMOD_ACC_F_QMODE_GET(&core->access)) {
        PHYMOD_IF_ERR_RETURN
            (eagle_pll_mode_set(&core->access, 0x9));
    } else {
        PHYMOD_IF_ERR_RETURN
            (eagle_pll_mode_set(&core->access, 0xa));
    }

    PHYMOD_IF_ERR_RETURN
        (qtce_core_lane_map_set(core, &init_config->lane_map)); 
    PHYMOD_IF_ERR_RETURN
        (_qtce_core_lane_override_set(core));
    PHYMOD_IF_ERR_RETURN
        (qmod_autoneg_timer_init(&core->access));
    PHYMOD_IF_ERR_RETURN
        (qmod_master_port_num_set(&core->access, 0));
    /* don't overide the fw that set in config set if not specified */
    firmware_core_config_tmp = init_config->firmware_core_config;
    firmware_core_config_tmp.CoreConfigFromPCS = 0;
    /* set the vco rate to be default at 10.0000G */
    if (PHYMOD_ACC_F_QMODE_GET(&core->access)) {
        firmware_core_config_tmp.VcoRate = 0x12;
    } else {
        firmware_core_config_tmp.VcoRate = 0x3;
    }

    PHYMOD_IF_ERR_RETURN
        (qtce_phy_firmware_core_config_set(&phy_access_copy, firmware_core_config_tmp)); 

    /* release core soft reset */
    PHYMOD_IF_ERR_RETURN
        (eagle_core_soft_reset_release(&core_copy.access, 1));
        
    return PHYMOD_E_NONE;
}


int qtce_phy_init(const phymod_phy_access_t* phy, const phymod_phy_init_config_t* init_config)
{
    int pll_restart = 0;
    const phymod_access_t *pm_acc = &phy->access;
    phymod_phy_access_t pm_phy_copy;
    int start_lane, num_lane, i, lane_id, sub_port;
    phymod_polarity_t tmp_pol;
    uint32_t sc_enable = 0;
    PHYMOD_MEMSET(&tmp_pol, 0x0, sizeof(tmp_pol));
    PHYMOD_MEMCPY(&pm_phy_copy, phy, sizeof(pm_phy_copy));

    /* next program the tx fir taps and driver current based on the input */
    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(pm_acc, &start_lane, &num_lane));
    PHYMOD_IF_ERR_RETURN
        (qmod_lane_info(&phy->access, &lane_id, &sub_port));

    start_lane = lane_id ;  
    pm_phy_copy.access.lane_mask = 1 << start_lane;

    if (PHYMOD_ACC_F_QMODE_GET(&phy->access)) {
        PHYMOD_IF_ERR_RETURN(qmod_port_state_set(&pm_phy_copy.access, QMOD_PORT_STATE_CONFIGED, sub_port, 1));

        PHYMOD_IF_ERR_RETURN(qmod_speedchange_get(&pm_phy_copy.access, &sc_enable)) ;
        if (sc_enable) {
            return PHYMOD_E_NONE;
        }
    }

    /* per lane based reset release */
    PHYMOD_IF_ERR_RETURN
        (qmod_pmd_x4_reset(&pm_phy_copy.access));

    /* poll for per lane uc_dsc_ready */
    for (i = 0; i < num_lane; i++) {
        pm_phy_copy.access.lane_mask = 1 << (start_lane + i);
        PHYMOD_IF_ERR_RETURN
            (eagle_lane_soft_reset_release(&pm_phy_copy.access, 1));
    }

    /* program the rx/tx polarity */
    tmp_pol.tx_polarity = (init_config->polarity.tx_polarity) & 0x1;
    tmp_pol.rx_polarity = (init_config->polarity.rx_polarity) & 0x1;
    PHYMOD_IF_ERR_RETURN
        (qtce_phy_polarity_set(phy, &tmp_pol));

    for (i = 0; i < num_lane; i++) {
        pm_phy_copy.access.lane_mask = 1 << (start_lane + i);
        PHYMOD_IF_ERR_RETURN
            (_qtce_phy_tx_set(&pm_phy_copy, &init_config->tx[i]));
    }

    pm_phy_copy.access.lane_mask = 0x1;

    PHYMOD_IF_ERR_RETURN
        (qmod_update_port_mode(&pm_phy_copy.access, &pll_restart));

    pm_phy_copy.access.lane_mask = 1 << start_lane;
    PHYMOD_IF_ERR_RETURN
        (qmod_rx_lane_control_set(&pm_phy_copy.access, 1));
    PHYMOD_IF_ERR_RETURN
        (qmod_tx_lane_control_set(&pm_phy_copy.access, QMOD_TX_LANE_RESET_TRAFFIC_ENABLE));         /* TX_LANE_CONTROL */

    return PHYMOD_E_NONE;
}


int qtce_phy_loopback_set(const phymod_phy_access_t* phy, phymod_loopback_mode_t loopback, uint32_t enable)
{
    int start_lane, num_lane, lane_id, sub_port;
    int rv = PHYMOD_E_NONE;
    int i = 0;
    phymod_phy_access_t phy_copy;
    if (PHYMOD_ACC_F_QMODE_GET(&phy->access) && enable) {
        return PHYMOD_E_UNAVAIL;
    }

    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));

    /* next figure out the lane num and start_lane based on the input */
    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(&phy->access, &start_lane, &num_lane));

    PHYMOD_IF_ERR_RETURN
        (qmod_lane_info(&phy->access, &lane_id, &sub_port));
    start_lane = lane_id ;

    phy_copy.access.lane_mask = 0x1 << (i + start_lane);
    
      
    switch (loopback) {
    case phymodLoopbackGlobal :
        PHYMOD_IF_ERR_RETURN(qmod_tx_loopback_control(&phy_copy.access, enable, start_lane, num_lane));
        break;
    case phymodLoopbackGlobalPMD :
        for (i = 0; i < num_lane; i++) {
            phy_copy.access.lane_mask = 0x1 << (i + start_lane);
            PHYMOD_IF_ERR_RETURN(qmod_tx_squelch_set(&phy_copy.access, enable));
            PHYMOD_IF_ERR_RETURN(eagle_tsc_dig_lpbk(&phy_copy.access, (uint8_t) enable));
            PHYMOD_IF_ERR_RETURN(eagle_pmd_force_signal_detect(&phy_copy.access, (int) enable));
            PHYMOD_IF_ERR_RETURN(qmod_rx_lane_control_set(&phy_copy.access, 1));
        }
        break;
    case phymodLoopbackRemotePMD :
        PHYMOD_IF_ERR_RETURN(eagle_tsc_rmt_lpbk(&phy_copy.access, (uint8_t)enable));
        break;
    case phymodLoopbackRemotePCS :
        PHYMOD_IF_ERR_RETURN(qmod_rx_loopback_control(&phy_copy.access, enable, enable, enable)); /* RAVI */
        break;
    default :
        break;
    }             
    return rv;
}

int qtce_phy_loopback_get(const phymod_phy_access_t* phy, phymod_loopback_mode_t loopback, uint32_t* enable)
{
    uint32_t enable_core;
    int start_lane, num_lane, lane_id, sub_port;
    phymod_phy_access_t phy_copy;

    *enable = 0;

    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));
    /* next figure out the lane num and start_lane based on the input */
    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(&phy->access, &start_lane, &num_lane));

    PHYMOD_IF_ERR_RETURN
        (qmod_lane_info(&phy->access, &lane_id, &sub_port));
    start_lane = lane_id ;

    phy_copy.access.lane_mask = 0x1 << start_lane;
    

    switch (loopback) {
    case phymodLoopbackGlobal :
        PHYMOD_IF_ERR_RETURN(qmod_tx_loopback_get(&phy_copy.access, &enable_core));
        *enable = (enable_core >> start_lane) & 0x1; 
        break;
    case phymodLoopbackGlobalPMD :
        PHYMOD_IF_ERR_RETURN(eagle_pmd_loopback_get(&phy_copy.access, enable));
        break;
    case phymodLoopbackRemotePMD :
        /* PHYMOD_IF_ERR_RETURN(eagle_tsc_rmt_lpbk(&phy->access, (uint8_t)enable)); */
        break;
    case phymodLoopbackRemotePCS :
        /* PHYMOD_IF_ERR_RETURN(qtce_rx_loopback_control(&phy->access, enable, enable, enable)); */ /* RAVI */
        break;
    default :
        break;
    }             
    return PHYMOD_E_NONE;
}


int qtce_phy_rx_pmd_locked_get(const phymod_phy_access_t* phy, uint32_t* rx_seq_done){
    int lane_id, sub_port;
    phymod_phy_access_t phy_copy;

    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));
    PHYMOD_IF_ERR_RETURN
        (qmod_lane_info(&phy->access, &lane_id, &sub_port));
    phy_copy.access.lane_mask = 1 << lane_id;

    PHYMOD_IF_ERR_RETURN(qmod_pmd_lock_get(&phy_copy.access, rx_seq_done));
    return PHYMOD_E_NONE;
}


int qtce_phy_link_status_get(const phymod_phy_access_t* phy, uint32_t* link_status)
{
    int lane_id, sub_port;
    phymod_phy_access_t pm_phy_copy;

    PHYMOD_MEMCPY(&pm_phy_copy, phy, sizeof(pm_phy_copy));
    PHYMOD_IF_ERR_RETURN
        (qmod_lane_info(&phy->access, &lane_id, &sub_port));
    pm_phy_copy.access.lane_mask = 1 << lane_id;

    PHYMOD_IF_ERR_RETURN(qmod_get_pcs_link_status(&pm_phy_copy.access, sub_port, link_status));
    return PHYMOD_E_NONE;
}


int qtce_phy_pcs_userspeed_set(const phymod_phy_access_t* phy, const phymod_pcs_userspeed_config_t* config)
{
    return PHYMOD_E_UNAVAIL;
}

int qtce_phy_pcs_userspeed_get(const phymod_phy_access_t* phy, const phymod_pcs_userspeed_config_t* config)
{
    return PHYMOD_E_UNAVAIL;
}


int qtce_phy_status_dump(const phymod_phy_access_t* phy)
{
    int lane_id, sub_port;
    phymod_phy_access_t phy_copy;

    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));
    PHYMOD_IF_ERR_RETURN
        (qmod_lane_info(&phy->access, &lane_id, &sub_port));
    phy_copy.access.lane_mask = 1 << lane_id;

    PHYMOD_IF_ERR_RETURN
        (eagle_tsc_display_core_state(&phy_copy.access));
    PHYMOD_IF_ERR_RETURN
        (eagle_tsc_display_lane_debug_status(&phy_copy.access));

    return PHYMOD_E_NONE;
}

int qtce_phy_reg_read(const phymod_phy_access_t* phy, uint32_t reg_addr, uint32_t *val)
{
    int lane_id, sub_port;
    phymod_phy_access_t phy_copy;

    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));
    PHYMOD_IF_ERR_RETURN
        (qmod_lane_info(&phy->access, &lane_id, &sub_port));
    phy_copy.access.lane_mask = 1 << lane_id;

    PHYMOD_IF_ERR_RETURN(phymod_tsc_iblk_read(&phy_copy.access, reg_addr, val));
    return PHYMOD_E_NONE;
}


int qtce_phy_reg_write(const phymod_phy_access_t* phy, uint32_t reg_addr, uint32_t val)
{
    int lane_id, sub_port;
    phymod_phy_access_t phy_copy;

    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));
    PHYMOD_IF_ERR_RETURN
        (qmod_lane_info(&phy->access, &lane_id, &sub_port));
    phy_copy.access.lane_mask = 1 << lane_id;

    PHYMOD_IF_ERR_RETURN(phymod_tsc_iblk_write(&phy_copy.access, reg_addr, val));
    return PHYMOD_E_NONE;  
}

int qtce_phy_eee_set(const phymod_phy_access_t* phy, uint32_t enable)
{
    uint32_t lpi_bypass;
    lpi_bypass = PHYMOD_LPI_BYPASS_GET(enable);
    enable &= 0x1;
    if (lpi_bypass) {
    PHYMOD_IF_ERR_RETURN(qmod_eee_control_set(&phy->access, enable));
    } else {
        return PHYMOD_E_UNAVAIL;
    }
    return PHYMOD_E_NONE;
}

int qtce_phy_eee_get(const phymod_phy_access_t* phy, uint32_t* enable)
{
    if (PHYMOD_LPI_BYPASS_GET(*enable)) {
        PHYMOD_IF_ERR_RETURN(qmod_eee_control_get(&phy->access, enable));
        PHYMOD_LPI_BYPASS_SET(*enable);
    } else {
        return PHYMOD_E_UNAVAIL;
    }
    return PHYMOD_E_NONE;
}

int qtce_phy_rx_ppm_get(const phymod_phy_access_t* phy, int16_t* rx_ppm)
{
    int start_lane, num_lane;
    phymod_phy_access_t pm_phy_copy;

    PHYMOD_MEMCPY(&pm_phy_copy, phy, sizeof(pm_phy_copy));

    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(&phy->access, &start_lane, &num_lane));

    pm_phy_copy.access.lane_mask = 1 << start_lane;
    PHYMOD_IF_ERR_RETURN
        (eagle_tsc_rx_ppm(&pm_phy_copy.access, rx_ppm));

    return PHYMOD_E_NONE;
}

int qtce_phy_synce_clk_ctrl_set(const phymod_phy_access_t* phy,
                                  phymod_synce_clk_ctrl_t cfg)
{
    int lane_id, sub_port;
    phymod_phy_access_t phy_copy;

    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));
    PHYMOD_IF_ERR_RETURN
        (qmod_lane_info(&phy->access, &lane_id, &sub_port));
    phy_copy.access.lane_mask = 1 << lane_id;

    PHYMOD_IF_ERR_RETURN
        (qmod_synce_mode_set(&phy_copy.access, cfg.stg0_mode, cfg.stg1_mode));

    PHYMOD_IF_ERR_RETURN
        (qmod_synce_clk_ctrl_set(&phy_copy.access, cfg.sdm_val));

    return PHYMOD_E_NONE;
}

int qtce_phy_synce_clk_ctrl_get(const phymod_phy_access_t* phy,
                                  phymod_synce_clk_ctrl_t *cfg)
{
    int lane_id, sub_port;
    phymod_phy_access_t phy_copy;

    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));
    PHYMOD_IF_ERR_RETURN
        (qmod_lane_info(&phy->access, &lane_id, &sub_port));
    phy_copy.access.lane_mask = 1 << lane_id;

    PHYMOD_IF_ERR_RETURN
        (qmod_synce_mode_get(&phy_copy.access, &(cfg->stg0_mode), &(cfg->stg1_mode)));

    PHYMOD_IF_ERR_RETURN
        (qmod_synce_clk_ctrl_get(&phy_copy.access, &(cfg->sdm_val)));

    return PHYMOD_E_NONE;
}

#endif /* PHYMOD_QTCE_SUPPORT */
