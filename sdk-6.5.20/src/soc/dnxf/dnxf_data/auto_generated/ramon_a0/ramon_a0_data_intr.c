

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_INTR

#include <soc/dnxf/dnxf_data/auto_generated/dnxf_data_internal_intr.h>
#include <soc/dnxf/dnxf_data/auto_generated/dnxf_data_property.h>
#include <soc/mcm/allenum.h>
#include <soc/dnxf/ramon/ramon_intr.h>








static shr_error_e
ramon_a0_dnxf_data_intr_general_nof_interrupts_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnxf_data_module_intr;
    int submodule_index = dnxf_data_intr_submodule_general;
    int define_index = dnxf_data_intr_general_define_nof_interrupts;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = RAMON_INT_LAST;

    
    define->data = RAMON_INT_LAST;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
ramon_a0_dnxf_data_intr_general_polled_irq_mode_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnxf_data_module_intr;
    int submodule_index = dnxf_data_intr_submodule_general;
    int define_index = dnxf_data_intr_general_define_polled_irq_mode;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0;

    
    define->data = 0;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = spn_POLLED_IRQ_MODE;
    define->property.doc = 
        "\n"
        "Are interrupts supported by polling mode.\n"
        "\n"
    ;
    define->property.method = dnxc_data_property_method_enable;
    define->property.method_str = "enable";

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));

    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &define->property, -1, &define->data));
    
    define->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}



