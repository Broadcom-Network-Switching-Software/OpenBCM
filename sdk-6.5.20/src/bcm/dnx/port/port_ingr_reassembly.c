/** \file port_ingr_reassembly.c
 * 
 *
 * Port Ingress reassembly context management functionality for DNX.
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

#include <bcm/types.h>
#include <bcm/error.h>

#include <shared/shrextend/shrextend_debug.h>
#include <soc/memory.h>
#include <soc/feature.h>
#include <soc/types.h>
#include <soc/register.h>
#include <soc/dnxc/drv_dnxc_utils.h>

#include <soc/dnx/dbal/dbal.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_port.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_ingr_reassembly.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_nif.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/access/dnx_ingress_reassembly_access.h>

#include <bcm_int/dnx/cosq/egress/egr_queuing.h>
#include <bcm_int/dnx/mirror/mirror_rcy.h>

#include <bcm_int/dnx/port/port_ingr_reassembly.h>
#include "port_ingr_reassembly_utils.h"
#include "port_ingr_reassembly_alloc.h"

#include <bcm_int/dnx/algo/port/algo_port_mgmt.h>

#define DNX_PORT_INGR_REASSEMBLY_NON_INTERLEAVED_ID 0
#define DNX_PORT_INGR_REASSEMBLY_INVALID_INTERFACE -1

/** ------------------------------------------------------------------------------------------------------------ */
/*
 * Utils 
 */

/**
 * \brief - Returns true in is_required argument iff port interface requires reassembly context
 */
static shr_error_e
dnx_port_ingr_reassembly_port_is_context_required(
    int unit,
    bcm_port_t port,
    int *is_required)
{
    dnx_algo_port_info_s port_info;
    dnx_algo_port_tdm_mode_e tdm_mode;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, port, &port_info));

    if (!DNX_ALGO_PORT_TYPE_IS_ING_TM(unit, port_info) || DNX_ALGO_PORT_TYPE_IS_L1(unit, port_info))
    {
        *is_required = 0;
    }
    else if (DNX_ALGO_PORT_TYPE_IS_NIF_ETH(unit, port_info, 0, 0))
    {
        *is_required = 1;
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_tdm_get(unit, port, &tdm_mode));

        if (tdm_mode == DNX_ALGO_PORT_TDM_MODE_BYPASS)
        {
            *is_required = 0;
        }
        else
        {
            *is_required = 1;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Returns true in is_ingress_channelized argument iff port interface allows channels in ingress
 */
static shr_error_e
dnx_port_ingr_reassembly_port_is_ingress_channelized(
    int unit,
    bcm_port_t port,
    int *is_ingress_channelized)
{
    dnx_algo_port_info_s port_info;

    SHR_FUNC_INIT_VARS(unit);
    *is_ingress_channelized = 0;

    SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, port, &port_info));

    switch (port_info.port_type)
    {
        case DNX_ALGO_PORT_TYPE_NIF_ILKN:
        case DNX_ALGO_PORT_TYPE_CPU:
        case DNX_ALGO_PORT_TYPE_RCY:
        case DNX_ALGO_PORT_TYPE_RCY_MIRROR:
        case DNX_ALGO_PORT_TYPE_SAT:
            *is_ingress_channelized = 1;
            break;
        default:
            *is_ingress_channelized = 0;
            break;
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Returns channel number as seen by IRE module
 */
static shr_error_e
dnx_port_ingr_reassembly_channel_get(
    int unit,
    bcm_port_t port,
    int priority,
    int *channel)
{
    dnx_algo_port_info_s port_info;
    int is_channelized;

    SHR_FUNC_INIT_VARS(unit);
    *channel = DNX_ALGO_PORT_INVALID;

    SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, port, &port_info));

    if (DNX_ALGO_PORT_TYPE_IS_NIF_ETH(unit, port_info, TRUE, 0) || DNX_ALGO_PORT_TYPE_IS_FLEXE_MAC(unit, port_info, 0))
    {
        *channel = priority;
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_port_ingr_reassembly_port_is_ingress_channelized(unit, port, &is_channelized));
        if (is_channelized)
        {
            /** Get channel number for channelized interfaces */
            SHR_IF_ERR_EXIT(dnx_algo_port_channel_get(unit, port, channel));
        }
        else
        {
            *channel = DNX_ALGO_PORT_INVALID;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Returns true in should_have_context argument iff reassembly context should be configured for the port
 */

static shr_error_e
dnx_port_ingr_reassembly_port_should_set_context(
    int unit,
    bcm_port_t port,
    int *should_have_context)
{
    int is_context_required, is_ingress_channelized;

    SHR_FUNC_INIT_VARS(unit);

    *should_have_context = 0;

    /** check if reassembly context treats port's interface type */
    SHR_IF_ERR_EXIT(dnx_port_ingr_reassembly_port_is_context_required(unit, port, &is_context_required));
    *should_have_context = is_context_required;

    if (is_context_required)
    {
        SHR_IF_ERR_EXIT(dnx_port_ingr_reassembly_port_is_ingress_channelized(unit, port, &is_ingress_channelized));

        if (!is_ingress_channelized)
        {
            int is_master_port;

            /** check if the port should be skipped from the reassembly mapping */
            SHR_IF_ERR_EXIT(dnx_algo_port_is_master_get(unit, port, &is_master_port));

            if (!is_master_port)
            {
                /** for non ingress-channelized ports, only master is mapped in the reassembly context table */
                *should_have_context = 0;
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Returns base address in context map for the first channel of the port interface in context_map_address argument
 * 
 * \remark 
 * * This is the value of "Port to Base Address map" and this is used to build a key of Context Map
 */
static shr_error_e
dnx_port_ingr_reassembly_port_context_map_base_address_get(
    int unit,
    bcm_port_t port,
    int priority,
    uint32 *context_map_address)
{
    const dnx_data_ingr_reassembly_context_context_map_t *context_info;
    uint32 context_map_start_address;
    uint32 flags = 0;

    dnx_algo_port_info_s port_info;
    int nof_entries;
    int interface_offset;
    int phy_port, client_index;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, port, &port_info));

    context_info = dnx_data_ingr_reassembly.context.context_map_get(unit, port_info.port_type);
    context_map_start_address = context_info->start_index;

    /** calculate context map base address according to interface type */
    switch (port_info.port_type)
    {
        case DNX_ALGO_PORT_TYPE_NIF_ILKN:
            SHR_IF_ERR_EXIT(dnx_algo_port_interface_offset_get(unit, port, &interface_offset /** ilkn id */ ));
            nof_entries = dnx_data_port.general.max_nof_channels_get(unit);
            *context_map_address = context_map_start_address + nof_entries * interface_offset;
            break;

        case DNX_ALGO_PORT_TYPE_NIF_ETH:
            SHR_IF_ERR_EXIT(dnx_algo_port_nif_first_phy_get(unit, port, flags, &phy_port /* zero based */ ));

            interface_offset = (phy_port % dnx_data_nif.phys.nof_phys_per_core_get(unit));
            nof_entries = dnx_data_ingr_reassembly.priority.nif_eth_priorities_nof_get(unit);
            *context_map_address = context_map_start_address + nof_entries * interface_offset;
            break;

        case DNX_ALGO_PORT_TYPE_FLEXE_MAC:
            SHR_IF_ERR_EXIT(dnx_algo_port_channel_get(unit, port, &client_index));

            nof_entries = dnx_data_ingr_reassembly.priority.nif_eth_priorities_nof_get(unit);
            *context_map_address = context_map_start_address + nof_entries * client_index;
            break;
        case DNX_ALGO_PORT_TYPE_RCY_MIRROR:
            *context_map_address = context_map_start_address
                + priority * dnx_data_port.general.max_nof_channels_get(unit);
            break;
        default:
            *context_map_address = context_map_start_address;
            break;
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Returns address in context map for the port in context_map_address argument
 * 
 * \remark 
 *   * This function should be called for interfaces which use regular Context Map only
 */
static shr_error_e
dnx_port_ingr_reassembly_port_context_map_address_get(
    int unit,
    bcm_port_t port,
    int priority,
    uint32 *context_map_address)
{
    int channel = 0;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_port_ingr_reassembly_port_context_map_base_address_get(unit, port, priority,
                                                                               context_map_address));

    /** Get channel number */
    SHR_IF_ERR_EXIT(dnx_port_ingr_reassembly_channel_get(unit, port, priority, &channel));

    if (channel != DNX_ALGO_PORT_INVALID)
    {
        *context_map_address += channel;
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Returns address of interleaved bitmap for the port in bitmap_address argument
 * 
 * \remark 
 *   * This function should be called for interleaved interfaces only
 */
static shr_error_e
dnx_port_ingr_reassembly_port_interleaved_id_get(
    int unit,
    bcm_port_t port,
    int *interleaved_id)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_interface_offset_get(unit, port, interleaved_id /* ilkn id */ ));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - get port termination context for the port
 */
static shr_error_e
dnx_port_ingr_reassembly_port_termination_context_get(
    int unit,
    bcm_port_t port,
    uint32 *port_termination_context)
{

    int core_id = 0;
    uint32 tm_port;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_tm_port_get(unit, port, &core_id, &tm_port));
    *port_termination_context = tm_port;

exit:
    SHR_FUNC_EXIT;

}

/** ------------------------------------------------------------------------------------------------------ */
/*
 *   Low Level DBAL Access
 */

/**
 * \brief - Writes context_map_address at index 'index' in "port to base address" map for port
 */
static shr_error_e
dnx_port_ingr_reassembly_context_map_base_address_hw_set(
    int unit,
    bcm_core_t core_id,
    uint32 ingr_reassembly_interface,
    uint32 context_map_address,
    int is_interleaved,
    int interleaved_interface_id)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_INGR_REASSEMBLY_PORT_TO_CONTEXT_MAP_BASE_ADDRESS_TABLE,
                                      &entry_handle_id));

    /** Setting key fields */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_INGRESS_REASSEMBLY_INTERFACE,
                               ingr_reassembly_interface);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);

    /** Setting value fields */
    dbal_entry_value_field32_set(unit, entry_handle_id,
                                 DBAL_FIELD_REASSEMBLY_CONTEXT_MAP_BASE_ADDRESS, INST_SINGLE, context_map_address);

    /** Setting value fields */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_INTERLEAVED, INST_SINGLE, is_interleaved);

    /** Setting value fields */
    dbal_entry_value_field32_set(unit, entry_handle_id,
                                 DBAL_FIELD_INTERLEAVED_INTERFACE_ID, INST_SINGLE, interleaved_interface_id);

    /** Performing the action */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - set interleaved bitmap for provided port and reassembly_context
 */
