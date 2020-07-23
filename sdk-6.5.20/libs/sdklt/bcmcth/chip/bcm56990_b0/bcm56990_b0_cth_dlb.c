/*! \file bcm56990_b0_cth_dlb.c
 *
 * Chip specific implementation for BCMCTH DLB.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_debug.h>

#include <bcmdrd/bcmdrd_pt.h>
#include <bcmptm/bcmptm.h>
#include <bcmdrd/chip/bcm56990_b0_defs.h>
#include <bcmbd/chip/bcm56990_b0_acc.h>

#include <bcmdrd/bcmdrd_field.h>

#include <bcmltd/chip/bcmltd_id.h>
#include <bcmltd/bcmltd_lt_types.h>

#include <bcmcth/bcmcth_dlb_util.h>
#include <bcmcth/bcmcth_dlb_drv.h>

/* Debug log target definition */
#define BSL_LOG_MODULE BSL_LS_BCMCTH_DLB

/*******************************************************************************
 * Local definitions
 */

#define DLB_NHI_MEMBER_TBL_CNT          5
#define DLB_PORT_MEMBER_TBL_CNT         4
#define DLB_NHI_MEMBER_CNT_PER_TBL      13
#define DLB_PORT_MEMBER_CNT_PER_TBL     16
#define DLB_ECMP_MAX_GROUP              128
#define DLB_ECMP_MAX_COUNT              0x7fffffff

typedef struct dlb_bk_s {
    /*! Total member bitmap for DLB_ECMP_FLOWSET resource. */
    SHR_BITDCL *mbmp;
    bool group_enable[DLB_ECMP_MAX_GROUP];
} dlb_bk_t;

static dlb_bk_t grp_info[BCMDRD_CONFIG_MAX_UNITS];
#define DLB_ECMP_FLOWSET_MEMBER_BLK_BMP(_u_) (grp_info[_u_].mbmp)
#define DLB_ECMP_FLOWSET_MEMBER_BLK_USED_SET_RANGE(_u_, _idx_, _count_) \
    SHR_BITSET_RANGE(DLB_ECMP_FLOWSET_MEMBER_BLK_BMP(_u_), _idx_, _count_)
#define DLB_ECMP_FLOWSET_MEMBER_BLK_USED_CLR_RANGE(_u_, _idx_, _count_) \
    SHR_BITCLR_RANGE(DLB_ECMP_FLOWSET_MEMBER_BLK_BMP(_u_), _idx_, _count_)
#define DLB_ECMP_FLOWSET_MEMBER_BLK_NULL_RANGE(_u_, _idx_, _count_) \
    SHR_BITNULL_RANGE(DLB_ECMP_FLOWSET_MEMBER_BLK_BMP(_u_), _idx_, _count_)

#define DLB_ECMP_GROUP_ENABLE(_u_, _idx_) (grp_info[_u_].group_enable[_idx_])

#define DLB_ECMP_FLOWSET_BLKSZ_SHIFT    8

/*******************************************************************************
 * Private data
 */

static const bcmdrd_sid_t nhi_sid[DLB_NHI_MEMBER_TBL_CNT] = {
     DLB_ECMP_GROUP_NHI_MEMBERS_0_TO_12m,
     DLB_ECMP_GROUP_NHI_MEMBERS_13_TO_25m,
     DLB_ECMP_GROUP_NHI_MEMBERS_26_TO_38m,
     DLB_ECMP_GROUP_NHI_MEMBERS_39_TO_51m,
     DLB_ECMP_GROUP_NHI_MEMBERS_52_TO_63m
};

static const bcmdrd_sid_t port_sid[DLB_PORT_MEMBER_TBL_CNT] = {
     DLB_ECMP_GROUP_PORT_MEMBERS_0_TO_15m,
     DLB_ECMP_GROUP_PORT_MEMBERS_16_TO_31m,
     DLB_ECMP_GROUP_PORT_MEMBERS_32_TO_47m,
     DLB_ECMP_GROUP_PORT_MEMBERS_48_TO_63m,
};


static const bcmdrd_sid_t alt_nhi_sid[DLB_NHI_MEMBER_TBL_CNT] = {
     DLB_ECMP_GROUP_ALT_NHI_MEMBERS_0_TO_12m,
     DLB_ECMP_GROUP_ALT_NHI_MEMBERS_13_TO_25m,
     DLB_ECMP_GROUP_ALT_NHI_MEMBERS_26_TO_38m,
     DLB_ECMP_GROUP_ALT_NHI_MEMBERS_39_TO_51m,
     DLB_ECMP_GROUP_ALT_NHI_MEMBERS_52_TO_63m
};

static const bcmdrd_sid_t alt_port_sid[DLB_PORT_MEMBER_TBL_CNT] = {
     DLB_ECMP_GROUP_ALT_PORT_MEMBERS_0_TO_15m,
     DLB_ECMP_GROUP_ALT_PORT_MEMBERS_16_TO_31m,
     DLB_ECMP_GROUP_ALT_PORT_MEMBERS_32_TO_47m,
     DLB_ECMP_GROUP_ALT_PORT_MEMBERS_48_TO_63m,
};

static const bcmdrd_fid_t nhi_field[BCMCTH_DLB_ECMP_GROUP_MAX_MEMBERS] = {
    MEMBER_0_NHIf, MEMBER_1_NHIf, MEMBER_2_NHIf, MEMBER_3_NHIf,
    MEMBER_4_NHIf, MEMBER_5_NHIf, MEMBER_6_NHIf, MEMBER_7_NHIf,
    MEMBER_8_NHIf, MEMBER_9_NHIf, MEMBER_10_NHIf, MEMBER_11_NHIf,
    MEMBER_12_NHIf, MEMBER_13_NHIf, MEMBER_14_NHIf, MEMBER_15_NHIf,
    MEMBER_16_NHIf, MEMBER_17_NHIf, MEMBER_18_NHIf, MEMBER_19_NHIf,
    MEMBER_20_NHIf, MEMBER_21_NHIf, MEMBER_22_NHIf, MEMBER_23_NHIf,
    MEMBER_24_NHIf, MEMBER_25_NHIf, MEMBER_26_NHIf, MEMBER_27_NHIf,
    MEMBER_28_NHIf, MEMBER_29_NHIf, MEMBER_30_NHIf, MEMBER_31_NHIf,
    MEMBER_32_NHIf, MEMBER_33_NHIf, MEMBER_34_NHIf, MEMBER_35_NHIf,
    MEMBER_36_NHIf, MEMBER_37_NHIf, MEMBER_38_NHIf, MEMBER_39_NHIf,
    MEMBER_40_NHIf, MEMBER_41_NHIf, MEMBER_42_NHIf, MEMBER_43_NHIf,
    MEMBER_44_NHIf, MEMBER_45_NHIf, MEMBER_46_NHIf, MEMBER_47_NHIf,
    MEMBER_48_NHIf, MEMBER_49_NHIf, MEMBER_50_NHIf, MEMBER_51_NHIf,
    MEMBER_52_NHIf, MEMBER_53_NHIf, MEMBER_54_NHIf, MEMBER_55_NHIf,
    MEMBER_56_NHIf, MEMBER_57_NHIf, MEMBER_58_NHIf, MEMBER_59_NHIf,
    MEMBER_60_NHIf, MEMBER_61_NHIf, MEMBER_62_NHIf, MEMBER_63_NHIf
};

static const bcmdrd_fid_t nhi_valid_field[BCMCTH_DLB_ECMP_GROUP_MAX_MEMBERS] = {
    MEMBER_0_NHI_VALIDf, MEMBER_1_NHI_VALIDf,
    MEMBER_2_NHI_VALIDf, MEMBER_3_NHI_VALIDf,
    MEMBER_4_NHI_VALIDf, MEMBER_5_NHI_VALIDf,
    MEMBER_6_NHI_VALIDf, MEMBER_7_NHI_VALIDf,
    MEMBER_8_NHI_VALIDf, MEMBER_9_NHI_VALIDf,
    MEMBER_10_NHI_VALIDf, MEMBER_11_NHI_VALIDf,
    MEMBER_12_NHI_VALIDf, MEMBER_13_NHI_VALIDf,
    MEMBER_14_NHI_VALIDf, MEMBER_15_NHI_VALIDf,
    MEMBER_16_NHI_VALIDf, MEMBER_17_NHI_VALIDf,
    MEMBER_18_NHI_VALIDf, MEMBER_19_NHI_VALIDf,
    MEMBER_20_NHI_VALIDf, MEMBER_21_NHI_VALIDf,
    MEMBER_22_NHI_VALIDf, MEMBER_23_NHI_VALIDf,
    MEMBER_24_NHI_VALIDf, MEMBER_25_NHI_VALIDf,
    MEMBER_26_NHI_VALIDf, MEMBER_27_NHI_VALIDf,
    MEMBER_28_NHI_VALIDf, MEMBER_29_NHI_VALIDf,
    MEMBER_30_NHI_VALIDf, MEMBER_31_NHI_VALIDf,
    MEMBER_32_NHI_VALIDf, MEMBER_33_NHI_VALIDf,
    MEMBER_34_NHI_VALIDf, MEMBER_35_NHI_VALIDf,
    MEMBER_36_NHI_VALIDf, MEMBER_37_NHI_VALIDf,
    MEMBER_38_NHI_VALIDf, MEMBER_39_NHI_VALIDf,
    MEMBER_40_NHI_VALIDf, MEMBER_41_NHI_VALIDf,
    MEMBER_42_NHI_VALIDf, MEMBER_43_NHI_VALIDf,
    MEMBER_44_NHI_VALIDf, MEMBER_45_NHI_VALIDf,
    MEMBER_46_NHI_VALIDf, MEMBER_47_NHI_VALIDf,
    MEMBER_48_NHI_VALIDf, MEMBER_49_NHI_VALIDf,
    MEMBER_50_NHI_VALIDf, MEMBER_51_NHI_VALIDf,
    MEMBER_52_NHI_VALIDf, MEMBER_53_NHI_VALIDf,
    MEMBER_54_NHI_VALIDf, MEMBER_55_NHI_VALIDf,
    MEMBER_56_NHI_VALIDf, MEMBER_57_NHI_VALIDf,
    MEMBER_58_NHI_VALIDf, MEMBER_59_NHI_VALIDf,
    MEMBER_60_NHI_VALIDf, MEMBER_61_NHI_VALIDf,
    MEMBER_62_NHI_VALIDf, MEMBER_63_NHI_VALIDf
};

