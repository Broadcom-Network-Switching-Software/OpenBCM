
/* *INDENT-OFF* */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_COMMON

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_module_testing.h>




extern shr_error_e jr2_a0_data_module_testing_attach(
    int unit);


extern shr_error_e j2c_a0_data_module_testing_attach(
    int unit);


extern shr_error_e q2a_a0_data_module_testing_attach(
    int unit);


extern shr_error_e j2p_a0_data_module_testing_attach(
    int unit);




static shr_error_e
dnx_data_module_testing_unsupported_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "unsupported";
    submodule_data->doc = "";
    
    submodule_data->nof_features = _dnx_data_module_testing_unsupported_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data module_testing unsupported features");

    submodule_data->features[dnx_data_module_testing_unsupported_supported_feature].name = "supported_feature";
    submodule_data->features[dnx_data_module_testing_unsupported_supported_feature].doc = "";
    submodule_data->features[dnx_data_module_testing_unsupported_supported_feature].labels[0] = "test";
    submodule_data->features[dnx_data_module_testing_unsupported_supported_feature].labels[1] = "feature";
    submodule_data->features[dnx_data_module_testing_unsupported_supported_feature].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_module_testing_unsupported_unsupported_feature].name = "unsupported_feature";
    submodule_data->features[dnx_data_module_testing_unsupported_unsupported_feature].doc = "";
    submodule_data->features[dnx_data_module_testing_unsupported_unsupported_feature].labels[0] = "try";
    submodule_data->features[dnx_data_module_testing_unsupported_unsupported_feature].labels[1] = "label";
    submodule_data->features[dnx_data_module_testing_unsupported_unsupported_feature].flags |= DNXC_DATA_F_FEATURE;

    
    submodule_data->nof_defines = _dnx_data_module_testing_unsupported_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data module_testing unsupported defines");

    submodule_data->defines[dnx_data_module_testing_unsupported_define_supported_def].name = "supported_def";
    submodule_data->defines[dnx_data_module_testing_unsupported_define_supported_def].doc = "";
    submodule_data->defines[dnx_data_module_testing_unsupported_define_supported_def].labels[0] = "test";
    submodule_data->defines[dnx_data_module_testing_unsupported_define_supported_def].labels[1] = "define";
    
    submodule_data->defines[dnx_data_module_testing_unsupported_define_supported_def].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_module_testing_unsupported_define_unsupported_def].name = "unsupported_def";
    submodule_data->defines[dnx_data_module_testing_unsupported_define_unsupported_def].doc = "";
    submodule_data->defines[dnx_data_module_testing_unsupported_define_unsupported_def].labels[0] = "try";
    
    submodule_data->defines[dnx_data_module_testing_unsupported_define_unsupported_def].flags |= DNXC_DATA_F_DEFINE;

    
    submodule_data->nof_tables = _dnx_data_module_testing_unsupported_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data module_testing unsupported tables");

    
    submodule_data->tables[dnx_data_module_testing_unsupported_table_supported_table].name = "supported_table";
    submodule_data->tables[dnx_data_module_testing_unsupported_table_supported_table].doc = "";
    submodule_data->tables[dnx_data_module_testing_unsupported_table_supported_table].labels[0] = "test";
    submodule_data->tables[dnx_data_module_testing_unsupported_table_supported_table].labels[1] = "table";
    submodule_data->tables[dnx_data_module_testing_unsupported_table_supported_table].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_module_testing_unsupported_table_supported_table].size_of_values = sizeof(dnx_data_module_testing_unsupported_supported_table_t);
    submodule_data->tables[dnx_data_module_testing_unsupported_table_supported_table].entry_get = dnx_data_module_testing_unsupported_supported_table_entry_str_get;

    
    submodule_data->tables[dnx_data_module_testing_unsupported_table_supported_table].nof_keys = 0;

    
    submodule_data->tables[dnx_data_module_testing_unsupported_table_supported_table].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_module_testing_unsupported_table_supported_table].values, dnxc_data_value_t, submodule_data->tables[dnx_data_module_testing_unsupported_table_supported_table].nof_values, "_dnx_data_module_testing_unsupported_table_supported_table table values");
    submodule_data->tables[dnx_data_module_testing_unsupported_table_supported_table].values[0].name = "num";
    submodule_data->tables[dnx_data_module_testing_unsupported_table_supported_table].values[0].type = "uint32";
    submodule_data->tables[dnx_data_module_testing_unsupported_table_supported_table].values[0].doc = "generic number";
    submodule_data->tables[dnx_data_module_testing_unsupported_table_supported_table].values[0].offset = UTILEX_OFFSETOF(dnx_data_module_testing_unsupported_supported_table_t, num);

    
    submodule_data->tables[dnx_data_module_testing_unsupported_table_unsupported_table].name = "unsupported_table";
    submodule_data->tables[dnx_data_module_testing_unsupported_table_unsupported_table].doc = "";
    submodule_data->tables[dnx_data_module_testing_unsupported_table_unsupported_table].labels[0] = "try";
    submodule_data->tables[dnx_data_module_testing_unsupported_table_unsupported_table].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_module_testing_unsupported_table_unsupported_table].size_of_values = sizeof(dnx_data_module_testing_unsupported_unsupported_table_t);
    submodule_data->tables[dnx_data_module_testing_unsupported_table_unsupported_table].entry_get = dnx_data_module_testing_unsupported_unsupported_table_entry_str_get;

    
    submodule_data->tables[dnx_data_module_testing_unsupported_table_unsupported_table].nof_keys = 0;

    
    submodule_data->tables[dnx_data_module_testing_unsupported_table_unsupported_table].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_module_testing_unsupported_table_unsupported_table].values, dnxc_data_value_t, submodule_data->tables[dnx_data_module_testing_unsupported_table_unsupported_table].nof_values, "_dnx_data_module_testing_unsupported_table_unsupported_table table values");
    submodule_data->tables[dnx_data_module_testing_unsupported_table_unsupported_table].values[0].name = "num";
    submodule_data->tables[dnx_data_module_testing_unsupported_table_unsupported_table].values[0].type = "uint32";
    submodule_data->tables[dnx_data_module_testing_unsupported_table_unsupported_table].values[0].doc = "generic number";
    submodule_data->tables[dnx_data_module_testing_unsupported_table_unsupported_table].values[0].offset = UTILEX_OFFSETOF(dnx_data_module_testing_unsupported_unsupported_table_t, num);


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_module_testing_unsupported_feature_get(
    int unit,
    dnx_data_module_testing_unsupported_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_module_testing, dnx_data_module_testing_submodule_unsupported, feature);
}


uint32
dnx_data_module_testing_unsupported_supported_def_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_module_testing, dnx_data_module_testing_submodule_unsupported, dnx_data_module_testing_unsupported_define_supported_def);
}

uint32
dnx_data_module_testing_unsupported_unsupported_def_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_module_testing, dnx_data_module_testing_submodule_unsupported, dnx_data_module_testing_unsupported_define_unsupported_def);
}



const dnx_data_module_testing_unsupported_supported_table_t *
dnx_data_module_testing_unsupported_supported_table_get(
    int unit)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_module_testing, dnx_data_module_testing_submodule_unsupported, dnx_data_module_testing_unsupported_table_supported_table);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
    return (const dnx_data_module_testing_unsupported_supported_table_t *) data;

}

const dnx_data_module_testing_unsupported_unsupported_table_t *
dnx_data_module_testing_unsupported_unsupported_table_get(
    int unit)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_module_testing, dnx_data_module_testing_submodule_unsupported, dnx_data_module_testing_unsupported_table_unsupported_table);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
    return (const dnx_data_module_testing_unsupported_unsupported_table_t *) data;

}


shr_error_e
dnx_data_module_testing_unsupported_supported_table_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_module_testing_unsupported_supported_table_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_module_testing, dnx_data_module_testing_submodule_unsupported, dnx_data_module_testing_unsupported_table_supported_table);
    data = (const dnx_data_module_testing_unsupported_supported_table_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, 0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->num);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_module_testing_unsupported_unsupported_table_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_module_testing_unsupported_unsupported_table_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_module_testing, dnx_data_module_testing_submodule_unsupported, dnx_data_module_testing_unsupported_table_unsupported_table);
    data = (const dnx_data_module_testing_unsupported_unsupported_table_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, 0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->num);
            break;
    }

    SHR_FUNC_EXIT;
}


const dnxc_data_table_info_t *
dnx_data_module_testing_unsupported_supported_table_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_module_testing, dnx_data_module_testing_submodule_unsupported, dnx_data_module_testing_unsupported_table_supported_table);

}

const dnxc_data_table_info_t *
dnx_data_module_testing_unsupported_unsupported_table_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_module_testing, dnx_data_module_testing_submodule_unsupported, dnx_data_module_testing_unsupported_table_unsupported_table);

}






static shr_error_e
dnx_data_module_testing_standard_submodule_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "standard_submodule";
    submodule_data->doc = "";
    
    submodule_data->nof_features = _dnx_data_module_testing_standard_submodule_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data module_testing standard_submodule features");

    submodule_data->features[dnx_data_module_testing_standard_submodule_standard_feature].name = "standard_feature";
    submodule_data->features[dnx_data_module_testing_standard_submodule_standard_feature].doc = "";
    submodule_data->features[dnx_data_module_testing_standard_submodule_standard_feature].flags |= DNXC_DATA_F_FEATURE;


    submodule_data->features[dnx_data_module_testing_standard_submodule_feature_family_dnx2].name = "feature_family_dnx2";
    submodule_data->features[dnx_data_module_testing_standard_submodule_feature_family_dnx2].doc = "";
    submodule_data->features[dnx_data_module_testing_standard_submodule_feature_family_dnx2].flags |= DNXC_DATA_F_FEATURE;
    if (!soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->features[dnx_data_module_testing_standard_submodule_feature_family_dnx2].data = 0;
    }

    
    submodule_data->nof_defines = _dnx_data_module_testing_standard_submodule_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data module_testing standard_submodule defines");

    submodule_data->defines[dnx_data_module_testing_standard_submodule_define_standard_define].name = "standard_define";
    submodule_data->defines[dnx_data_module_testing_standard_submodule_define_standard_define].doc = "";
    
    submodule_data->defines[dnx_data_module_testing_standard_submodule_define_standard_define].flags |= DNXC_DATA_F_DEFINE;


    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_module_testing_standard_submodule_define_define_family_dnx2].name = "define_family_dnx2";
        submodule_data->defines[dnx_data_module_testing_standard_submodule_define_define_family_dnx2].doc = "";
        
        submodule_data->defines[dnx_data_module_testing_standard_submodule_define_define_family_dnx2].flags |= DNXC_DATA_F_DEFINE;
    }

    submodule_data->defines[dnx_data_module_testing_standard_submodule_define_standard_numeric].name = "standard_numeric";
    submodule_data->defines[dnx_data_module_testing_standard_submodule_define_standard_numeric].doc = "";
    
    submodule_data->defines[dnx_data_module_testing_standard_submodule_define_standard_numeric].flags |= DNXC_DATA_F_NUMERIC;


    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_module_testing_standard_submodule_define_num_family_dnx2].name = "num_family_dnx2";
        submodule_data->defines[dnx_data_module_testing_standard_submodule_define_num_family_dnx2].doc = "";
        
        submodule_data->defines[dnx_data_module_testing_standard_submodule_define_num_family_dnx2].flags |= DNXC_DATA_F_NUMERIC;
    }


    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_module_testing_standard_submodule_define_num_property_family_dnx2].name = "num_property_family_dnx2";
        submodule_data->defines[dnx_data_module_testing_standard_submodule_define_num_property_family_dnx2].doc = "";
        
        submodule_data->defines[dnx_data_module_testing_standard_submodule_define_num_property_family_dnx2].flags |= DNXC_DATA_F_NUMERIC;
    }

    
    submodule_data->nof_tables = _dnx_data_module_testing_standard_submodule_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data module_testing standard_submodule tables");

    
    submodule_data->tables[dnx_data_module_testing_standard_submodule_table_standard_table].name = "standard_table";
    submodule_data->tables[dnx_data_module_testing_standard_submodule_table_standard_table].doc = "";
    submodule_data->tables[dnx_data_module_testing_standard_submodule_table_standard_table].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_module_testing_standard_submodule_table_standard_table].size_of_values = sizeof(dnx_data_module_testing_standard_submodule_standard_table_t);
    submodule_data->tables[dnx_data_module_testing_standard_submodule_table_standard_table].entry_get = dnx_data_module_testing_standard_submodule_standard_table_entry_str_get;

    
    submodule_data->tables[dnx_data_module_testing_standard_submodule_table_standard_table].nof_keys = 1;
    submodule_data->tables[dnx_data_module_testing_standard_submodule_table_standard_table].keys[0].name = "standard_table_key";
    submodule_data->tables[dnx_data_module_testing_standard_submodule_table_standard_table].keys[0].doc = "";

    
    submodule_data->tables[dnx_data_module_testing_standard_submodule_table_standard_table].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_module_testing_standard_submodule_table_standard_table].values, dnxc_data_value_t, submodule_data->tables[dnx_data_module_testing_standard_submodule_table_standard_table].nof_values, "_dnx_data_module_testing_standard_submodule_table_standard_table table values");
    submodule_data->tables[dnx_data_module_testing_standard_submodule_table_standard_table].values[0].name = "standard_table_value";
    submodule_data->tables[dnx_data_module_testing_standard_submodule_table_standard_table].values[0].type = "uint32";
    submodule_data->tables[dnx_data_module_testing_standard_submodule_table_standard_table].values[0].doc = "table value";
    submodule_data->tables[dnx_data_module_testing_standard_submodule_table_standard_table].values[0].offset = UTILEX_OFFSETOF(dnx_data_module_testing_standard_submodule_standard_table_t, standard_table_value);


    if (soc_is(unit, DNX2_DEVICE))
    {
        
        submodule_data->tables[dnx_data_module_testing_standard_submodule_table_table_family_dnx2].name = "table_family_dnx2";
        submodule_data->tables[dnx_data_module_testing_standard_submodule_table_table_family_dnx2].doc = "";
        submodule_data->tables[dnx_data_module_testing_standard_submodule_table_table_family_dnx2].flags |= DNXC_DATA_F_TABLE;
        submodule_data->tables[dnx_data_module_testing_standard_submodule_table_table_family_dnx2].size_of_values = sizeof(dnx_data_module_testing_standard_submodule_table_family_dnx2_t);
        submodule_data->tables[dnx_data_module_testing_standard_submodule_table_table_family_dnx2].entry_get = dnx_data_module_testing_standard_submodule_table_family_dnx2_entry_str_get;

        
        submodule_data->tables[dnx_data_module_testing_standard_submodule_table_table_family_dnx2].nof_keys = 1;
        submodule_data->tables[dnx_data_module_testing_standard_submodule_table_table_family_dnx2].keys[0].name = "table_family_dnx2_key";
        submodule_data->tables[dnx_data_module_testing_standard_submodule_table_table_family_dnx2].keys[0].doc = "";

        
        submodule_data->tables[dnx_data_module_testing_standard_submodule_table_table_family_dnx2].nof_values = 1;
        DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_module_testing_standard_submodule_table_table_family_dnx2].values, dnxc_data_value_t, submodule_data->tables[dnx_data_module_testing_standard_submodule_table_table_family_dnx2].nof_values, "_dnx_data_module_testing_standard_submodule_table_table_family_dnx2 table values");
        submodule_data->tables[dnx_data_module_testing_standard_submodule_table_table_family_dnx2].values[0].name = "table_family_dnx2_value";
        submodule_data->tables[dnx_data_module_testing_standard_submodule_table_table_family_dnx2].values[0].type = "uint32";
        submodule_data->tables[dnx_data_module_testing_standard_submodule_table_table_family_dnx2].values[0].doc = "table value";
        submodule_data->tables[dnx_data_module_testing_standard_submodule_table_table_family_dnx2].values[0].offset = UTILEX_OFFSETOF(dnx_data_module_testing_standard_submodule_table_family_dnx2_t, table_family_dnx2_value);
    }


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_module_testing_standard_submodule_feature_get(
    int unit,
    dnx_data_module_testing_standard_submodule_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_module_testing, dnx_data_module_testing_submodule_standard_submodule, feature);
}


uint32
dnx_data_module_testing_standard_submodule_standard_define_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_module_testing, dnx_data_module_testing_submodule_standard_submodule, dnx_data_module_testing_standard_submodule_define_standard_define);
}


