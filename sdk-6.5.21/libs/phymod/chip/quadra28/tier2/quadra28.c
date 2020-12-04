/*
 *         
 * 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/*
 *
 * 
 *
 * $Copyright:
 * All Rights Reserved.$
 *
 * File: quadra28.c
 * Purpose: tier2 phymod support for Broadcom 40G Quadra28 phy
 * note:
 * Specifications:

 * Repeater, retimer operation.

 * Supports the following data speeds:
 * 1.25 Gbps line rate (1 GbE data rate, 8x oversampled over 10 Gbps line rate)
 * 10.3125 Gbps line rate (10 GbE data rate)
 * 11.5 Gpbs line rate (for backplane application, proprietary data rate)
 * 410.3125 Gbps line rate (40 GbE data rate)
 *
 * Supports the following line-side connections:
 * 1 GbE and 10 GbE SFP+ SR and LR optical modules
 * 40 GbE QSFP SR4 and LR4 optical modules
 * 1 GbE and 10 GbE SFP+ CR (CX1) copper cable
 * 40 GbE QSFP CR4 copper cable
 * 10 GbE KR, 11.5 Gbps line rate and 40 GbE KR4 backplanes
 *
 * Operates with the following reference clocks:
 * Single 156.25 MHz differential clock for 1.25 Gbps, 10.3125 Gpbs and 11.5 Gbps
 * line rates
 *
 * Supports autonegotiation as follows:
 * Clause 73 only for starting Clause 72 and requesting FEC
 * No speed resolution performed
 * No Clause 73 in 11.5 Gbps line rate, only Clause 72 supported
 * Clause 72 may be enabled standalone for close systems
 * Clause 37 is supported
     
  */

#include <phymod/phymod.h>
#include <phymod/phymod_system.h>
#include <phymod/phymod_dispatch.h>


#ifdef PHYMOD_QUADRA28_SUPPORT
#include <phymod/chip/quadra28.h>

/* To be defined */
#include "../tier1/quadra28_types.h"
#include "../tier1/quadra28_cfg_seq.h"
#include "../tier1/quadra28_reg_access.h"
#include "../tier1/bcmi_quadra28_defs.h"

/* Microcontroller Firmware */
extern unsigned char quadra28_ucode_bin[];
extern uint32_t quadra28_ucode_len;

int _quadra28_core_firmware_load (const phymod_core_access_t* core,
                                  const phymod_core_init_config_t *init_config);


/*
 *    Set Interface config
 *    This function sets interface configuration (interface, speed, frequency of
 *    operation). This is requried to put the quadra28 into specific mode specified
 *    by the user.
 *
 *    @param phy                Pointer to phymod phy access structure
 *    @param flags              Currently unused and reserved for future use
 *    @param config             Interface config structure where user specifies
 *                              interface, speed and the frequency of operation.
 *
 *    @return PHYMOD_E_NONE     successful function execution
 */
int quadra28_phy_interface_config_set(const phymod_phy_access_t* phy,
                                   uint32_t flags,
                                   const phymod_phy_inf_config_t* config)
{
    if (config->data_rate == 100 || config->data_rate == 10) {
        uint32_t enable = 0;
        PHYMOD_IF_ERR_RETURN(
            _quadra28_phy_retimer_enable_get(&phy->access, &enable));
        if (!enable) {
            return PHYMOD_E_NONE;
        }
    }
    return quadra28_set_config_mode(phy,
                       config->interface_type,
                       config->data_rate,
                       config->ref_clock, 0xFFFF, 0xFFFF);
}

/**   Get Interface config
 *    This function retrieves interface configuration(interface, speed and
 *    frequency of operation) from the device.
 *
 *    @param phy                Pointer to phymod phy access structure
 *    @param flags              Currently unused and reserved for future use
 *    @param config             Interface config structure where
 *                              interface, speed and the frequency of operation
 *                              will be populated from device .
 *
 *    @return PHYMOD_E_NONE     successful function execution
 */
int quadra28_phy_interface_config_get(const phymod_phy_access_t* phy,
                                   uint32_t flags,
                                   phymod_ref_clk_t ref_clock,
                                   phymod_phy_inf_config_t* config)
{
    return quadra28_get_config_mode(&phy->access,
                       &config->interface_type,
                       &config->data_rate,
                       &config->ref_clock,
                       &config->interface_modes);
}


/**   Core Init
 *    This function initializes the quadra core and
 *    downloads the firmware. It sets the PHY in
 *    default mode.
 *
 *    @param phy                Pointer to phymod core access structure
 *    @param init_config        Init configuration specified by user
 *    @param core_status        Core status read from PHY chip
 *
 *    @return PHYMOD_E_NONE     successful function execution
 */
int quadra28_core_init(const phymod_core_access_t *core,
                       const phymod_core_init_config_t *init_config,
                       const phymod_core_status_t *core_status) 
{

    PHYMOD_IF_ERR_RETURN(
        _quadra28_core_firmware_load (core, init_config));


    return PHYMOD_E_NONE;

}