static const bcmdrd_fid_t port_field[BCMCTH_DLB_ECMP_GROUP_MAX_MEMBERS] = {
    MEMBER_0_PORTf, MEMBER_1_PORTf, MEMBER_2_PORTf, MEMBER_3_PORTf,
    MEMBER_4_PORTf, MEMBER_5_PORTf, MEMBER_6_PORTf, MEMBER_7_PORTf,
    MEMBER_8_PORTf, MEMBER_9_PORTf, MEMBER_10_PORTf, MEMBER_11_PORTf,
    MEMBER_12_PORTf, MEMBER_13_PORTf, MEMBER_14_PORTf, MEMBER_15_PORTf,
    MEMBER_16_PORTf, MEMBER_17_PORTf, MEMBER_18_PORTf, MEMBER_19_PORTf,
    MEMBER_20_PORTf, MEMBER_21_PORTf, MEMBER_22_PORTf, MEMBER_23_PORTf,
    MEMBER_24_PORTf, MEMBER_25_PORTf, MEMBER_26_PORTf, MEMBER_27_PORTf,
    MEMBER_28_PORTf, MEMBER_29_PORTf, MEMBER_30_PORTf, MEMBER_31_PORTf,
    MEMBER_32_PORTf, MEMBER_33_PORTf, MEMBER_34_PORTf, MEMBER_35_PORTf,
    MEMBER_36_PORTf, MEMBER_37_PORTf, MEMBER_38_PORTf, MEMBER_39_PORTf,
    MEMBER_40_PORTf, MEMBER_41_PORTf, MEMBER_42_PORTf, MEMBER_43_PORTf,
    MEMBER_44_PORTf, MEMBER_45_PORTf, MEMBER_46_PORTf, MEMBER_47_PORTf,
    MEMBER_48_PORTf, MEMBER_49_PORTf, MEMBER_50_PORTf, MEMBER_51_PORTf,
    MEMBER_52_PORTf, MEMBER_53_PORTf, MEMBER_54_PORTf, MEMBER_55_PORTf,
    MEMBER_56_PORTf, MEMBER_57_PORTf, MEMBER_58_PORTf, MEMBER_59_PORTf,
    MEMBER_60_PORTf, MEMBER_61_PORTf, MEMBER_62_PORTf, MEMBER_63_PORTf
};

static const bcmdrd_fid_t port_valid_field[BCMCTH_DLB_ECMP_GROUP_MAX_MEMBERS] = {
    MEMBER_0_PORT_VALIDf, MEMBER_1_PORT_VALIDf,
    MEMBER_2_PORT_VALIDf, MEMBER_3_PORT_VALIDf,
    MEMBER_4_PORT_VALIDf, MEMBER_5_PORT_VALIDf,
    MEMBER_6_PORT_VALIDf, MEMBER_7_PORT_VALIDf,
    MEMBER_8_PORT_VALIDf, MEMBER_9_PORT_VALIDf,
    MEMBER_10_PORT_VALIDf, MEMBER_11_PORT_VALIDf,
    MEMBER_12_PORT_VALIDf, MEMBER_13_PORT_VALIDf,
    MEMBER_14_PORT_VALIDf, MEMBER_15_PORT_VALIDf,
    MEMBER_16_PORT_VALIDf, MEMBER_17_PORT_VALIDf,
    MEMBER_18_PORT_VALIDf, MEMBER_19_PORT_VALIDf,
    MEMBER_20_PORT_VALIDf, MEMBER_21_PORT_VALIDf,
    MEMBER_22_PORT_VALIDf, MEMBER_23_PORT_VALIDf,
    MEMBER_24_PORT_VALIDf, MEMBER_25_PORT_VALIDf,
    MEMBER_26_PORT_VALIDf, MEMBER_27_PORT_VALIDf,
    MEMBER_28_PORT_VALIDf, MEMBER_29_PORT_VALIDf,
    MEMBER_30_PORT_VALIDf, MEMBER_31_PORT_VALIDf,
    MEMBER_32_PORT_VALIDf, MEMBER_33_PORT_VALIDf,
    MEMBER_34_PORT_VALIDf, MEMBER_35_PORT_VALIDf,
    MEMBER_36_PORT_VALIDf, MEMBER_37_PORT_VALIDf,
    MEMBER_38_PORT_VALIDf, MEMBER_39_PORT_VALIDf,
    MEMBER_40_PORT_VALIDf, MEMBER_41_PORT_VALIDf,
    MEMBER_42_PORT_VALIDf, MEMBER_43_PORT_VALIDf,
    MEMBER_44_PORT_VALIDf, MEMBER_45_PORT_VALIDf,
    MEMBER_46_PORT_VALIDf, MEMBER_47_PORT_VALIDf,
    MEMBER_48_PORT_VALIDf, MEMBER_49_PORT_VALIDf,
    MEMBER_50_PORT_VALIDf, MEMBER_51_PORT_VALIDf,
    MEMBER_52_PORT_VALIDf, MEMBER_53_PORT_VALIDf,
    MEMBER_54_PORT_VALIDf, MEMBER_55_PORT_VALIDf,
    MEMBER_56_PORT_VALIDf, MEMBER_57_PORT_VALIDf,
    MEMBER_58_PORT_VALIDf, MEMBER_59_PORT_VALIDf,
    MEMBER_60_PORT_VALIDf, MEMBER_61_PORT_VALIDf,
    MEMBER_62_PORT_VALIDf, MEMBER_63_PORT_VALIDf
};

static const bcmdrd_fid_t dlb_enable_reg[2] = {
    DLB_ID_0_TO_63_ENABLEr,
    DLB_ID_64_TO_127_ENABLEr
};

/*******************************************************************************
 * Local functions
 */
static void
bcmcth_dlb_pt_op_info_t_init(bcmcth_dlb_pt_op_info_t *pt_op_info)
{
    if (pt_op_info) {
        sal_memset(&(pt_op_info->dyn_info), 0, sizeof(bcmbd_pt_dyn_info_t));
        sal_memset(&(pt_op_info->misc_info), 0, sizeof(bcmptm_misc_info_t));

        pt_op_info->drd_sid = BCM56990_B0_ENUM_COUNT;
        pt_op_info->trans_id = BCMPORT_INVALID;
        pt_op_info->req_lt_sid = BCM56990_B0_ENUM_COUNT;
        pt_op_info->rsp_lt_sid = BCM56990_B0_ENUM_COUNT;
        pt_op_info->req_flags = BCMPTM_REQ_FLAGS_NO_FLAGS;
        pt_op_info->rsp_flags = BCMPTM_REQ_FLAGS_NO_FLAGS;
        pt_op_info->op = BCMPTM_OP_NOP;
        pt_op_info->dyn_info.index = BCMPORT_INVALID;
        pt_op_info->dyn_info.tbl_inst = BCMPORT_ALL_PIPES_INST;
        pt_op_info->wsize = 0;
    }
    return;
}

static int
bcm56990_b0_dlb_ecmp_nhi_member_set(int unit, bcmcth_dlb_ecmp_t *lt_entry)
{
    int i;
    bcmcth_dlb_pt_op_info_t pt_op_info;
    uint32_t field_value = 0;
    uint32_t *buf_ptr = NULL;
    int mem_idx;
    uint8_t member_cnt_per_entry[DLB_NHI_MEMBER_TBL_CNT];
    DLB_ECMP_GROUP_NHI_MEMBERS_0_TO_12m_t nhi_member_entry[DLB_NHI_MEMBER_TBL_CNT];

    SHR_FUNC_ENTER(unit);

    if (!BCMCTH_DLB_LT_FIELD_GET(
             lt_entry->fbmp, BCMCTH_DLB_ECMP_LT_FIELD_NHOP_IDS)) {
        SHR_EXIT();

    }
    sal_memset(&nhi_member_entry, 0, sizeof(nhi_member_entry));
    sal_memset(&member_cnt_per_entry, 0, sizeof(member_cnt_per_entry));

    for (i = 0; i < BCMCTH_DLB_ECMP_GROUP_MAX_MEMBERS; i++) {
        mem_idx = i / DLB_NHI_MEMBER_CNT_PER_TBL;
        if (BCMCTH_DLB_LT_FIELD_GET(lt_entry->pri_nh_array_bmp, i)) {
            member_cnt_per_entry[mem_idx]++;
        }
        buf_ptr = (uint32_t *)(&nhi_member_entry[mem_idx]);

        /* set hw value */
        if (i < lt_entry->num_paths) {
            field_value = 1;
        } else {
            field_value = 0;
        }
        bcmdrd_pt_field_set(unit, nhi_sid[mem_idx], buf_ptr,
                                nhi_valid_field[i], &field_value);

        field_value = (uint32_t)(lt_entry->nhop_id[i]);
        bcmdrd_pt_field_set(unit, nhi_sid[mem_idx], buf_ptr,
                            nhi_field[i], &field_value);
    }

    for (i = 0; i < DLB_NHI_MEMBER_TBL_CNT; i++) {
        if (member_cnt_per_entry[i]) {
            buf_ptr = (uint32_t *)(&nhi_member_entry[i]);
            /* write */
            bcmcth_dlb_pt_op_info_t_init(&pt_op_info);
            pt_op_info.drd_sid = nhi_sid[i];
            pt_op_info.trans_id = lt_entry->op_arg->trans_id;
            pt_op_info.req_flags =
              bcmptm_lt_entry_attrib_to_ptm_req_flags(lt_entry->op_arg->attrib);
            pt_op_info.req_lt_sid = lt_entry->glt_sid;
            pt_op_info.op = BCMPTM_OP_WRITE;

            pt_op_info.wsize = bcmdrd_pt_entry_wsize(unit, nhi_sid[i]);

            /* Set up hardware table index to write. */
            pt_op_info.dyn_info.index = lt_entry->dlb_id;

            SHR_IF_ERR_EXIT
               (bcmcth_dlb_pt_write(unit, &pt_op_info, buf_ptr));
        }
    }
exit:
    SHR_FUNC_EXIT();
}

