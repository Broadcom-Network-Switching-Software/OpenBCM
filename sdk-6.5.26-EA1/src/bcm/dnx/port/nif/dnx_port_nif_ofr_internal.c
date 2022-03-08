/*
 *
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 *
 */

#include "dnx_port_nif_ofr_internal.h"
#include "dnx_port_nif_link_list_internal.h"
#include <soc/dnx/dbal/dbal.h>
#include <bcm_int/dnx/algo/port/algo_port_mgmt.h>
#include <bcm_int/dnx/port/imb/imb_common.h>
#include <bcm_int/dnx/algo/port/algo_port_imb.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_nif.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_tdm.h>
#include <soc/dnx/dbal/auto_generated/dbal_defines_enum_field_types.h>
#include <soc/dnx/swstate/auto_generated/types/dnx_port_nif_types.h>
#include <soc/dnx/swstate/auto_generated/access/dnx_port_nif_access.h>
#include <soc/dnx/swstate/auto_generated/access/dnx_port_imb_access.h>

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_PORT

#define DNX_PORT_NIF_OFR_NOF_BYTES_PER_MEMORY_SECTION   (64)

shr_error_e
dnx_port_ofr_info_init(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_port_ofr_high_priority_to_ire_channel_set(
    int unit)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_OFR_MAP_RMC_MACHINE, &entry_handle_id));

    /*
     * set value field
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_HIGH_ON_CH_1, INST_SINGLE, TRUE);
    /*
     * commit value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_port_ofr_power_down_set(
    int unit,
    int enable)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_OFR_POWER_DOWN, &entry_handle_id));
    /*
     * set key fields
     */
    /*
     * set value field
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_POWER_DOWN, INST_SINGLE, enable);
    /*
     * commit value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_port_ofr_sar_scheduler_request_enable_set(
    int unit,
    int enable)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_OFR_SAR_RX_SCHEDULER, &entry_handle_id));

    /*
     * set value field
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_REQUEST_ENABLE, INST_SINGLE, enable);

    /*
     * commit value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_port_ofr_sar_scheduler_transaction_cnt_reset_set(
    int unit,
    int in_reset)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_OFR_SAR_RX_SCHEDULER, &entry_handle_id));

    /*
     * set value field
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TRANSACTION_COUNTER_RESET, INST_SINGLE, in_reset ?
                                 DBAL_ENUM_FVAL_NIF_RESET_REVERSE_LOGIC_IN_RESET :
                                 DBAL_ENUM_FVAL_NIF_RESET_REVERSE_LOGIC_OUT_OF_RESET);

    /*
     * commit value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_port_ofr_async_unit_enable(
    int unit,
    int enable)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_OFR_ASYNC_UNIT_CTRL, &entry_handle_id));
    /*
     * set key fields
     */
    /*
     * set value field
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ENABLE_ASYNC_UNIT, INST_SINGLE, enable);
    /*
     * commit value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_port_ofr_gear_shift_reset(
    int unit,
    bcm_port_t port,
    int in_reset)
{
    int nif_interface_id, core;
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_nif_interface_id_get(unit, port, 0, &core, &nif_interface_id));

    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_OFR_GEAR_SHIFT_RESET, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OFR_CONTEXT, nif_interface_id);
    /*
     * set value field
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_GS_RESET, INST_SINGLE, in_reset ?
                                 DBAL_ENUM_FVAL_NIF_RESET_REVERSE_LOGIC_IN_RESET :
                                 DBAL_ENUM_FVAL_NIF_RESET_REVERSE_LOGIC_OUT_OF_RESET);
    /*
     * commit value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_port_ofr_sar_datapath_enable(
    int unit,
    int enable)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_OFR_PORT_SAR_DATAPATH_ENABLE, &entry_handle_id));

    /*
     * set value fields
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SAR_DATAPATH_ENABLE, INST_SINGLE, enable);

    /*
     * commit the value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_port_ofr_unpack_logic_reset(
    int unit,
    bcm_port_t port,
    int in_reset)
{
    int nif_interface_id, core;
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_nif_interface_id_get(unit, port, 0, &core, &nif_interface_id));

    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_OFR_UNPACK_LOGIC_RESET, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OFR_CONTEXT, nif_interface_id);
    /*
     * set value field
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESET, INST_SINGLE, in_reset ?
                                 DBAL_ENUM_FVAL_NIF_RESET_REVERSE_LOGIC_IN_RESET :
                                 DBAL_ENUM_FVAL_NIF_RESET_REVERSE_LOGIC_OUT_OF_RESET);
    /*
     * commit value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_port_ofr_rx_fifo_reset(
    int unit,
    bcm_port_t port,
    const bcm_port_prio_config_t * priority_config,
    int in_reset)
{
    int nif_interface_id, core, priority_group;
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_nif_interface_id_get(unit, port, 0, &core, &nif_interface_id));

    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_OFR_RX_FIFO_RESET, &entry_handle_id));

    for (priority_group = 0; priority_group < priority_config->nof_priority_groups; ++priority_group)
    {
        /*
         * set key fields
         */
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_RMC_ID,
                                   nif_interface_id +
                                   priority_group * dnx_data_nif.ofr.nof_rmc_per_priority_group_get(unit));
        /*
         * set value field
         */
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESET, INST_SINGLE, in_reset ?
                                     DBAL_ENUM_FVAL_NIF_RESET_REVERSE_LOGIC_IN_RESET :
                                     DBAL_ENUM_FVAL_NIF_RESET_REVERSE_LOGIC_OUT_OF_RESET);
        /*
         * commit value
         */
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_port_ofr_prd_bypass_hw_set(
    int unit,
    int bypass_enable)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_OFR_PRD_BYPASS, &entry_handle_id));
    /*
     * set key fields
     */
    /*
     * set value field
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PRD_BYPASS, INST_SINGLE, bypass_enable);
    /*
     * commit value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_port_ofr_quantum_counter_reset(
    int unit,
    bcm_port_t port,
    const bcm_port_prio_config_t * priority_config,
    int in_reset)
{
    int priority_group, priority = 0;
    int nif_interface_id, core;
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_nif_interface_id_get(unit, port, 0, &core, &nif_interface_id));

    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_OFR_RX_SCHEDULER_QUANTUM_COUNTER_RESET, &entry_handle_id));

    for (priority_group = 0; priority_group < priority_config->nof_priority_groups; ++priority_group)
    {
        if (DNX_PORT_NIF_OFR_PRIORITY_IS_LOW_OR_HIGH(priority_config->priority_groups[priority_group]))
        {
            priority = (DNX_PORT_NIF_OFR_PRIORITY_IS_LOW(priority_config->priority_groups[priority_group])) ? 1 : 0;

            /*
             * set key fields
             */
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_NIF_PORT_ID, nif_interface_id);
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PRIORITY, priority);
            /*
             * set value field
             */
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESET, INST_SINGLE, in_reset ?
                                         DBAL_ENUM_FVAL_NIF_RESET_REVERSE_LOGIC_IN_RESET :
                                         DBAL_ENUM_FVAL_NIF_RESET_REVERSE_LOGIC_OUT_OF_RESET);

            /*
             * commit value
             */
            SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
        }
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_port_ofr_port_params_set(
    int unit,
    bcm_port_t port)
{
    int is_tdm = 0, is_fp = 0;
    dnx_algo_port_info_s port_info;
    dnx_algo_port_if_tdm_mode_e if_tdm_mode;
    int nif_interface_id, core;
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, port, &port_info));

    SHR_IF_ERR_EXIT(dnx_algo_port_if_tdm_mode_get(unit, port, &if_tdm_mode));

    SHR_IF_ERR_EXIT(dnx_algo_port_nif_interface_id_get(unit, port, 0, &core, &nif_interface_id));

    is_tdm = (if_tdm_mode == DNX_ALGO_PORT_IF_NO_TDM) ? 0 : 1;

    
    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_OFR_PORT_PARAM, &entry_handle_id));
    /*
     * set key fields
     */

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_NIF_ETH_PORT_ID, nif_interface_id);
    /*
     * set value field
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PORT_HAS_TDM, INST_SINGLE, is_tdm);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PORT_HAS_XPRESS, INST_SINGLE, is_fp);
    /*
     * commit value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_port_ofr_rmc_machine_id_internal_get(
    int unit,
    bcm_port_t port,
    bcm_port_nif_scheduler_t sch_prio,
    int *rmc_machine_id)
{
    int is_tdm = 0;
    dnx_algo_port_if_tdm_mode_e if_tdm_mode;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_if_tdm_mode_get(unit, port, &if_tdm_mode));

    is_tdm = (if_tdm_mode == DNX_ALGO_PORT_IF_NO_TDM) ? 0 : 1;
    if (is_tdm)
    {
        *rmc_machine_id = (bcmPortNifSchedulerTDM == sch_prio) ? 0 : 1;
    }
    else
    {
        *rmc_machine_id = (bcmPortNifSchedulerHigh == sch_prio) ? 0 : 1;
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_port_ofr_rmc_id_get(
    int unit,
    bcm_port_t port,
    const bcm_port_prio_config_t * priority_config,
    int priority_group,
    int *rmc_id)
{
    int rmc_machine_id = 0;
    int nif_interface_id, core;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_nif_interface_id_get(unit, port, 0, &core, &nif_interface_id));
    SHR_IF_ERR_EXIT(dnx_port_ofr_rmc_machine_id_internal_get
                    (unit, port, priority_config->priority_groups[priority_group].sch_priority, &rmc_machine_id));

    *rmc_id = nif_interface_id + rmc_machine_id * dnx_data_nif.ofr.nof_rmc_per_priority_group_get(unit);

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_port_ofr_rmc_config_set(
    int unit,
    bcm_port_t port,
    const bcm_port_prio_config_t * priority_config)
{
    int is_tdm = 0, is_over_high = 0;
    dnx_algo_port_info_s port_info;
    dnx_algo_port_if_tdm_mode_e if_tdm_mode;
    int nif_interface_id, core, priority_group;
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, port, &port_info));

    SHR_IF_ERR_EXIT(dnx_algo_port_if_tdm_mode_get(unit, port, &if_tdm_mode));

    SHR_IF_ERR_EXIT(dnx_algo_port_nif_interface_id_get(unit, port, 0, &core, &nif_interface_id));

    is_tdm = (if_tdm_mode == DNX_ALGO_PORT_IF_NO_TDM) ? 0 : 1;

    if (if_tdm_mode == DNX_ALGO_PORT_IF_TDM_HYBRID)
    {
        for (priority_group = 0; priority_group < priority_config->nof_priority_groups; ++priority_group)
        {
            if (DNX_PORT_NIF_OFR_PRIORITY_IS_LOW_OR_HIGH(priority_config->priority_groups[priority_group]))
            {
                is_over_high =
                    (DNX_PORT_NIF_OFR_PRIORITY_IS_LOW(priority_config->priority_groups[priority_group])) ? 1 : 0;

                break;
            }
        }
    }
    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_OFR_ETH_RMC_SCHEDULER_CONFIG, &entry_handle_id));

    /*
     * set key fields
     */

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_RMC_ID, nif_interface_id);

    /*
     * set value field
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RMC_IS_TDM, INST_SINGLE, is_tdm);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TDM_OVER_HIGH, INST_SINGLE, is_over_high);
    /*
     * commit value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}
shr_error_e
dnx_port_ofr_rmc_thresholds_set(
    int unit,
    bcm_port_t port,
    const bcm_port_prio_config_t * priority_config)
{
    uint32 rmc_fifo_size = 0, nof_sections_allocated;
    dnx_algo_port_rmc_info_t rmc;
    int priority_group, core, nif_interface_id, priority_index = 0;
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_nif_interface_id_get(unit, port, 0, &core, &nif_interface_id));

    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_OFR_RMC_THRESHOLD, &entry_handle_id));

    for (priority_group = 0; priority_group < priority_config->nof_priority_groups; ++priority_group)
    {

        SHR_IF_ERR_EXIT(dnx_algo_port_nif_logical_fifo_get(unit, port, 0, priority_group, &rmc));
        /*
         * set key fields
         */
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_RMC_ID, rmc.rmc_id);

        SHR_IF_ERR_EXIT(dnx_port_ofr_rmc_machine_id_internal_get
                        (unit, port, priority_config->priority_groups[priority_group].sch_priority, &priority_index));
        SHR_IF_ERR_EXIT(dnx_port_link_list_db_double_priority_nof_sections_allocated_get
                        (unit, DNX_PORT_NIF_OFR_LINK_LIST, nif_interface_id, priority_index, &nof_sections_allocated));

        /*
         * threshold should be configured to 80% of fifo size
         * where fifo size =  number of linked_list sections for this port (RMC) * 64 [number of bytes per section]
         */
        /*
         * get threshold after ovf
         */
        rmc_fifo_size = nof_sections_allocated * DNX_PORT_NIF_OFR_NOF_BYTES_PER_MEMORY_SECTION;
        rmc.nof_entries = nof_sections_allocated;

        SHR_IF_ERR_EXIT(dnx_algo_port_imb_threshold_after_ovf_get(unit, port, rmc_fifo_size, &rmc.thr_after_ovf));
        SHR_IF_ERR_EXIT(dnx_algo_port_nif_logical_fifo_set(unit, port, 0, priority_group, &rmc));

        /*
         * set value field
         */
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_THR_AFTER_OVF, INST_SINGLE, rmc.thr_after_ovf);
        /*
         * commit value
         */
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_port_ofr_port_priority_config_internal_get(
    int unit,
    bcm_port_t port,
    bcm_port_prio_config_t * priority_config)
{
    int ii, nof_priority_groups;
    dnx_algo_port_rmc_info_t rmc;
    SHR_FUNC_INIT_VARS(unit);

    nof_priority_groups = dnx_data_nif.eth.priority_groups_nof_get(unit);

    priority_config->nof_priority_groups = 0;

    for (ii = 0; ii < nof_priority_groups; ++ii)
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_nif_logical_fifo_get(unit, port, 0, ii, &rmc));
        if (rmc.rmc_id == IMB_COSQ_RMC_INVALID)
        {
            /*
             * RMC is invalid
             */
            continue;
        }
        priority_config->priority_groups[ii].sch_priority = rmc.sch_priority;
        priority_config->priority_groups[ii].source_priority = rmc.prd_priority;
        priority_config->priority_groups[ii].num_of_entries = rmc.nof_entries;
        ++priority_config->nof_priority_groups;
    }
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_port_ofr_transaction_counter_reset(
    int unit,
    bcm_port_t port,
    int in_reset)
{
    dnx_algo_port_info_s port_info;
    dnx_algo_port_ilkn_access_info_t ilkn_access_info;
    dnx_algo_port_tdm_mode_e tdm_mode;
    bcm_port_prio_config_t priority_config;
    int nif_interface_id, core, priority_group;
    int priority;
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, port, &port_info));

    if ((DNX_ALGO_PORT_TYPE_IS_NIF_ETH(unit, port_info, DNX_ALGO_PORT_TYPE_INCLUDE(STIF, L1))) ||
        (DNX_ALGO_PORT_TYPE_IS_FRAMER_MAC(unit, port_info, FALSE, FALSE)))
    {
        SHR_IF_ERR_EXIT(dnx_port_ofr_port_priority_config_internal_get(unit, port, &priority_config));

        SHR_IF_ERR_EXIT(dnx_algo_port_nif_interface_id_get(unit, port, 0, &core, &nif_interface_id));

        /*
         * alloc DBAL table handle
         */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC
                        (unit, DBAL_TABLE_NIF_OFR_RX_SCHEDULER_TRANSACTION_COUNTER_RESET, &entry_handle_id));

        for (priority_group = 0; priority_group < priority_config.nof_priority_groups; ++priority_group)
        {
            if (DNX_PORT_NIF_OFR_PRIORITY_IS_LOW_OR_HIGH(priority_config.priority_groups[priority_group]))
            {
                priority = (DNX_PORT_NIF_OFR_PRIORITY_IS_LOW(priority_config.priority_groups[priority_group])) ? 1 : 0;

                /*
                 * set key fields
                 */
                dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OFR_CONTEXT, nif_interface_id);
                dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PRIORITY, priority);
                /*
                 * set value field
                 */
                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESET, INST_SINGLE, in_reset ?
                                             DBAL_ENUM_FVAL_NIF_RESET_REVERSE_LOGIC_IN_RESET :
                                             DBAL_ENUM_FVAL_NIF_RESET_REVERSE_LOGIC_OUT_OF_RESET);
                /*
                 * commit value
                 */
                SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
            }
        }
    }

    else if (DNX_ALGO_PORT_TYPE_IS_NIF_ILKN(unit, port_info, DNX_ALGO_PORT_TYPE_INCLUDE(L1)))
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_tdm_get(unit, port, &tdm_mode));
        SHR_IF_ERR_EXIT(dnx_algo_port_ilkn_access_info_get(unit, port, &ilkn_access_info));
        SHR_IF_ERR_EXIT(dnx_port_ofr_internal_ilkn_rx_counter_reset_set(unit, ilkn_access_info.ilkn_id,
                                                                        tdm_mode == DNX_ALGO_PORT_TDM_MODE_BYPASS,
                                                                        in_reset));
    }
    else if (DNX_ALGO_PORT_TYPE_IS_FRAMER_SAR(unit, port_info, TRUE))
    {
        /*
         * alloc DBAL table handle
         */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC
                        (unit, DBAL_TABLE_NIF_OFR_RX_SCHEDULER_SAR_TRANSACTION_COUNTER_RESET, &entry_handle_id));
        /*
         * set key fields
         */
        /*
         * set value field
         */
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESET, INST_SINGLE, in_reset ?
                                     DBAL_ENUM_FVAL_NIF_RESET_REVERSE_LOGIC_IN_RESET :
                                     DBAL_ENUM_FVAL_NIF_RESET_REVERSE_LOGIC_OUT_OF_RESET);
        /*
         * commit value
         */
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_port_ofr_rx_sch_context_reset(
    int unit,
    bcm_port_t port,
    const bcm_port_prio_config_t * priority_config,
    int in_reset)
{
    int priority_group, priority = 0;
    int nif_interface_id, core;
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_nif_interface_id_get(unit, port, 0, &core, &nif_interface_id));

    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_OFR_RX_SCHEDULER_CONTEXT_RESET, &entry_handle_id));

    for (priority_group = 0; priority_group < priority_config->nof_priority_groups; ++priority_group)
    {
        if (DNX_PORT_NIF_OFR_PRIORITY_IS_LOW_OR_HIGH(priority_config->priority_groups[priority_group]))
        {
            priority = (DNX_PORT_NIF_OFR_PRIORITY_IS_LOW(priority_config->priority_groups[priority_group])) ? 1 : 0;

            /*
             * set key fields
             */
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OFR_CONTEXT, nif_interface_id);
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PRIORITY, priority);
            /*
             * set value field
             */
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESET, INST_SINGLE, in_reset ?
                                         DBAL_ENUM_FVAL_NIF_RESET_REVERSE_LOGIC_IN_RESET :
                                         DBAL_ENUM_FVAL_NIF_RESET_REVERSE_LOGIC_OUT_OF_RESET);

            /*
             * commit value
             */
            SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
        }
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_port_ofr_quantum_load_enable(
    int unit,
    bcm_port_t port,
    const bcm_port_prio_config_t * priority_config,
    int enable)
{
    int priority_group, priority = 0;
    int nif_interface_id, core;
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_nif_interface_id_get(unit, port, 0, &core, &nif_interface_id));

    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_OFR_RX_SCHEDULER_DRR_CONFIG, &entry_handle_id));

    for (priority_group = 0; priority_group < priority_config->nof_priority_groups; ++priority_group)
    {
        if (DNX_PORT_NIF_OFR_PRIORITY_IS_LOW_OR_HIGH(priority_config->priority_groups[priority_group]))
        {
            priority = (DNX_PORT_NIF_OFR_PRIORITY_IS_LOW(priority_config->priority_groups[priority_group])) ? 1 : 0;

            /*
             * set key fields
             */
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_NIF_PORT_ID, nif_interface_id);
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PRIORITY, priority);
            /*
             * set value field
             */
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_QUANTUM_LOAD, INST_SINGLE, enable);

            /*
             * commit value
             */
            SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
        }
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_port_ofr_deficit_rr_config_set(
    int unit,
    bcm_port_t port,
    const bcm_port_prio_config_t * priority_config,
    int enable)
{
    int in_reset, priority_group, priority = 0;
    dnx_algo_port_info_s port_info;
    dnx_algo_port_if_tdm_mode_e if_tdm_mode;
    int nif_interface_id, core;
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, port, &port_info));

    SHR_IF_ERR_EXIT(dnx_algo_port_nif_interface_id_get(unit, port, 0, &core, &nif_interface_id));

    SHR_IF_ERR_EXIT(dnx_algo_port_if_tdm_mode_get(unit, port, &if_tdm_mode));

    in_reset = (enable) ? IMB_COMMON_OUT_OF_RESET : IMB_COMMON_IN_RESET;

    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_OFR_RX_SCHEDULER_DRR_CONFIG, &entry_handle_id));

    for (priority_group = 0; priority_group < priority_config->nof_priority_groups; ++priority_group)
    {
        if (DNX_PORT_NIF_OFR_PRIORITY_IS_LOW_OR_HIGH(priority_config->priority_groups[priority_group]))
        {
            priority = (DNX_PORT_NIF_OFR_PRIORITY_IS_LOW(priority_config->priority_groups[priority_group])) ? 1 : 0;
        }
        if (if_tdm_mode == DNX_ALGO_PORT_IF_TDM_HYBRID)
        {
            if ((DNX_ALGO_PORT_TYPE_IS_NIF_ETH(unit, port_info, DNX_ALGO_PORT_TYPE_INCLUDE(STIF, L1))) &&
                (DNX_PORT_NIF_OFR_PRIORITY_IS_TDM(priority_config->priority_groups[priority_group])))
            {
                priority = 1;   /* High priority should be in reset in case of TDM */
            }
        }
        /*
         * set key fields
         */
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_NIF_PORT_ID, nif_interface_id);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PRIORITY, priority);
        /*
         * set value field
         */
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESET, INST_SINGLE, in_reset ?
                                     DBAL_ENUM_FVAL_NIF_RESET_REVERSE_LOGIC_IN_RESET :
                                     DBAL_ENUM_FVAL_NIF_RESET_REVERSE_LOGIC_OUT_OF_RESET);

        /*
         * commit value
         */
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_port_ofr_rx_sch_context_quantum_value_set(
    int unit,
    bcm_port_t port,
    int value)
{
    int nif_interface_id, core;
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Get the NIF port Id
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_nif_interface_id_get(unit, port, 0, &core, &nif_interface_id));
    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_OFR_RX_SCEHDULER_CONTEXT_CONFIG, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_NIF_PORT_ID, nif_interface_id);
    /*
     * set value field
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_QUANTUM_VALUE, INST_SINGLE, value);

    /*
     * commit value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;

}

static shr_error_e
dnx_port_ofr_rx_sch_low_high_config_set(
    int unit,
    bcm_port_t port,
    const bcm_port_prio_config_t * priority_config,
    int priority_group,
    int enable)
{
    int nif_interface_id, core;
    uint32 entry_handle_id = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Get the NIF port Id
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_nif_interface_id_get(unit, port, 0, &core, &nif_interface_id));

    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_OFR_RX_SCHEDULER_REQUEST_ENABLE_CONFIG, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_NIF_PORT_ID, nif_interface_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PRIORITY,
                               (DNX_PORT_NIF_OFR_PRIORITY_IS_LOW
                                (priority_config->priority_groups[priority_group])) ? 1 : 0);
    /*
     * set value field
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_REQUEST_ENABLE, INST_SINGLE, enable);

    /*
     * commit value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_port_ofr_rx_sch_ilkn_is_high_set(
    int unit,
    bcm_port_t port,
    const bcm_port_prio_config_t * priority_config,
    int priority_group,
    int enable)
{
    dnx_algo_port_ilkn_access_info_t ilkn_access_info;
    uint32 entry_handle_id = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_ilkn_access_info_get(unit, port, &ilkn_access_info));

    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_OFR_ILKN_SCHEDULER_CONFIG, &entry_handle_id));

    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ILKN_ID, ilkn_access_info.ilkn_id);

    /*
     * set value fields
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_ILKN_HIGH, INST_SINGLE,
                                 DNX_PORT_NIF_OFR_PRIORITY_IS_HIGH(priority_config->priority_groups[priority_group]));
    /*
     * commit the value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_port_ofr_rx_sch_tdm_config_set(
    int unit,
    bcm_port_t port,
    const bcm_port_prio_config_t * priority_config,
    int priority_group,
    int enable)
{
    int nif_interface_id, core;
    uint32 entry_handle_id = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Get the NIF port Id
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_nif_interface_id_get(unit, port, 0, &core, &nif_interface_id));

    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_OFR_TDM_ETH_RX_SCHEDULER_REQUEST_ENABLE, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_NIF_ETH_PORT_ID, nif_interface_id);
    /*
     * set value field
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_REQUEST_ENABLE, INST_SINGLE, enable);
    /*
     * commit value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_port_ofr_rx_sch_ilkn_request_config_set(
    int unit,
    bcm_port_t port,
    bcm_port_nif_scheduler_t bcm_sch_prio,
    int enable)
{
    dnx_algo_port_ilkn_access_info_t ilkn_access_info;
    uint32 entry_handle_id = 0, dbal_sch_prio;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_ilkn_access_info_get(unit, port, &ilkn_access_info));

    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_OFR_ILKN_RX_SCHEDULER_REQUEST_ENABLE, &entry_handle_id));

    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ILKN_ID, ilkn_access_info.ilkn_id);
    SHR_IF_ERR_EXIT(imb_common_dbal_sch_prio_from_bcm_sch_prio_get(unit, bcm_sch_prio, &dbal_sch_prio));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PRIORITY, dbal_sch_prio);

    /*
     * set value field
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_REQUEST_ENABLE, INST_SINGLE, enable);

    /*
     * commit value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_port_ofr_rx_sch_config_set(
    int unit,
    bcm_port_t port,
    const bcm_port_prio_config_t * priority_config,
    int enable)
{
    dnx_algo_port_info_s port_info;
    int priority_group;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, port, &port_info));

    for (priority_group = 0; priority_group < priority_config->nof_priority_groups; ++priority_group)
    {
        switch (priority_config->priority_groups[priority_group].sch_priority)
        {
            case bcmPortNifSchedulerLow:
            case bcmPortNifSchedulerHigh:
                if (DNX_ALGO_PORT_TYPE_IS_NIF_ETH(unit, port_info, DNX_ALGO_PORT_TYPE_INCLUDE(STIF, L1))
                    || DNX_ALGO_PORT_TYPE_IS_FRAMER_MAC(unit, port_info, TRUE, TRUE))
                {
                    SHR_IF_ERR_EXIT(dnx_port_ofr_rx_sch_low_high_config_set
                                    (unit, port, priority_config, priority_group, enable));
                }
                else if (DNX_ALGO_PORT_TYPE_IS_NIF_ILKN(unit, port_info, DNX_ALGO_PORT_TYPE_INCLUDE(L1)))
                {
                    SHR_IF_ERR_EXIT(dnx_port_ofr_rx_sch_ilkn_request_config_set
                                    (unit, port, priority_config->priority_groups[priority_group].sch_priority,
                                     enable));
                    SHR_IF_ERR_EXIT(dnx_port_ofr_rx_sch_ilkn_is_high_set
                                    (unit, port, priority_config, priority_group, enable));
                }
                break;
            case bcmPortNifSchedulerTDM:
                if (DNX_ALGO_PORT_TYPE_IS_NIF_ETH(unit, port_info, DNX_ALGO_PORT_TYPE_INCLUDE(STIF, L1)))
                {
                    SHR_IF_ERR_EXIT(dnx_port_ofr_rx_sch_tdm_config_set
                                    (unit, port, priority_config, priority_group, enable));
                }
                else if (DNX_ALGO_PORT_TYPE_IS_NIF_ILKN(unit, port_info, DNX_ALGO_PORT_TYPE_INCLUDE(L1)))
                {
                    SHR_IF_ERR_EXIT(dnx_port_ofr_rx_sch_ilkn_request_config_set
                                    (unit, port, priority_config->priority_groups[priority_group].sch_priority,
                                     enable));
                }
                break;
        }
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - set one of the TPID values per port profile to be recognized
 *        by the PRD parser. if a packet TPID is not recognized,
 *        the packet will get the default priority. each port
 *        can have four TPID values.
 *
 * see .h file
 */
