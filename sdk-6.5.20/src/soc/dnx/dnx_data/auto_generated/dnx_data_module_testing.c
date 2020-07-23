

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_COMMON

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_module_testing.h>



extern shr_error_e jr2_a0_data_module_testing_attach(
    int unit);



static shr_error_e
dnx_data_module_testing_unsupported_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "unsupported";
    submodule_data->doc = "submodule used to make sure that unsupporrted data will return the expected values";
    
    submodule_data->nof_features = _dnx_data_module_testing_unsupported_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data module_testing unsupported features");

    submodule_data->features[dnx_data_module_testing_unsupported_supported_feature].name = "supported_feature";
    submodule_data->features[dnx_data_module_testing_unsupported_supported_feature].doc = "supported feature expected to return 1";
    submodule_data->features[dnx_data_module_testing_unsupported_supported_feature].labels[0] = "test";
    submodule_data->features[dnx_data_module_testing_unsupported_supported_feature].labels[1] = "feature";
    submodule_data->features[dnx_data_module_testing_unsupported_supported_feature].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_module_testing_unsupported_unsupported_feature].name = "unsupported_feature";
    submodule_data->features[dnx_data_module_testing_unsupported_unsupported_feature].doc = "usupported feature expected to return 0";
    submodule_data->features[dnx_data_module_testing_unsupported_unsupported_feature].labels[0] = "try";
    submodule_data->features[dnx_data_module_testing_unsupported_unsupported_feature].labels[1] = "label";
    submodule_data->features[dnx_data_module_testing_unsupported_unsupported_feature].flags |= DNXC_DATA_F_FEATURE;

    
    submodule_data->nof_defines = _dnx_data_module_testing_unsupported_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data module_testing unsupported defines");

    submodule_data->defines[dnx_data_module_testing_unsupported_define_supported_def].name = "supported_def";
    submodule_data->defines[dnx_data_module_testing_unsupported_define_supported_def].doc = "supported define expected to return value";
    submodule_data->defines[dnx_data_module_testing_unsupported_define_supported_def].labels[0] = "test";
    submodule_data->defines[dnx_data_module_testing_unsupported_define_supported_def].labels[1] = "define";
    
    submodule_data->defines[dnx_data_module_testing_unsupported_define_supported_def].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_module_testing_unsupported_define_unsupported_def].name = "unsupported_def";
    submodule_data->defines[dnx_data_module_testing_unsupported_define_unsupported_def].doc = "unsupported feature expected to return NULL";
    submodule_data->defines[dnx_data_module_testing_unsupported_define_unsupported_def].labels[0] = "try";
    
    submodule_data->defines[dnx_data_module_testing_unsupported_define_unsupported_def].flags |= DNXC_DATA_F_DEFINE;

    
    submodule_data->nof_tables = _dnx_data_module_testing_unsupported_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data module_testing unsupported tables");

    
    submodule_data->tables[dnx_data_module_testing_unsupported_table_supported_table].name = "supported_table";
    submodule_data->tables[dnx_data_module_testing_unsupported_table_supported_table].doc = "supported table expected to return the required struct";
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
    submodule_data->tables[dnx_data_module_testing_unsupported_table_unsupported_table].doc = "usupported table expected to return NULL";
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
dnx_data_module_testing_types_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "types";
    submodule_data->doc = "test different data types";
    
    submodule_data->nof_features = _dnx_data_module_testing_types_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data module_testing types features");

    
    submodule_data->nof_defines = _dnx_data_module_testing_types_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data module_testing types defines");

    submodule_data->defines[dnx_data_module_testing_types_define_def_signed].name = "def_signed";
    submodule_data->defines[dnx_data_module_testing_types_define_def_signed].doc = "signed define";
    
    submodule_data->defines[dnx_data_module_testing_types_define_def_signed].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_module_testing_types_define_def_unsigned].name = "def_unsigned";
    submodule_data->defines[dnx_data_module_testing_types_define_def_unsigned].doc = "unsigned define";
    
    submodule_data->defines[dnx_data_module_testing_types_define_def_unsigned].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_module_testing_types_define_num_signed].name = "num_signed";
    submodule_data->defines[dnx_data_module_testing_types_define_num_signed].doc = "signed numeric";
    
    submodule_data->defines[dnx_data_module_testing_types_define_num_signed].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_module_testing_types_define_num_unsigned].name = "num_unsigned";
    submodule_data->defines[dnx_data_module_testing_types_define_num_unsigned].doc = "unsigned numeric";
    
    submodule_data->defines[dnx_data_module_testing_types_define_num_unsigned].flags |= DNXC_DATA_F_NUMERIC;

    
    submodule_data->nof_tables = _dnx_data_module_testing_types_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data module_testing types tables");

    
    submodule_data->tables[dnx_data_module_testing_types_table_all].name = "all";
    submodule_data->tables[dnx_data_module_testing_types_table_all].doc = "test all supported types";
    submodule_data->tables[dnx_data_module_testing_types_table_all].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_module_testing_types_table_all].size_of_values = sizeof(dnx_data_module_testing_types_all_t);
    submodule_data->tables[dnx_data_module_testing_types_table_all].entry_get = dnx_data_module_testing_types_all_entry_str_get;

    
    submodule_data->tables[dnx_data_module_testing_types_table_all].nof_keys = 0;

    
    submodule_data->tables[dnx_data_module_testing_types_table_all].nof_values = 9;
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
    submodule_data->tables[dnx_data_module_testing_types_table_all].values[5].name = "arrval";
    submodule_data->tables[dnx_data_module_testing_types_table_all].values[5].type = "uint32[10]";
    submodule_data->tables[dnx_data_module_testing_types_table_all].values[5].doc = "test array";
    submodule_data->tables[dnx_data_module_testing_types_table_all].values[5].offset = UTILEX_OFFSETOF(dnx_data_module_testing_types_all_t, arrval);
    submodule_data->tables[dnx_data_module_testing_types_table_all].values[6].name = "pbmpval";
    submodule_data->tables[dnx_data_module_testing_types_table_all].values[6].type = "bcm_pbmp_t";
    submodule_data->tables[dnx_data_module_testing_types_table_all].values[6].doc = "test pbmp";
    submodule_data->tables[dnx_data_module_testing_types_table_all].values[6].offset = UTILEX_OFFSETOF(dnx_data_module_testing_types_all_t, pbmpval);
    submodule_data->tables[dnx_data_module_testing_types_table_all].values[7].name = "strval";
    submodule_data->tables[dnx_data_module_testing_types_table_all].values[7].type = "char *";
    submodule_data->tables[dnx_data_module_testing_types_table_all].values[7].doc = "test string";
    submodule_data->tables[dnx_data_module_testing_types_table_all].values[7].offset = UTILEX_OFFSETOF(dnx_data_module_testing_types_all_t, strval);
    submodule_data->tables[dnx_data_module_testing_types_table_all].values[8].name = "bufferval";
    submodule_data->tables[dnx_data_module_testing_types_table_all].values[8].type = "char[100]";
    submodule_data->tables[dnx_data_module_testing_types_table_all].values[8].doc = "test buffer";
    submodule_data->tables[dnx_data_module_testing_types_table_all].values[8].offset = UTILEX_OFFSETOF(dnx_data_module_testing_types_all_t, bufferval);


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
            DNXC_DATA_MGMT_ARR_STR(buffer, 10, data->arrval);
            break;
        case 6:
            DNXC_DATA_MGMT_PBMP_STR(buffer, data->pbmpval);
            break;
        case 7:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%s", data->strval == NULL ? "" : data->strval);
            break;
        case 8:
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
    submodule_data->doc = "test different data types";
    
    submodule_data->nof_features = _dnx_data_module_testing_property_methods_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data module_testing property_methods features");

    submodule_data->features[dnx_data_module_testing_property_methods_feature_enable].name = "feature_enable";
    submodule_data->features[dnx_data_module_testing_property_methods_feature_enable].doc = "test feature enable method";
    submodule_data->features[dnx_data_module_testing_property_methods_feature_enable].labels[0] = "test";
    submodule_data->features[dnx_data_module_testing_property_methods_feature_enable].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_module_testing_property_methods_feature_disable].name = "feature_disable";
    submodule_data->features[dnx_data_module_testing_property_methods_feature_disable].doc = "test feature enable method with default disabled";
    submodule_data->features[dnx_data_module_testing_property_methods_feature_disable].flags |= DNXC_DATA_F_FEATURE;

    
    submodule_data->nof_defines = _dnx_data_module_testing_property_methods_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data module_testing property_methods defines");

    submodule_data->defines[dnx_data_module_testing_property_methods_define_numeric_range].name = "numeric_range";
    submodule_data->defines[dnx_data_module_testing_property_methods_define_numeric_range].doc = "test numeric range method";
    submodule_data->defines[dnx_data_module_testing_property_methods_define_numeric_range].labels[0] = "test";
    submodule_data->defines[dnx_data_module_testing_property_methods_define_numeric_range].labels[1] = "try";
    
    submodule_data->defines[dnx_data_module_testing_property_methods_define_numeric_range].flags |= DNXC_DATA_F_NUMERIC;

    
    submodule_data->nof_tables = _dnx_data_module_testing_property_methods_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data module_testing property_methods tables");

    
    submodule_data->tables[dnx_data_module_testing_property_methods_table_enable].name = "enable";
    submodule_data->tables[dnx_data_module_testing_property_methods_table_enable].doc = "test all supported types";
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
    submodule_data->tables[dnx_data_module_testing_property_methods_table_port_enable].doc = "test all supported types";
    submodule_data->tables[dnx_data_module_testing_property_methods_table_port_enable].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_module_testing_property_methods_table_port_enable].size_of_values = sizeof(dnx_data_module_testing_property_methods_port_enable_t);
    submodule_data->tables[dnx_data_module_testing_property_methods_table_port_enable].entry_get = dnx_data_module_testing_property_methods_port_enable_entry_str_get;

    
    submodule_data->tables[dnx_data_module_testing_property_methods_table_port_enable].nof_keys = 1;
    submodule_data->tables[dnx_data_module_testing_property_methods_table_port_enable].keys[0].name = "port";
    submodule_data->tables[dnx_data_module_testing_property_methods_table_port_enable].keys[0].doc = "logical port";

    
    submodule_data->tables[dnx_data_module_testing_property_methods_table_port_enable].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_module_testing_property_methods_table_port_enable].values, dnxc_data_value_t, submodule_data->tables[dnx_data_module_testing_property_methods_table_port_enable].nof_values, "_dnx_data_module_testing_property_methods_table_port_enable table values");
    submodule_data->tables[dnx_data_module_testing_property_methods_table_port_enable].values[0].name = "val";
    submodule_data->tables[dnx_data_module_testing_property_methods_table_port_enable].values[0].type = "uint32";
    submodule_data->tables[dnx_data_module_testing_property_methods_table_port_enable].values[0].doc = "test port enable method";
    submodule_data->tables[dnx_data_module_testing_property_methods_table_port_enable].values[0].offset = UTILEX_OFFSETOF(dnx_data_module_testing_property_methods_port_enable_t, val);

    
    submodule_data->tables[dnx_data_module_testing_property_methods_table_suffix_enable].name = "suffix_enable";
    submodule_data->tables[dnx_data_module_testing_property_methods_table_suffix_enable].doc = "test all supported types";
    submodule_data->tables[dnx_data_module_testing_property_methods_table_suffix_enable].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_module_testing_property_methods_table_suffix_enable].size_of_values = sizeof(dnx_data_module_testing_property_methods_suffix_enable_t);
    submodule_data->tables[dnx_data_module_testing_property_methods_table_suffix_enable].entry_get = dnx_data_module_testing_property_methods_suffix_enable_entry_str_get;

    
    submodule_data->tables[dnx_data_module_testing_property_methods_table_suffix_enable].nof_keys = 1;
    submodule_data->tables[dnx_data_module_testing_property_methods_table_suffix_enable].keys[0].name = "link";
    submodule_data->tables[dnx_data_module_testing_property_methods_table_suffix_enable].keys[0].doc = "link id";

    
    submodule_data->tables[dnx_data_module_testing_property_methods_table_suffix_enable].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_module_testing_property_methods_table_suffix_enable].values, dnxc_data_value_t, submodule_data->tables[dnx_data_module_testing_property_methods_table_suffix_enable].nof_values, "_dnx_data_module_testing_property_methods_table_suffix_enable table values");
    submodule_data->tables[dnx_data_module_testing_property_methods_table_suffix_enable].values[0].name = "val";
    submodule_data->tables[dnx_data_module_testing_property_methods_table_suffix_enable].values[0].type = "uint32";
    submodule_data->tables[dnx_data_module_testing_property_methods_table_suffix_enable].values[0].doc = "test suffix enable method";
    submodule_data->tables[dnx_data_module_testing_property_methods_table_suffix_enable].values[0].offset = UTILEX_OFFSETOF(dnx_data_module_testing_property_methods_suffix_enable_t, val);

    
    submodule_data->tables[dnx_data_module_testing_property_methods_table_range].name = "range";
    submodule_data->tables[dnx_data_module_testing_property_methods_table_range].doc = "test prop range method types";
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
    submodule_data->tables[dnx_data_module_testing_property_methods_table_port_range].doc = "test prop port range method types";
    submodule_data->tables[dnx_data_module_testing_property_methods_table_port_range].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_module_testing_property_methods_table_port_range].size_of_values = sizeof(dnx_data_module_testing_property_methods_port_range_t);
    submodule_data->tables[dnx_data_module_testing_property_methods_table_port_range].entry_get = dnx_data_module_testing_property_methods_port_range_entry_str_get;

    
    submodule_data->tables[dnx_data_module_testing_property_methods_table_port_range].nof_keys = 1;
    submodule_data->tables[dnx_data_module_testing_property_methods_table_port_range].keys[0].name = "port";
    submodule_data->tables[dnx_data_module_testing_property_methods_table_port_range].keys[0].doc = "logical port";

    
    submodule_data->tables[dnx_data_module_testing_property_methods_table_port_range].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_module_testing_property_methods_table_port_range].values, dnxc_data_value_t, submodule_data->tables[dnx_data_module_testing_property_methods_table_port_range].nof_values, "_dnx_data_module_testing_property_methods_table_port_range table values");
    submodule_data->tables[dnx_data_module_testing_property_methods_table_port_range].values[0].name = "val";
    submodule_data->tables[dnx_data_module_testing_property_methods_table_port_range].values[0].type = "uint32";
    submodule_data->tables[dnx_data_module_testing_property_methods_table_port_range].values[0].doc = "see table";
    submodule_data->tables[dnx_data_module_testing_property_methods_table_port_range].values[0].offset = UTILEX_OFFSETOF(dnx_data_module_testing_property_methods_port_range_t, val);

    
    submodule_data->tables[dnx_data_module_testing_property_methods_table_suffix_range].name = "suffix_range";
    submodule_data->tables[dnx_data_module_testing_property_methods_table_suffix_range].doc = "test prop suffix range method types";
    submodule_data->tables[dnx_data_module_testing_property_methods_table_suffix_range].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_module_testing_property_methods_table_suffix_range].size_of_values = sizeof(dnx_data_module_testing_property_methods_suffix_range_t);
    submodule_data->tables[dnx_data_module_testing_property_methods_table_suffix_range].entry_get = dnx_data_module_testing_property_methods_suffix_range_entry_str_get;

    
    submodule_data->tables[dnx_data_module_testing_property_methods_table_suffix_range].nof_keys = 1;
    submodule_data->tables[dnx_data_module_testing_property_methods_table_suffix_range].keys[0].name = "link";
    submodule_data->tables[dnx_data_module_testing_property_methods_table_suffix_range].keys[0].doc = "link id";

    
    submodule_data->tables[dnx_data_module_testing_property_methods_table_suffix_range].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_module_testing_property_methods_table_suffix_range].values, dnxc_data_value_t, submodule_data->tables[dnx_data_module_testing_property_methods_table_suffix_range].nof_values, "_dnx_data_module_testing_property_methods_table_suffix_range table values");
    submodule_data->tables[dnx_data_module_testing_property_methods_table_suffix_range].values[0].name = "val";
    submodule_data->tables[dnx_data_module_testing_property_methods_table_suffix_range].values[0].type = "uint32";
    submodule_data->tables[dnx_data_module_testing_property_methods_table_suffix_range].values[0].doc = "see table";
    submodule_data->tables[dnx_data_module_testing_property_methods_table_suffix_range].values[0].offset = UTILEX_OFFSETOF(dnx_data_module_testing_property_methods_suffix_range_t, val);

    
    submodule_data->tables[dnx_data_module_testing_property_methods_table_direct_map].name = "direct_map";
    submodule_data->tables[dnx_data_module_testing_property_methods_table_direct_map].doc = "test prop direct map method types";
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
    submodule_data->tables[dnx_data_module_testing_property_methods_table_port_direct_map].doc = "test prop port direct map method types";
    submodule_data->tables[dnx_data_module_testing_property_methods_table_port_direct_map].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_module_testing_property_methods_table_port_direct_map].size_of_values = sizeof(dnx_data_module_testing_property_methods_port_direct_map_t);
    submodule_data->tables[dnx_data_module_testing_property_methods_table_port_direct_map].entry_get = dnx_data_module_testing_property_methods_port_direct_map_entry_str_get;

    
    submodule_data->tables[dnx_data_module_testing_property_methods_table_port_direct_map].nof_keys = 1;
    submodule_data->tables[dnx_data_module_testing_property_methods_table_port_direct_map].keys[0].name = "port";
    submodule_data->tables[dnx_data_module_testing_property_methods_table_port_direct_map].keys[0].doc = "logical port";

    
    submodule_data->tables[dnx_data_module_testing_property_methods_table_port_direct_map].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_module_testing_property_methods_table_port_direct_map].values, dnxc_data_value_t, submodule_data->tables[dnx_data_module_testing_property_methods_table_port_direct_map].nof_values, "_dnx_data_module_testing_property_methods_table_port_direct_map table values");
    submodule_data->tables[dnx_data_module_testing_property_methods_table_port_direct_map].values[0].name = "val";
    submodule_data->tables[dnx_data_module_testing_property_methods_table_port_direct_map].values[0].type = "uint32";
    submodule_data->tables[dnx_data_module_testing_property_methods_table_port_direct_map].values[0].doc = "see table";
    submodule_data->tables[dnx_data_module_testing_property_methods_table_port_direct_map].values[0].offset = UTILEX_OFFSETOF(dnx_data_module_testing_property_methods_port_direct_map_t, val);

    
    submodule_data->tables[dnx_data_module_testing_property_methods_table_suffix_direct_map].name = "suffix_direct_map";
    submodule_data->tables[dnx_data_module_testing_property_methods_table_suffix_direct_map].doc = "test prop suffix direct map method types";
    submodule_data->tables[dnx_data_module_testing_property_methods_table_suffix_direct_map].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_module_testing_property_methods_table_suffix_direct_map].size_of_values = sizeof(dnx_data_module_testing_property_methods_suffix_direct_map_t);
    submodule_data->tables[dnx_data_module_testing_property_methods_table_suffix_direct_map].entry_get = dnx_data_module_testing_property_methods_suffix_direct_map_entry_str_get;

    
    submodule_data->tables[dnx_data_module_testing_property_methods_table_suffix_direct_map].nof_keys = 1;
    submodule_data->tables[dnx_data_module_testing_property_methods_table_suffix_direct_map].keys[0].name = "link";
    submodule_data->tables[dnx_data_module_testing_property_methods_table_suffix_direct_map].keys[0].doc = "link id";

    
    submodule_data->tables[dnx_data_module_testing_property_methods_table_suffix_direct_map].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_module_testing_property_methods_table_suffix_direct_map].values, dnxc_data_value_t, submodule_data->tables[dnx_data_module_testing_property_methods_table_suffix_direct_map].nof_values, "_dnx_data_module_testing_property_methods_table_suffix_direct_map table values");
    submodule_data->tables[dnx_data_module_testing_property_methods_table_suffix_direct_map].values[0].name = "val";
    submodule_data->tables[dnx_data_module_testing_property_methods_table_suffix_direct_map].values[0].type = "uint32";
    submodule_data->tables[dnx_data_module_testing_property_methods_table_suffix_direct_map].values[0].doc = "see table";
    submodule_data->tables[dnx_data_module_testing_property_methods_table_suffix_direct_map].values[0].offset = UTILEX_OFFSETOF(dnx_data_module_testing_property_methods_suffix_direct_map_t, val);

    
    submodule_data->tables[dnx_data_module_testing_property_methods_table_custom].name = "custom";
    submodule_data->tables[dnx_data_module_testing_property_methods_table_custom].doc = "test prop custom method types";
    submodule_data->tables[dnx_data_module_testing_property_methods_table_custom].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_module_testing_property_methods_table_custom].size_of_values = sizeof(dnx_data_module_testing_property_methods_custom_t);
    submodule_data->tables[dnx_data_module_testing_property_methods_table_custom].entry_get = dnx_data_module_testing_property_methods_custom_entry_str_get;

    
    submodule_data->tables[dnx_data_module_testing_property_methods_table_custom].nof_keys = 2;
    submodule_data->tables[dnx_data_module_testing_property_methods_table_custom].keys[0].name = "link";
    submodule_data->tables[dnx_data_module_testing_property_methods_table_custom].keys[0].doc = "link id";
    submodule_data->tables[dnx_data_module_testing_property_methods_table_custom].keys[1].name = "pipe";
    submodule_data->tables[dnx_data_module_testing_property_methods_table_custom].keys[1].doc = "pipe id";

    
    submodule_data->tables[dnx_data_module_testing_property_methods_table_custom].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_module_testing_property_methods_table_custom].values, dnxc_data_value_t, submodule_data->tables[dnx_data_module_testing_property_methods_table_custom].nof_values, "_dnx_data_module_testing_property_methods_table_custom table values");
    submodule_data->tables[dnx_data_module_testing_property_methods_table_custom].values[0].name = "val";
    submodule_data->tables[dnx_data_module_testing_property_methods_table_custom].values[0].type = "uint32";
    submodule_data->tables[dnx_data_module_testing_property_methods_table_custom].values[0].doc = "see table";
    submodule_data->tables[dnx_data_module_testing_property_methods_table_custom].values[0].offset = UTILEX_OFFSETOF(dnx_data_module_testing_property_methods_custom_t, val);

    
    submodule_data->tables[dnx_data_module_testing_property_methods_table_pbmp].name = "pbmp";
    submodule_data->tables[dnx_data_module_testing_property_methods_table_pbmp].doc = "test pbmp property moethod";
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
    submodule_data->tables[dnx_data_module_testing_property_methods_table_port_pbmp].doc = "test port pbmp property moethod";
    submodule_data->tables[dnx_data_module_testing_property_methods_table_port_pbmp].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_module_testing_property_methods_table_port_pbmp].size_of_values = sizeof(dnx_data_module_testing_property_methods_port_pbmp_t);
    submodule_data->tables[dnx_data_module_testing_property_methods_table_port_pbmp].entry_get = dnx_data_module_testing_property_methods_port_pbmp_entry_str_get;

    
    submodule_data->tables[dnx_data_module_testing_property_methods_table_port_pbmp].nof_keys = 1;
    submodule_data->tables[dnx_data_module_testing_property_methods_table_port_pbmp].keys[0].name = "port";
    submodule_data->tables[dnx_data_module_testing_property_methods_table_port_pbmp].keys[0].doc = "logical port number";

    
    submodule_data->tables[dnx_data_module_testing_property_methods_table_port_pbmp].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_module_testing_property_methods_table_port_pbmp].values, dnxc_data_value_t, submodule_data->tables[dnx_data_module_testing_property_methods_table_port_pbmp].nof_values, "_dnx_data_module_testing_property_methods_table_port_pbmp table values");
    submodule_data->tables[dnx_data_module_testing_property_methods_table_port_pbmp].values[0].name = "val";
    submodule_data->tables[dnx_data_module_testing_property_methods_table_port_pbmp].values[0].type = "bcm_pbmp_t";
    submodule_data->tables[dnx_data_module_testing_property_methods_table_port_pbmp].values[0].doc = "see table";
    submodule_data->tables[dnx_data_module_testing_property_methods_table_port_pbmp].values[0].offset = UTILEX_OFFSETOF(dnx_data_module_testing_property_methods_port_pbmp_t, val);

    
    submodule_data->tables[dnx_data_module_testing_property_methods_table_suffix_pbmp].name = "suffix_pbmp";
    submodule_data->tables[dnx_data_module_testing_property_methods_table_suffix_pbmp].doc = "test suffix pbmp property moethod";
    submodule_data->tables[dnx_data_module_testing_property_methods_table_suffix_pbmp].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_module_testing_property_methods_table_suffix_pbmp].size_of_values = sizeof(dnx_data_module_testing_property_methods_suffix_pbmp_t);
    submodule_data->tables[dnx_data_module_testing_property_methods_table_suffix_pbmp].entry_get = dnx_data_module_testing_property_methods_suffix_pbmp_entry_str_get;

    
    submodule_data->tables[dnx_data_module_testing_property_methods_table_suffix_pbmp].nof_keys = 1;
    submodule_data->tables[dnx_data_module_testing_property_methods_table_suffix_pbmp].keys[0].name = "index";
    submodule_data->tables[dnx_data_module_testing_property_methods_table_suffix_pbmp].keys[0].doc = "index to test";

    
    submodule_data->tables[dnx_data_module_testing_property_methods_table_suffix_pbmp].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_module_testing_property_methods_table_suffix_pbmp].values, dnxc_data_value_t, submodule_data->tables[dnx_data_module_testing_property_methods_table_suffix_pbmp].nof_values, "_dnx_data_module_testing_property_methods_table_suffix_pbmp table values");
    submodule_data->tables[dnx_data_module_testing_property_methods_table_suffix_pbmp].values[0].name = "val";
    submodule_data->tables[dnx_data_module_testing_property_methods_table_suffix_pbmp].values[0].type = "bcm_pbmp_t";
    submodule_data->tables[dnx_data_module_testing_property_methods_table_suffix_pbmp].values[0].doc = "see table";
    submodule_data->tables[dnx_data_module_testing_property_methods_table_suffix_pbmp].values[0].offset = UTILEX_OFFSETOF(dnx_data_module_testing_property_methods_suffix_pbmp_t, val);

    
    submodule_data->tables[dnx_data_module_testing_property_methods_table_str].name = "str";
    submodule_data->tables[dnx_data_module_testing_property_methods_table_str].doc = "test str property method";
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
    submodule_data->tables[dnx_data_module_testing_property_methods_table_port_str].doc = "test port str property moethod";
    submodule_data->tables[dnx_data_module_testing_property_methods_table_port_str].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_module_testing_property_methods_table_port_str].size_of_values = sizeof(dnx_data_module_testing_property_methods_port_str_t);
    submodule_data->tables[dnx_data_module_testing_property_methods_table_port_str].entry_get = dnx_data_module_testing_property_methods_port_str_entry_str_get;

    
    submodule_data->tables[dnx_data_module_testing_property_methods_table_port_str].nof_keys = 1;
    submodule_data->tables[dnx_data_module_testing_property_methods_table_port_str].keys[0].name = "port";
    submodule_data->tables[dnx_data_module_testing_property_methods_table_port_str].keys[0].doc = "logical port number";

    
    submodule_data->tables[dnx_data_module_testing_property_methods_table_port_str].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_module_testing_property_methods_table_port_str].values, dnxc_data_value_t, submodule_data->tables[dnx_data_module_testing_property_methods_table_port_str].nof_values, "_dnx_data_module_testing_property_methods_table_port_str table values");
    submodule_data->tables[dnx_data_module_testing_property_methods_table_port_str].values[0].name = "val";
    submodule_data->tables[dnx_data_module_testing_property_methods_table_port_str].values[0].type = "char *";
    submodule_data->tables[dnx_data_module_testing_property_methods_table_port_str].values[0].doc = "see table";
    submodule_data->tables[dnx_data_module_testing_property_methods_table_port_str].values[0].offset = UTILEX_OFFSETOF(dnx_data_module_testing_property_methods_port_str_t, val);

    
    submodule_data->tables[dnx_data_module_testing_property_methods_table_suffix_str].name = "suffix_str";
    submodule_data->tables[dnx_data_module_testing_property_methods_table_suffix_str].doc = "test suffix str property moethod";
    submodule_data->tables[dnx_data_module_testing_property_methods_table_suffix_str].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_module_testing_property_methods_table_suffix_str].size_of_values = sizeof(dnx_data_module_testing_property_methods_suffix_str_t);
    submodule_data->tables[dnx_data_module_testing_property_methods_table_suffix_str].entry_get = dnx_data_module_testing_property_methods_suffix_str_entry_str_get;

    
    submodule_data->tables[dnx_data_module_testing_property_methods_table_suffix_str].nof_keys = 1;
    submodule_data->tables[dnx_data_module_testing_property_methods_table_suffix_str].keys[0].name = "index";
    submodule_data->tables[dnx_data_module_testing_property_methods_table_suffix_str].keys[0].doc = "index to test";

    
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
    submodule_data->doc = "test pointers to dnx data and arithmetic operations";
    
    submodule_data->nof_features = _dnx_data_module_testing_data_pointers_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data module_testing data_pointers features");

    
    submodule_data->nof_defines = _dnx_data_module_testing_data_pointers_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data module_testing data_pointers defines");

    submodule_data->defines[dnx_data_module_testing_data_pointers_define_cores].name = "cores";
    submodule_data->defines[dnx_data_module_testing_data_pointers_define_cores].doc = "equal to num of cores";
    
    submodule_data->defines[dnx_data_module_testing_data_pointers_define_cores].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_module_testing_data_pointers_define_add_cores].name = "add_cores";
    submodule_data->defines[dnx_data_module_testing_data_pointers_define_add_cores].doc = "equal to num of cores + 1";
    
    submodule_data->defines[dnx_data_module_testing_data_pointers_define_add_cores].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_module_testing_data_pointers_define_mul_cores].name = "mul_cores";
    submodule_data->defines[dnx_data_module_testing_data_pointers_define_mul_cores].doc = "equal to num of cores * 3";
    
    submodule_data->defines[dnx_data_module_testing_data_pointers_define_mul_cores].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_module_testing_data_pointers_define_div_cores].name = "div_cores";
    submodule_data->defines[dnx_data_module_testing_data_pointers_define_div_cores].doc = "equal to num of cores / 2";
    
    submodule_data->defines[dnx_data_module_testing_data_pointers_define_div_cores].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_module_testing_data_pointers_define_op_cores].name = "op_cores";
    submodule_data->defines[dnx_data_module_testing_data_pointers_define_op_cores].doc = "equal to num of cores * 3 + 2 / 2 ";
    
    submodule_data->defines[dnx_data_module_testing_data_pointers_define_op_cores].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_module_testing_data_pointers_define_num_cores].name = "num_cores";
    submodule_data->defines[dnx_data_module_testing_data_pointers_define_num_cores].doc = "equal to num of cores";
    
    submodule_data->defines[dnx_data_module_testing_data_pointers_define_num_cores].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_module_testing_data_pointers_define_num_add_cores].name = "num_add_cores";
    submodule_data->defines[dnx_data_module_testing_data_pointers_define_num_add_cores].doc = "equal to num of cores + 1";
    
    submodule_data->defines[dnx_data_module_testing_data_pointers_define_num_add_cores].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_module_testing_data_pointers_define_num_mul_cores].name = "num_mul_cores";
    submodule_data->defines[dnx_data_module_testing_data_pointers_define_num_mul_cores].doc = "equal to num of cores * 3";
    
    submodule_data->defines[dnx_data_module_testing_data_pointers_define_num_mul_cores].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_module_testing_data_pointers_define_num_div_cores].name = "num_div_cores";
    submodule_data->defines[dnx_data_module_testing_data_pointers_define_num_div_cores].doc = "equal to num of cores / 2";
    
    submodule_data->defines[dnx_data_module_testing_data_pointers_define_num_div_cores].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_module_testing_data_pointers_define_num_op_cores].name = "num_op_cores";
    submodule_data->defines[dnx_data_module_testing_data_pointers_define_num_op_cores].doc = "equal to num of cores * 3 + 2 / 2 ";
    
    submodule_data->defines[dnx_data_module_testing_data_pointers_define_num_op_cores].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_module_testing_data_pointers_define_num_last_cores].name = "num_last_cores";
    submodule_data->defines[dnx_data_module_testing_data_pointers_define_num_last_cores].doc = "equal to num of cores";
    
    submodule_data->defines[dnx_data_module_testing_data_pointers_define_num_last_cores].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_module_testing_data_pointers_define_num_table_cores].name = "num_table_cores";
    submodule_data->defines[dnx_data_module_testing_data_pointers_define_num_table_cores].doc = "pointer to a table entry";
    
    submodule_data->defines[dnx_data_module_testing_data_pointers_define_num_table_cores].flags |= DNXC_DATA_F_NUMERIC;

    
    submodule_data->nof_tables = _dnx_data_module_testing_data_pointers_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data module_testing data_pointers tables");

    
    submodule_data->tables[dnx_data_module_testing_data_pointers_table_table_pointers].name = "table_pointers";
    submodule_data->tables[dnx_data_module_testing_data_pointers_table_table_pointers].doc = "test key using data pointer";
    submodule_data->tables[dnx_data_module_testing_data_pointers_table_table_pointers].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_module_testing_data_pointers_table_table_pointers].size_of_values = sizeof(dnx_data_module_testing_data_pointers_table_pointers_t);
    submodule_data->tables[dnx_data_module_testing_data_pointers_table_table_pointers].entry_get = dnx_data_module_testing_data_pointers_table_pointers_entry_str_get;

    
    submodule_data->tables[dnx_data_module_testing_data_pointers_table_table_pointers].nof_keys = 1;
    submodule_data->tables[dnx_data_module_testing_data_pointers_table_table_pointers].keys[0].name = "index";
    submodule_data->tables[dnx_data_module_testing_data_pointers_table_table_pointers].keys[0].doc = "size of cores";

    
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
    submodule_data->doc = "test dbal db init procedure";
    
    submodule_data->nof_features = _dnx_data_module_testing_dbal_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data module_testing dbal features");

    
    submodule_data->nof_defines = _dnx_data_module_testing_dbal_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data module_testing dbal defines");

    submodule_data->defines[dnx_data_module_testing_dbal_define_vrf_field_size].name = "vrf_field_size";
    submodule_data->defines[dnx_data_module_testing_dbal_define_vrf_field_size].doc = "field size test";
    
    submodule_data->defines[dnx_data_module_testing_dbal_define_vrf_field_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_module_testing_dbal_define_field_define_example].name = "field_define_example";
    submodule_data->defines[dnx_data_module_testing_dbal_define_field_define_example].doc = "field define example";
    
    submodule_data->defines[dnx_data_module_testing_dbal_define_field_define_example].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_module_testing_dbal_define_res_max_val].name = "res_max_val";
    submodule_data->defines[dnx_data_module_testing_dbal_define_res_max_val].doc = "usage example";
    
    submodule_data->defines[dnx_data_module_testing_dbal_define_res_max_val].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_module_testing_dbal_define_res_field_size].name = "res_field_size";
    submodule_data->defines[dnx_data_module_testing_dbal_define_res_field_size].doc = "usage example";
    
    submodule_data->defines[dnx_data_module_testing_dbal_define_res_field_size].flags |= DNXC_DATA_F_DEFINE;

    
    submodule_data->nof_tables = _dnx_data_module_testing_dbal_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data module_testing dbal tables");

    
    submodule_data->tables[dnx_data_module_testing_dbal_table_all].name = "all";
    submodule_data->tables[dnx_data_module_testing_dbal_table_all].doc = "test all supported types";
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
    SHR_IF_ERR_EXIT(dnx_data_module_testing_types_init(unit, &module_data->submodules[dnx_data_module_testing_submodule_types]));
    SHR_IF_ERR_EXIT(dnx_data_module_testing_property_methods_init(unit, &module_data->submodules[dnx_data_module_testing_submodule_property_methods]));
    SHR_IF_ERR_EXIT(dnx_data_module_testing_data_pointers_init(unit, &module_data->submodules[dnx_data_module_testing_submodule_data_pointers]));
    SHR_IF_ERR_EXIT(dnx_data_module_testing_dbal_init(unit, &module_data->submodules[dnx_data_module_testing_submodule_dbal]));
    
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
    }
    else
    if (dnxc_data_mgmt_is_j2c_a1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_module_testing_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_q2a_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_module_testing_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_q2a_b0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_module_testing_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_q2a_b1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_module_testing_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_j2p_a0(unit))
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

