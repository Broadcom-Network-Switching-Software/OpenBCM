

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_EGRESSDNX

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_tdm.h>



extern shr_error_e jr2_a0_data_tdm_attach(
    int unit);
extern shr_error_e j2c_a0_data_tdm_attach(
    int unit);
extern shr_error_e q2a_a0_data_tdm_attach(
    int unit);



static shr_error_e
dnx_data_tdm_params_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "params";
    submodule_data->doc = "default values for tdm module";
    
    submodule_data->nof_features = _dnx_data_tdm_params_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data tdm params features");

    submodule_data->features[dnx_data_tdm_params_is_supported].name = "is_supported";
    submodule_data->features[dnx_data_tdm_params_is_supported].doc = "Marks presence of TDM functionality";
    submodule_data->features[dnx_data_tdm_params_is_supported].flags |= DNXC_DATA_F_FEATURE;

    
    submodule_data->nof_defines = _dnx_data_tdm_params_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data tdm params defines");

    submodule_data->defines[dnx_data_tdm_params_define_context_map_base_size].name = "context_map_base_size";
    submodule_data->defines[dnx_data_tdm_params_define_context_map_base_size].doc = "Number of bits in binary representation of base context which is added to SID to                    get the 'global SID'. Log2 of 'context_map_base_nof'";
    
    submodule_data->defines[dnx_data_tdm_params_define_context_map_base_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_tdm_params_define_context_map_base_nof].name = "context_map_base_nof";
    submodule_data->defines[dnx_data_tdm_params_define_context_map_base_nof].doc = "Maximal Number of 'base context's";
    
    submodule_data->defines[dnx_data_tdm_params_define_context_map_base_nof].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_tdm_params_define_sid_min_size].name = "sid_min_size";
    submodule_data->defines[dnx_data_tdm_params_define_sid_min_size].doc = "Minimal number of bits in binary representation of Stream ID.";
    
    submodule_data->defines[dnx_data_tdm_params_define_sid_min_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_tdm_params_define_sid_size].name = "sid_size";
    submodule_data->defines[dnx_data_tdm_params_define_sid_size].doc = "Number of bits in binary representation of Stream ID. Log2 of 'nof_stream_ids'";
    
    submodule_data->defines[dnx_data_tdm_params_define_sid_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_tdm_params_define_nof_stream_ids].name = "nof_stream_ids";
    submodule_data->defines[dnx_data_tdm_params_define_nof_stream_ids].doc = "Maximal Number of Stream IDs. Note that we include here the stream IDs which are below the 'min'";
    
    submodule_data->defines[dnx_data_tdm_params_define_nof_stream_ids].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_tdm_params_define_global_sid_offset_size].name = "global_sid_offset_size";
    submodule_data->defines[dnx_data_tdm_params_define_global_sid_offset_size].doc = "Number of bits in binary representation of GLOBAL SID offset. Log2 of 'global_sid_offset_nof'";
    
    submodule_data->defines[dnx_data_tdm_params_define_global_sid_offset_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_tdm_params_define_global_sid_offset_nof].name = "global_sid_offset_nof";
    submodule_data->defines[dnx_data_tdm_params_define_global_sid_offset_nof].doc = "Number of GLOBAL SID offsets. Maximal value of GLOBAL SID offset plus 1";
    
    submodule_data->defines[dnx_data_tdm_params_define_global_sid_offset_nof].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_tdm_params_define_sid_offset_size].name = "sid_offset_size";
    submodule_data->defines[dnx_data_tdm_params_define_sid_offset_size].doc = "Number of bits in binary representation of SID offset, in bytes. Log2 of 'sid_offset_nof'";
    
    submodule_data->defines[dnx_data_tdm_params_define_sid_offset_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_tdm_params_define_sid_offset_nof].name = "sid_offset_nof";
    submodule_data->defines[dnx_data_tdm_params_define_sid_offset_nof].doc = "Number of SID offsets when counted in bytes. Maximal value of SID offset plus 1";
    
    submodule_data->defines[dnx_data_tdm_params_define_sid_offset_nof].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_tdm_params_define_sid_offset_nof_in_bits].name = "sid_offset_nof_in_bits";
    submodule_data->defines[dnx_data_tdm_params_define_sid_offset_nof_in_bits].doc = "Number of SID offsets when counted in bits. Maximal value of SID offset in bits plus 1";
    
    submodule_data->defines[dnx_data_tdm_params_define_sid_offset_nof_in_bits].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_tdm_params_define_context_size].name = "context_size";
    submodule_data->defines[dnx_data_tdm_params_define_context_size].doc = "Context ID size, in bits. Log2 of 'nof_contexts'";
    
    submodule_data->defines[dnx_data_tdm_params_define_context_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_tdm_params_define_nof_contexts].name = "nof_contexts";
    submodule_data->defines[dnx_data_tdm_params_define_nof_contexts].doc = "Maximal Number of Context IDs";
    
    submodule_data->defines[dnx_data_tdm_params_define_nof_contexts].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_tdm_params_define_pkt_size_size].name = "pkt_size_size";
    submodule_data->defines[dnx_data_tdm_params_define_pkt_size_size].doc = "Size of pkt_size field";
    
    submodule_data->defines[dnx_data_tdm_params_define_pkt_size_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_tdm_params_define_pkt_size_lower_limit].name = "pkt_size_lower_limit";
    submodule_data->defines[dnx_data_tdm_params_define_pkt_size_lower_limit].doc = "Minimum packet size for Bypass Mode";
    
    submodule_data->defines[dnx_data_tdm_params_define_pkt_size_lower_limit].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_tdm_params_define_pkt_size_upper_limit].name = "pkt_size_upper_limit";
    submodule_data->defines[dnx_data_tdm_params_define_pkt_size_upper_limit].doc = "Maximum packet size for Bypass Mode";
    
    submodule_data->defines[dnx_data_tdm_params_define_pkt_size_upper_limit].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_tdm_params_define_link_mask_size].name = "link_mask_size";
    submodule_data->defines[dnx_data_tdm_params_define_link_mask_size].doc = "Size direct link mask for bypass traffic";
    
    submodule_data->defines[dnx_data_tdm_params_define_link_mask_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_tdm_params_define_link_mask_nof].name = "link_mask_nof";
    submodule_data->defines[dnx_data_tdm_params_define_link_mask_nof].doc = "Number of direct link masks for bypass traffic";
    
    submodule_data->defines[dnx_data_tdm_params_define_link_mask_nof].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_tdm_params_define_tdm_tc].name = "tdm_tc";
    submodule_data->defines[dnx_data_tdm_params_define_tdm_tc].doc = "TDM traffic class";
    
    submodule_data->defines[dnx_data_tdm_params_define_tdm_tc].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_tdm_params_define_tdm_dp].name = "tdm_dp";
    submodule_data->defines[dnx_data_tdm_params_define_tdm_dp].doc = "TDM drop precedence";
    
    submodule_data->defines[dnx_data_tdm_params_define_tdm_dp].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_tdm_params_define_mode].name = "mode";
    submodule_data->defines[dnx_data_tdm_params_define_mode].doc = "TDM mode - defines header type. If NONE - tdm disabled";
    
    submodule_data->defines[dnx_data_tdm_params_define_mode].flags |= DNXC_DATA_F_NUMERIC;

    
    submodule_data->nof_tables = _dnx_data_tdm_params_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data tdm params tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_tdm_params_feature_get(
    int unit,
    dnx_data_tdm_params_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_tdm, dnx_data_tdm_submodule_params, feature);
}


