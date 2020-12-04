/** \file algo_port_imb.c
 *  Port IMB algorithms for DNX.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_PORT
/*
 * Includes.
 * {
 */
#include <shared/shrextend/shrextend_debug.h>
#include <shared/utilex/utilex_integer_arithmetic.h>
#include <bcm_int/dnx/algo/port/algo_port_mgmt.h>
#include <bcm_int/dnx/algo/port/algo_port_imb.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/access/dnx_algo_port_imb_access.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_nif.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_device.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_fabric.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_port.h>
#include <bcm_int/dnx/port/imb/imb.h>
#include <bcm_int/dnx/port/imb/imb_common.h>

/*
 * }
 */
/*
 * Macros
 * {
 */
/**
 * Resource name defines for algo port imb
 * {
 */
#define DNX_ALGO_PORT_IMB_SCHEDULER_RMC_ID_RESOURCE "Scheduler RMC id"
/*
 * }
 */

/*
 * Local functions (documantation in function implementation)
 * {
 */
/*
 * }
 */
/**
 * \brief - calculate the thershold after overflow for the fifo according to the FIFO size. 
 * After FIFO reches overflow, writing to the fifo will resume only after fifo level 
 * goes below this value.  
 * 
 * see .h file
 */
shr_error_e
dnx_algo_port_imb_threshold_after_ovf_get(
    int unit,
    bcm_port_t port,
    uint32 fifo_size,
    uint32 *thr_after_ovf)
{
    SHR_FUNC_INIT_VARS(unit);
    /*
     * ASIC recommended 80% of fifo size*  
     */
    *thr_after_ovf = fifo_size * 8 / 10;

    SHR_FUNC_EXIT;
}

/**
 * \brief - calculate the weight to be set for the ETHU in the 
 * Low priority scheduler. the weight is set per ETHU - the 
 * bigger the weight, the ETHU will get more BW in the RR. 
 * 
 * see .h file
 */
