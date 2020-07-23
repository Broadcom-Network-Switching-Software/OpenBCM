/** \file ecgm_dbal.c
* 
*
* this file contains wrapper functions
* for accessing the ECGM DBAL tables.
* each table has a function to set / get.
* in cases where the table results in more than one field
* the access is done either via designated types 
* (with all the struct's fields used to set or get)
* or via item name and value.
* 
* the types are defined in 
* egress_congestion_dbal.h 
* and 
* bcm_int/dnx/cosq/egress_congestion.h
* 
* tables are found in files:
*   hl_tm_ecgm_mapping.xml
*   hl_tm_ecgm_thresholds.xml
*/

/*
* This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
* 
* Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif

#define BSL_LOG_MODULE BSL_LS_BCMDNX_COSQ
#include <soc/dnx/dbal/dbal.h>
#include "ecgm_dbal.h"
#include <shared/utilex/utilex_bitstream.h>     /* for alpha bit manipulations */
#include <bcm_int/dnx/cmn/dnxcmn.h>     /* for dnxcmn_polling */
#include <soc/dnx/dbal/auto_generated/dbal_defines_fields.h>

/**
* static functions:
*/

/* \brief - delete fifo static functions:
*
* these functions are being called from dnx_ecgm_dbal_delete_fifo_thresholds_set / get
* according to requested fifo type (short / long packets / lcd fifo),
* to access two different tables.
* (due to HW constraints, the thresholds for short packets / tdm traffic and
* the thresholds for long packets / lcd fifo had to be placed in different dbal tables.)
*/

static shr_error_e delete_fifo_long_lcd_thresholds_set(
    uint32 unit,
    uint32 core_id,
    dnx_ecgm_delete_fifo_type_e type,
    const ecgm_delete_fifo_thresholds_t * thresholds);

static shr_error_e delete_fifo_long_lcd_thresholds_get(
    uint32 unit,
    uint32 core_id,
    dnx_ecgm_delete_fifo_type_e type,
    ecgm_delete_fifo_thresholds_t * thresholds);

static shr_error_e delete_fifo_tdm_short_thresholds_set(
    uint32 unit,
    uint32 core_id,
    const ecgm_delete_fifo_thresholds_t * thresholds);

static shr_error_e delete_fifo_tdm_short_thresholds_get(
    uint32 unit,
    uint32 core_id,
    ecgm_delete_fifo_thresholds_t * thresholds);

/*
 * \brief
 * this function is being called from core threshold tables
 * to set a specific field.
 */
static shr_error_e core_table_set_field(
    uint32 unit,
    uint32 core_id,
    dbal_tables_e table_name,
    dbal_fields_e field_name,
    uint32 set_value);

/*
* \brief
* this function is being called from core threshold tables
* to get a specific field.
*/
static shr_error_e core_table_get_field(
    uint32 unit,
    uint32 core_id,
    dbal_tables_e table_name,
    dbal_fields_e field_name,
    uint32 *value);

/*
 * \brief
 * these two internal functions are called from
 * dnx_ecgm_dbal_internal_to_port_profile_set
 * to set both memory tables (for tm port and base queue pair mapping)
 * to the same port profile id.
 */
static shr_error_e dnx_ecgm_dbal_internal_base_q_pair_to_port_profile_set(
    uint32 unit,
    uint32 core_id,
    uint32 base_q_pair,
    uint32 port_profile);

static shr_error_e dnx_ecgm_dbal_internal_tm_port_to_port_profile_set(
    uint32 unit,
    uint32 core_id,
    uint32 tm_port,
    uint32 port_profile);

/* helping function to get the msb for alpha */
static shr_error_e dnx_ecgm_dbal_alpha_msb_get(
    int unit,
    uint32 *alpha_msb);

/* ECGM_MULTICAST_QUEUE_MAP_TO_SP */

/**
 * See ecgm_dbal.h
 */
shr_error_e
dnx_ecgm_dbal_dynamic_memory_access_set(
    int unit,
    int enable)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_ECGM_ENABLE_DYNAMIC_MEMORY_ACCESS, &entry_handle_id));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ENABLE, INST_SINGLE, enable ? 1 : 0);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Gets enable dynamic memory access for ecgm block
 *
 * \param [in] unit - unit number
 * \param [in] enable - enable/disable 1/0
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e
dnx_ecgm_dbal_dynamic_memory_access_get(
    int unit,
    uint32 *enable)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_ECGM_ENABLE_DYNAMIC_MEMORY_ACCESS, &entry_handle_id));
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_ENABLE, INST_SINGLE, enable);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
* \brief - Set service pool ID for a multicast queue
*/
shr_error_e
dnx_ecgm_dbal_multicast_queue_to_sp_set(
    uint32 unit,
    uint32 core_id,
    uint32 port_profile,
    uint32 queue_offset,
    uint32 mc_pd_sp)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_ECGM_MULTICAST_QUEUE_MAP_TO_SP, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ECGM_PORT_PROFILE, port_profile);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ECGM_QUEUE_OFFSET, queue_offset);

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MC_PD_SP, INST_SINGLE, mc_pd_sp);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
* \brief -  Get service pool ID for a multicast queue
*/
shr_error_e
dnx_ecgm_dbal_multicast_queue_to_sp_get(
    uint32 unit,
    uint32 core_id,
    uint32 port_profile,
    uint32 queue_offset,
    uint32 *mc_pd_sp)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_ECGM_MULTICAST_QUEUE_MAP_TO_SP, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ECGM_PORT_PROFILE, port_profile);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ECGM_QUEUE_OFFSET, queue_offset);

    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_MC_PD_SP, INST_SINGLE, mc_pd_sp);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/* ECGM_MULTICAST_QUEUE_MAP_TO_SP_ENABLE */

/**
* \brief - Set enable bit for multicast queue to service pool mapping
*/
shr_error_e
dnx_ecgm_dbal_multicast_queue_to_sp_enable_set(
    uint32 unit,
    uint32 core_id,
    uint32 map_enable)          /* 0 = mapping disabled, 1 = mapping enabled */
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_ECGM_MULTICAST_QUEUE_MAP_TO_SP_ENABLE, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MAP_ENABLE, INST_SINGLE, map_enable);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
* \brief - Get enable bit for multicast queue to service pool mapping
*/
shr_error_e
dnx_ecgm_dbal_multicast_queue_to_sp_enable_get(
    uint32 unit,
    uint32 core_id,
    uint32 *map_enable)         /* 0 = mapping disabled, 1 = mapping enabled */
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_ECGM_MULTICAST_QUEUE_MAP_TO_SP_ENABLE, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);

    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_MAP_ENABLE, INST_SINGLE, map_enable);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/* ECGM_MAP_MC_SE */

/**
* \brief -  Set eligibility to use shared resources (data buffers)
*           per service pool and system DP.
*/
shr_error_e
dnx_ecgm_dbal_mc_se_set(
    uint32 unit,
    uint32 core_id,
    uint32 system_dp,
    uint32 service_pool,
    uint32 shared_resource_eligibility)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_ECGM_MAP_MC_SE, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_SYS_DP, system_dp);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_SP, service_pool);

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SHARED_RESOURCE_ELIGIBILITY, INST_SINGLE,
                                 shared_resource_eligibility);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
