/*! \file inmem_internal.c
 *
 * In memory internal functions implementation.
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
#include <sal/sal_thread.h>
#include <shr/shr_debug.h>
#include <shr/shr_error.h>
#include <shr/shr_ha.h>
#include <shr/shr_fmm.h>
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
#define BUFF_HDR_SET(x,len,d_elem,opcode) \
    (x) = (((uint64_t)(len)) << 32) | ((d_elem) << 3) | (uint8_t)opcode

#define BUFF_HDR_LEN(x) (((x) >> 32) & 0xFFFFFFFF)

#define ROUND_TO_WORD_SIZE(len) (((len) + 3) / 4) * 4

/*
 * The following definitions related to multiple processing paths. There are total
 * of two processing paths:
 * 1. Modeled path
 * 2. Interactive path.
 * Since each path being processed with its own thread there should be resources
 * allocated for each path. It is not a good idea to block one path for the
 * other, in particular blocking the modeled path for the interactive path
 * should be avoided.
 */
#define INTERACTIVE_IDX         1
#define MODELED_IDX             0
#define MAX_PROCESSING_PATH     2

/* This is the maximum number of read-only fields per entry */
#define MAX_READ_ONLY_FIELDS    17

static bcmltd_fields_t
    *read_only_resources[MAX_PROCESSING_PATH][BCMDRD_CONFIG_MAX_UNITS];

static bool ha_enabled;

bool bcmimm_big_endian;

/*******************************************************************************
 * Private functions
 */
/*!
 *\brief Extract entry keys from output BE buffers.
 *
 * This function extracts the key field out from the key buffer.
 *
 * \param [in] key_buf Is the key buffer
 * \param [in] tbl Is a pointer to the logical table information.
 * \param [out] out Is the output field array where the fields will be
 * extracted to.
 * \param [in,out] tot_count Is the current field total count. It is being
 * used as the index into the output field array. It also allows to determine
 * the total number of fields that had been written into the out structure.
 *
 * \return None.
 */
static void extract_key_into_flds(uint8_t *key_buf,
                                  table_info_t *tbl,
                                  bcmimm_table_dynamic_info_t *dyn_info,
                                  bcmltd_fields_t *out,
                                  size_t *tot_count)
{
    uint32_t j;
    int idx;
    bcmltd_field_t *out_fld;

    for (j = 0, idx = 0;
         j < tbl->key_fld_cnt && *tot_count < out->count;
         j++, idx++) {
        out_fld = out->field[(*tot_count)++];
        bcmimm_extract_key_fld(tbl, dyn_info, key_buf, idx, out_fld);
    }
}

/*!
 *\brief Allocates data and key buffers.
 *
 * This function allocates the data and key buffers. The initial buffers
 * already associated with the table. However, for transaction with more than
 * a single entry the buffers will be allocated from the auxiliary block.
 * The buffer allocation doesn't specifies the exact buffer size required for
 * data (key has fixed size buffer derived from the table information). It is
 * therefore assumed that data should be the largest possible buffer
 * (determined during table initialization). The actual size will be set
 * once the function parsed the data.
 *
 * Since the auxiliary block being shared by all tables under the unit, it
 * is require to protect the block from multiple simultaneous access. In
 * particular between the step of buffer allocation and the time where the
 * allocation length is set (see \ref update_aux_blk()).
 * The function also calculates the HA pointers for these buffers.
 *
 * \param [in] unit Is the unit associated with the operation.
 * \param [in] tbl Is the table associated with the operation.
 * \param [out] d_ptr This is HA pointer pointing to the data portion of
 * the entry.
 * \param [out] k_ptr This is HA pointer pointing to the key portion of
 * the entry.
 * \param [out] k_buf This will point to the key buffer.
 * \param [out] aux Indicates if the buffers where allocated from the
 * auxiliary block or not.
 *
 * \return A pointer to the data buffer if success and NULL otherwise.
 */
static uint8_t *buffs_alloc(int unit,
                            table_info_t *tbl,
                            uint32_t *d_ptr,
                            uint32_t *k_ptr,
                            uint8_t **k_buf,
                            bool *aux)
{
    uint8_t *buf;
    bcmimm_dynamic_blk_t *blk_ctrl = &bcmimm_dynamic_buffs[unit];

    if (tbl->pending_ops == 0) {
        buf = blk_ctrl->data_blk.blk + tbl->data_bits;
        BCMIMM_BUF_PTR(*d_ptr, GENERIC_DATA_COMP_ID, tbl->data_bits);
        *k_buf = blk_ctrl->key_blk.blk + tbl->key_bits;
        BCMIMM_BUF_PTR(*k_ptr, GENERIC_KEY_COMP_ID, tbl->key_bits);
        *aux = false;
    } else {    /* Dynamic allocation */
        generic_array_blk_t *aux_blk;
        uint32_t len;
        uint32_t key_size = 1;

        if (tbl->key_fld_cnt) {
            key_size = BCMIMM_BITS_TO_BYTES(tbl->key_arry_size);
        }
        aux_blk = (generic_array_blk_t *)blk_ctrl->aux_blk.blk;
        if (!aux_blk) {
            return NULL;
        }
        len = tbl->data_bits_len + BUFF_HEADER_SIZE;  /* Data buffer */
        len += key_size;   /* Add key buffer */
        len = ROUND_TO_WORD_SIZE(len);
        sal_mutex_take(blk_ctrl->aux_blk.sync_obj, SAL_MUTEX_FOREVER);
        if (aux_blk->blk_size - aux_blk->free_offset < len) {
            sal_mutex_give(blk_ctrl->aux_blk.sync_obj);
            return NULL;
        }
        *k_buf = blk_ctrl->aux_blk.blk + aux_blk->free_offset;
        buf = *k_buf + key_size;
        BCMIMM_BUF_PTR(*k_ptr, AUX_TBL_COMP_ID, aux_blk->free_offset);
        *d_ptr = *k_ptr + key_size;
        *aux = true;
    }
    return buf;
}

/*!
 *\brief Free data and key buffers.
 *
 * This function frees previously allocated buffer.
 *
 * \param [in] unit Is the unit associated with the operation.
 * \param [in] tbl Is the table where the action taken place.
 * \param [in] d_len Is the data length portion of the buffer.
 * \param [in] k_len Is the key length portion of the buffer.
 * \param [in] aux Indicates if the buffer was allocated from the auxiliary
 * blocks or not.
 *
 * \return None.
 */
