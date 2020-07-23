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
#include <phymod/phymod_util.h>

#include "../tier1/sesto_address_defines.h"
#include "../tier1/sesto_reg_structs.h"
#include "../tier1/sesto_cfg_seq.h"

/**   Core identify 
 *    This function reads PMD Identifiers, Set is identified if
 *    sesto identified. 
 *
 *    @param core               Pointer to phymod core access structure 
 *    @param core_id            Core ID which is supplied by interface layer 
 *    @param is_identified      Flag variable to return device 
 *                              identification status 
 *
 *    @return PHYMOD_E_NONE     successful function execution 
 */
int sesto_core_identify(const phymod_core_access_t* core, uint32_t core_id, uint32_t* is_identified)
{
    const phymod_access_t *pm_acc = &core->access;
    uint32_t id0 = 0, chip_id = 0;
    uint32_t id1 = 0;
    uint32_t rev = 0;
    int rv = PHYMOD_E_NONE;
    
    *is_identified = 0;

    SESTO_RESET_SLICE(pm_acc, SESTO_DEV_PMA_PMD);
    if (core_id == 0) {
        READ_SESTO_PMA_PMD_REG(pm_acc, IEEE_PMA_PMD_BLK0_PMD_IDENTIFIER_REGISTER_0_ADR, id0);
        READ_SESTO_PMA_PMD_REG(pm_acc, IEEE_PMA_PMD_BLK0_PMD_IDENTIFIER_REGISTER_1_ADR, id1);
    } else {
        id0 = (core_id >> 16) & 0xffff;
        id1 = core_id & 0xffff;
    }
    if (id0 == SESTO_PMD_ID0 && id1 == SESTO_PMD_ID1) {
        /* PHY IDs match - now check model */
        SESTO_IF_ERR_RETURN(sesto_get_chipid(pm_acc, &chip_id, &rev));
        if (chip_id == SESTO_CHIP_ID_82764 || chip_id == SESTO_CHIP_ID_82792 ||
            chip_id == SESTO_CHIP_ID_82790 || chip_id == SESTO_CHIP_ID_82796)  {
            if (rev == 0xA0) {
                *is_identified = 1;
            } else {
                /* enable Broadcast */
                *is_identified = 0x80000001;
            }
        }
    }

ERR:
    return rv;
}

/**   Core information 
 *    This function gives the core version
 *
 *    @param core               Pointer to phymod core access structure 
 *    @param info               Pointer to core version 
 *
 *    @return PHYMOD_E_NONE     successful function execution 
 */
int sesto_core_info_get(const phymod_core_access_t* core, phymod_core_info_t* info)
{
    const phymod_access_t *pm_acc = &core->access;
    uint32_t chip_id = 0;
    uint32_t id0 = 0;
    uint32_t id1 = 0;
    uint32_t rev = 0;
    char core_name[15]="Sesto";
    int rv = PHYMOD_E_NONE;

    SESTO_IF_ERR_RETURN(sesto_get_chipid(pm_acc, &chip_id, &rev));
    info->serdes_id = chip_id;
    info->core_version = phymodCoreVersionSestoA0;
    if (rev == 0xA0) {
        info->core_version = phymodCoreVersionSestoA0;
        PHYMOD_STRCAT(core_name, "A0");
    } else if (rev == 0xB0){
        info->core_version = phymodCoreVersionSestoB0;
        PHYMOD_STRCAT(core_name, "B0");
    }
    PHYMOD_STRCPY(info->name, core_name);

    READ_SESTO_PMA_PMD_REG(pm_acc, IEEE_PMA_PMD_BLK0_PMD_IDENTIFIER_REGISTER_0_ADR, id0);
    info->phy_id0 = id0;
    READ_SESTO_PMA_PMD_REG(pm_acc, IEEE_PMA_PMD_BLK0_PMD_IDENTIFIER_REGISTER_1_ADR, id1);
    info->phy_id1 = id1;

ERR:
    return rv;
}


int sesto_core_lane_map_set(const phymod_core_access_t* core, const phymod_lane_map_t* lane_map)
{
        
    
    /* Place your code here */

        
    return PHYMOD_E_UNAVAIL;
    
}

int sesto_core_lane_map_get(const phymod_core_access_t* core, phymod_lane_map_t* lane_map)
{
        
    
    /* Place your code here */

        
    return PHYMOD_E_UNAVAIL;
    
}

/**   Core Reset 
 *    This function reset sesto core, it support hard and soft reset
 *
 *    @param core               Pointer to phymod core access structure 
 *    @param reset_mode         Represent hard/soft reset to perform
 *    @param direction          Represet direction of reset. Sesto ignore this parameter
 *
 *    @return PHYMOD_E_NONE     successful function execution 
 */
int sesto_core_reset_set(const phymod_core_access_t* core, phymod_reset_mode_t reset_mode, phymod_reset_direction_t direction)
{
    return _sesto_core_reset_set(&core->access, reset_mode, direction);   
    
}

int sesto_core_reset_get(const phymod_core_access_t* core, phymod_reset_mode_t reset_mode, phymod_reset_direction_t* direction)
{
        
    
    /* Place your code here */

        
    return PHYMOD_E_UNAVAIL;
    
}

/**   Get Firmware info
 *    This function get the firmware information such as master firmware version and master checksum
 *
 *    @param core               Pointer to phymod core access structure 
 *    @param fw_info            Represent firmware version and checksum.
 *    
 *    @return PHYMOD_E_NONE     successful function execution 
 */
int sesto_core_firmware_info_get(const phymod_core_access_t* core, phymod_core_firmware_info_t* fw_info)
{
    int rv = PHYMOD_E_NONE;

    SES_GEN_CNTRLS_FIRMWARE_VERSION_TYPE_T firmware_version;
    SES_GEN_CNTRLS_MST_RUNNING_CHKSUM_TYPE_T mst_running_chksum;

    /* Read the firmware version */
    READ_SESTO_PMA_PMD_REG(&core->access, SES_GEN_CNTRLS_FIRMWARE_VERSION_ADR,
                                     firmware_version.data);
    fw_info->fw_version = firmware_version.data;

    READ_SESTO_PMA_PMD_REG(&core->access,
                                SES_GEN_CNTRLS_MST_RUNNING_CHKSUM_ADR,
                                mst_running_chksum.data);

    fw_info->fw_crc = mst_running_chksum.data; 

ERR:
    return rv;
}

