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
#include <phymod/chip/bcmi_qsgmiie_serdes_defs.h>
#include <phymod/chip/qsgmiie.h>
#include "../tier1/tqmod_cfg_seq.h"
#include "../../tsce/tier1/temod_enum_defines.h"
#include "../../tsce/tier1/temod.h"
#include "../../tsce/tier1/tePCSRegEnums.h"
#include "../../eagle/tier1/eagle_cfg_seq.h"
#include "../../eagle/tier1/eagle_tsc_interface.h"
#include "../../eagle/tier1/eagle_tsc_dependencies.h"
#include "../../../include/phymod/chip/eagle.h"


#define QSGMIIE_ID0             0x0143
#define QSGMIIE_ID1             0xbff0
#define QSGMIIE_REV_MASK        0x0
#define TSCE4_MODEL             0x12

#define PHY_REG_ADDR_DEVAD(_phy_reg)                        (((_phy_reg) >> 27) & 0x1f)
#define QSGMII_REG_ADDR_REG(_phy_reg)                       ((((_phy_reg) & 0x8000) >> 11) | ((_phy_reg) & 0xf))
#define IS_QSGMII_REGISTER(_phy_reg)                        (((_phy_reg) & 0xf800f000) == 0x8000)

#define QSGMII_MODEL            0x07
#define EAGLE_MODEL             0x1a

#define TSCE_NOF_LANES_IN_CORE (4)
#define TSCE_PHY_ALL_LANES (0xf)
#define TSCE_CORE_TO_PHY_ACCESS(_phy_access, _core_access) \
    do{\
        PHYMOD_MEMCPY(&(_phy_access)->access, &(_core_access)->access, sizeof((_phy_access)->access));\
        (_phy_access)->type           = (_core_access)->type; \
        (_phy_access)->port_loc       = (_core_access)->port_loc; \
        (_phy_access)->device_op_mode = (_core_access)->device_op_mode; \
        (_phy_access)->access.lane_mask = TSCE_PHY_ALL_LANES; \
    }while(0)

/* uController's firmware */
extern unsigned char tsce_ucode[];
extern unsigned short tsce_ucode_ver;
extern unsigned short tsce_ucode_crc;
extern unsigned short tsce_ucode_len;

int qsgmiie_core_identify(const phymod_core_access_t* core, uint32_t core_id, uint32_t* is_identified)
{
    int ioerr = 0;
    const phymod_access_t *pm_acc = &core->access;
    PHYID2r_t id2;
    PHYID3r_t id3;
    MAIN0_SERDESIDr_t serdesid;
    uint32_t model;

    *is_identified = 0;

    if(core_id == 0){
        ioerr += READ_PHYID2r(pm_acc, &id2);
        ioerr += READ_PHYID3r(pm_acc, &id3);
    }
    else{
        PHYID2r_SET(id2, ((core_id >> 16) & 0xffff));
        PHYID3r_SET(id3, core_id & 0xffff);
    }

    if (PHYID2r_GET(id2) == QSGMIIE_ID0 &&
        (PHYID3r_GET(id3) &= ~QSGMIIE_REV_MASK) == QSGMIIE_ID1) {
        /* PHY IDs match - now check PCS model */
        ioerr += READ_MAIN0_SERDESIDr(pm_acc, &serdesid);
        model = MAIN0_SERDESIDr_MODEL_NUMBERf_GET(serdesid);
        if (model == TSCE4_MODEL)  {
                *is_identified = 1;
        }
    }
        
    return ioerr ? PHYMOD_E_IO : PHYMOD_E_NONE;    
    return PHYMOD_E_NONE;
    
}


int qsgmiie_core_info_get(const phymod_core_access_t* core, phymod_core_info_t* info)
{
        
    
    /* Place your code here */

        
    return PHYMOD_E_UNAVAIL;
    
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


int qsgmiie_core_lane_map_set(const phymod_core_access_t* core, const phymod_lane_map_t* lane_map)
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
    PHYMOD_IF_ERR_RETURN(tqmod_pcs_lane_swap(&core->access, pcs_swap));

    PHYMOD_IF_ERR_RETURN
        (temod_pmd_addr_lane_swap(&core->access, addr_index_swap));
    PHYMOD_IF_ERR_RETURN
        (temod_pmd_lane_swap_tx(&core->access, pmd_tx_map));
    return PHYMOD_E_NONE;
}

int qsgmiie_core_lane_map_get(const phymod_core_access_t* core, phymod_lane_map_t* lane_map)
{
    uint32_t pmd_swap = 0 , pcs_swap = 0, lane;
    PHYMOD_IF_ERR_RETURN(tqmod_pcs_lane_swap_get(&core->access, &pcs_swap));
    PHYMOD_IF_ERR_RETURN(temod_pmd_lane_swap_tx_get(&core->access, &pmd_swap));
    for( lane = 0 ; lane < TSCE_NOF_LANES_IN_CORE ; lane++){
        lane_map->lane_map_rx[lane] = (pcs_swap>>(lane*4)) & 3;
        lane_map->lane_map_tx[lane] = (pmd_swap>>(lane_map->lane_map_rx[lane]*4)) & 3;
    }
    lane_map->num_of_lanes = TSCE_NOF_LANES_IN_CORE;

    return PHYMOD_E_NONE;
    
}


int qsgmiie_core_reset_set(const phymod_core_access_t* core, phymod_reset_mode_t reset_mode, phymod_reset_direction_t direction)
{
        
    
    /* Place your code here */

        
    return PHYMOD_E_UNAVAIL;
    
}

int qsgmiie_core_reset_get(const phymod_core_access_t* core, phymod_reset_mode_t reset_mode, phymod_reset_direction_t* direction)
{
        
    
    /* Place your code here */

        
    return PHYMOD_E_UNAVAIL;
    
}