static void buffs_free(int unit,
                       table_info_t *tbl,
                       uint32_t d_len,
                       uint32_t k_len,
                       bool aux)
{
    bcmimm_dynamic_blk_t *blk_ctrl = &bcmimm_dynamic_buffs[unit];
    generic_array_blk_t *aux_blk;
    uint8_t *buf;
    uint32_t key_size = 1;

    if (tbl->pending_ops > 0) {
        tbl->pending_ops--;
    }

    if (tbl->key_fld_cnt) {
        key_size = BCMIMM_BITS_TO_BYTES(tbl->key_arry_size);
    }
    if (!aux) {
        /* Clean up the buffer */
        buf = blk_ctrl->data_blk.blk + tbl->data_bits;
        sal_memset(buf, 0, tbl->data_bits_len + BUFF_HEADER_SIZE);
        buf = blk_ctrl->key_blk.blk + tbl->key_bits;
        sal_memset(buf, 0, key_size);
        return;
    }
    aux_blk = (generic_array_blk_t *)blk_ctrl->aux_blk.blk;
    aux_blk->free_offset -= ROUND_TO_WORD_SIZE(d_len + k_len);
    buf = ((uint8_t *)aux_blk) + aux_blk->free_offset;
    sal_memset(buf, 0, ROUND_TO_WORD_SIZE(d_len + k_len));
}


/*!
 *\brief Cancel the buffer usage.
 *
 * This function being called after a buffer was allocated (using
 * \ref buffs_alloc()) but the buffer was never set using
 * \ref bcmimm_buf_hdr_update().
 *
 * \param [in] unit Is the unit associated with this entry.
 * \param [in] tbl Is the table where the action taken place.
 * \param [in] aux Indicates if the buffer was allocated from the auxiliary
 * blocks or not.
 *
 * \return None.
 */

static void buffs_cancel(int unit, table_info_t *tbl, bool aux)
{
    bcmimm_dynamic_blk_t *blk_ctrl = &bcmimm_dynamic_buffs[unit];
    generic_array_blk_t *aux_blk;
    uint8_t *buf;
    uint32_t key_size = 1;

    if (tbl->key_fld_cnt) {
        key_size = BCMIMM_BITS_TO_BYTES(tbl->key_arry_size);
    }
    /*
     * Auxiliary blocks are locked until the data length has been determined
     * and the value of free_offset has been set.
     * However, if we cancel then the free offset was never set. So we will
     * continue to use the old value, but we need to unlock the auxiliary block
     * mutex.
     */
    if (aux) {
        aux_blk = (generic_array_blk_t *)blk_ctrl->aux_blk.blk;
        buf = blk_ctrl->aux_blk.blk + aux_blk->free_offset;
        sal_memset(buf, 0, tbl->data_bits_len + key_size + BUFF_HEADER_SIZE);
        sal_mutex_give(blk_ctrl->aux_blk.sync_obj);
    } else {
        buf = blk_ctrl->data_blk.blk + tbl->data_bits;
        sal_memset(buf, 0, tbl->data_bits_len + BUFF_HEADER_SIZE);
        buf = blk_ctrl->key_blk.blk + tbl->key_bits;
        sal_memset(buf, 0, key_size);
    }
}

int bcmimm_table_buffers_init(int unit,
                              table_info_t *t_info)
{
    bcmimm_dynamic_blk_t *blk_ctrl = &bcmimm_dynamic_buffs[unit];
    generic_array_blk_t *gen_blk;
    uint32_t key_buf_len;
    uint32_t d_buf_len;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    d_buf_len = t_info->data_bits_len + BUFF_HEADER_SIZE;
    gen_blk = (generic_array_blk_t *)blk_ctrl->data_blk.blk;
    if (gen_blk->free_offset + d_buf_len > gen_blk->blk_size) {
        SHR_ERR_EXIT(SHR_E_MEMORY);
    }
    /* Allocate the data bits */
    t_info->data_bits = gen_blk->free_offset;
    /* Each data buffer has an header  */
    gen_blk->free_offset += d_buf_len;

    if (t_info->key_fld_cnt > 0) {
        key_buf_len = BCMIMM_BITS_TO_BYTES(t_info->key_arry_size);
    } else {
        key_buf_len = 1;
    }
    gen_blk = (generic_array_blk_t *)blk_ctrl->key_blk.blk;
    if (gen_blk->free_offset + key_buf_len > gen_blk->blk_size) {
        SHR_ERR_EXIT(SHR_E_MEMORY);
    }
    t_info->key_bits = gen_blk->free_offset;
    gen_blk->free_offset += key_buf_len;

exit:
    SHR_FUNC_EXIT();
}


/*!
 *\brief Insert an entry into basic table.
 *
 * This function inserts an entry into basic table type. The entry is first
 * been compressed by calling \ref bcmimm_compress_fields_into_buffer(). The
 * compressed fields being handed to the back end which saves the fields values
 * in a HA memory.
 *
 * \param [in] unit Is the unit associated with this entry.
 * \param [in] info All the required information to perform this operation.
 * \param [in] key_size The size of the key buffer.
 * \param [in] key_buf Key buffer.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
static int basic_insert(int unit,
                        bcmimm_common_info_t *info,
                        uint32_t key_size,
                        uint8_t *key_buf)
{
    table_info_t *tbl = info->tbl;
    bcmimm_table_dynamic_info_t *dyn_info;
    uint8_t *f_data;
    bcmimm_be_fields_t be_in;
    uint32_t data_field_cnt;
    uint32_t data_fld_idx = 0;

    SHR_FUNC_ENTER(unit);

    data_field_cnt = info->in->count + info->extra_in->count - tbl->key_fld_cnt;
    /* The data portion starts after the field ID tuple */
    f_data = info->data_buf + BCMIMM_BE_FID_SIZE * data_field_cnt;

    be_in.fid = (uint32_t *)info->data_buf;
    be_in.fdata = f_data;

    sal_memset(key_buf, 0, key_size);
    dyn_info = &bcmimm_dynamic_info[unit][tbl->fields_idx];
    data_fld_idx = bcmimm_compress_fields_into_buffer(info->in, info->extra_in,
                                                      tbl, dyn_info,
                                                      key_buf, f_data,
                                                      NULL, be_in.fid);


    /* Update the transaction information */
    if (info->enable_ha) {
        bcmimm_buf_hdr_update(unit,
                              info->data_buf,
                              BUFF_HEADER_SIZE, /* No data to store */
                              key_size,
                              0,               /* 0 data elements */
                              BCMLT_OPCODE_INSERT,
                              info->aux);

        SHR_IF_ERR_EXIT(bcmimm_update_trans_info(unit, tbl, info->d_ptr,
                                                 info->k_ptr, info->trans_id));
    } else {
        SHR_IF_ERR_EXIT
            (bcmimm_update_trans_info(unit, tbl, 0, 0, info->trans_id));
    }

    be_in.count = (size_t)data_fld_idx;
    SHR_IF_ERR_EXIT(bcmimm_be_entry_insert(tbl->blk_hdl, key_buf, &be_in));

