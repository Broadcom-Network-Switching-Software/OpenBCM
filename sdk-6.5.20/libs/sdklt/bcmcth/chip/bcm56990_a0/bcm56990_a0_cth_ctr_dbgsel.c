/*! \file bcm56990_a0_cth_ctr_dbgsel.c
 *
 * BCM56990 A0 device specific ING/EGR Debug Counter Trigger Selection Driver.
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
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmcth/bcmcth_ctr_dbgsel_util.h>
#include <bcmcth/bcmcth_ctr_dbgsel_drv.h>
#include <bcmdrd/chip/bcm56990_a0_enum.h>
#include <bcmdrd/chip/bcm56990_a0_defs.h>

/*******************************************************************************
 * Local definitions
 */
#define BSL_LOG_MODULE BSL_LS_BCMCTH_CTRDBGSEL

#define UINT_64_HI(src)         ((uint32_t) ((src) >> 32))
#define UINT_64_LO(src)         ((uint32_t) (src))

static bcmdrd_sid_t rdbgcx_regs[9] = {
    RDBGC0_SELECTr, RDBGC1_SELECTr, RDBGC2_SELECTr, RDBGC3_SELECTr,
    RDBGC4_SELECTr, RDBGC5_SELECTr, RDBGC6_SELECTr, RDBGC7_SELECTr,
    RDBGC8_SELECTr
};

static bcmdrd_sid_t tdbgcx_regs[12] = {
    TDBGC0_SELECTr, TDBGC1_SELECTr, TDBGC2_SELECTr, TDBGC3_SELECTr,
    TDBGC4_SELECTr, TDBGC5_SELECTr, TDBGC6_SELECTr, TDBGC7_SELECTr,
    TDBGC8_SELECTr, TDBGC9_SELECTr, TDBGC10_SELECTr, TDBGC11_SELECTr
};

