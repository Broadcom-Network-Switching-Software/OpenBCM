/*! \file bcm56990_a0_ecmp.c
 *
 * This file implements BCM56990_A0 device specific ECMP functions.
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
#include <shr/shr_error.h>
#include <shr/shr_pb.h>
#include <shr/shr_ha.h>
#include <bcmdrd/chip/bcm56990_a0_enum.h>
#include <bcmdrd/bcmdrd_pt.h>
#include <bcmptm/bcmptm.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmlrd/bcmlrd_map.h>
#include <bcmmgmt/bcmmgmt_sysm.h>
#include <bcmecmp/bcmecmp_db_internal.h>
#include <bcmecmp/bcmecmp_group_util.h>
#include <bcmecmp/bcmecmp_member_weight.h>

/*******************************************************************************
 * Defines
 */
#define BSL_LOG_MODULE BSL_LS_BCMLTX_ECMP

#define NHOPS_PER_MEMB_ENTRY (4)

#define NHOPS_PER_MEMB_ENTRY_WECMP (1)

#define MEMBER_WEIGHT_QUANT_FACTOR (4096)

/*
 * MEMBER_REPLICATION and MEMBER_WEIGHTED use the same WEIGHTED_SIZE value
 * in LT, and this WEIGHTED_SIZE is the real hardware value of
 * MEMBER_REPLICATION.
 * For MEMBER_WEIGHT, use this offset plus WEIGHTED_SIZE to get the real
 * hardware value.
 */
#define MEMBER_WEIGHT_WEIGHTED_SIZE_HW_FIELD_OFFSET (6)

typedef struct bcm56990_a0_ecmp_memb_drd_fields_s {
    bcmdrd_fid_t entry_type;

    bcmdrd_fid_t wecmp_weight;
    bcmdrd_fid_t wecmp_nhop_id[2];

    bcmdrd_fid_t ecmp_nhop_id[4];
} bcm56990_a0_ecmp_memb_drd_fields_t;

typedef struct bcm56990_a0_ecmp_grp_drd_fields_s {
    bcmdrd_fid_t lb_mode;

    bcmdrd_fid_t base_ptr;
    bcmdrd_fid_t count;

    bcmdrd_fid_t flex_ctr_object;
    bcmdrd_fid_t flex_ctr_action_sel;
} bcm56990_a0_ecmp_grp_drd_fields_t;

/*******************************************************************************
 * Local definitions
 */
static bcmecmp_control_t *ecmp_ctrl_local[BCMECMP_NUM_UNITS_MAX] = {NULL};
/*******************************************************************************
 * Private functions
 */
static void
bcm56990_a0_ecmp_grp_drd_fields_t_init(bcm56990_a0_ecmp_grp_drd_fields_t *gdrd)
{
    if (gdrd) {
        sal_memset(gdrd, INVALIDf, sizeof(*gdrd));
    }
    return;
}

static void
bcm56990_a0_ecmp_memb_drd_fields_t_init(bcm56990_a0_ecmp_memb_drd_fields_t *mdrd)
{
    if (mdrd) {
        sal_memset(mdrd, INVALIDf, sizeof(*mdrd));
    }
    return;
}

static void
bcm56990_a0_ecmp_grp_ltd_fields_t_init(bcmecmp_grp_ltd_fields_t *gltd)
{
    if (gltd) {
        sal_memset(gltd, BCMLTD_SID_INVALID, sizeof(*gltd));
    }
    return;
}

static void
bcm56990_a0_ecmp_lt_field_attrs_t_init(bcmecmp_lt_field_attrs_t *fattrs)
{
    if (fattrs) {
        sal_memset(fattrs, BCMECMP_INVALID, sizeof(*fattrs));
    }
    return;
}

static void
bcm56990_a0_ecmp_pt_op_info_t_init(bcmecmp_pt_op_info_t *pt_op_info)
{
    /* Initialize structure member variables. */
    if (pt_op_info) {
        pt_op_info->drd_sid = INVALIDm;
        pt_op_info->req_lt_sid = BCMLTD_SID_INVALID;
        pt_op_info->rsp_lt_sid = BCMLTD_SID_INVALID;
        pt_op_info->req_flags = BCMPTM_REQ_FLAGS_NO_FLAGS;
        pt_op_info->rsp_flags = BCMPTM_REQ_FLAGS_NO_FLAGS;
        pt_op_info->op = BCMPTM_OP_NOP;
        pt_op_info->dyn_info.index = BCMECMP_INVALID;
        pt_op_info->dyn_info.tbl_inst = BCMECMP_ALL_PIPES_INST;
        pt_op_info->wsize = 0;
        pt_op_info->dma = FALSE;
        pt_op_info->ecount = 0;
        pt_op_info->op_arg = NULL;
    }

    return;
}

