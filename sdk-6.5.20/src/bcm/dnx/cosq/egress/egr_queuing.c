/**
 * \file egr_queuing.c
 *
 * Port Egress reassembly context management functionality for DNX.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_EGRESSDNX

#include <shared/utilex/utilex_bitstream.h>
#include <sal/core/libc.h>
#include <bcm/types.h>
#include <bcm/error.h>
#include <bcm/cosq.h>
#include <soc/dnx/dbal/dbal.h>
#include <bcm_int/dnx/cmn/dnxcmn.h>
#include <soc/dnx/swstate/auto_generated/access/dnx_egr_db_access.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_egr_queuing.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_device.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_tdm.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_port.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_fabric.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_esb.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_nif.h>
#include <soc/dnx/dnx_err_recovery_manager.h>
#include <bcm_int/dnx/gtimer/gtimer.h>
#include <bcm_int/dnx/cosq/egress/egq_ofp_rates.h>
#include <bcm_int/dnx/cosq/egress/egq_dbal.h>
#include <bcm_int/dnx/cosq/egress/cosq_egq.h>
#include <bcm_int/dnx/cosq/egress/egq_ps_db.h>
#include <bcm_int/dnx/cosq/cosq.h>
#include <bcm_int/dnx/cosq/egress/esb.h>
#include <bcm_int/dnx/cosq/egress/egr_queuing.h>
#include <bcm_int/dnx/algo/port/algo_port_mgmt.h>
#include <bcm_int/dnx/algo/cal/algo_cal.h>
#include <bcm_int/dnx/tdm/tdm.h>
#include <sal/core/boot.h>
#include <bcm_int/dnx_dispatch.h>
#include <bcm_int/dnx/port/port.h>
#include <bcm_int/dnx/algo/egq/egq_alloc_mngr.h>

#include <bcm_int/dnx/port/port_ingr_reassembly.h>

#include "ecgm_dbal.h"
#include "phantom_queues.h"



#include <soc/feature.h>
#include <soc/types.h>
#include <soc/dnxc/drv_dnxc_utils.h>
#ifdef ADAPTER_SERVER_MODE
#include <soc/dnx/adapter/adapter_gen.h>
#endif
/**
 * \brief -
 *   Get the index of the smallest member of an uin32 array (1st occurence)
 *
 * \param [in] numbers -
 *   Pointer to an array of 'uint32'. Number of elements is 'nof_numbers'.
 * \param [in] nof_numbers -
 *   See 'numbers' above.
 * \param [out] smallest_idx_p -
 *   Pointer to uint32. This procedure loads pointed memory by the index
 *   of the smallest member of 'numbers[]'.
 * \return
 *   If zero (_SHR_E_NONE), then no error was encountered.
 *   Otherwise, see shr_error_e
 * \remark
 *   * None
 * \see
 *   * dnx_egr_queuing_lcm_above_sum_get
 */
static shr_error_e
dnx_egr_queuing_get_smallest_idx(
    uint32 *numbers,
    uint32 nof_numbers,
    uint32 *smallest_idx_p)
{
    int ii;

    SHR_FUNC_INIT_VARS(NO_UNIT);
    *smallest_idx_p = 0;
    for (ii = 1; ii < nof_numbers; ii++)
    {
        if (numbers[ii] < numbers[*smallest_idx_p])
        {
            *smallest_idx_p = ii;
        }
    }
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
/**
 * See .h file
 */
int
dnx_egr_queuing_nif_rate_to_weight(
    int unit,
    uint32 rate,
    uint32 *weight_p)
{
    SHR_FUNC_INIT_VARS(unit);

    if (rate == 0)
    {
        *weight_p = 0;
    }
    else
    {
        /*
         * Dividing rate by the MUX calendar resolution and rounding up to power of 2
         */
        rate = UTILEX_DIV_ROUND_UP(rate, dnx_data_egr_queuing.common_max_val.mux_calendar_resolution_get(unit));
        *weight_p = utilex_power_of_2(utilex_log2_round_up(rate));
    }
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief -
 *   Given an array of uint32, get the index of the last member which is non-zero.
 *
 * \param [in] arr -
 *   Pointer to 'uint32'. Array to investigate. Number of elements on array
 *   is 'arr_length'.
 * \param [in] arr_length -
 *   uint32. Number of element on 'arr'.
 * \return
 *   Index of last non-zero element. See 'arr' above.
 * \remark
 *   * None
 * \see
 *   * dnx_egr_q_nif_cal_set
 */
static uint32
dnx_egr_queuing_get_last_value(
    uint32 *arr,
    uint32 arr_length)
{
    uint32 ii, last_idx = 0;

    last_idx = 0;
    for (ii = 0; ii < arr_length; ii++)
    {
        if (arr[ii] != 0)
        {
            last_idx = ii;
        }
    }
    return last_idx;
}
/**
 * \brief -
 *   Given an array of uint32, Indicate whether all members are
 *   equal to each other (return a non-zero value).
 *
 * \param [in] numbers -
 *   Pointer to 'uint32'. Array to investigate. Number of elements on array
 *   is 'nof_numbers'.
 * \param [in] nof_numbers -
 *   uint32. Number of element on 'numbers'.
 * \return
 *   Non-zero: if all members are equal. See 'numbers' above.
 * \remark
 *   * None
 * \see
 *   * dnx_egr_q_nif_cal_set
 */
static int
dnx_egr_queuing_is_all_equal(
    uint32 *numbers,
    uint32 nof_numbers)
{
    uint32 ii, comparator = numbers[0];
    for (ii = 1; ii < nof_numbers; ii++)
    {
        if (numbers[ii] != comparator)
        {
            return FALSE;
        }
    }
    return TRUE;
}
/**
 * \brief -
 *   Given an array of uint32 and a minimal 'base', get the 'least common
 *   multiplier' that is larger than or equal to 'base'. See 'remark'
 *
 * \param [in] unit -
 *   HW Unit-ID
 * \param [in] numbers -
 *   Pointer to 'uint32'. Array to investigate. Number of elements on array
 *   is 'nof_numbers'.
 * \param [in] nof_numbers -
 *   uint32. Number of element on 'numbers'.
 * \param [in] sum -
 *   uint32. This is the 'base'. The required LCM must be larger or equal to
 *   this 'base'.
 * \param [in] lcm_p -
 *   Pointer to uint32. This procedure loads pointed memory by an LCM that is
 *   larger than 'sum'
 * \return
 *   If zero (_SHR_E_NONE), then no error was encountered.
 *   Otherwise, see shr_error_e
 * \remark
 *   * LCM is the smallest number that is a multiplication of all array
 *     elements.
 * \see
 *   * dnx_egr_q_nif_cal_set
 */
static int
dnx_egr_queuing_lcm_above_sum_get(
    int unit,
    uint32 *numbers,
    uint32 nof_numbers,
    uint32 sum,
    uint32 *lcm_p)
{
    uint32 curr_iter[BCM_PBMP_PORT_MAX] = { 0 }, current_lcm = 0, smallest_idx = 0;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(numbers, _SHR_E_PARAM, "numbers");
    sal_memcpy(curr_iter, numbers, nof_numbers * sizeof(curr_iter[0]));

    while (current_lcm < sum)
    {
        if (dnx_egr_queuing_is_all_equal(curr_iter, nof_numbers))
        {
            if (current_lcm != curr_iter[0])
            {
                current_lcm = curr_iter[0];
                continue;
            }
        }
        SHR_IF_ERR_EXIT(dnx_egr_queuing_get_smallest_idx(curr_iter, nof_numbers, &smallest_idx));
        curr_iter[smallest_idx] += numbers[smallest_idx];
    }
    *lcm_p = current_lcm;
exit:
    SHR_FUNC_EXIT;
}

/*
 * See egr_queuing.h file
 */
shr_error_e
dnx_egr_queuing_cal_params_calc(
    int unit,
    uint32 *weights,
    uint32 max_cal_len,
    uint32 *cal_len_optimal,
    uint32 *nof_cal_objects,
    uint32 *dummy_start)
{
    uint32 total_weight_sum = 0;
    uint32 calendar_length, nof_padding_if;
    int ii, jj = 0, nof_active_if = 0;
    uint32 lcm_temp[BCM_PBMP_PORT_MAX];
    SHR_FUNC_INIT_VARS(unit);

    *dummy_start = 0;
    sal_memset(lcm_temp, 0, sizeof(lcm_temp));
    for (ii = 0, jj = 0; ii < BCM_PBMP_PORT_MAX; ii++)
    {
        /**
         * Calculate the number of active IFs/queues and populate the LCM (lowest common multiple)
         * array that will be investigated in order to find the lowest common multiple for the calendar length
         */
        if (weights[ii] != 0)
        {
            nof_active_if++;
            lcm_temp[jj++] = weights[ii];
            total_weight_sum += weights[ii];
        }
    }

    do
    {
        /**
         * Get LCM (lowest common multiple) for the calendar length
         */
        SHR_IF_ERR_EXIT(dnx_egr_queuing_lcm_above_sum_get
                        (unit, lcm_temp, nof_active_if, total_weight_sum, &calendar_length));
        if (calendar_length > max_cal_len)
        {
            /**
             * If the calculated calendar length is bigger than calendar max length, weights needs to be divided by 2 to fit the limitation
             */
            total_weight_sum = 0;
            for (ii = 0, jj = 0; ii < BCM_PBMP_PORT_MAX; ii++)
            {
                if (weights[ii] != 0)
                {
                    lcm_temp[jj++] = weights[ii] = (weights[ii] + 1) / 2;
                    total_weight_sum += weights[ii];
                }
            }
        }
    }
    while (calendar_length > max_cal_len);

    /**
     * Calculate how many dummy interfaces are required and pad accordingly
     */
    nof_padding_if = calendar_length - total_weight_sum;
    *dummy_start = dnx_egr_queuing_get_last_value(weights, BCM_PBMP_PORT_MAX) + 1;
    for (ii = 0; ii < nof_padding_if; ii++)
    {
        weights[*dummy_start + ii] = 1;
    }

    *nof_cal_objects = *dummy_start + nof_padding_if + 1;
    *cal_len_optimal = calendar_length;
exit:
    SHR_FUNC_EXIT;
}

/*
 * See egr_queuing.h file
 */
shr_error_e
dnx_egr_queuing_recycle_bandwidth_set(
    int unit,
    int core,
    int is_mirror,
    uint32 kbits_sec_speed)
{
    uint32 internal_rate;
    dbal_fields_e dbal_field_id;
    uint32 egq_resolution;

    SHR_FUNC_INIT_VARS(unit);
    /*
     * For DATA mode. Conversion coefficient is 'credits per bit'
     */
    egq_resolution = dnx_data_egr_queuing.params.cal_res_get(unit);
    /*
     * Convert input Kilo bits per sec to credits.
     * Not a calendar so the calendar slots and length will be 1
     */
    SHR_IF_ERR_EXIT(dnx_ofp_rates_calculate_credits_from_rate
                    (unit, 1, 1, 1, egq_resolution, FALSE, kbits_sec_speed, &internal_rate));
    if (is_mirror)
    {
        dbal_field_id = DBAL_FIELD_HP_MIRR_RATE;
    }
    else
    {
        dbal_field_id = DBAL_FIELD_RCY_RATE;
    }
    SHR_IF_ERR_EXIT(dnx_dbal_gen_set(unit, DBAL_TABLE_EGQ_SHAPER_RCY_MIRR_CONFIGURATION,
                                     1, 1,
                                     GEN_DBAL_FIELD32, DBAL_FIELD_CORE_ID, core,
                                     GEN_DBAL_FIELD32, dbal_field_id, INST_SINGLE, internal_rate,
                                     GEN_DBAL_FIELD_LAST_MARK));
exit:
    SHR_FUNC_EXIT;
}
/*
 * See egr_queuing.h file
 */
shr_error_e
dnx_egr_queuing_recycle_bandwidth_get(
    int unit,
    int core,
    int is_mirror,
    uint32 *kbits_sec_speed_p)
{
    dbal_fields_e dbal_field_id;
    uint32 internal_rate;

    SHR_FUNC_INIT_VARS(unit);
    if (is_mirror)
    {
        dbal_field_id = DBAL_FIELD_HP_MIRR_RATE;
    }
    else
    {
        dbal_field_id = DBAL_FIELD_RCY_RATE;
    }
    SHR_IF_ERR_EXIT(dnx_dbal_gen_get(unit, DBAL_TABLE_EGQ_SHAPER_RCY_MIRR_CONFIGURATION,
                                     1, 1,
                                     GEN_DBAL_FIELD32, DBAL_FIELD_CORE_ID, core,
                                     GEN_DBAL_FIELD32, dbal_field_id, INST_SINGLE, &internal_rate,
                                     GEN_DBAL_FIELD_LAST_MARK));
    /*
     * translate from internal rate (credits) to Kbps
     */
    if (internal_rate != 0)
    {
        SHR_IF_ERR_EXIT(dnx_ofp_rates_calculate_rate_from_credits
                        (unit, 1, 1, 1, 0, DNX_EGQ_UNITS_VAL_IN_BITS_INTF_RECYCLE, &internal_rate));
    }
    *kbits_sec_speed_p = internal_rate;
exit:
    SHR_FUNC_EXIT;
}
/*
 * See egr_queuing.h file
 */
shr_error_e
dnx_egr_recycle_burst_set(
    int unit,
    int core,
    int is_mirror,
    uint32 burst)
{
    dbal_fields_e dbal_field_id;

    SHR_FUNC_INIT_VARS(unit);
    if (is_mirror)
    {
        dbal_field_id = DBAL_FIELD_HP_MIRR_MAX_BURST;
    }
    else
    {
        dbal_field_id = DBAL_FIELD_RCY_MAX_BURST;
    }
    /*
     * We are assuming that input 'core' may also be 'BCM_CORE_ALL', in which case
     * DBAL will operate on all cores.
     */
    SHR_IF_ERR_EXIT(dnx_dbal_gen_set
                    (unit, DBAL_TABLE_EGQ_SHAPER_RCY_MIRR_CONFIGURATION,
                     1, 1,
                     GEN_DBAL_FIELD32, DBAL_FIELD_CORE_ID, core, GEN_DBAL_FIELD32, dbal_field_id, INST_SINGLE, burst,
                     GEN_DBAL_FIELD_LAST_MARK));
exit:
    SHR_FUNC_EXIT;
}
/*
 * See egr_queuing.h file
 */
shr_error_e
dnx_egr_recycle_burst_get(
    int unit,
    int core,
    int is_mirror,
    uint32 *burst)
{
    dbal_fields_e dbal_field_id;

    SHR_FUNC_INIT_VARS(unit);
    if (is_mirror)
    {
        dbal_field_id = DBAL_FIELD_HP_MIRR_MAX_BURST;
    }
    else
    {
        dbal_field_id = DBAL_FIELD_RCY_MAX_BURST;
    }
    SHR_IF_ERR_EXIT(dnx_dbal_gen_get
                    (unit, DBAL_TABLE_EGQ_SHAPER_RCY_MIRR_CONFIGURATION,
                     1, 1,
                     GEN_DBAL_FIELD32, DBAL_FIELD_CORE_ID, core, GEN_DBAL_FIELD32, dbal_field_id, INST_SINGLE, burst,
                     GEN_DBAL_FIELD_LAST_MARK));
exit:
    SHR_FUNC_EXIT;
}
/*
 * See egr_queuing.h file
 */
shr_error_e
dnx_egr_recycle_weight_set(
    int unit,
    int core,
    int is_mirror,
    uint32 weight)
{
    dbal_fields_e dbal_field_id_weight;
    dbal_fields_e dbal_field_id_disable;

    SHR_FUNC_INIT_VARS(unit);
    if (is_mirror)
    {
        dbal_field_id_weight = DBAL_FIELD_HP_MIRR_WEIGHT;
        dbal_field_id_disable = DBAL_FIELD_HP_MIRR_WFQ_DIS;
    }
    else
    {
        dbal_field_id_weight = DBAL_FIELD_RCY_WEIGHT;
        dbal_field_id_disable = DBAL_FIELD_RCY_WFQ_DIS;
    }
    SHR_IF_ERR_EXIT(dnx_dbal_gen_set
                    (unit, DBAL_TABLE_EGQ_SHAPER_RCY_MIRR_CONFIGURATION,
                     1, 2,
                     GEN_DBAL_FIELD32, DBAL_FIELD_CORE_ID, core,
                     GEN_DBAL_FIELD32, dbal_field_id_weight, INST_SINGLE, weight,
                     GEN_DBAL_FIELD32, dbal_field_id_disable, INST_SINGLE, ((weight == 0) ? 1 : 0),
                     GEN_DBAL_FIELD_LAST_MARK));
exit:
    SHR_FUNC_EXIT;
}
/*
 * See egr_queuing.h file
 */
shr_error_e
dnx_egr_recycle_weight_get(
    int unit,
    int core,
    int is_mirror,
    uint32 *weight)
{
    dbal_fields_e dbal_field_id;

    SHR_FUNC_INIT_VARS(unit);
    if (is_mirror)
    {
        dbal_field_id = DBAL_FIELD_HP_MIRR_WEIGHT;
    }
    else
    {
        dbal_field_id = DBAL_FIELD_RCY_WEIGHT;
    }
    SHR_IF_ERR_EXIT(dnx_dbal_gen_get
                    (unit, DBAL_TABLE_EGQ_SHAPER_RCY_MIRR_CONFIGURATION,
                     1, 1,
                     GEN_DBAL_FIELD32, DBAL_FIELD_CORE_ID, core, GEN_DBAL_FIELD32, dbal_field_id, INST_SINGLE, weight,
                     GEN_DBAL_FIELD_LAST_MARK));
exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief - map port to egress recycle interface
 *
 * \param [in] unit -  Unit-ID
 * \param [in] port -  port
 *
 * \return
 *   shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_egr_port_recycle_interface_map_set(
    int unit,
    bcm_port_t port)
{
    bcm_core_t core;
    int base_q_pair;
    int interface_offset;

    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, port, &core));
    SHR_IF_ERR_EXIT(dnx_algo_port_base_q_pair_get(unit, port, &base_q_pair));
    SHR_IF_ERR_EXIT(dnx_algo_port_interface_offset_get(unit, port, &interface_offset));
    /*
     * write value to HW
     */
    /*
     * We assume that 'core' may also be 'BCM_CORE_ALL' and that it may be passed, as is,
     * to DBAL (I.e., we assume that DBAL_CORE_ALL is the same as BCM_CORE_ALL).
     */
    SHR_IF_ERR_EXIT(dnx_dbal_gen_set(unit, DBAL_TABLE_EGQ_SHAPING_OTM_ATTRIBUTES,
                                     2, 1,
                                     GEN_DBAL_FIELD32, DBAL_FIELD_QPAIR, base_q_pair,
                                     GEN_DBAL_FIELD32, DBAL_FIELD_CORE_ID, core,
                                     GEN_DBAL_FIELD32, DBAL_FIELD_RCY_TXQ_NUM, INST_SINGLE,
                                     (uint32) (interface_offset ? 1 : 0), GEN_DBAL_FIELD_LAST_MARK));
exit:
    SHR_FUNC_EXIT;
}

/*
 * See egr_queuing.h file
 */
shr_error_e
dnx_egr_port_recycle_interface_map_get(
    int unit,
    bcm_port_t port,
    int *rcy_if_idx)
{
    bcm_core_t core;
    int base_q_pair;
    uint32 *rcy_if_idx_p;

    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, port, &core));
    SHR_IF_ERR_EXIT(dnx_algo_port_base_q_pair_get(unit, port, &base_q_pair));
    rcy_if_idx_p = (uint32 *) rcy_if_idx;
    SHR_IF_ERR_EXIT(dnx_dbal_gen_get(unit, DBAL_TABLE_EGQ_SHAPING_OTM_ATTRIBUTES,
                                     2, 1,
                                     GEN_DBAL_FIELD32, DBAL_FIELD_QPAIR, base_q_pair,
                                     GEN_DBAL_FIELD32, DBAL_FIELD_CORE_ID, core,
                                     GEN_DBAL_FIELD32, DBAL_FIELD_RCY_TXQ_NUM, INST_SINGLE, rcy_if_idx_p,
                                     GEN_DBAL_FIELD_LAST_MARK));
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - map egr interface to nif
 *
 * \param [in] unit -  Unit-ID
 * \param [in] port -  port
 *
 * \return
 *   shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_egq_interface_to_nif_port_map_set(
    int unit,
    bcm_port_t port)
{
    int egr_if;
    dnx_algo_port_ethu_access_info_t cdu_info;
    int nif_port;
    bcm_core_t core;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_ethu_access_info_get(unit, port, &cdu_info));
    SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, port, &core));
    SHR_IF_ERR_EXIT(dnx_algo_port_nif_first_phy_get(unit, port, 0, &nif_port));
    SHR_IF_ERR_EXIT(dnx_algo_port_egr_if_get(unit, port, &egr_if));

    SHR_IF_ERR_EXIT(dnx_dbal_gen_set(unit, DBAL_TABLE_EGQ_INTERFACE_ATTRIBUTES,
                                     2, 3,
                                     GEN_DBAL_FIELD32, DBAL_FIELD_EGQ_IF, (uint32) egr_if,
                                     GEN_DBAL_FIELD32, DBAL_FIELD_CORE_ID, core,
                                     GEN_DBAL_FIELD32, DBAL_FIELD_PM_NUM, INST_SINGLE,
                                     (uint32) (cdu_info.ethu_id_in_core), GEN_DBAL_FIELD32,
                                     DBAL_FIELD_PM_INTERNAL_PORT, INST_SINGLE,
                                     (uint32) (cdu_info.first_lane_in_port), GEN_DBAL_FIELD32,
                                     DBAL_FIELD_NIF_PORT_NUM, INST_SINGLE,
                                     (uint32) (nif_port / dnx_data_device.general.nof_cores_get(unit)),
                                     GEN_DBAL_FIELD_LAST_MARK));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - configures credit size to HW
 *
 * \param [in] unit -  Unit-ID
 * \param [in] port -  port
 * \param [in] credit_size -  credit size for port
 *
 * \return
 *   shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_egr_queuing_nif_credit_set(
    int unit,
    bcm_port_t port,
    uint32 credit_size)
{

    int core;
    int egr_if;

    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(dnx_algo_port_egr_if_get(unit, port, &egr_if));
    SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, port, &core));
    /*
     * configure TXI credit per interface. CREDIT_INIT needs to be set and cleared for the interface after setting the size
     */
    SHR_IF_ERR_EXIT(dnx_dbal_gen_set(unit, DBAL_TABLE_EGQ_GLOBAL_CONFIGURATION,
                                     1, 1,
                                     GEN_DBAL_FIELD32, DBAL_FIELD_CORE_ID, (uint32) core,
                                     GEN_DBAL_FIELD32, DBAL_FIELD_EGQ_NIF_CREDIT_SIZE, INST_SINGLE,
                                     (uint32) (credit_size), GEN_DBAL_FIELD_LAST_MARK));
    /*
     * Set the bit corresponding to indicated interface and update, this way, credit size
     */
    SHR_IF_ERR_EXIT(dnx_dbal_gen_set(unit, DBAL_TABLE_EGQ_INTERFACE_REG_ATTRIBUTES,
                                     2, 1,
                                     GEN_DBAL_FIELD32, DBAL_FIELD_CORE_ID, (uint32) core,
                                     GEN_DBAL_FIELD32, DBAL_FIELD_EGQ_IF, (uint32) egr_if,
                                     GEN_DBAL_FIELD32, DBAL_FIELD_CREDIT_INIT_BIT, INST_SINGLE, (uint32) 1,
                                     GEN_DBAL_FIELD_LAST_MARK));
    /*
     * Clear the bit corresponding to indicated interface (back to default value of '0')
     */
    SHR_IF_ERR_EXIT(dnx_dbal_gen_set(unit, DBAL_TABLE_EGQ_INTERFACE_REG_ATTRIBUTES,
                                     2, 1,
                                     GEN_DBAL_FIELD32, DBAL_FIELD_CORE_ID, (uint32) core,
                                     GEN_DBAL_FIELD32, DBAL_FIELD_EGQ_IF, (uint32) egr_if,
                                     GEN_DBAL_FIELD32, DBAL_FIELD_CREDIT_INIT_BIT, INST_SINGLE, (uint32) 0,
                                     GEN_DBAL_FIELD_LAST_MARK));
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - gets the clock rate index, which is used as key to table if_speed_params.
 *            for some devices, the configuration is changed for different clock rates.
 * \param [in] unit -  Unit-ID
 * \param [out] index - key index to if_speed_params
 *
 * \return
 *   shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_egr_queuing_if_speed_params_clock_rate_index_get(
    int unit,
    int *index)
{
    int clk_idx;
    SHR_FUNC_INIT_VARS(unit);
    /** get the clock_rate_index */
    for (clk_idx = 0; clk_idx < dnx_data_egr_queuing.params.if_speed_params_clk_th_info_get(unit)->key_size[0];
         clk_idx++)
    {
        if (dnx_data_device.general.core_clock_khz_get(unit) <=
            dnx_data_egr_queuing.params.if_speed_params_clk_th_get(unit, clk_idx)->max_clock_rate_th)
        {
            break;
        }
    }
    if (clk_idx == dnx_data_egr_queuing.params.if_speed_params_clk_th_info_get(unit)->key_size[0])
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "No matching clock rate in table if_speed_params_clk_th. core_clock_khz=%d\n",
                     dnx_data_device.general.core_clock_khz_get(unit));
    }
    *index = clk_idx;
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - gets credit size & irdy values from dnx data based on interface speed
 *
 * \param [in] unit -  Unit-ID
 * \param [in] port -  port
 * \param [in] if_speed -  interface speed, Mbps units.
 * \param [out] credit_size -  credit size for port
 * \param [out] irdy_thr -  irdy threshold for port
 * \param [out] txq_max_bytes - max bytes threshold for txq transmit
 * \param [out] min_gap_hp - min gap for the consecutive access to HP queue
 * \param [out] fqp_min_gap - fqp min gap
 *
 * \return
 *   shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_egr_queuing_nif_info_get(
    int unit,
    bcm_port_t port,
    int if_speed,
    uint32 *credit_size,
    uint32 *irdy_thr,
    uint32 *txq_max_bytes,
    uint32 *min_gap_hp,
    uint32 *fqp_min_gap)
{
    uint32 idx;
    int clk_idx;
    int if_id;
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(dnx_algo_port_egr_if_get(unit, port, &if_id));

    SHR_IF_ERR_EXIT(dnx_egr_queuing_if_speed_params_clock_rate_index_get(unit, &clk_idx));
    /*
     * get TXI credit and NRDY values received from ASIC
     * NIF interfaces are searched at the table according speed. Special interfaces are searched according interface ID.
     */
    for (idx = 0; idx < dnx_data_egr_queuing.params.if_speed_params_info_get(unit)->key_size[0]; idx++)
    {
        if (dnx_data_egr_queuing.params.if_speed_params_get(unit, idx, clk_idx)->if_id == if_id)
        {
            break;
        }
        else
        {
            if (if_speed <= dnx_data_egr_queuing.params.if_speed_params_get(unit, idx, clk_idx)->speed)
            {
                break;
            }
        }
    }
    if (idx < dnx_data_egr_queuing.params.if_speed_params_info_get(unit)->key_size[0])
    {
        *credit_size = dnx_data_egr_queuing.params.if_speed_params_get(unit, idx, clk_idx)->crdt_size;
        *irdy_thr = dnx_data_egr_queuing.params.if_speed_params_get(unit, idx, clk_idx)->irdy_thr;
        *txq_max_bytes = dnx_data_egr_queuing.params.if_speed_params_get(unit, idx, clk_idx)->txq_max_bytes;
        *min_gap_hp = dnx_data_egr_queuing.params.if_speed_params_get(unit, idx, clk_idx)->min_gap_hp;
        *fqp_min_gap = dnx_data_egr_queuing.params.if_speed_params_get(unit, idx, clk_idx)->fqp_min_gap;
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "NIF parameters are not found %d\n", port);
    }
exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_egr_queuing_total_egr_if_credits_update(
    int unit,
    bcm_port_t logical_port,
    int speed)
{
    uint32 prev_credit_size = 0, new_credit_size = 0, irdy_th, txq_max_bytes, min_gap_hp, fqp_min_gap;
    int total_egr_if_credits;
    int prev_if_speed = 0;
    int has_speed;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_has_speed(unit, logical_port, DNX_ALGO_PORT_SPEED_F_TX, &has_speed));
    if (has_speed)
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_interface_rate_get(unit, logical_port, DNX_ALGO_PORT_SPEED_F_TX, &prev_if_speed));
        SHR_IF_ERR_EXIT(dnx_egr_queuing_nif_info_get
                        (unit, logical_port, prev_if_speed, &prev_credit_size, &irdy_th, &txq_max_bytes, &min_gap_hp,
                         &fqp_min_gap));
    }

    SHR_IF_ERR_EXIT(dnx_egr_db.total_egr_if_credits.get(unit, &total_egr_if_credits));

    total_egr_if_credits -= prev_credit_size;
    if (speed != 0)
    {
        SHR_IF_ERR_EXIT(dnx_egr_queuing_nif_info_get
                        (unit, logical_port, speed, &new_credit_size, &irdy_th, &txq_max_bytes, &min_gap_hp,
                         &fqp_min_gap));
    }
    total_egr_if_credits += new_credit_size;

    if (total_egr_if_credits > dnx_data_egr_queuing.params.txq_max_credits_get(unit))
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "txq total_egr_if_credits=%d, above maximum allowed: %d  \n",
                     total_egr_if_credits, dnx_data_egr_queuing.params.txq_max_credits_get(unit));
    }
    SHR_IF_ERR_EXIT(dnx_egr_db.total_egr_if_credits.set(unit, total_egr_if_credits));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - set credit size for special interfaces, which there credit size do not depend in the speed
 *
 * \param [in] unit -  Unit-ID
 * \param [in] port_info -  port type
 * \param [in] egr_if -  interface id
 * \param [in] port -  local port
 *
 * \return
 *   shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_egr_queuing_special_if_credit_set(
    int unit,
    dnx_algo_port_info_s port_info,
    int egr_if,
    bcm_port_t port)
{
    int idx, clk_idx;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_egr_queuing_if_speed_params_clock_rate_index_get(unit, &clk_idx));
    for (idx = 0; idx < dnx_data_egr_queuing.params.if_speed_params_info_get(unit)->key_size[0]; idx++)
    {
        if (dnx_data_egr_queuing.params.if_speed_params_get(unit, idx, clk_idx)->if_id == egr_if)
        {
            break;
        }
    }
    if (idx == dnx_data_egr_queuing.params.if_speed_params_info_get(unit)->key_size[0])
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "interface %d wasn't found \n", egr_if);
    }
    SHR_IF_ERR_EXIT(dnx_egr_queuing_nif_credit_set
                    (unit, port, dnx_data_egr_queuing.params.if_speed_params_get(unit, idx, clk_idx)->crdt_size));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See egr_queuing.h
 */
shr_error_e
dnx_egr_queuing_nif_credit_default_set(
    int unit,
    bcm_port_t port)
{
    uint32 credit_size, irdy_th, txq_max_bytes, min_gap_hp, fqp_min_gap;
    int if_speed;

    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(dnx_algo_port_interface_rate_get(unit, port, DNX_ALGO_PORT_SPEED_F_TX, &if_speed));
    SHR_IF_ERR_EXIT(dnx_egr_queuing_nif_info_get
                    (unit, port, if_speed, &credit_size, &irdy_th, &txq_max_bytes, &min_gap_hp, &fqp_min_gap));
    SHR_IF_ERR_EXIT(dnx_egr_queuing_nif_credit_set(unit, port, credit_size));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - handle FQP interface profile allocate single -
 *
 * \param [in] unit -  Unit-ID
 * \param [in] core -  core
 * \param [in] egr_if -  egress interface
 * \param [in] profile_params - profile parameter structure
 *
 * \return
 *   shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_egr_queuing_fqp_profile_alloc_single(
    int unit,
    int core,
    int egr_if,
    dnx_fqp_profile_params_t * profile_params)
{
    int profile_id;
    uint8 is_first;

    SHR_FUNC_INIT_VARS(unit);

    if (dnx_data_egr_queuing.params.feature_get(unit, dnx_data_egr_queuing_params_if_attributes_profile_exist) == 1)
    {
        SHR_IF_ERR_EXIT(dnx_am_fqp_profile_alloc_single(unit, core, &profile_id, profile_params, &is_first));
        if (is_first)
        {
            /** Create new profile */
            SHR_IF_ERR_EXIT(dnx_egq_dbal_fqp_profile_params_set(unit, core, profile_id, profile_params));
        }
        /** Map egr_if to new profile */
        SHR_IF_ERR_EXIT(dnx_egq_dbal_fqp_profile_id_set(unit, core, egr_if, profile_id));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - handle FQP interface profile settings -
 *  handle exchange, set new profile_id and set the parameters values to HW
 *
 * \param [in] unit -  Unit-ID
 * \param [in] core -  core
 * \param [in] egr_if -  egress interface
 * \param [in] field_type -  dbal field_type
 * \param [in] field_val - field value
 *
 * \return
 *   shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_egr_queuing_fqp_param_set(
    int unit,
    int core,
    int egr_if,
    uint32 field_type,
    int field_val)
{
    SHR_FUNC_INIT_VARS(unit);

    if (dnx_data_egr_queuing.params.feature_get(unit, dnx_data_egr_queuing_params_if_attributes_profile_exist) == 0)
    {
        SHR_IF_ERR_EXIT(dnx_dbal_gen_set(unit, DBAL_TABLE_EGQ_INTERFACE_ATTRIBUTES,
                                         2, 1,
                                         GEN_DBAL_FIELD32, DBAL_FIELD_CORE_ID, (uint32) core,
                                         GEN_DBAL_FIELD32, DBAL_FIELD_EGQ_IF, (uint32) egr_if,
                                         GEN_DBAL_FIELD32, field_type, INST_SINGLE, (uint32) field_val,
                                         GEN_DBAL_FIELD_LAST_MARK));
    }
    else
    {
        dnx_fqp_profile_params_t profile_params;
        int old_profile_id;
        uint8 is_last;
        SHR_IF_ERR_EXIT(dnx_egq_dbal_fqp_profile_id_get(unit, core, egr_if, &old_profile_id));
        SHR_IF_ERR_EXIT(dnx_egq_dbal_fqp_profile_params_get(unit, core, egr_if, old_profile_id, &profile_params));
        switch (field_type)
        {
            case DBAL_FIELD_TXQ_MAX_BYTES_TH:
                profile_params.txq_max_bytes = field_val;
                break;
            case DBAL_FIELD_TXI_IRDY_TH:
                profile_params.irdy_thr = field_val;
                break;
            case DBAL_FIELD_TXQ_TDM_IRDY_SEL:
                profile_params.txq_tdm_irdy = field_val;
                break;
            case DBAL_FIELD_IFC_PART_OF_SUB_CALENDAR:
                profile_params.is_belong_to_sub_calendar = field_val;
                break;
            case DBAL_FIELD_MIN_GAP:
                profile_params.min_gap = field_val;
                break;
            case DBAL_FIELD_ALLOW_CONS_SEL:
                profile_params.serve_consecutive_if = field_val;
                break;
            default:
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "invalid field_type %d\n", field_type);
        }
        SHR_IF_ERR_EXIT(dnx_am_fqp_profile_free_single(unit, core, old_profile_id, &is_last));
        SHR_IF_ERR_EXIT(dnx_egr_queuing_fqp_profile_alloc_single(unit, core, egr_if, &profile_params));
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See egr_queuing.h
 */
shr_error_e
dnx_egr_queuing_interface_alloc(
    int unit,
    bcm_port_t port,
    int master_logical_port,
    bcm_core_t core,
    bcm_port_if_t interface_type,
    int channelized_if,
    int *egr_if)
{
    int if_occupied;
    int ii;

    SHR_FUNC_INIT_VARS(unit);
    *egr_if = -1;

    if (master_logical_port == DNX_ALGO_PORT_INVALID)
    {
        /*
         * First port in interface
         */
        switch (interface_type)
        {
            case SOC_PORT_IF_CPU:
            {
                *egr_if = dnx_data_egr_queuing.params.egr_if_cpu_get(unit);
                break;
            }
            case SOC_PORT_IF_OLP:
            {
                *egr_if = dnx_data_egr_queuing.params.egr_if_olp_get(unit);
                break;
            }
            case SOC_PORT_IF_OAMP:
            {
                *egr_if = dnx_data_egr_queuing.params.egr_if_oam_get(unit);
                break;
            }
            case SOC_PORT_IF_RCY:
            {
                *egr_if = dnx_data_egr_queuing.params.egr_if_rcy_get(unit);
                break;
            }
            case SOC_PORT_IF_SAT:
            {
                *egr_if = dnx_data_egr_queuing.params.egr_if_sat_get(unit);
                break;
            }
            case SOC_PORT_IF_EVENTOR:
            {
                *egr_if = dnx_data_egr_queuing.params.egr_if_eventor_get(unit);
                break;
            }
            default:
            {
                /** for none channelized ifc, try to allocated outside the channelized pool */
                if (channelized_if == FALSE)
                {
                    for (ii = dnx_data_egr_queuing.params.nof_egr_ch_interfaces_get(unit);
                         ii < dnx_data_egr_queuing.params.nof_egr_interfaces_get(unit); ii++)
                    {
                        SHR_IF_ERR_EXIT(dnx_egr_db.interface_occ.get(unit, core, ii, &if_occupied));
                        if (if_occupied == FALSE)
                        {
                            *egr_if = ii;
                            SHR_IF_ERR_EXIT(dnx_egr_db.interface_occ.set(unit, core, *egr_if, TRUE));
                            break;
                        }
                    }
                }
                /** If no ifc was allocated yet, allocate it from the channelized pool */
                if (*egr_if == -1)
                {
                    for (ii = dnx_data_egr_queuing.params.egr_if_nif_base_get(unit);
                         ii < dnx_data_egr_queuing.params.nof_egr_ch_interfaces_get(unit); ii++)
                    {
                        SHR_IF_ERR_EXIT(dnx_egr_db.interface_occ.get(unit, core, ii, &if_occupied));
                        if (if_occupied == FALSE)
                        {
                            *egr_if = ii;
                            SHR_IF_ERR_EXIT(dnx_egr_db.interface_occ.set(unit, core, *egr_if, TRUE));
                            break;
                        }
                    }
                    if (ii >= dnx_data_egr_queuing.params.nof_egr_ch_interfaces_get(unit))
                    {
                        SHR_ERR_EXIT(_SHR_E_MEMORY, "no free egress interface to allocate for port %d\n", port);
                    }
                }
                break;
            }
        }
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_egr_if_get(unit, master_logical_port, egr_if));
    }
    if (*egr_if == -1)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL,
                     "Invalid egr_if is indicated for master_logical_port %d and interface type %d\n",
                     master_logical_port, interface_type);
    }
exit:
    SHR_FUNC_EXIT;
}

/*
 * See egr_queuing.h
 */
shr_error_e
dnx_egr_queuing_if_thr_set(
    int unit,
    bcm_port_t port)
{
    uint32 credit_size, irdy_thr, txq_max_bytes, min_gap_hp;
    int egr_if;
    bcm_core_t core;
    int if_speed;
    uint32 fqp_min_gap;

    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(dnx_algo_port_interface_rate_get(unit, port, DNX_ALGO_PORT_SPEED_F_TX, &if_speed));
    SHR_IF_ERR_EXIT(dnx_egr_queuing_nif_info_get
                    (unit, port, if_speed, &credit_size, &irdy_thr, &txq_max_bytes, &min_gap_hp, &fqp_min_gap));
    SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, port, &core));
    /*
     * We assume that 'core' may also be 'BCM_CORE_ALL' and that it may be passed, as is,
     * to DBAL (I.e., we assume that DBAL_CORE_ALL is the same as BCM_CORE_ALL).
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_egr_if_get(unit, port, &egr_if));
    SHR_IF_ERR_EXIT(dnx_egr_queuing_fqp_param_set(unit, core, egr_if, DBAL_FIELD_TXI_IRDY_TH, irdy_thr));
    SHR_IF_ERR_EXIT(dnx_egr_queuing_fqp_param_set(unit, core, egr_if, DBAL_FIELD_TXQ_MAX_BYTES_TH, txq_max_bytes));
    SHR_IF_ERR_EXIT(dnx_egr_queuing_fqp_param_set(unit, core, egr_if, DBAL_FIELD_MIN_GAP, fqp_min_gap));
    /** if fqp_min_gap == 0, need to allow servce interface consecutively */
    SHR_IF_ERR_EXIT(dnx_egr_queuing_fqp_param_set(unit, core, egr_if, DBAL_FIELD_ALLOW_CONS_SEL, (fqp_min_gap == 0)));

    SHR_IF_ERR_EXIT(dnx_dbal_gen_set(unit, DBAL_TABLE_EGQ_INTERFACE_ATTRIBUTES,
                                     2, 1,
                                     GEN_DBAL_FIELD32, DBAL_FIELD_CORE_ID, (uint32) core,
                                     GEN_DBAL_FIELD32, DBAL_FIELD_EGQ_IF, (uint32) egr_if,
                                     GEN_DBAL_FIELD32, DBAL_FIELD_MIN_GAP_HP, INST_SINGLE, (uint32) min_gap_hp,
                                     GEN_DBAL_FIELD_LAST_MARK));
