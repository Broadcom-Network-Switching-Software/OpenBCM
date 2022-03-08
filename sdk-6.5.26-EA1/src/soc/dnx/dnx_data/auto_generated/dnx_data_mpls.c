
/* *INDENT-OFF* */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_MPLS

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_mpls.h>



#ifdef BCM_DNX2_SUPPORT

extern shr_error_e jr2_a0_data_mpls_attach(
    int unit);

#endif 
#ifdef BCM_DNX2_SUPPORT

extern shr_error_e jr2_b0_data_mpls_attach(
    int unit);

#endif 
#ifdef BCM_DNX2_SUPPORT

extern shr_error_e j2c_a0_data_mpls_attach(
    int unit);

#endif 
#ifdef BCM_DNX2_SUPPORT

extern shr_error_e q2a_a0_data_mpls_attach(
    int unit);

#endif 
#ifdef BCM_DNX2_SUPPORT

extern shr_error_e q2a_b0_data_mpls_attach(
    int unit);

#endif 
#ifdef BCM_DNX2_SUPPORT

extern shr_error_e j2p_a0_data_mpls_attach(
    int unit);

#endif 
#ifdef BCM_DNX2_SUPPORT

extern shr_error_e j2x_a0_data_mpls_attach(
    int unit);

#endif 



