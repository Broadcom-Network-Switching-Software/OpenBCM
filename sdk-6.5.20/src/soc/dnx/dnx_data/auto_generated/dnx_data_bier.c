

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_BIERDNX

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_bier.h>



extern shr_error_e jr2_a0_data_bier_attach(
    int unit);
extern shr_error_e jr2_b0_data_bier_attach(
    int unit);
extern shr_error_e j2c_a0_data_bier_attach(
    int unit);
extern shr_error_e q2a_a0_data_bier_attach(
    int unit);



static shr_error_e
dnx_data_bier_params_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "params";
    submodule_data->doc = "bier parameters";
    
    submodule_data->nof_features = _dnx_data_bier_params_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data bier params features");

    submodule_data->features[dnx_data_bier_params_is_supported].name = "is_supported";
    submodule_data->features[dnx_data_bier_params_is_supported].doc = "Is bier supported";
    submodule_data->features[dnx_data_bier_params_is_supported].flags |= DNXC_DATA_F_FEATURE;

    
    submodule_data->nof_defines = _dnx_data_bier_params_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data bier params defines");

    submodule_data->defines[dnx_data_bier_params_define_bitstring_size].name = "bitstring_size";
    submodule_data->defines[dnx_data_bier_params_define_bitstring_size].doc = "bier bitstring size";
    
    submodule_data->defines[dnx_data_bier_params_define_bitstring_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_bier_params_define_bfr_entry_size].name = "bfr_entry_size";
    submodule_data->defines[dnx_data_bier_params_define_bfr_entry_size].doc = "the size of bfr entry in mcdb";
    
    submodule_data->defines[dnx_data_bier_params_define_bfr_entry_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_bier_params_define_bank_entries].name = "bank_entries";
    submodule_data->defines[dnx_data_bier_params_define_bank_entries].doc = "the number of bfr entries at a bank";
    
    submodule_data->defines[dnx_data_bier_params_define_bank_entries].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_bier_params_define_bundle_nof_banks].name = "bundle_nof_banks";
    submodule_data->defines[dnx_data_bier_params_define_bundle_nof_banks].doc = "the number of banks in a bundle";
    
    submodule_data->defines[dnx_data_bier_params_define_bundle_nof_banks].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_bier_params_define_bundle_nof_banks_net].name = "bundle_nof_banks_net";
    submodule_data->defines[dnx_data_bier_params_define_bundle_nof_banks_net].doc = "the number of banks in a bundle which reffered with BFR-id";
    
    submodule_data->defines[dnx_data_bier_params_define_bundle_nof_banks_net].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_bier_params_define_egress_only].name = "egress_only";
    submodule_data->defines[dnx_data_bier_params_define_egress_only].doc = "no ingress replication in the system";
    
    submodule_data->defines[dnx_data_bier_params_define_egress_only].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_bier_params_define_nof_egr_table_copies].name = "nof_egr_table_copies";
    submodule_data->defines[dnx_data_bier_params_define_nof_egr_table_copies].doc = "the number of egress table copies";
    
    submodule_data->defines[dnx_data_bier_params_define_nof_egr_table_copies].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_bier_params_define_ingress_bfr_pos_msb_start].name = "ingress_bfr_pos_msb_start";
    submodule_data->defines[dnx_data_bier_params_define_ingress_bfr_pos_msb_start].doc = "ingress bfr table start from msb";
    
    submodule_data->defines[dnx_data_bier_params_define_ingress_bfr_pos_msb_start].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_bier_params_define_nof_bfr_entries].name = "nof_bfr_entries";
    submodule_data->defines[dnx_data_bier_params_define_nof_bfr_entries].doc = "bier supported bfr size";
    
    submodule_data->defines[dnx_data_bier_params_define_nof_bfr_entries].flags |= DNXC_DATA_F_NUMERIC;

    
    submodule_data->nof_tables = _dnx_data_bier_params_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data bier params tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_bier_params_feature_get(
    int unit,
    dnx_data_bier_params_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_bier, dnx_data_bier_submodule_params, feature);
}


uint32
dnx_data_bier_params_bitstring_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_bier, dnx_data_bier_submodule_params, dnx_data_bier_params_define_bitstring_size);
}

uint32
dnx_data_bier_params_bfr_entry_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_bier, dnx_data_bier_submodule_params, dnx_data_bier_params_define_bfr_entry_size);
}

uint32
dnx_data_bier_params_bank_entries_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_bier, dnx_data_bier_submodule_params, dnx_data_bier_params_define_bank_entries);
}

uint32
dnx_data_bier_params_bundle_nof_banks_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_bier, dnx_data_bier_submodule_params, dnx_data_bier_params_define_bundle_nof_banks);
}

uint32
dnx_data_bier_params_bundle_nof_banks_net_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_bier, dnx_data_bier_submodule_params, dnx_data_bier_params_define_bundle_nof_banks_net);
}

uint32
dnx_data_bier_params_egress_only_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_bier, dnx_data_bier_submodule_params, dnx_data_bier_params_define_egress_only);
}

uint32
dnx_data_bier_params_nof_egr_table_copies_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_bier, dnx_data_bier_submodule_params, dnx_data_bier_params_define_nof_egr_table_copies);
}

uint32
dnx_data_bier_params_ingress_bfr_pos_msb_start_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_bier, dnx_data_bier_submodule_params, dnx_data_bier_params_define_ingress_bfr_pos_msb_start);
}

uint32
dnx_data_bier_params_nof_bfr_entries_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_bier, dnx_data_bier_submodule_params, dnx_data_bier_params_define_nof_bfr_entries);
}







shr_error_e
dnx_data_bier_init(
    int unit,
    dnxc_data_module_t *module_data)
{
    SHR_FUNC_INIT_VARS(unit);

    
    module_data->name = "bier";
    module_data->nof_submodules = _dnx_data_bier_submodule_nof;
    DNXC_DATA_ALLOC(module_data->submodules, dnxc_data_submodule_t, module_data->nof_submodules, "_dnxc_data bier submodules");

    
    SHR_IF_ERR_EXIT(dnx_data_bier_params_init(unit, &module_data->submodules[dnx_data_bier_submodule_params]));
    
    if (dnxc_data_mgmt_is_jr2_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_bier_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_jr2_b0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_bier_attach(unit));
        SHR_IF_ERR_EXIT(jr2_b0_data_bier_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_jr2_b1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_bier_attach(unit));
        SHR_IF_ERR_EXIT(jr2_b0_data_bier_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_j2c_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_bier_attach(unit));
        SHR_IF_ERR_EXIT(j2c_a0_data_bier_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_j2c_a1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_bier_attach(unit));
        SHR_IF_ERR_EXIT(j2c_a0_data_bier_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_q2a_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_bier_attach(unit));
        SHR_IF_ERR_EXIT(q2a_a0_data_bier_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_q2a_b0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_bier_attach(unit));
        SHR_IF_ERR_EXIT(q2a_a0_data_bier_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_q2a_b1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_bier_attach(unit));
        SHR_IF_ERR_EXIT(q2a_a0_data_bier_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_j2p_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_bier_attach(unit));
    }
    else
    {
        SHR_EXIT();
    }

exit:
    SHR_FUNC_EXIT;
}
#undef BSL_LOG_MODULE

