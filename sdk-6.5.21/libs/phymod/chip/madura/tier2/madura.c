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
#include <phymod/phymod_reg.h>
#include <phymod/phymod_dispatch.h>

#ifdef PHYMOD_MADURA_SUPPORT


#include <phymod/chip/madura.h>
#include "../tier1/madura_cfg_seq.h"
#include "../tier1/bcmi_madura_defs.h"

/* Microcontroller Firmware */
extern unsigned char madura_falcon_ucode[];
extern uint32_t madura_falcon_ucode_len;

/**   Core identify 
 *    This function reads PMD Identifiers, Set is identified if
 *    madura identified. 
 *
 *    @param core               Pointer to phymod core access structure 
 *    @param core_id            Core ID which is supplied by interface layer 
 *    @param is_identified      Flag variable to return device 
 *                              identification status 
 *
 *    @return PHYMOD_E_NONE     successful function execution 
 */
int madura_core_identify(const phymod_core_access_t* core, uint32_t core_id, uint32_t* is_identified)
{
	const phymod_access_t *pm_acc = &core->access;
	uint32_t chip_id;
	uint32_t rev_id;

	PMD_IDENTIFIER_0r_t id0;
	PMD_IDENTIFIER_1r_t id1;

	PHYMOD_MEMSET(&id0, 0, sizeof(PMD_IDENTIFIER_0r_t));
	PHYMOD_MEMSET(&id1, 0, sizeof(PMD_IDENTIFIER_1r_t));
	*is_identified = 0;

	if (core_id == 0) {
		PHYMOD_IF_ERR_RETURN (
				READ_PMD_IDENTIFIER_1r(pm_acc, &id1));
		PHYMOD_IF_ERR_RETURN (
				READ_PMD_IDENTIFIER_0r(pm_acc, &id0));
	} else {
		id0.v[0] = (core_id >> 16) & 0xffff;
		id1.v[0] = core_id & 0xffff;
		/* id1=core_id & 0xffff; */
	}
	/* 
     *  Add support for incorrect OTP with improper IEEE IDs 
     *  if (id0.v[0] == MADURA_PMD_ID0 && id1.v[0] == MADURA_PMD_ID1)
     */ {
		/* PHY IDs match - now check model */
		PHYMOD_IF_ERR_RETURN(madura_get_chipid(pm_acc, &chip_id, &rev_id));
		if (chip_id == MADURA_CHIP_ID_82864) {
			*is_identified = 1;
		}
	}
	
	return PHYMOD_E_NONE;
}


/**   Core information 
 *    This function gives the core version
 *
 *    @param core               Pointer to phymod core access structure 
 *    @param info               Pointer to core version 
 *
 *    @return PHYMOD_E_NONE     successful function execution 
 */
int madura_core_info_get(const phymod_core_access_t* core, phymod_core_info_t* info)
{
    uint32_t chip_id;
    uint32_t rev_id;
    PMD_IDENTIFIER_0r_t id0;
    PMD_IDENTIFIER_1r_t id1;

    PHYMOD_IF_ERR_RETURN (
               madura_get_chipid(&core->access, &chip_id, &rev_id));
    info->serdes_id = chip_id;
    info->core_version = phymodCoreVersionMadura;
    PHYMOD_STRNCPY(info->name, "Madura", PHYMOD_STRLEN("Madura")+1);

    PHYMOD_IF_ERR_RETURN (
           READ_PMD_IDENTIFIER_1r(&core->access, &id1));
    info->phy_id1 = id1.v[0];
    PHYMOD_IF_ERR_RETURN (
           READ_PMD_IDENTIFIER_0r(&core->access, &id0));
    info->phy_id0 = id0.v[0];
    
    return PHYMOD_E_NONE;
    
}

/**   Get Firmware info
 *    This function get the firmware information such as master firmware version and master checksum
 *
 *    @param core               Pointer to phymod core access structure 
 *    @param fw_info            Represent firmware version and checksum.
 *    
 *    @return PHYMOD_E_NONE     successful function execution 
 */
