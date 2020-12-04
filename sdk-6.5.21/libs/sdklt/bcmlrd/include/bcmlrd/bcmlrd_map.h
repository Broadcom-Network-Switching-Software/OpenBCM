/*! \file bcmlrd_map.h
 *
 * Logical Table Mapping
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMLRD_MAP_H
#define BCMLRD_MAP_H

#include <sal/sal_types.h>
#include <bcmlrd/bcmlrd_types.h>
#include <bcmlrd/bcmlrd_internal.h>

/*!
 * \brief Get mapping info for a table and device.
 *
 * Get the mapping info for a table and device.
 *
 * \param [in]  unit     Unit number.
 * \param [in]  sid      Table ID.
 * \param [out] map      Map info.
 *
 * \retval 0  OK
 *
 * \retval <0 Error - invalid unit, invalid sid, sid not defined for
 * device, mapping not available, etc.
 */
extern int
bcmlrd_map_get(int unit, bcmlrd_sid_t sid, bcmlrd_map_t const **map);

/*!
 * \brief List of all tables referenced by a logical table.
 *
 * Get a list of all  tables referenced by a logical table.
 *
 * \param [in]  unit     Unit number.
 * \param [in]  sid      Logical Table ID.
 * \param [in]  count    Number of Destination Table Desc allocated.
 * \param [out] dst      List of Destination Table Descriptors.
 * \param [out] actual   Number of Table Descriptors returned.
 *
 * \retval 0  OK
 *
 * \retval <0 Error - invalid unit, invalid sid, sid not defined for
 * device, mapping not available, etc.
 */

extern int
bcmlrd_mapped_dest_get(int unit,
                       bcmlrd_sid_t sid,
                       size_t count,
                       bcmlrd_table_desc_t *dst,
                       size_t *actual);

/*!
 * \brief Get map table attribute value
 *
 * Get map table attribute value
 *
 * \param [in]  unit     Unit number.
 * \param [in]  sid      Table id.
 * \param [in]  name     Table attribute name.
 * \param [out] value    Table attribute value.
 *
 * \retval 0  OK
 *
 * \retval <0 Error - invalid unit,
 */
extern int
bcmlrd_map_table_attr_get(int unit,
                          bcmlrd_sid_t sid,
                          bcmlrd_map_table_attr_name_t name,
                          uint32_t *value);

/*!
 * \brief Get count of given map table attribute.
 *
 * This function returns the number of instances for the given
 * map table attribute.
 *
 * \param [in]  unit        Unit number.
 * \param [in]  sid         Table ID.
 * \param [in]  name        Table attribute name.
 * \param [out] count       Count for table attribute name.
 *
 * \retval SHR_E_NONE       Success.
 * \retval !SHR_E_NONE      Failure.
 */
extern int
bcmlrd_map_table_attr_count_get(int unit,
                                bcmlrd_sid_t sid,
                                bcmlrd_map_table_attr_name_t name,
                                size_t *count);

/*!
 * \brief Get map table attribute value list.
 *
 * This function gets the list of values for the given
 * map table attribute.
 *
 * \param [in]  unit        Unit number.
 * \param [in]  sid         Table ID.
 * \param [in]  name        Table attribute name.
 * \param [in]  list_max    Number of attribute values allocated.
 * \param [out] value_list  List of table attribute values.
 * \param [out] num_value   Number of table attribute values returned.
 *
 * \retval SHR_E_NONE       Success.
 * \retval !SHR_E_NONE      Failure.
 */
extern int
bcmlrd_map_table_attr_list_get(int unit,
                               bcmlrd_sid_t sid,
                               bcmlrd_map_table_attr_name_t name,
                               size_t list_max,
                               uint32_t *value_list,
                               size_t *num_value);

/*!
 * \brief Get map table attribute value for given index.
 *
 * This function gets the value for the given map table attribute
 * in the specified index.
 *
 * \param [in]  unit        Unit number.
 * \param [in]  sid         Table ID.
 * \param [in]  name        Table attribute name.
 * \param [in]  idx         Table attribute index.
 * \param [out] value       Table attribute value.
 *
 * \retval SHR_E_NONE       Success.
 * \retval !SHR_E_NONE      Failure.
 */
extern int
bcmlrd_map_table_attr_get_by_idx(int unit,
                                 bcmlrd_sid_t sid,
                                 bcmlrd_map_table_attr_name_t name,
                                 uint32_t idx,
                                 uint32_t *value);

/*!
 * \brief Function to check if the logical field in unmapped.
 *
 * This routine checks if the logical field is unmapped
 * for the given map and fid.
 *
 * \param [in]  map             Pointer to map structure.
 * \param [in]  fid             Logical Field ID.
 *
 * \retval true if the field is unmapped.
 * \retval false if the field is not unmapped.
 */

static inline bool
bcmlrd_map_field_is_unmapped(const bcmlrd_map_t *map, bcmlrd_fid_t fid)
{
    return bcmlrd_field_is_unmapped_internal(map, fid);
}

/*!
 * \brief Return the mapped field data.
 *
 * Get a pointer to the mapped field data for the given unit, sid, and fid.
 *
 * \param [in]  unit            Unit number.
 * \param [in]  sid             Logical Table symbol ID.
 * \param [in]  fid             Logical Field ID.
 * \param [out] field           Pointer to returned field data.
 *
 * \retval SHR_E_NONE           Returned field data.
 * \retval SHR_E_UNAVAIL        unit/table/field not found.
 */
extern int bcmlrd_field_get(int unit,
                            bcmlrd_sid_t sid,
                            bcmlrd_fid_t fid,
                            const bcmlrd_field_data_t **field);

#endif /* BCMLRD_MAP_H */
