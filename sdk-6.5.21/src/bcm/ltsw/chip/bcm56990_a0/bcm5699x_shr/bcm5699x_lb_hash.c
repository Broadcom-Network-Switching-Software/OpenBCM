/*! \file bcm5699x_lb_hash.c
 *
 * BCM5699x LB Hash subordinate driver.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bcm_int/ltsw/mbcm/lb_hash.h>
#include <bcm_int/ltsw/xgs/lb_hash.h>
#include <bcm_int/ltsw/dev.h>

#include <bcmltd/chip/bcmltd_str.h>
#include <shr/shr_debug.h>

/******************************************************************************
 * Local definitions
 */

#define BSL_LOG_MODULE BSL_LS_BCM_HASH

/******************************************************************************
 * Private functions
 */

static const bcmint_ltsw_lb_hash_os_tbl_t lb_hash_os_tbls[bcmiLbHosTypeCount] = {
    [bcmiLbHosPortTypeDlbEcmp] = {
        .valid = 1,
        .flow_based = 0,
        .name = LB_HASH_PORT_DLB_ECMP_OUTPUT_SELECTIONs,
        .key_name = PORT_IDs,
        .offset_name = OFFSETs,
        .sub_sel_name = SUBSET_SELECTs,
        .concat_name = CONCATs,
    },
    [bcmiLbHosPortTypeEcmp] = {
        .valid = 1,
        .flow_based = 0,
        .name = LB_HASH_PORT_L3_ECMP_OUTPUT_SELECTIONs,
        .key_name = PORT_IDs,
        .offset_name = OFFSETs,
        .sub_sel_name = SUBSET_SELECTs,
        .concat_name = CONCATs,
    },
    [bcmiLbHosPortTypeEntropy] = {
        .valid = 1,
        .flow_based = 0,
        .name = LB_HASH_PORT_ENTROPY_LABEL_OUTPUT_SELECTIONs,
        .key_name = PORT_IDs,
        .offset_name = OFFSETs,
        .sub_sel_name = SUBSET_SELECTs,
        .concat_name = CONCATs,
    },
    [bcmiLbHosPortTypeUcTrunk] = {
        .valid = 1,
        .flow_based = 0,
        .name = LB_HASH_PORT_TRUNK_OUTPUT_SELECTIONs,
        .key_name = PORT_IDs,
        .offset_name = UC_OFFSETs,
        .sub_sel_name = UC_SUBSET_SELECTs,
        .concat_name = UC_CONCATs,
    },
    [bcmiLbHosPortTypeNonUcTrunk] = {
        .valid = 1,
        .flow_based = 0,
        .name = LB_HASH_PORT_TRUNK_OUTPUT_SELECTIONs,
        .key_name = PORT_IDs,
        .offset_name = NONUC_OFFSETs,
        .sub_sel_name = NONUC_SUBSET_SELECTs,
        .concat_name = NONUC_CONCATs,
    },
    [bcmiLbHosPortTypeMplsEcmp] = {
        .valid = 1,
        .flow_based = 0,
        .name = LB_HASH_PORT_MPLS_ECMP_OUTPUT_SELECTIONs,
        .key_name = PORT_IDs,
        .offset_name = OFFSETs,
        .sub_sel_name = SUBSET_SELECTs,
        .concat_name = CONCATs,
    },
    [bcmiLbHosPortTypePlfs] = {
        .valid = 1,
        .flow_based = 0,
        .name = LB_HASH_PORT_PLFS_OUTPUT_SELECTIONs,
        .key_name = PORT_IDs,
        .offset_name = OFFSETs,
        .sub_sel_name = SUBSET_SELECTs,
        .concat_name = CONCATs,
    },
    [bcmiLbHosPortTypeRhEcmp] = {
        .valid = 1,
        .flow_based = 0,
        .name = LB_HASH_PORT_RH_ECMP_OUTPUT_SELECTIONs,
        .key_name = PORT_IDs,
        .offset_name = OFFSETs,
        .sub_sel_name = SUBSET_SELECTs,
        .concat_name = CONCATs,
    },
    [bcmiLbHosPortTypeOverlayRhEcmp] = {
        .valid = 1,
        .flow_based = 0,
        .name = LB_HASH_PORT_RH_ECMP_OUTPUT_SELECTIONs,
        .key_name = PORT_IDs,
        .offset_name = OVERLAY_OFFSETs,
        .sub_sel_name = OVERLAY_SUBSET_SELECTs,
        .concat_name = OVERLAY_CONCATs,
    },
    [bcmiLbHosPortTypeWEcmp] = {
        .valid = 1,
        .flow_based = 0,
        .name = LB_HASH_PORT_L3_ECMP_OUTPUT_SELECTIONs,
        .key_name = PORT_IDs,
        .offset_name = MEMBER_WEIGHT_OFFSETs,
        .sub_sel_name = MEMBER_WEIGHT_SUBSET_SELECTs,
        .concat_name = MEMBER_WEIGHT_CONCATs,
    },
    [bcmiLbHosPortTypeMplsOverlayEcmp] = {
        .valid = 1,
        .flow_based = 0,
        .name = LB_HASH_PORT_MPLS_ECMP_OUTPUT_SELECTIONs,
        .key_name = PORT_IDs,
        .offset_name = OVERLAY_OFFSETs,
        .sub_sel_name = OVERLAY_SUBSET_SELECTs,
        .concat_name = OVERLAY_CONCATs,
    },
    [bcmiLbHosPortTypeOverlayEcmp] = {
        .valid = 1,
        .flow_based = 0,
        .name = LB_HASH_PORT_L3_ECMP_OUTPUT_SELECTIONs,
        .key_name = PORT_IDs,
        .offset_name = OVERLAY_OFFSETs,
        .sub_sel_name = OVERLAY_SUBSET_SELECTs,
        .concat_name = OVERLAY_CONCATs,
    },
    [bcmiLbHosPortTypeOverlayWEcmp] = {
        .valid = 1,
        .flow_based = 0,
        .name = LB_HASH_PORT_L3_ECMP_OUTPUT_SELECTIONs,
        .key_name = PORT_IDs,
        .offset_name = OVERLAY_MEMBER_WEIGHT_OFFSETs,
        .sub_sel_name = OVERLAY_MEMBER_WEIGHT_SUBSET_SELECTs,
        .concat_name = OVERLAY_MEMBER_WEIGHT_CONCATs,
    },
    [bcmiLbHosPortTypeL2Ecmp] = {
        .valid = 1,
        .flow_based = 0,
        .name = LB_HASH_PORT_L2_ECMP_OUTPUT_SELECTIONs,
        .key_name = PORT_IDs,
        .offset_name = OFFSETs,
        .sub_sel_name = SUBSET_SELECTs,
        .concat_name = CONCATs,
    },
    [bcmiLbHosPortTypeL2OverlayEcmp] = {
        .valid = 1,
        .flow_based = 0,
        .name = LB_HASH_PORT_L2_ECMP_OUTPUT_SELECTIONs,
        .key_name = PORT_IDs,
        .offset_name = OVERLAY_OFFSETs,
        .sub_sel_name = OVERLAY_SUBSET_SELECTs,
        .concat_name = OVERLAY_CONCATs,
    },
    [bcmiLbHosFlowTypeDlbEcmp] = {
        .valid = 1,
        .flow_based = 1,
        .name = LB_HASH_FLOW_DLB_ECMP_OUTPUT_SELECTIONs,
        .key_name = LB_HASH_FLOW_DLB_ECMP_OUTPUT_SELECTION_IDs,
        .offset_name = OFFSETs,
        .sub_sel_name = SUBSET_SELECTs,
        .concat_name = CONCATs,
    },
    [bcmiLbHosFlowTypeEcmp] = {
        .valid = 1,
        .flow_based = 1,
        .name = LB_HASH_FLOW_ECMP_OUTPUT_SELECTIONs,
        .key_name = LB_HASH_FLOW_ECMP_OUTPUT_SELECTION_IDs,
        .offset_name = OFFSETs,
        .sub_sel_name = SUBSET_SELECTs,
        .concat_name = CONCATs,
    },
    [bcmiLbHosFlowTypeEntropy] = {
        .valid = 1,
        .flow_based = 1,
        .name = LB_HASH_FLOW_LBID_OR_ENTROPY_LABEL_OUTPUT_SELECTIONs,
        .key_name = LB_HASH_FLOW_LBID_OR_ENTROPY_LABEL_OUTPUT_SELECTION_IDs,
        .offset_name = OFFSETs,
        .sub_sel_name = SUBSET_SELECTs,
        .concat_name = CONCATs,
    },
    [bcmiLbHosFlowTypeNonUcTrunk] = {
        .valid = 1,
        .flow_based = 1,
        .name = LB_HASH_FLOW_TRUNK_OUTPUT_SELECTIONs,
        .key_name = LB_HASH_FLOW_TRUNK_OUTPUT_SELECTION_IDs,
        .offset_name = NONUC_OFFSETs,
        .sub_sel_name = NONUC_SUBSET_SELECTs,
        .concat_name = NONUC_CONCATs,
    },
    [bcmiLbHosFlowTypeUcTrunk] = {
        .valid = 1,
        .flow_based = 1,
        .name = LB_HASH_FLOW_TRUNK_OUTPUT_SELECTIONs,
        .key_name = LB_HASH_FLOW_TRUNK_OUTPUT_SELECTION_IDs,
        .offset_name = UC_OFFSETs,
        .sub_sel_name = UC_SUBSET_SELECTs,
        .concat_name = UC_CONCATs,
    },
    [bcmiLbHosFlowTypeWECMP] = {
        .valid = 1,
        .flow_based = 1,
        .name = LB_HASH_FLOW_ECMP_OUTPUT_SELECTIONs,
        .key_name = LB_HASH_FLOW_ECMP_OUTPUT_SELECTION_IDs,
        .offset_name = MEMBER_WEIGHT_OFFSETs,
        .sub_sel_name = MEMBER_WEIGHT_SUBSET_SELECTs,
        .concat_name = MEMBER_WEIGHT_CONCATs,
    },
    [bcmiLbHosFlowTypeOverlayWECMP] = {
        .valid = 1,
        .flow_based = 1,
        .name = LB_HASH_FLOW_ECMP_OUTPUT_SELECTIONs,
        .key_name = LB_HASH_FLOW_ECMP_OUTPUT_SELECTION_IDs,
        .offset_name = OVERLAY_MEMBER_WEIGHT_OFFSETs,
        .sub_sel_name = OVERLAY_MEMBER_WEIGHT_SUBSET_SELECTs,
        .concat_name = OVERLAY_MEMBER_WEIGHT_CONCATs,
    },
    [bcmiLbHosFlowTypeOverlayECMP] = {
        .valid = 1,
        .flow_based = 1,
        .name = LB_HASH_FLOW_ECMP_OUTPUT_SELECTIONs,
        .key_name = LB_HASH_FLOW_ECMP_OUTPUT_SELECTION_IDs,
        .offset_name = OVERLAY_OFFSETs,
        .sub_sel_name = OVERLAY_SUBSET_SELECTs,
        .concat_name = OVERLAY_CONCATs,
    },
    [bcmiLbHosFlowTypeRhEcmp] = {
        .valid = 1,
        .flow_based = 1,
        .name = LB_HASH_FLOW_RH_ECMP_OUTPUT_SELECTIONs,
        .key_name = LB_HASH_FLOW_RH_ECMP_OUTPUT_SELECTION_IDs,
        .offset_name = OFFSETs,
        .sub_sel_name = SUBSET_SELECTs,
        .concat_name = CONCATs,
    },
    [bcmiLbHosFlowTypeOverlayRhEcmp] = {
        .valid = 1,
        .flow_based = 1,
        .name = LB_HASH_FLOW_RH_ECMP_OUTPUT_SELECTIONs,
        .key_name = LB_HASH_FLOW_RH_ECMP_OUTPUT_SELECTION_IDs,
        .offset_name = OVERLAY_OFFSETs,
        .sub_sel_name = OVERLAY_SUBSET_SELECTs,
        .concat_name = OVERLAY_CONCATs,
    },
    [bcmiLbHosFlowTypeMplsEcmp] = {
        .valid = 1,
        .flow_based = 1,
        .name = LB_HASH_FLOW_MPLS_ECMP_OUTPUT_SELECTIONs,
        .key_name = LB_HASH_FLOW_MPLS_ECMP_OUTPUT_SELECTION_IDs,
        .offset_name = OFFSETs,
        .sub_sel_name = SUBSET_SELECTs,
        .concat_name = CONCATs,
    },
    [bcmiLbHosFlowTypeMplsOverlayEcmp] = {
        .valid = 1,
        .flow_based = 1,
        .name = LB_HASH_FLOW_MPLS_ECMP_OUTPUT_SELECTIONs,
        .key_name = LB_HASH_FLOW_MPLS_ECMP_OUTPUT_SELECTION_IDs,
        .offset_name = OVERLAY_OFFSETs,
        .sub_sel_name = OVERLAY_SUBSET_SELECTs,
        .concat_name = OVERLAY_CONCATs,
    },
    [bcmiLbHosFlowTypePlfs] = {
        .valid = 1,
        .flow_based = 1,
        .name = LB_HASH_FLOW_PLFS_OUTPUT_SELECTIONs,
        .key_name = LB_HASH_FLOW_PLFS_OUTPUT_SELECTION_IDs,
        .offset_name = OFFSETs,
        .sub_sel_name = SUBSET_SELECTs,
        .concat_name = CONCATs,
    },
    [bcmiLbHosFlowTypeL2Ecmp] = {
        .valid = 1,
        .flow_based = 1,
        .name = LB_HASH_FLOW_L2_ECMP_OUTPUT_SELECTIONs,
        .key_name = LB_HASH_FLOW_L2_ECMP_OUTPUT_SELECTION_IDs,
        .offset_name = OFFSETs,
        .sub_sel_name = SUBSET_SELECTs,
        .concat_name = CONCATs,
    },
    [bcmiLbHosFlowTypeL2OverlayEcmp] = {
        .valid = 1,
        .flow_based = 1,
        .name = LB_HASH_FLOW_L2_ECMP_OUTPUT_SELECTIONs,
        .key_name = LB_HASH_FLOW_L2_ECMP_OUTPUT_SELECTION_IDs,
        .offset_name = OVERLAY_OFFSETs,
        .sub_sel_name = OVERLAY_SUBSET_SELECTs,
        .concat_name = OVERLAY_CONCATs,
    },
};

