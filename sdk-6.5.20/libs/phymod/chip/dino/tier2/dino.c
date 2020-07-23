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
#include <phymod/phymod_diagnostics.h>
#include <phymod/phymod_diagnostics_dispatch.h>
#include <phymod/chip/dino.h>

#include "../tier1/bcmi_dino_defs.h"
#include "../tier1/dino_cfg_seq.h"
#include "../tier1/dino_diag_seq.h"
#include "../tier1/dino_serdes/common/srds_api_enum.h"

/**   Core identify 
 *    This function reads PMD Identifiers, Set is identified if
 *    dino identified. 
 *
 *    @param core               Pointer to phymod core access structure 
 *    @param core_id            Core ID which is supplied by interface layer 
 *    @param is_identified      Flag variable to return device 
 *                              identification status 
 *
 *    @return PHYMOD_E_NONE     successful function execution 
 */
int dino_core_identify(const phymod_core_access_t* core, uint32_t core_id, uint32_t* is_identified)
{
	const phymod_access_t *pm_acc = &core->access;
	uint32_t chip_id = 0;
	uint32_t rev_id = 0;
	*is_identified = 0;

	PHYMOD_IF_ERR_RETURN(dino_get_chipid(pm_acc, &chip_id, &rev_id));
	if ((chip_id == DINO_CHIP_ID_82332) || 
        (chip_id == DINO_CHIP_ID_82793) ||
        (chip_id == DINO_CHIP_ID_82795)) {
	    *is_identified = 0x80000001;
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
int dino_core_info_get(const phymod_core_access_t* core, phymod_core_info_t* info)
{
    uint32_t chip_id = 0;
    uint32_t rev_id = 0;
	PMD_IDENTIFIERr_t id0;
	IEEE_PMA_PMD_BLK0_PMD_IDENTIFIERr_t id1;
	PHYMOD_MEMSET(&id0, 0, sizeof(PMD_IDENTIFIERr_t));
	PHYMOD_MEMSET(&id1, 0, sizeof(IEEE_PMA_PMD_BLK0_PMD_IDENTIFIERr_t));

    PHYMOD_IF_ERR_RETURN (
               dino_get_chipid(&core->access, &chip_id, &rev_id));
    info->serdes_id = chip_id;
    info->core_version = phymodCoreVersionDino;

	PHYMOD_IF_ERR_RETURN(
	    READ_IEEE_PMA_PMD_BLK0_PMD_IDENTIFIERr(&core->access, &id1));
    info->phy_id1 = id1.v[0];
	PHYMOD_IF_ERR_RETURN(
	    READ_PMD_IDENTIFIERr(&core->access, &id0));
    info->phy_id0 = id0.v[0];
    PHYMOD_STRCPY(info->name, "Dino");

    return PHYMOD_E_NONE;
}


/**   Core Reset 
 *    This function reset dino core, it support hard and soft reset
 *
 *    @param core               Pointer to phymod core access structure 
 *    @param reset_mode         Represent hard/soft reset to perform
 *    @param direction          Represet direction of reset. dino ignore this parameter
 *
 *    @return PHYMOD_E_NONE     successful function execution 
 */
int dino_core_reset_set(const phymod_core_access_t* core, phymod_reset_mode_t reset_mode, phymod_reset_direction_t direction)
{
    return _dino_core_reset_set(&core->access, reset_mode, direction);   
    
}

int dino_core_reset_get(const phymod_core_access_t* core, phymod_reset_mode_t reset_mode, phymod_reset_direction_t* direction)
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
int dino_core_firmware_info_get(const phymod_core_access_t* core, phymod_core_firmware_info_t* fw_info)
{
    FIRMWARE_VERSIONr_t  firmware_version;
    DWNLD_15r_t dwnld_15;
    DWNLD_16r_t dwnld_16;
    DWNLD_17r_t dwnld_17;

    /* Read the firmware version */
    PHYMOD_IF_ERR_RETURN(
        READ_FIRMWARE_VERSIONr(&core->access, &firmware_version));

    fw_info->fw_version = FIRMWARE_VERSIONr_FIRMWARE_VERSION_VALf_GET(firmware_version);

    PHYMOD_IF_ERR_RETURN(
        READ_DWNLD_15r(&core->access, &dwnld_15));
    PHYMOD_IF_ERR_RETURN(
        READ_DWNLD_16r(&core->access, &dwnld_16));
    PHYMOD_IF_ERR_RETURN(
        READ_DWNLD_17r(&core->access, &dwnld_17));

    if ((dwnld_15.v[0] == 0x600D) && (dwnld_16.v[0] == 0x600D) && (dwnld_17.v[0] == 0x600D)) {
        fw_info->fw_crc = dwnld_15.v[0];
    }

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
 *                                   -> pass_thru :  Set it to 0 for Gearbox and 1 for Passthrough
 *    
 *    @return PHYMOD_E_NONE     successful function execution 
 */

int dino_phy_interface_config_set(const phymod_phy_access_t* phy, uint32_t flags, const phymod_phy_inf_config_t* config)
{
    return (_dino_phy_interface_config_set(phy, flags, config));
}

/**   Get interface config
 *    This function gets interface configuration (interface, speed, frequency of
 *    operation, PT mode). 
 *
 *    NOTE: Default system side interface type will be IEEE mode and speed is compatible with lane side configured speed.
 *     Ex: User configured 40G speed with interface type KR4 on lane side then system side speed is 40G and interface type is IEEE mode.
 *
 *    @param phy                Pointer to phymod phy access structure 
 *    @param flags              Reserved for future use
 *    @param ref_clock          Reference Clock        
 *    @param config             Gets Interface config such as,
 *                                - Interface Type
 *                                - Speed 
 *                                - Frequency.
 *                                - device_aux_modes : a structure that have auxilary mode details
 *                                   -> pass_thru :  0 for Gearbox and 1 for Passthrough
 *    
 *    @return PHYMOD_E_NONE     successful function execution 
 */

int dino_phy_interface_config_get(const phymod_phy_access_t* phy, uint32_t flags, uint32_t ref_clock, phymod_phy_inf_config_t* config)
{
    int rv = PHYMOD_E_NONE;
    phymod_phy_access_t phy_copy;
    PHYMOD_MEMSET(&phy_copy, 0, sizeof(phymod_phy_access_t));
    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phymod_phy_access_t));
    rv = _dino_phy_interface_config_get(&phy_copy, flags, config);
    phy = &phy_copy;
    return rv;
}

/**   dino core initialization
 *    This function initialize the dino core by downlaoding the firmware.
 *
 *    @param core               Pointer to phymod core access structure 
 *    @param init_config        Init configuration specified by user 
 *    @param core_status        PMD status read from PHY chip 
 *
 *    @return PHYMOD_E_NONE     successful function execution 
 */

int dino_core_init(const phymod_core_access_t* core, const phymod_core_init_config_t* init_config, const phymod_core_status_t* core_status)
{
    return (_dino_core_init(core, init_config));
}

int dino_phy_init(const phymod_phy_access_t* phy, const phymod_phy_init_config_t* init_config)
{
    phymod_polarity_t polarity;
    phymod_phy_access_t phy_copy;

    PHYMOD_MEMSET(&polarity, 0, sizeof(phymod_polarity_t));
    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phymod_phy_access_t));

    if ((init_config->polarity.tx_polarity != 0) || 
          (init_config->polarity.rx_polarity != 0)) {
        if ((init_config->interface.data_rate != DINO_SPD_100G) && 
            (init_config->interface.data_rate != DINO_SPD_106G)) {    
            PHYMOD_IF_ERR_RETURN(
                dino_phy_polarity_get (phy, &polarity));
            polarity.rx_polarity &= ~(phy->access.lane_mask);
            polarity.rx_polarity |= (init_config->polarity.rx_polarity & phy->access.lane_mask);
            polarity.tx_polarity &= ~(phy->access.lane_mask);
            polarity.tx_polarity |= (init_config->polarity.tx_polarity & phy->access.lane_mask);
        } else {
            polarity.rx_polarity = init_config->polarity.rx_polarity;
            polarity.tx_polarity = init_config->polarity.tx_polarity;
        }

        PHYMOD_IF_ERR_RETURN(
            dino_phy_polarity_set (phy, &polarity)); 
    }

    /*enable/disable repeater/retimer*/ 
    if (init_config->op_mode == phymodOperationModeRetimer) {
        PHYMOD_IF_ERR_RETURN
            (_dino_phy_rptr_rtmr_mode_set(phy, phymodOperationModeRetimer));
    } else {
        /* By default HW set it as repeater*/ 
        PHYMOD_IF_ERR_RETURN
            (_dino_phy_rptr_rtmr_mode_set(phy, phymodOperationModeRepeater));
    }

     /* Looback disable during phy_init to be defult */
    PHYMOD_IF_ERR_RETURN
        (dino_phy_loopback_set(phy, phymodLoopbackGlobalPMD, DINO_DISABLE));
    PHYMOD_IF_ERR_RETURN
        (dino_phy_loopback_set(phy, phymodLoopbackRemotePMD, DINO_DISABLE));
    /* system side interface bit shift */
    phy_copy.port_loc = phymodPortLocSys;
    PHYMOD_IF_ERR_RETURN
        (dino_phy_loopback_set(&phy_copy, phymodLoopbackGlobalPMD, DINO_DISABLE));
    PHYMOD_IF_ERR_RETURN
        (dino_phy_loopback_set(&phy_copy, phymodLoopbackRemotePMD, DINO_DISABLE));
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
 *    @param enable                    Enable or disable
 *        0 - disable
 *        1 - enable
 *
 *    @return PHYMOD_E_NONE     successful function execution
 */
