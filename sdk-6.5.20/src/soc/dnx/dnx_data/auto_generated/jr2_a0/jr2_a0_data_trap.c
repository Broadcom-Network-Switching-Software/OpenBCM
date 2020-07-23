

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_RX

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_trap.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_property.h>
#include <bcm_int/dnx/rx/rx_trap.h>







static shr_error_e
jr2_a0_dnx_data_trap_ingress_ndp_trap_en_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_trap;
    int submodule_index = dnx_data_trap_submodule_ingress;
    int feature_index = dnx_data_trap_ingress_ndp_trap_en;
    SHR_FUNC_INIT_VARS(unit);

    feature = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].features[feature_index];
    
    feature->default_data = 0;

    
    feature->data = 0;

    
    feature->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_feature_value_set(unit, feature));

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_trap_ingress_trap_in_lif_profile_mapping_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_trap;
    int submodule_index = dnx_data_trap_submodule_ingress;
    int feature_index = dnx_data_trap_ingress_trap_in_lif_profile_mapping;
    SHR_FUNC_INIT_VARS(unit);

    feature = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].features[feature_index];
    
    feature->default_data = 0;

    
    feature->data = 0;

    
    feature->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_feature_value_set(unit, feature));

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_trap_ingress_svtag_trap_en_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_trap;
    int submodule_index = dnx_data_trap_submodule_ingress;
    int feature_index = dnx_data_trap_ingress_svtag_trap_en;
    SHR_FUNC_INIT_VARS(unit);

    feature = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].features[feature_index];
    
    feature->default_data = 0;

    
    feature->data = 0;

    
    feature->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_feature_value_set(unit, feature));

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_trap_ingress_oam_offset_actions_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_trap;
    int submodule_index = dnx_data_trap_submodule_ingress;
    int feature_index = dnx_data_trap_ingress_oam_offset_actions;
    SHR_FUNC_INIT_VARS(unit);

    feature = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].features[feature_index];
    
    feature->default_data = 0;

    
    feature->data = 0;

    
    feature->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_feature_value_set(unit, feature));

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_trap_ingress_snoop_code_clear_action_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_trap;
    int submodule_index = dnx_data_trap_submodule_ingress;
    int feature_index = dnx_data_trap_ingress_snoop_code_clear_action;
    SHR_FUNC_INIT_VARS(unit);

    feature = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].features[feature_index];
    
    feature->default_data = 0;

    
    feature->data = 0;

    
    feature->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_feature_value_set(unit, feature));

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_trap_ingress_mirror_code_clear_action_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_trap;
    int submodule_index = dnx_data_trap_submodule_ingress;
    int feature_index = dnx_data_trap_ingress_mirror_code_clear_action;
    SHR_FUNC_INIT_VARS(unit);

    feature = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].features[feature_index];
    
    feature->default_data = 0;

    
    feature->data = 0;

    
    feature->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_feature_value_set(unit, feature));

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_trap_ingress_stat_sampling_code_clear_action_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_trap;
    int submodule_index = dnx_data_trap_submodule_ingress;
    int feature_index = dnx_data_trap_ingress_stat_sampling_code_clear_action;
    SHR_FUNC_INIT_VARS(unit);

    feature = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].features[feature_index];
    
    feature->default_data = 0;

    
    feature->data = 0;

    
    feature->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_feature_value_set(unit, feature));

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_trap_ingress_ipv6_mapped_dest_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_trap;
    int submodule_index = dnx_data_trap_submodule_ingress;
    int feature_index = dnx_data_trap_ingress_ipv6_mapped_dest;
    SHR_FUNC_INIT_VARS(unit);

    feature = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].features[feature_index];
    
    feature->default_data = 0;

    
    feature->data = 0;

    
    feature->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_feature_value_set(unit, feature));

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_trap_ingress_mpls_termination_fail_over_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_trap;
    int submodule_index = dnx_data_trap_submodule_ingress;
    int feature_index = dnx_data_trap_ingress_mpls_termination_fail_over;
    SHR_FUNC_INIT_VARS(unit);

    feature = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].features[feature_index];
    
    feature->default_data = 0;

    
    feature->data = 0;

    
    feature->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_feature_value_set(unit, feature));

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_trap_ingress_checksum_feature_fixed_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_trap;
    int submodule_index = dnx_data_trap_submodule_ingress;
    int feature_index = dnx_data_trap_ingress_checksum_feature_fixed;
    SHR_FUNC_INIT_VARS(unit);

    feature = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].features[feature_index];
    
    feature->default_data = 0;

    
    feature->data = 0;

    
    feature->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_feature_value_set(unit, feature));

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_trap_ingress_header_size_err_blocking_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_trap;
    int submodule_index = dnx_data_trap_submodule_ingress;
    int feature_index = dnx_data_trap_ingress_header_size_err_blocking;
    SHR_FUNC_INIT_VARS(unit);

    feature = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].features[feature_index];
    
    feature->default_data = 0;

    
    feature->data = 0;

    
    feature->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_feature_value_set(unit, feature));

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_trap_ingress_l4_trap_res_before_l3_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_trap;
    int submodule_index = dnx_data_trap_submodule_ingress;
    int feature_index = dnx_data_trap_ingress_l4_trap_res_before_l3;
    SHR_FUNC_INIT_VARS(unit);

    feature = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].features[feature_index];
    
    feature->default_data = 0;

    
    feature->data = 0;

    
    feature->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_feature_value_set(unit, feature));

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_trap_ingress_comp_mc_before_routing_traps_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_trap;
    int submodule_index = dnx_data_trap_submodule_ingress;
    int feature_index = dnx_data_trap_ingress_comp_mc_before_routing_traps;
    SHR_FUNC_INIT_VARS(unit);

    feature = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].features[feature_index];
    
    feature->default_data = 0;

    
    feature->data = 0;

    
    feature->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_feature_value_set(unit, feature));

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_trap_ingress_soft_mem_err_over_dest_err_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_trap;
    int submodule_index = dnx_data_trap_submodule_ingress;
    int feature_index = dnx_data_trap_ingress_soft_mem_err_over_dest_err;
    SHR_FUNC_INIT_VARS(unit);

    feature = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].features[feature_index];
    
    feature->default_data = 0;

    
    feature->data = 0;

    
    feature->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_feature_value_set(unit, feature));

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_trap_ingress_eth_default_proc_fix_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_trap;
    int submodule_index = dnx_data_trap_submodule_ingress;
    int feature_index = dnx_data_trap_ingress_eth_default_proc_fix;
    SHR_FUNC_INIT_VARS(unit);

    feature = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].features[feature_index];
    
    feature->default_data = 0;

    
    feature->data = 0;

    
    feature->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_feature_value_set(unit, feature));

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_trap_ingress_fwd_domain_mode_trap_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_trap;
    int submodule_index = dnx_data_trap_submodule_ingress;
    int feature_index = dnx_data_trap_ingress_fwd_domain_mode_trap;
    SHR_FUNC_INIT_VARS(unit);

    feature = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].features[feature_index];
    
    feature->default_data = 0;

    
    feature->data = 0;

    
    feature->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_feature_value_set(unit, feature));

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_trap_ingress_sa_equals_zero_trap_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_trap;
    int submodule_index = dnx_data_trap_submodule_ingress;
    int feature_index = dnx_data_trap_ingress_sa_equals_zero_trap;
    SHR_FUNC_INIT_VARS(unit);

    feature = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].features[feature_index];
    
    feature->default_data = 0;

    
    feature->data = 0;

    
    feature->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_feature_value_set(unit, feature));

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_trap_ingress_latency_measurement_bug_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_trap;
    int submodule_index = dnx_data_trap_submodule_ingress;
    int feature_index = dnx_data_trap_ingress_latency_measurement_bug;
    SHR_FUNC_INIT_VARS(unit);

    feature = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].features[feature_index];
    
    feature->default_data = 0;

    
    feature->data = 0;

    
    feature->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_feature_value_set(unit, feature));

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_trap_ingress_ser_hw_support_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_trap;
    int submodule_index = dnx_data_trap_submodule_ingress;
    int feature_index = dnx_data_trap_ingress_ser_hw_support;
    SHR_FUNC_INIT_VARS(unit);

    feature = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].features[feature_index];
    
    feature->default_data = 1;

    
    feature->data = 1;

    
    feature->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_feature_value_set(unit, feature));

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_trap_ingress_compatible_mc_hw_support_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_trap;
    int submodule_index = dnx_data_trap_submodule_ingress;
    int feature_index = dnx_data_trap_ingress_compatible_mc_hw_support;
    SHR_FUNC_INIT_VARS(unit);

    feature = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].features[feature_index];
    
    feature->default_data = 1;

    
    feature->data = 1;

    
    feature->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_feature_value_set(unit, feature));

