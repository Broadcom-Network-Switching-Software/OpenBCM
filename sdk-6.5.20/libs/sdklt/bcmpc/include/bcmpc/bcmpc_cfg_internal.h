/*! \file bcmpc_cfg_internal.h
 *
 * BCMPC bulk configuration Internal Functions.
 *
 * Declaration of the structures, enumerations, and functions to implement
 * the Port Control (BCMPC) module.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMPC_CFG_INTERNAL_H
#define BCMPC_CFG_INTERNAL_H

#include <shr/shr_bitop.h>
#include <bcmpc/bcmpc_types.h>
#include <bcmpc/bcmpc_lport_internal.h>
#include <bcmpc/bcmpc_pm_internal.h>
#include <bcmpc/bcmpc_pm_drv_types.h>

/*!
 * In the below data strcture, each element is
 * a bitmap associated with fields of individual
 * PC LT's.
 * This bitmap is used to store which PC LT fields
 * have been configured by the user in the config
 * and is read during the pre_config stage of the
 * PC system manager initialization.
 * This is required to be stored because during bulk
 * configuration, the actual user only configured
 * information is not present.
 */
typedef struct bcmpc_lt_user_cfg_info_s {

    /*! Bitmap of PC_PORTt fields. */
    SHR_BITDCLNAME(lt_port_fbmp, PC_PORT_FIELD_ID_MAX);

    /*! Bitmap of PC_MAC_CONTROLt fields. */
    SHR_BITDCLNAME(lt_mac_ctrl_fbmp, PC_MAC_CTRL_FIELD_ID_MAX);

    /*! Bitmap of PC_PORT_TIMESYNCt fields. */
    SHR_BITDCLNAME(lt_port_timesync_fbmp, BCMPC_TIMESYNC_LT_FLD_MAX);

    /*! Bitmap of PC_TX_TAPSt fields. */
    SHR_BITDCLNAME(lt_tx_taps_fbmp, PC_TX_TAPS_FIELD_ID_MAX);

    /*! Bitmap of PC_PMD_FIRMWAREt fields. */
    SHR_BITDCLNAME(lt_pmd_firmware_fbmp, PC_PMD_FIRMWARE_FIELD_ID_MAX);

    /*! Bitmap of PC_PHY_CONTROLt fields. */
    SHR_BITDCLNAME(lt_phy_control_fbmp, PC_PHY_CTRL_FIELD_ID_MAX);

    /*! Bitmap of PC_PFCt fields. */
    SHR_BITDCLNAME(lt_pfc_fbmp, PC_PFC_FIELD_ID_MAX);

} bcmpc_lt_user_cfg_info_t;

/*!
 * This data structure is used to store and validate the PC component
 * LT configuration. This is primarlly used during the system initialization
 * in "RUN" stage. The data structure is populated on a per Port Macro basis.
 */
typedef struct bcmpc_bulk_cfg_info_s {

    /*! Flag to indicate the config file validation fail. */
    bool              config_error;

    /*! Temporary buffer for storing the logical to physical port map. */
    bcmpc_lport_t     *l2p_buf;

    /*! Temporary buffer for storing the physical to physical port map. */
    bcmpc_pport_t     *p2l_buf;

    /*! Temporary buffer for storing the logical port validation status. */
    bool              *lport_valid;

    /*! Temporary buffer for storing the number of logcial ports configured. */
    uint32_t          num_lports;

    /*! List of port configuration as populated in PC_PORT. */
    bcmpc_port_cfg_t  **pcfg;

    /*! List of speed configuration information of all logical ports. */
    pm_speed_config_t *speed_cfg;

}bcmpc_bulk_cfg_info_t;

/*!
 * \brief Enable/Disable the PC per-port or bulk configuration mode.
 *
 * Enable to set PC port-based configuration mode.
 *
 * \param [in] unit Unit number.
 * \param [in] enable 1 to enable the port-based configuration.
 *                    0 to disable the port-based configuration,
 *                      indicates bulk port configuration.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_UNIT Unit not found.
 */
extern int
bcmpc_pc_port_based_config_set(int unit, bool enable);

/*!
 * \brief Get the current PC port configuration mode.
 *
 * Get the enable state of the port-based PC
 * configuration mode.
 *
 * \param [in] unit Unit number.
 * \param [out] enable 1 if enable port-based configuration.
 *                     0 if bulk port configuration mode.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_UNIT Unit not found.
 */
extern int
bcmpc_pc_port_based_config_get(int unit, bool *enable);

/*!
 * \brief Apply PC configuration to the hardware.
 *
 * BCMPC component is in bulk configuration mode during cold boot
 * that is the config file which is read in the pre-config stage
 * is parsed during the System Manager run stage to apply the
 * PC configuration.
 *
 * This function will be invoked during System Manager run stage
 * to traverse all the PC LT's to build the per Port Macro
 * based port configuration and use this configuration to
 * program the hardware. Per-Port Macro based configuration
 * is effective to derive the VCO's rates to be used to bring up
 * the SERDES cores as compared to applying per-port based
 * configuration. The assumption is the config file is already
 * populated by the user with all the required logical port
 * confgiuration. After applying the configuration, the BCMPC
 * component switches to per-port based configuration.
 *
 * PC_PORT_PHYS_MAP and PC_PORT Lt's are fundamental blocks for
 * PC. All the other PC LT's are update-only, that is the tables
 * are pre-populated during port configuration based on the
 * logical port configuration as defined in PC_PORT LT and
 * any user updates to the other LT's are applied on top of these
 * settings.
 *
 * NOTE: If the config file does not have any PC configuration
 * specified, then the PC component will run in per-port based
 * configuration
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_UNIT Unit not found.
 * \retval SHR_E_FAIL Failed to apply the configuration,
 *                    due to config file errors.
 */
extern int
bcmpc_config_apply(int unit);


#endif /* BCMPC_CFG_INTERNAL_H */
