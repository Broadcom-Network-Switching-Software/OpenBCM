/*! \file bcmltm_db_core_internal.h
 *
 * Logical Table Manager - Core functions.
 *
 * This file contains routines that provide general information
 * for logical tables derived from the LRD table definition and maps.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMLTM_DB_CORE_INTERNAL_H
#define BCMLTM_DB_CORE_INTERNAL_H

#include <shr/shr_types.h>
#include <bcmltd/bcmltd_table.h>
#include <bcmlrd/bcmlrd_types.h>
#include <bcmltm/bcmltm_types_internal.h>
#include <bcmltm/bcmltm_internal.h>
#include <bcmltm/bcmltm_db_types_internal.h>
#include <bcmltm/bcmltm_info_internal.h>


/*!
 * \brief Map groups count for a logical table.
 */
typedef struct bcmltm_db_map_group_count_s {
    /*! Direct map groups count. */
    uint32_t dm;

    /*! Customer Table Handler map groups count. */
    uint32_t cth;

    /*! LTM Managed map groups count. */
    uint32_t ltm;

    /*! Field Validation map groups count. */
    uint32_t fv;

    /*! Table Commit map groups count. */
    uint32_t tc;
} bcmltm_db_map_group_count_t;


/*!
 * \brief Get table info for a given table.
 *
 * Utility wrapper function for the LRD table get routine.  This wrapper
 * returns an error if the table information is NULL.
 *
 * \param [in] sid Logical table ID.
 * \param [out] tbl Returning table information.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_db_lrd_table_get(bcmlrd_sid_t sid,
                        const bcmlrd_table_rep_t **tbl);

/*!
 * \brief Get the table map groups count.
 *
 * This routine gets the count for the various LRD map groups
 * on the specified logical table.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical table ID.
 * \param [out] count Map groups count.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_db_map_group_count_get(int unit, bcmlrd_sid_t sid,
                              bcmltm_db_map_group_count_t *count);

/*!
 * \brief Parse the LRD table attributes for a given LT.
 *
 * This routine parses the table attributes defined in the LT
 * map file.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [out] attr Table attributes.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_db_table_attr_parse(int unit, bcmlrd_sid_t sid,
                           bcmltm_table_attr_t *attr);

/*!
 * \brief Get the total number of LRD table symbol IDs.
 *
 * Get the total number of LRD table IDs.  This count represents
 * the total number of logical tables defined for all devices.
 * As such, some sids may not be valid or applicable on a given device.
 *
 * For instance:
 *   Device-A: valid SIDs are 0, 1, 2, 3
 *   Device-B: valid SIDs are 0, 3
 *   Device-C: valid SIDs are 4, 5
 *
 * For above example, the count should return 6.
 *
 * \retval Total number of LRD table symbol IDs.
 */
extern uint32_t
bcmltm_db_table_count_get(void);

