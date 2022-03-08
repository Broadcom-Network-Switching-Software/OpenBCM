/*
 ** \file port.c $Id$ PORT procedures for DNX.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_PORT
/*
 * Include files which are specifically for DNX. Final location.
 * {
 */

#include <math.h>
#include <soc/sand/shrextend/shrextend_debug.h>
#include <soc/memory.h>
#include <soc/feature.h>
#include <soc/types.h>
#include <soc/register.h>
#include <soc/counter.h>
#include <soc/dnx/dnx_err_recovery_manager.h>
#include <soc/dnx/recovery/generic_state_journal.h>
#include <soc/dnxc/drv_dnxc_utils.h>

/*
 * }
 */
/*
 * Include files currently used for DNX. To be modified and moved to
 * final location.
 * {
 */
#include <soc/dnx/dbal/dbal.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_port.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_nif.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/access/dnx_algo_port_access.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/types/dnx_algo_port_types.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_headers.h>
#include <bcm_int/dnx/auto_generated/dnx_stat_dispatch.h>
#include <bcm_int/dnx/auto_generated/dnx_port_dispatch.h>
/*
 * }
 */
/*
 * Include files.
 * {
 */
#include <shared/bslenum.h>
#include <shared/utilex/utilex_bitstream.h>
#include <bcm/error.h>
#include <bcm/types.h>
#include <bcm/port.h>
#include <bcm_int/dnx/algo/algo_gpm.h>
#include <bcm_int/dnx/algo/port/algo_port_mgmt.h>
#include <bcm_int/dnx/init/init.h>
#include <bcm_int/dnx/port/port_signal_quality.h>
#include <bcm_int/dnx/algo/port/algo_port_utils.h>
#include <bcm_int/dnx/stat/mib/mib_stat.h>

#include <sal/core/dpc.h>

#include "port_utils.h"

/*
 * }
 */

/*
 * Macros.
 * {
 */

/*
 * }
 */

/*
 * Defines.
 * {
 */
#define DNX_PORT_SIGNAL_QUALITY_ER_BITS_PER_SYMBOL 10
#define DNX_PORT_SIGNAL_QUALITY_ER_MANTISSA_BITS_NOF 16
#define DNX_PORT_SIGNAL_QUALITY_ER_MANTISSA_OFFSET 16
#define DNX_PORT_SIGNAL_QUALITY_ER_EXPONENT_BITS_NOF 16
#define DNX_PORT_SIGNAL_QUALITY_ER_EXPONENT_OFFSET 0

/*
 * }
 */

/*
 * Typedefs
 * {
 */

/*
 * }
 */

static shr_error_e
dnx_port_signal_quality_er_port_db_update(
    int unit,
    int first_phy,
    uint64 ber_cnt,
    uint64 prev_uncorrectable_cnt)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_db.signal_quality.phy_state.interval_cnt.set(unit, first_phy, 0));
    SHR_IF_ERR_EXIT(dnx_algo_port_db.signal_quality.phy_state.prev_cnt.set(unit, first_phy, ber_cnt));
    SHR_IF_ERR_EXIT(dnx_algo_port_db.signal_quality.phy_state.prev_uncorrectable_cnt.set(unit,
                                                                                         first_phy,
                                                                                         prev_uncorrectable_cnt));

exit:
    SHR_FUNC_EXIT;
}

static void
dnx_port_signal_quality_er_man_and_exp_to_float_get(
    uint16 man,
    int16 exp,
    float *val)
{
    *val = pow(10, exp);
    *val *= man;
}

static void
dnx_port_signal_quality_er_float_to_man_and_exp_get(
    float val,
    uint16 *man,
    int16 *exp)
{
    val *= pow(10, 12);
    *exp = 0;

    while (val >= (1 << DNX_PORT_SIGNAL_QUALITY_ER_MANTISSA_BITS_NOF))
    {
        val /= 10;
        *exp += 1;
    }

    *exp -= 12;
    *man = (int16) val;
}