* \brief -  Get eligibility to use shared resources (data buffers)
*           per service pool and system DP.
*/
shr_error_e
dnx_ecgm_dbal_mc_se_get(
    uint32 unit,
    uint32 core_id,
    uint32 system_dp,
    uint32 service_pool,
    uint32 *shared_resource_eligibility)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_ECGM_MAP_MC_SE, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_SYS_DP, system_dp);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_SP, service_pool);

    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_SHARED_RESOURCE_ELIGIBILITY, INST_SINGLE,
                               shared_resource_eligibility);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/* ECGM_MAP_MC_TC_TO_DB_SP */

/**
* \brief -  Set a data buffers service pool id to a multicast TC.
*/
shr_error_e
dnx_ecgm_dbal_mc_tc_to_db_sp_set(
    uint32 unit,
    uint32 mc_tc,
    uint32 db_service_pool)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_ECGM_MAP_MC_TC_TO_DB_SP, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_MC_TC, mc_tc);

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DB_SP, INST_SINGLE, db_service_pool);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
* \brief -  Get a data buffers service pool id for a multicast TC.
*/
shr_error_e
dnx_ecgm_dbal_mc_tc_to_db_sp_get(
    uint32 unit,
    uint32 mc_tc,
    uint32 *db_service_pool)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_ECGM_MAP_MC_TC_TO_DB_SP, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_MC_TC, mc_tc);

    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_DB_SP, INST_SINGLE, db_service_pool);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/* ECGM_EGQ_IF_TO_INTERFACE_PROFILE_MAP */

/**
* \brief -  Set an interface profile id to an EGQ interface id
*/
shr_error_e
dnx_ecgm_dbal_egq_if_to_interface_profile_map_set(
    uint32 unit,
    uint32 core_id,
    uint32 egq_interface,
    uint32 interface_profile)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_ECGM_EGQ_IF_TO_INTERFACE_PROFILE_MAP, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_EGQ_IF, egq_interface);

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_INTERFACE_PROFILE, INST_SINGLE, interface_profile);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
* \brief -  Get the interface profile id of an EGQ interface id
*/
shr_error_e
dnx_ecgm_dbal_egq_if_to_interface_profile_map_get(
    uint32 unit,
    uint32 core_id,
    uint32 egq_interface,
    uint32 *interface_profile)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_ECGM_EGQ_IF_TO_INTERFACE_PROFILE_MAP, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_EGQ_IF, egq_interface);

    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_INTERFACE_PROFILE, INST_SINGLE, interface_profile);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/* ECGM_QUEUE_DISABLE */

/**
* \brief - Set a disable indication bit for a queue pair number.
*/
shr_error_e
dnx_ecgm_dbal_queue_disable_set(
    uint32 unit,
    uint32 core_id,
    uint32 queue_pair,
    uint32 is_queue_disabled)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_ECGM_QUEUE_DISABLE, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_QPAIR, queue_pair);

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_QUEUE_DISABLED, INST_SINGLE, is_queue_disabled);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
* \brief - Get a disable indication bit for a queue pair number.
*/
shr_error_e
dnx_ecgm_dbal_queue_disable_get(
    uint32 unit,
    uint32 core_id,
    uint32 queue_pair,
    uint32 *is_queue_disabled)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_ECGM_QUEUE_DISABLE, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_QPAIR, queue_pair);

    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_IS_QUEUE_DISABLED, INST_SINGLE, is_queue_disabled);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/* ECGM_INTERFACE_MULTICAST_THRESHOLDS */

/**
* \brief - Set a packet descriptors drop threshold for an interface profile and priority.
*/
shr_error_e
dnx_ecgm_dbal_interface_multicast_thresholds_set(
    uint32 unit,
    uint32 core_id,
    dbal_enum_value_field_ecgm_priority_e priority,
    uint32 interface_profile,
    uint32 pd_drop)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_ECGM_INTERFACE_MULTICAST_THRESHOLDS, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ECGM_PRIORITY, priority);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_INTERFACE_PROFILE, interface_profile);

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PD_DROP, INST_SINGLE, pd_drop);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
* \brief - Get the packet descriptors drop threshold for an interface profile and priority.
*/
shr_error_e
dnx_ecgm_dbal_interface_multicast_thresholds_get(
    uint32 unit,
    uint32 core_id,
    dbal_enum_value_field_ecgm_priority_e priority,
    uint32 interface_profile,
    uint32 *pd_drop)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_ECGM_INTERFACE_MULTICAST_THRESHOLDS, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ECGM_PRIORITY, priority);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_INTERFACE_PROFILE, interface_profile);

    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_PD_DROP, INST_SINGLE, pd_drop);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/* ECGM_TM_PORT_TO_PORT_PROFILE_MAP */

/**
* \brief - Set a port profile id to an OTM port.
*/
shr_error_e
dnx_ecgm_dbal_tm_port_to_port_profile_set(
    uint32 unit,
    uint32 core_id,
    uint32 tm_port,
    uint32 base_q_pair,
    uint32 port_profile)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_ecgm_dbal_internal_tm_port_to_port_profile_set(unit, core_id, tm_port, port_profile));
    SHR_IF_ERR_EXIT(dnx_ecgm_dbal_internal_base_q_pair_to_port_profile_set(unit, core_id, base_q_pair, port_profile));

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief - Get the port profile id for an OTM port.
*/
shr_error_e
dnx_ecgm_dbal_tm_port_to_port_profile_get(
    uint32 unit,
    uint32 core_id,
    uint32 tm_port,
    uint32 *port_profile)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_ECGM_TM_PORT_TO_PORT_PROFILE_MAP, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_TM_PORT, tm_port);

    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_ECGM_PORT_PROFILE, INST_SINGLE, port_profile);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/* ECGM_MULTICAST_QUEUE_TC_THRESHOLDS */

/**
* \brief - Set a reserved packet descriptors drop threshold to a multicast queue.
*/
shr_error_e
dnx_ecgm_dbal_multicast_queue_tc_thresholds_set(
    uint32 unit,
    uint32 core_id,
    uint32 port_profile,
    uint32 queue_offset,
    uint32 reserved_pd_drop)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_ECGM_MULTICAST_QUEUE_TC_THRESHOLDS, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ECGM_PORT_PROFILE, port_profile);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ECGM_QUEUE_OFFSET, queue_offset);

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESERVED_PD_DROP, INST_SINGLE, reserved_pd_drop);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
* \brief - Get the reserved packet descriptors drop threshold for a multicast queue.
*/
shr_error_e
dnx_ecgm_dbal_multicast_queue_tc_thresholds_get(
    uint32 unit,
    uint32 core_id,
    uint32 port_profile,
    uint32 queue_offset,
    uint32 *reserved_pd_drop)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_ECGM_MULTICAST_QUEUE_TC_THRESHOLDS, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ECGM_PORT_PROFILE, port_profile);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ECGM_QUEUE_OFFSET, queue_offset);

    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_RESERVED_PD_DROP, INST_SINGLE, reserved_pd_drop);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/* ECGM_MAP_SYS_TC_DP_TO_MC_COS_PARAMS */