int sesto_phy_firmware_core_config_set(const phymod_phy_access_t* phy, phymod_firmware_core_config_t fw_config)
{
    /* Place your code here */

    return PHYMOD_E_UNAVAIL;
}


int sesto_phy_firmware_core_config_get(const phymod_phy_access_t* phy, phymod_firmware_core_config_t* fw_config)
{
    /* Place your code here */

    return PHYMOD_E_UNAVAIL;
}


int sesto_phy_firmware_lane_config_set(const phymod_phy_access_t* phy, phymod_firmware_lane_config_t fw_config)
{
    return _sesto_firmware_lane_config_set(phy, fw_config);
}


int sesto_phy_firmware_lane_config_get(const phymod_phy_access_t* phy, phymod_firmware_lane_config_t* fw_config)
{
    return _sesto_firmware_lane_config_get(phy, fw_config);
}

/**  Restart PLL Sequecer
 *   This function is used to get restart the PLL sequencer
 *
 *
 *    @param core                Pointer to phymod core access structure
 *    @param flags               Reserved for Furtuer use
 *    @param operation           Operation to perform on PLL sequencer
 *
 *    @return PHYMOD_E_NONE     successful function execution
 */

int sesto_core_pll_sequencer_restart(const phymod_core_access_t* core, uint32_t flags, phymod_sequencer_operation_t operation)
{
    return _sesto_pll_sequencer_restart(core, operation);
}


int sesto_core_wait_event(const phymod_core_access_t* core, phymod_core_event_t event, uint32_t timeout)
{
        
    
    /* Place your code here */

        
    return PHYMOD_E_UNAVAIL;
    
}


int sesto_phy_rx_restart(const phymod_phy_access_t* phy)
{
    phymod_core_access_t core;
    PHYMOD_MEMCPY(&core, phy, sizeof(phymod_core_access_t));

    return _sesto_pll_sequencer_restart(&core, phymodSeqOpRestart);
}

/**  PHY polarity set
 *   This function is used to set the lane polarity
 *
 *   @param phy          Pointer to phymod phy access structure
 *   @param polarity     Pointer to phymod_polarity_t for rx and tx
 * 
 *   @return PHYMOD_E_NONE     successful function execution
 */


int sesto_phy_polarity_set(const phymod_phy_access_t* phy, const phymod_polarity_t* polarity)
{
    return _sesto_tx_rx_polarity_set(phy, polarity->tx_polarity, polarity->rx_polarity);
}

/**  PHY polarity get
 *   This function is used to get the lane polarity
 *
 *   @param phy          Pointer to phymod phy access structure
 *   @param polarity     Pointer to phymod_polarity_t for rx and tx
 *
 *   @return PHYMOD_E_NONE     successful function execution
 */

int sesto_phy_polarity_get(const phymod_phy_access_t* phy, phymod_polarity_t* polarity)
{
    return _sesto_tx_rx_polarity_get(phy, &polarity->tx_polarity, &polarity->rx_polarity);
}

/**  Tx set
 *   This function is used to set transmitter analog parameters
 *
 *    @param phy                Pointer to phymod phy access structure
 *    @param tx                 Pointer to tx param structure
 *
 *    @return PHYMOD_E_NONE     successful function execution
 */
int sesto_phy_tx_set(const phymod_phy_access_t* phy, const phymod_tx_t* tx)
{
    return _sesto_tx_set(phy, tx);
}

/**  Tx get
 *   This function is used to get transmitter analog parameters
 *
 *    @param phy                Pointer to phymod phy access structure
 *    @param tx                 Pointer to tx param structure
 *
 *    @return PHYMOD_E_NONE     successful function execution
 */
int sesto_phy_tx_get(const phymod_phy_access_t* phy, phymod_tx_t* tx)
{
    return _sesto_tx_get(phy, tx);
}




int sesto_phy_media_type_tx_get(const phymod_phy_access_t* phy, phymod_media_typed_t media, phymod_tx_t* tx)
{
    phymod_phy_access_t phy_copy;

    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phymod_phy_access_t));
    /* system side interface bit shift */
    phy_copy.port_loc = phymodPortLocSys;

    return _sesto_tx_get(&phy_copy, tx);
}


int sesto_phy_tx_override_set(const phymod_phy_access_t* phy, const phymod_tx_override_t* tx_override)
{
        
    
    /* Place your code here */

        
    return PHYMOD_E_UNAVAIL;
    
}

int sesto_phy_tx_override_get(const phymod_phy_access_t* phy, phymod_tx_override_t* tx_override)
{
        
    
    /* Place your code here */

        
    return PHYMOD_E_UNAVAIL;
    
}

/**  Rx set
 *   This function is used to set receiver analog parameters
 *
 *    @param phy                Pointer to phymod phy access structure
 *    @param rx                 Pointer to rx param structure
 *
 *    @return PHYMOD_E_NONE     successful function execution
 */
int sesto_phy_rx_set(const phymod_phy_access_t* phy, const phymod_rx_t* rx)
{
    return _sesto_rx_set(phy, rx);
}

/**  Rx get
 *   This function is used to get receiver analog parameters
 *
 *    @param phy                Pointer to phymod phy access structure
 *    @param rx                 Pointer to rx param structure
 *
 *    @return PHYMOD_E_NONE     successful function execution
 */
int sesto_phy_rx_get(const phymod_phy_access_t* phy, phymod_rx_t* rx)
{
    return _sesto_rx_get(phy, rx);
}

/**  PHY Rx adaptation resume
 *   This function is used to perform PHY reset
 *
 *    @param phy                Pointer to phymod phy access structure
 *
 *    @return PHYMOD_E_NONE     successful function execution
 */
int sesto_phy_rx_adaptation_resume(const phymod_phy_access_t* phy)
{
    return _sesto_rx_adaptation_resume(phy);
}


/**  Tx Rx Phy Reset Set
 *   This function is used to set TX/RX Phy Reset options
 *
 *    @param phy                Pointer to phymod phy access structure
 *    @param reset              Pointer to phymod phy reset structure
 *                              to set TX Reset and RX Reset
 *
 *    @return PHYMOD_E_NONE     successful function execution
 */

int sesto_phy_reset_set(const phymod_phy_access_t* phy, const phymod_phy_reset_t* reset)
{
    return _sesto_phy_reset_set(phy, reset);
}
/**  Tx Rx Phy Reset Get
 *   This function is used to get TX/RX Phy Reset options
 *
 *    @param phy                Pointer to phymod phy access structure
 *    @param reset              Pointer to phymod phy reset structure
 *                              to get TX Reset and RX Reset
 *
 *    @return PHYMOD_E_NONE     successful function execution
 */
