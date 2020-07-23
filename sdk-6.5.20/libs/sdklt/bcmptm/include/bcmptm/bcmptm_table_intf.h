/*! \file bcmptm_table_intf.h
 *
 * This file contains interfaces for the table information handler.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMPTM_TABLE_INTF_H
#define BCMPTM_TABLE_INTF_H

#include <sal/sal_types.h>
#include <bcmltd/bcmltd_handler.h>

/*!
 * \brief Get the entry limit for a modeled keyed logical table.
 *
 * This function gets the entry limit for the specified modeled keyed
 * logical table.
 *
 * The returned entry limit value is reflected in TABLE_INFO.ENTRY_LIMIT.
 *
 * \param [in]  unit Unit number.
 * \param [in]  trans_id Transaction identifier.
 * \param [in]  sid Logical table ID.
 * \param [in]  table_arg Table arguments.
 * \param [out] table_data Returning table entry limit.
 * \param [in]  arg Handler arguments (not used).
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmptm_mreq_keyed_table_entry_limit_get(int unit,
                        uint32_t trans_id,
                        bcmltd_sid_t sid,
                        const bcmltd_table_entry_limit_arg_t *table_arg,
                        bcmltd_table_entry_limit_t *table_data,
                        const bcmltd_generic_arg_t *arg);

#endif /* BCMPTM_TABLE_INTF_H */
