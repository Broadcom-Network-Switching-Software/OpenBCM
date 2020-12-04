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
#include <phymod/phymod_dispatch.h>
#include <phymod/phymod_debug.h>
#include <phymod/phymod_util.h>
#include <phymod/chip/bcmi_qtce16_xgxs_defs.h>
#include <phymod/chip/merlin16.h>

#include "qtce16/tier1/qe16PCSRegEnums.h"
#include "qtce16/tier1/qmod16_enum_defines.h"
#include "qtce16/tier1/qmod16_device.h"
#include "qtce16/tier1/qmod16.h"
#include "qtce16/tier1/qmod16_defines.h"
#include "qtce16/tier1/qmod16_sc_lkup_table.h"
#include "qtce16/tier1/phy_tsc_iblk.h"
#include "merlin16/tier1/merlin16_cfg_seq.h"
#include "merlin16/tier1/merlin16_common.h" 
#include "merlin16/tier1/merlin16_interface.h" 
#include "merlin16/tier1/merlin16_dependencies.h" 
#include "merlin16/tier1/merlin16_internal.h"

#ifdef PHYMOD_QTCE16_SUPPORT

#define QTCE16_ID0        0x600d
#define QTCE16_ID1        0x8770
#define QTCE16_REV_MASK   0x0

#define QTCE16_MODEL     0x16

#define QTCE16_TX_E      0x10

#define QTCE16_NOF_DFES (5)
#define QTCE16_NOF_LANES_IN_CORE (4) 
#define QTCE16_LANE_SWAP_LANE_MASK (0x3)
#define QTCE16_PHY_ALL_LANES (0xf)
#define QTCE16_CORE_TO_PHY_ACCESS(_phy_access, _core_access) \
    do{\
        PHYMOD_MEMCPY(&(_phy_access)->access, &(_core_access)->access, sizeof((_phy_access)->access));\
        (_phy_access)->type = (_core_access)->type; \
        (_phy_access)->access.lane_mask = QTCE16_PHY_ALL_LANES; \
    }while(0)

extern unsigned char merlin16_ucode[];
extern unsigned short merlin16_ucode_ver;
extern unsigned short merlin16_ucode_crc;
extern unsigned short merlin16_ucode_len;

STATIC
int _qtce16_phy_firmware_lane_config_set(const phymod_phy_access_t* phy, phymod_firmware_lane_config_t fw_config)
{
    struct merlin16_uc_lane_config_st serdes_firmware_config;
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

        if (!is_warm_boot) {
            PHYMOD_IF_ERR_RETURN (merlin16_set_uc_lane_cfg(&phy_copy.access, serdes_firmware_config));
        }
    }
    return PHYMOD_E_NONE;
}

int qtce16_core_identify(const phymod_core_access_t* core, uint32_t core_id, uint32_t* is_identified)
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

    if (PHYID2r_GET(id2) == QTCE16_ID0 &&
        (PHYID3r_GET(id3) &= ~QTCE16_REV_MASK) == QTCE16_ID1) {
        /* PHY IDs match - now check PCS model */
        ioerr += READ_MAIN_SERDESIDr(pm_acc, &serdesid);
        model = MAIN_SERDESIDr_MODEL_NUMBERf_GET(serdesid);
        if (model == QTCE16_MODEL)  {
            *is_identified = 1;
        }
    }
    rv = ioerr ? PHYMOD_E_IO : PHYMOD_E_NONE;
    return rv ;    
}


int qtce16_core_info_get(const phymod_core_access_t* phy, phymod_core_info_t* info)
{        
    uint32_t serdes_id;
    PHYID2r_t id2;
    PHYID3r_t id3;
    const phymod_access_t *pm_acc = &phy->access;
    PHYMOD_IF_ERR_RETURN
        (qmod16_revid_read(&phy->access, &serdes_id));
    info->serdes_id = serdes_id;
    if ((serdes_id & 0x3f) == QTCE16_MODEL) {
        info->core_version = phymodCoreVersionQtce16;
        PHYMOD_STRNCPY(info->name, "Qtce16", PHYMOD_STRLEN("Qtce16")+1);  /* need to remove when the master rev*/
    }
    /* need to update with the master rev
    PHYMOD_IF_ERR_RETURN
        (phymod_core_name_get(phy, serdes_id, core_name, info));
    */

    PHYMOD_IF_ERR_RETURN(READ_PHYID2r(pm_acc, &id2));
    PHYMOD_IF_ERR_RETURN(READ_PHYID3r(pm_acc, &id3));

    info->phy_id0 = (uint16_t) id2.v[0];
    info->phy_id1 = (uint16_t) id3.v[0];
    return PHYMOD_E_NONE;
}

/* 
 * set lane swapping for core 
 * The tx swap is composed of tx PCS swap. 
 * The rx swap is composed of rx PCS swap
 *
 * lane_map_tx and lane_map_rx[lane=logic_lane] are logic-lane base.
 * pcs_swap and register is logic_lane base. 
 */
int qtce16_core_lane_map_set(const phymod_core_access_t* core, const phymod_lane_map_t* lane_map)
{        
    uint32_t pcs_rx_swap = 0 , pcs_tx_swap = 0, lane;
    uint8_t pmd_tx_lane_map[PHYMOD_MAX_LANES_PER_CORE];
    uint8_t pmd_rx_lane_map[PHYMOD_MAX_LANES_PER_CORE];
    uint8_t num_lanes = (uint8_t) lane_map->num_of_lanes;

    if(lane_map->num_of_lanes != QTCE16_NOF_LANES_IN_CORE){
        return PHYMOD_E_CONFIG;
    }
    for( lane = 0 ; lane < QTCE16_NOF_LANES_IN_CORE ; lane++){
        if(lane_map->lane_map_rx[lane] >= QTCE16_NOF_LANES_IN_CORE){
            return PHYMOD_E_CONFIG;
        }
        /* encode each lane as four bits */
        /* pcs_rx_map[lane] = rx_map[lane] */
        pcs_rx_swap += lane_map->lane_map_rx[lane]<<(lane*4);
    }

    for( lane = 0 ; lane < QTCE16_NOF_LANES_IN_CORE ; lane++){
        if(lane_map->lane_map_tx[lane] >= QTCE16_NOF_LANES_IN_CORE){
            return PHYMOD_E_CONFIG;
        }
        pcs_tx_swap += lane_map->lane_map_tx[lane]<<(lane*4);
    }

    for (lane = 0; lane < QTCE16_NOF_LANES_IN_CORE; lane++) {
        pmd_tx_lane_map[(int)lane_map->lane_map_tx[lane]] = lane;
        pmd_rx_lane_map[(int)lane_map->lane_map_rx[lane]] = lane;
    }


    PHYMOD_IF_ERR_RETURN(qmod16_pcs_lane_swap(&core->access, pcs_tx_swap, pcs_rx_swap));

    PHYMOD_IF_ERR_RETURN
        (merlin16_map_lanes(&core->access, num_lanes, pmd_tx_lane_map, pmd_rx_lane_map));

    return PHYMOD_E_NONE;
}

int qtce16_core_lane_map_get(const phymod_core_access_t* core, phymod_lane_map_t* lane_map)
{        
    uint32_t pcs_tx_swap = 0 , pcs_rx_swap = 0, lane; 
    PHYMOD_IF_ERR_RETURN(qmod16_pcs_lane_swap_get(&core->access, &pcs_tx_swap, &pcs_rx_swap));
    for( lane = 0 ; lane < QTCE16_NOF_LANES_IN_CORE ; lane++){
        /* deccode each lane from four bits */
        lane_map->lane_map_rx[lane] = (pcs_rx_swap>>(lane*4)) & QTCE16_LANE_SWAP_LANE_MASK;

        lane_map->lane_map_tx[lane] = (pcs_tx_swap>>(lane*4)) & QTCE16_LANE_SWAP_LANE_MASK;
    }
    lane_map->num_of_lanes = QTCE16_NOF_LANES_IN_CORE;
    return PHYMOD_E_NONE;
}


int qtce16_core_firmware_info_get(const phymod_core_access_t* core, phymod_core_firmware_info_t* fw_info)
{
    /* 
     * It's O.K to use this code as is since the firmware CRC is already checked at 
     * at the time we load it.
     */
    fw_info->fw_crc = merlin16_ucode_crc;
    fw_info->fw_version = merlin16_ucode_ver;
    return PHYMOD_E_NONE;
    
}
/* load qtce16 fw. the fw_loader parameter is valid just for external fw load */
STATIC
int _qtce16_core_firmware_load(const phymod_core_access_t* core, phymod_firmware_load_method_t load_method, phymod_firmware_loader_f fw_loader)
{


    switch(load_method){
    case phymodFirmwareLoadMethodInternal:
        PHYMOD_IF_ERR_RETURN(merlin16_ucode_mdio_load(&core->access, merlin16_ucode, merlin16_ucode_len));
        break;
    case phymodFirmwareLoadMethodExternal:
        PHYMOD_NULL_CHECK(fw_loader); 

        PHYMOD_IF_ERR_RETURN(merlin16_ucode_pram_load_pre(&core->access));
        PHYMOD_IF_ERR_RETURN(fw_loader(core, merlin16_ucode_len, merlin16_ucode));
        PHYMOD_IF_ERR_RETURN(merlin16_ucode_pram_load_post(&core->access)); 
        break;
    case phymodFirmwareLoadMethodNone:
        break;
    default:
        PHYMOD_RETURN_WITH_ERR(PHYMOD_E_CONFIG, (_PHYMOD_MSG("illegal fw load method %u"), load_method));
    }
    return PHYMOD_E_NONE;
}


int qtce16_phy_firmware_core_config_set(const phymod_phy_access_t* phy, phymod_firmware_core_config_t fw_config)
{
    struct merlin16_uc_core_config_st serdes_firmware_core_config;
    PHYMOD_MEMSET(&serdes_firmware_core_config, 0, sizeof(serdes_firmware_core_config));
    serdes_firmware_core_config.field.core_cfg_from_pcs = fw_config.CoreConfigFromPCS;
    serdes_firmware_core_config.field.vco_rate = fw_config.VcoRate;
 
    PHYMOD_IF_ERR_RETURN(merlin16_INTERNAL_set_uc_core_config(&phy->access, serdes_firmware_core_config));
    return PHYMOD_E_NONE;
}

