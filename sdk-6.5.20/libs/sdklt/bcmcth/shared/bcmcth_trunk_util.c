/*! \file bcmcth_trunk_util.c
 *
 * bcmcth_trunk utility function
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_debug.h>
#include <bcmbd/bcmbd.h>
#include <bcmdrd/bcmdrd_pt.h>
#include <bcmlrd/bcmlrd_table.h>
#include <bcmlrd/bcmlrd_client.h>
#include <bcmptm/bcmptm.h>
#include <bcmcth/bcmcth_trunk_types.h>
#include <bcmcth/bcmcth_trunk_util.h>

/*******************************************************************************
 * Defines
 */
/* BSL Module TBD */
#define BSL_LOG_MODULE BSL_LS_BCMCTH_TRUNK

/*******************************************************************************
 * Private functions
 */
/*!
 * \brief Convert the union value bcmlt_field_data_t to uint64_t.
 *
 * \param [in] width Field width in bits.
 * \param [in] field_data Field map data which includes the max and min field
 *                        value in bcmlt_field_data_t.
 * \param [out] min Minimum field value in uint64_t.
 * \param [out] max Maximum field value in uint64_t.
 */
static void
lth_field_data_to_u64(uint32_t width, const bcmlrd_field_data_t *field_data,
                      uint64_t *min, uint64_t *max)
{
    if (width == 1) {
        *min = (uint64_t)field_data->min->is_true;
        *max = (uint64_t)field_data->max->is_true;
    } else if (width <= 8) {
        *min = (uint64_t)field_data->min->u8;
        *max = (uint64_t)field_data->max->u8;
    } else if (width <= 16) {
        *min = (uint64_t)field_data->min->u16;
        *max = (uint64_t)field_data->max->u16;
    } else if (width <= 32) {
        *min = (uint64_t)field_data->min->u32;
        *max = (uint64_t)field_data->max->u32;
    } else {
        *min = (uint64_t)field_data->min->u64;
        *max = (uint64_t)field_data->max->u64;
    }
}

/*******************************************************************************
 * Public Functions
 */

int
bcmcth_trunk_hw_read_dma(int unit,
                         const bcmltd_op_arg_t *op_arg,
                         bcmltd_sid_t lt_id,
                         bcmdrd_sid_t pt_id,
                         int start_index,
                         int entry_cnt,
                         void *entry_buf)
{
    bcmbd_pt_dyn_info_t pt_info;
    uint32_t rsp_entry_wsize;
    bcmltd_sid_t rsp_ltid = 0;
    uint32_t rsp_flags = 0;
    bcmptm_misc_info_t misc_info;
    uint64_t req_flags;

    SHR_FUNC_ENTER(unit);

    sal_memset(&pt_info, 0, sizeof(bcmbd_pt_dyn_info_t));
    sal_memset(&misc_info, 0, sizeof(bcmptm_misc_info_t));
    pt_info.index = start_index;
    pt_info.tbl_inst = -1;
    misc_info.dma_enable = TRUE;
    misc_info.dma_entry_count = entry_cnt;
    rsp_entry_wsize = entry_cnt * bcmdrd_pt_entry_wsize(unit, pt_id);
    req_flags = bcmptm_lt_entry_attrib_to_ptm_req_flags(op_arg->attrib);
    SHR_IF_ERR_EXIT
        (bcmptm_ltm_mreq_indexed_lt(unit, req_flags, pt_id, &pt_info,
                                    NULL, &misc_info, BCMPTM_OP_READ,
                                    NULL, rsp_entry_wsize, lt_id,
                                    op_arg->trans_id, NULL, NULL,
                                    entry_buf, &rsp_ltid, &rsp_flags));

 exit:
    SHR_FUNC_EXIT();
}


