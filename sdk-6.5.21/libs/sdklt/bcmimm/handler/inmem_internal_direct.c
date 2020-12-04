/*! \file inmem_internal_direct.c
 *
 * In memory internal functions implementation of direct table.
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
#include <shr/shr_fmm.h>
#include <bcmimm/bcmimm_internal.h>

/*******************************************************************************
 * Local definitions
 */
#define BSL_LOG_MODULE BSL_LS_BCMIMM_FRONTEND

/*******************************************************************************
 * Private functions
 */
/*!
 *\brief Compress fields structures of tables with array fields.
 *
 * This function compressed field information for tables that have one or more
 * fields as array. The compression eliminates the flags field from the data
 * structure. However, before eliminating the flags field from the field data
 * structure the function checks for the \c SHR_FMM_FIELD_DEL flag. If exist
 * the function set the index of the field in the f_del array to true.
 *
 * \param [in] unit Is the unit associated with this entry.
 * \param [in] tbl IMM Table information associated with this operation.
 * \param [in] in The input fields.
 * \param [in] in_extra More input fields obtained from the component
 * that is associated with the LT.
 * \param [in] key_idx_ar Key index array indicates the indexes of the fields
 * in in->field that are key fields.
 * \param [out] working_buf The buffer to place the compressed output.
 * \param [out] f_del Array indicating the index of the fields that should be
 * deleted.
 *
 * \return The number of fields that were compressed.
 */
static uint32_t compress_fields_array(int unit,
                                      table_info_t *tbl,
                                      const bcmltd_fields_t *in,
                                      const bcmltd_fields_t *extra_in,
                                      uint32_t *key_idx_ar,
                                      uint8_t *working_buf,
                                      bool *f_del)
{
    const bcmltd_field_t *fld;
    size_t j;
    bcmimm_table_dynamic_info_t *dyn_info;
    bcmimm_be_direct_tbl_field_array_t *out_fld =
        (bcmimm_be_direct_tbl_field_array_t *)working_buf;
    int idx = 0;
    uint32_t data_idx = 0;
    const bcmltd_fields_t *input;
    int k;

    dyn_info = &bcmimm_dynamic_info[unit][tbl->fields_idx];

    for (k = 0; k < 2; k++) {
        if (k == 0) {
            input = in;
        } else {
            input = extra_in;
        }
        if (!input) {
            continue;
        }
        for (j = 0; j < input->count; j++) {
            fld = input->field[j];
            /* Is it a key field? */
            if (tbl->key_fld_cnt > 0 && key_idx_ar &&
                    key_idx_ar[idx] == j && k == 0) {
                idx++;
                continue;
            }
            out_fld->id = fld->id;
            if (f_del) {
                if (!dyn_info->update_only) {
                    f_del[data_idx] = (fld->flags & SHR_FMM_FIELD_DEL) != 0;
                } else {
                    f_del[data_idx] = false;
                }
            }
            out_fld->data = fld->data;
            out_fld->idx = fld->idx;
            out_fld++;
            data_idx++;
        }
    }
    return data_idx;
}

/*!
 *\brief Compress fields structures of tables without array fields.
 *
 * This function compressed field information for basic tables (have no fields
 * that are arrays). The compression eliminates the index and flags field from
 * the data structure. However, before eliminating the flags field from the
 * field data structure the function checks for the \c SHR_FMM_FIELD_DEL flag.
 * If exist the function set the index of the field in the f_del array to true.
 *
 * \param [in] unit Is the unit associated with this entry.
 * \param [in] tbl IMM Table information associated with this operation.
 * \param [in] in The input fields.
 * \param [in] in_extra More input fields obtained from the component
 * that is associated with the LT.
 * \param [in] key_idx_ar Key index array indicates the indexes of the fields
 * in in->field that are key fields.
 * \param [out] working_buf The buffer to place th ecompressed output.
 * \param [out] f_del Array indicating the index of the fields that should be
 * deleted.
 *
 * \return The number of fields that were compressed.
 */
