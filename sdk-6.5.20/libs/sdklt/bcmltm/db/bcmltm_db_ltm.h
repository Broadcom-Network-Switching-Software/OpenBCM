/*! \file bcmltm_db_ltm.h
 *
 * Logical Table Manager - Information for LTM Managed Tables.
 *
 * This file contains routines to construct information for
 * LTM managed tables, such as TABLE_ LTs.
 *
 * This information is derived from the LRD LTM Managed Table map groups.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMLTM_DB_LTM_H
#define BCMLTM_DB_LTM_H

#include <shr/shr_types.h>

#include <bcmltm/bcmltm_md_internal.h>
#include <bcmltm/bcmltm_wb_types_internal.h>

/*!
 * \brief LTM Managed Tables Information.
 *
 * This structure contains information derived from the LRD
 * for LTM Managed map groups.
 *
 * The information is referenced by the LTM metadata and
 * is used for different purposes, such as node cookies for FA and EE
 * nodes.
 */
typedef struct bcmltm_db_ltm_info_s {
    /*! Working buffer block ID for this LT LTM. */
    bcmltm_wb_block_id_t wb_block_id;

    /*!
     * LTM internal bcmltm_fa_ltm_t used as the FA node cookie
     * to get the keys for TRAVERSE operations.
     */
    bcmltm_fa_ltm_t *fa_ltm;

    /*!
     * LT KEY fields mapping bcmltm_field_select_mapping_t used
     * as the FA Keys node cookie.
     *
     * This information can be shared among several BCMLTM_MD operations
     * for a given symbol.
     */
    bcmltm_field_select_mapping_t *ltm_key;

    /*!
     * LT field spec list used as the EE node cookie.
     *
     * This information can be shared among several BCMLTM_MD operations
     * for a given symbol.
     */
    bcmltm_field_spec_list_t *fs_list;

} bcmltm_db_ltm_info_t;


/*!
 * \brief Create information for LTM managed table.
 *
 * Create information for LTM managed map groups for the specified
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
bcmltm_db_ltm_info_create(int unit, bcmlrd_sid_t sid,
                          bcmltm_db_ltm_info_t **info_ptr);

/*!
 * \brief Destroy information for LTM managed table.
 *
 * Destroy given LTM managed table information.
 *
 * \param [in] info Pointer to info to destroy.
 */
extern void
bcmltm_db_ltm_info_destroy(bcmltm_db_ltm_info_t *info);

#endif /* BCMLTM_DB_LTM_H */
