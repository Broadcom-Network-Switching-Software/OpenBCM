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

#ifdef PHYMOD_FURIA_SUPPORT
#include <phymod/chip/furia.h>
#include "../../furia/tier1/furia_regs_structs.h"
#include "../../furia/tier1/furia_reg_access.h"
#include "../../furia/tier1/furia_address_defines.h"
#include "../../furia/tier1/furia_micro_seq.h"
#include "../../furia/tier1/furia_types.h"
#include "../../furia/tier1/furia_cfg_seq.h"
#include "furia_82212_helper.h"

int furia_82212_core_identify(const phymod_core_access_t* core, uint32_t core_id, uint32_t* is_identified)
{
    int ioerr = 0;
    const phymod_access_t *pm_acc = &core->access;
    IEEE_PMA_PMD_BLK0_PMD_IDENTIFIER_REGISTER_0_t id2;
    IEEE_PMA_PMD_BLK0_PMD_IDENTIFIER_REGISTER_1_t id3; 
    uint32_t chip_id = 0;
    chip_id =  _furia_get_chip_id(pm_acc);

    *is_identified = 0;

    if(core_id == 0){
        ioerr += 
        READ_FURIA_PMA_PMD_REG(pm_acc,
                               IEEE_PMA_PMD_BLK0_pmd_identifier_register_0_Adr,
                               &id2.data);  
        ioerr += 
        READ_FURIA_PMA_PMD_REG(pm_acc,
                               IEEE_PMA_PMD_BLK0_pmd_identifier_register_1_Adr,
                               &id3.data);  
    } else {
        id2.data = (uint16_t) ((core_id >> 16) & 0xffff);
        id3.data = (uint16_t) core_id & 0xffff;
    }

    if (((id2.data) == FURIA_ID0 &&
        (id3.data) == FURIA_ID1) && (chip_id == FURIA_ID_82212)) {

        /* PHY IDs match */
        *is_identified = 1;
    }
        
    return ioerr ? PHYMOD_E_IO : PHYMOD_E_NONE;    
}


int furia_82212_core_info_get(const phymod_core_access_t* core, phymod_core_info_t* info)
{
    PHYMOD_IF_ERR_RETURN(furia_core_info_get(core,info)); 
    
    return PHYMOD_E_NONE;
    
}


int furia_82212_core_reset_set(const phymod_core_access_t* core, phymod_reset_mode_t reset_mode, phymod_reset_direction_t direction)
{
    phymod_core_access_t core_cpy;
    uint16_t die_idx = 0;
    PHYMOD_MEMSET(&core_cpy, 0, sizeof(phymod_core_access_t));
    PHYMOD_MEMCPY(&core_cpy, core, sizeof(phymod_core_access_t));
   
    core_cpy.access.addr &= (~1);
    for (die_idx = 0; die_idx < FURIA_82212_MAX_DIE; die_idx ++) {
        core_cpy.access.addr |= die_idx;

        PHYMOD_IF_ERR_RETURN(
           furia_core_reset_set(&core_cpy, reset_mode, direction));
    }
    
    return PHYMOD_E_NONE;
    
}

int furia_82212_core_reset_get(const phymod_core_access_t* core, phymod_reset_mode_t reset_mode, phymod_reset_direction_t* direction)
{
    return PHYMOD_E_UNAVAIL;
}


int furia_82212_core_firmware_info_get(const phymod_core_access_t* core, phymod_core_firmware_info_t* fw_info)
{
     PHYMOD_IF_ERR_RETURN(
         furia_core_firmware_info_get(core, fw_info));
    
    return PHYMOD_E_NONE;
    
}


int furia_82212_phy_firmware_core_config_set(const phymod_phy_access_t* phy, phymod_firmware_core_config_t fw_core_config)
{
    return PHYMOD_E_NONE;
    
}

int furia_82212_phy_firmware_core_config_get(const phymod_phy_access_t* phy, phymod_firmware_core_config_t* fw_core_config)
{
        
    return PHYMOD_E_NONE;
    
}

int furia_82212_phy_firmware_lane_config_set(const phymod_phy_access_t* phy, phymod_firmware_lane_config_t fw_lane_config)
{
    phymod_phy_access_t phy_cpy;
    uint16_t lane_idx = 0;
    PHYMOD_MEMSET(&phy_cpy, 0, sizeof(phymod_phy_access_t));
    PHYMOD_MEMCPY(&phy_cpy, phy, sizeof(phymod_phy_access_t));
   
    for (lane_idx = 0; lane_idx < FURIA_82212_MAX_LANE; lane_idx ++) {
        if (phy->access.lane_mask & (1 << lane_idx)) {
            FURIA_82212_CALL_PHYDRV(phy_cpy, lane_idx,
                  furia_phy_firmware_lane_config_set(&phy_cpy, fw_lane_config));
        }
    }
   
    return PHYMOD_E_NONE;
    
}

int furia_82212_phy_firmware_lane_config_get(const phymod_phy_access_t* phy, phymod_firmware_lane_config_t* fw_lane_config)
{
    phymod_phy_access_t phy_cpy;
    uint16_t lane_idx = 0;
    PHYMOD_MEMSET(&phy_cpy, 0, sizeof(phymod_phy_access_t));
    PHYMOD_MEMCPY(&phy_cpy, phy, sizeof(phymod_phy_access_t));
   
    for (lane_idx = 0; lane_idx < FURIA_82212_MAX_LANE; lane_idx ++) {
        if (phy->access.lane_mask & (1 << lane_idx)) {
            FURIA_82212_CALL_PHYDRV(phy_cpy, lane_idx,
                  furia_phy_firmware_lane_config_get(&phy_cpy, fw_lane_config));
            break;
        }
    }
        
    return PHYMOD_E_NONE;
}


