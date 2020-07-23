/*! \file bcmltm_db_dm_track.c
 *
 * Logical Table Manager - Information for Track Index.
 *
 * This file contains routines to construct information for
 * track index in direct map index logical tables.
 *
 * This information is derived from the LRD Field Transform map entries
 * in the Physical (direct map) map groups.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_types.h>
#include <shr/shr_debug.h>
#include <shr/shr_util.h>
#include <sal/sal_libc.h>
#include <bsl/bsl.h>

#include <bcmltd/bcmltd_types.h>
#include <bcmlrd/bcmlrd_types.h>
#include <bcmlrd/bcmlrd_map.h>

#include <bcmltm/bcmltm_internal.h>
#include <bcmltm/bcmltm_types_internal.h>
#include <bcmltm/bcmltm_util_internal.h>
#include <bcmltm/bcmltm_md_internal.h>
#include <bcmltm/bcmltm_wb_lt_internal.h>
#include <bcmltm/bcmltm_wb_lt_pvt_internal.h>
#include <bcmltm/bcmltm_lta_intf_internal.h>
#include <bcmltm/bcmltm_db_core_internal.h>

#include "bcmltm_db_map.h"
#include "bcmltm_db_md_util.h"
#include "bcmltm_db_field_select.h"
#include "bcmltm_db_dm_util.h"
#include "bcmltm_db_dm_field.h"
#include "bcmltm_db_dm_track.h"

/*******************************************************************************
 * Local definitions
 */

/* Debug log target definition */
#define BSL_LOG_MODULE BSL_LS_BCMLTM_METADATA

/*! Get mask for specified number of bits. */
#define BIT_MASK(_bits)    ((1 << (_bits)) - 1)

/*
 * Track Index Maximum Fields
 *
 * Maximum number of track index field elements:
 *   PT INDEX
 *   PT MEM PARAM (PORT or TABLE INST)
 *   PT SID SEL
 *   FIELD SEL
 */
#define TRACK_INDEX_MAX_FIELDS    4

/*!
 * \brief Track Field Information.
 *
 * This structure contains information for a track field element
 * used as a component to calculate the track index.
 *
 * This data is used to construct the metadata for the track index
 * fields.
 */
typedef struct track_field_info_s {
    /*! Track index field element offset. */
    uint32_t offset;

    /*! Track index field element maximum value. */
    uint32_t max;

    /*! Track index field element bit length. */
    uint32_t bitlen;

    /*! TRUE if track index field element is allocatable. */
    bool allocatable;
} track_field_info_t;


/* Assume max number of track index element map entries per PT */
#define TRACK_INDEX_MAX_MAPS    50

/*******************************************************************************
 * Private functions
 */

/*!
 * \brief Get the number of bits to hold a given value.
 *
 * This routine returns the number of bits to hold a given value.
 *
 * \param [in] value Value.
 *
 * \retval Number of bits for value.
 */
static uint32_t
bitlen_get(uint32_t value)
{
    return (shr_util_log2(value) + 1);
}

/*!
 * \brief Get track field internal information.
 *
 * This routine gets the track field information used to
 * construct the track index field metadata.
 *
 * \param [in] offset Track field element offset.
 * \param [in] max Maximum value of the track field element.
 * \param [in] alloc Indicates if track field element is allocatable.
 * \param [out] field Track field information.
 */
static void
track_field_info_get(uint32_t offset, uint32_t max, bool alloc,
                     track_field_info_t *field)
{
    field->offset = offset;
    field->max = max;
    field->bitlen = bitlen_get(max);
    field->allocatable = alloc;

    return;
}

/*!
 * \brief Get the track index field metadata.
 *
 * This routine gets the track index field metadata.
 *
 * \param [in] info Track field internal information.
 * \param [in] shift Shift applied to the track field element.
 * \param [out] field Track index field metadata.
 */
static void
track_index_field_get(track_field_info_t *info,
                      uint32_t shift,
                      bcmltm_track_index_field_t *field)
{
    field->field_offset = info->offset;
    field->field_mask = BIT_MASK(info->bitlen);
    field->field_shift = shift;
    field->allocatable = info->allocatable;

    return;
}

/*!
 * \brief Get working buffer offset and maximum value for a PT index.
 *
 * This routine gets the working buffer offset for a map to a PT index.
 *
 * If table has no mapped PT index, the returning offset is invalid.
 *
 * This routine also returns a list of map entries that correspond
 * to the chosen track PT field.  This list can be used to construct
 * other metadata required to convert the decoded track PT fields
 * to API fields.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [in] attr Table attributes.
 * \param [in] pt_list PT list.
 * \param [in] in_ptid If valid, search must be within this PT ID.
 * \param [out] map_entry_list List of map entries chosen for track PT fields.
 * \param [out] out_ptid PT ID chosen for track PT fields.
 * \param [out] field_offset Offset of PT index, if found.
 *                           Otherwise, returning offset is invalid.
 * \param [out] field_max Maximum value of PT index, if found.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_NOT_FOUND No mapped PT index found.
 * \retval !SHR_E_NONE Failure.
 */
