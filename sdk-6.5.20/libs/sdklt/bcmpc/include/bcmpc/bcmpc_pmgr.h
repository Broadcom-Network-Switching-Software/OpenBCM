/*! \file bcmpc_pmgr.h
 *
 * BCMPC Port Manager (PMGR) APIs.
 *
 * Declaration of the structures, enumerations, and functions to implement
 * the Port Control (BCMPC) module.
 *
 * The Port Manager is responsible for updating the hardware based on the
 * changes to the logical tables. The Port Manager does not write to hardware
 * directly, but relies on the PMGR drivers.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMPC_PMGR_H
#define BCMPC_PMGR_H

#include <bcmpc/bcmpc_types.h>
#include <bcmpc/bcmpc_fdr.h>

/*!
 * \brief Enumeration of vco rate.
 */
typedef enum bcmpc_vco_type_e {

    /*! No pll configured. */
    BCMPC_VCO_RATE_NONE,

    /*! vco rate is 10.3125G. */
    BCMPC_VCO_RATE_10P3125G,

    /*! vco rate is 12.5G. */
    BCMPC_VCO_RATE_12P5G,

    /*! vco rate is 20.625G. */
    BCMPC_VCO_RATE_20P625G,

    /*! vco rate is 25.781G. */
    BCMPC_VCO_RATE_25P781G,

    /*! vco rate is 26.562G. */
    BCMPC_VCO_RATE_26P562G,

    /*! vco rate is 41P25G. */
    BCMPC_VCO_RATE_41P25G,

    /*! vco rate is 51P562G. */
    BCMPC_VCO_RATE_51P562G,

    /*! vco rate is 53P125G. */
    BCMPC_VCO_RATE_53P125G

} bcmpc_vco_type_t;

/*! \brief Globally available port TX timestamp information structure.  */
typedef struct bcmpc_pmgr_port_timestamp_s {
    /*! Low 32-bits of Timestamp in FIFO. */
    uint32_t timestamp_lo;

    /*! High 32-bits of timestamp in FIFO. */
    uint32_t timestamp_hi;

    /*! Sequence ID of sent ptp packet. */
    uint32_t sequence_id;

    /*! Sub-nanosecond of 1588 transmit packet. */
    uint32_t timestamp_sub_nanosec;
} bcmpc_pmgr_port_timestamp_t;

/*! Egress timestamping mode enumerations. */
typedef enum bcmpc_pmgr_egr_timestamp_mode_s {
    /*! 32-bit egress 1588 timestamp. */
    BCMPC_PMGR_32_BIT_EGRESS_TIMESTAMP = 0,

    /*! 48-bit egress 1588 timestamp. */
    BCMPC_PMGR_48_BIT_EGRESS_TIMESTAMP = 1
} bcmpc_pmgr_egr_timestamp_mode_t;

/*!
 * \brief Get the link state of the given physical port.
 *
 * This API is intended for use by software linkscan.
 *
 * It will return a completed link state which consider both of PHY link state
 * and MAC fault state.
 *
 * When a port is set to MAC/PHY local loopback mode, the link state will be
 * focred "up".
 *
 * \param [in] unit Unit number.
 * \param [in] pport Device physical port.
 * \param [out] link Link state. 1 for link up and 0 for down.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmpc_pmgr_link_state_get(int unit, bcmpc_pport_t pport, int *link);

/*!
 * \brief Get the link fault state of the given physical port.
 *
 * This API is intended for use by software linkscan.
 *
 * It only returns the remote fault state for now, but it might be revised to
 * return both of local and remote fault state if required.
 *
 * \param [in] unit Unit number.
 * \param [in] pport Device physical port.
 * \param [out] rmt_fault Remote fault state.
 *                        1 when remote fault is detected, otherwise 0.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmpc_pmgr_fault_state_get(int unit, bcmpc_pport_t pport, int *rmt_fault);

/*!
 * \brief Execute the link change process per the link state.
 *
 * \param [in] unit Unit number.
 * \param [in] pport Device physical port.
 * \param [in] link Link state.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmpc_pmgr_port_link_change(int unit, bcmpc_pport_t pport, int link);

/*!
 * \brief Get port TX packet timestamp from FIFO.
 *
 * \param [in] unit Unit number.
 * \param [in] lport device logical port.
 * \param [out] timestamp_info Packet timestamp data structure.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_EMPTY No timestamps in FIFO.
 * \retval SHR_E_FAIL Failure.
 * \retval SHR_E_UNVAL This function is not supported by the driver instance.
 */
extern int
bcmpc_pmgr_port_tx_timestamp_info_get(int unit, bcmpc_lport_t lport,
                                 bcmpc_pmgr_port_timestamp_t *timestamp_info);

/*!
 * \brief Get egress 1588 timestamp mode.
 *
 * \param [in] unit Unit number.
 * \param [out] timestamp_mode Timestamp mode enumeration
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Invalid input parameter.
 * \retval SHR_E_ACCESS Failed to access the register.
 */
extern int
bcmpc_pmgr_egress_timestamp_mode_get(int unit,
                             bcmpc_pmgr_egr_timestamp_mode_t *timestamp_mode);

/*!
 * \brief Set egress 1588 timestamp mode.
 *
 * \param [in] unit Unit number.
 * \param [in] timestamp_mode Timestamp mode enumeration
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Invalid input parameter.
 * \retval SHR_E_ACCESS Failed to access the register.
 */
extern int
bcmpc_pmgr_egress_timestamp_mode_set(int unit,
                             bcmpc_pmgr_egr_timestamp_mode_t timestamp_mode);

/*!
 * \brief Logical port FDR control set function.
 *
 * \param [in] unit Unit number.
 * \param [in] lport Logical port number.
 * \param [in] pport Physical port number.
 * \param [in] fdr_ctrl FDR control configuration information.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 */
extern int
bcmpc_pmgr_fdr_control_set(int unit, bcmpc_lport_t lport, bcmpc_pport_t pport,
                           bcmpc_fdr_port_control_t *fdr_ctrl);

/*!
 * \brief Logical port FDR statistics get funtion.
 *
 * \param [in] unit Unit number
 * \param [in] lport Logical port number.
 * \param [in] pport Physical port number.
 * \param [in] fdr_stats FDR statistics information.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 */
extern int
bcmpc_pmgr_fdr_stats_get(int unit, bcmpc_lport_t lport, bcmpc_pport_t pport,
                         bcmpc_fdr_port_stats_t *fdr_stats);

#endif /* BCMPC_PMGR_H */