/**
* \brief - Set multicast traffic class, drop precedence eligibility and priority to a system traffic class and drop precedence.
*/
shr_error_e
dnx_ecgm_dbal_sys_tc_dp_to_mc_cos_params_set(
    uint32 unit,
    uint32 core_id,
    uint32 sys_tc,
    uint32 sys_dp,
    const ecgm_cos_params_t * cos_params)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_ECGM_MAP_SYS_TC_DP_TO_MC_COS_PARAMS, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_SYS_TC, sys_tc);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_SYS_DP, sys_dp);

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MC_TC, INST_SINGLE, cos_params->mc_tc);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SHARED_RESOURCE_ELIGIBILITY_DP, INST_SINGLE,
                                 cos_params->se_dp);
    /**
     * MC-HP need to mapped to SP0 and MC-LP to SP1. therefore we need to revert the SP parameter while mapping the FIFOs
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MC_PRIORITY, INST_SINGLE, cos_params->mc_sp ? 0 : 1);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
* \brief -  Get multicast traffic class, drop precedence eligibility and priority to a system traffic class and drop precedence.
*/
shr_error_e
dnx_ecgm_dbal_sys_tc_dp_to_mc_cos_params_get(
    uint32 unit,
    uint32 core_id,
    uint32 sys_tc,
    uint32 sys_dp,
    ecgm_cos_params_t * cos_params)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_ECGM_MAP_SYS_TC_DP_TO_MC_COS_PARAMS, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_SYS_TC, sys_tc);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_SYS_DP, sys_dp);

    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_MC_TC, INST_SINGLE, &(cos_params->mc_tc));
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_SHARED_RESOURCE_ELIGIBILITY_DP, INST_SINGLE,
                               &(cos_params->se_dp));
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_MC_PRIORITY, INST_SINGLE, &(cos_params->mc_sp));
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
    /**
     * MC-HP is mapped to SP0 and MC-LP to SP1. therefore we need to revert FIFO result in order to get the correct SP
     */
    cos_params->mc_sp = cos_params->mc_sp ? 0 : 1;
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/* ECGM_CORE_GLOBAL_THRESHOLDS */

/**
* \brief -  Set the core's packet descriptors and data buffers drop and flow control thresholds.
*/
shr_error_e
dnx_ecgm_dbal_core_global_threshold_set(
    uint32 unit,
    uint32 core_id,
    dbal_fields_e field_name,
    uint32 set_value)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(core_table_set_field(unit, core_id, DBAL_TABLE_ECGM_CORE_GLOBAL_THRESHOLDS, field_name, set_value));

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief -  Get the core's packet descriptors and data buffers drop and flow control thresholds.
*/
shr_error_e
dnx_ecgm_dbal_core_global_threshold_get(
    uint32 unit,
    uint32 core_id,
    dbal_fields_e field_name,
    uint32 *value)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(core_table_get_field(unit, core_id, DBAL_TABLE_ECGM_CORE_GLOBAL_THRESHOLDS, field_name, value));

exit:
    SHR_FUNC_EXIT;
}

/* ECGM_CORE_UNICAST_THRESHOLDS */

/**
* \brief -  Set the core's packet descriptors and data buffers drop and flow control thresholds for unicast traffic.
*/
shr_error_e
dnx_ecgm_dbal_core_unicast_threshold_set(
    uint32 unit,
    uint32 core_id,
    dbal_fields_e field_name,
    uint32 set_value)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(core_table_set_field
                    (unit, core_id, DBAL_TABLE_ECGM_CORE_UNICAST_THRESHOLDS, field_name, set_value));
exit:
    SHR_FUNC_EXIT;
}

/**
* \brief -  Get the core's packet descriptors and data buffers drop and flow control thresholds for unicast traffic.
*/
shr_error_e
dnx_ecgm_dbal_core_unicast_threshold_get(
    uint32 unit,
    uint32 core_id,
    dbal_fields_e field_name,
    uint32 *value)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(core_table_get_field(unit, core_id, DBAL_TABLE_ECGM_CORE_UNICAST_THRESHOLDS, field_name, value));
exit:
    SHR_FUNC_EXIT;
}

/* ECGM_CORE_MULTICAST_THRESHOLDS */

/**
* \brief -  Set the core's packet descriptors and data buffers drop and flow control thresholds for multicast traffic.
*/
shr_error_e
dnx_ecgm_dbal_core_multicast_threshold_set(
    uint32 unit,
    uint32 core_id,
    dbal_fields_e field_name,
    uint32 set_value)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(core_table_set_field
                    (unit, core_id, DBAL_TABLE_ECGM_CORE_MULTICAST_THRESHOLDS, field_name, set_value));
exit:
    SHR_FUNC_EXIT;
}

/**
* \brief -  Get the core's packet descriptors and data buffers drop and flow control thresholds for multicast traffic.
*/
shr_error_e
dnx_ecgm_dbal_core_multicast_threshold_get(
    uint32 unit,
    uint32 core_id,
    dbal_fields_e field_name,
    uint32 *value)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(core_table_get_field(unit, core_id, DBAL_TABLE_ECGM_CORE_MULTICAST_THRESHOLDS, field_name, value));
exit:
    SHR_FUNC_EXIT;
}

/* ECGM_CORE_MULTICAST_SP_THRESHOLDS */

/**
* \brief -  Set resources thresholds for a service pool:
*           drop, flow control and reserved thresholds
*           for packet descriptors and data buffers.
*/
shr_error_e
dnx_ecgm_dbal_core_multicast_sp_threshold_set(
    uint32 unit,
    uint32 core_id,
    uint32 service_pool_id,
    dbal_fields_e field_name,
    uint32 value)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_ECGM_CORE_MULTICAST_SP_THRESHOLDS, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_SERVICE_POOL_ID, service_pool_id);

    dbal_entry_value_field32_set(unit, entry_handle_id, field_name, INST_SINGLE, value);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
* \brief -  Get resources thresholds for a service pool:
*           drop, flow control and reserved thresholds
*           for packet descriptors and data buffers.
*/
shr_error_e
dnx_ecgm_dbal_core_multicast_sp_threshold_get(
    uint32 unit,
    uint32 core_id,
    uint32 service_pool_id,
    dbal_fields_e field_name,
    uint32 *value)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_ECGM_CORE_MULTICAST_SP_THRESHOLDS, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_SERVICE_POOL_ID, service_pool_id);

    dbal_value_field32_request(unit, entry_handle_id, field_name, INST_SINGLE, value);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/* ECGM_CORE_TC_MULTICAST_THRESHOLDS */

/**
* \brief -  Set data buffers and packet descriptors flow control thresholds for a multicast traffic class.
*/
shr_error_e
dnx_ecgm_dbal_core_tc_multicast_threshold_set(
    uint32 unit,
    uint32 core_id,
    uint32 tc,
    dbal_fields_e field_name,
    uint32 value)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_ECGM_CORE_TC_MULTICAST_THRESHOLDS, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_TC, tc);
    dbal_entry_value_field32_set(unit, entry_handle_id, field_name, INST_SINGLE, value);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
* \brief -  Get data buffers and packet descriptors flow control thresholds for a multicast traffic class.
*/
shr_error_e
dnx_ecgm_dbal_core_tc_multicast_threshold_get(
    uint32 unit,
    uint32 core_id,
    uint32 tc,
    dbal_fields_e field_name,
    uint32 *value)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_ECGM_CORE_TC_MULTICAST_THRESHOLDS, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_TC, tc);
    dbal_value_field32_request(unit, entry_handle_id, field_name, INST_SINGLE, value);

    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/* ECGM_CORE_TC_MULTICAST_SP_THRESHOLDS */