/**   Core firmware download
 *    This function downloads the firmware using either internal or external
 *    method as specified by the user. Firmware loader function supplied by user
 *    will be used for external download method.
 *
 *    @param core               Pointer to phymod core access structure
 *    @param load_method        Load method will be specified by user
 *    @param fw_loader          Loader function specified by user and will be
 *                              used for external download method
 *
 *    @return PHYMOD_E_NONE     successful function execution
 */
int _quadra28_core_firmware_load (const phymod_core_access_t* core,
                                  const phymod_core_init_config_t *init_config)
{
    int ret_val = 0, dload_method = init_config->firmware_load_method;
    const phymod_access_t *pm_acc = &core->access;
    uint32_t chip_id;
    uint32_t new_firmware_version=0;
    phymod_core_firmware_info_t fw_info;

    PHYMOD_MEMSET(&fw_info, 0, sizeof(phymod_core_firmware_info_t));
    PHYMOD_IF_ERR_RETURN(
        quadra28_core_firmware_info_get(core, &fw_info));
    new_firmware_version = quadra28_ucode_bin[quadra28_ucode_len-3];
    chip_id =  _quadra28_get_chip_id(&core->access);

    switch(dload_method)
    {
        case phymodFirmwareLoadMethodInternal:
            if(PHYMOD_CORE_INIT_F_FW_FORCE_DOWNLOAD_GET(init_config)){
                if (PHYMOD_CORE_INIT_F_UNTIL_FW_LOAD_GET(init_config)) {
                    return quadra28_before_fw_load(core, init_config, chip_id);
                } else if (PHYMOD_CORE_INIT_F_EXECUTE_FW_LOAD_GET(init_config)) {
                    return quadra28_bcast_fw_load(core, init_config,
                                                  quadra28_ucode_bin, quadra28_ucode_len);
                } else if (PHYMOD_CORE_INIT_F_RESUME_AFTER_FW_LOAD_GET(init_config)) {
                       ret_val = quadra28_bcast_after_fw_load(core, chip_id);
                       if(ret_val != 0){
                           PHYMOD_DEBUG_ERROR(("Firmware Verification failed %d\n",ret_val)); 

                           /* Doing Softreset to clear Broadcast*/
                           PHYMOD_IF_ERR_RETURN(
                               quadra28_bcast_disable(core, chip_id));
      
                       }
                       return ret_val; 
                } else if(PHYMOD_CORE_INIT_F_FW_LOAD_END_GET(init_config)){
                      PHYMOD_DIAG_OUT(("Firmware download success\n"));
                      /* Using sratch Reg to store and retrevie later*/
                      PHYMOD_IF_ERR_RETURN(
                          phymod_raw_iblk_write(&core->access, 0x1c91A,init_config->op_datapath));
                      PHYMOD_IF_ERR_RETURN(
                          phymod_raw_iblk_write(&core->access, 0x1c91B,init_config->op_datapath));

                      /* Doing Softreset to clear Broadcast*/
                      PHYMOD_IF_ERR_RETURN(
                           quadra28_bcast_disable(core, chip_id));

                }
            } else if(PHYMOD_CORE_INIT_F_FW_AUTO_DOWNLOAD_GET(init_config) && new_firmware_version != fw_info.fw_version){
                if (PHYMOD_CORE_INIT_F_UNTIL_FW_LOAD_GET(init_config)) {
                    return quadra28_before_fw_load(core, init_config, chip_id);
                } else if (PHYMOD_CORE_INIT_F_EXECUTE_FW_LOAD_GET(init_config)) {
                    return quadra28_bcast_fw_load(core, init_config,
                                                  quadra28_ucode_bin, quadra28_ucode_len);
                } else if (PHYMOD_CORE_INIT_F_RESUME_AFTER_FW_LOAD_GET(init_config)) {
                       ret_val = quadra28_bcast_after_fw_load(core, chip_id);
                       if(ret_val != 0){
                           PHYMOD_DEBUG_ERROR(("Firmware Verification failed %d\n",ret_val)); 

                           /* Doing Softreset to clear Broadcast*/
                           PHYMOD_IF_ERR_RETURN(
                               quadra28_bcast_disable(core, chip_id));
      
                       }
                       return ret_val; 
                } else if(PHYMOD_CORE_INIT_F_FW_LOAD_END_GET(init_config)){
                      PHYMOD_DIAG_OUT(("Firmware download success\n"));
                      /* Using sratch Reg to store and retrevie later*/
                      PHYMOD_IF_ERR_RETURN(
                          phymod_raw_iblk_write(&core->access, 0x1c91A,init_config->op_datapath));
                      PHYMOD_IF_ERR_RETURN(
                          phymod_raw_iblk_write(&core->access, 0x1c91B,init_config->op_datapath));

                      /* Doing Softreset to clear Broadcast*/
                      PHYMOD_IF_ERR_RETURN(
                           quadra28_bcast_disable(core, chip_id));
                }
            } else if(fw_info.fw_version == 0){
                if (PHYMOD_CORE_INIT_F_UNTIL_FW_LOAD_GET(init_config)) {
                    return quadra28_before_fw_load(core, init_config, chip_id);
                } else if (PHYMOD_CORE_INIT_F_EXECUTE_FW_LOAD_GET(init_config)) {
                    return quadra28_bcast_fw_load(core, init_config,
                                                  quadra28_ucode_bin, quadra28_ucode_len);
                } else if (PHYMOD_CORE_INIT_F_RESUME_AFTER_FW_LOAD_GET(init_config)) {
                       ret_val = quadra28_bcast_after_fw_load(core, chip_id);
                       if(ret_val != 0){
                           PHYMOD_DEBUG_ERROR(("Firmware Verification failed %d\n",ret_val)); 

                           /* Doing Softreset to clear Broadcast*/
                           PHYMOD_IF_ERR_RETURN(
                               quadra28_bcast_disable(core, chip_id));
      
                       }
                       return ret_val; 
                } else if(PHYMOD_CORE_INIT_F_FW_LOAD_END_GET(init_config)){
                      PHYMOD_DIAG_OUT(("Firmware download success\n"));
                      /* Using sratch Reg to store and retrevie later*/
                      PHYMOD_IF_ERR_RETURN(
                          phymod_raw_iblk_write(&core->access, 0x1c91A,init_config->op_datapath));
                      PHYMOD_IF_ERR_RETURN(
                          phymod_raw_iblk_write(&core->access, 0x1c91B,init_config->op_datapath));

                      /* Doing Softreset to clear Broadcast*/
                      PHYMOD_IF_ERR_RETURN(
                           quadra28_bcast_disable(core,chip_id));
                }
            } else {
                if (fw_info.fw_version != 0) {
                    MDIO_BROADCAST_CONTROLr_t mdio_broadcast_ctrl_reg;
                    PHYMOD_IF_ERR_RETURN
                        (READ_MDIO_BROADCAST_CONTROLr(&core->access, &mdio_broadcast_ctrl_reg));
                    if (BCMI_QUADRA28_MDIO_BROADCAST_CONTROLr_MDIO_BCAST_ENf_GET(mdio_broadcast_ctrl_reg)) {
                        /* Using sratch Reg to store and retrevie later*/
                        PHYMOD_IF_ERR_RETURN(
                           phymod_raw_iblk_write(&core->access, 0x1c91A,init_config->op_datapath));
                        PHYMOD_IF_ERR_RETURN(
                           phymod_raw_iblk_write(&core->access, 0x1c91B,init_config->op_datapath));
                        /* Doing Softreset to clear Broadcast*/
                        PHYMOD_IF_ERR_RETURN(
                            quadra28_bcast_disable(core,chip_id));
                     }
                }
            }
        break;
        case phymodFirmwareLoadMethodExternal:
            return PHYMOD_E_UNAVAIL;
        break;
        case phymodFirmwareLoadMethodNone:
        break;
        case phymodFirmwareLoadMethodProgEEPROM:
            PHYMOD_DIAG_OUT(("Firmware will be downloaded first, and flashed on to EEPROM \n"));
            PHYMOD_DIAG_OUT(("This process will take few minutes.....\n"));
            ret_val = quadra28_rom_dload(pm_acc, quadra28_ucode_bin,
                                         quadra28_ucode_len);
            if (ret_val != PHYMOD_E_NONE) {
                PHYMOD_RETURN_WITH_ERR
                (PHYMOD_E_FAIL,
                 (_PHYMOD_MSG("Flashing Firmware to EEPROM failed")));
            } else {
                PHYMOD_DEBUG_VERBOSE(("Firmware is flashed to EEPROM successfully\n"));
            }
            /* Using sratch Reg to store and retrevie later*/
            PHYMOD_IF_ERR_RETURN(
                 phymod_raw_iblk_write(&core->access, 0x1c91A,init_config->op_datapath));
            PHYMOD_IF_ERR_RETURN(
                 phymod_raw_iblk_write(&core->access, 0x1c91B,init_config->op_datapath));
        break;
        default:
            PHYMOD_RETURN_WITH_ERR
                (PHYMOD_E_CONFIG,
                 (_PHYMOD_MSG("illegal fw load method")));
    }
    return PHYMOD_E_NONE;
}

 
/**   Get PHY link status
 *    This function retrieves PHY link status from device
 *
 *    @param phy                Pointer to phymod phy access structure
 *    @param link_status        Link status retrieved from the device
 *
 *    @return PHYMOD_E_NONE     successful function execution
 */