static uint32_t compress_fields_basic(int unit,
                                      table_info_t *tbl,
                                      const bcmltd_fields_t *in,
                                      const bcmltd_fields_t *extra_in,
                                      uint32_t *key_idx_ar,
                                      uint8_t *working_buf,
                                      bool *f_del)
{
    const bcmltd_field_t *fld;
    bcmimm_table_dynamic_info_t *dyn_info;
    size_t j;
    const bcmltd_fields_t *input;
    bcmimm_be_direct_tbl_field_basic_t *out_fld =
        (bcmimm_be_direct_tbl_field_basic_t *)working_buf;
    int idx = 0;
    uint32_t data_idx = 0;
    int k;

    dyn_info = &bcmimm_dynamic_info[unit][tbl->fields_idx];

    for (k = 0; k < 2; k++) {
        if (k == 0) {
            input = in;
        } else {
            input = extra_in;
        }
        if (!input) {
            continue;
        }
        for (j = 0; j < input->count; j++) {
            /* Is it a key field? */
            if (tbl->key_fld_cnt > 0 && key_idx_ar &&
                    key_idx_ar[idx] == j && k == 0) {
                idx++;
                continue;
            }
            fld = input->field[j];
            out_fld->id = fld->id;
            if (f_del) {
                if (!dyn_info->update_only) {
                    f_del[data_idx] = (fld->flags & SHR_FMM_FIELD_DEL) != 0;
                } else {
                    f_del[data_idx] = false;
                }
            }
            out_fld->data = fld->data;
            out_fld++;
            data_idx++;
        }
    }
    return data_idx;
}

/*!
 *\brief Extract compress fields structures of direct tables with array fields.
 *
 * This function extracts compressed field information for direct tables type
 * that has one or more array fields.
 * Its operation is inverse to the function \ref compress_fields_array().
 *
 * \param [in] be_out The compressed buffer as it comes from the back end.
 * \param [in] tbl The relevant table of which the entry belongs to.
 * \param [out] out Structure to place the uncompressed fields information.
 *
 * \return SHR_E_NONE
 */
static int extract_array_buf_info_flds(bcmimm_be_fields_t *be_out,
                                       table_info_t *tbl,
                                       bcmltd_fields_t *out)
{
    bcmimm_be_direct_tbl_field_array_t *fld = be_out->fdata;
    uint32_t j;
    bcmltd_field_t *out_fld;

    for (j = 0; j < be_out->count; j++, fld++) {
        out_fld = out->field[j];
        out_fld->data = fld->data;
        out_fld->id = fld->id;
        out_fld->idx = fld->idx;
        out_fld->flags = 0;
    }
    return SHR_E_NONE;
}

/*!
 *\brief Extract compress fields structures of direct tables with array fields.
 *
 * This function extracts compressed field information for direct tables type
 * that has no array fields.
 * Its operation is inverse to the function \ref compress_fields_basic().
 *
 * \param [in] be_out The compressed buffer as it comes from the back end.
 * \param [in] tbl The relevant table of which the entry belongs to.
 * \param [out] out Structure to place the uncompressed fields information.
 *
 * \return SHR_E_NONE
 */
static int extract_basic_buf_info_flds(bcmimm_be_fields_t *be_out,
                                       table_info_t *tbl,
                                       bcmltd_fields_t *out)
{
    bcmimm_be_direct_tbl_field_basic_t *fld = be_out->fdata;
    uint32_t j;
    bcmltd_field_t *out_fld;

    for (j = 0; j < be_out->count; j++, fld++) {
        out_fld = out->field[j];
        out_fld->data = fld->data;
        out_fld->id = fld->id;
        out_fld->idx = 0;
        out_fld->flags = 0;
    }
    return SHR_E_NONE;
}

/*******************************************************************************
 * External functions local to this module
 */
uint32_t bcmimm_direct_compress_fields(int unit,
                                       table_info_t *tbl,
                                       const bcmltd_fields_t *in,
                                       const bcmltd_fields_t *extra_in,
                                       uint32_t *key_idx_ar,
                                       uint8_t *working_buf,
                                       bool *f_del)
{
    if (tbl->array_idx_size) {
        return compress_fields_array(unit, tbl, in, extra_in, key_idx_ar,
                                     working_buf, f_del);
    } else {
        return compress_fields_basic(unit, tbl, in, extra_in, key_idx_ar,
                                     working_buf, f_del);
    }
}

int bcmimm_direct_extract_buf_into_flds(bcmimm_be_fields_t *be_out,
                                        table_info_t *tbl,
                                        bcmltd_fields_t *out)
{
    if (tbl->array_idx_size > 0) {
        return extract_array_buf_info_flds(be_out, tbl, out);
    } else {
        return extract_basic_buf_info_flds(be_out, tbl, out);
    }
}

