/*! \file bcmltm_db_dm_util.h
 *
 * Logical Table Manager - Direct Mapped utility functions.
 *
 * This file contains utility routines used in the construction
 * of information for direct map tables.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMLTM_DB_DM_UTIL_H
#define BCMLTM_DB_DM_UTIL_H

#include <shr/shr_types.h>

#include <bcmdrd/bcmdrd_types.h>
#include <bcmlrd/bcmlrd_types.h>

#include <bcmltm/bcmltm_md_internal.h>
#include <bcmltm/bcmltm_wb_types_internal.h>

#include "bcmltm_db_map.h"

/*!
 * \brief Physical Table Information.
 *
 * This is used to construct the LTM metadata.  For instance, the
 * information indicates the Working Buffer content to use for the
 * PTM interface.
 */
typedef struct bcmltm_db_pt_s {
    /*! Physical table ID. */
    bcmdrd_sid_t sid;

    /*!
     * Bit flags to denote physical table parameters:
     *     BCMLTM_PT_OP_FLAGS_xxx
     * These are used to indicate the content and format
     * for the PTM parameter list and the entry data sections of the
     * generic working buffer layout.
     */
    uint32_t flags;

    /*! The number of PTM operations to perform for this PT. */
    uint32_t num_ops;

    /*! Working buffer block ID for this PT. */
    bcmltm_wb_block_id_t wb_block_id;
} bcmltm_db_pt_t;

/*!
 * \brief Information for all directly mapped PTs in a table.
 *
 * This information is used to construct the LTM metadata.
 */
typedef struct bcmltm_db_dm_pt_info_s {
    /*! Number of mapped physical tables in a table. */
    uint32_t num_pts;

    /*!
     * Variable length array of PT info.
     * The index for a given PT is the same index used to construct
     * the PT information in bcmltm_pt_list_t.
     */
    bcmltm_db_pt_t pts[];
} bcmltm_db_dm_pt_info_t;

/*!
 * \brief Field map for direct mapped field.
 *
 * Contains field map information from parsed LRD map entries of type:
 *     BCMLRD_MAP_ENTRY_MAPPED_KEY
 *     BCMLRD_MAP_ENTRY_MAPPED_VALUE
 */
typedef struct bcmltm_map_field_mapped_s {
    /*! API-facing field ID. */
    uint32_t src_field_id;

    /*! The index of the field in an array. */
    uint32_t src_field_idx;

    /*! The default value for this field. */
    uint64_t default_value;

    /*! The minimum value for this field. */
    uint64_t minimum_value;

    /*! The maximum value for this field. */
    uint64_t maximum_value;

    /*! Destination field ID (PT). */
    uint32_t dst_field_id;

    /*! PTM WB block for this field map. */
    const bcmltm_wb_block_t *ptm_wb_block;

    /*!
     * PT entry index.
     *
     * Index LTs: Used to indicate the PT operation on a given PT ID.
     *            Numbering must be compact starting at 0.
     * Keyed LTs: Used for extended physical fields [0..3].
     */
    uint16_t pt_entry_idx;

    /*! The WBC minimum bit position of this field. */
    uint16_t wbc_minbit;

    /*! The WBC maximum bit position of this field. */
    uint16_t wbc_maxbit;

    /*! Next field map. */
    struct bcmltm_map_field_mapped_s *next;
} bcmltm_map_field_mapped_t;

/*!
 * \brief Field map list for direct mapped fields LT to PT.
 *
 * Contains list of field maps information from parsed LRD map entries of type:
 *     BCMLRD_MAP_ENTRY_MAPPED_KEY
 *     BCMLRD_MAP_ENTRY_MAPPED_VALUE
 */
typedef struct bcmltm_map_field_mapped_list_s {
    /*! Size of array. */
    uint32_t max;

    /*! Number of valid fields in array. */
    uint32_t num;

    /*! Array of field map. */
    bcmltm_map_field_mapped_t *field_maps;
} bcmltm_map_field_mapped_list_t;

/*!
 * \brief LT mapping for fixed fields.
 *
 * Contains mapping entries of type:
 *     BCMLRD_MAP_ENTRY_FIXED_KEY
 *     BCMLRD_MAP_ENTRY_FIXED_VALUE
 */
