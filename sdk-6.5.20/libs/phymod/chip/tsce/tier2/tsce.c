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
#include <phymod/chip/bcmi_tsce_xgxs_defs.h>
#include <phymod/chip/tsce.h>
#include <phymod/chip/eagle_common.h>
#include "../../tsce/tier1/temod_enum_defines.h"
#include "../../tsce/tier1/temod.h"
#include "../../tsce/tier1/temod_device.h"
#include "../../tsce/tier1/temod_sc_lkup_table.h"
#include "../../tsce/tier1/tePCSRegEnums.h"
#include "../../eagle/tier1/eagle_cfg_seq.h"  
#include "../../eagle/tier1/eagle_tsc_common.h" 
#include "../../eagle/tier1/eagle_tsc_interface.h" 
#include "../../eagle/tier1/eagle_tsc_debug_functions.h" 
#include "../../eagle/tier1/eagle_tsc_dependencies.h" 



#define TSCE_ID0        0x600d
#define TSCE_ID1        0x8770
#define TSCE_REV_MASK   0x0

#define TSCE4_MODEL     0x12
#define TSCE12_MODEL    0x13
#define EAGLE_MODEL     0x1a
#define TSCE_TECH_PROC  0x3     /* 28nm */

#define TSCE_NOF_DFES (5)
#define TSCE_NOF_LANES_IN_CORE (4)
#define TSCE_LANE_SWAP_LANE_MASK (0x3)
#define TSCE_PHY_ALL_LANES (0xf)
#define TSCE_CORE_TO_PHY_ACCESS(_phy_access, _core_access) \
    do{\
        PHYMOD_MEMCPY(&(_phy_access)->access, &(_core_access)->access, sizeof((_phy_access)->access));\
        (_phy_access)->type           = (_core_access)->type; \
        (_phy_access)->port_loc       = (_core_access)->port_loc; \
        (_phy_access)->device_op_mode = (_core_access)->device_op_mode; \
        (_phy_access)->access.lane_mask = TSCE_PHY_ALL_LANES; \
    }while(0)

#define TSCE_MAX_FIRMWARES (5)

#define TSCE_USERSPEED_SELECT(_phy, _config, _type)  \
    ((_config)->mode==phymodPcsUserSpeedModeST) ?                       \
      temod_st_control_field_set(&(_phy)->access, (_config)->current_entry, _type, (_config)->value) : \
        temod_override_set(&(_phy)->access, _type, (_config)->value) 

#define TSCE_USERSPEED_CREDIT_SELECT(_phy, _config, _type)  \
    ((_config)->mode==phymodPcsUserSpeedModeST) ?                       \
      temod_st_credit_field_set(&(_phy)->access, (_config)->current_entry, _type, (_config)->value) : \
        temod_credit_override_set(&(_phy)->access, _type, (_config)->value) 

#define TSCE_PMD_CRC_UCODE  1
/* uController's firmware */
extern unsigned char tsce_ucode[];
extern unsigned short tsce_ucode_ver;
extern unsigned short tsce_ucode_crc;
extern unsigned short tsce_ucode_len;

typedef int (*sequncer_control_f)(const phymod_access_t* core, uint32_t enable);
typedef int (*rx_DFE_tap_control_set_f)(const phymod_access_t* phy, uint32_t val);
extern int tsce_phy_interface_config_get(const phymod_phy_access_t* phy, uint32_t flags, phymod_ref_clk_t ref_clock, phymod_phy_inf_config_t* config);

STATIC
int _tsce_phy_firmware_lane_config_set(const phymod_phy_access_t* phy, phymod_firmware_lane_config_t fw_config)
{
    struct eagle_tsc_uc_lane_config_st serdes_firmware_config;
    phymod_phy_access_t phy_copy;
    int start_lane, num_lane, i;
    /* uint32_t rst_status; */
    uint32_t is_warm_boot;

    PHYMOD_MEMSET(&serdes_firmware_config, 0x0, sizeof(serdes_firmware_config));
    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(&phy->access, &start_lane, &num_lane));
    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));

    for (i = 0; i < num_lane; i++) {
        if (!PHYMOD_LANEPBMP_MEMBER(phy->access.lane_mask, start_lane + i)) {
            continue;
        }
        phy_copy.access.lane_mask = 1 << (start_lane + i);
        serdes_firmware_config.field.lane_cfg_from_pcs = fw_config.LaneConfigFromPCS;
        serdes_firmware_config.field.an_enabled        = fw_config.AnEnabled;
        serdes_firmware_config.field.dfe_on            = fw_config.DfeOn; 
        serdes_firmware_config.field.force_brdfe_on    = fw_config.ForceBrDfe;
        /* serdes_firmware_config.field.cl72_emulation_en = fw_config.Cl72Enable; */
        serdes_firmware_config.field.scrambling_dis    = fw_config.ScramblingDisable;
        serdes_firmware_config.field.unreliable_los    = fw_config.UnreliableLos;
        serdes_firmware_config.field.media_type        = fw_config.MediaType; 
        serdes_firmware_config.field.cl72_auto_polarity_en   = fw_config.Cl72AutoPolEn;
        serdes_firmware_config.field.cl72_restart_timeout_en = fw_config.Cl72RestTO;

        PHYMOD_IF_ERR_RETURN(PHYMOD_IS_WRITE_DISABLED(&phy_copy.access, &is_warm_boot));

		
        if(!is_warm_boot) {
            /* PHYMOD_IF_ERR_RETURN(eagle_lane_soft_reset_read(&phy_copy.access, &rst_status)); */
            /* if(rst_status) */ PHYMOD_IF_ERR_RETURN (eagle_lane_soft_reset_release(&phy_copy.access, 0));
            PHYMOD_IF_ERR_RETURN(eagle_tsc_set_uc_lane_cfg(&phy_copy.access, serdes_firmware_config));
            /* if(rst_status) */ PHYMOD_IF_ERR_RETURN (eagle_lane_soft_reset_release(&phy_copy.access, 1));
        }
    }
    return PHYMOD_E_NONE;
}

int tsce_core_identify(const phymod_core_access_t* core, uint32_t core_id, uint32_t* is_identified)
{
    int ioerr = 0;
    const phymod_access_t *pm_acc = &core->access;
    PHYID2r_t id2;
    PHYID3r_t id3;
    MAIN0_SERDESIDr_t serdesid;
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

    if (PHYID2r_GET(id2) == TSCE_ID0 &&
        (PHYID3r_GET(id3) &= ~TSCE_REV_MASK) == TSCE_ID1) {
        /* PHY IDs match - now check PCS model */
        ioerr += READ_MAIN0_SERDESIDr(pm_acc, &serdesid);
        model = MAIN0_SERDESIDr_MODEL_NUMBERf_GET(serdesid);
        if (model == TSCE4_MODEL || model == TSCE12_MODEL)  {
            /* PCS model matches - now check PMD model */
            /* for now Bypass the pmd register rev check */
            /* ioerr += READ_DIG_REVID0r(pm_acc, &revid);
            if (DIG_REVID0r_REVID_MODELf_GET(revid) == EAGLE_MODEL) */ {
                if(MAIN0_SERDESIDr_TECH_PROCf_GET(serdesid) == TSCE_TECH_PROC) {
                    *is_identified = 1;
                }
            }
        }
    }
    rv = ioerr ? PHYMOD_E_IO : PHYMOD_E_NONE;
    return rv ;
}


int tsce_core_info_get(const phymod_core_access_t* phy, phymod_core_info_t* info)
{
    uint32_t serdes_id;
    PHYID2r_t id2;
    PHYID3r_t id3;
    char core_name[15];
    const phymod_access_t *pm_acc = &phy->access;

    PHYMOD_IF_ERR_RETURN
        (temod_revid_read(&phy->access, &serdes_id));

    info->serdes_id = serdes_id;
    if ((serdes_id & 0x3f) == TSCE4_MODEL) {
        info->core_version = phymodCoreVersionTsce4A0;
        PHYMOD_STRNCPY(core_name, "tsce4", PHYMOD_STRLEN("tsce4")+1);
    } else {
        info->core_version = phymodCoreVersionTsce12A0;
        PHYMOD_STRNCPY(core_name, "tsce12", PHYMOD_STRLEN("tsce12")+1);
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
 * set lane swapping for core 
 * The tx swap is composed of PCS swap and after that the PMD swap. 
 * The rx swap is composed just by PCS swap
 *
 * lane_map_tx and lane_map_rx[lane=logic_lane] are logic-lane base.
 * pcs_swap and register is logic_lane base.
 * but pmd_tx_map and addr_index_swap (and registers) are physical lane base 
 */
 
int tsce_core_lane_map_set(const phymod_core_access_t* core, const phymod_lane_map_t* lane_map)
{
    uint32_t pcs_swap = 0 , pmd_swap = 0, lane;
    uint32_t addr_index_swap = 0, pmd_tx_map =0;

    if(lane_map->num_of_lanes != TSCE_NOF_LANES_IN_CORE){
        return PHYMOD_E_CONFIG;
    }
    for( lane = 0 ; lane < TSCE_NOF_LANES_IN_CORE ; lane++){
        if(lane_map->lane_map_rx[lane] >= TSCE_NOF_LANES_IN_CORE){
            return PHYMOD_E_CONFIG;
        }
        /* encode each lane as four bits */
        /* pcs_map[lane] = rx_map[lane] */
        pcs_swap += lane_map->lane_map_rx[lane]<<(lane*4);
    }

    for( lane = 0 ; lane < TSCE_NOF_LANES_IN_CORE ; lane++){
        if(lane_map->lane_map_tx[lane] >= TSCE_NOF_LANES_IN_CORE){
            return PHYMOD_E_CONFIG;
        }
        /* encode each lane as four bits. pmd_swap is logic base */
        /* considering the pcs lane swap: pmd_map[pcs_map[lane]] = tx_map[lane] */
        pmd_swap += lane_map->lane_map_tx[lane]<<(lane_map->lane_map_rx[lane]*4);
    }

    for( lane = 0 ; lane < TSCE_NOF_LANES_IN_CORE ; lane++){
        addr_index_swap |= (lane << 4*((pcs_swap >> lane*4) & 0xf)) ;
        pmd_tx_map      |= (lane << 4*((pmd_swap >> lane*4) & 0xf)) ;
    }
    if (!PHYMOD_DEVICE_OP_MODE_PCS_BYPASS_GET(core->device_op_mode)) {
        PHYMOD_IF_ERR_RETURN(temod_pcs_lane_swap(&core->access, pcs_swap));

        PHYMOD_IF_ERR_RETURN
            (temod_pmd_addr_lane_swap(&core->access, addr_index_swap));
    }
    PHYMOD_IF_ERR_RETURN
        (temod_pmd_lane_swap_tx(&core->access, pmd_tx_map));
    return PHYMOD_E_NONE;
}


int tsce_core_lane_map_get(const phymod_core_access_t* core, phymod_lane_map_t* lane_map)
{
    uint32_t pmd_swap = 0 , pcs_swap = 0, lane; 
    PHYMOD_IF_ERR_RETURN(temod_pcs_lane_swap_get(&core->access, &pcs_swap));
    PHYMOD_IF_ERR_RETURN(temod_pmd_lane_swap_tx_get(&core->access, &pmd_swap));
    for( lane = 0 ; lane < TSCE_NOF_LANES_IN_CORE ; lane++){
        /* deccode each lane from four bits */
        lane_map->lane_map_rx[lane] = (pcs_swap>>(lane*4)) & TSCE_LANE_SWAP_LANE_MASK;
        /* considering the pcs lane swap: tx_map[lane] = pmd_map[pcs_map[lane]] */
        lane_map->lane_map_tx[lane] = (pmd_swap>>(lane_map->lane_map_rx[lane]*4)) & TSCE_LANE_SWAP_LANE_MASK;
    }
    lane_map->num_of_lanes = TSCE_NOF_LANES_IN_CORE;   
    return PHYMOD_E_NONE;
}


int tsce_core_reset_set(const phymod_core_access_t* core, phymod_reset_mode_t reset_mode, phymod_reset_direction_t direction)
{
    
    /* Place your code here */

        
    return PHYMOD_E_NONE;
}


int tsce_core_reset_get(const phymod_core_access_t* core, phymod_reset_mode_t reset_mode, phymod_reset_direction_t* direction)
{       
    return PHYMOD_E_UNAVAIL;
}

int tsce_core_firmware_info_get(const phymod_core_access_t* core, phymod_core_firmware_info_t* fw_info)
{
    
    /* 
     * It's O.K to use this code as is since the firmware CRC is already checked at 
     * at the time we load it.
     */
    fw_info->fw_crc = tsce_ucode_crc;
    fw_info->fw_version = tsce_ucode_ver;
    return PHYMOD_E_NONE;
}

/* load tsce fw. the fw_loader parameter is valid just for external fw load */
STATIC
int _tsce_core_firmware_load(const phymod_core_access_t* core, phymod_firmware_load_method_t load_method, phymod_firmware_loader_f fw_loader)
{
#ifndef TSCE_PMD_CRC_UCODE
    phymod_core_firmware_info_t actual_fw;
#endif


    switch(load_method){
    case phymodFirmwareLoadMethodInternal:
        PHYMOD_IF_ERR_RETURN(eagle_tsc_ucode_mdio_load(&core->access, tsce_ucode, tsce_ucode_len));
        break;
    case phymodFirmwareLoadMethodExternal:
        PHYMOD_NULL_CHECK(fw_loader); 

        eagle_pram_flop_set(&core->access, 0x0);

        PHYMOD_IF_ERR_RETURN(eagle_tsc_ucode_init(&core->access));

        PHYMOD_IF_ERR_RETURN
            (temod_pram_abl_enable_set(&core->access,1));
        PHYMOD_IF_ERR_RETURN
            (eagle_pram_firmware_enable(&core->access, 1));

        PHYMOD_IF_ERR_RETURN(fw_loader(core, tsce_ucode_len, tsce_ucode)); 

        PHYMOD_IF_ERR_RETURN
            (eagle_pram_firmware_enable(&core->access, 0));
        PHYMOD_IF_ERR_RETURN
            (temod_pram_abl_enable_set(&core->access,0));
        break;
    case phymodFirmwareLoadMethodNone:
        break;
    default:
        PHYMOD_RETURN_WITH_ERR(PHYMOD_E_CONFIG, (_PHYMOD_MSG("illegal fw load method %u"), load_method));
    }
    return PHYMOD_E_NONE;
}

int tsce_phy_firmware_core_config_set(const phymod_phy_access_t* phy, phymod_firmware_core_config_t fw_config)
{
    struct eagle_tsc_uc_core_config_st serdes_firmware_core_config;
    PHYMOD_MEMSET(&serdes_firmware_core_config, 0, sizeof(serdes_firmware_core_config));
    serdes_firmware_core_config.field.core_cfg_from_pcs = fw_config.CoreConfigFromPCS;
    serdes_firmware_core_config.field.vco_rate = fw_config.VcoRate; 
    PHYMOD_IF_ERR_RETURN(eagle_tsc_set_uc_core_config(&phy->access, serdes_firmware_core_config));
    return PHYMOD_E_NONE;
}


int tsce_phy_firmware_core_config_get(const phymod_phy_access_t* phy, phymod_firmware_core_config_t* fw_config)
{
    struct eagle_tsc_uc_core_config_st serdes_firmware_core_config;
    PHYMOD_IF_ERR_RETURN(eagle_tsc_get_uc_core_config(&phy->access, &serdes_firmware_core_config));
    PHYMOD_MEMSET(fw_config, 0, sizeof(*fw_config));
    fw_config->CoreConfigFromPCS = serdes_firmware_core_config.field.core_cfg_from_pcs;
    fw_config->VcoRate = serdes_firmware_core_config.field.vco_rate;
    return PHYMOD_E_NONE; 
}


int tsce_phy_firmware_lane_config_set(const phymod_phy_access_t* phy, phymod_firmware_lane_config_t fw_config)
{
    phymod_phy_access_t phy_copy;
    int start_lane, num_lane, i;

    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(&phy->access, &start_lane, &num_lane));
    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));

    /*Hold the per lne soft reset bit*/
    for (i = 0; i < num_lane; i++) {
        if (!PHYMOD_LANEPBMP_MEMBER(phy->access.lane_mask, start_lane + i)) {
            continue;
        }
        phy_copy.access.lane_mask = 1 << (start_lane + i);
        PHYMOD_IF_ERR_RETURN
            (eagle_lane_soft_reset_release(&phy_copy.access, 0));
    }

    PHYMOD_IF_ERR_RETURN
         (_tsce_phy_firmware_lane_config_set(phy, fw_config));
    /*Hold the per lne soft reset bit*/
    for (i = 0; i < num_lane; i++) {
        if (!PHYMOD_LANEPBMP_MEMBER(phy->access.lane_mask, start_lane + i)) {
            continue;
        }
        phy_copy.access.lane_mask = 1 << (start_lane + i);
        PHYMOD_IF_ERR_RETURN
            (eagle_lane_soft_reset_release(&phy_copy.access, 1));
    }

    PHYMOD_IF_ERR_RETURN
        (temod_trigger_speed_change(&phy->access));

    return PHYMOD_E_NONE; 
}


int tsce_phy_firmware_lane_config_get(const phymod_phy_access_t* phy, phymod_firmware_lane_config_t* fw_config)
{
    struct eagle_tsc_uc_lane_config_st serdes_firmware_config;
    PHYMOD_MEMSET(&serdes_firmware_config, 0x0, sizeof(serdes_firmware_config));

    PHYMOD_IF_ERR_RETURN(eagle_tsc_get_uc_lane_cfg(&phy->access, &serdes_firmware_config));
    PHYMOD_MEMSET(fw_config, 0, sizeof(*fw_config));
    fw_config->LaneConfigFromPCS = serdes_firmware_config.field.lane_cfg_from_pcs;
    fw_config->AnEnabled         = serdes_firmware_config.field.an_enabled;
    fw_config->DfeOn             = serdes_firmware_config.field.dfe_on;
    fw_config->ForceBrDfe        = serdes_firmware_config.field.force_brdfe_on;
    fw_config->Cl72AutoPolEn     = serdes_firmware_config.field.cl72_auto_polarity_en;
    fw_config->Cl72RestTO        = serdes_firmware_config.field.cl72_restart_timeout_en;
    fw_config->ScramblingDisable = serdes_firmware_config.field.scrambling_dis;
    fw_config->UnreliableLos     = serdes_firmware_config.field.unreliable_los;
    fw_config->MediaType         = serdes_firmware_config.field.media_type;
    fw_config->Cl72AutoPolEn     = serdes_firmware_config.field.cl72_auto_polarity_en;
    fw_config->Cl72RestTO        = serdes_firmware_config.field.cl72_restart_timeout_en;
     
    return PHYMOD_E_NONE; 
}

/* reset pll sequencer
 * flags - unused parameter
 */
int tsce_core_pll_sequencer_restart(const phymod_core_access_t* core, uint32_t flags, phymod_sequencer_operation_t operation)
{
    switch (operation) {
        case phymodSeqOpStop:
            PHYMOD_IF_ERR_RETURN
                (temod_pll_sequencer_control(&core->access, 0));
        break;
        case phymodSeqOpStart:
            PHYMOD_IF_ERR_RETURN
                (temod_pll_sequencer_control(&core->access, 1));
            
            /* PHYMOD_IF_ERR_RETURN
                (temod_pll_lock_wait(&core->access, 250000)); */
        break;
        case phymodSeqOpRestart:
            PHYMOD_IF_ERR_RETURN
                (temod_pll_sequencer_control(&core->access, 0));
            
            PHYMOD_IF_ERR_RETURN
                (temod_pll_sequencer_control(&core->access, 1));
            
            /* PHYMOD_IF_ERR_RETURN
                (temod_pll_lock_wait(&core->access, 250000)); */
        break;
        default:
            return PHYMOD_E_UNAVAIL;
        break;
    }     
    return PHYMOD_E_NONE;
}

int tsce_core_wait_event(const phymod_core_access_t* core, phymod_core_event_t event, uint32_t timeout)
{
    switch(event){
    case phymodCoreEventPllLock: 
        /* PHYMOD_IF_ERR_RETURN(temod_pll_lock_wait(&core->access, timeout)); */
        break;
    default:
        PHYMOD_RETURN_WITH_ERR(PHYMOD_E_CONFIG, (_PHYMOD_MSG("illegal wait event %u"), event));
    }
    return PHYMOD_E_NONE;
}

int tsce_phy_pll_multiplier_get(const phymod_phy_access_t* core, uint32_t* pll_multiplier)
{
    return PHYMOD_E_NONE;
}


/* reset rx sequencer 
 * flags - unused parameter
 */
int tsce_phy_rx_restart(const phymod_phy_access_t* phy)
{
    PHYMOD_IF_ERR_RETURN(eagle_tsc_rx_restart(&phy->access, 1)); 
    return PHYMOD_E_NONE;
}


int tsce_phy_polarity_set(const phymod_phy_access_t* phy, const phymod_polarity_t* polarity)
{
    PHYMOD_IF_ERR_RETURN
        (temod_tx_rx_polarity_set(&phy->access, polarity->tx_polarity, polarity->rx_polarity));
    return PHYMOD_E_NONE;
}


int tsce_phy_polarity_get(const phymod_phy_access_t* phy, phymod_polarity_t* polarity)
{
   PHYMOD_IF_ERR_RETURN
        (temod_tx_rx_polarity_get(&phy->access, &polarity->tx_polarity, &polarity->rx_polarity));
    return PHYMOD_E_NONE;
}

STATIC
int _tsce_phy_drivermode_phymod_to_tier1(int8_t phymod_drivermode, int8_t* drivermode)
{
    switch (phymod_drivermode) {
        case phymodTxDriverModeDefault:
             *drivermode = DM_DEFAULT;
             break;
        case phymodTxDriverModeHalfAmpHiImped:
             *drivermode = DM_HALF_AMPLITUDE_HI_IMPED;
             break;
        case phymodTxDriverModeHalfAmp:
             *drivermode = DM_HALF_AMPLITUDE;
             break;
        case phymodTxDriverModeNotSupported:
             *drivermode = DM_NOT_SUPPORTED;
             break;
        default:
             return PHYMOD_E_PARAM;
    }
    return PHYMOD_E_NONE;

}