exit:
    SHR_FUNC_EXIT();
}

/*!
 *\brief Update an entry of a basic table.
 *
 * This function updates an entry of a basic table type. The entry input is
 * first been compressed by calling \ref bcmimm_compress_fields_into_buffer().
 * The compressed fields along with an array of fields that should be deleted
 * are being handed to the back end which updates the entry information in the
 * HA memory.
 * If atomic transaction is supported this function also saves the current
 * content of the entry in HA buffer. If the transaction is aborted the content
 * of this entry will be restored.
 *
 * \param [in] unit Is the unit associated with this entry.
 * \param [in] info All the required information to perform this operation.
 * \param [in] working_buf A buffer to use for field compression.
 * \param [in] key_size The size of the key buffer.
 * \param [in] key_buf Key buffer.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
static int basic_update(int unit,
                        bcmimm_common_info_t *info,
                        uint8_t *working_buf,
                        uint32_t key_size,
                        uint8_t *key_buf)
{
    uint8_t *f_data;
    bool *f_del;
    bcmimm_be_fields_upd_t be_in;
    bcmimm_be_fields_t be_out;
    bcmimm_table_dynamic_info_t *dyn_info;
    table_info_t *tbl = info->tbl;
    uint32_t data_field_cnt;
    uint32_t max_fields;
    uint32_t data_f_size;   /* The size of a field entry (data+idx+id) */
    uint32_t data_fld_idx = 0;

    SHR_FUNC_ENTER(unit);

    /*
     * Field ID size if for the touple (field SID, idx) where idx may not
     * exist for tables without array.
     */
    data_f_size = tbl->f_data_size;
    data_field_cnt = info->in->count + info->extra_in->count - tbl->key_fld_cnt;
    max_fields = tbl->data_bits_len /
        (data_f_size  + BCMIMM_BE_FID_SIZE + sizeof(bool));
    /* Check that the input is not too large */
    if (max_fields < data_field_cnt) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    sal_memset(key_buf, 0, key_size);
    f_data = working_buf + BCMIMM_BE_FID_SIZE * data_field_cnt;
    f_del = (bool *)(f_data + tbl->f_data_size * data_field_cnt);

    /* Build the input to the back end in be_in. */
    be_in.fid = (uint32_t *)working_buf;
    be_in.del = f_del;
    be_in.fdata = f_data;

    /* Prepare the update data buffer */
    dyn_info = &bcmimm_dynamic_info[unit][tbl->fields_idx];
    sal_memset(working_buf, 0, tbl->data_bits_len);
    data_fld_idx = bcmimm_compress_fields_into_buffer(info->in, info->extra_in,
                                                      tbl, dyn_info, key_buf,
                                                      f_data, f_del,
                                                      be_in.fid);

    be_in.count = (size_t)data_fld_idx;

    if (info->enable_ha) {
        /* Keep the current entry values in be_out using HA mem */
        be_out.fid = (uint32_t *)info->data_buf;
        be_out.fdata = info->data_buf + (BCMIMM_BE_FID_SIZE * max_fields);
        be_out.count = max_fields;

        /* Read the content of the current entry. */
        SHR_IF_ERR_EXIT
            (bcmimm_be_entry_lookup(tbl->blk_hdl,
                                    key_buf,
                                    &be_out));

        
        f_data = be_out.fdata;
        be_out.fdata = info->data_buf + be_out.count * BCMIMM_BE_FID_SIZE;
        sal_memmove(be_out.fdata, f_data, be_out.count * tbl->f_data_size);

        /* Now cache the old entry in case it is needed to recover from abort */
        bcmimm_buf_hdr_update(unit,
                              info->data_buf,
                              tbl->data_bits_len + BUFF_HEADER_SIZE,
                              key_size,
                              be_out.count,
                              BCMLT_OPCODE_UPDATE,
                              info->aux);

        /* Update the transaction information */
        SHR_IF_ERR_EXIT
            (bcmimm_update_trans_info(unit, tbl, info->d_ptr,
                                      info->k_ptr, info->trans_id));
    } else {
        SHR_IF_ERR_EXIT
            (bcmimm_update_trans_info(unit, tbl, 0, 0, info->trans_id));

    }

    SHR_IF_ERR_EXIT(bcmimm_be_entry_update(tbl->blk_hdl, key_buf, &be_in));

exit:
    SHR_FUNC_EXIT();
}

/*!
 *\brief Delete an entry from a basic table.
 *
 * This function deletes an entry from a basic table type by calling the
 * back-end function.
 * If atomic transaction is supported this function also saves the current
 * content of the entry in HA buffer. If the transaction is aborted the content
 * of this entry will be restored.
 *
 * \param [in] unit Is the unit associated with this entry.
 * \param [in] info All the required information to perform this operation.
 * \param [in] key_size The size of the key buffer.
 * \param [in] key_buf Key buffer.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
static int basic_delete(int unit,
                        bcmimm_common_info_t *info,
                        uint32_t key_size,
                        uint8_t *key_buf)
{
    uint8_t *f_data;
    bcmimm_be_fields_t be_out;
    bcmimm_table_dynamic_info_t *dyn_info;
    table_info_t *tbl = info->tbl;
    uint32_t data_f_size;

    SHR_FUNC_ENTER(unit);
    sal_memset(key_buf, 0, key_size);

    /* Build the key */
    dyn_info = &bcmimm_dynamic_info[unit][tbl->fields_idx];
    SHR_IF_ERR_EXIT(bcmimm_key_build(tbl, dyn_info, info->in, key_buf));

    if (info->enable_ha) {
        data_f_size = BCMIMM_BE_FID_SIZE + tbl->f_data_size;

        be_out.count = tbl->data_bits_len / data_f_size;
        f_data = info->data_buf + BCMIMM_BE_FID_SIZE * be_out.count;
        be_out.fid = (uint32_t *)info->data_buf;
        be_out.fdata = f_data;

        SHR_IF_ERR_EXIT
            (bcmimm_be_entry_lookup(tbl->blk_hdl, key_buf, &be_out));

        bcmimm_buf_hdr_update(unit,
                              info->data_buf,
                              tbl->data_bits_len + BUFF_HEADER_SIZE,
                              key_size,
                              be_out.count,
                              BCMLT_OPCODE_DELETE,
                              info->aux);

        SHR_IF_ERR_EXIT(bcmimm_update_trans_info(unit, tbl, info->d_ptr,
                                                 info->k_ptr, info->trans_id));
    } else {
        SHR_IF_ERR_EXIT(bcmimm_update_trans_info(unit, tbl, 0,
                                                 0, info->trans_id));
    }

    /* Delete the entry from the back-end */
    SHR_IF_ERR_EXIT(bcmimm_be_entry_delete(tbl->blk_hdl, key_buf));