int
dnx_ofr_prd_tpid_hw_set(
    int unit,
    uint32 prd_profile,
    uint32 tpid_index,
    uint32 tpid_value)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_PRD_OFR_TPID, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PRD_PROFILE, prd_profile);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_INDEX, tpid_index);
    /*
     * set value field
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TPID, INST_SINGLE, tpid_value);
    /*
     * commit value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - get the TPID value per port profile to be recognized
 *        by the PRD parser. if a packet TPID is not recognized,
 *        the packet will get the default priority. each port
 *        can have four TPID values.
 *
 * see .h file
 */
int
dnx_ofr_prd_tpid_hw_get(
    int unit,
    uint32 prd_profile,
    uint32 tpid_index,
    uint32 *tpid_value)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_PRD_OFR_TPID, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PRD_PROFILE, prd_profile);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_INDEX, tpid_index);
    /*
     * request for value field
     */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_TPID, INST_SINGLE, tpid_value);
    /*
     * commit value
     */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - fill the TM priority map (PRD hard stage). add the
 *        the map priority value to be returned per TC + DP
 *        values
 *
 * see .h file
 */
int
dnx_ofr_prd_map_tm_tc_dp_hw_set(
    int unit,
    uint32 tc,
    uint32 dp,
    uint32 priority,
    uint32 is_tdm)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_PRD_OFR_TM_PRIORITY_MAP, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_TC, tc);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_DP, dp);
    /*
     * set value field
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PRIORITY, INST_SINGLE, priority);
    if (dnx_data_tdm.params.feature_get(unit, dnx_data_tdm_params_is_supported))
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_TDM, INST_SINGLE, is_tdm);
    }
    /*
     * commit value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - get information from TM priority map (PRD hard
 *        stage). get the priority given for a combination of
 *        TC+DP.
 *
 * see .h file
 */