int qsgmiie_core_firmware_info_get(const phymod_core_access_t* core, phymod_core_firmware_info_t* fw_info)
{
        
    
    fw_info->fw_crc = tsce_ucode_crc;
    fw_info->fw_version = tsce_ucode_ver;
    return PHYMOD_E_NONE;    
}

/* load tsce fw. the fw_loader parameter is valid just for external fw load */
STATIC
int _qsgmiie_core_firmware_load(const phymod_core_access_t* core, phymod_firmware_load_method_t load_method, phymod_firmware_loader_f fw_loader)
{
    phymod_core_firmware_info_t actual_fw;

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
    if(load_method != phymodFirmwareLoadMethodNone){
        PHYMOD_IF_ERR_RETURN(qsgmiie_core_firmware_info_get(core, &actual_fw));
        if((tsce_ucode_crc != actual_fw.fw_crc) || (tsce_ucode_ver != actual_fw.fw_version)){
            PHYMOD_RETURN_WITH_ERR(PHYMOD_E_CONFIG, (_PHYMOD_MSG("fw load validation was failed")));
        }
    }
    return PHYMOD_E_NONE;
}

int qsgmiie_phy_firmware_core_config_set(const phymod_phy_access_t* phy, phymod_firmware_core_config_t fw_config)
{
    struct eagle_tsc_uc_core_config_st serdes_firmware_core_config;
    /* coverity fix: deferencing &serdes_firmware_core_config in sizeof() 
     * so that entire struct is set to value 0 by PHYMOD_MEMSET 
     */
    PHYMOD_MEMSET(&serdes_firmware_core_config, 0, sizeof(*&serdes_firmware_core_config));
    serdes_firmware_core_config.field.core_cfg_from_pcs = fw_config.CoreConfigFromPCS;
    serdes_firmware_core_config.field.vco_rate = fw_config.VcoRate; 
    PHYMOD_IF_ERR_RETURN(eagle_tsc_set_uc_core_config(&phy->access, serdes_firmware_core_config));
    return PHYMOD_E_NONE;
}

int qsgmiie_phy_firmware_core_config_get(const phymod_phy_access_t* phy, phymod_firmware_core_config_t* fw_config)
{
    struct eagle_tsc_uc_core_config_st serdes_firmware_core_config;
    PHYMOD_IF_ERR_RETURN(eagle_tsc_get_uc_core_config(&phy->access, &serdes_firmware_core_config));
    /* coverity fix: deferencing pointer fw_config in sizeof()
     * so that entire struct is set to value 0 by PHYMOD_MEMSET
     */
    PHYMOD_MEMSET(fw_config, 0, sizeof(*fw_config));
    fw_config->CoreConfigFromPCS = serdes_firmware_core_config.field.core_cfg_from_pcs;
    fw_config->VcoRate = serdes_firmware_core_config.field.vco_rate;
    return PHYMOD_E_NONE; 
}

int qsgmiie_phy_firmware_lane_config_set(const phymod_phy_access_t* phy, phymod_firmware_lane_config_t fw_config)
{
    struct eagle_tsc_uc_lane_config_st serdes_firmware_config;

    PHYMOD_MEMSET(&serdes_firmware_config, 0x0, sizeof(serdes_firmware_config));

    serdes_firmware_config.field.lane_cfg_from_pcs = fw_config.LaneConfigFromPCS;
    serdes_firmware_config.field.an_enabled        = fw_config.AnEnabled;
    serdes_firmware_config.field.dfe_on            = fw_config.DfeOn; 
    serdes_firmware_config.field.force_brdfe_on    = fw_config.ForceBrDfe;
    serdes_firmware_config.field.cl72_auto_polarity_en = fw_config.Cl72AutoPolEn;
    serdes_firmware_config.field.cl72_restart_timeout_en = fw_config.Cl72RestTO;
    serdes_firmware_config.field.scrambling_dis    = fw_config.ScramblingDisable;
    serdes_firmware_config.field.unreliable_los    = fw_config.UnreliableLos;
    serdes_firmware_config.field.media_type        = fw_config.MediaType;
    PHYMOD_IF_ERR_RETURN(eagle_tsc_set_uc_lane_cfg(&phy->access, serdes_firmware_config));
    return PHYMOD_E_NONE;
}