typedef struct bcmltm_map_field_fixed_list_s {
    /*! Size of array. */
    uint32_t max;

    /*! Number of valid fields in array. */
    uint32_t num;

    /*! Array of fixed field map. */
    bcmltm_fixed_field_t *field_maps;
} bcmltm_map_field_fixed_list_t;

/*!
 * \brief Physical field information for Field Transform.
 *
 * Contains field wbc information for Field Transform.
 */
typedef struct bcmltm_xfrm_dst_field_s {
    /*! Array of Field IDs. */
    uint32_t field_id;

    /*!
     * PT entry index.
     *
     * Index LTs: Used to indicate the PT operation on a given PT ID.
     *            Numbering must be compact starting at 0.
     * Keyed LTs: Used for extended physical fields [0..3].
     */
    uint16_t pt_entry_idx;

    /*! Physical field information. */
    bcmltm_wb_coordinate_t wbc;

} bcmltm_xfrm_dst_field_t;

/*!
 * \brief All physical fields information for Field Transform.
 *
 * Contains all field wbc information for Field Transform.
 */
typedef struct bcmltm_xfrm_dst_fields_s {
    /*! Physical table ID of destination fields. */
    bcmdrd_sid_t ptid;

    /*! Number of fields. */
    uint32_t num;

    /*! Array of physical field(wbc) information. */
    bcmltm_xfrm_dst_field_t *field;

} bcmltm_xfrm_dst_fields_t;

/*!
 * \brief LT mapping information for Transform.
 *
 * Contains LT map information for a field transform.
 */
typedef struct bcmltm_map_xfrm_s {
    /*! Map entry type. */
    bcmlrd_map_entry_type_t entry_type;

    /*! Transform handler. */
    bcmltd_xfrm_handler_t handler;

    /*! Transform Destination fields(wbc). */
    bcmltm_xfrm_dst_fields_t dst;
} bcmltm_map_xfrm_t;

/*!
 * \brief LT mapping for Transform functions.
 *
 * Contains mapping entries of type:
 *     BCMLRD_MAP_ENTRY_FWD/REV_KEY/VALUE_FIELD_XFRM_HANDLER.
 */
typedef struct bcmltm_map_field_xfrm_list_s {
    /*! Size of array. */
    uint32_t max;

    /*! Number of valid xfrm in array. */
    uint32_t num;

    /*! Array of xfrms. */
    bcmltm_map_xfrm_t *xfrms;
} bcmltm_map_field_xfrm_list_t;

/*!
 * \brief LT entries mapping information for direct PT map groups.
 *
 * This structure is used to hold information parsed from the LRD map entries.
 *
 * The information is then copied into the static internal
 * data structure which will contain the adequate array
 * size for the different field map lists.  The static internal data
 * structure lt_info holds information used and referenced by the LTM
 * metadata, such as cookies for the different FA and EE nodes.
 */
typedef struct bcmltm_db_dm_map_entries_s {
    /*! Indicates if PT suppress is available for table. */
    bool pt_suppress;

    /*! Indicates if table contains a map to track index. */
    bool track_index_map;

    /*! Key field direct maps. */
    bcmltm_map_field_mapped_list_t keys;

    /*! Value field direct maps. */
    bcmltm_map_field_mapped_list_t values;

    /*! Fixed key field maps. */
    bcmltm_map_field_fixed_list_t fixed_keys;

    /*! Fixed value field maps. */
    bcmltm_map_field_fixed_list_t fixed_values;

    /*! Forward Key Transform handler. */
    bcmltm_map_field_xfrm_list_t fwd_key_xfrms;

    /*! Forward Value Transform handler. */
    bcmltm_map_field_xfrm_list_t fwd_value_xfrms;

    /*! Reverse Key Transform handler. */
    bcmltm_map_field_xfrm_list_t rev_key_xfrms;

    /*! Reverse Value Transform handler. */
    bcmltm_map_field_xfrm_list_t rev_value_xfrms;
} bcmltm_db_dm_map_entries_t;

/*!
 * \brief Direct Mapped Arguments.
 *
 * This structure contains base information for a direct mapped table
 * and can be used as an input argument to routines to pass various
 * data and help reduce the number of input function arguments.
 */