static shr_error_e
dnx_port_ingr_reassembly_interleaved_context_bitmap_hw_set(
    int unit,
    bcm_core_t core_id,
    uint32 interleaved_id,
    uint32 reassembly_context,
    int is_set)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_INGR_REASSEMBLY_INTERLEAVED_REASSEMBLY_CONTEXT_BITMAP,
                                      &entry_handle_id));

    /** Setting key fields */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_INTERLEAVED_INTERFACE_ID, interleaved_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_REASSEMBLY_CONTEXT, reassembly_context);

    /** Setting value fields */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ACTIVE, INST_SINGLE, is_set);

    /** Performing the action */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - writes reassembly context for port 
 */
static shr_error_e
dnx_port_ingr_reassembly_context_hw_set(
    int unit,
    bcm_port_t port,
    uint32 context_map_address,
    uint32 reassembly_context)
{
    int core_id = 0;
    uint32 entry_handle_id;
    dnx_algo_port_info_s port_info;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, port, &core_id));

    /** all interfaces are mapped to the same DBAL table */

    SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, port, &port_info));

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_INGR_REASSEMBLY_CONTEXT_MAP_TABLE, &entry_handle_id));

    /** Setting key fields */
    dbal_entry_key_field32_set(unit, entry_handle_id,
                               DBAL_FIELD_REASSEMBLY_CONTEXT_MAP_OFFSET_EXT, context_map_address);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);

    /** Setting value fields */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_REASSEMBLY_CONTEXT, INST_SINGLE, reassembly_context);

    /** Performing the action */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - writes port termination context for port 
 */