int qtce16_phy_firmware_core_config_get(const phymod_phy_access_t* phy, phymod_firmware_core_config_t* fw_config)
{
    struct merlin16_uc_core_config_st serdes_firmware_core_config;
    PHYMOD_IF_ERR_RETURN(merlin16_get_uc_core_config(&phy->access, &serdes_firmware_core_config));
    PHYMOD_MEMSET(fw_config, 0, sizeof(*fw_config));
    fw_config->CoreConfigFromPCS = serdes_firmware_core_config.field.core_cfg_from_pcs;
    fw_config->VcoRate = serdes_firmware_core_config.field.vco_rate;
    return PHYMOD_E_NONE;
}


int qtce16_phy_firmware_lane_config_set(const phymod_phy_access_t* phy, phymod_firmware_lane_config_t fw_lane_config)
{
    phymod_phy_access_t phy_copy;
    int start_lane, num_lane, i, lane_id, sub_port;

    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(&phy->access, &start_lane, &num_lane));
    PHYMOD_IF_ERR_RETURN
        (qmod16_lane_info(&phy->access, &lane_id, &sub_port));
    start_lane = lane_id;
    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));

    /*Hold the per lne soft reset bit*/
    for (i = 0; i < num_lane; i++) {
        phy_copy.access.lane_mask = 1 << (start_lane + i);
        PHYMOD_IF_ERR_RETURN
            (merlin16_lane_soft_reset_release(&phy_copy.access, 0));
    }

    for (i = 0; i < num_lane; i++) {
        phy_copy.access.lane_mask = 1 << (start_lane + i);
    PHYMOD_IF_ERR_RETURN
        (_qtce16_phy_firmware_lane_config_set(&phy_copy, fw_lane_config));
    }
    /*Hold the per lne soft reset bit*/
    for (i = 0; i < num_lane; i++) {
        phy_copy.access.lane_mask = 1 << (start_lane + i);
        PHYMOD_IF_ERR_RETURN
            (merlin16_lane_soft_reset_release(&phy_copy.access, 1));
    }

    /* speed trigger here */
    phy_copy.access.lane_mask = 1 << start_lane;
    PHYMOD_IF_ERR_RETURN
        (qmod16_trigger_speed_change(&phy_copy.access));
    return PHYMOD_E_NONE;
}

int qtce16_phy_firmware_lane_config_get(const phymod_phy_access_t* phy, phymod_firmware_lane_config_t* fw_config)
{        
    
    struct merlin16_uc_lane_config_st serdes_firmware_config;
    phymod_phy_access_t phy_copy;
    int lane_id, sub_port;

    PHYMOD_IF_ERR_RETURN
        (qmod16_lane_info(&phy->access, &lane_id, &sub_port));
    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));
    phy_copy.access.lane_mask = 1 << lane_id;

    PHYMOD_IF_ERR_RETURN(merlin16_get_uc_lane_cfg(&phy_copy.access, &serdes_firmware_config));
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

int qtce16_phy_polarity_set(const phymod_phy_access_t* phy, const phymod_polarity_t* polarity)
{        
    phymod_phy_access_t phy_copy;
    int lane_id, sub_port;

    PHYMOD_IF_ERR_RETURN
        (qmod16_lane_info(&phy->access, &lane_id, &sub_port));
    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));
    phy_copy.access.lane_mask = 1 << lane_id;

    PHYMOD_IF_ERR_RETURN
        (qmod16_tx_rx_polarity_set(&phy_copy.access, polarity->tx_polarity, polarity->rx_polarity));

    return PHYMOD_E_NONE;
    
}

int qtce16_phy_polarity_get(const phymod_phy_access_t* phy, phymod_polarity_t* polarity)
{        
    phymod_phy_access_t phy_copy;
    int lane_id, sub_port;

    PHYMOD_IF_ERR_RETURN
        (qmod16_lane_info(&phy->access, &lane_id, &sub_port));
    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));
    phy_copy.access.lane_mask = 1 << lane_id;

    PHYMOD_IF_ERR_RETURN
        (qmod16_tx_rx_polarity_get(&phy_copy.access, &polarity->tx_polarity, &polarity->rx_polarity));
    
    return PHYMOD_E_NONE;    
}


int qtce16_phy_tx_set(const phymod_phy_access_t* phy, const phymod_tx_t* tx)
{        
    phymod_phy_access_t phy_copy;
    int lane_id, sub_port;

    PHYMOD_IF_ERR_RETURN
        (qmod16_lane_info(&phy->access, &lane_id, &sub_port));
    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));
    phy_copy.access.lane_mask = 1 << lane_id;

    PHYMOD_IF_ERR_RETURN
        (merlin16_apply_txfir_cfg(&phy_copy.access, (int8_t)tx->pre, (int8_t)tx->main, (int8_t)tx->post, (int8_t)tx->post2));
    return PHYMOD_E_NONE;    
}

int qtce16_phy_tx_get(const phymod_phy_access_t* phy, phymod_tx_t* tx)
{
    phymod_phy_access_t phy_copy;
    int lane_id, sub_port;
    int8_t value = 0;

    PHYMOD_IF_ERR_RETURN
        (qmod16_lane_info(&phy->access, &lane_id, &sub_port));
    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));
    phy_copy.access.lane_mask = 1 << lane_id;

    PHYMOD_IF_ERR_RETURN
        (merlin16_read_tx_afe(&phy_copy.access, TX_AFE_PRE, &value));
    tx->pre = value;
    PHYMOD_IF_ERR_RETURN
        (merlin16_read_tx_afe(&phy_copy.access, TX_AFE_MAIN, &value));
    tx->main = value;
    PHYMOD_IF_ERR_RETURN
        (merlin16_read_tx_afe(&phy_copy.access, TX_AFE_POST1, &value));
    tx->post = value;
    PHYMOD_IF_ERR_RETURN
        (merlin16_read_tx_afe(&phy_copy.access, TX_AFE_POST2, &value));
    tx->post2 = value;

    return PHYMOD_E_NONE;
}

int qtce16_phy_cl72_set(const phymod_phy_access_t* phy, uint32_t cl72_en)
{
    struct merlin16_uc_lane_config_st serdes_firmware_config;
    phymod_phy_access_t phy_copy;
    int lane_id, sub_port;

    PHYMOD_IF_ERR_RETURN
        (qmod16_lane_info(&phy->access, &lane_id, &sub_port));
    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));
    phy_copy.access.lane_mask = 1 << lane_id;

    PHYMOD_IF_ERR_RETURN(merlin16_get_uc_lane_cfg(&phy_copy.access, &serdes_firmware_config));

    if (serdes_firmware_config.field.dfe_on == 0) {
      PHYMOD_DEBUG_ERROR(("ERROR :: DFE is off : Can not start CL72 with no DFE\n"));
      return PHYMOD_E_CONFIG;
    }

    PHYMOD_IF_ERR_RETURN
        (qmod16_clause72_control(&phy->access, cl72_en));

    return PHYMOD_E_NONE;
}

int qtce16_phy_cl72_get(const phymod_phy_access_t* phy, uint32_t* cl72_en)
{
    phymod_phy_access_t phy_copy;
    int lane_id, sub_port;

    PHYMOD_IF_ERR_RETURN
        (qmod16_lane_info(&phy->access, &lane_id, &sub_port));
    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));
    phy_copy.access.lane_mask = 1 << lane_id;

    PHYMOD_IF_ERR_RETURN
        (merlin16_pmd_cl72_enable_get(&phy_copy.access, cl72_en));

    return PHYMOD_E_NONE;
}

int qtce16_phy_cl72_status_get(const phymod_phy_access_t* phy, phymod_cl72_status_t* status)
{
    uint32_t local_status;
    phymod_phy_access_t phy_copy;
    int lane_id, sub_port;

    PHYMOD_IF_ERR_RETURN
        (qmod16_lane_info(&phy->access, &lane_id, &sub_port));
    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));
    phy_copy.access.lane_mask = 1 << lane_id;

    PHYMOD_IF_ERR_RETURN
        (merlin16_pmd_cl72_receiver_status(&phy_copy.access, &local_status));
    status->locked = local_status;

    return PHYMOD_E_NONE;
}

int qtce16_phy_tx_lane_control_set(const phymod_phy_access_t* phy, phymod_phy_tx_lane_control_t tx_control)
{        
   phymod_phy_access_t pm_phy_copy;
    int start_lane, num_lane, lane_id, sub_port;
    int qmode = 0, subports = 0, sq_value = 0;

    PHYMOD_MEMCPY(&pm_phy_copy, phy, sizeof(pm_phy_copy));
    /* next program the tx fir taps and driver current based on the input */
    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(&phy->access, &start_lane, &num_lane));

    PHYMOD_IF_ERR_RETURN
        (qmod16_lane_info(&phy->access, &lane_id, &sub_port));

    start_lane = lane_id ;  
    pm_phy_copy.access.lane_mask = 0x1 << start_lane;

    if (PHYMOD_ACC_F_QMODE_GET(&phy->access) || PHYMOD_ACC_F_USXMODE_GET(&phy->access)) {
        qmode = 1;
        PHYMOD_IF_ERR_RETURN(qmod16_port_state_get(&pm_phy_copy.access, QMOD16_PORT_STATE_CONFIGED, &subports));
    }

    switch (tx_control) {
    case phymodTxTrafficDisable:
        PHYMOD_IF_ERR_RETURN(qmod16_tx_lane_control_set(&pm_phy_copy.access, QMOD16_TX_LANE_TRAFFIC_DISABLE));
        break;
    case phymodTxTrafficEnable:
        PHYMOD_IF_ERR_RETURN(qmod16_tx_lane_control_set(&pm_phy_copy.access, QMOD16_TX_LANE_TRAFFIC_ENABLE));
        break;
    case phymodTxReset:
        PHYMOD_IF_ERR_RETURN(qmod16_tx_lane_control_set(&pm_phy_copy.access, QMOD16_TX_LANE_RESET));
        break;
    case phymodTxSquelchOn:
        PHYMOD_IF_ERR_RETURN(qmod16_port_state_set(&pm_phy_copy.access, QMOD16_PORT_STATE_TX_SQUELCH, sub_port, 1));
        PHYMOD_IF_ERR_RETURN(qmod16_port_state_get(&pm_phy_copy.access, QMOD16_PORT_STATE_TX_SQUELCH, &sq_value));
        if ((qmode == 0) || (sq_value == subports)) { 
            /* not qmode or all subports want to squelch*/
            PHYMOD_IF_ERR_RETURN(qmod16_tx_squelch_set(&pm_phy_copy.access, 1));
        }
        break;
    case phymodTxSquelchOff:
        PHYMOD_IF_ERR_RETURN(qmod16_port_state_set(&pm_phy_copy.access, QMOD16_PORT_STATE_TX_SQUELCH, sub_port, 0));
        PHYMOD_IF_ERR_RETURN(qmod16_tx_squelch_set(&pm_phy_copy.access, 0));
        break;
    default:
        break;
    }     
    return PHYMOD_E_NONE;
    
}