int madura_core_firmware_info_get(const phymod_core_access_t* core, phymod_core_firmware_info_t* fw_info)
{
        
    FIRMWARE_VERSIONr_t  firmware_version;
    MST_RUNNING_CHKSUMr_t  mst_running_chksum;

    /* Read the firmware version */
    PHYMOD_IF_ERR_RETURN(
        READ_FIRMWARE_VERSIONr(&core->access, &firmware_version));

    fw_info->fw_version = FIRMWARE_VERSIONr_FIRMWARE_VERSION_VALf_GET(firmware_version);

    PHYMOD_IF_ERR_RETURN(
        READ_MST_RUNNING_CHKSUMr(&core->access, &mst_running_chksum));

    fw_info->fw_crc = MST_RUNNING_CHKSUMr_MST_RUNNING_CHKSUM_VALf_GET(mst_running_chksum);

    return PHYMOD_E_NONE;    
    
}

/**   Core Reset
 *    This function reset madura core, it support hard and soft reset
 *
 *    @param core               Pointer to phymod core access structure
 *    @param reset_mode         Represent hard/soft reset to perform
 *    @param direction          Represet direction of reset. Sesto ignore this parameter
 *
 *    @return PHYMOD_E_NONE     successful function execution
 */
int madura_core_reset_set(const phymod_core_access_t* core, phymod_reset_mode_t reset_mode, phymod_reset_direction_t direction)
{
    return _madura_core_reset_set(&core->access, reset_mode, direction);

}

int madura_core_reset_get(const phymod_core_access_t* core, phymod_reset_mode_t reset_mode, phymod_reset_direction_t* direction)
{


    /* Place your code here */


    return PHYMOD_E_UNAVAIL;

}

int madura_phy_firmware_lane_config_set(const phymod_phy_access_t* phy, phymod_firmware_lane_config_t fw_config)
{
    return _madura_firmware_lane_config_set(&phy->access, fw_config);
}


