
/* *INDENT-OFF* */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2021 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_FLDPROCDNX

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_kleap_stage_info.h>



#ifdef BCM_DNX2_SUPPORT

extern shr_error_e jr2_a0_data_kleap_stage_info_attach(
    int unit);

#endif 



static shr_error_e
dnx_data_kleap_stage_info_kleap_stage_info_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "kleap_stage_info";
    submodule_data->doc = ".";
    
    submodule_data->nof_features = _dnx_data_kleap_stage_info_kleap_stage_info_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data kleap_stage_info kleap_stage_info features");

    
    submodule_data->nof_defines = _dnx_data_kleap_stage_info_kleap_stage_info_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data kleap_stage_info kleap_stage_info defines");

    submodule_data->defines[dnx_data_kleap_stage_info_kleap_stage_info_define_nof_kleap_stages].name = "nof_kleap_stages";
    submodule_data->defines[dnx_data_kleap_stage_info_kleap_stage_info_define_nof_kleap_stages].doc = "";
    
    submodule_data->defines[dnx_data_kleap_stage_info_kleap_stage_info_define_nof_kleap_stages].flags |= DNXC_DATA_F_NUMERIC;

    
    submodule_data->nof_tables = _dnx_data_kleap_stage_info_kleap_stage_info_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data kleap_stage_info kleap_stage_info tables");

    
    submodule_data->tables[dnx_data_kleap_stage_info_kleap_stage_info_table_info_per_stage].name = "info_per_stage";
    submodule_data->tables[dnx_data_kleap_stage_info_kleap_stage_info_table_info_per_stage].doc = "Info per stage";
    submodule_data->tables[dnx_data_kleap_stage_info_kleap_stage_info_table_info_per_stage].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_kleap_stage_info_kleap_stage_info_table_info_per_stage].size_of_values = sizeof(dnx_data_kleap_stage_info_kleap_stage_info_info_per_stage_t);
    submodule_data->tables[dnx_data_kleap_stage_info_kleap_stage_info_table_info_per_stage].entry_get = dnx_data_kleap_stage_info_kleap_stage_info_info_per_stage_entry_str_get;

    
    submodule_data->tables[dnx_data_kleap_stage_info_kleap_stage_info_table_info_per_stage].nof_keys = 1;
    submodule_data->tables[dnx_data_kleap_stage_info_kleap_stage_info_table_info_per_stage].keys[0].name = "stage_index";
    submodule_data->tables[dnx_data_kleap_stage_info_kleap_stage_info_table_info_per_stage].keys[0].doc = "Stage index";

    
    submodule_data->tables[dnx_data_kleap_stage_info_kleap_stage_info_table_info_per_stage].nof_values = 19;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_kleap_stage_info_kleap_stage_info_table_info_per_stage].values, dnxc_data_value_t, submodule_data->tables[dnx_data_kleap_stage_info_kleap_stage_info_table_info_per_stage].nof_values, "_dnx_data_kleap_stage_info_kleap_stage_info_table_info_per_stage table values");
    submodule_data->tables[dnx_data_kleap_stage_info_kleap_stage_info_table_info_per_stage].values[0].name = "stage_name";
    submodule_data->tables[dnx_data_kleap_stage_info_kleap_stage_info_table_info_per_stage].values[0].type = "char *";
    submodule_data->tables[dnx_data_kleap_stage_info_kleap_stage_info_table_info_per_stage].values[0].doc = "Stage name";
    submodule_data->tables[dnx_data_kleap_stage_info_kleap_stage_info_table_info_per_stage].values[0].offset = UTILEX_OFFSETOF(dnx_data_kleap_stage_info_kleap_stage_info_info_per_stage_t, stage_name);
    submodule_data->tables[dnx_data_kleap_stage_info_kleap_stage_info_table_info_per_stage].values[1].name = "type";
    submodule_data->tables[dnx_data_kleap_stage_info_kleap_stage_info_table_info_per_stage].values[1].type = "int";
    submodule_data->tables[dnx_data_kleap_stage_info_kleap_stage_info_table_info_per_stage].values[1].doc = "Single, Double, Invalid";
    submodule_data->tables[dnx_data_kleap_stage_info_kleap_stage_info_table_info_per_stage].values[1].offset = UTILEX_OFFSETOF(dnx_data_kleap_stage_info_kleap_stage_info_info_per_stage_t, type);
    submodule_data->tables[dnx_data_kleap_stage_info_kleap_stage_info_table_info_per_stage].values[2].name = "nof_sub_stages";
    submodule_data->tables[dnx_data_kleap_stage_info_kleap_stage_info_table_info_per_stage].values[2].type = "int";
    submodule_data->tables[dnx_data_kleap_stage_info_kleap_stage_info_table_info_per_stage].values[2].doc = "Single=1, Double=2, Invalid=0";
    submodule_data->tables[dnx_data_kleap_stage_info_kleap_stage_info_table_info_per_stage].values[2].offset = UTILEX_OFFSETOF(dnx_data_kleap_stage_info_kleap_stage_info_info_per_stage_t, nof_sub_stages);
    submodule_data->tables[dnx_data_kleap_stage_info_kleap_stage_info_table_info_per_stage].values[3].name = "nof_kbrs";
    submodule_data->tables[dnx_data_kleap_stage_info_kleap_stage_info_table_info_per_stage].values[3].type = "int";
    submodule_data->tables[dnx_data_kleap_stage_info_kleap_stage_info_table_info_per_stage].values[3].doc = "nof KBRs";
    submodule_data->tables[dnx_data_kleap_stage_info_kleap_stage_info_table_info_per_stage].values[3].offset = UTILEX_OFFSETOF(dnx_data_kleap_stage_info_kleap_stage_info_info_per_stage_t, nof_kbrs);
    submodule_data->tables[dnx_data_kleap_stage_info_kleap_stage_info_table_info_per_stage].values[4].name = "nof_ffc";
    submodule_data->tables[dnx_data_kleap_stage_info_kleap_stage_info_table_info_per_stage].values[4].type = "int";
    submodule_data->tables[dnx_data_kleap_stage_info_kleap_stage_info_table_info_per_stage].values[4].doc = "nof FFC";
    submodule_data->tables[dnx_data_kleap_stage_info_kleap_stage_info_table_info_per_stage].values[4].offset = UTILEX_OFFSETOF(dnx_data_kleap_stage_info_kleap_stage_info_info_per_stage_t, nof_ffc);
    submodule_data->tables[dnx_data_kleap_stage_info_kleap_stage_info_table_info_per_stage].values[5].name = "nof_ffc_g";
    submodule_data->tables[dnx_data_kleap_stage_info_kleap_stage_info_table_info_per_stage].values[5].type = "int";
    submodule_data->tables[dnx_data_kleap_stage_info_kleap_stage_info_table_info_per_stage].values[5].doc = "nof FFC Group";
    submodule_data->tables[dnx_data_kleap_stage_info_kleap_stage_info_table_info_per_stage].values[5].offset = UTILEX_OFFSETOF(dnx_data_kleap_stage_info_kleap_stage_info_info_per_stage_t, nof_ffc_g);
    submodule_data->tables[dnx_data_kleap_stage_info_kleap_stage_info_table_info_per_stage].values[6].name = "nof_pd";
    submodule_data->tables[dnx_data_kleap_stage_info_kleap_stage_info_table_info_per_stage].values[6].type = "int";
    submodule_data->tables[dnx_data_kleap_stage_info_kleap_stage_info_table_info_per_stage].values[6].doc = "nof PD";
    submodule_data->tables[dnx_data_kleap_stage_info_kleap_stage_info_table_info_per_stage].values[6].offset = UTILEX_OFFSETOF(dnx_data_kleap_stage_info_kleap_stage_info_info_per_stage_t, nof_pd);
    submodule_data->tables[dnx_data_kleap_stage_info_kleap_stage_info_table_info_per_stage].values[7].name = "dbal_table_resource_mapping";
    submodule_data->tables[dnx_data_kleap_stage_info_kleap_stage_info_table_info_per_stage].values[7].type = "int";
    submodule_data->tables[dnx_data_kleap_stage_info_kleap_stage_info_table_info_per_stage].values[7].doc = "dbal_table_resource_mapping";
    submodule_data->tables[dnx_data_kleap_stage_info_kleap_stage_info_table_info_per_stage].values[7].offset = UTILEX_OFFSETOF(dnx_data_kleap_stage_info_kleap_stage_info_info_per_stage_t, dbal_table_resource_mapping);
    submodule_data->tables[dnx_data_kleap_stage_info_kleap_stage_info_table_info_per_stage].values[8].name = "dbal_table_kbr_info";
    submodule_data->tables[dnx_data_kleap_stage_info_kleap_stage_info_table_info_per_stage].values[8].type = "int";
    submodule_data->tables[dnx_data_kleap_stage_info_kleap_stage_info_table_info_per_stage].values[8].doc = "dbal_table_kbr_info";
    submodule_data->tables[dnx_data_kleap_stage_info_kleap_stage_info_table_info_per_stage].values[8].offset = UTILEX_OFFSETOF(dnx_data_kleap_stage_info_kleap_stage_info_info_per_stage_t, dbal_table_kbr_info);
    submodule_data->tables[dnx_data_kleap_stage_info_kleap_stage_info_table_info_per_stage].values[9].name = "dbal_table_ffc_instruction";
    submodule_data->tables[dnx_data_kleap_stage_info_kleap_stage_info_table_info_per_stage].values[9].type = "int";
    submodule_data->tables[dnx_data_kleap_stage_info_kleap_stage_info_table_info_per_stage].values[9].doc = "dbal_table_ffc_instruction";
    submodule_data->tables[dnx_data_kleap_stage_info_kleap_stage_info_table_info_per_stage].values[9].offset = UTILEX_OFFSETOF(dnx_data_kleap_stage_info_kleap_stage_info_info_per_stage_t, dbal_table_ffc_instruction);
    submodule_data->tables[dnx_data_kleap_stage_info_kleap_stage_info_table_info_per_stage].values[10].name = "dbal_table_ffc_quad_is_acl";
    submodule_data->tables[dnx_data_kleap_stage_info_kleap_stage_info_table_info_per_stage].values[10].type = "int";
    submodule_data->tables[dnx_data_kleap_stage_info_kleap_stage_info_table_info_per_stage].values[10].doc = "dbal_table_ffc_quad_is_acl";
    submodule_data->tables[dnx_data_kleap_stage_info_kleap_stage_info_table_info_per_stage].values[10].offset = UTILEX_OFFSETOF(dnx_data_kleap_stage_info_kleap_stage_info_info_per_stage_t, dbal_table_ffc_quad_is_acl);
    submodule_data->tables[dnx_data_kleap_stage_info_kleap_stage_info_table_info_per_stage].values[11].name = "dbal_table_pd_info";
    submodule_data->tables[dnx_data_kleap_stage_info_kleap_stage_info_table_info_per_stage].values[11].type = "int";
    submodule_data->tables[dnx_data_kleap_stage_info_kleap_stage_info_table_info_per_stage].values[11].doc = "dbal_table_pd_info";
    submodule_data->tables[dnx_data_kleap_stage_info_kleap_stage_info_table_info_per_stage].values[11].offset = UTILEX_OFFSETOF(dnx_data_kleap_stage_info_kleap_stage_info_info_per_stage_t, dbal_table_pd_info);
    submodule_data->tables[dnx_data_kleap_stage_info_kleap_stage_info_table_info_per_stage].values[12].name = "dbal_kbr_idx_field_per_stage";
    submodule_data->tables[dnx_data_kleap_stage_info_kleap_stage_info_table_info_per_stage].values[12].type = "int";
    submodule_data->tables[dnx_data_kleap_stage_info_kleap_stage_info_table_info_per_stage].values[12].doc = "dbal_kbr_idx_field_per_stage";
    submodule_data->tables[dnx_data_kleap_stage_info_kleap_stage_info_table_info_per_stage].values[12].offset = UTILEX_OFFSETOF(dnx_data_kleap_stage_info_kleap_stage_info_info_per_stage_t, dbal_kbr_idx_field_per_stage);
    submodule_data->tables[dnx_data_kleap_stage_info_kleap_stage_info_table_info_per_stage].values[13].name = "dbal_context_profile_field_per_stage";
    submodule_data->tables[dnx_data_kleap_stage_info_kleap_stage_info_table_info_per_stage].values[13].type = "int";
    submodule_data->tables[dnx_data_kleap_stage_info_kleap_stage_info_table_info_per_stage].values[13].doc = "dbal_context_profile_field_per_stage";
    submodule_data->tables[dnx_data_kleap_stage_info_kleap_stage_info_table_info_per_stage].values[13].offset = UTILEX_OFFSETOF(dnx_data_kleap_stage_info_kleap_stage_info_info_per_stage_t, dbal_context_profile_field_per_stage);
    submodule_data->tables[dnx_data_kleap_stage_info_kleap_stage_info_table_info_per_stage].values[14].name = "dbal_ffc_instruction_field_per_stage";
    submodule_data->tables[dnx_data_kleap_stage_info_kleap_stage_info_table_info_per_stage].values[14].type = "int";
    submodule_data->tables[dnx_data_kleap_stage_info_kleap_stage_info_table_info_per_stage].values[14].doc = "dbal_ffc_instruction_field_per_stage";
    submodule_data->tables[dnx_data_kleap_stage_info_kleap_stage_info_table_info_per_stage].values[14].offset = UTILEX_OFFSETOF(dnx_data_kleap_stage_info_kleap_stage_info_info_per_stage_t, dbal_ffc_instruction_field_per_stage);
    submodule_data->tables[dnx_data_kleap_stage_info_kleap_stage_info_table_info_per_stage].values[15].name = "dbal_litrally_instruction_field_per_stage";
    submodule_data->tables[dnx_data_kleap_stage_info_kleap_stage_info_table_info_per_stage].values[15].type = "int";
    submodule_data->tables[dnx_data_kleap_stage_info_kleap_stage_info_table_info_per_stage].values[15].doc = "dbal_litrally_instruction_field_per_stage";
    submodule_data->tables[dnx_data_kleap_stage_info_kleap_stage_info_table_info_per_stage].values[15].offset = UTILEX_OFFSETOF(dnx_data_kleap_stage_info_kleap_stage_info_info_per_stage_t, dbal_litrally_instruction_field_per_stage);
    submodule_data->tables[dnx_data_kleap_stage_info_kleap_stage_info_table_info_per_stage].values[16].name = "dbal_relative_header_instruction_field_per_stage";
    submodule_data->tables[dnx_data_kleap_stage_info_kleap_stage_info_table_info_per_stage].values[16].type = "int";
    submodule_data->tables[dnx_data_kleap_stage_info_kleap_stage_info_table_info_per_stage].values[16].doc = "dbal_relative_header_instruction_field_per_stage";
    submodule_data->tables[dnx_data_kleap_stage_info_kleap_stage_info_table_info_per_stage].values[16].offset = UTILEX_OFFSETOF(dnx_data_kleap_stage_info_kleap_stage_info_info_per_stage_t, dbal_relative_header_instruction_field_per_stage);
    submodule_data->tables[dnx_data_kleap_stage_info_kleap_stage_info_table_info_per_stage].values[17].name = "dbal_header_instruction_field_per_stage";
    submodule_data->tables[dnx_data_kleap_stage_info_kleap_stage_info_table_info_per_stage].values[17].type = "int";
    submodule_data->tables[dnx_data_kleap_stage_info_kleap_stage_info_table_info_per_stage].values[17].doc = "dbal_header_instruction_field_per_stage";
    submodule_data->tables[dnx_data_kleap_stage_info_kleap_stage_info_table_info_per_stage].values[17].offset = UTILEX_OFFSETOF(dnx_data_kleap_stage_info_kleap_stage_info_info_per_stage_t, dbal_header_instruction_field_per_stage);
    submodule_data->tables[dnx_data_kleap_stage_info_kleap_stage_info_table_info_per_stage].values[18].name = "dbal_record_instruction_field_per_stage";
    submodule_data->tables[dnx_data_kleap_stage_info_kleap_stage_info_table_info_per_stage].values[18].type = "int";
    submodule_data->tables[dnx_data_kleap_stage_info_kleap_stage_info_table_info_per_stage].values[18].doc = "dbal_record_instruction_field_per_stage";
    submodule_data->tables[dnx_data_kleap_stage_info_kleap_stage_info_table_info_per_stage].values[18].offset = UTILEX_OFFSETOF(dnx_data_kleap_stage_info_kleap_stage_info_info_per_stage_t, dbal_record_instruction_field_per_stage);

    
    submodule_data->tables[dnx_data_kleap_stage_info_kleap_stage_info_table_info_per_stage_per_kbr].name = "info_per_stage_per_kbr";
    submodule_data->tables[dnx_data_kleap_stage_info_kleap_stage_info_table_info_per_stage_per_kbr].doc = "";
    submodule_data->tables[dnx_data_kleap_stage_info_kleap_stage_info_table_info_per_stage_per_kbr].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_kleap_stage_info_kleap_stage_info_table_info_per_stage_per_kbr].size_of_values = sizeof(dnx_data_kleap_stage_info_kleap_stage_info_info_per_stage_per_kbr_t);
    submodule_data->tables[dnx_data_kleap_stage_info_kleap_stage_info_table_info_per_stage_per_kbr].entry_get = dnx_data_kleap_stage_info_kleap_stage_info_info_per_stage_per_kbr_entry_str_get;

    
    submodule_data->tables[dnx_data_kleap_stage_info_kleap_stage_info_table_info_per_stage_per_kbr].nof_keys = 1;
    submodule_data->tables[dnx_data_kleap_stage_info_kleap_stage_info_table_info_per_stage_per_kbr].keys[0].name = "stage_index";
    submodule_data->tables[dnx_data_kleap_stage_info_kleap_stage_info_table_info_per_stage_per_kbr].keys[0].doc = "stage index";

    
    submodule_data->tables[dnx_data_kleap_stage_info_kleap_stage_info_table_info_per_stage_per_kbr].nof_values = 3;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_kleap_stage_info_kleap_stage_info_table_info_per_stage_per_kbr].values, dnxc_data_value_t, submodule_data->tables[dnx_data_kleap_stage_info_kleap_stage_info_table_info_per_stage_per_kbr].nof_values, "_dnx_data_kleap_stage_info_kleap_stage_info_table_info_per_stage_per_kbr table values");
    submodule_data->tables[dnx_data_kleap_stage_info_kleap_stage_info_table_info_per_stage_per_kbr].values[0].name = "nof_kbrs";
    submodule_data->tables[dnx_data_kleap_stage_info_kleap_stage_info_table_info_per_stage_per_kbr].values[0].type = "int";
    submodule_data->tables[dnx_data_kleap_stage_info_kleap_stage_info_table_info_per_stage_per_kbr].values[0].doc = "nof KBRs";
    submodule_data->tables[dnx_data_kleap_stage_info_kleap_stage_info_table_info_per_stage_per_kbr].values[0].offset = UTILEX_OFFSETOF(dnx_data_kleap_stage_info_kleap_stage_info_info_per_stage_per_kbr_t, nof_kbrs);
    submodule_data->tables[dnx_data_kleap_stage_info_kleap_stage_info_table_info_per_stage_per_kbr].values[1].name = "kbr2physical";
    submodule_data->tables[dnx_data_kleap_stage_info_kleap_stage_info_table_info_per_stage_per_kbr].values[1].type = "int[19]";
    submodule_data->tables[dnx_data_kleap_stage_info_kleap_stage_info_table_info_per_stage_per_kbr].values[1].doc = "";
    submodule_data->tables[dnx_data_kleap_stage_info_kleap_stage_info_table_info_per_stage_per_kbr].values[1].offset = UTILEX_OFFSETOF(dnx_data_kleap_stage_info_kleap_stage_info_info_per_stage_per_kbr_t, kbr2physical);
    submodule_data->tables[dnx_data_kleap_stage_info_kleap_stage_info_table_info_per_stage_per_kbr].values[2].name = "kbr2key_sig";
    submodule_data->tables[dnx_data_kleap_stage_info_kleap_stage_info_table_info_per_stage_per_kbr].values[2].type = "int[19]";
    submodule_data->tables[dnx_data_kleap_stage_info_kleap_stage_info_table_info_per_stage_per_kbr].values[2].doc = "";
    submodule_data->tables[dnx_data_kleap_stage_info_kleap_stage_info_table_info_per_stage_per_kbr].values[2].offset = UTILEX_OFFSETOF(dnx_data_kleap_stage_info_kleap_stage_info_info_per_stage_per_kbr_t, kbr2key_sig);

    
    submodule_data->tables[dnx_data_kleap_stage_info_kleap_stage_info_table_info_per_stage_per_pd].name = "info_per_stage_per_pd";
    submodule_data->tables[dnx_data_kleap_stage_info_kleap_stage_info_table_info_per_stage_per_pd].doc = "";
    submodule_data->tables[dnx_data_kleap_stage_info_kleap_stage_info_table_info_per_stage_per_pd].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_kleap_stage_info_kleap_stage_info_table_info_per_stage_per_pd].size_of_values = sizeof(dnx_data_kleap_stage_info_kleap_stage_info_info_per_stage_per_pd_t);
    submodule_data->tables[dnx_data_kleap_stage_info_kleap_stage_info_table_info_per_stage_per_pd].entry_get = dnx_data_kleap_stage_info_kleap_stage_info_info_per_stage_per_pd_entry_str_get;

    
    submodule_data->tables[dnx_data_kleap_stage_info_kleap_stage_info_table_info_per_stage_per_pd].nof_keys = 2;
    submodule_data->tables[dnx_data_kleap_stage_info_kleap_stage_info_table_info_per_stage_per_pd].keys[0].name = "stage_index";
    submodule_data->tables[dnx_data_kleap_stage_info_kleap_stage_info_table_info_per_stage_per_pd].keys[0].doc = "stage index";
    submodule_data->tables[dnx_data_kleap_stage_info_kleap_stage_info_table_info_per_stage_per_pd].keys[1].name = "pd_index";
    submodule_data->tables[dnx_data_kleap_stage_info_kleap_stage_info_table_info_per_stage_per_pd].keys[1].doc = "pd index";

    
    submodule_data->tables[dnx_data_kleap_stage_info_kleap_stage_info_table_info_per_stage_per_pd].nof_values = 2;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_kleap_stage_info_kleap_stage_info_table_info_per_stage_per_pd].values, dnxc_data_value_t, submodule_data->tables[dnx_data_kleap_stage_info_kleap_stage_info_table_info_per_stage_per_pd].nof_values, "_dnx_data_kleap_stage_info_kleap_stage_info_table_info_per_stage_per_pd table values");
    submodule_data->tables[dnx_data_kleap_stage_info_kleap_stage_info_table_info_per_stage_per_pd].values[0].name = "pd_mapping_type";
    submodule_data->tables[dnx_data_kleap_stage_info_kleap_stage_info_table_info_per_stage_per_pd].values[0].type = "int";
    submodule_data->tables[dnx_data_kleap_stage_info_kleap_stage_info_table_info_per_stage_per_pd].values[0].doc = "";
    submodule_data->tables[dnx_data_kleap_stage_info_kleap_stage_info_table_info_per_stage_per_pd].values[0].offset = UTILEX_OFFSETOF(dnx_data_kleap_stage_info_kleap_stage_info_info_per_stage_per_pd_t, pd_mapping_type);
    submodule_data->tables[dnx_data_kleap_stage_info_kleap_stage_info_table_info_per_stage_per_pd].values[1].name = "pd_mapping_name";
    submodule_data->tables[dnx_data_kleap_stage_info_kleap_stage_info_table_info_per_stage_per_pd].values[1].type = "char *";
    submodule_data->tables[dnx_data_kleap_stage_info_kleap_stage_info_table_info_per_stage_per_pd].values[1].doc = "";
    submodule_data->tables[dnx_data_kleap_stage_info_kleap_stage_info_table_info_per_stage_per_pd].values[1].offset = UTILEX_OFFSETOF(dnx_data_kleap_stage_info_kleap_stage_info_info_per_stage_per_pd_t, pd_mapping_name);

    
    submodule_data->tables[dnx_data_kleap_stage_info_kleap_stage_info_table_info_per_stage_per_sub_stage].name = "info_per_stage_per_sub_stage";
    submodule_data->tables[dnx_data_kleap_stage_info_kleap_stage_info_table_info_per_stage_per_sub_stage].doc = "";
    submodule_data->tables[dnx_data_kleap_stage_info_kleap_stage_info_table_info_per_stage_per_sub_stage].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_kleap_stage_info_kleap_stage_info_table_info_per_stage_per_sub_stage].size_of_values = sizeof(dnx_data_kleap_stage_info_kleap_stage_info_info_per_stage_per_sub_stage_t);
    submodule_data->tables[dnx_data_kleap_stage_info_kleap_stage_info_table_info_per_stage_per_sub_stage].entry_get = dnx_data_kleap_stage_info_kleap_stage_info_info_per_stage_per_sub_stage_entry_str_get;

    
    submodule_data->tables[dnx_data_kleap_stage_info_kleap_stage_info_table_info_per_stage_per_sub_stage].nof_keys = 2;
    submodule_data->tables[dnx_data_kleap_stage_info_kleap_stage_info_table_info_per_stage_per_sub_stage].keys[0].name = "stage_index";
    submodule_data->tables[dnx_data_kleap_stage_info_kleap_stage_info_table_info_per_stage_per_sub_stage].keys[0].doc = "stage index";
    submodule_data->tables[dnx_data_kleap_stage_info_kleap_stage_info_table_info_per_stage_per_sub_stage].keys[1].name = "sub_stage_index";
    submodule_data->tables[dnx_data_kleap_stage_info_kleap_stage_info_table_info_per_stage_per_sub_stage].keys[1].doc = "sub stage index";

    
    submodule_data->tables[dnx_data_kleap_stage_info_kleap_stage_info_table_info_per_stage_per_sub_stage].nof_values = 6;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_kleap_stage_info_kleap_stage_info_table_info_per_stage_per_sub_stage].values, dnxc_data_value_t, submodule_data->tables[dnx_data_kleap_stage_info_kleap_stage_info_table_info_per_stage_per_sub_stage].nof_values, "_dnx_data_kleap_stage_info_kleap_stage_info_table_info_per_stage_per_sub_stage table values");
    submodule_data->tables[dnx_data_kleap_stage_info_kleap_stage_info_table_info_per_stage_per_sub_stage].values[0].name = "kleap_sub_stage_name";
    submodule_data->tables[dnx_data_kleap_stage_info_kleap_stage_info_table_info_per_stage_per_sub_stage].values[0].type = "char *";
    submodule_data->tables[dnx_data_kleap_stage_info_kleap_stage_info_table_info_per_stage_per_sub_stage].values[0].doc = "";
    submodule_data->tables[dnx_data_kleap_stage_info_kleap_stage_info_table_info_per_stage_per_sub_stage].values[0].offset = UTILEX_OFFSETOF(dnx_data_kleap_stage_info_kleap_stage_info_info_per_stage_per_sub_stage_t, kleap_sub_stage_name);
    submodule_data->tables[dnx_data_kleap_stage_info_kleap_stage_info_table_info_per_stage_per_sub_stage].values[1].name = "stage_index_in_pipe";
    submodule_data->tables[dnx_data_kleap_stage_info_kleap_stage_info_table_info_per_stage_per_sub_stage].values[1].type = "int";
    submodule_data->tables[dnx_data_kleap_stage_info_kleap_stage_info_table_info_per_stage_per_sub_stage].values[1].doc = "";
    submodule_data->tables[dnx_data_kleap_stage_info_kleap_stage_info_table_info_per_stage_per_sub_stage].values[1].offset = UTILEX_OFFSETOF(dnx_data_kleap_stage_info_kleap_stage_info_info_per_stage_per_sub_stage_t, stage_index_in_pipe);
    submodule_data->tables[dnx_data_kleap_stage_info_kleap_stage_info_table_info_per_stage_per_sub_stage].values[2].name = "dbal_context_enum";
    submodule_data->tables[dnx_data_kleap_stage_info_kleap_stage_info_table_info_per_stage_per_sub_stage].values[2].type = "int";
    submodule_data->tables[dnx_data_kleap_stage_info_kleap_stage_info_table_info_per_stage_per_sub_stage].values[2].doc = "";
    submodule_data->tables[dnx_data_kleap_stage_info_kleap_stage_info_table_info_per_stage_per_sub_stage].values[2].offset = UTILEX_OFFSETOF(dnx_data_kleap_stage_info_kleap_stage_info_info_per_stage_per_sub_stage_t, dbal_context_enum);
    submodule_data->tables[dnx_data_kleap_stage_info_kleap_stage_info_table_info_per_stage_per_sub_stage].values[3].name = "dbal_stage_context_id";
    submodule_data->tables[dnx_data_kleap_stage_info_kleap_stage_info_table_info_per_stage_per_sub_stage].values[3].type = "int";
    submodule_data->tables[dnx_data_kleap_stage_info_kleap_stage_info_table_info_per_stage_per_sub_stage].values[3].doc = "";
    submodule_data->tables[dnx_data_kleap_stage_info_kleap_stage_info_table_info_per_stage_per_sub_stage].values[3].offset = UTILEX_OFFSETOF(dnx_data_kleap_stage_info_kleap_stage_info_info_per_stage_per_sub_stage_t, dbal_stage_context_id);
    submodule_data->tables[dnx_data_kleap_stage_info_kleap_stage_info_table_info_per_stage_per_sub_stage].values[4].name = "dbal_stage_context_properties";
    submodule_data->tables[dnx_data_kleap_stage_info_kleap_stage_info_table_info_per_stage_per_sub_stage].values[4].type = "int";
    submodule_data->tables[dnx_data_kleap_stage_info_kleap_stage_info_table_info_per_stage_per_sub_stage].values[4].doc = "";
    submodule_data->tables[dnx_data_kleap_stage_info_kleap_stage_info_table_info_per_stage_per_sub_stage].values[4].offset = UTILEX_OFFSETOF(dnx_data_kleap_stage_info_kleap_stage_info_info_per_stage_per_sub_stage_t, dbal_stage_context_properties);
    submodule_data->tables[dnx_data_kleap_stage_info_kleap_stage_info_table_info_per_stage_per_sub_stage].values[5].name = "literally_signal_structure_name";
    submodule_data->tables[dnx_data_kleap_stage_info_kleap_stage_info_table_info_per_stage_per_sub_stage].values[5].type = "char *";
    submodule_data->tables[dnx_data_kleap_stage_info_kleap_stage_info_table_info_per_stage_per_sub_stage].values[5].doc = "";
    submodule_data->tables[dnx_data_kleap_stage_info_kleap_stage_info_table_info_per_stage_per_sub_stage].values[5].offset = UTILEX_OFFSETOF(dnx_data_kleap_stage_info_kleap_stage_info_info_per_stage_per_sub_stage_t, literally_signal_structure_name);


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_kleap_stage_info_kleap_stage_info_feature_get(
    int unit,
    dnx_data_kleap_stage_info_kleap_stage_info_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_kleap_stage_info, dnx_data_kleap_stage_info_submodule_kleap_stage_info, feature);
}