static int
track_field_pt_index_get(int unit, bcmlrd_sid_t sid,
                         const bcmltm_table_attr_t *attr,
                         const bcmltm_pt_list_t *pt_list,
                         bcmdrd_sid_t in_ptid,
                         bcmltm_db_map_entry_list_t *map_entry_list,
                         bcmdrd_sid_t *out_ptid,
                         uint32_t *field_offset,
                         uint32_t *field_max)
{
    const bcmlrd_map_t *map = NULL;
    const bcmlrd_map_group_t *group = NULL;
    const bcmlrd_map_entry_t *entry = NULL;
    uint32_t group_idx;
    uint32_t entry_idx;
    bcmdrd_sid_t ptid = BCMDRD_INVALID_ID;
    uint32_t fid = 0;
    bool pt_found = FALSE;
    bool me_found = FALSE;

    SHR_FUNC_ENTER(unit);

    *out_ptid = BCMDRD_INVALID_ID;
    *field_offset = BCMLTM_WB_OFFSET_INVALID;
    *field_max = 0;

    SHR_IF_ERR_EXIT
        (bcmltm_db_map_entry_list_init(unit, sid,
                                       TRACK_INDEX_MAX_MAPS,
                                       map_entry_list));

    /* Get LRD mapping */
    SHR_IF_ERR_EXIT
        (bcmlrd_map_get(unit, sid, &map));

    /*
     * Find all entry maps within a PT that target the PT index.
     * Usually, the LT index corresponds to the PT index:
     *   __INDEX
     *
     * Need to get all entries since there may be more than
     * one API key that map to PT subfields.  The complete list of maps
     * is required in order to translate the PT key back to API keys
     * during a key allocation or key traverse operation.
     * Example:
     *     F1: PT1.__INDEX[1..0]
     *     F2: PT1.__INDEX[3..2]
     *
     * Also, there are cases where a field is mapped to both __INDEX and also
     * to a memory parameter (i.e. __PORT, __INSTANCE).  In this case,
     * the offset is part of the 'index' offset (for index copy and
     * tracking purposes).
     *
     * Example of logical key field is mapped to both an index and
     * memory parameter:
     *           F1: PT1.__INDEX
     *           F1: PT2.__PORT
     */
    for (group_idx = 0; (group_idx < map->groups) && !pt_found; group_idx++) {
        group = &map->group[group_idx];
        /* Skip non-PT groups */
        if (group->dest.kind != BCMLRD_MAP_PHYSICAL) {
            continue;
        }

        ptid = group->dest.id;

        /* If input PT is provided, skip groups that do not match */
        if ((in_ptid != BCMDRD_INVALID_ID) && (ptid != in_ptid)) {
            continue;
        }

        for (entry_idx = 0; entry_idx < group->entries; entry_idx++) {
            entry = &group->entry[entry_idx];
            me_found = FALSE;

            switch(entry->entry_type) {
            case BCMLRD_MAP_ENTRY_MAPPED_KEY: /* Direct mapped field */
                fid = entry->desc.field_id;
                if (fid == BCMLRD_FIELD_INDEX) {
                    me_found = TRUE;
                } else if ((fid == BCMLRD_FIELD_PORT) ||
                           (fid == BCMLRD_FIELD_INSTANCE)) {
                    /* Check if source is also mapped to __INDEX */
                    if (bcmltm_db_field_key_pt_index_mapped(unit, sid,
                                             entry->u.mapped.src.field_id)) {
                        me_found = TRUE;
                    }
                }
                break;

            case BCMLRD_MAP_ENTRY_REV_KEY_FIELD_XFRM_HANDLER: /* Transform */
                if (bcmltm_db_xfrm_field_find(unit, entry, 0,
                                              BCMLRD_FIELD_INDEX)) {
                    me_found = TRUE;
                    fid = BCMLRD_FIELD_INDEX;
                }
                break;

            default:
                break;
            }

            /* Add map entry to list */
            if (me_found) {
                SHR_IF_ERR_EXIT
                    (bcmltm_db_map_entry_add(map_entry_list,
                                             group_idx, entry_idx));

                /*
                 * Get PT field offset
                 *
                 * Only one offset is needed since that covers
                 * the entire PT key (for the case of several
                 * API keys mapping to PT subfields).
                 */
                if (*field_offset == BCMLTM_WB_OFFSET_INVALID) {
                    SHR_IF_ERR_EXIT
                        (bcmltm_db_pt_param_offset_get(unit, pt_list, ptid,
                                                       fid, field_offset));
                }

                pt_found = TRUE;
            }
        }
    }

    if (!pt_found) {
        SHR_VERBOSE_EXIT(SHR_E_NOT_FOUND);  /* No PT track element */
    }

    /* PT track field element found */
    *field_max = attr->index_max;
    *out_ptid = ptid;

 exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get working buffer offset and maximum value for a PT memory param.
 *
 * This routine gets the working buffer offset for a map to a PT memory
 * parameter, such as port or table instance.
 *
 * If table has no mapped PT memory parameter, the returning offset is invalid.
 *
 * This routine also returns a list of map entries that correspond
 * to the chosen track PT field.  This list can be used to construct
 * other metadata required to convert the decoded track PT fields
 * to API fields.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [in] attr Table attributes.
 * \param [in] pt_list PT list.
 * \param [in] in_ptid If valid, search must be within this PT ID.
 * \param [out] map_entry_list List of map entries chosen for track PT fields.
 * \param [out] out_ptid PT ID chosen for track PT fields.
 * \param [out] field_offset Offset of first PT memory parameter, if found.
 *                           Otherwise, returning offset is invalid.
 * \param [out] field_max Maximum value of PT memory parameter, if found.

 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_NOT_FOUND No mapped PT memory parameter found.
 * \retval !SHR_E_NONE Failure.
 */
static int
track_field_pt_mem_param_get(int unit, bcmlrd_sid_t sid,
                             const bcmltm_table_attr_t *attr,
                             const bcmltm_pt_list_t *pt_list,
                             bcmdrd_sid_t in_ptid,
                             bcmltm_db_map_entry_list_t *map_entry_list,
                             bcmdrd_sid_t *out_ptid,
                             uint32_t *field_offset,
                             uint32_t *field_max)
{
    const bcmlrd_map_t *map = NULL;
    const bcmlrd_map_group_t *group = NULL;
    const bcmlrd_map_entry_t *entry = NULL;
    uint32_t group_idx;
    uint32_t entry_idx;
    bcmdrd_sid_t ptid = BCMDRD_INVALID_ID;
    uint32_t fid = 0;
    bool pt_found = FALSE;
    bool me_found = FALSE;

    SHR_FUNC_ENTER(unit);

    *out_ptid = BCMDRD_INVALID_ID;
    *field_offset = BCMLTM_WB_OFFSET_INVALID;
    *field_max = 0;

    SHR_IF_ERR_EXIT
        (bcmltm_db_map_entry_list_init(unit, sid,
                                       TRACK_INDEX_MAX_MAPS,
                                       map_entry_list));

    /* Get LRD mapping */
    SHR_IF_ERR_EXIT
        (bcmlrd_map_get(unit, sid, &map));

    /*
     * Find all entry maps within a PT that target the PT memory parameter.
     * The memory parameters are normally mapped to these virtual
     * destination fields:
     *   __PORT
     *   __INSTANCE
     *
     * Need to get all entries since there may be more than
     * one API key that map to PT subfields.  The complete list of maps
     * is required in order to translate the PT key back to API keys
     * during a key allocation or key traverse operation.
     *
     * If the source field also maps to the PT Index (__INDEX),
     * this should be part of the index offset and not
     * the memory parameter (for index copy and
     * tracking purposes).
     */
    for (group_idx = 0; (group_idx < map->groups) && !pt_found; group_idx++) {
        group = &map->group[group_idx];
        /* Skip non-PT groups */
        if (group->dest.kind != BCMLRD_MAP_PHYSICAL) {
            continue;
        }

        ptid = group->dest.id;

        /* If input PT is provided, skip groups that do not match */
        if ((in_ptid != BCMDRD_INVALID_ID) && (ptid != in_ptid)) {
            continue;
        }

        for (entry_idx = 0; entry_idx < group->entries; entry_idx++) {
            entry = &group->entry[entry_idx];
            me_found = FALSE;

            switch(entry->entry_type) {
            case BCMLRD_MAP_ENTRY_MAPPED_KEY:
                /* Direct mapped field */
                fid = entry->desc.field_id;
                if ((fid == BCMLRD_FIELD_PORT) ||
                    (fid == BCMLRD_FIELD_INSTANCE)) {
                    /* Check if source is also mapped to __INDEX */
                    if (!bcmltm_db_field_key_pt_index_mapped(unit, sid,
                                              entry->u.mapped.src.field_id)) {
                        me_found = TRUE;
                    }
                }
                break;

            case BCMLRD_MAP_ENTRY_REV_KEY_FIELD_XFRM_HANDLER:
                /*
                 * Transform
                 * Need the reverse map because this will be used
                 * to recover the API field back for alloc or traverse.
                 */
                if (bcmltm_db_xfrm_field_find(unit, entry, 0,
                                              BCMLRD_FIELD_PORT)) {
                    me_found = TRUE;
                    fid = BCMLRD_FIELD_PORT;
                } else if (bcmltm_db_xfrm_field_find(unit, entry, 0,
                                                     BCMLRD_FIELD_INSTANCE)) {
                    me_found = TRUE;
                    fid = BCMLRD_FIELD_INSTANCE;
                }
                break;

            default:
                break;
            }

            /* Add map entry to list */
            if (me_found) {
                SHR_IF_ERR_EXIT
                    (bcmltm_db_map_entry_add(map_entry_list,
                                             group_idx, entry_idx));

                /*
                 * Get PT field offset
                 *
                 * Only one offset is needed since that covers
                 * the entire PT key (for the case of several
                 * API keys mapping to PT subfields).
                 */
                if (*field_offset == BCMLTM_WB_OFFSET_INVALID) {
                    SHR_IF_ERR_EXIT
                        (bcmltm_db_pt_param_offset_get(unit, pt_list, ptid,
                                                       fid, field_offset));
                }

                pt_found = TRUE;
            }
        }
    }

    if (!pt_found) {
        SHR_VERBOSE_EXIT(SHR_E_NOT_FOUND);  /* No PT track element */
    }

    /* PT track field element found */
    *field_max = attr->mem_param_max;
    *out_ptid = ptid;

 exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get working buffer offset and maximum value for a PT SID selector.
 *
 * This routine gets the working buffer offset for a PT SID selector.
 * PT SID selectors are defined through table transforms.
 *
 * If table has no PT SID selector, the returning offset is invalid.
 *
 * This routine also returns a list of map entries that correspond
 * to the chosen track PT field.  This list can be used to construct
 * other metadata required to convert the decoded track PT fields
 * to API fields.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [in] attr Table attributes.
 * \param [in] pt_list PT list.
 * \param [in] in_ptid If valid, search must be within this PT ID.
 * \param [out] map_entry_list List of map entries chosen for track PT fields.
 * \param [out] out_ptid PT ID chosen for track PT fields.
 * \param [out] field_offset Offset of PT SID selector, if found.
 *                           Otherwise, returning offset is invalid.
 * \param [out] max Maximum value of PT SID selector, if found.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_NOT_FOUND No PT SID selector selection found.
 * \retval !SHR_E_NONE Failure.
 */
static int
track_field_pt_sid_sel_get(int unit, bcmlrd_sid_t sid,
                           const bcmltm_table_attr_t *attr,
                           const bcmltm_pt_list_t *pt_list,
                           bcmdrd_sid_t in_ptid,
                           bcmltm_db_map_entry_list_t *map_entry_list,
                           bcmdrd_sid_t *out_ptid,
                           uint32_t *field_offset,
                           uint32_t *field_max)
{
    const bcmlrd_map_t *map = NULL;
    const bcmlrd_map_group_t *group = NULL;
    const bcmlrd_map_entry_t *entry = NULL;
    const bcmltd_xfrm_handler_t *xfrm = NULL;
    uint32_t group_idx;
    uint32_t entry_idx;
    bcmdrd_sid_t ptid = BCMDRD_INVALID_ID;
    uint32_t fid = BCMLRD_FIELD_TABLE_SEL;
    bool pt_found = FALSE;
    bool me_found = FALSE;

    SHR_FUNC_ENTER(unit);

    *out_ptid = BCMDRD_INVALID_ID;
    *field_offset = BCMLTM_WB_OFFSET_INVALID;
    *field_max = 0;

    SHR_IF_ERR_EXIT
        (bcmltm_db_map_entry_list_init(unit, sid,
                                       TRACK_INDEX_MAX_MAPS,
                                       map_entry_list));

    /* Get LRD mapping */
    SHR_IF_ERR_EXIT
        (bcmlrd_map_get(unit, sid, &map));

    /*
     * Find all entry maps within a PT that target the PT SID selector.
     * The PT SID selector is specified through table transforms.
     */
    for (group_idx = 0; (group_idx < map->groups) && !pt_found; group_idx++) {
        group = &map->group[group_idx];
        /* Skip non-PT groups */
        if (group->dest.kind != BCMLRD_MAP_PHYSICAL) {
            continue;
        }

        ptid = group->dest.id;

        /* If input PT is provided, skip groups that do not match */
        if ((in_ptid != BCMDRD_INVALID_ID) && (ptid != in_ptid)) {
            continue;
        }

        for (entry_idx = 0; entry_idx < group->entries; entry_idx++) {
            entry = &group->entry[entry_idx];
            me_found = FALSE;

            switch(entry->entry_type) {
            case BCMLRD_MAP_ENTRY_REV_KEY_FIELD_XFRM_HANDLER: /* Transform */
                xfrm = bcmltm_lta_intf_xfrm_handler_get(unit,
                                    entry->u.xfrm.desc->handler_id);
                if (xfrm != NULL) {
                    if (xfrm->arg->tables > 0) {
                        me_found = TRUE;
                    }
                }
                break;

            default:
                break;
            }

            /* Add map entry to list */
            if (me_found) {
                SHR_IF_ERR_EXIT
                    (bcmltm_db_map_entry_add(map_entry_list,
                                             group_idx, entry_idx));

                /*
                 * Get PT field offset
                 *
                 * Only one offset is needed since that covers
                 * the entire PT key (for the case of several
                 * API keys mapping to PT subfields).
                 */
                if (*field_offset == BCMLTM_WB_OFFSET_INVALID) {
                    SHR_IF_ERR_EXIT
                        (bcmltm_db_pt_param_offset_get(unit, pt_list, ptid,
                                                       fid, field_offset));
                }

                pt_found = TRUE;
            }
        }
    }

    if (!pt_found) {
        SHR_VERBOSE_EXIT(SHR_E_NOT_FOUND);  /* No PT track element */
    }

    /* PT track field element found */
    *field_max = attr->sid_sel_max;
    *out_ptid = ptid;

 exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get information for track field elements.
 *
 * This routine gets information for the fields chosen to be used
 * as the track index field elements.
 * the track index.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [in] attr Table attributes.
 * \param [in] pt_list PT list.
 * \param [in] info_list_max Array size of field list.
 * \param [out] field_info_list List of track field elements information.
 * \param [out] num_fields Number of track field elements returned.
 * \param [out] map_list_ptr List of map entries chosen for track elements.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
track_index_pt_field_info_list_get(int unit, bcmlrd_sid_t sid,
                                   const bcmltm_table_attr_t *attr,
                                   const bcmltm_pt_list_t *pt_list,
                                   uint32_t info_list_max,
                                   track_field_info_t *field_info_list,
                                   uint32_t *num_fields,
                                   bcmltm_db_map_entry_list_t **map_list_ptr)
{
    uint32_t num_track_fields = 0;
    bcmdrd_sid_t in_ptid = BCMDRD_INVALID_ID;
    bcmdrd_sid_t out_ptid = BCMDRD_INVALID_ID;
    uint32_t field_idx = 0;
    track_field_info_t *field;
    bool alloc;
    uint32_t index_offset, mem_param_offset, sid_sel_offset;
    uint32_t index_max, mem_param_max, sid_sel_max;
    bcmltm_db_map_entry_list_t index_map_list;
    bcmltm_db_map_entry_list_t mem_param_map_list;
    bcmltm_db_map_entry_list_t sid_sel_map_list;
    bcmltm_db_map_entry_list_t *map_list = NULL;

    SHR_FUNC_ENTER(unit);

    sal_memset(field_info_list, 0, sizeof(*field_info_list)*info_list_max);
    *num_fields = 0;
    *map_list_ptr = NULL;

    sal_memset(&index_map_list, 0, sizeof(index_map_list));
    sal_memset(&mem_param_map_list, 0, sizeof(mem_param_map_list));
    sal_memset(&sid_sel_map_list, 0, sizeof(sid_sel_map_list));

    /* Get the physical tracking elements */

    /* PT Index */
    SHR_IF_ERR_EXIT_EXCEPT_IF
        (track_field_pt_index_get(unit, sid, attr, pt_list, in_ptid,
                                  &index_map_list, &out_ptid,
                                  &index_offset, &index_max),
         SHR_E_NOT_FOUND);
    if (index_offset != BCMLTM_WB_OFFSET_INVALID) {
        num_track_fields++;
        in_ptid = out_ptid;  /* Use same PT in next searches */
    }

    /* PT Memory Parameter (Port or Table Instance) */
    SHR_IF_ERR_EXIT_EXCEPT_IF
        (track_field_pt_mem_param_get(unit, sid, attr, pt_list, in_ptid,
                                      &mem_param_map_list, &out_ptid,
                                      &mem_param_offset, &mem_param_max),
         SHR_E_NOT_FOUND);
    if (mem_param_offset != BCMLTM_WB_OFFSET_INVALID) {
        num_track_fields++;
        in_ptid = out_ptid;  /* Use same PT in next searches */
    }

    /* PT SID Select */
    SHR_IF_ERR_EXIT_EXCEPT_IF
        (track_field_pt_sid_sel_get(unit, sid, attr, pt_list, in_ptid,
                                    &sid_sel_map_list, &out_ptid,
                                    &sid_sel_offset, &sid_sel_max),
         SHR_E_NOT_FOUND);
    if (sid_sel_offset != BCMLTM_WB_OFFSET_INVALID) {
        num_track_fields++;
        in_ptid = out_ptid;  /* Use same PT in next searches */
    }

    

    if (num_track_fields == 0) {
        SHR_EXIT();
    }

    if (num_track_fields > info_list_max) {
        const char *table_name;

        table_name = bcmltm_lt_table_sid_to_name(unit, sid);
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "%s(ltid=%d): "
                              "Total track element count exceeds "
                              "max list size max=%d actual=%d\n"),
                   table_name, sid,
                   info_list_max, num_track_fields));
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    /*
     * PT Index
     *
     * PT Index is an allocatable element when LT is IwA.
     */
    if (index_offset != BCMLTM_WB_OFFSET_INVALID) {
        /* If IwA, this is allocatable */
        if (BCMLTM_TABLE_TYPE_IS_ALLOC_INDEX(attr->type)) {
            alloc = TRUE;
        } else {
            alloc = FALSE;
        }

        field = &field_info_list[field_idx++];
        track_field_info_get(index_offset, index_max, alloc, field);
    }

    /* PT Memory Parameter */
    if (mem_param_offset != BCMLTM_WB_OFFSET_INVALID) {
        alloc = FALSE;
        field = &field_info_list[field_idx++];
        track_field_info_get(mem_param_offset, mem_param_max, alloc, field);
    }

    /* PT SID Select */
    if (sid_sel_offset != BCMLTM_WB_OFFSET_INVALID) {
        alloc = FALSE;
        field = &field_info_list[field_idx++];
        track_field_info_get(sid_sel_offset, sid_sel_max, alloc, field);
    }

    

    *num_fields = field_idx;

    /*
     * Add map entries from each the map list of each track index element.
     * The returned map entry list will be used to create the metadata
     * for direct field maps and transforms to provide the mechanism
     * to convert the track index elements back to API fields.
      */
    SHR_IF_ERR_EXIT
        (bcmltm_db_map_entry_list_create(unit, sid,
                                         TRACK_INDEX_MAX_MAPS,
                                         &map_list));
    SHR_IF_ERR_EXIT
        (bcmltm_db_map_entry_list_add(map_list, &index_map_list));
    SHR_IF_ERR_EXIT
        (bcmltm_db_map_entry_list_add(map_list, &mem_param_map_list));
    SHR_IF_ERR_EXIT
        (bcmltm_db_map_entry_list_add(map_list, &sid_sel_map_list));

    *map_list_ptr = map_list;

 exit:
    if (SHR_FUNC_ERR()) {
        bcmltm_db_map_entry_list_destroy(map_list);
        *num_fields = 0;
        *map_list_ptr = NULL;
    }

    bcmltm_db_map_entry_list_cleanup(&sid_sel_map_list);
    bcmltm_db_map_entry_list_cleanup(&mem_param_map_list);
    bcmltm_db_map_entry_list_cleanup(&index_map_list);

    SHR_FUNC_EXIT();
}

/*!
 * \brief Create track index fields metadata for PT based field elements.
 *
 * This routine creates the list of track index field elements required
 * to manage the tracking index based on the device physical parameters:
 *   PT Index
 *   PT Memory Parameter: Port or Table Instance
 *   PT SID Selector
 *
 * This routine also outputs the overall track index max value and
 * its mask for allocatable elements.
 *
 * Currently, this assumes that the min and max limits for
 * the same type of physical parameters are equal.
 * For instance, an LT maps to PT1 and PT2:
 *    If PT1 index (min..max) is (0..3), then
 *       PT2 index (min..max) is (0..3) too.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [in] attr Table attributes.
 * \param [in] pt_list PT list.
 * \param [out] fields_ptr Pointer to track field list.
 * \param [out] num_fields Number of track field elements.
 * \param [out] track_index_max Maximum tracking index value.
 * \param [out] track_alloc_mask Mask for allocatable fields.
 * \param [out] map_list_ptr List of map entries chosen for track elements.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
track_index_fields_pt_create(int unit, bcmlrd_sid_t sid,
                             const bcmltm_table_attr_t *attr,
                             const bcmltm_pt_list_t *pt_list,
                             bcmltm_track_index_field_t **fields_ptr,
                             uint32_t *num_fields,
                             uint32_t *track_index_max,
                             uint32_t *track_alloc_mask,
                             bcmltm_db_map_entry_list_t **map_list_ptr)
{
    bcmltm_track_index_field_t *fields = NULL;
    bcmltm_track_index_field_t *field;
    unsigned int size;
    uint32_t num_track_fields;
    track_field_info_t info_list[TRACK_INDEX_MAX_FIELDS];
    track_field_info_t *info;
    uint32_t i;
    uint32_t field_idx = 0;
    uint32_t shift = 0;
    uint32_t all_index_max = 0;
    uint32_t all_alloc_mask = 0x0;

    SHR_FUNC_ENTER(unit);

    *num_fields = 0;
    *fields_ptr = NULL;
    *track_index_max = 0;
    *track_alloc_mask = 0x0;

    /* Get track elements field information */
    SHR_IF_ERR_EXIT
        (track_index_pt_field_info_list_get(unit, sid, attr, pt_list,
                                            TRACK_INDEX_MAX_FIELDS,
                                            info_list,
                                            &num_track_fields,
                                            map_list_ptr));
    if (num_track_fields == 0) {
        SHR_EXIT();
    }

    /* Allocate track index field list */
    size = sizeof(*fields) * num_track_fields;
    SHR_ALLOC(fields, size, "bcmltmDmTrackIndexFields");
    SHR_NULL_CHECK(fields, SHR_E_MEMORY);
    sal_memset(fields, 0, size);

    /*
     * Place allocatable track elements first.
     * These need to be placed at the LSB bits of the tracking index.
     */
    for (i = 0; i < num_track_fields; i++) {
        info = &info_list[i];
        if (!info->allocatable) {
            continue;  /* Skip required elements */
        }

        field = &fields[field_idx++];
        track_index_field_get(info, shift, field);
        all_index_max |= (info->max << shift);
        all_alloc_mask |= (field->field_mask << shift);
        shift += info->bitlen;
    }

    /* Place required track elements next */
    for (i = 0; i < num_track_fields; i++) {
        info = &info_list[i];
        if (info->allocatable) {
            continue;  /* Skip allocatable elements */
        }

        field = &fields[field_idx++];
        track_index_field_get(info, shift, field);
        all_index_max |= (info->max << shift);
        shift += info->bitlen;
    }

    /* Sanity check: all tracking elements must fit within 32 bits */
    if (shift > 32) {
        const char *table_name;

        table_name = bcmltm_lt_table_sid_to_name(unit, sid);
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "%s(ltid=%d): "
                              "Total track elements width exceeds "
                              "32 bits width=%d\n"),
                   table_name, sid, shift));
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    *num_fields = field_idx;
    *fields_ptr = fields;
    *track_index_max = all_index_max;
    *track_alloc_mask = all_alloc_mask;

 exit:
    if (SHR_FUNC_ERR()) {
        SHR_FREE(fields);
        *num_fields = 0;
        *fields_ptr = NULL;
        *track_index_max = 0;
        *track_alloc_mask = 0x0;
    }

    SHR_FUNC_EXIT();
}

