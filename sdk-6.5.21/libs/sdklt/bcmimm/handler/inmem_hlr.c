/*! \file inmem_hlr.c
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

/*
 * Allow 3 bits for opcode (LS bits), followed by 29 bits for elements count.
 * The upper word is 32 bits for len buffer length
 */
#define BUFF_HDR_OPCODE(x) ((x) & 0x7)
#define BUFF_HDR_D_ELEM(x) (((x) >> 3) & 0x1FFFFFFF)

#define VALIDATE_FIELD_DATA(_f, _d) \
    ((_f)->width <= 64 &&      \
     (_d > (_f)->max_val || _d < (_f)->min_val))

/*******************************************************************************
 * Private functions
 */
/*!
 *\brief Finds a table that matches the sub parameter.
 *
 * This function searches the tables list associated with the unit for a
 * logical table that was created with the sub-component that matches the
 * input \c sub parameter.
 *
 * \param [in] unit Is the unit associated with the logical table.
 * \param [in] sub Is the sub-component that was used to create the logical
 * table HA block.
 * \param [out] idx Is the index in the bcmimm_tables_list of the matched table.
 *
 * \return Pointer to the table with matched sub or NULL when table not found.
 */
static table_info_t *tbl_sub_find(int unit, shr_ha_sub_id sub, uint32_t *idx)
{
    table_info_t *tbl;
    uint32_t j;

    for (j = 0; j < bcmimm_tables_list[unit]->allocated; j++) {
        tbl = &bcmimm_tables_list[unit]->tbl[j];
        if (sub == tbl->sub) {
            if (idx) {
                *idx = j;
            }
            return tbl;
        }
    }
    return NULL;
}

/*!
 *\brief Maps fixed sub-module to fixed block address.
 *
 * This function maps sub (component) value to the address of a fixed HA block
 * that is associated with storing fixed transaction buffers.
 *
 * \param [in] blk_ctrl This is the block control associated with all the
 * fixed HA transaction blocks.
 * \param [in] sub Is the sub-component to match.
 *
 * \return Pointer to the fixed table with associated sub. This function
 * should not fail.
 */
static uint8_t *sub_to_blk(bcmimm_dynamic_blk_t *blk_ctrl, uint8_t sub)
{
    uint8_t *blk = NULL;

    switch (sub) {
    case GENERIC_DATA_COMP_ID:
        blk = blk_ctrl->data_blk.blk;
        break;
    case GENERIC_KEY_COMP_ID:
        blk = blk_ctrl->key_blk.blk;
        break;
    case AUX_TBL_COMP_ID:
        blk = blk_ctrl->aux_blk.blk;
        break;
    default:
        assert(0);
    }
    return blk;
}

/*!
 *\brief Retrieves buffers pointers from their corresponding HA pointers.
 *
 * This function retrieves the data and key buffers based on the HA pointers
 * provided as input parameters.
 *
 * \param [in] unit Is the unit associated with the logical table.
 * \param [in] d_ptr Is the HA data pointer to the data buffer for the entry.
 * \param [in] d_ptr Is the HA key pointer.
 * \param [out] k_buf Is a pointer that will be set to point to the buffer
 * of the key for the entry.
 *
 * \return Pointer to the data buffer when success and NULL otherwise.
 */
static uint8_t *buffers_retrieve(int unit,
                                 uint32_t d_ptr,
                                 uint32_t k_ptr,
                                 uint8_t **k_buf)
{
    uint8_t *d_buf;
    uint8_t sub;
    uint32_t offset;
    uint8_t *blk;

    if (d_ptr == 0) {
        return NULL;
    }
    sub = BCMIMM_PTR_SUB_INFO(d_ptr);
    offset = BCMIMM_PTR_OFFSET_INFO(d_ptr);
    blk = sub_to_blk(&bcmimm_dynamic_buffs[unit], sub);

    d_buf = blk + offset;
    /* Now get the key blk */
    sub = BCMIMM_PTR_SUB_INFO(k_ptr);
    offset = BCMIMM_PTR_OFFSET_INFO(k_ptr);
    blk = sub_to_blk(&bcmimm_dynamic_buffs[unit], sub);
    *k_buf = blk + offset;
    return d_buf;
}