exit:
    SHR_FUNC_EXIT;
}

/*
 * See egr_queuing.h
 */
shr_error_e
dnx_egr_queuing_if_sub_calendar_set(
    int unit,
    bcm_port_t port)
{
    int egr_if;
    bcm_core_t core;
    int sub_calendar_valid;
    int if_rate_mbps;
    SHR_FUNC_INIT_VARS(unit);

    if (dnx_data_egr_queuing.params.feature_get(unit, dnx_data_egr_queuing_params_sub_calendar) == TRUE)
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, port, &core));
        SHR_IF_ERR_EXIT(dnx_algo_port_egr_if_get(unit, port, &egr_if));
        SHR_IF_ERR_EXIT(dnx_algo_port_interface_rate_get(unit, port, DNX_ALGO_PORT_SPEED_F_TX, &if_rate_mbps));
        sub_calendar_valid =
            (if_rate_mbps <= dnx_data_egr_queuing.common_max_val.slow_port_speed_get(unit)) ? TRUE : FALSE;
        /** update EPS block */
        SHR_IF_ERR_EXIT(dnx_dbal_gen_set(unit, DBAL_TABLE_EGQ_INTERFACE_ATTRIBUTES, 2, 1,
                                         GEN_DBAL_FIELD32, DBAL_FIELD_CORE_ID, (uint32) core,
                                         GEN_DBAL_FIELD32, DBAL_FIELD_EGQ_IF, (uint32) egr_if,
                                         GEN_DBAL_FIELD32, DBAL_FIELD_IFC_PART_OF_SUB_CALENDAR, INST_SINGLE,
                                         sub_calendar_valid, GEN_DBAL_FIELD_LAST_MARK));
        /** update FQP block */
        SHR_IF_ERR_EXIT(dnx_egr_queuing_fqp_param_set
                        (unit, core, egr_if, DBAL_FIELD_IFC_PART_OF_SUB_CALENDAR, sub_calendar_valid));
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_egr_queuing_if_sub_calendar_get(
    int unit,
    int if_rate_mbps,
    int *egr_if_belong_to_sub_calendar)
{
    SHR_FUNC_INIT_VARS(unit);
    *egr_if_belong_to_sub_calendar = FALSE;
    if (dnx_data_egr_queuing.params.feature_get(unit, dnx_data_egr_queuing_params_sub_calendar) == TRUE)
    {
        if (if_rate_mbps <= dnx_data_egr_queuing.common_max_val.slow_port_speed_get(unit))
        {
            *egr_if_belong_to_sub_calendar = TRUE;
        }
    }

/*exit:*/
    SHR_FUNC_EXIT;
}

/**
 * \brief - set an entry value into the calendar
 *
 * \param [in] unit -  Unit-ID
 * \param [in] core_id -  core_id
 * \param [in] cal_type -  FQP or PQP
 * \param [in] cal_sel -  calendar A or B
 * \param [in] slot_id -  calendar slot to update
 * \param [in] cal_entry -  entry value
 *
 * \return
 *   shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_egr_queuing_nif_cal_entry_set(
    int unit,
    int core_id,
    dnx_egr_cal_type_e cal_type,
    uint32 cal_sel,
    int slot_id,
    dnx_ofp_sch_rates_cal_entry_t cal_entry)
{

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    if (cal_type == CAL_TYPE_PQP)
    {
        SHR_IF_ERR_EXIT(dnx_dbal_gen_set(unit, DBAL_TABLE_EGQ_PQP_NIF_CAL,
                                         3, 1,
                                         GEN_DBAL_FIELD32, DBAL_FIELD_CORE_ID, core_id,
                                         GEN_DBAL_FIELD32, DBAL_FIELD_SET_SELECT, cal_sel,
                                         GEN_DBAL_FIELD32, DBAL_FIELD_SLOT_ID, (uint32) (slot_id),
                                         GEN_DBAL_FIELD32, DBAL_FIELD_PQP_NIF_PORT_MUX, INST_SINGLE, cal_entry,
                                         GEN_DBAL_FIELD_LAST_MARK));
    }
    else
    {
        uint32 entry_handle_id;
        /** Taking a handle */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EGQ_FQP_NIF_CAL, &entry_handle_id));
        /** Setting key fields */
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_SLOT_ID, (uint32) (slot_id));
        if (dnx_data_egr_queuing.params.feature_get(unit, dnx_data_egr_queuing_params_fqp_calender_set_select) == 1)
        {
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_SET_SELECT, cal_sel);
        }
        /** Setting value fields */
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FQP_NIF_PORT_MUX, INST_SINGLE, cal_entry);

        /** Preforming the action */
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - switch between two copies of calendars
 *
 * \param [in] unit -  Unit-ID
 * \param [in] core_id -  core_id
 * \param [in] next_cal -  new calender selected: 0 or 1
 *
 * \return
 *   shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_egr_queuing_nif_cal_switch_set(
    int unit,
    int core_id,
    uint32 next_cal)
{
    SHR_FUNC_INIT_VARS(unit);

    /** switch calendar sequence may be different between pqp and fqp if fqp_calender_set_select= FALSE */
    if (dnx_data_egr_queuing.params.feature_get(unit, dnx_data_egr_queuing_params_fqp_calender_set_select))
    {
        SHR_IF_ERR_EXIT(dnx_dbal_gen_set(unit, DBAL_TABLE_EGQ_SHAPER_GLOBAL_CONFIGURATION,
                                         1, 2,
                                         GEN_DBAL_FIELD32, DBAL_FIELD_CORE_ID, core_id,
                                         GEN_DBAL_FIELD32, DBAL_FIELD_FQP_CALENDAR_SET_SEL, INST_SINGLE, next_cal,
                                         GEN_DBAL_FIELD32, DBAL_FIELD_PQP_CALENDAR_SET_SEL, INST_SINGLE, next_cal,
                                         GEN_DBAL_FIELD_LAST_MARK));
    }
    else
    {
        /** handle pqp calendar set  */
        SHR_IF_ERR_EXIT(dnx_dbal_gen_set(unit, DBAL_TABLE_EGQ_SHAPER_GLOBAL_CONFIGURATION,
                                         1, 1,
                                         GEN_DBAL_FIELD32, DBAL_FIELD_CORE_ID, core_id,
                                         GEN_DBAL_FIELD32, DBAL_FIELD_PQP_CALENDAR_SET_SEL, INST_SINGLE, next_cal,
                                         GEN_DBAL_FIELD_LAST_MARK));

        /** handle Fqp switch - toggle bit from 0 > 1 */
        SHR_IF_ERR_EXIT(dnx_dbal_gen_set(unit, DBAL_TABLE_EGQ_SHAPER_GLOBAL_CONFIGURATION, 1, 1,
                                         GEN_DBAL_FIELD32, DBAL_FIELD_CORE_ID, core_id,
                                         GEN_DBAL_FIELD32, DBAL_FIELD_FQP_CALENDAR_SWITCH_EN, INST_SINGLE,
                                         (uint32) (0), GEN_DBAL_FIELD_LAST_MARK));
        SHR_IF_ERR_EXIT(dnx_dbal_gen_set(unit, DBAL_TABLE_EGQ_SHAPER_GLOBAL_CONFIGURATION, 1, 1,
                                         GEN_DBAL_FIELD32, DBAL_FIELD_CORE_ID, core_id,
                                         GEN_DBAL_FIELD32, DBAL_FIELD_FQP_CALENDAR_SWITCH_EN, INST_SINGLE,
                                         (uint32) (1), GEN_DBAL_FIELD_LAST_MARK));
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See egr_queuing.h
 */
shr_error_e
dnx_egr_queuing_nif_calendar_set(
    int unit,
    bcm_core_t core)
{
    bcm_port_t port;
    int egr_if;
    int if_rate_mbps, rcy_fqp_weight;
    bcm_pbmp_t port_bm;
    uint32 cal_len_optimal, nof_cal_objects;
    int ii;
    uint32 current_cal, next_cal;
    uint32 weights[BCM_PBMP_PORT_MAX], lcm_temp[BCM_PBMP_PORT_MAX];
    dnx_ofp_rates_cal_sch_t *calendar;
    uint32 dummy_start;
    dnx_egr_cal_type_e cal_type;
    dnx_algo_port_info_s interface_type;
    int interface_offset, is_master_port;
    int has_speed;
    int core_id;
    int egr_if_belong_to_sub_calendar, max_sub_calendar_slow_port_rate, nof_slow_ports_in_sub_calendar;
    int nif_cal_len;
    SHR_FUNC_INIT_VARS(unit);

    calendar = NULL;

    dummy_start = 0;
    SHR_ALLOC(calendar, sizeof(*calendar), "calendar egr_q_nif_cal", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);
    DNXCMN_CORES_ITER(unit, core, core_id)
    {
        max_sub_calendar_slow_port_rate = 0;
        nof_slow_ports_in_sub_calendar = 0;
        for (cal_type = 0; cal_type < NUM_DNX_EGR_CAL_TYPE; cal_type++)
        {
            sal_memset(lcm_temp, 0, sizeof(lcm_temp));
            sal_memset(weights, 0, sizeof(weights));
            rcy_fqp_weight = 0;

            if (cal_type == CAL_TYPE_PQP)
            {
                nif_cal_len = dnx_data_egr_queuing.params.nif_cal_len_pqp_get(unit);
            }
            else
            {
                nif_cal_len = dnx_data_egr_queuing.params.nif_cal_len_fqp_get(unit);
            }
            /**
             * Get all Master logical ports for all cores. Will be used to itterate and calculate the
             * per port weights.
             */
            SHR_IF_ERR_EXIT(dnx_algo_port_logicals_get(unit, core_id, DNX_ALGO_PORT_LOGICALS_TYPE_TM_EGR_QUEUING,
                                                       DNX_ALGO_PORT_LOGICALS_F_MASTER_ONLY, &port_bm));
            BCM_PBMP_ITER(port_bm, port)
            {
                /**
                 * Get interface rate, only interfaces that have speed must be added to the calendar.
                 */
                SHR_IF_ERR_EXIT(dnx_algo_port_has_speed(unit, port, DNX_ALGO_PORT_SPEED_F_TX, &has_speed));
                if (has_speed == 0)
                {
                    if_rate_mbps = 0;
                }
                else
                {
                    SHR_IF_ERR_EXIT(dnx_algo_port_interface_rate_get
                                    (unit, port, DNX_ALGO_PORT_SPEED_F_TX, &if_rate_mbps));
                }

                /**
                 * Weight is calculated per interface.
                 */
                SHR_IF_ERR_EXIT(dnx_algo_port_is_master_get(unit, port, &is_master_port));
                if (!is_master_port)
                {
                    continue;
                }

                /**
                 * Get configuration parameters for the current port.
                 */
                SHR_IF_ERR_EXIT(dnx_algo_port_egr_if_get(unit, port, &egr_if));
                SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, port, &interface_type));
                SHR_IF_ERR_EXIT(dnx_algo_port_interface_offset_get(unit, port, &interface_offset));

                /**
                 * FQP recycle interface is special in a way that it has 2 egress interfaces.
                 * for the the second interface the egress interface id should be replaced
                 */
                if (DNX_ALGO_PORT_TYPE_IS_RCY(unit, interface_type) && (cal_type == CAL_TYPE_FQP) && interface_offset)
                {
                    egr_if = dnx_data_egr_queuing.params.egr_if_txi_rcy_get(unit);
                }

                SHR_IF_ERR_EXIT(dnx_egr_queuing_if_sub_calendar_get
                                (unit, if_rate_mbps, &egr_if_belong_to_sub_calendar));
                if (egr_if_belong_to_sub_calendar == TRUE)
                {
                    /**
                     * Ports under a certain port speed are grouped together and are represented all as one in the MUX calendar
                     * with total rate of max_rate*nof_if
                     */
                    max_sub_calendar_slow_port_rate = UTILEX_MAX(max_sub_calendar_slow_port_rate, if_rate_mbps);
                    nof_slow_ports_in_sub_calendar++;
                }
                else
                {
                    /**
                     * Calculate weight according to the interface rate
                     */
                    SHR_IF_ERR_EXIT(dnx_egr_queuing_nif_rate_to_weight(unit, if_rate_mbps, &weights[egr_if]));
                }

                /**
                 * PQP has only 1 recycle interface. Therefore, the weight of the interface is determined by the sum of both interfaces speeds.
                 */
                if (DNX_ALGO_PORT_TYPE_IS_RCY(unit, interface_type) && (cal_type == CAL_TYPE_PQP))
                {
                    weights[egr_if] += rcy_fqp_weight;
                    rcy_fqp_weight = weights[egr_if];
                }
            }

            /**
             * Handle the sub_calendar - Calculate the weight of the dedicated queue for Slow Ports
             */
            if (dnx_data_egr_queuing.params.feature_get(unit, dnx_data_egr_queuing_params_sub_calendar) == TRUE)
            {
                SHR_IF_ERR_EXIT(dnx_egr_queuing_nif_rate_to_weight
                                (unit, (max_sub_calendar_slow_port_rate * nof_slow_ports_in_sub_calendar),
                                 &weights[dnx_data_egr_queuing.params.sub_calendar_ifc_get(unit)]));
            }

            /**
             * Calculate the calendar parameters
             */
            SHR_IF_ERR_EXIT(dnx_egr_queuing_cal_params_calc
                            (unit, weights, nif_cal_len, &cal_len_optimal, &nof_cal_objects, &dummy_start));

            /**
             * Given the optimal calendar length and the
             * corresponding weight (in slots) of each port,
             * build a calendar that will avoid burstiness
             * behavior as much as possible.
             */
            SHR_IF_ERR_EXIT(dnx_algo_cal_simple_fixed_len_cal_build(unit, weights, nof_cal_objects, cal_len_optimal,
                                                                    nif_cal_len, calendar->slots));

            /**
             * Get the current active calendar and determine what the next actove calendar will be
             */
            SHR_IF_ERR_EXIT(dnx_dbal_gen_get(unit, DBAL_TABLE_EGQ_SHAPER_GLOBAL_CONFIGURATION,
                                             1, 1,
                                             GEN_DBAL_FIELD32, DBAL_FIELD_CORE_ID, core_id,
                                             GEN_DBAL_FIELD32, DBAL_FIELD_PQP_CALENDAR_SET_SEL, INST_SINGLE,
                                             &current_cal, GEN_DBAL_FIELD_LAST_MARK));
            next_cal = current_cal ? 0 : 1;

            /**
             * Replace dummies and the rest of calendar with invalid IFs and write the values to HW
             */
            for (ii = 0; ii < nif_cal_len; ii++)
            {
                if (ii >= cal_len_optimal || calendar->slots[ii] >= dummy_start)
                {
                    calendar->slots[ii] =
                        utilex_power_of_2(dnx_data_egr_queuing.params.nof_bits_in_egr_interface_get(unit)) - 1;
                }
                /*
                 * Set the value of FQP_NIF_PORT_MUX/PQP_NIF_PORT_MUX
                 */
                SHR_IF_ERR_EXIT(dnx_egr_queuing_nif_cal_entry_set
                                (unit, core_id, cal_type, next_cal, ii, calendar->slots[ii]));
            }
        }

        /**
         * Switch active calendar A<->B
         */
        SHR_IF_ERR_EXIT(dnx_egr_queuing_nif_cal_switch_set(unit, core_id, next_cal));

    }
exit:
    SHR_FREE(calendar);
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 *   Enable/disable port qpair flow control (related to specified logical port).
 * \param [in] unit -
 *   Int. Identifier of HW platform.
 * \param [in] logical_port -
 *   bcm_port_t. Logical port identifying the specific base qpair on which rquired
 *   operations are to be done.
 * \param [in] enable -
 *   uint32. Flag. If TRUE then enable all qpairs shapers. Else, disable.
 *   (Affected qpairs are 'base_q_pair and corresponding 'priority' q_pairs).
 * \return
 *    \retval Zero - On success
 *    \retval Error - Identifier as per shr_error_e
 * \remark
 *   * Specific qpair is controlled via HW memory
 *     EPS_QP_CFG_2,EPS_QP_CFG (field FORCE_OR_IGNORE_FC)
 * \see
 *   * dnx_egr_queuing_port_enable_and_flush_set
 *   * dnx_algo_port_base_q_pair_get
 */
static shr_error_e
dnx_egr_queuing_port_fc_enable_set(
    int unit,
    bcm_port_t logical_port,
    uint32 enable)
{
    int base_q_pair, nof_priorities, cosq;
    bcm_core_t core;

    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(dnx_algo_port_base_q_pair_get(unit, logical_port, &base_q_pair));
    SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, logical_port, &core));
    SHR_IF_ERR_EXIT(dnx_algo_port_priorities_nof_get(unit, logical_port, &nof_priorities));
    /*
     * Flow control mapping CMIC to EGQ
     */
    for (cosq = 0; cosq < nof_priorities; cosq++)
    {
        /*
         * set related qpairs
         */
        SHR_IF_ERR_EXIT(dnx_dbal_gen_set(unit, DBAL_TABLE_EGQ_QPAIR_ATTRIBUTES,
                                         2, 1,
                                         GEN_DBAL_FIELD32, DBAL_FIELD_QPAIR, (uint32) (base_q_pair + cosq),
                                         GEN_DBAL_FIELD32, DBAL_FIELD_CORE_ID, (uint32) core,
                                         GEN_DBAL_FIELD32, DBAL_FIELD_FORCE_OR_IGNORE_FC, INST_SINGLE,
                                         (uint32) (enable ? 1 : 0), GEN_DBAL_FIELD_LAST_MARK));
    }
exit:
    SHR_FUNC_EXIT;
}
/*
 * \brief -
 *   This procedure is a helper. It is called, within the sequence of adding
 *   a port, from dnx_egr_queuing_port_add_handle(), to carry out all EGQ
 *   initializations, related to the qpairs corresponding to this newly
 *   added port.
 *   (This is, then, a step in bcm_dnx_port_add())
 * \param [in] unit -
 *   HW identifier of unit.
 * \return
 *   If zero (_SHR_E_NONE), then no error was encountered.
 *   Otherwise, see shr_error_e
 * \remark
 *   * ps priority mode configuration
 *   * port qpair attributes - interface, TDM indication, priority (TDM to high and the rest to low)
 *   * port tc/dp mapping per port
 *   * port channel configuration
 *   * Tcg shaping packet mode
 * \see
 *   * dnx_egr_queuing_port_add_handle
 *   * port_add_remove_sequence (step "EgqQueuing")
 *   * bcm_dnx_port_add
 *   * bcm_port_add API
 *   * port_add_remove_sequence[]
 */