static void
bcm56990_a0_ctr_dbgsel_enum2bit_map_init(bool ingress, int *enum2bit)
{
    if (ingress) {
        enum2bit[CTR_ING_DBG_SEL_VLAN_MEMBERSHIP_DROP] = 106;
        enum2bit[CTR_ING_DBG_SEL_VLAN_BLOCKED_DROP] = 105;
        enum2bit[CTR_ING_DBG_SEL_VLAN_STG_DROP] = 104;
        enum2bit[CTR_ING_DBG_SEL_NONUC_TRUNK_RESOLUTION_MASK] = 103;
        enum2bit[CTR_ING_DBG_SEL_NONUC_MASK] = 102;
        enum2bit[CTR_ING_DBG_SEL_MC_DROP] = 101;
        enum2bit[CTR_ING_DBG_SEL_MAC_MASK] = 100;
        enum2bit[CTR_ING_DBG_SEL_ING_EGR_MASK] = 99;
        enum2bit[CTR_ING_DBG_SEL_FP_ING_DELAYED_DROP] = 98;
        enum2bit[CTR_ING_DBG_SEL_FP_REDIRECT_MASK] = 97;
        enum2bit[CTR_ING_DBG_SEL_EGR_MASK] = 96;
        enum2bit[CTR_ING_DBG_SEL_SRC_PORT_KNOCKOUT_DROP] = 91;
        enum2bit[CTR_ING_DBG_SEL_RX_UC] = 90;
        enum2bit[CTR_ING_DBG_SEL_RX_TNL_ERR] = 89;
        enum2bit[CTR_ING_DBG_SEL_RX_TNL] = 88;
        enum2bit[CTR_ING_DBG_SEL_RX_PORT_DROP] = 87;
        enum2bit[CTR_ING_DBG_SEL_RX_MC_DROP] = 86;
        enum2bit[CTR_ING_DBG_SEL_RX_DROP] = 85;
        enum2bit[CTR_ING_DBG_SEL_NON_IP_DISCARD] = 84;
        enum2bit[CTR_ING_DBG_SEL_PARITY_ERR_DROP] = 83;
        enum2bit[CTR_ING_DBG_SEL_MTU_CHECK_FAIL] = 82;
        enum2bit[CTR_ING_DBG_SEL_TRUNK_FAILOVER_LOOPBACK_DISCARD] = 81;
        enum2bit[CTR_ING_DBG_SEL_TRUNK_FAILOVER_LOOPBACK] = 80;
        enum2bit[CTR_ING_DBG_SEL_MC_BRIDGED] = 79;
        enum2bit[CTR_ING_DBG_SEL_DOS_L2] = 78;
        enum2bit[CTR_ING_DBG_SEL_DOS_ICMP] = 77;
        enum2bit[CTR_ING_DBG_SEL_DOS_FRAGMENT] = 76;
        enum2bit[CTR_ING_DBG_SEL_BLOCK_MASK_DROP] = 75;
        enum2bit[CTR_ING_DBG_SEL_BFD_UNKNOWN_VER_OR_DISCARD] = 74;
        enum2bit[CTR_ING_DBG_SEL_BFD_UNKNOWN_CTRL_PKT] = 73;
        enum2bit[CTR_ING_DBG_SEL_BFD_UNKNOWN_ACH_ERR] = 72;
        enum2bit[CTR_ING_DBG_SEL_IPV6_MC_ROUTED] = 71;
        enum2bit[CTR_ING_DBG_SEL_RX_IPV6_HDR_ERR] = 70;
        enum2bit[CTR_ING_DBG_SEL_RX_IPV6] = 69;
        enum2bit[CTR_ING_DBG_SEL_RX_IPV6_DISCARD] = 68;
        enum2bit[CTR_ING_DBG_SEL_IPV4_MC_ROUTED] = 67;
        enum2bit[CTR_ING_DBG_SEL_RX_IPV4_HDR_ERR] = 66;
        enum2bit[CTR_ING_DBG_SEL_RX_IPV4] = 65;
        enum2bit[CTR_ING_DBG_SEL_RX_IPV4_DISCARD] = 64;
        enum2bit[CTR_ING_DBG_SEL_PVLAN_VID_MISMATCH] = 60;
        enum2bit[CTR_ING_DBG_SEL_INVALID_VLAN] = 59;
        enum2bit[CTR_ING_DBG_SEL_INVALID_TPID] = 58;
        enum2bit[CTR_ING_DBG_SEL_PORT_ING_VLAN_MEMBERSHIP] = 57;
        enum2bit[CTR_ING_DBG_SEL_VLAN_CC_OR_PBT] = 56;
        enum2bit[CTR_ING_DBG_SEL_VLAN_FP] = 55;
        enum2bit[CTR_ING_DBG_SEL_MPLS_TTL_CHECK_FAIL] = 51;
        enum2bit[CTR_ING_DBG_SEL_MPLS_LABEL_MISS] = 50;
        enum2bit[CTR_ING_DBG_SEL_MPLS_INVALID_PAYLOAD] = 49;
        enum2bit[CTR_ING_DBG_SEL_MPLS_INVALID_CW] = 48;
        enum2bit[CTR_ING_DBG_SEL_MPLS_INVALID_ACTION] = 47;
        enum2bit[CTR_ING_DBG_SEL_MPLS_GAL_LABEL] = 46;
        enum2bit[CTR_ING_DBG_SEL_TNL_ERROR] = 45;
        enum2bit[CTR_ING_DBG_SEL_TNL_DECAP_ECN] = 44;
        enum2bit[CTR_ING_DBG_SEL_TIME_SYNC_PKT] = 43;
        enum2bit[CTR_ING_DBG_SEL_TAG_UNTAG_DISCARD] = 42;
        enum2bit[CTR_ING_DBG_SEL_SRC_ROUTE] = 39;
        enum2bit[CTR_ING_DBG_SEL_SPANNING_TREE_STATE] = 38;
        enum2bit[CTR_ING_DBG_SEL_COMPOSITE_ERROR] = 37;
        enum2bit[CTR_ING_DBG_SEL_PROTECTION_DATA_DROP] = 35;
        enum2bit[CTR_ING_DBG_SEL_PROTCOL_PKT_CTRL_DROP] = 34;
        enum2bit[CTR_ING_DBG_SEL_PORT_FILTERING_MODE] = 33;
        enum2bit[CTR_ING_DBG_SEL_NHOP_DROP] = 32;
        enum2bit[CTR_ING_DBG_SEL_L2_MY_STATION] = 31;
        enum2bit[CTR_ING_DBG_SEL_MC_INDEX_ERR] = 30;
        enum2bit[CTR_ING_DBG_SEL_MPLS_STAGE] = 29;
        enum2bit[CTR_ING_DBG_SEL_SRC_MAC_ZERO] = 28;
        enum2bit[CTR_ING_DBG_SEL_L3_TTL_ERR] = 27;
        enum2bit[CTR_ING_DBG_SEL_L3_HDR_ERR] = 26;
        enum2bit[CTR_ING_DBG_SEL_DST_L3_LOOKUP_MISS] = 25;
        enum2bit[CTR_ING_DBG_SEL_SRC_L3_DISCARD] = 23;
        enum2bit[CTR_ING_DBG_SEL_DST_L3_DISCARD] = 23;
        enum2bit[CTR_ING_DBG_SEL_SRC_L2_STATIC_MOVE] = 22;
        enum2bit[CTR_ING_DBG_SEL_SRC_L2_DISCARD] = 21;
        enum2bit[CTR_ING_DBG_SEL_DST_L2_DISCARD] = 20;
        enum2bit[CTR_ING_DBG_SEL_IP_MC_DROP] = 19;
        enum2bit[CTR_ING_DBG_SEL_INT_VECTOR_MISS_MATCH_DROP] = 16;
        enum2bit[CTR_ING_DBG_SEL_INT_DP_EXCEPTION_DROP] = 15;
        enum2bit[CTR_ING_DBG_SEL_FP_ING_DROP] = 14;
        enum2bit[CTR_ING_DBG_SEL_FP_REDIRECT_DROP] = 13;
        enum2bit[CTR_ING_DBG_SEL_ECMP_RESOLUTION_ERR] = 12;
        enum2bit[CTR_ING_DBG_SEL_SRC_MAC_EQUALS_DST_MAC] = 10;
        enum2bit[CTR_ING_DBG_SEL_TRUNK_FAIL_LOOPBACK] = 9;
        enum2bit[CTR_ING_DBG_SEL_DOS_L4_ATTACK] = 8;
        enum2bit[CTR_ING_DBG_SEL_DOS_L3_ATTACK] = 7;
        enum2bit[CTR_ING_DBG_SEL_IPV6_PROTOCOL_ERR] = 6;
        enum2bit[CTR_ING_DBG_SEL_IPV4_PROTOCOL_ERR] = 5;
        enum2bit[CTR_ING_DBG_SEL_MAC_CONTROL_FRAME] = 4;
        enum2bit[CTR_ING_DBG_SEL_CPU_MANAGED_LEARNING] = 3;
        enum2bit[CTR_ING_DBG_SEL_CFI_OR_L3_DISABLE] = 2;
        enum2bit[CTR_ING_DBG_SEL_BPDU] = 1;
        enum2bit[CTR_ING_DBG_SEL_BFD_TERMINATED_DROP] = 0;
    } else { /* egress */
        enum2bit[CTR_EGR_DBG_SEL_PKT_TOO_SMALL] = 31;
        enum2bit[CTR_EGR_DBG_SEL_INVALID_1588_PKT] = 30;
        enum2bit[CTR_EGR_DBG_SEL_LB_TO_LB_DROP] = 29;
        enum2bit[CTR_EGR_DBG_SEL_PKT_TOO_LARGE] = 28;
        enum2bit[CTR_EGR_DBG_SEL_MODID_GT_31] = 27;
        enum2bit[CTR_EGR_DBG_SEL_IPMC_L3_SELF_INTF] = 26;
        enum2bit[CTR_EGR_DBG_SEL_PARITY_ERR] = 25;
        enum2bit[CTR_EGR_DBG_SEL_L2_MTU_FAIL] = 24;
        enum2bit[CTR_EGR_DBG_SEL_CELL_TOO_SMALL] = 23;
        enum2bit[CTR_EGR_DBG_SEL_HIGIG_MC] = 22;
        enum2bit[CTR_EGR_DBG_SEL_HIGIG_UC] = 21;
        enum2bit[CTR_EGR_DBG_SEL_SRC_IP_LINK_LOCAL] = 20;
        enum2bit[CTR_EGR_DBG_SEL_MIRROR] = 19;
        enum2bit[CTR_EGR_DBG_SEL_PKT_DROP] = 18;
        enum2bit[CTR_EGR_DBG_SEL_L2_MC_DROP] = 17;
        enum2bit[CTR_EGR_DBG_SEL_STG_DROP] = 15;
        enum2bit[CTR_EGR_DBG_SEL_VLAN_XLATE_MISS] = 14;
        enum2bit[CTR_EGR_DBG_SEL_INVALID_VLAN] = 13;
        enum2bit[CTR_EGR_DBG_SEL_VLAN_TAGGED] = 12;
        enum2bit[CTR_EGR_DBG_SEL_CFI_DROP] = 11;
        enum2bit[CTR_EGR_DBG_SEL_TTL_DROP] = 10;
        enum2bit[CTR_EGR_DBG_SEL_TNL_ERR] = 9;
        enum2bit[CTR_EGR_DBG_SEL_TNL_PKT] = 8;
        enum2bit[CTR_EGR_DBG_SEL_IPMCV6_DROP] = 7;
        enum2bit[CTR_EGR_DBG_SEL_IPMCV6_PKT] = 6;
        enum2bit[CTR_EGR_DBG_SEL_IPV6_DROP] = 5;
        enum2bit[CTR_EGR_DBG_SEL_IPV6_PKT] = 4;
        enum2bit[CTR_EGR_DBG_SEL_IPMCV4_DROP] = 3;
        enum2bit[CTR_EGR_DBG_SEL_IPMCV4_PKT] = 2;
        enum2bit[CTR_EGR_DBG_SEL_IPV4_DROP] = 1;
        enum2bit[CTR_EGR_DBG_SEL_IPV4_PKT] = 0;
    }
}