int
dnx_ofr_prd_map_tm_tc_dp_hw_get(
    int unit,
    uint32 tc,
    uint32 dp,
    uint32 *priority,
    uint32 *is_tdm)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_PRD_OFR_TM_PRIORITY_MAP, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_TC, tc);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_DP, dp);

    /*
     * request for value field
     */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_PRIORITY, INST_SINGLE, priority);
    if (dnx_data_tdm.params.feature_get(unit, dnx_data_tdm_params_is_supported))
    {
        dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_IS_TDM, INST_SINGLE, is_tdm);
    }
    else
    {
        *is_tdm = 0;
    }
    /*
     * commit value
     */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - fill the IP priority table (PRD hard stage). set
 *        priority value for a specific DSCP.
 *
 * see .h file
 */
int
dnx_ofr_prd_map_ip_dscp_hw_set(
    int unit,
    uint32 dscp,
    uint32 priority,
    uint32 is_tdm)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_PRD_OFR_IP_PRIORITY_MAP, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_DSCP, dscp);
    /*
     * set value field
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PRIORITY, INST_SINGLE, priority);
    if (dnx_data_tdm.params.feature_get(unit, dnx_data_tdm_params_is_supported))
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_TDM, INST_SINGLE, is_tdm);
    }
    /*
     * commit value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - get information from IP priority table (PRD hard
 *        stage). get the priority returned for a specific DSCP
 *
 * See .h file
 */
