

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_COMMON

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_aod.h>



extern shr_error_e jr2_a0_data_aod_attach(
    int unit);
extern shr_error_e jr2_b0_data_aod_attach(
    int unit);
extern shr_error_e j2c_a0_data_aod_attach(
    int unit);
extern shr_error_e q2a_a0_data_aod_attach(
    int unit);
extern shr_error_e q2a_b0_data_aod_attach(
    int unit);
extern shr_error_e j2p_a0_data_aod_attach(
    int unit);



static shr_error_e
dnx_data_aod_tables_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "tables";
    submodule_data->doc = "AOD tables";
    
    submodule_data->nof_features = _dnx_data_aod_tables_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data aod tables features");

    
    submodule_data->nof_defines = _dnx_data_aod_tables_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data aod tables defines");

    
    submodule_data->nof_tables = _dnx_data_aod_tables_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data aod tables tables");

    
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER___image].name = "ERPP_FWD_CONTEXT_FILTER___image";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER___image].doc = "table related image";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER___image].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER___image].size_of_values = sizeof(dnx_data_aod_tables_ERPP_FWD_CONTEXT_FILTER___image_t);
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER___image].entry_get = dnx_data_aod_tables_ERPP_FWD_CONTEXT_FILTER___image_entry_str_get;

    
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER___image].nof_keys = 0;

    
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER___image].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER___image].values, dnxc_data_value_t, submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER___image].nof_values, "_dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER___image table values");
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER___image].values[0].name = "image";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER___image].values[0].type = "char *";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER___image].values[0].doc = "related image name";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER___image].values[0].offset = UTILEX_OFFSETOF(dnx_data_aod_tables_ERPP_FWD_CONTEXT_FILTER___image_t, image);

    
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].name = "ERPP_FWD_CONTEXT_FILTER";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].doc = "ERPP_FWD_CONTEXT_FILTER";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].size_of_values = sizeof(dnx_data_aod_tables_ERPP_FWD_CONTEXT_FILTER_t);
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].entry_get = dnx_data_aod_tables_ERPP_FWD_CONTEXT_FILTER_entry_str_get;

    
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].nof_keys = 1;
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].keys[0].name = "index";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].keys[0].doc = "entry index";

    
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].nof_values = 97;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values, dnxc_data_value_t, submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].nof_values, "_dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER table values");
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[0].name = "valid";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[0].type = "uint32";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[0].doc = "indicates whether entry is valid";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[0].offset = UTILEX_OFFSETOF(dnx_data_aod_tables_ERPP_FWD_CONTEXT_FILTER_t, valid);
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[1].name = "doc";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[1].type = "char *";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[1].doc = "entry documentation";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[1].offset = UTILEX_OFFSETOF(dnx_data_aod_tables_ERPP_FWD_CONTEXT_FILTER_t, doc);
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[2].name = "Egress_FWD_Code";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[2].type = "char *";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[2].doc = "Egress_FWD_Code";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[2].offset = UTILEX_OFFSETOF(dnx_data_aod_tables_ERPP_FWD_CONTEXT_FILTER_t, Egress_FWD_Code);
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[3].name = "Egress_FWD_Code_mask";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[3].type = "uint32";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[3].doc = "Egress_FWD_Code_mask";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[3].offset = UTILEX_OFFSETOF(dnx_data_aod_tables_ERPP_FWD_CONTEXT_FILTER_t, Egress_FWD_Code_mask);
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[4].name = "Egress_FWD_Code_valid";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[4].type = "uint32";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[4].doc = "Egress_FWD_Code_valid";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[4].offset = UTILEX_OFFSETOF(dnx_data_aod_tables_ERPP_FWD_CONTEXT_FILTER_t, Egress_FWD_Code_valid);
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[5].name = "DSS_Stacking";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[5].type = "uint32";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[5].doc = "DSS_Stacking";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[5].offset = UTILEX_OFFSETOF(dnx_data_aod_tables_ERPP_FWD_CONTEXT_FILTER_t, DSS_Stacking);
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[6].name = "DSS_Stacking_valid";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[6].type = "uint32";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[6].doc = "DSS_Stacking_valid";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[6].offset = UTILEX_OFFSETOF(dnx_data_aod_tables_ERPP_FWD_CONTEXT_FILTER_t, DSS_Stacking_valid);
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[7].name = "Exclude_Source";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[7].type = "uint32";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[7].doc = "Exclude_Source";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[7].offset = UTILEX_OFFSETOF(dnx_data_aod_tables_ERPP_FWD_CONTEXT_FILTER_t, Exclude_Source);
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[8].name = "Exclude_Source_valid";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[8].type = "uint32";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[8].doc = "Exclude_Source_valid";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[8].offset = UTILEX_OFFSETOF(dnx_data_aod_tables_ERPP_FWD_CONTEXT_FILTER_t, Exclude_Source_valid);
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[9].name = "GLEM_PP_or_NON_PP";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[9].type = "uint32";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[9].doc = "GLEM_PP_or_NON_PP";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[9].offset = UTILEX_OFFSETOF(dnx_data_aod_tables_ERPP_FWD_CONTEXT_FILTER_t, GLEM_PP_or_NON_PP);
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[10].name = "GLEM_PP_or_NON_PP_valid";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[10].type = "uint32";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[10].doc = "GLEM_PP_or_NON_PP_valid";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[10].offset = UTILEX_OFFSETOF(dnx_data_aod_tables_ERPP_FWD_CONTEXT_FILTER_t, GLEM_PP_or_NON_PP_valid);
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[11].name = "Hairpin_or_Same_Interface";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[11].type = "uint32";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[11].doc = "Hairpin_or_Same_Interface";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[11].offset = UTILEX_OFFSETOF(dnx_data_aod_tables_ERPP_FWD_CONTEXT_FILTER_t, Hairpin_or_Same_Interface);
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[12].name = "Hairpin_or_Same_Interface_valid";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[12].type = "uint32";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[12].doc = "Hairpin_or_Same_Interface_valid";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[12].offset = UTILEX_OFFSETOF(dnx_data_aod_tables_ERPP_FWD_CONTEXT_FILTER_t, Hairpin_or_Same_Interface_valid);
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[13].name = "IPv4_Filters";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[13].type = "uint32";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[13].doc = "IPv4_Filters";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[13].offset = UTILEX_OFFSETOF(dnx_data_aod_tables_ERPP_FWD_CONTEXT_FILTER_t, IPv4_Filters);
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[14].name = "IPv4_Filters_valid";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[14].type = "uint32";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[14].doc = "IPv4_Filters_valid";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[14].offset = UTILEX_OFFSETOF(dnx_data_aod_tables_ERPP_FWD_CONTEXT_FILTER_t, IPv4_Filters_valid);
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[15].name = "IPv4_Filters_Dip_Zero";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[15].type = "uint32";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[15].doc = "IPv4_Filters_Dip_Zero";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[15].offset = UTILEX_OFFSETOF(dnx_data_aod_tables_ERPP_FWD_CONTEXT_FILTER_t, IPv4_Filters_Dip_Zero);
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[16].name = "IPv4_Filters_Dip_Zero_valid";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[16].type = "uint32";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[16].doc = "IPv4_Filters_Dip_Zero_valid";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[16].offset = UTILEX_OFFSETOF(dnx_data_aod_tables_ERPP_FWD_CONTEXT_FILTER_t, IPv4_Filters_Dip_Zero_valid);
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[17].name = "IPv4_Filters_Header_Checksum";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[17].type = "uint32";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[17].doc = "IPv4_Filters_Header_Checksum";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[17].offset = UTILEX_OFFSETOF(dnx_data_aod_tables_ERPP_FWD_CONTEXT_FILTER_t, IPv4_Filters_Header_Checksum);
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[18].name = "IPv4_Filters_Header_Checksum_valid";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[18].type = "uint32";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[18].doc = "IPv4_Filters_Header_Checksum_valid";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[18].offset = UTILEX_OFFSETOF(dnx_data_aod_tables_ERPP_FWD_CONTEXT_FILTER_t, IPv4_Filters_Header_Checksum_valid);
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[19].name = "IPv4_Filters_Header_Length";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[19].type = "uint32";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[19].doc = "IPv4_Filters_Header_Length";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[19].offset = UTILEX_OFFSETOF(dnx_data_aod_tables_ERPP_FWD_CONTEXT_FILTER_t, IPv4_Filters_Header_Length);
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[20].name = "IPv4_Filters_Header_Length_valid";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[20].type = "uint32";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[20].doc = "IPv4_Filters_Header_Length_valid";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[20].offset = UTILEX_OFFSETOF(dnx_data_aod_tables_ERPP_FWD_CONTEXT_FILTER_t, IPv4_Filters_Header_Length_valid);
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[21].name = "IPv4_Filters_Options";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[21].type = "uint32";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[21].doc = "IPv4_Filters_Options";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[21].offset = UTILEX_OFFSETOF(dnx_data_aod_tables_ERPP_FWD_CONTEXT_FILTER_t, IPv4_Filters_Options);
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[22].name = "IPv4_Filters_Options_valid";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[22].type = "uint32";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[22].doc = "IPv4_Filters_Options_valid";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[22].offset = UTILEX_OFFSETOF(dnx_data_aod_tables_ERPP_FWD_CONTEXT_FILTER_t, IPv4_Filters_Options_valid);
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[23].name = "IPv4_Filters_Sip_Equal_Dip";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[23].type = "uint32";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[23].doc = "IPv4_Filters_Sip_Equal_Dip";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[23].offset = UTILEX_OFFSETOF(dnx_data_aod_tables_ERPP_FWD_CONTEXT_FILTER_t, IPv4_Filters_Sip_Equal_Dip);
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[24].name = "IPv4_Filters_Sip_Equal_Dip_valid";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[24].type = "uint32";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[24].doc = "IPv4_Filters_Sip_Equal_Dip_valid";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[24].offset = UTILEX_OFFSETOF(dnx_data_aod_tables_ERPP_FWD_CONTEXT_FILTER_t, IPv4_Filters_Sip_Equal_Dip_valid);
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[25].name = "IPv4_Filters_Sip_Multicast";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[25].type = "uint32";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[25].doc = "IPv4_Filters_Sip_Multicast";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[25].offset = UTILEX_OFFSETOF(dnx_data_aod_tables_ERPP_FWD_CONTEXT_FILTER_t, IPv4_Filters_Sip_Multicast);
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[26].name = "IPv4_Filters_Sip_Multicast_valid";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[26].type = "uint32";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[26].doc = "IPv4_Filters_Sip_Multicast_valid";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[26].offset = UTILEX_OFFSETOF(dnx_data_aod_tables_ERPP_FWD_CONTEXT_FILTER_t, IPv4_Filters_Sip_Multicast_valid);
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[27].name = "IPv4_Filters_Total_Length";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[27].type = "uint32";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[27].doc = "IPv4_Filters_Total_Length";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[27].offset = UTILEX_OFFSETOF(dnx_data_aod_tables_ERPP_FWD_CONTEXT_FILTER_t, IPv4_Filters_Total_Length);
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[28].name = "IPv4_Filters_Total_Length_valid";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[28].type = "uint32";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[28].doc = "IPv4_Filters_Total_Length_valid";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[28].offset = UTILEX_OFFSETOF(dnx_data_aod_tables_ERPP_FWD_CONTEXT_FILTER_t, IPv4_Filters_Total_Length_valid);
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[29].name = "IPv4_Filters_Version";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[29].type = "uint32";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[29].doc = "IPv4_Filters_Version";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[29].offset = UTILEX_OFFSETOF(dnx_data_aod_tables_ERPP_FWD_CONTEXT_FILTER_t, IPv4_Filters_Version);
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[30].name = "IPv4_Filters_Version_valid";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[30].type = "uint32";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[30].doc = "IPv4_Filters_Version_valid";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[30].offset = UTILEX_OFFSETOF(dnx_data_aod_tables_ERPP_FWD_CONTEXT_FILTER_t, IPv4_Filters_Version_valid);
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[31].name = "IPv6_Filters";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[31].type = "uint32";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[31].doc = "IPv6_Filters";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[31].offset = UTILEX_OFFSETOF(dnx_data_aod_tables_ERPP_FWD_CONTEXT_FILTER_t, IPv6_Filters);
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[32].name = "IPv6_Filters_valid";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[32].type = "uint32";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[32].doc = "IPv6_Filters_valid";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[32].offset = UTILEX_OFFSETOF(dnx_data_aod_tables_ERPP_FWD_CONTEXT_FILTER_t, IPv6_Filters_valid);
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[33].name = "IPv6_Filters_Dip_Is_All";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[33].type = "uint32";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[33].doc = "IPv6_Filters_Dip_Is_All";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[33].offset = UTILEX_OFFSETOF(dnx_data_aod_tables_ERPP_FWD_CONTEXT_FILTER_t, IPv6_Filters_Dip_Is_All);
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[34].name = "IPv6_Filters_Dip_Is_All_valid";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[34].type = "uint32";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[34].doc = "IPv6_Filters_Dip_Is_All_valid";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[34].offset = UTILEX_OFFSETOF(dnx_data_aod_tables_ERPP_FWD_CONTEXT_FILTER_t, IPv6_Filters_Dip_Is_All_valid);
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[35].name = "IPv6_Filters_Ipv4_Cmp_Dst";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[35].type = "uint32";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[35].doc = "IPv6_Filters_Ipv4_Cmp_Dst";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[35].offset = UTILEX_OFFSETOF(dnx_data_aod_tables_ERPP_FWD_CONTEXT_FILTER_t, IPv6_Filters_Ipv4_Cmp_Dst);
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[36].name = "IPv6_Filters_Ipv4_Cmp_Dst_valid";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[36].type = "uint32";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[36].doc = "IPv6_Filters_Ipv4_Cmp_Dst_valid";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[36].offset = UTILEX_OFFSETOF(dnx_data_aod_tables_ERPP_FWD_CONTEXT_FILTER_t, IPv6_Filters_Ipv4_Cmp_Dst_valid);
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[37].name = "IPv6_Filters_Ipv4_Mapped_Dst";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[37].type = "uint32";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[37].doc = "IPv6_Filters_Ipv4_Mapped_Dst";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[37].offset = UTILEX_OFFSETOF(dnx_data_aod_tables_ERPP_FWD_CONTEXT_FILTER_t, IPv6_Filters_Ipv4_Mapped_Dst);
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[38].name = "IPv6_Filters_Ipv4_Mapped_Dst_valid";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[38].type = "uint32";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[38].doc = "IPv6_Filters_Ipv4_Mapped_Dst_valid";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[38].offset = UTILEX_OFFSETOF(dnx_data_aod_tables_ERPP_FWD_CONTEXT_FILTER_t, IPv6_Filters_Ipv4_Mapped_Dst_valid);
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[39].name = "IPv6_Filters_Link_Local_Dst";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[39].type = "uint32";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[39].doc = "IPv6_Filters_Link_Local_Dst";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[39].offset = UTILEX_OFFSETOF(dnx_data_aod_tables_ERPP_FWD_CONTEXT_FILTER_t, IPv6_Filters_Link_Local_Dst);
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[40].name = "IPv6_Filters_Link_Local_Dst_valid";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[40].type = "uint32";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[40].doc = "IPv6_Filters_Link_Local_Dst_valid";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[40].offset = UTILEX_OFFSETOF(dnx_data_aod_tables_ERPP_FWD_CONTEXT_FILTER_t, IPv6_Filters_Link_Local_Dst_valid);
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[41].name = "IPv6_Filters_Link_Local_Src";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[41].type = "uint32";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[41].doc = "IPv6_Filters_Link_Local_Src";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[41].offset = UTILEX_OFFSETOF(dnx_data_aod_tables_ERPP_FWD_CONTEXT_FILTER_t, IPv6_Filters_Link_Local_Src);
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[42].name = "IPv6_Filters_Link_Local_Src_valid";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[42].type = "uint32";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[42].doc = "IPv6_Filters_Link_Local_Src_valid";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[42].offset = UTILEX_OFFSETOF(dnx_data_aod_tables_ERPP_FWD_CONTEXT_FILTER_t, IPv6_Filters_Link_Local_Src_valid);
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[43].name = "IPv6_Filters_Loopback";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[43].type = "uint32";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[43].doc = "IPv6_Filters_Loopback";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[43].offset = UTILEX_OFFSETOF(dnx_data_aod_tables_ERPP_FWD_CONTEXT_FILTER_t, IPv6_Filters_Loopback);
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[44].name = "IPv6_Filters_Loopback_valid";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[44].type = "uint32";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[44].doc = "IPv6_Filters_Loopback_valid";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[44].offset = UTILEX_OFFSETOF(dnx_data_aod_tables_ERPP_FWD_CONTEXT_FILTER_t, IPv6_Filters_Loopback_valid);
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[45].name = "IPv6_Filters_Mc_Dst";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[45].type = "uint32";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[45].doc = "IPv6_Filters_Mc_Dst";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[45].offset = UTILEX_OFFSETOF(dnx_data_aod_tables_ERPP_FWD_CONTEXT_FILTER_t, IPv6_Filters_Mc_Dst);
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[46].name = "IPv6_Filters_Mc_Dst_valid";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[46].type = "uint32";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[46].doc = "IPv6_Filters_Mc_Dst_valid";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[46].offset = UTILEX_OFFSETOF(dnx_data_aod_tables_ERPP_FWD_CONTEXT_FILTER_t, IPv6_Filters_Mc_Dst_valid);
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[47].name = "IPv6_Filters_Next_Header_Is_Zeros";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[47].type = "uint32";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[47].doc = "IPv6_Filters_Next_Header_Is_Zeros";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[47].offset = UTILEX_OFFSETOF(dnx_data_aod_tables_ERPP_FWD_CONTEXT_FILTER_t, IPv6_Filters_Next_Header_Is_Zeros);
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[48].name = "IPv6_Filters_Next_Header_Is_Zeros_valid";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[48].type = "uint32";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[48].doc = "IPv6_Filters_Next_Header_Is_Zeros_valid";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[48].offset = UTILEX_OFFSETOF(dnx_data_aod_tables_ERPP_FWD_CONTEXT_FILTER_t, IPv6_Filters_Next_Header_Is_Zeros_valid);
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[49].name = "IPv6_Filters_Sip_Is_All";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[49].type = "uint32";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[49].doc = "IPv6_Filters_Sip_Is_All";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[49].offset = UTILEX_OFFSETOF(dnx_data_aod_tables_ERPP_FWD_CONTEXT_FILTER_t, IPv6_Filters_Sip_Is_All);
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[50].name = "IPv6_Filters_Sip_Is_All_valid";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[50].type = "uint32";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[50].doc = "IPv6_Filters_Sip_Is_All_valid";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[50].offset = UTILEX_OFFSETOF(dnx_data_aod_tables_ERPP_FWD_CONTEXT_FILTER_t, IPv6_Filters_Sip_Is_All_valid);
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[51].name = "IPv6_Filters_Sip_Is_Mc";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[51].type = "uint32";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[51].doc = "IPv6_Filters_Sip_Is_Mc";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[51].offset = UTILEX_OFFSETOF(dnx_data_aod_tables_ERPP_FWD_CONTEXT_FILTER_t, IPv6_Filters_Sip_Is_Mc);
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[52].name = "IPv6_Filters_Sip_Is_Mc_valid";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[52].type = "uint32";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[52].doc = "IPv6_Filters_Sip_Is_Mc_valid";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[52].offset = UTILEX_OFFSETOF(dnx_data_aod_tables_ERPP_FWD_CONTEXT_FILTER_t, IPv6_Filters_Sip_Is_Mc_valid);
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[53].name = "IPv6_Filters_Site_Local_Dst";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[53].type = "uint32";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[53].doc = "IPv6_Filters_Site_Local_Dst";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[53].offset = UTILEX_OFFSETOF(dnx_data_aod_tables_ERPP_FWD_CONTEXT_FILTER_t, IPv6_Filters_Site_Local_Dst);
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[54].name = "IPv6_Filters_Site_Local_Dst_valid";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[54].type = "uint32";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[54].doc = "IPv6_Filters_Site_Local_Dst_valid";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[54].offset = UTILEX_OFFSETOF(dnx_data_aod_tables_ERPP_FWD_CONTEXT_FILTER_t, IPv6_Filters_Site_Local_Dst_valid);
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[55].name = "IPv6_Filters_Site_Local_Src";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[55].type = "uint32";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[55].doc = "IPv6_Filters_Site_Local_Src";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[55].offset = UTILEX_OFFSETOF(dnx_data_aod_tables_ERPP_FWD_CONTEXT_FILTER_t, IPv6_Filters_Site_Local_Src);
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[56].name = "IPv6_Filters_Site_Local_Src_valid";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[56].type = "uint32";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[56].doc = "IPv6_Filters_Site_Local_Src_valid";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[56].offset = UTILEX_OFFSETOF(dnx_data_aod_tables_ERPP_FWD_CONTEXT_FILTER_t, IPv6_Filters_Site_Local_Src_valid);
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[57].name = "IPv6_Filters_Version";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[57].type = "uint32";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[57].doc = "IPv6_Filters_Version";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[57].offset = UTILEX_OFFSETOF(dnx_data_aod_tables_ERPP_FWD_CONTEXT_FILTER_t, IPv6_Filters_Version);
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[58].name = "IPv6_Filters_Version_valid";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[58].type = "uint32";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[58].doc = "IPv6_Filters_Version_valid";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[58].offset = UTILEX_OFFSETOF(dnx_data_aod_tables_ERPP_FWD_CONTEXT_FILTER_t, IPv6_Filters_Version_valid);
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[59].name = "Invalid_OTM";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[59].type = "uint32";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[59].doc = "Invalid_OTM";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[59].offset = UTILEX_OFFSETOF(dnx_data_aod_tables_ERPP_FWD_CONTEXT_FILTER_t, Invalid_OTM);
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[60].name = "Invalid_OTM_valid";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[60].type = "uint32";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[60].doc = "Invalid_OTM_valid";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[60].offset = UTILEX_OFFSETOF(dnx_data_aod_tables_ERPP_FWD_CONTEXT_FILTER_t, Invalid_OTM_valid);
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[61].name = "L4_Filters";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[61].type = "uint32";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[61].doc = "L4_Filters";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[61].offset = UTILEX_OFFSETOF(dnx_data_aod_tables_ERPP_FWD_CONTEXT_FILTER_t, L4_Filters);
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[62].name = "L4_Filters_valid";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[62].type = "uint32";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[62].doc = "L4_Filters_valid";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[62].offset = UTILEX_OFFSETOF(dnx_data_aod_tables_ERPP_FWD_CONTEXT_FILTER_t, L4_Filters_valid);
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[63].name = "L4_Filters_Tcp_Fragment_Incomplete_Hdr";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[63].type = "uint32";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[63].doc = "L4_Filters_Tcp_Fragment_Incomplete_Hdr";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[63].offset = UTILEX_OFFSETOF(dnx_data_aod_tables_ERPP_FWD_CONTEXT_FILTER_t, L4_Filters_Tcp_Fragment_Incomplete_Hdr);
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[64].name = "L4_Filters_Tcp_Fragment_Incomplete_Hdr_valid";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[64].type = "uint32";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[64].doc = "L4_Filters_Tcp_Fragment_Incomplete_Hdr_valid";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[64].offset = UTILEX_OFFSETOF(dnx_data_aod_tables_ERPP_FWD_CONTEXT_FILTER_t, L4_Filters_Tcp_Fragment_Incomplete_Hdr_valid);
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[65].name = "L4_Filters_Tcp_Fragment_Offset_Less_Than_8";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[65].type = "uint32";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[65].doc = "L4_Filters_Tcp_Fragment_Offset_Less_Than_8";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[65].offset = UTILEX_OFFSETOF(dnx_data_aod_tables_ERPP_FWD_CONTEXT_FILTER_t, L4_Filters_Tcp_Fragment_Offset_Less_Than_8);
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[66].name = "L4_Filters_Tcp_Fragment_Offset_Less_Than_8_valid";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[66].type = "uint32";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[66].doc = "L4_Filters_Tcp_Fragment_Offset_Less_Than_8_valid";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[66].offset = UTILEX_OFFSETOF(dnx_data_aod_tables_ERPP_FWD_CONTEXT_FILTER_t, L4_Filters_Tcp_Fragment_Offset_Less_Than_8_valid);
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[67].name = "L4_Filters_Tcp_Seq_Num_Zero_Flags_Set";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[67].type = "uint32";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[67].doc = "L4_Filters_Tcp_Seq_Num_Zero_Flags_Set";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[67].offset = UTILEX_OFFSETOF(dnx_data_aod_tables_ERPP_FWD_CONTEXT_FILTER_t, L4_Filters_Tcp_Seq_Num_Zero_Flags_Set);
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[68].name = "L4_Filters_Tcp_Seq_Num_Zero_Flags_Set_valid";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[68].type = "uint32";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[68].doc = "L4_Filters_Tcp_Seq_Num_Zero_Flags_Set_valid";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[68].offset = UTILEX_OFFSETOF(dnx_data_aod_tables_ERPP_FWD_CONTEXT_FILTER_t, L4_Filters_Tcp_Seq_Num_Zero_Flags_Set_valid);
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[69].name = "L4_Filters_Tcp_Seq_Num_Zero_Flags_Zero";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[69].type = "uint32";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[69].doc = "L4_Filters_Tcp_Seq_Num_Zero_Flags_Zero";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[69].offset = UTILEX_OFFSETOF(dnx_data_aod_tables_ERPP_FWD_CONTEXT_FILTER_t, L4_Filters_Tcp_Seq_Num_Zero_Flags_Zero);
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[70].name = "L4_Filters_Tcp_Seq_Num_Zero_Flags_Zero_valid";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[70].type = "uint32";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[70].doc = "L4_Filters_Tcp_Seq_Num_Zero_Flags_Zero_valid";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[70].offset = UTILEX_OFFSETOF(dnx_data_aod_tables_ERPP_FWD_CONTEXT_FILTER_t, L4_Filters_Tcp_Seq_Num_Zero_Flags_Zero_valid);
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[71].name = "L4_Filters_Tcp_Src_Port_Equals_Dst_Port";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[71].type = "uint32";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[71].doc = "L4_Filters_Tcp_Src_Port_Equals_Dst_Port";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[71].offset = UTILEX_OFFSETOF(dnx_data_aod_tables_ERPP_FWD_CONTEXT_FILTER_t, L4_Filters_Tcp_Src_Port_Equals_Dst_Port);
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[72].name = "L4_Filters_Tcp_Src_Port_Equals_Dst_Port_valid";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[72].type = "uint32";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[72].doc = "L4_Filters_Tcp_Src_Port_Equals_Dst_Port_valid";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[72].offset = UTILEX_OFFSETOF(dnx_data_aod_tables_ERPP_FWD_CONTEXT_FILTER_t, L4_Filters_Tcp_Src_Port_Equals_Dst_Port_valid);
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[73].name = "L4_Filters_Tcp_Syn_And_Fin_Are_Set";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[73].type = "uint32";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[73].doc = "L4_Filters_Tcp_Syn_And_Fin_Are_Set";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[73].offset = UTILEX_OFFSETOF(dnx_data_aod_tables_ERPP_FWD_CONTEXT_FILTER_t, L4_Filters_Tcp_Syn_And_Fin_Are_Set);
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[74].name = "L4_Filters_Tcp_Syn_And_Fin_Are_Set_valid";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[74].type = "uint32";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[74].doc = "L4_Filters_Tcp_Syn_And_Fin_Are_Set_valid";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[74].offset = UTILEX_OFFSETOF(dnx_data_aod_tables_ERPP_FWD_CONTEXT_FILTER_t, L4_Filters_Tcp_Syn_And_Fin_Are_Set_valid);
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[75].name = "L4_Filters_Udp_Src_Port_Equals_Dst_Port";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[75].type = "uint32";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[75].doc = "L4_Filters_Udp_Src_Port_Equals_Dst_Port";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[75].offset = UTILEX_OFFSETOF(dnx_data_aod_tables_ERPP_FWD_CONTEXT_FILTER_t, L4_Filters_Udp_Src_Port_Equals_Dst_Port);
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[76].name = "L4_Filters_Udp_Src_Port_Equals_Dst_Port_valid";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[76].type = "uint32";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[76].doc = "L4_Filters_Udp_Src_Port_Equals_Dst_Port_valid";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[76].offset = UTILEX_OFFSETOF(dnx_data_aod_tables_ERPP_FWD_CONTEXT_FILTER_t, L4_Filters_Udp_Src_Port_Equals_Dst_Port_valid);
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[77].name = "LAG_Multicast";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[77].type = "uint32";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[77].doc = "LAG_Multicast";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[77].offset = UTILEX_OFFSETOF(dnx_data_aod_tables_ERPP_FWD_CONTEXT_FILTER_t, LAG_Multicast);
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[78].name = "LAG_Multicast_valid";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[78].type = "uint32";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[78].doc = "LAG_Multicast_valid";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[78].offset = UTILEX_OFFSETOF(dnx_data_aod_tables_ERPP_FWD_CONTEXT_FILTER_t, LAG_Multicast_valid);
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[79].name = "MTU";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[79].type = "uint32";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[79].doc = "MTU";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[79].offset = UTILEX_OFFSETOF(dnx_data_aod_tables_ERPP_FWD_CONTEXT_FILTER_t, MTU);
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[80].name = "MTU_valid";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[80].type = "uint32";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[80].doc = "MTU_valid";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[80].offset = UTILEX_OFFSETOF(dnx_data_aod_tables_ERPP_FWD_CONTEXT_FILTER_t, MTU_valid);
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[81].name = "RQP_Discard";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[81].type = "uint32";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[81].doc = "RQP_Discard";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[81].offset = UTILEX_OFFSETOF(dnx_data_aod_tables_ERPP_FWD_CONTEXT_FILTER_t, RQP_Discard);
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[82].name = "RQP_Discard_valid";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[82].type = "uint32";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[82].doc = "RQP_Discard_valid";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[82].offset = UTILEX_OFFSETOF(dnx_data_aod_tables_ERPP_FWD_CONTEXT_FILTER_t, RQP_Discard_valid);
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[83].name = "Split_Horizon";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[83].type = "uint32";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[83].doc = "Split_Horizon";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[83].offset = UTILEX_OFFSETOF(dnx_data_aod_tables_ERPP_FWD_CONTEXT_FILTER_t, Split_Horizon);
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[84].name = "Split_Horizon_valid";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[84].type = "uint32";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[84].doc = "Split_Horizon_valid";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[84].offset = UTILEX_OFFSETOF(dnx_data_aod_tables_ERPP_FWD_CONTEXT_FILTER_t, Split_Horizon_valid);
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[85].name = "TDM_Indication";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[85].type = "uint32";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[85].doc = "TDM_Indication";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[85].offset = UTILEX_OFFSETOF(dnx_data_aod_tables_ERPP_FWD_CONTEXT_FILTER_t, TDM_Indication);
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[86].name = "TDM_Indication_valid";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[86].type = "uint32";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[86].doc = "TDM_Indication_valid";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[86].offset = UTILEX_OFFSETOF(dnx_data_aod_tables_ERPP_FWD_CONTEXT_FILTER_t, TDM_Indication_valid);
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[87].name = "TTL_Scoping";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[87].type = "uint32";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[87].doc = "TTL_Scoping";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[87].offset = UTILEX_OFFSETOF(dnx_data_aod_tables_ERPP_FWD_CONTEXT_FILTER_t, TTL_Scoping);
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[88].name = "TTL_Scoping_valid";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[88].type = "uint32";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[88].doc = "TTL_Scoping_valid";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[88].offset = UTILEX_OFFSETOF(dnx_data_aod_tables_ERPP_FWD_CONTEXT_FILTER_t, TTL_Scoping_valid);
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[89].name = "TTL_eq_0";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[89].type = "uint32";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[89].doc = "TTL_eq_0";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[89].offset = UTILEX_OFFSETOF(dnx_data_aod_tables_ERPP_FWD_CONTEXT_FILTER_t, TTL_eq_0);
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[90].name = "TTL_eq_0_valid";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[90].type = "uint32";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[90].doc = "TTL_eq_0_valid";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[90].offset = UTILEX_OFFSETOF(dnx_data_aod_tables_ERPP_FWD_CONTEXT_FILTER_t, TTL_eq_0_valid);
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[91].name = "TTL_eq_1";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[91].type = "uint32";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[91].doc = "TTL_eq_1";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[91].offset = UTILEX_OFFSETOF(dnx_data_aod_tables_ERPP_FWD_CONTEXT_FILTER_t, TTL_eq_1);
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[92].name = "TTL_eq_1_valid";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[92].type = "uint32";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[92].doc = "TTL_eq_1_valid";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[92].offset = UTILEX_OFFSETOF(dnx_data_aod_tables_ERPP_FWD_CONTEXT_FILTER_t, TTL_eq_1_valid);
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[93].name = "Unacceptable_Frame_Types";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[93].type = "uint32";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[93].doc = "Unacceptable_Frame_Types";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[93].offset = UTILEX_OFFSETOF(dnx_data_aod_tables_ERPP_FWD_CONTEXT_FILTER_t, Unacceptable_Frame_Types);
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[94].name = "Unacceptable_Frame_Types_valid";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[94].type = "uint32";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[94].doc = "Unacceptable_Frame_Types_valid";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[94].offset = UTILEX_OFFSETOF(dnx_data_aod_tables_ERPP_FWD_CONTEXT_FILTER_t, Unacceptable_Frame_Types_valid);
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[95].name = "Unknown_DA";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[95].type = "uint32";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[95].doc = "Unknown_DA";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[95].offset = UTILEX_OFFSETOF(dnx_data_aod_tables_ERPP_FWD_CONTEXT_FILTER_t, Unknown_DA);
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[96].name = "Unknown_DA_valid";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[96].type = "uint32";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[96].doc = "Unknown_DA_valid";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER].values[96].offset = UTILEX_OFFSETOF(dnx_data_aod_tables_ERPP_FWD_CONTEXT_FILTER_t, Unknown_DA_valid);

    
    submodule_data->tables[dnx_data_aod_tables_table_EGRESS_PER_FORWARD_CODE___image].name = "EGRESS_PER_FORWARD_CODE___image";
    submodule_data->tables[dnx_data_aod_tables_table_EGRESS_PER_FORWARD_CODE___image].doc = "table related image";
    submodule_data->tables[dnx_data_aod_tables_table_EGRESS_PER_FORWARD_CODE___image].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_aod_tables_table_EGRESS_PER_FORWARD_CODE___image].size_of_values = sizeof(dnx_data_aod_tables_EGRESS_PER_FORWARD_CODE___image_t);
    submodule_data->tables[dnx_data_aod_tables_table_EGRESS_PER_FORWARD_CODE___image].entry_get = dnx_data_aod_tables_EGRESS_PER_FORWARD_CODE___image_entry_str_get;

    
    submodule_data->tables[dnx_data_aod_tables_table_EGRESS_PER_FORWARD_CODE___image].nof_keys = 0;

    
    submodule_data->tables[dnx_data_aod_tables_table_EGRESS_PER_FORWARD_CODE___image].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_aod_tables_table_EGRESS_PER_FORWARD_CODE___image].values, dnxc_data_value_t, submodule_data->tables[dnx_data_aod_tables_table_EGRESS_PER_FORWARD_CODE___image].nof_values, "_dnx_data_aod_tables_table_EGRESS_PER_FORWARD_CODE___image table values");
    submodule_data->tables[dnx_data_aod_tables_table_EGRESS_PER_FORWARD_CODE___image].values[0].name = "image";
    submodule_data->tables[dnx_data_aod_tables_table_EGRESS_PER_FORWARD_CODE___image].values[0].type = "char *";
    submodule_data->tables[dnx_data_aod_tables_table_EGRESS_PER_FORWARD_CODE___image].values[0].doc = "related image name";
    submodule_data->tables[dnx_data_aod_tables_table_EGRESS_PER_FORWARD_CODE___image].values[0].offset = UTILEX_OFFSETOF(dnx_data_aod_tables_EGRESS_PER_FORWARD_CODE___image_t, image);

    
    submodule_data->tables[dnx_data_aod_tables_table_EGRESS_PER_FORWARD_CODE].name = "EGRESS_PER_FORWARD_CODE";
    submodule_data->tables[dnx_data_aod_tables_table_EGRESS_PER_FORWARD_CODE].doc = "EGRESS_PER_FORWARD_CODE";
    submodule_data->tables[dnx_data_aod_tables_table_EGRESS_PER_FORWARD_CODE].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_aod_tables_table_EGRESS_PER_FORWARD_CODE].size_of_values = sizeof(dnx_data_aod_tables_EGRESS_PER_FORWARD_CODE_t);
    submodule_data->tables[dnx_data_aod_tables_table_EGRESS_PER_FORWARD_CODE].entry_get = dnx_data_aod_tables_EGRESS_PER_FORWARD_CODE_entry_str_get;

    
    submodule_data->tables[dnx_data_aod_tables_table_EGRESS_PER_FORWARD_CODE].nof_keys = 2;
    submodule_data->tables[dnx_data_aod_tables_table_EGRESS_PER_FORWARD_CODE].keys[0].name = "index";
    submodule_data->tables[dnx_data_aod_tables_table_EGRESS_PER_FORWARD_CODE].keys[0].doc = "entry index";
    submodule_data->tables[dnx_data_aod_tables_table_EGRESS_PER_FORWARD_CODE].keys[1].name = "soc_value";
    submodule_data->tables[dnx_data_aod_tables_table_EGRESS_PER_FORWARD_CODE].keys[1].doc = "soc property value";

    
    submodule_data->tables[dnx_data_aod_tables_table_EGRESS_PER_FORWARD_CODE].nof_values = 33;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_aod_tables_table_EGRESS_PER_FORWARD_CODE].values, dnxc_data_value_t, submodule_data->tables[dnx_data_aod_tables_table_EGRESS_PER_FORWARD_CODE].nof_values, "_dnx_data_aod_tables_table_EGRESS_PER_FORWARD_CODE table values");
    submodule_data->tables[dnx_data_aod_tables_table_EGRESS_PER_FORWARD_CODE].values[0].name = "valid";
    submodule_data->tables[dnx_data_aod_tables_table_EGRESS_PER_FORWARD_CODE].values[0].type = "uint32";
    submodule_data->tables[dnx_data_aod_tables_table_EGRESS_PER_FORWARD_CODE].values[0].doc = "indicates whether entry is valid";
    submodule_data->tables[dnx_data_aod_tables_table_EGRESS_PER_FORWARD_CODE].values[0].offset = UTILEX_OFFSETOF(dnx_data_aod_tables_EGRESS_PER_FORWARD_CODE_t, valid);
    submodule_data->tables[dnx_data_aod_tables_table_EGRESS_PER_FORWARD_CODE].values[1].name = "doc";
    submodule_data->tables[dnx_data_aod_tables_table_EGRESS_PER_FORWARD_CODE].values[1].type = "char *";
    submodule_data->tables[dnx_data_aod_tables_table_EGRESS_PER_FORWARD_CODE].values[1].doc = "entry documentation";
    submodule_data->tables[dnx_data_aod_tables_table_EGRESS_PER_FORWARD_CODE].values[1].offset = UTILEX_OFFSETOF(dnx_data_aod_tables_EGRESS_PER_FORWARD_CODE_t, doc);
    submodule_data->tables[dnx_data_aod_tables_table_EGRESS_PER_FORWARD_CODE].values[2].name = "Egress_FWD_Code";
    submodule_data->tables[dnx_data_aod_tables_table_EGRESS_PER_FORWARD_CODE].values[2].type = "char *";
    submodule_data->tables[dnx_data_aod_tables_table_EGRESS_PER_FORWARD_CODE].values[2].doc = "Egress_FWD_Code";
    submodule_data->tables[dnx_data_aod_tables_table_EGRESS_PER_FORWARD_CODE].values[2].offset = UTILEX_OFFSETOF(dnx_data_aod_tables_EGRESS_PER_FORWARD_CODE_t, Egress_FWD_Code);
    submodule_data->tables[dnx_data_aod_tables_table_EGRESS_PER_FORWARD_CODE].values[3].name = "Egress_FWD_Code_mask";
    submodule_data->tables[dnx_data_aod_tables_table_EGRESS_PER_FORWARD_CODE].values[3].type = "uint32";
    submodule_data->tables[dnx_data_aod_tables_table_EGRESS_PER_FORWARD_CODE].values[3].doc = "Egress_FWD_Code_mask";
    submodule_data->tables[dnx_data_aod_tables_table_EGRESS_PER_FORWARD_CODE].values[3].offset = UTILEX_OFFSETOF(dnx_data_aod_tables_EGRESS_PER_FORWARD_CODE_t, Egress_FWD_Code_mask);
    submodule_data->tables[dnx_data_aod_tables_table_EGRESS_PER_FORWARD_CODE].values[4].name = "Egress_FWD_Code_valid";
    submodule_data->tables[dnx_data_aod_tables_table_EGRESS_PER_FORWARD_CODE].values[4].type = "uint32";
    submodule_data->tables[dnx_data_aod_tables_table_EGRESS_PER_FORWARD_CODE].values[4].doc = "Egress_FWD_Code_valid";
    submodule_data->tables[dnx_data_aod_tables_table_EGRESS_PER_FORWARD_CODE].values[4].offset = UTILEX_OFFSETOF(dnx_data_aod_tables_EGRESS_PER_FORWARD_CODE_t, Egress_FWD_Code_valid);
    submodule_data->tables[dnx_data_aod_tables_table_EGRESS_PER_FORWARD_CODE].values[5].name = "Acceptable_Frame_Type_Enable";
    submodule_data->tables[dnx_data_aod_tables_table_EGRESS_PER_FORWARD_CODE].values[5].type = "uint32";
    submodule_data->tables[dnx_data_aod_tables_table_EGRESS_PER_FORWARD_CODE].values[5].doc = "Acceptable_Frame_Type_Enable";
    submodule_data->tables[dnx_data_aod_tables_table_EGRESS_PER_FORWARD_CODE].values[5].offset = UTILEX_OFFSETOF(dnx_data_aod_tables_EGRESS_PER_FORWARD_CODE_t, Acceptable_Frame_Type_Enable);
    submodule_data->tables[dnx_data_aod_tables_table_EGRESS_PER_FORWARD_CODE].values[6].name = "Acceptable_Frame_Type_Enable_valid";
    submodule_data->tables[dnx_data_aod_tables_table_EGRESS_PER_FORWARD_CODE].values[6].type = "uint32";
    submodule_data->tables[dnx_data_aod_tables_table_EGRESS_PER_FORWARD_CODE].values[6].doc = "Acceptable_Frame_Type_Enable_valid";
    submodule_data->tables[dnx_data_aod_tables_table_EGRESS_PER_FORWARD_CODE].values[6].offset = UTILEX_OFFSETOF(dnx_data_aod_tables_EGRESS_PER_FORWARD_CODE_t, Acceptable_Frame_Type_Enable_valid);
    submodule_data->tables[dnx_data_aod_tables_table_EGRESS_PER_FORWARD_CODE].values[7].name = "FWD_Context_is_PP_Enable";
    submodule_data->tables[dnx_data_aod_tables_table_EGRESS_PER_FORWARD_CODE].values[7].type = "uint32";
    submodule_data->tables[dnx_data_aod_tables_table_EGRESS_PER_FORWARD_CODE].values[7].doc = "FWD_Context_is_PP_Enable";
    submodule_data->tables[dnx_data_aod_tables_table_EGRESS_PER_FORWARD_CODE].values[7].offset = UTILEX_OFFSETOF(dnx_data_aod_tables_EGRESS_PER_FORWARD_CODE_t, FWD_Context_is_PP_Enable);
    submodule_data->tables[dnx_data_aod_tables_table_EGRESS_PER_FORWARD_CODE].values[8].name = "FWD_Context_is_PP_Enable_valid";
    submodule_data->tables[dnx_data_aod_tables_table_EGRESS_PER_FORWARD_CODE].values[8].type = "uint32";
    submodule_data->tables[dnx_data_aod_tables_table_EGRESS_PER_FORWARD_CODE].values[8].doc = "FWD_Context_is_PP_Enable_valid";
    submodule_data->tables[dnx_data_aod_tables_table_EGRESS_PER_FORWARD_CODE].values[8].offset = UTILEX_OFFSETOF(dnx_data_aod_tables_EGRESS_PER_FORWARD_CODE_t, FWD_Context_is_PP_Enable_valid);
    submodule_data->tables[dnx_data_aod_tables_table_EGRESS_PER_FORWARD_CODE].values[9].name = "Forward_Code_IPv4_or_IPv6";
    submodule_data->tables[dnx_data_aod_tables_table_EGRESS_PER_FORWARD_CODE].values[9].type = "uint32";
    submodule_data->tables[dnx_data_aod_tables_table_EGRESS_PER_FORWARD_CODE].values[9].doc = "Forward_Code_IPv4_or_IPv6";
    submodule_data->tables[dnx_data_aod_tables_table_EGRESS_PER_FORWARD_CODE].values[9].offset = UTILEX_OFFSETOF(dnx_data_aod_tables_EGRESS_PER_FORWARD_CODE_t, Forward_Code_IPv4_or_IPv6);
    submodule_data->tables[dnx_data_aod_tables_table_EGRESS_PER_FORWARD_CODE].values[10].name = "Forward_Code_IPv4_or_IPv6_valid";
    submodule_data->tables[dnx_data_aod_tables_table_EGRESS_PER_FORWARD_CODE].values[10].type = "uint32";
    submodule_data->tables[dnx_data_aod_tables_table_EGRESS_PER_FORWARD_CODE].values[10].doc = "Forward_Code_IPv4_or_IPv6_valid";
    submodule_data->tables[dnx_data_aod_tables_table_EGRESS_PER_FORWARD_CODE].values[10].offset = UTILEX_OFFSETOF(dnx_data_aod_tables_EGRESS_PER_FORWARD_CODE_t, Forward_Code_IPv4_or_IPv6_valid);
    submodule_data->tables[dnx_data_aod_tables_table_EGRESS_PER_FORWARD_CODE].values[11].name = "Learn_Enable";
    submodule_data->tables[dnx_data_aod_tables_table_EGRESS_PER_FORWARD_CODE].values[11].type = "uint32";
    submodule_data->tables[dnx_data_aod_tables_table_EGRESS_PER_FORWARD_CODE].values[11].doc = "Learn_Enable";
    submodule_data->tables[dnx_data_aod_tables_table_EGRESS_PER_FORWARD_CODE].values[11].offset = UTILEX_OFFSETOF(dnx_data_aod_tables_EGRESS_PER_FORWARD_CODE_t, Learn_Enable);
    submodule_data->tables[dnx_data_aod_tables_table_EGRESS_PER_FORWARD_CODE].values[12].name = "Learn_Enable_valid";
    submodule_data->tables[dnx_data_aod_tables_table_EGRESS_PER_FORWARD_CODE].values[12].type = "uint32";
    submodule_data->tables[dnx_data_aod_tables_table_EGRESS_PER_FORWARD_CODE].values[12].doc = "Learn_Enable_valid";
    submodule_data->tables[dnx_data_aod_tables_table_EGRESS_PER_FORWARD_CODE].values[12].offset = UTILEX_OFFSETOF(dnx_data_aod_tables_EGRESS_PER_FORWARD_CODE_t, Learn_Enable_valid);
    submodule_data->tables[dnx_data_aod_tables_table_EGRESS_PER_FORWARD_CODE].values[13].name = "Protocol_Ethernet_Enable";
    submodule_data->tables[dnx_data_aod_tables_table_EGRESS_PER_FORWARD_CODE].values[13].type = "uint32";
    submodule_data->tables[dnx_data_aod_tables_table_EGRESS_PER_FORWARD_CODE].values[13].doc = "Protocol_Ethernet_Enable";
    submodule_data->tables[dnx_data_aod_tables_table_EGRESS_PER_FORWARD_CODE].values[13].offset = UTILEX_OFFSETOF(dnx_data_aod_tables_EGRESS_PER_FORWARD_CODE_t, Protocol_Ethernet_Enable);
    submodule_data->tables[dnx_data_aod_tables_table_EGRESS_PER_FORWARD_CODE].values[14].name = "Protocol_Ethernet_Enable_valid";
    submodule_data->tables[dnx_data_aod_tables_table_EGRESS_PER_FORWARD_CODE].values[14].type = "uint32";
    submodule_data->tables[dnx_data_aod_tables_table_EGRESS_PER_FORWARD_CODE].values[14].doc = "Protocol_Ethernet_Enable_valid";
    submodule_data->tables[dnx_data_aod_tables_table_EGRESS_PER_FORWARD_CODE].values[14].offset = UTILEX_OFFSETOF(dnx_data_aod_tables_EGRESS_PER_FORWARD_CODE_t, Protocol_Ethernet_Enable_valid);
    submodule_data->tables[dnx_data_aod_tables_table_EGRESS_PER_FORWARD_CODE].values[15].name = "Protocol_IPv4_MC_Enable";
    submodule_data->tables[dnx_data_aod_tables_table_EGRESS_PER_FORWARD_CODE].values[15].type = "uint32";
    submodule_data->tables[dnx_data_aod_tables_table_EGRESS_PER_FORWARD_CODE].values[15].doc = "Protocol_IPv4_MC_Enable";
    submodule_data->tables[dnx_data_aod_tables_table_EGRESS_PER_FORWARD_CODE].values[15].offset = UTILEX_OFFSETOF(dnx_data_aod_tables_EGRESS_PER_FORWARD_CODE_t, Protocol_IPv4_MC_Enable);
    submodule_data->tables[dnx_data_aod_tables_table_EGRESS_PER_FORWARD_CODE].values[16].name = "Protocol_IPv4_MC_Enable_valid";
    submodule_data->tables[dnx_data_aod_tables_table_EGRESS_PER_FORWARD_CODE].values[16].type = "uint32";
    submodule_data->tables[dnx_data_aod_tables_table_EGRESS_PER_FORWARD_CODE].values[16].doc = "Protocol_IPv4_MC_Enable_valid";
    submodule_data->tables[dnx_data_aod_tables_table_EGRESS_PER_FORWARD_CODE].values[16].offset = UTILEX_OFFSETOF(dnx_data_aod_tables_EGRESS_PER_FORWARD_CODE_t, Protocol_IPv4_MC_Enable_valid);
    submodule_data->tables[dnx_data_aod_tables_table_EGRESS_PER_FORWARD_CODE].values[17].name = "Protocol_IPv4_UC_Enable";
    submodule_data->tables[dnx_data_aod_tables_table_EGRESS_PER_FORWARD_CODE].values[17].type = "uint32";
    submodule_data->tables[dnx_data_aod_tables_table_EGRESS_PER_FORWARD_CODE].values[17].doc = "Protocol_IPv4_UC_Enable";
    submodule_data->tables[dnx_data_aod_tables_table_EGRESS_PER_FORWARD_CODE].values[17].offset = UTILEX_OFFSETOF(dnx_data_aod_tables_EGRESS_PER_FORWARD_CODE_t, Protocol_IPv4_UC_Enable);
    submodule_data->tables[dnx_data_aod_tables_table_EGRESS_PER_FORWARD_CODE].values[18].name = "Protocol_IPv4_UC_Enable_valid";
    submodule_data->tables[dnx_data_aod_tables_table_EGRESS_PER_FORWARD_CODE].values[18].type = "uint32";
    submodule_data->tables[dnx_data_aod_tables_table_EGRESS_PER_FORWARD_CODE].values[18].doc = "Protocol_IPv4_UC_Enable_valid";
    submodule_data->tables[dnx_data_aod_tables_table_EGRESS_PER_FORWARD_CODE].values[18].offset = UTILEX_OFFSETOF(dnx_data_aod_tables_EGRESS_PER_FORWARD_CODE_t, Protocol_IPv4_UC_Enable_valid);
    submodule_data->tables[dnx_data_aod_tables_table_EGRESS_PER_FORWARD_CODE].values[19].name = "Protocol_IPv6_MC_Enable";
    submodule_data->tables[dnx_data_aod_tables_table_EGRESS_PER_FORWARD_CODE].values[19].type = "uint32";
    submodule_data->tables[dnx_data_aod_tables_table_EGRESS_PER_FORWARD_CODE].values[19].doc = "Protocol_IPv6_MC_Enable";
    submodule_data->tables[dnx_data_aod_tables_table_EGRESS_PER_FORWARD_CODE].values[19].offset = UTILEX_OFFSETOF(dnx_data_aod_tables_EGRESS_PER_FORWARD_CODE_t, Protocol_IPv6_MC_Enable);
    submodule_data->tables[dnx_data_aod_tables_table_EGRESS_PER_FORWARD_CODE].values[20].name = "Protocol_IPv6_MC_Enable_valid";
    submodule_data->tables[dnx_data_aod_tables_table_EGRESS_PER_FORWARD_CODE].values[20].type = "uint32";
    submodule_data->tables[dnx_data_aod_tables_table_EGRESS_PER_FORWARD_CODE].values[20].doc = "Protocol_IPv6_MC_Enable_valid";
    submodule_data->tables[dnx_data_aod_tables_table_EGRESS_PER_FORWARD_CODE].values[20].offset = UTILEX_OFFSETOF(dnx_data_aod_tables_EGRESS_PER_FORWARD_CODE_t, Protocol_IPv6_MC_Enable_valid);
    submodule_data->tables[dnx_data_aod_tables_table_EGRESS_PER_FORWARD_CODE].values[21].name = "Protocol_IPv6_UC_Enable";
    submodule_data->tables[dnx_data_aod_tables_table_EGRESS_PER_FORWARD_CODE].values[21].type = "uint32";
    submodule_data->tables[dnx_data_aod_tables_table_EGRESS_PER_FORWARD_CODE].values[21].doc = "Protocol_IPv6_UC_Enable";
    submodule_data->tables[dnx_data_aod_tables_table_EGRESS_PER_FORWARD_CODE].values[21].offset = UTILEX_OFFSETOF(dnx_data_aod_tables_EGRESS_PER_FORWARD_CODE_t, Protocol_IPv6_UC_Enable);
    submodule_data->tables[dnx_data_aod_tables_table_EGRESS_PER_FORWARD_CODE].values[22].name = "Protocol_IPv6_UC_Enable_valid";
    submodule_data->tables[dnx_data_aod_tables_table_EGRESS_PER_FORWARD_CODE].values[22].type = "uint32";
    submodule_data->tables[dnx_data_aod_tables_table_EGRESS_PER_FORWARD_CODE].values[22].doc = "Protocol_IPv6_UC_Enable_valid";
    submodule_data->tables[dnx_data_aod_tables_table_EGRESS_PER_FORWARD_CODE].values[22].offset = UTILEX_OFFSETOF(dnx_data_aod_tables_EGRESS_PER_FORWARD_CODE_t, Protocol_IPv6_UC_Enable_valid);
    submodule_data->tables[dnx_data_aod_tables_table_EGRESS_PER_FORWARD_CODE].values[23].name = "Protocol_MPLS_Enable";
    submodule_data->tables[dnx_data_aod_tables_table_EGRESS_PER_FORWARD_CODE].values[23].type = "uint32";
    submodule_data->tables[dnx_data_aod_tables_table_EGRESS_PER_FORWARD_CODE].values[23].doc = "Protocol_MPLS_Enable";
    submodule_data->tables[dnx_data_aod_tables_table_EGRESS_PER_FORWARD_CODE].values[23].offset = UTILEX_OFFSETOF(dnx_data_aod_tables_EGRESS_PER_FORWARD_CODE_t, Protocol_MPLS_Enable);
    submodule_data->tables[dnx_data_aod_tables_table_EGRESS_PER_FORWARD_CODE].values[24].name = "Protocol_MPLS_Enable_valid";
    submodule_data->tables[dnx_data_aod_tables_table_EGRESS_PER_FORWARD_CODE].values[24].type = "uint32";
    submodule_data->tables[dnx_data_aod_tables_table_EGRESS_PER_FORWARD_CODE].values[24].doc = "Protocol_MPLS_Enable_valid";
    submodule_data->tables[dnx_data_aod_tables_table_EGRESS_PER_FORWARD_CODE].values[24].offset = UTILEX_OFFSETOF(dnx_data_aod_tables_EGRESS_PER_FORWARD_CODE_t, Protocol_MPLS_Enable_valid);
    submodule_data->tables[dnx_data_aod_tables_table_EGRESS_PER_FORWARD_CODE].values[25].name = "STP_Enable";
    submodule_data->tables[dnx_data_aod_tables_table_EGRESS_PER_FORWARD_CODE].values[25].type = "uint32";
    submodule_data->tables[dnx_data_aod_tables_table_EGRESS_PER_FORWARD_CODE].values[25].doc = "STP_Enable";
    submodule_data->tables[dnx_data_aod_tables_table_EGRESS_PER_FORWARD_CODE].values[25].offset = UTILEX_OFFSETOF(dnx_data_aod_tables_EGRESS_PER_FORWARD_CODE_t, STP_Enable);
    submodule_data->tables[dnx_data_aod_tables_table_EGRESS_PER_FORWARD_CODE].values[26].name = "STP_Enable_valid";
    submodule_data->tables[dnx_data_aod_tables_table_EGRESS_PER_FORWARD_CODE].values[26].type = "uint32";
    submodule_data->tables[dnx_data_aod_tables_table_EGRESS_PER_FORWARD_CODE].values[26].doc = "STP_Enable_valid";
    submodule_data->tables[dnx_data_aod_tables_table_EGRESS_PER_FORWARD_CODE].values[26].offset = UTILEX_OFFSETOF(dnx_data_aod_tables_EGRESS_PER_FORWARD_CODE_t, STP_Enable_valid);
    submodule_data->tables[dnx_data_aod_tables_table_EGRESS_PER_FORWARD_CODE].values[27].name = "Split_Horizon_Enable";
    submodule_data->tables[dnx_data_aod_tables_table_EGRESS_PER_FORWARD_CODE].values[27].type = "uint32";
    submodule_data->tables[dnx_data_aod_tables_table_EGRESS_PER_FORWARD_CODE].values[27].doc = "Split_Horizon_Enable";
    submodule_data->tables[dnx_data_aod_tables_table_EGRESS_PER_FORWARD_CODE].values[27].offset = UTILEX_OFFSETOF(dnx_data_aod_tables_EGRESS_PER_FORWARD_CODE_t, Split_Horizon_Enable);
    submodule_data->tables[dnx_data_aod_tables_table_EGRESS_PER_FORWARD_CODE].values[28].name = "Split_Horizon_Enable_valid";
    submodule_data->tables[dnx_data_aod_tables_table_EGRESS_PER_FORWARD_CODE].values[28].type = "uint32";
    submodule_data->tables[dnx_data_aod_tables_table_EGRESS_PER_FORWARD_CODE].values[28].doc = "Split_Horizon_Enable_valid";
    submodule_data->tables[dnx_data_aod_tables_table_EGRESS_PER_FORWARD_CODE].values[28].offset = UTILEX_OFFSETOF(dnx_data_aod_tables_EGRESS_PER_FORWARD_CODE_t, Split_Horizon_Enable_valid);
    submodule_data->tables[dnx_data_aod_tables_table_EGRESS_PER_FORWARD_CODE].values[29].name = "Termination_Enable";
    submodule_data->tables[dnx_data_aod_tables_table_EGRESS_PER_FORWARD_CODE].values[29].type = "uint32";
    submodule_data->tables[dnx_data_aod_tables_table_EGRESS_PER_FORWARD_CODE].values[29].doc = "Termination_Enable";
    submodule_data->tables[dnx_data_aod_tables_table_EGRESS_PER_FORWARD_CODE].values[29].offset = UTILEX_OFFSETOF(dnx_data_aod_tables_EGRESS_PER_FORWARD_CODE_t, Termination_Enable);
    submodule_data->tables[dnx_data_aod_tables_table_EGRESS_PER_FORWARD_CODE].values[30].name = "Termination_Enable_valid";
    submodule_data->tables[dnx_data_aod_tables_table_EGRESS_PER_FORWARD_CODE].values[30].type = "uint32";
    submodule_data->tables[dnx_data_aod_tables_table_EGRESS_PER_FORWARD_CODE].values[30].doc = "Termination_Enable_valid";
    submodule_data->tables[dnx_data_aod_tables_table_EGRESS_PER_FORWARD_CODE].values[30].offset = UTILEX_OFFSETOF(dnx_data_aod_tables_EGRESS_PER_FORWARD_CODE_t, Termination_Enable_valid);
    submodule_data->tables[dnx_data_aod_tables_table_EGRESS_PER_FORWARD_CODE].values[31].name = "Vlan_Membership_Enable";
    submodule_data->tables[dnx_data_aod_tables_table_EGRESS_PER_FORWARD_CODE].values[31].type = "uint32";
    submodule_data->tables[dnx_data_aod_tables_table_EGRESS_PER_FORWARD_CODE].values[31].doc = "Vlan_Membership_Enable";
    submodule_data->tables[dnx_data_aod_tables_table_EGRESS_PER_FORWARD_CODE].values[31].offset = UTILEX_OFFSETOF(dnx_data_aod_tables_EGRESS_PER_FORWARD_CODE_t, Vlan_Membership_Enable);
    submodule_data->tables[dnx_data_aod_tables_table_EGRESS_PER_FORWARD_CODE].values[32].name = "Vlan_Membership_Enable_valid";
    submodule_data->tables[dnx_data_aod_tables_table_EGRESS_PER_FORWARD_CODE].values[32].type = "uint32";
    submodule_data->tables[dnx_data_aod_tables_table_EGRESS_PER_FORWARD_CODE].values[32].doc = "Vlan_Membership_Enable_valid";
    submodule_data->tables[dnx_data_aod_tables_table_EGRESS_PER_FORWARD_CODE].values[32].offset = UTILEX_OFFSETOF(dnx_data_aod_tables_EGRESS_PER_FORWARD_CODE_t, Vlan_Membership_Enable_valid);

    
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FORWARD_CODE_SELECTION___image].name = "ERPP_FORWARD_CODE_SELECTION___image";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FORWARD_CODE_SELECTION___image].doc = "table related image";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FORWARD_CODE_SELECTION___image].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FORWARD_CODE_SELECTION___image].size_of_values = sizeof(dnx_data_aod_tables_ERPP_FORWARD_CODE_SELECTION___image_t);
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FORWARD_CODE_SELECTION___image].entry_get = dnx_data_aod_tables_ERPP_FORWARD_CODE_SELECTION___image_entry_str_get;

    
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FORWARD_CODE_SELECTION___image].nof_keys = 0;

    
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FORWARD_CODE_SELECTION___image].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_aod_tables_table_ERPP_FORWARD_CODE_SELECTION___image].values, dnxc_data_value_t, submodule_data->tables[dnx_data_aod_tables_table_ERPP_FORWARD_CODE_SELECTION___image].nof_values, "_dnx_data_aod_tables_table_ERPP_FORWARD_CODE_SELECTION___image table values");
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FORWARD_CODE_SELECTION___image].values[0].name = "image";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FORWARD_CODE_SELECTION___image].values[0].type = "char *";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FORWARD_CODE_SELECTION___image].values[0].doc = "related image name";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FORWARD_CODE_SELECTION___image].values[0].offset = UTILEX_OFFSETOF(dnx_data_aod_tables_ERPP_FORWARD_CODE_SELECTION___image_t, image);

    
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FORWARD_CODE_SELECTION].name = "ERPP_FORWARD_CODE_SELECTION";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FORWARD_CODE_SELECTION].doc = "ERPP_FORWARD_CODE_SELECTION";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FORWARD_CODE_SELECTION].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FORWARD_CODE_SELECTION].size_of_values = sizeof(dnx_data_aod_tables_ERPP_FORWARD_CODE_SELECTION_t);
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FORWARD_CODE_SELECTION].entry_get = dnx_data_aod_tables_ERPP_FORWARD_CODE_SELECTION_entry_str_get;

    
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FORWARD_CODE_SELECTION].nof_keys = 2;
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FORWARD_CODE_SELECTION].keys[0].name = "index";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FORWARD_CODE_SELECTION].keys[0].doc = "entry index";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FORWARD_CODE_SELECTION].keys[1].name = "soc_value";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FORWARD_CODE_SELECTION].keys[1].doc = "soc property value";

    
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FORWARD_CODE_SELECTION].nof_values = 28;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_aod_tables_table_ERPP_FORWARD_CODE_SELECTION].values, dnxc_data_value_t, submodule_data->tables[dnx_data_aod_tables_table_ERPP_FORWARD_CODE_SELECTION].nof_values, "_dnx_data_aod_tables_table_ERPP_FORWARD_CODE_SELECTION table values");
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FORWARD_CODE_SELECTION].values[0].name = "valid";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FORWARD_CODE_SELECTION].values[0].type = "uint32";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FORWARD_CODE_SELECTION].values[0].doc = "indicates whether entry is valid";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FORWARD_CODE_SELECTION].values[0].offset = UTILEX_OFFSETOF(dnx_data_aod_tables_ERPP_FORWARD_CODE_SELECTION_t, valid);
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FORWARD_CODE_SELECTION].values[1].name = "doc";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FORWARD_CODE_SELECTION].values[1].type = "char *";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FORWARD_CODE_SELECTION].values[1].doc = "entry documentation";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FORWARD_CODE_SELECTION].values[1].offset = UTILEX_OFFSETOF(dnx_data_aod_tables_ERPP_FORWARD_CODE_SELECTION_t, doc);
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FORWARD_CODE_SELECTION].values[2].name = "FAI";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FORWARD_CODE_SELECTION].values[2].type = "uint32";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FORWARD_CODE_SELECTION].values[2].doc = "FAI";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FORWARD_CODE_SELECTION].values[2].offset = UTILEX_OFFSETOF(dnx_data_aod_tables_ERPP_FORWARD_CODE_SELECTION_t, FAI);
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FORWARD_CODE_SELECTION].values[3].name = "FAI_mask";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FORWARD_CODE_SELECTION].values[3].type = "uint32";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FORWARD_CODE_SELECTION].values[3].doc = "FAI_mask";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FORWARD_CODE_SELECTION].values[3].offset = UTILEX_OFFSETOF(dnx_data_aod_tables_ERPP_FORWARD_CODE_SELECTION_t, FAI_mask);
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FORWARD_CODE_SELECTION].values[4].name = "FAI_valid";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FORWARD_CODE_SELECTION].values[4].type = "uint32";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FORWARD_CODE_SELECTION].values[4].doc = "FAI_valid";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FORWARD_CODE_SELECTION].values[4].offset = UTILEX_OFFSETOF(dnx_data_aod_tables_ERPP_FORWARD_CODE_SELECTION_t, FAI_valid);
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FORWARD_CODE_SELECTION].values[5].name = "FWD_Layer_Qualifier";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FORWARD_CODE_SELECTION].values[5].type = "uint32";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FORWARD_CODE_SELECTION].values[5].doc = "FWD_Layer_Qualifier";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FORWARD_CODE_SELECTION].values[5].offset = UTILEX_OFFSETOF(dnx_data_aod_tables_ERPP_FORWARD_CODE_SELECTION_t, FWD_Layer_Qualifier);
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FORWARD_CODE_SELECTION].values[6].name = "FWD_Layer_Qualifier_mask";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FORWARD_CODE_SELECTION].values[6].type = "uint32";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FORWARD_CODE_SELECTION].values[6].doc = "FWD_Layer_Qualifier_mask";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FORWARD_CODE_SELECTION].values[6].offset = UTILEX_OFFSETOF(dnx_data_aod_tables_ERPP_FORWARD_CODE_SELECTION_t, FWD_Layer_Qualifier_mask);
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FORWARD_CODE_SELECTION].values[7].name = "FWD_Layer_Qualifier_valid";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FORWARD_CODE_SELECTION].values[7].type = "uint32";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FORWARD_CODE_SELECTION].values[7].doc = "FWD_Layer_Qualifier_valid";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FORWARD_CODE_SELECTION].values[7].offset = UTILEX_OFFSETOF(dnx_data_aod_tables_ERPP_FORWARD_CODE_SELECTION_t, FWD_Layer_Qualifier_valid);
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FORWARD_CODE_SELECTION].values[8].name = "FWD_Layer_Type";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FORWARD_CODE_SELECTION].values[8].type = "char *";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FORWARD_CODE_SELECTION].values[8].doc = "FWD_Layer_Type";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FORWARD_CODE_SELECTION].values[8].offset = UTILEX_OFFSETOF(dnx_data_aod_tables_ERPP_FORWARD_CODE_SELECTION_t, FWD_Layer_Type);
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FORWARD_CODE_SELECTION].values[9].name = "FWD_Layer_Type_mask";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FORWARD_CODE_SELECTION].values[9].type = "uint32";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FORWARD_CODE_SELECTION].values[9].doc = "FWD_Layer_Type_mask";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FORWARD_CODE_SELECTION].values[9].offset = UTILEX_OFFSETOF(dnx_data_aod_tables_ERPP_FORWARD_CODE_SELECTION_t, FWD_Layer_Type_mask);
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FORWARD_CODE_SELECTION].values[10].name = "FWD_Layer_Type_valid";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FORWARD_CODE_SELECTION].values[10].type = "uint32";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FORWARD_CODE_SELECTION].values[10].doc = "FWD_Layer_Type_valid";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FORWARD_CODE_SELECTION].values[10].offset = UTILEX_OFFSETOF(dnx_data_aod_tables_ERPP_FORWARD_CODE_SELECTION_t, FWD_Layer_Type_valid);
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FORWARD_CODE_SELECTION].values[11].name = "FWD_code_port_profile";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FORWARD_CODE_SELECTION].values[11].type = "char *";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FORWARD_CODE_SELECTION].values[11].doc = "FWD_code_port_profile";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FORWARD_CODE_SELECTION].values[11].offset = UTILEX_OFFSETOF(dnx_data_aod_tables_ERPP_FORWARD_CODE_SELECTION_t, FWD_code_port_profile);
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FORWARD_CODE_SELECTION].values[12].name = "FWD_code_port_profile_mask";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FORWARD_CODE_SELECTION].values[12].type = "uint32";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FORWARD_CODE_SELECTION].values[12].doc = "FWD_code_port_profile_mask";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FORWARD_CODE_SELECTION].values[12].offset = UTILEX_OFFSETOF(dnx_data_aod_tables_ERPP_FORWARD_CODE_SELECTION_t, FWD_code_port_profile_mask);
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FORWARD_CODE_SELECTION].values[13].name = "FWD_code_port_profile_valid";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FORWARD_CODE_SELECTION].values[13].type = "uint32";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FORWARD_CODE_SELECTION].values[13].doc = "FWD_code_port_profile_valid";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FORWARD_CODE_SELECTION].values[13].offset = UTILEX_OFFSETOF(dnx_data_aod_tables_ERPP_FORWARD_CODE_SELECTION_t, FWD_code_port_profile_valid);
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FORWARD_CODE_SELECTION].values[14].name = "TM_Action_Type";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FORWARD_CODE_SELECTION].values[14].type = "uint32";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FORWARD_CODE_SELECTION].values[14].doc = "TM_Action_Type";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FORWARD_CODE_SELECTION].values[14].offset = UTILEX_OFFSETOF(dnx_data_aod_tables_ERPP_FORWARD_CODE_SELECTION_t, TM_Action_Type);
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FORWARD_CODE_SELECTION].values[15].name = "TM_Action_Type_mask";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FORWARD_CODE_SELECTION].values[15].type = "uint32";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FORWARD_CODE_SELECTION].values[15].doc = "TM_Action_Type_mask";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FORWARD_CODE_SELECTION].values[15].offset = UTILEX_OFFSETOF(dnx_data_aod_tables_ERPP_FORWARD_CODE_SELECTION_t, TM_Action_Type_mask);
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FORWARD_CODE_SELECTION].values[16].name = "TM_Action_Type_valid";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FORWARD_CODE_SELECTION].values[16].type = "uint32";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FORWARD_CODE_SELECTION].values[16].doc = "TM_Action_Type_valid";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FORWARD_CODE_SELECTION].values[16].offset = UTILEX_OFFSETOF(dnx_data_aod_tables_ERPP_FORWARD_CODE_SELECTION_t, TM_Action_Type_valid);
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FORWARD_CODE_SELECTION].values[17].name = "ingress_trapped";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FORWARD_CODE_SELECTION].values[17].type = "uint32";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FORWARD_CODE_SELECTION].values[17].doc = "ingress_trapped";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FORWARD_CODE_SELECTION].values[17].offset = UTILEX_OFFSETOF(dnx_data_aod_tables_ERPP_FORWARD_CODE_SELECTION_t, ingress_trapped);
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FORWARD_CODE_SELECTION].values[18].name = "ingress_trapped_mask";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FORWARD_CODE_SELECTION].values[18].type = "uint32";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FORWARD_CODE_SELECTION].values[18].doc = "ingress_trapped_mask";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FORWARD_CODE_SELECTION].values[18].offset = UTILEX_OFFSETOF(dnx_data_aod_tables_ERPP_FORWARD_CODE_SELECTION_t, ingress_trapped_mask);
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FORWARD_CODE_SELECTION].values[19].name = "ingress_trapped_valid";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FORWARD_CODE_SELECTION].values[19].type = "uint32";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FORWARD_CODE_SELECTION].values[19].doc = "ingress_trapped_valid";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FORWARD_CODE_SELECTION].values[19].offset = UTILEX_OFFSETOF(dnx_data_aod_tables_ERPP_FORWARD_CODE_SELECTION_t, ingress_trapped_valid);
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FORWARD_CODE_SELECTION].values[20].name = "is_Applet";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FORWARD_CODE_SELECTION].values[20].type = "uint32";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FORWARD_CODE_SELECTION].values[20].doc = "is_Applet";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FORWARD_CODE_SELECTION].values[20].offset = UTILEX_OFFSETOF(dnx_data_aod_tables_ERPP_FORWARD_CODE_SELECTION_t, is_Applet);
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FORWARD_CODE_SELECTION].values[21].name = "is_Applet_mask";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FORWARD_CODE_SELECTION].values[21].type = "uint32";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FORWARD_CODE_SELECTION].values[21].doc = "is_Applet_mask";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FORWARD_CODE_SELECTION].values[21].offset = UTILEX_OFFSETOF(dnx_data_aod_tables_ERPP_FORWARD_CODE_SELECTION_t, is_Applet_mask);
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FORWARD_CODE_SELECTION].values[22].name = "is_Applet_valid";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FORWARD_CODE_SELECTION].values[22].type = "uint32";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FORWARD_CODE_SELECTION].values[22].doc = "is_Applet_valid";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FORWARD_CODE_SELECTION].values[22].offset = UTILEX_OFFSETOF(dnx_data_aod_tables_ERPP_FORWARD_CODE_SELECTION_t, is_Applet_valid);
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FORWARD_CODE_SELECTION].values[23].name = "is_TDM";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FORWARD_CODE_SELECTION].values[23].type = "uint32";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FORWARD_CODE_SELECTION].values[23].doc = "is_TDM";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FORWARD_CODE_SELECTION].values[23].offset = UTILEX_OFFSETOF(dnx_data_aod_tables_ERPP_FORWARD_CODE_SELECTION_t, is_TDM);
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FORWARD_CODE_SELECTION].values[24].name = "is_TDM_mask";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FORWARD_CODE_SELECTION].values[24].type = "uint32";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FORWARD_CODE_SELECTION].values[24].doc = "is_TDM_mask";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FORWARD_CODE_SELECTION].values[24].offset = UTILEX_OFFSETOF(dnx_data_aod_tables_ERPP_FORWARD_CODE_SELECTION_t, is_TDM_mask);
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FORWARD_CODE_SELECTION].values[25].name = "is_TDM_valid";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FORWARD_CODE_SELECTION].values[25].type = "uint32";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FORWARD_CODE_SELECTION].values[25].doc = "is_TDM_valid";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FORWARD_CODE_SELECTION].values[25].offset = UTILEX_OFFSETOF(dnx_data_aod_tables_ERPP_FORWARD_CODE_SELECTION_t, is_TDM_valid);
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FORWARD_CODE_SELECTION].values[26].name = "Egress_FWD_Code";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FORWARD_CODE_SELECTION].values[26].type = "char *";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FORWARD_CODE_SELECTION].values[26].doc = "Egress_FWD_Code";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FORWARD_CODE_SELECTION].values[26].offset = UTILEX_OFFSETOF(dnx_data_aod_tables_ERPP_FORWARD_CODE_SELECTION_t, Egress_FWD_Code);
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FORWARD_CODE_SELECTION].values[27].name = "Egress_FWD_Code_valid";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FORWARD_CODE_SELECTION].values[27].type = "uint32";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FORWARD_CODE_SELECTION].values[27].doc = "Egress_FWD_Code_valid";
    submodule_data->tables[dnx_data_aod_tables_table_ERPP_FORWARD_CODE_SELECTION].values[27].offset = UTILEX_OFFSETOF(dnx_data_aod_tables_ERPP_FORWARD_CODE_SELECTION_t, Egress_FWD_Code_valid);

    
    submodule_data->tables[dnx_data_aod_tables_table_ETPP_FORWARD_CODE_SELECTION___image].name = "ETPP_FORWARD_CODE_SELECTION___image";
    submodule_data->tables[dnx_data_aod_tables_table_ETPP_FORWARD_CODE_SELECTION___image].doc = "table related image";
    submodule_data->tables[dnx_data_aod_tables_table_ETPP_FORWARD_CODE_SELECTION___image].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_aod_tables_table_ETPP_FORWARD_CODE_SELECTION___image].size_of_values = sizeof(dnx_data_aod_tables_ETPP_FORWARD_CODE_SELECTION___image_t);
    submodule_data->tables[dnx_data_aod_tables_table_ETPP_FORWARD_CODE_SELECTION___image].entry_get = dnx_data_aod_tables_ETPP_FORWARD_CODE_SELECTION___image_entry_str_get;

    
    submodule_data->tables[dnx_data_aod_tables_table_ETPP_FORWARD_CODE_SELECTION___image].nof_keys = 0;

    
    submodule_data->tables[dnx_data_aod_tables_table_ETPP_FORWARD_CODE_SELECTION___image].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_aod_tables_table_ETPP_FORWARD_CODE_SELECTION___image].values, dnxc_data_value_t, submodule_data->tables[dnx_data_aod_tables_table_ETPP_FORWARD_CODE_SELECTION___image].nof_values, "_dnx_data_aod_tables_table_ETPP_FORWARD_CODE_SELECTION___image table values");
    submodule_data->tables[dnx_data_aod_tables_table_ETPP_FORWARD_CODE_SELECTION___image].values[0].name = "image";
    submodule_data->tables[dnx_data_aod_tables_table_ETPP_FORWARD_CODE_SELECTION___image].values[0].type = "char *";
    submodule_data->tables[dnx_data_aod_tables_table_ETPP_FORWARD_CODE_SELECTION___image].values[0].doc = "related image name";
    submodule_data->tables[dnx_data_aod_tables_table_ETPP_FORWARD_CODE_SELECTION___image].values[0].offset = UTILEX_OFFSETOF(dnx_data_aod_tables_ETPP_FORWARD_CODE_SELECTION___image_t, image);

    
    submodule_data->tables[dnx_data_aod_tables_table_ETPP_FORWARD_CODE_SELECTION].name = "ETPP_FORWARD_CODE_SELECTION";
    submodule_data->tables[dnx_data_aod_tables_table_ETPP_FORWARD_CODE_SELECTION].doc = "ETPP_FORWARD_CODE_SELECTION";
    submodule_data->tables[dnx_data_aod_tables_table_ETPP_FORWARD_CODE_SELECTION].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_aod_tables_table_ETPP_FORWARD_CODE_SELECTION].size_of_values = sizeof(dnx_data_aod_tables_ETPP_FORWARD_CODE_SELECTION_t);
    submodule_data->tables[dnx_data_aod_tables_table_ETPP_FORWARD_CODE_SELECTION].entry_get = dnx_data_aod_tables_ETPP_FORWARD_CODE_SELECTION_entry_str_get;

    
    submodule_data->tables[dnx_data_aod_tables_table_ETPP_FORWARD_CODE_SELECTION].nof_keys = 2;
    submodule_data->tables[dnx_data_aod_tables_table_ETPP_FORWARD_CODE_SELECTION].keys[0].name = "index";
    submodule_data->tables[dnx_data_aod_tables_table_ETPP_FORWARD_CODE_SELECTION].keys[0].doc = "entry index";
    submodule_data->tables[dnx_data_aod_tables_table_ETPP_FORWARD_CODE_SELECTION].keys[1].name = "soc_value";
    submodule_data->tables[dnx_data_aod_tables_table_ETPP_FORWARD_CODE_SELECTION].keys[1].doc = "soc property value";

    
    submodule_data->tables[dnx_data_aod_tables_table_ETPP_FORWARD_CODE_SELECTION].nof_values = 31;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_aod_tables_table_ETPP_FORWARD_CODE_SELECTION].values, dnxc_data_value_t, submodule_data->tables[dnx_data_aod_tables_table_ETPP_FORWARD_CODE_SELECTION].nof_values, "_dnx_data_aod_tables_table_ETPP_FORWARD_CODE_SELECTION table values");
    submodule_data->tables[dnx_data_aod_tables_table_ETPP_FORWARD_CODE_SELECTION].values[0].name = "valid";
    submodule_data->tables[dnx_data_aod_tables_table_ETPP_FORWARD_CODE_SELECTION].values[0].type = "uint32";
    submodule_data->tables[dnx_data_aod_tables_table_ETPP_FORWARD_CODE_SELECTION].values[0].doc = "indicates whether entry is valid";
    submodule_data->tables[dnx_data_aod_tables_table_ETPP_FORWARD_CODE_SELECTION].values[0].offset = UTILEX_OFFSETOF(dnx_data_aod_tables_ETPP_FORWARD_CODE_SELECTION_t, valid);
    submodule_data->tables[dnx_data_aod_tables_table_ETPP_FORWARD_CODE_SELECTION].values[1].name = "doc";
    submodule_data->tables[dnx_data_aod_tables_table_ETPP_FORWARD_CODE_SELECTION].values[1].type = "char *";
    submodule_data->tables[dnx_data_aod_tables_table_ETPP_FORWARD_CODE_SELECTION].values[1].doc = "entry documentation";
    submodule_data->tables[dnx_data_aod_tables_table_ETPP_FORWARD_CODE_SELECTION].values[1].offset = UTILEX_OFFSETOF(dnx_data_aod_tables_ETPP_FORWARD_CODE_SELECTION_t, doc);
    submodule_data->tables[dnx_data_aod_tables_table_ETPP_FORWARD_CODE_SELECTION].values[2].name = "ACE_Value";
    submodule_data->tables[dnx_data_aod_tables_table_ETPP_FORWARD_CODE_SELECTION].values[2].type = "char *";
    submodule_data->tables[dnx_data_aod_tables_table_ETPP_FORWARD_CODE_SELECTION].values[2].doc = "ACE_Value";
    submodule_data->tables[dnx_data_aod_tables_table_ETPP_FORWARD_CODE_SELECTION].values[2].offset = UTILEX_OFFSETOF(dnx_data_aod_tables_ETPP_FORWARD_CODE_SELECTION_t, ACE_Value);
    submodule_data->tables[dnx_data_aod_tables_table_ETPP_FORWARD_CODE_SELECTION].values[3].name = "ACE_Value_mask";
    submodule_data->tables[dnx_data_aod_tables_table_ETPP_FORWARD_CODE_SELECTION].values[3].type = "uint32";
    submodule_data->tables[dnx_data_aod_tables_table_ETPP_FORWARD_CODE_SELECTION].values[3].doc = "ACE_Value_mask";
    submodule_data->tables[dnx_data_aod_tables_table_ETPP_FORWARD_CODE_SELECTION].values[3].offset = UTILEX_OFFSETOF(dnx_data_aod_tables_ETPP_FORWARD_CODE_SELECTION_t, ACE_Value_mask);
    submodule_data->tables[dnx_data_aod_tables_table_ETPP_FORWARD_CODE_SELECTION].values[4].name = "ACE_Value_valid";
    submodule_data->tables[dnx_data_aod_tables_table_ETPP_FORWARD_CODE_SELECTION].values[4].type = "uint32";
    submodule_data->tables[dnx_data_aod_tables_table_ETPP_FORWARD_CODE_SELECTION].values[4].doc = "ACE_Value_valid";
    submodule_data->tables[dnx_data_aod_tables_table_ETPP_FORWARD_CODE_SELECTION].values[4].offset = UTILEX_OFFSETOF(dnx_data_aod_tables_ETPP_FORWARD_CODE_SELECTION_t, ACE_Value_valid);
    submodule_data->tables[dnx_data_aod_tables_table_ETPP_FORWARD_CODE_SELECTION].values[5].name = "FAI";
    submodule_data->tables[dnx_data_aod_tables_table_ETPP_FORWARD_CODE_SELECTION].values[5].type = "uint32";
    submodule_data->tables[dnx_data_aod_tables_table_ETPP_FORWARD_CODE_SELECTION].values[5].doc = "FAI";
    submodule_data->tables[dnx_data_aod_tables_table_ETPP_FORWARD_CODE_SELECTION].values[5].offset = UTILEX_OFFSETOF(dnx_data_aod_tables_ETPP_FORWARD_CODE_SELECTION_t, FAI);
    submodule_data->tables[dnx_data_aod_tables_table_ETPP_FORWARD_CODE_SELECTION].values[6].name = "FAI_mask";
    submodule_data->tables[dnx_data_aod_tables_table_ETPP_FORWARD_CODE_SELECTION].values[6].type = "uint32";
    submodule_data->tables[dnx_data_aod_tables_table_ETPP_FORWARD_CODE_SELECTION].values[6].doc = "FAI_mask";
    submodule_data->tables[dnx_data_aod_tables_table_ETPP_FORWARD_CODE_SELECTION].values[6].offset = UTILEX_OFFSETOF(dnx_data_aod_tables_ETPP_FORWARD_CODE_SELECTION_t, FAI_mask);
    submodule_data->tables[dnx_data_aod_tables_table_ETPP_FORWARD_CODE_SELECTION].values[7].name = "FAI_valid";
    submodule_data->tables[dnx_data_aod_tables_table_ETPP_FORWARD_CODE_SELECTION].values[7].type = "uint32";
    submodule_data->tables[dnx_data_aod_tables_table_ETPP_FORWARD_CODE_SELECTION].values[7].doc = "FAI_valid";
    submodule_data->tables[dnx_data_aod_tables_table_ETPP_FORWARD_CODE_SELECTION].values[7].offset = UTILEX_OFFSETOF(dnx_data_aod_tables_ETPP_FORWARD_CODE_SELECTION_t, FAI_valid);
    submodule_data->tables[dnx_data_aod_tables_table_ETPP_FORWARD_CODE_SELECTION].values[8].name = "FWD_Layer_Qualifier";
    submodule_data->tables[dnx_data_aod_tables_table_ETPP_FORWARD_CODE_SELECTION].values[8].type = "uint32";
    submodule_data->tables[dnx_data_aod_tables_table_ETPP_FORWARD_CODE_SELECTION].values[8].doc = "FWD_Layer_Qualifier";
    submodule_data->tables[dnx_data_aod_tables_table_ETPP_FORWARD_CODE_SELECTION].values[8].offset = UTILEX_OFFSETOF(dnx_data_aod_tables_ETPP_FORWARD_CODE_SELECTION_t, FWD_Layer_Qualifier);
    submodule_data->tables[dnx_data_aod_tables_table_ETPP_FORWARD_CODE_SELECTION].values[9].name = "FWD_Layer_Qualifier_mask";
    submodule_data->tables[dnx_data_aod_tables_table_ETPP_FORWARD_CODE_SELECTION].values[9].type = "uint32";
    submodule_data->tables[dnx_data_aod_tables_table_ETPP_FORWARD_CODE_SELECTION].values[9].doc = "FWD_Layer_Qualifier_mask";
    submodule_data->tables[dnx_data_aod_tables_table_ETPP_FORWARD_CODE_SELECTION].values[9].offset = UTILEX_OFFSETOF(dnx_data_aod_tables_ETPP_FORWARD_CODE_SELECTION_t, FWD_Layer_Qualifier_mask);
    submodule_data->tables[dnx_data_aod_tables_table_ETPP_FORWARD_CODE_SELECTION].values[10].name = "FWD_Layer_Qualifier_valid";
    submodule_data->tables[dnx_data_aod_tables_table_ETPP_FORWARD_CODE_SELECTION].values[10].type = "uint32";
    submodule_data->tables[dnx_data_aod_tables_table_ETPP_FORWARD_CODE_SELECTION].values[10].doc = "FWD_Layer_Qualifier_valid";
    submodule_data->tables[dnx_data_aod_tables_table_ETPP_FORWARD_CODE_SELECTION].values[10].offset = UTILEX_OFFSETOF(dnx_data_aod_tables_ETPP_FORWARD_CODE_SELECTION_t, FWD_Layer_Qualifier_valid);
    submodule_data->tables[dnx_data_aod_tables_table_ETPP_FORWARD_CODE_SELECTION].values[11].name = "FWD_Layer_Type";
    submodule_data->tables[dnx_data_aod_tables_table_ETPP_FORWARD_CODE_SELECTION].values[11].type = "char *";
    submodule_data->tables[dnx_data_aod_tables_table_ETPP_FORWARD_CODE_SELECTION].values[11].doc = "FWD_Layer_Type";
    submodule_data->tables[dnx_data_aod_tables_table_ETPP_FORWARD_CODE_SELECTION].values[11].offset = UTILEX_OFFSETOF(dnx_data_aod_tables_ETPP_FORWARD_CODE_SELECTION_t, FWD_Layer_Type);
    submodule_data->tables[dnx_data_aod_tables_table_ETPP_FORWARD_CODE_SELECTION].values[12].name = "FWD_Layer_Type_mask";
    submodule_data->tables[dnx_data_aod_tables_table_ETPP_FORWARD_CODE_SELECTION].values[12].type = "uint32";
    submodule_data->tables[dnx_data_aod_tables_table_ETPP_FORWARD_CODE_SELECTION].values[12].doc = "FWD_Layer_Type_mask";
    submodule_data->tables[dnx_data_aod_tables_table_ETPP_FORWARD_CODE_SELECTION].values[12].offset = UTILEX_OFFSETOF(dnx_data_aod_tables_ETPP_FORWARD_CODE_SELECTION_t, FWD_Layer_Type_mask);
    submodule_data->tables[dnx_data_aod_tables_table_ETPP_FORWARD_CODE_SELECTION].values[13].name = "FWD_Layer_Type_valid";
    submodule_data->tables[dnx_data_aod_tables_table_ETPP_FORWARD_CODE_SELECTION].values[13].type = "uint32";
    submodule_data->tables[dnx_data_aod_tables_table_ETPP_FORWARD_CODE_SELECTION].values[13].doc = "FWD_Layer_Type_valid";
    submodule_data->tables[dnx_data_aod_tables_table_ETPP_FORWARD_CODE_SELECTION].values[13].offset = UTILEX_OFFSETOF(dnx_data_aod_tables_ETPP_FORWARD_CODE_SELECTION_t, FWD_Layer_Type_valid);
    submodule_data->tables[dnx_data_aod_tables_table_ETPP_FORWARD_CODE_SELECTION].values[14].name = "FWD_code_port_profile";
    submodule_data->tables[dnx_data_aod_tables_table_ETPP_FORWARD_CODE_SELECTION].values[14].type = "char *";
    submodule_data->tables[dnx_data_aod_tables_table_ETPP_FORWARD_CODE_SELECTION].values[14].doc = "FWD_code_port_profile";
    submodule_data->tables[dnx_data_aod_tables_table_ETPP_FORWARD_CODE_SELECTION].values[14].offset = UTILEX_OFFSETOF(dnx_data_aod_tables_ETPP_FORWARD_CODE_SELECTION_t, FWD_code_port_profile);
    submodule_data->tables[dnx_data_aod_tables_table_ETPP_FORWARD_CODE_SELECTION].values[15].name = "FWD_code_port_profile_mask";
    submodule_data->tables[dnx_data_aod_tables_table_ETPP_FORWARD_CODE_SELECTION].values[15].type = "uint32";
    submodule_data->tables[dnx_data_aod_tables_table_ETPP_FORWARD_CODE_SELECTION].values[15].doc = "FWD_code_port_profile_mask";
    submodule_data->tables[dnx_data_aod_tables_table_ETPP_FORWARD_CODE_SELECTION].values[15].offset = UTILEX_OFFSETOF(dnx_data_aod_tables_ETPP_FORWARD_CODE_SELECTION_t, FWD_code_port_profile_mask);
    submodule_data->tables[dnx_data_aod_tables_table_ETPP_FORWARD_CODE_SELECTION].values[16].name = "FWD_code_port_profile_valid";
    submodule_data->tables[dnx_data_aod_tables_table_ETPP_FORWARD_CODE_SELECTION].values[16].type = "uint32";
    submodule_data->tables[dnx_data_aod_tables_table_ETPP_FORWARD_CODE_SELECTION].values[16].doc = "FWD_code_port_profile_valid";
    submodule_data->tables[dnx_data_aod_tables_table_ETPP_FORWARD_CODE_SELECTION].values[16].offset = UTILEX_OFFSETOF(dnx_data_aod_tables_ETPP_FORWARD_CODE_SELECTION_t, FWD_code_port_profile_valid);
    submodule_data->tables[dnx_data_aod_tables_table_ETPP_FORWARD_CODE_SELECTION].values[17].name = "TM_Action_Type";
    submodule_data->tables[dnx_data_aod_tables_table_ETPP_FORWARD_CODE_SELECTION].values[17].type = "uint32";
    submodule_data->tables[dnx_data_aod_tables_table_ETPP_FORWARD_CODE_SELECTION].values[17].doc = "TM_Action_Type";
    submodule_data->tables[dnx_data_aod_tables_table_ETPP_FORWARD_CODE_SELECTION].values[17].offset = UTILEX_OFFSETOF(dnx_data_aod_tables_ETPP_FORWARD_CODE_SELECTION_t, TM_Action_Type);
    submodule_data->tables[dnx_data_aod_tables_table_ETPP_FORWARD_CODE_SELECTION].values[18].name = "TM_Action_Type_mask";
    submodule_data->tables[dnx_data_aod_tables_table_ETPP_FORWARD_CODE_SELECTION].values[18].type = "uint32";
    submodule_data->tables[dnx_data_aod_tables_table_ETPP_FORWARD_CODE_SELECTION].values[18].doc = "TM_Action_Type_mask";
    submodule_data->tables[dnx_data_aod_tables_table_ETPP_FORWARD_CODE_SELECTION].values[18].offset = UTILEX_OFFSETOF(dnx_data_aod_tables_ETPP_FORWARD_CODE_SELECTION_t, TM_Action_Type_mask);
    submodule_data->tables[dnx_data_aod_tables_table_ETPP_FORWARD_CODE_SELECTION].values[19].name = "TM_Action_Type_valid";
    submodule_data->tables[dnx_data_aod_tables_table_ETPP_FORWARD_CODE_SELECTION].values[19].type = "uint32";
    submodule_data->tables[dnx_data_aod_tables_table_ETPP_FORWARD_CODE_SELECTION].values[19].doc = "TM_Action_Type_valid";
    submodule_data->tables[dnx_data_aod_tables_table_ETPP_FORWARD_CODE_SELECTION].values[19].offset = UTILEX_OFFSETOF(dnx_data_aod_tables_ETPP_FORWARD_CODE_SELECTION_t, TM_Action_Type_valid);
    submodule_data->tables[dnx_data_aod_tables_table_ETPP_FORWARD_CODE_SELECTION].values[20].name = "ingress_trapped";
    submodule_data->tables[dnx_data_aod_tables_table_ETPP_FORWARD_CODE_SELECTION].values[20].type = "uint32";
    submodule_data->tables[dnx_data_aod_tables_table_ETPP_FORWARD_CODE_SELECTION].values[20].doc = "ingress_trapped";
    submodule_data->tables[dnx_data_aod_tables_table_ETPP_FORWARD_CODE_SELECTION].values[20].offset = UTILEX_OFFSETOF(dnx_data_aod_tables_ETPP_FORWARD_CODE_SELECTION_t, ingress_trapped);
    submodule_data->tables[dnx_data_aod_tables_table_ETPP_FORWARD_CODE_SELECTION].values[21].name = "ingress_trapped_mask";
    submodule_data->tables[dnx_data_aod_tables_table_ETPP_FORWARD_CODE_SELECTION].values[21].type = "uint32";
    submodule_data->tables[dnx_data_aod_tables_table_ETPP_FORWARD_CODE_SELECTION].values[21].doc = "ingress_trapped_mask";
    submodule_data->tables[dnx_data_aod_tables_table_ETPP_FORWARD_CODE_SELECTION].values[21].offset = UTILEX_OFFSETOF(dnx_data_aod_tables_ETPP_FORWARD_CODE_SELECTION_t, ingress_trapped_mask);
    submodule_data->tables[dnx_data_aod_tables_table_ETPP_FORWARD_CODE_SELECTION].values[22].name = "ingress_trapped_valid";
    submodule_data->tables[dnx_data_aod_tables_table_ETPP_FORWARD_CODE_SELECTION].values[22].type = "uint32";
    submodule_data->tables[dnx_data_aod_tables_table_ETPP_FORWARD_CODE_SELECTION].values[22].doc = "ingress_trapped_valid";
    submodule_data->tables[dnx_data_aod_tables_table_ETPP_FORWARD_CODE_SELECTION].values[22].offset = UTILEX_OFFSETOF(dnx_data_aod_tables_ETPP_FORWARD_CODE_SELECTION_t, ingress_trapped_valid);
    submodule_data->tables[dnx_data_aod_tables_table_ETPP_FORWARD_CODE_SELECTION].values[23].name = "is_Applet";
    submodule_data->tables[dnx_data_aod_tables_table_ETPP_FORWARD_CODE_SELECTION].values[23].type = "uint32";
    submodule_data->tables[dnx_data_aod_tables_table_ETPP_FORWARD_CODE_SELECTION].values[23].doc = "is_Applet";
    submodule_data->tables[dnx_data_aod_tables_table_ETPP_FORWARD_CODE_SELECTION].values[23].offset = UTILEX_OFFSETOF(dnx_data_aod_tables_ETPP_FORWARD_CODE_SELECTION_t, is_Applet);
    submodule_data->tables[dnx_data_aod_tables_table_ETPP_FORWARD_CODE_SELECTION].values[24].name = "is_Applet_mask";
    submodule_data->tables[dnx_data_aod_tables_table_ETPP_FORWARD_CODE_SELECTION].values[24].type = "uint32";
    submodule_data->tables[dnx_data_aod_tables_table_ETPP_FORWARD_CODE_SELECTION].values[24].doc = "is_Applet_mask";
    submodule_data->tables[dnx_data_aod_tables_table_ETPP_FORWARD_CODE_SELECTION].values[24].offset = UTILEX_OFFSETOF(dnx_data_aod_tables_ETPP_FORWARD_CODE_SELECTION_t, is_Applet_mask);
    submodule_data->tables[dnx_data_aod_tables_table_ETPP_FORWARD_CODE_SELECTION].values[25].name = "is_Applet_valid";
    submodule_data->tables[dnx_data_aod_tables_table_ETPP_FORWARD_CODE_SELECTION].values[25].type = "uint32";
    submodule_data->tables[dnx_data_aod_tables_table_ETPP_FORWARD_CODE_SELECTION].values[25].doc = "is_Applet_valid";
    submodule_data->tables[dnx_data_aod_tables_table_ETPP_FORWARD_CODE_SELECTION].values[25].offset = UTILEX_OFFSETOF(dnx_data_aod_tables_ETPP_FORWARD_CODE_SELECTION_t, is_Applet_valid);
    submodule_data->tables[dnx_data_aod_tables_table_ETPP_FORWARD_CODE_SELECTION].values[26].name = "is_TDM";
    submodule_data->tables[dnx_data_aod_tables_table_ETPP_FORWARD_CODE_SELECTION].values[26].type = "uint32";
    submodule_data->tables[dnx_data_aod_tables_table_ETPP_FORWARD_CODE_SELECTION].values[26].doc = "is_TDM";
    submodule_data->tables[dnx_data_aod_tables_table_ETPP_FORWARD_CODE_SELECTION].values[26].offset = UTILEX_OFFSETOF(dnx_data_aod_tables_ETPP_FORWARD_CODE_SELECTION_t, is_TDM);
    submodule_data->tables[dnx_data_aod_tables_table_ETPP_FORWARD_CODE_SELECTION].values[27].name = "is_TDM_mask";
    submodule_data->tables[dnx_data_aod_tables_table_ETPP_FORWARD_CODE_SELECTION].values[27].type = "uint32";
    submodule_data->tables[dnx_data_aod_tables_table_ETPP_FORWARD_CODE_SELECTION].values[27].doc = "is_TDM_mask";
    submodule_data->tables[dnx_data_aod_tables_table_ETPP_FORWARD_CODE_SELECTION].values[27].offset = UTILEX_OFFSETOF(dnx_data_aod_tables_ETPP_FORWARD_CODE_SELECTION_t, is_TDM_mask);
    submodule_data->tables[dnx_data_aod_tables_table_ETPP_FORWARD_CODE_SELECTION].values[28].name = "is_TDM_valid";
    submodule_data->tables[dnx_data_aod_tables_table_ETPP_FORWARD_CODE_SELECTION].values[28].type = "uint32";
    submodule_data->tables[dnx_data_aod_tables_table_ETPP_FORWARD_CODE_SELECTION].values[28].doc = "is_TDM_valid";
    submodule_data->tables[dnx_data_aod_tables_table_ETPP_FORWARD_CODE_SELECTION].values[28].offset = UTILEX_OFFSETOF(dnx_data_aod_tables_ETPP_FORWARD_CODE_SELECTION_t, is_TDM_valid);
    submodule_data->tables[dnx_data_aod_tables_table_ETPP_FORWARD_CODE_SELECTION].values[29].name = "Egress_FWD_Code";
    submodule_data->tables[dnx_data_aod_tables_table_ETPP_FORWARD_CODE_SELECTION].values[29].type = "char *";
    submodule_data->tables[dnx_data_aod_tables_table_ETPP_FORWARD_CODE_SELECTION].values[29].doc = "Egress_FWD_Code";
    submodule_data->tables[dnx_data_aod_tables_table_ETPP_FORWARD_CODE_SELECTION].values[29].offset = UTILEX_OFFSETOF(dnx_data_aod_tables_ETPP_FORWARD_CODE_SELECTION_t, Egress_FWD_Code);
    submodule_data->tables[dnx_data_aod_tables_table_ETPP_FORWARD_CODE_SELECTION].values[30].name = "Egress_FWD_Code_valid";
    submodule_data->tables[dnx_data_aod_tables_table_ETPP_FORWARD_CODE_SELECTION].values[30].type = "uint32";
    submodule_data->tables[dnx_data_aod_tables_table_ETPP_FORWARD_CODE_SELECTION].values[30].doc = "Egress_FWD_Code_valid";
    submodule_data->tables[dnx_data_aod_tables_table_ETPP_FORWARD_CODE_SELECTION].values[30].offset = UTILEX_OFFSETOF(dnx_data_aod_tables_ETPP_FORWARD_CODE_SELECTION_t, Egress_FWD_Code_valid);

    
    submodule_data->tables[dnx_data_aod_tables_table_EGRESS_PARSING_START_TYPE_CONTEXT___image].name = "EGRESS_PARSING_START_TYPE_CONTEXT___image";
    submodule_data->tables[dnx_data_aod_tables_table_EGRESS_PARSING_START_TYPE_CONTEXT___image].doc = "table related image";
    submodule_data->tables[dnx_data_aod_tables_table_EGRESS_PARSING_START_TYPE_CONTEXT___image].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_aod_tables_table_EGRESS_PARSING_START_TYPE_CONTEXT___image].size_of_values = sizeof(dnx_data_aod_tables_EGRESS_PARSING_START_TYPE_CONTEXT___image_t);
    submodule_data->tables[dnx_data_aod_tables_table_EGRESS_PARSING_START_TYPE_CONTEXT___image].entry_get = dnx_data_aod_tables_EGRESS_PARSING_START_TYPE_CONTEXT___image_entry_str_get;

    
    submodule_data->tables[dnx_data_aod_tables_table_EGRESS_PARSING_START_TYPE_CONTEXT___image].nof_keys = 0;

    
    submodule_data->tables[dnx_data_aod_tables_table_EGRESS_PARSING_START_TYPE_CONTEXT___image].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_aod_tables_table_EGRESS_PARSING_START_TYPE_CONTEXT___image].values, dnxc_data_value_t, submodule_data->tables[dnx_data_aod_tables_table_EGRESS_PARSING_START_TYPE_CONTEXT___image].nof_values, "_dnx_data_aod_tables_table_EGRESS_PARSING_START_TYPE_CONTEXT___image table values");
    submodule_data->tables[dnx_data_aod_tables_table_EGRESS_PARSING_START_TYPE_CONTEXT___image].values[0].name = "image";
    submodule_data->tables[dnx_data_aod_tables_table_EGRESS_PARSING_START_TYPE_CONTEXT___image].values[0].type = "char *";
    submodule_data->tables[dnx_data_aod_tables_table_EGRESS_PARSING_START_TYPE_CONTEXT___image].values[0].doc = "related image name";
    submodule_data->tables[dnx_data_aod_tables_table_EGRESS_PARSING_START_TYPE_CONTEXT___image].values[0].offset = UTILEX_OFFSETOF(dnx_data_aod_tables_EGRESS_PARSING_START_TYPE_CONTEXT___image_t, image);

    
    submodule_data->tables[dnx_data_aod_tables_table_EGRESS_PARSING_START_TYPE_CONTEXT].name = "EGRESS_PARSING_START_TYPE_CONTEXT";
    submodule_data->tables[dnx_data_aod_tables_table_EGRESS_PARSING_START_TYPE_CONTEXT].doc = "EGRESS_PARSING_START_TYPE_CONTEXT";
    submodule_data->tables[dnx_data_aod_tables_table_EGRESS_PARSING_START_TYPE_CONTEXT].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_aod_tables_table_EGRESS_PARSING_START_TYPE_CONTEXT].size_of_values = sizeof(dnx_data_aod_tables_EGRESS_PARSING_START_TYPE_CONTEXT_t);
    submodule_data->tables[dnx_data_aod_tables_table_EGRESS_PARSING_START_TYPE_CONTEXT].entry_get = dnx_data_aod_tables_EGRESS_PARSING_START_TYPE_CONTEXT_entry_str_get;

    
    submodule_data->tables[dnx_data_aod_tables_table_EGRESS_PARSING_START_TYPE_CONTEXT].nof_keys = 2;
    submodule_data->tables[dnx_data_aod_tables_table_EGRESS_PARSING_START_TYPE_CONTEXT].keys[0].name = "index";
    submodule_data->tables[dnx_data_aod_tables_table_EGRESS_PARSING_START_TYPE_CONTEXT].keys[0].doc = "entry index";
    submodule_data->tables[dnx_data_aod_tables_table_EGRESS_PARSING_START_TYPE_CONTEXT].keys[1].name = "soc_value";
    submodule_data->tables[dnx_data_aod_tables_table_EGRESS_PARSING_START_TYPE_CONTEXT].keys[1].doc = "soc property value";

    
    submodule_data->tables[dnx_data_aod_tables_table_EGRESS_PARSING_START_TYPE_CONTEXT].nof_values = 7;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_aod_tables_table_EGRESS_PARSING_START_TYPE_CONTEXT].values, dnxc_data_value_t, submodule_data->tables[dnx_data_aod_tables_table_EGRESS_PARSING_START_TYPE_CONTEXT].nof_values, "_dnx_data_aod_tables_table_EGRESS_PARSING_START_TYPE_CONTEXT table values");
    submodule_data->tables[dnx_data_aod_tables_table_EGRESS_PARSING_START_TYPE_CONTEXT].values[0].name = "valid";
    submodule_data->tables[dnx_data_aod_tables_table_EGRESS_PARSING_START_TYPE_CONTEXT].values[0].type = "uint32";
    submodule_data->tables[dnx_data_aod_tables_table_EGRESS_PARSING_START_TYPE_CONTEXT].values[0].doc = "indicates whether entry is valid";
    submodule_data->tables[dnx_data_aod_tables_table_EGRESS_PARSING_START_TYPE_CONTEXT].values[0].offset = UTILEX_OFFSETOF(dnx_data_aod_tables_EGRESS_PARSING_START_TYPE_CONTEXT_t, valid);
    submodule_data->tables[dnx_data_aod_tables_table_EGRESS_PARSING_START_TYPE_CONTEXT].values[1].name = "doc";
    submodule_data->tables[dnx_data_aod_tables_table_EGRESS_PARSING_START_TYPE_CONTEXT].values[1].type = "char *";
    submodule_data->tables[dnx_data_aod_tables_table_EGRESS_PARSING_START_TYPE_CONTEXT].values[1].doc = "entry documentation";
    submodule_data->tables[dnx_data_aod_tables_table_EGRESS_PARSING_START_TYPE_CONTEXT].values[1].offset = UTILEX_OFFSETOF(dnx_data_aod_tables_EGRESS_PARSING_START_TYPE_CONTEXT_t, doc);
    submodule_data->tables[dnx_data_aod_tables_table_EGRESS_PARSING_START_TYPE_CONTEXT].values[2].name = "Parsing_Start_Type";
    submodule_data->tables[dnx_data_aod_tables_table_EGRESS_PARSING_START_TYPE_CONTEXT].values[2].type = "char *";
    submodule_data->tables[dnx_data_aod_tables_table_EGRESS_PARSING_START_TYPE_CONTEXT].values[2].doc = "Parsing_Start_Type";
    submodule_data->tables[dnx_data_aod_tables_table_EGRESS_PARSING_START_TYPE_CONTEXT].values[2].offset = UTILEX_OFFSETOF(dnx_data_aod_tables_EGRESS_PARSING_START_TYPE_CONTEXT_t, Parsing_Start_Type);
    submodule_data->tables[dnx_data_aod_tables_table_EGRESS_PARSING_START_TYPE_CONTEXT].values[3].name = "Parsing_Start_Type_mask";
    submodule_data->tables[dnx_data_aod_tables_table_EGRESS_PARSING_START_TYPE_CONTEXT].values[3].type = "uint32";
    submodule_data->tables[dnx_data_aod_tables_table_EGRESS_PARSING_START_TYPE_CONTEXT].values[3].doc = "Parsing_Start_Type_mask";
    submodule_data->tables[dnx_data_aod_tables_table_EGRESS_PARSING_START_TYPE_CONTEXT].values[3].offset = UTILEX_OFFSETOF(dnx_data_aod_tables_EGRESS_PARSING_START_TYPE_CONTEXT_t, Parsing_Start_Type_mask);
    submodule_data->tables[dnx_data_aod_tables_table_EGRESS_PARSING_START_TYPE_CONTEXT].values[4].name = "Parsing_Start_Type_valid";
    submodule_data->tables[dnx_data_aod_tables_table_EGRESS_PARSING_START_TYPE_CONTEXT].values[4].type = "uint32";
    submodule_data->tables[dnx_data_aod_tables_table_EGRESS_PARSING_START_TYPE_CONTEXT].values[4].doc = "Parsing_Start_Type_valid";
    submodule_data->tables[dnx_data_aod_tables_table_EGRESS_PARSING_START_TYPE_CONTEXT].values[4].offset = UTILEX_OFFSETOF(dnx_data_aod_tables_EGRESS_PARSING_START_TYPE_CONTEXT_t, Parsing_Start_Type_valid);
    submodule_data->tables[dnx_data_aod_tables_table_EGRESS_PARSING_START_TYPE_CONTEXT].values[5].name = "ERPP_1st_Parser_Parser_Context";
    submodule_data->tables[dnx_data_aod_tables_table_EGRESS_PARSING_START_TYPE_CONTEXT].values[5].type = "char *";
    submodule_data->tables[dnx_data_aod_tables_table_EGRESS_PARSING_START_TYPE_CONTEXT].values[5].doc = "ERPP_1st_Parser_Parser_Context";
    submodule_data->tables[dnx_data_aod_tables_table_EGRESS_PARSING_START_TYPE_CONTEXT].values[5].offset = UTILEX_OFFSETOF(dnx_data_aod_tables_EGRESS_PARSING_START_TYPE_CONTEXT_t, ERPP_1st_Parser_Parser_Context);
    submodule_data->tables[dnx_data_aod_tables_table_EGRESS_PARSING_START_TYPE_CONTEXT].values[6].name = "ERPP_1st_Parser_Parser_Context_valid";
    submodule_data->tables[dnx_data_aod_tables_table_EGRESS_PARSING_START_TYPE_CONTEXT].values[6].type = "uint32";
    submodule_data->tables[dnx_data_aod_tables_table_EGRESS_PARSING_START_TYPE_CONTEXT].values[6].doc = "ERPP_1st_Parser_Parser_Context_valid";
    submodule_data->tables[dnx_data_aod_tables_table_EGRESS_PARSING_START_TYPE_CONTEXT].values[6].offset = UTILEX_OFFSETOF(dnx_data_aod_tables_EGRESS_PARSING_START_TYPE_CONTEXT_t, ERPP_1st_Parser_Parser_Context_valid);

    
    submodule_data->tables[dnx_data_aod_tables_table_EGRESS_PARSING_CONTEXT_MAP_JR1_MODE___image].name = "EGRESS_PARSING_CONTEXT_MAP_JR1_MODE___image";
    submodule_data->tables[dnx_data_aod_tables_table_EGRESS_PARSING_CONTEXT_MAP_JR1_MODE___image].doc = "table related image";
    submodule_data->tables[dnx_data_aod_tables_table_EGRESS_PARSING_CONTEXT_MAP_JR1_MODE___image].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_aod_tables_table_EGRESS_PARSING_CONTEXT_MAP_JR1_MODE___image].size_of_values = sizeof(dnx_data_aod_tables_EGRESS_PARSING_CONTEXT_MAP_JR1_MODE___image_t);
    submodule_data->tables[dnx_data_aod_tables_table_EGRESS_PARSING_CONTEXT_MAP_JR1_MODE___image].entry_get = dnx_data_aod_tables_EGRESS_PARSING_CONTEXT_MAP_JR1_MODE___image_entry_str_get;

    
    submodule_data->tables[dnx_data_aod_tables_table_EGRESS_PARSING_CONTEXT_MAP_JR1_MODE___image].nof_keys = 0;

    
    submodule_data->tables[dnx_data_aod_tables_table_EGRESS_PARSING_CONTEXT_MAP_JR1_MODE___image].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_aod_tables_table_EGRESS_PARSING_CONTEXT_MAP_JR1_MODE___image].values, dnxc_data_value_t, submodule_data->tables[dnx_data_aod_tables_table_EGRESS_PARSING_CONTEXT_MAP_JR1_MODE___image].nof_values, "_dnx_data_aod_tables_table_EGRESS_PARSING_CONTEXT_MAP_JR1_MODE___image table values");
    submodule_data->tables[dnx_data_aod_tables_table_EGRESS_PARSING_CONTEXT_MAP_JR1_MODE___image].values[0].name = "image";
    submodule_data->tables[dnx_data_aod_tables_table_EGRESS_PARSING_CONTEXT_MAP_JR1_MODE___image].values[0].type = "char *";
    submodule_data->tables[dnx_data_aod_tables_table_EGRESS_PARSING_CONTEXT_MAP_JR1_MODE___image].values[0].doc = "related image name";
    submodule_data->tables[dnx_data_aod_tables_table_EGRESS_PARSING_CONTEXT_MAP_JR1_MODE___image].values[0].offset = UTILEX_OFFSETOF(dnx_data_aod_tables_EGRESS_PARSING_CONTEXT_MAP_JR1_MODE___image_t, image);

    
    submodule_data->tables[dnx_data_aod_tables_table_EGRESS_PARSING_CONTEXT_MAP_JR1_MODE].name = "EGRESS_PARSING_CONTEXT_MAP_JR1_MODE";
    submodule_data->tables[dnx_data_aod_tables_table_EGRESS_PARSING_CONTEXT_MAP_JR1_MODE].doc = "EGRESS_PARSING_CONTEXT_MAP_JR1_MODE";
    submodule_data->tables[dnx_data_aod_tables_table_EGRESS_PARSING_CONTEXT_MAP_JR1_MODE].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_aod_tables_table_EGRESS_PARSING_CONTEXT_MAP_JR1_MODE].size_of_values = sizeof(dnx_data_aod_tables_EGRESS_PARSING_CONTEXT_MAP_JR1_MODE_t);
    submodule_data->tables[dnx_data_aod_tables_table_EGRESS_PARSING_CONTEXT_MAP_JR1_MODE].entry_get = dnx_data_aod_tables_EGRESS_PARSING_CONTEXT_MAP_JR1_MODE_entry_str_get;

    
    submodule_data->tables[dnx_data_aod_tables_table_EGRESS_PARSING_CONTEXT_MAP_JR1_MODE].nof_keys = 2;
    submodule_data->tables[dnx_data_aod_tables_table_EGRESS_PARSING_CONTEXT_MAP_JR1_MODE].keys[0].name = "index";
    submodule_data->tables[dnx_data_aod_tables_table_EGRESS_PARSING_CONTEXT_MAP_JR1_MODE].keys[0].doc = "entry index";
    submodule_data->tables[dnx_data_aod_tables_table_EGRESS_PARSING_CONTEXT_MAP_JR1_MODE].keys[1].name = "soc_value";
    submodule_data->tables[dnx_data_aod_tables_table_EGRESS_PARSING_CONTEXT_MAP_JR1_MODE].keys[1].doc = "soc property value";

    
    submodule_data->tables[dnx_data_aod_tables_table_EGRESS_PARSING_CONTEXT_MAP_JR1_MODE].nof_values = 7;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_aod_tables_table_EGRESS_PARSING_CONTEXT_MAP_JR1_MODE].values, dnxc_data_value_t, submodule_data->tables[dnx_data_aod_tables_table_EGRESS_PARSING_CONTEXT_MAP_JR1_MODE].nof_values, "_dnx_data_aod_tables_table_EGRESS_PARSING_CONTEXT_MAP_JR1_MODE table values");
    submodule_data->tables[dnx_data_aod_tables_table_EGRESS_PARSING_CONTEXT_MAP_JR1_MODE].values[0].name = "valid";
    submodule_data->tables[dnx_data_aod_tables_table_EGRESS_PARSING_CONTEXT_MAP_JR1_MODE].values[0].type = "uint32";
    submodule_data->tables[dnx_data_aod_tables_table_EGRESS_PARSING_CONTEXT_MAP_JR1_MODE].values[0].doc = "indicates whether entry is valid";
    submodule_data->tables[dnx_data_aod_tables_table_EGRESS_PARSING_CONTEXT_MAP_JR1_MODE].values[0].offset = UTILEX_OFFSETOF(dnx_data_aod_tables_EGRESS_PARSING_CONTEXT_MAP_JR1_MODE_t, valid);
    submodule_data->tables[dnx_data_aod_tables_table_EGRESS_PARSING_CONTEXT_MAP_JR1_MODE].values[1].name = "doc";
    submodule_data->tables[dnx_data_aod_tables_table_EGRESS_PARSING_CONTEXT_MAP_JR1_MODE].values[1].type = "char *";
    submodule_data->tables[dnx_data_aod_tables_table_EGRESS_PARSING_CONTEXT_MAP_JR1_MODE].values[1].doc = "entry documentation";
    submodule_data->tables[dnx_data_aod_tables_table_EGRESS_PARSING_CONTEXT_MAP_JR1_MODE].values[1].offset = UTILEX_OFFSETOF(dnx_data_aod_tables_EGRESS_PARSING_CONTEXT_MAP_JR1_MODE_t, doc);
    submodule_data->tables[dnx_data_aod_tables_table_EGRESS_PARSING_CONTEXT_MAP_JR1_MODE].values[2].name = "JR_FWD_CODE";
    submodule_data->tables[dnx_data_aod_tables_table_EGRESS_PARSING_CONTEXT_MAP_JR1_MODE].values[2].type = "char *";
    submodule_data->tables[dnx_data_aod_tables_table_EGRESS_PARSING_CONTEXT_MAP_JR1_MODE].values[2].doc = "JR_FWD_CODE";
    submodule_data->tables[dnx_data_aod_tables_table_EGRESS_PARSING_CONTEXT_MAP_JR1_MODE].values[2].offset = UTILEX_OFFSETOF(dnx_data_aod_tables_EGRESS_PARSING_CONTEXT_MAP_JR1_MODE_t, JR_FWD_CODE);
    submodule_data->tables[dnx_data_aod_tables_table_EGRESS_PARSING_CONTEXT_MAP_JR1_MODE].values[3].name = "JR_FWD_CODE_mask";
    submodule_data->tables[dnx_data_aod_tables_table_EGRESS_PARSING_CONTEXT_MAP_JR1_MODE].values[3].type = "uint32";
    submodule_data->tables[dnx_data_aod_tables_table_EGRESS_PARSING_CONTEXT_MAP_JR1_MODE].values[3].doc = "JR_FWD_CODE_mask";
    submodule_data->tables[dnx_data_aod_tables_table_EGRESS_PARSING_CONTEXT_MAP_JR1_MODE].values[3].offset = UTILEX_OFFSETOF(dnx_data_aod_tables_EGRESS_PARSING_CONTEXT_MAP_JR1_MODE_t, JR_FWD_CODE_mask);
    submodule_data->tables[dnx_data_aod_tables_table_EGRESS_PARSING_CONTEXT_MAP_JR1_MODE].values[4].name = "JR_FWD_CODE_valid";
    submodule_data->tables[dnx_data_aod_tables_table_EGRESS_PARSING_CONTEXT_MAP_JR1_MODE].values[4].type = "uint32";
    submodule_data->tables[dnx_data_aod_tables_table_EGRESS_PARSING_CONTEXT_MAP_JR1_MODE].values[4].doc = "JR_FWD_CODE_valid";
    submodule_data->tables[dnx_data_aod_tables_table_EGRESS_PARSING_CONTEXT_MAP_JR1_MODE].values[4].offset = UTILEX_OFFSETOF(dnx_data_aod_tables_EGRESS_PARSING_CONTEXT_MAP_JR1_MODE_t, JR_FWD_CODE_valid);
    submodule_data->tables[dnx_data_aod_tables_table_EGRESS_PARSING_CONTEXT_MAP_JR1_MODE].values[5].name = "ERPP_1st_Parser_Parser_Context";
    submodule_data->tables[dnx_data_aod_tables_table_EGRESS_PARSING_CONTEXT_MAP_JR1_MODE].values[5].type = "char *";
    submodule_data->tables[dnx_data_aod_tables_table_EGRESS_PARSING_CONTEXT_MAP_JR1_MODE].values[5].doc = "ERPP_1st_Parser_Parser_Context";
    submodule_data->tables[dnx_data_aod_tables_table_EGRESS_PARSING_CONTEXT_MAP_JR1_MODE].values[5].offset = UTILEX_OFFSETOF(dnx_data_aod_tables_EGRESS_PARSING_CONTEXT_MAP_JR1_MODE_t, ERPP_1st_Parser_Parser_Context);
    submodule_data->tables[dnx_data_aod_tables_table_EGRESS_PARSING_CONTEXT_MAP_JR1_MODE].values[6].name = "ERPP_1st_Parser_Parser_Context_valid";
    submodule_data->tables[dnx_data_aod_tables_table_EGRESS_PARSING_CONTEXT_MAP_JR1_MODE].values[6].type = "uint32";
    submodule_data->tables[dnx_data_aod_tables_table_EGRESS_PARSING_CONTEXT_MAP_JR1_MODE].values[6].doc = "ERPP_1st_Parser_Parser_Context_valid";
    submodule_data->tables[dnx_data_aod_tables_table_EGRESS_PARSING_CONTEXT_MAP_JR1_MODE].values[6].offset = UTILEX_OFFSETOF(dnx_data_aod_tables_EGRESS_PARSING_CONTEXT_MAP_JR1_MODE_t, ERPP_1st_Parser_Parser_Context_valid);

    
    submodule_data->tables[dnx_data_aod_tables_table_MPLS_SPECIAL_LABEL_PROFILE_ATTRIBUTES___image].name = "MPLS_SPECIAL_LABEL_PROFILE_ATTRIBUTES___image";
    submodule_data->tables[dnx_data_aod_tables_table_MPLS_SPECIAL_LABEL_PROFILE_ATTRIBUTES___image].doc = "table related image";
    submodule_data->tables[dnx_data_aod_tables_table_MPLS_SPECIAL_LABEL_PROFILE_ATTRIBUTES___image].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_aod_tables_table_MPLS_SPECIAL_LABEL_PROFILE_ATTRIBUTES___image].size_of_values = sizeof(dnx_data_aod_tables_MPLS_SPECIAL_LABEL_PROFILE_ATTRIBUTES___image_t);
    submodule_data->tables[dnx_data_aod_tables_table_MPLS_SPECIAL_LABEL_PROFILE_ATTRIBUTES___image].entry_get = dnx_data_aod_tables_MPLS_SPECIAL_LABEL_PROFILE_ATTRIBUTES___image_entry_str_get;

    
    submodule_data->tables[dnx_data_aod_tables_table_MPLS_SPECIAL_LABEL_PROFILE_ATTRIBUTES___image].nof_keys = 0;

    
    submodule_data->tables[dnx_data_aod_tables_table_MPLS_SPECIAL_LABEL_PROFILE_ATTRIBUTES___image].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_aod_tables_table_MPLS_SPECIAL_LABEL_PROFILE_ATTRIBUTES___image].values, dnxc_data_value_t, submodule_data->tables[dnx_data_aod_tables_table_MPLS_SPECIAL_LABEL_PROFILE_ATTRIBUTES___image].nof_values, "_dnx_data_aod_tables_table_MPLS_SPECIAL_LABEL_PROFILE_ATTRIBUTES___image table values");
    submodule_data->tables[dnx_data_aod_tables_table_MPLS_SPECIAL_LABEL_PROFILE_ATTRIBUTES___image].values[0].name = "image";
    submodule_data->tables[dnx_data_aod_tables_table_MPLS_SPECIAL_LABEL_PROFILE_ATTRIBUTES___image].values[0].type = "char *";
    submodule_data->tables[dnx_data_aod_tables_table_MPLS_SPECIAL_LABEL_PROFILE_ATTRIBUTES___image].values[0].doc = "related image name";
    submodule_data->tables[dnx_data_aod_tables_table_MPLS_SPECIAL_LABEL_PROFILE_ATTRIBUTES___image].values[0].offset = UTILEX_OFFSETOF(dnx_data_aod_tables_MPLS_SPECIAL_LABEL_PROFILE_ATTRIBUTES___image_t, image);

    
    submodule_data->tables[dnx_data_aod_tables_table_MPLS_SPECIAL_LABEL_PROFILE_ATTRIBUTES].name = "MPLS_SPECIAL_LABEL_PROFILE_ATTRIBUTES";
    submodule_data->tables[dnx_data_aod_tables_table_MPLS_SPECIAL_LABEL_PROFILE_ATTRIBUTES].doc = "MPLS_SPECIAL_LABEL_PROFILE_ATTRIBUTES";
    submodule_data->tables[dnx_data_aod_tables_table_MPLS_SPECIAL_LABEL_PROFILE_ATTRIBUTES].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_aod_tables_table_MPLS_SPECIAL_LABEL_PROFILE_ATTRIBUTES].size_of_values = sizeof(dnx_data_aod_tables_MPLS_SPECIAL_LABEL_PROFILE_ATTRIBUTES_t);
    submodule_data->tables[dnx_data_aod_tables_table_MPLS_SPECIAL_LABEL_PROFILE_ATTRIBUTES].entry_get = dnx_data_aod_tables_MPLS_SPECIAL_LABEL_PROFILE_ATTRIBUTES_entry_str_get;

    
    submodule_data->tables[dnx_data_aod_tables_table_MPLS_SPECIAL_LABEL_PROFILE_ATTRIBUTES].nof_keys = 1;
    submodule_data->tables[dnx_data_aod_tables_table_MPLS_SPECIAL_LABEL_PROFILE_ATTRIBUTES].keys[0].name = "index";
    submodule_data->tables[dnx_data_aod_tables_table_MPLS_SPECIAL_LABEL_PROFILE_ATTRIBUTES].keys[0].doc = "entry index";

    
    submodule_data->tables[dnx_data_aod_tables_table_MPLS_SPECIAL_LABEL_PROFILE_ATTRIBUTES].nof_values = 43;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_aod_tables_table_MPLS_SPECIAL_LABEL_PROFILE_ATTRIBUTES].values, dnxc_data_value_t, submodule_data->tables[dnx_data_aod_tables_table_MPLS_SPECIAL_LABEL_PROFILE_ATTRIBUTES].nof_values, "_dnx_data_aod_tables_table_MPLS_SPECIAL_LABEL_PROFILE_ATTRIBUTES table values");
    submodule_data->tables[dnx_data_aod_tables_table_MPLS_SPECIAL_LABEL_PROFILE_ATTRIBUTES].values[0].name = "valid";
    submodule_data->tables[dnx_data_aod_tables_table_MPLS_SPECIAL_LABEL_PROFILE_ATTRIBUTES].values[0].type = "uint32";
    submodule_data->tables[dnx_data_aod_tables_table_MPLS_SPECIAL_LABEL_PROFILE_ATTRIBUTES].values[0].doc = "indicates whether entry is valid";
    submodule_data->tables[dnx_data_aod_tables_table_MPLS_SPECIAL_LABEL_PROFILE_ATTRIBUTES].values[0].offset = UTILEX_OFFSETOF(dnx_data_aod_tables_MPLS_SPECIAL_LABEL_PROFILE_ATTRIBUTES_t, valid);
    submodule_data->tables[dnx_data_aod_tables_table_MPLS_SPECIAL_LABEL_PROFILE_ATTRIBUTES].values[1].name = "doc";
    submodule_data->tables[dnx_data_aod_tables_table_MPLS_SPECIAL_LABEL_PROFILE_ATTRIBUTES].values[1].type = "char *";
    submodule_data->tables[dnx_data_aod_tables_table_MPLS_SPECIAL_LABEL_PROFILE_ATTRIBUTES].values[1].doc = "entry documentation";
    submodule_data->tables[dnx_data_aod_tables_table_MPLS_SPECIAL_LABEL_PROFILE_ATTRIBUTES].values[1].offset = UTILEX_OFFSETOF(dnx_data_aod_tables_MPLS_SPECIAL_LABEL_PROFILE_ATTRIBUTES_t, doc);
    submodule_data->tables[dnx_data_aod_tables_table_MPLS_SPECIAL_LABEL_PROFILE_ATTRIBUTES].values[2].name = "VTT_MPLS_Special_Label_Profile";
    submodule_data->tables[dnx_data_aod_tables_table_MPLS_SPECIAL_LABEL_PROFILE_ATTRIBUTES].values[2].type = "char *";
    submodule_data->tables[dnx_data_aod_tables_table_MPLS_SPECIAL_LABEL_PROFILE_ATTRIBUTES].values[2].doc = "VTT_MPLS_Special_Label_Profile";
    submodule_data->tables[dnx_data_aod_tables_table_MPLS_SPECIAL_LABEL_PROFILE_ATTRIBUTES].values[2].offset = UTILEX_OFFSETOF(dnx_data_aod_tables_MPLS_SPECIAL_LABEL_PROFILE_ATTRIBUTES_t, VTT_MPLS_Special_Label_Profile);
    submodule_data->tables[dnx_data_aod_tables_table_MPLS_SPECIAL_LABEL_PROFILE_ATTRIBUTES].values[3].name = "VTT_MPLS_Special_Label_Profile_mask";
    submodule_data->tables[dnx_data_aod_tables_table_MPLS_SPECIAL_LABEL_PROFILE_ATTRIBUTES].values[3].type = "uint32";
    submodule_data->tables[dnx_data_aod_tables_table_MPLS_SPECIAL_LABEL_PROFILE_ATTRIBUTES].values[3].doc = "VTT_MPLS_Special_Label_Profile_mask";
    submodule_data->tables[dnx_data_aod_tables_table_MPLS_SPECIAL_LABEL_PROFILE_ATTRIBUTES].values[3].offset = UTILEX_OFFSETOF(dnx_data_aod_tables_MPLS_SPECIAL_LABEL_PROFILE_ATTRIBUTES_t, VTT_MPLS_Special_Label_Profile_mask);
    submodule_data->tables[dnx_data_aod_tables_table_MPLS_SPECIAL_LABEL_PROFILE_ATTRIBUTES].values[4].name = "VTT_MPLS_Special_Label_Profile_valid";
    submodule_data->tables[dnx_data_aod_tables_table_MPLS_SPECIAL_LABEL_PROFILE_ATTRIBUTES].values[4].type = "uint32";
    submodule_data->tables[dnx_data_aod_tables_table_MPLS_SPECIAL_LABEL_PROFILE_ATTRIBUTES].values[4].doc = "VTT_MPLS_Special_Label_Profile_valid";
    submodule_data->tables[dnx_data_aod_tables_table_MPLS_SPECIAL_LABEL_PROFILE_ATTRIBUTES].values[4].offset = UTILEX_OFFSETOF(dnx_data_aod_tables_MPLS_SPECIAL_LABEL_PROFILE_ATTRIBUTES_t, VTT_MPLS_Special_Label_Profile_valid);
    submodule_data->tables[dnx_data_aod_tables_table_MPLS_SPECIAL_LABEL_PROFILE_ATTRIBUTES].values[5].name = "Check_Bos";
    submodule_data->tables[dnx_data_aod_tables_table_MPLS_SPECIAL_LABEL_PROFILE_ATTRIBUTES].values[5].type = "uint32";
    submodule_data->tables[dnx_data_aod_tables_table_MPLS_SPECIAL_LABEL_PROFILE_ATTRIBUTES].values[5].doc = "Check_Bos";
    submodule_data->tables[dnx_data_aod_tables_table_MPLS_SPECIAL_LABEL_PROFILE_ATTRIBUTES].values[5].offset = UTILEX_OFFSETOF(dnx_data_aod_tables_MPLS_SPECIAL_LABEL_PROFILE_ATTRIBUTES_t, Check_Bos);
    submodule_data->tables[dnx_data_aod_tables_table_MPLS_SPECIAL_LABEL_PROFILE_ATTRIBUTES].values[6].name = "Check_Bos_valid";
    submodule_data->tables[dnx_data_aod_tables_table_MPLS_SPECIAL_LABEL_PROFILE_ATTRIBUTES].values[6].type = "uint32";
    submodule_data->tables[dnx_data_aod_tables_table_MPLS_SPECIAL_LABEL_PROFILE_ATTRIBUTES].values[6].doc = "Check_Bos_valid";
    submodule_data->tables[dnx_data_aod_tables_table_MPLS_SPECIAL_LABEL_PROFILE_ATTRIBUTES].values[6].offset = UTILEX_OFFSETOF(dnx_data_aod_tables_MPLS_SPECIAL_LABEL_PROFILE_ATTRIBUTES_t, Check_Bos_valid);
    submodule_data->tables[dnx_data_aod_tables_table_MPLS_SPECIAL_LABEL_PROFILE_ATTRIBUTES].values[7].name = "ECN_Compare_Strength";
    submodule_data->tables[dnx_data_aod_tables_table_MPLS_SPECIAL_LABEL_PROFILE_ATTRIBUTES].values[7].type = "uint32";
    submodule_data->tables[dnx_data_aod_tables_table_MPLS_SPECIAL_LABEL_PROFILE_ATTRIBUTES].values[7].doc = "ECN_Compare_Strength";
    submodule_data->tables[dnx_data_aod_tables_table_MPLS_SPECIAL_LABEL_PROFILE_ATTRIBUTES].values[7].offset = UTILEX_OFFSETOF(dnx_data_aod_tables_MPLS_SPECIAL_LABEL_PROFILE_ATTRIBUTES_t, ECN_Compare_Strength);
    submodule_data->tables[dnx_data_aod_tables_table_MPLS_SPECIAL_LABEL_PROFILE_ATTRIBUTES].values[8].name = "ECN_Compare_Strength_valid";
    submodule_data->tables[dnx_data_aod_tables_table_MPLS_SPECIAL_LABEL_PROFILE_ATTRIBUTES].values[8].type = "uint32";
    submodule_data->tables[dnx_data_aod_tables_table_MPLS_SPECIAL_LABEL_PROFILE_ATTRIBUTES].values[8].doc = "ECN_Compare_Strength_valid";
    submodule_data->tables[dnx_data_aod_tables_table_MPLS_SPECIAL_LABEL_PROFILE_ATTRIBUTES].values[8].offset = UTILEX_OFFSETOF(dnx_data_aod_tables_MPLS_SPECIAL_LABEL_PROFILE_ATTRIBUTES_t, ECN_Compare_Strength_valid);
    submodule_data->tables[dnx_data_aod_tables_table_MPLS_SPECIAL_LABEL_PROFILE_ATTRIBUTES].values[9].name = "ECN_Mapping_Profile";
    submodule_data->tables[dnx_data_aod_tables_table_MPLS_SPECIAL_LABEL_PROFILE_ATTRIBUTES].values[9].type = "uint32";
    submodule_data->tables[dnx_data_aod_tables_table_MPLS_SPECIAL_LABEL_PROFILE_ATTRIBUTES].values[9].doc = "ECN_Mapping_Profile";
    submodule_data->tables[dnx_data_aod_tables_table_MPLS_SPECIAL_LABEL_PROFILE_ATTRIBUTES].values[9].offset = UTILEX_OFFSETOF(dnx_data_aod_tables_MPLS_SPECIAL_LABEL_PROFILE_ATTRIBUTES_t, ECN_Mapping_Profile);
    submodule_data->tables[dnx_data_aod_tables_table_MPLS_SPECIAL_LABEL_PROFILE_ATTRIBUTES].values[10].name = "ECN_Mapping_Profile_valid";
    submodule_data->tables[dnx_data_aod_tables_table_MPLS_SPECIAL_LABEL_PROFILE_ATTRIBUTES].values[10].type = "uint32";
    submodule_data->tables[dnx_data_aod_tables_table_MPLS_SPECIAL_LABEL_PROFILE_ATTRIBUTES].values[10].doc = "ECN_Mapping_Profile_valid";
    submodule_data->tables[dnx_data_aod_tables_table_MPLS_SPECIAL_LABEL_PROFILE_ATTRIBUTES].values[10].offset = UTILEX_OFFSETOF(dnx_data_aod_tables_MPLS_SPECIAL_LABEL_PROFILE_ATTRIBUTES_t, ECN_Mapping_Profile_valid);
    submodule_data->tables[dnx_data_aod_tables_table_MPLS_SPECIAL_LABEL_PROFILE_ATTRIBUTES].values[11].name = "ECN_Propagation_Strength";
    submodule_data->tables[dnx_data_aod_tables_table_MPLS_SPECIAL_LABEL_PROFILE_ATTRIBUTES].values[11].type = "uint32";
    submodule_data->tables[dnx_data_aod_tables_table_MPLS_SPECIAL_LABEL_PROFILE_ATTRIBUTES].values[11].doc = "ECN_Propagation_Strength";
    submodule_data->tables[dnx_data_aod_tables_table_MPLS_SPECIAL_LABEL_PROFILE_ATTRIBUTES].values[11].offset = UTILEX_OFFSETOF(dnx_data_aod_tables_MPLS_SPECIAL_LABEL_PROFILE_ATTRIBUTES_t, ECN_Propagation_Strength);
    submodule_data->tables[dnx_data_aod_tables_table_MPLS_SPECIAL_LABEL_PROFILE_ATTRIBUTES].values[12].name = "ECN_Propagation_Strength_valid";
    submodule_data->tables[dnx_data_aod_tables_table_MPLS_SPECIAL_LABEL_PROFILE_ATTRIBUTES].values[12].type = "uint32";
    submodule_data->tables[dnx_data_aod_tables_table_MPLS_SPECIAL_LABEL_PROFILE_ATTRIBUTES].values[12].doc = "ECN_Propagation_Strength_valid";
    submodule_data->tables[dnx_data_aod_tables_table_MPLS_SPECIAL_LABEL_PROFILE_ATTRIBUTES].values[12].offset = UTILEX_OFFSETOF(dnx_data_aod_tables_MPLS_SPECIAL_LABEL_PROFILE_ATTRIBUTES_t, ECN_Propagation_Strength_valid);
    submodule_data->tables[dnx_data_aod_tables_table_MPLS_SPECIAL_LABEL_PROFILE_ATTRIBUTES].values[13].name = "Expected_Bos";
    submodule_data->tables[dnx_data_aod_tables_table_MPLS_SPECIAL_LABEL_PROFILE_ATTRIBUTES].values[13].type = "uint32";
    submodule_data->tables[dnx_data_aod_tables_table_MPLS_SPECIAL_LABEL_PROFILE_ATTRIBUTES].values[13].doc = "Expected_Bos";
    submodule_data->tables[dnx_data_aod_tables_table_MPLS_SPECIAL_LABEL_PROFILE_ATTRIBUTES].values[13].offset = UTILEX_OFFSETOF(dnx_data_aod_tables_MPLS_SPECIAL_LABEL_PROFILE_ATTRIBUTES_t, Expected_Bos);
    submodule_data->tables[dnx_data_aod_tables_table_MPLS_SPECIAL_LABEL_PROFILE_ATTRIBUTES].values[14].name = "Expected_Bos_valid";
    submodule_data->tables[dnx_data_aod_tables_table_MPLS_SPECIAL_LABEL_PROFILE_ATTRIBUTES].values[14].type = "uint32";
    submodule_data->tables[dnx_data_aod_tables_table_MPLS_SPECIAL_LABEL_PROFILE_ATTRIBUTES].values[14].doc = "Expected_Bos_valid";
    submodule_data->tables[dnx_data_aod_tables_table_MPLS_SPECIAL_LABEL_PROFILE_ATTRIBUTES].values[14].offset = UTILEX_OFFSETOF(dnx_data_aod_tables_MPLS_SPECIAL_LABEL_PROFILE_ATTRIBUTES_t, Expected_Bos_valid);
    submodule_data->tables[dnx_data_aod_tables_table_MPLS_SPECIAL_LABEL_PROFILE_ATTRIBUTES].values[15].name = "Force_Has_CW";
    submodule_data->tables[dnx_data_aod_tables_table_MPLS_SPECIAL_LABEL_PROFILE_ATTRIBUTES].values[15].type = "uint32";
    submodule_data->tables[dnx_data_aod_tables_table_MPLS_SPECIAL_LABEL_PROFILE_ATTRIBUTES].values[15].doc = "Force_Has_CW";
    submodule_data->tables[dnx_data_aod_tables_table_MPLS_SPECIAL_LABEL_PROFILE_ATTRIBUTES].values[15].offset = UTILEX_OFFSETOF(dnx_data_aod_tables_MPLS_SPECIAL_LABEL_PROFILE_ATTRIBUTES_t, Force_Has_CW);
    submodule_data->tables[dnx_data_aod_tables_table_MPLS_SPECIAL_LABEL_PROFILE_ATTRIBUTES].values[16].name = "Force_Has_CW_valid";
    submodule_data->tables[dnx_data_aod_tables_table_MPLS_SPECIAL_LABEL_PROFILE_ATTRIBUTES].values[16].type = "uint32";
    submodule_data->tables[dnx_data_aod_tables_table_MPLS_SPECIAL_LABEL_PROFILE_ATTRIBUTES].values[16].doc = "Force_Has_CW_valid";
    submodule_data->tables[dnx_data_aod_tables_table_MPLS_SPECIAL_LABEL_PROFILE_ATTRIBUTES].values[16].offset = UTILEX_OFFSETOF(dnx_data_aod_tables_MPLS_SPECIAL_LABEL_PROFILE_ATTRIBUTES_t, Force_Has_CW_valid);
    submodule_data->tables[dnx_data_aod_tables_table_MPLS_SPECIAL_LABEL_PROFILE_ATTRIBUTES].values[17].name = "Force_Is_OAM";
    submodule_data->tables[dnx_data_aod_tables_table_MPLS_SPECIAL_LABEL_PROFILE_ATTRIBUTES].values[17].type = "uint32";
    submodule_data->tables[dnx_data_aod_tables_table_MPLS_SPECIAL_LABEL_PROFILE_ATTRIBUTES].values[17].doc = "Force_Is_OAM";
    submodule_data->tables[dnx_data_aod_tables_table_MPLS_SPECIAL_LABEL_PROFILE_ATTRIBUTES].values[17].offset = UTILEX_OFFSETOF(dnx_data_aod_tables_MPLS_SPECIAL_LABEL_PROFILE_ATTRIBUTES_t, Force_Is_OAM);
    submodule_data->tables[dnx_data_aod_tables_table_MPLS_SPECIAL_LABEL_PROFILE_ATTRIBUTES].values[18].name = "Force_Is_OAM_valid";
    submodule_data->tables[dnx_data_aod_tables_table_MPLS_SPECIAL_LABEL_PROFILE_ATTRIBUTES].values[18].type = "uint32";
    submodule_data->tables[dnx_data_aod_tables_table_MPLS_SPECIAL_LABEL_PROFILE_ATTRIBUTES].values[18].doc = "Force_Is_OAM_valid";
    submodule_data->tables[dnx_data_aod_tables_table_MPLS_SPECIAL_LABEL_PROFILE_ATTRIBUTES].values[18].offset = UTILEX_OFFSETOF(dnx_data_aod_tables_MPLS_SPECIAL_LABEL_PROFILE_ATTRIBUTES_t, Force_Is_OAM_valid);
    submodule_data->tables[dnx_data_aod_tables_table_MPLS_SPECIAL_LABEL_PROFILE_ATTRIBUTES].values[19].name = "Force_Parser_Context";
    submodule_data->tables[dnx_data_aod_tables_table_MPLS_SPECIAL_LABEL_PROFILE_ATTRIBUTES].values[19].type = "uint32";
    submodule_data->tables[dnx_data_aod_tables_table_MPLS_SPECIAL_LABEL_PROFILE_ATTRIBUTES].values[19].doc = "Force_Parser_Context";
    submodule_data->tables[dnx_data_aod_tables_table_MPLS_SPECIAL_LABEL_PROFILE_ATTRIBUTES].values[19].offset = UTILEX_OFFSETOF(dnx_data_aod_tables_MPLS_SPECIAL_LABEL_PROFILE_ATTRIBUTES_t, Force_Parser_Context);
    submodule_data->tables[dnx_data_aod_tables_table_MPLS_SPECIAL_LABEL_PROFILE_ATTRIBUTES].values[20].name = "Force_Parser_Context_valid";
    submodule_data->tables[dnx_data_aod_tables_table_MPLS_SPECIAL_LABEL_PROFILE_ATTRIBUTES].values[20].type = "uint32";
    submodule_data->tables[dnx_data_aod_tables_table_MPLS_SPECIAL_LABEL_PROFILE_ATTRIBUTES].values[20].doc = "Force_Parser_Context_valid";
    submodule_data->tables[dnx_data_aod_tables_table_MPLS_SPECIAL_LABEL_PROFILE_ATTRIBUTES].values[20].offset = UTILEX_OFFSETOF(dnx_data_aod_tables_MPLS_SPECIAL_LABEL_PROFILE_ATTRIBUTES_t, Force_Parser_Context_valid);
    submodule_data->tables[dnx_data_aod_tables_table_MPLS_SPECIAL_LABEL_PROFILE_ATTRIBUTES].values[21].name = "Force_Parser_Context_Value";
    submodule_data->tables[dnx_data_aod_tables_table_MPLS_SPECIAL_LABEL_PROFILE_ATTRIBUTES].values[21].type = "char *";
    submodule_data->tables[dnx_data_aod_tables_table_MPLS_SPECIAL_LABEL_PROFILE_ATTRIBUTES].values[21].doc = "Force_Parser_Context_Value";
    submodule_data->tables[dnx_data_aod_tables_table_MPLS_SPECIAL_LABEL_PROFILE_ATTRIBUTES].values[21].offset = UTILEX_OFFSETOF(dnx_data_aod_tables_MPLS_SPECIAL_LABEL_PROFILE_ATTRIBUTES_t, Force_Parser_Context_Value);
    submodule_data->tables[dnx_data_aod_tables_table_MPLS_SPECIAL_LABEL_PROFILE_ATTRIBUTES].values[22].name = "Force_Parser_Context_Value_valid";
    submodule_data->tables[dnx_data_aod_tables_table_MPLS_SPECIAL_LABEL_PROFILE_ATTRIBUTES].values[22].type = "uint32";
    submodule_data->tables[dnx_data_aod_tables_table_MPLS_SPECIAL_LABEL_PROFILE_ATTRIBUTES].values[22].doc = "Force_Parser_Context_Value_valid";
    submodule_data->tables[dnx_data_aod_tables_table_MPLS_SPECIAL_LABEL_PROFILE_ATTRIBUTES].values[22].offset = UTILEX_OFFSETOF(dnx_data_aod_tables_MPLS_SPECIAL_LABEL_PROFILE_ATTRIBUTES_t, Force_Parser_Context_Value_valid);
    submodule_data->tables[dnx_data_aod_tables_table_MPLS_SPECIAL_LABEL_PROFILE_ATTRIBUTES].values[23].name = "Labels_To_Terminate";
    submodule_data->tables[dnx_data_aod_tables_table_MPLS_SPECIAL_LABEL_PROFILE_ATTRIBUTES].values[23].type = "uint32";
    submodule_data->tables[dnx_data_aod_tables_table_MPLS_SPECIAL_LABEL_PROFILE_ATTRIBUTES].values[23].doc = "Labels_To_Terminate";
    submodule_data->tables[dnx_data_aod_tables_table_MPLS_SPECIAL_LABEL_PROFILE_ATTRIBUTES].values[23].offset = UTILEX_OFFSETOF(dnx_data_aod_tables_MPLS_SPECIAL_LABEL_PROFILE_ATTRIBUTES_t, Labels_To_Terminate);
    submodule_data->tables[dnx_data_aod_tables_table_MPLS_SPECIAL_LABEL_PROFILE_ATTRIBUTES].values[24].name = "Labels_To_Terminate_valid";
    submodule_data->tables[dnx_data_aod_tables_table_MPLS_SPECIAL_LABEL_PROFILE_ATTRIBUTES].values[24].type = "uint32";
    submodule_data->tables[dnx_data_aod_tables_table_MPLS_SPECIAL_LABEL_PROFILE_ATTRIBUTES].values[24].doc = "Labels_To_Terminate_valid";
    submodule_data->tables[dnx_data_aod_tables_table_MPLS_SPECIAL_LABEL_PROFILE_ATTRIBUTES].values[24].offset = UTILEX_OFFSETOF(dnx_data_aod_tables_MPLS_SPECIAL_LABEL_PROFILE_ATTRIBUTES_t, Labels_To_Terminate_valid);
    submodule_data->tables[dnx_data_aod_tables_table_MPLS_SPECIAL_LABEL_PROFILE_ATTRIBUTES].values[25].name = "PHB_Compare_Strength";
    submodule_data->tables[dnx_data_aod_tables_table_MPLS_SPECIAL_LABEL_PROFILE_ATTRIBUTES].values[25].type = "uint32";
    submodule_data->tables[dnx_data_aod_tables_table_MPLS_SPECIAL_LABEL_PROFILE_ATTRIBUTES].values[25].doc = "PHB_Compare_Strength";
    submodule_data->tables[dnx_data_aod_tables_table_MPLS_SPECIAL_LABEL_PROFILE_ATTRIBUTES].values[25].offset = UTILEX_OFFSETOF(dnx_data_aod_tables_MPLS_SPECIAL_LABEL_PROFILE_ATTRIBUTES_t, PHB_Compare_Strength);
    submodule_data->tables[dnx_data_aod_tables_table_MPLS_SPECIAL_LABEL_PROFILE_ATTRIBUTES].values[26].name = "PHB_Compare_Strength_valid";
    submodule_data->tables[dnx_data_aod_tables_table_MPLS_SPECIAL_LABEL_PROFILE_ATTRIBUTES].values[26].type = "uint32";
    submodule_data->tables[dnx_data_aod_tables_table_MPLS_SPECIAL_LABEL_PROFILE_ATTRIBUTES].values[26].doc = "PHB_Compare_Strength_valid";
    submodule_data->tables[dnx_data_aod_tables_table_MPLS_SPECIAL_LABEL_PROFILE_ATTRIBUTES].values[26].offset = UTILEX_OFFSETOF(dnx_data_aod_tables_MPLS_SPECIAL_LABEL_PROFILE_ATTRIBUTES_t, PHB_Compare_Strength_valid);
    submodule_data->tables[dnx_data_aod_tables_table_MPLS_SPECIAL_LABEL_PROFILE_ATTRIBUTES].values[27].name = "PHB_Propagation_Strength";
    submodule_data->tables[dnx_data_aod_tables_table_MPLS_SPECIAL_LABEL_PROFILE_ATTRIBUTES].values[27].type = "uint32";
    submodule_data->tables[dnx_data_aod_tables_table_MPLS_SPECIAL_LABEL_PROFILE_ATTRIBUTES].values[27].doc = "PHB_Propagation_Strength";
    submodule_data->tables[dnx_data_aod_tables_table_MPLS_SPECIAL_LABEL_PROFILE_ATTRIBUTES].values[27].offset = UTILEX_OFFSETOF(dnx_data_aod_tables_MPLS_SPECIAL_LABEL_PROFILE_ATTRIBUTES_t, PHB_Propagation_Strength);
    submodule_data->tables[dnx_data_aod_tables_table_MPLS_SPECIAL_LABEL_PROFILE_ATTRIBUTES].values[28].name = "PHB_Propagation_Strength_valid";
    submodule_data->tables[dnx_data_aod_tables_table_MPLS_SPECIAL_LABEL_PROFILE_ATTRIBUTES].values[28].type = "uint32";
    submodule_data->tables[dnx_data_aod_tables_table_MPLS_SPECIAL_LABEL_PROFILE_ATTRIBUTES].values[28].doc = "PHB_Propagation_Strength_valid";
    submodule_data->tables[dnx_data_aod_tables_table_MPLS_SPECIAL_LABEL_PROFILE_ATTRIBUTES].values[28].offset = UTILEX_OFFSETOF(dnx_data_aod_tables_MPLS_SPECIAL_LABEL_PROFILE_ATTRIBUTES_t, PHB_Propagation_Strength_valid);
    submodule_data->tables[dnx_data_aod_tables_table_MPLS_SPECIAL_LABEL_PROFILE_ATTRIBUTES].values[29].name = "Qos_Mapping_Profile";
    submodule_data->tables[dnx_data_aod_tables_table_MPLS_SPECIAL_LABEL_PROFILE_ATTRIBUTES].values[29].type = "uint32";
    submodule_data->tables[dnx_data_aod_tables_table_MPLS_SPECIAL_LABEL_PROFILE_ATTRIBUTES].values[29].doc = "Qos_Mapping_Profile";
    submodule_data->tables[dnx_data_aod_tables_table_MPLS_SPECIAL_LABEL_PROFILE_ATTRIBUTES].values[29].offset = UTILEX_OFFSETOF(dnx_data_aod_tables_MPLS_SPECIAL_LABEL_PROFILE_ATTRIBUTES_t, Qos_Mapping_Profile);
    submodule_data->tables[dnx_data_aod_tables_table_MPLS_SPECIAL_LABEL_PROFILE_ATTRIBUTES].values[30].name = "Qos_Mapping_Profile_valid";
    submodule_data->tables[dnx_data_aod_tables_table_MPLS_SPECIAL_LABEL_PROFILE_ATTRIBUTES].values[30].type = "uint32";
    submodule_data->tables[dnx_data_aod_tables_table_MPLS_SPECIAL_LABEL_PROFILE_ATTRIBUTES].values[30].doc = "Qos_Mapping_Profile_valid";
    submodule_data->tables[dnx_data_aod_tables_table_MPLS_SPECIAL_LABEL_PROFILE_ATTRIBUTES].values[30].offset = UTILEX_OFFSETOF(dnx_data_aod_tables_MPLS_SPECIAL_LABEL_PROFILE_ATTRIBUTES_t, Qos_Mapping_Profile_valid);
    submodule_data->tables[dnx_data_aod_tables_table_MPLS_SPECIAL_LABEL_PROFILE_ATTRIBUTES].values[31].name = "Remark_Compare_Strength";
    submodule_data->tables[dnx_data_aod_tables_table_MPLS_SPECIAL_LABEL_PROFILE_ATTRIBUTES].values[31].type = "uint32";
    submodule_data->tables[dnx_data_aod_tables_table_MPLS_SPECIAL_LABEL_PROFILE_ATTRIBUTES].values[31].doc = "Remark_Compare_Strength";
    submodule_data->tables[dnx_data_aod_tables_table_MPLS_SPECIAL_LABEL_PROFILE_ATTRIBUTES].values[31].offset = UTILEX_OFFSETOF(dnx_data_aod_tables_MPLS_SPECIAL_LABEL_PROFILE_ATTRIBUTES_t, Remark_Compare_Strength);
    submodule_data->tables[dnx_data_aod_tables_table_MPLS_SPECIAL_LABEL_PROFILE_ATTRIBUTES].values[32].name = "Remark_Compare_Strength_valid";
    submodule_data->tables[dnx_data_aod_tables_table_MPLS_SPECIAL_LABEL_PROFILE_ATTRIBUTES].values[32].type = "uint32";
    submodule_data->tables[dnx_data_aod_tables_table_MPLS_SPECIAL_LABEL_PROFILE_ATTRIBUTES].values[32].doc = "Remark_Compare_Strength_valid";
    submodule_data->tables[dnx_data_aod_tables_table_MPLS_SPECIAL_LABEL_PROFILE_ATTRIBUTES].values[32].offset = UTILEX_OFFSETOF(dnx_data_aod_tables_MPLS_SPECIAL_LABEL_PROFILE_ATTRIBUTES_t, Remark_Compare_Strength_valid);
    submodule_data->tables[dnx_data_aod_tables_table_MPLS_SPECIAL_LABEL_PROFILE_ATTRIBUTES].values[33].name = "Remark_Propagation_Strength";
    submodule_data->tables[dnx_data_aod_tables_table_MPLS_SPECIAL_LABEL_PROFILE_ATTRIBUTES].values[33].type = "uint32";
    submodule_data->tables[dnx_data_aod_tables_table_MPLS_SPECIAL_LABEL_PROFILE_ATTRIBUTES].values[33].doc = "Remark_Propagation_Strength";
    submodule_data->tables[dnx_data_aod_tables_table_MPLS_SPECIAL_LABEL_PROFILE_ATTRIBUTES].values[33].offset = UTILEX_OFFSETOF(dnx_data_aod_tables_MPLS_SPECIAL_LABEL_PROFILE_ATTRIBUTES_t, Remark_Propagation_Strength);
    submodule_data->tables[dnx_data_aod_tables_table_MPLS_SPECIAL_LABEL_PROFILE_ATTRIBUTES].values[34].name = "Remark_Propagation_Strength_valid";
    submodule_data->tables[dnx_data_aod_tables_table_MPLS_SPECIAL_LABEL_PROFILE_ATTRIBUTES].values[34].type = "uint32";
    submodule_data->tables[dnx_data_aod_tables_table_MPLS_SPECIAL_LABEL_PROFILE_ATTRIBUTES].values[34].doc = "Remark_Propagation_Strength_valid";
    submodule_data->tables[dnx_data_aod_tables_table_MPLS_SPECIAL_LABEL_PROFILE_ATTRIBUTES].values[34].offset = UTILEX_OFFSETOF(dnx_data_aod_tables_MPLS_SPECIAL_LABEL_PROFILE_ATTRIBUTES_t, Remark_Propagation_Strength_valid);
    submodule_data->tables[dnx_data_aod_tables_table_MPLS_SPECIAL_LABEL_PROFILE_ATTRIBUTES].values[35].name = "Special_After_Tunnel";
    submodule_data->tables[dnx_data_aod_tables_table_MPLS_SPECIAL_LABEL_PROFILE_ATTRIBUTES].values[35].type = "uint32";
    submodule_data->tables[dnx_data_aod_tables_table_MPLS_SPECIAL_LABEL_PROFILE_ATTRIBUTES].values[35].doc = "Special_After_Tunnel";
    submodule_data->tables[dnx_data_aod_tables_table_MPLS_SPECIAL_LABEL_PROFILE_ATTRIBUTES].values[35].offset = UTILEX_OFFSETOF(dnx_data_aod_tables_MPLS_SPECIAL_LABEL_PROFILE_ATTRIBUTES_t, Special_After_Tunnel);
    submodule_data->tables[dnx_data_aod_tables_table_MPLS_SPECIAL_LABEL_PROFILE_ATTRIBUTES].values[36].name = "Special_After_Tunnel_valid";
    submodule_data->tables[dnx_data_aod_tables_table_MPLS_SPECIAL_LABEL_PROFILE_ATTRIBUTES].values[36].type = "uint32";
    submodule_data->tables[dnx_data_aod_tables_table_MPLS_SPECIAL_LABEL_PROFILE_ATTRIBUTES].values[36].doc = "Special_After_Tunnel_valid";
    submodule_data->tables[dnx_data_aod_tables_table_MPLS_SPECIAL_LABEL_PROFILE_ATTRIBUTES].values[36].offset = UTILEX_OFFSETOF(dnx_data_aod_tables_MPLS_SPECIAL_LABEL_PROFILE_ATTRIBUTES_t, Special_After_Tunnel_valid);
    submodule_data->tables[dnx_data_aod_tables_table_MPLS_SPECIAL_LABEL_PROFILE_ATTRIBUTES].values[37].name = "Special_Before_Tunnel";
    submodule_data->tables[dnx_data_aod_tables_table_MPLS_SPECIAL_LABEL_PROFILE_ATTRIBUTES].values[37].type = "uint32";
    submodule_data->tables[dnx_data_aod_tables_table_MPLS_SPECIAL_LABEL_PROFILE_ATTRIBUTES].values[37].doc = "Special_Before_Tunnel";
    submodule_data->tables[dnx_data_aod_tables_table_MPLS_SPECIAL_LABEL_PROFILE_ATTRIBUTES].values[37].offset = UTILEX_OFFSETOF(dnx_data_aod_tables_MPLS_SPECIAL_LABEL_PROFILE_ATTRIBUTES_t, Special_Before_Tunnel);
    submodule_data->tables[dnx_data_aod_tables_table_MPLS_SPECIAL_LABEL_PROFILE_ATTRIBUTES].values[38].name = "Special_Before_Tunnel_valid";
    submodule_data->tables[dnx_data_aod_tables_table_MPLS_SPECIAL_LABEL_PROFILE_ATTRIBUTES].values[38].type = "uint32";
    submodule_data->tables[dnx_data_aod_tables_table_MPLS_SPECIAL_LABEL_PROFILE_ATTRIBUTES].values[38].doc = "Special_Before_Tunnel_valid";
    submodule_data->tables[dnx_data_aod_tables_table_MPLS_SPECIAL_LABEL_PROFILE_ATTRIBUTES].values[38].offset = UTILEX_OFFSETOF(dnx_data_aod_tables_MPLS_SPECIAL_LABEL_PROFILE_ATTRIBUTES_t, Special_Before_Tunnel_valid);
    submodule_data->tables[dnx_data_aod_tables_table_MPLS_SPECIAL_LABEL_PROFILE_ATTRIBUTES].values[39].name = "TTL_Compare_Strength";
    submodule_data->tables[dnx_data_aod_tables_table_MPLS_SPECIAL_LABEL_PROFILE_ATTRIBUTES].values[39].type = "uint32";
    submodule_data->tables[dnx_data_aod_tables_table_MPLS_SPECIAL_LABEL_PROFILE_ATTRIBUTES].values[39].doc = "TTL_Compare_Strength";
    submodule_data->tables[dnx_data_aod_tables_table_MPLS_SPECIAL_LABEL_PROFILE_ATTRIBUTES].values[39].offset = UTILEX_OFFSETOF(dnx_data_aod_tables_MPLS_SPECIAL_LABEL_PROFILE_ATTRIBUTES_t, TTL_Compare_Strength);
    submodule_data->tables[dnx_data_aod_tables_table_MPLS_SPECIAL_LABEL_PROFILE_ATTRIBUTES].values[40].name = "TTL_Compare_Strength_valid";
    submodule_data->tables[dnx_data_aod_tables_table_MPLS_SPECIAL_LABEL_PROFILE_ATTRIBUTES].values[40].type = "uint32";
    submodule_data->tables[dnx_data_aod_tables_table_MPLS_SPECIAL_LABEL_PROFILE_ATTRIBUTES].values[40].doc = "TTL_Compare_Strength_valid";
    submodule_data->tables[dnx_data_aod_tables_table_MPLS_SPECIAL_LABEL_PROFILE_ATTRIBUTES].values[40].offset = UTILEX_OFFSETOF(dnx_data_aod_tables_MPLS_SPECIAL_LABEL_PROFILE_ATTRIBUTES_t, TTL_Compare_Strength_valid);
    submodule_data->tables[dnx_data_aod_tables_table_MPLS_SPECIAL_LABEL_PROFILE_ATTRIBUTES].values[41].name = "TTL_Propagation_Strength";
    submodule_data->tables[dnx_data_aod_tables_table_MPLS_SPECIAL_LABEL_PROFILE_ATTRIBUTES].values[41].type = "uint32";
    submodule_data->tables[dnx_data_aod_tables_table_MPLS_SPECIAL_LABEL_PROFILE_ATTRIBUTES].values[41].doc = "TTL_Propagation_Strength";
    submodule_data->tables[dnx_data_aod_tables_table_MPLS_SPECIAL_LABEL_PROFILE_ATTRIBUTES].values[41].offset = UTILEX_OFFSETOF(dnx_data_aod_tables_MPLS_SPECIAL_LABEL_PROFILE_ATTRIBUTES_t, TTL_Propagation_Strength);
    submodule_data->tables[dnx_data_aod_tables_table_MPLS_SPECIAL_LABEL_PROFILE_ATTRIBUTES].values[42].name = "TTL_Propagation_Strength_valid";
    submodule_data->tables[dnx_data_aod_tables_table_MPLS_SPECIAL_LABEL_PROFILE_ATTRIBUTES].values[42].type = "uint32";
    submodule_data->tables[dnx_data_aod_tables_table_MPLS_SPECIAL_LABEL_PROFILE_ATTRIBUTES].values[42].doc = "TTL_Propagation_Strength_valid";
    submodule_data->tables[dnx_data_aod_tables_table_MPLS_SPECIAL_LABEL_PROFILE_ATTRIBUTES].values[42].offset = UTILEX_OFFSETOF(dnx_data_aod_tables_MPLS_SPECIAL_LABEL_PROFILE_ATTRIBUTES_t, TTL_Propagation_Strength_valid);


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_aod_tables_feature_get(
    int unit,
    dnx_data_aod_tables_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_aod, dnx_data_aod_submodule_tables, feature);
}




