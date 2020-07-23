/*! \file bcmltm_db_tc.h
 *
 * Logical Table Manager - Information for Table Commit Interfaces.
 *
 * This file contains routines to construct information for
 * table commit interfaces in logical tables.
 *
 * This information is derived from the LRD Table Commit map groups.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMLTM_DB_TC_H
#define BCMLTM_DB_TC_H

#include <shr/shr_types.h>

#include <bcmltm/bcmltm_md_internal.h>


/*!
 * \brief Table Commit Handlers Information.
 *
 * This structure contains information derived from the LRD
 * for Table Commit map groups.
 *
 * The information is referenced by the LTM metadata and
 * is used for different purposes.
 */
typedef struct bcmltm_db_tc_info_s {
    /*! Table Commit handler list. */
    bcmltm_table_commit_list_t *tc_list;
} bcmltm_db_tc_info_t;


/*!
 * \brief Create information for Table Commit Interfaces.
 *
 * Create information for Table Commit interfaces for the specified
 * logical table.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [out] info_ptr Returning pointer to info.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_db_tc_info_create(int unit, bcmlrd_sid_t sid,
                         bcmltm_db_tc_info_t **info_ptr);

/*!
 * \brief Destroy information for Table Commit Interfaces.
 *
 * Destroy given Table Commit Interfaces information.
 *
 * \param [in] info Pointer to info to destroy.
 */
extern void
bcmltm_db_tc_info_destroy(bcmltm_db_tc_info_t *info);

#endif /* BCMLTM_DB_TC_H */