exit:
    SHR_FUNC_EXIT();
}

/*!
 *\brief Lookup an entry of a basic table.
 *
 * This function look up an entry of a basic table type by calling the back-end
 * lookup function.
 *
 * \param [in] unit Is the unit associated with this entry.
 * \param [in] info All the required information to perform this operation. In
 * particular this structure contains a pointer to the out buffer where the
 * entry content will be written into.
 * \param [in] key_size The size of the key buffer.
 * \param [in] key_buf Key buffer.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
static int basic_lookup(int unit,
                        bcmimm_common_info_t *info,
                        uint32_t key_size,
                        uint8_t *key_buf)
{
    table_info_t *tbl = info->tbl;
    bcmimm_table_dynamic_info_t *dyn_info;
    uint32_t data_f_size;
    uint8_t *f_data;
    bcmimm_be_fields_t *be_out = (bcmimm_be_fields_t *)info->extra_in;

    SHR_FUNC_ENTER(unit);
    /* Build the key */
    dyn_info = &bcmimm_dynamic_info[unit][tbl->fields_idx];
    SHR_IF_ERR_EXIT(bcmimm_key_build(tbl, dyn_info, info->in, key_buf));

    data_f_size = BCMIMM_BE_FID_SIZE + tbl->f_data_size;

    be_out->count = tbl->data_bits_len / data_f_size;
    f_data = info->data_buf + BCMIMM_BE_FID_SIZE * be_out->count;
    be_out->fid = (uint32_t *)info->data_buf;
    be_out->fdata = f_data;

    /* Get the data from the back-end */
    SHR_IF_ERR_EXIT
        (bcmimm_be_entry_lookup(tbl->blk_hdl,
                                key_buf,
                                be_out));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Determine if a key is present within a list of fields.
 *
 * This function searches throughout the lists of fields for all the key fields.
 * It returns true if all key fields were found.
 * There are two lists of fields as input. One (\c fields) generated by the
 * component callback and the other (\c in) is the application input. This
 * function searches both lists for the key fields.
 *
 * \param [in] unit The associated unit.
 * \param [in] tbl The LT table structure.
 * \param [in] fields The list of input fields generated by the component.
 * \param [in] in The list of input fields generated by the application.
 *
 * \return true if all the key fields had been founded. False otherwise.
 */
static bool key_present(int unit,
                        table_info_t *tbl,
                        const bcmltd_fields_t *fields,
                        const bcmltd_fields_t *in)
{
    size_t idx;
    const bcmltd_field_t *fld;
    int key_field_cnt = tbl->key_fld_cnt;
    bcmimm_table_dynamic_info_t *dyn_info;
    const bcmltd_fields_t *flds;
    int j;

    dyn_info = &bcmimm_dynamic_info[unit][tbl->fields_idx];

    for (j = 0, flds = fields; j < 2; j++, flds = in) {
        for (idx = 0; idx < flds->count; idx++) {
            fld = flds->field[idx];
            if (bcmimm_is_key_field(tbl, dyn_info, fld->id) != -1) {
                key_field_cnt--;
                if (key_field_cnt == 0) {
                    return true;
                }
            }
        }
    }
    return false;
}

/*******************************************************************************
 * External functions local to this module
 */
void bcmimm_ha_enable_set(bool enable)
{
    ha_enabled = enable;
}

void bcmimm_buf_hdr_update(int unit,
                           uint8_t *data_buf,
                           uint32_t d_len,
                           uint32_t k_len,
                           uint32_t d_elem,
                           uint8_t opcode,
                           bool aux)
{
    uint64_t *buf_hdr;
    bcmimm_dynamic_blk_t *blk_ctrl = &bcmimm_dynamic_buffs[unit];
    generic_array_blk_t *aux_blk;

    buf_hdr = (uint64_t *)(data_buf - BUFF_HEADER_SIZE);
    if (aux) {
        aux_blk = (generic_array_blk_t *)blk_ctrl->aux_blk.blk;
        aux_blk->free_offset += ROUND_TO_WORD_SIZE(d_len + k_len);
        sal_mutex_give(blk_ctrl->aux_blk.sync_obj);
        BUFF_HDR_SET(*buf_hdr, d_len, d_elem, opcode);
    } else {
        BUFF_HDR_SET(*buf_hdr, d_len, d_elem, opcode);
    }
}

int bcmimm_update_trans_info(int unit,
                             table_info_t *tbl,
                             uint32_t d_ptr,
                             uint32_t k_ptr,
                             uint32_t trans_id)
{
    imm_trans_info_t *blk_hdr = (imm_trans_info_t *)
        (bcmimm_dynamic_buffs[unit].trans_blk.blk + sizeof(generic_array_blk_t));
    imm_trans_elem_t *trans_elem;

    /* Don't record operations of interactive entries */
    if (tbl->interactive) {
        return SHR_E_NONE;
    }

    if (blk_hdr->elem_cnt != 0) {
        if (blk_hdr->trans_id != trans_id) {
            return SHR_E_INTERNAL;
        }
        if (blk_hdr->elem_cnt >= BCMIMM_MAX_TRANS_SIZE) {
            return SHR_E_FULL;
        }
    } else {
        blk_hdr->trans_id = trans_id;
        blk_hdr->start_idx = 0;
        blk_hdr->unit = unit;
    }
    trans_elem = &blk_hdr->elem[blk_hdr->elem_cnt++];
    trans_elem->data = d_ptr;
    trans_elem->key = k_ptr;
    trans_elem->sub = tbl->sub;
    tbl->pending_ops++;
    return SHR_E_NONE;
}

int bcmimm_internal_init(void)
{
    uint32_t tmp = 1;
    uint8_t *tmp_str = (uint8_t *)&tmp;

    bcmimm_big_endian = (*tmp_str == 0);

    bcmimm_be_generic_init();

    return SHR_E_NONE;
}

void bcmimm_internal_shutdown(void)
{
    bcmimm_be_generic_shutdown();
}

