/*! \file bcmltm_db_cth.h
 *
 * Logical Table Manager - Information for Custom Table Handlers.
 *
 * This file contains routines to construct information for
 * custom table handlers.
 *
 * This information is derived from the LRD Custom Table Handler map groups.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMLTM_DB_CTH_H
#define BCMLTM_DB_CTH_H

#include <shr/shr_types.h>

#include <bcmltm/bcmltm_md_internal.h>
#include <bcmltm/bcmltm_wb_types_internal.h>


/*!
 * \brief Custom Table Handler Information.
 *
 * This structure contains information derived from the LRD
 * Custom Table Handler map groups.
 *
 * The information is referenced by the LTM metadata and
 * is used for different purposes, such as node cookies for FA and EE
 * nodes.
 */
typedef struct bcmltm_db_cth_info_s {
    /*! Working buffer block ID for this LTA CTH. */
    bcmltm_wb_block_id_t wb_block_id;

    /*! Custom Table Handler interface. */
    const bcmltd_table_handler_t *handler;

    /*! Custom Table Handler interface type. */
    bcmltm_cth_handler_intf_type_t intf_type;

    /*!
     * LTA table parameters used as the LTA EE node cookie.
     *
     * This information can be shared among several BCMLTM_MD operations
     * for a given symbol.
     */
    bcmltm_lta_table_params_t *table_params;

    /*!
     * LTA input field list used as the LTA FA node cookie for input.
     *
     * This information can be shared among several BCMLTM_MD operations
     * for a given symbol.
     */
    bcmltm_lta_field_list_t *fin;

    /*!
     * LTA input key field list used for CTH DELETE/LOOKUP/TRAVERSE operations.
     */
    bcmltm_lta_field_list_t *fin_keys;

    /*!
     * LTA output field list used as the LTA FA node cookie for output.
     *
     * This information can be shared among several BCMLTM_MD operations
     * for a given symbol.
     */
    bcmltm_lta_field_list_t *fout;

} bcmltm_db_cth_info_t;


/*!
 * \brief Create information for Custom Table Handler.
 *
 * Create information for Custom Table Handler for the specified
 * logical table.
 *
 * Currently, only 1 custom table handler is supported per logical table.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [out] info_ptr Returning pointer to info.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_db_cth_info_create(int unit, bcmlrd_sid_t sid,
                          bcmltm_db_cth_info_t **info_ptr);

/*!
 * \brief Destroy information for Custom Table Handler.
 *
 * Destroy given Custom Table Handler information.
 *
 * \param [in] info Pointer to info to destroy.
 */
extern void
bcmltm_db_cth_info_destroy(bcmltm_db_cth_info_t *info);


#endif /* BCMLTM_DB_CTH_H */
