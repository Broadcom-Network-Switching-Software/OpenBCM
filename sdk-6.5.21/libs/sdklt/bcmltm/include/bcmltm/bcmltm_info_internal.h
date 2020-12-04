/*! \file bcmltm_info_internal.h
 *
 * Logical Table Manager - Low level table information functions.
 *
 * This file contains routines that provide low level
 * properties information for logical tables derived
 * from the LRD table definition and maps.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMLTM_INFO_INTERNAL_H
#define BCMLTM_INFO_INTERNAL_H

#include <shr/shr_types.h>
#include <bcmltd/bcmltd_table.h>
#include <bcmlrd/bcmlrd_types.h>
#include <bcmltm/bcmltm_types_internal.h>

/*!
 * \brief Get the logical table type.
 *
 * This routine gets the table type for the specified logical table,
 * such as simple index, index with allocation, hash, tcam, or
 * configuration table.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical table ID.
 * \param [out] type Table type.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_info_table_type_get(int unit,
                           bcmlrd_sid_t sid,
                           bcmltm_table_type_t *type);

/*!
 * \brief Get the table map type.
 *
 * This routine gets the map type for the specified logical table.
 * The map type indicates if a table is a direct mapped table,
 * a custom table, or a special LTM internal table.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical table ID.
 * \param [out] type Table map type.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_info_table_map_type_get(int unit,
                               bcmlrd_sid_t sid,
                               bcmltm_table_map_type_t *type);

/*!
 * \brief Get the table operating mode.
 *
 * This routine gets the operating mode, interactive or modeled,
 * for the specified logical table.
  *
 * \param [in] unit Unit number.
 * \param [in] sid Logical table ID.
 * \param [out] mode Table operating mode.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_info_table_mode_get(int unit,
                           bcmlrd_sid_t sid,
                           bcmltm_table_mode_t *mode);

/*!
 * \brief Get the custom table handler ID.
 *
 * This routine gets the custom table handler ID for the specified
 * logical table.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical table ID.
 * \param [out] handler_id CTH handler ID.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_info_table_cth_handler_id_get(int unit,
                                     bcmlrd_sid_t sid,
                                     uint32_t *handler_id);

/*!
 * \brief Get the table entry limit handler ID.
 *
 * This routine gets the table entry limit handler ID for
 * the given logical table as specified by the table map attribute.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical table ID.
 * \param [out] handler_id Table entry limit handler ID.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_info_table_entry_limit_handler_id_get(int unit,
                                             bcmlrd_sid_t sid,
                                             uint32_t *handler_id);

#endif /* BCMLTM_INFO_INTERNAL_H */