/**
* \brief -  Set multicast service pool resource thresholds for a traffic class.
*/
shr_error_e
dnx_ecgm_dbal_core_tc_multicast_sp_threshold_set(
    uint32 unit,
    uint32 core_id,
    uint32 tc,
    uint32 service_pool_id,
    dbal_fields_e field_name,
    int value)
{
    uint32 entry_handle_id;
    uint32 alpha[1];
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_ECGM_CORE_TC_MULTICAST_SP_THRESHOLDS, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_TC, tc);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_SERVICE_POOL_ID, service_pool_id);

    if (field_name == DBAL_FIELD_DB_DROP_ALPHA_0 || field_name == DBAL_FIELD_DB_DROP_ALPHA_1)
    {
        if (value < 0)
        {
            SHR_IF_ERR_EXIT(dnx_ecgm_dbal_alpha_int_to_hw_get(unit, value, alpha));
            value = (int) alpha[0];
        }
    }

    dbal_entry_value_field32_set(unit, entry_handle_id, field_name, INST_SINGLE, value);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
* \brief -  Get multicast service pool resource thresholds for a traffic class.
*/
shr_error_e
dnx_ecgm_dbal_core_tc_multicast_sp_threshold_get(
    uint32 unit,
    uint32 core_id,
    uint32 tc,
    uint32 service_pool_id,
    dbal_fields_e field_name,
    int *value)
{
    uint32 entry_handle_id;
    uint32 alpha;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_ECGM_CORE_TC_MULTICAST_SP_THRESHOLDS, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_TC, tc);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_SERVICE_POOL_ID, service_pool_id);

    dbal_value_field32_request(unit, entry_handle_id, field_name, INST_SINGLE, (uint32 *) value);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

    if (field_name == DBAL_FIELD_DB_DROP_ALPHA_0 || field_name == DBAL_FIELD_DB_DROP_ALPHA_1)
    {
        alpha = (uint32) *value;
        SHR_IF_ERR_EXIT(dnx_ecgm_dbal_alpha_hw_to_int_get(unit, alpha, value));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/* ECGM_INTERFACE_UNICAST_THRESHOLDS */

/**
* \brief -  Set data buffers and packet descriptors flow control thresholds
*           for unicast interface profile and priority.
*/
shr_error_e
dnx_ecgm_dbal_interface_unicast_thresholds_set(
    uint32 unit,
    uint32 core_id,
    dbal_enum_value_field_ecgm_priority_e priority,
    uint32 interface_profile,
    const dnx_ecgm_uc_fc_thresholds_t * thresholds)
{
    uint32 alpha[1];
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_ECGM_INTERFACE_UNICAST_THRESHOLDS, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ECGM_PRIORITY, priority);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_INTERFACE_PROFILE, interface_profile);

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PD_FLOW_CONTROL_MAX, INST_SINGLE,
                                 thresholds->pd_fc.max);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PD_FLOW_CONTROL_MIN, INST_SINGLE,
                                 thresholds->pd_fc.min);

    if (thresholds->pd_fc.alpha < 0)
    {
        SHR_IF_ERR_EXIT(dnx_ecgm_dbal_alpha_int_to_hw_get(unit, thresholds->pd_fc.alpha, alpha));
    }

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PD_FLOW_CONTROL_ALPHA, INST_SINGLE,
                                 (thresholds->pd_fc.alpha < 0) ? alpha[0] : thresholds->pd_fc.alpha);

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DB_FLOW_CONTROL_MAX, INST_SINGLE,
                                 thresholds->db_fc.max);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DB_FLOW_CONTROL_MIN, INST_SINGLE,
                                 thresholds->db_fc.min);

    if (thresholds->db_fc.alpha < 0)
    {
        SHR_IF_ERR_EXIT(dnx_ecgm_dbal_alpha_int_to_hw_get(unit, thresholds->db_fc.alpha, alpha));
    }
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DB_FLOW_CONTROL_ALPHA, INST_SINGLE,
                                 (thresholds->db_fc.alpha < 0) ? alpha[0] : thresholds->db_fc.alpha);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
* \brief -  Get data buffers and packet descriptors flow control thresholds
*           for unicast interface profile and priority.
*/
shr_error_e
dnx_ecgm_dbal_interface_unicast_thresholds_get(
    uint32 unit,
    uint32 core_id,
    dbal_enum_value_field_ecgm_priority_e priority,
    uint32 interface_profile,
    dnx_ecgm_uc_fc_thresholds_t * thresholds)
{
    uint32 alpha;
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_ECGM_INTERFACE_UNICAST_THRESHOLDS, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ECGM_PRIORITY, priority);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_INTERFACE_PROFILE, interface_profile);

    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_PD_FLOW_CONTROL_MAX, INST_SINGLE,
                               &(thresholds->pd_fc.max));
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_PD_FLOW_CONTROL_MIN, INST_SINGLE,
                               &(thresholds->pd_fc.min));

    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_PD_FLOW_CONTROL_ALPHA, INST_SINGLE, &(alpha));

    SHR_IF_ERR_EXIT(dnx_ecgm_dbal_alpha_hw_to_int_get(unit, alpha, &(thresholds->pd_fc.alpha)));

    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_DB_FLOW_CONTROL_MAX, INST_SINGLE,
                               &(thresholds->db_fc.max));
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_DB_FLOW_CONTROL_MIN, INST_SINGLE,
                               &(thresholds->db_fc.min));
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_DB_FLOW_CONTROL_ALPHA, INST_SINGLE, &(alpha));

    SHR_IF_ERR_EXIT(dnx_ecgm_dbal_alpha_hw_to_int_get(unit, alpha, &(thresholds->db_fc.alpha)));

    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/* ECGM_UNICAST_PORT_THRESHOLDS */

/**
* \brief -  Set data buffers and packet descriptors thresholds for port profile.
*/
shr_error_e
dnx_ecgm_dbal_unicast_port_thresholds_set(
    uint32 unit,
    uint32 core_id,
    uint32 port_profile,
    const dnx_ecgm_uc_thresholds_t * thresholds)
{
    uint32 alpha[1];
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_ECGM_UNICAST_PORT_THRESHOLDS, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ECGM_PORT_PROFILE, port_profile);

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PD_DROP, INST_SINGLE, thresholds->pd_drop);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DB_DROP, INST_SINGLE, thresholds->db_drop);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PD_FLOW_CONTROL_MAX, INST_SINGLE,
                                 thresholds->flow_control.pd_fc.max);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PD_FLOW_CONTROL_MIN, INST_SINGLE,
                                 thresholds->flow_control.pd_fc.min);

    if (thresholds->flow_control.pd_fc.alpha < 0)
    {
        SHR_IF_ERR_EXIT(dnx_ecgm_dbal_alpha_int_to_hw_get(unit, thresholds->flow_control.pd_fc.alpha, alpha));
    }

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PD_FLOW_CONTROL_ALPHA, INST_SINGLE,
                                 (thresholds->flow_control.pd_fc.alpha <
                                  0) ? alpha[0] : thresholds->flow_control.pd_fc.alpha);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DB_FLOW_CONTROL_MAX, INST_SINGLE,
                                 thresholds->flow_control.db_fc.max);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DB_FLOW_CONTROL_MIN, INST_SINGLE,
                                 thresholds->flow_control.db_fc.min);

    if (thresholds->flow_control.db_fc.alpha < 0)
    {
        SHR_IF_ERR_EXIT(dnx_ecgm_dbal_alpha_int_to_hw_get(unit, thresholds->flow_control.db_fc.alpha, alpha));
    }

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DB_FLOW_CONTROL_ALPHA, INST_SINGLE,
                                 (thresholds->flow_control.db_fc.alpha <
                                  0) ? alpha[0] : thresholds->flow_control.db_fc.alpha);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