uint32
dnx_data_kleap_stage_info_kleap_stage_info_nof_kleap_stages_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_kleap_stage_info, dnx_data_kleap_stage_info_submodule_kleap_stage_info, dnx_data_kleap_stage_info_kleap_stage_info_define_nof_kleap_stages);
}



const dnx_data_kleap_stage_info_kleap_stage_info_info_per_stage_t *
dnx_data_kleap_stage_info_kleap_stage_info_info_per_stage_get(
    int unit,
    int stage_index)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_kleap_stage_info, dnx_data_kleap_stage_info_submodule_kleap_stage_info, dnx_data_kleap_stage_info_kleap_stage_info_table_info_per_stage);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, stage_index, 0);
    return (const dnx_data_kleap_stage_info_kleap_stage_info_info_per_stage_t *) data;

}

const dnx_data_kleap_stage_info_kleap_stage_info_info_per_stage_per_kbr_t *
dnx_data_kleap_stage_info_kleap_stage_info_info_per_stage_per_kbr_get(
    int unit,
    int stage_index)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_kleap_stage_info, dnx_data_kleap_stage_info_submodule_kleap_stage_info, dnx_data_kleap_stage_info_kleap_stage_info_table_info_per_stage_per_kbr);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, stage_index, 0);
    return (const dnx_data_kleap_stage_info_kleap_stage_info_info_per_stage_per_kbr_t *) data;

}

