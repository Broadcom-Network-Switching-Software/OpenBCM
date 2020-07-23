/*! \file bcmltm_state.c
 *
 * Logical Table Manager
 *
 * This module contains routines for the modification, retrieval, and
 * manipulation of Logical Table state.
 *
 * Each LT has a basic set of table state, which may include other optional
 * elements for the overall state.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_debug.h>
#include <shr/shr_util.h>
#include <bsl/bsl.h>
#include <bcmlrd/bcmlrd_table.h>
#include <bcmltm/bcmltm_util_internal.h>
#include <bcmltm/bcmltm_state_internal.h>
#include <bcmltm/bcmltm_dump.h>


/*******************************************************************************
 * Local definitions
 */

/* Debug log target definition */
#define BSL_LOG_MODULE BSL_LS_BCMLTM_STATE

static bcmltm_state_mgmt_t lt_state_mgmt[BCMDRD_CONFIG_MAX_UNITS];

#define BCMLTM_BITMAP_HEX_CHUNK 8
#define BCMLTM_BITMAP_CHUNK_BITS 32
#define BCMLTM_BITMAP_CHUNKS_LINE 6
#define BCMLTM_BITMAP_LINE_BITS \
    (BCMLTM_BITMAP_CHUNK_BITS * BCMLTM_BITMAP_CHUNKS_LINE)

/*******************************************************************************
 * Private functions
 */

/*!
 * \brief Display LT state bitmap.
 *
 * Dump the LT state information for an entry bitmap.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_state Pointer to LT state structure.
 * \param [in] data Pointer to LT state bitmap.
 * \param [in,out] pb Print buffer to populate output data.
 *
 * \retval None
 */
static void
bcmltm_lt_bitmap_dump(int unit,
                      bcmltm_lt_state_t *lt_state,
                      void *data,
                      shr_pb_t *pb)
{
    uint32_t bitmap_size, bit, chunk_val;
    SHR_BITDCL *bitmap;
    char chunk[BCMLTM_BITMAP_HEX_CHUNK + 1];
    SHR_PB_LOCAL_DECL(pb);

    bitmap_size = lt_state->bitmap_size;
    bitmap = data;

    chunk_val = 0;
    shr_pb_printf(pb, "    %6d: ", 0);
    for (bit = 0; bit < bitmap_size; bit++) {
        chunk_val <<= 1;
        if (SHR_BITGET(bitmap, bit)) {
            chunk_val |= 1;
        }

        /* Convert to hex character */
        if (((bit + 1) % BCMLTM_BITMAP_CHUNK_BITS) == 0) {
            shr_util_format_uint32(chunk, chunk_val,
                                   BCMLTM_BITMAP_HEX_CHUNK, 16);
            shr_pb_printf(pb, " %s", chunk);
            chunk_val = 0;
        }
        /* Next line */
        if (((bit + 1) % BCMLTM_BITMAP_LINE_BITS) == 0) {
            shr_pb_printf(pb, "\n    %6d: ", bit);
        }
    }

    /* Cleanup final line */
    if ((bit % BCMLTM_BITMAP_CHUNK_BITS) != 0) {
        chunk_val <<= 32 - (bit % BCMLTM_BITMAP_CHUNK_BITS);
        shr_util_format_uint32(chunk, chunk_val,
                               BCMLTM_BITMAP_HEX_CHUNK, 16);
        shr_pb_printf(pb, " %s", chunk);
        chunk_val = 0;
    }
    /* Final line */
    if (((bit + 1) % BCMLTM_BITMAP_LINE_BITS) != 0) {
        shr_pb_printf(pb, "\n");
    }

    SHR_PB_LOCAL_DONE();
}

/*!
 * \brief Display LT state data information.
 *
 * Dump the LT state data information per state data type.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_state Pointer to LT state structure.
 * \param [in,out] pb Print buffer to populate output data.
 *
 * \retval None
 */