uint32
dnx_data_module_testing_standard_submodule_define_family_dnx2_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "define_family_dnx2", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_module_testing, dnx_data_module_testing_submodule_standard_submodule, dnx_data_module_testing_standard_submodule_define_define_family_dnx2);
}


uint32
dnx_data_module_testing_standard_submodule_standard_numeric_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_module_testing, dnx_data_module_testing_submodule_standard_submodule, dnx_data_module_testing_standard_submodule_define_standard_numeric);
}


uint32
dnx_data_module_testing_standard_submodule_num_family_dnx2_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "num_family_dnx2", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_module_testing, dnx_data_module_testing_submodule_standard_submodule, dnx_data_module_testing_standard_submodule_define_num_family_dnx2);
}



uint32
dnx_data_module_testing_standard_submodule_num_property_family_dnx2_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "num_property_family_dnx2", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_module_testing, dnx_data_module_testing_submodule_standard_submodule, dnx_data_module_testing_standard_submodule_define_num_property_family_dnx2);
}




const dnx_data_module_testing_standard_submodule_standard_table_t *
dnx_data_module_testing_standard_submodule_standard_table_get(
    int unit,
    int standard_table_key)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_module_testing, dnx_data_module_testing_submodule_standard_submodule, dnx_data_module_testing_standard_submodule_table_standard_table);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, standard_table_key, 0);
    return (const dnx_data_module_testing_standard_submodule_standard_table_t *) data;

}


const dnx_data_module_testing_standard_submodule_table_family_dnx2_t *
dnx_data_module_testing_standard_submodule_table_family_dnx2_get(
    int unit,
    int table_family_dnx2_key)
{
    char *data;
    dnxc_data_table_t *table;

    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "table_family_dnx2", "DNX2");
    }
    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_module_testing, dnx_data_module_testing_submodule_standard_submodule, dnx_data_module_testing_standard_submodule_table_table_family_dnx2);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, table_family_dnx2_key, 0);
    return (const dnx_data_module_testing_standard_submodule_table_family_dnx2_t *) data;

}



shr_error_e
dnx_data_module_testing_standard_submodule_standard_table_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_module_testing_standard_submodule_standard_table_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_module_testing, dnx_data_module_testing_submodule_standard_submodule, dnx_data_module_testing_standard_submodule_table_standard_table);
    data = (const dnx_data_module_testing_standard_submodule_standard_table_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->standard_table_value);
            break;
    }

    SHR_FUNC_EXIT;
}


shr_error_e
dnx_data_module_testing_standard_submodule_table_family_dnx2_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_module_testing_standard_submodule_table_family_dnx2_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_module_testing, dnx_data_module_testing_submodule_standard_submodule, dnx_data_module_testing_standard_submodule_table_table_family_dnx2);
    data = (const dnx_data_module_testing_standard_submodule_table_family_dnx2_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->table_family_dnx2_value);
            break;
    }

    SHR_FUNC_EXIT;
}



const dnxc_data_table_info_t *
dnx_data_module_testing_standard_submodule_standard_table_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_module_testing, dnx_data_module_testing_submodule_standard_submodule, dnx_data_module_testing_standard_submodule_table_standard_table);

}


const dnxc_data_table_info_t *
dnx_data_module_testing_standard_submodule_table_family_dnx2_info_get(
    int unit)
{

    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "table_family_dnx2", "DNX2");
    }
    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_module_testing, dnx_data_module_testing_submodule_standard_submodule, dnx_data_module_testing_standard_submodule_table_table_family_dnx2);

}







static shr_error_e
dnx_data_module_testing_types_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "types";
    submodule_data->doc = "";
    
    submodule_data->nof_features = _dnx_data_module_testing_types_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data module_testing types features");

    
    submodule_data->nof_defines = _dnx_data_module_testing_types_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data module_testing types defines");

    submodule_data->defines[dnx_data_module_testing_types_define_def_signed].name = "def_signed";
    submodule_data->defines[dnx_data_module_testing_types_define_def_signed].doc = "";
    
    submodule_data->defines[dnx_data_module_testing_types_define_def_signed].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_module_testing_types_define_def_unsigned].name = "def_unsigned";
    submodule_data->defines[dnx_data_module_testing_types_define_def_unsigned].doc = "";
    
    submodule_data->defines[dnx_data_module_testing_types_define_def_unsigned].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_module_testing_types_define_num_signed].name = "num_signed";
    submodule_data->defines[dnx_data_module_testing_types_define_num_signed].doc = "";
    
    submodule_data->defines[dnx_data_module_testing_types_define_num_signed].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_module_testing_types_define_num_unsigned].name = "num_unsigned";
    submodule_data->defines[dnx_data_module_testing_types_define_num_unsigned].doc = "";
    
    submodule_data->defines[dnx_data_module_testing_types_define_num_unsigned].flags |= DNXC_DATA_F_NUMERIC;

    
    submodule_data->nof_tables = _dnx_data_module_testing_types_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data module_testing types tables");

    
    submodule_data->tables[dnx_data_module_testing_types_table_all].name = "all";
    submodule_data->tables[dnx_data_module_testing_types_table_all].doc = "";
    submodule_data->tables[dnx_data_module_testing_types_table_all].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_module_testing_types_table_all].size_of_values = sizeof(dnx_data_module_testing_types_all_t);
    submodule_data->tables[dnx_data_module_testing_types_table_all].entry_get = dnx_data_module_testing_types_all_entry_str_get;

    
    submodule_data->tables[dnx_data_module_testing_types_table_all].nof_keys = 0;

    
    submodule_data->tables[dnx_data_module_testing_types_table_all].nof_values = 10;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_module_testing_types_table_all].values, dnxc_data_value_t, submodule_data->tables[dnx_data_module_testing_types_table_all].nof_values, "_dnx_data_module_testing_types_table_all table values");
    submodule_data->tables[dnx_data_module_testing_types_table_all].values[0].name = "intval";
    submodule_data->tables[dnx_data_module_testing_types_table_all].values[0].type = "int";
    submodule_data->tables[dnx_data_module_testing_types_table_all].values[0].doc = "test int";
    submodule_data->tables[dnx_data_module_testing_types_table_all].values[0].offset = UTILEX_OFFSETOF(dnx_data_module_testing_types_all_t, intval);
    submodule_data->tables[dnx_data_module_testing_types_table_all].values[1].name = "uint8val";
    submodule_data->tables[dnx_data_module_testing_types_table_all].values[1].type = "uint8";
    submodule_data->tables[dnx_data_module_testing_types_table_all].values[1].doc = "test uint8";
    submodule_data->tables[dnx_data_module_testing_types_table_all].values[1].offset = UTILEX_OFFSETOF(dnx_data_module_testing_types_all_t, uint8val);
    submodule_data->tables[dnx_data_module_testing_types_table_all].values[2].name = "charval";
    submodule_data->tables[dnx_data_module_testing_types_table_all].values[2].type = "char";
    submodule_data->tables[dnx_data_module_testing_types_table_all].values[2].doc = "test char";
    submodule_data->tables[dnx_data_module_testing_types_table_all].values[2].offset = UTILEX_OFFSETOF(dnx_data_module_testing_types_all_t, charval);
    submodule_data->tables[dnx_data_module_testing_types_table_all].values[3].name = "uint16val";
    submodule_data->tables[dnx_data_module_testing_types_table_all].values[3].type = "uint16";
    submodule_data->tables[dnx_data_module_testing_types_table_all].values[3].doc = "test uint16";
    submodule_data->tables[dnx_data_module_testing_types_table_all].values[3].offset = UTILEX_OFFSETOF(dnx_data_module_testing_types_all_t, uint16val);
    submodule_data->tables[dnx_data_module_testing_types_table_all].values[4].name = "enumval";
    submodule_data->tables[dnx_data_module_testing_types_table_all].values[4].type = "bcm_fabric_device_type_t";
    submodule_data->tables[dnx_data_module_testing_types_table_all].values[4].doc = "test enum";
    submodule_data->tables[dnx_data_module_testing_types_table_all].values[4].offset = UTILEX_OFFSETOF(dnx_data_module_testing_types_all_t, enumval);
    submodule_data->tables[dnx_data_module_testing_types_table_all].values[5].name = "arrdefaultall";
    submodule_data->tables[dnx_data_module_testing_types_table_all].values[5].type = "uint32[10]";
    submodule_data->tables[dnx_data_module_testing_types_table_all].values[5].doc = "test array default all";
    submodule_data->tables[dnx_data_module_testing_types_table_all].values[5].offset = UTILEX_OFFSETOF(dnx_data_module_testing_types_all_t, arrdefaultall);
    submodule_data->tables[dnx_data_module_testing_types_table_all].values[6].name = "arrval";
    submodule_data->tables[dnx_data_module_testing_types_table_all].values[6].type = "uint32[10]";
    submodule_data->tables[dnx_data_module_testing_types_table_all].values[6].doc = "test array";
    submodule_data->tables[dnx_data_module_testing_types_table_all].values[6].offset = UTILEX_OFFSETOF(dnx_data_module_testing_types_all_t, arrval);
    submodule_data->tables[dnx_data_module_testing_types_table_all].values[7].name = "pbmpval";
    submodule_data->tables[dnx_data_module_testing_types_table_all].values[7].type = "bcm_pbmp_t";
    submodule_data->tables[dnx_data_module_testing_types_table_all].values[7].doc = "test pbmp";
    submodule_data->tables[dnx_data_module_testing_types_table_all].values[7].offset = UTILEX_OFFSETOF(dnx_data_module_testing_types_all_t, pbmpval);
    submodule_data->tables[dnx_data_module_testing_types_table_all].values[8].name = "strval";
    submodule_data->tables[dnx_data_module_testing_types_table_all].values[8].type = "char *";
    submodule_data->tables[dnx_data_module_testing_types_table_all].values[8].doc = "test string";
    submodule_data->tables[dnx_data_module_testing_types_table_all].values[8].offset = UTILEX_OFFSETOF(dnx_data_module_testing_types_all_t, strval);
    submodule_data->tables[dnx_data_module_testing_types_table_all].values[9].name = "bufferval";
    submodule_data->tables[dnx_data_module_testing_types_table_all].values[9].type = "char[100]";
    submodule_data->tables[dnx_data_module_testing_types_table_all].values[9].doc = "test buffer";
    submodule_data->tables[dnx_data_module_testing_types_table_all].values[9].offset = UTILEX_OFFSETOF(dnx_data_module_testing_types_all_t, bufferval);


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_module_testing_types_feature_get(
    int unit,
    dnx_data_module_testing_types_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_module_testing, dnx_data_module_testing_submodule_types, feature);
}


int
dnx_data_module_testing_types_def_signed_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_module_testing, dnx_data_module_testing_submodule_types, dnx_data_module_testing_types_define_def_signed);
}

uint32
dnx_data_module_testing_types_def_unsigned_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_module_testing, dnx_data_module_testing_submodule_types, dnx_data_module_testing_types_define_def_unsigned);
}

int
dnx_data_module_testing_types_num_signed_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_module_testing, dnx_data_module_testing_submodule_types, dnx_data_module_testing_types_define_num_signed);
}

uint32
dnx_data_module_testing_types_num_unsigned_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_module_testing, dnx_data_module_testing_submodule_types, dnx_data_module_testing_types_define_num_unsigned);
}



const dnx_data_module_testing_types_all_t *
dnx_data_module_testing_types_all_get(
    int unit)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_module_testing, dnx_data_module_testing_submodule_types, dnx_data_module_testing_types_table_all);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
    return (const dnx_data_module_testing_types_all_t *) data;

}


shr_error_e
dnx_data_module_testing_types_all_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_module_testing_types_all_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_module_testing, dnx_data_module_testing_submodule_types, dnx_data_module_testing_types_table_all);
    data = (const dnx_data_module_testing_types_all_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, 0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->intval);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->uint8val);
            break;
        case 2:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->charval);
            break;
        case 3:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->uint16val);
            break;
        case 4:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->enumval);
            break;
        case 5:
            DNXC_DATA_MGMT_ARR_STR(buffer, 10, data->arrdefaultall);
            break;
        case 6:
            DNXC_DATA_MGMT_ARR_STR(buffer, 10, data->arrval);
            break;
        case 7:
            DNXC_DATA_MGMT_PBMP_STR(buffer, data->pbmpval);
            break;
        case 8:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%s", data->strval == NULL ? "" : data->strval);
            break;
        case 9:
            DNXC_DATA_MGMT_ARR_STR(buffer, 100, data->bufferval);
            break;
    }

    SHR_FUNC_EXIT;
}


const dnxc_data_table_info_t *
dnx_data_module_testing_types_all_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_module_testing, dnx_data_module_testing_submodule_types, dnx_data_module_testing_types_table_all);

}






