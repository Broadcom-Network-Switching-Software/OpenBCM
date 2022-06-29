/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 *
 * DNXF STAT
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif

#define BSL_LOG_MODULE BSL_LS_BCMDNX_STAT
#include <shared/bsl.h>
#include <shared/error.h>
#include <bcm/types.h>
#include <bcm/error.h>
#include <bcm_int/common/debug.h>
#include <bcm/debug.h>
#include <bcm/stat.h>

#include <soc/defs.h>
#include <soc/dnxf/cmn/dnxf_drv.h>
#include <soc/dnxf/cmn/dnxf_defs.h>
#include <soc/dnxf/cmn/dnxf_stat.h>
#include <soc/dnxf/cmn/dnxf_fabric.h>
#include <soc/dnxf/cmn/mbcm.h>
#include <soc/dnxf/dnxf_data/auto_generated/dnxf_data_port.h>
#include <soc/dnxc/drv_dnxc_utils.h>
#include <soc/dnxf/swstate/auto_generated/access/dnxf_access.h>

int
bcm_dnxf_stat_init(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "API not supported!");

exit:
    SHR_FUNC_EXIT;
}

/*
 * Function:
 *      dnxf_stat_init
 * Purpose:
 *      Initialize the BCM statistics module
 * Parameters:
 *      unit  - (IN)     Unit number.
 * Returns:
 *      _SHR_E_NONE        Success
 *      _SHR_E_INTERNAL    Device access failure
 */
int
dnxf_stat_init(
    int unit)
{
    int interval;
    uint32 flags;
    bcm_pbmp_t counter_pbmp;
    soc_control_t *soc = SOC_CONTROL(unit);

    SHR_FUNC_INIT_VARS(unit);

    interval = dnxf_data_port.stat.thread_interval_get(unit);
    if ((SAL_BOOT_PLISIM) && (interval > 0 && interval < SOC_COUNTER_INTERVAL_SIM_DEFAULT))
    {
        interval = SOC_COUNTER_INTERVAL_SIM_DEFAULT;
    }

    /*
     * Update interval/pbmp from or to SW DB
     */
    if (SOC_WARM_BOOT(unit))
    {
        SHR_IF_ERR_EXIT(dnxf_state.mib.interval.get(unit, &interval));
        SHR_IF_ERR_EXIT(dnxf_state.mib.counter_pbmp.get(unit, &counter_pbmp));
    }
    else
    {
        SHR_IF_ERR_EXIT(dnxf_state.mib.interval.set(unit, interval));
        BCM_PBMP_ASSIGN(counter_pbmp, PBMP_SFI_ALL(unit));
        SHR_IF_ERR_EXIT(dnxf_state.mib.counter_pbmp.set(unit, counter_pbmp));
    }

    flags = dnxf_data_port.stat.thread_flags_get(unit);
    SHR_IF_ERR_EXIT(soc_counter_start(unit, flags, interval, counter_pbmp));

    /*
     * update counters bitmap in case counter thread is not start
     */
    if (interval == 0)
    {
        SOC_PBMP_ASSIGN(soc->counter_pbmp, counter_pbmp);
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * Function:
 *      _bcm_dnxf_stat_deinit
 * Purpose:
 *      Internal function  - Deinitialize the BCM statistics module
 * Parameters:
 *      unit  - (IN)     Unit number.
 * Returns:
 *      _SHR_E_NONE        Success
 *      _SHR_E_INTERNAL    Device access failure
 */
int
dnxf_stat_deinit(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(soc_counter_stop(unit));

exit:
    SHR_FUNC_EXIT;
}

/*
 * Function:
 *      bcm_dnxf_stat_clear
 * Purpose:
 *      Clear the port-based statistics for the indicated device port
 * Parameters:
 *      unit  - (IN)     Unit number.
 *      port  - (IN)     Zero-based device or logical port number
 * Returns:
 *      _SHR_E_NONE        Success
 *      _SHR_E_INTERNAL    Device access failure
 */
int
bcm_dnxf_stat_clear(
    int unit,
    bcm_port_t port)
{
    SHR_FUNC_INIT_VARS(unit);

    DNXF_LINK_INPUT_CHECK(unit, port);

    SHR_IF_ERR_EXIT(soc_controlled_counter_clear(unit, port));

exit:
    SHR_FUNC_EXIT;
}

/*
 * Function:
 *      bcm_dnxf_stat_sync
 * Purpose:
 *      Synchronize software counters with hardware
 * Parameters:
 *      unit  - (IN)     Unit number.
 * Returns:
 *      _SHR_E_NONE        Success
 *      _SHR_E_INTERNAL    Device access failure
 *      _SHR_E_DISABLED    Unit's counter disabled
 */
int
bcm_dnxf_stat_sync(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(soc_counter_sync(unit));

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnxf_stat_get_verify(
    int unit,
    bcm_port_t port,
    bcm_stat_val_t type,
    uint64 *value)
{
    int counters[SOC_DNXF_MAX_NOF_COUNTERS_PER_STAT_TYPE] = { 0 };
    int array_size = 0;

    SHR_FUNC_INIT_VARS(unit);

    DNXF_LINK_INPUT_CHECK(unit, port);
    SHR_NULL_CHECK(value, _SHR_E_PARAM, "value");

    if (_SHR_E_NONE != MBCM_DNXF_DRIVER_CALL(unit, mbcm_dnxf_stat_is_supported_type, (unit, port, type)))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "type %d isn't supported for port %d", type, port);
    }

    if (snmpBcmSignalQualityLastErrorRate == type)
    {
        SHR_IF_ERR_EXIT(MBCM_DNXF_DRIVER_CALL(unit, mbcm_dnxf_mapping_stat_get,
                                              (unit, port, counters, &array_size, type,
                                               SOC_DNXF_MAX_NOF_COUNTERS_PER_STAT_TYPE)));

        /**
         * Based on priority set by soc_feature_ignore_controlled_counter_priority, it will add
         * only the relevant counters according to their predefined level
         * and those will be controlled counters.
         * Since snmpBcmSignalQualityLastErrorRate is not collected via controlled_counter_get,
         * it must be verified that it is marked as controlled.
         */
        if (0 < array_size && !COUNTER_IS_COLLECTED(SOC_CONTROL(unit)->controlled_counters[counters[0]]))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Wrong usage, type snmpBcmSignalQualityLastErrorRate must be controlled");
        }
    }

exit:
    SHR_FUNC_EXIT;
}
/*
 * Function:
 *      bcm_dnxf_stat_get
 * Purpose:
 *      Get the specified statistics from the device
 * Parameters:
 *      unit  - (IN)     Unit number.
 *      port  - (IN)     Zero-based device or logical port number
 *      type  - (IN)     SNMP statistics type defined in bcm_stat_val_t
 *      value - (OUT)    Counter value
 * Returns:
 *      _SHR_E_NONE       Success.
 *      _SHR_E_PARAM      Illegal parameter.
 *      _SHR_E_BADID      Illegal port number.
 *      _SHR_E_INTERNAL   Device access failure.
 *      _SHR_E_UNAVAIL    Counter/variable is not implemented on this current chip.
 */
