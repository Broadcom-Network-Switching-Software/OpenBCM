/*! \file bcmcth_l2_util.h
 *
 * Header file of BCMCTH L2 utilities.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMCTH_L2_UTIL_H
#define BCMCTH_L2_UTIL_H

#include <bcmltd/bcmltd_types.h>
#include <bcmdrd/bcmdrd_types.h>

/*! Maximum field ID of L2 logical tables. */
#define L2_LT_FIELD_ID_MAX      32

/*! Bits of valid struct members. */
typedef enum l2_learn_control_lt_fields_e {
    LEARN_CONTROL_REPORT = 0,
    LEARN_CONTROL_SLOW_POLL
} l2_learn_control_lt_fields_t;

/*!
 * \brief Structure for L2_LEARN_CONTORL/LEARN_CACHE_CONTROL LT entry.
 */
typedef struct l2_learn_control_info_s {
    /*! Bitmap of Fields to be operated. */
    SHR_BITDCLNAME(fbmp, L2_LT_FIELD_ID_MAX);

    /*! Store LT entry REPORT Field value. */
    bool enable;

    /*! Store LT entry SLOW_POLL Field value. */
    bool slow_poll;

} l2_learn_control_info_t;

/*!
 * \brief Modeled hw mem/reg table read for BCMCTH L2.
 *
 * \param [in] unit Unit number.
 * \param [in] trans_id Transaction identifier.
 * \param [in] lt_id Logical table identifier.
 * \param [in] pt_id Memory table identifier.
 * \param [in] pipe Pipe of memory entry to read.
 * \param [in] index Index of memory entry to read.
 * \param [out] entry_data Returned data value of entry.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcmcth_l2_per_pipe_mreq_read(int unit,
                             uint32_t trans_id,
                             bcmltd_sid_t lt_id,
                             bcmdrd_sid_t pt_id,
                             int pipe,
                             int index,
                             void *entry_data);


/*!
 * \brief Modeled hw mem/reg table write for BCMCTH L2.
 *
 * \param [in] unit Unit number.
 * \param [in] trans_id Transaction identifier.
 * \param [in] lt_id Logical table identifier.
 * \param [in] pt_id Memory table identifier.
 * \param [in] pipe Pipe of memory entry to write.
 * \param [in] index Index of memory entry to write.
 * \param [in] entry_data New data value of entry to set.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmcth_l2_per_pipe_mreq_write(int unit,
                              uint32_t trans_id,
                              bcmltd_sid_t lt_id,
                              bcmdrd_sid_t pt_id,
                              int pipe,
                              int index,
                              void *entry_data);


/*!
 * \brief Interactive hw mem/reg table read for BCMCTH L2.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_id Logical table identifier.
 * \param [in] pt_id Memory table identifier.
 * \param [in] pipe Pipe of memory entry to write.
 * \param [in] index Index of memory entry to write.
 * \param [in] entry_data New data value of entry to set.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmcth_l2_per_pipe_ireq_read(int unit,
                             bcmltd_sid_t lt_id,
                             bcmdrd_sid_t pt_id,
                             int pipe,
                             int index,
                             void * entry_data);

/*!
 * \brief Interactive hw mem/reg table write for BCMCTH L2.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_id Logical table identifier.
 * \param [in] pt_id Memory table identifier.
 * \param [in] pipe Pipe of memory entry to write.
 * \param [in] index Index of memory entry to write.
 * \param [in] entry_data New data value of entry to set.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmcth_l2_per_pipe_ireq_write(int unit,
                              bcmltd_sid_t lt_id,
                              bcmdrd_sid_t pt_id,
                              int pipe,
                              int index,
                              void * entry_data);


/*!
 * \brief Generic hw mem/reg table read function for BCMCTH L2 component.
 *
 * \param [in] unit Unit number.
 * \param [in] trans_id Transaction identifier.
 * \param [in] lt_id Logical table identifier.
 * \param [in] pt_id Memory table identifier.
 * \param [in] pipe Pipe of memory entry to read.
 * \param [in] start_index Start index of pt to read.
 * \param [in] entry_cnt Entry count to read.
 * \param [out] entry_buf Returned buffer of entry.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmcth_l2_hw_mreq_read_dma(int unit, uint32_t trans_id, bcmltd_sid_t lt_id,
                           bcmdrd_sid_t pt_id, int pipe, int start_index,
                           int entry_cnt, void *entry_buf);

/*!
 * \brief Generic hw mem/reg table read function for BCMCTH L2 component.
 *
 * \param [in] unit Unit number.
 * \param [in] trans_id Transaction identifier.
 * \param [in] lt_id Logical table identifier.
 * \param [in] pt_id Memory table identifier.
 * \param [in] pipe Pipe of memory entry to read.
 * \param [in] start_index Start index of pt to read.
 * \param [in] entry_cnt Entry count to read.
 * \param [out] entry_buf Returned buffer of entry.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmcth_l2_hw_mreq_write_dma(int unit, uint32_t trans_id, bcmltd_sid_t lt_id,
                            bcmdrd_sid_t pt_id, int pipe, int start_index,
                            int entry_cnt, void *entry_buf);


#endif /* BCMCTH_L2_UTIL_H */
