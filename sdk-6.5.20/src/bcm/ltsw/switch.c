/*! \file switch.c
 *
 * Switch control interfaces.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bcm_int/ltsw/switch.h>
#include <bcm_int/ltsw/l2.h>
#include <bcm_int/ltsw/l3.h>
#include <bcm_int/ltsw/l3_fib.h>
#include <bcm_int/ltsw/l3_egress.h>
#include <bcm_int/ltsw/vlan.h>
#include <bcm_int/ltsw/uft.h>
#include <bcm_int/ltsw/port.h>
#include <bcm_int/ltsw/obm.h>
#include <bcm_int/ltsw/mbcm/switch.h>
#include <bcm_int/ltsw/mbcm/l3.h>
#include <bcm_int/ltsw/dev.h>
#include <bcm_int/ltsw/lb_hash.h>
#include <bcm_int/ltsw/virtual.h>

#include <sal/sal_mutex.h>
#include <shr/shr_debug.h>
#include <bcmbd/bcmbd_led.h>
#include <bcmbd/bcmbd_led_internal.h>
#include <bcmbd/bcmbd_pvt.h>

/******************************************************************************
 * Local definitions
 */

#define BSL_LOG_MODULE BSL_LS_BCM_SWITCH

/*!
 * \brief Switch event callback information structure.
 */
typedef struct switch_event_cb_info_s {

    /*! Next callback info handle. */
    struct switch_event_cb_info_s *next;

    /*! Callback function. */
    bcm_switch_event_cb_t cb_func;

    /*! User-provided context. */
    void *userdata;

} switch_event_cb_info_t;

typedef struct switch_ctrl_s {

    /*! True if switch module is initialized. */
    bool inited;

    /*! Mutex lock for inserting/removing callback entry from the list.*/
    sal_mutex_t mutex;

    /*! List of registered callback functions. */
    switch_event_cb_info_t *event_cbs;

    /*! The total number of registered callback functions. */
    int cbs_cnt;

} switch_ctrl_t;

static switch_ctrl_t switch_ctrl_info[BCM_MAX_NUM_UNITS];

/*******************************************************************************
 * Public Internal HSDK functions
 */
/*!
 * \brief Initializes the switch module.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
int
bcmi_ltsw_switch_init(int unit)
{
    switch_ctrl_t *sc_info;

    SHR_FUNC_ENTER(unit);

    if (!bcmi_ltsw_dev_exists(unit)) {
        SHR_ERR_EXIT(SHR_E_UNIT);
    }

    sc_info = &switch_ctrl_info[unit];

    if (sc_info->inited) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_switch_detach(unit));
    }

    if (sc_info->mutex == NULL) {
        sc_info->mutex = sal_mutex_create("ltswSwitchCtrlMutex");
        SHR_NULL_CHECK(sc_info->mutex, SHR_E_MEMORY);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_switch_init(unit));

    sc_info->inited = TRUE;

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief De-initializes the switch module.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
int
bcmi_ltsw_switch_detach(int unit)
{
    switch_ctrl_t *sc_info;
    switch_event_cb_info_t *cb_info, *curr;

    SHR_FUNC_ENTER(unit);

    if (!bcmi_ltsw_dev_exists(unit)) {
        SHR_ERR_EXIT(SHR_E_UNIT);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_switch_detach(unit));

    sc_info = &switch_ctrl_info[unit];

    if (sc_info->inited == FALSE) {
        SHR_EXIT();
    }
    if (!sc_info->mutex) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INIT);
    }

    sal_mutex_take(sc_info->mutex, SAL_MUTEX_FOREVER);

    /* Unregister all callback functions. */
    curr = sc_info->event_cbs;
    while ((cb_info = curr) != NULL) {
        curr = curr->next;
        SHR_FREE(cb_info);
    }

    sc_info->inited = FALSE;
    sc_info->event_cbs = NULL;
    sc_info->cbs_cnt = 0;

    sal_mutex_give(sc_info->mutex);
    sal_mutex_destroy(sc_info->mutex);
    sc_info->mutex = NULL;

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Generate switch event to application.
 *
 * When the function is called, all callback functions are executed.
 *
 * \param [in] unit Unit number.
 * \param [in] event Switch event.
 * \param [in] arg1 The first callback function argument.
 * \param [in] arg2 The second callback function argument.
 * \param [in] arg3 The third callback function argument.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
int
bcmi_ltsw_switch_event_generate(int unit,  bcm_switch_event_t event,
                                uint32 arg1, uint32 arg2, uint32 arg3)
{
    switch_event_cb_info_t *curr = NULL;

    if (!bcmi_ltsw_dev_exists(unit)) {
        return SHR_E_UNIT;
    }

    curr = switch_ctrl_info[unit].event_cbs;
    while (curr != NULL) {
        if (curr->cb_func) {
            curr->cb_func(unit, event, arg1, arg2, arg3, curr->userdata);
        }
        curr = curr->next;
    }
    return SHR_E_NONE;
}

/*!
 * \brief Covert a drop event enum value to Logical Table symbol string.
 *
 * \param [in] unit Unit number.
 * \param [in] drop_event Drop event enum value.
 * \param [out] table Logical Table name.
 * \param [out] symbol Logical Table symbol string.
 *
 * \retval SHR_E_NONE Success.
 * \retval SHR_E_PARAM Invalid switch drop event enum value.
 * \retval SHR_E_NOT_FOUND Symbol string not found for the specified drop event.
 */
int
bcmi_ltsw_switch_drop_event_to_symbol(int unit, bcm_pkt_drop_event_t drop_event,
                                      const char **table, const char **symbol)
{
    return mbcm_ltsw_switch_drop_event_to_symbol(unit, drop_event, table, symbol);
}

/*!
 * \brief Covert a trace event enum value to Logical Table symbol string.
 *
 * \param [in] unit Unit number.
 * \param [in] trace_event Trace event enum value.
 * \param [out] table Logical Table name.
 * \param [out] symbol Logical Table symbol string.
 *
 * \retval SHR_E_NONE Success.
 * \retval SHR_E_PARAM Invalid switch trace event enum value.
 * \retval SHR_E_NOT_FOUND Symbol string not found for the specified trace event.
 */
int
bcmi_ltsw_switch_trace_event_to_symbol(int unit, bcm_pkt_trace_event_t trace_event,
                                       const char **table, const char **symbol)
{
    return mbcm_ltsw_switch_trace_event_to_symbol(unit, trace_event, table, symbol);
}

/*!
 * \brief Set a mirror instance id to an EP recirculate profile entry.
 *
 * \param [in] unit Unit number.
 * \param [in] recirc_profile_id EP Recirculate profile id.
 * \param [in] mirror_instance_id Mirror instance id.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
int
bcmi_ltsw_switch_ep_recirc_profile_mirror_instance_set(
    int unit,
    int recirc_profile_id,
    int mirror_instance_id)
{
    return mbcm_ltsw_switch_ep_recirc_profile_mirror_instance_set(
               unit,
               recirc_profile_id,
               mirror_instance_id);
}

/*!
 * \brief Get the mirror instance id from EP recirculate profile entries.
 *
 * \param [in] unit Unit number.
 * \param [out] mirror_instance_id Mirror instance id.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
int
bcmi_ltsw_switch_ep_recirc_profile_mirror_instance_get(
    int unit,
    int *mirror_instance_id)
{
    return mbcm_ltsw_switch_ep_recirc_profile_mirror_instance_get(
               unit,
               mirror_instance_id);
}

/*******************************************************************************
 * Public HSDK functions
 */
int
bcm_ltsw_switch_control_port_set(int unit, bcm_port_t port,
                                 bcm_switch_control_t type, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_gport_validate(unit, port, &port));

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_scp_set(unit, port, type, arg));

exit:
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_switch_control_port_get(int unit, bcm_port_t port,
                                 bcm_switch_control_t type, int *arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_gport_validate(unit, port, &port));

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_scp_get(unit, port, type, arg));

exit:
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_switch_control_set(int unit, bcm_switch_control_t type, int arg)
{
    return mbcm_ltsw_sc_set(unit, type, arg);
}

int
bcm_ltsw_switch_control_get(int unit, bcm_switch_control_t type, int *arg)
{
    return mbcm_ltsw_sc_get(unit, type, arg);
}

int
bcm_ltsw_switch_pkt_protocol_control_add(int unit, uint32_t options,
                                         bcm_switch_pkt_protocol_match_t *match,
                                         bcm_switch_pkt_control_action_t *action,
                                         int priority)
{
    return mbcm_ltsw_spc_add(unit, options, match, action, priority);
}

int
bcm_ltsw_switch_pkt_protocol_control_get(int unit,
                                         bcm_switch_pkt_protocol_match_t *match,
                                         bcm_switch_pkt_control_action_t *action,
                                         int *priority)
{
    return mbcm_ltsw_spc_get(unit, match, action, priority);
}

int
bcm_ltsw_switch_pkt_protocol_control_delete(int unit,
                                            bcm_switch_pkt_protocol_match_t *match)
{
    return mbcm_ltsw_spc_delete(unit, match);
}

