/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
/*! \file range.c
 *
 * XFS Range Driver.
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

#include <sal/sal_types.h>
#include <shr/shr_debug.h>
#include <sal/sal_mutex.h>

/******************************************************************************
 * Local definitions
 */
#define BSL_LOG_MODULE BSL_LS_BCM_RANGE

int
xfs_ltsw_range_clear(int unit)
{
    SHR_FUNC_ENTER(unit);

    /* Clear Range Check table. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_clear(unit, FP_COMPRESSION_RANGE_CHECKs));

exit:
    SHR_FUNC_EXIT();
}

int
xfs_ltsw_range_rtype_add(int unit,
        bcmlt_entry_handle_t range_check_lt,
        bcmint_range_tbls_info_t *tbls_info,
        bcm_range_config_t *range_config)
{
    int rtype_val = 0;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(range_config, SHR_E_PARAM);
    SHR_NULL_CHECK(tbls_info, SHR_E_PARAM);

    switch (range_config->rtype) {
        case bcmRangeTypeL4SrcPort:
            rtype_val = 0;
            break;
        case bcmRangeTypeL4DstPort:
            rtype_val = 1;
            break;
        case bcmRangeTypePacketLength:
            rtype_val = 2;
            break;
        case bcmRangeTypeIcmpTypeCode:
            rtype_val = 3;
            break;
        default:
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_UNAVAIL);
    }

    /* Add Range Type field to RANGE CHECK LT. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(range_check_lt,
                               tbls_info->range_type,
                               rtype_val));

exit:
    SHR_FUNC_EXIT();
}

int
xfs_ltsw_range_rtype_get(int unit,
        bcmlt_entry_handle_t range_check_lt,
        bcmint_range_tbls_info_t *tbls_info,
        bcm_range_config_t *range_config)
{
    uint64_t rtype = 0;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(range_config, SHR_E_PARAM);
    SHR_NULL_CHECK(tbls_info, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(range_check_lt,
                               tbls_info->range_type,
                               &(rtype)));

    switch (rtype) {
        case 0:
            range_config->rtype = bcmRangeTypeL4SrcPort;
            break;
        case 1:
            range_config->rtype = bcmRangeTypeL4DstPort;
            break;
        case 2:
            range_config->rtype = bcmRangeTypePacketLength;
            break;
        case 3:
            range_config->rtype = bcmRangeTypeIcmpTypeCode;
            break;
        default:
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_UNAVAIL);
    }

exit:
    SHR_FUNC_EXIT();
}