exit:
    SHR_FUNC_EXIT;
}



static shr_error_e
jr2_a0_dnx_data_trap_ingress_protocol_traps_init_mode_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_trap;
    int submodule_index = dnx_data_trap_submodule_ingress;
    int define_index = dnx_data_trap_ingress_define_protocol_traps_init_mode;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = DNX_RX_TRAP_PROTOCOL_MODE_IN_LIF;

    
    define->data = DNX_RX_TRAP_PROTOCOL_MODE_IN_LIF;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = spn_PROTOCOL_TRAPS_MODE;
    define->property.doc = 
        "\n"
        "Set the initialization property of the protocol trap indexing,\n"
        "they can be done per:\n"
        "IN_LIF - Trap profiling address is the IN-LIF\n"
        "IN_PORT - Trap profiling address is the IN-PORT\n"
        "\n"
    ;
    define->property.method = dnxc_data_property_method_direct_map;
    define->property.method_str = "direct_map";
    define->property.nof_mapping = 2;
    DNXC_DATA_ALLOC(define->property.mapping, dnxc_data_property_map_t, define->property.nof_mapping, "dnxc_data property mapping");

    define->property.mapping[0].name = "IN_LIF";
    define->property.mapping[0].val = DNX_RX_TRAP_PROTOCOL_MODE_IN_LIF;
    define->property.mapping[0].is_default = 1 ;
    define->property.mapping[1].name = "IN_PORT";
    define->property.mapping[1].val = DNX_RX_TRAP_PROTOCOL_MODE_IN_PORT;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));

    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &define->property, -1, &define->data));
    
    define->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_trap_ingress_protocol_traps_lif_profile_starting_offset_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_trap;
    int submodule_index = dnx_data_trap_submodule_ingress;
    int define_index = dnx_data_trap_ingress_define_protocol_traps_lif_profile_starting_offset;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0;

    
    define->data = 0;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_trap_ingress_nof_predefeind_traps_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_trap;
    int submodule_index = dnx_data_trap_submodule_ingress;
    int define_index = dnx_data_trap_ingress_define_nof_predefeind_traps;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 256;

    
    define->data = 256;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_trap_ingress_nof_user_defined_traps_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_trap;
    int submodule_index = dnx_data_trap_submodule_ingress;
    int define_index = dnx_data_trap_ingress_define_nof_user_defined_traps;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 224;

    
    define->data = 224;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_trap_ingress_nof_1588_traps_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_trap;
    int submodule_index = dnx_data_trap_submodule_ingress;
    int define_index = dnx_data_trap_ingress_define_nof_1588_traps;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 8;

    
    define->data = 8;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_trap_ingress_nof_entries_action_table_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_trap;
    int submodule_index = dnx_data_trap_submodule_ingress;
    int define_index = dnx_data_trap_ingress_define_nof_entries_action_table;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 512;

    
    define->data = 512;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_trap_ingress_nof_protocol_trap_profiles_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_trap;
    int submodule_index = dnx_data_trap_submodule_ingress;
    int define_index = dnx_data_trap_ingress_define_nof_protocol_trap_profiles;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 4;

    
    define->data = 4;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_trap_ingress_nof_recycle_cmds_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_trap;
    int submodule_index = dnx_data_trap_submodule_ingress;
    int define_index = dnx_data_trap_ingress_define_nof_recycle_cmds;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 16;

    
    define->data = 16;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_trap_ingress_nof_programmable_traps_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_trap;
    int submodule_index = dnx_data_trap_submodule_ingress;
    int define_index = dnx_data_trap_ingress_define_nof_programmable_traps;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 8;

    
    define->data = 8;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_trap_ingress_nof_lif_traps_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_trap;
    int submodule_index = dnx_data_trap_submodule_ingress;
    int define_index = dnx_data_trap_ingress_define_nof_lif_traps;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 8;

    
    define->data = 8;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_trap_ingress_nof_icmp_compressed_types_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_trap;
    int submodule_index = dnx_data_trap_submodule_ingress;
    int define_index = dnx_data_trap_ingress_define_nof_icmp_compressed_types;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 32;

    
    define->data = 32;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_trap_ingress_nof_stat_objs_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_trap;
    int submodule_index = dnx_data_trap_submodule_ingress;
    int define_index = dnx_data_trap_ingress_define_nof_stat_objs;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 10;

    
    define->data = 10;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_trap_ingress_max_nof_stat_objs_to_overwrite_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_trap;
    int submodule_index = dnx_data_trap_submodule_ingress;
    int define_index = dnx_data_trap_ingress_define_max_nof_stat_objs_to_overwrite;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 2;

    
    define->data = 2;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_trap_ingress_nof_error_code_profiles_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_trap;
    int submodule_index = dnx_data_trap_submodule_ingress;
    int define_index = dnx_data_trap_ingress_define_nof_error_code_profiles;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0;

    
    define->data = 0;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_trap_ingress_nof_sci_profiles_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_trap;
    int submodule_index = dnx_data_trap_submodule_ingress;
    int define_index = dnx_data_trap_ingress_define_nof_sci_profiles;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0;

    
    define->data = 0;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_trap_ingress_nof_error_codes_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_trap;
    int submodule_index = dnx_data_trap_submodule_ingress;
    int define_index = dnx_data_trap_ingress_define_nof_error_codes;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0;

    
    define->data = 0;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_trap_ingress_nof_scis_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_trap;
    int submodule_index = dnx_data_trap_submodule_ingress;
    int define_index = dnx_data_trap_ingress_define_nof_scis;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0;

    
    define->data = 0;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}