static int
bcm56990_a0_ecmp_pt_write(int unit,
                          bcmecmp_pt_op_info_t *op_info,
                          uint32_t *buf)
{
    uint32_t *rsp_ent_buf = NULL; /* Valid buffer required only for READ. */
    void *ptr_pt_ovr_info = NULL; /* Physical Table Override info pointer. */
    bcmptm_misc_info_t misc_info; /* Miscellaneous DMA oper info.  */
    uint64_t req_flags; /* PTM required flags. */

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(op_info, SHR_E_PARAM);
    SHR_NULL_CHECK(buf, SHR_E_PARAM);

    /* Initialize the structure and set the values for PTM call. */
    sal_memset(&misc_info, 0, sizeof(bcmptm_misc_info_t));
    if (op_info->dma) {
        if (!op_info->ecount) {
            /* To perform TDMA valid entries count value is required. */
            SHR_ERR_EXIT(SHR_E_INTERNAL);
        }
        misc_info.dma_enable = op_info->dma;
        misc_info.dma_entry_count = op_info->ecount;
    }

    req_flags = bcmptm_lt_entry_attrib_to_ptm_req_flags(op_info->op_arg->attrib);

    /*
     * Check if Write operation has to be performed using DMA engine. If DMA is
     * enabled, pass valid 'misc_info' argument to PTM function or else pass
     * NULL value as argument to misc_info PTM function input parameter.
     */
    if (misc_info.dma_enable) {
        SHR_IF_ERR_EXIT
            (bcmptm_ltm_mreq_indexed_lt(unit,
                                        req_flags,
                                        op_info->drd_sid,
                                        (void *)&(op_info->dyn_info),
                                        ptr_pt_ovr_info,
                                        &misc_info,
                                        op_info->op,
                                        buf,
                                        op_info->wsize,
                                        op_info->req_lt_sid,
                                        op_info->op_arg->trans_id,
                                        NULL,
                                        NULL,
                                        rsp_ent_buf,
                                        &(op_info->rsp_lt_sid),
                                        &(op_info->rsp_flags)));
    } else {
        SHR_IF_ERR_EXIT
            (bcmptm_ltm_mreq_indexed_lt(unit,
                                        req_flags,
                                        op_info->drd_sid,
                                        (void *)&(op_info->dyn_info),
                                        ptr_pt_ovr_info,
                                        NULL, /* misc_info */
                                        op_info->op,
                                        buf,
                                        op_info->wsize,
                                        op_info->req_lt_sid,
                                        op_info->op_arg->trans_id,
                                        NULL,
                                        NULL,
                                        rsp_ent_buf,
                                        &(op_info->rsp_lt_sid),
                                        &(op_info->rsp_flags)));
    }

    LOG_DEBUG(BSL_LOG_MODULE,
              (BSL_META_U(unit,
                          "\tpt_write[drd_sid=%d idx=%d]: trans_id=0x%x "
                          "req_ltid=0x%x rsp_ltid=0x%x\n"),
              op_info->drd_sid,
              op_info->dyn_info.index,
              op_info->op_arg->trans_id,
              op_info->req_lt_sid,
              op_info->rsp_lt_sid));

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56990_a0_ecmp_pt_read(int unit,
                         bcmecmp_pt_op_info_t *op_info,
                         uint32_t *buf)
{
    uint32_t *ent_buf = NULL; /* Valid buffer required only for READ. */
    void *ptr_pt_ovr_info = NULL; /* Physical Table Override info pointer. */
    bcmptm_misc_info_t misc_info; /* Miscellaneous DMA oper info.  */
    uint64_t req_flags; /* PTM required flags. */
    uint32_t flags_0, flags_1;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(op_info, SHR_E_PARAM);
    SHR_NULL_CHECK(buf, SHR_E_PARAM);

    req_flags = bcmptm_lt_entry_attrib_to_ptm_req_flags(op_info->op_arg->attrib);
    flags_0 = (uint32_t) req_flags;
    flags_1 = (uint32_t) (req_flags >> 32);
    LOG_DEBUG(BSL_LOG_MODULE,
              (BSL_META_U(unit,
                          "\tpt_read-in[drd_sid=%d hw_idx=%d]: trans_id=0x%x "
                          " req_flags=0x%x%x "
                          " req_ltid=0x%x rsp_ltid=0x%x\n"),
              op_info->drd_sid,
              op_info->dyn_info.index,
              op_info->op_arg->trans_id,
              flags_1,
              flags_0,
              op_info->req_lt_sid,
              op_info->rsp_lt_sid));

    /* Initialize the structure and set the values for PTM call. */
    sal_memset(&misc_info, 0, sizeof(bcmptm_misc_info_t));
    if (op_info->dma) {
        if (!op_info->ecount) {
            /* To perform TDMA valid entries count value is required. */
            SHR_ERR_EXIT(SHR_E_INTERNAL);
        }
        misc_info.dma_enable = op_info->dma;
        misc_info.dma_entry_count = op_info->ecount;
    }

    /*
     * Check if Write operation has to be performed using DMA engine. If DMA is
     * enabled, pass valid 'misc_info' argument to PTM function or else pass
     * NULL value as argument to misc_info PTM function input parameter.
     */
    if (misc_info.dma_enable) {
        SHR_IF_ERR_EXIT
            (bcmptm_ltm_mreq_indexed_lt(unit,
                                        req_flags,
                                        op_info->drd_sid,
                                        (void *)&(op_info->dyn_info),
                                        ptr_pt_ovr_info,
                                        &misc_info,
                                        op_info->op,
                                        ent_buf,
                                        op_info->wsize,
                                        op_info->req_lt_sid,
                                        op_info->op_arg->trans_id,
                                        NULL,
                                        NULL,
                                        buf,
                                        &(op_info->rsp_lt_sid),
                                        &(op_info->rsp_flags)));

    } else {
        SHR_IF_ERR_EXIT
            (bcmptm_ltm_mreq_indexed_lt(unit,
                                        req_flags,
                                        op_info->drd_sid,
                                        (void *)&(op_info->dyn_info),
                                        ptr_pt_ovr_info,
                                        NULL, /* misc_info */
                                        op_info->op,
                                        ent_buf,
                                        op_info->wsize,
                                        op_info->req_lt_sid,
                                        op_info->op_arg->trans_id,
                                        NULL,
                                        NULL,
                                        buf,
                                        &(op_info->rsp_lt_sid),
                                        &(op_info->rsp_flags)));
    }

    /*
     * PTM Values:
     *  - BCMPTM_LTID_RSP_NOT_SUPPORTED 0xFFFFFFFD
     *  - BCMPTM_LTID_RSP_HW 0xFFFFFFFF
     */
    LOG_DEBUG(BSL_LOG_MODULE,
              (BSL_META_U(unit,
                          "\tpt_read-out[drd_sid=%d hw_idx=%d]: trans_id=0x%x "
                          " req_flags=0x%x%x "
                          " req_ltid=0x%x rsp_ltid=0x%x\n"),
              op_info->drd_sid,
              op_info->dyn_info.index,
              op_info->op_arg->trans_id,
              flags_1,
              flags_0,
              op_info->req_lt_sid,
              op_info->rsp_lt_sid));

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56990_a0_ecmp_lt_entry_memb_write(int unit, bcmecmp_lt_entry_t *lt_ent,
                                     int ent_idx, int ent_num, bool dma_en,
                                     uint32_t *buf)
{
    bcmecmp_pt_op_info_t op_info;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(lt_ent, SHR_E_PARAM);
    SHR_NULL_CHECK(buf, SHR_E_PARAM);

    bcm56990_a0_ecmp_pt_op_info_t_init(&op_info);
    op_info.op = BCMPTM_OP_WRITE;
    op_info.op_arg = lt_ent->op_arg;
    op_info.req_lt_sid = lt_ent->glt_sid;
    op_info.drd_sid = BCMECMP_TBL_DRD_SID(unit, member, lt_ent->grp_type);
    op_info.wsize = BCMECMP_TBL_ENT_SIZE(unit, member, lt_ent->grp_type);
    op_info.dyn_info.index = ent_idx;
    op_info.ecount = ent_num;
    op_info.dma = dma_en;

    SHR_IF_ERR_EXIT
        (bcm56990_a0_ecmp_pt_write(unit, &op_info, buf));

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56990_a0_ecmp_lt_entry_memb_read(int unit, bcmecmp_lt_entry_t *lt_ent,
                                    int ent_idx, int ent_num, bool dma_en,
                                    uint32_t *buf)
{
    bcmecmp_pt_op_info_t op_info;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(lt_ent, SHR_E_PARAM);
    SHR_NULL_CHECK(buf, SHR_E_PARAM);

    bcm56990_a0_ecmp_pt_op_info_t_init(&op_info);
    op_info.op = BCMPTM_OP_READ;
    op_info.op_arg = lt_ent->op_arg;
    op_info.req_lt_sid = lt_ent->glt_sid;
    op_info.drd_sid = BCMECMP_TBL_DRD_SID(unit, member, lt_ent->grp_type);
    op_info.wsize = BCMECMP_TBL_ENT_SIZE(unit, member, lt_ent->grp_type) * ent_num;
    op_info.dyn_info.index = ent_idx;
    op_info.ecount = ent_num;
    op_info.dma = dma_en;

    SHR_IF_ERR_EXIT
        (bcm56990_a0_ecmp_pt_read(unit, &op_info, buf));

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56990_a0_ecmp_lt_entry_grp_write(int unit, bcmecmp_lt_entry_t *lt_ent,
                                    uint32_t *buf)
{
    bcmecmp_pt_op_info_t op_info;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(lt_ent, SHR_E_PARAM);
    SHR_NULL_CHECK(buf, SHR_E_PARAM);

    bcm56990_a0_ecmp_pt_op_info_t_init(&op_info);
    op_info.op = BCMPTM_OP_WRITE;
    op_info.op_arg = lt_ent->op_arg;
    op_info.req_lt_sid = lt_ent->glt_sid;
    op_info.drd_sid = BCMECMP_TBL_DRD_SID(unit, group, lt_ent->grp_type);
    op_info.wsize = BCMECMP_TBL_ENT_SIZE(unit, group, lt_ent->grp_type);
    op_info.dyn_info.index = lt_ent->ecmp_id;

    SHR_IF_ERR_EXIT
        (bcm56990_a0_ecmp_pt_write(unit, &op_info, buf));

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56990_a0_ecmp_lt_entry_grp_read(int unit, bcmecmp_lt_entry_t *lt_ent,
                                   uint32_t *buf)
{
    bcmecmp_pt_op_info_t op_info;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(lt_ent, SHR_E_PARAM);
    SHR_NULL_CHECK(buf, SHR_E_PARAM);

    bcm56990_a0_ecmp_pt_op_info_t_init(&op_info);
    op_info.op = BCMPTM_OP_READ;
    op_info.op_arg = lt_ent->op_arg;
    op_info.req_lt_sid = lt_ent->glt_sid;
    op_info.drd_sid = BCMECMP_TBL_DRD_SID(unit, group, lt_ent->grp_type);
    op_info.wsize = BCMECMP_TBL_ENT_SIZE(unit, group, lt_ent->grp_type);
    op_info.dyn_info.index = lt_ent->ecmp_id;

    SHR_IF_ERR_EXIT
        (bcm56990_a0_ecmp_pt_read(unit, &op_info, buf));

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56990_a0_ecmp_dfrag_memb_write(int unit, bcmecmp_grp_defrag_t *dfrag,
                                  int ent_idx, int ent_num, bool dma_en,
                                  uint32_t *buf)
{
    bcmecmp_pt_op_info_t op_info;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(dfrag, SHR_E_PARAM);
    SHR_NULL_CHECK(buf, SHR_E_PARAM);

    bcm56990_a0_ecmp_pt_op_info_t_init(&op_info);
    op_info.op =BCMPTM_OP_WRITE;
    op_info.op_arg = dfrag->op_arg;
    op_info.req_lt_sid = dfrag->glt_sid;
    op_info.drd_sid = BCMECMP_TBL_DRD_SID(unit, member, dfrag->gtype);
    op_info.wsize = BCMECMP_TBL_ENT_SIZE(unit, member, dfrag->gtype);
    op_info.dyn_info.index = ent_idx;
    op_info.ecount = ent_num;
    op_info.dma = dma_en;

    SHR_IF_ERR_EXIT
        (bcm56990_a0_ecmp_pt_write(unit, &op_info, buf));

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56990_a0_ecmp_dfrag_memb_read(int unit, bcmecmp_grp_defrag_t *dfrag,
                                 int ent_idx, int ent_num, bool dma_en,
                                 uint32_t *buf)
{
    bcmecmp_pt_op_info_t op_info;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(dfrag, SHR_E_PARAM);
    SHR_NULL_CHECK(buf, SHR_E_PARAM);

    bcm56990_a0_ecmp_pt_op_info_t_init(&op_info);
    op_info.op =BCMPTM_OP_READ;
    op_info.op_arg = dfrag->op_arg;
    op_info.req_lt_sid = dfrag->glt_sid;
    op_info.drd_sid = BCMECMP_TBL_DRD_SID(unit, member, dfrag->gtype);
    op_info.wsize = BCMECMP_TBL_ENT_SIZE(unit, member, dfrag->gtype) * ent_num;
    op_info.dyn_info.index = ent_idx;
    op_info.ecount = ent_num;
    op_info.dma = dma_en;

    SHR_IF_ERR_EXIT
        (bcm56990_a0_ecmp_pt_read(unit, &op_info, buf));

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56990_a0_ecmp_dfrag_grp_write(int unit, bcmecmp_grp_defrag_t *dfrag,
                                 uint32_t *buf)
{
    bcmecmp_pt_op_info_t op_info;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(dfrag, SHR_E_PARAM);
    SHR_NULL_CHECK(buf, SHR_E_PARAM);

    bcm56990_a0_ecmp_pt_op_info_t_init(&op_info);
    op_info.op = BCMPTM_OP_WRITE;
    op_info.op_arg = dfrag->op_arg;
    op_info.req_lt_sid = dfrag->glt_sid;
    op_info.drd_sid = BCMECMP_TBL_DRD_SID(unit, group, dfrag->gtype);
    op_info.wsize = BCMECMP_TBL_ENT_SIZE(unit, group, dfrag->gtype);
    op_info.dyn_info.index = dfrag->ecmp_id;

    SHR_IF_ERR_EXIT
        (bcm56990_a0_ecmp_pt_write(unit, &op_info, buf));

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56990_a0_ecmp_dfrag_grp_read(int unit, bcmecmp_grp_defrag_t *dfrag,
                                uint32_t *buf)
{
    bcmecmp_pt_op_info_t op_info;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(dfrag, SHR_E_PARAM);
    SHR_NULL_CHECK(buf, SHR_E_PARAM);

    bcm56990_a0_ecmp_pt_op_info_t_init(&op_info);
    op_info.op = BCMPTM_OP_READ;
    op_info.op_arg = dfrag->op_arg;
    op_info.req_lt_sid = dfrag->glt_sid;
    op_info.drd_sid = BCMECMP_TBL_DRD_SID(unit, group, dfrag->gtype);
    op_info.wsize = BCMECMP_TBL_ENT_SIZE(unit, group, dfrag->gtype);
    op_info.dyn_info.index = dfrag->ecmp_id;

    SHR_IF_ERR_EXIT
        (bcm56990_a0_ecmp_pt_read(unit, &op_info, buf));

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56990_a0_ecmp_info_print(int unit)
{
    shr_pb_t *pb; /* Print buffer. */

    SHR_FUNC_ENTER(unit);

    pb = shr_pb_create();
    shr_pb_printf(pb,
                  "\n\tECMP INFO - (Unit = %d, ECMP_MODE = %s):\n"
                  "\tECMP_CONFIG (Dev)            : wsize = %d\n",
                  unit,
                  ((BCMECMP_MODE(unit) == BCMECMP_MODE_FULL) ? "FULL" :
                   "INVALID"),
                   BCMECMP_REG_ENT_SIZE(unit, config));
    switch (BCMECMP_MODE(unit)) {
        case BCMECMP_MODE_FULL:
            shr_pb_printf(pb,
                          "\n\t============== Single ==============\n"
                          "\tINITIAL_L3_ECMP_GROUPm (Grp) : wsize = %d\n"
                          "\tL3_ECMP_COUNTm (Grp)         : wsize = %d\n"
                          "\tINITIAL_L3_ECMPm (Memb)      : wsize = %d\n"
                          "\tL3_ECMPm (Memb)              : wsize = %d\n"
                          "\tL3_ECMP_RRLB_CNTm            : wsize = %d\n"
                          "\tECMP Tbls max entry sz       : wsize = %d\n"
                          "\tGrp  : idx_min = %d, idx_max = %d, tbl_sz = %d\n"
                          "\tMemb : idx_min = %d, idx_max = %d, tbl_sz = %d\n"
                          "\t\tECMP_ID      : min = %d, max = %d\n"
                          "\t\tLB_MODE      : min = %d, max = %d\n"
                          "\t\tMAX_PATHS    : min = %d, max = %d\n"
                          "\t\tNUM_PATHS    : min = %d, max = %d\n"
                          "\t\tNHOP_ID      : min = %d, max = %d\n"
                          "\t\tRH_SIZE      : min = %d, max = %d\n",
                          BCMECMP_TBL_ENT_SIZE(unit, init_group,
                                               BCMECMP_GRP_TYPE_SINGLE),
                          BCMECMP_TBL_ENT_SIZE(unit, group,
                                               BCMECMP_GRP_TYPE_SINGLE),
                          BCMECMP_TBL_ENT_SIZE(unit, init_member,
                                               BCMECMP_GRP_TYPE_SINGLE),
                          BCMECMP_TBL_ENT_SIZE(unit, member,
                                               BCMECMP_GRP_TYPE_SINGLE),
                          BCMECMP_TBL_ENT_SIZE(unit, rrlb_cnt,
                                               BCMECMP_GRP_TYPE_SINGLE),
                          BCMECMP_TBL_ENT_SIZE_MAX(unit),
                          BCMECMP_TBL_IDX_MIN(unit, group,
                                              BCMECMP_GRP_TYPE_SINGLE),
                          BCMECMP_TBL_IDX_MAX(unit, group,
                                              BCMECMP_GRP_TYPE_SINGLE),
                          BCMECMP_TBL_SIZE(unit, group,
                                           BCMECMP_GRP_TYPE_SINGLE),
                          BCMECMP_TBL_IDX_MIN(unit, member,
                                              BCMECMP_GRP_TYPE_SINGLE),
                          BCMECMP_TBL_IDX_MAX(unit, member,
                                              BCMECMP_GRP_TYPE_SINGLE),
                          BCMECMP_TBL_SIZE(unit, member,
                                           BCMECMP_GRP_TYPE_SINGLE),
                          BCMECMP_LT_MIN_ECMP_ID(unit, group,
                                                 BCMECMP_GRP_TYPE_SINGLE),
                          BCMECMP_LT_MAX_ECMP_ID(unit, group,
                                                 BCMECMP_GRP_TYPE_SINGLE),
                          BCMECMP_LT_MIN_LB_MODE(unit, group,
                                                 BCMECMP_GRP_TYPE_SINGLE),
                          BCMECMP_LT_MAX_LB_MODE(unit, group,
                                                 BCMECMP_GRP_TYPE_SINGLE),
                          BCMECMP_LT_MIN_PATHS(unit, group,
                                               BCMECMP_GRP_TYPE_SINGLE),
                          BCMECMP_LT_MAX_PATHS(unit, group,
                                               BCMECMP_GRP_TYPE_SINGLE),
                          BCMECMP_LT_MIN_NPATHS(unit, group,
                                                BCMECMP_GRP_TYPE_SINGLE),
                          BCMECMP_LT_MAX_NPATHS(unit, group,
                                                BCMECMP_GRP_TYPE_SINGLE),
                          BCMECMP_LT_MIN_NHOP_ID(unit, group,
                                                 BCMECMP_GRP_TYPE_SINGLE),
                          BCMECMP_LT_MAX_NHOP_ID(unit, group,
                                                 BCMECMP_GRP_TYPE_SINGLE),
                          BCMECMP_LT_MIN_RH_SIZE(unit, group,
                                                 BCMECMP_GRP_TYPE_SINGLE),
                          BCMECMP_LT_MAX_RH_SIZE(unit, group,
                                                 BCMECMP_GRP_TYPE_SINGLE));
            break;
        default:
            break;
    }
    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "%s"), shr_pb_str(pb)));
    shr_pb_destroy(pb);

    SHR_FUNC_EXIT();
}

static int
bcm56990_a0_ecmp_sids_init(int unit)
{
    SHR_FUNC_ENTER(unit);

    /* Initialize device ECMP config register DRD and LTD SID values. */
    BCMECMP_REG_DRD_SID(unit, config) = BCMLTD_SID_INVALID;
    BCMECMP_REG_LTD_SID(unit, config) = BCMLTD_SID_INVALID;

    BCMECMP_TBL_DRD_SID
        (unit, init_group, BCMECMP_GRP_TYPE_WEIGHTED_0) = INVALIDm;
    BCMECMP_TBL_DRD_SID
        (unit, group, BCMECMP_GRP_TYPE_WEIGHTED_0) = INVALIDm;
    BCMECMP_TBL_LTD_SID
        (unit, init_group, BCMECMP_GRP_TYPE_WEIGHTED_0) = INVALIDm;
    BCMECMP_TBL_LTD_SID
        (unit, group, BCMECMP_GRP_TYPE_WEIGHTED_0) = INVALIDm;

    /* Initialize ECMP_MODE specific DRD + LTD SID values. */
    switch (BCMECMP_MODE(unit)) {
        case BCMECMP_MODE_FULL:
            /* Initialize Full-Flat ECMP resolution tables DRD SID values. */
            BCMECMP_TBL_DRD_SID
                (unit, group, BCMECMP_GRP_TYPE_SINGLE) = L3_ECMP_COUNTm;

            /* Initialize Full-Flat ECMP member table DRD_SID value. */
            BCMECMP_TBL_DRD_SID
                (unit, member, BCMECMP_GRP_TYPE_SINGLE) = L3_ECMPm;

            /* Initialize Full-Flat ECMP logical table SID value. */
            BCMECMP_TBL_LTD_SID
                (unit, group, BCMECMP_GRP_TYPE_SINGLE) = ECMPt;

             /* Initialize weighted ECMP resolution tables DRD SID values. */
            BCMECMP_TBL_DRD_SID
                (unit, group, BCMECMP_GRP_TYPE_WEIGHTED) = L3_ECMP_COUNTm;

            /* Initialize weighted ECMP member table DRD_SID value. */
            BCMECMP_TBL_DRD_SID
                (unit, member, BCMECMP_GRP_TYPE_WEIGHTED) = L3_ECMPm;

            /* Initialize weighted ECMP logical table SID value. */
            BCMECMP_TBL_LTD_SID
                (unit, group, BCMECMP_GRP_TYPE_WEIGHTED) = ECMP_WEIGHTEDt;

            /* Overlay and Underlay LTD SIDs are invalid for Full ECMP mode. */
            BCMECMP_TBL_LTD_SID
                (unit, group, BCMECMP_GRP_TYPE_OVERLAY) = BCMLTD_SID_INVALID;
            BCMECMP_TBL_LTD_SID
                (unit, group, BCMECMP_GRP_TYPE_UNDERLAY) = BCMLTD_SID_INVALID;

            /*
             * Pre-IFP ECMP stage is removed in TH4 device hence initialize
             * DRD_SID value for related ECMP tables as invalid.
             */
            BCMECMP_TBL_DRD_SID
                (unit, init_group, BCMECMP_GRP_TYPE_SINGLE) = INVALIDm;
            BCMECMP_TBL_DRD_SID
                (unit, init_member, BCMECMP_GRP_TYPE_SINGLE) = INVALIDm;

            /*
             * TH4 device does not support round-robin load balancing mode,
             * initialize related memory's DRD_SID value as invalid.
             */
            BCMECMP_TBL_DRD_SID
                (unit, rrlb_cnt, BCMECMP_GRP_TYPE_SINGLE) = INVALIDm;
            break;

        default:
            SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56990_a0_ecmp_sids_clear(int unit)
{
    SHR_FUNC_ENTER(unit);

    /* Clear device ECMP config register DRD and LTD SID values. */
    BCMECMP_REG_DRD_SID(unit, config) = INVALIDr;
    BCMECMP_REG_LTD_SID(unit, config) = BCMLTD_SID_INVALID;

    switch (BCMECMP_MODE(unit)) {
        case BCMECMP_MODE_FULL:
            /* Clear DRD-SID information for single level groups. */
            BCMECMP_TBL_DRD_SID
                (unit, group, BCMECMP_GRP_TYPE_SINGLE) = INVALIDm;
            BCMECMP_TBL_DRD_SID
                (unit, member, BCMECMP_GRP_TYPE_SINGLE) = INVALIDm;

            BCMECMP_TBL_DRD_SID
                (unit, group, BCMECMP_GRP_TYPE_WEIGHTED) = INVALIDm;
            BCMECMP_TBL_DRD_SID
                (unit, member, BCMECMP_GRP_TYPE_WEIGHTED) = INVALIDm;

            /*
             * Clear ECMP Group table LTD-SID information for single level
             * groups.
             */
            BCMECMP_TBL_LTD_SID
                (unit, group, BCMECMP_GRP_TYPE_SINGLE) = BCMLTD_SID_INVALID;

            BCMECMP_TBL_LTD_SID
                (unit, group, BCMECMP_GRP_TYPE_WEIGHTED) = BCMLTD_SID_INVALID;

            /* TH4 device does not support Pre-IFP ECMP tables. */
            BCMECMP_TBL_DRD_SID
                (unit, init_group, BCMECMP_GRP_TYPE_SINGLE) = INVALIDm;
            BCMECMP_TBL_DRD_SID
                (unit, init_member, BCMECMP_GRP_TYPE_SINGLE) = INVALIDm;
            BCMECMP_TBL_DRD_SID
                (unit, rrlb_cnt, BCMECMP_GRP_TYPE_SINGLE) = INVALIDm;
            break;

        default:
            SHR_EXIT();
    }

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56990_a0_ecmp_hw_tbls_range_init(int unit)
{
    uint32_t wsize; /* Hardware table entry size in words. */
    int memb_imin;  /* ECMP Member hardware table Min index value. */
    int memb_imax;  /* ECMP Member hardware table Max index value. */
    int grp_imin;   /* ECMP Group hardware table Min index value. */
    int grp_imax;   /* ECMP Group hardware table Max index value. */
    int memb_size;

    SHR_FUNC_ENTER(unit);

    /* TH4 device does not support ECMP config register. */
    BCMECMP_REG_ENT_SIZE(unit, config) = 0;

    /* Initialize table information based on the device ECMP mode. */
    switch (BCMECMP_MODE(unit)) {
        case BCMECMP_MODE_FULL:
            /*
             * TH4 device does not support Pre-IFP ECMP group and member
             * tables hence initialize entry size as zero for these tables for
             * this device.
             */
            BCMECMP_TBL_ENT_SIZE
                (unit, init_group, BCMECMP_GRP_TYPE_SINGLE) = 0;
            BCMECMP_TBL_ENT_SIZE
                (unit, init_member, BCMECMP_GRP_TYPE_SINGLE) = 0;

            BCMECMP_TBL_ENT_SIZE_MAX(unit) = 0;

            /*
             * Get Post-IFP ECMP group table HW entry size (in words) from
             * DRD.
             */
            wsize = bcmdrd_pt_entry_wsize(unit,
                                          BCMECMP_TBL_DRD_SID(unit, group,
                                              BCMECMP_GRP_TYPE_SINGLE));
            BCMECMP_TBL_ENT_SIZE
                (unit, group, BCMECMP_GRP_TYPE_SINGLE) = wsize;
            if (wsize > BCMECMP_TBL_ENT_SIZE_MAX(unit)) {
                BCMECMP_TBL_ENT_SIZE_MAX(unit) = wsize;
            }

            BCMECMP_TBL_ENT_SIZE
                (unit, group,
                 BCMECMP_GRP_TYPE_WEIGHTED) = BCMECMP_TBL_ENT_SIZE
                                                  (unit, group,
                                                   BCMECMP_GRP_TYPE_SINGLE);

            /*
             * Get Post-IFP ECMP member table HW entry size (in words) from
             * DRD.
             */
            wsize = bcmdrd_pt_entry_wsize(unit,
                                          BCMECMP_TBL_DRD_SID(unit, member,
                                              BCMECMP_GRP_TYPE_SINGLE));
            BCMECMP_TBL_ENT_SIZE
                (unit, member, BCMECMP_GRP_TYPE_SINGLE) = wsize;
            if (wsize > BCMECMP_TBL_ENT_SIZE_MAX(unit)) {
                BCMECMP_TBL_ENT_SIZE_MAX(unit) = wsize;
            }

            BCMECMP_TBL_ENT_SIZE
                (unit, member,
                 BCMECMP_GRP_TYPE_WEIGHTED) = BCMECMP_TBL_ENT_SIZE
                                                  (unit, member,
                                                   BCMECMP_GRP_TYPE_SINGLE);

            /*
             * TH4 device does not support round-robin load balancing mode and
             * related tables. Initialize entry size for this table as zero in
             * SW database.
             */
            BCMECMP_TBL_ENT_SIZE(unit, rrlb_cnt, BCMECMP_GRP_TYPE_SINGLE) = 0;

            /* Get ECMP Member HW table Min and Max index ranges from DRD. */
            memb_imin = bcmdrd_pt_index_min(unit,
                                            BCMECMP_TBL_DRD_SID(unit, member,
                                                BCMECMP_GRP_TYPE_SINGLE));
            memb_imax = bcmdrd_pt_index_max(unit,
                                            BCMECMP_TBL_DRD_SID(unit, member,
                                                BCMECMP_GRP_TYPE_SINGLE));

            /* Get ECMP Group HW table Min and Max index ranges from DRD. */
            grp_imin = bcmdrd_pt_index_min
                           (unit, BCMECMP_TBL_DRD_SID(unit, group,
                                                      BCMECMP_GRP_TYPE_SINGLE));
            grp_imax = bcmdrd_pt_index_max
                           (unit, BCMECMP_TBL_DRD_SID(unit, group,
                                                      BCMECMP_GRP_TYPE_SINGLE));

            /*
             * ECMP Member HW Table Min & Max index, Size and Min-index used
             * status initialization.
             */
            memb_size = (memb_imax + 1) * NHOPS_PER_MEMB_ENTRY;
            /*memb_imax = memb_size - 1;*/
            /* Use the pt imin/imax directly for ITBM */
            BCMECMP_TBL_IDX_MIN
                (unit, member, BCMECMP_GRP_TYPE_SINGLE) = memb_imin;
            BCMECMP_TBL_IDX_MAX
                (unit, member, BCMECMP_GRP_TYPE_SINGLE) = memb_imax;
            BCMECMP_TBL_SIZE
                (unit, member,
                 BCMECMP_GRP_TYPE_SINGLE) = memb_size;
            BCMECMP_TBL_IDX_MAX_USED
                (unit, member, BCMECMP_GRP_TYPE_SINGLE) = memb_imin;

            memb_size = (memb_imax + 1) * NHOPS_PER_MEMB_ENTRY_WECMP;
            BCMECMP_TBL_IDX_MIN
                (unit, member, BCMECMP_GRP_TYPE_WEIGHTED) = memb_imin;
            BCMECMP_TBL_IDX_MAX
                (unit, member, BCMECMP_GRP_TYPE_WEIGHTED) = memb_imax;
            BCMECMP_TBL_SIZE
                (unit, member,
                 BCMECMP_GRP_TYPE_WEIGHTED) = memb_size;
            BCMECMP_TBL_IDX_MAX_USED
                (unit, member, BCMECMP_GRP_TYPE_WEIGHTED) = memb_imin;

            /*
             * ECMP Group HW Table Min & Max index, Size and Min-index used
             * status initialization.
             */
            BCMECMP_TBL_IDX_MIN
                (unit, group, BCMECMP_GRP_TYPE_SINGLE) = grp_imin;
            BCMECMP_TBL_IDX_MAX
                (unit, group, BCMECMP_GRP_TYPE_SINGLE) = grp_imax;
            BCMECMP_TBL_SIZE
                (unit, group,
                 BCMECMP_GRP_TYPE_SINGLE) = BCMECMP_TBL_IDX_MAX
                                                (unit, group,
                                                 BCMECMP_GRP_TYPE_SINGLE) + 1;
            BCMECMP_TBL_IDX_MAX_USED
                (unit, group, BCMECMP_GRP_TYPE_SINGLE) = grp_imin;

            BCMECMP_TBL_IDX_MIN
                (unit, group, BCMECMP_GRP_TYPE_WEIGHTED) = grp_imin;
            BCMECMP_TBL_IDX_MAX
                (unit, group, BCMECMP_GRP_TYPE_WEIGHTED) = grp_imax;
            BCMECMP_TBL_SIZE
                (unit, group,
                 BCMECMP_GRP_TYPE_WEIGHTED) = BCMECMP_TBL_IDX_MAX
                                                (unit, group,
                                                 BCMECMP_GRP_TYPE_WEIGHTED) + 1;
            BCMECMP_TBL_IDX_MAX_USED
                (unit, group, BCMECMP_GRP_TYPE_WEIGHTED) = grp_imin;
            break;

        default:
            SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56990_a0_ecmp_hw_tbls_range_clear(int unit)
{
    SHR_FUNC_ENTER(unit);

    /* Clear device ECMP config register entry size in words value. */
    BCMECMP_REG_ENT_SIZE(unit, config) = 0;

    /* Clear Max hardware tables entry size info. */
    BCMECMP_TBL_ENT_SIZE_MAX(unit) = 0;

    switch (BCMECMP_MODE(unit)) {
        case BCMECMP_MODE_FULL:
            /* Clear Initial Group Table entry size in words from DRD. */
            BCMECMP_TBL_ENT_SIZE
                (unit, init_group, BCMECMP_GRP_TYPE_SINGLE) = 0;
            BCMECMP_TBL_ENT_SIZE
                (unit, init_member, BCMECMP_GRP_TYPE_SINGLE) = 0;
            BCMECMP_TBL_ENT_SIZE(unit, group, BCMECMP_GRP_TYPE_SINGLE) = 0;
            BCMECMP_TBL_ENT_SIZE(unit, member, BCMECMP_GRP_TYPE_SINGLE) = 0;
            BCMECMP_TBL_ENT_SIZE(unit, rrlb_cnt, BCMECMP_GRP_TYPE_SINGLE) = 0;
            BCMECMP_TBL_ENT_SIZE(unit, group, BCMECMP_GRP_TYPE_WEIGHTED) = 0;
            BCMECMP_TBL_ENT_SIZE(unit, member, BCMECMP_GRP_TYPE_WEIGHTED) = 0;

            /* Clear ECMP Member HW Table indicies information. */
            BCMECMP_TBL_IDX_MIN
                (unit, member, BCMECMP_GRP_TYPE_SINGLE) = BCMECMP_INVALID;
            BCMECMP_TBL_IDX_MAX
                (unit, member, BCMECMP_GRP_TYPE_SINGLE) = BCMECMP_INVALID;
            BCMECMP_TBL_IDX_MAX_USED
                (unit, member, BCMECMP_GRP_TYPE_SINGLE) = BCMECMP_INVALID;
            BCMECMP_TBL_SIZE(unit, member, BCMECMP_GRP_TYPE_SINGLE) = 0;

            /* Clear ECMP Group HW Table indicies range information. */
            BCMECMP_TBL_IDX_MIN
                (unit, group, BCMECMP_GRP_TYPE_SINGLE) = BCMECMP_INVALID;
            BCMECMP_TBL_IDX_MAX
                (unit, group, BCMECMP_GRP_TYPE_SINGLE) = BCMECMP_INVALID;
            BCMECMP_TBL_IDX_MAX_USED
                (unit, group, BCMECMP_GRP_TYPE_SINGLE) = BCMECMP_INVALID;
            BCMECMP_TBL_SIZE(unit, group, BCMECMP_GRP_TYPE_SINGLE) = 0;

            /* Clear weighted ECMP Member HW Table indicies information. */
            BCMECMP_TBL_IDX_MIN
                (unit, member, BCMECMP_GRP_TYPE_WEIGHTED) = BCMECMP_INVALID;
            BCMECMP_TBL_IDX_MAX
                (unit, member, BCMECMP_GRP_TYPE_WEIGHTED) = BCMECMP_INVALID;
            BCMECMP_TBL_IDX_MAX_USED
                (unit, member, BCMECMP_GRP_TYPE_WEIGHTED) = BCMECMP_INVALID;
            BCMECMP_TBL_SIZE(unit, member, BCMECMP_GRP_TYPE_WEIGHTED) = 0;

            /* Clear weighted ECMP Group HW Table indicies range information. */
            BCMECMP_TBL_IDX_MIN
                (unit, group, BCMECMP_GRP_TYPE_WEIGHTED) = BCMECMP_INVALID;
            BCMECMP_TBL_IDX_MAX
                (unit, group, BCMECMP_GRP_TYPE_WEIGHTED) = BCMECMP_INVALID;
            BCMECMP_TBL_IDX_MAX_USED
                (unit, group, BCMECMP_GRP_TYPE_WEIGHTED) = BCMECMP_INVALID;
            BCMECMP_TBL_SIZE(unit, group, BCMECMP_GRP_TYPE_WEIGHTED) = 0;
            break;

        default:
            SHR_EXIT();
    }

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56990_a0_ecmp_single_lt_flds_init(int unit,
                                     const bcmlrd_map_t *map)
{
    bcm56990_a0_ecmp_grp_drd_fields_t *gdrd_fields = NULL;  /* Group tbl DRD info. */
    bcm56990_a0_ecmp_memb_drd_fields_t *mdrd_fields = NULL; /* Member tbl DRD info. */
    bcmecmp_grp_ltd_fields_t *lt_fids = NULL;   /* LT entry field IDs. */
    bcmecmp_lt_field_attrs_t *grp_attrs = NULL; /* Single LT field attrs. */
    uint32_t idx;    /* Enum symbol array index. */
    uint32_t lb_val; /* LB mode value. */
    const bcmlrd_field_data_t *fdata = NULL; /* LT field data map. */
    const char *const lb_name[BCMECMP_LB_MODE_COUNT] = BCMECMP_LB_MODE_NAME;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(map, SHR_E_INTERNAL);

    /* Allocate memory for storing Member table entry fields DRD ID values. */
    BCMECMP_ALLOC(mdrd_fields, sizeof(*mdrd_fields), "bcmecmpSingleMdrdFlds");
    BCMECMP_TBL_DRD_FIELDS
        (unit, member, BCMECMP_GRP_TYPE_SINGLE) = (void *)mdrd_fields;
    /* TH4 device does not support Pre-IFP ECMP tables. */
    BCMECMP_TBL_DRD_FIELDS(unit, init_member, BCMECMP_GRP_TYPE_SINGLE) = NULL;

    /* Initialize Member table entry fields DRD ID values. */
    bcm56990_a0_ecmp_memb_drd_fields_t_init(mdrd_fields);
    mdrd_fields->entry_type = ENTRY_TYPEf;
    mdrd_fields->wecmp_weight = WECMPv_WEIGHTf;
    mdrd_fields->wecmp_nhop_id[0] = WECMPv_NEXT_HOP_INDEX_Af;
    mdrd_fields->wecmp_nhop_id[1] = WECMPv_NEXT_HOP_INDEX_Bf;
    mdrd_fields->ecmp_nhop_id[0] = ECMPv_NEXT_HOP_INDEX_0f;
    mdrd_fields->ecmp_nhop_id[1] = ECMPv_NEXT_HOP_INDEX_1f;
    mdrd_fields->ecmp_nhop_id[2] = ECMPv_NEXT_HOP_INDEX_2f;
    mdrd_fields->ecmp_nhop_id[3] = ECMPv_NEXT_HOP_INDEX_3f;

    /* Allocate memory for storing Group table entry fields DRD ID values. */
    BCMECMP_ALLOC(gdrd_fields, sizeof(*gdrd_fields), "bcmecmpSingleGdrdFlds");
    BCMECMP_TBL_DRD_FIELDS
        (unit, group, BCMECMP_GRP_TYPE_SINGLE) = (void *)gdrd_fields;
    /* TH4 device does not support Pre-IFP ECMP tables. */
    BCMECMP_TBL_DRD_FIELDS(unit, init_group, BCMECMP_GRP_TYPE_SINGLE) = NULL;

    /* Initialize Group table entry fields DRD ID values. */
    bcm56990_a0_ecmp_grp_drd_fields_t_init(gdrd_fields);
    gdrd_fields->count = COUNTf;
    gdrd_fields->base_ptr = BASE_PTRf;
    gdrd_fields->lb_mode = LB_MODEf;
    gdrd_fields->flex_ctr_object = FLEX_CTR_OBJECTf;
    gdrd_fields->flex_ctr_action_sel = FLEX_CTR_ACTION_SELf;

    /*
     * Allocate memory for storing ECMP LT logical
     * fields Min, Max attribute values.
     */
    BCMECMP_ALLOC(grp_attrs, sizeof(*grp_attrs), "bcmecmpSingleGrpAttrs");
    BCMECMP_LT_FIELD_ATTRS_VOID_PTR
        (unit, group, BCMECMP_GRP_TYPE_SINGLE) = (void *)grp_attrs;
    bcm56990_a0_ecmp_lt_field_attrs_t_init(grp_attrs);

    /*
     * Initialize Group logical table field identifer values based on the
     * device ECMP mode.
     */
    BCMECMP_ALLOC(lt_fids, sizeof(*lt_fids), "bcmecmpSingleGrpLtdFids");
    BCMECMP_TBL_LTD_FIELDS
        (unit, group, BCMECMP_GRP_TYPE_SINGLE) = (void *)lt_fids;

    /*
     * Initialize Logical Table LTD field identifiers values for
     * single lookup ECMP Group LT logical fields.
     */
    bcm56990_a0_ecmp_grp_ltd_fields_t_init(lt_fids);
    lt_fids->ecmp_id_fid = ECMPt_ECMP_IDf;
    lt_fids->lb_mode_fid = ECMPt_LB_MODEf;
    lt_fids->nhop_sorted_fid = ECMPt_NHOP_SORTEDf;
    lt_fids->max_paths_fid = ECMPt_MAX_PATHSf;
    lt_fids->num_paths_fid = ECMPt_NUM_PATHSf;
    lt_fids->nhop_id_fid = ECMPt_NHOP_IDf;
    lt_fids->rh_size_fid = ECMPt_RH_SIZEf;
    lt_fids->rh_num_paths_fid = ECMPt_RH_NUM_PATHSf;
    lt_fids->ctr_ing_eflex_action_id_fid = ECMPt_ECMP_CTR_ING_EFLEX_ACTION_IDf;
    lt_fids->ctr_ing_eflex_object_fid = ECMPt_CTR_ING_EFLEX_OBJECTf;

    /* Get ECMP ID min and max values from LRD field data. */
    fdata = &(map->field_data->field[lt_fids->ecmp_id_fid]);
    BCMECMP_LT_MIN_ECMP_ID
        (unit, group, BCMECMP_GRP_TYPE_SINGLE) = fdata->min->u16;
    BCMECMP_LT_MAX_ECMP_ID
        (unit, group, BCMECMP_GRP_TYPE_SINGLE) = fdata->max->u16;

    /*
     * Get load balancing mode min and max values from LRD field
     * data.
     */
    fdata = &(map->field_data->field[lt_fids->lb_mode_fid]);
    BCMECMP_LT_MIN_LB_MODE
        (unit, group, BCMECMP_GRP_TYPE_SINGLE) = fdata->min->u32;
    BCMECMP_LT_MAX_LB_MODE
        (unit, group, BCMECMP_GRP_TYPE_SINGLE) = fdata->max->u32;

    /*
     * Iterate over lb_mode enum symbols, get enum values and store in
     * SW database.
     */
    for (idx = 0; idx < fdata->edata->num_sym; idx++) {
        /* Get LB_MODE enum value. */
        lb_val = (uint32_t)fdata->edata->sym[idx].val;

        /*
         * Check and confirm LB_MODE enum name matches expected name for
         * this LB_MODE enum value. Return error and exit
         * initialization if invalid/unrecognized LB_MODE enum name is
         * found.
         */
        if (!sal_strcmp(lb_name[lb_val], fdata->edata->sym[idx].name)) {
            /*
             * Initialize supported LB modes for single level ECMP
             * LT.
             */
            BCMECMP_LT_LB_MODE
                (unit, group, BCMECMP_GRP_TYPE_SINGLE, lb_val) = lb_val;
        } else {
            SHR_ERR_EXIT(SHR_E_INTERNAL);
        }
    }

    /*
     * It's not mandatory that MAX_PATHS and NUM_PATHS values must be
     * in powers-of-two for entries in ECMP LT for this device.
     */
    BCMECMP_LT_POW_TWO_PATHS
        (unit, group, BCMECMP_GRP_TYPE_SINGLE) = FALSE;

    /*
     * Get MAX_PATHS logical table LRD field data pointer.
     * Initialize device max_paths minimum and maximum values.
     */
    fdata = &(map->field_data->field[lt_fids->max_paths_fid]);
    BCMECMP_LT_MIN_PATHS
        (unit, group, BCMECMP_GRP_TYPE_SINGLE) = fdata->min->u16;
    BCMECMP_LT_MAX_PATHS
        (unit, group, BCMECMP_GRP_TYPE_SINGLE) = fdata->max->u16;

    /*
     * Get NUM_PATHS logical table LRD field data pointer.
     * Initialize device num_paths minimum and maximum values.
     */
    fdata = &(map->field_data->field[lt_fids->num_paths_fid]);
    BCMECMP_LT_MIN_NPATHS
        (unit, group, BCMECMP_GRP_TYPE_SINGLE) = fdata->min->u16;
    BCMECMP_LT_MAX_NPATHS
        (unit, group, BCMECMP_GRP_TYPE_SINGLE) = fdata->max->u16;

    /*
     * Get NHOP_ID logical table LRD field data pointer.
     * Initialize device NHOP_ID minimum and maximum values.
     */
    fdata = &(map->field_data->field[lt_fids->nhop_id_fid]);
    BCMECMP_LT_MIN_NHOP_ID
        (unit, group, BCMECMP_GRP_TYPE_SINGLE) = fdata->min->u16;
    BCMECMP_LT_MAX_NHOP_ID
        (unit, group, BCMECMP_GRP_TYPE_SINGLE) = fdata->max->u16;

    /*
     * Get rh_size logical table LRD field data pointer.
     * Initialize device rh_size minimum and maximum values.
     */
    fdata = &(map->field_data->field[lt_fids->rh_size_fid]);
    BCMECMP_LT_MIN_RH_SIZE
        (unit, group, BCMECMP_GRP_TYPE_SINGLE) = fdata->min->u32;
    BCMECMP_LT_MAX_RH_SIZE
        (unit, group, BCMECMP_GRP_TYPE_SINGLE) = fdata->max->u32;

    /*
     * Get RH_NUM_PATHS logical table LRD field data pointer.
     * Initialize device rh_num_paths minimum and maximum values.
     */
    fdata = &(map->field_data->field[lt_fids->rh_num_paths_fid]);
    BCMECMP_LT_MIN_RH_NPATHS
        (unit, group, BCMECMP_GRP_TYPE_SINGLE) = fdata->min->u16;
    BCMECMP_LT_MAX_RH_NPATHS
        (unit, group, BCMECMP_GRP_TYPE_SINGLE) = fdata->max->u16;

    fdata = &(map->field_data->field[lt_fids->ctr_ing_eflex_action_id_fid]);
    BCMECMP_LT_MIN_CTR_ING_EFLEX_ACTION_ID
        (unit, group, BCMECMP_GRP_TYPE_SINGLE) = fdata->min->u16;
    BCMECMP_LT_MAX_CTR_ING_EFLEX_ACTION_ID
        (unit, group, BCMECMP_GRP_TYPE_SINGLE) = fdata->max->u16;

    fdata = &(map->field_data->field[lt_fids->ctr_ing_eflex_object_fid]);
    BCMECMP_LT_MIN_CTR_ING_EFLEX_OBJECT
        (unit, group, BCMECMP_GRP_TYPE_SINGLE) = fdata->min->u16;
    BCMECMP_LT_MAX_CTR_ING_EFLEX_OBJECT
        (unit, group, BCMECMP_GRP_TYPE_SINGLE) = fdata->max->u16;

exit:
    if (SHR_FUNC_ERR()) {
        BCMECMP_FREE(gdrd_fields);
        BCMECMP_FREE(mdrd_fields);
        BCMECMP_FREE(lt_fids);
        BCMECMP_FREE(grp_attrs);
    }

    SHR_FUNC_EXIT();
}

static int
bcm56990_a0_ecmp_single_grps_flds_cleanup(int unit)
{
    bcm56990_a0_ecmp_grp_drd_fields_t *gdrd_fields = NULL;  /* Group tbl DRD info. */
    bcm56990_a0_ecmp_memb_drd_fields_t *mdrd_fields = NULL; /* Member tbl DRD info. */
    bcmecmp_grp_ltd_fields_t *lt_fids = NULL;   /* LT entry fields LTD ID. */
    bcmecmp_lt_field_attrs_t *grp_attrs = NULL; /* Single LT field attrs. */

    SHR_FUNC_ENTER(unit);

    /* Free memory allocated for storing Group tables DRD fields data. */
    gdrd_fields = (bcm56990_a0_ecmp_grp_drd_fields_t *)BCMECMP_TBL_DRD_FIELDS
                                                           (unit, group,
                                                            BCMECMP_GRP_TYPE_SINGLE);
    BCMECMP_FREE(gdrd_fields);

    /* Free memory allocated for storing Member tables DRD fields data. */
    mdrd_fields = (bcm56990_a0_ecmp_memb_drd_fields_t *)BCMECMP_TBL_DRD_FIELDS
                                                            (unit, member,
                                                             BCMECMP_GRP_TYPE_SINGLE);
    BCMECMP_FREE(mdrd_fields);

    /* Free memory allocated for storing ECMP LT logical fields data. */
    lt_fids = (bcmecmp_grp_ltd_fields_t *)BCMECMP_TBL_LTD_FIELDS
                                                (unit, group,
                                                 BCMECMP_GRP_TYPE_SINGLE);
    BCMECMP_FREE(lt_fids);

    /*
     * Free memory allocated for storing ECMP LT
     * logical fields attributes information.
     */
    grp_attrs = (bcmecmp_lt_field_attrs_t *)BCMECMP_LT_FIELD_ATTRS_VOID_PTR
                                                (unit, group,
                                                 BCMECMP_GRP_TYPE_SINGLE);
    BCMECMP_FREE(grp_attrs);

    SHR_FUNC_EXIT();
}

static int
bcm56990_a0_ecmp_weighted_lt_flds_init(int unit,
                                       const bcmlrd_map_t *map)
{
    bcm56990_a0_ecmp_grp_drd_fields_t *gdrd_fields = NULL;  /* Group tbl DRD info. */
    bcm56990_a0_ecmp_memb_drd_fields_t *mdrd_fields = NULL; /* Member tbl DRD info. */
    bcmecmp_grp_ltd_fields_t *lt_fids = NULL;   /* LT entry field IDs. */
    bcmecmp_lt_field_attrs_t *grp_attrs = NULL; /* Single LT field attrs. */
    const bcmlrd_field_data_t *fdata = NULL; /* LT field data map. */

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(map, SHR_E_INTERNAL);

    /* Allocate memory for storing Member table entry fields DRD ID values. */
    BCMECMP_ALLOC(mdrd_fields, sizeof(*mdrd_fields), "bcmecmpSingleMdrdFlds");
    BCMECMP_TBL_DRD_FIELDS
        (unit, member, BCMECMP_GRP_TYPE_WEIGHTED) = (void *)mdrd_fields;

    /* Initialize Member table entry fields DRD ID values. */
    bcm56990_a0_ecmp_memb_drd_fields_t_init(mdrd_fields);
    mdrd_fields->entry_type = ENTRY_TYPEf;
    mdrd_fields->wecmp_weight = WECMPv_WEIGHTf;
    mdrd_fields->wecmp_nhop_id[0] = WECMPv_NEXT_HOP_INDEX_Af;
    mdrd_fields->wecmp_nhop_id[1] = WECMPv_NEXT_HOP_INDEX_Bf;
    mdrd_fields->ecmp_nhop_id[0] = ECMPv_NEXT_HOP_INDEX_0f;
    mdrd_fields->ecmp_nhop_id[1] = ECMPv_NEXT_HOP_INDEX_1f;
    mdrd_fields->ecmp_nhop_id[2] = ECMPv_NEXT_HOP_INDEX_2f;
    mdrd_fields->ecmp_nhop_id[3] = ECMPv_NEXT_HOP_INDEX_3f;

    /* Allocate memory for storing Group table entry fields DRD ID values. */
    BCMECMP_ALLOC(gdrd_fields, sizeof(*gdrd_fields), "bcmecmpSingleGdrdFlds");
    BCMECMP_TBL_DRD_FIELDS
        (unit, group, BCMECMP_GRP_TYPE_WEIGHTED) = (void *)gdrd_fields;

    /* Initialize Group table entry fields DRD ID values. */
    bcm56990_a0_ecmp_grp_drd_fields_t_init(gdrd_fields);
    gdrd_fields->count = COUNTf;
    gdrd_fields->base_ptr = BASE_PTRf;
    gdrd_fields->lb_mode = LB_MODEf;
    gdrd_fields->flex_ctr_object = FLEX_CTR_OBJECTf;
    gdrd_fields->flex_ctr_action_sel = FLEX_CTR_ACTION_SELf;

    /*
     * Allocate memory for storing ECMP LT logical
     * fields Min, Max attribute values.
     */
    BCMECMP_ALLOC(grp_attrs, sizeof(*grp_attrs), "bcmecmpSingleGrpAttrs");
    BCMECMP_LT_FIELD_ATTRS_VOID_PTR
        (unit, group, BCMECMP_GRP_TYPE_WEIGHTED) = (void *)grp_attrs;
    bcm56990_a0_ecmp_lt_field_attrs_t_init(grp_attrs);

    /*
     * Initialize Group logical table field identifier values based on the
     * device ECMP mode.
     */
    BCMECMP_ALLOC(lt_fids, sizeof(*lt_fids), "bcmecmpSingleGrpLtdFids");
    BCMECMP_TBL_LTD_FIELDS
        (unit, group, BCMECMP_GRP_TYPE_WEIGHTED) = (void *)lt_fids;

    /*
     * Initialize Logical Table LTD field identifiers values for
     * single lookup ECMP Group LT logical fields.
     */
    bcm56990_a0_ecmp_grp_ltd_fields_t_init(lt_fids);
    lt_fids->ecmp_id_fid = ECMP_WEIGHTEDt_ECMP_IDf;
    lt_fids->weighted_size_fid = ECMP_WEIGHTEDt_WEIGHTED_SIZEf;
    lt_fids->num_paths_fid = ECMP_WEIGHTEDt_NUM_PATHSf;
    lt_fids->nhop_id_fid = ECMP_WEIGHTEDt_NHOP_IDf;
    lt_fids->weighted_mode_fid = ECMP_WEIGHTEDt_WEIGHTED_MODEf;
    lt_fids->weight_fid = ECMP_WEIGHTEDt_WEIGHTf;
    lt_fids->ctr_ing_eflex_action_id_fid =
        ECMP_WEIGHTEDt_ECMP_CTR_ING_EFLEX_ACTION_IDf;
    lt_fids->ctr_ing_eflex_object_fid =
        ECMP_WEIGHTEDt_CTR_ING_EFLEX_OBJECTf;

    /* Get ECMP ID min and max values from LRD field data. */
    fdata = &(map->field_data->field[lt_fids->ecmp_id_fid]);
    BCMECMP_LT_MIN_ECMP_ID
        (unit, group, BCMECMP_GRP_TYPE_WEIGHTED) = fdata->min->u16;
    BCMECMP_LT_MAX_ECMP_ID
        (unit, group, BCMECMP_GRP_TYPE_WEIGHTED) = fdata->max->u16;

    /*
     * Get weighted ECMP group size min and max values from LRD field
     * data.
     */
    fdata = &(map->field_data->field[lt_fids->weighted_size_fid]);
    BCMECMP_LT_MIN_WEIGHTED_SIZE
        (unit, group, BCMECMP_GRP_TYPE_WEIGHTED) = fdata->min->u32;
    BCMECMP_LT_MAX_WEIGHTED_SIZE
        (unit, group, BCMECMP_GRP_TYPE_WEIGHTED) = fdata->max->u32;

    /*
     * Get weighted ECMP group mode min and max values from LRD field
     * data.
     */
    fdata = &(map->field_data->field[lt_fids->weighted_mode_fid]);
    BCMECMP_LT_MIN_WEIGHTED_MODE
        (unit, group, BCMECMP_GRP_TYPE_WEIGHTED) = fdata->min->u32;
    BCMECMP_LT_MAX_WEIGHTED_MODE
        (unit, group, BCMECMP_GRP_TYPE_WEIGHTED) = fdata->max->u32;

    /*
     * Get weighted ECMP group weight min and max values from LRD field
     * data.
     */
    fdata = &(map->field_data->field[lt_fids->weight_fid]);
    BCMECMP_LT_MIN_WEIGHT
        (unit, group, BCMECMP_GRP_TYPE_WEIGHTED) = fdata->min->u16;
    BCMECMP_LT_MAX_WEIGHT
        (unit, group, BCMECMP_GRP_TYPE_WEIGHTED) = fdata->max->u16;

    /*
     * Get NUM_PATHS logical table LRD field data pointer.
     * Initialize device num_paths minimum and maximum values.
     */
    fdata = &(map->field_data->field[lt_fids->num_paths_fid]);
    BCMECMP_LT_MIN_NPATHS
        (unit, group, BCMECMP_GRP_TYPE_WEIGHTED) = fdata->min->u16;
    BCMECMP_LT_MAX_NPATHS
        (unit, group, BCMECMP_GRP_TYPE_WEIGHTED) = fdata->max->u16;
    BCMECMP_LT_MIN_PATHS
        (unit, group, BCMECMP_GRP_TYPE_WEIGHTED) = 0;
    BCMECMP_LT_MAX_PATHS
        (unit, group, BCMECMP_GRP_TYPE_WEIGHTED) = fdata->max->u16;

    /*
     * Get NHOP_ID logical table LRD field data pointer.
     * Initialize device NHOP_ID minimum and maximum values.
     */
    fdata = &(map->field_data->field[lt_fids->nhop_id_fid]);
    BCMECMP_LT_MIN_NHOP_ID
        (unit, group, BCMECMP_GRP_TYPE_WEIGHTED) = fdata->min->u16;
    BCMECMP_LT_MAX_NHOP_ID
        (unit, group, BCMECMP_GRP_TYPE_WEIGHTED) = fdata->max->u16;

    BCMECMP_LT_POW_TWO_PATHS
        (unit, group, BCMECMP_GRP_TYPE_WEIGHTED) = FALSE;

    fdata = &(map->field_data->field[lt_fids->ctr_ing_eflex_action_id_fid]);
    BCMECMP_LT_MIN_CTR_ING_EFLEX_ACTION_ID
        (unit, group, BCMECMP_GRP_TYPE_WEIGHTED) = fdata->min->u16;
    BCMECMP_LT_MAX_CTR_ING_EFLEX_ACTION_ID
        (unit, group, BCMECMP_GRP_TYPE_WEIGHTED) = fdata->max->u16;

    fdata = &(map->field_data->field[lt_fids->ctr_ing_eflex_object_fid]);
    BCMECMP_LT_MIN_CTR_ING_EFLEX_OBJECT
        (unit, group, BCMECMP_GRP_TYPE_WEIGHTED) = fdata->min->u16;
    BCMECMP_LT_MAX_CTR_ING_EFLEX_OBJECT
        (unit, group, BCMECMP_GRP_TYPE_WEIGHTED) = fdata->max->u16;

exit:
    if (SHR_FUNC_ERR()) {
        BCMECMP_FREE(gdrd_fields);
        BCMECMP_FREE(mdrd_fields);
        BCMECMP_FREE(lt_fids);
        BCMECMP_FREE(grp_attrs);
    }

    SHR_FUNC_EXIT();
}

static int
bcm56990_a0_ecmp_weighted_grps_flds_cleanup(int unit)
{
    bcm56990_a0_ecmp_grp_drd_fields_t *gdrd_fields = NULL;  /* Group tbl DRD info. */
    bcm56990_a0_ecmp_memb_drd_fields_t *mdrd_fields = NULL; /* Member tbl DRD info. */
    bcmecmp_grp_ltd_fields_t *lt_fids = NULL;   /* LT entry fields LTD ID. */
    bcmecmp_lt_field_attrs_t *grp_attrs = NULL; /* Single LT field attrs. */

    SHR_FUNC_ENTER(unit);

    /* Free memory allocated for storing Group tables DRD fields data. */
    gdrd_fields = (bcm56990_a0_ecmp_grp_drd_fields_t *)BCMECMP_TBL_DRD_FIELDS
                                                           (unit, group,
                                                            BCMECMP_GRP_TYPE_WEIGHTED);
    BCMECMP_FREE(gdrd_fields);

    /* Free memory allocated for storing Member tables DRD fields data. */
    mdrd_fields = (bcm56990_a0_ecmp_memb_drd_fields_t *)BCMECMP_TBL_DRD_FIELDS
                                                            (unit, member,
                                                             BCMECMP_GRP_TYPE_WEIGHTED);
    BCMECMP_FREE(mdrd_fields);

    /* Free memory allocated for storing ECMP LT logical fields data. */
    lt_fids = (bcmecmp_grp_ltd_fields_t *)BCMECMP_TBL_LTD_FIELDS
                                                (unit, group,
                                                 BCMECMP_GRP_TYPE_WEIGHTED);
    BCMECMP_FREE(lt_fids);

    /*
     * Free memory allocated for storing ECMP LT
     * logical fields attributes information.
     */
    grp_attrs = (bcmecmp_lt_field_attrs_t *)BCMECMP_LT_FIELD_ATTRS_VOID_PTR
                                                (unit, group,
                                                 BCMECMP_GRP_TYPE_WEIGHTED);
    BCMECMP_FREE(grp_attrs);

    SHR_FUNC_EXIT();
}

static int
bcm56990_a0_ecmp_fields_init(int unit)
{
    const bcmlrd_map_t *map = NULL; /* LT map. */

    SHR_FUNC_ENTER(unit);

    /* Initialize device ECMP resolution mode field identifier value. */
    BCMECMP_MODE_DRD_FID(unit, config) = INVALIDf;

    switch (BCMECMP_MODE(unit)) {
        case BCMECMP_MODE_FULL:
            /* Get LRD mapping information for ECMP LT. */
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlrd_map_get(unit,
                                BCMECMP_TBL_LTD_SID
                                    (unit, group, BCMECMP_GRP_TYPE_SINGLE),
                                &map));
            SHR_NULL_CHECK(map, SHR_E_INTERNAL);
            SHR_IF_ERR_VERBOSE_EXIT
                (bcm56990_a0_ecmp_single_lt_flds_init(unit, map));

            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlrd_map_get(unit,
                                BCMECMP_TBL_LTD_SID
                                    (unit, group, BCMECMP_GRP_TYPE_WEIGHTED),
                                &map));
            SHR_NULL_CHECK(map, SHR_E_INTERNAL);
            SHR_IF_ERR_VERBOSE_EXIT
                (bcm56990_a0_ecmp_weighted_lt_flds_init(unit, map));
            break;

        default:
            SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

exit:
    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "rv=%d"),
                bcm56990_a0_ecmp_info_print(unit)));

    SHR_FUNC_EXIT();
}

static int
bcm56990_a0_ecmp_fields_cleanup(int unit)
{

    SHR_FUNC_ENTER(unit);

    /* Clear device ECMP resolution mode field identifier value. */
    BCMECMP_MODE_DRD_FID(unit, config) = INVALIDf;

    switch (BCMECMP_MODE(unit)) {
        case BCMECMP_MODE_FULL:
            /* Cleanup Single ECMP resolution LT fields information. */
            bcm56990_a0_ecmp_single_grps_flds_cleanup(unit);

            /* Cleanup weighted ECMP resolution LT fields information. */
            bcm56990_a0_ecmp_weighted_grps_flds_cleanup(unit);
            break;

        default:
            SHR_EXIT();
    }

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56990_a0_ecmp_itbm_lists_init(int unit)
{
    shr_itbm_list_params_t params;
    shr_itbm_list_hdl_t list_hdl = NULL;
    int rv = SHR_E_NONE;
    bcmecmp_tbl_prop_t *grp_tbl = NULL;
    bcmecmp_tbl_prop_t *memb_tbl_1 = NULL;
    bcmecmp_tbl_prop_t *memb_tbl_2 = NULL;
    bcmecmp_info_t *ecmp_info = &(ecmp_ctrl_local[unit]->ecmp_info);

    SHR_FUNC_ENTER(unit);

    grp_tbl = &(ecmp_info->group[BCMECMP_GRP_TYPE_SINGLE]);
    shr_itbm_list_params_t_init(&params);
    params.unit = unit;
    params.table_id = grp_tbl->drd_sid;
    params.min = grp_tbl->imin;
    params.max = grp_tbl->imax;
    params.first = params.min;
    params.last = params.max;
    params.comp_id = BCMMGMT_ECMP_COMP_ID;
    params.block_stat = false;

    rv = shr_itbm_list_create(params, "bcmecmpGrpSingleList", &list_hdl);
    if (rv == SHR_E_NONE && list_hdl != NULL) {
        grp_tbl->list_hdl = list_hdl;
        grp_tbl = &(ecmp_info->group[BCMECMP_GRP_TYPE_WEIGHTED]);
        grp_tbl->list_hdl = list_hdl;
    } else {
        LOG_VERBOSE(BSL_LOG_MODULE,
                            (BSL_META_U(unit,
                                        "Error: RM failed to create list %d\n"),
                             rv));
        SHR_ERR_EXIT(rv);
    }

    memb_tbl_1 = &(ecmp_info->member[BCMECMP_GRP_TYPE_SINGLE]);
    params.table_id = memb_tbl_1->drd_sid;
    params.min = memb_tbl_1->imin;
    params.max = memb_tbl_1->imax;
    params.first = params.min;
    params.last = params.max;
    params.block_stat = true;
    params.buckets = true;
    params.entries_per_bucket = NHOPS_PER_MEMB_ENTRY;
    params.reverse = false;

    list_hdl = NULL;
    rv = shr_itbm_list_create(params, "bcmecmpMemNWECMPList", &list_hdl);
    if (rv == SHR_E_NONE && list_hdl != NULL) {
        memb_tbl_1->list_hdl = list_hdl;
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "RM rv=%d list %p.\n"),
                     rv, ((void *)list_hdl)));
    } else {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "RM rv=%d list %p.\n"),
                   rv, ((void *)list_hdl)));
        SHR_ERR_EXIT(rv);
    }

    memb_tbl_2 = &(ecmp_info->member[BCMECMP_GRP_TYPE_WEIGHTED]);
    params.entries_per_bucket = NHOPS_PER_MEMB_ENTRY_WECMP;
    params.reverse = TRUE;
    list_hdl = NULL;
    rv = shr_itbm_list_create(params, "bcmecmpMemWECMPList", &list_hdl);
    if (rv == SHR_E_NONE && list_hdl != NULL) {
        memb_tbl_2->list_hdl = list_hdl;
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "RM rv=%d list %p.\n"),
                     rv, ((void *)list_hdl)));
    } else {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "RM rv=%d list %p.\n"),
                   rv, ((void *)list_hdl)));
        SHR_ERR_EXIT(rv);
    }

    /*
     * Share the SINGLE type member list for MEMBER_REPLICATION groups
     * in ECMP_WEIGHTED.
     */
    memb_tbl_2->list_hdl_1 = memb_tbl_1->list_hdl;