const dnx_data_kleap_stage_info_kleap_stage_info_info_per_stage_per_pd_t *
dnx_data_kleap_stage_info_kleap_stage_info_info_per_stage_per_pd_get(
    int unit,
    int stage_index,
    int pd_index)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_kleap_stage_info, dnx_data_kleap_stage_info_submodule_kleap_stage_info, dnx_data_kleap_stage_info_kleap_stage_info_table_info_per_stage_per_pd);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, stage_index, pd_index);
    return (const dnx_data_kleap_stage_info_kleap_stage_info_info_per_stage_per_pd_t *) data;

}

const dnx_data_kleap_stage_info_kleap_stage_info_info_per_stage_per_sub_stage_t *
dnx_data_kleap_stage_info_kleap_stage_info_info_per_stage_per_sub_stage_get(
    int unit,
    int stage_index,
    int sub_stage_index)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_kleap_stage_info, dnx_data_kleap_stage_info_submodule_kleap_stage_info, dnx_data_kleap_stage_info_kleap_stage_info_table_info_per_stage_per_sub_stage);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, stage_index, sub_stage_index);
    return (const dnx_data_kleap_stage_info_kleap_stage_info_info_per_stage_per_sub_stage_t *) data;

}


shr_error_e
dnx_data_kleap_stage_info_kleap_stage_info_info_per_stage_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_kleap_stage_info_kleap_stage_info_info_per_stage_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_kleap_stage_info, dnx_data_kleap_stage_info_submodule_kleap_stage_info, dnx_data_kleap_stage_info_kleap_stage_info_table_info_per_stage);
    data = (const dnx_data_kleap_stage_info_kleap_stage_info_info_per_stage_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%s", data->stage_name == NULL ? "" : data->stage_name);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->type);
            break;
        case 2:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->nof_sub_stages);
            break;
        case 3:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->nof_kbrs);
            break;
        case 4:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->nof_ffc);
            break;
        case 5:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->nof_ffc_g);
            break;
        case 6:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->nof_pd);
            break;
        case 7:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->dbal_table_resource_mapping);
            break;
        case 8:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->dbal_table_kbr_info);
            break;
        case 9:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->dbal_table_ffc_instruction);
            break;
        case 10:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->dbal_table_ffc_quad_is_acl);
            break;
        case 11:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->dbal_table_pd_info);
            break;
        case 12:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->dbal_kbr_idx_field_per_stage);
            break;
        case 13:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->dbal_context_profile_field_per_stage);
            break;
        case 14:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->dbal_ffc_instruction_field_per_stage);
            break;
        case 15:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->dbal_litrally_instruction_field_per_stage);
            break;
        case 16:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->dbal_relative_header_instruction_field_per_stage);
            break;
        case 17:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->dbal_header_instruction_field_per_stage);
            break;
        case 18:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->dbal_record_instruction_field_per_stage);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_kleap_stage_info_kleap_stage_info_info_per_stage_per_kbr_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_kleap_stage_info_kleap_stage_info_info_per_stage_per_kbr_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_kleap_stage_info, dnx_data_kleap_stage_info_submodule_kleap_stage_info, dnx_data_kleap_stage_info_kleap_stage_info_table_info_per_stage_per_kbr);
    data = (const dnx_data_kleap_stage_info_kleap_stage_info_info_per_stage_per_kbr_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->nof_kbrs);
            break;
        case 1:
            DNXC_DATA_MGMT_ARR_STR(buffer, 19, data->kbr2physical);
            break;
        case 2:
            DNXC_DATA_MGMT_ARR_STR(buffer, 19, data->kbr2key_sig);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_kleap_stage_info_kleap_stage_info_info_per_stage_per_pd_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_kleap_stage_info_kleap_stage_info_info_per_stage_per_pd_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_kleap_stage_info, dnx_data_kleap_stage_info_submodule_kleap_stage_info, dnx_data_kleap_stage_info_kleap_stage_info_table_info_per_stage_per_pd);
    data = (const dnx_data_kleap_stage_info_kleap_stage_info_info_per_stage_per_pd_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, key1);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->pd_mapping_type);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%s", data->pd_mapping_name == NULL ? "" : data->pd_mapping_name);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_kleap_stage_info_kleap_stage_info_info_per_stage_per_sub_stage_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_kleap_stage_info_kleap_stage_info_info_per_stage_per_sub_stage_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_kleap_stage_info, dnx_data_kleap_stage_info_submodule_kleap_stage_info, dnx_data_kleap_stage_info_kleap_stage_info_table_info_per_stage_per_sub_stage);
    data = (const dnx_data_kleap_stage_info_kleap_stage_info_info_per_stage_per_sub_stage_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, key1);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%s", data->kleap_sub_stage_name == NULL ? "" : data->kleap_sub_stage_name);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->stage_index_in_pipe);
            break;
        case 2:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->dbal_context_enum);
            break;
        case 3:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->dbal_stage_context_id);
            break;
        case 4:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->dbal_stage_context_properties);
            break;
        case 5:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%s", data->literally_signal_structure_name == NULL ? "" : data->literally_signal_structure_name);
            break;
    }

    SHR_FUNC_EXIT;
}