int furia_82212_core_pll_sequencer_restart(const phymod_core_access_t* core, uint32_t flags, phymod_sequencer_operation_t operation)
{
    phymod_core_access_t core_cpy;
    uint16_t lane_idx = 0;
    PHYMOD_MEMSET(&core_cpy, 0, sizeof(phymod_core_access_t));
    PHYMOD_MEMCPY(&core_cpy,core, sizeof(phymod_core_access_t));
   
    for (lane_idx = 0; lane_idx < FURIA_82212_MAX_LANE; lane_idx ++) {
        if (core->access.lane_mask & (1 << lane_idx)) {
            FURIA_82212_CALL_PHYDRV(core_cpy, lane_idx,
                  furia_core_pll_sequencer_restart(&core_cpy, flags, operation));
        }
    }
        
    return PHYMOD_E_NONE;
}


int furia_82212_phy_polarity_set(const phymod_phy_access_t* phy, const phymod_polarity_t* polarity)
{
    phymod_phy_access_t phy_cpy;
    uint16_t lane_idx = 0;
    PHYMOD_MEMSET(&phy_cpy, 0, sizeof(phymod_phy_access_t));
    PHYMOD_MEMCPY(&phy_cpy, phy, sizeof(phymod_phy_access_t));
   
    for (lane_idx = 0; lane_idx < FURIA_82212_MAX_LANE; lane_idx ++) {
        if (phy->access.lane_mask & (1 << lane_idx)) {
            FURIA_82212_CALL_PHYDRV(phy_cpy, lane_idx,
                  furia_phy_polarity_set(&phy_cpy, polarity));
        }
    }
        
    return PHYMOD_E_NONE;
}

int furia_82212_phy_polarity_get(const phymod_phy_access_t* phy, phymod_polarity_t* polarity)
{
    phymod_phy_access_t phy_cpy;
    uint16_t lane_idx = 0;
    PHYMOD_MEMSET(&phy_cpy, 0, sizeof(phymod_phy_access_t));
    PHYMOD_MEMCPY(&phy_cpy, phy, sizeof(phymod_phy_access_t));
   
    for (lane_idx = 0; lane_idx < FURIA_82212_MAX_LANE; lane_idx ++) {
        if (phy->access.lane_mask & (1 << lane_idx)) {
            FURIA_82212_CALL_PHYDRV(phy_cpy, lane_idx,
                  furia_phy_polarity_get(&phy_cpy, polarity));
            break;
        }
    }
    return PHYMOD_E_NONE;
    
}


int furia_82212_phy_tx_set(const phymod_phy_access_t* phy, const phymod_tx_t* tx)
{
    phymod_phy_access_t phy_cpy;
    uint16_t lane_idx = 0;
    PHYMOD_MEMSET(&phy_cpy, 0, sizeof(phymod_phy_access_t));
    PHYMOD_MEMCPY(&phy_cpy, phy, sizeof(phymod_phy_access_t));
   
    for (lane_idx = 0; lane_idx < FURIA_82212_MAX_LANE; lane_idx ++) {
        if (phy->access.lane_mask & (1 << lane_idx)) {
            FURIA_82212_CALL_PHYDRV(phy_cpy, lane_idx,
                  furia_phy_tx_set(&phy_cpy, tx));
        }
    }
   
    return PHYMOD_E_NONE;
    
}

int furia_82212_phy_tx_get(const phymod_phy_access_t* phy, phymod_tx_t* tx)
{
    phymod_phy_access_t phy_cpy;
    uint16_t lane_idx = 0;
    PHYMOD_MEMSET(&phy_cpy, 0, sizeof(phymod_phy_access_t));
    PHYMOD_MEMCPY(&phy_cpy, phy, sizeof(phymod_phy_access_t));
   
    for (lane_idx = 0; lane_idx < FURIA_82212_MAX_LANE; lane_idx ++) {
        if (phy->access.lane_mask & (1 << lane_idx)) {
            FURIA_82212_CALL_PHYDRV(phy_cpy, lane_idx,
                  furia_phy_tx_get(&phy_cpy, tx));
            break;
        }
    }

    return PHYMOD_E_NONE;
}


int furia_82212_phy_rx_set(const phymod_phy_access_t* phy, const phymod_rx_t* rx)
{
    phymod_phy_access_t phy_cpy;
    uint16_t lane_idx = 0;
    PHYMOD_MEMSET(&phy_cpy, 0, sizeof(phymod_phy_access_t));
    PHYMOD_MEMCPY(&phy_cpy, phy, sizeof(phymod_phy_access_t));
   
    for (lane_idx = 0; lane_idx < FURIA_82212_MAX_LANE; lane_idx ++) {
        if (phy->access.lane_mask & (1 << lane_idx)) {
            FURIA_82212_CALL_PHYDRV(phy_cpy, lane_idx,
                  furia_phy_rx_set(&phy_cpy, rx));
        }
    }
        
    return PHYMOD_E_NONE;
    
}

int furia_82212_phy_rx_get(const phymod_phy_access_t* phy, phymod_rx_t* rx)
{
    phymod_phy_access_t phy_cpy;
    uint16_t lane_idx = 0;
    PHYMOD_MEMSET(&phy_cpy, 0, sizeof(phymod_phy_access_t));
    PHYMOD_MEMCPY(&phy_cpy, phy, sizeof(phymod_phy_access_t));
   
    for (lane_idx = 0; lane_idx < FURIA_82212_MAX_LANE; lane_idx ++) {
        if (phy->access.lane_mask & (1 << lane_idx)) {
            FURIA_82212_CALL_PHYDRV(phy_cpy, lane_idx,
                  furia_phy_rx_get(&phy_cpy, rx));
            break;
        }
    }
    
    return PHYMOD_E_NONE;
    
}


int furia_82212_phy_reset_set(const phymod_phy_access_t* phy, const phymod_phy_reset_t* reset)
{
    phymod_phy_access_t phy_cpy;
    uint16_t lane_idx = 0;
    PHYMOD_MEMSET(&phy_cpy, 0, sizeof(phymod_phy_access_t));
    PHYMOD_MEMCPY(&phy_cpy, phy, sizeof(phymod_phy_access_t));
   
    for (lane_idx = 0; lane_idx < FURIA_82212_MAX_LANE; lane_idx ++) {
        if (phy->access.lane_mask & (1 << lane_idx)) {
            FURIA_82212_CALL_PHYDRV(phy_cpy, lane_idx,
                  furia_phy_reset_set(&phy_cpy, reset));
        }
    }
       
    return PHYMOD_E_NONE;
    
}