static void
bcmltm_state_lt_data_dump(int unit,
                          bcmltm_lt_state_t *lt_state,
                          shr_pb_t *pb)
{
    bcmltm_lt_state_data_t *state_iter;
    bcmltm_ha_ptr_t state_iter_hap, state_data_hap;
    void *data;
    const char *sdt_str[BCMLTM_STATE_TYPE_COUNT] = {
        "Inuse Bitmap",
        "Valid Bitmap",
        "Global Bitmap"
    };
    SHR_PB_LOCAL_DECL(pb);

    state_iter_hap = lt_state->table_state_data_hap;

    while (state_iter_hap != BCMLTM_HA_PTR_INVALID) {
        state_iter = bcmltm_ha_ptr_to_mem_ptr(unit, state_iter_hap);
        if (state_iter == NULL) {
            break;
        }
        if (state_iter->state_data_type >= BCMLTM_STATE_TYPE_COUNT) {
            /* Invalid state data type */
            continue;
        }

        shr_pb_printf(pb, "    === State data for LT ID %d ===\n",
                      state_iter->ltid);
        shr_pb_printf(pb, "        Size %d, Type %s\n",
                      state_iter->state_data_size,
                      sdt_str[state_iter->state_data_type]);

        state_data_hap = state_iter->state_data_hap;
        data = bcmltm_ha_ptr_to_mem_ptr(unit, state_data_hap);
        if (data == NULL) {
            break;
        }

        /* Some bitmap */
        bcmltm_lt_bitmap_dump(unit, lt_state, data, pb);

        state_iter_hap = state_iter->next_state_data_hap;
    }

    SHR_PB_LOCAL_DONE();
}

/*******************************************************************************
 * Public functions
 */

bcmltm_state_mgmt_t *
bcmltm_state_mgmt_get(int unit)
{
    return &(lt_state_mgmt[unit]);
}

int
bcmltm_state_data_get(int unit,
                      bcmltm_lt_state_t *lt_state,
                      bcmltm_state_type_t state_type,
                      void **state_data)
{
    bcmltm_lt_state_data_t *state_iter;
    bcmltm_ha_ptr_t state_iter_hap, state_data_hap;

    state_iter_hap = lt_state->table_state_data_hap;

    while (state_iter_hap != BCMLTM_HA_PTR_INVALID) {
        state_iter = bcmltm_ha_ptr_to_mem_ptr(unit, state_iter_hap);
        if (state_iter == NULL) {
            break;
        }
        if (state_iter->state_data_type == state_type) {
            state_data_hap = state_iter->state_data_hap;
            *state_data = bcmltm_ha_ptr_to_mem_ptr(unit, state_data_hap);
            return SHR_E_NONE;
        }
        state_iter_hap = state_iter->next_state_data_hap;
    }

    *state_data = NULL;
    return SHR_E_NOT_FOUND;
}

int
bcmltm_state_data_size_get(int unit,
                           bcmltm_lt_state_t *lt_state,
                           bcmltm_state_type_t state_type,
                           uint32_t *state_data_size)
{
    bcmltm_lt_state_data_t *state_iter;
    bcmltm_ha_ptr_t state_iter_hap;

    state_iter_hap = lt_state->table_state_data_hap;

    while (state_iter_hap != BCMLTM_HA_PTR_INVALID) {
        state_iter = bcmltm_ha_ptr_to_mem_ptr(unit, state_iter_hap);
        if (state_iter == NULL) {
            break;
        }
        if (state_iter->state_data_type == state_type) {
            *state_data_size = state_iter->state_data_size;
            return SHR_E_NONE;
        }
        state_iter_hap = state_iter->next_state_data_hap;
    }

    return SHR_E_NOT_FOUND;
}