static shr_error_e
dnx_egr_queuing_port_add_qpair_handle(
    int unit,
    bcm_port_t logical_port)
{
    dnx_algo_port_info_s port_info;
    dbal_tables_e dbal_table_id;
    int base_q_pair, num_priorities;
    int egr_if;
    uint32 tm_port;
    bcm_gport_t gport;
    int channelized, core, channel;
    uint32 is_tdm;
    uint32 queue_priority;
    dnx_algo_port_tdm_mode_e tdm_mode;
    uint32 cosq;
    int calendar_id;
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(dnx_algo_port_base_q_pair_get(unit, logical_port, &base_q_pair));
    SHR_IF_ERR_EXIT(dnx_algo_port_tm_port_get(unit, logical_port, &core, &tm_port));
    SHR_IF_ERR_EXIT(dnx_algo_port_priorities_nof_get(unit, logical_port, &num_priorities));
    SHR_IF_ERR_EXIT(dnx_algo_port_is_channelized_get(unit, logical_port, &channelized));
    SHR_IF_ERR_EXIT(dnx_algo_port_channel_get(unit, logical_port, &channel));
    /*
     * get egress interface
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_egr_if_get(unit, logical_port, &egr_if));
    SHR_IF_ERR_EXIT(dnx_egr_queuing_calendar_index_get(unit, logical_port, &calendar_id));
    SHR_IF_ERR_EXIT(dnx_algo_port_tdm_get(unit, logical_port, &tdm_mode));
    is_tdm = (tdm_mode == DNX_ALGO_PORT_TDM_MODE_NONE) ? FALSE : TRUE;
    SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, logical_port, &port_info));
    /*
     * map tm port to base queue pair and setting CGM interface
     */
    SHR_IF_ERR_EXIT(dnx_dbal_gen_set(unit, DBAL_TABLE_EGQ_TM_PORT_ATTRIBUTES,
                                     2, 2,
                                     GEN_DBAL_FIELD32, DBAL_FIELD_TM_PORT, (uint32) (tm_port),
                                     GEN_DBAL_FIELD32, DBAL_FIELD_CORE_ID, (uint32) core,
                                     GEN_DBAL_FIELD32, DBAL_FIELD_QPAIR_BASE, INST_SINGLE, (uint32) (base_q_pair),
                                     GEN_DBAL_FIELD32, DBAL_FIELD_CGM_INTERFACE, INST_SINGLE, (uint32) (egr_if),
                                     GEN_DBAL_FIELD_LAST_MARK));
    /*
     * ps priority mode configuration
     */
    SHR_IF_ERR_EXIT(dnx_egr_queuing_port_prio_mode_set(unit, core, logical_port, num_priorities));
    /*
     * OTM port attributes - shaper mode, shaper index
     */
    if (dnx_data_egr_queuing.params.feature_get(unit, dnx_data_egr_queuing_params_almost_empty_delay) == 1)
    {
        SHR_IF_ERR_EXIT(dnx_dbal_gen_set(unit, DBAL_TABLE_EGQ_SHAPING_OTM_ATTRIBUTES, 2, 1,
                                 /** Keys */
                                         GEN_DBAL_FIELD32, DBAL_FIELD_QPAIR, (uint32) (base_q_pair),
                                         GEN_DBAL_FIELD32, DBAL_FIELD_CORE_ID, (uint32) core,
                                 /** fields */
                                         GEN_DBAL_FIELD32, DBAL_FIELD_PORT_ALMOST_EMPTY_DELAY, INST_SINGLE,
                                         (uint32) (TRUE), GEN_DBAL_FIELD_LAST_MARK));

    }
    SHR_IF_ERR_EXIT(dnx_dbal_gen_set(unit, DBAL_TABLE_EGQ_SHAPING_OTM_ATTRIBUTES, 2, 1,
                                     /*
                                      * Keys
                                      */
                                     GEN_DBAL_FIELD32, DBAL_FIELD_QPAIR, (uint32) (base_q_pair),
                                     GEN_DBAL_FIELD32, DBAL_FIELD_CORE_ID, (uint32) core,
                                     /*
                                      * fields
                                      */
                                     GEN_DBAL_FIELD32, DBAL_FIELD_OTM_CAL_SPR_INDX, INST_SINGLE, calendar_id,
                                     GEN_DBAL_FIELD_LAST_MARK));
    /*
     * Change table id.
     */
    dbal_table_id = DBAL_TABLE_EGQ_QPAIR_ATTRIBUTES;
    /*
     * port qpair attributes - interface, TDM indication, priority (TDM to high and the rest to low)
     */

    queue_priority = is_tdm ? BCM_COSQ_SP0 : BCM_COSQ_SP1;

    BCM_GPORT_LOCAL_SET(gport, logical_port);
    SHR_IF_ERR_EXIT(bcm_dnx_cosq_gport_sched_set(unit, gport, 0, queue_priority, -1));

    for (cosq = 0; cosq < num_priorities; cosq++)
    {
        if (dnx_data_tdm.params.feature_get(unit, dnx_data_tdm_params_is_supported) == TRUE)
        {
            SHR_IF_ERR_EXIT(dnx_dbal_gen_set(unit, dbal_table_id,
                                             2, 1,
                                             GEN_DBAL_FIELD32, DBAL_FIELD_QPAIR, (uint32) (base_q_pair + cosq),
                                             GEN_DBAL_FIELD32, DBAL_FIELD_CORE_ID, (uint32) core,
                                             GEN_DBAL_FIELD32, DBAL_FIELD_QP_IS_TDM, INST_SINGLE, (uint32) (is_tdm),
                                             GEN_DBAL_FIELD_LAST_MARK));
        }
        SHR_IF_ERR_EXIT(dnx_dbal_gen_set(unit, dbal_table_id, 2, 1,
                                         /** Keys. */
                                         GEN_DBAL_FIELD32, DBAL_FIELD_QPAIR, (uint32) (base_q_pair + cosq),
                                         GEN_DBAL_FIELD32, DBAL_FIELD_CORE_ID, (uint32) core,
                                         /** Fields. */
                                         GEN_DBAL_FIELD32, DBAL_FIELD_MAP_QP_TO_IFC, INST_SINGLE, (uint32) (egr_if),
                                         GEN_DBAL_FIELD_LAST_MARK));

        if (dnx_data_egr_queuing.params.feature_get(unit, dnx_data_egr_queuing_params_almost_empty_delay) == 1)
        {
            SHR_IF_ERR_EXIT(dnx_dbal_gen_set(unit, dbal_table_id, 2, 1,
                                             /** Keys. */
                                             GEN_DBAL_FIELD32, DBAL_FIELD_QPAIR, (uint32) (base_q_pair + cosq),
                                             GEN_DBAL_FIELD32, DBAL_FIELD_CORE_ID, (uint32) core,
                                             /** Fields. */
                                             GEN_DBAL_FIELD32, DBAL_FIELD_Q_ALMOST_EMPTY_DELAY, INST_SINGLE,
                                             (uint32) (DNX_EGR_Q_ALMOST_EMPTY_DELAY), GEN_DBAL_FIELD_LAST_MARK));
        }

        /*
         * Setting default qpair scheduling. UC has SP over MC
         */
        BCM_COSQ_GPORT_UCAST_EGRESS_QUEUE_SET(gport, logical_port);
        SHR_IF_ERR_EXIT(bcm_dnx_cosq_gport_sched_set(unit, gport, cosq, BCM_COSQ_SP0, 0));
    }
    /*
     * port tc/dp mapping per port
     */
    SHR_IF_ERR_EXIT(dnx_egr_queuing_default_tc_dp_map_set(unit, logical_port));
    /*
     * port channel configuration
     * Special case for CPU interface : Channel number is actually CPU_COS which should always be EGQ Qpair# mod 64
     */
    /*
     * Change table id.
     */
    dbal_table_id = DBAL_TABLE_EGQ_QPAIR_ATTRIBUTES;
    for (cosq = 0; cosq < num_priorities; cosq++)
    {
        if (DNX_ALGO_PORT_TYPE_IS_CPU(unit, port_info))
        {
            SHR_IF_ERR_EXIT(dnx_dbal_gen_set(unit, dbal_table_id,
                                             2, 2,
                                             GEN_DBAL_FIELD32, DBAL_FIELD_QPAIR, (uint32) (base_q_pair + cosq),
                                             GEN_DBAL_FIELD32, DBAL_FIELD_CORE_ID, (uint32) core,
                                             GEN_DBAL_FIELD32, DBAL_FIELD_CPU_COS, INST_SINGLE,
                                             (uint32) (channel + cosq), GEN_DBAL_FIELD32, DBAL_FIELD_SRC_PORT,
                                             INST_SINGLE, (uint32) (tm_port), GEN_DBAL_FIELD_LAST_MARK));
            /*
             *  Set the flow control mapping CMIC to EGQ
             */
            SHR_IF_ERR_EXIT(dnx_egq_dbal_cmic_egq_map_set(unit, core, (channel + cosq), (base_q_pair + cosq)));
        }
        else
        {
            SHR_IF_ERR_EXIT(dnx_dbal_gen_set(unit, dbal_table_id,
                                             2, 1,
                                             GEN_DBAL_FIELD32, DBAL_FIELD_QPAIR, (uint32) (base_q_pair + cosq),
                                             GEN_DBAL_FIELD32, DBAL_FIELD_CORE_ID, (uint32) core,
                                             GEN_DBAL_FIELD32, DBAL_FIELD_CHANNEL, INST_SINGLE, (uint32) (channel),
                                             GEN_DBAL_FIELD_LAST_MARK));
        }
    }
    /*
     * Set interface shaping for non-channelized ports to maximum. The shaping is determined by port.
     */
    if (!channelized)
    {
        SHR_IF_ERR_EXIT(dnx_ofp_rates_if_calendar_internal_rate_set
                        (unit, core, calendar_id, dnx_data_egr_queuing.params.max_credit_number_get(unit)));
    }
exit:
    SHR_FUNC_EXIT;
}

/*
 * \brief -
 *   This procedure is a helper. It is called, within the sequence of adding
 *   a port, from dnx_egr_queuing_port_add_handle(), to carry out all EGQ
 *   initializations, related to the interface corresponding to this newly
 *   added port.
 *   (This is, then, a step in bcm_dnx_port_add())
 * \param [in] unit -
 *   HW identifier of unit.
 * \return
 *   If zero (_SHR_E_NONE), then no error was encountered.
 *   Otherwise, see shr_error_e
 * \remark
 *   * Per interface configuration most of the configuration relevant only
 *     for master port
 *   * configure PS/q_pair , channelized, TDM indication
 *   * For 'master' port, use dnx_data_egr_queuing.params.initial_packet_mode
 *     to get initial packet_mode for containing interface. Otherwise, use
 *     the value written into HW (IFC_SPR_PKT_MODE)
 * \see
 *   * dnx_egr_queuing_port_add_handle
 *   * port_add_remove_sequence (step "EgqQueuing")
 *   * bcm_dnx_port_add
 *   * bcm_port_add API
 *   * port_add_remove_sequence[]
 */
