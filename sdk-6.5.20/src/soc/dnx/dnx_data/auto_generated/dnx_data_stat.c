

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_STAT

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_stat.h>



extern shr_error_e jr2_a0_data_stat_attach(
    int unit);
extern shr_error_e jr2_b0_data_stat_attach(
    int unit);
extern shr_error_e j2c_a0_data_stat_attach(
    int unit);
extern shr_error_e q2a_a0_data_stat_attach(
    int unit);
extern shr_error_e j2p_a0_data_stat_attach(
    int unit);



static shr_error_e
dnx_data_stat_stat_pp_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "stat_pp";
    submodule_data->doc = "general stat data";
    
    submodule_data->nof_features = _dnx_data_stat_stat_pp_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data stat stat_pp features");

    submodule_data->features[dnx_data_stat_stat_pp_full_stat_support].name = "full_stat_support";
    submodule_data->features[dnx_data_stat_stat_pp_full_stat_support].doc = "Full stat support is present";
    submodule_data->features[dnx_data_stat_stat_pp_full_stat_support].flags |= DNXC_DATA_F_FEATURE;

    
    submodule_data->nof_defines = _dnx_data_stat_stat_pp_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data stat stat_pp defines");

    submodule_data->defines[dnx_data_stat_stat_pp_define_max_irpp_profile_value].name = "max_irpp_profile_value";
    submodule_data->defines[dnx_data_stat_stat_pp_define_max_irpp_profile_value].doc = "max irpp profile size";
    submodule_data->defines[dnx_data_stat_stat_pp_define_max_irpp_profile_value].labels[0] = "j2p_notrev";
    
    submodule_data->defines[dnx_data_stat_stat_pp_define_max_irpp_profile_value].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_stat_stat_pp_define_max_etpp_profile_value].name = "max_etpp_profile_value";
    submodule_data->defines[dnx_data_stat_stat_pp_define_max_etpp_profile_value].doc = "max etpp profile size";
    submodule_data->defines[dnx_data_stat_stat_pp_define_max_etpp_profile_value].labels[0] = "j2p_notrev";
    
    submodule_data->defines[dnx_data_stat_stat_pp_define_max_etpp_profile_value].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_stat_stat_pp_define_max_etpp_counting_profile_value].name = "max_etpp_counting_profile_value";
    submodule_data->defines[dnx_data_stat_stat_pp_define_max_etpp_counting_profile_value].doc = "max etpp counting profile size";
    submodule_data->defines[dnx_data_stat_stat_pp_define_max_etpp_counting_profile_value].labels[0] = "j2p_notrev";
    
    submodule_data->defines[dnx_data_stat_stat_pp_define_max_etpp_counting_profile_value].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_stat_stat_pp_define_max_etpp_metering_profile_value].name = "max_etpp_metering_profile_value";
    submodule_data->defines[dnx_data_stat_stat_pp_define_max_etpp_metering_profile_value].doc = "max etpp metering profile size";
    submodule_data->defines[dnx_data_stat_stat_pp_define_max_etpp_metering_profile_value].labels[0] = "j2p_notrev";
    
    submodule_data->defines[dnx_data_stat_stat_pp_define_max_etpp_metering_profile_value].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_stat_stat_pp_define_max_erpp_profile_value].name = "max_erpp_profile_value";
    submodule_data->defines[dnx_data_stat_stat_pp_define_max_erpp_profile_value].doc = "max erpp profile size";
    submodule_data->defines[dnx_data_stat_stat_pp_define_max_erpp_profile_value].labels[0] = "j2p_notrev";
    
    submodule_data->defines[dnx_data_stat_stat_pp_define_max_erpp_profile_value].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_stat_stat_pp_define_etpp_reversed_stat_cmd].name = "etpp_reversed_stat_cmd";
    submodule_data->defines[dnx_data_stat_stat_pp_define_etpp_reversed_stat_cmd].doc = "ETPP statistics are built wrong in FWD, ENCAP";
    submodule_data->defines[dnx_data_stat_stat_pp_define_etpp_reversed_stat_cmd].labels[0] = "j2p_notrev";
    
    submodule_data->defines[dnx_data_stat_stat_pp_define_etpp_reversed_stat_cmd].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_stat_stat_pp_define_etpp_enc_stat_cmd_is_reversed].name = "etpp_enc_stat_cmd_is_reversed";
    submodule_data->defines[dnx_data_stat_stat_pp_define_etpp_enc_stat_cmd_is_reversed].doc = "ETPP statistics are built wrong in FWD, ENCAP (some devices reverse it themselves for the trap)";
    submodule_data->defines[dnx_data_stat_stat_pp_define_etpp_enc_stat_cmd_is_reversed].labels[0] = "j2p_notrev";
    
    submodule_data->defines[dnx_data_stat_stat_pp_define_etpp_enc_stat_cmd_is_reversed].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_stat_stat_pp_define_etpp_egress_vsi_always_pushed].name = "etpp_egress_vsi_always_pushed";
    submodule_data->defines[dnx_data_stat_stat_pp_define_etpp_egress_vsi_always_pushed].doc = "ETPP ENCAP stages push VSI stats entry regardless of use_vsd enabler";
    submodule_data->defines[dnx_data_stat_stat_pp_define_etpp_egress_vsi_always_pushed].labels[0] = "j2p_notrev";
    
    submodule_data->defines[dnx_data_stat_stat_pp_define_etpp_egress_vsi_always_pushed].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_stat_stat_pp_define_irpp_start_profile_value].name = "irpp_start_profile_value";
    submodule_data->defines[dnx_data_stat_stat_pp_define_irpp_start_profile_value].doc = "irpp profile first index";
    submodule_data->defines[dnx_data_stat_stat_pp_define_irpp_start_profile_value].labels[0] = "j2p_notrev";
    
    submodule_data->defines[dnx_data_stat_stat_pp_define_irpp_start_profile_value].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_stat_stat_pp_define_erpp_start_profile_value].name = "erpp_start_profile_value";
    submodule_data->defines[dnx_data_stat_stat_pp_define_erpp_start_profile_value].doc = "erpp profile first index";
    submodule_data->defines[dnx_data_stat_stat_pp_define_erpp_start_profile_value].labels[0] = "j2p_notrev";
    
    submodule_data->defines[dnx_data_stat_stat_pp_define_erpp_start_profile_value].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_stat_stat_pp_define_etpp_start_profile_value].name = "etpp_start_profile_value";
    submodule_data->defines[dnx_data_stat_stat_pp_define_etpp_start_profile_value].doc = "etpp profile first index";
    submodule_data->defines[dnx_data_stat_stat_pp_define_etpp_start_profile_value].labels[0] = "j2p_notrev";
    
    submodule_data->defines[dnx_data_stat_stat_pp_define_etpp_start_profile_value].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_stat_stat_pp_define_etpp_counting_start_profile_value].name = "etpp_counting_start_profile_value";
    submodule_data->defines[dnx_data_stat_stat_pp_define_etpp_counting_start_profile_value].doc = "etpp counting profile first index";
    submodule_data->defines[dnx_data_stat_stat_pp_define_etpp_counting_start_profile_value].labels[0] = "j2p_notrev";
    
    submodule_data->defines[dnx_data_stat_stat_pp_define_etpp_counting_start_profile_value].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_stat_stat_pp_define_etpp_metering_start_profile_value].name = "etpp_metering_start_profile_value";
    submodule_data->defines[dnx_data_stat_stat_pp_define_etpp_metering_start_profile_value].doc = "etpp metering profile first index";
    submodule_data->defines[dnx_data_stat_stat_pp_define_etpp_metering_start_profile_value].labels[0] = "j2p_notrev";
    
    submodule_data->defines[dnx_data_stat_stat_pp_define_etpp_metering_start_profile_value].flags |= DNXC_DATA_F_DEFINE;

    
    submodule_data->nof_tables = _dnx_data_stat_stat_pp_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data stat stat_pp tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_stat_stat_pp_feature_get(
    int unit,
    dnx_data_stat_stat_pp_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_stat, dnx_data_stat_submodule_stat_pp, feature);
}