int
dnx_ofr_prd_map_ip_dscp_hw_get(
    int unit,
    uint32 dscp,
    uint32 *priority,
    uint32 *is_tdm)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_PRD_OFR_IP_PRIORITY_MAP, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_DSCP, dscp);

    /*
     * request for value field
     */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_PRIORITY, INST_SINGLE, priority);
    if (dnx_data_tdm.params.feature_get(unit, dnx_data_tdm_params_is_supported))
    {
        dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_IS_TDM, INST_SINGLE, is_tdm);
    }
    else
    {
        *is_tdm = 0;
    }
    /*
     * commit value
     */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - fill the MPLS priority table (PRD hard stage). set
 *        priority value for a specific EXP value.
 *
 * see .h file
 */
int
dnx_ofr_prd_map_mpls_exp_hw_set(
    int unit,
    uint32 exp,
    uint32 priority,
    uint32 is_tdm)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_PRD_OFR_MPLS_PRIORITY_MAP, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_EXP, exp);
    /*
     * set value field
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PRIORITY, INST_SINGLE, priority);
    if (dnx_data_tdm.params.feature_get(unit, dnx_data_tdm_params_is_supported))
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_TDM, INST_SINGLE, is_tdm);
    }
    /*
     * commit value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - get information from the MPLS priority table. get
 *        the priority returned for a specific EXP value
 *
 * see .h file
 */
int
dnx_ofr_prd_map_mpls_exp_hw_get(
    int unit,
    uint32 exp,
    uint32 *priority,
    uint32 *is_tdm)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_PRD_OFR_MPLS_PRIORITY_MAP, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_EXP, exp);

    /*
     * request for value field
     */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_PRIORITY, INST_SINGLE, priority);
    if (dnx_data_tdm.params.feature_get(unit, dnx_data_tdm_params_is_supported))
    {
        dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_IS_TDM, INST_SINGLE, is_tdm);
    }
    else
    {
        *is_tdm = 0;
    }
    /*
     * commit value
     */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - fill the ETH (VLAN) Priority table (PRD hard
 *        stage). set a priority value to match a specific
 *        PCP+DEI combination
 *
 * see .h file
 */
int
dnx_ofr_prd_map_pcp_dei_hw_set(
    int unit,
    uint32 pcp,
    uint32 dei,
    uint32 priority,
    uint32 is_tdm)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_PRD_OFR_VLAN_PRIORITY_MAP, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PCP, pcp);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_DEI, dei);
    /*
     * set value field
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PRIORITY, INST_SINGLE, priority);
    if (dnx_data_tdm.params.feature_get(unit, dnx_data_tdm_params_is_supported))
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_TDM, INST_SINGLE, is_tdm);
    }
    /*
     * commit value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;

}

/**
 * \brief - get information from the ETH (VLAN) priority table
 *        (PRD hard stage). get the priority returned for a
 *        specific combination of PCP+DEI
 *
 * see .h file
 */