static shr_error_e
dnx_data_module_testing_property_methods_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "property_methods";
    submodule_data->doc = "";
    
    submodule_data->nof_features = _dnx_data_module_testing_property_methods_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data module_testing property_methods features");

    submodule_data->features[dnx_data_module_testing_property_methods_feature_enable].name = "feature_enable";
    submodule_data->features[dnx_data_module_testing_property_methods_feature_enable].doc = "";
    submodule_data->features[dnx_data_module_testing_property_methods_feature_enable].labels[0] = "test";
    submodule_data->features[dnx_data_module_testing_property_methods_feature_enable].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_module_testing_property_methods_feature_disable].name = "feature_disable";
    submodule_data->features[dnx_data_module_testing_property_methods_feature_disable].doc = "";
    submodule_data->features[dnx_data_module_testing_property_methods_feature_disable].flags |= DNXC_DATA_F_FEATURE;

    
    submodule_data->nof_defines = _dnx_data_module_testing_property_methods_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data module_testing property_methods defines");

    submodule_data->defines[dnx_data_module_testing_property_methods_define_numeric_range].name = "numeric_range";
    submodule_data->defines[dnx_data_module_testing_property_methods_define_numeric_range].doc = "";
    submodule_data->defines[dnx_data_module_testing_property_methods_define_numeric_range].labels[0] = "test";
    submodule_data->defines[dnx_data_module_testing_property_methods_define_numeric_range].labels[1] = "try";
    
    submodule_data->defines[dnx_data_module_testing_property_methods_define_numeric_range].flags |= DNXC_DATA_F_NUMERIC;

    
    submodule_data->nof_tables = _dnx_data_module_testing_property_methods_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data module_testing property_methods tables");

    
    submodule_data->tables[dnx_data_module_testing_property_methods_table_enable].name = "enable";
    submodule_data->tables[dnx_data_module_testing_property_methods_table_enable].doc = "";
    submodule_data->tables[dnx_data_module_testing_property_methods_table_enable].labels[0] = "try";
    submodule_data->tables[dnx_data_module_testing_property_methods_table_enable].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_module_testing_property_methods_table_enable].size_of_values = sizeof(dnx_data_module_testing_property_methods_enable_t);
    submodule_data->tables[dnx_data_module_testing_property_methods_table_enable].entry_get = dnx_data_module_testing_property_methods_enable_entry_str_get;

    
    submodule_data->tables[dnx_data_module_testing_property_methods_table_enable].nof_keys = 0;

    
    submodule_data->tables[dnx_data_module_testing_property_methods_table_enable].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_module_testing_property_methods_table_enable].values, dnxc_data_value_t, submodule_data->tables[dnx_data_module_testing_property_methods_table_enable].nof_values, "_dnx_data_module_testing_property_methods_table_enable table values");
    submodule_data->tables[dnx_data_module_testing_property_methods_table_enable].values[0].name = "val";
    submodule_data->tables[dnx_data_module_testing_property_methods_table_enable].values[0].type = "uint32";
    submodule_data->tables[dnx_data_module_testing_property_methods_table_enable].values[0].doc = "test enable method";
    submodule_data->tables[dnx_data_module_testing_property_methods_table_enable].values[0].offset = UTILEX_OFFSETOF(dnx_data_module_testing_property_methods_enable_t, val);

    
    submodule_data->tables[dnx_data_module_testing_property_methods_table_port_enable].name = "port_enable";
    submodule_data->tables[dnx_data_module_testing_property_methods_table_port_enable].doc = "";
    submodule_data->tables[dnx_data_module_testing_property_methods_table_port_enable].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_module_testing_property_methods_table_port_enable].size_of_values = sizeof(dnx_data_module_testing_property_methods_port_enable_t);
    submodule_data->tables[dnx_data_module_testing_property_methods_table_port_enable].entry_get = dnx_data_module_testing_property_methods_port_enable_entry_str_get;

    
    submodule_data->tables[dnx_data_module_testing_property_methods_table_port_enable].nof_keys = 1;
    submodule_data->tables[dnx_data_module_testing_property_methods_table_port_enable].keys[0].name = "port";
    submodule_data->tables[dnx_data_module_testing_property_methods_table_port_enable].keys[0].doc = "";

    
    submodule_data->tables[dnx_data_module_testing_property_methods_table_port_enable].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_module_testing_property_methods_table_port_enable].values, dnxc_data_value_t, submodule_data->tables[dnx_data_module_testing_property_methods_table_port_enable].nof_values, "_dnx_data_module_testing_property_methods_table_port_enable table values");
    submodule_data->tables[dnx_data_module_testing_property_methods_table_port_enable].values[0].name = "val";
    submodule_data->tables[dnx_data_module_testing_property_methods_table_port_enable].values[0].type = "uint32";
    submodule_data->tables[dnx_data_module_testing_property_methods_table_port_enable].values[0].doc = "test port enable method";
    submodule_data->tables[dnx_data_module_testing_property_methods_table_port_enable].values[0].offset = UTILEX_OFFSETOF(dnx_data_module_testing_property_methods_port_enable_t, val);

    
    submodule_data->tables[dnx_data_module_testing_property_methods_table_suffix_enable].name = "suffix_enable";
    submodule_data->tables[dnx_data_module_testing_property_methods_table_suffix_enable].doc = "";
    submodule_data->tables[dnx_data_module_testing_property_methods_table_suffix_enable].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_module_testing_property_methods_table_suffix_enable].size_of_values = sizeof(dnx_data_module_testing_property_methods_suffix_enable_t);
    submodule_data->tables[dnx_data_module_testing_property_methods_table_suffix_enable].entry_get = dnx_data_module_testing_property_methods_suffix_enable_entry_str_get;

    
    submodule_data->tables[dnx_data_module_testing_property_methods_table_suffix_enable].nof_keys = 1;
    submodule_data->tables[dnx_data_module_testing_property_methods_table_suffix_enable].keys[0].name = "link";
    submodule_data->tables[dnx_data_module_testing_property_methods_table_suffix_enable].keys[0].doc = "";

    
    submodule_data->tables[dnx_data_module_testing_property_methods_table_suffix_enable].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_module_testing_property_methods_table_suffix_enable].values, dnxc_data_value_t, submodule_data->tables[dnx_data_module_testing_property_methods_table_suffix_enable].nof_values, "_dnx_data_module_testing_property_methods_table_suffix_enable table values");
    submodule_data->tables[dnx_data_module_testing_property_methods_table_suffix_enable].values[0].name = "val";
    submodule_data->tables[dnx_data_module_testing_property_methods_table_suffix_enable].values[0].type = "uint32";
    submodule_data->tables[dnx_data_module_testing_property_methods_table_suffix_enable].values[0].doc = "test suffix enable method";
    submodule_data->tables[dnx_data_module_testing_property_methods_table_suffix_enable].values[0].offset = UTILEX_OFFSETOF(dnx_data_module_testing_property_methods_suffix_enable_t, val);

    
    submodule_data->tables[dnx_data_module_testing_property_methods_table_range].name = "range";
    submodule_data->tables[dnx_data_module_testing_property_methods_table_range].doc = "";
    submodule_data->tables[dnx_data_module_testing_property_methods_table_range].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_module_testing_property_methods_table_range].size_of_values = sizeof(dnx_data_module_testing_property_methods_range_t);
    submodule_data->tables[dnx_data_module_testing_property_methods_table_range].entry_get = dnx_data_module_testing_property_methods_range_entry_str_get;

    
    submodule_data->tables[dnx_data_module_testing_property_methods_table_range].nof_keys = 0;

    
    submodule_data->tables[dnx_data_module_testing_property_methods_table_range].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_module_testing_property_methods_table_range].values, dnxc_data_value_t, submodule_data->tables[dnx_data_module_testing_property_methods_table_range].nof_values, "_dnx_data_module_testing_property_methods_table_range table values");
    submodule_data->tables[dnx_data_module_testing_property_methods_table_range].values[0].name = "val";
    submodule_data->tables[dnx_data_module_testing_property_methods_table_range].values[0].type = "uint32";
    submodule_data->tables[dnx_data_module_testing_property_methods_table_range].values[0].doc = "see table";
    submodule_data->tables[dnx_data_module_testing_property_methods_table_range].values[0].offset = UTILEX_OFFSETOF(dnx_data_module_testing_property_methods_range_t, val);

    
    submodule_data->tables[dnx_data_module_testing_property_methods_table_port_range].name = "port_range";
    submodule_data->tables[dnx_data_module_testing_property_methods_table_port_range].doc = "";
    submodule_data->tables[dnx_data_module_testing_property_methods_table_port_range].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_module_testing_property_methods_table_port_range].size_of_values = sizeof(dnx_data_module_testing_property_methods_port_range_t);
    submodule_data->tables[dnx_data_module_testing_property_methods_table_port_range].entry_get = dnx_data_module_testing_property_methods_port_range_entry_str_get;

    
    submodule_data->tables[dnx_data_module_testing_property_methods_table_port_range].nof_keys = 1;
    submodule_data->tables[dnx_data_module_testing_property_methods_table_port_range].keys[0].name = "port";
    submodule_data->tables[dnx_data_module_testing_property_methods_table_port_range].keys[0].doc = "";

    
    submodule_data->tables[dnx_data_module_testing_property_methods_table_port_range].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_module_testing_property_methods_table_port_range].values, dnxc_data_value_t, submodule_data->tables[dnx_data_module_testing_property_methods_table_port_range].nof_values, "_dnx_data_module_testing_property_methods_table_port_range table values");
    submodule_data->tables[dnx_data_module_testing_property_methods_table_port_range].values[0].name = "val";
    submodule_data->tables[dnx_data_module_testing_property_methods_table_port_range].values[0].type = "uint32";
    submodule_data->tables[dnx_data_module_testing_property_methods_table_port_range].values[0].doc = "see table";
    submodule_data->tables[dnx_data_module_testing_property_methods_table_port_range].values[0].offset = UTILEX_OFFSETOF(dnx_data_module_testing_property_methods_port_range_t, val);

    
    submodule_data->tables[dnx_data_module_testing_property_methods_table_suffix_range].name = "suffix_range";
    submodule_data->tables[dnx_data_module_testing_property_methods_table_suffix_range].doc = "";
    submodule_data->tables[dnx_data_module_testing_property_methods_table_suffix_range].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_module_testing_property_methods_table_suffix_range].size_of_values = sizeof(dnx_data_module_testing_property_methods_suffix_range_t);
    submodule_data->tables[dnx_data_module_testing_property_methods_table_suffix_range].entry_get = dnx_data_module_testing_property_methods_suffix_range_entry_str_get;

    
    submodule_data->tables[dnx_data_module_testing_property_methods_table_suffix_range].nof_keys = 1;
    submodule_data->tables[dnx_data_module_testing_property_methods_table_suffix_range].keys[0].name = "link";
    submodule_data->tables[dnx_data_module_testing_property_methods_table_suffix_range].keys[0].doc = "";

    
    submodule_data->tables[dnx_data_module_testing_property_methods_table_suffix_range].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_module_testing_property_methods_table_suffix_range].values, dnxc_data_value_t, submodule_data->tables[dnx_data_module_testing_property_methods_table_suffix_range].nof_values, "_dnx_data_module_testing_property_methods_table_suffix_range table values");
    submodule_data->tables[dnx_data_module_testing_property_methods_table_suffix_range].values[0].name = "val";
    submodule_data->tables[dnx_data_module_testing_property_methods_table_suffix_range].values[0].type = "uint32";
    submodule_data->tables[dnx_data_module_testing_property_methods_table_suffix_range].values[0].doc = "see table";
    submodule_data->tables[dnx_data_module_testing_property_methods_table_suffix_range].values[0].offset = UTILEX_OFFSETOF(dnx_data_module_testing_property_methods_suffix_range_t, val);

    
    submodule_data->tables[dnx_data_module_testing_property_methods_table_direct_map].name = "direct_map";
    submodule_data->tables[dnx_data_module_testing_property_methods_table_direct_map].doc = "";
    submodule_data->tables[dnx_data_module_testing_property_methods_table_direct_map].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_module_testing_property_methods_table_direct_map].size_of_values = sizeof(dnx_data_module_testing_property_methods_direct_map_t);
    submodule_data->tables[dnx_data_module_testing_property_methods_table_direct_map].entry_get = dnx_data_module_testing_property_methods_direct_map_entry_str_get;

    
    submodule_data->tables[dnx_data_module_testing_property_methods_table_direct_map].nof_keys = 0;

    
    submodule_data->tables[dnx_data_module_testing_property_methods_table_direct_map].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_module_testing_property_methods_table_direct_map].values, dnxc_data_value_t, submodule_data->tables[dnx_data_module_testing_property_methods_table_direct_map].nof_values, "_dnx_data_module_testing_property_methods_table_direct_map table values");
    submodule_data->tables[dnx_data_module_testing_property_methods_table_direct_map].values[0].name = "val";
    submodule_data->tables[dnx_data_module_testing_property_methods_table_direct_map].values[0].type = "uint32";
    submodule_data->tables[dnx_data_module_testing_property_methods_table_direct_map].values[0].doc = "see table";
    submodule_data->tables[dnx_data_module_testing_property_methods_table_direct_map].values[0].offset = UTILEX_OFFSETOF(dnx_data_module_testing_property_methods_direct_map_t, val);

    
    submodule_data->tables[dnx_data_module_testing_property_methods_table_port_direct_map].name = "port_direct_map";
    submodule_data->tables[dnx_data_module_testing_property_methods_table_port_direct_map].doc = "";
    submodule_data->tables[dnx_data_module_testing_property_methods_table_port_direct_map].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_module_testing_property_methods_table_port_direct_map].size_of_values = sizeof(dnx_data_module_testing_property_methods_port_direct_map_t);
    submodule_data->tables[dnx_data_module_testing_property_methods_table_port_direct_map].entry_get = dnx_data_module_testing_property_methods_port_direct_map_entry_str_get;

    
    submodule_data->tables[dnx_data_module_testing_property_methods_table_port_direct_map].nof_keys = 1;
    submodule_data->tables[dnx_data_module_testing_property_methods_table_port_direct_map].keys[0].name = "port";
    submodule_data->tables[dnx_data_module_testing_property_methods_table_port_direct_map].keys[0].doc = "";

    
    submodule_data->tables[dnx_data_module_testing_property_methods_table_port_direct_map].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_module_testing_property_methods_table_port_direct_map].values, dnxc_data_value_t, submodule_data->tables[dnx_data_module_testing_property_methods_table_port_direct_map].nof_values, "_dnx_data_module_testing_property_methods_table_port_direct_map table values");
    submodule_data->tables[dnx_data_module_testing_property_methods_table_port_direct_map].values[0].name = "val";
    submodule_data->tables[dnx_data_module_testing_property_methods_table_port_direct_map].values[0].type = "uint32";
    submodule_data->tables[dnx_data_module_testing_property_methods_table_port_direct_map].values[0].doc = "see table";
    submodule_data->tables[dnx_data_module_testing_property_methods_table_port_direct_map].values[0].offset = UTILEX_OFFSETOF(dnx_data_module_testing_property_methods_port_direct_map_t, val);

    
    submodule_data->tables[dnx_data_module_testing_property_methods_table_suffix_direct_map].name = "suffix_direct_map";
    submodule_data->tables[dnx_data_module_testing_property_methods_table_suffix_direct_map].doc = "";
    submodule_data->tables[dnx_data_module_testing_property_methods_table_suffix_direct_map].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_module_testing_property_methods_table_suffix_direct_map].size_of_values = sizeof(dnx_data_module_testing_property_methods_suffix_direct_map_t);
    submodule_data->tables[dnx_data_module_testing_property_methods_table_suffix_direct_map].entry_get = dnx_data_module_testing_property_methods_suffix_direct_map_entry_str_get;

    
    submodule_data->tables[dnx_data_module_testing_property_methods_table_suffix_direct_map].nof_keys = 1;
    submodule_data->tables[dnx_data_module_testing_property_methods_table_suffix_direct_map].keys[0].name = "link";
    submodule_data->tables[dnx_data_module_testing_property_methods_table_suffix_direct_map].keys[0].doc = "";

    
    submodule_data->tables[dnx_data_module_testing_property_methods_table_suffix_direct_map].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_module_testing_property_methods_table_suffix_direct_map].values, dnxc_data_value_t, submodule_data->tables[dnx_data_module_testing_property_methods_table_suffix_direct_map].nof_values, "_dnx_data_module_testing_property_methods_table_suffix_direct_map table values");
    submodule_data->tables[dnx_data_module_testing_property_methods_table_suffix_direct_map].values[0].name = "val";
    submodule_data->tables[dnx_data_module_testing_property_methods_table_suffix_direct_map].values[0].type = "uint32";
    submodule_data->tables[dnx_data_module_testing_property_methods_table_suffix_direct_map].values[0].doc = "see table";
    submodule_data->tables[dnx_data_module_testing_property_methods_table_suffix_direct_map].values[0].offset = UTILEX_OFFSETOF(dnx_data_module_testing_property_methods_suffix_direct_map_t, val);

    
    submodule_data->tables[dnx_data_module_testing_property_methods_table_custom].name = "custom";
    submodule_data->tables[dnx_data_module_testing_property_methods_table_custom].doc = "";
    submodule_data->tables[dnx_data_module_testing_property_methods_table_custom].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_module_testing_property_methods_table_custom].size_of_values = sizeof(dnx_data_module_testing_property_methods_custom_t);
    submodule_data->tables[dnx_data_module_testing_property_methods_table_custom].entry_get = dnx_data_module_testing_property_methods_custom_entry_str_get;

    
    submodule_data->tables[dnx_data_module_testing_property_methods_table_custom].nof_keys = 2;
    submodule_data->tables[dnx_data_module_testing_property_methods_table_custom].keys[0].name = "link";
    submodule_data->tables[dnx_data_module_testing_property_methods_table_custom].keys[0].doc = "";
    submodule_data->tables[dnx_data_module_testing_property_methods_table_custom].keys[1].name = "pipe";
    submodule_data->tables[dnx_data_module_testing_property_methods_table_custom].keys[1].doc = "";

    
    submodule_data->tables[dnx_data_module_testing_property_methods_table_custom].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_module_testing_property_methods_table_custom].values, dnxc_data_value_t, submodule_data->tables[dnx_data_module_testing_property_methods_table_custom].nof_values, "_dnx_data_module_testing_property_methods_table_custom table values");
    submodule_data->tables[dnx_data_module_testing_property_methods_table_custom].values[0].name = "val";
    submodule_data->tables[dnx_data_module_testing_property_methods_table_custom].values[0].type = "uint32";
    submodule_data->tables[dnx_data_module_testing_property_methods_table_custom].values[0].doc = "see table";
    submodule_data->tables[dnx_data_module_testing_property_methods_table_custom].values[0].offset = UTILEX_OFFSETOF(dnx_data_module_testing_property_methods_custom_t, val);

    
    submodule_data->tables[dnx_data_module_testing_property_methods_table_pbmp].name = "pbmp";
    submodule_data->tables[dnx_data_module_testing_property_methods_table_pbmp].doc = "";
    submodule_data->tables[dnx_data_module_testing_property_methods_table_pbmp].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_module_testing_property_methods_table_pbmp].size_of_values = sizeof(dnx_data_module_testing_property_methods_pbmp_t);
    submodule_data->tables[dnx_data_module_testing_property_methods_table_pbmp].entry_get = dnx_data_module_testing_property_methods_pbmp_entry_str_get;

    
    submodule_data->tables[dnx_data_module_testing_property_methods_table_pbmp].nof_keys = 0;

    
    submodule_data->tables[dnx_data_module_testing_property_methods_table_pbmp].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_module_testing_property_methods_table_pbmp].values, dnxc_data_value_t, submodule_data->tables[dnx_data_module_testing_property_methods_table_pbmp].nof_values, "_dnx_data_module_testing_property_methods_table_pbmp table values");
    submodule_data->tables[dnx_data_module_testing_property_methods_table_pbmp].values[0].name = "val";
    submodule_data->tables[dnx_data_module_testing_property_methods_table_pbmp].values[0].type = "bcm_pbmp_t";
    submodule_data->tables[dnx_data_module_testing_property_methods_table_pbmp].values[0].doc = "see table";
    submodule_data->tables[dnx_data_module_testing_property_methods_table_pbmp].values[0].offset = UTILEX_OFFSETOF(dnx_data_module_testing_property_methods_pbmp_t, val);

    
    submodule_data->tables[dnx_data_module_testing_property_methods_table_port_pbmp].name = "port_pbmp";
    submodule_data->tables[dnx_data_module_testing_property_methods_table_port_pbmp].doc = "";
    submodule_data->tables[dnx_data_module_testing_property_methods_table_port_pbmp].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_module_testing_property_methods_table_port_pbmp].size_of_values = sizeof(dnx_data_module_testing_property_methods_port_pbmp_t);
    submodule_data->tables[dnx_data_module_testing_property_methods_table_port_pbmp].entry_get = dnx_data_module_testing_property_methods_port_pbmp_entry_str_get;

    
    submodule_data->tables[dnx_data_module_testing_property_methods_table_port_pbmp].nof_keys = 1;
    submodule_data->tables[dnx_data_module_testing_property_methods_table_port_pbmp].keys[0].name = "port";
    submodule_data->tables[dnx_data_module_testing_property_methods_table_port_pbmp].keys[0].doc = "";

    
    submodule_data->tables[dnx_data_module_testing_property_methods_table_port_pbmp].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_module_testing_property_methods_table_port_pbmp].values, dnxc_data_value_t, submodule_data->tables[dnx_data_module_testing_property_methods_table_port_pbmp].nof_values, "_dnx_data_module_testing_property_methods_table_port_pbmp table values");
    submodule_data->tables[dnx_data_module_testing_property_methods_table_port_pbmp].values[0].name = "val";
    submodule_data->tables[dnx_data_module_testing_property_methods_table_port_pbmp].values[0].type = "bcm_pbmp_t";
    submodule_data->tables[dnx_data_module_testing_property_methods_table_port_pbmp].values[0].doc = "see table";
    submodule_data->tables[dnx_data_module_testing_property_methods_table_port_pbmp].values[0].offset = UTILEX_OFFSETOF(dnx_data_module_testing_property_methods_port_pbmp_t, val);

    
    submodule_data->tables[dnx_data_module_testing_property_methods_table_suffix_pbmp].name = "suffix_pbmp";
    submodule_data->tables[dnx_data_module_testing_property_methods_table_suffix_pbmp].doc = "";
    submodule_data->tables[dnx_data_module_testing_property_methods_table_suffix_pbmp].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_module_testing_property_methods_table_suffix_pbmp].size_of_values = sizeof(dnx_data_module_testing_property_methods_suffix_pbmp_t);
    submodule_data->tables[dnx_data_module_testing_property_methods_table_suffix_pbmp].entry_get = dnx_data_module_testing_property_methods_suffix_pbmp_entry_str_get;

    
    submodule_data->tables[dnx_data_module_testing_property_methods_table_suffix_pbmp].nof_keys = 1;
    submodule_data->tables[dnx_data_module_testing_property_methods_table_suffix_pbmp].keys[0].name = "index";
    submodule_data->tables[dnx_data_module_testing_property_methods_table_suffix_pbmp].keys[0].doc = "";

    
    submodule_data->tables[dnx_data_module_testing_property_methods_table_suffix_pbmp].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_module_testing_property_methods_table_suffix_pbmp].values, dnxc_data_value_t, submodule_data->tables[dnx_data_module_testing_property_methods_table_suffix_pbmp].nof_values, "_dnx_data_module_testing_property_methods_table_suffix_pbmp table values");
    submodule_data->tables[dnx_data_module_testing_property_methods_table_suffix_pbmp].values[0].name = "val";
    submodule_data->tables[dnx_data_module_testing_property_methods_table_suffix_pbmp].values[0].type = "bcm_pbmp_t";
    submodule_data->tables[dnx_data_module_testing_property_methods_table_suffix_pbmp].values[0].doc = "see table";
    submodule_data->tables[dnx_data_module_testing_property_methods_table_suffix_pbmp].values[0].offset = UTILEX_OFFSETOF(dnx_data_module_testing_property_methods_suffix_pbmp_t, val);

    
    submodule_data->tables[dnx_data_module_testing_property_methods_table_str].name = "str";
    submodule_data->tables[dnx_data_module_testing_property_methods_table_str].doc = "";
    submodule_data->tables[dnx_data_module_testing_property_methods_table_str].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_module_testing_property_methods_table_str].size_of_values = sizeof(dnx_data_module_testing_property_methods_str_t);
    submodule_data->tables[dnx_data_module_testing_property_methods_table_str].entry_get = dnx_data_module_testing_property_methods_str_entry_str_get;

    
    submodule_data->tables[dnx_data_module_testing_property_methods_table_str].nof_keys = 0;

    
    submodule_data->tables[dnx_data_module_testing_property_methods_table_str].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_module_testing_property_methods_table_str].values, dnxc_data_value_t, submodule_data->tables[dnx_data_module_testing_property_methods_table_str].nof_values, "_dnx_data_module_testing_property_methods_table_str table values");
    submodule_data->tables[dnx_data_module_testing_property_methods_table_str].values[0].name = "val";
    submodule_data->tables[dnx_data_module_testing_property_methods_table_str].values[0].type = "char *";
    submodule_data->tables[dnx_data_module_testing_property_methods_table_str].values[0].doc = "see table";
    submodule_data->tables[dnx_data_module_testing_property_methods_table_str].values[0].offset = UTILEX_OFFSETOF(dnx_data_module_testing_property_methods_str_t, val);

    
    submodule_data->tables[dnx_data_module_testing_property_methods_table_port_str].name = "port_str";
    submodule_data->tables[dnx_data_module_testing_property_methods_table_port_str].doc = "";
    submodule_data->tables[dnx_data_module_testing_property_methods_table_port_str].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_module_testing_property_methods_table_port_str].size_of_values = sizeof(dnx_data_module_testing_property_methods_port_str_t);
    submodule_data->tables[dnx_data_module_testing_property_methods_table_port_str].entry_get = dnx_data_module_testing_property_methods_port_str_entry_str_get;

    
    submodule_data->tables[dnx_data_module_testing_property_methods_table_port_str].nof_keys = 1;
    submodule_data->tables[dnx_data_module_testing_property_methods_table_port_str].keys[0].name = "port";
    submodule_data->tables[dnx_data_module_testing_property_methods_table_port_str].keys[0].doc = "";

    
    submodule_data->tables[dnx_data_module_testing_property_methods_table_port_str].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_module_testing_property_methods_table_port_str].values, dnxc_data_value_t, submodule_data->tables[dnx_data_module_testing_property_methods_table_port_str].nof_values, "_dnx_data_module_testing_property_methods_table_port_str table values");
    submodule_data->tables[dnx_data_module_testing_property_methods_table_port_str].values[0].name = "val";
    submodule_data->tables[dnx_data_module_testing_property_methods_table_port_str].values[0].type = "char *";
    submodule_data->tables[dnx_data_module_testing_property_methods_table_port_str].values[0].doc = "see table";
    submodule_data->tables[dnx_data_module_testing_property_methods_table_port_str].values[0].offset = UTILEX_OFFSETOF(dnx_data_module_testing_property_methods_port_str_t, val);

    
    submodule_data->tables[dnx_data_module_testing_property_methods_table_suffix_str].name = "suffix_str";
    submodule_data->tables[dnx_data_module_testing_property_methods_table_suffix_str].doc = "";
    submodule_data->tables[dnx_data_module_testing_property_methods_table_suffix_str].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_module_testing_property_methods_table_suffix_str].size_of_values = sizeof(dnx_data_module_testing_property_methods_suffix_str_t);
    submodule_data->tables[dnx_data_module_testing_property_methods_table_suffix_str].entry_get = dnx_data_module_testing_property_methods_suffix_str_entry_str_get;

    
    submodule_data->tables[dnx_data_module_testing_property_methods_table_suffix_str].nof_keys = 1;
    submodule_data->tables[dnx_data_module_testing_property_methods_table_suffix_str].keys[0].name = "index";
    submodule_data->tables[dnx_data_module_testing_property_methods_table_suffix_str].keys[0].doc = "";

    
    submodule_data->tables[dnx_data_module_testing_property_methods_table_suffix_str].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_module_testing_property_methods_table_suffix_str].values, dnxc_data_value_t, submodule_data->tables[dnx_data_module_testing_property_methods_table_suffix_str].nof_values, "_dnx_data_module_testing_property_methods_table_suffix_str table values");
    submodule_data->tables[dnx_data_module_testing_property_methods_table_suffix_str].values[0].name = "val";
    submodule_data->tables[dnx_data_module_testing_property_methods_table_suffix_str].values[0].type = "char *";
    submodule_data->tables[dnx_data_module_testing_property_methods_table_suffix_str].values[0].doc = "see table";
    submodule_data->tables[dnx_data_module_testing_property_methods_table_suffix_str].values[0].offset = UTILEX_OFFSETOF(dnx_data_module_testing_property_methods_suffix_str_t, val);


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_module_testing_property_methods_feature_get(
    int unit,
    dnx_data_module_testing_property_methods_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_module_testing, dnx_data_module_testing_submodule_property_methods, feature);
}