static shr_error_e
ramon_a0_dnxf_data_intr_general_active_on_init_set(
    int unit)
{
    int offset_index;
    dnxf_data_intr_general_active_on_init_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnxf_data_module_intr;
    int submodule_index = dnxf_data_intr_submodule_general;
    int table_index = dnxf_data_intr_general_table_active_on_init;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = 46;
    table->info_get.key_size[0] = 46;

    
    table->values[0].default_val = "INVALIDr";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnxf_data_intr_general_active_on_init_t, (1 * (table->keys[0].size) + 1  ), "data of dnxf_data_intr_general_table_active_on_init");

    
    default_data = (dnxf_data_intr_general_active_on_init_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->intr = INVALIDr;
    
    for (offset_index = 0; offset_index < table->keys[0].size; offset_index++)
    {
        data = (dnxf_data_intr_general_active_on_init_t *) dnxc_data_mgmt_table_data_get(unit, table, offset_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (0 < table->keys[0].size)
    {
        data = (dnxf_data_intr_general_active_on_init_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
        data->intr = RAMON_INT_CCH_ERROR_ECC;
    }
    if (1 < table->keys[0].size)
    {
        data = (dnxf_data_intr_general_active_on_init_t *) dnxc_data_mgmt_table_data_get(unit, table, 1, 0);
        data->intr = RAMON_INT_LCM_ERROR_ECC;
    }
    if (2 < table->keys[0].size)
    {
        data = (dnxf_data_intr_general_active_on_init_t *) dnxc_data_mgmt_table_data_get(unit, table, 2, 0);
        data->intr = RAMON_INT_LCM_LCM_INT_REG;
    }
    if (3 < table->keys[0].size)
    {
        data = (dnxf_data_intr_general_active_on_init_t *) dnxc_data_mgmt_table_data_get(unit, table, 3, 0);
        data->intr = RAMON_INT_RTP_ERROR_ECC;
    }
    if (4 < table->keys[0].size)
    {
        data = (dnxf_data_intr_general_active_on_init_t *) dnxc_data_mgmt_table_data_get(unit, table, 4, 0);
        data->intr = RAMON_INT_RTP_GENERAL_INT_REG;
    }
    if (5 < table->keys[0].size)
    {
        data = (dnxf_data_intr_general_active_on_init_t *) dnxc_data_mgmt_table_data_get(unit, table, 5, 0);
        data->intr = RAMON_INT_MCT_ERROR_ECC;
    }
    if (6 < table->keys[0].size)
    {
        data = (dnxf_data_intr_general_active_on_init_t *) dnxc_data_mgmt_table_data_get(unit, table, 6, 0);
        data->intr = RAMON_INT_QRH_ERROR_ECC;
    }
    if (7 < table->keys[0].size)
    {
        data = (dnxf_data_intr_general_active_on_init_t *) dnxc_data_mgmt_table_data_get(unit, table, 7, 0);
        data->intr = RAMON_INT_QRH_DRH_INT_REG;
    }
    if (8 < table->keys[0].size)
    {
        data = (dnxf_data_intr_general_active_on_init_t *) dnxc_data_mgmt_table_data_get(unit, table, 8, 0);
        data->intr = RAMON_INT_QRH_CRH_INT_REG;
    }
    if (9 < table->keys[0].size)
    {
        data = (dnxf_data_intr_general_active_on_init_t *) dnxc_data_mgmt_table_data_get(unit, table, 9, 0);
        data->intr = RAMON_INT_DCML_ERROR_ECC;
    }
    if (10 < table->keys[0].size)
    {
        data = (dnxf_data_intr_general_active_on_init_t *) dnxc_data_mgmt_table_data_get(unit, table, 10, 0);
        data->intr = RAMON_INT_DCML_ERROR_FPC_FREE;
    }
    if (11 < table->keys[0].size)
    {
        data = (dnxf_data_intr_general_active_on_init_t *) dnxc_data_mgmt_table_data_get(unit, table, 11, 0);
        data->intr = RAMON_INT_DCML_ERROR_FPC_ALLOC;
    }
    if (12 < table->keys[0].size)
    {
        data = (dnxf_data_intr_general_active_on_init_t *) dnxc_data_mgmt_table_data_get(unit, table, 12, 0);
        data->intr = RAMON_INT_DCH_ERROR_ECC;
    }
    if (13 < table->keys[0].size)
    {
        data = (dnxf_data_intr_general_active_on_init_t *) dnxc_data_mgmt_table_data_get(unit, table, 13, 0);
        data->intr = RAMON_INT_FSRD_ERROR_ECC;
    }
    if (14 < table->keys[0].size)
    {
        data = (dnxf_data_intr_general_active_on_init_t *) dnxc_data_mgmt_table_data_get(unit, table, 14, 0);
        data->intr = RAMON_INT_FMAC_ERROR_ECC;
    }
    if (15 < table->keys[0].size)
    {
        data = (dnxf_data_intr_general_active_on_init_t *) dnxc_data_mgmt_table_data_get(unit, table, 15, 0);
        data->intr = RAMON_INT_FMAC_INT_REG_1;
    }
    if (16 < table->keys[0].size)
    {
        data = (dnxf_data_intr_general_active_on_init_t *) dnxc_data_mgmt_table_data_get(unit, table, 16, 0);
        data->intr = RAMON_INT_FMAC_INT_REG_2;
    }
    if (17 < table->keys[0].size)
    {
        data = (dnxf_data_intr_general_active_on_init_t *) dnxc_data_mgmt_table_data_get(unit, table, 17, 0);
        data->intr = RAMON_INT_FMAC_INT_REG_3;
    }
    if (18 < table->keys[0].size)
    {
        data = (dnxf_data_intr_general_active_on_init_t *) dnxc_data_mgmt_table_data_get(unit, table, 18, 0);
        data->intr = RAMON_INT_FMAC_INT_REG_4;
    }
    if (19 < table->keys[0].size)
    {
        data = (dnxf_data_intr_general_active_on_init_t *) dnxc_data_mgmt_table_data_get(unit, table, 19, 0);
        data->intr = RAMON_INT_FMAC_INT_REG_5;
    }
    if (20 < table->keys[0].size)
    {
        data = (dnxf_data_intr_general_active_on_init_t *) dnxc_data_mgmt_table_data_get(unit, table, 20, 0);
        data->intr = RAMON_INT_FMAC_INT_REG_6;
    }
    if (21 < table->keys[0].size)
    {
        data = (dnxf_data_intr_general_active_on_init_t *) dnxc_data_mgmt_table_data_get(unit, table, 21, 0);
        data->intr = RAMON_INT_FMAC_INT_REG_7;
    }
    if (22 < table->keys[0].size)
    {
        data = (dnxf_data_intr_general_active_on_init_t *) dnxc_data_mgmt_table_data_get(unit, table, 22, 0);
        data->intr = RAMON_INT_FMAC_INT_REG_8;
    }
    if (23 < table->keys[0].size)
    {
        data = (dnxf_data_intr_general_active_on_init_t *) dnxc_data_mgmt_table_data_get(unit, table, 23, 0);
        data->intr = RAMON_INT_ECI_ERROR_ECC;
    }
    if (24 < table->keys[0].size)
    {
        data = (dnxf_data_intr_general_active_on_init_t *) dnxc_data_mgmt_table_data_get(unit, table, 24, 0);
        data->intr = RAMON_INT_FSRD_INT_REG_0;
    }
    if (25 < table->keys[0].size)
    {
        data = (dnxf_data_intr_general_active_on_init_t *) dnxc_data_mgmt_table_data_get(unit, table, 25, 0);
        data->intr = RAMON_INT_CCH_ECC_ECC_1B_ERR_INT;
    }
    if (26 < table->keys[0].size)
    {
        data = (dnxf_data_intr_general_active_on_init_t *) dnxc_data_mgmt_table_data_get(unit, table, 26, 0);
        data->intr = RAMON_INT_CCH_ECC_ECC_2B_ERR_INT;
    }
    if (27 < table->keys[0].size)
    {
        data = (dnxf_data_intr_general_active_on_init_t *) dnxc_data_mgmt_table_data_get(unit, table, 27, 0);
        data->intr = RAMON_INT_LCM_ECC_ECC_1B_ERR_INT;
    }
    if (28 < table->keys[0].size)
    {
        data = (dnxf_data_intr_general_active_on_init_t *) dnxc_data_mgmt_table_data_get(unit, table, 28, 0);
        data->intr = RAMON_INT_LCM_ECC_ECC_2B_ERR_INT;
    }
    if (29 < table->keys[0].size)
    {
        data = (dnxf_data_intr_general_active_on_init_t *) dnxc_data_mgmt_table_data_get(unit, table, 29, 0);
        data->intr = RAMON_INT_RTP_ECC_ECC_1B_ERR_INT;
    }
    if (30 < table->keys[0].size)
    {
        data = (dnxf_data_intr_general_active_on_init_t *) dnxc_data_mgmt_table_data_get(unit, table, 30, 0);
        data->intr = RAMON_INT_RTP_ECC_ECC_2B_ERR_INT;
    }
    if (31 < table->keys[0].size)
    {
        data = (dnxf_data_intr_general_active_on_init_t *) dnxc_data_mgmt_table_data_get(unit, table, 31, 0);
        data->intr = RAMON_INT_MCT_ECC_ECC_1B_ERR_INT;
    }
    if (32 < table->keys[0].size)
    {
        data = (dnxf_data_intr_general_active_on_init_t *) dnxc_data_mgmt_table_data_get(unit, table, 32, 0);
        data->intr = RAMON_INT_MCT_ECC_ECC_2B_ERR_INT;
    }
    if (33 < table->keys[0].size)
    {
        data = (dnxf_data_intr_general_active_on_init_t *) dnxc_data_mgmt_table_data_get(unit, table, 33, 0);
        data->intr = RAMON_INT_QRH_ECC_ECC_1B_ERR_INT;
    }
    if (34 < table->keys[0].size)
    {
        data = (dnxf_data_intr_general_active_on_init_t *) dnxc_data_mgmt_table_data_get(unit, table, 34, 0);
        data->intr = RAMON_INT_QRH_ECC_ECC_2B_ERR_INT;
    }
    if (35 < table->keys[0].size)
    {
        data = (dnxf_data_intr_general_active_on_init_t *) dnxc_data_mgmt_table_data_get(unit, table, 35, 0);
        data->intr = RAMON_INT_DCML_ECC_ECC_1B_ERR_INT;
    }
    if (36 < table->keys[0].size)
    {
        data = (dnxf_data_intr_general_active_on_init_t *) dnxc_data_mgmt_table_data_get(unit, table, 36, 0);
        data->intr = RAMON_INT_DCML_ECC_ECC_2B_ERR_INT;
    }
    if (37 < table->keys[0].size)
    {
        data = (dnxf_data_intr_general_active_on_init_t *) dnxc_data_mgmt_table_data_get(unit, table, 37, 0);
        data->intr = RAMON_INT_DCH_ECC_ECC_1B_ERR_INT;
    }
    if (38 < table->keys[0].size)
    {
        data = (dnxf_data_intr_general_active_on_init_t *) dnxc_data_mgmt_table_data_get(unit, table, 38, 0);
        data->intr = RAMON_INT_DCH_ECC_ECC_2B_ERR_INT;
    }
    if (39 < table->keys[0].size)
    {
        data = (dnxf_data_intr_general_active_on_init_t *) dnxc_data_mgmt_table_data_get(unit, table, 39, 0);
        data->intr = RAMON_INT_FSRD_ECC_ECC_1B_ERR_INT;
    }
    if (40 < table->keys[0].size)
    {
        data = (dnxf_data_intr_general_active_on_init_t *) dnxc_data_mgmt_table_data_get(unit, table, 40, 0);
        data->intr = RAMON_INT_FSRD_ECC_ECC_2B_ERR_INT;
    }
    if (41 < table->keys[0].size)
    {
        data = (dnxf_data_intr_general_active_on_init_t *) dnxc_data_mgmt_table_data_get(unit, table, 41, 0);
        data->intr = RAMON_INT_FMAC_ECC_PARITY_ERR_INT;
    }
    if (42 < table->keys[0].size)
    {
        data = (dnxf_data_intr_general_active_on_init_t *) dnxc_data_mgmt_table_data_get(unit, table, 42, 0);
        data->intr = RAMON_INT_FMAC_ECC_ECC_1B_ERR_INT;
    }
    if (43 < table->keys[0].size)
    {
        data = (dnxf_data_intr_general_active_on_init_t *) dnxc_data_mgmt_table_data_get(unit, table, 43, 0);
        data->intr = RAMON_INT_FMAC_ECC_ECC_2B_ERR_INT;
    }
    if (44 < table->keys[0].size)
    {
        data = (dnxf_data_intr_general_active_on_init_t *) dnxc_data_mgmt_table_data_get(unit, table, 44, 0);
        data->intr = RAMON_INT_ECI_ECC_ECC_1B_ERR_INT;
    }
    if (45 < table->keys[0].size)
    {
        data = (dnxf_data_intr_general_active_on_init_t *) dnxc_data_mgmt_table_data_get(unit, table, 45, 0);
        data->intr = RAMON_INT_ECI_ECC_ECC_2B_ERR_INT;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
ramon_a0_dnxf_data_intr_general_disable_on_init_set(
    int unit)
{
    int offset_index;
    dnxf_data_intr_general_disable_on_init_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnxf_data_module_intr;
    int submodule_index = dnxf_data_intr_submodule_general;
    int table_index = dnxf_data_intr_general_table_disable_on_init;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = 1;
    table->info_get.key_size[0] = 1;

    
    table->values[0].default_val = "INVALIDr";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnxf_data_intr_general_disable_on_init_t, (1 * (table->keys[0].size) + 1  ), "data of dnxf_data_intr_general_table_disable_on_init");

    
    default_data = (dnxf_data_intr_general_disable_on_init_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->intr = INVALIDr;
    
    for (offset_index = 0; offset_index < table->keys[0].size; offset_index++)
    {
        data = (dnxf_data_intr_general_disable_on_init_t *) dnxc_data_mgmt_table_data_get(unit, table, offset_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (0 < table->keys[0].size)
    {
        data = (dnxf_data_intr_general_disable_on_init_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
        data->intr = RAMON_INT_FMAC_AUTO_DOC_NAME_6_0;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
ramon_a0_dnxf_data_intr_general_disable_print_on_init_set(
    int unit)
{
    int offset_index;
    dnxf_data_intr_general_disable_print_on_init_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnxf_data_module_intr;
    int submodule_index = dnxf_data_intr_submodule_general;
    int table_index = dnxf_data_intr_general_table_disable_print_on_init;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = 1;
    table->info_get.key_size[0] = 1;

    
    table->values[0].default_val = "INVALIDr";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnxf_data_intr_general_disable_print_on_init_t, (1 * (table->keys[0].size) + 1  ), "data of dnxf_data_intr_general_table_disable_print_on_init");

    
    default_data = (dnxf_data_intr_general_disable_print_on_init_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->intr = INVALIDr;
    
    for (offset_index = 0; offset_index < table->keys[0].size; offset_index++)
    {
        data = (dnxf_data_intr_general_disable_print_on_init_t *) dnxc_data_mgmt_table_data_get(unit, table, offset_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (0 < table->keys[0].size)
    {
        data = (dnxf_data_intr_general_disable_print_on_init_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
        data->intr = INVALIDr;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}







static shr_error_e
ramon_a0_dnxf_data_intr_ser_mem_mask_set(
    int unit)
{
    int index_index;
    dnxf_data_intr_ser_mem_mask_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnxf_data_module_intr;
    int submodule_index = dnxf_data_intr_submodule_ser;
    int table_index = dnxf_data_intr_ser_table_mem_mask;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = 21;
    table->info_get.key_size[0] = 21;

    
    table->values[0].default_val = "INVALIDr";
    table->values[1].default_val = "INVALIDf";
    table->values[2].default_val = "dnxc_mem_mask_mode_none";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnxf_data_intr_ser_mem_mask_t, (1 * (table->keys[0].size) + 1  ), "data of dnxf_data_intr_ser_table_mem_mask");

    
    default_data = (dnxf_data_intr_ser_mem_mask_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->reg = INVALIDr;
    default_data->field = INVALIDf;
    default_data->mode = dnxc_mem_mask_mode_none;
    
    for (index_index = 0; index_index < table->keys[0].size; index_index++)
    {
        data = (dnxf_data_intr_ser_mem_mask_t *) dnxc_data_mgmt_table_data_get(unit, table, index_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (0 < table->keys[0].size)
    {
        data = (dnxf_data_intr_ser_mem_mask_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
        data->reg = BRDC_DCH_ECC_ERR_1B_MONITOR_MEM_MASKr;
        data->field = INVALIDf;
        data->mode = dnxc_mem_mask_mode_one;
    }
    if (1 < table->keys[0].size)
    {
        data = (dnxf_data_intr_ser_mem_mask_t *) dnxc_data_mgmt_table_data_get(unit, table, 1, 0);
        data->reg = BRDC_DCH_ECC_ERR_2B_MONITOR_MEM_MASKr;
        data->field = INVALIDf;
        data->mode = dnxc_mem_mask_mode_one;
    }
    if (2 < table->keys[0].size)
    {
        data = (dnxf_data_intr_ser_mem_mask_t *) dnxc_data_mgmt_table_data_get(unit, table, 2, 0);
        data->reg = BRDC_CCH_ECC_ERR_1B_MONITOR_MEM_MASKr;
        data->field = INVALIDf;
        data->mode = dnxc_mem_mask_mode_one;
    }
    if (3 < table->keys[0].size)
    {
        data = (dnxf_data_intr_ser_mem_mask_t *) dnxc_data_mgmt_table_data_get(unit, table, 3, 0);
        data->reg = BRDC_CCH_ECC_ERR_2B_MONITOR_MEM_MASKr;
        data->field = INVALIDf;
        data->mode = dnxc_mem_mask_mode_one;
    }
    if (4 < table->keys[0].size)
    {
        data = (dnxf_data_intr_ser_mem_mask_t *) dnxc_data_mgmt_table_data_get(unit, table, 4, 0);
        data->reg = BRDC_DCML_ECC_ERR_1B_MONITOR_MEM_MASKr;
        data->field = INVALIDf;
        data->mode = dnxc_mem_mask_mode_one;
    }
    if (5 < table->keys[0].size)
    {
        data = (dnxf_data_intr_ser_mem_mask_t *) dnxc_data_mgmt_table_data_get(unit, table, 5, 0);
        data->reg = BRDC_DCML_ECC_ERR_2B_MONITOR_MEM_MASKr;
        data->field = INVALIDf;
        data->mode = dnxc_mem_mask_mode_one;
    }
    if (6 < table->keys[0].size)
    {
        data = (dnxf_data_intr_ser_mem_mask_t *) dnxc_data_mgmt_table_data_get(unit, table, 6, 0);
        data->reg = BRDC_FMAC_ECC_ERR_1B_MONITOR_MEM_MASKr;
        data->field = INVALIDf;
        data->mode = dnxc_mem_mask_mode_one;
    }
    if (7 < table->keys[0].size)
    {
        data = (dnxf_data_intr_ser_mem_mask_t *) dnxc_data_mgmt_table_data_get(unit, table, 7, 0);
        data->reg = BRDC_FMAC_ECC_ERR_2B_MONITOR_MEM_MASKr;
        data->field = INVALIDf;
        data->mode = dnxc_mem_mask_mode_one;
    }
    if (8 < table->keys[0].size)
    {
        data = (dnxf_data_intr_ser_mem_mask_t *) dnxc_data_mgmt_table_data_get(unit, table, 8, 0);
        data->reg = BRDC_FMAC_PAR_ERR_MEM_MASKr;
        data->field = INVALIDf;
        data->mode = dnxc_mem_mask_mode_one;
    }
    if (9 < table->keys[0].size)
    {
        data = (dnxf_data_intr_ser_mem_mask_t *) dnxc_data_mgmt_table_data_get(unit, table, 9, 0);
        data->reg = BRDC_LCM_ECC_ERR_1B_MONITOR_MEM_MASKr;
        data->field = INVALIDf;
        data->mode = dnxc_mem_mask_mode_one;
    }
    if (10 < table->keys[0].size)
    {
        data = (dnxf_data_intr_ser_mem_mask_t *) dnxc_data_mgmt_table_data_get(unit, table, 10, 0);
        data->reg = BRDC_LCM_ECC_ERR_2B_MONITOR_MEM_MASKr;
        data->field = INVALIDf;
        data->mode = dnxc_mem_mask_mode_one;
    }
    if (11 < table->keys[0].size)
    {
        data = (dnxf_data_intr_ser_mem_mask_t *) dnxc_data_mgmt_table_data_get(unit, table, 11, 0);
        data->reg = BRDC_QRH_ECC_ERR_1B_MONITOR_MEM_MASKr;
        data->field = INVALIDf;
        data->mode = dnxc_mem_mask_mode_one;
    }
    if (12 < table->keys[0].size)
    {
        data = (dnxf_data_intr_ser_mem_mask_t *) dnxc_data_mgmt_table_data_get(unit, table, 12, 0);
        data->reg = BRDC_QRH_ECC_ERR_2B_MONITOR_MEM_MASKr;
        data->field = INVALIDf;
        data->mode = dnxc_mem_mask_mode_one;
    }
    if (13 < table->keys[0].size)
    {
        data = (dnxf_data_intr_ser_mem_mask_t *) dnxc_data_mgmt_table_data_get(unit, table, 13, 0);
        data->reg = ECI_ECC_ERR_1B_MONITOR_MEM_MASKr;
        data->field = INVALIDf;
        data->mode = dnxc_mem_mask_mode_one;
    }
    if (14 < table->keys[0].size)
    {
        data = (dnxf_data_intr_ser_mem_mask_t *) dnxc_data_mgmt_table_data_get(unit, table, 14, 0);
        data->reg = ECI_ECC_ERR_2B_MONITOR_MEM_MASKr;
        data->field = INVALIDf;
        data->mode = dnxc_mem_mask_mode_one;
    }
    if (15 < table->keys[0].size)
    {
        data = (dnxf_data_intr_ser_mem_mask_t *) dnxc_data_mgmt_table_data_get(unit, table, 15, 0);
        data->reg = MCT_ECC_ERR_1B_MONITOR_MEM_MASKr;
        data->field = INVALIDf;
        data->mode = dnxc_mem_mask_mode_one;
    }
    if (16 < table->keys[0].size)
    {
        data = (dnxf_data_intr_ser_mem_mask_t *) dnxc_data_mgmt_table_data_get(unit, table, 16, 0);
        data->reg = MCT_ECC_ERR_2B_MONITOR_MEM_MASKr;
        data->field = INVALIDf;
        data->mode = dnxc_mem_mask_mode_one;
    }
    if (17 < table->keys[0].size)
    {
        data = (dnxf_data_intr_ser_mem_mask_t *) dnxc_data_mgmt_table_data_get(unit, table, 17, 0);
        data->reg = RTP_ECC_ERR_1B_MONITOR_MEM_MASKr;
        data->field = INVALIDf;
        data->mode = dnxc_mem_mask_mode_one;
    }
    if (18 < table->keys[0].size)
    {
        data = (dnxf_data_intr_ser_mem_mask_t *) dnxc_data_mgmt_table_data_get(unit, table, 18, 0);
        data->reg = RTP_ECC_ERR_2B_MONITOR_MEM_MASKr;
        data->field = INVALIDf;
        data->mode = dnxc_mem_mask_mode_one;
    }
    if (19 < table->keys[0].size)
    {
        data = (dnxf_data_intr_ser_mem_mask_t *) dnxc_data_mgmt_table_data_get(unit, table, 19, 0);
        data->reg = BRDC_FMAC_ECC_ERR_1B_MONITOR_MEM_MASKr;
        data->field = TX_TRR_DATA_HEADER_ECC_1B_ERR_MASKf;
        data->mode = dnxc_mem_mask_mode_zero;
    }
    if (20 < table->keys[0].size)
    {
        data = (dnxf_data_intr_ser_mem_mask_t *) dnxc_data_mgmt_table_data_get(unit, table, 20, 0);
        data->reg = BRDC_FMAC_ECC_ERR_2B_MONITOR_MEM_MASKr;
        data->field = TX_TRR_DATA_HEADER_ECC_2B_ERR_MASKf;
        data->mode = dnxc_mem_mask_mode_zero;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
ramon_a0_dnxf_data_intr_ser_xor_mem_set(
    int unit)
{
    int index_index;
    dnxf_data_intr_ser_xor_mem_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnxf_data_module_intr;
    int submodule_index = dnxf_data_intr_submodule_ser;
    int table_index = dnxf_data_intr_ser_table_xor_mem;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = 1;
    table->info_get.key_size[0] = 1;

    
    table->values[0].default_val = "INVALIDm";
    table->values[1].default_val = "0";
    table->values[2].default_val = "0";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnxf_data_intr_ser_xor_mem_t, (1 * (table->keys[0].size) + 1  ), "data of dnxf_data_intr_ser_table_xor_mem");

    
    default_data = (dnxf_data_intr_ser_xor_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->mem = INVALIDm;
    default_data->sram_banks_bits = 0;
    default_data->entry_used_bits = 0;
    
    for (index_index = 0; index_index < table->keys[0].size; index_index++)
    {
        data = (dnxf_data_intr_ser_xor_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, index_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (0 < table->keys[0].size)
    {
        data = (dnxf_data_intr_ser_xor_mem_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
        data->mem = MCT_MCTm;
        data->sram_banks_bits = 2;
        data->entry_used_bits = 13;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}



shr_error_e
ramon_a0_data_intr_attach(
    int unit)
{
    dnxc_data_module_t *module = NULL;
    dnxc_data_submodule_t *submodule = NULL;
    dnxc_data_define_t *define = NULL;
    dnxc_data_feature_t *feature = NULL;
    dnxc_data_table_t *table = NULL;
    int module_index = dnxf_data_module_intr;
    int submodule_index = -1, data_index = -1;
    SHR_FUNC_INIT_VARS(unit);

    COMPILER_REFERENCE(define);
    COMPILER_REFERENCE(feature);
    COMPILER_REFERENCE(table);
    COMPILER_REFERENCE(submodule);
    COMPILER_REFERENCE(data_index);
    COMPILER_REFERENCE(submodule_index);
    module = &_dnxc_data[unit].modules[module_index];
    
    submodule_index = dnxf_data_intr_submodule_general;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnxf_data_intr_general_define_nof_interrupts;
    define = &submodule->defines[data_index];
    define->set = ramon_a0_dnxf_data_intr_general_nof_interrupts_set;
    data_index = dnxf_data_intr_general_define_polled_irq_mode;
    define = &submodule->defines[data_index];
    define->set = ramon_a0_dnxf_data_intr_general_polled_irq_mode_set;

    

    
    data_index = dnxf_data_intr_general_table_active_on_init;
    table = &submodule->tables[data_index];
    table->set = ramon_a0_dnxf_data_intr_general_active_on_init_set;
    data_index = dnxf_data_intr_general_table_disable_on_init;
    table = &submodule->tables[data_index];
    table->set = ramon_a0_dnxf_data_intr_general_disable_on_init_set;
    data_index = dnxf_data_intr_general_table_disable_print_on_init;
    table = &submodule->tables[data_index];
    table->set = ramon_a0_dnxf_data_intr_general_disable_print_on_init_set;
    
    submodule_index = dnxf_data_intr_submodule_ser;
    submodule = &module->submodules[submodule_index];

    

    

    
    data_index = dnxf_data_intr_ser_table_mem_mask;
    table = &submodule->tables[data_index];
    table->set = ramon_a0_dnxf_data_intr_ser_mem_mask_set;
    data_index = dnxf_data_intr_ser_table_xor_mem;
    table = &submodule->tables[data_index];
    table->set = ramon_a0_dnxf_data_intr_ser_xor_mem_set;

    SHR_FUNC_EXIT;
}
#undef BSL_LOG_MODULE