int qtce16_phy_tx_lane_control_get(const phymod_phy_access_t* phy, phymod_phy_tx_lane_control_t* tx_control)
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
        (qmod16_lane_info(&phy->access, &lane_id, &sub_port));
    start_lane = lane_id ;
    pm_phy_copy.access.lane_mask = 0x1 << start_lane;

    PHYMOD_IF_ERR_RETURN(qmod16_port_state_get(&pm_phy_copy.access, QMOD16_PORT_STATE_TX_SQUELCH, &sq_value));
    enable = sq_value & (1 << sub_port);

    /* next check if PMD loopback is on */ 
    if (enable) {                           
        PHYMOD_IF_ERR_RETURN(merlin16_pmd_loopback_get(&pm_phy_copy.access, &lb_enable));
        if (lb_enable) enable = 0;
    }

    if(enable) {
        *tx_control = phymodTxSquelchOn;
    } else {
        PHYMOD_IF_ERR_RETURN(qmod16_tx_lane_control_get(&pm_phy_copy.access, &reset, &tx_lane));
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


int qtce16_phy_rx_lane_control_set(const phymod_phy_access_t* phy, phymod_phy_rx_lane_control_t rx_control)
{
    phymod_phy_access_t pm_phy_copy;
    int start_lane, num_lane, lane_id, sub_port;
    int qmode = 0, subports = 0, sq_value = 0;

    PHYMOD_MEMCPY(&pm_phy_copy, phy, sizeof(pm_phy_copy));
    /* next program the tx fir taps and driver current based on the input */
    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(&phy->access, &start_lane, &num_lane));

    PHYMOD_IF_ERR_RETURN
        (qmod16_lane_info(&phy->access, &lane_id, &sub_port));

    start_lane = lane_id ;  
    pm_phy_copy.access.lane_mask = 0x1 << start_lane;

    if (PHYMOD_ACC_F_QMODE_GET(&phy->access) || PHYMOD_ACC_F_USXMODE_GET(&phy->access)) {
        qmode = 1;
        PHYMOD_IF_ERR_RETURN(qmod16_port_state_get(&pm_phy_copy.access, QMOD16_PORT_STATE_CONFIGED, &subports));
    }
    switch (rx_control) {
    case phymodRxReset:
        PHYMOD_IF_ERR_RETURN(qmod16_rx_lane_control_set(&pm_phy_copy.access, 1));
        break;
    case phymodRxSquelchOn:
        PHYMOD_IF_ERR_RETURN(qmod16_port_state_set(&pm_phy_copy.access, QMOD16_PORT_STATE_RX_SQUELCH, sub_port, 1));
        PHYMOD_IF_ERR_RETURN(qmod16_port_state_get(&pm_phy_copy.access, QMOD16_PORT_STATE_RX_SQUELCH, &sq_value));
        if ((qmode == 0) || (sq_value == subports)) {  
            /* not qmode or all subports want to squelch */
            PHYMOD_IF_ERR_RETURN(qmod16_rx_squelch_set(&pm_phy_copy.access, 1));    
        }
        break;
    case phymodRxSquelchOff:
        PHYMOD_IF_ERR_RETURN(qmod16_port_state_set(&pm_phy_copy.access, QMOD16_PORT_STATE_RX_SQUELCH, sub_port, 0));
        PHYMOD_IF_ERR_RETURN(qmod16_rx_squelch_set(&pm_phy_copy.access, 0));
        break;
    default:
        break;
    }   
    return PHYMOD_E_NONE;
    
}

int qtce16_phy_rx_lane_control_get(const phymod_phy_access_t* phy, phymod_phy_rx_lane_control_t* rx_control)
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
        (qmod16_lane_info(&phy->access, &lane_id, &sub_port));

    start_lane = lane_id ;
    pm_phy_copy.access.lane_mask = 0x1 << start_lane;

    PHYMOD_IF_ERR_RETURN(qmod16_port_state_get(&pm_phy_copy.access, QMOD16_PORT_STATE_RX_SQUELCH, &sq_value));
    enable = sq_value & (1 << sub_port);

    /* next check if PMD loopback is on */ 
    if (enable) {                           
        PHYMOD_IF_ERR_RETURN(merlin16_pmd_loopback_get(&pm_phy_copy.access, &lb_enable));
        if (lb_enable) enable = 0;
    }
    if(enable) {
        *rx_control = phymodRxSquelchOn;
    } else {
        PHYMOD_IF_ERR_RETURN(qmod16_rx_lane_control_get(&pm_phy_copy.access, &reset));
        if (reset == 0) {
            *rx_control = phymodRxReset;
        } else {
            *rx_control = phymodRxSquelchOff;
        }
    }   
    return PHYMOD_E_NONE;    
}

int qtce16_phy_power_set(const phymod_phy_access_t* phy, const phymod_phy_power_t* power)
{        
     phymod_phy_access_t pm_phy_copy;
    int start_lane, num_lane, lane_id, sub_port;
    
    PHYMOD_MEMCPY(&pm_phy_copy, phy, sizeof(pm_phy_copy));
    /* next program the tx fir taps and driver current based on the input */
    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(&phy->access, &start_lane, &num_lane));

    PHYMOD_IF_ERR_RETURN
        (qmod16_lane_info(&phy->access, &lane_id, &sub_port));

    start_lane = lane_id ;
    pm_phy_copy.access.lane_mask = 0x1 << start_lane;


    if ((power->tx == phymodPowerOff) && (power->rx == phymodPowerOff)) {        
        PHYMOD_IF_ERR_RETURN(qmod16_port_enable_set(&pm_phy_copy.access, 0));
    }
    if ((power->tx == phymodPowerOn) && (power->rx == phymodPowerOn)) {
        PHYMOD_IF_ERR_RETURN(qmod16_port_enable_set(&pm_phy_copy.access, 1));
    }

    if ((power->tx == phymodPowerOff) && (power->rx == phymodPowerNoChange)) {
        /* disable tx on the PMD side */
        PHYMOD_IF_ERR_RETURN(merlin16_tx_disable(&pm_phy_copy.access, 1));
    }
    if ((power->tx == phymodPowerOn) && (power->rx == phymodPowerNoChange)) {
        /* enable tx on the PMD side */
        PHYMOD_IF_ERR_RETURN(merlin16_tx_disable(&pm_phy_copy.access, 0));
    }
    if ((power->tx == phymodPowerNoChange) && (power->rx == phymodPowerOff)) {
        /* disable rx on the PMD side */
        PHYMOD_IF_ERR_RETURN(qmod16_rx_squelch_set(&pm_phy_copy.access, 1));
    }
    if ((power->tx == phymodPowerNoChange) && (power->rx == phymodPowerOn)) {
        /* enable rx on the PMD side */
        PHYMOD_IF_ERR_RETURN(qmod16_rx_squelch_set(&pm_phy_copy.access, 0));
    }    
    return PHYMOD_E_NONE;
    
}

int qtce16_phy_power_get(const phymod_phy_access_t* phy, phymod_phy_power_t* power)
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
        (qmod16_lane_info(&phy->access, &lane_id, &sub_port));
    
    start_lane = lane_id ;
    pm_phy_copy.access.lane_mask = 0x1 << start_lane;

    PHYMOD_IF_ERR_RETURN(qmod16_rx_squelch_get(&pm_phy_copy.access, &enable));

    /* next check if PMD loopback is on */ 
    if (enable) {                           
        PHYMOD_IF_ERR_RETURN(merlin16_pmd_loopback_get(&pm_phy_copy.access, &lb_enable));
        if (lb_enable) enable = 0;
    }

    power->rx = (enable == 1)? phymodPowerOff: phymodPowerOn;
    power->tx = (enable == 1)? phymodPowerOff: phymodPowerOn;
        
    return PHYMOD_E_NONE;
}