static shr_error_e
dnx_port_ingr_reassembly_port_termination_hw_set(
    int unit,
    bcm_port_t port,
    uint32 context_map_address,
    uint32 port_termination_context)
{
    int core_id = 0;
    uint32 entry_handle_id;
    dnx_algo_port_info_s port_info;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, port, &core_id));

    /** all interfaces are mapped to the same DBAL table */

    SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, port, &port_info));

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_INGR_REASSEMBLY_CONTEXT_MAP_TABLE, &entry_handle_id));

    /** Setting key fields */
    dbal_entry_key_field32_set(unit, entry_handle_id,
                               DBAL_FIELD_REASSEMBLY_CONTEXT_MAP_OFFSET_EXT, context_map_address);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);

    /** Setting value fields */
    dbal_entry_value_field32_set(unit, entry_handle_id,
                                 DBAL_FIELD_PORT_TERMINATION_CONTEXT, INST_SINGLE, port_termination_context);

    /** Performing the action */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - read reassembly and port termination context for port 
 */
static shr_error_e
dnx_port_ingr_reassembly_reassembly_and_port_termination_hw_get(
    int unit,
    bcm_port_t port,
    uint32 context_map_address,
    uint32 *reassembly_context,
    uint32 *port_termination_context)
{
    int core_id = 0;
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, port, &core_id));

    /** All interfaces mapped to the same DBAL table */

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_INGR_REASSEMBLY_CONTEXT_MAP_TABLE, &entry_handle_id));

    /** Setting key fields */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_REASSEMBLY_CONTEXT_MAP_OFFSET_EXT,
                               context_map_address);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);

    /** Getting value fields */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_REASSEMBLY_CONTEXT, INST_SINGLE, reassembly_context);
    dbal_value_field32_request(unit, entry_handle_id,
                               DBAL_FIELD_PORT_TERMINATION_CONTEXT, INST_SINGLE, port_termination_context);

    /** Performing the action */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Calculates special interface type as defined in DBAL and returns yes/no mark in is_special_type
 * to identify ports with special interface type
 */
static shr_error_e
dnx_port_ingr_reassembly_interface_type_to_port_type_priority_get(
    int unit,
    dbal_enum_value_field_reassembly_special_interface_type_e inteface_type,
    dnx_algo_port_type_e * port_type,
    int *priority)
{

    SHR_FUNC_INIT_VARS(unit);

    *priority = 0;

    switch (inteface_type)
    {
        case DBAL_ENUM_FVAL_REASSEMBLY_SPECIAL_INTERFACE_TYPE_SAT:
            *port_type = DNX_ALGO_PORT_TYPE_SAT;
            break;
        case DBAL_ENUM_FVAL_REASSEMBLY_SPECIAL_INTERFACE_TYPE_CPU:
            *port_type = DNX_ALGO_PORT_TYPE_CPU;
            break;
        case DBAL_ENUM_FVAL_REASSEMBLY_SPECIAL_INTERFACE_TYPE_RCY:
            *port_type = DNX_ALGO_PORT_TYPE_RCY;
            break;
        case DBAL_ENUM_FVAL_REASSEMBLY_SPECIAL_INTERFACE_TYPE_MIRROR_LOSSLESS:
            *port_type = DNX_ALGO_PORT_TYPE_RCY_MIRROR;
            *priority = DNX_MIRROR_RCY_PRIORITY_LOSSLESS;
            break;
        case DBAL_ENUM_FVAL_REASSEMBLY_SPECIAL_INTERFACE_TYPE_MIRROR_HIGH_PRIORTY:
            *port_type = DNX_ALGO_PORT_TYPE_RCY_MIRROR;
            *priority = DNX_MIRROR_RCY_PRIORITY_NORMAL;
            break;
        default:
            *port_type = DNX_ALGO_PORT_TYPE_INVALID;
            break;
    }

    SHR_FUNC_EXIT;
}

/**
 * \brief - configure base address to context table for special interface 
 * when base map is used for NIF ports only
 */
static shr_error_e
dnx_port_ingr_reassembly_special_interface_base_address_hw_set(
    int unit,
    dbal_enum_value_field_reassembly_special_interface_type_e interface_type,
    uint32 base_address)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_INGR_REASSEMBLY_SPECIAL_INTERFACE_BASE_ADDRESS,
                                      &entry_handle_id));

    /** Setting key fields */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_REASSEMBLY_SPECIAL_INTERFACE_TYPE, interface_type);

    /** Setting value fields */
    dbal_entry_value_field32_set(unit, entry_handle_id,
                                 DBAL_FIELD_REASSEMBLY_CONTEXT_MAP_BASE_ADDRESS, INST_SINGLE, base_address);

    /** Performing the action */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - configure base address to context table for special interface 
 */
static shr_error_e
dnx_port_ingr_reassembly_special_interface_base_address_hw_init(
    int unit)
{
    const dnx_data_ingr_reassembly_context_context_map_t *context_info;
    dbal_enum_value_field_reassembly_special_interface_type_e interface_type;
    uint32 base_address;
    dnx_algo_port_type_e port_type;
    int priority;

    SHR_FUNC_INIT_VARS(unit);

    for (interface_type = 0; interface_type < DBAL_NOF_ENUM_REASSEMBLY_SPECIAL_INTERFACE_TYPE_VALUES; interface_type++)
    {
        SHR_IF_ERR_EXIT(dnx_port_ingr_reassembly_interface_type_to_port_type_priority_get(unit, interface_type,
                                                                                          &port_type, &priority));
        if (port_type != DNX_ALGO_PORT_TYPE_INVALID)
        {
            context_info = dnx_data_ingr_reassembly.context.context_map_get(unit, port_type);
            base_address = context_info->start_index + priority * dnx_data_port.general.max_nof_channels_get(unit);
            SHR_IF_ERR_EXIT(dnx_port_ingr_reassembly_special_interface_base_address_hw_set(unit, interface_type,
                                                                                           base_address));
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Initialize BAD_REASSEMBLY_CONTEXT table
 */
static shr_error_e
dnx_port_ingr_reassembly_bad_context_hw_init(
    int unit)
{
    uint32 entry_handle_id;
    int reassembly_context = dnx_data_ingr_reassembly.context.invalid_context_get(unit);
    int nof_contexts = dnx_data_ingr_reassembly.context.nof_contexts_get(unit);

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** mark invalid context as allocated on all cores, so it wouldn't be allocated */
    if (dnx_data_ingr_reassembly.context.invalid_context_get(unit) < nof_contexts)
    {
        SHR_IF_ERR_EXIT(dnx_port_ingr_reassembly_reassembly_context_mark_allocated(unit, reassembly_context));
    }

    /*
     * set invalid context in BAD_REASSEMBLY_CONTEXT table 
     */

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_INGR_REASSEMBLY_BAD_REASSEMBLY_CONTEXT, &entry_handle_id));

    /** Setting value fields */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_REASSEMBLY_CONTEXT, INST_SINGLE, reassembly_context);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_VALID, INST_SINGLE, 1);

    /** Performing the action */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    /*
     * configure the whole Context Map table to contain invalid reassembly context 
     */

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_INGR_REASSEMBLY_CONTEXT_MAP_TABLE, &entry_handle_id));

    /** Set Range Key Fields */
    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_REASSEMBLY_CONTEXT_MAP_OFFSET_EXT,
                                     DBAL_RANGE_ALL, DBAL_RANGE_ALL);

    /** Set core Key Fields */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, BCM_CORE_ALL);

    /** Setting value fields */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_REASSEMBLY_CONTEXT, INST_SINGLE, reassembly_context);

    /** Performing the action */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT | DBAL_COMMIT_OVERRUN));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - HW Initialization of port ingress reassembly
 */