int
bcm_dnxf_stat_get(
    int unit,
    bcm_port_t port,
    bcm_stat_val_t type,
    uint64 *value)
{
    int counters[SOC_DNXF_MAX_NOF_COUNTERS_PER_STAT_TYPE] = { 0 };
    int array_size = 0;

    SHR_FUNC_INIT_VARS(unit);

    /** invoke_verify */
    SHR_INVOKE_VERIFY_DNXC(dnxf_stat_get_verify(unit, port, type, value));

    SHR_IF_ERR_EXIT(MBCM_DNXF_DRIVER_CALL(unit, mbcm_dnxf_mapping_stat_get,
                                          (unit, port, counters, &array_size, type,
                                           SOC_DNXF_MAX_NOF_COUNTERS_PER_STAT_TYPE)));

    SHR_IF_ERR_EXIT(soc_dnxf_stat_get(unit, port, value, counters, array_size));

exit:
    SHR_FUNC_EXIT;
}

static int
bcm_dnxf_stat_rate_get_verify(
    int unit,
    bcm_port_t port,
    bcm_stat_val_t type,
    uint64 *rate)
{
    VOL int *counter_interval_p;

    SHR_FUNC_INIT_VARS(unit);

    DNXF_LINK_INPUT_CHECK(unit, port);

    SHR_NULL_CHECK(rate, _SHR_E_PARAM, "rate");

    /** verify counter is controlled */
    if (!dnx_drv_soc_controlled_counter_use(unit, port))
    {
        SHR_ERR_EXIT(_SHR_E_UNAVAIL, "given port:%d is not using controlled counter", port);
    }

    /** verify counter thread is enabled */
    SHR_IF_ERR_EXIT(soc_control_element_address_get(unit, COUNTER_INTERVAL, (void **) (&counter_interval_p)));
    if (*counter_interval_p == 0)
    {
        SHR_ERR_EXIT(_SHR_E_UNAVAIL, "rate API is nut supported when counter thread is disabled\n");
    }

    /** verify type is supported for port */
    if (_SHR_E_NONE != MBCM_DNXF_DRIVER_CALL(unit, mbcm_dnxf_stat_is_supported_type, (unit, port, type)))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "type %d isn't supported for port %d", type, port);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get the rate of a specified statistics from the device
 *      Rate is normalized to a 1 second period.
 * \param [in] unit - Unit number
 * \param [in] port - Zero-based device or logical port number
 * \param [in] type - SNMP statistics type defined in bcm_stat_val_t
 * \param [out] rate - Counter rate. Counter delta per second.
 * \return _SHR_E_xxx
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnxf_stat_rate_get(
    int unit,
    bcm_port_t port,
    bcm_stat_val_t type,
    uint64 *rate)
{
    int counters[SOC_DNXF_MAX_NOF_COUNTERS_PER_STAT_TYPE] = { 0 };
    int array_size = 0;

    SHR_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNXC(bcm_dnxf_stat_rate_get_verify(unit, port, type, rate));

    SHR_IF_ERR_EXIT(MBCM_DNXF_DRIVER_CALL(unit, mbcm_dnxf_mapping_stat_get,
                                          (unit, port, counters, &array_size, type,
                                           SOC_DNXF_MAX_NOF_COUNTERS_PER_STAT_TYPE)));

    SHR_IF_ERR_EXIT(soc_dnxf_stat_rate_get(unit, port, rate, counters, array_size));

exit:
    SHR_FUNC_EXIT;
}