/*!
 * \brief Get the Logical Table type.
 *
 * This function gets the logical table type as specified in
 * the table definition.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical table ID.
 * \param [out] type Table type.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static inline int
bcmltm_db_core_table_type_get(int unit,
                              bcmlrd_sid_t sid,
                              bcmltm_table_type_t *type)
{
    return bcmltm_info_table_type_get(unit, sid, type);
}

/*!
 * \brief Get table prepopulate attribute.
 *
 * This routine gets the table prepopulate attribute.
 *
 * If prepopulate is TRUE, all logical entries are marked as inuse
 * during initialization.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical table ID.
 * \param [out] prepopulate TRUE, if table prepopulate is set.
 *                          FALSE, if table prepopulate is not set.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_db_table_prepopulate_get(int unit,
                                bcmlrd_sid_t sid,
                                bool *prepopulate);

/*!
 * \brief Get number of fields for a logical table.
 *
 * This routine returns the number of field IDs and
 * field elements (id,idx) for the specified logical table.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical Table ID.
 * \param [out] num_fid Number of field IDs (fid).
 * \param [out] num_fid_idx Number of field elements (fid,idx).
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_db_field_count_get(int unit, bcmlrd_sid_t sid,
                          uint32_t *num_fid, uint32_t *num_fid_idx);

/*!
 * \brief Get number of Key fields for a logical table.
 *
 * This routine returns the number Key field IDs and
 * field elements (id,idx) for the specified logical table.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical Table ID.
 * \param [out] num_fid Number of Key field IDs (fid).
 * \param [out] num_fid_idx Number of Key field elements (fid,idx).
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_db_field_key_count_get(int unit, bcmlrd_sid_t sid,
                              uint32_t *num_fid, uint32_t *num_fid_idx);


/*!
 * \brief Get number of Value fields for a logical table.
 *
 * This routine returns the number Value field IDs and
 * field elements (id,idx) for the specified logical table.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical Table ID.
 * \param [out] num_fid Number of Value field IDs (fid).
 * \param [out] num_fid_idx Number of Value field elements (fid,idx).
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_db_field_value_count_get(int unit, bcmlrd_sid_t sid,
                                uint32_t *num_fid, uint32_t *num_fid_idx);


/*!
 * \brief Get list of field IDs and index count for a logical table.
 *
 * This routine returns the list of field IDs and field index count
 * for the specified logical table.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical Table ID.
 * \param [in] list_max Size of allocated list.
 * \param [out] list List of field IDs and index count.
 * \param [out] num Number of fields returned.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_db_field_list_get(int unit, bcmlrd_sid_t sid,
                         uint32_t list_max,
                         bcmltm_fid_idx_t *list, uint32_t *num);

/*!
 * \brief Get list of Key field IDs and index count for a logical table.
 *
 * This routine returns the list of Key field IDs and field index count
 * for the specified logical table.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical Table ID.
 * \param [in] list_max Size of allocated list.
 * \param [out] list List of Key field IDs and index count.
 * \param [out] num Number of Key fields returned.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_db_field_key_list_get(int unit, bcmlrd_sid_t sid,
                             uint32_t list_max,
                             bcmltm_fid_idx_t *list, uint32_t *num);

/*!
 * \brief Get list of Value field IDs and index count for a logical table.
 *
 * This routine returns the list of Value field IDs and field index count
 * for the specified logical table.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical Table ID.
 * \param [in] list_max Size of allocated list.
 * \param [out] list List of Value field IDs and index count.
 * \param [out] num Number of Value fields returned.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_db_field_value_list_get(int unit, bcmlrd_sid_t sid,
                               uint32_t list_max,
                               bcmltm_fid_idx_t *list, uint32_t *num);

/*!
 * \brief Get local field ID from global field ID.
 *
 * This routine gets the local field ID for the specified logical
 * table given a global field ID.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical table ID.
 * \param [in] gfid Global field ID.
 * \param [out] fid Local field ID.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_db_gfid_to_fid(int unit,
                      bcmltd_sid_t sid,
                      bcmltd_gfid_t gfid,
                      bcmltd_fid_t *fid);

/*!
 * \brief Get global field ID from local field ID.
 *
 * This routine gets the global field ID for the specified logical
 * table given a local field ID.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical table ID.
 * \param [in] fid Local field ID.
 * \param [out] gfid Global field ID.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_db_fid_to_gfid(int unit,
                      bcmltd_sid_t sid,
                      bcmltd_fid_t fid,
                      bcmltd_gfid_t *gfid);

/*!
 * \brief Get the maximum field ID for a given logical table.
 *
 * This routine gets the maximum local field ID for the specified
 * logical table.
 *
 * Field IDs are local to a table and are dense, this
 * is, field IDs are [0..fid_max].  However, depending on the table map,
 * a field ID within that range may or may not be mapped (valid).  For
 * unmapped field, functions that take a field ID would normally return
 * SHR_E_UNAVAIL.
 *
 * If a table has no fields, the function returns SHR_E_UNAVAIL.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical table ID.
 * \param [out] fid_max Maximum logical field ID.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_UNAVAIL Logical table has no fields.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_db_fid_max_get(int unit, bcmlrd_sid_t sid,
                      bcmlrd_fid_t *fid_max);

/*!
 * \brief Convert a field into field index min/max bit positions.
 *
 * Convert a field into required number of field elements
 * necessary to hold the entire field width.   The resulting data
 * is a set of arrays for the min and max bit positions
 * for each of the field indexes.
 *
 * This function is normally called on physical fields where the caller
 * needs to know the min and max bit positions for each field element
 * on a wide field.
 *
 * Field:  field[0]:  [min_0 .. max_0]
 *         field[1]:  [min_1 .. max_1]
 *         field[n]:  [min_n .. max_n]
 *
 * \param [in] unit Unit number.
 * \param [in] fid Field ID.
 * \param [in] minbit Minimum bit position of field.
 * \param [in] maxbit Maximum bit position of field.
 * \param [in] list_max Array size.
 * \param [out] fidx_minbit Returning minimum bit positions for field indexes.
 * \param [out] fidx_maxbit Returning maximum bit positions for field indexes.
 * \param [out] num_fidx Returning actual number of field indexes.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_db_field_to_fidx_min_max(int unit, uint32_t fid,
                                uint16_t minbit, uint16_t maxbit,
                                uint32_t list_max,
                                uint16_t *fidx_minbit,
                                uint16_t *fidx_maxbit,
                                uint32_t *num_fidx);

/*!
 * \brief Get the minimum, maximum, and default values for a scalar field.
 *
 * This routine gets the minimum, maximum, and default values for
 * the specified logical scalar field (width <= 64).
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical table ID.
 * \param [in] fid Logical field ID.
 * \param [out] min Minimum value, if not null.
 * \param [out] max Maximum value, if not null.
 * \param [out] default_value Default value, if not null.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_db_field_scalar_values_get(int unit, bcmlrd_sid_t sid,
                                  bcmlrd_fid_t fid,
                                  uint64_t *min, uint64_t *max,
                                  uint64_t *default_value);

/*!
 * \brief Check if logical field is a key field.
 *
 * This routine checks if the given logical field is a key field.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical table ID.
 * \param [in] fid Logical field ID.
 * \param [out] is_key TRUE, if field is a key field.
 *                     FALSE, if field is a value field.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_db_field_is_key(int unit,
                       bcmlrd_sid_t sid,
                       bcmlrd_fid_t fid,
                       bool *is_key);

/*!
 * \brief Check if logical field is a read only field.
 *
 * This routine checks if the given logical field is a read only field.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical table ID.
 * \param [in] fid Logical field ID.
 * \param [out] is_read_only TRUE, if field is read only.
 *                           FALSE, if field is not read only.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_db_field_is_read_only(int unit, bcmlrd_sid_t sid,
                             bcmlrd_fid_t fid,
                             bool *is_read_only);

/*!
 * \brief Check if logical field is an enumeration.
 *
 * This routine checks if the given logical field is an enumeration
 * (has associated symbols).
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical table ID.
 * \param [in] fid Logical field ID.
 * \param [out] is_enum TRUE, if field is an enumeration.
 *                      FALSE, if field is not an enumeration.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_db_field_is_enum(int unit, bcmlrd_sid_t sid,
                        bcmlrd_fid_t fid,
                        bool *is_enum);

/*!
 * \brief  Check if field ID is a virtual field.
 *
 * This routine checks if the given field ID is a virtual field.
 *
 * A virtual field is a special field indicator that is primarily
 * used to specify a destination that does not have a corresponding
 * HW field.  For instance, this is used to indicate PT keys (index,
 * port, instance).
 *
 * Note if a field ID is not virtual, the field can be a logical field
 * or a HW field.  Logical field IDs and HW field IDs are in the positive
 * number range.  The caller context needs to have other information
 * to differentiate between these two.
 *
 * \param [in] unit Unit number.
 * \param [in] fid Field ID.
 * \param [out] is_virtual TRUE, if field is a virtual field.
 *                         FALSE, if field is not a virtual field (thus,
 *                         ID can be a logical field ID or a HW field ID).
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_db_field_is_virtual(int unit, uint32_t fid,
                           bool *is_virtual);

/*!
 * \brief  Check if logical field is a field selector.
 *
 * This routine checks if the given logical field is a field selector.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical table ID.
 * \param [in] fid Logical field ID.
 * \param [out] is_selector TRUE, if field is a field selector.
 *                          FALSE, if field is not a field selector.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_db_field_is_selector(int unit, bcmlrd_sid_t sid,
                            bcmlrd_fid_t fid,
                            bool *is_selector);

/*!
 * \brief Get the number of selection groups for a logical field.
 *
 * This routine gets the number of selection groups for which
 * the specified logical field is a member.
 *
 * The selection group count reflects only those
 * selections where the field presence (valid or not) is subject
 * to a selection.
 *
 * The returning selection group count can be used by
 * TABLE_FIELD_INFO.NUM_GROUP which indicates if a field is
 * is subject to a selection group (num_group > 0).  If so,
 * then the selection group number is [0 .. <num_group-1>].
 *
 * For example, if a selected field is specified in all selector values
 * of a field selector, then the selection group count would be zero because
 * logically the field is always present.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical table ID.
 * \param [in] fid Logical field ID.
 * \param [out] num_group Number of selection groups for field.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_db_field_select_group_count_get(int unit, bcmlrd_sid_t sid,
                                       bcmlrd_fid_t fid,
                                       uint32_t *num_group);

/*!
 * \brief Indicate if logical field maps to a PT index.
 *
 * This routine returns TRUE if the given logical field is a key
 * and maps to a PT index.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical Table ID.
 * \param [in] fid Logical Field ID.
 *
 * \retval TRUE, if given LT field maps to a key PT index.
 * \retval FALSE, otherwise.
 */