int _qtce16_pll_multiplier_get(uint32_t pll_div, uint32_t *pll_multiplier)
{
    switch (pll_div) {
    case 0x0:
        *pll_multiplier = 46;
        break;
    case 0x1:
        *pll_multiplier = 72;
        break;
    case QMOD16_PLL_MODE_DIV_40:
        *pll_multiplier = 40;
        break;
    case QMOD16_PLL_MODE_DIV_42:
        *pll_multiplier = 42;
        break;
    case QMOD16_PLL_MODE_DIV_48:
        *pll_multiplier = 48;
        break;
    case 0x5:
        *pll_multiplier = 50;
        break;
    case QMOD16_PLL_MODE_DIV_52:
        *pll_multiplier = 52;
        break;
    case QMOD16_PLL_MODE_DIV_54:
        *pll_multiplier = 54;
        break;
    case QMOD16_PLL_MODE_DIV_60:
        *pll_multiplier = 60;
        break;
    case QMOD16_PLL_MODE_DIV_64:
        *pll_multiplier = 64;
        break;
    case QMOD16_PLL_MODE_DIV_66:
        *pll_multiplier = 66;
        break;
    case 0xb:
        *pll_multiplier = 68;
        break;
    case QMOD16_PLL_MODE_DIV_70:
        *pll_multiplier = 70;
        break;
    case QMOD16_PLL_MODE_DIV_80:
        *pll_multiplier = 80;
        break;
    case QMOD16_PLL_MODE_DIV_92:
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
int _qtce16_qsgmii_interface_config_set(const phymod_phy_access_t* phy, uint32_t flags, const phymod_phy_inf_config_t* config)
{
    qmod16_spd_intfc_type spd_intf = QMOD16_SPD_ILLEGAL;
    qmod16_spd_intfc_type qmod16_spd_intf = QMOD16_SPD_ILLEGAL, base_spd_intf;
    phymod_phy_access_t pm_phy_copy;
    int      start_lane, lane_id, sub_port, num_lane ;

    PHYMOD_MEMCPY(&pm_phy_copy, phy, sizeof(pm_phy_copy));

    /* next program the tx fir taps and driver current based on the input */
    /* get num_lane only in QTC */
    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(&phy->access, &start_lane, &num_lane));

    PHYMOD_IF_ERR_RETURN
        (qmod16_lane_info(&phy->access, &lane_id, &sub_port));

    start_lane = lane_id ;  
    pm_phy_copy.access.lane_mask = 0x1 << start_lane;

    spd_intf = QMOD16_SPD_1000_SGMII; /* to prevent undefinded QMOD16_SPD_ILLEGAL accessing tables */

    switch(config->data_rate) {
    case 10:
        spd_intf = QMOD16_SPD_10_SGMII;
        break;
    case 100:
        spd_intf = QMOD16_SPD_100_SGMII;
        break;
    case 1000:
        spd_intf = QMOD16_SPD_1000_SGMII;
        break;
    case 2500:
        spd_intf = QMOD16_SPD_2500_USXGMII;
        break;
    default:
        spd_intf = QMOD16_SPD_ILLEGAL;
        break;
    }
    
    qmod16_spd_intf = spd_intf;
    base_spd_intf = PHYMOD_ACC_F_QMODE_GET(&phy->access) ? QMOD16_SPD_1000_SGMII : QMOD16_SPD_2500_USXGMII;

    /* All sub-port speed changes have to go through the base, not replicated speed */
    PHYMOD_IF_ERR_RETURN
        (qmod16_set_qport_spd(&pm_phy_copy.access, sub_port, base_spd_intf));
    
    if (qmod16_spd_intf != base_spd_intf) {
        PHYMOD_USLEEP(200);
        PHYMOD_IF_ERR_RETURN
            (qmod16_set_qport_spd(&pm_phy_copy.access, sub_port, qmod16_spd_intf));
    }

    return PHYMOD_E_NONE;

}

int qtce16_phy_interface_config_set(const phymod_phy_access_t* phy, uint32_t flags, const phymod_phy_inf_config_t* config)
{
    uint32_t current_pll_div=0;
    uint32_t new_pll_div=0;
    uint16_t new_speed_vec=0;
    qmod16_spd_intfc_type spd_intf = QMOD16_SPD_ILLEGAL;
    phymod_phy_access_t pm_phy_copy;
    int start_lane, num_lane, i;
    int      lane_id, sub_port ;
    uint32_t sc_enable = 0;
    uint32_t u_os_mode = 0;
    phymod_firmware_lane_config_t firmware_lane_config;

    PHYMOD_MEMCPY(&pm_phy_copy, phy, sizeof(pm_phy_copy));

    firmware_lane_config.MediaType = phymodFirmwareMediaTypePcbTraceBackPlane;  /* MediaType=0 */

    /* next program the tx fir taps and driver current based on the input */
    /* get num_lane only in QTC */
    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(&phy->access, &start_lane, &num_lane));

    PHYMOD_IF_ERR_RETURN
        (qmod16_lane_info(&phy->access, &lane_id, &sub_port));

    start_lane = lane_id ;  
    pm_phy_copy.access.lane_mask = 0x1 << start_lane;
    
    /* Set subport speed if the lane speed is configured in QSGMII/USXGMII mode */
    if (PHYMOD_ACC_F_QMODE_GET(&phy->access) || PHYMOD_ACC_F_USXMODE_GET(&phy->access)) {
        qmod16_speedchange_get(&pm_phy_copy.access, &sc_enable) ;
        if (sc_enable) {
            return _qtce16_qsgmii_interface_config_set(phy, flags, config);
        }
    }

    PHYMOD_IF_ERR_RETURN
        (qmod16_reset(&pm_phy_copy.access)); 
      
    /* Hold the per lane soft reset bit */
    for (i = 0; i < num_lane; i++) {
        pm_phy_copy.access.lane_mask = 1 << (start_lane + i);
        PHYMOD_IF_ERR_RETURN
            (merlin16_lane_soft_reset_release(&pm_phy_copy.access, 0));
    }

    pm_phy_copy.access.lane_mask = 0x1 << start_lane;
     PHYMOD_IF_ERR_RETURN
        (qtce16_phy_firmware_lane_config_get(&pm_phy_copy, &firmware_lane_config));

    /* make sure that an and config from pcs is off */
    firmware_lane_config.AnEnabled = 0;
    firmware_lane_config.LaneConfigFromPCS = 0;
    if (PHYMOD_INTF_MODES_FIBER_GET(config)) {
        firmware_lane_config.MediaType = phymodFirmwareMediaTypeOptics;
    }     

    if (config->data_rate == 10 || config->data_rate == 100 || config->data_rate == 1000) {
        if (config->interface_type == phymodInterfaceSGMII) {
            firmware_lane_config.MediaType = phymodFirmwareMediaTypePcbTraceBackPlane;
        }
        if (config->interface_type == phymodInterface1000X) {
            firmware_lane_config.MediaType = phymodFirmwareMediaTypeOptics;
        }
    } else if (config->data_rate == 2500) {
        if (config->interface_type == phymodInterfaceSGMII) {
            firmware_lane_config.MediaType  = phymodFirmwareMediaTypePcbTraceBackPlane;
            firmware_lane_config.DfeOn      = 1;
            firmware_lane_config.Cl72RestTO = 1;
        } else {
            firmware_lane_config.MediaType = phymodFirmwareMediaTypeOptics;
        }
    }

    switch (config->data_rate) {
    case 10:
        spd_intf = QMOD16_SPD_10_X1_10;
        break;
    case 100:
        spd_intf = QMOD16_SPD_100_X1_10;
        break;
    case 1000:
        spd_intf = QMOD16_SPD_1000_X1_10;
        break;
    default:
        spd_intf = QMOD16_SPD_ILLEGAL;
        break;
    }

    if (PHYMOD_ACC_F_QMODE_GET(&phy->access)) {
         spd_intf = QMOD16_SPD_4000;
    } else if (PHYMOD_ACC_F_USXMODE_GET(&phy->access)) {
         spd_intf = QMOD16_SPD_10G_X1_USXGMII;
    }

    PHYMOD_IF_ERR_RETURN
        (qmod16_get_plldiv(&pm_phy_copy.access, &current_pll_div));

    PHYMOD_IF_ERR_RETURN
        (qmod16_plldiv_lkup_get(&pm_phy_copy.access, spd_intf, &new_pll_div, &new_speed_vec));

    PHYMOD_IF_ERR_RETURN
        (qmod16_pmd_osmode_set(&pm_phy_copy.access, spd_intf, u_os_mode));

    /* Don't support PLL change because 10.3125G is for USXGMII and 10G for QSGMII/SGMII/GMII.*/
    if (current_pll_div != new_pll_div) {
        PHYMOD_RETURN_WITH_ERR(PHYMOD_E_CONFIG, 
                               (_PHYMOD_MSG("Not support VCO change: %u %u\r\n"),
                                 current_pll_div, new_pll_div));
    }

    for (i = 0; i < num_lane; i++) {
        pm_phy_copy.access.lane_mask = 0x1 << (start_lane + i);
        PHYMOD_IF_ERR_RETURN
            (_qtce16_phy_firmware_lane_config_set(&pm_phy_copy, firmware_lane_config));
    }

    /* Release the per lane soft reset bit */
    for (i = 0; i < num_lane; i++) {
        pm_phy_copy.access.lane_mask = 1 << (start_lane + i);
        PHYMOD_IF_ERR_RETURN
            (merlin16_lane_soft_reset_release(&pm_phy_copy.access, 1));
    }

    /* Set lane speed ID and trigger speed change */
    PHYMOD_IF_ERR_RETURN
        (qmod16_set_spd_intf(&pm_phy_copy.access, spd_intf, 0));

    /* Set sub-port speed of this lane */
    if (PHYMOD_ACC_F_QMODE_GET(&phy->access) || PHYMOD_ACC_F_USXMODE_GET(&phy->access)) {
        PHYMOD_IF_ERR_RETURN
            (_qtce16_qsgmii_interface_config_set(phy, flags, config));
    }


    return PHYMOD_E_NONE;
}

int _qtce16_speed_id_interface_config_get(const phymod_phy_access_t* phy, int speed_id, 
                                        phymod_phy_inf_config_t* config, uint16_t an_enable, 
                                        phymod_firmware_lane_config_t *lane_config)
{
    int ilkn_set;
    int osr_mode;
    uint32_t vco_rate;
    uint32_t pll_multiplier;
    uint32_t current_pll_div=0;
    uint32_t actual_osr, actual_osr_rem;
    phymod_osr_mode_t osr_mode_enum;

    PHYMOD_IF_ERR_RETURN
        (qmod16_get_plldiv(&phy->access, &current_pll_div));

    ilkn_set = 0;
    /* coverity[dead_error_condition] */
    if(ilkn_set) {
        /* coverity[dead_error_begin] */
        config->interface_type = phymodInterfaceBypass;
        PHYMOD_IF_ERR_RETURN
            (_qtce16_pll_multiplier_get(current_pll_div, &pll_multiplier));
        PHYMOD_IF_ERR_RETURN
            (merlin16_osr_mode_get(&phy->access, &osr_mode));

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

        PHYMOD_IF_ERR_RETURN(merlin16_osr_mode_to_enum(osr_mode, &osr_mode_enum));
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
      case 0x5:
          config->data_rate = 1000;
          config->interface_type = phymodInterfaceKX;
          break;
      case 0x6:
          config->data_rate = 2500;
          if (lane_config->MediaType == phymodFirmwareMediaTypeOptics) {
              config->interface_type = phymodInterfaceSR;
          } else {
              config->interface_type = phymodInterfaceSGMII;
          }
          break;
      case 0x39:
          config->data_rate = 10;
          config->interface_type = phymodInterfaceSGMII;
          break;
      case 0x3a:
          config->data_rate = 100;
          config->interface_type = phymodInterfaceSGMII;
          break;
      case 0x3b:
          config->data_rate = 1000;
          config->interface_type = phymodInterfaceSGMII;
          break;
      case 0x3c:  /* this is qsgmii mode */
          config->data_rate = 1000;
          config->interface_type = phymodInterfaceSGMII;
          break;
      case 0x48:
          config->data_rate = 2500;
          config->interface_type = phymodInterfaceSGMII;
          break;
      default:
          config->data_rate = 0;
          config->interface_type = phymodInterfaceSGMII;
          break;
      }
    }
    return PHYMOD_E_NONE;
}