int qsgmiie_phy_firmware_lane_config_get(const phymod_phy_access_t* phy, phymod_firmware_lane_config_t* fw_config)
{
    struct eagle_tsc_uc_lane_config_st serdes_firmware_config;
    PHYMOD_IF_ERR_RETURN(eagle_tsc_get_uc_lane_cfg(&phy->access, &serdes_firmware_config));
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


int qsgmiie_core_pll_sequencer_restart(const phymod_core_access_t* core, uint32_t flags, phymod_sequencer_operation_t operation)
{
        
    
    /* Place your code here */

        
    return PHYMOD_E_UNAVAIL;
    
}


int qsgmiie_core_wait_event(const phymod_core_access_t* core, phymod_core_event_t event, uint32_t timeout)
{
        
    
    /* Place your code here */

        
    return PHYMOD_E_UNAVAIL;
    
}


int qsgmiie_phy_rx_restart(const phymod_phy_access_t* phy)
{
    return PHYMOD_E_UNAVAIL;
}


int qsgmiie_phy_polarity_set(const phymod_phy_access_t* phy, const phymod_polarity_t* polarity)
{
        
    PHYMOD_IF_ERR_RETURN
        (tqmod_tx_rx_polarity_set(&phy->access, polarity->tx_polarity, polarity->rx_polarity));

    return PHYMOD_E_NONE;
    
}

int qsgmiie_phy_polarity_get(const phymod_phy_access_t* phy, phymod_polarity_t* polarity)
{
        
     PHYMOD_IF_ERR_RETURN
         (tqmod_tx_rx_polarity_get(&phy->access, &polarity->tx_polarity, &polarity->rx_polarity));
     return PHYMOD_E_NONE;
    
}


int qsgmiie_phy_tx_set(const phymod_phy_access_t* phy, const phymod_tx_t* tx)
{
    phymod_phy_access_t pm_phy_copy;
    int start_lane, num_lane;

    PHYMOD_MEMCPY(&pm_phy_copy, phy, sizeof(pm_phy_copy));
    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(&phy->access, &start_lane, &num_lane));

    pm_phy_copy.access.lane_mask = 0x1 << (start_lane/4);

    
    PHYMOD_IF_ERR_RETURN
        (eagle_tsc_write_tx_afe(&pm_phy_copy.access, TX_AFE_PRE, (int8_t)tx->pre));
    PHYMOD_IF_ERR_RETURN
        (eagle_tsc_write_tx_afe(&pm_phy_copy.access, TX_AFE_MAIN, (int8_t)tx->main));
    PHYMOD_IF_ERR_RETURN
        (eagle_tsc_write_tx_afe(&pm_phy_copy.access, TX_AFE_POST1, (int8_t)tx->post));
    PHYMOD_IF_ERR_RETURN
        (eagle_tsc_write_tx_afe(&pm_phy_copy.access, TX_AFE_POST2, (int8_t)tx->post2));
    PHYMOD_IF_ERR_RETURN
        (eagle_tsc_write_tx_afe(&pm_phy_copy.access, TX_AFE_POST3, (int8_t)tx->post3));
    PHYMOD_IF_ERR_RETURN
        (eagle_tsc_write_tx_afe(&pm_phy_copy.access, TX_AFE_AMP,  (int8_t)tx->amp));
    if((tx->drivermode != -1) &&
       (phy->device_op_mode & PHYMOD_INTF_CONFIG_TX_FIR_DRIVERMODE_ENABLE)) {
        PHYMOD_IF_ERR_RETURN
            (eagle_tsc_write_tx_afe(&pm_phy_copy.access, TX_AFE_DRIVERMODE,  (int8_t)tx->drivermode));
    }
        
    return PHYMOD_E_NONE;
    
}

int qsgmiie_phy_tx_get(const phymod_phy_access_t* phy, phymod_tx_t* tx)
{
    phymod_phy_access_t pm_phy_copy;
    int start_lane, num_lane;
    int8_t value = 0;

    PHYMOD_MEMCPY(&pm_phy_copy, phy, sizeof(pm_phy_copy));
    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(&phy->access, &start_lane, &num_lane));

    pm_phy_copy.access.lane_mask = 0x1 << (start_lane/4);

    PHYMOD_IF_ERR_RETURN
        (eagle_tsc_read_tx_afe(&pm_phy_copy.access, TX_AFE_PRE, &value));
    tx->pre = value;
    PHYMOD_IF_ERR_RETURN
        (eagle_tsc_read_tx_afe(&pm_phy_copy.access, TX_AFE_MAIN, &value));
    tx->main = value;
    PHYMOD_IF_ERR_RETURN
        (eagle_tsc_read_tx_afe(&pm_phy_copy.access, TX_AFE_POST1, &value));
    tx->post = value;
    PHYMOD_IF_ERR_RETURN
        (eagle_tsc_read_tx_afe(&pm_phy_copy.access, TX_AFE_POST2, &value));
    tx->post2 = value;
    PHYMOD_IF_ERR_RETURN
        (eagle_tsc_read_tx_afe(&pm_phy_copy.access, TX_AFE_POST3, &value));
    tx->post3 = value;
    PHYMOD_IF_ERR_RETURN
        (eagle_tsc_read_tx_afe(&pm_phy_copy.access, TX_AFE_AMP, &value));
    tx->amp = value;
    PHYMOD_IF_ERR_RETURN
        (eagle_tsc_read_tx_afe(&pm_phy_copy.access, TX_AFE_DRIVERMODE, &value));
    tx->drivermode = value;

    return PHYMOD_E_NONE;
    
}


int qsgmiie_phy_tx_override_set(const phymod_phy_access_t* phy, const phymod_tx_override_t* tx_override)
{
        
    
    /* Place your code here */

        
    return PHYMOD_E_UNAVAIL;
    
}

int qsgmiie_phy_tx_override_get(const phymod_phy_access_t* phy, phymod_tx_override_t* tx_override)
{
        
    
    /* Place your code here */

        
    return PHYMOD_E_UNAVAIL;
    
}


int qsgmiie_phy_rx_set(const phymod_phy_access_t* phy, const phymod_rx_t* rx)
{
        
    
    /* Place your code here */

        
    return PHYMOD_E_UNAVAIL;
    
}

int qsgmiie_phy_rx_get(const phymod_phy_access_t* phy, phymod_rx_t* rx)
{
        
    
    /* Place your code here */

        
    return PHYMOD_E_UNAVAIL;
    
}


int qsgmiie_phy_reset_set(const phymod_phy_access_t* phy, const phymod_phy_reset_t* reset)
{
        
    
    /* Place your code here */

        
    return PHYMOD_E_UNAVAIL;
    
}

int qsgmiie_phy_reset_get(const phymod_phy_access_t* phy, phymod_phy_reset_t* reset)
{
        
    
    /* Place your code here */

        
    return PHYMOD_E_UNAVAIL;
    
}


