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
#include <phymod/chip/falcon_dpll.h>
#include <phymod/chip/tscf_gen3.h>
#include <phymod/chip/bcmi_tscf_gen3_xgxs_defs.h>
#include "tscf_gen3/tier1/tefmod_gen3.h"
#include "tscf_gen3/tier1/tefmod_gen3_enum_defines.h"
#include "tscf_gen3/tier1/tefmod_gen3_sc_lkup_table.h"
#include "falcon_dpll/tier1/falcon2_monterey_interface.h"
#include "falcon_dpll/tier1/falcon2_monterey_debug_functions.h"
#include "falcon_dpll/tier1/falcon_api_uc_common.h"
#include "falcon_dpll/tier1/falcon_dpll_cfg_seq.h"


#define TSCF_GEN3_ID0                 	0x600d
#define TSCF_GEN3_ID1                 	0x8770
#define TSC4F_GEN3_MODEL              	0x24
#define TSCF_GEN3_TECH_PROC           	0x3
#define TSCF_GEN3_NOF_LANES_IN_CORE     4
#define TSCF_GEN3_LANE_SWAP_LANE_MASK   3
#define TSCF_GEN3_NOF_DFES              6
#define TSCF_GEN3_PHY_ALL_LANES         0xf

#define TSCF_GEN3_PMD_CRC_UCODE_VERIFY  1

/* uController's firmware */
extern unsigned char falcon_dpll_ucode[];
extern unsigned short falcon_dpll_ucode_ver;
extern unsigned short falcon_dpll_ucode_crc;
extern unsigned short falcon_dpll_ucode_len;


#define TSCF_GEN3_CORE_TO_PHY_ACCESS(_phy_access, _core_access) \
    do{\
        PHYMOD_MEMCPY(&(_phy_access)->access, &(_core_access)->access, sizeof((_phy_access)->access));\
        (_phy_access)->type           = (_core_access)->type; \
        (_phy_access)->port_loc       = (_core_access)->port_loc; \
        (_phy_access)->device_op_mode = (_core_access)->device_op_mode; \
        (_phy_access)->access.lane_mask = TSCF_GEN3_PHY_ALL_LANES; \
    }while(0)

STATIC
int _tscf_gen3_phy_firmware_lane_config_set(const phymod_phy_access_t* phy, phymod_firmware_lane_config_t fw_config)
{
    struct falcon2_monterey_uc_lane_config_st serdes_firmware_config;
    phymod_phy_access_t phy_copy;
    int start_lane, num_lane, i;
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
        serdes_firmware_config.field.dfe_lp_mode       = fw_config.LpDfeOn;
        serdes_firmware_config.field.force_brdfe_on    = fw_config.ForceBrDfe;
        serdes_firmware_config.field.media_type        = fw_config.MediaType;
        serdes_firmware_config.field.unreliable_los    = fw_config.UnreliableLos;
        serdes_firmware_config.field.scrambling_dis    = fw_config.ScramblingDisable;
        serdes_firmware_config.field.cl72_auto_polarity_en   = fw_config.Cl72AutoPolEn;
        serdes_firmware_config.field.cl72_restart_timeout_en = fw_config.Cl72RestTO;
        /*serdes_firmware_config.field.en_short_chn_osx1 = fw_config.short_chn;*/

        PHYMOD_IF_ERR_RETURN(PHYMOD_IS_WRITE_DISABLED(&phy_copy.access, &is_warm_boot));

        if(!is_warm_boot) {
            PHYMOD_IF_ERR_RETURN (falcon2_monterey_lane_soft_reset_release(&phy_copy.access, 0));
            PHYMOD_IF_ERR_RETURN(falcon2_monterey_set_uc_lane_cfg(&phy_copy.access, serdes_firmware_config));
            PHYMOD_IF_ERR_RETURN (falcon2_monterey_lane_soft_reset_release(&phy_copy.access, 1));

        }
    }

    return PHYMOD_E_NONE;
}

int tscf_gen3_core_identify(const phymod_core_access_t* core, uint32_t core_id, uint32_t* is_identified)
{
    int ioerr = 0;
    const phymod_access_t *pm_acc = &core->access;
    PHYID2r_t id2;
    PHYID3r_t id3;
    MAIN0_SERDESIDr_t serdes_id;
    uint32_t model;

    *is_identified = 0;

    if (core_id == 0){
        ioerr += READ_PHYID2r(pm_acc, &id2);
        ioerr += READ_PHYID3r(pm_acc, &id3);
    } else {
        PHYID2r_SET(id2, ((core_id >> 16) & 0xffff));
        PHYID3r_SET(id3, core_id & 0xffff);
    }

    if (PHYID2r_REGID1f_GET(id2) == TSCF_GEN3_ID0 &&
       (PHYID3r_REGID2f_GET(id3) == TSCF_GEN3_ID1)) {
        /* PHY IDs match - now check SERDES ID model number */
        ioerr += READ_MAIN0_SERDESIDr(pm_acc, &serdes_id);
        model = MAIN0_SERDESIDr_MODEL_NUMBERf_GET(serdes_id);
        if (model == TSC4F_GEN3_MODEL)  {
            if (MAIN0_SERDESIDr_TECH_PROCf_GET(serdes_id) == TSCF_GEN3_TECH_PROC) {
                *is_identified = 1;
            }
        }
    }

    return ioerr ? PHYMOD_E_IO : PHYMOD_E_NONE;
}

int tscf_gen3_core_info_get(const phymod_core_access_t* core, phymod_core_info_t* info)
{        
    uint32_t serdes_id = TSC4F_GEN3_MODEL; 
    char core_name[15] = "TscfGen3";
    PHYID2r_t id2;
    PHYID3r_t id3;
    const phymod_access_t *pm_acc = &core->access;

     PHYMOD_IF_ERR_RETURN
        (tefmod_gen3_revid_read(&core->access, &serdes_id));
    PHYMOD_IF_ERR_RETURN
        (phymod_core_name_get(core, serdes_id, core_name, info));
    info->serdes_id = serdes_id;
    info->core_version = phymodCoreVersionTscfGen3;

    PHYMOD_IF_ERR_RETURN(READ_PHYID2r(pm_acc, &id2));
    PHYMOD_IF_ERR_RETURN(READ_PHYID3r(pm_acc, &id3));

    info->phy_id0 = (uint16_t) id2.v[0];
    info->phy_id1 = (uint16_t) id3.v[0];

    return PHYMOD_E_NONE;
}

/* pcs_or_pmd = 1 => do the PCS lane swap
   pcs_or_pmd = 0 => do the PMD lane swap */
static int _tscf_gen3_core_lane_map_part_set(const phymod_core_access_t* core, const phymod_lane_map_t* lane_map, int pcs_or_pmd)
{
    uint32_t lane, pcs_tx_swap = 0, pcs_rx_swap = 0;
    uint8_t pmd_tx_addr[4], pmd_rx_addr[4];

    if (lane_map->num_of_lanes != TSCF_GEN3_NOF_LANES_IN_CORE){
        return PHYMOD_E_CONFIG;
    }
    for (lane = 0; lane < TSCF_GEN3_NOF_LANES_IN_CORE; lane++){
        if ((lane_map->lane_map_tx[lane] >= TSCF_GEN3_NOF_LANES_IN_CORE)||
             (lane_map->lane_map_rx[lane] >= TSCF_GEN3_NOF_LANES_IN_CORE)){
            return PHYMOD_E_CONFIG;
        }
        /* Encode each lane as four bits */
        pcs_tx_swap += lane_map->lane_map_tx[lane]<<(lane*4);
        pcs_rx_swap += lane_map->lane_map_rx[lane]<<(lane*4);
    }
    /* PMD lane addr is based on PCS logical to physical mapping */
    for (lane = 0; lane < TSCF_GEN3_NOF_LANES_IN_CORE; lane++){
        pmd_tx_addr[((pcs_tx_swap >> (lane*4)) & 0xf)] = lane;
        pmd_rx_addr[((pcs_rx_swap >> (lane*4)) & 0xf)] = lane;
    }

    if(pcs_or_pmd) {
        PHYMOD_IF_ERR_RETURN
            (tefmod_gen3_pcs_tx_lane_swap(&core->access, pcs_tx_swap));
        PHYMOD_IF_ERR_RETURN
            (tefmod_gen3_pcs_rx_lane_swap(&core->access, pcs_rx_swap));
    } else {
        PHYMOD_IF_ERR_RETURN
            (falcon2_monterey_map_lanes(&core->access, TSCF_GEN3_NOF_LANES_IN_CORE, pmd_tx_addr, pmd_rx_addr));
    }

    return PHYMOD_E_NONE;
}

int tscf_gen3_core_lane_map_set(const phymod_core_access_t* core, const phymod_lane_map_t* lane_map)
{
   PHYMOD_IF_ERR_RETURN
       (_tscf_gen3_core_lane_map_part_set(core, lane_map, 1));
   PHYMOD_IF_ERR_RETURN
       (_tscf_gen3_core_lane_map_part_set(core, lane_map, 0));

    return PHYMOD_E_NONE;
}

int tscf_gen3_core_lane_map_get(const phymod_core_access_t* core, phymod_lane_map_t* lane_map)
{        
    uint32_t pcs_tx_swap = 0 , pcs_rx_swap = 0, lane;

    PHYMOD_IF_ERR_RETURN(tefmod_gen3_pcs_tx_lane_swap_get(&core->access, &pcs_tx_swap));
    PHYMOD_IF_ERR_RETURN(tefmod_gen3_pcs_rx_lane_swap_get(&core->access, &pcs_rx_swap));

    for (lane = 0; lane < TSCF_GEN3_NOF_LANES_IN_CORE ; lane++){
        /* Decode each lane from four bits */
        lane_map->lane_map_tx[lane] = (pcs_tx_swap>>(lane*4)) & TSCF_GEN3_LANE_SWAP_LANE_MASK;
        lane_map->lane_map_rx[lane] = (pcs_rx_swap>>(lane*4)) & TSCF_GEN3_LANE_SWAP_LANE_MASK;
    }
    lane_map->num_of_lanes = TSCF_GEN3_NOF_LANES_IN_CORE;
 
    return PHYMOD_E_NONE;
}


int tscf_gen3_core_reset_set(const phymod_core_access_t* core, phymod_reset_mode_t reset_mode, phymod_reset_direction_t direction)
{        
    
    return PHYMOD_E_UNAVAIL;
    
}

int tscf_gen3_core_reset_get(const phymod_core_access_t* core, phymod_reset_mode_t reset_mode, phymod_reset_direction_t* direction)
{        
    
    return PHYMOD_E_UNAVAIL;

}

int tscf_gen3_core_firmware_info_get(const phymod_core_access_t* core, phymod_core_firmware_info_t* fw_info)
{        
    
    return PHYMOD_E_NONE;
    
}

STATIC
int _tscf_gen3_core_firmware_load(const phymod_core_access_t* core, phymod_firmware_load_method_t load_method, phymod_firmware_loader_f fw_loader) 
{
    /* coverity[assignment:FALSE] */
    load_method = phymodFirmwareLoadMethodInternal;
    /* coverity[const:FALSE] */
    /* coverity[dead_error_condition:FALSE] */
    switch(load_method){
    case phymodFirmwareLoadMethodInternal:
        PHYMOD_IF_ERR_RETURN(falcon2_monterey_ucode_mdio_load(&core->access, falcon_dpll_ucode, falcon_dpll_ucode_len));
        break;
    case phymodFirmwareLoadMethodExternal:
        PHYMOD_NULL_CHECK(fw_loader);
        PHYMOD_IF_ERR_RETURN(falcon2_monterey_ucode_init(&core->access));
        PHYMOD_IF_ERR_RETURN
            (falcon2_monterey_pram_firmware_enable(&core->access, 1, 0));
        PHYMOD_IF_ERR_RETURN(fw_loader(core, falcon_dpll_ucode_len, falcon_dpll_ucode));
        PHYMOD_IF_ERR_RETURN
            (falcon2_monterey_pram_firmware_enable(&core->access, 0, 0));
        break;
    /* coverity[DEADCODE:FALSE] */
    case phymodFirmwareLoadMethodNone:
        break;
    /* coverity[dead_event_begin:FALSE] */
    default:
        PHYMOD_RETURN_WITH_ERR(PHYMOD_E_CONFIG, (_PHYMOD_MSG("illegal fw load method %u"), load_method));
    }

    return PHYMOD_E_NONE;
}

int tscf_gen3_phy_firmware_core_config_set(const phymod_phy_access_t* phy, phymod_firmware_core_config_t fw_core_config)
{        
    struct falcon2_monterey_uc_core_config_st serdes_firmware_core_config;   

    PHYMOD_MEMSET(&serdes_firmware_core_config, 0, sizeof(serdes_firmware_core_config));
    serdes_firmware_core_config.field.core_cfg_from_pcs = fw_core_config.CoreConfigFromPCS;
    serdes_firmware_core_config.field.vco_rate = fw_core_config.VcoRate;
    serdes_firmware_core_config.field.disable_write_pll_iqp = fw_core_config.disable_write_pll_iqp;
    PHYMOD_IF_ERR_RETURN(falcon2_monterey_set_uc_core_config(&phy->access, serdes_firmware_core_config));

    return PHYMOD_E_NONE;
}

int tscf_gen3_phy_firmware_core_config_get(const phymod_phy_access_t* phy, phymod_firmware_core_config_t* fw_core_config)
{        
    struct falcon2_monterey_uc_core_config_st serdes_firmware_core_config;    

    PHYMOD_MEMSET(fw_core_config, 0, sizeof(*fw_core_config));
    PHYMOD_IF_ERR_RETURN(falcon2_monterey_get_uc_core_config(&phy->access, &serdes_firmware_core_config));
    fw_core_config->CoreConfigFromPCS = serdes_firmware_core_config.field.core_cfg_from_pcs;
    fw_core_config->VcoRate = serdes_firmware_core_config.field.vco_rate;
    fw_core_config->disable_write_pll_iqp = serdes_firmware_core_config.field.disable_write_pll_iqp;
        
    return PHYMOD_E_NONE;
}

int tscf_gen3_phy_firmware_lane_config_set(const phymod_phy_access_t* phy, phymod_firmware_lane_config_t fw_lane_config)
{        
    phymod_phy_access_t phy_copy; 
    int start_lane, num_lane, i;
             
    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(&phy->access, &start_lane, &num_lane));
    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));

    /* Hold the per lane soft reset bit */
    for (i = 0; i < num_lane; i++) {
        if (!PHYMOD_LANEPBMP_MEMBER(phy->access.lane_mask, start_lane + i)) {
            continue;
        }
        phy_copy.access.lane_mask = 1 << (start_lane + i);
        PHYMOD_IF_ERR_RETURN
            (falcon2_monterey_lane_soft_reset_release(&phy_copy.access, 0));
    }

    PHYMOD_IF_ERR_RETURN
         (_tscf_gen3_phy_firmware_lane_config_set(phy, fw_lane_config));

    /* Release the per lane soft reset bit */
    for (i = 0; i < num_lane; i++) {
        if (!PHYMOD_LANEPBMP_MEMBER(phy->access.lane_mask, start_lane + i)) {
            continue;
        }
        phy_copy.access.lane_mask = 1 << (start_lane + i);
        PHYMOD_IF_ERR_RETURN
            (falcon2_monterey_lane_soft_reset_release(&phy_copy.access, 1));
    }

    /* Toggle the pcs data path reset */
    PHYMOD_IF_ERR_RETURN
        (tefmod_gen3_trigger_speed_change(&phy->access));

    return PHYMOD_E_NONE;
}

int tscf_gen3_phy_firmware_lane_config_get(const phymod_phy_access_t* phy, phymod_firmware_lane_config_t* fw_lane_config)
{        
    struct falcon2_monterey_uc_lane_config_st serdes_firmware_config;
    phymod_phy_access_t phy_copy;
    int start_lane, num_lane;

    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));
    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(&phy->access, &start_lane, &num_lane));

    phy_copy.access.lane_mask = 1 << start_lane ;

    PHYMOD_MEMSET(&serdes_firmware_config, 0x0, sizeof(serdes_firmware_config));
    PHYMOD_IF_ERR_RETURN(falcon2_monterey_get_uc_lane_cfg(&phy_copy.access, &serdes_firmware_config));
    PHYMOD_MEMSET(fw_lane_config, 0, sizeof(*fw_lane_config));
    fw_lane_config->LaneConfigFromPCS = serdes_firmware_config.field.lane_cfg_from_pcs;
    fw_lane_config->AnEnabled         = serdes_firmware_config.field.an_enabled;
    fw_lane_config->DfeOn             = serdes_firmware_config.field.dfe_on;
    fw_lane_config->LpDfeOn           = serdes_firmware_config.field.dfe_lp_mode;
    fw_lane_config->ForceBrDfe        = serdes_firmware_config.field.force_brdfe_on;
    fw_lane_config->ScramblingDisable = serdes_firmware_config.field.scrambling_dis;
    fw_lane_config->UnreliableLos     = serdes_firmware_config.field.unreliable_los;
    fw_lane_config->MediaType         = serdes_firmware_config.field.media_type;
    fw_lane_config->Cl72AutoPolEn     = serdes_firmware_config.field.cl72_auto_polarity_en;
    fw_lane_config->Cl72RestTO        = serdes_firmware_config.field.cl72_restart_timeout_en;
        
    return PHYMOD_E_NONE;
}

int tscf_gen3_core_pll_sequencer_restart(const phymod_core_access_t* core, uint32_t flags, phymod_sequencer_operation_t operation)
{        
    
    return PHYMOD_E_UNAVAIL;
    
}

int tscf_gen3_core_wait_event(const phymod_core_access_t* core, phymod_core_event_t event, uint32_t timeout)
{        
    
    return PHYMOD_E_NONE;
    
}

int tscf_gen3_phy_rx_restart(const phymod_phy_access_t* phy)
{        
    
    PHYMOD_IF_ERR_RETURN(falcon2_monterey_rx_restart(&phy->access, 1));

    return PHYMOD_E_NONE;
}

int tscf_gen3_phy_polarity_set(const phymod_phy_access_t* phy, const phymod_polarity_t* polarity)
{        
    PHYMOD_IF_ERR_RETURN
        (tefmod_gen3_tx_rx_polarity_set(&phy->access, polarity->tx_polarity, polarity->rx_polarity));

    return PHYMOD_E_NONE;
}

int tscf_gen3_phy_polarity_get(const phymod_phy_access_t* phy, phymod_polarity_t* polarity)
{        
    PHYMOD_IF_ERR_RETURN
        (tefmod_gen3_tx_rx_polarity_get(&phy->access, &polarity->tx_polarity, &polarity->rx_polarity));
        
    return PHYMOD_E_NONE;
}

STATIC
int _tscf_gen3_phy_drivermode_phymod_to_tier1(int8_t phymod_drivermode, int8_t* drivermode)
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
int _tscf_gen3_phy_drivermode_tier1_to_phymod(int8_t drivermode, int8_t* phymod_drivermode)
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