static shr_error_e
dnx_port_ingr_reassembly_hw_init(
    int unit)
{

    SHR_FUNC_INIT_VARS(unit);

    /** intialize bad reassembly context */
    SHR_IF_ERR_EXIT(dnx_port_ingr_reassembly_bad_context_hw_init(unit));

    /** initialize base address to context table for special interfaces */
    SHR_IF_ERR_EXIT(dnx_port_ingr_reassembly_special_interface_base_address_hw_init(unit));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Returns index to "port to base address" map for the port in base_map_index argument.
 * if an interface does not use "Port to Base Map"
 * DNX_PORT_INGR_REASSEMBLY_INVALID_INTERFACE is returned
 */
static shr_error_e
dnx_port_ingr_reassembly_port_to_interface_get(
    int unit,
    bcm_port_t port,
    int *ingr_reassembly_interface)
{
    uint32 flags = 0;

    dnx_algo_port_info_s port_info;
    bcm_core_t core;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, port, &port_info));

    *ingr_reassembly_interface = DNX_PORT_INGR_REASSEMBLY_INVALID_INTERFACE;

    /** calculate index according to interface type */
    switch (port_info.port_type)
    {
        case DNX_ALGO_PORT_TYPE_NIF_ILKN:
        case DNX_ALGO_PORT_TYPE_NIF_ETH:
        case DNX_ALGO_PORT_TYPE_FLEXE_MAC:
            SHR_IF_ERR_EXIT(dnx_algo_port_nif_interface_id_get(unit, port, flags, &core, ingr_reassembly_interface));
            break;

        default:
            *ingr_reassembly_interface = DNX_PORT_INGR_REASSEMBLY_INVALID_INTERFACE;
            break;
    }

exit:
    SHR_FUNC_EXIT;
}

/** ------------------------------------------------------------------------------------------------------------------ */
/*
 *   Abstract Table: ReAssembly Context Table
 *             Key: port
 *           Value: reassembly context 
 *           Value: port termination context 
 */

/**
 * \brief - set reassembly context for port 
 */
static shr_error_e
dnx_port_ingr_reassembly_context_set(
    int unit,
    bcm_port_t port,
    int priority,
    uint32 reassembly_context)
{

    uint32 context_map_address = 0;
    int is_interleaved = 0;
    int interleaved_id = DNX_PORT_INGR_REASSEMBLY_NON_INTERLEAVED_ID;
    bcm_core_t core_id;

    SHR_FUNC_INIT_VARS(unit);

    /** "port to base address map" is configured by port termination function */

    /** compute address in context map  */
    SHR_IF_ERR_EXIT(dnx_port_ingr_reassembly_port_context_map_address_get(unit, port, priority, &context_map_address));

    /** at the end -- write the context to the HW */
    SHR_IF_ERR_EXIT(dnx_port_ingr_reassembly_context_hw_set(unit, port, context_map_address, reassembly_context));

    SHR_IF_ERR_EXIT(dnx_port_ingr_reassembly_is_interface_interleaved(unit, port, &is_interleaved));
    if (is_interleaved)
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, port, &core_id));

        SHR_IF_ERR_EXIT(dnx_port_ingr_reassembly_port_interleaved_id_get(unit, port, &interleaved_id));
        SHR_IF_ERR_EXIT(dnx_port_ingr_reassembly_interleaved_context_bitmap_hw_set(unit, core_id, interleaved_id,
                                                                                   reassembly_context, TRUE));
    }

exit:
    SHR_FUNC_EXIT;

}

/**
 * \brief - update port termination context for port
 * without configuring base table
 */
static shr_error_e
dnx_port_ingr_reassembly_port_termination_update(
    int unit,
    bcm_port_t port,
    int priority,
    uint32 port_termination_context)
{
    uint32 context_map_address = 0;

    SHR_FUNC_INIT_VARS(unit);

    /** compute address in context map  */
    SHR_IF_ERR_EXIT(dnx_port_ingr_reassembly_port_context_map_address_get(unit, port, priority, &context_map_address));

    /** write the context to the HW */
    SHR_IF_ERR_EXIT(dnx_port_ingr_reassembly_port_termination_hw_set(unit, port,
                                                                     context_map_address, port_termination_context));

exit:
    SHR_FUNC_EXIT;

}

/**
 * \brief - set port termination context for port 
 */
static shr_error_e
dnx_port_ingr_reassembly_port_termination_set(
    int unit,
    bcm_port_t port,
    int priority,
    uint32 port_termination_context)
{

    uint32 context_map_start;
    int ingress_reassembly_interface;
    dnx_algo_port_info_s port_info;
    int is_interleaved = 0;
    int interleaved_id = DNX_PORT_INGR_REASSEMBLY_NON_INTERLEAVED_ID;
    bcm_core_t core_id;

    SHR_FUNC_INIT_VARS(unit);

    /** index to "port to base address map" */
    SHR_IF_ERR_EXIT(dnx_port_ingr_reassembly_port_to_interface_get(unit, port, &ingress_reassembly_interface));

    if (ingress_reassembly_interface != DNX_PORT_INGR_REASSEMBLY_INVALID_INTERFACE)
    {
        /*
         * "Port to Base Map" is used for this interface 
         * and should be configured 
         */

        /** data for "port to base address map" */

        SHR_IF_ERR_EXIT(dnx_port_ingr_reassembly_port_context_map_base_address_get(unit, port, priority,
                                                                                   &context_map_start));

        SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, port, &port_info));
        SHR_IF_ERR_EXIT(dnx_port_ingr_reassembly_is_interface_interleaved(unit, port, &is_interleaved));

        /** calculate bitmap address for interleaved interface */
        if (is_interleaved)
        {
            SHR_IF_ERR_EXIT(dnx_port_ingr_reassembly_port_interleaved_id_get(unit, port, &interleaved_id));
        }

        SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, port, &core_id));

        /** configure "port to context base address" map */
        SHR_IF_ERR_EXIT(dnx_port_ingr_reassembly_context_map_base_address_hw_set(unit, core_id,
                                                                                 ingress_reassembly_interface,
                                                                                 context_map_start,
                                                                                 is_interleaved, interleaved_id));
    }

    /** update port termination in context map  */
    SHR_IF_ERR_EXIT(dnx_port_ingr_reassembly_port_termination_update(unit, port, priority, port_termination_context));

