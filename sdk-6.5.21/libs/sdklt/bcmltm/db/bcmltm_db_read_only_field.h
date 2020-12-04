/*! \file bcmltm_db_read_only_field.h
 *
 * Logical Table Manager - Information for Read-Only Field Lists.
 *
 * This file contains interfaces to construct information for
 * read-only field lists logical tables.
 *
 * Read-only field maps are used to check API input value fields do not
 * assign to a field which is not writeable.
 *
 * The read-only field checks are applicable only to tables without
 * field selections.  Field selection logic checks for read-only fields
 * as part of the copy to API cache.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMLTM_DB_READ_ONLY_FIELD_H
#define BCMLTM_DB_READ_ONLY_FIELD_H

#include <shr/shr_types.h>
#include <bcmlrd/bcmlrd_types.h>

#include <bcmltm/bcmltm_md_internal.h>

/*!
 * \brief Create read-only field list for a logical table.
 *
 * This routine creates the read-only field list for the given
 * logical table.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical Table ID.
 * \param [out] info_ptr Returning pointer to read-only field list information.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_db_read_only_field_info_create(int unit,
                                      bcmlrd_sid_t sid,
                                      bcmltm_ro_field_list_t **info_ptr);

/*!
 * \brief Destroy read-only field list information.
 *
 * This routine destroys the given read-only field list information.
 *
 * \param [in] info Read-only field list information to destroy.
 */
extern void
bcmltm_db_read_only_field_info_destroy(bcmltm_ro_field_list_t *info);

#endif /* BCMLTM_DB_READ_ONLY_FIELD_H */