uint32
dnx_data_stat_stat_pp_max_irpp_profile_value_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_stat, dnx_data_stat_submodule_stat_pp, dnx_data_stat_stat_pp_define_max_irpp_profile_value);
}

uint32
dnx_data_stat_stat_pp_max_etpp_profile_value_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_stat, dnx_data_stat_submodule_stat_pp, dnx_data_stat_stat_pp_define_max_etpp_profile_value);
}

uint32
dnx_data_stat_stat_pp_max_etpp_counting_profile_value_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_stat, dnx_data_stat_submodule_stat_pp, dnx_data_stat_stat_pp_define_max_etpp_counting_profile_value);
}

uint32
dnx_data_stat_stat_pp_max_etpp_metering_profile_value_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_stat, dnx_data_stat_submodule_stat_pp, dnx_data_stat_stat_pp_define_max_etpp_metering_profile_value);
}

uint32
dnx_data_stat_stat_pp_max_erpp_profile_value_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_stat, dnx_data_stat_submodule_stat_pp, dnx_data_stat_stat_pp_define_max_erpp_profile_value);
}

uint32
dnx_data_stat_stat_pp_etpp_reversed_stat_cmd_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_stat, dnx_data_stat_submodule_stat_pp, dnx_data_stat_stat_pp_define_etpp_reversed_stat_cmd);
}

