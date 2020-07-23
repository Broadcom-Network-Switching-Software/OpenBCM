/*! \file bcmcfg_lt.h
 *
 * BCMCFG logical table accessors.
 *
 * These APIs access config table field values either by table and
 * field IDs (used by low level SDKLT software components) or by table
 * and field names (used by SDKLT applications).
 *
 * Config tables are those tables that have a table_type attribute of
 * "config".
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMCFG_LT_H
#define BCMCFG_LT_H

#include <bcmltd/bcmltd_types.h>
#include <bcmltd/bcmltd_handler.h>

/*!
 * \brief Get a scalar field value for a config logical table by id
 *
 * Get a scalar value of a field for the given config logical table.
 * Return an error if the sid is not a config logical table or fid is
 * invalid.
 *
 * \param [in]  unit            Unit number.
 * \param [in]  sid             Logical table sid.
 * \param [in]  fid             Logical table fid.
 * \param [out] value           Pointer to field value.
 *
 * \retval 0  OK
 * \retval <0 Error
 */

extern int
bcmcfg_field_get(int unit,
                 bcmltd_sid_t sid,
                 bcmltd_fid_t fid,
                 uint64_t *value);

/*!
 * \brief Get the array field values for a config logical table by id
 *
 * Get the values of an array field for the given config logical table.
 * Return an error if the sid is not a config logical table or fid is
 * invalid.
 *
 * \param [in]  unit            Unit number.
 * \param [in]  sid             Logical table sid.
 * \param [in]  fid             Logical table fid.
 * \param [in]  start_idx       Start index of the field.
 * \param [in]  num_of_elem     Number of elements.
 * \param [out] value           Pointer to field value.
 * \param [out] actual          Number of elements actually obtained.
 *
 * \retval 0  OK
 * \retval <0 Error
 */

extern int
bcmcfg_field_array_get(int unit,
                       bcmltd_sid_t sid,
                       bcmltd_fid_t fid,
                       uint32_t start_idx,
                       uint32_t num_of_elem,
                       uint64_t *value,
                       size_t *actual);

/*!
 * \brief Set a scalar field for a config logical table by id
 *
 * Set a config table field to the given scalar value.
 *
 * \param [in]  unit            Unit number.
 * \param [in]  sid             Logical table sid.
 * \param [in]  fid             Logical table fid.
 * \param [in]  value           Field value.
 *
 * \retval SHR_E_NONE           Operation is successful.
 * \retval SHR_E_UNAVAIL        Device/Table/Field does not exist.
 * \retval SHR_E_PARAM          Table is not a config table.
 */

extern int
bcmcfg_field_set(int unit,
                 bcmltd_sid_t sid,
                 bcmltd_fid_t fid,
                 uint64_t value);

/*!
 * \brief Set the array field values for a config logical table by id
 *
 * Set the values of an array field for the given config logical table.
 * Return an error if the sid is not a config logical table or fid is
 * invalid.
 *
 * \param [in]  unit            Unit number.
 * \param [in]  sid             Logical table sid.
 * \param [in]  fid             Logical table fid.
 * \param [in]  start_idx       Start index of the field.
 * \param [in]  num_of_elem     Number of elements.
 * \param [out] value           Pointer to field values.
 *
 * \retval 0  OK
 * \retval <0 Error
 */

extern int
bcmcfg_field_array_set(int unit,
                       bcmltd_sid_t sid,
                       bcmltd_fid_t fid,
                       uint32_t start_idx,
                       uint32_t num_of_elem,
                       uint64_t *value);

/*!
 * \brief Return a symbolic field for a config logical table by id
 *
 * Get a symbolic value of a particular field for the given config
 * logical table. Return an error if the sid is not a config logical
 * table, fid is invalid, or the field was not symbolic (logical table
 * enum type).
 *
 * \param [in]  unit            Unit number.
 * \param [in]  sid             Logical table sid.
 * \param [in]  fid             Logical table fid.
 * \param [out] sym             Field symbol.
 *
 * \retval 0  OK
 * \retval <0 Error
 */

extern int
bcmcfg_field_symbol_get(int unit,
                        bcmltd_sid_t sid,
                        bcmltd_fid_t fid,
                        const char **sym);

/*!
 * \brief Get a scalar field for a config logical table by name
 *
 * Get a scalar value of a particular field for the given config
 * logical table by table and field string name.  Return an error if
 * the table_name is not a config logical table or if the table_name
 * or field_name is invalid.
 *
 * \param [in]  unit            Unit number.
 * \param [in]  table_name      Logical table name.
 * \param [in]  field_name      Logical table field name.
 * \param [out] value           Pointer to field value.
 *
 * \retval 0  OK
 * \retval <0 Error
 */