static shr_error_e
dnx_egr_queuing_port_add_interface_handle(
    int unit,
    bcm_port_t logical_port)
{
    bcm_port_t master_port;
    dbal_tables_e dbal_table_id;
    int channelized, core, compensation;
    uint32 ps, tm_port;
    uint32 is_tdm_if;
    dnx_algo_port_if_tdm_mode_e if_tdm_mode;
    int packet_mode;
    int egr_if;
    uint32 dbal_field_val;
    dnx_algo_port_info_s port_info;
    int egr_interleave;
    int base_q_pair, num_priorities;

    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(dnx_algo_port_master_get(unit, logical_port, 0, &master_port));
    SHR_IF_ERR_EXIT(dnx_algo_port_base_q_pair_get(unit, logical_port, &base_q_pair));
    SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, logical_port, &port_info));
    SHR_IF_ERR_EXIT(dnx_algo_port_tm_port_get(unit, logical_port, &core, &tm_port));
    SHR_IF_ERR_EXIT(dnx_algo_port_priorities_nof_get(unit, logical_port, &num_priorities));
    SHR_IF_ERR_EXIT(dnx_algo_port_is_channelized_get(unit, logical_port, &channelized));
    SHR_IF_ERR_EXIT(dnx_algo_port_is_egress_interleave_get(unit, logical_port, &egr_interleave));

    /** get egress interface */
    SHR_IF_ERR_EXIT(dnx_algo_port_egr_if_get(unit, logical_port, &egr_if));
    SHR_IF_ERR_EXIT(dnx_algo_port_if_tdm_mode_get(unit, logical_port, &if_tdm_mode));
    is_tdm_if = (if_tdm_mode == DNX_ALGO_PORT_IF_NO_TDM) ? FALSE : TRUE;

    dbal_table_id = DBAL_TABLE_EGQ_INTERFACE_ATTRIBUTES;
    if (logical_port == master_port)
    {
        /** allocate first default FQP profile for new interface */
        dnx_fqp_profile_params_t profile_params;
        sal_memset(&profile_params, 0x0, sizeof(dnx_fqp_profile_params_t));
        SHR_IF_ERR_EXIT(dnx_egr_queuing_fqp_profile_alloc_single(unit, core, egr_if, &profile_params));

        SHR_IF_ERR_EXIT(dnx_dbal_gen_set(unit, dbal_table_id,
                                         2, 2,
                                         GEN_DBAL_FIELD32, DBAL_FIELD_EGQ_IF, (uint32) (egr_if),
                                         GEN_DBAL_FIELD32, DBAL_FIELD_CORE_ID, (uint32) core,
                                         GEN_DBAL_FIELD32, DBAL_FIELD_TDM_HP_SHARE_TXQ, INST_SINGLE,
                                         (uint32) (!is_tdm_if), GEN_DBAL_FIELD32, DBAL_FIELD_IFC_IS_ACTIVE, INST_SINGLE,
                                         (uint32) (1), GEN_DBAL_FIELD_LAST_MARK));

        /*
         * write the channelized interface information only if the interface can be used for channelized interface
         * For master port, start by marking ALL PSs as not mapped to specified interface.
         */
        if (egr_if < dnx_data_egr_queuing.params.nof_egr_ch_interfaces_get(unit))
        {
            {
                dbal_table_id = DBAL_TABLE_EGQ_CHANNELIZED_INTERFACE_ATTRIBUTES;
                SHR_IF_ERR_EXIT(dnx_dbal_gen_set(unit, dbal_table_id,
                                                 2, 2,
                                                 GEN_DBAL_FIELD32, DBAL_FIELD_EGQ_CH_IF, (uint32) (egr_if),
                                                 GEN_DBAL_FIELD32, DBAL_FIELD_CORE_ID, (uint32) core,
                                                 GEN_DBAL_FIELD32, DBAL_FIELD_IFC_IS_CHANNELIZED, INST_SINGLE,
                                                 (uint32) (channelized ? 1 : 0), GEN_DBAL_FIELD32,
                                                 DBAL_FIELD_MAP_PS_TO_IFC, INST_ALL, 0, GEN_DBAL_FIELD_LAST_MARK));
            }
        }
        /*
         * Shaper mode configuration per calendar. Below, it is set for containing interface
         * and all contained ports.
         * For 'master port', use the defaut as stored in 'data'.
         */
        packet_mode = dnx_data_egr_queuing.params.initial_packet_mode_get(unit);

        /** Map egr interface to nif */
        if (DNX_ALGO_PORT_TYPE_IS_NIF(unit, port_info, FALSE, FALSE, FALSE, FALSE)
            && (!dnx_data_esb.general.feature_get(unit, dnx_data_esb_general_esb_support)))
        {
            SHR_IF_ERR_EXIT(dnx_egq_interface_to_nif_port_map_set(unit, logical_port));
        }
#ifdef ADAPTER_SERVER_MODE
        /** adapter specific case - indicate the first lane when ILKN data ports exists, since adapter can't get this info */
        if ((SAL_BOOT_PLISIM) && (dnx_data_nif.ilkn.feature_get(unit, dnx_data_nif_ilkn_is_ilu_supported)) &&
            (DNX_ALGO_PORT_TYPE_IS_NIF(unit, port_info, FALSE, FALSE, FALSE, FALSE)))
        {
            SHR_IF_ERR_EXIT(adapter_egq_interface_port_map_set(unit, logical_port, egr_if));
        }
#endif
        /*
         * configure txq max thr, tdm indication and TDM interleave mode which valid only for ILKN
         * Use new table: DBAL_TABLE_EGQ_INTERFACE_ATTRIBUTES
         */
        dbal_table_id = DBAL_TABLE_EGQ_INTERFACE_ATTRIBUTES;
        SHR_IF_ERR_EXIT(dnx_dbal_gen_set(unit, dbal_table_id,
                                         2, 1,
                                         GEN_DBAL_FIELD32, DBAL_FIELD_EGQ_IF, (uint32) (egr_if),
                                         GEN_DBAL_FIELD32, DBAL_FIELD_CORE_ID, (uint32) core,
                                         GEN_DBAL_FIELD32, DBAL_FIELD_INTRLV_NOT_PKT_BOUND, INST_SINGLE,
                                         (uint32) egr_interleave, GEN_DBAL_FIELD_LAST_MARK));
        /*
         * configures the selection of Almost Full minimum gap when All Qs under an
         * interface are AE but the interface is not.
         * All values below refer to DBAL_FIELD_IFC_AE_SEL.
         */
        if (channelized)
        {
            dbal_field_val = DNX_EGR_IFC_MIN_GAP_CHANNELIZED;
        }
        else if (num_priorities < 4)
        {
            dbal_field_val = DNX_EGR_IFC_MIN_GAP_1_2;
        }
        else
        {
            dbal_field_val = DNX_EGR_IFC_MIN_GAP_4_8;
        }
        SHR_IF_ERR_EXIT(dnx_dbal_gen_set(unit, dbal_table_id,
                                         2, 1,
                                         GEN_DBAL_FIELD32, DBAL_FIELD_EGQ_IF, (uint32) (egr_if),
                                         GEN_DBAL_FIELD32, DBAL_FIELD_CORE_ID, (uint32) core,
                                         GEN_DBAL_FIELD32, DBAL_FIELD_IFC_AE_SEL, INST_SINGLE,
                                         (uint32) (dbal_field_val), GEN_DBAL_FIELD_LAST_MARK));

        if (dnx_data_egr_queuing.params.feature_get(unit, dnx_data_egr_queuing_params_txq_tdm_irdy_sel_exist) == 1)
        {
            SHR_IF_ERR_EXIT(dnx_egr_queuing_fqp_param_set(unit, core, egr_if, DBAL_FIELD_TXQ_TDM_IRDY_SEL, is_tdm_if));
        }

        /** This table needs to configured for both recycle egress interfaces exactly the same */
        if (DNX_ALGO_PORT_TYPE_IS_RCY(unit, port_info))
        {
            if (dnx_data_egr_queuing.params.feature_get(unit, dnx_data_egr_queuing_params_txq_tdm_irdy_sel_exist) == 1)
            {
                SHR_IF_ERR_EXIT(dnx_egr_queuing_fqp_param_set(unit, core,
                                                              dnx_data_egr_queuing.params.egr_if_txi_rcy_get(unit),
                                                              DBAL_FIELD_TXQ_TDM_IRDY_SEL, is_tdm_if));
            }

        }
    }
    else
    {
        dnx_algo_port_info_s port_info;
        int has_rx_speed = 0;
        int has_tx_speed = 0;

        /** get port info */
        SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, master_port, &port_info));

        /*
         * Shaper mode configuration per calendar. Below, it is set for containing interface
         * and all contained ports.
         * For non-'master port', use the value currently in HW (for containing interface).
         */
        SHR_IF_ERR_EXIT(dnx_egr_queuing_interface_packet_mode_get(unit, logical_port, &packet_mode));

        /** configure port enable and speed for slave ports according to master port */
        SHR_IF_ERR_EXIT(dnx_algo_port_has_speed(unit, master_port, DNX_ALGO_PORT_SPEED_F_RX, &has_rx_speed));
        SHR_IF_ERR_EXIT(dnx_algo_port_has_speed(unit, master_port, DNX_ALGO_PORT_SPEED_F_TX, &has_tx_speed));
        /** only if master port has speed */
        if (has_rx_speed || has_tx_speed)
        {
            bcm_port_resource_t master_port_resource;
            int tx_speed;
            int rx_speed;

            /** get master port "port resource" */
            SHR_IF_ERR_EXIT(bcm_dnx_port_resource_get(unit, master_port, &master_port_resource));

            /** change slave speed configuration */
            /** set Tx speed in mgmt db */
            tx_speed = master_port_resource.speed;
            SHR_IF_ERR_EXIT(dnx_algo_port_speed_set(unit, logical_port, DNX_ALGO_PORT_SPEED_F_TX, tx_speed));

            /** get RX speed */
            if (master_port_resource.flags & BCM_PORT_RESOURCE_ASYMMETRICAL)
            {
                rx_speed = master_port_resource.rx_speed;
            }
            else
            {
                rx_speed = tx_speed;
            }
            /** set Rx speed in mgmt db */
            SHR_IF_ERR_EXIT(dnx_algo_port_speed_set(unit, logical_port, DNX_ALGO_PORT_SPEED_F_RX, rx_speed));
        }

        /** only for NIF ports */
        if (DNX_ALGO_PORT_TYPE_IS_NIF(unit, port_info, TRUE, TRUE, TRUE, TRUE))
        {
            uint32 port_enable = 0;
            /** set master port enable state to slave ports */
            SHR_IF_ERR_EXIT(dnx_egr_queuing_port_enable_get(unit, master_port, &port_enable));
            SHR_IF_ERR_EXIT(dnx_egr_queuing_port_enable_and_flush_set(unit, logical_port, port_enable, FALSE));
        }
    }

    /** Set shaper mode configuration for indicated logical port. */
    SHR_IF_ERR_EXIT(dnx_egr_queuing_interface_packet_mode_set(unit, logical_port, packet_mode));

    if (egr_if < dnx_data_egr_queuing.params.nof_egr_ch_interfaces_get(unit) && channelized)
    {
        ps = base_q_pair / DNX_EGR_NOF_Q_PAIRS_IN_PS;
        dbal_field_val = 1;

        /** Set array element at index 'ps'*/
        dbal_table_id = DBAL_TABLE_EGQ_CHANNELIZED_INTERFACE_ATTRIBUTES;
        SHR_IF_ERR_EXIT(dnx_dbal_gen_set(unit, dbal_table_id,
                                         2, 1,
                                         GEN_DBAL_FIELD32, DBAL_FIELD_EGQ_CH_IF, (uint32) (egr_if),
                                         GEN_DBAL_FIELD32, DBAL_FIELD_CORE_ID, (uint32) core,
                                         GEN_DBAL_FIELD32, DBAL_FIELD_MAP_PS_TO_IFC, ps, dbal_field_val,
                                         GEN_DBAL_FIELD_LAST_MARK));
    }
    else
    {
        /** Set the value: DBAL_FIELD_NON_CH_IFC_BASE_QUEUE_PAIR */
        SHR_IF_ERR_EXIT(dnx_dbal_gen_set(unit, dbal_table_id,
                                         2, 1,
                                         GEN_DBAL_FIELD32, DBAL_FIELD_EGQ_IF, (uint32) (egr_if),
                                         GEN_DBAL_FIELD32, DBAL_FIELD_CORE_ID, (uint32) core,
                                         GEN_DBAL_FIELD32, DBAL_FIELD_NON_CH_IFC_BASE_QUEUE_PAIR, INST_SINGLE,
                                         (uint32) (base_q_pair), GEN_DBAL_FIELD_LAST_MARK));
    }

    /** map recycle port to recycle interface (only for recycle ports). */
    if (DNX_ALGO_PORT_TYPE_IS_RCY(unit, port_info))
    {
        SHR_IF_ERR_EXIT(dnx_egr_port_recycle_interface_map_set(unit, logical_port));
    }

    if (channelized)
    {
        /** set per-qpair compensation to be the same as the other channels on the interface */
        SHR_IF_ERR_EXIT(dnx_egr_port_compensation_get(unit, master_port, &compensation));
        SHR_IF_ERR_EXIT(dnx_egr_port_compensation_set(unit, logical_port, compensation));
    }

    /** update credit size for special interfaces */
    /** Nif interfaces is being updated only when speed is configured */
    if ((DNX_ALGO_PORT_TYPE_IS_TM(unit, port_info)) &&
        (!DNX_ALGO_PORT_TYPE_IS_NIF(unit, port_info, TRUE, TRUE, TRUE, TRUE)) &&
        (!DNX_ALGO_PORT_TYPE_IS_FLEXE(unit, port_info, TRUE, TRUE)))
    {
        SHR_IF_ERR_EXIT(dnx_egr_queuing_special_if_credit_set(unit, port_info, egr_if, logical_port));
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See egr_queuing.h file
 */
shr_error_e
dnx_egr_queuing_port_packet_mode_set(
    int unit,
    bcm_port_t logical_port,
    int arg)
{
    int core;
    int base_q_pair, num_priorities;
    int prio_index;
    uint32 packet_mode;

    SHR_FUNC_INIT_VARS(unit);
    packet_mode = ((arg != 0) ? 1 : 0);
    SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, logical_port, &core));
    SHR_IF_ERR_EXIT(dnx_algo_port_base_q_pair_get(unit, logical_port, &base_q_pair));
    SHR_IF_ERR_EXIT(dnx_algo_port_priorities_nof_get(unit, logical_port, &num_priorities));
    /*
     * OTM port attributes - shaper mode, shaper index
     */
    SHR_IF_ERR_EXIT(dnx_dbal_gen_set(unit, DBAL_TABLE_EGQ_SHAPING_OTM_ATTRIBUTES, 2, 1,
                                     /*
                                      * Keys
                                      */
                                     GEN_DBAL_FIELD32, DBAL_FIELD_QPAIR, (uint32) (base_q_pair),
                                     GEN_DBAL_FIELD32, DBAL_FIELD_CORE_ID, (uint32) core,
                                     /*
                                      * fields
                                      */
                                     GEN_DBAL_FIELD32, DBAL_FIELD_OTM_SPR_PKT_MODE, INST_SINGLE,
                                     (uint32) (packet_mode), GEN_DBAL_FIELD_LAST_MARK));

    for (prio_index = 0; prio_index < num_priorities; prio_index++)
    {
        /**
         * Set Q-pair shaping to packet mode.
         */
        SHR_IF_ERR_EXIT(dnx_dbal_gen_set(unit, DBAL_TABLE_EGQ_QPAIR_ATTRIBUTES, 2, 1,
                                         /*
                                          * Keys
                                          */
                                         GEN_DBAL_FIELD32, DBAL_FIELD_QPAIR,
                                         (uint32) (base_q_pair + prio_index), GEN_DBAL_FIELD32,
                                         DBAL_FIELD_CORE_ID, (uint32) core,
                                         /*
                                          * Fields
                                          */
                                         GEN_DBAL_FIELD32, DBAL_FIELD_QP_SPR_PKT_MODE, INST_SINGLE,
                                         (uint32) (packet_mode), GEN_DBAL_FIELD_LAST_MARK));

        /**
         * Set TCG shaping to packet mode.
         */
        SHR_IF_ERR_EXIT(dnx_dbal_gen_set(unit, DBAL_TABLE_EGQ_TCG_ATTRIBUTES, 2, 1,
                                         /*
                                          * Keys
                                          */
                                         GEN_DBAL_FIELD32, DBAL_FIELD_TCG_INDEX,
                                         (uint32) (base_q_pair + prio_index), GEN_DBAL_FIELD32,
                                         DBAL_FIELD_CORE_ID, (uint32) core,
                                         /*
                                          * Fields
                                          */
                                         GEN_DBAL_FIELD32, DBAL_FIELD_TCG_SPR_PKT_MODE, INST_SINGLE,
                                         (uint32) (packet_mode), GEN_DBAL_FIELD_LAST_MARK));
    }
exit:
    SHR_FUNC_EXIT;
}
/*
 * See egr_queuing.h file
 */
shr_error_e
dnx_egr_queuing_port_packet_mode_get(
    int unit,
    bcm_port_t logical_port,
    int *arg)
{
    int core;
    int base_q_pair;
    uint32 packet_mode = 0;

    SHR_FUNC_INIT_VARS(unit);
    /*
     * Just good practice.
     */
    *arg = 0;
    SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, logical_port, &core));
    SHR_IF_ERR_EXIT(dnx_algo_port_base_q_pair_get(unit, logical_port, &base_q_pair));
    /**
     * Get value from OTM port attributes HW
     */
    SHR_IF_ERR_EXIT(dnx_dbal_gen_get(unit, DBAL_TABLE_EGQ_SHAPING_OTM_ATTRIBUTES, 2, 1,
                                     /*
                                      * Keys
                                      */
                                     GEN_DBAL_FIELD32, DBAL_FIELD_QPAIR, (uint32) (base_q_pair),
                                     GEN_DBAL_FIELD32, DBAL_FIELD_CORE_ID, (uint32) core,
                                     /*
                                      * fields
                                      */
                                     GEN_DBAL_FIELD32, DBAL_FIELD_OTM_SPR_PKT_MODE, INST_SINGLE,
                                     &packet_mode, GEN_DBAL_FIELD_LAST_MARK));
    *arg = packet_mode;
exit:
    SHR_FUNC_EXIT;
}

/*
 * See egr_queuing.h file
 */
shr_error_e
dnx_egr_queuing_interface_packet_mode_set(
    int unit,
    bcm_port_t logical_port,
    int arg)
{
    int core;
    bcm_port_t logical_port_i;
    int egr_if;
    dbal_tables_e dbal_table_id;
    uint32 packet_mode;
    int calendar_id;
    SHR_FUNC_INIT_VARS(unit);
    packet_mode = ((arg != 0) ? 1 : 0);
    SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, logical_port, &core));
    SHR_IF_ERR_EXIT(dnx_algo_port_egr_if_get(unit, logical_port, &egr_if));
    SHR_IF_ERR_EXIT(dnx_egr_queuing_calendar_index_get(unit, logical_port, &calendar_id));
    {
        /*
         * Set HW shaper for interface (shaper mode configuration per calendar)
         */
        dbal_table_id = DBAL_TABLE_EGQ_SHAPER_GLOBAL_CONFIGURATION;
        /*
         * Set the value
         */
        SHR_IF_ERR_EXIT(dnx_dbal_gen_set(unit, dbal_table_id, 1, 1,
                                         /*
                                          * Keys
                                          */
                                         GEN_DBAL_FIELD32, DBAL_FIELD_CORE_ID, (uint32) core,
                                         /*
                                          * Fields
                                          */
                                         GEN_DBAL_FIELD32, DBAL_FIELD_IFC_SPR_PKT_MODE, calendar_id, packet_mode,
                                         GEN_DBAL_FIELD_LAST_MARK));
    }
    /*
     * Go through all logical ports related to specified interface and
     * set 'packet mode' for them (same as done on interface, above).
     */
    logical_port_i = DNX_ALGO_PORT_INVALID;
    do
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_next_logical_on_if_get(unit, core, egr_if, &logical_port_i));
        if (logical_port_i != DNX_ALGO_PORT_INVALID)
        {
            SHR_IF_ERR_EXIT(dnx_egr_queuing_port_packet_mode_set(unit, logical_port_i, packet_mode));
        }
        else
        {
            break;
        }
    }
    while (TRUE);

exit:
    SHR_FUNC_EXIT;
}
/*
 * See egr_queuing.h file
 */
shr_error_e
dnx_egr_queuing_interface_packet_mode_get(
    int unit,
    bcm_port_t logical_port,
    int *arg)
{
    int core;
    dbal_tables_e dbal_table_id;
    uint32 packet_mode;
    int calendar_id;
    SHR_FUNC_INIT_VARS(unit);

    /** Just good practice. */
    *arg = 0;
    SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, logical_port, &core));
    SHR_IF_ERR_EXIT(dnx_egr_queuing_calendar_index_get(unit, logical_port, &calendar_id));
    {
        /** Get HW shaper for interface (shaper mode configuration per calendar) */
        dbal_table_id = DBAL_TABLE_EGQ_SHAPER_GLOBAL_CONFIGURATION;

        /** Set the value */
        SHR_IF_ERR_EXIT(dnx_dbal_gen_get(unit, dbal_table_id, 1, 1,
                                         /** Keys */
                                         GEN_DBAL_FIELD32, DBAL_FIELD_CORE_ID, (uint32) core,
                                         /** Fields */
                                         GEN_DBAL_FIELD32, DBAL_FIELD_IFC_SPR_PKT_MODE, calendar_id, &packet_mode,
                                         GEN_DBAL_FIELD_LAST_MARK));
    }
    *arg = packet_mode;
exit:
    SHR_FUNC_EXIT;
}

/*
 * See egr_queuing.h file
 */
shr_error_e
dnx_egr_queuing_port_add_handle(
    int unit)
{
    bcm_port_t logical_port;
    uint32 tm_port;
    int core;
    dnx_algo_port_info_s port_info;

    SHR_FUNC_INIT_VARS(unit);
    /*
     * get logical port information
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_added_port_get(unit, &logical_port));
    SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, logical_port, &port_info));
    /**
     * Omit non egress ports
     */
    if (!DNX_ALGO_PORT_TYPE_IS_EGR_TM(unit, port_info))
    {
        SHR_EXIT();
    }
    SHR_IF_ERR_EXIT(dnx_algo_port_tm_port_get(unit, logical_port, &core, &tm_port));
    /*
     * set mapping between pp_dsp to tm_port, mapping should be 1 to 1
     */
    SHR_IF_ERR_EXIT(dnx_port_pp_dsp_to_tm_mapping_set(unit, core, tm_port, tm_port));
    /*
     * Enable Flow Control
     */
    SHR_IF_ERR_EXIT(dnx_egr_queuing_port_fc_enable_set(unit, logical_port, TRUE));
    /*
     * Per interface configuration most of the configuration relevant only for master port
     *
     * Configure PS/q_pair , channelized, TDM indication
     */
    SHR_IF_ERR_EXIT(dnx_egr_queuing_port_add_interface_handle(unit, logical_port));
    /*
     * Other qpair-related initializations.
     */
    SHR_IF_ERR_EXIT(dnx_egr_queuing_port_add_qpair_handle(unit, logical_port));

    /*
     * Egress Shared Buffers alloc per port
     */
    if (dnx_data_esb.general.feature_get(unit, dnx_data_esb_general_esb_support))
    {
        SHR_IF_ERR_EXIT(dnx_esb_port_add(unit, logical_port));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - egr interface release per port
 *
 * \param [in] unit -  Unit-ID
 * \param [in] port -  port
 *
 * \return
 *   shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_egr_queuing_interface_free(
    int unit,
    bcm_port_t port)
{
    int egr_if;
    int core;
    SHR_FUNC_INIT_VARS(unit);

    if (dnx_algo_port_valid_verify(unit, port) == _SHR_E_NONE)
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_egr_if_get(unit, port, &egr_if));
        SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, port, &core));
        SHR_IF_ERR_EXIT(dnx_egr_db.interface_occ.set(unit, core, egr_if, FALSE));
        SHR_IF_ERR_EXIT(dnx_algo_port_egr_if_unset(unit, port));
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See egr_queuing.h file
 */