static int
track_index_map_entry_add(int unit, bcmlrd_sid_t sid,
                          bcmltm_db_map_entry_list_t *map_entry_list)
{
    const bcmlrd_map_t *map = NULL;
    const bcmlrd_map_group_t *group = NULL;
    const bcmlrd_map_entry_t *entry = NULL;
    uint32_t group_idx;
    uint32_t entry_idx;
    bool me_found = FALSE;

    SHR_FUNC_ENTER(unit);

    /* Get LRD mapping */
    SHR_IF_ERR_EXIT
        (bcmlrd_map_get(unit, sid, &map));

    /*
     * Find entry map that supplies the track index.
     *
     * A map entry transform can supply the track index
     * with the following virtual destination field:
     *   __TRACK_INDEX
     *
     * A transform with track index should contain all the
     * logical key fields.  Search for the map entry can
     * exit when one map entry is found.
     */
    for (group_idx = 0; (group_idx < map->groups) && !me_found; group_idx++) {
        group = &map->group[group_idx];
        /* Skip non-PT groups */
        if (group->dest.kind != BCMLRD_MAP_PHYSICAL) {
            continue;
        }

        for (entry_idx = 0; entry_idx < group->entries; entry_idx++) {
            entry = &group->entry[entry_idx];
            me_found = FALSE;

            switch(entry->entry_type) {
            case BCMLRD_MAP_ENTRY_REV_KEY_FIELD_XFRM_HANDLER:
                /*
                 * Transform
                 * Need the reverse map because this will be used
                 * to recover the API field back for alloc or traverse.
                 */
                if (bcmltm_db_xfrm_field_find(unit, entry, FALSE,
                                              BCMLRD_FIELD_TRACK_INDEX)) {
                    me_found = TRUE;
                }
                break;

            default:
                break;
            }

            /* Add map entry to list */
            if (me_found) {
                SHR_IF_ERR_EXIT
                    (bcmltm_db_map_entry_add(map_entry_list,
                                             group_idx, entry_idx));
                break;
            }
        }
    }

    if (!me_found) {
        /* No transform with track index found */
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }

 exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Create track index fields metadata for mapped track index.
 *
 * This routine creates the list of track index field elements metadata
 * for a table that has a map to the track index.
 *
 * A mapped track index is provided through a forward field transform map.
 * Therefore, there is no track field elements to encode/decode.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [in] attr Table attributes.
 * \param [out] fields_ptr Pointer to track field list.
 * \param [out] num_fields Number of track field elements.
 * \param [out] track_index_max Maximum tracking index value.
 * \param [out] track_alloc_mask Mask for allocatable fields.
 * \param [out] map_list_ptr List of map entries chosen for track elements.

 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
track_index_fields_map_create(int unit, bcmlrd_sid_t sid,
                              const bcmltm_table_attr_t *attr,
                              bcmltm_track_index_field_t **fields_ptr,
                              uint32_t *num_fields,
                              uint32_t *track_index_max,
                              uint32_t *track_alloc_mask,
                              bcmltm_db_map_entry_list_t **map_list_ptr)
{
    int rv;
    uint32_t max;
    bcmltm_db_map_entry_list_t *map_list = NULL;

    SHR_FUNC_ENTER(unit);

    /* Sanity check: IwA is not supported */
    if (BCMLTM_TABLE_TYPE_IS_ALLOC_INDEX(attr->type)) {
        const char *table_name;

        table_name = bcmltm_lt_table_sid_to_name(unit, sid);
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "%s(ltid=%d): "
                              "Track Index map does not support IwA LT\n"),
                   table_name, sid));
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    /*
     * Track index is provided by field transform,
     * so there is no track field elements needed.
     */
    *num_fields = 0;
    *fields_ptr = NULL;
    *track_index_max = 0;
    *track_alloc_mask = 0x0; /* Allocation is not supported */
    *map_list_ptr = NULL;

    /* Get max for track index */
    rv = bcmlrd_map_table_attr_get(unit, sid,
                    BCMLRD_MAP_TABLE_ATTRIBUTE_TRACK_INDEX_MAX_INDEX,
                    &max);
    if (SHR_FAILURE(rv)) {
        if (rv == SHR_E_UNAVAIL) {
            /* If not available, then set max to zero */
            max = 0;
        } else {
            SHR_ERR_EXIT(rv);
        }
    }

    /* Add map entry of transform that supplies the track index */
    SHR_IF_ERR_EXIT
        (bcmltm_db_map_entry_list_create(unit, sid, 1,
                                         &map_list));
    SHR_IF_ERR_EXIT
        (track_index_map_entry_add(unit, sid, map_list));


    *track_index_max = max;
    *map_list_ptr = map_list;

 exit:
    if (SHR_FUNC_ERR()) {
        bcmltm_db_map_entry_list_destroy(map_list);
        *map_list_ptr = NULL;
    }

    SHR_FUNC_EXIT();
}