static shr_error_e
jr2_a0_dnx_data_trap_erpp_unknown_da_trap_en_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_trap;
    int submodule_index = dnx_data_trap_submodule_erpp;
    int feature_index = dnx_data_trap_erpp_unknown_da_trap_en;
    SHR_FUNC_INIT_VARS(unit);

    feature = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].features[feature_index];
    
    feature->default_data = 0;

    
    feature->data = 0;

    
    feature->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_feature_value_set(unit, feature));

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_trap_erpp_glem_not_found_trap_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_trap;
    int submodule_index = dnx_data_trap_submodule_erpp;
    int feature_index = dnx_data_trap_erpp_glem_not_found_trap;
    SHR_FUNC_INIT_VARS(unit);

    feature = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].features[feature_index];
    
    feature->default_data = 1;

    
    feature->data = 1;

    
    feature->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_feature_value_set(unit, feature));

exit:
    SHR_FUNC_EXIT;
}



static shr_error_e
jr2_a0_dnx_data_trap_erpp_nof_erpp_app_traps_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_trap;
    int submodule_index = dnx_data_trap_submodule_erpp;
    int define_index = dnx_data_trap_erpp_define_nof_erpp_app_traps;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 19;

    
    define->data = 19;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_trap_erpp_nof_erpp_forward_profiles_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_trap;
    int submodule_index = dnx_data_trap_submodule_erpp;
    int define_index = dnx_data_trap_erpp_define_nof_erpp_forward_profiles;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 8;

    
    define->data = 8;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_trap_erpp_nof_erpp_user_configurable_profiles_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_trap;
    int submodule_index = dnx_data_trap_submodule_erpp;
    int define_index = dnx_data_trap_erpp_define_nof_erpp_user_configurable_profiles;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 7;

    
    define->data = 7;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_trap_erpp_nof_erpp_snif_profiles_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_trap;
    int submodule_index = dnx_data_trap_submodule_erpp;
    int define_index = dnx_data_trap_erpp_define_nof_erpp_snif_profiles;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 4;

    
    define->data = 4;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}