static shr_error_e
dnx_data_mpls_general_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "general";
    submodule_data->doc = "general mpls constants";
    
    submodule_data->nof_features = _dnx_data_mpls_general_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data mpls general features");

    submodule_data->features[dnx_data_mpls_general_mpls_term_1_or_2_labels].name = "mpls_term_1_or_2_labels";
    submodule_data->features[dnx_data_mpls_general_mpls_term_1_or_2_labels].doc = "If current label is not BOS, terminate it and the subsequent one else terminate it only in case of label-to-terminate is 3.";
    submodule_data->features[dnx_data_mpls_general_mpls_term_1_or_2_labels].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_mpls_general_mpls_term_till_bos].name = "mpls_term_till_bos";
    submodule_data->features[dnx_data_mpls_general_mpls_term_till_bos].doc = "Terminate the MPLS labels till BOS. Note the maximum number of labels to terminate by the feature is 3.";
    submodule_data->features[dnx_data_mpls_general_mpls_term_till_bos].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_mpls_general_mpls_ioam_d_flag_stamping].name = "mpls_ioam_d_flag_stamping";
    submodule_data->features[dnx_data_mpls_general_mpls_ioam_d_flag_stamping].doc = "IOAM stamping feature support";
    submodule_data->features[dnx_data_mpls_general_mpls_ioam_d_flag_stamping].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_mpls_general_stack_termination_indication].name = "stack_termination_indication";
    submodule_data->features[dnx_data_mpls_general_stack_termination_indication].doc = "mpls stack termination indication";
    submodule_data->features[dnx_data_mpls_general_stack_termination_indication].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_mpls_general_support_alternate_marking].name = "support_alternate_marking";
    submodule_data->features[dnx_data_mpls_general_support_alternate_marking].doc = "indicates if alternate marking is supported";
    submodule_data->features[dnx_data_mpls_general_support_alternate_marking].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_mpls_general_fhei_init_v1].name = "fhei_init_v1";
    submodule_data->features[dnx_data_mpls_general_fhei_init_v1].doc = "indicates if FHEI init V1 is supported";
    submodule_data->features[dnx_data_mpls_general_fhei_init_v1].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_mpls_general_per_port_ilm_lookup_supported].name = "per_port_ilm_lookup_supported";
    submodule_data->features[dnx_data_mpls_general_per_port_ilm_lookup_supported].doc = "indicates if the per port ILM lookup is supported";
    submodule_data->features[dnx_data_mpls_general_per_port_ilm_lookup_supported].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_mpls_general_enc2_implicit_null_w_ahp_supported].name = "enc2_implicit_null_w_ahp_supported";
    submodule_data->features[dnx_data_mpls_general_enc2_implicit_null_w_ahp_supported].doc = "indicates if AHP is supported for implicit_null entry im enc2 stage";
    submodule_data->features[dnx_data_mpls_general_enc2_implicit_null_w_ahp_supported].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_mpls_general_configurable_fhei_supported].name = "configurable_fhei_supported";
    submodule_data->features[dnx_data_mpls_general_configurable_fhei_supported].doc = "defines whether FHEI to EES translation is configurable";
    submodule_data->features[dnx_data_mpls_general_configurable_fhei_supported].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_mpls_general_mpls_raw_vtt_handling_supported].name = "mpls_raw_vtt_handling_supported";
    submodule_data->features[dnx_data_mpls_general_mpls_raw_vtt_handling_supported].doc = "defines whether handling MPLS_RAW in VTT is supported";
    submodule_data->features[dnx_data_mpls_general_mpls_raw_vtt_handling_supported].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_mpls_general_mpls_terminator_force_commit].name = "mpls_terminator_force_commit";
    submodule_data->features[dnx_data_mpls_general_mpls_terminator_force_commit].doc = "Indicates if force commit in mpls port/evpn terminator is supported";
    submodule_data->features[dnx_data_mpls_general_mpls_terminator_force_commit].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_mpls_general_per_intf_lookup_supported].name = "per_intf_lookup_supported";
    submodule_data->features[dnx_data_mpls_general_per_intf_lookup_supported].doc = "Indicates if MPLS label with previous interface lookup is supported";
    submodule_data->features[dnx_data_mpls_general_per_intf_lookup_supported].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_mpls_general_coupling_label_lookup_supported].name = "coupling_label_lookup_supported";
    submodule_data->features[dnx_data_mpls_general_coupling_label_lookup_supported].doc = "Indicates if 2 MPLS labels lookup is supported";
    submodule_data->features[dnx_data_mpls_general_coupling_label_lookup_supported].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_mpls_general_tandem_supported].name = "tandem_supported";
    submodule_data->features[dnx_data_mpls_general_tandem_supported].doc = "Indicates if same MPLS entry can be handled as TOS and TOS+1 in encap stage";
    submodule_data->features[dnx_data_mpls_general_tandem_supported].flags |= DNXC_DATA_F_FEATURE;

    
    submodule_data->nof_defines = _dnx_data_mpls_general_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data mpls general defines");

    submodule_data->defines[dnx_data_mpls_general_define_nof_mpls_termination_profiles].name = "nof_mpls_termination_profiles";
    submodule_data->defines[dnx_data_mpls_general_define_nof_mpls_termination_profiles].doc = "Number of MPLS Termination Profiles";
    
    submodule_data->defines[dnx_data_mpls_general_define_nof_mpls_termination_profiles].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_mpls_general_define_nof_mpls_push_profiles].name = "nof_mpls_push_profiles";
    submodule_data->defines[dnx_data_mpls_general_define_nof_mpls_push_profiles].doc = "JR mode Number of MPLS Push Profiles";
    
    submodule_data->defines[dnx_data_mpls_general_define_nof_mpls_push_profiles].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_mpls_general_define_nof_bits_mpls_label].name = "nof_bits_mpls_label";
    submodule_data->defines[dnx_data_mpls_general_define_nof_bits_mpls_label].doc = "Specifies number of bits used to represent an MPLS label";
    
    submodule_data->defines[dnx_data_mpls_general_define_nof_bits_mpls_label].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_mpls_general_define_log_nof_mpls_range_elements].name = "log_nof_mpls_range_elements";
    submodule_data->defines[dnx_data_mpls_general_define_log_nof_mpls_range_elements].doc = "Specifies number of bits used to represent number of available elements on any of mpls-range tables";
    
    submodule_data->defines[dnx_data_mpls_general_define_log_nof_mpls_range_elements].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_mpls_general_define_nof_mpls_range_elements].name = "nof_mpls_range_elements";
    submodule_data->defines[dnx_data_mpls_general_define_nof_mpls_range_elements].doc = "Specifies number of available elements on any of mpls-range tables";
    
    submodule_data->defines[dnx_data_mpls_general_define_nof_mpls_range_elements].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_mpls_general_define_mpls_special_label_encap_handling].name = "mpls_special_label_encap_handling";
    submodule_data->defines[dnx_data_mpls_general_define_mpls_special_label_encap_handling].doc = "Indicates whether special labels should be handled separatly";
    
    submodule_data->defines[dnx_data_mpls_general_define_mpls_special_label_encap_handling].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_mpls_general_define_mpls_control_word_supported].name = "mpls_control_word_supported";
    submodule_data->defines[dnx_data_mpls_general_define_mpls_control_word_supported].doc = "Indicates whether control word non-zero values supported.";
    
    submodule_data->defines[dnx_data_mpls_general_define_mpls_control_word_supported].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_mpls_general_define_mpls_second_stage_parser_handling].name = "mpls_second_stage_parser_handling";
    submodule_data->defines[dnx_data_mpls_general_define_mpls_second_stage_parser_handling].doc = "Indicates whether 2nd stage parser supported properly.";
    
    submodule_data->defines[dnx_data_mpls_general_define_mpls_second_stage_parser_handling].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_mpls_general_define_mpls_control_word_flag_bits_lsb].name = "mpls_control_word_flag_bits_lsb";
    submodule_data->defines[dnx_data_mpls_general_define_mpls_control_word_flag_bits_lsb].doc = "Indicates the control word flag bits location.";
    
    submodule_data->defines[dnx_data_mpls_general_define_mpls_control_word_flag_bits_lsb].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_mpls_general_define_fhei_mps_type_nof_bits].name = "fhei_mps_type_nof_bits";
    submodule_data->defines[dnx_data_mpls_general_define_fhei_mps_type_nof_bits].doc = "Indicates the number of mpls fhei type bits.";
    
    submodule_data->defines[dnx_data_mpls_general_define_fhei_mps_type_nof_bits].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_mpls_general_define_mpls_speculative_learning_handling].name = "mpls_speculative_learning_handling";
    submodule_data->defines[dnx_data_mpls_general_define_mpls_speculative_learning_handling].doc = "Indicates whether learning informatino from PWE is handled correctly,  in respect to speculative parsing.";
    
    submodule_data->defines[dnx_data_mpls_general_define_mpls_speculative_learning_handling].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_mpls_general_define_gach_with_flow_label_supported].name = "gach_with_flow_label_supported";
    submodule_data->defines[dnx_data_mpls_general_define_gach_with_flow_label_supported].doc = "Indicates whether G-ACH with Flow label simultaneously supported.";
    
    submodule_data->defines[dnx_data_mpls_general_define_gach_with_flow_label_supported].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_mpls_general_define_max_nof_mpls_term_stages].name = "max_nof_mpls_term_stages";
    submodule_data->defines[dnx_data_mpls_general_define_max_nof_mpls_term_stages].doc = "Maximal number of possible stages for MPLS termination";
    
    submodule_data->defines[dnx_data_mpls_general_define_max_nof_mpls_term_stages].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_mpls_general_define_nof_php_gport_lif].name = "nof_php_gport_lif";
    submodule_data->defines[dnx_data_mpls_general_define_nof_php_gport_lif].doc = "Indicates the LIF number used for PHP, JR1 mode only.";
    
    submodule_data->defines[dnx_data_mpls_general_define_nof_php_gport_lif].flags |= DNXC_DATA_F_NUMERIC;

    
    submodule_data->nof_tables = _dnx_data_mpls_general_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data mpls general tables");

    
    submodule_data->tables[dnx_data_mpls_general_table_fwd1_trap_destination].name = "fwd1_trap_destination";
    submodule_data->tables[dnx_data_mpls_general_table_fwd1_trap_destination].doc = "";
    submodule_data->tables[dnx_data_mpls_general_table_fwd1_trap_destination].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_mpls_general_table_fwd1_trap_destination].size_of_values = sizeof(dnx_data_mpls_general_fwd1_trap_destination_t);
    submodule_data->tables[dnx_data_mpls_general_table_fwd1_trap_destination].entry_get = dnx_data_mpls_general_fwd1_trap_destination_entry_str_get;

    
    submodule_data->tables[dnx_data_mpls_general_table_fwd1_trap_destination].nof_keys = 1;
    submodule_data->tables[dnx_data_mpls_general_table_fwd1_trap_destination].keys[0].name = "index";
    submodule_data->tables[dnx_data_mpls_general_table_fwd1_trap_destination].keys[0].doc = "";

    
    submodule_data->tables[dnx_data_mpls_general_table_fwd1_trap_destination].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_mpls_general_table_fwd1_trap_destination].values, dnxc_data_value_t, submodule_data->tables[dnx_data_mpls_general_table_fwd1_trap_destination].nof_values, "_dnx_data_mpls_general_table_fwd1_trap_destination table values");
    submodule_data->tables[dnx_data_mpls_general_table_fwd1_trap_destination].values[0].name = "context";
    submodule_data->tables[dnx_data_mpls_general_table_fwd1_trap_destination].values[0].type = "dbal_fields_e";
    submodule_data->tables[dnx_data_mpls_general_table_fwd1_trap_destination].values[0].doc = "";
    submodule_data->tables[dnx_data_mpls_general_table_fwd1_trap_destination].values[0].offset = UTILEX_OFFSETOF(dnx_data_mpls_general_fwd1_trap_destination_t, context);


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_mpls_general_feature_get(
    int unit,
    dnx_data_mpls_general_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_mpls, dnx_data_mpls_submodule_general, feature);
}