int
bcmcth_trunk_hw_write_dma(int unit,
                          const bcmltd_op_arg_t *op_arg,
                          bcmltd_sid_t lt_id,
                          bcmdrd_sid_t pt_id,
                          int start_index,
                          int entry_cnt,
                          void *entry_buf)
{
    bcmbd_pt_dyn_info_t pt_info;
    uint32_t entry_wsize;
    bcmltd_sid_t rsp_ltid = 0;
    uint32_t rsp_flags = 0;
    bcmptm_misc_info_t misc_info;
    uint64_t req_flags;

    SHR_FUNC_ENTER(unit);

    sal_memset(&pt_info, 0, sizeof(bcmbd_pt_dyn_info_t));
    sal_memset(&misc_info, 0, sizeof(bcmptm_misc_info_t));
    pt_info.index = start_index;
    pt_info.tbl_inst = -1;
    misc_info.dma_enable = TRUE;
    misc_info.dma_entry_count = entry_cnt;
    entry_wsize = entry_cnt * bcmdrd_pt_entry_wsize(unit, pt_id);
    req_flags = bcmptm_lt_entry_attrib_to_ptm_req_flags(op_arg->attrib);
    SHR_IF_ERR_EXIT
        (bcmptm_ltm_mreq_indexed_lt(unit, req_flags, pt_id, &pt_info,
                                    NULL, &misc_info, BCMPTM_OP_WRITE,
                                    entry_buf, entry_wsize, lt_id,
                                    op_arg->trans_id, NULL, NULL,
                                    NULL, &rsp_ltid, &rsp_flags));

 exit:
    SHR_FUNC_EXIT();
}


int
bcmcth_trunk_hw_read(int unit, const bcmltd_op_arg_t *op_arg,
                     bcmltd_sid_t lt_id, bcmdrd_sid_t pt_id,
                     int index, void *entry_data)
{
    bcmbd_pt_dyn_info_t pt_info;
    uint32_t rsp_entry_wsize;
    bcmltd_sid_t rsp_ltid = 0;
    uint32_t rsp_flags = 0;
    uint64_t req_flags;

    SHR_FUNC_ENTER(unit);

    sal_memset(&pt_info, 0, sizeof(bcmbd_pt_dyn_info_t));
    pt_info.index = index;
    rsp_entry_wsize = bcmdrd_pt_entry_wsize(unit, pt_id);
    req_flags = bcmptm_lt_entry_attrib_to_ptm_req_flags(op_arg->attrib);
    SHR_IF_ERR_EXIT
        (bcmptm_ltm_mreq_indexed_lt(unit, req_flags, pt_id, &pt_info,
                                    NULL, NULL,
                                    BCMPTM_OP_READ,
                                    NULL, rsp_entry_wsize, lt_id,
                                    op_arg->trans_id, NULL, NULL,
                                    entry_data, &rsp_ltid, &rsp_flags));

 exit:
    SHR_FUNC_EXIT();
}


int
bcmcth_trunk_hw_write(int unit, const bcmltd_op_arg_t *op_arg,
                      bcmltd_sid_t lt_id, bcmdrd_sid_t pt_id,
                      int index, void *entry_data)
{
    bcmbd_pt_dyn_info_t pt_info;
    bcmltd_sid_t rsp_ltid = 0;
    uint32_t rsp_flags = 0;
    uint64_t req_flags;

    SHR_FUNC_ENTER(unit);

    sal_memset(&pt_info, 0, sizeof(bcmbd_pt_dyn_info_t));
    pt_info.index = index;
    pt_info.tbl_inst = -1;
    req_flags = bcmptm_lt_entry_attrib_to_ptm_req_flags(op_arg->attrib);
    SHR_IF_ERR_EXIT
        (bcmptm_ltm_mreq_indexed_lt(unit, req_flags, pt_id, &pt_info,
                                    NULL, NULL, BCMPTM_OP_WRITE,
                                    entry_data, 0, lt_id,
                                    op_arg->trans_id, NULL, NULL,
                                    NULL, &rsp_ltid, &rsp_flags));

 exit:
    SHR_FUNC_EXIT();
}

int
bcmcth_trunk_pt_hw_read(int unit, const bcmltd_op_arg_t *op_arg,
                        bcmltd_sid_t lt_id, bcmdrd_sid_t pt_id,
                        int index, void *entry_data)
{
    bcmbd_pt_dyn_info_t pt_info;
    uint32_t rsp_entry_wsize;
    bcmltd_sid_t rsp_ltid = 0;
    uint32_t rsp_flags = 0;
    uint64_t req_flags;

    SHR_FUNC_ENTER(unit);

    sal_memset(&pt_info, 0, sizeof(bcmbd_pt_dyn_info_t));
    pt_info.tbl_inst = index;
    rsp_entry_wsize = bcmdrd_pt_entry_wsize(unit, pt_id);
    req_flags = bcmptm_lt_entry_attrib_to_ptm_req_flags(op_arg->attrib);
    SHR_IF_ERR_EXIT
        (bcmptm_ltm_mreq_indexed_lt(unit, req_flags, pt_id, &pt_info,
                                    NULL, NULL, BCMPTM_OP_READ,
                                    NULL, rsp_entry_wsize, lt_id,
                                    op_arg->trans_id, NULL, NULL,
                                    entry_data, &rsp_ltid, &rsp_flags));

 exit:
    SHR_FUNC_EXIT();
}