uint32
dnx_data_stat_stat_pp_etpp_enc_stat_cmd_is_reversed_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_stat, dnx_data_stat_submodule_stat_pp, dnx_data_stat_stat_pp_define_etpp_enc_stat_cmd_is_reversed);
}

uint32
dnx_data_stat_stat_pp_etpp_egress_vsi_always_pushed_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_stat, dnx_data_stat_submodule_stat_pp, dnx_data_stat_stat_pp_define_etpp_egress_vsi_always_pushed);
}

uint32
dnx_data_stat_stat_pp_irpp_start_profile_value_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_stat, dnx_data_stat_submodule_stat_pp, dnx_data_stat_stat_pp_define_irpp_start_profile_value);
}

uint32
dnx_data_stat_stat_pp_erpp_start_profile_value_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_stat, dnx_data_stat_submodule_stat_pp, dnx_data_stat_stat_pp_define_erpp_start_profile_value);
}

uint32
dnx_data_stat_stat_pp_etpp_start_profile_value_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_stat, dnx_data_stat_submodule_stat_pp, dnx_data_stat_stat_pp_define_etpp_start_profile_value);
}

uint32
dnx_data_stat_stat_pp_etpp_counting_start_profile_value_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_stat, dnx_data_stat_submodule_stat_pp, dnx_data_stat_stat_pp_define_etpp_counting_start_profile_value);
}

uint32
dnx_data_stat_stat_pp_etpp_metering_start_profile_value_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_stat, dnx_data_stat_submodule_stat_pp, dnx_data_stat_stat_pp_define_etpp_metering_start_profile_value);
}










