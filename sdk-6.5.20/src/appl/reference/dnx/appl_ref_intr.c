/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Purpose:    Implements application interrupt lists for DNX.
 */

/*************
 * INCLUDES  *
 */
#include <shared/bsl.h>
#include <bcm/error.h>
#include <bcm/debug.h>
#include <bcm/types.h>
#include <bcm/switch.h>
#include <bcm_int/common/debug.h>
#include <shared/shrextend/shrextend_debug.h>
#include <soc/drv.h>

#include <soc/dnx/dnx_data/auto_generated/dnx_data_intr.h>
#include <soc/dnx/swstate/auto_generated/access/interrupt_access.h>

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

/* } */
/*************
 * GLOBALS   *
 */
/* { */

/*************
 * FUNCTIONS *
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
dnx_interrupt_appl_log_defaults_set(
    int unit)
{
    int rv, i;
    bcm_switch_event_control_t event;
    const dnxc_data_table_info_t *table_info;
    const dnx_data_intr_general_disable_print_on_init_t *intr_disable_print;
    uint32 nof_interrupts;
    SHR_FUNC_INIT_VARS(unit);

    event.action = bcmSwitchEventLog;
    event.index = 0;
    event.event_id = BCM_SWITCH_EVENT_CONTROL_ALL;

    rv = bcm_switch_event_control_set(unit, BCM_SWITCH_EVENT_DEVICE_INTERRUPT, event, 0x1);
    SHR_IF_ERR_EXIT(rv);

    nof_interrupts = dnx_data_intr.general.nof_interrupts_get(unit);
    table_info = dnx_data_intr.general.disable_print_on_init_info_get(unit);

    if (table_info->key_size[0] > nof_interrupts)
    {
        LOG_ERROR(BSL_LS_APPLDNX_INTR, (BSL_META_U(unit, "ERROR: Interrupts disable print key too large\n")));
        SHR_IF_ERR_EXIT(_SHR_E_INTERNAL);
    }
    for (i = 0; i < table_info->key_size[0]; i++)
    {
        intr_disable_print = dnx_data_intr.general.disable_print_on_init_get(unit, i);
        if (intr_disable_print->intr < nof_interrupts)
        {
            event.event_id = intr_disable_print->intr;
            rv = bcm_switch_event_control_set(unit, BCM_SWITCH_EVENT_DEVICE_INTERRUPT, event, 0);
            SHR_IF_ERR_EXIT(rv);
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
dnx_interrupt_appl_mask_defaults_set(
    int unit)
{
    int rv, i;
    bcm_switch_event_control_t event;
    const dnxc_data_table_info_t *table_info;
    const dnx_data_intr_general_active_on_init_t *intr_active;
    const dnx_data_intr_general_disable_on_init_t *intr_disable;
    uint32 nof_interrupts;

    SHR_FUNC_INIT_VARS(unit);

    if (!SOC_INTR_IS_SUPPORTED(unit))
    {
        LOG_ERROR(BSL_LS_APPLDNX_INTR, (BSL_META_U(unit, "ERROR: No interrupts for device\n")));
        SHR_IF_ERR_EXIT(SOC_E_UNAVAIL);
    }

    nof_interrupts = dnx_data_intr.general.nof_interrupts_get(unit);

    /*
     * Interrupts disable on init
     */
    table_info = dnx_data_intr.general.disable_on_init_info_get(unit);
    if (table_info->key_size[0] > nof_interrupts)
    {
        LOG_ERROR(BSL_LS_APPLDNX_INTR, (BSL_META_U(unit, "ERROR: Interrupts disable on init key too large\n")));
        SHR_IF_ERR_EXIT(_SHR_E_INTERNAL);
    }
    for (i = 0; i < table_info->key_size[0]; i++)
    {
        intr_disable = dnx_data_intr.general.disable_on_init_get(unit, i);
        if (intr_disable->intr < nof_interrupts)
        {
            event.event_id = intr_disable->intr;
            event.action = bcmSwitchEventMask;
            event.index = BCM_CORE_ALL;
            rv = bcm_switch_event_control_set(unit, BCM_SWITCH_EVENT_DEVICE_INTERRUPT, event, 1);
            SHR_IF_ERR_EXIT(rv);
        }
    }

    /*
     *  Interrupts active on init
     */
    table_info = dnx_data_intr.general.active_on_init_info_get(unit);
    if (table_info->key_size[0] > nof_interrupts)
    {
        LOG_ERROR(BSL_LS_APPLDNX_INTR, (BSL_META_U(unit, "ERROR: Interrupts enable on init key too large\n")));
        SHR_IF_ERR_EXIT(_SHR_E_INTERNAL);
    }
    for (i = 0; i < table_info->key_size[0]; i++)
    {
        intr_active = dnx_data_intr.general.active_on_init_get(unit, i);
        if (intr_active->intr < nof_interrupts)
        {
            event.event_id = intr_active->intr;
            event.action = bcmSwitchEventMask;
            event.index = BCM_CORE_ALL;
            rv = bcm_switch_event_control_set(unit, BCM_SWITCH_EVENT_DEVICE_INTERRUPT, event, 0);
            SHR_IF_ERR_EXIT(rv);
        }
    }
exit:
    SHR_FUNC_EXIT;
}

#undef BSL_LOG_MODULE