int
bcmcth_trunk_pt_hw_write(int unit, const bcmltd_op_arg_t *op_arg,
                         bcmltd_sid_t lt_id, bcmdrd_sid_t pt_id,
                         int index, void *entry_data)
{
    bcmbd_pt_dyn_info_t pt_info;
    bcmltd_sid_t rsp_ltid = 0;
    uint32_t rsp_flags = 0;
    uint64_t req_flags;

    SHR_FUNC_ENTER(unit);

    sal_memset(&pt_info, 0, sizeof(bcmbd_pt_dyn_info_t));
    pt_info.tbl_inst = index;
    req_flags = bcmptm_lt_entry_attrib_to_ptm_req_flags(op_arg->attrib);
    SHR_IF_ERR_EXIT
        (bcmptm_ltm_mreq_indexed_lt(unit, req_flags, pt_id, &pt_info,
                                    NULL, NULL, BCMPTM_OP_WRITE,
                                    entry_data, 0, lt_id,
                                    op_arg->trans_id, NULL, NULL,
                                    NULL, &rsp_ltid, &rsp_flags));

 exit:
    SHR_FUNC_EXIT();
}

int
bcmcth_trunk_ireq_read(int unit, bcmltd_sid_t lt_id,
                       bcmdrd_sid_t pt_id, int index, void *entry_data)
{
    bcmbd_pt_dyn_info_t pt_info;
    uint32_t            rsp_entry_wsize;
    bcmltd_sid_t        rsp_ltid = 0;
    uint32_t            rsp_flags = 0;

    SHR_FUNC_ENTER(unit);

    sal_memset(&pt_info, 0, sizeof(bcmbd_pt_dyn_info_t));
    pt_info.index = index;
    rsp_entry_wsize = bcmdrd_pt_entry_wsize(unit, pt_id);
    SHR_IF_ERR_EXIT
        (bcmptm_ltm_ireq_read(unit, 0, pt_id, &pt_info,
                              NULL, rsp_entry_wsize, lt_id,
                              entry_data, &rsp_ltid, &rsp_flags));
 exit:
    SHR_FUNC_EXIT();
}

int
bcmcth_trunk_ireq_write(int unit, bcmltd_sid_t lt_id,
                        bcmdrd_sid_t pt_id, int index, void *entry_data)
{
    bcmbd_pt_dyn_info_t pt_info;
    bcmltd_sid_t rsp_ltid = 0;
    uint32_t rsp_flags = 0;

    SHR_FUNC_ENTER(unit);

    sal_memset(&pt_info, 0, sizeof(bcmbd_pt_dyn_info_t));
    pt_info.index = index;
    SHR_IF_ERR_EXIT
        (bcmptm_ltm_ireq_write(unit, 0, pt_id, &pt_info, NULL,
                               entry_data, lt_id, &rsp_ltid, &rsp_flags));

 exit:
    SHR_FUNC_EXIT();
}

/*
 * Infields from LT API may just have partial fields of a LT.
 * This function is to update members of parameters.
 */
int
bcmcth_trunk_param_update(int unit, bcmcth_trunk_group_param_t *param,
                          bcmcth_trunk_group_t *grp)
{
    int px;

    SHR_FUNC_ENTER(unit);

    if ((!param) || (!grp)) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    if (!param->lb_mode_valid) {
        param->lb_mode = grp->lb_mode;
    }
    if (!param->uc_rtag_valid) {
        param->uc_rtag = grp->uc_rtag;
    }
    if (!param->uc_max_members_valid) {
        param->uc_max_members = grp->uc_max_members;
    }
    if (!param->uc_agm_pool_valid) {
        param->uc_agm_pool = grp->uc_agm_pool;
    }
    if (!param->uc_agm_id_valid) {
        param->uc_agm_id = grp->uc_agm_id;
    }
    if (!param->uc_cnt_valid) {
        param->uc_cnt = grp->uc_cnt;
    }
    if (!param->nonuc_cnt_valid) {
        param->nonuc_cnt = grp->nonuc_cnt;
    }
    for (px = 0; px < param->uc_cnt; px++) {
        if (!param->uc_modid_valid[px]) {
            param->uc_modid[px] = grp->uc_modid[px];
        }
        if (!param->uc_modport_valid[px]) {
            param->uc_modport[px] = grp->uc_modport[px];
        }
    }
    for (px = 0; px < param->nonuc_cnt; px++) {
        if (!param->nonuc_modid_valid[px]) {
            param->nonuc_modid[px] = grp->nonuc_modid[px];
        }
        if (!param->nonuc_modport_valid[px]) {
            param->nonuc_modport[px] = grp->nonuc_modport[px];
        }
    }

exit:
    SHR_FUNC_EXIT();
}