int tscf_gen3_phy_tx_set(const phymod_phy_access_t* phy, const phymod_tx_t* tx)
{
    int8_t drivermode;

    PHYMOD_IF_ERR_RETURN
        (falcon2_monterey_write_tx_afe(&phy->access, TX_AFE_PRE, (int8_t)tx->pre));
    PHYMOD_IF_ERR_RETURN
        (falcon2_monterey_write_tx_afe(&phy->access, TX_AFE_MAIN, (int8_t)tx->main));
    PHYMOD_IF_ERR_RETURN
        (falcon2_monterey_write_tx_afe(&phy->access, TX_AFE_POST1, (int8_t)tx->post));
    PHYMOD_IF_ERR_RETURN
        (falcon2_monterey_write_tx_afe(&phy->access, TX_AFE_POST2, (int8_t)tx->post2));
    PHYMOD_IF_ERR_RETURN
        (falcon2_monterey_write_tx_afe(&phy->access, TX_AFE_POST3, (int8_t)tx->post3));
    PHYMOD_IF_ERR_RETURN
        (falcon2_monterey_write_tx_afe(&phy->access, TX_AFE_AMP,  (int8_t)tx->amp));
    if((tx->drivermode != -1) &&
       (phy->device_op_mode & PHYMOD_INTF_CONFIG_TX_FIR_DRIVERMODE_ENABLE)) {
        PHYMOD_IF_ERR_RETURN
            (_tscf_gen3_phy_drivermode_phymod_to_tier1(tx->drivermode, &drivermode));
        PHYMOD_IF_ERR_RETURN
            (falcon2_monterey_write_tx_afe(&phy->access, TX_AFE_DRIVERMODE, drivermode));
    }

    return PHYMOD_E_NONE;

}

int tscf_gen3_phy_tx_get(const phymod_phy_access_t* phy, phymod_tx_t* tx)
{
    int8_t value = 0;
    int8_t drivermode;

    PHYMOD_IF_ERR_RETURN
        (falcon2_monterey_read_tx_afe(&phy->access, TX_AFE_PRE, &value));
    tx->pre = value;
    PHYMOD_IF_ERR_RETURN
        (falcon2_monterey_read_tx_afe(&phy->access, TX_AFE_MAIN, &value));
    tx->main = value;
    PHYMOD_IF_ERR_RETURN
        (falcon2_monterey_read_tx_afe(&phy->access, TX_AFE_POST1, &value));
    tx->post = value;
    PHYMOD_IF_ERR_RETURN
        (falcon2_monterey_read_tx_afe(&phy->access, TX_AFE_POST2, &value));
    tx->post2 = value;
    PHYMOD_IF_ERR_RETURN
        (falcon2_monterey_read_tx_afe(&phy->access, TX_AFE_POST3, &value));
    tx->post3 = value;
    PHYMOD_IF_ERR_RETURN
        (falcon2_monterey_read_tx_afe(&phy->access, TX_AFE_AMP, &value));
    tx->amp = value;
    PHYMOD_IF_ERR_RETURN
        (falcon2_monterey_read_tx_afe(&phy->access, TX_AFE_DRIVERMODE, &drivermode));
    PHYMOD_IF_ERR_RETURN
        (_tscf_gen3_phy_drivermode_tier1_to_phymod(drivermode, &value));
    tx->drivermode = value;

    return PHYMOD_E_NONE;
}


int tscf_gen3_phy_tx_override_set(const phymod_phy_access_t* phy, const phymod_tx_override_t* tx_override)
{        
    PHYMOD_IF_ERR_RETURN
        (falcon2_monterey_tx_pi_freq_override(&phy->access,
                                    tx_override->phase_interpolator.enable,
                                    tx_override->phase_interpolator.value));
 
    return PHYMOD_E_NONE;
    
}

int tscf_gen3_phy_tx_override_get(const phymod_phy_access_t* phy, phymod_tx_override_t* tx_override)
{        
    int16_t pi_value;

    PHYMOD_IF_ERR_RETURN
        (falcon2_monterey_tx_pi_control_get(&phy->access, &pi_value));
    tx_override->phase_interpolator.value = (int32_t) pi_value;

 return PHYMOD_E_NONE;
    
}

int tscf_gen3_phy_txpi_config_set(const phymod_phy_access_t* phy, const phymod_txpi_config_t* config)
{
    phymod_phy_access_t phy_copy;

    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));

    PHYMOD_IF_ERR_RETURN
         (falcon2_monterey_tx_pi_enable_set(&phy_copy.access, config->enable));

    PHYMOD_IF_ERR_RETURN
         (falcon2_monterey_tx_pi_ext_pd_select_set(&phy_copy.access, config->mode == PHYMOD_TXPI_EXT_PD_MODE ? 1 : 0));

    return PHYMOD_E_NONE;
}

int tscf_gen3_phy_txpi_config_get(const phymod_phy_access_t* phy, phymod_txpi_config_t* config)
{
    phymod_phy_access_t phy_copy;
    uint32_t ext_pd;

    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));

    PHYMOD_IF_ERR_RETURN
         (falcon2_monterey_tx_pi_enable_get(&phy_copy.access, &config->enable));

    PHYMOD_IF_ERR_RETURN
         (falcon2_monterey_tx_pi_ext_pd_select_get(&phy_copy.access, &ext_pd));

    config->mode = ext_pd ? PHYMOD_TXPI_EXT_PD_MODE : PHYMOD_TXPI_NORMAL_MODE;

    return PHYMOD_E_NONE;
}

int tscf_gen3_phy_rx_set(const phymod_phy_access_t* phy, const phymod_rx_t* rx)
{        
    uint32_t i;
    uint8_t uc_lane_stopped;
    phymod_phy_access_t pm_phy_copy;
    int start_lane, num_lane, k;

    PHYMOD_MEMCPY(&pm_phy_copy, phy, sizeof(pm_phy_copy));
    /* Program the tx fir taps and driver current based on the input */
    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(&phy->access, &start_lane, &num_lane));

    /* Check params */
    if((rx->num_of_dfe_taps == 0) || (rx->num_of_dfe_taps > TSCF_GEN3_NOF_DFES)){
        PHYMOD_RETURN_WITH_ERR(PHYMOD_E_CONFIG, (_PHYMOD_MSG("illegal number of DFEs to set %u"), (unsigned int)rx->num_of_dfe_taps));
    }

    for (k = 0; k < num_lane; k++) {
        if (!PHYMOD_LANEPBMP_MEMBER(phy->access.lane_mask, start_lane + k)) {
            continue;
        }
        pm_phy_copy.access.lane_mask = 1 << (start_lane + k);

        /* Check if Micro is stopped on a lane */
        PHYMOD_IF_ERR_RETURN(falcon2_monterey_stop_uc_lane_status(&pm_phy_copy.access, &uc_lane_stopped));
        if (!uc_lane_stopped) {
            PHYMOD_IF_ERR_RETURN(falcon2_monterey_stop_rx_adaptation(&pm_phy_copy.access, 1));
        }
        /* vga set */
        PHYMOD_IF_ERR_RETURN(falcon2_monterey_write_rx_afe(&pm_phy_copy.access, RX_AFE_VGA, rx->vga.value));
        /* dfe set */
        for (i = 0 ; i < rx->num_of_dfe_taps ; i++){
            switch (i) {
                case 0:
                        PHYMOD_IF_ERR_RETURN(falcon2_monterey_write_rx_afe(&pm_phy_copy.access, RX_AFE_DFE1, rx->dfe[i].value));
                    break;
                case 1:
                        PHYMOD_IF_ERR_RETURN(falcon2_monterey_write_rx_afe(&pm_phy_copy.access, RX_AFE_DFE2, rx->dfe[i].value));
                    break;
                case 2:
                        PHYMOD_IF_ERR_RETURN(falcon2_monterey_write_rx_afe(&pm_phy_copy.access, RX_AFE_DFE3, rx->dfe[i].value));
                    break;
                case 3:
                        PHYMOD_IF_ERR_RETURN(falcon2_monterey_write_rx_afe(&pm_phy_copy.access, RX_AFE_DFE4, rx->dfe[i].value));
                    break;
                case 4:
                        PHYMOD_IF_ERR_RETURN(falcon2_monterey_write_rx_afe(&pm_phy_copy.access, RX_AFE_DFE5, rx->dfe[i].value));
                    break;
                case 5:
                        PHYMOD_IF_ERR_RETURN(falcon2_monterey_write_rx_afe(&pm_phy_copy.access, RX_AFE_DFE6, rx->dfe[i].value));
                    break;
                default:
                    return PHYMOD_E_PARAM;
            }
        }

        /*peaking filter set*/
        PHYMOD_IF_ERR_RETURN(falcon2_monterey_write_rx_afe(&pm_phy_copy.access, RX_AFE_PF, rx->peaking_filter.value));

        /* low freq peak filter */
        PHYMOD_IF_ERR_RETURN(falcon2_monterey_write_rx_afe(&pm_phy_copy.access, RX_AFE_PF2, (int8_t)rx->low_freq_peaking_filter.value));
    }

    return PHYMOD_E_NONE;
}

int tscf_gen3_phy_rx_get(const phymod_phy_access_t* phy, phymod_rx_t* rx)
{        
    int8_t tmpData;

    PHYMOD_IF_ERR_RETURN
        (falcon2_monterey_read_rx_afe(&phy->access, RX_AFE_VGA,  &tmpData));
    rx->vga.value = tmpData;
    PHYMOD_IF_ERR_RETURN
        (falcon2_monterey_read_rx_afe(&phy->access, RX_AFE_DFE1,  &tmpData));
    rx->dfe[0].value = tmpData;
    PHYMOD_IF_ERR_RETURN
        (falcon2_monterey_read_rx_afe(&phy->access, RX_AFE_DFE2,  &tmpData));
    rx->dfe[1].value = tmpData;
    PHYMOD_IF_ERR_RETURN
        (falcon2_monterey_read_rx_afe(&phy->access, RX_AFE_DFE3,  &tmpData));
    rx->dfe[2].value = tmpData;
    PHYMOD_IF_ERR_RETURN
        (falcon2_monterey_read_rx_afe(&phy->access, RX_AFE_DFE4,  &tmpData));
    rx->dfe[3].value = tmpData;
    PHYMOD_IF_ERR_RETURN
        (falcon2_monterey_read_rx_afe(&phy->access, RX_AFE_DFE5,  &tmpData));
    rx->dfe[4].value = tmpData;
    PHYMOD_IF_ERR_RETURN
        (falcon2_monterey_read_rx_afe(&phy->access, RX_AFE_DFE6,  &tmpData));
    rx->dfe[5].value = tmpData;
    PHYMOD_IF_ERR_RETURN
        (falcon2_monterey_read_rx_afe(&phy->access, RX_AFE_PF,  &tmpData));
    rx->peaking_filter.value = tmpData;
    PHYMOD_IF_ERR_RETURN
        (falcon2_monterey_read_rx_afe(&phy->access, RX_AFE_PF2,  &tmpData));
    rx->low_freq_peaking_filter.value = tmpData;

    rx->num_of_dfe_taps = 6;
    rx->dfe[0].enable = 1;
    rx->dfe[1].enable = 1;
    rx->dfe[2].enable = 1;
    rx->dfe[3].enable = 1;
    rx->dfe[4].enable = 1;
    rx->dfe[5].enable = 1;
    rx->vga.enable = 1;
    rx->low_freq_peaking_filter.enable = 1;
    rx->peaking_filter.enable = 1;

    return PHYMOD_E_NONE;
}


int tscf_gen3_phy_rx_adaptation_resume(const phymod_phy_access_t* phy)
{        
    uint8_t uc_lane_stopped;

    PHYMOD_IF_ERR_RETURN(falcon2_monterey_stop_uc_lane_status(&phy->access, &uc_lane_stopped));
    if (uc_lane_stopped) {
        PHYMOD_IF_ERR_RETURN(falcon2_monterey_stop_rx_adaptation(&phy->access, 0));
    }
        
    return PHYMOD_E_NONE;
}

int tscf_gen3_phy_reset_set(const phymod_phy_access_t* phy, const phymod_phy_reset_t* reset)
{        

    return PHYMOD_E_NONE;
    
}

int tscf_gen3_phy_reset_get(const phymod_phy_access_t* phy, phymod_phy_reset_t* reset)
{        
    
    return PHYMOD_E_NONE;
    
}


int tscf_gen3_phy_power_set(const phymod_phy_access_t* phy, const phymod_phy_power_t* power)
{        
    phymod_phy_access_t pm_phy_copy;
    int start_lane, num_lane, i;

    PHYMOD_MEMCPY(&pm_phy_copy, phy, sizeof(pm_phy_copy));
    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(&phy->access, &start_lane, &num_lane));
    if ((power->tx == phymodPowerOff) && (power->rx == phymodPowerOff)) {
        for (i = 0; i < num_lane; i++) {
            if (!PHYMOD_LANEPBMP_MEMBER(phy->access.lane_mask, start_lane + i)) {
                continue;
            }
            pm_phy_copy.access.lane_mask = 1 << (start_lane + i);
            PHYMOD_IF_ERR_RETURN(tefmod_gen3_port_enable_set(&pm_phy_copy.access, 0));
        }
    }
    if ((power->tx == phymodPowerOn) && (power->rx == phymodPowerOn)) {
        for (i = 0; i < num_lane; i++) {
            if (!PHYMOD_LANEPBMP_MEMBER(phy->access.lane_mask, start_lane + i)) {
                continue;
            }
            pm_phy_copy.access.lane_mask = 1 << (start_lane + i);
            PHYMOD_IF_ERR_RETURN(tefmod_gen3_port_enable_set(&pm_phy_copy.access, 1));
            PHYMOD_IF_ERR_RETURN(tefmod_gen3_power_control(&phy->access, 0, 0));
        }
    }
    if ((power->tx == phymodPowerOff) && (power->rx == phymodPowerNoChange)) {
        /* Disable tx on the PMD side */
        PHYMOD_IF_ERR_RETURN(falcon2_monterey_tx_disable(&phy->access, 1));
    }
    if ((power->tx == phymodPowerOn) && (power->rx == phymodPowerNoChange)) {
        /* Enable tx on the PMD side */
        PHYMOD_IF_ERR_RETURN(falcon2_monterey_tx_disable(&phy->access, 0));
    }
    if ((power->tx == phymodPowerNoChange) && (power->rx == phymodPowerOff)) {
        /* Force PMD signal detect value */
        PHYMOD_IF_ERR_RETURN(tefmod_gen3_rx_squelch_set(&phy->access, 1));
    }
    if ((power->tx == phymodPowerNoChange) && (power->rx == phymodPowerOn)) {
        /* Remove PMD signal detect value */
        PHYMOD_IF_ERR_RETURN(tefmod_gen3_rx_squelch_set(&phy->access, 0));
    }

    return PHYMOD_E_NONE;
}

int tscf_gen3_phy_power_get(const phymod_phy_access_t* phy, phymod_phy_power_t* power)
{        
    int enable;
    uint32_t lb_enable;
    phymod_phy_access_t pm_phy_copy;
    int start_lane, num_lane;

    PHYMOD_MEMCPY(&pm_phy_copy, phy, sizeof(pm_phy_copy));
    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(&phy->access, &start_lane, &num_lane));

    pm_phy_copy.access.lane_mask = 0x1 << start_lane;

    PHYMOD_IF_ERR_RETURN(tefmod_gen3_rx_squelch_get(&pm_phy_copy.access, &enable));

    /* Check if PMD loopback is on */
    if (enable) {
        PHYMOD_IF_ERR_RETURN(falcon2_monterey_pmd_loopback_get(&pm_phy_copy.access, &lb_enable));
        if (lb_enable) enable = 0;
    }

    power->rx = (enable == 1)? phymodPowerOff: phymodPowerOn;
    /* Commented the following line. Because if in PMD loopback mode, we squelch the
           xmit, and we should still see the correct port status */
    /* PHYMOD_IF_ERR_RETURN(tefmod_gen3_tx_squelch_get(&pm_phy_copy.access, &enable)); */
    power->tx = (enable == 1)? phymodPowerOff: phymodPowerOn;

 
    return PHYMOD_E_NONE;
    
}

int tscf_gen3_phy_hg2_codec_control_set(const phymod_phy_access_t* phy, phymod_phy_hg2_codec_t hg2_codec)
{        
    tefmod_gen3_hg2_codec_t local_copy;

    switch (hg2_codec) {
        case phymodBcmHG2CodecOff: 
            local_copy = TEFMOD_HG2_CODEC_OFF;
            break;
        case phymodBcmHG2CodecOnWith8ByteIPG:  
            local_copy = TEFMOD_HG2_CODEC_ON_8BYTE_IPG;
            break;
        case phymodBcmHG2CodecOnWith9ByteIPG:  
            local_copy = TEFMOD_HG2_CODEC_ON_9BYTE_IPG;
            break;
        default: 
            local_copy = TEFMOD_HG2_CODEC_OFF;
            break;
    }
    PHYMOD_IF_ERR_RETURN(tefmod_gen3_hg2_codec_set(&phy->access, local_copy));
        
    return PHYMOD_E_NONE;
}

int tscf_gen3_phy_hg2_codec_control_get(const phymod_phy_access_t* phy, phymod_phy_hg2_codec_t* hg2_codec)
{        
    tefmod_gen3_hg2_codec_t local_copy;

    PHYMOD_IF_ERR_RETURN(tefmod_gen3_hg2_codec_get(&phy->access, &local_copy));

    switch (local_copy) {
        case TEFMOD_HG2_CODEC_OFF: 
            *hg2_codec = phymodBcmHG2CodecOff;
            break;
        case TEFMOD_HG2_CODEC_ON_8BYTE_IPG:  
            *hg2_codec = phymodBcmHG2CodecOnWith8ByteIPG;
            break;
        case TEFMOD_HG2_CODEC_ON_9BYTE_IPG:  
            *hg2_codec = phymodBcmHG2CodecOnWith9ByteIPG;
            break;
        default: 
            *hg2_codec = phymodBcmHG2CodecOff;
            break;
    }

    return PHYMOD_E_NONE;
}