int quadra28_phy_link_status_get(const phymod_phy_access_t* phy,
                              uint32_t* link_status)
{
    return quadra28_link_status(phy, link_status);
}
                   
/**   PHY register read
 *    This function reads the PHY register
 *
 *    @param phy                Pointer to phymod phy access structure
 *    @param reg_addr           PHY Register Address to Read
 *    @param val                PHY Register content of the specified
 *                              register Address to Read
 *
 *    @return PHYMOD_E_NONE     successful function execution
 */
int quadra28_phy_reg_read(const phymod_phy_access_t* phy, uint32_t reg_addr,
                       uint32_t *val)
{
    return phymod_raw_iblk_read(&phy->access,\
                        QUADRA28_CLAUSE45_ADDR((phy->access.devad), (reg_addr)),\
                        val);
}


/**   PHY register write
 *    This function is used to write content to PHY register
 *
 *    @param phy                Pointer to phymod phy access structure
 *    @param reg_addr           PHY Register Address to Write
 *    @param val                Content/Value to Write to the PHY Register
 *
 *    @return PHYMOD_E_NONE     successful function execution
 */
int quadra28_phy_reg_write(const phymod_phy_access_t* phy, uint32_t reg_addr,
                        uint32_t val)
{

    return phymod_raw_iblk_write(&phy->access,\
                         QUADRA28_CLAUSE45_ADDR((phy->access.devad), (reg_addr)),\
                         val);
}