uint32
dnx_data_module_testing_property_methods_numeric_range_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_module_testing, dnx_data_module_testing_submodule_property_methods, dnx_data_module_testing_property_methods_define_numeric_range);
}



const dnx_data_module_testing_property_methods_enable_t *
dnx_data_module_testing_property_methods_enable_get(
    int unit)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_module_testing, dnx_data_module_testing_submodule_property_methods, dnx_data_module_testing_property_methods_table_enable);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
    return (const dnx_data_module_testing_property_methods_enable_t *) data;

}

const dnx_data_module_testing_property_methods_port_enable_t *
dnx_data_module_testing_property_methods_port_enable_get(
    int unit,
    int port)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_module_testing, dnx_data_module_testing_submodule_property_methods, dnx_data_module_testing_property_methods_table_port_enable);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, port, 0);
    return (const dnx_data_module_testing_property_methods_port_enable_t *) data;

}

const dnx_data_module_testing_property_methods_suffix_enable_t *
dnx_data_module_testing_property_methods_suffix_enable_get(
    int unit,
    int link)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_module_testing, dnx_data_module_testing_submodule_property_methods, dnx_data_module_testing_property_methods_table_suffix_enable);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, link, 0);
    return (const dnx_data_module_testing_property_methods_suffix_enable_t *) data;

}

const dnx_data_module_testing_property_methods_range_t *
dnx_data_module_testing_property_methods_range_get(
    int unit)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_module_testing, dnx_data_module_testing_submodule_property_methods, dnx_data_module_testing_property_methods_table_range);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
    return (const dnx_data_module_testing_property_methods_range_t *) data;

}

const dnx_data_module_testing_property_methods_port_range_t *
dnx_data_module_testing_property_methods_port_range_get(
    int unit,
    int port)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_module_testing, dnx_data_module_testing_submodule_property_methods, dnx_data_module_testing_property_methods_table_port_range);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, port, 0);
    return (const dnx_data_module_testing_property_methods_port_range_t *) data;

}

const dnx_data_module_testing_property_methods_suffix_range_t *
dnx_data_module_testing_property_methods_suffix_range_get(
    int unit,
    int link)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_module_testing, dnx_data_module_testing_submodule_property_methods, dnx_data_module_testing_property_methods_table_suffix_range);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, link, 0);
    return (const dnx_data_module_testing_property_methods_suffix_range_t *) data;

}

const dnx_data_module_testing_property_methods_direct_map_t *
dnx_data_module_testing_property_methods_direct_map_get(
    int unit)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_module_testing, dnx_data_module_testing_submodule_property_methods, dnx_data_module_testing_property_methods_table_direct_map);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
    return (const dnx_data_module_testing_property_methods_direct_map_t *) data;

}

const dnx_data_module_testing_property_methods_port_direct_map_t *
dnx_data_module_testing_property_methods_port_direct_map_get(
    int unit,
    int port)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_module_testing, dnx_data_module_testing_submodule_property_methods, dnx_data_module_testing_property_methods_table_port_direct_map);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, port, 0);
    return (const dnx_data_module_testing_property_methods_port_direct_map_t *) data;

}

const dnx_data_module_testing_property_methods_suffix_direct_map_t *
dnx_data_module_testing_property_methods_suffix_direct_map_get(
    int unit,
    int link)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_module_testing, dnx_data_module_testing_submodule_property_methods, dnx_data_module_testing_property_methods_table_suffix_direct_map);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, link, 0);
    return (const dnx_data_module_testing_property_methods_suffix_direct_map_t *) data;

}

const dnx_data_module_testing_property_methods_custom_t *
dnx_data_module_testing_property_methods_custom_get(
    int unit,
    int link,
    int pipe)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_module_testing, dnx_data_module_testing_submodule_property_methods, dnx_data_module_testing_property_methods_table_custom);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, link, pipe);
    return (const dnx_data_module_testing_property_methods_custom_t *) data;

}

const dnx_data_module_testing_property_methods_pbmp_t *
dnx_data_module_testing_property_methods_pbmp_get(
    int unit)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_module_testing, dnx_data_module_testing_submodule_property_methods, dnx_data_module_testing_property_methods_table_pbmp);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
    return (const dnx_data_module_testing_property_methods_pbmp_t *) data;

}

const dnx_data_module_testing_property_methods_port_pbmp_t *
dnx_data_module_testing_property_methods_port_pbmp_get(
    int unit,
    int port)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_module_testing, dnx_data_module_testing_submodule_property_methods, dnx_data_module_testing_property_methods_table_port_pbmp);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, port, 0);
    return (const dnx_data_module_testing_property_methods_port_pbmp_t *) data;

}

const dnx_data_module_testing_property_methods_suffix_pbmp_t *
dnx_data_module_testing_property_methods_suffix_pbmp_get(
    int unit,
    int index)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_module_testing, dnx_data_module_testing_submodule_property_methods, dnx_data_module_testing_property_methods_table_suffix_pbmp);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, index, 0);
    return (const dnx_data_module_testing_property_methods_suffix_pbmp_t *) data;

}

const dnx_data_module_testing_property_methods_str_t *
dnx_data_module_testing_property_methods_str_get(
    int unit)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_module_testing, dnx_data_module_testing_submodule_property_methods, dnx_data_module_testing_property_methods_table_str);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
    return (const dnx_data_module_testing_property_methods_str_t *) data;

}

const dnx_data_module_testing_property_methods_port_str_t *
dnx_data_module_testing_property_methods_port_str_get(
    int unit,
    int port)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_module_testing, dnx_data_module_testing_submodule_property_methods, dnx_data_module_testing_property_methods_table_port_str);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, port, 0);
    return (const dnx_data_module_testing_property_methods_port_str_t *) data;

}

const dnx_data_module_testing_property_methods_suffix_str_t *
dnx_data_module_testing_property_methods_suffix_str_get(
    int unit,
    int index)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_module_testing, dnx_data_module_testing_submodule_property_methods, dnx_data_module_testing_property_methods_table_suffix_str);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, index, 0);
    return (const dnx_data_module_testing_property_methods_suffix_str_t *) data;

}