const dnx_data_aod_tables_ERPP_FWD_CONTEXT_FILTER___image_t *
dnx_data_aod_tables_ERPP_FWD_CONTEXT_FILTER___image_get(
    int unit)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_aod, dnx_data_aod_submodule_tables, dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER___image);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
    return (const dnx_data_aod_tables_ERPP_FWD_CONTEXT_FILTER___image_t *) data;

}

const dnx_data_aod_tables_ERPP_FWD_CONTEXT_FILTER_t *
dnx_data_aod_tables_ERPP_FWD_CONTEXT_FILTER_get(
    int unit,
    int index)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_aod, dnx_data_aod_submodule_tables, dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, index, 0);
    return (const dnx_data_aod_tables_ERPP_FWD_CONTEXT_FILTER_t *) data;

}

const dnx_data_aod_tables_EGRESS_PER_FORWARD_CODE___image_t *
dnx_data_aod_tables_EGRESS_PER_FORWARD_CODE___image_get(
    int unit)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_aod, dnx_data_aod_submodule_tables, dnx_data_aod_tables_table_EGRESS_PER_FORWARD_CODE___image);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
    return (const dnx_data_aod_tables_EGRESS_PER_FORWARD_CODE___image_t *) data;

}

const dnx_data_aod_tables_EGRESS_PER_FORWARD_CODE_t *
dnx_data_aod_tables_EGRESS_PER_FORWARD_CODE_get(
    int unit,
    int index,
    int soc_value)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_aod, dnx_data_aod_submodule_tables, dnx_data_aod_tables_table_EGRESS_PER_FORWARD_CODE);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, index, soc_value);
    return (const dnx_data_aod_tables_EGRESS_PER_FORWARD_CODE_t *) data;

}