uint32
dnx_data_mpls_general_nof_mpls_termination_profiles_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mpls, dnx_data_mpls_submodule_general, dnx_data_mpls_general_define_nof_mpls_termination_profiles);
}

uint32
dnx_data_mpls_general_nof_mpls_push_profiles_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mpls, dnx_data_mpls_submodule_general, dnx_data_mpls_general_define_nof_mpls_push_profiles);
}

uint32
dnx_data_mpls_general_nof_bits_mpls_label_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mpls, dnx_data_mpls_submodule_general, dnx_data_mpls_general_define_nof_bits_mpls_label);
}

uint32
dnx_data_mpls_general_log_nof_mpls_range_elements_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mpls, dnx_data_mpls_submodule_general, dnx_data_mpls_general_define_log_nof_mpls_range_elements);
}

uint32
dnx_data_mpls_general_nof_mpls_range_elements_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mpls, dnx_data_mpls_submodule_general, dnx_data_mpls_general_define_nof_mpls_range_elements);
}

uint32
dnx_data_mpls_general_mpls_special_label_encap_handling_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mpls, dnx_data_mpls_submodule_general, dnx_data_mpls_general_define_mpls_special_label_encap_handling);
}

uint32
dnx_data_mpls_general_mpls_control_word_supported_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mpls, dnx_data_mpls_submodule_general, dnx_data_mpls_general_define_mpls_control_word_supported);
}

