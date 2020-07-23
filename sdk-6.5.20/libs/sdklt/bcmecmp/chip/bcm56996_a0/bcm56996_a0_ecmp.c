/*! \file bcm56996_a0_ecmp.c
 *
 * This file implements BCM56996_A0 device specific ECMP functions.
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
#include <bcmdrd/chip/bcm56996_a0_enum.h>
#include <bcmdrd/chip/bcm56996_a0_defs.h>
#include <bcmdrd/bcmdrd_pt.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmlrd/bcmlrd_map.h>
#include <bcmlrd/bcmlrd_table.h>
#include <bcmmgmt/bcmmgmt_sysm.h>
#include <bcmecmp/bcmecmp_db_internal.h>
#include <bcmecmp/bcmecmp_group_util.h>
#include <bcmecmp/bcmecmp_member_weight.h>

/*******************************************************************************
 * Defines
 */
#define BSL_LOG_MODULE BSL_LS_BCMLTX_ECMP

/*! ECMP members per hardware member table entry. */
#define NHOPS_PER_MEMB_ENTRY (4)

/*! Overlay ECMP Members per hardware member table entry. */
#define NHOPS_PER_MEMB_ENTRY_OVERLAY (2)

/*! WECMP Members per hardware member table entry. */
#define NHOPS_PER_MEMB_ENTRY_WECMP (1)

#define MEMBER_WEIGHT_QUANT_FACTOR (4096)

#define OVERLAY_GRP_ID_OFFSET (4096)

/*! Total number of member table per chip. */
#define BANK_NUM_PER_CHIP 4

/*! Total number of hardware entries per bank. */
#define HW_ENTRIES_PER_BANK 4096

/*! Logical members count for overlay ECMP group. */
#define OVERLAY_MEMB_COUNT 4

/*! Default number of member table banks for overlay ECMP. */
#define DEFAULT_NUM_O_MEMB_BANK 0

/*! Entry type of hardware member table for weighted underlay ECMP. */
#define MEMB_WEIGHTED_ECMP_ENTRY_TYPE 1

/*! Entry type of hardware member table for overlay ECMP. */
#define MEMB_HECMP_ENTRY_TYPE 2

/*! Entry type of hardware member table for weighted overlay ECMP. */
#define MEMB_WEIGHTED_HECMP_ENTRY_TYPE 3

/*!
 * \brief underlay weighted ECMP group size.
 */
typedef enum bcm56996_a0_weighted_size_e {
    BCM56996_WEIGHTED_SIZE_0_511 = 2,
    BCM56996_WEIGHTED_SIZE_1K = 5,
    BCM56996_WEIGHTED_SIZE_2K = 6,
    BCM56996_WEIGHTED_SIZE_4K = 7,
    BCM56996_WEIGHTED_SIZE_COUNT
} bcm56996_a0_weighted_size_t;

/*!
 * \brief overlay weighted ECMP group size.
 */
typedef enum bcm56996_a0_ovlay_weighted_size_e {
    BCM56996_OVLAY_WEIGHTED_SIZE_0_127 = 3,
    BCM56996_OVLAY_WEIGHTED_SIZE_COUNT
} bcm56996_a0_ovlay_weighted_size_t;

/*!
 * MEMBER_REPLICATION and MEMBER_WEIGHTED use the same WEIGHTED_SIZE value
 * in LT, and this WEIGHTED_SIZE is the real hardware value of
 * MEMBER_REPLICATION.
 * For MEMBER_WEIGHT, use this offset plus WEIGHTED_SIZE to get the real
 * hardware value.
 */
#define MEMBER_WEIGHT_WEIGHTED_SIZE_HW_FIELD_OFFSET (6)

typedef struct bcm56996_a0_ecmp_memb_drd_fields_s {
    bcmdrd_fid_t entry_type;
    bcmdrd_fid_t wecmp_weight;
    bcmdrd_fid_t wecmp_nhop_id[2];
    bcmdrd_fid_t wecmp_o_nhop_id[2];
    bcmdrd_fid_t wecmp_u_ecmp_nhop[2];
    bcmdrd_fid_t wecmp_u_ecmp_id[2];
    bcmdrd_fid_t o_nhop_id[2];
    bcmdrd_fid_t u_ecmp_nhop[2];
    bcmdrd_fid_t u_ecmp_id[2];
    bcmdrd_fid_t u_nhop_id[2];
    bcmdrd_fid_t ecmp_nhop_id[4];
} bcm56996_a0_ecmp_memb_drd_fields_t;

typedef struct bcm56996_a0_ecmp_grp_drd_fields_s {
    bcmdrd_fid_t lb_mode;

    bcmdrd_fid_t base_ptr;
    bcmdrd_fid_t count;

    bcmdrd_fid_t flex_ctr_object;
    bcmdrd_fid_t flex_ctr_action_sel;
} bcm56996_a0_ecmp_grp_drd_fields_t;

/*!
 * \brief ECMP Member list type.
 */
typedef enum bcmecmp_memb_list_e {
    BCMECMP_MEMB_LIST_ULAY = 0,
    BCMECMP_MEMB_LIST_ULAY_WEIGHT,
    BCMECMP_MEMB_LIST_OLAY,
    BCMECMP_MEMB_LIST_OLAY_WEIGHT,
    BCMECMP_MEMB_LIST_COUNT
} bcmecmp_memb_list_t;


/*******************************************************************************
 * Local definitions
 */
static bcmecmp_control_t *ecmp_ctrl_local[BCMECMP_NUM_UNITS_MAX] = {NULL};
/*******************************************************************************
 * Private functions
 */
static void
bcm56996_a0_ecmp_grp_drd_fields_t_init(bcm56996_a0_ecmp_grp_drd_fields_t *gdrd)
{
    if (gdrd) {
        sal_memset(gdrd, INVALIDf, sizeof(*gdrd));
    }
    return;
}

static void
bcm56996_a0_ecmp_memb_drd_fields_t_init(bcm56996_a0_ecmp_memb_drd_fields_t *mdrd)
{
    if (mdrd) {
        sal_memset(mdrd, INVALIDf, sizeof(*mdrd));
    }
    return;
}

static void
bcm56996_a0_ecmp_grp_ltd_fields_t_init(bcmecmp_grp_ltd_fields_t *gltd)
{
    if (gltd) {
        sal_memset(gltd, BCMLTD_SID_INVALID, sizeof(*gltd));
    }
    return;
}

static void
bcm56996_a0_ecmp_lt_field_attrs_t_init(bcmecmp_lt_field_attrs_t *fattrs)
{
    if (fattrs) {
        sal_memset(fattrs, BCMECMP_INVALID, sizeof(*fattrs));
    }
    return;
}