STATIC
int _tsce_phy_drivermode_tier1_to_phymod(int8_t drivermode, int8_t* phymod_drivermode)
{
    switch (drivermode) {
        case DM_DEFAULT:
             *phymod_drivermode = phymodTxDriverModeDefault;
             break;
        case DM_HALF_AMPLITUDE_HI_IMPED:
             *phymod_drivermode = phymodTxDriverModeHalfAmpHiImped;
             break;
        case DM_HALF_AMPLITUDE:
             *phymod_drivermode = phymodTxDriverModeHalfAmp;
             break;
        case DM_NOT_SUPPORTED:
             *phymod_drivermode = phymodTxDriverModeNotSupported;
             break;
        default:
             return PHYMOD_E_INTERNAL;
    }
    return PHYMOD_E_NONE;

}

int tsce_phy_tx_set(const phymod_phy_access_t* phy, const phymod_tx_t* tx)
{
    int8_t drivermode;

    
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
    if((tx->drivermode != -1) && 
       (phy->device_op_mode & PHYMOD_INTF_CONFIG_TX_FIR_DRIVERMODE_ENABLE)) {
        PHYMOD_IF_ERR_RETURN
            (_tsce_phy_drivermode_phymod_to_tier1(tx->drivermode, &drivermode));
        PHYMOD_IF_ERR_RETURN
            (eagle_tsc_write_tx_afe(&phy->access, TX_AFE_DRIVERMODE, drivermode));
    }
    return PHYMOD_E_NONE;
}

int tsce_phy_tx_get(const phymod_phy_access_t* phy, phymod_tx_t* tx)
{
    int8_t drivermode = 0;
    int8_t value = 0;

    PHYMOD_IF_ERR_RETURN
        (eagle_tsc_read_tx_afe(&phy->access, TX_AFE_PRE, &value));
    tx->pre = value;
    PHYMOD_IF_ERR_RETURN
        (eagle_tsc_read_tx_afe(&phy->access, TX_AFE_MAIN, &value));
    tx->main = value;
    PHYMOD_IF_ERR_RETURN
        (eagle_tsc_read_tx_afe(&phy->access, TX_AFE_POST1, &value));
    tx->post = value;
    PHYMOD_IF_ERR_RETURN
        (eagle_tsc_read_tx_afe(&phy->access, TX_AFE_POST2, &value));
    tx->post2 = value;
    PHYMOD_IF_ERR_RETURN
        (eagle_tsc_read_tx_afe(&phy->access, TX_AFE_POST3, &value));
    tx->post3 = value;
    PHYMOD_IF_ERR_RETURN
        (eagle_tsc_read_tx_afe(&phy->access, TX_AFE_AMP, &value));
    tx->amp = value;
    PHYMOD_IF_ERR_RETURN
        (eagle_tsc_read_tx_afe(&phy->access, TX_AFE_DRIVERMODE, &drivermode));
    PHYMOD_IF_ERR_RETURN
        (_tsce_phy_drivermode_tier1_to_phymod(drivermode, &value));
    tx->drivermode = value;

    return PHYMOD_E_NONE;
}

int tsce_phy_tx_override_set(const phymod_phy_access_t* phy, const phymod_tx_override_t* tx_override)
{
    PHYMOD_IF_ERR_RETURN
        (eagle_tsc_tx_pi_freq_override(&phy->access,
                                    tx_override->phase_interpolator.enable,
                                    tx_override->phase_interpolator.value));    
    return PHYMOD_E_NONE;
}

int tsce_phy_tx_override_get(const phymod_phy_access_t* phy, phymod_tx_override_t* tx_override)
{  
    int16_t pi_value;

    PHYMOD_IF_ERR_RETURN
        (eagle_tx_pi_control_get(&phy->access, &pi_value));

    tx_override->phase_interpolator.value = (int32_t) pi_value;

    return PHYMOD_E_NONE;
}


int tsce_phy_rx_set(const phymod_phy_access_t* phy, const phymod_rx_t* rx)
{
    uint32_t i;
    uint8_t uc_lane_stopped;
    phymod_phy_access_t pm_phy_copy;
    int start_lane, num_lane, k;

    PHYMOD_MEMCPY(&pm_phy_copy, phy, sizeof(pm_phy_copy));
    /* next program the tx fir taps and driver current based on the input */
    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(&phy->access, &start_lane, &num_lane));

    /* params check */
    if((rx->num_of_dfe_taps == 0) || (rx->num_of_dfe_taps > TSCE_NOF_DFES)){
        PHYMOD_RETURN_WITH_ERR(PHYMOD_E_CONFIG, (_PHYMOD_MSG("illegal number of DFEs to set %u"), (unsigned int)rx->num_of_dfe_taps));
    }

    for (k = 0; k < num_lane; k++) {
        if (!PHYMOD_LANEPBMP_MEMBER(phy->access.lane_mask, start_lane + k)) {
            continue;
        }
        pm_phy_copy.access.lane_mask = 1 << (start_lane + k);

        /*vga set*/
        /* first check if uc lane is stopped already */
        PHYMOD_IF_ERR_RETURN(eagle_tsc_stop_uc_lane_status(&pm_phy_copy.access, (uint8_t *) &uc_lane_stopped));
        if (!uc_lane_stopped) {
            PHYMOD_IF_ERR_RETURN(eagle_tsc_stop_rx_adaptation(&pm_phy_copy.access, 1));
        }
        PHYMOD_IF_ERR_RETURN(eagle_tsc_stop_rx_adaptation(&pm_phy_copy.access, 1));
        PHYMOD_IF_ERR_RETURN(eagle_tsc_write_rx_afe(&pm_phy_copy.access, RX_AFE_VGA, rx->vga.value));
        /* dfe set */
        for (i = 0 ; i < rx->num_of_dfe_taps ; i++){
            switch (i) {
                case 0:
                    PHYMOD_IF_ERR_RETURN(eagle_tsc_write_rx_afe(&phy->access, RX_AFE_DFE1, rx->dfe[i].value));
                    break;
                case 1:
                    PHYMOD_IF_ERR_RETURN(eagle_tsc_write_rx_afe(&phy->access, RX_AFE_DFE2, rx->dfe[i].value));
                    break;
                case 2:
                    PHYMOD_IF_ERR_RETURN(eagle_tsc_write_rx_afe(&phy->access, RX_AFE_DFE3, rx->dfe[i].value));
                    break;
                case 3:
                    PHYMOD_IF_ERR_RETURN(eagle_tsc_write_rx_afe(&phy->access, RX_AFE_DFE4, rx->dfe[i].value));
                    break;
                case 4:
                    PHYMOD_IF_ERR_RETURN(eagle_tsc_write_rx_afe(&phy->access, RX_AFE_DFE5, rx->dfe[i].value));
                    break;
                default:
                    return PHYMOD_E_PARAM;
            }
        }
        /*peaking filter set*/
        PHYMOD_IF_ERR_RETURN(eagle_tsc_write_rx_afe(&pm_phy_copy.access, RX_AFE_PF, rx->peaking_filter.value));

        /* low freq peak filter */
        PHYMOD_IF_ERR_RETURN(eagle_tsc_write_rx_afe(&pm_phy_copy.access, RX_AFE_PF2, (int8_t)rx->low_freq_peaking_filter.value));
     
    }
    return PHYMOD_E_NONE;
}


int tsce_phy_rx_get(const phymod_phy_access_t* phy, phymod_rx_t* rx)
{
    int8_t tmpData;

    PHYMOD_IF_ERR_RETURN
        (eagle_tsc_read_rx_afe(&phy->access, RX_AFE_PF, &tmpData));
    rx->peaking_filter.value = tmpData;
    PHYMOD_IF_ERR_RETURN
        (eagle_tsc_read_rx_afe(&phy->access, RX_AFE_PF2, &tmpData));
    rx->low_freq_peaking_filter.value = tmpData;
    PHYMOD_IF_ERR_RETURN
        (eagle_tsc_read_rx_afe(&phy->access, RX_AFE_VGA,&tmpData));
    rx->vga.value = tmpData;
    PHYMOD_IF_ERR_RETURN
        (eagle_tsc_read_rx_afe(&phy->access, RX_AFE_DFE1,&tmpData));
    rx->dfe[0].value = tmpData;
    PHYMOD_IF_ERR_RETURN
        (eagle_tsc_read_rx_afe(&phy->access, RX_AFE_DFE2,&tmpData));
    rx->dfe[1].value = tmpData;
    PHYMOD_IF_ERR_RETURN
        (eagle_tsc_read_rx_afe(&phy->access, RX_AFE_DFE3, &tmpData));
    rx->dfe[2].value = tmpData;
    PHYMOD_IF_ERR_RETURN
        (eagle_tsc_read_rx_afe(&phy->access, RX_AFE_DFE4,&tmpData));
    rx->dfe[3].value = tmpData;
    PHYMOD_IF_ERR_RETURN
        (eagle_tsc_read_rx_afe(&phy->access, RX_AFE_DFE5,&tmpData));
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


int tsce_phy_reset_set(const phymod_phy_access_t* phy, const phymod_phy_reset_t* reset)
{
        
        
    
    /* Place your code here */

        
    return PHYMOD_E_NONE;
        
    
}


int tsce_phy_reset_get(const phymod_phy_access_t* phy, phymod_phy_reset_t* reset)
{
        
        
    
    /* Place your code here */

        
    return PHYMOD_E_NONE;
        
    
}


int tsce_phy_power_set(const phymod_phy_access_t* phy, const phymod_phy_power_t* power)
{
    phymod_phy_access_t pm_phy_copy;
    int start_lane, num_lane, i;

    PHYMOD_MEMCPY(&pm_phy_copy, phy, sizeof(pm_phy_copy));
    /* next program the tx fir taps and driver current based on the input */
    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(&phy->access, &start_lane, &num_lane));

    if ((power->tx == phymodPowerOff) && (power->rx == phymodPowerOff)) {
        for (i = 0; i < num_lane; i++) {
            if (!PHYMOD_LANEPBMP_MEMBER(phy->access.lane_mask, start_lane + i)) {
                continue;
            }
            pm_phy_copy.access.lane_mask = 1 << (start_lane + i);
            PHYMOD_IF_ERR_RETURN(temod_port_enable_set(&pm_phy_copy.access, 0));
        }
    }
    if ((power->tx == phymodPowerOn) && (power->rx == phymodPowerOn)) {
        for (i = 0; i < num_lane; i++) {
            if (!PHYMOD_LANEPBMP_MEMBER(phy->access.lane_mask, start_lane + i)) {
                continue;
            }
            pm_phy_copy.access.lane_mask = 1 << (start_lane + i);
            PHYMOD_IF_ERR_RETURN(temod_port_enable_set(&pm_phy_copy.access, 1));
        }
    }
    if ((power->tx == phymodPowerOff) && (power->rx == phymodPowerNoChange)) {
            /* disable tx on the PMD side */
            PHYMOD_IF_ERR_RETURN(eagle_tsc_tx_disable(&phy->access, 1));
    }
    if ((power->tx == phymodPowerOn) && (power->rx == phymodPowerNoChange)) {
            /* enable tx on the PMD side */
            PHYMOD_IF_ERR_RETURN(eagle_tsc_tx_disable(&phy->access, 0));
    }
    if ((power->tx == phymodPowerNoChange) && (power->rx == phymodPowerOff)) {
            /* disable rx on the PMD side */
            PHYMOD_IF_ERR_RETURN(temod_rx_squelch_set(&phy->access, 1));
    }
    if ((power->tx == phymodPowerNoChange) && (power->rx == phymodPowerOn)) {
            /* enable rx on the PMD side */
            PHYMOD_IF_ERR_RETURN(temod_rx_squelch_set(&phy->access, 0));
    }
    return PHYMOD_E_NONE; 
}

int tsce_phy_power_get(const phymod_phy_access_t* phy, phymod_phy_power_t* power)
{
    int enable;
    uint32_t lb_enable;
    phymod_phy_access_t pm_phy_copy;
    int start_lane, num_lane;

    PHYMOD_MEMCPY(&pm_phy_copy, phy, sizeof(pm_phy_copy));
    /* next program the tx fir taps and driver current based on the input */
    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(&phy->access, &start_lane, &num_lane));

    pm_phy_copy.access.lane_mask = 0x1 << start_lane;

    PHYMOD_IF_ERR_RETURN(temod_rx_squelch_get(&pm_phy_copy.access, &enable));

    /* next check if PMD loopback is on */ 
    if (enable) {                           
        PHYMOD_IF_ERR_RETURN(eagle_pmd_loopback_get(&pm_phy_copy.access, &lb_enable));
        if (lb_enable) enable = 0;
    }

    power->rx = (enable == 1)? phymodPowerOff: phymodPowerOn;
    /* Commented the following line. Because if in PMD loopback mode, we squelch the
           xmit, and we should still see the correct port status */
    /* PHYMOD_IF_ERR_RETURN(temod_tx_squelch_get(&pm_phy_copy.access, &enable)); */
    power->tx = (enable == 1)? phymodPowerOff: phymodPowerOn;
    return PHYMOD_E_NONE;
}

int tsce_phy_tx_lane_control_set(const phymod_phy_access_t* phy, phymod_phy_tx_lane_control_t tx_control)
{   
    switch (tx_control) {
    case phymodTxTrafficDisable:
        PHYMOD_IF_ERR_RETURN(temod_tx_lane_control_set(&phy->access, TEMOD_TX_LANE_TRAFFIC_DISABLE));
        break;
    case phymodTxTrafficEnable:
        PHYMOD_IF_ERR_RETURN(temod_tx_lane_control_set(&phy->access, TEMOD_TX_LANE_TRAFFIC_ENABLE));
        break;
    case phymodTxReset:
        PHYMOD_IF_ERR_RETURN(temod_tx_lane_control_set(&phy->access, TEMOD_TX_LANE_RESET));
        break;
    case phymodTxSquelchOn:
        PHYMOD_IF_ERR_RETURN(temod_tx_squelch_set(&phy->access, 1));
        break;
    case phymodTxSquelchOff:
        PHYMOD_IF_ERR_RETURN(temod_tx_squelch_set(&phy->access, 0));
        break;
    default:
        break;
    }
    return PHYMOD_E_NONE;
}

int tsce_phy_tx_lane_control_get(const phymod_phy_access_t* phy, phymod_phy_tx_lane_control_t* tx_control)
{   
    int enable, reset, tx_lane;
    uint32_t lb_enable;
    phymod_phy_access_t pm_phy_copy;
    int start_lane, num_lane;

    PHYMOD_MEMCPY(&pm_phy_copy, phy, sizeof(pm_phy_copy));
    /* next program the tx fir taps and driver current based on the input */
    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(&phy->access, &start_lane, &num_lane));

    pm_phy_copy.access.lane_mask = 0x1 << start_lane;

    PHYMOD_IF_ERR_RETURN(temod_tx_squelch_get(&pm_phy_copy.access, &enable));

    /* next check if PMD loopback is on */ 
    if (enable) {                           
        PHYMOD_IF_ERR_RETURN(eagle_pmd_loopback_get(&pm_phy_copy.access, &lb_enable));
        if (lb_enable) enable = 0;
    }

    if(enable) {
        *tx_control = phymodTxSquelchOn;
    } else {
        PHYMOD_IF_ERR_RETURN(temod_tx_lane_control_get(&pm_phy_copy.access, &reset, &tx_lane));
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


int tsce_phy_rx_lane_control_set(const phymod_phy_access_t* phy, phymod_phy_rx_lane_control_t rx_control)
{   
    phymod_phy_access_t pm_phy_copy;
    int start_lane, num_lane, i;

    PHYMOD_MEMCPY(&pm_phy_copy, phy, sizeof(pm_phy_copy));
    /* next program the tx fir taps and driver current based on the input */
    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(&phy->access, &start_lane, &num_lane));

    pm_phy_copy.access.lane_mask = 0x1 << start_lane;

    switch (rx_control) {
    case phymodRxReset:
        PHYMOD_IF_ERR_RETURN(temod_rx_lane_control_set(&phy->access, 1));
        break;
    case phymodRxDisable:
        PHYMOD_IF_ERR_RETURN(temod_rx_lane_control_set(&phy->access, 0));
        break;
    case phymodRxSquelchOn:
        for (i = 0; i < num_lane; i++) {
            if (!PHYMOD_LANEPBMP_MEMBER(phy->access.lane_mask, start_lane + i)) {
                continue;
            }
            pm_phy_copy.access.lane_mask = 1 << (start_lane + i);
            PHYMOD_IF_ERR_RETURN(temod_rx_squelch_set(&pm_phy_copy.access, 1));
        }
        break;
    case phymodRxSquelchOff:
        for (i = 0; i < num_lane; i++) {
            if (!PHYMOD_LANEPBMP_MEMBER(phy->access.lane_mask, start_lane + i)) {
                continue;
            }
            pm_phy_copy.access.lane_mask = 1 << (start_lane + i);
            PHYMOD_IF_ERR_RETURN(temod_rx_squelch_set(&pm_phy_copy.access, 0));
        }
        break;
    default:
        break;
    }
    return PHYMOD_E_NONE;
}

int tsce_phy_rx_lane_control_get(const phymod_phy_access_t* phy, phymod_phy_rx_lane_control_t* rx_control)
{   
    int enable, reset;
    uint32_t lb_enable;
    phymod_phy_access_t pm_phy_copy;
    int start_lane, num_lane;

    PHYMOD_MEMCPY(&pm_phy_copy, phy, sizeof(pm_phy_copy));
    /* next program the tx fir taps and driver current based on the input */
    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(&phy->access, &start_lane, &num_lane));

    pm_phy_copy.access.lane_mask = 0x1 << start_lane;

    PHYMOD_IF_ERR_RETURN(temod_rx_squelch_get(&pm_phy_copy.access, &enable));
    /* next check if PMD loopback is on */ 
    if (enable) {                           
        PHYMOD_IF_ERR_RETURN(eagle_pmd_loopback_get(&pm_phy_copy.access, &lb_enable));
        if (lb_enable) enable = 0;
    }
    if(enable) {
        *rx_control = phymodRxSquelchOn;
    } else {
        PHYMOD_IF_ERR_RETURN(temod_rx_lane_control_get(&pm_phy_copy.access, &reset));
        if (reset == 0) {
            *rx_control = phymodRxReset;
        } else {
            *rx_control = phymodRxSquelchOff;
        }
    }
    return PHYMOD_E_NONE;
}

int tsce_phy_fec_enable_set(const phymod_phy_access_t* phy, uint32_t enable)
{
    int i, start_lane, num_lane;
    phymod_phy_access_t phy_copy;
    if ((phy->access.lane_mask == 0xf) ||
        (phy->access.lane_mask == 0xc) ||
        (phy->access.lane_mask == 0x3)) {
        PHYMOD_IF_ERR_RETURN(temod_fecmode_set(&phy->access, enable));
    } else {
        PHYMOD_IF_ERR_RETURN
            (phymod_util_lane_config_get(&phy->access, &start_lane, &num_lane));
        PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));

        for (i = 0; i < num_lane; i++) {
            if (!PHYMOD_LANEPBMP_MEMBER(phy->access.lane_mask, start_lane + i)) {
                continue;
            }
            phy_copy.access.lane_mask = 1 << (start_lane + i);
            PHYMOD_IF_ERR_RETURN(temod_fecmode_set(&phy_copy.access, enable));
        }
    }
    return PHYMOD_E_NONE;

}

int tsce_phy_fec_enable_get(const phymod_phy_access_t* phy, uint32_t* enable)
{
    PHYMOD_IF_ERR_RETURN(temod_fecmode_get(&phy->access, enable));
    return PHYMOD_E_NONE;

}

STATIC
int _tsce_pll_multiplier_get(uint32_t pll_div, uint32_t *pll_multiplier)
{
    switch (pll_div) {
    case 0x0:
        *pll_multiplier = 46;
        break;
    case 0x1:
        *pll_multiplier = 72;
        break;
    case TEMOD_PLL_MODE_DIV_40:
        *pll_multiplier = 40;
        break;
    case TEMOD_PLL_MODE_DIV_42:
        *pll_multiplier = 42;
        break;
    case TEMOD_PLL_MODE_DIV_48:
        *pll_multiplier = 48;
        break;
    case 0x5:
        *pll_multiplier = 50;
        break;
    case TEMOD_PLL_MODE_DIV_52:
        *pll_multiplier = 52;
        break;
    case TEMOD_PLL_MODE_DIV_54:
        *pll_multiplier = 54;
        break;
    case TEMOD_PLL_MODE_DIV_60:
        *pll_multiplier = 60;
        break;
    case TEMOD_PLL_MODE_DIV_64:
        *pll_multiplier = 64;
        break;
    case TEMOD_PLL_MODE_DIV_66:
        *pll_multiplier = 66;
        break;
    case 0xb:
        *pll_multiplier = 68;
        break;
    case TEMOD_PLL_MODE_DIV_70:
        *pll_multiplier = 70;
        break;
    case TEMOD_PLL_MODE_DIV_80:
        *pll_multiplier = 80;
        break;
    case TEMOD_PLL_MODE_DIV_92:
        *pll_multiplier = 92;
        break;
    case 0xf:
        *pll_multiplier = 100;
        break;
    case TEMOD_PLL_MODE_DIV_82P5:
        *pll_multiplier = 82;
        break;
    case TEMOD_PLL_MODE_DIV_87P5:
        *pll_multiplier = 87;
        break;
    default:
        *pll_multiplier = 66;
        break;
    }
    return PHYMOD_E_NONE;
}

STATIC
int _tsce_st_hto_interface_config_set(const temod_device_aux_modes_t *device, int lane_num, int speed_vec, uint32_t *new_pll_div, int16_t *os_mode)
{
    int i, max_lane=4 ;
    *os_mode = -1 ;
    /* ST */
    for(i=0;i<max_lane; i++) {
        if(device->st_hcd[i]==speed_vec){
            *new_pll_div = device->st_pll_div[i];
            *os_mode     = device->st_os[i] ;
            break ;
        }
    }
    /* HTO */
    if(device->hto_enable[lane_num]){
        *new_pll_div = device->hto_pll_div[lane_num];
        *os_mode     = device->hto_os[lane_num] ;
    }
    return PHYMOD_E_NONE;
} 