int sesto_phy_reset_get(const phymod_phy_access_t* phy, phymod_phy_reset_t* reset)
{
    return _sesto_phy_reset_get(phy, reset);
}

/**  Tx Rx power Set
 *   This function is used to set TX/RX power options
 *
 *    @param phy                Pointer to phymod phy access structure
 *    @param power              Pointer to phymod phy power structure
 *                              to set TX Power and RX Power
 *
 *    @return PHYMOD_E_NONE     successful function execution
 */

int sesto_phy_power_set(const phymod_phy_access_t* phy, const phymod_phy_power_t* power)
{
    int rv = PHYMOD_E_NONE;

    SESTO_IF_ERR_RETURN(_sesto_tx_power_set(phy, power->tx));
    SESTO_IF_ERR_RETURN(_sesto_rx_power_set(phy, power->rx));

ERR:
    return rv;
}

/**  Tx Rx power Get
 *   This function is used to get TX/RX power options
 *
 *    @param phy                Pointer to phymod phy access structure
 *    @param power              Pointer to phymod phy power structure
 *                              to get TX Power and RX Power
 *
 *    @return PHYMOD_E_NONE     successful function execution
 */

int sesto_phy_power_get(const phymod_phy_access_t* phy, phymod_phy_power_t* power)
{
    return _sesto_tx_rx_power_get(phy, power);
}

/**  Tx lane control set
 *   This function is used to set the lane control for example
 *   resetting the traffic
 *
 *    @param phy                Pointer to phymod phy access structure
 *    @param tx_control         Tx lane control
 *          0 - Traffic disable, currently not available
 *          1 - Traffic enable, currently not available
 *          2 - Tx Datapath reset
 *          3 - Tx Squelch on
 *          4 - Tx Squelch off
 *
 *    @return PHYMOD_E_NONE     successful function execution
 */

int sesto_phy_tx_lane_control_set(const phymod_phy_access_t* phy, phymod_phy_tx_lane_control_t tx_control)
{
    return _sesto_tx_lane_control_set(phy, tx_control);
}

/**  Tx lane control get
 *   This function is used to get the lane control for example
 *   resetting the traffic
 *
 *  @param phy                Pointer to phymod phy access structure
 *  @param tx_control
 *          0 - Traffic disable, currently not available
 *          1 - Traffic enable, currently not available
 *          2 - Tx Datapath reset
 *          3 - Tx Squelch on
 *          4 - Tx Squelch off
 *
 *          Return value of tx_control is enable or disable of specified lane control option
 *
 *    @return PHYMOD_E_NONE     successful function execution
 */


int sesto_phy_tx_lane_control_get(const phymod_phy_access_t* phy, phymod_phy_tx_lane_control_t* tx_control)
{
    return _sesto_tx_lane_control_get(phy, tx_control);
}

/**  Rx lane control set
 *   This function is used to set the lane control for example
 *   resetting the traffic
 *
 *    @param phy                Pointer to phymod phy access structure
 *    @param rx_control         Rx lane control
 *          0 - Rx datapath reset
 *          1 - Rx squelch on
 *          2 - Rx squelch off
 *
 *    @return PHYMOD_E_NONE     successful function execution
 */
int sesto_phy_rx_lane_control_set(const phymod_phy_access_t* phy, phymod_phy_rx_lane_control_t rx_control)
{
    return _sesto_rx_lane_control_set(phy, rx_control);
}
/**  Rx lane control get
 *   This function is used to set the lane control for example
 *   resetting the traffic
 *
 *    @param phy                Pointer to phymod phy access structure
 *    @param rx_control         Rx lane control
 *          0 - Rx datapath reset
 *          1 - Rx squelch on
 *          2 - Rx squelch off
 *
 *          Return value of tx_control is enable or disable of specified lane control option
 *
 *    @return PHYMOD_E_NONE     successful function execution
 */

int sesto_phy_rx_lane_control_get(const phymod_phy_access_t* phy, phymod_phy_rx_lane_control_t* rx_control)
{
    return _sesto_rx_lane_control_get(phy, rx_control);
}

/**   Set interface config
 *    This function sets interface configuration (interface, speed, frequency of
 *    operation, PT mode, BCM84793 etc). 
 *    This is requried to put the chip into specific mode specified
 *
 *    NOTE: Default system side interface type will be IEEE mode and speed is compatible with lane side configured speed.
 *     Ex: User configured 40G speed with interface type KR4 on lane side then system side speed is 40G and interface type is IEEE mode.
 *
 *    @param phy                Pointer to phymod phy access structure 
 *    @param flags              Reserved for future use
 *    @param config             Interface config structure where user specifies 
 *                                - Interface Type
 *                                - Speed 
 *                                - Frequency.
 *                                - device_aux_modes : a structure that have auxilary mode details
 *                                   -> pass_thru : To enable repeater on 10/40G mode of operation
 *                                   -> gearbox_100g_inverse_mode : Applicable only for 100G mode, 25G lanes on system side and
 *                                      10G lanes on line side
 *                                   -> BCM84793_capablity : To enable BCM84793 compatible on lanes
 *                                   -> passthru_sys_side_core : When pass through is enabled user are allowed to configure system side
 *                                      core i.e. 25G lanes or 10G lanes
 *                                   -> reserved: Only for future use
 *    
 *    @return PHYMOD_E_NONE     successful function execution 
 */