int furia_82212_phy_reset_get(const phymod_phy_access_t* phy, phymod_phy_reset_t* reset)
{
    phymod_phy_access_t phy_cpy;
    uint16_t lane_idx = 0;
    PHYMOD_MEMSET(&phy_cpy, 0, sizeof(phymod_phy_access_t));
    PHYMOD_MEMCPY(&phy_cpy, phy, sizeof(phymod_phy_access_t));
   
    for (lane_idx = 0; lane_idx < FURIA_82212_MAX_LANE; lane_idx ++) {
        if (phy->access.lane_mask & (1 << lane_idx)) {
            FURIA_82212_CALL_PHYDRV(phy_cpy, lane_idx,
                  furia_phy_reset_get(&phy_cpy, reset));
            break;
        }
    }
    return PHYMOD_E_NONE;
    
}


int furia_82212_phy_power_set(const phymod_phy_access_t* phy, const phymod_phy_power_t* power)
{
    phymod_phy_access_t phy_cpy;
    uint16_t lane_idx = 0;
    PHYMOD_MEMSET(&phy_cpy, 0, sizeof(phymod_phy_access_t));
    PHYMOD_MEMCPY(&phy_cpy, phy, sizeof(phymod_phy_access_t));
   
    for (lane_idx = 0; lane_idx < FURIA_82212_MAX_LANE; lane_idx ++) {
        if (phy->access.lane_mask & (1 << lane_idx)) {
            FURIA_82212_CALL_PHYDRV(phy_cpy, lane_idx,
                  furia_phy_power_set(&phy_cpy, power));
        }
    }
       
    return PHYMOD_E_NONE;
    
}

int furia_82212_phy_power_get(const phymod_phy_access_t* phy, phymod_phy_power_t* power)
{
    phymod_phy_access_t phy_cpy;
    uint16_t lane_idx = 0;
    PHYMOD_MEMSET(&phy_cpy, 0, sizeof(phymod_phy_access_t));
    PHYMOD_MEMCPY(&phy_cpy, phy, sizeof(phymod_phy_access_t));
   
    for (lane_idx = 0; lane_idx < FURIA_82212_MAX_LANE; lane_idx ++) {
        if (phy->access.lane_mask & (1 << lane_idx)) {
            FURIA_82212_CALL_PHYDRV(phy_cpy, lane_idx,
                  furia_phy_power_get(&phy_cpy, power));
            break;
        }
    }
        
    return PHYMOD_E_NONE;
    
}


int furia_82212_phy_tx_lane_control_set(const phymod_phy_access_t* phy, phymod_phy_tx_lane_control_t tx_control)
{
    phymod_phy_access_t phy_cpy;
    uint16_t lane_idx = 0;
    PHYMOD_MEMSET(&phy_cpy, 0, sizeof(phymod_phy_access_t));
    PHYMOD_MEMCPY(&phy_cpy, phy, sizeof(phymod_phy_access_t));
   
    for (lane_idx = 0; lane_idx < FURIA_82212_MAX_LANE; lane_idx ++) {
        if (phy->access.lane_mask & (1 << lane_idx)) {
            FURIA_82212_CALL_PHYDRV(phy_cpy, lane_idx,
                  furia_phy_tx_lane_control_set(&phy_cpy, tx_control));
        }
    }
        
    return PHYMOD_E_NONE;
    
}

int furia_82212_phy_tx_lane_control_get(const phymod_phy_access_t* phy, phymod_phy_tx_lane_control_t* tx_control)
{
    phymod_phy_access_t phy_cpy;
    uint16_t lane_idx = 0;
    PHYMOD_MEMSET(&phy_cpy, 0, sizeof(phymod_phy_access_t));
    PHYMOD_MEMCPY(&phy_cpy, phy, sizeof(phymod_phy_access_t));
   
    for (lane_idx = 0; lane_idx < FURIA_82212_MAX_LANE; lane_idx ++) {
        if (phy->access.lane_mask & (1 << lane_idx)) {
            FURIA_82212_CALL_PHYDRV(phy_cpy, lane_idx,
                  furia_phy_tx_lane_control_get(&phy_cpy, tx_control));
            break;
        }
    }
       
    return PHYMOD_E_NONE;
    
}


int furia_82212_phy_rx_lane_control_set(const phymod_phy_access_t* phy, phymod_phy_rx_lane_control_t rx_control)
{
    phymod_phy_access_t phy_cpy;
    uint16_t lane_idx = 0;
    PHYMOD_MEMSET(&phy_cpy, 0, sizeof(phymod_phy_access_t));
    PHYMOD_MEMCPY(&phy_cpy, phy, sizeof(phymod_phy_access_t));
   
    for (lane_idx = 0; lane_idx < FURIA_82212_MAX_LANE; lane_idx ++) {
        if (phy->access.lane_mask & (1 << lane_idx)) {
            FURIA_82212_CALL_PHYDRV(phy_cpy, lane_idx,
                  furia_phy_rx_lane_control_set(&phy_cpy, rx_control));
        }
    }
       
    return PHYMOD_E_NONE;
    
}

int furia_82212_phy_rx_lane_control_get(const phymod_phy_access_t* phy, phymod_phy_rx_lane_control_t* rx_control)
{
    phymod_phy_access_t phy_cpy;
    uint16_t lane_idx = 0;
    PHYMOD_MEMSET(&phy_cpy, 0, sizeof(phymod_phy_access_t));
    PHYMOD_MEMCPY(&phy_cpy, phy, sizeof(phymod_phy_access_t));
   
    for (lane_idx = 0; lane_idx < FURIA_82212_MAX_LANE; lane_idx ++) {
        if (phy->access.lane_mask & (1 << lane_idx)) {
            FURIA_82212_CALL_PHYDRV(phy_cpy, lane_idx,
                  furia_phy_rx_lane_control_get(&phy_cpy, rx_control));
            break;
        }
    }
       
    return PHYMOD_E_NONE;
    
}