int
bcm_ltsw_switch_pkt_protocol_control_traverse(int unit,
    bcm_switch_pkt_protocol_control_traverse_cb cb,
    void *user_data)
{
    return mbcm_ltsw_spc_traverse(unit, cb, user_data);
}

int
bcm_ltsw_switch_pkt_protocol_control_delete_all(int unit)
{
    return mbcm_ltsw_spc_delete_all(unit);
}

/*!
 * \brief Add an entry into packet integrity check table.
 *
 * \param [in] unit Unit number.
 * \param [in] option Options flag.
 * \param [in] match Pointer to a packet integrity match attribute structure.
 * \param [in] action Pointer to a packet control action structure.
 * \param [in] priority Entry priority.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
int
bcm_ltsw_switch_pkt_integrity_check_add(int unit, uint32 options,
                                        bcm_switch_pkt_integrity_match_t *match,
                                        bcm_switch_pkt_control_action_t *action,
                                        int priority)
{
    return mbcm_ltsw_switch_pkt_integrity_check_add(unit, options, match, action,
                                                    priority);
}

/*!
 * \brief Get an entry from packet integrity check table.
 *
 * \param [in] unit Unit number.
 * \param [in] match Pointer to a packet integrity match attribute structure.
 * \param [out] action Pointer to a packet control action structure.
 * \param [out] priority Entry priority.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
int
bcm_ltsw_switch_pkt_integrity_check_get(int unit,
                                        bcm_switch_pkt_integrity_match_t *match,
                                        bcm_switch_pkt_control_action_t *action,
                                        int *priority)
{
    return mbcm_ltsw_switch_pkt_integrity_check_get(unit, match, action, priority);
}

/*!
 * \brief Delete an entry from packet integrity check table.
 *
 * \param [in] unit Unit number.
 * \param [in] match Pointer to a packet integrity match structure.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
int
bcm_ltsw_switch_pkt_integrity_check_delete(int unit,
                                           bcm_switch_pkt_integrity_match_t *match)
{
    return mbcm_ltsw_switch_pkt_integrity_check_delete(unit, match);
}

/*!
 * \brief Registers a callback function for switch critical events.
 *
 * Several callback functions could be registered, they all will be called upon critical event in
 * the reverse order they are registered. If registered callback is called it is advised to log the
 * information and reset the chip. Same callback function with different userdata is allowed to
 * be registered.
 *
 * \param [in] unit Unit number.
 * \param [in] cb Callback function.
 * \param [in] userdata User-provided context.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
int
bcm_ltsw_switch_event_register(int unit, bcm_switch_event_cb_t cb,
                               void *userdata)
{
    bool locked = FALSE;
    switch_ctrl_t *sc_info;
    switch_event_cb_info_t *curr = NULL;
    switch_event_cb_info_t *cb_info = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(cb, SHR_E_PARAM);
    if (!bcmi_ltsw_dev_exists(unit)) {
        SHR_ERR_EXIT(SHR_E_UNIT);
    }

    sc_info = &switch_ctrl_info[unit];
    curr = sc_info->event_cbs;

    SHR_IF_ERR_EXIT
        (sal_mutex_take(sc_info->mutex, SAL_MUTEX_FOREVER));
    locked = TRUE;

    /* Check the callback function already in the list. */
    while (curr != NULL) {
        if ((cb == curr->cb_func) && (curr->userdata == userdata)) {
            LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_U(unit,
                                    "Callback function already registered.\n")));
            SHR_EXIT();
        }
        curr = curr->next;
    }

    SHR_ALLOC(cb_info, sizeof(switch_event_cb_info_t), "LtswSwitchCbInfo");
    SHR_NULL_CHECK(cb_info, SHR_E_MEMORY);
    cb_info->cb_func = cb;
    cb_info->userdata = userdata;

    /* Insert the entry in the head of the list. */
    cb_info->next = sc_info->event_cbs;
    sc_info->event_cbs = cb_info;
    sc_info->cbs_cnt++;
    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "Register callback function successful.\n")));