extern bool
bcmltm_db_field_key_pt_index_mapped(int unit, bcmlrd_sid_t sid,
                                    uint32_t fid);

/*!
 * \brief Check if logical key field is allocatable on an IwA LT.
 *
 * This routine checks if given logical key field is allocatable (optional)
 * on an IwA table (key fields are always required in any other table types).
 *
 * Index with Allocation LTs have allocatable key fields and possibly
 * required key fields.  Allocatable key fields can be absent during
 * the INSERT operation, and if so, they are allocated by LTM.
 *
 * - Allocatable keys:  Those that map to the PT index.
 * - Required keys:     Those that do not map to PT index, such as
 *                      PT port, PT instance, PT table select, Field selector.
 *
 * Assumptions:
 * - Field ID is a valid key field ID (invalid input returns FALSE).
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical table ID.
 * \param [in] fid Logical field ID.
 *
 * \retval TRUE, if key field is optional (allocatable) for INSERT on IwA LT.
 * \retval FALSE, if key field is required for INSERT or failure.
 */
extern bool
bcmltm_db_field_key_is_alloc(int unit, bcmlrd_sid_t sid,
                                   uint32_t fid);

/*!
 * \brief Check if given transform contains allocatable keys.
 *
 * This routine checks if given key transform contains
 * optional key fields (allocatable).
 *
 * Index with Allocation LTs have optional key fields and possibly
 * required key fields.  Optional key fields can be absent during
 * the INSERT operation, and if so, they are allocated by LTM.
 *
 * - Allocatable keys:  Those that map to the PT index.
 * - Required keys:     Those that do not map to PT index, such as
 *                      PT port, PT instance, PT table select, Field selector.
 *
 * \param [in] attr Table attributes.
 * \param [in] xfrm_type Transform type.
 * \param [in] xfrm_arg Transform argument.
 *
 * \retval TRUE, if transform contains allocatable key fields.
 * \retval FALSE, if transform does not contain allocatable key fields.
 */
extern bool
bcmltm_db_xfrm_key_is_alloc(const bcmltm_table_attr_t *attr,
                            int xfrm_type,
                            const bcmltd_transform_arg_t *xfrm_arg);

#endif /* BCMLTM_DB_CORE_INTERNAL_H */
