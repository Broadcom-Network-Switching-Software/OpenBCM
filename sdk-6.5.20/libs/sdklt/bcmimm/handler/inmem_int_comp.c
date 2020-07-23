/*! \file inmem_int_comp.c
 *
 *  In-memory inter component API
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <sal/sal_types.h>
#include <sal/sal_mutex.h>
#include <sal/sal_libc.h>
#include <shr/shr_debug.h>
#include <shr/shr_error.h>
#include <shr/shr_ha.h>
#include <shr/shr_fmm.h>
#include <shr/shr_sysm.h>
#include <bcmissu/issu_api.h>
#include <bcmdrd_config.h>
#include <bcmlrd/bcmlrd_client.h>
#include <bcmltm/bcmltm_types.h>
#include <bcmevm/bcmevm_api.h>
#include <bcmltd/bcmltd_lt_types.h>
#include <bcmimm/bcmimm_backend.h>
#include <bcmimm/bcmimm_int_comp.h>
#include <bcmimm/generated/front_e_ha.h>
#include <bcmimm/bcmimm_internal.h>

#define BSL_LOG_MODULE BSL_LS_BCMIMM_FRONTEND

/*!
 *\brief Send table change notification to the notify component.
 *
 * This function prepares the parameters to send notification to upper
 * layers via the notify component.
 *
 * \param [in] unit Is the unit associated with the notification.
 * \param [in] opcode Indicates the operation associated with the notification.
 * \param [in] hi_pri Indicates if the notification should be high priority.
 * \param [in] table_id Is the LT ID.
 * \param [in] in Is the input fields of the modified entry.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int notify_change(int unit,
                         bcmlt_opcode_t opcode,
                         bool hi_pri,
                         uint32_t table_id,
                         const bcmltd_fields_t *in)
{
    size_t j;
    bcmltm_field_list_t *cur_field = NULL;
    bcmltm_field_list_t *gen_field = NULL;
    bcmltm_field_list_t *fields = NULL;
    bcmltd_field_t *in_fld;
    int rv;

    for (j = 0; j < in->count; j++) {
        in_fld = in->field[j];
        gen_field = shr_fmm_alloc();
        if (!gen_field) {
            bcmimm_internal_free_lists(NULL, fields);
            return SHR_E_MEMORY;
        }
        sal_memcpy(gen_field, in_fld, sizeof(*gen_field));
        if (!fields) {
            fields = gen_field;
            cur_field = gen_field;
        } else {
            cur_field->next = gen_field;
            cur_field = gen_field;
        }
    }
    gen_field->next = NULL;  /* Null terminate the list */

    /* Make the notification call */
    rv = bcmevm_table_notify(unit, hi_pri, table_id, opcode, fields);

    /* No need to free the field list. It will be free by the TRM */
    return rv;
}

/*!
 * \brief Validate proper state for SB changes.
 *
 * This function being called when the SB interface is being called to modify
 * an entry. The purpose of this function is to validate that changes like that
 * will only take place during run or stop states when in warm boot. The
 * function will post a warning if the validation fails.
 * The purpose of this function is to force components not to modify IMM values
 * during warm boot.
 *
 * \param [in] unit The unit associated with the operation.
 * \param [in] tbl The IMM table associated with the operation.
 *
 * \return SHR_E_NONE operation is valid.
 * \return SHR_E_ACCESS The operation is not allowed during this state.
 * \return SHR_E_INTERNAL Unexpected error obtaining the system state.
 */
static int check_sysm_state(int unit, table_info_t *tbl)
{
    int rv;
    shr_sysm_states_t sys_state;
    bcmimm_table_dynamic_info_t *dyn_info;

    rv = shr_sysm_instance_state_get(SHR_SYSM_CAT_UNIT, unit, &sys_state);
    if (rv != SHR_E_NONE) {
        return SHR_E_INTERNAL;
    }
    dyn_info = &bcmimm_dynamic_info[unit][tbl->fields_idx];
    if (sys_state == SHR_SYSM_SHUTDOWN) {
        LOG_WARN(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "Update table %s during shutdown state\n"),
                   (dyn_info ? dyn_info->tbl_name : "")));
        return SHR_E_ACCESS;
    }
    if (sys_state != SHR_SYSM_RUN && sys_state != SHR_SYSM_STOP &&
        bcmimm_is_warm() && (!bcmissu_is_active())) {
        LOG_WARN(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "Update table %s during warm boot in state %d\n"),
                   (dyn_info ? dyn_info->tbl_name : ""), sys_state));
        return SHR_E_ACCESS;
    }
    return SHR_E_NONE;
}

