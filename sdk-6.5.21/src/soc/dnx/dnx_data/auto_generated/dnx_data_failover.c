

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_FAILOVER

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_failover.h>



extern shr_error_e jr2_a0_data_failover_attach(
    int unit);



static shr_error_e
dnx_data_failover_path_select_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "path_select";
    submodule_data->doc = "protection path select data";
    
    submodule_data->nof_features = _dnx_data_failover_path_select_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data failover path_select features");

    
    submodule_data->nof_defines = _dnx_data_failover_path_select_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data failover path_select defines");

    submodule_data->defines[dnx_data_failover_path_select_define_egress_size].name = "egress_size";
    submodule_data->defines[dnx_data_failover_path_select_define_egress_size].doc = "size of egress path select table";
    
    submodule_data->defines[dnx_data_failover_path_select_define_egress_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_failover_path_select_define_ingress_size].name = "ingress_size";
    submodule_data->defines[dnx_data_failover_path_select_define_ingress_size].doc = "size of ingress path select table";
    
    submodule_data->defines[dnx_data_failover_path_select_define_ingress_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_failover_path_select_define_fec_size].name = "fec_size";
    submodule_data->defines[dnx_data_failover_path_select_define_fec_size].doc = "size of fec path select table";
    
    submodule_data->defines[dnx_data_failover_path_select_define_fec_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_failover_path_select_define_egr_no_protection].name = "egr_no_protection";
    submodule_data->defines[dnx_data_failover_path_select_define_egr_no_protection].doc = "protection path select pointer dedicate for no egress protection";
    
    submodule_data->defines[dnx_data_failover_path_select_define_egr_no_protection].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_failover_path_select_define_ing_no_protection].name = "ing_no_protection";
    submodule_data->defines[dnx_data_failover_path_select_define_ing_no_protection].doc = "protection path select pointer dedicate for no ingress protection";
    
    submodule_data->defines[dnx_data_failover_path_select_define_ing_no_protection].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_failover_path_select_define_fec_no_protection].name = "fec_no_protection";
    submodule_data->defines[dnx_data_failover_path_select_define_fec_no_protection].doc = "protection path select pointer dedicate for no fec protection";
    
    submodule_data->defines[dnx_data_failover_path_select_define_fec_no_protection].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_failover_path_select_define_fec_facility_protection].name = "fec_facility_protection";
    submodule_data->defines[dnx_data_failover_path_select_define_fec_facility_protection].doc = "protection path select pointer dedicate for fec facility protection";
    
    submodule_data->defines[dnx_data_failover_path_select_define_fec_facility_protection].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_failover_path_select_define_nof_fec_path_select_banks].name = "nof_fec_path_select_banks";
    submodule_data->defines[dnx_data_failover_path_select_define_nof_fec_path_select_banks].doc = "number of banks included in fec protection path select table";
    
    submodule_data->defines[dnx_data_failover_path_select_define_nof_fec_path_select_banks].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_failover_path_select_define_fec_bank_size].name = "fec_bank_size";
    submodule_data->defines[dnx_data_failover_path_select_define_fec_bank_size].doc = "failover fec bank size";
    
    submodule_data->defines[dnx_data_failover_path_select_define_fec_bank_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_failover_path_select_define_in_lif_protection_pointer_nof_msb].name = "in_lif_protection_pointer_nof_msb";
    submodule_data->defines[dnx_data_failover_path_select_define_in_lif_protection_pointer_nof_msb].doc = "Number MS bits from protection pointer used for HW access to the fields(0-3) of memory IPPA_VTT_PATH_SELECT.";
    
    submodule_data->defines[dnx_data_failover_path_select_define_in_lif_protection_pointer_nof_msb].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_failover_path_select_define_in_lif_protection_pointer_nof_lsb].name = "in_lif_protection_pointer_nof_lsb";
    submodule_data->defines[dnx_data_failover_path_select_define_in_lif_protection_pointer_nof_lsb].doc = "Number LS bits from protection pointer used for HW access to the entries (0-4095) of memory IPPA_VTT_PATH_SELECT.";
    
    submodule_data->defines[dnx_data_failover_path_select_define_in_lif_protection_pointer_nof_lsb].flags |= DNXC_DATA_F_DEFINE;

    
    submodule_data->nof_tables = _dnx_data_failover_path_select_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data failover path_select tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_failover_path_select_feature_get(
    int unit,
    dnx_data_failover_path_select_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_failover, dnx_data_failover_submodule_path_select, feature);
}


