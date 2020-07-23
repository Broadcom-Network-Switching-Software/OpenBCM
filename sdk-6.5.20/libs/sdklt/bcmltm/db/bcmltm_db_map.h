/*! \file bcmltm_db_map.h
 *
 * Logical Table Manager - Utility interfaces for LRD maps.
 *
 * This file contains routines to get information from the LRD map database.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMLTM_DB_MAP_H
#define BCMLTM_DB_MAP_H

#include <shr/shr_types.h>

#include <bcmlrd/bcmlrd_types.h>

/*! Check if map entry type is a direct field map */
#define BCMLTM_DB_MAP_ENTRY_TYPE_IS_DM(_e)              \
    (((_e) == BCMLRD_MAP_ENTRY_MAPPED_KEY) ||           \
     ((_e) == BCMLRD_MAP_ENTRY_MAPPED_VALUE))

/*! Check if map entry type is a fixed field map */
#define BCMLTM_DB_MAP_ENTRY_TYPE_IS_FIXED(_e)          \
    (((_e) == BCMLRD_MAP_ENTRY_FIXED_KEY) ||           \
     ((_e) == BCMLRD_MAP_ENTRY_FIXED_VALUE))

/*! Check if map entry type is a transform */
#define BCMLTM_DB_MAP_ENTRY_TYPE_IS_XFRM(_e)                            \
    (((_e) == BCMLRD_MAP_ENTRY_FWD_KEY_FIELD_XFRM_HANDLER) ||           \
     ((_e) == BCMLRD_MAP_ENTRY_FWD_VALUE_FIELD_XFRM_HANDLER) ||         \
     ((_e) == BCMLRD_MAP_ENTRY_REV_KEY_FIELD_XFRM_HANDLER) ||           \
     ((_e) == BCMLRD_MAP_ENTRY_REV_VALUE_FIELD_XFRM_HANDLER) ||         \
     ((_e) == BCMLRD_MAP_ENTRY_ALWAYS_REV_VALUE_FIELD_XFRM_HANDLER))

/*! Check if map entry type is a forward transform */
#define BCMLTM_DB_MAP_ENTRY_TYPE_IS_FWD_XFRM(_e)                        \
    (((_e) == BCMLRD_MAP_ENTRY_FWD_KEY_FIELD_XFRM_HANDLER) ||           \
     ((_e) == BCMLRD_MAP_ENTRY_FWD_VALUE_FIELD_XFRM_HANDLER))

/*! Check if map entry type is a key transform */
#define BCMLTM_DB_MAP_ENTRY_TYPE_IS_KEY_XFRM(_e)                        \
    (((_e) == BCMLRD_MAP_ENTRY_FWD_KEY_FIELD_XFRM_HANDLER) ||           \
     ((_e) == BCMLRD_MAP_ENTRY_REV_KEY_FIELD_XFRM_HANDLER))

/*!
 * \brief Map entry information.
 *
 * This information is used to identify a given map entry in
 * the LRD map data.
 */
typedef struct bcmltm_db_map_entry_s {
    uint32_t group_idx;
    uint32_t entry_idx;
} bcmltm_db_map_entry_t;

/*!
 * \brief Map entry list.
 *
 * This structure contains a list for LRD map entries.
 */
typedef struct bcmltm_db_map_entry_list_s {
    /* Reference to LRD map database (for easy access) */
    const bcmlrd_map_t *map;

    /*! Size of map entries array. */
    uint32_t max_entries;

    /*! Number of valid map entries in array. */
    uint32_t num_entries;

    /*! Array of map entries. */
    bcmltm_db_map_entry_t *entries;
} bcmltm_db_map_entry_list_t;


/*!
 * \brief Return LRD map group.
 *
 * This routine returns the LRD map group for given map entry identifier.
 * It assumes that the input identifier is valid within the LRD map.
 *
 * \param [in] map LRD map.
 * \param [in] map_entry Map entry identifier.
 *
 * \retval Pointer to LRD map group.
 */
static inline const bcmlrd_map_group_t *
bcmltm_db_map_lrd_group_get(const bcmlrd_map_t *map,
                            const bcmltm_db_map_entry_t *map_entry)
{
    return &map->group[map_entry->group_idx];
}

/*!
 * \brief Return LRD map entry.
 *
 * This routine returns the LRD map entry for given map entry identifier.
 * It assumes that the input identifier is valid within the LRD map.
 *
 * \param [in] map LRD map.
 * \param [in] map_entry Map entry identifier.
 *
 * \retval Pointer to LRD map entry.
 */
static inline const bcmlrd_map_entry_t *
bcmltm_db_map_lrd_entry_get(const bcmlrd_map_t *map,
                            const bcmltm_db_map_entry_t *map_entry)
{
    return &map->group[map_entry->group_idx].entry[map_entry->entry_idx];
}

/*!
 * \brief Initialize map entry list structure.
 *
 * This function initializes the given map entry list and allocates
 * the specified array size for the map entries.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical Table ID.
 * \param [in] max_entries Maximum array size.
 * \param [out] list List to initialize.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_db_map_entry_list_init(int unit, bcmlrd_sid_t sid,
                              uint32_t max_entries,
                              bcmltm_db_map_entry_list_t *list);

/*!
 * \brief Cleanup map entry list structure.
 *
 * This function deallocates any allocated memory in
 * the given map entry list.
 *
 * \param [in] list List to cleanup.
 */
extern void
bcmltm_db_map_entry_list_cleanup(bcmltm_db_map_entry_list_t *list);