const dnx_data_aod_tables_ERPP_FORWARD_CODE_SELECTION___image_t *
dnx_data_aod_tables_ERPP_FORWARD_CODE_SELECTION___image_get(
    int unit)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_aod, dnx_data_aod_submodule_tables, dnx_data_aod_tables_table_ERPP_FORWARD_CODE_SELECTION___image);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
    return (const dnx_data_aod_tables_ERPP_FORWARD_CODE_SELECTION___image_t *) data;

}

const dnx_data_aod_tables_ERPP_FORWARD_CODE_SELECTION_t *
dnx_data_aod_tables_ERPP_FORWARD_CODE_SELECTION_get(
    int unit,
    int index,
    int soc_value)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_aod, dnx_data_aod_submodule_tables, dnx_data_aod_tables_table_ERPP_FORWARD_CODE_SELECTION);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, index, soc_value);
    return (const dnx_data_aod_tables_ERPP_FORWARD_CODE_SELECTION_t *) data;

}

const dnx_data_aod_tables_ETPP_FORWARD_CODE_SELECTION___image_t *
dnx_data_aod_tables_ETPP_FORWARD_CODE_SELECTION___image_get(
    int unit)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_aod, dnx_data_aod_submodule_tables, dnx_data_aod_tables_table_ETPP_FORWARD_CODE_SELECTION___image);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
    return (const dnx_data_aod_tables_ETPP_FORWARD_CODE_SELECTION___image_t *) data;

}

