/*! \file bcmcth_tnl_util.c
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
#include <bcmltd/bcmltd_handler.h>
#include <bcmptm/bcmptm.h>
#include <bcmcth/bcmcth_tnl_util.h>

/*******************************************************************************
 * Defines
 */
#define BSL_LOG_MODULE BSL_LS_BCMCTH_TNL

/*******************************************************************************
 * Public Functions
 */
int
bcmcth_tnl_hw_read(int unit, const bcmltd_op_arg_t *op_arg, bcmltd_sid_t lt_id,
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
        bcmptm_ltm_mreq_indexed_lt(unit, op_arg->attrib, pt_id, &pt_info, NULL,
                                   NULL, BCMPTM_OP_READ,
                                   NULL, rsp_entry_wsize, lt_id,
                                   op_arg->trans_id, NULL, NULL,
                                   entry_data, &rsp_ltid, &rsp_flags));

 exit:
    SHR_FUNC_EXIT();
}


int
bcmcth_tnl_hw_write(int unit, const bcmltd_op_arg_t *op_arg, bcmltd_sid_t lt_id,
                    bcmdrd_sid_t pt_id, int index, void *entry_data)
{
    bcmbd_pt_dyn_info_t pt_info;
    bcmltd_sid_t rsp_ltid = 0;
    uint32_t rsp_flags = 0;

    SHR_FUNC_ENTER(unit);

    sal_memset(&pt_info, 0, sizeof(bcmbd_pt_dyn_info_t));
    pt_info.index = index;
    SHR_IF_ERR_EXIT(
        bcmptm_ltm_mreq_indexed_lt(unit, op_arg->attrib, pt_id, &pt_info, NULL,
                                   NULL, BCMPTM_OP_WRITE,
                                   entry_data, 0, lt_id,
                                   op_arg->trans_id, NULL, NULL,
                                   NULL, &rsp_ltid, &rsp_flags));

 exit:
    SHR_FUNC_EXIT();
}
