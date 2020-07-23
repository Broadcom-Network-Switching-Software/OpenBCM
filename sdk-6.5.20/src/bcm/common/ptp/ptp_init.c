/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:    ptp_init.c
 *
 * Purpose: 
 *
 * Functions:
 *      bcm_common_ptp_detach
 *      bcm_common_ptp_lock
 *      bcm_common_ptp_unlock
 *      bcm_common_ptp_init
 *      bcm_common_ptp_stack_create
 *      bcm_common_ptp_time_format_set
 *      bcm_common_ptp_cb_register
 *      bcm_common_ptp_cb_unregister
 *
 *      _bcm_ptp_clock_cache_init
 *      _bcm_ptp_clock_cache_detach
 *      _bcm_ptp_clock_cache_stack_create
 *      _bcm_ptp_clock_cache_info_create
 *      _bcm_ptp_clock_cache_info_get
 *      _bcm_ptp_clock_cache_info_set
 *      _bcm_ptp_clock_cache_signal_get
 *      _bcm_ptp_clock_cache_signal_set
 *      _bcm_ptp_clock_cache_tod_input_get
 *      _bcm_ptp_clock_cache_tod_input_set
 *      _bcm_ptp_clock_cache_tod_output_get
 *      _bcm_ptp_clock_cache_tod_output_set
 */

#if defined(INCLUDE_PTP)

#include <soc/defs.h>
#include <soc/drv.h>
#include <sal/core/dpc.h>
#include <shared/bsl.h>

#if defined(BCM_CMICM_SUPPORT)
#include <soc/uc.h>
#endif

#include <bcm/ptp.h>
#include <bcm_int/common/ptp.h>
#include <bcm_int/ptp_common.h>
#include <bcm/error.h>

#include <soc/uc_msg.h>



static int next_stack_id[BCM_MAX_NUM_UNITS] = {0};  /* per-unit ID of next stack */
_bcm_ptp_unit_cache_t _bcm_common_ptp_unit_array[BCM_MAX_NUM_UNITS] = {{0}};
_bcm_ptp_info_t _bcm_common_ptp_info[BCM_MAX_NUM_UNITS];

/*
 * Function:
 *      bcm_common_ptp_lock
 * Purpose:
 *      Locks the unit mutex
 * Parameters:
 *      unit - (IN) Unit number.
 * Returns:
 *      BCM_E_NONE
 *      BCM_E_INIT
 * Notes:
 */
int bcm_common_ptp_lock(int unit)
{
    if (_bcm_common_ptp_info[unit].mutex == NULL) {
        return BCM_E_INIT;
    }

    return _bcm_ptp_mutex_take(_bcm_common_ptp_info[unit].mutex, 100);
}

/*
 * Function:
 *      bcm_common_ptp_unlock
 * Purpose:
 *      Unlocks the unit mutex
 * Parameters:
 *      unit - (IN) Unit number.
 * Returns:
 *      BCM_E_NONE
 *      BCM_E_INTERNAL
 * Notes:
 */