/*******************************************************************************
 * Public functions
 */
int bcmimm_entry_lookup(int unit,
                        bcmltd_sid_t sid,
                        const bcmltd_fields_t *in,
                        bcmltd_fields_t *out)
{
    table_info_t *tbl;
    uint8_t *f_data;
    uint8_t *key_buf;   /* Buffer to place the key */
    uint8_t *data_buf = NULL;  /* Buffer for output */
    uint32_t data_f_size;
    bcmimm_be_fields_t be_out;
    uint32_t key_size = 1;
    bcmimm_table_dynamic_info_t *dyn_info;

    SHR_FUNC_ENTER(unit);

    /* Input validation */
    SHR_NULL_CHECK(out, SHR_E_PARAM);
    SHR_NULL_CHECK(in, SHR_E_PARAM);     /* Must have valid 'in' */
    if (out->count == 0 || (!out->field) || (!bcmimm_unit_is_active(unit))) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    /*
     * This function should not be blocked by the NB. Therefore the
     * implementation is done directly here instead of reusing the NB path.
     */
    tbl = bcmimm_tbl_find(unit, sid, NULL);
    if (!tbl) {
        SHR_VERBOSE_EXIT(SHR_E_NOT_FOUND);
    }

    if (tbl->key_fld_cnt > 0) {
        /*
         * For non keyless tables perform sanity check on the input as
         * it should hold the key.
         */
        if (in->count == 0 || !in->field) {
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
        key_size = BCMIMM_BITS_TO_BYTES(tbl->key_arry_size);
    }
    if (tbl->type == IMM_DIRECT) {
        key_size = sizeof(uint32_t);
        if (tbl->array_idx_size > 0) {
            data_f_size = sizeof(bcmimm_be_direct_tbl_field_array_t);
        } else {
            data_f_size = sizeof(bcmimm_be_direct_tbl_field_basic_t);
        }
    } else {
        data_f_size = BCMIMM_BE_FID_SIZE + tbl->f_data_size;
    }
    /* Allocate the data buffer. Should be optimized to avoid malloc */
    data_buf = bcmimm_working_buff_alloc(unit);
    sal_memset(data_buf, 0, tbl->data_bits_len);

    key_buf = data_buf + tbl->data_bits_len;
    sal_memset(key_buf, 0, key_size);
    /* Build the key */
    dyn_info = &bcmimm_dynamic_info[unit][tbl->fields_idx];
    SHR_IF_ERR_EXIT(bcmimm_key_build(tbl, dyn_info, in, key_buf));

    be_out.count = tbl->data_bits_len / data_f_size;
    if (tbl->type == IMM_DIRECT) {
        be_out.fdata = data_buf;
        be_out.fid = NULL;
    } else {
        f_data = data_buf + BCMIMM_BE_FID_SIZE * be_out.count;
        be_out.fid = (uint32_t *)data_buf;
        be_out.fdata = f_data;
    }

    /* Get the data from the back-end */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmimm_be_entry_lookup(tbl->blk_hdl,
                                key_buf,
                                &be_out));
    if (be_out.count > out->count) {
        SHR_ERR_EXIT(SHR_E_RESOURCE);
    }

    /*
     * Extract the data fields.
     */
    if (tbl->type == IMM_DIRECT) {
        bcmimm_direct_extract_buf_into_flds(&be_out, tbl, out);
    } else {
        bcmimm_extract_buf_into_flds(&be_out, tbl, out);
    }
    out->count = be_out.count;

exit:
    if (data_buf) {
        bcmimm_working_buff_free(unit);
    }
    SHR_FUNC_EXIT();
}