/*!
 * \brief Create and initialize a map entry list structure.
 *
 * This function creates and initializes a map entry list and allocates
 * the specified array size for the map entries.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical Table ID.
 * \param [in] max_entries Maximum array size.
 * \param [out] list_ptr Returning pointer to list.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_db_map_entry_list_create(int unit, bcmlrd_sid_t sid,
                                uint32_t max_entries,
                                bcmltm_db_map_entry_list_t **list_ptr);

/*!
 * \brief Destroy map entry list structure.
 *
 * This function deallocates the given map entry list and
 * any allocated memory inside the list.
 *
 * \param [in] list List to destroy.
 */
extern void
bcmltm_db_map_entry_list_destroy(bcmltm_db_map_entry_list_t *list);

/*!
 * \brief Add map entry to list.
 *
 * This function adds the given map entry to the map entry list.
 *
 * NOTE: It assumes the given group and entry indexes are valid.
 *
 * \param [in,out] list List to add map entry to.
 * \param [in] group_idx LRD group index.
 * \param [in] entry_idx LRD entry index.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_db_map_entry_add(bcmltm_db_map_entry_list_t *list,
                        uint32_t group_idx,
                        uint32_t entry_idx);

/*!
 * \brief Add list of map entries to list.
 *
 * This function adds a list of map entries to the given map entry list.
 *
 * \param [in,out] list List to add map entries to.
 * \param [in] from_list List of map entries to add.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_db_map_entry_list_add(bcmltm_db_map_entry_list_t *list,
                             const bcmltm_db_map_entry_list_t *from_list);

/*!
 * \brief Find map entry for given map entry identifier.
 *
 * This routine finds and returns the map entry that matches
 * the map entry identifier in the given list.
 *
 * \param [in] list Map entry list to search.
 * \param [in] group_idx Group index to find.
 * \param [in] entry_idx Entry index to find.
 *
 * \retval Pointer to matching map entry, if found.
 * \retval NULL, if not found.
 */
extern const bcmltm_db_map_entry_t *
bcmltm_db_map_entry_find(const bcmltm_db_map_entry_list_t *list,
                         uint32_t group_idx,
                         uint32_t entry_idx);

/*!
 * \brief Get the total count of LRD map entries.
 *
 * This routine gets the count of map entries in all map groups
 * on the specified logical table.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical table ID.
 * \param [out] count Total map entries count.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_db_map_entries_count_get(int unit,
                                bcmlrd_sid_t sid,
                                uint32_t *count);

/*!
 * \brief Get default, minimum, and maximum values.
 *
 * This routine gets the default, minimum, and maximum values for
 * for the given logical field.
 *
 * It assumes that the arrays are large enough to hold the returning
 * values.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical table ID.
 * \param [in] fid Logical field ID.
 * \param [in] idx_count Size of arrays and expected field index count.
 * \param [out] default_values Default values.
 * \param [out] minimum_values Minimum values.
 * \param [out] maximum_values Maximum values.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_db_field_def_min_max_get(int unit,
                                bcmlrd_sid_t sid,
                                uint32_t fid,
                                uint32_t idx_count,
                                uint64_t *default_values,
                                uint64_t *minimum_values,
                                uint64_t *maximum_values);

/*!
 * \brief Find field mapping to destination field.
 *
 * This routine checks if the given logical table contains
 * a field mapping to the specified virtual destination field.
 *
 * The search includes the following types of field map entries:
 *   - Direct field maps (key and value)
 *   - Fixed field maps (key and value)
 *   - Transform field maps (key and value)
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical table ID.
 * \param [in] ptid Physical table ID.
 *                  If an INVALID PT ID is provided, search is
 *                  done in all PT groups (any PT ID).
 * \param [in] fid Destination field ID to find.
 *
 * \retval TRUE Map to destination field is found.
 * \retval FALSE Map to destination field is not found, or failure.
 */
extern bool
bcmltm_db_map_dst_field_find(int unit,
                             bcmlrd_sid_t sid,
                             bcmdrd_sid_t ptid,
                             uint32_t fid);

/*!
 * \brief Find given field in transform entry.
 *
 * This routine checks if the given field ID is part of the transform
 * field list.  Depending on the input argument, it looks into
 * either the source field list (API facing field) or
 * the destination field list (PT).
 *
 * \param [in] unit Unit number.
 * \param [in] entry LRD transform map entry to search.
 * \param [in] src Indicates to look into the source or destination fields.
 * \param [in] fid Field ID to find.
 *
 * \retval TRUE Field ID is found.
 * \retval FALSE Field ID is not found.
 */
extern bool
bcmltm_db_xfrm_field_find(int unit,
                          const bcmlrd_map_entry_t *entry,
                          bool src,
                          uint32_t fid);

/*!
 * \brief Find given field in any transform.
 *
 * This routine checks if the given field ID is
 * present in any transform of the specified logical table.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical Table ID.
 * \param [in] src If TRUE, field is source (API),
 *                 If FALSE, field is destination (PT).
 * \param [in] fid Field ID to find.
 *
 * \retval TRUE Field ID is found.
 * \retval FALSE Field ID is not found, or failure.
 */
extern bool
bcmltm_db_xfrm_all_field_find(int unit,
                              bcmlrd_sid_t sid,
                              bool src,
                              uint32_t fid);

/*!
 * \brief Sanity check for transform map data.
 *
 * This routine performs sanity checks for transform information
 * in the given LRD transform map entry.
 * If successful, it returns the corresponding transform handler.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical Table ID.
 * \param [in] entry LRD transform map entry to check.
 * \param [out] lta_handler Returning transform handler if successful.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_db_xfrm_check(int unit,
                     bcmlrd_sid_t sid,
                     const bcmlrd_map_entry_t *entry,
                     const bcmltd_xfrm_handler_t **lta_handler);

#endif /* BCMLTM_DB_MAP_H */