int furia_82212_phy_fec_enable_set(const phymod_phy_access_t* phy, uint32_t enable)
{
    phymod_phy_access_t phy_cpy;
    uint16_t lane_idx = 0;
    PHYMOD_MEMSET(&phy_cpy, 0, sizeof(phymod_phy_access_t));
    PHYMOD_MEMCPY(&phy_cpy, phy, sizeof(phymod_phy_access_t));
   
    for (lane_idx = 0; lane_idx < FURIA_82212_MAX_LANE; lane_idx ++) {
        if (phy->access.lane_mask & (1 << lane_idx)) {
            FURIA_82212_CALL_PHYDRV(phy_cpy, lane_idx,
                  furia_phy_fec_enable_set(&phy_cpy, enable));
        }
    }
       
    return PHYMOD_E_NONE;
       
}

int furia_82212_phy_fec_enable_get(const phymod_phy_access_t* phy, uint32_t* enable)
{
    phymod_phy_access_t phy_cpy;
    uint16_t lane_idx = 0;
    PHYMOD_MEMSET(&phy_cpy, 0, sizeof(phymod_phy_access_t));
    PHYMOD_MEMCPY(&phy_cpy, phy, sizeof(phymod_phy_access_t));
   
    for (lane_idx = 0; lane_idx < FURIA_82212_MAX_LANE; lane_idx ++) {
        if (phy->access.lane_mask & (1 << lane_idx)) {
            FURIA_82212_CALL_PHYDRV(phy_cpy, lane_idx,
                  furia_phy_fec_enable_get(&phy_cpy, enable));
            break;
        }
    }
       
    return PHYMOD_E_NONE;
    
}


int furia_82212_phy_interface_config_set(const phymod_phy_access_t* phy, uint32_t flags, const phymod_phy_inf_config_t* config)
{
    phymod_phy_access_t phy_cpy;
    uint16_t lane_idx = 0, die_idx = 0;
    PHYMOD_MEMSET(&phy_cpy, 0, sizeof(phymod_phy_access_t));
    PHYMOD_MEMCPY(&phy_cpy, phy, sizeof(phymod_phy_access_t));
    
    if (phy->access.lane_mask == FURIA_82212_BCAST_LANEMASK) {
        for (die_idx = 0; die_idx < FURIA_82212_MAX_DIE; die_idx ++) {
            phy_cpy.access.addr |= die_idx;
            /* Broadcast to Die 0 and 1*/
            phy_cpy.access.lane_mask = 0xFFF;
            PHYMOD_IF_ERR_RETURN(
                  furia_phy_interface_config_set(&phy_cpy, flags, config));
        }
    } else {
        for (lane_idx = 0; lane_idx < FURIA_82212_MAX_LANE; lane_idx ++) {
            if (phy->access.lane_mask & (1 << lane_idx)) {
                FURIA_82212_CALL_PHYDRV(phy_cpy, lane_idx,
                      furia_phy_interface_config_set(&phy_cpy, flags, config));
           }
        }
    }
    return PHYMOD_E_NONE;
    
}

int furia_82212_phy_interface_config_get(const phymod_phy_access_t* phy, uint32_t flags, phymod_ref_clk_t ref_clock, phymod_phy_inf_config_t* config)
{
    phymod_phy_access_t phy_cpy;
    uint16_t lane_idx = 0;
    PHYMOD_MEMSET(&phy_cpy, 0, sizeof(phymod_phy_access_t));
    PHYMOD_MEMCPY(&phy_cpy, phy, sizeof(phymod_phy_access_t));
   
    for (lane_idx = 0; lane_idx < FURIA_82212_MAX_LANE; lane_idx ++) {
        if (phy->access.lane_mask & (1 << lane_idx)) {
            FURIA_82212_CALL_PHYDRV(phy_cpy, lane_idx,
                  furia_phy_interface_config_get(&phy_cpy, flags, ref_clock, config));
        }
    }
        
    return PHYMOD_E_NONE;
    
}


int furia_82212_phy_cl72_set(const phymod_phy_access_t* phy, uint32_t cl72_en)
{
    phymod_phy_access_t phy_cpy;
    uint16_t lane_idx = 0;
    PHYMOD_MEMSET(&phy_cpy, 0, sizeof(phymod_phy_access_t));
    PHYMOD_MEMCPY(&phy_cpy, phy, sizeof(phymod_phy_access_t));
   
    for (lane_idx = 0; lane_idx < FURIA_82212_MAX_LANE; lane_idx ++) {
        if (phy->access.lane_mask & (1 << lane_idx)) {
            FURIA_82212_CALL_PHYDRV(phy_cpy, lane_idx,
                  furia_phy_cl72_set(&phy_cpy, cl72_en));
        }
    }
    return PHYMOD_E_NONE;
    
}

int furia_82212_phy_cl72_get(const phymod_phy_access_t* phy, uint32_t* cl72_en)
{
    phymod_phy_access_t phy_cpy;
    uint16_t lane_idx = 0;
    PHYMOD_MEMSET(&phy_cpy, 0, sizeof(phymod_phy_access_t));
    PHYMOD_MEMCPY(&phy_cpy, phy, sizeof(phymod_phy_access_t));
   
    for (lane_idx = 0; lane_idx < FURIA_82212_MAX_LANE; lane_idx ++) {
        if (phy->access.lane_mask & (1 << lane_idx)) {
            FURIA_82212_CALL_PHYDRV(phy_cpy, lane_idx,
                  furia_phy_cl72_get(&phy_cpy, cl72_en));
            break;
        }
    }
    return PHYMOD_E_NONE;
    
}