int tsce_phy_interface_config_set(const phymod_phy_access_t* phy, uint32_t flags, const phymod_phy_inf_config_t* config)
{  
    uint32_t current_pll_div=0;
    uint32_t new_pll_div=0;
    uint16_t new_speed_vec=0;
    int16_t  new_os_mode =-1;
    temod_spd_intfc_type spd_intf = TEMOD_SPD_ILLEGAL;
    phymod_phy_access_t pm_phy_copy;
    int start_lane=0, num_lane, i, pll_switch = 0;
    uint32_t os_dfeon=0;
    uint32_t scrambling_dis=0;
    uint32_t u_os_mode = 0;
    int      dfe_adjust = -1 ; 
    int lane_bkup; 
    uint32_t pll_multiplier, vco_rate;
    enum eagle_tsc_pll_enum new_pll_div_enum;
    int cl72_allowed=0, cl72_req=0, high_vco_1G = 1;
    uint32_t mode_flip=0;
    phymod_phy_inf_config_t temp_config;
    temod_pll_mode_type pll_mode;
    uint16_t or_val=0;
    int rv = PHYMOD_E_NONE;

    /* sc_table_entry exp_entry; RAVI */
    phymod_firmware_lane_config_t firmware_lane_config;  
    phymod_firmware_core_config_t firmware_core_config;

    PHYMOD_MEMCPY(&pm_phy_copy, phy, sizeof(pm_phy_copy));
    PHYMOD_MEMSET(&firmware_lane_config, 0x0, sizeof(firmware_lane_config));
    PHYMOD_MEMSET(&firmware_core_config, 0x0, sizeof(firmware_core_config));
    PHYMOD_MEMSET(&temp_config, 0x0, sizeof(temp_config));

    /* special mode */
    if(flags&PHYMOD_INTF_F_SET_CORE_MAP_MODE) {
        int pmode = -1;
        /* 100g/120g port */

        /* mode flip, in TSC12 odd number ports are reverse.
           So it need to be swapped between 244 and 442 mode.
           343 is fine as those are symmertic. */
        if( flags & PHYMOD_INTF_F_CORE_MAP_MODE_FLIP) {
           mode_flip = 1; 
        }

        if(PHYMOD_INTF_MODES_TRIPLE_CORE_GET(config)) {
             
            if (PHYMOD_INTF_MODES_TC_442_GET(config)) {
                if(mode_flip){
                    pmode = 1;
                } else {
                    pmode = 2 ;
                }
            }
            else if (PHYMOD_INTF_MODES_TC_244_GET(config)){ 
                if(mode_flip) {
                    pmode = 2 ;
                } else {
                    pmode = 1 ;
                }
            }
            else if (PHYMOD_INTF_MODES_TC_343_GET(config))
                pmode=0;

            if(config->data_rate>=120000) {
                PHYMOD_IF_ERR_RETURN
                    (temod_tsc12_control(&pm_phy_copy.access, 2, pmode));
            } else if(config->data_rate>=100000) {
                PHYMOD_IF_ERR_RETURN
                    (temod_tsc12_control(&pm_phy_copy.access, 1, pmode));
            } else {
                PHYMOD_IF_ERR_RETURN
                    (temod_tsc12_control(&pm_phy_copy.access, 0, pmode));
            }
        } else {
            (temod_tsc12_control(&pm_phy_copy.access, 0, 0));
        }
        return PHYMOD_E_NONE;
    } 
    if (flags & PHYMOD_INTF_F_SET_SPD_TRIGGER) {
        if (config->data_rate >= 100000) {
            switch(config->data_rate) {
            case 100000: spd_intf = TEMOD_SPD_100G_CR10;    break ;
            case 106000: 
            case 107000: spd_intf = TEMOD_SPD_107G_HG_CR10; break ;
            case 120000: spd_intf = TEMOD_SPD_120G_CR12;    break ;
            case 127000: spd_intf = TEMOD_SPD_127G_HG_CR12; break ;
            }
            PHYMOD_IF_ERR_RETURN
                (temod_set_spd_intf(&phy->access, spd_intf, 0));
        }
        return PHYMOD_E_NONE;
    }
    if (flags & PHYMOD_INTF_F_SET_SPD_DISABLE) {
        if (config->data_rate >= 100000) {
            pm_phy_copy.access.lane_mask =0xf;
            num_lane = 0x4;
            start_lane = 0;
            PHYMOD_IF_ERR_RETURN
                (temod_disable_set(&pm_phy_copy.access));
            /* next set the firmware lane config properly */
            /* Hold the per lne soft reset bit */
            for (i = 0; i < num_lane; i++) {
                pm_phy_copy.access.lane_mask = 1 << (start_lane + i);
                PHYMOD_IF_ERR_RETURN
                    (eagle_lane_soft_reset_release(&pm_phy_copy.access, 0));
            }
            pm_phy_copy.access.lane_mask = 0x1 << start_lane;
            PHYMOD_IF_ERR_RETURN
               (tsce_phy_firmware_lane_config_get(&pm_phy_copy, &firmware_lane_config));

            if (PHYMOD_INTF_MODES_FIBER_GET(config)) {
                firmware_lane_config.MediaType = phymodFirmwareMediaTypeOptics;                    
            } else {
                if (config->interface_type == phymodInterfaceCR10) {
                    firmware_lane_config.MediaType = phymodFirmwareMediaTypeCopperCable;
                } else {
                    firmware_lane_config.MediaType = phymodFirmwareMediaTypePcbTraceBackPlane;
                }
            }    
          
            for (i = 0; i < num_lane; i++) {
                pm_phy_copy.access.lane_mask = 0x1 << (start_lane + i);
                PHYMOD_IF_ERR_RETURN
                     (_tsce_phy_firmware_lane_config_set(&pm_phy_copy, firmware_lane_config));
            }
    
            /* release the per lne soft reset bit */
            for (i = 0; i < num_lane; i++) {
                pm_phy_copy.access.lane_mask = 1 << (start_lane + i);
                PHYMOD_IF_ERR_RETURN
                    (eagle_lane_soft_reset_release(&pm_phy_copy.access, 1));
            }
        }
        return PHYMOD_E_NONE;
    }
    /* end of special mode */

    if (flags & PHYMOD_INTF_F_SPEED_CONFIG_CLEAR) {
        
        /* disable speed change trigger. */
        PHYMOD_IF_ERR_RETURN
            (temod_disable_set(&phy->access));
 
        /* disable cl82_multi_pipe_mode, cl82_mld_phys_map to 
           HW default 343 mode. */
        PHYMOD_IF_ERR_RETURN 
            (temod_tsc12_control(&phy->access, 0, 0));

        return PHYMOD_E_NONE;
    }

    /* end of special mode */
    firmware_lane_config.MediaType = 0;

    /* next program the tx fir taps and driver current based on the input */
    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(&phy->access, &start_lane, &num_lane));

    /* reset pcs */
    temod_disable_set(&phy->access);

    /* Hold the per lne soft reset bit */
    for (i = 0; i < num_lane; i++) {
        if (!PHYMOD_LANEPBMP_MEMBER(phy->access.lane_mask, start_lane + i)) {
            continue;
        }
        pm_phy_copy.access.lane_mask = 1 << (start_lane + i);
        PHYMOD_IF_ERR_RETURN
            (eagle_lane_soft_reset_release(&pm_phy_copy.access, 0));
    }
    /* deassert pmd_tx_disable_pin_dis if it is set by ILKn */
    /* remove pmd_tx_disable_pin_dis it may be asserted because of ILKn */
    if(config->interface_type != phymodInterfaceBypass) {
        for (i = 0; i < num_lane; i++) {
            if (!PHYMOD_LANEPBMP_MEMBER(phy->access.lane_mask, start_lane + i)) {
                continue;
            }
            pm_phy_copy.access.lane_mask = 1 << (start_lane + i);
            PHYMOD_IF_ERR_RETURN
              (eagle_pmd_tx_disable_pin_dis_set(&phy->access, 0));    
        }
    } 
    /* disable CL72 */
    for (i = 0; i < num_lane; i++) {
        if (!PHYMOD_LANEPBMP_MEMBER(phy->access.lane_mask, start_lane + i)) {
            continue;
        }
        pm_phy_copy.access.lane_mask = 1 << (start_lane + i);
        PHYMOD_IF_ERR_RETURN
            (temod_clause72_control(&pm_phy_copy.access, 0|TEMOD_CL72_CONTROL_NO_TRIGER)) ;
    }

    pm_phy_copy.access.lane_mask = 0x1 << start_lane;
     PHYMOD_IF_ERR_RETURN
        (tsce_phy_firmware_lane_config_get(&pm_phy_copy, &firmware_lane_config)); 

    /* make sure that an and config from pcs is off */
    firmware_core_config.CoreConfigFromPCS = 0;
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

    PHYMOD_IF_ERR_RETURN
        (temod_update_port_mode(&phy->access, (int *) &pll_switch));

    spd_intf = TEMOD_SPD_10_X1_SGMII; /* to prevent undefinded TEMOD_SPD_ILLEGAL accessing tables */

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
    
    cl72_req = (flags & (PHYMOD_INTF_F_CL72_REQUESTED_BY_CNFG|PHYMOD_INTF_F_CL72_REQUESTED_BY_API))?1:0;
    switch(config->data_rate) {
    case 10:
        if(config->pll_divider_req==40) {
            spd_intf = TEMOD_SPD_10_SGMII;
            high_vco_1G = 0;
        } else {
            spd_intf = TEMOD_SPD_10_X1_SGMII;
        }
        break;
    case 100:
        if(config->pll_divider_req==40) {
            spd_intf = TEMOD_SPD_100_SGMII;
            high_vco_1G = 0;
        } else {
            spd_intf = TEMOD_SPD_100_X1_SGMII;
        }
        break;
    case 1000:
        if(config->pll_divider_req==40) {
            spd_intf = TEMOD_SPD_1000_SGMII;
            high_vco_1G = 0;
        } else {
            spd_intf = TEMOD_SPD_1000_X1_SGMII;
        }
        break;
    case 2500:
        if(config->pll_divider_req==40) {
            spd_intf = TEMOD_SPD_2500;
            high_vco_1G = 0;
        } else {
            spd_intf = TEMOD_SPD_2500_X1;
        }
        break;
    case 5000:
        if (config->interface_type == phymodInterfaceRXAUI) {
            temod_credit_override_set(&phy->access, TEMOD_CREDIT_MAC, 0x4);
            new_os_mode =  TEMOD_PMA_OS_MODE_2;
            if (PHYMOD_INTF_MODES_SCR_GET(config)) {
               spd_intf = TEMOD_SPD_10000_DXGXS_SCR;
            } else {
               spd_intf = TEMOD_SPD_10000_DXGXS;
            }
        } else if (PHYMOD_INTF_MODES_HIGIG_GET(config)) {
            if(config->pll_divider_req==40) {
                spd_intf = TEMOD_SPD_5000;
            } else {
                spd_intf = TEMOD_SPD_5000_XFI;
            }
        } else {
            if(config->pll_divider_req==40) {
                spd_intf = TEMOD_SPD_5000;
            } else {
                spd_intf = TEMOD_SPD_5000_XFI;
            }
        }
        break;
    case 10000:
        if (config->interface_type == phymodInterfaceXAUI) {
            spd_intf = TEMOD_SPD_10000;
        } else if (config->interface_type == phymodInterfaceRXAUI) {
            /* There is no seperate speed Id defined for dual lane 10G
             * in HG mode. both HG/IEEE uses the same speed Id 
             */
            if (PHYMOD_INTF_MODES_SCR_GET(config)) {
                spd_intf = TEMOD_SPD_10000_DXGXS_SCR;
            } else {
                spd_intf = TEMOD_SPD_10000_DXGXS;
            }
          
        } else if(config->interface_type == phymodInterfaceX2) {
            spd_intf = TEMOD_SPD_10000_X2;
        } else {
            cl72_allowed = 1 ;
            if (config->interface_type == phymodInterfaceSFI) {
                spd_intf = TEMOD_SPD_10000_XFI;
                dfe_adjust = 0 ;
            } else if(config->interface_type == phymodInterfaceXFI) {
                spd_intf = TEMOD_SPD_10000_XFI;
                dfe_adjust = 0 ;
            } else if(config->interface_type == phymodInterfaceXGMII) {
                spd_intf = TEMOD_SPD_10000;
            } else if(config->interface_type == phymodInterfaceKR) {  
                spd_intf = TEMOD_SPD_10000_XFI;
                dfe_adjust = 1;
            } else {
                spd_intf = TEMOD_SPD_10000_XFI;
                if (PHYMOD_INTF_MODES_FIBER_GET(config)) {
                    dfe_adjust = 0 ;
                }
            }
        }
        break;
    case 10500:
		/* TEMOD_SPD_10500_HI is 4-lane port */
        if (PHYMOD_INTF_MODES_HIGIG_GET(config)) {
            spd_intf = TEMOD_SPD_10500_HI;
        } else {
      		spd_intf = TEMOD_SPD_10500_HI;    
        }
        break;
    case 11000:
        cl72_allowed = 1 ;
        if (PHYMOD_INTF_MODES_HIGIG_GET(config)) {
            spd_intf = TEMOD_SPD_10600_XFI_HG;
        } else {
            spd_intf = TEMOD_SPD_10000_XFI;
        }
        break;
    case 12000:
        if(config->interface_type == phymodInterfaceRXAUI) {
            spd_intf = TEMOD_SPD_12773_HI_DXGXS;
        } else {
            cl72_allowed = 1 ;
            spd_intf = TEMOD_SPD_10000_XFI;
            if ((config->interface_type == phymodInterfaceSFI) ||
                (config->interface_type == phymodInterfaceXFI)) {
                dfe_adjust = 0 ;
            }
        }
        break;
    case 13000:
        if(config->interface_type == phymodInterfaceRXAUI) {
            spd_intf = TEMOD_SPD_12773_DXGXS;
            /*
            if (PHYMOD_INTF_MODES_HIGIG_GET(config)) {
                spd_intf = TEMOD_SPD_12773_DXGXS;
            } else {
                spd_intf = TEMOD_SPD_12773_DXGXS;
            }
            */
        } else {
            spd_intf = TEMOD_SPD_13000;
        }
        break;
    case 15000:
        spd_intf = TEMOD_SPD_15000;
        break;
    case 15750:
        if (PHYMOD_INTF_MODES_HIGIG_GET(config)) {
            spd_intf = TEMOD_SPD_15750_HI_DXGXS;
        }
        break;
    case 16000:
        spd_intf = TEMOD_SPD_16000;
        break;
    case 20000:
        if(config->interface_type == phymodInterfaceKR2 ||
           config->interface_type == phymodInterfaceCR2) {
            cl72_allowed = 1 ;
            spd_intf = TEMOD_SPD_20G_MLD_DXGXS ;
        } else if(config->interface_type == phymodInterfaceRXAUI) {
            cl72_allowed = 1 ;
            spd_intf = TEMOD_SPD_20G_DXGXS;
        } else {
            if (PHYMOD_INTF_MODES_SCR_GET(config)) {
                spd_intf = TEMOD_SPD_20000_SCR;
            } else {
                spd_intf = TEMOD_SPD_20000;
            }
        }
        break;
    case 21000:
        if(config->interface_type == phymodInterfaceRXAUI) {
            spd_intf = TEMOD_SPD_20G_DXGXS;
        } else if(config->interface_type == phymodInterfaceKR2 ||
                  config->interface_type == phymodInterfaceCR2) {
            spd_intf = TEMOD_SPD_21G_HI_MLD_DXGXS; 
        } else {
            spd_intf = TEMOD_SPD_21000;
        }
        break;
    case 25000:
        if (num_lane == 2) {
            cl72_allowed = 1 ;
            spd_intf = TEMOD_SPD_20G_MLD_DXGXS ;
        } else {
            spd_intf = TEMOD_SPD_25455 ;
        }
        break;
    case 25450:
        spd_intf = TEMOD_SPD_25455 ;
        break;
    case 30000:
    case 32000:
        spd_intf = TEMOD_SPD_31500;
        break;
    case 40000:
        cl72_allowed = 1 ;
        spd_intf = TEMOD_SPD_40G_XLAUI;
        if (config->interface_type == phymodInterfaceXLAUI) {
            firmware_lane_config.MediaType = phymodFirmwareMediaTypePcbTraceBackPlane;
            dfe_adjust = 0 ;
        } else if(PHYMOD_INTF_MODES_FIBER_GET(config)) {
            firmware_lane_config.MediaType = phymodFirmwareMediaTypeOptics ;
             dfe_adjust = 0 ;
        } else if(config->interface_type == phymodInterfaceCR4) {
            firmware_lane_config.MediaType = phymodFirmwareMediaTypeCopperCable ;
        } else if (config->interface_type == phymodInterfaceKR4) {
            firmware_lane_config.MediaType = phymodFirmwareMediaTypePcbTraceBackPlane;
        } else if((config->interface_type == phymodInterfaceXGMII) || PHYMOD_INTF_MODES_HIGIG_GET(config)) {
            spd_intf = TEMOD_SPD_40G_X4;
        } else {
            firmware_lane_config.MediaType = phymodFirmwareMediaTypePcbTraceBackPlane ;
        }
        break;
    case 42000:
        cl72_allowed = 1 ;
        if ((config->interface_type == phymodInterfaceKR4) && 
            PHYMOD_INTF_MODES_HIGIG_GET(config)) {
            spd_intf = TEMOD_SPD_42G_XLAUI;
        }else if((config->interface_type == phymodInterfaceCR4) &&
            PHYMOD_INTF_MODES_HIGIG_GET(config)) {
            spd_intf = TEMOD_SPD_42G_XLAUI;
            firmware_lane_config.MediaType = phymodFirmwareMediaTypeCopperCable;
        }else if((config->interface_type == phymodInterfaceXGMII) || PHYMOD_INTF_MODES_HIGIG_GET(config)) {
            spd_intf = TEMOD_SPD_40G_X4;
        } else {
            spd_intf = TEMOD_SPD_42G_XLAUI;
        }
        break;
    case 48000:
        cl72_allowed = 1 ;
        spd_intf = TEMOD_SPD_40G_XLAUI;
        firmware_lane_config.MediaType = phymodFirmwareMediaTypePcbTraceBackPlane;
        dfe_adjust = 0 ;
        break;
    case 100000:
        cl72_allowed = 1 ;
        spd_intf = TEMOD_SPD_100G_CR10;
        break;
    case 106000:
    case 107000:
        cl72_allowed = 1 ;
        spd_intf = TEMOD_SPD_107G_HG_CR10;
        break; 
    case 120000:
        cl72_allowed = 1 ;
        spd_intf = TEMOD_SPD_120G_CR12;
        break;
    case 127000:
        cl72_allowed = 1 ;
        spd_intf = TEMOD_SPD_127G_HG_CR12;
        break;
    default:
        break;
    }

    /* need to decide if we need to set low VCO for 2.5G,1G or below */
    if (!high_vco_1G) {
        PHYMOD_IF_ERR_RETURN
            (temod_cl37_high_vco_set(&pm_phy_copy.access, 0));
    }

    if(PHYMOD_INTF_MODES_HIGIG_GET(config)) {
       PHYMOD_IF_ERR_RETURN
          (temod_encode_set(&phy->access, spd_intf, 1));
       PHYMOD_IF_ERR_RETURN
          (temod_decode_set(&phy->access, spd_intf, 1));
    } else {
       PHYMOD_IF_ERR_RETURN
          (temod_encode_set(&phy->access, spd_intf, 0));
       PHYMOD_IF_ERR_RETURN
          (temod_decode_set(&phy->access, spd_intf, 0));
    }

    if(config->data_rate==10||config->data_rate==100||
       config->data_rate==1000) {
        if  (config->interface_type == phymodInterfaceSGMII) {
            firmware_lane_config.MediaType = phymodFirmwareMediaTypePcbTraceBackPlane;
        }
        if (config->interface_type == phymodInterface1000X) {
            firmware_lane_config.MediaType = phymodFirmwareMediaTypeOptics;
        }
    }
    /* get the current PLL setting */
    pm_phy_copy.access.lane_mask = 1 << start_lane;
    PHYMOD_IF_ERR_RETURN
        (temod_pll_config_get(&pm_phy_copy.access, &pll_mode));
    current_pll_div = (uint32_t) pll_mode;     

    if(config->interface_type == phymodInterfaceBypass) {

        rv = eagle_tsc_get_vco(config, &vco_rate, &new_pll_div_enum, &new_os_mode);
        /* eagle tier1 error codes are not compatible with SDK error codes
         * convert the eagle tier1 err code to PHYMOD error code 
         */
        if (rv != PHYMOD_E_NONE) {
            return PHYMOD_E_CONFIG;
        }

        rv = eagle_tsc_get_pll_div_from_enum(new_pll_div_enum, &new_pll_div);
        if (rv != PHYMOD_E_NONE) {
            return PHYMOD_E_CONFIG;
        }

    } else {

        PHYMOD_IF_ERR_RETURN
          (temod_plldiv_lkup_get(&phy->access, spd_intf, config->ref_clock, &new_pll_div, &new_speed_vec));

        /* For 12.12G, data rate specified is 12000, and pll_div is set pll_mode_div_80 */
        if (config->ref_clock == phymodRefClk156Mhz) {
            switch(config->data_rate) {
            case 11000: new_pll_div = TEMOD_PLL_MODE_DIV_70;  break ;
            case 12000: new_pll_div = TEMOD_PLL_MODE_DIV_80;  break ;
            case 21000: 
             if (num_lane == 2) {
                new_pll_div = TEMOD_PLL_MODE_DIV_70;
             }
             break ; 
            case 25000:
             if (num_lane == 2) {
                new_pll_div = TEMOD_PLL_MODE_DIV_80;
             }
             break ;
            case 42000: new_pll_div = TEMOD_PLL_MODE_DIV_70;  break ;
            case 48000: new_pll_div = TEMOD_PLL_MODE_DIV_80;  break ;
            case 106000: new_pll_div = TEMOD_PLL_MODE_DIV_70;  break ;
            case 127000: new_pll_div = TEMOD_PLL_MODE_DIV_70;  break ;
            default: break ;
            }
        }
        if (config->ref_clock == phymodRefClk125Mhz) {
            switch(config->data_rate) {
            case 11000: new_pll_div = TEMOD_PLL_MODE_DIV_87P5;  break ;
            case 12000: new_pll_div = TEMOD_PLL_MODE_DIV_100;  break ;
            case 21000: 
             if (num_lane == 2) {
                new_pll_div = TEMOD_PLL_MODE_DIV_87P5;
             }
             break ; 
            case 25000:
             if (num_lane == 2) {
                new_pll_div = TEMOD_PLL_MODE_DIV_100;
             }
             break ;
            case 42000: new_pll_div = TEMOD_PLL_MODE_DIV_87P5;  break ;
            case 48000: new_pll_div = TEMOD_PLL_MODE_DIV_100;  break ;
            case 106000: new_pll_div = TEMOD_PLL_MODE_DIV_87P5;  break ;
            case 127000: new_pll_div = TEMOD_PLL_MODE_DIV_87P5;  break ;
            default: break ;
            }
        }
    }

    if(config->device_aux_modes !=NULL){    
        PHYMOD_IF_ERR_RETURN
            (_tsce_st_hto_interface_config_set(config->device_aux_modes, start_lane, new_speed_vec, &new_pll_div, &new_os_mode)) ;
    }

    if(new_os_mode>=0) {
        u_os_mode = new_os_mode | TEMOD_OVERRIDE_CFG ;
    } else {
        u_os_mode = 0 ;
    }

    
    PHYMOD_IF_ERR_RETURN
        (temod_pmd_osmode_set(&phy->access, spd_intf, u_os_mode));
    PHYMOD_IF_ERR_RETURN(temod_osdfe_on_lkup_get(&phy->access, spd_intf, &os_dfeon));
    PHYMOD_IF_ERR_RETURN(temod_scrambling_dis_lkup_get(&phy->access, spd_intf, &scrambling_dis));
    firmware_lane_config.DfeOn = 0;
    firmware_lane_config.ScramblingDisable = scrambling_dis;
    
    if(PHYMOD_INTF_F_SCRAMBLE_FORCED_ON & flags) {
        firmware_lane_config.ScramblingDisable = 0 ;
        
        or_val = 1;
        PHYMOD_IF_ERR_RETURN
            (temod_override_set(&phy->access, TEMOD_OVERRIDE_SCR_MODE , or_val));
        PHYMOD_IF_ERR_RETURN
            (temod_override_set(&phy->access, TEMOD_OVERRIDE_DESCR_MODE, or_val));
    } else if (PHYMOD_INTF_F_SCRAMBLE_FORCED_OFF & flags) {
        firmware_lane_config.ScramblingDisable = 1 ;

        or_val = 0;
        PHYMOD_IF_ERR_RETURN
            (temod_override_set(&phy->access, TEMOD_OVERRIDE_SCR_MODE , or_val));
        PHYMOD_IF_ERR_RETURN
            (temod_override_set(&phy->access, TEMOD_OVERRIDE_DESCR_MODE, or_val));
    } else {
        /* Override scrambler settings in PCS when disable scrambler is needed*/
        if (scrambling_dis){
            or_val = 0;
            PHYMOD_IF_ERR_RETURN
                (temod_override_set(&phy->access, TEMOD_OVERRIDE_SCR_MODE , or_val));
            PHYMOD_IF_ERR_RETURN
                (temod_override_set(&phy->access, TEMOD_OVERRIDE_DESCR_MODE, or_val));
        }
        else {
            PHYMOD_IF_ERR_RETURN
                (temod_override_clear(&phy->access, TEMOD_OVERRIDE_SCR_MODE));
            PHYMOD_IF_ERR_RETURN
                (temod_override_clear(&phy->access, TEMOD_OVERRIDE_DESCR_MODE));
        }
    }

    if(os_dfeon == 0x1)
      firmware_lane_config.DfeOn = 1;
    
    if(dfe_adjust >=0) 
        firmware_lane_config.DfeOn = dfe_adjust ;

    /* the speed is not supported */
    if (new_pll_div == TEMOD_PLL_MODE_DIV_ILLEGAL) {
        PHYMOD_RETURN_WITH_ERR(PHYMOD_E_CONFIG, 
                               (_PHYMOD_MSG("this speed %d is not supported at this ref clock %d"),
                                 config->data_rate, config->ref_clock));
    }

    /* if pll change is enabled. new_pll_div is the reg vector value */
    if((current_pll_div != new_pll_div) && (PHYMOD_INTF_F_DONT_TURN_OFF_PLL & flags)){
        /*Terminate the execution of the function*/
        PHYMOD_DEBUG_WARN(("PLL has to change for speed_set from %u to %u but DONT_TURN_OFF_PLL flag is enabled \n",
                           (unsigned int)current_pll_div, (unsigned int)new_pll_div));
        return PHYMOD_E_NONE;
    }


    /* pll switch is required and expected */
    if((current_pll_div != new_pll_div) && !(PHYMOD_INTF_F_DONT_TURN_OFF_PLL & flags)) {
        /* phymod_access_t tmp_phy_access; */
        
        /* Change in PLL, so reset all the ports first. Check for TSC12 */
        lane_bkup = pm_phy_copy.access.lane_mask;
        pm_phy_copy.access.lane_mask = 0xf;
        temod_disable_set(&pm_phy_copy.access);
        pm_phy_copy.access.lane_mask = lane_bkup;

        /* tmp_phy_access = phy->access; */

        PHYMOD_IF_ERR_RETURN
            (eagle_core_soft_reset_release(&pm_phy_copy.access, 0));

        /* release the uc reset */
        PHYMOD_IF_ERR_RETURN
            (eagle_uc_reset(&pm_phy_copy.access ,1)); 

        /* set the PLL divider */
        PHYMOD_IF_ERR_RETURN
            (temod_pll_config_set(&pm_phy_copy.access, new_pll_div, config->ref_clock));

        PHYMOD_IF_ERR_RETURN
            (_tsce_pll_multiplier_get(new_pll_div, &pll_multiplier));
        /* update the VCO rate properly */
        switch (config->ref_clock) {
            case phymodRefClk156Mhz:
                vco_rate = pll_multiplier * 156 + pll_multiplier * 25 / 100;
                break;
            case phymodRefClk125Mhz:
                vco_rate = pll_multiplier * 125;
                if ((pll_multiplier == 82) || (pll_multiplier == 87)) {
                    vco_rate = pll_multiplier * 125 + 62;
                }
                break;
            default:
                vco_rate = pll_multiplier * 156 + pll_multiplier * 25 / 100;
                break;
        }
        firmware_core_config.VcoRate = (vco_rate - 5750) / 250 + 1;                 

        /* change the  master port num to the current caller port */
        PHYMOD_IF_ERR_RETURN
            (temod_master_port_num_set(&pm_phy_copy.access, start_lane));

        PHYMOD_IF_ERR_RETURN
            (temod_pll_reset_enable_set(&pm_phy_copy.access, 1));

        /* update the firmware config properly */
        PHYMOD_IF_ERR_RETURN
            (tsce_phy_firmware_core_config_set(&pm_phy_copy, firmware_core_config));
        PHYMOD_IF_ERR_RETURN
            (eagle_core_soft_reset_release(&pm_phy_copy.access, 1));
    }
    
    if(cl72_req&cl72_allowed){
         for (i = 0; i < num_lane; i++) {
            if (!PHYMOD_LANEPBMP_MEMBER(phy->access.lane_mask, start_lane + i)) {
                continue;
            }
             pm_phy_copy.access.lane_mask = 1 << (start_lane + i);
             PHYMOD_IF_ERR_RETURN
                 (temod_clause72_control(&pm_phy_copy.access, 1|TEMOD_CL72_CONTROL_NO_TRIGER)) ;
         }
    }
    
    if(config->interface_type != phymodInterfaceBypass) {
        if(flags & PHYMOD_INTF_F_SET_SPD_NO_TRIGGER) {
          (temod_set_spd_intf(&phy->access, spd_intf, 1));
        } else {
        PHYMOD_IF_ERR_RETURN
          (temod_set_spd_intf(&phy->access, spd_intf, 0));
        }
    }

    /* TX Params are set outside this func */
    /* change TX parameters if enabled */
    /* if((PHYMOD_IF_F_DONT_OVERIDE_TX_PARAMS & flags) == 0)
       PHYMOD_IF_ERR_RETURN
       (tsce_phy_media_type_tx_get(phy, phymodMediaTypeMid, &tx_params));
       PHYMOD_IF_ERR_RETURN
       (tsce_phy_tx_set(phy, &tx_params));
    */

    for (i = 0; i < num_lane; i++) {
        if (!PHYMOD_LANEPBMP_MEMBER(phy->access.lane_mask, start_lane + i)) {
            continue;
        }
        pm_phy_copy.access.lane_mask = 0x1 << (start_lane + i);
        PHYMOD_IF_ERR_RETURN
             (_tsce_phy_firmware_lane_config_set(&pm_phy_copy, firmware_lane_config));
    }
    
    /* release the per lne soft reset bit */
    for (i = 0; i < num_lane; i++) {
        if (!PHYMOD_LANEPBMP_MEMBER(phy->access.lane_mask, start_lane + i)) {
            continue;
        }
        pm_phy_copy.access.lane_mask = 1 << (start_lane + i);
        PHYMOD_IF_ERR_RETURN
            (eagle_lane_soft_reset_release(&pm_phy_copy.access, 1));
    }

    return PHYMOD_E_NONE;
}

