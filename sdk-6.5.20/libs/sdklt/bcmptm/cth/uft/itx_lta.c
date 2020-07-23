/*! \file itx_lta.c
 *
 * Index Tile mode Transform functions implementations.
 *
 * These functions provide the LTA xforms and associated support for
 * tile-aware index LTs.
 *
 * When a tile mode changes, the L2P map
 * from the LT index key to the physical (PTID, PT index) may change.
 * This RM-like subcomponent registers for notification on tile mode change.
 * When such a change occurs, the affected LTs (the one previously mapped
 * to this tile, and the one now mapped to it) must
 * 1) recalculate the L2P index map records
 * 2) record the map for later use
 * 3) Update LTS_TCAM_DATA values for each active tile to which this
 *    LT maps.
 *
 * When a table op is issued for a tile-aware index LT, the LT index key
 * must be converted to (PTID, PT index) for any PTM ops.
 * An LTA Transform function uses the L2P map determined from tile mode
 * configuration to seek the correct physical coordinates.
 *
 * The auto-generated LT maps for such LTs will contain the transform
 * specification with lta_args for this index conversion.  The track
 * index used to manage index LTs within the LTM is also provided
 * by the identity transform (LT index => track index).
 *
 * A simple implementation of entry_limit_get is provided for LTA
 * link to permit LTM retrieval of the TABLE_INFO.ENTRY_LIMIT value.
 *
 * This component also provides a mechanism for flex counters to know
 * the correct order of PTs so that the counter signalling mechanism
 * operates properly.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <shr/shr_util.h>
#include <bcmdrd/bcmdrd_pt.h>
#include <bcmdrd/bcmdrd_field.h>
#include <bcmlrd/bcmlrd_conf.h>
#include <bcmptm/bcmptm_uft.h>
#include <bcmptm/bcmptm_itx.h>
#include <bcmptm/bcmptm_cth_uft_be_internal.h>
#include <bcmevm/bcmevm_api.h>
#include "itx_internal.h"
#include "sbr_device.h"
#include "sbr_util.h"

/*******************************************************************************
 * Local definitions
 */

/* BSL Module */
#define BSL_LOG_MODULE BSL_LS_BCMPTM_UFT

/* Maximum multi-tile LT index */
#define BCMPTM_ITX_ILT_MAX_INDEX 0xffff

#define BCMPTM_ITX_ID_INVALID ((uint32_t)-1)

static bcmptm_itx_lt_map_list_t *itx_maps[BCMDRD_CONFIG_MAX_UNITS];

static const bcmptm_tile_acc_mode_list_t *acc_mode_list[BCMDRD_CONFIG_MAX_UNITS];

/*! Array of variant specific attach functions. */
#define BCMLRD_VARIANT_ENTRY(_bd,_bu,_va,_ve,_vu,_vv,_vo,_vd,_r0,_r1) \
    &_bd##_vu##_va##_ptm_itm_tile_list,

const bcmptm_tile_list_t *bcmptm_itx_tile_list[] = {
    NULL, /* dummy entry for type "none" */
#include <bcmlrd/chip/bcmlrd_variant.h>
    NULL /* end-of-list */
};


/*******************************************************************************
 * Private functions
 */

/*!
 * \brief Index LT tile mode HW LTS_TCAM modifications.
 *
 * This function determines the per-tile index ranges and updates the
 * LTS_TCAM entry to reflect the new bounds.
 *
 * \param [in] unit      Unit number.
 * \param [in] ltid      Logical table ID.
 * \param [in] ev_data   Data associated with the event.
 * \param [in] tr_info   Tile range info for this tile.
 *
 * \retval SHR_E_NONE  OK
 * \retval !SHR_E_NONE ERROR
 */