static shr_error_e
dnx_data_stat_diag_counter_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "diag_counter";
    submodule_data->doc = "diag counter related infomation.";
    
    submodule_data->nof_features = _dnx_data_stat_diag_counter_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data stat diag_counter features");

    
    submodule_data->nof_defines = _dnx_data_stat_diag_counter_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data stat diag_counter defines");

    
    submodule_data->nof_tables = _dnx_data_stat_diag_counter_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data stat diag_counter tables");

    
    submodule_data->tables[dnx_data_stat_diag_counter_table_ovf_info].name = "ovf_info";
    submodule_data->tables[dnx_data_stat_diag_counter_table_ovf_info].doc = "irregular overfield infomation";
    submodule_data->tables[dnx_data_stat_diag_counter_table_ovf_info].labels[0] = "j2p_notrev";
    submodule_data->tables[dnx_data_stat_diag_counter_table_ovf_info].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_stat_diag_counter_table_ovf_info].size_of_values = sizeof(dnx_data_stat_diag_counter_ovf_info_t);
    submodule_data->tables[dnx_data_stat_diag_counter_table_ovf_info].entry_get = dnx_data_stat_diag_counter_ovf_info_entry_str_get;

    
    submodule_data->tables[dnx_data_stat_diag_counter_table_ovf_info].nof_keys = 1;
    submodule_data->tables[dnx_data_stat_diag_counter_table_ovf_info].keys[0].name = "index";
    submodule_data->tables[dnx_data_stat_diag_counter_table_ovf_info].keys[0].doc = "overflow field index";

    
    submodule_data->tables[dnx_data_stat_diag_counter_table_ovf_info].nof_values = 3;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_stat_diag_counter_table_ovf_info].values, dnxc_data_value_t, submodule_data->tables[dnx_data_stat_diag_counter_table_ovf_info].nof_values, "_dnx_data_stat_diag_counter_table_ovf_info table values");
    submodule_data->tables[dnx_data_stat_diag_counter_table_ovf_info].values[0].name = "reg";
    submodule_data->tables[dnx_data_stat_diag_counter_table_ovf_info].values[0].type = "soc_reg_t";
    submodule_data->tables[dnx_data_stat_diag_counter_table_ovf_info].values[0].doc = "register name for counter";
    submodule_data->tables[dnx_data_stat_diag_counter_table_ovf_info].values[0].offset = UTILEX_OFFSETOF(dnx_data_stat_diag_counter_ovf_info_t, reg);
    submodule_data->tables[dnx_data_stat_diag_counter_table_ovf_info].values[1].name = "cnt_field";
    submodule_data->tables[dnx_data_stat_diag_counter_table_ovf_info].values[1].type = "soc_field_t";
    submodule_data->tables[dnx_data_stat_diag_counter_table_ovf_info].values[1].doc = "cnt field name.";
    submodule_data->tables[dnx_data_stat_diag_counter_table_ovf_info].values[1].offset = UTILEX_OFFSETOF(dnx_data_stat_diag_counter_ovf_info_t, cnt_field);
    submodule_data->tables[dnx_data_stat_diag_counter_table_ovf_info].values[2].name = "ovf_field";
    submodule_data->tables[dnx_data_stat_diag_counter_table_ovf_info].values[2].type = "soc_field_t";
    submodule_data->tables[dnx_data_stat_diag_counter_table_ovf_info].values[2].doc = "overflow field name";
    submodule_data->tables[dnx_data_stat_diag_counter_table_ovf_info].values[2].offset = UTILEX_OFFSETOF(dnx_data_stat_diag_counter_ovf_info_t, ovf_field);


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_stat_diag_counter_feature_get(
    int unit,
    dnx_data_stat_diag_counter_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_stat, dnx_data_stat_submodule_diag_counter, feature);
}




const dnx_data_stat_diag_counter_ovf_info_t *
dnx_data_stat_diag_counter_ovf_info_get(
    int unit,
    int index)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_stat, dnx_data_stat_submodule_diag_counter, dnx_data_stat_diag_counter_table_ovf_info);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, index, 0);
    return (const dnx_data_stat_diag_counter_ovf_info_t *) data;

}


shr_error_e
dnx_data_stat_diag_counter_ovf_info_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_stat_diag_counter_ovf_info_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_stat, dnx_data_stat_submodule_diag_counter, dnx_data_stat_diag_counter_table_ovf_info);
    data = (const dnx_data_stat_diag_counter_ovf_info_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->reg);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->cnt_field);
            break;
        case 2:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->ovf_field);
            break;
    }

    SHR_FUNC_EXIT;
}


const dnxc_data_table_info_t *
dnx_data_stat_diag_counter_ovf_info_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_stat, dnx_data_stat_submodule_diag_counter, dnx_data_stat_diag_counter_table_ovf_info);

}