STATIC
int _tsce_speed_id_interface_config_get(const phymod_phy_access_t* phy, int speed_id, 
                                        phymod_phy_inf_config_t* config, uint16_t an_enable, 
                                        phymod_firmware_lane_config_t *lane_config)
{
    int ilkn_set;
    int osr_mode;
    int hg_enable = 0;
    uint32_t vco_rate;
    uint32_t pll_multiplier;
    uint32_t current_pll_div=0;
    int u_os_mode = 0xff;
    uint32_t actual_osr, actual_osr_rem;
    phymod_osr_mode_t osr_mode_enum;
    temod_pll_mode_type pll_mode;

    if (config->ref_clock == phymodRefClk125Mhz) {
        PHYMOD_IF_ERR_RETURN
            (temod_pll_config_get(&phy->access, &pll_mode));
        current_pll_div = (uint32_t) pll_mode;     
    } else {    
        PHYMOD_IF_ERR_RETURN
            (temod_get_plldiv(&phy->access, &current_pll_div));
    }

    PHYMOD_IF_ERR_RETURN
        (temod_hg_enable_get(&phy->access, &hg_enable));

    PHYMOD_IF_ERR_RETURN(temod_pcs_ilkn_chk(&phy->access, &ilkn_set));
    if(ilkn_set) {
        config->interface_type = phymodInterfaceBypass;
        PHYMOD_IF_ERR_RETURN
            (_tsce_pll_multiplier_get(current_pll_div, &pll_multiplier));
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
          if (lane_config->MediaType == phymodFirmwareMediaTypeOptics) {
              config->interface_type = phymodInterface1000X;
          } else {
              config->interface_type = phymodInterfaceSGMII;
          }     
          config->data_rate = 1000;
          break;
      case 0x4:
          config->data_rate = 1000;
          config->interface_type = phymodInterfaceCX;
          break;
      case 0x5:
          config->data_rate = 1000;
          config->interface_type = phymodInterfaceKX;
          break;
      case 0x6:
          if (lane_config->MediaType == phymodFirmwareMediaTypeOptics) {
              config->interface_type = phymodInterfaceSR;
          } else {
              config->interface_type = phymodInterfaceSGMII;
          }
          config->data_rate = 2500;
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
          config->data_rate = 10000;
          config->interface_type = phymodInterfaceXGMII;
          break;
      case 0xb:
          config->data_rate = 13000;
          config->interface_type = phymodInterfaceXGMII;
          break;
      case 0xc:
          config->data_rate = 15000;
          config->interface_type = phymodInterfaceXGMII;
          break;
      case 0xd:
          config->data_rate = 16000;
          config->interface_type = phymodInterfaceXGMII;
          break;
      case 0xe:
          config->data_rate = 20000;
          config->interface_type = phymodInterfaceCX4;
          break;
      case 0xf:
          PHYMOD_IF_ERR_RETURN
             (temod_pmd_osmode_get(&phy->access,  &u_os_mode));
          if(u_os_mode == TEMOD_PMA_OS_MODE_2) {
            config->data_rate = 5000;
            config->interface_type = phymodInterfaceRXAUI;
          } else {
            config->data_rate = 10000;
            config->interface_type = phymodInterfaceRXAUI;
          }
          break;
      case 0x10:
          PHYMOD_IF_ERR_RETURN
             (temod_pmd_osmode_get(&phy->access,  &u_os_mode));
          if(u_os_mode == TEMOD_PMA_OS_MODE_2) {
            config->data_rate = 5000;
            config->interface_type = phymodInterfaceRXAUI;
          } else {
            config->data_rate = 10000;
            config->interface_type = phymodInterfaceX2;
          }
          break;
      case 0x11:
          config->data_rate = 20000;
          config->interface_type = phymodInterfaceXGMII;
          break;
      case 0x12:
          config->data_rate = 10500;
          config->interface_type = phymodInterfaceX2;
          break;
      case 0x13:
          config->data_rate = 21000;
          config->interface_type = phymodInterfaceCX4;
          break;
      case 0x14:
          config->data_rate = 13000;  /* round up from 12700 */
          config->interface_type = phymodInterfaceX2;
          break;
      case 0x15:
          config->data_rate = 25450;
          config->interface_type = phymodInterfaceKR4;
          break;
      case 0x16:
          config->data_rate = 15750;
          config->interface_type = phymodInterfaceX2;
          break;
      case 0x17:
          config->data_rate = 31500;
          config->interface_type = phymodInterfaceXGMII;
          break;
      case 0x18:
          config->data_rate = 31500;
          config->interface_type = phymodInterfaceKR4;
          break;
      case 0x19:
          config->data_rate = 20000;
          config->interface_type = phymodInterfaceCX2;
          break;
      case 0x1a:
          if (config->ref_clock == phymodRefClk125Mhz) {
              if(current_pll_div==TEMOD_PLL_MODE_DIV_87P5) {
                config->data_rate = 21000;
              } else {
                config->data_rate = 20000;
              }
          } else {
              if(current_pll_div==TEMOD_PLL_MODE_DIV_70) {
                config->data_rate = 21000;
              } else {
                config->data_rate = 20000;
              }
          }
          config->interface_type = phymodInterfaceX2;
          break;
      case 0x1b:    /* digital_operationSpeeds_SPEED_40G_X4; BRCM */
          if (config->ref_clock == phymodRefClk125Mhz) {
              if(current_pll_div==TEMOD_PLL_MODE_DIV_87P5) {
                  config->data_rate = 42000;
              } else {
                  config->data_rate = 40000;
              }
           } else {
              if(current_pll_div==TEMOD_PLL_MODE_DIV_70) {
                  config->data_rate = 42000;
              } else {
                  config->data_rate = 40000;
              }
          }
          config->interface_type = phymodInterfaceXGMII;
          break;
      case 0x1c:
          if (config->ref_clock == phymodRefClk125Mhz) {
              if(current_pll_div==TEMOD_PLL_MODE_DIV_100) {
                  config->data_rate = 12000;
              } else if(current_pll_div==TEMOD_PLL_MODE_DIV_87P5) {
                  config->data_rate = 11000;
              } else {
                  config->data_rate = 10000;
              }
          } else {
              if(current_pll_div==TEMOD_PLL_MODE_DIV_80) {
                  config->data_rate = 12000;
              } else if(current_pll_div==TEMOD_PLL_MODE_DIV_70) {
                  config->data_rate = 11000;
              } else {
                  config->data_rate = 10000;
              }
          }
          config->interface_type = phymodInterfaceKR;
          if(!an_enable) {
              if (lane_config->MediaType == phymodFirmwareMediaTypeOptics) {
                  if(config->interface_type == phymodInterfaceSR) {
                      config->interface_type = phymodInterfaceSR ;
                  } else {
                      config->interface_type = phymodInterfaceSFI ;
                  }
              } else if (lane_config->MediaType == phymodFirmwareMediaTypeCopperCable) {
                  config->interface_type = phymodInterfaceCR ;
              } else if (lane_config->DfeOn == 1) { 
                  config->interface_type = phymodInterfaceKR;
              } else {
                  config->interface_type = phymodInterfaceXFI ;
              }
          }
          break;
      case 0x1d:
          config->data_rate = 11000;
          if (lane_config->MediaType == phymodFirmwareMediaTypeOptics) {
              if(config->interface_type == phymodInterfaceSR) {
                  config->interface_type = phymodInterfaceSR ;
              } else {
                  config->interface_type = phymodInterfaceSFI ;
              }
          } else {
              config->interface_type = phymodInterfaceXFI ;
          }
          break;
      case 0x1e:
          if (config->ref_clock == phymodRefClk125Mhz) {
              if(current_pll_div == TEMOD_PLL_MODE_DIV_100) {
                  config->data_rate = 25000;
              } else if(current_pll_div == TEMOD_PLL_MODE_DIV_87P5) {
                  config->data_rate = 21000;
              } else {
                  config->data_rate = 20000;
              }
          } else {
              if(current_pll_div == TEMOD_PLL_MODE_DIV_80) {
                  config->data_rate = 25000;
              } else if(current_pll_div == TEMOD_PLL_MODE_DIV_70) {
                  config->data_rate = 21000;
              } else {
                  config->data_rate = 20000;
              }
          }
          if (lane_config->MediaType == phymodFirmwareMediaTypeCopperCable) {
              config->interface_type = phymodInterfaceCR2;
          } else {
              config->interface_type = phymodInterfaceKR2;
          }
          break;
      case 0x1f:
          if (config->ref_clock == phymodRefClk125Mhz) {
              if(current_pll_div == TEMOD_PLL_MODE_DIV_100) {
                  config->data_rate = 25000;
              } else if(current_pll_div == TEMOD_PLL_MODE_DIV_87P5) {
                  config->data_rate = 21000;
              } else {
                  config->data_rate = 20000;
              }
          } else {
              if(current_pll_div == TEMOD_PLL_MODE_DIV_80) {
                  config->data_rate = 25000;
              } else if(current_pll_div == TEMOD_PLL_MODE_DIV_70) {
                  config->data_rate = 21000;
              } else {
                  config->data_rate = 20000;
              }
          }
          config->interface_type = phymodInterfaceCR2;
          break;
      case 0x20:
          config->data_rate = 21000;
          config->interface_type = phymodInterfaceKR2;
          break;
      case 0x21:  /* digital_operationSpeeds_SPEED_40G_KR4 */
          if (config->ref_clock == phymodRefClk125Mhz) {
              if (current_pll_div == TEMOD_PLL_MODE_DIV_100) {
                  config->data_rate = 48000;
              } else if (current_pll_div == TEMOD_PLL_MODE_DIV_87P5) {
                  config->data_rate = 42000;
              } else {
                  config->data_rate = 40000;
              }
              
          } else {
              if (current_pll_div == TEMOD_PLL_MODE_DIV_80) {
                  config->data_rate = 48000;
              } else if (current_pll_div == TEMOD_PLL_MODE_DIV_70) {
                  config->data_rate = 42000;
              } else {
                  config->data_rate = 40000;
              }
          }
          if (lane_config->MediaType == phymodFirmwareMediaTypeOptics) {
              config->interface_type = phymodInterfaceSR4;
          } else if (lane_config->MediaType == phymodFirmwareMediaTypeCopperCable) {
              config->interface_type = phymodInterfaceCR4;
          } else if ((lane_config->MediaType == phymodFirmwareMediaTypePcbTraceBackPlane) && hg_enable) { 
              config->interface_type = phymodInterfaceKR4;
          } else if(lane_config->DfeOn) {
              config->interface_type = phymodInterfaceKR4;
          } else { 
              config->interface_type = phymodInterfaceXLAUI;
          }
          break;
      case 0x22:
          config->data_rate = 40000;
          config->interface_type = phymodInterfaceCR4;
          break;
      case 0x23:  /* digital_operationSpeeds_SPEED_42G_X4; atcually MLD */
          config->data_rate = 42000;
          if(lane_config->MediaType == phymodFirmwareMediaTypeCopperCable){
              config->interface_type = phymodInterfaceCR4;
          }else if(lane_config->DfeOn) {
              config->interface_type = phymodInterfaceKR4;
          } else {
              config->interface_type = phymodInterfaceXLAUI;
          }
          break;
      case 0x24:
          config->data_rate = 100000;
          config->interface_type = phymodInterfaceCR10;
          break;
      case 0x25:
          config->data_rate = 106000;
          config->interface_type = phymodInterfaceCAUI;
          break;
      case 0x26:
          config->data_rate = 120000;
/*        config->interface_type = phymodInterfaceXGMII; */
          config->interface_type = phymodInterfaceCAUI;
          break;
      case 0x27:
          config->data_rate = 127000;
          config->interface_type = phymodInterfaceCAUI;
          break;
      case 0x31:
          config->data_rate = 5000;
          config->interface_type = phymodInterface1000X;
          break;
      case 0x32:
          config->data_rate = 10500;
          config->interface_type = phymodInterfaceXGMII;
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
          if (lane_config->MediaType == phymodFirmwareMediaTypeOptics) {
              config->interface_type = phymodInterface1000X;
          } else {
              config->interface_type = phymodInterfaceSGMII;
          }     
          config->data_rate = 1000;
          break;
      case 0x38:
          if (lane_config->MediaType == phymodFirmwareMediaTypeOptics) {
              config->interface_type = phymodInterface1000X;
          } else {
              config->interface_type = phymodInterfaceSGMII;
          }
          config->data_rate = 2500;
          break;
      default:
          config->data_rate = 0;
          config->interface_type = phymodInterfaceSGMII; 
          break;
      }
    }
    return PHYMOD_E_NONE;
}


