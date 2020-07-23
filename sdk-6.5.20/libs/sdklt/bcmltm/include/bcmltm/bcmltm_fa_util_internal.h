/*! \file bcmltm_fa_util_internal.h
 *
 * Logical Table Manager Binary Trees.
 * Field Adaptation Nodes.
 *
 * A set of functions and definitions forming the nodes of
 * LTM FA binary trees.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMLTM_FA_UTIL_INTERNAL_H
#define BCMLTM_FA_UTIL_INTERNAL_H

#include <bcmltm/bcmltm_internal.h>
#include <bcmltm/bcmltm_util_internal.h>


/*!
 * \brief Log verbose for missing field.
 *
 * This macro logs a verbose message to indicate a missing field.
 *
 * \param [in] _u Unit number.
 * \param [in] _sid Table ID, logical or physical.
 * \param [in] _fid Field ID, logical or physical.
 * \param [in] _fidx Field index.
 * \param [in] _lt Indicates if this is a physical or logical table/field IDs.
 *                 If TRUE, the IDs are logical table and field IDs.
 *                 If FALSE, the IDs are physical table and field IDs.
 */
#define BCMLTM_LOG_VERBOSE_FIELD_MISSING(_u, _sid, _fid, _fidx, _lt)    \
    do {                                                                \
        const char *_table_name;                                        \
        const char *_field_name;                                        \
        const char *_table_sid_type;                                    \
                                                                        \
        _table_name = bcmltm_table_sid_to_name(_u, _sid, _lt);          \
        _field_name = bcmltm_field_fid_to_name(_u, _sid, _fid, _lt);    \
        _table_sid_type = bcmltm_table_sid_type_str(_lt);               \
                                                                        \
        LOG_VERBOSE(BSL_LOG_MODULE,                                     \
                   (BSL_META_U(_u,                                      \
                               "LTM %s(%s=%d): "                        \
                               "Field %s[%d](fid=%d) missing.\n"),      \
                    _table_name, _table_sid_type, _sid,                 \
                    _field_name, _fidx, _fid));                         \
    } while (0)

/*!
 * \brief Log verbose for supplied read-only field.
 *
 * This macro logs a verbose message to indicate a write to read-only field.
 *
 * \param [in] _u Unit number.
 * \param [in] _sid Table ID, logical or physical.
 * \param [in] _fid Field ID, logical or physical.
 * \param [in] _fidx Field index.
 * \param [in] _lt Indicates if this is a physical or logical table/field IDs.
 *                 If TRUE, the IDs are logical table and field IDs.
 *                 If FALSE, the IDs are physical table and field IDs.
 */
#define BCMLTM_LOG_VERBOSE_FIELD_RO_WRITE(_u, _sid, _fid, _fidx, _lt)   \
    do {                                                                \
        const char *_table_name;                                        \
        const char *_field_name;                                        \
        const char *_table_sid_type;                                    \
                                                                        \
        _table_name = bcmltm_table_sid_to_name(_u, _sid, _lt);          \
        _field_name = bcmltm_field_fid_to_name(_u, _sid, _fid, _lt);    \
        _table_sid_type = bcmltm_table_sid_type_str(_lt);               \
                                                                        \
        LOG_VERBOSE(BSL_LOG_MODULE,                                     \
                   (BSL_META_U(_u,                                      \
                               "LTM %s(%s=%d): "                        \
                               "Write to read-only field"               \
                               " %s[%d](fid=%d).\n"),                   \
                    _table_name, _table_sid_type, _sid,                 \
                    _field_name, _fidx, _fid));                         \
    } while (0)

/*!
 * \brief Log verbose for incomplete wide field.
 *
 * This macro logs a verbose message to indicate a wide field with
 * partial elements.
 *
 * \param [in] _u Unit number.
 * \param [in] _sid Table ID, logical or physical.
 * \param [in] _fid Field ID, logical or physical.
 * \param [in] _lt Indicates if this is a physical or logical table/field IDs.
 *                 If TRUE, the IDs are logical table and field IDs.
 *                 If FALSE, the IDs are physical table and field IDs.
 */
#define BCMLTM_LOG_VERBOSE_WIDE_FRAGMENT(_u, _sid, _fid, _lt)           \
    do {                                                                \
        const char *_table_name;                                        \
        const char *_field_name;                                        \
        const char *_table_sid_type;                                    \
                                                                        \
        _table_name = bcmltm_table_sid_to_name(_u, _sid, _lt);          \
        _field_name = bcmltm_field_fid_to_name(_u, _sid, _fid, _lt);    \
        _table_sid_type = bcmltm_table_sid_type_str(_lt);               \
                                                                        \
        LOG_VERBOSE(BSL_LOG_MODULE,                                     \
                   (BSL_META_U(_u,                                      \
                               "LTM %s(%s=%d): "                        \
                               "Partial wide field"                     \
                               " %s(fid=%d).\n"),                       \
                    _table_name, _table_sid_type, _sid,                 \
                    _field_name, _fid));                                \
    } while (0)

