/*! \file bcmltm_db_types_internal.h
 *
 * Logical Table Manager - Database Types Definitions.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMLTM_DB_TYPES_INTERNAL_H
#define BCMLTM_DB_TYPES_INTERNAL_H

#include <shr/shr_types.h>

#include <bcmltm/bcmltm_types_internal.h>



/*!
 * \brief Field Transform Type : Forward Key Transform.
 */
#define BCMLTM_DB_XFRM_TYPE_FWD_KEY_TRANSFORM   \
    BCMLTM_FIELD_XFRM_TYPE_FWD_KEY

/*!
 * \brief Field Transform Type : Forward Value Transform.
 */
#define BCMLTM_DB_XFRM_TYPE_FWD_VALUE_TRANSFORM  \
    BCMLTM_FIELD_XFRM_TYPE_FWD_VALUE

/*!
 * \brief Field Transform Type : Reverse Key Transform.
 */
#define BCMLTM_DB_XFRM_TYPE_REV_KEY_TRANSFORM    \
    BCMLTM_FIELD_XFRM_TYPE_REV_KEY

/*!
 * \brief Field Transform Type : Reverse Value Transform.
 */
#define BCMLTM_DB_XFRM_TYPE_REV_VALUE_TRANSFORM  \
    BCMLTM_FIELD_XFRM_TYPE_REV_VALUE

/*!
 * \brief Maximum number of field selection groups.
 *
 * This number is used to define a static array size and avoid
 * allocating memory in the following structure.  It can be
 * increased, if needed.
 */
#define BCMLTM_DB_FIELD_SELECT_MAX_COUNT  50

/*!
 * \brief Field selection core information.
 *
 * This structure contains core information for a field selection
 * group.
 */
typedef struct bcmltm_db_fs_core_s {
    /*!
     * Field Selection ID.
     *
     * This ID uniquely identifies a field selection group
     * within a logical table.
     */
    bcmltm_field_selection_id_t selection_id;

    /*!
     * Field selector type.
     *
     * Indicates if the field selector is a key or value
     * (or none for the unconditional selection group).
     */
    bcmltm_field_selector_type_t selector_type;

    /*!
     * Field selection level
     *   Level 0: no field selector (unconditional).
     *   Level 1: has 0 parents.
     *   ...
     *   Level n: has (n-1) parents.
     */
    uint32_t level;

} bcmltm_db_fs_core_t;

/*!
 * \brief Field selections core information.
 *
 * This structure contains core information for all the field selection
 * groups for a logical table.
 *
 * This information can be used to obtain the selection IDs
 * for a given level.
 */
typedef struct bcmltm_db_fs_core_info_s {
    /*!
     * Number of field selection levels for the logical table.
     *
     * A level may have more than 1 selection group.
     */
    uint32_t num_levels;

    /*!
     * Number of selections in array that have field selectors.
     *
     * This number is to differentiate from 'num_selections',
     * which includes both, the unconditional selection
     * group (no field selector) and selection groups with field selectors.
     */
    uint32_t num_with_selectors;

    /*!
     * Number of field selections in array.
     *
     * This is the total number of selections in the array
     * which may include the unconditional group (no field selector).
     */
    uint32_t num_selections;

    /*! Array of field selections. */
    bcmltm_db_fs_core_t selections[BCMLTM_DB_FIELD_SELECT_MAX_COUNT];

} bcmltm_db_fs_core_info_t;

#endif /* BCMLTM_DB_TYPES_INTERNAL_H */