const dnx_data_aod_tables_ETPP_FORWARD_CODE_SELECTION_t *
dnx_data_aod_tables_ETPP_FORWARD_CODE_SELECTION_get(
    int unit,
    int index,
    int soc_value)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_aod, dnx_data_aod_submodule_tables, dnx_data_aod_tables_table_ETPP_FORWARD_CODE_SELECTION);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, index, soc_value);
    return (const dnx_data_aod_tables_ETPP_FORWARD_CODE_SELECTION_t *) data;

}

const dnx_data_aod_tables_EGRESS_PARSING_START_TYPE_CONTEXT___image_t *
dnx_data_aod_tables_EGRESS_PARSING_START_TYPE_CONTEXT___image_get(
    int unit)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_aod, dnx_data_aod_submodule_tables, dnx_data_aod_tables_table_EGRESS_PARSING_START_TYPE_CONTEXT___image);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
    return (const dnx_data_aod_tables_EGRESS_PARSING_START_TYPE_CONTEXT___image_t *) data;

}

const dnx_data_aod_tables_EGRESS_PARSING_START_TYPE_CONTEXT_t *
dnx_data_aod_tables_EGRESS_PARSING_START_TYPE_CONTEXT_get(
    int unit,
    int index,
    int soc_value)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_aod, dnx_data_aod_submodule_tables, dnx_data_aod_tables_table_EGRESS_PARSING_START_TYPE_CONTEXT);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, index, soc_value);
    return (const dnx_data_aod_tables_EGRESS_PARSING_START_TYPE_CONTEXT_t *) data;

}