static shr_error_e
dnx_port_signal_quality_er_encoded_val_to_man_and_exp_get(
    int unit,
    uint32 encoded_val,
    uint16 *man,
    int16 *exp)
{
    uint32 man_extracted, exp_extracted;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(utilex_bitstream_get_field
                    (&encoded_val, DNX_PORT_SIGNAL_QUALITY_ER_EXPONENT_OFFSET,
                     DNX_PORT_SIGNAL_QUALITY_ER_EXPONENT_BITS_NOF, &exp_extracted));
    SHR_IF_ERR_EXIT(utilex_bitstream_get_field
                    (&encoded_val, DNX_PORT_SIGNAL_QUALITY_ER_MANTISSA_OFFSET,
                     DNX_PORT_SIGNAL_QUALITY_ER_MANTISSA_BITS_NOF, &man_extracted));

    *exp = (int16) exp_extracted;
    *man = (uint16) man_extracted;

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_port_signal_quality_er_encoded_val_to_float_get(
    int unit,
    uint32 encoded_val,
    float *val)
{
    uint16 man = 0;
    int16 exp = 0;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * encode mantissa and exponent so that:
     *
     * ber_encoded = 16-bit man | 16-bit exp
     *                  MSB           LSB
     */

    SHR_IF_ERR_EXIT(dnx_port_signal_quality_er_encoded_val_to_man_and_exp_get(unit, encoded_val, &man, &exp));

    dnx_port_signal_quality_er_man_and_exp_to_float_get(man, exp, val);

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_port_signal_quality_er_float_to_encoded_val_get(
    int unit,
    float val,
    uint32 *encoded_val)
{
    uint16 man;
    int16 exp;

    SHR_FUNC_INIT_VARS(unit);

    dnx_port_signal_quality_er_float_to_man_and_exp_get(val, &man, &exp);

    /*
     * encode mantissa and exponent so that:
     *
     * ber_encoded = 16-bit man | 16-bit exp
     *                  MSB           LSB
     */
    SHR_IF_ERR_EXIT(utilex_bitstream_set_field
                    (encoded_val, DNX_PORT_SIGNAL_QUALITY_ER_EXPONENT_OFFSET,
                     DNX_PORT_SIGNAL_QUALITY_ER_EXPONENT_BITS_NOF, exp));
    SHR_IF_ERR_EXIT(utilex_bitstream_set_field
                    (encoded_val, DNX_PORT_SIGNAL_QUALITY_ER_MANTISSA_OFFSET,
                     DNX_PORT_SIGNAL_QUALITY_ER_MANTISSA_BITS_NOF, man));

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_port_signal_quality_er_last_error_rate_set(
    int unit,
    int first_phy,
    float error_rate)
{
    uint32 error_rate_encoded[1] = { 0 };

    SHR_FUNC_INIT_VARS(unit);

    /** encode error rate into mantissa and exponent */
    dnx_port_signal_quality_er_float_to_encoded_val_get(unit, error_rate, error_rate_encoded);

    /** store encoded value */
    SHR_IF_ERR_EXIT(dnx_algo_port_db.signal_quality.phy_state.
                    prev_error_rate.set(unit, first_phy, error_rate_encoded[0]));

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_port_signal_quality_er_hold_off_interval_cnt_set(
    int unit,
    int first_phy,
    uint8 is_clear,
    uint32 interval)
{
    uint32 hold_off_interval;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_db.signal_quality.phy_state.hold_off_interval_cnt.get(unit, first_phy,
                                                                                        &hold_off_interval));

    if (is_clear)
    {
        hold_off_interval = 0;
    }
    else
    {
        hold_off_interval += interval;
    }

    SHR_IF_ERR_EXIT(dnx_algo_port_db.signal_quality.phy_state.hold_off_interval_cnt.set(unit, first_phy,
                                                                                        hold_off_interval));

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_port_signal_quality_er_is_hold_off_get(
    int unit,
    int first_phy,
    bcm_port_signal_quality_error_rate_state_t prev_state,
    bcm_port_signal_quality_error_rate_state_t new_state,
    uint32 sample_interval,
    uint8 *is_hold_off)
{
    uint32 hold_off_interval, cfg_hold_off_interval;

    SHR_FUNC_INIT_VARS(unit);

    /** get the configured hold off interval */
    SHR_IF_ERR_EXIT(dnx_algo_port_db.signal_quality.phy_config.
                    hold_off_interval.get(unit, first_phy, &cfg_hold_off_interval));
    /** get the current hold off interval */
    SHR_IF_ERR_EXIT(dnx_algo_port_db.signal_quality.phy_state.
                    hold_off_interval_cnt.get(unit, first_phy, &hold_off_interval));

    /** No need to mask the SD/SF state update to recover in case that holds off interval exceeded the configured one */
    if (hold_off_interval + sample_interval >= cfg_hold_off_interval)
    {
        *is_hold_off = FALSE;
    }
    else
    {
        uint8 is_prev_state_fail_deg = (BCM_PORT_SIGNAL_QUALITY_ERROR_RATE_STATE_DEGRADE == prev_state
                                        || BCM_PORT_SIGNAL_QUALITY_ERROR_RATE_STATE_FAIL == prev_state);
        uint8 is_next_state_recover = (BCM_PORT_SIGNAL_QUALITY_ERROR_RATE_STATE_FAIL_RECOVER == new_state
                                       || BCM_PORT_SIGNAL_QUALITY_ERROR_RATE_STATE_DEGRADE_RECOVER == new_state);

        /** The Signal state will be held off since the interval counter didn't exceed and the new pending state is recovering  */
        if (is_prev_state_fail_deg && is_next_state_recover)
        {
            *is_hold_off = TRUE;
        }
        else
        {
            *is_hold_off = FALSE;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

void
dnx_port_signal_quality_er_threshold_handle(
    void *_unit,
    void *_port,
    void *_new_state,
    void *_er_rate,
    void *ignored_b)
{
    float error_rate;
    uint32 remote_fault_enabled;
    int first_phy;

    /** following variables are carried on heap memory */
    int unit = PTR_TO_INT(_unit);
    bcm_port_t port = PTR_TO_INT(_port);
    uint32 new_state = PTR_TO_INT(_new_state);
    uint32 error_rate_encoded = PTR_TO_INT(_er_rate);

    SHR_FUNC_INIT_VARS(unit);

    /** convert ER to mantissa representation*/
    SHR_IF_ERR_EXIT(dnx_port_signal_quality_er_encoded_val_to_float_get(unit, error_rate_encoded, &error_rate));

    /*
     * call user callback using soc_event_generate
     */
    SHR_IF_ERR_EXIT(dnx_drv_soc_event_generate
                    (unit, BCM_SWITCH_EVENT_SIGNAL_QUALITY_CHANGE, port, new_state, error_rate_encoded));

    /*
     * handle remote fault
     */

    SHR_IF_ERR_EXIT(dnx_algo_port_nif_first_phy_get(unit, port, 0, &first_phy));

    /** check if user configured remote fault for that port */
    SHR_IF_ERR_EXIT(dnx_algo_port_db.signal_quality.phy_config.
                    action_enabled.get(unit, first_phy, &remote_fault_enabled));

    if (remote_fault_enabled)
    {
        /*
         * Check whether we need to write to HW, or whether this stage can be skipped.
         */
        int link_fault_force = (new_state == BCM_PORT_SIGNAL_QUALITY_ERROR_RATE_STATE_FAIL ? 1 : 0);
        int current_link_fault_force = FALSE;
        bcm_port_signal_quality_error_rate_state_t prev_state;

        /** read previous state */
        SHR_IF_ERR_EXIT(dnx_algo_port_db.signal_quality.phy_state.prev_state_handler.get(unit, first_phy, &prev_state));

        /**
         * if previous state is 'signal fail' and the user did configure the port to use 'remote_fault' indication
         * then the remote fault indication must be up
         */
        current_link_fault_force = (prev_state == BCM_PORT_SIGNAL_QUALITY_ERROR_RATE_STATE_FAIL ? 1 : 0);

        if (current_link_fault_force != link_fault_force)
        {
            /** perform remote fault if user configured it */
            SHR_IF_ERR_EXIT(bcm_dnx_port_control_set(unit, port, bcmPortControlLinkFaultRemoteForce, link_fault_force));

        }
    }

    /*
     * update state
     *
     * NOTE: 'ber' and 'interval' stateful counters are updated in counter thread
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_db.signal_quality.phy_state.prev_state_handler.set(unit, first_phy, new_state));

exit:
    SHR_VOID_FUNC_EXIT;
}

static shr_error_e
dnx_port_signal_quality_er_event_raise(
    int unit,
    bcm_port_t port,
    bcm_port_signal_quality_error_rate_state_t new_state,
    float error_rate)
{
    uint32 error_rate_encoded[1] = { 0 };

    SHR_FUNC_INIT_VARS(unit);

    dnx_port_signal_quality_er_float_to_encoded_val_get(unit, error_rate, error_rate_encoded);

    /*
     * raise an event only on state change
     */
    if (SHR_FAILURE(sal_dpc(dnx_port_signal_quality_er_threshold_handle,
                            INT_TO_PTR(unit), INT_TO_PTR(port), INT_TO_PTR(new_state),
                            INT_TO_PTR(error_rate_encoded[0]), 0)))
    {
        SHR_ERR_EXIT(_SHR_E_FULL, "Failed to queue a routine to DPC\n");
    }

exit:
    SHR_FUNC_EXIT;
}

/** see header file */
static shr_error_e
dnx_port_signal_quality_er_port_rate_get(
    int unit,
    bcm_port_t port,
    uint64 *port_rate)
{
    uint32 serdes_rate;
    int nof_phys;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_serdes_rate_get(unit, port, &serdes_rate));

    SHR_IF_ERR_EXIT(dnx_algo_port_nif_phys_nof_get(unit, port, &nof_phys));

    *port_rate = ((uint64) serdes_rate) * nof_phys;

exit:
    SHR_FUNC_EXIT;
}

static _shr_error_t
dnx_port_signal_quality_er_fec_bit_error_compensate(
    int unit,
    bcm_port_t port,
    uint64 count,
    uint64 *compensated_count)
{
    bcm_port_resource_t resource;
    const dnx_data_nif_signal_quality_supported_fecs_t *fec_entry = NULL;

    SHR_FUNC_INIT_VARS(unit);

    *compensated_count = 0;

    /** get port fec type */
    SHR_IF_ERR_EXIT(bcm_dnx_port_resource_get(unit, port, &resource));

    /** get compensation data based on FEC type */
    fec_entry = dnx_data_nif.signal_quality.supported_fecs_get(unit, resource.fec_type);

    /** if the FEC requires compensation, do the math */
    if (fec_entry->total_bits != 0)
    {
        *compensated_count = count * fec_entry->total_bits / fec_entry->counted_bits;
    }

exit:
    SHR_FUNC_EXIT;
}

static bcm_error_t
dnx_port_signal_quality_er_error_rate_get(
    int unit,
    bcm_port_t port,
    int interval,
    uint8 is_ber,
    uint64 er_cnt,
    uint64 uncorrectable_er_cnt,
    float *error_rate)
{
    int first_phy;
    uint64 good_cnt = 0;
    uint64 port_rate = 0;

    SHR_FUNC_INIT_VARS(unit);

    *error_rate = 0;

    SHR_IF_ERR_EXIT(dnx_algo_port_nif_first_phy_get(unit, port, 0, &first_phy));

    /** calculate error rate value */
    SHR_IF_ERR_EXIT(dnx_port_signal_quality_er_port_rate_get(unit, port, &port_rate));

    if (is_ber)
    {
        /** BER */

        /*
         * Each uncorrectable error is considered as 16 symbol errors.
         * This is only an approximation since once an uncorrectable error occurs FEC has
         * no way of knowing the accurate amount of wrong bits.
         *
         * Skip heavy multiplications in case value is 0
         */
        if (uncorrectable_er_cnt)
        {
            bcm_port_resource_t resource;
            int min_nof_bad_symbols;

            SHR_IF_ERR_EXIT(bcm_dnx_port_resource_get(unit, port, &resource));

            /** minimum bad symbols in codeword */
            min_nof_bad_symbols =
                dnx_data_nif.signal_quality.supported_fecs_get(unit,
                                                               resource.fec_type)->max_nof_correctable_symbols + 1;

            er_cnt += uncorrectable_er_cnt * min_nof_bad_symbols * DNX_PORT_SIGNAL_QUALITY_ER_BITS_PER_SYMBOL;
        }

        good_cnt = port_rate /** Mbps */  * interval /** Micro-sec */ ;

        /** In some FEC types, bit errors are not counted over parity region */
        SHR_IF_ERR_EXIT(dnx_port_signal_quality_er_fec_bit_error_compensate(unit, port, er_cnt, &er_cnt));
    }
    else
    {
        /** Symbol */
        good_cnt = port_rate /** Mbps */  * interval /** Micro-sec */  / DNX_PORT_SIGNAL_QUALITY_ER_BITS_PER_SYMBOL;
    }

    /** er_cnt will not be 0 at this stage */
    if (good_cnt)
    {
        *error_rate = ((float) er_cnt) / good_cnt;
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_port_signal_quality_er_stat_get(
    int unit,
    bcm_port_t port,
    uint8 is_ber,
    uint64 *curr_er_cnt,
    uint64 *curr_uncorrectable_er_cnt)
{
    SHR_FUNC_INIT_VARS(unit);

    *curr_uncorrectable_er_cnt = 0;

    if (is_ber)
    {
        /** read current BER value */
        SHR_IF_ERR_EXIT(soc_counter_get(unit, port, dnx_mib_counter_rx_rs_fec_bit_error, 0, curr_er_cnt));

        /** check if uncorrectable FEC */
        SHR_IF_ERR_EXIT(soc_counter_get
                        (unit, port, dnx_mib_counter_nif_rx_fec_uncorrectable_errors, 0, curr_uncorrectable_er_cnt));
    }
    else
    {
        /** read current SER value */
        SHR_IF_ERR_EXIT(soc_counter_get(unit, port, dnx_mib_counter_rx_rs_fec_symbol_error_rate, 0, curr_er_cnt));

        /*
         * For SER mode, HW increments the SER by 16 for each uncorrectable error, so
         * there is no need to take uncorrectable errors into account during error rate
         * calculations.
         */
        *curr_uncorrectable_er_cnt = 0;
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief   This function is run in counter thread and examines the
 *          counters for each port.
 */
static void
dnx_port_signal_quality_er_check(
    int unit)
{
    bcm_port_t port;
    bcm_pbmp_t enabled_ports;
    int iteration_interval;

    SHR_FUNC_INIT_VARS(unit);

    /** Suppress error recovery. */
    DNX_ERR_RECOVERY_SUPPRESS(unit);

    /** get bitmap of ports that use Signal quality ER feature */
    SHR_IF_ERR_EXIT(dnx_algo_port_db.signal_quality.enabled_ports.pbmp_get(unit, &enabled_ports));

    /** get actual time passed since last sample */
    SHR_IF_ERR_EXIT(soc_counter_actual_interval_get(unit, &iteration_interval));

    _SHR_PBMP_ITER(enabled_ports, port)
    {
        int first_phy;
        uint32 interval_cnt, cfg_interval, sample_interval;

        SHR_IF_ERR_EXIT(dnx_algo_port_nif_first_phy_get(unit, port, 0, &first_phy));

        SHR_IF_ERR_EXIT(dnx_algo_port_db.signal_quality.phy_state.interval_cnt.get(unit, first_phy, &interval_cnt));
        SHR_IF_ERR_EXIT(dnx_algo_port_db.signal_quality.phy_config.interval.get(unit, first_phy, &cfg_interval));

        sample_interval = interval_cnt + iteration_interval;


        /** update the interval_cnt */
        SHR_IF_ERR_EXIT(dnx_algo_port_db.signal_quality.phy_state.interval_cnt.set(unit, first_phy, sample_interval));
        /*
         * check if time to sample
         */
        if (sample_interval >= cfg_interval)
        {
            dnx_algo_port_signal_quality_phy_state_t phy_state;
            dnx_algo_port_signal_quality_phy_config_t phy_config;
            uint64 er_cnt = 0, prev_er_cnt, curr_er_cnt;
            uint64 uncorrectable_er_cnt = 0, prev_uncorrectable_cnt, curr_uncorrectable_er_cnt = 0;
            bcm_port_signal_quality_error_rate_state_t prev_state, state;
            float fail_thr, fail_cu_thr, degrade_thr, degrade_cu_thr;
            float er_rate = 0;
            uint8 is_ber;

            /** The default action will be to increment the hold off interval counter */
            uint8 is_hold_off_interval_cnt_clear = FALSE;

            SHR_IF_ERR_EXIT(dnx_algo_port_db.signal_quality.phy_state.get(unit, first_phy, &phy_state));
            SHR_IF_ERR_EXIT(dnx_algo_port_db.signal_quality.phy_config.get(unit, first_phy, &phy_config));

            /** if port is a member of measure_mode bitmap, means that it measures using BER */
            SHR_IF_ERR_EXIT(dnx_algo_port_db.signal_quality.measure_mode.pbmp_member(unit, port, &is_ber));

            prev_er_cnt = phy_state.prev_cnt;
            prev_uncorrectable_cnt = phy_state.prev_uncorrectable_cnt;
            prev_state = phy_state.prev_state_sampler;

            /*
             * get the relevant stat value: BER/SER and uncorrectable frame count
             */
            SHR_IF_ERR_EXIT(dnx_port_signal_quality_er_stat_get
                            (unit, port, is_ber, &curr_er_cnt, &curr_uncorrectable_er_cnt));

            /*
             * skip if wrap around occurred
             */
            if (curr_uncorrectable_er_cnt < prev_uncorrectable_cnt || curr_er_cnt < prev_er_cnt)
            {
                /*
                 * restart count in case a wrap-around occurs or a user has modified the value of the counter using APIs
                 */
                SHR_IF_ERR_EXIT(dnx_port_signal_quality_er_port_db_update
                                (unit, first_phy, curr_er_cnt, curr_uncorrectable_er_cnt));

                continue;
            }

            uncorrectable_er_cnt = curr_uncorrectable_er_cnt - prev_uncorrectable_cnt;
            er_cnt = curr_er_cnt - prev_er_cnt;

            /** calculate error rate */
            SHR_IF_ERR_EXIT(dnx_port_signal_quality_er_error_rate_get
                            (unit, port, sample_interval, is_ber, er_cnt, uncorrectable_er_cnt, &er_rate));

            /** store latest calculated error rate in sw-state */
            SHR_IF_ERR_EXIT(dnx_port_signal_quality_er_last_error_rate_set(unit, first_phy, er_rate));

            state = prev_state;

            /** convert thresholds to float representation */
            SHR_IF_ERR_EXIT(dnx_port_signal_quality_er_encoded_val_to_float_get(unit, phy_config.fail_thr, &fail_thr));
            SHR_IF_ERR_EXIT(dnx_port_signal_quality_er_encoded_val_to_float_get
                            (unit, phy_config.fail_cu_thr, &fail_cu_thr));
            SHR_IF_ERR_EXIT(dnx_port_signal_quality_er_encoded_val_to_float_get
                            (unit, phy_config.degrade_thr, &degrade_thr));
            SHR_IF_ERR_EXIT(dnx_port_signal_quality_er_encoded_val_to_float_get
                            (unit, phy_config.degrade_cu_thr, &degrade_cu_thr));

            /*
             * in case both thresholds are crossed during a single sample, need to generate 2 events.
             * The order of the event is dependent on the current error rate
             */
            if (er_rate > 0 && er_rate > fail_thr
                && prev_state == BCM_PORT_SIGNAL_QUALITY_ERROR_RATE_STATE_DEGRADE_RECOVER)
            {
                /** Two event should be raised */
                SHR_IF_ERR_EXIT(dnx_port_signal_quality_er_event_raise
                                (unit, port, BCM_PORT_SIGNAL_QUALITY_ERROR_RATE_STATE_DEGRADE, er_rate));

                SHR_IF_ERR_EXIT(dnx_port_signal_quality_er_event_raise
                                (unit, port, BCM_PORT_SIGNAL_QUALITY_ERROR_RATE_STATE_FAIL, er_rate));

                SHR_IF_ERR_EXIT(dnx_algo_port_db.signal_quality.phy_state.prev_state_sampler.set(unit, first_phy,
                                                                                                 BCM_PORT_SIGNAL_QUALITY_ERROR_RATE_STATE_FAIL));

                /** Reset hold off interval counter upon state update to signal fail */
                is_hold_off_interval_cnt_clear = TRUE;
            }
            else if (er_rate < degrade_cu_thr && prev_state == BCM_PORT_SIGNAL_QUALITY_ERROR_RATE_STATE_FAIL)
            {
                uint8 is_hold_off = FALSE;

                /** Check if event should be held off */
                SHR_IF_ERR_EXIT(dnx_port_signal_quality_er_is_hold_off_get
                                (unit, first_phy, prev_state, BCM_PORT_SIGNAL_QUALITY_ERROR_RATE_STATE_DEGRADE_RECOVER,
                                 sample_interval, &is_hold_off));

                if (FALSE == is_hold_off)
                {
                    /** Two event should be raised */
                    SHR_IF_ERR_EXIT(dnx_port_signal_quality_er_event_raise
                                    (unit, port, BCM_PORT_SIGNAL_QUALITY_ERROR_RATE_STATE_FAIL_RECOVER, er_rate));

                    SHR_IF_ERR_EXIT(dnx_port_signal_quality_er_event_raise
                                    (unit, port, BCM_PORT_SIGNAL_QUALITY_ERROR_RATE_STATE_DEGRADE_RECOVER, er_rate));

                    SHR_IF_ERR_EXIT(dnx_algo_port_db.signal_quality.phy_state.prev_state_sampler.set(unit, first_phy,
                                                                                                     BCM_PORT_SIGNAL_QUALITY_ERROR_RATE_STATE_DEGRADE_RECOVER));
                }

                /**  Counter increment during transient state: fail -> recover */
                is_hold_off_interval_cnt_clear = FALSE;
            }
            else
            {
                /*
                 * check if error rate value exceeds threshold and determine the next state
                 */
                if (er_rate > 0 && er_rate >= fail_thr && prev_state != BCM_PORT_SIGNAL_QUALITY_ERROR_RATE_STATE_FAIL)
                {
                    state = BCM_PORT_SIGNAL_QUALITY_ERROR_RATE_STATE_FAIL;

                    /** Reset hold off interval counter upon state update to state fail */
                    is_hold_off_interval_cnt_clear = TRUE;
                }
                else if (er_rate < degrade_cu_thr
                         && prev_state != BCM_PORT_SIGNAL_QUALITY_ERROR_RATE_STATE_DEGRADE_RECOVER)
                {
                    state = BCM_PORT_SIGNAL_QUALITY_ERROR_RATE_STATE_DEGRADE_RECOVER;

                    /** Increment hold off interval counter */
                    is_hold_off_interval_cnt_clear = FALSE;
                }
                else if (er_rate < fail_cu_thr && prev_state == BCM_PORT_SIGNAL_QUALITY_ERROR_RATE_STATE_FAIL)
                {
                    state = BCM_PORT_SIGNAL_QUALITY_ERROR_RATE_STATE_FAIL_RECOVER;

                    /** Increment hold off interval counter */
                    is_hold_off_interval_cnt_clear = FALSE;
                }
                else if (er_rate >= degrade_thr
                         && prev_state == BCM_PORT_SIGNAL_QUALITY_ERROR_RATE_STATE_DEGRADE_RECOVER)
                {
                    state = BCM_PORT_SIGNAL_QUALITY_ERROR_RATE_STATE_DEGRADE;

                    /** Reset hold off interval counter upon state update to state degrade */
                    is_hold_off_interval_cnt_clear = TRUE;
                }

                /** notify in case the state change */
                if (state != prev_state)
                {
                    uint8 is_hold_off = FALSE;

                    /** Check if event should be held off */
                    SHR_IF_ERR_EXIT(dnx_port_signal_quality_er_is_hold_off_get
                                    (unit, first_phy, prev_state, state, sample_interval, &is_hold_off));

                    if (FALSE == is_hold_off)
                    {
                        SHR_IF_ERR_EXIT(dnx_port_signal_quality_er_event_raise(unit, port, state, er_rate));

                        SHR_IF_ERR_EXIT(dnx_algo_port_db.signal_quality.phy_state.prev_state_sampler.set(unit,
                                                                                                         first_phy,
                                                                                                         state));
                    }
                }
            }

            /** update state db */
            SHR_IF_ERR_EXIT(dnx_port_signal_quality_er_port_db_update
                            (unit, first_phy, curr_er_cnt, curr_uncorrectable_er_cnt));

            /** update hold off interval counter, increment if_hold_off_interval_cnt_clear equal to true otherwise reset it */
            SHR_IF_ERR_EXIT(dnx_port_signal_quality_er_hold_off_interval_cnt_set
                            (unit, first_phy, is_hold_off_interval_cnt_clear, sample_interval));
        }
    }

exit:

    /** Continue error recovery. */
    DNX_ERR_RECOVERY_UNSUPPRESS(unit);

    SHR_VOID_FUNC_EXIT;
}


/** see header file */
shr_error_e
dnx_port_signal_quality_er_init(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    /** verify feature */
    if (!dnx_data_nif.signal_quality.feature_get(unit, dnx_data_nif_signal_quality_is_supported))
    {
        SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Signal Quality is not supported for current device \n");
    }

    /** init sw-state */
    SHR_IF_ERR_EXIT(dnx_algo_port_db.signal_quality.phy_config.alloc(unit));

    SHR_IF_ERR_EXIT(dnx_algo_port_db.signal_quality.phy_state.alloc(unit));


exit:
    SHR_FUNC_EXIT;
}

/** see header file */
shr_error_e
dnx_port_signal_quality_er_control_port_and_type_verify(
    int unit,
    bcm_port_t port,
    bcm_port_control_t type)
{
    bcm_port_resource_t resource;
    int local_port;
    dnx_algo_port_info_s port_info;

    SHR_FUNC_INIT_VARS(unit);

    /** verify feature */
    if (!dnx_data_nif.signal_quality.feature_get(unit, dnx_data_nif_signal_quality_is_supported))
    {
        SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Signal Quality ER is not supported for current device \n");
    }

    /** verify port is valid */
    if (!BCM_GPORT_IS_LOCAL(port) && ((port < 0) || (port >= SOC_MAX_NUM_PORTS)))
    {
        SHR_ERR_EXIT(_SHR_E_PORT, "Invalid Port!\n");
    }

    local_port = (BCM_GPORT_IS_LOCAL(port)) ? BCM_GPORT_LOCAL_GET(port) : port;
    SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, local_port, &port_info));
    if (!DNX_ALGO_PORT_TYPE_IS_NIF_ETH(unit, port_info, 0))
    {
        SHR_ERR_EXIT(_SHR_E_PORT, "Port %d has invalid port type, must be NIF ETH!\n", local_port);
    }

    /** verify FEC is enabled */
    SHR_IF_ERR_EXIT(bcm_dnx_port_resource_get(unit, port, &resource));
    if (!dnx_data_nif.signal_quality.supported_fecs_get(unit, resource.fec_type)->is_valid)
    {
        SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Port must use a supported FEC type to work with Signal Quality ER logic\n");
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_port_signal_quality_er_control_set_verify(
    int unit,
    bcm_port_t port,
    bcm_port_control_t type,
    int value)
{
    float min_thr_value = 0, max_thr_value = 0, thr_value = 0;
    uint32 encoded_thr;
    int16 min_exp, max_exp;
    int first_phy;
    uint32 max_interval = 0;
    uint32 min_interval = 0;

    SHR_FUNC_INIT_VARS(unit);

    /** get phy correlating to the logical port */
    SHR_IF_ERR_EXIT(dnx_algo_port_nif_first_phy_get(unit, port, 0, &first_phy));

    switch (type)
    {
        case bcmPortControlSignalQualityErDegradeThreshold:

            /** min and max are based on absolute supported values */
            min_exp = dnx_data_nif.signal_quality.min_degrade_threshold_get(unit);
            dnx_port_signal_quality_er_man_and_exp_to_float_get(1, min_exp, &min_thr_value);

            max_exp = dnx_data_nif.signal_quality.max_degrade_threshold_get(unit);
            dnx_port_signal_quality_er_man_and_exp_to_float_get(1, max_exp, &max_thr_value);

            /** convert user encoded threshold to regular float */
            SHR_IF_ERR_EXIT(dnx_port_signal_quality_er_encoded_val_to_float_get(unit, value, &thr_value));

            /** verify thr is in range */
            if (thr_value > max_thr_value || thr_value < min_thr_value)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Threshold is out of range\n");
            }

            break;

        case bcmPortControlSignalQualityErDegradeRecoverThreshold:

            /** max threshold based on absolute maximum */
            min_exp = dnx_data_nif.signal_quality.min_degrade_threshold_get(unit);
            dnx_port_signal_quality_er_man_and_exp_to_float_get(1, min_exp, &min_thr_value);

            /** recover threshold should not be stricter than regular threshold */
            SHR_IF_ERR_EXIT(dnx_algo_port_db.signal_quality.phy_config.degrade_thr.get(unit, first_phy, &encoded_thr));

            SHR_IF_ERR_EXIT(dnx_port_signal_quality_er_encoded_val_to_float_get(unit, encoded_thr, &max_thr_value));

            /** convert user encoded threshold to regular float */
            SHR_IF_ERR_EXIT(dnx_port_signal_quality_er_encoded_val_to_float_get(unit, value, &thr_value));

            /** verify thr is in range */
            if (thr_value > max_thr_value || thr_value < min_thr_value)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Threshold is out of range\n");
            }

            break;

        case bcmPortControlSignalQualityErFailThreshold:

             /** min and max are based on absolute supported values */
            min_exp = dnx_data_nif.signal_quality.min_fail_threshold_get(unit);
            dnx_port_signal_quality_er_man_and_exp_to_float_get(1, min_exp, &min_thr_value);

            max_exp = dnx_data_nif.signal_quality.max_fail_threshold_get(unit);
            dnx_port_signal_quality_er_man_and_exp_to_float_get(1, max_exp, &max_thr_value);

            /** convert user encoded threshold to regular float */
            SHR_IF_ERR_EXIT(dnx_port_signal_quality_er_encoded_val_to_float_get(unit, value, &thr_value));

             /** verify thr is in range */
            if (thr_value > max_thr_value || thr_value < min_thr_value)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Threshold is out of range\n");
            }

            break;

        case bcmPortControlSignalQualityErFailRecoverThreshold:

            /** max threshold based on absolute maximum */
            min_exp = dnx_data_nif.signal_quality.min_fail_threshold_get(unit);
            dnx_port_signal_quality_er_man_and_exp_to_float_get(1, min_exp, &min_thr_value);

             /** recover threshold should not be stricter than regular threshold */
            SHR_IF_ERR_EXIT(dnx_algo_port_db.signal_quality.phy_config.fail_thr.get(unit, first_phy, &encoded_thr));

            SHR_IF_ERR_EXIT(dnx_port_signal_quality_er_encoded_val_to_float_get(unit, encoded_thr, &max_thr_value));

            /** convert user encoded threshold to regular float */
            SHR_IF_ERR_EXIT(dnx_port_signal_quality_er_encoded_val_to_float_get(unit, value, &thr_value));

            /** verify thr is in range */
            if (thr_value > max_thr_value || thr_value < min_thr_value)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Threshold is out of range\n");
            }

            break;

        case bcmPortControlSignalQualityErMeasureMode:

            /** verify BER is supportedd */
            if (!dnx_data_nif.signal_quality.feature_get(unit, dnx_data_nif_signal_quality_is_ber_supported))
            {
                if (value == BCM_PORT_SIGNAL_QUALITY_ERROR_RATE_MEASURE_MODE_BER)
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM, "Measurements based on BER is not supported\n");
                }
            }

            /** verify SER is supported */
            if (!dnx_data_nif.signal_quality.feature_get(unit, dnx_data_nif_signal_quality_is_ser_supported))
            {
                if (value == BCM_PORT_SIGNAL_QUALITY_ERROR_RATE_MEASURE_MODE_SER)
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM, "Measurements based on SER is not supported\n");
                }
            }

            /** verify value is within range */
            SHR_RANGE_VERIFY(value,
                             BCM_PORT_SIGNAL_QUALITY_ERROR_RATE_MEASURE_MODE_DISABLED,
                             BCM_PORT_SIGNAL_QUALITY_ERROR_RATE_MEASURE_MODE_LAST - 1,
                             _SHR_E_PARAM, "Value is out of range\n");

            break;

        case bcmPortControlSignalQualityErActionEnable:
            SHR_RANGE_VERIFY(value, 0, 1, _SHR_E_PARAM, "Value should be 0 or 1");

            break;

        case bcmPortControlSignalQualityErInterval:

            /** get interval range */

            min_interval = dnx_data_nif.signal_quality.min_interval_get(unit);
            max_interval = dnx_data_nif.signal_quality.max_interval_get(unit);

            SHR_RANGE_VERIFY(value, min_interval, max_interval, _SHR_E_PARAM, "Value is out of range\n");

            break;

        case bcmPortControlSignalQualityErHoldOffInterval:

            /** verify value is in range */
            min_interval = dnx_data_nif.signal_quality.min_interval_get(unit);
            max_interval = dnx_data_nif.signal_quality.max_interval_get(unit);

            /** Zero value is used to disable the hold off feature **/
            if (0 != value)
            {
                SHR_RANGE_VERIFY(value, min_interval, max_interval, _SHR_E_PARAM, "Value is out of range\n");
            }

            break;

        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "type %d is not supported\n", type);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief       in case no port is enabled, remove logic from counter thread
 */
static shr_error_e
dnx_port_signal_quality_er_counter_thread_bc_set(
    int unit)
{
    shr_error_e rv;
    uint8 is_empty;

    SHR_FUNC_INIT_VARS(unit);

    /** check if any port uses the logic */
    SHR_IF_ERR_EXIT(dnx_algo_port_db.signal_quality.enabled_ports.pbmp_is_null(unit, &is_empty));

    /** Add CB to counter thread */
    if (!is_empty)
    {
        /** add cb to counter thread in case at least 1 port uses signal quality ER */
        SHR_IF_ERR_EXIT(soc_counter_extra_register(unit, dnx_port_signal_quality_er_check));
    }
    else
    {
        /** remove cb from counter thread if there is no port that uses the feature */
        rv = soc_counter_extra_unregister(unit, dnx_port_signal_quality_er_check);
        if (rv == _SHR_E_NOT_FOUND)
        {
            SHR_EXIT();
        }
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_port_signal_quality_er_state_db_port_clear(
    int unit,
    int first_phy)
{
    SHR_FUNC_INIT_VARS(unit);

    /** Reset previous state */
    SHR_IF_ERR_EXIT(dnx_algo_port_db.signal_quality.phy_state.prev_state_sampler.set(unit, first_phy,
                                                                                     BCM_PORT_SIGNAL_QUALITY_ERROR_RATE_STATE_DEGRADE_RECOVER));

    SHR_IF_ERR_EXIT(dnx_algo_port_db.signal_quality.phy_state.prev_state_handler.set(unit, first_phy,
                                                                                     BCM_PORT_SIGNAL_QUALITY_ERROR_RATE_STATE_DEGRADE_RECOVER));

exit:
    SHR_FUNC_EXIT;
}

/** see header file */
shr_error_e
dnx_port_signal_quality_er_state_db_clear(
    int unit)
{
    bcm_pbmp_t enabled_ports;
    bcm_port_t port;

    SHR_FUNC_INIT_VARS(unit);

    /** get bitmap of ports that use Signal quality ER feature */
    SHR_IF_ERR_EXIT(dnx_algo_port_db.signal_quality.enabled_ports.pbmp_get(unit, &enabled_ports));

    /** interate over all port and set state to default DEGRADE_RECOVER */
    _SHR_PBMP_ITER(enabled_ports, port)
    {
        int first_phy;

        /** get phy correlating to the logical port */
        SHR_IF_ERR_EXIT(dnx_algo_port_nif_first_phy_get(unit, port, 0, &first_phy));

        /** clear state of corresponding first_phy */
        SHR_IF_ERR_EXIT(dnx_port_signal_quality_er_state_db_port_clear(unit, first_phy));
    }

exit:
    SHR_FUNC_EXIT;
}

/** see header file */
shr_error_e
dnx_port_signal_quality_er_control_set(
    int unit,
    bcm_port_t port,
    bcm_port_control_t type,
    int value)
{
    int first_phy;
    int is_enable;

    SHR_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNXC(dnx_port_signal_quality_er_control_set_verify(unit, port, type, value));

    /** get phy correlating to the logical port */
    SHR_IF_ERR_EXIT(dnx_algo_port_nif_first_phy_get(unit, port, 0, &first_phy));

    switch (type)
    {
        case bcmPortControlSignalQualityErDegradeThreshold:
            SHR_IF_ERR_EXIT(dnx_algo_port_db.signal_quality.phy_config.degrade_thr.set(unit, first_phy, value));
            SHR_IF_ERR_EXIT(dnx_algo_port_db.signal_quality.phy_config.degrade_cu_thr.set(unit, first_phy, value));

            break;
        case bcmPortControlSignalQualityErDegradeRecoverThreshold:
            SHR_IF_ERR_EXIT(dnx_algo_port_db.signal_quality.phy_config.degrade_cu_thr.set(unit, first_phy, value));

            break;
        case bcmPortControlSignalQualityErFailThreshold:
            SHR_IF_ERR_EXIT(dnx_algo_port_db.signal_quality.phy_config.fail_thr.set(unit, first_phy, value));
            SHR_IF_ERR_EXIT(dnx_algo_port_db.signal_quality.phy_config.fail_cu_thr.set(unit, first_phy, value));

            break;
        case bcmPortControlSignalQualityErFailRecoverThreshold:
            SHR_IF_ERR_EXIT(dnx_algo_port_db.signal_quality.phy_config.fail_cu_thr.set(unit, first_phy, value));

            break;
        case bcmPortControlSignalQualityErMeasureMode:
            is_enable = (value != BCM_PORT_SIGNAL_QUALITY_ERROR_RATE_MEASURE_MODE_DISABLED ? 1 : 0);

            if (is_enable)
            {
                /** enable port */
                SHR_IF_ERR_EXIT(dnx_algo_port_db.signal_quality.enabled_ports.pbmp_port_add(unit, port));

                /** SER or BER */
                if (value == BCM_PORT_SIGNAL_QUALITY_ERROR_RATE_MEASURE_MODE_BER)
                {
                    SHR_IF_ERR_EXIT(dnx_algo_port_db.signal_quality.measure_mode.pbmp_port_add(unit, port));
                }
                else
                {
                    SHR_IF_ERR_EXIT(dnx_algo_port_db.signal_quality.measure_mode.pbmp_port_remove(unit, port));
                }
            }
            else
            {
                /** disable port */
                SHR_IF_ERR_EXIT(dnx_algo_port_db.signal_quality.enabled_ports.pbmp_port_remove(unit, port));
                SHR_IF_ERR_EXIT(dnx_algo_port_db.signal_quality.measure_mode.pbmp_port_remove(unit, port));

            }

            /** Reset previous state */
            SHR_IF_ERR_EXIT(dnx_port_signal_quality_er_state_db_port_clear(unit, first_phy));

            /** clear state variable */
            SHR_IF_ERR_EXIT(dnx_port_signal_quality_er_port_db_update(unit, first_phy,
                                                                      0xffffffffffffffffUL, 0xffffffffffffffffUL));

            /** clear last calculated error rate */
            SHR_IF_ERR_EXIT(dnx_port_signal_quality_er_last_error_rate_set(unit, first_phy, 0));

            /*
             * in case no port is enabled, add/remove logic to/from counter thread
             */
            SHR_IF_ERR_EXIT(dnx_port_signal_quality_er_counter_thread_bc_set(unit));

            break;
        case bcmPortControlSignalQualityErInterval:
            SHR_IF_ERR_EXIT(dnx_algo_port_db.signal_quality.phy_config.interval.set(unit, first_phy, value));

            break;
        case bcmPortControlSignalQualityErActionEnable:
            SHR_IF_ERR_EXIT(dnx_algo_port_db.signal_quality.phy_config.action_enabled.set(unit, first_phy, value));

            break;
        case bcmPortControlSignalQualityErHoldOffInterval:
            SHR_IF_ERR_EXIT(dnx_algo_port_db.signal_quality.phy_config.hold_off_interval.set(unit, first_phy, value));

            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "type %d is not supported\n", type);
    }
exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_port_signal_quality_er_measure_mode_get(
    int unit,
    bcm_port_t port,
    int is_enabled,
    int *measure_mode)
{
    uint8 is_ber;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_db.signal_quality.measure_mode.pbmp_member(unit, port, &is_ber));

    if (!is_enabled)
    {
        /** disabled */
        *measure_mode = BCM_PORT_SIGNAL_QUALITY_ERROR_RATE_MEASURE_MODE_DISABLED;
    }
    else
    {
        if (is_ber)
        {
            /** BER */
            *measure_mode = BCM_PORT_SIGNAL_QUALITY_ERROR_RATE_MEASURE_MODE_BER;
        }
        else
        {
            /** SER */
            *measure_mode = BCM_PORT_SIGNAL_QUALITY_ERROR_RATE_MEASURE_MODE_SER;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/** see header file */
shr_error_e
dnx_port_signal_quality_er_control_get(
    int unit,
    bcm_port_t port,
    bcm_port_control_t type,
    int *value_p)
{
    uint8 is_enabled;
    int first_phy;

    SHR_FUNC_INIT_VARS(unit);

    /** get phy correlating to the logical port */
    SHR_IF_ERR_EXIT(dnx_algo_port_nif_first_phy_get(unit, port, 0, &first_phy));

    switch (type)
    {
        case bcmPortControlSignalQualityErDegradeThreshold:
            SHR_IF_ERR_EXIT(dnx_algo_port_db.signal_quality.phy_config.
                            degrade_thr.get(unit, first_phy, (uint32 *) value_p));

            break;
        case bcmPortControlSignalQualityErDegradeRecoverThreshold:
            SHR_IF_ERR_EXIT(dnx_algo_port_db.signal_quality.phy_config.
                            degrade_cu_thr.get(unit, first_phy, (uint32 *) value_p));

            break;

        case bcmPortControlSignalQualityErFailThreshold:
            SHR_IF_ERR_EXIT(dnx_algo_port_db.signal_quality.phy_config.
                            fail_thr.get(unit, first_phy, (uint32 *) value_p));

            break;

        case bcmPortControlSignalQualityErFailRecoverThreshold:
            SHR_IF_ERR_EXIT(dnx_algo_port_db.signal_quality.phy_config.
                            fail_cu_thr.get(unit, first_phy, (uint32 *) value_p));

            break;

        case bcmPortControlSignalQualityErMeasureMode:
            SHR_IF_ERR_EXIT(dnx_algo_port_db.signal_quality.enabled_ports.pbmp_member(unit, port, &is_enabled));

            SHR_IF_ERR_EXIT(dnx_port_signal_quality_er_measure_mode_get(unit, port, is_enabled, value_p));

            break;

        case bcmPortControlSignalQualityErInterval:
            SHR_IF_ERR_EXIT(dnx_algo_port_db.signal_quality.phy_config.
                            interval.get(unit, first_phy, (uint32 *) value_p));
            break;

        case bcmPortControlSignalQualityErActionEnable:
            SHR_IF_ERR_EXIT(dnx_algo_port_db.signal_quality.phy_config.
                            action_enabled.get(unit, first_phy, (uint32 *) value_p));
            break;

        case bcmPortControlSignalQualityErHoldOffInterval:
            SHR_IF_ERR_EXIT(dnx_algo_port_db.signal_quality.phy_config.
                            hold_off_interval.get(unit, first_phy, (uint32 *) value_p));
            break;

        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "type %d is not supported\n", type);
    }
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_port_signal_quality_last_er_error_rate_get(
    int unit,
    bcm_port_t port,
    uint32 *value_p)
{
    SHR_FUNC_INIT_VARS(unit);

    *value_p = 0;

    /** verify feature */
    if (dnx_data_nif.signal_quality.feature_get(unit, dnx_data_nif_signal_quality_is_supported))
    {
        uint8 is_enabled;

        SHR_IF_ERR_EXIT(dnx_algo_port_db.signal_quality.enabled_ports.pbmp_member(unit, port, &is_enabled));

        if (is_enabled)
        {
            int first_phy;

            /** get phy correlating to the logical port */
            SHR_IF_ERR_EXIT(dnx_algo_port_nif_first_phy_get(unit, port, 0, &first_phy));

            /** get the last calculated error rate */
            SHR_IF_ERR_EXIT(dnx_algo_port_db.signal_quality.phy_state.prev_error_rate.get(unit, first_phy, value_p));
        }
    }

exit:
    SHR_FUNC_EXIT;
}