int
dnx_ofr_prd_map_pcp_dei_hw_get(
    int unit,
    uint32 pcp,
    uint32 dei,
    uint32 *priority,
    uint32 *is_tdm)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_PRD_OFR_VLAN_PRIORITY_MAP, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PCP, pcp);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_DEI, dei);
    /*
     * request for value field
     */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_PRIORITY, INST_SINGLE, priority);
    if (dnx_data_tdm.params.feature_get(unit, dnx_data_tdm_params_is_supported))
    {
        dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_IS_TDM, INST_SINGLE, is_tdm);
    }
    else
    {
        *is_tdm = 0;
    }
    /*
     * commit value
     */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - set PRD Parser whether to ignore indication of IP
 *        dscp even if packet is identified as IP.
 *
 * see .h file
 */
int
dnx_ofr_prd_ignore_ip_dscp_hw_set(
    int unit,
    int prd_profile,
    uint32 ignore_ip_dscp)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_PRD_OFR_PORT_PROPERTIES, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PRD_PROFILE, prd_profile);
    /*
     * set value field
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IGNORE_IP_DSCP, INST_SINGLE, ignore_ip_dscp);
    /*
     * commit value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - get from PRD parser indication whether it ignores IP
 *        DSCP
 *
 * see .h file
 */
int
dnx_ofr_prd_ignore_ip_dscp_hw_get(
    int unit,
    int prd_profile,
    uint32 *ignore_ip_dscp)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_PRD_OFR_PORT_PROPERTIES, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PRD_PROFILE, prd_profile);

    /*
     * request for value field
     */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_IGNORE_IP_DSCP, INST_SINGLE, ignore_ip_dscp);
    /*
     * commit value
     */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - set PRD parser to ignore MPLS EXP even if packet is
 *        identified as MPLS
 *
 * see .h file
 */
int
dnx_ofr_prd_ignore_mpls_exp_hw_set(
    int unit,
    int prd_profile,
    uint32 ignore_mpls_exp)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_PRD_OFR_PORT_PROPERTIES, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PRD_PROFILE, prd_profile);
    /*
     * set value field
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IGNORE_MPLS_EXP, INST_SINGLE, ignore_mpls_exp);
    /*
     * commit value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - get from PRD parser indication whether it ignores
 *        MPLS EXP
 *
 * see .h file
 */
int
dnx_ofr_prd_ignore_mpls_exp_hw_get(
    int unit,
    int prd_profile,
    uint32 *ignore_mpls_exp)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_PRD_OFR_PORT_PROPERTIES, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PRD_PROFILE, prd_profile);

    /*
     * request for value field
     */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_IGNORE_MPLS_EXP, INST_SINGLE, ignore_mpls_exp);
    /*
     * commit value
     */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - set PRD Parser whether to ignore inner tag PCP DEI
 *        indication even if packet is identified as double
 *        tagged
 *
 * see .h file
 */
int
dnx_ofr_prd_ignore_inner_tag_hw_set(
    int unit,
    int prd_profile,
    uint32 ignore_inner_tag)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_PRD_OFR_PORT_PROPERTIES, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PRD_PROFILE, prd_profile);
    /*
     * set value field
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IGNORE_INNER_ETH_TAG, INST_SINGLE, ignore_inner_tag);
    /*
     * commit value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - get from PRD parser indication whether it ignores
 *        inner VLAN tag
 *
 * see .h file
 */
int
dnx_ofr_prd_ignore_inner_tag_hw_get(
    int unit,
    int prd_profile,
    uint32 *ignore_inner_tag)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_PRD_OFR_PORT_PROPERTIES, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PRD_PROFILE, prd_profile);

    /*
     * request for value field
     */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_IGNORE_INNER_ETH_TAG, INST_SINGLE, ignore_inner_tag);
    /*
     * commit value
     */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - set PRD Parser whether to ignore outer tag PCP DEI
 *        indication even if packet is identified as VLAN tagged
 *
 * see .h file
 */
int
dnx_ofr_prd_ignore_outer_tag_hw_set(
    int unit,
    int prd_profile,
    uint32 ignore_outer_tag)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_PRD_OFR_PORT_PROPERTIES, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PRD_PROFILE, prd_profile);
    /*
     * set value field
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IGNORE_OUTER_ETH_TAG, INST_SINGLE, ignore_outer_tag);
    /*
     * commit value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - get from PRD parser indication whether it ignores
 *        outer VLAN tag
 *
 * see .h file
 */
int
dnx_ofr_prd_ignore_outer_tag_hw_get(
    int unit,
    int prd_profile,
    uint32 *ignore_outer_tag)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_PRD_OFR_PORT_PROPERTIES, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PRD_PROFILE, prd_profile);

    /*
     * request for value field
     */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_IGNORE_OUTER_ETH_TAG, INST_SINGLE, ignore_outer_tag);
    /*
     * commit value
     */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - set default priority for the PRD hard stage parser.
 *        if the packet is not identified as IP/MPLS/double
 *        tagged/single tagged or if the relevant indications
 *        are set to ignore, the default priority will be given
 *        to the packet. (the priority from the hard stage can
 *        be later overridden by the priority from the soft
 *        stage.
 *
 * see .h file
 */
int
dnx_ofr_prd_default_priority_hw_set(
    int unit,
    int prd_profile,
    uint32 default_priority)
{
    uint32 entry_handle_id;
    uint32 is_tdm_supported = dnx_data_tdm.params.feature_get(unit, dnx_data_tdm_params_is_supported);
    uint32 is_tdm = 0;
    uint32 max_priority = dnx_data_nif.prd.nof_priorities_get(unit) - 1;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    is_tdm = (default_priority == BCM_COSQ_INGRESS_PORT_DROP_PRIORITY_TDM) ? 1 : 0;

    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_PRD_OFR_PORT_PROPERTIES, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PRD_PROFILE, prd_profile);
    /*
     * set value field
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DEFAULT_PRIORITY, INST_SINGLE,
                                 (is_tdm) ? max_priority : default_priority);
    if (is_tdm_supported)
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_TDM, INST_SINGLE, is_tdm);
    }
    /*
     * commit value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - get default priority given in the PRD hard stage
 *        parser.
 *
 * see .h file
 */
int
dnx_ofr_prd_default_priority_hw_get(
    int unit,
    int prd_profile,
    uint32 *default_priority)
{
    uint32 entry_handle_id;
    uint32 is_tdm_supported = dnx_data_tdm.params.feature_get(unit, dnx_data_tdm_params_is_supported);
    uint32 is_tdm = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_PRD_OFR_PORT_PROPERTIES, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PRD_PROFILE, prd_profile);
    /*
     * request for value field
     */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_DEFAULT_PRIORITY, INST_SINGLE, default_priority);
    if (is_tdm_supported)
    {
        dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_IS_TDM, INST_SINGLE, &is_tdm);
    }
    /*
     * commit value
     */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

    /*
     * Return TDM as default priority if it is set 
     */
    if (is_tdm)
    {
        *default_priority = BCM_COSQ_INGRESS_PORT_DROP_PRIORITY_TDM;
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - set configurable ether type to a ether type code.
 *        the ether type codes are meaningful in the PRD soft
 *        stage (TCAM).
 *
 * See .h file
 */
int
dnx_ofr_prd_custom_ether_type_hw_set(
    int unit,
    uint32 ether_type_code,
    uint32 ether_type_val)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_PRD_OFR_CONFIGURABLE_ETHER_TYPE, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ETHER_TYPE_CODE, ether_type_code);
    /*
     * set value field
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ETHER_TYPE_VAL, INST_SINGLE, ether_type_val);
    /*
     * commit value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - get the ether type value mapped to a specific ether
 *        type code, out of the configurable ether types (codes
 *        1-6).
 *
 * see .h file
 */
int
dnx_ofr_prd_custom_ether_type_hw_get(
    int unit,
    uint32 ether_type_code,
    uint32 *ether_type_val)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_PRD_OFR_CONFIGURABLE_ETHER_TYPE, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ETHER_TYPE_CODE, ether_type_code);
    /*
     * request for value field
     */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_ETHER_TYPE_VAL, INST_SINGLE, ether_type_val);
    /*
     * commit value
     */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
* \brief - set MPLS special label properties. if one of the
*        MPLS labels is identified as special label and its
*        value match one of the special label values, priority
*        for the packet is taken from the special label
*        properties.
*
* see .h file
*/
int
dnx_ofr_prd_mpls_special_label_hw_set(
    int unit,
    uint32 special_label_index,
    const bcm_cosq_ingress_port_drop_mpls_special_label_config_t * label_config)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_PRD_OFR_MPLS_SPECIAL_LABEL, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_INDEX, special_label_index);
    /*
     * set value field
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LABEL_VALUE, INST_SINGLE, label_config->label_value);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PRIORITY, INST_SINGLE, label_config->priority);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_TDM, INST_SINGLE, label_config->is_tdm);
    /*
     * commit value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
* \brief - get current configuration of MPLS special label
*
* see .h file
*/
int
dnx_ofr_prd_mpls_special_label_hw_get(
    int unit,
    uint32 special_label_index,
    bcm_cosq_ingress_port_drop_mpls_special_label_config_t * label_config)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_PRD_OFR_MPLS_SPECIAL_LABEL, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_INDEX, special_label_index);
    /*
     * request for value field
     */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_LABEL_VALUE, INST_SINGLE, &label_config->label_value);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_PRIORITY, INST_SINGLE, &label_config->priority);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_IS_TDM, INST_SINGLE, &label_config->is_tdm);
    /*
     * commit value
     */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - set PRD thresholds per priority. based on the
 *        priority given from the parser, the packet is sent to
 *        the correct RMC. in each RMC there is a thresholds per
 *        priority which is mapped to it.
 *
 * see .h file
 */