const dnx_data_aod_tables_EGRESS_PARSING_CONTEXT_MAP_JR1_MODE___image_t *
dnx_data_aod_tables_EGRESS_PARSING_CONTEXT_MAP_JR1_MODE___image_get(
    int unit)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_aod, dnx_data_aod_submodule_tables, dnx_data_aod_tables_table_EGRESS_PARSING_CONTEXT_MAP_JR1_MODE___image);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
    return (const dnx_data_aod_tables_EGRESS_PARSING_CONTEXT_MAP_JR1_MODE___image_t *) data;

}

const dnx_data_aod_tables_EGRESS_PARSING_CONTEXT_MAP_JR1_MODE_t *
dnx_data_aod_tables_EGRESS_PARSING_CONTEXT_MAP_JR1_MODE_get(
    int unit,
    int index,
    int soc_value)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_aod, dnx_data_aod_submodule_tables, dnx_data_aod_tables_table_EGRESS_PARSING_CONTEXT_MAP_JR1_MODE);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, index, soc_value);
    return (const dnx_data_aod_tables_EGRESS_PARSING_CONTEXT_MAP_JR1_MODE_t *) data;

}

const dnx_data_aod_tables_MPLS_SPECIAL_LABEL_PROFILE_ATTRIBUTES___image_t *
dnx_data_aod_tables_MPLS_SPECIAL_LABEL_PROFILE_ATTRIBUTES___image_get(
    int unit)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_aod, dnx_data_aod_submodule_tables, dnx_data_aod_tables_table_MPLS_SPECIAL_LABEL_PROFILE_ATTRIBUTES___image);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
    return (const dnx_data_aod_tables_MPLS_SPECIAL_LABEL_PROFILE_ATTRIBUTES___image_t *) data;

}

