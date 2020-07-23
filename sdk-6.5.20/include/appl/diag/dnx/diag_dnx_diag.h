
/*! \file diag_dnx_diag.h
 * Purpose: Extern declarations for command functions and
 *          their associated usage strings.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef DIAG_DNX_DIAG_H_INCLUDED
#define DIAG_DNX_DIAG_H_INCLUDED

/*************
 * INCLUDES  *
 */
#include <soc/dnx/dnx_data/auto_generated/dnx_data_device.h>
/*************
 *  MACROES  *
 */
#define DNX_DIAG_COUNTER_REG_NOF_REGS (4)
/** Length of graphical print buffer */
#define DNX_DIAG_GRAPHICAL_PRINT_BUFFER_LENGTH 50
/** Length of block name */
#define DNX_DIAG_BLOCK_NAME_LENGTH 8

/*************
 *  DEFINES  *
 */
typedef enum
{
    /** block display graphic on left side */
    dnx_graphical_nif_rx = 0,
    dnx_graphical_ire,
    dnx_graphical_spb,
    dnx_graphical_ddp,
    dnx_graphical_cgm,
    dnx_graphical_ipt,
    dnx_graphical_fdt,
    /** block display graphic on right side */
    dnx_graphical_nif_tx,
    dnx_graphical_esb,
    dnx_graphical_epni,
    dnx_graphical_pqp,
    dnx_graphical_rqp,
    dnx_graphical_fda,
    dnx_graphical_fdr,
    /** must be at last */
    dnx_graphical_block_count
} dnx_diag_graphical_block;
typedef struct reason_buffer_s
{
    char buffer[DNX_DIAG_GRAPHICAL_PRINT_BUFFER_LENGTH];
} reason_buffer_t;
typedef struct dnx_counter_packet_dropped_reason_s
{
    int nof_reason;
    char block_name[DNX_DIAG_BLOCK_NAME_LENGTH];
    reason_buffer_t *reason_buffer;
} dnx_counter_packet_dropped_reason_t;

/**
 * \brief
 * Callback to get drop reason.
 */
typedef int (
    *dnx_dropped_reason_get_f) (
    int unit,
    soc_reg_t reg,
    soc_field_t field,
    int cdsp,
    dnx_counter_packet_dropped_reason_t * dropped_reason);

/*************
 * GLOBALS   *
 */

/*************
 * FUNCTIONS *
 */
/**
 * \brief
 *   For DNX device only
 *   get CGM drop reason string according to register/field
 * \param [in] unit - Unit #
 * \param [in] reg - error counter register
 * \param [in] field - error counter field
 * \param [in] cdsp - error counter per core
 * \param [out] reason - drop reason string
 * \return
 *      Standard error handling
 */
shr_error_e dnx_cgm_drop_reason_to_str(
    int unit,
    soc_reg_t reg,
    soc_field_t field,
    int cdsp,
    dnx_counter_packet_dropped_reason_t * reason);

/**
 * \brief
 *   For DNX device only
 *   get RQP drop reason string according to register/field
 * \param [in] unit - Unit #
 * \param [in] reg - error counter register
 * \param [in] field - error counter field
 * \param [in] cdsp - error counter per core
 * \param [out] reason - drop reason string
 * \return
 *      Standard error handling
 */
shr_error_e dnx_rqp_drop_reason_to_str(
    int unit,
    soc_reg_t reg,
    soc_field_t field,
    int cdsp,
    dnx_counter_packet_dropped_reason_t * reason);

/**
 * \brief
 *   For DNX device only
 *   get PQP drop reason string according to register/field
 * \param [in] unit - Unit #
 * \param [in] reg - error counter register
 * \param [in] field - error counter field
 * \param [in] cdsp - error counter per core
 * \param [out] reason - drop reason string
 * \return
 *      Standard error handling
 */
shr_error_e dnx_pqp_drop_reason_to_str(
    int unit,
    soc_reg_t reg,
    soc_field_t field,
    int cdsp,
    dnx_counter_packet_dropped_reason_t * reason);

/**
 * \brief
 *   For DNX device only
 *   get ERPP drop reason string according to register/field
 * \param [in] unit - Unit #
 * \param [in] reg - error counter register
 * \param [in] field - error counter field
 * \param [in] cdsp - error counter per core
 * \param [out] reason - drop reason string
 * \return
 *      Standard error handling
 */
shr_error_e dnx_erpp_drop_reason_to_str(
    int unit,
    soc_reg_t reg,
    soc_field_t field,
    int cdsp,
    dnx_counter_packet_dropped_reason_t * reason);

/**
 * \brief
 *   For DNX device only
 *   get counter value according to block/counter
 * \param [in] unit - Unit #
 * \param [in] name - block name belong to counter register
 * \param [in] counterName - counter register name
 * \param [out] counter - counter value
 * \return
 *      Standard error handling
 */
shr_error_e dnx_diag_get_counter_specific(
    int unit,
    const char *name,
    const char *counterName,
    uint64 *counter);

#endif /* DIAG_DNX_DIAG_H_INCLUDED */
