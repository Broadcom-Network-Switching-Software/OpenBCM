/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/*************
 * INCLUDES  *
 */
#include <shared/bsl.h>
#include <bcm/error.h>
#include <bcm/debug.h>
#include <bcm/switch.h>
#include <bcm/cosq.h>
#include <bcm_int/common/debug.h>
#include <soc/drv.h>
#include <appl/diag/system.h>
#include <appl/reference/dnxc/appl_ref_intr.h>
#include <shared/shrextend/shrextend_debug.h>
#if defined BCM_DNX_SUPPORT
#include <soc/sand/sand_aux_access.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_ingr_congestion.h>
#endif
#include <soc/dnxc/dnxc_ha.h>
#include <soc/dnx/intr/auto_generated/jr2/jr2_intr.h>
#include <soc/dnx/intr/auto_generated/j2c/j2c_intr.h>
#include <soc/dnx/intr/auto_generated/q2a/q2a_intr.h>
#include <soc/dnx/intr/auto_generated/j2p/j2p_intr.h>
#include <soc/dnxc/dnxc_intr_handler.h>

/*************
 * DEFINES   *
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_APPLDNX_INTR

/*************
 * TYPE DEFS *
 */

/*************
 * FUNCTIONS *
 */

/**
 * \brief - enable interrupt reference log except which explicit disabled
 *
 * \param [in] unit - unit #
 * \return
 * \see
 *   * None
 */