int bcmimm_internal_unit_init(int unit)
{
    int j, k;
    bcmltd_field_t *flds;

    SHR_FUNC_ENTER(unit);

    for (j = 0; j < MAX_PROCESSING_PATH; j++) {
        SHR_ALLOC(read_only_resources[j][unit],
                  sizeof(bcmltd_fields_t), "bcmimmRo");
        SHR_NULL_CHECK(read_only_resources[j][unit], SHR_E_MEMORY);

        read_only_resources[j][unit]->field = NULL;
        SHR_ALLOC(read_only_resources[j][unit]->field,
                  sizeof(bcmltd_field_t *) * MAX_READ_ONLY_FIELDS, "bcmimmRo");
        SHR_NULL_CHECK(read_only_resources[j][unit]->field, SHR_E_MEMORY);

        flds = NULL;
        SHR_ALLOC(flds,  sizeof(*flds) * MAX_READ_ONLY_FIELDS, "bcmimmRo");
        SHR_NULL_CHECK(flds, SHR_E_MEMORY);
        read_only_resources[j][unit]->count = MAX_READ_ONLY_FIELDS;
        for (k = 0; k < MAX_READ_ONLY_FIELDS; k++) {
            read_only_resources[j][unit]->field[k] = flds++;
        }
    }
    SHR_IF_ERR_EXIT(bcmimm_be_unit_init(unit));

exit:
    SHR_FUNC_EXIT();
}

void bcmimm_internal_unit_shutdown(int unit)
{
    int j;

    for (j = 0; j < MAX_PROCESSING_PATH; j++) {
        SHR_FREE(read_only_resources[j][unit]->field[0]);

        SHR_FREE(read_only_resources[j][unit]->field);

        SHR_FREE(read_only_resources[j][unit]);
    }
    bcmimm_be_unit_shutdown(unit);
}

void bcmimm_internal_free_lists(bcmltd_field_t *k_fields,
                                bcmltd_field_t *d_fields)
{
    bcmltd_field_t *gen_field;  /* Generic field (data or key) */

    /* Free the key fields */
    while (k_fields) {
        gen_field = k_fields;
        k_fields = gen_field->next;
        shr_fmm_free(gen_field);
    }

    /* Free the data fields */
    while (d_fields) {
        gen_field = d_fields;
        d_fields = gen_field->next;
        shr_fmm_free(gen_field);
    }
}