static int
bcm5699x_ltsw_lb_hash_os_tbl_db_get(int unit, bcmint_ltsw_lb_hash_os_tbl_db_t *tbl_db)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(tbl_db, SHR_E_PARAM);

    tbl_db->tbls = lb_hash_os_tbls;

exit:
    SHR_FUNC_EXIT();
}

static int
bcm5699x_ltsw_lb_hash_versatile_control_set(int unit,
                                            bcmi_ltsw_hash_versatile_control_t control,
                                            int value)
{
    int dunit;
    uint64_t field_value;
    bcmlt_entry_handle_t eh = BCMLT_INVALID_HDL;
    const char *field_name = NULL;

    SHR_FUNC_ENTER(unit);

    field_value = (uint32_t)value;

    dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, LB_HASH_VERSATILE_CONTROLs, &eh));

    switch (control) {
        case bcmiHashVersatileControlBlockAInitValue0:
            field_name = HASH0_INITIAL_VALUE_0s;
            break;
        case bcmiHashVersatileControlBlockAInitValue1:
            field_name = HASH0_INITIAL_VALUE_1s;
            break;
        case bcmiHashVersatileControlBlockBInitValue0:
            field_name = HASH1_INITIAL_VALUE_0s;
            break;
        case bcmiHashVersatileControlBlockBInitValue1:
            field_name = HASH1_INITIAL_VALUE_1s;
            break;
        default:
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_UNAVAIL);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(eh, field_name, field_value));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_set_commit(unit, eh, BCMLT_PRIORITY_NORMAL));