* \brief -  Get data buffers and packet descriptors thresholds for port profile.
*/
shr_error_e
dnx_ecgm_dbal_unicast_port_thresholds_get(
    uint32 unit,
    uint32 core_id,
    uint32 port_profile,
    dnx_ecgm_uc_thresholds_t * thresholds)
{
    uint32 alpha;
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_ECGM_UNICAST_PORT_THRESHOLDS, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ECGM_PORT_PROFILE, port_profile);

    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_PD_DROP, INST_SINGLE, &(thresholds->pd_drop));
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_DB_DROP, INST_SINGLE, &(thresholds->db_drop));
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_PD_FLOW_CONTROL_MAX, INST_SINGLE,
                               &(thresholds->flow_control.pd_fc.max));
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_PD_FLOW_CONTROL_MIN, INST_SINGLE,
                               &(thresholds->flow_control.pd_fc.min));
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_PD_FLOW_CONTROL_ALPHA, INST_SINGLE, &(alpha));

    SHR_IF_ERR_EXIT(dnx_ecgm_dbal_alpha_hw_to_int_get(unit, alpha, &(thresholds->flow_control.pd_fc.alpha)));

    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_DB_FLOW_CONTROL_MAX, INST_SINGLE,
                               &(thresholds->flow_control.db_fc.max));
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_DB_FLOW_CONTROL_MIN, INST_SINGLE,
                               &(thresholds->flow_control.db_fc.min));
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_DB_FLOW_CONTROL_ALPHA, INST_SINGLE, &(alpha));

    SHR_IF_ERR_EXIT(dnx_ecgm_dbal_alpha_hw_to_int_get(unit, alpha, &(thresholds->flow_control.db_fc.alpha)));

    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/* ECGM_UNICAST_QUEUE_THRESHOLDS */

/**
* \brief -  Set data buffers and packet descriptors thresholds for a unicast queue.
*/
shr_error_e
dnx_ecgm_dbal_unicast_queue_thresholds_set(
    uint32 unit,
    uint32 core_id,
    uint32 port_profile,
    uint32 queue_offset,
    const dnx_ecgm_uc_thresholds_t * thresholds)
{
    uint32 alpha[1];
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_ECGM_UNICAST_QUEUE_THRESHOLDS, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ECGM_PORT_PROFILE, port_profile);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ECGM_QUEUE_OFFSET, queue_offset);

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PD_DROP, INST_SINGLE, thresholds->pd_drop);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DB_DROP, INST_SINGLE, thresholds->db_drop);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PD_FLOW_CONTROL_MAX, INST_SINGLE,
                                 thresholds->flow_control.pd_fc.max);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PD_FLOW_CONTROL_MIN, INST_SINGLE,
                                 thresholds->flow_control.pd_fc.min);

    if (thresholds->flow_control.pd_fc.alpha < 0)
    {
        SHR_IF_ERR_EXIT(dnx_ecgm_dbal_alpha_int_to_hw_get(unit, thresholds->flow_control.pd_fc.alpha, alpha));
    }

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PD_FLOW_CONTROL_ALPHA, INST_SINGLE,
                                 (thresholds->flow_control.pd_fc.alpha <
                                  0) ? alpha[0] : thresholds->flow_control.pd_fc.alpha);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DB_FLOW_CONTROL_MAX, INST_SINGLE,
                                 thresholds->flow_control.db_fc.max);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DB_FLOW_CONTROL_MIN, INST_SINGLE,
                                 thresholds->flow_control.db_fc.min);

    if (thresholds->flow_control.db_fc.alpha < 0)
    {
        SHR_IF_ERR_EXIT(dnx_ecgm_dbal_alpha_int_to_hw_get(unit, thresholds->flow_control.db_fc.alpha, alpha));
    }

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DB_FLOW_CONTROL_ALPHA, INST_SINGLE,
                                 (thresholds->flow_control.db_fc.alpha <
                                  0) ? alpha[0] : thresholds->flow_control.db_fc.alpha);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
* \brief -  Get data buffers and packet descriptors thresholds for a unicast queue.
*/
shr_error_e
dnx_ecgm_dbal_unicast_queue_thresholds_get(
    uint32 unit,
    uint32 core_id,
    uint32 port_profile,
    uint32 queue_offset,
    dnx_ecgm_uc_thresholds_t * thresholds)
{
    uint32 alpha;
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_ECGM_UNICAST_QUEUE_THRESHOLDS, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ECGM_PORT_PROFILE, port_profile);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ECGM_QUEUE_OFFSET, queue_offset);

    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_PD_DROP, INST_SINGLE, &(thresholds->pd_drop));
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_DB_DROP, INST_SINGLE, &(thresholds->db_drop));
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_PD_FLOW_CONTROL_MAX, INST_SINGLE,
                               &(thresholds->flow_control.pd_fc.max));
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_PD_FLOW_CONTROL_MIN, INST_SINGLE,
                               &(thresholds->flow_control.pd_fc.min));
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_PD_FLOW_CONTROL_ALPHA, INST_SINGLE, &(alpha));

    SHR_IF_ERR_EXIT(dnx_ecgm_dbal_alpha_hw_to_int_get(unit, alpha, &(thresholds->flow_control.pd_fc.alpha)));

    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_DB_FLOW_CONTROL_MAX, INST_SINGLE,
                               &(thresholds->flow_control.db_fc.max));
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_DB_FLOW_CONTROL_MIN, INST_SINGLE,
                               &(thresholds->flow_control.db_fc.min));
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_DB_FLOW_CONTROL_ALPHA, INST_SINGLE, &(alpha));

    SHR_IF_ERR_EXIT(dnx_ecgm_dbal_alpha_hw_to_int_get(unit, alpha, &(thresholds->flow_control.db_fc.alpha)));

    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}
/* ECGM_MULTICAST_PORT_THRESHOLDS */

/**
* \brief -  Set data buffers and packet descriptors drop thresholds for a multicast port.
*/
shr_error_e
dnx_ecgm_dbal_multicast_port_thresholds_set(
    uint32 unit,
    uint32 core_id,
    uint32 port_profile,
    const dnx_ecgm_mc_drop_thresholds_t * thresholds)
{
    uint32 alpha[1];
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_ECGM_MULTICAST_PORT_THRESHOLDS, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ECGM_PORT_PROFILE, port_profile);

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DB_DROP, INST_SINGLE, thresholds->db_drop);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PD_DROP_MAX, INST_SINGLE, thresholds->pd_drop.max);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PD_DROP_MIN, INST_SINGLE, thresholds->pd_drop.min);

    if (thresholds->pd_drop.alpha < 0)
    {
        SHR_IF_ERR_EXIT(dnx_ecgm_dbal_alpha_int_to_hw_get(unit, thresholds->pd_drop.alpha, alpha));
    }

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PD_DROP_ALPHA, INST_SINGLE,
                                 (thresholds->pd_drop.alpha < 0) ? alpha[0] : thresholds->pd_drop.alpha);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