/*!
 * \brief Create track index field elements metadata.
 *
 * This routine creates the track index field elements metadata
 * for the given table.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [in] dm_arg DM arguments.
 * \param [in] pt_list PT list.
 * \param [out] fields_ptr Pointer to track field list.
 * \param [out] num_fields Number of track field elements.
 * \param [out] track_index_max Maximum tracking index value.
 * \param [out] track_alloc_mask Mask for allocatable fields.
 * \param [out] map_list_ptr List of map entries chosen for track elements.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
track_index_fields_create(int unit, bcmlrd_sid_t sid,
                          const bcmltm_db_dm_arg_t *dm_arg,
                          const bcmltm_pt_list_t *pt_list,
                          bcmltm_track_index_field_t **fields_ptr,
                          uint32_t *num_fields,
                          uint32_t *track_index_max,
                          uint32_t *track_alloc_mask,
                          bcmltm_db_map_entry_list_t **map_list_ptr)
{
    const bcmltm_table_attr_t *attr = dm_arg->attr;
    uint32_t num_keys;
    uint32_t num_fid_idx;

    SHR_FUNC_ENTER(unit);

    /* Check for LT without keys */
    SHR_IF_ERR_EXIT
        (bcmltm_db_field_key_count_get(unit, sid, &num_keys, &num_fid_idx));
    if (num_keys == 0) {
        /*
         * There is only 1 entry in a keyless table.
         * This entry always uses a track index of zero.
         */
        *fields_ptr = NULL;
        *num_fields = 0;
        *track_index_max = 0;
        *track_alloc_mask = 0x0;
        *map_list_ptr = NULL;

        SHR_EXIT();
    }

    if (dm_arg->track_index_map) {
        /* Track index is supplied by transform map */
        SHR_IF_ERR_EXIT
            (track_index_fields_map_create(unit, sid, attr,
                                           fields_ptr, num_fields,
                                           track_index_max, track_alloc_mask,
                                           map_list_ptr));
    } else {
        /* Track index is supplied by LTM (using PT elements) */
        SHR_IF_ERR_EXIT
            (track_index_fields_pt_create(unit, sid, attr, pt_list,
                                          fields_ptr, num_fields,
                                          track_index_max, track_alloc_mask,
                                          map_list_ptr));
    }

 exit:
    SHR_FUNC_EXIT();
}