int qsgmiie_phy_power_set(const phymod_phy_access_t* phy, const phymod_phy_power_t* power)
{
        
        
    phymod_phy_access_t pm_phy_copy;
    int start_lane, num_lane;

    PHYMOD_MEMCPY(&pm_phy_copy, phy, sizeof(pm_phy_copy));
    /*next program the tx fir taps and driver current based on the input*/
    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(&phy->access, &start_lane, &num_lane));

    if ((power->tx == phymodPowerOff) && (power->rx == phymodPowerOff)) {
        PHYMOD_IF_ERR_RETURN(tqmod_tx_squelch_set(&pm_phy_copy.access, 1));
        PHYMOD_IF_ERR_RETURN(tqmod_rx_squelch_set(&pm_phy_copy.access, 1));
    }
    if ((power->tx == phymodPowerOn) && (power->rx == phymodPowerOn)) {
        PHYMOD_IF_ERR_RETURN(tqmod_tx_squelch_set(&pm_phy_copy.access, 0));
        PHYMOD_IF_ERR_RETURN(tqmod_rx_squelch_set(&pm_phy_copy.access, 0));
    }

    pm_phy_copy.access.lane_mask = 0x1 << (start_lane/4);
    if ((power->tx == phymodPowerOff) && (power->rx == phymodPowerNoChange)) {
            /*disable tx on the PMD side */
            PHYMOD_IF_ERR_RETURN(eagle_tsc_tx_disable(&pm_phy_copy.access, 1));
    }
    if ((power->tx == phymodPowerOn) && (power->rx == phymodPowerNoChange)) {
            /*enable tx on the PMD side */
            PHYMOD_IF_ERR_RETURN(eagle_tsc_tx_disable(&pm_phy_copy.access, 0));
    }
    if ((power->tx == phymodPowerNoChange) && (power->rx == phymodPowerOff)) {
            /*disable rx on the PMD side */
            PHYMOD_IF_ERR_RETURN(temod_rx_squelch_set(&pm_phy_copy.access, 1));
    }
    if ((power->tx == phymodPowerNoChange) && (power->rx == phymodPowerOn)) {
            /*enable rx on the PMD side */
            PHYMOD_IF_ERR_RETURN(temod_rx_squelch_set(&pm_phy_copy.access, 0));
    }
        
    return PHYMOD_E_NONE;
    
}

int qsgmiie_phy_power_get(const phymod_phy_access_t* phy, phymod_phy_power_t* power)
{
    int enable;
    uint32_t lb_enable;
    phymod_phy_access_t pm_phy_copy;
    int start_lane, num_lane;

    PHYMOD_MEMCPY(&pm_phy_copy, phy, sizeof(pm_phy_copy));
    /* next program the tx fir taps and driver current based on the input */
    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(&phy->access, &start_lane, &num_lane));

    pm_phy_copy.access.lane_mask = 0x1 << (start_lane/4);

    PHYMOD_IF_ERR_RETURN(temod_rx_squelch_get(&pm_phy_copy.access, &enable));

    /* next check if PMD loopback is on */ 
    if (enable) {                           
        PHYMOD_IF_ERR_RETURN(eagle_pmd_loopback_get(&pm_phy_copy.access, &lb_enable));
        if (lb_enable) enable = 0;
    }

    power->rx = (enable == 1)? phymodPowerOff: phymodPowerOn;
    PHYMOD_IF_ERR_RETURN(temod_tx_squelch_get(&pm_phy_copy.access, &enable));
    power->tx = (enable == 1)? phymodPowerOff: phymodPowerOn;
    return PHYMOD_E_NONE;
}

int qsgmiie_phy_tx_lane_control_set(const phymod_phy_access_t* phy, phymod_phy_tx_lane_control_t tx_control)
{
    switch (tx_control) {
    case phymodTxTrafficDisable:
        PHYMOD_IF_ERR_RETURN(tqmod_tx_lane_control_set(&phy->access, TEMOD_TX_LANE_TRAFFIC_DISABLE));
        break;
    case phymodTxTrafficEnable:
        PHYMOD_IF_ERR_RETURN(tqmod_tx_lane_control_set(&phy->access, TEMOD_TX_LANE_TRAFFIC_ENABLE));
        break;
    case phymodTxReset:
        PHYMOD_IF_ERR_RETURN(tqmod_tx_lane_control_set(&phy->access, TEMOD_TX_LANE_RESET));
        break;
    case phymodTxSquelchOn:
        PHYMOD_IF_ERR_RETURN(tqmod_tx_squelch_set(&phy->access, 1));
        break;
    case phymodTxSquelchOff:
        PHYMOD_IF_ERR_RETURN(tqmod_tx_squelch_set(&phy->access, 0));
        break;
    default:
        break;
    }
    return PHYMOD_E_NONE;
    
}


