/*! \file bcmltm_md_pthru_internal.h
 *
 * Logical Table Manager Physical Table Pass Through (Thru) Metadata.
 *
 * These definitions are internal to the LTM component and is
 * intended, therefore, to be used only by LTM sub-components.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMLTM_MD_PTHRU_INTERNAL_H
#define BCMLTM_MD_PTHRU_INTERNAL_H

#include <shr/shr_types.h>

#include <bcmltm/bcmltm_md_internal.h>
#include <bcmltm/bcmltm_db_pthru_internal.h>


/*!
 * \brief Initialize the LTM metadata for PT Pass Thru on a given unit.
 *
 * This function requires caller to synchronize access to this routine
 * by possible multiple threads.
 *
 * Assumes unit is valid.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_md_pthru_create(int unit);


/*!
 * \brief Destroy the LTM metadata for PT Pass Thru on a given unit.
 *
 * Cleanup all data associated with the LTM PT Pass Thru for
 * the specified unit.
 *
 * This function requires caller to synchronize access to this routine
 * by possible multiple threads.
 *
 * Assumes unit is valid.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_md_pthru_destroy(int unit);


/*!
 * \brief Retrieve the metadata for a given logical table.
 *
 * Get the pointer to the table metadata for a given table id
 * on the specified unit.
 *
 * Assumes unit is valid.
 *
 * \param [in] unit Unit number.
 * \param [in] ltid Table ID.
 * \param [out] ltm_md_ptr Returns pointer to the table metadata.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_md_pthru_lt_retrieve(int unit,
                            uint32_t ltid,
                            bcmltm_lt_md_t **ltm_md_ptr);

/*!
 * \brief Get the maximum Working Buffer size for PT PassThru.
 *
 * Get the maximum Working Buffer size required to support
 * PT PassThru tables.
 *
 * \param [in] unit Unit number.
 * \param [out] wb_max Returns Working Buffer maximum.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_md_pthru_wb_max_get(int unit,
                           uint32_t *wb_max);

/*!
 * \brief Retrieve the key field list for a physical table.
 *
 * Get the pointer to the key field list for a given physical table ID
 * on the specified unit.
 *
 * Assumes unit is valid and !null param pointer.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Physical table ID.
 * \param [out] list Pointer to key field list.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_md_pthru_key_field_list_retrieve(int unit, bcmdrd_sid_t sid,
                                        const bcmltm_field_map_list_t **list);


/*!
 * \brief Retrieve the value field list for a physical table.
 *
 * Get the pointer to the value field list for a given physical table ID
 * on the specified unit.
 *
 * Assumes unit is valid and !null param pointer.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Physical table ID.
 * \param [out] list Pointer to value field list.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_md_pthru_value_field_list_retrieve(int unit, bcmdrd_sid_t sid,
                                          const bcmltm_field_map_list_t **list);


/*!
 * \brief Get field information for register/memory field.
 *
 * Obtain field information (name, start bit, end bit, etc.) for
 * specified symbol ID and field ID.
 *
 * Assumes unit is valid and !null param pointer.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Physical table ID.
 * \param [in] fid Field ID.
 * \param [out] finfo Field information structure to fill.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static inline int
bcmltm_md_pthru_field_info_get(int unit, bcmdrd_sid_t sid, bcmdrd_fid_t fid,
                               bcmdrd_sym_field_info_t *finfo)
{
    return bcmltm_db_pthru_field_info_get(unit, sid, fid, finfo);
}

#endif /* BCMLTM_MD_PTHRU_INTERNAL_H */