static void
bcm56996_a0_ecmp_pt_op_info_t_init(bcmecmp_pt_op_info_t *pt_op_info)
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
bcm56996_a0_ecmp_lt_entry_memb_write(int unit, bcmecmp_lt_entry_t *lt_ent,
                                     int ent_idx, int ent_num, bool dma_en,
                                     uint32_t *buf)
{
    bcmecmp_pt_op_info_t op_info;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(lt_ent, SHR_E_PARAM);
    SHR_NULL_CHECK(buf, SHR_E_PARAM);

    bcm56996_a0_ecmp_pt_op_info_t_init(&op_info);
    op_info.op = BCMPTM_OP_WRITE;
    op_info.op_arg = lt_ent->op_arg;
    op_info.req_lt_sid = lt_ent->glt_sid;
    op_info.drd_sid = BCMECMP_TBL_DRD_SID(unit, member, lt_ent->grp_type);
    op_info.wsize = BCMECMP_TBL_ENT_SIZE(unit, member, lt_ent->grp_type);
    op_info.dyn_info.index = ent_idx;
    op_info.ecount = ent_num;
    op_info.dma = dma_en;

    SHR_IF_ERR_EXIT
        (bcmecmp_pt_write(unit, &op_info, buf));

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56996_a0_ecmp_lt_entry_memb_read(int unit, bcmecmp_lt_entry_t *lt_ent,
                                    int ent_idx, int ent_num, bool dma_en,
                                    uint32_t *buf)
{
    bcmecmp_pt_op_info_t op_info;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(lt_ent, SHR_E_PARAM);
    SHR_NULL_CHECK(buf, SHR_E_PARAM);

    bcm56996_a0_ecmp_pt_op_info_t_init(&op_info);
    op_info.op = BCMPTM_OP_READ;
    op_info.op_arg = lt_ent->op_arg;
    op_info.req_lt_sid = lt_ent->glt_sid;
    op_info.drd_sid = BCMECMP_TBL_DRD_SID(unit, member, lt_ent->grp_type);
    op_info.wsize = BCMECMP_TBL_ENT_SIZE(unit, member, lt_ent->grp_type) * ent_num;
    op_info.dyn_info.index = ent_idx;
    op_info.ecount = ent_num;
    op_info.dma = dma_en;

    SHR_IF_ERR_EXIT
        (bcmecmp_pt_read(unit, &op_info, buf));

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56996_a0_ecmp_lt_entry_grp_write(int unit, bcmecmp_lt_entry_t *lt_ent,
                                    uint32_t *buf)
{
    bcmecmp_pt_op_info_t op_info;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(lt_ent, SHR_E_PARAM);
    SHR_NULL_CHECK(buf, SHR_E_PARAM);

    bcm56996_a0_ecmp_pt_op_info_t_init(&op_info);
    op_info.op = BCMPTM_OP_WRITE;
    op_info.op_arg = lt_ent->op_arg;
    op_info.req_lt_sid = lt_ent->glt_sid;
    op_info.drd_sid = BCMECMP_TBL_DRD_SID(unit, group, lt_ent->grp_type);
    op_info.wsize = BCMECMP_TBL_ENT_SIZE(unit, group, lt_ent->grp_type);
    if (bcmecmp_grp_is_overlay(lt_ent->grp_type)) {
        op_info.dyn_info.index = lt_ent->ecmp_id - OVERLAY_GRP_ID_OFFSET;
    } else {
        op_info.dyn_info.index = lt_ent->ecmp_id;
    }

    SHR_IF_ERR_EXIT
        (bcmecmp_pt_write(unit, &op_info, buf));

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56996_a0_ecmp_lt_entry_grp_read(int unit, bcmecmp_lt_entry_t *lt_ent,
                                   uint32_t *buf)
{
    bcmecmp_pt_op_info_t op_info;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(lt_ent, SHR_E_PARAM);
    SHR_NULL_CHECK(buf, SHR_E_PARAM);

    bcm56996_a0_ecmp_pt_op_info_t_init(&op_info);
    op_info.op = BCMPTM_OP_READ;
    op_info.op_arg = lt_ent->op_arg;
    op_info.req_lt_sid = lt_ent->glt_sid;
    op_info.drd_sid = BCMECMP_TBL_DRD_SID(unit, group, lt_ent->grp_type);
    op_info.wsize = BCMECMP_TBL_ENT_SIZE(unit, group, lt_ent->grp_type);
    if (bcmecmp_grp_is_overlay(lt_ent->grp_type)) {
        op_info.dyn_info.index = lt_ent->ecmp_id - OVERLAY_GRP_ID_OFFSET;
    } else {
        op_info.dyn_info.index = lt_ent->ecmp_id;
    }

    SHR_IF_ERR_EXIT
        (bcmecmp_pt_read(unit, &op_info, buf));

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56996_a0_ecmp_dfrag_memb_write(int unit, bcmecmp_grp_defrag_t *dfrag,
                                  int ent_idx, int ent_num, bool dma_en,
                                  uint32_t *buf)
{
    bcmecmp_pt_op_info_t op_info;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(dfrag, SHR_E_PARAM);
    SHR_NULL_CHECK(buf, SHR_E_PARAM);

    bcm56996_a0_ecmp_pt_op_info_t_init(&op_info);
    op_info.op =BCMPTM_OP_WRITE;
    op_info.op_arg = dfrag->op_arg;
    op_info.req_lt_sid = dfrag->glt_sid;
    op_info.drd_sid = BCMECMP_TBL_DRD_SID(unit, member, dfrag->gtype);
    op_info.wsize = BCMECMP_TBL_ENT_SIZE(unit, member, dfrag->gtype);
    op_info.dyn_info.index = ent_idx;
    op_info.ecount = ent_num;
    op_info.dma = dma_en;

    SHR_IF_ERR_EXIT
        (bcmecmp_pt_write(unit, &op_info, buf));

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56996_a0_ecmp_dfrag_memb_read(int unit, bcmecmp_grp_defrag_t *dfrag,
                                 int ent_idx, int ent_num, bool dma_en,
                                 uint32_t *buf)
{
    bcmecmp_pt_op_info_t op_info;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(dfrag, SHR_E_PARAM);
    SHR_NULL_CHECK(buf, SHR_E_PARAM);

    bcm56996_a0_ecmp_pt_op_info_t_init(&op_info);
    op_info.op =BCMPTM_OP_READ;
    op_info.op_arg = dfrag->op_arg;
    op_info.req_lt_sid = dfrag->glt_sid;
    op_info.drd_sid = BCMECMP_TBL_DRD_SID(unit, member, dfrag->gtype);
    op_info.wsize = BCMECMP_TBL_ENT_SIZE(unit, member, dfrag->gtype) * ent_num;
    op_info.dyn_info.index = ent_idx;
    op_info.ecount = ent_num;
    op_info.dma = dma_en;

    SHR_IF_ERR_EXIT
        (bcmecmp_pt_read(unit, &op_info, buf));

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56996_a0_ecmp_dfrag_grp_write(int unit, bcmecmp_grp_defrag_t *dfrag,
                                 uint32_t *buf)
{
    bcmecmp_pt_op_info_t op_info;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(dfrag, SHR_E_PARAM);
    SHR_NULL_CHECK(buf, SHR_E_PARAM);

    bcm56996_a0_ecmp_pt_op_info_t_init(&op_info);
    op_info.op = BCMPTM_OP_WRITE;
    op_info.op_arg = dfrag->op_arg;
    op_info.req_lt_sid = dfrag->glt_sid;
    op_info.drd_sid = BCMECMP_TBL_DRD_SID(unit, group, dfrag->gtype);
    op_info.wsize = BCMECMP_TBL_ENT_SIZE(unit, group, dfrag->gtype);
    if (bcmecmp_grp_is_overlay(dfrag->gtype)) {
        op_info.dyn_info.index = dfrag->ecmp_id - OVERLAY_GRP_ID_OFFSET;
    } else {
        op_info.dyn_info.index = dfrag->ecmp_id;
    }

    SHR_IF_ERR_EXIT
        (bcmecmp_pt_write(unit, &op_info, buf));

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56996_a0_ecmp_dfrag_grp_read(int unit, bcmecmp_grp_defrag_t *dfrag,
                                uint32_t *buf)
{
    bcmecmp_pt_op_info_t op_info;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(dfrag, SHR_E_PARAM);
    SHR_NULL_CHECK(buf, SHR_E_PARAM);

    bcm56996_a0_ecmp_pt_op_info_t_init(&op_info);
    op_info.op = BCMPTM_OP_READ;
    op_info.op_arg = dfrag->op_arg;
    op_info.req_lt_sid = dfrag->glt_sid;
    op_info.drd_sid = BCMECMP_TBL_DRD_SID(unit, group, dfrag->gtype);
    op_info.wsize = BCMECMP_TBL_ENT_SIZE(unit, group, dfrag->gtype);
    op_info.dyn_info.index = dfrag->ecmp_id;
    if (bcmecmp_grp_is_overlay(dfrag->gtype)) {
        op_info.dyn_info.index = dfrag->ecmp_id - OVERLAY_GRP_ID_OFFSET;
    } else {
        op_info.dyn_info.index = dfrag->ecmp_id;
    }

    SHR_IF_ERR_EXIT
        (bcmecmp_pt_read(unit, &op_info, buf));

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56996_a0_ecmp_info_print(int unit)
{
    shr_pb_t *pb; /* Print buffer. */

    SHR_FUNC_ENTER(unit);

    pb = shr_pb_create();
    shr_pb_printf(pb,
                  "\n\tECMP INFO - (Unit = %d, ECMP_MODE = %s):\n",
                  unit,
                  ((BCMECMP_MODE(unit) == BCMECMP_MODE_HIER) ? "HIER" :
                   "INVALID"));
    switch (BCMECMP_MODE(unit)) {
        case BCMECMP_MODE_HIER:
            shr_pb_printf(pb,
                          "\n\t============== Underlay ==============\n"
                          "\tL3_ECMP_COUNTm (Grp)         : wsize = %d\n"
                          "\tL3_ECMPm (Memb)              : wsize = %d\n"
                          "\tECMP Tbls max entry sz       : wsize = %d\n"
                          "\tGrp  : idx_min = %d, idx_max = %d, tbl_sz = %d\n"
                          "\tMemb : idx_min = %d, idx_max = %d, tbl_sz = %d\n"
                          "\t\tECMP_ID      : min = %d, max = %d\n"
                          "\t\tLB_MODE      : min = %d, max = %d\n"
                          "\t\tMAX_PATHS    : min = %d, max = %d\n"
                          "\t\tNUM_PATHS    : min = %d, max = %d\n"
                          "\t\tNHOP_ID      : min = %d, max = %d\n"
                          "\t\tRH_SIZE      : min = %d, max = %d\n",
                          BCMECMP_TBL_ENT_SIZE(unit, group,
                                               BCMECMP_GRP_TYPE_UNDERLAY),
                          BCMECMP_TBL_ENT_SIZE(unit, member,
                                               BCMECMP_GRP_TYPE_UNDERLAY),
                          BCMECMP_TBL_ENT_SIZE_MAX(unit),
                          BCMECMP_TBL_IDX_MIN(unit, group,
                                              BCMECMP_GRP_TYPE_UNDERLAY),
                          BCMECMP_TBL_IDX_MAX(unit, group,
                                              BCMECMP_GRP_TYPE_UNDERLAY),
                          BCMECMP_TBL_SIZE(unit, group,
                                           BCMECMP_GRP_TYPE_UNDERLAY),
                          BCMECMP_TBL_IDX_MIN(unit, member,
                                              BCMECMP_GRP_TYPE_UNDERLAY),
                          BCMECMP_TBL_IDX_MAX(unit, member,
                                              BCMECMP_GRP_TYPE_UNDERLAY),
                          BCMECMP_TBL_SIZE(unit, member,
                                           BCMECMP_GRP_TYPE_UNDERLAY),
                          BCMECMP_LT_MIN_ECMP_ID(unit, group,
                                                 BCMECMP_GRP_TYPE_UNDERLAY),
                          BCMECMP_LT_MAX_ECMP_ID(unit, group,
                                                 BCMECMP_GRP_TYPE_UNDERLAY),
                          BCMECMP_LT_MIN_LB_MODE(unit, group,
                                                 BCMECMP_GRP_TYPE_UNDERLAY),
                          BCMECMP_LT_MAX_LB_MODE(unit, group,
                                                 BCMECMP_GRP_TYPE_UNDERLAY),
                          BCMECMP_LT_MIN_PATHS(unit, group,
                                               BCMECMP_GRP_TYPE_UNDERLAY),
                          BCMECMP_LT_MAX_PATHS(unit, group,
                                               BCMECMP_GRP_TYPE_UNDERLAY),
                          BCMECMP_LT_MIN_NPATHS(unit, group,
                                                BCMECMP_GRP_TYPE_UNDERLAY),
                          BCMECMP_LT_MAX_NPATHS(unit, group,
                                                BCMECMP_GRP_TYPE_UNDERLAY),
                          BCMECMP_LT_MIN_NHOP_ID(unit, group,
                                                 BCMECMP_GRP_TYPE_UNDERLAY),
                          BCMECMP_LT_MAX_NHOP_ID(unit, group,
                                                 BCMECMP_GRP_TYPE_UNDERLAY),
                          BCMECMP_LT_MIN_RH_SIZE(unit, group,
                                                 BCMECMP_GRP_TYPE_UNDERLAY),
                          BCMECMP_LT_MAX_RH_SIZE(unit, group,
                                                 BCMECMP_GRP_TYPE_UNDERLAY));
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
bcm56996_a0_ecmp_sids_init(int unit)
{
    SHR_FUNC_ENTER(unit);

    /* Overlay DRD and LTD SIDs. */
    BCMECMP_TBL_DRD_SID
        (unit, group, BCMECMP_GRP_TYPE_OVERLAY) = L3_ECMP_GROUP_HIERARCHICALm;
    BCMECMP_TBL_DRD_SID
        (unit, member, BCMECMP_GRP_TYPE_OVERLAY) = L3_ECMPm;
    BCMECMP_TBL_LTD_SID
        (unit, group, BCMECMP_GRP_TYPE_OVERLAY) = ECMP_OVERLAYt;

    /* Overlay weighted DRD and LTD SIDs. */
    BCMECMP_TBL_DRD_SID
        (unit, group, BCMECMP_GRP_TYPE_OVERLAY_WEIGHTED) =
                                                    L3_ECMP_GROUP_HIERARCHICALm;
    BCMECMP_TBL_DRD_SID
        (unit, member, BCMECMP_GRP_TYPE_OVERLAY_WEIGHTED) = L3_ECMPm;
    BCMECMP_TBL_LTD_SID
        (unit, group, BCMECMP_GRP_TYPE_OVERLAY_WEIGHTED) =
                                                         ECMP_WEIGHTED_OVERLAYt;

    /* Underlay DRD and LTD SIDs. */
    BCMECMP_TBL_DRD_SID
        (unit, group, BCMECMP_GRP_TYPE_UNDERLAY) = L3_ECMP_COUNTm;
    BCMECMP_TBL_DRD_SID
        (unit, member, BCMECMP_GRP_TYPE_UNDERLAY) = L3_ECMPm;
    BCMECMP_TBL_LTD_SID
        (unit, group, BCMECMP_GRP_TYPE_UNDERLAY) = ECMP_UNDERLAYt;

    /* Underlay weighted DRD and LTD SIDs. */
    BCMECMP_TBL_DRD_SID
        (unit, group, BCMECMP_GRP_TYPE_UNDERLAY_WEIGHTED) = L3_ECMP_COUNTm;
    BCMECMP_TBL_DRD_SID
        (unit, member, BCMECMP_GRP_TYPE_UNDERLAY_WEIGHTED) = L3_ECMPm;
    BCMECMP_TBL_LTD_SID
        (unit, group, BCMECMP_GRP_TYPE_UNDERLAY_WEIGHTED) =
                                                        ECMP_WEIGHTED_UNDERLAYt;

    /* Initialize device ECMP config register DRD and LTD SID values. */
    BCMECMP_REG_DRD_SID(unit, config) = BCMLTD_SID_INVALID;
    BCMECMP_REG_LTD_SID(unit, config) = BCMLTD_SID_INVALID;

    /* Initialize Full-Flat ECMP resolution tables DRD SID values. */
    BCMECMP_TBL_DRD_SID
        (unit, group, BCMECMP_GRP_TYPE_SINGLE) = INVALIDm;

    /* Initialize Full-Flat ECMP member table DRD_SID value. */
    BCMECMP_TBL_DRD_SID
        (unit, member, BCMECMP_GRP_TYPE_SINGLE) = INVALIDm;

    /* Initialize Full-Flat ECMP logical table SID value. */
    BCMECMP_TBL_LTD_SID
        (unit, group, BCMECMP_GRP_TYPE_SINGLE) = INVALIDm;

     /* Initialize weighted ECMP resolution tables DRD SID values. */
    BCMECMP_TBL_DRD_SID
        (unit, group, BCMECMP_GRP_TYPE_WEIGHTED) = INVALIDm;

    /* Initialize weighted ECMP member table DRD_SID value. */
    BCMECMP_TBL_DRD_SID
        (unit, member, BCMECMP_GRP_TYPE_WEIGHTED) = INVALIDm;

    /* Initialize weighted ECMP logical table SID value. */
    BCMECMP_TBL_LTD_SID
        (unit, group, BCMECMP_GRP_TYPE_WEIGHTED) = INVALIDm;

    SHR_FUNC_EXIT();
}

static int
bcm56996_a0_ecmp_sids_clear(int unit)
{
    SHR_FUNC_ENTER(unit);

    /* Clear device ECMP config register DRD and LTD SID values. */
    BCMECMP_REG_DRD_SID(unit, config) = INVALIDr;
    BCMECMP_REG_LTD_SID(unit, config) = BCMLTD_SID_INVALID;

    switch (BCMECMP_MODE(unit)) {
        case BCMECMP_MODE_HIER:
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

            /* TH4G device does not support Pre-IFP ECMP tables. */
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
bcm56996_a0_ecmp_memb_bank_decode(int unit,
                                  bcmecmp_limit_t *l3_bank)
{
   int count = 0;

   if (l3_bank->num_o_memb_bank_valid) {
        SHR_BITCOUNT_RANGE(l3_bank->memb_bank_bitmap,
                           count,
                           0,
                           BANK_NUM_PER_CHIP);
        l3_bank->num_o_memb_bank = count;
    }

    return SHR_E_NONE;
}

static int
bcm56996_a0_ecmp_ovlay_bank_read(int unit, uint8_t *num_o_memb_bank)
{
    L3_ECMP_BANK_CONFIGr_t memb_bank_buf;
    bcmecmp_limit_t l3_bank;

    SHR_FUNC_ENTER(unit);

    L3_ECMP_BANK_CONFIGr_CLR(memb_bank_buf);
    SHR_IF_ERR_EXIT
        (bcmecmp_pt_ireq_read(unit, ECMP_LIMIT_CONTROLt,
                              L3_ECMP_BANK_CONFIGr, 0, &memb_bank_buf));
    l3_bank.memb_bank_bitmap[0] =
                                L3_ECMP_BANK_CONFIGr_BITMAPf_GET(memb_bank_buf);
    l3_bank.num_o_memb_bank_valid = 1;
    bcm56996_a0_ecmp_memb_bank_decode(unit, &l3_bank);

    *num_o_memb_bank = l3_bank.num_o_memb_bank;

exit:
     SHR_FUNC_EXIT();

}

static void
bcm56996_a0_ecmp_hw_memb_tbls_range_init(int unit, uint8_t num_o_memb_bank)
{
    int memb_imin;  /* ECMP Member hardware table Min index value. */
    int memb_imax;  /* ECMP Member hardware table Max index value. */
    int memb_size;
    int o_memb_imin;
    int o_memb_imax;
    int u_memb_imin;
    int u_memb_imax;

    /* Get ECMP Member HW table Min and Max index ranges from DRD. */
    memb_imin = bcmdrd_pt_index_min(unit,
                                    BCMECMP_TBL_DRD_SID(unit, member,
                                                    BCMECMP_GRP_TYPE_UNDERLAY));
    memb_imax = bcmdrd_pt_index_max(unit,
                                    BCMECMP_TBL_DRD_SID(unit, member,
                                                    BCMECMP_GRP_TYPE_UNDERLAY));

    u_memb_imin = memb_imin;
    u_memb_imax = (memb_imax * (BANK_NUM_PER_CHIP - num_o_memb_bank))
                  / BANK_NUM_PER_CHIP;
    o_memb_imin = num_o_memb_bank ? (u_memb_imax + 1) : BCMECMP_INVALID;
    o_memb_imax = num_o_memb_bank ? memb_imax : BCMECMP_INVALID;

    /*
     * ECMP Member HW Table Min & Max index, Size and Min-index used
     * status initialization.
     */
    memb_size = (u_memb_imax - u_memb_imin + 1) * NHOPS_PER_MEMB_ENTRY;
    BCMECMP_TBL_IDX_MIN
        (unit, member, BCMECMP_GRP_TYPE_UNDERLAY) = u_memb_imin;
    BCMECMP_TBL_IDX_MAX
        (unit, member, BCMECMP_GRP_TYPE_UNDERLAY) = u_memb_imax;
    BCMECMP_TBL_SIZE
        (unit, member,
         BCMECMP_GRP_TYPE_UNDERLAY) = memb_size;
    BCMECMP_TBL_IDX_MAX_USED
        (unit, member, BCMECMP_GRP_TYPE_UNDERLAY) = u_memb_imax;

    BCMECMP_TBL_IDX_MIN
        (unit, member, BCMECMP_GRP_TYPE_UNDERLAY_WEIGHTED) = u_memb_imin;
    BCMECMP_TBL_IDX_MAX
        (unit, member, BCMECMP_GRP_TYPE_UNDERLAY_WEIGHTED) = u_memb_imax;
    /* Member replication max index is 64K, Member weight 16K. */
    memb_size = (u_memb_imax - u_memb_imin + 1) * NHOPS_PER_MEMB_ENTRY_WECMP;
    BCMECMP_TBL_SIZE
        (unit, member,
         BCMECMP_GRP_TYPE_UNDERLAY_WEIGHTED) = memb_size;
    BCMECMP_TBL_IDX_MAX_USED
        (unit, member, BCMECMP_GRP_TYPE_UNDERLAY_WEIGHTED) = u_memb_imin;

    /* Overlay member table patition is not allocated as default. */
    BCMECMP_TBL_IDX_MIN
        (unit, member, BCMECMP_GRP_TYPE_OVERLAY) = o_memb_imin;
    BCMECMP_TBL_IDX_MAX
        (unit, member, BCMECMP_GRP_TYPE_OVERLAY) = o_memb_imax;
    if (num_o_memb_bank) {
        memb_size =
                 (o_memb_imax - o_memb_imin + 1) * NHOPS_PER_MEMB_ENTRY_OVERLAY;
    } else {
        memb_size = 0;
    }
    BCMECMP_TBL_SIZE
        (unit, member,
         BCMECMP_GRP_TYPE_OVERLAY) = memb_size;
    BCMECMP_TBL_IDX_MAX_USED
        (unit, member, BCMECMP_GRP_TYPE_OVERLAY) = o_memb_imin;

    BCMECMP_TBL_IDX_MIN
        (unit, member, BCMECMP_GRP_TYPE_OVERLAY_WEIGHTED) = o_memb_imin;
    BCMECMP_TBL_IDX_MAX
        (unit, member, BCMECMP_GRP_TYPE_OVERLAY_WEIGHTED) = o_memb_imax;
    if (num_o_memb_bank) {
        memb_size =
                 (o_memb_imax - o_memb_imin + 1) * NHOPS_PER_MEMB_ENTRY_WECMP;
    } else {
        memb_size = 0;
    }
    BCMECMP_TBL_SIZE
        (unit, member,
         BCMECMP_GRP_TYPE_OVERLAY_WEIGHTED) = memb_size;
    BCMECMP_TBL_IDX_MAX_USED
        (unit, member, BCMECMP_GRP_TYPE_OVERLAY_WEIGHTED) = o_memb_imin;

    return;
}

static int
bcm56996_a0_ecmp_hw_tbls_range_init(int unit, bool warm)
{
    uint32_t wsize; /* Hardware table entry size in words. */
    int grp_imin;   /* ECMP Group hardware table Min index value. */
    int grp_imax;   /* ECMP Group hardware table Max index value. */
    uint8_t num_o_memb_bank = DEFAULT_NUM_O_MEMB_BANK;

    SHR_FUNC_ENTER(unit);

    /* TH4G device does not support ECMP config register. */
    BCMECMP_REG_ENT_SIZE(unit, config) = 0;

    BCMECMP_TBL_ENT_SIZE_MAX(unit) = 0;

    /*
     * Get Post-IFP ECMP group table HW entry size (in words) from
     * DRD.
     */
    wsize = bcmdrd_pt_entry_wsize(unit,
                                  BCMECMP_TBL_DRD_SID(unit, group,
                                                     BCMECMP_GRP_TYPE_OVERLAY));
    BCMECMP_TBL_ENT_SIZE
        (unit, group, BCMECMP_GRP_TYPE_OVERLAY) = wsize;
    if (wsize > BCMECMP_TBL_ENT_SIZE_MAX(unit)) {
        BCMECMP_TBL_ENT_SIZE_MAX(unit) = wsize;
    }

    BCMECMP_TBL_ENT_SIZE(unit, group,
                         BCMECMP_GRP_TYPE_OVERLAY_WEIGHTED) = wsize;

    wsize = bcmdrd_pt_entry_wsize(unit,
                                  BCMECMP_TBL_DRD_SID(unit, group,
                                                    BCMECMP_GRP_TYPE_UNDERLAY));
    BCMECMP_TBL_ENT_SIZE
        (unit, group, BCMECMP_GRP_TYPE_UNDERLAY) = wsize;
    if (wsize > BCMECMP_TBL_ENT_SIZE_MAX(unit)) {
        BCMECMP_TBL_ENT_SIZE_MAX(unit) = wsize;
    }

    BCMECMP_TBL_ENT_SIZE(unit, group,
                         BCMECMP_GRP_TYPE_UNDERLAY_WEIGHTED) = wsize;

    /*
     * Get Post-IFP ECMP member table HW entry size (in words) from
     * DRD.
     */
    wsize = bcmdrd_pt_entry_wsize(unit,
                                  BCMECMP_TBL_DRD_SID(unit, member,
                                                     BCMECMP_GRP_TYPE_OVERLAY));
    BCMECMP_TBL_ENT_SIZE
        (unit, member, BCMECMP_GRP_TYPE_OVERLAY) = wsize;
    if (wsize > BCMECMP_TBL_ENT_SIZE_MAX(unit)) {
        BCMECMP_TBL_ENT_SIZE_MAX(unit) = wsize;
    }

    BCMECMP_TBL_ENT_SIZE(unit, member,
                         BCMECMP_GRP_TYPE_OVERLAY_WEIGHTED) = wsize;
    BCMECMP_TBL_ENT_SIZE(unit, member,
                         BCMECMP_GRP_TYPE_UNDERLAY) = wsize;
    BCMECMP_TBL_ENT_SIZE(unit, member,
                         BCMECMP_GRP_TYPE_UNDERLAY_WEIGHTED) = wsize;

    /* Get ECMP Group HW table Min and Max index ranges from DRD. */
    grp_imin = bcmdrd_pt_index_min
                   (unit, BCMECMP_TBL_DRD_SID(unit, group,
                                              BCMECMP_GRP_TYPE_UNDERLAY));
    grp_imax = bcmdrd_pt_index_max
                   (unit, BCMECMP_TBL_DRD_SID(unit, group,
                                              BCMECMP_GRP_TYPE_UNDERLAY));

    /*
     * ECMP Group HW Table Min & Max index, Size and Min-index used
     * status initialization.
     */
    BCMECMP_TBL_IDX_MIN
        (unit, group, BCMECMP_GRP_TYPE_UNDERLAY) = grp_imin;
    BCMECMP_TBL_IDX_MAX
        (unit, group, BCMECMP_GRP_TYPE_UNDERLAY) = grp_imax;
    BCMECMP_TBL_SIZE
        (unit, group,
         BCMECMP_GRP_TYPE_UNDERLAY) = grp_imax - grp_imin + 1;
    BCMECMP_TBL_IDX_MAX_USED
        (unit, group, BCMECMP_GRP_TYPE_UNDERLAY) = grp_imin;

    BCMECMP_TBL_IDX_MIN
        (unit, group, BCMECMP_GRP_TYPE_UNDERLAY_WEIGHTED) = grp_imin;
    BCMECMP_TBL_IDX_MAX
        (unit, group, BCMECMP_GRP_TYPE_UNDERLAY_WEIGHTED) = grp_imax;
    BCMECMP_TBL_SIZE
        (unit, group,
         BCMECMP_GRP_TYPE_UNDERLAY_WEIGHTED) = grp_imax - grp_imin + 1;
    BCMECMP_TBL_IDX_MAX_USED
        (unit, group, BCMECMP_GRP_TYPE_UNDERLAY_WEIGHTED) = grp_imin;

    /* Get ECMP Group HW table Min and Max index ranges from DRD. */
    grp_imin = bcmdrd_pt_index_min
                   (unit, BCMECMP_TBL_DRD_SID(unit, group,
                                              BCMECMP_GRP_TYPE_OVERLAY));
    grp_imax = bcmdrd_pt_index_max
                   (unit, BCMECMP_TBL_DRD_SID(unit, group,
                                              BCMECMP_GRP_TYPE_OVERLAY));

    BCMECMP_TBL_IDX_MIN
        (unit, group, BCMECMP_GRP_TYPE_OVERLAY) = grp_imin;
    BCMECMP_TBL_IDX_MAX
        (unit, group, BCMECMP_GRP_TYPE_OVERLAY) = grp_imax;
    BCMECMP_TBL_SIZE
        (unit, group,
         BCMECMP_GRP_TYPE_OVERLAY) = grp_imax - grp_imin + 1;
    BCMECMP_TBL_IDX_MAX_USED
        (unit, group, BCMECMP_GRP_TYPE_OVERLAY) = grp_imin;

    BCMECMP_TBL_IDX_MIN
        (unit, group, BCMECMP_GRP_TYPE_OVERLAY_WEIGHTED) = grp_imin;
    BCMECMP_TBL_IDX_MAX
        (unit, group, BCMECMP_GRP_TYPE_OVERLAY_WEIGHTED) = grp_imax;
    BCMECMP_TBL_SIZE
        (unit, group,
         BCMECMP_GRP_TYPE_OVERLAY_WEIGHTED) = grp_imax - grp_imin + 1;
    BCMECMP_TBL_IDX_MAX_USED
        (unit, group, BCMECMP_GRP_TYPE_OVERLAY_WEIGHTED) = grp_imin;

    if (warm) {
        SHR_IF_ERR_EXIT
            (bcm56996_a0_ecmp_ovlay_bank_read(unit, &num_o_memb_bank));
    }
    bcm56996_a0_ecmp_hw_memb_tbls_range_init(unit, num_o_memb_bank);

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56996_a0_ecmp_hw_tbls_range_clear(int unit)
{
    SHR_FUNC_ENTER(unit);

    /* Clear device ECMP config register entry size in words value. */
    BCMECMP_REG_ENT_SIZE(unit, config) = 0;

    /* Clear Max hardware tables entry size info. */
    BCMECMP_TBL_ENT_SIZE_MAX(unit) = 0;

    switch (BCMECMP_MODE(unit)) {
        case BCMECMP_MODE_HIER:
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
            SHR_ERR_EXIT(SHR_E_NONE);
    }

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56996_a0_ecmp_overlay_lt_flds_init(int unit,
                                      const bcmlrd_map_t *map)
{
    bcm56996_a0_ecmp_grp_drd_fields_t *gdrd_fields = NULL;  /* Group tbl DRD info. */
    bcm56996_a0_ecmp_memb_drd_fields_t *mdrd_fields = NULL; /* Member tbl DRD info. */
    bcmecmp_grp_ltd_fields_t *lt_fids = NULL;   /* LT entry field IDs. */
    bcmecmp_lt_field_attrs_t *grp_attrs = NULL; /* overlay LT field attrs. */
    uint32_t idx;    /* Enum symbol array index. */
    uint32_t lb_val; /* LB mode value. */
    const bcmlrd_field_data_t *fdata = NULL; /* LT field data map. */
    const char *const lb_name[BCMECMP_LB_MODE_COUNT] = BCMECMP_LB_MODE_NAME;
    bcmecmp_info_t *ecmp_info = NULL;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(map, SHR_E_INTERNAL);

    /* Allocate memory for storing Member table entry fields DRD ID values. */
    BCMECMP_ALLOC(mdrd_fields, sizeof(*mdrd_fields), "bcmecmpOverlayMdrdFlds");
    ecmp_info = &(ecmp_ctrl_local[unit]->ecmp_info);
    ecmp_info->member[BCMECMP_GRP_TYPE_OVERLAY].drd_fids = (void *)mdrd_fields;
    /* TH4G device does not support Pre-IFP ECMP tables. */
    ecmp_info->init_member[BCMECMP_GRP_TYPE_OVERLAY].drd_fids = NULL;

    /* Initialize Member table entry fields DRD ID values. */
    bcm56996_a0_ecmp_memb_drd_fields_t_init(mdrd_fields);
    mdrd_fields->entry_type = ENTRY_TYPEf;
    mdrd_fields->o_nhop_id[0] = HIERARCHICAL_ECMPv_O_NEXT_HOP_INDEX_0f;
    mdrd_fields->o_nhop_id[1] = HIERARCHICAL_ECMPv_O_NEXT_HOP_INDEX_1f;
    mdrd_fields->u_ecmp_nhop[0] = HIERARCHICAL_ECMPv_U_NHI_OR_ECMP_GROUP_0f;
    mdrd_fields->u_ecmp_nhop[1] = HIERARCHICAL_ECMPv_U_NHI_OR_ECMP_GROUP_1f;;
    mdrd_fields->u_ecmp_id[0] = HIERARCHICAL_ECMPv_U_ECMP_GROUP_0f;
    mdrd_fields->u_ecmp_id[1] = HIERARCHICAL_ECMPv_U_ECMP_GROUP_1f;
    mdrd_fields->u_nhop_id[0] = HIERARCHICAL_ECMPv_U_NEXT_HOP_INDEX_0f;
    mdrd_fields->u_nhop_id[1] = HIERARCHICAL_ECMPv_U_NEXT_HOP_INDEX_1f;

    /* Allocate memory for storing Group table entry fields DRD ID values. */
    BCMECMP_ALLOC(gdrd_fields, sizeof(*gdrd_fields), "bcmecmpOverlayGdrdFlds");
    ecmp_info->group[BCMECMP_GRP_TYPE_OVERLAY].drd_fids = (void *)gdrd_fields;
    /* TH4G device does not support Pre-IFP ECMP tables. */
    ecmp_info->init_group[BCMECMP_GRP_TYPE_OVERLAY].drd_fids = NULL;

    /* Initialize Group table entry fields DRD ID values. */
    bcm56996_a0_ecmp_grp_drd_fields_t_init(gdrd_fields);
    gdrd_fields->count = COUNTf;
    gdrd_fields->base_ptr = BASE_PTRf;
    gdrd_fields->lb_mode = LB_MODEf;
    gdrd_fields->flex_ctr_object = FLEX_CTR_OBJECTf;
    gdrd_fields->flex_ctr_action_sel = FLEX_CTR_ACTION_SELf;

    /*
     * Allocate memory for storing ECMP LT logical
     * fields Min, Max attribute values.
     */
    BCMECMP_ALLOC(grp_attrs, sizeof(*grp_attrs), "bcmecmpOverlayGrpAttrs");
    ecmp_info->group[BCMECMP_GRP_TYPE_OVERLAY].lt_fattrs = (void *)grp_attrs;
    bcm56996_a0_ecmp_lt_field_attrs_t_init(grp_attrs);

    /*
     * Initialize Group logical table field identifer values based on the
     * device ECMP mode.
     */
    BCMECMP_ALLOC(lt_fids, sizeof(*lt_fids), "bcmecmpOverlayGrpLtdFids");
    ecmp_info->group[BCMECMP_GRP_TYPE_OVERLAY].ltd_fids = (void *)lt_fids;

    /*
     * Initialize Logical Table LTD field identifiers values for
     * overlay lookup ECMP Group LT logical fields.
     */
    bcm56996_a0_ecmp_grp_ltd_fields_t_init(lt_fids);
    lt_fids->ecmp_id_fid = ECMP_OVERLAYt_ECMP_IDf;
    lt_fids->lb_mode_fid = ECMP_OVERLAYt_LB_MODEf;
    lt_fids->max_paths_fid = ECMP_OVERLAYt_MAX_PATHSf;
    lt_fids->num_paths_fid = ECMP_OVERLAYt_NUM_PATHSf;
    lt_fids->nhop_id_fid = ECMP_OVERLAYt_UNDERLAY_NHOP_IDf;
    lt_fids->o_nhop_id_fid = ECMP_OVERLAYt_OVERLAY_NHOP_IDf;
    lt_fids->uecmp_id_fid = ECMP_OVERLAYt_ECMP_UNDERLAY_IDf;
    lt_fids->ecmp_nhop_fid = ECMP_OVERLAYt_ECMP_NHOP_UNDERLAYf;
    lt_fids->rh_nhop_id_fid = ECMP_OVERLAYt_RH_UNDERLAY_NHOP_IDf;
    lt_fids->rh_o_nhop_id_fid = ECMP_OVERLAYt_RH_OVERLAY_NHOP_IDf;
    lt_fids->rh_uecmp_id_fid = ECMP_OVERLAYt_RH_ECMP_UNDERLAY_IDf;
    lt_fids->rh_ecmp_nhop_fid = ECMP_OVERLAYt_RH_ECMP_NHOP_UNDERLAYf;
    lt_fids->rh_size_fid = ECMP_OVERLAYt_RH_SIZEf;
    lt_fids->rh_num_paths_fid = ECMP_OVERLAYt_RH_NUM_PATHSf;
    lt_fids->rh_seed_fid = ECMP_OVERLAYt_RH_RANDOM_SEEDf;
    lt_fids->ctr_ing_eflex_action_id_fid =
                            ECMP_OVERLAYt_ECMP_OVERLAY_CTR_ING_EFLEX_ACTION_IDf;
    lt_fids->ctr_ing_eflex_object_fid = ECMP_OVERLAYt_CTR_ING_EFLEX_OBJECTf;

    /* Get ECMP ID min and max values from LRD field data. */
    fdata = &(map->field_data->field[lt_fids->ecmp_id_fid]);
    grp_attrs->min_ecmp_id = fdata->min->u16;
    grp_attrs->max_ecmp_id = fdata->max->u16;

    /*
     * Get load balancing mode min and max values from LRD field
     * data.
     */
    fdata = &(map->field_data->field[lt_fids->lb_mode_fid]);
    grp_attrs->min_lb_mode = fdata->min->u32;
    grp_attrs->max_lb_mode = fdata->max->u32;

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
             * Initialize supported LB modes for level 1 ECMP
             * LT.
             */
            grp_attrs->lb_mode[lb_val] = lb_val;
        } else {
            SHR_ERR_EXIT(SHR_E_INTERNAL);
        }
    }

    /*
     * It's not mandatory that MAX_PATHS and NUM_PATHS values must be
     * in powers-of-two for entries in ECMP LT for this device.
     */
    grp_attrs->pow_two_paths = FALSE;

    /*
     * Get MAX_PATHS logical table LRD field data pointer.
     * Initialize device max_paths minimum and maximum values.
     */
    fdata = &(map->field_data->field[lt_fids->max_paths_fid]);
    grp_attrs->min_paths = fdata->min->u16;
    grp_attrs->max_paths = fdata->max->u16;

    /*
     * Get NUM_PATHS logical table LRD field data pointer.
     * Initialize device num_paths minimum and maximum values.
     */
    fdata = &(map->field_data->field[lt_fids->num_paths_fid]);
    grp_attrs->min_npaths = fdata->min->u16;
    grp_attrs->max_npaths = fdata->max->u16;

    /*
     * Get NHOP_ID or UNDERLAY_NHOP_ID logical table LRD field data pointer.
     * Initialize device NHOP_ID minimum and maximum values.
     */
    fdata = &(map->field_data->field[lt_fids->nhop_id_fid]);
    grp_attrs->min_nhop_id = fdata->min->u16;
    grp_attrs->max_nhop_id = fdata->max->u16;

    /*
     * Get ECMP_UNDERLAY_ID logical table LRD field data pointer.
     * Initialize device ECMP_UNDERLAY_ID minimum and maximum values.
     */
    fdata = &(map->field_data->field[lt_fids->uecmp_id_fid]);
    grp_attrs->min_uecmp_id = fdata->min->u16;
    grp_attrs->max_uecmp_id = fdata->max->u16;

    /*
     * Get NHOP_OVERLAY_ID logical table LRD field data pointer.
     * Initialize device NHOP_OVERLAY_ID minimum and maximum values.
     */
    fdata = &(map->field_data->field[lt_fids->o_nhop_id_fid]);
    grp_attrs->min_o_nhop_id = fdata->min->u16;
    grp_attrs->max_o_nhop_id = fdata->max->u16;

    /* RH grp NHOP related fields use the same attrs as non-RH grp NHOP. */

    /*
     * Get rh_size logical table LRD field data pointer.
     * Initialize device rh_size minimum and maximum values.
     */
    fdata = &(map->field_data->field[lt_fids->rh_size_fid]);
    grp_attrs->min_rh_size_enc = fdata->min->u32;
    grp_attrs->max_rh_size_enc = fdata->max->u32;

    /*
     * Get RH_NUM_PATHS logical table LRD field data pointer.
     * Initialize device rh_num_paths minimum and maximum values.
     */
    fdata = &(map->field_data->field[lt_fids->rh_num_paths_fid]);
    grp_attrs->min_rh_npaths = fdata->min->u16;
    grp_attrs->max_rh_npaths = fdata->max->u16;

    fdata = &(map->field_data->field[lt_fids->rh_seed_fid]);
    grp_attrs->min_rh_seed = fdata->min->u32;
    grp_attrs->max_rh_seed = fdata->max->u32;

    fdata = &(map->field_data->field[lt_fids->ctr_ing_eflex_action_id_fid]);
    grp_attrs->min_ctr_ing_eflex_action_id = fdata->min->u16;
    grp_attrs->max_ctr_ing_eflex_action_id = fdata->max->u16;

    fdata = &(map->field_data->field[lt_fids->ctr_ing_eflex_object_fid]);
    grp_attrs->min_ctr_ing_eflex_object = fdata->min->u16;
    grp_attrs->max_ctr_ing_eflex_object = fdata->max->u16;

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
bcm56996_a0_ecmp_underlay_lt_flds_init(int unit,
                                      const bcmlrd_map_t *map)
{
    bcm56996_a0_ecmp_grp_drd_fields_t *gdrd_fields = NULL;  /* Group tbl DRD info. */
    bcm56996_a0_ecmp_memb_drd_fields_t *mdrd_fields = NULL; /* Member tbl DRD info. */
    bcmecmp_grp_ltd_fields_t *lt_fids = NULL;   /* LT entry field IDs. */
    bcmecmp_lt_field_attrs_t *grp_attrs = NULL; /* overlay LT field attrs. */
    uint32_t idx;    /* Enum symbol array index. */
    uint32_t lb_val; /* LB mode value. */
    const bcmlrd_field_data_t *fdata = NULL; /* LT field data map. */
    const char *const lb_name[BCMECMP_LB_MODE_COUNT] = BCMECMP_LB_MODE_NAME;
    bcmecmp_info_t *ecmp_info = NULL;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(map, SHR_E_INTERNAL);

    /* Allocate memory for storing Member table entry fields DRD ID values. */
    BCMECMP_ALLOC(mdrd_fields, sizeof(*mdrd_fields), "bcmecmpUnderlayMdrdFlds");
    ecmp_info = &(ecmp_ctrl_local[unit]->ecmp_info);
    ecmp_info->member[BCMECMP_GRP_TYPE_UNDERLAY].drd_fids = (void *)mdrd_fields;
    /* TH4G device does not support Pre-IFP ECMP tables. */
    ecmp_info->init_member[BCMECMP_GRP_TYPE_UNDERLAY].drd_fids = NULL;

    /* Initialize Member table entry fields DRD ID values. */
    bcm56996_a0_ecmp_memb_drd_fields_t_init(mdrd_fields);
    mdrd_fields->entry_type = ENTRY_TYPEf;
    mdrd_fields->ecmp_nhop_id[0] = ECMPv_NEXT_HOP_INDEX_0f;
    mdrd_fields->ecmp_nhop_id[1] = ECMPv_NEXT_HOP_INDEX_1f;
    mdrd_fields->ecmp_nhop_id[2] = ECMPv_NEXT_HOP_INDEX_2f;
    mdrd_fields->ecmp_nhop_id[3] = ECMPv_NEXT_HOP_INDEX_3f;

    /* Allocate memory for storing Group table entry fields DRD ID values. */
    BCMECMP_ALLOC(gdrd_fields, sizeof(*gdrd_fields), "bcmecmpUnerlayGdrdFlds");
    ecmp_info->group[BCMECMP_GRP_TYPE_UNDERLAY].drd_fids = (void *)gdrd_fields;
    /* TH4G device does not support Pre-IFP ECMP tables. */
    ecmp_info->init_group[BCMECMP_GRP_TYPE_UNDERLAY].drd_fids = NULL;

    /* Initialize Group table entry fields DRD ID values. */
    bcm56996_a0_ecmp_grp_drd_fields_t_init(gdrd_fields);
    gdrd_fields->count = COUNTf;
    gdrd_fields->base_ptr = BASE_PTRf;
    gdrd_fields->lb_mode = LB_MODEf;
    gdrd_fields->flex_ctr_object = FLEX_CTR_OBJECTf;
    gdrd_fields->flex_ctr_action_sel = FLEX_CTR_ACTION_SELf;

    /*
     * Allocate memory for storing ECMP LT logical
     * fields Min, Max attribute values.
     */
    BCMECMP_ALLOC(grp_attrs, sizeof(*grp_attrs), "bcmecmpUnerlayGrpAttrs");
    ecmp_info->group[BCMECMP_GRP_TYPE_UNDERLAY].lt_fattrs = (void *)grp_attrs;
    bcm56996_a0_ecmp_lt_field_attrs_t_init(grp_attrs);

    /*
     * Initialize Group logical table field identifer values based on the
     * device ECMP mode.
     */
    BCMECMP_ALLOC(lt_fids, sizeof(*lt_fids), "bcmecmpUnerlayGrpLtdFids");
    ecmp_info->group[BCMECMP_GRP_TYPE_UNDERLAY].ltd_fids = (void *)lt_fids;

    /*
     * Initialize Logical Table LTD field identifiers values for
     * underlay ECMP Group LT logical fields.
     */
    bcm56996_a0_ecmp_grp_ltd_fields_t_init(lt_fids);
    lt_fids->ecmp_id_fid = ECMP_UNDERLAYt_ECMP_IDf;
    lt_fids->lb_mode_fid = ECMP_UNDERLAYt_LB_MODEf;
    lt_fids->max_paths_fid = ECMP_UNDERLAYt_MAX_PATHSf;
    lt_fids->num_paths_fid = ECMP_UNDERLAYt_NUM_PATHSf;
    lt_fids->nhop_sorted_fid = ECMP_UNDERLAYt_NHOP_SORTEDf;
    lt_fids->nhop_id_fid = ECMP_UNDERLAYt_NHOP_IDf;
    lt_fids->rh_nhop_id_fid = ECMP_UNDERLAYt_RH_NHOP_IDf;
    lt_fids->rh_size_fid = ECMP_UNDERLAYt_RH_SIZEf;
    lt_fids->rh_num_paths_fid = ECMP_UNDERLAYt_RH_NUM_PATHSf;
    lt_fids->rh_seed_fid = ECMP_UNDERLAYt_RH_RANDOM_SEEDf;
    lt_fids->ctr_ing_eflex_action_id_fid =
                                    ECMP_UNDERLAYt_ECMP_CTR_ING_EFLEX_ACTION_IDf;
    lt_fids->ctr_ing_eflex_object_fid = ECMP_UNDERLAYt_CTR_ING_EFLEX_OBJECTf;

    /* Get ECMP ID min and max values from LRD field data. */
    fdata = &(map->field_data->field[lt_fids->ecmp_id_fid]);
    grp_attrs->min_ecmp_id = fdata->min->u16;
    grp_attrs->max_ecmp_id = fdata->max->u16;

    /*
     * Get load balancing mode min and max values from LRD field
     * data.
     */
    fdata = &(map->field_data->field[lt_fids->lb_mode_fid]);
    grp_attrs->min_lb_mode = fdata->min->u32;
    grp_attrs->max_lb_mode = fdata->max->u32;

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
             * Initialize supported LB modes for level 1 ECMP
             * LT.
             */
            grp_attrs->lb_mode[lb_val] = lb_val;
        } else {
            SHR_ERR_EXIT(SHR_E_INTERNAL);
        }
    }

    /*
     * It's not mandatory that MAX_PATHS and NUM_PATHS values must be
     * in powers-of-two for entries in ECMP LT for this device.
     */
    grp_attrs->pow_two_paths = FALSE;

    /*
     * Get MAX_PATHS logical table LRD field data pointer.
     * Initialize device max_paths minimum and maximum values.
     */
    fdata = &(map->field_data->field[lt_fids->max_paths_fid]);
    grp_attrs->min_paths = fdata->min->u16;
    grp_attrs->max_paths = fdata->max->u16;

    /*
     * Get NUM_PATHS logical table LRD field data pointer.
     * Initialize device num_paths minimum and maximum values.
     */
    fdata = &(map->field_data->field[lt_fids->num_paths_fid]);
    grp_attrs->min_npaths = fdata->min->u16;
    grp_attrs->max_npaths = fdata->max->u16;

    /*
     * Get NHOP_ID or UNDERLAY_NHOP_ID logical table LRD field data pointer.
     * Initialize device NHOP_ID minimum and maximum values.
     */
    fdata = &(map->field_data->field[lt_fids->nhop_id_fid]);
    grp_attrs->min_nhop_id = fdata->min->u16;
    grp_attrs->max_nhop_id = fdata->max->u16;

    /*
     * Get rh_size logical table LRD field data pointer.
     * Initialize device rh_size minimum and maximum values.
     */
    fdata = &(map->field_data->field[lt_fids->rh_size_fid]);
    grp_attrs->min_rh_size_enc = fdata->min->u32;
    grp_attrs->max_rh_size_enc = fdata->max->u32;

    /*
     * Get RH_NUM_PATHS logical table LRD field data pointer.
     * Initialize device rh_num_paths minimum and maximum values.
     */
    fdata = &(map->field_data->field[lt_fids->rh_num_paths_fid]);
    grp_attrs->min_rh_npaths = fdata->min->u16;
    grp_attrs->max_rh_npaths = fdata->max->u16;

    fdata = &(map->field_data->field[lt_fids->rh_seed_fid]);
    grp_attrs->min_rh_seed = fdata->min->u32;
    grp_attrs->max_rh_seed = fdata->max->u32;

    fdata = &(map->field_data->field[lt_fids->ctr_ing_eflex_action_id_fid]);
    grp_attrs->min_ctr_ing_eflex_action_id = fdata->min->u16;
    grp_attrs->max_ctr_ing_eflex_action_id = fdata->max->u16;

    fdata = &(map->field_data->field[lt_fids->ctr_ing_eflex_object_fid]);
    grp_attrs->min_ctr_ing_eflex_object = fdata->min->u16;
    grp_attrs->max_ctr_ing_eflex_object = fdata->max->u16;

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
bcm56996_a0_ecmp_grps_flds_cleanup(int unit, bcmecmp_grp_type_t grp_type)
{
    bcm56996_a0_ecmp_grp_drd_fields_t *gdrd_fields = NULL;  /* Group tbl DRD info. */
    bcm56996_a0_ecmp_memb_drd_fields_t *mdrd_fields = NULL; /* Member tbl DRD info. */
    bcmecmp_grp_ltd_fields_t *lt_fids = NULL;   /* LT entry fields LTD ID. */
    bcmecmp_lt_field_attrs_t *grp_attrs = NULL; /* Single LT field attrs. */
    bcmecmp_info_t *ecmp_info = NULL;

    SHR_FUNC_ENTER(unit);

    ecmp_info = &(ecmp_ctrl_local[unit]->ecmp_info);

    /* Free memory allocated for storing Group tables DRD fields data. */
    gdrd_fields = (bcm56996_a0_ecmp_grp_drd_fields_t *)
                       ecmp_info->group[grp_type].drd_fids;
    BCMECMP_FREE(gdrd_fields);

    /* Free memory allocated for storing Member tables DRD fields data. */
    mdrd_fields = (bcm56996_a0_ecmp_memb_drd_fields_t *)
                       ecmp_info->member[grp_type].drd_fids;
    BCMECMP_FREE(mdrd_fields);

    /* Free memory allocated for storing ECMP LT logical fields data. */
    lt_fids = (bcmecmp_grp_ltd_fields_t *)
                   ecmp_info->group[grp_type].ltd_fids;
    BCMECMP_FREE(lt_fids);

    /*
     * Free memory allocated for storing ECMP LT
     * logical fields attributes information.
     */
    grp_attrs = (bcmecmp_lt_field_attrs_t *)
                     ecmp_info->group[grp_type].lt_fattrs;
    BCMECMP_FREE(grp_attrs);

    SHR_FUNC_EXIT();
}

static int
bcm56996_a0_ecmp_ovlay_weighted_lt_flds_init(int unit,
                                             const bcmlrd_map_t *map)
{
    bcm56996_a0_ecmp_grp_drd_fields_t *gdrd_fields = NULL;  /* Group tbl DRD info. */
    bcm56996_a0_ecmp_memb_drd_fields_t *mdrd_fields = NULL; /* Member tbl DRD info. */
    bcmecmp_grp_ltd_fields_t *lt_fids = NULL;   /* LT entry field IDs. */
    bcmecmp_lt_field_attrs_t *grp_attrs = NULL; /* Single LT field attrs. */
    const bcmlrd_field_data_t *fdata = NULL; /* LT field data map. */
    bcmecmp_info_t *ecmp_info = NULL;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(map, SHR_E_INTERNAL);

    /* Allocate memory for storing Member table entry fields DRD ID values. */
    BCMECMP_ALLOC(mdrd_fields, sizeof(*mdrd_fields), "bcmecmpOvlyWeightMdrdFlds");
    ecmp_info = &(ecmp_ctrl_local[unit]->ecmp_info);
    ecmp_info->member[BCMECMP_GRP_TYPE_OVERLAY_WEIGHTED].drd_fids =
                                                            (void *)mdrd_fields;

    /* Initialize Member table entry fields DRD ID values. */
    bcm56996_a0_ecmp_memb_drd_fields_t_init(mdrd_fields);
    mdrd_fields->entry_type = ENTRY_TYPEf;
    mdrd_fields->wecmp_weight = HIERARCHICAL_WECMPv_WEIGHTf;
    mdrd_fields->wecmp_nhop_id[0] = HIERARCHICAL_WECMPv_U_NEXT_HOP_INDEX_Af;
    mdrd_fields->wecmp_nhop_id[1] = HIERARCHICAL_WECMPv_U_NEXT_HOP_INDEX_Bf;
    mdrd_fields->wecmp_u_ecmp_id[0] = HIERARCHICAL_WECMPv_U_ECMP_GROUP_Af;
    mdrd_fields->wecmp_u_ecmp_id[1] = HIERARCHICAL_WECMPv_U_ECMP_GROUP_Bf;
    mdrd_fields->wecmp_u_ecmp_nhop[0] =
                                     HIERARCHICAL_WECMPv_U_NHI_OR_ECMP_GROUP_Af;
    mdrd_fields->wecmp_u_ecmp_nhop[1] =
                                     HIERARCHICAL_WECMPv_U_NHI_OR_ECMP_GROUP_Bf;
    mdrd_fields->wecmp_o_nhop_id[0] = HIERARCHICAL_WECMPv_O_NEXT_HOP_INDEX_Af;
    mdrd_fields->wecmp_o_nhop_id[1] = HIERARCHICAL_WECMPv_O_NEXT_HOP_INDEX_Bf;

    mdrd_fields->o_nhop_id[0] = HIERARCHICAL_ECMPv_O_NEXT_HOP_INDEX_0f;
    mdrd_fields->o_nhop_id[1] = HIERARCHICAL_ECMPv_O_NEXT_HOP_INDEX_1f;
    mdrd_fields->u_ecmp_nhop[0] = HIERARCHICAL_ECMPv_U_NHI_OR_ECMP_GROUP_0f;
    mdrd_fields->u_ecmp_nhop[1] = HIERARCHICAL_ECMPv_U_NHI_OR_ECMP_GROUP_1f;;
    mdrd_fields->u_ecmp_id[0] = HIERARCHICAL_ECMPv_U_ECMP_GROUP_0f;
    mdrd_fields->u_ecmp_id[1] = HIERARCHICAL_ECMPv_U_ECMP_GROUP_1f;
    mdrd_fields->u_nhop_id[0] = HIERARCHICAL_ECMPv_U_NEXT_HOP_INDEX_0f;
    mdrd_fields->u_nhop_id[1] = HIERARCHICAL_ECMPv_U_NEXT_HOP_INDEX_1f;

    /* Allocate memory for storing Group table entry fields DRD ID values. */
    BCMECMP_ALLOC(gdrd_fields, sizeof(*gdrd_fields), "bcmecmpOvlyWeightGdrdFlds");
    ecmp_info->group[BCMECMP_GRP_TYPE_OVERLAY_WEIGHTED].drd_fids =
                                                            (void *)gdrd_fields;

    /* Initialize Group table entry fields DRD ID values. */
    bcm56996_a0_ecmp_grp_drd_fields_t_init(gdrd_fields);
    gdrd_fields->count = COUNTf;
    gdrd_fields->base_ptr = BASE_PTRf;
    gdrd_fields->lb_mode = LB_MODEf;
    gdrd_fields->flex_ctr_object = FLEX_CTR_OBJECTf;
    gdrd_fields->flex_ctr_action_sel = FLEX_CTR_ACTION_SELf;

    /*
     * Allocate memory for storing ECMP LT logical
     * fields Min, Max attribute values.
     */
    BCMECMP_ALLOC(grp_attrs, sizeof(*grp_attrs), "bcmecmpOvlyWeightGrpAttrs");
    ecmp_info->group[BCMECMP_GRP_TYPE_OVERLAY_WEIGHTED].lt_fattrs =
                                                              (void *)grp_attrs;
    bcm56996_a0_ecmp_lt_field_attrs_t_init(grp_attrs);

    /*
     * Initialize Group logical table field identifier values based on the
     * device ECMP mode.
     */
    BCMECMP_ALLOC(lt_fids, sizeof(*lt_fids), "bcmecmpOvlyWeightGrpLtdFids");
    ecmp_info->group[BCMECMP_GRP_TYPE_OVERLAY_WEIGHTED].ltd_fids =
                                                                (void *)lt_fids;

    /*
     * Initialize Logical Table LTD field identifiers values for
     * single lookup ECMP Group LT logical fields.
     */
    bcm56996_a0_ecmp_grp_ltd_fields_t_init(lt_fids);
    lt_fids->ecmp_id_fid = ECMP_WEIGHTED_OVERLAYt_ECMP_IDf;
    lt_fids->num_paths_fid = ECMP_WEIGHTED_OVERLAYt_NUM_PATHSf;
    lt_fids->nhop_id_fid = ECMP_WEIGHTED_OVERLAYt_UNDERLAY_NHOP_IDf;
    lt_fids->uecmp_id_fid = ECMP_WEIGHTED_OVERLAYt_ECMP_UNDERLAY_IDf;
    lt_fids->ecmp_nhop_fid = ECMP_WEIGHTED_OVERLAYt_ECMP_NHOP_UNDERLAYf;
    lt_fids->o_nhop_id_fid = ECMP_WEIGHTED_OVERLAYt_OVERLAY_NHOP_IDf;
    lt_fids->weight_fid = ECMP_WEIGHTED_OVERLAYt_WEIGHTf;
    lt_fids->ctr_ing_eflex_action_id_fid =
        ECMP_WEIGHTED_OVERLAYt_ECMP_OVERLAY_CTR_ING_EFLEX_ACTION_IDf;
    lt_fids->ctr_ing_eflex_object_fid =
        ECMP_WEIGHTED_OVERLAYt_CTR_ING_EFLEX_OBJECTf;

    /* Get ECMP ID min and max values from LRD field data. */
    fdata = &(map->field_data->field[lt_fids->ecmp_id_fid]);
    grp_attrs->min_ecmp_id = fdata->min->u16;
    grp_attrs->max_ecmp_id = fdata->max->u16;

    /*
     * It's not mandatory that MAX_PATHS and NUM_PATHS values must be
     * in powers-of-two for entries in ECMP LT for this device.
     */
    grp_attrs->pow_two_paths = FALSE;

    /*
     * Get NUM_PATHS logical table LRD field data pointer.
     * Initialize device num_paths minimum and maximum values.
     */
    fdata = &(map->field_data->field[lt_fids->num_paths_fid]);
    grp_attrs->min_npaths = fdata->min->u16;
    grp_attrs->max_npaths = fdata->max->u16;
    grp_attrs->min_paths = 1;
    grp_attrs->max_paths = fdata->max->u16;

    /*
     * Get UNDERLAY_NHOP_ID logical table LRD field data pointer.
     * Initialize device NHOP_ID minimum and maximum values.
     */
    fdata = &(map->field_data->field[lt_fids->nhop_id_fid]);
    grp_attrs->min_nhop_id = fdata->min->u16;
    grp_attrs->max_nhop_id = fdata->max->u16;

    /*
     * Get ECMP_UNDERLAY_ID logical table LRD field data pointer.
     * Initialize device ECMP_UNDERLAY_ID minimum and maximum values.
     */
    fdata = &(map->field_data->field[lt_fids->uecmp_id_fid]);
    grp_attrs->min_uecmp_id = fdata->min->u16;
    grp_attrs->max_uecmp_id = fdata->max->u16;

    /*
     * Get NHOP_OVERLAY_ID logical table LRD field data pointer.
     * Initialize device NHOP_OVERLAY_ID minimum and maximum values.
     */
    fdata = &(map->field_data->field[lt_fids->o_nhop_id_fid]);
    grp_attrs->min_o_nhop_id = fdata->min->u16;
    grp_attrs->max_o_nhop_id = fdata->max->u16;

    fdata = &(map->field_data->field[lt_fids->weight_fid]);
    grp_attrs->min_weight = fdata->min->u16;
    grp_attrs->max_weight = fdata->max->u16;

    grp_attrs->min_weighted_size = BCM56996_OVLAY_WEIGHTED_SIZE_0_127;
    grp_attrs->max_weighted_size = BCM56996_OVLAY_WEIGHTED_SIZE_0_127;
    grp_attrs->def_weighted_size = BCM56996_OVLAY_WEIGHTED_SIZE_0_127;

    grp_attrs->min_weighted_mode = BCMECMP_WEIGHTED_MODE_MEMBER_WEIGHT;
    grp_attrs->max_weighted_mode = BCMECMP_WEIGHTED_MODE_MEMBER_WEIGHT;

    fdata = &(map->field_data->field[lt_fids->ctr_ing_eflex_action_id_fid]);
    grp_attrs->min_ctr_ing_eflex_action_id = fdata->min->u16;
    grp_attrs->max_ctr_ing_eflex_action_id = fdata->max->u16;

    fdata = &(map->field_data->field[lt_fids->ctr_ing_eflex_object_fid]);
    grp_attrs->min_ctr_ing_eflex_object = fdata->min->u16;
    grp_attrs->max_ctr_ing_eflex_object = fdata->max->u16;

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
bcm56996_a0_ecmp_unlay_weighted_lt_flds_init(int unit,
                                             const bcmlrd_map_t *map)
{
    bcm56996_a0_ecmp_grp_drd_fields_t *gdrd_fields = NULL;  /* Group tbl DRD info. */
    bcm56996_a0_ecmp_memb_drd_fields_t *mdrd_fields = NULL; /* Member tbl DRD info. */
    bcmecmp_grp_ltd_fields_t *lt_fids = NULL;   /* LT entry field IDs. */
    bcmecmp_lt_field_attrs_t *grp_attrs = NULL; /* Single LT field attrs. */
    const bcmlrd_field_data_t *fdata = NULL; /* LT field data map. */
    bcmecmp_info_t *ecmp_info = NULL;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(map, SHR_E_INTERNAL);

    /* Allocate memory for storing Member table entry fields DRD ID values. */
    BCMECMP_ALLOC(mdrd_fields, sizeof(*mdrd_fields), "bcmecmpUnlyWeightMdrdFlds");
    ecmp_info = &(ecmp_ctrl_local[unit]->ecmp_info);
    ecmp_info->member[BCMECMP_GRP_TYPE_UNDERLAY_WEIGHTED].drd_fids =
                                                            (void *)mdrd_fields;

    /* Initialize Member table entry fields DRD ID values. */
    bcm56996_a0_ecmp_memb_drd_fields_t_init(mdrd_fields);
    mdrd_fields->entry_type = ENTRY_TYPEf;
    mdrd_fields->wecmp_weight = WECMPv_WEIGHTf;
    mdrd_fields->wecmp_nhop_id[0] = WECMPv_NEXT_HOP_INDEX_Af;
    mdrd_fields->wecmp_nhop_id[1] = WECMPv_NEXT_HOP_INDEX_Bf;

    mdrd_fields->ecmp_nhop_id[0] = ECMPv_NEXT_HOP_INDEX_0f;
    mdrd_fields->ecmp_nhop_id[1] = ECMPv_NEXT_HOP_INDEX_1f;
    mdrd_fields->ecmp_nhop_id[2] = ECMPv_NEXT_HOP_INDEX_2f;
    mdrd_fields->ecmp_nhop_id[3] = ECMPv_NEXT_HOP_INDEX_3f;

    /* Allocate memory for storing Group table entry fields DRD ID values. */
    BCMECMP_ALLOC(gdrd_fields, sizeof(*gdrd_fields), "bcmecmpUnlyWeightGdrdFlds");
    ecmp_info->group[BCMECMP_GRP_TYPE_UNDERLAY_WEIGHTED].drd_fids =
                                                            (void *)gdrd_fields;

    /* Initialize Group table entry fields DRD ID values. */
    bcm56996_a0_ecmp_grp_drd_fields_t_init(gdrd_fields);
    gdrd_fields->count = COUNTf;
    gdrd_fields->base_ptr = BASE_PTRf;
    gdrd_fields->lb_mode = LB_MODEf;
    gdrd_fields->flex_ctr_object = FLEX_CTR_OBJECTf;
    gdrd_fields->flex_ctr_action_sel = FLEX_CTR_ACTION_SELf;

    /*
     * Allocate memory for storing ECMP LT logical
     * fields Min, Max attribute values.
     */
    BCMECMP_ALLOC(grp_attrs, sizeof(*grp_attrs), "bcmecmpOvlyWeightGrpAttrs");
    ecmp_info->group[BCMECMP_GRP_TYPE_UNDERLAY_WEIGHTED].lt_fattrs =
                                                              (void *)grp_attrs;
    bcm56996_a0_ecmp_lt_field_attrs_t_init(grp_attrs);

    /*
     * Initialize Group logical table field identifier values based on the
     * device ECMP mode.
     */
    BCMECMP_ALLOC(lt_fids, sizeof(*lt_fids), "bcmecmpOvlyWeightGrpLtdFids");
    ecmp_info->group[BCMECMP_GRP_TYPE_UNDERLAY_WEIGHTED].ltd_fids =
                                                                (void *)lt_fids;

    /*
     * Initialize Logical Table LTD field identifiers values for
     * single lookup ECMP Group LT logical fields.
     */
    bcm56996_a0_ecmp_grp_ltd_fields_t_init(lt_fids);
    lt_fids->ecmp_id_fid = ECMP_WEIGHTED_UNDERLAYt_ECMP_IDf;
    lt_fids->num_paths_fid = ECMP_WEIGHTED_UNDERLAYt_NUM_PATHSf;
    lt_fids->nhop_id_fid = ECMP_WEIGHTED_UNDERLAYt_NHOP_IDf;
    lt_fids->weight_fid = ECMP_WEIGHTED_UNDERLAYt_WEIGHTf;
    lt_fids->weighted_size_fid = ECMP_WEIGHTED_UNDERLAYt_WEIGHTED_SIZEf;
    lt_fids->weighted_mode_fid = ECMP_WEIGHTED_UNDERLAYt_WEIGHTED_MODEf;
    lt_fids->ctr_ing_eflex_action_id_fid =
        ECMP_WEIGHTED_UNDERLAYt_ECMP_CTR_ING_EFLEX_ACTION_IDf;
    lt_fids->ctr_ing_eflex_object_fid =
        ECMP_WEIGHTED_UNDERLAYt_CTR_ING_EFLEX_OBJECTf;

    /* Get ECMP ID min and max values from LRD field data. */
    fdata = &(map->field_data->field[lt_fids->ecmp_id_fid]);
    grp_attrs->min_ecmp_id = fdata->min->u16;
    grp_attrs->max_ecmp_id = fdata->max->u16;

    /*
     * It's not mandatory that MAX_PATHS and NUM_PATHS values must be
     * in powers-of-two for entries in ECMP LT for this device.
     */
    grp_attrs->pow_two_paths = FALSE;

    /*
     * Get NUM_PATHS logical table LRD field data pointer.
     * Initialize device num_paths minimum and maximum values.
     */
    fdata = &(map->field_data->field[lt_fids->num_paths_fid]);
    grp_attrs->min_npaths = fdata->min->u16;
    grp_attrs->max_npaths = fdata->max->u16;
    grp_attrs->min_paths = 1;
    grp_attrs->max_paths = fdata->max->u16;

    /*
     * Get UNDERLAY_NHOP_ID logical table LRD field data pointer.
     * Initialize device NHOP_ID minimum and maximum values.
     */
    fdata = &(map->field_data->field[lt_fids->nhop_id_fid]);
    grp_attrs->min_nhop_id = fdata->min->u16;
    grp_attrs->max_nhop_id = fdata->max->u16;

    fdata = &(map->field_data->field[lt_fids->weighted_size_fid]);
    grp_attrs->min_weighted_size = fdata->min->u32;
    grp_attrs->max_weighted_size = fdata->max->u32;
    grp_attrs->def_weighted_size = fdata->def->u32;

    fdata = &(map->field_data->field[lt_fids->weight_fid]);
    grp_attrs->min_weight = fdata->min->u16;
    grp_attrs->max_weight = fdata->max->u16;

    /*
     * Get weighted ECMP group mode min and max values from LRD field
     * data.
     */
    fdata = &(map->field_data->field[lt_fids->weighted_mode_fid]);
    grp_attrs->min_weighted_mode = fdata->min->u32;
    grp_attrs->max_weighted_mode = fdata->max->u32;

    fdata = &(map->field_data->field[lt_fids->ctr_ing_eflex_action_id_fid]);
    grp_attrs->min_ctr_ing_eflex_action_id = fdata->min->u16;
    grp_attrs->max_ctr_ing_eflex_action_id = fdata->max->u16;

    fdata = &(map->field_data->field[lt_fids->ctr_ing_eflex_object_fid]);
    grp_attrs->min_ctr_ing_eflex_object = fdata->min->u16;
    grp_attrs->max_ctr_ing_eflex_object = fdata->max->u16;

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
bcm56996_a0_ecmp_fields_init(int unit)
{
    const bcmlrd_map_t *map = NULL; /* LT map. */

    SHR_FUNC_ENTER(unit);

    /* Initialize device ECMP resolution mode field identifier value. */
    BCMECMP_MODE_DRD_FID(unit, config) = INVALIDf;

    /* Get LRD mapping information for ECMP LT. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlrd_map_get(unit,
                        BCMECMP_TBL_LTD_SID
                            (unit, group, BCMECMP_GRP_TYPE_OVERLAY),
                        &map));
    SHR_NULL_CHECK(map, SHR_E_INTERNAL);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcm56996_a0_ecmp_overlay_lt_flds_init(unit, map));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlrd_map_get(unit,
                        BCMECMP_TBL_LTD_SID
                            (unit, group, BCMECMP_GRP_TYPE_OVERLAY_WEIGHTED),
                        &map));
    SHR_NULL_CHECK(map, SHR_E_INTERNAL);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcm56996_a0_ecmp_ovlay_weighted_lt_flds_init(unit, map));

    /* Get LRD mapping information for ECMP LT. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlrd_map_get(unit,
                        BCMECMP_TBL_LTD_SID
                            (unit, group, BCMECMP_GRP_TYPE_UNDERLAY),
                        &map));
    SHR_NULL_CHECK(map, SHR_E_INTERNAL);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcm56996_a0_ecmp_underlay_lt_flds_init(unit, map));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlrd_map_get(unit,
                        BCMECMP_TBL_LTD_SID
                            (unit, group, BCMECMP_GRP_TYPE_UNDERLAY_WEIGHTED),
                        &map));
    SHR_NULL_CHECK(map, SHR_E_INTERNAL);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcm56996_a0_ecmp_unlay_weighted_lt_flds_init(unit, map));

exit:
    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "rv=%d"),
                bcm56996_a0_ecmp_info_print(unit)));

    SHR_FUNC_EXIT();
}

static int
bcm56996_a0_ecmp_fields_cleanup(int unit)
{

    SHR_FUNC_ENTER(unit);

    /* Clear device ECMP resolution mode field identifier value. */
    BCMECMP_MODE_DRD_FID(unit, config) = INVALIDf;

    switch (BCMECMP_MODE(unit)) {
        case BCMECMP_MODE_HIER:
            /* Cleanup Overlay ECMP resolution LT fields information. */
            bcm56996_a0_ecmp_grps_flds_cleanup(unit, BCMECMP_GRP_TYPE_OVERLAY);

            /* Cleanup Overlay weighted ECMP resolution LT fields information. */
            bcm56996_a0_ecmp_grps_flds_cleanup
                                     (unit, BCMECMP_GRP_TYPE_OVERLAY_WEIGHTED);

            /* Cleanup Underlay ECMP resolution LT fields information. */
            bcm56996_a0_ecmp_grps_flds_cleanup(unit, BCMECMP_GRP_TYPE_UNDERLAY);

            /* Cleanup Underlay weighted ECMP resolution LT fields information. */
            bcm56996_a0_ecmp_grps_flds_cleanup
                                     (unit, BCMECMP_GRP_TYPE_UNDERLAY_WEIGHTED);
            break;

        default:
            SHR_EXIT();
    }

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56996_a0_ecmp_itbm_grp_lists_init(int unit)
{
    shr_itbm_list_params_t params;
    shr_itbm_list_hdl_t list_hdl = NULL;
    int rv = SHR_E_NONE;
    bcmecmp_tbl_prop_t *grp_tbl = NULL;
    bcmecmp_tbl_prop_t *grp_tbl_1 = NULL;
    bcmecmp_info_t *ecmp_info = &(ecmp_ctrl_local[unit]->ecmp_info);

    SHR_FUNC_ENTER(unit);

    grp_tbl = &(ecmp_info->group[BCMECMP_GRP_TYPE_OVERLAY]);
    shr_itbm_list_params_t_init(&params);
    params.unit = unit;
    params.table_id = grp_tbl->drd_sid;
    params.min = grp_tbl->imin + OVERLAY_GRP_ID_OFFSET;
    params.max = grp_tbl->imax + OVERLAY_GRP_ID_OFFSET;
    params.first = params.min;
    params.last = params.max;
    params.comp_id = BCMMGMT_ECMP_COMP_ID;
    params.block_stat = false;

    rv = shr_itbm_list_create(params, "bcmecmpGrpOverlayList", &list_hdl);
    if (rv == SHR_E_NONE && list_hdl != NULL) {
        grp_tbl->list_hdl = list_hdl;
        grp_tbl = &(ecmp_info->group[BCMECMP_GRP_TYPE_OVERLAY_WEIGHTED]);
        grp_tbl->list_hdl = list_hdl;
    } else {
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "Error: RM failed to create overlay glist %d\n"),
                                rv));
        SHR_ERR_EXIT(rv);
    }

    grp_tbl_1 = &(ecmp_info->group[BCMECMP_GRP_TYPE_UNDERLAY]);
    shr_itbm_list_params_t_init(&params);
    params.unit = unit;
    params.table_id = grp_tbl_1->drd_sid;
    params.min = grp_tbl_1->imin;
    params.max = grp_tbl_1->imax;
    params.first = params.min;
    params.last = params.max;
    params.comp_id = BCMMGMT_ECMP_COMP_ID;
    params.block_stat = false;

    rv = shr_itbm_list_create(params, "bcmecmpGrpUnderlayList", &list_hdl);
    if (rv == SHR_E_NONE && list_hdl != NULL) {
        grp_tbl_1->list_hdl = list_hdl;
        grp_tbl_1 = &(ecmp_info->group[BCMECMP_GRP_TYPE_UNDERLAY_WEIGHTED]);
        grp_tbl_1->list_hdl = list_hdl;
    } else {
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "Error: RM failed to create underlay glist %d\n"),
                                rv));
        SHR_ERR_EXIT(rv);
    }

exit:
    if (SHR_FUNC_ERR()) {
        /* group */
        if (grp_tbl->list_hdl) {
            list_hdl = grp_tbl->list_hdl;
            grp_tbl->list_hdl = NULL;
            grp_tbl = &(ecmp_info->group[BCMECMP_GRP_TYPE_OVERLAY]);
            grp_tbl->list_hdl = NULL;
            shr_itbm_list_destroy(list_hdl);
        }

        if (grp_tbl_1 && grp_tbl_1->list_hdl) {
            list_hdl = grp_tbl_1->list_hdl;
            grp_tbl_1->list_hdl = NULL;
            grp_tbl_1 = &(ecmp_info->group[BCMECMP_GRP_TYPE_UNDERLAY]);
            grp_tbl_1->list_hdl = NULL;
            shr_itbm_list_destroy(list_hdl);
        }
    }
    SHR_FUNC_EXIT();
}

static int
bcm56996_a0_ecmp_itbm_memb_lists_init(int unit)
{
    shr_itbm_list_params_t params;
    shr_itbm_list_hdl_t list_hdl = NULL;
    int rv = SHR_E_NONE;
    bcmecmp_tbl_prop_t *memb_tbl_1 = NULL;
    bcmecmp_tbl_prop_t *memb_tbl_2 = NULL;
    bcmecmp_tbl_prop_t *memb_tbl_3 = NULL;
    bcmecmp_tbl_prop_t *memb_tbl_4 = NULL;
    bcmecmp_info_t *ecmp_info = &(ecmp_ctrl_local[unit]->ecmp_info);
    int memb_imin;  /* ECMP Member hardware table Min index value. */
    int memb_imax;  /* ECMP Member hardware table Max index value. */

    SHR_FUNC_ENTER(unit);

    memb_imin = bcmdrd_pt_index_min(unit,
                                    BCMECMP_TBL_DRD_SID(unit, member,
                                                    BCMECMP_GRP_TYPE_UNDERLAY));
    memb_imax = bcmdrd_pt_index_max(unit,
                                    BCMECMP_TBL_DRD_SID(unit, member,
                                                    BCMECMP_GRP_TYPE_UNDERLAY));

    memb_tbl_1 = &(ecmp_info->member[BCMECMP_GRP_TYPE_UNDERLAY]);
    shr_itbm_list_params_t_init(&params);
    params.unit = unit;
    params.comp_id = BCMMGMT_ECMP_COMP_ID;
    params.table_id = memb_tbl_1->drd_sid;
    params.min = memb_imin;
    params.max = memb_imax;
    params.first = memb_tbl_1->imin;
    params.last = memb_tbl_1->imax;
    params.block_stat = true;
    params.buckets = true;
    params.entries_per_bucket = NHOPS_PER_MEMB_ENTRY;
    params.reverse = false;

    list_hdl = NULL;
    rv = shr_itbm_list_create(params, "bcmecmpMemUnderlayList", &list_hdl);
    if (rv == SHR_E_NONE && list_hdl != NULL) {
        memb_tbl_1->list_hdl = list_hdl;
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "Succeed to create underlay memb list.\n")));
    } else {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "Fail to create underlay memb list rv=%d.\n"),
                   rv));
        SHR_ERR_EXIT(rv);
    }

    memb_tbl_2 = &(ecmp_info->member[BCMECMP_GRP_TYPE_UNDERLAY_WEIGHTED]);
    params.entries_per_bucket = NHOPS_PER_MEMB_ENTRY_WECMP;
    params.reverse = TRUE;
    list_hdl = NULL;
    rv = shr_itbm_list_create(params, "bcmecmpMemUnderlayWECMPList", &list_hdl);
    if (rv == SHR_E_NONE && list_hdl != NULL) {
        memb_tbl_2->list_hdl = list_hdl;
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "Succeed to create underlay"
                                " weighted memb list.\n")));
    } else {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "Fail to create underlay weighted"
                              " memb list rv=%d.\n"),
                   rv));
        SHR_ERR_EXIT(rv);
    }
    /*
     * Share the underlay type member list for MEMBER_REPLICATION groups
     * in ECMP_WEIGHTED.
     */
    memb_tbl_2->list_hdl_1 = memb_tbl_1->list_hdl;

    memb_tbl_3 = &(ecmp_info->member[BCMECMP_GRP_TYPE_OVERLAY]);
    memb_tbl_4 = &(ecmp_info->member[BCMECMP_GRP_TYPE_OVERLAY_WEIGHTED]);
    if (memb_tbl_3->tbl_size > 0) {
        params.comp_id = BCMMGMT_ECMP_COMP_ID;
        params.table_id = memb_tbl_3->drd_sid;
        params.min = memb_imin;
        params.max = memb_imax;
        params.first = memb_tbl_3->imin;
        params.last = memb_tbl_3->imax;
        params.block_stat = true;
        params.buckets = true;
        params.entries_per_bucket = NHOPS_PER_MEMB_ENTRY_OVERLAY;
        params.reverse = false;

        list_hdl = NULL;
        rv = shr_itbm_list_create(params, "bcmecmpMemOverlayList", &list_hdl);
        if (rv == SHR_E_NONE && list_hdl != NULL) {
            memb_tbl_3->list_hdl = list_hdl;
            LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_U(unit,
                                    "Succeed to create overlay memb list.\n")));
        } else {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit,
                                  "Fail to create overlay list rv=%d.\n"),
                       rv));
            SHR_ERR_EXIT(rv);
        }

        /* For overlay weighted group. */
        params.entries_per_bucket = NHOPS_PER_MEMB_ENTRY_WECMP;
        params.reverse = TRUE;
        list_hdl = NULL;
        rv = shr_itbm_list_create(params, "bcmecmpMemOverlayWECMPList",
                                  &list_hdl);
        if (rv == SHR_E_NONE && list_hdl != NULL) {
            memb_tbl_4->list_hdl = list_hdl;
            LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_U(unit,
                                    "Succeed to create overlay"
                                    " weighted memb list.\n")));
        } else {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit,
                                  "Fail to create overlay weighted"
                                  " memb list rv=%d.\n"),
                       rv));
            SHR_ERR_EXIT(rv);
        }
    }

