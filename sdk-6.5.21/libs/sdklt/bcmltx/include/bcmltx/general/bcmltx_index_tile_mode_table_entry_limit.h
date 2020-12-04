/*! \file bcmltx_index_tile_mode_table_entry_limit.h
 *
 * Table entry limit handler for index tile mode tables.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMLTX_INDEX_TILE_MODE_TABLE_ENTRY_LIMIT_H
#define BCMLTX_INDEX_TILE_MODE_TABLE_ENTRY_LIMIT_H

#include <bcmltd/bcmltd_handler.h>

/*!
 * \brief Index tile mode table entry limit get.
 *
 * This is the handler function for the index tile mode
 * table entry limit handler.
 *
 * \param [in] unit Unit Number.
 * \param [in] trans_id LT transaction ID.
 * \param [in] sid logical table ID.
 * \param [in] table_arg Table arguments.
 * \param [out] table_data Returning table actual entry limit.
 * \param [in] arg Component arguments.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltx_index_tile_mode_table_entry_limit_get(int unit,
                            uint32_t trans_id,
                            bcmltd_sid_t sid,
                            const bcmltd_table_entry_limit_arg_t *table_arg,
                            bcmltd_table_entry_limit_t *table_data,
                            const bcmltd_generic_arg_t *arg);

#endif /* BCMLTX_INDEX_TILE_MODE_TABLE_ENTRY_LIMIT_H */