/**   PHY core info get
 *    This function is used to write content to PHY register
 *
 *    @param phy                Pointer to phymod phy access structure
 *    @param info               Pointer to core info structure
 *
 *    @return PHYMOD_E_NONE     successful function execution
 */
int quadra28_core_info_get(const phymod_core_access_t* phy, phymod_core_info_t* info)
{
    uint32_t chip_id;
    PMD_IDENTIFIER_0r_t id0;
    PMD_IDENTIFIER_1r_t id1;

    chip_id =  _quadra28_get_chip_id(&phy->access);
    info->serdes_id = chip_id;
    info->core_version = phymodCoreVersionQuadra28;
    PHYMOD_STRNCPY(info->name, "Quadra28", PHYMOD_STRLEN("Quadra28")+1);

    PHYMOD_IF_ERR_RETURN (
           READ_PMD_IDENTIFIER_1r(&phy->access, &id1));
    info->phy_id1 = id1.v[0];
    PHYMOD_IF_ERR_RETURN (
           READ_PMD_IDENTIFIER_0r(&phy->access, &id0));
    info->phy_id0 = id0.v[0];

    return PHYMOD_E_NONE;
}
/**  PHY polarity set
 *   This function is used to set the lane polarity
 *
 *    @param phy                Pointer to phymod phy access structure
 *    @param polarity           Pointer to phymod_polarity_t for rx and tx
 *
 *    @return PHYMOD_E_NONE     successful function execution
 */
int quadra28_phy_polarity_set(const phymod_phy_access_t* phy, const phymod_polarity_t* polarity)
{
    return quadra28_tx_rx_polarity_set(phy, polarity->tx_polarity, polarity->rx_polarity);
}


/**  PHY polarity get
 *   This function is used to get the lane polarity
 *
 *    @param phy                Pointer to phymod phy access structure
 *    @param polarity           Pointer to phymod_polarity_t for rx and tx
 *
 *    @return PHYMOD_E_NONE     successful function execution
 */
int quadra28_phy_polarity_get(const phymod_phy_access_t* phy, phymod_polarity_t* polarity)
{
    return quadra28_tx_rx_polarity_get(phy, &polarity->tx_polarity, &polarity->rx_polarity);
}

/**  PHY Rx PMD lock
 *   PHY Rx PMD lock This function is used to get the rx PMD lock status
 *
 *    @param phy                Pointer to phymod phy access structure
 *    @param rx_seq_done        Pointer to rx sequence
 *
 *    @return PHYMOD_E_NONE     successful function execution
 */
int quadra28_phy_rx_pmd_locked_get(const phymod_phy_access_t* phy, uint32_t* rx_seq_done){

    return quadra28_pmd_lock_get(phy, rx_seq_done);
}


/**  PHY power set
 *   PHY power set This function is used to set the power
 *
 *    @param phy                Pointer to phymod phy access structure
 *    @param power              Pointer to phymod phy power structure
 *
 *    @return PHYMOD_E_NONE     successful function execution
 */
int quadra28_phy_power_set(const phymod_phy_access_t* phy,
                        const phymod_phy_power_t* power)
{
    return _quadra28_phy_power_set(phy, power);
}

/*
 *    @param flags               Reserved for Furtuer use
 *    @param operation           Operation to perform on PLL sequencer
 *
 *    @return PHYMOD_E_NONE     successful function execution
 */
int quadra28_core_pll_sequencer_restart(const phymod_core_access_t* core, uint32_t flags, phymod_sequencer_operation_t operation)
{

    return  _quadra28_pll_seq_restart(&core->access, flags, operation);

}
/**  FEC enable set
 *   This function is used to enable the FEC
 *
 *    @param phy                Pointer to phymod phy access structure
 *    @param enable             enable or disable
 *        1 - enable
 *        0 - disable
 *
 *    @return PHYMOD_E_NONE     successful function execution
 */