static shr_error_e
jr2_a0_dnx_data_trap_etpp_glem_hw_support_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_trap;
    int submodule_index = dnx_data_trap_submodule_etpp;
    int feature_index = dnx_data_trap_etpp_glem_hw_support;
    SHR_FUNC_INIT_VARS(unit);

    feature = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].features[feature_index];
    
    feature->default_data = 1;

    
    feature->data = 1;

    
    feature->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_feature_value_set(unit, feature));

exit:
    SHR_FUNC_EXIT;
}



static shr_error_e
jr2_a0_dnx_data_trap_etpp_nof_etpp_app_traps_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_trap;
    int submodule_index = dnx_data_trap_submodule_etpp;
    int define_index = dnx_data_trap_etpp_define_nof_etpp_app_traps;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 11;

    
    define->data = 11;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_trap_etpp_nof_etpp_user_configurable_profiles_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_trap;
    int submodule_index = dnx_data_trap_submodule_etpp;
    int define_index = dnx_data_trap_etpp_define_nof_etpp_user_configurable_profiles;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 4;

    
    define->data = 4;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_trap_etpp_nof_etpp_trap_profiles_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_trap;
    int submodule_index = dnx_data_trap_submodule_etpp;
    int define_index = dnx_data_trap_etpp_define_nof_etpp_trap_profiles;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 7;

    
    define->data = 7;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_trap_etpp_nof_etpp_oam_traps_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_trap;
    int submodule_index = dnx_data_trap_submodule_etpp;
    int define_index = dnx_data_trap_etpp_define_nof_etpp_oam_traps;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 7;

    
    define->data = 7;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_trap_etpp_nof_etpp_snif_profiles_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_trap;
    int submodule_index = dnx_data_trap_submodule_etpp;
    int define_index = dnx_data_trap_etpp_define_nof_etpp_snif_profiles;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 9;

    
    define->data = 9;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_trap_etpp_nof_mtu_profiles_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_trap;
    int submodule_index = dnx_data_trap_submodule_etpp;
    int define_index = dnx_data_trap_etpp_define_nof_mtu_profiles;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 8;

    
    define->data = 8;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_trap_etpp_nof_etpp_lif_traps_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_trap;
    int submodule_index = dnx_data_trap_submodule_etpp;
    int define_index = dnx_data_trap_etpp_define_nof_etpp_lif_traps;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 4;

    
    define->data = 4;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_trap_etpp_max_mtu_value_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_trap;
    int submodule_index = dnx_data_trap_submodule_etpp;
    int define_index = dnx_data_trap_etpp_define_max_mtu_value;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 12287;

    
    define->data = 12287;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_trap_etpp_trap_context_port_profile_4b_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_trap;
    int submodule_index = dnx_data_trap_submodule_etpp;
    int define_index = dnx_data_trap_etpp_define_trap_context_port_profile_4b;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0;

    
    define->data = 0;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}