* \brief -  Get data buffers and packet descriptors drop thresholds for a multicast port.
*/
shr_error_e
dnx_ecgm_dbal_multicast_port_thresholds_get(
    uint32 unit,
    uint32 core_id,
    uint32 port_profile,
    dnx_ecgm_mc_drop_thresholds_t * thresholds)
{
    uint32 alpha;
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_ECGM_MULTICAST_PORT_THRESHOLDS, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ECGM_PORT_PROFILE, port_profile);

    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_DB_DROP, INST_SINGLE, &(thresholds->db_drop));
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_PD_DROP_MAX, INST_SINGLE, &(thresholds->pd_drop.max));
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_PD_DROP_MIN, INST_SINGLE, &(thresholds->pd_drop.min));
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_PD_DROP_ALPHA, INST_SINGLE, &(alpha));

    SHR_IF_ERR_EXIT(dnx_ecgm_dbal_alpha_hw_to_int_get(unit, alpha, &(thresholds->pd_drop.alpha)));

    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/* ECGM_MULTICAST_QUEUE_TC_DP_THRESHOLDS */

/**
* \brief -  Set data buffers and packet descriptors drop thresholds for a multicast queue and dp.
*/
shr_error_e
dnx_ecgm_dbal_multicast_queue_tc_dp_thresholds_set(
    uint32 unit,
    uint32 core_id,
    uint32 port_profile,
    uint32 queue_offset,
    uint32 dp,
    const dnx_ecgm_mc_drop_thresholds_t * thresholds)
{
    uint32 alpha[1];
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_ECGM_MULTICAST_QUEUE_TC_DP_THRESHOLDS, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ECGM_PORT_PROFILE, port_profile);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ECGM_QUEUE_OFFSET, queue_offset);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_DP, dp);

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DB_DROP, INST_SINGLE, thresholds->db_drop);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PD_DROP_MAX, INST_SINGLE, thresholds->pd_drop.max);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PD_DROP_MIN, INST_SINGLE, thresholds->pd_drop.min);

    if (thresholds->pd_drop.alpha < 0)
    {
        SHR_IF_ERR_EXIT(dnx_ecgm_dbal_alpha_int_to_hw_get(unit, thresholds->pd_drop.alpha, alpha));
    }

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PD_DROP_ALPHA, INST_SINGLE,
                                 (thresholds->pd_drop.alpha < 0) ? alpha[0] : thresholds->pd_drop.alpha);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
* \brief -  Get data buffers and packet descriptors drop thresholds for a multicast queue and dp.
*/
shr_error_e
dnx_ecgm_dbal_multicast_queue_tc_dp_thresholds_get(
    uint32 unit,
    uint32 core_id,
    uint32 port_profile,
    uint32 queue_offset,
    uint32 dp,
    dnx_ecgm_mc_drop_thresholds_t * thresholds)
{
    uint32 alpha;
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_ECGM_MULTICAST_QUEUE_TC_DP_THRESHOLDS, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ECGM_PORT_PROFILE, port_profile);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ECGM_QUEUE_OFFSET, queue_offset);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_DP, dp);

    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_DB_DROP, INST_SINGLE, &(thresholds->db_drop));
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_PD_DROP_MAX, INST_SINGLE, &(thresholds->pd_drop.max));
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_PD_DROP_MIN, INST_SINGLE, &(thresholds->pd_drop.min));
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_PD_DROP_ALPHA, INST_SINGLE, &(alpha));

    SHR_IF_ERR_EXIT(dnx_ecgm_dbal_alpha_hw_to_int_get(unit, alpha, &thresholds->pd_drop.alpha));

    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/* ECGM_DELETE_FIFO_MODE */

/**
* \brief -  Set delete FIFO mode.
*/
shr_error_e
dnx_ecgm_dbal_delete_fifo_mode_set(
    uint32 unit,
    uint32 core_id,
    ecgm_delete_fifo_mode_e mode)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_ECGM_DELETE_FIFO_MODE, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ECGM_DELETE_FIFO_MODE, INST_SINGLE, mode);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
* \brief -  Get delete FIFO mode.
*/
shr_error_e
dnx_ecgm_dbal_delete_fifo_mode_get(
    uint32 unit,
    uint32 core_id,
    ecgm_delete_fifo_mode_e * mode)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_ECGM_DELETE_FIFO_MODE, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);

    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_ECGM_DELETE_FIFO_MODE, INST_SINGLE, mode);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/* DELETE FIFOs THRESHOLDS CONFIGURATION */

/**
* \brief -  Set delete FIFO thresholds.
*/
shr_error_e
dnx_ecgm_dbal_delete_fifo_thresholds_set(
    uint32 unit,
    uint32 core_id,
    dnx_ecgm_delete_fifo_type_e type,
    const ecgm_delete_fifo_thresholds_t * thresholds)
{
    SHR_FUNC_INIT_VARS(unit);

    if (type == ECGM_LONG_PACKETS_FIFO || type == ECGM_LCD_FIFO)
    {
        SHR_IF_ERR_EXIT(delete_fifo_long_lcd_thresholds_set(unit, core_id, type, thresholds));
    }

    else if (type == ECGM_TDM_AND_SHORT_PACKETS)
    {
        SHR_IF_ERR_EXIT(delete_fifo_tdm_short_thresholds_set(unit, core_id, thresholds));
    }

    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "delete fifo type %d is invalid", type);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief -  Get delete FIFO thresholds.
*/
shr_error_e
dnx_ecgm_dbal_delete_fifo_thresholds_get(
    uint32 unit,
    uint32 core_id,
    dnx_ecgm_delete_fifo_type_e type,
    ecgm_delete_fifo_thresholds_t * thresholds)
{
    SHR_FUNC_INIT_VARS(unit);

    if (type == ECGM_LONG_PACKETS_FIFO || type == ECGM_LCD_FIFO)
    {
        SHR_IF_ERR_EXIT(delete_fifo_long_lcd_thresholds_get(unit, core_id, type, thresholds));
    }

    else if (type == ECGM_TDM_AND_SHORT_PACKETS)
    {
        SHR_IF_ERR_EXIT(delete_fifo_tdm_short_thresholds_get(unit, core_id, thresholds));
    }

    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "delete fifo type %d is invalid", type);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief - see .h file.
*/
shr_error_e
dnx_ecgm_dbal_queue_pd_count_zero_poll(
    uint32 unit,
    uint32 core_id,
    sal_usecs_t time_out,
    int32 min_polls,
    uint32 queue_pair,
    int is_multicast)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_ECGM_QUEUE_PD_COUNT, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_QPAIR, queue_pair);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_MC, is_multicast);
    SHR_IF_ERR_EXIT(dnxcmn_polling(unit, time_out, min_polls, entry_handle_id, DBAL_FIELD_PD_COUNT, 0));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/* alpha bits handling */
