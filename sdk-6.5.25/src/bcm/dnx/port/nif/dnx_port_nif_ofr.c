/**
 *
 * \file dnx_port_nif_ofr.c
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2021 Broadcom Inc. All rights reserved.
 *
 * NIF OFR procedures for DNX.
 *
 */

/*
 * Include files:
 * {
 */

#include "dnx_port_nif_ofr_internal.h"
#include "dnx_port_nif_link_list_internal.h"
#include "dnx_port_nif_calendar_internal.h"

#include <soc/dnx/dbal/dbal.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_nif.h>
#include <bcm_int/dnx/algo/port/algo_port_mgmt.h>
#include <bcm_int/dnx/port/imb/imb_common.h>
#include <bcm_int/dnx/port/nif/dnx_port_nif_ofr.h>
#include <shared/shrextend/shrextend_error.h>
#include <shared/shrextend/shrextend_debug.h>
#include <shared/error.h>
#include <soc/dnx/swstate/auto_generated/access/dnx_port_nif_access.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/access/dnx_algo_port_access.h>

/*
 * }
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_PORT

typedef struct
{
    int free_slots;
    uint8 nof_groups;
    uint8 *mem_groups;
    float avg;
} dnx_port_nif_ofr_priority_allocation_info_t;

shr_error_e
dnx_port_ofr_init(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    
    SHR_IF_ERR_EXIT(dnx_port_ofr_enable_dynamic_memories_set(unit, TRUE));

    /*
     * Initialize OFR sw state.
     */
    SHR_IF_ERR_EXIT(dnx_port_ofr_info_init(unit));

    /*
     * Take out of reset the Async Unit logic
     */
    SHR_IF_ERR_EXIT(dnx_port_ofr_async_unit_enable(unit, TRUE));

    /*
     * Set PRD bypass mode to 0.
     * This is a debug feature to bypass the PRD parser, and it is turned on by default.
     * it should not be exposed as API.
     * this will enable working with the PRD later on.
     */
    SHR_IF_ERR_EXIT(dnx_port_ofr_prd_bypass_hw_set(unit, FALSE));
    /*
     * Initialize ofr link list.
     */
    SHR_IF_ERR_EXIT(dnx_port_link_list_init
                    (unit, DNX_PORT_NIF_OFR_LINK_LIST, dnx_data_nif.ofr.nof_rx_memory_sections_get(unit),
                     dnx_data_nif.ofr.nof_total_rmcs_get(unit)));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_port_ofr_power_down(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(dnx_port_ofr_power_down_set(unit, TRUE));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_port_ofr_port_add(
    int unit,
    bcm_port_t port)
{
    dnx_algo_port_info_s port_info;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, port, &port_info));
    /*
     * For Eth ports - Configure in HW port parameters; has TDM/ Frame Preemption
     */
    if (DNX_ALGO_PORT_TYPE_IS_NIF_ETH(unit, port_info, DNX_ALGO_PORT_TYPE_INCLUDE(STIF, L1)))
    {
        SHR_IF_ERR_EXIT(dnx_port_ofr_port_params_set(unit, port));
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_port_ofr_port_priority_config_get(
    int unit,
    bcm_port_t port,
    bcm_port_prio_config_t * priority_config)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_port_ofr_port_priority_config_internal_get(unit, port, priority_config));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Validate imb_ethu_port_priority_config_set params
 */