static shr_error_e
dnx_data_stat_drop_reasons_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "drop_reasons";
    submodule_data->doc = "tm drop reasons";
    
    submodule_data->nof_features = _dnx_data_stat_drop_reasons_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data stat drop_reasons features");

    
    submodule_data->nof_defines = _dnx_data_stat_drop_reasons_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data stat drop_reasons defines");

    submodule_data->defines[dnx_data_stat_drop_reasons_define_nof_drop_reasons].name = "nof_drop_reasons";
    submodule_data->defines[dnx_data_stat_drop_reasons_define_nof_drop_reasons].doc = "number of drop reasons";
    
    submodule_data->defines[dnx_data_stat_drop_reasons_define_nof_drop_reasons].flags |= DNXC_DATA_F_DEFINE;

    
    submodule_data->nof_tables = _dnx_data_stat_drop_reasons_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data stat drop_reasons tables");

    
    submodule_data->tables[dnx_data_stat_drop_reasons_table_group_drop_reason_index].name = "group_drop_reason_index";
    submodule_data->tables[dnx_data_stat_drop_reasons_table_group_drop_reason_index].doc = "mapping drop reasons group enum to hw bit index";
    submodule_data->tables[dnx_data_stat_drop_reasons_table_group_drop_reason_index].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_stat_drop_reasons_table_group_drop_reason_index].size_of_values = sizeof(dnx_data_stat_drop_reasons_group_drop_reason_index_t);
    submodule_data->tables[dnx_data_stat_drop_reasons_table_group_drop_reason_index].entry_get = dnx_data_stat_drop_reasons_group_drop_reason_index_entry_str_get;

    
    submodule_data->tables[dnx_data_stat_drop_reasons_table_group_drop_reason_index].nof_keys = 1;
    submodule_data->tables[dnx_data_stat_drop_reasons_table_group_drop_reason_index].keys[0].name = "group";
    submodule_data->tables[dnx_data_stat_drop_reasons_table_group_drop_reason_index].keys[0].doc = "group enum value";

    
    submodule_data->tables[dnx_data_stat_drop_reasons_table_group_drop_reason_index].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_stat_drop_reasons_table_group_drop_reason_index].values, dnxc_data_value_t, submodule_data->tables[dnx_data_stat_drop_reasons_table_group_drop_reason_index].nof_values, "_dnx_data_stat_drop_reasons_table_group_drop_reason_index table values");
    submodule_data->tables[dnx_data_stat_drop_reasons_table_group_drop_reason_index].values[0].name = "index";
    submodule_data->tables[dnx_data_stat_drop_reasons_table_group_drop_reason_index].values[0].type = "uint32";
    submodule_data->tables[dnx_data_stat_drop_reasons_table_group_drop_reason_index].values[0].doc = "hw group index used in dbal";
    submodule_data->tables[dnx_data_stat_drop_reasons_table_group_drop_reason_index].values[0].offset = UTILEX_OFFSETOF(dnx_data_stat_drop_reasons_group_drop_reason_index_t, index);

    
    submodule_data->tables[dnx_data_stat_drop_reasons_table_drop_reason_index].name = "drop_reason_index";
    submodule_data->tables[dnx_data_stat_drop_reasons_table_drop_reason_index].doc = "mapping drop reasons bit enum to hw bit index";
    submodule_data->tables[dnx_data_stat_drop_reasons_table_drop_reason_index].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_stat_drop_reasons_table_drop_reason_index].size_of_values = sizeof(dnx_data_stat_drop_reasons_drop_reason_index_t);
    submodule_data->tables[dnx_data_stat_drop_reasons_table_drop_reason_index].entry_get = dnx_data_stat_drop_reasons_drop_reason_index_entry_str_get;

    
    submodule_data->tables[dnx_data_stat_drop_reasons_table_drop_reason_index].nof_keys = 1;
    submodule_data->tables[dnx_data_stat_drop_reasons_table_drop_reason_index].keys[0].name = "drop_reason_bit";
    submodule_data->tables[dnx_data_stat_drop_reasons_table_drop_reason_index].keys[0].doc = "drop reason bit representative enum";

    
    submodule_data->tables[dnx_data_stat_drop_reasons_table_drop_reason_index].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_stat_drop_reasons_table_drop_reason_index].values, dnxc_data_value_t, submodule_data->tables[dnx_data_stat_drop_reasons_table_drop_reason_index].nof_values, "_dnx_data_stat_drop_reasons_table_drop_reason_index table values");
    submodule_data->tables[dnx_data_stat_drop_reasons_table_drop_reason_index].values[0].name = "index";
    submodule_data->tables[dnx_data_stat_drop_reasons_table_drop_reason_index].values[0].type = "uint32";
    submodule_data->tables[dnx_data_stat_drop_reasons_table_drop_reason_index].values[0].doc = "hw group index used in dbal";
    submodule_data->tables[dnx_data_stat_drop_reasons_table_drop_reason_index].values[0].offset = UTILEX_OFFSETOF(dnx_data_stat_drop_reasons_drop_reason_index_t, index);

    
    submodule_data->tables[dnx_data_stat_drop_reasons_table_drop_reasons_groups].name = "drop_reasons_groups";
    submodule_data->tables[dnx_data_stat_drop_reasons_table_drop_reasons_groups].doc = "default drop reasons groups";
    submodule_data->tables[dnx_data_stat_drop_reasons_table_drop_reasons_groups].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_stat_drop_reasons_table_drop_reasons_groups].size_of_values = sizeof(dnx_data_stat_drop_reasons_drop_reasons_groups_t);
    submodule_data->tables[dnx_data_stat_drop_reasons_table_drop_reasons_groups].entry_get = dnx_data_stat_drop_reasons_drop_reasons_groups_entry_str_get;

    
    submodule_data->tables[dnx_data_stat_drop_reasons_table_drop_reasons_groups].nof_keys = 1;
    submodule_data->tables[dnx_data_stat_drop_reasons_table_drop_reasons_groups].keys[0].name = "name";
    submodule_data->tables[dnx_data_stat_drop_reasons_table_drop_reasons_groups].keys[0].doc = "drop reasons group name";

    
    submodule_data->tables[dnx_data_stat_drop_reasons_table_drop_reasons_groups].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_stat_drop_reasons_table_drop_reasons_groups].values, dnxc_data_value_t, submodule_data->tables[dnx_data_stat_drop_reasons_table_drop_reasons_groups].nof_values, "_dnx_data_stat_drop_reasons_table_drop_reasons_groups table values");
    submodule_data->tables[dnx_data_stat_drop_reasons_table_drop_reasons_groups].values[0].name = "drop_reasons";
    submodule_data->tables[dnx_data_stat_drop_reasons_table_drop_reasons_groups].values[0].type = "bcm_cosq_drop_reason_t[DNX_DATA_MAX_STAT_DROP_REASONS_NOF_DROP_REASONS]";
    submodule_data->tables[dnx_data_stat_drop_reasons_table_drop_reasons_groups].values[0].doc = "default drop reasons";
    submodule_data->tables[dnx_data_stat_drop_reasons_table_drop_reasons_groups].values[0].offset = UTILEX_OFFSETOF(dnx_data_stat_drop_reasons_drop_reasons_groups_t, drop_reasons);


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_stat_drop_reasons_feature_get(
    int unit,
    dnx_data_stat_drop_reasons_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_stat, dnx_data_stat_submodule_drop_reasons, feature);
}