int
bcmltm_state_copy(int unit,
                  bcmltm_ha_ptr_t lt_state_src_hap,
                  bcmltm_ha_ptr_t lt_state_dst_hap)
{
    bcmltm_lt_state_t *lt_state_src, *lt_state_dst;
    bcmltm_ha_ptr_t tsd_src_hap, tsd_dst_hap;
    bcmltm_lt_state_data_t *tsd_src, *tsd_dst;
    void *data_src, *data_dst;

    lt_state_src = bcmltm_ha_ptr_to_mem_ptr(unit, lt_state_src_hap);
    lt_state_dst = bcmltm_ha_ptr_to_mem_ptr(unit, lt_state_dst_hap);

    if ((lt_state_src == NULL) || (lt_state_dst == NULL)) {
        return SHR_E_INTERNAL;
    }

    /* Note the link before copying over it */
    tsd_dst_hap = lt_state_dst->table_state_data_hap;

    sal_memcpy(lt_state_dst, lt_state_src, sizeof(bcmltm_lt_state_t));

    /* Restore link */
    lt_state_dst->table_state_data_hap = tsd_dst_hap;

    /* Follow chain of source LT state */
    tsd_src_hap = lt_state_src->table_state_data_hap;

    while (!BCMLTM_HA_PTR_IS_INVALID(tsd_src_hap)) {
        tsd_src = bcmltm_ha_ptr_to_mem_ptr(unit, tsd_src_hap);
        tsd_dst = bcmltm_ha_ptr_to_mem_ptr(unit, tsd_dst_hap);
        if ((tsd_src == NULL) || (tsd_dst == NULL)) {
            return SHR_E_INTERNAL;
        }

        if ((tsd_dst->state_data_size != tsd_src->state_data_size) ||
            (tsd_dst->state_data_type != tsd_src->state_data_type)) {
            /* Mismatched src and destination characteristics */
            return SHR_E_INTERNAL;
        }

        data_src = bcmltm_ha_ptr_to_mem_ptr(unit, tsd_src->state_data_hap);
        data_dst = bcmltm_ha_ptr_to_mem_ptr(unit, tsd_dst->state_data_hap);
        if ((data_src == NULL) || (data_dst == NULL)) {
            return SHR_E_INTERNAL;
        }

        /* Copy state data */
        sal_memcpy(data_dst, data_src, tsd_src->state_data_size);

        tsd_src_hap = tsd_src->next_state_data_hap;
        tsd_dst_hap = tsd_dst->next_state_data_hap;
    }

    if (!BCMLTM_HA_PTR_IS_INVALID(tsd_dst_hap)) {
        return SHR_E_INTERNAL;
    }

    return SHR_E_NONE;
}

int
bcmltm_state_clone(int unit,
                   uint8_t lt_idx,
                   bcmltm_ha_ptr_t lt_state_src_hap,
                   bcmltm_ha_ptr_t *lt_state_dst_hap_p)
{
    bcmltm_lt_state_t *lt_state_src, *lt_state_dst;
    bcmltm_ha_ptr_t tsd_src_hap, tsd_dst_hap, *last_tsd_dst_hap;
    bcmltm_lt_state_data_t *tsd_src, *tsd_dst;
    bcmltm_ha_ptr_t state_data_dst_hap;

    /* Fetch the rollback state pointer for this lt_idx */
    *lt_state_dst_hap_p =
        lt_state_mgmt[unit].lt_state_rollback_chunk_hap[lt_idx];

    lt_state_src = bcmltm_ha_ptr_to_mem_ptr(unit, lt_state_src_hap);
    lt_state_dst = bcmltm_ha_ptr_to_mem_ptr(unit, *lt_state_dst_hap_p);

    if ((lt_state_src == NULL) || (lt_state_dst == NULL)) {
        return SHR_E_INTERNAL;
    }

    /* Do not copy LT state here */

    /* Follow chain of source LT state */
    tsd_src_hap = lt_state_src->table_state_data_hap;

    /* Fetch the rollback state data pointer for this lt_idx */
    tsd_dst_hap =
        lt_state_mgmt[unit].lt_state_data_rollback_chunk_hap[lt_idx];
    last_tsd_dst_hap = &(lt_state_dst->table_state_data_hap);

    while (!BCMLTM_HA_PTR_IS_INVALID(tsd_src_hap)) {
        /* Link last structure to this one */
        *last_tsd_dst_hap = tsd_dst_hap;

        tsd_src = bcmltm_ha_ptr_to_mem_ptr(unit, tsd_src_hap);
        tsd_dst = bcmltm_ha_ptr_to_mem_ptr(unit, tsd_dst_hap);
        if ((tsd_src == NULL) || (tsd_dst == NULL)) {
            return SHR_E_INTERNAL;
        }

        /* Establish structure links for state data record */
        sal_memcpy(tsd_dst, tsd_src, sizeof(bcmltm_lt_state_data_t));

        state_data_dst_hap =
            bcmltm_ha_ptr_increment(unit, tsd_dst_hap,
                                    sizeof(bcmltm_lt_state_data_t));

        if (BCMLTM_HA_PTR_IS_INVALID(state_data_dst_hap)) {
            /* Out of reserved HA space? */
            return SHR_E_INTERNAL;
        }

        /* Do not copy state data yet */
        tsd_dst->state_data_hap = state_data_dst_hap;

        last_tsd_dst_hap = &(tsd_dst->next_state_data_hap);
        tsd_dst_hap =
            bcmltm_ha_ptr_increment(unit, state_data_dst_hap,
                                    tsd_dst->state_data_size);

        if (BCMLTM_HA_PTR_IS_INVALID(tsd_dst_hap)) {
            /* Out of reserved HA space? */
            return SHR_E_INTERNAL;
        }
        /* Advance to next glob */
        tsd_src_hap = tsd_src->next_state_data_hap;
    }

    *last_tsd_dst_hap = BCMLTM_HA_PTR_INVALID;

    return bcmltm_state_copy(unit, lt_state_src_hap, *lt_state_dst_hap_p);
}