shr_error_e
dnx_egr_queuing_port_remove_handle(
    int unit)
{
    bcm_port_t port;
    uint32 tm_port, pp_port;
    int is_last, is_allocated, core;
    int old_profile, new_profile;
    int base_q_pair, num_priorities;
    int egr_if;
    int channelized;
    int free_ps;
    uint32 ps;
    bcm_cos_queue_t cosq;
    bcm_gport_t gport;
    bcm_port_t next_master_port;
    dnx_algo_port_info_s port_info;
    dnx_cosq_egress_queue_mapping_info_t *mapping_info = NULL;
    dbal_tables_e dbal_table_id;
    uint32 dbal_field_val;
    SHR_FUNC_INIT_VARS(unit);
    /*
     * omit non egress ports
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_removed_port_get(unit, &port));
    SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, port, &port_info));
    if (!DNX_ALGO_PORT_TYPE_IS_EGR_TM(unit, port_info))
    {
        SHR_EXIT();
    }
    /*
     * get logical port information
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_base_q_pair_get(unit, port, &base_q_pair));
    SHR_IF_ERR_EXIT(dnx_algo_port_tm_port_get(unit, port, &core, &tm_port));
    SHR_IF_ERR_EXIT(dnx_algo_port_pp_port_get(unit, port, &core, &pp_port));
    SHR_IF_ERR_EXIT(dnx_algo_port_priorities_nof_get(unit, port, &num_priorities));
    SHR_IF_ERR_EXIT(dnx_algo_port_is_channelized_get(unit, port, &channelized));
    SHR_IF_ERR_EXIT(dnx_algo_port_egr_if_get(unit, port, &egr_if));
    SHR_IF_ERR_EXIT(dnx_algo_port_master_get(unit, port, DNX_ALGO_PORT_MASTER_F_NEXT, &next_master_port));
    /*
     * update pqp and fqp mux
     */
    /*
     * SHR_IF_ERR_EXIT(MBCM_DNX_SOC_DRIVER_CALL(unit, mbcm_dnx_egr_q_nif_cal_set, (unit, core)));
     */
    SHR_IF_ERR_EXIT(dnx_egr_queuing_nif_calendar_set(unit, core));
    /*
     *  Exchange tc/dp template manager to default
     */
    SHR_ALLOC(mapping_info, sizeof(*mapping_info),
              "egress queue mapping info handle memory", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);
    if (mapping_info == NULL)
    {
        SHR_ERR_EXIT(_SHR_E_MEMORY, "unit %d, failed to allocate egress queue mapping info handle memory\n", unit);
    }

    sal_memset(mapping_info, 0x0, sizeof(*mapping_info));

    SHR_IF_ERR_EXIT(dnx_am_template_egress_queue_mapping_exchange(unit, pp_port, core,
                                                                  mapping_info, &old_profile,
                                                                  &is_last, &new_profile, &is_allocated));
    /*
     * Map port to default profile
     */
    /*
     * SHR_IF_ERR_EXIT(MBCM_DNX_SOC_DRIVER_CALL(unit, mbcm_dnx_egr_q_profile_map_set, (unit, core, tm_port, new_profile)));
     */
    SHR_IF_ERR_EXIT(dnx_egr_queuing_profile_map_set(unit, core, pp_port, new_profile));
    /*
     * release base_q_pair
     */
    SHR_IF_ERR_EXIT(dnx_ps_db_base_qpair_free(unit, port, base_q_pair, &free_ps));
    if (free_ps)
    {
        /*
         * configure the PS which belongs to the interface and the the active indication
         */
        ps = base_q_pair / DNX_EGR_NOF_Q_PAIRS_IN_PS;
        if (egr_if < dnx_data_egr_queuing.params.nof_egr_ch_interfaces_get(unit))
        {
            /*
             * Clear array element at index 'ps'
             */
            dbal_table_id = DBAL_TABLE_EGQ_CHANNELIZED_INTERFACE_ATTRIBUTES;
            dbal_field_val = 0;
            SHR_IF_ERR_EXIT(dnx_dbal_gen_set(unit, dbal_table_id,
                                             2, 1,
                                             GEN_DBAL_FIELD32, DBAL_FIELD_EGQ_CH_IF, (uint32) egr_if,
                                             GEN_DBAL_FIELD32, DBAL_FIELD_CORE_ID, (uint32) core,
                                             GEN_DBAL_FIELD32, DBAL_FIELD_MAP_PS_TO_IFC, ps, dbal_field_val,
                                             GEN_DBAL_FIELD_LAST_MARK));
        }
    }

    /*
     * Egress Shared Buffers free per port
     */
    if (dnx_data_esb.general.feature_get(unit, dnx_data_esb_general_esb_support))
    {
        SHR_IF_ERR_EXIT(dnx_esb_port_remove(unit, port));
    }

    /*
     * release egress interface only if this is the last port
     */
    if (next_master_port == DNX_ALGO_PORT_INVALID)
    {
        int fqp_profile_id;
        uint8 is_last;
        dbal_table_id = DBAL_TABLE_EGQ_INTERFACE_ATTRIBUTES;
        SHR_IF_ERR_EXIT(dnx_dbal_gen_set(unit, dbal_table_id,
                                         2, 1,
                                         GEN_DBAL_FIELD32, DBAL_FIELD_EGQ_IF, (uint32) egr_if,
                                         GEN_DBAL_FIELD32, DBAL_FIELD_CORE_ID, (uint32) core,
                                         GEN_DBAL_FIELD32, DBAL_FIELD_IFC_IS_ACTIVE, INST_SINGLE, (uint32) (0),
                                         GEN_DBAL_FIELD_LAST_MARK));

        if (dnx_data_egr_queuing.params.feature_get(unit, dnx_data_egr_queuing_params_if_attributes_profile_exist) == 1)
        {
            SHR_IF_ERR_EXIT(dnx_egq_dbal_fqp_profile_id_get(unit, core, egr_if, &fqp_profile_id));
            SHR_IF_ERR_EXIT(dnx_am_fqp_profile_free_single(unit, core, fqp_profile_id, &is_last));
        }
        SHR_IF_ERR_EXIT(dnx_egr_queuing_total_egr_if_credits_update(unit, port, 0));
        SHR_IF_ERR_EXIT(dnx_egr_queuing_interface_free(unit, port));
    }
    /*
     * clear entry in pp_dsp mapping table
     */
    SHR_IF_ERR_EXIT(dnx_port_pp_dsp_table_mapping_clear(unit, core, tm_port));

    /*
     * For ports with priorities > tcg_min_priorities restore the
     * TC <-> TCG mapping configuration to default
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_priorities_nof_get(unit, port, &num_priorities));
    if (num_priorities >= dnx_data_egr_queuing.params.tcg_min_priorities_get(unit))
    {
        for (cosq = 0; cosq < num_priorities; cosq++)
        {
            BCM_COSQ_GPORT_PORT_TC_SET(gport, port);
            SHR_IF_ERR_EXIT(bcm_dnx_cosq_gport_sched_set(unit, gport, cosq, cosq, -1));
        }
    }

exit:
    SHR_FREE(mapping_info);
    SHR_FUNC_EXIT;
}
/*
 * See egr_queuing.h file
 */
shr_error_e
dnx_egr_queuing_prio_over_cast_set(
    int unit,
    bcm_port_t logical_port,
    int enable)
{
    bcm_core_t core = 0;
    int base_q_pair;

    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(dnx_algo_port_base_q_pair_get(unit, logical_port, &base_q_pair));
    SHR_IF_ERR_EXIT(dnx_dbal_gen_set(unit, DBAL_TABLE_EGQ_SHAPING_OTM_ATTRIBUTES,
                                     2, 1,
                                     GEN_DBAL_FIELD32, DBAL_FIELD_QPAIR, (uint32) base_q_pair,
                                     GEN_DBAL_FIELD32, DBAL_FIELD_CORE_ID, (uint32) core,
                                     GEN_DBAL_FIELD32, DBAL_FIELD_SP_OVER_WFQ_2P_PORT, INST_SINGLE, (uint32) (enable),
                                     GEN_DBAL_FIELD_LAST_MARK));
exit:
    SHR_FUNC_EXIT;
}
/*
 * See egr_queuing.h file
 */
shr_error_e
dnx_egr_queuing_prio_over_cast_get(
    int unit,
    bcm_port_t logical_port,
    int *enable_p)
{
    bcm_core_t core = 0;
    int base_q_pair;

    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(dnx_algo_port_base_q_pair_get(unit, logical_port, &base_q_pair));
    SHR_IF_ERR_EXIT(dnx_dbal_gen_get(unit, DBAL_TABLE_EGQ_SHAPING_OTM_ATTRIBUTES,
                                     2, 1,
                                     GEN_DBAL_FIELD32, DBAL_FIELD_QPAIR, (uint32) base_q_pair,
                                     GEN_DBAL_FIELD32, DBAL_FIELD_CORE_ID, (uint32) core,
                                     GEN_DBAL_FIELD32, DBAL_FIELD_SP_OVER_WFQ_2P_PORT, INST_SINGLE,
                                     (uint32 *) (enable_p), GEN_DBAL_FIELD_LAST_MARK));
exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief -
 *   Enable/disable qpair shapers, related to specified logical port.
 * \param [in] unit -
 *   Int. Identifier of HW platform.
 * \param [in] logical_port -
 *   bcm_port_t. Logical port identifying the specific base qpair on which rquired
 *   operations are to be done.
 * \param [in] enable -
 *   uint32. Flag. If TRUE then enable all qpairs shapers. Else, disable.
 *   (Affected qpairs are 'base_q_pair and corresponding 'priority' q_pairs).
 * \return
 *    \retval Zero - On success
 *    \retval Error - Identifier as per shr_error_e
 * \remark
 *   * Operation is carried out only if general flag, in HW, indicating
 *     whether qpair shapers are enabled, is asserted.
 *   * Specific qpair is controlled via HW memory
 *     EPS_QP_CFG_2,EPS_QP_CFG (field QPAIR_SPR_DIS)
 * \see
 *   * dnx_egr_queuing_port_all_shapers_enable_set
 *   * dnx_algo_port_base_q_pair_get
 */
static shr_error_e
dnx_egr_queuing_port_q_pair_shaper_enable_set(
    int unit,
    bcm_port_t logical_port,
    uint32 enable)
{
    int core, base_q_pair, nof_priorities, cosq;
    uint32 q_pair_shp_en_f;
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(dnx_algo_port_base_q_pair_get(unit, logical_port, &base_q_pair));
    SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, logical_port, &core));
    SHR_IF_ERR_EXIT(dnx_algo_port_priorities_nof_get(unit, logical_port, &nof_priorities));
    {
        SHR_IF_ERR_EXIT(dnx_dbal_gen_get(unit, DBAL_TABLE_EGQ_SHAPER_GLOBAL_CONFIGURATION,
                                         1, 1,
                                         GEN_DBAL_FIELD32, DBAL_FIELD_CORE_ID, (uint32) core,
                                         GEN_DBAL_FIELD32, DBAL_FIELD_QPAIR_SPR_ENA, INST_SINGLE,
                                         (uint32 *) (&q_pair_shp_en_f), GEN_DBAL_FIELD_LAST_MARK));
        if (q_pair_shp_en_f)
        {
            for (cosq = 0; cosq < nof_priorities; cosq++)
            {
                SHR_IF_ERR_EXIT(dnx_dbal_gen_set(unit, DBAL_TABLE_EGQ_QPAIR_ATTRIBUTES,
                                                 2, 1,
                                                 GEN_DBAL_FIELD32, DBAL_FIELD_QPAIR, (uint32) (base_q_pair + cosq),
                                                 GEN_DBAL_FIELD32, DBAL_FIELD_CORE_ID, (uint32) core,
                                                 GEN_DBAL_FIELD32, DBAL_FIELD_QPAIR_SPR_DIS, INST_SINGLE,
                                                 (uint32) (!enable), GEN_DBAL_FIELD_LAST_MARK));
            }
        }
    }
exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief -
 *   Enable/disable port shaper, related to specified logical port.
 * \param [in] unit -
 *   Int. Identifier of HW platform.
 * \param [in] logical_port -
 *   bcm_port_t. Logical port identifying the specific base qpair on which rquired
 *   operation is to be done.
 * \param [in] enable -
 *   uint32. Flag. If TRUE then enable port-qpair shaper. Else, disable.
 *   (Affected qpair is 'base_q_pair).
 * \return
 *    \retval Zero - On success
 *    \retval Error - Identifier as per shr_error_e
 * \remark
 *   * Operation is carried out only if general flag, in HW, indicating
 *     whether port shapers are enabled, is asserted.
 *   * Specific qpairs are controlled via HW memory
 *     EPS_OTM_CFG_2,EPS_OTM_CFG (field OTM_SPR_DIS)
 * \see
 *   * dnx_egr_queuing_port_all_shapers_enable_set
 *   * dnx_algo_port_base_q_pair_get
 */
static shr_error_e
dnx_egr_queuing_port_otm_shaper_enable_set(
    int unit,
    bcm_port_t logical_port,
    uint32 enable)
{
    int base_q_pair;
    uint32 otm_shaper_en;
    bcm_core_t core;
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(dnx_algo_port_base_q_pair_get(unit, logical_port, &base_q_pair));
    SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, logical_port, &core));
    {
        SHR_IF_ERR_EXIT(dnx_dbal_gen_get(unit, DBAL_TABLE_EGQ_SHAPER_GLOBAL_CONFIGURATION,
                                         1, 1,
                                         GEN_DBAL_FIELD32, DBAL_FIELD_CORE_ID, (uint32) core,
                                         GEN_DBAL_FIELD32, DBAL_FIELD_OTM_SPR_ENA, INST_SINGLE,
                                         (uint32 *) (&otm_shaper_en), GEN_DBAL_FIELD_LAST_MARK));
        if (otm_shaper_en)
        {
            SHR_IF_ERR_EXIT(dnx_dbal_gen_set(unit, DBAL_TABLE_EGQ_SHAPING_OTM_ATTRIBUTES,
                                             2, 1,
                                             GEN_DBAL_FIELD32, DBAL_FIELD_QPAIR, (uint32) (base_q_pair),
                                             GEN_DBAL_FIELD32, DBAL_FIELD_CORE_ID, (uint32) core,
                                             GEN_DBAL_FIELD32, DBAL_FIELD_OTM_SPR_DIS, INST_SINGLE,
                                             (uint32) (!enable), GEN_DBAL_FIELD_LAST_MARK));
        }
    }
exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief -
 *   Enable/disable interface shaper correlated with specified port.
 * \param [in] unit -
 *   Int. Identifier of HW platform.
 * \param [in] logical_port -
 *   bcm_port_t. Logical port identifying the specific base qpair on which rquired
 *   operations are to be done.
 * \param [in] enable -
 *   uint32. Flag. If TRUE then enable interface shaper. Else, disable.
 * \return
 *    \retval Zero - On success
 *    \retval Error - Identifier as per shr_error_e
 * \remark
 *   * Operation is carried out only if general flag, indicating whether
 *     all inreface shapers are enabled (in HW), is asserted.
 *   * Specific interface is controlled via HW register
 *     EPS_EGRESS_CALENDAR_SHAPER_SETTINGS (field IFC_SPR_DIS)
 * \see
 *   * dnx_egr_queuing_port_all_shapers_enable_set
 *   * dnx_algo_port_egr_if_get
 */
shr_error_e
dnx_egr_queuing_interface_shaper_enable_set(
    int unit,
    bcm_port_t logical_port,
    uint32 enable)
{
    uint32 if_shaper_en;
    bcm_core_t core;
    int calendar_id;
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, logical_port, &core));
    SHR_IF_ERR_EXIT(dnx_egr_queuing_calendar_index_get(unit, logical_port, &calendar_id));
    {
        SHR_IF_ERR_EXIT(dnx_dbal_gen_get(unit, DBAL_TABLE_EGQ_SHAPER_GLOBAL_CONFIGURATION,
                                         1, 1,
                                         GEN_DBAL_FIELD32, DBAL_FIELD_CORE_ID, (uint32) core,
                                         GEN_DBAL_FIELD32, DBAL_FIELD_INTERFACE_SPR_ENA, INST_SINGLE,
                                         (uint32 *) (&if_shaper_en), GEN_DBAL_FIELD_LAST_MARK));
        if (if_shaper_en)
        {
            uint32 field_val;
            field_val = ((enable == TRUE) ? 0 : 1);
            /*
             * Set the value
             */
            SHR_IF_ERR_EXIT(dnx_dbal_gen_set(unit, DBAL_TABLE_EGQ_SHAPER_GLOBAL_CONFIGURATION,
                                             1, 1,
                                             GEN_DBAL_FIELD32, DBAL_FIELD_CORE_ID, (uint32) core,
                                             GEN_DBAL_FIELD32, DBAL_FIELD_IFC_SPR_DIS, calendar_id, field_val,
                                             GEN_DBAL_FIELD_LAST_MARK));
        }
    }
exit:
    SHR_FUNC_EXIT;
}

/*
 * \brief -
 *   Enable/disable all port shapers: interface, otm, qpair, TCG.
 *   Inreface is enabled only for 'channelized' ports.
 * \param [in] unit -
 *   Int. Identifier of HW platform.
 * \param [in] logical_port -
 *   bcm_port_t. Logical port identifying the specific base qpair on which rquired
 *   operations are to be done.
 * \param [in] enable -
 *   uint32. Flag.
 *   If TRUE then enable all specified shapers.
 *   Else disable all.
 * \return
 *    \retval Zero - On success
 *    \retval Error - Identifier as per shr_error_e
 * \remark
 *   * None
 * \see
 *   * dnx_egr_queuing_port_enable_and_flush_set
 */
static shr_error_e
dnx_egr_queuing_port_all_shapers_enable_set(
    int unit,
    bcm_port_t logical_port,
    uint32 enable)
{
    int is_channelized;
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(dnx_algo_port_is_channelized_get(unit, logical_port, &is_channelized));
    /**
     * interface shaper
     */
    if (is_channelized)
    {
        SHR_IF_ERR_EXIT(dnx_egr_queuing_interface_shaper_enable_set(unit, logical_port, enable));
    }
    /**
     * qpair shaper
     */
    SHR_IF_ERR_EXIT(dnx_egr_queuing_port_q_pair_shaper_enable_set(unit, logical_port, enable));
    /**
     * tcg shaper
     */
    SHR_IF_ERR_EXIT(dnx_ofp_rates_port_tcg_shaper_enable_set(unit, logical_port, enable));
    /**
     * port shaper
     */
    SHR_IF_ERR_EXIT(dnx_egr_queuing_port_otm_shaper_enable_set(unit, logical_port, enable));
exit:
    SHR_FUNC_EXIT;
}

/*
 * See egr_queuing.h file
 */