uint32
dnx_data_failover_path_select_egress_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_failover, dnx_data_failover_submodule_path_select, dnx_data_failover_path_select_define_egress_size);
}

uint32
dnx_data_failover_path_select_ingress_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_failover, dnx_data_failover_submodule_path_select, dnx_data_failover_path_select_define_ingress_size);
}

uint32
dnx_data_failover_path_select_fec_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_failover, dnx_data_failover_submodule_path_select, dnx_data_failover_path_select_define_fec_size);
}

uint32
dnx_data_failover_path_select_egr_no_protection_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_failover, dnx_data_failover_submodule_path_select, dnx_data_failover_path_select_define_egr_no_protection);
}

uint32
dnx_data_failover_path_select_ing_no_protection_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_failover, dnx_data_failover_submodule_path_select, dnx_data_failover_path_select_define_ing_no_protection);
}

uint32
dnx_data_failover_path_select_fec_no_protection_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_failover, dnx_data_failover_submodule_path_select, dnx_data_failover_path_select_define_fec_no_protection);
}

uint32
dnx_data_failover_path_select_fec_facility_protection_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_failover, dnx_data_failover_submodule_path_select, dnx_data_failover_path_select_define_fec_facility_protection);
}

uint32
dnx_data_failover_path_select_nof_fec_path_select_banks_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_failover, dnx_data_failover_submodule_path_select, dnx_data_failover_path_select_define_nof_fec_path_select_banks);
}

uint32
dnx_data_failover_path_select_fec_bank_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_failover, dnx_data_failover_submodule_path_select, dnx_data_failover_path_select_define_fec_bank_size);
}

uint32
dnx_data_failover_path_select_in_lif_protection_pointer_nof_msb_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_failover, dnx_data_failover_submodule_path_select, dnx_data_failover_path_select_define_in_lif_protection_pointer_nof_msb);
}

uint32
dnx_data_failover_path_select_in_lif_protection_pointer_nof_lsb_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_failover, dnx_data_failover_submodule_path_select, dnx_data_failover_path_select_define_in_lif_protection_pointer_nof_lsb);
}










static shr_error_e
dnx_data_failover_facility_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "facility";
    submodule_data->doc = "Facility protection data";
    
    submodule_data->nof_features = _dnx_data_failover_facility_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data failover facility features");

    submodule_data->features[dnx_data_failover_facility_accelerated_mode_enable].name = "accelerated_mode_enable";
    submodule_data->features[dnx_data_failover_facility_accelerated_mode_enable].doc = "enable/disable accelerated facility protection.";
    submodule_data->features[dnx_data_failover_facility_accelerated_mode_enable].flags |= DNXC_DATA_F_FEATURE;

    
    submodule_data->nof_defines = _dnx_data_failover_facility_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data failover facility defines");

    
    submodule_data->nof_tables = _dnx_data_failover_facility_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data failover facility tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_failover_facility_feature_get(
    int unit,
    dnx_data_failover_facility_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_failover, dnx_data_failover_submodule_facility, feature);
}








shr_error_e
dnx_data_failover_init(
    int unit,
    dnxc_data_module_t *module_data)
{
    SHR_FUNC_INIT_VARS(unit);

    
    module_data->name = "failover";
    module_data->nof_submodules = _dnx_data_failover_submodule_nof;
    DNXC_DATA_ALLOC(module_data->submodules, dnxc_data_submodule_t, module_data->nof_submodules, "_dnxc_data failover submodules");

    
    SHR_IF_ERR_EXIT(dnx_data_failover_path_select_init(unit, &module_data->submodules[dnx_data_failover_submodule_path_select]));
    SHR_IF_ERR_EXIT(dnx_data_failover_facility_init(unit, &module_data->submodules[dnx_data_failover_submodule_facility]));
    
    if (dnxc_data_mgmt_is_jr2_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_failover_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_jr2_b0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_failover_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_jr2_b1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_failover_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_j2c_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_failover_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_j2c_a1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_failover_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_q2a_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_failover_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_q2a_b0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_failover_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_q2a_b1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_failover_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_j2p_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_failover_attach(unit));
    }
    else
    {
        SHR_EXIT();
    }

exit:
    SHR_FUNC_EXIT;
}
#undef BSL_LOG_MODULE