exit:
    if (SHR_FUNC_ERR()) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "RM failed to cleanup list rv=%d \
                              list %p\n"),
                   rv, ((void *)list_hdl)));
        if (list_hdl) {
            shr_itbm_list_destroy(list_hdl);
        }
        list_hdl = grp_tbl->list_hdl;
        if (list_hdl) {
            grp_tbl->list_hdl = NULL;
            grp_tbl = &(ecmp_info->group[BCMECMP_GRP_TYPE_SINGLE]);
            grp_tbl->list_hdl = NULL;
            shr_itbm_list_destroy(list_hdl);
        }

        if (memb_tbl_1) {
            list_hdl = memb_tbl_1->list_hdl;
            if (list_hdl) {
                memb_tbl_1->list_hdl = NULL;
                shr_itbm_list_destroy(list_hdl);
            }
        }

        if (memb_tbl_2) {
            list_hdl = memb_tbl_2->list_hdl;
            if (list_hdl) {
                memb_tbl_2->list_hdl = NULL;
                shr_itbm_list_destroy(list_hdl);
            }
            memb_tbl_2->list_hdl_1 = NULL;
        }
    }
    SHR_FUNC_EXIT();
}

static int
bcm56990_a0_ecmp_itbm_lists_cleanup(int unit)
{
    shr_itbm_list_hdl_t list_hdl;
    int rv = SHR_E_NONE;
    bcmecmp_tbl_prop_t *tbl_prop = NULL;
    bcmecmp_info_t *ecmp_info = &(ecmp_ctrl_local[unit]->ecmp_info);

    SHR_FUNC_ENTER(unit);
    tbl_prop = &(ecmp_info->group[BCMECMP_GRP_TYPE_SINGLE]);
    list_hdl = tbl_prop->list_hdl;
    rv = shr_itbm_list_destroy(list_hdl);
    SHR_IF_ERR_EXIT(rv);
    tbl_prop->list_hdl = NULL;
    tbl_prop = &(ecmp_info->group[BCMECMP_GRP_TYPE_WEIGHTED]);
    tbl_prop->list_hdl = NULL;

    tbl_prop =  &(ecmp_info->member[BCMECMP_GRP_TYPE_SINGLE]);
    list_hdl = tbl_prop->list_hdl;
    rv = shr_itbm_list_destroy(list_hdl);
    SHR_IF_ERR_EXIT(rv);
    tbl_prop->list_hdl = NULL;

    tbl_prop =  &(ecmp_info->member[BCMECMP_GRP_TYPE_WEIGHTED]);
    list_hdl = tbl_prop->list_hdl;
    rv = shr_itbm_list_destroy(list_hdl);
    SHR_IF_ERR_EXIT(rv);
    tbl_prop->list_hdl = NULL;
    tbl_prop->list_hdl_1 = NULL;

exit:
    if (SHR_FUNC_ERR()) {
       if (list_hdl) {
           LOG_VERBOSE(BSL_LOG_MODULE,
                       (BSL_META_U(unit,
                                   "Error: RM cleaning up list %p"),
                        ((void *)list_hdl)));
       }
    }
    SHR_FUNC_EXIT();
}