int
dnx_ofr_prd_threshold_hw_set(
    int unit,
    uint32 rmc,
    uint32 priority,
    uint32 threshold)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_PRD_OFR_THRESHOLDS, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_RMC_ID, rmc);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PRIORITY, priority);
    /*
     * set value field
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_THRESHOLD, INST_SINGLE, threshold);
    /*
     * commit value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - get the current threshold for a specific priority in
 *        a specific RMC.
 *
 * see .h file
 */
int
dnx_ofr_prd_threshold_hw_get(
    int unit,
    uint32 rmc,
    uint32 priority,
    uint32 *threshold)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_PRD_OFR_THRESHOLDS, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_RMC_ID, rmc);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PRIORITY, priority);
    /*
     * request for value field
     */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_THRESHOLD, INST_SINGLE, threshold);
    /*
     * commit value
     */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - fill the PRD soft stage TCAM table. there are 32
 *        entries in the table. if there is a hit, the priority
 *        for the packet will be taken from the TCAM entry
 *        information
 *
 * see .h file
 */

int
dnx_ofr_prd_tcam_entry_hw_set(
    int unit,
    uint32 entry_index,
    const dnx_cosq_prd_tcam_entry_info_t * entry_info)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_PRD_OFR_TCAM, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ENTRY_ID, entry_index);
    /*
     * set value field
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ETHER_TYPE_CODE, INST_SINGLE,
                                 entry_info->ether_code_val);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ETHER_TYPE_CODE_MASK, INST_SINGLE,
                                 entry_info->ether_code_mask);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ENTRY_OFFSETS_VALUE, INST_SINGLE,
                                 entry_info->offset_array_val);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ENTRY_OFFSETS_MASK, INST_SINGLE,
                                 entry_info->offset_array_mask);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PRIORITY, INST_SINGLE, entry_info->priority);
    if (dnx_data_tdm.params.feature_get(unit, dnx_data_tdm_params_is_supported))
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TDM, INST_SINGLE, entry_info->tdm);
    }
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_VALID, INST_SINGLE, entry_info->is_entry_valid);
    /*
     * commit value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - get information from the PRD soft stage (TCAM)
 *        table. get specific entry information
 *
 * see .h file
 */
int
dnx_ofr_prd_tcam_entry_hw_get(
    int unit,
    uint32 entry_index,
    dnx_cosq_prd_tcam_entry_info_t * entry_info)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_PRD_OFR_TCAM, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ENTRY_ID, entry_index);
    /*
     * request for value field
     */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_ETHER_TYPE_CODE, INST_SINGLE,
                               &entry_info->ether_code_val);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_ETHER_TYPE_CODE_MASK, INST_SINGLE,
                               &entry_info->ether_code_mask);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_ENTRY_OFFSETS_VALUE, INST_SINGLE,
                               &entry_info->offset_array_val);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_ENTRY_OFFSETS_MASK, INST_SINGLE,
                               &entry_info->offset_array_mask);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_PRIORITY, INST_SINGLE, &entry_info->priority);
    if (dnx_data_tdm.params.feature_get(unit, dnx_data_tdm_params_is_supported))
    {
        dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_TDM, INST_SINGLE, &entry_info->tdm);
    }
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_VALID, INST_SINGLE, &entry_info->is_entry_valid);
    /*
     * commit value
     */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - set the PRD soft stage (TCAM) key. there is a key
 *        per ether type (total of 16 keys). the key is build of
 *        4 offsets given in the packet header. from each
 *        offset, 8 bits are taken to create a total of 32 bit.
 *        when comparing to the TCAM entries, the ether type
 *        code joins the key to create 36bit key:
 *        |ether type code|offset 4|offset 3|offset 2|offset 1|
 *        -----------------------------------------------------
 *        35              31       23       15       7        0
 *
 * see .h file
 */
int
dnx_ofr_prd_tcam_entry_key_offset_hw_set(
    int unit,
    uint32 ether_type_code,
    uint32 offset_index,
    uint32 offset_value)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_PRD_OFR_TCAM_KEY, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ETHER_TYPE_CODE, ether_type_code);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OFFSET_INDEX, offset_index);
    /*
     * set value field
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OFFSET, INST_SINGLE, offset_value);
    /*
     * commit value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - get information about the TCAM key for a specific
 *        ether type.
 *
 * see .h file
 */
int
dnx_ofr_prd_tcam_entry_key_offset_hw_get(
    int unit,
    uint32 ether_type_code,
    uint32 offset_index,
    uint32 *offset_value)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_PRD_OFR_TCAM_KEY, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ETHER_TYPE_CODE, ether_type_code);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OFFSET_INDEX, offset_index);
    /*
     * request for value field
     */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_OFFSET, INST_SINGLE, offset_value);
    /*
     * commit value
     */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - set the offset base for the TCAM key. it means the
 *        offsets that compose the key will start from this
 *        offset base. the offset base have 3 options:
 * 0=>start of packet
 * 1=>end of eth header
 * 2=>end of header after eth header.
 *
 * see .h file
 */
int
dnx_ofr_prd_tcam_entry_key_offset_base_hw_set(
    int unit,
    uint32 ether_type_code,
    uint32 offset_base)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_PRD_OFR_TCAM_KEY_OFFSET_BASE, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ETHER_TYPE_CODE, ether_type_code);
    /*
     * set value field
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OFFSET_BASE, INST_SINGLE, offset_base);
    /*
     * commit value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - get the current offset base for the given ether
 *        code.
 *
 * see .h file
 */
int
dnx_ofr_prd_tcam_entry_key_offset_base_hw_get(
    int unit,
    uint32 ether_type_code,
    uint32 *offset_base)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_PRD_OFR_TCAM_KEY_OFFSET_BASE, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ETHER_TYPE_CODE, ether_type_code);
    /*
     * request for value field
     */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_OFFSET_BASE, INST_SINGLE, offset_base);
    /*
     * commit value
     */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - set ether type size (in bytes) for the given ether
 *        type code. the ether type size is only used if the
 *        offset base for the key is "end of header after eth
 *        header"
 *
 * see .h file
 */
int
dnx_ofr_prd_ether_type_size_hw_set(
    int unit,
    uint32 ether_type_code,
    uint32 ether_type_size)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_PRD_OFR_ETHER_TYPE_CODE_HEADER_SIZE, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ETHER_TYPE_CODE, ether_type_code);
    /*
     * set value field
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ETHER_TYPE_SIZE, INST_SINGLE, ether_type_size);
    /*
     * commit value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - get the ether type currently set for a specific
 *        ether type code
 *
 * see .h file
 */
int
dnx_ofr_prd_ether_type_size_hw_get(
    int unit,
    uint32 ether_type_code,
    uint32 *ether_type_size)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_PRD_OFR_ETHER_TYPE_CODE_HEADER_SIZE, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ETHER_TYPE_CODE, ether_type_code);
    /*
     * request for value field
     */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_ETHER_TYPE_SIZE, INST_SINGLE, ether_type_size);
    /*
     * commit value
     */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - set Control Frame properties to be recognized by the
 *        PRD parser. if a control frame is identified, it
 *        automatically gets the highest priority (3). each
 *        packet is compared against all control frame
 *        properties of the ETHU
 *
 * see .h file
 */
int
dnx_ofr_prd_control_frame_hw_set(
    int unit,
    uint32 control_frame_index,
    const bcm_cosq_ingress_drop_control_frame_config_t * control_frame_config)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_PRD_OFR_CONTROL_FRAME, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_INDEX, control_frame_index);
    /*
     * set value field
     */
    dbal_entry_value_field64_set(unit, entry_handle_id, DBAL_FIELD_MAC_DA_VALUE, INST_SINGLE,
                                 control_frame_config->mac_da_val);
    dbal_entry_value_field64_set(unit, entry_handle_id, DBAL_FIELD_MAC_DA_MASK, INST_SINGLE,
                                 control_frame_config->mac_da_mask);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ETHER_TYPE_CODE, INST_SINGLE,
                                 control_frame_config->ether_type_code);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ETHER_TYPE_CODE_MASK, INST_SINGLE,
                                 control_frame_config->ether_type_code_mask);
    /*
     * commit value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - get Control Frame properties recognized by the
 *        parser.
 *
 * see .h file
 */