uint32
dnx_data_tdm_params_context_map_base_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_tdm, dnx_data_tdm_submodule_params, dnx_data_tdm_params_define_context_map_base_size);
}

uint32
dnx_data_tdm_params_context_map_base_nof_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_tdm, dnx_data_tdm_submodule_params, dnx_data_tdm_params_define_context_map_base_nof);
}

uint32
dnx_data_tdm_params_sid_min_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_tdm, dnx_data_tdm_submodule_params, dnx_data_tdm_params_define_sid_min_size);
}

uint32
dnx_data_tdm_params_sid_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_tdm, dnx_data_tdm_submodule_params, dnx_data_tdm_params_define_sid_size);
}

uint32
dnx_data_tdm_params_nof_stream_ids_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_tdm, dnx_data_tdm_submodule_params, dnx_data_tdm_params_define_nof_stream_ids);
}

uint32
dnx_data_tdm_params_global_sid_offset_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_tdm, dnx_data_tdm_submodule_params, dnx_data_tdm_params_define_global_sid_offset_size);
}

uint32
dnx_data_tdm_params_global_sid_offset_nof_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_tdm, dnx_data_tdm_submodule_params, dnx_data_tdm_params_define_global_sid_offset_nof);
}

uint32
dnx_data_tdm_params_sid_offset_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_tdm, dnx_data_tdm_submodule_params, dnx_data_tdm_params_define_sid_offset_size);
}