uint32
dnx_data_stat_drop_reasons_nof_drop_reasons_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_stat, dnx_data_stat_submodule_drop_reasons, dnx_data_stat_drop_reasons_define_nof_drop_reasons);
}



const dnx_data_stat_drop_reasons_group_drop_reason_index_t *
dnx_data_stat_drop_reasons_group_drop_reason_index_get(
    int unit,
    int group)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_stat, dnx_data_stat_submodule_drop_reasons, dnx_data_stat_drop_reasons_table_group_drop_reason_index);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, group, 0);
    return (const dnx_data_stat_drop_reasons_group_drop_reason_index_t *) data;

}

const dnx_data_stat_drop_reasons_drop_reason_index_t *
dnx_data_stat_drop_reasons_drop_reason_index_get(
    int unit,
    int drop_reason_bit)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_stat, dnx_data_stat_submodule_drop_reasons, dnx_data_stat_drop_reasons_table_drop_reason_index);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, drop_reason_bit, 0);
    return (const dnx_data_stat_drop_reasons_drop_reason_index_t *) data;

}

const dnx_data_stat_drop_reasons_drop_reasons_groups_t *
dnx_data_stat_drop_reasons_drop_reasons_groups_get(
    int unit,
    int name)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_stat, dnx_data_stat_submodule_drop_reasons, dnx_data_stat_drop_reasons_table_drop_reasons_groups);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, name, 0);
    return (const dnx_data_stat_drop_reasons_drop_reasons_groups_t *) data;

}