exit:
    if (locked) {
        sal_mutex_give(sc_info->mutex);
    }
    if (SHR_FUNC_ERR()) {
        SHR_FREE(cb_info);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Unregisters a callback function for switch critical events.
 *
 * If userdata = NULL then all matched callback functions will be unregistered.
 *
 * \param [in] unit Unit number.
 * \param [in] cb Callback function.
 * \param [in] userdata User-provided context.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
int
bcm_ltsw_switch_event_unregister(int unit, bcm_switch_event_cb_t cb,
                                 void *userdata)
{
    bool locked = FALSE;
    switch_ctrl_t *sc_info;
    switch_event_cb_info_t **curr = NULL;
    switch_event_cb_info_t *cb_info = NULL;
    int i;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(cb, SHR_E_PARAM);
    if (!bcmi_ltsw_dev_exists(unit)) {
        SHR_ERR_EXIT(SHR_E_UNIT);
    }

    sc_info = &switch_ctrl_info[unit];
    curr = &sc_info->event_cbs;

    SHR_IF_ERR_EXIT
        (sal_mutex_take(sc_info->mutex, SAL_MUTEX_FOREVER));
    locked = TRUE;

    i = 0;
    while ((*curr) != NULL) {
        cb_info = *curr;
        if (cb_info->cb_func == cb) {
            if (userdata == NULL) {
                /* Remove all matched entries from the list. */
                *curr = (*curr)->next;
                SHR_FREE(cb_info);
                sc_info->cbs_cnt--;
                LOG_VERBOSE(BSL_LOG_MODULE,
                            (BSL_META_U(unit,
                                        "Unregister callback(%d) successful.\n"),
                             i));
                i++;
                continue;
            }

            /* Remove the single entry from the list. */
            if (cb_info->userdata == userdata) {
                *curr = (*curr)->next;
                SHR_FREE(cb_info);
                sc_info->cbs_cnt--;
                LOG_VERBOSE(BSL_LOG_MODULE,
                            (BSL_META_U(unit,
                                        "Unregister single callback successful.\n")));
                break;
            }
        }
        curr = &(*curr)->next;
    }

exit:
    if (locked) {
        sal_mutex_give(sc_info->mutex);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Configure a switch drop event monitor.
 *
 * This function configures monitor for a given switch drop event. The caller must specify
 * a switch drop event and the associated actions in the monitor structure.
 *
 * \param [in] unit Unit number.
 * \param [in] monitor Pointer to a switch drop event monitor structure.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
int
bcm_ltsw_switch_drop_event_mon_set(int unit, bcm_switch_drop_event_mon_t *monitor)
{
    return mbcm_ltsw_switch_drop_event_mon_set(unit, monitor);
}

/*!
 * \brief Get a switch drop event monitor configuration.
 *
 * This function get monitor configuration for a given switch drop event. The caller must specify
 * a drop event in the monitor structure. The associated actions will be returned.
 *
 * \param [in] unit Unit number.
 * \param [in/out] monitor Pointer to a switch drop event monitor structure.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
int
bcm_ltsw_switch_drop_event_mon_get(int unit, bcm_switch_drop_event_mon_t *monitor)
{
    return mbcm_ltsw_switch_drop_event_mon_get(unit, monitor);
}

/*!
 * \brief Configure a switch trace event monitor.
 *
 * This function configures monitor for a given switch trace event. The caller must specify a
 * switch trace event and the associated actions in the monitor structure.
 *
 * \param [in] unit Unit number.
 * \param [in] monitor Pointer to a switch trace event monitor structure.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
int
bcm_ltsw_switch_trace_event_mon_set(int unit, bcm_switch_trace_event_mon_t *monitor)
{
    return mbcm_ltsw_switch_trace_event_mon_set(unit, monitor);
}

/*!
 * \brief Get a switch trace event monitor configuration.
 *
 * This function get monitor configuration for a given switch trace event. The caller must specify
 * a trace event in the monitor structure. The associated actions will be returned.
 *
 * \param [in] unit Unit number.
 * \param [in/out] monitor Pointer to a switch trace event monitor structure.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
int
bcm_ltsw_switch_trace_event_mon_get(int unit, bcm_switch_trace_event_mon_t *monitor)
{
    return mbcm_ltsw_switch_trace_event_mon_get(unit, monitor);
}

/*!
 * \brief Retrieve the number of API objects.
 *
 * This function is used to get the number of API objects currently residing in the system.
 *
 * \param [in] unit Unit number.
 * \param [in] object Object name for which the count is to be retrieved.
 * \param [out] entries The number of entries in the system.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
int
bcm_ltsw_switch_object_count_get(int unit, bcm_switch_object_t object,
                                 int *entries)
{
    int cnt;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(entries, SHR_E_PARAM);
    if (!bcmi_ltsw_dev_exists(unit)) {
        SHR_ERR_EXIT(SHR_E_UNIT);
    }
    *entries = 0;

    switch (object) {
        case bcmSwitchObjectL2EntryCurrent:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_l2_count_get(unit, entries));
            break;
        case bcmSwitchObjectL2EntryMax:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_l2_size_get(unit, entries));
            break;
        case bcmSwitchObjectVlanCurrent:
             SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_vlan_count_get(unit, entries));
            break;
        case bcmSwitchObjectL3HostCurrent:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_l3_fib_object_cnt_get(unit,
                                         BCMI_LTSW_L3_FIB_OBJ_HOST_USED,
                                         BCMI_LTSW_L3_FIB_OBJ_F_BASE_ENTRIES,
                                         entries));
            break;
        case bcmSwitchObjectL3HostV4Max:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_l3_fib_object_cnt_get(unit,
                                         BCMI_LTSW_L3_FIB_OBJ_HOST_V4_MAX,
                                         BCMI_LTSW_L3_FIB_OBJ_F_BASE_ENTRIES,
                                         entries));
            break;
        case bcmSwitchObjectL3HostV4Used:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_l3_fib_object_cnt_get(unit,
                                         BCMI_LTSW_L3_FIB_OBJ_HOST_V4_USED,
                                         BCMI_LTSW_L3_FIB_OBJ_F_BASE_ENTRIES,
                                         entries));
            break;
        case bcmSwitchObjectL3HostV6Max:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_l3_fib_object_cnt_get(unit,
                                         BCMI_LTSW_L3_FIB_OBJ_HOST_V6_MAX,
                                         BCMI_LTSW_L3_FIB_OBJ_F_BASE_ENTRIES,
                                         entries));
            break;
        case bcmSwitchObjectL3HostV6Used:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_l3_fib_object_cnt_get(unit,
                                         BCMI_LTSW_L3_FIB_OBJ_HOST_V6_USED,
                                         BCMI_LTSW_L3_FIB_OBJ_F_BASE_ENTRIES,
                                         entries));
            break;
        case bcmSwitchObjectIpmcCurrent:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_l3_fib_object_cnt_get(unit,
                                         BCMI_LTSW_L3_FIB_OBJ_HOST_MC_USED,
                                         0,
                                         entries));
            break;
        case bcmSwitchObjectIpmcV4Max:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_l3_fib_object_cnt_get(unit,
                                         BCMI_LTSW_L3_FIB_OBJ_HOST_MC_V4_MAX,
                                         0,
                                         entries));
            break;
        case bcmSwitchObjectIpmcV4Used:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_l3_fib_object_cnt_get(unit,
                                         BCMI_LTSW_L3_FIB_OBJ_HOST_MC_V4_USED,
                                         0,
                                         entries));
            break;
        case bcmSwitchObjectIpmcV6Max:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_l3_fib_object_cnt_get(unit,
                                         BCMI_LTSW_L3_FIB_OBJ_HOST_MC_V6_MAX,
                                         0,
                                         entries));
            break;
        case bcmSwitchObjectIpmcV6Used:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_l3_fib_object_cnt_get(unit,
                                         BCMI_LTSW_L3_FIB_OBJ_HOST_MC_V6_USED,
                                         0,
                                         entries));
            break;
        case bcmSwitchObjectL3HostUcV4Max:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_l3_fib_object_cnt_get(unit,
                                         BCMI_LTSW_L3_FIB_OBJ_HOST_UC_V4_MAX,
                                         0,
                                         entries));
            break;
        case bcmSwitchObjectL3HostUcV6Max:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_l3_fib_object_cnt_get(unit,
                                         BCMI_LTSW_L3_FIB_OBJ_HOST_UC_V6_MAX,
                                         0,
                                         entries));
            break;
        case bcmSwitchObjectL3HostUcV4Used:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_l3_fib_object_cnt_get(unit,
                                         BCMI_LTSW_L3_FIB_OBJ_HOST_UC_V4_USED,
                                         0,
                                         entries));
            break;
        case bcmSwitchObjectL3HostUcV6Used:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_l3_fib_object_cnt_get(unit,
                                         BCMI_LTSW_L3_FIB_OBJ_HOST_UC_V6_USED,
                                         0,
                                         entries));
            break;
        case bcmSwitchObjectIpmcSrcGroupV4Max:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_l3_fib_object_cnt_get(unit,
                                         BCMI_LTSW_L3_FIB_OBJ_HOST_MC_SG_V4_MAX,
                                         0,
                                         entries));
            break;
        case bcmSwitchObjectIpmcSrcGroupV6Max:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_l3_fib_object_cnt_get(unit,
                                         BCMI_LTSW_L3_FIB_OBJ_HOST_MC_SG_V6_MAX,
                                         0,
                                         entries));
            break;
        case bcmSwitchObjectIpmcSrcGroupV4Used:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_l3_fib_object_cnt_get(unit,
                                         BCMI_LTSW_L3_FIB_OBJ_HOST_MC_SG_V4_USED,
                                         0,
                                         entries));
            break;
        case bcmSwitchObjectIpmcSrcGroupV6Used:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_l3_fib_object_cnt_get(unit,
                                         BCMI_LTSW_L3_FIB_OBJ_HOST_MC_SG_V6_USED,
                                         0,
                                         entries));
            break;
        case bcmSwitchObjectIpmcGroupV4Max:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_l3_fib_object_cnt_get(unit,
                                         BCMI_LTSW_L3_FIB_OBJ_HOST_MC_G_V4_MAX,
                                         0,
                                         entries));
            break;
        case bcmSwitchObjectIpmcGroupV6Max:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_l3_fib_object_cnt_get(unit,
                                         BCMI_LTSW_L3_FIB_OBJ_HOST_MC_G_V6_MAX,
                                         0,
                                         entries));
            break;
        case bcmSwitchObjectIpmcGroupV4Used:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_l3_fib_object_cnt_get(unit,
                                         BCMI_LTSW_L3_FIB_OBJ_HOST_MC_G_V4_USED,
                                         0,
                                         entries));
            break;
        case bcmSwitchObjectIpmcGroupV6Used:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_l3_fib_object_cnt_get(unit,
                                         BCMI_LTSW_L3_FIB_OBJ_HOST_MC_G_V6_USED,
                                         0,
                                         entries));
            break;
        case bcmSwitchObjectL3RouteCurrent:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_l3_fib_object_cnt_get(unit,
                                         BCMI_LTSW_L3_FIB_OBJ_ROUTE_USED,
                                         BCMI_LTSW_L3_FIB_OBJ_F_BASE_ENTRIES,
                                         entries));
            break;
        case bcmSwitchObjectL3RouteV4RoutesMax:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_l3_fib_object_cnt_get(unit,
                                         BCMI_LTSW_L3_FIB_OBJ_ROUTE_V4_MAX,
                                         0,
                                         entries));
            break;
        case bcmSwitchObjectL3RouteV4RoutesUsed:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_l3_fib_object_cnt_get(unit,
                                         BCMI_LTSW_L3_FIB_OBJ_ROUTE_V4_USED,
                                         0,
                                         entries));
            break;
        case bcmSwitchObjectL3RouteV6Routes64bMax:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_l3_fib_object_cnt_get(unit,
                                         BCMI_LTSW_L3_FIB_OBJ_ROUTE_V6_64_MAX,
                                         0,
                                         entries));
            break;
        case bcmSwitchObjectL3RouteV6Routes128bMax:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_l3_fib_object_cnt_get(unit,
                                         BCMI_LTSW_L3_FIB_OBJ_ROUTE_V6_128_MAX,
                                         0,
                                         entries));
            break;
        case bcmSwitchObjectL3RouteV6Routes64bUsed:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_l3_fib_object_cnt_get(unit,
                                         BCMI_LTSW_L3_FIB_OBJ_ROUTE_V6_64_USED,
                                         0,
                                         entries));
            break;
        case bcmSwitchObjectL3RouteV6Routes128bUsed:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_l3_fib_object_cnt_get(unit,
                                         BCMI_LTSW_L3_FIB_OBJ_ROUTE_V6_128_USED,
                                         0,
                                         entries));
            break;
        case bcmSwitchObjectL3RouteTotalUsedRoutes:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_l3_fib_object_cnt_get(unit,
                                         BCMI_LTSW_L3_FIB_OBJ_ROUTE_USED,
                                         0,
                                         entries));
            break;
        case bcmSwitchObjectL3EgressCurrent:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_l3_egress_obj_cnt_get(unit,
                                                 BCMI_LTSW_L3_EGR_OBJ_T_OL,
                                                 0, &cnt));
            *entries = cnt;
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_l3_egress_obj_cnt_get(unit,
                                                 BCMI_LTSW_L3_EGR_OBJ_T_UL,
                                                 0, &cnt));
            *entries += cnt;
            break;
        case bcmSwitchObjectL3EgressMax:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_l3_egress_obj_cnt_get(unit,
                                                 BCMI_LTSW_L3_EGR_OBJ_T_OL,
                                                 &cnt, 0));
            *entries = cnt;
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_l3_egress_obj_cnt_get(unit,
                                                 BCMI_LTSW_L3_EGR_OBJ_T_UL,
                                                 &cnt, 0));
            *entries += cnt;
            break;
        case bcmSwitchObjectEcmpCurrent:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_l3_egress_obj_cnt_get(unit,
                                                 BCMI_LTSW_L3_EGR_OBJ_T_ECMP_OL,
                                                 0, &cnt));
            *entries = cnt;
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_l3_egress_obj_cnt_get(unit,
                                                 BCMI_LTSW_L3_EGR_OBJ_T_ECMP_UL,
                                                 0, &cnt));
            *entries += cnt;
            break;
        case bcmSwitchObjectEcmpMax:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_l3_egress_obj_cnt_get(unit,
                                                 BCMI_LTSW_L3_EGR_OBJ_T_ECMP_OL,
                                                 &cnt, 0));
            *entries = cnt;
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_l3_egress_obj_cnt_get(unit,
                                                 BCMI_LTSW_L3_EGR_OBJ_T_ECMP_UL,
                                                 &cnt, 0));
            *entries += cnt;
            break;
        case bcmSwitchObjectL3RoutePrivateV4RouteProjection:
            if (SHR_FAILURE(bcmi_ltsw_l3_route_grp_projection(unit,
                                                   bcmL3RouteGroupPrivateV4,
                                                   entries, NULL, NULL))) {
                SHR_IF_ERR_EXIT
                    (bcmi_ltsw_l3_route_grp_testdb(unit,
                                                   bcmL3RouteGroupPrivateV4, 0,
                                                   entries, NULL, NULL, NULL));
            }
            break;
        case bcmSwitchObjectL3RouteGlobalV4RouteProjection:
            if (SHR_FAILURE(bcmi_ltsw_l3_route_grp_projection(unit,
                                                   bcmL3RouteGroupGlobalV4,
                                                   entries, NULL, NULL))) {
                SHR_IF_ERR_EXIT
                    (bcmi_ltsw_l3_route_grp_testdb(unit,
                                                   bcmL3RouteGroupGlobalV4, 0,
                                                   entries, NULL, NULL, NULL));
            }
            break;
        case bcmSwitchObjectL3RouteOverrideV4RouteProjection:
            if (SHR_FAILURE(bcmi_ltsw_l3_route_grp_projection(unit,
                                                   bcmL3RouteGroupOverrideV4,
                                                   entries, NULL, NULL))) {
                SHR_IF_ERR_EXIT
                    (bcmi_ltsw_l3_route_grp_testdb(unit,
                                                   bcmL3RouteGroupOverrideV4, 0,
                                                   entries, NULL, NULL, NULL));
            }
            break;
        case bcmSwitchObjectL3RoutePrivateV6RouteProjection:
            if (SHR_FAILURE(bcmi_ltsw_l3_route_grp_projection(unit,
                                                   bcmL3RouteGroupPrivateV6,
                                                   entries, NULL, NULL))) {
                SHR_IF_ERR_EXIT
                    (bcmi_ltsw_l3_route_grp_testdb(unit,
                                                   bcmL3RouteGroupPrivateV6, 0,
                                                   entries, NULL, NULL, NULL));
            }
            break;
        case bcmSwitchObjectL3RouteGlobalV6RouteProjection:
            if (SHR_FAILURE(bcmi_ltsw_l3_route_grp_projection(unit,
                                                   bcmL3RouteGroupGlobalV6,
                                                   entries, NULL, NULL))) {
                SHR_IF_ERR_EXIT
                    (bcmi_ltsw_l3_route_grp_testdb(unit,
                                                   bcmL3RouteGroupGlobalV6, 0,
                                                   entries, NULL, NULL, NULL));
            }
            break;
        case bcmSwitchObjectL3RouteOverrideV6RouteProjection:
            if (SHR_FAILURE(bcmi_ltsw_l3_route_grp_projection(unit,
                                                   bcmL3RouteGroupOverrideV6,
                                                   entries, NULL, NULL))) {
                SHR_IF_ERR_EXIT
                    (bcmi_ltsw_l3_route_grp_testdb(unit,
                                                   bcmL3RouteGroupOverrideV6, 0,
                                                   entries, NULL, NULL, NULL));
            }
            break;
        default:
            SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Retrieve the number of API objects.
 *
 * This function is similar to bcm_switch_object_count_get except for
 * retrieving for multiple objects.
 *
 * \param [in] unit Unit number.
 * \param [in] object_size Number of objects for which the entries are to be retrieved.
 * \param [in] object_array Object names for which the count is to be retrieved.
 * \param [out] entries The number of entries for each object in array.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
int
bcm_ltsw_switch_object_count_multi_get(int unit, int object_size,
                                       bcm_switch_object_t *object_array,
                                       int *entries)
{
    int index = 0;
    int cnt;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(entries, SHR_E_PARAM);
    SHR_NULL_CHECK(object_array, SHR_E_PARAM);
    if (!bcmi_ltsw_dev_exists(unit)) {
        SHR_ERR_EXIT(SHR_E_UNIT);
    }
    if (object_size <= 0) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    for (index = 0; index < object_size; index++) {
        entries[index] = 0;
        switch (object_array[index]) {
            case bcmSwitchObjectL2EntryCurrent:
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmi_ltsw_l2_count_get(unit, &entries[index]));
                break;
            case bcmSwitchObjectL2EntryMax:
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmi_ltsw_l2_size_get(unit, &entries[index]));
                break;
            case bcmSwitchObjectVlanCurrent:
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmi_ltsw_vlan_count_get(unit, &entries[index]));
                break;
            case bcmSwitchObjectL3HostCurrent:
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmi_ltsw_l3_fib_object_cnt_get(unit,
                                         BCMI_LTSW_L3_FIB_OBJ_HOST_USED,
                                         BCMI_LTSW_L3_FIB_OBJ_F_BASE_ENTRIES,
                                         &entries[index]));
                break;
            case bcmSwitchObjectL3HostV4Max:
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmi_ltsw_l3_fib_object_cnt_get(unit,
                                         BCMI_LTSW_L3_FIB_OBJ_HOST_V4_MAX,
                                         BCMI_LTSW_L3_FIB_OBJ_F_BASE_ENTRIES,
                                         &entries[index]));
                break;
            case bcmSwitchObjectL3HostV4Used:
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmi_ltsw_l3_fib_object_cnt_get(unit,
                                         BCMI_LTSW_L3_FIB_OBJ_HOST_V4_USED,
                                         BCMI_LTSW_L3_FIB_OBJ_F_BASE_ENTRIES,
                                         &entries[index]));
                break;
            case bcmSwitchObjectL3HostV6Max:
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmi_ltsw_l3_fib_object_cnt_get(unit,
                                         BCMI_LTSW_L3_FIB_OBJ_HOST_V6_MAX,
                                         BCMI_LTSW_L3_FIB_OBJ_F_BASE_ENTRIES,
                                         &entries[index]));
            break;
            case bcmSwitchObjectL3HostV6Used:
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmi_ltsw_l3_fib_object_cnt_get(unit,
                                         BCMI_LTSW_L3_FIB_OBJ_HOST_V6_USED,
                                         BCMI_LTSW_L3_FIB_OBJ_F_BASE_ENTRIES,
                                         &entries[index]));
                break;
            case bcmSwitchObjectIpmcCurrent:
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmi_ltsw_l3_fib_object_cnt_get(unit,
                                         BCMI_LTSW_L3_FIB_OBJ_HOST_MC_USED,
                                         0,
                                         &entries[index]));
                break;
            case bcmSwitchObjectIpmcV4Max:
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmi_ltsw_l3_fib_object_cnt_get(unit,
                                         BCMI_LTSW_L3_FIB_OBJ_HOST_MC_V4_MAX,
                                         0,
                                         &entries[index]));
                break;
            case bcmSwitchObjectIpmcV4Used:
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmi_ltsw_l3_fib_object_cnt_get(unit,
                                         BCMI_LTSW_L3_FIB_OBJ_HOST_MC_V4_USED,
                                         0,
                                         &entries[index]));
                break;
            case bcmSwitchObjectIpmcV6Max:
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmi_ltsw_l3_fib_object_cnt_get(unit,
                                         BCMI_LTSW_L3_FIB_OBJ_HOST_MC_V6_MAX,
                                         0,
                                         &entries[index]));
                break;
            case bcmSwitchObjectIpmcV6Used:
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmi_ltsw_l3_fib_object_cnt_get(unit,
                                         BCMI_LTSW_L3_FIB_OBJ_HOST_MC_V6_USED,
                                         0,
                                         &entries[index]));
                break;
            case bcmSwitchObjectL3HostUcV4Max:
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmi_ltsw_l3_fib_object_cnt_get(unit,
                                         BCMI_LTSW_L3_FIB_OBJ_HOST_UC_V4_MAX,
                                         0,
                                         &entries[index]));
            break;
            case bcmSwitchObjectL3HostUcV6Max:
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmi_ltsw_l3_fib_object_cnt_get(unit,
                                         BCMI_LTSW_L3_FIB_OBJ_HOST_UC_V6_MAX,
                                         0,
                                         &entries[index]));
            break;
            case bcmSwitchObjectL3HostUcV4Used:
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmi_ltsw_l3_fib_object_cnt_get(unit,
                                         BCMI_LTSW_L3_FIB_OBJ_HOST_UC_V4_USED,
                                         0,
                                         &entries[index]));
            break;
            case bcmSwitchObjectL3HostUcV6Used:
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmi_ltsw_l3_fib_object_cnt_get(unit,
                                         BCMI_LTSW_L3_FIB_OBJ_HOST_UC_V6_USED,
                                         0,
                                         &entries[index]));
            break;
            case bcmSwitchObjectIpmcSrcGroupV4Max:
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmi_ltsw_l3_fib_object_cnt_get(unit,
                                         BCMI_LTSW_L3_FIB_OBJ_HOST_MC_SG_V4_MAX,
                                         0,
                                         &entries[index]));
            break;
            case bcmSwitchObjectIpmcSrcGroupV6Max:
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmi_ltsw_l3_fib_object_cnt_get(unit,
                                         BCMI_LTSW_L3_FIB_OBJ_HOST_MC_SG_V6_MAX,
                                         0,
                                         &entries[index]));
            break;
            case bcmSwitchObjectIpmcSrcGroupV4Used:
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmi_ltsw_l3_fib_object_cnt_get(unit,
                                         BCMI_LTSW_L3_FIB_OBJ_HOST_MC_SG_V4_USED,
                                         0,
                                         &entries[index]));
            break;
            case bcmSwitchObjectIpmcSrcGroupV6Used:
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmi_ltsw_l3_fib_object_cnt_get(unit,
                                         BCMI_LTSW_L3_FIB_OBJ_HOST_MC_SG_V6_USED,
                                         0,
                                         &entries[index]));
            break;
            case bcmSwitchObjectIpmcGroupV4Max:
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmi_ltsw_l3_fib_object_cnt_get(unit,
                                         BCMI_LTSW_L3_FIB_OBJ_HOST_MC_G_V4_MAX,
                                         0,
                                         &entries[index]));
            break;
            case bcmSwitchObjectIpmcGroupV6Max:
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmi_ltsw_l3_fib_object_cnt_get(unit,
                                         BCMI_LTSW_L3_FIB_OBJ_HOST_MC_G_V6_MAX,
                                         0,
                                         &entries[index]));
            break;
            case bcmSwitchObjectIpmcGroupV4Used:
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmi_ltsw_l3_fib_object_cnt_get(unit,
                                         BCMI_LTSW_L3_FIB_OBJ_HOST_MC_G_V4_USED,
                                         0,
                                         &entries[index]));
            break;
            case bcmSwitchObjectIpmcGroupV6Used:
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmi_ltsw_l3_fib_object_cnt_get(unit,
                                         BCMI_LTSW_L3_FIB_OBJ_HOST_MC_G_V6_USED,
                                         0,
                                         &entries[index]));
            break;
            case bcmSwitchObjectL3RouteCurrent:
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmi_ltsw_l3_fib_object_cnt_get(unit,
                                         BCMI_LTSW_L3_FIB_OBJ_ROUTE_USED,
                                         BCMI_LTSW_L3_FIB_OBJ_F_BASE_ENTRIES,
                                         &entries[index]));
                break;
            case bcmSwitchObjectL3RouteV4RoutesMax:
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmi_ltsw_l3_fib_object_cnt_get(unit,
                                         BCMI_LTSW_L3_FIB_OBJ_ROUTE_V4_MAX,
                                         0,
                                         &entries[index]));
                break;
            case bcmSwitchObjectL3RouteV4RoutesUsed:
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmi_ltsw_l3_fib_object_cnt_get(unit,
                                         BCMI_LTSW_L3_FIB_OBJ_ROUTE_V4_USED,
                                         0,
                                         &entries[index]));
                break;
            case bcmSwitchObjectL3RouteV6Routes64bMax:
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmi_ltsw_l3_fib_object_cnt_get(unit,
                                         BCMI_LTSW_L3_FIB_OBJ_ROUTE_V6_64_MAX,
                                         0,
                                         &entries[index]));
                break;
            case bcmSwitchObjectL3RouteV6Routes128bMax:
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmi_ltsw_l3_fib_object_cnt_get(unit,
                                         BCMI_LTSW_L3_FIB_OBJ_ROUTE_V6_128_MAX,
                                         0,
                                         &entries[index]));
                break;
            case bcmSwitchObjectL3RouteV6Routes64bUsed:
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmi_ltsw_l3_fib_object_cnt_get(unit,
                                         BCMI_LTSW_L3_FIB_OBJ_ROUTE_V6_64_USED,
                                         0,
                                         &entries[index]));
                break;
            case bcmSwitchObjectL3RouteV6Routes128bUsed:
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmi_ltsw_l3_fib_object_cnt_get(unit,
                                         BCMI_LTSW_L3_FIB_OBJ_ROUTE_V6_128_USED,
                                         0,
                                         &entries[index]));
                break;
            case bcmSwitchObjectL3RouteTotalUsedRoutes:
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmi_ltsw_l3_fib_object_cnt_get(unit,
                                         BCMI_LTSW_L3_FIB_OBJ_ROUTE_USED,
                                         0,
                                         &entries[index]));
                break;
            case bcmSwitchObjectL3EgressCurrent:
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmi_ltsw_l3_egress_obj_cnt_get(unit,
                                                     BCMI_LTSW_L3_EGR_OBJ_T_OL,
                                                     0, &cnt));
                entries[index] = cnt;
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmi_ltsw_l3_egress_obj_cnt_get(unit,
                                                     BCMI_LTSW_L3_EGR_OBJ_T_UL,
                                                     0, &cnt));
                entries[index] += cnt;
                break;
            case bcmSwitchObjectL3EgressMax:
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmi_ltsw_l3_egress_obj_cnt_get(unit,
                                                     BCMI_LTSW_L3_EGR_OBJ_T_OL,
                                                     &cnt, 0));
                entries[index] = cnt;
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmi_ltsw_l3_egress_obj_cnt_get(unit,
                                                     BCMI_LTSW_L3_EGR_OBJ_T_UL,
                                                     &cnt, 0));
                entries[index] += cnt;
                break;
            case bcmSwitchObjectEcmpCurrent:
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmi_ltsw_l3_egress_obj_cnt_get(unit,
                                                 BCMI_LTSW_L3_EGR_OBJ_T_ECMP_OL,
                                                 0, &cnt));
                entries[index] = cnt;
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmi_ltsw_l3_egress_obj_cnt_get(unit,
                                                 BCMI_LTSW_L3_EGR_OBJ_T_ECMP_UL,
                                                 0, &cnt));
                entries[index] += cnt;
                break;
            case bcmSwitchObjectEcmpMax:
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmi_ltsw_l3_egress_obj_cnt_get(unit,
                                                 BCMI_LTSW_L3_EGR_OBJ_T_ECMP_OL,
                                                 &cnt, 0));
                entries[index] = cnt;
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmi_ltsw_l3_egress_obj_cnt_get(unit,
                                                 BCMI_LTSW_L3_EGR_OBJ_T_ECMP_UL,
                                                 &cnt, 0));
                entries[index] += cnt;
                break;
            case bcmSwitchObjectL3RoutePrivateV4RouteProjection:
                if (SHR_FAILURE(bcmi_ltsw_l3_route_grp_projection(unit,
                                                   bcmL3RouteGroupPrivateV4,
                                                   &entries[index],
                                                   NULL, NULL))) {
                    SHR_IF_ERR_EXIT
                        (bcmi_ltsw_l3_route_grp_testdb(unit,
                                                   bcmL3RouteGroupPrivateV4,
                                                   0, &entries[index],
                                                   NULL, NULL, NULL));
                }
                break;
            case bcmSwitchObjectL3RouteGlobalV4RouteProjection:
                if (SHR_FAILURE(bcmi_ltsw_l3_route_grp_projection(unit,
                                                   bcmL3RouteGroupGlobalV4,
                                                   &entries[index],
                                                   NULL, NULL))) {
                    SHR_IF_ERR_EXIT
                        (bcmi_ltsw_l3_route_grp_testdb(unit,
                                                   bcmL3RouteGroupGlobalV4,
                                                   0, &entries[index],
                                                   NULL, NULL, NULL));
                }
                break;
            case bcmSwitchObjectL3RouteOverrideV4RouteProjection:
                if (SHR_FAILURE(bcmi_ltsw_l3_route_grp_projection(unit,
                                                   bcmL3RouteGroupOverrideV4,
                                                   &entries[index],
                                                   NULL, NULL))) {
                    SHR_IF_ERR_EXIT
                        (bcmi_ltsw_l3_route_grp_testdb(unit,
                                                   bcmL3RouteGroupOverrideV4,
                                                   0, &entries[index],
                                                   NULL, NULL, NULL));
                }
                break;
            case bcmSwitchObjectL3RoutePrivateV6RouteProjection:
                if (SHR_FAILURE(bcmi_ltsw_l3_route_grp_projection(unit,
                                                   bcmL3RouteGroupPrivateV6,
                                                   &entries[index],
                                                   NULL, NULL))) {
                    SHR_IF_ERR_EXIT
                        (bcmi_ltsw_l3_route_grp_testdb(unit,
                                                   bcmL3RouteGroupPrivateV6,
                                                   0, &entries[index],
                                                   NULL, NULL, NULL));
                }
                break;
            case bcmSwitchObjectL3RouteGlobalV6RouteProjection:
                if (SHR_FAILURE(bcmi_ltsw_l3_route_grp_projection(unit,
                                                   bcmL3RouteGroupGlobalV6,
                                                   &entries[index],
                                                   NULL, NULL))) {
                    SHR_IF_ERR_EXIT
                        (bcmi_ltsw_l3_route_grp_testdb(unit,
                                                   bcmL3RouteGroupGlobalV6,
                                                   0, &entries[index],
                                                   NULL, NULL, NULL));
                }
                break;
            case bcmSwitchObjectL3RouteOverrideV6RouteProjection:
                if (SHR_FAILURE(bcmi_ltsw_l3_route_grp_projection(unit,
                                                   bcmL3RouteGroupOverrideV6,
                                                   &entries[index],
                                                   NULL, NULL))) {
                    SHR_IF_ERR_EXIT
                        (bcmi_ltsw_l3_route_grp_testdb(unit,
                                                   bcmL3RouteGroupOverrideV6,
                                                   0, &entries[index],
                                                   NULL, NULL, NULL));
                }
                break;
            default:
                SHR_ERR_EXIT(SHR_E_UNAVAIL);
        }
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get number of LED microcontrollers.
 *
 * \param Parameters:
 * \param [in] unit Unit number.
 * \param [out] led_uc_num Number of LED microcontrollers.
 *
 * \retval SHR_E_NONE on operation successful
 *         SHR_E_PARAM on invalid input parameter value
 *         SHR_E_UNAVAIL on feature Not supported
 */
int
bcm_ltsw_switch_led_uc_num_get(int unit,
                               int *led_uc_num)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(led_uc_num, SHR_E_PARAM);

    *led_uc_num = bcmbd_led_uc_num_get(unit);

exit:
    SHR_FUNC_EXIT();
}
/*!
 * \brief Read control data of LED firmware.
 *
 * \param [in]  unit Unit number.
 * \param [in]  led_uc LED microcontroller number.
 * \param [in]  offset Position to start led control data read.
 * \param [out] data Read data buffer.
 * \param [in]  len Length of read data.
 *
 * \retval SHR_E_NONE on operation successful.
 *         SHR_E_PARAM on invalid input parameter value.
 *         SHR_E_UNAVAIL on feature Not supported.
 */
int
bcm_ltsw_switch_led_control_data_read(int unit,
                                      int led_uc,
                                      int offset,
                                      uint8 *data,
                                      int len)
{
    int i;
    uint8 data8;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(data, SHR_E_PARAM);

    for (i = 0; i < len; i ++) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmbd_led_uc_swdata_read(unit, led_uc, i + offset, &data8));
        data[i] = data8;
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Write control data of LED firmware.
 *
 * \param [in] unit Unit number.
 * \param [in] led_uc LED microcontroller number.
 * \param [in] offset Position to start led control data write.
 * \param [in] data Write data buffer.
 * \param [in] len Length of write data.
 *
 * \retval SHR_E_NONE on operation successful.
 *         SHR_E_PARAM on invalid input parameter value.
 *         SHR_E_UNAVAIL on feature Not supported.
 */
int
bcm_ltsw_switch_led_control_data_write(int unit,
                                       int led_uc,
                                       int offset,
                                       const uint8 *data,
                                       int len)
{
    int i;
    uint8 data8;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(data, SHR_E_PARAM);

    for (i = 0; i < len; i ++) {
        data8 = data[i];
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmbd_led_uc_swdata_write(unit, led_uc, i + offset, data8));
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Load LED firmware binary.
 *
 * \param [in] unit Unit number.
 * \param [in] led_uc LED microcontroller number.
 * \param [in] data Firmware binary content.
 * \param [in] len Length of firmware binary.
 *
 * \retval SHR_E_NONE on operation successful.
 *         SHR_E_PARAM on invalid input parameter value.
 *         SHR_E_UNAVAIL on feature Not supported.
 */
int
bcm_ltsw_switch_led_fw_load(int unit,
                            int led_uc,
                            const uint8 *data,
                            int len)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(data, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmbd_led_uc_fw_load(unit, led_uc, data, len));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get if LED firmware is started or not.
 *
 * \param [in] unit Unit number.
 * \param [in] led_uc LED microcontroller number.
 * \param [out] start Whether firmware is started(1) or stopped(0).
 *
 * \retval SHR_E_NONE on operation successful.
 *         SHR_E_PARAM on invalid input parameter value.
 *         SHR_E_UNAVAIL on feature Not supported.
 */
int
bcm_ltsw_switch_led_fw_start_get(int unit,
                                 int led_uc,
                                 int *start)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(start, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmbd_led_uc_start_get(unit, led_uc, start));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Start/stop LED firmware.
 *
 * \param [in] unit Unit number.
 * \param [in] led_uc LED microcontroller number.
 * \param [in] start Value to request firmware to start(1) or stop(0).
 *
 * \retval SHR_E_NONE on operation successful.
 *         SHR_E_PARAM on invalid input parameter value.
 *         SHR_E_UNAVAIL on feature Not supported.
 */
int
bcm_ltsw_switch_led_fw_start_set(int unit,
                                 int led_uc,
                                 int start)
{
    SHR_FUNC_ENTER(unit);

    if (start) {
        SHR_IF_ERR_VERBOSE_EXIT(bcmbd_led_uc_start(unit, led_uc));
    } else {
        SHR_IF_ERR_VERBOSE_EXIT(bcmbd_led_uc_stop(unit, led_uc));
    }

exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief Map physical port to LED microcontroller number and port index.
 *
 * \param [in] unit Unit number.
 * \param [in] port Physical port number.
 * \param [out] led_uc LED microcontroller number.
 * \param [out] led_uc_port Port index controlled by corresponding LED microcontroller.
 *
 * \retval SHR_E_NONE on operation successful.
 *         SHR_E_PARAM on invalid input parameter value.
 *         SHR_E_UNAVAIL on feature Not supported.
 */
int
bcm_ltsw_switch_led_port_to_uc_port_get(int unit,
                                        int port,
                                        int *led_uc,
                                        int *led_uc_port)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(led_uc, SHR_E_PARAM);
    SHR_NULL_CHECK(led_uc_port, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmbd_port_to_led_uc_port(unit, port, led_uc, led_uc_port));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Configure the mapping from physical port to LED microcontroller
 *        number and port index.
 *
 * \param [in] unit Unit number.
 * \param [in] port Physical port number.
 * \param [in] led_uc LED microcontroller number.
 * \param [in] led_uc_port Port index controlled by corresponding LED microcontroller.
 *
 * \retval SHR_E_NONE on operation successful.
 *         SHR_E_PARAM on invalid input parameter value.
 *         SHR_E_UNAVAIL on feature Not supported.
 */
int
bcm_ltsw_switch_led_port_to_uc_port_set(int unit,
                                        int port,
                                        int led_uc,
                                        int led_uc_port)
{
    return SHR_E_UNAVAIL;
}

/*!
 * \brief Get information about a hash bank.
 *
 * \param [in] unit Unit Number.
 * \param [in/out] bank_info Hash bank information structure.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_switch_hash_bank_info_get(int unit,
                                   bcm_switch_hash_bank_info_t *bank_info)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
       (bcmi_ltsw_uft_bank_info_get(unit, bank_info));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get the hash banks assigned to a hash table.
 *
 * \param [in] unit Unit number.
 * \param [in] hash_table Hash table.
 * \param [in] array_size Array size of bank_array.
 * \param [out] bank_array Hash bank array.
 * \param [out] bank_array Actual number of hash banks returned in bank_array.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
int
bcm_ltsw_switch_hash_bank_multi_get(int unit,
                                    bcm_switch_hash_table_t hash_table,
                                    int array_size,
                                    bcm_switch_hash_bank_t *bank_array,
                                    int *num_banks)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
       (bcmi_ltsw_uft_bank_multi_get(unit, hash_table, array_size, bank_array,
                                     num_banks));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get the hash tables to which a given hash bank is assigned.
 *
 * \param [in] unit Unit Number.
 * \param [in] bank_num Hash bank number.
 * \param [in] array_size Array size of table_array.
 * \param [out] table_array Hash table array.
 * \param [out] num_tables Actual number of hash tables returned in table_array.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_switch_hash_table_multi_get(int unit,
                                    bcm_switch_hash_bank_t bank_num,
                                    int array_size,
                                    bcm_switch_hash_table_t *table_array,
                                    int *num_tables)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
       (bcmi_ltsw_uft_table_multi_get(unit, bank_num, array_size, table_array,
                                      num_tables));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Hash bank control set function.
 *
 * \param [in] unit Unit number.
 * \param [in] bank_control Hash bank control structure.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
int
bcm_ltsw_switch_hash_bank_control_set(int unit,
                                      bcm_switch_hash_bank_control_t *bank_control)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
       (bcmi_ltsw_uft_bank_control_set(unit, bank_control));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Hash bank control get function.
 *
 * \param [in] unit Unit number.
 * \param [in/out] bank_control Hash bank control structure.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
int
bcm_ltsw_switch_hash_bank_control_get(int unit,
                                      bcm_switch_hash_bank_control_t *bank_control)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
       (bcmi_ltsw_uft_bank_control_get(unit, bank_control));

exit:
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_switch_ipv6_reserved_multicast_addr_multi_set(
    int unit,
    int inner,
    int num,
    bcm_ip6_t* ip6_addr,
    bcm_ip6_t* ip6_mask)

{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(ip6_addr, SHR_E_PARAM);
    SHR_NULL_CHECK(ip6_mask, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_l3_ipv6_reserved_multicast_addr_multi_set(unit, num, ip6_addr, ip6_mask));

exit:
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_switch_ipv6_reserved_multicast_addr_multi_get(
    int unit,
    int inner,
    int num,
    bcm_ip6_t *ip6_addr,
    bcm_ip6_t *ip6_mask)

{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(ip6_addr, SHR_E_PARAM);
    SHR_NULL_CHECK(ip6_mask, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_l3_ipv6_reserved_multicast_addr_multi_get(unit, num, ip6_addr, ip6_mask));

exit:
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_switch_hash_banks_config_set(
    int unit,
    bcm_switch_hash_table_t hash_table,
    uint32 bank_num,
    int hash_type,
    uint32 hash_offset)

{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_uft_bank_config_set(unit, hash_table,
                                       bank_num, hash_type,
                                       hash_offset));

exit:
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_switch_hash_banks_config_get(
    int unit,
    bcm_switch_hash_table_t hash_table,
    uint32 bank_num,
    int *hash_type,
    uint32 *hash_offset)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_uft_bank_config_get(unit, hash_table,
                                       bank_num, hash_type,
                                       hash_offset));

exit:
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_switch_hash_banks_max_get(
    int unit,
    bcm_switch_hash_table_t hash_table,
    uint32 *bank_count)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_uft_bank_max_get(unit, hash_table,
                                    bank_count));

exit:
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_switch_opaque_tag_config_get(
    int unit,
    int index,
    bcm_switch_opaque_tag_params_t *opaque_tag_config)
{
    int tag_size, ethertype, tag_type, valid;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_l2_opaque_tag_get(unit, index, &tag_size,
                                     &ethertype, &tag_type, &valid));
    opaque_tag_config->tag_size = tag_size;
    opaque_tag_config->ethertype = ethertype;
    opaque_tag_config->tag_type = tag_type;
    opaque_tag_config->valid = valid;

exit:
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_switch_opaque_tag_config_set(
    int unit,
    int index,
    bcm_switch_opaque_tag_params_t *opaque_tag_config)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_l2_opaque_tag_set(unit, index,
                                     (int)opaque_tag_config->tag_size,
                                     (int)opaque_tag_config->ethertype,
                                     (int)opaque_tag_config->tag_type,
                                     (int)opaque_tag_config->valid));

exit:
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_switch_payload_opaque_tag_config_get(
    int unit,
    int index,
    bcm_switch_opaque_tag_params_t *opaque_tag_config)
{
    int tag_size, ethertype, tag_type, valid;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_l2_payload_opaque_tag_get(unit, index, &tag_size,
                                             &ethertype, &tag_type, &valid));
    opaque_tag_config->tag_size = tag_size;
    opaque_tag_config->ethertype = ethertype;
    opaque_tag_config->tag_type = tag_type;
    opaque_tag_config->valid = valid;

exit:
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_switch_payload_opaque_tag_config_set(
    int unit,
    int index,
    bcm_switch_opaque_tag_params_t *opaque_tag_config)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_l2_payload_opaque_tag_set(unit, index,
                                             (int)opaque_tag_config->tag_size,
                                             (int)opaque_tag_config->ethertype,
                                             (int)opaque_tag_config->tag_type,
                                             (int)opaque_tag_config->valid));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Set mapping of bcm_obm_code_point to bcm_obm_priority
 *
 * \param [in] unit Unit Number.
 * \param [in] gport Generic port number.
 * \param [in] switch_obm_classifier_type OBM classifier type.
 * \param [in] switch_obm_classifier OBM classifier.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
int
bcm_ltsw_switch_obm_classifier_mapping_set(
    int unit,
    bcm_gport_t gport,
    bcm_switch_obm_classifier_type_t switch_obm_classifier_type,
    bcm_switch_obm_classifier_t *switch_obm_classifier)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
       (bcmi_ltsw_obm_classifier_mapping_multi_set(unit, gport, switch_obm_classifier_type,
                                                   1, switch_obm_classifier));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get mapping of bcm_obm_code_point to bcm_obm_priority
 *
 * \param [in] unit Unit Number.
 * \param [in] gport Generic port number.
 * \param [in] switch_obm_classifier_type OBM classifier type.
 * \param [out] switch_obm_classifier OBM classifier.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
int
bcm_ltsw_switch_obm_classifier_mapping_get(
    int unit,
    bcm_gport_t gport,
    bcm_switch_obm_classifier_type_t switch_obm_classifier_type,
    bcm_switch_obm_classifier_t *switch_obm_classifier)
{
    int array_count = 0;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
       (bcmi_ltsw_obm_classifier_mapping_multi_get(unit, gport, switch_obm_classifier_type,
                                                   1, switch_obm_classifier, &array_count));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Set multiple mappings of bcm_obm_code_point to bcm_obm_priority
 *
 * \param [in] unit Unit Number.
 * \param [in] gport Generic port number.
 * \param [in] switch_obm_classifier_type OBM classifier type.
 * \param [in] array_count Array size of switch_obm_classifier.
 * \param [in] switch_obm_classifier OBM classifier.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
int
bcm_ltsw_switch_obm_classifier_mapping_multi_set(
    int unit,
    bcm_gport_t gport,
    bcm_switch_obm_classifier_type_t switch_obm_classifier_type,
    int array_count,
    bcm_switch_obm_classifier_t *switch_obm_classifier)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
       (bcmi_ltsw_obm_classifier_mapping_multi_set(unit, gport, switch_obm_classifier_type,
                                                   array_count, switch_obm_classifier));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get multiple mappings of bcm_obm_code_point to bcm_obm_priority
 *
 * \param [in] unit Unit Number.
 * \param [in] gport Generic port number.
 * \param [in] switch_obm_classifier_type OBM classifier type.
 * \param [in] array_max Array size of switch_obm_classifier.
 * \param [out] switch_obm_classifier OBM classifier.
 * \param [out] array_count Actual number of OBM classifiers returned in switch_obm_classifier.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
int
bcm_ltsw_switch_obm_classifier_mapping_multi_get(
    int unit,
    bcm_gport_t gport,
    bcm_switch_obm_classifier_type_t switch_obm_classifier_type,
    int array_max,
    bcm_switch_obm_classifier_t *switch_obm_classifier,
    int *array_count)

{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
       (bcmi_ltsw_obm_classifier_mapping_multi_get(unit, gport, switch_obm_classifier_type,
                                                   array_max, switch_obm_classifier,
                                                   array_count));

exit:
    SHR_FUNC_EXIT();
}

int bcm_ltsw_switch_temperature_monitor_get(
    int unit,
    int temperature_max,
    bcm_switch_temperature_monitor_t *temperature_array,
    int *temperature_count)
{
    size_t count;
    size_t sensor_count;
    int i;
    bcmbd_pvt_temp_rec_t *rptr = NULL;
    bcmbd_pvt_temp_rec_t *uptr = NULL;
    int max_cnt;
    uint32_t equation = PVT_EQUATION_1;

    SHR_FUNC_ENTER(unit);

    /* Call bcmbd api to do actual work */
    count = 0;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmbd_pvt_temperature_get(unit, count, equation, rptr, &sensor_count));

    count = sensor_count;

    if (count == 0) {
        SHR_EXIT();
    }

    rptr = sal_alloc(sizeof(bcmbd_pvt_temp_rec_t) * count,
                     "bcmSwitchTemp");
    if (rptr == NULL) {
        SHR_ERR_EXIT(SHR_E_MEMORY);
    }

    sal_memset(rptr, 0, sizeof(bcmbd_pvt_temp_rec_t) * count);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmbd_pvt_temperature_get(unit, count, equation,
                                   rptr, &sensor_count));

    max_cnt = count;
    max_cnt = temperature_max < max_cnt ? temperature_max : max_cnt;

    uptr = rptr;
    for (i = 0; i < max_cnt; i++) {
        temperature_array[i].curr = (int)(uptr->current * 10);
        temperature_array[i].peak = (int)(uptr->max * 10);
        uptr++;
    }

    *temperature_count = max_cnt;

exit:
    if (rptr) {
        sal_free(rptr);
    }
    SHR_FUNC_EXIT();
}

int bcm_ltsw_switch_hash_entry_create_qset(
    int unit,
    bcm_field_qset_t qset,
    bcm_hash_entry_t *entry)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_lb_hash_flex_entry_create(unit, qset, entry));