shr_error_e
dnx_data_module_testing_property_methods_enable_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_module_testing_property_methods_enable_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_module_testing, dnx_data_module_testing_submodule_property_methods, dnx_data_module_testing_property_methods_table_enable);
    data = (const dnx_data_module_testing_property_methods_enable_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, 0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->val);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_module_testing_property_methods_port_enable_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_module_testing_property_methods_port_enable_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_module_testing, dnx_data_module_testing_submodule_property_methods, dnx_data_module_testing_property_methods_table_port_enable);
    data = (const dnx_data_module_testing_property_methods_port_enable_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->val);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_module_testing_property_methods_suffix_enable_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_module_testing_property_methods_suffix_enable_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_module_testing, dnx_data_module_testing_submodule_property_methods, dnx_data_module_testing_property_methods_table_suffix_enable);
    data = (const dnx_data_module_testing_property_methods_suffix_enable_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->val);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_module_testing_property_methods_range_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_module_testing_property_methods_range_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_module_testing, dnx_data_module_testing_submodule_property_methods, dnx_data_module_testing_property_methods_table_range);
    data = (const dnx_data_module_testing_property_methods_range_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, 0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->val);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_module_testing_property_methods_port_range_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_module_testing_property_methods_port_range_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_module_testing, dnx_data_module_testing_submodule_property_methods, dnx_data_module_testing_property_methods_table_port_range);
    data = (const dnx_data_module_testing_property_methods_port_range_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->val);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_module_testing_property_methods_suffix_range_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_module_testing_property_methods_suffix_range_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_module_testing, dnx_data_module_testing_submodule_property_methods, dnx_data_module_testing_property_methods_table_suffix_range);
    data = (const dnx_data_module_testing_property_methods_suffix_range_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->val);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_module_testing_property_methods_direct_map_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_module_testing_property_methods_direct_map_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_module_testing, dnx_data_module_testing_submodule_property_methods, dnx_data_module_testing_property_methods_table_direct_map);
    data = (const dnx_data_module_testing_property_methods_direct_map_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, 0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->val);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_module_testing_property_methods_port_direct_map_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_module_testing_property_methods_port_direct_map_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_module_testing, dnx_data_module_testing_submodule_property_methods, dnx_data_module_testing_property_methods_table_port_direct_map);
    data = (const dnx_data_module_testing_property_methods_port_direct_map_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->val);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_module_testing_property_methods_suffix_direct_map_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_module_testing_property_methods_suffix_direct_map_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_module_testing, dnx_data_module_testing_submodule_property_methods, dnx_data_module_testing_property_methods_table_suffix_direct_map);
    data = (const dnx_data_module_testing_property_methods_suffix_direct_map_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->val);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_module_testing_property_methods_custom_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_module_testing_property_methods_custom_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_module_testing, dnx_data_module_testing_submodule_property_methods, dnx_data_module_testing_property_methods_table_custom);
    data = (const dnx_data_module_testing_property_methods_custom_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, key1);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->val);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_module_testing_property_methods_pbmp_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_module_testing_property_methods_pbmp_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_module_testing, dnx_data_module_testing_submodule_property_methods, dnx_data_module_testing_property_methods_table_pbmp);
    data = (const dnx_data_module_testing_property_methods_pbmp_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, 0, 0);
    switch (value_index)
    {
        case 0:
            DNXC_DATA_MGMT_PBMP_STR(buffer, data->val);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_module_testing_property_methods_port_pbmp_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_module_testing_property_methods_port_pbmp_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_module_testing, dnx_data_module_testing_submodule_property_methods, dnx_data_module_testing_property_methods_table_port_pbmp);
    data = (const dnx_data_module_testing_property_methods_port_pbmp_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            DNXC_DATA_MGMT_PBMP_STR(buffer, data->val);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_module_testing_property_methods_suffix_pbmp_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_module_testing_property_methods_suffix_pbmp_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_module_testing, dnx_data_module_testing_submodule_property_methods, dnx_data_module_testing_property_methods_table_suffix_pbmp);
    data = (const dnx_data_module_testing_property_methods_suffix_pbmp_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            DNXC_DATA_MGMT_PBMP_STR(buffer, data->val);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_module_testing_property_methods_str_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_module_testing_property_methods_str_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_module_testing, dnx_data_module_testing_submodule_property_methods, dnx_data_module_testing_property_methods_table_str);
    data = (const dnx_data_module_testing_property_methods_str_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, 0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%s", data->val == NULL ? "" : data->val);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_module_testing_property_methods_port_str_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_module_testing_property_methods_port_str_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_module_testing, dnx_data_module_testing_submodule_property_methods, dnx_data_module_testing_property_methods_table_port_str);
    data = (const dnx_data_module_testing_property_methods_port_str_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%s", data->val == NULL ? "" : data->val);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_module_testing_property_methods_suffix_str_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_module_testing_property_methods_suffix_str_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_module_testing, dnx_data_module_testing_submodule_property_methods, dnx_data_module_testing_property_methods_table_suffix_str);
    data = (const dnx_data_module_testing_property_methods_suffix_str_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%s", data->val == NULL ? "" : data->val);
            break;
    }

    SHR_FUNC_EXIT;
}


const dnxc_data_table_info_t *
dnx_data_module_testing_property_methods_enable_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_module_testing, dnx_data_module_testing_submodule_property_methods, dnx_data_module_testing_property_methods_table_enable);

}

const dnxc_data_table_info_t *
dnx_data_module_testing_property_methods_port_enable_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_module_testing, dnx_data_module_testing_submodule_property_methods, dnx_data_module_testing_property_methods_table_port_enable);

}

const dnxc_data_table_info_t *
dnx_data_module_testing_property_methods_suffix_enable_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_module_testing, dnx_data_module_testing_submodule_property_methods, dnx_data_module_testing_property_methods_table_suffix_enable);

}

const dnxc_data_table_info_t *
dnx_data_module_testing_property_methods_range_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_module_testing, dnx_data_module_testing_submodule_property_methods, dnx_data_module_testing_property_methods_table_range);

}

const dnxc_data_table_info_t *
dnx_data_module_testing_property_methods_port_range_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_module_testing, dnx_data_module_testing_submodule_property_methods, dnx_data_module_testing_property_methods_table_port_range);

}

const dnxc_data_table_info_t *
dnx_data_module_testing_property_methods_suffix_range_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_module_testing, dnx_data_module_testing_submodule_property_methods, dnx_data_module_testing_property_methods_table_suffix_range);

}

const dnxc_data_table_info_t *
dnx_data_module_testing_property_methods_direct_map_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_module_testing, dnx_data_module_testing_submodule_property_methods, dnx_data_module_testing_property_methods_table_direct_map);

}

const dnxc_data_table_info_t *
dnx_data_module_testing_property_methods_port_direct_map_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_module_testing, dnx_data_module_testing_submodule_property_methods, dnx_data_module_testing_property_methods_table_port_direct_map);

}

const dnxc_data_table_info_t *
dnx_data_module_testing_property_methods_suffix_direct_map_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_module_testing, dnx_data_module_testing_submodule_property_methods, dnx_data_module_testing_property_methods_table_suffix_direct_map);

}

const dnxc_data_table_info_t *
dnx_data_module_testing_property_methods_custom_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_module_testing, dnx_data_module_testing_submodule_property_methods, dnx_data_module_testing_property_methods_table_custom);

}

const dnxc_data_table_info_t *
dnx_data_module_testing_property_methods_pbmp_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_module_testing, dnx_data_module_testing_submodule_property_methods, dnx_data_module_testing_property_methods_table_pbmp);

}

const dnxc_data_table_info_t *
dnx_data_module_testing_property_methods_port_pbmp_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_module_testing, dnx_data_module_testing_submodule_property_methods, dnx_data_module_testing_property_methods_table_port_pbmp);

}

const dnxc_data_table_info_t *
dnx_data_module_testing_property_methods_suffix_pbmp_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_module_testing, dnx_data_module_testing_submodule_property_methods, dnx_data_module_testing_property_methods_table_suffix_pbmp);

}

const dnxc_data_table_info_t *
dnx_data_module_testing_property_methods_str_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_module_testing, dnx_data_module_testing_submodule_property_methods, dnx_data_module_testing_property_methods_table_str);

}

const dnxc_data_table_info_t *
dnx_data_module_testing_property_methods_port_str_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_module_testing, dnx_data_module_testing_submodule_property_methods, dnx_data_module_testing_property_methods_table_port_str);

}

const dnxc_data_table_info_t *
dnx_data_module_testing_property_methods_suffix_str_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_module_testing, dnx_data_module_testing_submodule_property_methods, dnx_data_module_testing_property_methods_table_suffix_str);

}






static shr_error_e
dnx_data_module_testing_data_pointers_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "data_pointers";
    submodule_data->doc = "";
    
    submodule_data->nof_features = _dnx_data_module_testing_data_pointers_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data module_testing data_pointers features");

    
    submodule_data->nof_defines = _dnx_data_module_testing_data_pointers_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data module_testing data_pointers defines");

    submodule_data->defines[dnx_data_module_testing_data_pointers_define_cores].name = "cores";
    submodule_data->defines[dnx_data_module_testing_data_pointers_define_cores].doc = "";
    
    submodule_data->defines[dnx_data_module_testing_data_pointers_define_cores].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_module_testing_data_pointers_define_add_cores].name = "add_cores";
    submodule_data->defines[dnx_data_module_testing_data_pointers_define_add_cores].doc = "";
    
    submodule_data->defines[dnx_data_module_testing_data_pointers_define_add_cores].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_module_testing_data_pointers_define_mul_cores].name = "mul_cores";
    submodule_data->defines[dnx_data_module_testing_data_pointers_define_mul_cores].doc = "";
    
    submodule_data->defines[dnx_data_module_testing_data_pointers_define_mul_cores].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_module_testing_data_pointers_define_div_cores].name = "div_cores";
    submodule_data->defines[dnx_data_module_testing_data_pointers_define_div_cores].doc = "";
    
    submodule_data->defines[dnx_data_module_testing_data_pointers_define_div_cores].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_module_testing_data_pointers_define_op_cores].name = "op_cores";
    submodule_data->defines[dnx_data_module_testing_data_pointers_define_op_cores].doc = "";
    
    submodule_data->defines[dnx_data_module_testing_data_pointers_define_op_cores].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_module_testing_data_pointers_define_num_cores].name = "num_cores";
    submodule_data->defines[dnx_data_module_testing_data_pointers_define_num_cores].doc = "";
    
    submodule_data->defines[dnx_data_module_testing_data_pointers_define_num_cores].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_module_testing_data_pointers_define_num_add_cores].name = "num_add_cores";
    submodule_data->defines[dnx_data_module_testing_data_pointers_define_num_add_cores].doc = "";
    
    submodule_data->defines[dnx_data_module_testing_data_pointers_define_num_add_cores].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_module_testing_data_pointers_define_num_mul_cores].name = "num_mul_cores";
    submodule_data->defines[dnx_data_module_testing_data_pointers_define_num_mul_cores].doc = "";
    
    submodule_data->defines[dnx_data_module_testing_data_pointers_define_num_mul_cores].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_module_testing_data_pointers_define_num_div_cores].name = "num_div_cores";
    submodule_data->defines[dnx_data_module_testing_data_pointers_define_num_div_cores].doc = "";
    
    submodule_data->defines[dnx_data_module_testing_data_pointers_define_num_div_cores].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_module_testing_data_pointers_define_num_op_cores].name = "num_op_cores";
    submodule_data->defines[dnx_data_module_testing_data_pointers_define_num_op_cores].doc = "";
    
    submodule_data->defines[dnx_data_module_testing_data_pointers_define_num_op_cores].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_module_testing_data_pointers_define_num_last_cores].name = "num_last_cores";
    submodule_data->defines[dnx_data_module_testing_data_pointers_define_num_last_cores].doc = "";
    
    submodule_data->defines[dnx_data_module_testing_data_pointers_define_num_last_cores].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_module_testing_data_pointers_define_num_table_cores].name = "num_table_cores";
    submodule_data->defines[dnx_data_module_testing_data_pointers_define_num_table_cores].doc = "";
    
    submodule_data->defines[dnx_data_module_testing_data_pointers_define_num_table_cores].flags |= DNXC_DATA_F_NUMERIC;

    
    submodule_data->nof_tables = _dnx_data_module_testing_data_pointers_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data module_testing data_pointers tables");

    
    submodule_data->tables[dnx_data_module_testing_data_pointers_table_table_pointers].name = "table_pointers";
    submodule_data->tables[dnx_data_module_testing_data_pointers_table_table_pointers].doc = "";
    submodule_data->tables[dnx_data_module_testing_data_pointers_table_table_pointers].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_module_testing_data_pointers_table_table_pointers].size_of_values = sizeof(dnx_data_module_testing_data_pointers_table_pointers_t);
    submodule_data->tables[dnx_data_module_testing_data_pointers_table_table_pointers].entry_get = dnx_data_module_testing_data_pointers_table_pointers_entry_str_get;

    
    submodule_data->tables[dnx_data_module_testing_data_pointers_table_table_pointers].nof_keys = 1;
    submodule_data->tables[dnx_data_module_testing_data_pointers_table_table_pointers].keys[0].name = "index";
    submodule_data->tables[dnx_data_module_testing_data_pointers_table_table_pointers].keys[0].doc = "";

    
    submodule_data->tables[dnx_data_module_testing_data_pointers_table_table_pointers].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_module_testing_data_pointers_table_table_pointers].values, dnxc_data_value_t, submodule_data->tables[dnx_data_module_testing_data_pointers_table_table_pointers].nof_values, "_dnx_data_module_testing_data_pointers_table_table_pointers table values");
    submodule_data->tables[dnx_data_module_testing_data_pointers_table_table_pointers].values[0].name = "num_cores";
    submodule_data->tables[dnx_data_module_testing_data_pointers_table_table_pointers].values[0].type = "uint32";
    submodule_data->tables[dnx_data_module_testing_data_pointers_table_table_pointers].values[0].doc = "dummy";
    submodule_data->tables[dnx_data_module_testing_data_pointers_table_table_pointers].values[0].offset = UTILEX_OFFSETOF(dnx_data_module_testing_data_pointers_table_pointers_t, num_cores);


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_module_testing_data_pointers_feature_get(
    int unit,
    dnx_data_module_testing_data_pointers_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_module_testing, dnx_data_module_testing_submodule_data_pointers, feature);
}


uint32
dnx_data_module_testing_data_pointers_cores_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_module_testing, dnx_data_module_testing_submodule_data_pointers, dnx_data_module_testing_data_pointers_define_cores);
}

uint32
dnx_data_module_testing_data_pointers_add_cores_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_module_testing, dnx_data_module_testing_submodule_data_pointers, dnx_data_module_testing_data_pointers_define_add_cores);
}

uint32
dnx_data_module_testing_data_pointers_mul_cores_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_module_testing, dnx_data_module_testing_submodule_data_pointers, dnx_data_module_testing_data_pointers_define_mul_cores);
}

uint32
dnx_data_module_testing_data_pointers_div_cores_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_module_testing, dnx_data_module_testing_submodule_data_pointers, dnx_data_module_testing_data_pointers_define_div_cores);
}

uint32
dnx_data_module_testing_data_pointers_op_cores_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_module_testing, dnx_data_module_testing_submodule_data_pointers, dnx_data_module_testing_data_pointers_define_op_cores);
}

uint32
dnx_data_module_testing_data_pointers_num_cores_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_module_testing, dnx_data_module_testing_submodule_data_pointers, dnx_data_module_testing_data_pointers_define_num_cores);
}

uint32
dnx_data_module_testing_data_pointers_num_add_cores_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_module_testing, dnx_data_module_testing_submodule_data_pointers, dnx_data_module_testing_data_pointers_define_num_add_cores);
}

uint32
dnx_data_module_testing_data_pointers_num_mul_cores_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_module_testing, dnx_data_module_testing_submodule_data_pointers, dnx_data_module_testing_data_pointers_define_num_mul_cores);
}

uint32
dnx_data_module_testing_data_pointers_num_div_cores_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_module_testing, dnx_data_module_testing_submodule_data_pointers, dnx_data_module_testing_data_pointers_define_num_div_cores);
}

uint32
dnx_data_module_testing_data_pointers_num_op_cores_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_module_testing, dnx_data_module_testing_submodule_data_pointers, dnx_data_module_testing_data_pointers_define_num_op_cores);
}

uint32
dnx_data_module_testing_data_pointers_num_last_cores_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_module_testing, dnx_data_module_testing_submodule_data_pointers, dnx_data_module_testing_data_pointers_define_num_last_cores);
}

uint32
dnx_data_module_testing_data_pointers_num_table_cores_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_module_testing, dnx_data_module_testing_submodule_data_pointers, dnx_data_module_testing_data_pointers_define_num_table_cores);
}



const dnx_data_module_testing_data_pointers_table_pointers_t *
dnx_data_module_testing_data_pointers_table_pointers_get(
    int unit,
    int index)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_module_testing, dnx_data_module_testing_submodule_data_pointers, dnx_data_module_testing_data_pointers_table_table_pointers);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, index, 0);
    return (const dnx_data_module_testing_data_pointers_table_pointers_t *) data;

}


shr_error_e
dnx_data_module_testing_data_pointers_table_pointers_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_module_testing_data_pointers_table_pointers_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_module_testing, dnx_data_module_testing_submodule_data_pointers, dnx_data_module_testing_data_pointers_table_table_pointers);
    data = (const dnx_data_module_testing_data_pointers_table_pointers_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->num_cores);
            break;
    }

    SHR_FUNC_EXIT;
}


const dnxc_data_table_info_t *
dnx_data_module_testing_data_pointers_table_pointers_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_module_testing, dnx_data_module_testing_submodule_data_pointers, dnx_data_module_testing_data_pointers_table_table_pointers);

}