static int
dnx_port_ofr_port_priority_config_verify(
    int unit,
    bcm_port_t port,
    const bcm_port_prio_config_t * priority_config)
{
    int nof_prio_groups_supported, ii, nof_entries, prd_priority_mask, rmc_id, prd_prio;
    int prio_found[DNX_DATA_MAX_NIF_ETH_PRIORITY_GROUPS_NOF] = { 0 };
    uint32 profile_rmc_group[DNX_DATA_MAX_NIF_PRD_NOF_PRIORITIES], rmc_group, prd_profile;
    uint8 rmc_group_found = FALSE, is_first_port_on_profile = TRUE;
    bcm_port_nif_scheduler_t sch_prio;
    SHR_FUNC_INIT_VARS(unit);

    nof_prio_groups_supported = dnx_data_nif.eth.priority_groups_nof_get(unit);

    if (priority_config->nof_priority_groups > nof_prio_groups_supported)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Number of priority groups %d is not supported\n",
                     priority_config->nof_priority_groups);
    }

    SHR_IF_ERR_EXIT(dnx_algo_port_prd_profile_get(unit, port, &prd_profile));
    SHR_IF_ERR_EXIT(dnx_ofr_profile_rx_priority_to_rmc_map_get(unit, prd_profile, profile_rmc_group));

    /** NIF ports */
    SHR_IF_ERR_EXIT(dnx_ofr_prd_is_single_port_on_profile_get(unit, port, &is_first_port_on_profile));

    for (ii = 0; ii < priority_config->nof_priority_groups; ++ii)
    {
        nof_entries = priority_config->priority_groups[ii].num_of_entries;
        sch_prio = priority_config->priority_groups[ii].sch_priority;
        prd_priority_mask = priority_config->priority_groups[ii].source_priority;

        /*
         * Supported values for num_of_entries is -1 or > dnx_data_nif.eth.priority_group_nof_entries_min
         */
        if (nof_entries < dnx_data_nif.eth.priority_group_nof_entries_min_get(unit) && nof_entries != -1)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Number of entries %d is invalid\n", nof_entries);
        }
        if (prio_found[sch_prio])
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Having two priority groups with the same scheduler priority is not supported\n");
        }
        prio_found[sch_prio] = 1;

        if (prd_priority_mask == 0)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Priority group %d is not mapped to any source priority\n", ii);
        }
        else if (prd_priority_mask & (~IMB_PRD_PRIORITY_ALL))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Priority group %d source priority is invalid\n", ii);
        }

        if ((prd_profile != DNX_PORT_NIF_OFR_DEFAULT_PRD_PROFILE) && !is_first_port_on_profile)
        {
            SHR_IF_ERR_EXIT(dnx_port_ofr_rmc_id_get(unit, port, priority_config, ii, &rmc_id));
            rmc_group = rmc_id < dnx_data_nif.ofr.nof_rmc_per_priority_group_get(unit) ? 0 : 1;

            rmc_group_found = FALSE;
            for (prd_prio = 0; prd_prio < dnx_data_nif.prd.nof_priorities_get(unit); prd_prio++)
            {
                if (rmc_group == profile_rmc_group[prd_prio])
                {
                    rmc_group_found = TRUE;
                    break;
                }
            }
            if (!rmc_group_found)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Trying to change profile which other ports are already assigned to\n");
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief - Set the priority group configuration into SW db.
 * priority group translate to a RMC (logical fifo)
 *
 * \param [in] unit - chip unit id.
 * \param [in] port - local id
 * \param [in] priority_config - priority group specification
 *
 * \return
 *   int
 *
 * \remarks
 *   * None
 * \see
 *   * bcm_port_prio_config_t
 */
static int
dnx_port_ofr_port_priority_config_db_update(
    int unit,
    bcm_port_t port,
    const bcm_port_prio_config_t * priority_config)
{
    int priority_group;
    dnx_algo_port_rmc_info_t rmc;
    SHR_FUNC_INIT_VARS(unit);

    for (priority_group = 0; priority_group < priority_config->nof_priority_groups; ++priority_group)
    {
        sal_memset(&rmc, 0, sizeof(dnx_algo_port_rmc_info_t));
        /*
         * copy logical fifo priorities from user configuration
         */
        rmc.sch_priority = priority_config->priority_groups[priority_group].sch_priority;
        rmc.prd_priority = priority_config->priority_groups[priority_group].source_priority;

        /*
         * get relevant RMC id
         */
        SHR_IF_ERR_EXIT(dnx_port_ofr_rmc_id_get(unit, port, priority_config, priority_group, &rmc.rmc_id));
        SHR_IF_ERR_EXIT(dnx_algo_port_nif_logical_fifo_set(unit, port, 0, priority_group, &rmc));
    }
exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_port_ofr_most_vacant_memory_group_get(
    int unit,
    int *mem_group)
{
    int max_size = 0, size, memory_group;
    SHR_FUNC_INIT_VARS(unit);

    for (memory_group = 0; memory_group < dnx_data_nif.ofr.nof_rx_memory_groups_get(unit); ++memory_group)
    {
        SHR_IF_ERR_EXIT(dnx_port_nif_db.ofr_link_list.list_info.size.get(unit, memory_group, &size));
        if (size > max_size)
        {
            max_size = size;
            *mem_group = memory_group;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_port_ofr_double_link_list_calc(
    int unit,
    bcm_port_t port,
    int nof_sections,
    uint8 **alloc_info)
{
    bcm_port_prio_config_t priority_config;
    dnx_algo_port_if_tdm_mode_e if_tdm_mode;
    int port_is_tdm = 0, alloc_index = 0;
    dnx_port_nif_ofr_priority_allocation_info_t *priority_allocation_config = NULL;
    uint8 priority_with_no_groups_found;
    int nof_sections_in_group;
    int size, nof_groups_assinged;
    int priority_group, iter, most_vacant_group, priority_group_assigned;
    int *groups_free_size = NULL;
    int remain_to_alloc, nof_groups_to_alloc;
    int location, memory_group_id, size_in_memory_group, nof_section_to_allocate_in_group;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Get number of priorities for this client 
     */
    SHR_IF_ERR_EXIT(dnx_port_ofr_port_priority_config_get(unit, port, &priority_config));

    SHR_ALLOC_SET_ZERO(priority_allocation_config,
                       (sizeof(dnx_port_nif_ofr_priority_allocation_info_t) * priority_config.nof_priority_groups),
                       "priority allocation config", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);
    SHR_NULL_CHECK(priority_allocation_config, _SHR_E_MEMORY, "priority_allocation_config");

    /*
     * Init the priority allocation info struct 
     */
    for (priority_group = 0; priority_group < priority_config.nof_priority_groups; ++priority_group)
    {
        nof_sections_in_group =
            (priority_config.priority_groups[priority_group].num_of_entries ==
             -1) ? nof_sections : priority_config.priority_groups[priority_group].num_of_entries;
        priority_allocation_config[priority_group].free_slots = 0 - nof_sections_in_group;
        priority_allocation_config[priority_group].nof_groups = 0;
        SHR_ALLOC_SET_ZERO(priority_allocation_config[priority_group].mem_groups,
                           sizeof(uint8) * dnx_data_nif.ofr.nof_rx_memory_groups_get(unit),
                           "memeory groups allocated per priority", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);
    }
    for (iter = 0; iter < dnx_data_nif.ofr.nof_rx_memory_groups_get(unit); ++iter)
    {
        /*
         * Choose the most vacant group
         */
        SHR_IF_ERR_EXIT(dnx_port_ofr_most_vacant_memory_group_get(unit, &most_vacant_group));

        /*
         * If a priority with num_groups=0 exists assign the group to it.
         */
        priority_with_no_groups_found = FALSE;
        for (priority_group = 0; priority_group < priority_config.nof_priority_groups; ++priority_group)
        {
            if (priority_allocation_config[priority_group].nof_groups == 0)
            {
                priority_with_no_groups_found = TRUE;
                break;
            }
        }
        if (priority_with_no_groups_found)
        {
            priority_group_assigned = priority_group;
        }
        else
        {
            /*
             * If we have 2 priorities we compare the averages 
             */
            if (priority_config.nof_priority_groups > 1)
            {
                priority_group_assigned =
                    (priority_allocation_config[0].avg <= priority_allocation_config[1].avg) ? 0 : 1;

            }
            else
            {
                priority_group_assigned = 0;
            }
        }
        /*
         * Assign the memory group to the priority - update priority allocation info
         */
        SHR_IF_ERR_EXIT(dnx_port_nif_db.ofr_link_list.list_info.size.get(unit, most_vacant_group, &size));
        nof_groups_assinged = priority_allocation_config[priority_group_assigned].nof_groups;
        priority_allocation_config[priority_group_assigned].free_slots += size;
        priority_allocation_config[priority_group_assigned].mem_groups[nof_groups_assinged] = most_vacant_group;
        priority_allocation_config[priority_group_assigned].nof_groups = ++nof_groups_assinged;
        priority_allocation_config[priority_group_assigned].avg =
            priority_allocation_config[priority_group_assigned].free_slots / nof_groups_assinged;
    }

    /*
     * compute allocation info - how many section to allocate in each memory group per priority 
     */

    SHR_IF_ERR_EXIT(dnx_algo_port_if_tdm_mode_get(unit, port, &if_tdm_mode));
    port_is_tdm = (if_tdm_mode == DNX_ALGO_PORT_IF_NO_TDM) ? 0 : 1;

    for (priority_group = 0; priority_group < priority_config.nof_priority_groups; ++priority_group)
    {
        nof_sections_in_group =
            (priority_config.priority_groups[priority_group].num_of_entries ==
             -1) ? nof_sections : priority_config.priority_groups[priority_group].num_of_entries;

        SHR_ALLOC_ERR_EXIT(groups_free_size, (sizeof(int) * priority_allocation_config[priority_group].nof_groups),
                           "Array of free size in each memory group used for this priority", "%s%s%s\r\n", EMPTY, EMPTY,
                           EMPTY);

        remain_to_alloc = nof_sections_in_group;
        nof_groups_to_alloc = priority_allocation_config[priority_group].nof_groups;

        if (priority_config.nof_priority_groups > 1)
        {
            if (port_is_tdm)
            {
                alloc_index =
                    (DNX_PORT_NIF_OFR_PRIORITY_IS_TDM(priority_config.priority_groups[priority_group])) ? 0 : 1;
            }
            else
            {
                alloc_index =
                    (DNX_PORT_NIF_OFR_PRIORITY_IS_HIGH(priority_config.priority_groups[priority_group])) ? 0 : 1;
            }
        }

        for (iter = 0; iter < nof_groups_to_alloc; iter++)
        {
            SHR_IF_ERR_EXIT(dnx_port_nif_db.ofr_link_list.list_info.
                            size.get(unit, priority_allocation_config[priority_group].mem_groups[iter],
                                     &groups_free_size[iter]));
        }

        while (nof_groups_to_alloc > 0)
        {
            /*
             * Find the the group with the minimum free place 
             */
            location = 0;
            for (iter = 1; iter < priority_allocation_config[priority_group].nof_groups; iter++)
            {
                if (groups_free_size[iter] < groups_free_size[location])
                {
                    location = iter;
                }
            }
            memory_group_id = priority_allocation_config[priority_group].mem_groups[location];
            size_in_memory_group = groups_free_size[location];

            nof_section_to_allocate_in_group = UTILEX_MIN(size_in_memory_group, remain_to_alloc / nof_groups_to_alloc);
            alloc_info[alloc_index][memory_group_id] = nof_section_to_allocate_in_group;
            nof_groups_to_alloc--;
            remain_to_alloc -= nof_section_to_allocate_in_group;
            groups_free_size[location] = dnx_data_nif.ofr.nof_rx_mem_sections_per_group_get(unit) + 1;
        }

        if (remain_to_alloc > 0)
        {
            SHR_ERR_EXIT(_SHR_E_RESOURCE, "Not enough free sections to allocate. alloc_size: %d.",
                         nof_sections_in_group);
        }

        SHR_FREE(groups_free_size);
    }

exit:
    if (priority_allocation_config != NULL)
    {
        SHR_FREE(priority_allocation_config);
    }

    if (groups_free_size != NULL)
    {
        SHR_FREE(groups_free_size);
    }
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_port_ofr_rx_mem_link_list_set(
    int unit,
    bcm_port_t port,
    int enable)
{
    int speed, nof_sections;
    int nif_interface_id, core, priority_group;
    bcm_port_prio_config_t priority_config;
    uint8 **alloc_info = NULL;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Get OFR context
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_nif_interface_id_get(unit, port, 0, &core, &nif_interface_id));

    if (enable)
    {

        /*
         * Get number of priorities for this client
         */
        SHR_IF_ERR_EXIT(dnx_port_ofr_port_priority_config_get(unit, port, &priority_config));
        /*
         * Get nof_sections from port's speed
         */
        SHR_IF_ERR_EXIT(dnx_algo_port_speed_get(unit, port, DNX_ALGO_PORT_SPEED_F_RX, &speed));
        nof_sections = speed / dnx_data_nif.ofr.rx_memory_link_list_speed_granularity_get(unit);

        /*
         * Decide per priority group which memory sections to use : TBD
         * The OFR RX buffer is carved into 640 sections of 512B .
         * We have a total of 640 units, in a link list based on memories. splitted into 4 groups.
         * Please notice that in case of a port working with 2 priorities  (High and Low, TDM and Non-TDM, Express and non express)
         * We have to set the RMC units at two spareted groups.
         * For example the TDM will use units at the range of 0-159 and the non-tdm will use units at the range of 160-319 and 480-639.
         */
        /*
         * Allocate 2 link lists - 1 per priority group : TBD
         */
        SHR_IF_ERR_EXIT(dnx_port_link_list_alloc(unit, DNX_PORT_NIF_OFR_LINK_LIST, nof_sections, nif_interface_id));

        
        if (0)
        {
            SHR_ALLOC_SET_ZERO(alloc_info, sizeof(uint8 *) * priority_config.nof_priority_groups,
                               "Alloc info per priority", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);
            for (priority_group = 0; priority_group < priority_config.nof_priority_groups; priority_group++)
            {
                SHR_ALLOC_SET_ZERO(alloc_info[priority_group],
                                   sizeof(uint8) * dnx_data_nif.ofr.nof_rx_memory_groups_get(unit),
                                   "Alloc info per priority", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);
            }

            /*
             * If single priority: Distribute the entries among all 4 groups with "best balance"
             */
            SHR_IF_ERR_EXIT(dnx_port_ofr_double_link_list_calc(unit, port, nof_sections, alloc_info));

            /*
             * SHR_IF_ERR_EXIT(dnx_port_double_link_list_alloc(unit, DNX_PORT_NIF_OFR_LINK_LIST, alloc_info,
             * nif_interface_id));
             */

        }
    }
    else
    {
        /*
         * Free link list
         */
        SHR_IF_ERR_EXIT(dnx_port_link_list_free(unit, DNX_PORT_NIF_OFR_LINK_LIST, nif_interface_id));
    }

exit:
    sal_free(alloc_info);
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_port_ofr_port_priority_hw_set(
    int unit,
    bcm_port_t port,
    const bcm_port_prio_config_t * priority_config)
{
    dnx_algo_port_info_s port_info;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, port, &port_info));
    /*
     * For Eth ports - SET RMC configuration according to port priority configuration
     */
    if (DNX_ALGO_PORT_TYPE_IS_NIF_ETH(unit, port_info, DNX_ALGO_PORT_TYPE_INCLUDE(STIF, L1)))
    {
        SHR_IF_ERR_EXIT(dnx_port_ofr_rmc_config_set(unit, port, priority_config));
    }
    /*
     * Enable Scheduler RR request bit per context
     */
    SHR_IF_ERR_EXIT(dnx_port_ofr_rx_sch_config_set(unit, port, priority_config, TRUE));

    /*
     * Take out of reset  the quantum  counter for this context
     */
    SHR_IF_ERR_EXIT(dnx_port_ofr_quantum_counter_reset(unit, port, priority_config, IMB_COMMON_OUT_OF_RESET));

    /*
     * Set the OFR RX memory linked list according to the priority config
     */
    SHR_IF_ERR_EXIT(dnx_port_ofr_rx_mem_link_list_set(unit, port, TRUE));

    /*
     * Configure the Port RMcs Threshold after overflow
     */
    SHR_IF_ERR_EXIT(dnx_port_ofr_rmc_thresholds_set(unit, port, priority_config));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Map PRD priority to RMC
 */
int
dnx_ofr_port_rx_priority_to_rmc_config_set(
    int unit,
    bcm_port_t port,
    dnx_algo_port_rmc_info_t * rmc_arr,
    int nof_rmcs)
{
    int ii;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * change the priority mapping
     */
    for (ii = 0; ii < nof_rmcs; ++ii)
    {
        if (rmc_arr[ii].rmc_id == IMB_COSQ_RMC_INVALID)
        {
            continue;
        }
        /*
         * Map priorities to RMC
         */
        SHR_IF_ERR_EXIT(dnx_ofr_internal_port_rx_priority_to_rmc_map(unit, port, &rmc_arr[ii]));
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_port_ofr_port_priority_config_set(
    int unit,
    bcm_port_t port,
    const bcm_port_prio_config_t * priority_config)
{
    int port_has_speed = 0, priority_group;
    dnx_algo_port_rmc_info_t *rmc_arr = NULL;
    SHR_FUNC_INIT_VARS(unit);

    SHR_ALLOC_SET_ZERO(rmc_arr, priority_config->nof_priority_groups * sizeof(dnx_algo_port_rmc_info_t), "RMC info",
                       "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

    /*
     * verify data is correct
     */
    SHR_IF_ERR_EXIT(dnx_port_ofr_port_priority_config_verify(unit, port, priority_config));

    /*
     * update new configuration on SW DB
     */
    SHR_IF_ERR_EXIT(dnx_port_ofr_port_priority_config_db_update(unit, port, priority_config));

    /*
     * Fill in the RMC info array for this port 
     */
    for (priority_group = 0; priority_group < priority_config->nof_priority_groups; ++priority_group)
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_nif_logical_fifo_get(unit, port, 0, priority_group, &rmc_arr[priority_group]));
    }

    /*
     * Map port profile PRD priorities to port RMCs
     */
    SHR_IF_ERR_EXIT(dnx_ofr_port_rx_priority_to_rmc_config_set
                    (unit, port, rmc_arr, priority_config->nof_priority_groups));

    SHR_IF_ERR_EXIT(dnx_algo_port_has_speed(unit, port, 0, &port_has_speed));
    if (port_has_speed)
    {
        /*
         * Set configuration to HW
         */
        SHR_IF_ERR_EXIT(dnx_port_ofr_port_priority_hw_set(unit, port, priority_config));
    }

exit:
    SHR_FREE(rmc_arr);
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_port_ofr_port_remove(
    int unit,
    bcm_port_t port)
{
    SHR_FUNC_INIT_VARS(unit);
    /*
     * Do nothing.
     */
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_port_ofr_rx_sch_context_set(
    int unit,
    bcm_port_t port)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Set the RX sch quantum
     */
    SHR_IF_ERR_EXIT(dnx_port_ofr_rx_sch_context_quantum_value_set(unit, port));

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_port_ofr_rx_sch_context_enable(
    int unit,
    bcm_port_t port,
    int enable)
{
    dnx_algo_port_info_s port_info;
    bcm_port_prio_config_t priority_config;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, port, &port_info));

    if ((DNX_ALGO_PORT_TYPE_IS_NIF_ETH(unit, port_info, DNX_ALGO_PORT_TYPE_INCLUDE(STIF, L1))) ||
        (DNX_ALGO_PORT_TYPE_IS_FLEXE_MAC(unit, port_info, FALSE)))
    {
        /*
         * get priorities of this port
         */
        SHR_IF_ERR_EXIT(dnx_port_ofr_port_priority_config_get(unit, port, &priority_config));
        /*
         * Set the Deficit RR configuration
         */
        SHR_IF_ERR_EXIT(dnx_port_ofr_deficit_rr_config_set(unit, port, &priority_config, enable));

        /*
         * Reset RX sch for this context according to High/Low
         */
        SHR_IF_ERR_EXIT(dnx_port_ofr_rx_sch_context_reset
                        (unit, port, &priority_config, (enable) ? IMB_COMMON_OUT_OF_RESET : IMB_COMMON_IN_RESET));

        /*
         * Enable quantum load for this context according to High/Low
         */
        SHR_IF_ERR_EXIT(dnx_port_ofr_quantum_load_enable(unit, port, &priority_config, enable));
    }
    /*
     * Take out of reset transaction counter
     */
    SHR_IF_ERR_EXIT(dnx_port_ofr_transaction_counter_reset
                    (unit, port, ((enable) ? IMB_COMMON_OUT_OF_RESET : IMB_COMMON_IN_RESET)));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_port_ofr_port_enable(
    int unit,
    bcm_port_t port,
    int enable)
{
    int in_reset;
    bcm_port_prio_config_t priority_config;
    dnx_algo_port_info_s port_info;
    SHR_FUNC_INIT_VARS(unit);

    in_reset = (enable) ? IMB_COMMON_OUT_OF_RESET : IMB_COMMON_IN_RESET;

    SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, port, &port_info));

    if ((DNX_ALGO_PORT_TYPE_IS_NIF_ETH(unit, port_info, DNX_ALGO_PORT_TYPE_INCLUDE(STIF, L1))) ||
        (DNX_ALGO_PORT_TYPE_IS_FLEXE_MAC(unit, port_info, FALSE)))
    {
        /*
         * get priorities of this port
         */
        SHR_IF_ERR_EXIT(dnx_port_ofr_port_priority_config_get(unit, port, &priority_config));
        /*
         * Take out of reset  the Gear shift for this context
         */
        SHR_IF_ERR_EXIT(dnx_port_ofr_gear_shift_reset(unit, port, in_reset));

        /*
         * Take out of reset  the Gear shift for this context
         */
        SHR_IF_ERR_EXIT(dnx_port_ofr_unpack_logic_reset(unit, port, in_reset));

        /*
         * Reset RX FIFOs for this context
         */
        SHR_IF_ERR_EXIT(dnx_port_ofr_rx_fifo_reset(unit, port, &priority_config, in_reset));
    }
    /*
     * Enable RX Schedule for this context
     */
    SHR_IF_ERR_EXIT(dnx_port_ofr_rx_sch_context_enable(unit, port, enable));

    if (DNX_ALGO_PORT_TYPE_IS_FLEXE_SAR(unit, port_info))
    {
        /*
         * Set SAR scheduler request enable
         */
        SHR_IF_ERR_EXIT(dnx_port_ofr_sar_scheduler_request_enable_set(unit, enable));

        /*
         * Set SAR scheduler transaction count enable
         */
        SHR_IF_ERR_EXIT(dnx_port_ofr_sar_scheduler_transaction_cnt_reset_set(unit, in_reset));
    }
exit:
    SHR_FUNC_EXIT;
}