exit:
    SHR_FUNC_EXIT;

}

/**
 * \brief - unset reassembly and port termination context for port 
 */
static shr_error_e
dnx_port_ingr_reassembly_reassembly_and_port_termination_unset(
    int unit,
    bcm_port_t port,
    int priority,
    uint32 reassembly_context,
    uint32 port_termination_context,
    int is_dealloc)
{

    int is_interleaved, is_ingress_channelized;
    dnx_algo_port_info_s port_info;
    uint32 context_map_address = 0;
    bcm_core_t core_id;
    int handle_tdm;
    uint32 tdm_flags;
    bcm_port_t master_port;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, port, &port_info));

    if (is_dealloc)
    {
        /** the reassembly context was deallocated */
        SHR_IF_ERR_EXIT(dnx_port_ingr_reassembly_is_interface_interleaved(unit, port, &is_interleaved));

        if (DNX_ALGO_PORT_TYPE_IS_NIF_ILKN(unit, port_info, 0, 0) && is_interleaved)
        {
            int interleaved_id;

            /** calculate bitmap address for interleaved interface */
            SHR_IF_ERR_EXIT(dnx_port_ingr_reassembly_port_interleaved_id_get(unit, port, &interleaved_id));

            SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, port, &core_id));

            /** remove the port from the bitmap */
            SHR_IF_ERR_EXIT(dnx_port_ingr_reassembly_interleaved_context_bitmap_hw_set(unit, core_id, interleaved_id,
                                                                                       reassembly_context, FALSE));
        }
    }

    SHR_IF_ERR_EXIT(dnx_port_ingr_reassembly_port_is_ingress_channelized(unit, port, &is_ingress_channelized));

    SHR_IF_ERR_EXIT(dnx_port_ingr_reassembly_port_should_handle_tdm(unit, port, &handle_tdm));
    tdm_flags = (handle_tdm ? 0 : DNX_ALGO_PORT_MASTER_F_NON_TDM_BYPASS);

    /** get master port */
    SHR_IF_ERR_EXIT(dnx_algo_port_master_get(unit, port, tdm_flags, &master_port));

    /** non ingress channelized port which isn't last - we might need to switch port termination context */
    if (!is_ingress_channelized && (port == master_port) && !is_dealloc)
    {

        bcm_port_t next_master;
        uint32 new_termination_context;

        SHR_IF_ERR_EXIT(dnx_algo_port_master_get(unit, port, DNX_ALGO_PORT_MASTER_F_NEXT | tdm_flags, &next_master));

       /** get new port termination context  */
        SHR_IF_ERR_EXIT(dnx_port_ingr_reassembly_port_termination_context_get(unit, next_master,
                                                                              &new_termination_context));

        /** update port termination in context map  */
        SHR_IF_ERR_EXIT(dnx_port_ingr_reassembly_port_termination_update(unit, next_master, priority,
                                                                         new_termination_context));

    }

    /** set reassembly context in Context map as invalid */
    if (is_ingress_channelized || is_dealloc)
    {
        uint32 invalid_reassembly_context = dnx_data_ingr_reassembly.context.invalid_context_get(unit);

        /** compute address in context map  */
        SHR_IF_ERR_EXIT(dnx_port_ingr_reassembly_port_context_map_address_get(unit, port, priority,
                                                                              &context_map_address));

        SHR_IF_ERR_EXIT(dnx_port_ingr_reassembly_context_hw_set(unit, port, context_map_address,
                                                                invalid_reassembly_context));
    }

exit:
    SHR_FUNC_EXIT;

}

/** --------------------------------------------------------------------------------------------------------------- */

/*
 *  Top Level Functionality 
 */

/**
 * \brief - get reassembly context and port termination context
 * consider disabled contexts as well
 */
shr_error_e
dnx_port_ingr_reassembly_reassembly_and_port_termination_get_with_disabled(
    int unit,
    bcm_port_t port,
    int priority,
    int consider_disabled,
    uint32 *reassembly_context,
    uint32 *port_termination_context)
{

    int is_context_required;
    uint32 context_map_address = 0;

    SHR_FUNC_INIT_VARS(unit);

    *reassembly_context = DNX_PORT_INGR_REASSEMBLY_NON_INGRESS_PORT_CONTEXT;
    *port_termination_context = DNX_PORT_INGR_REASSEMBLY_NON_INGRESS_PORT_CONTEXT;

    /** check whether this port's interface uses reassembly context */
    SHR_IF_ERR_EXIT(dnx_port_ingr_reassembly_port_is_context_required(unit, port, &is_context_required));

    if (is_context_required)
    {
        /** compute address in context map  */
        SHR_IF_ERR_EXIT(dnx_port_ingr_reassembly_port_context_map_address_get(unit, port, priority,
                                                                              &context_map_address));

        /** read the context from HW */
        SHR_IF_ERR_EXIT(dnx_port_ingr_reassembly_reassembly_and_port_termination_hw_get(unit, port,
                                                                                        context_map_address,
                                                                                        reassembly_context,
                                                                                        port_termination_context));

        if (consider_disabled && *reassembly_context == dnx_data_ingr_reassembly.context.invalid_context_get(unit))
        {
            /** context is disabled */
            SHR_IF_ERR_EXIT(dnx_ingress_reassembly_db.disabled_context.get(unit, port, reassembly_context));

        }
    }

exit:
    SHR_FUNC_EXIT;

}

/*
 * See .h file
 */