shr_error_e
dnx_data_stat_drop_reasons_group_drop_reason_index_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_stat_drop_reasons_group_drop_reason_index_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_stat, dnx_data_stat_submodule_drop_reasons, dnx_data_stat_drop_reasons_table_group_drop_reason_index);
    data = (const dnx_data_stat_drop_reasons_group_drop_reason_index_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->index);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_stat_drop_reasons_drop_reason_index_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_stat_drop_reasons_drop_reason_index_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_stat, dnx_data_stat_submodule_drop_reasons, dnx_data_stat_drop_reasons_table_drop_reason_index);
    data = (const dnx_data_stat_drop_reasons_drop_reason_index_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->index);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_stat_drop_reasons_drop_reasons_groups_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_stat_drop_reasons_drop_reasons_groups_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_stat, dnx_data_stat_submodule_drop_reasons, dnx_data_stat_drop_reasons_table_drop_reasons_groups);
    data = (const dnx_data_stat_drop_reasons_drop_reasons_groups_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            DNXC_DATA_MGMT_ARR_STR(buffer, DNX_DATA_MAX_STAT_DROP_REASONS_NOF_DROP_REASONS, data->drop_reasons);
            break;
    }

    SHR_FUNC_EXIT;
}


const dnxc_data_table_info_t *
dnx_data_stat_drop_reasons_group_drop_reason_index_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_stat, dnx_data_stat_submodule_drop_reasons, dnx_data_stat_drop_reasons_table_group_drop_reason_index);

}

const dnxc_data_table_info_t *
dnx_data_stat_drop_reasons_drop_reason_index_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_stat, dnx_data_stat_submodule_drop_reasons, dnx_data_stat_drop_reasons_table_drop_reason_index);

}

const dnxc_data_table_info_t *
dnx_data_stat_drop_reasons_drop_reasons_groups_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_stat, dnx_data_stat_submodule_drop_reasons, dnx_data_stat_drop_reasons_table_drop_reasons_groups);

}



shr_error_e
dnx_data_stat_init(
    int unit,
    dnxc_data_module_t *module_data)
{
    SHR_FUNC_INIT_VARS(unit);

    
    module_data->name = "stat";
    module_data->nof_submodules = _dnx_data_stat_submodule_nof;
    DNXC_DATA_ALLOC(module_data->submodules, dnxc_data_submodule_t, module_data->nof_submodules, "_dnxc_data stat submodules");

    
    SHR_IF_ERR_EXIT(dnx_data_stat_stat_pp_init(unit, &module_data->submodules[dnx_data_stat_submodule_stat_pp]));
    SHR_IF_ERR_EXIT(dnx_data_stat_diag_counter_init(unit, &module_data->submodules[dnx_data_stat_submodule_diag_counter]));
    SHR_IF_ERR_EXIT(dnx_data_stat_drop_reasons_init(unit, &module_data->submodules[dnx_data_stat_submodule_drop_reasons]));
    
    if (dnxc_data_mgmt_is_jr2_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_stat_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_jr2_b0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_stat_attach(unit));
        SHR_IF_ERR_EXIT(jr2_b0_data_stat_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_jr2_b1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_stat_attach(unit));
        SHR_IF_ERR_EXIT(jr2_b0_data_stat_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_j2c_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_stat_attach(unit));
        SHR_IF_ERR_EXIT(j2c_a0_data_stat_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_j2c_a1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_stat_attach(unit));
        SHR_IF_ERR_EXIT(j2c_a0_data_stat_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_q2a_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_stat_attach(unit));
        SHR_IF_ERR_EXIT(q2a_a0_data_stat_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_q2a_b0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_stat_attach(unit));
        SHR_IF_ERR_EXIT(q2a_a0_data_stat_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_q2a_b1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_stat_attach(unit));
        SHR_IF_ERR_EXIT(q2a_a0_data_stat_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_j2p_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_stat_attach(unit));
        SHR_IF_ERR_EXIT(j2p_a0_data_stat_attach(unit));
    }
    else
    {
        SHR_EXIT();
    }

exit:
    SHR_FUNC_EXIT;
}
#undef BSL_LOG_MODULE