int furia_82212_phy_cl72_status_get(const phymod_phy_access_t* phy, phymod_cl72_status_t* status)
{
    phymod_phy_access_t phy_cpy;
    uint16_t lane_idx = 0;
    phymod_cl72_status_t sts;
    PHYMOD_MEMSET(&phy_cpy, 0, sizeof(phymod_phy_access_t));
    PHYMOD_MEMCPY(&phy_cpy, phy, sizeof(phymod_phy_access_t));
    status->locked = 0xFFFF; 

    for (lane_idx = 0; lane_idx < FURIA_82212_MAX_LANE; lane_idx ++) {
        if (phy->access.lane_mask & (1 << lane_idx)) {
            FURIA_82212_CALL_PHYDRV(phy_cpy, lane_idx,
                  furia_phy_cl72_status_get(&phy_cpy, &sts));
            status->locked &= sts.locked; 
        }
    }
       
    return PHYMOD_E_NONE;
    
}


int furia_82212_phy_autoneg_ability_set(const phymod_phy_access_t* phy, const phymod_autoneg_ability_t* an_ability_set_type)
{
        
    return PHYMOD_E_NONE;
    
}

int furia_82212_phy_autoneg_ability_get(const phymod_phy_access_t* phy, phymod_autoneg_ability_t* an_ability_get_type)
{
    return PHYMOD_E_NONE;
    
}


int furia_82212_phy_autoneg_remote_ability_get(const phymod_phy_access_t* phy, phymod_autoneg_ability_t* an_ability_get_type)
{
    return PHYMOD_E_NONE;
    
}


int furia_82212_phy_autoneg_set(const phymod_phy_access_t* phy, const phymod_autoneg_control_t* an)
{
    return PHYMOD_E_NONE;
    
}

int furia_82212_phy_autoneg_get(const phymod_phy_access_t* phy, phymod_autoneg_control_t* an, uint32_t* an_done)
{
    return PHYMOD_E_NONE;
    
}


int furia_82212_core_init(const phymod_core_access_t* core, const phymod_core_init_config_t* init_config, const phymod_core_status_t* core_status)
{
    phymod_core_access_t core_cpy;
    uint16_t die_idx = 0;
    PHYMOD_MEMSET(&core_cpy, 0, sizeof(phymod_core_access_t));
    PHYMOD_MEMCPY(&core_cpy, core, sizeof(phymod_core_access_t));
   
    core_cpy.access.addr &= (~1);
    for (die_idx = 0; die_idx < FURIA_82212_MAX_DIE; die_idx ++) {
        core_cpy.access.addr |= die_idx;
        PHYMOD_IF_ERR_RETURN(furia_core_init(&core_cpy, init_config, core_status));
    }
    return PHYMOD_E_NONE;
    
}


int furia_82212_phy_init(const phymod_phy_access_t* phy, const phymod_phy_init_config_t* init_config)
{
    phymod_phy_access_t phy_cpy;
    uint16_t lane_idx = 0;
    PHYMOD_MEMSET(&phy_cpy, 0, sizeof(phymod_phy_access_t));
    PHYMOD_MEMCPY(&phy_cpy, phy, sizeof(phymod_phy_access_t));
   
    for (lane_idx = 0; lane_idx < FURIA_82212_MAX_LANE; lane_idx ++) {
        if (phy->access.lane_mask & (1 << lane_idx)) {
            FURIA_82212_CALL_PHYDRV(phy_cpy, lane_idx,
                  furia_phy_init(&phy_cpy, init_config));
        }
    }

    return PHYMOD_E_NONE;
    
}


int furia_82212_phy_loopback_set(const phymod_phy_access_t* phy, phymod_loopback_mode_t loopback, uint32_t enable)
{
        
    return PHYMOD_E_NONE;
    
}

int furia_82212_phy_loopback_get(const phymod_phy_access_t* phy, phymod_loopback_mode_t loopback, uint32_t* enable)
{
    *enable = 0;
        
    return PHYMOD_E_NONE;
    
}


int furia_82212_phy_rx_pmd_locked_get(const phymod_phy_access_t* phy, uint32_t* rx_pmd_locked)
{
    phymod_phy_access_t phy_cpy;
    uint16_t lane_idx = 0;
    uint32_t lock = 0xFFFF;
    PHYMOD_MEMSET(&phy_cpy, 0, sizeof(phymod_phy_access_t));
    PHYMOD_MEMCPY(&phy_cpy, phy, sizeof(phymod_phy_access_t));
   
    *rx_pmd_locked = 0xFFFF;
    for (lane_idx = 0; lane_idx < FURIA_82212_MAX_LANE; lane_idx ++) {
        if (phy->access.lane_mask & (1 << lane_idx)) {
            FURIA_82212_CALL_PHYDRV(phy_cpy, lane_idx,
                  furia_phy_rx_pmd_locked_get(&phy_cpy, &lock));
            *rx_pmd_locked &= lock;
        }
    }
   
    return PHYMOD_E_NONE;
    
}


int furia_82212_phy_link_status_get(const phymod_phy_access_t* phy, uint32_t* link_status)
{
    phymod_phy_access_t phy_cpy;
    uint16_t lane_idx = 0;
    uint32_t status = 0xFFFF;
    PHYMOD_MEMSET(&phy_cpy, 0, sizeof(phymod_phy_access_t));
    PHYMOD_MEMCPY(&phy_cpy, phy, sizeof(phymod_phy_access_t));
   
    *link_status = 0xFFFF;
    for (lane_idx = 0; lane_idx < FURIA_82212_MAX_LANE; lane_idx ++) {
        if (phy->access.lane_mask & (1 << lane_idx)) {
            FURIA_82212_CALL_PHYDRV(phy_cpy, lane_idx,
                  furia_phy_link_status_get(&phy_cpy, &status));
            *link_status &= status;
        }
    }
       
    return PHYMOD_E_NONE;
    
}