static int
bcm56990_a0_ecmp_tables_init(int unit)
{
    SHR_FUNC_ENTER(unit);

    /* Initialize ECMP tables DRD-SID and LTD-SID details. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcm56990_a0_ecmp_sids_init(unit));

    /* Initialize ECMP hardware tables size and index range info. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcm56990_a0_ecmp_hw_tbls_range_init(unit));

    /* Initialize ITBM list for hardware tables. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcm56990_a0_ecmp_itbm_lists_init(unit));

    /* Initialize ECMP tables logical and physical field details. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcm56990_a0_ecmp_fields_init(unit));

    exit:
        SHR_FUNC_EXIT();
}

static int
bcm56990_a0_ecmp_tables_clear(int unit)
{
    SHR_FUNC_ENTER(unit);

    /* Clear ECMP tables logical and physical field details. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcm56990_a0_ecmp_fields_cleanup(unit));

    /* Clear ECMP hardware tables size and index range info. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcm56990_a0_ecmp_hw_tbls_range_clear(unit));

    /* Clear the ITBM lists for ECMP hardware tables. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcm56990_a0_ecmp_itbm_lists_cleanup(unit));

    /* Clear ECMP tables DRD-SID and LTD-SID details. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcm56990_a0_ecmp_sids_clear(unit));

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56990_a0_ecmp_mode_init(int unit)
{
    SHR_FUNC_ENTER(unit);

    /*
     * Validate Admin configured device ECMP mode is valid for this chip
     * before proceeding with further initialization.
     */
    if (BCMECMP_MODE(unit) > BCMECMP_MODE_FULL) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "Invalid ECMP MODE=%u configured for Unit=%d.\n"),
                   BCMECMP_MODE(unit),
                   unit));
        SHR_ERR_EXIT(SHR_E_CONFIG);
    }

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56990_a0_ecmp_db_init(int unit, bool warm)
{
    bcmecmp_tbl_prop_t *tbl_ptr; /* Pointer to ECMP hardware table info. */
    size_t tbl_size; /* ECMP group or member table size. */
    uint32_t ha_alloc_sz = 0; /* Size of allocated HA memory block. */
    uint32_t tot_ha_mem = 0; /* Total HA memory allocated by ECMP CTH. */
    uint32_t blk_sig = 0x56990000; /* ECMP HA block signature. */
    bcmecmp_hw_entry_ha_blk_t *hw_array_ptr;
    bcmecmp_grp_ha_blk_t *grp_array_ptr;

    SHR_FUNC_ENTER(unit);

    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit,
                         "%s warm = %s.\n"),
              __func__,
              warm ? "TRUE" : "FALSE"));

    switch (BCMECMP_MODE(unit)) {
        case BCMECMP_MODE_FULL:
            /*
             * Calculate the size of HA memory block to be allocated for storing
             * ECMP member table active array elements.
             */
            tbl_size = (size_t)BCMECMP_TBL_SIZE
                                    (unit, member, BCMECMP_GRP_TYPE_SINGLE);

            /*
             * Allocate memory for active Member Table entries array
             * elements.
             */
            SHR_IF_ERR_EXIT
                (bcmecmp_hw_entry_ha_alloc(unit, warm, tbl_size,
                                           BCMECMP_MEMB_TBL_SUB_COMP_ID,
                                           (blk_sig |
                                            BCMECMP_MEMB_TBL_SUB_COMP_ID),
                                           &hw_array_ptr, &ha_alloc_sz));
            tbl_ptr = BCMECMP_TBL_PTR(unit, member, BCMECMP_GRP_TYPE_SINGLE);
            tbl_ptr->ent_arr = hw_array_ptr->array;
            tot_ha_mem += ha_alloc_sz;

            (BCMECMP_TBL_PTR
                (unit, member,
                 BCMECMP_GRP_TYPE_WEIGHTED))->ent_arr = tbl_ptr->ent_arr;

            /*
             * Calculate the size of HA memory block to be allocated for storing
             * ECMP group table active array elements.
             */
            tbl_size = (size_t)BCMECMP_TBL_SIZE
                                    (unit, group, BCMECMP_GRP_TYPE_SINGLE);

            /* Allocate memory for active Group table entries array elements. */
            SHR_IF_ERR_EXIT
                (bcmecmp_hw_entry_ha_alloc(unit, warm, tbl_size,
                                           BCMECMP_GRP_TBL_SUB_COMP_ID,
                                           (blk_sig |
                                            BCMECMP_GRP_TBL_SUB_COMP_ID),
                                           &hw_array_ptr, &ha_alloc_sz));
            tbl_ptr = BCMECMP_TBL_PTR(unit, group, BCMECMP_GRP_TYPE_SINGLE);
            tbl_ptr->ent_arr = hw_array_ptr->array;
            tot_ha_mem += ha_alloc_sz;

            (BCMECMP_TBL_PTR
                (unit, group,
                 BCMECMP_GRP_TYPE_WEIGHTED))->ent_arr = tbl_ptr->ent_arr;
            break;

        default:
            SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    /* Allocate memory for active ECMP group LT entries array elements. */
    SHR_IF_ERR_EXIT
        (bcmecmp_grp_attr_ha_alloc(unit,
                                   warm,
                                   tbl_size,
                                   BCMECMP_GRP_SUB_COMP_ID,
                                   (blk_sig | BCMECMP_GRP_SUB_COMP_ID),
                                   &grp_array_ptr,
                                   &ha_alloc_sz));
    BCMECMP_GRP_INFO_PTR(unit) = grp_array_ptr->array;
    /* Used for ITBM group restore when warmboot. */
    BCMECMP_INFO(unit).grp_arr_sz = grp_array_ptr->array_size;
    tot_ha_mem += ha_alloc_sz;

    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "Total ECMP HA Mem = %uKB.\n"),
                tot_ha_mem / 1024));

exit:
    if (SHR_FUNC_ERR()) {
        if (BCMECMP_GRP_INFO_PTR(unit)) {
            grp_array_ptr = (bcmecmp_grp_ha_blk_t *)
                             ((uint8_t *)BCMECMP_GRP_INFO_PTR(unit)
                              - sizeof(bcmecmp_grp_ha_blk_t));
            LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_U(unit,
                                    "\n\t free grp HA addr=0x%p\n"),
                         (void*)grp_array_ptr));
            shr_ha_mem_free(unit, grp_array_ptr);
            BCMECMP_GRP_INFO_PTR(unit) = NULL;
        }

        switch (BCMECMP_MODE(unit)) {
            case BCMECMP_MODE_FULL:
                tbl_ptr = BCMECMP_TBL_PTR
                              (unit, member, BCMECMP_GRP_TYPE_SINGLE);
                if (tbl_ptr->ent_arr) {
                    hw_array_ptr = (bcmecmp_hw_entry_ha_blk_t *)
                                    ((uint8_t *)tbl_ptr->ent_arr
                                     - sizeof(bcmecmp_hw_entry_ha_blk_t));
                    LOG_VERBOSE(BSL_LOG_MODULE,
                                (BSL_META_U(unit,
                                            "\n\t free memb hw HA=0x%p\n"),
                                 (void*)hw_array_ptr));
                    shr_ha_mem_free(unit, hw_array_ptr);
                    tbl_ptr->ent_arr = NULL;
                }

                tbl_ptr = BCMECMP_TBL_PTR
                              (unit, group, BCMECMP_GRP_TYPE_SINGLE);
                if (tbl_ptr->ent_arr) {
                    hw_array_ptr = (bcmecmp_hw_entry_ha_blk_t *)
                                    ((uint8_t *)tbl_ptr->ent_arr
                                     - sizeof(bcmecmp_hw_entry_ha_blk_t));
                    LOG_VERBOSE(BSL_LOG_MODULE,
                                (BSL_META_U(unit,
                                            "\n\t free grp hw HA=0x%p\n"),
                                 (void*)hw_array_ptr));
                    shr_ha_mem_free(unit, hw_array_ptr);
                    tbl_ptr->ent_arr = NULL;
                }
                break;

            default:
                SHR_ERR_EXIT(SHR_E_INTERNAL);
        }
    }

    SHR_FUNC_EXIT();
}