int
appl_dnxc_interrupt_appl_log_defaults_set(
    int unit)
{
    int rv;
    bcm_switch_event_control_t event;

    SHR_FUNC_INIT_VARS(unit);
#ifdef BCM_WARM_BOOT_SUPPORT
    if (!SOC_WARM_BOOT(unit))
#endif
    {
        event.action = bcmSwitchEventLog;
        event.index = 0;
        event.event_id = BCM_SWITCH_EVENT_CONTROL_ALL;

        rv = bcm_switch_event_control_set(unit, BCM_SWITCH_EVENT_DEVICE_INTERRUPT, event, 0x1);
        SHR_IF_ERR_EXIT(rv);

#ifdef BCM_DNXF_SUPPORT
        if (SOC_IS_DNXF(unit))
        {
            SHR_IF_ERR_EXIT(dnxf_interrupt_appl_log_defaults_set(unit));
        }
        else
#endif
#ifdef BCM_DNX_SUPPORT
        if (SOC_IS_DNX(unit))
        {
            SHR_IF_ERR_EXIT(dnx_interrupt_appl_log_defaults_set(unit));
        }
        else
#endif
        {
            SHR_IF_ERR_EXIT(_SHR_E_UNAVAIL);
        }
    }
exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief - mask/unmask interrupt which explicit declaired
 *
 * \param [in] unit - unit #
 * \return
 * \see
 *   * None
 */
int
appl_dnxc_interrupt_appl_mask_defaults_set(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    if (!SOC_INTR_IS_SUPPORTED(unit))
    {
        LOG_ERROR(BSL_LS_APPLDNX_INTR, (BSL_META_U(unit, "ERROR: No interrupts for device\n")));
        SHR_IF_ERR_EXIT(_SHR_E_UNAVAIL);
    }

#ifdef BCM_WARM_BOOT_SUPPORT
    if (!SOC_WARM_BOOT(unit))
#endif
    {
#ifdef BCM_DNXF_SUPPORT
        if (SOC_IS_DNXF(unit))
        {
            SHR_IF_ERR_EXIT(dnxf_interrupt_appl_mask_defaults_set(unit));
        }
        else
#endif
#ifdef BCM_DNX_SUPPORT
        if (SOC_IS_DNX(unit))
        {
            SHR_IF_ERR_EXIT(dnx_interrupt_appl_mask_defaults_set(unit));
        }
        else
#endif
        {
            LOG_ERROR(BSL_LS_APPLDNX_INTR, (BSL_META_U(unit, "ERROR: Only support DNX and DNXF device\n")));
            SHR_IF_ERR_EXIT(_SHR_E_UNAVAIL);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/** Functions handling interrupts for devices JR2 and above.*/
#if defined BCM_DNX_SUPPORT
static int
appl_dnxc_intr_handle_PbVsqPgCongestionFifoOrdy(
    int unit,
    bcm_core_t core_id,
    uint32 interrupt_id)
{
    bcm_cosq_inst_congestion_info_key_t cong_key;
    bcm_cosq_inst_congestion_info_data_t cong_info;
    uint32 flags = 0;
    int fifo_size = dnx_data_ingr_congestion.vsq.vsq_e_congestion_notification_fifo_size_get(unit);
    int fifo_index;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Get Congestion FIFO info
     * We read at most the size of the FIFO, as the FIFO can be filled during read.
     * Here we only print the value.
     */
    for (fifo_index = 0; fifo_index < fifo_size; fifo_index++)
    {
        cong_key.core_id = core_id;
        cong_key.type = bcmCosqInstCongestionInfoTypePgVsq;
        SHR_IF_ERR_EXIT(bcm_cosq_inst_congestion_info_get(unit, flags, &cong_key, &cong_info));

        if (!cong_info.valid)
        {
            break;
        }

        sal_printf("CGM_CONGESTION_PbVsqPgCongestionFifoOrdy, "
                   "core ID %d, valid: %d, resource type: %d, PG index: %d\n",
                   core_id, cong_info.valid, cong_info.resource, cong_info.id);
    }

    sal_printf("CGM_CONGESTION_PbVsqPgCongestionFifoOrdy, core ID %d, read %d notifications.\n", core_id, fifo_index);

exit:
    SHR_FUNC_EXIT;
}

static int
appl_dnxc_intr_handle_PbVsqLlfcCongestionFifoOrdy(
    int unit,
    bcm_core_t core_id,
    uint32 interrupt_id)
{
    bcm_cosq_inst_congestion_info_key_t cong_key;
    bcm_cosq_inst_congestion_info_data_t cong_info;
    uint32 flags = 0;
    int fifo_size = dnx_data_ingr_congestion.vsq.vsq_f_congestion_notification_fifo_size_get(unit);
    int fifo_index;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Get Congestion FIFO info
     * We read at most the size of the FIFO, as the FIFO can be filled during read.
     * Here we only print the value.
     */
    for (fifo_index = 0; fifo_index < fifo_size; fifo_index++)
    {
        cong_key.core_id = core_id;
        cong_key.type = bcmCosqInstCongestionInfoTypeLlfcVsq;
        SHR_IF_ERR_EXIT(bcm_cosq_inst_congestion_info_get(unit, flags, &cong_key, &cong_info));

        if (!cong_info.valid)
        {
            break;
        }

        sal_printf("CGM_CONGESTION_PbVsqLlfcCongestionFifoOrdy, "
                   "core ID %d, valid: %d, resource type: %d, port index: %d, Pool Id: %d\n",
                   core_id, cong_info.valid, cong_info.resource, cong_info.id, cong_info.pool_id);
    }

    sal_printf("CGM_CONGESTION_PbVsqLlfcCongestionFifoOrdy, core ID %d, read %d notifications.\n", core_id, fifo_index);

exit:
    SHR_FUNC_EXIT;
}

static int
appl_dnxc_intr_handle_VoqCongestionFifoOrdy(
    int unit,
    bcm_core_t core_id,
    uint32 interrupt_id)
{
    bcm_cosq_inst_congestion_info_key_t cong_key;
    bcm_cosq_inst_congestion_info_data_t cong_info;
    uint32 flags = 0;
    int fifo_size = dnx_data_ingr_congestion.voq.voq_congestion_notification_fifo_size_get(unit);
    int fifo_index;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Get Congestion FIFO info
     * We read at most the size of the FIFO, as the FIFO can be filled during read.
     * Here we only print the value.
     */
    for (fifo_index = 0; fifo_index < fifo_size; fifo_index++)
    {
        cong_key.core_id = core_id;
        cong_key.type = bcmCosqInstCongestionInfoTypeVoq;
        SHR_IF_ERR_EXIT(bcm_cosq_inst_congestion_info_get(unit, flags, &cong_key, &cong_info));

        if (!cong_info.valid)
        {
            break;
        }

        sal_printf("CGM_CONGESTION_VoqCongestionFifoOrdy, "
                   "core ID %d, valid: %d, resource type: %d, Queue index: %d\n",
                   core_id, cong_info.valid, cong_info.resource, cong_info.id);
    }

    sal_printf("CGM_CONGESTION_VoqCongestionFifoOrdy, core ID %d, read %d notifications.\n", core_id, fifo_index);

exit:
    SHR_FUNC_EXIT;
}

/** End of functions handling interrupts for JR2 and above.*/
#endif

static void
appl_dnxc_interrupt_appl_cb_overriding_action(
    int unit,
    uint32 interrupt_id,
    bcm_core_t core_id,
    void *userdata)
{
    /*
     * Perform actions for JR2 and above.
     */
#if defined BCM_DNX_SUPPORT
    if (SOC_IS_DNX(unit))
    {
        int rc = _SHR_E_NONE;
        /*
         * Perform the action relevant for the specific interrupt.
         */
        if (dnxc_int_name_to_id(unit, "CGM_CONGESTION_PB_VSQ_PG_CONGESTION_FIFO_ORDY") == interrupt_id)
        {
            rc = appl_dnxc_intr_handle_PbVsqPgCongestionFifoOrdy(unit, core_id, interrupt_id);
        }
        else if (dnxc_int_name_to_id(unit, "CGM_CONGESTION_PB_VSQ_LLFC_CONGESTION_FIFO_ORDY") == interrupt_id)
        {
            rc = appl_dnxc_intr_handle_PbVsqLlfcCongestionFifoOrdy(unit, core_id, interrupt_id);
        }
        else if (dnxc_int_name_to_id(unit, "CGM_CONGESTION_VOQ_CONGESTION_FIFO_ORDY") == interrupt_id)
        {
            rc = appl_dnxc_intr_handle_VoqCongestionFifoOrdy(unit, core_id, interrupt_id);
        }

        if (SHR_FAILURE(rc))
        {
            sal_printf("Error performing action for interrupt %d core ID %d unit %d.\n", interrupt_id, core_id, unit);
        }
    }
#endif

    return;
}

void
appl_dnxc_interrupt_appl_cb(
    int unit,
    bcm_switch_event_t event,
    uint32 arg1,
    uint32 arg2,
    uint32 arg3,
    void *userdata)
{
    if (BCM_SWITCH_EVENT_DEVICE_INTERRUPT != event)
    {
        return;
    }

    if (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "ser_reset_cb_en", 0))
    {
        LOG_DEBUG(BSL_LS_APPLDNX_INTR,
                  (BSL_META_U(unit, "user callback called for interrupt number %u, instance %u, reset %u\n"), arg1,
                   arg2, arg3));
    }
    else
    {
        LOG_DEBUG(BSL_LS_APPLDNX_INTR,
                  (BSL_META_U(unit, "user callback called for interrupt number %u, instance %u\n"), arg1, arg2));
    }

    appl_dnxc_interrupt_appl_cb_overriding_action(unit, arg1, arg2, userdata);

    return;
}
/**
 * \brief - init interrupt application for reference
 *
 * \param [in] unit - unit #
 * \return
 * \see
 *   * None
 */
int
appl_dnxc_interrupt_init(
    int unit)
{
    int rc;
    bcm_switch_event_control_t bcm_switch_event_control;

    SHR_FUNC_INIT_VARS(unit);

    memset((void *) &bcm_switch_event_control, 0, sizeof(bcm_switch_event_control_t));

    rc = bcm_switch_event_register(unit, appl_dnxc_interrupt_appl_cb, NULL);
    SHR_IF_ERR_EXIT(rc);

    rc = appl_dnxc_interrupt_appl_log_defaults_set(unit);
    SHR_IF_ERR_EXIT(rc);

    rc = appl_dnxc_interrupt_appl_mask_defaults_set(unit);
    SHR_IF_ERR_EXIT(rc);

    /*
     * Enable all interrupts
     */
    bcm_switch_event_control.action = bcmSwitchEventMask;
    bcm_switch_event_control.event_id = BCM_SWITCH_EVENT_CONTROL_ALL;
    rc = bcm_switch_event_control_set(unit, BCM_SWITCH_EVENT_DEVICE_INTERRUPT, bcm_switch_event_control, 0x0);
    SHR_IF_ERR_EXIT(rc);

exit:
    SHR_FUNC_EXIT;
}

static int
appl_dnxc_interrupt_deinit_internal(
    int unit,
    int cache_upd_req)
{
    int rc;
    bcm_switch_event_control_t bcm_switch_event_control;

    SHR_FUNC_INIT_VARS(unit);
    memset((void *) &bcm_switch_event_control, 0, sizeof(bcm_switch_event_control_t));
    rc = bcm_switch_event_unregister(unit, appl_dnxc_interrupt_appl_cb, NULL);
    SHR_IF_ERR_EXIT(rc);

    if (cache_upd_req)
    {
        /** Allow writing to registers during deinit */
        /** During deinit the SDK tries to update all memories cache.
            this may expose ECC errors which were hidden and the interrupt handler tries to fix
            them.
            The motivation is to avoid risk of 2 bits ecc when driver is down
            (in case next step will be WB). */
        SHR_IF_ERR_EXIT(dnxc_ha_tmp_allow_access_enable(unit, UTILEX_SEQ_ALLOW_SCHAN));

        /*
         * Update cached memories to detect ECC and parity errors
         */
        rc = bcm_switch_control_set(unit, bcmSwitchCacheTableUpdateAll, 0);
        SHR_IF_ERR_EXIT(rc);

        /** Return to normal mode */
        SHR_IF_ERR_EXIT(dnxc_ha_tmp_allow_access_disable(unit, UTILEX_SEQ_ALLOW_SCHAN));

    }

    /*
     * Disable all interrupts
     */
    bcm_switch_event_control.action = bcmSwitchEventMask;
    bcm_switch_event_control.event_id = BCM_SWITCH_EVENT_CONTROL_ALL;
    rc = bcm_switch_event_control_set(unit, BCM_SWITCH_EVENT_DEVICE_INTERRUPT, bcm_switch_event_control, 0x1);
    SHR_IF_ERR_EXIT(rc);
exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief - deinit interrupt application for reference
 *
 * \param [in] unit - unit #
 * \return
 * \see
 *   * None
 */
int
appl_dnxc_interrupt_deinit(
    int unit)
{
#ifdef ADAPTER_SERVER_MODE
    /** adapter do not need to update the caching */
    return (appl_dnxc_interrupt_deinit_internal(unit, 0));
#else
    return (appl_dnxc_interrupt_deinit_internal(unit, 1));
#endif
}

int
appl_dnxc_interrupt_nocache_deinit(
    int unit)
{
    return (appl_dnxc_interrupt_deinit_internal(unit, 0));
}

#undef BSL_LOG_MODULE
