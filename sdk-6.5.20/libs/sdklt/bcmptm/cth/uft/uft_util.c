/*! \file uft_util.c
 *
 * uft utility function
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/*******************************************************************************
 * Includes
 */
#include <shr/shr_debug.h>
#include <bcmbd/bcmbd.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmimm/bcmimm_int_comp.h>
#include <bcmdrd/bcmdrd_pt.h>
#include <bcmlrd/bcmlrd_table.h>
#include <bcmptm/bcmptm_cth_uft_be_internal.h>
#include "uft_util.h"

/*******************************************************************************
 * Local Defintions
 */

/* BSL Module TBD */
#define BSL_LOG_MODULE BSL_LS_BCMPTM_UFT

/*******************************************************************************
 * Public Functions
 */

int
bcmptm_uft_lt_hw_read(int unit, bcmltd_sid_t lt_id,
                      const bcmltd_op_arg_t *op_arg,
                      bcmdrd_sid_t pt_id, int index,
                      uint32_t entry_size, void *entry_data)
{
    bcmbd_pt_dyn_info_t pt_info;
    uint32_t rsp_entry_wsize;
    bcmltd_sid_t rsp_ltid = 0;
    uint32_t rsp_flags = 0;
    uint64_t req_flags = 0;

    SHR_FUNC_ENTER(unit);

    pt_info.index = index;
    pt_info.tbl_inst = -1;
    rsp_entry_wsize = bcmdrd_pt_entry_wsize(unit, pt_id);
    if (entry_size < rsp_entry_wsize) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }
    req_flags = bcmptm_lt_entry_attrib_to_ptm_req_flags(op_arg->attrib);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_ltm_mreq_indexed_lt(unit, req_flags, pt_id, &pt_info,
                                    NULL, NULL, BCMPTM_OP_READ,
                                    NULL, rsp_entry_wsize, lt_id,
                                    op_arg->trans_id, NULL, NULL,
                                    entry_data, &rsp_ltid, &rsp_flags));

 exit:
    SHR_FUNC_EXIT();
}


int
bcmptm_uft_lt_hw_write(int unit, bcmltd_sid_t lt_id,
                       const bcmltd_op_arg_t *op_arg,
                       bcmdrd_sid_t pt_id, int index, void *entry_data)
{
    bcmbd_pt_dyn_info_t pt_info;
    bcmltd_sid_t rsp_ltid = 0;
    uint32_t rsp_flags = 0;
    uint64_t req_flags = 0;

    SHR_FUNC_ENTER(unit);

    pt_info.index = index;
    pt_info.tbl_inst = -1;
    req_flags = bcmptm_lt_entry_attrib_to_ptm_req_flags(op_arg->attrib);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_ltm_mreq_indexed_lt(unit, req_flags, pt_id, &pt_info,
                                    NULL, NULL, BCMPTM_OP_WRITE,
                                    entry_data, 0, lt_id,
                                    op_arg->trans_id, NULL, NULL,
                                    NULL, &rsp_ltid, &rsp_flags));

 exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_uft_lt_ireq_read(int unit, bcmltd_sid_t lt_id,
                        bcmdrd_sid_t pt_id, int index,
                        uint32_t entry_size, void *entry_data)
{
    bcmbd_pt_dyn_info_t pt_info;
    uint32_t            rsp_entry_wsize;
    bcmltd_sid_t rsp_ltid = 0;
    uint32_t rsp_flags = 0;

    SHR_FUNC_ENTER(unit);

    pt_info.index = index;
    pt_info.tbl_inst = -1;
    rsp_entry_wsize = bcmdrd_pt_entry_wsize(unit, pt_id);
    if (entry_size < rsp_entry_wsize) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_ltm_ireq_read(unit, 0, pt_id, &pt_info, NULL,
                              rsp_entry_wsize, lt_id,
                              entry_data,&rsp_ltid, &rsp_flags));

 exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_uft_lt_ireq_write(int unit, bcmltd_sid_t lt_id,
                         bcmdrd_sid_t pt_id, int index, void *entry_data)
{
    bcmbd_pt_dyn_info_t pt_info;
    bcmltd_sid_t rsp_ltid = 0;
    uint32_t rsp_flags = 0;

    SHR_FUNC_ENTER(unit);

    pt_info.index = index;
    pt_info.tbl_inst = -1;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_ltm_ireq_write(unit, 0, pt_id, &pt_info, NULL,
                               entry_data, lt_id, &rsp_ltid, &rsp_flags));

 exit:
    SHR_FUNC_EXIT();
}

int
uft_lt_grp_get_from_imm(int unit, uft_dev_em_grp_t *grp)
{
    uint32_t fid, fidx;
    uint64_t fval;
    size_t idx;
    bcmltd_fields_t input, output;

    SHR_FUNC_ENTER(unit);

    input.count  = 1;
    input.field  = NULL;
    output.count = UFT_FIELD_COUNT_MAX;
    output.field = NULL;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_uft_lt_fields_buff_alloc(unit, &input));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_uft_lt_fields_buff_alloc(unit, &output));
    input.field[0]->id = DEVICE_EM_GROUPt_DEVICE_EM_GROUP_IDf;
    input.field[0]->data = grp->grp_id;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmimm_entry_lookup(unit, DEVICE_EM_GROUPt,
                            &input, &output));

    for (idx = 0; idx < output.count; idx++) {
        fid = output.field[idx]->id;
        fval = output.field[idx]->data;
        fidx = output.field[idx]->idx;
        switch (fid) {
            case DEVICE_EM_GROUPt_DEVICE_EM_GROUP_IDf:
                break;
            case DEVICE_EM_GROUPt_ROBUSTf:
                grp->robust = fval;
                break;
            case DEVICE_EM_GROUPt_DEVICE_EM_BANK_IDf:
                grp->bank[fidx] = fval;
                break;
            case DEVICE_EM_GROUPt_NUM_BANKSf:
                grp->bank_cnt = fval;
                break;
            case DEVICE_EM_GROUPt_VECTOR_TYPEf:
                grp->vector_type = fval;
                break;
            default:
                break;
        }
    }

exit:
    (void) bcmptm_uft_lt_fields_buff_free(unit, &input);
    (void) bcmptm_uft_lt_fields_buff_free(unit, &output);
    SHR_FUNC_EXIT();
}

int
bcmptm_uft_hw_write_dma(int unit,
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

