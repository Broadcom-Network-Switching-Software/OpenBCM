/*! \file bcmcth_mirror_util.h
 *
 * MIRROR component utility function.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMCTH_MIRROR_UTIL_H
#define BCMCTH_MIRROR_UTIL_H

/*******************************************************************************
 * Includes
 */
#include <shr/shr_types.h>
#include <shr/shr_bitop.h>
#include <bcmdrd_config.h>
#include <bcmdrd/bcmdrd_types.h>
#include <bcmltd/bcmltd_types.h>
#include <bcmbd/bcmbd.h>
#include <bcmptm/bcmptm.h>
#include <bcmptm/bcmptm_types.h>

/*! Monitor Identifier.  */
typedef int bcmmirror_id_t;

/*!
 * \brief This macro is used to set a Field present indicator bit status.
 *
 * Sets the Field \c _f present bit indicator in the Field Bitmap \c _m.
 *
 * \param [in] _m Field bitmap.
 * \param [in] _f Field position in the bitmap.
 *
 * \returns Nothing.
 */
#define BCMMIRROR_LT_FIELD_SET(_m, _f) \
            do {                       \
                SHR_BITSET(_m, _f);;   \
            } while (0)

/*!
 * \brief This macro is used to get the Field present indicator bit status.
 *
 * Returns the Field \c _f present bit indicator status from the Field
 * Bitmap \c _m.
 *
 * \param [in] _m Field bitmap.
 * \param [in] _f Field position in the bitmap.
 *
 * \returns Field present indicator bit status from the Field bitmap.
 */
#define BCMMIRROR_LT_FIELD_GET(_m, _f)  (SHR_BITGET(_m, _f))

/*! HA subcomponent ID for mirror encap. */
#define BCMMIRROR_ENCAP_SUB_COMP_ID (1)

/*! HA subcomponent ID for mirror encap. */
#define BCMMIRROR_ENCAP_FLEX_HDR_SUB_COMP_ID (2)

/*******************************************************************************
 * Public functions
 */
/*!
 * \brief Generic hw mem table read for BCMMIRROR.
 *
 * \param [in] unit Unit number.
 * \param [in] trans_id Transaction identifier.
 * \param [in] lt_id Logical table identifier.
 * \param [in] pt_id Memory table identifier.
 * \param [in] index Index of memory entry to read.
 * \param [out] entry_data Returned data value of entry.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmcth_mirror_pt_read(int unit, uint32_t trans_id, bcmltd_sid_t lt_id,
                      bcmdrd_sid_t pt_id, int index, void *entry_data);

/*!
 * \brief Generic hw mem table write for BCMMIRROR.
 *
 * \param [in] unit Unit number.
 * \param [in] trans_id Transaction identifier.
 * \param [in] lt_id Logical table identifier.
 * \param [in] pt_id Memory table identifier.
 * \param [in] index Index of memory entry to write.
 * \param [in] entry_data New data value of entry to set.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcmcth_mirror_pt_write(int unit, uint32_t trans_id, bcmltd_sid_t lt_id,
                       bcmdrd_sid_t pt_id, int index, void *entry_data);

/*!
 * \brief Generic hw mem table write for BCMMIRROR.
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
int
bcmcth_mirror_pt_pipe_write(int unit, uint32_t trans_id, bcmltd_sid_t lt_id,
                            bcmdrd_sid_t pt_id, int pipe, int index,
                            void *entry_data);

/*!
 * \brief Generic hw mem table write for BCMMIRROR.
 *
 * \param [in] unit Unit number.
 * \param [in] trans_id Transaction identifier.
 * \param [in] lt_id Logical table identifier.
 * \param [in] pt_id Memory table identifier.
 * \param [in] index Index of memory entry to write.
 * \param [in] entry_data New data value of entry to set.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcmcth_mirror_pt_write(int unit, uint32_t trans_id, bcmltd_sid_t lt_id,
                       bcmdrd_sid_t pt_id, int index, void *entry_data);

/*!
 * \brief Generic hw reg table read for BCMMIRROR.
 *
 * \param [in] unit Unit number.
 * \param [in] trans_id Transaction identifier.
 * \param [in] lt_id Logical table identifier.
 * \param [in] pt_id Memory table identifier.
 * \param [in] index Index of memory entry to read.
 * \param [out] entry_data Returned data value of entry.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmcth_mirror_pr_read(int unit, uint32_t trans_id, bcmltd_sid_t lt_id,
                      bcmdrd_sid_t pt_id, int index, void *entry_data);

/*!
 * \brief Generic hw reg table write for BCMMIRROR.
 *
 * \param [in] unit Unit number.
 * \param [in] trans_id Transaction identifier.
 * \param [in] lt_id Logical table identifier.
 * \param [in] pt_id Memory table identifier.
 * \param [in] index Index of memory entry to write.
 * \param [in] entry_data New data value of entry to set.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcmcth_mirror_pr_write(int unit, uint32_t trans_id, bcmltd_sid_t lt_id,
                       bcmdrd_sid_t pt_id, int index, void *entry_data);

/*!
 * \brief Generic hw table interactive read for BCMMIRROR.
 *
 * \param [in] unit Unit number.
 * \param [in] pt_id Memory table identifier.
 * \param [in] index Index of memory entry to write.
 * \param [in] entry_data New data value of entry to set.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcmcth_mirror_ireq_read(int unit, bcmdrd_sid_t pt_id,
                        int index, void *entry_data);

#endif /* BCMCTH_MIRROR_UTIL_H */