/* flags- unused parameter */
int tsce_phy_interface_config_get(const phymod_phy_access_t* phy, uint32_t flags, phymod_ref_clk_t ref_clock, phymod_phy_inf_config_t* config)
{
    int speed_id;
    phymod_firmware_lane_config_t firmware_lane_config;  
    phymod_phy_access_t pm_phy_copy;
    int start_lane, num_lane;
    temod_an_control_t an_control;
    phymod_autoneg_control_t autoneg_control;
    int an_complete = 0;
    int hg_enable = 0;

    config->ref_clock = ref_clock;
    PHYMOD_MEMCPY(&pm_phy_copy, phy, sizeof(pm_phy_copy));
    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(&phy->access, &start_lane, &num_lane));

    PHYMOD_IF_ERR_RETURN
        (temod_speed_id_get(&phy->access, &speed_id));

    pm_phy_copy.access.lane_mask = 0x1 << start_lane;

    PHYMOD_MEMSET(&an_control, 0x0,  sizeof(temod_an_control_t));
    PHYMOD_MEMSET(&autoneg_control, 0x0, sizeof(autoneg_control));
    PHYMOD_IF_ERR_RETURN
        (temod_autoneg_control_get(&pm_phy_copy.access, &an_control, &an_complete));

    PHYMOD_IF_ERR_RETURN
        (tsce_phy_firmware_lane_config_get(&pm_phy_copy, &firmware_lane_config));
    
    PHYMOD_IF_ERR_RETURN
        (_tsce_speed_id_interface_config_get(phy, speed_id, config, an_control.enable, &firmware_lane_config));

    if (firmware_lane_config.MediaType == phymodFirmwareMediaTypeOptics) {
        PHYMOD_INTF_MODES_FIBER_SET(config);
    } else if (firmware_lane_config.MediaType == phymodFirmwareMediaTypeCopperCable) {
        PHYMOD_INTF_MODES_FIBER_CLR(config);
        PHYMOD_INTF_MODES_COPPER_SET(config);
    } else {
        PHYMOD_INTF_MODES_FIBER_CLR(config);
        PHYMOD_INTF_MODES_BACKPLANE_SET(config);
    }

    if (config->data_rate >= 100000) {
        switch(firmware_lane_config.MediaType) {
            case phymodFirmwareMediaTypePcbTraceBackPlane:
                /* no difference between CAUI and KR10. upper layer willhandle the conversion */
                config->interface_type = phymodInterfaceCAUI;
                break;
            case phymodFirmwareMediaTypeCopperCable:
                config->interface_type = phymodInterfaceCR10;
                break;
            case phymodFirmwareMediaTypeOptics:
                /*
                 * There is no difference between SR10 and LR10. tsce will return SR10 and let 
                 * the upper layers hand
                */
                config->interface_type = phymodInterfaceSR10;
                break;
            default:
                config->interface_type = phymodInterfaceCAUI;
                break;
        }
    }

    switch (config->interface_type) {
    case phymodInterfaceSGMII:
    {
        if(config->data_rate == 1000) {
            if (!PHYMOD_INTF_MODES_FIBER_GET(config)) {
                config->interface_type = phymodInterfaceSGMII;
            } else {
                config->interface_type = phymodInterface1000X;
            }
        } else {
            config->interface_type = phymodInterfaceSGMII;
        }
        break;
    }
    case phymodInterfaceKR:
    {
        /* first check if an is enable or not */
        PHYMOD_IF_ERR_RETURN
        (tsce_phy_autoneg_get(phy, &autoneg_control, (uint32_t *) &an_complete));
        if (!autoneg_control.enable) {
            if(config->data_rate == 10000) {
                if (!PHYMOD_INTF_MODES_FIBER_GET(config)) {
                   if (firmware_lane_config.DfeOn == 1) {        
                        config->interface_type = phymodInterfaceKR;
                    } else {
                        config->interface_type = phymodInterfaceXFI;
                    }
                } else {
                    config->interface_type = phymodInterfaceSFI;
                }
            } else {
                if (PHYMOD_INTF_MODES_FIBER_GET(config)) {
                    config->interface_type = phymodInterfaceSR;
                } else {
                    config->interface_type = phymodInterfaceKR;
                }
            }            
        } else {
            config->interface_type = phymodInterfaceKR;
        }
        break;
    }
    default:
        break;
    }

    PHYMOD_IF_ERR_RETURN
        (temod_hg_enable_get(&phy->access, &hg_enable));

    if (hg_enable) {
        PHYMOD_INTF_MODES_HIGIG_SET(config);
    } else {
        PHYMOD_INTF_MODES_HIGIG_CLR(config);
    }

    return PHYMOD_E_NONE;
}


int tsce_phy_cl72_set(const phymod_phy_access_t* phy, uint32_t cl72_en)
{
    struct eagle_tsc_uc_lane_config_st serdes_firmware_config;

    PHYMOD_IF_ERR_RETURN(eagle_tsc_get_uc_lane_cfg(&phy->access, &serdes_firmware_config));

    if(serdes_firmware_config.field.dfe_on == 0) {
      PHYMOD_DEBUG_ERROR(("ERROR :: DFE is off : Can not start CL72 with no DFE\n"));  
      return PHYMOD_E_CONFIG;
    }

    PHYMOD_IF_ERR_RETURN
        (temod_clause72_control(&phy->access, cl72_en));
    return PHYMOD_E_NONE;
}

int tsce_phy_cl72_get(const phymod_phy_access_t* phy, uint32_t* cl72_en)
{
    uint32_t local_en;;
    PHYMOD_IF_ERR_RETURN
        (eagle_pmd_cl72_enable_get(&phy->access, &local_en));
    *cl72_en = local_en;
    return PHYMOD_E_NONE;
}


int tsce_phy_cl72_status_get(const phymod_phy_access_t* phy, phymod_cl72_status_t* status)
{
    uint32_t local_status;
    PHYMOD_IF_ERR_RETURN
        (eagle_pmd_cl72_receiver_status(&phy->access, &local_status));
    status->locked = local_status;
    return PHYMOD_E_NONE;
}

int tsce_phy_autoneg_ability_set(const phymod_phy_access_t* phy, const phymod_autoneg_ability_t* an_ability)
{
    temod_an_ability_t value;
    int start_lane, num_lane;
    phymod_phy_access_t phy_copy;
    /* next program the tx fir taps and driver current based on the input */
    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(&phy->access, &start_lane, &num_lane));

    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));
    phy_copy.access.lane_mask = 0x1 << start_lane;

    PHYMOD_MEMSET(&value, 0x0, sizeof(value));

    value.cl37_adv.an_cl72 = an_ability->an_cl72;
    value.cl73_adv.an_cl72 = an_ability->an_cl72;
    value.cl37_adv.an_hg2 = an_ability->an_hg2;

    if (PHYMOD_AN_FEC_CL74_GET(an_ability->an_fec)) {
        value.cl37_adv.an_fec = TEMOD_FEC_CL74_SUPRTD_REQSTD;
        value.cl73_adv.an_fec = TEMOD_FEC_CL74_SUPRTD_REQSTD;
    } else if (PHYMOD_AN_FEC_OFF_GET(an_ability->an_fec)) {
        value.cl37_adv.an_fec = TEMOD_FEC_SUPRTD_NOT_REQSTD;
        value.cl73_adv.an_fec = TEMOD_FEC_SUPRTD_NOT_REQSTD;
    }

    /* check if sgmii  or not */
    if (PHYMOD_AN_CAP_SGMII_GET(an_ability)) {
        switch (an_ability->sgmii_speed) {
        case phymod_CL37_SGMII_10M:
            value.cl37_adv.cl37_sgmii_speed = TEMOD_CL37_SGMII_10M;
            break;
        case phymod_CL37_SGMII_100M:
            value.cl37_adv.cl37_sgmii_speed = TEMOD_CL37_SGMII_100M;
            break;
        case phymod_CL37_SGMII_1000M:
            value.cl37_adv.cl37_sgmii_speed = TEMOD_CL37_SGMII_1000M;
            break;
        default:
            value.cl37_adv.cl37_sgmii_speed = TEMOD_CL37_SGMII_1000M;
            break;
        }
    }
    /* next check pause */
    if (PHYMOD_AN_CAP_SYMM_PAUSE_GET(an_ability) && !PHYMOD_AN_CAP_ASYM_PAUSE_GET(an_ability)) {
        value.cl37_adv.an_pause = TEMOD_SYMM_PAUSE;
        value.cl73_adv.an_pause = TEMOD_SYMM_PAUSE;
    }
    if (PHYMOD_AN_CAP_ASYM_PAUSE_GET(an_ability) && !PHYMOD_AN_CAP_SYMM_PAUSE_GET(an_ability)) {
        value.cl37_adv.an_pause = TEMOD_ASYM_PAUSE;
        value.cl73_adv.an_pause = TEMOD_ASYM_PAUSE;
    }
    if (PHYMOD_AN_CAP_ASYM_PAUSE_GET(an_ability) && PHYMOD_AN_CAP_SYMM_PAUSE_GET(an_ability)) {
        value.cl37_adv.an_pause = TEMOD_ASYM_SYMM_PAUSE;
        value.cl73_adv.an_pause = TEMOD_ASYM_SYMM_PAUSE;
    }

    /* check cl73 and cl73 bam ability */
    if (PHYMOD_AN_CAP_1G_KX_GET(an_ability->an_cap)) 
        value.cl73_adv.an_base_speed |= 1 << TEMOD_CL73_1000BASE_KX;
    if (PHYMOD_AN_CAP_10G_KX4_GET(an_ability->an_cap)) 
        value.cl73_adv.an_base_speed |= 1 << TEMOD_CL73_10GBASE_KX4;
    if (PHYMOD_AN_CAP_10G_KR_GET(an_ability->an_cap)) 
        value.cl73_adv.an_base_speed |= 1 << TEMOD_CL73_10GBASE_KR;
    if (PHYMOD_AN_CAP_40G_KR4_GET(an_ability->an_cap)) 
        value.cl73_adv.an_base_speed |= 1 << TEMOD_CL73_40GBASE_KR4;
    if (PHYMOD_AN_CAP_40G_CR4_GET(an_ability->an_cap)) 
        value.cl73_adv.an_base_speed |= 1 << TEMOD_CL73_40GBASE_CR4;
    if (PHYMOD_AN_CAP_100G_CR10_GET(an_ability->an_cap)) 
        value.cl73_adv.an_base_speed |= 1 << TEMOD_CL73_100GBASE_CR10;

    /* next check cl73 bam ability */
    if (PHYMOD_BAM_CL73_CAP_20G_KR2_GET(an_ability->cl73bam_cap)) 
        value.cl73_adv.an_bam_speed |= 1 << TEMOD_CL73_BAM_20GBASE_KR2;
    if (PHYMOD_BAM_CL73_CAP_20G_CR2_GET(an_ability->cl73bam_cap)) 
        value.cl73_adv.an_bam_speed |= 1 << TEMOD_CL73_BAM_20GBASE_CR2;

    /* check cl37 and cl37 bam ability */
    if (PHYMOD_BAM_CL37_CAP_2P5G_GET(an_ability->cl37bam_cap)) 
        value.cl37_adv.an_bam_speed |= 1 << TEMOD_CL37_BAM_2p5GBASE_X;
    if (PHYMOD_BAM_CL37_CAP_5G_X4_GET(an_ability->cl37bam_cap)) 
        value.cl37_adv.an_bam_speed |= 1 << TEMOD_CL37_BAM_5GBASE_X4;
    if (PHYMOD_BAM_CL37_CAP_6G_X4_GET(an_ability->cl37bam_cap)) 
        value.cl37_adv.an_bam_speed |= 1 << TEMOD_CL37_BAM_6GBASE_X4;
    if (PHYMOD_BAM_CL37_CAP_10G_HIGIG_GET(an_ability->cl37bam_cap)) 
        value.cl37_adv.an_bam_speed |= 1 << TEMOD_CL37_BAM_10GBASE_X4;
    if (PHYMOD_BAM_CL37_CAP_10G_CX4_GET(an_ability->cl37bam_cap)) 
        value.cl37_adv.an_bam_speed |= 1 << TEMOD_CL37_BAM_10GBASE_X4_CX4;
    if (PHYMOD_BAM_CL37_CAP_12G_X4_GET(an_ability->cl37bam_cap)) 
        value.cl37_adv.an_bam_speed |= 1 << TEMOD_CL37_BAM_12GBASE_X4;
    if (PHYMOD_BAM_CL37_CAP_12P5_X4_GET(an_ability->cl37bam_cap)) 
        value.cl37_adv.an_bam_speed |= 1 << TEMOD_CL37_BAM_12p5GBASE_X4;
    if (PHYMOD_BAM_CL37_CAP_10G_X2_CX4_GET(an_ability->cl37bam_cap)) 
        value.cl37_adv.an_bam_speed |= 1 << TEMOD_CL37_BAM_10GBASE_X2_CX4;
    if (PHYMOD_BAM_CL37_CAP_10G_DXGXS_GET(an_ability->cl37bam_cap)) 
        value.cl37_adv.an_bam_speed |= 1 << TEMOD_CL37_BAM_10GBASE_X2;
    if (PHYMOD_BAM_CL37_CAP_10P5G_DXGXS_GET(an_ability->cl37bam_cap)) 
        value.cl37_adv.an_bam_speed |= 1 << TEMOD_CL37_BAM_BAM_10p5GBASE_X2;
    if (PHYMOD_BAM_CL37_CAP_12P7_DXGXS_GET(an_ability->cl37bam_cap)) 
        value.cl37_adv.an_bam_speed |= 1 << TEMOD_CL37_BAM_12p7GBASE_X2;
    if (PHYMOD_BAM_CL37_CAP_20G_X2_CX4_GET(an_ability->cl37bam_cap)) 
        value.cl37_adv.an_bam_speed1 |= 1 << TEMOD_CL37_BAM_20GBASE_X2_CX4;
    if (PHYMOD_BAM_CL37_CAP_20G_X2_GET(an_ability->cl37bam_cap)) 
        value.cl37_adv.an_bam_speed1 |= 1 << TEMOD_CL37_BAM_20GBASE_X2;
    if (PHYMOD_BAM_CL37_CAP_13G_X4_GET(an_ability->cl37bam_cap))
        value.cl37_adv.an_bam_speed1 |= 1 << TEMOD_CL37_BAM_13GBASE_X4;
    if (PHYMOD_BAM_CL37_CAP_15G_X4_GET(an_ability->cl37bam_cap)) 
        value.cl37_adv.an_bam_speed1 |= 1 << TEMOD_CL37_BAM_15GBASE_X4;
    if (PHYMOD_BAM_CL37_CAP_16G_X4_GET(an_ability->cl37bam_cap)) 
        value.cl37_adv.an_bam_speed1 |= 1 << TEMOD_CL37_BAM_16GBASE_X4;
    if (PHYMOD_BAM_CL37_CAP_20G_X4_CX4_GET(an_ability->cl37bam_cap)) 
        value.cl37_adv.an_bam_speed1 |= 1 << TEMOD_CL37_BAM_20GBASE_X4_CX4;
    if (PHYMOD_BAM_CL37_CAP_20G_X4_GET(an_ability->cl37bam_cap)) 
        value.cl37_adv.an_bam_speed1 |= 1 << TEMOD_CL37_BAM_20GBASE_X4;
    if (PHYMOD_BAM_CL37_CAP_21G_X4_GET(an_ability->cl37bam_cap))
        value.cl37_adv.an_bam_speed1 |= 1 << TEMOD_CL37_BAM_21GBASE_X4;
    if (PHYMOD_BAM_CL37_CAP_25P455G_GET(an_ability->cl37bam_cap)) 
        value.cl37_adv.an_bam_speed1 |= 1 << TEMOD_CL37_BAM_25p455GBASE_X4;
    if (PHYMOD_BAM_CL37_CAP_31P5G_GET(an_ability->cl37bam_cap)) 
        value.cl37_adv.an_bam_speed1 |= 1 << TEMOD_CL37_BAM_31p5GBASE_X4;
    if (PHYMOD_BAM_CL37_CAP_32P7G_GET(an_ability->cl37bam_cap)) 
        value.cl37_adv.an_bam_speed1 |= 1 << TEMOD_CL37_BAM_32p7GBASE_X4;
    if (PHYMOD_BAM_CL37_CAP_40G_GET(an_ability->cl37bam_cap)) 
        value.cl37_adv.an_bam_speed1 |= 1 << TEMOD_CL37_BAM_40GBASE_X4;

    PHYMOD_IF_ERR_RETURN
        (temod_autoneg_set(&phy_copy.access, &value));
    return PHYMOD_E_NONE;
    
}

