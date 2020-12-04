/*! \file bcmpc_pm_drv_pm4x25.c
 *
 * PM driver for PM4x25.
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


/*******************************************************************************
 * Local definitions
 */

/* Debug log target definition */
#define BSL_LOG_MODULE BSL_LS_BCMPC_PMGR

/*! The number of lanes in PM4x25. */
#define PM4X25_LANES 4

/*! \brief PM4x25 lane map. */
typedef struct pm_lane_layout_s {
    int lane_map[PM4X25_LANES];
} pm_lane_layout_t;


/*******************************************************************************
 * PM driver function vector
 */

static int
pm4x25_speed_config_validate(int unit, const pm_access_t *pm_acc,
                             pm_oper_status_t *op_st,
                             pm_info_t *pm_info, pm_speed_config_t *speed_cfg)
{
    uint32_t idx, *valid_speeds, num_speeds;
    uint32_t valid_speeds_1_lane[] = { 1000, 10000, 20000, 25000, 27000 };
    uint32_t valid_speeds_2_lane[] = { 20000, 21000, 40000, 42000, 50000,
                                       53000 };
    uint32_t valid_speeds_4_lane[] = { 40000, 42000, 50000, 53000, 100000,
                                       106000 };

    SHR_FUNC_ENTER(unit);

    if (speed_cfg->fec != PM_PORT_FEC_NONE &&
        speed_cfg->fec != PM_PORT_FEC_BASE_R &&
        speed_cfg->fec != PM_PORT_FEC_RS_528) {
        SHR_ERR_EXIT(SHR_E_FAIL);
    }

    if (speed_cfg->num_lanes == 1) {
        valid_speeds = valid_speeds_1_lane;
        num_speeds = COUNTOF(valid_speeds_1_lane);
    } else if (speed_cfg->num_lanes == 2) {
        valid_speeds = valid_speeds_2_lane;
        num_speeds = COUNTOF(valid_speeds_2_lane);
    } else {
        valid_speeds = valid_speeds_4_lane;
        num_speeds = COUNTOF(valid_speeds_4_lane);
    }

    for (idx = 0; idx < num_speeds; idx++) {
        if (speed_cfg->speed == valid_speeds[idx]) {
            SHR_EXIT();
        }
    }

    SHR_ERR_EXIT(SHR_E_FAIL);

exit:
    SHR_FUNC_EXIT();
}


/*******************************************************************************
 * Public functions
 */

bcmpc_pm_drv_t bcmpc_pm_drv_pm4x25 = {
    .speed_config_validate = pm4x25_speed_config_validate
};