int furia_82212_phy_status_dump(const phymod_phy_access_t* phy)
{
    phymod_phy_access_t phy_cpy;
    uint16_t lane_idx = 0;
    PHYMOD_MEMSET(&phy_cpy, 0, sizeof(phymod_phy_access_t));
    PHYMOD_MEMCPY(&phy_cpy, phy, sizeof(phymod_phy_access_t));
   
    for (lane_idx = 0; lane_idx < FURIA_82212_MAX_LANE; lane_idx ++) {
        if (phy->access.lane_mask & (1 << lane_idx)) {
            FURIA_82212_CALL_PHYDRV(phy_cpy, lane_idx,
                  furia_phy_status_dump(&phy_cpy));
        }
    }
        
    return PHYMOD_E_NONE;
    
}


int furia_82212_phy_reg_read(const phymod_phy_access_t* phy, uint32_t reg_addr, uint32_t* val)
{
    phymod_phy_access_t phy_cpy;
    uint16_t lane_idx = 0;
    PHYMOD_MEMSET(&phy_cpy, 0, sizeof(phymod_phy_access_t));
    PHYMOD_MEMCPY(&phy_cpy, phy, sizeof(phymod_phy_access_t));
   
    for (lane_idx = 0; lane_idx < FURIA_82212_MAX_LANE; lane_idx ++) {
        if (phy->access.lane_mask & (1 << lane_idx)) {
            FURIA_82212_CALL_PHYDRV(phy_cpy, lane_idx,
                furia_phy_reg_read(&phy_cpy, reg_addr, val));
        }
    }

    return PHYMOD_E_NONE;
    
}


int furia_82212_phy_reg_write(const phymod_phy_access_t* phy, uint32_t reg_addr, uint32_t val)
{
    phymod_phy_access_t phy_cpy;
    uint16_t lane_idx = 0;
    PHYMOD_MEMSET(&phy_cpy, 0, sizeof(phymod_phy_access_t));
    PHYMOD_MEMCPY(&phy_cpy, phy, sizeof(phymod_phy_access_t));
   
    for (lane_idx = 0; lane_idx < FURIA_82212_MAX_LANE; lane_idx ++) {
        if (phy->access.lane_mask & (1 << lane_idx)) {
            FURIA_82212_CALL_PHYDRV(phy_cpy, lane_idx,
                  furia_phy_reg_write(&phy_cpy, reg_addr, val));
        }
    }
   
    return PHYMOD_E_NONE;
    
}


int furia_82212_phy_lane_cross_switch_map_set(const phymod_phy_access_t* phy, const uint32_t* tx_array)
{
    phymod_phy_access_t phy_cpy;
    uint16_t lane_idx = 0;
    uint16_t die_id = 0;
    uint32_t tx_array_conv[FURIA_82212_MAX_LANE];/*12 element type array*/
    PHYMOD_MEMSET(&phy_cpy, 0, sizeof(phymod_phy_access_t));
    PHYMOD_MEMCPY(&phy_cpy, phy, sizeof(phymod_phy_access_t));

    die_id = ( tx_array[6] ) & ( 1 ) ;

    for (lane_idx = 0; lane_idx < FURIA_82212_MAX_LANE; lane_idx ++) {
        FURIA_82212_CALL_PHYDRV(phy_cpy, lane_idx,
              furia_phy_lane_cross_switch_map_get(&phy_cpy, (uint32_t*)&tx_array_conv));/*get 12 element array*/
    }

    /*add array, to higher 7-12 elements for die id 1*/
    /*add array, to lower 1-6 elements for die id 0*/
    for (lane_idx = 0; lane_idx < FURIA_82212_MAX_DIE_LANE; lane_idx ++) {
        tx_array_conv[lane_idx + ( 6 * die_id )] = tx_array[lane_idx] ;
    }

    for (lane_idx = 0; lane_idx < FURIA_82212_MAX_LANE; lane_idx ++) {
        FURIA_82212_CALL_PHYDRV(phy_cpy, lane_idx,
              furia_phy_lane_cross_switch_map_set(&phy_cpy, tx_array_conv));
    }
 
    return PHYMOD_E_NONE;
    
}

int furia_82212_phy_lane_cross_switch_map_get(const phymod_phy_access_t* phy, uint32_t* tx_array)
{
    phymod_phy_access_t phy_cpy;
    uint16_t lane_idx = 0;
    uint16_t die_id = 0;
    uint32_t copy_tx_array[FURIA_82212_MAX_LANE];/*12 element type array*/
    PHYMOD_MEMSET(&phy_cpy, 0, sizeof(phymod_phy_access_t));
    PHYMOD_MEMCPY(&phy_cpy, phy, sizeof(phymod_phy_access_t));

    die_id = ( tx_array[6] ) & ( 1 ) ;/*die select*/
    for (lane_idx = 0; lane_idx < FURIA_82212_MAX_LANE; lane_idx ++) {
        FURIA_82212_CALL_PHYDRV(phy_cpy, lane_idx,
              furia_phy_lane_cross_switch_map_get(&phy_cpy, (uint32_t*)&copy_tx_array));
    }
    /*shift array, convert 12 to 8 elements for glue layer*/
    for (lane_idx = 0; lane_idx < FURIA_82212_MAX_DIE_LANE; lane_idx ++) {
                    tx_array[lane_idx]= copy_tx_array[ lane_idx + (6*die_id) ] ;
    }
 
    return PHYMOD_E_NONE;
    
}


int furia_82212_phy_intr_enable_set(const phymod_phy_access_t* phy, uint32_t enable)
{
    phymod_phy_access_t phy_cpy;
    uint16_t lane_idx = 0;
    uint16_t prev_die = 0xF;
    PHYMOD_MEMSET(&phy_cpy, 0, sizeof(phymod_phy_access_t));
    PHYMOD_MEMCPY(&phy_cpy, phy, sizeof(phymod_phy_access_t));
   
    for (lane_idx = 0; lane_idx < FURIA_82212_MAX_LANE; lane_idx ++) {
        if (phy->access.lane_mask & (1 << lane_idx)) {
            phy_cpy.access.addr &= ~(1);
            phy_cpy.access.addr |= FURIA_82212_GET_DIE_NO(lane_idx);
            if (FURIA_82212_GET_DIE_NO(lane_idx) == prev_die) {
                continue;
            }
            PHYMOD_DEBUG_VERBOSE(("INTR enable Set Addr:%x\n",phy_cpy.access.addr)); 
            FURIA_82212_CALL_PHYDRV(phy_cpy, lane_idx,
                  furia_phy_intr_enable_set(&phy_cpy, enable));
            prev_die = FURIA_82212_GET_DIE_NO(lane_idx);
        }
    }
        
    return PHYMOD_E_NONE;
    
}