int sesto_phy_interface_config_set(const phymod_phy_access_t* phy, uint32_t flags, const phymod_phy_inf_config_t* config)
{
    uint16_t data1 = 0, retry_cnt = SESTO_FW_DLOAD_RETRY_CNT;
    uint32_t chip_id = 0, rev = 0;
    phymod_phy_inf_config_t cfg;
    SESTO_DEVICE_AUX_MODE_T *aux_mode;
    uint16_t lane = 0, lane_mask = 0;
    int rv = PHYMOD_E_NONE;
 
    PHYMOD_MEMSET(&cfg, 0, sizeof(phymod_phy_inf_config_t));
    PHYMOD_MEMCPY(&cfg, config, sizeof(phymod_phy_inf_config_t));
    cfg.device_aux_modes = PHYMOD_MALLOC(sizeof(SESTO_DEVICE_AUX_MODE_T), "sesto_device_aux_mode");
    aux_mode = (SESTO_DEVICE_AUX_MODE_T*) cfg.device_aux_modes;
    if (aux_mode == NULL) {
        PHYMOD_DEBUG_VERBOSE(("AUX MODE MEM NOT ALLOC\n"));
        return PHYMOD_E_PARAM;
    }

    PHYMOD_MEMSET(aux_mode, 0, sizeof(SESTO_DEVICE_AUX_MODE_T));
    /* Filling device aux mode */
    SESTO_IF_ERR_RETURN(sesto_get_chipid(&phy->access, &chip_id, &rev));
    if (chip_id != SESTO_CHIP_ID_82764) {
        aux_mode->passthru_sys_side_core = PHYMOD_INTF_CONFIG_PORT_PHY_MODE_REVERSE_GET(phy) ? SESTO_FALCON_CORE: SESTO_MERLIN_CORE;
    } else {
        aux_mode->passthru_sys_side_core = PHYMOD_INTF_CONFIG_PORT_PHY_MODE_REVERSE_GET(phy) ? SESTO_MERLIN_CORE: SESTO_FALCON_CORE;
    }
    aux_mode->BCM84793_capablity = PHYMOD_INTF_CONFIG_PHY_PIN_COMPATIBILITY_ENABLE_GET(phy);
    aux_mode->pass_thru = (!PHYMOD_INTF_CONFIG_PHY_GEARBOX_ENABLE_GET(phy));

    if ((config->data_rate == SESTO_SPD_1G) && (aux_mode->passthru_sys_side_core != SESTO_FALCON_CORE)) {
            PHYMOD_RETURN_WITH_ERR(PHYMOD_E_INIT,
                (_PHYMOD_MSG("ERR:: 1G datarate is supported only if Merlin is on line side\n")));
    }

    SESTO_IF_ERR_RETURN(
            _sesto_phy_interface_config_set(phy, flags, &cfg));

    /* Enable FW After configuring mode */
    SESTO_IF_ERR_RETURN(
        _sesto_fw_enable(&phy->access, SESTO_ENABLE)); 
    do {
        SESTO_CHIP_FIELD_READ(&phy->access, SES_GEN_CNTRLS_FIRMWARE_ENABLE, fw_enable_val, data1); 
     
        PHYMOD_DEBUG_VERBOSE(("FW Clear:%d\n",data1));
        retry_cnt--;
    } while((data1 != 0) && (retry_cnt));
    if (retry_cnt == 0) {
        PHYMOD_RETURN_WITH_ERR(PHYMOD_E_INIT,
                (_PHYMOD_MSG("WARN:: FW Enable not cleared\n")));
    }

    /* For 1G support */
    lane_mask = PHYMOD_ACC_LANE_MASK(&phy->access);
    if (config->data_rate == SESTO_SPD_1G) {
        for (lane = 0; lane < SESTO_MAX_MERLIN_LANE; lane ++) {
            if (lane_mask & (1 << lane)) {
                SESTO_IF_ERR_RETURN(_sesto_set_slice_reg (&phy->access,
                            SESTO_SLICE_UNICAST, SESTO_MERLIN_CORE, SESTO_DEV_PMA_PMD, 0, lane));
                /* Merlin specific register to disable ucode monitor controls */
                WRITE_SESTO_PMA_PMD_REG(&phy->access, 0xD00E, 0x000D);
                WRITE_SESTO_PMA_PMD_REG(&phy->access, 0xD00D, 0x7F07);
                WRITE_SESTO_PMA_PMD_REG(&phy->access, 0xD001, 0x215);
                WRITE_SESTO_PMA_PMD_REG(&phy->access, 0xD004, 0x901);
            }
        }
    }

ERR:
    SESTO_RESET_SLICE(&phy->access, SESTO_DEV_PMA_PMD);
    PHYMOD_FREE(cfg.device_aux_modes);

    return rv;
}

/**   Get interface config
 *    This function gets interface configuration (interface, speed, frequency of
 *    operation, PT mode, BCM84793 etc). 
 *
 *    NOTE: Default system side interface type will be IEEE mode and speed is compatible with lane side configured speed.
 *     Ex: User configured 40G speed with interface type KR4 on lane side then system side speed is 40G and interface type is IEEE mode.
 *
 *    @param phy                Pointer to phymod phy access structure 
 *    @param flags              Reserved for future use
 *    @param config             Gets Interface config such as,
 *                                - Interface Type
 *                                - Speed 
 *                                - Frequency.
 *                                - device_aux_modes : a structure that have auxilary mode details
 *                                   -> pass_thru : To enable repeater on 10/40G mode of operation
 *                                   -> gearbox_100g_inverse_mode : Applicable only for 100G mode, 25G lanes on system side and
 *                                      10G lanes on line side
 *                                   -> BCM84793_capablity : To enable BCM84793 compatible on lanes
 *                                   -> passthru_sys_side_core : When pass through is enabled user are allowed to configure system side
 *                                      core i.e. 25G lanes or 10G lanes
 *                                   -> reserved: Only for future use
 *    
 *    @return PHYMOD_E_NONE     successful function execution 
 */

int sesto_phy_interface_config_get(const phymod_phy_access_t* phy, uint32_t flags, uint32_t ref_clock, phymod_phy_inf_config_t* config)
{
    SESTO_DEVICE_AUX_MODE_T  *aux_mode;
    int rv = PHYMOD_E_NONE;

    config->device_aux_modes = PHYMOD_MALLOC(sizeof(SESTO_DEVICE_AUX_MODE_T), "sesto_device_aux_mode");
    aux_mode = (SESTO_DEVICE_AUX_MODE_T*) config->device_aux_modes;
    if (aux_mode == NULL) {
        PHYMOD_DEBUG_VERBOSE(("AUX MODE MEM NOT ALLOC\n"));
        return PHYMOD_E_PARAM;
    }
    PHYMOD_MEMSET(aux_mode, 0, sizeof(SESTO_DEVICE_AUX_MODE_T));

    SESTO_IF_ERR_RETURN (
            _sesto_phy_interface_config_get(phy, flags, config));
        
ERR:
    PHYMOD_FREE(config->device_aux_modes);

    return rv;
}

/**  PHY CL73 ability Set
 *   This function is used to set the CL73 ability of a lane
 *
 *    @param phy                Pointer to phymod phy access structure
 *    @param an_ability         Pointer to the ability of cl73
 *
 *    @return PHYMOD_E_NONE     successful function execution
 */