extern int
bcmcfg_field_name_value_get(int unit,
                            const char *table_name,
                            const char *field_name,
                            uint64_t *value);

/*!
 * \brief Get the values of an array field for a config logical table by name
 *
 * Get the valus of an array field of a particular field for the given config
 * logical table by table and field string name.  Return an error if
 * the table_name is not a config logical table or if the table_name
 * or field_name is invalid.
 *
 * \param [in]  unit            Unit number.
 * \param [in]  table_name      Logical table name.
 * \param [in]  field_name      Logical table field name.
 * \param [in]  start_idx       Start index of the field.
 * \param [in]  num_of_elem     Number of elements.
 * \param [out] value           Pointer to field value.
 * \param [out] actual          Number of elements actually obtained.
 *
 * \retval 0  OK
 * \retval <0 Error
 */

extern int
bcmcfg_field_name_value_array_get(int unit,
                                  const char *table_name,
                                  const char *field_name,
                                  uint32_t start_idx,
                                  uint32_t num_of_elem,
                                  uint64_t *value,
                                  size_t *actual);

/*!
 * \brief Set a scalar field value for a config logical table by name
 *
 * Set a scalar value of a particular field for the given config
 * logical table by table and field string name.  Return an error if
 * the table_name is not a config logical table or if the table_name
 * or field_name is invalid.
 *
 * \param [in]  unit            Unit number.
 * \param [in]  table_name      Logical table name.
 * \param [in]  field_name      Logical table field name.
 * \param [in]  value           Field value.
 *
 * \retval SHR_E_NONE           Operation is successful.
 * \retval SHR_E_UNAVAIL        Device/Table/Field does not exist.
 * \retval SHR_E_PARAM          Table is not a config table.
 */
extern int
bcmcfg_field_name_value_set(int unit,
                            const char *table_name,
                            const char *field_name,
                            uint64_t value);

/*!
 * \brief Set the values of an array field for a config logical table by name
 *
 * Set the values of an array field for the given config
 * logical table by table and field string name.  Return an error if
 * the table_name is not a config logical table or if the table_name
 * or field_name is invalid.
 *
 * \param [in]  unit            Unit number.
 * \param [in]  table_name      Logical table name.
 * \param [in]  field_name      Logical table field name.
 * \param [in]  start_idx       Logical table field start index.
 * \param [in]  num_of_elem     Number of elements.
 * \param [in]  value           Pointer to field values.
 *
 * \retval 0  OK
 * \retval <0 Error
 */

extern int
bcmcfg_field_name_value_array_set(int unit,
                                  const char *table_name,
                                  const char *field_name,
                                  uint32_t start_idx,
                                  uint32_t num_of_elem,
                                  uint64_t *value);

/*!
 * \brief Get a scalar field enum symbol for a config logical table by name
 *
 * Get a scalar enum symbol of a particular field for the given config
 * logical table by table and field string name.  Return an error if
 * the table_name is not a config logical table or if the table_name
 * or field_name is invalid.
 *
 * \param [in]  unit            Unit number.
 * \param [in]  table_name      Logical table name.
 * \param [in]  field_name      Logical table field name.
 * \param [out] symbol          Enum symbol.
 *
 * \retval 0  OK
 * \retval <0 Error
 */

extern int
bcmcfg_field_name_symbol_get(int unit,
                             const char *table_name,
                             const char *field_name,
                             const char **symbol);

/*!
 * \brief Set a scalar field enum symbol for a config logical table by name
 *
 * Set a scalar enum symbol of a particular field for the given config
 * logical table by table and field string name.  Return an error if
 * the table_name is not a config logical table or if the table_name
 * or field_name is invalid.
 *
 * \param [in]  unit            Unit number.
 * \param [in]  table_name      Logical table name.
 * \param [in]  field_name      Logical table field name.
 * \param [in]  symbol          Enum symbol.
 *
 * \retval SHR_E_NONE           Operation is successful.
 * \retval SHR_E_UNAVAIL        Device/Table/Field does not exist.
 * \retval SHR_E_PARAM          Table is not a config table.
 */
extern int
bcmcfg_field_name_symbol_set(int unit,
                             const char *table_name,
                             const char *field_name,
                             const char *symbol);
#endif /* BCMCFG_LT_H */