int
dnx_ofr_prd_control_frame_hw_get(
    int unit,
    uint32 control_frame_index,
    bcm_cosq_ingress_drop_control_frame_config_t * control_frame_config)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_PRD_OFR_CONTROL_FRAME, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_INDEX, control_frame_index);
    /*
     * request for value field
     */
    dbal_value_field64_request(unit, entry_handle_id, DBAL_FIELD_MAC_DA_VALUE, INST_SINGLE,
                               &control_frame_config->mac_da_val);
    dbal_value_field64_request(unit, entry_handle_id, DBAL_FIELD_MAC_DA_MASK, INST_SINGLE,
                               &control_frame_config->mac_da_mask);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_ETHER_TYPE_CODE, INST_SINGLE,
                               &control_frame_config->ether_type_code);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_ETHER_TYPE_CODE_MASK, INST_SINGLE,
                               &control_frame_config->ether_type_code_mask);
    /*
     * commit value
     */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

int
dnx_ofr_rx_prd_port_profile_map_hw_set(
    int unit,
    int nif_interface_id,
    int prd_profile)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_PRD_OFR_PORT_PROFILE_MAP, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_NIF_PORT_ID, nif_interface_id);

    /*
     * set value field
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PRD_PROFILE, INST_SINGLE, prd_profile);
    /*
     * commit the value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - enable PRD hard stage per RMC. this effectively
 *        enable the PRD feature.
 *
 * see .h file
 */
int
dnx_ofr_prd_hard_stage_enable_hw_set(
    int unit,
    uint32 rmc,
    uint32 enable)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_PRD_OFR_ENABLERS, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_RMC_ID, rmc);
    /*
     * set value field
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ENABLE_HARD_STAGE, INST_SINGLE, enable);
    /*
     * commit value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - get enable indication for the PRD hard stage per RMC
 *
 * see .h file
 */
int
dnx_ofr_prd_hard_stage_enable_hw_get(
    int unit,
    uint32 rmc,
    uint32 *enable)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_PRD_OFR_ENABLERS, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_RMC_ID, rmc);
    /*
     * request for value field
     */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_ENABLE_HARD_STAGE, INST_SINGLE, enable);
    /*
     * commit value
     */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - enable PRD soft stage per port. PRD soft stage is a
 *        TCAM for which the user configures both the table and
 *        the key. if there is a hit in the TCAM, the priority
 *        from the TCAM entry will override the priority from
 *        the hard stage
 *
 * see .h file
 */
int
dnx_ofr_prd_soft_stage_enable_hw_set(
    int unit,
    int prd_profile,
    uint32 enable_eth,
    uint32 enable_tm)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_PRD_OFR_PORT_PROPERTIES, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PRD_PROFILE, prd_profile);
    /*
     * set value field
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SOFT_STAGE_ENABLE_ETH, INST_SINGLE, enable_eth);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SOFT_STAGE_ENABLE_TM, INST_SINGLE, enable_tm);
    /*
     * commit value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - get enable indication for PRD soft stage
 *
 * see .h file
 */
int
dnx_ofr_prd_soft_stage_enable_hw_get(
    int unit,
    int prd_profile,
    uint32 *enable_eth,
    uint32 *enable_tm)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_PRD_OFR_PORT_PROPERTIES, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PRD_PROFILE, prd_profile);
    /*
     * request for value field
     */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_SOFT_STAGE_ENABLE_ETH, INST_SINGLE, enable_eth);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_SOFT_STAGE_ENABLE_TM, INST_SINGLE, enable_tm);
    /*
     * commit value
     */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_port_ofr_enable_dynamic_memories_set(
    int unit,
    int enable)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_OFR_ENABLE_MEMORIES, &entry_handle_id));

    /*
     * Enabling OFT memories
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_INIT_MEMORY, INST_SINGLE, 1);

    /*
     * commit the value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - set ITMH TC + DP offsets for the PRD.
 *
 * see .h file
 */
int
dnx_ofr_prd_itmh_offsets_hw_set(
    int unit,
    uint32 tc_offset,
    uint32 dp_offset)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_PRD_OFR_HARD_STAGE_CTRL, &entry_handle_id));
    /*
     * Set TC and DP offset
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ITMH_TC_OFFSET, INST_SINGLE, tc_offset);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ITMH_DP_OFFSET, INST_SINGLE, dp_offset);
    /*
     * commit value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - set FTMH  TC + DP offsets for the PRD
 *
 * see .h file
 */
int
dnx_ofr_prd_ftmh_offsets_hw_set(
    int unit,
    uint32 tc_offset,
    uint32 dp_offset)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_PRD_OFR_HARD_STAGE_CTRL, &entry_handle_id));
    /*
     * Set TC and DP offset
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FTMH_TC_OFFSET, INST_SINGLE, tc_offset);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FTMH_DP_OFFSET, INST_SINGLE, dp_offset);
    /*
     * commit value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - set outer tag size for the port. if port is port
 *        Extender, the outer tag size should be set to 8B,
 *        otherwise 4B
 *
 * see .h file
 */
int
dnx_ofr_prd_outer_tag_size_hw_set(
    int unit,
    int prd_profile,
    uint32 outer_tag_size)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_PRD_OFR_PORT_PROPERTIES, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PRD_PROFILE, prd_profile);
    /*
     * set value field
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OUTER_TAG_SIZE, INST_SINGLE, outer_tag_size);
    /*
     * commit value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - set PRD port type, according to the header type of
 *        the port
 *
 * see .h file
 */
int
dnx_ofr_prd_port_type_hw_set(
    int unit,
    int prd_profile,
    uint32 prd_port_type)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_PRD_OFR_PORT_PROPERTIES, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PRD_PROFILE, prd_profile);
    /*
     * set value field
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PORT_TYPE, INST_SINGLE, prd_port_type);
    /*
     * commit value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - map physical lane in the ETHU + PRD priority to a specific RMC (logical FIFO)
 *
 * \param [in] unit - chip unit id
 * \param [in] prd_profile - prd profile to map
 * \param [in] rmc - logical fifo info
 *
 * \return
 *   int
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
static int
dnx_ofr_profile_rx_priority_to_rmc_hw_map(
    int unit,
    uint32 prd_profile,
    dnx_algo_port_rmc_info_t * rmc)
{
    uint32 entry_handle_id;
    uint32 priority_group;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * rmc priority group (0 for high and 1 for low)
     */
    priority_group = rmc->rmc_id < dnx_data_nif.ofr.nof_rmc_per_priority_group_get(unit) ? 0 : 1;
    /*
     * map RMC to lane + PRD priority
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_OFR_RX_PRIORITY_TO_RMC_MAP, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PRD_PROFILE, prd_profile);

    /*
     * RMC can be mapped to more than one PRD priority of the lane (even to all priorities), so each priority is
     * checked independently
     */
    if (rmc->prd_priority & BCM_PORT_F_PRIORITY_0)
    {
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PRD_PRIORITY, 0);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_LOW, INST_SINGLE, priority_group);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }
    if (rmc->prd_priority & BCM_PORT_F_PRIORITY_1)
    {
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PRD_PRIORITY, 1);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_LOW, INST_SINGLE, priority_group);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }
    if (rmc->prd_priority & BCM_PORT_F_PRIORITY_2)
    {
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PRD_PRIORITY, 2);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_LOW, INST_SINGLE, priority_group);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }
    if (rmc->prd_priority & BCM_PORT_F_PRIORITY_3)
    {
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PRD_PRIORITY, 3);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_LOW, INST_SINGLE, priority_group);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - map physical lane in the ETHU + PRD priority to a specific RMC (logical FIFO)
 *
 * \param [in] unit - chip unit id
 * \param [in] prd_profile - prd profile to map
 * \param [in] rmc_group - array in size of number of priorities defining the RMC machine per priority
 *
 * \return
 *   int
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int
dnx_ofr_profile_rx_priority_to_rmc_map_get(
    int unit,
    int prd_profile,
    uint32 *rmc_group)
{
    uint32 entry_handle_id;
    int prd_prio;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * map RMC to lane + PRD priority
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_OFR_RX_PRIORITY_TO_RMC_MAP, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PRD_PROFILE, prd_profile);

    /*
     * RMC can be mapped to more than one PRD priority of the lane (even to all priorities), so each priority is
     * checked independently
     */
    for (prd_prio = 0; prd_prio < dnx_data_nif.prd.nof_priorities_get(unit); prd_prio++)
    {
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PRD_PRIORITY, prd_prio);
        dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_IS_LOW, INST_SINGLE, &rmc_group[prd_prio]);
        SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - map PRD priority to a
 *        specific RMC (logical FIFO)
 */
