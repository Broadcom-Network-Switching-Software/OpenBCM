/*! \file inmem_util.c
 *
 * In memory table handler.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <sal/sal_types.h>
#include <sal/sal_mutex.h>
#include <sal/sal_assert.h>
#include <sal/sal_libc.h>
#include <shr/shr_debug.h>
#include <shr/shr_error.h>
#include <shr/shr_ha.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmdrd_config.h>
#include <bcmimm/bcmimm_basic.h>
#include <bcmimm/bcmimm_backend.h>
#include <bcmimm/bcmimm_int_comp.h>
#include <bcmimm/generated/front_e_ha.h>
#include <bcmimm/bcmimm_internal.h>

/*******************************************************************************
 * Local definitions
 */
#define BSL_LOG_MODULE BSL_LS_BCMIMM_FRONTEND


/*******************************************************************************
 * Private functions
 */
/*!
 *\brief Separate the input fields to key and data field lists.
 *
 * This function constructs key and data field lists from the input
 * that contains all the fields.
 *
 * \param [in] in Is the input parameter containing the key and data fields
 * mixed together.
 * \param [out] out_k_fields Is the list containing only key fields.
 * \param [out] out_d_fields Is the list containing only data fields.
 * \param [in] tbl Is the in-memory logical table associated with the data.
 * \param [in] dyn_info Dynamic info contains static information gathered
 * from the LRD regarding the LT fields attributes.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int build_field_lists(const bcmltd_fields_t *in,
                             bcmltd_field_t **out_k_fields,
                             bcmltd_field_t **out_d_fields,
                             table_info_t *tbl,
                             bcmimm_table_dynamic_info_t *dyn_info)
{
    bcmltd_field_t *k_field = NULL;    /* Last key field */
    bcmltd_field_t *d_field = NULL;    /* Last data field */
    bcmltd_field_t *gen_field;  /* Generic field (data or key) */
    bcmltd_field_t *output_list;
    const bcmltd_field_t *fld;
    bool is_key;
    size_t j;
    int key_count = 0;

    *out_k_fields = NULL;
    *out_d_fields = NULL;

    /* Each field will be added at the end of its relevant list */
    for (j = 0; j < in->count; j++) {
        fld = in->field[j];
        if (key_count < tbl->key_fld_cnt) {
            is_key = (bcmimm_is_key_field(tbl, dyn_info, fld->id) != -1);
        } else {
            is_key = false;
        }

        if (is_key) {
            key_count++;
            output_list = *out_k_fields;
        } else {
            output_list = *out_d_fields;
        }

        /* Allocate a new field */
        gen_field = shr_fmm_alloc();
        if (!gen_field) {
            bcmimm_internal_free_lists(*out_k_fields, *out_d_fields);
            return SHR_E_MEMORY;
        }

        /* Assign the field info */
        if (fld->flags & SHR_FMM_FIELD_DEL) {
            field_info_t *def_field = dyn_info->data_fields;
            uint32_t k;
            /* Find the default value of the field */
            for (k = 0; k < tbl->data_fld_cnt; k++) {
                if (def_field->fid == fld->id) {
                    break;
                }
                def_field++;
            }
            if (k == tbl->data_fld_cnt) {
                return SHR_E_INTERNAL;
            }
            gen_field->data = def_field->def_val;
        } else {
            gen_field->data = fld->data;
        }
        gen_field->id = fld->id;
        gen_field->idx = fld->idx;
        gen_field->flags = fld->flags;
        gen_field->next = NULL;

        /* If the relevant list is empty */
        if (!output_list) {
            if (is_key) {
                k_field = gen_field;
                *out_k_fields = gen_field;
            } else {
                d_field = gen_field;
                *out_d_fields = gen_field;
            }
        } else {
            if (is_key) {
                k_field->next = gen_field;
                k_field = gen_field;
            } else {
                d_field->next = gen_field;
                d_field = gen_field;
            }
        }
    }
    return SHR_E_NONE;
}

/*!
 * \brief Insert key field into the key buffer.
 *
 * This function places a key field value into the specific location of the
 * field (offset & width) within the key.
 *
 * \param [in] tbl Pointer to the logical table information.
 * \param [in] dyn_info Dynamic info contains static information gathered
 * from the LRD regarding the LT fields attributes.
 * \param [in] key_buf The key buffer where the key value should be inserted.
 * \param [in] idx The index of the key field in the table \c tbl.
 * \param [in] data The value of the key field.
 *
 * \return None
 */