/*
 * Function:
 *      bcm_dnxf_stat_multi_get
 * Purpose:
 *      Get the specified statistics from the device
 * Parameters:
 *      unit      - (IN)     Unit number.
 *      port      - (IN)     Zero-based device or logical port number
 *      nstat     - (IN)     Number of elements in stat array
 *      stat_arr  - (IN)     Array of SNMP statistics types defined in bcm_stat_val_t
 *      value_arr - (OUT)    Collected 64-bit or 32-bit statistics values
 * Returns:
 *  _SHR_E_XXX
 */
int
bcm_dnxf_stat_multi_get(
    int unit,
    bcm_port_t port,
    int nstat,
    bcm_stat_val_t * stat_arr,
    uint64 *value_arr)
{
    int i;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(stat_arr, _SHR_E_PARAM, "stat_arr");
    SHR_NULL_CHECK(value_arr, _SHR_E_PARAM, "value_arr");

    for (i = 0; i < nstat; i++)
    {
        SHR_IF_ERR_EXIT(bcm_dnxf_stat_get(unit, port, stat_arr[i], &(value_arr[i])));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get the rate of a specified statistics from the device
 *      Rate is normalized to a 1 second period.
 * \param [in] unit - Unit ID
 * \param [in] port - Zero-based device or logical port number
 * \param [in] nstat - Number of elements in stat array
 * \param [in] stat_arr - Array of SNMP statistics types defined in bcm_stat_val_t
 * \param [out] rate_arr - Collected 64-bit rate of a statistics. Rate is normalized to a 1 second period.
 * \return _SHR_E_XXX
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnxf_stat_rate_multi_get(
    int unit,
    bcm_port_t port,
    int nstat,
    bcm_stat_val_t * stat_arr,
    uint64 *rate_arr)
{
    int i;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(stat_arr, _SHR_E_PARAM, "stat_arr");
    SHR_NULL_CHECK(rate_arr, _SHR_E_PARAM, "rate_arr");

    for (i = 0; i < nstat; i++)
    {
        SHR_IF_ERR_EXIT(bcm_dnxf_stat_rate_get(unit, port, stat_arr[i], &(rate_arr[i])));
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * Function:
 *      bcm_dnxf_stat_get32
 * Purpose:
 *      Get the specified statistics from the device
 * Parameters:
 *      unit  - (IN)     Unit number.
 *      port  - (IN)     Zero-based device or logical port number
 *      type  - (IN)     SNMP statistics type defined in bcm_stat_val_t
 *      value - (OUT)    Counter value
 * Returns:
 *      _SHR_E_NONE       Success.
 *      _SHR_E_PARAM      Illegal parameter.
 *      _SHR_E_BADID      Illegal port number.
 *      _SHR_E_INTERNAL   Device access failure.
 *      _SHR_E_UNAVAIL    Counter/variable is not implemented on this current chip.
 *      _SHR_E_FAIL       Counter overflowed
 */
int
bcm_dnxf_stat_get32(
    int unit,
    bcm_port_t port,
    bcm_stat_val_t type,
    uint32 *value)
{
    uint64 value64;
    uint32 value64_hi;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(bcm_dnxf_stat_get(unit, port, type, &value64));

    COMPILER_64_TO_32_HI(value64_hi, value64);
    if (value64_hi > 0)
    {
        SHR_ERR_EXIT(_SHR_E_FAIL, "statictics counter type %d overflow", type);
    }

    COMPILER_64_TO_32_LO(*value, value64);

exit:
    SHR_FUNC_EXIT;
}

int
bcm_dnxf_stat_multi_get32(
    int unit,
    bcm_port_t port,
    int nstat,
    bcm_stat_val_t * stat_arr,
    uint32 *value_arr)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "bcm_dnxf_stat_multi_get32 is not available");
exit:
    SHR_FUNC_EXIT;
}

#undef BSL_LOG_MODULE