int quadra28_phy_fec_enable_set(const phymod_phy_access_t* phy, uint32_t enable)
{
    return _quadra28_phy_fec_enable_set(phy, enable);
}
/**  FEC enable get
 *   This function is used to check whether or not FEC is enabled
 *
 *    @param phy                Pointer to phymod phy access structure
 *    @param enable             enable or disable
 *        1 - enable
 *        0 - disable
 *
 *    @return PHYMOD_E_NONE     successful function execution
 */
int quadra28_phy_fec_enable_get(const phymod_phy_access_t* phy, uint32_t* enable)
{
    return _quadra28_phy_fec_enable_get(phy, enable);
}
/**  PHY status dump
 *   This function is used to display status dump of a core and for given lane
 *
 *    @param phy                Pointer to phymod phy access structure
 *
 *    @return PHYMOD_E_NONE     successful function execution
 */
int quadra28_phy_status_dump(const phymod_phy_access_t* phy)
{

    PHYMOD_IF_ERR_RETURN(
            _quadra28_phy_status_dump(phy));

    return PHYMOD_E_NONE;

}

static int _quadra28_reset_mode(const phymod_access_t *pa)
{
    int speed = 0;
    BCMI_QUADRA28_GENERAL_PURPOSE_3r_t gp_reg3_reg_val;
    BCMI_QUADRA28_APPS_MODE_0r_t apps_mode0_reg_val;
    PHYMOD_MEMSET(&gp_reg3_reg_val, 0, sizeof(BCMI_QUADRA28_GENERAL_PURPOSE_3r_t));
    PHYMOD_MEMSET(&apps_mode0_reg_val, 0, sizeof(BCMI_QUADRA28_APPS_MODE_0r_t));

    PHYMOD_IF_ERR_RETURN(
        BCMI_QUADRA28_READ_GENERAL_PURPOSE_3r(pa, &gp_reg3_reg_val));
    speed = gp_reg3_reg_val.v[0] & 0xF ;
    if (speed == 0x4 || speed == 0x7) {
        PHYMOD_IF_ERR_RETURN(
            quadra28_channel_select (pa, Q28_ALL_LANE));
        apps_mode0_reg_val.v[0] = gp_reg3_reg_val.v[0];
        BCMI_QUADRA28_APPS_MODE_0r_FINISH_CHANGEf_SET(apps_mode0_reg_val, 0);
        PHYMOD_IF_ERR_RETURN(
            BCMI_QUADRA28_WRITE_APPS_MODE_0r(pa, apps_mode0_reg_val));
        PHYMOD_USLEEP(100);
        BCMI_QUADRA28_APPS_MODE_0r_SET(apps_mode0_reg_val, 0x8882);
        PHYMOD_IF_ERR_RETURN(
            BCMI_QUADRA28_WRITE_APPS_MODE_0r(pa, apps_mode0_reg_val));
        {
            BCMI_QUADRA28_PMD_CONTROLr_t pmd_ctrl;
            PHYMOD_MEMSET(&pmd_ctrl, 0, sizeof(BCMI_QUADRA28_PMD_CONTROLr_t));
            PHYMOD_IF_ERR_RETURN(
                BCMI_QUADRA28_READ_PMD_CONTROLr(pa, &pmd_ctrl));
            BCMI_QUADRA28_PMD_CONTROLr_RESETf_SET(pmd_ctrl, 1);
            PHYMOD_IF_ERR_RETURN(
                BCMI_QUADRA28_WRITE_PMD_CONTROLr(pa, pmd_ctrl));
        }
        PHYMOD_USLEEP(500);
        PHYMOD_IF_ERR_RETURN(
                 _quadra28_intf_update_wait_check(pa,apps_mode0_reg_val.v[0], 50000));
    }
 
    return PHYMOD_E_NONE;
}