static int
bcmptm_itx_tile_range_update(int unit,
                             uint32_t ltid,
                             uint64_t ev_data,
                             bcmptm_itx_tile_range_info_t *tr_info)
{
    uint32_t en_val, en_mask, tile_mode, seed, index_mask;
    int shift;
    bcmltd_op_arg_t op_arg;
    bool robust;

    SHR_FUNC_ENTER(unit);

    op_arg.trans_id = BCMPTM_UFT_EV_INFO_TRANS_ID_GET(ev_data);
    op_arg.attrib = 0;

    if ((tr_info->tile_index_min == BCMPTM_ITX_ID_INVALID) &&
        (tr_info->tile_index_max == 0)) {
        en_val = 0;
        en_mask = 0;
    } else {
        index_mask = (1 << tr_info->pt_index_bit_size) - 1;
        shift =
            shr_util_log2(tr_info->tile_index_max -
                          tr_info->tile_index_min + 1);
        en_mask = BCMPTM_ITX_ILT_MAX_INDEX  << shift;
        en_mask &= index_mask;
        en_val = tr_info->tile_index_min & index_mask;
    }

    SHR_IF_ERR_EXIT
        (bcmptm_uft_tile_cfg_get(unit, tr_info->tile_id,
                                 &tile_mode, &robust, &seed));

    SHR_IF_ERR_EXIT
        (bcmptm_sbr_lookup_en_mask_set(unit, tr_info->tile_id,
                                       tile_mode,
                                       ltid,
                                       BCMPTM_SBR_LOOKUP0,
                                       BCMPTM_UFT_EV_INFO_LTID_GET(ev_data),
                                       &op_arg,
                                       en_val,
                                       en_mask));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Index LT tile mode HW LTS_TCAM modifications.
 *
 * This function determines the per-tile index ranges and updates the
 * LTS_TCAM entry to reflect the new bounds.
 *
 * \param [in] unit    Unit number.
 * \param [in] lt_map  Index LT physical table mapping info.
 * \param [in] ev_data Data associated with the event.
 *
 * \retval SHR_E_NONE  OK
 * \retval !SHR_E_NONE ERROR
 */
static int
bcmptm_itx_lt_map_to_hw(int unit,
                        bcmptm_itx_lt_map_t *lt_map,
                        uint64_t ev_data)
{
    uint32_t tix;
    bcmptm_itx_tile_range_list_t *tr_list;
    bcmptm_itx_tile_range_info_t *tr_info;

    SHR_FUNC_ENTER(unit);

    tr_list = lt_map->tile_range_list;

    for (tix = 0; tix < tr_list->tile_num; tix++) {
        tr_info = &(tr_list->tile_list[tix]);

        SHR_IF_ERR_EXIT
            (bcmptm_itx_tile_range_update(unit,
                                          lt_map->ltid,
                                          ev_data,
                                          tr_info));
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Index LT tile mode index range calculations.
 *
 * This function recalculates one index LTs mappings to PTs.
 *
 * \param [in] unit       Unit number.
 * \param [in] banks_info Banks info for the ltid's LOOKUP0.
 * \param [in/out] lt_map Index LT physical table mapping info.
 *
 * \retval N/A
 */
static void
bcmptm_itx_lt_index_range_construct(int unit,
                                    bcmptm_lt_lookup_banks_info_t *banks_info,
                                    bcmptm_itx_lt_map_t *lt_map)
{
    uint32_t ptix, bix;
    int32_t tix, tile_count;
    const bcmptm_tile_info_t *hw_tile_info;
    const bcmptm_tile_memory_t *memory;
    bcmptm_itx_index_range_list_t *ir_list;
    bcmptm_itx_index_range_info_t *ir_info = NULL;
    bcmptm_itx_tile_range_info_t *tile_info = NULL;
    uint32_t last_tile_id = BCMPTM_ITX_ID_INVALID;

    hw_tile_info = lt_map->hw_tile_info;
    ir_list = lt_map->index_range_list[lt_map->active_list];
    ir_list->entry_limit = 0;
    ir_list->pt_num = 0;
    tile_count = lt_map->tile_max;
    tix = -1;

    for (ptix = 0; ptix < hw_tile_info->memory_count; ptix++) {
        memory = &(hw_tile_info->memory[ptix]);

        for (bix = 0; bix < banks_info->bank_cnt; bix++) {
            if (memory->sid == banks_info->bank_attr[bix].bank_sid) {
                /* PT is mapped, add to range list */
                ir_info = &(ir_list->pt_map_list[ir_list->pt_num]);
                ir_info->pt_id = memory->sid;
                ir_info->tile_id = memory->tile_id;
                ir_info->lt_index_min = ir_list->entry_limit;
                ir_info->logical_multiplier = memory->logical_multiplier;
                if (memory->logical_multiplier != 0) {
                    ir_list->entry_limit +=
                        (memory->size * memory->logical_multiplier);
                } else {
                    ir_list->entry_limit += memory->size;
                }
                ir_info->lt_index_max = ir_list->entry_limit - 1;
                ir_list->pt_num++;
                break;
            }
        }

        /*
         * This logic expects the requirement that all PTs within
         * a tile are mapped to the same LT.
         * The PTs within a tile must have the same logical multipler.
         */
        if (bix < banks_info->bank_cnt) {
            /* Mapped tile */
            if (last_tile_id != memory->tile_id) {
                /* New tile ID */
                tix++;
                if (tix >= tile_count) {
                    break;
                }
                last_tile_id = memory->tile_id;
                tile_info = &(lt_map->tile_range_list->tile_list[tix]);
                tile_info->tile_id = memory->tile_id;

                tile_info->tile_index_min = ir_info->lt_index_min;
                tile_info->tile_index_max = ir_info->lt_index_max;

                tile_info->logical_multiplier = memory->logical_multiplier;
                tile_info->pt_index_bit_size = memory->pt_index_bit_size;
            } else if (tile_info != NULL) {
                tile_info->tile_index_max = ir_info->lt_index_max;
            }
        }
    }

    lt_map->tile_range_list->tile_num = tix + 1;
}

/*!
 * \brief Index LT tile mode ACC_MODE update.
 *
 * This function sets the access mode during tile reconfiguration
 * to match the curent physical table width settings.
 *
 * \param [in] unit       Unit number.
 * \param [in] am_list    Access mode registers update list.
 * \param [in] ev_data    Data associated with the event.
 *
 * \retval SHR_E_NONE  OK
 * \retval !SHR_E_NONE ERROR
 */
static int
bcmptm_itx_acc_mode_update(int unit,
                           const bcmptm_tile_acc_mode_list_t *am_list,
                           uint64_t ev_data)
{
    uint32_t entry_data[BCMDRD_MAX_PT_WSIZE];
    uint32_t tix, tmix, acc_mode = 0;
    const bcmptm_tile_acc_mode_info_t *tam_info;
    const bcmptm_tile_acc_mode_value_t *tamv;
    bcmdrd_sym_field_info_t finfo;
    bcmltd_op_arg_t op_arg;
    bcmltd_sid_t ltid;
    uint32_t tile_mode, seed;
    bool robust;

    SHR_FUNC_ENTER(unit);

    op_arg.trans_id = BCMPTM_UFT_EV_INFO_TRANS_ID_GET(ev_data);
    op_arg.attrib = 0;
    ltid = BCMPTM_UFT_EV_INFO_LTID_GET(ev_data);

    for (tix = 0; tix < am_list->tile_acc_mode_info_count; tix++) {
        tam_info = &(am_list->tile_acc_mode_info[tix]);
        sal_memset(entry_data, 0, SHR_WORDS2BYTES(BCMDRD_MAX_PT_WSIZE));

        if (tam_info->tile_mode_values_count == 1) {
            /* Fixed ACC_MODE values to retain on reconfig */
            acc_mode = tam_info->tile_mode_values[0].acc_mode_value;
        } else {
            SHR_IF_ERR_EXIT
                (bcmptm_uft_tile_cfg_get(unit, tam_info->tile_id,
                                         &tile_mode, &robust, &seed));

            for (tmix = 0; tmix < tam_info->tile_mode_values_count; tmix++) {
                tamv = &(tam_info->tile_mode_values[tmix]);
                if (tamv->tile_mode == tile_mode) {
                    acc_mode = tamv->acc_mode_value;
                    break;
                }
            }

            if (tmix == tam_info->tile_mode_values_count) {
                /* No match */
                continue;
            }
        }

        /* Update register value */
        SHR_IF_ERR_EXIT
            (bcmptm_sbr_lt_hw_read(unit, FALSE,
                                   ltid,
                                   &op_arg,
                                   tam_info->sid_acc_moder,
                                   0, /* No index for this register */
                                   BCMDRD_MAX_PT_WSIZE,
                                   entry_data));

        SHR_IF_ERR_EXIT
            (bcmdrd_pt_field_info_get(unit,
                                      tam_info->sid_acc_moder,
                                      tam_info->fid_acc_modef,
                                      &finfo));
        bcmdrd_field_set(entry_data,
                         finfo.minbit, finfo.maxbit,
                         &acc_mode);

        SHR_IF_ERR_EXIT
            (bcmptm_sbr_lt_hw_write(unit, FALSE,
                                    ltid,
                                    &op_arg,
                                    tam_info->sid_acc_moder,
                                    0, /* No index for this register */
                                    entry_data));
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Index LT tile mode change callback.
 *
 * This function recalculates the index LT mappings upon notification
 * from the UFT of a tile mode change.
 *
 * \param [in] unit    Unit number.
 * \param [in] event   Event for which the function had been called for.
 * \param [in] ev_data Data associated with the event.
 *
 * \retval None.
 */
static void
bcmptm_itx_tile_event_handler(int unit,
                              const char *event,
                              uint64_t ev_data)
{
    int rv;
    uint32_t ltix;
    bcmptm_itx_lt_map_list_t *lt_map_list;
    bcmptm_itx_lt_map_t *lt_map;
    bcmptm_lt_lookup_banks_info_t banks_info;

    lt_map_list = itx_maps[unit];
    if (lt_map_list == NULL) {
        return;
    }

    for (ltix = 0; ltix < lt_map_list->lt_num; ltix++) {
        lt_map = &(lt_map_list->lt_maps[ltix]);

        rv = bcmptm_uft_bank_hw_attr_get_from_lt(unit, lt_map->ltid,
                                                 0,
                                                 &banks_info);
        if (SHR_FAILURE(rv)) {
            continue;
        }

        /* Switch to backup if appropriate */
        if (!lt_map->backup_valid) {
            lt_map->active_list = (lt_map->active_list ? 0 : 1);
            lt_map->backup_valid = TRUE;
        }

        bcmptm_itx_lt_index_range_construct(unit, &banks_info, lt_map);

        if (BCMPTM_UFT_EV_INFO_TRANS_ID_GET(ev_data) !=
            BCMPTM_ITX_ID_INVALID) {
            rv = bcmptm_itx_lt_map_to_hw(unit, lt_map, ev_data);
            if (SHR_FAILURE(rv)) {
                continue;
            }
        }
    }

    /* Update ACC_MODE for tiles have multiple settings. */
    if (acc_mode_list[unit] != NULL) {
        if (BCMPTM_UFT_EV_INFO_TRANS_ID_GET(ev_data) !=
            BCMPTM_ITX_ID_INVALID) {
            rv = bcmptm_itx_acc_mode_update(unit, acc_mode_list[unit], ev_data);
            if (SHR_FAILURE(rv)) {
                
            }
        }
    }
}

/*!
 * \brief Convert LT key index to PT values.
 *
 * This function searches the LT to PT map index ranges to find the
 * PT range containing the provided LT index.  It then returns the
 * PT index and PT table ID values.
 *
 * \param [in]  lt_index        LT index to find.
 * \param [in]  lt_index_list   Pointer to LT map range list.
 * \param [out]  pt_index       Pointer to return PT index.
 * \param [out]  pt_id          Pointer to return PT ID.
 * \param [out]  entry_sub      Pointer to return entry subsection.
 *
 * \retval SHR_E_NONE  OK
 * \retval SHR_E_PARAM LT index out of valid range.
 */
static int
bcmptm_itx_lt_map_search(uint32_t lt_index,
                         bcmptm_itx_index_range_list_t *lt_index_list,
                         uint32_t *pt_index,
                         uint32_t *pt_id,
                         uint32_t *entry_sub)
{
    uint32_t map_ix;
    bcmptm_itx_index_range_info_t *lt_idx_range;

    for (map_ix = 0; map_ix < lt_index_list->pt_num; map_ix++) {
        lt_idx_range = &(lt_index_list->pt_map_list[map_ix]);
        if ((lt_index >= lt_idx_range->lt_index_min) &&
            (lt_index <= lt_idx_range->lt_index_max)) {
            if (lt_idx_range->logical_multiplier != 0) {
                *pt_index = (lt_index - lt_idx_range->lt_index_min) /
                    lt_idx_range->logical_multiplier;
                *entry_sub = (lt_index - lt_idx_range->lt_index_min) %
                    lt_idx_range->logical_multiplier;
            } else {
                *pt_index = lt_index - lt_idx_range->lt_index_min;
                *entry_sub = 0;
            }
            *pt_id = lt_idx_range->pt_id;
            return SHR_E_NONE;
        }
    }

    return SHR_E_PARAM;
}

/*!
 * \brief Convert PT values to LT key index.
 *
 * This function combines the PT index, PT table ID, and PT
 * entry subsection values to produce the corresponding LT index.
 *
 * \param [in]  pt_index        Physical table index.
 * \param [in]  pt_id           Physical table ID.
 * \param [in]  entry_sub       Physical entry subsection.
 * \param [in]  lt_index_list   Pointer to LT map range list.
 * \param [out] lt_index        LT index to find.
 *
 * \retval SHR_E_NONE  OK
 * \retval SHR_E_PARAM LT index out of valid range.
 */
static int
bcmptm_itx_lt_index_construct(uint32_t pt_index,
                              uint32_t pt_id,
                              uint32_t entry_sub,
                              bcmptm_itx_index_range_list_t *lt_index_list,
                              uint32_t *lt_index)
{
    uint32_t map_ix;
    bcmptm_itx_index_range_info_t *lt_idx_range;

    for (map_ix = 0; map_ix < lt_index_list->pt_num; map_ix++) {
        lt_idx_range = &(lt_index_list->pt_map_list[map_ix]);
        if (pt_id == lt_idx_range->pt_id) {
            if (lt_idx_range->logical_multiplier != 0) {
                *lt_index = (pt_index * lt_idx_range->logical_multiplier)
                    + lt_idx_range->lt_index_min + entry_sub;
            } else {
                *lt_index = pt_index + lt_idx_range->lt_index_min;
            }
            return SHR_E_NONE;
        }
    }

    return SHR_E_INTERNAL;
}

/*!
 * \brief Convert PT ID to PT select index.
 *
 * This function searches the transform argument table select list
 * to match the provided PT ID.  If found, the select index is returned. *
 * \param [in]  pt_id         PT ID to find.
 * \param [in]  arg           Transform arguments.
 * \param [out]  pt_select    Pointer to return PT select index.
 *
 * \retval SHR_E_NONE  OK
 * \retval !SHR_E_NONE ERROR
 */
static int
bcmptm_itx_ptid_to_pt_select(uint32_t pt_id,
                             const bcmltd_transform_arg_t *arg,
                             uint32_t *pt_select)
{
    uint32_t select_ix;

    for (select_ix = 0; select_ix < arg->tables; select_ix++) {
        if (pt_id == arg->table[select_ix]) {
           *pt_select = select_ix;
            return SHR_E_NONE;
        }
    }

    return SHR_E_INTERNAL;
}

/*!
 * \brief Comparison function for ITX metadata binary search.
 *
 * The ITX metadata array, ordered by LTID, is searched
 * in the bcmptm_itx_lt_map_get function.
 * The binary search algorithm requires this comparison function.
 *
 * \param [in] ltid_p Pointer to LTID match.
 * \param [in] itx_map_p Pointer to ITX LT map metadata to compare.
 */
static int
itx_lt_map_compare(const void *ltid_p,
                   const void *itx_map_p)
{
    const uint32_t ltid = *(uint32_t *)ltid_p;
    const bcmptm_itx_lt_map_t *itx_map = itx_map_p;

    if (ltid < itx_map->ltid) {
        return -1;
    } else if (ltid > itx_map->ltid) {
        return 1;
    } else {
        return 0;
    }
}

/*!
 * \brief Retrieve Index LT tile mode map.
 *
 * This function searches the list of tile mode index LTs for a map
 * structure mathcing the provided LTID.
 *
 * \param [in]  unit            Unit number.
 * \param [in]  ltid            Logical table ID.
 *
 * \retval Pointer to LT map if found, NULL if not found.
 */
static bcmptm_itx_lt_map_t *
bcmptm_itx_lt_map_get(int unit,
                      uint32_t ltid)
{
    bcmptm_itx_lt_map_list_t *itx_map;

    itx_map = itx_maps[unit];
    if (itx_map == NULL) {
        return NULL;
    }

    /* Binary search of metadata, which is ordered by (fid, fidx) */
    return sal_bsearch(&ltid,
                       itx_map->lt_maps, itx_map->lt_num,
                       sizeof(bcmptm_itx_lt_map_t),
                       itx_lt_map_compare);
}

/*!
 * \brief Implemenation of ITX PT index, PTID, and entry subunit transform.
 *
 * Convert tile-aware LT key field index to PT entry subunit, PT select,
 * and PT index.
 *
 * \param [in]  unit          Unit Number.
 * \param [in]  in            If TRUE, multiple logical entries per physical.
 * \param [in]  in            Input field values.
 * \param [out] out           Output field values.
 * \param [in]  arg           Transform arguments.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
static int
bcmptm_itx_index_select_transform_core(int unit,
                                       bool multi_l2p,
                                       const bcmltd_fields_t *in,
                                       bcmltd_fields_t *out,
                                       const bcmltd_transform_arg_t *arg)
{
    bcmptm_itx_lt_map_t *lt_map;
    uint32_t lt_idx, pt_idx, pt_id, pt_select, entry_sub;

    SHR_FUNC_ENTER(unit);

    lt_map = bcmptm_itx_lt_map_get(unit, arg->comp_data->sid);
    SHR_NULL_CHECK(lt_map, SHR_E_UNAVAIL);

    if (in->count != 1) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    lt_idx = in->field[0]->data;

    LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit,
                "bcmptm_itx_index_select_transform LT index = %d\n"), lt_idx));

    SHR_IF_ERR_EXIT
        (bcmptm_itx_lt_map_search(lt_idx,
                           lt_map->index_range_list[lt_map->active_list],
                                  &pt_idx, &pt_id, &entry_sub));

    SHR_IF_ERR_EXIT
        (bcmptm_itx_ptid_to_pt_select(pt_id, arg, &pt_select));

    out->count = 0;

    /* output __INDEX */
    out->field[out->count]->id   = arg->rfield[out->count];
    out->field[out->count]->data = pt_idx;
    out->count++;

    /* output __TABLE_SEL */
    out->field[out->count]->id   = arg->rfield[out->count];
    out->field[out->count]->data = pt_select;
    out->count++;

    if (multi_l2p) {
        /* output __SELECTOR */
        out->field[out->count]->id   = arg->rfield[out->count];
        out->field[out->count]->data = entry_sub;
        out->count++;
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Implemenation of ITX PT index, PTID, and entry subunit
 *        reverse transform.
 *
 * Convert tile-aware PT entry subunit, PT select, and PT index to
 * LT key field index.
 *
 * \param [in]  unit          Unit Number.
 * \param [in]  in            If TRUE, multiple logical entries per physical.
 * \param [in]  in            Input field values.
 * \param [out] out           Output field values.
 * \param [in]  arg           Transform arguments.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
static int
bcmptm_itx_index_select_rev_transform_core(int unit,
                                           bool multi_l2p,
                                           const bcmltd_fields_t *in,
                                           bcmltd_fields_t *out,
                                           const bcmltd_transform_arg_t *arg)
{
    bcmptm_itx_lt_map_t *lt_map;
    uint32_t lt_idx, pt_idx = 0, pt_id, pt_select = 0, entry_sub = 0, if_idx;

    SHR_FUNC_ENTER(unit);

    lt_map = bcmptm_itx_lt_map_get(unit, arg->comp_data->sid);
    SHR_NULL_CHECK(lt_map, SHR_E_UNAVAIL);

    if (multi_l2p) {
        if (in->count != 3) {
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
    } else {
        if (in->count != 2) {
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
    }

    /* Search for PT index */
    for (if_idx = 0; if_idx < in->count; if_idx++) {
        if ((in->field[if_idx]->id ==
             arg->field_list->field_array[0].field_id) &&
            (in->field[if_idx]->idx ==
             arg->field_list->field_array[0].field_idx)) {
            pt_idx = in->field[if_idx]->data;
            break;
        }
    }
    if (if_idx == in->count) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    /* Search for PT select */
    for (if_idx = 0; if_idx < in->count; if_idx++) {
        if ((in->field[if_idx]->id ==
             arg->field_list->field_array[1].field_id) &&
            (in->field[if_idx]->idx ==
             arg->field_list->field_array[1].field_idx)) {
             pt_select = in->field[if_idx]->data;
            break;
        }
    }
    if (if_idx == in->count) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    if (multi_l2p) {
        /* Search for entry subsection */
        for (if_idx = 0; if_idx < in->count; if_idx++) {
            if ((in->field[if_idx]->id ==
                 arg->field_list->field_array[2].field_id) &&
                (in->field[if_idx]->idx ==
                 arg->field_list->field_array[2].field_idx)) {
                entry_sub = in->field[if_idx]->data;
                break;
            }
        }
        if (if_idx == in->count) {
            SHR_ERR_EXIT(SHR_E_INTERNAL);
        }
    }

    /* Translate PT select to PT id */
    pt_id = arg->table[pt_select];

    LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit,
                "bcmptm_itx_index_select_rev_transform\n"
                "PT index = %d, PT select = %d, PT id = %d"
                                            " Entry subsection = %d"),
                                 pt_idx, pt_select, pt_id, entry_sub));

    SHR_IF_ERR_EXIT
        (bcmptm_itx_lt_index_construct(pt_idx, pt_id, entry_sub,
                           lt_map->index_range_list[lt_map->active_list],
                                       &lt_idx));

    out->count = 0;

    /* output LT key index */
    out->field[out->count]->id   = arg->rfield[out->count];
    out->field[out->count]->data = lt_idx;
    out->count++;

exit:
    SHR_FUNC_EXIT();
}

/*******************************************************************************
 * Public Functions
 */

int
bcmptm_itx_init(int unit, bool warm)
{
    uint32_t ltix, ptix;
    bcmptm_itx_lt_map_list_t *lt_map_list = NULL;
    bcmptm_itx_lt_map_t *lt_map;
    const bcmptm_tile_list_t *tile_list;
    const bcmptm_tile_info_t *hw_tile_info;
    bcmptm_itx_index_range_list_t *index_range_list;
    bcmptm_itx_tile_range_list_t *tile_range_list;
    const bcmptm_tile_memory_t *memory;
    uint32_t last_tile_id = BCMPTM_ITX_ID_INVALID;
    uint32_t itx_lt_count, pt_count, tile_count;
    uint32_t alloc_size;
    bcmlrd_variant_t variant;
    uint64_t ev_data;

    SHR_FUNC_ENTER(unit);

    variant = bcmlrd_variant_get(unit);
    if (variant == BCMLRD_VARIANT_T_NONE) {
        SHR_IF_ERR_EXIT(SHR_E_UNIT);
    }
    tile_list = bcmptm_itx_tile_list[variant];
    if (tile_list == NULL) {
        SHR_EXIT();
    }

    /* Retrieve root Index LT tile mode info structure for unit */
    itx_lt_count = tile_list->tile_info_count;

    if (itx_lt_count == 0) {
        /* No index tile mode LTs */
        SHR_EXIT();
    }

    /* Space for LT records. */
    alloc_size = sizeof(bcmptm_itx_lt_map_list_t) +
        sizeof(bcmptm_itx_lt_map_t) * itx_lt_count;
    SHR_ALLOC(lt_map_list, alloc_size, "bcmptmItxLtMapList");
    SHR_NULL_CHECK(lt_map_list, SHR_E_MEMORY);
    sal_memset(lt_map_list, 0, alloc_size);

    itx_maps[unit] = lt_map_list;

    lt_map_list->lt_num = itx_lt_count;

    for (ltix = 0; ltix < itx_lt_count; ltix++) {
        lt_map = &(lt_map_list->lt_maps[ltix]);

        hw_tile_info = &(tile_list->tile_info[ltix]);

        pt_count = hw_tile_info->memory_count;

        lt_map->ltid = hw_tile_info->sid;
        lt_map->pt_max = pt_count;
        lt_map->active_list = 0;
        lt_map->backup_valid = FALSE;
        lt_map->hw_tile_info = hw_tile_info;

        tile_count = 0;
        last_tile_id = BCMPTM_ITX_ID_INVALID;

        for (ptix = 0; ptix < pt_count; ptix++) {
            memory = &(hw_tile_info->memory[ptix]);

            lt_map->entry_maximum += memory->size;

            /*
             * Tile count.
             * This logic depends on the following array order:
             * memory tile information array elements for the same
             * tile must be placed consecutively.
             */
            if (last_tile_id != memory->tile_id) {
                tile_count++;
                last_tile_id = memory->tile_id;
            }
        }
        lt_map->tile_max = tile_count;

        /* Space for LT records. */
        alloc_size = sizeof(bcmptm_itx_index_range_list_t) +
            sizeof(bcmptm_itx_index_range_info_t) * pt_count;

        index_range_list = NULL;
        SHR_ALLOC(index_range_list, alloc_size, "bcmptmItxIndexRangeList0");
        SHR_NULL_CHECK(index_range_list, SHR_E_MEMORY);
        sal_memset(index_range_list, 0, alloc_size);
        lt_map->index_range_list[0] = index_range_list;

        index_range_list = NULL;
        SHR_ALLOC(index_range_list, alloc_size, "bcmptmItxIndexRangeList1");
        SHR_NULL_CHECK(index_range_list, SHR_E_MEMORY);
        sal_memset(index_range_list, 0, alloc_size);
        lt_map->index_range_list[1] = index_range_list;

        /* Space for tile update records. */
        alloc_size = sizeof(bcmptm_itx_tile_range_list_t) +
            sizeof(bcmptm_itx_tile_range_info_t) * tile_count;
        tile_range_list = NULL;
        SHR_ALLOC(tile_range_list, alloc_size, "bcmptmItxTileRangeList1");
        SHR_NULL_CHECK(tile_range_list, SHR_E_MEMORY);
        sal_memset(tile_range_list, 0, alloc_size);
        lt_map->tile_range_list = tile_range_list;
    }

    acc_mode_list[unit] = tile_list->tile_acc_mode_list;

    /* Initialize LT entry maps and LTS_TCAM entries */
    ev_data =
        BCMPTM_UFT_EV_INFO_CONSTRUCT(BCMPTM_ITX_ID_INVALID,
                                     BCMPTM_ITX_ID_INVALID);
    bcmptm_itx_tile_event_handler(unit, NULL, ev_data);
    bcmptm_itx_commit(unit, BCMPTM_ITX_ID_INVALID, NULL);

    /* Register for event notification on tile mode change */
    SHR_IF_ERR_EXIT
        (bcmevm_register_published_event(unit,
                                         BCMPTM_EVENT_UFT_TILE_CHANGE_COMMIT,
                                         bcmptm_itx_tile_event_handler));

exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_itx_stop(int unit)
{
    SHR_FUNC_ENTER(unit);

    if (itx_maps[unit] == NULL) {
        SHR_EXIT();
    }
    SHR_IF_ERR_EXIT
        (bcmevm_unregister_published_event(unit,
                                           BCMPTM_EVENT_UFT_TILE_CHANGE_COMMIT,
                                           bcmptm_itx_tile_event_handler));

exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_itx_cleanup(int unit, bool warm)
{
    uint32_t ltix;
    bcmptm_itx_lt_map_list_t *lt_map_list;

    lt_map_list = itx_maps[unit];

    if (lt_map_list != NULL) {
        for (ltix = 0; ltix < lt_map_list->lt_num; ltix++) {
            SHR_FREE(lt_map_list->lt_maps[ltix].tile_range_list);
            SHR_FREE(lt_map_list->lt_maps[ltix].index_range_list[1]);
            SHR_FREE(lt_map_list->lt_maps[ltix].index_range_list[0]);
        }
        SHR_FREE(itx_maps[unit]);
    }

    return SHR_E_NONE;
}

int
bcmptm_itx_index_select_transform(int unit,
                                  const bcmltd_fields_t *in,
                                  bcmltd_fields_t *out,
                                  const bcmltd_transform_arg_t *arg)
{
    return bcmptm_itx_index_select_transform_core(unit, FALSE, in, out, arg);
}

int
bcmptm_itx_index_select_rev_transform(int unit,
                                      const bcmltd_fields_t *in,
                                      bcmltd_fields_t *out,
                                      const bcmltd_transform_arg_t *arg)
{
    return bcmptm_itx_index_select_rev_transform_core(unit, FALSE,
                                                      in, out, arg);
}

int
bcmptm_itx_multi_index_select_transform(int unit,
                                        const bcmltd_fields_t *in,
                                        bcmltd_fields_t *out,
                                        const bcmltd_transform_arg_t *arg)
{
    return bcmptm_itx_index_select_transform_core(unit, TRUE, in, out, arg);
}

int
bcmptm_itx_multi_index_select_rev_transform(int unit,
                                            const bcmltd_fields_t *in,
                                            bcmltd_fields_t *out,
                                            const bcmltd_transform_arg_t *arg)
{
    return bcmptm_itx_index_select_rev_transform_core(unit, TRUE,
                                                      in, out, arg);
}

int
bcmptm_itx_prepare(int unit,
                   uint32_t trans_id,
                   const bcmltd_generic_arg_t *arg)
{
    uint32_t ltix;
    bcmptm_itx_lt_map_list_t *lt_map_list;

    SHR_FUNC_ENTER(unit);

    lt_map_list = itx_maps[unit];
    SHR_NULL_CHECK(lt_map_list, SHR_E_INIT);

    for (ltix = 0; ltix < lt_map_list->lt_num; ltix++) {
        if (lt_map_list->lt_maps[ltix].backup_valid) {
            SHR_ERR_EXIT(SHR_E_INTERNAL);
        }
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_itx_commit(int unit,
                  uint32_t trans_id,
                  const bcmltd_generic_arg_t *arg)
{
    uint32_t ltix;
    bcmptm_itx_lt_map_list_t *lt_map_list;

    SHR_FUNC_ENTER(unit);

    lt_map_list = itx_maps[unit];
    SHR_NULL_CHECK(lt_map_list, SHR_E_INIT);

    for (ltix = 0; ltix < lt_map_list->lt_num; ltix++) {
        if (lt_map_list->lt_maps[ltix].backup_valid) {
            /* Discard backup index range list */
            lt_map_list->lt_maps[ltix].backup_valid = FALSE;
        }
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_itx_abort(int unit,
                 uint32_t trans_id,
                 const bcmltd_generic_arg_t *arg)
{
    uint32_t ltix;
    bcmptm_itx_lt_map_list_t *lt_map_list;
    bcmptm_itx_lt_map_t *lt_map;

    SHR_FUNC_ENTER(unit);

    lt_map_list = itx_maps[unit];
    SHR_NULL_CHECK(lt_map_list, SHR_E_INIT);

    for (ltix = 0; ltix < lt_map_list->lt_num; ltix++) {
        lt_map = &(lt_map_list->lt_maps[ltix]);
        if (lt_map->backup_valid) {
            /* Revert to backup index range list */
            lt_map->active_list = (lt_map->active_list ? 0 : 1);
            lt_map->backup_valid = FALSE;
        }
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_itx_entry_limit_get(int unit,
                           uint32_t trans_id,
                           bcmltd_sid_t sid,
                           const bcmltd_table_entry_limit_arg_t *table_arg,
                           bcmltd_table_entry_limit_t *table_data,
                           const bcmltd_generic_arg_t *arg)
{
    bcmptm_itx_lt_map_t *lt_map;

    SHR_FUNC_ENTER(unit);

    lt_map = bcmptm_itx_lt_map_get(unit, sid);
    SHR_NULL_CHECK(lt_map, SHR_E_UNAVAIL);

    table_data->entry_limit =
        lt_map->index_range_list[lt_map->active_list]->entry_limit;

exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_itx_tile_list_get(int unit,
                         const bcmptm_tile_list_t **tile_list)
{
    bcmlrd_variant_t variant;

    SHR_FUNC_ENTER(unit);

    variant = bcmlrd_variant_get(unit);
    if (variant == BCMLRD_VARIANT_T_NONE) {
        SHR_IF_ERR_EXIT(SHR_E_UNIT);
    }
    *tile_list = bcmptm_itx_tile_list[variant];

exit:
    SHR_FUNC_EXIT();
}