int bcmimm_direct_entry_insert(int unit, bcmimm_common_info_t *info)
{
    uint32_t key;  /* since this is direct index key cannot exceed 32 bits */
    bcmimm_table_dynamic_info_t *dyn_info;
    uint32_t key_idx_ar[16];
    table_info_t *tbl = info->tbl;
    bcmimm_be_fields_t be_in;
    uint32_t data_field_cnt;

    SHR_FUNC_ENTER(unit);
    dyn_info = &bcmimm_dynamic_info[unit][tbl->fields_idx];
    /* Check that there are no more then 16 key fields. */
    if (tbl->key_fld_cnt > 16) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }
    if (tbl->key_fld_cnt > 0) {
        sal_memset(key_idx_ar,  0, sizeof(key_idx_ar));
        bcmimm_compress_direct_key(tbl, info->in, dyn_info, &key, key_idx_ar);
    } else {
        key = 0;
    }
    data_field_cnt = bcmimm_direct_compress_fields(unit, tbl, info->in,
                                                   info->extra_in, key_idx_ar,
                                                   info->data_buf, NULL);

    /* Update the transaction information */
    if (info->enable_ha) {
        bcmimm_buf_hdr_update(unit,
                              info->data_buf,
                              BUFF_HEADER_SIZE, /* No data to store */
                              sizeof(key),
                              0,               /* 0 data elements */
                              BCMLT_OPCODE_INSERT,
                              info->aux);

        SHR_IF_ERR_EXIT(bcmimm_update_trans_info(unit, tbl, info->d_ptr,
                                                 info->k_ptr, info->trans_id));
    } else {
        SHR_IF_ERR_EXIT
            (bcmimm_update_trans_info(unit, tbl, 0, 0, info->trans_id));
    }

    /* Build the input to the back end in be_in. */
    be_in.fid = NULL;
    be_in.fdata = info->data_buf;
    be_in.count = data_field_cnt;
    SHR_IF_ERR_EXIT(bcmimm_be_entry_insert(tbl->blk_hdl, &key, &be_in));

exit:
    SHR_FUNC_EXIT();
}

int bcmimm_direct_entry_update(int unit,
                               bcmimm_common_info_t *info,
                               uint8_t *working_buf)
{
    uint32_t key;  /* since this is direct index key cannot exceed 32 bits */
    bcmimm_table_dynamic_info_t *dyn_info;
    uint32_t key_idx_ar[16];
    table_info_t *tbl = info->tbl;
    bcmimm_be_fields_t be_out;
    uint32_t data_f_size;   /* The size of a field entry (data+idx+id) */
    bool *f_del;
    uint32_t data_field_cnt;
    bcmimm_be_fields_upd_t be_in;

    SHR_FUNC_ENTER(unit);
    /* Check that there are no more then 16 key fields. */
    if (tbl->key_fld_cnt > 16) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }
    dyn_info = &bcmimm_dynamic_info[unit][tbl->fields_idx];
    sal_memset(working_buf, 0, tbl->data_bits_len);
    if (tbl->array_idx_size) {
        data_f_size = sizeof(bcmimm_be_direct_tbl_field_array_t);
    } else {
        data_f_size = sizeof(bcmimm_be_direct_tbl_field_basic_t);
    }
    if (tbl->key_fld_cnt > 0) {
        sal_memset(key_idx_ar,  0, sizeof(key_idx_ar));
        bcmimm_compress_direct_key(tbl, info->in, dyn_info, &key, key_idx_ar);
    } else {
        key = 0;
    }
    data_field_cnt = info->in->count + info->extra_in->count  - tbl->key_fld_cnt;
    f_del = (bool *)(working_buf + data_field_cnt * data_f_size);
    /* Reuse the variable data_field_cnt */
    data_field_cnt = bcmimm_direct_compress_fields(unit, tbl, info->in,
                                                   info->extra_in, key_idx_ar,
                                                   working_buf, f_del);


    if (info->enable_ha) {
        be_out.count = tbl->data_bits_len / data_f_size;
        be_out.fdata = info->data_buf;
        SHR_IF_ERR_EXIT
            (bcmimm_be_entry_lookup(tbl->blk_hdl, &key, &be_out));
        /* Now cache the old entry in case it is needed to recover from abort */
        bcmimm_buf_hdr_update(unit,
                              info->data_buf,
                              tbl->data_bits_len + BUFF_HEADER_SIZE,
                              sizeof(key),
                              be_out.count,
                              BCMLT_OPCODE_UPDATE,
                              info->aux);

        /* Update the transaction information */
        SHR_IF_ERR_EXIT(bcmimm_update_trans_info(unit, tbl, info->d_ptr,
                                                 info->k_ptr, info->trans_id));
    } else {
        SHR_IF_ERR_EXIT
            (bcmimm_update_trans_info(unit, tbl, 0, 0, info->trans_id));
    }

    /* Build the input to the back end in be_in. */
    be_in.fid = NULL;
    be_in.del = f_del;
    be_in.fdata = working_buf;
    be_in.count = data_field_cnt;

    SHR_IF_ERR_EXIT
        (bcmimm_be_entry_update(tbl->blk_hdl, &key, &be_in));