static int
bcm56990_a0_ctr_ing_dbgsel_set(int unit, int op, uint8_t ridx, int *enum2bit,
                               ctr_dbgsel_trigger_t *trigger_array)
{
    RDBGC_SELECT_4r_t select_4;
    RDBGC_SELECT_3r_t select_3;
    RDBGC_SELECT_2r_t select_2;
    RDBGC0_SELECTr_t select0;
    uint64_t value[2] = {0};
    uint32_t reg_value[4] = {0};

    SHR_FUNC_ENTER(unit);

    RDBGC_SELECT_4r_CLR(select_4);
    RDBGC_SELECT_3r_CLR(select_3);
    RDBGC_SELECT_2r_CLR(select_2);
    RDBGC0_SELECTr_CLR(select0);

    if (op == CTR_DBGSEL_UPDATE) {
        /* Read the current reg value. */
        SHR_IF_ERR_EXIT(
            bcmcth_ctr_dbgsel_ireq_read(unit, CTR_ING_DEBUG_SELECTt,
                                        RDBGC_SELECT_4r, ridx,
                                        &RDBGC_SELECT_4r_GET(select_4)));

        SHR_IF_ERR_EXIT(
            bcmcth_ctr_dbgsel_ireq_read(unit, CTR_ING_DEBUG_SELECTt,
                                        RDBGC_SELECT_3r, ridx,
                                        &RDBGC_SELECT_3r_GET(select_3)));

        SHR_IF_ERR_EXIT(
            bcmcth_ctr_dbgsel_ireq_read(unit, CTR_ING_DEBUG_SELECTt,
                                        RDBGC_SELECT_2r, ridx,
                                        &RDBGC_SELECT_2r_GET(select_2)));

        SHR_IF_ERR_EXIT(
            bcmcth_ctr_dbgsel_ireq_read(unit, CTR_ING_DEBUG_SELECTt,
                                        rdbgcx_regs[ridx], 0,
                                        &RDBGC0_SELECTr_GET(select0)));

        reg_value[3] = RDBGC_SELECT_4r_BITMAPf_GET(select_4);
        reg_value[2] = RDBGC_SELECT_3r_BITMAPf_GET(select_3);
        reg_value[1] = RDBGC_SELECT_2r_BITMAPf_GET(select_2);
        reg_value[0] = RDBGC0_SELECTr_BITMAPf_GET(select0);

        bcmcth_ctr_dbgsel_bitmap_update(3, enum2bit, trigger_array, reg_value+3);
        bcmcth_ctr_dbgsel_bitmap_update(2, enum2bit, trigger_array, reg_value+2);
        bcmcth_ctr_dbgsel_bitmap_update(1, enum2bit, trigger_array, reg_value+1);
        bcmcth_ctr_dbgsel_bitmap_update(0, enum2bit, trigger_array, reg_value);

        /* Update reg value. */
        RDBGC_SELECT_4r_BITMAPf_SET(select_4, reg_value[3]);
        RDBGC_SELECT_3r_BITMAPf_SET(select_3, reg_value[2]);
        RDBGC_SELECT_2r_BITMAPf_SET(select_2, reg_value[1]);
        RDBGC0_SELECTr_BITMAPf_SET(select0, reg_value[0]);
    } else {
        if (op == CTR_DBGSEL_INSERT) {
            bcmcth_ctr_dbgsel_bitmap_convert(enum2bit, trigger_array, value);
        }
        /* Set reg value. */
        RDBGC_SELECT_4r_BITMAPf_SET(select_4, UINT_64_HI(*(value+1)));
        RDBGC_SELECT_3r_BITMAPf_SET(select_3, UINT_64_LO(*(value+1)));
        RDBGC_SELECT_2r_BITMAPf_SET(select_2, UINT_64_HI(*value));
        RDBGC0_SELECTr_BITMAPf_SET(select0, UINT_64_LO(*value));
    }

    /* Write the updated reg value. */
    SHR_IF_ERR_EXIT(
        bcmcth_ctr_dbgsel_ireq_write(unit, CTR_ING_DEBUG_SELECTt,
                                     RDBGC_SELECT_4r, ridx,
                                     &RDBGC_SELECT_4r_GET(select_4)));

    SHR_IF_ERR_EXIT(
        bcmcth_ctr_dbgsel_ireq_write(unit, CTR_ING_DEBUG_SELECTt,
                                     RDBGC_SELECT_3r, ridx,
                                     &RDBGC_SELECT_3r_GET(select_3)));

    SHR_IF_ERR_EXIT(
        bcmcth_ctr_dbgsel_ireq_write(unit, CTR_ING_DEBUG_SELECTt,
                                     RDBGC_SELECT_2r, ridx,
                                     &RDBGC_SELECT_2r_GET(select_2)));

    SHR_IF_ERR_EXIT(
        bcmcth_ctr_dbgsel_ireq_write(unit, CTR_ING_DEBUG_SELECTt,
                                     rdbgcx_regs[ridx], 0,
                                     &RDBGC0_SELECTr_GET(select0)));

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56990_a0_ctr_egr_dbgsel_set(int unit, int op, uint8_t ridx, int *enum2bit,
                               ctr_dbgsel_trigger_t *trigger_array)
{
    TDBGC_SELECT_2r_t select_2;
    TDBGC0_SELECTr_t select0;
    uint64_t value[2] = {0};
    uint32_t reg_value[2] = {0};

    SHR_FUNC_ENTER(unit);

    TDBGC_SELECT_2r_CLR(select_2);
    TDBGC0_SELECTr_CLR(select0);

    if (op == CTR_DBGSEL_UPDATE) {
        /* Read the current reg value. */
        SHR_IF_ERR_EXIT(
            bcmcth_ctr_dbgsel_ireq_read(unit, CTR_EGR_DEBUG_SELECTt,
                                        TDBGC_SELECT_2r, ridx,
                                        &TDBGC_SELECT_2r_GET(select_2)));

        SHR_IF_ERR_EXIT(
            bcmcth_ctr_dbgsel_ireq_read(unit, CTR_EGR_DEBUG_SELECTt,
                                        tdbgcx_regs[ridx], 0,
                                        &TDBGC0_SELECTr_GET(select0)));
        reg_value[1] = TDBGC_SELECT_2r_BITMAPf_GET(select_2);
        reg_value[0] = TDBGC0_SELECTr_BITMAPf_GET(select0);

        bcmcth_ctr_dbgsel_bitmap_update(1, enum2bit, trigger_array, reg_value+1);
        bcmcth_ctr_dbgsel_bitmap_update(0, enum2bit, trigger_array, reg_value);

        /* Update reg value. */
        TDBGC_SELECT_2r_BITMAPf_SET(select_2, reg_value[1]);
        TDBGC0_SELECTr_BITMAPf_SET(select0, reg_value[0]);
    } else {
        if (op == CTR_DBGSEL_INSERT) {
            bcmcth_ctr_dbgsel_bitmap_convert(enum2bit, trigger_array, value);
        }
        /* Set reg value. */
        TDBGC_SELECT_2r_BITMAPf_SET(select_2, UINT_64_HI(*value));
        TDBGC0_SELECTr_BITMAPf_SET(select0, UINT_64_LO(*value));
    }

    /* Write the updated reg value. */
    SHR_IF_ERR_EXIT(
        bcmcth_ctr_dbgsel_ireq_write(unit, CTR_EGR_DEBUG_SELECTt,
                                     TDBGC_SELECT_2r, ridx,
                                     &TDBGC_SELECT_2r_GET(select_2)));

    SHR_IF_ERR_EXIT(
        bcmcth_ctr_dbgsel_ireq_write(unit, CTR_EGR_DEBUG_SELECTt,
                                     tdbgcx_regs[ridx], 0,
                                     &TDBGC0_SELECTr_GET(select0)));

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56990_a0_ctr_dbgsel_set(int unit,
                           bool ingress,
                           int op,
                           uint8_t idx,
                           ctr_dbgsel_trigger_t *trigger_array)
{
    int enum2bit[CTR_DBG_SEL_NUM];

    SHR_FUNC_ENTER(unit);

    bcm56990_a0_ctr_dbgsel_enum2bit_map_init(ingress, enum2bit);

    if (ingress) {
        SHR_IF_ERR_EXIT
            (bcm56990_a0_ctr_ing_dbgsel_set(unit, op, idx, enum2bit,
                                            trigger_array));
    } else {
        SHR_IF_ERR_EXIT
            (bcm56990_a0_ctr_egr_dbgsel_set(unit, op, idx, enum2bit,
                                            trigger_array));
    }

exit:
    SHR_FUNC_EXIT();
}

static bcmcth_ctr_dbgsel_drv_t bcm56990_a0_ctr_dbgsel_drv = {
   .dbgsel_set = &bcm56990_a0_ctr_dbgsel_set,
};

/*******************************************************************************
 * Public functions
 */
bcmcth_ctr_dbgsel_drv_t *
bcm56990_a0_cth_ctr_dbgsel_drv_get(int unit)
{
    return &bcm56990_a0_ctr_dbgsel_drv;
}
