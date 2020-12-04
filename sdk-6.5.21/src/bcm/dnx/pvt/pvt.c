/** \file pvt.c
 *
 * Handles the PVT monitors functionalities for DNX.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_PVT

/*
 * Include files currently used for DNX.
 * {
 */
#include <shared/shrextend/shrextend_debug.h>
#include <soc/dnx/dbal/dbal.h>
#include <bcm_int/dnx/field/field_entry.h>
#include <bcm/error.h>
#include <bcm/debug.h>
#include <bcm/switch.h>

/*
 * }
 */
/*
 * Include files.
 * {
 */
#include <bcm/types.h>
#include <shared/bslenum.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_pvt.h>

/*
 * }
 */

/*
 * DEFINEs
 * {
 */

/*
 * }
 */

/*
 * MACROs
 * {
 */

/*
 * }
 */

/**
 * \brief
 *   Initialize PVT monitor module.
 *
 * \param [in] unit -
 *   The unit number.
 * \return
 *   See \ref shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e
dnx_pvt_mon_init(
    int unit)
{
    int nof_pvt_mon;
    uint32 entry_handle_id;
    int i;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    nof_pvt_mon = dnx_data_pvt.general.nof_pvt_monitors_get(unit);

    /*
     * Init
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_PVT_MON_ENABLE, &entry_handle_id));
    for (i = 0; i < nof_pvt_mon; i++)
    {
        dbal_entry_value_field64_set(unit, entry_handle_id, DBAL_FIELD_PVT_MON_CONTROL, i, 0);
        dbal_entry_value_field64_set(unit, entry_handle_id, DBAL_FIELD_PVT_MON_CONTROL_POWERDOWN, i, 0);
        dbal_entry_value_field64_set(unit, entry_handle_id, DBAL_FIELD_PVT_MON_CONTROL_VAVSMON_PWRDN, i, 0);
        dbal_entry_value_field64_set(unit, entry_handle_id, DBAL_FIELD_PVT_MON_CONTROL_SEL, i, 0);
    }
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    DBAL_HANDLE_FREE(unit, entry_handle_id);

    /*
     * Powerdown
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_PVT_MON_ENABLE, &entry_handle_id));
    for (i = 0; i < nof_pvt_mon; i++)
    {
        dbal_entry_value_field16_set(unit, entry_handle_id, DBAL_FIELD_PVT_MON_CONTROL_POWERDOWN, i, 1);
    }
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    DBAL_HANDLE_FREE(unit, entry_handle_id);

    /*
     * Powerup
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_PVT_MON_ENABLE, &entry_handle_id));
    for (i = 0; i < nof_pvt_mon; i++)
    {
        dbal_entry_value_field16_set(unit, entry_handle_id, DBAL_FIELD_PVT_MON_CONTROL_POWERDOWN, i, 0);
    }
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    DBAL_HANDLE_FREE(unit, entry_handle_id);

    /*
     * Reset
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_PVT_MON_ENABLE, &entry_handle_id));
    for (i = 0; i < nof_pvt_mon; i++)
    {
        dbal_entry_value_field16_set(unit, entry_handle_id, DBAL_FIELD_PVT_MON_CONTROL_RESET, i, 1);
    }
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    DBAL_HANDLE_FREE(unit, entry_handle_id);

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/** see header file */
shr_error_e
dnx_pvt_mon_vol_change(
    int unit,
    int voltage)
{
    int nof_pvt_mon;
    uint32 entry_handle_id;
    int i;
    uint16 sel_val;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    nof_pvt_mon = dnx_data_pvt.general.nof_pvt_monitors_get(unit);
    sel_val = voltage ? dnx_data_pvt.general.vol_sel_get(unit) : 0;
    /*
     * Init
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_PVT_MON_ENABLE, &entry_handle_id));
    for (i = 0; i < nof_pvt_mon; i++)
    {
        dbal_entry_value_field64_set(unit, entry_handle_id, DBAL_FIELD_PVT_MON_CONTROL, i, 0);
        dbal_entry_value_field16_set(unit, entry_handle_id, DBAL_FIELD_PVT_MON_CONTROL_POWERDOWN, i, 0);
        dbal_entry_value_field16_set(unit, entry_handle_id, DBAL_FIELD_PVT_MON_CONTROL_VAVSMON_PWRDN, i, 0);
        dbal_entry_value_field16_set(unit, entry_handle_id, DBAL_FIELD_PVT_MON_CONTROL_SEL, i, sel_val);
    }
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    sal_usleep(10);
    /*
     * Powerdown
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_PVT_MON_ENABLE, entry_handle_id));
    for (i = 0; i < nof_pvt_mon; i++)
    {
        dbal_entry_value_field16_set(unit, entry_handle_id, DBAL_FIELD_PVT_MON_CONTROL_POWERDOWN, i, 1);
    }
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    sal_usleep(10);

    /*
     * Powerup
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_PVT_MON_ENABLE, entry_handle_id));
    for (i = 0; i < nof_pvt_mon; i++)
    {
        dbal_entry_value_field16_set(unit, entry_handle_id, DBAL_FIELD_PVT_MON_CONTROL_POWERDOWN, i, 0);
    }
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    sal_usleep(10);

    /*
     * Reset
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_PVT_MON_ENABLE, entry_handle_id));
    for (i = 0; i < nof_pvt_mon; i++)
    {
        dbal_entry_value_field16_set(unit, entry_handle_id, DBAL_FIELD_PVT_MON_CONTROL_RESET, i, 1);
    }
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    sal_usleep(10);

    /*
     * Clear Peak
     */
    if (dnx_data_pvt.general.feature_get(unit, dnx_data_pvt_general_pvt_peak_clear_support))
    {
        SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_PVT_MON_ENABLE, entry_handle_id));
        for (i = 0; i < nof_pvt_mon; i++)
        {
            dbal_entry_value_field16_set(unit, entry_handle_id, DBAL_FIELD_PVT_MON_CONTROL_PEAK_CLEAR, i, 1);
        }
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
        sal_usleep(10);

        SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_PVT_MON_ENABLE, entry_handle_id));
        for (i = 0; i < nof_pvt_mon; i++)
        {
            dbal_entry_value_field16_set(unit, entry_handle_id, DBAL_FIELD_PVT_MON_CONTROL_PEAK_CLEAR, i, 0);
        }
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
        sal_usleep(10);
    }
    DBAL_HANDLE_FREE(unit, entry_handle_id);

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
* \brief
*   API gets temperature info for PVT monitors
*   modes
* \par DIRECT INPUT:
*   \param [in] unit - unit ID
*   \param [in] voltage_max - Maximal number of acceptable temperature sensors
*   \param [out] voltage - Pointer to array to be loaded by this procedure. Each
*        element contains current voltage and peak voltage
*   \param [out] voltage_remainder - Pointer to array to be loaded by this procedure. Each
*        element contains current voltage remainder and peak voltage remainder
*   \param [out] voltage_count  - the number of sensors (and, therefore, the number of valid
*        elements on voltage).
* \par INDIRECT INPUT:
*   * None
* \par DIRECT OUTPUT:
*   int -
* \par INDIRECT OUTPUT:
*   * None
* \remark
*   *
* \see
*   * None
*/
int
dnx_pvt_mon_voltage_get(
    int unit,
    int voltage_max,
    bcm_switch_temperature_monitor_t * voltage,
    bcm_switch_temperature_monitor_t * voltage_remainder,
    int *voltage_count)
{
    int nof_pvt_mon;
    int vol_multiper, vol_factor, vol_divisor;
    uint32 entry_handle_id;
    uint16 current[DNX_DATA_MAX_PVT_GENERAL_NOF_PVT_MONITORS];
    uint16 peak[DNX_DATA_MAX_PVT_GENERAL_NOF_PVT_MONITORS];
    int i;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(voltage, _SHR_E_PARAM, "voltage");
    SHR_NULL_CHECK(voltage_remainder, _SHR_E_PARAM, "voltage_remainder");
    SHR_NULL_CHECK(voltage_count, _SHR_E_PARAM, "temperature_count");

    nof_pvt_mon = dnx_data_pvt.general.nof_pvt_monitors_get(unit);
    vol_multiper = dnx_data_pvt.general.vol_multiple_get(unit);
    vol_factor = dnx_data_pvt.general.vol_factor_get(unit);
    vol_divisor = dnx_data_pvt.general.vol_divisor_get(unit);

    if (voltage_max < nof_pvt_mon)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Array size should be equal to or larger than %d.\n", nof_pvt_mon);
    }

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_PVT_TEMPERATURE_MONITOR, &entry_handle_id));
    for (i = 0; i < nof_pvt_mon; i++)
    {
        dbal_value_field16_request(unit, entry_handle_id, DBAL_FIELD_THERMAL_DATA, i, &current[i]);
        dbal_value_field16_request(unit, entry_handle_id, DBAL_FIELD_PEAK_THERMAL_DATA, i, &peak[i]);
    }
    /** Performing the action */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

    for (i = 0; i < nof_pvt_mon; i++)
    {
        /*
         * vol [1 mv] = (vol_multiper * vol_factor * curr) / vol_divisor
         */
        voltage[i].curr = (vol_multiper * vol_factor * current[i]) / vol_divisor;
        /*
         * peak [1 mv] = (vol_multiper * vol_factor * curr) / vol_divisor
         */
        voltage[i].peak = (vol_multiper * vol_factor * peak[i]) / vol_divisor;

        voltage_remainder[i].curr = (vol_multiper * vol_factor * current[i]) % vol_divisor;
        voltage_remainder[i].curr = (voltage_remainder[i].curr * 1000) / vol_divisor;

        voltage_remainder[i].peak = (vol_multiper * vol_factor * peak[i]) % vol_divisor;
        voltage_remainder[i].peak = (voltage_remainder[i].peak * 1000) / vol_divisor;

    }

    *voltage_count = nof_pvt_mon;

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * }
 */