static shr_error_e
dnx_data_module_testing_dbal_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "dbal";
    submodule_data->doc = "";
    
    submodule_data->nof_features = _dnx_data_module_testing_dbal_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data module_testing dbal features");

    
    submodule_data->nof_defines = _dnx_data_module_testing_dbal_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data module_testing dbal defines");

    submodule_data->defines[dnx_data_module_testing_dbal_define_vrf_field_size].name = "vrf_field_size";
    submodule_data->defines[dnx_data_module_testing_dbal_define_vrf_field_size].doc = "";
    
    submodule_data->defines[dnx_data_module_testing_dbal_define_vrf_field_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_module_testing_dbal_define_field_define_example].name = "field_define_example";
    submodule_data->defines[dnx_data_module_testing_dbal_define_field_define_example].doc = "";
    
    submodule_data->defines[dnx_data_module_testing_dbal_define_field_define_example].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_module_testing_dbal_define_res_max_val].name = "res_max_val";
    submodule_data->defines[dnx_data_module_testing_dbal_define_res_max_val].doc = "";
    
    submodule_data->defines[dnx_data_module_testing_dbal_define_res_max_val].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_module_testing_dbal_define_res_field_size].name = "res_field_size";
    submodule_data->defines[dnx_data_module_testing_dbal_define_res_field_size].doc = "";
    
    submodule_data->defines[dnx_data_module_testing_dbal_define_res_field_size].flags |= DNXC_DATA_F_DEFINE;

    
    submodule_data->nof_tables = _dnx_data_module_testing_dbal_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data module_testing dbal tables");

    
    submodule_data->tables[dnx_data_module_testing_dbal_table_all].name = "all";
    submodule_data->tables[dnx_data_module_testing_dbal_table_all].doc = "";
    submodule_data->tables[dnx_data_module_testing_dbal_table_all].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_module_testing_dbal_table_all].size_of_values = sizeof(dnx_data_module_testing_dbal_all_t);
    submodule_data->tables[dnx_data_module_testing_dbal_table_all].entry_get = dnx_data_module_testing_dbal_all_entry_str_get;

    
    submodule_data->tables[dnx_data_module_testing_dbal_table_all].nof_keys = 0;

    
    submodule_data->tables[dnx_data_module_testing_dbal_table_all].nof_values = 4;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_module_testing_dbal_table_all].values, dnxc_data_value_t, submodule_data->tables[dnx_data_module_testing_dbal_table_all].nof_values, "_dnx_data_module_testing_dbal_table_all table values");
    submodule_data->tables[dnx_data_module_testing_dbal_table_all].values[0].name = "vlan_id_in_ingress";
    submodule_data->tables[dnx_data_module_testing_dbal_table_all].values[0].type = "uint32";
    submodule_data->tables[dnx_data_module_testing_dbal_table_all].values[0].doc = "field in table size test";
    submodule_data->tables[dnx_data_module_testing_dbal_table_all].values[0].offset = UTILEX_OFFSETOF(dnx_data_module_testing_dbal_all_t, vlan_id_in_ingress);
    submodule_data->tables[dnx_data_module_testing_dbal_table_all].values[1].name = "fid_in_mact";
    submodule_data->tables[dnx_data_module_testing_dbal_table_all].values[1].type = "uint32";
    submodule_data->tables[dnx_data_module_testing_dbal_table_all].values[1].doc = "field in table valid test";
    submodule_data->tables[dnx_data_module_testing_dbal_table_all].values[1].offset = UTILEX_OFFSETOF(dnx_data_module_testing_dbal_all_t, fid_in_mact);
    submodule_data->tables[dnx_data_module_testing_dbal_table_all].values[2].name = "key_max_val";
    submodule_data->tables[dnx_data_module_testing_dbal_table_all].values[2].type = "uint32";
    submodule_data->tables[dnx_data_module_testing_dbal_table_all].values[2].doc = "usage example";
    submodule_data->tables[dnx_data_module_testing_dbal_table_all].values[2].offset = UTILEX_OFFSETOF(dnx_data_module_testing_dbal_all_t, key_max_val);
    submodule_data->tables[dnx_data_module_testing_dbal_table_all].values[3].name = "key_field_size";
    submodule_data->tables[dnx_data_module_testing_dbal_table_all].values[3].type = "uint32";
    submodule_data->tables[dnx_data_module_testing_dbal_table_all].values[3].doc = "usage example";
    submodule_data->tables[dnx_data_module_testing_dbal_table_all].values[3].offset = UTILEX_OFFSETOF(dnx_data_module_testing_dbal_all_t, key_field_size);


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_module_testing_dbal_feature_get(
    int unit,
    dnx_data_module_testing_dbal_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_module_testing, dnx_data_module_testing_submodule_dbal, feature);
}


uint32
dnx_data_module_testing_dbal_vrf_field_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_module_testing, dnx_data_module_testing_submodule_dbal, dnx_data_module_testing_dbal_define_vrf_field_size);
}

uint32
dnx_data_module_testing_dbal_field_define_example_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_module_testing, dnx_data_module_testing_submodule_dbal, dnx_data_module_testing_dbal_define_field_define_example);
}

uint32
dnx_data_module_testing_dbal_res_max_val_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_module_testing, dnx_data_module_testing_submodule_dbal, dnx_data_module_testing_dbal_define_res_max_val);
}

uint32
dnx_data_module_testing_dbal_res_field_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_module_testing, dnx_data_module_testing_submodule_dbal, dnx_data_module_testing_dbal_define_res_field_size);
}



const dnx_data_module_testing_dbal_all_t *
dnx_data_module_testing_dbal_all_get(
    int unit)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_module_testing, dnx_data_module_testing_submodule_dbal, dnx_data_module_testing_dbal_table_all);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
    return (const dnx_data_module_testing_dbal_all_t *) data;

}


shr_error_e
dnx_data_module_testing_dbal_all_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_module_testing_dbal_all_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_module_testing, dnx_data_module_testing_submodule_dbal, dnx_data_module_testing_dbal_table_all);
    data = (const dnx_data_module_testing_dbal_all_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, 0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->vlan_id_in_ingress);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->fid_in_mact);
            break;
        case 2:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->key_max_val);
            break;
        case 3:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->key_field_size);
            break;
    }

    SHR_FUNC_EXIT;
}


const dnxc_data_table_info_t *
dnx_data_module_testing_dbal_all_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_module_testing, dnx_data_module_testing_submodule_dbal, dnx_data_module_testing_dbal_table_all);

}






static shr_error_e
dnx_data_module_testing_example_tests_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "example_tests";
    submodule_data->doc = "";
    
    submodule_data->nof_features = _dnx_data_module_testing_example_tests_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data module_testing example_tests features");

    
    submodule_data->nof_defines = _dnx_data_module_testing_example_tests_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data module_testing example_tests defines");

    submodule_data->defines[dnx_data_module_testing_example_tests_define_field_size].name = "field_size";
    submodule_data->defines[dnx_data_module_testing_example_tests_define_field_size].doc = "";
    
    submodule_data->defines[dnx_data_module_testing_example_tests_define_field_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_module_testing_example_tests_define_large_nof_elements].name = "large_nof_elements";
    submodule_data->defines[dnx_data_module_testing_example_tests_define_large_nof_elements].doc = "";
    
    submodule_data->defines[dnx_data_module_testing_example_tests_define_large_nof_elements].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_module_testing_example_tests_define_default_memory_size].name = "default_memory_size";
    submodule_data->defines[dnx_data_module_testing_example_tests_define_default_memory_size].doc = "";
    
    submodule_data->defines[dnx_data_module_testing_example_tests_define_default_memory_size].flags |= DNXC_DATA_F_DEFINE;

    
    submodule_data->nof_tables = _dnx_data_module_testing_example_tests_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data module_testing example_tests tables");

    
    submodule_data->tables[dnx_data_module_testing_example_tests_table_all].name = "all";
    submodule_data->tables[dnx_data_module_testing_example_tests_table_all].doc = "";
    submodule_data->tables[dnx_data_module_testing_example_tests_table_all].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_module_testing_example_tests_table_all].size_of_values = sizeof(dnx_data_module_testing_example_tests_all_t);
    submodule_data->tables[dnx_data_module_testing_example_tests_table_all].entry_get = dnx_data_module_testing_example_tests_all_entry_str_get;

    
    submodule_data->tables[dnx_data_module_testing_example_tests_table_all].nof_keys = 0;

    
    submodule_data->tables[dnx_data_module_testing_example_tests_table_all].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_module_testing_example_tests_table_all].values, dnxc_data_value_t, submodule_data->tables[dnx_data_module_testing_example_tests_table_all].nof_values, "_dnx_data_module_testing_example_tests_table_all table values");
    submodule_data->tables[dnx_data_module_testing_example_tests_table_all].values[0].name = "vlan_id_in_ingress";
    submodule_data->tables[dnx_data_module_testing_example_tests_table_all].values[0].type = "uint32";
    submodule_data->tables[dnx_data_module_testing_example_tests_table_all].values[0].doc = "field in table size test";
    submodule_data->tables[dnx_data_module_testing_example_tests_table_all].values[0].offset = UTILEX_OFFSETOF(dnx_data_module_testing_example_tests_all_t, vlan_id_in_ingress);

    
    submodule_data->tables[dnx_data_module_testing_example_tests_table_mem_consumption].name = "mem_consumption";
    submodule_data->tables[dnx_data_module_testing_example_tests_table_mem_consumption].doc = "";
    submodule_data->tables[dnx_data_module_testing_example_tests_table_mem_consumption].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_module_testing_example_tests_table_mem_consumption].size_of_values = sizeof(dnx_data_module_testing_example_tests_mem_consumption_t);
    submodule_data->tables[dnx_data_module_testing_example_tests_table_mem_consumption].entry_get = dnx_data_module_testing_example_tests_mem_consumption_entry_str_get;

    
    submodule_data->tables[dnx_data_module_testing_example_tests_table_mem_consumption].nof_keys = 1;
    submodule_data->tables[dnx_data_module_testing_example_tests_table_mem_consumption].keys[0].name = "module_id";
    submodule_data->tables[dnx_data_module_testing_example_tests_table_mem_consumption].keys[0].doc = "";

    
    submodule_data->tables[dnx_data_module_testing_example_tests_table_mem_consumption].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_module_testing_example_tests_table_mem_consumption].values, dnxc_data_value_t, submodule_data->tables[dnx_data_module_testing_example_tests_table_mem_consumption].nof_values, "_dnx_data_module_testing_example_tests_table_mem_consumption table values");
    submodule_data->tables[dnx_data_module_testing_example_tests_table_mem_consumption].values[0].name = "memory_size";
    submodule_data->tables[dnx_data_module_testing_example_tests_table_mem_consumption].values[0].type = "uint32";
    submodule_data->tables[dnx_data_module_testing_example_tests_table_mem_consumption].values[0].doc = "Memory size threshold in bytes.";
    submodule_data->tables[dnx_data_module_testing_example_tests_table_mem_consumption].values[0].offset = UTILEX_OFFSETOF(dnx_data_module_testing_example_tests_mem_consumption_t, memory_size);


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_module_testing_example_tests_feature_get(
    int unit,
    dnx_data_module_testing_example_tests_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_module_testing, dnx_data_module_testing_submodule_example_tests, feature);
}


uint32
dnx_data_module_testing_example_tests_field_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_module_testing, dnx_data_module_testing_submodule_example_tests, dnx_data_module_testing_example_tests_define_field_size);
}

uint32
dnx_data_module_testing_example_tests_large_nof_elements_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_module_testing, dnx_data_module_testing_submodule_example_tests, dnx_data_module_testing_example_tests_define_large_nof_elements);
}

uint32
dnx_data_module_testing_example_tests_default_memory_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_module_testing, dnx_data_module_testing_submodule_example_tests, dnx_data_module_testing_example_tests_define_default_memory_size);
}



const dnx_data_module_testing_example_tests_all_t *
dnx_data_module_testing_example_tests_all_get(
    int unit)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_module_testing, dnx_data_module_testing_submodule_example_tests, dnx_data_module_testing_example_tests_table_all);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
    return (const dnx_data_module_testing_example_tests_all_t *) data;

}

const dnx_data_module_testing_example_tests_mem_consumption_t *
dnx_data_module_testing_example_tests_mem_consumption_get(
    int unit,
    int module_id)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_module_testing, dnx_data_module_testing_submodule_example_tests, dnx_data_module_testing_example_tests_table_mem_consumption);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, module_id, 0);
    return (const dnx_data_module_testing_example_tests_mem_consumption_t *) data;

}


shr_error_e
dnx_data_module_testing_example_tests_all_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_module_testing_example_tests_all_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_module_testing, dnx_data_module_testing_submodule_example_tests, dnx_data_module_testing_example_tests_table_all);
    data = (const dnx_data_module_testing_example_tests_all_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, 0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->vlan_id_in_ingress);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_module_testing_example_tests_mem_consumption_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_module_testing_example_tests_mem_consumption_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_module_testing, dnx_data_module_testing_submodule_example_tests, dnx_data_module_testing_example_tests_table_mem_consumption);
    data = (const dnx_data_module_testing_example_tests_mem_consumption_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->memory_size);
            break;
    }

    SHR_FUNC_EXIT;
}


const dnxc_data_table_info_t *
dnx_data_module_testing_example_tests_all_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_module_testing, dnx_data_module_testing_submodule_example_tests, dnx_data_module_testing_example_tests_table_all);

}

const dnxc_data_table_info_t *
dnx_data_module_testing_example_tests_mem_consumption_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_module_testing, dnx_data_module_testing_submodule_example_tests, dnx_data_module_testing_example_tests_table_mem_consumption);

}







static shr_error_e
dnx_data_module_testing_submodule_family_dnx2_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "submodule_family_dnx2";
    submodule_data->doc = "";
    
    submodule_data->nof_features = _dnx_data_module_testing_submodule_family_dnx2_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data module_testing submodule_family_dnx2 features");

    submodule_data->features[dnx_data_module_testing_submodule_family_dnx2_dnx2_submodule_feature].name = "dnx2_submodule_feature";
    submodule_data->features[dnx_data_module_testing_submodule_family_dnx2_dnx2_submodule_feature].doc = "";
    submodule_data->features[dnx_data_module_testing_submodule_family_dnx2_dnx2_submodule_feature].flags |= DNXC_DATA_F_FEATURE;
    if (!soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->features[dnx_data_module_testing_submodule_family_dnx2_dnx2_submodule_feature].data = 0;
    }

    
    submodule_data->nof_defines = _dnx_data_module_testing_submodule_family_dnx2_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data module_testing submodule_family_dnx2 defines");

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_module_testing_submodule_family_dnx2_define_dnx2_submodule_define].name = "dnx2_submodule_define";
        submodule_data->defines[dnx_data_module_testing_submodule_family_dnx2_define_dnx2_submodule_define].doc = "";
        
        submodule_data->defines[dnx_data_module_testing_submodule_family_dnx2_define_dnx2_submodule_define].flags |= DNXC_DATA_F_DEFINE;
    }

    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_module_testing_submodule_family_dnx2_define_dnx2_submodule_numeric].name = "dnx2_submodule_numeric";
        submodule_data->defines[dnx_data_module_testing_submodule_family_dnx2_define_dnx2_submodule_numeric].doc = "";
        
        submodule_data->defines[dnx_data_module_testing_submodule_family_dnx2_define_dnx2_submodule_numeric].flags |= DNXC_DATA_F_NUMERIC;
    }

    
    submodule_data->nof_tables = _dnx_data_module_testing_submodule_family_dnx2_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data module_testing submodule_family_dnx2 tables");

    if (soc_is(unit, DNX2_DEVICE))
    {
        
        submodule_data->tables[dnx_data_module_testing_submodule_family_dnx2_table_dnx2_submodule_table].name = "dnx2_submodule_table";
        submodule_data->tables[dnx_data_module_testing_submodule_family_dnx2_table_dnx2_submodule_table].doc = "";
        submodule_data->tables[dnx_data_module_testing_submodule_family_dnx2_table_dnx2_submodule_table].flags |= DNXC_DATA_F_TABLE;
        submodule_data->tables[dnx_data_module_testing_submodule_family_dnx2_table_dnx2_submodule_table].size_of_values = sizeof(dnx_data_module_testing_submodule_family_dnx2_dnx2_submodule_table_t);
        submodule_data->tables[dnx_data_module_testing_submodule_family_dnx2_table_dnx2_submodule_table].entry_get = dnx_data_module_testing_submodule_family_dnx2_dnx2_submodule_table_entry_str_get;

        
        submodule_data->tables[dnx_data_module_testing_submodule_family_dnx2_table_dnx2_submodule_table].nof_keys = 1;
        submodule_data->tables[dnx_data_module_testing_submodule_family_dnx2_table_dnx2_submodule_table].keys[0].name = "dnx2_submodule_table_key";
        submodule_data->tables[dnx_data_module_testing_submodule_family_dnx2_table_dnx2_submodule_table].keys[0].doc = "";

        
        submodule_data->tables[dnx_data_module_testing_submodule_family_dnx2_table_dnx2_submodule_table].nof_values = 1;
        DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_module_testing_submodule_family_dnx2_table_dnx2_submodule_table].values, dnxc_data_value_t, submodule_data->tables[dnx_data_module_testing_submodule_family_dnx2_table_dnx2_submodule_table].nof_values, "_dnx_data_module_testing_submodule_family_dnx2_table_dnx2_submodule_table table values");
        submodule_data->tables[dnx_data_module_testing_submodule_family_dnx2_table_dnx2_submodule_table].values[0].name = "dnx2_submodule_table_value";
        submodule_data->tables[dnx_data_module_testing_submodule_family_dnx2_table_dnx2_submodule_table].values[0].type = "uint32";
        submodule_data->tables[dnx_data_module_testing_submodule_family_dnx2_table_dnx2_submodule_table].values[0].doc = "table_value";
        submodule_data->tables[dnx_data_module_testing_submodule_family_dnx2_table_dnx2_submodule_table].values[0].offset = UTILEX_OFFSETOF(dnx_data_module_testing_submodule_family_dnx2_dnx2_submodule_table_t, dnx2_submodule_table_value);
    }


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_module_testing_submodule_family_dnx2_feature_get(
    int unit,
    dnx_data_module_testing_submodule_family_dnx2_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_module_testing, dnx_data_module_testing_submodule_submodule_family_dnx2, feature);
}