typedef struct bcmltm_db_dm_arg_s {
    /*! Table attributes. */
    const bcmltm_table_attr_t *attr;

    /*! Information for mapped PTs. */
    const bcmltm_db_dm_pt_info_t *pt_info;

    /*! Indicates if PT suppress is available for LT. */
    bool pt_suppress;

    /*! Indicates if track index is supplied by map. */
    bool track_index_map;

    /*! LT Private working buffer offsets. */
    const bcmltm_wb_lt_pvt_offsets_t *lt_pvt_wb_offsets;

} bcmltm_db_dm_arg_t;


/*!
 * \brief Find the PTM working buffer block for given Physical Table ID.
 *
 * This function finds and returns the PTM working buffer block
 * for the given Physical Table ID.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical table ID.
 * \param [in] pt_info PT information.
 * \param [in] ptid Physical table ID to look for.
 *
 * \retval Pointer to PTM working buffer block.
 *         NULL if not found.
 */
extern const bcmltm_wb_block_t *
bcmltm_db_dm_ptm_wb_block_find(int unit, bcmlrd_sid_t sid,
                               const bcmltm_db_dm_pt_info_t *pt_info,
                               bcmdrd_sid_t ptid);

/*!
 * \brief Create PT information for given logical table.
 *
 * Create the information for PTs mapped in the given logical table.
 *
 * The information is derived from parsing the LRD direct map group
 * kind: BCMLRD_MAP_PHYSICAL.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID (logical table).
 * \param [in] attr Table attributes.
 * \param [out] pt_info_ptr Returning pointer to table mapped PTs information.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_db_dm_pt_info_create(int unit,
                            bcmlrd_sid_t sid,
                            const bcmltm_table_attr_t *attr,
                            bcmltm_db_dm_pt_info_t **pt_info_ptr);

/*!
 * \brief Destroy PT information.
 *
 * Free allocated memory for PT information.
 *
 * \param [in] pt_info Pointer to PT info.
 */
extern void
bcmltm_db_dm_pt_info_destroy(bcmltm_db_dm_pt_info_t *pt_info);

/*!
 * \brief Get the Direct Mapped mapping information for a given table ID.
 *
 * Get the LT Direct Mapped mapping information for a given table ID.
 *
 * This routine parses the LRD entry map information for:
 *     Map Group Kind: BCMLRD_MAP_CUSTOM
 *     Map Entry Type: { BCMLRD_MAP_ENTRY_MAPPED_VALUE,
 *                       BCMLRD_MAP_ENTRY_FIXED_VALUE }
 *
 * Returning arrays will be allocated the max possible size in order
 * to walk the LRD map entries just once.  A later stage will
 * copy this information into the static data structure using
 * the right array sizes (the number of elements will be known by the
 * end of parsing).
 *
 * This routine allocates memory so caller must call corresponding routine
 * to free memory when this is no longer required.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [in] attr Table attributes.
 * \param [in] pt_info PT information for the LT.
 * \param [out] lt_map Returning LT mapping entries information.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_db_dm_map_entries_parse(int unit, bcmlrd_sid_t sid,
                               const bcmltm_table_attr_t *attr,
                               const bcmltm_db_dm_pt_info_t *pt_info,
                               bcmltm_db_dm_map_entries_t *lt_map);

/*!
 * \brief Cleanup LT mapping information.
 *
 * Free any internal memory allocated for the LT mapping information.
 *
 * \param [out] lt_map LT mapping information to clean.
 */
extern void
bcmltm_db_dm_map_entries_cleanup(bcmltm_db_dm_map_entries_t *lt_map);

