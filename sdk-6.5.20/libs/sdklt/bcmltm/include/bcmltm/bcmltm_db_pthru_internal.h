/*! \file bcmltm_db_pthru_internal.h
 *
 * Logical Table Manager - PT Pass Thru Table Database.
 *
 * This file contains routines to create and provide
 * information for Pass Thru tables.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMLTM_DB_PTHRU_INTERNAL_H
#define BCMLTM_DB_PTHRU_INTERNAL_H

#include <shr/shr_types.h>

#include <bcmltm/bcmltm_md_internal.h>
#include <bcmltm/bcmltm_wb_types_internal.h>

/*
 * PT Pass Thru Mapping.
 *
 * Table and fields mapping in PT Pass Thru is 1-1.
 *
 * This translates to 1 PT view and 1 PT operation for a view.
 */

/*! Number of PT views required for a table (1 mapped PT). */
#define BCMLTM_PTHRU_NUM_PT_VIEWS          1

/*! Number of PT ops required for a PT view. */
#define BCMLTM_PTHRU_NUM_PT_OPS            1

/*!
 * \brief Initialize PT Pass Thru table database.
 *
 * This routine initializes the database for PT Pass Thru tables and
 * creates internal data structures to maintain the information.
 *
 * \param [in] unit Unit number.
 * \param [in] sid_max_count Maximum number of PT Pass Thru tables on unit.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_db_pthru_init(int unit,
                     uint32_t sid_max_count);

/*!
 * \brief Cleanup PT Pass Thru table database.
 *
 * This routine frees any resources allocated by the PT Pass Thru
 * database.
 *
 * \param [in] unit Unit number.
 */
extern void
bcmltm_db_pthru_cleanup(int unit);

/*!
 * \brief Create table database information.
 *
 * This routine creates the database information for the given
 * table.
 *
 * The PT Pass Thru database stores the information of only 1 table
 * at any given time.  The function will only proceed to create
 * new data if there is no table in the database at the time.
 * If the database contains a valid table, the caller first
 * needs to destroy the existing table information.
 *
 * - If database contains a valid cached table and provided table ID
 *   is new, an error is returned.
 * - If provided table ID matches existing cached table ID,
 *   there is no action.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_db_pthru_table_create(int unit,
                             bcmdrd_sid_t sid);

/*!
 * \brief Destroy table database information.
 *
 * This routine destroys the database information for the given
 * table.
 *
 * The table ID BCMDRD_INVALID_ID has special meaning.  It indicates
 * to destroy the database for the current cached table.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_db_pthru_table_destroy(int unit,
                              bcmdrd_sid_t sid);

/*!
 * \brief Get the working buffer handle.
 *
 * This routine gets the working buffer handle for the given table.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [out] wb_handle_ptr Returning pointer to the WB handle.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_db_pthru_wb_handle_get(int unit,
                              uint32_t sid,
                              const bcmltm_wb_handle_t **wb_handle_ptr);

/*!
 * \brief Get the PT list metadata.
 *
 * This routine gets the PT index list metadata for given table.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [out] pt_list_ptr Returning pointer to PT list metadata.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_db_pthru_pt_list_get(int unit,
                            bcmdrd_sid_t sid,
                            const bcmltm_pt_list_t **pt_list_ptr);

/*!
 * \brief Get key fields mapping metadata.
 *
 * This routine gets the key fields mapping metadata for given table.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [out] fields_ptr Returning pointer to fields mapping metadata.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_db_pthru_key_fields_get(int unit,
                               bcmdrd_sid_t sid,
                         const bcmltm_field_select_mapping_t **fields_ptr);

/*!
 * \brief Get value fields mapping metadata.
 *
 * This routine gets the value fields mapping metadata for given table.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [out] fields_ptr Returning pointer to fields mapping metadata.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_db_pthru_value_fields_get(int unit,
                                 bcmdrd_sid_t sid,
                           const bcmltm_field_select_mapping_t **fields_ptr);

/*!
 * \brief Get field information for register/memory field.
 *
 * This routine gets the field information for the given table
 * and field.
 *
 * This function constructs the information on the fly and does not use
 * the cached table database.  Unlike other functions in this module,
 * the caller is not required to create the database for
 * the targeted table prior to using this routine.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [in] fid Field ID.
 * \param [out] finfo Field information to fill.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_db_pthru_field_info_get(int unit,
                               bcmdrd_sid_t sid,
                               bcmdrd_fid_t fid,
                               bcmdrd_sym_field_info_t *finfo);

#endif /* BCMLTM_DB_PTHRU_INTERNAL_H */