int sesto_phy_autoneg_ability_set(const phymod_phy_access_t* phy, const phymod_autoneg_ability_t* an_ability)
{
    sesto_an_ability_t value;
    int rv = PHYMOD_E_NONE;

    PHYMOD_MEMSET(&value, 0, sizeof(value));
    value.cl73_adv.an_fec = an_ability->an_fec;
    value.cl73_adv.an_cl72 = an_ability->an_cl72;
    value.an_master_lane = an_ability->an_master_lane;
    /*check if sgmii  or not */
    if (PHYMOD_AN_CAP_SGMII_GET(an_ability)){
        return rv;
    }

    /*next check pause */
    if (PHYMOD_AN_CAP_SYMM_PAUSE_GET(an_ability)) {
        value.cl73_adv.an_pause = SESTO_SYMM_PAUSE;
    } else if (PHYMOD_AN_CAP_ASYM_PAUSE_GET(an_ability)) {
        value.cl73_adv.an_pause = SESTO_ASYM_PAUSE;
    } else {
        value.cl73_adv.an_pause = SESTO_NO_PAUSE;
    }

    /*check cl73 and cl73 bam ability */
    if (PHYMOD_AN_CAP_1G_KX_GET(an_ability->an_cap)) {
        value.cl73_adv.an_base_speed = SESTO_CL73_1000BASE_KX;
    } else if (PHYMOD_AN_CAP_10G_KX4_GET(an_ability->an_cap)) {
        value.cl73_adv.an_base_speed = SESTO_CL73_10GBASE_KX4;
    } else if (PHYMOD_AN_CAP_10G_KR_GET(an_ability->an_cap)) {
        value.cl73_adv.an_base_speed = SESTO_CL73_10GBASE_KR;
    } else if (PHYMOD_AN_CAP_40G_KR4_GET(an_ability->an_cap)) {
        value.cl73_adv.an_base_speed = SESTO_CL73_40GBASE_KR4;
    } else if (PHYMOD_AN_CAP_40G_CR4_GET(an_ability->an_cap)) {
        value.cl73_adv.an_base_speed = SESTO_CL73_40GBASE_CR4;
    } else if (PHYMOD_AN_CAP_100G_CR4_GET(an_ability->an_cap)) {
        value.cl73_adv.an_base_speed = SESTO_CL73_100GBASE_CR4;
    } else if (PHYMOD_AN_CAP_100G_KR4_GET(an_ability->an_cap)) {
        value.cl73_adv.an_base_speed = SESTO_CL73_100GBASE_KR4;
    } else if (an_ability->an_cap == 0) {
        return rv;
    } else {
        return PHYMOD_E_PARAM;
    }

    SESTO_IF_ERR_RETURN
        (_sesto_autoneg_ability_set(phy, &value));

ERR:
    return rv;
}

/**  PHY CL73 remote ability get
 *   This function is used to get the CL73 remote ability of a lane
 *
 *    @param phy                Pointer to phymod phy access structure
 *    @param an_ability         Pointer to the ability of cl73
 *
 *    @return PHYMOD_E_NONE     successful function execution
 */
int sesto_phy_autoneg_remote_ability_get(const phymod_phy_access_t* phy, phymod_autoneg_ability_t* an_ability_get)
{
    int rv = PHYMOD_E_NONE;

    SESTO_IF_ERR_RETURN(
        _sesto_autoneg_remote_ability_get(phy, an_ability_get));

ERR:
    return rv;
}

/**  PHY CL73 ability get
 *   This function is used to get the CL73 ability of a lane
 *
 *    @param phy                Pointer to phymod phy access structure
 *    @param an_ability         Pointer to the ability of cl73
 *
 *    @return PHYMOD_E_NONE     successful function execution
 */

int sesto_phy_autoneg_ability_get(const phymod_phy_access_t* phy, phymod_autoneg_ability_t* an_ability_get_type)
{
    sesto_an_ability_t value;
    int rv = PHYMOD_E_NONE;

    SESTO_IF_ERR_RETURN(
        _sesto_autoneg_ability_get(phy, &value));

    an_ability_get_type->an_fec = value.cl73_adv.an_fec;
    switch (value.cl73_adv.an_pause) {
        case SESTO_ASYM_PAUSE:
            PHYMOD_AN_CAP_ASYM_PAUSE_SET(an_ability_get_type);
        break;
        case SESTO_SYMM_PAUSE:
            PHYMOD_AN_CAP_SYMM_PAUSE_SET(an_ability_get_type);
        break;
        default:
            break;
    }

    /*first check cl73 ability*/
    switch (value.cl73_adv.an_base_speed) {
        case SESTO_CL73_40GBASE_CR4:
            PHYMOD_AN_CAP_40G_CR4_SET(an_ability_get_type->an_cap);
        break;
        case SESTO_CL73_40GBASE_KR4:
            PHYMOD_AN_CAP_40G_KR4_SET(an_ability_get_type->an_cap);
        break;
        case SESTO_CL73_100GBASE_CR4:
            PHYMOD_AN_CAP_100G_CR4_SET(an_ability_get_type->an_cap);
        break;
        case SESTO_CL73_100GBASE_KR4:
            PHYMOD_AN_CAP_100G_KR4_SET(an_ability_get_type->an_cap);
        break;
        default:
            break;
    }

ERR:
    return rv;
}

/**  PHY enable/Disable CL73
 *   This function is used to enable/disable CL73 of a lane
 *
 *    @param phy                Pointer to phymod phy access structure
 *    @param an                 Pointer to the AN control
 *
 *    @return PHYMOD_E_NONE     successful function execution
 */

int sesto_phy_autoneg_set(const phymod_phy_access_t* phy, const phymod_autoneg_control_t* an)
{
    return (_sesto_autoneg_set(phy, an));
}

/**  Get CL73 completion state
 *   This function is used get CL73 completion state of a lane
 *
 *    @param phy                Pointer to phymod phy access structure
 *    @param an                 Pointer to the AN control
 *    @param an_done            Pointer to the AN completion state
 *
 *    @return PHYMOD_E_NONE     successful function execution
 */

int sesto_phy_autoneg_get(const phymod_phy_access_t* phy, phymod_autoneg_control_t* an, uint32_t* an_done)
{
    return (_sesto_autoneg_get(phy, an, an_done));
}


int sesto_phy_autoneg_status_get(const phymod_phy_access_t* phy, phymod_autoneg_status_t* status)
{
        
    
    /* Place your code here */

        
    return PHYMOD_E_UNAVAIL;
    
}