/*!
 * \brief Get the PT index offsets for given table.
 *
 * This function gets the PTM working buffer offsets for all the PT
 * indexes in a direct map table.
 *
 * Note that some fields may be mapped to both, a PT index
 * and also a PT port or table instance memory parameter.  In this case,
 * the field is treated as a 'index' (from the index tracking perspective)
 * and would be included in this output.
 *
 * This function returns offsets only on PT op 0.  This is intended
 * to be used for building the track index elements that LTM internally
 * composes.  This method is used for Index LTs where there is only 1 PT op
 * per PT ID.
 *
 * LTs with multiple PT ops per PT ID require a transform supplied
 * track index (where track field elements do not apply).
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical Table ID.
 * \param [in] pt_list PT list.
 * \param [in] offset_max Array size of offset.
 * \param [out] offset Returning offsets.
 * \param [out] num_offset Actual number of offsets returned.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_db_dm_pt_index_offsets_get(int unit, bcmlrd_sid_t sid,
                                  const bcmltm_pt_list_t *pt_list,
                                  uint32_t offset_max,
                                  uint32_t *offset, uint32_t *num_offset);

/*!
 * \brief Get the PT memory parameter offsets for given table.
 *
 * This function gets the PTM working buffer offsets for all the PT
 * memory parameters.
 *
 * Note that some fields may be mapped to both, a PT index
 * and also a PT port or table instance memory parameter.  In this case,
 * the field is treated as a 'index' (from the index tracking perspective)
 * and is not included in this output.
 *
 * This function returns offsets only on PT op 0.  This is intended
 * to be used for building the track index elements that LTM internally
 * composes.  This method is used for Index LTs where there is only 1 PT op
 * per PT ID.
 *
 * LTs with multiple PT ops per PT ID require a transform supplied
 * track index (where track field elements do not apply).
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical Table ID.
 * \param [in] pt_list PT list.
 * \param [in] offset_max Array size of offset.
 * \param [out] offset Returning offsets.
 * \param [out] num_offset Actual number of offsets returned.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_db_dm_pt_mem_param_offsets_get(int unit, bcmlrd_sid_t sid,
                                      const bcmltm_pt_list_t *pt_list,
                                      uint32_t offset_max,
                                      uint32_t *offset, uint32_t *num_offset);

/*!
 * \brief Get the PT SID selector offsets for given table.
 *
 * This function gets the PTM working buffer offsets for all the PT
 * SID selectors.
 *
 * This function returns offsets only on PT op 0.  This is intended
 * to be used for building the track index elements that LTM internally
 * composes.  This method is used for Index LTs where there is only 1 PT op
 * per PT ID.
 *
 * LTs with multiple PT ops per PT ID require a transform supplied
 * track index (where track field elements do not apply).
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical Table ID.
 * \param [in] pt_list PT list.
 * \param [in] offset_max Array size of offset.
 * \param [out] offset Returning offsets.
 * \param [out] num_offset Actual number of offsets returned.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_db_dm_pt_sid_sel_offsets_get(int unit, bcmlrd_sid_t sid,
                                    const bcmltm_pt_list_t *pt_list,
                                    uint32_t offset_max,
                                    uint32_t *offset, uint32_t *num_offset);

/*!
 * \brief Get the PT suppress offset for given table.
 *
 * This function gets the working buffer offset for the PT suppress status
 * for the specified logical table if applicable.
 *
 * If PT suppress is not applicable, an invalid offset is returned.
 *
 * The PT suppress capability is available if this status is supplied
 * by a transform.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical Table ID.
 * \param [in] pt_suppress Indicates if PT suppress is available for LT.
 * \param [out] offset PT suppress offset, if applicable.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_db_dm_pt_suppress_offset_get(int unit,
                                    bcmlrd_sid_t sid,
                                    bool pt_suppress,
                                    uint32_t *offset);
/*!
 * \brief Check if track index is supplied by a map.
 *
 * This routine checks if the track index is supplied by a transform map
 * in the given logical table.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical Table ID.
 *
 * \retval TRUE, if track index is supplied by a map.
 * \retval FALSE, if track index is not supplied by a map or failure.
 */
static inline bool
bcmltm_db_dm_is_track_index_map(int unit,
                                bcmlrd_sid_t sid)
{
    return bcmltm_db_xfrm_all_field_find(unit, sid,
                                         FALSE,
                                         BCMLRD_FIELD_TRACK_INDEX);
}

/*!
 * \brief Check if PT suppress is available.
 *
 * This routine checks if PT suppress is supplied by a transform map
 * in the given logical table.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical Table ID.
 *
 * \retval TRUE, if PT suppress is available.
 * \retval FALSE, if PT suppress is not applicable or failure.
 */
static inline bool
bcmltm_db_dm_is_pt_suppress(int unit,
                            bcmlrd_sid_t sid)
{
    return bcmltm_db_xfrm_all_field_find(unit, sid,
                                         FALSE,
                                         BCMLRD_FIELD_PT_SUPPRESS);
}

#endif /* BCMLTM_DB_DM_UTIL_H */