exit:
    if (SHR_FUNC_ERR()) {
        /* underlay member */
        if (memb_tbl_1->list_hdl) {
            list_hdl = memb_tbl_1->list_hdl;
            memb_tbl_1->list_hdl = NULL;
            shr_itbm_list_destroy(list_hdl);
        }

        if (memb_tbl_2 && memb_tbl_2->list_hdl) {
            list_hdl = memb_tbl_2->list_hdl;
            memb_tbl_2->list_hdl = NULL;
            shr_itbm_list_destroy(list_hdl);
            memb_tbl_2->list_hdl_1 = NULL;
        }

        /* overlay member */
        if (memb_tbl_3 && memb_tbl_3->list_hdl) {
            list_hdl = memb_tbl_3->list_hdl;
            memb_tbl_3->list_hdl = NULL;
            shr_itbm_list_destroy(list_hdl);
        }

        if (memb_tbl_4 && memb_tbl_4->list_hdl) {
            list_hdl = memb_tbl_4->list_hdl;
            memb_tbl_4->list_hdl = NULL;
            shr_itbm_list_destroy(list_hdl);
        }
    }
    SHR_FUNC_EXIT();
}

static int
bcm56996_a0_ecmp_itbm_lists_init(int unit)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcm56996_a0_ecmp_itbm_grp_lists_init(unit));
    SHR_IF_ERR_EXIT
        (bcm56996_a0_ecmp_itbm_memb_lists_init(unit));

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56996_a0_ecmp_itbm_grp_lists_cleanup(int unit)
{
    shr_itbm_list_hdl_t list_hdl;
    int rv = SHR_E_NONE;
    bcmecmp_tbl_prop_t *tbl_prop = NULL;
    bcmecmp_info_t *ecmp_info = &(ecmp_ctrl_local[unit]->ecmp_info);

    SHR_FUNC_ENTER(unit);
    tbl_prop = &(ecmp_info->group[BCMECMP_GRP_TYPE_OVERLAY]);
    list_hdl = tbl_prop->list_hdl;
    rv = shr_itbm_list_destroy(list_hdl);
    if (SHR_FAILURE(rv)) {
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "Fail to clean overlay grp list rv %d.\n"),
                                rv));
    }
    SHR_IF_ERR_EXIT(rv);
    tbl_prop->list_hdl = NULL;
    tbl_prop = &(ecmp_info->group[BCMECMP_GRP_TYPE_OVERLAY_WEIGHTED]);
    tbl_prop->list_hdl = NULL;

    tbl_prop = &(ecmp_info->group[BCMECMP_GRP_TYPE_UNDERLAY]);
    list_hdl = tbl_prop->list_hdl;
    rv = shr_itbm_list_destroy(list_hdl);
    if (SHR_FAILURE(rv)) {
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "Fail to clean underlay grp list rv %d.\n"),
                                rv));
    }
    SHR_IF_ERR_EXIT(rv);
    tbl_prop->list_hdl = NULL;
    tbl_prop = &(ecmp_info->group[BCMECMP_GRP_TYPE_UNDERLAY_WEIGHTED]);
    tbl_prop->list_hdl = NULL;

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56996_a0_ecmp_itbm_memb_lists_cleanup(int unit)
{
    shr_itbm_list_hdl_t list_hdl;
    int rv = SHR_E_NONE;
    bcmecmp_tbl_prop_t *tbl_prop = NULL;
    bcmecmp_info_t *ecmp_info = &(ecmp_ctrl_local[unit]->ecmp_info);

    SHR_FUNC_ENTER(unit);
    tbl_prop =  &(ecmp_info->member[BCMECMP_GRP_TYPE_UNDERLAY]);
    list_hdl = tbl_prop->list_hdl;
    rv = shr_itbm_list_destroy(list_hdl);
    if (SHR_FAILURE(rv)) {
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "Fail to clean underlay memb list rv %d.\n"),
                                rv));
    }
    SHR_IF_ERR_EXIT(rv);
    tbl_prop->list_hdl = NULL;

    tbl_prop =  &(ecmp_info->member[BCMECMP_GRP_TYPE_UNDERLAY_WEIGHTED]);
    list_hdl = tbl_prop->list_hdl;
    rv = shr_itbm_list_destroy(list_hdl);
    if (SHR_FAILURE(rv)) {
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "Fail to clean underlay wmemb list rv %d.\n"),
                                rv));
    }
    SHR_IF_ERR_EXIT(rv);
    tbl_prop->list_hdl = NULL;
    tbl_prop->list_hdl_1 = NULL;

    tbl_prop =  &(ecmp_info->member[BCMECMP_GRP_TYPE_OVERLAY]);
    list_hdl = tbl_prop->list_hdl;
    if (list_hdl) {
        rv = shr_itbm_list_destroy(list_hdl);
        if (SHR_FAILURE(rv)) {
            LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_U(unit,
                                    "Fail to clean overlay memb list rv %d.\n"),
                                    rv));
        }
        SHR_IF_ERR_EXIT(rv);
        tbl_prop->list_hdl = NULL;
    }

    tbl_prop =  &(ecmp_info->member[BCMECMP_GRP_TYPE_OVERLAY_WEIGHTED]);
    list_hdl = tbl_prop->list_hdl;
    if (list_hdl) {
        rv = shr_itbm_list_destroy(list_hdl);
        if (SHR_FAILURE(rv)) {
            LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_U(unit,
                                    "Fail to clean overlay wmemb list rv %d.\n"),
                                    rv));
        }
        SHR_IF_ERR_EXIT(rv);
        tbl_prop->list_hdl = NULL;
    }

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56996_a0_ecmp_itbm_lists_cleanup(int unit)
{
    SHR_FUNC_ENTER(unit);
    SHR_IF_ERR_EXIT
        (bcm56996_a0_ecmp_itbm_grp_lists_cleanup(unit));
    SHR_IF_ERR_EXIT
        (bcm56996_a0_ecmp_itbm_memb_lists_cleanup(unit));

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56996_a0_ecmp_itbm_olay_memb_lists_increase(int unit, uint8_t num_o_memb_bank,
                                               uint8_t old_num_o_memb_bank)
{
    bcmecmp_info_t *ecmp_info = &(ecmp_ctrl_local[unit]->ecmp_info);
    shr_itbm_list_hdl_t list_hdl;
    bcmecmp_tbl_prop_t *tbl_prop = NULL;
    int rv = SHR_E_NONE;
    bool list_chg[BCMECMP_MEMB_LIST_COUNT] = {false};
    int memb_imin;  /* ECMP Member hardware table Min index value. */
    int memb_imax;  /* ECMP Member hardware table Max index value. */
    shr_itbm_list_params_t params;

    SHR_FUNC_ENTER(unit);
    /*
     * It is nessary to reduce the lists first and then increase other lists.
     * Reduce the underlay lists first.
     */
    SHR_NULL_CHECK(ecmp_info, SHR_E_INTERNAL);
    tbl_prop =  &(ecmp_info->member[BCMECMP_GRP_TYPE_UNDERLAY]);
    SHR_NULL_CHECK(tbl_prop, SHR_E_INTERNAL);
    list_hdl = tbl_prop->list_hdl;
    /*
     * The imin and imax have been updated with the new_o_member_bank in the HW
     * member tables range init method before invoking this API.
     */
    rv = shr_itbm_list_resize(list_hdl, tbl_prop->imin, tbl_prop->imax);
    if (SHR_FAILURE(rv)) {
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "Fail to resize underlay memb list rv %d.\n"),
                                rv));
        SHR_ERR_EXIT(rv);
    }
    list_chg[BCMECMP_MEMB_LIST_ULAY] = true;

    tbl_prop =  &(ecmp_info->member[BCMECMP_GRP_TYPE_UNDERLAY_WEIGHTED]);
    list_hdl = tbl_prop->list_hdl;
    rv = shr_itbm_list_resize(list_hdl, tbl_prop->imin, tbl_prop->imax);
    if (SHR_FAILURE(rv)) {
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "Fail to resize underlay wmemb list rv %d.\n"),
                                rv));
        SHR_ERR_EXIT(rv);
    }
    list_chg[BCMECMP_MEMB_LIST_ULAY_WEIGHT] = true;

    /* Increase the overlay lists. */
    memb_imin = bcmdrd_pt_index_min(unit,
                                    BCMECMP_TBL_DRD_SID(unit, member,
                                                    BCMECMP_GRP_TYPE_UNDERLAY));
    memb_imax = bcmdrd_pt_index_max(unit,
                                    BCMECMP_TBL_DRD_SID(unit, member,
                                                    BCMECMP_GRP_TYPE_UNDERLAY));
    shr_itbm_list_params_t_init(&params);
    tbl_prop =  &(ecmp_info->member[BCMECMP_GRP_TYPE_OVERLAY]);
    SHR_NULL_CHECK(tbl_prop, SHR_E_INTERNAL);
    list_hdl = tbl_prop->list_hdl;
    if (list_hdl) {
        rv = shr_itbm_list_resize(list_hdl, tbl_prop->imin, tbl_prop->imax);
        if (SHR_FAILURE(rv)) {
            LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_U(unit,
                                    "Fail to resize overlay memb list rv %d,"
                                    " new ovlay bank number %d.\n"),
                                    rv, num_o_memb_bank));
            SHR_ERR_EXIT(rv);
        }
    } else {
        /* Create overlay group. */
        params.unit = unit;
        params.comp_id = BCMMGMT_ECMP_COMP_ID;
        params.table_id = tbl_prop->drd_sid;
        params.min = memb_imin;
        params.max = memb_imax;
        params.first = tbl_prop->imin;
        params.last = tbl_prop->imax;
        params.block_stat = true;
        params.buckets = true;
        params.entries_per_bucket = NHOPS_PER_MEMB_ENTRY_OVERLAY;
        params.reverse = false;

        list_hdl = NULL;
        rv = shr_itbm_list_create(params, "bcmecmpMemOverlayList", &list_hdl);
        if (rv == SHR_E_NONE && list_hdl != NULL) {
            tbl_prop->list_hdl = list_hdl;
            LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_U(unit,
                                    "Succeed to create overlay memb list.\n")));
        } else {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit,
                                  "Fail to create overlay list rv=%d.\n"),
                       rv));
            SHR_ERR_EXIT(rv);
        }
    }
    list_chg[BCMECMP_MEMB_LIST_OLAY] = true;

    tbl_prop =  &(ecmp_info->member[BCMECMP_GRP_TYPE_OVERLAY_WEIGHTED]);
    SHR_NULL_CHECK(tbl_prop, SHR_E_INTERNAL);
    list_hdl = tbl_prop->list_hdl;
    if (list_hdl) {
        rv = shr_itbm_list_resize(list_hdl, tbl_prop->imin, tbl_prop->imax);
        if (SHR_FAILURE(rv)) {
            LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_U(unit,
                                    "Fail to resize overlay memb list rv %d,"
                                    " new ovlay bank number %d.\n"),
                                    rv, num_o_memb_bank));
            SHR_ERR_EXIT(rv);
        }
    } else {
        /* Create overlay weighted group. */
        params.unit = unit;
        params.comp_id = BCMMGMT_ECMP_COMP_ID;
        params.table_id = tbl_prop->drd_sid;
        params.min = memb_imin;
        params.max = memb_imax;
        params.first = tbl_prop->imin;
        params.last = tbl_prop->imax;
        params.block_stat = true;
        params.buckets = true;
        params.entries_per_bucket = NHOPS_PER_MEMB_ENTRY_WECMP;
        params.reverse = TRUE;
        list_hdl = NULL;
        rv = shr_itbm_list_create(params, "bcmecmpMemOverlayWECMPList",
                                  &list_hdl);
        if (rv == SHR_E_NONE && list_hdl != NULL) {
            tbl_prop->list_hdl = list_hdl;
            LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_U(unit,
                                    "Succeed to create overlay"
                                    " weighted memb list.\n")));
        } else {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit,
                                  "Fail to create overlay weighted"
                                  " memb list rv=%d.\n"),
                       rv));
            SHR_ERR_EXIT(rv);
        }
    }
    list_chg[BCMECMP_MEMB_LIST_OLAY_WEIGHT] = true;