static void bcmimm_insert_key_fld(table_info_t *tbl,
                                  bcmimm_table_dynamic_info_t *dyn_info,
                                  uint8_t *key_buf,
                                  int idx,
                                  uint64_t data)
{
    uint32_t byte_offset;
    uint32_t bit_offset;
    uint32_t k;
    uint8_t *buf;
    uint32_t width; /* The width (in bits) of the key field */
    uint32_t bytes_width;

    /*
     * Due to teh fact that we place 64 bit value into a byte array we can't
     * use regular value assignment but rather memory copy or assign the value
     * one byte at a time.
     * We therefore must select which bytes from the 64 bits input value are
     * relevant and take only those.
     * This brings an issue for big endian architecture since the key value
     * typically occupies the higher bytes so simple memory copy will not
     * capture the key value.
     * Therefore for big endian we right shift the value all the way to
     * the MSB (which is byte 0).
     */

    /* For a single field key simply copy the data */
    if (tbl->key_fld_cnt == 1) {
        bytes_width = BCMIMM_BITS_TO_BYTES(dyn_info->key_fields[0].width);
        if (bcmimm_big_endian) {
            width = dyn_info->key_fields[idx].width;
            /*
             * The IMM treats the key fields on a byte boundary and not bits.
             * Therefore we can left shift all the way until the MS bit is part
             * of the key. However this might skew the key value if treated as
             * bit key. For example a key value of 0x7 will be shifted to 0xE0.
             * Yet, all the key bits will be captured on a byte boundary
             * processing.
             */
            data <<= (64 - width);
        }
        sal_memcpy(key_buf, &data, bytes_width);
        return;
    }

    /* For multiple key fields */
    bit_offset = 0;
    for (k = 0; k < (uint32_t)idx; k++) {
        bit_offset += dyn_info->key_fields[k].width;
    }
    width = dyn_info->key_fields[idx].width;
    byte_offset = bit_offset / 8;
    bit_offset %= 8;
    buf = key_buf + byte_offset;
    while (width) {
        if (bit_offset) {
            uint8_t mask = (1 << (8 - bit_offset)) - 1;
            *buf |= (data & mask) << bit_offset;
            /*
             * We shift the data so we always can handle the next byte from
             * bit #0 instead of handling fraction of bytes on the data side.
             */
            data >>= (8 - bit_offset);
            if (width > (8 - bit_offset)) {
                width -= (8 - bit_offset);
            } else {
                width = 0;
            }
            bit_offset = 0;
        } else {
            uint32_t len = (width > 8 ? 8 : width);
            *buf |= data & 0xFF;
            width -= len;
            data >>= len;
        }
        buf++;
    }
}

/* Helper function for quick sort based on field ID and index */
static int out_field_sort(const void *a, const void *b)
{
    bcmltd_field_t *fa = *(bcmltd_field_t **)a;
    bcmltd_field_t *fb = *(bcmltd_field_t **)b;

    if (fb->id == fa->id) {
        return (int)(fa->idx - fb->idx);
    } else {
        return (int)(fa->id - fb->id);
    }
}

/*!
 * \brief Extract key value from direct table lookup/traverse operation.
 *
 * This function extract the value of a key field from the combined key value.
 * A key value might be composed of multiple key fields. Therefore this
 * function uses the value of the \c idx parameter to extract the value of
 * the proper key field.
 *
 * \param [in] tbl The table associated with the operation.
 * \param [in] dyn_info Dynamic info about the key fields.
 * \param [in] key_buf The buffer of the key.
 * \param [in] idx Indicating the field index in the dynamic info structure.
 * \param [out] fld The extracted field data value.
 */
static void extract_direct_key(table_info_t *tbl,
                               bcmimm_table_dynamic_info_t *dyn_info,
                               uint8_t *key_buf,
                               int idx,
                               bcmltd_field_t *fld)
{
    uint32_t key = *(uint32_t *)key_buf;
    uint32_t bit_offset;
    uint32_t k;
    uint32_t mask;

    if (tbl->key_fld_cnt == 1) {
        fld->data = key;
    } else {
        bit_offset = 0;
        for (k = 0; k < (uint32_t)idx; k++) {
            bit_offset += dyn_info->key_fields[k].width;
        }
        key >>= bit_offset;
        mask = (1 << dyn_info->key_fields[idx].width) - 1;
        fld->data = key & mask;
    }
}

/*******************************************************************************
 * External functions local to this module
 */