int tscf_gen3_phy_tx_lane_control_set(const phymod_phy_access_t* phy, phymod_phy_tx_lane_control_t tx_control)
{        
    phymod_firmware_lane_config_t fw_lane_config;

    PHYMOD_IF_ERR_RETURN (tscf_gen3_phy_firmware_lane_config_get(phy, &fw_lane_config));

    switch (tx_control) {
    case phymodTxTrafficDisable:
        PHYMOD_IF_ERR_RETURN(tefmod_gen3_tx_lane_control_set(&phy->access, TEFMOD_TX_LANE_TRAFFIC_DISABLE));
        break;
    case phymodTxTrafficEnable:
        PHYMOD_IF_ERR_RETURN(tefmod_gen3_tx_lane_control_set(&phy->access, TEFMOD_TX_LANE_TRAFFIC_ENABLE));
        break;
    case phymodTxReset:
        PHYMOD_IF_ERR_RETURN(tefmod_gen3_tx_lane_control_set(&phy->access, TEFMOD_TX_LANE_RESET));
        break;
    case phymodTxSquelchOn:
        PHYMOD_IF_ERR_RETURN(tefmod_gen3_tx_squelch_set(&phy->access, 1));
        break;
    case phymodTxSquelchOff:
        PHYMOD_IF_ERR_RETURN(tefmod_gen3_tx_squelch_set(&phy->access, 0));
        break;
    case phymodTxElectricalIdleEnable:
        if (fw_lane_config.LaneConfigFromPCS == 0) {
            PHYMOD_IF_ERR_RETURN(falcon2_monterey_electrical_idle_set(&phy->access, 1));
        }else{
            return PHYMOD_E_UNAVAIL; 
        }
        break;
    case phymodTxElectricalIdleDisable:
        if (fw_lane_config.LaneConfigFromPCS == 0) {
            PHYMOD_IF_ERR_RETURN(falcon2_monterey_electrical_idle_set(&phy->access, 0));
        }else{
            return PHYMOD_E_UNAVAIL;
        }
        break;
    default:
        break;
    }
        
    return PHYMOD_E_NONE;
}

int tscf_gen3_phy_tx_lane_control_get(const phymod_phy_access_t* phy, phymod_phy_tx_lane_control_t* tx_control)
{        
    int enable, reset, tx_lane;
    uint32_t lb_enable;
    phymod_phy_access_t pm_phy_copy;
    int start_lane, num_lane;

    PHYMOD_MEMCPY(&pm_phy_copy, phy, sizeof(pm_phy_copy));
    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(&phy->access, &start_lane, &num_lane));

    pm_phy_copy.access.lane_mask = 0x1 << start_lane;

    PHYMOD_IF_ERR_RETURN(tefmod_gen3_tx_squelch_get(&pm_phy_copy.access, &enable));

    /* Check if PMD loopback is on */
    if (enable) {
        PHYMOD_IF_ERR_RETURN(falcon2_monterey_pmd_loopback_get(&pm_phy_copy.access, &lb_enable));
        if (lb_enable) enable = 0;
    }
    if (enable) {
        *tx_control = phymodTxSquelchOn;
    } else {
        PHYMOD_IF_ERR_RETURN(tefmod_gen3_tx_lane_control_get(&pm_phy_copy.access, &reset, &tx_lane));
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


int tscf_gen3_phy_rx_lane_control_set(const phymod_phy_access_t* phy, phymod_phy_rx_lane_control_t rx_control)
{        
     phymod_phy_access_t pm_phy_copy;
    int start_lane, num_lane, i;

    PHYMOD_MEMCPY(&pm_phy_copy, phy, sizeof(pm_phy_copy));
    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(&phy->access, &start_lane, &num_lane));

    pm_phy_copy.access.lane_mask = 0x1 << start_lane;

    switch (rx_control) {
    case phymodRxReset:
        PHYMOD_IF_ERR_RETURN(tefmod_gen3_rx_lane_control_set(&phy->access, 1));
        break;
    case phymodRxSquelchOn:
        for (i = 0; i < num_lane; i++) {
            if (!PHYMOD_LANEPBMP_MEMBER(phy->access.lane_mask, start_lane + i)) {
                continue;
            }
            pm_phy_copy.access.lane_mask = 1 << (start_lane + i);
            PHYMOD_IF_ERR_RETURN(tefmod_gen3_rx_squelch_set(&pm_phy_copy.access, 1));
        }
        break;
    case phymodRxSquelchOff:
        for (i = 0; i < num_lane; i++) {
            if (!PHYMOD_LANEPBMP_MEMBER(phy->access.lane_mask, start_lane + i)) {
                continue;
            }
            pm_phy_copy.access.lane_mask = 1 << (start_lane + i);
            PHYMOD_IF_ERR_RETURN(tefmod_gen3_rx_squelch_set(&pm_phy_copy.access, 0));
        }
        break;
    default:
        break;
    }
 
    return PHYMOD_E_NONE;
}

int tscf_gen3_phy_rx_lane_control_get(const phymod_phy_access_t* phy, phymod_phy_rx_lane_control_t* rx_control)
{        
    int enable, reset;
    uint32_t lb_enable;
    phymod_phy_access_t pm_phy_copy;
    int start_lane, num_lane;

    PHYMOD_MEMCPY(&pm_phy_copy, phy, sizeof(pm_phy_copy));
    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(&phy->access, &start_lane, &num_lane));

    pm_phy_copy.access.lane_mask = 0x1 << start_lane;

    PHYMOD_IF_ERR_RETURN(tefmod_gen3_rx_squelch_get(&pm_phy_copy.access, &enable));
    /* Check if PMD loopback is on */
    if (enable) {
        PHYMOD_IF_ERR_RETURN(falcon2_monterey_pmd_loopback_get(&pm_phy_copy.access, &lb_enable));
        if (lb_enable) enable = 0;
    }
    if (enable) {
        *rx_control = phymodRxSquelchOn;
    } else {
        PHYMOD_IF_ERR_RETURN(tefmod_gen3_rx_lane_control_get(&pm_phy_copy.access, &reset));
        if (reset == 0) {
            *rx_control = phymodRxReset;
        } else {
            *rx_control = phymodRxSquelchOff;
        }
    }
        
    return PHYMOD_E_NONE;
}


int tscf_gen3_phy_fec_enable_set(const phymod_phy_access_t* phy, uint32_t enable)
{        
    int fec_en;
    tefmod_gen3_fec_type_t fec_type;

    /* first check FEC type */
    if (PHYMOD_FEC_CL91_GET(enable)) {
        fec_type = TEFMOD_CL91;
    } else if (PHYMOD_FEC_CL108_GET(enable)) {
        fec_type = TEFMOD_CL108;
    } else {
        fec_type = TEFMOD_CL74;
    }

    /* check FEC on/off */
    fec_en  = enable & TEFMOD_PHY_CONTROL_FEC_MASK;
    PHYMOD_IF_ERR_RETURN(
      tefmod_gen3_FEC_control(&phy->access, fec_type, fec_en, 0));

    return PHYMOD_E_NONE;
}

int tscf_gen3_phy_fec_enable_get(const phymod_phy_access_t* phy, uint32_t* enable)
{        
    int fec_en;
    tefmod_gen3_fec_type_t fec_type;

    /* first check FEC type */
    if (PHYMOD_FEC_CL91_GET(*enable)) {
        fec_type = TEFMOD_CL91;
    } else if (PHYMOD_FEC_CL108_GET(*enable)) {
        fec_type = TEFMOD_CL108;
    } else {
        fec_type = TEFMOD_CL74;
    }

    PHYMOD_IF_ERR_RETURN(
      tefmod_gen3_FEC_get(&phy->access, fec_type, &fec_en));
    PHYMOD_DEBUG_VERBOSE(("FEC enable state :: %x :: fec_type :: %x\n", fec_en, fec_type));
    *enable = (uint32_t) fec_en;
        
    return PHYMOD_E_NONE;
}

int tscf_gen3_phy_autoneg_oui_set(const phymod_phy_access_t* phy, phymod_autoneg_oui_t an_oui)
{        
    tefmod_gen3_an_oui_t oui;

    oui.oui                    = an_oui.oui;
    oui.oui_override_hpam_adv  = an_oui.oui_override_hpam_adv;
    oui.oui_override_hpam_det  = an_oui.oui_override_hpam_det;
    oui.oui_override_bam73_adv = an_oui.oui_override_bam73_adv;
    oui.oui_override_bam73_det = an_oui.oui_override_bam73_det;

    PHYMOD_IF_ERR_RETURN(tefmod_gen3_an_oui_set(&phy->access, oui));
        
    return PHYMOD_E_NONE;
}

int tscf_gen3_phy_autoneg_oui_get(const phymod_phy_access_t* phy, phymod_autoneg_oui_t* an_oui)
{        
    tefmod_gen3_an_oui_t oui;

    PHYMOD_IF_ERR_RETURN(tefmod_gen3_an_oui_get(&phy->access, &oui));
    an_oui->oui_override_hpam_adv  = oui.oui_override_hpam_adv;
    an_oui->oui_override_hpam_det  = oui.oui_override_hpam_det;
    an_oui->oui_override_bam73_adv = oui.oui_override_bam73_adv;
    an_oui->oui_override_bam73_det = oui.oui_override_bam73_det;

    return PHYMOD_E_NONE;
}

int tscf_gen3_phy_eee_set(const phymod_phy_access_t* phy, uint32_t enable)
{        
    uint32_t lpi_bypass;
    int rv = PHYMOD_E_NONE;
    lpi_bypass = PHYMOD_LPI_BYPASS_GET(enable);
    enable &= 0x1;
    if (lpi_bypass) {
        rv = tefmod_gen3_eee_control_set(&phy->access,enable);
    } else {
        return PHYMOD_E_UNAVAIL;
    }

    return rv;
}

int tscf_gen3_phy_eee_get(const phymod_phy_access_t* phy, uint32_t* enable)
{        
    if (PHYMOD_LPI_BYPASS_GET(*enable)) {
        PHYMOD_IF_ERR_RETURN(tefmod_gen3_eee_control_get(&phy->access, enable));
        PHYMOD_LPI_BYPASS_SET(*enable);
    } else {
        return PHYMOD_E_UNAVAIL;
    }

    return PHYMOD_E_NONE;
}

