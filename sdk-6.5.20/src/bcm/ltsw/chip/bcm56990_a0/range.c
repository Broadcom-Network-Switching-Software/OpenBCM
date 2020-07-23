/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
/*! \file field.c
 *
 * BCM56990_A0 Range Driver.
 */
#include <bsl/bsl.h>
#include <bcm/types.h>
#include <bcm/range.h>

#include <bcm_int/control.h>
#include <bcm_int/ltsw/range_int.h>
#include <bcm_int/ltsw_dispatch.h>
#include <bcm_int/ltsw/mbcm/range.h>
#include <bcm_int/ltsw/util.h>
#include <bcm_int/ltsw/lt_intf.h>
#include <bcmltd/chip/bcmltd_str.h>

#include <bcm_int/ltsw/xgs/udf.h>

#include <sal/sal_types.h>
#include <shr/shr_debug.h>
#include <sal/sal_mutex.h>

/******************************************************************************
 * Local definitions
 */
#define BSL_LOG_MODULE BSL_LS_BCM_RANGE

bcmint_range_tbls_info_t bcm56990_a0_range_tbls_info = {
    .range_sid = "FP_ING_RANGE_CHECK",
    .range_key_fid = "FP_ING_RANGE_CHECK_ID",
    .range_max = "MAX_VALUE",
    .range_min = "MIN_VALUE",
    .range_type = "RC_FIELD_TYPE",
    .range_grp_sid = "FP_ING_RANGE_CHECK_GROUP",
    .range_grp_key_fid = "FP_ING_RANGE_CHECK_GROUP_ID",
    .range_grp_key_type_fid = "FP_ING_RANGE_GROUP",
    .oper_mode_sid = "FP_CONFIG",
    .oper_mode_fid = "FP_ING_RANGE_CHECK_OPERMODE_PIPEUNIQUE"
};

static int
bcm56990_a0_ltsw_range_tbls_info_init(int unit,
                                      bcmint_range_tbls_info_t **tbls_info)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(tbls_info, SHR_E_PARAM);

    *tbls_info = &bcm56990_a0_range_tbls_info;

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56990_a0_ltsw_range_rtype_add(int unit,
        bcmlt_entry_handle_t range_check_lt,
        bcmint_range_tbls_info_t *tbls_info,
        bcm_range_config_t *range_config)
{
    char                *rtype_val = NULL;
    bool                r_udf = false;
    uint16              udf_mask = ~0;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(range_config, SHR_E_PARAM);
    SHR_NULL_CHECK(tbls_info, SHR_E_PARAM);

    switch (range_config->rtype) {
        case bcmRangeTypeL4SrcPort:
            rtype_val = "L4SRCPORT";
            break;
        case bcmRangeTypeL4DstPort:
            rtype_val = "L4DSTPORT";
            break;
        case bcmRangeTypeOuterVlan:
            rtype_val = "OUTER_VLANID";
            break;
        case bcmRangeTypePacketLength:
            rtype_val = "PKT_PAYLOAD_LEN";
            break;
        case bcmRangeTypeUdf:
            SHR_IF_ERR_VERBOSE_EXIT
                (xgs_ltsw_udf_obj_is_range_check(unit,
                                                 range_config->udf_id,
                                                 &r_udf));
            if (false == r_udf) {
                SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
            }
            /* Only 1 chunk can be used. So Mask has to be within 16 bits. */
            if ((range_config->offset + range_config->width) > 16) { 
                SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
            }
            udf_mask <<= (range_config->width);
            udf_mask = ~udf_mask;
            udf_mask <<= range_config->offset;
            rtype_val = "UDF";
            break;
        default:
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_UNAVAIL);
    }

    /* Add Range Type field to RANGE CHECK LT. */
    SHR_IF_ERR_VERBOSE_EXIT
       (bcmlt_entry_field_symbol_add(range_check_lt,
                              tbls_info->range_type,
                              rtype_val));

    if (true == r_udf) {
        SHR_IF_ERR_VERBOSE_EXIT
           (bcmlt_entry_field_add(range_check_lt,
                                  "UDF1_CHUNK2_MASK",
                                  udf_mask));
    }

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56990_a0_ltsw_range_rtype_get(int unit,
        bcmlt_entry_handle_t range_check_lt,
        bcmint_range_tbls_info_t *tbls_info,
        bcm_range_config_t *range_config)
{
    const char *rtype = NULL;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(range_config, SHR_E_PARAM);
    SHR_NULL_CHECK(tbls_info, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_symbol_get(range_check_lt,
                                      tbls_info->range_type,
                                      &(rtype)));

    if (!sal_strcmp("L4SRCPORT", rtype)) {
        range_config->rtype = bcmRangeTypeL4SrcPort;
    } else if (!sal_strcmp("L4DSTPORT", rtype)) {
        range_config->rtype = bcmRangeTypeL4DstPort;
    } else if (!sal_strcmp("OUTER_VLANID", rtype)) {
        range_config->rtype = bcmRangeTypeOuterVlan;
    } else if (!sal_strcmp("PKT_PAYLOAD_LEN", rtype)) {
        range_config->rtype = bcmRangeTypePacketLength;
    } else if (!sal_strcmp("UDF", rtype)) {
        range_config->rtype = bcmRangeTypeUdf;
    } else {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_UNAVAIL);
    }

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56990_a0_ltsw_range_clear(int unit)
{
    SHR_FUNC_ENTER(unit);

    /* Clear Range Check table. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_clear(unit, FP_ING_RANGE_CHECKs));

    /* Clear Range Group table. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_clear(unit, FP_ING_RANGE_CHECK_GROUPs));
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief mbcm driver mapping for bcm56990_a0
 */
static mbcm_ltsw_range_drv_t bcm56990_a0_ltsw_range_drv = {
    .range_tbls_info_init  = bcm56990_a0_ltsw_range_tbls_info_init,
    .range_clear = bcm56990_a0_ltsw_range_clear,
    .range_rtype_add = bcm56990_a0_ltsw_range_rtype_add,
    .range_rtype_get = bcm56990_a0_ltsw_range_rtype_get,
};

/******************************************************************************
 * Public functions
 */
int
bcm56990_a0_ltsw_range_drv_attach(int unit)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (mbcm_ltsw_range_drv_set(unit, &bcm56990_a0_ltsw_range_drv));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Attach functions for other members.
 */
#define BCM56990_A0_DRV_ATTACH_ENTRY(_dn,_vn,_pf,_pd,_r0) \
int _vn##_ltsw_range_drv_attach(int unit) \
{ \
    SHR_FUNC_ENTER(unit); \
    SHR_IF_ERR_VERBOSE_EXIT \
        (mbcm_ltsw_range_drv_set(unit, &bcm56990_a0_ltsw_range_drv)); \
exit: \
    SHR_FUNC_EXIT(); \
}
#include "sub_devlist.h"