shr_error_e
dnx_egr_queuing_port_enable_and_flush_set(
    int unit,
    bcm_port_t logical_port,
    uint32 enable,
    uint32 flush)
{
    int core;
    shr_error_e res;
    int base_q_pair, cosq, nof_priorities;
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, logical_port, &core));
    SHR_IF_ERR_EXIT(dnx_algo_port_base_q_pair_get(unit, logical_port, &base_q_pair));
    SHR_IF_ERR_EXIT(dnx_algo_port_priorities_nof_get(unit, logical_port, &nof_priorities));
    for (cosq = 0; cosq < nof_priorities; cosq++)
    {
        SHR_IF_ERR_EXIT(dnx_ecgm_dbal_queue_disable_set(unit, core, base_q_pair + cosq, enable ? 0 : 1));
    }

    if (enable == FALSE && flush == TRUE)
    {
        /**
         * Disable port shapers. Enable port shapers if failed
         */
        res = dnx_egr_queuing_port_all_shapers_enable_set(unit, logical_port, FALSE);
        if (SHR_FAILURE(res))
        {
            SHR_IF_ERR_EXIT(dnx_egr_queuing_port_all_shapers_enable_set(unit, logical_port, TRUE));
            SHR_ERR_EXIT(res, "Failure detected by %s at logical_port 0x%08X enable %d\r\n",
                         "dnx_egr_queuing_port_all_shapers_enable_set", logical_port, FALSE);
        }
        /**
         * Set egress port to ignore flow control. Enable port shapers if failed
         */
        res = dnx_egr_queuing_port_fc_enable_set(unit, logical_port, FALSE);
        if (SHR_FAILURE(res))
        {
            SHR_IF_ERR_EXIT(dnx_egr_queuing_port_all_shapers_enable_set(unit, logical_port, TRUE));
            SHR_ERR_EXIT(res, "Failure detected by %s at logical_port 0x%08X enable %d\r\n",
                         "dnx_egr_queuing_port_fc_enable_set", logical_port, FALSE);
        }
        /**
         * Make sure that the otm port has no unicast packet left. For failure enable port shapers and flow control
         */
        SHR_IF_ERR_EXIT(dnx_algo_port_priorities_nof_get(unit, logical_port, &nof_priorities));
        for (cosq = 0; cosq < nof_priorities; cosq++)
        {
            res =
                dnx_ecgm_dbal_queue_pd_count_zero_poll(unit, core, DNXCMN_TIMEOUT, DNXCMN_MIN_POLLS,
                                                       base_q_pair + cosq, 0);
            if (SHR_FAILURE(res))
            {
                SHR_IF_ERR_EXIT(dnx_egr_queuing_port_all_shapers_enable_set(unit, logical_port, TRUE));
                SHR_IF_ERR_EXIT(dnx_egr_queuing_port_fc_enable_set(unit, logical_port, TRUE));
                SHR_ERR_EXIT(res, "Failure detected by %s at cosq %d is_multicast %d\r\n",
                             "dnx_ecgm_dbal_queue_pd_count_zero_poll", cosq, 0);
            }
        }
        /**
         * Make sure that the otm port has no multicast packet left. For failure enable port shapers and flow control
         */
        for (cosq = 0; cosq < nof_priorities; cosq++)
        {
            res =
                dnx_ecgm_dbal_queue_pd_count_zero_poll(unit, core, DNXCMN_TIMEOUT, DNXCMN_MIN_POLLS,
                                                       base_q_pair + cosq, 1);
            if (SHR_FAILURE(res))
            {
                SHR_IF_ERR_EXIT(dnx_egr_queuing_port_all_shapers_enable_set(unit, logical_port, TRUE));
                SHR_IF_ERR_EXIT(dnx_egr_queuing_port_fc_enable_set(unit, logical_port, TRUE));
                SHR_ERR_EXIT(res, "Failure detected by %s at cosq %d is_multicast %d\r\n",
                             "dnx_ecgm_dbal_queue_pd_count_zero_poll", cosq, 1);
            }
        }
        /**
         * Set egress port to not ignore flow control
         */
        SHR_IF_ERR_EXIT(dnx_egr_queuing_port_fc_enable_set(unit, logical_port, TRUE));
        /**
         * Enable port shapers
         */
        SHR_IF_ERR_EXIT(dnx_egr_queuing_port_all_shapers_enable_set(unit, logical_port, TRUE));
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * see egr_queuing.h file
 */
shr_error_e
dnx_egr_queuing_port_enable_get(
    int unit,
    bcm_port_t port,
    uint32 *enable)
{
    int core;
    int base_q_pair;
    uint32 disable;
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, port, &core));
    SHR_IF_ERR_EXIT(dnx_algo_port_base_q_pair_get(unit, port, &base_q_pair));
    SHR_IF_ERR_EXIT(dnx_ecgm_dbal_queue_disable_get(unit, core, base_q_pair, &disable));
    *enable = !disable;
exit:
    SHR_FUNC_EXIT;
}

/*
 * See egr_queuing.h
 */
shr_error_e
dnx_egr_queuing_pqp_counter_configuration_set(
    int unit,
    int core,
    int count_by,
    int count_place)
{
    int is_by_qp = 0, is_by_otm = 0, is_by_if = 0;
    int qp = 0, otm = 0, egq_if = 0;
    int logic_port = 0, base_qp = 0;

    SHR_FUNC_INIT_VARS(unit);
    switch (count_by)
    {
        case DNX_EGQ_COUTNER_FILTER_BY_QP:
        {
            is_by_qp = 1;
            qp = count_place;
            break;
        }
        case DNX_EGQ_COUTNER_FILTER_BY_OTM:
        {
            is_by_otm = 1;
            otm = count_place;
            SHR_IF_ERR_EXIT(dnx_algo_port_tm_to_logical_get(unit, core, otm, &logic_port));
            SHR_IF_ERR_EXIT(dnx_algo_port_base_q_pair_get(unit, logic_port, &base_qp));
            break;
        }
        case DNX_EGQ_COUTNER_FILTER_BY_IF:
        {
            is_by_if = 1;
            egq_if = count_place;
            break;
        }
        case DNX_EGQ_COUTNER_FILTER_BY_NONE:
        {
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Unexpected counter by %d\n", count_by);
            break;
        }
    }
    SHR_IF_ERR_EXIT(dnx_dbal_gen_set(unit, DBAL_TABLE_EGQ_PQP_COUNTER, 1, 12,
                                     GEN_DBAL_FIELD32, DBAL_FIELD_CORE_ID, (uint32) core,
    /** Set counter by qp, hr or egq if */
                                     GEN_DBAL_FIELD32, DBAL_FIELD_ENQ_COUNT_BY_QP, INST_SINGLE, (uint32) (is_by_qp),
                                     GEN_DBAL_FIELD32, DBAL_FIELD_ENQ_COUNT_BY_OTM, INST_SINGLE, (uint32) (is_by_otm),
                                     GEN_DBAL_FIELD32, DBAL_FIELD_ENQ_COUNT_BY_IF, INST_SINGLE, (uint32) (is_by_if),
                                     GEN_DBAL_FIELD32, DBAL_FIELD_DEQ_COUNT_BY_QP, INST_SINGLE, (uint32) (is_by_qp),
                                     GEN_DBAL_FIELD32, DBAL_FIELD_DEQ_COUNT_BY_OTM, INST_SINGLE, (uint32) (is_by_otm),
                                     GEN_DBAL_FIELD32, DBAL_FIELD_DEQ_COUNT_BY_IF, INST_SINGLE, (uint32) (is_by_if),
    /** Set place to track its count   */
                                     GEN_DBAL_FIELD32, DBAL_FIELD_ENQ_QP, INST_SINGLE, (uint32) (qp),
                                     GEN_DBAL_FIELD32, DBAL_FIELD_ENQ_OTM, INST_SINGLE, (uint32) (otm),
                                     GEN_DBAL_FIELD32, DBAL_FIELD_ENQ_EGQ_IF, INST_SINGLE, (uint32) (egq_if),
                                     GEN_DBAL_FIELD32, DBAL_FIELD_DEQ_QP, INST_SINGLE, (uint32) (qp),
    /** base qpair is requested to set to DEQ_OTM due to HW design   */
                                     GEN_DBAL_FIELD32, DBAL_FIELD_DEQ_OTM, INST_SINGLE, (uint32) (base_qp),
                                     GEN_DBAL_FIELD32, DBAL_FIELD_DEQ_EGQ_IF, INST_SINGLE, (uint32) (egq_if),
                                     GEN_DBAL_FIELD_LAST_MARK));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See egr_queuing.h
 */
shr_error_e
dnx_egr_queuing_epni_counter_configuration_set(
    int unit,
    int core,
    int flag,
    int count_by,
    int count_place)
{
    int port;
    int base_qp;
    int num_priorities;
    int is_count_byte = 0;
    int q_mask, if_mask, mirror_pri_mask;
    int q_val = 0, if_val = 0, mirror_pri = 0;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_tables_field_max_value_get
                    (unit, DBAL_TABLE_EGQ_EPNI_COUNTER, DBAL_FIELD_Q_MASK, 0, 0, 0, &q_mask));
    SHR_IF_ERR_EXIT(dbal_tables_field_max_value_get
                    (unit, DBAL_TABLE_EGQ_EPNI_COUNTER, DBAL_FIELD_IF_MASK, 0, 0, 0, &if_mask));
    SHR_IF_ERR_EXIT(dbal_tables_field_max_value_get
                    (unit, DBAL_TABLE_EGQ_EPNI_COUNTER, DBAL_FIELD_MIRR_PRIO_MASK, 0, 0, 0, &mirror_pri_mask));
    switch (count_by)
    {
        case DNX_EGQ_COUTNER_FILTER_BY_QP:
            q_mask = 0;
            q_val = count_place;
            break;
        case DNX_EGQ_COUTNER_FILTER_BY_OTM:
            port = count_place;
            SHR_IF_ERR_EXIT(dnx_algo_port_base_q_pair_get(unit, port, &base_qp));
            SHR_IF_ERR_EXIT(dnx_algo_port_priorities_nof_get(unit, port, &num_priorities));
            q_mask = num_priorities - 1;
            q_val = base_qp;
            break;
        case DNX_EGQ_COUTNER_FILTER_BY_IF:
            if_mask = 0;
            if_val = count_place;
            break;
        case DNX_EGQ_COUTNER_FILTER_BY_MIRROR:
            mirror_pri_mask = 0;
            mirror_pri = count_place;
            break;
        case DNX_EGQ_COUTNER_FILTER_BY_NONE:
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Unexpected counter by %d\n", count_by);
            break;
    }

    if (flag & DNX_EGQ_CONFIGURATION_COUNT_BYTES)
    {
        is_count_byte = 1;
    }
    SHR_IF_ERR_EXIT(dnx_dbal_gen_set(unit, DBAL_TABLE_EGQ_EPNI_COUNTER, 1, 7,
                                     GEN_DBAL_FIELD32, DBAL_FIELD_CORE_ID, (uint32) core,
    /** Set counter by qp, hr or egq if */
                                     GEN_DBAL_FIELD32, DBAL_FIELD_COUNT_BYTES, INST_SINGLE, (uint32) (is_count_byte),
                                     GEN_DBAL_FIELD32, DBAL_FIELD_Q_VAL, INST_SINGLE, (uint32) (q_val),
                                     GEN_DBAL_FIELD32, DBAL_FIELD_Q_MASK, INST_SINGLE, (uint32) (q_mask),
                                     GEN_DBAL_FIELD32, DBAL_FIELD_IF_VAL, INST_SINGLE, (uint32) (if_val),
                                     GEN_DBAL_FIELD32, DBAL_FIELD_IF_MASK, INST_SINGLE, (uint32) (if_mask),
                                     GEN_DBAL_FIELD32, DBAL_FIELD_MIRR_PRIO_VAL, INST_SINGLE, (uint32) (mirror_pri),
                                     GEN_DBAL_FIELD32, DBAL_FIELD_MIRR_PRIO_MASK, INST_SINGLE,
                                     (uint32) (mirror_pri_mask), GEN_DBAL_FIELD_LAST_MARK));
exit:
    SHR_FUNC_EXIT;
}
/*
 * See egr_queuing.h file
 */
shr_error_e
dnx_egr_queuing_port_disable(
    int unit)
{
    bcm_port_t port;
    dnx_algo_port_info_s port_info;
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(dnx_algo_port_removed_port_get(unit, &port));
    SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, port, &port_info));
    /**
     * omit non egress ports
     */
    if (!DNX_ALGO_PORT_TYPE_IS_EGR_TM(unit, port_info))
    {
        SHR_EXIT();
    }

    /** disable port qpairs and flush */
    SHR_IF_ERR_EXIT(dnx_egr_queuing_port_enable_and_flush_set(unit, port, FALSE, TRUE));
exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_egq_queuing_shaper_global_config_init(
    int unit,
    int core_id)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EGQ_SHAPER_GLOBAL_CONFIGURATION, &entry_handle_id));
    /** Setting key fields */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, (uint32) core_id);
    /** Setting value fields */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PQP_CALENDAR_SWITCH_EN, INST_SINGLE, TRUE);
    if (dnx_data_egr_queuing.params.feature_get(unit, dnx_data_egr_queuing_params_fqp_calender_set_select) == TRUE)
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FQP_CALENDAR_SWITCH_EN, INST_SINGLE, TRUE);
    }
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FORCE_FC_MODE, INST_SINGLE, FALSE);

    /** global shaper max burst needs to be double size of the standard max burst size */
    if (dnx_data_egr_queuing.params.feature_get(unit, dnx_data_egr_queuing_params_total_shaper_max_burst_exist) == TRUE)
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TOTAL_SHAPER_BURST_MAX, INST_SINGLE,
                                     2 * DNX_OFP_RATES_BURST_DEFAULT);
    }

    /** Preforming the action */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * See egr_queuing.h file
 */
shr_error_e
dnx_egr_queuing_init(
    int unit)
{
    uint32 profile;
    int core;
    dnx_cosq_egress_queue_mapping_info_t *mapping_info = NULL;
    uint32 tdm_exist;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * RQP configuration to resolve ecc error
     */
    if (dnx_data_device.general.feature_get(unit, dnx_data_device_general_rqp_ecc_err_exist))
    {
        SHR_IF_ERR_EXIT(dnx_dbal_gen_set(unit, DBAL_TABLE_EGQ_GLOBAL_CONFIGURATION, 1, 1,
                                         GEN_DBAL_FIELD32, DBAL_FIELD_CORE_ID, BCM_CORE_ALL,
                                         GEN_DBAL_FIELD32, DBAL_FIELD_RQP_ECC_ERR, INST_SINGLE, 0x80,
                                         GEN_DBAL_FIELD_LAST_MARK));
    }
    if (dnx_data_tdm.params.feature_get(unit, dnx_data_tdm_params_is_supported) == TRUE)
    {
        tdm_exist = (dnx_data_tdm.params.mode_get(unit) != TDM_MODE_NONE) ? TRUE : FALSE;
    }
    else
    {
        tdm_exist = FALSE;
    }

    /*
     * init egr data base
     */
    SHR_IF_ERR_EXIT(dnx_egr_db.init(unit));

    /*
     * create port tc/dp default profile
     */
    SHR_ALLOC_SET_ZERO(mapping_info,
                       sizeof(dnx_cosq_egress_queue_mapping_info_t),
                       "egress queue mapping info handle memory", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);
    profile = DNX_EGR_QUEUE_DEFAULT_PROFILE_MAP;
    SHR_IF_ERR_EXIT(dnx_am_template_egress_queue_mapping_create(unit, profile, mapping_info));
    {
        /*
         * All settings are for all-cores.
         */
        core = BCM_CORE_ALL;
        /*
         * 1. Configures egress multicast replication fifo parameters received from dnx data to HW
         * 2. Define TXQ threshold in PDs. values received from ASIC
         * 3. Enable invalid OTM to support Egress MC groups with trunk destinations
         * 4. Set PRD_ALMOST_FULL_TH to 10 (HW default is 0x100). The motivation for this change is
         *    that this threshold indicates number of empty places in DB. When number of empty places
         *    is less than this threshold then design stops receiving new traffic from FDA.
         */
        SHR_IF_ERR_EXIT(dnx_egq_dbal_emr_fifo_default_set(unit, core, tdm_exist));
        SHR_IF_ERR_EXIT(dnx_dbal_gen_set(unit, DBAL_TABLE_EGQ_GLOBAL_CONFIGURATION, 1, 4,
                                         GEN_DBAL_FIELD32, DBAL_FIELD_CORE_ID, (uint32) core,
                                         /*
                                          * Set the values. Note that we define, here, a 'group' of HW tables because they
                                          * all do the same: ERPP_TDM_GENERAL_CONFIGURATIONr, RQP_TDM_GENERAL_CONFIGURATIONr,
                                          * ETPPA_TDM_GENERAL_CONFIGURATIONr
                                          */
                                         GEN_DBAL_FIELD32, DBAL_FIELD_TXQ_IREADY_TH,
                                         INST_SINGLE, (uint32) (dnx_data_egr_queuing.params.txq_iready_th_get(unit)),
                                         GEN_DBAL_FIELD32, DBAL_FIELD_TXQ_TDM_IREADY_TH, INST_SINGLE,
                                         (uint32) (dnx_data_egr_queuing.params.txq_tdm_iready_th_get(unit)),
                                         GEN_DBAL_FIELD32, DBAL_FIELD_INVALID_OTM_ENA, INST_SINGLE, (uint32) (1),
                                         GEN_DBAL_FIELD32, DBAL_FIELD_INVALID_OTM_NUMBER, INST_SINGLE,
                                         dnx_data_egr_queuing.params.invalid_otm_port_get(unit),
                                         GEN_DBAL_FIELD_LAST_MARK));

        if (dnx_data_egr_queuing.reassembly.feature_get(unit, dnx_data_egr_queuing_reassembly_prs))
        {
            SHR_IF_ERR_EXIT(dnx_dbal_gen_set(unit, DBAL_TABLE_EGQ_GLOBAL_CONFIGURATION, 1, 1,
                                             GEN_DBAL_FIELD32, DBAL_FIELD_CORE_ID, (uint32) core,
                                             GEN_DBAL_FIELD32, DBAL_FIELD_PRD_ALMOST_FULL_TH, INST_SINGLE, 10,
                                             GEN_DBAL_FIELD_LAST_MARK));

            SHR_IF_ERR_EXIT(dnx_dbal_gen_set(unit, DBAL_TABLE_EGQ_GLOBAL_CONFIGURATION, 1, 2,
                                             GEN_DBAL_FIELD32, DBAL_FIELD_CORE_ID, (uint32) core,
                                             GEN_DBAL_FIELD32, DBAL_FIELD_PRS_FIFO_ALMOST_FULL_TH, INST_SINGLE,
                                             0x4a, GEN_DBAL_FIELD32, DBAL_FIELD_PRS_FIFO_ALMOST_FULL_2_TH,
                                             INST_SINGLE, 0x50, GEN_DBAL_FIELD_LAST_MARK));
        }

        /*
         * for devices supporting small burst size
         */
        if (dnx_data_egr_queuing.params.burst_size_below_mtu_get(unit))
        {
            SHR_IF_ERR_EXIT(dnx_dbal_gen_set(unit, DBAL_TABLE_EGQ_SHAPER_GLOBAL_CONFIGURATION, 1, 4,
                                             GEN_DBAL_FIELD32, DBAL_FIELD_CORE_ID, (uint32) core,
                                             GEN_DBAL_FIELD32, DBAL_FIELD_SMALL_BURST_ENABLE_CONFIG, INST_SINGLE, 0x3,
                                             GEN_DBAL_FIELD32, DBAL_FIELD_OTM_TOTAL_ALLOW_ACC_ABOVE_MAX, INST_SINGLE,
                                             FALSE, GEN_DBAL_FIELD32, DBAL_FIELD_OTM_ALLOW_ACC_ABOVE_MAX, INST_SINGLE,
                                             FALSE, GEN_DBAL_FIELD32, DBAL_FIELD_QP_ALLOW_ACC_ABOVE_MAX, INST_SINGLE,
                                             FALSE, GEN_DBAL_FIELD_LAST_MARK));
        }

        /*
         * 1. fqp/pqp calendar enable
         * 2. set force mode
         */
        SHR_IF_ERR_EXIT(dnx_egq_queuing_shaper_global_config_init(unit, core));
    }
    /*
     * setting recycle/mirror interface burst to max
     */
    SHR_IF_ERR_EXIT(dnx_egr_recycle_burst_set(unit, _SHR_CORE_ALL, 0, DNX_OFP_RATES_BURST_DEFAULT));
    SHR_IF_ERR_EXIT(dnx_egr_recycle_burst_set(unit, _SHR_CORE_ALL, 1, DNX_OFP_RATES_BURST_DEFAULT));

    /*
     * Phantom Queues init
     */
    if (dnx_data_egr_queuing.rate_measurement.feature_get(unit, dnx_data_egr_queuing_rate_measurement_is_supported))
    {
        SHR_IF_ERR_EXIT(dnx_phantom_queues_init(unit));
    }

    SHR_IF_ERR_EXIT(dnx_egq_dbal_pqp_almost_empty_thresholds_set(unit));

    /*
     * Egress Shared Buffers init
     */
    if (dnx_data_esb.general.feature_get(unit, dnx_data_esb_general_esb_support))
    {
        SHR_IF_ERR_EXIT(dnx_esb_init(unit));
    }

    /** mask out EPNI counters - requires for 'diag counter graphical' */
    SHR_IF_ERR_EXIT(dnx_egq_dbal_epni_counter_configuration_reset(unit, BCM_CORE_ALL));

    /** enable ERPP bubble mechanism */
    SHR_IF_ERR_EXIT(dnx_dbal_gen_set(unit, DBAL_TABLE_EGQ_GLOBAL_CONFIGURATION, 1, 1,
                                     GEN_DBAL_FIELD32, DBAL_FIELD_CORE_ID, BCM_CORE_ALL,
                                     GEN_DBAL_FIELD32, DBAL_FIELD_ERPP_BUBBLE_REQ_EN, INST_SINGLE, TRUE,
                                     GEN_DBAL_FIELD_LAST_MARK));

exit:
    SHR_FREE(mapping_info);
    SHR_FUNC_EXIT;
}