/*
 * Infields from LT API may just have partial fields of a LT.
 * This function is to update members of parameters.
 */
int
bcmcth_trunk_fast_param_update(int unit, bcmcth_trunk_fast_group_param_t *param,
                               bcmcth_trunk_fast_group_t *grp)
{
    int px;

    SHR_FUNC_ENTER(unit);

    if (param == NULL) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    if (!param->lb_mode_valid) {
        param->lb_mode = grp->lb_mode;
    }
    if (!param->uc_rtag_valid) {
        param->uc_rtag = grp->uc_rtag;
    }
    if (!param->uc_agm_pool_valid) {
        param->uc_agm_pool = grp->uc_agm_pool;
    }
    if (!param->uc_agm_id_valid) {
        param->uc_agm_id = grp->uc_agm_id;
    }
    if (!param->uc_cnt_valid) {
        param->uc_cnt = grp->uc_cnt;
    }
    if (!param->nonuc_cnt_valid) {
        param->nonuc_cnt = grp->nonuc_cnt;
    }
    for (px = 0; px < param->uc_cnt; px++) {
        if (!param->uc_modid_valid[px]) {
            param->uc_modid[px] = grp->uc_modid[px];
        }
        if (!param->uc_modport_valid[px]) {
            param->uc_modport[px] = grp->uc_modport[px];
        }
    }
    for (px = 0; px < param->nonuc_cnt; px++) {
        if (!param->nonuc_modid_valid[px]) {
            param->nonuc_modid[px] = grp->nonuc_modid[px];
        }
        if (!param->nonuc_modport_valid[px]) {
            param->nonuc_modport[px] = grp->nonuc_modport[px];
        }
    }
    if (!param->ing_eflex_action_id_valid) {
        param->ing_eflex_action_id = grp->ing_eflex_action_id;
    }
    if (!param->ing_eflex_object_valid) {
        param->ing_eflex_object = grp->ing_eflex_object;
    }

exit:

    SHR_FUNC_EXIT();
}

/*
 * Infields from LT API may just have partial fields of a LT.
 * This function is to update members of parameters.
 */
int
bcmcth_trunk_failover_param_update(int unit,
                                   bcmcth_trunk_failover_param_t *param,
                                   bcmcth_trunk_failover_t *entry)
{
    int px;

    SHR_FUNC_ENTER(unit);

    if ((!param) || (!entry)) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    if (!param->rtag_valid) {
        param->rtag = entry->rtag;
    }
    if (!param->failover_cnt_valid) {
        param->failover_cnt = entry->failover_cnt;
    }
    for (px = 0; px < param->failover_cnt; px++) {
        if (!param->failover_modid_valid[px]) {
            param->failover_modid[px] = entry->failover_modid[px];
        }
        if (!param->failover_modport_valid[px]) {
            param->failover_modport[px] = entry->failover_modport[px];
        }
    }

exit:
    SHR_FUNC_EXIT();
}


int
bcmcth_trunk_field_value_range_get(int unit, uint32_t sid, uint32_t fid,
                                   uint64_t *min, uint64_t *max)
{
    const bcmlrd_field_data_t *field_data;
    const bcmlrd_table_rep_t *tab_md;
    const bcmltd_field_rep_t *field_md;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmlrd_field_get(unit, sid, fid, &field_data));

    tab_md = bcmlrd_table_get(sid);

    SHR_NULL_CHECK(tab_md, SHR_E_FAIL);
    if (fid >= tab_md->fields) {
        SHR_ERR_EXIT(SHR_E_FAIL);
    }

    field_md = &(tab_md->field[fid]);
    lth_field_data_to_u64(field_md->width, field_data, min, max);

exit:
    SHR_FUNC_EXIT();
}