int tsce_phy_autoneg_ability_get(const phymod_phy_access_t* phy, phymod_autoneg_ability_t* an_ability_get_type){
    temod_an_ability_t value;
    phymod_phy_access_t phy_copy;
    int start_lane, num_lane;
    temod_an_control_t an_control;
    int an_complete = 0;
    int an_fec = 0;


    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(&phy->access, &start_lane, &num_lane));
    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));
    phy_copy.access.lane_mask = 0x1 << start_lane;
    PHYMOD_MEMSET(&value, 0x0, sizeof(value));
    PHYMOD_MEMSET(&an_control, 0x0, sizeof(temod_an_control_t));

    PHYMOD_IF_ERR_RETURN
        (temod_autoneg_local_ability_get(&phy_copy.access, &value));
     PHYMOD_IF_ERR_RETURN
        (temod_autoneg_control_get(&phy_copy.access, &an_control, &an_complete));

    an_ability_get_type->an_cl72 = value.cl37_adv.an_cl72 | value.cl73_adv.an_cl72;
    an_ability_get_type->an_hg2 = value.cl37_adv.an_hg2;

    
    an_fec = value.cl37_adv.an_fec | value.cl73_adv.an_fec;
    an_ability_get_type->an_fec = 0;
    if (an_fec == TEMOD_FEC_CL74_SUPRTD_REQSTD) {
        PHYMOD_AN_FEC_CL74_SET(an_ability_get_type->an_fec);
    } else {
        PHYMOD_AN_FEC_OFF_SET(an_ability_get_type->an_fec);
    }

    if ((value.cl37_adv.an_pause == TEMOD_ASYM_PAUSE)||(value.cl73_adv.an_pause == TEMOD_ASYM_PAUSE)) {
        PHYMOD_AN_CAP_ASYM_PAUSE_SET(an_ability_get_type);
    } else if ((value.cl37_adv.an_pause == TEMOD_SYMM_PAUSE)||(value.cl73_adv.an_pause == TEMOD_SYMM_PAUSE)) {
        PHYMOD_AN_CAP_SYMM_PAUSE_SET(an_ability_get_type);
    } else if ((value.cl37_adv.an_pause == TEMOD_ASYM_SYMM_PAUSE)||(value.cl73_adv.an_pause == TEMOD_ASYM_SYMM_PAUSE)) {
        PHYMOD_AN_CAP_ASYM_PAUSE_SET(an_ability_get_type);
        PHYMOD_AN_CAP_SYMM_PAUSE_SET(an_ability_get_type);
    }

    /* get AN Clause */
    switch (an_control.an_type) {
        case TEMOD_AN_MODE_CL73:
            PHYMOD_AN_CAP_CL73_SET(an_ability_get_type);
            break;
        case TEMOD_AN_MODE_CL37:
            PHYMOD_AN_CAP_CL37_SET(an_ability_get_type);
            break;
        case TEMOD_AN_MODE_CL73BAM:
            PHYMOD_AN_CAP_CL73BAM_SET(an_ability_get_type);
            break;
        case TEMOD_AN_MODE_CL37BAM:
            PHYMOD_AN_CAP_CL37BAM_SET(an_ability_get_type);
            break;
        case TEMOD_AN_MODE_HPAM:
            PHYMOD_AN_CAP_HPAM_SET(an_ability_get_type);
            break;
        case TEMOD_AN_MODE_SGMII:
            PHYMOD_AN_CAP_SGMII_SET(an_ability_get_type);
            break;
        case TEMOD_AN_MODE_CL37_SGMII:
            PHYMOD_AN_CAP_SGMII_SET(an_ability_get_type);
            break;
        default:
            
            break;
    }

    /* get the cl37 sgmii speed */
    if (PHYMOD_AN_CAP_SGMII_GET(an_ability_get_type)){
        switch (value.cl37_adv.cl37_sgmii_speed) {
        case TEMOD_CL37_SGMII_10M:
            an_ability_get_type->sgmii_speed = phymod_CL37_SGMII_10M;
            break;
        case TEMOD_CL37_SGMII_100M:
            an_ability_get_type->sgmii_speed = phymod_CL37_SGMII_100M;
            break;
        case TEMOD_CL37_SGMII_1000M:
            an_ability_get_type->sgmii_speed = phymod_CL37_SGMII_1000M;
            break;
        default:
            break;
        }
    }            
    /* first check cl73 ability */
    if (value.cl73_adv.an_base_speed &  1 << TEMOD_CL73_100GBASE_CR10) 
        PHYMOD_AN_CAP_100G_CR10_SET(an_ability_get_type->an_cap);
    if (value.cl73_adv.an_base_speed & 1 << TEMOD_CL73_40GBASE_CR4) 
        PHYMOD_AN_CAP_40G_CR4_SET(an_ability_get_type->an_cap);
    if (value.cl73_adv.an_base_speed & 1 << TEMOD_CL73_40GBASE_KR4) 
        PHYMOD_AN_CAP_40G_KR4_SET(an_ability_get_type->an_cap);
    if (value.cl73_adv.an_base_speed & 1 << TEMOD_CL73_10GBASE_KR) 
        PHYMOD_AN_CAP_10G_KR_SET(an_ability_get_type->an_cap);
    if (value.cl73_adv.an_base_speed & 1 << TEMOD_CL73_10GBASE_KX4) 
        PHYMOD_AN_CAP_10G_KX4_SET(an_ability_get_type->an_cap);
    if (value.cl73_adv.an_base_speed & 1 << TEMOD_CL73_1000BASE_KX) 
        PHYMOD_AN_CAP_1G_KX_SET(an_ability_get_type->an_cap);

    /* next check cl73 bam ability */
    if (value.cl73_adv.an_bam_speed & 1 << TEMOD_CL73_BAM_20GBASE_KR2) 
        PHYMOD_BAM_CL73_CAP_20G_KR2_SET(an_ability_get_type->cl73bam_cap);
    if (value.cl73_adv.an_bam_speed & 1 << TEMOD_CL73_BAM_20GBASE_CR2) 
        PHYMOD_BAM_CL73_CAP_20G_CR2_SET(an_ability_get_type->cl73bam_cap);

    /* check cl37 bam ability */
    if (value.cl37_adv.an_bam_speed & 1 << TEMOD_CL37_BAM_2p5GBASE_X) 
        PHYMOD_BAM_CL37_CAP_2P5G_SET(an_ability_get_type->cl37bam_cap);
    if (value.cl37_adv.an_bam_speed & 1 << TEMOD_CL37_BAM_5GBASE_X4) 
        PHYMOD_BAM_CL37_CAP_5G_X4_SET(an_ability_get_type->cl37bam_cap);
    if (value.cl37_adv.an_bam_speed & 1 << TEMOD_CL37_BAM_6GBASE_X4) 
        PHYMOD_BAM_CL37_CAP_6G_X4_SET(an_ability_get_type->cl37bam_cap);
    if (value.cl37_adv.an_bam_speed & 1 << TEMOD_CL37_BAM_10GBASE_X4) 
        PHYMOD_BAM_CL37_CAP_10G_HIGIG_SET(an_ability_get_type->cl37bam_cap);
    if (value.cl37_adv.an_bam_speed & 1 << TEMOD_CL37_BAM_10GBASE_X4_CX4) 
        PHYMOD_BAM_CL37_CAP_10G_CX4_SET(an_ability_get_type->cl37bam_cap);
    if (value.cl37_adv.an_bam_speed & 1 << TEMOD_CL37_BAM_10GBASE_X2) 
        PHYMOD_BAM_CL37_CAP_10G_DXGXS_SET(an_ability_get_type->cl37bam_cap);
    if (value.cl37_adv.an_bam_speed & 1 << TEMOD_CL37_BAM_10GBASE_X2_CX4) 
        PHYMOD_BAM_CL37_CAP_10G_X2_CX4_SET(an_ability_get_type->cl37bam_cap);
    if (value.cl37_adv.an_bam_speed & 1 << TEMOD_CL37_BAM_BAM_10p5GBASE_X2) 
        PHYMOD_BAM_CL37_CAP_10P5G_DXGXS_SET(an_ability_get_type->cl37bam_cap);
    if (value.cl37_adv.an_bam_speed & 1 << TEMOD_CL37_BAM_12GBASE_X4) 
        PHYMOD_BAM_CL37_CAP_12G_X4_SET(an_ability_get_type->cl37bam_cap);
    if (value.cl37_adv.an_bam_speed & 1 << TEMOD_CL37_BAM_12p5GBASE_X4) 
        PHYMOD_BAM_CL37_CAP_12P5_X4_SET(an_ability_get_type->cl37bam_cap);
    if (value.cl37_adv.an_bam_speed & 1 << TEMOD_CL37_BAM_12p7GBASE_X2) 
        PHYMOD_BAM_CL37_CAP_12P7_DXGXS_SET(an_ability_get_type->cl37bam_cap);

    if (value.cl37_adv.an_bam_speed1 & 1 << TEMOD_CL37_BAM_13GBASE_X4) 
        PHYMOD_BAM_CL37_CAP_13G_X4_SET(an_ability_get_type->cl37bam_cap);
    if (value.cl37_adv.an_bam_speed1 & 1 << TEMOD_CL37_BAM_15GBASE_X4) 
        PHYMOD_BAM_CL37_CAP_15G_X4_SET(an_ability_get_type->cl37bam_cap);
    if (value.cl37_adv.an_bam_speed1 & 1 << TEMOD_CL37_BAM_15p75GBASE_X2) 
        PHYMOD_BAM_CL37_CAP_12P7_DXGXS_SET(an_ability_get_type->cl37bam_cap);
    if (value.cl37_adv.an_bam_speed1 & 1 << TEMOD_CL37_BAM_16GBASE_X4) 
        PHYMOD_BAM_CL37_CAP_16G_X4_SET(an_ability_get_type->cl37bam_cap);
    if (value.cl37_adv.an_bam_speed1 & 1 << TEMOD_CL37_BAM_20GBASE_X4_CX4) 
        PHYMOD_BAM_CL37_CAP_20G_X4_CX4_SET(an_ability_get_type->cl37bam_cap);
    if (value.cl37_adv.an_bam_speed1 & 1 << TEMOD_CL37_BAM_20GBASE_X4) 
        PHYMOD_BAM_CL37_CAP_20G_X4_SET(an_ability_get_type->cl37bam_cap);
    if (value.cl37_adv.an_bam_speed1 & 1 << TEMOD_CL37_BAM_20GBASE_X2) 
        PHYMOD_BAM_CL37_CAP_20G_X2_SET(an_ability_get_type->cl37bam_cap);
    if (value.cl37_adv.an_bam_speed1 & 1 << TEMOD_CL37_BAM_20GBASE_X2_CX4) 
        PHYMOD_BAM_CL37_CAP_20G_X2_CX4_SET(an_ability_get_type->cl37bam_cap);
    if (value.cl37_adv.an_bam_speed1 & 1 << TEMOD_CL37_BAM_21GBASE_X4) 
        PHYMOD_BAM_CL37_CAP_21G_X4_SET(an_ability_get_type->cl37bam_cap);
    if (value.cl37_adv.an_bam_speed1 & 1 << TEMOD_CL37_BAM_25p455GBASE_X4) 
        PHYMOD_BAM_CL37_CAP_25P455G_SET(an_ability_get_type->cl37bam_cap);
    if (value.cl37_adv.an_bam_speed1 & 1 << TEMOD_CL37_BAM_31p5GBASE_X4) 
        PHYMOD_BAM_CL37_CAP_31P5G_SET(an_ability_get_type->cl37bam_cap);
    if (value.cl37_adv.an_bam_speed1 & 1 << TEMOD_CL37_BAM_32p7GBASE_X4) 
        PHYMOD_BAM_CL37_CAP_32P7G_SET(an_ability_get_type->cl37bam_cap);
    if (value.cl37_adv.an_bam_speed1 & 1 << TEMOD_CL37_BAM_40GBASE_X4) 
        PHYMOD_BAM_CL37_CAP_40G_SET(an_ability_get_type->cl37bam_cap);

    return PHYMOD_E_NONE;
}

int tsce_phy_autoneg_remote_ability_get(const phymod_phy_access_t* phy, phymod_autoneg_ability_t* an_ability_get_type)
{

    temod_an_ability_t value;
    temod_an_control_t an_control;
    phymod_phy_access_t phy_copy;
    int an_complete = 0;
    int start_lane, num_lane;
    int an_fec = 0;
    
    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(&phy->access, &start_lane, &num_lane));
    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));
    phy_copy.access.lane_mask = 0x1 << start_lane;
    PHYMOD_MEMSET(&value, 0x0, sizeof(value));
    PHYMOD_MEMSET(&an_control, 0x0, sizeof(an_control));
     
    PHYMOD_IF_ERR_RETURN
        (temod_autoneg_remote_ability_get(&phy_copy.access, &value));
    an_ability_get_type->an_cl72 = value.cl37_adv.an_cl72 | value.cl73_adv.an_cl72;
    an_ability_get_type->an_hg2 = value.cl37_adv.an_hg2;
    
    an_fec = value.cl37_adv.an_fec | value.cl73_adv.an_fec;
    if (an_fec == TEMOD_FEC_CL74_SUPRTD_REQSTD) {
        PHYMOD_AN_FEC_CL74_SET(an_ability_get_type->an_fec);
    } else {
        PHYMOD_AN_FEC_OFF_SET(an_ability_get_type->an_fec);
    }

    PHYMOD_IF_ERR_RETURN
        (temod_autoneg_control_get(&phy_copy.access, &an_control, &an_complete));

    /* get AN Clause */
    switch (an_control.an_type) {
        case TEMOD_AN_MODE_CL73:
            PHYMOD_AN_CAP_CL73_SET(an_ability_get_type);
            break;
        case TEMOD_AN_MODE_CL37:
            PHYMOD_AN_CAP_CL37_SET(an_ability_get_type);
            break;
        case TEMOD_AN_MODE_CL73BAM:
            PHYMOD_AN_CAP_CL73BAM_SET(an_ability_get_type);
            break;
        case TEMOD_AN_MODE_CL37BAM:
            PHYMOD_AN_CAP_CL37BAM_SET(an_ability_get_type);
            break;
        case TEMOD_AN_MODE_HPAM:
            PHYMOD_AN_CAP_HPAM_SET(an_ability_get_type);
            break;
        case TEMOD_AN_MODE_SGMII:
            PHYMOD_AN_CAP_SGMII_SET(an_ability_get_type);
            break;
        case TEMOD_AN_MODE_CL37_SGMII:
            PHYMOD_AN_CAP_SGMII_SET(an_ability_get_type);
            break;
        default:
            break;
    }

    if (an_control.an_type == TEMOD_AN_MODE_CL73 || an_control.an_type == TEMOD_AN_MODE_CL73BAM) {
      if (value.cl73_adv.an_pause == TEMOD_ASYM_PAUSE) {
          PHYMOD_AN_CAP_ASYM_PAUSE_SET(an_ability_get_type);
      } else if (value.cl73_adv.an_pause == TEMOD_SYMM_PAUSE) {
          PHYMOD_AN_CAP_SYMM_PAUSE_SET(an_ability_get_type);
      } else if (value.cl73_adv.an_pause == TEMOD_ASYM_SYMM_PAUSE) {
          PHYMOD_AN_CAP_ASYM_PAUSE_SET(an_ability_get_type);
          PHYMOD_AN_CAP_SYMM_PAUSE_SET(an_ability_get_type);
      }
    } else {
      if (value.cl37_adv.an_pause == TEMOD_ASYM_PAUSE) {
          PHYMOD_AN_CAP_ASYM_PAUSE_SET(an_ability_get_type);
      } else if (value.cl37_adv.an_pause == TEMOD_SYMM_PAUSE) {
          PHYMOD_AN_CAP_SYMM_PAUSE_SET(an_ability_get_type);
      } else if (value.cl37_adv.an_pause == TEMOD_ASYM_SYMM_PAUSE) {
          PHYMOD_AN_CAP_ASYM_PAUSE_SET(an_ability_get_type);
          PHYMOD_AN_CAP_SYMM_PAUSE_SET(an_ability_get_type);
      }
    }

    if (an_control.an_type == TEMOD_AN_MODE_CL37) {
        PHYMOD_AN_CAP_CL37_SET(an_ability_get_type);
    }

    /* get the cl37 sgmii speed */
    if (PHYMOD_AN_CAP_SGMII_GET(an_ability_get_type)) {
        switch (value.cl37_adv.cl37_sgmii_speed) {
        case TEMOD_CL37_SGMII_10M:
            an_ability_get_type->sgmii_speed = phymod_CL37_SGMII_10M;
            break;
        case TEMOD_CL37_SGMII_100M:
            an_ability_get_type->sgmii_speed = phymod_CL37_SGMII_100M;
            break;
        case TEMOD_CL37_SGMII_1000M:
            an_ability_get_type->sgmii_speed = phymod_CL37_SGMII_1000M;
            break;
        default:
            break;
        }
    }            

    /* first check cl73 ability */
    if (value.cl73_adv.an_base_speed &  1 << TEMOD_CL73_100GBASE_CR10) 
        PHYMOD_AN_CAP_100G_CR10_SET(an_ability_get_type->an_cap);
    if (value.cl73_adv.an_base_speed & 1 << TEMOD_CL73_40GBASE_CR4) 
        PHYMOD_AN_CAP_40G_CR4_SET(an_ability_get_type->an_cap);
    if (value.cl73_adv.an_base_speed & 1 << TEMOD_CL73_40GBASE_KR4) 
        PHYMOD_AN_CAP_40G_KR4_SET(an_ability_get_type->an_cap);
    if (value.cl73_adv.an_base_speed & 1 << TEMOD_CL73_10GBASE_KR) 
        PHYMOD_AN_CAP_10G_KR_SET(an_ability_get_type->an_cap);
    if (value.cl73_adv.an_base_speed & 1 << TEMOD_CL73_10GBASE_KX4) 
        PHYMOD_AN_CAP_10G_KX4_SET(an_ability_get_type->an_cap);
    if (value.cl73_adv.an_base_speed & 1 << TEMOD_CL73_1000BASE_KX) 
        PHYMOD_AN_CAP_1G_KX_SET(an_ability_get_type->an_cap);

    /* next check cl73 bam ability */
    /*UD0 in UP4 indicates Over 1G ability when an mode is cl37 bam*/
    if (PHYMOD_AN_CAP_CL73BAM_GET(an_ability_get_type)) {
        if (value.cl73_adv.an_bam_speed & 1 << TEMOD_CL73_BAM_20GBASE_KR2) 
            PHYMOD_BAM_CL73_CAP_20G_KR2_SET(an_ability_get_type->cl73bam_cap);
        if (value.cl73_adv.an_bam_speed & 1 << TEMOD_CL73_BAM_20GBASE_CR2) 
            PHYMOD_BAM_CL73_CAP_20G_CR2_SET(an_ability_get_type->cl73bam_cap);
    }

    /* check cl37 bam ability */
    if (value.cl37_adv.an_bam_speed & 1 << TEMOD_CL37_BAM_2p5GBASE_X) 
        PHYMOD_BAM_CL37_CAP_2P5G_SET(an_ability_get_type->cl37bam_cap);
    if (value.cl37_adv.an_bam_speed & 1 << TEMOD_CL37_BAM_5GBASE_X4) 
        PHYMOD_BAM_CL37_CAP_5G_X4_SET(an_ability_get_type->cl37bam_cap);
    if (value.cl37_adv.an_bam_speed & 1 << TEMOD_CL37_BAM_6GBASE_X4) 
        PHYMOD_BAM_CL37_CAP_6G_X4_SET(an_ability_get_type->cl37bam_cap);
    if (value.cl37_adv.an_bam_speed & 1 << TEMOD_CL37_BAM_10GBASE_X4) 
        PHYMOD_BAM_CL37_CAP_10G_HIGIG_SET(an_ability_get_type->cl37bam_cap);
    if (value.cl37_adv.an_bam_speed & 1 << TEMOD_CL37_BAM_10GBASE_X4_CX4) 
        PHYMOD_BAM_CL37_CAP_10G_CX4_SET(an_ability_get_type->cl37bam_cap);
    if (value.cl37_adv.an_bam_speed & 1 << TEMOD_CL37_BAM_10GBASE_X2) 
        PHYMOD_BAM_CL37_CAP_10G_DXGXS_SET(an_ability_get_type->cl37bam_cap);
    if (value.cl37_adv.an_bam_speed & 1 << TEMOD_CL37_BAM_10GBASE_X2_CX4) 
        PHYMOD_BAM_CL37_CAP_10G_X2_CX4_SET(an_ability_get_type->cl37bam_cap);
    if (value.cl37_adv.an_bam_speed & 1 << TEMOD_CL37_BAM_BAM_10p5GBASE_X2) 
        PHYMOD_BAM_CL37_CAP_10P5G_DXGXS_SET(an_ability_get_type->cl37bam_cap);
    if (value.cl37_adv.an_bam_speed & 1 << TEMOD_CL37_BAM_12GBASE_X4) 
        PHYMOD_BAM_CL37_CAP_12G_X4_SET(an_ability_get_type->cl37bam_cap);
    if (value.cl37_adv.an_bam_speed & 1 << TEMOD_CL37_BAM_12p5GBASE_X4) 
        PHYMOD_BAM_CL37_CAP_12P5_X4_SET(an_ability_get_type->cl37bam_cap);
    if (value.cl37_adv.an_bam_speed & 1 << TEMOD_CL37_BAM_12p7GBASE_X2) 
        PHYMOD_BAM_CL37_CAP_12P7_DXGXS_SET(an_ability_get_type->cl37bam_cap);

    if (value.cl37_adv.an_bam_speed1 & 1 << TEMOD_CL37_BAM_13GBASE_X4) 
        PHYMOD_BAM_CL37_CAP_13G_X4_SET(an_ability_get_type->cl37bam_cap);
    if (value.cl37_adv.an_bam_speed1 & 1 << TEMOD_CL37_BAM_15GBASE_X4) 
        PHYMOD_BAM_CL37_CAP_15G_X4_SET(an_ability_get_type->cl37bam_cap);
    if (value.cl37_adv.an_bam_speed1 & 1 << TEMOD_CL37_BAM_15p75GBASE_X2) 
        PHYMOD_BAM_CL37_CAP_12P7_DXGXS_SET(an_ability_get_type->cl37bam_cap);
    if (value.cl37_adv.an_bam_speed1 & 1 << TEMOD_CL37_BAM_16GBASE_X4) 
        PHYMOD_BAM_CL37_CAP_16G_X4_SET(an_ability_get_type->cl37bam_cap);
    if (value.cl37_adv.an_bam_speed1 & 1 << TEMOD_CL37_BAM_20GBASE_X4_CX4) 
        PHYMOD_BAM_CL37_CAP_20G_X4_CX4_SET(an_ability_get_type->cl37bam_cap);
    if (value.cl37_adv.an_bam_speed1 & 1 << TEMOD_CL37_BAM_20GBASE_X4) 
        PHYMOD_BAM_CL37_CAP_20G_X4_SET(an_ability_get_type->cl37bam_cap);
    if (value.cl37_adv.an_bam_speed1 & 1 << TEMOD_CL37_BAM_20GBASE_X2) 
        PHYMOD_BAM_CL37_CAP_20G_X2_SET(an_ability_get_type->cl37bam_cap);
    if (value.cl37_adv.an_bam_speed1 & 1 << TEMOD_CL37_BAM_20GBASE_X2_CX4) 
        PHYMOD_BAM_CL37_CAP_20G_X2_CX4_SET(an_ability_get_type->cl37bam_cap);
    if (value.cl37_adv.an_bam_speed1 & 1 << TEMOD_CL37_BAM_21GBASE_X4) 
        PHYMOD_BAM_CL37_CAP_21G_X4_SET(an_ability_get_type->cl37bam_cap);
    if (value.cl37_adv.an_bam_speed1 & 1 << TEMOD_CL37_BAM_25p455GBASE_X4) 
        PHYMOD_BAM_CL37_CAP_25P455G_SET(an_ability_get_type->cl37bam_cap);
    if (value.cl37_adv.an_bam_speed1 & 1 << TEMOD_CL37_BAM_31p5GBASE_X4) 
        PHYMOD_BAM_CL37_CAP_31P5G_SET(an_ability_get_type->cl37bam_cap);
    if (value.cl37_adv.an_bam_speed1 & 1 << TEMOD_CL37_BAM_32p7GBASE_X4) 
        PHYMOD_BAM_CL37_CAP_32P7G_SET(an_ability_get_type->cl37bam_cap);
    if (value.cl37_adv.an_bam_speed1 & 1 << TEMOD_CL37_BAM_40GBASE_X4) 
        PHYMOD_BAM_CL37_CAP_40G_SET(an_ability_get_type->cl37bam_cap);

    return PHYMOD_E_NONE;
}

int tsce_phy_autoneg_restart_set(const phymod_phy_access_t* phy, const phymod_autoneg_control_t* an)
{
    int start_lane, num_lane ;
    phymod_phy_access_t phy_copy;
    temod_an_control_t an_control;

    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(&phy->access, &start_lane, &num_lane));
    
    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));
    phy_copy.access.lane_mask = 0x1 << start_lane;

    if(an->an_mode == phymod_AN_MODE_CL37_SGMII) {
        an_control.enable       = an->enable;
        an_control.an_type = TEMOD_AN_MODE_CL37_SGMII;
        PHYMOD_IF_ERR_RETURN
            (temod_autoneg_restart(&phy_copy.access, &an_control));
    } 
    return PHYMOD_E_NONE;
}


