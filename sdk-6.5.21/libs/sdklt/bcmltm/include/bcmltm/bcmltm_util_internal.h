/*! \file bcmltm_util_internal.h
 *
 * Logical Table Manager Utility Interfaces.
 *
 * This file contains low level utility routines.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMLTM_UTIL_INTERNAL_H
#define BCMLTM_UTIL_INTERNAL_H

#include <shr/shr_types.h>
#include <bcmdrd/bcmdrd_types.h>
#include <bcmlrd/bcmlrd_types.h>

/*!
 * \brief Get table name for given physical table ID.
 *
 * This function returns a string pointer to the table name
 * for the given physical table ID.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Physical table ID.
 *
 * \retval String pointer to the table name or UNKNOWN if not found.
 */
extern const char *
bcmltm_pt_table_sid_to_name(int unit,
                            bcmdrd_sid_t sid);

/*!
 * \brief Get table name for given logical table ID.
 *
 * This function returns a string pointer to the table name
 * for the given logical table ID.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical table ID.
 *
 * \retval String pointer to the table name or UNKNOWN if not found.
 */
extern const char *
bcmltm_lt_table_sid_to_name(int unit,
                            bcmlrd_sid_t sid);

/*!
 * \brief Get table name for given table ID.
 *
 * This function returns a string pointer to the table name
 * for the given logical or physical table ID.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID, logical or physical.
 * \param [in] logical Indicates if table ID is logical or physical.
 *                     If TRUE, the ID is a logical table ID.
 *                     If FALSE, the ID is a physical table ID.
 *
 * \retval String pointer to the table name or UNKNOWN if not found.
 */
extern const char *
bcmltm_table_sid_to_name(int unit,
                         uint32_t sid,
                         bool logical);

/*!
 * \brief Return table ID type string.
 *
 * This helper routine returns a string pointer to the table ID type
 * to be used when displaying the table ID.
 *
 * \param [in] logical Indicates if table is logical or physical.
 *
 * \retval String pointer to the table ID type.
 */
static inline const char *
bcmltm_table_sid_type_str(bool logical)
{
    if (logical) {
        return "ltid";
    } else {
        return "ptid";
    }
}

/*!
 * \brief Get field name for given physical field ID.
 *
 * This function returns a string pointer to the field name
 * for the given physical table and field IDs.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Physical table ID.
 * \param [in] fid Physical field ID.
 *
 * \retval String pointer to the field name or UNKNOWN if not found.
 */
extern const char *
bcmltm_pt_field_fid_to_name(int unit,
                            bcmdrd_sid_t sid,
                            bcmdrd_fid_t fid);

/*!
 * \brief Get field name for given logical field ID.
 *
 * This function returns a string pointer to the field name
 * for the given logical table and field IDs.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical table ID.
 * \param [in] fid Logical field ID.
 *
 * \retval String pointer to the field name or UNKNOWN if not found.
 */
extern const char *
bcmltm_lt_field_fid_to_name(int unit,
                            bcmlrd_sid_t sid,
                            bcmlrd_fid_t fid);

/*!
 * \brief Get field name for given field ID.
 *
 * This function returns a string pointer to the field name
 * for the given logical or physical table and field IDs.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID, logical or physical.
 * \param [in] fid Field ID, logical or physical.
 * \param [in] logical Indicates if IDs are logical or physical.
 *                     If TRUE, the IDs are logical table and field IDs.
 *                     If FALSE, the IDs are physical table and field IDs.
 *
 * \retval String pointer to the field name or UNKNOWN if not found.
 */
extern const char *
bcmltm_field_fid_to_name(int unit,
                         uint32_t sid,
                         uint32_t fid,
                         bool logical);

#endif /* BCMLTM_UTIL_INTERNAL_H */