/*!
 *\brief Validate input field value.
 *
 * This function compares the input field values to the min and max values
 * set for each field. Input field can be key or data field.
 *
 * \param [in] in unit The unit associated with the entry.
 * \param [in] in Is the input fields to validate.
 * \param [in] allow_ro Indicates if RO fields are allowed
 * \param [in] tbl Is the LT information structure.
 * \param [out] key_fields The number of key fields.
 *
 * \return SHR_E_NONE if succesful and error code otherwise.
 */
static int validate_values(int unit,
                           const bcmltd_fields_t *in,
                           bool allow_ro,
                           table_info_t *tbl,
                           uint32_t *key_fields)
{
    field_info_t *fld;
    bcmltd_field_t *in_fld;
    size_t j;
    uint32_t fld_idx;
    bool found;
    bcmimm_table_dynamic_info_t *dyn_info;

    *key_fields = 0;
    dyn_info = &bcmimm_dynamic_info[unit][tbl->fields_idx];
    for (j = 0; j < in->count; j++) {
        in_fld = in->field[j];
        found = false;
        /* Search the data fields */
        fld = dyn_info->data_fields;
        for (fld_idx = 0; fld_idx < tbl->data_fld_cnt; fld_idx++) {
            if (fld->fid == in_fld->id) {
                if (fld->read_only &&
                    (!allow_ro || in_fld->flags & SHR_FMM_FIELD_DEL)) {
                    return SHR_E_ACCESS;  /* Can't provide read-only field */
                }
                if (!(in_fld->flags & SHR_FMM_FIELD_DEL) &&
                    VALIDATE_FIELD_DATA(fld, in_fld->data)) {
                    return SHR_E_PARAM;
                }
                found = true;
                break;
            }
            fld++;
        }

        if (found) {
            continue;
        }
        /* Search the key fields */
        fld = dyn_info->key_fields;
        for (fld_idx = 0; fld_idx < tbl->key_fld_cnt; fld_idx++) {
            if (fld->fid == in_fld->id) {
                if (VALIDATE_FIELD_DATA(fld, in_fld->data)) {
                    return SHR_E_PARAM;
                }
                (*key_fields)++;
                break;
            }
            fld++;
        }
    }
    return SHR_E_NONE;
}

/*!
 * \brief Call registered commit callback function.
 *
 * This function notifies a component that is registered to receive commit
 * events for a table.
 *
 * \param [in] unit Is the unit associated with the LT.
 * \param [in] tbl This is the table being changed.
 * \param [in] idx Is the index in table in the table callback list. The
 * index is the index of the table in the master \c table_list[].
 * \param [in] trans_id Is the transaction ID associated with this callback.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int bcmimm_notify_commit(int unit,
                                table_info_t *tbl,
                                uint32_t idx,
                                uint32_t trans_id)
{
    bcmimm_lt_complete_cb *cb_func = bcmimm_tables_cb[unit][idx].cb.commit;

    if (!cb_func) {
        return SHR_E_NONE;
    }

    /* Call the component callback function */
    return cb_func(unit, tbl->sid, trans_id,
                   bcmimm_tables_cb[unit][idx].context);
}

/*!
 * \brief Call registered abort callback function.
 *
 * This function notifies a component that is registered to receive abort
 * events for a table.
 *
 * \param [in] unit Is the unit associated with the LT.
 * \param [in] tbl This is the table being changed.
 * \param [in] idx Is the index in table in the table callback list. The
 * index is the index of the table in the master \c table_list[].
 * \param [in] trans_id Is the transaction ID associated with this callback.
 *
 * \return None.
 */
static void bcmimm_notify_abort(int unit,
                               table_info_t *tbl,
                               uint32_t idx,
                               uint32_t trans_id)
{
    bcmimm_lt_abort_cb *cb_func = bcmimm_tables_cb[unit][idx].cb.abort;

    if (!cb_func) {
        return;
    }

    /* Call the component callback abort function */
    cb_func(unit, tbl->sid, trans_id, bcmimm_tables_cb[unit][idx].context);
}

/*******************************************************************************
 * Public functions
 */