int qtce16_phy_interface_config_get(const phymod_phy_access_t* phy, uint32_t flags, phymod_ref_clk_t ref_clock, phymod_phy_inf_config_t* config)
{        
    int speed_id;
    phymod_firmware_lane_config_t firmware_lane_config;  
    phymod_phy_access_t pm_phy_copy;
    int start_lane, num_lane;
    qmod16_an_control_t an_control;
    qmod16_an_ability_t an_ability_st;
    int an_complete = 0;
    int      lane_id, sub_port ;

    config->ref_clock = ref_clock;
    PHYMOD_MEMCPY(&pm_phy_copy, phy, sizeof(pm_phy_copy));
    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(&phy->access, &start_lane, &num_lane));

    PHYMOD_IF_ERR_RETURN
        (qmod16_lane_info(&phy->access, &lane_id, &sub_port));
    start_lane = lane_id ;
    
    pm_phy_copy.access.lane_mask = 0x1 << start_lane;
    PHYMOD_IF_ERR_RETURN
        (qmod16_speed_id_get(&pm_phy_copy.access, &speed_id, sub_port));

    PHYMOD_MEMSET(&an_control, 0x0,  sizeof(qmod16_an_control_t));
    PHYMOD_IF_ERR_RETURN
        (qmod16_autoneg_control_get(&pm_phy_copy.access, sub_port, &an_control, &an_complete));

    /* There is no resolved speed ID register for USXGMII. Get it from AN resolution HCD if AN is enabled */
    if (PHYMOD_ACC_F_USXMODE_GET(&phy->access) && an_control.enable && an_complete) {
        PHYMOD_MEMSET(&an_ability_st, 0x0,  sizeof(qmod16_an_ability_t));
        PHYMOD_IF_ERR_RETURN
            (qmod16_autoneg_resolution_ability_get(&pm_phy_copy.access, &an_ability_st, sub_port));

        speed_id = an_ability_st.cl37_adv.cl37_sgmii_speed;
    }

    PHYMOD_IF_ERR_RETURN
        (qtce16_phy_firmware_lane_config_get(phy, &firmware_lane_config));
    
    PHYMOD_IF_ERR_RETURN
        (_qtce16_speed_id_interface_config_get(&pm_phy_copy, speed_id, config, an_control.enable, &firmware_lane_config));

    if (firmware_lane_config.MediaType == phymodFirmwareMediaTypeOptics) {
        PHYMOD_INTF_MODES_FIBER_SET(config);
    } else {
        PHYMOD_INTF_MODES_FIBER_CLR(config);
    }

    return PHYMOD_E_NONE;
    
}

int qtce16_phy_autoneg_ability_set(const phymod_phy_access_t* phy, const phymod_autoneg_ability_t* an_ability)
{        
    qmod16_an_ability_t value;
    int start_lane, num_lane;
    phymod_phy_access_t phy_copy;
    int      lane_id, sub_port ;

    /* next program the tx fir taps and driver current based on the input */
    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(&phy->access, &start_lane, &num_lane));

    PHYMOD_IF_ERR_RETURN
        (qmod16_lane_info(&phy->access, &lane_id, &sub_port));

    start_lane = lane_id ;  

    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));
    phy_copy.access.lane_mask = 0x1 << start_lane;

    PHYMOD_MEMSET(&value, 0x0, sizeof(value));

    /* check if sgmii  or not */
    if (PHYMOD_AN_CAP_SGMII_GET(an_ability)) {
        switch (an_ability->sgmii_speed) {
        case phymod_CL37_SGMII_10M:
            value.cl37_adv.cl37_sgmii_speed = QMOD16_CL37_SGMII_10M;
            break;
        case phymod_CL37_SGMII_100M:
            value.cl37_adv.cl37_sgmii_speed = QMOD16_CL37_SGMII_100M;
            break;
        case phymod_CL37_SGMII_1000M:
            value.cl37_adv.cl37_sgmii_speed = QMOD16_CL37_SGMII_1000M;
            break;
        case phymod_CL37_SGMII_2500M:
            value.cl37_adv.cl37_sgmii_speed = QMOD16_CL37_SGMII_2500M;
            break;
        default:
            value.cl37_adv.cl37_sgmii_speed = QMOD16_CL37_SGMII_1000M;
            break;
        }
        if( PHYMOD_AN_CAP_HALF_DUPLEX_GET(an_ability)) {
            value.cl37_adv.an_duplex = QMOD16_AN_HALF_DUPLEX_SET ;
        }
    }
    /* next check pause */
    if (PHYMOD_AN_CAP_SYMM_PAUSE_GET(an_ability) && !PHYMOD_AN_CAP_ASYM_PAUSE_GET(an_ability)) {
        value.cl37_adv.an_pause = QMOD16_SYMM_PAUSE;
    }
    if (PHYMOD_AN_CAP_ASYM_PAUSE_GET(an_ability) && !PHYMOD_AN_CAP_SYMM_PAUSE_GET(an_ability)) {
        value.cl37_adv.an_pause = QMOD16_ASYM_PAUSE;
    }
    if (PHYMOD_AN_CAP_ASYM_PAUSE_GET(an_ability) && PHYMOD_AN_CAP_SYMM_PAUSE_GET(an_ability)) {
        value.cl37_adv.an_pause = QMOD16_ASYM_SYMM_PAUSE;
    }

    /* check cl37 and cl37 bam ability */
    if (PHYMOD_AN_CAP_CL37BAM_GET(an_ability)) {
        value.cl37_adv.an_bam_speed |= 1 << QMOD16_CL37_BAM_2p5GBASE_X;
        PHYMOD_IF_ERR_RETURN
            (qmod16_an_cl37_bam_abilities(&phy_copy.access, &value, sub_port));
    } else { 
        PHYMOD_IF_ERR_RETURN
            (qmod16_autoneg_cl37_base_abilities(&phy_copy.access, &value, sub_port));
    }
        
    return PHYMOD_E_NONE;
    
}

int qtce16_phy_autoneg_ability_get(const phymod_phy_access_t* phy, phymod_autoneg_ability_t* an_ability_get_type)
{        
    qmod16_an_ability_t value;
    phymod_phy_access_t phy_copy;
    int      lane_id, sub_port ;


    PHYMOD_IF_ERR_RETURN
        (qmod16_lane_info(&phy->access, &lane_id, &sub_port));

    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));
    phy_copy.access.lane_mask = 0x1 << lane_id;
    PHYMOD_MEMSET(&value, 0x0, sizeof(value));
     
    PHYMOD_IF_ERR_RETURN
        (qmod16_autoneg_local_ability_get(&phy_copy.access, &value, sub_port));

    if(value.cl37_adv.an_type != QMOD16_AN_MODE_NONE) {
        if (value.cl37_adv.an_pause == QMOD16_ASYM_PAUSE) {
            PHYMOD_AN_CAP_ASYM_PAUSE_SET(an_ability_get_type);
        } else if (value.cl37_adv.an_pause == QMOD16_SYMM_PAUSE) {
            PHYMOD_AN_CAP_SYMM_PAUSE_SET(an_ability_get_type);
        } else if (value.cl37_adv.an_pause == QMOD16_ASYM_SYMM_PAUSE) {
            PHYMOD_AN_CAP_ASYM_PAUSE_SET(an_ability_get_type);
            PHYMOD_AN_CAP_SYMM_PAUSE_SET(an_ability_get_type);
        }
        if(value.cl37_adv.an_type & QMOD16_AN_MODE_CL37) {
            PHYMOD_AN_CAP_CL37_SET(an_ability_get_type) ;
            PHYMOD_AN_CAP_HALF_DUPLEX_CLR(an_ability_get_type) ;
        }
        if(value.cl37_adv.an_type & QMOD16_AN_MODE_SGMII) {
            PHYMOD_AN_CAP_SGMII_SET(an_ability_get_type) ;
            /* get the cl37 sgmii speed */
            switch (value.cl37_adv.cl37_sgmii_speed) {
            case QMOD16_CL37_SGMII_10M:
                an_ability_get_type->sgmii_speed = phymod_CL37_SGMII_10M;
                break;
            case QMOD16_CL37_SGMII_100M:
                an_ability_get_type->sgmii_speed = phymod_CL37_SGMII_100M;
                break;
            case QMOD16_CL37_SGMII_1000M:
                an_ability_get_type->sgmii_speed = phymod_CL37_SGMII_1000M;
                break;
            case QMOD16_CL37_SGMII_2500M:
                an_ability_get_type->sgmii_speed = phymod_CL37_SGMII_2500M;
                break;
            default:
                break;
            }
            if(value.cl37_adv.an_duplex & QMOD16_AN_HALF_DUPLEX_SET ) { 
                PHYMOD_AN_CAP_HALF_DUPLEX_SET(an_ability_get_type) ;
            } else {
                PHYMOD_AN_CAP_HALF_DUPLEX_CLR(an_ability_get_type) ;
            }
        }
        if(value.cl37_adv.an_type & QMOD16_AN_MODE_CL37BAM) {
            /* check cl37 bam ability */
            if (value.cl37_adv.an_bam_speed & 1 << QMOD16_CL37_BAM_2p5GBASE_X) 
                PHYMOD_BAM_CL37_CAP_2P5G_SET(an_ability_get_type->cl37bam_cap);
        }
    }
    return PHYMOD_E_NONE;
    
}