int
bcmcth_trunk_field_get(int unit, const bcmltd_fields_t *in, uint32_t fid,
                       uint64_t *fval)
{
    size_t i;

    SHR_FUNC_ENTER(unit);

    *fval = 0;

    for (i = 0; i < in->count; i++) {
        if (fid == in->field[i]->id) {
            *fval = in->field[i]->data;
            SHR_EXIT();
        }
    }

    SHR_ERR_EXIT(SHR_E_FAIL);

exit:
    SHR_FUNC_EXIT();
}

int
bcmcth_trunk_member_free(int unit, SHR_BITDCL *bmp,
                         uint32_t base, int num_entries)
{
    SHR_FUNC_ENTER(unit);

    SHR_BITCLR_RANGE(bmp, base, num_entries);

    SHR_FUNC_EXIT();
}

int
bcmcth_trunk_member_base_get(int unit, SHR_BITDCL *bmp,
                             int tbl_size, int num_entries, uint32_t *base)
{
    int base_ptr, max_base_ptr;

    SHR_FUNC_ENTER(unit);

    base_ptr = 0;
    max_base_ptr = tbl_size - num_entries;

    for (; base_ptr <= max_base_ptr; base_ptr++) {
        /* Check if the contiguous region of TRUNK_MEMBERm table from
         * index base_ptr to (base_ptr + num_entries - 1) is free.
         */
        if (SHR_BITNULL_RANGE(bmp, base_ptr, num_entries)) {
            break;
        }
    }

    if (base_ptr > max_base_ptr) {
        SHR_ERR_EXIT(SHR_E_RESOURCE);
    }

    *base = base_ptr;
    SHR_BITSET_RANGE(bmp, base_ptr, num_entries);

 exit:
    SHR_FUNC_EXIT();
}

int
bcmcth_trunk_max_members_update(int unit, SHR_BITDCL *bmp,
                                uint32_t tbl_size,
                                uint32_t old_max, uint32_t old_base,
                                uint32_t new_max, uint32_t *new_base_ptr)
{
    uint32_t idx;

    SHR_FUNC_ENTER(unit);

    if (new_max < old_max) {
        *new_base_ptr = old_base;
        SHR_BITCLR_RANGE(bmp, (old_base + new_max), (old_max - new_max));
        SHR_EXIT();
    }

    /* Check if it is possible to increase max grp size in-place. */
    for (idx = old_base + old_max;
         (idx < (old_base + new_max)) && (idx <= tbl_size); idx++) {
        if (SHR_BITGET(bmp, idx)) {
            break;
        }
    }
    if (idx == (old_base + new_max)) {
        *new_base_ptr = old_base;
        SHR_BITSET_RANGE(bmp, (old_base + old_max), (new_max - old_max));
        SHR_EXIT();
    }

    SHR_IF_ERR_EXIT
        (bcmcth_trunk_member_base_get(unit, bmp, tbl_size,
                                      new_max, new_base_ptr));

 exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get the field value given the imm field list and field ID.
 *
 * \param [in] unit Unit number.
 * \param [in] in  List of imm fields.
 * \param [in] fid Field ID.
 * \param [out] fval Field value.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcmcth_trunk_imm_field_get(int unit,
                           const bcmltd_field_t *in,
                           uint32_t fid,
                           uint64_t *fval)
{
   SHR_FUNC_ENTER(unit);
   SHR_NULL_CHECK(in, SHR_E_INTERNAL);

    *fval = 0;
    do {
        if (fid == in->id) {
            *fval = in->data;
            SHR_EXIT();
        }
        in = in->next;
    } while (in != NULL);
    SHR_ERR_EXIT(SHR_E_NOT_FOUND);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Validate the index is  less than the count.
 *
 * Parse  Checks if the member port index is in range
 *        of unicast or non-unicast member count.
 *
 * \param [in] unit Unit number.
 * \param [in] cnt  Count.
 * \param [in] idx  Array Index.
 *
 * \retval SHR_E_NONE if validation is successful.
 * \retval SHR_E_FAIL if validation failed.
 */
int
bcmcth_trunk_validate_member_idx(int unit,
                                 int cnt,
                                 int idx)
{
    SHR_FUNC_ENTER(unit);

    if (idx >= cnt) {
        LOG_VERBOSE(BSL_LOG_MODULE,
                   (BSL_META_U(
                    unit,
                    "Member array idx %d invalid.\n"),
                    idx));
        SHR_ERR_EXIT(SHR_E_PARAM);
    }
exit:
    SHR_FUNC_EXIT();
}