static int
bcm56990_b0_dlb_ecmp_port_member_set(int unit, bcmcth_dlb_ecmp_t *lt_entry)
{
    int i;
    bcmcth_dlb_pt_op_info_t pt_op_info;
    uint32_t field_value = 0;
    uint32_t *buf_ptr = NULL;
    int mem_idx;
    uint8_t member_cnt_per_entry[DLB_PORT_MEMBER_TBL_CNT];
    DLB_ECMP_GROUP_PORT_MEMBERS_0_TO_15m_t port_member_entry[DLB_PORT_MEMBER_TBL_CNT];

    SHR_FUNC_ENTER(unit);
    if (!BCMCTH_DLB_LT_FIELD_GET(
             lt_entry->fbmp, BCMCTH_DLB_ECMP_LT_FIELD_PORT_IDS)) {
        SHR_EXIT();

    }

    sal_memset(&port_member_entry, 0, sizeof(port_member_entry));
    sal_memset(&member_cnt_per_entry, 0, sizeof(member_cnt_per_entry));

    for (i = 0; i < BCMCTH_DLB_ECMP_GROUP_MAX_MEMBERS; i++) {
        mem_idx = i / DLB_PORT_MEMBER_CNT_PER_TBL;
        if (BCMCTH_DLB_LT_FIELD_GET(lt_entry->pri_port_array_bmp, i)) {
            member_cnt_per_entry[mem_idx]++;
        }
        buf_ptr = (uint32_t *)(&port_member_entry[mem_idx]);

        /* set hw value */
        if (i < lt_entry->num_paths) {
            field_value = 1;
        } else {
            field_value = 0;
        }
        bcmdrd_pt_field_set(unit, port_sid[mem_idx], buf_ptr,
                            port_valid_field[i], &field_value);

        field_value = (uint32_t)(lt_entry->port_id[i]);
        bcmdrd_pt_field_set(unit, port_sid[mem_idx], buf_ptr,
                            port_field[i], &field_value);
    }

    for (i = 0; i < DLB_PORT_MEMBER_TBL_CNT; i++) {
        if (member_cnt_per_entry[i]) {
            buf_ptr = (uint32_t *)(&port_member_entry[i]);
            /* write */
            bcmcth_dlb_pt_op_info_t_init(&pt_op_info);
            pt_op_info.drd_sid = port_sid[i];
            pt_op_info.trans_id = lt_entry->op_arg->trans_id;
            pt_op_info.req_flags =
              bcmptm_lt_entry_attrib_to_ptm_req_flags(lt_entry->op_arg->attrib);
            pt_op_info.req_lt_sid = lt_entry->glt_sid;
            pt_op_info.op = BCMPTM_OP_WRITE;

            pt_op_info.wsize = bcmdrd_pt_entry_wsize(unit, port_sid[i]);

            /* Set up hardware table index to write. */
            pt_op_info.dyn_info.index = lt_entry->dlb_id;

            SHR_IF_ERR_EXIT
               (bcmcth_dlb_pt_write(unit, &pt_op_info, buf_ptr));
        }
    }
exit:
    SHR_FUNC_EXIT();
}


static int
bcm56990_b0_dlb_ecmp_alternate_nhi_member_set(int unit, bcmcth_dlb_ecmp_t *lt_entry)
{
    int i;
    bcmcth_dlb_pt_op_info_t pt_op_info;
    uint32_t field_value = 0;
    uint32_t *buf_ptr = NULL;
    int mem_idx;
    uint8_t member_cnt_per_entry[DLB_NHI_MEMBER_TBL_CNT];
    DLB_ECMP_GROUP_ALT_NHI_MEMBERS_0_TO_12m_t nhi_member_entry[DLB_NHI_MEMBER_TBL_CNT];

    SHR_FUNC_ENTER(unit);
    if (!BCMCTH_DLB_LT_FIELD_GET(
             lt_entry->fbmp, BCMCTH_DLB_ECMP_LT_FIELD_ALTERNATE_NHOP_IDS)) {
        SHR_EXIT();

    }

    sal_memset(&nhi_member_entry, 0, sizeof(nhi_member_entry));
    sal_memset(&member_cnt_per_entry, 0, sizeof(member_cnt_per_entry));

    for (i = 0; i < BCMCTH_DLB_ECMP_GROUP_MAX_MEMBERS; i++) {
        mem_idx = i / DLB_NHI_MEMBER_CNT_PER_TBL;
        if (BCMCTH_DLB_LT_FIELD_GET(lt_entry->alt_nh_array_bmp, i)) {
            member_cnt_per_entry[mem_idx]++;
        }
        buf_ptr = (uint32_t *)(&nhi_member_entry[mem_idx]);

        /* set hw value */
        if (i < lt_entry->num_alternate_paths) {
            field_value = 1;
        } else {
            field_value = 0;
        }
        bcmdrd_pt_field_set(unit, alt_nhi_sid[mem_idx], buf_ptr,
                                nhi_valid_field[i], &field_value);

        field_value = (uint32_t)(lt_entry->alternate_nhop_id[i]);
        bcmdrd_pt_field_set(unit, alt_nhi_sid[mem_idx], buf_ptr,
                            nhi_field[i], &field_value);

    }
    for (i = 0; i < DLB_NHI_MEMBER_TBL_CNT; i++) {
        if (member_cnt_per_entry[i]) {
            buf_ptr = (uint32_t *)(&nhi_member_entry[i]);
            /* write */
            bcmcth_dlb_pt_op_info_t_init(&pt_op_info);
            pt_op_info.drd_sid = alt_nhi_sid[i];
            pt_op_info.trans_id = lt_entry->op_arg->trans_id;
            pt_op_info.req_flags =
              bcmptm_lt_entry_attrib_to_ptm_req_flags(lt_entry->op_arg->attrib);
            pt_op_info.req_lt_sid = lt_entry->glt_sid;
            pt_op_info.op = BCMPTM_OP_WRITE;

            pt_op_info.wsize = bcmdrd_pt_entry_wsize(unit, alt_nhi_sid[i]);

            /* Set up hardware table index to write. */
            pt_op_info.dyn_info.index = lt_entry->dlb_id;

            SHR_IF_ERR_EXIT
               (bcmcth_dlb_pt_write(unit, &pt_op_info, buf_ptr));
        }
    }
exit:
    SHR_FUNC_EXIT();
}


static int
bcm56990_b0_dlb_ecmp_alternate_port_member_set(int unit, bcmcth_dlb_ecmp_t *lt_entry)
{
    int i;
    bcmcth_dlb_pt_op_info_t pt_op_info;
    uint32_t field_value = 0;
    uint32_t *buf_ptr = NULL;
    int mem_idx;
    uint8_t member_cnt_per_entry[DLB_PORT_MEMBER_TBL_CNT];
    DLB_ECMP_GROUP_ALT_PORT_MEMBERS_0_TO_15m_t port_member_entry[DLB_PORT_MEMBER_TBL_CNT];

    SHR_FUNC_ENTER(unit);

    if (!BCMCTH_DLB_LT_FIELD_GET(
             lt_entry->fbmp, BCMCTH_DLB_ECMP_LT_FIELD_ALTERNATE_PORT_IDS)) {
        SHR_EXIT();

    }
    sal_memset(&port_member_entry, 0, sizeof(port_member_entry));
    sal_memset(&member_cnt_per_entry, 0, sizeof(member_cnt_per_entry));

    for (i = 0; i < BCMCTH_DLB_ECMP_GROUP_MAX_MEMBERS; i++) {
        mem_idx = i / DLB_PORT_MEMBER_CNT_PER_TBL;
        if (BCMCTH_DLB_LT_FIELD_GET(lt_entry->alt_port_array_bmp, i)) {
            member_cnt_per_entry[mem_idx]++;
        }
        buf_ptr = (uint32_t *)(&port_member_entry[mem_idx]);

        /* set hw value */
        if (i < lt_entry->num_alternate_paths) {
            field_value = 1;
        } else {
            field_value = 0;
        }
        bcmdrd_pt_field_set(unit, alt_port_sid[mem_idx], buf_ptr,
                                port_valid_field[i], &field_value);

        field_value = (uint32_t)(lt_entry->alternate_port_id[i]);
        bcmdrd_pt_field_set(unit, alt_port_sid[mem_idx], buf_ptr,
                            port_field[i], &field_value);
    }

    for (i = 0; i < DLB_PORT_MEMBER_TBL_CNT; i++) {
        if (member_cnt_per_entry[i]) {
            buf_ptr = (uint32_t *)(&port_member_entry[i]);
            /* write */
            bcmcth_dlb_pt_op_info_t_init(&pt_op_info);
            pt_op_info.drd_sid = alt_port_sid[i];
            pt_op_info.trans_id = lt_entry->op_arg->trans_id;
            pt_op_info.req_flags =
              bcmptm_lt_entry_attrib_to_ptm_req_flags(lt_entry->op_arg->attrib);
            pt_op_info.req_lt_sid = lt_entry->glt_sid;
            pt_op_info.op = BCMPTM_OP_WRITE;

            pt_op_info.wsize = bcmdrd_pt_entry_wsize(unit, alt_port_sid[i]);

            /* Set up hardware table index to write. */
            pt_op_info.dyn_info.index = lt_entry->dlb_id;

            SHR_IF_ERR_EXIT
               (bcmcth_dlb_pt_write(unit, &pt_op_info, buf_ptr));
        }
    }
exit:
    SHR_FUNC_EXIT();
}

