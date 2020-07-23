/*! \file bcm56780_a0_ina_2_4_13_range.c
 *
 * BCM56780_A0 ina_2_4_13 Range driver.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#include <bcm_int/ltsw/mbcm/range.h>
#include <bcm_int/ltsw/range_int.h>
#include <bcm_int/ltsw/lt_intf.h>
#include <shr/shr_debug.h>
#include <bcmltd/chip/bcmltd_str.h>
#include <bcmltd/chip/bcmltd_id.h>

/******************************************************************************
 * Local definitions
 */

#define BSL_LOG_MODULE BSL_LS_BCM_RANGE


bcmint_range_tbls_info_t bcm56780_a0_ina_2_4_13_range_tbls_info = {
    .range_sid = "FP_COMPRESSION_RANGE_CHECK",
    .range_key_fid = "FP_COMPRESSION_RANGE_CHECK_ID",
    .range_max = "MAX_VALUE",
    .range_min = "MIN_VALUE",
    .range_type = "FIELD_TYPE",
    .range_grp_sid = "FP_COMPRESSION_RANGE_CHECK_GROUP",
    .range_grp_key_fid = "FP_COMPRESSION_RANGE_CHECK_GROUP_ID",
    .range_grp_key_type_fid = "FP_COMPRESSION_RANGE_CHECK_GROUP",
    .oper_mode_sid = "FP_CONFIG",
    .oper_mode_fid = "FP_ING_RANGE_CHECK_OPERMODE_PIPEUNIQUE"
};

static int
bcm56780_a0_ina_2_4_13_ltsw_range_tbls_info_init(int unit,
                        bcmint_range_tbls_info_t **tbls_info)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(tbls_info, SHR_E_PARAM);

    *tbls_info = &bcm56780_a0_ina_2_4_13_range_tbls_info;

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief range driver function variable for bcm56780_a0 ina_2_4_13 device.
 */
static mbcm_ltsw_range_drv_t bcm56780_a0_ina_2_4_13_ltsw_range_drv = {
    .range_tbls_info_init = bcm56780_a0_ina_2_4_13_ltsw_range_tbls_info_init,
};

/******************************************************************************
 * Public functions
 */

int
bcm56780_a0_ina_2_4_13_ltsw_range_drv_attach(int unit)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_range_drv_set(unit, &bcm56780_a0_ina_2_4_13_ltsw_range_drv));

exit:
    SHR_FUNC_EXIT();
}