int bcmimm_is_key_field(table_info_t *tbl,
                        bcmimm_table_dynamic_info_t *dyn_info,
                        uint32_t fid)
{
    int idx;

    for (idx = 0; idx < (int)tbl->key_fld_cnt; idx++) {
        if (dyn_info->key_fields[idx].fid == fid) {
            return idx;
        }
    }
    return -1;
}

int bcmimm_invoke_validate_cb(int unit,
                              bcmltd_sid_t sid,
                              bcmimm_entry_event_t action,
                              uint32_t idx,
                              const bcmltd_fields_t *in,
                              table_info_t *tbl)
{
    bcmltd_field_t *out_k_fields; /* Key fields output list */
    bcmltd_field_t *out_d_fields; /* Data fields output list */
    bcmimm_lt_validate_cb *cb_func = bcmimm_tables_cb[unit][idx].cb.validate;
    int rv;
    bcmimm_table_dynamic_info_t *dyn_info;

    if (!cb_func) {
        return SHR_E_NONE;
    }

    dyn_info = &bcmimm_dynamic_info[unit][tbl->fields_idx];
    rv = build_field_lists(in, &out_k_fields, &out_d_fields, tbl, dyn_info);
    if (rv != SHR_E_NONE) {
        return rv;
    }
    rv = cb_func(unit, sid, action, out_k_fields,
                 out_d_fields, bcmimm_tables_cb[unit][idx].context);

    bcmimm_internal_free_lists(out_k_fields, out_d_fields);
    return rv;
}

void bcmimm_compress_direct_key(table_info_t *tbl,
                               const bcmltd_fields_t *in,
                               bcmimm_table_dynamic_info_t *dyn_info,
                               uint32_t *key,
                               uint32_t *key_idx_ar)
{
    size_t j;
    const bcmltd_field_t *fld;
    int idx;
    uint32_t key_cnt = tbl->key_fld_cnt;
    uint32_t key_i = 0;
    uint32_t val;
    uint32_t bit_offset;
    uint32_t k;

    *key = 0;
    for (j = 0; j < in->count; j++) {
        fld = in->field[j];
        if ((idx = bcmimm_is_key_field(tbl, dyn_info, fld->id)) != -1) {
            val = (uint32_t)(fld->data & 0xFFFFFFFF);
            if (tbl->key_fld_cnt == 1) {
                *key = val;
            } else {
                /* For multiple key fields */
                bit_offset = 0;
                for (k = 0; k < (uint32_t)idx; k++) {
                    bit_offset += dyn_info->key_fields[k].width;
                }
                *key |= val << bit_offset;
            }
            if (key_idx_ar) {
                key_idx_ar[key_i++] = (uint32_t)j;
                if (key_i >= key_cnt) {
                    break;
                }
            }
        }
    }
}

int bcmimm_notify_stage(int unit,
                        const bcmltd_fields_t *in,
                        table_info_t *tbl,
                        uint32_t idx,
                        const bcmltd_op_arg_t *op_arg,
                        bcmimm_entry_event_t reason,
                        bcmltd_fields_t *returned_fields)
{
    bcmltd_field_t *out_k_fields; /* Key fields output list */
    bcmltd_field_t *out_d_fields; /* Data fields output list */
    bcmimm_lt_stage_cb *cb_func = bcmimm_tables_cb[unit][idx].cb.stage;
    bcmimm_lt_stage_ex_cb *cb_ex_func = bcmimm_tables_cb[unit][idx].cb.op_stage;
    int rv;
    bcmimm_table_dynamic_info_t *dyn_info;

    if (!cb_func && !cb_ex_func) {
        if (returned_fields) {
            returned_fields->count = 0;
        }
        return SHR_E_NONE;
    }

    dyn_info = &bcmimm_dynamic_info[unit][tbl->fields_idx];
    if (build_field_lists(in, &out_k_fields,
                          &out_d_fields, tbl, dyn_info) != SHR_E_NONE) {
        return SHR_E_MEMORY;
    }

    /*
     * Clear all the output fields, if relevant. Note that the fields being
     * allocated in bcmimm_internal_unit_init() as a bulk. So to save
     * iterations we use the notion that the allocated field block is pointed
     * by field 0 (see also bcmimm_internal_unit_shutdown()).
     */
    if (returned_fields) {
        sal_memset(returned_fields->field[0], 0,
                   sizeof(bcmltd_field_t) * returned_fields->count);
    }

    /* Call the component callback staging function */
    if (cb_ex_func) {
        rv = cb_ex_func(unit,
                         tbl->sid,
                         op_arg,
                         reason,
                         out_k_fields,
                         out_d_fields,
                         bcmimm_tables_cb[unit][idx].context,
                         returned_fields);
    } else {
        rv = cb_func(unit,
                     tbl->sid,
                     op_arg->trans_id,
                     reason,
                     out_k_fields,
                     out_d_fields,
                     bcmimm_tables_cb[unit][idx].context,
                     returned_fields);
    }

    /* Free the temporary fields */
    bcmimm_internal_free_lists(out_k_fields, out_d_fields);
    return rv;
}

