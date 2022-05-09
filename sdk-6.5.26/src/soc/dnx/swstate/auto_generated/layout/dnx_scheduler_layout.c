
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SWSTATEDNX_GENERAL

#include <soc/dnx/swstate/auto_generated/types/dnx_scheduler_types.h>
#include <soc/dnx/swstate/auto_generated/layout/dnx_scheduler_layout.h>

dnxc_sw_state_layout_t layout_array_dnx_scheduler[DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_SCHEDULER_DB_NOF_PARAMS)];

shr_error_e
dnx_scheduler_init_layout_structure(int unit)
{

    int dnx_scheduler_db__fmq__fmq_flow_id__default_val = -1;
    sch_virtual_flows_sw_state_type_e dnx_scheduler_db__virtual_flows__is_enabled__default_val = SCH_VIRTUAL_FLOWS_UNINITALIZED;


    int idx;

    DNXC_SW_STATE_INIT_FUNC_DEFS;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_SCHEDULER_DB);
    layout_array_dnx_scheduler[idx].name = "dnx_scheduler_db";
    layout_array_dnx_scheduler[idx].type = "dnx_scheduler_db_t";
    layout_array_dnx_scheduler[idx].doc = "DB for use by Scheduler";
    layout_array_dnx_scheduler[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_scheduler[idx].size_of = sizeof(dnx_scheduler_db_t);
    layout_array_dnx_scheduler[idx].parent  = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_scheduler[idx].first_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_SCHEDULER_DB__FIRST);
    layout_array_dnx_scheduler[idx].last_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_SCHEDULER_DB__LAST);

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_SCHEDULER_DB__PORT_SHAPER);
    layout_array_dnx_scheduler[idx].name = "dnx_scheduler_db__port_shaper";
    layout_array_dnx_scheduler[idx].type = "dnx_scheduler_db_shaper_t**";
    layout_array_dnx_scheduler[idx].doc = "Port shaper per base HR";
    layout_array_dnx_scheduler[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_scheduler[idx].size_of = sizeof(dnx_scheduler_db_shaper_t**);
    layout_array_dnx_scheduler[idx].array_indexes[0].num_elements = dnx_data_device.general.nof_cores_get(unit);
    layout_array_dnx_scheduler[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_DYNAMIC_DNX_DATA;
    layout_array_dnx_scheduler[idx].array_indexes[1].num_elements = dnx_data_sch.flow.nof_hr_get(unit);
    layout_array_dnx_scheduler[idx].array_indexes[1].index_type = DNXC_SWSTATE_ARRAY_INDEX_DYNAMIC_DNX_DATA;
    layout_array_dnx_scheduler[idx].parent  = 0;
    layout_array_dnx_scheduler[idx].first_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_SCHEDULER_DB__PORT_SHAPER__FIRST);
    layout_array_dnx_scheduler[idx].last_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_SCHEDULER_DB__PORT_SHAPER__LAST);

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_SCHEDULER_DB__HR);
    layout_array_dnx_scheduler[idx].name = "dnx_scheduler_db__hr";
    layout_array_dnx_scheduler[idx].type = "dnx_scheduler_db_hr_info_t**";
    layout_array_dnx_scheduler[idx].doc = "Info per HR";
    layout_array_dnx_scheduler[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_scheduler[idx].size_of = sizeof(dnx_scheduler_db_hr_info_t**);
    layout_array_dnx_scheduler[idx].array_indexes[0].num_elements = dnx_data_device.general.nof_cores_get(unit);
    layout_array_dnx_scheduler[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_DYNAMIC_DNX_DATA;
    layout_array_dnx_scheduler[idx].array_indexes[1].num_elements = dnx_data_sch.flow.nof_hr_get(unit);
    layout_array_dnx_scheduler[idx].array_indexes[1].index_type = DNXC_SWSTATE_ARRAY_INDEX_DYNAMIC_DNX_DATA;
    layout_array_dnx_scheduler[idx].parent  = 0;
    layout_array_dnx_scheduler[idx].first_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_SCHEDULER_DB__HR__FIRST);
    layout_array_dnx_scheduler[idx].last_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_SCHEDULER_DB__HR__LAST);

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_SCHEDULER_DB__INTERFACE);
    layout_array_dnx_scheduler[idx].name = "dnx_scheduler_db__interface";
    layout_array_dnx_scheduler[idx].type = "dnx_scheduler_db_interface_info_t*";
    layout_array_dnx_scheduler[idx].doc = "Info per sch interface";
    layout_array_dnx_scheduler[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_scheduler[idx].size_of = sizeof(dnx_scheduler_db_interface_info_t*);
    layout_array_dnx_scheduler[idx].array_indexes[0].num_elements = dnx_data_device.general.nof_cores_get(unit);
    layout_array_dnx_scheduler[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_DYNAMIC_DNX_DATA;
    layout_array_dnx_scheduler[idx].parent  = 0;
    layout_array_dnx_scheduler[idx].first_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_SCHEDULER_DB__INTERFACE__FIRST);
    layout_array_dnx_scheduler[idx].last_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_SCHEDULER_DB__INTERFACE__LAST);

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_SCHEDULER_DB__FMQ);
    layout_array_dnx_scheduler[idx].name = "dnx_scheduler_db__fmq";
    layout_array_dnx_scheduler[idx].type = "dnx_scheduler_db_fmq_info*";
    layout_array_dnx_scheduler[idx].doc = "Info per FMQ";
    layout_array_dnx_scheduler[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_scheduler[idx].size_of = sizeof(dnx_scheduler_db_fmq_info*);
    layout_array_dnx_scheduler[idx].array_indexes[0].num_elements = dnx_data_device.general.nof_cores_get(unit);
    layout_array_dnx_scheduler[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_DYNAMIC_DNX_DATA;
    layout_array_dnx_scheduler[idx].parent  = 0;
    layout_array_dnx_scheduler[idx].first_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_SCHEDULER_DB__FMQ__FIRST);
    layout_array_dnx_scheduler[idx].last_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_SCHEDULER_DB__FMQ__LAST);

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_SCHEDULER_DB__VIRTUAL_FLOWS);
    layout_array_dnx_scheduler[idx].name = "dnx_scheduler_db__virtual_flows";
    layout_array_dnx_scheduler[idx].type = "dnx_scheduler_db_virtual_flows_info_t";
    layout_array_dnx_scheduler[idx].doc = "Info about virtual flow";
    layout_array_dnx_scheduler[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_scheduler[idx].size_of = sizeof(dnx_scheduler_db_virtual_flows_info_t);
    layout_array_dnx_scheduler[idx].parent  = 0;
    layout_array_dnx_scheduler[idx].first_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_SCHEDULER_DB__VIRTUAL_FLOWS__FIRST);
    layout_array_dnx_scheduler[idx].last_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_SCHEDULER_DB__VIRTUAL_FLOWS__LAST);

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_SCHEDULER_DB__GENERAL);
    layout_array_dnx_scheduler[idx].name = "dnx_scheduler_db__general";
    layout_array_dnx_scheduler[idx].type = "dnx_scheduler_db_general_info*";
    layout_array_dnx_scheduler[idx].doc = "General info";
    layout_array_dnx_scheduler[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_scheduler[idx].size_of = sizeof(dnx_scheduler_db_general_info*);
    layout_array_dnx_scheduler[idx].array_indexes[0].num_elements = dnx_data_device.general.nof_cores_get(unit);
    layout_array_dnx_scheduler[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_DYNAMIC_DNX_DATA;
    layout_array_dnx_scheduler[idx].parent  = 0;
    layout_array_dnx_scheduler[idx].first_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_SCHEDULER_DB__GENERAL__FIRST);
    layout_array_dnx_scheduler[idx].last_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_SCHEDULER_DB__GENERAL__LAST);

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_SCHEDULER_DB__PORT_SHAPER__RATE);
    layout_array_dnx_scheduler[idx].name = "dnx_scheduler_db__port_shaper__rate";
    layout_array_dnx_scheduler[idx].type = "int";
    layout_array_dnx_scheduler[idx].doc = "rate value";
    layout_array_dnx_scheduler[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_scheduler[idx].size_of = sizeof(int);
    layout_array_dnx_scheduler[idx].parent  = 1;
    layout_array_dnx_scheduler[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_scheduler[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_SCHEDULER_DB__PORT_SHAPER__VALID);
    layout_array_dnx_scheduler[idx].name = "dnx_scheduler_db__port_shaper__valid";
    layout_array_dnx_scheduler[idx].type = "int";
    layout_array_dnx_scheduler[idx].doc = "Boolean, is valid value";
    layout_array_dnx_scheduler[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_scheduler[idx].size_of = sizeof(int);
    layout_array_dnx_scheduler[idx].parent  = 1;
    layout_array_dnx_scheduler[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_scheduler[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_SCHEDULER_DB__HR__IS_COLORED);
    layout_array_dnx_scheduler[idx].name = "dnx_scheduler_db__hr__is_colored";
    layout_array_dnx_scheduler[idx].type = "int";
    layout_array_dnx_scheduler[idx].doc = "is HR colored";
    layout_array_dnx_scheduler[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_scheduler[idx].size_of = sizeof(int);
    layout_array_dnx_scheduler[idx].parent  = 2;
    layout_array_dnx_scheduler[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_scheduler[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_SCHEDULER_DB__INTERFACE__MODIFIED);
    layout_array_dnx_scheduler[idx].name = "dnx_scheduler_db__interface__modified";
    layout_array_dnx_scheduler[idx].type = "SHR_BITDCL*";
    layout_array_dnx_scheduler[idx].doc = "is interface modified and required calendar recalculation";
    layout_array_dnx_scheduler[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_scheduler[idx].size_of = sizeof(SHR_BITDCL*);
    layout_array_dnx_scheduler[idx].array_indexes[0].num_elements = dnx_data_sch.interface.nof_sch_interfaces_get(unit);
    layout_array_dnx_scheduler[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_BITMAP;
    layout_array_dnx_scheduler[idx].parent  = 3;
    layout_array_dnx_scheduler[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_scheduler[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_SCHEDULER_DB__FMQ__RESERVED_HR_FLOW_ID);
    layout_array_dnx_scheduler[idx].name = "dnx_scheduler_db__fmq__reserved_hr_flow_id";
    layout_array_dnx_scheduler[idx].type = "int*";
    layout_array_dnx_scheduler[idx].doc = "User reserved HR flow ids";
    layout_array_dnx_scheduler[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_scheduler[idx].size_of = sizeof(int*);
    layout_array_dnx_scheduler[idx].array_indexes[0].num_elements = dnx_data_sch.general.nof_fmq_class_get(unit);
    layout_array_dnx_scheduler[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_DYNAMIC_DNX_DATA;
    layout_array_dnx_scheduler[idx].parent  = 4;
    layout_array_dnx_scheduler[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_scheduler[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_SCHEDULER_DB__FMQ__NOF_RESERVED_HR);
    layout_array_dnx_scheduler[idx].name = "dnx_scheduler_db__fmq__nof_reserved_hr";
    layout_array_dnx_scheduler[idx].type = "int";
    layout_array_dnx_scheduler[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_scheduler[idx].size_of = sizeof(int);
    layout_array_dnx_scheduler[idx].parent  = 4;
    layout_array_dnx_scheduler[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_scheduler[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_scheduler[idx].labels = DNXC_SW_STATE_LAYOUT_LABEL_COUNTER;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_SCHEDULER_DB__FMQ__FMQ_FLOW_ID);
    layout_array_dnx_scheduler[idx].name = "dnx_scheduler_db__fmq__fmq_flow_id";
    layout_array_dnx_scheduler[idx].type = "int*";
    layout_array_dnx_scheduler[idx].doc = "Allocated FMQ flow IDs";
    layout_array_dnx_scheduler[idx].default_value= &(dnx_scheduler_db__fmq__fmq_flow_id__default_val);
    layout_array_dnx_scheduler[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_scheduler[idx].size_of = sizeof(int*);
    layout_array_dnx_scheduler[idx].array_indexes[0].num_elements = dnx_data_sch.general.nof_fmq_class_get(unit);
    layout_array_dnx_scheduler[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_DYNAMIC_DNX_DATA;
    layout_array_dnx_scheduler[idx].parent  = 4;
    layout_array_dnx_scheduler[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_scheduler[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_SCHEDULER_DB__VIRTUAL_FLOWS__IS_ENABLED);
    layout_array_dnx_scheduler[idx].name = "dnx_scheduler_db__virtual_flows__is_enabled";
    layout_array_dnx_scheduler[idx].type = "sch_virtual_flows_sw_state_type_e";
    layout_array_dnx_scheduler[idx].doc = "Boolean, is virtual flows enabled";
    layout_array_dnx_scheduler[idx].default_value= &(dnx_scheduler_db__virtual_flows__is_enabled__default_val);
    layout_array_dnx_scheduler[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_scheduler[idx].size_of = sizeof(sch_virtual_flows_sw_state_type_e);
    layout_array_dnx_scheduler[idx].parent  = 5;
    layout_array_dnx_scheduler[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_scheduler[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_SCHEDULER_DB__VIRTUAL_FLOWS__IS_FLOW_VIRTUAL);
    layout_array_dnx_scheduler[idx].name = "dnx_scheduler_db__virtual_flows__is_flow_virtual";
    layout_array_dnx_scheduler[idx].type = "SHR_BITDCL**";
    layout_array_dnx_scheduler[idx].doc = "Bitmap used to indicate that the flow is virtual";
    layout_array_dnx_scheduler[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_scheduler[idx].size_of = sizeof(SHR_BITDCL**);
    layout_array_dnx_scheduler[idx].array_indexes[0].num_elements = dnx_data_device.general.nof_cores_get(unit);
    layout_array_dnx_scheduler[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_DYNAMIC_DNX_DATA;
    layout_array_dnx_scheduler[idx].array_indexes[1].num_elements = dnx_data_sch.flow.nof_flows_get(unit)/dnx_data_sch.flow.min_connector_bundle_size_get(unit);
    layout_array_dnx_scheduler[idx].array_indexes[1].index_type = DNXC_SWSTATE_ARRAY_INDEX_BITMAP;
    layout_array_dnx_scheduler[idx].parent  = 5;
    layout_array_dnx_scheduler[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_scheduler[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_scheduler[idx].labels = DNXC_SW_STATE_LAYOUT_LABEL_ACCESS_PTR;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_SCHEDULER_DB__GENERAL__RESERVED_ERP_HR_FLOW_ID);
    layout_array_dnx_scheduler[idx].name = "dnx_scheduler_db__general__reserved_erp_hr_flow_id";
    layout_array_dnx_scheduler[idx].type = "int";
    layout_array_dnx_scheduler[idx].doc = "unused reserved ERP HR flow ids";
    layout_array_dnx_scheduler[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_scheduler[idx].size_of = sizeof(int);
    layout_array_dnx_scheduler[idx].array_indexes[0].num_elements = 8;
    layout_array_dnx_scheduler[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_STATIC;
    layout_array_dnx_scheduler[idx].parent  = 6;
    layout_array_dnx_scheduler[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_scheduler[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_SCHEDULER_DB__GENERAL__NOF_RESERVED_ERP_HR);
    layout_array_dnx_scheduler[idx].name = "dnx_scheduler_db__general__nof_reserved_erp_hr";
    layout_array_dnx_scheduler[idx].type = "int";
    layout_array_dnx_scheduler[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_scheduler[idx].size_of = sizeof(int);
    layout_array_dnx_scheduler[idx].parent  = 6;
    layout_array_dnx_scheduler[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_scheduler[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_scheduler[idx].labels = DNXC_SW_STATE_LAYOUT_LABEL_COUNTER;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_SCHEDULER_DB__GENERAL__NON_DEFAULT_FLOW_ORDER);
    layout_array_dnx_scheduler[idx].name = "dnx_scheduler_db__general__non_default_flow_order";
    layout_array_dnx_scheduler[idx].type = "uint8";
    layout_array_dnx_scheduler[idx].doc = "[Boolean] indicate if core has non-default flow quartet order configuration";
    layout_array_dnx_scheduler[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_scheduler[idx].size_of = sizeof(uint8);
    layout_array_dnx_scheduler[idx].parent  = 6;
    layout_array_dnx_scheduler[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_scheduler[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_SCHEDULER_DB__GENERAL__IS_COMPOSITE_FQ_SUPPORTED);
    layout_array_dnx_scheduler[idx].name = "dnx_scheduler_db__general__is_composite_fq_supported";
    layout_array_dnx_scheduler[idx].type = "uint8";
    layout_array_dnx_scheduler[idx].doc = "[Boolean] indicate if composite FQ scheduling elements are supported per core";
    layout_array_dnx_scheduler[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_scheduler[idx].size_of = sizeof(uint8);
    layout_array_dnx_scheduler[idx].parent  = 6;
    layout_array_dnx_scheduler[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_scheduler[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;


    dnxc_sw_state_layout_init_structure(unit, DNX_SW_STATE_DNX_SCHEDULER_DB, layout_array_dnx_scheduler, sw_state_layout_array[unit][DNX_SCHEDULER_MODULE_ID], DNX_SW_STATE_DNX_SCHEDULER_DB_NOF_PARAMS);

    DNXC_SW_STATE_FUNC_RETURN;
}

#undef BSL_LOG_MODULE