const dnxc_data_table_info_t *
dnx_data_kleap_stage_info_kleap_stage_info_info_per_stage_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_kleap_stage_info, dnx_data_kleap_stage_info_submodule_kleap_stage_info, dnx_data_kleap_stage_info_kleap_stage_info_table_info_per_stage);

}

const dnxc_data_table_info_t *
dnx_data_kleap_stage_info_kleap_stage_info_info_per_stage_per_kbr_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_kleap_stage_info, dnx_data_kleap_stage_info_submodule_kleap_stage_info, dnx_data_kleap_stage_info_kleap_stage_info_table_info_per_stage_per_kbr);

}

const dnxc_data_table_info_t *
dnx_data_kleap_stage_info_kleap_stage_info_info_per_stage_per_pd_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_kleap_stage_info, dnx_data_kleap_stage_info_submodule_kleap_stage_info, dnx_data_kleap_stage_info_kleap_stage_info_table_info_per_stage_per_pd);

}

const dnxc_data_table_info_t *
dnx_data_kleap_stage_info_kleap_stage_info_info_per_stage_per_sub_stage_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_kleap_stage_info, dnx_data_kleap_stage_info_submodule_kleap_stage_info, dnx_data_kleap_stage_info_kleap_stage_info_table_info_per_stage_per_sub_stage);

}