const dnx_data_aod_tables_MPLS_SPECIAL_LABEL_PROFILE_ATTRIBUTES_t *
dnx_data_aod_tables_MPLS_SPECIAL_LABEL_PROFILE_ATTRIBUTES_get(
    int unit,
    int index)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_aod, dnx_data_aod_submodule_tables, dnx_data_aod_tables_table_MPLS_SPECIAL_LABEL_PROFILE_ATTRIBUTES);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, index, 0);
    return (const dnx_data_aod_tables_MPLS_SPECIAL_LABEL_PROFILE_ATTRIBUTES_t *) data;

}


shr_error_e
dnx_data_aod_tables_ERPP_FWD_CONTEXT_FILTER___image_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_aod_tables_ERPP_FWD_CONTEXT_FILTER___image_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_aod, dnx_data_aod_submodule_tables, dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER___image);
    data = (const dnx_data_aod_tables_ERPP_FWD_CONTEXT_FILTER___image_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, 0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%s", data->image == NULL ? "" : data->image);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_aod_tables_ERPP_FWD_CONTEXT_FILTER_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_aod_tables_ERPP_FWD_CONTEXT_FILTER_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_aod, dnx_data_aod_submodule_tables, dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER);
    data = (const dnx_data_aod_tables_ERPP_FWD_CONTEXT_FILTER_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->valid);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%s", data->doc == NULL ? "" : data->doc);
            break;
        case 2:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%s", data->Egress_FWD_Code == NULL ? "" : data->Egress_FWD_Code);
            break;
        case 3:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->Egress_FWD_Code_mask);
            break;
        case 4:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->Egress_FWD_Code_valid);
            break;
        case 5:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->DSS_Stacking);
            break;
        case 6:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->DSS_Stacking_valid);
            break;
        case 7:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->Exclude_Source);
            break;
        case 8:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->Exclude_Source_valid);
            break;
        case 9:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->GLEM_PP_or_NON_PP);
            break;
        case 10:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->GLEM_PP_or_NON_PP_valid);
            break;
        case 11:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->Hairpin_or_Same_Interface);
            break;
        case 12:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->Hairpin_or_Same_Interface_valid);
            break;
        case 13:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->IPv4_Filters);
            break;
        case 14:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->IPv4_Filters_valid);
            break;
        case 15:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->IPv4_Filters_Dip_Zero);
            break;
        case 16:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->IPv4_Filters_Dip_Zero_valid);
            break;
        case 17:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->IPv4_Filters_Header_Checksum);
            break;
        case 18:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->IPv4_Filters_Header_Checksum_valid);
            break;
        case 19:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->IPv4_Filters_Header_Length);
            break;
        case 20:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->IPv4_Filters_Header_Length_valid);
            break;
        case 21:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->IPv4_Filters_Options);
            break;
        case 22:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->IPv4_Filters_Options_valid);
            break;
        case 23:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->IPv4_Filters_Sip_Equal_Dip);
            break;
        case 24:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->IPv4_Filters_Sip_Equal_Dip_valid);
            break;
        case 25:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->IPv4_Filters_Sip_Multicast);
            break;
        case 26:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->IPv4_Filters_Sip_Multicast_valid);
            break;
        case 27:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->IPv4_Filters_Total_Length);
            break;
        case 28:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->IPv4_Filters_Total_Length_valid);
            break;
        case 29:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->IPv4_Filters_Version);
            break;
        case 30:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->IPv4_Filters_Version_valid);
            break;
        case 31:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->IPv6_Filters);
            break;
        case 32:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->IPv6_Filters_valid);
            break;
        case 33:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->IPv6_Filters_Dip_Is_All);
            break;
        case 34:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->IPv6_Filters_Dip_Is_All_valid);
            break;
        case 35:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->IPv6_Filters_Ipv4_Cmp_Dst);
            break;
        case 36:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->IPv6_Filters_Ipv4_Cmp_Dst_valid);
            break;
        case 37:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->IPv6_Filters_Ipv4_Mapped_Dst);
            break;
        case 38:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->IPv6_Filters_Ipv4_Mapped_Dst_valid);
            break;
        case 39:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->IPv6_Filters_Link_Local_Dst);
            break;
        case 40:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->IPv6_Filters_Link_Local_Dst_valid);
            break;
        case 41:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->IPv6_Filters_Link_Local_Src);
            break;
        case 42:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->IPv6_Filters_Link_Local_Src_valid);
            break;
        case 43:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->IPv6_Filters_Loopback);
            break;
        case 44:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->IPv6_Filters_Loopback_valid);
            break;
        case 45:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->IPv6_Filters_Mc_Dst);
            break;
        case 46:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->IPv6_Filters_Mc_Dst_valid);
            break;
        case 47:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->IPv6_Filters_Next_Header_Is_Zeros);
            break;
        case 48:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->IPv6_Filters_Next_Header_Is_Zeros_valid);
            break;
        case 49:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->IPv6_Filters_Sip_Is_All);
            break;
        case 50:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->IPv6_Filters_Sip_Is_All_valid);
            break;
        case 51:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->IPv6_Filters_Sip_Is_Mc);
            break;
        case 52:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->IPv6_Filters_Sip_Is_Mc_valid);
            break;
        case 53:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->IPv6_Filters_Site_Local_Dst);
            break;
        case 54:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->IPv6_Filters_Site_Local_Dst_valid);
            break;
        case 55:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->IPv6_Filters_Site_Local_Src);
            break;
        case 56:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->IPv6_Filters_Site_Local_Src_valid);
            break;
        case 57:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->IPv6_Filters_Version);
            break;
        case 58:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->IPv6_Filters_Version_valid);
            break;
        case 59:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->Invalid_OTM);
            break;
        case 60:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->Invalid_OTM_valid);
            break;
        case 61:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->L4_Filters);
            break;
        case 62:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->L4_Filters_valid);
            break;
        case 63:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->L4_Filters_Tcp_Fragment_Incomplete_Hdr);
            break;
        case 64:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->L4_Filters_Tcp_Fragment_Incomplete_Hdr_valid);
            break;
        case 65:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->L4_Filters_Tcp_Fragment_Offset_Less_Than_8);
            break;
        case 66:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->L4_Filters_Tcp_Fragment_Offset_Less_Than_8_valid);
            break;
        case 67:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->L4_Filters_Tcp_Seq_Num_Zero_Flags_Set);
            break;
        case 68:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->L4_Filters_Tcp_Seq_Num_Zero_Flags_Set_valid);
            break;
        case 69:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->L4_Filters_Tcp_Seq_Num_Zero_Flags_Zero);
            break;
        case 70:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->L4_Filters_Tcp_Seq_Num_Zero_Flags_Zero_valid);
            break;
        case 71:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->L4_Filters_Tcp_Src_Port_Equals_Dst_Port);
            break;
        case 72:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->L4_Filters_Tcp_Src_Port_Equals_Dst_Port_valid);
            break;
        case 73:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->L4_Filters_Tcp_Syn_And_Fin_Are_Set);
            break;
        case 74:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->L4_Filters_Tcp_Syn_And_Fin_Are_Set_valid);
            break;
        case 75:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->L4_Filters_Udp_Src_Port_Equals_Dst_Port);
            break;
        case 76:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->L4_Filters_Udp_Src_Port_Equals_Dst_Port_valid);
            break;
        case 77:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->LAG_Multicast);
            break;
        case 78:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->LAG_Multicast_valid);
            break;
        case 79:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->MTU);
            break;
        case 80:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->MTU_valid);
            break;
        case 81:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->RQP_Discard);
            break;
        case 82:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->RQP_Discard_valid);
            break;
        case 83:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->Split_Horizon);
            break;
        case 84:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->Split_Horizon_valid);
            break;
        case 85:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->TDM_Indication);
            break;
        case 86:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->TDM_Indication_valid);
            break;
        case 87:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->TTL_Scoping);
            break;
        case 88:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->TTL_Scoping_valid);
            break;
        case 89:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->TTL_eq_0);
            break;
        case 90:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->TTL_eq_0_valid);
            break;
        case 91:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->TTL_eq_1);
            break;
        case 92:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->TTL_eq_1_valid);
            break;
        case 93:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->Unacceptable_Frame_Types);
            break;
        case 94:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->Unacceptable_Frame_Types_valid);
            break;
        case 95:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->Unknown_DA);
            break;
        case 96:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->Unknown_DA_valid);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_aod_tables_EGRESS_PER_FORWARD_CODE___image_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_aod_tables_EGRESS_PER_FORWARD_CODE___image_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_aod, dnx_data_aod_submodule_tables, dnx_data_aod_tables_table_EGRESS_PER_FORWARD_CODE___image);
    data = (const dnx_data_aod_tables_EGRESS_PER_FORWARD_CODE___image_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, 0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%s", data->image == NULL ? "" : data->image);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_aod_tables_EGRESS_PER_FORWARD_CODE_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_aod_tables_EGRESS_PER_FORWARD_CODE_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_aod, dnx_data_aod_submodule_tables, dnx_data_aod_tables_table_EGRESS_PER_FORWARD_CODE);
    data = (const dnx_data_aod_tables_EGRESS_PER_FORWARD_CODE_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, key1);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->valid);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%s", data->doc == NULL ? "" : data->doc);
            break;
        case 2:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%s", data->Egress_FWD_Code == NULL ? "" : data->Egress_FWD_Code);
            break;
        case 3:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->Egress_FWD_Code_mask);
            break;
        case 4:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->Egress_FWD_Code_valid);
            break;
        case 5:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->Acceptable_Frame_Type_Enable);
            break;
        case 6:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->Acceptable_Frame_Type_Enable_valid);
            break;
        case 7:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->FWD_Context_is_PP_Enable);
            break;
        case 8:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->FWD_Context_is_PP_Enable_valid);
            break;
        case 9:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->Forward_Code_IPv4_or_IPv6);
            break;
        case 10:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->Forward_Code_IPv4_or_IPv6_valid);
            break;
        case 11:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->Learn_Enable);
            break;
        case 12:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->Learn_Enable_valid);
            break;
        case 13:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->Protocol_Ethernet_Enable);
            break;
        case 14:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->Protocol_Ethernet_Enable_valid);
            break;
        case 15:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->Protocol_IPv4_MC_Enable);
            break;
        case 16:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->Protocol_IPv4_MC_Enable_valid);
            break;
        case 17:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->Protocol_IPv4_UC_Enable);
            break;
        case 18:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->Protocol_IPv4_UC_Enable_valid);
            break;
        case 19:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->Protocol_IPv6_MC_Enable);
            break;
        case 20:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->Protocol_IPv6_MC_Enable_valid);
            break;
        case 21:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->Protocol_IPv6_UC_Enable);
            break;
        case 22:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->Protocol_IPv6_UC_Enable_valid);
            break;
        case 23:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->Protocol_MPLS_Enable);
            break;
        case 24:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->Protocol_MPLS_Enable_valid);
            break;
        case 25:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->STP_Enable);
            break;
        case 26:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->STP_Enable_valid);
            break;
        case 27:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->Split_Horizon_Enable);
            break;
        case 28:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->Split_Horizon_Enable_valid);
            break;
        case 29:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->Termination_Enable);
            break;
        case 30:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->Termination_Enable_valid);
            break;
        case 31:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->Vlan_Membership_Enable);
            break;
        case 32:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->Vlan_Membership_Enable_valid);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_aod_tables_ERPP_FORWARD_CODE_SELECTION___image_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_aod_tables_ERPP_FORWARD_CODE_SELECTION___image_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_aod, dnx_data_aod_submodule_tables, dnx_data_aod_tables_table_ERPP_FORWARD_CODE_SELECTION___image);
    data = (const dnx_data_aod_tables_ERPP_FORWARD_CODE_SELECTION___image_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, 0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%s", data->image == NULL ? "" : data->image);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_aod_tables_ERPP_FORWARD_CODE_SELECTION_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_aod_tables_ERPP_FORWARD_CODE_SELECTION_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_aod, dnx_data_aod_submodule_tables, dnx_data_aod_tables_table_ERPP_FORWARD_CODE_SELECTION);
    data = (const dnx_data_aod_tables_ERPP_FORWARD_CODE_SELECTION_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, key1);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->valid);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%s", data->doc == NULL ? "" : data->doc);
            break;
        case 2:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->FAI);
            break;
        case 3:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->FAI_mask);
            break;
        case 4:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->FAI_valid);
            break;
        case 5:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->FWD_Layer_Qualifier);
            break;
        case 6:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->FWD_Layer_Qualifier_mask);
            break;
        case 7:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->FWD_Layer_Qualifier_valid);
            break;
        case 8:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%s", data->FWD_Layer_Type == NULL ? "" : data->FWD_Layer_Type);
            break;
        case 9:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->FWD_Layer_Type_mask);
            break;
        case 10:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->FWD_Layer_Type_valid);
            break;
        case 11:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%s", data->FWD_code_port_profile == NULL ? "" : data->FWD_code_port_profile);
            break;
        case 12:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->FWD_code_port_profile_mask);
            break;
        case 13:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->FWD_code_port_profile_valid);
            break;
        case 14:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->TM_Action_Type);
            break;
        case 15:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->TM_Action_Type_mask);
            break;
        case 16:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->TM_Action_Type_valid);
            break;
        case 17:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->ingress_trapped);
            break;
        case 18:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->ingress_trapped_mask);
            break;
        case 19:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->ingress_trapped_valid);
            break;
        case 20:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->is_Applet);
            break;
        case 21:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->is_Applet_mask);
            break;
        case 22:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->is_Applet_valid);
            break;
        case 23:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->is_TDM);
            break;
        case 24:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->is_TDM_mask);
            break;
        case 25:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->is_TDM_valid);
            break;
        case 26:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%s", data->Egress_FWD_Code == NULL ? "" : data->Egress_FWD_Code);
            break;
        case 27:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->Egress_FWD_Code_valid);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_aod_tables_ETPP_FORWARD_CODE_SELECTION___image_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_aod_tables_ETPP_FORWARD_CODE_SELECTION___image_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_aod, dnx_data_aod_submodule_tables, dnx_data_aod_tables_table_ETPP_FORWARD_CODE_SELECTION___image);
    data = (const dnx_data_aod_tables_ETPP_FORWARD_CODE_SELECTION___image_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, 0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%s", data->image == NULL ? "" : data->image);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_aod_tables_ETPP_FORWARD_CODE_SELECTION_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_aod_tables_ETPP_FORWARD_CODE_SELECTION_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_aod, dnx_data_aod_submodule_tables, dnx_data_aod_tables_table_ETPP_FORWARD_CODE_SELECTION);
    data = (const dnx_data_aod_tables_ETPP_FORWARD_CODE_SELECTION_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, key1);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->valid);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%s", data->doc == NULL ? "" : data->doc);
            break;
        case 2:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%s", data->ACE_Value == NULL ? "" : data->ACE_Value);
            break;
        case 3:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->ACE_Value_mask);
            break;
        case 4:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->ACE_Value_valid);
            break;
        case 5:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->FAI);
            break;
        case 6:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->FAI_mask);
            break;
        case 7:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->FAI_valid);
            break;
        case 8:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->FWD_Layer_Qualifier);
            break;
        case 9:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->FWD_Layer_Qualifier_mask);
            break;
        case 10:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->FWD_Layer_Qualifier_valid);
            break;
        case 11:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%s", data->FWD_Layer_Type == NULL ? "" : data->FWD_Layer_Type);
            break;
        case 12:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->FWD_Layer_Type_mask);
            break;
        case 13:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->FWD_Layer_Type_valid);
            break;
        case 14:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%s", data->FWD_code_port_profile == NULL ? "" : data->FWD_code_port_profile);
            break;
        case 15:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->FWD_code_port_profile_mask);
            break;
        case 16:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->FWD_code_port_profile_valid);
            break;
        case 17:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->TM_Action_Type);
            break;
        case 18:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->TM_Action_Type_mask);
            break;
        case 19:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->TM_Action_Type_valid);
            break;
        case 20:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->ingress_trapped);
            break;
        case 21:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->ingress_trapped_mask);
            break;
        case 22:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->ingress_trapped_valid);
            break;
        case 23:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->is_Applet);
            break;
        case 24:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->is_Applet_mask);
            break;
        case 25:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->is_Applet_valid);
            break;
        case 26:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->is_TDM);
            break;
        case 27:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->is_TDM_mask);
            break;
        case 28:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->is_TDM_valid);
            break;
        case 29:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%s", data->Egress_FWD_Code == NULL ? "" : data->Egress_FWD_Code);
            break;
        case 30:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->Egress_FWD_Code_valid);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_aod_tables_EGRESS_PARSING_START_TYPE_CONTEXT___image_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_aod_tables_EGRESS_PARSING_START_TYPE_CONTEXT___image_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_aod, dnx_data_aod_submodule_tables, dnx_data_aod_tables_table_EGRESS_PARSING_START_TYPE_CONTEXT___image);
    data = (const dnx_data_aod_tables_EGRESS_PARSING_START_TYPE_CONTEXT___image_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, 0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%s", data->image == NULL ? "" : data->image);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_aod_tables_EGRESS_PARSING_START_TYPE_CONTEXT_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_aod_tables_EGRESS_PARSING_START_TYPE_CONTEXT_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_aod, dnx_data_aod_submodule_tables, dnx_data_aod_tables_table_EGRESS_PARSING_START_TYPE_CONTEXT);
    data = (const dnx_data_aod_tables_EGRESS_PARSING_START_TYPE_CONTEXT_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, key1);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->valid);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%s", data->doc == NULL ? "" : data->doc);
            break;
        case 2:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%s", data->Parsing_Start_Type == NULL ? "" : data->Parsing_Start_Type);
            break;
        case 3:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->Parsing_Start_Type_mask);
            break;
        case 4:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->Parsing_Start_Type_valid);
            break;
        case 5:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%s", data->ERPP_1st_Parser_Parser_Context == NULL ? "" : data->ERPP_1st_Parser_Parser_Context);
            break;
        case 6:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->ERPP_1st_Parser_Parser_Context_valid);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_aod_tables_EGRESS_PARSING_CONTEXT_MAP_JR1_MODE___image_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_aod_tables_EGRESS_PARSING_CONTEXT_MAP_JR1_MODE___image_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_aod, dnx_data_aod_submodule_tables, dnx_data_aod_tables_table_EGRESS_PARSING_CONTEXT_MAP_JR1_MODE___image);
    data = (const dnx_data_aod_tables_EGRESS_PARSING_CONTEXT_MAP_JR1_MODE___image_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, 0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%s", data->image == NULL ? "" : data->image);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_aod_tables_EGRESS_PARSING_CONTEXT_MAP_JR1_MODE_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_aod_tables_EGRESS_PARSING_CONTEXT_MAP_JR1_MODE_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_aod, dnx_data_aod_submodule_tables, dnx_data_aod_tables_table_EGRESS_PARSING_CONTEXT_MAP_JR1_MODE);
    data = (const dnx_data_aod_tables_EGRESS_PARSING_CONTEXT_MAP_JR1_MODE_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, key1);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->valid);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%s", data->doc == NULL ? "" : data->doc);
            break;
        case 2:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%s", data->JR_FWD_CODE == NULL ? "" : data->JR_FWD_CODE);
            break;
        case 3:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->JR_FWD_CODE_mask);
            break;
        case 4:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->JR_FWD_CODE_valid);
            break;
        case 5:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%s", data->ERPP_1st_Parser_Parser_Context == NULL ? "" : data->ERPP_1st_Parser_Parser_Context);
            break;
        case 6:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->ERPP_1st_Parser_Parser_Context_valid);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_aod_tables_MPLS_SPECIAL_LABEL_PROFILE_ATTRIBUTES___image_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_aod_tables_MPLS_SPECIAL_LABEL_PROFILE_ATTRIBUTES___image_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_aod, dnx_data_aod_submodule_tables, dnx_data_aod_tables_table_MPLS_SPECIAL_LABEL_PROFILE_ATTRIBUTES___image);
    data = (const dnx_data_aod_tables_MPLS_SPECIAL_LABEL_PROFILE_ATTRIBUTES___image_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, 0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%s", data->image == NULL ? "" : data->image);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_aod_tables_MPLS_SPECIAL_LABEL_PROFILE_ATTRIBUTES_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_aod_tables_MPLS_SPECIAL_LABEL_PROFILE_ATTRIBUTES_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_aod, dnx_data_aod_submodule_tables, dnx_data_aod_tables_table_MPLS_SPECIAL_LABEL_PROFILE_ATTRIBUTES);
    data = (const dnx_data_aod_tables_MPLS_SPECIAL_LABEL_PROFILE_ATTRIBUTES_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->valid);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%s", data->doc == NULL ? "" : data->doc);
            break;
        case 2:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%s", data->VTT_MPLS_Special_Label_Profile == NULL ? "" : data->VTT_MPLS_Special_Label_Profile);
            break;
        case 3:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->VTT_MPLS_Special_Label_Profile_mask);
            break;
        case 4:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->VTT_MPLS_Special_Label_Profile_valid);
            break;
        case 5:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->Check_Bos);
            break;
        case 6:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->Check_Bos_valid);
            break;
        case 7:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->ECN_Compare_Strength);
            break;
        case 8:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->ECN_Compare_Strength_valid);
            break;
        case 9:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->ECN_Mapping_Profile);
            break;
        case 10:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->ECN_Mapping_Profile_valid);
            break;
        case 11:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->ECN_Propagation_Strength);
            break;
        case 12:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->ECN_Propagation_Strength_valid);
            break;
        case 13:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->Expected_Bos);
            break;
        case 14:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->Expected_Bos_valid);
            break;
        case 15:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->Force_Has_CW);
            break;
        case 16:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->Force_Has_CW_valid);
            break;
        case 17:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->Force_Is_OAM);
            break;
        case 18:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->Force_Is_OAM_valid);
            break;
        case 19:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->Force_Parser_Context);
            break;
        case 20:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->Force_Parser_Context_valid);
            break;
        case 21:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%s", data->Force_Parser_Context_Value == NULL ? "" : data->Force_Parser_Context_Value);
            break;
        case 22:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->Force_Parser_Context_Value_valid);
            break;
        case 23:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->Labels_To_Terminate);
            break;
        case 24:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->Labels_To_Terminate_valid);
            break;
        case 25:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->PHB_Compare_Strength);
            break;
        case 26:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->PHB_Compare_Strength_valid);
            break;
        case 27:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->PHB_Propagation_Strength);
            break;
        case 28:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->PHB_Propagation_Strength_valid);
            break;
        case 29:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->Qos_Mapping_Profile);
            break;
        case 30:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->Qos_Mapping_Profile_valid);
            break;
        case 31:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->Remark_Compare_Strength);
            break;
        case 32:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->Remark_Compare_Strength_valid);
            break;
        case 33:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->Remark_Propagation_Strength);
            break;
        case 34:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->Remark_Propagation_Strength_valid);
            break;
        case 35:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->Special_After_Tunnel);
            break;
        case 36:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->Special_After_Tunnel_valid);
            break;
        case 37:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->Special_Before_Tunnel);
            break;
        case 38:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->Special_Before_Tunnel_valid);
            break;
        case 39:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->TTL_Compare_Strength);
            break;
        case 40:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->TTL_Compare_Strength_valid);
            break;
        case 41:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->TTL_Propagation_Strength);
            break;
        case 42:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->TTL_Propagation_Strength_valid);
            break;
    }

    SHR_FUNC_EXIT;
}