static shr_error_e
jr2_a0_dnx_data_trap_strength_max_strength_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_trap;
    int submodule_index = dnx_data_trap_submodule_strength;
    int define_index = dnx_data_trap_strength_define_max_strength;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 15;

    
    define->data = 15;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_trap_strength_max_snp_strength_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_trap;
    int submodule_index = dnx_data_trap_submodule_strength;
    int define_index = dnx_data_trap_strength_define_max_snp_strength;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 7;

    
    define->data = 7;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_trap_strength_max_ingress_to_egress_compressed_strength_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_trap;
    int submodule_index = dnx_data_trap_submodule_strength;
    int define_index = dnx_data_trap_strength_define_max_ingress_to_egress_compressed_strength;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 7;

    
    define->data = 7;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_trap_strength_default_trap_strength_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_trap;
    int submodule_index = dnx_data_trap_submodule_strength;
    int define_index = dnx_data_trap_strength_define_default_trap_strength;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 4;

    
    define->data = 4;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = spn_DEFAULT_TRAP_STRENGTH;
    define->property.doc = 
        "\n"
        "Default trap strength used when drop / trap packets to CPU.\n"
        "\n"
    ;
    define->property.method = dnxc_data_property_method_range;
    define->property.method_str = "range";
    define->property.range_min = 0;
    define->property.range_max = 15;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));

    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &define->property, -1, &define->data));
    
    define->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_trap_strength_default_snoop_strength_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_trap;
    int submodule_index = dnx_data_trap_submodule_strength;
    int define_index = dnx_data_trap_strength_define_default_snoop_strength;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 1;

    
    define->data = 1;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = spn_DEFAULT_SNOOP_STRENGTH;
    define->property.doc = 
        "\n"
        "Default snoop strength used when drop / trap packets to CPU.\n"
        "\n"
    ;
    define->property.method = dnxc_data_property_method_range;
    define->property.method_str = "range";
    define->property.range_min = 0;
    define->property.range_max = 7;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));

    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &define->property, -1, &define->data));
    
    define->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}







