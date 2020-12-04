

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_COSQ

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_ipq.h>



extern shr_error_e jr2_a0_data_ipq_attach(
    int unit);
extern shr_error_e j2c_a0_data_ipq_attach(
    int unit);
extern shr_error_e q2a_a0_data_ipq_attach(
    int unit);
extern shr_error_e j2p_a0_data_ipq_attach(
    int unit);



static shr_error_e
dnx_data_ipq_queues_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "queues";
    submodule_data->doc = "queues data";
    
    submodule_data->nof_features = _dnx_data_ipq_queues_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data ipq queues features");

    submodule_data->features[dnx_data_ipq_queues_voq_resolution_per_dp].name = "voq_resolution_per_dp";
    submodule_data->features[dnx_data_ipq_queues_voq_resolution_per_dp].doc = "support mapping TC to VOQ offset per DP";
    submodule_data->features[dnx_data_ipq_queues_voq_resolution_per_dp].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_ipq_queues_tc_profile_from_tm_command].name = "tc_profile_from_tm_command";
    submodule_data->features[dnx_data_ipq_queues_tc_profile_from_tm_command].doc = "support getting TC profile from TM command (when mapping packet TC to VOQ offset)";
    submodule_data->features[dnx_data_ipq_queues_tc_profile_from_tm_command].flags |= DNXC_DATA_F_FEATURE;

    
    submodule_data->nof_defines = _dnx_data_ipq_queues_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data ipq queues defines");

    submodule_data->defines[dnx_data_ipq_queues_define_nof_queues].name = "nof_queues";
    submodule_data->defines[dnx_data_ipq_queues_define_nof_queues].doc = "Number of queues per core.";
    
    submodule_data->defines[dnx_data_ipq_queues_define_nof_queues].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_ipq_queues_define_min_bundle_size].name = "min_bundle_size";
    submodule_data->defines[dnx_data_ipq_queues_define_min_bundle_size].doc = "Minimum number of queues in a queue bundle.";
    
    submodule_data->defines[dnx_data_ipq_queues_define_min_bundle_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_ipq_queues_define_fmq_bundle_size].name = "fmq_bundle_size";
    submodule_data->defines[dnx_data_ipq_queues_define_fmq_bundle_size].doc = "Bundle size of the single FMQ when working with FMQ simple scheduling mode.";
    
    submodule_data->defines[dnx_data_ipq_queues_define_fmq_bundle_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_ipq_queues_define_max_fmq_id].name = "max_fmq_id";
    submodule_data->defines[dnx_data_ipq_queues_define_max_fmq_id].doc = "Maximal fabric multicast queue id.";
    
    submodule_data->defines[dnx_data_ipq_queues_define_max_fmq_id].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_ipq_queues_define_queue_id_bits].name = "queue_id_bits";
    submodule_data->defines[dnx_data_ipq_queues_define_queue_id_bits].doc = "Number of bits in queue id";
    
    submodule_data->defines[dnx_data_ipq_queues_define_queue_id_bits].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_ipq_queues_define_voq_offset_bits].name = "voq_offset_bits";
    submodule_data->defines[dnx_data_ipq_queues_define_voq_offset_bits].doc = "Number of bits in VOQ offset- used when mapping packet TC (and possible other parameters) to VOQ offset";
    
    submodule_data->defines[dnx_data_ipq_queues_define_voq_offset_bits].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_ipq_queues_define_min_bundle_size_bits].name = "min_bundle_size_bits";
    submodule_data->defines[dnx_data_ipq_queues_define_min_bundle_size_bits].doc = "number of bits in min_bundle_size";
    
    submodule_data->defines[dnx_data_ipq_queues_define_min_bundle_size_bits].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_ipq_queues_define_tc_profile_from_tm_command_nof_bits].name = "tc_profile_from_tm_command_nof_bits";
    submodule_data->defines[dnx_data_ipq_queues_define_tc_profile_from_tm_command_nof_bits].doc = "number of bits in tc_profile_from_tm_command";
    
    submodule_data->defines[dnx_data_ipq_queues_define_tc_profile_from_tm_command_nof_bits].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_ipq_queues_define_flow_quartet_nof_bits].name = "flow_quartet_nof_bits";
    submodule_data->defines[dnx_data_ipq_queues_define_flow_quartet_nof_bits].doc = "number of bits of flow quartet";
    
    submodule_data->defines[dnx_data_ipq_queues_define_flow_quartet_nof_bits].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_ipq_queues_define_invalid_queue].name = "invalid_queue";
    submodule_data->defines[dnx_data_ipq_queues_define_invalid_queue].doc = "invalid queue id";
    
    submodule_data->defines[dnx_data_ipq_queues_define_invalid_queue].flags |= DNXC_DATA_F_NUMERIC;

    
    submodule_data->nof_tables = _dnx_data_ipq_queues_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data ipq queues tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_ipq_queues_feature_get(
    int unit,
    dnx_data_ipq_queues_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_ipq, dnx_data_ipq_submodule_queues, feature);
}