shr_error_e
dnx_port_ingr_reassembly_reassembly_and_port_termination_get(
    int unit,
    bcm_port_t port,
    int priority,
    uint32 *reassembly_context,
    uint32 *port_termination_context)
{

    int consider_disabled = FALSE;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_port_ingr_reassembly_reassembly_and_port_termination_get_with_disabled(unit, port, priority,
                                                                                               consider_disabled,
                                                                                               reassembly_context,
                                                                                               port_termination_context));
exit:
    SHR_FUNC_EXIT;

}

/**
 * \brief - returns number of priorities for a port type in reassembly  module
 */
static shr_error_e
dnx_port_ingr_reassembly_interface_type_priorities_nof_get(
    int unit,
    bcm_port_t port,
    int *priorities_nof)
{
    dnx_algo_port_info_s port_info;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, port, &port_info));

    switch (port_info.port_type)
    {
        case DNX_ALGO_PORT_TYPE_NIF_ETH:
        case DNX_ALGO_PORT_TYPE_FLEXE_MAC:
            *priorities_nof = dnx_data_ingr_reassembly.priority.nif_eth_priorities_nof_get(unit);
            break;
        case DNX_ALGO_PORT_TYPE_RCY:
            *priorities_nof = 1; /** each rcy channel has single priority */
            break;
        case DNX_ALGO_PORT_TYPE_RCY_MIRROR:
            *priorities_nof = dnx_data_ingr_reassembly.priority.mirror_priorities_nof_get(unit);
            break;
        default:
            *priorities_nof = 1;
            break;
    }

exit:
    SHR_FUNC_EXIT;

}

/*
 * See .h file
 */
shr_error_e
dnx_port_ingr_reassembly_reassembly_and_port_termination_get_all(
    int unit,
    bcm_port_t port,
    uint32 reassembly_context[DNX_DATA_MAX_INGR_REASSEMBLY_PRIORITY_PRIORITIES_NOF],
    uint32 *port_termination_context)
{
    int is_context_required;
    uint32 context_map_address = 0;
    int priorities_nof, prio;

    SHR_FUNC_INIT_VARS(unit);

    /** initialize all entries -- not all ports have 2 priorities */
    for (prio = 0; prio < DNX_DATA_MAX_INGR_REASSEMBLY_PRIORITY_PRIORITIES_NOF; prio++)
    {
        reassembly_context[prio] = DNX_PORT_INGR_REASSEMBLY_NON_INGRESS_PORT_CONTEXT;
    }
    *port_termination_context = DNX_PORT_INGR_REASSEMBLY_NON_INGRESS_PORT_CONTEXT;

    /** check whether this port's interface uses reassembly context */
    SHR_IF_ERR_EXIT(dnx_port_ingr_reassembly_port_is_context_required(unit, port, &is_context_required));

    if (is_context_required)
    {
        SHR_IF_ERR_EXIT(dnx_port_ingr_reassembly_interface_type_priorities_nof_get(unit, port, &priorities_nof));

        for (prio = 0; prio < priorities_nof; prio++)
        {
            /** compute address in context map  */
            SHR_IF_ERR_EXIT(dnx_port_ingr_reassembly_port_context_map_address_get(unit, port, prio,
                                                                                  &context_map_address));

            /** at the end -- read the context from HW */
            SHR_IF_ERR_EXIT(dnx_port_ingr_reassembly_reassembly_and_port_termination_hw_get(unit, port,
                                                                                            context_map_address,
                                                                                            &reassembly_context[prio],
                                                                                            port_termination_context));
        }
    }

exit:
    SHR_FUNC_EXIT;

}

/*
 * See .h file
 */
shr_error_e
dnx_port_ingr_reassembly_context_for_cgm_get_all(
    int unit,
    bcm_port_t port,
    uint32 reassembly_context[DNX_DATA_MAX_INGR_REASSEMBLY_PRIORITY_CGM_PRIORITIES_NOF])
{
    uint32 port_termination_context;
    dnx_algo_port_info_s port_info;
    int prio;
    int core_id;
    int interface_index;
    int nof_contexts_per_rcy_if;

    SHR_FUNC_INIT_VARS(unit);

    /** initialize all contexts */
    for (prio = 0; prio < DNX_DATA_MAX_INGR_REASSEMBLY_PRIORITY_CGM_PRIORITIES_NOF; prio++)
    {
        reassembly_context[prio] = DNX_PORT_INGR_REASSEMBLY_NON_INGRESS_PORT_CONTEXT;
    }

    SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, port, &port_info));

    if (DNX_ALGO_PORT_TYPE_IS_RCY(unit, port_info))
    {
        /** Get the relevant core ID from the port */
        SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, port, &core_id));
        SHR_IF_ERR_EXIT(dnx_algo_port_interface_offset_get(unit, port, &interface_index));
        SHR_IF_ERR_EXIT(dnx_port_ingr_reassembly_nof_contexts_per_rcy_if_get(unit, &nof_contexts_per_rcy_if));

        for (prio = 0; prio < nof_contexts_per_rcy_if; prio++)
        {
            SHR_IF_ERR_EXIT(dnx_ingress_reassembly_db.rcy.
                            reassembly_context.get(unit, core_id, interface_index, prio, &reassembly_context[prio]));
        }
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_port_ingr_reassembly_reassembly_and_port_termination_get_all(unit, port,
                                                                                         reassembly_context,
                                                                                         &port_termination_context));
    }

exit:
    SHR_FUNC_EXIT;

}

/*
 * See .h file
 */