/*!
 * \brief Log verbose for field value out of bounds.
 *
 * This macro logs a verbose message to indicate a field value out of bounds.
 *
 * \param [in] _u Unit number.
 * \param [in] _sid Table ID, logical or physical.
 * \param [in] _fid Field ID, logical or physical.
 * \param [in] _fidx Field index.
 * \param [in] _lt Indicates if this is a physical or logical table/field IDs.
 *                 If TRUE, the IDs are logical table and field IDs.
 *                 If FALSE, the IDs are physical table and field IDs.
 * \param [in] _val Field value.
 * \param [in] _min Minimum field value.
 * \param [in] _max Maximum field value.
 */
#define BCMLTM_LOG_VERBOSE_FIELD_OOB(_u, _sid, _fid, _fidx, _lt, _val, _min, _max) \
    do {                                                                    \
        const char *_table_name;                                            \
        const char *_field_name;                                            \
        const char *_table_sid_type;                                        \
                                                                            \
        _table_name = bcmltm_table_sid_to_name(_u, _sid, _lt);              \
        _field_name = bcmltm_field_fid_to_name(_u, _sid, _fid, _lt);        \
        _table_sid_type = bcmltm_table_sid_type_str(_lt);                   \
                                                                            \
        LOG_VERBOSE(BSL_LOG_MODULE,                                         \
                    (BSL_META_U(_u,                                         \
                                "LTM %s(%s=%d): "                           \
                                "Field %s[%d](fid=%d)"                      \
                                " value 0x%" PRIx64 " out of bounds"        \
                                "(0x%" PRIx64 " - 0x%" PRIx64 ").\n"),      \
                     _table_name, _table_sid_type, _sid,                    \
                     _field_name, _fidx, _fid,                              \
                     _val, _min, _max));                                    \
    } while (0)

/*!
 * \brief Log verbose for field value that does not fit destination coordinates.
 *
 * This macro logs a verbose message to indicate a field value does not
 * fit within the destination bit coordinates.
 *
 * \param [in] _u Unit number.
 * \param [in] _sid Table ID, logical or physical.
 * \param [in] _fid Field ID, logical or physical.
 * \param [in] _fidx Field index.
 * \param [in] _lt Indicates if this is a physical or logical table/field IDs.
 *                 If TRUE, the IDs are logical table and field IDs.
 *                 If FALSE, the IDs are physical table and field IDs.
 * \param [in] _val Field value.
 * \param [in] _wbc Destination bit coordinates.
 */
#define BCMLTM_LOG_VERBOSE_FIELD_UNFIT(_u, _sid, _fid, _fidx, _lt, _val, _wbc) \
    do {                                                                     \
        const char *_table_name;                                             \
        const char *_field_name;                                             \
        const char *_table_sid_type;                                         \
                                                                             \
        _table_name = bcmltm_table_sid_to_name(_u, _sid, _lt);               \
        _field_name = bcmltm_field_fid_to_name(_u, _sid, _fid, _lt);         \
        _table_sid_type = bcmltm_table_sid_type_str(_lt);                    \
                                                                             \
        LOG_VERBOSE(BSL_LOG_MODULE,                                          \
                    (BSL_META_U(_u,                                          \
                                "LTM %s(%s=%d): "                            \
                                "Field %s[%d](fid=%d)"                       \
                                " value 0x%" PRIx64 " invalid fit"           \
                                "(minbit=%d maxbit=%d).\n"),                 \
                     _table_name, _table_sid_type, _sid,                     \
                     _field_name, _fidx, _fid,                               \
                     _val, (_wbc)->minbit, (_wbc)->maxbit));                 \
    } while (0)

/*!
 * \brief Validate the API-supplied field value for a specific field ID
 *
 * Check that the set bits of the API-supplied field value fit within
 * the valid bit size of the target Working Buffer coordinate.
 *
 * \param [in] wbc Working Buffer Coordinate
 * \param [in] field_val A uint32_t array holding the field value.
 *
 * \retval bool TRUE if field value fits within the WBC location.
 *              FALSE if the field value exceeds the WBC size.
 */
extern bool
bcmltm_field_fit_check(bcmltm_wb_coordinate_t *wbc, uint32_t *field_val);

/*!
 * \brief Retrieve the API-supplied field value for a specific field ID
 *
 * Search the API sourced field value list for the field value matching
 * the supplied field ID.  Return a pointer to the field list structure
 * for that field value if found.  Otherwise return NULL.
 *
 * \param [in] in_fields List of API-provided field value structures.
 * \param [in] api_field_id The field ID to seek within the in_fields
 *             field value list.
 * \param [in] field_idx The field array index for this field ID to
 *             match in the field value list.
 *
 * \retval non-NULL Pointer to API field list structure matching field ID
 * \retval NULL Matching field value not found for provided field ID.
 */