uint32
dnx_data_module_testing_submodule_family_dnx2_dnx2_submodule_define_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "dnx2_submodule_define", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_module_testing, dnx_data_module_testing_submodule_submodule_family_dnx2, dnx_data_module_testing_submodule_family_dnx2_define_dnx2_submodule_define);
}

uint32
dnx_data_module_testing_submodule_family_dnx2_dnx2_submodule_numeric_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "dnx2_submodule_numeric", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_module_testing, dnx_data_module_testing_submodule_submodule_family_dnx2, dnx_data_module_testing_submodule_family_dnx2_define_dnx2_submodule_numeric);
}



const dnx_data_module_testing_submodule_family_dnx2_dnx2_submodule_table_t *
dnx_data_module_testing_submodule_family_dnx2_dnx2_submodule_table_get(
    int unit,
    int dnx2_submodule_table_key)
{
    char *data;
    dnxc_data_table_t *table;

    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "dnx2_submodule_table", "DNX2");
    }
    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_module_testing, dnx_data_module_testing_submodule_submodule_family_dnx2, dnx_data_module_testing_submodule_family_dnx2_table_dnx2_submodule_table);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, dnx2_submodule_table_key, 0);
    return (const dnx_data_module_testing_submodule_family_dnx2_dnx2_submodule_table_t *) data;

}


shr_error_e
dnx_data_module_testing_submodule_family_dnx2_dnx2_submodule_table_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_module_testing_submodule_family_dnx2_dnx2_submodule_table_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_module_testing, dnx_data_module_testing_submodule_submodule_family_dnx2, dnx_data_module_testing_submodule_family_dnx2_table_dnx2_submodule_table);
    data = (const dnx_data_module_testing_submodule_family_dnx2_dnx2_submodule_table_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->dnx2_submodule_table_value);
            break;
    }

    SHR_FUNC_EXIT;
}


const dnxc_data_table_info_t *
dnx_data_module_testing_submodule_family_dnx2_dnx2_submodule_table_info_get(
    int unit)
{

    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "dnx2_submodule_table", "DNX2");
    }
    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_module_testing, dnx_data_module_testing_submodule_submodule_family_dnx2, dnx_data_module_testing_submodule_family_dnx2_table_dnx2_submodule_table);

}







static shr_error_e
dnx_data_module_testing_compile_flags_submodule_standard_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "compile_flags_submodule_standard";
    submodule_data->doc = "";
    
    submodule_data->nof_features = _dnx_data_module_testing_compile_flags_submodule_standard_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data module_testing compile_flags_submodule_standard features");

#ifdef INCLUDE_XFLOW_MACSEC

    submodule_data->features[dnx_data_module_testing_compile_flags_submodule_standard_feature_flags].name = "feature_flags";
    submodule_data->features[dnx_data_module_testing_compile_flags_submodule_standard_feature_flags].doc = "";
    submodule_data->features[dnx_data_module_testing_compile_flags_submodule_standard_feature_flags].flags |= DNXC_DATA_F_FEATURE;

#endif 
    
    submodule_data->nof_defines = _dnx_data_module_testing_compile_flags_submodule_standard_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data module_testing compile_flags_submodule_standard defines");

#ifdef INCLUDE_XFLOW_MACSEC

    submodule_data->defines[dnx_data_module_testing_compile_flags_submodule_standard_define_define_ifdef].name = "define_ifdef";
    submodule_data->defines[dnx_data_module_testing_compile_flags_submodule_standard_define_define_ifdef].doc = "";
    
    submodule_data->defines[dnx_data_module_testing_compile_flags_submodule_standard_define_define_ifdef].flags |= DNXC_DATA_F_DEFINE;

#endif 
#ifndef INCLUDE_XFLOW_MACSEC

    submodule_data->defines[dnx_data_module_testing_compile_flags_submodule_standard_define_define_ifndef].name = "define_ifndef";
    submodule_data->defines[dnx_data_module_testing_compile_flags_submodule_standard_define_define_ifndef].doc = "";
    
    submodule_data->defines[dnx_data_module_testing_compile_flags_submodule_standard_define_define_ifndef].flags |= DNXC_DATA_F_DEFINE;

#endif 
#if defined(INCLUDE_XFLOW_MACSEC)

    submodule_data->defines[dnx_data_module_testing_compile_flags_submodule_standard_define_define_if].name = "define_if";
    submodule_data->defines[dnx_data_module_testing_compile_flags_submodule_standard_define_define_if].doc = "";
    
    submodule_data->defines[dnx_data_module_testing_compile_flags_submodule_standard_define_define_if].flags |= DNXC_DATA_F_DEFINE;

#endif 
#ifdef INCLUDE_XFLOW_MACSEC

    submodule_data->defines[dnx_data_module_testing_compile_flags_submodule_standard_define_numeric_flags].name = "numeric_flags";
    submodule_data->defines[dnx_data_module_testing_compile_flags_submodule_standard_define_numeric_flags].doc = "";
    
    submodule_data->defines[dnx_data_module_testing_compile_flags_submodule_standard_define_numeric_flags].flags |= DNXC_DATA_F_NUMERIC;

#endif 
    
    submodule_data->nof_tables = _dnx_data_module_testing_compile_flags_submodule_standard_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data module_testing compile_flags_submodule_standard tables");

#ifdef INCLUDE_XFLOW_MACSEC

    
    submodule_data->tables[dnx_data_module_testing_compile_flags_submodule_standard_table_table_flags].name = "table_flags";
    submodule_data->tables[dnx_data_module_testing_compile_flags_submodule_standard_table_table_flags].doc = "";
    submodule_data->tables[dnx_data_module_testing_compile_flags_submodule_standard_table_table_flags].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_module_testing_compile_flags_submodule_standard_table_table_flags].size_of_values = sizeof(dnx_data_module_testing_compile_flags_submodule_standard_table_flags_t);
    submodule_data->tables[dnx_data_module_testing_compile_flags_submodule_standard_table_table_flags].entry_get = dnx_data_module_testing_compile_flags_submodule_standard_table_flags_entry_str_get;

    
    submodule_data->tables[dnx_data_module_testing_compile_flags_submodule_standard_table_table_flags].nof_keys = 1;
    submodule_data->tables[dnx_data_module_testing_compile_flags_submodule_standard_table_table_flags].keys[0].name = "table_flags_key";
    submodule_data->tables[dnx_data_module_testing_compile_flags_submodule_standard_table_table_flags].keys[0].doc = "";

    
    submodule_data->tables[dnx_data_module_testing_compile_flags_submodule_standard_table_table_flags].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_module_testing_compile_flags_submodule_standard_table_table_flags].values, dnxc_data_value_t, submodule_data->tables[dnx_data_module_testing_compile_flags_submodule_standard_table_table_flags].nof_values, "_dnx_data_module_testing_compile_flags_submodule_standard_table_table_flags table values");
    submodule_data->tables[dnx_data_module_testing_compile_flags_submodule_standard_table_table_flags].values[0].name = "table_flags_value";
    submodule_data->tables[dnx_data_module_testing_compile_flags_submodule_standard_table_table_flags].values[0].type = "uint32";
    submodule_data->tables[dnx_data_module_testing_compile_flags_submodule_standard_table_table_flags].values[0].doc = "table_value";
    submodule_data->tables[dnx_data_module_testing_compile_flags_submodule_standard_table_table_flags].values[0].offset = UTILEX_OFFSETOF(dnx_data_module_testing_compile_flags_submodule_standard_table_flags_t, table_flags_value);

#endif 

exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_module_testing_compile_flags_submodule_standard_feature_get(
    int unit,
    dnx_data_module_testing_compile_flags_submodule_standard_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_module_testing, dnx_data_module_testing_submodule_compile_flags_submodule_standard, feature);
}


#ifdef INCLUDE_XFLOW_MACSEC

uint32
dnx_data_module_testing_compile_flags_submodule_standard_define_ifdef_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_module_testing, dnx_data_module_testing_submodule_compile_flags_submodule_standard, dnx_data_module_testing_compile_flags_submodule_standard_define_define_ifdef);
}


#endif 
#ifndef INCLUDE_XFLOW_MACSEC

uint32
dnx_data_module_testing_compile_flags_submodule_standard_define_ifndef_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_module_testing, dnx_data_module_testing_submodule_compile_flags_submodule_standard, dnx_data_module_testing_compile_flags_submodule_standard_define_define_ifndef);
}


#endif 
#if defined(INCLUDE_XFLOW_MACSEC)

uint32
dnx_data_module_testing_compile_flags_submodule_standard_define_if_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_module_testing, dnx_data_module_testing_submodule_compile_flags_submodule_standard, dnx_data_module_testing_compile_flags_submodule_standard_define_define_if);
}


#endif 
#ifdef INCLUDE_XFLOW_MACSEC

uint32
dnx_data_module_testing_compile_flags_submodule_standard_numeric_flags_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_module_testing, dnx_data_module_testing_submodule_compile_flags_submodule_standard, dnx_data_module_testing_compile_flags_submodule_standard_define_numeric_flags);
}


#endif 


#ifdef INCLUDE_XFLOW_MACSEC

const dnx_data_module_testing_compile_flags_submodule_standard_table_flags_t *
dnx_data_module_testing_compile_flags_submodule_standard_table_flags_get(
    int unit,
    int table_flags_key)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_module_testing, dnx_data_module_testing_submodule_compile_flags_submodule_standard, dnx_data_module_testing_compile_flags_submodule_standard_table_table_flags);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, table_flags_key, 0);
    return (const dnx_data_module_testing_compile_flags_submodule_standard_table_flags_t *) data;

}


#endif 

#ifdef INCLUDE_XFLOW_MACSEC

shr_error_e
dnx_data_module_testing_compile_flags_submodule_standard_table_flags_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_module_testing_compile_flags_submodule_standard_table_flags_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_module_testing, dnx_data_module_testing_submodule_compile_flags_submodule_standard, dnx_data_module_testing_compile_flags_submodule_standard_table_table_flags);
    data = (const dnx_data_module_testing_compile_flags_submodule_standard_table_flags_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->table_flags_value);
            break;
    }

    SHR_FUNC_EXIT;
}


#endif 

#ifdef INCLUDE_XFLOW_MACSEC

const dnxc_data_table_info_t *
dnx_data_module_testing_compile_flags_submodule_standard_table_flags_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_module_testing, dnx_data_module_testing_submodule_compile_flags_submodule_standard, dnx_data_module_testing_compile_flags_submodule_standard_table_table_flags);

}


#endif 


#ifdef INCLUDE_XFLOW_MACSEC




static shr_error_e
dnx_data_module_testing_compile_flags_submodule_flags_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "compile_flags_submodule_flags";
    submodule_data->doc = "";
    
    submodule_data->nof_features = _dnx_data_module_testing_compile_flags_submodule_flags_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data module_testing compile_flags_submodule_flags features");

    submodule_data->features[dnx_data_module_testing_compile_flags_submodule_flags_feature_no_flags].name = "feature_no_flags";
    submodule_data->features[dnx_data_module_testing_compile_flags_submodule_flags_feature_no_flags].doc = "";
    submodule_data->features[dnx_data_module_testing_compile_flags_submodule_flags_feature_no_flags].flags |= DNXC_DATA_F_FEATURE;

#ifdef BCM_WARM_BOOT_SUPPORT

    submodule_data->features[dnx_data_module_testing_compile_flags_submodule_flags_feature_flags].name = "feature_flags";
    submodule_data->features[dnx_data_module_testing_compile_flags_submodule_flags_feature_flags].doc = "";
    submodule_data->features[dnx_data_module_testing_compile_flags_submodule_flags_feature_flags].flags |= DNXC_DATA_F_FEATURE;

#endif 
    
    submodule_data->nof_defines = _dnx_data_module_testing_compile_flags_submodule_flags_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data module_testing compile_flags_submodule_flags defines");

    submodule_data->defines[dnx_data_module_testing_compile_flags_submodule_flags_define_define_no_flags].name = "define_no_flags";
    submodule_data->defines[dnx_data_module_testing_compile_flags_submodule_flags_define_define_no_flags].doc = "";
    
    submodule_data->defines[dnx_data_module_testing_compile_flags_submodule_flags_define_define_no_flags].flags |= DNXC_DATA_F_DEFINE;

#ifdef BCM_WARM_BOOT_SUPPORT

    submodule_data->defines[dnx_data_module_testing_compile_flags_submodule_flags_define_define_flags].name = "define_flags";
    submodule_data->defines[dnx_data_module_testing_compile_flags_submodule_flags_define_define_flags].doc = "";
    
    submodule_data->defines[dnx_data_module_testing_compile_flags_submodule_flags_define_define_flags].flags |= DNXC_DATA_F_DEFINE;

#endif 
    submodule_data->defines[dnx_data_module_testing_compile_flags_submodule_flags_define_numeric_no_flags].name = "numeric_no_flags";
    submodule_data->defines[dnx_data_module_testing_compile_flags_submodule_flags_define_numeric_no_flags].doc = "";
    
    submodule_data->defines[dnx_data_module_testing_compile_flags_submodule_flags_define_numeric_no_flags].flags |= DNXC_DATA_F_NUMERIC;

#ifdef BCM_WARM_BOOT_SUPPORT

    submodule_data->defines[dnx_data_module_testing_compile_flags_submodule_flags_define_numeric_flags].name = "numeric_flags";
    submodule_data->defines[dnx_data_module_testing_compile_flags_submodule_flags_define_numeric_flags].doc = "";
    
    submodule_data->defines[dnx_data_module_testing_compile_flags_submodule_flags_define_numeric_flags].flags |= DNXC_DATA_F_NUMERIC;

#endif 
    
    submodule_data->nof_tables = _dnx_data_module_testing_compile_flags_submodule_flags_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data module_testing compile_flags_submodule_flags tables");

    
    submodule_data->tables[dnx_data_module_testing_compile_flags_submodule_flags_table_table_no_flags].name = "table_no_flags";
    submodule_data->tables[dnx_data_module_testing_compile_flags_submodule_flags_table_table_no_flags].doc = "";
    submodule_data->tables[dnx_data_module_testing_compile_flags_submodule_flags_table_table_no_flags].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_module_testing_compile_flags_submodule_flags_table_table_no_flags].size_of_values = sizeof(dnx_data_module_testing_compile_flags_submodule_flags_table_no_flags_t);
    submodule_data->tables[dnx_data_module_testing_compile_flags_submodule_flags_table_table_no_flags].entry_get = dnx_data_module_testing_compile_flags_submodule_flags_table_no_flags_entry_str_get;

    
    submodule_data->tables[dnx_data_module_testing_compile_flags_submodule_flags_table_table_no_flags].nof_keys = 1;
    submodule_data->tables[dnx_data_module_testing_compile_flags_submodule_flags_table_table_no_flags].keys[0].name = "table_no_flags_key";
    submodule_data->tables[dnx_data_module_testing_compile_flags_submodule_flags_table_table_no_flags].keys[0].doc = "";

    
    submodule_data->tables[dnx_data_module_testing_compile_flags_submodule_flags_table_table_no_flags].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_module_testing_compile_flags_submodule_flags_table_table_no_flags].values, dnxc_data_value_t, submodule_data->tables[dnx_data_module_testing_compile_flags_submodule_flags_table_table_no_flags].nof_values, "_dnx_data_module_testing_compile_flags_submodule_flags_table_table_no_flags table values");
    submodule_data->tables[dnx_data_module_testing_compile_flags_submodule_flags_table_table_no_flags].values[0].name = "table_no_flags_value";
    submodule_data->tables[dnx_data_module_testing_compile_flags_submodule_flags_table_table_no_flags].values[0].type = "uint32";
    submodule_data->tables[dnx_data_module_testing_compile_flags_submodule_flags_table_table_no_flags].values[0].doc = "table_value";
    submodule_data->tables[dnx_data_module_testing_compile_flags_submodule_flags_table_table_no_flags].values[0].offset = UTILEX_OFFSETOF(dnx_data_module_testing_compile_flags_submodule_flags_table_no_flags_t, table_no_flags_value);

