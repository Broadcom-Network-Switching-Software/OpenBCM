/*! \file bcmltd_table.h
 *
 * Table definition APIs.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMLTD_TABLE_H
#define BCMLTD_TABLE_H

#include <bcmltd/bcmltd_types.h>

/*!
 * \brief Get a table definition.
 *
 * Get a table definition for a  for a given table ID.
 *
 * \param [in]  sid             Logical Table ID.
 *
 * \retval !NULL  OK
 * \retval NULL   invalid table ID
 */
extern const bcmltd_table_rep_t *
bcmltd_table_get(bcmltd_sid_t sid);


/*!
 * \brief Get the number of field indexes for given field width.
 *
 * Return the number of field indexes required for given field width.
 *
 * \param [in] width Field width in bits.
 *
 * \retval Number of indexes required.
 */
extern uint32_t
bcmltd_field_idx_count_get(uint32_t width);

/*!
 * \brief Return the index for a table name.
 *
 * Note: This API is DEPRECATED.
 *
 * \param [in] table_name       Logical Table name.
 *
 * \retval >= 0                 Table index.
 * \retval SHR_E_UNAVAIL        Table not found.
 */

extern int bcmltd_table_name_to_idx(const char *table_name);

/*!
 * \brief Return the table and field index for a table and field names.
 *
 * For the given table and field names, get the table ID and
 * ID of the field within the logical table.
 *
 * Note: This API is DEPRECATED.
 *
 * \param [in]  unit            Unit Number.
 * \param [in]  table_name      Logical Table name.
 * \param [in]  field_name      Logical Field name.
 * \param [out] sid             Logical Table ID.
 * \param [out] fid             Field ID within the LT.
 *
 * \retval SHR_E_NONE           Success.
 * \retval SHR_E_UNAVAIL        Table/Field not found.
 */
extern int
bcmltd_table_field_name_to_idx(int unit,
                               const char *table_name,
                               const char *field_name,
                               bcmltd_sid_t *sid,
                               bcmltd_fid_t *fid);

/*!
 * \brief Return the field index for a table ID and field name.
 *
 * Get the ID of the field for the given table ID and field name.
 *
 * \param [in]  sid             Logical Table ID.
 * \param [in]  field_name      Logical Field name.
 * \param [out] fid             Field ID within the LT.
 *
 * \retval SHR_E_NONE           Success.
 * \retval SHR_E_UNAVAIL        Field not found.
 */
extern int
bcmltd_field_name_to_idx(bcmltd_sid_t sid,
                         const char *field_name,
                         bcmltd_fid_t *fid);

/*!
 * \brief Get local field ID for given table and global field ID.
 *
 * Get local field ID for given table and global field ID.
 *
 * \param [in] sid              Logical Table ID.
 * \param [in] gfid             Logical field global ID.
 * \param [out] fid             Pointer to logical field local ID.
 *
 * \retval SHE_E_NONE           Operation successful.
 * \retval SHR_E_UNAVAIL        Table/Field not found.
 */
extern int
bcmltd_fid_get(bcmltd_sid_t sid,
               bcmltd_gfid_t gfid,
               bcmltd_fid_t *fid);

/*!
 * \brief Get global field ID for given table and local field ID.
 *
 * Get global field ID for given table and local field ID.
 *
 * \param [in] sid              Logical Table ID.
 * \param [in] fid              Logical field local ID.
 * \param [out] gfid            Pointer to logical field global ID.
 *
 * \retval SHE_E_NONE           Operation successful.
 * \retval SHR_E_UNAVAIL        Table/Field not found.
 */
extern int
bcmltd_gfid_get(bcmltd_sid_t sid,
                bcmltd_fid_t fid,
                bcmltd_gfid_t *gfid);

/*!
 * \brief Covert a field value to symbol string.
 *
 * Convert a symbol field uint32_t value to symbol.
 *
 * \param [in]  sid             Table ID.
 * \param [in]  fid             Table field ID.
 * \param [in]  val             Field value.
 * \param [out] sym             Field symbol.
 *
 * \retval 0  OK
 * \retval <0 ERROR
 */

extern int
bcmltd_field_value_to_symbol(bcmltd_sid_t sid,
                             bcmltd_fid_t fid,
                             uint32_t val,
                             const char **sym);

/*!
 * \brief Return the field depth.
 *
 * \param [in]  sid             Logical Table ID.
 * \param [in]  fid             Logical Field name.
 * \param [out] depth           Field depth.
 *
 * \retval SHR_E_NONE           Success.
 * \retval SHR_E_PARAM          The output buffer is NULL.
 * \retval SHR_E_UNAVAIL        Table not found.
 * \retval SHR_E_UNAVAIL        Field not found.
 */
extern int
bcmltd_field_depth_get(bcmltd_sid_t sid,
                       bcmltd_fid_t fid,
                       uint32_t *depth);

#endif /* BCMLTD_TABLE_H */