int qtce16_phy_autoneg_remote_ability_get(const phymod_phy_access_t* phy, phymod_autoneg_ability_t* an_ability_get_type)
{        
    
    qmod16_an_ability_t value;
    phymod_phy_access_t phy_copy;
    int      lane_id, sub_port ;

    
    PHYMOD_IF_ERR_RETURN
        (qmod16_lane_info(&phy->access, &lane_id, &sub_port));

    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));
    phy_copy.access.lane_mask = 0x1 << lane_id;
    PHYMOD_MEMSET(&value, 0x0, sizeof(value));
     
    PHYMOD_IF_ERR_RETURN
        (qmod16_autoneg_remote_ability_get(&phy_copy.access, &value, sub_port));

    if (value.cl37_adv.an_pause == QMOD16_ASYM_PAUSE) {
        PHYMOD_AN_CAP_ASYM_PAUSE_SET(an_ability_get_type);
    } else if (value.cl37_adv.an_pause == QMOD16_SYMM_PAUSE) {
        PHYMOD_AN_CAP_SYMM_PAUSE_SET(an_ability_get_type);
    } else if (value.cl37_adv.an_pause == QMOD16_ASYM_SYMM_PAUSE) {
        PHYMOD_AN_CAP_ASYM_PAUSE_SET(an_ability_get_type);
        PHYMOD_AN_CAP_SYMM_PAUSE_SET(an_ability_get_type);
    }
    if(value.cl37_adv.an_type != QMOD16_AN_MODE_NONE) {     
       if(value.cl37_adv.an_type & QMOD16_AN_MODE_CL37) {
            PHYMOD_AN_CAP_CL37_SET(an_ability_get_type) ;
            PHYMOD_AN_CAP_HALF_DUPLEX_CLR(an_ability_get_type) ;
       }
        if(value.cl37_adv.an_type & QMOD16_AN_MODE_SGMII) {
            PHYMOD_AN_CAP_SGMII_SET(an_ability_get_type) ;
            /* get the cl37 sgmii speed */
            switch (value.cl37_adv.cl37_sgmii_speed) {
            case QMOD16_CL37_SGMII_10M:
                an_ability_get_type->sgmii_speed = phymod_CL37_SGMII_10M;
                break;
            case QMOD16_CL37_SGMII_100M:
                an_ability_get_type->sgmii_speed = phymod_CL37_SGMII_100M;
                break;
            case QMOD16_CL37_SGMII_1000M:
                an_ability_get_type->sgmii_speed = phymod_CL37_SGMII_1000M;
                break;
            case QMOD16_CL37_SGMII_2500M:
                an_ability_get_type->sgmii_speed = phymod_CL37_SGMII_2500M;
                break;
            default:
                break;
            }
        }
        /* check cl37 bam ability */
        if (value.cl37_adv.an_bam_speed & 1 << QMOD16_CL37_BAM_2p5GBASE_X) 
            PHYMOD_BAM_CL37_CAP_2P5G_SET(an_ability_get_type->cl37bam_cap);
    }
        
    return PHYMOD_E_NONE;
    
}

STATIC
int _qtce16_qsgmii_autoneg_set(const phymod_phy_access_t* phy, const phymod_autoneg_control_t* an)
{
    int num_lane_adv_encoded;
    int start_lane, num_lane, lane_id, sub_port;
    phymod_phy_access_t phy_copy;
    qmod16_an_control_t an_control;
    
    
    /* next program the tx fir taps and driver current based on the input */
    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(&phy->access, &start_lane, &num_lane));

    PHYMOD_IF_ERR_RETURN
        (qmod16_lane_info(&phy->access, &lane_id, &sub_port));

    start_lane = lane_id ;

    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));
    phy_copy.access.lane_mask = 0x1 << start_lane;

    num_lane_adv_encoded = 0;  /* one lane */
    an_control.pd_kx4_en = 0;  /* for now  disable */
    an_control.pd_kx_en  = 0;   /* for now disable */
    an_control.num_lane_adv = num_lane_adv_encoded;
    an_control.enable       = an->enable;
    an_control.an_property_type = 0x0;   /* for now disable */
    an_control.an_type          = QMOD16_AN_MODE_SGMII;

    phy_copy.access.lane_mask = 0x1 << start_lane;

    PHYMOD_IF_ERR_RETURN
        (qmod16_autoneg_control(&phy_copy.access, &an_control, sub_port));
    
    return PHYMOD_E_NONE;

}

int qtce16_phy_autoneg_set(const phymod_phy_access_t* phy, const phymod_autoneg_control_t* an)
{        
    phymod_firmware_lane_config_t firmware_lane_config;
    int start_lane, num_lane, i, lane_id, sub_port;
    phymod_phy_access_t phy_copy;
    qmod16_an_control_t an_control;

    /* QTCE16 doesn't support 10G KX4 and 1G KX parallel detection */
    if (PHYMOD_AN_F_SET_CL73_PDET_KX4_ENABLE_GET(an) ||
        PHYMOD_AN_F_SET_CL73_PDET_KX_ENABLE_GET(an)) {
        return PHYMOD_E_UNAVAIL;
    }

    if (PHYMOD_ACC_F_QMODE_GET(&phy->access) || PHYMOD_ACC_F_USXMODE_GET(&phy->access)) {
        return _qtce16_qsgmii_autoneg_set(phy, an);
    }

    /* next program the tx fir taps and driver current based on the input */
    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(&phy->access, &start_lane, &num_lane));

    PHYMOD_IF_ERR_RETURN
        (qmod16_lane_info(&phy->access, &lane_id, &sub_port));

    start_lane = lane_id ;  
    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));
    phy_copy.access.lane_mask = 0x1 << start_lane;

    an_control.pd_kx4_en = 0;  /* for now  disable */
    an_control.pd_kx_en = 0;   /* for now disable */
    an_control.num_lane_adv = 0;
    an_control.enable       = an->enable;
    an_control.an_property_type = 0x0;   /* for now disable */  
    switch (an->an_mode) {
    case phymod_AN_MODE_CL37:
        an_control.an_type = QMOD16_AN_MODE_CL37;
        break;
    case phymod_AN_MODE_CL37BAM:
        { 
            an_control.an_type = QMOD16_AN_MODE_CL37BAM;
            break;
        }
    case phymod_AN_MODE_SGMII:
        an_control.an_type = QMOD16_AN_MODE_SGMII;
        break;
    default:
        an_control.an_type = QMOD16_AN_MODE_CL37;
        break;
    }

    /* put pcs into reset */
    PHYMOD_IF_ERR_RETURN
            (qmod16_reset(&phy_copy.access)); 
    
    for (i = 0; i < num_lane; i++) {
        phy_copy.access.lane_mask = 0x1 << (i + start_lane);
        PHYMOD_IF_ERR_RETURN
            (merlin16_lane_soft_reset_release(&phy_copy.access, 0));
    }
     
    /* make sure the firmware config is set to an eenabled */
    PHYMOD_IF_ERR_RETURN
        (qtce16_phy_firmware_lane_config_get(phy, &firmware_lane_config));
    if (an->enable) {
        firmware_lane_config.AnEnabled = 1;
        firmware_lane_config.LaneConfigFromPCS = 1;
    } else {
        firmware_lane_config.AnEnabled = 0;
        firmware_lane_config.LaneConfigFromPCS = 0;
    }

    for (i = 0; i < num_lane; i++) {
        phy_copy.access.lane_mask = 0x1 << (i + start_lane);
        PHYMOD_IF_ERR_RETURN
            (_qtce16_phy_firmware_lane_config_set(&phy_copy, firmware_lane_config));
    }

    for (i = 0; i < num_lane; i++) {
        phy_copy.access.lane_mask = 0x1 << (i + start_lane);
        PHYMOD_IF_ERR_RETURN
            (merlin16_lane_soft_reset_release(&phy_copy.access, 1));
    } 

    if (!an->enable) {  /* would SGMII AN exception */
        PHYMOD_IF_ERR_RETURN
            (qmod16_trigger_speed_change(&phy_copy.access));  
    }

    phy_copy.access.lane_mask = 0x1 << start_lane;
    PHYMOD_IF_ERR_RETURN
        (qmod16_autoneg_control(&phy_copy.access, &an_control, sub_port));
    
        
    return PHYMOD_E_NONE;
    
}

int qtce16_phy_autoneg_get(const phymod_phy_access_t* phy, phymod_autoneg_control_t* an, uint32_t* an_done)
{        

    qmod16_an_control_t an_control;
    phymod_phy_access_t phy_copy;
    int start_lane, num_lane, lane_id, sub_port; 
    int an_complete = 0;
    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));

    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(&phy->access, &start_lane, &num_lane));

    PHYMOD_IF_ERR_RETURN
        (qmod16_lane_info(&phy->access, &lane_id, &sub_port));

    start_lane = lane_id ;  
    phy_copy.access.lane_mask = 0x1 << start_lane;

    PHYMOD_MEMSET(&an_control, 0x0,  sizeof(qmod16_an_control_t));
    PHYMOD_IF_ERR_RETURN
        (qmod16_autoneg_control_get(&phy_copy.access,  sub_port, &an_control, &an_complete));
    
    if (an_control.enable) {
        an->enable = 1;
        if (an_control.an_type == QMOD16_AN_MODE_CL37) {
            an->an_mode = phymod_AN_MODE_CL37;
        } else if (an_control.an_type == QMOD16_AN_MODE_CL37BAM) {
            an->an_mode = phymod_AN_MODE_CL37BAM;
        } else {
            an->an_mode = phymod_AN_MODE_SGMII;
        }
        *an_done = an_complete; 
    } else {
        an->enable = 0;
    }

    /* QTCE16 doesn't support 10G KX4 and 1G KX parallel detection */
    PHYMOD_AN_F_SET_CL73_PDET_KX_ENABLE_CLR(an);
    PHYMOD_AN_F_SET_CL73_PDET_KX4_ENABLE_CLR(an);

        
    return PHYMOD_E_NONE;
    
}


int qtce16_phy_autoneg_status_get(const phymod_phy_access_t* phy, phymod_autoneg_status_t* status)
{

    return PHYMOD_E_NONE;
    
}