int qsgmiie_phy_tx_lane_control_get(const phymod_phy_access_t* phy, phymod_phy_tx_lane_control_t* tx_control)
{
    int enable, reset, tx_lane;
    uint32_t lb_enable;
    phymod_phy_access_t pm_phy_copy;
    int start_lane, num_lane;

    PHYMOD_MEMCPY(&pm_phy_copy, phy, sizeof(pm_phy_copy));
    /* next program the tx fir taps and driver current based on the input */
    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(&phy->access, &start_lane, &num_lane));

    PHYMOD_IF_ERR_RETURN(tqmod_tx_squelch_get(&phy->access, &enable));

    pm_phy_copy.access.lane_mask = 0x1 << (start_lane/4);
    /* next check if PMD loopback is on */ 
    if (enable) {                           
        PHYMOD_IF_ERR_RETURN(eagle_pmd_loopback_get(&pm_phy_copy.access, &lb_enable));
        if (lb_enable) enable = 0;
    }

    if(enable) {
        *tx_control = phymodTxSquelchOn;
    } else {
        PHYMOD_IF_ERR_RETURN(tqmod_tx_lane_control_get(&phy->access, &reset, &tx_lane));
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
int qsgmiie_phy_rx_lane_control_set(const phymod_phy_access_t* phy, phymod_phy_rx_lane_control_t rx_control)
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
        PHYMOD_IF_ERR_RETURN(tqmod_rx_lane_control_set(&phy->access, 1));
        break;
    case phymodRxSquelchOn:
        for (i = 0; i < num_lane; i++) {
            if (!PHYMOD_LANEPBMP_MEMBER(phy->access.lane_mask, start_lane + i)) {
                continue;
            }
            pm_phy_copy.access.lane_mask = 1 << (start_lane + i);
            PHYMOD_IF_ERR_RETURN(tqmod_rx_squelch_set(&pm_phy_copy.access, 1));
        }
        break;
    case phymodRxSquelchOff:
        for (i = 0; i < num_lane; i++) {
            if (!PHYMOD_LANEPBMP_MEMBER(phy->access.lane_mask, start_lane + i)) {
                continue;
            }
            pm_phy_copy.access.lane_mask = 1 << (start_lane + i);
            PHYMOD_IF_ERR_RETURN(tqmod_rx_squelch_set(&pm_phy_copy.access, 0));
        }
        break;
    default:
        break;
    }
    return PHYMOD_E_NONE;
}

int qsgmiie_phy_rx_lane_control_get(const phymod_phy_access_t* phy, phymod_phy_rx_lane_control_t* rx_control)
{
    int enable, reset;
    uint32_t lb_enable;
    phymod_phy_access_t pm_phy_copy;
    int start_lane, num_lane;

    PHYMOD_MEMCPY(&pm_phy_copy, phy, sizeof(pm_phy_copy));
    /* next program the tx fir taps and driver current based on the input */
    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(&phy->access, &start_lane, &num_lane));

    PHYMOD_IF_ERR_RETURN(tqmod_rx_squelch_get(&phy->access, &enable));

    pm_phy_copy.access.lane_mask = 0x1 << (start_lane/4);
    /* next check if PMD loopback is on */ 
    if (enable) {                           
        PHYMOD_IF_ERR_RETURN(eagle_pmd_loopback_get(&pm_phy_copy.access, &lb_enable));
        if (lb_enable) enable = 0;
    }
    if(enable) {
        *rx_control = phymodRxSquelchOn;
    } else {
        PHYMOD_IF_ERR_RETURN(tqmod_rx_lane_control_get(&phy->access, &reset));
        if (reset == 0) {
            *rx_control = phymodRxReset;
        } else {
            *rx_control = phymodRxSquelchOff;
        }
    }
    return PHYMOD_E_NONE;
}

STATIC
int _qsgmiie_speed_id_interface_config_get(const phymod_phy_access_t* phy, int speed_id, phymod_phy_inf_config_t* config)
{
    switch (speed_id) {
    case 0x0:
        config->data_rate = 10;
        break;
    case 0x1:
        config->data_rate = 100;
        break;
    case 0x2:
        config->data_rate = 1000;
        break;
    default:
        config->data_rate = 0;
        break;
    }
    config->interface_type = phymodInterfaceSGMII;

    return PHYMOD_E_NONE;
}


int qsgmiie_phy_interface_config_set(const phymod_phy_access_t* phy, uint32_t flags, const phymod_phy_inf_config_t* config)
{

    PHYMOD_IF_ERR_RETURN
        (tqmod_set_spd_intf(&phy->access, config->data_rate));

    return PHYMOD_E_NONE;

}

int qsgmiie_phy_interface_config_get(const phymod_phy_access_t* phy, 
                                     uint32_t flags,
                                     phymod_ref_clk_t ref_clock, 
                                     phymod_phy_inf_config_t* config)
{
    int speed_id;

    config->ref_clock = ref_clock;

    PHYMOD_IF_ERR_RETURN
        (tqmod_speed_id_get(&phy->access, &speed_id));

    PHYMOD_IF_ERR_RETURN
        (_qsgmiie_speed_id_interface_config_get(phy, speed_id, config));         

    return PHYMOD_E_NONE;
}

int qsgmiie_phy_autoneg_ability_set(const phymod_phy_access_t* phy, const phymod_autoneg_ability_t* an_ability)
{
    tqmod_an_ability_t value; 
    int start_lane, num_lane;
    phymod_phy_access_t phy_copy;

    /* next program the tx fir taps and driver current based on the input */
    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(&phy->access, &start_lane, &num_lane));

    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));
    phy_copy.access.lane_mask = 0x1 << start_lane;

    PHYMOD_MEMSET(&value, 0x0, sizeof(value));

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
    if (PHYMOD_AN_CAP_SYMM_PAUSE_GET(an_ability)) {
        value.cl37_adv.an_pause = TEMOD_SYMM_PAUSE;
    } else if (PHYMOD_AN_CAP_ASYM_PAUSE_GET(an_ability)) {
        value.cl37_adv.an_pause = TEMOD_ASYM_PAUSE;
    }


    return PHYMOD_E_NONE;

}

int qsgmiie_phy_autoneg_ability_get(const phymod_phy_access_t* phy, phymod_autoneg_ability_t* an_ability_get_type)
{
        
    tqmod_an_ability_t value;
    phymod_phy_access_t phy_copy;
    int start_lane, num_lane;

    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(&phy->access, &start_lane, &num_lane));
    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));
    phy_copy.access.lane_mask = 0x1 << start_lane;
    PHYMOD_MEMSET(&value, 0x0, sizeof(value));
     