uint32
dnx_data_mpls_general_mpls_second_stage_parser_handling_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mpls, dnx_data_mpls_submodule_general, dnx_data_mpls_general_define_mpls_second_stage_parser_handling);
}

uint32
dnx_data_mpls_general_mpls_control_word_flag_bits_lsb_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mpls, dnx_data_mpls_submodule_general, dnx_data_mpls_general_define_mpls_control_word_flag_bits_lsb);
}

uint32
dnx_data_mpls_general_fhei_mps_type_nof_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mpls, dnx_data_mpls_submodule_general, dnx_data_mpls_general_define_fhei_mps_type_nof_bits);
}

uint32
dnx_data_mpls_general_mpls_speculative_learning_handling_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mpls, dnx_data_mpls_submodule_general, dnx_data_mpls_general_define_mpls_speculative_learning_handling);
}

uint32
dnx_data_mpls_general_gach_with_flow_label_supported_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mpls, dnx_data_mpls_submodule_general, dnx_data_mpls_general_define_gach_with_flow_label_supported);
}

uint32
dnx_data_mpls_general_max_nof_mpls_term_stages_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mpls, dnx_data_mpls_submodule_general, dnx_data_mpls_general_define_max_nof_mpls_term_stages);
}

uint32
dnx_data_mpls_general_nof_php_gport_lif_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mpls, dnx_data_mpls_submodule_general, dnx_data_mpls_general_define_nof_php_gport_lif);
}



const dnx_data_mpls_general_fwd1_trap_destination_t *
dnx_data_mpls_general_fwd1_trap_destination_get(
    int unit,
    int index)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_mpls, dnx_data_mpls_submodule_general, dnx_data_mpls_general_table_fwd1_trap_destination);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, index, 0);
    return (const dnx_data_mpls_general_fwd1_trap_destination_t *) data;

}


shr_error_e
dnx_data_mpls_general_fwd1_trap_destination_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_mpls_general_fwd1_trap_destination_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_mpls, dnx_data_mpls_submodule_general, dnx_data_mpls_general_table_fwd1_trap_destination);
    data = (const dnx_data_mpls_general_fwd1_trap_destination_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->context);
            break;
    }

    SHR_FUNC_EXIT;
}