exit:
    SHR_FUNC_EXIT();
}

int bcm_ltsw_switch_hash_entry_destroy(
    int unit,
    bcm_hash_entry_t entry)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_lb_hash_flex_entry_destroy(unit, entry));

exit:
    SHR_FUNC_EXIT();
}

int bcm_ltsw_switch_hash_entry_install(
    int unit,
    bcm_hash_entry_t entry,
    uint32 offset)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_lb_hash_flex_entry_install(unit, entry, offset));

exit:
    SHR_FUNC_EXIT();
}

int bcm_ltsw_switch_hash_entry_reinstall(
    int unit,
    bcm_hash_entry_t entry,
    uint32 offset)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_lb_hash_flex_entry_reinstall(unit, entry, offset));

exit:
    SHR_FUNC_EXIT();
}

int bcm_ltsw_switch_hash_entry_remove(
    int unit,
    bcm_hash_entry_t entry)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_lb_hash_flex_entry_remove(unit, entry));

exit:
    SHR_FUNC_EXIT();
}

int bcm_ltsw_switch_hash_entry_set(
    int unit,
    bcm_hash_entry_t entry,
    bcm_switch_hash_entry_config_t *config)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_lb_hash_flex_entry_set(unit, entry, config));

exit:
    SHR_FUNC_EXIT();
}