int qtce16_core_init(const phymod_core_access_t* core, const phymod_core_init_config_t* init_config, const phymod_core_status_t* core_status)
{        
    phymod_phy_access_t phy_access, phy_access_copy;
    phymod_core_access_t  core_copy;
    phymod_firmware_core_config_t  firmware_core_config_tmp;
    uint32_t  uc_active = 0;
    int i, num_lane, start_lane;


    PHYMOD_MEMCPY(&core_copy, core, sizeof(core_copy));
    core_copy.access.lane_mask = 0x1;

    PHYMOD_IF_ERR_RETURN(phymod_phy_access_t_init(&phy_access));
    QTCE16_CORE_TO_PHY_ACCESS(&phy_access, core);
    PHYMOD_MEMCPY(&phy_access_copy, &phy_access, sizeof(phy_access_copy));
    phy_access_copy.access.lane_mask = 0x1;

    PHYMOD_IF_ERR_RETURN
        (qmod16_pmd_reset_seq(&core_copy.access, core_status->pmd_active));

    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(&phy_access.access, &start_lane, &num_lane));

    /*
     * Before programming the PMD lane address map register, the PMD lanes
     * have to be reset. Without do this, writing the PMD lane address map
     * regsiter will not take effect, meaning the reading value != writing
     * value.
     */
    for (i = 0; i < QTCE16_NOF_LANES_IN_CORE; i++) {
        phy_access.access.lane_mask = 1 << (start_lane + i);
        PHYMOD_IF_ERR_RETURN
            (qmod16_pmd_x4_reset(&phy_access.access));
    }

    PHYMOD_IF_ERR_RETURN(merlin16_uc_active_get(&core_copy.access, &uc_active));
    if (uc_active) {
        return(PHYMOD_E_NONE);
    }

    /* Propgram shim fifo threshold for USXGMII mode */
    if (PHYMOD_ACC_F_USXMODE_GET(&core->access)) {
        PHYMOD_IF_ERR_RETURN(qmod16_usgmii_shim_fifo_threshold_set(&core_copy.access));
    }

    /* need to set the heart beat default is for 156.25M */
    if (init_config->interface.ref_clock == phymodRefClk125Mhz) {
        PHYMOD_IF_ERR_RETURN
            (qmod16_refclk_set(&core_copy.access, QMOD16REFCLK125MHZ)) ;
    } else {
        PHYMOD_IF_ERR_RETURN
            (qmod16_refclk_set(&core_copy.access, QMOD16REFCLK156MHZ)) ;
    }
  
     PHYMOD_IF_ERR_RETURN
        (qtce16_core_lane_map_set(&core_copy, &init_config->lane_map));

    PHYMOD_IF_ERR_RETURN
        (merlin16_uc_reset(&phy_access_copy.access, 1));

    if (_qtce16_core_firmware_load(&core_copy, init_config->firmware_load_method, init_config->firmware_loader)) {
        PHYMOD_DEBUG_ERROR(("devad 0x%x lane 0x%x: UC firmware-load failed\n", core->access.addr, core->access.lane_mask));  
        PHYMOD_IF_ERR_RETURN (PHYMOD_E_INIT);
    }

    /* merlin16 programmer quide */
    PHYMOD_IF_ERR_RETURN
        (merlin16_pmd_ln_h_rstb_pkill_override( &phy_access_copy.access, 0x1));

    PHYMOD_IF_ERR_RETURN
        (merlin16_uc_reset(&phy_access_copy.access, 0));
    PHYMOD_IF_ERR_RETURN
        (merlin16_wait_uc_active(&phy_access_copy.access));

    /* Initialize software information table for the micro */
    PHYMOD_IF_ERR_RETURN
        (merlin16_init_merlin16_info(&core_copy.access));

    if (init_config->firmware_load_method != phymodFirmwareLoadMethodNone) {
        if (PHYMOD_CORE_INIT_F_FIRMWARE_LOAD_VERIFY_GET(init_config)) {
            PHYMOD_IF_ERR_RETURN
                (merlin16_start_ucode_crc_calc(&core_copy.access, merlin16_ucode_len));
        }
    }

    if (init_config->firmware_load_method != phymodFirmwareLoadMethodNone) {
        if (PHYMOD_CORE_INIT_F_FIRMWARE_LOAD_VERIFY_GET(init_config)) {
            PHYMOD_IF_ERR_RETURN
                (merlin16_check_ucode_crc(&core_copy.access, merlin16_ucode_crc, 250));
        }
    }

    PHYMOD_IF_ERR_RETURN(
        merlin16_pmd_ln_h_rstb_pkill_override( &phy_access_copy.access, 0x0));

    PHYMOD_IF_ERR_RETURN
        (merlin16_core_soft_reset_release(&core_copy.access, 0));

    /* plldiv CONFIG */
    if (PHYMOD_ACC_F_USXMODE_GET(&core->access)) {
        PHYMOD_IF_ERR_RETURN
            (merlin16_configure_pll_refclk_div(&core_copy.access, MERLIN16_PLL_REFCLK_156P25MHZ, MERLIN16_PLL_DIV_66));
    } else {
        PHYMOD_IF_ERR_RETURN
            (merlin16_configure_pll_refclk_div(&core_copy.access, MERLIN16_PLL_REFCLK_156P25MHZ, MERLIN16_PLL_DIV_64));
    }
   
    PHYMOD_IF_ERR_RETURN
        (qmod16_autoneg_timer_init(&core_copy.access));
    PHYMOD_IF_ERR_RETURN
        (qmod16_master_port_num_set(&core_copy.access, 0));

    /* don't overide the fw that set in config set if not specified */
    PHYMOD_IF_ERR_RETURN
        (qtce16_phy_firmware_core_config_get(&phy_access_copy, &firmware_core_config_tmp));
    firmware_core_config_tmp.CoreConfigFromPCS = 0;
    PHYMOD_IF_ERR_RETURN
        (qtce16_phy_firmware_core_config_set(&phy_access_copy, firmware_core_config_tmp)); 

    /* release core soft reset */
    PHYMOD_IF_ERR_RETURN
        (merlin16_core_soft_reset_release(&core_copy.access, 1));
        
    return PHYMOD_E_NONE;
    
}


int qtce16_phy_pll_multiplier_get(const phymod_phy_access_t* phy, uint32_t* core_vco_pll_multiplier)
{        
    
    /* Place your code here */

        
    return PHYMOD_E_NONE;
    
}


int qtce16_phy_init(const phymod_phy_access_t* phy, const phymod_phy_init_config_t* init_config)
{
    const phymod_access_t *pm_acc = &phy->access;
    phymod_phy_access_t pm_phy_copy;
    int start_lane, num_lane, i, lane_id, sub_port;
    uint32_t sc_enable = 0;
    phymod_polarity_t tmp_pol;
    PHYMOD_MEMCPY(&pm_phy_copy, phy, sizeof(pm_phy_copy));
    PHYMOD_MEMSET(&tmp_pol, 0x0, sizeof(tmp_pol));

    /* next program the tx fir taps and driver current based on the input */
    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(pm_acc, &start_lane, &num_lane));
    PHYMOD_IF_ERR_RETURN
        (qmod16_lane_info(&phy->access, &lane_id, &sub_port));

    start_lane = lane_id ;  
    pm_phy_copy.access.lane_mask = 1 << start_lane;

    if (PHYMOD_ACC_F_QMODE_GET(&phy->access) || PHYMOD_ACC_F_USXMODE_GET(&phy->access)) {
        PHYMOD_IF_ERR_RETURN(qmod16_port_state_set(&pm_phy_copy.access, QMOD16_PORT_STATE_CONFIGED, sub_port, 1));
        PHYMOD_IF_ERR_RETURN(qmod16_speedchange_get(&pm_phy_copy.access, &sc_enable)) ;
        /* this lane has been initialized */
        if (sc_enable) {
            return PHYMOD_E_NONE;
        }
    }

    /* per lane based reset release */
    PHYMOD_IF_ERR_RETURN
        (qmod16_pmd_x4_reset(&pm_phy_copy.access));

    /* poll for per lane uc_dsc_ready */
    for (i = 0; i < num_lane; i++) {
        pm_phy_copy.access.lane_mask = 1 << (start_lane + i);
        PHYMOD_IF_ERR_RETURN
            (merlin16_lane_soft_reset_release(&pm_phy_copy.access, 1));
    }

    /* program the rx/tx polarity */
    tmp_pol.tx_polarity = (init_config->polarity.tx_polarity) & 0x1;
    tmp_pol.rx_polarity = (init_config->polarity.rx_polarity) & 0x1;
    PHYMOD_IF_ERR_RETURN(qtce16_phy_polarity_set(phy, &tmp_pol));

    /* configure TX parameters */
    PHYMOD_IF_ERR_RETURN
        (qtce16_phy_tx_set(phy, &init_config->tx[0]));

    pm_phy_copy.access.lane_mask = 1 << start_lane;
    PHYMOD_IF_ERR_RETURN
        (qmod16_rx_lane_control_set(&pm_phy_copy.access, TRUE));
    PHYMOD_IF_ERR_RETURN
        (qmod16_tx_lane_control_set(&pm_phy_copy.access, QMOD16_TX_LANE_RESET_TRAFFIC_ENABLE));         /* TX_LANE_CONTROL */
        
    return PHYMOD_E_NONE;
    
}


int qtce16_phy_loopback_set(const phymod_phy_access_t* phy, phymod_loopback_mode_t loopback, uint32_t enable)
{        
    
    int start_lane, num_lane, lane_id, sub_port;
    int i = 0;
    phymod_phy_access_t phy_copy;
    if ((PHYMOD_ACC_F_QMODE_GET(&phy->access) || PHYMOD_ACC_F_USXMODE_GET(&phy->access)) && enable) {
        return PHYMOD_E_UNAVAIL;
    }

    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));

    /* next figure out the lane num and start_lane based on the input */
    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(&phy->access, &start_lane, &num_lane));

    PHYMOD_IF_ERR_RETURN
        (qmod16_lane_info(&phy->access, &lane_id, &sub_port));
    start_lane = lane_id ;

    phy_copy.access.lane_mask = 0x1 << (i + start_lane);

      
    switch (loopback) {
    case phymodLoopbackGlobal :
        PHYMOD_IF_ERR_RETURN(qmod16_tx_loopback_control(&phy_copy.access, enable, start_lane, num_lane));
        break;
    case phymodLoopbackGlobalPMD :
        PHYMOD_IF_ERR_RETURN(merlin16_pmd_loopback_set(&phy_copy.access, enable));
        break;
    case phymodLoopbackRemotePMD :
        PHYMOD_IF_ERR_RETURN(merlin16_rmt_lpbk(&phy_copy.access, (uint8_t)enable));
        break;
    case phymodLoopbackRemotePCS :
        PHYMOD_IF_ERR_RETURN(qmod16_rx_loopback_control(&phy_copy.access, enable, enable, enable));
        break;
    default :
        break;
    }
        
    return PHYMOD_E_NONE;
    
}