/*    PHYMOD_IF_ERR_RETURN
        (tqmod_autoneg_local_ability_get(&phy_copy.access, &value));*/
    value.cl37_adv.cl37_sgmii_speed  = TEMOD_CL37_SGMII_10M |
                                  TEMOD_CL37_SGMII_100M |
                                  TEMOD_CL37_SGMII_1000M;
    value.cl37_adv.an_pause = TEMOD_NO_PAUSE | TEMOD_ASYM_PAUSE;
    switch (value.cl37_adv.an_pause) {
    case TEMOD_ASYM_PAUSE:
        PHYMOD_AN_CAP_ASYM_PAUSE_SET(an_ability_get_type);
        break;
    case TEMOD_SYMM_PAUSE:
        PHYMOD_AN_CAP_SYMM_PAUSE_SET(an_ability_get_type);
        break;
    default:
        break;
    } 
    return PHYMOD_E_NONE;
    
}


int qsgmiie_phy_autoneg_set(const phymod_phy_access_t* phy, const phymod_autoneg_control_t* an)
{
        
    phymod_firmware_lane_config_t firmware_lane_config;
    int start_lane, num_lane;
    phymod_phy_access_t phy_copy;
    tqmod_an_control_t an_control;
    int enable;

    /* next program the tx fir taps and driver current based on the input */
    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(&phy->access, &start_lane, &num_lane));

    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));
    phy_copy.access.lane_mask = 0x1 << start_lane;

    an_control.enable       = an->enable;
    an_control.an_property_type = 0x0;   /* for now disable */  
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
    default:
        return PHYMOD_E_PARAM;
    }
    enable = an_control.enable;

    /* make sure the firmware config is set to an eenabled */
    phy_copy.access.lane_mask = 0x1 << (start_lane / 4);
    PHYMOD_IF_ERR_RETURN
        (eagle_lane_soft_reset_release(&phy_copy.access, 0));


    PHYMOD_IF_ERR_RETURN
        (qsgmiie_phy_firmware_lane_config_get(&phy_copy, &firmware_lane_config));
    if (an->enable) {
        firmware_lane_config.AnEnabled = 1;
        firmware_lane_config.LaneConfigFromPCS = 1;
    } else {
        firmware_lane_config.AnEnabled = 0;
        firmware_lane_config.LaneConfigFromPCS = 0;
    }
    PHYMOD_IF_ERR_RETURN
        (qsgmiie_phy_firmware_lane_config_set(&phy_copy, firmware_lane_config));

    PHYMOD_IF_ERR_RETURN
        (eagle_lane_soft_reset_release(&phy_copy.access, 1));

    phy_copy.access.lane_mask = 0x1 << start_lane;
    PHYMOD_IF_ERR_RETURN
        (tqmod_autoneg_set(&phy_copy.access, &enable));       

    return PHYMOD_E_NONE;
    
}

int qsgmiie_phy_autoneg_get(const phymod_phy_access_t* phy, phymod_autoneg_control_t* an, uint32_t* an_done)
{
        
    tqmod_an_control_t an_control;
    phymod_phy_access_t phy_copy;
    int start_lane, num_lane; 
/*    int an_complete = 0;*/

    /* next program the tx fir taps and driver current based on the input */
    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(&phy->access, &start_lane, &num_lane));

    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));
    phy_copy.access.lane_mask = 0x1 << start_lane;

    PHYMOD_MEMSET(&an_control, 0x0,  sizeof(tqmod_an_control_t));
/*    PHYMOD_IF_ERR_RETURN
        (temod_autoneg_control_get(&phy_copy.access, &an_control, &an_complete));*/
    
    if (an_control.enable) {
        an->enable = 1;
        *an_done = 0; 
    } else {
        an->enable = 0;
    }                
    return PHYMOD_E_NONE;
}