shr_error_e
dnx_ecgm_dbal_alpha_int_to_hw_get(
    int unit,
    int alpha_int,
    uint32 *alpha_uint32)
{
    uint32 mask = 0;
    uint32 alpha_msb;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_ecgm_dbal_alpha_msb_get(unit, &alpha_msb));
    SHR_IF_ERR_EXIT(dnx_ecgm_dbal_alpha_max_val_get(unit, &mask));

    *alpha_uint32 = 0;
    alpha_int *= (-1);
    *alpha_uint32 = alpha_int & mask;
    SHR_IF_ERR_EXIT(utilex_bitstream_set_bit(alpha_uint32, alpha_msb));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_ecgm_dbal_alpha_hw_to_int_get(
    int unit,
    uint32 alpha_uint32,
    int *alpha_int)
{
    uint32 mask = 0;
    uint32 alpha_msb;
    uint32 alpha_uint32_arr[1] = { alpha_uint32 };
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_ecgm_dbal_alpha_msb_get(unit, &alpha_msb));
    SHR_IF_ERR_EXIT(dnx_ecgm_dbal_alpha_max_val_get(unit, &mask));

    *alpha_int = 0;
    *alpha_int = alpha_uint32 & mask;

    if (utilex_bitstream_test_bit(alpha_uint32_arr, alpha_msb))
    {
        *alpha_int *= (-1);
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_ecgm_dbal_alpha_max_val_get(
    int unit,
    uint32 *max_alpha)
{
    uint32 alpha_msb;
    SHR_FUNC_INIT_VARS(unit);

    *max_alpha = 0;

    SHR_IF_ERR_EXIT(dnx_ecgm_dbal_alpha_msb_get(unit, &alpha_msb));
    SHR_IF_ERR_EXIT(utilex_bitstream_set_bit_range(max_alpha, 0, alpha_msb - 1));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_ecgm_dbal_tc_sp_threshold_mode_set(
    uint32 unit,
    uint32 core_id,
    ecgm_tc_sp_threshold_mode_e pd_mode,
    ecgm_tc_sp_threshold_mode_e db_mode)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC
                    (unit, DBAL_TABLE_ECGM_THRESHOLDS_STRICT_PRIORITY_OR_DISCRETE_PARTITIONING, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ECGM_SP_TC_TH_MODE_PD, INST_SINGLE, pd_mode);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ECGM_SP_TC_TH_MODE_DB, INST_SINGLE, db_mode);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_ecgm_dbal_tc_sp_threshold_mode_get(
    uint32 unit,
    uint32 core_id,
    ecgm_tc_sp_threshold_mode_e * pd_mode,
    ecgm_tc_sp_threshold_mode_e * db_mode)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC
                    (unit, DBAL_TABLE_ECGM_THRESHOLDS_STRICT_PRIORITY_OR_DISCRETE_PARTITIONING, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);

    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_ECGM_SP_TC_TH_MODE_PD, INST_SINGLE, pd_mode);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_ECGM_SP_TC_TH_MODE_DB, INST_SINGLE, db_mode);

    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_ecgm_dbal_core_global_resources_statistics_current_get(
    uint32 unit,
    uint32 core_id,
    ecgm_core_global_resources_statistics_t * core_resource)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC
                    (unit, DBAL_TABLE_ECGM_CORE_GLOBAL_RESOURCES_STATISTICS_CURRENT, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);

    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_TOTAL_PD, INST_SINGLE, &(core_resource->total_pd));
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_UC_PD, INST_SINGLE, &(core_resource->uc_pd));
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_MC_PD, INST_SINGLE, &(core_resource->mc_pd));
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_TOTAL_DB, INST_SINGLE, &(core_resource->total_db));
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_UC_DB, INST_SINGLE, &(core_resource->uc_db));
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_MC_DB, INST_SINGLE, &(core_resource->mc_db));

    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_ecgm_dbal_core_global_resources_statistics_max_get(
    uint32 unit,
    uint32 core_id,
    ecgm_core_global_resources_statistics_t * core_resource)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_ECGM_CORE_GLOBAL_RESOURCES_STATISTICS_MAX, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);

    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_TOTAL_PD, INST_SINGLE, &(core_resource->total_pd));
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_UC_PD, INST_SINGLE, &(core_resource->uc_pd));
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_MC_PD, INST_SINGLE, &(core_resource->mc_pd));
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_TOTAL_DB, INST_SINGLE, &(core_resource->total_db));
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_UC_DB, INST_SINGLE, &(core_resource->uc_db));
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_MC_DB, INST_SINGLE, &(core_resource->mc_db));

    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_ecgm_dbal_interface_resources_statistics_current_get(
    uint32 unit,
    uint32 core_id,
    uint32 is_mc,
    dbal_enum_value_field_ecgm_priority_e ecgm_priority,
    uint32 egq_if,
    uint32 *pd,
    uint32 *db)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_ECGM_INTERFACE_RESOURCES_STATISTICS_CURRENT, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_MC, is_mc);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ECGM_PRIORITY, ecgm_priority);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_EGQ_IF, egq_if);

    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_PD, INST_SINGLE, pd);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_DB, INST_SINGLE, db);

    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_ecgm_dbal_interface_resources_statistics_max_get(
    uint32 unit,
    uint32 core_id,
    uint32 is_mc,
    dbal_enum_value_field_ecgm_priority_e ecgm_priority,
    uint32 egq_if,
    uint32 *pd,
    uint32 *db)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_ECGM_INTERFACE_RESOURCES_STATISTICS_MAX, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_MC, is_mc);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ECGM_PRIORITY, ecgm_priority);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_EGQ_IF, egq_if);

    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_PD, INST_SINGLE, pd);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_DB, INST_SINGLE, db);

    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_ecgm_dbal_port_resources_statistics_current_get(
    uint32 unit,
    uint32 core_id,
    uint32 is_mc,
    uint32 otm_port,
    uint32 *pd,
    uint32 *db)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_ECGM_PORT_RESOURCES_STATISTICS_CURRENT, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_MC, is_mc);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OTM_PORT, otm_port);

    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_PD, INST_SINGLE, pd);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_DB, INST_SINGLE, db);

    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_ecgm_dbal_port_resources_statistics_max_get(
    uint32 unit,
    uint32 core_id,
    uint32 is_mc,
    uint32 otm_port,
    uint32 *pd,
    uint32 *db)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_ECGM_PORT_RESOURCES_STATISTICS_MAX, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_MC, is_mc);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OTM_PORT, otm_port);

    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_PD, INST_SINGLE, pd);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_DB, INST_SINGLE, db);

    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_ecgm_dbal_queue_resources_statistics_current_get(
    uint32 unit,
    uint32 core_id,
    uint32 is_mc,
    uint32 queue_pair,
    uint32 *pd,
    uint32 *db)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_ECGM_QUEUE_RESOURCES_STATISTICS_CURRENT, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_MC, is_mc);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_QPAIR, queue_pair);

    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_PD, INST_SINGLE, pd);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_DB, INST_SINGLE, db);

    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_ecgm_dbal_queue_resources_statistics_max_get(
    uint32 unit,
    uint32 core_id,
    uint32 is_mc,
    uint32 queue_pair,
    uint32 *pd,
    uint32 *db)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_ECGM_QUEUE_RESOURCES_STATISTICS_MAX, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_MC, is_mc);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_QPAIR, queue_pair);

    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_PD, INST_SINGLE, pd);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_DB, INST_SINGLE, db);

    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_ecgm_dbal_service_pool_resources_statistics_current_get(
    uint32 unit,
    uint32 core_id,
    uint32 sp_id,
    ecgm_service_pool_resources_statistics_t * sp_resource)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC
                    (unit, DBAL_TABLE_ECGM_SERVICE_POOL_RESOURCES_STATISTICS_CURRENT, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_SERVICE_POOL_ID, sp_id);

    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_PD, INST_SINGLE, &(sp_resource->pd));
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_DB, INST_SINGLE, &(sp_resource->db));
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_RESERVED_PD, INST_SINGLE, &(sp_resource->reserved_pd));
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_RESERVED_DB, INST_SINGLE, &(sp_resource->reserved_db));

    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_ecgm_dbal_resources_statistics_max_clear(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_table_clear(unit, DBAL_TABLE_ECGM_INTERFACE_RESOURCES_STATISTICS_MAX));
    SHR_IF_ERR_EXIT(dbal_table_clear(unit, DBAL_TABLE_ECGM_PORT_RESOURCES_STATISTICS_MAX));
    SHR_IF_ERR_EXIT(dbal_table_clear(unit, DBAL_TABLE_ECGM_QUEUE_RESOURCES_STATISTICS_MAX));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_ecgm_dbal_service_pool_resources_statistics_max_get(
    uint32 unit,
    uint32 core_id,
    uint32 sp_id,
    ecgm_service_pool_resources_statistics_t * sp_resource)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_ECGM_SERVICE_POOL_RESOURCES_STATISTICS_MAX, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_SERVICE_POOL_ID, sp_id);

    /*
     * only request 2 of the 4 members in sp_resource since max statistics are not relevant for reserved.
     */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_PD, INST_SINGLE, &(sp_resource->pd));
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_DB, INST_SINGLE, &(sp_resource->db));

    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_ecgm_dbal_service_pool_tc_resources_statistics_current_get(
    uint32 unit,
    uint32 core_id,
    uint32 sp_id,
    uint32 tc,
    uint32 *pd,
    uint32 *db)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC
                    (unit, DBAL_TABLE_ECGM_SERVICE_POOL_TC_RESOURCES_STATISTICS_CURRENT, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_SERVICE_POOL_ID, sp_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_TC, tc);

    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_PD, INST_SINGLE, pd);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_DB, INST_SINGLE, db);

    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_ecgm_dbal_service_pool_tc_resources_statistics_max_get(
    uint32 unit,
    uint32 core_id,
    uint32 sp_id,
    uint32 tc,
    uint32 *pd,
    uint32 *db)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC
                    (unit, DBAL_TABLE_ECGM_SERVICE_POOL_TC_RESOURCES_STATISTICS_MAX, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_SERVICE_POOL_ID, sp_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_TC, tc);

    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_PD, INST_SINGLE, pd);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_DB, INST_SINGLE, db);

    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_ecgm_dbal_disable_max_statistics_tracking_set(
    uint32 unit,
    uint32 core_id,
    uint32 is_tracking)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_ECGM_DISABLE_MAX_STATISTICS_TRACKING, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DISABLE_MAX_STAT_TRACKING, INST_SINGLE, is_tracking);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/** static functions */