int madura_phy_firmware_lane_config_get(const phymod_phy_access_t* phy, phymod_firmware_lane_config_t* fw_config)
{
    return _madura_firmware_lane_config_get(&phy->access, fw_config);
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

int madura_core_pll_sequencer_restart(const phymod_core_access_t* core, uint32_t flags, phymod_sequencer_operation_t operation)
{
    return _madura_pll_sequencer_restart(core, operation);
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
int madura_phy_power_set(const phymod_phy_access_t* phy, const phymod_phy_power_t* power)
{
    PHYMOD_IF_ERR_RETURN(
      _madura_tx_power_set(&phy->access, power->tx));
    PHYMOD_IF_ERR_RETURN(
      _madura_rx_power_set(&phy->access, power->rx));
    return PHYMOD_E_NONE;

}

/**  Tx set
 *   This function is used to set transmitter analog parameters
 *
 *    @param phy                Pointer to phymod phy access structure
 *    @param tx                 Pointer to tx param structure
 *
 *    @return PHYMOD_E_NONE     successful function execution
 */

int madura_phy_tx_set(const phymod_phy_access_t* phy, const phymod_tx_t* tx)
{
        
    return _madura_tx_set(&phy->access, tx); 
    
}

/**  Tx get
 *   This function is used to get transmitter analog parameters
 *
 *    @param phy                Pointer to phymod phy access structure
 *    @param tx                 Pointer to tx param structure
 *
 *    @return PHYMOD_E_NONE     successful function execution
 */
int madura_phy_tx_get(const phymod_phy_access_t* phy, phymod_tx_t* tx)
{
        
   return _madura_tx_get(&phy->access, tx);
 
}

/**  Rx set
 *   This function is used to set receiver analog parameters
 *
 *    @param phy                Pointer to phymod phy access structure
 *    @param rx                 Pointer to rx param structure
 *
 *    @return PHYMOD_E_NONE     successful function execution
 */

int madura_phy_rx_set(const phymod_phy_access_t* phy, const phymod_rx_t* rx)
{

    return _madura_rx_set(&phy->access, rx); 
    
}

/**  Rx get
 *   This function is used to get receiver analog parameters
 *
 *    @param phy                Pointer to phymod phy access structure
 *    @param rx                 Pointer to rx param structure
 *
 *    @return PHYMOD_E_NONE     successful function execution
 */
int madura_phy_rx_get(const phymod_phy_access_t* phy, phymod_rx_t* rx)
{
        
    return _madura_rx_get(&phy->access, rx); 
    
}

/**  PHY Rx adaptation resume
 *   This function is used to perform PHY reset
 *
 *    @param phy                Pointer to phymod phy access structure
 *
 *    @return PHYMOD_E_NONE     successful function execution
 */
int madura_phy_rx_adaptation_resume(const phymod_phy_access_t* phy)
{
    return _madura_rx_adaptation_resume(&phy->access);
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


int madura_phy_power_get(const phymod_phy_access_t* phy, phymod_phy_power_t* power)
{
    return _madura_tx_rx_power_get(&phy->access, power);

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

int madura_phy_tx_lane_control_set(const phymod_phy_access_t* phy, phymod_phy_tx_lane_control_t tx_control)
{
    return _madura_tx_lane_control_set(&phy->access, tx_control);
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

int madura_phy_tx_lane_control_get(const phymod_phy_access_t* phy, phymod_phy_tx_lane_control_t* tx_control)
{
    return _madura_tx_lane_control_get(&phy->access, tx_control);
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

int madura_phy_rx_lane_control_set(const phymod_phy_access_t* phy, phymod_phy_rx_lane_control_t rx_control)
{
    return _madura_rx_lane_control_set(&phy->access, rx_control);
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

int madura_phy_rx_lane_control_get(const phymod_phy_access_t* phy, phymod_phy_rx_lane_control_t* rx_control)
{
    return _madura_rx_lane_control_get(&phy->access, rx_control);
}

/**  PHY polarity set
 *   This function is used to set the lane polarity
 *
 *   @param phy          Pointer to phymod phy access structure
 *   @param polarity     Pointer to phymod_polarity_t for rx and tx
 *
 *   @return PHYMOD_E_NONE     successful function execution
 */
int madura_phy_polarity_set(const phymod_phy_access_t* phy, const phymod_polarity_t* polarity)
{
    return _madura_tx_rx_polarity_set(&phy->access, polarity->tx_polarity, polarity->rx_polarity);
}


/**  PHY polarity get
 *   This function is used to get the lane polarity
 *
 *   @param phy          Pointer to phymod phy access structure
 *   @param polarity     Pointer to phymod_polarity_t for rx and tx
 *
 *   @return PHYMOD_E_NONE     successful function execution
 */
int madura_phy_polarity_get(const phymod_phy_access_t* phy, phymod_polarity_t* polarity)
{
    return _madura_tx_rx_polarity_get(&phy->access, &polarity->tx_polarity, &polarity->rx_polarity);
}

int madura_phy_interface_config_set(const phymod_phy_access_t* phy, uint32_t flags, const phymod_phy_inf_config_t* config)
{
    uint16_t retry_cnt = 5;
    FIRMWARE_ENr_t fwe;

    PHYMOD_IF_ERR_RETURN (
            _madura_phy_interface_config_set(phy, flags, config));

    /* Enable FW After configuring mode */
    PHYMOD_IF_ERR_RETURN(
        _madura_fw_enable(&phy->access, MADURA_ENABLE));

    do {
        PHYMOD_USLEEP(100);

        PHYMOD_IF_ERR_RETURN(
            READ_FIRMWARE_ENr(&phy->access, &fwe));
    } while ((FIRMWARE_ENr_FW_ENABLE_VALf_GET(fwe) != 0) && (--retry_cnt ));

    if (retry_cnt == 0) {
        PHYMOD_DEBUG_VERBOSE(("WARN:: FW Enable not cleared\n"));
    }
    PHYMOD_USLEEP(500000); /* sleep added for standalone prbs issue */
    return PHYMOD_E_NONE;

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
int madura_phy_interface_config_get(const phymod_phy_access_t* phy, uint32_t flags, phymod_ref_clk_t ref_clock, phymod_phy_inf_config_t* config)
{
        
    MADURA_DEVICE_AUX_MODE_T  *aux_mode;

    config->device_aux_modes = PHYMOD_MALLOC(sizeof(MADURA_DEVICE_AUX_MODE_T), "madura_device_aux_mode");
    aux_mode = (MADURA_DEVICE_AUX_MODE_T*) config->device_aux_modes;
    if (aux_mode == NULL) {
        PHYMOD_DEBUG_VERBOSE(("AUX MODE MEM NOT ALLOC\n"));
        return PHYMOD_E_PARAM;
    }
    PHYMOD_MEMSET(aux_mode, 0, sizeof(MADURA_DEVICE_AUX_MODE_T));

    MADURA_IF_ERR_RETURN_FREE(config->device_aux_modes, (
            _madura_phy_interface_config_get(&phy->access, flags, config)));

    PHYMOD_FREE(config->device_aux_modes);
    return PHYMOD_E_NONE;
    
}

/**   Madura core initialization
 *    This function initialize the madura core by downlaoding the firmware.
 *
 *    @param core               Pointer to phymod core access structure 
 *    @param init_config        Init configuration specified by user 
 *    @param core_status        PMD status read from PHY chip 
 *
 *    @return PHYMOD_E_NONE     successful function execution 
 */


int madura_core_init(const phymod_core_access_t* core, const phymod_core_init_config_t* init_config, const phymod_core_status_t* core_status)
{
    uint32_t new_firmware_version=0;
    phymod_core_firmware_info_t fw_info;

    PHYMOD_MEMSET(&fw_info, 0, sizeof(phymod_core_firmware_info_t));
    PHYMOD_IF_ERR_RETURN(
		    madura_core_firmware_info_get(core, &fw_info));
    new_firmware_version = madura_falcon_ucode[madura_falcon_ucode_len-4]<<8 | madura_falcon_ucode[madura_falcon_ucode_len-3];

    if(PHYMOD_CORE_INIT_F_FW_AUTO_DOWNLOAD_GET(init_config)){/* for auto download*/
       if( new_firmware_version != fw_info.fw_version){
        PHYMOD_IF_ERR_RETURN(
	    _madura_core_init(core, init_config));
       }
    }else{ /*rest of the cases continue like before*/
        PHYMOD_IF_ERR_RETURN(
	    _madura_core_init(core, init_config));
    }
    
    return PHYMOD_E_NONE;
    
}


int madura_phy_init(const phymod_phy_access_t* phy, const phymod_phy_init_config_t* init_config)
{
    PHYMOD_IF_ERR_RETURN(
        madura_phy_interface_config_set(phy, 0, &init_config->interface));
    /*enable/disable repeater/retimer*/ 
    if (init_config->op_mode == phymodOperationModeRetimer) {
        PHYMOD_IF_ERR_RETURN
            (_madura_core_rptr_rtmr_mode_set(&phy->access, phymodOperationModeRetimer));
    } else {
         /* By default HW set it as repeater*/ 
            (_madura_core_rptr_rtmr_mode_set(&phy->access, phymodOperationModeRepeater));
    }
   return PHYMOD_E_NONE;
    
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
int madura_phy_loopback_set(const phymod_phy_access_t* phy, phymod_loopback_mode_t loopback, uint32_t enable)
{
    uint32_t ena_dis = 0; 
    _madura_loopback_get(&phy->access, loopback, &ena_dis);
    if (ena_dis || enable) {
	    return _madura_loopback_set(&phy->access, loopback, enable);
    }
    return PHYMOD_E_NONE;
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
int madura_phy_loopback_get(const phymod_phy_access_t* phy, phymod_loopback_mode_t loopback, uint32_t* enable)
{
    return _madura_loopback_get(&phy->access, loopback, enable);
}


/**   Get PHY RX PMD link status 
 *    This function retrieves RX PMD lock status of madura 
 *
 *    @param phy                Pointer to phymod phy access structure 
 *    @param rx_pmd_locked      Rx PMD Link status retrieved from Madura
 *
 *    @return PHYMOD_E_NONE     successful function execution 
 */

int madura_phy_rx_pmd_locked_get(const phymod_phy_access_t* phy, uint32_t* rx_pmd_locked)
{
        
    return (_madura_rx_pmd_lock_get(&phy->access, rx_pmd_locked));
    
}


/**   Get PCS link status
 *    This function retrieves PCS link status of madura when PCS Monitor is enabled,
 *    This function return PMD lock when PCS monitor is disabled.
 *
 *    @param phy                Pointer to phymod phy access structure 
 *    @param link_status        Retrives PCS Link of Madura
 *
 *    @return PHYMOD_E_NONE     successful function execution 
 */

int madura_phy_link_status_get(const phymod_phy_access_t* phy, uint32_t* link_status)
{
    return  _madura_get_pcs_link_status(&phy->access, link_status);

}


int madura_phy_status_dump(const phymod_phy_access_t* phy)
{
   return   _madura_phy_status_dump(&phy->access);  
}


/**   Read Register
 *    This function read user specified register based on the specified Device type 
 *
 *    @param phy                Pointer to phymod phy access structure.
 *    @param reg_addr           Madura register address
 *    @param val                Output parameter, represents the value of address specified
 *
 *    @return PHYMOD_E_NONE     successful function execution 
 */
int madura_phy_reg_read(const phymod_phy_access_t* phy, uint32_t reg_addr, uint32_t* val)
{
    uint16_t dev_add = 0;
    dev_add = (phy->access.devad) ? phy->access.devad : MADURA_DEV_PMA_PMD;

    if (dev_add == MADURA_DEV_PMA_PMD) {
        READ_MADURA_PMA_PMD_REG(&phy->access, reg_addr, *val);
    } else {
        READ_MADURA_AN_REG(&phy->access, reg_addr, *val);
    }
        
    return PHYMOD_E_NONE;
    
}


/**   Write Register
 *    This function write user specified value to the specified address 
 *    based on the specified Device type 
 *
 *    @param phy                Pointer to phymod phy access structure.
 *    @param reg_addr           Madura register address
 *    @param val                Represents the value to be written
 *
 *    @return PHYMOD_E_NONE     successful function execution 
 */
int madura_phy_reg_write(const phymod_phy_access_t* phy, uint32_t reg_addr, uint32_t val)
{
        
    uint16_t dev_add = 0;
    dev_add = (phy->access.devad) ? phy->access.devad : MADURA_DEV_PMA_PMD;
    PHYMOD_DEBUG_VERBOSE(("WRITE reg:%x data:%x\n", reg_addr, val));
    if (dev_add == MADURA_DEV_PMA_PMD) {
        WRITE_MADURA_PMA_PMD_REG(&phy->access, reg_addr, val);
    } else {
        WRITE_MADURA_AN_REG(&phy->access, reg_addr, val);
    }
        
    return PHYMOD_E_NONE;
    
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

int madura_phy_reset_set(const phymod_phy_access_t* phy, const phymod_phy_reset_t* reset)
{
    return _madura_phy_reset_set(&phy->access, reset);
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
int madura_phy_reset_get(const phymod_phy_access_t* phy, phymod_phy_reset_t* reset)
{
    return _madura_phy_reset_get(&phy->access, reset);
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
int madura_phy_fec_enable_set(const phymod_phy_access_t* phy, uint32_t enable)
{
    return _madura_fec_enable_set(&phy->access, enable);
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
int madura_phy_fec_enable_get(const phymod_phy_access_t* phy, uint32_t* enable)
{
    return _madura_fec_enable_get(&phy->access, enable);
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

int madura_phy_cl72_set(const phymod_phy_access_t* phy, uint32_t cl72_en)
{
    return _madura_force_tx_training_set(&phy->access, cl72_en);
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

int madura_phy_cl72_get(const phymod_phy_access_t* phy, uint32_t* cl72_en)
{
    return _madura_force_tx_training_get(&phy->access, cl72_en);
}


/**  Get CL72 status
 *   This function is used to get the status of Tx training
 *
 *    @param phy                Pointer to phymod phy access structure
 *    @param status             CL72 Training status
 *
 *    @return PHYMOD_E_NONE     successful function execution
 */

int madura_phy_cl72_status_get(const phymod_phy_access_t* phy, phymod_cl72_status_t* status)
{
    return _madura_force_tx_training_status_get(&phy->access, status);
}

/**  PHY CL73 ability Set
 *   This function is used to set the CL73 ability of a lane
 *
 *    @param phy                Pointer to phymod phy access structure
 *    @param an_ability         Pointer to the ability of cl73
 *
 *    @return PHYMOD_E_NONE     successful function execution
 */
int madura_phy_autoneg_ability_set(const phymod_phy_access_t* phy, const phymod_autoneg_ability_t* an_ability)
{
    madura_an_ability_t value;
    int rv = PHYMOD_E_NONE;

    PHYMOD_MEMSET(&value, 0, sizeof(value));
    value.cl73_adv.an_fec = an_ability->an_fec;
    value.an_master_lane = an_ability->an_master_lane;
    /*check if sgmii  or not */
    if (PHYMOD_AN_CAP_SGMII_GET(an_ability)){
        return rv;
    }

    /*next check pause */
    if (PHYMOD_AN_CAP_SYMM_PAUSE_GET(an_ability)) {
        value.cl73_adv.an_pause = MADURA_SYMM_PAUSE;
    } else if (PHYMOD_AN_CAP_ASYM_PAUSE_GET(an_ability)) {
        value.cl73_adv.an_pause = MADURA_ASYM_PAUSE;
    } else {
        value.cl73_adv.an_pause = MADURA_NO_PAUSE;
    }

    /*check cl73 and cl73 bam ability */
    if (PHYMOD_AN_CAP_40G_KR4_GET(an_ability->an_cap)) {
        value.cl73_adv.an_base_speed = MADURA_CL73_40GBASE_KR4;
    } else if (PHYMOD_AN_CAP_40G_CR4_GET(an_ability->an_cap)) {
        value.cl73_adv.an_base_speed = MADURA_CL73_40GBASE_CR4;
    } else if (PHYMOD_AN_CAP_100G_CR4_GET(an_ability->an_cap)) {
        value.cl73_adv.an_base_speed = MADURA_CL73_100GBASE_CR4;
    } else if (PHYMOD_AN_CAP_100G_KR4_GET(an_ability->an_cap)) {
        value.cl73_adv.an_base_speed = MADURA_CL73_100GBASE_KR4;
    } else if (an_ability->an_cap == 0) {
        return rv;
    } else {
        return PHYMOD_E_PARAM;
    }

    PHYMOD_IF_ERR_RETURN
        (_madura_autoneg_ability_set(&phy->access, &value));

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

int madura_phy_autoneg_ability_get(const phymod_phy_access_t* phy, phymod_autoneg_ability_t* an_ability_get_type)
{
    madura_an_ability_t value;

    PHYMOD_IF_ERR_RETURN(
        _madura_autoneg_ability_get(&phy->access, &value));

    an_ability_get_type->an_fec = value.cl73_adv.an_fec;
    an_ability_get_type->an_master_lane = value.an_master_lane ;
    switch (value.cl73_adv.an_pause) {
        case MADURA_ASYM_PAUSE:
            PHYMOD_AN_CAP_ASYM_PAUSE_SET(an_ability_get_type);
        break;
        case MADURA_SYMM_PAUSE:
            PHYMOD_AN_CAP_SYMM_PAUSE_SET(an_ability_get_type);
        break;
        default:
            break;
    }

    /*first check cl73 ability*/
    switch (value.cl73_adv.an_base_speed) {
        case MADURA_CL73_40GBASE_CR4:
            PHYMOD_AN_CAP_40G_CR4_SET(an_ability_get_type->an_cap);
        break;
        case MADURA_CL73_40GBASE_KR4:
            PHYMOD_AN_CAP_40G_KR4_SET(an_ability_get_type->an_cap);
        break;
        case MADURA_CL73_100GBASE_CR4:
            PHYMOD_AN_CAP_100G_CR4_SET(an_ability_get_type->an_cap);
        break;
        case MADURA_CL73_100GBASE_KR4:
            PHYMOD_AN_CAP_100G_KR4_SET(an_ability_get_type->an_cap);
        break;
        default:
            break;
    }

    return PHYMOD_E_NONE;
}

/**  PHY CL73 remote ability get
 *   This function is used to get the CL73 remote ability of a lane
 *
 *    @param phy                Pointer to phymod phy access structure
 *    @param an_ability         Pointer to the ability of cl73
 *
 *    @return PHYMOD_E_NONE     successful function execution
 */
int madura_phy_autoneg_remote_ability_get(const phymod_phy_access_t* phy, phymod_autoneg_ability_t* an_ability_get)
{
    int rv = PHYMOD_E_NONE;

    PHYMOD_IF_ERR_RETURN(
        _madura_autoneg_remote_ability_get(&phy->access, an_ability_get));

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
int madura_phy_autoneg_set(const phymod_phy_access_t* phy, const phymod_autoneg_control_t* an)
{
    return (_madura_autoneg_set(phy, an));
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
int madura_phy_autoneg_get(const phymod_phy_access_t* phy, phymod_autoneg_control_t* an, uint32_t* an_done)
{
    return (_madura_autoneg_get(&phy->access, an, an_done));
}


int madura_phy_autoneg_status_get(const phymod_phy_access_t* phy, phymod_autoneg_status_t* status)
{
    
    /* Place your code here */
    return PHYMOD_E_NONE;
    
}
/**  Interrupt enable set 
 *   This function is used to enable or disable particular interrupt
 *
 *    @param phy                Pointer to phymod phy access structure
 *    @param enable             Pending status mask
 *
 *    @return PHYMOD_E_NONE     successful function execution
 */
int madura_phy_intr_enable_set(const phymod_phy_access_t* phy,  uint32_t enable)
{
    uint32_t intr_index = 0;

    for (intr_index = 0; intr_index < MADURA_MAX_INTRS_SUPPORT; intr_index ++) {
        if (enable & (0x1 << intr_index)) {
	    PHYMOD_IF_ERR_RETURN(_madura_ext_intr_enable_set(phy, (0x1 << intr_index), 1));
	}else{
	    PHYMOD_IF_ERR_RETURN(_madura_ext_intr_enable_set(phy, (0x1 << intr_index), 0));
 	}
    }
    return PHYMOD_E_NONE;
}

/**  Interrupt enable get 
 *   This function is used to check whether or not particular interrupt is enabled
 *
 *    @param phy                Pointer to phymod phy access structure
 *    @param enable             Enable mask
 *
 *    @return PHYMOD_E_NONE     successful function execution
 */
int madura_phy_intr_enable_get(const phymod_phy_access_t* phy,  uint32_t* enable)
{
    uint32_t intr_index = 0;
    uint32_t value = 0;

    *enable = 0;
    for (intr_index = 0; intr_index < MADURA_MAX_INTRS_SUPPORT; intr_index ++) {
        PHYMOD_IF_ERR_RETURN(
			_madura_ext_intr_enable_get(phy, (0x1 << intr_index), &value));
        if (value) {
            *enable |= (0x1 << intr_index);
        }
    }
    return PHYMOD_E_NONE;

    
}

/**  Interrupt status get 
 *   This function is used to check whether or not particualr interrupt is pending
 *
 *    @param phy                Pointer to phymod phy access structure
 *    @param status             Pending status mask
 *
 *    @return PHYMOD_E_NONE     successful function execution
 */
int madura_phy_intr_status_get(const phymod_phy_access_t* phy, uint32_t* intr_status)
{

    uint32_t intr_index = 0;
    uint32_t value = 0;

    for (intr_index = 0; intr_index < MADURA_MAX_INTRS_SUPPORT; intr_index ++) {
        PHYMOD_IF_ERR_RETURN(
			_madura_ext_intr_status_get(phy,(0x1 << intr_index), &value));
        if (value) {
            *intr_status |= (0x1 << intr_index);
        }
    } 
    return PHYMOD_E_NONE; 
}

/**  Interrupt status clear 
 *   This function is used for clearing interrupt status
 *
 *    @param phy                Pointer to phymod phy access structure
 *    @param uint32             clear mask
 *
 *    @return PHYMOD_E_NONE     successful function execution
 */
int madura_phy_intr_status_clear(const phymod_phy_access_t* phy, uint32_t intr_clear)
{

    uint32_t intr_index = 0;

    for (intr_index = 0; intr_index < MADURA_MAX_INTRS_SUPPORT; intr_index ++) {
        if (intr_clear & (0x1 << intr_index)) {
            PHYMOD_IF_ERR_RETURN(
			    _madura_ext_intr_status_clear(phy, (0x1 << intr_index)));
        }
    }
    return PHYMOD_E_NONE;
}


int madura_phy_i2c_read(const phymod_phy_access_t* phy, uint32_t flags, uint32_t addr, uint32_t offset, uint32_t size, uint8_t* data)
{
    return _madura_module_read(&phy->access, addr, offset, size, data);
}


int madura_phy_i2c_write(const phymod_phy_access_t* phy, uint32_t flags, uint32_t addr, uint32_t offset, uint32_t size, const uint8_t* data)
{
   return _madura_module_write(&phy->access, addr, offset, size, data);
}


int madura_phy_gpio_config_set(const phymod_phy_access_t* phy, int pin_no, phymod_gpio_mode_t gpio_mode)
{
   return _madura_gpio_config_set(&phy->access, pin_no, gpio_mode); 
}

int madura_phy_gpio_config_get(const phymod_phy_access_t* phy, int pin_no, phymod_gpio_mode_t* gpio_mode)
{
   return _madura_gpio_config_get(&phy->access, pin_no, gpio_mode); 
}


int madura_phy_gpio_pin_value_set(const phymod_phy_access_t* phy, int pin_no, int value)
{
   return _madura_gpio_pin_value_set(&phy->access, pin_no, value);
    
}

int madura_phy_gpio_pin_value_get(const phymod_phy_access_t* phy, int pin_no, int* value)
{
   return _madura_gpio_pin_value_get(&phy->access, pin_no, value);
}

int madura_phy_op_mode_get(const phymod_phy_access_t* phy, phymod_operation_mode_t* op_mode)
{
    return PHYMOD_E_UNAVAIL;
}

#endif /* PHYMOD_MADURA_SUPPORT */