static shr_error_e
jr2_a0_dnx_data_trap_register_name_changes_out_lif_mcdb_ptr_signal_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_trap;
    int submodule_index = dnx_data_trap_submodule_register_name_changes;
    int define_index = dnx_data_trap_register_name_changes_define_out_lif_mcdb_ptr_signal;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0;

    
    define->data = 0;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_trap_register_name_changes_added_n_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_trap;
    int submodule_index = dnx_data_trap_submodule_register_name_changes;
    int define_index = dnx_data_trap_register_name_changes_define_added_n;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0;

    
    define->data = 0;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_trap_register_name_changes_per_pp_port_change_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_trap;
    int submodule_index = dnx_data_trap_submodule_register_name_changes;
    int define_index = dnx_data_trap_register_name_changes_define_per_pp_port_change;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0;

    
    define->data = 0;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_trap_register_name_changes_visibility_signal_0_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_trap;
    int submodule_index = dnx_data_trap_submodule_register_name_changes;
    int define_index = dnx_data_trap_register_name_changes_define_visibility_signal_0;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 1;

    
    define->data = 1;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}




shr_error_e
jr2_a0_data_trap_attach(
    int unit)
{
    dnxc_data_module_t *module = NULL;
    dnxc_data_submodule_t *submodule = NULL;
    dnxc_data_define_t *define = NULL;
    dnxc_data_feature_t *feature = NULL;
    dnxc_data_table_t *table = NULL;
    int module_index = dnx_data_module_trap;
    int submodule_index = -1, data_index = -1;
    SHR_FUNC_INIT_VARS(unit);

    COMPILER_REFERENCE(define);
    COMPILER_REFERENCE(feature);
    COMPILER_REFERENCE(table);
    COMPILER_REFERENCE(submodule);
    COMPILER_REFERENCE(data_index);
    COMPILER_REFERENCE(submodule_index);
    module = &_dnxc_data[unit].modules[module_index];
    
    submodule_index = dnx_data_trap_submodule_ingress;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_trap_ingress_define_protocol_traps_init_mode;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_trap_ingress_protocol_traps_init_mode_set;
    data_index = dnx_data_trap_ingress_define_protocol_traps_lif_profile_starting_offset;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_trap_ingress_protocol_traps_lif_profile_starting_offset_set;
    data_index = dnx_data_trap_ingress_define_nof_predefeind_traps;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_trap_ingress_nof_predefeind_traps_set;
    data_index = dnx_data_trap_ingress_define_nof_user_defined_traps;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_trap_ingress_nof_user_defined_traps_set;
    data_index = dnx_data_trap_ingress_define_nof_1588_traps;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_trap_ingress_nof_1588_traps_set;
    data_index = dnx_data_trap_ingress_define_nof_entries_action_table;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_trap_ingress_nof_entries_action_table_set;
    data_index = dnx_data_trap_ingress_define_nof_protocol_trap_profiles;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_trap_ingress_nof_protocol_trap_profiles_set;
    data_index = dnx_data_trap_ingress_define_nof_recycle_cmds;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_trap_ingress_nof_recycle_cmds_set;
    data_index = dnx_data_trap_ingress_define_nof_programmable_traps;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_trap_ingress_nof_programmable_traps_set;
    data_index = dnx_data_trap_ingress_define_nof_lif_traps;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_trap_ingress_nof_lif_traps_set;
    data_index = dnx_data_trap_ingress_define_nof_icmp_compressed_types;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_trap_ingress_nof_icmp_compressed_types_set;
    data_index = dnx_data_trap_ingress_define_nof_stat_objs;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_trap_ingress_nof_stat_objs_set;
    data_index = dnx_data_trap_ingress_define_max_nof_stat_objs_to_overwrite;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_trap_ingress_max_nof_stat_objs_to_overwrite_set;
    data_index = dnx_data_trap_ingress_define_nof_error_code_profiles;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_trap_ingress_nof_error_code_profiles_set;
    data_index = dnx_data_trap_ingress_define_nof_sci_profiles;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_trap_ingress_nof_sci_profiles_set;
    data_index = dnx_data_trap_ingress_define_nof_error_codes;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_trap_ingress_nof_error_codes_set;
    data_index = dnx_data_trap_ingress_define_nof_scis;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_trap_ingress_nof_scis_set;

    
    data_index = dnx_data_trap_ingress_ndp_trap_en;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_trap_ingress_ndp_trap_en_set;
    data_index = dnx_data_trap_ingress_trap_in_lif_profile_mapping;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_trap_ingress_trap_in_lif_profile_mapping_set;
    data_index = dnx_data_trap_ingress_svtag_trap_en;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_trap_ingress_svtag_trap_en_set;
    data_index = dnx_data_trap_ingress_oam_offset_actions;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_trap_ingress_oam_offset_actions_set;
    data_index = dnx_data_trap_ingress_snoop_code_clear_action;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_trap_ingress_snoop_code_clear_action_set;
    data_index = dnx_data_trap_ingress_mirror_code_clear_action;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_trap_ingress_mirror_code_clear_action_set;
    data_index = dnx_data_trap_ingress_stat_sampling_code_clear_action;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_trap_ingress_stat_sampling_code_clear_action_set;
    data_index = dnx_data_trap_ingress_ipv6_mapped_dest;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_trap_ingress_ipv6_mapped_dest_set;
    data_index = dnx_data_trap_ingress_mpls_termination_fail_over;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_trap_ingress_mpls_termination_fail_over_set;
    data_index = dnx_data_trap_ingress_checksum_feature_fixed;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_trap_ingress_checksum_feature_fixed_set;
    data_index = dnx_data_trap_ingress_header_size_err_blocking;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_trap_ingress_header_size_err_blocking_set;
    data_index = dnx_data_trap_ingress_l4_trap_res_before_l3;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_trap_ingress_l4_trap_res_before_l3_set;
    data_index = dnx_data_trap_ingress_comp_mc_before_routing_traps;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_trap_ingress_comp_mc_before_routing_traps_set;
    data_index = dnx_data_trap_ingress_soft_mem_err_over_dest_err;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_trap_ingress_soft_mem_err_over_dest_err_set;
    data_index = dnx_data_trap_ingress_eth_default_proc_fix;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_trap_ingress_eth_default_proc_fix_set;
    data_index = dnx_data_trap_ingress_fwd_domain_mode_trap;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_trap_ingress_fwd_domain_mode_trap_set;
    data_index = dnx_data_trap_ingress_sa_equals_zero_trap;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_trap_ingress_sa_equals_zero_trap_set;
    data_index = dnx_data_trap_ingress_latency_measurement_bug;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_trap_ingress_latency_measurement_bug_set;
    data_index = dnx_data_trap_ingress_ser_hw_support;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_trap_ingress_ser_hw_support_set;
    data_index = dnx_data_trap_ingress_compatible_mc_hw_support;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_trap_ingress_compatible_mc_hw_support_set;

    
    
    submodule_index = dnx_data_trap_submodule_erpp;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_trap_erpp_define_nof_erpp_app_traps;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_trap_erpp_nof_erpp_app_traps_set;
    data_index = dnx_data_trap_erpp_define_nof_erpp_forward_profiles;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_trap_erpp_nof_erpp_forward_profiles_set;
    data_index = dnx_data_trap_erpp_define_nof_erpp_user_configurable_profiles;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_trap_erpp_nof_erpp_user_configurable_profiles_set;
    data_index = dnx_data_trap_erpp_define_nof_erpp_snif_profiles;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_trap_erpp_nof_erpp_snif_profiles_set;

    
    data_index = dnx_data_trap_erpp_unknown_da_trap_en;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_trap_erpp_unknown_da_trap_en_set;
    data_index = dnx_data_trap_erpp_glem_not_found_trap;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_trap_erpp_glem_not_found_trap_set;

    
    
    submodule_index = dnx_data_trap_submodule_etpp;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_trap_etpp_define_nof_etpp_app_traps;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_trap_etpp_nof_etpp_app_traps_set;
    data_index = dnx_data_trap_etpp_define_nof_etpp_user_configurable_profiles;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_trap_etpp_nof_etpp_user_configurable_profiles_set;
    data_index = dnx_data_trap_etpp_define_nof_etpp_trap_profiles;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_trap_etpp_nof_etpp_trap_profiles_set;
    data_index = dnx_data_trap_etpp_define_nof_etpp_oam_traps;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_trap_etpp_nof_etpp_oam_traps_set;
    data_index = dnx_data_trap_etpp_define_nof_etpp_snif_profiles;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_trap_etpp_nof_etpp_snif_profiles_set;
    data_index = dnx_data_trap_etpp_define_nof_mtu_profiles;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_trap_etpp_nof_mtu_profiles_set;
    data_index = dnx_data_trap_etpp_define_nof_etpp_lif_traps;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_trap_etpp_nof_etpp_lif_traps_set;
    data_index = dnx_data_trap_etpp_define_max_mtu_value;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_trap_etpp_max_mtu_value_set;
    data_index = dnx_data_trap_etpp_define_trap_context_port_profile_4b;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_trap_etpp_trap_context_port_profile_4b_set;

    
    data_index = dnx_data_trap_etpp_glem_hw_support;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_trap_etpp_glem_hw_support_set;

    
    
    submodule_index = dnx_data_trap_submodule_strength;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_trap_strength_define_max_strength;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_trap_strength_max_strength_set;
    data_index = dnx_data_trap_strength_define_max_snp_strength;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_trap_strength_max_snp_strength_set;
    data_index = dnx_data_trap_strength_define_max_ingress_to_egress_compressed_strength;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_trap_strength_max_ingress_to_egress_compressed_strength_set;
    data_index = dnx_data_trap_strength_define_default_trap_strength;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_trap_strength_default_trap_strength_set;
    data_index = dnx_data_trap_strength_define_default_snoop_strength;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_trap_strength_default_snoop_strength_set;

    

    
    
    submodule_index = dnx_data_trap_submodule_register_name_changes;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_trap_register_name_changes_define_out_lif_mcdb_ptr_signal;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_trap_register_name_changes_out_lif_mcdb_ptr_signal_set;
    data_index = dnx_data_trap_register_name_changes_define_added_n;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_trap_register_name_changes_added_n_set;
    data_index = dnx_data_trap_register_name_changes_define_per_pp_port_change;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_trap_register_name_changes_per_pp_port_change_set;
    data_index = dnx_data_trap_register_name_changes_define_visibility_signal_0;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_trap_register_name_changes_visibility_signal_0_set;

    

    

    SHR_FUNC_EXIT;
}
#undef BSL_LOG_MODULE