static shr_error_e
delete_fifo_long_lcd_thresholds_set(
    uint32 unit,
    uint32 core_id,
    dnx_ecgm_delete_fifo_type_e type,
    const ecgm_delete_fifo_thresholds_t * thresholds)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_ECGM_DELETE_FIFO_THRESHOLDS, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FIFO_TYPE, type);

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FQP_HIGH_PRIORITY, INST_SINGLE,
                                 thresholds->fqp_high_priority);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_STOP_MC_LOW, INST_SINGLE, thresholds->stop_mc_low);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_STOP_MC_HIGH, INST_SINGLE, thresholds->stop_mc_high);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_STOP_ALL, INST_SINGLE, thresholds->stop_all);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

static shr_error_e
delete_fifo_long_lcd_thresholds_get(
    uint32 unit,
    uint32 core_id,
    dnx_ecgm_delete_fifo_type_e type,
    ecgm_delete_fifo_thresholds_t * thresholds)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_ECGM_DELETE_FIFO_THRESHOLDS, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FIFO_TYPE, type);

    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_FQP_HIGH_PRIORITY, INST_SINGLE,
                               &(thresholds->fqp_high_priority));
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_STOP_MC_LOW, INST_SINGLE, &(thresholds->stop_mc_low));
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_STOP_MC_HIGH, INST_SINGLE,
                               &(thresholds->stop_mc_high));
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_STOP_ALL, INST_SINGLE, &(thresholds->stop_all));
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

static shr_error_e
delete_fifo_tdm_short_thresholds_set(
    uint32 unit,
    uint32 core_id,
    const ecgm_delete_fifo_thresholds_t * thresholds)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_ECGM_DELETE_FIFO_TDM_SHORT_THRESHOLDS, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FQP_HIGH_PRIORITY, INST_SINGLE,
                                 thresholds->fqp_high_priority);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_STOP_MC_LOW, INST_SINGLE, thresholds->stop_mc_low);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_STOP_MC_HIGH, INST_SINGLE, thresholds->stop_mc_high);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_STOP_ALL, INST_SINGLE, thresholds->stop_all);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

static shr_error_e
delete_fifo_tdm_short_thresholds_get(
    uint32 unit,
    uint32 core_id,
    ecgm_delete_fifo_thresholds_t * thresholds)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_ECGM_DELETE_FIFO_TDM_SHORT_THRESHOLDS, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);

    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_FQP_HIGH_PRIORITY, INST_SINGLE,
                               &(thresholds->fqp_high_priority));
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_STOP_MC_LOW, INST_SINGLE, &(thresholds->stop_mc_low));
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_STOP_MC_HIGH, INST_SINGLE,
                               &(thresholds->stop_mc_high));
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_STOP_ALL, INST_SINGLE, &(thresholds->stop_all));
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

static shr_error_e
core_table_set_field(
    uint32 unit,
    uint32 core_id,
    dbal_tables_e table_name,
    dbal_fields_e field_name,
    uint32 set_value)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table_name, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);

    dbal_entry_value_field32_set(unit, entry_handle_id, field_name, INST_SINGLE, set_value);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

static shr_error_e
core_table_get_field(
    uint32 unit,
    uint32 core_id,
    dbal_tables_e table_name,
    dbal_fields_e field_name,
    uint32 *value)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table_name, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);

    dbal_value_field32_request(unit, entry_handle_id, field_name, INST_SINGLE, value);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_ecgm_dbal_internal_tm_port_to_port_profile_set(
    uint32 unit,
    uint32 core_id,
    uint32 tm_port,
    uint32 port_profile)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_ECGM_TM_PORT_TO_PORT_PROFILE_MAP, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_TM_PORT, tm_port);

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ECGM_PORT_PROFILE, INST_SINGLE, port_profile);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_ecgm_dbal_internal_base_q_pair_to_port_profile_set(
    uint32 unit,
    uint32 core_id,
    uint32 base_q_pair,
    uint32 port_profile)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_ECGM_BASE_Q_PAIR_TO_PORT_PROFILE_MAP, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_BASE_Q_PAIR, base_q_pair);

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ECGM_PORT_PROFILE, INST_SINGLE, port_profile);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_ecgm_dbal_alpha_msb_get(
    int unit,
    uint32 *alpha_msb)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * since all alphas are of the same size - getting 
     * the alpha size from a random table, 
     * regardless of the specific table to which the value is intended 
     */
    SHR_IF_ERR_EXIT(dbal_tables_field_size_get
                    (unit, DBAL_TABLE_ECGM_CORE_TC_MULTICAST_SP_THRESHOLDS, DBAL_FIELD_DB_DROP_ALPHA_0, FALSE, 0, 0,
                     (int *) alpha_msb));

    /*
     * subtracting 1 to get the index of msb 
     */
    *alpha_msb -= 1;

exit:
    SHR_FUNC_EXIT;
}