exit:
    if (eh != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(eh);
        eh = BCMLT_INVALID_HDL;
    }
    SHR_FUNC_EXIT();
}

static int
bcm5699x_ltsw_lb_hash_versatile_control_get(int unit,
                                            bcmi_ltsw_hash_versatile_control_t control,
                                            int *value)
{
    int rv;
    int dunit;
    uint64_t field_value;
    bcmlt_entry_handle_t eh = BCMLT_INVALID_HDL;
    const char *field_name = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(value, SHR_E_PARAM);

    dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, LB_HASH_VERSATILE_CONTROLs, &eh));

    rv = bcmi_lt_entry_commit(unit, eh, BCMLT_OPCODE_LOOKUP, BCMLT_PRIORITY_NORMAL);

    if (rv == SHR_E_NOT_FOUND) {
        *value = 0;
        SHR_EXIT();
    } else {
        SHR_IF_ERR_EXIT_EXCEPT_IF(rv, SHR_E_NOT_FOUND);
    }

    switch (control) {
        case bcmiHashVersatileControlBlockAInitValue0:
            field_name = HASH0_INITIAL_VALUE_0s;
            break;
        case bcmiHashVersatileControlBlockAInitValue1:
            field_name = HASH0_INITIAL_VALUE_1s;
            break;
        case bcmiHashVersatileControlBlockBInitValue0:
            field_name = HASH1_INITIAL_VALUE_0s;
            break;
        case bcmiHashVersatileControlBlockBInitValue1:
            field_name = HASH1_INITIAL_VALUE_1s;
            break;
        default:
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_UNAVAIL);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(eh, field_name, &field_value));
    *value = (int)field_value;

exit:
    if (eh != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(eh);
        eh = BCMLT_INVALID_HDL;
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Lb_hash sub driver functions for bcm5699x.
 */
static mbcm_ltsw_lb_hash_drv_t bcm5699x_lb_hash_sub_drv = {
    .lb_hash_os_tbl_db_get = bcm5699x_ltsw_lb_hash_os_tbl_db_get,
    .lb_hash_versatile_control_set = bcm5699x_ltsw_lb_hash_versatile_control_set,
    .lb_hash_versatile_control_get = bcm5699x_ltsw_lb_hash_versatile_control_get,
};

/******************************************************************************
 * Public functions
 */

int
bcm5699x_lb_hash_sub_drv_attach(int unit, void *hdl)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_lb_hash_drv_set(unit, &bcm5699x_lb_hash_sub_drv));

exit:
    SHR_FUNC_EXIT();
}