/**   Sesto core initialization
 *    This function initialize the sesto core by downlaoding the firmware.
 *
 *    @param core               Pointer to phymod core access structure 
 *    @param init_config        Init configuration specified by user 
 *    @param core_status        PMD status read from PHY chip 
 *
 *    @return PHYMOD_E_NONE     successful function execution 
 */

int sesto_core_init(const phymod_core_access_t* core, const phymod_core_init_config_t* init_config, const phymod_core_status_t* core_status)
{
    uint32_t chip_id = 0, rev = 0;
    uint8_t sys_side_core = 0;
    int rv = PHYMOD_E_NONE;

    SESTO_IF_ERR_RETURN(
            _sesto_core_init(core, init_config));
    
    SESTO_IF_ERR_RETURN(sesto_get_chipid(&core->access, &chip_id, &rev));

    if (chip_id != SESTO_CHIP_ID_82764) {
        sys_side_core = PHYMOD_INTF_CONFIG_PORT_PHY_MODE_REVERSE_GET(core) ? SESTO_FALCON_CORE: SESTO_MERLIN_CORE;
    } else {
        sys_side_core = PHYMOD_INTF_CONFIG_PORT_PHY_MODE_REVERSE_GET(core) ? SESTO_MERLIN_CORE: SESTO_FALCON_CORE;
    }
    /* Added chip restriction fix:
       The line side core selection(whether 10G or 20G/25G is the line side)
       should be made first with the Mode Config API before enabling any other configuration.*/
    SESTO_CHIP_FIELD_WRITE(&core->access, DP_SESTO_MODE_CTRL0, mode_falcon_line, !sys_side_core);

ERR:
    return rv;

}


int sesto_phy_init(const phymod_phy_access_t* phy, const phymod_phy_init_config_t* init_config)
{
    int rv = PHYMOD_E_NONE;
    phymod_phy_access_t phy_copy;

    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phymod_phy_access_t));

    SESTO_IF_ERR_RETURN
        (sesto_phy_interface_config_set(phy, 0 /*flags*/, &init_config->interface));
    SESTO_IF_ERR_RETURN
        (_sesto_core_cfg_polarity_set(phy, init_config->polarity.tx_polarity, init_config->polarity.rx_polarity));
    SESTO_IF_ERR_RETURN
        (_sesto_phy_repeater_mode_set(phy, init_config->op_mode));
    SESTO_IF_ERR_RETURN
        (_sesto_core_cfg_tx_set(phy,init_config->tx));
    /* Looback disable during phy_init to be defult */
    SESTO_IF_ERR_RETURN
        (_sesto_loopback_set(phy, phymodLoopbackGlobalPMD, SESTO_DISABLE));
    SESTO_IF_ERR_RETURN
        (_sesto_loopback_set(phy, phymodLoopbackRemotePMD, SESTO_DISABLE));
    /* system side interface bit shift */
    phy_copy.port_loc = phymodPortLocSys;
    SESTO_IF_ERR_RETURN
        (_sesto_loopback_set(&phy_copy, phymodLoopbackGlobalPMD, SESTO_DISABLE));
    SESTO_IF_ERR_RETURN
        (_sesto_loopback_set(&phy_copy, phymodLoopbackRemotePMD, SESTO_DISABLE));

ERR:
    return rv;
}

/**  Set loopback
 *   This function is used to set a particular loopback mode
 *
 *    @param phy                Pointer to phymod phy access structure
 *    @param loopback           Type of loopback
 *        0 - Global loopback
 *        1 - Global PMD loopback
 *        2 - Remote PMD loopback
 *        3 - Remote PCS loopback
 *        4 - Digital PMD loopback
 *    @enable                    Enable or disable
 *        0 - disable
 *        1 - enable
 *
 *    @return PHYMOD_E_NONE     successful function execution
 */
int sesto_phy_loopback_set(const phymod_phy_access_t* phy, phymod_loopback_mode_t loopback, uint32_t enable)
{
    return _sesto_loopback_set(phy, loopback, enable);
}

/**  Get loopback
 *   This function is used to get the status of a particular loopback mode
 *   whether or not enabled
 *
 *    @param phy                Pointer to phymod phy access structure
 *    @param loopback           Type of loopback
 *        0 - Global loopback
 *        1 - Global PMD loopback
 *        2 - Remote PMD loopback
 *        3 - Remote PCS loopback
 *        4 - Digital PMD loopback
 *    @enable                    Enable or disable
 *        0 - disable
 *        1 - enable
 *
 *    @return PHYMOD_E_NONE     successful function execution
 */
int sesto_phy_loopback_get(const phymod_phy_access_t* phy, phymod_loopback_mode_t loopback, uint32_t* enable)
{
    return _sesto_loopback_get(phy, loopback, enable);
}
/**   Get PHY RX PMD link status 
 *    This function retrieves RX PMD lock status of sesto 
 *
 *    @param phy                Pointer to phymod phy access structure 
 *    @param rx_pmd_locked      Rx PMD Link status retrieved from Sesto
 *
 *    @return PHYMOD_E_NONE     successful function execution 
 */

int sesto_phy_rx_pmd_locked_get(const phymod_phy_access_t* phy, uint32_t* rx_pmd_locked)
{
    return (_sesto_rx_pmd_lock_get(phy, rx_pmd_locked));
}

/**   Get PCS link status
 *    This function retrieves PCS link status of sesto when PCS Monitor is enabled,
 *    This function return PMD lock when PCS monitor is disabled.
 *
 *    @param phy                Pointer to phymod phy access structure 
 *    @param link_status        Retrives PCS Link of Sesto
 *
 *    @return PHYMOD_E_NONE     successful function execution 
 */

int sesto_phy_link_status_get(const phymod_phy_access_t* phy, uint32_t* link_status)
{
    return _sesto_phy_pcs_link_get(phy, link_status);
}

/**   Read Register
 *    This function read user specified register based on the specified Device type 
 *
 *    @param phy                Pointer to phymod phy access structure.
 *    @param reg_addr           Sesto register address
 *    @param val                Output parameter, represents the value of address specified
 *
 *    @return PHYMOD_E_NONE     successful function execution 
 */