uint32
dnx_data_tdm_params_sid_offset_nof_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_tdm, dnx_data_tdm_submodule_params, dnx_data_tdm_params_define_sid_offset_nof);
}

uint32
dnx_data_tdm_params_sid_offset_nof_in_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_tdm, dnx_data_tdm_submodule_params, dnx_data_tdm_params_define_sid_offset_nof_in_bits);
}

uint32
dnx_data_tdm_params_context_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_tdm, dnx_data_tdm_submodule_params, dnx_data_tdm_params_define_context_size);
}

uint32
dnx_data_tdm_params_nof_contexts_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_tdm, dnx_data_tdm_submodule_params, dnx_data_tdm_params_define_nof_contexts);
}

uint32
dnx_data_tdm_params_pkt_size_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_tdm, dnx_data_tdm_submodule_params, dnx_data_tdm_params_define_pkt_size_size);
}

uint32
dnx_data_tdm_params_pkt_size_lower_limit_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_tdm, dnx_data_tdm_submodule_params, dnx_data_tdm_params_define_pkt_size_lower_limit);
}

uint32
dnx_data_tdm_params_pkt_size_upper_limit_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_tdm, dnx_data_tdm_submodule_params, dnx_data_tdm_params_define_pkt_size_upper_limit);
}

uint32
dnx_data_tdm_params_link_mask_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_tdm, dnx_data_tdm_submodule_params, dnx_data_tdm_params_define_link_mask_size);
}

uint32
dnx_data_tdm_params_link_mask_nof_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_tdm, dnx_data_tdm_submodule_params, dnx_data_tdm_params_define_link_mask_nof);
}

uint32
dnx_data_tdm_params_tdm_tc_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_tdm, dnx_data_tdm_submodule_params, dnx_data_tdm_params_define_tdm_tc);
}

uint32
dnx_data_tdm_params_tdm_dp_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_tdm, dnx_data_tdm_submodule_params, dnx_data_tdm_params_define_tdm_dp);
}

uint32
dnx_data_tdm_params_mode_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_tdm, dnx_data_tdm_submodule_params, dnx_data_tdm_params_define_mode);
}







shr_error_e
dnx_data_tdm_init(
    int unit,
    dnxc_data_module_t *module_data)
{
    SHR_FUNC_INIT_VARS(unit);

    
    module_data->name = "tdm";
    module_data->nof_submodules = _dnx_data_tdm_submodule_nof;
    DNXC_DATA_ALLOC(module_data->submodules, dnxc_data_submodule_t, module_data->nof_submodules, "_dnxc_data tdm submodules");

    
    SHR_IF_ERR_EXIT(dnx_data_tdm_params_init(unit, &module_data->submodules[dnx_data_tdm_submodule_params]));
    
    if (dnxc_data_mgmt_is_jr2_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_tdm_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_jr2_b0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_tdm_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_jr2_b1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_tdm_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_j2c_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_tdm_attach(unit));
        SHR_IF_ERR_EXIT(j2c_a0_data_tdm_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_j2c_a1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_tdm_attach(unit));
        SHR_IF_ERR_EXIT(j2c_a0_data_tdm_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_q2a_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_tdm_attach(unit));
        SHR_IF_ERR_EXIT(q2a_a0_data_tdm_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_q2a_b0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_tdm_attach(unit));
        SHR_IF_ERR_EXIT(q2a_a0_data_tdm_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_q2a_b1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_tdm_attach(unit));
        SHR_IF_ERR_EXIT(q2a_a0_data_tdm_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_j2p_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_tdm_attach(unit));
    }
    else
    {
        SHR_EXIT();
    }

exit:
    SHR_FUNC_EXIT;
}
#undef BSL_LOG_MODULE