static int
bcm56990_a0_ecmp_db_clear(int unit)
{
    bcmecmp_tbl_prop_t *tbl_ptr; /* Pointer to ECMP hardware table info. */

    SHR_FUNC_ENTER(unit);

    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit,
                         "%s.\n"), __func__));
    /*
     * Don't free the HA memory, initialize all HA pointer to NULL for
     * cleanup.
     */
    /* Clear ECMP group logical table active array base pointers. */
    BCMECMP_GRP_INFO_PTR(unit) = NULL;

    switch (BCMECMP_MODE(unit)) {
        case BCMECMP_MODE_FULL:
            /* Clear Member table active array base pointers. */
            tbl_ptr = BCMECMP_TBL_PTR(unit, member, BCMECMP_GRP_TYPE_SINGLE);
            tbl_ptr->ent_arr = NULL;

            /* Clear Group table active array base pointers. */
            tbl_ptr = BCMECMP_TBL_PTR(unit, group, BCMECMP_GRP_TYPE_SINGLE);
            tbl_ptr->ent_arr = NULL;

            /* Clear weighted ECMP Member table active array base pointers. */
            tbl_ptr = BCMECMP_TBL_PTR(unit, member, BCMECMP_GRP_TYPE_WEIGHTED);
            tbl_ptr->ent_arr = NULL;

            /* Clear weighted ECMP Group table active array base pointers. */
            tbl_ptr = BCMECMP_TBL_PTR(unit, group, BCMECMP_GRP_TYPE_WEIGHTED);
            tbl_ptr->ent_arr = NULL;

            /* Used for ITBM group restore when warmboot. */
            BCMECMP_INFO(unit).grp_arr_sz = 0;
            break;

        default:
            SHR_EXIT();
    }

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56990_a0_ecmp_abort_db_init(int unit, bool warm)
{
    bcmecmp_tbl_prop_t *tbl_ptr; /* Pointer to ECMP hardware table info. */
    size_t tbl_size; /* ECMP group or member table size. */
    uint32_t ha_alloc_sz = 0; /* Size of allocated HA memory block. */
    uint32_t tot_ha_mem = 0; /* Total HA memory allocated by ECMP CTH. */
    uint32_t blk_sig = 0x56980000; /* ECMP HA block signature. */
    bcmecmp_hw_entry_ha_blk_t *hw_array_ptr;
    bcmecmp_grp_ha_blk_t *grp_array_ptr;

    SHR_FUNC_ENTER(unit);

    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit,
                         "%s warm = %s.\n"),
              __func__,
              warm ? "TRUE" : "FALSE"));

    switch (BCMECMP_MODE(unit)) {
        case BCMECMP_MODE_FULL:
            /*
             * Calculate the size of HA memory block to be allocated for storing
             * ECMP member table active array elements.
             */
            tbl_size = (size_t)BCMECMP_TBL_SIZE
                                    (unit, member, BCMECMP_GRP_TYPE_SINGLE);
            /*
             * Allocate memory for active Member Table entries array
             * elements.
             */
            SHR_IF_ERR_EXIT
                (bcmecmp_hw_entry_ha_alloc(unit, warm, tbl_size,
                                           BCMECMP_MEMB_TBL_BK_SUB_COMP_ID,
                                           (blk_sig |
                                            BCMECMP_MEMB_TBL_BK_SUB_COMP_ID),
                                           &hw_array_ptr, &ha_alloc_sz));
            tbl_ptr = BCMECMP_TBL_PTR(unit, member, BCMECMP_GRP_TYPE_SINGLE);
            tbl_ptr->ent_bk_arr = hw_array_ptr->array;
            tot_ha_mem += ha_alloc_sz;

            (BCMECMP_TBL_PTR
                (unit, member,
                 BCMECMP_GRP_TYPE_WEIGHTED))->ent_bk_arr = tbl_ptr->ent_bk_arr;

            /*
             * Calculate the size of HA memory block to be allocated for storing
             * ECMP group table active array elements.
             */
            tbl_size = (size_t)BCMECMP_TBL_SIZE
                                    (unit, group, BCMECMP_GRP_TYPE_SINGLE);

            SHR_IF_ERR_EXIT
                (bcmecmp_hw_entry_ha_alloc(unit, warm, tbl_size,
                                           BCMECMP_GRP_TBL_BK_SUB_COMP_ID,
                                           (blk_sig |
                                            BCMECMP_GRP_TBL_BK_SUB_COMP_ID),
                                           &hw_array_ptr, &ha_alloc_sz));
            tbl_ptr = BCMECMP_TBL_PTR(unit, group, BCMECMP_GRP_TYPE_SINGLE);
            tbl_ptr->ent_bk_arr = hw_array_ptr->array;
            tot_ha_mem += ha_alloc_sz;

            (BCMECMP_TBL_PTR
                (unit, group,
                 BCMECMP_GRP_TYPE_WEIGHTED))->ent_bk_arr = tbl_ptr->ent_bk_arr;
            break;

        default:
            SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    /* Allocate memory for active ECMP group LT entries array elements. */
    SHR_IF_ERR_EXIT
        (bcmecmp_grp_attr_ha_alloc(unit,
                                   warm,
                                   tbl_size,
                                   BCMECMP_GRP_BK_SUB_COMP_ID,
                                   (blk_sig | BCMECMP_GRP_BK_SUB_COMP_ID),
                                   &grp_array_ptr,
                                   &ha_alloc_sz));
    BCMECMP_GRP_INFO_BK_PTR(unit) = grp_array_ptr->array;
    tot_ha_mem += ha_alloc_sz;

    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "Total ECMP HA BK Mem = %uKB.\n"),
                tot_ha_mem / 1024));

exit:
    if (SHR_FUNC_ERR()) {
        if (BCMECMP_GRP_INFO_BK_PTR(unit)) {
            grp_array_ptr = (bcmecmp_grp_ha_blk_t *)
                             ((uint8_t *)BCMECMP_GRP_INFO_BK_PTR(unit)
                              - sizeof(bcmecmp_grp_ha_blk_t));
            LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_U(unit,
                                    "\n\t free bk grp HA addr=0x%p\n"),
                         (void*)grp_array_ptr));
            shr_ha_mem_free(unit, grp_array_ptr);
            BCMECMP_GRP_INFO_BK_PTR(unit) = NULL;
        }

        switch (BCMECMP_MODE(unit)) {
            case BCMECMP_MODE_FULL:
                tbl_ptr = BCMECMP_TBL_PTR
                              (unit, member, BCMECMP_GRP_TYPE_SINGLE);
                if (tbl_ptr->ent_bk_arr) {
                    hw_array_ptr = (bcmecmp_hw_entry_ha_blk_t *)
                                    ((uint8_t *)tbl_ptr->ent_bk_arr
                                     - sizeof(bcmecmp_hw_entry_ha_blk_t));
                    LOG_VERBOSE(BSL_LOG_MODULE,
                                (BSL_META_U(unit,
                                            "\n\tfree bk memb hw HA=0x%p\n"),
                                 (void*)hw_array_ptr));
                    shr_ha_mem_free(unit, hw_array_ptr);
                    tbl_ptr->ent_bk_arr = NULL;
                }

                tbl_ptr = BCMECMP_TBL_PTR
                              (unit, group, BCMECMP_GRP_TYPE_SINGLE);
                if (tbl_ptr->ent_bk_arr) {
                    hw_array_ptr = (bcmecmp_hw_entry_ha_blk_t *)
                                    ((uint8_t *)tbl_ptr->ent_bk_arr
                                     - sizeof(bcmecmp_hw_entry_ha_blk_t));
                    LOG_VERBOSE(BSL_LOG_MODULE,
                                (BSL_META_U(unit,
                                            "\n\tfree bk grp hw HA=0x%p\n"),
                                 (void*)hw_array_ptr));
                    shr_ha_mem_free(unit, hw_array_ptr);
                    tbl_ptr->ent_bk_arr = NULL;
                }
                break;

            default:
                SHR_ERR_EXIT(SHR_E_INTERNAL);
        }
    }

    SHR_FUNC_EXIT();
}

static int
bcm56990_a0_ecmp_abort_db_clear(int unit)
{
    bcmecmp_tbl_prop_t *tbl_ptr; /* Pointer to ECMP hardware table info. */

    SHR_FUNC_ENTER(unit);

    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit,
                         "%s.\n"), __func__));
    /*
     * Don't free the HA memory, initialize all HA pointer to NULL for
     * cleanup.
     */
    /* Clear ECMP group logical table active and backup arrays base pointers. */
    BCMECMP_GRP_INFO_BK_PTR(unit) = NULL;

    switch (BCMECMP_MODE(unit)) {
        case BCMECMP_MODE_FULL:
            /* Clear Member table entries array base pointer. */
            tbl_ptr = BCMECMP_TBL_PTR(unit, member, BCMECMP_GRP_TYPE_SINGLE);
            tbl_ptr->ent_bk_arr = NULL;

            /* Clear Group table entries array base pointer. */
            tbl_ptr = BCMECMP_TBL_PTR(unit, group, BCMECMP_GRP_TYPE_SINGLE);
            tbl_ptr->ent_bk_arr = NULL;

            /* Clear weighted ECMP Member table entries array base pointer. */
            tbl_ptr = BCMECMP_TBL_PTR(unit, member, BCMECMP_GRP_TYPE_WEIGHTED);
            tbl_ptr->ent_bk_arr = NULL;

            /* Clear weighted ECMP Group table entries array base pointer. */
            tbl_ptr = BCMECMP_TBL_PTR(unit, group, BCMECMP_GRP_TYPE_WEIGHTED);
            tbl_ptr->ent_bk_arr = NULL;
            break;

        default:
            /*
             * Chip ECMP Mode Init function will detect mis-configured Mode value
             * and return config error before table entries are initialized.
             * So, there are no entry resources to free during cleanup for
             * invalid ECMP Mode config case.
             */
            SHR_EXIT();
    }

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56990_a0_ecmp_entries_init(int unit, bool warm)
{
    bcmecmp_info_t *ecmp_info = NULL;

    SHR_FUNC_ENTER(unit);

    /*
     * Allocate HA memory for managing ECMP group and member table
     * entries.
     */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcm56990_a0_ecmp_db_init(unit, warm));

    /* Initialize atomic transaction related SW database. */
    if (BCMECMP_ATOMIC_TRANS(unit)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcm56990_a0_ecmp_abort_db_init(unit, warm));
    }

    ecmp_info = &(bcmecmp_get_ecmp_control(unit)->ecmp_info);
    BCMECMP_ALLOC
        (ecmp_info->rhg,
         sizeof(*(ecmp_info->rhg)) * BCMECMP_RHG_INFO_PTR_ARRAY_SIZE,
         "bcmecmpDevRhgInfo");
    if (BCMECMP_ATOMIC_TRANS(unit)) {
        BCMECMP_ALLOC
            (ecmp_info->rhg_bk,
             sizeof(*(ecmp_info->rhg_bk)) * BCMECMP_RHG_INFO_PTR_ARRAY_SIZE,
             "bcmecmpDevRhgBkInfo");
    }

    exit:
        SHR_FUNC_EXIT();
}

static int
bcm56990_a0_itbm_group_cleanup(int unit)
{
    int ecmp_id, gstart_idx, gend_idx;
    uint32_t mstart_idx = 0;
    uint32_t max_members = 0;
    int rv = SHR_E_NONE;
    bcmecmp_info_t *ecmp_info;
    shr_itbm_list_hdl_t list_hdl;
    bcmecmp_grp_attr_t *grp_attr, *grp_attr_base;
    uint32_t bucket, first;
    int gtype;

    SHR_FUNC_ENTER(unit);

    ecmp_info = &(bcmecmp_get_ecmp_control(unit)->ecmp_info);
    grp_attr_base = (bcmecmp_grp_attr_t *)(ecmp_info->grp_arr);
    gstart_idx = 0;
    gend_idx = ecmp_info->grp_arr_sz;
    for (ecmp_id = gstart_idx; ecmp_id < gend_idx; ecmp_id++) {
        grp_attr = grp_attr_base + ecmp_id;
        /* check if the HA memory has the group configured. */
        if (!grp_attr->in_use_itbm) {
            continue;
        }

        gtype = grp_attr->grp_type;
        list_hdl = ecmp_info->group[gtype].list_hdl;
        rv = shr_itbm_list_block_free
                 (list_hdl,
                  1,
                  SHR_ITBM_INVALID,
                  ecmp_id);
        if (rv) {
            LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_U(unit,
                                    "\n Fail to free the group block "
                                    "for Group_id %d.\n"),
                         ecmp_id));
            SHR_ERR_EXIT(SHR_E_INTERNAL);
        }

        mstart_idx = grp_attr->mstart_idx;
        bcmecmp_itbm_bucket_first_get(unit, gtype, grp_attr->weighted_mode,
                                      mstart_idx, &bucket, &first);
        /* Check RH group's max_members. */
        if (grp_attr->lb_mode == BCMECMP_LB_MODE_RESILIENT) {
            max_members = grp_attr->rh_entries_cnt;
        } else {
            max_members = grp_attr->max_paths;
        }
        list_hdl = ecmp_info->member[gtype].list_hdl;
        if (bcmecmp_grp_is_member_replication(gtype, grp_attr->weighted_mode)) {
            list_hdl = ecmp_info->member[gtype].list_hdl_1;
        }
        rv = shr_itbm_list_block_free
                 (list_hdl,
                  max_members,
                  bucket,
                  first);
        if (rv) {
            LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_U(unit,
                                    "\n Fail to free the member block "
                                    "block for Group_id %d.\n"),
                         ecmp_id));
            SHR_ERR_EXIT(SHR_E_INTERNAL);
        }
    }

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56990_a0_ecmp_entries_cleanup(int unit)
{
    bcmecmp_info_t *ecmp_info = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcm56990_a0_itbm_group_cleanup(unit));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcm56990_a0_ecmp_db_clear(unit));

    /* Clear atomic transaction related SW database info. */
    if (BCMECMP_ATOMIC_TRANS(unit)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcm56990_a0_ecmp_abort_db_clear(unit));
    }

    ecmp_info = &(bcmecmp_get_ecmp_control(unit)->ecmp_info);
    BCMECMP_FREE(ecmp_info->rhg);
    BCMECMP_FREE(ecmp_info->rhg_bk);

    exit:
        SHR_FUNC_EXIT();
}
static int bcm56990_a0_ecmp_lt_entry_alloc(int unit)
{
    SHR_FUNC_ENTER(unit);

    /* Allocate memory to store LT Entry information. */
    BCMECMP_ALLOC
        (BCMECMP_LT_ENT_PTR_VOID(unit),  sizeof(bcmecmp_lt_entry_t),
         "bcmecmpDevLtEntInfo");

    /* Allocate memory to store current LT Entry information. */
    BCMECMP_ALLOC
        (BCMECMP_CURRENT_LT_ENT_PTR_VOID(unit), sizeof(bcmecmp_lt_entry_t),
         "bcmecmpDevCurrentLtEntInfo");
    exit:
        SHR_FUNC_EXIT();
}

static void bcm56990_a0_ecmp_lt_entry_free(int unit)
{
    /* Free memory allocated for LT Entry buffer. */
    if (BCMECMP_LT_ENT_PTR(unit)) {
        BCMECMP_FREE(BCMECMP_LT_ENT_PTR_VOID(unit));
    }

    /* Free memory allocated for current LT Entry buffer. */
    if (BCMECMP_CURRENT_LT_ENT_PTR(unit)) {
        BCMECMP_FREE(BCMECMP_CURRENT_LT_ENT_PTR_VOID(unit));
    }
}

static int
bcm56990_a0_ecmp_info_init(int unit, bool warm)
{
    SHR_FUNC_ENTER(unit);

    /* Initialize BCMCFG-ECMP mode to hardware ECMP mode mapping list. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcm56990_a0_ecmp_mode_init(unit));

    /* Initialize ECMP tables information for Tomahawk4 device. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcm56990_a0_ecmp_tables_init(unit));

    /*
     * Initialize ECMP Group and Member tables entries array for Single level,
     * Overlay and Underlay groups support.
     */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcm56990_a0_ecmp_entries_init(unit, warm));

    /* Allocate LT entry. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcm56990_a0_ecmp_lt_entry_alloc(unit));

    exit:
        SHR_FUNC_EXIT();
}

static int
bcm56990_a0_ecmp_info_cleanup(int unit)
{
    SHR_FUNC_ENTER(unit);

    /* Cleanup memory allocated for ECMP Group and Member table entries info. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcm56990_a0_ecmp_entries_cleanup(unit));

    /* Clear ECMP tables information stored in SW. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcm56990_a0_ecmp_tables_clear(unit));

    /* Initialize device ECMP mode to hardware reset default value. */
    BCMECMP_MODE(unit) = BCMECMP_MODE_FULL;

    /* Free LT entry. */
    bcm56990_a0_ecmp_lt_entry_free(unit);

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56990_a0_ecmp_mode_config(int unit)
{
    return SHR_E_NONE;
}

static int
bcm56990_a0_ecmp_memb_ent_range_get(int start_idx, int num_nhop, int *ent_idx,
                                    int *ent_offset, int *ent_num)
{
    int tmp = 0;

    if (ent_idx == NULL || ent_offset == NULL) {
        return SHR_E_PARAM;
    }

    *ent_idx = start_idx / NHOPS_PER_MEMB_ENTRY;
    *ent_offset = start_idx % NHOPS_PER_MEMB_ENTRY;

    if (*ent_offset != 0) {
        tmp++;
        num_nhop -= (NHOPS_PER_MEMB_ENTRY - *ent_offset);
    }

    if (num_nhop > 0) {
        tmp += num_nhop / NHOPS_PER_MEMB_ENTRY;
        if ((num_nhop % NHOPS_PER_MEMB_ENTRY) != 0) {
            tmp++;
        }
    }

    if (ent_num != NULL) {
        *ent_num = tmp;
    }

    return SHR_E_NONE;
}

static int
bcm56990_a0_ecmp_memb_table_pio_write(int unit, bcmecmp_lt_entry_t *lt_ent,
                                      bool rh_mode)
{
    bcm56990_a0_ecmp_memb_drd_fields_t *memb_flds;
    bcmecmp_grp_type_t gtype;
    uint32_t *pt_entry_buff = NULL;
    uint32_t *nhop_arr = NULL, val;
    int nhop_arr_idx = 0;
    int start_idx, num_paths, max_paths;
    int ent_idx, ent_offset;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(lt_ent, SHR_E_PARAM);

    gtype = lt_ent->grp_type;

    memb_flds = (bcm56990_a0_ecmp_memb_drd_fields_t *)BCMECMP_TBL_DRD_FIELDS
                                                          (unit, member, gtype);
    SHR_NULL_CHECK(memb_flds, SHR_E_INTERNAL);

    BCMECMP_ALLOC(pt_entry_buff,
                  BCMECMP_TBL_ENT_SIZE_MAX(unit) * sizeof(uint32_t),
                  "bcmecmpBcm56990MembTblPioWritePtBuf");

    start_idx = lt_ent->mstart_idx;
    if (rh_mode) {
        num_paths = lt_ent->rh_entries_cnt;
        max_paths = lt_ent->rh_entries_cnt;
        nhop_arr = (uint32_t *)lt_ent->rh_entries_arr;
    } else {
        num_paths = lt_ent->num_paths;
        max_paths = lt_ent->max_paths;
        nhop_arr = (uint32_t *)lt_ent->nhop_id;
    }

    SHR_IF_ERR_EXIT
        (bcm56990_a0_ecmp_memb_ent_range_get(start_idx, max_paths, &ent_idx,
                                             &ent_offset, NULL));

    nhop_arr_idx = 0;
    while (max_paths > 0) {
        sal_memset(pt_entry_buff, 0,
                   BCMECMP_TBL_ENT_SIZE_MAX(unit) * sizeof(uint32_t));

        /*
         * The group may not occupy all of four members of the first
         * and the last entry.
         */
        if (ent_offset != 0 || max_paths < NHOPS_PER_MEMB_ENTRY) {
            SHR_IF_ERR_EXIT
                (bcm56990_a0_ecmp_lt_entry_memb_read(unit, lt_ent, ent_idx, 1,
                                                     false, pt_entry_buff));
        }

        while (ent_offset < NHOPS_PER_MEMB_ENTRY && max_paths > 0) {
            if (nhop_arr_idx < num_paths) {
                val = (uint32_t)nhop_arr[nhop_arr_idx];
            } else {
                val = 0;
            }

            SHR_IF_ERR_EXIT
                (bcmdrd_pt_field_set(unit,
                                     BCMECMP_TBL_DRD_SID(unit, member, gtype),
                                     pt_entry_buff,
                                     memb_flds->ecmp_nhop_id[ent_offset],
                                     &(val)));

            ent_offset++;
            nhop_arr_idx++;
            max_paths--;
        }

        SHR_IF_ERR_EXIT
            (bcm56990_a0_ecmp_lt_entry_memb_write(unit, lt_ent, ent_idx, 1,
                                                  false, pt_entry_buff));

        ent_idx++;
        ent_offset = 0;
    }

exit:
    BCMECMP_FREE(pt_entry_buff);
    SHR_FUNC_EXIT();
}