shr_error_e
dnx_data_kleap_stage_info_init(
    int unit,
    dnxc_data_module_t *module_data)
{
    SHR_FUNC_INIT_VARS(unit);

    
    module_data->name = "kleap_stage_info";
    module_data->nof_submodules = _dnx_data_kleap_stage_info_submodule_nof;
    DNXC_DATA_ALLOC(module_data->submodules, dnxc_data_submodule_t, module_data->nof_submodules, "_dnxc_data kleap_stage_info submodules");

    
    SHR_IF_ERR_EXIT(dnx_data_kleap_stage_info_kleap_stage_info_init(unit, &module_data->submodules[dnx_data_kleap_stage_info_submodule_kleap_stage_info]));
    
#ifdef BCM_DNX2_SUPPORT

    if (dnxc_data_mgmt_is_jr2_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_kleap_stage_info_attach(unit));
    }
    else

#endif 
#ifdef BCM_DNX2_SUPPORT

    if (dnxc_data_mgmt_is_jr2_b0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_kleap_stage_info_attach(unit));
    }
    else

#endif 
#ifdef BCM_DNX2_SUPPORT

    if (dnxc_data_mgmt_is_jr2_b1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_kleap_stage_info_attach(unit));
    }
    else

#endif 
#ifdef BCM_DNX2_SUPPORT

    if (dnxc_data_mgmt_is_j2c_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_kleap_stage_info_attach(unit));
    }
    else