exit:
    if (SHR_FUNC_ERR()) {
        bcm56996_a0_ecmp_hw_memb_tbls_range_init(unit, old_num_o_memb_bank);

        if (list_chg[BCMECMP_MEMB_LIST_OLAY]) {
            tbl_prop =  &(ecmp_info->member[BCMECMP_GRP_TYPE_OVERLAY]);
            SHR_NULL_CHECK(tbl_prop, SHR_E_INTERNAL);
            list_hdl = tbl_prop->list_hdl;
            if (list_hdl) {
                if (old_num_o_memb_bank) {
                    rv = shr_itbm_list_resize(list_hdl, tbl_prop->imin,
                                              tbl_prop->imax);
                } else {
                    rv = shr_itbm_list_destroy(list_hdl);
                }
                if (SHR_FAILURE(rv)) {
                    LOG_ERROR(BSL_LOG_MODULE,
                              (BSL_META_U(unit,
                                          "Fail to recover overlay memb list rv %d,"
                                          "old ovlay bank number %d.\n"),
                                          rv, old_num_o_memb_bank));
                    SHR_ERR_EXIT(rv);
                }
            }
        }

        if (list_chg[BCMECMP_MEMB_LIST_OLAY_WEIGHT]) {
            tbl_prop =  &(ecmp_info->member[BCMECMP_GRP_TYPE_OVERLAY]);
            SHR_NULL_CHECK(tbl_prop, SHR_E_INTERNAL);
            list_hdl = tbl_prop->list_hdl;
            if (list_hdl) {
                if (old_num_o_memb_bank) {
                    rv = shr_itbm_list_resize(list_hdl, tbl_prop->imin,
                                              tbl_prop->imax);
                } else {
                    rv = shr_itbm_list_destroy(list_hdl);
                }
                if (SHR_FAILURE(rv)) {
                    LOG_ERROR(BSL_LOG_MODULE,
                              (BSL_META_U(unit,
                                          "Fail to recover overlay memb list rv %d,"
                                          "old ovlay bank number %d.\n"),
                                          rv, old_num_o_memb_bank));
                    SHR_ERR_EXIT(rv);
                }
            }
        }

        if (list_chg[BCMECMP_MEMB_LIST_ULAY]) {
            tbl_prop =  &(ecmp_info->member[BCMECMP_GRP_TYPE_UNDERLAY]);
            SHR_NULL_CHECK(tbl_prop, SHR_E_INTERNAL);
            list_hdl = tbl_prop->list_hdl;
            rv = shr_itbm_list_resize(list_hdl, tbl_prop->imin, tbl_prop->imax);
            if (SHR_FAILURE(rv)) {
                LOG_ERROR(BSL_LOG_MODULE,
                          (BSL_META_U(unit,
                                      "Fail to recover underlay memb list rv %d.\n"),
                                      rv));
                SHR_ERR_EXIT(rv);
            }
        }

        if (list_chg[BCMECMP_MEMB_LIST_ULAY_WEIGHT]) {
            tbl_prop =  &(ecmp_info->member[BCMECMP_GRP_TYPE_UNDERLAY_WEIGHTED]);
            list_hdl = tbl_prop->list_hdl;
            rv = shr_itbm_list_resize(list_hdl, tbl_prop->imin, tbl_prop->imax);
            if (SHR_FAILURE(rv)) {
                LOG_ERROR(BSL_LOG_MODULE,
                          (BSL_META_U(unit,
                                      "Fail to recover underlay wmemb list rv %d.\n"),
                                      rv));
                SHR_ERR_EXIT(rv);
            }
        }
    }
    SHR_FUNC_EXIT();
}

static int
bcm56996_a0_ecmp_itbm_olay_memb_lists_reduce(int unit, uint8_t num_o_memb_bank,
                                             uint8_t old_num_o_memb_bank)
{
    bcmecmp_info_t *ecmp_info = &(ecmp_ctrl_local[unit]->ecmp_info);
    shr_itbm_list_hdl_t list_hdl;
    bcmecmp_tbl_prop_t *tbl_prop = NULL;
    int rv = SHR_E_NONE;
    bool list_chg[BCMECMP_MEMB_LIST_COUNT] = {false};
    int memb_imin;  /* ECMP Member hardware table Min index value. */
    int memb_imax;  /* ECMP Member hardware table Max index value. */
    shr_itbm_list_params_t params;

    SHR_FUNC_ENTER(unit);
    /*
     * It is nessary to reduce the lists first and then increase other lists.
     * Reduce the overlay lists first.
     */
    SHR_NULL_CHECK(ecmp_info, SHR_E_INTERNAL);
    tbl_prop = &(ecmp_info->member[BCMECMP_GRP_TYPE_OVERLAY]);
    SHR_NULL_CHECK(tbl_prop, SHR_E_INTERNAL);
    list_hdl = tbl_prop->list_hdl;
    if (list_hdl) {
        if (num_o_memb_bank){
            rv = shr_itbm_list_resize(list_hdl, tbl_prop->imin, tbl_prop->imax);
        } else {
            rv = shr_itbm_list_destroy(list_hdl);
        }
        if (SHR_FAILURE(rv)) {
            LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_U(unit,
                                    "Fail to resize overlay memb list rv %d,"
                                    " new ovlay bank number %d.\n"),
                                    rv, num_o_memb_bank));
            SHR_ERR_EXIT(rv);
        }
        if (!num_o_memb_bank) {
            tbl_prop->list_hdl = NULL;
        }
    } else {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }
    list_chg[BCMECMP_MEMB_LIST_OLAY] = true;

    tbl_prop = &(ecmp_info->member[BCMECMP_GRP_TYPE_OVERLAY_WEIGHTED]);
    SHR_NULL_CHECK(tbl_prop, SHR_E_INTERNAL);
    list_hdl = tbl_prop->list_hdl;
    if (list_hdl) {
        if (num_o_memb_bank){
            rv = shr_itbm_list_resize(list_hdl, tbl_prop->imin, tbl_prop->imax);
        } else {
            rv = shr_itbm_list_destroy(list_hdl);
        }
        if (SHR_FAILURE(rv)) {
            LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_U(unit,
                                    "Fail to resize overlay memb list rv %d,"
                                    " new ovlay bank number %d.\n"),
                                    rv, num_o_memb_bank));
            SHR_ERR_EXIT(rv);
        }
        if (!num_o_memb_bank) {
            tbl_prop->list_hdl = NULL;
        }
    } else {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }
    list_chg[BCMECMP_MEMB_LIST_OLAY_WEIGHT] = true;

    /* Increase the underlay lists. */
    tbl_prop = &(ecmp_info->member[BCMECMP_GRP_TYPE_UNDERLAY]);
    SHR_NULL_CHECK(tbl_prop, SHR_E_INTERNAL);
    list_hdl = tbl_prop->list_hdl;
    rv = shr_itbm_list_resize(list_hdl, tbl_prop->imin, tbl_prop->imax);
    if (SHR_FAILURE(rv)) {
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "Fail to resize underlay memb list rv %d.\n"),
                                rv));
        SHR_ERR_EXIT(rv);
    }
    list_chg[BCMECMP_MEMB_LIST_ULAY] = true;

    tbl_prop =  &(ecmp_info->member[BCMECMP_GRP_TYPE_UNDERLAY_WEIGHTED]);
    list_hdl = tbl_prop->list_hdl;
    rv = shr_itbm_list_resize(list_hdl, tbl_prop->imin, tbl_prop->imax);
    if (SHR_FAILURE(rv)) {
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "Fail to resize underlay wmemb list rv %d.\n"),
                                rv));
        SHR_ERR_EXIT(rv);
    }
    list_chg[BCMECMP_MEMB_LIST_ULAY_WEIGHT] = true;

exit:
    if (SHR_FUNC_ERR()) {
        bcm56996_a0_ecmp_hw_memb_tbls_range_init(unit, old_num_o_memb_bank);

        if (list_chg[BCMECMP_MEMB_LIST_ULAY]) {
            tbl_prop =  &(ecmp_info->member[BCMECMP_GRP_TYPE_UNDERLAY]);
            SHR_NULL_CHECK(tbl_prop, SHR_E_INTERNAL);
            list_hdl = tbl_prop->list_hdl;
            rv = shr_itbm_list_resize(list_hdl, tbl_prop->imin, tbl_prop->imax);
            if (SHR_FAILURE(rv)) {
                LOG_ERROR(BSL_LOG_MODULE,
                          (BSL_META_U(unit,
                                      "Fail to recover underlay memb list rv %d.\n"),
                                      rv));
            }
        }

        if (list_chg[BCMECMP_MEMB_LIST_ULAY_WEIGHT]) {
            tbl_prop =  &(ecmp_info->member[BCMECMP_GRP_TYPE_UNDERLAY_WEIGHTED]);
            list_hdl = tbl_prop->list_hdl;
            rv = shr_itbm_list_resize(list_hdl, tbl_prop->imin, tbl_prop->imax);
            if (SHR_FAILURE(rv)) {
                LOG_ERROR(BSL_LOG_MODULE,
                          (BSL_META_U(unit,
                                      "Fail to recover underlay wmemb list rv %d.\n"),
                                      rv));
            }
        }

        if (list_chg[BCMECMP_MEMB_LIST_OLAY]) {
            memb_imin = bcmdrd_pt_index_min(unit,
                                            BCMECMP_TBL_DRD_SID(unit, member,
                                                    BCMECMP_GRP_TYPE_UNDERLAY));
            memb_imax = bcmdrd_pt_index_max(unit,
                                            BCMECMP_TBL_DRD_SID(unit, member,
                                                    BCMECMP_GRP_TYPE_UNDERLAY));
            tbl_prop =  &(ecmp_info->member[BCMECMP_GRP_TYPE_OVERLAY]);
            SHR_NULL_CHECK(tbl_prop, SHR_E_INTERNAL);
            list_hdl = tbl_prop->list_hdl;
            if (list_hdl) {
                if (num_o_memb_bank) {
                    rv = shr_itbm_list_resize(list_hdl, tbl_prop->imin,
                                              tbl_prop->imax);
                }
            } else {
                shr_itbm_list_params_t_init(&params);
                params.unit = unit;
                params.comp_id = BCMMGMT_ECMP_COMP_ID;
                params.table_id = tbl_prop->drd_sid;
                params.min = memb_imin;
                params.max = memb_imax;
                params.first = tbl_prop->imin;
                params.last = tbl_prop->imax;
                params.block_stat = true;
                params.buckets = true;
                params.entries_per_bucket = NHOPS_PER_MEMB_ENTRY_OVERLAY;
                params.reverse = false;

                list_hdl = NULL;
                rv = shr_itbm_list_create(params, "bcmecmpMemOverlayList",
                                          &list_hdl);
                if (rv == SHR_E_NONE && list_hdl != NULL) {
                    tbl_prop->list_hdl = list_hdl;
                    LOG_VERBOSE(BSL_LOG_MODULE,
                                (BSL_META_U(unit,
                                            "Succeed to re-create overlay"
                                            " memb list.\n")));
                }
            }
            if (SHR_FAILURE(rv)) {
                LOG_ERROR(BSL_LOG_MODULE,
                          (BSL_META_U(unit,
                                      "Fail to recover overlay memb list rv %d,"
                                      "new ovlay bank number %d.\n"),
                                      rv, num_o_memb_bank));
            }
        }

        if (list_chg[BCMECMP_MEMB_LIST_OLAY_WEIGHT]) {
            memb_imin = bcmdrd_pt_index_min(unit,
                                            BCMECMP_TBL_DRD_SID(unit, member,
                                                    BCMECMP_GRP_TYPE_UNDERLAY));
            memb_imax = bcmdrd_pt_index_max(unit,
                                            BCMECMP_TBL_DRD_SID(unit, member,
                                                    BCMECMP_GRP_TYPE_UNDERLAY));
            tbl_prop =  &(ecmp_info->member[BCMECMP_GRP_TYPE_OVERLAY_WEIGHTED]);
            SHR_NULL_CHECK(tbl_prop, SHR_E_INTERNAL);
            list_hdl = tbl_prop->list_hdl;
            if (list_hdl) {
                if (num_o_memb_bank) {
                    rv = shr_itbm_list_resize(list_hdl, tbl_prop->imin,
                                              tbl_prop->imax);
                }
            } else {
                shr_itbm_list_params_t_init(&params);
                params.unit = unit;
                params.comp_id = BCMMGMT_ECMP_COMP_ID;
                params.table_id = tbl_prop->drd_sid;
                params.min = memb_imin;
                params.max = memb_imax;
                params.first = tbl_prop->imin;
                params.last = tbl_prop->imax;
                params.block_stat = true;
                params.buckets = true;
                params.entries_per_bucket = NHOPS_PER_MEMB_ENTRY_WECMP;
                params.reverse = true;

                list_hdl = NULL;
                rv = shr_itbm_list_create(params, "bcmecmpMemOverlayList",
                                          &list_hdl);
                if (rv == SHR_E_NONE && list_hdl != NULL) {
                    tbl_prop->list_hdl = list_hdl;
                    LOG_VERBOSE(BSL_LOG_MODULE,
                                (BSL_META_U(unit,
                                            "Succeed to re-create overlay"
                                            "wmemb list.\n")));
                }
            }
            if (SHR_FAILURE(rv)) {
                LOG_ERROR(BSL_LOG_MODULE,
                          (BSL_META_U(unit,
                                      "Fail to recover overlay wmemb list rv %d,"
                                      "new ovlay bank number %d.\n"),
                                      rv, num_o_memb_bank));
            }
        }
    }
    SHR_FUNC_EXIT();
}