int bcm_common_ptp_unlock(int unit)
{
    if (_bcm_ptp_mutex_give(_bcm_common_ptp_info[unit].mutex) != 0) {
        return BCM_E_INTERNAL;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_common_ptp_init
 * Purpose:
 *      Initialize the PTP subsystem
 * Parameters:
 *      unit - (IN) Unit number.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int
bcm_common_ptp_init(
    int unit)
{
    int rv = BCM_E_NONE;

    if (soc_feature(unit, soc_feature_ptp)) {
        if (_bcm_common_ptp_info[unit].mutex == NULL) {
            _bcm_common_ptp_info[unit].mutex = _bcm_ptp_mutex_create("ptp.mutex");
            if (_bcm_common_ptp_info[unit].mutex == NULL) {
                rv = BCM_E_MEMORY;
            }
        }

        if (_bcm_common_ptp_info[unit].stack_info == NULL){
            _bcm_common_ptp_info[unit].stack_info = soc_cm_salloc(unit,
                sizeof(_bcm_ptp_stack_info_t)*PTP_MAX_STACKS_PER_UNIT, "ptp_stack_info");
            sal_memset(_bcm_common_ptp_info[unit].stack_info, 0,
                sizeof(_bcm_ptp_stack_info_t)*PTP_MAX_STACKS_PER_UNIT);
        }

        _bcm_common_ptp_info[unit].memstate = PTP_MEMSTATE_INITIALIZED;

        if (BCM_FAILURE(rv = _bcm_ptp_rx_init(unit))) {
            PTP_ERROR_FUNC("_bcm_ptp_rx_init()");
            return rv;
        }

        if (BCM_FAILURE(rv = _bcm_ptp_clock_cache_init(unit))) {
            PTP_ERROR_FUNC("_bcm_ptp_clock_cache_init()");
            return rv;
        }

        if (BCM_FAILURE(rv = _bcm_ptp_management_init(unit))) {
            PTP_ERROR_FUNC("_bcm_ptp_management_init()");
            return rv;
        }

        if (BCM_FAILURE(rv = _bcm_ptp_unicast_master_table_init(unit))) {
            PTP_ERROR_FUNC("_bcm_ptp_unicast_master_table_init()");
            return rv;
        }

        if (BCM_FAILURE(rv = _bcm_ptp_unicast_slave_table_init(unit))) {
            PTP_ERROR_FUNC("_bcm_ptp_unicast_slave_table_init()");
            return rv;
        }

        if (BCM_FAILURE(rv = _bcm_ptp_acceptable_master_table_init(unit))) {
            PTP_ERROR_FUNC("_bcm_ptp_acceptable_master_table_init()");
            return rv;
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_common_ptp_detach
 * Purpose:
 *      Shut down the PTP subsystem
 * Parameters:
 *      unit - (IN) Unit number.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_common_ptp_detach(
    int unit)
{
    int i;
    int rv = BCM_E_NONE;
    

    if (soc_feature(unit, soc_feature_ptp)) {
        for (i = 0; i < PTP_MAX_STACKS_PER_UNIT; ++i) {
            if (_bcm_common_ptp_info[unit].stack_info[i].in_use) {
                if (BCM_FAILURE(rv = bcm_common_ptp_stack_destroy(unit, i))) {
                    PTP_ERROR_FUNC("bcm_common_ptp_stack_destroy()");
                }
            }
        }

        if (BCM_FAILURE(rv = _bcm_ptp_rx_detach(unit))) {
            PTP_ERROR_FUNC("_bcm_ptp_rx_detach()");
        }

        if (BCM_FAILURE(rv = _bcm_ptp_clock_cache_detach(unit))) {
            PTP_ERROR_FUNC("_bcm_ptp_clock_cache_detach()");
        }

        if (BCM_FAILURE(rv = _bcm_ptp_management_detach(unit))) {
            PTP_ERROR_FUNC("_bcm_ptp_management_detach()");
        }

        if (BCM_FAILURE(rv = _bcm_ptp_unicast_master_table_detach(unit))) {
            PTP_ERROR_FUNC("_bcm_ptp_unicast_master_table_detach()");
        }

        if (BCM_FAILURE(rv = _bcm_ptp_unicast_slave_table_detach(unit))) {
            PTP_ERROR_FUNC("_bcm_ptp_unicast_slave_table_detach()");
        }

        if (BCM_FAILURE(rv = _bcm_ptp_acceptable_master_table_detach(unit))) {
            PTP_ERROR_FUNC("_bcm_ptp_acceptable_master_table_detach()");
        }
    
        if (_bcm_common_ptp_info[unit].stack_info != NULL){ 
            soc_cm_sfree(unit, _bcm_common_ptp_info[unit].stack_info);
            _bcm_common_ptp_info[unit].stack_info = NULL;
        }

        if (_bcm_common_ptp_info[unit].mutex != NULL) {
            _bcm_ptp_mutex_destroy(_bcm_common_ptp_info[unit].mutex);
            _bcm_common_ptp_info[unit].mutex = NULL;
        }

    }

    return BCM_E_NONE;
}

#if defined(BCM_CMICM_SUPPORT) && defined(BCM_PTP_INTERNAL_STACK_SUPPORT)
/*
 * Function:
 *     _bcm_ptp_ts0_ts1_combined_mode_config_send
 * Purpose:
 *     Functioin to send ts0 ts1 combined mode enable flag
 * Parameters:
 *      unit - (IN) Unit number.
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int
_bcm_ptp_ts0_ts1_combined_mode_config_send(
    int unit,
    int ts0_ts1_combined_mode_en, int uC)
{
    mos_msg_data_t start_msg;
    int rc;
    int timeout_usec=1900000;

    start_msg.s.mclass = MOS_MSG_CLASS_1588;
    start_msg.s.subclass = MOS_MSG_SUBCLASS_TS0_TS1_COMBINED_MODE_EN;
    start_msg.s.len = 0;
    start_msg.s.data = bcm_htonl(ts0_ts1_combined_mode_en);

    rc = soc_cmic_uc_msg_send(unit, uC, &start_msg, timeout_usec);

    if (BCM_FAILURE(rc)) {
        LOG_WARN(BSL_LS_BCM_PTP, (BSL_META_U(unit,
           "Error is sending ptp profile configuration\n")
           ));
        rc = BCM_E_FAIL;
    }
    return (rc);
}


/*
 * Function:
 *     _bcm_ptp_sw_cf_update_mode_config_send
 * Purpose:
 *     Functioin to send cf update mode (software vs PP) flag
 * Parameters:
 *      unit - (IN) Unit number.
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int
_bcm_ptp_sw_cf_update_mode_config_send(
    int unit,
    int ptp_cf_update_mode, int uC)
{
    mos_msg_data_t start_msg;
    int rc;
    int timeout_usec=1900000;

    start_msg.s.mclass = MOS_MSG_CLASS_1588;
    start_msg.s.subclass = MOS_MSG_SUBCLASS_PTP_CF_SW_UPDATE_EN;
    start_msg.s.len = 0;
    start_msg.s.data = bcm_htonl(ptp_cf_update_mode);

    rc = soc_cmic_uc_msg_send(unit, uC, &start_msg, timeout_usec);

    if (BCM_FAILURE(rc)) {
        LOG_WARN(BSL_LS_BCM_PTP, (BSL_META_U(unit,
           "Error in sending ptp cf update mode\n")
           ));
        rc = BCM_E_FAIL;
    }
    return (rc);
}
#endif

/*
 * Function:
 *      bcm_common_ptp_stack_create
 * Purpose:
 *      Create a PTP stack instance
 * Parameters:
 *      unit - (IN) Unit number.
 *      ptp_info - (IN/OUT) Pointer to an PTP Stack Info structure
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int 
bcm_common_ptp_stack_create(
    int unit,
    bcm_ptp_stack_info_t *info)
{
    int rv = BCM_E_NONE;
    _bcm_ptp_stack_info_t *stack_p;
    _bcm_ptp_info_t *ptp_info_p;
    pbmp_t pbmp = {{0x03008000}};

    SET_PTP_INFO;
    if (soc_feature(unit, soc_feature_ptp)) {
        rv = bcm_common_ptp_lock(unit);
        if (BCM_FAILURE(rv)) {
           return rv;
        }

        if ((info->flags & BCM_PTP_STACK_WITH_ID) == 0) {
            info->id = (bcm_ptp_stack_id_t) next_stack_id[unit]++;
        }

        if (info->id >= PTP_MAX_STACKS_PER_UNIT) {
            /* Zero-based stack ID exceeds max. */
            rv = BCM_E_PARAM;
            goto done;
        }

        /* Check if stack is in use. */
        stack_p = &ptp_info_p->stack_info[info->id];
        if (stack_p->in_use) {
            rv = BCM_E_EXISTS;
            goto done;
        }

        stack_p->stack_id = info->id;
        stack_p->unit = unit;
        stack_p->in_use = 1;
        stack_p->flags = info->flags;

        stack_p->unicast_master_table_size = PTP_MAX_UNICAST_MASTER_TABLE_ENTRIES;
        stack_p->acceptable_master_table_size = PTP_MAX_ACCEPTABLE_MASTER_TABLE_ENTRIES;
        stack_p->unicast_slave_table_size = PTP_MAX_UNICAST_SLAVE_TABLE_ENTRIES;
        stack_p->multicast_slave_stats_size = PTP_MAX_MULTICAST_SLAVE_STATS_ENTRIES;

        /* Check if the stack supports Keystone */
        if (info->flags & BCM_PTP_STACK_EXTERNAL_TOP) {
#if defined(BCM_PTP_EXTERNAL_STACK_SUPPORT)
            if (BCM_FAILURE(rv = _bcm_ptp_external_stack_create(unit, info, stack_p->stack_id))) {
                PTP_ERROR_FUNC("_bcm_ptp_external_stack_create()");
                goto done;
            }
            stack_p->memstate = PTP_MEMSTATE_INITIALIZED;
#else
            rv = BCM_E_PARAM;
            goto done;
#endif
        } else {
#if defined(BCM_PTP_INTERNAL_STACK_SUPPORT)

            if (BCM_FAILURE(rv = _bcm_ptp_internal_stack_create(unit, info, stack_p->stack_id))) {
                stack_p->in_use = 0;
                next_stack_id[unit]--;
                goto done;
            }
            stack_p->memstate = PTP_MEMSTATE_INITIALIZED;
#else
            LOG_WARN(BSL_LS_BCM_PTP, (BSL_META_U(unit,
               "Neither PTP internal nor external stack support is enabled\n")
               ));
            rv = BCM_E_UNAVAIL;
            goto done;
#endif
        }
        if (stack_p->memstate != PTP_MEMSTATE_INITIALIZED) {
            LOG_WARN(BSL_LS_BCM_PTP, (BSL_META_U(unit,
                     "Found no supported PTP platforms\n\n")));

            rv = BCM_E_FAIL;
            goto done;
        }

#if defined(BCM_CMICM_SUPPORT) && defined(BCM_PTP_INTERNAL_STACK_SUPPORT)
        if (BCM_FAILURE(rv = _bcm_ptp_sw_cf_update_mode_config_send(unit,
                                 (soc_feature(unit, soc_feature_ptp_cf_sw_update) &&
                                  soc_property_get(unit, spn_PTP_CF_SW_UPDATE, 0)),
                                 stack_p->int_state.core_num))) {
            rv = BCM_E_FAIL;
            goto done;
        }

        if (BCM_FAILURE(rv = _bcm_ptp_ts0_ts1_combined_mode_config_send(unit,
            (stack_p->flags & BCM_PTP_STACK_TIMESTAMPER_COMBINED_MODE), stack_p->int_state.core_num))) {
            rv = BCM_E_FAIL;
            goto done;
        }
#endif

        if (BCM_FAILURE(rv = _bcm_ptp_rx_stack_create(                  /* call with blank MACs for Katana */
                unit, stack_p->stack_id, &stack_p->ext_info.host_mac,
                &stack_p->ext_info.top_mac, stack_p->ext_info.tpid,
                stack_p->ext_info.vlan))) {
            PTP_ERROR_FUNC("_bcm_ptp_rx_stack_create()");
            goto done;
        }

        if (BCM_FAILURE(rv = _bcm_ptp_clock_cache_stack_create(
            unit,stack_p->stack_id))) {
            PTP_ERROR_FUNC("_bcm_ptp_clock_cache_stack_create()");
            goto done;
        }

        if (BCM_FAILURE(rv = _bcm_ptp_management_stack_create(
                unit, stack_p->stack_id, &stack_p->ext_info.host_mac, 
                &stack_p->ext_info.top_mac, stack_p->ext_info.host_ip_addr, 
                stack_p->ext_info.top_ip_addr, stack_p->ext_info.vlan, 
                stack_p->ext_info.vlan_pri, pbmp))) {
            PTP_ERROR_FUNC("_bcm_ptp_management_stack_create()");
            goto done;
        }

        if (BCM_FAILURE(rv = _bcm_ptp_unicast_master_table_stack_create(
               unit, stack_p->stack_id))) {
            PTP_ERROR_FUNC("_bcm_ptp_unicast_master_table_stack_create()");
            goto done;
        }

        if (BCM_FAILURE(rv = _bcm_ptp_unicast_slave_table_stack_create(
                unit, stack_p->stack_id))) {
            PTP_ERROR_FUNC("_bcm_ptp_unicast_slave_table_stack_create()");
            goto done;
        }

        if (BCM_FAILURE(rv = _bcm_ptp_acceptable_master_table_stack_create(
                unit, stack_p->stack_id))) {
            PTP_ERROR_FUNC("_bcm_ptp_acceptable_master_table_stack_create()");
            goto done;
        }

        if (BCM_FAILURE(rv = _bcm_ptp_tunnel_queue_init(
                unit, stack_p->stack_id))) {
            PTP_ERROR_FUNC("_bcm_ptp_tunnel_queue_init()");
            goto done;
        }

        if (BCM_FAILURE(rv = _bcm_ptp_timestamp_queue_init(
                unit, stack_p->stack_id))) {
            PTP_ERROR_FUNC("_bcm_ptp_timestamp_queue_init()");
            goto done;
        }

        if (BCM_FAILURE(rv = _bcm_ptp_signaling_init(
                unit, stack_p->stack_id))) {
            PTP_ERROR_FUNC("_bcm_ptp_unicast_signaling_init()");
            goto done;
        }

        if (BCM_FAILURE(rv = bcm_common_ptp_telecom_g8265_init(
                unit, stack_p->stack_id, PTP_CLOCK_NUMBER_DEFAULT))) {
            PTP_ERROR_FUNC("bcm_common_ptp_telecom_g8265_init()");
            goto done;
        }

        if (BCM_FAILURE(rv = _bcm_ptp_ctdev_init(unit, stack_p->stack_id,
                PTP_CLOCK_NUMBER_DEFAULT, 0))) {
            PTP_ERROR_FUNC("_bcm_ptp_ctdev_init()");
            goto done;
        }

        /* T-DPLL support. */
        if (BCM_FAILURE(rv = bcm_tdpll_dpll_instance_init(unit, stack_p->stack_id))) {
            PTP_ERROR_FUNC("bcm_tdpll_dpll_instance_init()");
            goto done;
        }

        if (BCM_FAILURE(rv = bcm_tdpll_input_clock_init(unit, stack_p->stack_id))) {
            PTP_ERROR_FUNC("bcm_tdpll_input_clock_init()");
            goto done;
        }

        if (BCM_FAILURE(rv = bcm_tdpll_output_clock_init(unit, stack_p->stack_id))) {
            PTP_ERROR_FUNC("bcm_tdpll_output_clock_init()");
            goto done;
        }

        if (BCM_FAILURE(rv = bcm_esmc_init(unit, stack_p->stack_id))) {
            PTP_ERROR_FUNC("bcm_esmc_init()");
            goto done;
        }

        if (BCM_FAILURE(rv = bcm_tdpll_esmc_init(unit, stack_p->stack_id))) {
            PTP_ERROR_FUNC("bcm_tdpll_esmc_init()");
            goto done;
        }

done:
        BCM_IF_ERROR_RETURN(bcm_common_ptp_unlock(unit));
        if (rv == BCM_E_NONE) {
            LOG_VERBOSE(BSL_LS_BCM_PTP, (BSL_META_U(unit,
                        "PTP stack created\n")));
        }
    } else {
        rv = BCM_E_UNAVAIL;
        LOG_WARN(BSL_LS_BCM_PTP, (BSL_META_U(unit,
                 "PTP not supported on this unit\n")));
    }

    return rv;
}

/* bcm_ptp_stack_destroy: halts firmware (for internal stack) and undoes creation */
int bcm_common_ptp_stack_destroy(int unit, bcm_ptp_stack_id_t ptp_id)
{
    int rv;

    if (BCM_FAILURE(rv = _bcm_ptp_function_precheck(unit, ptp_id, 0, PTP_CLOCK_PORT_NUMBER_DEFAULT))) {
        PTP_ERROR_FUNC("PTP on unit not initialized.");
        return BCM_E_INIT;
    }

    BCM_IF_ERROR_RETURN(_bcm_ptp_stack_shutdown(unit, ptp_id));

    /* Halt the firmware if it is an internal stack */
#if defined(BCM_CMICM_SUPPORT) && defined(BCM_PTP_INTERNAL_STACK_SUPPORT)
    if (SOC_HAS_PTP_INTERNAL_STACK_SUPPORT(unit)) {
        _bcm_ptp_stack_info_t *stack_p = &_bcm_common_ptp_info[unit].stack_info[ptp_id];

        if ((stack_p->flags & BCM_PTP_STACK_EXTERNAL_TOP) == 0 && stack_p->in_use) {
            /* Stop the uc messaging before uc reset */
            soc_cmic_uc_msg_uc_stop(unit, stack_p->int_state.core_num);

            /* halt core running PTP: */
            soc_uc_reset(unit, stack_p->int_state.core_num);
        }
    }
#endif
    BCM_IF_ERROR_RETURN(_bcm_ptp_stack_cleanup(unit, ptp_id));

    return BCM_E_NONE;
}

#if defined(BCM_PTP_INTERNAL_STACK_SUPPORT)
/* Callback from MCS messaging layer: called before and after uC shutdown */
int _bcm_ptp_stack_deinit_callback(int unit,
                                   int uC,
                                   soc_cmic_uc_shutdown_stage_t stage,
                                   void *user_data)
{
    int ptp_id = PTR_TO_INT(user_data);

    LOG_VERBOSE(BSL_LS_BCM_PTP, (BSL_META_U(unit,
                "PTP stack deinit callback:  Unit: %d, ID: %d, stage: %d\n"),
                unit, ptp_id, (int)stage));

    switch (stage) {
    case SOC_CMIC_UC_SHUTDOWN_STARTING:
        /* Shut down threads */
        _bcm_ptp_stack_shutdown(unit, ptp_id);
        break;
    case SOC_CMIC_UC_SHUTDOWN_HALTED:
        /* Deallocate shared mem now that uC is halted */
        _bcm_ptp_stack_cleanup(unit, ptp_id);
        break;
    default:
        break;
    }

    return BCM_E_NONE;
}
#endif /* BCM_PTP_INTERNAL_STACK_SUPPORT */

int _bcm_ptp_stack_shutdown(int unit, bcm_ptp_stack_id_t ptp_id)
{
    int rv = BCM_E_FAIL;
    _bcm_ptp_stack_info_t *stack_p;

    rv = bcm_common_ptp_lock(unit);
    if (BCM_FAILURE(rv)) {
        return rv;
    }

    if (BCM_FAILURE(rv = _bcm_ptp_function_precheck(unit, ptp_id, 0, PTP_CLOCK_PORT_NUMBER_DEFAULT))) {
        PTP_ERROR_FUNC("PTP on unit not initialized.");
        goto done;
    }

    stack_p = &_bcm_common_ptp_info[unit].stack_info[ptp_id];
    if (!stack_p->in_use || stack_p->memstate != PTP_MEMSTATE_INITIALIZED) {
        PTP_ERROR_FUNC("PTP stack not in use.");
        goto done;
    }

#if defined(BCM_PTP_INTERNAL_STACK_SUPPORT)
    if ((stack_p->flags & BCM_PTP_STACK_EXTERNAL_TOP) == 0) {
        if (BCM_FAILURE(rv = _bcm_ptp_internal_stack_shutdown(unit, ptp_id))) {
            PTP_ERROR_FUNC("_bcm_ptp_internal_stack_shutdown()");
        }
    }
#endif /* BCM_PTP_INTERNAL_STACK_SUPPORT */

    stack_p->memstate = PTP_MEMSTATE_HALTING;

    if (BCM_FAILURE(rv = _bcm_ptp_tunnel_queue_cleanup(unit, ptp_id))) {
        PTP_ERROR_FUNC("_bcm_ptp_tunnel_queue_cleanup()");
        goto done;
    }

    if (BCM_FAILURE(rv = _bcm_ptp_timestamp_queue_cleanup(unit, ptp_id))) {
        PTP_ERROR_FUNC("_bcm_ptp_timestamp_queue_cleanup()");
        goto done;
    }

#ifdef BCM_PTP_EXT_SERVO_SUPPORT
    if (BCM_FAILURE(rv = _bcm_ptp_servo_deinit(unit, ptp_id, PTP_CLOCK_NUMBER_DEFAULT))) {
        PTP_ERROR_FUNC("_bcm_ptp_timestamp_queue_cleanup()");
        goto done;
    }
#endif /* BCM_PTP_EXT_SERVO_SUPPORT */

    if (BCM_FAILURE(rv = _bcm_ptp_signaling_cleanup(unit, ptp_id))) {
        PTP_ERROR_FUNC("_bcm_ptp_signaling_cleanup()");
        goto done;
    }

    if (BCM_FAILURE(rv = _bcm_ptp_acceptable_master_table_stack_destroy(unit, ptp_id))) {
        PTP_ERROR_FUNC("_bcm_ptp_acceptable_master_table_stack_destroy()");
        goto done;
    }

    if (BCM_FAILURE(rv = _bcm_ptp_unicast_slave_table_stack_destroy(unit, ptp_id))) {
        PTP_ERROR_FUNC("_bcm_ptp_unicast_slave_table_stack_destroy()");
        goto done;
    }

    if (BCM_FAILURE(rv = _bcm_ptp_unicast_master_table_stack_destroy(unit, ptp_id))) {
        PTP_ERROR_FUNC("_bcm_ptp_unicast_master_table_stack_destroy()");
        goto done;
    }

    if (BCM_FAILURE(rv = _bcm_ptp_management_stack_destroy(unit, ptp_id))) {
        PTP_ERROR_FUNC("_bcm_ptp_management_stack_destroy()");
        goto done;
    }

    if (BCM_FAILURE(rv = _bcm_ptp_clock_cache_stack_destroy(unit, ptp_id))) {
        PTP_ERROR_FUNC("_bcm_ptp_clock_cache_stack_destroy()");
        goto done;
    }

    if (BCM_FAILURE(rv = _bcm_ptp_rx_stack_destroy(unit, ptp_id))) {
        PTP_ERROR_FUNC("_bcm_ptp_rx_stack_destroy()");
        goto done;
    }

    if (BCM_FAILURE(rv = bcm_tdpll_esmc_cleanup(unit, ptp_id))) {
        PTP_ERROR_FUNC("bcm_tdpll_esmc_cleanup()");
        goto done;
    }

    if (BCM_FAILURE(rv = bcm_esmc_cleanup(unit, ptp_id))) {
        PTP_ERROR_FUNC("bcm_esmc_cleanup()");
        goto done;
    }

    if (BCM_FAILURE(rv = bcm_tdpll_output_clock_cleanup(unit, ptp_id))) {
        PTP_ERROR_FUNC("bcm_tdpll_output_clock_cleanup()");
        goto done;
    }

    if (BCM_FAILURE(rv = bcm_tdpll_input_clock_cleanup(unit))) {
        PTP_ERROR_FUNC("bcm_tdpll_input_clock_cleanup()");
        goto done;
    }

    if (BCM_FAILURE(rv = bcm_tdpll_dpll_instance_cleanup(unit, ptp_id))) {
        PTP_ERROR_FUNC("bcm_tdpll_instance_cleanup()");
        return rv;
    }

    if (BCM_FAILURE(rv = bcm_common_ptp_telecom_g8265_shutdown(unit))) {
        PTP_ERROR_FUNC("bcm_common_ptp_telecom_g8265_shutdown()");
        return rv;
    }

 done:
    BCM_IF_ERROR_RETURN(bcm_common_ptp_unlock(unit));
    if (rv == BCM_E_NONE) {
        LOG_VERBOSE(BSL_LS_BCM_PTP, (BSL_META_U(unit,
                    "PTP stack destroyed\n")));
    }

    return BCM_E_NONE;
}


/* _bcm_ptp_stack_cleanup:
   Cleans up after the remote-running stack has been terminated
*/
int _bcm_ptp_stack_cleanup(int unit, bcm_ptp_stack_id_t ptp_id)
{
    int rv = BCM_E_FAIL;
    _bcm_ptp_stack_info_t *stack_p;

    rv = bcm_common_ptp_lock(unit);
    if (BCM_FAILURE(rv)) {
        return rv;
    }

    if (_bcm_common_ptp_info[unit].memstate != PTP_MEMSTATE_INITIALIZED) {
        rv = BCM_E_INIT;
        goto done;
    }

    stack_p = &_bcm_common_ptp_info[unit].stack_info[ptp_id];

    if (stack_p->memstate != PTP_MEMSTATE_HALTING) {
        rv = BCM_E_FAIL;
        goto done;
    }

#if defined(BCM_PTP_INTERNAL_STACK_SUPPORT)
    if ((stack_p->flags & BCM_PTP_STACK_EXTERNAL_TOP) == 0) {
        _bcm_ptp_internal_stack_cleanup(unit, ptp_id);
    }
#endif /* BCM_PTP_INTERNAL_STACK_SUPPORT */

    /* only supporting a single stack per unit currently, so just reset */
    stack_p->in_use = 0;
    next_stack_id[unit] = 0;
    stack_p->memstate = PTP_MEMSTATE_STARTUP;

 done:
    BCM_IF_ERROR_RETURN(bcm_common_ptp_unlock(unit));
    return rv;
}


/* _bcm_ptp_running:
 *   Returns: BCM_E_NONE if PTP is running on the unit
 */
int _bcm_ptp_running(int unit)
{
    int ptp_id = 0;     /* Checks stack 0 on unit */
    _bcm_ptp_stack_info_t *stack_p = &_bcm_common_ptp_info[unit].stack_info[ptp_id];

    if (BCM_FAILURE(_bcm_ptp_function_precheck(unit, ptp_id, 0, 
                    PTP_CLOCK_PORT_NUMBER_DEFAULT))) {
        return BCM_E_UNAVAIL;
    }
        
    if (!stack_p->in_use || stack_p->memstate != PTP_MEMSTATE_INITIALIZED) {
         return BCM_E_UNAVAIL;
    }
        
    return BCM_E_NONE;
}
/*
 * Function:
 *      bcm_common_ptp_stack_get
 * Purpose:
 *      Get a PTP stack instance
 * Parameters:
 *      unit - Unit number
 *      ptp_id - ID of stack to get
 *      ptp_info - info structs for stack
 * Returns:
 *      int
 * Notes:
 */
int
bcm_common_ptp_stack_get(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    bcm_ptp_stack_info_t *ptp_info)
{
    _bcm_ptp_stack_info_t *stack_p;

    if (!ptp_info || ptp_id >= PTP_MAX_STACKS_PER_UNIT) {
        return BCM_E_PARAM;
    }
    stack_p = &_bcm_common_ptp_info[unit].stack_info[ptp_id];

    if (!stack_p->in_use) {
        return BCM_E_INIT;
    }

    ptp_info->id = ptp_id;
    ptp_info->flags = stack_p->flags;

#if defined(BCM_PTP_INTERNAL_STACK_SUPPORT)
    if (SOC_HAS_PTP_EXTERNAL_STACK_SUPPORT(unit)) {
        if (stack_p->flags & BCM_PTP_STACK_EXTERNAL_TOP) {
            ptp_info->ext_stack_info = &stack_p->ext_info;
        }
    }
#endif /* BCM_PTP_INTERNAL_STACK_SUPPORT */

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_common_ptp_stack_get_all
 * Purpose:
 *      Gets all PTP stack instances
 * Parameters:
 *      unit - Unit number
 *      max_size - maximum number of stacks to return
 *      ptp_info - info structs for stacks
 *      no_of_stacks - actual number of stacks that can be returned
 * Returns:
 *      int
 * Notes:
 */
int
bcm_common_ptp_stack_get_all(
    int unit,
    int max_size, 
    bcm_ptp_stack_info_t *ptp_info,
    int *no_of_stacks)
{
    int i;

    *no_of_stacks = next_stack_id[unit] = 0;
    for (i = 0; i < max_size && i < *no_of_stacks; ++i) {
        BCM_IF_ERROR_RETURN(bcm_common_ptp_stack_get(unit, i, &ptp_info[i]));
    }

    return BCM_E_NONE;
}

        
/*
 * Function:
 *      bcm_common_ptp_time_format_set
 * Purpose:
 *      Set Time Of Day format for PTP stack instance.
 * Parameters:
 *      unit - (IN) Unit number.
 *      ptp_info - (IN) PTP Stack ID
 *      format - (IN) Time of Day format
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int 
bcm_common_ptp_time_format_set(
    int unit, 
    bcm_ptp_stack_id_t ptp_id, 
    bcm_ptp_time_type_t type)
{
    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcm_common_ptp_cb_register
 * Purpose:
 *      Register a callback for handling PTP events
 * Parameters:
 *      unit - (IN) Unit number.
 *      cb_types - (IN) The set of PTP callbacks types for which the specified callback should be called
 *      cb - (IN) A pointer to the callback function to call for the specified PTP events
 *      user_data - (IN) Pointer to user data to supply in the callback
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int 
bcm_common_ptp_cb_register(
    int unit, 
    bcm_ptp_cb_types_t cb_types, 
    bcm_ptp_cb cb, 
    void *user_data)
{
    int rv = BCM_E_UNAVAIL;

    if (soc_feature(unit, soc_feature_ptp)) {
        rv = bcm_common_ptp_lock(unit);
        if (BCM_FAILURE(rv)) {
           return rv;
        }

        if (SHR_BITGET(cb_types.w, bcmPTPCallbackTypeManagement)) {
            rv = _bcm_ptp_register_management_callback(unit, cb, user_data);
        }

        if (SHR_BITGET(cb_types.w, bcmPTPCallbackTypeEvent)) {
            rv = _bcm_ptp_register_event_callback(unit, cb, user_data);
        }

        if (SHR_BITGET(cb_types.w, bcmPTPCallbackTypeFault)) {
            rv = _bcm_ptp_register_fault_callback(unit, cb, user_data);
        }

        if (SHR_BITGET(cb_types.w, bcmPTPCallbackTypeSignal)) {
            rv = _bcm_ptp_register_signal_callback(unit, cb, user_data);
        }

        if (SHR_BITGET(cb_types.w, bcmPTPCallbackTypeCheckPeers)) {
            rv = _bcm_ptp_register_peers_callback(unit, cb, user_data);
        }

        BCM_IF_ERROR_RETURN(bcm_common_ptp_unlock(unit));
    }

    return rv;
}

/*
 * Function:
 *      bcm_common_ptp_cb_unregister
 * Purpose:
 *      Unregister a callback for handling PTP events
 * Parameters:
 *      unit - (IN) Unit number.
 *      event_types - (IN) The set of PTP events to unregister for the specified callback
 *      cb - (IN) A pointer to the callback function to unregister from the specified PTP events
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int 
bcm_common_ptp_cb_unregister(
    int unit, 
    bcm_ptp_cb_types_t cb_types, 
    bcm_ptp_cb cb)
{
    int rv = BCM_E_UNAVAIL;

    if (soc_feature(unit, soc_feature_ptp)) {
        rv = bcm_common_ptp_lock(unit);
        if (BCM_FAILURE(rv)) {
           return rv;
        }

        if (SHR_BITGET(cb_types.w, bcmPTPCallbackTypeManagement)) {
            rv = _bcm_ptp_unregister_management_callback(unit);
        }

        if (SHR_BITGET(cb_types.w, bcmPTPCallbackTypeEvent)) {
            rv = _bcm_ptp_unregister_event_callback(unit);
        }

        if (SHR_BITGET(cb_types.w, bcmPTPCallbackTypeFault)) {
            rv = _bcm_ptp_unregister_fault_callback(unit);
        }

        if (SHR_BITGET(cb_types.w, bcmPTPCallbackTypeSignal)) {
            rv = _bcm_ptp_unregister_signal_callback(unit);
        }

        if (SHR_BITGET(cb_types.w, bcmPTPCallbackTypeCheckPeers)) {
            rv = _bcm_ptp_unregister_peers_callback(unit);
        }

        BCM_IF_ERROR_RETURN(bcm_common_ptp_unlock(unit));
    }

    return rv;
}

/*
 * Function:
 *      _bcm_ptp_clock_cache_init
 * Purpose:
 *      Initialize the PTP clock information caches a unit.
 * Parameters:
 *      unit - (IN) Unit number.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
_bcm_ptp_clock_cache_init(
    int unit)
{
    int rv = BCM_E_UNAVAIL;
    
    _bcm_ptp_stack_cache_t *stack_p;
    
    if (BCM_FAILURE(rv = _bcm_ptp_function_precheck(unit, PTP_STACK_ID_DEFAULT,
            PTP_CLOCK_NUMBER_DEFAULT, PTP_CLOCK_PORT_NUMBER_DEFAULT))) {
        PTP_ERROR_FUNC("_bcm_ptp_function_precheck()");  
        return rv;
    }
        
    
    if (NULL == _bcm_common_ptp_unit_array[unit].stack_array) {
        stack_p = sal_alloc(PTP_MAX_STACKS_PER_UNIT*
                            sizeof(_bcm_ptp_stack_cache_t),"Unit PTP Clock Information Caches");
    } else {
        stack_p = _bcm_common_ptp_unit_array[unit].stack_array;
    }
    
    if (!stack_p) {
        _bcm_common_ptp_unit_array[unit].memstate = PTP_MEMSTATE_FAILURE;
        return BCM_E_MEMORY;
    }

    memset(stack_p, 0x00, sizeof (_bcm_ptp_stack_cache_t));

    _bcm_common_ptp_unit_array[unit].stack_array = stack_p;
    _bcm_common_ptp_unit_array[unit].memstate = PTP_MEMSTATE_INITIALIZED;
    
    return rv;
}

/*
 * Function:
 *      _bcm_ptp_clock_cache_detach
 * Purpose:
 *      Shut down the PTP clock information caches a unit.
 * Parameters:
 *      unit - (IN) Unit number.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
_bcm_ptp_clock_cache_detach(
    int unit)
{
    if(_bcm_common_ptp_unit_array[unit].stack_array) {
        _bcm_common_ptp_unit_array[unit].memstate = PTP_MEMSTATE_STARTUP;
        sal_free(_bcm_common_ptp_unit_array[unit].stack_array);
        _bcm_common_ptp_unit_array[unit].stack_array = NULL;
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_ptp_clock_cache_stack_create
 * Purpose:
 *      Create the PTP clock information caches of a PTP stack.
 * Parameters:
 *      unit    - (IN) Unit number.
 *      ptp_id  - (IN) PTP stack ID.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
_bcm_ptp_clock_cache_stack_create(
    int unit, 
    bcm_ptp_stack_id_t ptp_id)
{
    int rv = BCM_E_UNAVAIL;
    int i;
    _bcm_ptp_clock_cache_t *clock_p;
    
    if (BCM_FAILURE(rv = _bcm_ptp_function_precheck(unit, ptp_id,
            PTP_CLOCK_NUMBER_DEFAULT, PTP_CLOCK_PORT_NUMBER_DEFAULT))) {
        PTP_ERROR_FUNC("_bcm_ptp_function_precheck()");  
        return rv;
    }
        
    if (_bcm_common_ptp_unit_array[unit].memstate != PTP_MEMSTATE_INITIALIZED) {
        return BCM_E_UNAVAIL;
    }

    
    clock_p = sal_alloc(PTP_MAX_CLOCK_INSTANCES*sizeof(_bcm_ptp_clock_cache_t),
                        "Stack PTP Information Caches");

    if (!clock_p) {
        _bcm_common_ptp_unit_array[unit].stack_array[ptp_id].memstate = 
            PTP_MEMSTATE_FAILURE;
        return BCM_E_MEMORY;
    }

    /* Mark the clocks in the newly-created clock array as not-in-use */
    for (i = 0; i < PTP_MAX_CLOCK_INSTANCES; ++i) {
        clock_p[i].in_use = 0;
    }

    _bcm_common_ptp_unit_array[unit].stack_array[ptp_id].clock_array = clock_p;
    _bcm_common_ptp_unit_array[unit].stack_array[ptp_id].memstate = 
        PTP_MEMSTATE_INITIALIZED;
    
    return rv;
}

/*
 * Function:
 *      _bcm_ptp_clock_cache_stack_destroy
 * Purpose:
 *      Destroy the PTP clock information caches of a PTP stack.
 * Parameters:
 *      unit    - (IN) Unit number.
 *      ptp_id  - (IN) PTP stack ID.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int
_bcm_ptp_clock_cache_stack_destroy(
    int unit,
    bcm_ptp_stack_id_t ptp_id)
{
    int rv;
    int i;
    _bcm_ptp_clock_cache_t *clock_array;

    if (BCM_FAILURE(rv = _bcm_ptp_function_precheck(unit, ptp_id,
            PTP_CLOCK_NUMBER_DEFAULT, PTP_CLOCK_PORT_NUMBER_DEFAULT))) {
        PTP_ERROR_FUNC("_bcm_ptp_function_precheck()");
        return rv;
    }

    if (_bcm_common_ptp_unit_array[unit].memstate != PTP_MEMSTATE_INITIALIZED) {
        return BCM_E_UNAVAIL;
    }

    if (_bcm_common_ptp_unit_array[unit].stack_array[ptp_id].memstate == PTP_MEMSTATE_INITIALIZED) {
        _bcm_common_ptp_unit_array[unit].stack_array[ptp_id].memstate = PTP_MEMSTATE_STARTUP;
        clock_array = _bcm_common_ptp_unit_array[unit].stack_array[ptp_id].clock_array;
        for (i = 0; i < PTP_MAX_CLOCK_INSTANCES; ++i) {
            if (clock_array->in_use == 1) {
                _bcm_ptp_mutex_destroy(clock_array[i].peerTable.lock);
            }
        }
        sal_free(clock_array);
        _bcm_common_ptp_unit_array[unit].stack_array[ptp_id].clock_array = 0;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_ptp_clock_cache_info_create
 * Purpose:
 *      Create the information cache of a PTP clock.
 * Parameters:
 *      unit      - (IN) Unit number.
 *      ptp_id    - (IN) PTP stack ID.
 *      clock_num - (IN) PTP clock number.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
_bcm_ptp_clock_cache_info_create(
    int unit, 
    bcm_ptp_stack_id_t ptp_id,
    int clock_num)
{
    int rv = BCM_E_UNAVAIL;
    _bcm_ptp_clock_cache_t *clock_cache;
    int i;

    if (BCM_FAILURE(rv = _bcm_ptp_function_precheck(unit, ptp_id, clock_num,
            PTP_CLOCK_PORT_NUMBER_DEFAULT))) {
        PTP_ERROR_FUNC("_bcm_ptp_function_precheck()");  
        return rv;
    }
        
    if ((_bcm_common_ptp_unit_array[unit].memstate != PTP_MEMSTATE_INITIALIZED) ||
        (_bcm_common_ptp_unit_array[unit].stack_array[ptp_id].memstate != 
            PTP_MEMSTATE_INITIALIZED)) {
        return BCM_E_UNAVAIL;
    }

    clock_cache = &_bcm_common_ptp_unit_array[unit].stack_array[ptp_id].clock_array[clock_num];

    sal_memset(&clock_cache->clock_info, 0, sizeof(bcm_ptp_clock_info_t));
    sal_memset(clock_cache->signal_output, 0, sizeof(bcm_ptp_signal_output_t) * PTP_MAX_OUTPUT_SIGNALS);
    sal_memset(clock_cache->tod_input, 0, sizeof(bcm_ptp_tod_input_t) * PTP_MAX_TOD_INPUTS);
    sal_memset(clock_cache->tod_output, 0, sizeof(bcm_ptp_tod_output_t) * PTP_MAX_TOD_OUTPUTS);
    sal_memset(clock_cache->port_info, 0, sizeof(bcm_ptp_clock_port_info_t) * PTP_MAX_CLOCK_INSTANCE_PORTS);
    sal_memset(clock_cache->portState, 0, sizeof(_bcm_ptp_port_state_t) * PTP_MAX_CLOCK_INSTANCE_PORTS);

    clock_cache->peerTable.num_peers = 0;
    if (!clock_cache->in_use) {
        clock_cache->peerTable.lock = _bcm_ptp_mutex_create("ptpstat.mutex");
        if (clock_cache->peerTable.lock == NULL) {
            return BCM_E_MEMORY;
        }
    }

    /* Initialize port states. */
    for (i = 0; i < PTP_MAX_CLOCK_INSTANCE_PORTS; ++i) {
        clock_cache->portState[i] = _bcm_ptp_state_initializing;
    }

    return rv;
}

/*
 * Function:
 *      _bcm_ptp_clock_cache_info_get
 * Purpose:
 *      Get the clock information cache of a PTP clock.
 * Parameters:
 *      unit       - (IN)  Unit number.
 *      ptp_id     - (IN)  PTP stack ID.
 *      clock_num  - (IN)  PTP clock number.
 *      clock_info - (OUT) PTP clock information cache.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int _bcm_ptp_clock_cache_info_get(
    int unit, 
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    bcm_ptp_clock_info_t *clock_info)
{
    int rv = BCM_E_UNAVAIL;
    
    if (BCM_FAILURE(rv = _bcm_ptp_function_precheck(unit, ptp_id, clock_num,
            PTP_CLOCK_PORT_NUMBER_DEFAULT))) {
        PTP_ERROR_FUNC("_bcm_ptp_function_precheck()");  
        return rv;
    }
        
    if ((_bcm_common_ptp_unit_array[unit].memstate != PTP_MEMSTATE_INITIALIZED) ||
        (_bcm_common_ptp_unit_array[unit].stack_array[ptp_id].memstate != 
            PTP_MEMSTATE_INITIALIZED)) {
        return BCM_E_UNAVAIL;
    }        

    *clock_info = _bcm_common_ptp_unit_array[unit].stack_array[ptp_id]
                                  .clock_array[clock_num].clock_info;
    
    return rv;
}

/*
 * Function:
 *      _bcm_ptp_clock_cache_info_set
 * Purpose:
 *      Set the clock information cache of a PTP clock.
 * Parameters:
 *      unit       - (IN) Unit number.
 *      ptp_id     - (IN) PTP stack ID.
 *      clock_num  - (IN) PTP clock number.
 *      clock_info - (IN) PTP clock information cache.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int _bcm_ptp_clock_cache_info_set(
    int unit, 
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    const bcm_ptp_clock_info_t *clock_info)
{
    int rv = BCM_E_UNAVAIL;
         
    if (BCM_FAILURE(rv = _bcm_ptp_function_precheck(unit, ptp_id, clock_num,
            PTP_CLOCK_PORT_NUMBER_DEFAULT))) {
        PTP_ERROR_FUNC("_bcm_ptp_function_precheck()");  
        return rv;
    }
        
    if ((_bcm_common_ptp_unit_array[unit].memstate != PTP_MEMSTATE_INITIALIZED) ||
        (_bcm_common_ptp_unit_array[unit].stack_array[ptp_id].memstate != 
            PTP_MEMSTATE_INITIALIZED)) {
        return BCM_E_UNAVAIL;
    }        

    _bcm_common_ptp_unit_array[unit].stack_array[ptp_id]
                    .clock_array[clock_num].clock_info = *clock_info;
    
    return rv;
}

int _bcm_ptp_clock_cache_signal_get(
    int unit, 
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    int signal_num,
    bcm_ptp_signal_output_t *signal)
{
    int rv = BCM_E_UNAVAIL;
    
    if (signal_num >= PTP_MAX_OUTPUT_SIGNALS || signal_num < 0) {
        return BCM_E_PARAM;
    }

    if (BCM_FAILURE(rv = _bcm_ptp_function_precheck(unit, ptp_id, clock_num,
            PTP_CLOCK_PORT_NUMBER_DEFAULT))) {
        PTP_ERROR_FUNC("_bcm_ptp_function_precheck()");  
        return rv;
    }
        
    if ((_bcm_common_ptp_unit_array[unit].memstate != PTP_MEMSTATE_INITIALIZED) ||
        (_bcm_common_ptp_unit_array[unit].stack_array[ptp_id].memstate != 
            PTP_MEMSTATE_INITIALIZED)) {
        return BCM_E_UNAVAIL;
    }        

    *signal = _bcm_common_ptp_unit_array[unit].stack_array[ptp_id].clock_array[clock_num].signal_output[signal_num];
    
    return rv;
}


int _bcm_ptp_clock_cache_signal_set(
    int unit, 
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    int signal_num,
    const bcm_ptp_signal_output_t *signal)
{
    int rv = BCM_E_UNAVAIL;
    
    if (signal_num >= PTP_MAX_OUTPUT_SIGNALS || signal_num < 0) {
        return BCM_E_PARAM;
    }

    if (BCM_FAILURE(rv = _bcm_ptp_function_precheck(unit, ptp_id, clock_num,
            PTP_CLOCK_PORT_NUMBER_DEFAULT))) {
        PTP_ERROR_FUNC("_bcm_ptp_function_precheck()");  
        return rv;
    }
        
    if ((_bcm_common_ptp_unit_array[unit].memstate != PTP_MEMSTATE_INITIALIZED) ||
        (_bcm_common_ptp_unit_array[unit].stack_array[ptp_id].memstate != 
            PTP_MEMSTATE_INITIALIZED)) {
        return BCM_E_UNAVAIL;
    }        

    _bcm_common_ptp_unit_array[unit].stack_array[ptp_id].clock_array[clock_num].signal_output[signal_num] = *signal;
    
    return rv;
}

/*
 * Function:
 *      _bcm_ptp_clock_cache_tod_input_get
 * Purpose:
 *      Get a ToD input configuration from PTP clock information cache.
 * Parameters:
 *      unit          - (IN)  Unit number.
 *      ptp_id        - (IN)  PTP stack ID.
 *      clock_num     - (IN)  PTP clock number.
 *      tod_input_num - (IN)  ToD input configuration number.
 *      tod_input     - (OUT) ToD input configuration.
 * Returns:
 *      BCM_E_XXX - Function status.
 * Notes:
 */
int _bcm_ptp_clock_cache_tod_input_get(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    int tod_input_num,
    bcm_ptp_tod_input_t *tod_input)
{
    int rv = BCM_E_UNAVAIL;

    if (tod_input_num >= PTP_MAX_TOD_INPUTS || tod_input_num < 0) {
        return BCM_E_PARAM;
    }

    if (BCM_FAILURE(rv = _bcm_ptp_function_precheck(unit, ptp_id, clock_num,
            PTP_CLOCK_PORT_NUMBER_DEFAULT))) {
        PTP_ERROR_FUNC("_bcm_ptp_function_precheck()");
        return rv;
    }

    if ((_bcm_common_ptp_unit_array[unit].memstate != PTP_MEMSTATE_INITIALIZED) ||
        (_bcm_common_ptp_unit_array[unit].stack_array[ptp_id].memstate !=
            PTP_MEMSTATE_INITIALIZED)) {
        return BCM_E_UNAVAIL;
    }

    *tod_input = _bcm_common_ptp_unit_array[unit].stack_array[ptp_id]
                                              .clock_array[clock_num]
                                              .tod_input[tod_input_num];

    return rv;
}

/*
 * Function:
 *      _bcm_ptp_clock_cache_tod_input_set
 * Purpose:
 *      Set a ToD input configuration in PTP clock information cache.
 * Parameters:
 *      unit          - (IN) Unit number.
 *      ptp_id        - (IN) PTP stack ID.
 *      clock_num     - (IN) PTP clock number.
 *      tod_input_num - (IN) ToD input configuration number.
 *      tod_input     - (IN) ToD input configuration.
 * Returns:
 *      BCM_E_XXX - Function status.
 * Notes:
 */
int _bcm_ptp_clock_cache_tod_input_set(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    int tod_input_num,
    const bcm_ptp_tod_input_t *tod_input)
{
    int rv = BCM_E_UNAVAIL;

    if (tod_input_num >= PTP_MAX_TOD_INPUTS || tod_input_num < 0) {
        return BCM_E_PARAM;
    }

    if (BCM_FAILURE(rv = _bcm_ptp_function_precheck(unit, ptp_id, clock_num,
            PTP_CLOCK_PORT_NUMBER_DEFAULT))) {
        PTP_ERROR_FUNC("_bcm_ptp_function_precheck()");
        return rv;
    }

    if ((_bcm_common_ptp_unit_array[unit].memstate != PTP_MEMSTATE_INITIALIZED) ||
        (_bcm_common_ptp_unit_array[unit].stack_array[ptp_id].memstate != 
            PTP_MEMSTATE_INITIALIZED)) {
        return BCM_E_UNAVAIL;
    }

    _bcm_common_ptp_unit_array[unit].stack_array[ptp_id]
                                 .clock_array[clock_num]
                                 .tod_input[tod_input_num] = *tod_input;

    return rv;
}

/*
 * Function:
 *      _bcm_ptp_clock_cache_tod_output_get
 * Purpose:
 *      Get a ToD output configuration from PTP clock information cache.
 * Parameters:
 *      unit           - (IN)  Unit number.
 *      ptp_id         - (IN)  PTP stack ID.
 *      clock_num      - (IN)  PTP clock number.
 *      tod_output_num - (IN)  ToD output configuration number.
 *      tod_output     - (OUT) ToD output configuration.
 * Returns:
 *      BCM_E_XXX - Function status.
 * Notes:
 */
int _bcm_ptp_clock_cache_tod_output_get(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    int tod_output_num,
    bcm_ptp_tod_output_t *tod_output)
{
    int rv = BCM_E_UNAVAIL;

    if (tod_output_num >= PTP_MAX_TOD_OUTPUTS || tod_output_num < 0) {
        return BCM_E_PARAM;
    }

    if (BCM_FAILURE(rv = _bcm_ptp_function_precheck(unit, ptp_id, clock_num,
            PTP_CLOCK_PORT_NUMBER_DEFAULT))) {
        PTP_ERROR_FUNC("_bcm_ptp_function_precheck()");
        return rv;
    }

    if ((_bcm_common_ptp_unit_array[unit].memstate != PTP_MEMSTATE_INITIALIZED) ||
        (_bcm_common_ptp_unit_array[unit].stack_array[ptp_id].memstate != 
            PTP_MEMSTATE_INITIALIZED)) {
        return BCM_E_UNAVAIL;
    }

    *tod_output = _bcm_common_ptp_unit_array[unit].stack_array[ptp_id]
                                               .clock_array[clock_num]
                                               .tod_output[tod_output_num];

    return rv;
}

/*
 * Function:
 *      _bcm_ptp_clock_cache_tod_output_set
 * Purpose:
 *      Set a ToD output configuration in PTP clock information cache.
 * Parameters:
 *      unit           - (IN) Unit number.
 *      ptp_id         - (IN) PTP stack ID.
 *      clock_num      - (IN) PTP clock number.
 *      tod_output_num - (IN) ToD output configuration number.
 *      tod_output     - (IN) ToD output configuration.
 * Returns:
 *      BCM_E_XXX - Function status.
 * Notes:
 */
int _bcm_ptp_clock_cache_tod_output_set(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    int tod_output_num,
    const bcm_ptp_tod_output_t *tod_output)
{
    int rv = BCM_E_UNAVAIL;

    if (tod_output_num >= PTP_MAX_TOD_OUTPUTS || tod_output_num < 0) {
        return BCM_E_PARAM;
    }

    if (BCM_FAILURE(rv = _bcm_ptp_function_precheck(unit, ptp_id, clock_num,
            PTP_CLOCK_PORT_NUMBER_DEFAULT))) {
        PTP_ERROR_FUNC("_bcm_ptp_function_precheck()");
        return rv;
    }

    if ((_bcm_common_ptp_unit_array[unit].memstate != PTP_MEMSTATE_INITIALIZED) ||
        (_bcm_common_ptp_unit_array[unit].stack_array[ptp_id].memstate != 
            PTP_MEMSTATE_INITIALIZED)) {
        return BCM_E_UNAVAIL;
    }

    _bcm_common_ptp_unit_array[unit].stack_array[ptp_id]
                                 .clock_array[clock_num]
                                 .tod_output[tod_output_num] = *tod_output;

    return rv;
}


extern int _bcm_ptp_set_max_unicast_masters(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int max_masters)
{
    _bcm_ptp_info_t *ptp_info_p;
    if (soc_feature(unit, soc_feature_ptp) == 0) {
        return BCM_E_UNAVAIL;
    }

    SET_PTP_INFO;
    if (ptp_info_p->memstate != PTP_MEMSTATE_INITIALIZED) {
        return BCM_E_PARAM;
    }

    ptp_info_p->stack_info->unicast_master_table_size = max_masters;
    return BCM_E_NONE;
}


extern int _bcm_ptp_set_max_acceptable_masters(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int max_masters)
{
    _bcm_ptp_info_t *ptp_info_p;
    if (soc_feature(unit, soc_feature_ptp) == 0) {
        return BCM_E_UNAVAIL;
    }

    SET_PTP_INFO;
    if (ptp_info_p->memstate != PTP_MEMSTATE_INITIALIZED) {
        return BCM_E_PARAM;
    }

    ptp_info_p->stack_info->acceptable_master_table_size = max_masters;
    return BCM_E_NONE;
}


extern int _bcm_ptp_set_max_unicast_slaves(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int max_slaves)
{
    _bcm_ptp_info_t *ptp_info_p;
    if (soc_feature(unit, soc_feature_ptp) == 0) {
        return BCM_E_UNAVAIL;
    }

    SET_PTP_INFO;
    if (ptp_info_p->memstate != PTP_MEMSTATE_INITIALIZED) {
        return BCM_E_PARAM;
    }

    ptp_info_p->stack_info->unicast_slave_table_size = max_slaves;
    return BCM_E_NONE;
}

extern int _bcm_ptp_set_max_multicast_slave_stats(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int max_entries)
{
    _bcm_ptp_info_t *ptp_info_p;
    if (soc_feature(unit, soc_feature_ptp) == 0) {
        return BCM_E_UNAVAIL;
    }

    SET_PTP_INFO;
    if (ptp_info_p->memstate != PTP_MEMSTATE_INITIALIZED) {
        return BCM_E_PARAM;
    }

    ptp_info_p->stack_info->multicast_slave_stats_size = max_entries;
    return BCM_E_NONE;
}


#endif /* defined(INCLUDE_PTP)*/
