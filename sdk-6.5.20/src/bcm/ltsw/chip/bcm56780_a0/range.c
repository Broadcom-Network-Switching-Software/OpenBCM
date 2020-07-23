/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
/*! \file range.c
 *
 * BCM56780_A0 Range Driver.
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
#include <bcm_int/ltsw/xfs/range.h>
#include <bcm_int/ltsw/chip/bcm56780_a0/variant_dispatch.h>

#include <sal/sal_types.h>
#include <shr/shr_debug.h>
#include <sal/sal_mutex.h>

/******************************************************************************
 * Local definitions
 */
#define BSL_LOG_MODULE BSL_LS_BCM_RANGE

/*!
 * \brief mbcm driver mapping for bcm56780_a0
 */
static mbcm_ltsw_range_drv_t bcm56780_a0_ltsw_range_drv = {
    .range_clear = xfs_ltsw_range_clear,
    .range_rtype_add = xfs_ltsw_range_rtype_add,
    .range_rtype_get = xfs_ltsw_range_rtype_get,
};

/******************************************************************************
 * Public functions
 */
int
bcm56780_a0_ltsw_range_drv_attach(int unit)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (mbcm_ltsw_range_drv_set(unit, &bcm56780_a0_ltsw_range_drv));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcm56780_a0_ltsw_variant_drv_attach(unit,
                                             BCM56780_A0_LTSW_VARIANT_RANGE));

exit:
    SHR_FUNC_EXIT();
}