static int
bcm56996_a0_ecmp_itbm_memb_lists_resize(int unit, uint8_t num_o_memb_bank)
{
    uint8_t old_num_o_memb_bank = DEFAULT_NUM_O_MEMB_BANK;
    int memb_size;

    SHR_FUNC_ENTER(unit);

    memb_size = BCMECMP_TBL_SIZE(unit, member, BCMECMP_GRP_TYPE_OVERLAY);
    if (memb_size) {
        /*
         * Here the current bank number of overlay members is got. It is
         * calcuated by: the total overlay member entry size divides NHOP number
         * per hardware entry and hardware entires number per bank.
         */
        old_num_o_memb_bank = memb_size / NHOPS_PER_MEMB_ENTRY_OVERLAY /
                              HW_ENTRIES_PER_BANK;
    }

    bcm56996_a0_ecmp_hw_memb_tbls_range_init(unit, num_o_memb_bank);

    if (num_o_memb_bank > old_num_o_memb_bank) {
        SHR_IF_ERR_EXIT
            (bcm56996_a0_ecmp_itbm_olay_memb_lists_increase(unit,
                                                            num_o_memb_bank,
                                                          old_num_o_memb_bank));
    } else {
        SHR_IF_ERR_EXIT
            (bcm56996_a0_ecmp_itbm_olay_memb_lists_reduce(unit,
                                                          num_o_memb_bank,
                                                          old_num_o_memb_bank));
    }

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56996_a0_itbm_group_cleanup(int unit)
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
bcm56996_a0_ecmp_tables_init(int unit, bool warm)
{
    SHR_FUNC_ENTER(unit);

    /* Initialize ECMP tables DRD-SID and LTD-SID details. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcm56996_a0_ecmp_sids_init(unit));

    /* Initialize ECMP hardware tables size and index range info. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcm56996_a0_ecmp_hw_tbls_range_init(unit, warm));

    /* Initialize ITBM list for hardware tables. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcm56996_a0_ecmp_itbm_lists_init(unit));

    /* Initialize ECMP tables logical and physical field details. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcm56996_a0_ecmp_fields_init(unit));

    exit:
        SHR_FUNC_EXIT();
}

static int
bcm56996_a0_ecmp_tables_clear(int unit)
{
    SHR_FUNC_ENTER(unit);

    /* Clear ECMP tables logical and physical field details. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcm56996_a0_ecmp_fields_cleanup(unit));

    /* Clear ECMP hardware tables size and index range info. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcm56996_a0_ecmp_hw_tbls_range_clear(unit));

    /* Clear the ITBM lists for ECMP hardware tables. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcm56996_a0_ecmp_itbm_lists_cleanup(unit));

    /* Clear ECMP tables DRD-SID and LTD-SID details. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcm56996_a0_ecmp_sids_clear(unit));

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56996_a0_ecmp_db_init(int unit, bool warm)
{
    bcmecmp_tbl_prop_t *tbl_ptr; /* Pointer to ECMP hardware table info. */
    size_t tbl_size; /* ECMP group or member table size. */
    uint32_t ha_alloc_sz = 0; /* Size of allocated HA memory block. */
    uint32_t tot_ha_mem = 0; /* Total HA memory allocated by ECMP CTH. */
    uint32_t blk_sig = 0x56996000; /* ECMP HA block signature. */
    bcmecmp_hw_entry_ha_blk_t *hw_array_ptr;
    bcmecmp_grp_ha_blk_t *grp_array_ptr;

    SHR_FUNC_ENTER(unit);

    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit,
                         "%s warm = %s.\n"),
              __func__,
              warm ? "TRUE" : "FALSE"));

    switch (BCMECMP_MODE(unit)) {
        case BCMECMP_MODE_HIER:
            /* Because ITBM is used, member entry table is not needed. */

            /*
             * Calculate the size of HA memory block to be allocated for storing
             * ECMP group table active array elements.
             */
            tbl_size = (size_t)BCMECMP_TBL_SIZE
                                    (unit, group, BCMECMP_GRP_TYPE_OVERLAY) +
                       (size_t)BCMECMP_TBL_SIZE
                                    (unit, group, BCMECMP_GRP_TYPE_UNDERLAY);

            /* Allocate memory for active Group table entries array elements. */
            SHR_IF_ERR_EXIT
                (bcmecmp_hw_entry_ha_alloc(unit, warm, tbl_size,
                                           BCMECMP_GRP_TBL_SUB_COMP_ID,
                                           (blk_sig |
                                            BCMECMP_GRP_TBL_SUB_COMP_ID),
                                           &hw_array_ptr, &ha_alloc_sz));
            tbl_ptr = BCMECMP_TBL_PTR(unit, group, BCMECMP_GRP_TYPE_OVERLAY);
            tbl_ptr->ent_arr = hw_array_ptr->array;
            tot_ha_mem += ha_alloc_sz;

            (BCMECMP_TBL_PTR
                (unit, group,
                 BCMECMP_GRP_TYPE_OVERLAY_WEIGHTED))->ent_arr = tbl_ptr->ent_arr;

            (BCMECMP_TBL_PTR
                (unit, group,
                 BCMECMP_GRP_TYPE_UNDERLAY))->ent_arr = tbl_ptr->ent_arr;

            (BCMECMP_TBL_PTR
                (unit, group,
                BCMECMP_GRP_TYPE_UNDERLAY_WEIGHTED))->ent_arr = tbl_ptr->ent_arr;
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
            BCMECMP_INFO(unit).grp_arr_sz = 0;
        }

        switch (BCMECMP_MODE(unit)) {
            case BCMECMP_MODE_HIER:
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
                (BCMECMP_TBL_PTR
                    (unit, group,
                     BCMECMP_GRP_TYPE_OVERLAY_WEIGHTED))->ent_arr = NULL;

                (BCMECMP_TBL_PTR
                    (unit, group,
                     BCMECMP_GRP_TYPE_UNDERLAY))->ent_arr = NULL;

                (BCMECMP_TBL_PTR
                    (unit, group,
                    BCMECMP_GRP_TYPE_UNDERLAY_WEIGHTED))->ent_arr = NULL;
                break;

            default:
                SHR_ERR_EXIT(SHR_E_INTERNAL);
        }
    }

    SHR_FUNC_EXIT();
}

static int
bcm56996_a0_ecmp_db_clear(int unit)
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
        case BCMECMP_MODE_HIER:
            /* Clear Group table active array base pointers. */
            tbl_ptr = BCMECMP_TBL_PTR(unit, group, BCMECMP_GRP_TYPE_OVERLAY);
            tbl_ptr->ent_arr = NULL;

            tbl_ptr = BCMECMP_TBL_PTR(unit, group, BCMECMP_GRP_TYPE_UNDERLAY);
            tbl_ptr->ent_arr = NULL;

            /* Clear weighted ECMP Group table active array base pointers. */
            tbl_ptr =
                BCMECMP_TBL_PTR(unit, group, BCMECMP_GRP_TYPE_OVERLAY_WEIGHTED);
            tbl_ptr->ent_arr = NULL;

            tbl_ptr =
                BCMECMP_TBL_PTR(unit, group, BCMECMP_GRP_TYPE_UNDERLAY_WEIGHTED);
            tbl_ptr->ent_arr = NULL;
            break;

        default:
            SHR_EXIT();
    }

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56996_a0_ecmp_abort_db_init(int unit, bool warm)
{
    bcmecmp_tbl_prop_t *tbl_ptr; /* Pointer to ECMP hardware table info. */
    size_t tbl_size; /* ECMP group or member table size. */
    uint32_t ha_alloc_sz = 0; /* Size of allocated HA memory block. */
    uint32_t tot_ha_mem = 0; /* Total HA memory allocated by ECMP CTH. */
    uint32_t blk_sig = 0x56996000; /* ECMP HA block signature. */
    bcmecmp_hw_entry_ha_blk_t *hw_array_ptr;
    bcmecmp_grp_ha_blk_t *grp_array_ptr;

    SHR_FUNC_ENTER(unit);

    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit,
                         "%s warm = %s.\n"),
              __func__,
              warm ? "TRUE" : "FALSE"));

    switch (BCMECMP_MODE(unit)) {
        case BCMECMP_MODE_HIER:
            /*
             * Calculate the size of HA memory block to be allocated for storing
             * ECMP group table active array elements.
             */
            tbl_size = (size_t)BCMECMP_TBL_SIZE
                                    (unit, group, BCMECMP_GRP_TYPE_OVERLAY) +
                       (size_t)BCMECMP_TBL_SIZE
                                    (unit, group, BCMECMP_GRP_TYPE_UNDERLAY);

            SHR_IF_ERR_EXIT
                (bcmecmp_hw_entry_ha_alloc(unit, warm, tbl_size,
                                           BCMECMP_GRP_TBL_BK_SUB_COMP_ID,
                                           (blk_sig |
                                            BCMECMP_GRP_TBL_BK_SUB_COMP_ID),
                                           &hw_array_ptr, &ha_alloc_sz));
            tbl_ptr = BCMECMP_TBL_PTR(unit, group, BCMECMP_GRP_TYPE_OVERLAY);
            tbl_ptr->ent_bk_arr = hw_array_ptr->array;
            tot_ha_mem += ha_alloc_sz;

            (BCMECMP_TBL_PTR
                (unit, group,
                 BCMECMP_GRP_TYPE_OVERLAY_WEIGHTED))->ent_bk_arr =
                                                            tbl_ptr->ent_bk_arr;

            (BCMECMP_TBL_PTR
                (unit, group,
                 BCMECMP_GRP_TYPE_UNDERLAY))->ent_bk_arr =
                                                            tbl_ptr->ent_bk_arr;

            (BCMECMP_TBL_PTR
                (unit, group,
                 BCMECMP_GRP_TYPE_UNDERLAY_WEIGHTED))->ent_bk_arr =
                                                            tbl_ptr->ent_bk_arr;
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
            case BCMECMP_MODE_HIER:
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

                (BCMECMP_TBL_PTR
                    (unit, group,
                     BCMECMP_GRP_TYPE_OVERLAY_WEIGHTED))->ent_bk_arr = NULL;

                (BCMECMP_TBL_PTR
                    (unit, group,
                     BCMECMP_GRP_TYPE_UNDERLAY))->ent_bk_arr = NULL;

                (BCMECMP_TBL_PTR
                    (unit, group,
                    BCMECMP_GRP_TYPE_UNDERLAY_WEIGHTED))->ent_bk_arr = NULL;
                break;

            default:
                SHR_ERR_EXIT(SHR_E_INTERNAL);
        }
    }

    SHR_FUNC_EXIT();
}

static int
bcm56996_a0_ecmp_abort_db_clear(int unit)
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
        case BCMECMP_MODE_HIER:
            /* Clear Group table entries array base pointer. */
            tbl_ptr = BCMECMP_TBL_PTR(unit, group, BCMECMP_GRP_TYPE_OVERLAY);
            tbl_ptr->ent_bk_arr = NULL;

            tbl_ptr = BCMECMP_TBL_PTR(unit, group, BCMECMP_GRP_TYPE_UNDERLAY);
            tbl_ptr->ent_bk_arr = NULL;

            /* Clear weighted ECMP Group table entries array base pointer. */
            tbl_ptr =
                BCMECMP_TBL_PTR(unit, group, BCMECMP_GRP_TYPE_OVERLAY_WEIGHTED);
            tbl_ptr->ent_bk_arr = NULL;

            tbl_ptr =
               BCMECMP_TBL_PTR(unit, group, BCMECMP_GRP_TYPE_UNDERLAY_WEIGHTED);
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
bcm56996_a0_ecmp_entries_init(int unit, bool warm)
{
    size_t tbl_size;
    bcmecmp_info_t *ecmp_info = NULL;

    SHR_FUNC_ENTER(unit);

    /*
     * Allocate HA memory for managing ECMP group and member table
     * entries.
     */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcm56996_a0_ecmp_db_init(unit, warm));

    /* Initialize atomic transaction related SW database. */
    if (BCMECMP_ATOMIC_TRANS(unit)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcm56996_a0_ecmp_abort_db_init(unit, warm));
    }

    tbl_size = (size_t)BCMECMP_TBL_SIZE
                            (unit, group, BCMECMP_GRP_TYPE_OVERLAY) +
               (size_t)BCMECMP_TBL_SIZE
                            (unit, group, BCMECMP_GRP_TYPE_UNDERLAY);
    ecmp_info = &(bcmecmp_get_ecmp_control(unit)->ecmp_info);
    BCMECMP_ALLOC
        (ecmp_info->rhg, sizeof(*(ecmp_info->rhg)) * tbl_size,
         "bcmecmpDevRhgInfo");
    if (BCMECMP_ATOMIC_TRANS(unit)) {
        BCMECMP_ALLOC
            (ecmp_info->rhg_bk, sizeof(*(ecmp_info->rhg_bk)) * tbl_size,
             "bcmecmpDevRhgBkInfo");
    }

    exit:
        SHR_FUNC_EXIT();
}

static int
bcm56996_a0_ecmp_entries_cleanup(int unit)
{
    bcmecmp_info_t *ecmp_info = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcm56996_a0_itbm_group_cleanup(unit));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcm56996_a0_ecmp_db_clear(unit));

    /* Clear atomic transaction related SW database info. */
    if (BCMECMP_ATOMIC_TRANS(unit)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcm56996_a0_ecmp_abort_db_clear(unit));
    }

    ecmp_info = &(bcmecmp_get_ecmp_control(unit)->ecmp_info);
    BCMECMP_FREE(ecmp_info->rhg);
    BCMECMP_FREE(ecmp_info->rhg_bk);

    exit:
        SHR_FUNC_EXIT();
}
static int bcm56996_a0_ecmp_lt_entry_alloc(int unit)
{
    SHR_FUNC_ENTER(unit);

    /* Allocate memory to store LT Entry information. */
    BCMECMP_ALLOC
        (BCMECMP_LT_ENT_PTR_VOID(unit), sizeof(bcmecmp_lt_entry_t),
         "bcmecmpDevLtEntInfo");

    /* Allocate memory to store current LT Entry information. */
    BCMECMP_ALLOC
        (BCMECMP_CURRENT_LT_ENT_PTR_VOID(unit), sizeof(bcmecmp_lt_entry_t),
         "bcmecmpDevCurrentLtEntInfo");
    exit:
        SHR_FUNC_EXIT();
}

static void bcm56996_a0_ecmp_lt_entry_free(int unit)
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
bcm56996_a0_ecmp_info_init(int unit, bool warm)
{
    SHR_FUNC_ENTER(unit);

    /* Initialize ECMP mode to Hierachical. */
    BCMECMP_MODE(unit) = BCMECMP_MODE_HIER;

    /* Initialize ECMP tables information for Tomahawk4G device. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcm56996_a0_ecmp_tables_init(unit, warm));

    /*
     * Initialize ECMP Group and Member tables entries array for
     * Overlay and Underlay groups support.
     */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcm56996_a0_ecmp_entries_init(unit, warm));

    /* Allocate LT entry. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcm56996_a0_ecmp_lt_entry_alloc(unit));

    exit:
        SHR_FUNC_EXIT();
}

static int
bcm56996_a0_ecmp_info_cleanup(int unit)
{
    SHR_FUNC_ENTER(unit);

    /* Cleanup memory allocated for ECMP Group and Member table entries info. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcm56996_a0_ecmp_entries_cleanup(unit));

    /* Clear ECMP tables information stored in SW. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcm56996_a0_ecmp_tables_clear(unit));

    /* Initialize device ECMP mode to hardware reset default value. */
    BCMECMP_MODE(unit) = BCMECMP_MODE_FULL;

    /* Free LT entry. */
    bcm56996_a0_ecmp_lt_entry_free(unit);

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56996_a0_ecmp_mode_config(int unit)
{
    return SHR_E_NONE;
}

static int
bcm56996_a0_ecmp_memb_ent_range_get(int start_idx, int num_nhop, int nh_per_entry,
                                    int *ent_idx, int *ent_offset, int *ent_num)
{
    int tmp = 0;

    if (ent_idx == NULL || ent_offset == NULL || nh_per_entry == 0) {
        return SHR_E_PARAM;
    }

    *ent_idx = start_idx / nh_per_entry;
    *ent_offset = start_idx % nh_per_entry;

    if (*ent_offset != 0) {
        tmp++;
        num_nhop -= (nh_per_entry - *ent_offset);
    }

    if (num_nhop > 0) {
        tmp += num_nhop / nh_per_entry;
        if ((num_nhop % nh_per_entry) != 0) {
            tmp++;
        }
    }

    if (ent_num != NULL) {
        *ent_num = tmp;
    }

    return SHR_E_NONE;
}