#ifdef BCM_WARM_BOOT_SUPPORT

    
    submodule_data->tables[dnx_data_module_testing_compile_flags_submodule_flags_table_table_flags].name = "table_flags";
    submodule_data->tables[dnx_data_module_testing_compile_flags_submodule_flags_table_table_flags].doc = "";
    submodule_data->tables[dnx_data_module_testing_compile_flags_submodule_flags_table_table_flags].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_module_testing_compile_flags_submodule_flags_table_table_flags].size_of_values = sizeof(dnx_data_module_testing_compile_flags_submodule_flags_table_flags_t);
    submodule_data->tables[dnx_data_module_testing_compile_flags_submodule_flags_table_table_flags].entry_get = dnx_data_module_testing_compile_flags_submodule_flags_table_flags_entry_str_get;

    
    submodule_data->tables[dnx_data_module_testing_compile_flags_submodule_flags_table_table_flags].nof_keys = 1;
    submodule_data->tables[dnx_data_module_testing_compile_flags_submodule_flags_table_table_flags].keys[0].name = "table_flag_key";
    submodule_data->tables[dnx_data_module_testing_compile_flags_submodule_flags_table_table_flags].keys[0].doc = "";

    
    submodule_data->tables[dnx_data_module_testing_compile_flags_submodule_flags_table_table_flags].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_module_testing_compile_flags_submodule_flags_table_table_flags].values, dnxc_data_value_t, submodule_data->tables[dnx_data_module_testing_compile_flags_submodule_flags_table_table_flags].nof_values, "_dnx_data_module_testing_compile_flags_submodule_flags_table_table_flags table values");
    submodule_data->tables[dnx_data_module_testing_compile_flags_submodule_flags_table_table_flags].values[0].name = "table_flag_value";
    submodule_data->tables[dnx_data_module_testing_compile_flags_submodule_flags_table_table_flags].values[0].type = "uint32";
    submodule_data->tables[dnx_data_module_testing_compile_flags_submodule_flags_table_table_flags].values[0].doc = "table_value";
    submodule_data->tables[dnx_data_module_testing_compile_flags_submodule_flags_table_table_flags].values[0].offset = UTILEX_OFFSETOF(dnx_data_module_testing_compile_flags_submodule_flags_table_flags_t, table_flag_value);

#endif 

exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_module_testing_compile_flags_submodule_flags_feature_get(
    int unit,
    dnx_data_module_testing_compile_flags_submodule_flags_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_module_testing, dnx_data_module_testing_submodule_compile_flags_submodule_flags, feature);
}


uint32
dnx_data_module_testing_compile_flags_submodule_flags_define_no_flags_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_module_testing, dnx_data_module_testing_submodule_compile_flags_submodule_flags, dnx_data_module_testing_compile_flags_submodule_flags_define_define_no_flags);
}

#ifdef BCM_WARM_BOOT_SUPPORT

uint32
dnx_data_module_testing_compile_flags_submodule_flags_define_flags_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_module_testing, dnx_data_module_testing_submodule_compile_flags_submodule_flags, dnx_data_module_testing_compile_flags_submodule_flags_define_define_flags);
}


#endif 
uint32
dnx_data_module_testing_compile_flags_submodule_flags_numeric_no_flags_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_module_testing, dnx_data_module_testing_submodule_compile_flags_submodule_flags, dnx_data_module_testing_compile_flags_submodule_flags_define_numeric_no_flags);
}

#ifdef BCM_WARM_BOOT_SUPPORT

uint32
dnx_data_module_testing_compile_flags_submodule_flags_numeric_flags_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_module_testing, dnx_data_module_testing_submodule_compile_flags_submodule_flags, dnx_data_module_testing_compile_flags_submodule_flags_define_numeric_flags);
}


#endif 


const dnx_data_module_testing_compile_flags_submodule_flags_table_no_flags_t *
dnx_data_module_testing_compile_flags_submodule_flags_table_no_flags_get(
    int unit,
    int table_no_flags_key)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_module_testing, dnx_data_module_testing_submodule_compile_flags_submodule_flags, dnx_data_module_testing_compile_flags_submodule_flags_table_table_no_flags);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, table_no_flags_key, 0);
    return (const dnx_data_module_testing_compile_flags_submodule_flags_table_no_flags_t *) data;

}

#ifdef BCM_WARM_BOOT_SUPPORT

const dnx_data_module_testing_compile_flags_submodule_flags_table_flags_t *
dnx_data_module_testing_compile_flags_submodule_flags_table_flags_get(
    int unit,
    int table_flag_key)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_module_testing, dnx_data_module_testing_submodule_compile_flags_submodule_flags, dnx_data_module_testing_compile_flags_submodule_flags_table_table_flags);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, table_flag_key, 0);
    return (const dnx_data_module_testing_compile_flags_submodule_flags_table_flags_t *) data;

}


#endif 

shr_error_e
dnx_data_module_testing_compile_flags_submodule_flags_table_no_flags_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_module_testing_compile_flags_submodule_flags_table_no_flags_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_module_testing, dnx_data_module_testing_submodule_compile_flags_submodule_flags, dnx_data_module_testing_compile_flags_submodule_flags_table_table_no_flags);
    data = (const dnx_data_module_testing_compile_flags_submodule_flags_table_no_flags_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->table_no_flags_value);
            break;
    }

    SHR_FUNC_EXIT;
}

#ifdef BCM_WARM_BOOT_SUPPORT

shr_error_e
dnx_data_module_testing_compile_flags_submodule_flags_table_flags_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_module_testing_compile_flags_submodule_flags_table_flags_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_module_testing, dnx_data_module_testing_submodule_compile_flags_submodule_flags, dnx_data_module_testing_compile_flags_submodule_flags_table_table_flags);
    data = (const dnx_data_module_testing_compile_flags_submodule_flags_table_flags_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->table_flag_value);
            break;
    }

    SHR_FUNC_EXIT;
}


#endif 

const dnxc_data_table_info_t *
dnx_data_module_testing_compile_flags_submodule_flags_table_no_flags_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_module_testing, dnx_data_module_testing_submodule_compile_flags_submodule_flags, dnx_data_module_testing_compile_flags_submodule_flags_table_table_no_flags);

}

#ifdef BCM_WARM_BOOT_SUPPORT

const dnxc_data_table_info_t *
dnx_data_module_testing_compile_flags_submodule_flags_table_flags_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_module_testing, dnx_data_module_testing_submodule_compile_flags_submodule_flags, dnx_data_module_testing_compile_flags_submodule_flags_table_table_flags);

}


#endif 



#endif 



static shr_error_e
dnx_data_module_testing_max_test_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "max_test";
    submodule_data->doc = "";
    
    submodule_data->nof_features = _dnx_data_module_testing_max_test_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data module_testing max_test features");

    
    submodule_data->nof_defines = _dnx_data_module_testing_max_test_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data module_testing max_test defines");

    submodule_data->defines[dnx_data_module_testing_max_test_define_greater_d3].name = "greater_d3";
    submodule_data->defines[dnx_data_module_testing_max_test_define_greater_d3].doc = "";
    
    submodule_data->defines[dnx_data_module_testing_max_test_define_greater_d3].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_module_testing_max_test_define_greater_d2].name = "greater_d2";
    submodule_data->defines[dnx_data_module_testing_max_test_define_greater_d2].doc = "";
    
    submodule_data->defines[dnx_data_module_testing_max_test_define_greater_d2].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_module_testing_max_test_define_same_value].name = "same_value";
    submodule_data->defines[dnx_data_module_testing_max_test_define_same_value].doc = "";
    
    submodule_data->defines[dnx_data_module_testing_max_test_define_same_value].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_module_testing_max_test_define_d2_undefined].name = "d2_undefined";
    submodule_data->defines[dnx_data_module_testing_max_test_define_d2_undefined].doc = "";
    
    submodule_data->defines[dnx_data_module_testing_max_test_define_d2_undefined].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_module_testing_max_test_define_d3_undefined].name = "d3_undefined";
    submodule_data->defines[dnx_data_module_testing_max_test_define_d3_undefined].doc = "";
    
    submodule_data->defines[dnx_data_module_testing_max_test_define_d3_undefined].flags |= DNXC_DATA_F_DEFINE;


    if (soc_is(unit, DNX2_DEVICE))
    {
        submodule_data->defines[dnx_data_module_testing_max_test_define_dnx2_exclusive].name = "dnx2_exclusive";
        submodule_data->defines[dnx_data_module_testing_max_test_define_dnx2_exclusive].doc = "";
        
        submodule_data->defines[dnx_data_module_testing_max_test_define_dnx2_exclusive].flags |= DNXC_DATA_F_DEFINE;
    }

#ifdef INCLUDE_XFLOW_MACSEC

    submodule_data->defines[dnx_data_module_testing_max_test_define_compile_flags].name = "compile_flags";
    submodule_data->defines[dnx_data_module_testing_max_test_define_compile_flags].doc = "";
    
    submodule_data->defines[dnx_data_module_testing_max_test_define_compile_flags].flags |= DNXC_DATA_F_DEFINE;

#endif 
    
    submodule_data->nof_tables = _dnx_data_module_testing_max_test_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data module_testing max_test tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_module_testing_max_test_feature_get(
    int unit,
    dnx_data_module_testing_max_test_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_module_testing, dnx_data_module_testing_submodule_max_test, feature);
}


uint32
dnx_data_module_testing_max_test_greater_d3_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_module_testing, dnx_data_module_testing_submodule_max_test, dnx_data_module_testing_max_test_define_greater_d3);
}

uint32
dnx_data_module_testing_max_test_greater_d2_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_module_testing, dnx_data_module_testing_submodule_max_test, dnx_data_module_testing_max_test_define_greater_d2);
}

uint32
dnx_data_module_testing_max_test_same_value_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_module_testing, dnx_data_module_testing_submodule_max_test, dnx_data_module_testing_max_test_define_same_value);
}

uint32
dnx_data_module_testing_max_test_d2_undefined_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_module_testing, dnx_data_module_testing_submodule_max_test, dnx_data_module_testing_max_test_define_d2_undefined);
}

uint32
dnx_data_module_testing_max_test_d3_undefined_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_module_testing, dnx_data_module_testing_submodule_max_test, dnx_data_module_testing_max_test_define_d3_undefined);
}


uint32
dnx_data_module_testing_max_test_dnx2_exclusive_get(
    int unit)
{
    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_get(unit, "dnx2_exclusive", "DNX2");
    }
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_module_testing, dnx_data_module_testing_submodule_max_test, dnx_data_module_testing_max_test_define_dnx2_exclusive);
}


#ifdef INCLUDE_XFLOW_MACSEC

uint32
dnx_data_module_testing_max_test_compile_flags_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_module_testing, dnx_data_module_testing_submodule_max_test, dnx_data_module_testing_max_test_define_compile_flags);
}


#endif 






shr_error_e
dnx_data_module_testing_init(
    int unit,
    dnxc_data_module_t *module_data)
{
    SHR_FUNC_INIT_VARS(unit);

    
    module_data->name = "module_testing";
    module_data->nof_submodules = _dnx_data_module_testing_submodule_nof;
    DNXC_DATA_ALLOC(module_data->submodules, dnxc_data_submodule_t, module_data->nof_submodules, "_dnxc_data module_testing submodules");

    
    SHR_IF_ERR_EXIT(dnx_data_module_testing_unsupported_init(unit, &module_data->submodules[dnx_data_module_testing_submodule_unsupported]));
    SHR_IF_ERR_EXIT(dnx_data_module_testing_standard_submodule_init(unit, &module_data->submodules[dnx_data_module_testing_submodule_standard_submodule]));
    SHR_IF_ERR_EXIT(dnx_data_module_testing_types_init(unit, &module_data->submodules[dnx_data_module_testing_submodule_types]));
    SHR_IF_ERR_EXIT(dnx_data_module_testing_property_methods_init(unit, &module_data->submodules[dnx_data_module_testing_submodule_property_methods]));
    SHR_IF_ERR_EXIT(dnx_data_module_testing_data_pointers_init(unit, &module_data->submodules[dnx_data_module_testing_submodule_data_pointers]));
    SHR_IF_ERR_EXIT(dnx_data_module_testing_dbal_init(unit, &module_data->submodules[dnx_data_module_testing_submodule_dbal]));
    SHR_IF_ERR_EXIT(dnx_data_module_testing_example_tests_init(unit, &module_data->submodules[dnx_data_module_testing_submodule_example_tests]));

    SHR_IF_ERR_EXIT(dnx_data_module_testing_submodule_family_dnx2_init(unit, &module_data->submodules[dnx_data_module_testing_submodule_submodule_family_dnx2]));

    SHR_IF_ERR_EXIT(dnx_data_module_testing_compile_flags_submodule_standard_init(unit, &module_data->submodules[dnx_data_module_testing_submodule_compile_flags_submodule_standard]));
#ifdef INCLUDE_XFLOW_MACSEC

    SHR_IF_ERR_EXIT(dnx_data_module_testing_compile_flags_submodule_flags_init(unit, &module_data->submodules[dnx_data_module_testing_submodule_compile_flags_submodule_flags]));

#endif 
    SHR_IF_ERR_EXIT(dnx_data_module_testing_max_test_init(unit, &module_data->submodules[dnx_data_module_testing_submodule_max_test]));
    

    if (dnxc_data_mgmt_is_jr2_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_module_testing_attach(unit));
    }
    else


    if (dnxc_data_mgmt_is_jr2_b0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_module_testing_attach(unit));
    }
    else


    if (dnxc_data_mgmt_is_jr2_b1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_module_testing_attach(unit));
    }
    else


    if (dnxc_data_mgmt_is_j2c_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_module_testing_attach(unit));
        SHR_IF_ERR_EXIT(j2c_a0_data_module_testing_attach(unit));
    }
    else


    if (dnxc_data_mgmt_is_j2c_a1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_module_testing_attach(unit));
        SHR_IF_ERR_EXIT(j2c_a0_data_module_testing_attach(unit));
    }
    else


    if (dnxc_data_mgmt_is_q2a_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_module_testing_attach(unit));
        SHR_IF_ERR_EXIT(q2a_a0_data_module_testing_attach(unit));
    }
    else


    if (dnxc_data_mgmt_is_q2a_b0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_module_testing_attach(unit));
        SHR_IF_ERR_EXIT(q2a_a0_data_module_testing_attach(unit));
    }
    else


    if (dnxc_data_mgmt_is_q2a_b1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_module_testing_attach(unit));
        SHR_IF_ERR_EXIT(q2a_a0_data_module_testing_attach(unit));
    }
    else


    if (dnxc_data_mgmt_is_j2p_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_module_testing_attach(unit));
        SHR_IF_ERR_EXIT(j2p_a0_data_module_testing_attach(unit));
    }
    else


    if (dnxc_data_mgmt_is_j2p_a1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_module_testing_attach(unit));
        SHR_IF_ERR_EXIT(j2p_a0_data_module_testing_attach(unit));
    }
    else


    if (dnxc_data_mgmt_is_j2p_a2(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_module_testing_attach(unit));
        SHR_IF_ERR_EXIT(j2p_a0_data_module_testing_attach(unit));
    }
    else


    if (dnxc_data_mgmt_is_j2x_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_module_testing_attach(unit));
    }
    else

    {
        SHR_EXIT();
    }

exit:
    SHR_FUNC_EXIT;
}
#undef BSL_LOG_MODULE