int bcm_ltsw_switch_hash_entry_get(
    int unit,
    bcm_hash_entry_t entry,
    bcm_switch_hash_entry_config_t *config)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_lb_hash_flex_entry_get(unit, entry, config));

exit:
    SHR_FUNC_EXIT();
}

int bcm_ltsw_switch_hash_entry_traverse(
    int unit,
    int flags,
    bcm_switch_hash_entry_traverse_cb cb_fn,
    void *user_data)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_lb_hash_flex_entry_traverse(unit, flags,
                                               cb_fn, user_data));

exit:
    SHR_FUNC_EXIT();
}

int bcm_ltsw_switch_hash_qualify_udf(
    int unit,
    bcm_hash_entry_t entry,
    bcm_udf_id_t udf_id,
    int length,
    uint8 *data,
    uint8 *mask)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_lb_hash_flex_qualify_udf(unit, entry, udf_id,
                                            length, data, mask));

exit:
    SHR_FUNC_EXIT();
}

int bcm_ltsw_switch_hash_qualify_udf_get(
    int unit,
    bcm_hash_entry_t entry,
    bcm_udf_id_t udf_id,
    int max_length,
    uint8 *data,
    uint8 *mask,
    int *actual_length)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_lb_hash_flex_qualify_udf_get(unit, entry, udf_id,
                                                max_length, data, mask,
                                                actual_length));