static int
bcm56990_a0_ecmp_memb_table_dma_write(int unit, bcmecmp_lt_entry_t *lt_ent,
                                      bool rh_mode)
{
    bcm56990_a0_ecmp_memb_drd_fields_t *memb_flds;
    bcmecmp_grp_type_t gtype;
    uint32_t *nhop_arr = NULL;
    uint32_t *dma_buf = NULL;
    uint32_t dma_alloc_sz = 0, ent_size, val;
    int nhop_arr_idx = 0, dma_buf_idx = 0;
    int start_idx, num_paths, max_paths;
    int ent_idx, ent_offset, ent_num;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(lt_ent, SHR_E_PARAM);

    gtype = lt_ent->grp_type;

    memb_flds = (bcm56990_a0_ecmp_memb_drd_fields_t *)BCMECMP_TBL_DRD_FIELDS
                                                          (unit, member, gtype);
    SHR_NULL_CHECK(memb_flds, SHR_E_INTERNAL);

    start_idx = lt_ent->mstart_idx;
    if (rh_mode) {
        num_paths = lt_ent->rh_entries_cnt;
        max_paths = lt_ent->rh_entries_cnt;
        nhop_arr = (uint32_t *)lt_ent->rh_entries_arr;
    } else {
        num_paths = lt_ent->num_paths;
        max_paths = lt_ent->max_paths;
        nhop_arr = (uint32_t *)lt_ent->nhop_id;
    }

    SHR_IF_ERR_EXIT
        (bcm56990_a0_ecmp_memb_ent_range_get(start_idx, max_paths, &ent_idx,
                                             &ent_offset, &ent_num));

    ent_size = BCMECMP_TBL_ENT_SIZE(unit, member, gtype);
    dma_alloc_sz = (ent_size * sizeof(uint32_t) * ent_num);
    BCMECMP_ALLOC(dma_buf, dma_alloc_sz, "bcmecmpBcm56990GrpDmaWritePtDmaBuf");

    nhop_arr_idx = 0;
    dma_buf_idx = 0;
    start_idx = ent_idx;

    while (max_paths > 0) {
        /*
         * The group may not occupy all of four members of the first
         * and the last entry.
         */
        if (ent_offset != 0 || max_paths < NHOPS_PER_MEMB_ENTRY) {
            SHR_IF_ERR_EXIT
                (bcm56990_a0_ecmp_lt_entry_memb_read(unit, lt_ent, ent_idx, 1, false,
                                                     &dma_buf[dma_buf_idx * ent_size]));
        }

        while (ent_offset < NHOPS_PER_MEMB_ENTRY && max_paths > 0) {
            if (nhop_arr_idx < num_paths) {
                val = (uint32_t)nhop_arr[nhop_arr_idx];
            } else {
                val = 0;
            }

            SHR_IF_ERR_EXIT
                (bcmdrd_pt_field_set(unit,
                                     BCMECMP_TBL_DRD_SID(unit, member, gtype),
                                     &dma_buf[dma_buf_idx * ent_size],
                                     memb_flds->ecmp_nhop_id[ent_offset],
                                     &(val)));

            ent_offset++;
            nhop_arr_idx++;
            max_paths--;
        }

        ent_idx++;
        ent_offset = 0;
        dma_buf_idx++;
    }

    SHR_IF_ERR_EXIT
        (bcm56990_a0_ecmp_lt_entry_memb_write(unit, lt_ent, start_idx, ent_num,
                                              true, dma_buf));

exit:
    BCMECMP_FREE(dma_buf);
    SHR_FUNC_EXIT();
}

static int
bcm56990_a0_ecmp_memb_table_pio_clear(int unit, bcmecmp_lt_entry_t *lt_ent,
                                      bool rh_mode)
{
    bcm56990_a0_ecmp_memb_drd_fields_t *memb_flds;
    bcmecmp_grp_type_t gtype;
    uint32_t *pt_entry_buff = NULL;
    uint32_t val = 0;
    int start_idx, max_paths;
    int ent_idx, ent_offset;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(lt_ent, SHR_E_PARAM);

    gtype = lt_ent->grp_type;

    memb_flds = (bcm56990_a0_ecmp_memb_drd_fields_t *)BCMECMP_TBL_DRD_FIELDS
                                                          (unit, member, gtype);
    SHR_NULL_CHECK(memb_flds, SHR_E_INTERNAL);

    BCMECMP_ALLOC(pt_entry_buff,
                  BCMECMP_TBL_ENT_SIZE_MAX(unit) * sizeof(uint32_t),
                  "bcmecmpBcm56990MembTblPioClearPtBuf");

    start_idx = BCMECMP_GRP_MEMB_TBL_START_IDX(unit, lt_ent->ecmp_id);
    if (rh_mode) {
        max_paths = BCMECMP_GRP_RH_ENTRIES_CNT(unit, lt_ent->ecmp_id);
    } else {
        max_paths = BCMECMP_GRP_MAX_PATHS(unit, lt_ent->ecmp_id);
    }

    SHR_IF_ERR_EXIT
        (bcm56990_a0_ecmp_memb_ent_range_get(start_idx, max_paths, &ent_idx,
                                             &ent_offset, NULL));

    while (max_paths > 0) {
        sal_memset(pt_entry_buff, 0,
                   BCMECMP_TBL_ENT_SIZE_MAX(unit) * sizeof(uint32_t));

        /*
         * The group may not occupy all of four members of the first
         * and the last entry.
         */
        if (ent_offset != 0 || max_paths < NHOPS_PER_MEMB_ENTRY) {
            SHR_IF_ERR_EXIT
                (bcm56990_a0_ecmp_lt_entry_memb_read(unit, lt_ent, ent_idx, 1,
                                                     false, pt_entry_buff));

            while (ent_offset < NHOPS_PER_MEMB_ENTRY && max_paths > 0) {
                SHR_IF_ERR_EXIT
                    (bcmdrd_pt_field_set(unit,
                                         BCMECMP_TBL_DRD_SID(unit, member, gtype),
                                         pt_entry_buff,
                                         memb_flds->ecmp_nhop_id[ent_offset],
                                         &(val)));

                ent_offset++;
                max_paths--;
            }
        } else {
            max_paths -= NHOPS_PER_MEMB_ENTRY;
        }

        SHR_IF_ERR_EXIT
            (bcm56990_a0_ecmp_lt_entry_memb_write(unit, lt_ent, ent_idx, 1,
                                                  false, pt_entry_buff));

        ent_idx++;
        ent_offset = 0;
    }

exit:
    BCMECMP_FREE(pt_entry_buff);
    SHR_FUNC_EXIT();
}

static int
bcm56990_a0_ecmp_memb_table_dma_clear(int unit, bcmecmp_lt_entry_t *lt_ent,
                                      bool rh_mode)
{
    bcm56990_a0_ecmp_memb_drd_fields_t *memb_flds;
    bcmecmp_grp_type_t gtype;
    uint32_t *dma_buf = NULL;
    uint32_t dma_alloc_sz = 0, dma_buf_idx;
    uint32_t ent_size, val = 0;
    int start_idx, max_paths;
    int ent_idx, ent_offset, ent_num;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(lt_ent, SHR_E_PARAM);

    gtype = lt_ent->grp_type;

    memb_flds = (bcm56990_a0_ecmp_memb_drd_fields_t *)BCMECMP_TBL_DRD_FIELDS
                                                          (unit, member, gtype);
    SHR_NULL_CHECK(memb_flds, SHR_E_INTERNAL);

    start_idx = BCMECMP_GRP_MEMB_TBL_START_IDX(unit, lt_ent->ecmp_id);
    if (rh_mode) {
        max_paths = BCMECMP_GRP_RH_ENTRIES_CNT(unit, lt_ent->ecmp_id);
    } else {
        max_paths = BCMECMP_GRP_MAX_PATHS(unit, lt_ent->ecmp_id);
    }

    SHR_IF_ERR_EXIT
        (bcm56990_a0_ecmp_memb_ent_range_get(start_idx, max_paths, &ent_idx,
                                             &ent_offset, &ent_num));

    ent_size = BCMECMP_TBL_ENT_SIZE(unit, member, gtype);
    dma_alloc_sz = (ent_size * sizeof(uint32_t) * ent_num);
    BCMECMP_ALLOC(dma_buf, dma_alloc_sz, "bcmecmpBcm56990GrpDmaClearPtDmaBuf");

    dma_buf_idx = 0;
    start_idx = ent_idx;

    while (max_paths > 0) {
        /*
         * The group may not occupy all of four members of the first
         * and the last entry.
         */
        if (ent_offset != 0 || max_paths < NHOPS_PER_MEMB_ENTRY) {
            SHR_IF_ERR_EXIT
                (bcm56990_a0_ecmp_lt_entry_memb_read(unit, lt_ent, ent_idx, 1, false,
                                                     &dma_buf[dma_buf_idx * ent_size]));

            while (ent_offset < NHOPS_PER_MEMB_ENTRY && max_paths > 0) {
                SHR_IF_ERR_EXIT
                    (bcmdrd_pt_field_set(unit,
                                         BCMECMP_TBL_DRD_SID(unit, member, gtype),
                                         &dma_buf[dma_buf_idx * ent_size],
                                         memb_flds->ecmp_nhop_id[ent_offset],
                                         &(val)));

                ent_offset++;
                max_paths--;
            }

            ent_idx++;
            ent_offset = 0;
            dma_buf_idx++;
        } else {
            ent_offset = max_paths / NHOPS_PER_MEMB_ENTRY;

            max_paths %= NHOPS_PER_MEMB_ENTRY;
            ent_idx += ent_offset;
            dma_buf_idx += ent_offset;
            ent_offset = 0;
        }
    }

    SHR_IF_ERR_EXIT
        (bcm56990_a0_ecmp_lt_entry_memb_write(unit, lt_ent, start_idx, ent_num,
                                              true, dma_buf));

exit:
    BCMECMP_FREE(dma_buf);
    SHR_FUNC_EXIT();
}

static int
bcm56990_a0_ecmp_memb_table_pio_read(int unit, bcmecmp_lt_entry_t *lt_ent,
                                     bool rh_mode)
{
    bcm56990_a0_ecmp_memb_drd_fields_t *memb_flds;
    bcmecmp_grp_type_t gtype;
    uint32_t *pt_entry_buff = NULL;
    uint32_t *nhop_arr = NULL;
    uint32_t val;
    int nhop_arr_idx = 0, start_idx, max_paths;
    int ent_idx, ent_offset;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(lt_ent, SHR_E_PARAM);

    gtype = lt_ent->grp_type;

    memb_flds = (bcm56990_a0_ecmp_memb_drd_fields_t *)BCMECMP_TBL_DRD_FIELDS
                                                          (unit, member, gtype);
    SHR_NULL_CHECK(memb_flds, SHR_E_INTERNAL);

    BCMECMP_ALLOC(pt_entry_buff,
                  BCMECMP_TBL_ENT_SIZE_MAX(unit) * sizeof(uint32_t),
                  "bcmecmpBcm56990MembTblPioReadPtBuf");

    start_idx = lt_ent->mstart_idx;
    if (rh_mode) {
        max_paths = lt_ent->rh_entries_cnt;
        nhop_arr = (uint32_t *)lt_ent->rh_entries_arr;
    } else {
        max_paths = lt_ent->max_paths;
        nhop_arr = (uint32_t *)lt_ent->nhop_id;
    }

    SHR_IF_ERR_EXIT
        (bcm56990_a0_ecmp_memb_ent_range_get(start_idx, max_paths, &ent_idx,
                                             &ent_offset, NULL));

    nhop_arr_idx = 0;

    while (max_paths > 0) {
        sal_memset(pt_entry_buff, 0,
                   BCMECMP_TBL_ENT_SIZE_MAX(unit) * sizeof(uint32_t));

        SHR_IF_ERR_EXIT
                (bcm56990_a0_ecmp_lt_entry_memb_read(unit, lt_ent, ent_idx, 1,
                                                     false, pt_entry_buff));

        while (ent_offset < NHOPS_PER_MEMB_ENTRY && max_paths > 0) {
            SHR_IF_ERR_EXIT
                (bcmdrd_pt_field_get(unit,
                                     BCMECMP_TBL_DRD_SID(unit, member, gtype),
                                     pt_entry_buff,
                                     memb_flds->ecmp_nhop_id[ent_offset],
                                     &val));
            nhop_arr[nhop_arr_idx] = (bcmecmp_nhop_id_t)val;

            ent_offset++;
            max_paths--;
            nhop_arr_idx++;
        }

        ent_idx++;
        ent_offset = 0;
    }

exit:
    BCMECMP_FREE(pt_entry_buff);
    SHR_FUNC_EXIT();
}

static int
bcm56990_a0_ecmp_memb_table_dma_read(int unit, bcmecmp_lt_entry_t *lt_ent,
                                     bool rh_mode)
{
    bcm56990_a0_ecmp_memb_drd_fields_t *memb_flds;
    bcmecmp_grp_type_t gtype;
    uint32_t *nhop_arr = NULL;
    uint32_t *dma_buf = NULL;
    uint32_t dma_alloc_sz = 0, ent_size, val;
    int nhop_arr_idx = 0, dma_buf_idx = 0;
    int start_idx, max_paths;
    int ent_idx, ent_offset, ent_num;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(lt_ent, SHR_E_PARAM);

    gtype = lt_ent->grp_type;

    memb_flds = (bcm56990_a0_ecmp_memb_drd_fields_t *)BCMECMP_TBL_DRD_FIELDS
                                                          (unit, member, gtype);
    SHR_NULL_CHECK(memb_flds, SHR_E_INTERNAL);

    start_idx = lt_ent->mstart_idx;
    if (rh_mode) {
        max_paths = lt_ent->rh_entries_cnt;
        nhop_arr = (uint32_t *)lt_ent->rh_entries_arr;
    } else {
        max_paths = lt_ent->max_paths;
        nhop_arr = (uint32_t *)lt_ent->nhop_id;
    }

    SHR_IF_ERR_EXIT
        (bcm56990_a0_ecmp_memb_ent_range_get(start_idx, max_paths, &ent_idx,
                                             &ent_offset, &ent_num));

    ent_size = BCMECMP_TBL_ENT_SIZE(unit, member, gtype);
    dma_alloc_sz = (ent_size * sizeof(uint32_t) * ent_num);
    BCMECMP_ALLOC(dma_buf, dma_alloc_sz, "bcmecmpBcm56990GrpDmaReadPtDmaBuf");

    SHR_IF_ERR_EXIT
        (bcm56990_a0_ecmp_lt_entry_memb_read(unit, lt_ent, ent_idx, ent_num,
                                             true, dma_buf));

    nhop_arr_idx = 0;
    dma_buf_idx = 0;

    while (max_paths > 0) {
        while (ent_offset < NHOPS_PER_MEMB_ENTRY && max_paths > 0) {
            SHR_IF_ERR_EXIT
                (bcmdrd_pt_field_get(unit,
                                     BCMECMP_TBL_DRD_SID(unit, member, gtype),
                                     &dma_buf[dma_buf_idx * ent_size],
                                     memb_flds->ecmp_nhop_id[ent_offset],
                                     &val));
            nhop_arr[nhop_arr_idx] = (bcmecmp_nhop_id_t)val;

            ent_offset++;
            max_paths--;
            nhop_arr_idx++;
        }

        ent_idx++;
        ent_offset = 0;
        dma_buf_idx++;
    }

exit:
    BCMECMP_FREE(dma_buf);
    SHR_FUNC_EXIT();
}

static int
bcm56990_a0_ecmp_grp_table_write(int unit, bcmecmp_lt_entry_t *lt_ent,
                                 bool rh_mode)
{
    bcmecmp_grp_type_t gtype;
    bcm56990_a0_ecmp_grp_drd_fields_t *grp_flds;
    uint32_t *pt_entry_buff = NULL;
    uint32_t hw_npaths = 0;
    uint32_t val = 0;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(lt_ent, SHR_E_PARAM);

    gtype = lt_ent->grp_type;

    grp_flds = (bcm56990_a0_ecmp_grp_drd_fields_t *)BCMECMP_TBL_DRD_FIELDS
                                                        (unit, group, gtype);
    SHR_NULL_CHECK(grp_flds, SHR_E_INTERNAL);

    /* Allocate hardware entry buffer. */
    BCMECMP_ALLOC(pt_entry_buff,
                  BCMECMP_TBL_ENT_SIZE_MAX(unit) * sizeof(uint32_t),
                  "bcmecmpBcm56990GrpTableWritePtBuf");

    /* Set LB_MODE value. */
    if (BCMECMP_GRP_TYPE_WEIGHTED != gtype) {
        val = lt_ent->lb_mode;
    } else {
        if (BCMECMP_WEIGHTED_MODE_MEMBER_WEIGHT == lt_ent->weighted_mode) {
            val = lt_ent->weighted_size +
                  MEMBER_WEIGHT_WEIGHTED_SIZE_HW_FIELD_OFFSET;
        } else {
            val = lt_ent->weighted_size;
        }
    }
    SHR_IF_ERR_EXIT
        (bcmdrd_pt_field_set(unit,
                             BCMECMP_TBL_DRD_SID(unit, group, gtype),
                             pt_entry_buff,
                             grp_flds->lb_mode,
                             &(val)));

    /*
     * If num_paths value is non-zero, decrement it by '1'
     * as hardware count field value is zero base.
     */
    if (rh_mode) {
        hw_npaths = lt_ent->rh_size_enc;
    } else {
        if (lt_ent->num_paths) {
            if (BCMECMP_GRP_TYPE_WEIGHTED != gtype ||
                (BCMECMP_LT_GRP_IS_MEMBER_WEIGHT(unit, lt_ent) &&
                 BCMECMP_WEIGHTED_SIZE_0_127 == lt_ent->weighted_size)) {
                hw_npaths = (uint32_t) lt_ent->num_paths - 1;
            }
        }
    }

    /* Set members COUNT value. */
    SHR_IF_ERR_EXIT
        (bcmdrd_pt_field_set(unit,
                             BCMECMP_TBL_DRD_SID(unit, group, gtype),
                             pt_entry_buff,
                             grp_flds->count,
                             &(hw_npaths)));

    /* Set Group's member table base pointer. */
    val = (uint32_t)lt_ent->mstart_idx;
    SHR_IF_ERR_EXIT
        (bcmdrd_pt_field_set(unit,
                             BCMECMP_TBL_DRD_SID(unit, group, gtype),
                             pt_entry_buff,
                             grp_flds->base_ptr,
                             &(val)));

    val = (uint32_t)lt_ent->ctr_ing_eflex_action_id;
    SHR_IF_ERR_EXIT
        (bcmdrd_pt_field_set(unit,
                             BCMECMP_TBL_DRD_SID(unit, group, gtype),
                             pt_entry_buff,
                             grp_flds->flex_ctr_action_sel,
                             &(val)));

    val = (uint32_t)lt_ent->ctr_ing_eflex_object;
    SHR_IF_ERR_EXIT
        (bcmdrd_pt_field_set(unit,
                             BCMECMP_TBL_DRD_SID(unit, group, gtype),
                             pt_entry_buff,
                             grp_flds->flex_ctr_object,
                             &(val)));

    /* Write entry to L3_ECMP_GROUP table. */
    SHR_IF_ERR_EXIT
        (bcm56990_a0_ecmp_lt_entry_grp_write(unit, lt_ent, pt_entry_buff));

exit:
    BCMECMP_FREE(pt_entry_buff);
    SHR_FUNC_EXIT();
}

static int
bcm56990_a0_ecmp_grp_table_clear(int unit, bcmecmp_lt_entry_t *lt_ent,
                                 bool rh_mode)
{
    uint32_t *pt_entry_buff = NULL;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(lt_ent, SHR_E_PARAM);

    BCMECMP_ALLOC(pt_entry_buff,
                  BCMECMP_TBL_ENT_SIZE_MAX(unit) * sizeof(uint32_t),
                  "bcmecmpBcm56990GrpTableClearPtBuf");

    SHR_IF_ERR_EXIT
        (bcm56990_a0_ecmp_lt_entry_grp_write(unit, lt_ent, pt_entry_buff));

exit:
    BCMECMP_FREE(pt_entry_buff);
    SHR_FUNC_EXIT();
}