int bcmimm_intern_entry_delete(int unit,
                               const bcmltd_op_arg_t *op_arg,
                               bcmltd_sid_t sid,
                               const bcmltd_fields_t *in,
                               bcmimm_tbl_owner_t owner)
{
    table_info_t *tbl;
    uint8_t *key_buf = NULL;
    uint8_t *data_buf = NULL;
    bool aux = false;
    uint32_t d_ptr = 0;
    uint32_t k_ptr = 0;
    uint32_t tbl_idx;
    uint32_t key_size = 1;   /* Single byte for keyless tables */
    bool enable_ha = false;
    bool staged = false;
    bcmimm_common_info_t common_info;
    uint32_t trans_id = op_arg->trans_id;

    SHR_FUNC_ENTER(unit);
    tbl = bcmimm_tbl_find(unit, sid, &tbl_idx);
    if (!tbl) {
        SHR_VERBOSE_EXIT(SHR_E_NOT_FOUND);
    }

    if (tbl->key_fld_cnt > 0) {
        if (in->count == 0 || in->field == NULL) {
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
        key_size = BCMIMM_BITS_TO_BYTES(tbl->key_arry_size);
    }

    enable_ha = ha_enabled && !tbl->interactive;
    staged = true;
    SHR_IF_ERR_EXIT
        (bcmimm_notify_stage(unit, in, tbl, tbl_idx,
                             op_arg, BCMIMM_ENTRY_DELETE, NULL));

    if (tbl->interactive) {
        SHR_ALLOC(key_buf, key_size, "bcmimmInter");
    } else {
        data_buf = buffs_alloc(unit, tbl, &d_ptr, &k_ptr, &key_buf, &aux);
        SHR_NULL_CHECK(data_buf, SHR_E_MEMORY);
        data_buf += BUFF_HEADER_SIZE;
    }

    common_info.tbl = tbl;
    common_info.trans_id = trans_id;
    common_info.in = in;
    common_info.extra_in = NULL;
    common_info.data_buf = data_buf;
    common_info.enable_ha = enable_ha;
    common_info.aux = aux;
    common_info.d_ptr = d_ptr;
    common_info.k_ptr = k_ptr;

    if (tbl->type == IMM_BASIC) {
        SHR_IF_ERR_EXIT
            (basic_delete(unit, &common_info,
                          key_size, key_buf));
    } else {
        SHR_IF_ERR_EXIT
            (bcmimm_direct_entry_delete(unit, &common_info));
    }
    staged = false;

    if (tbl->interactive) {
        SHR_FREE(key_buf);
    } else if (!enable_ha) {
        buffs_cancel(unit, tbl, aux);
    }
exit:
    /* In case of an error free ownership if owner is south bound */
    if (SHR_FUNC_ERR()) {
        if (tbl) {
            if (staged) {
                bcmimm_update_trans_info(unit, tbl, 0, 0, trans_id);
            }
            /* Clean up the buffer*/
            if (data_buf) {
                if (enable_ha) {
                    buffs_free(unit,
                               tbl,
                               tbl->data_bits_len + BUFF_HEADER_SIZE,
                               key_size,
                               aux);
                } else {
                    buffs_cancel(unit, tbl, aux);
                }
            } else {
                SHR_FREE(key_buf);
            }
        }
    }
    SHR_FUNC_EXIT();
}

int bcmimm_intern_entry_lookup(int unit,
                               const bcmltd_op_arg_t *op_arg,
                               bcmltd_sid_t sid,
                               const bcmltd_fields_t *in,
                               bcmltd_fields_t *out,
                               bcmimm_tbl_owner_t owner)
{
    table_info_t *tbl;
    bcmimm_be_fields_t be_out;
    uint8_t *data_buf = NULL;
    uint8_t *key_buf;
    bool aux = false;
    uint32_t d_ptr;
    uint32_t k_ptr;
    uint32_t key_size;
    uint32_t tbl_idx;
    bcmimm_common_info_t common_info;
    uint32_t trans_id = op_arg->trans_id;

    SHR_FUNC_ENTER(unit);

    tbl = bcmimm_tbl_find(unit, sid, &tbl_idx);
    if (!tbl) {
        SHR_VERBOSE_EXIT(SHR_E_NOT_FOUND);
    }

    if (tbl->key_fld_cnt > 0) {
        key_size = BCMIMM_BITS_TO_BYTES(tbl->key_arry_size);
    } else {
        key_size = 1;  /* Single byte for keyless tables */
    }

    if (tbl->interactive) {
        SHR_ALLOC(data_buf, tbl->data_bits_len + key_size, "bcmimmInter");
        SHR_NULL_CHECK(data_buf, SHR_E_MEMORY);
        key_buf = data_buf + tbl->data_bits_len;
    } else {
        data_buf = buffs_alloc(unit, tbl, &d_ptr, &k_ptr, &key_buf, &aux);
        SHR_NULL_CHECK(data_buf, SHR_E_MEMORY);
        data_buf += BUFF_HEADER_SIZE;
    }

    sal_memset(key_buf, 0, key_size);
    sal_memset(data_buf, 0, tbl->data_bits_len);
    common_info.tbl = tbl;
    common_info.trans_id = trans_id;
    common_info.in = in;
    common_info.extra_in = (bcmltd_fields_t *)&be_out;
    common_info.data_buf = data_buf;
    if (tbl->type == IMM_BASIC) {
        SHR_IF_ERR_EXIT(basic_lookup(unit, &common_info, key_size, key_buf));
        if (be_out.count > out->count) {
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
        bcmimm_extract_buf_into_flds(&be_out, tbl, out);
    } else {
        SHR_IF_ERR_EXIT(bcmimm_direct_entry_lookup(unit, &common_info));
        if (be_out.count > out->count) {
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
        bcmimm_direct_extract_buf_into_flds(&be_out, tbl, out);
    }
    /*
     * Fill up the entry with fields that were not present in the back end (were
     * never been set). Use default values for those fields.
     */
    if (be_out.count < tbl->max_num_field) {
        out->count = be_out.count +
            bcmimm_def_vals_fill(unit, out, be_out.count, tbl);
    } else {
        out->count = be_out.count;
    }
    SHR_IF_ERR_EXIT
        (bcmimm_notify_lookup(unit, in, tbl, tbl_idx,
                              op_arg, BCMIMM_LOOKUP, out));

exit:
    if (data_buf) {
        if (tbl->interactive) {
            SHR_FREE(data_buf);
        } else {
            buffs_cancel(unit, tbl, aux);
        }
    }

    SHR_FUNC_EXIT();
}

int bcmimm_intern_entry_update(int unit,
                               const bcmltd_op_arg_t *op_arg,
                               bcmltd_sid_t sid,
                               const bcmltd_fields_t *in,
                               bcmimm_tbl_owner_t owner)
{
    table_info_t *tbl;
    uint8_t *data_buf = NULL;
    uint8_t *key_buf;
    bool aux = false;
    uint32_t d_ptr = 0;
    uint32_t k_ptr = 0;
    uint32_t tbl_idx;
    uint32_t key_size = 1;    /* Single byte for keyless tables */
    uint8_t *working_buf = NULL;
    bool enable_ha = false;
    bool staged = false;
    bcmltd_fields_t *returned_fields;
    bcmimm_common_info_t common_info;
    uint32_t trans_id = op_arg->trans_id;

    SHR_FUNC_ENTER(unit);
    tbl = bcmimm_tbl_find(unit, sid, &tbl_idx);
    if (!tbl) {
        SHR_VERBOSE_EXIT(SHR_E_NOT_FOUND);
    }

    if (tbl->key_fld_cnt > 0) {
        if (in->count == 0 || in->field == NULL) {
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
    }

    enable_ha = ha_enabled && !tbl->interactive;

    if (tbl->interactive) {
        returned_fields = read_only_resources[INTERACTIVE_IDX][unit];
    } else {
        returned_fields = read_only_resources[MODELED_IDX][unit];
    }
    returned_fields->count = MAX_READ_ONLY_FIELDS;

    staged = true;
    /*
     * Notify the change before allocating the buffer. This allows the
     * notified component to read some values before we are taking the
     * working buffer.
     */
    SHR_IF_ERR_EXIT
        (bcmimm_notify_stage(unit, in, tbl, tbl_idx,
                             op_arg, BCMIMM_ENTRY_UPDATE, returned_fields));

    if (tbl->key_fld_cnt > 0) {
        key_size = BCMIMM_BITS_TO_BYTES(tbl->key_arry_size);
    }
    if (tbl->interactive) {
        SHR_ALLOC(working_buf, tbl->data_bits_len + key_size, "bcmimmInter");
        key_buf = working_buf + tbl->data_bits_len;
    } else {
        working_buf = bcmimm_working_buff_alloc(unit);
        data_buf = buffs_alloc(unit, tbl, &d_ptr, &k_ptr, &key_buf, &aux);
        SHR_NULL_CHECK(data_buf, SHR_E_MEMORY);
        data_buf += BUFF_HEADER_SIZE;

        sal_memset(data_buf, 0, tbl->data_bits_len);
    }
    SHR_NULL_CHECK(working_buf, SHR_E_MEMORY);

    common_info.tbl = tbl;
    common_info.trans_id = trans_id;
    common_info.in = in;
    common_info.extra_in = returned_fields;
    common_info.data_buf = data_buf;
    common_info.enable_ha = enable_ha;
    common_info.aux = aux;
    common_info.d_ptr = d_ptr;
    common_info.k_ptr = k_ptr;
    if (tbl->type == IMM_BASIC) {
        SHR_IF_ERR_EXIT
            (basic_update(unit, &common_info,
                          working_buf, key_size, key_buf));
    } else {
        SHR_IF_ERR_EXIT
            (bcmimm_direct_entry_update(unit, &common_info, working_buf));
    }
    staged = false;

    /* Return the working buffer */
    if (tbl->interactive) {
        SHR_FREE(working_buf);
    } else {
        bcmimm_working_buff_free(unit);
        if (!ha_enabled) {
            buffs_cancel(unit, tbl, aux);
        }
    }

exit:
    /* In case of an error free ownership if owner is south bound */
    if (SHR_FUNC_ERR()) {
        if (tbl) {
            if (staged) {
                bcmimm_update_trans_info(unit, tbl, 0, 0, trans_id);
            }
            /* Clean up the buffer*/
            if (data_buf) {
                if (enable_ha) {
                    buffs_free(unit,
                               tbl,
                               tbl->data_bits_len + BUFF_HEADER_SIZE,
                               key_size,
                               aux);
                } else {
                    buffs_cancel(unit, tbl, aux);
                }
            }

            if (working_buf) {
                if (tbl->interactive) {
                    SHR_FREE(working_buf);
                } else {
                    bcmimm_working_buff_free(unit);
                }
            }
        }
    }
    SHR_FUNC_EXIT();
}

int bcmimm_intern_entry_insert(int unit,
                               const bcmltd_op_arg_t *op_arg,
                               bcmltd_sid_t sid,
                               const bcmltd_fields_t *in,
                               bcmltd_fields_t *out,
                               bcmimm_tbl_owner_t owner)
{
    table_info_t *tbl;
    uint32_t data_f_size;
    uint8_t *data_buf = NULL;
    uint8_t *key_buf;
    bool aux = false;
    /* Extended HA pointer contains also the data block type */
    uint32_t d_ptr = 0;
    uint32_t k_ptr = 0;
    uint32_t tbl_idx;
    uint32_t key_size = 1;   /* Single byte for keyless tables */
    bool enable_ha = false;
    bcmltd_fields_t *returned_fields;
    bcmimm_common_info_t common_info;
    uint32_t trans_id = op_arg->trans_id;
    uint32_t data_field_cnt;
    bcmimm_table_dynamic_info_t *dyn_info;
    bool staged = false;

    SHR_FUNC_ENTER(unit);
    tbl = bcmimm_tbl_find(unit, sid, &tbl_idx);
    if (!tbl) {
        SHR_VERBOSE_EXIT(SHR_E_NOT_FOUND);
    }

    /*
     * Field ID size is for the 2-tuple (field SID, idx) where idx may not
     * exist for tables without array. This is another input validation.
     */
    data_f_size = tbl->f_data_size;
    dyn_info = &bcmimm_dynamic_info[unit][tbl->fields_idx];
    /* Find the number of data fields within the input */
    if (dyn_info->index_alloc) {
        size_t j;
        data_field_cnt = in->count;
        for (j = 0; j < in->count; j++) {
            if (bcmimm_is_key_field(tbl, dyn_info, in->field[j]->id) != -1) {
                data_field_cnt -= 1; /* Max single key field */
                break;
            }
        }
    } else {
        data_field_cnt = in->count - tbl->key_fld_cnt;
    }
    /* Check that the input is not too large */
    if (tbl->data_bits_len / (data_f_size  + BCMIMM_BE_FID_SIZE) <
            data_field_cnt) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    enable_ha = ha_enabled && !tbl->interactive;
    if (tbl->interactive) {
        returned_fields = read_only_resources[INTERACTIVE_IDX][unit];
    } else {
        returned_fields = read_only_resources[MODELED_IDX][unit];
    }
    returned_fields->count = MAX_READ_ONLY_FIELDS;

    if (out) {
        returned_fields = out;
    }
    staged = true;
    SHR_IF_ERR_EXIT
        (bcmimm_notify_stage(unit, in, tbl, tbl_idx,
                             op_arg, BCMIMM_ENTRY_INSERT, returned_fields));
    if (out && !key_present(unit, tbl, returned_fields, in)) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    if (tbl->key_fld_cnt > 0) {
        key_size = BCMIMM_BITS_TO_BYTES(tbl->key_arry_size);
    }

    if (tbl->interactive) {
        SHR_ALLOC(data_buf, tbl->data_bits_len + key_size, "bcmimmInter");
        SHR_NULL_CHECK(data_buf, SHR_E_MEMORY);
        key_buf = data_buf + tbl->data_bits_len;
    } else {
        data_buf = buffs_alloc(unit, tbl, &d_ptr, &k_ptr, &key_buf, &aux);
        SHR_NULL_CHECK(data_buf, SHR_E_MEMORY);
        data_buf += BUFF_HEADER_SIZE;
    }

    common_info.tbl = tbl;
    common_info.trans_id = trans_id;
    common_info.in = in;
    common_info.extra_in = returned_fields;
    common_info.data_buf = data_buf;
    common_info.enable_ha = enable_ha;
    common_info.aux = aux;
    common_info.d_ptr = d_ptr;
    common_info.k_ptr = k_ptr;
    if (tbl->type == IMM_BASIC) {
        SHR_IF_ERR_EXIT
            (basic_insert(unit, &common_info, key_size, key_buf));
    } else {
        SHR_IF_ERR_EXIT
            (bcmimm_direct_entry_insert(unit, &common_info));
    }
    staged = false;

    if (tbl->interactive) {
        SHR_FREE(data_buf);
    } else if (!ha_enabled) {
        buffs_cancel(unit, tbl, aux);
    }
exit:
    /* In case of an error free ownership if owner is south bound */
    if (SHR_FUNC_ERR()) {
        if (tbl) {
            if (staged) {
                bcmimm_update_trans_info(unit, tbl, 0, 0, trans_id);
            }
            /* Clean up the buffer*/
            if (data_buf) {
                if (enable_ha) {
                    buffs_free(unit, tbl, BUFF_HEADER_SIZE, key_size, aux);
                } else {
                    if (tbl->interactive) {
                        SHR_FREE(data_buf);
                    } else {
                        buffs_cancel(unit, tbl, aux);
                    }
                }
            }
        }
    }
    SHR_FUNC_EXIT();
}

int bcmimm_intern_entry_get_first(int unit,
                                  const bcmltd_op_arg_t *op_arg,
                                  bcmltd_sid_t sid,
                                  bcmltd_fields_t *out,
                                  bcmimm_tbl_owner_t owner)
{
    table_info_t *tbl;
    bcmimm_be_fields_t be_out;
    uint8_t *data_buf = NULL;
    uint8_t *key_buf;
    uint32_t buf_size;
    uint32_t key_size = 1;
    uint32_t data_f_size;
    size_t field_cnt;
    uint32_t tbl_idx;
    bcmimm_table_dynamic_info_t *dyn_info;
    bool use_snapshot = false;

    SHR_FUNC_ENTER(unit);
    tbl = bcmimm_tbl_find(unit, sid, &tbl_idx);
    if (!tbl) {
        SHR_VERBOSE_EXIT(SHR_E_NOT_FOUND);
    }

    if (op_arg) {
        if (op_arg->attrib & BCMLT_ENT_ATTR_TRAVERSE_DONE) {
            bcmimm_be_table_snapshot_free(tbl->blk_hdl);
            SHR_EXIT();
        }
        if (op_arg->attrib & BCMLT_ENT_ATTR_TRAVERSE_SNAPSHOT) {
            use_snapshot = true;
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmimm_be_table_snapshot_create(tbl->blk_hdl));
        }
    }
    if (tbl->key_fld_cnt > 0) {
        key_size = BCMIMM_BITS_TO_BYTES(tbl->key_arry_size);
    }
    if (tbl->type == IMM_DIRECT) {
        key_size = sizeof(uint32_t);
    }
    buf_size = key_size + tbl->data_bits_len;

    SHR_ALLOC(data_buf, buf_size, "bcmimmDataBuf");
    SHR_NULL_CHECK(data_buf, SHR_E_MEMORY);
    sal_memset(data_buf, 0, buf_size);

    if (tbl->type == IMM_BASIC) {
        data_f_size = BCMIMM_BE_FID_SIZE + tbl->f_data_size;
        be_out.count = tbl->data_bits_len / data_f_size;
        be_out.fid = (uint32_t *)data_buf;
        be_out.fdata = data_buf + BCMIMM_BE_FID_SIZE * be_out.count;
    } else {
        if (tbl->array_idx_size) {
            data_f_size = sizeof(bcmimm_be_direct_tbl_field_array_t);
        } else {
            data_f_size = sizeof(bcmimm_be_direct_tbl_field_basic_t);
        }
        be_out.count = tbl->data_bits_len / data_f_size;
        be_out.fid = NULL;
        be_out.fdata = data_buf;
    }
    key_buf = data_buf + tbl->data_bits_len; /* Key at the buffer ends */

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmimm_be_table_get_first(tbl->blk_hdl, use_snapshot,
                                   key_buf, &be_out));
    if (be_out.count + tbl->key_fld_cnt > out->count) {
        SHR_ERR_EXIT(SHR_E_RESOURCE);
    }

    /*
     * Note that be_out->count is the total number of fields that were
     * retrieved from the BE entry. This counts each array element.
     */
    dyn_info = &bcmimm_dynamic_info[unit][tbl->fields_idx];
    if (tbl->type == IMM_BASIC) {
        bcmimm_extract_buf_into_flds(&be_out, tbl, out);
    } else {
        bcmimm_direct_extract_buf_into_flds(&be_out, tbl, out);
    }
    field_cnt = be_out.count;
    extract_key_into_flds(key_buf, tbl, dyn_info, out, &field_cnt);
    if (owner == BCMIMM_TABLE_NB) {
        if (be_out.count < tbl->max_num_field) {
            out->count = field_cnt +
                bcmimm_def_vals_fill(unit, out, field_cnt, tbl);
        } else {
            out->count = field_cnt;
        }
        SHR_IF_ERR_EXIT(
            bcmimm_notify_lookup(unit, NULL, tbl, tbl_idx,
                                 op_arg, BCMIMM_TRAVERSE, out));
    } else {
        out->count = field_cnt;
    }

exit:
    SHR_FREE(data_buf);
    SHR_FUNC_EXIT();
}

int bcmimm_intern_entry_get_next(int unit,
                                 const bcmltd_op_arg_t *op_arg,
                                 bcmltd_sid_t sid,
                                 const bcmltd_fields_t *in,
                                 bcmltd_fields_t *out,
                                 bcmimm_tbl_owner_t owner)
{
    table_info_t *tbl;
    bcmimm_be_fields_t be_out;
    uint8_t *data_buf = NULL;
    uint8_t *key_buf;
    uint32_t buf_size;
    uint8_t *in_key_buf;
    uint32_t key_size = 1;
    uint32_t data_f_size;
    size_t field_cnt;
    uint32_t tbl_idx;
    bcmimm_table_dynamic_info_t *dyn_info;
    bool use_snapshot = false;

    SHR_FUNC_ENTER(unit);

    tbl = bcmimm_tbl_find(unit, sid, &tbl_idx);
    if (!tbl) {
        SHR_VERBOSE_EXIT(SHR_E_NOT_FOUND);
    }

    if (op_arg) {
        if (op_arg->attrib & BCMLT_ENT_ATTR_TRAVERSE_DONE) {
            bcmimm_be_table_snapshot_free(tbl->blk_hdl);
            SHR_EXIT();
        }
        if (op_arg->attrib & BCMLT_ENT_ATTR_TRAVERSE_SNAPSHOT) {
            use_snapshot = true;
        }
    }
    if (tbl->key_fld_cnt > 0) {
        key_size = BCMIMM_BITS_TO_BYTES(tbl->key_arry_size);
    }
    if (tbl->type == IMM_DIRECT) {
        key_size = sizeof(uint32_t);
    }
    /* Key size being taken twice, once for input and once for output */
    buf_size = key_size * 2 + tbl->data_bits_len;

    SHR_ALLOC(data_buf, buf_size, "bcmimmDataBuf");
    SHR_NULL_CHECK(data_buf, SHR_E_MEMORY);
    sal_memset(data_buf, 0, buf_size);

    if (tbl->type == IMM_BASIC) {
        data_f_size = BCMIMM_BE_FID_SIZE + tbl->f_data_size;
        be_out.count = tbl->data_bits_len / data_f_size;
        be_out.fid = (uint32_t *)data_buf;
        be_out.fdata = data_buf + BCMIMM_BE_FID_SIZE * be_out.count;
    } else {
        if (tbl->array_idx_size) {
            data_f_size = sizeof(bcmimm_be_direct_tbl_field_array_t);
        } else {
            data_f_size = sizeof(bcmimm_be_direct_tbl_field_basic_t);
        }
        be_out.count = tbl->data_bits_len / data_f_size;
        be_out.fid = NULL;
        be_out.fdata = data_buf;
    }
    in_key_buf = data_buf + tbl->data_bits_len;
    key_buf = in_key_buf + key_size;
    sal_memset(in_key_buf, 0, 2 * key_size);

    /* Extract the key field into the input key (in_key_buf) */
    dyn_info = &bcmimm_dynamic_info[unit][tbl->fields_idx];
    SHR_IF_ERR_EXIT(bcmimm_key_build(tbl, dyn_info, in, in_key_buf));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmimm_be_table_get_next(tbl->blk_hdl, use_snapshot,
                                  in_key_buf, key_buf, &be_out));

    if (be_out.count + tbl->key_fld_cnt > out->count) {
        SHR_ERR_EXIT(SHR_E_RESOURCE);
    }

    /*
     * Extract the data fields and key into the output (out).
     * For NB interface we also need to fill up all the fields that were not
     * been set with default values.
     */
    dyn_info = &bcmimm_dynamic_info[unit][tbl->fields_idx];
    if (tbl->type == IMM_BASIC) {
        bcmimm_extract_buf_into_flds(&be_out, tbl, out);
    } else {
        bcmimm_direct_extract_buf_into_flds(&be_out, tbl, out);
    }
    field_cnt = be_out.count;
    extract_key_into_flds(key_buf, tbl, dyn_info, out, &field_cnt);
    if (owner == BCMIMM_TABLE_NB) {
        if (be_out.count < tbl->max_num_field) {
            out->count = field_cnt +
                bcmimm_def_vals_fill(unit, out, field_cnt, tbl);
        } else {
            out->count = field_cnt;
        }
        SHR_IF_ERR_EXIT(
            bcmimm_notify_lookup(unit, in, tbl, tbl_idx,
                                 op_arg, BCMIMM_TRAVERSE, out));
    } else {
        out->count = field_cnt;
    }

exit:
    SHR_FREE(data_buf);
    SHR_FUNC_EXIT();
}