int
dnx_ofr_internal_port_rx_priority_to_rmc_map(
    int unit,
    bcm_port_t port,
    dnx_algo_port_rmc_info_t * rmc)
{
    uint32 prd_profile;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Get prd port profile
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_prd_profile_get(unit, port, &prd_profile));

    SHR_IF_ERR_EXIT(dnx_ofr_profile_rx_priority_to_rmc_hw_map(unit, prd_profile, rmc));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_port_ofr_internal_ilkn_rx_counter_reset_set(
    int unit,
    uint32 ilkn_id,
    int is_tdm,
    uint32 in_reset)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC
                    (unit, DBAL_TABLE_NIF_OFR_RX_SCHEDULER_ILKN_TRANSACTION_COUNTER_RESET, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ILKN_ID, ilkn_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_TDM, is_tdm);

    /*
     * set value fields
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESET,
                                 INST_SINGLE,
                                 in_reset ? DBAL_ENUM_FVAL_NIF_RESET_REVERSE_LOGIC_IN_RESET :
                                 DBAL_ENUM_FVAL_NIF_RESET_REVERSE_LOGIC_OUT_OF_RESET);

    /*
     * commit the value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_port_ofr_internal_ilkn_rx_is_high_priority_config_set(
    int unit,
    uint32 ilkn_id,
    uint32 is_priority_high)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_OFR_ILKN_SCHEDULER_CONFIG, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ILKN_ID, ilkn_id);

    /*
     * set value fields
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_ILKN_HIGH, INST_SINGLE, is_priority_high);

    /*
     * commit the value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_port_ofr_internal_ilkn_rx_sched_request_enable_set(
    int unit,
    uint32 ilkn_id,
    dbal_enum_value_field_nif_sch_priority_e dbal_sch_prio,
    uint32 is_enable)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_OFR_ILKN_RX_SCHEDULER_REQUEST_ENABLE, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ILKN_ID, ilkn_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_SCH_PRIORITY, dbal_sch_prio);

    /*
     * set value fields
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_REQUEST_ENABLE, INST_SINGLE, is_enable);

    /*
     * commit the value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_port_ofr_internal_ilkn_rx_sched_request_enable_get(
    int unit,
    uint32 ilkn_id,
    dbal_enum_value_field_nif_sch_priority_e dbal_sch_prio,
    uint32 *is_enabled)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_OFR_ILKN_RX_SCHEDULER_REQUEST_ENABLE, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ILKN_ID, ilkn_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_SCH_PRIORITY, dbal_sch_prio);

    /*
     * set value fields
     */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_REQUEST_ENABLE, INST_SINGLE, is_enabled);

    /*
     * commit the value
     */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

int
dnx_ofr_prd_global_drop_count_hw_get(
    int unit,
    uint32 priority,
    uint32 priority_group,
    uint64 *drop_count)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_OFR_PRD_GLOBAL_COUNTERS, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PRD_PRIORITY, priority);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PRIORITY_GROUP, priority_group);
    /*
     * set value fields
     */
    dbal_value_field64_request(unit, entry_handle_id, DBAL_FIELD_COUNT, INST_SINGLE, drop_count);
    /*
     * commit the value
     */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;

}

int
dnx_ofr_prd_port_counter_hw_set(
    int unit,
    uint32 counter,
    uint32 nif_port,
    uint32 priority_group)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_OFR_PRD_COUNTERS_MAP, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_COUNTER, counter);
    /*
     * set value fields
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_NIF_PORT_ID, INST_SINGLE, nif_port);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PRIORITY_GROUP, INST_SINGLE, priority_group);
    /*
     * commit the value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;

}

int
dnx_ofr_prd_counter_drop_count_hw_get(
    int unit,
    uint32 counter,
    uint64 *prio_0_drop_count,
    uint64 *prio_1_drop_count,
    uint64 *prio_2_drop_count,
    uint64 *prio_3_drop_count)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_OFR_PRD_MAPPED_COUNTERS_INDICATIONS, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_COUNTER, counter);
    /*
     * get value fields
     */
    dbal_value_field64_request(unit, entry_handle_id, DBAL_FIELD_PRIORITY_0_COUNT, INST_SINGLE, prio_0_drop_count);
    dbal_value_field64_request(unit, entry_handle_id, DBAL_FIELD_PRIORITY_1_COUNT, INST_SINGLE, prio_1_drop_count);
    dbal_value_field64_request(unit, entry_handle_id, DBAL_FIELD_PRIORITY_2_COUNT, INST_SINGLE, prio_2_drop_count);
    dbal_value_field64_request(unit, entry_handle_id, DBAL_FIELD_PRIORITY_3_COUNT, INST_SINGLE, prio_3_drop_count);

    /*
     * get the values
     */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;

}

shr_error_e
dnx_port_ofr_double_link_list_total_free_sections_get(
    int unit,
    bcm_port_t logical_port,
    int *total_free_sections)
{
    int free_size, rmc_machine_id;
    bcm_port_prio_config_t priority_config;
    dnx_port_nif_list_info_t *used_sub_lists_info = NULL;
    int core, nif_interface_id, priority_group, memory_group;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Get OFR context
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_nif_interface_id_get(unit, logical_port, 0, &core, &nif_interface_id));

    /*
     * Allocating memory for used_sub_lists_info.
     */
    SHR_ALLOC_SET_ZERO_ERR_EXIT(used_sub_lists_info,
                                sizeof(dnx_port_nif_list_info_t) * dnx_data_nif.ofr.nof_rx_memory_groups_get(unit),
                                "array of used sub-lists info", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

    /*
     * Get free lists size
     */
    for (memory_group = 0; memory_group < dnx_data_nif.ofr.nof_rx_memory_groups_get(unit); ++memory_group)
    {
        SHR_IF_ERR_EXIT(dnx_port_nif_db.ofr_link_list.list_info.size.get(unit, memory_group, &free_size));
        total_free_sections[memory_group] = free_size;
    }

    /*
     * Get number of priorities for this client
     */
    SHR_IF_ERR_EXIT(dnx_port_ofr_port_priority_config_internal_get(unit, logical_port, &priority_config));
    for (priority_group = 0; priority_group < priority_config.nof_priority_groups; ++priority_group)
    {
        SHR_IF_ERR_EXIT(dnx_port_ofr_rmc_machine_id_internal_get
                        (unit, logical_port, priority_config.priority_groups[priority_group].sch_priority,
                         &rmc_machine_id));
        SHR_IF_ERR_EXIT(dnx_port_double_priority_link_list_used_sub_lists_info_get
                        (unit, DNX_PORT_NIF_OFR_LINK_LIST, nif_interface_id, rmc_machine_id, used_sub_lists_info));

        for (memory_group = 0; memory_group < dnx_data_nif.ofr.nof_rx_memory_groups_get(unit); ++memory_group)
        {
            total_free_sections[memory_group] += used_sub_lists_info[memory_group].m_size;
        }

        sal_memset(used_sub_lists_info, 0,
                   sizeof(dnx_port_nif_list_info_t) * dnx_data_nif.ofr.nof_rx_memory_groups_get(unit));
    }

exit:
    SHR_FREE(used_sub_lists_info);
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_port_ofr_rx_fifo_status_counter_hw_map(
    int unit,
    uint32 counter,
    uint32 nif_port)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_OFR_RX_FIFO_STATUS_COUNTER_MAP, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_COUNTER_ID, counter);
    /*
     * set value fields
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_NIF_PORT_ID, INST_SINGLE, nif_port);
    /*
     * commit the value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_port_ofr_rx_fifo_status_hw_get(
    int unit,
    uint32 counter,
    uint32 priority_group,
    uint32 *max_occupancy,
    uint32 *fifo_level)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_OFR_RX_FIFO_STATUS, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_COUNTER_ID, counter);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PRIORITY_GROUP, priority_group);
    /*
     * set value fields
     */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_MAX_OCCUPANCY, INST_SINGLE, max_occupancy);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_FIFO_LEVEL, INST_SINGLE, fifo_level);
    /*
     * get entry
     */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_port_ofr_tx_fifo_status_counter_hw_map(
    int unit,
    uint32 counter,
    uint32 nif_port)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_OFT_TX_FIFO_STATUS_COUNTER_MAP, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_COUNTER_ID, counter);
    /*
     * set value fields
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_NIF_PORT_ID, INST_SINGLE, nif_port);
    /*
     * commit the value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_port_ofr_tx_fifo_status_hw_get(
    int unit,
    uint32 counter,
    uint32 *max_occupancy,
    uint32 *fifo_level)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_OFT_TX_FIFO_STATUS, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_COUNTER_ID, counter);
    /*
     * set value fields
     */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_MAX_OCCUPANCY, INST_SINGLE, max_occupancy);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_FIFO_LEVEL, INST_SINGLE, fifo_level);
    /*
     * get entry
     */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}