static int
bcm56990_a0_ecmp_grp_table_read(int unit, bcmecmp_lt_entry_t *lt_ent,
                                bool rh_mode)
{
    bcmecmp_grp_type_t gtype = BCMECMP_GRP_TYPE_COUNT;
    bcm56990_a0_ecmp_grp_drd_fields_t *grp_flds;
    uint32_t *pt_entry_buff = NULL;
    uint32_t num_paths[1] = {0};
    uint32_t lb_mode[1];
    uint32_t val = 0;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(lt_ent, SHR_E_PARAM);

    gtype = lt_ent->grp_type;

    grp_flds = (bcm56990_a0_ecmp_grp_drd_fields_t *)BCMECMP_TBL_DRD_FIELDS
                                                        (unit, group, gtype);
    SHR_NULL_CHECK(grp_flds, SHR_E_INTERNAL);

    BCMECMP_ALLOC(pt_entry_buff,
                  BCMECMP_TBL_ENT_SIZE_MAX(unit) * sizeof(uint32_t),
                  "bcmecmpBcm56990GrpTableReadPtBuf");

    SHR_IF_ERR_EXIT
        (bcm56990_a0_ecmp_lt_entry_grp_read(unit, lt_ent, pt_entry_buff));

    /* Get LB_MODE field value from ECMP Group table entry. */
    SHR_IF_ERR_EXIT
        (bcmdrd_pt_field_get(unit,
                             BCMECMP_TBL_DRD_SID(unit, group, gtype),
                             pt_entry_buff,
                             grp_flds->lb_mode,
                             lb_mode));
    if (BCMECMP_GRP_TYPE_WEIGHTED != gtype) {
        lt_ent->lb_mode = lb_mode[0];
    } else {
        if (lb_mode[0] == 0) {
            /* ECMP is not set to HW, for example numb_paths=0. */
            lt_ent->weighted_mode = BCMECMP_WEIGHTED_MODE_MEMBER_REPLICATION;
            lt_ent->weighted_size = BCMECMP_WEIGHTED_SIZE_256;
        } else if (lb_mode[0] >= BCMECMP_WEIGHTED_SIZE_0_127
                                + MEMBER_WEIGHT_WEIGHTED_SIZE_HW_FIELD_OFFSET) {
            lt_ent->weighted_mode = BCMECMP_WEIGHTED_MODE_MEMBER_WEIGHT;
            lt_ent->weighted_size = lb_mode[0] -
                                    MEMBER_WEIGHT_WEIGHTED_SIZE_HW_FIELD_OFFSET;
        } else {
            lt_ent->weighted_mode = BCMECMP_WEIGHTED_MODE_MEMBER_REPLICATION;
            lt_ent->weighted_size = lb_mode[0];
        }
    }

    /* Get NUM_PATHS field value from ECMP Group table entry. */
    SHR_IF_ERR_EXIT
        (bcmdrd_pt_field_get(unit,
                             BCMECMP_TBL_DRD_SID(unit, group, gtype),
                             pt_entry_buff,
                             grp_flds->count,
                             num_paths));

    if (rh_mode) {
        lt_ent->rh_entries_cnt = (1 << num_paths[0]);
    } else if (gtype == BCMECMP_GRP_TYPE_WEIGHTED &&
               lt_ent->weighted_size != BCMECMP_WEIGHTED_SIZE_0_127) {
        if (lb_mode[0] != 0) {
            lt_ent->num_paths = bcmecmp_weighted_size_convert_to_num_paths
                                    (lt_ent->weighted_size);
        } else {
            lt_ent->num_paths = 0;
        }
    } else {
        /*
         * Increment hardware NUM_PATHS value by "1" as zero-based numbering
         * is used for this "COUNT" field in the hardware entry.
         */
        lt_ent->num_paths = num_paths[0] + 1;
    }

    /* Get ECMP Member table base pointer from ECMP Group table entry. */
    SHR_IF_ERR_EXIT
        (bcmdrd_pt_field_get(unit,
                             BCMECMP_TBL_DRD_SID(unit, group, gtype),
                             pt_entry_buff,
                             grp_flds->base_ptr,
                             &val));
    lt_ent->mstart_idx = (int)val;

    SHR_IF_ERR_EXIT
        (bcmdrd_pt_field_get(unit,
                             BCMECMP_TBL_DRD_SID(unit, group, gtype),
                             pt_entry_buff,
                             grp_flds->flex_ctr_action_sel,
                             &val));
    lt_ent->ctr_ing_eflex_action_id = (int)val;

    SHR_IF_ERR_EXIT
        (bcmdrd_pt_field_get(unit,
                             BCMECMP_TBL_DRD_SID(unit, group, gtype),
                             pt_entry_buff,
                             grp_flds->flex_ctr_object,
                             &val));
    lt_ent->ctr_ing_eflex_object = (int)val;

exit:
    BCMECMP_FREE(pt_entry_buff);
    SHR_FUNC_EXIT();
}

static int
bcm56990_a0_ecmp_mw_memb_table_dma_read(int unit, bcmecmp_lt_entry_t *lt_ent)
{
    return SHR_E_UNAVAIL;
}

static int
bcm56990_a0_ecmp_mw_memb_table_dma_write(int unit, bcmecmp_lt_entry_t *lt_ent)
{
    bcmecmp_member_weight_grp_t *member_weight_grp = NULL;
    bcm56990_a0_ecmp_memb_drd_fields_t *memb_flds;
    bcmecmp_grp_type_t gtype;
    uint32_t *dma_buf = NULL;
    uint32_t dma_alloc_sz = 0, ent_size, val;
    int nhop_arr_idx = 0, start_idx;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(lt_ent, SHR_E_PARAM);

    gtype = lt_ent->grp_type;

    memb_flds = (bcm56990_a0_ecmp_memb_drd_fields_t *)BCMECMP_TBL_DRD_FIELDS
                                                          (unit, member, gtype);
    SHR_NULL_CHECK(memb_flds, SHR_E_INTERNAL);

    BCMECMP_ALLOC(member_weight_grp, sizeof(bcmecmp_member_weight_grp_t),
                  "bcmecmpBcm56990GrpDmaWritePtDmaBuf");

    BCMECMP_ALLOC(member_weight_grp->nhop_id_a,
                  (sizeof(bcmecmp_nhop_id_t) * BCMECMP_NHOP_ARRAY_SIZE),
                  "bcmecmpBcm56990GrpNHOPADmaWritePtDmaBuf");

    BCMECMP_ALLOC(member_weight_grp->nhop_id_b,
                  (sizeof(bcmecmp_nhop_id_t) * BCMECMP_NHOP_ARRAY_SIZE),
                  "bcmecmpBcm56990GrpNHOPBDmaWritePtDmaBuf");

    BCMECMP_ALLOC(member_weight_grp->weight_a,
                  (sizeof(uint32_t) * BCMECMP_NHOP_ARRAY_SIZE),
                  "bcmecmpBcm56990GrpWtADmaWritePtDmaBuf");

    BCMECMP_ALLOC(member_weight_grp->weight_b,
                  (sizeof(uint32_t) * BCMECMP_NHOP_ARRAY_SIZE),
                  "bcmecmpBcm56990GrpWtBDmaWritePtDmaBuf");

    /* Distribute weight. */
    SHR_IF_ERR_EXIT
        (bcmecmp_member_weight_distribute(unit,
                                          lt_ent->nhop_id,
                                          lt_ent->weight,
                                          lt_ent->num_paths,
                                          MEMBER_WEIGHT_QUANT_FACTOR,
                                          member_weight_grp));

    ent_size = BCMECMP_TBL_ENT_SIZE(unit, member, gtype);
    dma_alloc_sz = (ent_size * sizeof(uint32_t) * lt_ent->max_paths);
    BCMECMP_ALLOC(dma_buf, dma_alloc_sz, "bcmecmpBcm56990GrpDmaWritePtDmaBuf");

    for (nhop_arr_idx = 0; nhop_arr_idx < member_weight_grp->group_size;
         nhop_arr_idx++) {
        val = (uint32_t)member_weight_grp->nhop_id_a[nhop_arr_idx];

        SHR_IF_ERR_EXIT
            (bcmdrd_pt_field_set(unit,
                                 BCMECMP_TBL_DRD_SID(unit, member, gtype),
                                 &dma_buf[nhop_arr_idx * ent_size],
                                 memb_flds->wecmp_nhop_id[0],
                                 &(val)));

        val = (uint32_t)member_weight_grp->nhop_id_b[nhop_arr_idx];

        SHR_IF_ERR_EXIT
            (bcmdrd_pt_field_set(unit,
                                 BCMECMP_TBL_DRD_SID(unit, member, gtype),
                                 &dma_buf[nhop_arr_idx * ent_size],
                                 memb_flds->wecmp_nhop_id[1],
                                 &(val)));

        val = (uint32_t)member_weight_grp->weight_a[nhop_arr_idx] - 1;

        SHR_IF_ERR_EXIT
            (bcmdrd_pt_field_set(unit,
                                 BCMECMP_TBL_DRD_SID(unit, member, gtype),
                                 &dma_buf[nhop_arr_idx * ent_size],
                                 memb_flds->wecmp_weight,
                                 &(val)));

        val = 1;

        SHR_IF_ERR_EXIT
            (bcmdrd_pt_field_set(unit,
                                 BCMECMP_TBL_DRD_SID(unit, member, gtype),
                                 &dma_buf[nhop_arr_idx * ent_size],
                                 memb_flds->entry_type,
                                 &(val)));
    }

    start_idx = lt_ent->mstart_idx;
    SHR_IF_ERR_EXIT
        (bcm56990_a0_ecmp_lt_entry_memb_write(unit, lt_ent, start_idx,
                                              lt_ent->max_paths, true, dma_buf));

exit:
    if (member_weight_grp) {
        BCMECMP_FREE(member_weight_grp->nhop_id_a);
        BCMECMP_FREE(member_weight_grp->nhop_id_b);
        BCMECMP_FREE(member_weight_grp->weight_a);
        BCMECMP_FREE(member_weight_grp->weight_b);
    }
    BCMECMP_FREE(member_weight_grp);
    BCMECMP_FREE(dma_buf);
    SHR_FUNC_EXIT();
}

static int
bcm56990_a0_ecmp_mw_memb_table_dma_clear(int unit, bcmecmp_lt_entry_t *lt_ent)
{
    bcmecmp_grp_type_t gtype;
    uint32_t *dma_buf = NULL;
    uint32_t dma_alloc_sz = 0, ent_size;
    int start_idx, max_paths;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(lt_ent, SHR_E_PARAM);

    gtype = lt_ent->grp_type;

    start_idx = BCMECMP_GRP_MEMB_TBL_START_IDX(unit, lt_ent->ecmp_id);
    max_paths = BCMECMP_GRP_MAX_PATHS(unit, lt_ent->ecmp_id);

    ent_size = BCMECMP_TBL_ENT_SIZE(unit, member, gtype);
    dma_alloc_sz = (ent_size * sizeof(uint32_t) * max_paths);
    BCMECMP_ALLOC(dma_buf, dma_alloc_sz, "bcmecmpBcm56990GrpDmaWritePtDmaBuf");

    SHR_IF_ERR_EXIT
        (bcm56990_a0_ecmp_lt_entry_memb_write(unit, lt_ent, start_idx,
                                              max_paths, true, dma_buf));

exit:
    BCMECMP_FREE(dma_buf);
    SHR_FUNC_EXIT();
}

static int
bcm56990_a0_ecmp_mw_memb_dma_cpy(int unit,
                                 bcmecmp_grp_defrag_t *dfrag,
                                 bcmecmp_grp_defrag_t *sfrag)
{
    bcmecmp_grp_type_t gtype;
    bcmecmp_wal_info_t *wali = NULL;
    uint32_t *dma_buf = NULL;
    uint32_t dma_alloc_sz = 0, ent_size;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(sfrag, SHR_E_PARAM);
    SHR_NULL_CHECK(dfrag, SHR_E_PARAM);

    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "%s.\n"), __func__));

    /* Get and validate ECMP WALI pointer. */
    wali = BCMECMP_WALI_PTR(unit);
    SHR_NULL_CHECK(wali, SHR_E_INTERNAL);

    gtype = sfrag->gtype;
    ent_size = BCMECMP_TBL_ENT_SIZE(unit, member, gtype);

    dma_alloc_sz = (ent_size * sizeof(uint32_t) * sfrag->gsize);
    BCMECMP_ALLOC(dma_buf, dma_alloc_sz, "bcmecmp56990L3EcmpDmaCpy");

    /* Read from source fragment. */
    SHR_IF_ERR_EXIT
        (bcm56990_a0_ecmp_dfrag_memb_read(unit, sfrag, sfrag->mstart_idx,
                                          sfrag->gsize, true, dma_buf));


    /* Update WALI member table entries READ info. */
    if (wali->mwsize == 0) {
        wali->mwsize = BCMECMP_TBL_ENT_SIZE(unit, member, gtype);
    }
    wali->mr_ecount += sfrag->gsize;

    /* Write to destination fragment. */
    SHR_IF_ERR_EXIT
        (bcm56990_a0_ecmp_dfrag_memb_write(unit, dfrag, dfrag->mstart_idx,
                                           dfrag->gsize, true, dma_buf));

    /* Update WALI member table entries write count. */
    wali->mw_ecount += dfrag->gsize;

    BCMECMP_FREE(dma_buf);

exit:
    if (SHR_FUNC_ERR()) {
        BCMECMP_FREE(dma_buf);
    }
    SHR_FUNC_EXIT();
}

static int
bcm56990_a0_ecmp_mw_memb_dma_clr(int unit, bcmecmp_grp_defrag_t *frag)
{
    bcmecmp_grp_type_t gtype;
    bcmecmp_wal_info_t *wali = NULL;
    uint32_t *dma_buf = NULL;
    uint32_t dma_alloc_sz = 0, ent_size;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(frag, SHR_E_PARAM);

    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "%s.\n"), __func__));

    /* Get and validate ECMP WALI pointer. */
    wali = BCMECMP_WALI_PTR(unit);
    SHR_NULL_CHECK(wali, SHR_E_INTERNAL);

    gtype = frag->gtype;
    ent_size = BCMECMP_TBL_ENT_SIZE(unit, member, gtype);

    dma_alloc_sz = (ent_size * sizeof(uint32_t) * frag->gsize);
    BCMECMP_ALLOC(dma_buf, dma_alloc_sz, "bcmecmp56990L3EcmpDmaCpy");

    /* Write to destination fragment. */
    SHR_IF_ERR_EXIT
        (bcm56990_a0_ecmp_dfrag_memb_write(unit, frag, frag->mstart_idx, frag->gsize,
                                           true, dma_buf));

    /* Update WALI member table entries write count. */
    wali->mw_ecount += frag->gsize;

    BCMECMP_FREE(dma_buf);

exit:
    if (SHR_FUNC_ERR()) {
        BCMECMP_FREE(dma_buf);
    }
    SHR_FUNC_EXIT();
}

static int
bcm56990_a0_ecmp_grp_ins(int unit, void *info)
{
    bcmecmp_lt_entry_t *lt_ent = NULL;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(info, SHR_E_PARAM);

    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit,
                         "bcm56990_a0_ecmp_grp_ins.\n")));

    /* Get LT entry data pointer. */
    lt_ent = (bcmecmp_lt_entry_t *)info;

    /* Program ECMP Member table. */
    if (BCMECMP_USE_DMA(unit)) {
        if (BCMECMP_LT_GRP_IS_MEMBER_WEIGHT(unit, lt_ent)) {
            SHR_IF_ERR_EXIT
                (bcm56990_a0_ecmp_mw_memb_table_dma_write(unit, lt_ent));
        } else {
            SHR_IF_ERR_EXIT
                (bcm56990_a0_ecmp_memb_table_dma_write(unit, lt_ent, false));
        }
    } else {
        if (BCMECMP_LT_GRP_IS_MEMBER_WEIGHT(unit, lt_ent)) {
            SHR_ERR_EXIT(SHR_E_UNAVAIL);
        } else {
            SHR_IF_ERR_EXIT
                (bcm56990_a0_ecmp_memb_table_pio_write(unit, lt_ent, false));
        }
    }

    /* Program ECMP Group table. */
    if (lt_ent->num_paths != 0) {
        SHR_IF_ERR_EXIT
            (bcm56990_a0_ecmp_grp_table_write(unit, lt_ent, false));
    } else {
        SHR_IF_ERR_EXIT
            (bcm56990_a0_ecmp_grp_table_clear(unit, lt_ent, false));
    }

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56990_a0_ecmp_grp_del(int unit, void *info)
{
    bcmecmp_lt_entry_t *lt_ent = NULL;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(info, SHR_E_PARAM);

    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit,
                         "bcm56990_a0_ecmp_grp_del.\n")));

    /* Get LT entry data pointer. */
    lt_ent = (bcmecmp_lt_entry_t *)info;

    /* Program ECMP Member table. */
    if (BCMECMP_USE_DMA(unit)) {
        if (BCMECMP_GRP_IS_MEMBER_WEIGHT(unit, lt_ent->ecmp_id)) {
            SHR_IF_ERR_EXIT
                (bcm56990_a0_ecmp_mw_memb_table_dma_clear(unit, lt_ent));
        } else {
            SHR_IF_ERR_EXIT
                (bcm56990_a0_ecmp_memb_table_dma_clear(unit, lt_ent, false));
        }
    } else {
        if (BCMECMP_GRP_IS_MEMBER_WEIGHT(unit, lt_ent->ecmp_id)) {
            SHR_ERR_EXIT(SHR_E_UNAVAIL);
        } else {
            SHR_IF_ERR_EXIT
                (bcm56990_a0_ecmp_memb_table_pio_clear(unit, lt_ent, false));
        }
    }

    /* Program ECMP Group table. */
    SHR_IF_ERR_EXIT
        (bcm56990_a0_ecmp_grp_table_clear(unit, lt_ent, false));

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56990_a0_ecmp_grp_get(int unit, void *info)
{
    bcmecmp_lt_entry_t *lt_ent = NULL;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(info, SHR_E_PARAM);

    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit,
                         "bcm56990_a0_ecmp_grp_get.\n")));

    /* Get LT entry data pointer. */
    lt_ent = (bcmecmp_lt_entry_t *)info;

    /* Program ECMP Group table. */
    SHR_IF_ERR_EXIT
        (bcm56990_a0_ecmp_grp_table_read(unit, lt_ent, false));

    /* Program ECMP Member table. */
    if (BCMECMP_USE_DMA(unit)) {
        if (BCMECMP_LT_GRP_IS_MEMBER_WEIGHT(unit, lt_ent)) {
            SHR_IF_ERR_EXIT
                (bcm56990_a0_ecmp_mw_memb_table_dma_read(unit, lt_ent));
        } else {
            SHR_IF_ERR_EXIT
                (bcm56990_a0_ecmp_memb_table_dma_read(unit, lt_ent, false));
        }
    } else {
        if (BCMECMP_LT_GRP_IS_MEMBER_WEIGHT(unit, lt_ent)) {
            SHR_ERR_EXIT(SHR_E_UNAVAIL);
        } else {
            SHR_IF_ERR_EXIT
                (bcm56990_a0_ecmp_memb_table_pio_read(unit, lt_ent, false));
        }
    }

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56990_a0_ecmp_rh_grp_ins(int unit, void *info)
{
    bcmecmp_lt_entry_t *lt_ent = NULL;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(info, SHR_E_PARAM);

    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit,
                         "bcm56990_a0_ecmp_rh_grp_ins.\n")));

    /* Get LT entry data pointer. */
    lt_ent = (bcmecmp_lt_entry_t *)info;

    /* Program ECMP Member table. */
    if (BCMECMP_USE_DMA(unit)) {
        SHR_IF_ERR_EXIT
            (bcm56990_a0_ecmp_memb_table_dma_write(unit, lt_ent, true));
    } else {
        SHR_IF_ERR_EXIT
            (bcm56990_a0_ecmp_memb_table_pio_write(unit, lt_ent, true));
    }

    /* Program ECMP Group table. */
    if (lt_ent->rh_num_paths != 0) {
        SHR_IF_ERR_EXIT
            (bcm56990_a0_ecmp_grp_table_write(unit, lt_ent, true));
    } else {
        SHR_IF_ERR_EXIT
            (bcm56990_a0_ecmp_grp_table_clear(unit, lt_ent, true));
    }

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56990_a0_ecmp_rh_grp_del(int unit, void *info)
{
    bcmecmp_lt_entry_t *lt_ent = NULL;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(info, SHR_E_PARAM);

    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit,
                         "bcm56990_a0_ecmp_rh_grp_del.\n")));

    /* Get LT entry data pointer. */
    lt_ent = (bcmecmp_lt_entry_t *)info;

    /* Program ECMP Member table. */
    if (BCMECMP_USE_DMA(unit)) {
        SHR_IF_ERR_EXIT
            (bcm56990_a0_ecmp_memb_table_dma_clear(unit, lt_ent, true));
    } else {
        SHR_IF_ERR_EXIT
            (bcm56990_a0_ecmp_memb_table_pio_clear(unit, lt_ent, true));
    }

    /* Program ECMP Group table. */
    SHR_IF_ERR_EXIT
        (bcm56990_a0_ecmp_grp_table_clear(unit, lt_ent, true));

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56990_a0_ecmp_rh_grp_get(int unit, void *info)
{
    bcmecmp_lt_entry_t *lt_ent = NULL;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(info, SHR_E_PARAM);

    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit,
                         "bcm56990_a0_ecmp_rh_grp_get.\n")));

    /* Get LT entry data pointer. */
    lt_ent = (bcmecmp_lt_entry_t *)info;

    /* Program ECMP Group table. */
    SHR_IF_ERR_EXIT
        (bcm56990_a0_ecmp_grp_table_read(unit, lt_ent, true));

    /* Program ECMP Member table. */
    if (BCMECMP_USE_DMA(unit)) {
        SHR_IF_ERR_EXIT
            (bcm56990_a0_ecmp_memb_table_dma_read(unit, lt_ent, true));
    } else {
        SHR_IF_ERR_EXIT
            (bcm56990_a0_ecmp_memb_table_pio_read(unit, lt_ent, true));
    }

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56990_a0_ecmp_memb_dma_cpy(int unit,
                              bcmecmp_grp_defrag_t *dfrag,
                              bcmecmp_grp_defrag_t *sfrag)
{
    bcm56990_a0_ecmp_memb_drd_fields_t *memb_flds;
    bcmecmp_grp_type_t gtype;
    bcmecmp_wal_info_t *wali = NULL;
    bcmecmp_nhop_id_t *nhop_arr = NULL;
    uint32_t *dma_buf = NULL;
    uint32_t dma_alloc_sz = 0, ent_size, val;
    int nhop_arr_idx = 0, dma_buf_idx = 0;
    int start_idx, max_paths;
    int ent_idx, ent_offset, ent_num;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(sfrag, SHR_E_PARAM);
    SHR_NULL_CHECK(dfrag, SHR_E_PARAM);

    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "%s.\n"), __func__));

    /* Get and validate ECMP WALI pointer. */
    wali = BCMECMP_WALI_PTR(unit);
    SHR_NULL_CHECK(wali, SHR_E_INTERNAL);

    gtype = sfrag->gtype;
    ent_size = BCMECMP_TBL_ENT_SIZE(unit, member, gtype);

    memb_flds = (bcm56990_a0_ecmp_memb_drd_fields_t *)BCMECMP_TBL_DRD_FIELDS
                                                          (unit, member, gtype);
    SHR_NULL_CHECK(memb_flds, SHR_E_INTERNAL);

    BCMECMP_ALLOC(nhop_arr, sizeof(bcmecmp_nhop_id_t) * sfrag->gsize,
                  "bcmecmp56990L3EcmpDmaCpy");

    /*
     * One case of source and destination:
     * The source occupies three entries, starting from the third member of
     * the first entry:
     *   the first entry:  - - + +
     *   the second entry: + + + +
     *   the third entry:  + + - -
     * The destination occupies two entries:
     *   the first entry:  + + + +
     *   the second entry: + + + +
     */

    /*
     * Read from source fragment.
     */

    start_idx = sfrag->mstart_idx;
    max_paths = sfrag->gsize;

    SHR_IF_ERR_EXIT
        (bcm56990_a0_ecmp_memb_ent_range_get(start_idx, max_paths, &ent_idx,
                                             &ent_offset, &ent_num));

    dma_alloc_sz = (ent_size * sizeof(uint32_t) * ent_num);
    BCMECMP_ALLOC(dma_buf, dma_alloc_sz, "bcmecmp56990L3EcmpDmaCpy");

    SHR_IF_ERR_EXIT
        (bcm56990_a0_ecmp_dfrag_memb_read(unit, sfrag, ent_idx, ent_num,
                                          true, dma_buf));

    nhop_arr_idx = 0;
    dma_buf_idx = 0;

    while (max_paths > 0) {
        while (ent_offset < NHOPS_PER_MEMB_ENTRY && max_paths > 0) {
            SHR_IF_ERR_EXIT
                (bcmdrd_pt_field_get(unit,
                                     BCMECMP_TBL_DRD_SID(unit, member, gtype),
                                     &dma_buf[dma_buf_idx * ent_size],
                                     memb_flds->ecmp_nhop_id[ent_offset],
                                     &val));
            nhop_arr[nhop_arr_idx] = (bcmecmp_nhop_id_t)val;

            ent_offset++;
            max_paths--;
            nhop_arr_idx++;
        }

        ent_idx++;
        ent_offset = 0;
        dma_buf_idx++;
    }

    BCMECMP_FREE(dma_buf);

    /* Update WALI member table entries READ info. */
    if (wali->mwsize == 0) {
        wali->mwsize = BCMECMP_TBL_ENT_SIZE(unit, member, gtype);
    }
    wali->mr_ecount += ent_num;

    /*
     * Write to destination fragment.
     */

    start_idx = dfrag->mstart_idx;
    max_paths = dfrag->gsize;

    SHR_IF_ERR_EXIT
        (bcm56990_a0_ecmp_memb_ent_range_get(start_idx, max_paths, &ent_idx,
                                             &ent_offset, &ent_num));

    dma_alloc_sz = (ent_size * sizeof(uint32_t) * ent_num);
    BCMECMP_ALLOC(dma_buf, dma_alloc_sz, "bcmecmp56990L3EcmpDmaCpy");

    nhop_arr_idx = 0;
    dma_buf_idx = 0;
    start_idx = ent_idx;

    while (max_paths > 0) {
        /*
         * The group may not occupy all of four members of the first
         * and the last entry.
         */
        if (ent_offset != 0 || max_paths < NHOPS_PER_MEMB_ENTRY) {
            SHR_IF_ERR_EXIT
                (bcm56990_a0_ecmp_dfrag_memb_read(unit, dfrag, ent_idx, 1, false,
                                                  &dma_buf[dma_buf_idx * ent_size]));
        }

        while (ent_offset < NHOPS_PER_MEMB_ENTRY && max_paths > 0) {
            val = (uint32_t)nhop_arr[nhop_arr_idx];

            SHR_IF_ERR_EXIT
                (bcmdrd_pt_field_set(unit,
                                     BCMECMP_TBL_DRD_SID(unit, member, gtype),
                                     &dma_buf[dma_buf_idx * ent_size],
                                     memb_flds->ecmp_nhop_id[ent_offset],
                                     &(val)));

            ent_offset++;
            nhop_arr_idx++;
            max_paths--;
        }

        ent_idx++;
        ent_offset = 0;
        dma_buf_idx++;
    }

    SHR_IF_ERR_EXIT
        (bcm56990_a0_ecmp_dfrag_memb_write(unit, dfrag, start_idx, ent_num,
                                           true, dma_buf));

    BCMECMP_FREE(dma_buf);

    /* Update WALI member table entries write count. */
    wali->mw_ecount += ent_num;

