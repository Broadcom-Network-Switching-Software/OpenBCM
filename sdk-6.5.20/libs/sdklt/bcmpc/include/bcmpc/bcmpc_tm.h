/*! \file bcmpc_tm.h
 *
 * BCMPC TM (Traffic Manager) APIs.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMPC_TM_H
#define BCMPC_TM_H

#include <sal/sal_types.h>
#include <bcmpc/bcmpc_types_internal.h>

/*!
 * \name MMU port configuration flags.
 * \anchor BCMPC_MMU_F_xxx
 */

/*! \{ */

/*! Port is configured for Cell mode. */
#define BCMPC_MMU_F_CELL         (0)

/*! Port is configured for HiGig2 mode. */
#define BCMPC_MMU_F_HG2         (1 << 0)

/*! Port is configured for extended queuing. */
#define BCMPC_MMU_F_EXTQ        (1 << 1)

/*! Port is configured for over-subscription. */
#define BCMPC_MMU_F_OVSUB       (1 << 2)

/*! Port is management port */
#define BCMPC_MMU_F_MGMT        (1 << 3)

/*! Port is configured for HiGig3 encapsulation mode. */
#define BCMPC_MMU_F_HG3         (1 << 4)

/*! Port is configured for IEEE encapsulation mode. */
#define BCMPC_MMU_F_IEEE         (1 << 5)

/*! \} */

/*!
 * \brief BCMPC MMU port configuration.
 *
 * The information contained in this structure is provided for all
 * physical ports of a device whether they are in use or not.
 */
typedef struct bcmpc_mmu_port_cfg_s {

    /*! Maximum configured port speed (zero means the port is unused). */
    uint32_t speed_max;

    /*! Current port speed (set to zero if link down). */
    uint32_t speed_cur;

    /*! Port mode flags (see \ref BCMPC_MMU_F_xxx). */
    uint32_t flags;

    /*! Number of lanes in use in the port block. */
    int num_lanes;

    /*! Logical port number associated with this port. */
    bcmpc_lport_t lport;

    /*! Physical port number associated with this port. */
    bcmpc_pport_t pport;

} bcmpc_mmu_port_cfg_t;

/*!
 * \brief MMU update function pointer type.
 *
 * This function is called after each port configuration change to allow the
 * MMU component to reconfigure the MMU resources.
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
(*bcmpc_mmu_update_f)(int unit, size_t num_ports,
                      const bcmpc_mmu_port_cfg_t *old_cfg,
                      const bcmpc_mmu_port_cfg_t *new_cfg);

/*!
 * \brief TM port operation function pointer type.
 *
 * This function type is used to inform TM for the TM related operations in the
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
(*bcmpc_tm_port_op_exec_f)(int unit, bcmpc_pport_t pport,
                           char *op_name, uint32_t op_param);

/*!
 * \brief TM handler.
 *
 * This structure is the interface between PC and TM.
 *
 * The TM module need to register the handler functions in PC for each unit by
 * \ref bcmpc_tm_handler_register. And PC will callback to TM to do the
 * corresponding update.
 */
typedef struct bcmpc_tm_handler_s {

    /*! Validate port configuration with respect to TM resources. */
    bcmpc_mmu_update_f tm_validate;

    /*! Configure TM component. */
    bcmpc_mmu_update_f tm_configure;

    /*! Execute TM port operation. */
    bcmpc_tm_port_op_exec_f port_op_exec;

} bcmpc_tm_handler_t;


/*******************************************************************************
 * BCMPC TM functions
 */

/*!
 * \brief Register TM handler.
 *
 * Register the TM handler \c hdl for the given \c unit.
 *
 * \param [in] unit Unit number.
 * \param [in] hdl TM handler.
 *
 * \retval SHR_E_NONE No errors
 * \retval SHR_E_FAIL Fail to register the handler.
 */
extern int
bcmpc_tm_handler_register(int unit, bcmpc_tm_handler_t *hdl);

/*!
 * \brief Configure TM update callback mode.
 *
 * This function is a debug interface for testing the Traffic manager
 * (TM) callback interface in the Port Control component.
 *
 * Normally the TM callbacks will happen automatically during the port
 * configuration process, but this functionality can be disabled for
 * debugging purposes.
 *
 * \param [in] unit Unit number.
 * \param [in] manual Use manual MMU callback mode.
 *
 * \retval SHR_E_NONE Callback mode updated successfully.
 * \retval SHR_E_UNIT Unit not found.
 */
extern int
bcmpc_tm_manual_update_mode_set(int unit, bool manual);

/*!
 * \brief Get the current TM update callback mode.
 *
 * This function is a debug interface for testing the Traffic manager
 * (TM) callback interface in the Port Control component.
 *
 * \param [in] unit Unit number.
 * \param [out] manual Set if using manual MMU callback mode.
 *
 * \retval SHR_E_NONE Callback mode updated successfully.
 * \retval SHR_E_UNIT Unit not found.
 */
extern int
bcmpc_tm_manual_update_mode_get(int unit, bool *manual);

/*!
 * \brief Configure and/or initiate TM update callback.
 *
 * This function is a debug interface for testing the Traffic manager
 * (TM) callback interface in the Port Control component.
 *
 * Whether the callback is operating in normal (auto) mode or manual
 * mode, this API allows for an on-demand update of the MMU settings
 * according to the current port configuration.
 *
 * See also \ref bcmpc_tm_manual_update_mode_set.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE Callback mode updated successfully.
 * \retval SHR_E_UNIT Unit not found.
 */
extern int
bcmpc_tm_update_now(int unit);

#endif /* BCMPC_TM_H */