int furia_82212_phy_intr_enable_get(const phymod_phy_access_t* phy, uint32_t * enable)
{
    phymod_phy_access_t phy_cpy;
    uint16_t lane_idx = 0;
    uint16_t prev_die = 0xF;
    PHYMOD_MEMSET(&phy_cpy, 0, sizeof(phymod_phy_access_t));
    PHYMOD_MEMCPY(&phy_cpy, phy, sizeof(phymod_phy_access_t));
   
    for (lane_idx = 0; lane_idx < FURIA_82212_MAX_LANE; lane_idx ++) {
        if (phy->access.lane_mask & (1 << lane_idx)) {
            phy_cpy.access.addr &= ~(1);
            phy_cpy.access.addr |= FURIA_82212_GET_DIE_NO(lane_idx);
            if (FURIA_82212_GET_DIE_NO(lane_idx) == prev_die) {
                continue;
            }
            PHYMOD_DEBUG_VERBOSE((
              "INTR enable Get Addr:%x\n",phy_cpy.access.addr)); 
            FURIA_82212_CALL_PHYDRV(phy_cpy, lane_idx,
                  furia_phy_intr_enable_get(&phy_cpy, enable));
            prev_die = FURIA_82212_GET_DIE_NO(lane_idx);
        }
    }
        
    return PHYMOD_E_NONE;
    
}


int furia_82212_phy_intr_status_get(const phymod_phy_access_t* phy, uint32_t * intr_status)
{
    phymod_phy_access_t phy_cpy;
    uint16_t lane_idx = 0;
    uint16_t prev_die = 0xF;
    PHYMOD_MEMSET(&phy_cpy, 0, sizeof(phymod_phy_access_t));
    PHYMOD_MEMCPY(&phy_cpy, phy, sizeof(phymod_phy_access_t));
   
    for (lane_idx = 0; lane_idx < FURIA_82212_MAX_LANE; lane_idx ++) {
        if (phy->access.lane_mask & (1 << lane_idx)) {
            phy_cpy.access.addr &= ~(1);
            phy_cpy.access.addr |= FURIA_82212_GET_DIE_NO(lane_idx);
            if (FURIA_82212_GET_DIE_NO(lane_idx) == prev_die) {
                continue;
            }
            PHYMOD_DEBUG_VERBOSE((
             "INTR STS Get Addr:%x\n",phy_cpy.access.addr)); 
            FURIA_82212_CALL_PHYDRV(phy_cpy, lane_idx,
                  furia_phy_intr_status_get(&phy_cpy, intr_status));
            prev_die = FURIA_82212_GET_DIE_NO(lane_idx);
        }
    }
        
    return PHYMOD_E_NONE;
    
}


int furia_82212_phy_intr_status_clear(const phymod_phy_access_t* phy, uint32_t intr_type)
{
    phymod_phy_access_t phy_cpy;
    uint16_t lane_idx = 0;
    uint16_t prev_die = 0xF;
    PHYMOD_MEMSET(&phy_cpy, 0, sizeof(phymod_phy_access_t));
    PHYMOD_MEMCPY(&phy_cpy, phy, sizeof(phymod_phy_access_t));
   
    for (lane_idx = 0; lane_idx < FURIA_82212_MAX_LANE; lane_idx ++) {
        if (phy->access.lane_mask & (1 << lane_idx)) {
            phy_cpy.access.addr &= ~(1);
            phy_cpy.access.addr |= FURIA_82212_GET_DIE_NO(lane_idx);
            if (FURIA_82212_GET_DIE_NO(lane_idx) == prev_die) {
                continue;
            }
            PHYMOD_DEBUG_VERBOSE((
             "INTR STS clear:%x\n",phy_cpy.access.addr)); 
            FURIA_82212_CALL_PHYDRV(phy_cpy, lane_idx,
                  furia_phy_intr_status_clear(&phy_cpy, intr_type));
            prev_die = FURIA_82212_GET_DIE_NO(lane_idx);
        }
    }
       
    return PHYMOD_E_NONE;
    
}


int furia_82212_phy_i2c_read(const phymod_phy_access_t* phy, uint32_t flags, uint32_t addr, uint32_t offset, uint32_t size, uint8_t* data)
{
    phymod_phy_access_t phy_cpy;
    uint16_t lane_idx = 0;
    PHYMOD_MEMSET(&phy_cpy, 0, sizeof(phymod_phy_access_t));
    PHYMOD_MEMCPY(&phy_cpy, phy, sizeof(phymod_phy_access_t));
   
    for (lane_idx = 0; lane_idx < FURIA_82212_MAX_LANE; lane_idx ++) {
        if (phy->access.lane_mask & (1 << lane_idx)) {
            phy_cpy.access.addr &= ~(1);
            phy_cpy.access.addr |= FURIA_82212_GET_DIE_NO(lane_idx);
            PHYMOD_DEBUG_VERBOSE((
               "I2c read:%x\n",phy_cpy.access.addr)); 
            FURIA_82212_CALL_PHYDRV(phy_cpy, lane_idx,
                  furia_phy_i2c_read(&phy_cpy, flags, addr, offset, size, data));
            break;
        }
    }
   
    return PHYMOD_E_NONE;
    
}