int bcmimm_notify_lookup(int unit,
                         const bcmltd_fields_t *in,
                         table_info_t *tbl,
                         uint32_t idx,
                         const bcmltd_op_arg_t *op_arg,
                         bcmimm_lookup_type_t lkup_type,
                         bcmltd_fields_t *out)
{
    bcmimm_lt_lookup_cb *cb_func = bcmimm_tables_cb[unit][idx].cb.lookup;
    bcmimm_lt_lookup_ex_cb *cb_ex_func = bcmimm_tables_cb[unit][idx].cb.op_lookup;

    if (!cb_func && !cb_ex_func) {
        return SHR_E_NONE;
    }
    /* Sort the output fields */
    sal_qsort(out->field, out->count, sizeof(void *), out_field_sort);
    if (cb_ex_func) {
        return cb_ex_func(unit, tbl->sid, op_arg,
                          bcmimm_tables_cb[unit][idx].context,
                          lkup_type, in, out);
    } else {
        return cb_func(unit, tbl->sid, op_arg->trans_id,
                       bcmimm_tables_cb[unit][idx].context,
                       lkup_type, in, out);
    }
}

uint32_t bcmimm_def_vals_fill(int unit,
                              bcmltd_fields_t *out,
                              uint32_t start,
                              table_info_t *tbl)
{
    uint32_t left = out->count - start;
    uint32_t j;
    uint32_t k;
    field_info_t *fld;
    uint32_t fields_added = 0;
    bcmltd_field_t *out_fld;
    uint32_t idx;
    bcmimm_table_dynamic_info_t *dyn_info;

    dyn_info = &bcmimm_dynamic_info[unit][tbl->fields_idx];
    fld = dyn_info->data_fields;
    for (j = 0; (j < tbl->data_fld_cnt) && (left > 0); j++, fld++) {
        for (k = 0; k < start + fields_added; k++) {
            if (out->field[k]->id == fld->fid) {
                break;
            }
        }

        if (k == start + fields_added) {   /* Field was not in output */
            uint32_t size;
            if (fld->array_size) {
                size = fld->array_size;

            } else {
                /*
                 * Wide fields should be treated as array elements but all indexes
                 * should be reported.
                 */
                if (BCMIMM_BITS_TO_BYTES(fld->width) <= sizeof(out_fld->data)) {
                    size = 1;
                } else {
                    size = (BCMIMM_BITS_TO_BYTES(fld->width) +
                            sizeof(out_fld->data) - 1) / sizeof(out_fld->data);
                }
            }
            for (idx = 0; idx < size; idx++) {
                if (left == 0) {
                    break;
                }
                out_fld = out->field[start + fields_added];
                out_fld->data = fld->def_val;
                out_fld->id = fld->fid;
                out_fld->idx = idx;
                out_fld->next = NULL;
                left--;
                fields_added++;
            }
        } else if (fld->array_size > 0) {
        /* Verify that all the array elements are present*/
            uint32_t max_idx = out->field[k]->idx;

            idx = 0;
            do {
                /*
                 * Make room for the new fields. Array fields must follow each
                 * other (contiguous) and at the right order.
                 */
                if (idx < max_idx) {
                    uint32_t idx_to_move = max_idx - idx;
                    int i;

                    if (idx_to_move > left) {
                        break;
                    }
                    /* The last field is at index start + fields_added */
                    for (i = (int)(start + fields_added) - 1;
                         i >= (int)k; i--) {
                        sal_memcpy(out->field[i+idx_to_move],
                                   out->field[i],
                                   sizeof(bcmltd_field_t));
                    }
                }
                for (; idx < max_idx; idx++) {
                    out_fld = out->field[k++];
                    out_fld->data = fld->def_val;
                    out_fld->id = fld->fid;
                    out_fld->idx = idx;
                    out_fld->next = NULL;
                    left--;
                    fields_added++;
                }
                /* Calculate the next chunk (start and end point) boundary */
                for (; idx < fld->array_size; k++) {
                    /* At the end of the fields? */
                    if (k >= start + fields_added) {
                        max_idx = fld->array_size;
                        break;
                    }
                    out_fld = out->field[k];
                    /* If the elements are consecutive just increment the idx */
                    if (out_fld->id == fld->fid && out_fld->idx == idx) {
                        idx++;
                        continue;
                    }
                    /*
                     * We are here if there is a gap. If the ID still match
                     * then feel up the gap to the next array element.
                     * Otherwise fill up the array to the end.
                     */
                    if (out_fld->id == fld->fid) {
                        max_idx = out_fld->idx;
                    } else {
                        max_idx = fld->array_size;
                    }
                    break;
                }
            } while (idx < fld->array_size);
        }
    }
    return fields_added;
}