int bcmimm_entry_update(int unit,
                        bool hi_pri,
                        bcmltd_sid_t sid,
                        const bcmltd_fields_t *in) {
    table_info_t *tbl;
    uint32_t data_f_size;   /* The size of a field entry (data+idx+id) */
    uint32_t data_fld_idx = 0;
    uint8_t *f_data;
    bool *f_del;
    bcmimm_be_fields_upd_t be_in;
    uint8_t *key_buf;
    uint32_t data_field_cnt;
    uint32_t key_size = 1;    /* Single byte for keyless tables */
    uint8_t *working_buf = NULL;
    bcmimm_table_dynamic_info_t *dyn_info;

    SHR_FUNC_ENTER(unit);

    /* Input validation */
    SHR_NULL_CHECK(in, SHR_E_PARAM);
    if (!bcmimm_unit_is_active(unit)) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    tbl = bcmimm_tbl_find(unit, sid, NULL);
    if (!tbl) {
        SHR_VERBOSE_EXIT(SHR_E_NOT_FOUND);
    }

    SHR_IF_ERR_EXIT(check_sysm_state(unit, tbl));
    if (tbl->key_fld_cnt > 0) {
        if (in->count == 0 || in->field == NULL) {
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
    }

    /*
     * Field ID size if for the touple (field SID, idx) where idx may not
     * exist for tables without array.
     */
    data_f_size = tbl->f_data_size;
    data_field_cnt = in->count - tbl->key_fld_cnt;
    /* Check that the input is not too large */
    if (tbl->data_bits_len / (data_f_size  + BCMIMM_BE_FID_SIZE) <
            data_field_cnt) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    working_buf = bcmimm_working_buff_alloc(unit);

    if (tbl->key_fld_cnt > 0) {
        key_size = BCMIMM_BITS_TO_BYTES(tbl->key_arry_size);
    }
    if (tbl->type == IMM_DIRECT) {
        key_size = sizeof(uint32_t);
        if (tbl->array_idx_size) {
            data_f_size = sizeof(bcmimm_be_direct_tbl_field_array_t);
        } else {
            data_f_size = sizeof(bcmimm_be_direct_tbl_field_basic_t);
        }
    }
    sal_memset(working_buf, 0, tbl->data_bits_len + key_size);
    dyn_info = &bcmimm_dynamic_info[unit][tbl->fields_idx];

    if (tbl->type == IMM_DIRECT) {
        uint32_t key_idx_ar[16];
        be_in.fdata = working_buf;
        f_del = (bool *)(working_buf + data_f_size * data_field_cnt);
        key_buf = ((uint8_t *)f_del) + data_field_cnt * sizeof(bool);
        be_in.del = f_del;
        sal_memset(key_idx_ar,  0, sizeof(key_idx_ar));
        bcmimm_compress_direct_key(tbl, in, dyn_info,
                                   (uint32_t *)key_buf, key_idx_ar);
        data_fld_idx = bcmimm_direct_compress_fields(unit, tbl, in, NULL,
                                                     key_idx_ar, working_buf,
                                                     f_del);
    } else {
        f_data = working_buf + BCMIMM_BE_FID_SIZE * data_field_cnt;
        f_del = (bool *)(f_data + tbl->f_data_size * data_field_cnt);
        key_buf = ((uint8_t *)f_del) + data_field_cnt * sizeof(bool);

        /* Build the input to the back end in be_in. */
        be_in.fid = (uint32_t *)working_buf;
        be_in.del = f_del;
        be_in.fdata = f_data;

        /* Prepare the data buffer */
        data_fld_idx =
            bcmimm_compress_fields_into_buffer(in, NULL, tbl, dyn_info, key_buf,
                                               f_data, f_del, be_in.fid);
    }
    be_in.count = (size_t)data_fld_idx;

    SHR_IF_ERR_EXIT(bcmimm_be_entry_update(tbl->blk_hdl, key_buf, &be_in));

    /*
     * Return the wrking buffer as soon as possible since it is a shared
     * resource and other components might be trying to get it.
     */
    bcmimm_working_buff_free(unit);
    working_buf = NULL;
    SHR_IF_ERR_EXIT(notify_change(unit, BCMLT_OPCODE_UPDATE, hi_pri, sid, in));
exit:
    if (SHR_FUNC_ERR()) {
        if (working_buf) {
            bcmimm_working_buff_free(unit);
        }
    }
    SHR_FUNC_EXIT();
}

int bcmimm_entry_delete(int unit,
                        bcmltd_sid_t sid,
                        const bcmltd_fields_t *in) {
    table_info_t *tbl;
    uint8_t *key_buf = NULL;
    uint32_t key_size = 1;   /* Single byte for keyless tables */
    bcmimm_table_dynamic_info_t *dyn_info;

    SHR_FUNC_ENTER(unit);

    /* Input validation */
    SHR_NULL_CHECK(in, SHR_E_PARAM);
    if (!bcmimm_unit_is_active(unit)) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    tbl = bcmimm_tbl_find(unit, sid, NULL);
    if (!tbl) {
        SHR_VERBOSE_EXIT(SHR_E_NOT_FOUND);
    }

    SHR_IF_ERR_EXIT(check_sysm_state(unit, tbl));
    if (tbl->key_fld_cnt > 0) {
        if (in->count == 0 || in->field == NULL) {
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
        key_size = BCMIMM_BITS_TO_BYTES(tbl->key_arry_size);
    }
    if (tbl->type == IMM_DIRECT) {
        key_size = sizeof(uint32_t);
    }
    key_buf = bcmimm_working_buff_alloc(unit);

    sal_memset(key_buf, 0, key_size);

    /* Build the key */
    dyn_info = &bcmimm_dynamic_info[unit][tbl->fields_idx];
    SHR_IF_ERR_EXIT(bcmimm_key_build(tbl, dyn_info, in, key_buf));

    /* Delete the entry from the back-end */
    SHR_IF_ERR_EXIT(bcmimm_be_entry_delete(tbl->blk_hdl, key_buf));

    /*
     * Return the working buffer as soon as possible since it is a shared
     * resource and other components might be trying to get it.
     */
    bcmimm_working_buff_free(unit);
    key_buf = NULL;
    SHR_IF_ERR_EXIT(notify_change(unit, BCMLT_OPCODE_DELETE, false, sid, in));

exit:
    if (SHR_FUNC_ERR()) {
        if (key_buf) {
            bcmimm_working_buff_free(unit);
        }
    }
    SHR_FUNC_EXIT();
}

int bcmimm_entry_insert(int unit,
                        bcmltd_sid_t sid,
                        const bcmltd_fields_t *in)
{
    table_info_t *tbl;
    uint32_t data_f_size;
    uint8_t *f_data;
    bcmimm_be_fields_t be_in;
    uint32_t data_fld_idx = 0;
    uint32_t key_size = 1;   /* Single byte for keyless tables */
    uint8_t *key_buf;
    uint8_t *working_buf = NULL;
    bcmimm_table_dynamic_info_t *dyn_info;

    SHR_FUNC_ENTER(unit);

    /* Input validation */
    SHR_NULL_CHECK(in, SHR_E_PARAM);
    if (in->count == 0 || in->field == NULL ||
        (!bcmimm_unit_is_active(unit))) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    tbl = bcmimm_tbl_find(unit, sid, NULL);
    if (!tbl) {
        SHR_VERBOSE_EXIT(SHR_E_NOT_FOUND);
    }

    SHR_IF_ERR_EXIT(check_sysm_state(unit, tbl));
    /* Make sure that the additional entry do not exceed the entry_limit */
    if (tbl->entry_limit <= bcmimm_be_table_cnt_get(tbl->blk_hdl)) {
        SHR_ERR_EXIT(SHR_E_FULL);
    }
    /*
     * Field ID size is for the 2-tuple (field SID, idx) where idx may not
     * exist for tables without array. This is another input validation.
     */
    data_f_size = tbl->f_data_size;
    /* Check that the input is not too large */
    if (tbl->data_bits_len / (data_f_size  + BCMIMM_BE_FID_SIZE) <
            in->count - tbl->key_fld_cnt) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    working_buf = bcmimm_working_buff_alloc(unit);

    if (tbl->key_fld_cnt > 0) {
        key_size = BCMIMM_BITS_TO_BYTES(tbl->key_arry_size);
    }

    /* Clean up the working buffer */
    sal_memset(working_buf, 0, tbl->data_bits_len + key_size);
    dyn_info = &bcmimm_dynamic_info[unit][tbl->fields_idx];

    if (tbl->type == IMM_DIRECT) {
        uint32_t key;  /* since this is direct index key cannot exceed 32 bits */
        uint32_t key_idx_ar[16];

        /* Check that there are no more then 16 key fields. */
        if (tbl->key_fld_cnt > 16) {
            SHR_ERR_EXIT(SHR_E_INTERNAL);
        }
        if (tbl->key_fld_cnt > 0) {
            sal_memset(key_idx_ar,  0, sizeof(key_idx_ar));
            bcmimm_compress_direct_key(tbl, in, dyn_info, &key, key_idx_ar);
        } else {
            key = 0;
        }
        data_fld_idx = bcmimm_direct_compress_fields(unit, tbl, in,
                                                     NULL, key_idx_ar,
                                                     working_buf, NULL);
        /* Build the input to the back end in be_in. */
        be_in.fid = NULL;
        be_in.fdata = working_buf;
        be_in.count = data_fld_idx;
        SHR_IF_ERR_EXIT(bcmimm_be_entry_insert(tbl->blk_hdl, &key, &be_in));
    } else {
        key_buf = working_buf + tbl->data_bits_len;

        /* The data portion starts after the field ID tuple */
        f_data = working_buf +
                BCMIMM_BE_FID_SIZE * (in->count - tbl->key_fld_cnt);

        be_in.fid = (uint32_t *)working_buf;
        be_in.fdata = f_data;

        data_fld_idx = bcmimm_compress_fields_into_buffer(in, NULL, tbl,
                                                          dyn_info, key_buf,
                                                          f_data, NULL,
                                                          be_in.fid);

        be_in.count = (size_t)data_fld_idx;
        SHR_IF_ERR_EXIT(
            bcmimm_be_entry_insert(tbl->blk_hdl,
                                   key_buf,
                                   &be_in));
    }

    /*
     * Return the working buffer as soon as possible since it is a shared
     * resource and other components might be trying to get it.
     */
    bcmimm_working_buff_free(unit);
    working_buf = NULL;
    SHR_IF_ERR_EXIT(notify_change(unit, BCMLT_OPCODE_INSERT, false, sid, in));

exit:
    if (SHR_FUNC_ERR()) {
        if (working_buf) {
            bcmimm_working_buff_free(unit);
        }
    }
    SHR_FUNC_EXIT();
}

int bcmimm_entry_lock(int unit,
                      bcmltd_sid_t sid,
                      const bcmltd_fields_t *in) {
    table_info_t *tbl = NULL;
    uint8_t *key_buf;
    bcmimm_dynamic_blk_t *blk_ctrl = &bcmimm_dynamic_buffs[unit];
    uint32_t key_size;

    SHR_FUNC_ENTER(unit);

    /* Input validation */
    SHR_NULL_CHECK(in, SHR_E_PARAM);
    if (!bcmimm_unit_is_active(unit)) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    tbl = bcmimm_tbl_find(unit, sid, NULL);
    if (!tbl) {
        SHR_VERBOSE_EXIT(SHR_E_NOT_FOUND);
    }

    key_buf = blk_ctrl->key_blk.blk + tbl->key_bits;

    if (tbl->key_fld_cnt > 0) {
        /*
         * For non keyless tables perform sanity check on the input as
         * it should hold the key.
         */
        if (in->count == 0 || in->field == NULL) {
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
        key_size = BCMIMM_BITS_TO_BYTES(tbl->key_arry_size);
    } else {
        key_size = 1;
    }
    sal_memset(key_buf, 0, key_size);
    if (tbl->key_fld_cnt > 0) {
        bcmimm_table_dynamic_info_t *dyn_info;
        dyn_info = &bcmimm_dynamic_info[unit][tbl->fields_idx];
        SHR_IF_ERR_EXIT(bcmimm_key_build(tbl, dyn_info, in, key_buf));
    }
    SHR_IF_ERR_EXIT(bcmimm_be_entry_lock(tbl->blk_hdl, key_buf));
exit:
    SHR_FUNC_EXIT();
}

int bcmimm_entry_unlock(int unit,
                        bcmltd_sid_t sid,
                        const bcmltd_fields_t *in) {
    table_info_t *tbl = NULL;
    uint8_t *key_buf;
    bcmimm_dynamic_blk_t *blk_ctrl = &bcmimm_dynamic_buffs[unit];
    uint32_t key_size;

    SHR_FUNC_ENTER(unit);

    /* Input validation */
    SHR_NULL_CHECK(in, SHR_E_PARAM);
    if (in->count == 0 || in->field == NULL ||
        (!bcmimm_unit_is_active(unit))) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    tbl = bcmimm_tbl_find(unit, sid, NULL);
    if (!tbl) {
        SHR_VERBOSE_EXIT(SHR_E_NOT_FOUND);
    }

    key_buf = blk_ctrl->key_blk.blk + tbl->key_bits;

    if (tbl->key_fld_cnt > 0) {
        /*
         * For non keyless tables perform sanity check on the input as
         * it should hold the key.
         */
        if (in->count == 0 || in->field == NULL) {
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
        key_size = BCMIMM_BITS_TO_BYTES(tbl->key_arry_size);
    } else {
        key_size = 1;
    }

    sal_memset(key_buf, 0, key_size);
    if (tbl->key_fld_cnt > 0) {
        bcmimm_table_dynamic_info_t *dyn_info;
        dyn_info = &bcmimm_dynamic_info[unit][tbl->fields_idx];
        SHR_IF_ERR_EXIT(bcmimm_key_build(tbl, dyn_info, in, key_buf));
    }
    SHR_IF_ERR_EXIT(bcmimm_be_entry_unlock(tbl->blk_hdl, key_buf));
exit:
    SHR_FUNC_EXIT();
}

int bcmimm_entry_get_first(int unit,
                           bcmltd_sid_t sid,
                           bcmltd_fields_t *out)
{
    SHR_FUNC_ENTER(unit);

    /* Input validation */
    SHR_NULL_CHECK(out, SHR_E_PARAM);
    if (out->count == 0 || out->field == NULL) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }
    if (!bcmimm_unit_is_active(unit)) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_VERBOSE_EXIT(
        bcmimm_intern_entry_get_first(unit, NULL, sid, out, BCMIMM_TABLE_SB));

exit:
    SHR_FUNC_EXIT();
}

int bcmimm_entry_get_next(int unit,
                          bcmltd_sid_t sid,
                          const bcmltd_fields_t *in,
                          bcmltd_fields_t *out)
{
    SHR_FUNC_ENTER(unit);

    /* Input validation */
    SHR_NULL_CHECK(in, SHR_E_PARAM);
    SHR_NULL_CHECK(out, SHR_E_PARAM);
    if (in->count == 0 || out->count == 0 || (!bcmimm_unit_is_active(unit))) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }
    if (!in->field || !out->field) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_VERBOSE_EXIT(
        bcmimm_intern_entry_get_next(unit, NULL, sid, in,
                                     out, BCMIMM_TABLE_SB));

exit:
    SHR_FUNC_EXIT();
}

int bcmimm_lt_event_reg(int unit,
                        bcmltd_sid_t sid,
                        bcmimm_lt_cb_t *cb,
                        void *context)
{
    table_info_t *tbl;
    uint32_t idx;

    SHR_FUNC_ENTER(unit);

    if (!cb || unit < 0 || unit >= BCMDRD_CONFIG_MAX_UNITS) {
        SHR_IF_ERR_EXIT(SHR_E_PARAM);
    }
    tbl = bcmimm_tbl_find(unit, sid, &idx);
    if (!tbl) {
        SHR_VERBOSE_EXIT(SHR_E_NOT_FOUND);
    }
    bcmimm_tables_cb[unit][idx].cb = *cb;
    bcmimm_tables_cb[unit][idx].context = context;
exit:
    SHR_FUNC_EXIT();
}