int qsgmiie_core_init(const phymod_core_access_t* core, const phymod_core_init_config_t* init_config, const phymod_core_status_t* core_status)
{
    phymod_phy_access_t phy_access, phy_access_copy;
    phymod_core_access_t  core_copy;
    phymod_firmware_core_config_t  firmware_core_config_tmp;
#if 0
    temod_an_init_t an; 
#endif

    TSCE_CORE_TO_PHY_ACCESS(&phy_access, core);
    phy_access_copy = phy_access;
    PHYMOD_MEMCPY(&core_copy, core, sizeof(core_copy));
    core_copy.access.lane_mask = 0x1;
        phy_access_copy = phy_access;
        phy_access_copy.access = core->access;
        phy_access_copy.access.lane_mask = 0x1;
        phy_access_copy.type = core->type;

    PHYMOD_IF_ERR_RETURN
        (temod_pmd_reset_seq(&core_copy.access, core_status->pmd_active));

    /* need to set the heart beat default is for 156.25M */
    if (init_config->interface.ref_clock == phymodRefClk125Mhz) {
        /* we need to set the ref clock config differently */
    }
    if (_qsgmiie_core_firmware_load(&core_copy, init_config->firmware_load_method, init_config->firmware_loader)) {
        PHYMOD_DEBUG_ERROR(("devad 0x%"PRIx32" lane 0x%"PRIx32": UC firmware-load failed\n", core->access.addr, core->access.lane_mask));  
        PHYMOD_IF_ERR_RETURN (PHYMOD_E_INIT);
    }
    /* next we need to check if the load is correct or not */
    if (eagle_tsc_ucode_load_verify(&core_copy.access, (uint8_t *) &tsce_ucode, tsce_ucode_len)) {
        PHYMOD_DEBUG_ERROR(("devad 0x%"PRIx32" lane 0x%"PRIx32": UC load-verify failed\n", core->access.addr, core->access.lane_mask));  
        PHYMOD_IF_ERR_RETURN (PHYMOD_E_INIT);
    }

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

    /* poll the ready bit in 10 ms */
    PHYMOD_IF_ERR_RETURN
        (eagle_tsc_poll_uc_dsc_ready_for_cmd_equals_1(&phy_access_copy.access, 1));

    PHYMOD_IF_ERR_RETURN(
        eagle_pmd_ln_h_rstb_pkill_override( &phy_access_copy.access, 0x0));

    /* plldiv CONFIG */
    PHYMOD_IF_ERR_RETURN
        (eagle_pll_mode_set(&core->access, 0x9));

    /* now config the lane mapping and polarity */
    PHYMOD_IF_ERR_RETURN
        (qsgmiie_core_lane_map_set(core, &init_config->lane_map));
#if 0
  /* NEED TO REVISIT */
    PHYMOD_IF_ERR_RETURN
        (temod_autoneg_set_init(&core->access, &an));
#endif
/*    PHYMOD_IF_ERR_RETURN
        (temod_autoneg_timer_init(&core->access));*/
    PHYMOD_IF_ERR_RETURN
        (temod_master_port_num_set(&core->access, 0));
    /* don't overide the fw that set in config set if not specified */
    firmware_core_config_tmp = init_config->firmware_core_config;
    firmware_core_config_tmp.CoreConfigFromPCS = 0;
    /* set the vco rate to be default at 10.0000G */
    firmware_core_config_tmp.VcoRate = 0x12;

    PHYMOD_IF_ERR_RETURN
        (qsgmiie_phy_firmware_core_config_set(&phy_access_copy, firmware_core_config_tmp)); 

    /* release core soft reset */
    PHYMOD_IF_ERR_RETURN
        (eagle_core_soft_reset_release(&core_copy.access, 1));
        
    return PHYMOD_E_NONE;
        
}


int qsgmiie_phy_init(const phymod_phy_access_t* phy, const phymod_phy_init_config_t* init_config)
{
        
    const phymod_access_t *pm_acc = &phy->access;
    phymod_phy_access_t pm_phy_copy;
    int start_lane, num_lane, i;
    int an_en=init_config->an_en;
    int lane_bkup;
    phymod_polarity_t tmp_pol;

    PHYMOD_MEMSET(&tmp_pol, 0x0, sizeof(tmp_pol));
    PHYMOD_MEMCPY(&pm_phy_copy, phy, sizeof(pm_phy_copy));

    /* next program the tx fir taps and driver current based on the input */
    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(pm_acc, &start_lane, &num_lane));

    /* Enable autoneg for QSGMII pcs*/
    PHYMOD_IF_ERR_RETURN
       (tqmod_autoneg_set(&pm_phy_copy.access, &an_en)); 

   /* exit if lane number >4 (for pmd and tsce)*/
   if(start_lane>4) 
      return PHYMOD_E_NONE;
    /* per lane based reset release */
    PHYMOD_IF_ERR_RETURN
        (tqmod_pmd_x4_reset(pm_acc));

    /* poll for per lane uc_dsc_ready */
    lane_bkup = pm_phy_copy.access.lane_mask;
    for (i = 0; i < num_lane; i++) {
        pm_phy_copy.access.lane_mask = 1 << (start_lane + i);
        PHYMOD_IF_ERR_RETURN
            (eagle_lane_soft_reset_release(&pm_phy_copy.access, 1));
    }
    pm_phy_copy.access.lane_mask = lane_bkup;

    /* program the rx/tx polarity */
    for (i = 0; i < num_lane; i++) {
        pm_phy_copy.access.lane_mask = 0x1 << (i + start_lane);
        tmp_pol.tx_polarity = (init_config->polarity.tx_polarity) >> i & 0x1;
        tmp_pol.rx_polarity = (init_config->polarity.rx_polarity) >> i & 0x1;
        PHYMOD_IF_ERR_RETURN
            (qsgmiie_phy_polarity_set(&pm_phy_copy, &tmp_pol));
    }

    for (i = 0; i < num_lane; i++) {
        pm_phy_copy.access.lane_mask = 0x1 << (i + start_lane);
        PHYMOD_IF_ERR_RETURN
            (qsgmiie_phy_tx_set(&pm_phy_copy, &init_config->tx[i]));
    }

    pm_phy_copy.access.lane_mask = 0x1;

    /*PHYMOD_IF_ERR_RETURN
        (temod_update_port_mode(pm_acc, &pll_restart));*/

    PHYMOD_IF_ERR_RETURN
        (tqmod_rx_lane_control_set(pm_acc, 1));
    PHYMOD_IF_ERR_RETURN
        (tqmod_tx_lane_control_set(pm_acc, TQMOD_TX_LANE_RESET_TRAFFIC_ENABLE));         /* TX_LANE_CONTROL */

        
    for (i = 0; i < num_lane; i++) {
        pm_phy_copy.access.lane_mask = 0x1 << (i + start_lane);
    PHYMOD_IF_ERR_RETURN
        (tqmod_txfir_tx_disable_set(&pm_phy_copy.access));

    PHYMOD_IF_ERR_RETURN
        (tqmod_pmd_osmode_set(&pm_phy_copy.access, 0x1));

    PHYMOD_IF_ERR_RETURN
        (tqmod_init_pcs_ilkn(&pm_phy_copy.access));

    }
        
    return PHYMOD_E_NONE;
    
}