int bcmimm_basic_validate(int unit,
                          bcmlt_opcode_t opcode,
                          const bcmltd_fields_t *in,
                          const bcmltd_generic_arg_t *arg)
{
    table_info_t *tbl;
    int rv;
    uint8_t key_buf[BCMIMM_MAX_KEY_SIZE];
    uint32_t tbl_idx;
    bcmimm_entry_event_t action;
    uint32_t lock_cnt;
    bcmimm_table_dynamic_info_t *dyn_info;
    bool allow_ro = true;
    uint32_t key_fld_cnt;

    SHR_FUNC_ENTER(unit);

    tbl = bcmimm_tbl_find(unit, arg->sid, &tbl_idx);
    SHR_NULL_CHECK(tbl, SHR_E_INTERNAL);

    if (BCMIMM_BITS_TO_BYTES(tbl->key_arry_size) > BCMIMM_MAX_KEY_SIZE) {
        SHR_VERBOSE_EXIT(SHR_E_UNAVAIL);   /* Key size too large */
    }

    if (opcode == BCMLT_OPCODE_UPDATE || opcode == BCMLT_OPCODE_INSERT) {
        allow_ro = false;
    }
    SHR_IF_ERR_EXIT
        (validate_values(unit, in, allow_ro, tbl, &key_fld_cnt));

    if (opcode == BCMLT_OPCODE_TRAVERSE) {
        SHR_EXIT();
    }

    sal_memset(key_buf, 0, sizeof(key_buf));
    /*
     * Build the key, if exist
     */
    dyn_info = &bcmimm_dynamic_info[unit][tbl->fields_idx];
    /* If no key field and operation allowed without a key */
    if (dyn_info->index_alloc &&
        opcode == BCMLT_OPCODE_INSERT && !key_fld_cnt) {
        SHR_IF_ERR_EXIT
            (bcmimm_invoke_validate_cb(unit, arg->sid, BCMIMM_ENTRY_INSERT,
                                       tbl_idx, in, tbl));
        SHR_EXIT();
    }

    SHR_IF_ERR_EXIT(bcmimm_key_build(tbl, dyn_info, in, key_buf));

    switch (opcode) {
    case BCMLT_OPCODE_DELETE:
    case BCMLT_OPCODE_UPDATE:
        SHR_IF_ERR_EXIT(
            bcmimm_be_get_lock_cnt(tbl->blk_hdl, key_buf, &lock_cnt));
        if (lock_cnt > 0) {
            SHR_ERR_EXIT(SHR_E_RESOURCE);
        }
        if (opcode == BCMLT_OPCODE_DELETE) {
            action = BCMIMM_ENTRY_DELETE;
        } else {
            action = BCMIMM_ENTRY_UPDATE;
        }
        break;
    case BCMLT_OPCODE_LOOKUP:
        SHR_IF_ERR_EXIT(
            bcmimm_be_get_lock_cnt(tbl->blk_hdl, key_buf, &lock_cnt));
        action = BCMIMM_ENTRY_LOOKUP;
        break;
    case BCMLT_OPCODE_INSERT:
        /* Check if entry already exist */
        rv = bcmimm_be_get_lock_cnt(tbl->blk_hdl, key_buf, &lock_cnt);
        if (rv == SHR_E_NONE) {
            SHR_ERR_EXIT(SHR_E_EXISTS);
        }
        /* Check that the table is not full */
        if (tbl->entry_limit <= bcmimm_be_table_cnt_get(tbl->blk_hdl)) {
            SHR_ERR_EXIT(SHR_E_FULL);
        }
        action = BCMIMM_ENTRY_INSERT;
        break;
    default:
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_EXIT(
        bcmimm_invoke_validate_cb(unit, arg->sid, action, tbl_idx, in, tbl));
exit:
    SHR_FUNC_EXIT();
}

int bcmimm_basic_insert(int unit,
                        const bcmltd_op_arg_t *op_arg,
                        const bcmltd_fields_t *in,
                        bcmltd_fields_t *out,
                        const bcmltd_generic_arg_t *arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmimm_intern_entry_insert(unit,
                                    op_arg,
                                    arg->sid,
                                    in,
                                    NULL,
                                    BCMIMM_TABLE_NB));

exit:
    SHR_FUNC_EXIT();
}