/*******************************************************************************
 * Public functions
 */

int
bcmltm_db_dm_track_index_create(int unit,
                                bcmlrd_sid_t sid,
                                const bcmltm_db_dm_arg_t *dm_arg,
                                const bcmltm_pt_list_t *pt_list,
                                bcmltm_track_index_t **index_ptr,
                                bcmltm_db_map_entry_list_t **map_list_ptr)
{
    bcmltm_track_index_t *index = NULL;
    const bcmltm_table_attr_t *attr = dm_arg->attr;
    const bcmltm_wb_lt_pvt_offsets_t *lt_pvt_wb_offsets;
    uint32_t index_absent_offset = BCMLTM_WB_OFFSET_INVALID;

    SHR_FUNC_ENTER(unit);

    lt_pvt_wb_offsets = dm_arg->lt_pvt_wb_offsets;

    /* Allocate */
    SHR_ALLOC(index, sizeof(*index), "bcmltmDmTrackIndex");
    SHR_NULL_CHECK(index, SHR_E_MEMORY);
    sal_memset(index, 0, sizeof(*index));

    /* Valid for Index with Allocation LT */
    if (BCMLTM_TABLE_TYPE_IS_ALLOC_INDEX(attr->type)) {
        index_absent_offset = lt_pvt_wb_offsets->index_absent;
    }

    /* Get track fields */
    SHR_IF_ERR_EXIT
        (track_index_fields_create(unit, sid, dm_arg, pt_list,
                                   &index->track_index_fields,
                                   &index->track_index_field_num,
                                   &index->track_index_max,
                                   &index->track_alloc_mask,
                                   map_list_ptr));

    /* Fill rest of data */
    index->track_index_offset = lt_pvt_wb_offsets->track_index;
    index->index_absent_offset = index_absent_offset;

    *index_ptr = index;

 exit:
    if (SHR_FUNC_ERR()) {
        bcmltm_db_dm_track_index_destroy(index);
        *index_ptr = NULL;
    }

    SHR_FUNC_EXIT();
}

