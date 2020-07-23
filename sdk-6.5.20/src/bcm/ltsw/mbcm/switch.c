/*! \file switch.c
 *
 * MBCM switch control interfaces.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bcm_int/ltsw/mbcm/mbcm.h>
#include <bcm_int/ltsw/mbcm/switch.h>
#include <bcm_int/ltsw/switch_int.h>

#include <bsl/bsl.h>
#include <shr/shr_debug.h>

/******************************************************************************
 * Local definitions
 */

#define BSL_LOG_MODULE BSL_LS_BCM_SWITCH

static mbcm_ltsw_sc_drv_t sc_drv[BCM_MAX_NUM_UNITS];

/******************************************************************************
 * Public functions
 */

int
mbcm_ltsw_sc_drv_set(int unit, mbcm_ltsw_sc_drv_t *drv)
{
    mbcm_ltsw_sc_drv_t *local = &sc_drv[unit];

    mbcm_ltsw_drv_init((mbcm_ltsw_drv_t *)local,
                       (mbcm_ltsw_drv_t *)drv,
                       sizeof(*local)/MBCM_LTSW_FNPTR_SZ);

    return SHR_E_NONE;
}

int
mbcm_ltsw_scp_set(int unit, int port, bcm_switch_control_t type, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (sc_drv[unit].scp_set(unit, port, type, arg));

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_scp_get(int unit, int port, bcm_switch_control_t type, int *arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (sc_drv[unit].scp_get(unit, port, type, arg));

exit:
    SHR_FUNC_EXIT();
}


int
mbcm_ltsw_sc_set(int unit, bcm_switch_control_t type, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (sc_drv[unit].sc_set(unit, type, arg));

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_sc_get(int unit, bcm_switch_control_t type, int *arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (sc_drv[unit].sc_get(unit, type, arg));

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_spc_add(int unit, uint32_t options,
                  bcm_switch_pkt_protocol_match_t *match,
                  bcm_switch_pkt_control_action_t *action,
                  int priority)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (sc_drv[unit].spc_add(unit, options, match, action, priority));

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_spc_get(int unit,
                  bcm_switch_pkt_protocol_match_t *match,
                  bcm_switch_pkt_control_action_t *action,
                  int *priority)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (sc_drv[unit].spc_get(unit, match, action, priority));

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_spc_delete(int unit, bcm_switch_pkt_protocol_match_t *match)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (sc_drv[unit].spc_del(unit, match));

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_spc_traverse(int unit, bcm_switch_pkt_protocol_control_traverse_cb cb,
                       void *user_data)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (sc_drv[unit].spc_trav(unit, cb, user_data));

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_spc_delete_all(int unit)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (sc_drv[unit].spc_del_all(unit));

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_switch_pkt_integrity_check_add(int unit, uint32 options,
                                         bcm_switch_pkt_integrity_match_t *match,
                                         bcm_switch_pkt_control_action_t *action,
                                         int priority)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (sc_drv[unit].switch_pkt_integrity_check_add(unit, options, match,
                                                     action, priority));

exit:
    SHR_FUNC_EXIT();
}

extern int
mbcm_ltsw_switch_pkt_integrity_check_get(int unit,
                                         bcm_switch_pkt_integrity_match_t *match,
                                         bcm_switch_pkt_control_action_t *action,
                                         int *priority)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (sc_drv[unit].switch_pkt_integrity_check_get(unit, match, action, priority));

exit:
    SHR_FUNC_EXIT();
}

extern int
mbcm_ltsw_switch_pkt_integrity_check_delete(int unit,
                                            bcm_switch_pkt_integrity_match_t *match)

{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (sc_drv[unit].switch_pkt_integrity_check_delete(unit, match));

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_switch_drop_event_mon_set(int unit, bcm_switch_drop_event_mon_t *monitor)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (sc_drv[unit].switch_drop_event_mon_set(unit, monitor));

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_switch_drop_event_mon_get(int unit, bcm_switch_drop_event_mon_t *monitor)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (sc_drv[unit].switch_drop_event_mon_get(unit, monitor));

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_switch_trace_event_mon_set(int unit, bcm_switch_trace_event_mon_t *monitor)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (sc_drv[unit].switch_trace_event_mon_set(unit, monitor));

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_switch_trace_event_mon_get(int unit, bcm_switch_trace_event_mon_t *monitor)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (sc_drv[unit].switch_trace_event_mon_get(unit, monitor));

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_switch_drop_event_to_symbol(int unit, bcm_pkt_drop_event_t drop_event,
                                      const char **table, const char **symbol)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (sc_drv[unit].switch_drop_event_to_symbol(unit, drop_event, table, symbol));

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_switch_trace_event_to_symbol(int unit, bcm_pkt_trace_event_t trace_event,
                                       const char **table, const char **symbol)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (sc_drv[unit].switch_trace_event_to_symbol(unit, trace_event, table, symbol));

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_switch_db_get(int unit, bcmint_switch_db_t *switch_db)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (sc_drv[unit].switch_db_get(unit, switch_db));

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_switch_protocol_init(int unit)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (sc_drv[unit].switch_protocol_init(unit));

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_switch_protocol_handle(int unit, bcmlt_opcode_t opcode,
                                 bcm_switch_pkt_protocol_match_t *match,
                                 bcm_switch_pkt_control_action_t *action,
                                 int *priority)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (sc_drv[unit].switch_protocol_handle(unit, opcode, match, action,
                                             priority));

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_switch_pkt_integrity_check_init(int unit)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (sc_drv[unit].switch_pkt_integrity_check_init(unit));

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_switch_pkt_integrity_handle(int unit, bcmlt_opcode_t opcode,
                                      bcm_switch_pkt_integrity_match_t *match,
                                      bcm_switch_pkt_control_action_t *action,
                                      int *priority)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (sc_drv[unit].switch_pkt_integrity_handle(unit, opcode, match, action,
                                                  priority));

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_switch_mon_trace_event_init(int unit)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (sc_drv[unit].switch_mon_trace_event_init(unit));

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_switch_init(int unit)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (sc_drv[unit].switch_init(unit));

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_switch_detach(int unit)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (sc_drv[unit].switch_detach(unit));

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_switch_ep_recirc_profile_create(
    int unit,
    bcm_switch_ep_recirc_profile_t *recirc_profile,
    int *recirc_profile_id)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (sc_drv[unit].switch_ep_recirc_profile_create(unit, recirc_profile,
                                                      recirc_profile_id));

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_switch_ep_recirc_profile_destroy(
    int unit,
    int recirc_profile_id)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (sc_drv[unit].switch_ep_recirc_profile_destroy(unit, recirc_profile_id));

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_switch_ep_recirc_profile_get_all(
    int unit,
    int *recirc_profile_id_array,
    int *count)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (sc_drv[unit].switch_ep_recirc_profile_get_all(
                          unit, recirc_profile_id_array, count));

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_switch_ep_recirc_profile_get(
    int unit,
    int recirc_profile_id,
    bcm_switch_ep_recirc_profile_t *recirc_profile)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (sc_drv[unit].switch_ep_recirc_profile_get(
                          unit, recirc_profile_id, recirc_profile));

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_switch_ep_recirc_drop_event_control_set(
    int unit,
    bcm_switch_ep_recirc_drop_event_control_t *drop_event_ctrl)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (sc_drv[unit].switch_ep_recirc_drop_event_control_set(unit,
                                                              drop_event_ctrl));

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_switch_ep_recirc_drop_event_control_get(
    int unit,
    bcm_switch_ep_recirc_drop_event_control_t *drop_event_ctrl)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (sc_drv[unit].switch_ep_recirc_drop_event_control_get(unit,
                                                              drop_event_ctrl));

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_switch_ep_recirc_trace_event_control_set(
    int unit,
    bcm_switch_ep_recirc_trace_event_control_t *trace_event_ctrl)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (sc_drv[unit].switch_ep_recirc_trace_event_control_set(unit,
                                                               trace_event_ctrl));

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_switch_ep_recirc_trace_event_control_get(
    int unit,
    bcm_switch_ep_recirc_trace_event_control_t *trace_event_ctrl)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (sc_drv[unit].switch_ep_recirc_trace_event_control_get(unit,
                                                               trace_event_ctrl));

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_switch_ep_recirc_trace_event_enable_set(
    int unit,
    bcm_pkt_trace_event_t trace_event,
    int enable)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (sc_drv[unit].switch_ep_recirc_trace_event_enable_set(unit,
                                                              trace_event,
                                                              enable));

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_switch_ep_recirc_trace_event_enable_get_all(
    int unit,
    bcm_pkt_trace_event_t *trace_event_array,
    int *count)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (sc_drv[unit].switch_ep_recirc_trace_event_enable_get_all(unit,
                                                                  trace_event_array,
                                                                  count));

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_switch_ep_recirc_trace_event_enable_get(
    int unit,
    bcm_pkt_trace_event_t trace_event,
    int *enable)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (sc_drv[unit].switch_ep_recirc_trace_event_enable_get(unit,
                                                              trace_event,
                                                              enable));

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_switch_ep_recirc_profile_mirror_instance_set(
    int unit,
    int recirc_profile_id,
    int mirror_instance_id)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (sc_drv[unit].switch_ep_recirc_profile_mirror_instance_set(unit,
                                                                   recirc_profile_id,
                                                                   mirror_instance_id));

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_switch_ep_recirc_profile_mirror_instance_get(
    int unit,
    int *mirror_instance_id)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (sc_drv[unit].switch_ep_recirc_profile_mirror_instance_get(unit,
                                                                   mirror_instance_id));

exit:
    SHR_FUNC_EXIT();
}