int bcmimm_basic_insert_alloc(int unit,
                              const bcmltd_op_arg_t *op_arg,
                              const bcmltd_fields_t *in,
                              bcmltd_fields_t *out,
                              const bcmltd_generic_arg_t *arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmimm_intern_entry_insert(unit,
                                    op_arg,
                                    arg->sid,
                                    in,
                                    out,
                                    BCMIMM_TABLE_NB));

exit:
    SHR_FUNC_EXIT();
}

int bcmimm_basic_lookup(int unit,
                        const bcmltd_op_arg_t *op_arg,
                        const bcmltd_fields_t *in,
                        bcmltd_fields_t *out,
                        const bcmltd_generic_arg_t *arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmimm_intern_entry_lookup(unit,
                                    op_arg,
                                    arg->sid,
                                    in,
                                    out,
                                    BCMIMM_TABLE_NB));

exit:
    SHR_FUNC_EXIT();
}

int bcmimm_basic_delete(int unit,
                        const bcmltd_op_arg_t *op_arg,
                        const bcmltd_fields_t *in,
                        bcmltd_fields_t *out,
                        const bcmltd_generic_arg_t *arg)
{
    SHR_FUNC_ENTER(unit);
    SHR_IF_ERR_EXIT
        (bcmimm_intern_entry_delete(unit,
                                    op_arg,
                                    arg->sid,
                                    in,
                                    BCMIMM_TABLE_NB));
exit:
    SHR_FUNC_EXIT();
}

int bcmimm_basic_update(int unit,
                        const bcmltd_op_arg_t *op_arg,
                        const bcmltd_fields_t *in,
                        bcmltd_fields_t *out,
                        const bcmltd_generic_arg_t *arg)
{
    SHR_FUNC_ENTER(unit);
    SHR_IF_ERR_EXIT
        (bcmimm_intern_entry_update(unit,
                                    op_arg,
                                    arg->sid,
                                    in,
                                    BCMIMM_TABLE_NB));

exit:
    SHR_FUNC_EXIT();
}

