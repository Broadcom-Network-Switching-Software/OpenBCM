/*! \file bcmpc_pm.c
 *
 * BCMPC PM APIs
 *
 * This file implements the functions for configuring the whole PM and the
 * core(s) within a PM.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>
#include <shr/shr_debug.h>

#include <sal/sal_libc.h>
#include <sal/sal_types.h>
#include <sal/sal_assert.h>
#include <shr/shr_bitop.h>
#include <shr/shr_util.h>

#include <bcmpc/bcmpc_lport.h>
#include <bcmpc/bcmpc_lport_internal.h>
#include <bcmpc/bcmpc_topo_internal.h>
#include <bcmpc/bcmpc_pm_internal.h>
#include <bcmpc/bcmpc_pm_internal.h>
#include <bcmpc/bcmpc_pmgr_internal.h>

/*******************************************************************************
 * Local definitions
 */

/* Debug log target definition */
#define BSL_LOG_MODULE BSL_LS_BCMPC_PM


/*******************************************************************************
 * Private functions
 */

/*!
 * \brief Get the sub cores information of a PM.
 *
 * When an aggregated PM runs each core indepently, this function would return
 * the inforamtion for each sub-core by \c cores.
 * For the other cases, this function would simply copy the input \c mode to
 * the the first element of the output array \cores.
 *
 * \param [in] unit Unit number.
 * \param [in] pm_info PM information.
 * \param [in] mode PM mode.
 * \param [in] size Max size of \c cores.
 * \param [out] cores Core information buffer.
 * \param [out] actual Actual size of \c cores.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_MEMORY The buffer \c cores is too small.
 */
static int
pm_cores_from_mode(int unit, bcmpc_pm_info_t *pm_info, bcmpc_pm_mode_t *mode,
                   int size, bcmpc_pm_info_t *cores, int *actual)
{
    bcmpc_topo_type_t *pm_prop;
    int idx, lbmp;
    int lanes_per_core, ports_per_core;

    SHR_FUNC_ENTER(unit);

    pm_prop = &pm_info->prop;
    lbmp = mode->lane_map[0];

    /*
     * Assume that each subcore has the same number of lanes and physical ports.
     */
    assert(pm_prop->num_aggr > 0);
    lanes_per_core = pm_prop->num_lanes / pm_prop->num_aggr;
    ports_per_core = pm_prop->num_ports / pm_prop->num_aggr;

    /*
     * When an aggregated PM run each core indenpently, to calculate the PM
     * information for each sub core.
     */
    if (pm_prop->num_aggr > 1 &&
        shr_util_popcount(lbmp) <= lanes_per_core) {
        if (pm_prop->num_aggr > size) {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit, "Buffer is too small.\n")));
            SHR_ERR_EXIT(SHR_E_MEMORY);
        }

        for (idx = 0; idx < pm_prop->num_aggr; idx++) {
            bcmpc_topo_type_t *core_prop = &cores[idx].prop;
            cores[idx].base_pport = pm_info->base_pport + ports_per_core * idx;
            core_prop->num_lanes = lanes_per_core;
            core_prop->num_ports = ports_per_core;
            core_prop->num_aggr = 1;
            core_prop->pm_drv = pm_info->prop.pm_drv;
        }
        *actual = pm_prop->num_aggr;

        SHR_EXIT();
    }

    /* Otherwise, simply copy the content to the first element. */
    sal_memcpy(cores, pm_info, sizeof(*pm_info));
    *actual = 1;

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Set PM mode to hardware.
 *
 * \param [in] unit Unit number.
 * \param [in] pm_info PM information.
 * \param [in] mode PM mode.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Fail to set PM mode to hardware.
 */