int quadra28_core_identify(const phymod_core_access_t* core, uint32_t core_id, uint32_t* is_identified)
{
    uint32_t chip_id;
    PMD_IDENTIFIER_0r_t id0;
    PMD_IDENTIFIER_1r_t id1;
    const phymod_access_t *pm_acc = &core->access;
    
    PHYMOD_MEMSET(&id0, 0, sizeof(PMD_IDENTIFIER_0r_t));
    PHYMOD_MEMSET(&id1, 0, sizeof(PMD_IDENTIFIER_1r_t));
    *is_identified = 0;
    if (core_id == 0) {
        PHYMOD_IF_ERR_RETURN (
           READ_PMD_IDENTIFIER_1r(pm_acc, &id1));
        PHYMOD_IF_ERR_RETURN (
           READ_PMD_IDENTIFIER_0r(pm_acc, &id0));
    } else {
        id0.v[0] = (uint16_t) ((core_id >> 16) & 0xffff);
        id1.v[0] = (uint16_t) core_id & 0xffff;
    }
    if ((id0.v[0] == QUADRA28_PMD_ID0 &&
        id1.v[0] == QUADRA28_PMD_ID1)) {
        if (PHYMOD_ACC_F_PRECONDITION_GET(&core->access)) {
           return _quadra28_reset_mode(&core->access);
        } 
        chip_id =  _quadra28_get_chip_id(&core->access);
        if (chip_id == QUADRA28_82780_CHIP_ID || chip_id == QUADRA28_82752_CHIP_ID 
             || chip_id == QUADRA28_82758_CHIP_ID || chip_id == QUADRA28_8278F_CHIP_ID) {
            /* PHY IDs match and enable Broadcast */
            *is_identified = 0x80000001;
        }
    }
    return PHYMOD_E_NONE;

}
int quadra28_core_reset_set(const phymod_core_access_t* core, phymod_reset_mode_t reset_mode, phymod_reset_direction_t direction)
{
    PHYMOD_IF_ERR_RETURN (
         quadra28_soft_reset(&core->access, reset_mode, direction));
    return PHYMOD_E_NONE;

}

int quadra28_core_reset_get(const phymod_core_access_t* core, phymod_reset_mode_t reset_mode, phymod_reset_direction_t* direction)
{
    return PHYMOD_E_UNAVAIL;

}

int quadra28_core_firmware_info_get(const phymod_core_access_t* core, phymod_core_firmware_info_t* fw_info)
{
    PHYMOD_IF_ERR_RETURN( 
        quadra28_firmware_info_get(&core->access, fw_info));
    return PHYMOD_E_NONE;

}

int quadra28_phy_tx_set(const phymod_phy_access_t* phy, const phymod_tx_t* tx)
{
    return _quadra28_phy_tx_set(phy, tx);
}

int quadra28_phy_tx_get(const phymod_phy_access_t* phy, phymod_tx_t* tx)
{
    PHYMOD_IF_ERR_RETURN( 
        _quadra28_phy_tx_get(phy, tx));
    return PHYMOD_E_NONE;
}

int quadra28_phy_media_type_tx_get(const phymod_phy_access_t* phy, phymod_media_typed_t media, phymod_tx_t* tx)
{
    TXFIR_CONTROL1r_t txfir_ctrl1;
    TXFIR_CONTROL2r_t txfir_ctrl2;

    PHYMOD_MEMSET(tx, 0, sizeof(phymod_tx_t));
    PHYMOD_MEMSET(&txfir_ctrl1, 0, sizeof(TXFIR_CONTROL1r_t));
    PHYMOD_MEMSET(&txfir_ctrl2, 0, sizeof(TXFIR_CONTROL2r_t));
    PHYMOD_IF_ERR_RETURN(
        READ_TXFIR_CONTROL1r(&phy->access, &txfir_ctrl1));
    PHYMOD_IF_ERR_RETURN(
        READ_TXFIR_CONTROL2r(&phy->access, &txfir_ctrl2));

    tx->pre = TXFIR_CONTROL1r_TXFIR_PRE_OVERRIDEf_GET(txfir_ctrl1);
    tx->post = TXFIR_CONTROL1r_TXFIR_POST_OVERRIDEf_GET(txfir_ctrl1);
    tx->main = TXFIR_CONTROL2r_TXFIR_MAIN_OVERRIDEf_GET(txfir_ctrl2);
    tx->post2 = TXFIR_CONTROL2r_TXFIR_POST2f_GET(txfir_ctrl2);

    return PHYMOD_E_NONE;
}
int quadra28_phy_rx_set(const phymod_phy_access_t* phy, const phymod_rx_t* rx)
{
    return _quadra28_phy_rx_set(phy, rx);
}

int quadra28_phy_rx_get(const phymod_phy_access_t* phy, phymod_rx_t* rx)
{
    return _quadra28_phy_rx_get(phy, rx);
}

int quadra28_phy_reset_set(const phymod_phy_access_t* phy, const phymod_phy_reset_t* reset)
{
    return _quadra28_phy_reset_set(phy, reset);
}

int quadra28_phy_reset_get(const phymod_phy_access_t* phy, phymod_phy_reset_t* reset)
{
    return _quadra28_phy_reset_get(phy, reset);
}

int quadra28_phy_tx_lane_control_set(const phymod_phy_access_t* phy, phymod_phy_tx_lane_control_t tx_control)
{
    return quadra28_tx_lane_control_set(phy, tx_control);
} 

int quadra28_phy_tx_lane_control_get(const phymod_phy_access_t* phy, phymod_phy_tx_lane_control_t* tx_control)
{
    int enable = 0;
    *tx_control = phymodTxSquelchOn;

    PHYMOD_IF_ERR_RETURN (
       quadra28_tx_squelch_get(phy, &enable));
    
    if (!enable) {
        *tx_control = phymodTxSquelchOff;
    }
    return PHYMOD_E_NONE;

}