void
bcmltm_state_lt_get_register(int unit,
                             bcmltm_state_lt_get_f state_get_cb)
{
    lt_state_mgmt[unit].lt_state_get_cb = state_get_cb;
}

void
bcmltm_info_lt_get_register(int unit,
                            bcmltm_info_lt_get_f info_get_cb)
{
    lt_state_mgmt[unit].lt_info_get_cb = info_get_cb;
}

void
bcmltm_state_lt_valid_bitmap_update_register(int unit,
                                    bcmltm_state_lt_valid_bitmap_update_f
                                    lt_valid_bitmap_update_cb)
{
    lt_state_mgmt[unit].lt_valid_bitmap_update_cb = lt_valid_bitmap_update_cb;
}

void
bcmltm_entry_limit_get_register(int unit,
                                bcmltm_entry_limit_get_f entry_limit_get_cb)
{
    lt_state_mgmt[unit].entry_limit_get_cb = entry_limit_get_cb;
}

void
bcmltm_entry_usage_get_register(int unit,
                                bcmltm_entry_usage_get_f entry_usage_get_cb)
{
    lt_state_mgmt[unit].entry_usage_get_cb = entry_usage_get_cb;
}

int
bcmltm_state_lt_get(int unit,
                    uint32_t ltid,
                    bcmltm_lt_state_t **lt_state_p)
{
    if (lt_state_mgmt[unit].lt_state_get_cb == NULL) {
        if (lt_state_p != NULL) {
            *lt_state_p = NULL;
        }
        return SHR_E_UNAVAIL;
    }

    return lt_state_mgmt[unit].lt_state_get_cb(unit, ltid, lt_state_p);
}

int
bcmltm_info_lt_get(int unit,
                   uint32_t table_id,
                   uint32_t field_id,
                   uint32_t field_idx,
                   uint64_t *data_value)
{
    if (lt_state_mgmt[unit].lt_info_get_cb == NULL) {
        return SHR_E_UNAVAIL;
    }

    return lt_state_mgmt[unit].lt_info_get_cb(unit, table_id, field_id,
                                field_idx, data_value);
}

int
bcmltm_state_lt_valid_bitmap_update(int unit,
                                    uint32_t table_id)
{
    if (lt_state_mgmt[unit].lt_valid_bitmap_update_cb == NULL) {
        return SHR_E_UNAVAIL;
    }

    return lt_state_mgmt[unit].lt_valid_bitmap_update_cb(unit, table_id);
}