table_info_t *bcmimm_tbl_find(int unit, uint32_t sid, uint32_t *idx)
{
    table_info_t *tbl;
    uint32_t j;

    if (!bcmimm_tables_list[unit]) {
        return NULL;
    }
    for (j = 0; j < bcmimm_tables_list[unit]->allocated; j++) {
        tbl = &bcmimm_tables_list[unit]->tbl[j];
        if (sid == tbl->sid) {
            if (idx) {
                *idx = j;
            }
            return tbl;
        }
    }
    return NULL;
}

void bcmimm_extract_key_fld(table_info_t *tbl,
                            bcmimm_table_dynamic_info_t *dyn_info,
                            uint8_t *key_buf,
                            int idx,
                            bcmltd_field_t *fld)
{
    uint32_t byte_offset;
    uint32_t bit_offset;
    uint32_t k;
    uint8_t *buf;
    uint32_t width; /* The width (in bits) of the key field */
    uint32_t bytes_width;
    uint8_t mask;
    uint32_t bit_shift = 0;
    uint64_t data;

    fld->idx = 0;
    fld->id = dyn_info->key_fields[idx].fid;
    fld->data = 0;
    bit_offset = 0;

    if (tbl->type == IMM_DIRECT) {
        extract_direct_key(tbl, dyn_info, key_buf, idx, fld);
        return;
    }
    /* For a single field key simply copy the data */
    if (tbl->key_fld_cnt == 1) {
        bytes_width = BCMIMM_BITS_TO_BYTES(dyn_info->key_fields[0].width);
        sal_memcpy(&fld->data, key_buf, bytes_width);
        if (bcmimm_big_endian) {
            fld->data >>= (64 - dyn_info->key_fields[0].width);
        }
        return;
    }

    for (k = 0; k < (uint32_t)idx; k++) {
        bit_offset += dyn_info->key_fields[k].width;
    }
    width = dyn_info->key_fields[idx].width;
    byte_offset = bit_offset / 8;
    bit_offset &= 0x7;     /* Like mod 8 */
    buf = key_buf + byte_offset;

    if (bit_offset) {
        mask = (1 << (8 - bit_offset)) - 1;

        fld->data |= (*buf >> bit_offset) & mask;
        bit_shift += 8 - bit_offset;
        width -= (bit_shift < width ? bit_shift : width);
        buf++;
    }

    while (width) {
        uint32_t len = (width > 8 ? 8 : width);
        data = *buf;
        fld->data |= (data << bit_shift);
        width -= len;
        bit_shift += len;
        buf++;
    }
    fld->data &= ((uint64_t)1 << dyn_info->key_fields[idx].width) - 1;
}

int bcmimm_key_build(table_info_t *tbl,
                     bcmimm_table_dynamic_info_t *dyn_info,
                     const bcmltd_fields_t *in,
                     uint8_t *key_buf)
{
    uint32_t key_fields;
    const bcmltd_field_t *fld;
    int idx;
    size_t j;

    key_fields = tbl->key_fld_cnt;
    if (key_fields > BCMIMM_MAX_KEY_SIZE) {
        return SHR_E_INTERNAL;
    }

    if (tbl->type == IMM_DIRECT) {
        bcmimm_compress_direct_key(tbl, in, dyn_info,
                                   (uint32_t *)key_buf, NULL);
        return SHR_E_NONE;
    }
    for (j = 0; (j < in->count) && (key_fields > 0); j++) {
        fld = in->field[j];
        if ((idx = bcmimm_is_key_field(tbl, dyn_info,
                                       in->field[j]->id)) != -1) {
            bcmimm_insert_key_fld(tbl, dyn_info, key_buf, idx, fld->data);
            key_fields--;
        }
    }
    if (key_fields != 0) {
        return SHR_E_PARAM;
    }
    return SHR_E_NONE;
}