int sesto_phy_reg_read(const phymod_phy_access_t* phy, uint32_t reg_addr, uint32_t* val)
{
    uint16_t dev_add = 0;
    int rv = PHYMOD_E_NONE;

    dev_add = (phy->access.devad) ? phy->access.devad : SESTO_DEV_PMA_PMD;

    if (dev_add == SESTO_DEV_PMA_PMD) {
        READ_SESTO_PMA_PMD_REG(&phy->access, reg_addr, *val);
    } else {
        READ_SESTO_AN_REG(&phy->access, reg_addr, *val);
    }

ERR:
    return rv;
}

/**   Write Register
 *    This function write user specified value to the specified address 
 *    based on the specified Device type 
 *
 *    @param phy                Pointer to phymod phy access structure.
 *    @param reg_addr           Sesto register address
 *    @param val                Represents the value to be written
 *
 *    @return PHYMOD_E_NONE     successful function execution 
 */
int sesto_phy_reg_write(const phymod_phy_access_t* phy, uint32_t reg_addr, uint32_t val)
{
    uint16_t dev_add = 0;
    int rv = PHYMOD_E_NONE;

    dev_add = (phy->access.devad) ? phy->access.devad : SESTO_DEV_PMA_PMD;
    PHYMOD_DEBUG_VERBOSE(("WRITE reg:%x data:%x\n", reg_addr, val));
    if (dev_add == SESTO_DEV_PMA_PMD) {
        WRITE_SESTO_PMA_PMD_REG(&phy->access, reg_addr, val);
    } else {
        WRITE_SESTO_AN_REG(&phy->access, reg_addr, val);
    }
        
ERR:
    return rv;
}

/**   Retrives Revision 
 *    This function retrives sesto revision ID 
 *
 *    @param phy                Pointer to phymod phy access structure.
 *    @param rev_id             Output parameter, Represents the revision ID of sesto.
 *
 *    @return PHYMOD_E_NONE     successful function execution 
 */
int sesto_phy_rev_id(const phymod_phy_access_t* phy, uint32_t *rev_id)
{
    uint32_t chip_id = 0;
    int rv = PHYMOD_E_NONE;

    SESTO_IF_ERR_RETURN (
        sesto_get_chipid(&phy->access, &chip_id, rev_id));
 
    PHYMOD_DEBUG_VERBOSE(("CHIPID : 0x%x\n", chip_id));

ERR:
    return rv;
}

/**  Set CL72
 *   This function is used to enable/disable forced Tx training
 *
 *    @param phy                Pointer to phymod phy access structure
 *    @param enable             enable / disable force Tx training
 *        1 - enable
 *        0 - disable
 *
 *    @return PHYMOD_E_NONE     successful function execution
 */
int sesto_phy_cl72_set(const phymod_phy_access_t* phy, uint32_t enable)
{
    return _sesto_force_tx_training_set(phy, enable);
}


/**  Get CL72
 *   This function is used to get the status whether Tx training is enabled or disabled
 *
 *    @param phy                Pointer to phymod phy access structure
 *    @param enable             enable or disable
 *        1 - enable
 *        0 - disable
 *
 *    @return PHYMOD_E_NONE     successful function execution
 */
int sesto_phy_cl72_get(const phymod_phy_access_t* phy, uint32_t *enable)
{
    return _sesto_force_tx_training_get(phy, enable);
}

/**  Get CL72 status
 *   This function is used to get the status of Tx training
 *
 *    @param phy                Pointer to phymod phy access structure
 *    @param status             CL72 Training status
 *
 *    @return PHYMOD_E_NONE     successful function execution
 */
int sesto_phy_cl72_status_get(const phymod_phy_access_t* phy, phymod_cl72_status_t* status)
{
    return _sesto_force_tx_training_status_get(phy, status);
}

/**  Interrupt status get
 *   This function is used to check whether or not particualr interrupt is pending
 *
 *    @param phy                Pointer to phymod phy access structure
 *
 *    @param intr_status             Pending status
 *        1 - Pending
 *        0 - Serviced
 *
 *    @return PHYMOD_E_NONE     successful function execution
 */
int sesto_phy_intr_status_get(const phymod_phy_access_t *phy, uint32_t* intr_status)
{
    uint32_t intr_index = 0;
    uint32_t value = 0;
    int rv = PHYMOD_E_NONE;
    *intr_status = 0;

    for (intr_index = 0; intr_index < SESTO_MAX_INTRS_SUPPORT; intr_index ++) {
        SESTO_IF_ERR_RETURN(
            _sesto_ext_intr_status_get(phy, (0x1 << intr_index), &value));
        if (value) {
            *intr_status |= (0x1 << intr_index);
        }
    }
ERR:
    return rv;
}
/**  Interrupt enable set
 *   This function is used to enable or disable particular interrupt
 *
 *    @param phy                Pointer to phymod phy access structure
 *
 *    @param enable             Enable or disable
 *        1 - enable
 *        0 - disable
 *
 *    @return PHYMOD_E_NONE     successful function execution
 */
int sesto_phy_intr_enable_set(const phymod_phy_access_t *phy, uint32_t enable)
{
    uint32_t intr_index = 0;
    int rv = PHYMOD_E_NONE;

    for (intr_index = 0; intr_index < SESTO_MAX_INTRS_SUPPORT; intr_index ++) {
        if (enable & (0x1 << intr_index)) {
            SESTO_IF_ERR_RETURN(_sesto_ext_intr_enable_set(phy, (0x1 << intr_index), 1));
        } else {
            SESTO_IF_ERR_RETURN(_sesto_ext_intr_enable_set(phy, (0x1 << intr_index), 0));
        }
    }
ERR:
    return rv;
}
/**  Interrupt enable get
 *   This function is used to check whether or not particular interrupt is enabled
 *
 *    @param phy                Pointer to phymod phy access structure
 *
 *    @param enable             Enable or disable
 *        1 - enable
 *        0 - disable
 *
 *    @return PHYMOD_E_NONE     successful function execution
 */
int sesto_phy_intr_enable_get(const phymod_phy_access_t *phy, uint32_t* enable)
{
    uint32_t intr_index = 0;
    uint32_t value = 0;
    int rv = PHYMOD_E_NONE;

    *enable = 0;
    for (intr_index = 0; intr_index < SESTO_MAX_INTRS_SUPPORT; intr_index ++) {
        SESTO_IF_ERR_RETURN(
            _sesto_ext_intr_enable_get(phy, (0x1 << intr_index), &value));
        if (value) {
            *enable |= (0x1 << intr_index);
        }
    }
ERR:
    return rv;
}
/**  Interrupt status clear
 *   This function is used for clearing interrupt status
 *
 *    @param phy                Pointer to phymod phy access structure
 *
 *    @return PHYMOD_E_NONE     successful function execution
 */
