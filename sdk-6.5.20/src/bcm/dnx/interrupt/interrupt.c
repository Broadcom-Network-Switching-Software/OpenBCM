/** \file interrupt.c
 * 
 *
 * Interrupt Init/Deinit functions for DNX. \n
 */

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#  error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_INTR

#include <shared/shrextend/shrextend_debug.h>
#include <soc/memory.h>
#include <soc/feature.h>
#include <soc/types.h>
#include <soc/register.h>
#include <soc/dnxc/drv_dnxc_utils.h>

#include <shared/bsl.h>
#include <soc/intr.h>
#include <soc/iproc.h>
#include <soc/dnxc/intr.h>
#include <soc/dnxc/dnxc_intr_handler.h>
#include <soc/dnxc/swstate/dnxc_sw_state_utils.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_intr.h>
#include <soc/dnx/swstate/auto_generated/access/interrupt_access.h>

#include <soc/dnxc/dnxc_intr.h>
#include <soc/sand/sand_aux_access.h>

/**
 * \brief
 *   Initialize sw_state database.
 */
static shr_error_e
dnx_intr_sw_state_db_init(
    int unit)
{
    uint32 nof_interrupts = 0;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Init dnx_fabric_db
     */
    SHR_IF_ERR_EXIT(intr_db.init(unit));

    /*
     * Alloc groups
     */
    nof_interrupts = dnx_data_intr.general.nof_interrupts_get(unit);

    SHR_IF_ERR_EXIT(intr_db.storm_timed_count.alloc(unit, nof_interrupts));
    SHR_IF_ERR_EXIT(intr_db.flags.alloc(unit, nof_interrupts));
    SHR_IF_ERR_EXIT(intr_db.storm_timed_period.alloc(unit, nof_interrupts));

exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief
 *   Set default value for storm interrupt.
 */
static shr_error_e
dnx_intr_db_default_set(
    int unit)
{
    uint32 nof_interrupts;
    uint32 ii;
    SHR_FUNC_INIT_VARS(unit);

    nof_interrupts = dnx_data_intr.general.nof_interrupts_get(unit);
    for (ii = 0; ii < nof_interrupts; ii++)
    {
        SHR_IF_ERR_EXIT(intr_db.flags.set(unit, ii, 0));
        SHR_IF_ERR_EXIT(intr_db.storm_timed_count.set(unit, ii, 0));
        SHR_IF_ERR_EXIT(intr_db.storm_timed_period.set(unit, ii, 0));
    }
    /*
     * configure storm nominal default value 0
     */
    SHR_IF_ERR_EXIT(intr_db.storm_nominal.set(unit, 0));

exit:
    SHR_FUNC_EXIT;
}
/** See .h file */
shr_error_e
dnx_intr_init(
    int unit)
{
    uint32 value;

    SHR_FUNC_INIT_VARS(unit);

    if (!sw_state_is_warm_boot(unit))
    {
        /*
         * init sw state db 
         */
        SHR_IF_ERR_EXIT(dnx_intr_sw_state_db_init(unit));

        SHR_IF_ERR_EXIT(dnx_intr_db_default_set(unit));
    }
    else
    {
        /*
         * restore storm nominal value
         */
        uint32 *switch_event_nominal_storm_p;

        SHR_IF_ERR_EXIT(intr_db.storm_nominal.get(unit, &value));
        SHR_IF_ERR_EXIT(soc_control_element_address_get
                        (unit, SWITCH_EVENT_NOMINAL_STORM, (void **) (&switch_event_nominal_storm_p)));
        *switch_event_nominal_storm_p = value;
    }

    /*
     * Init interrupts
     */
    SHR_IF_ERR_EXIT(soc_dnxc_interrupts_init(unit));
exit:
    SHR_FUNC_EXIT;
}

/*
 * \brief
 *   Deinitialize sw_state database.
 */
static shr_error_e
dnx_intr_sw_state_db_deinit(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * sw state module deinit is done automatically at device deinit 
     */

    SHR_FUNC_EXIT;
}

/** See .h file */
shr_error_e
dnx_intr_deinit(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Deinit sw state db
     */
    SHR_IF_ERR_EXIT(dnx_intr_sw_state_db_deinit(unit));

    /*
     * Deinit interrupts
     */
    SHR_IF_ERR_EXIT(soc_dnxc_interrupts_deinit(unit));
exit:
    SHR_FUNC_EXIT;
}

#undef BSL_LOG_MODULE