int quadra28_phy_rx_lane_control_set(const phymod_phy_access_t* phy, phymod_phy_rx_lane_control_t rx_control)
{
    return quadra28_rx_lane_control_set(phy, rx_control);
}

int quadra28_phy_rx_lane_control_get(const phymod_phy_access_t* phy, phymod_phy_rx_lane_control_t* rx_control)
{
    int enable = 0;
    *rx_control = phymodRxSquelchOn;
    PHYMOD_IF_ERR_RETURN (
       quadra28_rx_squelch_get(phy, &enable));
    if (!enable) {
        *rx_control = phymodRxSquelchOff;
    }
    return PHYMOD_E_NONE;

}

int quadra28_phy_autoneg_ability_set(const phymod_phy_access_t* phy, const phymod_autoneg_ability_t* an_ability_set_type)
{
    q28_an_ability_t an_ability;
    an_ability.fec_ability = an_ability_set_type->an_fec;

    /*next check pause */
    if (PHYMOD_AN_CAP_SYMM_PAUSE_GET(an_ability_set_type)) {
        an_ability.pause_ability = Q28_SYMM_PAUSE;
    } else if (PHYMOD_AN_CAP_ASYM_PAUSE_GET(an_ability_set_type)) {
        an_ability.pause_ability = Q28_ASYM_PAUSE;
    } else {
        an_ability.pause_ability = Q28_NO_PAUSE;
    }

    /* Quadra28 FW set advertisement based on the speed, so no
     * need to set advert*/
    PHYMOD_IF_ERR_RETURN(_quadra28_phy_autoneg_ability_set(&phy->access, an_ability));

    return PHYMOD_E_NONE;
}

int quadra28_phy_autoneg_ability_get(const phymod_phy_access_t* phy, phymod_autoneg_ability_t* an_ability_get_type)
{
    return _quadra28_phy_autoneg_ability_get(&phy->access, an_ability_get_type);
}

int quadra28_phy_init(const phymod_phy_access_t* phy, const phymod_phy_init_config_t* init_config)
{
    uint32_t datapath = 0, datapath1 = 0;
    PHYMOD_IF_ERR_RETURN(
          phymod_raw_iblk_read(&phy->access, 0x1c91A, &datapath));
    PHYMOD_IF_ERR_RETURN(
          phymod_raw_iblk_read(&phy->access, 0x1c91B, &datapath1));
    datapath |= datapath1;

    PHYMOD_IF_ERR_RETURN
         (quadra28_set_config_mode(phy,
              init_config->interface.interface_type, init_config->interface.data_rate, 
              init_config->interface.ref_clock, datapath, init_config->op_mode));

    /* 0 is the Default init value coming from portmod*/
    if ((init_config->polarity.tx_polarity != 0) || 
          (init_config->polarity.rx_polarity != 0)) {
        PHYMOD_IF_ERR_RETURN(
            quadra28_tx_rx_polarity_set (phy, 
                init_config->polarity.tx_polarity, init_config->polarity.rx_polarity));
    }
    return PHYMOD_E_NONE;
}

int quadra28_phy_loopback_set(const phymod_phy_access_t* phy, phymod_loopback_mode_t loopback, uint32_t enable)
{
    return _quadra28_loopback_set(phy, loopback, enable);
}

int quadra28_phy_loopback_get(const phymod_phy_access_t* phy, phymod_loopback_mode_t loopback, uint32_t* enable)
{
    return _quadra28_loopback_get(phy, loopback, enable);    
}

int quadra28_phy_power_get(const phymod_phy_access_t* phy, phymod_phy_power_t* power)
{
    return _quadra28_phy_power_get(phy, power);
}

int quadra28_phy_autoneg_set(const phymod_phy_access_t* phy, const phymod_autoneg_control_t* an)
{
    return _quadra28_phy_autoneg_set(&phy->access, an);
}

int quadra28_phy_autoneg_get(const phymod_phy_access_t* phy, phymod_autoneg_control_t* an, uint32_t* an_done)
{
    return _quadra28_phy_autoneg_get(&phy->access, an, an_done);

}

int quadra28_phy_autoneg_remote_ability_get(const phymod_phy_access_t* phy, phymod_autoneg_ability_t* an_ability_get_type)
{
    return _quadra28_phy_autoneg_remote_ability_get(&phy->access, an_ability_get_type) ;
}

int quadra28_phy_cl72_set(const phymod_phy_access_t* phy, uint32_t cl72_en)
{
    return _quadra28_phy_cl72_set(phy, cl72_en);
}

int quadra28_phy_cl72_get(const phymod_phy_access_t* phy, uint32_t* cl72_en)
{
    return _quadra28_phy_cl72_get(phy, cl72_en);
}

int quadra28_phy_cl72_status_get(const phymod_phy_access_t* phy, phymod_cl72_status_t* status)
{
    return _quadra28_phy_cl72_status_get(phy, status);
}

