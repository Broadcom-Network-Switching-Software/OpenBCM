/*! \file bcmltm_db_field_select.h
 *
 * Logical Table Manager - Information for Field Selection.
 *
 * This file contains routines to construct information for
 * field selections.
 *
 * This information is derived from the LRD Field Selector maps.
 * and is used to construct various LTM metadata.
 *
 * The metadata created is used for the following LTM driver flow:
 * (field select opcode driver version)
 *     API <-> API Cache <-> [VALIDATE, XFRM] <-> PTM
 *
 * This driver flow can be applied to tables with or without field selections.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMLTM_DB_FIELD_SELECT_H
#define BCMLTM_DB_FIELD_SELECT_H

#include <shr/shr_types.h>
#include <shr/shr_pb_local.h>

#include <bcmltm/bcmltm_types_internal.h>

#include "bcmltm_db_map.h"

/*!
 * \brief Field Selection Map.
 *
 * This structure contains information for a field selection map
 * within a field selection.
 *
 * A field selection map defines the set of map entries, such as
 * direct field maps, field transforms, or fixed fields, that
 * corresponds to a given selector value within the field selection.
 */
typedef struct bcmltm_db_field_selection_map_s {
    /*! Selector value. */
    uint64_t selector_value;

    /*!
     * Selector map index.
     *
     * This is the index assigned to each of the selector values.
     * Map indexes start at 1.  A map index of 0 is invalid and
     * is reserved for unconditional maps (maps with no field selector).
     */
    uint32_t map_index;

    /*! Map entries list this field select map. */
    bcmltm_db_map_entry_list_t *map_entry_list;
} bcmltm_db_field_selection_map_t;

/*!
 * \brief Field Selection.
 *
 * This structure contains information for a field selection group.
 *
 * A field selection defines a field selector, within a select hierarchy,
 * along with its associated selector values and corresponding
 * maps.
 *
 * This structure can also be used for unconditional maps (maps
 * without field selectors).  If so, a special field selection ID is
 * provided.
 */
typedef struct bcmltm_db_field_selection_s {
    /*!
     * Field Selection ID.
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

    /*! Selector field ID. */
    uint32_t selector_fid;

    /*!
     * Parent field selection ID.
     * If selection has a parent selector, this is the field selection
     * ID of its parent.
     */
    bcmltm_field_selection_id_t parent_selection_id;

    /*! Parent selector field ID. */
    uint32_t parent_selector_fid;

    /*!
     * Parent selector value.
     * If applicable, this is the value of the parent selector
     * required to enable this selection group.
     */
    uint64_t parent_selector_value;

    /*! Parent map index. */
    uint32_t parent_map_index;

    /*!
     * Field selection level
     *   Level 0: no field selector (unconditional).
     *   Level 1: has 0 parents.
     *   ...
     *   Level n: has (n-1) parents.
     */
    uint32_t level;

    /*! Max number of select maps allocated in array. */
    uint32_t max_maps;

    /*! Number of valid select maps in this field selection. */
    uint32_t num_maps;

    /*! List of select maps. */
    bcmltm_db_field_selection_map_t **maps;

} bcmltm_db_field_selection_t;

/*!
 * \brief Field Selections List.
 *
 * This structure contains information for all the field selections
 * in a given logical table.
 *
 * This list may also include a special selection group within the list,
 * with level 0, that is intended to contain information for those maps
 * without any field selector (unconditional).
 */
typedef struct bcmltm_db_field_selection_list_s {
    /* Reference to LRD map select database (for easy access) */
    const bcmlrd_field_selector_data_t *selector_data;

    /*!
     * Number of field selection levels
     *   Level 0: no field selector (unconditional).
     *   Level 1: has 0 parents
     *   ...
     *   Level n: has (n-1) parents
     */
    uint32_t num_levels;

    /*! Max number of field selections allocated in array. */
    uint32_t max_selections;

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
    bcmltm_db_field_selection_t **selections;

} bcmltm_db_field_selection_list_t;


/*!
 * \brief Create the field selection list for a logical table.
 *
 * This routine creates the list of field selections for the given
 * logical table ID.
 *
 * The list contains those field selections that has selected map entries.
 * This means that a field selection will not be part of this list if
 * the selection does not have any map entry within its hierarchy.
 * Those field selections (type BCMLRD_FIELD_SELECTOR_TYPE_FIELD) have
 * no effect within the LTM field select logic (so, they are informational).
 *
 * If the logical table has at least one field selection, an additional
 * special selection group is added to the list which is intended
 * to contain all map entries with NO field selector.
 * This unconditional select group is identified with a special
 * selection ID and is always (and the only selection) at level 0.
 *
 * The availability of this additional unconditional selection (maps without
 * selection) helps streamline the metadata construction in later stages.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical Table ID.
 * \param [out] list_ptr Returning pointer to field selection list.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_db_field_selection_list_create(int unit,
                                      bcmlrd_sid_t sid,
                                      bcmltm_db_field_selection_list_t
                                      **list_ptr);

/*!
 * \brief Destroy field selection list structure.
 *
 * This function deallocates the given field selection list and
 * any allocated memory inside the list.
 *
 * \param [in] list Field selection list to destroy.
 */
extern void
bcmltm_db_field_selection_list_destroy(bcmltm_db_field_selection_list_t *list);

/*!
 * \brief Create a field selection group given a map entry list.
 *
 * This routine creates a field selection group with the given map
 * entry list.  This is useful for reusing existent metadata creation
 * routines that takes a selection group.
 *
 * The resulting field selection group is set as follows:
 * - Contains all map entries given in the input map entry list.
 * - Selection group is unconditional.
 * - There is only one select map.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical table ID.
 * \param [in] list List of map entries to populate field selection group.
 * \param [out] selection_ptr Returning pointer to new selection.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_db_field_selection_me_create(int unit,
                                bcmlrd_sid_t sid,
                                const bcmltm_db_map_entry_list_t *list,
                                bcmltm_db_field_selection_t **selection_ptr);
/*!
 * \brief  Destroy given field selection.
 *
 * This routine deallocates any memory allocated for the
 * field selection.
 *
 * \param [in] selection Field selection to destroy.
 */
extern void
bcmltm_db_field_selection_destroy(bcmltm_db_field_selection_t *selection);

/*!
 * \brief Display field selection list.
 *
 * This routine displays given field selection list structure.
 *
 * \param [in] list Field selection list to display.
 * \param [in,out] pb Print buffer to populate output data.
 */
extern void
bcmltm_db_field_selection_list_dump(const bcmltm_db_field_selection_list_t
                                    *list,
                                    shr_pb_t *pb);

#endif /* BCMLTM_DB_FIELD_SELECT_H */