shr_error_e
dnx_port_ingr_reassembly_map_set(
    int unit,
    bcm_port_t port,
    int priority)
{
    uint32 reassembly_context = 0;
    int call_alloc;

    SHR_FUNC_INIT_VARS(unit);

    /** check whether this port's interface uses reassembly context */

    SHR_IF_ERR_EXIT(dnx_port_ingr_reassembly_should_call_alloc(unit, port, priority, &call_alloc));
    if (call_alloc)
    {
        /** no context for this port -- need to configure */

        /** for RCY port -- allocate context for both priorities and save in sw state */

        /** Allocate reassembly context */
        SHR_IF_ERR_EXIT(dnx_port_ingr_reassembly_reassembly_context_alloc(unit, port, priority, &reassembly_context));

        /** at the end -- write the context to the HW */
        SHR_IF_ERR_EXIT(dnx_port_ingr_reassembly_context_set(unit, port, priority, reassembly_context));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - 
 * verify the port can configure context per priority
 */
static shr_error_e
dnx_port_ingr_reassembly_priority_verify(
    int unit,
    bcm_port_t port)
{

    dnx_algo_port_info_s port_info;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, port, &port_info));

    switch (port_info.port_type)
    {
        case DNX_ALGO_PORT_TYPE_NIF_ETH:
        case DNX_ALGO_PORT_TYPE_RCY_MIRROR:
        case DNX_ALGO_PORT_TYPE_FLEXE_MAC:
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Unexpected port type %d", port_info.port_type);
            break;
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_port_ingr_reassembly_priority_unset(
    int unit,
    bcm_port_t port,
    int priority)
{
    uint32 reassembly_context = 0, port_termination_context, invalid_context;
    int is_deallocated;

    SHR_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_port_ingr_reassembly_priority_verify(unit, port));

    SHR_IF_ERR_EXIT(dnx_port_ingr_reassembly_reassembly_and_port_termination_get(unit, port, priority,
                                                                                 &reassembly_context,
                                                                                 &port_termination_context));

    if (reassembly_context != dnx_data_ingr_reassembly.context.invalid_context_get(unit))
    {
        /** context exist for this priority -- need to dealloc */

        /** De-Allocate reassembly context */
        SHR_IF_ERR_EXIT(dnx_port_ingr_reassembly_reassembly_context_dealloc(unit, port, priority,
                                                                            reassembly_context,
                                                                            1 /** priority_dealloc */ ,
                                                                            &is_deallocated));

        /** at the end -- write invalid context to the HW */
        invalid_context = dnx_data_ingr_reassembly.context.invalid_context_get(unit);
        SHR_IF_ERR_EXIT(dnx_port_ingr_reassembly_context_set(unit, port, priority, invalid_context));
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_port_ingr_reassembly_per_priority_context_set(
    int unit,
    bcm_port_t port,
    int per_priority_exist[])
{
    int priority, priorities_nof;

    SHR_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_port_ingr_reassembly_priority_verify(unit, port));

    SHR_IF_ERR_EXIT(dnx_port_ingr_reassembly_interface_type_priorities_nof_get(unit, port, &priorities_nof));

    for (priority = 0; priority < priorities_nof; priority++)
    {
        if (per_priority_exist[priority])
        {
            SHR_IF_ERR_EXIT(dnx_port_ingr_reassembly_map_set(unit, port, priority));
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_port_ingr_reassembly_per_priority_context_unset(
    int unit,
    bcm_port_t port,
    int per_priority_exist[])
{
    int priority, priorities_nof;

    SHR_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_port_ingr_reassembly_priority_verify(unit, port));

    SHR_IF_ERR_EXIT(dnx_port_ingr_reassembly_interface_type_priorities_nof_get(unit, port, &priorities_nof));

    for (priority = 0; priority < priorities_nof; priority++)
    {
        if (per_priority_exist[priority] == 0)
        {
            SHR_IF_ERR_EXIT(dnx_port_ingr_reassembly_priority_unset(unit, port, priority));
        }

    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - configure port termination for provided port/priority
 */
static shr_error_e
dnx_port_ingr_port_termination_map_set(
    int unit,
    bcm_port_t port,
    int priority)
{
    uint32 context = 0;
    int should_set_context;

    SHR_FUNC_INIT_VARS(unit);

    /** check whether this port has a place in reassembly context/port termination table */
    SHR_IF_ERR_EXIT(dnx_port_ingr_reassembly_port_should_set_context(unit, port, &should_set_context));

    if (should_set_context)
    {
        /** Get port termination context */
        SHR_IF_ERR_EXIT(dnx_port_ingr_reassembly_port_termination_context_get(unit, port, &context));

        /** at the end -- write the context to the HW */
        SHR_IF_ERR_EXIT(dnx_port_ingr_reassembly_port_termination_set(unit, port, priority, context));

    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_port_ingr_reassembly_port_add(
    int unit,
    bcm_port_t port)
{
    int is_context_required;
    dnx_algo_port_info_s port_info;
    int temp_prio;
    int priorities_nof;

    SHR_FUNC_INIT_VARS(unit);

    /** check if reassembly context treats port's interface type */
    SHR_IF_ERR_EXIT(dnx_port_ingr_reassembly_port_is_context_required(unit, port, &is_context_required));

    if (is_context_required)
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, port, &port_info));

        /*
         * set port termination context 
         */
        SHR_IF_ERR_EXIT(dnx_port_ingr_reassembly_interface_type_priorities_nof_get(unit, port, &priorities_nof));

        for (temp_prio = 0; temp_prio < priorities_nof; temp_prio++)
        {
            /** set port termination */
            SHR_IF_ERR_EXIT(dnx_port_ingr_port_termination_map_set(unit, port, temp_prio));
        }

        /*
         * set reassembly context for port
         *
         * (this is done from port_add)
         */
        switch (port_info.port_type)
        {
            case DNX_ALGO_PORT_TYPE_NIF_ETH:
                /** for NIF ports reassembly context is not set from port_add */
                break;
            case DNX_ALGO_PORT_TYPE_RCY_MIRROR:
                /** for Mirror ports reassembly context is not set from port_add */
                break;
            case DNX_ALGO_PORT_TYPE_RCY:
            case DNX_ALGO_PORT_TYPE_NIF_ILKN:
            case DNX_ALGO_PORT_TYPE_CPU:
            case DNX_ALGO_PORT_TYPE_SAT:
            case DNX_ALGO_PORT_TYPE_OLP:
            case DNX_ALGO_PORT_TYPE_OAMP:
            case DNX_ALGO_PORT_TYPE_IPSEC:
                /** IPSEC does not exist in Jr2 */
            case DNX_ALGO_PORT_TYPE_EVENTOR:
            case DNX_ALGO_PORT_TYPE_CRPS:
                /** set reassembly context for the port */
                SHR_IF_ERR_EXIT(dnx_port_ingr_reassembly_map_set(unit, port, 0));
                break;
            default:
                break;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - unmap reassembly and port temination for specified port/priority
 */
static shr_error_e
dnx_port_ingr_reassembly_unmap_set(
    int unit,
    bcm_port_t port,
    int priority)
{
    uint32 reassembly_context = 0, port_termination_context = 0;
    int is_dealloc;

    SHR_FUNC_INIT_VARS(unit);

    /** read reassembly and port termination from HW */
    SHR_IF_ERR_EXIT(dnx_port_ingr_reassembly_reassembly_and_port_termination_get_with_disabled(unit, port,
                                                                                               priority,
                                                                                               TRUE,
                                                                                               &reassembly_context,
                                                                                               &port_termination_context));

    if (reassembly_context != dnx_data_ingr_reassembly.context.invalid_context_get(unit))
    {
        /** Deallocate reassembly context */
        SHR_IF_ERR_EXIT(dnx_port_ingr_reassembly_reassembly_context_dealloc(unit, port, priority,
                                                                            reassembly_context,
                                                                            0 /** port dealloc, not priority */ ,
                                                                            &is_dealloc));

        /** Update HW as required */
        SHR_IF_ERR_EXIT(dnx_port_ingr_reassembly_reassembly_and_port_termination_unset(unit, port, priority,
                                                                                       reassembly_context,
                                                                                       port_termination_context,
                                                                                       is_dealloc));
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_port_ingr_reassembly_port_remove(
    int unit,
    bcm_port_t port)
{
    int is_context_required;
    dnx_algo_port_info_s port_info;
    int temp_prio;
    int priorities_nof;

    SHR_FUNC_INIT_VARS(unit);

    /** check if reassembly context treats port's interface type */
    SHR_IF_ERR_EXIT(dnx_port_ingr_reassembly_port_is_context_required(unit, port, &is_context_required));

    if (is_context_required)
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, port, &port_info));

        /** mirror reassembly context must be unmapped by port_to_rcy_port_unmap_set */
        if (!DNX_ALGO_PORT_TYPE_IS_RCY_MIRROR(unit, port_info))
        {
            /*
             * set port termination context 
             */
            SHR_IF_ERR_EXIT(dnx_port_ingr_reassembly_interface_type_priorities_nof_get(unit, port, &priorities_nof));

            for (temp_prio = 0; temp_prio < priorities_nof; temp_prio++)
            {
                /** unmap reassembly and port termination */
                /** in mirror, it is possible that 2 priorities have the same context -- need to dealloc with care */
                SHR_IF_ERR_EXIT(dnx_port_ingr_reassembly_unmap_set(unit, port, temp_prio));
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_port_ingr_reassembly_init(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    /** This function is not called under WB */

    /*
     * Init sw state instance dnx_ingress_reassembly_db.
     */
    SHR_IF_ERR_EXIT(dnx_ingress_reassembly_db.init(unit));

    /*
     * Init resource manager
     */
    SHR_IF_ERR_EXIT(dnx_port_ingr_reassembly_res_mngr_init(unit));

    /** Hw init */
    SHR_IF_ERR_EXIT(dnx_port_ingr_reassembly_hw_init(unit));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - verify function for dnx_port_ingr_reassembly_enable_set/get
 */
static shr_error_e
dnx_port_ingr_reassembly_enable_verify(
    int unit,
    bcm_port_t port)
{
    dnx_algo_port_info_s port_info;
    dnx_algo_port_tdm_mode_e tdm_mode;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, port, &port_info));

    if (!DNX_ALGO_PORT_TYPE_IS_NIF_ILKN(unit, port_info, 1, 1))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "this API is supported for ILKN ports only (port %d)\n", port);
    }
    SHR_IF_ERR_EXIT(dnx_algo_port_tdm_get(unit, port, &tdm_mode));

    if (tdm_mode == DNX_ALGO_PORT_TDM_MODE_BYPASS)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "this API is not supported for TDM bypass ports (port %d)\n", port);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - return whether an ILKN channel is enabled at ingress
 */
shr_error_e
dnx_port_ingr_reassembly_enable_get(
    int unit,
    bcm_port_t port,
    int *enable)
{
    uint32 reassembly_context;
    uint32 port_termination_context;
    int priority = 0;

    SHR_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_port_ingr_reassembly_enable_verify(unit, port));

    SHR_IF_ERR_EXIT(dnx_port_ingr_reassembly_reassembly_and_port_termination_get(unit, port, priority,
                                                                                 &reassembly_context,
                                                                                 &port_termination_context));

    *enable = (reassembly_context == dnx_data_ingr_reassembly.context.invalid_context_get(unit));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - enable/disable single ILKN channel at ingress
 */
shr_error_e
dnx_port_ingr_reassembly_enable_set(
    int unit,
    bcm_port_t port,
    int enable)
{
    int current_enable;
    uint32 reassembly_context, new_reassembly_context;
    uint32 port_termination_context;
    int priority = 0;

    SHR_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_port_ingr_reassembly_enable_verify(unit, port));
    SHR_IF_ERR_EXIT(dnx_port_ingr_reassembly_enable_get(unit, port, &current_enable));

    if (enable != current_enable)
    {
        SHR_IF_ERR_EXIT(dnx_port_ingr_reassembly_reassembly_and_port_termination_get(unit, port, priority,
                                                                                     &reassembly_context,
                                                                                     &port_termination_context));
        if (enable == FALSE)
        {
            /** disable */
            SHR_IF_ERR_EXIT(dnx_ingress_reassembly_db.disabled_context.set(unit, port, reassembly_context));

            new_reassembly_context = dnx_data_ingr_reassembly.context.invalid_context_get(unit);
        }
        else
        {
            SHR_IF_ERR_EXIT(dnx_ingress_reassembly_db.disabled_context.get(unit, port, &new_reassembly_context));
        }

        /** update the context in the HW */
        SHR_IF_ERR_EXIT(dnx_port_ingr_reassembly_context_set(unit, port, priority, new_reassembly_context));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - For ILKN interface, disable the 144B check for SOP
 */
shr_error_e
dnx_port_ingr_reassembly_sop_check_config(
    int unit,
    bcm_port_t port)
{
    dnx_algo_port_info_s port_info;
    uint32 entry_handle_id;
    dnx_algo_port_ilkn_access_info_t ilkn_access_info;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, port, &port_info));

    if (DNX_ALGO_PORT_TYPE_IS_NIF_ILKN(unit, port_info, FALSE, TRUE))
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_ilkn_access_info_get(unit, port, &ilkn_access_info));
        /** Taking a handle */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_IRE_ILKN_SOP_CTRL, &entry_handle_id));

        /** Setting key fields */
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ILKN_CORE_ID, ilkn_access_info.ilkn_core);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ILKN_PORT_ID, ilkn_access_info.port_in_core);

        /** Setting value fields */
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DISABLE_144B_CHECK, INST_SINGLE, TRUE);

        /** Performing the action */
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}