const dnxc_data_table_info_t *
dnx_data_aod_tables_ERPP_FWD_CONTEXT_FILTER___image_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_aod, dnx_data_aod_submodule_tables, dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER___image);

}

const dnxc_data_table_info_t *
dnx_data_aod_tables_ERPP_FWD_CONTEXT_FILTER_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_aod, dnx_data_aod_submodule_tables, dnx_data_aod_tables_table_ERPP_FWD_CONTEXT_FILTER);

}

const dnxc_data_table_info_t *
dnx_data_aod_tables_EGRESS_PER_FORWARD_CODE___image_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_aod, dnx_data_aod_submodule_tables, dnx_data_aod_tables_table_EGRESS_PER_FORWARD_CODE___image);

}

const dnxc_data_table_info_t *
dnx_data_aod_tables_EGRESS_PER_FORWARD_CODE_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_aod, dnx_data_aod_submodule_tables, dnx_data_aod_tables_table_EGRESS_PER_FORWARD_CODE);

}

const dnxc_data_table_info_t *
dnx_data_aod_tables_ERPP_FORWARD_CODE_SELECTION___image_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_aod, dnx_data_aod_submodule_tables, dnx_data_aod_tables_table_ERPP_FORWARD_CODE_SELECTION___image);

}

const dnxc_data_table_info_t *
dnx_data_aod_tables_ERPP_FORWARD_CODE_SELECTION_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_aod, dnx_data_aod_submodule_tables, dnx_data_aod_tables_table_ERPP_FORWARD_CODE_SELECTION);

}