int quadra28_phy_i2c_read(const phymod_phy_access_t* phy, uint32_t flags, uint32_t addr, uint32_t offset, uint32_t size, uint8_t* data)
{
      
    return (_quadra28_i2c_read(&phy->access, addr, offset, size, data));
  
}



int quadra28_phy_i2c_write(const phymod_phy_access_t* phy, uint32_t flags, uint32_t addr, uint32_t offset, uint32_t size, const uint8_t* data)
{
        
    return(_quadra28_i2c_write(&phy->access, addr, offset, size, data));
    
}
int quadra28_edc_config_set(const phymod_phy_access_t* phy, const phymod_edc_config_t* edc_config)
{
    return _quadra28_edc_config_set(phy, edc_config);
}
int quadra28_edc_config_get(const phymod_phy_access_t* phy, phymod_edc_config_t* edc_config)
{
    return _quadra28_edc_config_get(phy, edc_config);
}

int quadra28_phy_rx_restart(const phymod_phy_access_t* phy)
{
    return _quadra28_pll_seq_restart(&phy->access, 0, phymodSeqOpRestart);
}

int quadra28_failover_mode_set(const phymod_phy_access_t* phy, phymod_failover_mode_t failover_mode)
{
    return _quadra28_failover_mode_set(phy, failover_mode);
}
int quadra28_failover_mode_get(const phymod_phy_access_t* phy, phymod_failover_mode_t* failover_mode)
{
    return _quadra28_failover_mode_get(phy, failover_mode);
}

int quadra28_phy_rx_adaptation_resume(const phymod_phy_access_t* phy)
{
    return _quadra28_phy_rx_adaptation_resume(phy);
}

int quadra28_phy_gpio_config_set(const phymod_phy_access_t* phy, int pin_no, phymod_gpio_mode_t gpio_mode)
{
    return _quadra28_phy_gpio_config_set(phy, pin_no, gpio_mode);
}
int quadra28_phy_gpio_config_get(const phymod_phy_access_t* phy, int pin_no, phymod_gpio_mode_t* gpio_mode)
{
    return _quadra28_phy_gpio_config_get(phy, pin_no, gpio_mode);
}
int quadra28_phy_gpio_pin_value_set(const phymod_phy_access_t* phy, int pin_no, int value)
{
    return _quadra28_phy_gpio_pin_value_set(phy, pin_no, value);
}
int quadra28_phy_gpio_pin_value_get(const phymod_phy_access_t* phy, int pin_no, int* value)
{
    return _quadra28_phy_gpio_pin_value_get(phy, pin_no, value);
}
int quadra28_core_lane_map_set(const phymod_core_access_t* core, const phymod_lane_map_t* lane_map)
{
    return PHYMOD_E_UNAVAIL;
}
int quadra28_core_lane_map_get(const phymod_core_access_t* core, phymod_lane_map_t* lane_map)
{
    return PHYMOD_E_UNAVAIL;
}
int quadra28_phy_firmware_core_config_set(const phymod_phy_access_t* phy, phymod_firmware_core_config_t fw_core_config)
{
    return PHYMOD_E_UNAVAIL;
}
int quadra28_phy_firmware_core_config_get(const phymod_phy_access_t* phy, phymod_firmware_core_config_t* fw_core_config)
{
    return PHYMOD_E_UNAVAIL;
}
int quadra28_phy_firmware_lane_config_set(const phymod_phy_access_t* phy, phymod_firmware_lane_config_t fw_lane_config)
{
    return PHYMOD_E_UNAVAIL;
}
int quadra28_phy_firmware_lane_config_get(const phymod_phy_access_t* phy, phymod_firmware_lane_config_t* fw_lane_config)
{
    return PHYMOD_E_UNAVAIL;
}
int quadra28_phy_multi_get(const phymod_phy_access_t* phy, phymod_multi_data_t* multi_data)
{
    return (_quadra28_i2c_read(&phy->access, multi_data->dev_addr, multi_data->offset, multi_data->max_size, multi_data->data));
}

int quadra28_phy_op_mode_get(const phymod_phy_access_t* phy, phymod_operation_mode_t* op_mode)
{
    uint32_t retimer = 0, speed = 0, interface_modes = 0;
    phymod_interface_t intf;
    phymod_ref_clk_t ref_clk; 
    
    PHYMOD_IF_ERR_RETURN(
         _quadra28_phy_retimer_enable_get(&phy->access, &retimer));

    PHYMOD_IF_ERR_RETURN(
        quadra28_get_config_mode(&phy->access, &intf, &speed, &ref_clk, &interface_modes));
   
    (void) intf;
    (void) ref_clk;
    (void) interface_modes;
    if ((retimer == 0) && (speed == 1000)) {
        *op_mode = phymodOperationModePassthru; 
    } else if(retimer == 1) {
        *op_mode = phymodOperationModeRetimer; 
    } else {
        *op_mode = phymodOperationModeRepeater; 
    }

    return PHYMOD_E_NONE;
}


#endif 