exit:
    if (SHR_FUNC_ERR()) {
        BCMECMP_FREE(dma_buf);
    }
    BCMECMP_FREE(nhop_arr);
    SHR_FUNC_EXIT();
}

static int
bcm56990_a0_ecmp_memb_cpy(int unit,
                          const void *mdest,
                          const void *msrc)
{
    bcmecmp_grp_defrag_t sfrag, dfrag;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(mdest, SHR_E_PARAM);
    SHR_NULL_CHECK(msrc, SHR_E_PARAM);

    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "%s.\n"), __func__));

    sal_memcpy(&sfrag, msrc, sizeof(sfrag));
    sal_memcpy(&dfrag, mdest, sizeof(dfrag));

    if (BCMECMP_USE_DMA(unit)) {
        if (BCMECMP_GRP_IS_MEMBER_WEIGHT(unit, sfrag.ecmp_id)) {
            SHR_IF_ERR_EXIT
                (bcm56990_a0_ecmp_mw_memb_dma_cpy(unit, &dfrag, &sfrag));
        } else {
            SHR_IF_ERR_EXIT
                (bcm56990_a0_ecmp_memb_dma_cpy(unit, &dfrag, &sfrag));
        }
    } else {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56990_a0_ecmp_memb_dma_clr(int unit, bcmecmp_grp_defrag_t *mclr)
{
    bcm56990_a0_ecmp_memb_drd_fields_t *memb_flds;
    bcmecmp_grp_type_t gtype;
    bcmecmp_wal_info_t *wali = NULL;
    uint32_t *dma_buf = NULL;
    uint32_t dma_alloc_sz = 0, ent_size, val = 0;
    int dma_buf_idx = 0, start_idx, max_paths;
    int ent_idx, ent_offset, ent_num;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(mclr, SHR_E_PARAM);

    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "bcm56990_a0_ecmp_memb_dma_clr.\n")));

    /* Get and validate ECMP WALI pointer. */
    wali = BCMECMP_WALI_PTR(unit);
    SHR_NULL_CHECK(wali, SHR_E_INTERNAL);

    gtype = mclr->gtype;
    ent_size = BCMECMP_TBL_ENT_SIZE(unit, member, gtype);

    memb_flds = (bcm56990_a0_ecmp_memb_drd_fields_t *)BCMECMP_TBL_DRD_FIELDS
                                                          (unit, member, gtype);
    SHR_NULL_CHECK(memb_flds, SHR_E_INTERNAL);

    start_idx = mclr->mstart_idx;
    max_paths = mclr->gsize;

    SHR_IF_ERR_EXIT
        (bcm56990_a0_ecmp_memb_ent_range_get(start_idx, max_paths, &ent_idx,
                                             &ent_offset, &ent_num));

    dma_alloc_sz = (ent_size * sizeof(uint32_t) * ent_num);
    BCMECMP_ALLOC(dma_buf, dma_alloc_sz, "bcmecmp56990L3EcmpDmaClr");

    dma_buf_idx = 0;
    start_idx = ent_idx;

    while (max_paths > 0) {
        /*
         * The group may not occupy all of four members of the first
         * and the last entry.
         */
        if (ent_offset != 0 || max_paths < NHOPS_PER_MEMB_ENTRY) {
            SHR_IF_ERR_EXIT
                (bcm56990_a0_ecmp_dfrag_memb_read(unit, mclr, ent_idx, 1, false,
                                                  &dma_buf[dma_buf_idx * ent_size]));

            while (ent_offset < NHOPS_PER_MEMB_ENTRY && max_paths > 0) {
                SHR_IF_ERR_EXIT
                    (bcmdrd_pt_field_set(unit,
                                         BCMECMP_TBL_DRD_SID(unit, member, gtype),
                                         &dma_buf[dma_buf_idx * ent_size],
                                         memb_flds->ecmp_nhop_id[ent_offset],
                                         &(val)));

                ent_offset++;
                max_paths--;
            }

            ent_idx++;
            ent_offset = 0;
            dma_buf_idx++;
        } else {
            ent_offset = max_paths / NHOPS_PER_MEMB_ENTRY;

            max_paths %= NHOPS_PER_MEMB_ENTRY;
            ent_idx += ent_offset;
            dma_buf_idx += ent_offset;
            ent_offset = 0;
        }
    }

    SHR_IF_ERR_EXIT
        (bcm56990_a0_ecmp_dfrag_memb_write(unit, mclr, start_idx, ent_num,
                                           true, dma_buf));

    /* Update WALI member table entries write count. */
    wali->mclr_ecount += ent_num;

exit:
    BCMECMP_FREE(dma_buf);
    SHR_FUNC_EXIT();
}

static int
bcm56990_a0_ecmp_memb_clr(int unit, const void *mfrag)
{
    bcmecmp_grp_defrag_t my_frag;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(mfrag, SHR_E_PARAM);

    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "%s.\n"), __func__));

    sal_memcpy(&my_frag, mfrag, sizeof(my_frag));

    if (BCMECMP_USE_DMA(unit)) {
        if (BCMECMP_GRP_IS_MEMBER_WEIGHT(unit, my_frag.ecmp_id)) {
            SHR_IF_ERR_EXIT
                (bcm56990_a0_ecmp_mw_memb_dma_clr(unit, &my_frag));
        } else {
            SHR_IF_ERR_EXIT
                (bcm56990_a0_ecmp_memb_dma_clr(unit, &my_frag));
        }
    } else {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

    exit:
        SHR_FUNC_EXIT();
}

static int
bcm56990_a0_ecmp_gmstart_updt(int unit, const void *ginfo)
{
    bcmecmp_grp_defrag_t *gupdt = (bcmecmp_grp_defrag_t *)ginfo;
    bcmecmp_grp_type_t gtype;
    bcmecmp_id_t ecmp_id = BCMECMP_INVALID;
    bcmecmp_grp_drd_fields_t *grp_flds;
    bcmecmp_wal_info_t *wali = NULL;
    uint32_t *pt_entry_buff = NULL;
    uint32_t val = 0;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(gupdt, SHR_E_PARAM);

    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "%s.\n"), __func__));

    /* Get and validate ECMP WALI pointer. */
    wali = BCMECMP_WALI_PTR(unit);
    SHR_NULL_CHECK(wali, SHR_E_INTERNAL);

    /* Get the ECMP group type and validate it. */
    gtype = gupdt->gtype;
    if (gtype < BCMECMP_GRP_TYPE_SINGLE || gtype > BCMECMP_GRP_TYPE_WEIGHTED) {
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "Invalid GTYPE=%d.\n"), gtype));
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    /* Get the ECMP_ID and validate it. */
    ecmp_id = gupdt->ecmp_id;
    if (ecmp_id < BCMECMP_LT_MIN_ECMP_ID(unit, group, gtype)
        || ecmp_id > BCMECMP_LT_MAX_ECMP_ID(unit, group, gtype)) {
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "Invalid ECMP_ID=%d.\n"), ecmp_id));
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    /* Allocate hardware entry buffer. */
    BCMECMP_ALLOC(pt_entry_buff,
                  BCMECMP_TBL_ENT_SIZE_MAX(unit) * sizeof(uint32_t),
                  "bcmecmp56990GrpMstartUpdt");

    SHR_IF_ERR_EXIT
        (bcm56990_a0_ecmp_dfrag_grp_read(unit, gupdt, pt_entry_buff));

    if (wali->gwsize == 0) {
        wali->gwsize = BCMECMP_TBL_ENT_SIZE(unit, group, gtype);
    }
    wali->gr_ecount += 1;

    grp_flds = (bcmecmp_grp_drd_fields_t *)BCMECMP_TBL_DRD_FIELDS
                                                (unit, group, gtype);
    SHR_NULL_CHECK(grp_flds, SHR_E_INTERNAL);

    val = (uint32_t)gupdt->mstart_idx;
    SHR_IF_ERR_EXIT
        (bcmdrd_pt_field_set(unit,
                             BCMECMP_TBL_DRD_SID(unit, group, gtype),
                             pt_entry_buff,
                             grp_flds->base_ptr,
                             &(val)));

    SHR_IF_ERR_EXIT
        (bcm56990_a0_ecmp_dfrag_grp_write(unit, gupdt, pt_entry_buff));

    wali->gw_ecount += 1;

exit:
    BCMECMP_FREE(pt_entry_buff);
    SHR_FUNC_EXIT();
}

static int
bcm56990_a0_ecmp_member_weight_index_width(void)
{
    return NHOPS_PER_MEMB_ENTRY;
}

/*!
 * \brief Get the group type of shared HW mem partition.
 *
 * This function gets the group type of the ITBM list sharing the same HW
 * partition, but with different reverse param.
 *
 * \param [in] gtype group type.
 *
 * \return group type.
 */
static bcmecmp_grp_type_t
bcm56990_a0_ecmp_share_group_type_get(bcmecmp_grp_type_t gtype)
{
    if (gtype == BCMECMP_GRP_TYPE_SINGLE) {
        return BCMECMP_GRP_TYPE_WEIGHTED;
    } else if (gtype == BCMECMP_GRP_TYPE_WEIGHTED) {
        return BCMECMP_GRP_TYPE_SINGLE;
    }

    return BCMECMP_GRP_TYPE_COUNT;
}

/*******************************************************************************
 * Public functions
 */
/*!
 * \brief Attach device ECMP driver functions.
 *
 * Attach device ECMP driver functions.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 */
int
bcm56990_a0_ecmp_attach(int unit)
{
    bcmecmp_drv_t *ecmp_drv = bcmecmp_drv_get(unit);

    SHR_FUNC_ENTER(unit);

    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit,
                         "bcm56990_a0_ecmp_attach.\n")));

    SHR_NULL_CHECK(ecmp_drv, SHR_E_FAIL);

    ecmp_ctrl_local[unit] = bcmecmp_get_ecmp_control(unit);

    BCMECMP_USE_INDEX_REMAP(unit) = FALSE;

    ecmp_ctrl_local[unit]->ecmp_info.itbm = true;

    /* Initialize device ECMP feature function pointers. */
    BCMECMP_FNCALL_PTR(unit) = ecmp_drv;

    /* Setup device ECMP component initialization function. */
    BCMECMP_FNCALL(unit, info_init) = &bcm56990_a0_ecmp_info_init;

    /* Setup device ECMP component clean-up function. */
    BCMECMP_FNCALL(unit, info_cleanup) = &bcm56990_a0_ecmp_info_cleanup;

    /* Setup device ECMP resolution mode hardware config function. */
    BCMECMP_FNCALL(unit, mode_config) = &bcm56990_a0_ecmp_mode_config;

    /* Initialize device ECMP group management function pointers. */
    BCMECMP_FNCALL(unit, grp_ins) = &bcm56990_a0_ecmp_grp_ins;
    BCMECMP_FNCALL(unit, grp_del) = &bcm56990_a0_ecmp_grp_del;
    BCMECMP_FNCALL(unit, grp_get) = &bcm56990_a0_ecmp_grp_get;
    BCMECMP_FNCALL(unit, rh_grp_ins) = &bcm56990_a0_ecmp_rh_grp_ins;
    BCMECMP_FNCALL(unit, rh_grp_del) = &bcm56990_a0_ecmp_rh_grp_del;
    BCMECMP_FNCALL(unit, rh_grp_get) = &bcm56990_a0_ecmp_rh_grp_get;
    BCMECMP_FNCALL(unit, rh_pre_config) = &bcmecmp_rh_groups_preconfig_with_itbm;
    BCMECMP_FNCALL(unit, memb_cpy) = &bcm56990_a0_ecmp_memb_cpy;
    BCMECMP_FNCALL(unit, memb_clr) = &bcm56990_a0_ecmp_memb_clr;
    BCMECMP_FNCALL(unit, gmstart_updt) = &bcm56990_a0_ecmp_gmstart_updt;
    BCMECMP_FNCALL(unit, member_weight_index_width) =
        &bcm56990_a0_ecmp_member_weight_index_width;
    BCMECMP_FNCALL(unit, grp_ol_lt_fields_fill) =
                            &bcmecmp_overlay_grp_lt_fields_fill;
    BCMECMP_FNCALL(unit, grp_ul_lt_fields_fill) =
                            &bcmecmp_underlay_grp_lt_fields_fill;
    BCMECMP_FNCALL(unit, grp_member_fields_count_get) = NULL;
    BCMECMP_FNCALL(unit, grp_member_fields_count_get) = NULL;

    ecmp_drv->shr_grp_type_get = &bcm56990_a0_ecmp_share_group_type_get;
    ecmp_drv->weight_size_to_max_paths = NULL;
    ecmp_drv->itbm_memb_lists_update = NULL;
    ecmp_drv->memb_bank_set= NULL;
    ecmp_drv->memb_bank_get = NULL;

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Detach device ECMP driver functions.
 *
 * Detach device ECMP driver functions.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 */
int
bcm56990_a0_ecmp_detach(int unit)
{
    bcmecmp_drv_t *ecmp_drv = bcmecmp_drv_get(unit);

    SHR_FUNC_ENTER(unit);

    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit,
                         "bcm56990_a0_ecmp_detach.\n")));

    if (BCMECMP_FNCALL_PTR(unit) && ecmp_drv) {
        /* Clear ECMP group management function pointers. */
        BCMECMP_FNCALL(unit, grp_get) = NULL;
        BCMECMP_FNCALL(unit, grp_del) = NULL;
        BCMECMP_FNCALL(unit, grp_ins) = NULL;
        BCMECMP_FNCALL(unit, rh_grp_ins) = NULL;
        BCMECMP_FNCALL(unit, rh_grp_del) = NULL;
        BCMECMP_FNCALL(unit, rh_grp_get) = NULL;
        BCMECMP_FNCALL(unit, rh_pre_config) = NULL;
        BCMECMP_FNCALL(unit, memb_cpy) = NULL;
        BCMECMP_FNCALL(unit, memb_clr) = NULL;
        BCMECMP_FNCALL(unit, gmstart_updt) = NULL;
        BCMECMP_FNCALL(unit, member_weight_index_width) = NULL;
        BCMECMP_FNCALL(unit, grp_ol_lt_fields_fill) = NULL;
        BCMECMP_FNCALL(unit, grp_ul_lt_fields_fill) = NULL;
        ecmp_drv->shr_grp_type_get = NULL;

        /* Clear device ECMP feature function pointers. */
        BCMECMP_FNCALL(unit, mode_config) = NULL;
        BCMECMP_FNCALL(unit, info_cleanup) = NULL;
        BCMECMP_FNCALL(unit, info_init) = NULL;
        BCMECMP_FNCALL_PTR(unit) = NULL;
    }

    ecmp_ctrl_local[unit] = NULL;

    SHR_FUNC_EXIT();
}