const dnxc_data_table_info_t *
dnx_data_aod_tables_ETPP_FORWARD_CODE_SELECTION___image_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_aod, dnx_data_aod_submodule_tables, dnx_data_aod_tables_table_ETPP_FORWARD_CODE_SELECTION___image);

}

const dnxc_data_table_info_t *
dnx_data_aod_tables_ETPP_FORWARD_CODE_SELECTION_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_aod, dnx_data_aod_submodule_tables, dnx_data_aod_tables_table_ETPP_FORWARD_CODE_SELECTION);

}

const dnxc_data_table_info_t *
dnx_data_aod_tables_EGRESS_PARSING_START_TYPE_CONTEXT___image_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_aod, dnx_data_aod_submodule_tables, dnx_data_aod_tables_table_EGRESS_PARSING_START_TYPE_CONTEXT___image);

}

const dnxc_data_table_info_t *
dnx_data_aod_tables_EGRESS_PARSING_START_TYPE_CONTEXT_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_aod, dnx_data_aod_submodule_tables, dnx_data_aod_tables_table_EGRESS_PARSING_START_TYPE_CONTEXT);

}

const dnxc_data_table_info_t *
dnx_data_aod_tables_EGRESS_PARSING_CONTEXT_MAP_JR1_MODE___image_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_aod, dnx_data_aod_submodule_tables, dnx_data_aod_tables_table_EGRESS_PARSING_CONTEXT_MAP_JR1_MODE___image);

}

const dnxc_data_table_info_t *
dnx_data_aod_tables_EGRESS_PARSING_CONTEXT_MAP_JR1_MODE_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_aod, dnx_data_aod_submodule_tables, dnx_data_aod_tables_table_EGRESS_PARSING_CONTEXT_MAP_JR1_MODE);

}

const dnxc_data_table_info_t *
dnx_data_aod_tables_MPLS_SPECIAL_LABEL_PROFILE_ATTRIBUTES___image_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_aod, dnx_data_aod_submodule_tables, dnx_data_aod_tables_table_MPLS_SPECIAL_LABEL_PROFILE_ATTRIBUTES___image);

}

const dnxc_data_table_info_t *
dnx_data_aod_tables_MPLS_SPECIAL_LABEL_PROFILE_ATTRIBUTES_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_aod, dnx_data_aod_submodule_tables, dnx_data_aod_tables_table_MPLS_SPECIAL_LABEL_PROFILE_ATTRIBUTES);

}



shr_error_e
dnx_data_aod_init(
    int unit,
    dnxc_data_module_t *module_data)
{
    SHR_FUNC_INIT_VARS(unit);

    
    module_data->name = "aod";
    module_data->nof_submodules = _dnx_data_aod_submodule_nof;
    DNXC_DATA_ALLOC(module_data->submodules, dnxc_data_submodule_t, module_data->nof_submodules, "_dnxc_data aod submodules");

    
    SHR_IF_ERR_EXIT(dnx_data_aod_tables_init(unit, &module_data->submodules[dnx_data_aod_submodule_tables]));
    
    if (dnxc_data_mgmt_is_jr2_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_aod_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_jr2_b0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_aod_attach(unit));
        SHR_IF_ERR_EXIT(jr2_b0_data_aod_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_jr2_b1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_aod_attach(unit));
        SHR_IF_ERR_EXIT(jr2_b0_data_aod_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_j2c_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_aod_attach(unit));
        SHR_IF_ERR_EXIT(j2c_a0_data_aod_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_j2c_a1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_aod_attach(unit));
        SHR_IF_ERR_EXIT(j2c_a0_data_aod_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_q2a_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_aod_attach(unit));
        SHR_IF_ERR_EXIT(q2a_a0_data_aod_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_q2a_b0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_aod_attach(unit));
        SHR_IF_ERR_EXIT(q2a_a0_data_aod_attach(unit));
        SHR_IF_ERR_EXIT(q2a_b0_data_aod_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_q2a_b1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_aod_attach(unit));
        SHR_IF_ERR_EXIT(q2a_a0_data_aod_attach(unit));
        SHR_IF_ERR_EXIT(q2a_b0_data_aod_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_j2p_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_aod_attach(unit));
        SHR_IF_ERR_EXIT(j2p_a0_data_aod_attach(unit));
    }
    else
    {
        SHR_EXIT();
    }

exit:
    SHR_FUNC_EXIT;
}
#undef BSL_LOG_MODULE

