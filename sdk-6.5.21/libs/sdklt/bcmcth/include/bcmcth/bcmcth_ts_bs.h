/*! \file bcmcth_ts_bs.h
 *
 * BCMCTH TS BROADSYNC LT related functions.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMCTH_TS_BS_H
#define BCMCTH_TS_BS_H

#include <bcmlrd/bcmlrd_types.h>
#include <bcmimm/bcmimm_int_comp.h>
#include <bcmcth/bcmcth_ts_bs_uc.h>

#include <shr/shr_thread.h>
#include <bcmcth/generated/ts_bs_ha.h>
#include <bcmbd/mcs_shared/mcs_msg_common.h>
#include <bcmbd/mcs_shared/mcs_bs_common.h>

/*! BS info data structure */
typedef struct bs_info_s {
    /*! BS control information stored in HA memory. */
    bcmcth_ts_bs_control_ha_t *ctrl;

    /*! BS log information stored in HA memory. */
    bcmcth_ts_bs_log_data_ha_t *log_data;

    /*! Event thread control. */
    shr_thread_t log_thread;

    /*! buffer for debug log */
    mcs_bs_log_info_t *log_buf_info;

    /*! Physical address of the log buffer */
    uint64_t log_buf_p_addr;

    /*! DMA buffer for holding send/receive messages */
    uint8_t *dma_buffer;

    /*! Physical address of the DMA buffer */
    uint64_t dma_buffer_p_addr;

    /*! Size of log table */
    uint32_t log_table_size;

    /*! fields for log table */
    bcmltd_fields_t log_fields;

    /*! number of fields in log table */
    size_t log_num_fields;

    /*! fields for log status table */
    bcmltd_fields_t log_status_fields;
} bcmcth_ts_bs_info_t;

/*! Structure to register callbacks for the BS IMM backed LTs. */
typedef struct bcmcth_ts_bs_imm_reg_s {
    /*! LT ID. */
    bcmltd_sid_t sid;

    /*!
     * Prepopulate function. Used to insert dummy entries for update_only
     * tables. Will be called prior to event registration if not NULL.
     */
    int (*prepopulate)(int unit, bcmltd_sid_t sid);

    /*! IMM event callback handler. */
    bcmimm_lt_cb_t *cb;
} bcmcth_ts_bs_imm_reg_t;

/*!
 * \brief Register BS IMM handlers.
 *
 * Register the IMM handlers required by the DEVICE_TS_BROADSYNC_XXX LTs.
 *
 * \param [in] unit Unit number.
 * \param [in] warm Warmboot indicator.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmcth_ts_bs_imm_register(int unit, bool warm);

/*!
 * \brief Register IMM handlers for global BS LTs.
 *
 * \param [in] unit Unit number.
 * \param [in] warm Warmboot indicator.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmcth_ts_bs_control_imm_register(int unit, bool warm);

/*!
 * \brief Register IMM handlers for BS interface LT.
 *
 * \param [in] unit Unit number.
 * \param [in] warm Warmboot indicator.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmcth_ts_bs_interface_imm_register(int unit, bool warm);

/*!
 * \brief Register callbacks for IMM events.
 *
 * Register callbacks for IMM events for LTs specified in \c imm_reg.
 *
 * \param [in] unit Unit number.
 * \param [in] warm Warmboot indicator.
 * \param [in] num_sids Number of IMM backed LTs to be registered.
 * \param [in] imm_reg Information about the LTs that need to be registed.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmcth_ts_bs_imm_cb_register(int unit, bool warm, int num_sids,
                               bcmcth_ts_bs_imm_reg_t *imm_reg);

/*!
 * \brief Free the memory allocated to hold LT fields.
 *
 * \param [in] unit Unit number.
 * \param [in] num_fields Number of LT fields.
 * \param [in] fields Array of fields that need to be freed.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmcth_ts_bs_fields_free(int unit,
                           size_t num_fields,
                           bcmltd_fields_t *fields);

/*!
 * \brief Allocate memory to hold \c num_fields number of LT fields.
 *
 * \param [in] unit Unit number.
 * \param [in] num_fields Number of LT fields.
 * \param [out] fields Array of fields with memory allocated to each of them.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmcth_ts_bs_fields_alloc(int unit, size_t num_fields,
                            bcmltd_fields_t *fields);

/*!
 * \brief Initialize BS.
 *
 * \param [in] unit Unit number.
 * \param [in] warm Warmboot indicator.
 *
 * \retval SHR_E_NONE No errors. Error code otherwise.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmcth_ts_bs_init(int unit, bool warm);

/*!
 * \brief De-initialize BS.
 *
 * \param [in] unit Unit number.
 *
 * \returns None
 */
extern int
bcmcth_ts_bs_deinit(int unit);

#endif /* BCMCTH_TS_BS_H */