int tsce_phy_autoneg_set(const phymod_phy_access_t* phy, const phymod_autoneg_control_t* an)
{
    int num_lane_adv_encoded;
    phymod_firmware_lane_config_t firmware_lane_config;
    phymod_firmware_core_config_t firmware_core_config_tmp;
    int start_lane, num_lane, i ;
    phymod_phy_access_t phy_copy;
    temod_an_control_t an_control;
    int ctrl_port = 0 ;
    int no_pll_switch = 0;
    uint32_t current_pll_div=0;

    /* next program the tx fir taps and driver current based on the input */
    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(&phy->access, &start_lane, &num_lane));

    PHYMOD_MEMSET(&firmware_lane_config, 0x0, sizeof(firmware_lane_config));
    PHYMOD_MEMSET(&firmware_core_config_tmp, 0x0, sizeof(firmware_core_config_tmp));

    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));
    phy_copy.access.lane_mask = 0x1 << start_lane;

    if((an->flags & PHYMOD_AN_F_SET_PRIOR_ENABLE)&& an->enable) {
        PHYMOD_IF_ERR_RETURN
            (temod_tsc12_control(&phy_copy.access, 0, -1));


        temod_disable_set(&phy->access);
        PHYMOD_IF_ERR_RETURN
            (tsce_phy_firmware_lane_config_get(&phy_copy, &firmware_lane_config));

        for (i = 0; i < num_lane; i++) {
            if (!PHYMOD_LANEPBMP_MEMBER(phy->access.lane_mask, start_lane + i)) {
                continue;
            }
            phy_copy.access.lane_mask = 0x1 << (i + start_lane);
            PHYMOD_IF_ERR_RETURN
                (eagle_lane_soft_reset_release(&phy_copy.access, 0));
        }

        if (an->enable) {
            firmware_lane_config.AnEnabled = 1;
            firmware_lane_config.LaneConfigFromPCS = 1;
        } else {
            firmware_lane_config.AnEnabled = 0;
            firmware_lane_config.LaneConfigFromPCS = 0;
        }
        for (i = 0; i < num_lane; i++) {
            if (!PHYMOD_LANEPBMP_MEMBER(phy->access.lane_mask, start_lane + i)) {
                continue;
            }
            phy_copy.access.lane_mask = 0x1 << (i + start_lane);
            PHYMOD_IF_ERR_RETURN
                (_tsce_phy_firmware_lane_config_set(&phy_copy, firmware_lane_config));
        }

        for (i = 0; i < num_lane; i++) {
            if (!PHYMOD_LANEPBMP_MEMBER(phy->access.lane_mask, start_lane + i)) {
                continue;
            }
            phy_copy.access.lane_mask = 0x1 << (i + start_lane);
            PHYMOD_IF_ERR_RETURN
                (eagle_lane_soft_reset_release(&phy_copy.access, 1));
        }


        return PHYMOD_E_NONE ;
    }

    /* make sure the firmware config is set to an eenabled */
    PHYMOD_IF_ERR_RETURN
        (tsce_phy_firmware_core_config_get(&phy_copy, &firmware_core_config_tmp));


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
        case 10:
            num_lane_adv_encoded = 3;
            break;
        default:
            return PHYMOD_E_PARAM;
    }

    if(PHYMOD_AN_F_SET_CL73_PDET_KX_ENABLE_GET(an)) {
        an_control.pd_kx_en = 1;
    } else {
        an_control.pd_kx_en = 0;
    }
    if(PHYMOD_AN_F_SET_CL73_PDET_KX4_ENABLE_GET(an)) {
        an_control.pd_kx4_en = 1;
    } else {
        an_control.pd_kx4_en = 0;
    }

    an_control.num_lane_adv = num_lane_adv_encoded;
    an_control.enable       = an->enable;
    if (PHYMOD_AN_F_SGMII_MASTER_MODE_GET(an)) {
        an_control.an_property_type = TEMOD_AN_PROPERTY_ENABLE_SGMII_MASTER_MODE;
    } else {
        an_control.an_property_type = 0x0;   /* for now disable */
    }
    switch (an->an_mode) {
    case phymod_AN_MODE_CL73:
        an_control.an_type = TEMOD_AN_MODE_CL73;
        break;
    case phymod_AN_MODE_CL37:
        an_control.an_type = TEMOD_AN_MODE_CL37;
        break;
    case phymod_AN_MODE_CL73BAM:
        an_control.an_type = TEMOD_AN_MODE_CL73BAM;
        break;
    case phymod_AN_MODE_CL37BAM:
        an_control.an_type = TEMOD_AN_MODE_CL37BAM;
        break;
    case phymod_AN_MODE_HPAM:
        an_control.an_type = TEMOD_AN_MODE_HPAM;
        break;
    case phymod_AN_MODE_SGMII:
        an_control.an_type = TEMOD_AN_MODE_SGMII;
        break;
    case phymod_AN_MODE_CL37BAM_10P9375G_VCO:
        an_control.an_type = TEMOD_AN_MODE_CL37BAM;
        no_pll_switch = 1;
        break;
    default:
        if(an->an_mode ==(phymod_AN_MODE_CL37_SGMII)) {
            an_control.an_type = TEMOD_AN_MODE_CL37_SGMII ;
        } else {
            an_control.an_type = TEMOD_AN_MODE_CL73;
        }
        break;
    }

    /* make sure the firmware config is set to an eenabled */
    PHYMOD_IF_ERR_RETURN
        (tsce_phy_firmware_core_config_get(&phy_copy, &firmware_core_config_tmp));

    /* Hold core soft reset */
    if ((an->num_lane_adv == 4) && !no_pll_switch) {
      PHYMOD_IF_ERR_RETURN
          (eagle_core_soft_reset_release(&phy_copy.access, 0));
    }

    temod_disable_set(&phy->access);

    for (i = 0; i < num_lane; i++) {
        if (!PHYMOD_LANEPBMP_MEMBER(phy->access.lane_mask, start_lane + i)) {
            continue;
        }
        phy_copy.access.lane_mask = 0x1 << (i + start_lane);
        PHYMOD_IF_ERR_RETURN
            (eagle_lane_soft_reset_release(&phy_copy.access, 0));
    }

    if (no_pll_switch) {
        /*check VCO rate first */
        PHYMOD_IF_ERR_RETURN
            (temod_get_plldiv(&phy_copy.access, &current_pll_div));
        /* need to switch to 10.9375GVCO if not */
        if (current_pll_div != 0xc) {
            temod_disable_set(&phy->access);
            PHYMOD_IF_ERR_RETURN
              (eagle_core_soft_reset_release(&phy_copy.access, 0));
            PHYMOD_USLEEP(500);
            /* set the PLL divider */
            PHYMOD_IF_ERR_RETURN
                (eagle_pll_mode_set(&phy_copy.access, 0xc));

            PHYMOD_IF_ERR_RETURN
                (temod_pll_reset_enable_set(&phy_copy.access, 1));

            PHYMOD_IF_ERR_RETURN
                (eagle_core_soft_reset_release(&phy_copy.access, 1));
            PHYMOD_USLEEP(500);
            PHYMOD_IF_ERR_RETURN
                (temod_trigger_speed_change(&phy->access));
        }
    }       
        
    /* make sure the firmware config is set to an eenabled */
    PHYMOD_IF_ERR_RETURN
        (tsce_phy_firmware_lane_config_get(&phy_copy, &firmware_lane_config));

    if (!PHYMOD_AN_F_IGNORE_MEDIUM_CHECK_GET(an)) {
        if(an_control.an_type == TEMOD_AN_MODE_SGMII) {
            firmware_lane_config.MediaType = phymodFirmwareMediaTypePcbTraceBackPlane;
        }
        if(an_control.an_type == TEMOD_AN_MODE_CL37) {
            firmware_lane_config.MediaType = phymodFirmwareMediaTypeOptics;
        }
    }

    if (an->enable) {
        firmware_lane_config.AnEnabled = 1;
        firmware_lane_config.LaneConfigFromPCS = 1;
        firmware_core_config_tmp.CoreConfigFromPCS = 1;
        firmware_lane_config.Cl72RestTO = 0;
    } else {
        firmware_lane_config.AnEnabled = 0;
        firmware_lane_config.LaneConfigFromPCS = 0;
        firmware_core_config_tmp.CoreConfigFromPCS = 0;
        firmware_lane_config.Cl72RestTO = 1;
    }

   if ((an->num_lane_adv == 4) && !no_pll_switch)  {
     phy_copy.access.lane_mask = 0x1 << start_lane;
     PHYMOD_IF_ERR_RETURN
        (tsce_phy_firmware_core_config_set(&phy_copy, firmware_core_config_tmp));
    }

    for (i = 0; i < num_lane; i++) {
        if (!PHYMOD_LANEPBMP_MEMBER(phy->access.lane_mask, start_lane + i)) {
            continue;
        }
        phy_copy.access.lane_mask = 0x1 << (i + start_lane);
        PHYMOD_IF_ERR_RETURN
            (_tsce_phy_firmware_lane_config_set(&phy_copy, firmware_lane_config));
    }
    /* Release core soft reset */
    if ((an->num_lane_adv == 4) && !no_pll_switch) {
      phy_copy.access.lane_mask = 0x1 << start_lane;
      PHYMOD_IF_ERR_RETURN
        (eagle_core_soft_reset_release(&phy_copy.access, 1));
    }

    for (i = 0; i < num_lane; i++) {
        if (!PHYMOD_LANEPBMP_MEMBER(phy->access.lane_mask, start_lane + i)) {
            continue;
        }
        phy_copy.access.lane_mask = 0x1 << (i + start_lane);
        PHYMOD_IF_ERR_RETURN
            (eagle_lane_soft_reset_release(&phy_copy.access, 1));
    }

    phy_copy.access.lane_mask = 0x1 << start_lane;
    PHYMOD_IF_ERR_RETURN
        (temod_trigger_speed_change(&phy_copy.access));

    if (!no_pll_switch) {
        if (an->enable) {
            if(an->num_lane_adv == 4)  { 
                ctrl_port = 1 ;
            } else {
            	  ctrl_port = 0 ;
            }
            PHYMOD_IF_ERR_RETURN
                (temod_set_an_port_mode(&phy->access, an->enable, num_lane_adv_encoded, start_lane, ctrl_port));
        } else {
            ctrl_port =0 ;
            PHYMOD_IF_ERR_RETURN
                (temod_set_an_port_mode(&phy->access, an->enable, num_lane_adv_encoded, start_lane, ctrl_port));
        }
    }
    /* temod_autoneg_control invokes disabling CL72 and spd_trigger when en=0 */

    PHYMOD_IF_ERR_RETURN
        (temod_autoneg_control(&phy_copy.access, &an_control));
    

    return PHYMOD_E_NONE;
    
}

int tsce_phy_autoneg_get(const phymod_phy_access_t* phy, phymod_autoneg_control_t* an, uint32_t* an_done)
{
    temod_an_control_t an_control;
    phymod_phy_access_t phy_copy;
    int start_lane, num_lane; 
    int an_complete = 0;

    /* next program the tx fir taps and driver current based on the input */
    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(&phy->access, &start_lane, &num_lane));

    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));
    phy_copy.access.lane_mask = 0x1 << start_lane;

    PHYMOD_MEMSET(&an_control, 0x0,  sizeof(temod_an_control_t));
    PHYMOD_IF_ERR_RETURN
        (temod_autoneg_control_get(&phy_copy.access, &an_control, &an_complete));
    
    if (an_control.enable) {
        an->enable = 1;
        *an_done = an_complete; 
    } else {
        an->enable = 0;
        *an_done   = 0;
    }
    
    if(an_control.pd_kx_en) {
        PHYMOD_AN_F_SET_CL73_PDET_KX_ENABLE_SET(an);
    } else {
        PHYMOD_AN_F_SET_CL73_PDET_KX_ENABLE_CLR(an);
    }
    if(an_control.pd_kx4_en) {
        PHYMOD_AN_F_SET_CL73_PDET_KX4_ENABLE_SET(an);
    } else {
        PHYMOD_AN_F_SET_CL73_PDET_KX4_ENABLE_CLR(an);
    }

    if (an_control.an_property_type == TEMOD_AN_PROPERTY_ENABLE_SGMII_MASTER_MODE) {
        PHYMOD_AN_F_SGMII_MASTER_MODE_SET(an);
    } else {
        PHYMOD_AN_F_SGMII_MASTER_MODE_CLR(an);
    }

    switch (an_control.an_type) {
        case TEMOD_AN_MODE_CL73:
            an->an_mode = phymod_AN_MODE_CL73;
            break;
        case TEMOD_AN_MODE_CL37:
            an->an_mode = phymod_AN_MODE_CL37;
            break;
        case TEMOD_AN_MODE_CL73BAM:
            an->an_mode = phymod_AN_MODE_CL73BAM;
            break;
        case TEMOD_AN_MODE_CL37BAM:
            an->an_mode = phymod_AN_MODE_CL37BAM;
            break;
        case TEMOD_AN_MODE_HPAM:
            an->an_mode = phymod_AN_MODE_HPAM;
            break;
        case TEMOD_AN_MODE_SGMII:
            an->an_mode = phymod_AN_MODE_SGMII;
            break;
        default:
            if(an_control.an_type == TEMOD_AN_MODE_CL37_SGMII) {
                an->an_mode = phymod_AN_MODE_CL37_SGMII ;
            } else {
                an->an_mode = phymod_AN_MODE_NONE;
            }
            break;
    }

    return PHYMOD_E_NONE;
}


int tsce_phy_autoneg_status_get(const phymod_phy_access_t* phy, phymod_autoneg_status_t* status)
{
    /* Place your code here */

        
    return PHYMOD_E_NONE;
}


STATIC 
int _tsce_core_init_pass1(const phymod_core_access_t* core, 
                          const phymod_core_init_config_t* init_config, 
                          const phymod_core_status_t* core_status)
{
    phymod_phy_access_t phy_access;
    phymod_core_access_t  core_copy;
    uint32_t  uc_active = 0 ;

    TSCE_CORE_TO_PHY_ACCESS(&phy_access, core);
    PHYMOD_MEMCPY(&core_copy, core, sizeof(core_copy));
    core_copy.access.lane_mask = 0x1;

    PHYMOD_IF_ERR_RETURN(eagle_uc_active_get(&core_copy.access, &uc_active)) ;
    if(uc_active) {
        return(PHYMOD_E_NONE);
    }

    PHYMOD_IF_ERR_RETURN
        (temod_pmd_reset_seq(&core_copy.access, core_status->pmd_active));

    /* need to set the heart beat default is for 156.25M */
    PHYMOD_IF_ERR_RETURN (temod_refclk_set(&core_copy.access,
                          init_config->interface.ref_clock));  
    
    if (_tsce_core_firmware_load(&core_copy, init_config->firmware_load_method, init_config->firmware_loader)) {
            PHYMOD_DEBUG_ERROR(("devad 0x%"PRIx32" lane 0x%"PRIx32": UC firmware-load failed\n", core->access.addr, core->access.lane_mask));  
            PHYMOD_IF_ERR_RETURN (PHYMOD_E_INIT);
    }
    return (PHYMOD_E_NONE);
}

STATIC 
int _tsce_core_init_pass2(const phymod_core_access_t* core, 
                          const phymod_core_init_config_t* init_config, 
                          const phymod_core_status_t* core_status)
{
    phymod_phy_access_t phy_access, phy_access_copy;
    phymod_core_access_t  core_copy;
    phymod_firmware_core_config_t  firmware_core_config_tmp;

    TSCE_CORE_TO_PHY_ACCESS(&phy_access, core);
    PHYMOD_MEMCPY(&core_copy, core, sizeof(core_copy));
    core_copy.access.lane_mask = 0x1;

    phy_access_copy = phy_access;
    phy_access_copy.access = core->access;
    phy_access_copy.access.lane_mask = 0x1;
    phy_access_copy.type = core->type;

#ifndef TSCE_PMD_CRC_UCODE
    /* next we need to check if the load is correct or not */
    if (eagle_tsc_ucode_load_verify(&core_copy.access, (uint8_t *) &tsce_ucode, tsce_ucode_len)) {
        PHYMOD_DEBUG_ERROR(("devad 0x%x lane 0x%x: UC load-verify failed\n", core->access.addr, core->access.lane_mask));  
        PHYMOD_IF_ERR_RETURN (PHYMOD_E_INIT);
    }
#endif /* TSCE_PMD_CRC_UCODE */

    PHYMOD_IF_ERR_RETURN
        (eagle_pmd_ln_h_rstb_pkill_override( &phy_access_copy.access, 0x1));

    /* next we need to set the uc active and release uc */
    PHYMOD_IF_ERR_RETURN
        (eagle_uc_active_set(&core_copy.access ,1)); 

    /* release the uc reset */
    PHYMOD_IF_ERR_RETURN
        (eagle_uc_reset(&core_copy.access ,1)); 
    /* we need to wait at least 10ms for the uc to settle */
    /* PHYMOD_USLEEP(10000); */

    if(PHYMOD_CORE_INIT_F_FIRMWARE_LOAD_VERIFY_GET(init_config)) {
#ifndef TSCE_PMD_CRC_UCODE
        /* poll the ready bit in 10 ms */
        eagle_tsc_poll_uc_dsc_ready_for_cmd_equals_1(&phy_access_copy.access, 1);
#else
        PHYMOD_IF_ERR_RETURN(
                eagle_tsc_ucode_crc_verify( &core_copy.access, tsce_ucode_len,tsce_ucode_crc));
#endif /* TSCE_PMD_CRC_UCODE */
    }

    PHYMOD_IF_ERR_RETURN(
        eagle_pmd_ln_h_rstb_pkill_override( &phy_access_copy.access, 0x0));

    /* next check if in pcs-bypass mode */
    if (PHYMOD_DEVICE_OP_MODE_PCS_BYPASS_GET(core->device_op_mode)) {
        phy_access_copy.access.lane_mask = 0xf; 
        PHYMOD_IF_ERR_RETURN(
            temod_pcs_ilkn_mode_set(&phy_access_copy.access));
        phy_access_copy.access.lane_mask = 0x1;
    }             

    /* now config the lane mapping and polarity */
    PHYMOD_IF_ERR_RETURN
        (tsce_core_lane_map_set(core, &init_config->lane_map));

    /* if TSC12, set the mld phys map mode */
    if(PHYMOD_INTF_MODES_TRIPLE_CORE_GET(&init_config->interface)) {
        if(PHYMOD_INTF_MODES_TC_343_GET(&init_config->interface)) {
            PHYMOD_IF_ERR_RETURN
                (temod_tsc12_control(&core->access, -1, 0));
        } else if(PHYMOD_INTF_MODES_TC_343_GET(&init_config->interface)) {
            PHYMOD_IF_ERR_RETURN
                (temod_tsc12_control(&core->access, -1, 1));
        } else if(PHYMOD_INTF_MODES_TC_343_GET(&init_config->interface)) {
            PHYMOD_IF_ERR_RETURN
                (temod_tsc12_control(&core->access, -1, 2));
        }
    }

#if 0
  /* NEED TO REVISIT */
    PHYMOD_IF_ERR_RETURN
        (temod_autoneg_set_init(&core->access, &an));
#endif
    PHYMOD_IF_ERR_RETURN
        (temod_autoneg_timer_init(&core->access));

    /* mld has 2 reg copies */
    phy_access_copy.access.lane_mask = 0xf;
    PHYMOD_IF_ERR_RETURN
        (temod_mld_am_timers_set(&phy_access_copy.access));

    phy_access_copy.access.lane_mask = 0x1;

    PHYMOD_IF_ERR_RETURN
        (temod_master_port_num_set(&core->access, 0));
    PHYMOD_IF_ERR_RETURN
        (temod_cl48_lfrfli_init(&core->access));

    PHYMOD_IF_ERR_RETURN
        (eagle_core_soft_reset_release(&core_copy.access, 0));
    
    /* plldiv CONFIG */
    PHYMOD_IF_ERR_RETURN
        (temod_pll_config_set(&phy_access_copy.access, TEMOD_PLL_MODE_DIV_66, init_config->interface.ref_clock));

    /* don't overide the fw that set in config set if not specified */
    firmware_core_config_tmp = init_config->firmware_core_config;
    firmware_core_config_tmp.CoreConfigFromPCS = 0;
    /* set the vco rate to be default at 10.3125G */
    firmware_core_config_tmp.VcoRate = 0x13;

    PHYMOD_IF_ERR_RETURN
        (tsce_phy_firmware_core_config_set(&phy_access_copy, firmware_core_config_tmp)); 

   
    PHYMOD_IF_ERR_RETURN
        (temod_cl74_chng_default (&core_copy.access));

    /* release core soft reset */
    PHYMOD_IF_ERR_RETURN
        (eagle_core_soft_reset_release(&core_copy.access, 1));
        
    return PHYMOD_E_NONE;
}

int tsce_core_init(const phymod_core_access_t* core, const phymod_core_init_config_t* init_config, const phymod_core_status_t* core_status)
{
    if ( (!PHYMOD_CORE_INIT_F_EXECUTE_PASS1_GET(init_config) &&
          !PHYMOD_CORE_INIT_F_EXECUTE_PASS2_GET(init_config)) ||
        PHYMOD_CORE_INIT_F_EXECUTE_PASS1_GET(init_config)) {
        PHYMOD_IF_ERR_RETURN
            (_tsce_core_init_pass1(core, init_config, core_status));

        if (PHYMOD_CORE_INIT_F_EXECUTE_PASS1_GET(init_config)) {
            return PHYMOD_E_NONE;
        }
    }

    if ( (!PHYMOD_CORE_INIT_F_EXECUTE_PASS1_GET(init_config) &&
          !PHYMOD_CORE_INIT_F_EXECUTE_PASS2_GET(init_config)) ||
        PHYMOD_CORE_INIT_F_EXECUTE_PASS2_GET(init_config)) {
        PHYMOD_IF_ERR_RETURN
            (_tsce_core_init_pass2(core, init_config, core_status));
    }

    return PHYMOD_E_NONE;

}

int tsce_phy_init(const phymod_phy_access_t* phy, const phymod_phy_init_config_t* init_config)
{
    int pll_restart = 0;
    const phymod_access_t *pm_acc = &phy->access;
    phymod_phy_access_t pm_phy_copy;
    int start_lane, num_lane, i;
    int lane_bkup;
    phymod_polarity_t tmp_pol;
    PHYMOD_MEMSET(&tmp_pol, 0x0, sizeof(tmp_pol));
    PHYMOD_MEMCPY(&pm_phy_copy, phy, sizeof(pm_phy_copy));

    /* next program the tx fir taps and driver current based on the input */
    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(pm_acc, &start_lane, &num_lane));
    /* per lane based reset release */
    PHYMOD_IF_ERR_RETURN
        (temod_pmd_x4_reset(pm_acc));

    /* poll for per lane uc_dsc_ready */
    lane_bkup = pm_phy_copy.access.lane_mask;
    for (i = 0; i < num_lane; i++) {
        if (!PHYMOD_LANEPBMP_MEMBER(phy->access.lane_mask, start_lane + i)) {
            continue;
        }
        pm_phy_copy.access.lane_mask = 1 << (start_lane + i);
        PHYMOD_IF_ERR_RETURN
            (eagle_lane_soft_reset_release(&pm_phy_copy.access, 1));
    }
    pm_phy_copy.access.lane_mask = lane_bkup;

    /* program the rx/tx polarity */
    for (i = 0; i < num_lane; i++) {
        if (!PHYMOD_LANEPBMP_MEMBER(phy->access.lane_mask, start_lane + i)) {
            continue;
        }
        pm_phy_copy.access.lane_mask = 0x1 << (i + start_lane);
        tmp_pol.tx_polarity = (init_config->polarity.tx_polarity) >> i & 0x1;
        tmp_pol.rx_polarity = (init_config->polarity.rx_polarity) >> i & 0x1;
        PHYMOD_IF_ERR_RETURN
            (tsce_phy_polarity_set(&pm_phy_copy, &tmp_pol));
    }

    for (i = 0; i < num_lane; i++) {
        if (!PHYMOD_LANEPBMP_MEMBER(phy->access.lane_mask, start_lane + i)) {
            continue;
        }
        pm_phy_copy.access.lane_mask = 0x1 << (i + start_lane);
        PHYMOD_IF_ERR_RETURN
            (tsce_phy_tx_set(&pm_phy_copy, &init_config->tx[i]));
    }

    /* next check if pcs-bypass mode  */
    if (PHYMOD_DEVICE_OP_MODE_PCS_BYPASS_GET(phy->device_op_mode)) {
        PHYMOD_IF_ERR_RETURN
            (eagle_pmd_tx_disable_pin_dis_set(&phy->access, 1));
        PHYMOD_IF_ERR_RETURN
          (temod_init_pcs_ilkn(&phy->access));   
    }


    pm_phy_copy.access.lane_mask = 0x1;

    PHYMOD_IF_ERR_RETURN
        (temod_update_port_mode(pm_acc, &pll_restart));

    PHYMOD_IF_ERR_RETURN
        (temod_rx_lane_control_set(pm_acc, 1));
    PHYMOD_IF_ERR_RETURN
        (temod_tx_lane_control_set(pm_acc, TEMOD_TX_LANE_RESET_TRAFFIC_ENABLE));         /* TX_LANE_CONTROL */

    return PHYMOD_E_NONE;
}


