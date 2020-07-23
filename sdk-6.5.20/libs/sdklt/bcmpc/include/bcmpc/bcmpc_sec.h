/*! \file bcmpc_sec.h
 *
 * BCMPC SEC APIs.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMPC_SEC_H
#define BCMPC_SEC_H

#include <sal/sal_types.h>
#include <bcmpc/bcmpc_types_internal.h>

/*!
 * \brief BCMPC SEC port configuration.
 *
 * The information contained in this structure is provided for all
 * physical ports of a device whether they are in use or not.
 */
typedef struct bcmpc_sec_port_cfg_s {

    /*! Number of lanes in use in the port block. */
    int num_lanes;

    /*! Logical port number associated with this port. */
    bcmpc_lport_t lport;

    /*! Physical port number associated with this port. */
    bcmpc_pport_t pport;

    /*! Current port speed (set to zero if link down). */
    uint32_t speed_cur;

} bcmpc_sec_port_cfg_t;

/*!
 * \brief SEC update function pointer type.
 *
 * This function is called after each port configuration change to allow the
 * SEC component to reconfigure the SEC resources.
 *
 * The configuration arrays are indexed by the physical port number.
 *
 * \param [in] unit Unit number.
 * \param [in] num_ports Number of physical ports in configuration arrays.
 * \param [in] old_cfg Current port configuration.
 * \param [in] new_cfg New/requested port configuration.
 *
 * \return SHR_E_NONE when no errors, otherwise !SHR_E_NONE.
 */
typedef int
(*bcmpc_sec_update_f)(int unit, size_t num_ports,
                      const bcmpc_sec_port_cfg_t *old_cfg,
                      const bcmpc_sec_port_cfg_t *new_cfg);

/*!
 * \brief SEC port operation function pointer type.
 *
 * This function type is used to inform SEC the SEC related operations in the
 * link up/down sequence.
 *
 * \param [in] unit Unit number.
 * \param [in] pport Physical port number.
 * \param [in] op_name Operation name.
 * \param [in] op_param Operation parameter.
 *
 * \return SHR_E_NONE when no errors, otherwise !SHR_E_NONE.
 */
typedef int
(*bcmpc_sec_port_op_exec_f)(int unit, bcmpc_pport_t pport,
                           char *op_name, uint32_t op_param);

/*!
 * \brief SEC get if valid on device.
 *
 * This function type is used to know if SEC is valid on the device.
 *
 * \param [in] unit Unit number.
 *
 * \return SHR_E_NONE when no errors, otherwise !SHR_E_NONE.
 */
typedef int
(*bcmpc_sec_valid_get_f)(int unit);

/*!
 * \brief Is SEC valid on the port macro.
 *
 * This function type is used to know if SEC is valid on the port macro.
 *
 * \param [in] unit Unit number.
 * \param [in] pm_id Port Macro
 *
 * \return SHR_E_NONE when no errors, otherwise !SHR_E_NONE.
 */
typedef int
(*bcmpc_pm_sec_enabled_get_f)(int unit, int pm_id);

/*!
 * \brief Get SEC port number for physical port.
 *
 * This function type is used to get the SEC port number given a physical port.
 *
 * \param [in] unit Unit number.
 *
 * \return SHR_E_NONE when no errors.
 *         SHR_E_PARAM if physical port is not mapped to SEC block.
 */
typedef int
(*bcmpc_sec_port_get_f)(int unit, int pport, int *sec_port);

/*!
 * \brief SEC handler.
 *
 * This structure is the interface between PC and SEC.
 *
 * The SEC module needs to register the handler functions in PC for each unit by
 * \ref bcmpc_sec_handler_register. And PC will callback SEC functions to do the
 * corresponding update.
 */
typedef struct bcmpc_sec_handler_s {

    /*! Validate port configuration with respect to SEC resources. */
    bcmpc_sec_update_f sec_validate;

    /*! Configure SEC component. */
    bcmpc_sec_update_f sec_configure;

    /*! Execute SEC port operation. */
    bcmpc_sec_port_op_exec_f port_op_exec;

    /*! Get SEC valid on device. */
    bcmpc_sec_valid_get_f sec_valid_get;

    /*! Is SEC enabled on the PM. */
    bcmpc_pm_sec_enabled_get_f is_pm_sec_enabled;

    /*! Get SEC port on device. */
    bcmpc_sec_port_get_f sec_port_get;

} bcmpc_sec_handler_t;

/*******************************************************************************
 * BCMPC SEC functions
 */

/*!
 * \brief Register SEC handler.
 *
 * Register the SEC handler \c hdl for the given \c unit.
 *
 * \param [in] unit Unit number.
 * \param [in] hdl SEC handler.
 *
 * \retval SHR_E_NONE No errors
 * \retval SHR_E_FAIL Fail to register the handler.
 */
extern int
bcmpc_sec_handler_register(int unit, bcmpc_sec_handler_t *hdl);

/*!
 * \brief Initialize the port config array.
 *
 * \param [in] num_ports Number of physical ports in configuration array.
 * \param [out] cfg Port configuration array.
 */
void
pc_sec_port_cfg_init(size_t num_ports, bcmpc_sec_port_cfg_t *cfg);

/*!
 * \brief Get the SEC handler
 *
 * \param [in] unit Unit number.
 * \param [out] hdl SEC handler.
 */
void
bcmpc_sec_handler_get(int unit, bcmpc_sec_handler_t **hdl);

#endif /* BCMPC_SEC_H */