int tscf_gen3_phy_interface_config_set(const phymod_phy_access_t* phy, uint32_t flags, const phymod_phy_inf_config_t* config)
{
    int16_t new_os_mode = -1;
    int16_t os_mode;
    int start_lane, num_lane, lane;
    uint32_t vco_rate;
    uint32_t new_pll_div;
    uint32_t cur_pll0_div = 0;
    uint32_t cur_pll1_div = 0;
    uint8_t cur_pll_select = 0;
    int new_pll_set = 0;
    int pll_index_update = 0;
    int pll_index = 0;
    phymod_phy_access_t pm_phy_copy;
    tefmod_gen3_spd_intfc_type_t spd_intf = TEFMOD_SPD_ILLEGAL;
    phymod_firmware_lane_config_t firmware_lane_config;
    phymod_firmware_core_config_t firmware_core_config;

    PHYMOD_MEMCPY(&pm_phy_copy, phy, sizeof(pm_phy_copy));
    PHYMOD_MEMSET(&firmware_lane_config, 0x0, sizeof(firmware_lane_config));
    PHYMOD_MEMSET(&firmware_core_config, 0x0, sizeof(firmware_core_config));
    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(&phy->access, &start_lane, &num_lane));

    /* Hold PCS lane reset */
    tefmod_gen3_disable_set(&phy->access);

    /* Assert the ln dp reset */
    for (lane = 0; lane < num_lane; lane++) {
        if(!PHYMOD_LANEPBMP_MEMBER(phy->access.lane_mask, start_lane+lane)) {
            continue;
        }
        pm_phy_copy.access.lane_mask = (1 << (start_lane + lane));
        PHYMOD_IF_ERR_RETURN
            (falcon2_monterey_lane_soft_reset_release(&pm_phy_copy.access, 0));
    }

    /* Remove pmd_tx_disable_pin_dis, it may be asserted because of ILKN*/
    if (config->interface_type != phymodInterfaceBypass) {
        for (lane = 0; lane < num_lane; lane++) {
            if(!PHYMOD_LANEPBMP_MEMBER(phy->access.lane_mask, start_lane+lane)) {
                continue;
            }
            pm_phy_copy.access.lane_mask = (1 << (start_lane + lane));
            PHYMOD_IF_ERR_RETURN
                (falcon2_monterey_pmd_tx_disable_pin_dis_set(&pm_phy_copy.access, 0));
        }
    }

    pm_phy_copy.access.lane_mask = 0x1;
    PHYMOD_IF_ERR_RETURN
        (tscf_gen3_phy_firmware_core_config_get(&pm_phy_copy, &firmware_core_config));

    pm_phy_copy.access.lane_mask = 0x1 << start_lane;
    PHYMOD_IF_ERR_RETURN
        (tscf_gen3_phy_firmware_lane_config_get(&pm_phy_copy, &firmware_lane_config));

    /* AutoNeg and core config from pcs are off */
    firmware_core_config.CoreConfigFromPCS = 0;
    firmware_lane_config.AnEnabled = 0;
    firmware_lane_config.LaneConfigFromPCS = 0;
    firmware_lane_config.DfeOn = 1;
    firmware_lane_config.LpDfeOn = 0;
    firmware_lane_config.Cl72RestTO = 1;
    firmware_lane_config.Cl72AutoPolEn = 0;
    firmware_lane_config.ScramblingDisable = 0;

    if (PHYMOD_INTF_MODES_FIBER_GET(config)) {
        firmware_lane_config.MediaType = phymodFirmwareMediaTypeOptics;
    } else if (PHYMOD_INTF_MODES_COPPER_GET(config)) {
        firmware_lane_config.MediaType = phymodFirmwareMediaTypeCopperCable;
    } else {
        firmware_lane_config.MediaType = phymodFirmwareMediaTypePcbTraceBackPlane;
    }

    if ((config->data_rate == 1000)||(config->data_rate == 2500)) {
       firmware_lane_config.DfeOn = 0;
    }

    /* Select port configuration mode */
    PHYMOD_IF_ERR_RETURN
        (tefmod_gen3_port_mode_select(&phy->access)); 

    /* Lookup speed ID table */
    if (config->interface_type == phymodInterface1000X) {
        if (config->data_rate == 2500){
            spd_intf = TEFMOD_SPD_2P5G_KX1;
        } else {
            if (config->pll_divider_req == 165) {
                spd_intf = TEFMOD_SPD_1G_25G;
            } else {
                spd_intf = TEFMOD_SPD_1G_20G;
            }
        }
    } else if (config->interface_type == phymodInterfaceSGMII) {
        if (config->data_rate == 2500){
            spd_intf = TEFMOD_SPD_2P5G_KX1;
        } else {
            if (config->pll_divider_req == 165) {
                spd_intf = TEFMOD_SPD_1G_25G;
            } else {
                spd_intf = TEFMOD_SPD_1G_20G;
            }
        }
    } else if ((config->interface_type == phymodInterfaceBypass) ||
                 (num_lane == 1)) {
        switch (config->data_rate) {
            case 5000:
                spd_intf = TEFMOD_SPD_5G_KR1;
                firmware_lane_config.DfeOn = 0;
                break;
            case 10000:
            case 12000:
                spd_intf = TEFMOD_SPD_10000_XFI;
                if (config->interface_type != phymodInterfaceBypass) {
                    if (!PHYMOD_INTF_MODES_COPPER_GET(config)) {
                        firmware_lane_config.DfeOn = 0;
                    }
                }
                break;
            case 11000:
                spd_intf = TEFMOD_SPD_10600_XFI_HG;
                if (config->interface_type != phymodInterfaceBypass) {
                    if (!PHYMOD_INTF_MODES_COPPER_GET(config)) {
                        firmware_lane_config.DfeOn = 0;
                    }
                }
                break;
            case 20000:
                spd_intf = TEFMOD_SPD_20000_XFI;
                break;
            case 21000:
            case 21200:
                spd_intf = TEFMOD_SPD_21200_XFI_HG;
                break;
            case 25000:
                spd_intf = TEFMOD_SPD_25000_XFI;
                break;
            case 27000:
                spd_intf = TEFMOD_SPD_26500_XFI_HG;
                firmware_lane_config.DfeOn = 1;
                firmware_lane_config.LpDfeOn = 0;
                if (PHYMOD_INTF_MODES_COPPER_GET(config)) {
                    firmware_lane_config.MediaType = phymodFirmwareMediaTypeCopperCable;
                } else {
                    firmware_lane_config.MediaType = phymodFirmwareMediaTypePcbTraceBackPlane;
                }
                break;
            default:
            /* If the interface is pcs bypass mode and check the speed */
            if (config->interface_type == phymodInterfaceBypass) {
                /* Disable DFE for lower speed */
                if (config->data_rate < 10000) {
                    firmware_lane_config.DfeOn = 0;
                }
            } else {
                spd_intf = TEFMOD_SPD_10000_XFI;
                firmware_lane_config.DfeOn = 0;
            }
            break;
        }
    } else if ((config->interface_type == phymodInterfaceKR2) ||
               (config->interface_type == phymodInterfaceCR2) ||
               (config->interface_type == phymodInterfaceXLAUI2) ||
               (config->interface_type == phymodInterfaceRXAUI) ||
               (config->interface_type == phymodInterfaceX2) ||
               (num_lane == 2)) {
        switch (config->data_rate) {
            case 20000:
                spd_intf = TEFMOD_SPD_20G_MLD_X2;
                if (PHYMOD_INTF_MODES_HIGIG_GET(config)) {
                   firmware_lane_config.DfeOn = 0;
                   firmware_lane_config.LpDfeOn = 0;
                }
                if (PHYMOD_INTF_MODES_FIBER_GET(config) && !(PHYMOD_INTF_MODES_HIGIG_GET(config))) {
                    firmware_lane_config.MediaType = phymodFirmwareMediaTypeOptics;
                } else if (PHYMOD_INTF_MODES_COPPER_GET(config)) {
                    firmware_lane_config.MediaType = phymodFirmwareMediaTypeCopperCable;
                } else {
                    firmware_lane_config.MediaType = phymodFirmwareMediaTypePcbTraceBackPlane;
                }
                break;
            case 21000:
            case 21200:
                firmware_lane_config.DfeOn = 0;
                firmware_lane_config.LpDfeOn = 0;
                spd_intf = TEFMOD_SPD_21G_MLD_HG_X2;
                if (PHYMOD_INTF_MODES_COPPER_GET(config)) {
                    firmware_lane_config.MediaType = phymodFirmwareMediaTypeCopperCable;
                } else {
                    firmware_lane_config.MediaType = phymodFirmwareMediaTypePcbTraceBackPlane;
                }
                break;
            case 40000:
                spd_intf = TEFMOD_SPD_40G_MLD_X2;
                if (config->interface_type == phymodInterfaceXLAUI2) {
                    firmware_lane_config.DfeOn = 0;
                    firmware_lane_config.LpDfeOn = 0;
                } else {
                    firmware_lane_config.DfeOn = 1;
                    firmware_lane_config.LpDfeOn = 1;
                }
                if (PHYMOD_INTF_MODES_FIBER_GET(config)) {
                    firmware_lane_config.MediaType = phymodFirmwareMediaTypeOptics;
                } else if (PHYMOD_INTF_MODES_COPPER_GET(config)) {
                    firmware_lane_config.MediaType = phymodFirmwareMediaTypeCopperCable;
                } else {
                    firmware_lane_config.MediaType = phymodFirmwareMediaTypePcbTraceBackPlane;
                }
                break;
            case 42000:
                spd_intf = TEFMOD_SPD_42G_MLD_HG_X2;
                firmware_lane_config.DfeOn = 1;
                firmware_lane_config.LpDfeOn = 1;
                if (PHYMOD_INTF_MODES_COPPER_GET(config)) {
                    firmware_lane_config.MediaType = phymodFirmwareMediaTypeCopperCable;
                } else {
                    firmware_lane_config.MediaType = phymodFirmwareMediaTypePcbTraceBackPlane;
                }
                break;
            case 50000:
                spd_intf = TEFMOD_SPD_50G_MLD_X2;
                break;
            case 53000:
                spd_intf = TEFMOD_SPD_53G_MLD_HG_X2;
                break;
            default:
                spd_intf = TEFMOD_SPD_20G_MLD_X2;
                break;
        }
    } else if (num_lane == 4) {
        switch (config->data_rate) {
            case 10000:
                spd_intf = TEFMOD_SPD_2P5G_KX1;
                firmware_lane_config.DfeOn = 0;
                firmware_lane_config.LpDfeOn = 0;
                firmware_lane_config.ScramblingDisable = 1;
                break;
            case 40000:
                if (PHYMOD_INTF_MODES_HIGIG_GET(config)) {
                    spd_intf = TEFMOD_SPD_42G_MLD_HG_X4;
                    firmware_lane_config.DfeOn = 1;
                    firmware_lane_config.LpDfeOn = 1;
                } else if (PHYMOD_INTF_MODES_FIBER_GET(config)) {
                    spd_intf = TEFMOD_SPD_40G_MLD_X4;
                    firmware_lane_config.DfeOn = 0;
                    firmware_lane_config.LpDfeOn = 0;
                } else {
                    if (config->interface_type == phymodInterfaceXLAUI) {
                        spd_intf = TEFMOD_SPD_40G_MLD_X4;
                        firmware_lane_config.DfeOn = 0;
                        firmware_lane_config.LpDfeOn = 0;
                    } else {
                        spd_intf = TEFMOD_SPD_40G_MLD_X4;
                        firmware_lane_config.DfeOn = 1;
                        firmware_lane_config.LpDfeOn = 1;
                    }
                }
                break;
            case 42000:
                spd_intf = TEFMOD_SPD_42G_MLD_HG_X4;
                firmware_lane_config.DfeOn = 1;
                firmware_lane_config.LpDfeOn = 1;
                break;
            case 48000:
                spd_intf = TEFMOD_SPD_40G_MLD_X4;
                firmware_lane_config.DfeOn = 0;
                firmware_lane_config.LpDfeOn = 0;
                break;
            case 50000:
                spd_intf = TEFMOD_SPD_50G_MLD_X4;
                break;
            case 53000:
                spd_intf = TEFMOD_SPD_53G_MLD_HG_X4;
                break;
            case 100000:
                if (PHYMOD_INTF_MODES_HIGIG_GET(config)) {
                    spd_intf = TEFMOD_SPD_106G_MLD_HG_X4;
                    firmware_lane_config.DfeOn = 1;
                    firmware_lane_config.LpDfeOn = 0;
                    if (PHYMOD_INTF_MODES_COPPER_GET(config)) {
                        firmware_lane_config.MediaType = phymodFirmwareMediaTypeCopperCable;
                    } else {
                        firmware_lane_config.MediaType = phymodFirmwareMediaTypePcbTraceBackPlane;
                    }
                } else {
                    spd_intf = TEFMOD_SPD_100G_MLD_X4;
                    if (PHYMOD_INTF_MODES_FIBER_GET(config)) {
                        firmware_lane_config.DfeOn = 0;
                        firmware_lane_config.LpDfeOn = 0;
                        firmware_lane_config.MediaType = phymodFirmwareMediaTypeOptics;
                    } else if (PHYMOD_INTF_MODES_COPPER_GET(config)) {
                        firmware_lane_config.DfeOn = 1;
                        firmware_lane_config.LpDfeOn = 0;
                        firmware_lane_config.MediaType = phymodFirmwareMediaTypeCopperCable;
                    } else {
                        firmware_lane_config.DfeOn = 1;
                        firmware_lane_config.LpDfeOn = 0;
                        firmware_lane_config.MediaType = phymodFirmwareMediaTypePcbTraceBackPlane;
                    }
                    if (config->interface_type == phymodInterfaceCAUI4 || config->interface_type == phymodInterfaceCAUI) {
                        firmware_lane_config.DfeOn = 1;
                        firmware_lane_config.LpDfeOn = 1;
                        firmware_lane_config.MediaType = phymodFirmwareMediaTypePcbTraceBackPlane;
                    }
                }
                break;
            case 106000:
                spd_intf = TEFMOD_SPD_106G_MLD_HG_X4;
                if (PHYMOD_INTF_MODES_COPPER_GET(config)) {
                    firmware_lane_config.DfeOn = 1;
                    firmware_lane_config.LpDfeOn = 0;
                    firmware_lane_config.MediaType = phymodFirmwareMediaTypeCopperCable;
                } else {
                    firmware_lane_config.DfeOn = 1;
                    firmware_lane_config.LpDfeOn = 0;
                    firmware_lane_config.MediaType = phymodFirmwareMediaTypePcbTraceBackPlane;
                }
                break;
            default:
                spd_intf = TEFMOD_SPD_40G_MLD_X4;
                firmware_lane_config.DfeOn = 0;
                break;
        }
    } else {
        PHYMOD_DEBUG_ERROR(("Incorrect speed config\n" ));
        return PHYMOD_E_CONFIG;
    }

    /* Get current PLL0 and PLL1 */
    PHYMOD_IF_ERR_RETURN
        (tefmod_gen3_pll_div_get(&pm_phy_copy.access, &cur_pll0_div, &cur_pll1_div));

    /* Get PLL_select */
    PHYMOD_IF_ERR_RETURN
        (falcon2_monterey_pll_select_get(&pm_phy_copy.access, &cur_pll_select));

    /* Get required PLL_DIV */ 
    if (config->interface_type == phymodInterfaceBypass){
        PHYMOD_IF_ERR_RETURN
            (falcon2_monterey_get_vco(config, &vco_rate, &new_pll_div, &new_os_mode));
    } else {
        PHYMOD_IF_ERR_RETURN
            (tefmod_gen3_pll_div_lkup_get(&phy->access, spd_intf, config->ref_clock, &new_pll_div));
    }

    /* If flag SINGLE_PLL_ONLY set
     * One PLL:   PLL1_DIV = 132(20G)/165(25G) 
     * Otherwise,
     * Dual PLL:
     *  - IEEE mode:  PLL0_DIV = 132(20G), PLL1_DIV = 165(25G)
     *  - HIGIG mode: PLL0_DIV = 140(21G)/ 175(27G), PLL1_DIV = 165(25G)
     */
    /* Only PLL1 is used for ETH port of mixed mode */
    if (flags == PHYMOD_INTF_F_SINGLE_PLL_ONLY) {
        if ((new_pll_div == phymod_TSCF_PLL_DIV132) || (new_pll_div == phymod_TSCF_PLL_DIV165)){
            if (new_pll_div != cur_pll1_div) {
                if (!(flags & PHYMOD_INTF_F_DONT_TURN_OFF_PLL)){
                    new_pll_set = 1;
                } else {
                     PHYMOD_DEBUG_ERROR(("DONT TURN OFF PLL flag is set\n" ));
                     return PHYMOD_E_CONFIG;
                }
            }
            pll_index_update = 1;
            pll_index = 1;
        } else {
            PHYMOD_DEBUG_ERROR(("Pll %d One PLL mode doesn't support \n", new_pll_div));
            return PHYMOD_E_CONFIG;
        }
    } else { /* HIGIG MODE */
        if (PHYMOD_INTF_MODES_HIGIG_GET(config)){
            /* For 10G, 20G, 40G(4 lanes) Higig2, pll_div is phymod_TSCF_PLL_DIV132 */
            if ((config->data_rate == 10000) && (num_lane == 1)) {
                new_pll_set = 0;
                pll_index_update = 1;
                pll_index = 0;
            } else if ((config->data_rate == 20000) && (num_lane == 2)) {
                new_pll_set = 0;
                pll_index_update = 1;
                pll_index = 0;
            } else if ((config->data_rate == 40000) && (num_lane == 4)) {
                new_pll_set = 0;
                pll_index_update = 1;
                pll_index = 0;
                new_pll_div = phymod_TSCF_PLL_DIV132;
            }
            if ((pll_index_update == 1) && (new_pll_div != cur_pll0_div)) {
                new_pll_set = 1;
            }
            if ((new_pll_div == phymod_TSCF_PLL_DIV140) || (new_pll_div == phymod_TSCF_PLL_DIV175)) {
                if (new_pll_div != cur_pll0_div){
                    if (!(flags & PHYMOD_INTF_F_DONT_TURN_OFF_PLL)){
                        new_pll_set = 1;
                        pll_index_update = 1;
                        pll_index = 0;
                    } else {
                        PHYMOD_DEBUG_ERROR(("DONT TURN OFF PLL flag is set\n" ));
                        return PHYMOD_E_CONFIG;
                    }
                }
                pll_index_update = 1;
                pll_index = 0;
            }
        } else {  /* IEEE MODE */
            if ((new_pll_div == phymod_TSCF_PLL_DIV132) || (new_pll_div == phymod_TSCF_PLL_DIV165)){
                if ((new_pll_div == cur_pll0_div) && (cur_pll_select != 0)){
                    pll_index_update = 1;
                    pll_index = 0;
                }
                if ((new_pll_div == cur_pll1_div) && (cur_pll_select != 1)){
                     pll_index_update = 1;
                     pll_index = 1;
                }
                /* Set PLL select as 1 when speed 1G with VCO 25.78125 */
                if ((config->data_rate == 1000) && (config->pll_divider_req == 165)) {
                    pll_index_update = 1;
                    pll_index = 1;
                }
            }
        }
    }


    /* Set os mode */
    if (new_os_mode >=0){
        os_mode = new_os_mode | 0x80000000;
    } else {
        os_mode = 0;
    }
    PHYMOD_IF_ERR_RETURN
        (tefmod_gen3_pmd_os_mode_set(&phy->access, spd_intf, config->ref_clock, os_mode));   

    if (new_pll_set) {
        /* Only PLL1 is used for ETH port of mixed mode */
        if (flags != PHYMOD_INTF_F_SINGLE_PLL_ONLY) {
            pm_phy_copy.access.pll_idx = 0;
            PHYMOD_IF_ERR_RETURN
                (falcon2_monterey_core_soft_reset_release(&pm_phy_copy.access, 0));
        }

        /* Update PLL1 for ETH port of mixed mode, otherwise leave PLL1 at 25G */
        if (flags == PHYMOD_INTF_F_SINGLE_PLL_ONLY) {
            pm_phy_copy.access.pll_idx = 1;
            PHYMOD_IF_ERR_RETURN
                (falcon2_monterey_core_soft_reset_release(&pm_phy_copy.access, 0));
        }
        /* PLL0 config */
        if (flags != PHYMOD_INTF_F_SINGLE_PLL_ONLY) {
            pm_phy_copy.access.pll_idx = 0;
            PHYMOD_IF_ERR_RETURN
                (tefmod_gen3_configure_pll(&pm_phy_copy.access, new_pll_div, config->ref_clock));
            PHYMOD_IF_ERR_RETURN
                (tefmod_gen3_pll_to_vco_rate(&pm_phy_copy.access, new_pll_div, config->ref_clock, &vco_rate));
            firmware_core_config.VcoRate = MHZ_TO_VCO_RATE(vco_rate);

            /* Update firmware core config */
            PHYMOD_IF_ERR_RETURN
                (tscf_gen3_phy_firmware_core_config_set(&pm_phy_copy, firmware_core_config));
            PHYMOD_IF_ERR_RETURN
                (falcon2_monterey_core_soft_reset_release(&pm_phy_copy.access, 1));
        }

        if (flags == PHYMOD_INTF_F_SINGLE_PLL_ONLY) {
            pm_phy_copy.access.pll_idx = 1;
            PHYMOD_IF_ERR_RETURN
                (tefmod_gen3_configure_pll(&pm_phy_copy.access, new_pll_div, config->ref_clock));
            PHYMOD_IF_ERR_RETURN
                (tefmod_gen3_pll_to_vco_rate(&pm_phy_copy.access, new_pll_div, config->ref_clock, &vco_rate));
            firmware_core_config.VcoRate = MHZ_TO_VCO_RATE(vco_rate);

            /* Update firmware core config */
            PHYMOD_IF_ERR_RETURN
                (tscf_gen3_phy_firmware_core_config_set(&pm_phy_copy, firmware_core_config));
            PHYMOD_IF_ERR_RETURN
                (falcon2_monterey_core_soft_reset_release(&pm_phy_copy.access, 1));
        }

        PHYMOD_IF_ERR_RETURN
            (tefmod_gen3_master_port_num_set(&phy->access, start_lane));
        PHYMOD_IF_ERR_RETURN
            (tefmod_gen3_pll_reset_enable_set(&phy->access, 1));
    }

     /* Select pll index for the port */
    if (pll_index_update) {
        for (lane = 0; lane < num_lane; lane++) {
            if (!PHYMOD_LANEPBMP_MEMBER(phy->access.lane_mask, start_lane + lane)) {
                continue;
            }
            pm_phy_copy.access.lane_mask = 0x1 << (start_lane + lane);
            PHYMOD_IF_ERR_RETURN
                (tefmod_gen3_pll_select_set(&pm_phy_copy.access, pll_index));
        }
    }
    if(config->interface_type != phymodInterfaceBypass) {
        PHYMOD_IF_ERR_RETURN
          (tefmod_gen3_set_spd_intf(&phy->access, spd_intf));
    }

    /* Update firmware lane config */
    for (lane = 0; lane < num_lane; lane++) {
        if (!PHYMOD_LANEPBMP_MEMBER(phy->access.lane_mask, start_lane + lane)) {
            continue;
        }
        pm_phy_copy.access.lane_mask = 0x1 << (start_lane + lane);
        PHYMOD_IF_ERR_RETURN
             (_tscf_gen3_phy_firmware_lane_config_set(&pm_phy_copy, firmware_lane_config));
    }

    /* Release the ln dp reset */
    for (lane = 0; lane < num_lane; lane++) {
        if (!PHYMOD_LANEPBMP_MEMBER(phy->access.lane_mask, start_lane + lane)) {
            continue;
        }
        pm_phy_copy.access.lane_mask = 1 << (start_lane + lane);
        PHYMOD_IF_ERR_RETURN
            (falcon2_monterey_lane_soft_reset_release(&pm_phy_copy.access, 1));
    }

    return PHYMOD_E_NONE;
}