#endif 
#ifdef BCM_DNX2_SUPPORT

    if (dnxc_data_mgmt_is_j2c_a1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_kleap_stage_info_attach(unit));
    }
    else

#endif 
#ifdef BCM_DNX2_SUPPORT

    if (dnxc_data_mgmt_is_q2a_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_kleap_stage_info_attach(unit));
    }
    else

#endif 
#ifdef BCM_DNX2_SUPPORT

    if (dnxc_data_mgmt_is_q2a_b0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_kleap_stage_info_attach(unit));
    }
    else

#endif 
#ifdef BCM_DNX2_SUPPORT

    if (dnxc_data_mgmt_is_q2a_b1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_kleap_stage_info_attach(unit));
    }
    else

#endif 
#ifdef BCM_DNX2_SUPPORT

    if (dnxc_data_mgmt_is_j2p_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_kleap_stage_info_attach(unit));
    }
    else

#endif 
#ifdef BCM_DNX2_SUPPORT

    if (dnxc_data_mgmt_is_j2p_a1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_kleap_stage_info_attach(unit));
    }
    else

#endif 
#ifdef BCM_DNX2_SUPPORT

    if (dnxc_data_mgmt_is_j2p_a2(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_kleap_stage_info_attach(unit));
    }
    else

#endif 
#ifdef BCM_DNX2_SUPPORT

    if (dnxc_data_mgmt_is_j2x_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_kleap_stage_info_attach(unit));
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

