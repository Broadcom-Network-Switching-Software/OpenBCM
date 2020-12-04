/*! \file bcmpc_fdr.h
 *
 * BCMPC Logical Port FEC Flight Data Recorder (FDR) APIs.
 *
 * Declaration of the structures, enumerations, and functions to implement
 * the Port Control (BCMPC) module.
 *
 * The FEC FDR library provides the functions to enable, control and retrieve
 * FDR statistics on a logical port.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMPC_FDR_H
#define BCMPC_FDR_H

#include <shr/shr_bitop.h>
#include <bcmpc/bcmpc_lt_cfg.h>
#include <bcmpc/bcmpc_types.h>

/*!
 * \brief PC_RSFEC_FLIGHT_DATA_RECORDER LT fields information.
 */
typedef enum bcmpc_fdr_lt_fields_e {
    BCMPC_FDR_CTRL_LT_FLD_ENABLE = 0,
    BCMPC_FDR_CTRL_LT_FLD_SYMERR_WINDOW_SIZE,
    BCMPC_FDR_CTRL_LT_FLD_SYMERR_THRESHOLD,
    BCMPC_FDR_CTRL_LT_FLD_SYMERR_START_VALUE,
    BCMPC_FDR_CTRL_LT_FLD_FDR_INTR_ENABLE
} bcmpc_fdr_lt_fields_t;

/*! Maximum field ID of PC_FDR_CTRL LTs. */
#define PC_FDR_CTRL_FIELD_ID_MAX 5

/*!
 * \brief Enumerations for Symbol Error Window sizes.
 */
typedef enum bcmpc_fdr_symbol_err_window_sz_s {

    /*! Window size of 128 code words. */
    BCMPC_SYMBOL_ERROR_WINDOW_CW_128 = 0,

    /*! Window size of 1K code words. */
    BCMPC_SYMBOL_ERROR_WINDOW_CW_1K,

    /*! Window size of 8K code words. */
    BCMPC_SYMBOL_ERROR_WINDOW_CW_8K,

    /*! Window size of 64K code words. */
    BCMPC_SYMBOL_ERROR_WINDOW_CW_64K,

    /*! Window size of 512K code words. */
    BCMPC_SYMBOL_ERROR_WINDOW_CW_512K,

    /*! Window size of 4M code words. */
    BCMPC_SYMBOL_ERROR_WINDOW_CW_4M,

    /*! Window size of 32M code words. */
    BCMPC_SYMBOL_ERROR_WINDOW_CW_32M,

    /*! Window size of 256M code words. */
    BCMPC_SYMBOL_ERROR_WINDOW_CW_256M

} bcmpc_fdr_symbol_err_window_sz_t;

/*!
 * \brief Flight Data Recorder (FDR) configuration structure.
 *
 * This data structure defines the attributes for FDR
 * configuration on a port.
 */
typedef struct bcmpc_fdr_port_control_s {

    /*! Bitmap of fields to be operated */
    SHR_BITDCLNAME(fbmp, PC_FDR_CTRL_FIELD_ID_MAX);

    /*! Device logical port. */
    bcmpc_lport_t lport;

    /*! Enable FDR. */
    bool enable;

    /*! Symbol Error Window (SEW) size. */
    bcmpc_fdr_symbol_err_window_sz_t window_size;

    /*!
     * Indicates a threshold value that triggers a notification to
     * the application, if it is achieved in a non-overlapping window
     * as specified in SYMBOL_ERROR_WINDOW.
     */
    uint32_t symbol_error_threshold;

    /*!
     * Set to indicate that the number of symbol error start value is
     * 9, else it indicates the symbol error start value 0.
     */
    bool symbol_error_start_value;

    /*!
     * Specify to enable Flight Data Recorder (FDR) interrupt.
     * When enabled, if symbol errors cross the value set in
     * symbol_error_threshold, interrupt triggers. On interrupt
     * trigger, FDR statistics are updated.
     */
    bool interrupt_enable;

    /*! Operational state of the PC_FDR_CONTROL entry. */
    bcmpc_entry_state_t oper_state;

} bcmpc_fdr_port_control_t;

/*!
 * \brief Flight Data Recorder (FDR) statiistics structure.
 *
 * This data structure defines the attributes for FDR
 * statistics on a port.
 */
typedef struct bcmpc_fdr_port_stats_s {

    /*!
     * Indicates the time when the Flight Data Recorder engine
     * starts to collect data. The start time indicates either
     * the time when FDR is enabled or the time when the
     * application reads the statistics.
     */
    uint64_t start_time;

    /*!
     * Indicates the time when the last statistics are collected
     * which are used for the statistics accumulation.
     */
    uint64_t end_time;

    /*!
     * Indicates the total number of uncorrectable code words.
     */
    uint32_t num_uncorr_cws;

    /*!
     * Total number of code words used for the statistics
     * collection.
     */
    uint32_t num_cws;

    /*!  Total number of symbol errors detected. */
    uint32_t num_symerrs;

    /*!
     * Number of Code Words (CW) with S + 0 Errors where
     * S is specified in SYMBOL_ERROR_START_VALUE field in
     * PC_FDR_CONTROL.
     */
    uint32_t s0_errs;

    /*!
     * Number of Code Words (CW) with S + 1 Errors where
     * S is specified in SYMBOL_ERROR_START_VALUE field in
     * PC_FDR_CONTROL.
     */
    uint32_t s1_errs;

    /*!
     * Number of Code Words (CW) with S + 2 Errors where
     * S is specified in SYMBOL_ERROR_START_VALUE field in
     * PC_FDR_CONTROL.
     */
    uint32_t s2_errs;


    /*!
     * Number of Code Words (CW) with S + 3 Errors where
     * S is specified in SYMBOL_ERROR_START_VALUE field in
     * PC_FDR_CONTROL.
     */
    uint32_t s3_errs;

    /*!
     * Number of Code Words (CW) with S + 4 Errors where
     * S is specified in SYMBOL_ERROR_START_VALUE field in
     * PC_FDR_CONTROL.
     */
    uint32_t s4_errs;

    /*!
     * Number of Code Words (CW) with S + 5 Errors where
     * S is specified in SYMBOL_ERROR_START_VALUE field in
     * PC_FDR_CONTROL.
     */
    uint32_t s5_errs;

    /*!
     * Number of Code Words (CW) with S + 6 Errors where
     * S is specified in SYMBOL_ERROR_START_VALUE field in
     * PC_FDR_CONTROL.
     */
    uint32_t s6_errs;

    /*!
     * Number of Code Words (CW) with S + 7 Errors where
     * S is specified in SYMBOL_ERROR_START_VALUE field in
     * PC_FDR_CONTROL.
     */
    uint32_t s7_errs;


    /*!
     * Number of Code Words (CW) with S + 8 Errors where
     * S is specified in SYMBOL_ERROR_START_VALUE field in
     * PC_FDR_CONTROL.
     */
    uint32_t s8_errs;

    /*! Operational state of the PC_FDR_STATUS entry. */
    bcmpc_entry_state_t oper_state;

} bcmpc_fdr_port_stats_t;

#endif /* BCMPC_FDR_H */