void
bcmltm_db_dm_track_index_destroy(bcmltm_track_index_t *index)
{
    if (index == NULL) {
        return;
    }

    if (index->track_index_fields != NULL) {
        SHR_FREE(index->track_index_fields);
    }
    SHR_FREE(index);

    return;
}

int
bcmltm_db_dm_track_info_create(int unit,
                               bcmlrd_sid_t sid,
                               const bcmltm_db_dm_arg_t *dm_arg,
                               const bcmltm_db_map_entry_list_t *map_list,
                               const bcmltm_db_dm_xfrm_list_t *rev_key_xfrms,
                               bcmltm_db_dm_track_info_t **info_ptr)
{
    bcmltm_db_dm_track_info_t *info = NULL;
    bcmltm_table_type_t table_type;
    bcmltm_db_field_selection_t *selection = NULL;
    bcmltm_db_dm_field_map_spec_t fspec;

    SHR_FUNC_ENTER(unit);

    *info_ptr = NULL;

    /* Applicable only to Index LTs */
    table_type = dm_arg->attr->type;
    if (!BCMLTM_TABLE_TYPE_IS_INDEX(table_type)) {
        SHR_EXIT();
    }

    /* Allocate */
    SHR_ALLOC(info, sizeof(*info), "bcmltmDbDmTrackInfo");
    SHR_NULL_CHECK(info, SHR_E_MEMORY);
    sal_memset(info, 0, sizeof(*info));

    SHR_IF_ERR_EXIT
        (bcmltm_db_field_selection_me_create(unit, sid,
                                             map_list,
                                             &selection));

    /* Create direct field maps for track index elements */
    bcmltm_db_dm_field_map_spec_set(&fspec,
                                    BCMLTM_FIELD_TYPE_KEY,
                                    BCMLTM_FIELD_DIR_IN,
                                    BCMLTM_FIELD_KEY_TYPE_ALL);
    SHR_IF_ERR_EXIT
        (bcmltm_db_dm_field_mapping_create(unit, sid, dm_arg,
                                           selection, &fspec,
                                           &info->track_to_api));

    /* Create transform reference list for track index elements */
    SHR_IF_ERR_EXIT
        (bcmltm_db_dm_xfrm_list_ref_create(unit, sid,
                                           map_list,
                                           rev_key_xfrms,
                                           BCMLTM_FIELD_XFRM_TYPE_REV_KEY,
                                           &info->track_rev_xfrms));

    *info_ptr = info;

 exit:
    if (SHR_FUNC_ERR()) {
        bcmltm_db_dm_track_info_destroy(info);
        *info_ptr = NULL;
    }

    bcmltm_db_field_selection_destroy(selection);
    SHR_FUNC_EXIT();
}

void
bcmltm_db_dm_track_info_destroy(bcmltm_db_dm_track_info_t *info)
{
    if (info == NULL) {
        return;
    }

    /* Destroy transform information */
    bcmltm_db_dm_xfrm_list_ref_destroy(info->track_rev_xfrms);

    /* Destroy direct field mapping */
    bcmltm_db_field_mapping_destroy(info->track_to_api);

    SHR_FREE(info);

    return;
}