STATIC
int _tscf_gen3_speed_id_interface_config_get(const phymod_phy_access_t* phy, int speed_id, phymod_phy_inf_config_t* config)
{
    int ilkn_set = 0;
    uint32_t pll_div, pll0_div, pll1_div; 
    uint8_t pll_select;
    int osr_mode;
    int div_osr_value;
    uint32_t vco_rate;

    PHYMOD_IF_ERR_RETURN(tefmod_gen3_pcs_ilkn_chk(&phy->access, &ilkn_set));
    PHYMOD_IF_ERR_RETURN
        (tefmod_gen3_pll_div_get(&phy->access, &pll0_div, &pll1_div)); 
    PHYMOD_IF_ERR_RETURN
        (falcon2_monterey_pll_select_get(&phy->access, &pll_select));
    pll_div = (pll_select == 1)? pll1_div : pll0_div;

    if(ilkn_set) {
        config->interface_type = phymodInterfaceBypass;
        PHYMOD_IF_ERR_RETURN
            (falcon2_monterey_osr_mode_get(&phy->access, &osr_mode));
        PHYMOD_IF_ERR_RETURN
            (tefmod_gen3_pll_to_vco_rate(&phy->access, pll_div, config->ref_clock, &vco_rate));    

        div_osr_value = 1 << osr_mode;
        config->data_rate = vco_rate/div_osr_value;
    } else {
        switch (speed_id) {
        case 0x0:
            config->data_rate = 10000;
            config->interface_type = phymodInterfaceCR;
            break;
        case 0x1:
            config->data_rate = 10000;
            config->interface_type = phymodInterfaceKR;
            break;
        case 0x2:
            config->data_rate = 10000;
            config->interface_type = phymodInterfaceKR;
            break;
        case 0x4:
            config->data_rate = 11000;
            config->interface_type = phymodInterfaceCR;
            break;
        case 0x5:
            config->data_rate = 11000;
            config->interface_type = phymodInterfaceKR;
            break;
        case 0x6:
            config->data_rate = 11000;
            config->interface_type = phymodInterfaceKR;
            break;
        case 0x8:
            config->data_rate = 20000;
            config->interface_type = phymodInterfaceCR;
            break;
        case 0x9:
            config->data_rate = 20000;
            config->interface_type = phymodInterfaceKR;
            break;
        case 0xa:
            config->data_rate = 20000;
            config->interface_type = phymodInterfaceKR;
            break;
        case 0xc:
            config->data_rate = 21000;
            config->interface_type = phymodInterfaceCR;
            break;
        case 0xd:
            config->data_rate = 21000;
            config->interface_type = phymodInterfaceKR;
            break;
        case 0xe:
            config->data_rate = 21000;
            config->interface_type = phymodInterfaceKR;
            break;
        case 0x10:
            config->data_rate = 25000;
            config->interface_type = phymodInterfaceCR;
            break;
        case 0x11:
            config->data_rate = 25000;
            config->interface_type = phymodInterfaceKR;
            break;
        case 0x12:
            config->data_rate = 25000;
            config->interface_type = phymodInterfaceKR;
            break;
        case 0x15:
            config->data_rate = 27000;
            config->interface_type = phymodInterfaceKR;
            break;
        case 0x16:
            config->data_rate = 27000;
            config->interface_type = phymodInterfaceKR;
            break;
        case 0x18:
            config->data_rate = 20000;
            config->interface_type = phymodInterfaceCR2;
            break;
        case 0x19:
            config->data_rate = 20000;
            config->interface_type = phymodInterfaceKR2;
            break;
        case 0x1a:
            config->data_rate = 20000;
            config->interface_type = phymodInterfaceKR2;
            break;
        case 0x1c:
            config->data_rate = 21000;
            config->interface_type = phymodInterfaceCR2;
            break;
        case 0x1d:
            config->data_rate = 21000;
            config->interface_type = phymodInterfaceKR2;
            break;
        case 0x1e:
            config->data_rate = 21000;
            config->interface_type = phymodInterfaceKR2;
            break;
        case 0x20:
            config->data_rate = 40000;
            config->interface_type = phymodInterfaceCR2;
            break;
        case 0x21:
            config->data_rate = 40000;
            config->interface_type = phymodInterfaceKR2;
            break;
        case 0x22:
            config->data_rate = 40000;
            config->interface_type = phymodInterfaceKR2;
            break;
        case 0x24:
            config->data_rate = 42000;
            config->interface_type = phymodInterfaceCR2;
            break;
        case 0x25:
            config->data_rate = 42000;
            config->interface_type = phymodInterfaceKR2;
            break;
        case 0x26:
            config->data_rate = 42000;
            config->interface_type = phymodInterfaceKR2;
            break;
        case 0x28:
            config->data_rate = 40000;
            if ((config->ref_clock == phymodRefClk156Mhz) &&
                (pll_div == phymod_TSCF_PLL_DIV140)) {
                config->data_rate =42000;
            }
            config->interface_type = phymodInterfaceCR4;
            break;
        case 0x29:
            config->data_rate = 40000;
            if ((config->ref_clock == phymodRefClk156Mhz) &&
                (pll_div == phymod_TSCF_PLL_DIV140)) {
                config->data_rate =42000;
            }
            config->interface_type = phymodInterfaceKR4;
            break;
        case 0x2a:
            config->data_rate = 40000;
            if ((config->ref_clock == phymodRefClk156Mhz) &&
                (pll_div == phymod_TSCF_PLL_DIV140)) {
                config->data_rate =42000;
            }
            config->interface_type = phymodInterfaceKR4;
            break;
        case 0x2c:
            config->data_rate = 42000;
            config->interface_type = phymodInterfaceCR4;
            break;
        case 0x2d:
            config->data_rate = 42000;
            config->interface_type = phymodInterfaceKR4;
            break;
        case 0x2e:
            config->data_rate = 42000;
            if ((config->ref_clock == phymodRefClk156Mhz) &&
                (pll_div == phymod_TSCF_PLL_DIV132)) {
                config->data_rate =40000;
            }
            if ((config->ref_clock == phymodRefClk125Mhz) &&
                (pll_div == phymod_TSCF_PLL_DIV165)) {
                config->data_rate =40000;
            }
            config->interface_type = phymodInterfaceKR4;
            break;
        case 0x30:
            config->data_rate = 50000;
            config->interface_type = phymodInterfaceCR2;
            break;
        case 0x31:
            config->data_rate = 50000;
            config->interface_type = phymodInterfaceKR2;
            break;
        case 0x32:
            config->data_rate = 50000;
            config->interface_type = phymodInterfaceKR2;
            break;
        case 0x34:
            config->data_rate = 53000;
            config->interface_type = phymodInterfaceCR2;
            break;
        case 0x35:
            config->data_rate = 53000;
            config->interface_type = phymodInterfaceKR2;
            break;
        case 0x36:
            config->data_rate = 53000;
            config->interface_type = phymodInterfaceKR2;
            break;
        case 0x38:
            config->data_rate = 50000;
            config->interface_type = phymodInterfaceCR4;
            break;
        case 0x39:
            config->data_rate = 50000;
            config->interface_type = phymodInterfaceKR4;
            break;
        case 0x3a:
            config->data_rate = 50000;
            config->interface_type = phymodInterfaceKR4;
            break;
        case 0x3c:
            config->data_rate = 53000;
            config->interface_type = phymodInterfaceCR4;
            break;
        case 0x3d:
            config->data_rate = 53000;
            config->interface_type = phymodInterfaceKR4;
            break;
        case 0x3e:
            config->data_rate = 53000;
            config->interface_type = phymodInterfaceKR4;
            break;
        case 0x40:
            config->data_rate = 100000;
            config->interface_type = phymodInterfaceCR4;
            break;
        case 0x41:
            config->data_rate = 100000;
            config->interface_type = phymodInterfaceKR4;
            break;
        case 0x42:
            config->data_rate = 100000;
            config->interface_type = phymodInterfaceKR4;
            break;
        case 0x44:
            config->data_rate = 106000;
            if (pll_div == phymod_TSCF_PLL_DIV165) {
                config->data_rate = 100000;
            }
            config->interface_type = phymodInterfaceCR4;
            break;
        case 0x45:
            config->data_rate = 106000;
            if (pll_div == phymod_TSCF_PLL_DIV165) {
                config->data_rate = 100000;
            }
            config->interface_type = phymodInterfaceKR4;
            break;
        case 0x46:
            config->data_rate = 106000;
            if (pll_div == phymod_TSCF_PLL_DIV165) {
                config->data_rate = 100000;
            }
            config->interface_type = phymodInterfaceKR4;
            break;
        case 0x48:
            config->data_rate = 20000;
            config->interface_type = phymodInterfaceKR2;
            break;
        case 0x50:
            config->data_rate = 25000;
            config->interface_type = phymodInterfaceKR2;
            break;
        case 0x58:
            config->data_rate = 1000;
            config->interface_type = phymodInterfaceSGMII;
            break;
        case 0x60:
            config->data_rate = 1000;
            config->interface_type = phymodInterfaceSGMII;
            break;
        case 0x62:
            config->data_rate = 2500;
            config->interface_type = phymodInterfaceSGMII;
            break;
        case 0x63:
            config->data_rate = 5000;
            config->interface_type = phymodInterfaceKR;
            break;
        case 0x70:
            config->data_rate = 25000;
            config->interface_type = phymodInterfaceCR;
            break;
        case 0x71:
            config->data_rate = 25000;
            config->interface_type = phymodInterfaceCR;
            break;
        case 0x72:
            config->data_rate = 25000;
            config->interface_type = phymodInterfaceKR;
            break;
        case 0x73:
            config->data_rate = 25000;
            config->interface_type = phymodInterfaceKR;
            break;
        default:
            config->data_rate = 10000;
            config->interface_type = phymodInterfaceKR;
            break;
        }
    }

    return PHYMOD_E_NONE;
}

int tscf_gen3_phy_interface_config_get(const phymod_phy_access_t* phy, uint32_t flags, phymod_ref_clk_t ref_clock, phymod_phy_inf_config_t* config)
{        
    int speed_id;
    phymod_firmware_lane_config_t firmware_lane_config;
    phymod_phy_access_t pm_phy_copy;
    int start_lane, num_lane;
    phymod_autoneg_control_t an_control;
    int an_done = 0;
    an_control.flags = 0;

    config->ref_clock = ref_clock;
    PHYMOD_MEMCPY(&pm_phy_copy, phy, sizeof(pm_phy_copy));

    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(&phy->access, &start_lane, &num_lane));
    PHYMOD_IF_ERR_RETURN
        (tefmod_gen3_speed_id_get(&phy->access, &speed_id));
    PHYMOD_IF_ERR_RETURN
        (_tscf_gen3_speed_id_interface_config_get(phy, speed_id, config));
    /* Read the current media type */
    pm_phy_copy.access.lane_mask = 0x1 << start_lane;
    PHYMOD_IF_ERR_RETURN
        (tscf_gen3_phy_firmware_lane_config_get(&pm_phy_copy, &firmware_lane_config));
    if (firmware_lane_config.MediaType == phymodFirmwareMediaTypeOptics) {
        PHYMOD_INTF_MODES_FIBER_SET(config);
    } else if (firmware_lane_config.MediaType == phymodFirmwareMediaTypeCopperCable) {
        PHYMOD_INTF_MODES_FIBER_CLR(config);
        PHYMOD_INTF_MODES_COPPER_SET(config);
    } else {
        PHYMOD_INTF_MODES_FIBER_CLR(config);
        PHYMOD_INTF_MODES_BACKPLANE_SET(config);
    }
    /* Next need to check for 40G 4 lanes mode, it's for XLAUI or KR4 */
    if ((config->data_rate == 40000) && (!(firmware_lane_config.DfeOn)) &&
         (firmware_lane_config.MediaType == phymodFirmwareMediaTypePcbTraceBackPlane)) {
        if (num_lane == 2) {
            config->interface_type = phymodInterfaceXLAUI2;
        } else {
            config->interface_type = phymodInterfaceXLAUI;
        }
    }
    PHYMOD_IF_ERR_RETURN
        (tscf_gen3_phy_autoneg_get(phy, &an_control, (uint32_t *) &an_done));

    /* Next need to check if we are CAUI4 100G mode */
    if (!(an_control.enable && an_done)) {
        if ((config->data_rate == 100000) && (firmware_lane_config.LpDfeOn) &&
            (firmware_lane_config.MediaType == phymodFirmwareMediaTypePcbTraceBackPlane)) {
             config->interface_type = phymodInterfaceCAUI4;
        }
    }

    switch (config->interface_type) {
    case phymodInterfaceSGMII:
        if (PHYMOD_INTF_MODES_FIBER_GET(config)) {
            config->interface_type = phymodInterface1000X;
        } else {
            config->interface_type = phymodInterfaceSGMII;
        }

        /*
         * Speed ID 0x58 and 0x60 are interpreted as phymodInterfaceSGMII
         * by _tscf_gen3_speed_id_interface_config_get(). This is correct when
         * the port is in forced speed mode. However, when AN is enabled
         * and completed the interface type cannot be SGMII because TSCF
         * does not support CL37 & CL37bam. The correct interface type in
         * this scenario should be phymodInterfaceKX.
         */
        if (an_control.enable && an_done) {
            config->interface_type = phymodInterfaceKX;
        }
        break;
    case phymodInterfaceXFI:
        if (PHYMOD_INTF_MODES_COPPER_GET(config)) {
            config->interface_type = phymodInterfaceCR;
        } else {
            config->interface_type = phymodInterfaceXFI;
        }
        break;
    case phymodInterfaceKR2:
        if (!an_control.enable) {
            if (PHYMOD_INTF_MODES_FIBER_GET(config)) {
                config->interface_type = phymodInterfaceSR2;
            } else {
                if (PHYMOD_INTF_MODES_COPPER_GET(config)) {
                    config->interface_type = phymodInterfaceCR2;
                } else {
                    config->interface_type = phymodInterfaceKR2;
                }
            }
        }
        break;
    case phymodInterfaceKR4:
        if (!an_control.enable) {
            if (PHYMOD_INTF_MODES_FIBER_GET(config)) {
                config->interface_type = phymodInterfaceSR4;
                /*if (pCfg->speed_config.line_interface == TSCF_IF_LR4) {
                    config->interface_type = phymodInterfaceLR4;
                }*/
            } else {
                if (PHYMOD_INTF_MODES_COPPER_GET(config)) {
                    config->interface_type = phymodInterfaceCR4;
                } else {
                    config->interface_type = phymodInterfaceKR4;
                }
            }
        }
        break;
    case phymodInterfaceKR:
    {
        if (!an_control.enable) {
            if(config->data_rate == 10000) {
                if (/*PHY_EXTERNAL_MODE(unit, port) ||*/ !PHYMOD_INTF_MODES_FIBER_GET(config)) {
                    if (PHYMOD_INTF_MODES_COPPER_GET(config)) {
                        config->interface_type = phymodInterfaceCR;
                    } else {
                        config->interface_type = phymodInterfaceXFI;
                    }
                } else {
                    config->interface_type = phymodInterfaceSFI;
                }
            } else{
                if (PHYMOD_INTF_MODES_FIBER_GET(config)) {
                    config->interface_type = phymodInterfaceSR;
                } else {
                    if (PHYMOD_INTF_MODES_COPPER_GET(config)) {
                        config->interface_type = phymodInterfaceCR;
                    } else {
                        config->interface_type = phymodInterfaceKR;
                    }
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

    return PHYMOD_E_NONE;
}

int tscf_gen3_phy_cl72_set(const phymod_phy_access_t* phy, uint32_t cl72_en)
{        
    struct falcon2_monterey_uc_lane_config_st serdes_firmware_config;
    PHYMOD_IF_ERR_RETURN(falcon2_monterey_get_uc_lane_cfg(&phy->access, &serdes_firmware_config));

    if(serdes_firmware_config.field.dfe_on == 0) {
      PHYMOD_DEBUG_ERROR(("ERROR :: DFE is off : Can not start CL72/CL93 with no DFE\n"));
      return PHYMOD_E_CONFIG;
    }

    PHYMOD_IF_ERR_RETURN
        (tefmod_gen3_clause72_control(&phy->access, cl72_en));

    return PHYMOD_E_NONE;
    
}

int tscf_gen3_phy_cl72_get(const phymod_phy_access_t* phy, uint32_t* cl72_en)
{        
    PHYMOD_IF_ERR_RETURN
        (falcon2_monterey_clause72_control_get(&phy->access, cl72_en));

    return PHYMOD_E_NONE;
    
}


int tscf_gen3_phy_cl72_status_get(const phymod_phy_access_t* phy, phymod_cl72_status_t* status)
{        
    uint32_t local_status; 
    PHYMOD_IF_ERR_RETURN
        (falcon2_monterey_pmd_cl72_receiver_status(&phy->access, &local_status));
    status->locked = local_status;

    return PHYMOD_E_NONE;
}


int tscf_gen3_phy_autoneg_ability_set(const phymod_phy_access_t* phy, const phymod_autoneg_ability_t* an_ability)
{        
    tefmod_gen3_an_adv_ability_t value;
    int start_lane, num_lane;
    phymod_phy_access_t phy_copy;

    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(&phy->access, &start_lane, &num_lane));

    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));
    phy_copy.access.lane_mask = 0x1 << start_lane;

    PHYMOD_MEMSET(&value, 0x0, sizeof(value));

    value.an_cl72 = an_ability->an_cl72;
    if (PHYMOD_AN_FEC_OFF_GET(an_ability->an_fec)) {
        value.an_fec = TEFMOD_FEC_SUPRTD_NOT_REQSTD;
    } else {
        if (PHYMOD_AN_FEC_CL74_GET(an_ability->an_fec)){
            value.an_fec = TEFMOD_FEC_CL74_SUPRTD_REQSTD;
        }
       if (PHYMOD_AN_FEC_CL91_GET(an_ability->an_fec)){
            value.an_fec |= TEFMOD_FEC_CL91_SUPRTD_REQSTD;
       }
    }

    /* value.an_fec = an_ability->an_fec; */
    value.an_hg2 = an_ability->an_hg2;

    /* next check pause */
    if (PHYMOD_AN_CAP_SYMM_PAUSE_GET(an_ability) && !PHYMOD_AN_CAP_ASYM_PAUSE_GET(an_ability)) {
        value.an_pause = TEFMOD_SYMM_PAUSE;
    }
    if (PHYMOD_AN_CAP_ASYM_PAUSE_GET(an_ability) && !PHYMOD_AN_CAP_SYMM_PAUSE_GET(an_ability)) {
        value.an_pause = TEFMOD_ASYM_PAUSE;
    }
    if (PHYMOD_AN_CAP_ASYM_PAUSE_GET(an_ability) && PHYMOD_AN_CAP_SYMM_PAUSE_GET(an_ability)) {
        value.an_pause = TEFMOD_ASYM_SYMM_PAUSE;
    }
    /* check cl73 ability */
    if (PHYMOD_AN_CAP_1G_KX_GET(an_ability->an_cap)){
        value.an_base_speed |= 1 << TEFMOD_CL73_1GBASE_KX;
    }
    if (PHYMOD_AN_CAP_10G_KR_GET(an_ability->an_cap)){
        value.an_base_speed |= 1 << TEFMOD_CL73_10GBASE_KR1;
    }
    if (PHYMOD_AN_CAP_40G_KR4_GET(an_ability->an_cap)){
        value.an_base_speed |= 1 << TEFMOD_CL73_40GBASE_KR4;
    }
    if (PHYMOD_AN_CAP_40G_CR4_GET(an_ability->an_cap)){
        value.an_base_speed |= 1 << TEFMOD_CL73_40GBASE_CR4;
    }
    if (PHYMOD_AN_CAP_100G_KR4_GET(an_ability->an_cap)) {
        value.an_base_speed |= 1 << TEFMOD_CL73_100GBASE_KR4;
    }
    if (PHYMOD_AN_CAP_100G_CR4_GET(an_ability->an_cap)) {
        value.an_base_speed |= 1 << TEFMOD_CL73_100GBASE_CR4;
    }
    if (PHYMOD_AN_CAP_25G_CR1_GET(an_ability->an_cap)){
        value.an_base_speed |= 1 << TEFMOD_CL73_25GBASE_CR1;
    }
    if (PHYMOD_AN_CAP_25G_KR1_GET(an_ability->an_cap)){
        value.an_base_speed |= 1 << TEFMOD_CL73_25GBASE_KR1;
    }
    if (PHYMOD_AN_CAP_25G_CRS1_GET(an_ability->an_cap)){
        value.an_base_speed |= 1 << TEFMOD_CL73_25GBASE_CRS1;
    }
    if (PHYMOD_AN_CAP_25G_KRS1_GET(an_ability->an_cap)){
       value.an_base_speed |= 1 << TEFMOD_CL73_25GBASE_KRS1;
    }
    if (PHYMOD_AN_CAP_2P5G_X_GET(an_ability->an_cap)){
       value.an_base_speed |= 1 << TEFMOD_CL73_2P5GBASE_KX1;
    }
    if (PHYMOD_AN_CAP_5G_KR1_GET(an_ability->an_cap)){
       value.an_base_speed |= 1 << TEFMOD_CL73_5GBASE_KR1;
    }

    /* check cl73 bam ability */
    if (PHYMOD_BAM_CL73_CAP_20G_KR2_GET(an_ability->cl73bam_cap)){
        value.an_bam_speed |= 1 << TEFMOD_CL73_BAM_20GBASE_KR2;
    }
    if (PHYMOD_BAM_CL73_CAP_20G_CR2_GET(an_ability->cl73bam_cap)){
        value.an_bam_speed |= 1 << TEFMOD_CL73_BAM_20GBASE_CR2;
    }
    if (PHYMOD_BAM_CL73_CAP_40G_KR2_GET(an_ability->cl73bam_cap)){
        value.an_bam_speed |= 1 << TEFMOD_CL73_BAM_40GBASE_KR2;
    }
    if (PHYMOD_BAM_CL73_CAP_40G_CR2_GET(an_ability->cl73bam_cap)){
        value.an_bam_speed |= 1 << TEFMOD_CL73_BAM_40GBASE_CR2;
    }
    if (PHYMOD_BAM_CL73_CAP_50G_KR2_GET(an_ability->cl73bam_cap)){
        value.an_bam_speed |= 1 << TEFMOD_CL73_BAM_50GBASE_KR2;
    }
    if (PHYMOD_BAM_CL73_CAP_50G_CR2_GET(an_ability->cl73bam_cap)){
        value.an_bam_speed |= 1 << TEFMOD_CL73_BAM_50GBASE_CR2;
    }
    if (PHYMOD_BAM_CL73_CAP_50G_KR4_GET(an_ability->cl73bam_cap)){
        value.an_bam_speed |= 1 << TEFMOD_CL73_BAM_50GBASE_KR4;
    }
    if (PHYMOD_BAM_CL73_CAP_50G_CR4_GET(an_ability->cl73bam_cap)){
        value.an_bam_speed |= 1 << TEFMOD_CL73_BAM_50GBASE_CR4;
    }
    if (PHYMOD_BAM_CL73_CAP_20G_KR1_GET(an_ability->cl73bam_cap)){
        value.an_bam_speed1 |= 1 << TEFMOD_CL73_BAM_20GBASE_KR1;
    }
    if (PHYMOD_BAM_CL73_CAP_20G_CR1_GET(an_ability->cl73bam_cap)){
        value.an_bam_speed1 |= 1 << TEFMOD_CL73_BAM_20GBASE_CR1;
    }
    if (PHYMOD_BAM_CL73_CAP_25G_KR1_GET(an_ability->cl73bam_cap)){
        value.an_bam_speed1 |= 1 << TEFMOD_CL73_BAM_25GBASE_KR1;
    }
    if (PHYMOD_BAM_CL73_CAP_25G_CR1_GET(an_ability->cl73bam_cap)){
        value.an_bam_speed1 |= 1 << TEFMOD_CL73_BAM_25GBASE_CR1;
    }
    PHYMOD_IF_ERR_RETURN
        (tefmod_gen3_autoneg_ability_set(&phy_copy.access, &value));
 
    return PHYMOD_E_NONE;
}

int tscf_gen3_phy_autoneg_ability_get(const phymod_phy_access_t* phy, phymod_autoneg_ability_t* an_ability_get_type)
{        
    tefmod_gen3_an_adv_ability_t value;
    phymod_phy_access_t phy_copy;
    int start_lane, num_lane;

    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(&phy->access, &start_lane, &num_lane));
    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));
    phy_copy.access.lane_mask = 0x1 << start_lane;
    PHYMOD_MEMSET(&value, 0x0, sizeof(value));
    
    /* Place your code here */

    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(&phy->access, &start_lane, &num_lane));
    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));
    phy_copy.access.lane_mask = 0x1 << start_lane;
    PHYMOD_MEMSET(&value, 0x0, sizeof(value));
    
    PHYMOD_IF_ERR_RETURN
        (tefmod_gen3_autoneg_ability_get(&phy_copy.access, &value));
    an_ability_get_type->an_cl72 = value.an_cl72;

    an_ability_get_type->an_fec = 0;
    if (value.an_fec == TEFMOD_FEC_NOT_SUPRTD) {
 
    } else if (value.an_fec == TEFMOD_FEC_SUPRTD_NOT_REQSTD) {
        PHYMOD_AN_FEC_OFF_SET(an_ability_get_type->an_fec);
    } else {
        if (value.an_fec & TEFMOD_FEC_CL74_SUPRTD_REQSTD) {
            PHYMOD_AN_FEC_CL74_SET(an_ability_get_type->an_fec);
        }
        if (value.an_fec & TEFMOD_FEC_CL91_SUPRTD_REQSTD) {
            PHYMOD_AN_FEC_CL91_SET(an_ability_get_type->an_fec);
        }
    }

    if (value.an_pause == TEFMOD_ASYM_PAUSE) {
        PHYMOD_AN_CAP_ASYM_PAUSE_SET(an_ability_get_type);
    } else if (value.an_pause == TEFMOD_SYMM_PAUSE) {
        PHYMOD_AN_CAP_SYMM_PAUSE_SET(an_ability_get_type);
    } else if (value.an_pause == TEFMOD_ASYM_SYMM_PAUSE) {
        PHYMOD_AN_CAP_ASYM_PAUSE_SET(an_ability_get_type);
        PHYMOD_AN_CAP_SYMM_PAUSE_SET(an_ability_get_type);
    }

    /* first check cl73 base  ability */
    if (value.an_base_speed &  1 << TEFMOD_CL73_100GBASE_CR4) {
        PHYMOD_AN_CAP_100G_CR4_SET(an_ability_get_type->an_cap);
    }
    if (value.an_base_speed & 1 << TEFMOD_CL73_100GBASE_KR4) {
        PHYMOD_AN_CAP_100G_KR4_SET(an_ability_get_type->an_cap);
    }
    if (value.an_base_speed & 1 << TEFMOD_CL73_40GBASE_CR4) {
        PHYMOD_AN_CAP_40G_CR4_SET(an_ability_get_type->an_cap);
    }
    if (value.an_base_speed & 1 << TEFMOD_CL73_40GBASE_KR4) {
        PHYMOD_AN_CAP_40G_KR4_SET(an_ability_get_type->an_cap);
    }
    if (value.an_base_speed & 1 << TEFMOD_CL73_10GBASE_KR1) {
        PHYMOD_AN_CAP_10G_KR_SET(an_ability_get_type->an_cap);
    }
    if (value.an_base_speed & 1 << TEFMOD_CL73_1GBASE_KX) {
        PHYMOD_AN_CAP_1G_KX_SET(an_ability_get_type->an_cap);
    }
    if (value.an_base_speed & 1 << TEFMOD_CL73_25GBASE_KR1) {
        PHYMOD_AN_CAP_25G_KR1_SET(an_ability_get_type->an_cap);
    }
    if (value.an_base_speed & 1 << TEFMOD_CL73_25GBASE_CR1) {
        PHYMOD_AN_CAP_25G_CR1_SET(an_ability_get_type->an_cap);
    }
    if (value.an_base_speed & 1 << TEFMOD_CL73_25GBASE_KRS1) {
        PHYMOD_AN_CAP_25G_KRS1_SET(an_ability_get_type->an_cap);
    }
    if (value.an_base_speed & 1 << TEFMOD_CL73_25GBASE_CRS1) {
        PHYMOD_AN_CAP_25G_CRS1_SET(an_ability_get_type->an_cap);
    }
    if (value.an_base_speed & 1 << TEFMOD_CL73_2P5GBASE_KX1) {
        PHYMOD_AN_CAP_2P5G_X_SET(an_ability_get_type->an_cap);
    }
    if (value.an_base_speed & 1 << TEFMOD_CL73_5GBASE_KR1) {
        PHYMOD_AN_CAP_5G_KR1_SET(an_ability_get_type->an_cap);
    }

    /* next check cl73 bam ability */
    if (value.an_bam_speed & 1 << TEFMOD_CL73_BAM_20GBASE_KR2){
        PHYMOD_BAM_CL73_CAP_20G_KR2_SET(an_ability_get_type->cl73bam_cap);
    }
    if (value.an_bam_speed & 1 << TEFMOD_CL73_BAM_20GBASE_CR2){
        PHYMOD_BAM_CL73_CAP_20G_CR2_SET(an_ability_get_type->cl73bam_cap);
    }
    if (value.an_bam_speed & 1 << TEFMOD_CL73_BAM_40GBASE_KR2){
        PHYMOD_BAM_CL73_CAP_40G_KR2_SET(an_ability_get_type->cl73bam_cap);
    }
    if (value.an_bam_speed & 1 << TEFMOD_CL73_BAM_40GBASE_CR2){
        PHYMOD_BAM_CL73_CAP_40G_CR2_SET(an_ability_get_type->cl73bam_cap);
    }
    if (value.an_bam_speed & 1 << TEFMOD_CL73_BAM_50GBASE_KR2){
        PHYMOD_BAM_CL73_CAP_50G_KR2_SET(an_ability_get_type->cl73bam_cap);
    }
    if (value.an_bam_speed & 1 << TEFMOD_CL73_BAM_50GBASE_CR2){
        PHYMOD_BAM_CL73_CAP_50G_CR2_SET(an_ability_get_type->cl73bam_cap);
    }
    if (value.an_bam_speed & 1 << TEFMOD_CL73_BAM_50GBASE_KR4){
        PHYMOD_BAM_CL73_CAP_50G_KR4_SET(an_ability_get_type->cl73bam_cap);
    }
    if (value.an_bam_speed & 1 << TEFMOD_CL73_BAM_50GBASE_CR4){
        PHYMOD_BAM_CL73_CAP_50G_CR4_SET(an_ability_get_type->cl73bam_cap);
    }
    if (value.an_bam_speed1 & 1 << TEFMOD_CL73_BAM_20GBASE_KR1){
        PHYMOD_BAM_CL73_CAP_20G_KR1_SET(an_ability_get_type->cl73bam_cap);
    }
    if (value.an_bam_speed1 & 1 << TEFMOD_CL73_BAM_20GBASE_CR1){
        PHYMOD_BAM_CL73_CAP_20G_CR1_SET(an_ability_get_type->cl73bam_cap);
    }
    if (value.an_bam_speed1 & 1 << TEFMOD_CL73_BAM_25GBASE_KR1){
        PHYMOD_BAM_CL73_CAP_25G_KR1_SET(an_ability_get_type->cl73bam_cap);
    }
    if (value.an_bam_speed1 & 1 << TEFMOD_CL73_BAM_25GBASE_CR1) {
        PHYMOD_BAM_CL73_CAP_25G_CR1_SET(an_ability_get_type->cl73bam_cap);
    }    

    return PHYMOD_E_NONE;
}