int qsgmiie_phy_loopback_set(const phymod_phy_access_t* phy, phymod_loopback_mode_t loopback, uint32_t enable)
{
    int start_lane, num_lane;
    int rv = PHYMOD_E_NONE;
    phymod_phy_access_t phy_copy;

    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));

    /* next figure out the lane num and start_lane based on the input */
    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(&phy->access, &start_lane, &num_lane));

    switch (loopback) {
    case phymodLoopbackGlobal :
    case phymodLoopbackGlobalPMD :
        phy_copy.access.lane_mask = 0x1 << (start_lane / 4);
        PHYMOD_IF_ERR_RETURN(eagle_tsc_dig_lpbk(&phy_copy.access, (uint8_t) enable));
        PHYMOD_IF_ERR_RETURN(eagle_pmd_force_signal_detect(&phy_copy.access, (int) enable));
        break;
    case phymodLoopbackRemotePMD :
        phy_copy.access.lane_mask = 0x1 << (start_lane / 4);
        PHYMOD_IF_ERR_RETURN(eagle_tsc_rmt_lpbk(&phy_copy.access, (uint8_t)enable));
        break;
    case phymodLoopbackRemotePCS :
        PHYMOD_IF_ERR_RETURN(temod_rx_loopback_control(&phy->access, enable, enable, enable)); /* RAVI */
        break;
    default :
        break;
    }             
    return rv;
    return PHYMOD_E_NONE;
    
}

int qsgmiie_phy_loopback_get(const phymod_phy_access_t* phy, phymod_loopback_mode_t loopback, uint32_t* enable)
{
    int start_lane, num_lane;
    phymod_phy_access_t phy_copy;

    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));

    /* next figure out the lane num and start_lane based on the input */
    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(&phy->access, &start_lane, &num_lane));

    switch (loopback) {
    case phymodLoopbackGlobal :
    case phymodLoopbackGlobalPMD :
        phy_copy.access.lane_mask = 0x1 << (start_lane / 4);
        PHYMOD_IF_ERR_RETURN(eagle_pmd_loopback_get(&phy_copy.access, enable));
        break;
    case phymodLoopbackRemotePMD :
        phy_copy.access.lane_mask = 0x1 << (start_lane / 4);
        PHYMOD_IF_ERR_RETURN(eagle_tsc_rmt_lpbk_get(&phy_copy.access, enable));
        break;
    case phymodLoopbackRemotePCS :
        /* PHYMOD_IF_ERR_RETURN(temod_rx_loopback_control(&phy->access, enable, enable, enable)); */ /* RAVI */
        break;
    default :
        break;
    } 
                
    return PHYMOD_E_NONE;
}


int qsgmiie_phy_rx_pmd_locked_get(const phymod_phy_access_t* phy, uint32_t* rx_seq_done)
{
    int start_lane, num_lane;
    phymod_phy_access_t phy_copy;

    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));

    /* next figure out the lane num and start_lane based on the input */
    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(&phy->access, &start_lane, &num_lane));
        
    phy_copy.access.lane_mask = 0x1 << (start_lane/4);
    
    PHYMOD_IF_ERR_RETURN(temod_pmd_lock_get(&phy_copy.access, rx_seq_done));

        
    return PHYMOD_E_NONE;
    
}


int qsgmiie_phy_link_status_get(const phymod_phy_access_t* phy, uint32_t* link_status)
{
        
   PHYMOD_IF_ERR_RETURN(tqmod_get_pcs_link_status(&phy->access, link_status));    
        
    return PHYMOD_E_NONE;
    
}


STATIC 
int qsgmiie_phy_reg_is_pcs_get(uint32_t reg_addr, int* is_qsgmii_pcs_reg) {

    *is_qsgmii_pcs_reg = (PHY_REG_ADDR_DEVAD(reg_addr) == 0) && 
        ((IS_QSGMII_REGISTER(reg_addr)) || (QSGMII_REG_ADDR_REG(reg_addr) < 0x10));
    return PHYMOD_E_NONE; 
}

int qsgmiie_phy_reg_read(const phymod_phy_access_t* phy, uint32_t reg_addr, uint32_t* val)
{
    int start_lane, num_lane, is_qsgmii_pcs;
    phymod_phy_access_t phy_copy;

    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));

    /* next figure out the lane num and start_lane based on the input */
    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(&phy->access, &start_lane, &num_lane));

    PHYMOD_IF_ERR_RETURN
        (qsgmiie_phy_reg_is_pcs_get(reg_addr, &is_qsgmii_pcs));

    if (!is_qsgmii_pcs) {
        phy_copy.access.lane_mask = 0x1 << (start_lane/4);
    }

    PHYMOD_IF_ERR_RETURN
        (phymod_tsc_iblk_read(&phy_copy.access, reg_addr, val));

    return PHYMOD_E_NONE;
}

int qsgmiie_phy_reg_write(const phymod_phy_access_t* phy, uint32_t reg_addr, uint32_t val)
{
    int start_lane, num_lane, is_qsgmii_pcs;
    phymod_phy_access_t phy_copy;

    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phy_copy));

    /* next figure out the lane num and start_lane based on the input */
    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(&phy->access, &start_lane, &num_lane));
        
    PHYMOD_IF_ERR_RETURN
        (qsgmiie_phy_reg_is_pcs_get(reg_addr, &is_qsgmii_pcs));

    if (!is_qsgmii_pcs) {
        phy_copy.access.lane_mask = 0x1 << (start_lane/4);
    }

    PHYMOD_IF_ERR_RETURN
        (phymod_tsc_iblk_write(&phy_copy.access, reg_addr, val));

    return PHYMOD_E_NONE;
}

int qsgmiie_phy_rx_signal_detect_get(const phymod_phy_access_t* phy, uint32_t* value)
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

int qsgmiie_phy_rx_ppm_get(const phymod_phy_access_t* phy, int16_t* rx_ppm)
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