exit:
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_switch_hash_qualify_data(
    int unit,
    bcm_hash_entry_t entry,
    int qual_id,
    uint32 data,
    uint32 mask)
{
    return BCM_E_UNAVAIL;
}

int
bcm_ltsw_switch_network_group_config_set(
    int unit,
    bcm_switch_network_group_t source_network_group_id,
    bcm_switch_network_group_config_t *config)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_virtual_network_group_split_config_set
            (unit, source_network_group_id, config));

exit:
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_switch_network_group_config_get(
    int unit,
    bcm_switch_network_group_t source_network_group_id,
    bcm_switch_network_group_config_t *config)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_virtual_network_group_split_config_get
            (unit, source_network_group_id, config));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Create an EP Recirculate profile.
 *
 * \param [in] unit Unit number.
 * \param [in/out] recirc_profile The EP Recirculate profile config structure.
 * \param [out] recirc_profile_id The EP Recirculate config profile id.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_switch_ep_recirc_profile_create(
    int unit,
    bcm_switch_ep_recirc_profile_t *recirc_profile,
    int *recirc_profile_id)
{
    return mbcm_ltsw_switch_ep_recirc_profile_create(unit, recirc_profile, recirc_profile_id);
}

/*!
 * \brief Destroy an EP Recirculate profile.
 *
 * \param [in] unit Unit number.
 * \param [in] recirc_profile_id The EP Recirculate config profile id.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_switch_ep_recirc_profile_destroy(
    int unit,
    int recirc_profile_id)
{
    return mbcm_ltsw_switch_ep_recirc_profile_destroy(unit, recirc_profile_id);
}

/*!
 * \brief Get all EP Recirculate profile ids.
 *
 * If recirc_profile_id_array == NULL, count will be OUT for the number of
 * profiles; if the size cannot fit, it will only copy according to the size.
 *
 * \param [in] unit Unit number.
 * \param [out] recirc_profile_id_array EP Recirculate profile id array.
 * \param [in/out] count Count of the EP Recirculate profiles.
 *
 * \retval BCM_E_NONE on success and error code otherwise.
 */