uint32
dnx_data_ipq_queues_nof_queues_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_ipq, dnx_data_ipq_submodule_queues, dnx_data_ipq_queues_define_nof_queues);
}

uint32
dnx_data_ipq_queues_min_bundle_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_ipq, dnx_data_ipq_submodule_queues, dnx_data_ipq_queues_define_min_bundle_size);
}

uint32
dnx_data_ipq_queues_fmq_bundle_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_ipq, dnx_data_ipq_submodule_queues, dnx_data_ipq_queues_define_fmq_bundle_size);
}

uint32
dnx_data_ipq_queues_max_fmq_id_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_ipq, dnx_data_ipq_submodule_queues, dnx_data_ipq_queues_define_max_fmq_id);
}

uint32
dnx_data_ipq_queues_queue_id_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_ipq, dnx_data_ipq_submodule_queues, dnx_data_ipq_queues_define_queue_id_bits);
}

uint32
dnx_data_ipq_queues_voq_offset_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_ipq, dnx_data_ipq_submodule_queues, dnx_data_ipq_queues_define_voq_offset_bits);
}

uint32
dnx_data_ipq_queues_min_bundle_size_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_ipq, dnx_data_ipq_submodule_queues, dnx_data_ipq_queues_define_min_bundle_size_bits);
}

uint32
dnx_data_ipq_queues_tc_profile_from_tm_command_nof_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_ipq, dnx_data_ipq_submodule_queues, dnx_data_ipq_queues_define_tc_profile_from_tm_command_nof_bits);
}

uint32
dnx_data_ipq_queues_flow_quartet_nof_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_ipq, dnx_data_ipq_submodule_queues, dnx_data_ipq_queues_define_flow_quartet_nof_bits);
}

uint32
dnx_data_ipq_queues_invalid_queue_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_ipq, dnx_data_ipq_submodule_queues, dnx_data_ipq_queues_define_invalid_queue);
}










static shr_error_e
dnx_data_ipq_regions_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "regions";
    submodule_data->doc = "regions data";
    
    submodule_data->nof_features = _dnx_data_ipq_regions_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data ipq regions features");

    
    submodule_data->nof_defines = _dnx_data_ipq_regions_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data ipq regions defines");

    submodule_data->defines[dnx_data_ipq_regions_define_nof_queues].name = "nof_queues";
    submodule_data->defines[dnx_data_ipq_regions_define_nof_queues].doc = "Number of queues in a region.";
    
    submodule_data->defines[dnx_data_ipq_regions_define_nof_queues].flags |= DNXC_DATA_F_DEFINE;

    
    submodule_data->nof_tables = _dnx_data_ipq_regions_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data ipq regions tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_ipq_regions_feature_get(
    int unit,
    dnx_data_ipq_regions_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_ipq, dnx_data_ipq_submodule_regions, feature);
}


uint32
dnx_data_ipq_regions_nof_queues_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_ipq, dnx_data_ipq_submodule_regions, dnx_data_ipq_regions_define_nof_queues);
}










static shr_error_e
dnx_data_ipq_tc_map_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "tc_map";
    submodule_data->doc = "TC map info";
    
    submodule_data->nof_features = _dnx_data_ipq_tc_map_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data ipq tc_map features");

    
    submodule_data->nof_defines = _dnx_data_ipq_tc_map_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data ipq tc_map defines");

    submodule_data->defines[dnx_data_ipq_tc_map_define_nof_voq_flow_profiles].name = "nof_voq_flow_profiles";
    submodule_data->defines[dnx_data_ipq_tc_map_define_nof_voq_flow_profiles].doc = "Number of TC mapping profiles for flow based forwarding (used in template manager).";
    
    submodule_data->defines[dnx_data_ipq_tc_map_define_nof_voq_flow_profiles].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_ipq_tc_map_define_voq_flow_profiles_bits].name = "voq_flow_profiles_bits";
    submodule_data->defines[dnx_data_ipq_tc_map_define_voq_flow_profiles_bits].doc = "Number of bits to express TC mapping profiles for flow based forwarding.";
    
    submodule_data->defines[dnx_data_ipq_tc_map_define_voq_flow_profiles_bits].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_ipq_tc_map_define_default_voq_flow_profile_idx].name = "default_voq_flow_profile_idx";
    submodule_data->defines[dnx_data_ipq_tc_map_define_default_voq_flow_profile_idx].doc = "Default TC mapping profile index for flow based forwarding.";
    
    submodule_data->defines[dnx_data_ipq_tc_map_define_default_voq_flow_profile_idx].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_ipq_tc_map_define_nof_sysport_profiles].name = "nof_sysport_profiles";
    submodule_data->defines[dnx_data_ipq_tc_map_define_nof_sysport_profiles].doc = "Number of TC mapping profiles for sysport forwarding (used in template manager).";
    
    submodule_data->defines[dnx_data_ipq_tc_map_define_nof_sysport_profiles].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_ipq_tc_map_define_default_sysport_profile_idx].name = "default_sysport_profile_idx";
    submodule_data->defines[dnx_data_ipq_tc_map_define_default_sysport_profile_idx].doc = "Default TC mapping profile index for sysport forwarding.";
    
    submodule_data->defines[dnx_data_ipq_tc_map_define_default_sysport_profile_idx].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_ipq_tc_map_define_voq_resolution_type_max_value].name = "voq_resolution_type_max_value";
    submodule_data->defines[dnx_data_ipq_tc_map_define_voq_resolution_type_max_value].doc = "Maximal value of VOQ_RESOLUTION_TYPE_ENCODING dbal field";
    
    submodule_data->defines[dnx_data_ipq_tc_map_define_voq_resolution_type_max_value].flags |= DNXC_DATA_F_DEFINE;

    
    submodule_data->nof_tables = _dnx_data_ipq_tc_map_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data ipq tc_map tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_ipq_tc_map_feature_get(
    int unit,
    dnx_data_ipq_tc_map_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_ipq, dnx_data_ipq_submodule_tc_map, feature);
}