static int
bcm56996_a0_ecmp_memb_table_pio_write(int unit, bcmecmp_lt_entry_t *lt_ent,
                                      bool rh_mode)
{
    bcm56996_a0_ecmp_memb_drd_fields_t *memb_flds;
    bcmecmp_grp_type_t gtype;
    uint32_t *pt_entry_buff = NULL;
    uint32_t *nhop_arr = NULL, val;
    int nhop_arr_idx = 0;
    int start_idx, num_paths, max_paths;
    int ent_idx, ent_offset;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(lt_ent, SHR_E_PARAM);

    gtype = lt_ent->grp_type;

    memb_flds = (bcm56996_a0_ecmp_memb_drd_fields_t *)BCMECMP_TBL_DRD_FIELDS
                                                          (unit, member, gtype);
    SHR_NULL_CHECK(memb_flds, SHR_E_INTERNAL);

    BCMECMP_ALLOC(pt_entry_buff,
                  BCMECMP_TBL_ENT_SIZE_MAX(unit) * sizeof(uint32_t),
                  "bcmecmpBcm56996MembTblPioWritePtBuf");

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
        (bcm56996_a0_ecmp_memb_ent_range_get(start_idx, max_paths,
                                             NHOPS_PER_MEMB_ENTRY, &ent_idx,
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
                (bcm56996_a0_ecmp_lt_entry_memb_read(unit, lt_ent, ent_idx, 1,
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
            (bcm56996_a0_ecmp_lt_entry_memb_write(unit, lt_ent, ent_idx, 1,
                                                  false, pt_entry_buff));

        ent_idx++;
        ent_offset = 0;
    }

exit:
    BCMECMP_FREE(pt_entry_buff);
    SHR_FUNC_EXIT();
}

static int
bcm56996_a0_ecmp_memb_table_dma_write(int unit, bcmecmp_lt_entry_t *lt_ent,
                                      bool rh_mode, int nh_per_ent, bool ovlay)
{
    bcm56996_a0_ecmp_memb_drd_fields_t *memb_flds;
    bcmecmp_grp_type_t gtype;
    uint32_t *nhop_arr = NULL;
    uint32_t *dma_buf = NULL;
    uint32_t dma_alloc_sz = 0, ent_size, val;
    int nhop_arr_idx = 0, dma_buf_idx = 0;
    int start_idx, num_paths, max_paths;
    int ent_idx, ent_offset, ent_num;
    uint32_t *o_nhop_arr = NULL;
    uint32_t *uecmp_arr = NULL;
    uint8_t *u_ecmp_nhop_arr = NULL;
    bool *rh_u_ecmp_nhop_arr = NULL;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(lt_ent, SHR_E_PARAM);

    gtype = lt_ent->grp_type;

    memb_flds = (bcm56996_a0_ecmp_memb_drd_fields_t *)BCMECMP_TBL_DRD_FIELDS
                                                          (unit, member, gtype);
    SHR_NULL_CHECK(memb_flds, SHR_E_INTERNAL);

    start_idx = lt_ent->mstart_idx;
    if (rh_mode) {
        num_paths = lt_ent->rh_entries_cnt;
        max_paths = lt_ent->rh_entries_cnt;
        nhop_arr = (uint32_t *)lt_ent->rh_entries_arr;
        if (ovlay) {
            o_nhop_arr = (uint32_t *)lt_ent->rh_o_entries_arr;
            rh_u_ecmp_nhop_arr = lt_ent->rh_u_ecmp_nhop_arr;
            uecmp_arr = (uint32_t *)lt_ent->rh_entries_arr;
        }
    } else {
        num_paths = lt_ent->num_paths;
        max_paths = lt_ent->max_paths;
        nhop_arr = (uint32_t *)lt_ent->nhop_id;
        if (ovlay) {
            o_nhop_arr = (uint32_t *)lt_ent->o_nhop_id;
            u_ecmp_nhop_arr = (uint8_t *)lt_ent->u_ecmp_nhop;
            uecmp_arr = (uint32_t *)lt_ent->uecmp_id;
        }
    }

    SHR_IF_ERR_EXIT
        (bcm56996_a0_ecmp_memb_ent_range_get(start_idx, max_paths, nh_per_ent,
                                             &ent_idx, &ent_offset, &ent_num));

    ent_size = BCMECMP_TBL_ENT_SIZE(unit, member, gtype);
    dma_alloc_sz = (ent_size * sizeof(uint32_t) * ent_num);
    BCMECMP_ALLOC(dma_buf, dma_alloc_sz, "bcmecmpBcm56996GrpDmaWritePtDmaBuf");

    nhop_arr_idx = 0;
    dma_buf_idx = 0;
    start_idx = ent_idx;

    while (max_paths > 0) {
        /*
         * The group may not occupy all of four members of the first
         * and the last entry.
         */
        if (ent_offset != 0 || max_paths < nh_per_ent) {
            SHR_IF_ERR_EXIT
                (bcm56996_a0_ecmp_lt_entry_memb_read(unit, lt_ent, ent_idx, 1, false,
                                                     &dma_buf[dma_buf_idx * ent_size]));
        }

        while (ent_offset < nh_per_ent && max_paths > 0) {
            if (nhop_arr_idx >= num_paths) {
                ent_offset++;
                nhop_arr_idx++;
                max_paths--;
                continue;
            }
            if (!ovlay) {
                val = nhop_arr[nhop_arr_idx];
                SHR_IF_ERR_EXIT
                    (bcmdrd_pt_field_set(unit,
                                         BCMECMP_TBL_DRD_SID(unit, member, gtype),
                                         &dma_buf[dma_buf_idx * ent_size],
                                         memb_flds->ecmp_nhop_id[ent_offset],
                                         &(val)));
            } else {
                val = rh_mode ? rh_u_ecmp_nhop_arr[nhop_arr_idx] :
                      (uint32_t)u_ecmp_nhop_arr[nhop_arr_idx];
                SHR_IF_ERR_EXIT
                    (bcmdrd_pt_field_set(unit,
                                         BCMECMP_TBL_DRD_SID(unit, member, gtype),
                                         &dma_buf[dma_buf_idx * ent_size],
                                         memb_flds->u_ecmp_nhop[ent_offset],
                                         &(val)));
                if (val) {
                    SHR_IF_ERR_EXIT
                        (bcmdrd_pt_field_set(unit,
                                             BCMECMP_TBL_DRD_SID
                                                 (unit, member, gtype),
                                             &dma_buf[dma_buf_idx * ent_size],
                                             memb_flds->u_ecmp_id[ent_offset],
                                             &(uecmp_arr[nhop_arr_idx])));
                } else {
                    SHR_IF_ERR_EXIT
                        (bcmdrd_pt_field_set(unit,
                                             BCMECMP_TBL_DRD_SID
                                                 (unit, member, gtype),
                                             &dma_buf[dma_buf_idx * ent_size],
                                             memb_flds->u_nhop_id[ent_offset],
                                             &(nhop_arr[nhop_arr_idx])));
                }
                SHR_IF_ERR_EXIT
                    (bcmdrd_pt_field_set(unit,
                                         BCMECMP_TBL_DRD_SID(unit, member, gtype),
                                         &dma_buf[dma_buf_idx * ent_size],
                                         memb_flds->o_nhop_id[ent_offset],
                                         &(o_nhop_arr[nhop_arr_idx])));
            }
            ent_offset++;
            nhop_arr_idx++;
            max_paths--;
        }

        if (ovlay && nhop_arr_idx <= num_paths) {
            /* Overlay entry type should be set for every dma_buf_idx */
            val = MEMB_HECMP_ENTRY_TYPE;
            SHR_IF_ERR_EXIT
                (bcmdrd_pt_field_set(unit,
                                     BCMECMP_TBL_DRD_SID(unit, member, gtype),
                                     &dma_buf[dma_buf_idx * ent_size],
                                     memb_flds->entry_type,
                                     &(val)));
        }
        ent_idx++;
        ent_offset = 0;
        dma_buf_idx++;
    }

    SHR_IF_ERR_EXIT
        (bcm56996_a0_ecmp_lt_entry_memb_write(unit, lt_ent, start_idx, ent_num,
                                              true, dma_buf));

exit:
    BCMECMP_FREE(dma_buf);
    SHR_FUNC_EXIT();
}

static int
bcm56996_a0_ecmp_memb_table_pio_clear(int unit, bcmecmp_lt_entry_t *lt_ent,
                                      bool rh_mode)
{
    bcm56996_a0_ecmp_memb_drd_fields_t *memb_flds;
    bcmecmp_grp_type_t gtype;
    uint32_t *pt_entry_buff = NULL;
    uint32_t val = 0;
    int start_idx, max_paths;
    int ent_idx, ent_offset;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(lt_ent, SHR_E_PARAM);

    gtype = lt_ent->grp_type;

    memb_flds = (bcm56996_a0_ecmp_memb_drd_fields_t *)BCMECMP_TBL_DRD_FIELDS
                                                          (unit, member, gtype);
    SHR_NULL_CHECK(memb_flds, SHR_E_INTERNAL);

    BCMECMP_ALLOC(pt_entry_buff,
                  BCMECMP_TBL_ENT_SIZE_MAX(unit) * sizeof(uint32_t),
                  "bcmecmpBcm56996MembTblPioClearPtBuf");

    start_idx = BCMECMP_GRP_MEMB_TBL_START_IDX(unit, lt_ent->ecmp_id);
    if (rh_mode) {
        max_paths = BCMECMP_GRP_RH_ENTRIES_CNT(unit, lt_ent->ecmp_id);
    } else {
        max_paths = BCMECMP_GRP_MAX_PATHS(unit, lt_ent->ecmp_id);
    }

    SHR_IF_ERR_EXIT
        (bcm56996_a0_ecmp_memb_ent_range_get(start_idx, max_paths,
                                             NHOPS_PER_MEMB_ENTRY, &ent_idx,
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
                (bcm56996_a0_ecmp_lt_entry_memb_read(unit, lt_ent, ent_idx, 1,
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
            (bcm56996_a0_ecmp_lt_entry_memb_write(unit, lt_ent, ent_idx, 1,
                                                  false, pt_entry_buff));

        ent_idx++;
        ent_offset = 0;
    }

exit:
    BCMECMP_FREE(pt_entry_buff);
    SHR_FUNC_EXIT();
}

static int
bcm56996_a0_ecmp_memb_table_dma_clear(int unit, bcmecmp_lt_entry_t *lt_ent,
                                      bool rh_mode, int nh_per_ent, bool ovlay)
{
    bcm56996_a0_ecmp_memb_drd_fields_t *memb_flds;
    bcmecmp_grp_type_t gtype;
    uint32_t *dma_buf = NULL;
    uint32_t dma_alloc_sz = 0, dma_buf_idx;
    uint32_t ent_size, val = 0;
    int start_idx, max_paths;
    int ent_idx, ent_offset, ent_num;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(lt_ent, SHR_E_PARAM);

    gtype = lt_ent->grp_type;

    memb_flds = (bcm56996_a0_ecmp_memb_drd_fields_t *)BCMECMP_TBL_DRD_FIELDS
                                                          (unit, member, gtype);
    SHR_NULL_CHECK(memb_flds, SHR_E_INTERNAL);

    start_idx = BCMECMP_GRP_MEMB_TBL_START_IDX(unit, lt_ent->ecmp_id);
    if (rh_mode) {
        max_paths = BCMECMP_GRP_RH_ENTRIES_CNT(unit, lt_ent->ecmp_id);
    } else {
        max_paths = BCMECMP_GRP_MAX_PATHS(unit, lt_ent->ecmp_id);
    }

    SHR_IF_ERR_EXIT
        (bcm56996_a0_ecmp_memb_ent_range_get(start_idx, max_paths, nh_per_ent,
                                             &ent_idx, &ent_offset, &ent_num));

    ent_size = BCMECMP_TBL_ENT_SIZE(unit, member, gtype);
    dma_alloc_sz = (ent_size * sizeof(uint32_t) * ent_num);
    BCMECMP_ALLOC(dma_buf, dma_alloc_sz, "bcmecmpBcm56996GrpDmaClearPtDmaBuf");

    dma_buf_idx = 0;
    start_idx = ent_idx;

    while (max_paths > 0) {
        /*
         * The group may not occupy all of four members of the first
         * and the last entry.
         */
        if (ent_offset != 0 || max_paths < nh_per_ent) {
            SHR_IF_ERR_EXIT
                (bcm56996_a0_ecmp_lt_entry_memb_read
                     (unit, lt_ent, ent_idx, 1, false,
                      &dma_buf[dma_buf_idx * ent_size]));

            while (ent_offset < nh_per_ent && max_paths > 0) {
                if (!ovlay) {
                    SHR_IF_ERR_EXIT
                        (bcmdrd_pt_field_set(unit,
                                             BCMECMP_TBL_DRD_SID
                                                          (unit, member, gtype),
                                             &dma_buf[dma_buf_idx * ent_size],
                                             memb_flds->ecmp_nhop_id[ent_offset],
                                             &(val)));
                } else {
                    SHR_IF_ERR_EXIT
                        (bcmdrd_pt_field_set(unit,
                                             BCMECMP_TBL_DRD_SID
                                                 (unit, member, gtype),
                                             &dma_buf[dma_buf_idx * ent_size],
                                             memb_flds->u_ecmp_nhop[ent_offset],
                                             &(val)));
                    SHR_IF_ERR_EXIT
                        (bcmdrd_pt_field_set(unit,
                                             BCMECMP_TBL_DRD_SID
                                                 (unit, member, gtype),
                                             &dma_buf[dma_buf_idx * ent_size],
                                             memb_flds->u_ecmp_id[ent_offset],
                                             &(val)));
                    SHR_IF_ERR_EXIT
                        (bcmdrd_pt_field_set(unit,
                                             BCMECMP_TBL_DRD_SID
                                                 (unit, member, gtype),
                                             &dma_buf[dma_buf_idx * ent_size],
                                             memb_flds->u_nhop_id[ent_offset],
                                             &(val)));
                    SHR_IF_ERR_EXIT
                        (bcmdrd_pt_field_set(unit,
                                             BCMECMP_TBL_DRD_SID
                                                 (unit, member, gtype),
                                             &dma_buf[dma_buf_idx * ent_size],
                                             memb_flds->o_nhop_id[ent_offset],
                                             &(val)));
                }
                ent_offset++;
                max_paths--;
            }
            ent_idx++;
            ent_offset = 0;
            dma_buf_idx++;
        } else {
            ent_offset = max_paths / nh_per_ent;

            max_paths %= nh_per_ent;
            ent_idx += ent_offset;
            dma_buf_idx += ent_offset;
            ent_offset = 0;
        }
    }

    SHR_IF_ERR_EXIT
        (bcm56996_a0_ecmp_lt_entry_memb_write(unit, lt_ent, start_idx, ent_num,
                                              true, dma_buf));

exit:
    BCMECMP_FREE(dma_buf);
    SHR_FUNC_EXIT();
}

static int
bcm56996_a0_ecmp_memb_table_pio_read(int unit, bcmecmp_lt_entry_t *lt_ent,
                                     bool rh_mode)
{
    bcm56996_a0_ecmp_memb_drd_fields_t *memb_flds;
    bcmecmp_grp_type_t gtype;
    uint32_t *pt_entry_buff = NULL;
    uint32_t *nhop_arr = NULL;
    uint32_t val;
    int nhop_arr_idx = 0, start_idx, max_paths;
    int ent_idx, ent_offset;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(lt_ent, SHR_E_PARAM);

    gtype = lt_ent->grp_type;

    memb_flds = (bcm56996_a0_ecmp_memb_drd_fields_t *)BCMECMP_TBL_DRD_FIELDS
                                                          (unit, member, gtype);
    SHR_NULL_CHECK(memb_flds, SHR_E_INTERNAL);

    BCMECMP_ALLOC(pt_entry_buff,
                  BCMECMP_TBL_ENT_SIZE_MAX(unit) * sizeof(uint32_t),
                  "bcmecmpBcm56996MembTblPioReadPtBuf");

    start_idx = lt_ent->mstart_idx;
    if (rh_mode) {
        max_paths = lt_ent->rh_entries_cnt;
        nhop_arr = (uint32_t *)lt_ent->rh_entries_arr;
    } else {
        max_paths = lt_ent->max_paths;
        nhop_arr = (uint32_t *)lt_ent->nhop_id;
    }

    SHR_IF_ERR_EXIT
        (bcm56996_a0_ecmp_memb_ent_range_get(start_idx, max_paths,
                                             NHOPS_PER_MEMB_ENTRY, &ent_idx,
                                             &ent_offset, NULL));

    nhop_arr_idx = 0;

    while (max_paths > 0) {
        sal_memset(pt_entry_buff, 0,
                   BCMECMP_TBL_ENT_SIZE_MAX(unit) * sizeof(uint32_t));

        SHR_IF_ERR_EXIT
                (bcm56996_a0_ecmp_lt_entry_memb_read(unit, lt_ent, ent_idx, 1,
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
bcm56996_a0_ecmp_memb_table_dma_read(int unit, bcmecmp_lt_entry_t *lt_ent,
                                     bool rh_mode, int nh_per_ent, bool ovlay)
{
    bcm56996_a0_ecmp_memb_drd_fields_t *memb_flds;
    bcmecmp_grp_type_t gtype;
    uint32_t *nhop_arr = NULL;
    uint32_t *dma_buf = NULL;
    uint32_t dma_alloc_sz = 0, ent_size, val;
    int nhop_arr_idx = 0, dma_buf_idx = 0;
    int start_idx, max_paths;
    int ent_idx, ent_offset, ent_num;
    uint32_t *o_nhop_arr = NULL;
    uint32_t *uecmp_arr = NULL;
    uint8_t *u_ecmp_nhop_arr = NULL;
    bool *rh_u_ecmp_nhop_arr = NULL;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(lt_ent, SHR_E_PARAM);

    gtype = lt_ent->grp_type;

    memb_flds = (bcm56996_a0_ecmp_memb_drd_fields_t *)BCMECMP_TBL_DRD_FIELDS
                                                          (unit, member, gtype);
    SHR_NULL_CHECK(memb_flds, SHR_E_INTERNAL);

    start_idx = lt_ent->mstart_idx;
    if (rh_mode) {
        max_paths = lt_ent->rh_entries_cnt;
        nhop_arr = (uint32_t *)lt_ent->rh_entries_arr;
        if (ovlay) {
            o_nhop_arr = (uint32_t *)lt_ent->rh_o_entries_arr;
            rh_u_ecmp_nhop_arr = lt_ent->rh_u_ecmp_nhop_arr;
            uecmp_arr = (uint32_t *)lt_ent->rh_entries_arr;
        }
    } else {
        max_paths = lt_ent->max_paths;
        nhop_arr = (uint32_t *)lt_ent->nhop_id;
        if (ovlay) {
            o_nhop_arr = (uint32_t *)lt_ent->o_nhop_id;
            u_ecmp_nhop_arr = (uint8_t *)lt_ent->u_ecmp_nhop;
            uecmp_arr = (uint32_t *)lt_ent->uecmp_id;
        }
    }

    SHR_IF_ERR_EXIT
        (bcm56996_a0_ecmp_memb_ent_range_get(start_idx, max_paths,
                                             nh_per_ent, &ent_idx,
                                             &ent_offset, &ent_num));

    ent_size = BCMECMP_TBL_ENT_SIZE(unit, member, gtype);
    dma_alloc_sz = (ent_size * sizeof(uint32_t) * ent_num);
    BCMECMP_ALLOC(dma_buf, dma_alloc_sz, "bcmecmpBcm56996GrpDmaReadPtDmaBuf");

    SHR_IF_ERR_EXIT
        (bcm56996_a0_ecmp_lt_entry_memb_read(unit, lt_ent, ent_idx, ent_num,
                                             true, dma_buf));

    nhop_arr_idx = 0;
    dma_buf_idx = 0;

    while (max_paths > 0) {
        while (ent_offset < nh_per_ent && max_paths > 0) {
            if (!ovlay) {
                SHR_IF_ERR_EXIT
                    (bcmdrd_pt_field_get(unit,
                                         BCMECMP_TBL_DRD_SID(unit, member, gtype),
                                         &dma_buf[dma_buf_idx * ent_size],
                                         memb_flds->ecmp_nhop_id[ent_offset],
                                         &val));
                nhop_arr[nhop_arr_idx] = (bcmecmp_nhop_id_t)val;
            } else {
                SHR_IF_ERR_EXIT
                    (bcmdrd_pt_field_get(unit,
                                         BCMECMP_TBL_DRD_SID(unit, member, gtype),
                                         &dma_buf[dma_buf_idx * ent_size],
                                         memb_flds->u_ecmp_nhop[ent_offset],
                                         &(val)));
                if (rh_mode) {
                    rh_u_ecmp_nhop_arr[nhop_arr_idx] = (bool)val;
                } else {
                    u_ecmp_nhop_arr[nhop_arr_idx] = (uint8_t)val;
                }
                if (val) {
                    SHR_IF_ERR_EXIT
                        (bcmdrd_pt_field_get(unit,
                                             BCMECMP_TBL_DRD_SID
                                                 (unit, member, gtype),
                                             &dma_buf[dma_buf_idx * ent_size],
                                             memb_flds->u_ecmp_id[ent_offset],
                                             &(val)));
                    uecmp_arr[nhop_arr_idx] = val;
                } else {
                    SHR_IF_ERR_EXIT
                        (bcmdrd_pt_field_get(unit,
                                             BCMECMP_TBL_DRD_SID
                                                 (unit, member, gtype),
                                             &dma_buf[dma_buf_idx * ent_size],
                                             memb_flds->u_nhop_id[ent_offset],
                                             &(val)));
                    nhop_arr[nhop_arr_idx] = val;
                }

                SHR_IF_ERR_EXIT
                    (bcmdrd_pt_field_get(unit,
                                         BCMECMP_TBL_DRD_SID(unit, member, gtype),
                                         &dma_buf[dma_buf_idx * ent_size],
                                         memb_flds->o_nhop_id[ent_offset],
                                         &(val)));
                o_nhop_arr[nhop_arr_idx] = val;
            }

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
bcm56996_a0_ecmp_grp_table_write(int unit, bcmecmp_lt_entry_t *lt_ent,
                                 bool rh_mode)
{
    bcmecmp_grp_type_t gtype;
    bcm56996_a0_ecmp_grp_drd_fields_t *grp_flds;
    uint32_t *pt_entry_buff = NULL;
    uint32_t hw_npaths = 0;
    uint32_t val = 0;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(lt_ent, SHR_E_PARAM);

    gtype = lt_ent->grp_type;

    grp_flds = (bcm56996_a0_ecmp_grp_drd_fields_t *)BCMECMP_TBL_DRD_FIELDS
                                                        (unit, group, gtype);
    SHR_NULL_CHECK(grp_flds, SHR_E_INTERNAL);

    /* Allocate hardware entry buffer. */
    BCMECMP_ALLOC(pt_entry_buff,
                  BCMECMP_TBL_ENT_SIZE_MAX(unit) * sizeof(uint32_t),
                  "bcmecmpBcm56996GrpTableWritePtBuf");

    /* Set LB_MODE value. */
    if (!BCMECMP_GRP_IS_WEIGHTED(gtype)) {
        val = lt_ent->lb_mode;
    } else {
        if (!bcmecmp_grp_is_overlay(gtype) &&
            BCMECMP_WEIGHTED_MODE_MEMBER_WEIGHT == lt_ent->weighted_mode) {
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
            if (BCMECMP_GRP_TYPE_UNDERLAY_WEIGHTED != gtype ||
                (BCMECMP_LT_GRP_IS_MEMBER_WEIGHT(unit, lt_ent) &&
                 BCM56996_WEIGHTED_SIZE_0_511 == lt_ent->weighted_size)) {
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

    /* Write entry to L3_ECMP_GROUP/L3_ECMP_GROUP_HIRARCHICAL table. */
    SHR_IF_ERR_EXIT
        (bcm56996_a0_ecmp_lt_entry_grp_write(unit, lt_ent, pt_entry_buff));

exit:
    BCMECMP_FREE(pt_entry_buff);
    SHR_FUNC_EXIT();
}

static int
bcm56996_a0_ecmp_grp_table_clear(int unit, bcmecmp_lt_entry_t *lt_ent,
                                 bool rh_mode)
{
    uint32_t *pt_entry_buff = NULL;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(lt_ent, SHR_E_PARAM);

    BCMECMP_ALLOC(pt_entry_buff,
                  BCMECMP_TBL_ENT_SIZE_MAX(unit) * sizeof(uint32_t),
                  "bcmecmpBcm56996GrpTableClearPtBuf");

    SHR_IF_ERR_EXIT
        (bcm56996_a0_ecmp_lt_entry_grp_write(unit, lt_ent, pt_entry_buff));

exit:
    BCMECMP_FREE(pt_entry_buff);
    SHR_FUNC_EXIT();
}

static uint32_t
bcm56996_a0_weight_size_to_max_paths(uint32_t weighted_size)
{
    uint32_t num_paths;

    switch (weighted_size) {
        case BCM56996_OVLAY_WEIGHTED_SIZE_0_127:
            num_paths = 128;
            break;
        case BCM56996_WEIGHTED_SIZE_0_511:
            num_paths = 512;
            break;
        case BCM56996_WEIGHTED_SIZE_1K:
            num_paths = 1024;
            break;
        case BCM56996_WEIGHTED_SIZE_2K:
            num_paths = 2048;
            break;
        case BCM56996_WEIGHTED_SIZE_4K:
            num_paths = 4096;
            break;
        default:
            num_paths = 0;
            break;
    }

    return num_paths;
}

static int
bcm56996_a0_ecmp_grp_table_read(int unit, bcmecmp_lt_entry_t *lt_ent,
                                bool rh_mode)
{
    bcmecmp_grp_type_t gtype = BCMECMP_GRP_TYPE_COUNT;
    bcm56996_a0_ecmp_grp_drd_fields_t *grp_flds;
    uint32_t *pt_entry_buff = NULL;
    uint32_t num_paths[1] = {0};
    uint32_t lb_mode[1];
    uint32_t val = 0;
    bcmecmp_lt_field_attrs_t *lt_fattr = NULL;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(lt_ent, SHR_E_PARAM);

    gtype = lt_ent->grp_type;

    grp_flds = (bcm56996_a0_ecmp_grp_drd_fields_t *)BCMECMP_TBL_DRD_FIELDS
                                                        (unit, group, gtype);
    SHR_NULL_CHECK(grp_flds, SHR_E_INTERNAL);

    BCMECMP_ALLOC(pt_entry_buff,
                  BCMECMP_TBL_ENT_SIZE_MAX(unit) * sizeof(uint32_t),
                  "bcmecmpBcm56996GrpTableReadPtBuf");

    SHR_IF_ERR_EXIT
        (bcm56996_a0_ecmp_lt_entry_grp_read(unit, lt_ent, pt_entry_buff));

    /* Get LB_MODE field value from ECMP Group table entry. */
    SHR_IF_ERR_EXIT
        (bcmdrd_pt_field_get(unit,
                             BCMECMP_TBL_DRD_SID(unit, group, gtype),
                             pt_entry_buff,
                             grp_flds->lb_mode,
                             lb_mode));
    if (!BCMECMP_GRP_IS_WEIGHTED(gtype)) {
        lt_ent->lb_mode = lb_mode[0];
    } else {
        if (lb_mode[0] == 0) {
            /* ECMP is not set to HW, for example numb_paths=0. */
            lt_ent->weighted_mode = BCMECMP_WEIGHTED_MODE_MEMBER_REPLICATION;
            lt_fattr = (bcmecmp_lt_field_attrs_t *)
                       ecmp_ctrl_local[unit]->ecmp_info.group[gtype].lt_fattrs;
            if (lt_fattr &&
                lt_fattr->def_weighted_size != (uint32_t)BCMECMP_INVALID) {
                lt_ent->weighted_size = lt_fattr->def_weighted_size;
            }
        } else if (lb_mode[0] >= BCM56996_WEIGHTED_SIZE_0_511
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
               (lt_ent->weighted_size != BCM56996_WEIGHTED_SIZE_0_511 &&
                lt_ent->weighted_size != BCM56996_OVLAY_WEIGHTED_SIZE_0_127)) {
        if (lb_mode[0] != 0) {
            lt_ent->num_paths = bcm56996_a0_weight_size_to_max_paths
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
bcm56996_a0_ecmp_mw_memb_table_dma_read(int unit, bcmecmp_lt_entry_t *lt_ent)
{
    return SHR_E_UNAVAIL;
}

static int
bcm56996_a0_ecmp_mw_memb_table_dma_write(int unit, bcmecmp_lt_entry_t *lt_ent,
                                         bool ovlay)
{
    bcmecmp_member_weight_grp_t *member_weight_grp = NULL;
    bcm56996_a0_ecmp_memb_drd_fields_t *memb_flds;
    bcmecmp_grp_type_t gtype;
    uint32_t *dma_buf = NULL;
    uint32_t dma_alloc_sz = 0, ent_size, val;
    int nhop_arr_idx = 0, start_idx;
    int *m_idx_arr = NULL;
    uint32_t i;
    int memb_idx= 0;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(lt_ent, SHR_E_PARAM);

    gtype = lt_ent->grp_type;

    memb_flds = (bcm56996_a0_ecmp_memb_drd_fields_t *)BCMECMP_TBL_DRD_FIELDS
                                                          (unit, member, gtype);
    SHR_NULL_CHECK(memb_flds, SHR_E_INTERNAL);

    BCMECMP_ALLOC(member_weight_grp, sizeof(bcmecmp_member_weight_grp_t),
                  "bcmecmpBcm56996GrpDmaWritePtDmaBuf");

    BCMECMP_ALLOC(member_weight_grp->nhop_id_a,
                  (sizeof(bcmecmp_nhop_id_t) * BCMECMP_NHOP_ARRAY_SIZE),
                  "bcmecmpBcm56996GrpNHOPADmaWritePtDmaBuf");

    BCMECMP_ALLOC(member_weight_grp->nhop_id_b,
                  (sizeof(bcmecmp_nhop_id_t) * BCMECMP_NHOP_ARRAY_SIZE),
                  "bcmecmpBcm56996GrpNHOPBDmaWritePtDmaBuf");

    BCMECMP_ALLOC(member_weight_grp->weight_a,
                  (sizeof(uint32_t) * BCMECMP_NHOP_ARRAY_SIZE),
                  "bcmecmpBcm56996GrpWtADmaWritePtDmaBuf");

    BCMECMP_ALLOC(member_weight_grp->weight_b,
                  (sizeof(uint32_t) * BCMECMP_NHOP_ARRAY_SIZE),
                  "bcmecmpBcm56996GrpWtBDmaWritePtDmaBuf");

    /* Distribute weight. */
    if (!ovlay) {
        SHR_IF_ERR_EXIT
            (bcmecmp_member_weight_distribute(unit,
                                              lt_ent->nhop_id,
                                              lt_ent->weight,
                                              lt_ent->num_paths,
                                              MEMBER_WEIGHT_QUANT_FACTOR,
                                              member_weight_grp));
    } else {
        if (lt_ent->num_paths > 0) {
            /* Input array index list as nhop list for overlay. */
            BCMECMP_ALLOC(m_idx_arr, sizeof(int) * lt_ent->num_paths,
                          "bcmecmpBcm56996GrpDmaWriteMidxArr");
            for (i = 0; i < lt_ent->num_paths; i++) {
                m_idx_arr[i] = i;
            }
            SHR_IF_ERR_EXIT
                (bcmecmp_member_weight_distribute(unit,
                                                  m_idx_arr,
                                                  lt_ent->weight,
                                                  lt_ent->num_paths,
                                                  MEMBER_WEIGHT_QUANT_FACTOR,
                                                  member_weight_grp));
        }
    }

    ent_size = BCMECMP_TBL_ENT_SIZE(unit, member, gtype);
    dma_alloc_sz = (ent_size * sizeof(uint32_t) * lt_ent->max_paths);
    BCMECMP_ALLOC(dma_buf, dma_alloc_sz, "bcmecmpBcm56996GrpDmaWritePtDmaBuf");

    for (nhop_arr_idx = 0; nhop_arr_idx < member_weight_grp->group_size;
         nhop_arr_idx++) {
        if (!ovlay) {
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
        } else {
            for (i = 0; i < 2; i++) {
                memb_idx = i ? member_weight_grp->nhop_id_a[nhop_arr_idx] :
                               member_weight_grp->nhop_id_b[nhop_arr_idx];

                val = lt_ent->u_ecmp_nhop[memb_idx];
                SHR_IF_ERR_EXIT
                    (bcmdrd_pt_field_set(unit,
                                         BCMECMP_TBL_DRD_SID
                                             (unit, member, gtype),
                                         &dma_buf[nhop_arr_idx * ent_size],
                                         memb_flds->wecmp_u_ecmp_nhop[i],
                                         &(val)));

                if (val) {
                    val = lt_ent->uecmp_id[memb_idx];
                    SHR_IF_ERR_EXIT
                        (bcmdrd_pt_field_set(unit,
                                             BCMECMP_TBL_DRD_SID
                                                 (unit, member, gtype),
                                             &dma_buf[nhop_arr_idx * ent_size],
                                             memb_flds->wecmp_u_ecmp_id[i],
                                             &(val)));
                } else {
                    val = lt_ent->nhop_id[memb_idx];
                    SHR_IF_ERR_EXIT
                        (bcmdrd_pt_field_set(unit,
                                             BCMECMP_TBL_DRD_SID
                                                 (unit, member, gtype),
                                             &dma_buf[nhop_arr_idx * ent_size],
                                             memb_flds->wecmp_nhop_id[i],
                                             &(val)));
                }

                val = lt_ent->o_nhop_id[memb_idx];
                SHR_IF_ERR_EXIT
                    (bcmdrd_pt_field_set(unit,
                                         BCMECMP_TBL_DRD_SID
                                             (unit, member, gtype),
                                         &dma_buf[nhop_arr_idx * ent_size],
                                         memb_flds->wecmp_o_nhop_id[i],
                                         &(val)));
            }
        }

        val = (uint32_t)member_weight_grp->weight_a[nhop_arr_idx] - 1;

        SHR_IF_ERR_EXIT
            (bcmdrd_pt_field_set(unit,
                                 BCMECMP_TBL_DRD_SID(unit, member, gtype),
                                 &dma_buf[nhop_arr_idx * ent_size],
                                 memb_flds->wecmp_weight,
                                 &(val)));

        if (!ovlay) {
            val = MEMB_WEIGHTED_ECMP_ENTRY_TYPE;
        } else {
            val = MEMB_WEIGHTED_HECMP_ENTRY_TYPE;
        }

        SHR_IF_ERR_EXIT
            (bcmdrd_pt_field_set(unit,
                                 BCMECMP_TBL_DRD_SID(unit, member, gtype),
                                 &dma_buf[nhop_arr_idx * ent_size],
                                 memb_flds->entry_type,
                                 &(val)));
    }

    start_idx = lt_ent->mstart_idx;
    SHR_IF_ERR_EXIT
        (bcm56996_a0_ecmp_lt_entry_memb_write(unit, lt_ent, start_idx,
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
    BCMECMP_FREE(m_idx_arr);
    SHR_FUNC_EXIT();
}

static int
bcm56996_a0_ecmp_mw_memb_table_dma_clear(int unit, bcmecmp_lt_entry_t *lt_ent)
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
    BCMECMP_ALLOC(dma_buf, dma_alloc_sz, "bcmecmpBcm56996GrpDmaWritePtDmaBuf");

    SHR_IF_ERR_EXIT
        (bcm56996_a0_ecmp_lt_entry_memb_write(unit, lt_ent, start_idx,
                                              max_paths, true, dma_buf));

exit:
    BCMECMP_FREE(dma_buf);
    SHR_FUNC_EXIT();
}

static int
bcm56996_a0_ecmp_mw_memb_dma_cpy(int unit,
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
    BCMECMP_ALLOC(dma_buf, dma_alloc_sz, "bcmecmp56996L3EcmpDmaCpy");

    /* Read from source fragment. */
    SHR_IF_ERR_EXIT
        (bcm56996_a0_ecmp_dfrag_memb_read(unit, sfrag, sfrag->mstart_idx,
                                          sfrag->gsize, true, dma_buf));


    /* Update WALI member table entries READ info. */
    if (wali->mwsize == 0) {
        wali->mwsize = BCMECMP_TBL_ENT_SIZE(unit, member, gtype);
    }
    wali->mr_ecount += sfrag->gsize;

    /* Write to destination fragment. */
    SHR_IF_ERR_EXIT
        (bcm56996_a0_ecmp_dfrag_memb_write(unit, dfrag, dfrag->mstart_idx,
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
bcm56996_a0_ecmp_mw_memb_dma_clr(int unit, bcmecmp_grp_defrag_t *frag)
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
    BCMECMP_ALLOC(dma_buf, dma_alloc_sz, "bcmecmp56996L3EcmpDmaClr");

    /* Write to destination fragment. */
    SHR_IF_ERR_EXIT
        (bcm56996_a0_ecmp_dfrag_memb_write(unit, frag, frag->mstart_idx, frag->gsize,
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
bcm56996_a0_ecmp_grp_ins(int unit, void *info)
{
    bcmecmp_lt_entry_t *lt_ent = NULL;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(info, SHR_E_PARAM);

    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit,
                         "bcm56996_a0_ecmp_grp_ins.\n")));

    /* Get LT entry data pointer. */
    lt_ent = (bcmecmp_lt_entry_t *)info;

    /* Program ECMP Member table. */
    if (BCMECMP_USE_DMA(unit)) {
        if (bcmecmp_grp_is_overlay(lt_ent->grp_type)) {
            if (BCMECMP_LT_GRP_IS_MEMBER_WEIGHT(unit, lt_ent)) {
                SHR_IF_ERR_EXIT
                    (bcm56996_a0_ecmp_mw_memb_table_dma_write(unit, lt_ent,
                                                              true));
            } else {
                SHR_IF_ERR_EXIT
                    (bcm56996_a0_ecmp_memb_table_dma_write
                         (unit, lt_ent, false,
                          NHOPS_PER_MEMB_ENTRY_OVERLAY,
                          true));
            }
        } else {
            if (BCMECMP_LT_GRP_IS_MEMBER_WEIGHT(unit, lt_ent)) {
                SHR_IF_ERR_EXIT
                    (bcm56996_a0_ecmp_mw_memb_table_dma_write(unit, lt_ent,
                                                              false));
            } else {
                SHR_IF_ERR_EXIT
                    (bcm56996_a0_ecmp_memb_table_dma_write(unit, lt_ent, false,
                                                           NHOPS_PER_MEMB_ENTRY,
                                                           false));
            }
        }
    } else {
        if (bcmecmp_grp_is_overlay(lt_ent->grp_type)) {
            SHR_ERR_EXIT(SHR_E_UNAVAIL);
        } else {
            if (BCMECMP_LT_GRP_IS_MEMBER_WEIGHT(unit, lt_ent)) {
                SHR_ERR_EXIT(SHR_E_UNAVAIL);
            } else {
                SHR_IF_ERR_EXIT
                    (bcm56996_a0_ecmp_memb_table_pio_write(unit, lt_ent, false));
            }
        }
    }

    /* Program ECMP Group table. */
    if (lt_ent->num_paths != 0) {
        SHR_IF_ERR_EXIT
            (bcm56996_a0_ecmp_grp_table_write(unit, lt_ent, false));
    } else {
        SHR_IF_ERR_EXIT
            (bcm56996_a0_ecmp_grp_table_clear(unit, lt_ent, false));
    }

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56996_a0_ecmp_grp_del(int unit, void *info)
{
    bcmecmp_lt_entry_t *lt_ent = NULL;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(info, SHR_E_PARAM);

    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit,
                         "bcm56996_a0_ecmp_grp_del.\n")));

    /* Get LT entry data pointer. */
    lt_ent = (bcmecmp_lt_entry_t *)info;

    /* Program ECMP Member table. */
    if (BCMECMP_USE_DMA(unit)) {
        if (BCMECMP_GRP_IS_MEMBER_WEIGHT(unit, lt_ent->ecmp_id)) {
            SHR_IF_ERR_EXIT
                (bcm56996_a0_ecmp_mw_memb_table_dma_clear(unit, lt_ent));
        } else {
            if (bcmecmp_grp_is_overlay(lt_ent->grp_type)) {
                /* Clean overlay ECMP*/
                SHR_IF_ERR_EXIT
                    (bcm56996_a0_ecmp_memb_table_dma_clear
                         (unit, lt_ent, false,
                          NHOPS_PER_MEMB_ENTRY_OVERLAY,
                          true));
            } else {
                SHR_IF_ERR_EXIT
                    (bcm56996_a0_ecmp_memb_table_dma_clear(unit, lt_ent, false,
                                                           NHOPS_PER_MEMB_ENTRY,
                                                           false));
            }
        }
    } else {
        if (BCMECMP_GRP_IS_MEMBER_WEIGHT(unit, lt_ent->ecmp_id)) {
            SHR_ERR_EXIT(SHR_E_UNAVAIL);
        } else {
            if (bcmecmp_grp_is_overlay(lt_ent->grp_type)) {
                SHR_ERR_EXIT(SHR_E_UNAVAIL);
            } else {
                SHR_IF_ERR_EXIT
                    (bcm56996_a0_ecmp_memb_table_pio_clear(unit, lt_ent, false));
            }
        }
    }

    /* Program ECMP Group table. */
    SHR_IF_ERR_EXIT
        (bcm56996_a0_ecmp_grp_table_clear(unit, lt_ent, false));

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56996_a0_ecmp_grp_get(int unit, void *info)
{
    bcmecmp_lt_entry_t *lt_ent = NULL;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(info, SHR_E_PARAM);

    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit,
                         "bcm56996_a0_ecmp_grp_get.\n")));

    /* Get LT entry data pointer. */
    lt_ent = (bcmecmp_lt_entry_t *)info;

    /* Program ECMP Group table. */
    SHR_IF_ERR_EXIT
        (bcm56996_a0_ecmp_grp_table_read(unit, lt_ent, false));

    /* Program ECMP Member table. */
    if (BCMECMP_USE_DMA(unit)) {
        if (BCMECMP_LT_GRP_IS_MEMBER_WEIGHT(unit, lt_ent)) {
            SHR_IF_ERR_EXIT
                (bcm56996_a0_ecmp_mw_memb_table_dma_read(unit, lt_ent));
        } else {
            if (bcmecmp_grp_is_overlay(lt_ent->grp_type)) {
                SHR_IF_ERR_EXIT
                    (bcm56996_a0_ecmp_memb_table_dma_read
                         (unit, lt_ent, false,
                          NHOPS_PER_MEMB_ENTRY_OVERLAY,
                          true));
            } else {
                SHR_IF_ERR_EXIT
                    (bcm56996_a0_ecmp_memb_table_dma_read(unit, lt_ent, false,
                                                          NHOPS_PER_MEMB_ENTRY,
                                                          false));
            }
        }
    } else {
        if (BCMECMP_LT_GRP_IS_MEMBER_WEIGHT(unit, lt_ent)) {
            SHR_ERR_EXIT(SHR_E_UNAVAIL);
        } else {
            if (bcmecmp_grp_is_overlay(lt_ent->grp_type)) {
                SHR_ERR_EXIT(SHR_E_UNAVAIL);
            } else {
                SHR_IF_ERR_EXIT
                    (bcm56996_a0_ecmp_memb_table_pio_read(unit, lt_ent, false));
            }
        }
    }

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56996_a0_ecmp_rh_grp_ins(int unit, void *info)
{
    bcmecmp_lt_entry_t *lt_ent = NULL;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(info, SHR_E_PARAM);

    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit,
                         "bcm56996_a0_ecmp_rh_grp_ins.\n")));

    /* Get LT entry data pointer. */
    lt_ent = (bcmecmp_lt_entry_t *)info;

    /* Program ECMP Member table. */
    if (BCMECMP_USE_DMA(unit)) {
        if (bcmecmp_grp_is_overlay(lt_ent->grp_type)) {
            SHR_IF_ERR_EXIT
                (bcm56996_a0_ecmp_memb_table_dma_write
                     (unit, lt_ent, true,
                      NHOPS_PER_MEMB_ENTRY_OVERLAY,
                      true));
        } else {
            SHR_IF_ERR_EXIT
                (bcm56996_a0_ecmp_memb_table_dma_write(unit, lt_ent, true,
                                                       NHOPS_PER_MEMB_ENTRY,
                                                       false));
        }
    } else {
        if (bcmecmp_grp_is_overlay(lt_ent->grp_type)) {
            SHR_ERR_EXIT(SHR_E_UNAVAIL);
        } else {
            SHR_IF_ERR_EXIT
                (bcm56996_a0_ecmp_memb_table_pio_write(unit, lt_ent, true));
        }
    }

    /* Program ECMP Group table. */
    if (lt_ent->rh_num_paths != 0) {
        SHR_IF_ERR_EXIT
            (bcm56996_a0_ecmp_grp_table_write(unit, lt_ent, true));
    } else {
        SHR_IF_ERR_EXIT
            (bcm56996_a0_ecmp_grp_table_clear(unit, lt_ent, true));
    }

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56996_a0_ecmp_rh_grp_del(int unit, void *info)
{
    bcmecmp_lt_entry_t *lt_ent = NULL;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(info, SHR_E_PARAM);

    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit,
                         "bcm56996_a0_ecmp_rh_grp_del.\n")));

    /* Get LT entry data pointer. */
    lt_ent = (bcmecmp_lt_entry_t *)info;

    /* Program ECMP Member table. */
    if (BCMECMP_USE_DMA(unit)) {
        if (bcmecmp_grp_is_overlay(lt_ent->grp_type)) {
            SHR_IF_ERR_EXIT
                (bcm56996_a0_ecmp_memb_table_dma_clear
                     (unit, lt_ent, true,
                      NHOPS_PER_MEMB_ENTRY_OVERLAY, true));
        } else {
            SHR_IF_ERR_EXIT
                (bcm56996_a0_ecmp_memb_table_dma_clear
                     (unit, lt_ent, true,
                      NHOPS_PER_MEMB_ENTRY, false));
        }
    } else {
        if (bcmecmp_grp_is_overlay(lt_ent->grp_type)) {
            SHR_ERR_EXIT(SHR_E_UNAVAIL);
        } else {
            SHR_IF_ERR_EXIT
                (bcm56996_a0_ecmp_memb_table_pio_clear(unit, lt_ent, true));
        }
    }

    /* Program ECMP Group table. */
    SHR_IF_ERR_EXIT
        (bcm56996_a0_ecmp_grp_table_clear(unit, lt_ent, true));

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56996_a0_ecmp_rh_grp_get(int unit, void *info)
{
    bcmecmp_lt_entry_t *lt_ent = NULL;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(info, SHR_E_PARAM);

    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit,
                         "bcm56996_a0_ecmp_rh_grp_get.\n")));

    /* Get LT entry data pointer. */
    lt_ent = (bcmecmp_lt_entry_t *)info;

    /* Program ECMP Group table. */
    SHR_IF_ERR_EXIT
        (bcm56996_a0_ecmp_grp_table_read(unit, lt_ent, true));

    /* Program ECMP Member table. */
    if (BCMECMP_USE_DMA(unit)) {
        if (bcmecmp_grp_is_overlay(lt_ent->grp_type)) {
            SHR_IF_ERR_EXIT
                (bcm56996_a0_ecmp_memb_table_dma_read
                     (unit, lt_ent, true,
                      NHOPS_PER_MEMB_ENTRY_OVERLAY, true));
        } else {
            SHR_IF_ERR_EXIT
                (bcm56996_a0_ecmp_memb_table_dma_read(unit, lt_ent, true,
                                                      NHOPS_PER_MEMB_ENTRY, false));
        }
    } else {
        if (bcmecmp_grp_is_overlay(lt_ent->grp_type)) {
            SHR_ERR_EXIT(SHR_E_UNAVAIL);
        } else {
            SHR_IF_ERR_EXIT
                (bcm56996_a0_ecmp_memb_table_pio_read(unit, lt_ent, true));
        }
    }

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56996_a0_ecmp_memb_dma_cpy(int unit,
                              bcmecmp_grp_defrag_t *dfrag,
                              bcmecmp_grp_defrag_t *sfrag,
                              int nh_per_ent,
                              bool ovlay)
{
    bcm56996_a0_ecmp_memb_drd_fields_t *memb_flds;
    bcmecmp_grp_type_t gtype;
    bcmecmp_wal_info_t *wali = NULL;
    bcmecmp_nhop_id_t *nhop_arr = NULL;
    uint32_t *dma_buf = NULL;
    uint32_t dma_alloc_sz = 0, ent_size, val;
    int nhop_arr_idx = 0, dma_buf_idx = 0;
    int start_idx, max_paths;
    int ent_idx, ent_offset, ent_num;
    uint32_t *o_nhop_arr = NULL;
    uint32_t *u_ecmp_nhop_arr = NULL;

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

    memb_flds = (bcm56996_a0_ecmp_memb_drd_fields_t *)BCMECMP_TBL_DRD_FIELDS
                                                          (unit, member, gtype);
    SHR_NULL_CHECK(memb_flds, SHR_E_INTERNAL);

    BCMECMP_ALLOC(nhop_arr, sizeof(bcmecmp_nhop_id_t) * sfrag->gsize,
                  "bcmecmp56996L3EcmpDmaCpy");
    if (ovlay) {
        BCMECMP_ALLOC(o_nhop_arr, sizeof(uint32_t) * sfrag->gsize,
                  "bcmecmp56996L3EcmpDmaCpyOvlay");
        BCMECMP_ALLOC(u_ecmp_nhop_arr, sizeof(uint32_t) * sfrag->gsize,
                  "bcmecmp56996L3EcmpDmaCpyUnlay");
    }

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
        (bcm56996_a0_ecmp_memb_ent_range_get(start_idx, max_paths,
                                             nh_per_ent, &ent_idx,
                                             &ent_offset, &ent_num));

    dma_alloc_sz = (ent_size * sizeof(uint32_t) * ent_num);
    BCMECMP_ALLOC(dma_buf, dma_alloc_sz, "bcmecmp56996L3EcmpDmaCpy");

    SHR_IF_ERR_EXIT
        (bcm56996_a0_ecmp_dfrag_memb_read(unit, sfrag, ent_idx, ent_num,
                                          true, dma_buf));

    nhop_arr_idx = 0;
    dma_buf_idx = 0;

    while (max_paths > 0) {
        while (ent_offset < nh_per_ent && max_paths > 0) {
            if (!ovlay) {
                SHR_IF_ERR_EXIT
                    (bcmdrd_pt_field_get(unit,
                                         BCMECMP_TBL_DRD_SID(unit, member, gtype),
                                         &dma_buf[dma_buf_idx * ent_size],
                                         memb_flds->ecmp_nhop_id[ent_offset],
                                         &val));
                nhop_arr[nhop_arr_idx] = (bcmecmp_nhop_id_t)val;
            } else {
                SHR_IF_ERR_EXIT
                    (bcmdrd_pt_field_get(unit,
                                         BCMECMP_TBL_DRD_SID(unit, member, gtype),
                                         &dma_buf[dma_buf_idx * ent_size],
                                         memb_flds->u_ecmp_nhop[ent_offset],
                                         &(val)));
                u_ecmp_nhop_arr[nhop_arr_idx] = val;
                if (val) {
                    SHR_IF_ERR_EXIT
                        (bcmdrd_pt_field_get(unit,
                                             BCMECMP_TBL_DRD_SID
                                                 (unit, member, gtype),
                                             &dma_buf[dma_buf_idx * ent_size],
                                             memb_flds->u_ecmp_id[ent_offset],
                                             &(val)));
                } else {
                    SHR_IF_ERR_EXIT
                        (bcmdrd_pt_field_get(unit,
                                             BCMECMP_TBL_DRD_SID
                                                 (unit, member, gtype),
                                             &dma_buf[dma_buf_idx * ent_size],
                                             memb_flds->u_nhop_id[ent_offset],
                                             &(val)));
                }
                nhop_arr[nhop_arr_idx] = val;

                SHR_IF_ERR_EXIT
                    (bcmdrd_pt_field_get(unit,
                                         BCMECMP_TBL_DRD_SID(unit, member, gtype),
                                         &dma_buf[dma_buf_idx * ent_size],
                                         memb_flds->o_nhop_id[ent_offset],
                                         &(val)));
                o_nhop_arr[nhop_arr_idx] = val;
            }

            ent_offset++;
            max_paths--;
            nhop_arr_idx++;
        }
        if (ovlay) {
            /* Overlay entry type should be set for every dma_buf_idx */
            val = MEMB_HECMP_ENTRY_TYPE;
            SHR_IF_ERR_EXIT
                (bcmdrd_pt_field_set(unit,
                                     BCMECMP_TBL_DRD_SID(unit, member, gtype),
                                     &dma_buf[dma_buf_idx * ent_size],
                                     memb_flds->entry_type,
                                     &(val)));
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
        (bcm56996_a0_ecmp_memb_ent_range_get(start_idx, max_paths,
                                             nh_per_ent, &ent_idx,
                                             &ent_offset, &ent_num));

    dma_alloc_sz = (ent_size * sizeof(uint32_t) * ent_num);
    BCMECMP_ALLOC(dma_buf, dma_alloc_sz, "bcmecmp56996L3EcmpDmaCpy");

    nhop_arr_idx = 0;
    dma_buf_idx = 0;
    start_idx = ent_idx;

    while (max_paths > 0) {
        /*
         * The group may not occupy all of four members of the first
         * and the last entry.
         */
        if (ent_offset != 0 || max_paths < nh_per_ent) {
            SHR_IF_ERR_EXIT
                (bcm56996_a0_ecmp_dfrag_memb_read(unit, dfrag, ent_idx, 1, false,
                                                  &dma_buf[dma_buf_idx * ent_size]));
        }

        while (ent_offset < nh_per_ent && max_paths > 0) {
            val = (uint32_t)nhop_arr[nhop_arr_idx];
            if (!ovlay) {
                SHR_IF_ERR_EXIT
                    (bcmdrd_pt_field_set(unit,
                                         BCMECMP_TBL_DRD_SID(unit, member, gtype),
                                         &dma_buf[dma_buf_idx * ent_size],
                                         memb_flds->ecmp_nhop_id[ent_offset],
                                         &(val)));
            } else {
                val = u_ecmp_nhop_arr[nhop_arr_idx];
                SHR_IF_ERR_EXIT
                    (bcmdrd_pt_field_set(unit,
                                         BCMECMP_TBL_DRD_SID(unit, member, gtype),
                                         &dma_buf[dma_buf_idx * ent_size],
                                         memb_flds->u_ecmp_nhop[ent_offset],
                                         &(val)));
                val = nhop_arr[nhop_arr_idx];
                if (u_ecmp_nhop_arr[nhop_arr_idx]) {
                    SHR_IF_ERR_EXIT
                        (bcmdrd_pt_field_set(unit,
                                             BCMECMP_TBL_DRD_SID
                                                 (unit, member, gtype),
                                             &dma_buf[dma_buf_idx * ent_size],
                                             memb_flds->u_ecmp_id[ent_offset],
                                             &(val)));
                } else {
                    SHR_IF_ERR_EXIT
                        (bcmdrd_pt_field_set(unit,
                                             BCMECMP_TBL_DRD_SID
                                                 (unit, member, gtype),
                                             &dma_buf[dma_buf_idx * ent_size],
                                             memb_flds->u_nhop_id[ent_offset],
                                             &(val)));
                }
                SHR_IF_ERR_EXIT
                    (bcmdrd_pt_field_set(unit,
                                         BCMECMP_TBL_DRD_SID(unit, member, gtype),
                                         &dma_buf[dma_buf_idx * ent_size],
                                         memb_flds->o_nhop_id[ent_offset],
                                         &(o_nhop_arr[nhop_arr_idx])));
            }

            ent_offset++;
            nhop_arr_idx++;
            max_paths--;
        }

        ent_idx++;
        ent_offset = 0;
        dma_buf_idx++;
    }

    SHR_IF_ERR_EXIT
        (bcm56996_a0_ecmp_dfrag_memb_write(unit, dfrag, start_idx, ent_num,
                                           true, dma_buf));

    BCMECMP_FREE(dma_buf);

    /* Update WALI member table entries write count. */
    wali->mw_ecount += ent_num;

exit:
    if (SHR_FUNC_ERR()) {
        BCMECMP_FREE(dma_buf);
    }
    BCMECMP_FREE(nhop_arr);
    BCMECMP_FREE(o_nhop_arr);
    BCMECMP_FREE(u_ecmp_nhop_arr);
    SHR_FUNC_EXIT();
}

static int
bcm56996_a0_ecmp_memb_cpy(int unit,
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
                (bcm56996_a0_ecmp_mw_memb_dma_cpy(unit, &dfrag, &sfrag));
        } else {
            if (bcmecmp_grp_is_overlay(sfrag.gtype)) {
                SHR_IF_ERR_EXIT
                    (bcm56996_a0_ecmp_memb_dma_cpy
                         (unit, &dfrag, &sfrag,
                          NHOPS_PER_MEMB_ENTRY_OVERLAY, true));
            } else {
                SHR_IF_ERR_EXIT
                    (bcm56996_a0_ecmp_memb_dma_cpy(unit, &dfrag, &sfrag,
                                                   NHOPS_PER_MEMB_ENTRY, false));
            }
        }
    } else {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56996_a0_ecmp_memb_dma_clr(int unit, bcmecmp_grp_defrag_t *mclr,
                              int nh_per_ent, bool ovlay)
{
    bcm56996_a0_ecmp_memb_drd_fields_t *memb_flds;
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
                            "bcm56996_a0_ecmp_memb_dma_clr.\n")));

    /* Get and validate ECMP WALI pointer. */
    wali = BCMECMP_WALI_PTR(unit);
    SHR_NULL_CHECK(wali, SHR_E_INTERNAL);

    gtype = mclr->gtype;
    ent_size = BCMECMP_TBL_ENT_SIZE(unit, member, gtype);

    memb_flds = (bcm56996_a0_ecmp_memb_drd_fields_t *)BCMECMP_TBL_DRD_FIELDS
                                                          (unit, member, gtype);
    SHR_NULL_CHECK(memb_flds, SHR_E_INTERNAL);

    start_idx = mclr->mstart_idx;
    max_paths = mclr->gsize;

    SHR_IF_ERR_EXIT
        (bcm56996_a0_ecmp_memb_ent_range_get(start_idx, max_paths,
                                             nh_per_ent, &ent_idx,
                                             &ent_offset, &ent_num));

    dma_alloc_sz = (ent_size * sizeof(uint32_t) * ent_num);
    BCMECMP_ALLOC(dma_buf, dma_alloc_sz, "bcmecmp56996L3EcmpDmaClr");

    dma_buf_idx = 0;
    start_idx = ent_idx;

    while (max_paths > 0) {
        /*
         * The group may not occupy all of four members of the first
         * and the last entry.
         */
        if (ent_offset != 0 || max_paths < nh_per_ent) {
            SHR_IF_ERR_EXIT
                (bcm56996_a0_ecmp_dfrag_memb_read(unit, mclr, ent_idx, 1, false,
                                                  &dma_buf[dma_buf_idx * ent_size]));

            while (ent_offset < nh_per_ent && max_paths > 0) {
                if (!ovlay) {
                    SHR_IF_ERR_EXIT
                        (bcmdrd_pt_field_set(unit,
                                             BCMECMP_TBL_DRD_SID(unit, member, gtype),
                                             &dma_buf[dma_buf_idx * ent_size],
                                             memb_flds->ecmp_nhop_id[ent_offset],
                                             &(val)));
                } else {
                    SHR_IF_ERR_EXIT
                        (bcmdrd_pt_field_set(unit,
                                             BCMECMP_TBL_DRD_SID
                                                 (unit, member, gtype),
                                             &dma_buf[dma_buf_idx * ent_size],
                                             memb_flds->u_ecmp_nhop[ent_offset],
                                             &(val)));
                    SHR_IF_ERR_EXIT
                        (bcmdrd_pt_field_set(unit,
                                             BCMECMP_TBL_DRD_SID
                                                 (unit, member, gtype),
                                             &dma_buf[dma_buf_idx * ent_size],
                                             memb_flds->u_ecmp_id[ent_offset],
                                             &(val)));
                    SHR_IF_ERR_EXIT
                        (bcmdrd_pt_field_set(unit,
                                             BCMECMP_TBL_DRD_SID
                                                 (unit, member, gtype),
                                             &dma_buf[dma_buf_idx * ent_size],
                                             memb_flds->u_nhop_id[ent_offset],
                                             &(val)));
                    SHR_IF_ERR_EXIT
                        (bcmdrd_pt_field_set(unit,
                                             BCMECMP_TBL_DRD_SID
                                                 (unit, member, gtype),
                                             &dma_buf[dma_buf_idx * ent_size],
                                             memb_flds->o_nhop_id[ent_offset],
                                             &(val)));
                }

                ent_offset++;
                max_paths--;
            }

            ent_idx++;
            ent_offset = 0;
            dma_buf_idx++;
        } else {
            ent_offset = max_paths / nh_per_ent;

            max_paths %= nh_per_ent;
            ent_idx += ent_offset;
            dma_buf_idx += ent_offset;
            ent_offset = 0;
        }
    }

    SHR_IF_ERR_EXIT
        (bcm56996_a0_ecmp_dfrag_memb_write(unit, mclr, start_idx, ent_num,
                                           true, dma_buf));

    /* Update WALI member table entries write count. */
    wali->mclr_ecount += ent_num;

exit:
    BCMECMP_FREE(dma_buf);
    SHR_FUNC_EXIT();
}

static int
bcm56996_a0_ecmp_memb_clr(int unit, const void *mfrag)
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
                (bcm56996_a0_ecmp_mw_memb_dma_clr(unit, &my_frag));
        } else {
            if (bcmecmp_grp_is_overlay(my_frag.gtype)) {
                SHR_IF_ERR_EXIT
                    (bcm56996_a0_ecmp_memb_dma_clr
                         (unit, &my_frag,
                          NHOPS_PER_MEMB_ENTRY_OVERLAY, true));
            } else {
                SHR_IF_ERR_EXIT
                    (bcm56996_a0_ecmp_memb_dma_clr(unit, &my_frag,
                                                   NHOPS_PER_MEMB_ENTRY, false));
            }
        }
    } else {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

    exit:
        SHR_FUNC_EXIT();
}

static int
bcm56996_a0_ecmp_gmstart_updt(int unit, const void *ginfo)
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
    if (gtype != BCMECMP_GRP_TYPE_OVERLAY &&
        gtype != BCMECMP_GRP_TYPE_UNDERLAY &&
        gtype != BCMECMP_GRP_TYPE_OVERLAY_WEIGHTED &&
        gtype != BCMECMP_GRP_TYPE_UNDERLAY_WEIGHTED) {
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
                  "bcmecmp56996GrpMstartUpdt");

    SHR_IF_ERR_EXIT
        (bcm56996_a0_ecmp_dfrag_grp_read(unit, gupdt, pt_entry_buff));

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
        (bcm56996_a0_ecmp_dfrag_grp_write(unit, gupdt, pt_entry_buff));

    wali->gw_ecount += 1;

exit:
    BCMECMP_FREE(pt_entry_buff);
    SHR_FUNC_EXIT();
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
bcm56996_a0_ecmp_share_group_type_get(bcmecmp_grp_type_t gtype)
{
    if (gtype == BCMECMP_GRP_TYPE_OVERLAY) {
        return BCMECMP_GRP_TYPE_OVERLAY_WEIGHTED;
    } else if (gtype == BCMECMP_GRP_TYPE_OVERLAY_WEIGHTED) {
        return BCMECMP_GRP_TYPE_OVERLAY;
    } else if (gtype == BCMECMP_GRP_TYPE_UNDERLAY) {
        return BCMECMP_GRP_TYPE_UNDERLAY_WEIGHTED;
    } else if (gtype == BCMECMP_GRP_TYPE_UNDERLAY_WEIGHTED) {
        return BCMECMP_GRP_TYPE_UNDERLAY;
    }

    return BCMECMP_GRP_TYPE_COUNT;
}

static int
becm56996_weighted_underlay_group_lt_fields_fill(int unit,
                                      const void *in_fld,
                                      void *ltentry)
{
    uint32_t fid;  /* Field identifier value. */
    uint64_t fval; /* Field value. */
    uint32_t idx;
    uint32_t num;
    uint64_t def_val;
    const bcmltd_field_t *in = in_fld;
    bcmecmp_lt_entry_t *lt_entry = ltentry;

    SHR_FUNC_ENTER(unit);

    /* Verifiy for null pointer input params and return error. */
    SHR_NULL_CHECK(in, SHR_E_PARAM);
    SHR_NULL_CHECK(lt_entry, SHR_E_PARAM);

    fid = in->id;
    fval = in->data;
    idx = in->idx;
    if (in->flags & SHR_FMM_FIELD_DEL) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlrd_field_default_get(unit, lt_entry->glt_sid, fid,
                                      1, &def_val, &num));
        fval = def_val;
    }

    switch (fid) {
        case ECMP_WEIGHTED_UNDERLAYt_ECMP_IDf:
            BCMECMP_LT_FIELD_SET(lt_entry->fbmp, BCMECMP_LT_FIELD_ECMP_ID);
            lt_entry->ecmp_id = (bcmecmp_id_t) fval;
            break;

        case ECMP_WEIGHTED_UNDERLAYt_WEIGHTED_SIZEf:
            BCMECMP_LT_FIELD_SET(lt_entry->fbmp,
                                 BCMECMP_LT_FIELD_WEIGHTED_SIZE);
            lt_entry->weighted_size = (uint32_t) fval;
            lt_entry->lb_mode = (uint32_t) fval;
            BCMECMP_LT_FIELD_SET(lt_entry->fbmp, BCMECMP_LT_FIELD_MAX_PATHS);
            lt_entry->max_paths = bcm56996_a0_weight_size_to_max_paths(fval);
            break;

        case ECMP_WEIGHTED_UNDERLAYt_WEIGHTED_MODEf:
            BCMECMP_LT_FIELD_SET(lt_entry->fbmp, BCMECMP_LT_FIELD_WEIGHTED_MODE);
            lt_entry->weighted_mode = (uint32_t) fval;
            break;

        case ECMP_WEIGHTED_UNDERLAYt_WEIGHTf:
            if (idx >= COUNTOF(lt_entry->weight)) {
                SHR_ERR_EXIT(SHR_E_PARAM);
            }

            BCMECMP_LT_FIELD_SET(lt_entry->fbmp, BCMECMP_LT_FIELD_WEIGHT);
            lt_entry->weight[idx] = (uint32_t) fval;
            lt_entry->weight_count += 1;
            break;

        case ECMP_WEIGHTED_UNDERLAYt_NUM_PATHSf:
            BCMECMP_LT_FIELD_SET (lt_entry->fbmp, BCMECMP_LT_FIELD_NUM_PATHS);
            lt_entry->num_paths = (uint32_t) fval;
            break;

        case ECMP_WEIGHTED_UNDERLAYt_NHOP_IDf:
            if (idx >= COUNTOF(lt_entry->nhop_id)) {
                SHR_ERR_EXIT(SHR_E_PARAM);
            }

            BCMECMP_LT_FIELD_SET(lt_entry->fbmp, BCMECMP_LT_FIELD_NHOP_ID);
            lt_entry->nhop_id[idx] = (int) fval;

            /* Increment parsed NHOP_IDs count. */
            lt_entry->nhop_ids_count += 1;
            break;

        case ECMP_WEIGHTED_UNDERLAYt_ECMP_CTR_ING_EFLEX_ACTION_IDf:
            BCMECMP_LT_FIELD_SET
                (lt_entry->fbmp, BCMECMP_LT_FIELD_CTR_ING_EFLEX_ACTION_ID);
            lt_entry->ctr_ing_eflex_action_id = (uint32_t) fval;
            break;

        case ECMP_WEIGHTED_UNDERLAYt_CTR_ING_EFLEX_OBJECTf:
            BCMECMP_LT_FIELD_SET
                (lt_entry->fbmp, BCMECMP_LT_FIELD_CTR_ING_EFLEX_OBJECT);
            lt_entry->ctr_ing_eflex_object = (uint32_t) fval;
            break;

        default:
            SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

exit:
    SHR_FUNC_EXIT();
}

static int
    bcm56996_a0_underlay_grp_lt_fields_fill(int unit,
                                        const void *in_fld,
                                        void *ltentry)
{
    uint32_t fid;  /* Field identifier value. */
    uint64_t fval; /* Field value. */
    uint32_t idx;
    uint32_t num;
    uint64_t def_val;
    const bcmltd_field_t *in = in_fld;
    bcmecmp_lt_entry_t *lt_entry = ltentry;

    SHR_FUNC_ENTER(unit);

    /* Verifiy for null pointer input params and return error. */
    SHR_NULL_CHECK(in, SHR_E_PARAM);
    SHR_NULL_CHECK(lt_entry, SHR_E_PARAM);

    if (lt_entry->grp_type == BCMECMP_GRP_TYPE_UNDERLAY_WEIGHTED) {
        SHR_IF_ERR_EXIT(
            becm56996_weighted_underlay_group_lt_fields_fill
                (unit, in_fld, ltentry));
        SHR_EXIT();
    }

    fid = in->id;
    fval = in->data;
    idx = in->idx;
    if (in->flags & SHR_FMM_FIELD_DEL) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlrd_field_default_get(unit, lt_entry->glt_sid, fid,
                                     1, &def_val, &num));
        fval = def_val;
    }

    switch (fid) {
        case ECMP_UNDERLAYt_ECMP_IDf:
            BCMECMP_LT_FIELD_SET(lt_entry->fbmp, BCMECMP_LT_FIELD_ECMP_ID);
            lt_entry->ecmp_id = (bcmecmp_id_t)fval;
            break;

        case ECMP_UNDERLAYt_LB_MODEf:
            BCMECMP_LT_FIELD_SET(lt_entry->fbmp, BCMECMP_LT_FIELD_LB_MODE);
            lt_entry->lb_mode = (uint32_t) fval;
            break;

        case ECMP_UNDERLAYt_NHOP_SORTEDf:
            BCMECMP_LT_FIELD_SET
                (lt_entry->fbmp, BCMECMP_LT_FIELD_NHOP_SORTED);
            lt_entry->nhop_sorted = fval;
            break;

        case ECMP_UNDERLAYt_MAX_PATHSf:
            BCMECMP_LT_FIELD_SET
                (lt_entry->fbmp, BCMECMP_LT_FIELD_MAX_PATHS);
            lt_entry->max_paths = (uint32_t) fval;
            break;

        case ECMP_UNDERLAYt_NUM_PATHSf:
            BCMECMP_LT_FIELD_SET
                (lt_entry->fbmp, BCMECMP_LT_FIELD_NUM_PATHS);
            lt_entry->num_paths = (uint32_t) fval;
            break;

        case ECMP_UNDERLAYt_NHOP_IDf:
            /*
             * Ignore default value for NHOP_ID when mismatch with LB. This
             * is for updating LB between RH and Regular
             */
            if (lt_entry->event == BCMIMM_ENTRY_UPDATE &&
                BCMECMP_GRP_LB_MODE(unit, lt_entry->ecmp_id) ==
                                                BCMECMP_LB_MODE_RESILIENT) {
                def_val = 0;
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmlrd_field_default_get(unit, lt_entry->glt_sid, fid,
                                              1, &def_val, &num));
                if (fval == def_val) {
                    break;
                }
            }

            if (idx >= COUNTOF(lt_entry->nhop_id)) {
                SHR_ERR_EXIT(SHR_E_PARAM);
            }

            BCMECMP_LT_FIELD_SET
                (lt_entry->fbmp, BCMECMP_LT_FIELD_NHOP_ID);
            lt_entry->nhop_id[idx] = (int) fval;

            /* Increment parsed NHOP_IDs count. */
            lt_entry->nhop_ids_count += 1;
            break;

        case ECMP_UNDERLAYt_RH_NHOP_IDf:
            /*
             * Ignore default value for RH_NHOP_ID when mismatch with LB. This
             * is for updating LB between RH and Regular
             */
            if (lt_entry->event == BCMIMM_ENTRY_UPDATE &&
                BCMECMP_GRP_LB_MODE(unit, lt_entry->ecmp_id) !=
                                                BCMECMP_LB_MODE_RESILIENT) {
                def_val = 0;
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmlrd_field_default_get(unit, lt_entry->glt_sid, fid,
                                              1, &def_val, &num));
                if (fval == def_val) {
                    break;
                }
            }

            if (idx >= COUNTOF(lt_entry->nhop_id)) {
                SHR_ERR_EXIT(SHR_E_PARAM);
            }

            BCMECMP_LT_FIELD_SET(lt_entry->fbmp, BCMECMP_LT_FIELD_RH_NHOP_ID);
            lt_entry->nhop_id[idx] = (int) fval;
            /* Increment parsed NHOP_IDs count. */
            lt_entry->nhop_ids_count += 1;
            break;

        case ECMP_UNDERLAYt_RH_SIZEf:
            BCMECMP_LT_FIELD_SET
                (lt_entry->fbmp, BCMECMP_LT_FIELD_RH_SIZE);
            lt_entry->rh_size_enc = (uint32_t) fval;
            lt_entry->rh_entries_cnt = (uint32_t)(1 <<
                                                    lt_entry->rh_size_enc);
                break;

        case ECMP_UNDERLAYt_RH_NUM_PATHSf:
            BCMECMP_LT_FIELD_SET
                (lt_entry->fbmp, BCMECMP_LT_FIELD_RH_NUM_PATHS);
            lt_entry->rh_num_paths = (uint32_t) fval;
        break;

        case ECMP_UNDERLAYt_RH_RANDOM_SEEDf:
            BCMECMP_LT_FIELD_SET
                (lt_entry->fbmp, BCMECMP_LT_FIELD_RH_RANDOM_SEED);
            lt_entry->rh_rand_seed = (uint32_t) fval;
            break;

        case ECMP_UNDERLAYt_ECMP_CTR_ING_EFLEX_ACTION_IDf:
            BCMECMP_LT_FIELD_SET
                (lt_entry->fbmp, BCMECMP_LT_FIELD_CTR_ING_EFLEX_ACTION_ID);
            lt_entry->ctr_ing_eflex_action_id = (uint32_t) fval;
            break;

        case ECMP_UNDERLAYt_CTR_ING_EFLEX_OBJECTf:
            BCMECMP_LT_FIELD_SET
                (lt_entry->fbmp, BCMECMP_LT_FIELD_CTR_ING_EFLEX_OBJECT);
            lt_entry->ctr_ing_eflex_object = (uint32_t) fval;
            break;

        default:
            SHR_ERR_EXIT(SHR_E_INTERNAL);
    }
    exit:
        SHR_FUNC_EXIT();
}

static int
becm56996_weighted_overlay_group_lt_fields_fill(int unit,
                                                const void *in_fld,
                                                void *ltentry)
{
    uint32_t fid;  /* Field identifier value. */
    uint64_t fval; /* Field value. */
    uint32_t idx;
    uint32_t num;
    uint64_t def_val;
    const bcmltd_field_t *in = in_fld;
    bcmecmp_lt_entry_t *lt_entry = ltentry;

    SHR_FUNC_ENTER(unit);

    /* Verifiy for null pointer input params and return error. */
    SHR_NULL_CHECK(in, SHR_E_PARAM);
    SHR_NULL_CHECK(lt_entry, SHR_E_PARAM);

    fid = in->id;
    fval = in->data;
    idx = in->idx;
    if (in->flags & SHR_FMM_FIELD_DEL) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlrd_field_default_get(unit, lt_entry->glt_sid, fid,
                                      1, &def_val, &num));
        fval = def_val;
    }

    switch (fid) {
        case ECMP_WEIGHTED_OVERLAYt_ECMP_IDf:
            BCMECMP_LT_FIELD_SET(lt_entry->fbmp, BCMECMP_LT_FIELD_ECMP_ID);
            lt_entry->ecmp_id = (bcmecmp_id_t) fval;
            break;

        case ECMP_WEIGHTED_OVERLAYt_WEIGHTf:
            if (idx >= COUNTOF(lt_entry->weight)) {
                SHR_ERR_EXIT(SHR_E_PARAM);
            }

            BCMECMP_LT_FIELD_SET(lt_entry->fbmp, BCMECMP_LT_FIELD_WEIGHT);
            lt_entry->weight[idx] = (uint32_t) fval;
            lt_entry->weight_count += 1;
            break;

        case ECMP_WEIGHTED_OVERLAYt_NUM_PATHSf:
            BCMECMP_LT_FIELD_SET (lt_entry->fbmp, BCMECMP_LT_FIELD_NUM_PATHS);
            lt_entry->num_paths = (uint32_t) fval;
            break;

        case ECMP_WEIGHTED_OVERLAYt_UNDERLAY_NHOP_IDf:
            if (idx >= COUNTOF(lt_entry->nhop_id)) {
                SHR_ERR_EXIT(SHR_E_PARAM);
            }

            BCMECMP_LT_FIELD_SET(lt_entry->fbmp, BCMECMP_LT_FIELD_NHOP_ID);
            lt_entry->nhop_id[idx] = (int) fval;

            /* Increment parsed NHOP_IDs count. */
            lt_entry->nhop_ids_count += 1;
            break;

        case ECMP_WEIGHTED_OVERLAYt_OVERLAY_NHOP_IDf:
            if (idx >= COUNTOF(lt_entry->o_nhop_id)) {
                SHR_ERR_EXIT(SHR_E_PARAM);
            }

            BCMECMP_LT_FIELD_SET(lt_entry->fbmp, BCMECMP_LT_FIELD_O_NHOP_ID);
            lt_entry->o_nhop_id[idx] = (int) fval;

            /* Increment parsed NHOP_IDs count. */
            lt_entry->o_nhop_ids_count += 1;
            break;

        case ECMP_WEIGHTED_OVERLAYt_ECMP_NHOP_UNDERLAYf:
            if (idx >= COUNTOF(lt_entry->u_ecmp_nhop)) {
                SHR_ERR_EXIT(SHR_E_PARAM);
            }

            BCMECMP_LT_FIELD_SET
                (lt_entry->fbmp, BCMECMP_LT_FIELD_ECMP_NHOP);
            lt_entry->u_ecmp_nhop[idx] = fval;

            /* Increment parsed NHOP_IDs count. */
            lt_entry->u_ecmp_nhop_count += 1;
            break;

        case ECMP_WEIGHTED_OVERLAYt_ECMP_UNDERLAY_IDf:
            if (idx >= COUNTOF(lt_entry->uecmp_id)) {
                SHR_ERR_EXIT(SHR_E_PARAM);
            }

            BCMECMP_LT_FIELD_SET
                (lt_entry->fbmp, BCMECMP_LT_FIELD_UECMP_ID);
            lt_entry->uecmp_id[idx] = (int) fval;

            /* Increment parsed ECMP_UNDERLAY_IDs count. */
            lt_entry->uecmp_ids_count += 1;
            break;

        case ECMP_WEIGHTED_OVERLAYt_ECMP_OVERLAY_CTR_ING_EFLEX_ACTION_IDf:
            BCMECMP_LT_FIELD_SET
                (lt_entry->fbmp, BCMECMP_LT_FIELD_CTR_ING_EFLEX_ACTION_ID);
            lt_entry->ctr_ing_eflex_action_id = (uint32_t) fval;
            break;

        case ECMP_WEIGHTED_OVERLAYt_CTR_ING_EFLEX_OBJECTf:
            BCMECMP_LT_FIELD_SET
                (lt_entry->fbmp, BCMECMP_LT_FIELD_CTR_ING_EFLEX_OBJECT);
            lt_entry->ctr_ing_eflex_object = (uint32_t) fval;
            break;

        default:
            SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief ECMP_OVERLAY LT entry fields fill routine.
 *
 * Parse ECMP_OVERLAY logical table entry logical fields
 * and store the data in lt_entry structure.
 *
 * \param [in] unit Unit number.
 * \param [in] in_fld Logical table database input field array.
 * \param [out] ltentry Logical table entry data buffer.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Invalid input parameter.
 * \retval !SHR_E_NONE Failed to convert \c in to \c ltentry.
 */
static int
bcm56996_a0_overlay_grp_lt_fields_fill(int unit,
                                       const void *in_fld,
                                       void *ltentry)
{
    uint32_t fid;  /* Field identifier value. */
    uint64_t fval; /* Field value. */
    uint32_t idx;
    uint32_t num;
    uint64_t def_val;
    const bcmltd_field_t *in = in_fld;
    bcmecmp_lt_entry_t *lt_entry = ltentry;

    SHR_FUNC_ENTER(unit);

    /* Verifiy for null pointer input params and return error. */
    SHR_NULL_CHECK(in, SHR_E_PARAM);
    SHR_NULL_CHECK(lt_entry, SHR_E_PARAM);

    if (lt_entry->grp_type == BCMECMP_GRP_TYPE_OVERLAY_WEIGHTED) {
        SHR_IF_ERR_EXIT
            (becm56996_weighted_overlay_group_lt_fields_fill
                (unit, in_fld, ltentry));
        SHR_EXIT();
    }

    fid = in->id;
    fval = in->data;
    idx = in->idx;
    if (in->flags & SHR_FMM_FIELD_DEL) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlrd_field_default_get(unit, lt_entry->glt_sid, fid,
                                     1, &def_val, &num));
        fval = def_val;
    }

    switch (fid) {
        case ECMP_OVERLAYt_ECMP_IDf:
            BCMECMP_LT_FIELD_SET(lt_entry->fbmp, BCMECMP_LT_FIELD_ECMP_ID);
            lt_entry->ecmp_id = (bcmecmp_id_t) fval;
            break;

        case ECMP_OVERLAYt_LB_MODEf:
            BCMECMP_LT_FIELD_SET(lt_entry->fbmp, BCMECMP_LT_FIELD_LB_MODE);
            lt_entry->lb_mode = (uint32_t) fval;
            break;

        case ECMP_OVERLAYt_MAX_PATHSf:
            BCMECMP_LT_FIELD_SET
                (lt_entry->fbmp, BCMECMP_LT_FIELD_MAX_PATHS);
            lt_entry->max_paths = (uint32_t) fval;
            break;

        case ECMP_OVERLAYt_NUM_PATHSf:
            BCMECMP_LT_FIELD_SET
                (lt_entry->fbmp, BCMECMP_LT_FIELD_NUM_PATHS);
            lt_entry->num_paths = (uint32_t) fval;
            break;

        case ECMP_OVERLAYt_ECMP_NHOP_UNDERLAYf:
            /*
             * Ignore default value for ECMP_NHOP_UNDERLAY when mismatch with LB.
             *  This is for updating LB between RH and Regular
             */
            if (lt_entry->event == BCMIMM_ENTRY_UPDATE &&
                BCMECMP_GRP_LB_MODE(unit, lt_entry->ecmp_id) ==
                                                BCMECMP_LB_MODE_RESILIENT) {
                def_val = 0;
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmlrd_field_default_get(unit, lt_entry->glt_sid, fid,
                                              1, &def_val, &num));
                if (fval == def_val) {
                    break;
                }
            }

            if (idx >= COUNTOF(lt_entry->u_ecmp_nhop)) {
                SHR_ERR_EXIT(SHR_E_PARAM);
            }

            BCMECMP_LT_FIELD_SET
                (lt_entry->fbmp, BCMECMP_LT_FIELD_ECMP_NHOP);
            lt_entry->u_ecmp_nhop[idx] = fval;

            /* Increment parsed NHOP_IDs count. */
            lt_entry->u_ecmp_nhop_count += 1;
            break;

        case ECMP_OVERLAYt_RH_ECMP_NHOP_UNDERLAYf:
            /*
             * Ignore default value for ECMP_NHOP_UNDERLAY when mismatch with LB.
             *  This is for updating LB between RH and Regular
             */
            if (lt_entry->event == BCMIMM_ENTRY_UPDATE &&
                BCMECMP_GRP_LB_MODE(unit, lt_entry->ecmp_id) !=
                                                BCMECMP_LB_MODE_RESILIENT) {
                def_val = 0;
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmlrd_field_default_get(unit, lt_entry->glt_sid, fid,
                                              1, &def_val, &num));
                if (fval == def_val) {
                    break;
                }
            }

            if (idx >= COUNTOF(lt_entry->u_ecmp_nhop)) {
                SHR_ERR_EXIT(SHR_E_PARAM);
            }

            BCMECMP_LT_FIELD_SET
                (lt_entry->fbmp, BCMECMP_LT_FIELD_RH_ECMP_NHOP);
            lt_entry->u_ecmp_nhop[idx] = fval;

            /* Increment parsed NHOP_IDs count. */
            lt_entry->u_ecmp_nhop_count += 1;
            break;

        case ECMP_OVERLAYt_ECMP_UNDERLAY_IDf:
            /*
             * Ignore default value for ECMP_UNDERLAY_ID when mismatch with LB.
             * This is for updating LB between RH and Regular
             */
            if (lt_entry->event == BCMIMM_ENTRY_UPDATE &&
                BCMECMP_GRP_LB_MODE(unit, lt_entry->ecmp_id) ==
                                                BCMECMP_LB_MODE_RESILIENT) {
                def_val = 0;
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmlrd_field_default_get(unit, lt_entry->glt_sid, fid,
                                              1, &def_val, &num));
                if (fval == def_val) {
                    break;
                }
            }

            if (idx >= COUNTOF(lt_entry->uecmp_id)) {
                SHR_ERR_EXIT(SHR_E_PARAM);
            }

            BCMECMP_LT_FIELD_SET
                (lt_entry->fbmp, BCMECMP_LT_FIELD_UECMP_ID);
            lt_entry->uecmp_id[idx] = (int) fval;

            /* Increment parsed ECMP_UNDERLAY_IDs count. */
            lt_entry->uecmp_ids_count += 1;
            break;

        case ECMP_OVERLAYt_RH_ECMP_UNDERLAY_IDf:
            /*
             * Ignore default value for RH_ECMP_UNDERLAY_ID when mismatch with
             *  LB. This is for updating LB between RH and Regular
             */
            if (lt_entry->event == BCMIMM_ENTRY_UPDATE &&
                BCMECMP_GRP_LB_MODE(unit, lt_entry->ecmp_id) !=
                                                BCMECMP_LB_MODE_RESILIENT) {
                def_val = 0;
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmlrd_field_default_get(unit, lt_entry->glt_sid, fid,
                                              1, &def_val, &num));
                if (fval == def_val) {
                    break;
                }
            }

            if (idx >= COUNTOF(lt_entry->uecmp_id)) {
                SHR_ERR_EXIT(SHR_E_PARAM);
            }

            BCMECMP_LT_FIELD_SET
                (lt_entry->fbmp, BCMECMP_LT_FIELD_RH_UECMP_ID);
            lt_entry->uecmp_id[idx] = (int) fval;

            /* Increment parsed ECMP_UNDERLAY_IDs count. */
            lt_entry->uecmp_ids_count += 1;
            break;

        case ECMP_OVERLAYt_UNDERLAY_NHOP_IDf:
            /*
             * Ignore default value for NHOP_ID when mismatch with LB. This
             * is for updating LB between RH and Regular
             */
            if (lt_entry->event == BCMIMM_ENTRY_UPDATE &&
                BCMECMP_GRP_LB_MODE(unit, lt_entry->ecmp_id) ==
                                                BCMECMP_LB_MODE_RESILIENT) {
                def_val = 0;
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmlrd_field_default_get(unit, lt_entry->glt_sid, fid,
                                              1, &def_val, &num));
                if (fval == def_val) {
                    break;
                }
            }

            if (idx >= COUNTOF(lt_entry->nhop_id)) {
                SHR_ERR_EXIT(SHR_E_PARAM);
            }

            BCMECMP_LT_FIELD_SET
                (lt_entry->fbmp, BCMECMP_LT_FIELD_NHOP_ID);
            lt_entry->nhop_id[idx] = (int) fval;

            /* Increment parsed NHOP_IDs count. */
            lt_entry->nhop_ids_count += 1;
            break;

        case ECMP_OVERLAYt_RH_UNDERLAY_NHOP_IDf:
            /*
             * Ignore default value for RH_NHOP_ID when mismatch with LB. This
             * is for updating LB between RH and Regular
             */
            if (lt_entry->event == BCMIMM_ENTRY_UPDATE &&
                BCMECMP_GRP_LB_MODE(unit, lt_entry->ecmp_id) !=
                                                BCMECMP_LB_MODE_RESILIENT) {
                def_val = 0;
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmlrd_field_default_get(unit, lt_entry->glt_sid, fid,
                                              1, &def_val, &num));
                if (fval == def_val) {
                    break;
                }
            }

            if (idx >= COUNTOF(lt_entry->nhop_id)) {
                SHR_ERR_EXIT(SHR_E_PARAM);
            }

            BCMECMP_LT_FIELD_SET
                (lt_entry->fbmp, BCMECMP_LT_FIELD_RH_NHOP_ID);
            lt_entry->nhop_id[idx] = (int) fval;

            /* Increment parsed NHOP_IDs count. */
            lt_entry->nhop_ids_count += 1;
            break;

        case ECMP_OVERLAYt_OVERLAY_NHOP_IDf:
            /*
             * Ignore default value for NHOP_ID when mismatch with LB. This
             * is for updating LB between RH and Regular
             */
            if (lt_entry->event == BCMIMM_ENTRY_UPDATE &&
                BCMECMP_GRP_LB_MODE(unit, lt_entry->ecmp_id) ==
                                                BCMECMP_LB_MODE_RESILIENT) {
                def_val = 0;
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmlrd_field_default_get(unit, lt_entry->glt_sid, fid,
                                              1, &def_val, &num));
                if (fval == def_val) {
                    break;
                }
            }

            if (idx >= COUNTOF(lt_entry->o_nhop_id)) {
                SHR_ERR_EXIT(SHR_E_PARAM);
            }

            BCMECMP_LT_FIELD_SET
                (lt_entry->fbmp, BCMECMP_LT_FIELD_O_NHOP_ID);
            lt_entry->o_nhop_id[idx] = (int) fval;

            /* Increment parsed NHOP_IDs count. */
            lt_entry->o_nhop_ids_count += 1;
            break;

        case ECMP_OVERLAYt_RH_OVERLAY_NHOP_IDf:
            /*
             * Ignore default value for RH_NHOP_ID when mismatch with LB. This
             * is for updating LB between RH and Regular
             */
            if (lt_entry->event == BCMIMM_ENTRY_UPDATE &&
                BCMECMP_GRP_LB_MODE(unit, lt_entry->ecmp_id) !=
                                                BCMECMP_LB_MODE_RESILIENT) {
                def_val = 0;
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmlrd_field_default_get(unit, lt_entry->glt_sid, fid,
                                              1, &def_val, &num));
                if (fval == def_val) {
                    break;
                }
            }

            if (idx >= COUNTOF(lt_entry->o_nhop_id)) {
                SHR_ERR_EXIT(SHR_E_PARAM);
            }

            BCMECMP_LT_FIELD_SET
                (lt_entry->fbmp, BCMECMP_LT_FIELD_RH_O_NHOP_ID);
            lt_entry->o_nhop_id[idx] = (int) fval;

            /* Increment parsed NHOP_IDs count. */
            lt_entry->o_nhop_ids_count += 1;
            break;

        case ECMP_OVERLAYt_RH_SIZEf:
            BCMECMP_LT_FIELD_SET
                (lt_entry->fbmp, BCMECMP_LT_FIELD_RH_SIZE);
            lt_entry->rh_size_enc = (uint32_t)fval;
            lt_entry->rh_entries_cnt = (uint32_t)(1 <<
                                                    lt_entry->rh_size_enc);
                break;

        case ECMP_OVERLAYt_RH_NUM_PATHSf:
            BCMECMP_LT_FIELD_SET
                (lt_entry->fbmp, BCMECMP_LT_FIELD_RH_NUM_PATHS);
            lt_entry->rh_num_paths = (uint32_t) fval;
            break;

        case ECMP_OVERLAYt_RH_RANDOM_SEEDf:
            BCMECMP_LT_FIELD_SET
                (lt_entry->fbmp, BCMECMP_LT_FIELD_RH_RANDOM_SEED);
            lt_entry->rh_rand_seed = (uint32_t) fval;
            break;

        case ECMP_OVERLAYt_ECMP_OVERLAY_CTR_ING_EFLEX_ACTION_IDf:
            BCMECMP_LT_FIELD_SET
                (lt_entry->fbmp, BCMECMP_LT_FIELD_CTR_ING_EFLEX_ACTION_ID);
            lt_entry->ctr_ing_eflex_action_id = (uint32_t) fval;
            break;

        case ECMP_OVERLAYt_CTR_ING_EFLEX_OBJECTf:
            BCMECMP_LT_FIELD_SET
                (lt_entry->fbmp, BCMECMP_LT_FIELD_CTR_ING_EFLEX_OBJECT);
            lt_entry->ctr_ing_eflex_object = (uint32_t) fval;
            break;

        default:
            SHR_ERR_EXIT(SHR_E_INTERNAL);
    }
    exit:
        SHR_FUNC_EXIT();
}

