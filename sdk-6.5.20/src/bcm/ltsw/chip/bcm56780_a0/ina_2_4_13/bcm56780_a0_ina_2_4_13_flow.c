/*! \file bcm56780_a0_ina_2_4_13_flow.c
 *
 * BCM56780_A0 INA_2_4_13 specific FLOW LT database.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bcm_int/ltsw/mbcm/flow.h>

#include <bcmltd/chip/bcmltd_str.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmltd/chip/bcmltd_device_constants.h>
#include <shr/shr_debug.h>

/******************************************************************************
 * Local definitions
 */

#define BSL_LOG_MODULE BSL_LS_BCM_FLOW

/* EGR_DVP LT INA_2_4_13 specific fields. */
static const bcmint_flow_fld_t
egr_dvp_flds[BCMINT_LTSW_FLOW_FLD_EGR_DVP_FLD_CNT] = {
    [BCMINT_LTSW_FLOW_FLD_EGR_DVP_L2_MTU_VALUE] = {
        L2_MTU_VALUEs,
    },
    [BCMINT_LTSW_FLOW_FLD_EGR_DVP_PROCESS_CTRL_2] = {
        PROCESS_CTRL_2s,
    },
};

static const bcmint_flow_lt_t
bcm56780_a0_ina_2_4_13_flow_lt[] = {
    [BCMINT_LTSW_FLOW_LT_EGR_DVP] = {
        .name = EGR_DVPs,
        .fld_bmp =(1 << BCMINT_LTSW_FLOW_FLD_EGR_DVP_L2_MTU_VALUE) |
                  (1 << BCMINT_LTSW_FLOW_FLD_EGR_DVP_PROCESS_CTRL_2),
        .flds = egr_dvp_flds
    }
};

/******************************************************************************
 * Private functions
 */

static int
bcm56780_a0_ina_2_4_13_flow_lt_db_get(int unit,
                                     bcmint_flow_lt_db_t *lt_db)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(lt_db, SHR_E_PARAM);

    /* Bitmap of bcm56780 INA_2_4_13 specific LTs. */
    lt_db->lt_bmp = (1 << BCMINT_LTSW_FLOW_LT_EGR_DVP);

    lt_db->lts = bcm56780_a0_ina_2_4_13_flow_lt;

exit:
    SHR_FUNC_EXIT();
}

static mbcm_ltsw_flow_drv_t bcm56780_a0_ina_2_4_13_ltsw_flow_drv = {
    /* Get FLOW LT database. */
    .flow_lt_db_get = bcm56780_a0_ina_2_4_13_flow_lt_db_get
};

/******************************************************************************
 * Public functions
 */
int
bcm56780_a0_ina_2_4_13_ltsw_flow_drv_attach(int unit)
{
    return mbcm_ltsw_flow_drv_set(unit,
                                  &bcm56780_a0_ina_2_4_13_ltsw_flow_drv);
}