int tscf_gen3_phy_autoneg_remote_ability_get(const phymod_phy_access_t* phy, phymod_autoneg_ability_t* an_ability_get_type)
{        

    uint16_t baseP0, baseP1, baseP2, nextP3, nextP4;
    int an_fec;

    PHYMOD_IF_ERR_RETURN
       (tefmod_gen3_autoneg_lp_status_get(&phy->access, &baseP0, &baseP1, &baseP2, &nextP3, &nextP4));

    if((baseP1 >> 5) & 1) {  /* D0 */
      PHYMOD_AN_CAP_1G_KX_SET(an_ability_get_type->an_cap);
    }
    if((baseP1 >> 7) & 1) {  /* D2 */
      PHYMOD_AN_CAP_10G_KR_SET(an_ability_get_type->an_cap);
    }
    if((baseP1 >> 8) & 1) {  /* D3 */
      PHYMOD_AN_CAP_40G_KR4_SET(an_ability_get_type->an_cap);
    }
    if((baseP1 >> 9) & 1) {  /* D4 */
      PHYMOD_AN_CAP_40G_CR4_SET(an_ability_get_type->an_cap);
    }
    if((baseP1 >> 12) & 1) {  /* D7 */
      PHYMOD_AN_CAP_100G_KR4_SET(an_ability_get_type->an_cap);
    }
    if((baseP1 >> 13) & 1) {  /* D8 */
      PHYMOD_AN_CAP_100G_CR4_SET(an_ability_get_type->an_cap);
    }

    if((baseP1 >> 14) & 1) {
      PHYMOD_AN_CAP_25G_KRS1_SET(an_ability_get_type->an_cap);
      PHYMOD_AN_CAP_25G_CRS1_SET(an_ability_get_type->an_cap);
    }
    if((baseP1 >> 15) & 1) {
      PHYMOD_AN_CAP_25G_KR1_SET(an_ability_get_type->an_cap);
      PHYMOD_AN_CAP_25G_CR1_SET(an_ability_get_type->an_cap);
    }

    an_fec = (baseP2 >> 14) & 3; /* D47:D46 */

        an_ability_get_type->an_fec = 0;
    if (an_fec == 0x0) {

    } else if (an_fec == 0x1) {
        PHYMOD_AN_FEC_OFF_SET(an_ability_get_type->an_fec);
    } else {
        if (an_fec == 0x3) {
            PHYMOD_AN_FEC_CL74_SET(an_ability_get_type->an_fec);
        }
    }

    an_fec = (baseP2 >> 12) & 3; /* D45:D44 */
    if (an_fec & 0x1) {
        PHYMOD_AN_FEC_CL91_SET(an_ability_get_type->an_fec);
    }
    if (an_fec & 0x2) {
        PHYMOD_AN_FEC_CL74_SET(an_ability_get_type->an_fec);
    }

    if((baseP0 >> 10) & 1) {   /* C0 */
      PHYMOD_AN_CAP_SYMM_PAUSE_SET(an_ability_get_type);
    }
    if((baseP0 >> 11) & 1) {    /* C1 */
      PHYMOD_AN_CAP_ASYM_PAUSE_SET(an_ability_get_type);
    }

    if((nextP3 >> 0) & 1) {
      PHYMOD_BAM_CL73_CAP_20G_KR2_SET(an_ability_get_type->cl73bam_cap);
    }
    if((nextP3 >> 1) & 1) {
      PHYMOD_BAM_CL73_CAP_20G_CR2_SET(an_ability_get_type->cl73bam_cap);
    }
    if((nextP3 >> 2) & 1) {
      PHYMOD_BAM_CL73_CAP_20G_KR1_SET(an_ability_get_type->cl73bam_cap);
    }
    if((nextP3 >> 3) & 1) {
      PHYMOD_BAM_CL73_CAP_20G_CR1_SET(an_ability_get_type->cl73bam_cap);
    }
    if((nextP3 >> 4) & 1) {
      PHYMOD_BAM_CL73_CAP_25G_KR1_SET(an_ability_get_type->cl73bam_cap);
    }
    if((nextP3 >> 5) & 1) {
      PHYMOD_BAM_CL73_CAP_25G_CR1_SET(an_ability_get_type->cl73bam_cap);
    }
    if((nextP3 >> 6) & 1) {
      PHYMOD_BAM_CL73_CAP_40G_KR2_SET(an_ability_get_type->cl73bam_cap);
    }
    if((nextP3 >> 7) & 1) {
      PHYMOD_BAM_CL73_CAP_40G_CR2_SET(an_ability_get_type->cl73bam_cap);
    }
    if((nextP3 >> 8) & 1) {
      PHYMOD_BAM_CL73_CAP_50G_KR2_SET(an_ability_get_type->cl73bam_cap);
    }
    if((nextP3 >> 9) & 1) {
      PHYMOD_BAM_CL73_CAP_50G_CR2_SET(an_ability_get_type->cl73bam_cap);
    }
    if((nextP4 >> 0) & 1) {
      PHYMOD_BAM_CL73_CAP_50G_KR4_SET(an_ability_get_type->cl73bam_cap);
    }
    if((nextP4 >> 1) & 1) {
      PHYMOD_BAM_CL73_CAP_50G_CR4_SET(an_ability_get_type->cl73bam_cap);
    }

    return PHYMOD_E_NONE;
}

int tscf_gen3_phy_autoneg_set(const phymod_phy_access_t* phy, const phymod_autoneg_control_t* an)
{        
    int num_lane_adv_encoded;
    int start_lane, num_lane;
    int i, an_enable_bitmap, single_port_mode = 0;
    int do_lane_config_set, do_core_config_set;
    phymod_firmware_lane_config_t firmware_lane_config;
    phymod_firmware_core_config_t firmware_core_config_tmp;
    tefmod_gen3_an_control_t an_control;
    phymod_phy_access_t phy_copy;
    tefmod_gen3_an_adv_ability_t value;

    PHYMOD_MEMSET(&firmware_lane_config, 0x0, sizeof(firmware_lane_config));
    PHYMOD_MEMSET(&firmware_core_config_tmp, 0x0, sizeof(firmware_core_config_tmp));

    PHYMOD_MEMSET(&an_control, 0x0, sizeof(an_control));
    PHYMOD_MEMSET(&value, 0x0, sizeof(value));

    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));
    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(&phy->access, &start_lane, &num_lane));
    PHYMOD_IF_ERR_RETURN
        (tefmod_gen3_autoneg_ability_get(&phy->access, &value));

    if (value.an_hg2){
        single_port_mode = 1;
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
        case 10:
            num_lane_adv_encoded = 3;
            break;
        default:
            return PHYMOD_E_PARAM;
    }

    /* Check if cl72 is allowed to change */
    if (PHYMOD_AN_F_ALLOW_CL72_CONFIG_CHANGE_GET(an)) {
        an_control.cl72_config_allowed = 1;
    } else {
        an_control.cl72_config_allowed = 0;
    }

    if(PHYMOD_AN_F_SET_CL73_PDET_KX_ENABLE_GET(an)) {
        an_control.pd_kx_en = 1;
    } else {
        an_control.pd_kx_en = 0;
    }
    if(PHYMOD_AN_F_SET_CL73_PDET_2P5G_KX_ENABLE_GET(an)) {
        an_control.pd_2P5G_kx_en = 1;
    } else {
        an_control.pd_2P5G_kx_en = 0;
    } 

    an_control.num_lane_adv = num_lane_adv_encoded;
    an_control.enable       = an->enable;
    an_control.an_property_type = 0x0;   /* for now disable */
    switch (an->an_mode) {
    case phymod_AN_MODE_CL73:
        an_control.an_type = TEFMOD_AN_MODE_CL73;
        break;
    case phymod_AN_MODE_CL73BAM:
        an_control.an_type = TEFMOD_AN_MODE_CL73BAM;
        break;
    case phymod_AN_MODE_MSA:
        an_control.an_type = TEFMOD_AN_MODE_MSA;
        break;
    case phymod_AN_MODE_CL73_MSA:
        an_control.an_type = TEFMOD_AN_MODE_CL73_MSA;
        break;
    case phymod_AN_MODE_HPAM:
        an_control.an_type = TEFMOD_AN_MODE_HPAM;
        break;
    default:
        an_control.an_type = TEFMOD_AN_MODE_CL73;
        break;
    }

    PHYMOD_IF_ERR_RETURN(tefmod_gen3_disable_set(&phy->access));

    if (single_port_mode) {
        PHYMOD_IF_ERR_RETURN
            (tefmod_gen3_set_an_single_port_mode(&phy->access, an->enable));
    }

    if(an->enable) {
        PHYMOD_IF_ERR_RETURN(tefmod_gen3_set_an_port_mode(&phy->access, num_lane, start_lane));
    }

    /* Check if any other lane has An on */
    an_enable_bitmap = 0;
    phy_copy.access.lane_mask = 0x1 << start_lane;
    if (!an->enable) {
        for (i = 0; i < 4; i++ ) {
            phy_copy.access.lane_mask = 0x1 << i;
            if (phy_copy.access.lane_mask & phy->access.lane_mask)  continue;
            PHYMOD_IF_ERR_RETURN
                (tscf_gen3_phy_firmware_lane_config_get(&phy_copy, &firmware_lane_config));
            if (firmware_lane_config.AnEnabled) {
                an_enable_bitmap |= 1 << i;
            }
        }

    }

    phy_copy.access.lane_mask = 0x1 << start_lane;

    /* make sure the firmware config is set to an eenabled */
    PHYMOD_IF_ERR_RETURN
        (tscf_gen3_phy_firmware_lane_config_get(&phy_copy, &firmware_lane_config));
    /* make sure the firmware config is set to an eenabled */
    PHYMOD_IF_ERR_RETURN
        (tscf_gen3_phy_firmware_core_config_get(&phy_copy, &firmware_core_config_tmp));
    do_lane_config_set = 0;
    do_core_config_set = 0;

    if (an->enable) {
        if(firmware_lane_config.AnEnabled != 1) {
          firmware_lane_config.AnEnabled = 1;
          do_lane_config_set = 1;
        }
        if(firmware_lane_config.LaneConfigFromPCS != 1) {
          firmware_lane_config.LaneConfigFromPCS = 1;
          do_lane_config_set = 1;
        }
        if(firmware_core_config_tmp.CoreConfigFromPCS != 1) {
          firmware_core_config_tmp.CoreConfigFromPCS = 1;
          do_core_config_set = 1;
        }
        firmware_lane_config.Cl72RestTO = 0;
    } else {
        if(firmware_lane_config.AnEnabled != 0) {
          firmware_lane_config.AnEnabled = 0;
          do_lane_config_set = 1;
        }
        if(firmware_lane_config.LaneConfigFromPCS != 0) {
          firmware_lane_config.LaneConfigFromPCS = 0;
          do_lane_config_set = 1;
        }
        if(firmware_core_config_tmp.CoreConfigFromPCS != 0) {
            if (!an_enable_bitmap) {
                firmware_core_config_tmp.CoreConfigFromPCS = 0;
                do_core_config_set = 1;
            }
        }
        firmware_lane_config.Cl72RestTO = 1;
    }

    if(do_core_config_set && single_port_mode) {
        PHYMOD_IF_ERR_RETURN
            (falcon2_monterey_core_soft_reset_release(&phy_copy.access, 0));
        PHYMOD_USLEEP(1000);

        PHYMOD_IF_ERR_RETURN
            (tscf_gen3_phy_firmware_core_config_set(&phy_copy, firmware_core_config_tmp));

        PHYMOD_IF_ERR_RETURN
            (falcon2_monterey_core_soft_reset_release(&phy_copy.access, 1));
    }

    if(do_lane_config_set) {
        for (i = 0; i < num_lane; i++) {
            if (!PHYMOD_LANEPBMP_MEMBER(phy->access.lane_mask, start_lane + i)) {
                continue;
            }
            phy_copy.access.lane_mask = 0x1 << (i + start_lane);
            PHYMOD_IF_ERR_RETURN
                (falcon2_monterey_lane_soft_reset_release(&phy_copy.access, 0));
        }

        PHYMOD_USLEEP(1000);
        for (i = 0; i < num_lane; i++) {
            if (!PHYMOD_LANEPBMP_MEMBER(phy->access.lane_mask, start_lane + i)) {
                continue;
            }
            phy_copy.access.lane_mask = 0x1 << (i + start_lane);
            PHYMOD_IF_ERR_RETURN
                (_tscf_gen3_phy_firmware_lane_config_set(&phy_copy, firmware_lane_config));
        }
        for (i = 0; i < num_lane; i++) {
            if (!PHYMOD_LANEPBMP_MEMBER(phy->access.lane_mask, start_lane + i)) {
                continue;
            }
            phy_copy.access.lane_mask = 0x1 << (i + start_lane);
            PHYMOD_IF_ERR_RETURN
                (falcon2_monterey_lane_soft_reset_release(&phy_copy.access, 1));
        }
    }

    phy_copy.access.lane_mask = 0x1 << start_lane;
    if(an->enable && single_port_mode) {
        PHYMOD_IF_ERR_RETURN
            (tefmod_gen3_master_port_num_set(&phy_copy.access, start_lane));
    }
    PHYMOD_IF_ERR_RETURN
        (tefmod_gen3_autoneg_control(&phy_copy.access, &an_control));

    if (!an->enable) {
        PHYMOD_IF_ERR_RETURN(tefmod_gen3_enable_set(&phy->access));
    }

    return PHYMOD_E_NONE;
}