exit:
    SHR_FUNC_EXIT();
}

int bcmimm_direct_entry_delete(int unit, bcmimm_common_info_t *info)
{
    uint32_t key;  /* since this is direct index key cannot exceed 32 bits */
    bcmimm_table_dynamic_info_t *dyn_info;
    table_info_t *tbl = info->tbl;
    uint32_t data_f_size;   /* The size of a field entry (data+idx+id) */
    bcmimm_be_fields_t be_out;

    SHR_FUNC_ENTER(unit);
    dyn_info = &bcmimm_dynamic_info[unit][tbl->fields_idx];
    if (tbl->array_idx_size) {
        data_f_size = sizeof(bcmimm_be_direct_tbl_field_array_t);
    } else {
        data_f_size = sizeof(bcmimm_be_direct_tbl_field_basic_t);
    }
    if (tbl->key_fld_cnt > 0) {
        bcmimm_compress_direct_key(tbl, info->in, dyn_info, &key, NULL);
    } else {
        key = 0;
    }

    if (info->enable_ha) {
        be_out.count = tbl->data_bits_len / data_f_size;
        be_out.fdata = info->data_buf;
        SHR_IF_ERR_EXIT
            (bcmimm_be_entry_lookup(tbl->blk_hdl, &key, &be_out));
        /* Now cache the old entry in case it is needed to recover from abort */
        bcmimm_buf_hdr_update(unit,
                              info->data_buf,
                              tbl->data_bits_len + BUFF_HEADER_SIZE,
                              sizeof(key),
                              be_out.count,
                              BCMLT_OPCODE_UPDATE,
                              info->aux);

        /* Update the transaction information */
        SHR_IF_ERR_EXIT(bcmimm_update_trans_info(unit, tbl, info->d_ptr,
                                                 info->k_ptr, info->trans_id));
    } else {
        SHR_IF_ERR_EXIT(bcmimm_update_trans_info(unit, tbl, 0,
                                                 0, info->trans_id));
    }

    /* Delete the entry from the back-end */
    SHR_IF_ERR_EXIT(bcmimm_be_entry_delete(tbl->blk_hdl, &key));
exit:
    SHR_FUNC_EXIT();
}

int bcmimm_direct_entry_lookup(int unit, bcmimm_common_info_t *info)
{
    uint32_t key;  /* since this is direct index key cannot exceed 32 bits */
    bcmimm_table_dynamic_info_t *dyn_info;
    table_info_t *tbl = info->tbl;
    uint32_t data_f_size;   /* The size of a field entry (data+idx+id) */
    bcmimm_be_fields_t *be_out = (bcmimm_be_fields_t *)info->extra_in;

    SHR_FUNC_ENTER(unit);
    dyn_info = &bcmimm_dynamic_info[unit][tbl->fields_idx];
    if (tbl->key_fld_cnt > 0) {
        bcmimm_compress_direct_key(tbl, info->in, dyn_info, &key, NULL);
    } else {
        key = 0;
    }
    if (tbl->array_idx_size) {
        data_f_size = sizeof(bcmimm_be_direct_tbl_field_array_t);
    } else {
        data_f_size = sizeof(bcmimm_be_direct_tbl_field_basic_t);
    }
    be_out->count = tbl->data_bits_len / data_f_size;
    be_out->fid = NULL;
    be_out->fdata = info->data_buf;
    SHR_IF_ERR_EXIT
        (bcmimm_be_entry_lookup(tbl->blk_hdl, &key, be_out));

exit:
    SHR_FUNC_EXIT();
}