static int
bcm56996_a0_ecmp_memb_bank_encode(int unit,
                             bcmecmp_limit_t *l3_bank)
{
    int off_set = 0;

    if (l3_bank->num_o_memb_bank_valid) {
        off_set = BANK_NUM_PER_CHIP - l3_bank->num_o_memb_bank;
        SHR_BITSET_RANGE(l3_bank->memb_bank_bitmap,
                         off_set,
                         l3_bank->num_o_memb_bank);
    }

    return SHR_E_NONE;
}

static int
bcm56996_a0_ecmp_memb_bank_set(int unit,
                               const bcmltd_op_arg_t *op_arg,
                               bcmltd_sid_t lt_id,
                               void *l3_limit)
{
    L3_ECMP_BANK_CONFIGr_t memb_bank_buf;
    bcmecmp_pt_op_info_t op_info;
    bcmecmp_limit_t *l3_bank = (bcmecmp_limit_t*)l3_limit;

    SHR_FUNC_ENTER(unit);

    bcm56996_a0_ecmp_memb_bank_encode(unit, l3_bank);

    if (l3_bank->num_o_memb_bank_valid) {
        L3_ECMP_BANK_CONFIGr_CLR(memb_bank_buf);
        /* Only low 8 bits are used. */
        L3_ECMP_BANK_CONFIGr_BITMAPf_SET
            (memb_bank_buf, l3_bank->memb_bank_bitmap[0]);

        bcm56996_a0_ecmp_pt_op_info_t_init(&op_info);
        op_info.op = BCMPTM_OP_WRITE;
        op_info.op_arg = op_arg;
        op_info.req_lt_sid = lt_id;
        op_info.drd_sid = L3_ECMP_BANK_CONFIGr;
        op_info.wsize = bcmdrd_pt_entry_wsize(unit, L3_ECMP_BANK_CONFIGr);
        op_info.dyn_info.index = 0;
        SHR_IF_ERR_EXIT
            (bcmecmp_pt_write(unit, &op_info, (uint32_t *)&memb_bank_buf));
    }

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56996_a0_ecmp_memb_bank_get(int unit,
                               const bcmltd_op_arg_t *op_arg,
                               bcmltd_sid_t lt_id,
                               void *l3_limit)
{
    L3_ECMP_BANK_CONFIGr_t memb_bank_buf;
    bcmecmp_pt_op_info_t op_info;
    bcmecmp_limit_t *l3_bank = (bcmecmp_limit_t *)l3_limit;

    SHR_FUNC_ENTER(unit);

    L3_ECMP_BANK_CONFIGr_CLR(memb_bank_buf);

    bcm56996_a0_ecmp_pt_op_info_t_init(&op_info);
    op_info.op = BCMPTM_OP_READ;
    op_info.op_arg = op_arg;
    op_info.req_lt_sid = lt_id;
    op_info.drd_sid = L3_ECMP_BANK_CONFIGr;
    op_info.wsize = bcmdrd_pt_entry_wsize(unit, L3_ECMP_BANK_CONFIGr);
    op_info.dyn_info.index = 0;

    SHR_IF_ERR_EXIT
        (bcmecmp_pt_read(unit, &op_info, (uint32_t *)&memb_bank_buf));
    l3_bank->memb_bank_bitmap[0] =
                                L3_ECMP_BANK_CONFIGr_BITMAPf_GET(memb_bank_buf);
    l3_bank->num_o_memb_bank_valid = 1;

    bcm56996_a0_ecmp_memb_bank_decode(unit, l3_bank);

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56996_a0_grp_member_fields_count_get(int unit, void *lt_entry)
{
    bcmecmp_lt_entry_t *lt_ent = (bcmecmp_lt_entry_t *)lt_entry;

    if (bcmecmp_grp_is_overlay(lt_ent->grp_type)) {
        return OVERLAY_MEMB_COUNT;
    } else {
        /* default value. */
        return 1;
    }
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
bcm56996_a0_ecmp_attach(int unit)
{
    bcmecmp_drv_t *ecmp_drv = bcmecmp_drv_get(unit);

    SHR_FUNC_ENTER(unit);

    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit,
                         "bcm56996_a0_ecmp_attach.\n")));

    SHR_NULL_CHECK(ecmp_drv, SHR_E_FAIL);

    ecmp_ctrl_local[unit] = bcmecmp_get_ecmp_control(unit);

    BCMECMP_USE_INDEX_REMAP(unit) = FALSE;

    ecmp_ctrl_local[unit]->ecmp_info.itbm = true;

    ecmp_ctrl_local[unit]->ecmp_info.ovlay_memb_count = OVERLAY_MEMB_COUNT;

    /* Initialize device ECMP feature function pointers. */
    BCMECMP_FNCALL_PTR(unit) = ecmp_drv;

    /* Setup device ECMP component initialization function. */
    BCMECMP_FNCALL(unit, info_init) = &bcm56996_a0_ecmp_info_init;

    /* Setup device ECMP component clean-up function. */
    BCMECMP_FNCALL(unit, info_cleanup) = &bcm56996_a0_ecmp_info_cleanup;

    /* Setup device ECMP resolution mode hardware config function. */
    BCMECMP_FNCALL(unit, mode_config) = &bcm56996_a0_ecmp_mode_config;

    /* Initialize device ECMP group management function pointers. */
    BCMECMP_FNCALL(unit, grp_ins) = &bcm56996_a0_ecmp_grp_ins;
    BCMECMP_FNCALL(unit, grp_del) = &bcm56996_a0_ecmp_grp_del;
    BCMECMP_FNCALL(unit, grp_get) = &bcm56996_a0_ecmp_grp_get;
    BCMECMP_FNCALL(unit, rh_grp_ins) = &bcm56996_a0_ecmp_rh_grp_ins;
    BCMECMP_FNCALL(unit, rh_grp_del) = &bcm56996_a0_ecmp_rh_grp_del;
    BCMECMP_FNCALL(unit, rh_grp_get) = &bcm56996_a0_ecmp_rh_grp_get;
    BCMECMP_FNCALL(unit, rh_pre_config) = &bcmecmp_rh_groups_preconfig_with_itbm;
    BCMECMP_FNCALL(unit, memb_cpy) = &bcm56996_a0_ecmp_memb_cpy;
    BCMECMP_FNCALL(unit, memb_clr) = &bcm56996_a0_ecmp_memb_clr;
    BCMECMP_FNCALL(unit, gmstart_updt) = &bcm56996_a0_ecmp_gmstart_updt;
    BCMECMP_FNCALL(unit, member_weight_index_width) = NULL;
    BCMECMP_FNCALL(unit, grp_ol_lt_fields_fill) =
                            &bcm56996_a0_overlay_grp_lt_fields_fill;
    BCMECMP_FNCALL(unit, grp_ul_lt_fields_fill) =
                            &bcm56996_a0_underlay_grp_lt_fields_fill;
    BCMECMP_FNCALL(unit, grp_member_fields_count_get) =
                                       &bcm56996_a0_grp_member_fields_count_get;
    ecmp_drv->shr_grp_type_get = &bcm56996_a0_ecmp_share_group_type_get;
    ecmp_drv->weight_size_to_max_paths = &bcm56996_a0_weight_size_to_max_paths;
    ecmp_drv->itbm_memb_lists_update = &bcm56996_a0_ecmp_itbm_memb_lists_resize;
    ecmp_drv->memb_bank_set= &bcm56996_a0_ecmp_memb_bank_set;
    ecmp_drv->memb_bank_get = &bcm56996_a0_ecmp_memb_bank_get;

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
bcm56996_a0_ecmp_detach(int unit)
{
    bcmecmp_drv_t *ecmp_drv = bcmecmp_drv_get(unit);

    SHR_FUNC_ENTER(unit);

    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit,
                         "bcm56996_a0_ecmp_detach.\n")));

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
        BCMECMP_FNCALL(unit, grp_member_fields_count_get) = NULL;
        ecmp_drv->shr_grp_type_get = NULL;
        ecmp_drv->weight_size_to_max_paths = NULL;
        ecmp_drv->itbm_memb_lists_update = NULL;
        ecmp_drv->memb_bank_set= NULL;
        ecmp_drv->memb_bank_get = NULL;

        /* Clear device ECMP feature function pointers. */
        BCMECMP_FNCALL(unit, mode_config) = NULL;
        BCMECMP_FNCALL(unit, info_cleanup) = NULL;
        BCMECMP_FNCALL(unit, info_init) = NULL;
        BCMECMP_FNCALL_PTR(unit) = NULL;
    }

    ecmp_ctrl_local[unit] = NULL;

    SHR_FUNC_EXIT();
}