int tscf_gen3_phy_autoneg_get(const phymod_phy_access_t* phy, phymod_autoneg_control_t* an, uint32_t* an_done)
{        
    tefmod_gen3_an_control_t an_control;
    phymod_phy_access_t phy_copy;
    int start_lane, num_lane;
    int an_complete = 0;

    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));
    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(&phy->access, &start_lane, &num_lane));
    phy_copy.access.lane_mask = 0x1 << start_lane;

    PHYMOD_MEMSET(&an_control, 0x0,  sizeof(tefmod_gen3_an_control_t));
    PHYMOD_IF_ERR_RETURN
        (tefmod_gen3_autoneg_control_get(&phy_copy.access, &an_control, &an_complete));

    if (an_control.enable) {
        an->enable = 1;
        *an_done = an_complete;
    } else {
        an->enable = 0;
    }

    if(an_control.pd_kx_en) {
        PHYMOD_AN_F_SET_CL73_PDET_KX_ENABLE_SET(an) ;
    }
    if(an_control.pd_2P5G_kx_en) {
        PHYMOD_AN_F_SET_CL73_PDET_2P5G_KX_ENABLE_SET(an) ;
    }

    switch (an_control.an_type) {
    case TEFMOD_AN_MODE_CL73:
        an->an_mode = phymod_AN_MODE_CL73;
        break;
    case TEFMOD_AN_MODE_CL73BAM:
        an->an_mode = phymod_AN_MODE_CL73BAM;
        break;
    case TEFMOD_AN_MODE_MSA:
        an->an_mode = phymod_AN_MODE_MSA;
        break;
    case TEFMOD_AN_MODE_CL73_MSA:
        an->an_mode = phymod_AN_MODE_CL73_MSA;
        break;
    case TEFMOD_AN_MODE_HPAM:
        an->an_mode = phymod_AN_MODE_HPAM;
        break;
    default:
        an->an_mode = phymod_AN_MODE_NONE;
        break;
    }
    
    return PHYMOD_E_NONE;
}

int tscf_gen3_phy_autoneg_status_get(const phymod_phy_access_t* phy, phymod_autoneg_status_t* status)
{        

    int speed_id, an_en, an_done;
    phymod_phy_inf_config_t config;
    phymod_ref_clk_t ref_clock;
    const phymod_access_t *pm_acc = &phy->access;

    PHYMOD_IF_ERR_RETURN
       (tefmod_gen3_autoneg_status_get(&phy->access, &an_en, &an_done));

    PHYMOD_IF_ERR_RETURN
       (tefmod_gen3_speed_id_get(&phy->access, &speed_id));

    PHYMOD_IF_ERR_RETURN
       (tefmod_gen3_refclk_get(pm_acc, &ref_clock));

    config.ref_clock = ref_clock;
    PHYMOD_IF_ERR_RETURN
       (_tscf_gen3_speed_id_interface_config_get(phy, speed_id, &config));

    status->enabled   = an_en;
    status->locked    = an_done;
    status->data_rate = config.data_rate;
    status->interface = config.interface_type;

    return PHYMOD_E_NONE;
}

/* Core initialization
 * 1. De-assert PMD core and PMD lane reset
 * 2. Set heartbeat for comclk
 * 3. Configure lane mapping
 * 4. Micro code load and verify
 * 4.1. Set uc_active = 1
 * 4.2. De-assert micro reset
 * 4.3. Poll for us_dsc_ready_for_cmd = 1 
 * 5. Configure registers for each PLL core and power up each PLL 
 * 6. Configure Core level micro RAM variables for each PLL core
 * 7. Release core DP soft (core_dp_s_rstb) reset for each PLL core
 */
STATIC
int _tscf_gen3_core_init_pass1(const phymod_core_access_t* core, const phymod_core_init_config_t* init_config, const phymod_core_status_t* core_status)
{
    int rv;
    int lane;
    phymod_phy_access_t phy_access;
    phymod_core_access_t  core_copy;
    uint32_t uc_enable = 0;

    TSCF_GEN3_CORE_TO_PHY_ACCESS(&phy_access, core);
    PHYMOD_MEMCPY(&core_copy, core, sizeof(core_copy));
    core_copy.access.lane_mask = 0x1;

    /* De-assert PMD core power and core data path reset */
    PHYMOD_IF_ERR_RETURN
        (tefmod_gen3_pmd_reset_seq(&core_copy.access, core_status->pmd_active));

    /* Release lane reset */
    for (lane = 0; lane < TSCF_GEN3_NOF_LANES_IN_CORE; lane++) {
        phy_access.access.lane_mask = 1 << lane;
        PHYMOD_IF_ERR_RETURN
            (tefmod_gen3_pmd_x4_reset(&phy_access.access));
    }

    PHYMOD_IF_ERR_RETURN
        (falcon2_monterey_uc_active_get(&phy_access.access, &uc_enable));
    if (uc_enable) return PHYMOD_E_NONE;

    /* Set the heart beat, based on clock src. */
    PHYMOD_IF_ERR_RETURN
        (tefmod_gen3_refclk_set(&core_copy.access, init_config->interface.ref_clock));

    /* Configure lane mapping */
    /* doing PMD lane swap */
    PHYMOD_IF_ERR_RETURN
        (_tscf_gen3_core_lane_map_part_set(&core_copy, &init_config->lane_map, 0));

    /*  Micro code load */
    rv = _tscf_gen3_core_firmware_load(&core_copy, init_config->firmware_load_method, init_config->firmware_loader);
    if (rv != PHYMOD_E_NONE) {
        PHYMOD_DEBUG_ERROR(("devad 0x%"PRIx32" lane 0x%"PRIx32": UC firmware-load failed\n", core->access.addr, core->access.lane_mask));
        PHYMOD_IF_ERR_RETURN(rv);
    }

#ifndef TSCF_GEN3_PMD_CRC_UCODE_VERIFY
    if (init_config->firmware_load_method != phymodFirmwareLoadMethodNone) {
        int rv;
        /* Read-back uCode from Program RAM and verify against ucode_image */
        rv = falcon2_monterey_ucode_load_verify(&core_copy.access, (uint8_t *) &falcon_dpll_ucode, falcon_dpll_ucode_len);
        if (rv != PHYMOD_E_NONE) {
            PHYMOD_DEBUG_ERROR(("devad 0x%x lane 0x%x: UC load-verify failed\n", core->access.addr, core->access.lane_mask));
            PHYMOD_IF_ERR_RETURN(rv);
        }
    }
#endif
    /* pmd lane hard reset */
    PHYMOD_IF_ERR_RETURN
        (falcon2_monterey_pmd_ln_h_rstb_pkill_override(&core_copy.access, 0x1));

    /* Set uc_active = 1 */
    PHYMOD_IF_ERR_RETURN
        (falcon2_monterey_uc_active_set(&core_copy.access, 1));

    /* De-assert micro reset */
    PHYMOD_IF_ERR_RETURN
        (falcon2_monterey_uc_reset(&core_copy.access, 0));

    if (PHYMOD_CORE_INIT_F_FIRMWARE_LOAD_VERIFY_GET(init_config)) {
#ifdef TSCF_GEN3_PMD_CRC_UCODE_VERIFY
        PHYMOD_IF_ERR_RETURN(
            falcon2_monterey_start_ucode_crc_calc(&core_copy.access, falcon_dpll_ucode_len));
#endif
    }

    return PHYMOD_E_NONE;
}

STATIC
int _tscf_gen3_core_init_pass2(const phymod_core_access_t* core, const phymod_core_init_config_t* init_config, const phymod_core_status_t* core_status)
{
    phymod_phy_access_t phy_access, phy_access_copy;
    phymod_core_access_t  core_copy;
    phymod_firmware_core_config_t firmware_core_config_tmp;
    uint32_t vco_rate;

    TSCF_GEN3_CORE_TO_PHY_ACCESS(&phy_access, core);
    PHYMOD_MEMCPY(&core_copy, core, sizeof(core_copy));
    core_copy.access.lane_mask = 0x1;

    phy_access_copy = phy_access;
    phy_access_copy.access = core->access;
    phy_access_copy.access.lane_mask = 0x1;
    phy_access_copy.type = core->type;

    /* doing PCS lane swap */
    PHYMOD_IF_ERR_RETURN
        (_tscf_gen3_core_lane_map_part_set(&core_copy, &init_config->lane_map, 1));
    if (PHYMOD_CORE_INIT_F_FIRMWARE_LOAD_VERIFY_GET(init_config)) {
#ifndef TSCF_GEN3_PMD_CRC_UCODE_VERIFY
        /* poll the ready bit */
        PHYMOD_IF_ERR_RETURN(
            falcon2_monterey_poll_uc_dsc_ready_for_cmd_equals_1(&core_copy.access, 100));
#else
        PHYMOD_IF_ERR_RETURN(
            falcon2_monterey_check_ucode_crc(&core_copy.access, falcon_dpll_ucode_crc, 200));
#endif
    }

    /* release pmd lane hard reset */
    PHYMOD_IF_ERR_RETURN
        (falcon2_monterey_pmd_ln_h_rstb_pkill_override(&phy_access_copy.access, 0x0));

    PHYMOD_IF_ERR_RETURN
        (tefmod_gen3_autoneg_timer_init(&core_copy.access));

    PHYMOD_IF_ERR_RETURN
        (tefmod_gen3_master_port_num_set(&core_copy.access, 0));

    phy_access_copy.access.lane_mask = 0x1;
    /* Configure Core level RAM variables */
    firmware_core_config_tmp = init_config->firmware_core_config;
    firmware_core_config_tmp.CoreConfigFromPCS = 0;
    /* set charge pump current */
    if (init_config->afe_pll.afe_pll_change_default){
        firmware_core_config_tmp.disable_write_pll_iqp = 1;
    } else {
        firmware_core_config_tmp.disable_write_pll_iqp = 0;
    }

    /* reset core DP */
    /* Only PLL1 is used for Eth port in mixed mode */
    if (init_config->pll0_div_init_value){
        core_copy.access.pll_idx = 0; 
        PHYMOD_IF_ERR_RETURN
            (falcon2_monterey_afe_pll_reg_set(&core_copy.access, &init_config->afe_pll));
        PHYMOD_IF_ERR_RETURN
            (falcon2_monterey_core_soft_reset_release(&core_copy.access, 0));
    }

    core_copy.access.pll_idx = 1; 
    PHYMOD_IF_ERR_RETURN
        (falcon2_monterey_afe_pll_reg_set(&core_copy.access, &init_config->afe_pll));
    PHYMOD_IF_ERR_RETURN
        (falcon2_monterey_core_soft_reset_release(&core_copy.access, 0));

    /* PLL0 config */
    /* Skip to program PLL0, Only PLL1 is used for Eth port in mixed mode */
    if (init_config->pll0_div_init_value){
        phy_access_copy.access.pll_idx = 0;
        PHYMOD_IF_ERR_RETURN
            (tefmod_gen3_configure_pll(&phy_access_copy.access, init_config->pll0_div_init_value, init_config->interface.ref_clock));
        /* Get Core config variable VcoRate */
        PHYMOD_IF_ERR_RETURN
            (tefmod_gen3_pll_to_vco_rate(&phy_access_copy.access, init_config->pll0_div_init_value, init_config->interface.ref_clock, &vco_rate));
        firmware_core_config_tmp.VcoRate = MHZ_TO_VCO_RATE(vco_rate);
        /* Configure Core level RAM variables */
        PHYMOD_IF_ERR_RETURN
            (tscf_gen3_phy_firmware_core_config_set(&phy_access_copy, firmware_core_config_tmp));
    }

    /* PLL1 config */
    phy_access_copy.access.pll_idx = 1;
    PHYMOD_IF_ERR_RETURN
        (tefmod_gen3_configure_pll(&phy_access_copy.access, init_config->pll1_div_init_value, init_config->interface.ref_clock));
    /* Get Core config variable VcoRate */
    PHYMOD_IF_ERR_RETURN
        (tefmod_gen3_pll_to_vco_rate(&phy_access_copy.access, init_config->pll1_div_init_value, init_config->interface.ref_clock, &vco_rate));
    firmware_core_config_tmp.VcoRate = MHZ_TO_VCO_RATE(vco_rate);
    /* Configure Core level RAM variables */
    PHYMOD_IF_ERR_RETURN
        (tscf_gen3_phy_firmware_core_config_set(&phy_access_copy, firmware_core_config_tmp));

    PHYMOD_IF_ERR_RETURN
        (tefmod_gen3_cl74_chng_default (&core_copy.access));

    PHYMOD_IF_ERR_RETURN
        (tefmod_gen3_cl91_ecc_clear(&core_copy.access));

    /* Release core DP soft reset */
    if (init_config->pll0_div_init_value){
        core_copy.access.pll_idx = 0;
        PHYMOD_IF_ERR_RETURN
            (falcon2_monterey_core_soft_reset_release(&core_copy.access, 1));
    }

    core_copy.access.pll_idx = 1;
    PHYMOD_IF_ERR_RETURN
        (falcon2_monterey_core_soft_reset_release(&core_copy.access, 1));

   return PHYMOD_E_NONE;
}


int tscf_gen3_core_init(const phymod_core_access_t* core, const phymod_core_init_config_t* init_config, const phymod_core_status_t* core_status)
{

    if ((!PHYMOD_CORE_INIT_F_EXECUTE_PASS1_GET(init_config) &&
         !PHYMOD_CORE_INIT_F_EXECUTE_PASS2_GET(init_config)) ||
          PHYMOD_CORE_INIT_F_EXECUTE_PASS1_GET(init_config)) {
        PHYMOD_IF_ERR_RETURN
            (_tscf_gen3_core_init_pass1(core, init_config, core_status));

        if (PHYMOD_CORE_INIT_F_EXECUTE_PASS1_GET(init_config)) {
            return PHYMOD_E_NONE;
        }
    }

    if ( (!PHYMOD_CORE_INIT_F_EXECUTE_PASS1_GET(init_config) &&
          !PHYMOD_CORE_INIT_F_EXECUTE_PASS2_GET(init_config)) ||
          PHYMOD_CORE_INIT_F_EXECUTE_PASS2_GET(init_config)) {
        PHYMOD_IF_ERR_RETURN
            (_tscf_gen3_core_init_pass2(core, init_config, core_status));
    }

    return PHYMOD_E_NONE;
}


int tscf_gen3_phy_pll_multiplier_get(const phymod_phy_access_t* phy, uint32_t* pll_multiplier)
{
    phymod_access_t pm_acc_copy;
    int pll_mode;
    int pll_index=0;

    PHYMOD_MEMCPY(&pm_acc_copy, &phy->access, sizeof(pm_acc_copy));

    /* first get the  PLL mode */

    /*
     *  If PLL_INDEX_BY_LANE is set, get pll_idx from lane.
     *  Otherwise, use pll_idx from phy_access.
     */
    if (PHYMOD_ACC_F_PLL_INDEX_BY_LANE_GET(&pm_acc_copy)) {
        PHYMOD_IF_ERR_RETURN
            (tefmod_gen3_pll_select_get(&pm_acc_copy, &pll_index));

        pm_acc_copy.pll_idx = pll_index;
    }

    PHYMOD_IF_ERR_RETURN
        (tefmod_gen3_pll_mode_get(&pm_acc_copy, &pll_mode));

    /*next get the real pll divider value based on the PLL mode */
    PHYMOD_IF_ERR_RETURN
        (tefmod_gen3_pll_multiplier_get(pll_mode, pll_multiplier));

    return PHYMOD_E_NONE;
}

