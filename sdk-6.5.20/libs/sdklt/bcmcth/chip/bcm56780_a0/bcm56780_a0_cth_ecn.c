/*! \file bcm56780_a0_cth_ecn.c
 *
 * Chip stub for BCMCTH ECN.
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

#include <bcmcth/bcmcth_ecn_drv.h>
#include <bcmcth/bcmcth_util.h>
#include <bcmcth/bcmcth_ecn_exp_to_ecn.h>
#include <bcmdrd/bcmdrd_pt.h>


/* Debug log target definition */
#define BSL_LOG_MODULE BSL_LS_BCMCTH_ECN

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
bcm56780_a0_cth_ecn_exp_to_ecn_map_ecn_set(int unit, bcmcth_ecn_exp_to_ecn_map_t *lt_entry)
{
    bcmcth_pt_op_info_t pt_op_info;
    uint32_t *buf_ptr = NULL;
    uint32_t offset = 0;
    MPLS_PRE_PROC_EXP_TO_ECN_MAPPINGm_t exp_to_ecn_map_buf;
    SHR_BITDCLNAME(data, 512);

    SHR_FUNC_ENTER(unit);

    if (!BCMCTH_LT_FIELD_GET(
             lt_entry->fbmp, BCMCTH_ECN_NEW)) {
        SHR_EXIT();
    }
    sal_memset(&exp_to_ecn_map_buf, 0, sizeof(exp_to_ecn_map_buf));


    buf_ptr = (uint32_t *)&exp_to_ecn_map_buf;

    offset = (lt_entry->map_id << 5) | (lt_entry->exp << 2) | lt_entry->ecn;
    offset <<= 1;

    /* read */
    bcm56780_a0_cth_pt_op_info_t_init(&pt_op_info);
    pt_op_info.drd_sid = MPLS_PRE_PROC_EXP_TO_ECN_MAPPINGm;
    pt_op_info.trans_id = lt_entry->trans_id;
    pt_op_info.req_lt_sid = lt_entry->glt_sid;
    pt_op_info.op = BCMPTM_OP_READ;

    pt_op_info.wsize = bcmdrd_pt_entry_wsize(unit, MPLS_PRE_PROC_EXP_TO_ECN_MAPPINGm);

    /* Set up hardware table index to write. */
    pt_op_info.dyn_info.index = 0;

    SHR_IF_ERR_EXIT
       (bcmcth_pt_read(unit, &pt_op_info, buf_ptr));

    bcmdrd_pt_field_get(unit, MPLS_PRE_PROC_EXP_TO_ECN_MAPPINGm, buf_ptr,
                        DATAf, data);

    if (lt_entry->new_ecn & 0x1) {
        SHR_BITSET(data, offset);
    } else {
        SHR_BITCLR(data, offset);
    }

    if (lt_entry->new_ecn & 0x2) {
        SHR_BITSET(data, offset + 1);
    } else {
        SHR_BITCLR(data, offset + 1);
    }

    bcmdrd_pt_field_set(unit, MPLS_PRE_PROC_EXP_TO_ECN_MAPPINGm,
                        buf_ptr, DATAf, data);


    /* write */
    bcm56780_a0_cth_pt_op_info_t_init(&pt_op_info);
    pt_op_info.drd_sid = MPLS_PRE_PROC_EXP_TO_ECN_MAPPINGm;
    pt_op_info.trans_id = lt_entry->trans_id;
    pt_op_info.req_lt_sid = lt_entry->glt_sid;
    pt_op_info.op = BCMPTM_OP_WRITE;

    pt_op_info.wsize = bcmdrd_pt_entry_wsize(unit, MPLS_PRE_PROC_EXP_TO_ECN_MAPPINGm);

    /* Set up hardware table index to write. */
    pt_op_info.dyn_info.index = 0;

    SHR_IF_ERR_EXIT
        (bcmcth_pt_write(unit, &pt_op_info, buf_ptr));
exit:
    SHR_FUNC_EXIT();
}