const dnxc_data_table_info_t *
dnx_data_mpls_general_fwd1_trap_destination_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_mpls, dnx_data_mpls_submodule_general, dnx_data_mpls_general_table_fwd1_trap_destination);

}



shr_error_e
dnx_data_mpls_init(
    int unit,
    dnxc_data_module_t *module_data)
{
    SHR_FUNC_INIT_VARS(unit);

    
    module_data->name = "mpls";
    module_data->nof_submodules = _dnx_data_mpls_submodule_nof;
    DNXC_DATA_ALLOC(module_data->submodules, dnxc_data_submodule_t, module_data->nof_submodules, "_dnxc_data mpls submodules");

    
    SHR_IF_ERR_EXIT(dnx_data_mpls_general_init(unit, &module_data->submodules[dnx_data_mpls_submodule_general]));
    
#ifdef BCM_DNX2_SUPPORT

    if (dnxc_data_mgmt_is_jr2_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_mpls_attach(unit));
    }
    else

#endif 
#ifdef BCM_DNX2_SUPPORT

    if (dnxc_data_mgmt_is_jr2_b0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_mpls_attach(unit));
        SHR_IF_ERR_EXIT(jr2_b0_data_mpls_attach(unit));
    }
    else

#endif 
#ifdef BCM_DNX2_SUPPORT

    if (dnxc_data_mgmt_is_jr2_b1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_mpls_attach(unit));
        SHR_IF_ERR_EXIT(jr2_b0_data_mpls_attach(unit));
    }
    else

#endif 
#ifdef BCM_DNX2_SUPPORT

    if (dnxc_data_mgmt_is_j2c_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_mpls_attach(unit));
        SHR_IF_ERR_EXIT(j2c_a0_data_mpls_attach(unit));
    }
    else

#endif 
#ifdef BCM_DNX2_SUPPORT

    if (dnxc_data_mgmt_is_j2c_a1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_mpls_attach(unit));
        SHR_IF_ERR_EXIT(j2c_a0_data_mpls_attach(unit));
    }
    else

#endif 
#ifdef BCM_DNX2_SUPPORT

    if (dnxc_data_mgmt_is_q2a_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_mpls_attach(unit));
        SHR_IF_ERR_EXIT(q2a_a0_data_mpls_attach(unit));
    }
    else

#endif 
#ifdef BCM_DNX2_SUPPORT

    if (dnxc_data_mgmt_is_q2a_b0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_mpls_attach(unit));
        SHR_IF_ERR_EXIT(q2a_a0_data_mpls_attach(unit));
        SHR_IF_ERR_EXIT(q2a_b0_data_mpls_attach(unit));
    }
    else

#endif 
#ifdef BCM_DNX2_SUPPORT

    if (dnxc_data_mgmt_is_q2a_b1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_mpls_attach(unit));
        SHR_IF_ERR_EXIT(q2a_a0_data_mpls_attach(unit));
        SHR_IF_ERR_EXIT(q2a_b0_data_mpls_attach(unit));
    }
    else

#endif 
#ifdef BCM_DNX2_SUPPORT

    if (dnxc_data_mgmt_is_j2p_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_mpls_attach(unit));
        SHR_IF_ERR_EXIT(j2p_a0_data_mpls_attach(unit));
    }
    else

#endif 
#ifdef BCM_DNX2_SUPPORT

    if (dnxc_data_mgmt_is_j2p_a1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_mpls_attach(unit));
        SHR_IF_ERR_EXIT(j2p_a0_data_mpls_attach(unit));
    }
    else

#endif 
#ifdef BCM_DNX2_SUPPORT

    if (dnxc_data_mgmt_is_j2p_a2(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_mpls_attach(unit));
        SHR_IF_ERR_EXIT(j2p_a0_data_mpls_attach(unit));
    }
    else

#endif 
#ifdef BCM_DNX2_SUPPORT

    if (dnxc_data_mgmt_is_j2x_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_mpls_attach(unit));
        SHR_IF_ERR_EXIT(j2x_a0_data_mpls_attach(unit));
    }
    else

#endif 
    {
        SHR_EXIT();
    }

exit:
    SHR_FUNC_EXIT;
}
#undef BSL_LOG_MODULE