int sesto_phy_intr_status_clear(const phymod_phy_access_t *phy, uint32_t intr_clear)
{
    uint32_t intr_index = 0;
    int rv = PHYMOD_E_NONE;

    for (intr_index = 0; intr_index < SESTO_MAX_INTRS_SUPPORT; intr_index ++) {
        if (intr_clear & (0x1 << intr_index)) {
            SESTO_IF_ERR_RETURN(
                _sesto_ext_intr_status_clear(phy, (0x1 << intr_index)));
        }
    }
ERR:
    return rv;
}

/**  PHY status dump
 *   This function is used to display status dump of a core and for given lane
 *
 *    @param phy                Pointer to phymod phy access structure
 *
 *    @return PHYMOD_E_NONE     successful function execution
 */
int sesto_phy_status_dump(const phymod_phy_access_t* phy)
{
    return _sesto_phy_status_dump(phy);
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
int sesto_phy_fec_enable_set(const phymod_phy_access_t* phy, uint32_t enable)
{
    return _sesto_fec_enable_set(phy, enable);
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
int sesto_phy_fec_enable_get(const phymod_phy_access_t* phy, uint32_t* enable)
{
    return _sesto_fec_enable_get(phy, enable);
}

/**  FC/PCS checker enable set
 *   This function is used to enable or disable FC/PCS checker
 *
 *    @param phy                Pointer to phymod phy access structure
 *    @param fcpcs_chkr_mode    FC/PCS checker mode
 *    @param enable             Enable or disable
 *        1 -  enable
 *        0 -  disable
 *
 *    @return PHYMOD_E_NONE     successful function execution
 */
int sesto_phy_fc_pcs_chkr_enable_set(const phymod_phy_access_t* phy, uint32_t fcpcs_chkr_mode, uint32_t enable)
{
    return _sesto_pcs_link_monitor_enable_set(phy, enable);
}
/**  FC/PCS checker enable get
 *   This function is used to check whether or not FC/PCS checker enabled
 *
 *    @param phy                Pointer to phymod phy access structure
 *    @param fcpcs_chkr_mode    FC/PCS checker mode
 *    @param enable             Enable or disable
 *        1 -  enable
 *        0 -  disable
 */
int sesto_phy_fc_pcs_chkr_enable_get(const phymod_phy_access_t* phy, uint32_t fcpcs_chkr_mode, uint32_t *enable)
{
    return _sesto_pcs_link_monitor_enable_get(phy, enable);
}
/**  FC/PCS checker status get
 *   This function is used to lock, loss of lock, error count status of the FC/PCS checker
 *
 *    @param phy                Pointer to phymod phy access structure
 *    @param lock_status        Live lock status
 *        1 - Locked
 *        0 - Not locked
 *    @param lock_lost_lh       Loss of lock
 *        1 -  Loss of lock happened
 *        0 -  No Loss of lock happended
 *    @param error_count        Error count
 *
 *    @return PHYMOD_E_NONE     successful function execution
 */
int sesto_phy_fc_pcs_chkr_status_get(const phymod_phy_access_t* phy, uint32_t* lock_status, uint32_t* lock_lost_lh, uint32_t* error_count)
{
    return _sesto_get_pcs_link_status(phy, lock_status, lock_lost_lh, error_count);
}

/*Read data from I2C device attached to PHY*/
int sesto_phy_i2c_read(const phymod_phy_access_t* phy, uint32_t flags, uint32_t addr, uint32_t offset, uint32_t size, uint8_t* data)
{
    return _sesto_module_read(&phy->access, addr, offset, size, data);
}

/*Write data to I2C device attached to PHY*/
int sesto_phy_i2c_write(const phymod_phy_access_t* phy, uint32_t flags, uint32_t addr, uint32_t offset, uint32_t size, const uint8_t* data)
{
    return _sesto_module_write(&phy->access, addr, offset, size, data);
}

/*Set/Get the output/input value of a PHY GPIO pin*/
int sesto_phy_gpio_pin_value_set(const phymod_phy_access_t* phy, int pin_no, int value)
{
    return _sesto_gpio_pin_value_set(&phy->access, pin_no, value);
}
int sesto_phy_gpio_pin_value_get(const phymod_phy_access_t* phy, int pin_no, int* value)
{
    return _sesto_gpio_pin_value_get(&phy->access, pin_no, value);
}

/*Set/Get the configuration of a PHY GPIO pin*/
int sesto_phy_gpio_config_set(const phymod_phy_access_t* phy, int pin_no, phymod_gpio_mode_t gpio_mode)
{
    return _sesto_gpio_config_set(&phy->access, pin_no, gpio_mode);
}
int sesto_phy_gpio_config_get(const phymod_phy_access_t* phy, int pin_no, phymod_gpio_mode_t* gpio_mode)
{
    return _sesto_gpio_config_get(&phy->access, pin_no, gpio_mode);
}
int sesto_phy_short_chn_mode_enable_set(const phymod_phy_access_t* phy, uint32_t enable)
{
    return _sesto_phy_short_channel_mode_set(phy, enable);
}
int sesto_phy_short_chn_mode_enable_get(const phymod_phy_access_t* phy, uint32_t *enable, uint32_t *status)
{
    return _sesto_phy_short_channel_mode_get(phy, enable, status);
}
int sesto_phy_multi_get(const phymod_phy_access_t* phy, phymod_multi_data_t* multi_data)
{
    int rv=PHYMOD_E_NONE;
       SESTO_IF_ERR_RETURN
          (_sesto_module_read(&phy->access, multi_data->dev_addr, multi_data->offset, multi_data->max_size, multi_data->data));
          if(multi_data->actual_size){
              (*(multi_data->actual_size)) = multi_data->max_size;
          }
   ERR:
      return rv;
}

int sesto_phy_eye_margin_est_get(const phymod_phy_access_t* phy, phymod_eye_margin_mode_t eye_margin_mode, uint32_t* value)
{
    int hz_l, hz_r, vt_u, vt_d;
    PHYMOD_IF_ERR_RETURN
       (_sesto_phy_eye_margin_est_get(phy,&hz_l, &hz_r, &vt_u, &vt_d) );
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
  
int sesto_phy_op_mode_get(const phymod_phy_access_t* phy, phymod_operation_mode_t* op_mode)
{
    return PHYMOD_E_UNAVAIL;
}