static int
bcm56780_a0_cth_ecn_exp_to_ecn_map_drop_set(int unit, bcmcth_ecn_exp_to_ecn_map_t *lt_entry)
{
    bcmcth_pt_op_info_t pt_op_info;
    uint32_t *buf_ptr = NULL;
    uint32_t offset = 0;
    SHR_BITDCLNAME(data, 256);

    MPLS_PRE_PROC_EXP_TO_ECN_DROPm_t exp_to_ecn_drop_buf;

    SHR_FUNC_ENTER(unit);

    if (!BCMCTH_LT_FIELD_GET(
             lt_entry->fbmp, BCMCTH_ECN_DROP)) {
        SHR_EXIT();
    }
    sal_memset(&exp_to_ecn_drop_buf, 0, sizeof(exp_to_ecn_drop_buf));


    buf_ptr = (uint32_t *)&exp_to_ecn_drop_buf;

    offset = (lt_entry->map_id << 5) | (lt_entry->exp << 2) | lt_entry->ecn;

    /* read */
    bcm56780_a0_cth_pt_op_info_t_init(&pt_op_info);
    pt_op_info.drd_sid = MPLS_PRE_PROC_EXP_TO_ECN_DROPm;
    pt_op_info.trans_id = lt_entry->trans_id;
    pt_op_info.req_lt_sid = lt_entry->glt_sid;
    pt_op_info.op = BCMPTM_OP_READ;

    pt_op_info.wsize = bcmdrd_pt_entry_wsize(unit, MPLS_PRE_PROC_EXP_TO_ECN_DROPm);

    /* Set up hardware table index to write. */
    pt_op_info.dyn_info.index = 0;

    SHR_IF_ERR_EXIT
       (bcmcth_pt_read(unit, &pt_op_info, buf_ptr));

    bcmdrd_pt_field_get(unit, MPLS_PRE_PROC_EXP_TO_ECN_DROPm, buf_ptr,
                        DATAf, data);

    if (lt_entry->drop & 0x1) {
        SHR_BITSET(data, offset);
    } else {
        SHR_BITCLR(data, offset);
    }

    bcmdrd_pt_field_set(unit, MPLS_PRE_PROC_EXP_TO_ECN_DROPm,
                        buf_ptr, DATAf, data);


    /* write */
    bcm56780_a0_cth_pt_op_info_t_init(&pt_op_info);
    pt_op_info.drd_sid = MPLS_PRE_PROC_EXP_TO_ECN_DROPm;
    pt_op_info.trans_id = lt_entry->trans_id;
    pt_op_info.req_lt_sid = lt_entry->glt_sid;
    pt_op_info.op = BCMPTM_OP_WRITE;

    pt_op_info.wsize = bcmdrd_pt_entry_wsize(unit, MPLS_PRE_PROC_EXP_TO_ECN_DROPm);

    /* Set up hardware table index to write. */
    pt_op_info.dyn_info.index = 0;

    SHR_IF_ERR_EXIT
        (bcmcth_pt_write(unit, &pt_op_info, buf_ptr));
exit:
    SHR_FUNC_EXIT();
}

static int
bcm56780_a0_cth_ecn_exp_to_ecn_map_set(int unit, bcmcth_ecn_exp_to_ecn_map_t *lt_entry)
{
    SHR_FUNC_ENTER(unit);

    if (!BCMCTH_LT_FIELD_GET(
             lt_entry->fbmp, BCMCTH_ECN_NEW) &&
         !BCMCTH_LT_FIELD_GET(
             lt_entry->fbmp, BCMCTH_ECN_DROP)) {
        SHR_EXIT();
    }
    SHR_IF_ERR_EXIT
        (bcm56780_a0_cth_ecn_exp_to_ecn_map_ecn_set(unit, lt_entry));
    SHR_IF_ERR_EXIT
        (bcm56780_a0_cth_ecn_exp_to_ecn_map_drop_set(unit, lt_entry));

exit:
    SHR_FUNC_EXIT();
}


static bcmcth_ecn_drv_t bcm56780_a0_cth_ecn_drv = {
    .exp_to_ecn_map_set = bcm56780_a0_cth_ecn_exp_to_ecn_map_set
};

/*******************************************************************************
* Public functions
 */

bcmcth_ecn_drv_t *
bcm56780_a0_cth_ecn_drv_get(int unit)
{
    return &bcm56780_a0_cth_ecn_drv;
}