uint32
dnx_data_ipq_tc_map_nof_voq_flow_profiles_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_ipq, dnx_data_ipq_submodule_tc_map, dnx_data_ipq_tc_map_define_nof_voq_flow_profiles);
}

uint32
dnx_data_ipq_tc_map_voq_flow_profiles_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_ipq, dnx_data_ipq_submodule_tc_map, dnx_data_ipq_tc_map_define_voq_flow_profiles_bits);
}

uint32
dnx_data_ipq_tc_map_default_voq_flow_profile_idx_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_ipq, dnx_data_ipq_submodule_tc_map, dnx_data_ipq_tc_map_define_default_voq_flow_profile_idx);
}

uint32
dnx_data_ipq_tc_map_nof_sysport_profiles_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_ipq, dnx_data_ipq_submodule_tc_map, dnx_data_ipq_tc_map_define_nof_sysport_profiles);
}

uint32
dnx_data_ipq_tc_map_default_sysport_profile_idx_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_ipq, dnx_data_ipq_submodule_tc_map, dnx_data_ipq_tc_map_define_default_sysport_profile_idx);
}

uint32
dnx_data_ipq_tc_map_voq_resolution_type_max_value_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_ipq, dnx_data_ipq_submodule_tc_map, dnx_data_ipq_tc_map_define_voq_resolution_type_max_value);
}







shr_error_e
dnx_data_ipq_init(
    int unit,
    dnxc_data_module_t *module_data)
{
    SHR_FUNC_INIT_VARS(unit);

    
    module_data->name = "ipq";
    module_data->nof_submodules = _dnx_data_ipq_submodule_nof;
    DNXC_DATA_ALLOC(module_data->submodules, dnxc_data_submodule_t, module_data->nof_submodules, "_dnxc_data ipq submodules");

    
    SHR_IF_ERR_EXIT(dnx_data_ipq_queues_init(unit, &module_data->submodules[dnx_data_ipq_submodule_queues]));
    SHR_IF_ERR_EXIT(dnx_data_ipq_regions_init(unit, &module_data->submodules[dnx_data_ipq_submodule_regions]));
    SHR_IF_ERR_EXIT(dnx_data_ipq_tc_map_init(unit, &module_data->submodules[dnx_data_ipq_submodule_tc_map]));
    
    if (dnxc_data_mgmt_is_jr2_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_ipq_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_jr2_b0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_ipq_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_jr2_b1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_ipq_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_j2c_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_ipq_attach(unit));
        SHR_IF_ERR_EXIT(j2c_a0_data_ipq_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_j2c_a1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_ipq_attach(unit));
        SHR_IF_ERR_EXIT(j2c_a0_data_ipq_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_q2a_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_ipq_attach(unit));
        SHR_IF_ERR_EXIT(q2a_a0_data_ipq_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_q2a_b0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_ipq_attach(unit));
        SHR_IF_ERR_EXIT(q2a_a0_data_ipq_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_q2a_b1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_ipq_attach(unit));
        SHR_IF_ERR_EXIT(q2a_a0_data_ipq_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_j2p_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_ipq_attach(unit));
        SHR_IF_ERR_EXIT(j2p_a0_data_ipq_attach(unit));
    }
    else
    {
        SHR_EXIT();
    }

exit:
    SHR_FUNC_EXIT;
}
#undef BSL_LOG_MODULE

