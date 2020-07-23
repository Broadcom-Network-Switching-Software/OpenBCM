

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_INIT_PP

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_pp.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_property.h>
#include <soc/dnx/pp_stage.h>
#include <soc/dnx/dbal/auto_generated/dbal_defines_fields.h>









static shr_error_e
jr2_a0_dnx_data_pp_stages_params_set(
    int unit)
{
    int stage_index;
    dnx_data_pp_stages_params_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_pp;
    int submodule_index = dnx_data_pp_submodule_stages;
    int table_index = dnx_data_pp_stages_table_params;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = DNX_PP_STAGE_NOF;
    table->info_get.key_size[0] = DNX_PP_STAGE_NOF;

    
    table->values[0].default_val = "0";
    table->values[1].default_val = "unsupported";
    table->values[2].default_val = "unknown";
    table->values[3].default_val = "0";
    table->values[4].default_val = "0";
    table->values[5].default_val = "-1";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_pp_stages_params_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_pp_stages_table_params");

    
    default_data = (dnx_data_pp_stages_params_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->valid = 0;
    default_data->name = "unsupported";
    default_data->block = "unknown";
    default_data->is_fp = 0;
    default_data->is_kleap = 0;
    default_data->pema_id = -1;
    
    for (stage_index = 0; stage_index < table->keys[0].size; stage_index++)
    {
        data = (dnx_data_pp_stages_params_t *) dnxc_data_mgmt_table_data_get(unit, table, stage_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (DNX_PP_STAGE_NIF < table->keys[0].size)
    {
        data = (dnx_data_pp_stages_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_PP_STAGE_NIF, 0);
        data->valid = 1;
        data->name = "NIF";
        data->block = "IRPP";
        data->is_fp = 0;
        data->is_kleap = 0;
    }
    if (DNX_PP_STAGE_PRT < table->keys[0].size)
    {
        data = (dnx_data_pp_stages_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_PP_STAGE_PRT, 0);
        data->valid = 1;
        data->name = "PRT";
        data->block = "IRPP";
        data->is_fp = 0;
        data->is_kleap = 0;
    }
    if (DNX_PP_STAGE_IPARSER < table->keys[0].size)
    {
        data = (dnx_data_pp_stages_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_PP_STAGE_IPARSER, 0);
        data->valid = 1;
        data->name = "IParser";
        data->block = "IRPP";
        data->is_fp = 0;
        data->is_kleap = 0;
    }
    if (DNX_PP_STAGE_LLR < table->keys[0].size)
    {
        data = (dnx_data_pp_stages_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_PP_STAGE_LLR, 0);
        data->valid = 1;
        data->name = "LLR";
        data->block = "IRPP";
        data->is_fp = 0;
        data->is_kleap = 0;
    }
    if (DNX_PP_STAGE_VTT1 < table->keys[0].size)
    {
        data = (dnx_data_pp_stages_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_PP_STAGE_VTT1, 0);
        data->valid = 1;
        data->name = "VTT1";
        data->block = "IRPP";
        data->is_fp = 0;
        data->is_kleap = 1;
        data->pema_id = DBAL_ENUM_FVAL_PEMA_ID_VTT1;
    }
    if (DNX_PP_STAGE_VTT2 < table->keys[0].size)
    {
        data = (dnx_data_pp_stages_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_PP_STAGE_VTT2, 0);
        data->valid = 1;
        data->name = "VTT2";
        data->block = "IRPP";
        data->is_fp = 0;
        data->is_kleap = 1;
    }
    if (DNX_PP_STAGE_VTT3 < table->keys[0].size)
    {
        data = (dnx_data_pp_stages_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_PP_STAGE_VTT3, 0);
        data->valid = 1;
        data->name = "VTT3";
        data->block = "IRPP";
        data->is_fp = 0;
        data->is_kleap = 1;
        data->pema_id = DBAL_ENUM_FVAL_PEMA_ID_VTT3;
    }
    if (DNX_PP_STAGE_VTT4 < table->keys[0].size)
    {
        data = (dnx_data_pp_stages_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_PP_STAGE_VTT4, 0);
        data->valid = 1;
        data->name = "VTT4";
        data->block = "IRPP";
        data->is_fp = 0;
        data->is_kleap = 1;
    }
    if (DNX_PP_STAGE_VTT5 < table->keys[0].size)
    {
        data = (dnx_data_pp_stages_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_PP_STAGE_VTT5, 0);
        data->valid = 1;
        data->name = "VTT5";
        data->block = "IRPP";
        data->is_fp = 0;
        data->is_kleap = 1;
        data->pema_id = DBAL_ENUM_FVAL_PEMA_ID_VTT5;
    }
    if (DNX_PP_STAGE_IFWD1 < table->keys[0].size)
    {
        data = (dnx_data_pp_stages_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_PP_STAGE_IFWD1, 0);
        data->valid = 1;
        data->name = "FWD1";
        data->block = "IRPP";
        data->is_fp = 0;
        data->is_kleap = 1;
        data->pema_id = DBAL_ENUM_FVAL_PEMA_ID_FWD1;
    }
    if (DNX_PP_STAGE_IFWD2 < table->keys[0].size)
    {
        data = (dnx_data_pp_stages_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_PP_STAGE_IFWD2, 0);
        data->valid = 1;
        data->name = "FWD2";
        data->block = "IRPP";
        data->is_fp = 1;
        data->is_kleap = 1;
        data->pema_id = DBAL_ENUM_FVAL_PEMA_ID_FWD2;
    }
    if (DNX_PP_STAGE_IPMF1 < table->keys[0].size)
    {
        data = (dnx_data_pp_stages_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_PP_STAGE_IPMF1, 0);
        data->valid = 1;
        data->name = "IPMF1";
        data->block = "IRPP";
        data->is_fp = 1;
        data->is_kleap = 1;
        data->pema_id = DBAL_ENUM_FVAL_PEMA_ID_IPMF1;
    }
    if (DNX_PP_STAGE_IPMF2 < table->keys[0].size)
    {
        data = (dnx_data_pp_stages_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_PP_STAGE_IPMF2, 0);
        data->valid = 1;
        data->name = "IPMF2";
        data->block = "IRPP";
        data->is_fp = 1;
        data->is_kleap = 1;
    }
    if (DNX_PP_STAGE_FER < table->keys[0].size)
    {
        data = (dnx_data_pp_stages_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_PP_STAGE_FER, 0);
        data->valid = 1;
        data->name = "FER";
        data->block = "IRPP";
        data->is_fp = 0;
        data->is_kleap = 1;
    }
    if (DNX_PP_STAGE_IPMF3 < table->keys[0].size)
    {
        data = (dnx_data_pp_stages_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_PP_STAGE_IPMF3, 0);
        data->valid = 1;
        data->name = "IPMF3";
        data->block = "IRPP";
        data->is_fp = 1;
        data->is_kleap = 1;
        data->pema_id = DBAL_ENUM_FVAL_PEMA_ID_IPMF3;
    }
    if (DNX_PP_STAGE_LBP < table->keys[0].size)
    {
        data = (dnx_data_pp_stages_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_PP_STAGE_LBP, 0);
        data->valid = 1;
        data->name = "LBP";
        data->block = "IRPP";
        data->is_fp = 0;
        data->is_kleap = 0;
    }
    if (DNX_PP_STAGE_ITMR < table->keys[0].size)
    {
        data = (dnx_data_pp_stages_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_PP_STAGE_ITMR, 0);
        data->valid = 1;
        data->name = "ITMR";
        data->block = "IRPP";
        data->is_fp = 0;
        data->is_kleap = 0;
    }
    if (DNX_PP_STAGE_ITM < table->keys[0].size)
    {
        data = (dnx_data_pp_stages_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_PP_STAGE_ITM, 0);
        data->valid = 1;
        data->name = "ITM";
        data->block = "IRPP";
        data->is_fp = 0;
        data->is_kleap = 0;
    }
    if (DNX_PP_STAGE_ITPP < table->keys[0].size)
    {
        data = (dnx_data_pp_stages_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_PP_STAGE_ITPP, 0);
        data->valid = 1;
        data->name = "ITPP";
        data->block = "ITPP";
        data->is_fp = 0;
        data->is_kleap = 0;
    }
    if (DNX_PP_STAGE_FABRIC < table->keys[0].size)
    {
        data = (dnx_data_pp_stages_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_PP_STAGE_FABRIC, 0);
        data->valid = 1;
        data->name = "Fabric";
        data->block = "ERPP";
        data->is_fp = 0;
        data->is_kleap = 0;
    }
    if (DNX_PP_STAGE_ERPARSER < table->keys[0].size)
    {
        data = (dnx_data_pp_stages_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_PP_STAGE_ERPARSER, 0);
        data->valid = 1;
        data->name = "ERParser";
        data->block = "ERPP";
        data->is_fp = 0;
        data->is_kleap = 0;
    }
    if (DNX_PP_STAGE_ERPRP < table->keys[0].size)
    {
        data = (dnx_data_pp_stages_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_PP_STAGE_ERPRP, 0);
        data->valid = 1;
        data->name = "PRP";
        data->block = "ERPP";
        data->is_fp = 0;
        data->is_kleap = 0;
    }
    if (DNX_PP_STAGE_FILTER < table->keys[0].size)
    {
        data = (dnx_data_pp_stages_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_PP_STAGE_FILTER, 0);
        data->valid = 1;
        data->name = "Filter";
        data->block = "ERPP";
        data->is_fp = 0;
        data->is_kleap = 0;
    }
    if (DNX_PP_STAGE_EPMF < table->keys[0].size)
    {
        data = (dnx_data_pp_stages_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_PP_STAGE_EPMF, 0);
        data->valid = 1;
        data->name = "EPMF";
        data->block = "ERPP";
        data->is_fp = 1;
        data->is_kleap = 1;
    }
    if (DNX_PP_STAGE_ETMR < table->keys[0].size)
    {
        data = (dnx_data_pp_stages_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_PP_STAGE_ETMR, 0);
        data->valid = 1;
        data->name = "ETMR";
        data->block = "ERPP";
        data->is_fp = 0;
        data->is_kleap = 0;
    }
    if (DNX_PP_STAGE_ETM < table->keys[0].size)
    {
        data = (dnx_data_pp_stages_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_PP_STAGE_ETM, 0);
        data->valid = 1;
        data->name = "ETM";
        data->block = "ERPP";
        data->is_fp = 0;
        data->is_kleap = 0;
    }
    if (DNX_PP_STAGE_APPLET < table->keys[0].size)
    {
        data = (dnx_data_pp_stages_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_PP_STAGE_APPLET, 0);
        data->valid = 1;
        data->name = "Applet";
        data->block = "ETPP";
        data->is_fp = 0;
        data->is_kleap = 0;
    }
    if (DNX_PP_STAGE_ETPARSER < table->keys[0].size)
    {
        data = (dnx_data_pp_stages_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_PP_STAGE_ETPARSER, 0);
        data->valid = 1;
        data->name = "ETParser";
        data->block = "ETPP";
        data->is_fp = 0;
        data->is_kleap = 0;
    }
    if (DNX_PP_STAGE_ETPRP2 < table->keys[0].size)
    {
        data = (dnx_data_pp_stages_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_PP_STAGE_ETPRP2, 0);
        data->valid = 1;
        data->name = "PRP2";
        data->block = "ETPP";
        data->is_fp = 0;
        data->is_kleap = 0;
        data->pema_id = DBAL_ENUM_FVAL_PEMA_ID_PRP2;
    }
    if (DNX_PP_STAGE_TERM < table->keys[0].size)
    {
        data = (dnx_data_pp_stages_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_PP_STAGE_TERM, 0);
        data->valid = 1;
        data->name = "Term";
        data->block = "ETPP";
        data->is_fp = 0;
        data->is_kleap = 1;
        data->pema_id = DBAL_ENUM_FVAL_PEMA_ID_TERM;
    }
    if (DNX_PP_STAGE_EFWD < table->keys[0].size)
    {
        data = (dnx_data_pp_stages_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_PP_STAGE_EFWD, 0);
        data->valid = 1;
        data->name = "FWD";
        data->block = "ETPP";
        data->is_fp = 0;
        data->is_kleap = 1;
        data->pema_id = DBAL_ENUM_FVAL_PEMA_ID_FWD;
    }
    if (DNX_PP_STAGE_ENC1 < table->keys[0].size)
    {
        data = (dnx_data_pp_stages_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_PP_STAGE_ENC1, 0);
        data->valid = 1;
        data->name = "Enc1";
        data->block = "ETPP";
        data->is_fp = 0;
        data->is_kleap = 1;
        data->pema_id = DBAL_ENUM_FVAL_PEMA_ID_ENC1;
    }
    if (DNX_PP_STAGE_ENC2 < table->keys[0].size)
    {
        data = (dnx_data_pp_stages_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_PP_STAGE_ENC2, 0);
        data->valid = 1;
        data->name = "Enc2";
        data->block = "ETPP";
        data->is_fp = 0;
        data->is_kleap = 1;
        data->pema_id = DBAL_ENUM_FVAL_PEMA_ID_ENC2;
    }
    if (DNX_PP_STAGE_ENC3 < table->keys[0].size)
    {
        data = (dnx_data_pp_stages_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_PP_STAGE_ENC3, 0);
        data->valid = 1;
        data->name = "Enc3";
        data->block = "ETPP";
        data->is_fp = 0;
        data->is_kleap = 1;
        data->pema_id = DBAL_ENUM_FVAL_PEMA_ID_ENC3;
    }
    if (DNX_PP_STAGE_ENC4 < table->keys[0].size)
    {
        data = (dnx_data_pp_stages_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_PP_STAGE_ENC4, 0);
        data->valid = 1;
        data->name = "Enc4";
        data->block = "ETPP";
        data->is_fp = 0;
        data->is_kleap = 1;
        data->pema_id = DBAL_ENUM_FVAL_PEMA_ID_ENC4;
    }
    if (DNX_PP_STAGE_ENC5 < table->keys[0].size)
    {
        data = (dnx_data_pp_stages_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_PP_STAGE_ENC5, 0);
        data->valid = 1;
        data->name = "Enc5";
        data->block = "ETPP";
        data->is_fp = 0;
        data->is_kleap = 1;
        data->pema_id = DBAL_ENUM_FVAL_PEMA_ID_ENC5;
    }
    if (DNX_PP_STAGE_TRAP < table->keys[0].size)
    {
        data = (dnx_data_pp_stages_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_PP_STAGE_TRAP, 0);
        data->valid = 1;
        data->name = "Trap";
        data->block = "ETPP";
        data->is_fp = 0;
        data->is_kleap = 1;
        data->pema_id = DBAL_ENUM_FVAL_PEMA_ID_TRAP;
    }
    if (DNX_PP_STAGE_BTC < table->keys[0].size)
    {
        data = (dnx_data_pp_stages_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_PP_STAGE_BTC, 0);
        data->valid = 1;
        data->name = "BTC";
        data->block = "ETPP";
        data->is_fp = 0;
        data->is_kleap = 0;
    }
    if (DNX_PP_STAGE_ALIGNER < table->keys[0].size)
    {
        data = (dnx_data_pp_stages_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_PP_STAGE_ALIGNER, 0);
        data->valid = 1;
        data->name = "Aligner";
        data->block = "ETPP";
        data->is_fp = 0;
        data->is_kleap = 0;
    }
    if (DNX_PP_STAGE_EPNI < table->keys[0].size)
    {
        data = (dnx_data_pp_stages_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_PP_STAGE_EPNI, 0);
        data->valid = 1;
        data->name = "EPNI";
        data->block = "ETPP";
        data->is_fp = 0;
        data->is_kleap = 0;
    }
    if (DNX_PP_STAGE_ACE < table->keys[0].size)
    {
        data = (dnx_data_pp_stages_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_PP_STAGE_ACE, 0);
        data->valid = 0;
        data->name = "ACE";
        data->block = "Virtual";
        data->is_fp = 0;
        data->is_kleap = 0;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}






static shr_error_e
jr2_a0_dnx_data_pp_PEM_nof_pem_bits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_pp;
    int submodule_index = dnx_data_pp_submodule_PEM;
    int define_index = dnx_data_pp_PEM_define_nof_pem_bits;
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
jr2_a0_dnx_data_pp_application_device_image_set(
    int unit)
{
    dnx_data_pp_application_device_image_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_pp;
    int submodule_index = dnx_data_pp_submodule_application;
    int table_index = dnx_data_pp_application_table_device_image;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->values[0].default_val = "standard_1";
    table->values[1].default_val = "standard_1";
    
    DNXC_DATA_ALLOC(table->data, dnx_data_pp_application_device_image_t, (1 + 1  ), "data of dnx_data_pp_application_table_device_image");

    
    default_data = (dnx_data_pp_application_device_image_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->standard_image_name = "standard_1";
    default_data->name = "standard_1";
    
    data = (dnx_data_pp_application_device_image_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
    sal_memcpy(data, default_data, table->size_of_values);
    
    table->values[1].property.name = spn_PROGRAMMABILITY_IMAGE_NAME;
    table->values[1].property.doc =
        "\n"
        "Define the device application image, affects the ucode to load and the validity per image MDB tables\n"
        "\n"
    ;
    table->values[1].property.method = dnxc_data_property_method_str;
    table->values[1].property.method_str = "str";

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));

    data = (dnx_data_pp_application_device_image_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &table->values[1].property, -1, &data->name));
    
    table->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_pp_application_ucode_set(
    int unit)
{
    dnx_data_pp_application_ucode_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_pp;
    int submodule_index = dnx_data_pp_submodule_application;
    int table_index = dnx_data_pp_application_table_ucode;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->values[0].default_val = "pemla/ucode/standard_1/jer2pemla/u_code_db2pem.txt";
    
    DNXC_DATA_ALLOC(table->data, dnx_data_pp_application_ucode_t, (1 + 1  ), "data of dnx_data_pp_application_table_ucode");

    
    default_data = (dnx_data_pp_application_ucode_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->relative_path = "pemla/ucode/standard_1/jer2pemla/u_code_db2pem.txt";
    
    data = (dnx_data_pp_application_ucode_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
    sal_memcpy(data, default_data, table->size_of_values);
    
    table->values[0].property.name = spn_PROGRAMMABILITY_UCODE_RELATIVE_PATH;
    table->values[0].property.doc =
        "\n"
        "Define the aucode relative path\n"
        "\n"
    ;
    table->values[0].property.method = dnxc_data_property_method_str;
    table->values[0].property.method_str = "str";

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));

    data = (dnx_data_pp_application_ucode_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &table->values[0].property, -1, &data->relative_path));
    
    table->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}






static shr_error_e
jr2_a0_dnx_data_pp_ETPP_etps_type_size_bits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_pp;
    int submodule_index = dnx_data_pp_submodule_ETPP;
    int define_index = dnx_data_pp_ETPP_define_etps_type_size_bits;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 6;

    
    define->data = 6;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}







static shr_error_e
jr2_a0_dnx_data_pp_debug_packet_header_data_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_pp;
    int submodule_index = dnx_data_pp_submodule_debug;
    int define_index = dnx_data_pp_debug_define_packet_header_data;
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
jr2_a0_dnx_data_pp_debug_ptc_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_pp;
    int submodule_index = dnx_data_pp_submodule_debug;
    int define_index = dnx_data_pp_debug_define_ptc_size;
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
jr2_a0_dnx_data_pp_debug_valid_bytes_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_pp;
    int submodule_index = dnx_data_pp_submodule_debug;
    int define_index = dnx_data_pp_debug_define_valid_bytes;
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
jr2_a0_dnx_data_pp_debug_time_stamp_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_pp;
    int submodule_index = dnx_data_pp_submodule_debug;
    int define_index = dnx_data_pp_debug_define_time_stamp;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 32;

    
    define->data = 32;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}




shr_error_e
jr2_a0_data_pp_attach(
    int unit)
{
    dnxc_data_module_t *module = NULL;
    dnxc_data_submodule_t *submodule = NULL;
    dnxc_data_define_t *define = NULL;
    dnxc_data_feature_t *feature = NULL;
    dnxc_data_table_t *table = NULL;
    int module_index = dnx_data_module_pp;
    int submodule_index = -1, data_index = -1;
    SHR_FUNC_INIT_VARS(unit);

    COMPILER_REFERENCE(define);
    COMPILER_REFERENCE(feature);
    COMPILER_REFERENCE(table);
    COMPILER_REFERENCE(submodule);
    COMPILER_REFERENCE(data_index);
    COMPILER_REFERENCE(submodule_index);
    module = &_dnxc_data[unit].modules[module_index];
    
    submodule_index = dnx_data_pp_submodule_stages;
    submodule = &module->submodules[submodule_index];

    

    

    
    data_index = dnx_data_pp_stages_table_params;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_pp_stages_params_set;
    
    submodule_index = dnx_data_pp_submodule_PEM;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_pp_PEM_define_nof_pem_bits;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_pp_PEM_nof_pem_bits_set;

    

    
    
    submodule_index = dnx_data_pp_submodule_application;
    submodule = &module->submodules[submodule_index];

    

    

    
    data_index = dnx_data_pp_application_table_device_image;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_pp_application_device_image_set;
    data_index = dnx_data_pp_application_table_ucode;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_pp_application_ucode_set;
    
    submodule_index = dnx_data_pp_submodule_ETPP;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_pp_ETPP_define_etps_type_size_bits;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_pp_ETPP_etps_type_size_bits_set;

    

    
    
    submodule_index = dnx_data_pp_submodule_debug;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_pp_debug_define_packet_header_data;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_pp_debug_packet_header_data_set;
    data_index = dnx_data_pp_debug_define_ptc_size;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_pp_debug_ptc_size_set;
    data_index = dnx_data_pp_debug_define_valid_bytes;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_pp_debug_valid_bytes_set;
    data_index = dnx_data_pp_debug_define_time_stamp;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_pp_debug_time_stamp_set;

    

    

    SHR_FUNC_EXIT;
}
#undef BSL_LOG_MODULE

