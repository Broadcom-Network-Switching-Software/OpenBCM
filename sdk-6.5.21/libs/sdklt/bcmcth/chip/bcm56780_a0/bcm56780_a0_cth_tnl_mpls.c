/*! \file bcm56780_a0_cth_tnl_mpls.c
 *
 * Chip stub for BCMCTH TNL MPLS.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_debug.h>
#include <sal/sal_libc.h>
#include <shr/shr_bitop.h>
#include <bcmdrd/chip/bcm56780_a0_enum.h>
#include <bcmdrd/chip/bcm56780_a0_defs.h>
#include <bcmcth/bcmcth_tnl_mpls_drv.h>
#include <bcmcth/bcmcth_util.h>
#include <bcmdrd/bcmdrd_pt.h>
#include <bcmptm/bcmptm.h>


/* Debug log target definition */
#define BSL_LOG_MODULE BSL_LS_BCMCTH_TNL

/*******************************************************************************
* Private functions
 */
static void
bcm56780_a0_cth_pt_op_info_t_init(bcmcth_pt_op_info_t *pt_op_info)
{
    if (pt_op_info) {
        pt_op_info->drd_sid = BCM56780_A0_ENUM_COUNT;
        pt_op_info->trans_id = -1;
        pt_op_info->req_lt_sid = BCM56780_A0_ENUM_COUNT;
        pt_op_info->rsp_lt_sid = BCM56780_A0_ENUM_COUNT;
        pt_op_info->req_flags = BCMPTM_REQ_FLAGS_NO_FLAGS;
        pt_op_info->rsp_flags = BCMPTM_REQ_FLAGS_NO_FLAGS;
        pt_op_info->op = BCMPTM_OP_NOP;
        pt_op_info->dyn_info.index = -1;
        pt_op_info->dyn_info.tbl_inst = -1;
        pt_op_info->wsize = 0;
    }
    return;
}

static int
bcm56780_a0_cth_tnl_mpls_qos_ctrl_set(int unit,
                                  bcmcth_tnl_mpls_exp_qos_ctrl_t *lt_entry)
{
    bcmcth_pt_op_info_t pt_op_info;
    uint32_t *buf_ptr = NULL;
    uint32_t offset = 0;
    MPLS_PRE_PROC_EFFECTIVE_EXP_QOSm_t exp_qos_buf;
    SHR_BITDCLNAME(data, 256);

    SHR_FUNC_ENTER(unit);

    if (!BCMCTH_LT_FIELD_GET(
             lt_entry->fbmp, BCMCTH_TNL_MPLS_EXP_QOS_SEL)) {
        SHR_EXIT();
    }
    sal_memset(&exp_qos_buf, 0, sizeof(exp_qos_buf));


    buf_ptr = (uint32_t *)&exp_qos_buf;

    offset = (lt_entry->map_id[0] << 4) | (lt_entry->map_id[1] << 2) | lt_entry->map_id[2];
    offset |= (!!lt_entry->effective) << 6;
    offset <<= 1;

    /* read */
    bcm56780_a0_cth_pt_op_info_t_init(&pt_op_info);
    pt_op_info.drd_sid = MPLS_PRE_PROC_EFFECTIVE_EXP_QOSm;
    pt_op_info.trans_id = lt_entry->op_arg->trans_id;
    pt_op_info.req_flags =
        bcmptm_lt_entry_attrib_to_ptm_req_flags(lt_entry->op_arg->attrib);
    pt_op_info.req_lt_sid = lt_entry->glt_sid;
    pt_op_info.op = BCMPTM_OP_READ;

    pt_op_info.wsize = bcmdrd_pt_entry_wsize(unit, MPLS_PRE_PROC_EFFECTIVE_EXP_QOSm);

    /* Set up hardware table index to write. */
    pt_op_info.dyn_info.index = 0;

    SHR_IF_ERR_EXIT
       (bcmcth_pt_read(unit, &pt_op_info, buf_ptr));

    bcmdrd_pt_field_get(unit, MPLS_PRE_PROC_EFFECTIVE_EXP_QOSm, buf_ptr,
                        DATAf, data);

    if (lt_entry->qos_sel & 0x1) {
        SHR_BITSET(data, offset);
    } else {
        SHR_BITCLR(data, offset);
    }

    if (lt_entry->qos_sel & 0x2) {
        SHR_BITSET(data, offset + 1);
    } else {
        SHR_BITCLR(data, offset + 1);
    }

    bcmdrd_pt_field_set(unit, MPLS_PRE_PROC_EFFECTIVE_EXP_QOSm,
                        buf_ptr, DATAf, data);


    /* write */
    bcm56780_a0_cth_pt_op_info_t_init(&pt_op_info);
    pt_op_info.drd_sid = MPLS_PRE_PROC_EFFECTIVE_EXP_QOSm;
    pt_op_info.trans_id = lt_entry->op_arg->trans_id;
    pt_op_info.req_flags =
        bcmptm_lt_entry_attrib_to_ptm_req_flags(lt_entry->op_arg->attrib);
    pt_op_info.req_lt_sid = lt_entry->glt_sid;
    pt_op_info.op = BCMPTM_OP_WRITE;

    pt_op_info.wsize = bcmdrd_pt_entry_wsize(unit, MPLS_PRE_PROC_EFFECTIVE_EXP_QOSm);

    /* Set up hardware table index to write. */
    pt_op_info.dyn_info.index = 0;

    SHR_IF_ERR_EXIT
        (bcmcth_pt_write(unit, &pt_op_info, buf_ptr));
exit:
    SHR_FUNC_EXIT();
}

