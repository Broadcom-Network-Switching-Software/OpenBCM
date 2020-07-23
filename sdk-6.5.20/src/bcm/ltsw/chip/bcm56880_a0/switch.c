/*! \file switch.c
 *
 * Switch control interfaces for device bcm56880.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_debug.h>
#include <bcm_int/ltsw/xfs/switch.h>
#include <bcm_int/ltsw/mbcm/switch.h>
#include <bcm_int/ltsw/chip/bcm56880_a0/variant_dispatch.h>

#define BSL_LOG_MODULE BSL_LS_BCM_SWITCH

/******************************************************************************
 * Private functions
 */

static int
bcm56880_a0_scp_set(int unit, int port, bcm_switch_control_t type,
                    int arg)
{
    return bcmint_xfs_scp_set(unit, port, type, arg);
}

static int
bcm56880_a0_scp_get(int unit, int port, bcm_switch_control_t type,
                    int *arg)
{
    return bcmint_xfs_scp_get(unit, port, type, arg);
}

static int
bcm56880_a0_sc_set(int unit, bcm_switch_control_t type, int arg)
{
    return bcmint_xfs_sc_set(unit, type, arg);
}

static int
bcm56880_a0_sc_get(int unit, bcm_switch_control_t type, int *arg)
{
    return bcmint_xfs_sc_get(unit, type, arg);
}

static int
bcm56880_a0_spc_add(int unit, uint32_t options,
                    bcm_switch_pkt_protocol_match_t *match,
                    bcm_switch_pkt_control_action_t *action,
                    int priority)
{
    return bcmint_xfs_spc_add(unit, options, match, action, priority);
}

static int
bcm56880_a0_spc_get(int unit,
                    bcm_switch_pkt_protocol_match_t *match,
                    bcm_switch_pkt_control_action_t *action,
                    int *priority)
{
    return bcmint_xfs_spc_get(unit, match, action, priority);
}

static int
bcm56880_a0_spc_delete(int unit, bcm_switch_pkt_protocol_match_t *match)
{
    return bcmint_xfs_spc_delete(unit, match);
}

static int
bcm56880_a0_switch_pkt_integrity_check_add(int unit, uint32 options,
                                           bcm_switch_pkt_integrity_match_t *match,
                                           bcm_switch_pkt_control_action_t *action,
                                           int priority)
{
    return bcmint_xfs_switch_pkt_integrity_check_add(unit, options, match, action,
                                                     priority);
}

static int
bcm56880_a0_switch_pkt_integrity_check_get(int unit,
                                           bcm_switch_pkt_integrity_match_t *match,
                                           bcm_switch_pkt_control_action_t *action,
                                           int *priority)
{
    return bcmint_xfs_switch_pkt_integrity_check_get(unit, match, action, priority);
}

static int
bcm56880_a0_switch_pkt_integrity_check_delete(int unit,
                                              bcm_switch_pkt_integrity_match_t *match)

{
    return bcmint_xfs_switch_pkt_integrity_check_delete(unit, match);
}

static int
bcm56880_a0_switch_drop_event_mon_set(int unit, bcm_switch_drop_event_mon_t *monitor)
{
    return bcmint_xfs_switch_drop_event_mon_set(unit, monitor);
}

static int
bcm56880_a0_switch_drop_event_mon_get(int unit, bcm_switch_drop_event_mon_t *monitor)
{
    return bcmint_xfs_switch_drop_event_mon_get(unit, monitor);
}

static int
bcm56880_a0_switch_trace_event_mon_set(int unit, bcm_switch_trace_event_mon_t *monitor)
{
    return bcmint_xfs_switch_trace_event_mon_set(unit, monitor);
}

static int
bcm56880_a0_switch_trace_event_mon_get(int unit, bcm_switch_trace_event_mon_t *monitor)
{
    return bcmint_xfs_switch_trace_event_mon_get(unit, monitor);
}

static int
bcm56880_a0_switch_drop_event_to_symbol(int unit,
                                        bcm_pkt_drop_event_t drop_event,
                                        const char **table, const char **symbol)
{
    return bcmint_xfs_switch_drop_event_to_symbol(unit, drop_event, table, symbol);
}

static int
bcm56880_a0_switch_trace_event_to_symbol(int unit,
                                         bcm_pkt_trace_event_t trace_event,
                                         const char **table, const char **symbol)
{
    return bcmint_xfs_switch_trace_event_to_symbol(unit, trace_event, table, symbol);
}

static int
bcm56880_a0_switch_init(int unit)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_xfs_switch_init(unit));

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56880_a0_switch_detach(int unit)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_xfs_switch_detach(unit));

exit:
    SHR_FUNC_EXIT();
}

static mbcm_ltsw_sc_drv_t bcm56880_a0_sc_drv = {
    .scp_set = bcm56880_a0_scp_set,
    .scp_get = bcm56880_a0_scp_get,
    .sc_set = bcm56880_a0_sc_set,
    .sc_get = bcm56880_a0_sc_get,
    .spc_add = bcm56880_a0_spc_add,
    .spc_get = bcm56880_a0_spc_get,
    .spc_del = bcm56880_a0_spc_delete,
    .switch_pkt_integrity_check_add = bcm56880_a0_switch_pkt_integrity_check_add,
    .switch_pkt_integrity_check_get = bcm56880_a0_switch_pkt_integrity_check_get,
    .switch_pkt_integrity_check_delete = bcm56880_a0_switch_pkt_integrity_check_delete,
    .switch_drop_event_mon_set = bcm56880_a0_switch_drop_event_mon_set,
    .switch_drop_event_mon_get = bcm56880_a0_switch_drop_event_mon_get,
    .switch_trace_event_mon_set = bcm56880_a0_switch_trace_event_mon_set,
    .switch_trace_event_mon_get = bcm56880_a0_switch_trace_event_mon_get,
    .switch_drop_event_to_symbol = bcm56880_a0_switch_drop_event_to_symbol,
    .switch_trace_event_to_symbol = bcm56880_a0_switch_trace_event_to_symbol,
    .switch_init = bcm56880_a0_switch_init,
    .switch_detach = bcm56880_a0_switch_detach,
};

/******************************************************************************
 * Public functions
 */

int
bcm56880_a0_ltsw_sc_drv_attach(int unit)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_sc_drv_set(unit, &bcm56880_a0_sc_drv));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcm56880_a0_ltsw_variant_drv_attach(unit,
                                             BCM56880_A0_LTSW_VARIANT_SWITCH));
exit:
    SHR_FUNC_EXIT();
}

