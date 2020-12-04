/*! \file bcmltm_db_table_internal.h
 *
 * Logical Table Manager - Logical Table Information.
 *
 * This file contains interfaces to provide information on
 * logical tables tailored for the TABLE LTs.  However,
 * these interfaces can be used for other purposes as well.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMLTM_DB_TABLE_INTERNAL_H
#define BCMLTM_DB_TABLE_INTERNAL_H

#include <shr/shr_types.h>

#include <bcmlrd/bcmlrd_types.h>

#include <bcmltm/bcmltm_types_internal.h>


/*!
 * \brief Table Information.
 *
 * This structure contains information about the table definition
 * and mapping attributes.
 */
typedef struct bcmltm_table_info_s {
    /*! Table Type. */
    bcmltm_table_type_t type;

    /*! Table Map Type. */
    bcmltm_table_map_type_t map_type;

    /*! Operating mode: Modeled or Interactive. */
    bcmltm_table_mode_t mode;

    /*! True if table is read only. */
    bool read_only;

    /*! The number of key fields in this LT. */
    uint32_t num_keys;

    /*! The number of fields in this LT. */
    uint32_t num_fields;

    /*!
     * Maximum number of entries.
     *
     * This is the greatest size achievable
     * This is the maximum number of entries which may be installed
     * for the table in the current device variant.
     *
     * Index tables: this is calculated using the logical field limits.
     * Keyed tables: this is calculated using the PT limits.
     *
     * Note:
     * The entry limit (ENTRY_LIMIT) reflects the greatest number
     * of entries under the current table configuration.  This
     * entry_limit may be smaller than the entry_maximum.
     * This is the case for tables where the resource limit can be
     * resized by some reconfiguration.
     *
     * For fixed sizes tables, the entry_limit is the same
     * as this entry_maximum.
     */
    uint32_t entry_maximum;

    /*! The number of companion resource info tables. */
    uint32_t num_resource_info;

    /*! Supported opcodes. */
    bcmltm_opcodes_t opcodes;
} bcmltm_table_info_t;

/*!
 * \brief Table Resource Information.
 *
 * This structure contains the associated companion resource information
 * per index for a table.
 */
typedef struct bcmltm_table_resource_info_s {
    /*! Resource information table ID. */
    bcmlrd_sid_t ri_sid;
} bcmltm_table_resource_info_t;

/*!
 * \brief Field Information.
 *
 * This structure contains information about the field definition
 * and mapping attributes.
 */
typedef struct bcmltm_table_field_info_s {
    /*! True if this is a key field. */
    bool key;

    /*! True if this is a symbolic field. */
    bool symbol;

    /*! Minimum limit allowed. */
    uint64_t min_limit;

    /*! Maximum limit allowed. */
    uint64_t max_limit;

    /*! Default value. */
    uint64_t default_value;

    /*! True if this is an optional key field. */
    bool alloc_key_field;

    /*! True if this is a read only field. */
    bool read_only;

    /*! Number of array elements in this field. */
    uint32_t array_depth;

    /*! Number of scalar elements needed for field. */
    uint32_t elements;

    /*! Width of field in bits. */
    uint32_t width;

    /*! True if this is a selector field. */
    bool selector;

    /*!
     * Number of selection groups this field is a member of.
     * If this is non-zero, this field is a member of a field
     * selection (overlay) group.
     * If this is zero, this field is not part of any field selection group.
     */
    uint32_t num_select_group;
} bcmltm_table_field_info_t;

/*!
 * \brief Field Select Information.
 *
 * This structure contains information related to field selection
 * and is used to describe the association of fields and field selectors
 * used for field overlays and PDDs.
 */
typedef struct bcmltm_table_field_select_s {
    /*! Selector local field ID. */
    bcmlrd_fid_t selector_fid;

    /*! Selector global field ID. */
    bcmltd_gfid_t selector_gfid;

    /*! True if selector is a scalar, false if selector is an enum. */
    bool selector_scalar;

    /*! Scalar value of selector (valid if selector is a scalar). */
    uint64_t selector_scalar_value;

    /*! Enum value of selector (valid if selector is an enum). */
    uint64_t selector_enum_value;
} bcmltm_table_field_select_t;

/*!
 * \brief Get detailed information for a logical table.
 *
 * This routine returns information for definition and mapping attributes
 * for the specified logical table.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical table ID.
 * \param [out] info Table information.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_db_table_info_get(int unit, bcmlrd_sid_t sid,
                         bcmltm_table_info_t *info);

/*!
 * \brief Get resource information for a logical table.
 *
 * This routine returns the associated companion resource information
 * for the specified logical table and resource information index.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical table ID.
 * \param [in] idx Resource information index.
 * \param [out] info Resource information.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_db_table_resource_info_get(int unit,
                                  bcmlrd_sid_t sid,
                                  uint32_t idx,
                                  bcmltm_table_resource_info_t *info);

/*!
 * \brief Get detailed information for a logical field.
 *
 * This routine returns information for definition and mapping attributes
 * for the specified logical field.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical table ID.
 * \param [in] fid Logical field ID.
 * \param [out] info Field information.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_db_table_field_info_get(int unit, bcmlrd_sid_t sid,
                               bcmlrd_fid_t fid,
                               bcmltm_table_field_info_t *info);

/*!
 * \brief Get detailed information for a logical field by global ID.
 *
 * This routine returns information for definition and mapping attributes
 * for the specified logical field using the global field ID.
 *
 * Note that this interface uses the global field ID, rather than
 * the regular field ID (where the ID is local to the table).
 * A global field ID is a unique field identifier among all logical tables.
 * Global field IDs are used when the API field exposes enums.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical table ID.
 * \param [in] gfid Logical global field ID.
 * \param [out] info Field information.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_db_table_field_info_get_by_gfid(int unit, bcmlrd_sid_t sid,
                                       bcmltd_gfid_t gfid,
                                       bcmltm_table_field_info_t *info);

/*!
 * \brief Get field selection information for a logical field.
 *
 * This routine returns field selection information for the specified
 * logical field and selection group number.
 *
 * Field selection information is used to describe the association of
 * fields and field selectors used for field overlays and PDDs.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical table ID.
 * \param [in] fid Logical field ID.
 * \param [in] group Field selection group number.
 * \param [out] info Field selection information.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_db_table_field_select_get(int unit, bcmlrd_sid_t sid,
                                 bcmlrd_fid_t fid,
                                 uint32_t group,
                                 bcmltm_table_field_select_t *info);

/*!
 * \brief Get field selection information for a logical field by global ID.
 *
 * This routine returns field selection information for the specified
 * logical field (using global field ID) and selection group number.
 *
 * Field selection information is used to describe the association of
 * fields and field selectors used for field overlays and PDDs.
 *
 * Note that this interface uses the global field ID, rather than
 * the regular field ID (where the ID is local to the table).
 * A global field ID is a unique field identifier among all logical tables.
 * Global field IDs are used when the API field exposes enums.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical table ID.
 * \param [in] gfid Logical global field ID.
 * \param [in] group Field selection group number.
 * \param [out] info Field selection information.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_db_table_field_select_get_by_gfid(int unit, bcmlrd_sid_t sid,
                                         bcmltd_gfid_t gfid,
                                         uint32_t group,
                                         bcmltm_table_field_select_t *info);

#endif /* BCMLTM_DB_TABLE_INTERNAL_H */
