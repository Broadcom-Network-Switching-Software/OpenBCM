/*! \file bcmpc_pm_drv_pmsgmii4px2.c
 *
 * PM driver for SGMII4Px2.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>
#include <shr/shr_debug.h>
#include <shr/shr_util.h>

#include <bcmpc/bcmpc_util_internal.h>
#include <bcmpc/bcmpc_pm_drv_internal.h>
#include <bcmpc/bcmpc_pm.h>


/*******************************************************************************
 * Local definitions
 */

/* Debug log target definition */
#define BSL_LOG_MODULE BSL_LS_BCMPC_PMGR

/*! The number of lanes in VIPER. */
#define PMSGMII4PX2_LANES 4

/*! \brief VIPER lane map. */
typedef struct pm_lane_layout_s {
    int lane_map[PMSGMII4PX2_LANES];
} pm_lane_layout_t;


/*******************************************************************************
 * PM driver function vector
 */

static int
pmsgmii4px2_speed_config_validate(int unit, const pm_access_t *pm_acc,
                                  pm_oper_status_t *op_st,
                                  pm_info_t *pm_info,
                                  pm_speed_config_t *speed_cfg)
{
    return SHR_E_NONE;
}


/*******************************************************************************
 * Public functions
 */

bcmpc_pm_drv_t bcmpc_pm_drv_pmsgmii4px2 = {
    .speed_config_validate = pmsgmii4px2_speed_config_validate
};

