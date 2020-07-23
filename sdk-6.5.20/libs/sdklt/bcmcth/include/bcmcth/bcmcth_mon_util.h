/*! \file bcmcth_mon_util.h
 *
 * MON component utility function.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMCTH_MON_UTIL_H
#define BCMCTH_MON_UTIL_H

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
typedef int bcmmon_id_t;

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
#define BCMMON_LT_FIELD_SET(_m, _f)    \
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
#define BCMMON_LT_FIELD_GET(_m, _f)  (SHR_BITGET(_m, _f))

/*! HA subcomponent ID for mon trace. */
#define BCMMON_TRACE_SUB_COMP_ID (1)

/*! HA subcomponent ID for mon drop. */
#define BCMMON_DROP_SUB_COMP_ID  (2)

/*! HA subcomponent ID for mon telemetry. */
#define BCMMON_TELEMETRY_SUB_COMP_ID  (3)

/*! HA subcomponent ID for mon flowtracker. */
#define BCMMON_FLOWTRACKER_SUB_COMP_ID (4)

/*! HA subcomponent ID for mon INT. */
#define BCMMON_INT_SUB_COMP_ID  (5)

/*! HA subcomponent ID for mon flowtracker static flow entry */
#define BCMMON_FLOWTRACKER_STATIC_FLOW_ENTRY_SUB_COMP_ID (6)

/*! HA subcomponent ID for mon trace. */
#define BCMMON_REDIRECT_TRACE_SUB_COMP_ID (7)

/*******************************************************************************
 * Public functions
 */
/*!
 * \brief Generic hw mem table read for BCMMON.
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
bcmcth_mon_pt_read(int unit, uint32_t trans_id, bcmltd_sid_t lt_id,
                    bcmdrd_sid_t pt_id, int index, void *entry_data);

/*!
 * \brief Generic hw mem table write for BCMMON.
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
bcmcth_mon_pt_write(int unit, uint32_t trans_id, bcmltd_sid_t lt_id,
                     bcmdrd_sid_t pt_id, int index, void *entry_data);

/*!
 * \brief Generic hw reg table read for BCMMON.
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
bcmcth_mon_pr_read(int unit, uint32_t trans_id, bcmltd_sid_t lt_id,
                   bcmdrd_sid_t pt_id, int index, void *entry_data);

/*!
 * \brief Generic hw reg table write for BCMMON.
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
bcmcth_mon_pr_write(int unit, uint32_t trans_id, bcmltd_sid_t lt_id,
                    bcmdrd_sid_t pt_id, int index, void *entry_data);
/*!
 * \brief Checks whether a LT is an IMM mapped or not.
 *
 * This function checks whether a LT is an IMM mapped or not.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Symbol ID of a LT.
 *
 * \retval TRUE No errors.
 * \retval FALSE on errors.
 */
bool
bcmcth_mon_imm_mapped(int unit, const bcmdrd_sid_t sid);

/*!
 * \brief Generic interactive hw mem table read for BCMMON.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_id Logical table identifier.
 * \param [in] pt_id Memory table identifier.
 * \param [in] index Index of memory entry to read.
 * \param [out] entry_data Returned data value of entry.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmcth_mon_pt_iread(int unit, bcmltd_sid_t lt_id,
                    bcmdrd_sid_t pt_id, int index, void *entry_data);

/*!
 * \brief Generic interactive hw mem table read for a given pipe for BCMMON.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_id Logical table identifier.
 * \param [in] pt_id Memory table identifier.
 * \param [in] pipe  Pipe number.
 * \param [in] index Index of memory entry to read.
 * \param [out] entry_data Returned data value of entry.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmcth_mon_pt_iread_pipe(int unit, bcmltd_sid_t lt_id,
        bcmdrd_sid_t pt_id, int pipe, int index, void *entry_data);

/*!
 * \brief Generic interactive hw mem table write for BCMMON.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_id Logical table identifier.
 * \param [in] pt_id Memory table identifier.
 * \param [in] index Index of memory entry to write.
 * \param [in] entry_data New data value of entry to set.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmcth_mon_pt_iwrite(int unit, bcmltd_sid_t lt_id,
                     bcmdrd_sid_t pt_id, int index, void *entry_data);

/*!
 * \brief Generic hw mem table write for a given pipe for BCMMON
 *
 * \param [in] unit Unit number.
 * \param [in] lt_id Logical table identifier.
 * \param [in] pt_id Memory table identifier.
 * \param [in] pipe  Pipe number.
 * \param [in] index Index of memory entry to write.
 * \param [in] entry_data New data value of entry to set.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmcth_mon_pt_iwrite_pipe(int unit, bcmltd_sid_t lt_id,
        bcmdrd_sid_t pt_id, int pipe, int index, void *entry_data);
#endif /* BCMCTH_MON_UTIL_H */