static int
bcm56780_a0_cth_tnl_mpls_remark_ctrl_set(int unit,
                                     bcmcth_tnl_mpls_exp_remark_ctrl_t *lt_entry)
{
    bcmcth_pt_op_info_t pt_op_info;
    uint32_t *buf_ptr = NULL;
    uint32_t offset = 0;
    MPLS_PRE_PROC_EFFECTIVE_EXP_REMARKINGm_t exp_remark_buf;
    SHR_BITDCLNAME(data, 256);

    SHR_FUNC_ENTER(unit);

    if (!BCMCTH_LT_FIELD_GET(
             lt_entry->fbmp, BCMCTH_TNL_MPLS_EXP_REMARK_SEL)) {
        SHR_EXIT();
    }
    sal_memset(&exp_remark_buf, 0, sizeof(exp_remark_buf));


    buf_ptr = (uint32_t *)&exp_remark_buf;

    offset = (lt_entry->map_id[0] << 4) | (lt_entry->map_id[1] << 2) | lt_entry->map_id[2];
    offset |= (!!lt_entry->effective) << 6;
    offset <<= 1;

    /* read */
    bcm56780_a0_cth_pt_op_info_t_init(&pt_op_info);
    pt_op_info.drd_sid = MPLS_PRE_PROC_EFFECTIVE_EXP_REMARKINGm;
    pt_op_info.trans_id = lt_entry->op_arg->trans_id;
    pt_op_info.req_flags =
        bcmptm_lt_entry_attrib_to_ptm_req_flags(lt_entry->op_arg->attrib);
    pt_op_info.req_lt_sid = lt_entry->glt_sid;
    pt_op_info.op = BCMPTM_OP_READ;

    pt_op_info.wsize = bcmdrd_pt_entry_wsize(unit, MPLS_PRE_PROC_EFFECTIVE_EXP_REMARKINGm);

    /* Set up hardware table index to write. */
    pt_op_info.dyn_info.index = 0;

    SHR_IF_ERR_EXIT
       (bcmcth_pt_read(unit, &pt_op_info, buf_ptr));

    bcmdrd_pt_field_get(unit, MPLS_PRE_PROC_EFFECTIVE_EXP_REMARKINGm, buf_ptr,
                        DATAf, data);

    if (lt_entry->remark_sel & 0x1) {
        SHR_BITSET(data, offset);
    } else {
        SHR_BITCLR(data, offset);
    }

    if (lt_entry->remark_sel & 0x2) {
        SHR_BITSET(data, offset + 1);
    } else {
        SHR_BITCLR(data, offset + 1);
    }

    bcmdrd_pt_field_set(unit, MPLS_PRE_PROC_EFFECTIVE_EXP_REMARKINGm,
                        buf_ptr, DATAf, data);


    /* write */
    bcm56780_a0_cth_pt_op_info_t_init(&pt_op_info);
    pt_op_info.drd_sid = MPLS_PRE_PROC_EFFECTIVE_EXP_REMARKINGm;
    pt_op_info.trans_id = lt_entry->op_arg->trans_id;
    pt_op_info.req_flags =
        bcmptm_lt_entry_attrib_to_ptm_req_flags(lt_entry->op_arg->attrib);
    pt_op_info.req_lt_sid = lt_entry->glt_sid;
    pt_op_info.op = BCMPTM_OP_WRITE;

    pt_op_info.wsize = bcmdrd_pt_entry_wsize(unit, MPLS_PRE_PROC_EFFECTIVE_EXP_REMARKINGm);

    /* Set up hardware table index to write. */
    pt_op_info.dyn_info.index = 0;

    SHR_IF_ERR_EXIT
        (bcmcth_pt_write(unit, &pt_op_info, buf_ptr));
exit:
    SHR_FUNC_EXIT();
}



static bcmcth_tnl_mpls_drv_t bcm56780_a0_cth_tnl_mpls_drv = {
    .qos_ctrl_set = bcm56780_a0_cth_tnl_mpls_qos_ctrl_set,
    .remark_ctrl_set = bcm56780_a0_cth_tnl_mpls_remark_ctrl_set,
};

/*******************************************************************************
* Public functions
 */

bcmcth_tnl_mpls_drv_t *
bcm56780_a0_cth_tnl_mpls_drv_get(int unit)
{
    return &bcm56780_a0_cth_tnl_mpls_drv;
}