int dino_phy_loopback_set(const phymod_phy_access_t* phy, phymod_loopback_mode_t loopback, uint32_t enable)
{
    uint16_t lane = 0;
    uint16_t lane_mask = 0;
    uint16_t if_side = 0;
    uint32_t ena_dis = 0;
    int num_lanes = 0;
    uint32_t chip_id = 0;
    uint32_t rev_id = 0;
    BCMI_DINO_FIRMWARE_ENr_t fw_en;
    BCMI_DINO_DECD_MODE_STS0r_t mode_sts0;
    uint16_t gpreg_data  = 0;
    const phymod_access_t *pa = &phy->access;
    PHYMOD_MEMSET(&fw_en, 0 , sizeof(BCMI_DINO_FIRMWARE_ENr_t));
    PHYMOD_MEMSET(&mode_sts0, 0, sizeof(BCMI_DINO_DECD_MODE_STS0r_t));

    lane_mask = PHYMOD_ACC_LANE_MASK(pa);
    DINO_GET_INTF_SIDE(phy, if_side);
    PHYMOD_IF_ERR_RETURN (dino_get_chipid(&phy->access, &chip_id, &rev_id));
    DINO_GET_NUM_LANES(chip_id, num_lanes);

    PHYMOD_DEBUG_VERBOSE(("Dino looopback set\n"));
    
    PHYMOD_IF_ERR_RETURN(
        BCMI_DINO_READ_DECD_MODE_STS0r(pa, &mode_sts0));
    /* Check to see if the mode of operation is 100G PT. For 100G PT,
     * dino_fw_enable needs to be called only once
     */
    if (BCMI_DINO_DECD_MODE_STS0r_DECODED_MODE_100Gf_GET(mode_sts0)) {
        if (BCMI_DINO_DECD_MODE_STS0r_DECODED_MODE_100G_PT_2TO11f_GET(mode_sts0)) {
            lane_mask = DINO_100G_TYPE3_LANE_MASK;
        } else if (BCMI_DINO_DECD_MODE_STS0r_DECODED_MODE_100G_PT_1TO10f_GET(mode_sts0)) {
            lane_mask = DINO_100G_TYPE2_LANE_MASK;
        } else if (BCMI_DINO_DECD_MODE_STS0r_DECODED_MODE_100G_PT_0TO9f_GET(mode_sts0)) {
            lane_mask = DINO_100G_TYPE1_LANE_MASK;
        } else {
            lane_mask = DINO_100G_TYPE1_LANE_MASK;
        }
        if (enable) {
            READ_DINO_PMA_PMD_REG(pa, DINO_GPREG_0_ADR, gpreg_data);
            gpreg_data &= ~(DINO_FW_SM_ENABLE_MASK);
            gpreg_data |= (DINO_FW_SM_ENABLE_MASK);
            WRITE_DINO_PMA_PMD_REG(pa, DINO_GPREG_0_ADR, gpreg_data);
            PHYMOD_IF_ERR_RETURN
                (_dino_fw_enable(pa));
        }
        for (lane = 0; lane < num_lanes; lane ++) {
            if ((lane_mask & (1 << lane))) {
                PHYMOD_IF_ERR_RETURN(
                    _dino_loopback_get(pa, if_side, lane, loopback, &ena_dis));
                if (ena_dis || enable) {
	                PHYMOD_IF_ERR_RETURN(
                        _dino_loopback_set(pa, if_side, lane, loopback, enable));
                }
            }
        }
        if (!enable && ena_dis) {
            READ_DINO_PMA_PMD_REG(pa, DINO_GPREG_0_ADR, gpreg_data);
            gpreg_data &= ~(DINO_FW_SM_ENABLE_MASK);
            WRITE_DINO_PMA_PMD_REG(pa, DINO_GPREG_0_ADR, gpreg_data);
            PHYMOD_IF_ERR_RETURN
                (_dino_fw_enable(pa));
        }
    } else {
        for (lane = 0; lane < num_lanes; lane ++) {
            if ((lane_mask & (1 << lane))) {
                READ_DINO_PMA_PMD_REG(pa, (DINO_GPREG_0_ADR + lane), gpreg_data);
                if (enable) {
                    gpreg_data &= ~(DINO_FW_SM_ENABLE_MASK);
                    gpreg_data |= (DINO_FW_SM_ENABLE_MASK);
                }
                WRITE_DINO_PMA_PMD_REG(pa, (DINO_GPREG_0_ADR + lane), gpreg_data);
            }
        }
        PHYMOD_IF_ERR_RETURN
            (_dino_fw_enable(pa));

        for (lane = 0; lane < num_lanes; lane ++) {
            if ((lane_mask & (1 << lane))) {
                PHYMOD_IF_ERR_RETURN(
                    _dino_loopback_get(pa, if_side, lane, loopback, &ena_dis));
                if (ena_dis || enable) {
	                PHYMOD_IF_ERR_RETURN(
                        _dino_loopback_set(pa, if_side, lane, loopback, enable));
                }
            }
        }

        for (lane = 0; lane < num_lanes; lane ++) {
            if ((lane_mask & (1 << lane))) {
                READ_DINO_PMA_PMD_REG(pa, (DINO_GPREG_0_ADR + lane), gpreg_data);
                if (!enable) {
                    gpreg_data &= ~(DINO_FW_SM_ENABLE_MASK);
                }
                WRITE_DINO_PMA_PMD_REG(pa, (DINO_GPREG_0_ADR + lane), gpreg_data);
            }
        }
        PHYMOD_IF_ERR_RETURN
            (_dino_fw_enable(pa));
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
 *    @param enable                    Enable or disable
 *        0 - disable
 *        1 - enable
 *
 *    @return PHYMOD_E_NONE     successful function execution
 */
int dino_phy_loopback_get(const phymod_phy_access_t* phy, phymod_loopback_mode_t loopback, uint32_t* enable)
{
    uint16_t lane = 0;
    uint16_t lane_mask = 0;
    uint16_t if_side = 0;
    int num_lanes = 0;
    uint32_t chip_id = 0;
    uint32_t rev_id = 0;
    const phymod_access_t *pa = &phy->access;

    lane_mask = PHYMOD_ACC_LANE_MASK(pa);
    DINO_GET_INTF_SIDE(phy, if_side);
    PHYMOD_IF_ERR_RETURN (dino_get_chipid(&phy->access, &chip_id, &rev_id));
    DINO_GET_NUM_LANES(chip_id, num_lanes);

    for (lane = 0; lane < num_lanes; lane ++) {
        if ((lane_mask & (1 << lane))) {
            PHYMOD_IF_ERR_RETURN (
                _dino_set_slice_reg (pa, DINO_SLICE_UNICAST, if_side, lane));
            PHYMOD_IF_ERR_RETURN(
                _dino_loopback_get(pa, if_side, lane, loopback, enable));
        }
    }
    PHYMOD_IF_ERR_RETURN (
        _dino_set_slice_reg (pa, DINO_SLICE_RESET, DINO_IF_LINE, 0));

    return PHYMOD_E_NONE;
}
/**   Get PHY RX PMD link status 
 *    This function retrieves RX PMD lock status of dino 
 *
 *    @param phy                Pointer to phymod phy access structure 
 *    @param rx_pmd_locked      Rx PMD Link status retrieved from dino
 *
 *    @return PHYMOD_E_NONE     successful function execution 
 */

int dino_phy_rx_pmd_locked_get(const phymod_phy_access_t* phy, uint32_t* rx_pmd_locked)
{
    uint16_t lane = 0;
    uint16_t lane_mask = 0;
    uint16_t if_side = 0;
    int num_lanes = 0;
    uint32_t chip_id = 0;
    uint32_t rev_id = 0;
    const phymod_access_t *pa = &phy->access;
    lane_mask = PHYMOD_ACC_LANE_MASK(pa);
    DINO_GET_INTF_SIDE(phy, if_side);
    *rx_pmd_locked = 1;
    PHYMOD_IF_ERR_RETURN (dino_get_chipid(&phy->access, &chip_id, &rev_id));
    DINO_GET_NUM_LANES(chip_id, num_lanes);

    for (lane = 0; lane < num_lanes; lane ++) {
        if ((lane_mask & (1 << lane))) {
            PHYMOD_IF_ERR_RETURN (
                _dino_set_slice_reg (pa, DINO_SLICE_UNICAST, if_side, lane));
            PHYMOD_IF_ERR_RETURN(
                _dino_rx_pmd_lock_get(pa, if_side, lane, rx_pmd_locked));
        }
    }
    PHYMOD_IF_ERR_RETURN (
        _dino_set_slice_reg (pa, DINO_SLICE_RESET, DINO_IF_LINE, 0));

    return PHYMOD_E_NONE;
}

/**   Get PCS link status
 *    This function retrieves PCS link status of dino when PCS Monitor is enabled,
 *    This function return PMD lock when PCS monitor is disabled.
 *
 *    @param phy                Pointer to phymod phy access structure 
 *    @param link_status        Retrives PCS Link of dino
 *
 *    @return PHYMOD_E_NONE     successful function execution 
 */

int dino_phy_link_status_get(const phymod_phy_access_t* phy, uint32_t* link_status)
{
    uint16_t lane = 0;
    uint16_t lane_mask = 0;
    uint16_t if_side = 0;
    int num_lanes = 0;
    uint32_t chip_id = 0;
    uint32_t rev_id = 0;
    uint32_t enable = 0;
    uint32_t lock_status = 0;
    uint32_t lock_lost_lh = 0;
    uint32_t error_count = 0;
    const phymod_access_t *pa = &phy->access;
    lane_mask = PHYMOD_ACC_LANE_MASK(pa);
    DINO_GET_INTF_SIDE(phy, if_side);
    *link_status = 1;
    PHYMOD_IF_ERR_RETURN (dino_get_chipid(&phy->access, &chip_id, &rev_id));
    DINO_GET_NUM_LANES(chip_id, num_lanes);

    lock_status = 1;
    lock_lost_lh = 1;
    error_count = 0;
    enable = 1;

    for (lane = 0; lane < num_lanes; lane ++) {
        if ((lane_mask & (1 << lane))) {
            PHYMOD_IF_ERR_RETURN (
                _dino_set_slice_reg (pa, DINO_SLICE_UNICAST, if_side, lane));
            PHYMOD_IF_ERR_RETURN(
                _dino_get_rx_pmd_lock_status(pa, if_side, lane, link_status));
            PHYMOD_IF_ERR_RETURN (
                _dino_phy_link_mon_enable_get(pa, &enable));
            if (enable) {
                PHYMOD_IF_ERR_RETURN (
                    _dino_phy_link_mon_status_get(pa, &lock_status, &lock_lost_lh, &error_count));
                *link_status &= lock_status;
            }
        }
    }
    PHYMOD_IF_ERR_RETURN (
        _dino_set_slice_reg (pa, DINO_SLICE_RESET, DINO_IF_LINE, 0));

    return PHYMOD_E_NONE;
}

/**   Read Register
 *    This function read user specified register based on the specified Device type 
 *
 *    @param phy                Pointer to phymod phy access structure.
 *    @param reg_addr           dino register address
 *    @param val                Output parameter, represents the value of address specified
 *
 *    @return PHYMOD_E_NONE     successful function execution 
 */
int dino_phy_reg_read(const phymod_phy_access_t* phy, uint32_t reg_addr, uint32_t* val)
{
    uint16_t dev_add = 0;
    dev_add = (phy->access.devad) ? phy->access.devad : DINO_DEV_PMA_PMD;

    if (dev_add == DINO_DEV_PMA_PMD) {
        READ_DINO_PMA_PMD_REG(&phy->access, reg_addr, *val);
    } else {
        READ_DINO_AN_REG(&phy->access, reg_addr, *val);
    }

    return PHYMOD_E_NONE;
}

/**   Write Register
 *    This function write user specified value to the specified address 
 *    based on the specified Device type 
 *
 *    @param phy                Pointer to phymod phy access structure.
 *    @param reg_addr           dino register address
 *    @param val                Represents the value to be written
 *
 *    @return PHYMOD_E_NONE     successful function execution 
 */
int dino_phy_reg_write(const phymod_phy_access_t* phy, uint32_t reg_addr, uint32_t val)
{
    uint16_t dev_add = 0;

    dev_add = (phy->access.devad) ? phy->access.devad : DINO_DEV_PMA_PMD;
    if (dev_add == DINO_DEV_PMA_PMD) {
        WRITE_DINO_PMA_PMD_REG(&phy->access, reg_addr, val);
    } else {
        WRITE_DINO_AN_REG(&phy->access, reg_addr, val);
    }

    return PHYMOD_E_NONE;
}

/**   Retrives Revision 
 *    This function retrives dino revision ID 
 *
 *    @param phy                Pointer to phymod phy access structure.
 *    @param rev_id             Output parameter, Represents the revision ID of dino.
 *
 *    @return PHYMOD_E_NONE     successful function execution 
 */
int dino_phy_rev_id(const phymod_phy_access_t* phy, uint32_t *rev_id)
{
    uint32_t chip_id = 0;

    PHYMOD_IF_ERR_RETURN (
        dino_get_chipid(&phy->access, &chip_id, rev_id));
 
    return PHYMOD_E_NONE;
}

/**  PHY status dump
 *   This function is used to display status dump of a core and for given lane
 *
 *    @param phy                Pointer to phymod phy access structure
 *
 *    @return PHYMOD_E_NONE     successful function execution
 */
int dino_phy_status_dump(const phymod_phy_access_t* phy)
{
    uint16_t lane = 0;
    uint16_t lane_mask = 0;
    uint16_t if_side = 0;
    int num_lanes = 0;
    uint32_t chip_id = 0;
    uint32_t rev_id = 0;
    const phymod_access_t *pa = &phy->access;

    lane_mask = PHYMOD_ACC_LANE_MASK(pa);
    DINO_GET_INTF_SIDE(phy, if_side);
    PHYMOD_IF_ERR_RETURN (dino_get_chipid(&phy->access, &chip_id, &rev_id));
    DINO_GET_NUM_LANES(chip_id, num_lanes);

    PHYMOD_DEBUG_VERBOSE(("**********************************************\n"));
    PHYMOD_DEBUG_VERBOSE(("******* PHY status dump for PHY ID:%d ********\n", pa->addr));
    PHYMOD_DEBUG_VERBOSE(("**********************************************\n"));
    PHYMOD_DEBUG_VERBOSE(("**** PHY status dump for interface side:%d ****\n", if_side));
    PHYMOD_DEBUG_VERBOSE(("***********************************************\n"));

    /* For core status dump */
    for (lane = 0; lane < num_lanes; lane ++) {
        if ((lane_mask & (1 << lane))) {
            PHYMOD_IF_ERR_RETURN (
                _dino_set_slice_reg (pa, DINO_SLICE_UNICAST, if_side, lane));
            PHYMOD_IF_ERR_RETURN(
                _dino_phy_status_dump(pa, if_side, lane));
        }
    }

    PHYMOD_IF_ERR_RETURN (
        _dino_set_slice_reg (pa, DINO_SLICE_RESET, DINO_IF_LINE, 0));
    return PHYMOD_E_NONE;
}
int dino_phy_rx_lane_control_get(const phymod_phy_access_t* phy, phymod_phy_rx_lane_control_t* rx_control)
{
    uint16_t lane = 0;
    uint16_t lane_mask = 0;
    uint16_t if_side = 0;
    int num_lanes = 0;
    uint32_t chip_id = 0;
    uint32_t rev_id = 0;
    const phymod_access_t *pa = &phy->access;
    lane_mask = PHYMOD_ACC_LANE_MASK(pa);
    DINO_GET_INTF_SIDE(phy, if_side);
    PHYMOD_IF_ERR_RETURN (dino_get_chipid(&phy->access, &chip_id, &rev_id));
    DINO_GET_NUM_LANES(chip_id, num_lanes);

    for (lane = 0; lane < num_lanes; lane ++) {
        if ((lane_mask & (1 << lane))) {
            PHYMOD_IF_ERR_RETURN (
                _dino_set_slice_reg (pa, DINO_SLICE_UNICAST, if_side, lane));
            PHYMOD_IF_ERR_RETURN(
                _dino_phy_rx_lane_control_get(pa, if_side, lane, rx_control));
            break;
        }
    }
    PHYMOD_IF_ERR_RETURN (
        _dino_set_slice_reg (pa, DINO_SLICE_RESET, DINO_IF_LINE, 0));
    return PHYMOD_E_NONE;
}

int dino_phy_autoneg_set(const phymod_phy_access_t* phy, const phymod_autoneg_control_t* an)
{
    return (_dino_phy_autoneg_set(phy, an));
}

int dino_phy_autoneg_get(const phymod_phy_access_t* phy, phymod_autoneg_control_t* an, uint32_t* an_done)
{
    return (_dino_phy_autoneg_get(phy, an, an_done));
}

int dino_phy_tx_set(const phymod_phy_access_t* phy, const phymod_tx_t* tx)
{
    uint16_t lane = 0;
    uint16_t lane_mask = 0;
    uint16_t if_side = 0;
    int num_lanes = 0;
    uint32_t chip_id = 0;
    uint32_t rev_id = 0;
    const phymod_access_t *pa = &phy->access;
    lane_mask = PHYMOD_ACC_LANE_MASK(pa);
    DINO_GET_INTF_SIDE(phy, if_side);
    PHYMOD_IF_ERR_RETURN (dino_get_chipid(&phy->access, &chip_id, &rev_id));
    DINO_GET_NUM_LANES(chip_id, num_lanes);

    for (lane = 0; lane < num_lanes; lane ++) {
        if ((lane_mask & (1 << lane))) {
            PHYMOD_IF_ERR_RETURN(_dino_set_slice_reg (pa, DINO_SLICE_UNICAST, if_side, lane));
            PHYMOD_IF_ERR_RETURN(_dino_phy_tx_set(pa, if_side, lane, tx));
        }
    }
    PHYMOD_IF_ERR_RETURN (
        _dino_set_slice_reg (pa, DINO_SLICE_RESET, DINO_IF_LINE, 0));

    return PHYMOD_E_NONE;
}

int dino_phy_tx_get(const phymod_phy_access_t* phy, phymod_tx_t* tx)
{
    uint16_t lane = 0;
    uint16_t lane_mask = 0;
    uint16_t if_side = 0;
    int num_lanes = 0;
    uint32_t chip_id = 0;
    uint32_t rev_id = 0;
    const phymod_access_t *pa = &phy->access;
    lane_mask = PHYMOD_ACC_LANE_MASK(pa);
    DINO_GET_INTF_SIDE(phy, if_side);
    PHYMOD_IF_ERR_RETURN (dino_get_chipid(&phy->access, &chip_id, &rev_id));
    DINO_GET_NUM_LANES(chip_id, num_lanes);

    for (lane = 0; lane < num_lanes; lane ++) {
        if ((lane_mask & (1 << lane))) {
            PHYMOD_IF_ERR_RETURN (
                _dino_set_slice_reg (pa, DINO_SLICE_UNICAST, if_side, lane));
            PHYMOD_IF_ERR_RETURN(
                _dino_phy_tx_get(pa, if_side, lane, tx));
            break;
        }
    }
    PHYMOD_IF_ERR_RETURN (
        _dino_set_slice_reg (pa, DINO_SLICE_RESET, DINO_IF_LINE, 0));

    return PHYMOD_E_NONE;
}

int dino_phy_rx_set(const phymod_phy_access_t* phy, const phymod_rx_t* rx)
{
    uint16_t lane = 0;
    uint16_t lane_mask = 0;
    uint16_t if_side = 0;
    int num_lanes = 0;
    uint32_t chip_id = 0;
    uint32_t rev_id = 0;
    const phymod_access_t *pa = &phy->access;
    lane_mask = PHYMOD_ACC_LANE_MASK(pa);
    DINO_GET_INTF_SIDE(phy, if_side);
    PHYMOD_IF_ERR_RETURN (dino_get_chipid(&phy->access, &chip_id, &rev_id));
    DINO_GET_NUM_LANES(chip_id, num_lanes);

    for (lane = 0; lane < num_lanes; lane ++) {
        if ((lane_mask & (1 << lane))) {
            PHYMOD_IF_ERR_RETURN (
                _dino_set_slice_reg (pa, DINO_SLICE_UNICAST, if_side, lane));
            PHYMOD_IF_ERR_RETURN(
                _dino_phy_rx_set(pa, if_side, lane, rx));
        }
    }
    PHYMOD_IF_ERR_RETURN (
        _dino_set_slice_reg (pa, DINO_SLICE_RESET, DINO_IF_LINE, 0));

    return PHYMOD_E_NONE;
}

int dino_phy_rx_get(const phymod_phy_access_t* phy, phymod_rx_t* rx)
{
    uint16_t lane = 0;
    uint16_t lane_mask = 0;
    uint16_t if_side = 0;
    int num_lanes = 0;
    uint32_t chip_id = 0;
    uint32_t rev_id = 0;
    const phymod_access_t *pa = &phy->access;
    lane_mask = PHYMOD_ACC_LANE_MASK(pa);
    DINO_GET_INTF_SIDE(phy, if_side);
    PHYMOD_IF_ERR_RETURN (dino_get_chipid(&phy->access, &chip_id, &rev_id));
    DINO_GET_NUM_LANES(chip_id, num_lanes);

    for (lane = 0; lane < num_lanes; lane ++) {
        if ((lane_mask & (1 << lane))) {
            PHYMOD_IF_ERR_RETURN (
                _dino_set_slice_reg (pa, DINO_SLICE_UNICAST, if_side, lane));
            PHYMOD_IF_ERR_RETURN(
                _dino_phy_rx_get(pa, if_side, lane, rx));
        }
    }
    PHYMOD_IF_ERR_RETURN (
        _dino_set_slice_reg (pa, DINO_SLICE_RESET, DINO_IF_LINE, 0));

    return PHYMOD_E_NONE;
}

int dino_phy_rx_adaptation_resume(const phymod_phy_access_t* phy)
{
    uint16_t lane = 0;
    uint16_t lane_mask = 0;
    uint16_t if_side = 0;
    int num_lanes = 0;
    uint32_t chip_id = 0;
    uint32_t rev_id = 0;
    const phymod_access_t *pa = &phy->access;
    lane_mask = PHYMOD_ACC_LANE_MASK(pa);
    DINO_GET_INTF_SIDE(phy, if_side);
    PHYMOD_IF_ERR_RETURN (dino_get_chipid(&phy->access, &chip_id, &rev_id));
    DINO_GET_NUM_LANES(chip_id, num_lanes);

    for (lane = 0; lane < num_lanes; lane ++) {
        if ((lane_mask & (1 << lane))) {
            PHYMOD_IF_ERR_RETURN (
                _dino_set_slice_reg (pa, DINO_SLICE_UNICAST, if_side, lane));
            PHYMOD_IF_ERR_RETURN(
                _dino_phy_rx_adaptation_resume(pa, if_side, lane));
        }
    }
    PHYMOD_IF_ERR_RETURN (
        _dino_set_slice_reg (pa, DINO_SLICE_RESET, DINO_IF_LINE, 0));

    return  PHYMOD_E_NONE;
}

int dino_phy_power_set(const phymod_phy_access_t* phy, const phymod_phy_power_t* power)
{
    uint16_t lane = 0;
    uint16_t lane_mask = 0;
    uint16_t if_side = 0;
    int num_lanes = 0;
    uint32_t chip_id = 0;
    uint32_t rev_id = 0;
    const phymod_access_t *pa = &phy->access;

    lane_mask = PHYMOD_ACC_LANE_MASK(pa);
    DINO_GET_INTF_SIDE(phy, if_side);
    PHYMOD_IF_ERR_RETURN (dino_get_chipid(&phy->access, &chip_id, &rev_id));
    DINO_GET_NUM_LANES(chip_id, num_lanes);

    for (lane = 0; lane < num_lanes; lane ++) {
        if ((lane_mask & (1 << lane))) {
            PHYMOD_IF_ERR_RETURN (
                _dino_set_slice_reg (pa, DINO_SLICE_UNICAST, if_side, lane));
            PHYMOD_IF_ERR_RETURN(
                _dino_phy_power_set(phy, if_side, lane, power));
        }
    }
    PHYMOD_IF_ERR_RETURN (
        _dino_set_slice_reg (pa, DINO_SLICE_RESET, DINO_IF_LINE, 0));

    return PHYMOD_E_NONE;
}

int dino_phy_power_get(const phymod_phy_access_t* phy, phymod_phy_power_t* power)
{
    uint16_t lane = 0;
    uint16_t lane_mask = 0;
    uint16_t if_side = 0;
    int num_lanes = 0;
    uint32_t chip_id = 0;
    uint32_t rev_id = 0;
    const phymod_access_t *pa = &phy->access;

    lane_mask = PHYMOD_ACC_LANE_MASK(pa);
    DINO_GET_INTF_SIDE(phy, if_side);
    PHYMOD_IF_ERR_RETURN (dino_get_chipid(&phy->access, &chip_id, &rev_id));
    DINO_GET_NUM_LANES(chip_id, num_lanes);

    for (lane = 0; lane < num_lanes; lane ++) {
        if ((lane_mask & (1 << lane))) {
            PHYMOD_IF_ERR_RETURN (
                _dino_set_slice_reg (pa, DINO_SLICE_UNICAST, if_side, lane));
            PHYMOD_IF_ERR_RETURN(
                _dino_phy_power_get(phy, if_side, lane, power));
            break;
        }
    }
    PHYMOD_IF_ERR_RETURN (
        _dino_set_slice_reg (pa, DINO_SLICE_RESET, DINO_IF_LINE, 0));

    return PHYMOD_E_NONE;
}

int dino_phy_autoneg_ability_set(const phymod_phy_access_t* phy, const phymod_autoneg_ability_t* an_ability)
{
    dino_an_ability_t dino_an_ability;

    PHYMOD_MEMSET(&dino_an_ability, 0, sizeof(dino_an_ability));
    dino_an_ability.cl73_adv.an_fec = an_ability->an_fec;
    dino_an_ability.cl73_adv.an_cl72 = an_ability->an_cl72;
    dino_an_ability.an_master_lane = an_ability->an_master_lane;


    /*check if sgmii  or not */
    if (PHYMOD_AN_CAP_SGMII_GET(an_ability)){
        return PHYMOD_E_NONE;
    }

    /*next check pause */
    if (PHYMOD_AN_CAP_SYMM_PAUSE_GET(an_ability)) {
        dino_an_ability.cl73_adv.an_pause = DINO_SYMM_PAUSE;
    } else if (PHYMOD_AN_CAP_ASYM_PAUSE_GET(an_ability)) {
        dino_an_ability.cl73_adv.an_pause = DINO_ASYM_PAUSE;
    } else {
        dino_an_ability.cl73_adv.an_pause = DINO_NO_PAUSE;
    }

    /*check cl73 and cl73 bam ability */
    if (PHYMOD_AN_CAP_100G_CR4_GET(an_ability->an_cap)) {
        dino_an_ability.cl73_adv.an_base_speed = DINO_CL73_100GBASE_CR4;
    } else if (PHYMOD_AN_CAP_100G_KR4_GET(an_ability->an_cap)) {
        dino_an_ability.cl73_adv.an_base_speed = DINO_CL73_100GBASE_KR4;
    } else if (PHYMOD_AN_CAP_100G_CR10_GET(an_ability->an_cap)) {
        dino_an_ability.cl73_adv.an_base_speed = DINO_CL73_100GBASE_CR10;
    } else if (PHYMOD_AN_CAP_40G_KR4_GET(an_ability->an_cap)) {
        dino_an_ability.cl73_adv.an_base_speed = DINO_CL73_40GBASE_KR4;
    } else if (PHYMOD_AN_CAP_40G_CR4_GET(an_ability->an_cap)) {
        dino_an_ability.cl73_adv.an_base_speed = DINO_CL73_40GBASE_CR4;
    } else if (PHYMOD_AN_CAP_10G_KR_GET(an_ability->an_cap)) {
         
    } else if (PHYMOD_AN_CAP_1G_KX_GET(an_ability->an_cap)) {
         
    } else if (an_ability->an_cap == 0) {
        return PHYMOD_E_NONE;
    } else {
        return PHYMOD_E_PARAM;
    }

    PHYMOD_IF_ERR_RETURN
        (_dino_phy_autoneg_ability_set(phy, &dino_an_ability));

    return PHYMOD_E_NONE;
}

int dino_phy_autoneg_ability_get(const phymod_phy_access_t* phy, phymod_autoneg_ability_t* an_ability_get_type)
{
    dino_an_ability_t dino_an_ability;

    PHYMOD_IF_ERR_RETURN(
        _dino_phy_autoneg_ability_get(phy, &dino_an_ability));

    an_ability_get_type->an_fec = dino_an_ability.cl73_adv.an_fec;
    an_ability_get_type->an_cl72 = dino_an_ability.cl73_adv.an_cl72;
    an_ability_get_type->an_master_lane = dino_an_ability.an_master_lane;
    switch (dino_an_ability.cl73_adv.an_pause) {
        case DINO_ASYM_PAUSE:
            PHYMOD_AN_CAP_ASYM_PAUSE_SET(an_ability_get_type);
        break;
        case DINO_SYMM_PAUSE:
            PHYMOD_AN_CAP_SYMM_PAUSE_SET(an_ability_get_type);
        break;
        default:
            break;
    }

    /*first check cl73 ability*/
    switch (dino_an_ability.cl73_adv.an_base_speed) {
        case DINO_CL73_40GBASE_CR4:
            PHYMOD_AN_CAP_40G_CR4_SET(an_ability_get_type->an_cap);
        break;
        case DINO_CL73_40GBASE_KR4:
            PHYMOD_AN_CAP_40G_KR4_SET(an_ability_get_type->an_cap);
        break;
        case DINO_CL73_100GBASE_CR4:
            PHYMOD_AN_CAP_100G_CR4_SET(an_ability_get_type->an_cap);
        break;
        case DINO_CL73_100GBASE_CR10:
            PHYMOD_AN_CAP_100G_CR10_SET(an_ability_get_type->an_cap);
        break;
        case DINO_CL73_100GBASE_KR4:
            PHYMOD_AN_CAP_100G_KR4_SET(an_ability_get_type->an_cap);
        break;
        default:
            break;
    }
    return PHYMOD_E_NONE;
}

int dino_phy_autoneg_remote_ability_get(const phymod_phy_access_t* phy, phymod_autoneg_ability_t* an_ability_get)
{
    return _dino_phy_autoneg_remote_ability_get(phy, an_ability_get);
}

int dino_phy_media_type_tx_get(const phymod_phy_access_t* phy, phymod_media_typed_t media, phymod_tx_t* tx)
{

    PHYMOD_MEMSET(tx, 0, sizeof(phymod_tx_t));

    tx->pre = 0x0;
    tx->post = 0x0;
    tx->main = 0x3c;
    tx->post2 = 0x0;
    tx->amp = 0xc;

    return PHYMOD_E_NONE;
}
int dino_phy_reset_set(const phymod_phy_access_t* phy, const phymod_phy_reset_t* reset)
{
    uint16_t lane = 0;
    uint16_t lane_mask = 0;
    uint16_t if_side = 0;
    int num_lanes = 0;
    uint32_t chip_id = 0;
    uint32_t rev_id = 0;
    const phymod_access_t *pa = &phy->access;

    lane_mask = PHYMOD_ACC_LANE_MASK(pa);
    DINO_GET_INTF_SIDE(phy, if_side);
    PHYMOD_IF_ERR_RETURN (dino_get_chipid(&phy->access, &chip_id, &rev_id));
    DINO_GET_NUM_LANES(chip_id, num_lanes);

    for (lane = 0; lane < num_lanes; lane ++) {
        if ((lane_mask & (1 << lane))) {
            PHYMOD_IF_ERR_RETURN (
                _dino_set_slice_reg (pa, DINO_SLICE_UNICAST, if_side, lane));
            PHYMOD_IF_ERR_RETURN(
                _dino_phy_reset_set(pa, if_side, lane, reset));
        }
    }
    PHYMOD_IF_ERR_RETURN (
        _dino_set_slice_reg (pa, DINO_SLICE_RESET, DINO_IF_LINE, 0));

    return PHYMOD_E_NONE;
}
int dino_phy_reset_get(const phymod_phy_access_t* phy, phymod_phy_reset_t* reset)
{
    uint16_t lane = 0;
    uint16_t lane_mask = 0;
    uint16_t if_side = 0;
    int num_lanes = 0;
    uint32_t chip_id = 0;
    uint32_t rev_id = 0;
    const phymod_access_t *pa = &phy->access;

    lane_mask = PHYMOD_ACC_LANE_MASK(pa);
    DINO_GET_INTF_SIDE(phy, if_side);
    PHYMOD_IF_ERR_RETURN (dino_get_chipid(&phy->access, &chip_id, &rev_id));
    DINO_GET_NUM_LANES(chip_id, num_lanes);

    for (lane = 0; lane < num_lanes; lane ++) {
        if ((lane_mask & (1 << lane))) {
            PHYMOD_IF_ERR_RETURN (
                _dino_set_slice_reg (pa, DINO_SLICE_UNICAST, if_side, lane));
            PHYMOD_IF_ERR_RETURN(
                _dino_phy_reset_get(pa, if_side, lane, reset));
            break;
        }
    }
    PHYMOD_IF_ERR_RETURN (
        _dino_set_slice_reg (pa, DINO_SLICE_RESET, DINO_IF_LINE, 0));

    return PHYMOD_E_NONE;
}

int dino_phy_rx_lane_control_set(const phymod_phy_access_t* phy, phymod_phy_rx_lane_control_t rx_control)
{
    uint16_t lane = 0;
    uint16_t lane_mask = 0;
    uint16_t if_side = 0;
    int num_lanes = 0;
    uint32_t chip_id = 0;
    uint32_t rev_id = 0;
    const phymod_access_t *pa = &phy->access;
    lane_mask = PHYMOD_ACC_LANE_MASK(pa);
    DINO_GET_INTF_SIDE(phy, if_side);
    PHYMOD_IF_ERR_RETURN (dino_get_chipid(&phy->access, &chip_id, &rev_id));
    DINO_GET_NUM_LANES(chip_id, num_lanes);

    for (lane = 0; lane < num_lanes; lane ++) {
        if ((lane_mask & (1 << lane))) {
            PHYMOD_IF_ERR_RETURN (
                _dino_set_slice_reg (pa, DINO_SLICE_UNICAST, if_side, lane));
            PHYMOD_IF_ERR_RETURN(
                _dino_phy_rx_lane_control_set(pa, if_side, lane, rx_control));
        }
    }
    PHYMOD_IF_ERR_RETURN (
        _dino_set_slice_reg (pa, DINO_SLICE_RESET, DINO_IF_LINE, 0));

    return PHYMOD_E_NONE;
}

int dino_phy_tx_lane_control_set(const phymod_phy_access_t* phy, phymod_phy_tx_lane_control_t tx_control)
{
    uint16_t lane = 0;
    uint16_t lane_mask = 0;
    uint16_t if_side = 0;
    int num_lanes = 0;
    uint32_t chip_id = 0;
    uint32_t rev_id = 0;
    const phymod_access_t *pa = &phy->access;
    lane_mask = PHYMOD_ACC_LANE_MASK(pa);
    DINO_GET_INTF_SIDE(phy, if_side);
    PHYMOD_IF_ERR_RETURN (dino_get_chipid(&phy->access, &chip_id, &rev_id));
    DINO_GET_NUM_LANES(chip_id, num_lanes);

    for (lane = 0; lane < num_lanes; lane ++) {
        if ((lane_mask & (1 << lane))) {
            PHYMOD_IF_ERR_RETURN (
                _dino_set_slice_reg (pa, DINO_SLICE_UNICAST, if_side, lane));
            PHYMOD_IF_ERR_RETURN(
                _dino_phy_tx_lane_control_set(pa, if_side, lane, tx_control));
        }
    }
    PHYMOD_IF_ERR_RETURN (
        _dino_set_slice_reg (pa, DINO_SLICE_RESET, DINO_IF_LINE, 0));

    return PHYMOD_E_NONE;
}
int dino_phy_tx_lane_control_get(const phymod_phy_access_t* phy, phymod_phy_tx_lane_control_t* tx_control)
{
    uint16_t lane = 0;
    uint16_t lane_mask = 0;
    uint16_t if_side = 0;
    int num_lanes = 0;
    uint32_t chip_id = 0;
    uint32_t rev_id = 0;
    const phymod_access_t *pa = &phy->access;
    lane_mask = PHYMOD_ACC_LANE_MASK(pa);
    DINO_GET_INTF_SIDE(phy, if_side);
    PHYMOD_IF_ERR_RETURN (dino_get_chipid(&phy->access, &chip_id, &rev_id));
    DINO_GET_NUM_LANES(chip_id, num_lanes);

    for (lane = 0; lane < num_lanes; lane ++) {
        if ((lane_mask & (1 << lane))) {
            PHYMOD_IF_ERR_RETURN (
                _dino_set_slice_reg (pa, DINO_SLICE_UNICAST, if_side, lane));
            PHYMOD_IF_ERR_RETURN(
                _dino_phy_tx_lane_control_get(pa, if_side, lane, tx_control));
            break;
        }
    }
    PHYMOD_IF_ERR_RETURN (
        _dino_set_slice_reg (pa, DINO_SLICE_RESET, DINO_IF_LINE, 0));
    return PHYMOD_E_NONE;
}

int dino_phy_polarity_set(const phymod_phy_access_t* phy, const phymod_polarity_t* polarity)
{
    uint16_t lane = 0;
    uint16_t lane_mask = 0;
    int num_lanes = 0;
    uint32_t chip_id = 0;
    uint32_t rev_id = 0;
    uint16_t if_side = 0, tx_polarity = 0,rx_polarity = 0;
    const phymod_access_t *pa = &phy->access;

    lane_mask = PHYMOD_ACC_LANE_MASK(pa);
    DINO_GET_INTF_SIDE(phy, if_side);
    PHYMOD_IF_ERR_RETURN (dino_get_chipid(&phy->access, &chip_id, &rev_id));
    DINO_GET_NUM_LANES(chip_id, num_lanes);

    for (lane = 0; lane < num_lanes; lane ++) {
        if ((lane_mask & (1 << lane))) {
            PHYMOD_IF_ERR_RETURN (
                _dino_set_slice_reg (pa, DINO_SLICE_UNICAST, if_side, lane));
            if (polarity->tx_polarity != 0xFFFF) {
                tx_polarity = (polarity->tx_polarity & (1 << lane)) >> lane;
            } else {
                tx_polarity = 0xffff;
            }
            if (polarity->rx_polarity != 0xFFFF) {
                rx_polarity = (polarity->rx_polarity & (1 << lane)) >> lane;
            } else {
                rx_polarity = 0xffff;
            }
            PHYMOD_IF_ERR_RETURN(
                _dino_phy_polarity_set(pa, if_side, lane, tx_polarity, rx_polarity));
        }
    }

    PHYMOD_IF_ERR_RETURN (
        _dino_set_slice_reg (pa, DINO_SLICE_RESET, DINO_IF_LINE, 0));

    return PHYMOD_E_NONE;
}
int dino_phy_polarity_get(const phymod_phy_access_t* phy, phymod_polarity_t* polarity)
{
    uint16_t lane = 0;
    uint16_t lane_mask = 0;
    int num_lanes = 0;
    uint32_t chip_id = 0;
    uint32_t rev_id = 0;
    uint16_t if_side = 0, tx_polarity = 0,rx_polarity = 0;
    const phymod_access_t *pa = &phy->access;

    lane_mask = PHYMOD_ACC_LANE_MASK(pa);
    DINO_GET_INTF_SIDE(phy, if_side);
    PHYMOD_IF_ERR_RETURN (dino_get_chipid(&phy->access, &chip_id, &rev_id));
    DINO_GET_NUM_LANES(chip_id, num_lanes);
    polarity->tx_polarity = 0;
    polarity->rx_polarity = 0;

    for (lane = 0; lane < num_lanes; lane ++) {
        if ((lane_mask & (1 << lane))) {
            PHYMOD_IF_ERR_RETURN (
                _dino_set_slice_reg (pa, DINO_SLICE_UNICAST, if_side, lane));
            PHYMOD_IF_ERR_RETURN(
                _dino_phy_polarity_get(pa, if_side, lane, &tx_polarity, &rx_polarity));
            polarity->tx_polarity |= (tx_polarity << lane);
            polarity->rx_polarity |= (rx_polarity << lane);
        }
    }

    PHYMOD_IF_ERR_RETURN (
        _dino_set_slice_reg (pa, DINO_SLICE_RESET, DINO_IF_LINE, 0));

    return PHYMOD_E_NONE;
}
int dino_phy_fec_enable_set(const phymod_phy_access_t* phy, uint32_t enable)
{
    return _dino_phy_fec_set(phy, enable);
}

int dino_phy_fec_enable_get(const phymod_phy_access_t* phy, uint32_t* enable)
{
    return _dino_phy_fec_get(phy, enable);
}

/*
 *    @param flags               Reserved for Furtuer use
 *    @param operation           Operation to perform on PLL sequencer
 *
 *    @return PHYMOD_E_NONE     successful function execution
 */
int dino_core_pll_sequencer_restart(const phymod_core_access_t* core, uint32_t flags, phymod_sequencer_operation_t operation)
{
    uint16_t if_side = 0;
    const phymod_access_t *pa = &core->access;
    DINO_GET_INTF_SIDE(core, if_side);

    PHYMOD_IF_ERR_RETURN(
        _dino_core_pll_sequencer_restart(pa, if_side, operation));

    return  PHYMOD_E_NONE;
}

int dino_phy_rx_restart(const phymod_phy_access_t* phy)
{
    uint16_t if_side = 0;
    const phymod_access_t *pa = &phy->access;
    DINO_GET_INTF_SIDE(phy, if_side);

    PHYMOD_IF_ERR_RETURN(
        _dino_core_pll_sequencer_restart(pa, if_side, phymodSeqOpRestart));

    return  PHYMOD_E_NONE;
}

int dino_phy_cl72_set(const phymod_phy_access_t* phy, uint32_t cl72_en)
{
    return _dino_phy_cl72_set(phy, cl72_en);
}

int dino_phy_cl72_get(const phymod_phy_access_t* phy, uint32_t* cl72_en)
{
    return _dino_phy_cl72_get(phy, cl72_en);
}

int dino_phy_cl72_status_get(const phymod_phy_access_t* phy, phymod_cl72_status_t* status)
{
    uint16_t lane = 0;
    uint16_t lane_mask = 0;
    uint16_t if_side = 0;
    int num_lanes = 0;
    uint32_t chip_id = 0;
    uint32_t rev_id = 0;
    const phymod_access_t *pa = &phy->access;
    status->enabled = 1;
    status->locked = 1;

    lane_mask = PHYMOD_ACC_LANE_MASK(pa);
    DINO_GET_INTF_SIDE(phy, if_side);
    PHYMOD_IF_ERR_RETURN (dino_get_chipid(&phy->access, &chip_id, &rev_id));
    DINO_GET_NUM_LANES(chip_id, num_lanes);

    for (lane = 0; lane < num_lanes; lane ++) {
        if (lane_mask & (1 << lane)) {
             PHYMOD_IF_ERR_RETURN (
                 _dino_set_slice_reg (pa, DINO_SLICE_UNICAST, if_side, lane));
             PHYMOD_IF_ERR_RETURN(
                 _dino_phy_cl72_status_get(phy, if_side, lane, status));
        }
    }
    PHYMOD_IF_ERR_RETURN (
        _dino_set_slice_reg (pa, DINO_SLICE_RESET, DINO_IF_LINE, 0));

    return PHYMOD_E_NONE;
}

int dino_phy_firmware_core_config_set(const phymod_phy_access_t* phy, phymod_firmware_core_config_t fw_config)
{
    /* Place your code here */

    return PHYMOD_E_UNAVAIL;
}


int dino_phy_firmware_core_config_get(const phymod_phy_access_t* phy, phymod_firmware_core_config_t* fw_config)
{
    /* Place your code here */

    return PHYMOD_E_UNAVAIL;
}


int dino_phy_firmware_lane_config_set(const phymod_phy_access_t* phy, phymod_firmware_lane_config_t fw_config)
{
    return _dino_phy_firmware_lane_config_set(phy, fw_config);
}


int dino_phy_firmware_lane_config_get(const phymod_phy_access_t* phy, phymod_firmware_lane_config_t* fw_config)
{
    return _dino_phy_firmware_lane_config_get(phy, fw_config);
}
int dino_phy_intr_enable_set(const phymod_phy_access_t* phy, uint32_t enable)
{
    uint32_t intr_index = 0;
#ifdef SA_PLP_SUPPORT
    uint32_t ena_dis = 0;
    ena_dis = (enable >> 31) & 0x1;
#endif

#ifdef SA_PLP_SUPPORT
    for (intr_index = 0; intr_index < DINO_MAX_INTR_SUPPORT; intr_index ++) {
        if (ena_dis) {
            if (enable & (0x1 << intr_index)) {
                PHYMOD_IF_ERR_RETURN (
                    _dino_phy_intr_enable_set (phy, (0x1 << intr_index), 1));
            } 
        } else {
            if (enable & (0x1 << intr_index)) {
                PHYMOD_IF_ERR_RETURN (
                    _dino_phy_intr_enable_set (phy, (0x1 << intr_index), 0));
            }
        }
    }
#else
    for (intr_index = 0; intr_index < DINO_MAX_INTR_SUPPORT; intr_index ++) {
       if (enable & (0x1 << intr_index)) {
           PHYMOD_IF_ERR_RETURN (
               _dino_phy_intr_enable_set (phy, (0x1 << intr_index), 1));
       } else {
           PHYMOD_IF_ERR_RETURN (
               _dino_phy_intr_enable_set (phy, (0x1 << intr_index), 0));
       }
    }
#endif

    return PHYMOD_E_NONE;
}

int dino_phy_intr_enable_get(const phymod_phy_access_t* phy, uint32_t* enable)
{
    uint32_t intr_index = 0;
    uint32_t value = 0;

    for (intr_index = 0; intr_index < DINO_MAX_INTR_SUPPORT; intr_index ++) {
        PHYMOD_IF_ERR_RETURN (
            _dino_phy_intr_enable_get (phy, (0x1 << intr_index), &value));
        if (value) {
            *enable |= (0x1 << intr_index);
        }
    }

    return PHYMOD_E_NONE;
}

int dino_phy_intr_status_get(const phymod_phy_access_t* phy, uint32_t* intr_status)
{
    PHYMOD_IF_ERR_RETURN (
       _dino_phy_intr_status_get (phy, intr_status));

    return PHYMOD_E_NONE;
}

int dino_phy_intr_status_clear(const phymod_phy_access_t* phy, uint32_t intr_clr)
{
    uint32_t intr_index = 0;

    for (intr_index = 0; intr_index < DINO_MAX_INTR_SUPPORT; intr_index ++) {
        if (intr_clr & (0x1 << intr_index)) {
            PHYMOD_IF_ERR_RETURN (
                _dino_phy_intr_status_clear (phy, (0x1 << intr_index)));
        }
    }

    return PHYMOD_E_NONE;
}

int dino_phy_i2c_read(const phymod_phy_access_t* phy, uint32_t flags, uint32_t addr, uint32_t offset, uint32_t size, uint8_t* data)
{
    return _dino_phy_i2c_read(&phy->access, addr, offset, size, data);
}

int dino_phy_i2c_write(const phymod_phy_access_t* phy, uint32_t flags, uint32_t addr, uint32_t offset, uint32_t size, const uint8_t* data)
{
    return _dino_phy_i2c_write(&phy->access, addr, offset, size, data);
}
/*! \brief Config set for GPIO pins
 *  This set of APIs are used to configure the GPIO pins and 
 *  drive the values (set) and obtain the set values (get)
 *  These generic APIs are  provided to the user to configure GPIOs based on need
 *  User needs to provide direction an the pin number for a given pin   
 *
 *  @param phy            Phymod phy access 
 *  @param gpio_mode      It is 1 for output 2 for input               
 *  @param pin_no         See below table for supported values`
 * 
 * <table>
 * <caption id="multi_row">QSFP module 0 I/O Table</caption>
 * <tr><th>Pin number <th>Pin             <th>Direction
 * </tr>
 * <tr>
 * <td> 18 </td>
 * <td> LPMode  </td>
 * <td> Output </td>
 * </tr>
 * <tr>
 * <td> 21 </td>
 * <td> ModPrSl  </td>
 * <td> Input </td>
 * </tr>
 * <tr>
 * <td> 1 </td>
 * <td> ResetL  </td>
 * <td> Output </td>
 * </tr>
 * <tr>
 * <td> 0 </td>
 * <td> IntL  </td>
 * <td> Input </td>
 * </tr>
 * </table>
 *
 * <table>
 * <caption id="multi_row">QSFP module 1 I/O Table</caption>
 * <tr><th>Pin number <th>Pin             <th>Direction
 * </tr>
 * <tr>
 * <td> 19 </td>
 * <td> LPMode  </td>
 * <td> Output </td>
 * </tr>
 * <tr>
 * <td> 25 </td>
 * <td> ModPrSl  </td>
 * <td> Input </td>
 * </tr>
 * <tr>
 * <td> 9 </td>
 * <td> ResetL  </td>
 * <td> Output </td>
 * </tr>
 * <tr>
 * <td> 8 </td>
 * <td> IntL  </td>
 * <td> Input </td>
 * </tr>
 * </table>
 *
 * <table>
 * <caption id="multi_row">QSFP module 2 I/O Table</caption>
 * <tr><th>Pin number <th>Pin             <th>Direction
 * </tr>
 * <tr>
 * <td> 20 </td>
 * <td> LPMode  </td>
 * <td> Output </td>
 * </tr>
 * <tr>
 * <td> 29 </td>
 * <td> ModPrSl  </td>
 * <td> Input </td>
 * </tr>
 * <tr>
 * <td> 17 </td>
 * <td> ResetL  </td>
 * <td> Output </td>
 * </tr>
 * <tr>
 * <td> 16 </td>
 * <td> IntL  </td>
 * <td> Input </td>
 * </tr>
 * </table>
 *
 * <table>
 * <caption id="multi_row">SFP module 4 I/O Table</caption>
 * <tr><th>Pin number <th>Pin             <th>Direction
 * </tr>
 * <tr>
 * <td> 30 </td>
 * <td> Rx_LOS  </td>
 * <td> Input </td>
 * </tr>
 * <tr>
 * <td> 22 </td>
 * <td> MOD_ABS  </td>
 * <td> Input </td>
 * </tr>
 * <tr>
 * <td> 2 </td>
 * <td> TX_DISABLE  </td>
 * <td> Output </td>
 * </tr>
 * <tr>
 * <td> 3 </td>
 * <td> TX_FAULT  </td>
 * <td> Input </td>
 * </tr>
 * </table>
 *
 * <table>
 * <caption id="multi_row">SFP module 5 I/O Table</caption>
 * <tr><th>Pin number <th>Pin             <th>Direction
 * </tr>
 * <tr>
 * <td> 31 </td>
 * <td> Rx_LOS  </td>
 * <td> Input </td>
 * </tr>
 * <tr>
 * <td> 23 </td>
 * <td> MOD_ABS  </td>
 * <td> Input </td>
 * </tr>
 * <tr>
 * <td> 4 </td>
 * <td> TX_DISABLE  </td>
 * <td> Output </td>
 * </tr>
 * <tr>
 * <td> 5 </td>
 * <td> TX_FAULT  </td>
 * <td> Input </td>
 * </tr>
 * </table>
 *
 * <table>
 * <caption id="multi_row">SFP module 6 I/O Table</caption>
 * <tr><th>Pin number <th>Pin             <th>Direction
 * </tr>
 * <tr>
 * <td> 32 </td>
 * <td> Rx_LOS  </td>
 * <td> Input </td>
 * </tr>
 * <tr>
 * <td> 24 </td>
 * <td> MOD_ABS  </td>
 * <td> Input </td>
 * </tr>
 * <tr>
 * <td> 6 </td>
 * <td> TX_DISABLE  </td>
 * <td> Output </td>
 * </tr>
 * <tr>
 * <td> 7 </td>
 * <td> TX_FAULT  </td>
 * <td> Input </td>
 * </tr>
 * </table>
 *
 * <table>
 * <caption id="multi_row">SFP module 7 I/O Table</caption>
 * <tr><th>Pin number <th>Pin             <th>Direction
 * </tr>
 * <tr>
 * <td> 33 </td>
 * <td> Rx_LOS  </td>
 * <td> Input </td>
 * </tr>
 * <tr>
 * <td> 25 </td>
 * <td> MOD_ABS  </td>
 * <td> Input </td>
 * </tr>
 * <tr>
 * <td> 8 </td>
 * <td> TX_DISABLE  </td>
 * <td> Output </td>
 * </tr>
 * <tr>
 * <td> 9 </td>
 * <td> TX_FAULT  </td>
 * <td> Input </td>
 * </tr>
 * </table>
 *
 * <table>
 * <caption id="multi_row">SFP module 8 I/O Table</caption>
 * <tr><th>Pin number <th>Pin             <th>Direction
 * </tr>
 * <tr>
 * <td> 34 </td>
 * <td> Rx_LOS  </td>
 * <td> Input </td>
 * </tr>
 * <tr>
 * <td> 26 </td>
 * <td> MOD_ABS  </td>
 * <td> Input </td>
 * </tr>
 * <tr>
 * <td> 10 </td>
 * <td> TX_DISABLE  </td>
 * <td> Output </td>
 * </tr>
 * <tr>
 * <td> 11 </td>
 * <td> TX_FAULT  </td>
 * <td> Input </td>
 * </tr>
 * </table>
 *
 * <table>
 * <caption id="multi_row">SFP module 9 I/O Table</caption>
 * <tr><th>Pin number <th>Pin             <th>Direction
 * </tr>
 * <tr>
 * <td> 35 </td>
 * <td> Rx_LOS  </td>
 * <td> Input </td>
 * </tr>
 * <tr>
 * <td> 27 </td>
 * <td> MOD_ABS  </td>
 * <td> Input </td>
 * </tr>
 * <tr>
 * <td> 12 </td>
 * <td> TX_DISABLE  </td>
 * <td> Output </td>
 * </tr>
 * <tr>
 * <td> 13 </td>
 * <td> TX_FAULT  </td>
 * <td> Input </td>
 * </tr>
 * </table>
 *
 * <table>
 * <caption id="multi_row">SFP module 10 I/O Table</caption>
 * <tr><th>Pin number <th>Pin             <th>Direction
 * </tr>
 * <tr>
 * <td> 36 </td>
 * <td> Rx_LOS  </td>
 * <td> Input </td>
 * </tr>
 * <tr>
 * <td> 28 </td>
 * <td> MOD_ABS  </td>
 * <td> Input </td>
 * </tr>
 * <tr>
 * <td> 14 </td>
 * <td> TX_DISABLE  </td>
 * <td> Output </td>
 * </tr>
 * <tr>
 * <td> 15 </td>
 * <td> TX_FAULT  </td>
 * <td> Input </td>
 * </tr>
 * </table>
 *
 * <table>
 * <caption id="multi_row">SFP module 11 I/O Table</caption>
 * <tr><th>Pin number <th>Pin             <th>Direction
 * </tr>
 * <tr>
 * <td> 37 </td>
 * <td> Rx_LOS  </td>
 * <td> Input </td>
 * </tr>
 * <tr>
 * <td> 29 </td>
 * <td> MOD_ABS  </td>
 * <td> Input </td>
 * </tr>
 * <tr>
 * <td> 16 </td>
 * <td> TX_DISABLE  </td>
 * <td> Output </td>
 * </tr>
 * <tr>
 * <td> 17 </td>
 * <td> TX_FAULT  </td>
 * <td> Input </td>
 * </tr>
 * </table>
 *
 *
 *  @return PHYMOD_E_NONE (0) for success and corresponding error code on failure
 */
int dino_phy_gpio_config_set(const phymod_phy_access_t* phy, int pin_no, phymod_gpio_mode_t gpio_mode)
{
    return _dino_phy_gpio_config_set(&phy->access, pin_no, gpio_mode);
}

int dino_phy_gpio_config_get(const phymod_phy_access_t* phy, int pin_no, phymod_gpio_mode_t* gpio_mode)
{
    return _dino_phy_gpio_config_get(&phy->access, pin_no, gpio_mode);
}

int dino_phy_gpio_pin_value_set(const phymod_phy_access_t* phy, int pin_no, int value)
{
    return _dino_phy_gpio_pin_value_set(&phy->access, pin_no, value);
}

int dino_phy_gpio_pin_value_get(const phymod_phy_access_t* phy, int pin_no, int* value)
{
    return _dino_phy_gpio_pin_value_get(&phy->access, pin_no, value);
}

int dino_phy_multi_get(const phymod_phy_access_t* phy, phymod_multi_data_t* multi_data)
{
    return _dino_phy_multi_get(&phy->access, multi_data);
}

int dino_phy_op_mode_get(const phymod_phy_access_t* phy, phymod_operation_mode_t* op_mode)
{
    return PHYMOD_E_UNAVAIL;
}