uint32_t bcmimm_compress_fields_into_buffer(
     const bcmltd_fields_t *in,
     const bcmltd_fields_t *extra_in,
     table_info_t *tbl,
     bcmimm_table_dynamic_info_t *dyn_info,
     uint8_t *key_buf,
     uint8_t *f_data,
     bool *f_del,
     uint32_t *fid_buf)
{
    int idx;
    size_t j;
    const bcmltd_field_t *fld;
    uint32_t data_fld_idx = 0;
    uint32_t *fid = fid_buf;
    uint32_t data_f_size = tbl->f_data_size;
    const bcmltd_fields_t *inp = in;
    int k;

    for (k = 0; k < 2; k++, inp = extra_in) {
        if (!inp || inp->count == 0) {
            continue;
        }
        for (j = 0; j < inp->count; j++) {
            fld = inp->field[j];
            if (key_buf &&
                (idx = bcmimm_is_key_field(tbl, dyn_info, fld->id)) != -1) {
                bcmimm_insert_key_fld(tbl, dyn_info, key_buf, idx, fld->data);
            } else {
                /*
                 * Copy the data into the data array. The size of each data
                 * element is data_f_size.
                 */
                uint8_t *fld_const;

                fld_const = f_data + data_f_size * data_fld_idx;

                /*
                 * Copy the index tuple (field ID, idx) where idx is LS portion
                 */
                *fid = 0;
                if (tbl->array_idx_size > 0) {
                    *fid = fld->idx;
                }
                *fid |= fld->id << (tbl->array_idx_size * 8);
                fid++;

                /* Now copy the data portion */
                if (bcmimm_big_endian &&
                    (tbl->f_data_size < sizeof(fld->data))) {
                    uint8_t *data = (uint8_t *)&fld->data;

                    data += sizeof(fld->data) - tbl->f_data_size;
                    sal_memcpy(fld_const, data, tbl->f_data_size);
                } else {
                    sal_memcpy(fld_const, &fld->data, tbl->f_data_size);
                }

                /*
                 * Check if the field has to be unset. Remember that this
                 * operation can only exist for update operation. In this case
                 * there are two optional behavior:
                 * a. For update only table the field value should be set to
                 * its default value. Only the component can insert entries
                 * into update only tables, so we don't want to delete fields
                 * by the user and not allow the user to add them later.
                 * Also remember that an unset field will have the default
                 * value in its data field so there is nothing to do.
                 * b. For all other type of tables simply mark the field for
                 * deletion.
                 */
                if (f_del && (fld->flags & SHR_FMM_FIELD_DEL) &&
                    !dyn_info->update_only) {
                    f_del[data_fld_idx] = true;
                }
                data_fld_idx++;
            }
    }
    }
    return data_fld_idx;
}

int bcmimm_extract_buf_into_flds(bcmimm_be_fields_t *be_out,
                                 table_info_t *tbl,
                                 bcmltd_fields_t *out)
{
    size_t j;
    bcmltd_field_t *out_fld;
    uint8_t *buf_ptr = be_out->fdata;
    uint32_t *fid = be_out->fid;
    uint32_t mask;

    /* Extract all the data fields from the BE output */
    for (j = 0; j < be_out->count; j++) {
        out_fld = out->field[j];

        out_fld->flags = 0;
        /* Extract the index and FID */
        if (tbl->array_idx_size > 0) {
            out_fld->id = (*fid) >> (tbl->array_idx_size * 8);
            mask = (1 << (tbl->array_idx_size * 8)) - 1;
            out_fld->idx = (*fid) & mask;
        } else {
            out_fld->id = *fid;
            out_fld->idx = 0;
        }
        fid++;

        out_fld->data = 0;  /* Make sure the data starts with 0 */
        /* Extract the data portion */
        if (bcmimm_big_endian && (tbl->f_data_size < sizeof(out_fld->data))) {
            uint8_t *data = (uint8_t *)&out_fld->data;

            data += sizeof(out_fld->data) - tbl->f_data_size;
            sal_memcpy(data, buf_ptr, tbl->f_data_size);
        } else {
            sal_memcpy(&out_fld->data, buf_ptr, tbl->f_data_size);
        }
        buf_ptr += tbl->f_data_size;
    }

    return 0;
}