int qtce16_phy_loopback_get(const phymod_phy_access_t* phy, phymod_loopback_mode_t loopback, uint32_t* enable)
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
        (qmod16_lane_info(&phy->access, &lane_id, &sub_port));
    start_lane = lane_id ;

    phy_copy.access.lane_mask = 0x1 << start_lane;
    

    switch (loopback) {
    case phymodLoopbackGlobal :
        PHYMOD_IF_ERR_RETURN(qmod16_tx_loopback_get(&phy_copy.access, &enable_core));
        *enable = (enable_core >> start_lane) & 0x1; 
        break;
    case phymodLoopbackGlobalPMD :
        PHYMOD_IF_ERR_RETURN(merlin16_pmd_loopback_get(&phy_copy.access, enable));
        break;
    case phymodLoopbackRemotePMD :
        PHYMOD_IF_ERR_RETURN(merlin16_rmt_lpbk_get(&phy_copy.access, enable));
        break;
    case phymodLoopbackRemotePCS :
        PHYMOD_IF_ERR_RETURN(qmod16_rx_loopback_get(&phy_copy.access, enable));
        break;
    default :
        break;
    }   
    return PHYMOD_E_NONE;
    
}


int qtce16_phy_rx_pmd_locked_get(const phymod_phy_access_t* phy, uint32_t* rx_pmd_locked)
{        
    
    int lane_id, sub_port;
    phymod_phy_access_t phy_copy;

    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));
    PHYMOD_IF_ERR_RETURN
        (qmod16_lane_info(&phy->access, &lane_id, &sub_port));
    phy_copy.access.lane_mask = 1 << lane_id;

    PHYMOD_IF_ERR_RETURN(qmod16_pmd_lock_get(&phy_copy.access, rx_pmd_locked));
        
    return PHYMOD_E_NONE;
    
}


int qtce16_phy_link_status_get(const phymod_phy_access_t* phy, uint32_t* link_status)
{        
    
    int lane_id, sub_port;
    int rx_sq;
    phymod_phy_access_t pm_phy_copy;

    PHYMOD_MEMCPY(&pm_phy_copy, phy, sizeof(pm_phy_copy));
    PHYMOD_IF_ERR_RETURN
        (qmod16_lane_info(&phy->access, &lane_id, &sub_port));
    pm_phy_copy.access.lane_mask = 1 << lane_id;

    /* For QTC core, there is no register to disable subport.  Return link down  if the subport is disabled.*/
    if (PHYMOD_ACC_F_QMODE_GET(&phy->access) || PHYMOD_ACC_F_USXMODE_GET(&phy->access)) {
        PHYMOD_IF_ERR_RETURN(qmod16_port_state_get(&pm_phy_copy.access, QMOD16_PORT_STATE_RX_SQUELCH, &rx_sq));

        if (rx_sq & (1 << sub_port)) {
            *link_status = 0;
            return PHYMOD_E_NONE;
        }
    }

    PHYMOD_IF_ERR_RETURN(qmod16_get_pcs_link_status(&pm_phy_copy.access, sub_port, link_status));

        
    return PHYMOD_E_NONE;
    
}

int qtce16_phy_reg_read(const phymod_phy_access_t* phy, uint32_t reg_addr, uint32_t* val)
{
    int lane_id, sub_port;
    phymod_phy_access_t phy_copy;

    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));
    PHYMOD_IF_ERR_RETURN
        (qmod16_lane_info(&phy->access, &lane_id, &sub_port));
    phy_copy.access.lane_mask = 1 << lane_id;

    PHYMOD_IF_ERR_RETURN(phymod_tsc_iblk_read(&phy_copy.access, reg_addr, val));
    return PHYMOD_E_NONE;
}


int qtce16_phy_reg_write(const phymod_phy_access_t* phy, uint32_t reg_addr, uint32_t val)
{
    int lane_id, sub_port;
    phymod_phy_access_t phy_copy;

    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));
    PHYMOD_IF_ERR_RETURN
        (qmod16_lane_info(&phy->access, &lane_id, &sub_port));
    phy_copy.access.lane_mask = 1 << lane_id;

    PHYMOD_IF_ERR_RETURN(phymod_tsc_iblk_write(&phy_copy.access, reg_addr, val));
    return PHYMOD_E_NONE;
}

int qtce16_phy_eee_set(const phymod_phy_access_t* phy, uint32_t enable)
{
    int lane_id, sub_port;
    phymod_phy_access_t phy_copy;

    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));
    PHYMOD_IF_ERR_RETURN
        (qmod16_lane_info(&phy->access, &lane_id, &sub_port));
    phy_copy.access.lane_mask = 1 << lane_id;

    PHYMOD_IF_ERR_RETURN(qmod16_eee_control_set(&phy_copy.access, enable));
    return PHYMOD_E_NONE;
}

int qtce16_phy_eee_get(const phymod_phy_access_t* phy, uint32_t* enable)
{
    uint32_t enable_local;
    int lane_id, sub_port;
    phymod_phy_access_t phy_copy;

    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));
    PHYMOD_IF_ERR_RETURN
        (qmod16_lane_info(&phy->access, &lane_id, &sub_port));
    phy_copy.access.lane_mask = 1 << lane_id;

    PHYMOD_IF_ERR_RETURN(qmod16_eee_control_get(&phy_copy.access, &enable_local));
    *enable = enable_local;
    return PHYMOD_E_NONE;
}

int qtce16_phy_rx_ppm_get(const phymod_phy_access_t* phy, int16_t* rx_ppm)
{
    int lane_id, sub_port;
    phymod_phy_access_t phy_copy;

    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));
    PHYMOD_IF_ERR_RETURN
        (qmod16_lane_info(&phy->access, &lane_id, &sub_port));
    phy_copy.access.lane_mask = 1 << lane_id;

    PHYMOD_IF_ERR_RETURN
        (merlin16_tsc_rx_ppm(&phy_copy.access, rx_ppm));

    return PHYMOD_E_NONE;
}

int qtce16_phy_synce_clk_ctrl_set(const phymod_phy_access_t* phy,
                                  phymod_synce_clk_ctrl_t cfg)
{
    int lane_id, sub_port;
    phymod_phy_access_t phy_copy;

    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));
    PHYMOD_IF_ERR_RETURN
        (qmod16_lane_info(&phy->access, &lane_id, &sub_port));
    phy_copy.access.lane_mask = 1 << lane_id;

    PHYMOD_IF_ERR_RETURN
        (qmod16_synce_mode_set(&phy_copy.access, cfg.stg0_mode, cfg.stg1_mode));

    PHYMOD_IF_ERR_RETURN
        (qmod16_synce_clk_ctrl_set(&phy_copy.access, cfg.sdm_val));

    return PHYMOD_E_NONE;
}

int qtce16_phy_synce_clk_ctrl_get(const phymod_phy_access_t* phy,
                                  phymod_synce_clk_ctrl_t *cfg)
{
    int lane_id, sub_port;
    phymod_phy_access_t phy_copy;

    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));
    PHYMOD_IF_ERR_RETURN
        (qmod16_lane_info(&phy->access, &lane_id, &sub_port));
    phy_copy.access.lane_mask = 1 << lane_id;

    PHYMOD_IF_ERR_RETURN
        (qmod16_synce_mode_get(&phy_copy.access, &(cfg->stg0_mode), &(cfg->stg1_mode)));

    PHYMOD_IF_ERR_RETURN
        (qmod16_synce_clk_ctrl_get(&phy_copy.access, &(cfg->sdm_val)));

    return PHYMOD_E_NONE;
}


int qtce16_phy_pcs_tx_fsm_check(const phymod_phy_access_t* phy, uint32_t* tx_e_status)
{
    int lane_id, sub_port, latch_txfsm;
    phymod_phy_access_t phy_copy;

    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));
    PHYMOD_IF_ERR_RETURN
        (qmod16_lane_info(&phy->access, &lane_id, &sub_port));
    phy_copy.access.lane_mask = 1 << lane_id;

    PHYMOD_IF_ERR_RETURN
        (qmod16_tx_fsm_status_get(&phy_copy.access, sub_port, &latch_txfsm));

    if (latch_txfsm & QTCE16_TX_E) {
        *tx_e_status = 1;
    } else {
        *tx_e_status = 0;
    }

    return PHYMOD_E_NONE;
}

int qtce16_phy_pcs_reset(const phymod_phy_access_t* phy)
{
    int lane_id, sub_port;
    phymod_phy_access_t phy_copy;

    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));
    PHYMOD_IF_ERR_RETURN
        (qmod16_lane_info(&phy->access, &lane_id, &sub_port));
    phy_copy.access.lane_mask = 1 << lane_id;

    PHYMOD_IF_ERR_RETURN
        (qmod16_trigger_speed_change(&phy_copy.access));

    PHYMOD_IF_ERR_RETURN
        (qmod16_wait_sc_done(&phy_copy.access));

    return PHYMOD_E_NONE;
}

int qtce16_phy_duplex_set(const phymod_phy_access_t* phy, phymod_duplex_mode_t duplex)
{
    return PHYMOD_E_UNAVAIL;
}

int qtce16_phy_duplex_get(const phymod_phy_access_t* phy, phymod_duplex_mode_t* duplex)
{
    phymod_phy_access_t pm_phy_copy;
    qmod16_an_control_t an_control;
    qmod16_an_ability_t an_ability_st;
    int an_complete = 0;
    int lane_id, sub_port ;

    PHYMOD_MEMCPY(&pm_phy_copy, phy, sizeof(pm_phy_copy));

    PHYMOD_IF_ERR_RETURN(qmod16_lane_info(&phy->access, &lane_id, &sub_port));

    pm_phy_copy.access.lane_mask = 0x1 << lane_id;

    PHYMOD_MEMSET(&an_control, 0x0,  sizeof(qmod16_an_control_t));
    PHYMOD_MEMSET(&an_ability_st, 0x0,  sizeof(qmod16_an_ability_t));

    PHYMOD_IF_ERR_RETURN
        (qmod16_autoneg_control_get(&pm_phy_copy.access, sub_port, &an_control, &an_complete));

    if ((PHYMOD_ACC_F_QMODE_GET(&phy->access) || PHYMOD_ACC_F_USXMODE_GET(&phy->access)) &&
         an_control.enable && an_complete) {
        PHYMOD_IF_ERR_RETURN
            (qmod16_autoneg_resolution_ability_get(&pm_phy_copy.access, &an_ability_st, sub_port));

        *duplex = an_ability_st.cl37_adv.an_duplex ? phymodDuplexFull : phymodDuplexHalf;
        return PHYMOD_E_NONE;
    } else {
        return PHYMOD_E_UNAVAIL;
    }
}

#endif /* PHYMOD_QTCE16_SUPPORT */