int tsce_phy_loopback_set(const phymod_phy_access_t* phy, phymod_loopback_mode_t loopback, uint32_t enable)
{
    int start_lane, num_lane;
    int rv = PHYMOD_E_NONE;
    int i = 0;
    uint32_t cl72_en;
    phymod_phy_access_t phy_copy;

    const phymod_access_t *pm_acc;
    pm_acc = &phy->access;
    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));

    /* next figure out the lane num and start_lane based on the input */
    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(&phy->access, &start_lane, &num_lane));

    switch (loopback) {
    case phymodLoopbackGlobal :
        PHYMOD_IF_ERR_RETURN
            (tsce_phy_cl72_get(phy, &cl72_en));
        if((cl72_en == 1) && (enable == 1)) { 
             PHYMOD_DEBUG_ERROR(("adr=%0"PRIx32",lane 0x%x: Error! pcs gloop not supported with cl72 enabled\n",  pm_acc->addr, start_lane));  
             break;
        }
        PHYMOD_IF_ERR_RETURN(temod_tx_loopback_control(&phy->access, enable, start_lane, num_lane));
        break;
    case phymodLoopbackGlobalPMD :
        PHYMOD_IF_ERR_RETURN
            (tsce_phy_cl72_get(phy, &cl72_en));
        if((cl72_en == 1) && (enable == 1)) {
             PHYMOD_DEBUG_ERROR(("adr=%0"PRIx32",lane 0x%x: Error! pmd gloop not supported with cl72 enabled\n",  pm_acc->addr, start_lane));  
             break;
           }
        for (i = 0; i < num_lane; i++) {
            if (!PHYMOD_LANEPBMP_MEMBER(phy->access.lane_mask, start_lane + i)) {
                continue;
            }
            phy_copy.access.lane_mask = 0x1 << (i + start_lane);
            PHYMOD_IF_ERR_RETURN(temod_tx_squelch_set(&phy_copy.access, enable));
            PHYMOD_IF_ERR_RETURN(eagle_tsc_dig_lpbk(&phy_copy.access, (uint8_t) enable));
            PHYMOD_IF_ERR_RETURN(eagle_pmd_force_signal_detect(&phy_copy.access, (int) enable));
            PHYMOD_IF_ERR_RETURN(temod_rx_lane_control_set(&phy_copy.access, 1));
        }
        break;
    case phymodLoopbackRemotePMD :
        PHYMOD_IF_ERR_RETURN(eagle_tsc_rmt_lpbk(&phy->access, (uint8_t)enable));
        break;
    case phymodLoopbackRemotePCS :
        /* PHYMOD_IF_ERR_RETURN(temod_rx_loopback_control(&phy->access, enable, enable, enable)); */ /* RAVI */
        PHYMOD_RETURN_WITH_ERR(PHYMOD_E_CONFIG,
                               (_PHYMOD_MSG("PCS Remote LoopBack not supported")));
        break;
    default :
        break;
    }             
    return rv;
}

int tsce_phy_loopback_get(const phymod_phy_access_t* phy, phymod_loopback_mode_t loopback, uint32_t* enable)
{
    uint32_t enable_core;
    int start_lane, num_lane;

    *enable = 0;

    /* next figure out the lane num and start_lane based on the input */
    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(&phy->access, &start_lane, &num_lane));

    switch (loopback) {
    case phymodLoopbackGlobal :
        PHYMOD_IF_ERR_RETURN(temod_tx_loopback_get(&phy->access, &enable_core));
        *enable = (enable_core >> start_lane) & 0x1; 
        break;
    case phymodLoopbackGlobalPMD :
        PHYMOD_IF_ERR_RETURN(eagle_pmd_loopback_get(&phy->access, enable));
        break;
    case phymodLoopbackRemotePMD :
        PHYMOD_IF_ERR_RETURN(eagle_tsc_rmt_lpbk_get(&phy->access, enable));
        break;
    case phymodLoopbackRemotePCS :
        /* PHYMOD_IF_ERR_RETURN(temod_rx_loopback_control(&phy->access, enable, enable, enable)); */ /* RAVI */
        PHYMOD_RETURN_WITH_ERR(PHYMOD_E_CONFIG,
                               (_PHYMOD_MSG("PCS Remote LoopBack not supported")));
        break;
    default :
        break;
    }             
    return PHYMOD_E_NONE;
}


int tsce_phy_rx_pmd_locked_get(const phymod_phy_access_t* phy, uint32_t* rx_seq_done){
    PHYMOD_IF_ERR_RETURN(temod_pmd_lock_get(&phy->access, rx_seq_done));
    return PHYMOD_E_NONE;
}


int tsce_phy_link_status_get(const phymod_phy_access_t* phy, uint32_t* link_status)
{
    PHYMOD_IF_ERR_RETURN(temod_get_pcs_latched_link_status(&phy->access, link_status));
    return PHYMOD_E_NONE;
}


int tsce_phy_pcs_userspeed_set(const phymod_phy_access_t* phy, const phymod_pcs_userspeed_config_t* config)
{
    int rv ;  
    rv = PHYMOD_E_UNAVAIL ;
   
    switch(config->param){
    case phymodPcsUserSpeedParamEntry:
        /* no need */
        break ;
    case phymodPcsUserSpeedParamHCD:
        rv = TSCE_USERSPEED_SELECT(phy, config, TEMOD_OVERRIDE_SPDID) ;
        break ;
    case phymodPcsUserSpeedParamClear:
        if(config->mode==phymodPcsUserSpeedModeST) {
            rv = TSCE_USERSPEED_SELECT(phy, config, TEMOD_OVERRIDE_SPDID) ;
        }
        rv = TSCE_USERSPEED_SELECT(phy, config, TEMOD_OVERRIDE_RESET) ;
        break ;
    case phymodPcsUserSpeedParamPllDiv:
        /* not required */
        break ;
    case phymodPcsUserSpeedParamPmaOS:
        /* not required */
        break ;
    case phymodPcsUserSpeedParamScramble:
        rv = TSCE_USERSPEED_SELECT(phy, config, TEMOD_OVERRIDE_SCR_MODE) ;
        break ;
    case phymodPcsUserSpeedParamEncode:
        rv = TSCE_USERSPEED_SELECT(phy, config, TEMOD_OVERRIDE_ENCODE_MODE) ;
        break ;
    case phymodPcsUserSpeedParamCl48CheckEnd:
        rv = TSCE_USERSPEED_SELECT(phy, config, TEMOD_OVERRIDE_CHKEND_EN) ;
        break ;
    case phymodPcsUserSpeedParamBlkSync:
        rv = TSCE_USERSPEED_SELECT(phy, config, TEMOD_OVERRIDE_BLKSYNC_MODE) ;
        break ;
    case phymodPcsUserSpeedParamReorder:
        rv = TSCE_USERSPEED_SELECT(phy, config, TEMOD_OVERRIDE_REORDER_EN) ;
        break ;
    case phymodPcsUserSpeedParamCl36Enable:
        rv = TSCE_USERSPEED_SELECT(phy, config, TEMOD_OVERRIDE_CL36_EN) ;
        break ;
    case phymodPcsUserSpeedParamDescr1:
        rv = TSCE_USERSPEED_SELECT(phy, config, TEMOD_OVERRIDE_DESCR_MODE) ;
        break ;
    case phymodPcsUserSpeedParamDecode1:
        rv = TSCE_USERSPEED_SELECT(phy, config, TEMOD_OVERRIDE_DECODE_MODE) ;
        break ;
    case phymodPcsUserSpeedParamDeskew:
        rv = TSCE_USERSPEED_SELECT(phy, config, TEMOD_OVERRIDE_DESKEW_MODE) ;
        break ;
    case phymodPcsUserSpeedParamDescr2:
        rv = TSCE_USERSPEED_SELECT(phy, config, TEMOD_OVERRIDE_DESC2_MODE) ;
        break ;
    case phymodPcsUserSpeedParamDescr2ByteDel:
        rv = TSCE_USERSPEED_SELECT(phy, config, TEMOD_OVERRIDE_CL36BYTEDEL_MODE) ;
        break ;
    case phymodPcsUserSpeedParamBrcm64B66:
        rv = TSCE_USERSPEED_SELECT(phy, config, TEMOD_OVERRIDE_BRCM64B66_DESCR_MODE) ;
        break ;
    case phymodPcsUserSpeedParamSgmii:
        rv = TSCE_USERSPEED_CREDIT_SELECT(phy, config, TEMOD_CREDIT_SGMII_SPD) ;
        break ;
    case phymodPcsUserSpeedParamClkcnt0:
        rv = TSCE_USERSPEED_CREDIT_SELECT(phy, config, TEMOD_CREDIT_CLOCK_COUNT_0);
        break ;
    case phymodPcsUserSpeedParamClkcnt1:
        rv = TSCE_USERSPEED_CREDIT_SELECT(phy, config, TEMOD_CREDIT_CLOCK_COUNT_1);
        break ;
    case phymodPcsUserSpeedParamLpcnt0:
        rv = TSCE_USERSPEED_CREDIT_SELECT(phy, config, TEMOD_CREDIT_LOOP_COUNT_0);
        break ;
    case phymodPcsUserSpeedParamLpcnt1:
        rv = TSCE_USERSPEED_CREDIT_SELECT(phy, config, TEMOD_CREDIT_LOOP_COUNT_1);
        break ;
    case phymodPcsUserSpeedParamMacCGC:
        rv = TSCE_USERSPEED_CREDIT_SELECT(phy, config, TEMOD_CREDIT_MAC);
        break ;
    case phymodPcsUserSpeedParamRepcnt:
        rv = TSCE_USERSPEED_CREDIT_SELECT(phy, config, TEMOD_CREDIT_PCS_REPCNT);
        break ;
    case phymodPcsUserSpeedParamCrdtEn:
        rv = TSCE_USERSPEED_CREDIT_SELECT(phy, config, TEMOD_CREDIT_EN) ;
        break ;
    case phymodPcsUserSpeedParamPcsClkcnt:
        rv = TSCE_USERSPEED_CREDIT_SELECT(phy, config, TEMOD_CREDIT_PCS_CLOCK_COUNT_0);
        break ;
    case phymodPcsUserSpeedParamPcsCGC:
        rv = TSCE_USERSPEED_CREDIT_SELECT(phy, config, TEMOD_CREDIT_PCS_GEN_COUNT);
        break ;
    case phymodPcsUserSpeedParamCl72En:
        rv = TSCE_USERSPEED_SELECT(phy, config, TEMOD_OVERRIDE_CL72) ;
        break ;
    case phymodPcsUserSpeedParamNumOfLanes:
        rv = TSCE_USERSPEED_SELECT(phy, config, TEMOD_OVERRIDE_NUM_LANES) ;
        break ;
    default:
        break ;
    }
    return rv;
}

int tsce_phy_pcs_userspeed_get(const phymod_phy_access_t* phy, phymod_pcs_userspeed_config_t* config)
{
    return PHYMOD_E_UNAVAIL;
}

int tsce_phy_reg_read(const phymod_phy_access_t* phy, uint32_t reg_addr, uint32_t *val)
{
    PHYMOD_IF_ERR_RETURN(phymod_tsc_iblk_read(&phy->access, reg_addr, val));
    return PHYMOD_E_NONE;
}


int tsce_phy_reg_write(const phymod_phy_access_t* phy, uint32_t reg_addr, uint32_t val)
{
    PHYMOD_IF_ERR_RETURN(phymod_tsc_iblk_write(&phy->access, reg_addr, val));
    return PHYMOD_E_NONE;  
}

/* pass through LPI or convert LPI to IDEL*/
int tsce_phy_eee_set(const phymod_phy_access_t* phy, uint32_t enable)
{
    uint32_t lpi_bypass;
    int rv = PHYMOD_E_NONE ;
    lpi_bypass = PHYMOD_LPI_BYPASS_GET(enable);
    enable &= 0x1;
    if (lpi_bypass) {
        rv = temod_eee_control_set(&phy->access,enable);
    } else {
        return PHYMOD_E_UNAVAIL;
    }
    return rv;
}

int tsce_phy_eee_get(const phymod_phy_access_t* phy, uint32_t *enable)
{
    if (PHYMOD_LPI_BYPASS_GET(*enable)) {
        PHYMOD_IF_ERR_RETURN(temod_eee_control_get(&phy->access, enable));
        PHYMOD_LPI_BYPASS_SET(*enable);
    } else {
        return PHYMOD_E_UNAVAIL;
    }
    return PHYMOD_E_NONE;
}

int tsce_phy_rx_adaptation_resume(const phymod_phy_access_t* phy)
{
    uint8_t uc_lane_stopped;
    PHYMOD_IF_ERR_RETURN(eagle_tsc_stop_uc_lane_status(&phy->access, &uc_lane_stopped));
    if (uc_lane_stopped) {
        PHYMOD_IF_ERR_RETURN(eagle_tsc_stop_rx_adaptation(&phy->access, 0)); 
    }
    return PHYMOD_E_NONE;
}

int tsce_phy_autoneg_oui_set(const phymod_phy_access_t* phy, phymod_autoneg_oui_t an_oui) {
   temod_an_oui_t oui;

   oui.oui                    = an_oui.oui;
   oui.oui_override_hpam_adv  = an_oui.oui_override_hpam_adv;
   oui.oui_override_hpam_det  = an_oui.oui_override_hpam_det;
   oui.oui_override_bam73_adv = an_oui.oui_override_bam73_adv;
   oui.oui_override_bam73_det = an_oui.oui_override_bam73_det;
   oui.oui_override_bam37_adv = an_oui.oui_override_bam37_adv;
   oui.oui_override_bam37_det = an_oui.oui_override_bam37_det;

   PHYMOD_IF_ERR_RETURN(temod_an_oui_set(&phy->access, oui));

   return PHYMOD_E_NONE;
}

int tsce_phy_autoneg_oui_get(const phymod_phy_access_t* phy, phymod_autoneg_oui_t* an_oui) {
   temod_an_oui_t oui;

   PHYMOD_IF_ERR_RETURN(temod_an_oui_get(&phy->access, &oui));

   an_oui->oui_override_hpam_adv  = oui.oui_override_hpam_adv;
   an_oui->oui_override_hpam_det  = oui.oui_override_hpam_det;
   an_oui->oui_override_bam73_adv = oui.oui_override_bam73_adv;
   an_oui->oui_override_bam73_det = oui.oui_override_bam73_det;
   an_oui->oui_override_bam37_adv = oui.oui_override_bam37_adv;
   an_oui->oui_override_bam37_det = oui.oui_override_bam37_det;

   return PHYMOD_E_NONE;
}

int tsce_phy_rx_signal_detect_get(const phymod_phy_access_t* phy,  uint32_t* value)
{
    uint32_t local_rx_signal_det;
    int start_lane, num_lane, i;

    *value = 1;

    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(&phy->access, &start_lane, &num_lane));

    for (i = 0; i < num_lane; i++) {
        if (!PHYMOD_LANEPBMP_MEMBER(phy->access.lane_mask, start_lane + i)) {
            continue;
        }
        PHYMOD_IF_ERR_RETURN
            (eagle_tsc_signal_detect(&phy->access, &local_rx_signal_det));
        *value = *value & local_rx_signal_det;
    }

    return PHYMOD_E_NONE;
}

int tsce_phy_eye_margin_est_get(const phymod_phy_access_t* phy, phymod_eye_margin_mode_t eye_margin_mode, uint32_t* value) 
{
    int start_lane, num_lane;
    phymod_phy_access_t phy_copy;
    int hz_l, hz_r, vt_u, vt_d;


    /* first get the start_lane */
    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(&phy->access, &start_lane, &num_lane));

    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));
    phy_copy.access.lane_mask = 0x1 << start_lane;

    PHYMOD_IF_ERR_RETURN
        (eagle_tsc_get_eye_margin_est(&phy_copy.access, &hz_l, &hz_r, &vt_u, &vt_d));

    switch (eye_margin_mode) {
    case phymod_eye_marign_HZ_L:
        *value = hz_l;
        break;
    case phymod_eye_marign_HZ_R:
        *value = hz_r;
        break;
    case phymod_eye_marign_VT_U:
        *value = vt_u;
        break;
    case phymod_eye_marign_VT_D:
        *value = vt_d;
        break;
    default:
        *value = 0;
        break;
    }

    return PHYMOD_E_NONE;
}

int tsce_phy_hg2_codec_control_set(const phymod_phy_access_t* phy,
                                   phymod_phy_hg2_codec_t hg2_codec)
{
    temod_hg2_codec_t local_copy;
    switch (hg2_codec) {
        case phymodBcmHG2CodecOff:
            local_copy = TEMOD_HG2_CODEC_OFF;
            break;
        case phymodBcmHG2CodecOnWith8ByteIPG:
            local_copy = TEMOD_HG2_CODEC_ON_8BYTE_IPG;
            break;
        case phymodBcmHG2CodecOnWith9ByteIPG:
            local_copy = TEMOD_HG2_CODEC_ON_9BYTE_IPG;
            break;
        default:
            local_copy = TEMOD_HG2_CODEC_OFF;
            break;
        }
    PHYMOD_IF_ERR_RETURN(temod_hg2_codec_set(&phy->access, local_copy));
    return PHYMOD_E_NONE;
}

int tsce_phy_hg2_codec_control_get(const phymod_phy_access_t* phy,
                                   phymod_phy_hg2_codec_t* hg2_codec)
{
    temod_hg2_codec_t local_copy;

    PHYMOD_IF_ERR_RETURN(temod_hg2_codec_get(&phy->access, &local_copy));

    switch (local_copy) {
        case TEMOD_HG2_CODEC_OFF:
            *hg2_codec = phymodBcmHG2CodecOff;
            break;
        case TEMOD_HG2_CODEC_ON_8BYTE_IPG:
            *hg2_codec = phymodBcmHG2CodecOnWith8ByteIPG;
            break;
        case TEMOD_HG2_CODEC_ON_9BYTE_IPG:
            *hg2_codec = phymodBcmHG2CodecOnWith9ByteIPG;
            break;
        default:
            *hg2_codec = phymodBcmHG2CodecOff;
            break;
        }
    return PHYMOD_E_NONE;
}

int tsce_phy_rx_ppm_get(const phymod_phy_access_t* phy, int16_t* rx_ppm)
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

int tsce_phy_master_set(const phymod_phy_access_t* phy, phymod_master_mode_t master_mode)
{
    int start_lane, num_lane;
    phymod_phy_access_t phy_copy;
    int ms_mode = TEMOD_SGMII_SLAVE;

    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));

    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(&phy->access, &start_lane, &num_lane));

    phy_copy.access.lane_mask = 1 << start_lane;
    switch (master_mode) {
        case phymodMSSlave:
            ms_mode = TEMOD_SGMII_SLAVE;
            break;
        case phymodMSMaster:
            ms_mode = TEMOD_SGMII_MASTER;
            break;
        default:
            return PHYMOD_E_PARAM;
    }
    PHYMOD_IF_ERR_RETURN
        (temod_sgmii_master_set(&phy_copy.access, ms_mode));

    return PHYMOD_E_NONE;

}

int tsce_phy_master_get(const phymod_phy_access_t* phy, phymod_master_mode_t *master_mode)
{
    int start_lane, num_lane;
    phymod_phy_access_t phy_copy;
    int ms_mode;

    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));

    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(&phy->access, &start_lane, &num_lane));

    phy_copy.access.lane_mask = 1 << start_lane;
    PHYMOD_IF_ERR_RETURN
        (temod_sgmii_master_get(&phy_copy.access, &ms_mode));
    switch (ms_mode) {
        case TEMOD_SGMII_SLAVE:
            *master_mode = phymodMSSlave;
            break;
        case TEMOD_SGMII_MASTER:
            *master_mode = phymodMSMaster;
            break;
        default:
            *master_mode = phymodMSNone;
            break;
    }

    return PHYMOD_E_NONE;

}

int tsce_phy_synce_clk_ctrl_set(const phymod_phy_access_t* phy,
                                  phymod_synce_clk_ctrl_t cfg)
{
    phymod_phy_access_t phy_copy;
    int start_lane, num_lane;

    /* TSCE only supports stage1 divider mode */
    if (cfg.stg0_mode != -1 || cfg.sdm_val != 0) {
        return PHYMOD_E_UNAVAIL;
    }

    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(&phy->access, &start_lane, &num_lane));

    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));
    phy_copy.access.lane_mask = 0x1 << start_lane;

    PHYMOD_IF_ERR_RETURN
        (temod_synce_mode_set(&phy_copy.access, cfg.stg1_mode));

    return PHYMOD_E_NONE;
}

int tsce_phy_synce_clk_ctrl_get(const phymod_phy_access_t* phy,
                                  phymod_synce_clk_ctrl_t *cfg)
{
    PHYMOD_IF_ERR_RETURN
        (temod_synce_mode_get(&phy->access, &(cfg->stg1_mode)));

    return PHYMOD_E_NONE;
}