static int
pm_hw_mode_set(int unit, bcmpc_pm_info_t *pm_info, bcmpc_pm_mode_t *mode)
{
    int ci, pi, pm_pport, lane_shift;
    int cores_cnt;
    bcmpc_pm_info_t cores[BCMPC_NUM_AGGR_CORES_PER_PM_MAX], *core;
    bcmpc_pm_mode_t core_mode;

    SHR_FUNC_ENTER(unit);

    for (ci = 0; ci < COUNTOF(cores); ci++) {
        bcmpc_pm_info_t_init(&cores[ci]);
    }
    bcmpc_pm_mode_t_init(&core_mode);

    SHR_IF_ERR_VERBOSE_EXIT
        (pm_cores_from_mode(unit, pm_info, mode,
                            COUNTOF(cores), cores, &cores_cnt));

    /* Configure the mode of each core within the PM. */
    pm_pport = 0;
    lane_shift = 0;
    for (ci = 0; ci < cores_cnt; ci++) {
        core = &cores[ci];
        core_mode.opmode[0] = mode->opmode[ci];
        for (pi = 0; pi < core->prop.num_lanes; pi++, pm_pport++) {
            core_mode.speed_max[pi] = mode->speed_max[pm_pport];
            core_mode.lane_map[pi] = mode->lane_map[pm_pport] >> lane_shift;
        }
        lane_shift += core->prop.num_lanes;

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmpc_pmgr_core_mode_set(unit, core->base_pport,
                                      &core_mode));
    }

exit:
    SHR_FUNC_EXIT();
}


/*******************************************************************************
 * Internal Public functions
 */