int
bcm_ltsw_switch_ep_recirc_profile_get_all(
    int unit,
    int *recirc_profile_id_array,
    int *count)
{
    return mbcm_ltsw_switch_ep_recirc_profile_get_all(unit,
                                                      recirc_profile_id_array,
                                                      count);
}

/*!
 * \brief Get an EP Recirculate profile.
 *
 * \param [in] unit Unit number.
 * \param [in] recirc_profile_id The EP Recirculate profile id.
 * \param [out] recirc_profile The EP Recirculate profile.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_switch_ep_recirc_profile_get(
    int unit,
    int recirc_profile_id,
    bcm_switch_ep_recirc_profile_t *recirc_profile)
{
    return mbcm_ltsw_switch_ep_recirc_profile_get(unit,
                                                  recirc_profile_id,
                                                  recirc_profile);
}

/*!
 * \brief Set drop event based EP Recirculate control.
 *
 * \param [in] unit Unit number.
 * \param [in] drop_event_ctrl The recirculate drop event control.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_switch_ep_recirc_drop_event_control_set(
    int unit,
    bcm_switch_ep_recirc_drop_event_control_t *drop_event_ctrl)
{
    return mbcm_ltsw_switch_ep_recirc_drop_event_control_set(unit,
                                                             drop_event_ctrl);
}


/*!
 * \brief Get drop event based EP Recirculate control.
 *
 * \param [in] unit Unit number.
 * \param [out] drop_event_ctrl The recirculate drop event control.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_switch_ep_recirc_drop_event_control_get(
    int unit,
    bcm_switch_ep_recirc_drop_event_control_t *drop_event_ctrl)
{
    return mbcm_ltsw_switch_ep_recirc_drop_event_control_get(unit,
                                                             drop_event_ctrl);
}

/*!
 * \brief Set trace event based EP Recirculate control.
 *
 * \param [in] unit Unit number.
 * \param [in] trace_event_ctrl The recirculate trace event control.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_switch_ep_recirc_trace_event_control_set(
    int unit,
    bcm_switch_ep_recirc_trace_event_control_t *trace_event_ctrl)
{
    return mbcm_ltsw_switch_ep_recirc_trace_event_control_set(unit,
                                                              trace_event_ctrl);
}

/*!
 * \brief Get trace event based EP Recirculate control.
 *
 * \param [in] unit Unit number.
 * \param [out] trace_event_ctrl The recirculate trace event control.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_switch_ep_recirc_trace_event_control_get(
    int unit,
    bcm_switch_ep_recirc_trace_event_control_t *trace_event_ctrl)
{
    return mbcm_ltsw_switch_ep_recirc_trace_event_control_get(unit,
                                                              trace_event_ctrl);
}

/*!
 * \brief Enable/disable an egress trace event to trigger EP Recirculate.
 *
 * Users need to refer to related documents for the currently supported
 * trace event types.
 *
 * \param [in] unit Unit number.
 * \param [in] trace_event The trace event.
 * \param [in] enable The enabled status of the trace event.
 *
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_switch_ep_recirc_trace_event_enable_set(
    int unit,
    bcm_pkt_trace_event_t trace_event,
    int enable)
{
    return mbcm_ltsw_switch_ep_recirc_trace_event_enable_set(unit,
                                                             trace_event,
                                                             enable);
}

/*!
 * \brief Get all enabled egress trace event for EP Recirculate.
 *
 * If trace_event_array == NULL, count will be OUT for the number of
 * events; if the size cannot fit, it will only copy according to the size;
 *
 * \param [in] unit Unit number.
 * \param [out] trace_event_array Trace event array.
 * \param [in/out] count Count of the enabled trace events.
 *
 * \retval BCM_E_NONE on success and error code otherwise.
 */
int
bcm_ltsw_switch_ep_recirc_trace_event_enable_get_all(
    int unit,
    bcm_pkt_trace_event_t *trace_event_array,
    int *count)
{
    return mbcm_ltsw_switch_ep_recirc_trace_event_enable_get_all(unit,
                                                                 trace_event_array,
                                                                 count);
}

/*!
 * \brief Get enabled status of an egress trace event for EP Recirculate.
 *
 * Users need to refer to related documents for the currently supported
 * trace event types.
 *
 * \param [in] unit Unit number.
 * \param [in] trace_event The trace event.
 * \param [out] enable The enabled status of the trace event.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_switch_ep_recirc_trace_event_enable_get(
    int unit,
    bcm_pkt_trace_event_t trace_event,
    int *enable)
{
    return mbcm_ltsw_switch_ep_recirc_trace_event_enable_get(unit,
                                                             trace_event,
                                                             enable);
}

