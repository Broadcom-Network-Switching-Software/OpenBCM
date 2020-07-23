/*! \file latency_monitor.c
 *
 * Latency Distribution Histogram.
 * This file contains the management for LDH.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bcm/latency_monitor.h>

#include <bcm_int/ltsw_dispatch.h>
#include <bcm_int/ltsw/mbcm/ldh.h>
#include <bcm_int/ltsw/feature.h>

#include <bsl/bsl.h>
#include <shr/shr_debug.h>

/******************************************************************************
 * Local definitions
 */
#define BSL_LOG_MODULE BSL_LS_BCM_LATENCY_MONITOR


/******************************************************************************
 * Private functions
 */


/*******************************************************************************
 * Public Internal HSDK functions
 */

/******************************************************************************
 * Public Functions
 */
int
bcm_ltsw_latency_monitor_init(
    int unit)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (mbcm_ltsw_ldh_init(unit));
exit:
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_latency_monitor_detach(
    int unit)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (mbcm_ltsw_ldh_detach(unit));
exit:
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_latency_monitor_config_get(
    int unit,
    uint8 monitor_id,
    bcm_latency_monitor_config_t *config)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (mbcm_ltsw_ldh_monitor_id_check(unit, monitor_id));

    bcm_latency_monitor_config_t_init(config);
    SHR_IF_ERR_EXIT
        (mbcm_ltsw_ldh_monitor_config_get(unit, monitor_id, config));
exit:
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_latency_monitor_config_set(
    int unit,
    uint8 monitor_id,
    bcm_latency_monitor_config_t *config)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (mbcm_ltsw_ldh_monitor_config_check(unit, monitor_id, config));

    SHR_IF_ERR_EXIT
        (mbcm_ltsw_ldh_monitor_config_set(unit, monitor_id, config));
exit:
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_latency_monitor_enable(
    int unit,
    uint8 monitor_id,
    uint8 enable)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (mbcm_ltsw_ldh_monitor_id_check(unit, monitor_id));

    SHR_IF_ERR_EXIT
        (mbcm_ltsw_ldh_monitor_enable(unit, monitor_id, enable));
exit:
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_latency_monitor_stat_attach(
    int unit,
    uint8 monitor_id,
    uint32 stat_counter_id)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (mbcm_ltsw_ldh_monitor_id_check(unit, monitor_id));

    SHR_IF_ERR_EXIT
        (mbcm_ltsw_ldh_monitor_stat_attach(unit, monitor_id, stat_counter_id));
exit:
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_latency_monitor_stat_detach(
    int unit,
    uint8 monitor_id)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (mbcm_ltsw_ldh_monitor_id_check(unit, monitor_id));

    SHR_IF_ERR_EXIT
        (mbcm_ltsw_ldh_monitor_stat_detach(unit, monitor_id));
exit:
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_latency_monitor_stat_id_get(
    int unit,
    uint8 monitor_id,
    uint32 *stat_counter_id)
{
    SHR_FUNC_ENTER(unit);
    SHR_IF_ERR_EXIT
        (mbcm_ltsw_ldh_monitor_id_check(unit, monitor_id));
    SHR_NULL_CHECK(stat_counter_id, SHR_E_PARAM);

    SHR_IF_ERR_EXIT
        (mbcm_ltsw_ldh_monitor_stat_id_get(unit, monitor_id, stat_counter_id));
exit:
    SHR_FUNC_EXIT();

}