int tscf_gen3_phy_init(const phymod_phy_access_t* phy, const phymod_phy_init_config_t* init_config)
{
    const phymod_access_t *pm_acc = &phy->access;
    phymod_phy_access_t pm_phy_copy;
    int start_lane, num_lane, i;
    /*int lane_bkup; */
    phymod_polarity_t tmp_pol;
    phymod_firmware_lane_config_t firmware_lane_config;

    PHYMOD_MEMSET(&tmp_pol, 0x0, sizeof(tmp_pol));
    PHYMOD_MEMCPY(&pm_phy_copy, phy, sizeof(pm_phy_copy));
    PHYMOD_MEMSET(&firmware_lane_config, 0x0, sizeof(firmware_lane_config));

    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(pm_acc, &start_lane, &num_lane));

    /* De-assert PMD lane reset  */
    PHYMOD_IF_ERR_RETURN
        (tefmod_gen3_pmd_x4_reset(pm_acc));
#if 0
    lane_bkup = pm_phy_copy.access.lane_mask;
    for (i = 0; i < num_lane; i++) {
        pm_phy_copy.access.lane_mask = 1 << (start_lane + i);
        PHYMOD_IF_ERR_RETURN
            (falcon2_monterey_lane_soft_reset_release(&pm_phy_copy.access, 1));
    }
    pm_phy_copy.access.lane_mask = lane_bkup;
#endif
    /* Clear all the lane config */
    PHYMOD_MEMSET(&firmware_lane_config, 0x0, sizeof(firmware_lane_config));

    /* Program the rx/tx polarity */
    for (i = 0; i < num_lane; i++) {
        pm_phy_copy.access.lane_mask = 0x1 << (i + start_lane);
        tmp_pol.tx_polarity = (init_config->polarity.tx_polarity) >> i & 0x1;
        tmp_pol.rx_polarity = (init_config->polarity.rx_polarity) >> i & 0x1;
        PHYMOD_IF_ERR_RETURN
            (tscf_gen3_phy_polarity_set(&pm_phy_copy, &tmp_pol));
    }
    for (i = 0; i < num_lane; i++) {
        pm_phy_copy.access.lane_mask = 0x1 << (i + start_lane);
        /* set tx parameters */
        PHYMOD_IF_ERR_RETURN
            (tscf_gen3_phy_tx_set(&pm_phy_copy, &init_config->tx[i]));
    }

    for (i = 0; i < num_lane; i++) {
        pm_phy_copy.access.lane_mask = 0x1 << (i + start_lane);
        PHYMOD_IF_ERR_RETURN
             (_tscf_gen3_phy_firmware_lane_config_set(&pm_phy_copy, firmware_lane_config));
    }
#if 0
    /* Next check if pcs-bypass mode  */
    if (PHYMOD_DEVICE_OP_MODE_PCS_BYPASS_GET(phy->device_op_mode)) {
        PHYMOD_IF_ERR_RETURN
            (falcon2_monterey_pmd_tx_disable_pin_dis_set(&phy->access, 1));
        PHYMOD_IF_ERR_RETURN
          (tefmod_gen3_init_pcs_ilkn(&phy->access));
    }
#endif

    PHYMOD_IF_ERR_RETURN
        (tefmod_gen3_port_mode_select(pm_acc));

    PHYMOD_IF_ERR_RETURN
        (tefmod_gen3_rx_lane_control_set(pm_acc, 1));
    PHYMOD_IF_ERR_RETURN
        (tefmod_gen3_tx_lane_control_set(pm_acc, TEFMOD_TX_LANE_RESET_TRAFFIC_ENABLE));

    PHYMOD_IF_ERR_RETURN
        (tefmod_gen3_25g_rsfec_am_init(pm_acc));

   /* Enable FEC error bypass indication */
    PHYMOD_IF_ERR_RETURN
        (tefmod_gen3_fec_bypass_indication_set(pm_acc, 1));

    return PHYMOD_E_NONE;
}


int tscf_gen3_phy_loopback_set(const phymod_phy_access_t* phy, phymod_loopback_mode_t loopback, uint32_t enable)
{
    int i;
    int start_lane, num_lane;
    uint32_t cl72_en = 0; 
    phymod_phy_access_t phy_copy;

    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));

    /* Next figure out the lane num and start_lane based on the input */
    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(&phy->access, &start_lane, &num_lane));

    switch (loopback) {
    case phymodLoopbackGlobal :
        PHYMOD_IF_ERR_RETURN
            (tscf_gen3_phy_cl72_get(phy, &cl72_en));
        if (cl72_en == 1) {
             PHYMOD_DEBUG_ERROR(("adr=%0x,lane 0x%x: Error! pcs gloop not supported with cl72 enabled\n",  phy_copy.access.addr, start_lane));
             break;
        }
        PHYMOD_IF_ERR_RETURN(tefmod_gen3_tx_loopback_control(&phy->access, enable, start_lane, num_lane));
        break;

       case phymodLoopbackGlobalPMD :
        PHYMOD_IF_ERR_RETURN
            (tscf_gen3_phy_cl72_get(phy, &cl72_en));
        if (cl72_en == 1) {
             PHYMOD_DEBUG_ERROR(("adr=%0x,lane 0x%x: Error! pmd gloop not supported with cl72 enabled\n",  phy_copy.access.addr, start_lane));
             break;
        }
            for (i = 0; i < num_lane; i++) {
            phy_copy.access.lane_mask = 0x1 << (i + start_lane);
            PHYMOD_IF_ERR_RETURN(falcon2_monterey_dig_lpbk(&phy_copy.access, (uint8_t) enable));
            PHYMOD_IF_ERR_RETURN(falcon2_monterey_pmd_force_signal_detect(&phy_copy.access, (int) enable));
            PHYMOD_IF_ERR_RETURN(tefmod_gen3_rx_lane_control_set(&phy->access, 1));
        }
        break;
    case phymodLoopbackRemotePMD :
        PHYMOD_IF_ERR_RETURN(falcon2_monterey_rmt_lpbk(&phy->access, (uint8_t)enable));
        break;

    case phymodLoopbackRemotePCS :
        PHYMOD_RETURN_WITH_ERR(PHYMOD_E_CONFIG,
                               (_PHYMOD_MSG("PCS Remote LoopBack not supported")));
        break;
    default :
        break;
    }

    return PHYMOD_E_NONE;
}

int tscf_gen3_phy_loopback_get(const phymod_phy_access_t* phy, phymod_loopback_mode_t loopback, uint32_t* enable)
{        
    uint32_t enable_core;
    int start_lane, num_lane;

    *enable = 0;

    /* next figure out the lane num and start_lane based on the input */
    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(&phy->access, &start_lane, &num_lane));

    switch (loopback) {
    case phymodLoopbackGlobal :
        PHYMOD_IF_ERR_RETURN(tefmod_gen3_tx_loopback_get(&phy->access, &enable_core));
        *enable = (enable_core >> start_lane) & 0x1;
        break;

    case phymodLoopbackGlobalPMD :
        PHYMOD_IF_ERR_RETURN(falcon2_monterey_tsc_dig_lpbk_get(&phy->access, enable));
        break;
    case phymodLoopbackRemotePMD :
        PHYMOD_IF_ERR_RETURN(falcon2_monterey_tsc_rmt_lpbk_get(&phy->access, enable));
        break;

    case phymodLoopbackRemotePCS :
        PHYMOD_RETURN_WITH_ERR(PHYMOD_E_CONFIG,
                               (_PHYMOD_MSG("PCS Remote LoopBack not supported")));
        break;
    default :
        break;
    }
 
    return PHYMOD_E_NONE;
}


int tscf_gen3_phy_rx_pmd_locked_get(const phymod_phy_access_t* phy, uint32_t* rx_pmd_locked)
{        
    PHYMOD_IF_ERR_RETURN(tefmod_gen3_pmd_lock_get(&phy->access, rx_pmd_locked));  
        
    return PHYMOD_E_NONE;
}

int tscf_gen3_phy_rx_signal_detect_get(const phymod_phy_access_t* phy, uint32_t* value)
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
            (falcon2_monterey_tsc_signal_detect(&phy->access, &local_rx_signal_det));
        *value = *value & local_rx_signal_det;
    }
    
    return PHYMOD_E_NONE;
}

int tscf_gen3_phy_link_status_get(const phymod_phy_access_t* phy, uint32_t* link_status)
{        
    PHYMOD_IF_ERR_RETURN(tefmod_gen3_get_pcs_latched_link_status(&phy->access, link_status));
        
    return PHYMOD_E_NONE;
}


int tscf_gen3_phy_pcs_userspeed_set(const phymod_phy_access_t* phy, const phymod_pcs_userspeed_config_t* config)
{        
    return PHYMOD_E_UNAVAIL;
}

int tscf_gen3_phy_pcs_userspeed_get(const phymod_phy_access_t* phy, phymod_pcs_userspeed_config_t* config)
{        
    return PHYMOD_E_UNAVAIL;
}


int tscf_gen3_phy_reg_read(const phymod_phy_access_t* phy, uint32_t reg_addr, uint32_t* val)
{        
    phymod_phy_access_t phy_copy; 

    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));
    PHYMOD_IF_ERR_RETURN(phymod_tsc_iblk_read(&phy->access, reg_addr, val));

    return PHYMOD_E_NONE;
}


int tscf_gen3_phy_reg_write(const phymod_phy_access_t* phy, uint32_t reg_addr, uint32_t val)
{        
    phymod_phy_access_t phy_copy;

    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));
    PHYMOD_IF_ERR_RETURN(phymod_tsc_iblk_write(&phy->access, reg_addr, val));

    return PHYMOD_E_NONE;
}


int tscf_gen3_phy_eye_margin_est_get(const phymod_phy_access_t* phy, phymod_eye_margin_mode_t eye_margin_mode, uint32_t* value)
{        
    
    return PHYMOD_E_NONE;
    
}


int tscf_gen3_phy_rescal_set(const phymod_phy_access_t* phy, uint32_t enable, uint32_t value)
{        
    PHYMOD_IF_ERR_RETURN(falcon2_monterey_rescal_val_set(&phy->access, enable, value));
        
    return PHYMOD_E_NONE;
}

int tscf_gen3_phy_rescal_get(const phymod_phy_access_t* phy, uint32_t* value)
{        
    PHYMOD_IF_ERR_RETURN(falcon2_monterey_rescal_val_get(&phy->access, value)); 

    return PHYMOD_E_NONE;
}

/* Enable 1588 timestamping of packets within the TXP and RXP pipelines */
int tscf_gen3_timesync_enable_set(const phymod_phy_access_t* phy, uint32_t flags, uint32_t enable)
{
    phymod_phy_access_t phy_copy;
    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));

    PHYMOD_IF_ERR_RETURN
        (tefmod_gen3_1588_ts_enable_set(&phy_copy.access, enable));    

    return PHYMOD_E_NONE;

}

int tscf_gen3_timesync_enable_get(const phymod_phy_access_t* phy, uint32_t flags, uint32_t* enable)
{
    phymod_phy_access_t phy_copy;
    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));

    PHYMOD_IF_ERR_RETURN
        (tefmod_gen3_1588_ts_enable_get(&phy_copy.access, enable));

    return PHYMOD_E_NONE;
}

/* Set timesync offset */
int tscf_gen3_timesync_offset_set(const phymod_core_access_t* core, uint32_t ts_offset)
{
    phymod_core_access_t core_copy;
    PHYMOD_MEMCPY(&core_copy, core, sizeof(core_copy));

    PHYMOD_IF_ERR_RETURN
        (tefmod_gen3_1588_ts_offset_set(&core_copy.access, ts_offset));


    return PHYMOD_E_NONE;
}


int tscf_gen3_timesync_adjust_set(const phymod_phy_access_t* phy, uint32_t flags, uint32_t ts_am_norm_mode)
{
    phymod_phy_access_t phy_copy;
    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy)); 

    /* Set sfd_ts_en, crc_calc_mode, cl36_sop_adjust*/
    PHYMOD_IF_ERR_RETURN
        (tefmod_gen3_1588_pcs_control_set(&phy_copy.access));

    /* Set 1588 FCLK period */
    PHYMOD_IF_ERR_RETURN
        (tefmod_gen3_1588_fclk_set(&phy_copy.access));

    /* Set ui values */
    PHYMOD_IF_ERR_RETURN
        (tefmod_gen3_1588_ui_values_set(&phy_copy.access));

    /*Set RX/TX pmd fixed latency */
    PHYMOD_IF_ERR_RETURN
        (tefmod_gen3_1588_txrx_fixed_latency_set(&phy_copy.access));

    /* Program deskew adjustment for multi-lane ports */
    if (ts_am_norm_mode != phymodTimesyncCompensationModeNone) {
        PHYMOD_IF_ERR_RETURN
            (tefmod_gen3_1588_rx_deskew_set(&phy_copy.access, ts_am_norm_mode));
    }

    return PHYMOD_E_NONE;
}

int tscf_gen3_timesync_tx_info_get(const phymod_phy_access_t* phy, phymod_ts_fifo_status_t* ts_tx_info)
{
    phymod_phy_access_t phy_copy;
    tefmod_gen3_ts_tx_info_t local_ts_tx_info;

    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));
    PHYMOD_MEMSET(&local_ts_tx_info, 0, sizeof(tefmod_gen3_ts_tx_info_t));

    PHYMOD_IF_ERR_RETURN(tefmod_gen3_1588_tx_info_get(&phy_copy.access, &local_ts_tx_info));

    ts_tx_info->ts_in_fifo_lo = ((uint32_t)(local_ts_tx_info.ts_val_hi << 16)) | ((uint32_t)local_ts_tx_info.ts_val_lo);
    ts_tx_info->ts_in_fifo_hi = 0;
    ts_tx_info->ts_seq_id = (uint32_t)local_ts_tx_info.ts_seq_id;
    ts_tx_info->ts_sub_nanosec = (uint32_t)local_ts_tx_info.ts_sub_nanosec;

    return PHYMOD_E_NONE;
}

int tscf_gen3_phy_rx_ppm_get(const phymod_phy_access_t* phy, int16_t* rx_ppm)
{
    int start_lane, num_lane;
    phymod_phy_access_t pm_phy_copy;

    PHYMOD_MEMCPY(&pm_phy_copy, phy, sizeof(pm_phy_copy));

    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(&phy->access, &start_lane, &num_lane));

    pm_phy_copy.access.lane_mask = 1 << start_lane;
    PHYMOD_IF_ERR_RETURN
        (falcon2_monterey_tsc_rx_ppm(&pm_phy_copy.access, rx_ppm));

    return PHYMOD_E_NONE;
}

int tscf_gen3_phy_synce_clk_ctrl_set(const phymod_phy_access_t* phy,
                                  phymod_synce_clk_ctrl_t cfg)
{
    phymod_phy_access_t phy_copy;
    int start_lane, num_lane;

    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(&phy->access, &start_lane, &num_lane));

    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));
    phy_copy.access.lane_mask = 0x1 << start_lane;

    PHYMOD_IF_ERR_RETURN
        (tefmod_gen3_synce_stg0_mode_set(&phy_copy.access, cfg.stg0_mode));

    PHYMOD_IF_ERR_RETURN
        (tefmod_gen3_synce_stg1_mode_set(&phy_copy.access, cfg.stg1_mode));

    PHYMOD_IF_ERR_RETURN
        (tefmod_gen3_synce_clk_ctrl_set(&phy_copy.access, cfg.sdm_val));

    return PHYMOD_E_NONE;
}

int tscf_gen3_phy_synce_clk_ctrl_get(const phymod_phy_access_t* phy,
                                  phymod_synce_clk_ctrl_t *cfg)
{
    PHYMOD_IF_ERR_RETURN
        (tefmod_gen3_synce_stg0_mode_get(&phy->access, &(cfg->stg0_mode)));

    PHYMOD_IF_ERR_RETURN
        (tefmod_gen3_synce_stg1_mode_get(&phy->access, &(cfg->stg1_mode)));

    PHYMOD_IF_ERR_RETURN
        (tefmod_gen3_synce_clk_ctrl_get(&phy->access, &(cfg->sdm_val)));

    return PHYMOD_E_NONE;
}

int tscf_gen3_phy_osr_mode_get(const phymod_phy_access_t* phy, phymod_osr_mode_t* osr_mode)
{
    int local_osr_mode;

    PHYMOD_IF_ERR_RETURN
        (falcon2_monterey_osr_mode_get(&phy->access, &local_osr_mode));

    switch (local_osr_mode) {
    case 0:
        *osr_mode = phymodOversampleMode1;
        break;
    case 1:
        *osr_mode = phymodOversampleMode2;
        break;
    default:
        *osr_mode = phymodOversampleMode1;
        break;
    }
    return PHYMOD_E_NONE;
}

int tscf_gen3_phy_ref_clk_get(const phymod_phy_access_t* phy, phymod_ref_clk_t* ref_clk)
{
    PHYMOD_IF_ERR_RETURN
        (tefmod_gen3_refclk_get(&phy->access, ref_clk));

    return PHYMOD_E_NONE;
}

int tscf_gen3_phy_fec_bypass_indication_set(const phymod_phy_access_t* phy,
                                            uint32_t enable)
{
    uint32_t lane_enable, pcs_lane_enable;

     /* check if PCS lane is in reset */
     PHYMOD_IF_ERR_RETURN
         (tefmod_gen3_disable_get(&phy->access, &pcs_lane_enable));

     /* disable pcs lane if it is not in reset */
     if (pcs_lane_enable) {
         PHYMOD_IF_ERR_RETURN
             (tefmod_gen3_disable_set(&phy->access));
     }

     /* check if lane is in reset */
     PHYMOD_IF_ERR_RETURN
         (falcon2_monterey_lane_soft_reset_release_get(&phy->access, &lane_enable));

     /* if PMD lane is not in reset, then reset the lane first */
     if (lane_enable) {
         PHYMOD_IF_ERR_RETURN
             (falcon2_monterey_lane_soft_reset_release(&phy->access, 0));
     }

    PHYMOD_IF_ERR_RETURN(
      tefmod_gen3_fec_bypass_indication_set(&phy->access, enable));

    /* release the lane soft reset bit */
    if (lane_enable) {
        PHYMOD_IF_ERR_RETURN
            (falcon2_monterey_lane_soft_reset_release(&phy->access, 1));
    }

    /* re-enable pcs lane if pcs lane not in reset */
    if (pcs_lane_enable) {
        PHYMOD_IF_ERR_RETURN
            (tefmod_gen3_enable_set(&phy->access));
    }

    return PHYMOD_E_NONE;
}

int tscf_gen3_phy_fec_bypass_indication_get(const phymod_phy_access_t* phy,
                                            uint32_t* enable)
{
    PHYMOD_IF_ERR_RETURN(
      tefmod_gen3_fec_bypass_indication_get(&phy->access, enable));

    return PHYMOD_E_NONE;
}