int furia_82212_phy_i2c_write(const phymod_phy_access_t* phy, uint32_t flags, uint32_t addr, uint32_t offset, uint32_t size, const uint8_t* data)
{
    phymod_phy_access_t phy_cpy;
    uint16_t lane_idx = 0;
    uint16_t prev_die = 0xF;
    PHYMOD_MEMSET(&phy_cpy, 0, sizeof(phymod_phy_access_t));
    PHYMOD_MEMCPY(&phy_cpy, phy, sizeof(phymod_phy_access_t));
   
    for (lane_idx = 0; lane_idx < FURIA_82212_MAX_LANE; lane_idx ++) {
        if (phy->access.lane_mask & (1 << lane_idx)) {
            phy_cpy.access.addr &= ~(1);
            phy_cpy.access.addr |= FURIA_82212_GET_DIE_NO(lane_idx);
            if (FURIA_82212_GET_DIE_NO(lane_idx) == prev_die) {
                continue;
            }
            PHYMOD_DEBUG_VERBOSE((
              "I2c write:%x\n",phy_cpy.access.addr)); 
            FURIA_82212_CALL_PHYDRV(phy_cpy, lane_idx,
                  furia_phy_i2c_write(&phy_cpy, flags, addr, offset, size, data));
            prev_die = FURIA_82212_GET_DIE_NO(lane_idx);
        }
    }
        
    return PHYMOD_E_NONE;
    
}


int furia_82212_phy_gpio_config_set(const phymod_phy_access_t* phy, int pin_no, phymod_gpio_mode_t gpio_mode)
{
    phymod_phy_access_t phy_cpy;
    uint16_t lane_idx = 0;
    uint16_t prev_die = 0xF;
    PHYMOD_MEMSET(&phy_cpy, 0, sizeof(phymod_phy_access_t));
    PHYMOD_MEMCPY(&phy_cpy, phy, sizeof(phymod_phy_access_t));
   
    for (lane_idx = 0; lane_idx < FURIA_82212_MAX_LANE; lane_idx ++) {
        if (phy->access.lane_mask & (1 << lane_idx)) {
            phy_cpy.access.addr &= ~(1);
            phy_cpy.access.addr |= FURIA_82212_GET_DIE_NO(lane_idx);
            if (FURIA_82212_GET_DIE_NO(lane_idx) == prev_die) {
                continue;
            }
            PHYMOD_DEBUG_VERBOSE((
              "GPIO config set:%x\n",phy_cpy.access.addr)); 
            FURIA_82212_CALL_PHYDRV(phy_cpy, lane_idx,
                  furia_phy_gpio_config_set(&phy_cpy, pin_no, gpio_mode));
            prev_die = FURIA_82212_GET_DIE_NO(lane_idx);
        }
    }
       
    return PHYMOD_E_NONE;
    
}

int furia_82212_phy_gpio_config_get(const phymod_phy_access_t* phy, int pin_no, phymod_gpio_mode_t* gpio_mode)
{
    phymod_phy_access_t phy_cpy;
    uint16_t lane_idx = 0;
    PHYMOD_MEMSET(&phy_cpy, 0, sizeof(phymod_phy_access_t));
    PHYMOD_MEMCPY(&phy_cpy, phy, sizeof(phymod_phy_access_t));
   
    for (lane_idx = 0; lane_idx < FURIA_82212_MAX_LANE; lane_idx ++) {
        if (phy->access.lane_mask & (1 << lane_idx)) {
            phy_cpy.access.addr &= ~(1);
            phy_cpy.access.addr |= FURIA_82212_GET_DIE_NO(lane_idx);
            FURIA_82212_CALL_PHYDRV(phy_cpy, lane_idx,
                  furia_phy_gpio_config_get(&phy_cpy, pin_no, gpio_mode));
            break;
        }
    }
       
    return PHYMOD_E_NONE;
    
}


int furia_82212_phy_gpio_pin_value_set(const phymod_phy_access_t* phy, int pin_no, int value)
{
    phymod_phy_access_t phy_cpy;
    uint16_t lane_idx = 0;
    uint16_t prev_die = 0xF;
    PHYMOD_MEMSET(&phy_cpy, 0, sizeof(phymod_phy_access_t));
    PHYMOD_MEMCPY(&phy_cpy, phy, sizeof(phymod_phy_access_t));
   
    for (lane_idx = 0; lane_idx < FURIA_82212_MAX_LANE; lane_idx ++) {
        if (phy->access.lane_mask & (1 << lane_idx)) {
            phy_cpy.access.addr &= ~(1);
            phy_cpy.access.addr |= FURIA_82212_GET_DIE_NO(lane_idx);
            if (FURIA_82212_GET_DIE_NO(lane_idx) == prev_die) {
                continue;
            }
            FURIA_82212_CALL_PHYDRV(phy_cpy, lane_idx,
                  furia_phy_gpio_pin_value_set(&phy_cpy, pin_no, value));
            prev_die = FURIA_82212_GET_DIE_NO(lane_idx);
        }
    }
       
    return PHYMOD_E_NONE;
    
}

int furia_82212_phy_gpio_pin_value_get(const phymod_phy_access_t* phy, int pin_no, int* value)
{
    phymod_phy_access_t phy_cpy;
    uint16_t lane_idx = 0;
    PHYMOD_MEMSET(&phy_cpy, 0, sizeof(phymod_phy_access_t));
    PHYMOD_MEMCPY(&phy_cpy, phy, sizeof(phymod_phy_access_t));
   
    for (lane_idx = 0; lane_idx < FURIA_82212_MAX_LANE; lane_idx ++) {
        if (phy->access.lane_mask & (1 << lane_idx)) {
            phy_cpy.access.addr &= ~(1);
            phy_cpy.access.addr |= FURIA_82212_GET_DIE_NO(lane_idx);
            FURIA_82212_CALL_PHYDRV(phy_cpy, lane_idx,
                  furia_phy_gpio_pin_value_get(&phy_cpy, pin_no, value));
            break;
        }
    }

    return PHYMOD_E_NONE;
    
}

int furia_82212_phy_op_mode_get(const phymod_phy_access_t* phy, phymod_operation_mode_t* op_mode)
{
    return PHYMOD_E_UNAVAIL;
}

#endif /* PHYMOD_FURIA_SUPPORT */
