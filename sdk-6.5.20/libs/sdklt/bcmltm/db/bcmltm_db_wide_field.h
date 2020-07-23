/*! \file bcmltm_db_wide_field.h
 *
 * Logical Table Manager - Information for Wide Field Lists.
 *
 * This file contains interfaces to construct information for
 * wide field lists logical tables.
 *
 * Wide field maps are used to check API input value fields contain
 * either all or none of the individual elements of a field >64 bits.
 *
 * The wide field checks are applicable any direct-mapped logical table.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMLTM_DB_WIDE_FIELD_H
#define BCMLTM_DB_WIDE_FIELD_H

#include <shr/shr_types.h>
#include <bcmlrd/bcmlrd_types.h>

#include <bcmltm/bcmltm_md_internal.h>

/*!
 * \brief Create wide field list for a logical table.
 *
 * This routine creates the wide field list information for the given
 * logical table.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical Table ID.
 * \param [out] info_ptr Returning pointer to wide field map information.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_db_wide_field_info_create(int unit,
                                 bcmlrd_sid_t sid,
                                 bcmltm_wide_field_list_t **info_ptr);

/*!
 * \brief Destroy wide field list information.
 *
 * This routine destroys the given wide field list information.
 *
 * \param [in] info Wide field list information to destroy.
 */
extern void
bcmltm_db_wide_field_info_destroy(bcmltm_wide_field_list_t *info);

#endif /* BCMLTM_DB_WIDE_FIELD_H */