int bcmimm_basic_abort(int unit,
                       uint32_t trans_id,
                       const bcmltd_generic_arg_t *arg)
{
    imm_trans_info_t *blk_hdr = NULL;
    imm_trans_elem_t *trans_elem;
    uint8_t *d_buf;
    uint8_t *k_buf;
    uint8_t opcode;
    table_info_t *tbl;
    uint64_t *d_hdr;
    generic_array_blk_t *aux_blk;
    bcmimm_dynamic_blk_mgmt_t *dyn_blk;
    uint32_t *fid;
    uint8_t *f_data;
    bcmimm_be_fields_t be_param;
    uint32_t idx;
    uint32_t elem_cnt = 0;
    bool normal_exit = true;

    SHR_FUNC_ENTER(unit);
    if (!bcmimm_dynamic_buffs[unit].trans_blk.blk) {
        return SHR_E_INTERNAL;
    }
    blk_hdr = (imm_trans_info_t *)
        (bcmimm_dynamic_buffs[unit].trans_blk.blk + sizeof(generic_array_blk_t));
    elem_cnt = blk_hdr->elem_cnt;
    if (blk_hdr->trans_id != trans_id) {
        /* Silently discard unknown transaction IDs */
        normal_exit = false;
        SHR_EXIT();
    }

    while (blk_hdr->elem_cnt > blk_hdr->start_idx) {
        trans_elem = &blk_hdr->elem[blk_hdr->elem_cnt-1];
        tbl = tbl_sub_find(unit, trans_elem->sub, &idx);
        SHR_NULL_CHECK(tbl, SHR_E_INTERNAL);

        bcmimm_notify_abort(unit, tbl, idx, trans_id);

        d_buf = buffers_retrieve(unit,
                                 trans_elem->data,
                                 trans_elem->key,
                                 &k_buf);

        /* data buffer might be NULL when HA is disabled */
        if (d_buf) {
            d_hdr = (uint64_t *)d_buf;
            d_buf += BUFF_HEADER_SIZE;
            opcode = BUFF_HDR_OPCODE(*d_hdr);
            be_param.count = BUFF_HDR_D_ELEM(*d_hdr);
            fid = (uint32_t *)d_buf;
            f_data = d_buf + BCMIMM_BE_FID_SIZE * be_param.count;
            switch (opcode) {
            case BCMLT_OPCODE_INSERT:
                /*
                 * Need to delete the entry based on the key we kept. No need to
                 * verify the return value as this can fail.
                 */
                bcmimm_be_entry_delete(tbl->blk_hdl, k_buf);
                break;
            case BCMLT_OPCODE_DELETE:
                /* Restore the deleted entry. */
                be_param.fid = fid;
                be_param.fdata = f_data;
                SHR_IF_ERR_EXIT(
                    bcmimm_be_entry_insert(tbl->blk_hdl, k_buf, &be_param));
                break;
            case BCMLT_OPCODE_UPDATE:
                /*
                 * Restore the previous entry by deleting the last and
                 * inserting the previous one.
                 */
                bcmimm_be_entry_delete(tbl->blk_hdl, k_buf);
                be_param.fid = fid;
                be_param.fdata = f_data;
                SHR_IF_ERR_EXIT(
                    bcmimm_be_entry_insert(tbl->blk_hdl, k_buf, &be_param));

                break;
            default:
                assert(0);
                break;
            }
        }
        tbl->pending_ops--;
        blk_hdr->elem_cnt--;
    }
exit:
    if (normal_exit) {
        /*
         * This loop just verifies that the tables are indeed cleaned from
         * pending activities. If everything works well, the pending_ops field
         * should be zero for every table in the transaction. However, if we
         * crash during the abort we should continue to execute the loop above.
         * For this reason we can't set the field pending_ops to zero
         * above but only to decrement it.
         */
        while (elem_cnt > blk_hdr->start_idx) {
            trans_elem = &blk_hdr->elem[blk_hdr->start_idx];
            tbl = tbl_sub_find(unit, trans_elem->sub, &idx);
            SHR_NULL_CHECK(tbl, SHR_E_INTERNAL);
            tbl->pending_ops = 0;
            blk_hdr->start_idx++;
        }
        blk_hdr->elem_cnt = 0;  /* All done, clear the table */
        blk_hdr->start_idx = 0;

        /* Clear the auxiliary block if valid */
        dyn_blk = &bcmimm_dynamic_buffs[unit].aux_blk;
        aux_blk = (generic_array_blk_t *)dyn_blk->blk;
        if (aux_blk) {
            sal_mutex_take(dyn_blk->sync_obj, SAL_MUTEX_FOREVER);
            aux_blk->free_offset = sizeof(generic_array_blk_t);
            sal_mutex_give(dyn_blk->sync_obj);
        }
    }
    SHR_FUNC_EXIT();
}

int bcmimm_basic_commit(int unit,
                       uint32_t trans_id,
                       const bcmltd_generic_arg_t *arg)
{
    imm_trans_info_t *blk_hdr = NULL;
    imm_trans_elem_t *trans_elem;
    table_info_t *tbl;
    generic_array_blk_t *aux_blk;
    uint32_t tbl_idx;
    bcmimm_dynamic_blk_mgmt_t *dyn_blk;

    SHR_FUNC_ENTER(unit);
    blk_hdr = (imm_trans_info_t *)
       (bcmimm_dynamic_buffs[unit].trans_blk.blk + sizeof(generic_array_blk_t));
    if (blk_hdr->trans_id != trans_id) {
        /* Silently discard unknown transaction IDs */
        SHR_EXIT();
    }
    while (blk_hdr->elem_cnt > blk_hdr->start_idx) {
        trans_elem = &blk_hdr->elem[blk_hdr->start_idx];
        tbl = tbl_sub_find(unit, trans_elem->sub, &tbl_idx);
        SHR_NULL_CHECK(tbl, SHR_E_INTERNAL);

        SHR_IF_ERR_EXIT(
            bcmimm_notify_commit(unit, tbl, tbl_idx, trans_id));
        tbl->pending_ops = 0;
        blk_hdr->start_idx++;
    }

    sal_memset(blk_hdr, 0, sizeof(*blk_hdr)); /* All done, clear the table */

    /* Clear the auxiliary block */
    dyn_blk = &bcmimm_dynamic_buffs[unit].aux_blk;
    aux_blk = (generic_array_blk_t *)dyn_blk->blk;
    if (aux_blk) {
        sal_mutex_take(dyn_blk->sync_obj, SAL_MUTEX_FOREVER);
        aux_blk->free_offset = sizeof(generic_array_blk_t);
        sal_mutex_give(dyn_blk->sync_obj);
    }
exit:
    SHR_FUNC_EXIT();
}

