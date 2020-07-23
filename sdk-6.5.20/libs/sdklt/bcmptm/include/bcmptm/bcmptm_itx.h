/*! \file bcmptm_itx.h
 *
 * ITX (sub-component) interface functions and definitions to UFT-MGR.
 *
 * This file contains APIs and definitions of Index LT tile mode
 * LTA transform and associated elements.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMPTM_ITX_H
#define BCMPTM_ITX_H

/*******************************************************************************
 * Includes
 */
#include <sal/sal_types.h>
#include <bcmbd/bcmbd.h>
#include <bcmptm/bcmptm.h>
#include <bcmdrd/bcmdrd_types.h>
#include <bcmltd/bcmltd_handler.h>
#include <bcmptm/bcmptm_tile_info.h>

/*******************************************************************************
 * Defines
 */

/*******************************************************************************
 * Typedef
 */

/*******************************************************************************
 * Function prototypes
 */

/*
 * \brief Initialize ITX LTA records on this BCM unit.
 *
 * \param [in] unit Unit number.
 * \param [in] warm Inidicates cold or warm start status.
 *
 * \retval SHR_E_NONE On success.
 * \retval SHR_E_INIT On invalid device variant information.
 * \retval SHR_E_MEMORY On memory allocation failure.
 */
extern int
bcmptm_itx_init(int unit,
                bool warm);

/*
 * \brief Stop interaction with other components by ITX LTA implementation.
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE Success
 */
extern int
bcmptm_itx_stop(int unit);

/*
 * \brief Free all runtime resources allocated by ITX LTA implementation.
 *
 * \param [in] unit Unit number.
 * \param [in] warm Indicates cold or warm start status.
 *
 * \retval SHR_E_NONE Success
 */
extern int
bcmptm_itx_cleanup(int unit,
                   bool warm);

/*!
 * \brief ITX PT index and PTID transform
 *
 * Convert tile-aware LT key field index to PT select and PT index.
 *
 * \param [in]  unit          Unit Number.
 * \param [in]  in            Input field values.
 * \param [out] out           Output field values.
 * \param [in]  arg           Transform arguments.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
extern int
bcmptm_itx_index_select_transform(int unit,
                                  const bcmltd_fields_t *in,
                                  bcmltd_fields_t *out,
                                  const bcmltd_transform_arg_t *arg);

/*!
 * \brief ITX PT index and PTID reverse transform
 *
 * Convert tile-aware PT select and PT index to LT key field index.
 *
 * \param [in]  unit          Unit Number.
 * \param [in]  in            Input field values.
 * \param [out] out           Output field values.
 * \param [in]  arg           Transform arguments.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
extern int
bcmptm_itx_index_select_rev_transform(int unit,
                                      const bcmltd_fields_t *in,
                                      bcmltd_fields_t *out,
                                      const bcmltd_transform_arg_t *arg);

/*!
 * \brief ITX PT index, PTID, and selector transform
 *
 * Convert tile-aware LT key field index to PT select, PT index,
 * and field selector.
 *
 * \param [in]  unit          Unit Number.
 * \param [in]  in            Input field values.
 * \param [out] out           Output field values.
 * \param [in]  arg           Transform arguments.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
extern int
bcmptm_itx_multi_index_select_transform(int unit,
                                        const bcmltd_fields_t *in,
                                        bcmltd_fields_t *out,
                                        const bcmltd_transform_arg_t *arg);

/*!
 * \brief ITX PT index, PTID, and selector reverse transform
 *
 * Convert tile-aware PT select, PT index, and field selector to
 * LT key field index.
 *
 * \param [in]  unit          Unit Number.
 * \param [in]  in            Input field values.
 * \param [out] out           Output field values.
 * \param [in]  arg           Transform arguments.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
extern int
bcmptm_itx_multi_index_select_rev_transform(int unit,
                                            const bcmltd_fields_t *in,
                                            bcmltd_fields_t *out,
                                            const bcmltd_transform_arg_t *arg);

/*!
 * \brief ITX Atomic transaction preparation
 *
 * \param [in]  unit            Unit number.
 * \param [in]  trans_id        Transaction identifier.
 * \param [in]  arg             Handler arguments.
 *
 * Handler should return SHR_E_NONE if operation was performed.
 *
 * \retval SHR_E_NONE  OK
 * \retval !SHR_E_NONE ERROR
 */
extern int
bcmptm_itx_prepare(int unit,
                   uint32_t trans_id,
                   const bcmltd_generic_arg_t *arg);

/*!
 * \brief ITX Atomic transaction commit
 *
 * \param [in]  unit            Unit number.
 * \param [in]  trans_id        Transaction identifier.
 * \param [in]  arg             Handler arguments.
 *
 * Handler should return SHR_E_NONE if operation was performed.
 *
 * \retval SHR_E_NONE  OK
 * \retval !SHR_E_NONE ERROR
 */
extern int
bcmptm_itx_commit(int unit,
                  uint32_t trans_id,
                  const bcmltd_generic_arg_t *arg);

/*!
 * \brief ITX Atomic transaction abort
 *
 * \param [in]  unit            Unit number.
 * \param [in]  trans_id        Transaction identifier.
 * \param [in]  arg             Handler arguments.
 *
 * Handler should return SHR_E_NONE if operation was performed.
 *
 * \retval SHR_E_NONE  OK
 * \retval !SHR_E_NONE ERROR
 */
extern int
bcmptm_itx_abort(int unit,
                 uint32_t trans_id,
                 const bcmltd_generic_arg_t *arg);

/*!
 * \brief Get the entry limit for a tile-aware index LT.
 *
 * This function gets the entry limit for the tile-aware index LT with
 * the current set of tile modes.
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
bcmptm_itx_entry_limit_get(int unit,
                           uint32_t trans_id,
                           bcmltd_sid_t sid,
                           const bcmltd_table_entry_limit_arg_t *table_arg,
                           bcmltd_table_entry_limit_t *table_data,
                           const bcmltd_generic_arg_t *arg);

/*!
 * \brief Get tile list structure root for this unit.
 *
 * This function returns the root tile list data structure describing
 * the L2P map per tile mode for the given unit.
 *

 * \param [in]  unit Unit number.
 * \param [out] tile_list Returning table list root pointer.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmptm_itx_tile_list_get(int unit,
                         const bcmptm_tile_list_t **tile_list);

#endif /* BCMPTM_ITX_H */
