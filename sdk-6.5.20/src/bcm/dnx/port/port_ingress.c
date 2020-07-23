/** \file port_ingress.c
 * 
 *
 * Port Ingress functionality for DNX.
 */

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#  error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_PORT

#include <include/shared/shrextend/shrextend_debug.h>
#include <bcm_int/dnx/port/port_ingress.h>
#include <bcm_int/dnx/port/port_ingr_reassembly.h>
#include <bcm_int/dnx/algo/port/algo_port_mgmt.h>
#include <soc/dnx/dbal/dbal.h>

#include "port_ingr_reassembly.h"

/**
 * \brief - configure Cpu channel -> core mapping for the provided port (if the port is CPU port)
 */
shr_error_e
dnx_port_ingress_cpu_channel_config_set(
    int unit,
    bcm_port_t port)
{
    dnx_algo_port_info_s port_info;
    int core_id = 0;
    uint32 entry_handle_id;
    int channel;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    if (dnx_data_device.general.feature_get(unit, dnx_data_device_general_multi_core))
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, port, &port_info));
        if (DNX_ALGO_PORT_TYPE_IS_CPU(unit, port_info))
        {
            SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, port, &core_id));
            SHR_IF_ERR_EXIT(dnx_algo_port_channel_get(unit, port, &channel));

            /** Taking a handle */
            SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_CPU_CHANNEL_TO_CORE_TABLE, &entry_handle_id));

            /** Setting key fields */
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CPU_CHANNEL, channel);

            /** Setting value fields */
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE, INST_SINGLE, core_id);

            /** Performing the action */
            SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
        }
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - clean configuration Cpu channel -> core mapping for the provided port (if the port is CPU port)
 */
shr_error_e
dnx_port_ingress_cpu_channel_config_unset(
    int unit,
    bcm_port_t port)
{
    dnx_algo_port_info_s port_info;
    uint32 entry_handle_id;
    int channel;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    if (dnx_data_device.general.feature_get(unit, dnx_data_device_general_multi_core))
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, port, &port_info));
        if (DNX_ALGO_PORT_TYPE_IS_CPU(unit, port_info))
        {
            SHR_IF_ERR_EXIT(dnx_algo_port_channel_get(unit, port, &channel));

            /** Taking a handle */
            SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_CPU_CHANNEL_TO_CORE_TABLE, &entry_handle_id));

            /** Setting key fields */
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CPU_CHANNEL, channel);

            /** Setting value fields */
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE, INST_SINGLE, 0);

            /** Performing the action */
            SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
        }
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_port_ingress_port_add_handle(
    int unit)
{
    bcm_port_t port;
    SHR_FUNC_INIT_VARS(unit);

    /** get logical port which is about to be added */
    SHR_IF_ERR_EXIT(dnx_algo_port_added_port_get(unit, &port));

    /** configure cpu channel -> core mapping */
    SHR_IF_ERR_EXIT(dnx_port_ingress_cpu_channel_config_set(unit, port));

    /** configure reassembly context for the port */
    SHR_IF_ERR_EXIT(dnx_port_ingr_reassembly_port_add(unit, port));

    /** configure IRE SOP check */
    SHR_IF_ERR_EXIT(dnx_port_ingr_reassembly_sop_check_config(unit, port));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_port_ingress_port_remove_handle(
    int unit)
{
    bcm_port_t port;

    SHR_FUNC_INIT_VARS(unit);

    /** get logical port which is about to be removed */
    SHR_IF_ERR_EXIT(dnx_algo_port_removed_port_get(unit, &port));

    /** clean cpu channel -> core mapping */
    SHR_IF_ERR_EXIT(dnx_port_ingress_cpu_channel_config_unset(unit, port));

    /** update reassembly context for port removal */
    SHR_IF_ERR_EXIT(dnx_port_ingr_reassembly_port_remove(unit, port));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_port_ingress_init(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    /** This function is not called under WB */

    /*
     * Init ingress reassembly module
     */
    SHR_IF_ERR_EXIT(dnx_port_ingr_reassembly_init(unit));

exit:
    SHR_FUNC_EXIT;
}