int bcmimm_basic_first(int unit,
                       const bcmltd_op_arg_t *op_arg,
                       const bcmltd_fields_t *in,
                       bcmltd_fields_t *out,
                       const bcmltd_generic_arg_t *arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmimm_intern_entry_get_first(unit,
                                       op_arg,
                                       arg->sid,
                                       out,
                                       BCMIMM_TABLE_NB));

exit:
    SHR_FUNC_EXIT();
}

int bcmimm_basic_next(int unit,
                      const bcmltd_op_arg_t *op_arg,
                      const bcmltd_fields_t *in,
                      bcmltd_fields_t *out,
                      const bcmltd_generic_arg_t *arg)
{

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmimm_intern_entry_get_next(unit,
                                      op_arg,
                                      arg->sid,
                                      in,
                                      out,
                                      BCMIMM_TABLE_NB));

exit:
    SHR_FUNC_EXIT();
}

int bcmimm_basic_table_entry_inuse_get(
        int unit,
        uint32_t trans_id,
        bcmltd_sid_t sid,
        const bcmltd_table_entry_inuse_arg_t *table_arg,
        bcmltd_table_entry_inuse_t *table_data,
        const bcmltd_generic_arg_t *arg)
{
    table_info_t *tbl;

    SHR_FUNC_ENTER(unit);

    tbl = bcmimm_tbl_find(unit, sid, NULL);
    SHR_NULL_CHECK(tbl, SHR_E_INTERNAL);
    if (table_data) {
        table_data->entry_inuse = bcmimm_be_table_cnt_get(tbl->blk_hdl);
    }

exit:
    SHR_FUNC_EXIT();
}

int bcmimm_basic_table_max_entries_set(
        int unit,
        uint32_t trans_id,
        bcmltd_sid_t sid,
        const bcmltd_table_max_entries_arg_t *table_arg,
        const bcmltd_table_max_entries_t *table_data,
        const bcmltd_generic_arg_t *arg)
{
    table_info_t *tbl;
    uint32_t idx;
    bcmimm_lt_max_ent_set_cb *cb_func;
    uint32_t max_entries = (uint32_t)table_data->max_entries;

    SHR_FUNC_ENTER(unit);

    tbl = bcmimm_tbl_find(unit, sid, &idx);
    if (tbl == NULL) {
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             "NULL table_info: sid = %u\n"), sid));
        SHR_EXIT();
    }
    SHR_NULL_CHECK(tbl, SHR_E_INTERNAL);

    if (bcmimm_be_table_cnt_get(tbl->blk_hdl) > max_entries) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }
    /* Call the component if it provided callback for this event */
    cb_func = bcmimm_tables_cb[unit][idx].cb.max_ent_set;
    if (cb_func) {
        SHR_IF_ERR_EXIT(cb_func(unit, trans_id, sid, max_entries));
    }
    /*
     * Trust the caller that max_entries is never larger than the original
     * entry limit
     */
    tbl->entry_limit = max_entries;

exit:
    SHR_FUNC_EXIT();
}

int bcmimm_basic_table_entry_limit_get(
        int unit,
        uint32_t trans_id,
        bcmltd_sid_t sid,
        const bcmltd_table_entry_limit_arg_t *table_arg,
        bcmltd_table_entry_limit_t *table_data,
        const bcmltd_generic_arg_t *arg)
{
    table_info_t *tbl;
    uint32_t idx;
    bcmimm_lt_ent_lmt_get_cb *cb_func;
    uint32_t ent_limit;

    SHR_FUNC_ENTER(unit);

    tbl = bcmimm_tbl_find(unit, sid, &idx);
    SHR_NULL_CHECK(tbl, SHR_E_INTERNAL);

    /* Call the component if it provided callback for this event */
    cb_func = bcmimm_tables_cb[unit][idx].cb.ent_limit_get;
    if (cb_func) {
        SHR_IF_ERR_EXIT(cb_func(unit, trans_id, sid, &ent_limit));
        table_data->entry_limit = ent_limit;
    } else {
        table_data->entry_limit = table_arg->entry_maximum;
    }

exit:
    SHR_FUNC_EXIT();
}