int
bcmpc_internal_pm_mode_set(int unit, int pm_id, bcmpc_pm_mode_t *mode)
{
    bcmpc_pm_info_t pm_info;
    bcmpc_topo_type_t *pm_prop = &pm_info.prop;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmpc_pm_info_get(unit, pm_id, &pm_info));

    /* Deactivate the ports in this PM. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmpc_ports_deactivate(unit, pm_info.base_pport, pm_prop->num_ports));

    /* Update the configuration to Hardware. */
    SHR_IF_ERR_VERBOSE_EXIT
        (pm_hw_mode_set(unit, &pm_info, mode));

    /*
     * Stage the new PM configurations to make the PC internal sub-components
     * know we are currently updating the PM mode and be able to get the
     * the port attribues e.g. the number of lanes and max speed accroding to
     * the new PM mode.
     */
    SHR_IF_ERR_EXIT
        (bcmpc_pm_mode_stage_set(unit, pm_id, mode));

    /* Re-activate the ports in this PM based on the new mode. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmpc_ports_activate(unit, pm_info.base_pport, pm_prop->num_ports));

exit:
    bcmpc_pm_mode_stage_delete(unit, pm_id);

    SHR_FUNC_EXIT();
}

int
bcmpc_internal_pm_mode_delete(int unit, int pm_id)
{
    bcmpc_pm_info_t pm_info;
    bcmpc_topo_type_t *pm_prop = &pm_info.prop;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmpc_pm_info_get(unit, pm_id, &pm_info));

    /* Move the ports in this PM to invalid state. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmpc_ports_deactivate(unit, pm_info.base_pport, pm_prop->num_ports));

exit:
    SHR_FUNC_EXIT();
}

void
bcmpc_pm_mode_t_init(bcmpc_pm_mode_t *mode)
{
    sal_memset(mode, 0, sizeof(bcmpc_pm_mode_t));
}

void
bcmpc_pm_core_cfg_t_init(bcmpc_pm_core_cfg_t *ccfg)
{
    uint64_t i;

    sal_memset(ccfg, 0, sizeof(*ccfg));
    for (i = 0 ; i < BCMPC_NUM_LANES_PER_PM_MAX; i++) {
        BCMPC_LANE_MAP_SET(ccfg->tx_lane_map, i, i);
        BCMPC_LANE_MAP_SET(ccfg->rx_lane_map, i, i);
    }
}

int
bcmpc_pm_core_config_get(int unit, bcmpc_pm_core_t pm_core,
                         bcmpc_pm_core_cfg_t *core_config)
{
    bcmpc_pm_info_t pm_info;
    bcmpc_topo_type_t *pm_prop = &pm_info.prop;
    bcmpc_pm_core_cfg_t db_core_config;
    int num_lanes;
    int lane_idx;
    int rv = SHR_E_NONE;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmpc_pm_info_get(unit, pm_core.pm_id, &pm_info));

    /*
     * Get the number of lanes supported on the PortMacro. */
    num_lanes = pm_prop->num_lanes;

    bcmpc_pm_core_cfg_t_init(&db_core_config);

    /* Get the PC_PM_CORE configuration information from the IMM DB. */
    rv = bcmpc_db_imm_entry_lookup(unit, PC_PM_COREt, P(&pm_core),
                                   P(&db_core_config));

    /*
     * Check is auto fields are set for the RX lane swap, TX lane swap,
     * RX polarity flip, TX polarity flip.
     * If set - use defaults, that is no lane swap.
     * If unset - use the values from the LT.
     */
    if (rv == SHR_E_NONE) {
        core_config->rx_lane_map_auto = db_core_config.rx_lane_map_auto;
        core_config->tx_lane_map_auto = db_core_config.tx_lane_map_auto;
        core_config->rx_polarity_flip_auto =
                                     db_core_config.rx_polarity_flip_auto;
        core_config->tx_polarity_flip_auto =
                                     db_core_config.tx_polarity_flip_auto;
    } else if (rv == SHR_E_NOT_FOUND) {
        core_config->rx_lane_map_auto = true;
        core_config->tx_lane_map_auto = true;
        core_config->rx_polarity_flip_auto = true;
        core_config->tx_polarity_flip_auto = true;
    } else if (rv < 0) {
        SHR_ERR_EXIT(rv);
    }

    if (core_config->rx_lane_map_auto) {
        for (lane_idx = 0; lane_idx < num_lanes; lane_idx++) {
            BCMPC_LANE_MAP_SET(core_config->rx_lane_map, lane_idx, lane_idx);
        }
    } else {
        core_config->rx_lane_map = db_core_config.rx_lane_map;
    }

    if (core_config->tx_lane_map_auto) {
        for (lane_idx = 0; lane_idx < num_lanes; lane_idx++) {
            BCMPC_LANE_MAP_SET(core_config->tx_lane_map, lane_idx, lane_idx);
        }
    } else {
        core_config->tx_lane_map = db_core_config.tx_lane_map;
    }

    if (core_config->rx_polarity_flip_auto) {
        core_config->rx_polarity_flip = 0x0;
    } else {
        core_config->rx_polarity_flip = db_core_config.rx_polarity_flip;
    }

    if (core_config->tx_polarity_flip_auto) {
        core_config->tx_polarity_flip = 0x0;
    } else {
        core_config->tx_polarity_flip = db_core_config.tx_polarity_flip;
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmpc_pm_hw_core_config_get(int unit, bcmpc_pm_core_t pm_core,
                            bcmpc_pm_core_cfg_t *core_config)
{
    bcmpc_pm_info_t pm_info;
    bcmpc_pport_t pport = BCMPC_INVALID_PPORT;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmpc_pm_info_get(unit, pm_core.pm_id, &pm_info));

    pport = pm_info.base_pport;

    SHR_IF_ERR_EXIT
        (bcmpc_pmgr_pm_core_config_get(unit, pport, core_config));

exit:
    SHR_FUNC_EXIT();
}

int
bcmpc_pm_core_is_active(int unit, bcmpc_pm_core_t pm_core, bool *is_active)
{
    pm_info_t *pm_drv_info = NULL;

    SHR_FUNC_ENTER(unit);

    /* Get the associated PM core driver database information. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmpc_pm_drv_info_get(unit, pm_core.pm_id, &pm_drv_info));

    *is_active = pm_drv_info->pm_data.pm_ha_info->is_pm_initialized;

exit:
    SHR_FUNC_EXIT();
}