static int
bcm56990_b0_dlb_ecmp_port_bitmap_set(int unit, bcmcth_dlb_ecmp_t *lt_entry)
{
    int i;
    bcmcth_dlb_pt_op_info_t pt_op_info;
    uint32_t *buf_ptr = NULL;
    DLB_ECMP_GROUP_MEMBERSHIPm_t group_membership_entry;
    bcmdrd_pbmp_t primary_pbmp;
    bcmdrd_pbmp_t alternate_pbmp;

    SHR_FUNC_ENTER(unit);
    if (!BCMCTH_DLB_LT_FIELD_GET(
             lt_entry->fbmp, BCMCTH_DLB_ECMP_LT_FIELD_PORT_IDS) &&
        !BCMCTH_DLB_LT_FIELD_GET(
            lt_entry->fbmp, BCMCTH_DLB_ECMP_LT_FIELD_ALTERNATE_PORT_IDS) &&
        !BCMCTH_DLB_LT_FIELD_GET(
            lt_entry->fbmp, BCMCTH_DLB_ECMP_LT_FIELD_NUM_PATHS) &&
        !BCMCTH_DLB_LT_FIELD_GET(
            lt_entry->fbmp, BCMCTH_DLB_ECMP_LT_FIELD_ALTERNATE_NUM_PATHS)) {
        SHR_EXIT();
    }

    sal_memset(&group_membership_entry, 0, sizeof(group_membership_entry));
    sal_memset(&primary_pbmp, 0, sizeof(primary_pbmp));
    sal_memset(&alternate_pbmp, 0, sizeof(alternate_pbmp));

    buf_ptr = (uint32_t *)(&group_membership_entry);

    /* Set hardware entry info. */
    for (i = 0; i < BCMCTH_DLB_ECMP_GROUP_MAX_MEMBERS; i++) {
        if (i < lt_entry->num_paths) {
            BCMDRD_PBMP_PORT_ADD(primary_pbmp, lt_entry->port_id[i]);
        }

        if (i < lt_entry->num_alternate_paths) {
            BCMDRD_PBMP_PORT_ADD(alternate_pbmp, lt_entry->alternate_port_id[i]);
        }
    }

    bcmdrd_pt_field_set(unit, DLB_ECMP_GROUP_MEMBERSHIPm, buf_ptr,
                        PORT_MAPf, (uint32_t *)(&primary_pbmp));

    bcmdrd_pt_field_set(unit, DLB_ECMP_GROUP_MEMBERSHIPm, buf_ptr,
                        PORT_MAP_ALTERNATEf, (uint32_t *)(&alternate_pbmp));


    /* write */
    bcmcth_dlb_pt_op_info_t_init(&pt_op_info);
    pt_op_info.drd_sid = DLB_ECMP_GROUP_MEMBERSHIPm;
    pt_op_info.trans_id = lt_entry->op_arg->trans_id;
    pt_op_info.req_flags =
              bcmptm_lt_entry_attrib_to_ptm_req_flags(lt_entry->op_arg->attrib);
    pt_op_info.req_lt_sid = lt_entry->glt_sid;
    pt_op_info.op = BCMPTM_OP_WRITE;

    pt_op_info.wsize = bcmdrd_pt_entry_wsize(unit, DLB_ECMP_GROUP_MEMBERSHIPm);

    /* Set up hardware table index to write. */
    pt_op_info.dyn_info.index = lt_entry->dlb_id;

    SHR_IF_ERR_EXIT
       (bcmcth_dlb_pt_write(unit, &pt_op_info, buf_ptr));

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56990_b0_flow_set_base_get(int unit, bcmcth_dlb_ecmp_t *lt_entry)
{
    int blk_base_ptr, max_blk_base_ptr;
    int mem_tbl_size;
    int total_blks;
    int num_blks;
    SHR_FUNC_ENTER(unit);

    mem_tbl_size =
        bcmdrd_pt_index_max(unit, DLB_ECMP_FLOWSET_INST0m) -
        bcmdrd_pt_index_min(unit, DLB_ECMP_FLOWSET_INST0m) + 1;
    num_blks = 1 << (lt_entry->flow_set_size - 1);
    total_blks = mem_tbl_size >> DLB_ECMP_FLOWSET_BLKSZ_SHIFT;

    max_blk_base_ptr = total_blks - num_blks;

    for (blk_base_ptr = 0; blk_base_ptr <= max_blk_base_ptr; blk_base_ptr++) {
        /*
         * Check if the contiguous region of DLB_ECMP_FLOWSETm table
         * from index base_ptr to (base_ptr + num_entries - 1) is
         * free.
         */
        if (DLB_ECMP_FLOWSET_MEMBER_BLK_NULL_RANGE(unit, blk_base_ptr, num_blks)) {
            break;
        }
    }

    if (blk_base_ptr > max_blk_base_ptr) {
        SHR_ERR_EXIT(SHR_E_RESOURCE);
    }

    lt_entry->flow_set_base = blk_base_ptr;
 exit:
    SHR_FUNC_EXIT();
}

static int
bcm56990_b0_dlb_ecmp_flow_set_alloc(int unit, bcmcth_dlb_ecmp_t *lt_entry)
{
    SHR_FUNC_ENTER(unit);

    if (lt_entry->flow_set_size != lt_entry->prev_flow_set_size) {
        SHR_IF_ERR_EXIT
            (bcm56990_b0_flow_set_base_get(unit, lt_entry));
    }

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56990_b0_dlb_ecmp_flow_set_init(int unit, bcmcth_dlb_ecmp_t *lt_entry)
{
    bcmdrd_sid_t flowset_sid[2] = {DLB_ECMP_FLOWSET_INST0m,
                                   DLB_ECMP_FLOWSET_INST1m};
    bcmdrd_sid_t mflowset_sid[2] = {DLB_ECMP_FLOWSET_MEMBER_INST0m,
                                    DLB_ECMP_FLOWSET_MEMBER_INST1m};
    bcmcth_dlb_pt_op_info_t pt_op_info;
    int block_base_ptr;
    int num_entries_per_block;
    int flowset_entry_wsize, mflowset_entry_wsize;
    int num_blocks, prev_num_blocks;
    int alloc_size;
    int i, k, j, sid_idx, rv;
    uint32_t *block_ptr = NULL;
    uint32_t *mblock_ptr = NULL;
    uint32_t *flowset_buf_ptr = NULL;
    uint32_t *mflowset_buf_ptr = NULL;
    uint32_t field_value;
    uint8_t  dlb_member_count;

    SHR_FUNC_ENTER(unit);
    /* Do nothing if flow set size is not touched. */
    if (!BCMCTH_DLB_LT_FIELD_GET(
             lt_entry->fbmp, BCMCTH_DLB_ECMP_LT_FIELD_FLOW_SET_SIZE)) {
        SHR_EXIT();
    }

    if (lt_entry->prev_flow_set_size == lt_entry->flow_set_size) {
        SHR_EXIT();
    }

    prev_num_blocks = 1 << (lt_entry->prev_flow_set_size - 1);
    if (lt_entry->prev_flow_set_size != lt_entry->flow_set_size &&
        lt_entry->prev_flow_set_base != DLB_INVALID_FLOWSET_BASE) {
        /* remove the previous flowset ref cnt. */
        DLB_ECMP_FLOWSET_MEMBER_BLK_USED_CLR_RANGE(
            unit, lt_entry->prev_flow_set_base, prev_num_blocks);
    }

    rv = bcm56990_b0_dlb_ecmp_flow_set_alloc(unit, lt_entry);
    if (rv != SHR_E_NONE) {
        DLB_ECMP_FLOWSET_MEMBER_BLK_USED_SET_RANGE(
            unit, lt_entry->prev_flow_set_base, prev_num_blocks);
        SHR_IF_ERR_VERBOSE_EXIT(rv);
    }

    flowset_entry_wsize = bcmdrd_pt_entry_wsize(unit, DLB_ECMP_FLOWSET_INST0m);
    mflowset_entry_wsize = bcmdrd_pt_entry_wsize(unit, DLB_ECMP_FLOWSET_MEMBER_INST0m);
    num_blocks = 1 << (lt_entry->flow_set_size - 1);
    /* Set DLB flow set table */
    block_base_ptr = lt_entry->flow_set_base;
    num_entries_per_block = 1 << DLB_ECMP_FLOWSET_BLKSZ_SHIFT;
    alloc_size = num_entries_per_block * flowset_entry_wsize;
    block_ptr = sal_alloc(BCMDRD_WORDS2BYTES(alloc_size),
                          "bcmcthDlbEcmpFlowsetBlock");
    if (NULL == block_ptr) {
        SHR_ERR_EXIT(SHR_E_MEMORY);
    }
    sal_memset(block_ptr, 0, BCMDRD_WORDS2BYTES(alloc_size));

    alloc_size = num_entries_per_block * mflowset_entry_wsize;
    mblock_ptr = sal_alloc(BCMDRD_WORDS2BYTES(alloc_size),
                           "bcmcthDlbEcmpFlowsetMemberBlock");
    if (NULL == mblock_ptr) {
        SHR_ERR_EXIT(SHR_E_MEMORY);
    }
    sal_memset(mblock_ptr, 0, BCMDRD_WORDS2BYTES(alloc_size));

    dlb_member_count = lt_entry->num_paths + lt_entry->num_alternate_paths;

    for (i = 0; i < num_blocks; i++) {
        for (k = 0; k < num_entries_per_block; k++) {
            flowset_buf_ptr = block_ptr + k * flowset_entry_wsize;
            mflowset_buf_ptr = mblock_ptr + k * mflowset_entry_wsize;
            if (dlb_member_count > 0) {
                field_value = 1;
                bcmdrd_pt_field_set(unit, DLB_ECMP_FLOWSET_INST0m,
                                    flowset_buf_ptr, VALIDf, &field_value);
                j = (i * num_entries_per_block + k) % dlb_member_count;

                if (j < lt_entry->num_paths) {
                    field_value = (uint32_t)lt_entry->port_id[j];
                    bcmdrd_pt_field_set(unit, DLB_ECMP_FLOWSET_INST0m,
                                        flowset_buf_ptr,
                                        PORT_MEMBER_ASSIGNMENTf, &field_value);

                    field_value = j;
                    bcmdrd_pt_field_set(unit, DLB_ECMP_FLOWSET_MEMBER_INST0m,
                                        mflowset_buf_ptr,
                                        ECMP_MEMBER_ASSIGNMENTf, &field_value);

                    field_value = 0;
                    bcmdrd_pt_field_set(unit, DLB_ECMP_FLOWSET_MEMBER_INST0m,
                                        mflowset_buf_ptr,
                                        ECMP_MEMBER_IS_ALTERNATEf, &field_value);
                } else {
                    field_value = (uint32_t)lt_entry->alternate_port_id[j -
                                   lt_entry->num_paths];
                    bcmdrd_pt_field_set(unit, DLB_ECMP_FLOWSET_INST0m,
                                        flowset_buf_ptr,
                                        PORT_MEMBER_ASSIGNMENTf, &field_value);

                    field_value = j - lt_entry->num_paths;
                    bcmdrd_pt_field_set(unit, DLB_ECMP_FLOWSET_MEMBER_INST0m,
                                        mflowset_buf_ptr,
                                        ECMP_MEMBER_ASSIGNMENTf, &field_value);

                    field_value = 1;
                    bcmdrd_pt_field_set(unit, DLB_ECMP_FLOWSET_MEMBER_INST0m,
                                        mflowset_buf_ptr,
                                        ECMP_MEMBER_IS_ALTERNATEf, &field_value);
                }
            }
        }

        /* Write DLB_ECMP_FLOWSET. */
        for (sid_idx = 0; sid_idx < COUNTOF(flowset_sid); sid_idx++) {
            bcmcth_dlb_pt_op_info_t_init(&pt_op_info);
            pt_op_info.drd_sid = flowset_sid[sid_idx];
            pt_op_info.trans_id = lt_entry->op_arg->trans_id;
            pt_op_info.req_flags =
              bcmptm_lt_entry_attrib_to_ptm_req_flags(lt_entry->op_arg->attrib);
            pt_op_info.req_lt_sid = lt_entry->glt_sid;
            pt_op_info.op = BCMPTM_OP_WRITE;
            pt_op_info.wsize = flowset_entry_wsize;
            pt_op_info.misc_info.dma_enable = TRUE;
            pt_op_info.misc_info.dma_entry_count = num_entries_per_block;

            /* Set up hardware table index to write. */
            pt_op_info.dyn_info.index = (block_base_ptr + i) * num_entries_per_block;
            pt_op_info.dyn_info.tbl_inst = -1;

            SHR_IF_ERR_EXIT
                (bcmcth_dlb_pt_dma_write(unit, &pt_op_info, block_ptr));
        }

        /* Write DLB_ECMP_FLOWSET_MEMBER. */
        for (sid_idx = 0; sid_idx < COUNTOF(mflowset_sid); sid_idx++) {
            bcmcth_dlb_pt_op_info_t_init(&pt_op_info);
            pt_op_info.drd_sid = mflowset_sid[sid_idx];
            pt_op_info.trans_id = lt_entry->op_arg->trans_id;
            pt_op_info.req_flags =
              bcmptm_lt_entry_attrib_to_ptm_req_flags(lt_entry->op_arg->attrib);
            pt_op_info.req_lt_sid = lt_entry->glt_sid;
            pt_op_info.op = BCMPTM_OP_WRITE;
            pt_op_info.wsize = mflowset_entry_wsize;
            pt_op_info.misc_info.dma_enable = TRUE;
            pt_op_info.misc_info.dma_entry_count = num_entries_per_block;

            /* Set up hardware table index to write. */
            pt_op_info.dyn_info.index = (block_base_ptr + i) * num_entries_per_block;
            pt_op_info.dyn_info.tbl_inst = -1;

            SHR_IF_ERR_EXIT
                (bcmcth_dlb_pt_dma_write(unit, &pt_op_info, mblock_ptr));
        }
    }

    DLB_ECMP_FLOWSET_MEMBER_BLK_USED_SET_RANGE(unit, block_base_ptr, num_blocks);
exit:
    if (block_ptr != NULL) {
        sal_free(block_ptr);
    }
    if (mblock_ptr != NULL) {
        sal_free(mblock_ptr);
    }
    SHR_FUNC_EXIT();
}

static int
bcm56990_b0_dlb_ecmp_grp_ctrl_set(int unit, bcmcth_dlb_ecmp_t *lt_entry)
{
    bcmcth_dlb_pt_op_info_t pt_op_info;
    uint32_t field_value = 0;
    uint32_t *buf_ptr = NULL;

    DLB_ECMP_GROUP_CONTROLm_t dlb_ecmp_group_control_buf;

    SHR_FUNC_ENTER(unit);

    if (!BCMCTH_DLB_LT_FIELD_GET(
             lt_entry->fbmp, BCMCTH_DLB_ECMP_LT_FIELD_INACTIVITY_TIME) &&
         !BCMCTH_DLB_LT_FIELD_GET(
             lt_entry->fbmp, BCMCTH_DLB_ECMP_LT_FIELD_ASSIGNMENT_MODE) &&
         !BCMCTH_DLB_LT_FIELD_GET(
             lt_entry->fbmp, BCMCTH_DLB_ECMP_LT_FIELD_FLOW_SET_SIZE) &&
         !BCMCTH_DLB_LT_FIELD_GET(
             lt_entry->fbmp, BCMCTH_DLB_ECMP_LT_FIELD_ALTERNATE_PATH_COST) &&
         !BCMCTH_DLB_LT_FIELD_GET(
             lt_entry->fbmp, BCMCTH_DLB_ECMP_LT_FIELD_ALTERNATE_PATH_BIAS) &&
         !BCMCTH_DLB_LT_FIELD_GET(
             lt_entry->fbmp, BCMCTH_DLB_ECMP_LT_FIELD_PRIMARY_PATH_THRESHOLD) &&
         !BCMCTH_DLB_LT_FIELD_GET(
             lt_entry->fbmp, BCMCTH_DLB_ECMP_LT_FIELD_NUM_PATHS)) {
        SHR_EXIT();
    }
    sal_memset(&dlb_ecmp_group_control_buf, 0, sizeof(dlb_ecmp_group_control_buf));


    buf_ptr = (uint32_t *)&dlb_ecmp_group_control_buf;

    field_value = lt_entry->dlb_id;
    bcmdrd_pt_field_set(unit, DLB_ECMP_GROUP_CONTROLm, buf_ptr, GROUP_MEMBERSHIP_POINTERf,
                            &field_value);

    field_value = lt_entry->inactivity_time;
    bcmdrd_pt_field_set(unit, DLB_ECMP_GROUP_CONTROLm, buf_ptr, INACTIVITY_DURATIONf,
                        &field_value);


    field_value = lt_entry->assignment_mode;
    bcmdrd_pt_field_set(unit, DLB_ECMP_GROUP_CONTROLm, buf_ptr, PORT_ASSIGNMENT_MODEf,
                        &field_value);


    field_value = lt_entry->flow_set_size;
    bcmdrd_pt_field_set(unit, DLB_ECMP_GROUP_CONTROLm, buf_ptr, FLOW_SET_SIZEf,
                        &field_value);
    field_value = lt_entry->flow_set_base << DLB_ECMP_FLOWSET_BLKSZ_SHIFT;
    bcmdrd_pt_field_set(unit, DLB_ECMP_GROUP_CONTROLm, buf_ptr, FLOW_SET_BASEf,
                        &field_value);

    field_value = lt_entry->alternate_path_cost;
    bcmdrd_pt_field_set(unit, DLB_ECMP_GROUP_CONTROLm, buf_ptr, ALTERNATE_PATH_COSTf,
                        &field_value);

    field_value = lt_entry->alternate_path_bias;
    bcmdrd_pt_field_set(unit, DLB_ECMP_GROUP_CONTROLm, buf_ptr, ALTERNATE_PATH_BIASf,
                        &field_value);


    field_value = lt_entry->primary_path_threshold;
    bcmdrd_pt_field_set(unit, DLB_ECMP_GROUP_CONTROLm, buf_ptr, PRIMARY_PATH_THRESHOLDf,
                        &field_value);


    field_value = lt_entry->num_paths;
    bcmdrd_pt_field_set(unit, DLB_ECMP_GROUP_CONTROLm, buf_ptr, PRIMARY_GROUP_SIZEf,
                        &field_value);

    field_value = 1;
    bcmdrd_pt_field_set(unit, DLB_ECMP_GROUP_CONTROLm, buf_ptr, ENABLE_OPTIMAL_CANDIDATE_UPDATEf,
                        &field_value);

    /* write */
    bcmcth_dlb_pt_op_info_t_init(&pt_op_info);
    pt_op_info.drd_sid = DLB_ECMP_GROUP_CONTROLm;
    pt_op_info.trans_id = lt_entry->op_arg->trans_id;
    pt_op_info.req_flags =
              bcmptm_lt_entry_attrib_to_ptm_req_flags(lt_entry->op_arg->attrib);
    pt_op_info.req_lt_sid = lt_entry->glt_sid;
    pt_op_info.op = BCMPTM_OP_WRITE;

    pt_op_info.wsize = bcmdrd_pt_entry_wsize(unit, DLB_ECMP_GROUP_CONTROLm);

    /* Set up hardware table index to write. */
    pt_op_info.dyn_info.index = lt_entry->dlb_id;

    SHR_IF_ERR_EXIT
        (bcmcth_dlb_pt_write(unit, &pt_op_info, buf_ptr));
exit:
    SHR_FUNC_EXIT();
}

static int
bcm56990_b0_dlb_ecmp_enable(int unit, bcmcth_dlb_ecmp_t *lt_entry, bool enable)
{
    bcmcth_dlb_pt_op_info_t pt_op_info;
    uint32_t *buf_ptr = NULL;
    uint64_t en_bitmap64 = 0;
    uint32_t en_bitmap[2] = {0, 0};
    int reg_idx;
    DLB_ID_0_TO_63_ENABLEr_t dlb_en_entry;

    SHR_FUNC_ENTER(unit);
    if (DLB_ECMP_GROUP_ENABLE(unit, lt_entry->dlb_id) == enable) {
        SHR_EXIT();
    }
    sal_memset(&dlb_en_entry, 0, sizeof(dlb_en_entry));
    buf_ptr = (uint32_t *)&dlb_en_entry;
    reg_idx = lt_entry->dlb_id / 64;
    /* read */
    bcmcth_dlb_pt_op_info_t_init(&pt_op_info);
    pt_op_info.drd_sid = dlb_enable_reg[reg_idx];
    pt_op_info.trans_id = lt_entry->op_arg->trans_id;
    pt_op_info.req_flags =
              bcmptm_lt_entry_attrib_to_ptm_req_flags(lt_entry->op_arg->attrib);
    pt_op_info.req_lt_sid = lt_entry->glt_sid;
    pt_op_info.op = BCMPTM_OP_READ;

    pt_op_info.wsize = bcmdrd_pt_entry_wsize(unit, dlb_enable_reg[reg_idx]);

    /* Set up hardware table index to write. */
    pt_op_info.dyn_info.index = 0;

    SHR_IF_ERR_EXIT
       (bcmcth_dlb_pt_read(unit, &pt_op_info, buf_ptr));

    bcmdrd_pt_field_get(unit, dlb_enable_reg[reg_idx], buf_ptr,
                        BITMAPf, en_bitmap);

    en_bitmap64 = (((uint64_t)en_bitmap[1]) << 32) | en_bitmap[0];

    if (enable) {
        en_bitmap64 |= 1LL << (lt_entry->dlb_id);
    } else {
        en_bitmap64 &= ~(1LL << (lt_entry->dlb_id));
    }

    en_bitmap[0] = en_bitmap64 & 0xffffffff;
    en_bitmap[1] = (en_bitmap64 >> 32) & 0xffffffff;

    bcmdrd_pt_field_set(unit, dlb_enable_reg[reg_idx], buf_ptr,
                        BITMAPf, en_bitmap);

    /* write */
    bcmcth_dlb_pt_op_info_t_init(&pt_op_info);
    pt_op_info.drd_sid = dlb_enable_reg[reg_idx];
    pt_op_info.trans_id = lt_entry->op_arg->trans_id;
    pt_op_info.req_flags =
              bcmptm_lt_entry_attrib_to_ptm_req_flags(lt_entry->op_arg->attrib);
    pt_op_info.req_lt_sid = lt_entry->glt_sid;
    pt_op_info.op = BCMPTM_OP_WRITE;

    pt_op_info.wsize = bcmdrd_pt_entry_wsize(unit, dlb_enable_reg[reg_idx]);

    /* Set up hardware table index to write. */
    pt_op_info.dyn_info.index = 0;

    SHR_IF_ERR_EXIT
       (bcmcth_dlb_pt_write(unit, &pt_op_info, buf_ptr));

    DLB_ECMP_GROUP_ENABLE(unit, lt_entry->dlb_id) = enable;

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56990_b0_dlb_ecmp_nhi_member_clr(int unit, bcmcth_dlb_ecmp_t *lt_entry)
{
    int i;
    bcmcth_dlb_pt_op_info_t pt_op_info;
    uint32_t *buf_ptr = NULL;
    DLB_ECMP_GROUP_NHI_MEMBERS_0_TO_12m_t nhi_member_entry[DLB_NHI_MEMBER_TBL_CNT];

    SHR_FUNC_ENTER(unit);

    sal_memset(&nhi_member_entry, 0, sizeof(nhi_member_entry));

    for (i = 0; i < DLB_NHI_MEMBER_TBL_CNT; i++) {
        buf_ptr = (uint32_t *)(&nhi_member_entry[i]);
        /* write */
        bcmcth_dlb_pt_op_info_t_init(&pt_op_info);
        pt_op_info.drd_sid = nhi_sid[i];
        pt_op_info.trans_id = lt_entry->op_arg->trans_id;
        pt_op_info.req_flags =
              bcmptm_lt_entry_attrib_to_ptm_req_flags(lt_entry->op_arg->attrib);
        pt_op_info.req_lt_sid = lt_entry->glt_sid;
        pt_op_info.op = BCMPTM_OP_WRITE;

        pt_op_info.wsize = bcmdrd_pt_entry_wsize(unit, nhi_sid[i]);

        /* Set up hardware table index to write. */
        pt_op_info.dyn_info.index = lt_entry->dlb_id;

        SHR_IF_ERR_EXIT
           (bcmcth_dlb_pt_write(unit, &pt_op_info, buf_ptr));
    }
exit:
    SHR_FUNC_EXIT();
}

static int
bcm56990_b0_dlb_ecmp_port_member_clr(int unit, bcmcth_dlb_ecmp_t *lt_entry)
{
    int i;
    bcmcth_dlb_pt_op_info_t pt_op_info;
    uint32_t *buf_ptr = NULL;
    DLB_ECMP_GROUP_PORT_MEMBERS_0_TO_15m_t port_member_entry[DLB_PORT_MEMBER_TBL_CNT];

    SHR_FUNC_ENTER(unit);

    sal_memset(&port_member_entry, 0, sizeof(port_member_entry));

    for (i = 0; i < DLB_PORT_MEMBER_TBL_CNT; i++) {
        buf_ptr = (uint32_t *)(&port_member_entry[i]);
        /* write */
        bcmcth_dlb_pt_op_info_t_init(&pt_op_info);
        pt_op_info.drd_sid = port_sid[i];
        pt_op_info.trans_id = lt_entry->op_arg->trans_id;
        pt_op_info.req_flags =
              bcmptm_lt_entry_attrib_to_ptm_req_flags(lt_entry->op_arg->attrib);
        pt_op_info.req_lt_sid = lt_entry->glt_sid;
        pt_op_info.op = BCMPTM_OP_WRITE;

        pt_op_info.wsize = bcmdrd_pt_entry_wsize(unit, port_sid[i]);

        /* Set up hardware table index to write. */
        pt_op_info.dyn_info.index = lt_entry->dlb_id;

        SHR_IF_ERR_EXIT
           (bcmcth_dlb_pt_write(unit, &pt_op_info, buf_ptr));
    }
exit:
    SHR_FUNC_EXIT();
}

static int
bcm56990_b0_dlb_ecmp_alternate_nhi_member_clr(int unit, bcmcth_dlb_ecmp_t *lt_entry)
{
    int i;
    bcmcth_dlb_pt_op_info_t pt_op_info;
    uint32_t *buf_ptr = NULL;
    DLB_ECMP_GROUP_ALT_NHI_MEMBERS_0_TO_12m_t nhi_member_entry[DLB_NHI_MEMBER_TBL_CNT];

    SHR_FUNC_ENTER(unit);

    sal_memset(&nhi_member_entry, 0, sizeof(nhi_member_entry));

    for (i = 0; i < DLB_NHI_MEMBER_TBL_CNT; i++) {
        buf_ptr = (uint32_t *)(&nhi_member_entry[i]);
        /* write */
        bcmcth_dlb_pt_op_info_t_init(&pt_op_info);
        pt_op_info.drd_sid = alt_nhi_sid[i];
        pt_op_info.trans_id = lt_entry->op_arg->trans_id;
        pt_op_info.req_flags =
              bcmptm_lt_entry_attrib_to_ptm_req_flags(lt_entry->op_arg->attrib);
        pt_op_info.req_lt_sid = lt_entry->glt_sid;
        pt_op_info.op = BCMPTM_OP_WRITE;

        pt_op_info.wsize = bcmdrd_pt_entry_wsize(unit, alt_nhi_sid[i]);

        /* Set up hardware table index to write. */
        pt_op_info.dyn_info.index = lt_entry->dlb_id;

        SHR_IF_ERR_EXIT
           (bcmcth_dlb_pt_write(unit, &pt_op_info, buf_ptr));
    }
exit:
    SHR_FUNC_EXIT();
}

static int
bcm56990_b0_dlb_ecmp_alternate_port_member_clr(int unit, bcmcth_dlb_ecmp_t *lt_entry)
{
    int i;
    bcmcth_dlb_pt_op_info_t pt_op_info;
    uint32_t *buf_ptr = NULL;
    DLB_ECMP_GROUP_ALT_PORT_MEMBERS_0_TO_15m_t port_member_entry[DLB_PORT_MEMBER_TBL_CNT];

    SHR_FUNC_ENTER(unit);

    sal_memset(&port_member_entry, 0, sizeof(port_member_entry));

    for (i = 0; i < DLB_PORT_MEMBER_TBL_CNT; i++) {
        buf_ptr = (uint32_t *)(&port_member_entry[i]);
        /* write */
        bcmcth_dlb_pt_op_info_t_init(&pt_op_info);
        pt_op_info.drd_sid = alt_port_sid[i];
        pt_op_info.trans_id = lt_entry->op_arg->trans_id;
        pt_op_info.req_flags =
              bcmptm_lt_entry_attrib_to_ptm_req_flags(lt_entry->op_arg->attrib);
        pt_op_info.req_lt_sid = lt_entry->glt_sid;
        pt_op_info.op = BCMPTM_OP_WRITE;

        pt_op_info.wsize = bcmdrd_pt_entry_wsize(unit, alt_port_sid[i]);

        /* Set up hardware table index to write. */
        pt_op_info.dyn_info.index = lt_entry->dlb_id;

        SHR_IF_ERR_EXIT
           (bcmcth_dlb_pt_write(unit, &pt_op_info, buf_ptr));
    }
exit:
    SHR_FUNC_EXIT();
}

static int
bcm56990_b0_dlb_ecmp_grp_ctrl_clr(int unit, bcmcth_dlb_ecmp_t *lt_entry)
{
    bcmcth_dlb_pt_op_info_t pt_op_info;
    uint32_t *buf_ptr = NULL;

    DLB_ECMP_GROUP_CONTROLm_t dlb_ecmp_group_control_buf;

    SHR_FUNC_ENTER(unit);

    sal_memset(&dlb_ecmp_group_control_buf, 0, sizeof(dlb_ecmp_group_control_buf));

    buf_ptr = (uint32_t *)&dlb_ecmp_group_control_buf;

    /* write */
    bcmcth_dlb_pt_op_info_t_init(&pt_op_info);
    pt_op_info.drd_sid = DLB_ECMP_GROUP_CONTROLm;
    pt_op_info.trans_id = lt_entry->op_arg->trans_id;
    pt_op_info.req_flags =
              bcmptm_lt_entry_attrib_to_ptm_req_flags(lt_entry->op_arg->attrib);
    pt_op_info.req_lt_sid = lt_entry->glt_sid;
    pt_op_info.op = BCMPTM_OP_WRITE;

    pt_op_info.wsize = bcmdrd_pt_entry_wsize(unit, DLB_ECMP_GROUP_CONTROLm);

    /* Set up hardware table index to write. */
    pt_op_info.dyn_info.index = lt_entry->dlb_id;

    SHR_IF_ERR_EXIT
        (bcmcth_dlb_pt_write(unit, &pt_op_info, buf_ptr));
exit:
    SHR_FUNC_EXIT();
}

static int
bcm56990_b0_dlb_ecmp_port_bitmap_clr(int unit, bcmcth_dlb_ecmp_t *lt_entry)
{
    bcmcth_dlb_pt_op_info_t pt_op_info;
    uint32_t *buf_ptr = NULL;
    DLB_ECMP_GROUP_MEMBERSHIPm_t group_membership_entry;

    SHR_FUNC_ENTER(unit);

    sal_memset(&group_membership_entry, 0, sizeof(group_membership_entry));

    buf_ptr = (uint32_t *)(&group_membership_entry);

    /* write */
    bcmcth_dlb_pt_op_info_t_init(&pt_op_info);
    pt_op_info.drd_sid = DLB_ECMP_GROUP_MEMBERSHIPm;
    pt_op_info.trans_id = lt_entry->op_arg->trans_id;
    pt_op_info.req_flags =
              bcmptm_lt_entry_attrib_to_ptm_req_flags(lt_entry->op_arg->attrib);
    pt_op_info.req_lt_sid = lt_entry->glt_sid;
    pt_op_info.op = BCMPTM_OP_WRITE;

    pt_op_info.wsize = bcmdrd_pt_entry_wsize(unit, DLB_ECMP_GROUP_MEMBERSHIPm);

    /* Set up hardware table index to write. */
    pt_op_info.dyn_info.index = lt_entry->dlb_id;

    SHR_IF_ERR_EXIT
       (bcmcth_dlb_pt_write(unit, &pt_op_info, buf_ptr));

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56990_b0_dlb_ecmp_flow_set_cleanup(int unit, bcmcth_dlb_ecmp_t *lt_entry)
{
    int flow_set_base;
    int num_blocks;

    SHR_FUNC_ENTER(unit);

    /* remove the flowset ref cnt */
    flow_set_base = lt_entry->flow_set_base;
    num_blocks = 1 << (lt_entry->flow_set_size - 1);
    DLB_ECMP_FLOWSET_MEMBER_BLK_USED_CLR_RANGE(unit, flow_set_base, num_blocks);

    SHR_FUNC_EXIT();
}

static int
bcm56990_b0_dlb_ecmp_link_ctrl_set(int unit, bcmcth_dlb_ecmp_pctl_t *lt_entry)
{
    bcmcth_dlb_pt_op_info_t pt_op_info;
    uint32_t *buf_ptr = NULL;
    DLB_ECMP_LINK_CONTROLm_t   link_ctrl_entry;
    bcmdrd_pbmp_t override_pbmp;
    bcmdrd_pbmp_t force_link_status_pbmp;

    SHR_FUNC_ENTER(unit);
    if (!BCMCTH_DLB_LT_FIELD_GET(
             lt_entry->fbmp, BCMCTH_DLB_ECMP_PCTL_LT_FIELD_OVERRIDE) &&
        !BCMCTH_DLB_LT_FIELD_GET(
            lt_entry->fbmp, BCMCTH_DLB_ECMP_PCTL_LT_FIELD_FORCE_LINK_STATUS)) {
        SHR_EXIT();
    }

    sal_memset(&link_ctrl_entry, 0, sizeof(link_ctrl_entry));
    sal_memset(&override_pbmp, 0, sizeof(override_pbmp));
    sal_memset(&force_link_status_pbmp, 0, sizeof(force_link_status_pbmp));

    buf_ptr = (uint32_t *)(&link_ctrl_entry);

    /* read */
    bcmcth_dlb_pt_op_info_t_init(&pt_op_info);
    pt_op_info.drd_sid = DLB_ECMP_LINK_CONTROLm;
    pt_op_info.trans_id = lt_entry->op_arg->trans_id;
    pt_op_info.req_flags =
              bcmptm_lt_entry_attrib_to_ptm_req_flags(lt_entry->op_arg->attrib);
    pt_op_info.req_lt_sid = lt_entry->glt_sid;
    pt_op_info.op = BCMPTM_OP_READ;

    pt_op_info.wsize = bcmdrd_pt_entry_wsize(unit, DLB_ECMP_LINK_CONTROLm);

    /* Set up hardware table index to write. */
    pt_op_info.dyn_info.index = 0;

    SHR_IF_ERR_EXIT
       (bcmcth_dlb_pt_read(unit, &pt_op_info, buf_ptr));

    bcmdrd_pt_field_get(unit, DLB_ECMP_LINK_CONTROLm, buf_ptr,
                        SW_PORT_STATEf, (uint32_t *)(&force_link_status_pbmp));

    bcmdrd_pt_field_get(unit, DLB_ECMP_LINK_CONTROLm, buf_ptr,
                        SW_OVERRIDE_PORT_MAPf, (uint32_t *)(&override_pbmp));

    /* Set hardware entry info. */
    if (BCMCTH_DLB_LT_FIELD_GET(
             lt_entry->fbmp, BCMCTH_DLB_ECMP_PCTL_LT_FIELD_OVERRIDE)) {
        if (lt_entry->override) {
            BCMDRD_PBMP_PORT_ADD(override_pbmp, lt_entry->port_id);
        } else {
            BCMDRD_PBMP_PORT_REMOVE(override_pbmp, lt_entry->port_id);
        }
    }

    if (BCMCTH_DLB_LT_FIELD_GET(
             lt_entry->fbmp, BCMCTH_DLB_ECMP_PCTL_LT_FIELD_FORCE_LINK_STATUS)) {
        if (lt_entry->force_link_status) {
            BCMDRD_PBMP_PORT_ADD(force_link_status_pbmp, lt_entry->port_id);
        } else {
            BCMDRD_PBMP_PORT_REMOVE(force_link_status_pbmp, lt_entry->port_id);
        }
    }


    bcmdrd_pt_field_set(unit, DLB_ECMP_LINK_CONTROLm, buf_ptr,
                        SW_PORT_STATEf, (uint32_t *)(&force_link_status_pbmp));

    bcmdrd_pt_field_set(unit, DLB_ECMP_LINK_CONTROLm, buf_ptr,
                        SW_OVERRIDE_PORT_MAPf, (uint32_t *)(&override_pbmp));


    /* write */
    bcmcth_dlb_pt_op_info_t_init(&pt_op_info);
    pt_op_info.drd_sid = DLB_ECMP_LINK_CONTROLm;
    pt_op_info.trans_id = lt_entry->op_arg->trans_id;
    pt_op_info.req_flags =
              bcmptm_lt_entry_attrib_to_ptm_req_flags(lt_entry->op_arg->attrib);
    pt_op_info.req_lt_sid = lt_entry->glt_sid;
    pt_op_info.op = BCMPTM_OP_WRITE;

    pt_op_info.wsize = bcmdrd_pt_entry_wsize(unit, DLB_ECMP_LINK_CONTROLm);

    /* Set up hardware table index to write. */
    pt_op_info.dyn_info.index = 0;

    SHR_IF_ERR_EXIT
       (bcmcth_dlb_pt_write(unit, &pt_op_info, buf_ptr));

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56990_b0_dlb_ecmp_link_ctrl_clr(int unit, bcmcth_dlb_ecmp_pctl_t *lt_entry)
{
    bcmcth_dlb_pt_op_info_t pt_op_info;
    uint32_t *buf_ptr = NULL;
    DLB_ECMP_LINK_CONTROLm_t   link_ctrl_entry;
    bcmdrd_pbmp_t override_pbmp;
    bcmdrd_pbmp_t force_link_status_pbmp;

    SHR_FUNC_ENTER(unit);

    sal_memset(&link_ctrl_entry, 0, sizeof(link_ctrl_entry));
    sal_memset(&override_pbmp, 0, sizeof(override_pbmp));
    sal_memset(&force_link_status_pbmp, 0, sizeof(force_link_status_pbmp));

    buf_ptr = (uint32_t *)(&link_ctrl_entry);

    /* read */
    bcmcth_dlb_pt_op_info_t_init(&pt_op_info);
    pt_op_info.drd_sid = DLB_ECMP_LINK_CONTROLm;
    pt_op_info.trans_id = lt_entry->op_arg->trans_id;
    pt_op_info.req_flags =
              bcmptm_lt_entry_attrib_to_ptm_req_flags(lt_entry->op_arg->attrib);
    pt_op_info.req_lt_sid = lt_entry->glt_sid;
    pt_op_info.op = BCMPTM_OP_READ;

    pt_op_info.wsize = bcmdrd_pt_entry_wsize(unit, DLB_ECMP_LINK_CONTROLm);

    /* Set up hardware table index to write. */
    pt_op_info.dyn_info.index = 0;

    SHR_IF_ERR_EXIT
       (bcmcth_dlb_pt_read(unit, &pt_op_info, buf_ptr));

    bcmdrd_pt_field_get(unit, DLB_ECMP_LINK_CONTROLm, buf_ptr,
                        SW_PORT_STATEf, (uint32_t *)(&force_link_status_pbmp));

    bcmdrd_pt_field_get(unit, DLB_ECMP_LINK_CONTROLm, buf_ptr,
                        SW_OVERRIDE_PORT_MAPf, (uint32_t *)(&override_pbmp));

    /* Set hardware entry info. */

    BCMDRD_PBMP_PORT_REMOVE(override_pbmp, lt_entry->port_id);

    BCMDRD_PBMP_PORT_REMOVE(force_link_status_pbmp, lt_entry->port_id);

    bcmdrd_pt_field_set(unit, DLB_ECMP_LINK_CONTROLm, buf_ptr,
                        SW_PORT_STATEf, (uint32_t *)(&force_link_status_pbmp));

    bcmdrd_pt_field_set(unit, DLB_ECMP_LINK_CONTROLm, buf_ptr,
                        SW_OVERRIDE_PORT_MAPf, (uint32_t *)(&override_pbmp));


    /* write */
    bcmcth_dlb_pt_op_info_t_init(&pt_op_info);
    pt_op_info.drd_sid = DLB_ECMP_LINK_CONTROLm;
    pt_op_info.trans_id = lt_entry->op_arg->trans_id;
    pt_op_info.req_flags =
              bcmptm_lt_entry_attrib_to_ptm_req_flags(lt_entry->op_arg->attrib);
    pt_op_info.req_lt_sid = lt_entry->glt_sid;
    pt_op_info.op = BCMPTM_OP_WRITE;

    pt_op_info.wsize = bcmdrd_pt_entry_wsize(unit, DLB_ECMP_LINK_CONTROLm);

    /* Set up hardware table index to write. */
    pt_op_info.dyn_info.index = 0;

    SHR_IF_ERR_EXIT
       (bcmcth_dlb_pt_write(unit, &pt_op_info, buf_ptr));

exit:
    SHR_FUNC_EXIT();
}

static void
bcm56990_b0_dlb_enable(int unit)
{
    ING_DII_AUX_ARB_CONTROLr_t buf;
    int num_pipes, pipe;

    num_pipes = bcmdrd_pt_num_pipe_inst(unit, ING_DII_AUX_ARB_CONTROLr);
    for (pipe = 0; pipe < num_pipes; pipe++) {
        if (!bcmdrd_pt_index_valid(unit, ING_DII_AUX_ARB_CONTROLr, pipe, -1)) {
            continue;
        }

        READ_ING_DII_AUX_ARB_CONTROLr(unit, pipe, &buf);
        ING_DII_AUX_ARB_CONTROLr_DLB_HGT_256NS_REFRESH_ENABLEf_SET(buf, 1);
        WRITE_ING_DII_AUX_ARB_CONTROLr(unit, pipe, buf);
    }
    return;
}
/*******************************************************************************
 * Driver functions
 */
static int
bcm56990_b0_cth_dlb_ecmp_grp_info_recover(
    int unit, bcmcth_dlb_ecmp_entry_info_t *entry_info)
{
    bcmcth_dlb_ecmp_entry_info_t *ent_ptr;
    int mem_tbl_size;
    int total_blocks, ent_blocks;
    int i;

    SHR_FUNC_ENTER(unit);

    /*
     * For DLB_ECMP, total flowset memory size is 32K.
     * Each block size is 256.
     * Flow_set_size is an encoded number.
     * block sizes = 1 << (entry->flow_set_size - 1).
     */
    mem_tbl_size = bcmdrd_pt_index_max(unit, DLB_ECMP_FLOWSET_INST0m) + 1;
    if (mem_tbl_size < 0) {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }
    total_blocks = mem_tbl_size >> DLB_ECMP_FLOWSET_BLKSZ_SHIFT;
    sal_memset(grp_info[unit].mbmp, 0, SHR_BITALLOCSIZE(total_blocks));

    sal_memset(grp_info[unit].group_enable, 0,
               sizeof(grp_info[unit].group_enable));

    for (i = 0; i < DLB_ECMP_MAX_GROUP; i++) {
        ent_ptr = entry_info + i;
        if (ent_ptr->flow_set_size != 0) {
            ent_blocks = 1 << (ent_ptr->flow_set_size - 1);
            grp_info[unit].group_enable[i] = TRUE;
            DLB_ECMP_FLOWSET_MEMBER_BLK_USED_SET_RANGE
                (unit,
                 ent_ptr->flow_set_base_ptr,
                 ent_blocks);
        }
    }

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56990_b0_cth_dlb_ecmp_init(int unit,
                              bool warm,
                              bcmcth_dlb_ecmp_entry_info_t *entry_info)
{
    int mem_tbl_size;
    int total_blocks;

    SHR_FUNC_ENTER(unit);

    mem_tbl_size = bcmdrd_pt_index_max(unit, DLB_ECMP_FLOWSET_INST0m) + 1;
    if (mem_tbl_size < 0) {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }
    total_blocks = mem_tbl_size >> DLB_ECMP_FLOWSET_BLKSZ_SHIFT;
    grp_info[unit].mbmp = sal_alloc(SHR_BITALLOCSIZE(total_blocks),
                                    "bcmcthDlbFlowsetMemberBmp");
    if (grp_info[unit].mbmp == NULL) {
        SHR_ERR_EXIT(SHR_E_MEMORY);
    }
    sal_memset(grp_info[unit].mbmp, 0, SHR_BITALLOCSIZE(total_blocks));
    sal_memset(grp_info[unit].group_enable,
               0, sizeof(grp_info[unit].group_enable));

    if (!warm) {
        (void)bcm56990_b0_dlb_enable(unit);
    } else {
        SHR_IF_ERR_EXIT
            (bcm56990_b0_cth_dlb_ecmp_grp_info_recover(unit, entry_info));
    }

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56990_b0_cth_dlb_ecmp_cleanup(int unit)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(grp_info[unit].mbmp, SHR_E_INTERNAL);

    sal_free(grp_info[unit].mbmp);
    grp_info[unit].mbmp = NULL;

    sal_memset(grp_info[unit].group_enable,
               0, sizeof(grp_info[unit].group_enable));

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56990_b0_cth_dlb_grp_cnt_get(int unit, int *grp_cnt)
{
    SHR_FUNC_ENTER(unit);

    if (grp_cnt) {
        *grp_cnt = bcmdrd_pt_index_max(unit, DLB_ECMP_GROUP_CONTROLm)
                   - bcmdrd_pt_index_min(unit, DLB_ECMP_GROUP_CONTROLm)
                   + 1;
    }

    SHR_FUNC_EXIT();
}

static int
bcm56990_b0_cth_dlb_ecmp_set(int unit,
                             bcmcth_dlb_ecmp_t *lt_entry,
                             bool flow_set_init)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(lt_entry, SHR_E_PARAM);
    COMPILER_REFERENCE(flow_set_init);

    SHR_IF_ERR_EXIT
        (bcm56990_b0_dlb_ecmp_flow_set_init(unit, lt_entry));

    SHR_IF_ERR_EXIT
        (bcm56990_b0_dlb_ecmp_nhi_member_set(unit, lt_entry));

    SHR_IF_ERR_EXIT
        (bcm56990_b0_dlb_ecmp_port_member_set(unit, lt_entry));

    SHR_IF_ERR_EXIT
        (bcm56990_b0_dlb_ecmp_alternate_nhi_member_set(unit, lt_entry));

    SHR_IF_ERR_EXIT
        (bcm56990_b0_dlb_ecmp_alternate_port_member_set(unit, lt_entry));

    SHR_IF_ERR_EXIT
        (bcm56990_b0_dlb_ecmp_port_bitmap_set(unit, lt_entry));

    SHR_IF_ERR_EXIT
        (bcm56990_b0_dlb_ecmp_grp_ctrl_set(unit, lt_entry));

    SHR_IF_ERR_EXIT
        (bcm56990_b0_dlb_ecmp_enable(unit, lt_entry, true));

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56990_b0_cth_dlb_ecmp_del(int unit,
                             bcmcth_dlb_ecmp_t *lt_entry)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(lt_entry, SHR_E_PARAM);

    SHR_IF_ERR_EXIT
        (bcm56990_b0_dlb_ecmp_enable(unit, lt_entry, false));

    SHR_IF_ERR_EXIT
        (bcm56990_b0_dlb_ecmp_nhi_member_clr(unit, lt_entry));

    SHR_IF_ERR_EXIT
        (bcm56990_b0_dlb_ecmp_port_member_clr(unit, lt_entry));

    SHR_IF_ERR_EXIT
        (bcm56990_b0_dlb_ecmp_alternate_nhi_member_clr(unit, lt_entry));

    SHR_IF_ERR_EXIT
        (bcm56990_b0_dlb_ecmp_alternate_port_member_clr(unit, lt_entry));

    SHR_IF_ERR_EXIT
        (bcm56990_b0_dlb_ecmp_port_bitmap_clr(unit, lt_entry));

    SHR_IF_ERR_EXIT
        (bcm56990_b0_dlb_ecmp_grp_ctrl_clr(unit, lt_entry));

    SHR_IF_ERR_EXIT
        (bcm56990_b0_dlb_ecmp_flow_set_cleanup(unit, lt_entry));

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56990_b0_cth_dlb_ecmp_pctl_set(int unit,
                                  bcmcth_dlb_ecmp_pctl_t *lt_entry)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(lt_entry, SHR_E_PARAM);

    SHR_IF_ERR_EXIT
        (bcm56990_b0_dlb_ecmp_link_ctrl_set(unit, lt_entry));

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56990_b0_cth_dlb_ecmp_pctl_del(int unit,
                                  bcmcth_dlb_ecmp_pctl_t *lt_entry)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(lt_entry, SHR_E_PARAM);

    SHR_IF_ERR_EXIT
        (bcm56990_b0_dlb_ecmp_link_ctrl_clr(unit, lt_entry));

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56990_b0_cth_dlb_ecmp_stats_set(int unit,
                                   bcmcth_dlb_ecmp_stats_t *lt_entry)
{
    DLB_ECMP_GROUP_STATS_INST0m_t dlb_ecmp_group_stats_buf;
    bcmdrd_sid_t dlb_ecmp_group_stats_sid[2] = {DLB_ECMP_GROUP_STATS_INST0m,
                                                DLB_ECMP_GROUP_STATS_INST1m};
    bcmbd_pt_dyn_info_t pt_dyn_info;
    bcmltd_sid_t rsp_ltid = 0;
    uint32_t fval = 0, rsp_flags = 0;
    int inst_num, inst_idx, sid_idx;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(lt_entry, SHR_E_PARAM);

    if (lt_entry->fail_cnt > DLB_ECMP_MAX_COUNT ||
        lt_entry->port_reassignment_cnt > DLB_ECMP_MAX_COUNT ||
        lt_entry->member_reassignment_cnt > DLB_ECMP_MAX_COUNT) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    sal_memset(&dlb_ecmp_group_stats_buf, 0, sizeof(dlb_ecmp_group_stats_buf));

    inst_num = bcmdrd_pt_num_tbl_inst(unit, DLB_ECMP_GROUP_STATS_INST0m);

    fval = lt_entry->fail_cnt;
    DLB_ECMP_GROUP_STATS_INST0m_DLB_DROPSf_SET(dlb_ecmp_group_stats_buf, fval);

    fval = lt_entry->port_reassignment_cnt;
    DLB_ECMP_GROUP_STATS_INST0m_PORT_REASSIGNMENTSf_SET
        (dlb_ecmp_group_stats_buf, fval);

    fval = lt_entry->member_reassignment_cnt;
    DLB_ECMP_GROUP_STATS_INST0m_MEMBER_REASSIGNMENTSf_SET
        (dlb_ecmp_group_stats_buf, fval);

    pt_dyn_info.index = lt_entry->dlb_id;
    for (sid_idx = 0; sid_idx < COUNTOF(dlb_ecmp_group_stats_sid); sid_idx++) {
        for (inst_idx = 0; inst_idx < inst_num; inst_idx++) {
            if (!bcmdrd_pt_index_valid(unit, dlb_ecmp_group_stats_sid[sid_idx],
                                       inst_idx, -1)) {
                continue;
            }

            pt_dyn_info.tbl_inst = inst_idx;

            SHR_IF_ERR_EXIT
                (bcmptm_ltm_ireq_write(unit,
                                       BCMPTM_REQ_FLAGS_NO_FLAGS,
                                       dlb_ecmp_group_stats_sid[sid_idx],
                                       &pt_dyn_info,
                                       NULL,
                                       (uint32_t *)&dlb_ecmp_group_stats_buf,
                                       DLB_ECMP_STATSt,
                                       &rsp_ltid,
                                       &rsp_flags));
        }
    }

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56990_b0_cth_dlb_ecmp_stats_get(int unit,
                                   bcmcth_dlb_ecmp_stats_t *lt_entry)
{
    DLB_ECMP_GROUP_STATS_INST0m_t dlb_ecmp_group_stats_buf;
    bcmdrd_sid_t dlb_ecmp_group_stats_sid[2] = {DLB_ECMP_GROUP_STATS_INST0m,
                                                DLB_ECMP_GROUP_STATS_INST1m};
    bcmbd_pt_dyn_info_t pt_dyn_info;
    bcmltd_sid_t rsp_ltid = 0;
    uint32_t fval = 0, rsp_entry_wsize = 0, rsp_flags = 0;
    int inst_num, inst_idx, sid_idx;
    uint64_t req_flags;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(lt_entry, SHR_E_PARAM);
    lt_entry->fail_cnt = 0;
    lt_entry->port_reassignment_cnt = 0;
    lt_entry->member_reassignment_cnt = 0;

    sal_memset(&dlb_ecmp_group_stats_buf, 0, sizeof(dlb_ecmp_group_stats_buf));

    inst_num = bcmdrd_pt_num_tbl_inst(unit, DLB_ECMP_GROUP_STATS_INST0m);
    req_flags =
        bcmptm_lt_entry_attrib_to_ptm_req_flags(BCMLT_ENT_ATTR_GET_FROM_HW);

    pt_dyn_info.index = lt_entry->dlb_id;
    for (sid_idx = 0; sid_idx < COUNTOF(dlb_ecmp_group_stats_sid); sid_idx++) {
        for (inst_idx = 0; inst_idx < inst_num; inst_idx++) {
            if (!bcmdrd_pt_index_valid(unit, dlb_ecmp_group_stats_sid[sid_idx],
                                       inst_idx, -1)) {
                continue;
            }

            pt_dyn_info.tbl_inst = inst_idx;
            rsp_entry_wsize = bcmdrd_pt_entry_wsize(unit,
                                            dlb_ecmp_group_stats_sid[sid_idx]);

            SHR_IF_ERR_EXIT
                (bcmptm_ltm_ireq_read(unit,
                                      req_flags,
                                      dlb_ecmp_group_stats_sid[sid_idx],
                                      &pt_dyn_info,
                                      NULL,
                                      rsp_entry_wsize,
                                      DLB_ECMP_STATSt,
                                      (uint32_t *)&dlb_ecmp_group_stats_buf,
                                      &rsp_ltid,
                                      &rsp_flags));

            fval = DLB_ECMP_GROUP_STATS_INST0m_DLB_DROPSf_GET
                       (dlb_ecmp_group_stats_buf);
            lt_entry->fail_cnt += fval;

            fval = DLB_ECMP_GROUP_STATS_INST0m_PORT_REASSIGNMENTSf_GET
                       (dlb_ecmp_group_stats_buf);
            lt_entry->port_reassignment_cnt += fval;

            fval = DLB_ECMP_GROUP_STATS_INST0m_MEMBER_REASSIGNMENTSf_GET
                       (dlb_ecmp_group_stats_buf);
            lt_entry->member_reassignment_cnt += fval;
        }
    }

exit:
    SHR_FUNC_EXIT();
}

static bcmcth_dlb_drv_t bcm56990_b0_cth_dlb_drv = {
    .ecmp_init = bcm56990_b0_cth_dlb_ecmp_init,
    .ecmp_cleanup = bcm56990_b0_cth_dlb_ecmp_cleanup,
    .grp_cnt_get = bcm56990_b0_cth_dlb_grp_cnt_get,
    .ecmp_set = bcm56990_b0_cth_dlb_ecmp_set,
    .ecmp_del = bcm56990_b0_cth_dlb_ecmp_del,
    .ecmp_pctl_set = bcm56990_b0_cth_dlb_ecmp_pctl_set,
    .ecmp_pctl_del = bcm56990_b0_cth_dlb_ecmp_pctl_del,
    .ecmp_recover = bcm56990_b0_cth_dlb_ecmp_grp_info_recover,
    .ecmp_stats_set = bcm56990_b0_cth_dlb_ecmp_stats_set,
    .ecmp_stats_get = bcm56990_b0_cth_dlb_ecmp_stats_get,
};

/*******************************************************************************
 * Public Functions
 */

bcmcth_dlb_drv_t *
bcm56990_b0_cth_dlb_drv_get(int unit)
{
    return &bcm56990_b0_cth_dlb_drv;
}