shr_error_e
dnx_port_algo_imb_ethu_scheduler_weight_get(
    int unit,
    bcm_port_t port,
    int is_port_include,
    int *weight)
{
    int bandwidth;
    int nof_bits;
    int nof_ethu_schedulder_bits;
    SHR_FUNC_INIT_VARS(unit);

    nof_ethu_schedulder_bits = dnx_data_nif.scheduler.nof_weight_bits_get(unit);
    SHR_IF_ERR_EXIT(imb_bandwidth_get(unit, port, is_port_include, &bandwidth));

    if (bandwidth == 0)
    {
        nof_bits = 0;
    }
    else
    {
        /*
         * SW decision to add one bit in the scheduler for every 100G traffic on the ETHU
         */
        nof_bits = 1 + (bandwidth - 1) / dnx_data_nif.scheduler.rate_per_ethu_bit_get(unit);
    }

    /*
     * Number of bits should not exceed the number of ETHU scheduler bits
     */
    nof_bits = ((nof_bits > nof_ethu_schedulder_bits) ? nof_ethu_schedulder_bits : nof_bits);

    *weight = nof_bits;

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - 
 *   calculate the weight to be set for the ILU in the Low priority scheduler.
 *   The weight is set per bandwidth - the bigger the weight, the port/core
 *   will get more BW in the RR. 
 * 
 * see .h file
 */
shr_error_e
dnx_port_algo_imb_ilu_scheduler_weight_get(
    int unit,
    int bandwidth,
    int *nof_weight_bits)
{
    int nof_bits;
    int nof_ilu_schedulder_bits = dnx_data_nif.scheduler.nof_weight_bits_get(unit);
    SHR_FUNC_INIT_VARS(unit);

    if (bandwidth == 0)
    {
        nof_bits = 0;
    }
    else
    {
        /*
         * SW decision to add one bit in the scheduler for every 150G traffic on the ILU
         */
        nof_bits = 1 + (bandwidth - 1) / dnx_data_nif.scheduler.rate_per_ilu_bit_get(unit);
    }

    /*
     * Number of bits should not exceed the number of ILU scheduler bits
     */
    nof_bits = ((nof_bits > nof_ilu_schedulder_bits) ? nof_ilu_schedulder_bits : nof_bits);

    *nof_weight_bits = nof_bits;

    SHR_FUNC_EXIT;
}

/**
 * \brief - calculate the weight to be set for the RMC in the 
 * Low priority scheduler. the weight is set per RMC in the ETHU 
 * - the bigger the weight, the RMC will get more BW in the RR. 
 * 
 * see .h file
 */
shr_error_e
dnx_port_algo_imb_rmc_scheduler_weight_get(
    int unit,
    bcm_port_t port,
    int *weight)
{
    int speed;
    int nof_bits;
    int nof_rmc_schedulder_bits;
    dnx_algo_port_ethu_access_info_t ethu_info;
    uint32 rate_per_rmc_bit;
    SHR_FUNC_INIT_VARS(unit);

    nof_rmc_schedulder_bits = dnx_data_nif.scheduler.nof_rmc_bits_get(unit);
    SHR_IF_ERR_EXIT(dnx_algo_port_interface_rate_get(unit, port, DNX_ALGO_PORT_SPEED_F_RX, &speed));

    /*
     * Get ETHU access info
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_ethu_access_info_get(unit, port, &ethu_info));
    rate_per_rmc_bit = dnx_data_port.imb.imb_type_info_get(unit, ethu_info.imb_type)->rate_per_rmc_bit;

    /*
     * SW decision to add one bit in the scheduler for every 50G of the port speed
     */
    nof_bits = 1 + (speed - 1) / rate_per_rmc_bit;

    /*
     * Number of bits should not exceed the number of RMC scheduler bits
     */
    nof_bits = ((nof_bits > nof_rmc_schedulder_bits) ? nof_rmc_schedulder_bits : nof_bits);

    *weight = nof_bits;

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Allocate RMC ID.
 * 
 * see .h file
 */
shr_error_e
dnx_algo_port_imb_rmc_id_alloc(
    int unit,
    bcm_port_t port,
    int *rmc_id)
{
    shr_error_e rv;
    dnx_algo_port_ethu_access_info_t ethu_info;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Get ETHU access info 
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_ethu_access_info_get(unit, port, &ethu_info));

    /*
     * Allocate RMC ID
     */
    rv = dnx_algo_port_imb_db.rmc_id_res.allocate_single(unit, ethu_info.core, ethu_info.ethu_id_in_core, 0, NULL,
                                                         rmc_id);
    if (rv == _SHR_E_RESOURCE)
    {
        /** Resource error means no match - display error message */
        SHR_ERR_EXIT(_SHR_E_RESOURCE, "No free RMC id available.\n");
    }
    else
    {
        SHR_IF_ERR_EXIT(rv);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - algorithm to free the RMC id. 
 * 
 * see .h file
 */
shr_error_e
dnx_algo_port_imb_rmc_id_free(
    int unit,
    bcm_port_t port,
    int rmc_id)
{
    dnx_algo_port_ethu_access_info_t ethu_info;
    uint8 is_allocated;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Get ETHU access info 
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_ethu_access_info_get(unit, port, &ethu_info));

    /*
     * Free RMC Id
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_imb_db.
                    rmc_id_res.is_allocated(unit, ethu_info.core, ethu_info.ethu_id_in_core, rmc_id, &is_allocated));
    if (is_allocated == TRUE)
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_imb_db.
                        rmc_id_res.free_single(unit, ethu_info.core, ethu_info.ethu_id_in_core, rmc_id));
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Trying to free RMC ID %d, which is not allocated.\n", rmc_id);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Get number of free RMC IDs
 * 
 * see .h file
 */
shr_error_e
dnx_algo_port_imb_rmc_id_nof_free_elements_get(
    int unit,
    bcm_port_t port,
    int *nof_free_rmc_ids)
{
    dnx_algo_port_ethu_access_info_t ethu_info;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Get ETHU access info 
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_ethu_access_info_get(unit, port, &ethu_info));

    SHR_IF_ERR_EXIT(dnx_algo_port_imb_db.
                    rmc_id_res.nof_free_elements_get(unit, ethu_info.core, ethu_info.ethu_id_in_core,
                                                     nof_free_rmc_ids));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Create RMC ID resource manager.
 *   Some devices doesn't have enough RMCs for all ports, thus
 *   resource manager is needed.
 */
static shr_error_e
dnx_algo_port_imb_rmc_id_res_init(
    int unit)
{
    dnx_algo_res_create_data_t data;
    uint32 ethu_id_in_core;
    uint32 core_id;
    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&data, 0, sizeof(dnx_algo_res_create_data_t));
    data.first_element = 0;
    data.nof_elements = dnx_data_nif.eth.ethu_logical_fifo_nof_get(unit);
    sal_strncpy(data.name, DNX_ALGO_PORT_IMB_SCHEDULER_RMC_ID_RESOURCE, DNX_ALGO_RES_MNGR_MAX_NAME_LENGTH - 1);

    SHR_IF_ERR_EXIT(dnx_algo_port_imb_db.rmc_id_res.alloc(unit));
    for (core_id = 0; core_id < dnx_data_device.general.nof_cores_get(unit); core_id++)
    {
        for (ethu_id_in_core = 0; ethu_id_in_core < dnx_data_nif.eth.ethu_nof_per_core_get(unit); ethu_id_in_core++)
        {
            SHR_IF_ERR_EXIT(dnx_algo_port_imb_db.rmc_id_res.create(unit, core_id, ethu_id_in_core, &data, NULL));
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_port_imb_init(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    if (!sw_state_is_warm_boot(unit))
    {
        /*
         * Create SW State instance
         */
        SHR_IF_ERR_EXIT(dnx_algo_port_imb_db.init(unit));

        /*
         * Allocate SW STATE DBs  
         */
        SHR_IF_ERR_EXIT(dnx_algo_port_imb_rmc_id_res_init(unit));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - algorithm to calculate the first and last entry of a
 *        specific logical fifo in the CDU memory.
 * 
 * see .h file
 */
shr_error_e
dnx_port_algo_imb_rmc_entry_range_get(
    int unit,
    bcm_port_t port,
    int nof_entries,
    int prev_last_entry,
    int nof_prio_groups,
    uint32 *first_entry,
    uint32 *last_entry)
{
    int nof_phys, ethu_nof_lanes, ethu_nof_memory_entries;
    dnx_algo_port_ethu_access_info_t ethu_info;
    uint32 total_first_entry, total_last_entry, rmc_fifo_total_size;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * get port ETHU info
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_ethu_access_info_get(unit, port, &ethu_info));
    /*
     * get port nof phys
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_nif_phys_nof_get(unit, port, &nof_phys));

    ethu_nof_lanes = dnx_data_port.imb.imb_type_info_get(unit, ethu_info.imb_type)->nof_lanes;
    ethu_nof_memory_entries = dnx_data_port.imb.imb_type_info_get(unit, ethu_info.imb_type)->nof_memory_entries;
    /*
     * calculate first and last entry bounderies for all port's RMCs depending on first lane and nof lanes
     */
    total_first_entry = (ethu_info.first_lane_in_port * ethu_nof_memory_entries) / ethu_nof_lanes;
    total_last_entry = total_first_entry + ((nof_phys * ethu_nof_memory_entries) / ethu_nof_lanes) - 1;

    *first_entry = (prev_last_entry == -1) ? total_first_entry : prev_last_entry + 1;
    if (nof_entries == -1)
    {
        /*
         * number of entries should be divided equally between all RMCs
         */
        rmc_fifo_total_size = total_last_entry - total_first_entry;
        *last_entry = *first_entry + (rmc_fifo_total_size / nof_prio_groups);
    }
    else
    {
        /*
         * number of entries for the RMC is given specifically
         */
        *last_entry = *first_entry + nof_entries - 1;
    }
    if (*last_entry > total_last_entry)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Priority Group number of entries exceeds port %d FIFO limit", port);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - algorithm to calculate the the number of segments for specific ilkn port according to various parameters such as:
 *          device core clock, number of lanes, burst short size and serdes rate
 *
 * see .h file
 */
shr_error_e
dnx_algo_port_imb_ilkn_nof_segments_calc(
    int unit,
    int core_clk_khz,
    int num_lanes,
    int ilkn_burst_short,
    int serdes_speed,
    int *nof_segments)
{
    int core_clock = 0;
    int packet_size;
    int max_core_clock;
    int max_core_clock_dividend;
    int max_core_clock_divisor, lane2pipe_ratio_freq;
    int internal_pipes_calc;
    const int ui_f_width = 256;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Iterating with half and max nof segments to calculate the core clock which will be compared to the device frequency
     */
    for (*nof_segments = dnx_data_nif.ilkn.segments_half_nof_get(unit);
         *nof_segments <= dnx_data_nif.ilkn.segments_max_nof_get(unit);
         *nof_segments += dnx_data_nif.ilkn.segments_half_nof_get(unit))
    {
        for (packet_size = 1; packet_size < 516; packet_size++)
        {
            max_core_clock_dividend =
                ((UTILEX_DIV_ROUND_UP((packet_size * 8), ui_f_width) * 1000) / (*nof_segments)) * num_lanes *
                serdes_speed;
            if (((packet_size + 7) / 8) > (ilkn_burst_short / 8))
            {
                max_core_clock_divisor =
                    67 * ((UTILEX_DIV_ROUND_UP(packet_size, 8)) + (UTILEX_DIV_ROUND_UP(packet_size, 512)));
            }
            else
            {
                max_core_clock_divisor = 67 * ((ilkn_burst_short / 8) + (UTILEX_DIV_ROUND_UP(packet_size, 512)));
            }
            max_core_clock = UTILEX_DIV_ROUND(max_core_clock_dividend, max_core_clock_divisor);
            if (max_core_clock > core_clock)
            {
                core_clock = max_core_clock;
            }
        }
        internal_pipes_calc = UTILEX_DIV_ROUND_UP((num_lanes * serdes_speed * 1000), (core_clk_khz * 67));
        lane2pipe_ratio_freq = (num_lanes * serdes_speed * 1000) / (internal_pipes_calc * 67);
        /*
         * Check if core clock is still bigger than the device frequency
         */
        if ((core_clock > core_clk_khz) || (lane2pipe_ratio_freq > core_clk_khz))
        {
            core_clock = 0;
            continue;
        }
        else
        {
            break;
        }
    }

    if (*nof_segments > dnx_data_nif.ilkn.segments_max_nof_get(unit))
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Number of segment is not valid/n");
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - segment is a resource of ILKN, ehich has to be 
 *        shared between the two ports of the same ILKN core.
 *        the division can be as follows:
 * 1. ILKN0 has 4 segments (ILKN1 is not active) 
 * 2. ILKN0 and ILKN1 get two segments each. 
 * the number of segment a port should get is determined by the 
 * number of lanes, lane rate, burst short value and core clock 
 * of the device. if there is not enough segments to support the 
 * port, an error should be returned. 
 */
shr_error_e
dnx_algo_port_imb_nof_segments_default_get(
    int unit,
    bcm_port_t port,
    int *nof_segments)
{
    dnx_algo_port_info_s port_info;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Verify
     */
    /** Verify logical port is valid */
    SHR_IF_ERR_EXIT(dnx_algo_port_valid_verify(unit, port));

    /** Get port type */
    SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, port, &port_info));

    /*
     * later on we should add a calculation here based on SLE excel file. 
     * but in Jr2, there is only one port in every ILKN core, so we can always return 4 segments. 
     * we can generally say that whenever there can be only one port per ILKN core,                                                     .
     * we can set nof segment to 4 and skip the calculation.
     */
    if (DNX_ALGO_PORT_TYPE_IS_NIF_ILKN(unit, port_info, 1 /* elk */ , 1 /* L1 */ ))
    {
        if ((dnx_data_nif.ilkn.ilkn_unit_if_nof_get(unit) == 1)
            || (DNX_ALGO_PORT_TYPE_IS_NIF_ILKN_ELK(unit, port_info)))
        {
            *nof_segments = dnx_data_nif.ilkn.segments_max_nof_get(unit);
        }
        else
        {
            /*
             * the exact number of segments will be later determined by the portmod according to number of lanes and
             * serdes rate 
             */
            *nof_segments = dnx_data_nif.ilkn.segments_half_nof_get(unit);
        }
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Number of segment is a supported on ILKN ports only. port %d is not ILKN", port);
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * see .h file
 */

int
dnx_algo_port_imb_ethu_port_pm_boundary_get(
    int unit,
    bcm_port_t port,
    int *lower_bound,
    int *upper_bound)
{
    int first_phy;
    int table_index, pm_index;
    bcm_pbmp_t pm_phys;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Verify
     */
    /** Verify logical port is valid */
    SHR_IF_ERR_EXIT(dnx_algo_port_valid_verify(unit, port));

    SHR_IF_ERR_EXIT(dnx_algo_port_nif_first_phy_get(unit, port, 0, &first_phy));
    table_index = first_phy / dnx_data_nif.eth.phy_map_granularity_get(unit);
    pm_index = dnx_data_nif.eth.phy_map_get(unit, table_index)->pm_index;
    pm_phys = dnx_data_nif.eth.pm_properties_get(unit, pm_index)->phys;
    _SHR_PBMP_FIRST(pm_phys, *lower_bound);
    _SHR_PBMP_LAST(pm_phys, *upper_bound);

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - calculate the credit value from TMC to EGQ.
 *
 * see .h file
 */
shr_error_e
dnx_algo_imb_feu_tx_credit_val_get(
    int unit,
    int speed,
    int *credit_val)
{
    SHR_FUNC_INIT_VARS(unit);
    /*
     * ASIC recommended 32 * (client_rate / 5G)
     */
    *credit_val = 32 * speed / 5000;

    SHR_FUNC_EXIT;
}

#undef _ERR_MSG_MODULE_NAME