extern bcmltm_field_list_t *
bcmltm_api_find_field(bcmltm_field_list_t *in_fields,
                      uint32_t api_field_id,
                      uint32_t field_idx);

/*!
 * \brief Copy a uint32_t array field value into the Working Buffer.
 *
 * Copy the specified uint32_t field value array into the Working Buffer
 * at the location and field size specified.
 *
 * \param [in] wbc Working Buffer coordinate - offset, minbit, maxbit
 * \param [in] field_val uint32_t array containing the field value.
 * \param [out] ltm_buffer Pointer to the Working Buffer for this entry.
 *
 * \retval None.
 */
extern void
bcmltm_field_value_into_wbc(bcmltm_wb_coordinate_t *wbc,
                            uint32_t *field_val,
                            uint32_t *ltm_buffer);

/*!
 * \brief Retrieve a uint32_t array field value from the Working Buffer.
 *
 * Copy the specified field value from the Working Buffer into
 * the uint32_t field value array provided.
 *
 * \param [in] wbc Working Buffer coordinate - offset, minbit, maxbit
 * \param [out] field_val uint32_t array containing the field value.
 * \param [in] ltm_buffer Pointer to the Working Buffer for this entry.
 *
 * \retval None.
 */
extern void
bcmltm_field_value_from_wbc(bcmltm_wb_coordinate_t *wbc,
                            uint32_t *field_val,
                            uint32_t *ltm_buffer);

/*!
 * \brief Copy a uint32_t array field value per the PTM field map.
 *
 * Copy the specified uint32_t field value array into the Working Buffer
 * at the location and field size specified.
 *
 * \param [in] field_map PTM field structure pointer including WBC.
 * \param [in] field_val uint32_t array containing the field value.
 * \param [out] ltm_buffer Pointer to the Working Buffer for this entry.
 *
 * \retval None.
 */
extern void
bcmltm_field_value_into_buffer(bcmltm_field_map_t *field_map,
                               uint32_t *field_val,
                               uint32_t *ltm_buffer);

/*!
 * \brief Retrieve a uint32_t array field value per the PTM field map.
 *
 * Copy the specified field value from the Working Buffer into
 * the uint32_t field value array provided.
 *
 * \param [in] field_map PTM field structure pointer including WBC.
 * \param [out] field_val uint32_t array containing the field value.
 * \param [in] ltm_buffer Pointer to the Working Buffer for this entry.
 *
 * \retval None.
 */
extern void
bcmltm_field_value_from_buffer(bcmltm_field_map_t *field_map,
                               uint32_t *field_val,
                               uint32_t *ltm_buffer);

/*!
 * \brief Copy from a field structure into the Working Buffer.
 *
 * With the provided field map, copy the field data contained in the
 * Working Buffer field structure to the indicated WB coordinate.
 *
 * \param [in] field_map General field map structure pointer.
 * \param [out] ltm_buffer Pointer to the Working Buffer for this entry.
 *
 * \retval None.
 */
extern void
bcmltm_field_map_into_buffer(bcmltm_field_map_t *field_map,
                             uint32_t *ltm_buffer);

/*!
 * \brief Retrieve a field value from the Working Buffer.
 *
 * With the provided field map, copy the field data at the indicated
 * WB coordinate into the Working Buffer field structure.
 *
 * \param [in] field_map General field structure pointer.
 * \param [in] ltm_buffer Pointer to the Working Buffer for this entry.
 *
 * \retval None.
 */
extern void
bcmltm_field_map_from_buffer(bcmltm_field_map_t *field_map,
                             uint32_t *ltm_buffer);

/*!
 * \brief Retrieve a LTA field value from the Working Buffer.
 *
 * With the provided field map, copy the LTA field data at the indicated
 * WB coordinate into the Working Buffer field structure.
 * Complete the field structure with map information.
 *
 * \param [in] field_map General field structure pointer.
 * \param [in] ltm_buffer Pointer to the Working Buffer for this entry.
 *
 * \retval None.
 */
extern void
bcmltm_field_map_lta_from_buffer(bcmltm_field_map_t *field_map,
                                 uint32_t *ltm_buffer);

/*!
 * \brief Search a Logical Table Adapter I/O structure.
 *
 * Traverse a LTA I/O structure to locate the field
 * matching the given API field ID and index.  Return the field
 * value structure pointer if found, NULL if not.
 *
 * \param [in] lta_fields LTA fields input/output structure pointer
 * \param [in] field_id Field identifier value.
 * \param [in] field_idx The field array index for this field ID to
 *             match in the field value list.
 *
 * \retval Not-NULL Matching field value structure.
 * \retval NULL No matching field value found.
 */
extern bcmltm_field_list_t *
bcmltm_lta_find_field(const bcmltd_fields_t *lta_fields,
                      uint32_t field_id,
                      uint32_t field_idx);

#endif /* BCMLTM_FA_UTIL_INTERNAL_H */