/*
 * See egr_queuing.h file
 */
shr_error_e
dnx_egr_queuing_deinit(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);
    /*
     * Sw state module deinit is done automatically at device deinit
     */
    SHR_FUNC_EXIT;
}
/*
 *    Internal conversion to the table entry format
 */
/**
 * \brief - Internal conversion. Load pointed structure by input values. See 'entry'
 *   below.
 *
 * \param [in] unit -
 *   HW Unit-ID
 * \param [in] map_type_ndx -
 *   dnx_egr_q_prio_mapping_type_e. See explanation in 'entry' below.
 * \param [in] tc_ndx -
 *   uint32. Value to load into 'entry->tc'
 * \param [in] dp_ndx -
 *   uint32. Value to load into 'entry->dp'
 * \param [in] map_profile_ndx -
 *   uint32. Value to load into 'entry->map_profile'
 * \param [out] entry_p -
 *   Pointer to dnx_egq_tc_dp_map_tbl_entry_t. This procedure loads pointed structure
 *   by internal values related to the various input parameters. (Actually, all input
 *   parameters are loaded as is except for 'entry->is_egr_mc' which is loaded by
 *   FALSE if 'map_type_ndx' is DNX_EGR_UCAST_TO_SCHED and by TRUE if 'map_type_ndx'
 *   is DNX_EGR_MCAST_TO_UNSCHED.
 * \return
 *   If zero (_SHR_E_NONE), then no error was encountered.
 *   Otherwise, see shr_error_e
 * \remark
 *   * None
 * \see
 *   * dnx_egr_queuing_prio_set
 */
static shr_error_e
dnx_egr_queuing_prio_map_entry_get(
    int unit,
    dnx_egr_q_prio_mapping_type_e map_type_ndx,
    uint32 tc_ndx,
    uint32 dp_ndx,
    uint32 map_profile_ndx,
    dnx_egq_tc_dp_map_tbl_entry_t * entry_p)
{
    uint8 is_egr_mc;
    SHR_FUNC_INIT_VARS(unit);
    is_egr_mc = FALSE;
    sal_memset(entry_p, 0, sizeof(*entry_p));
    entry_p->map_profile = map_profile_ndx;
    entry_p->dp = dp_ndx;
    entry_p->tc = tc_ndx;
    switch (map_type_ndx)
    {
        case DNX_EGR_UCAST_TO_SCHED:
        {
            is_egr_mc = FALSE;
            break;
        }
        case DNX_EGR_MCAST_TO_UNSCHED:
        {
            is_egr_mc = TRUE;
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Illegal map_type_ndx (%d). See dnx_egr_q_prio_mapping_type_e", map_type_ndx);
        }
    }
    entry_p->is_egr_mc = is_egr_mc;
exit:
    SHR_FUNC_EXIT;
}
/*
 *  See egr_queuing.h
 */
shr_error_e
dnx_egr_queuing_prio_set(
    int unit,
    int core,
    dnx_egr_q_prio_mapping_type_e map_type_ndx,
    uint32 tc_ndx,
    uint32 dp_ndx,
    uint32 map_profile_ndx,
    dnx_egr_q_priority_t * priority)
{
    dnx_egq_tc_dp_map_tbl_entry_t entry;
    dnx_egq_tc_dp_map_tbl_data_t tbl_data;
    SHR_FUNC_INIT_VARS(unit);
    sal_memset(&entry, 0, sizeof(entry));
    /*
     * Load 'entry' by the various input parameters. They will be used as 'key' to
     * DBAL table DBAL_TABLE_EGQ_TC_DP_MAP
     */
    SHR_IF_ERR_EXIT(dnx_egr_queuing_prio_map_entry_get(unit, map_type_ndx, tc_ndx, dp_ndx, map_profile_ndx, &entry));
    tbl_data.dp = priority->dp;
    tbl_data.tc = priority->tc;
    SHR_IF_ERR_EXIT(dnx_egq_dbal_tc_dp_map_tbl_set(unit, core, &entry, &tbl_data));
exit:
    SHR_FUNC_EXIT;
}
/*
 *  See egr_queuing.h
 */
shr_error_e
dnx_egr_queuing_profile_map_set(
    int unit,
    int core_id,
    uint32 internal_port,
    uint32 map_profile_id)
{
    dnx_egq_ppct_tbl_data_t ppct_tbl_data;
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(dnx_egq_dbal_cos_map_profile_get(unit, core_id, internal_port, &ppct_tbl_data));
    ppct_tbl_data.cos_map_profile = map_profile_id;
    SHR_IF_ERR_EXIT(dnx_egq_dbal_cos_map_profile_set(unit, core_id, internal_port, &ppct_tbl_data));
exit:
    SHR_FUNC_EXIT;
}
/*
 *  See egr_queuing.h
 */
int
dnx_egr_queuing_profile_map_get(
    int unit,
    int core_id,
    uint32 pp_port,
    uint32 *map_profile_id)
{
    dnx_egq_ppct_tbl_data_t tbl_data;
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(dnx_egq_dbal_cos_map_profile_get(unit, core_id, pp_port, &tbl_data));
    *map_profile_id = tbl_data.cos_map_profile;
exit:
    SHR_FUNC_EXIT;
}
/*
 *  See egr_queuing.h
 */
int
dnx_egr_queuing_port_prio_mode_set(
    int unit,
    int core_id,
    dnx_port_t logical_port,
    dnx_egr_port_priority_mode_e priority_mode)
{
    uint32 egq_priority_val;
    uint32 ps;
    int base_q_pair;

    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(dnx_algo_port_base_q_pair_get(unit, logical_port, &base_q_pair));
    ps = base_q_pair / DNX_EGR_NOF_Q_PAIRS_IN_PS;
    switch (priority_mode)
    {
        case DNX_EGR_PORT_ONE_PRIORITY:
        {
            egq_priority_val = DNX_EGQ_PS_MODE_ONE_PRIORITY_VAL;
            break;
        }
        case DNX_EGR_PORT_TWO_PRIORITIES:
        {
            egq_priority_val = DNX_EGQ_PS_MODE_TWO_PRIORITY_VAL;
            break;
        }
        case DNX_EGR_PORT_FOUR_PRIORITIES:
        {
            egq_priority_val = DNX_EGQ_PS_MODE_FOUR_PRIORITY_VAL;
            break;
        }
        case DNX_EGR_PORT_EIGHT_PRIORITIES:
        {
            egq_priority_val = DNX_EGQ_PS_MODE_EIGHT_PRIORITY_VAL;
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid priority mode %d", priority_mode);
        }
    }
    SHR_IF_ERR_EXIT(dnx_dbal_gen_set(unit, DBAL_TABLE_EGQ_SHAPING_PS_MODE, 2, 1,
    /** 2 keys */
                                     GEN_DBAL_FIELD32, DBAL_FIELD_PORT_SCHEDULER, (uint32) (ps),
                                     GEN_DBAL_FIELD32, DBAL_FIELD_CORE_ID, (uint32) core_id,
    /** 1 field  */
                                     GEN_DBAL_FIELD32, DBAL_FIELD_PS_MODE, INST_SINGLE, (uint32) (egq_priority_val),
                                     GEN_DBAL_FIELD_LAST_MARK));
exit:
    SHR_FUNC_EXIT;
}
/*
 * See egr_queuing.h
 */
shr_error_e
dnx_egr_queuing_is_high_priority_queue_get(
    int unit,
    int core,
    bcm_port_t logical_port,
    uint32 cosq,
    int *is_high_priority)
{
    dnx_egr_ofp_sch_info_t info;
    int base_q_pair, nof_q_pairs;
    dnx_egr_ofp_interface_prio_e nif_priority;
    uint32 nif_prio_val;

    SHR_FUNC_INIT_VARS(unit);
    sal_memset(&info, 0, sizeof(info));
    /*
     * Retrieve base_q_pair
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_base_q_pair_get(unit, logical_port, &base_q_pair));
    SHR_IF_ERR_EXIT(dnx_algo_port_priorities_nof_get(unit, logical_port, &nof_q_pairs));
    SHR_IF_ERR_EXIT(dnx_dbal_gen_get(unit, DBAL_TABLE_EGQ_QPAIR_ATTRIBUTES,
                                     2, 1,
                                     GEN_DBAL_FIELD32, DBAL_FIELD_QPAIR, (uint32) (base_q_pair + cosq),
                                     GEN_DBAL_FIELD32, DBAL_FIELD_CORE_ID, (uint32) core,
                                     GEN_DBAL_FIELD32, DBAL_FIELD_QP_IS_HP, INST_SINGLE, (uint32 *) (&nif_prio_val),
                                     GEN_DBAL_FIELD_LAST_MARK));
    nif_priority = (nif_prio_val == 0) ? DNX_EGR_OFP_INTERFACE_PRIO_LOW : DNX_EGR_OFP_INTERFACE_PRIO_HIGH;
    if (nif_priority == DNX_EGR_OFP_INTERFACE_PRIO_HIGH)
    {
        *is_high_priority = TRUE;
    }
    else
    {
        *is_high_priority = FALSE;
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * See cosq_egq.h
 */
static shr_error_e
dnx_egr_queuing_port_is_high_verify(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    int mode,
    int weight,
    int is_set)
{
    SHR_FUNC_INIT_VARS(unit);

    if (is_set == TRUE)
    {
        /*
         * Set verify
         */
        switch (mode)
        {
            case BCM_COSQ_SP0:
            case BCM_COSQ_SP1:
                break;

            default:
                SHR_ERR_EXIT(_SHR_E_PARAM, "unit %d, invalid mode %d. Range is SP0-SP1 (%d-%d)", unit, mode,
                             BCM_COSQ_SP0, BCM_COSQ_SP1);
        }
    }
    else
    {
        /*
         * Get verify
         */
        if (cosq != 0)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid parameter: cosq: %d. Cosq must be set to 0 in case of Local gport",
                         cosq);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * See cosq_egq.h
 */
shr_error_e
dnx_egr_queuing_port_is_high_set(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    int mode,
    int weight)
{
    int num_priorities;
    bcm_port_t logical_port;
    dnx_algo_port_info_s port_info;
    int rcy_priority;
    SHR_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_egr_queuing_port_is_high_verify(unit, gport, cosq, mode, weight, TRUE));

    logical_port = BCM_GPORT_LOCAL_GET(gport);

    SHR_IF_ERR_EXIT(dnx_algo_port_priorities_nof_get(unit, logical_port, &num_priorities));
    for (cosq = 0; cosq < num_priorities; cosq++)
    {
        SHR_IF_ERR_EXIT(dnx_egq_dbal_gport_scheduler_queue_sched_set(unit, gport, cosq, mode, weight));
    }

    /*
     * In case of RCY port AND context is allocated per priority - update reassembly context according to priority
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, logical_port, &port_info));
    if ((DNX_ALGO_PORT_TYPE_IS_RCY(unit, port_info))
        && (dnx_data_ingr_reassembly.
            context.feature_get(unit, dnx_data_ingr_reassembly_context_rcy_interleaving_between_priorities)))
    {
        rcy_priority = (mode == BCM_COSQ_SP0) ? 1 : 0;
        SHR_IF_ERR_EXIT(dnx_port_ingr_reassembly_map_set(unit, logical_port, rcy_priority));
    }

exit:
    SHR_FUNC_EXIT;
}
/**
 * See cosq_egq.h
 */
shr_error_e
dnx_egr_queuing_port_is_high_get(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    int *mode,
    int *weight)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_egr_queuing_port_is_high_verify(unit, gport, cosq, 0, 0, FALSE));

    SHR_IF_ERR_EXIT(dnx_cosq_egq_gport_egress_scheduler_queue_sched_get(unit, gport, cosq, mode, weight));
exit:
    SHR_FUNC_EXIT;
}
/*
 * Purpose:
 * init egress port to a specific configuration (out_tc,out_dp)
 */
int
dnx_egr_queuing_default_tc_dp_map_set(
    int unit,
    bcm_port_t port)
{
    uint32 pp_port;
    int nof_priorities;
    int is_last, is_allocated, core;
    int old_profile, new_profile;
    dnx_cosq_egress_queue_mapping_info_t mapping_info;
    int tc, dp, priority;

    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(dnx_algo_port_pp_port_get(unit, port, &core, &pp_port));
    SHR_IF_ERR_EXIT(dnx_algo_port_priorities_nof_get(unit, port, &nof_priorities));
    sal_memset(&mapping_info, 0x0, sizeof(mapping_info));
    /*
     * Get Old data
     */
    SHR_IF_ERR_EXIT(dnx_am_template_egress_queue_mapping_data_get(unit, pp_port, core, &mapping_info));
    for (tc = 0; tc < DNX_COSQ_NOF_TC; tc++)
    {
        /*
         * select Q
         */
        switch (nof_priorities)
        {
            case 1:
            {
                priority = 0;
                break;
            }
            case 2:
            {
                priority = (tc < 4 ? 0 : 1);
                break;
            }
            case 4:
            {
                priority = (tc >> 1);
                break;
            }
            case 8:
            default:
            {
                priority = tc;
                break;
            }
        }
        for (dp = 0; dp < DNX_COSQ_NOF_DP; dp++)
        {
            /*
             * set unsched traffic mapping
             */
            mapping_info.queue_mapping[DNX_EGR_MCAST_TO_UNSCHED][tc][dp].dp = dp;
            mapping_info.queue_mapping[DNX_EGR_MCAST_TO_UNSCHED][tc][dp].tc = priority;
            /*
             * set sched traffic mapping
             */
            mapping_info.queue_mapping[DNX_EGR_UCAST_TO_SCHED][tc][dp].dp = dp;
            mapping_info.queue_mapping[DNX_EGR_UCAST_TO_SCHED][tc][dp].tc = priority;
        }
    }
    /*
     * Exchange data
     */
    SHR_IF_ERR_EXIT(dnx_am_template_egress_queue_mapping_exchange
                    (unit, pp_port, core, &mapping_info, &old_profile, &is_last, &new_profile, &is_allocated));
    if (is_allocated)
    {
        /*
         * Create new profile
         */
        SHR_IF_ERR_EXIT(dnx_egr_queuing_profile_mapping_set(unit, core, new_profile, &mapping_info));
    }

    /*
     * Map port to new profile
     */
    SHR_IF_ERR_EXIT(dnx_egr_queuing_profile_map_set(unit, core, pp_port, new_profile));
exit:
    SHR_FUNC_EXIT;

}
/*
 * See cosq_egq.h
 */
shr_error_e
dnx_egr_queuing_profile_mapping_set(
    int unit,
    int core,
    int profile,
    const dnx_cosq_egress_queue_mapping_info_t * mapping_info)
{
    int index_dp, index_pri, index_type;
    dnx_egr_q_priority_t egr_prio;

    SHR_FUNC_INIT_VARS(unit);
    sal_memset(&egr_prio, 0, sizeof(egr_prio));
    /*
     * Run over all data and commit to hw
     */
    for (index_type = 0; index_type < DNX_DEVICE_COSQ_EGR_NOF_Q_PRIO_MAPPING_TYPES; index_type++)
    {
        for (index_pri = 0; index_pri < DNX_COSQ_NOF_TC; index_pri++)
        {
            for (index_dp = 0; index_dp < DNX_COSQ_NOF_DP; index_dp++)
            {
                egr_prio.dp = mapping_info->queue_mapping[index_type][index_pri][index_dp].dp;
                egr_prio.tc = mapping_info->queue_mapping[index_type][index_pri][index_dp].tc;
                SHR_IF_ERR_EXIT(dnx_egr_queuing_prio_set
                                (unit, core, index_type, index_pri, index_dp, profile, &egr_prio));
            }
        }
    }
exit:
    SHR_FUNC_EXIT;
}

/** see .h file */
shr_error_e
dnx_egr_queuing_calendar_index_get(
    int unit,
    bcm_port_t logical_port,
    int *calendar_id)
{
    int egr_if;
    int channelized;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_egr_if_get(unit, logical_port, &egr_if));
    SHR_IF_ERR_EXIT(dnx_algo_port_is_channelized_get(unit, logical_port, &channelized));

    /** if all interfaces can be channelized, the calendar id is equal to the interface id */
    if (dnx_data_egr_queuing.params.nof_egr_ch_interfaces_get(unit) ==
        dnx_data_egr_queuing.params.nof_egr_interfaces_get(unit))
    {
        *calendar_id = egr_if;
    }
    else
    {
        /** only channelized interface has 1x1 calendar id. None channelized goes all to last calendar id*/
        if (channelized == TRUE)
        {
            *calendar_id = egr_if;
        }
        else
        {
            *calendar_id = dnx_data_egr_queuing.params.nof_egr_ch_interfaces_get(unit);
        }
    }

exit:
    SHR_FUNC_EXIT;

}

/** see .h file */
shr_error_e
dnx_egr_queuing_hw_egr_if_convert(
    int unit,
    int hw_egr_if,
    bcm_port_if_t * bcm_egr_if)
{
    bcm_port_if_t egr_if;
    SHR_FUNC_INIT_VARS(unit);

    /** for invalid interface - no connection type */
    if (hw_egr_if == -1)
    {
        egr_if = BCM_PORT_IF_NULL;
    }
    else if (dnx_data_egr_queuing.params.egr_if_cpu_get(unit) == hw_egr_if)
    {
        egr_if = BCM_PORT_IF_CPU;
    }
    else if (dnx_data_egr_queuing.params.egr_if_sat_get(unit) == hw_egr_if)
    {
        egr_if = BCM_PORT_IF_SAT;
    }
    else if (dnx_data_egr_queuing.params.egr_if_oam_get(unit) == hw_egr_if)
    {
        egr_if = BCM_PORT_IF_OAMP;
    }
    else if (dnx_data_egr_queuing.params.egr_if_olp_get(unit) == hw_egr_if)
    {
        egr_if = BCM_PORT_IF_OLP;
    }
    else if (dnx_data_egr_queuing.params.egr_if_rcy_get(unit) == hw_egr_if)
    {
        egr_if = BCM_PORT_IF_RCY;
    }
    else if (dnx_data_egr_queuing.params.egr_if_txi_rcy_get(unit) == hw_egr_if)
    {
        egr_if = BCM_PORT_IF_RCY_MIRROR;
    }
    else if (dnx_data_egr_queuing.params.egr_if_eventor_get(unit) == hw_egr_if)
    {
        egr_if = BCM_PORT_IF_EVENTOR;
    }
    /** Any HW interface that does not belong to one of the special interfaces, will be threated as NIF_ETH */
    else
    {
        egr_if = BCM_PORT_IF_NIF_ETH;
    }

    *bcm_egr_if = egr_if;
/** exit: */
    SHR_FUNC_EXIT;
}