int
bcmltm_entry_limit_get(int unit,
                       uint32_t table_id,
                       const bcmltd_table_entry_limit_handler_t
                       **table_entry_limit)
{
    if (lt_state_mgmt[unit].entry_limit_get_cb == NULL) {
        return SHR_E_UNAVAIL;
    }

    return lt_state_mgmt[unit].entry_limit_get_cb(unit, table_id,
                                                  table_entry_limit);
}

int
bcmltm_entry_usage_get(int unit,
                       uint32_t table_id,
                       const bcmltd_table_entry_usage_handler_t
                       **table_entry_usage)
{
    if (lt_state_mgmt[unit].entry_usage_get_cb == NULL) {
        return SHR_E_UNAVAIL;
    }

    return lt_state_mgmt[unit].entry_usage_get_cb(unit, table_id,
                                                  table_entry_usage);
}

void
bcmltm_state_lt_dump(int unit,
                     bcmltm_lt_state_t *lt_state,
                     shr_pb_t *pb)
{
    uint32_t ltid;
    const char *name;
    SHR_PB_LOCAL_DECL(pb);

    if (lt_state == NULL) {
        return;
    }

    ltid = lt_state->ltid;
    name = bcmltm_lt_table_sid_to_name(unit, ltid);

    shr_pb_printf(pb, "=== State for LT ID %d: %s ===\n", ltid, name);
    if (lt_state->initialized) {
        shr_pb_printf(pb, "    Table op PT info %sabled\n",
                      lt_state->table_op_pt_info_enable ? "en" : "dis");
        shr_pb_printf(pb,
                      "    Entry count %d, Max entries %d, Bitmap size %d\n",
                      lt_state->entry_count,
                      lt_state->max_entry_count,
                      lt_state->bitmap_size);

        bcmltm_state_lt_data_dump(unit, lt_state, pb);
    } else {
        shr_pb_printf(pb, "    Uninitialized\n");
    }

    shr_pb_printf(pb, "\n");

    SHR_PB_LOCAL_DONE();
}

void
bcmltm_state_lt_dump_by_id(int unit,
                           uint32_t ltid,
                           shr_pb_t *pb)
{
    int rv;
    bcmltm_lt_state_t *lt_state;
    SHR_PB_LOCAL_DECL(pb);

    rv = bcmltm_state_lt_get(unit, ltid, &lt_state);
    if (SHR_FAILURE(rv)) {
        shr_pb_printf(pb, "LT %d state retrieval failure: %s\n",
                      ltid, shr_errmsg(rv));
    } else {
        bcmltm_state_lt_dump(unit, lt_state, pb);
    }

    SHR_PB_LOCAL_DONE();
}

void
bcmltm_state_all_dump(int unit,
                      shr_pb_t *pb)
{
    bcmltm_ha_ptr_t state_block_hap = BCMLTM_HA_PTR_INVALID;
    bcmltm_ha_block_header_t *block_header;
    bcmltm_lt_state_t *lt_state_base, *lt_state;
    int32_t lt_count, ltix;
    SHR_PB_LOCAL_DECL(pb);

    /* Get HA pointer to start of LT state */
    state_block_hap = BCMLTM_HA_PTR(BCMLTM_HA_BLOCK_TYPE_STATE, 0);
    block_header = bcmltm_ha_ptr_to_mem_ptr(unit, state_block_hap);
    lt_state_base =
        (bcmltm_lt_state_t *)(((char *)block_header) + sizeof(*block_header));

    /* Determine number of LT state records in HA */
    lt_count =
        (block_header->allocated_size - sizeof(*block_header)) /
        sizeof(bcmltm_lt_state_t);

    shr_pb_printf(pb, "LTM state for all %d Logical Tables\n", lt_count);

    for (ltix = 0; ltix < lt_count; ltix++) {
        lt_state = &(lt_state_base[ltix]);
        shr_pb_printf(pb, "LT count %d\n", ltix);
        bcmltm_state_lt_dump(unit, lt_state, pb);
    }

    SHR_PB_LOCAL_DONE();
}
