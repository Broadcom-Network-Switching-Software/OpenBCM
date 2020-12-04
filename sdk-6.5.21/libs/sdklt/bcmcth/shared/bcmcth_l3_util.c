/*! \file bcmcth_l3_util.c
 *
 * Tunnel utility function
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
#include <bcmcth/bcmcth_l3_util.h>

/*******************************************************************************
 * Defines
 */
#define BSL_LOG_MODULE BSL_LS_BCMCTH_L3

/*******************************************************************************
 * Public Functions
 */
int
bcmcth_l3_hw_read(int unit, uint32_t trans_id, bcmltd_sid_t lt_id,
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
    SHR_IF_ERR_EXIT(
        bcmptm_ltm_mreq_indexed_lt(unit, 0, pt_id, &pt_info, NULL, NULL,
                                   BCMPTM_OP_READ,
                                   NULL, rsp_entry_wsize, lt_id,
                                   trans_id, NULL, NULL,
                                   entry_data, &rsp_ltid, &rsp_flags));

 exit:
    SHR_FUNC_EXIT();
}


int
bcmcth_l3_hw_write(int unit, uint32_t trans_id, bcmltd_sid_t lt_id,
                   bcmdrd_sid_t pt_id, int index, void *entry_data)
{
    bcmbd_pt_dyn_info_t pt_info;
    bcmltd_sid_t rsp_ltid = 0;
    uint32_t rsp_flags = 0;

    SHR_FUNC_ENTER(unit);

    sal_memset(&pt_info, 0, sizeof(bcmbd_pt_dyn_info_t));
    pt_info.index = index;
    SHR_IF_ERR_EXIT(
        bcmptm_ltm_mreq_indexed_lt(unit, 0, pt_id, &pt_info, NULL, NULL,
                                   BCMPTM_OP_WRITE,
                                   entry_data, 0, lt_id,
                                   trans_id, NULL, NULL,
                                   NULL, &rsp_ltid, &rsp_flags));

 exit:
    SHR_FUNC_EXIT();
}

int
bcmcth_l3_hw_op_read(int unit, const bcmltd_op_arg_t *op_arg, bcmltd_sid_t lt_id,
                     bcmdrd_sid_t pt_id, int index, bool ireq, void *entry_data)
{
    bcmbd_pt_dyn_info_t pt_info;
    uint32_t            rsp_entry_wsize;
    bcmltd_sid_t        rsp_ltid = 0;
    uint32_t            rsp_flags = 0;
    uint32_t            req_flags = 0;

    SHR_FUNC_ENTER(unit);

    sal_memset(&pt_info, 0, sizeof(bcmbd_pt_dyn_info_t));
    pt_info.index = index;
    rsp_entry_wsize = bcmdrd_pt_entry_wsize(unit, pt_id);
    req_flags = bcmptm_lt_entry_attrib_to_ptm_req_flags(op_arg->attrib);

    if (ireq) {
        req_flags =
            bcmptm_lt_entry_attrib_to_ptm_req_flags(BCMLT_ENT_ATTR_GET_FROM_HW);
        SHR_IF_ERR_EXIT(
            bcmptm_ltm_ireq_read(unit,
                                 req_flags,
                                 pt_id,
                                 &pt_info,
                                 NULL,
                                 rsp_entry_wsize,
                                 lt_id,
                                 entry_data,
                                 &rsp_ltid,
                                 &rsp_flags));
    } else {
        SHR_IF_ERR_EXIT(
            bcmptm_ltm_mreq_indexed_lt(unit, req_flags, pt_id, &pt_info, NULL,
                                       NULL, BCMPTM_OP_READ,
                                       NULL, rsp_entry_wsize, lt_id,
                                       op_arg->trans_id, NULL, NULL,
                                       entry_data, &rsp_ltid, &rsp_flags));
    }
 exit:
    SHR_FUNC_EXIT();
}


int
bcmcth_l3_hw_op_write(int unit, const bcmltd_op_arg_t *op_arg,
                      bcmltd_sid_t lt_id,bcmdrd_sid_t pt_id, int index,
                      void *entry_data)
{
    bcmbd_pt_dyn_info_t pt_info;
    bcmltd_sid_t rsp_ltid = 0;
    uint32_t rsp_flags = 0;
    uint32_t req_flags = 0;

    SHR_FUNC_ENTER(unit);

    sal_memset(&pt_info, 0, sizeof(bcmbd_pt_dyn_info_t));
    pt_info.index = index;
    req_flags = bcmptm_lt_entry_attrib_to_ptm_req_flags(op_arg->attrib);
    SHR_IF_ERR_EXIT(
        bcmptm_ltm_mreq_indexed_lt(unit, req_flags, pt_id, &pt_info, NULL,
                                   NULL, BCMPTM_OP_WRITE,
                                   entry_data, 0, lt_id,
                                   op_arg->trans_id, NULL, NULL,
                                   NULL, &rsp_ltid, &rsp_flags));

 exit:
    SHR_FUNC_EXIT();
}
