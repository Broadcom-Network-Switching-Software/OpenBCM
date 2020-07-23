

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_PORT

#include <soc/dnxf/dnxf_data/auto_generated/dnxf_data_internal_port.h>
#include <soc/dnxf/dnxf_data/auto_generated/dnxf_data_property.h>
#include <bcm/port.h>
#include <bcm/types.h>
#include <shared/pbmp.h>
#include <soc/dnxc/dnxc_defs.h>
#include <soc/dnxc/dnxc_port.h>
#include <soc/dnxf/cmn/dnxf_defs.h>
#include <soc/dnxf/cmn/dnxf_lane_map_db.h>
#include <soc/portmod/portmod.h>
#include <soc/dnxf/dnxf_data/auto_generated/dnxf_data_device.h>








static shr_error_e
ramon_a0_dnxf_data_port_general_nof_links_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnxf_data_module_port;
    int submodule_index = dnxf_data_port_submodule_general;
    int define_index = dnxf_data_port_general_define_nof_links;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 192;

    
    define->data = 192;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
ramon_a0_dnxf_data_port_general_nof_pms_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnxf_data_module_port;
    int submodule_index = dnxf_data_port_submodule_general;
    int define_index = dnxf_data_port_general_define_nof_pms;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 24;

    
    define->data = 24;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
ramon_a0_dnxf_data_port_general_max_bucket_fill_rate_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnxf_data_module_port;
    int submodule_index = dnxf_data_port_submodule_general;
    int define_index = dnxf_data_port_general_define_max_bucket_fill_rate;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0xB;

    
    define->data = 0xB;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
ramon_a0_dnxf_data_port_general_link_mask_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnxf_data_module_port;
    int submodule_index = dnxf_data_port_submodule_general;
    int define_index = dnxf_data_port_general_define_link_mask;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0xFF;

    
    define->data = 0xFF;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
ramon_a0_dnxf_data_port_general_aldwp_max_val_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnxf_data_module_port;
    int submodule_index = dnxf_data_port_submodule_general;
    int define_index = dnxf_data_port_general_define_aldwp_max_val;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0x3F;

    
    define->data = 0x3F;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
ramon_a0_dnxf_data_port_general_fmac_clock_khz_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnxf_data_module_port;
    int submodule_index = dnxf_data_port_submodule_general;
    int define_index = dnxf_data_port_general_define_fmac_clock_khz;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 892000;

    
    define->data = 892000;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
ramon_a0_dnxf_data_port_general_speed_max_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnxf_data_module_port;
    int submodule_index = dnxf_data_port_submodule_general;
    int define_index = dnxf_data_port_general_define_speed_max;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 53125;

    
    define->data = 53125;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
ramon_a0_dnxf_data_port_general_fw_load_method_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnxf_data_module_port;
    int submodule_index = dnxf_data_port_submodule_general;
    int define_index = dnxf_data_port_general_define_fw_load_method;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0;

    
    define->data = 0;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = "load_firmware";
    define->property.doc = 
        "\n"
        "The method of the firmware load.\n"
        "2 - fast; 1 - MDIO load; 0 - do not load firmware.\n"
        "Byte 0 of the SoC property value.\n"
        "\n"
    ;
    define->property.method = dnxc_data_property_method_custom;
    define->property.method_str = "custom";

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));

    SHR_IF_ERR_EXIT(dnxf_data_property_port_general_fw_load_method_read(unit, (uint32 *) &define->data));
    
    define->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
ramon_a0_dnxf_data_port_general_fw_crc_check_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnxf_data_module_port;
    int submodule_index = dnxf_data_port_submodule_general;
    int define_index = dnxf_data_port_general_define_fw_crc_check;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0;

    
    define->data = 0;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = "load_firmware";
    define->property.doc = 
        "\n"
        "Enable firmware CRC check.\n"
        "Byte 1: bit 0 of the SoC property value.\n"
        "\n"
    ;
    define->property.method = dnxc_data_property_method_custom;
    define->property.method_str = "custom";

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));

    SHR_IF_ERR_EXIT(dnxf_data_property_port_general_fw_crc_check_read(unit, (uint32 *) &define->data));
    
    define->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
ramon_a0_dnxf_data_port_general_fw_load_verify_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnxf_data_module_port;
    int submodule_index = dnxf_data_port_submodule_general;
    int define_index = dnxf_data_port_general_define_fw_load_verify;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0;

    
    define->data = 0;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = "load_firmware";
    define->property.doc = 
        "\n"
        "Enable firmware verification, byte by byte (time consuming).\n"
        "Byte 1: bit 4 of the SoC property value.\n"
        "\n"
    ;
    define->property.method = dnxc_data_property_method_custom;
    define->property.method_str = "custom";

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));

    SHR_IF_ERR_EXIT(dnxf_data_property_port_general_fw_load_verify_read(unit, (uint32 *) &define->data));
    
    define->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
ramon_a0_dnxf_data_port_general_mac_bucket_fill_rate_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnxf_data_module_port;
    int submodule_index = dnxf_data_port_submodule_general;
    int define_index = dnxf_data_port_general_define_mac_bucket_fill_rate;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 6;

    
    define->data = 6;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = spn_FABRIC_MAC_BUCKET_FILL_RATE;
    define->property.doc = 
        "\n"
        "Mac bucket fill rate.\n"
        "Bucket-Fill-Rate. Number of good cells that add a token to the bucket.\n"
        "BktFillRate 3:0 must be configured to be no bigger than 0xB.\n"
        "The number BktFillRate 3:0 is the exponent of 2,\n"
        "i.e., the number of good cells adding a token to the bucket is 2^BktFillRate 3:0.\n"
        "Good cell is defined according to RxCntCfg\n"
        "\n"
    ;
    define->property.method = dnxc_data_property_method_range;
    define->property.method_str = "range";
    define->property.range_min = 0;
    define->property.range_max = 11;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));

    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &define->property, -1, &define->data));
    
    define->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}



static shr_error_e
ramon_a0_dnxf_data_port_general_supported_phys_set(
    int unit)
{
    dnxf_data_port_general_supported_phys_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnxf_data_module_port;
    int submodule_index = dnxf_data_port_submodule_general;
    int table_index = dnxf_data_port_general_table_supported_phys;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->values[0].default_val = "0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff";
    
    DNXC_DATA_ALLOC(table->data, dnxf_data_port_general_supported_phys_t, (1 + 1  ), "data of dnxf_data_port_general_table_supported_phys");

    
    default_data = (dnxf_data_port_general_supported_phys_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    #if 6 > _SHR_PBMP_WORD_MAX
    #error "out of bound access to array"
    #endif
    _SHR_PBMP_WORD_SET(default_data->pbmp, 0, 0xffffffff);
    _SHR_PBMP_WORD_SET(default_data->pbmp, 1, 0xffffffff);
    _SHR_PBMP_WORD_SET(default_data->pbmp, 2, 0xffffffff);
    _SHR_PBMP_WORD_SET(default_data->pbmp, 3, 0xffffffff);
    _SHR_PBMP_WORD_SET(default_data->pbmp, 4, 0xffffffff);
    _SHR_PBMP_WORD_SET(default_data->pbmp, 5, 0xffffffff);
    
    data = (dnxf_data_port_general_supported_phys_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
    sal_memcpy(data, default_data, table->size_of_values);

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
ramon_a0_dnxf_data_port_general_fmac_bus_size_set(
    int unit)
{
    int mode_index;
    dnxf_data_port_general_fmac_bus_size_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnxf_data_module_port;
    int submodule_index = dnxf_data_port_submodule_general;
    int table_index = dnxf_data_port_general_table_fmac_bus_size;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = bcmPortPhyFecCount;
    table->info_get.key_size[0] = bcmPortPhyFecCount;

    
    table->values[0].default_val = "0";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnxf_data_port_general_fmac_bus_size_t, (1 * (table->keys[0].size) + 1  ), "data of dnxf_data_port_general_table_fmac_bus_size");

    
    default_data = (dnxf_data_port_general_fmac_bus_size_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->size = 0;
    
    for (mode_index = 0; mode_index < table->keys[0].size; mode_index++)
    {
        data = (dnxf_data_port_general_fmac_bus_size_t *) dnxc_data_mgmt_table_data_get(unit, table, mode_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (bcmPortPhyFecBaseR < table->keys[0].size)
    {
        data = (dnxf_data_port_general_fmac_bus_size_t *) dnxc_data_mgmt_table_data_get(unit, table, bcmPortPhyFecBaseR, 0);
        data->size = 66;
    }
    if (bcmPortPhyFecNone < table->keys[0].size)
    {
        data = (dnxf_data_port_general_fmac_bus_size_t *) dnxc_data_mgmt_table_data_get(unit, table, bcmPortPhyFecNone, 0);
        data->size = 66;
    }
    if (bcmPortPhyFecRs206 < table->keys[0].size)
    {
        data = (dnxf_data_port_general_fmac_bus_size_t *) dnxc_data_mgmt_table_data_get(unit, table, bcmPortPhyFecRs206, 0);
        data->size = 70;
    }
    if (bcmPortPhyFecRs108 < table->keys[0].size)
    {
        data = (dnxf_data_port_general_fmac_bus_size_t *) dnxc_data_mgmt_table_data_get(unit, table, bcmPortPhyFecRs108, 0);
        data->size = 70;
    }
    if (bcmPortPhyFecRs545 < table->keys[0].size)
    {
        data = (dnxf_data_port_general_fmac_bus_size_t *) dnxc_data_mgmt_table_data_get(unit, table, bcmPortPhyFecRs545, 0);
        data->size = 70;
    }
    if (bcmPortPhyFecRs304 < table->keys[0].size)
    {
        data = (dnxf_data_port_general_fmac_bus_size_t *) dnxc_data_mgmt_table_data_get(unit, table, bcmPortPhyFecRs304, 0);
        data->size = 70;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}






static shr_error_e
ramon_a0_dnxf_data_port_stat_thread_interval_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnxf_data_module_port;
    int submodule_index = dnxf_data_port_submodule_stat;
    int define_index = dnxf_data_port_stat_define_thread_interval;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0;

    
    define->data = 0;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->flags |= DNXC_DATA_F_INIT_ONLY;
    
    define->property.name = spn_BCM_STAT_INTERVAL;
    define->property.doc = 
        "\n"
        "Sets the statistics thread interval.\n"
        "Value is in microseconds.\n"
        "\n"
    ;
    define->property.method = dnxc_data_property_method_range;
    define->property.method_str = "range";
    define->property.range_min = 0;
    define->property.range_max = 4294967295;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));

    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &define->property, -1, &define->data));
    
    define->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
ramon_a0_dnxf_data_port_stat_thread_counter_pri_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnxf_data_module_port;
    int submodule_index = dnxf_data_port_submodule_stat;
    int define_index = dnxf_data_port_stat_define_thread_counter_pri;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0;

    
    define->data = 0;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = spn_COUNTER_THREAD_PRI;
    define->property.doc = 
        "\n"
        "Sets the counter thread priority.\n"
        "0 is highes and 255 is lowest.\n"
        "\n"
    ;
    define->property.method = dnxc_data_property_method_range;
    define->property.method_str = "range";
    define->property.range_min = 0;
    define->property.range_max = 255;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));

    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &define->property, -1, &define->data));
    
    define->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
ramon_a0_dnxf_data_port_stat_thread_timeout_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnxf_data_module_port;
    int submodule_index = dnxf_data_port_submodule_stat;
    int define_index = dnxf_data_port_stat_define_thread_timeout;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0;

    
    define->data = 0;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = spn_BCM_STAT_SYNC_TIMEOUT;
    define->property.doc = 
        "\n"
        "Statistic thread sync operation timeout\n"
        "\n"
    ;
    define->property.method = dnxc_data_property_method_range;
    define->property.method_str = "range";
    define->property.range_min = 0;
    define->property.range_max = 255;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));

    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &define->property, -1, &define->data));
    
    define->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}








static shr_error_e
ramon_a0_dnxf_data_port_static_add_port_info_set(
    int unit)
{
    int port_index;
    dnxf_data_port_static_add_port_info_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnxf_data_module_port;
    int submodule_index = dnxf_data_port_submodule_static_add;
    int table_index = dnxf_data_port_static_add_table_port_info;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->flags |= DNXC_DATA_F_INIT_ONLY;

    
    table->keys[0].size = dnxf_data_port.general.nof_links_get(unit);
    table->info_get.key_size[0] = dnxf_data_port.general.nof_links_get(unit);

    
    table->values[0].default_val = "DNXC_PORT_INVALID_SPEED";
    table->values[1].default_val = "BCM_PORT_RESOURCE_DEFAULT_REQUEST";
    table->values[2].default_val = "BCM_PORT_RESOURCE_DEFAULT_REQUEST";
    table->values[3].default_val = "0";
    table->values[4].default_val = "0";
    table->values[5].default_val = "0";
    table->values[6].default_val = "0";
    table->values[7].default_val = "0";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnxf_data_port_static_add_port_info_t, (1 * (table->keys[0].size) + 1  ), "data of dnxf_data_port_static_add_table_port_info");

    
    default_data = (dnxf_data_port_static_add_port_info_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->speed = DNXC_PORT_INVALID_SPEED;
    default_data->link_training = BCM_PORT_RESOURCE_DEFAULT_REQUEST;
    default_data->fec_type = BCM_PORT_RESOURCE_DEFAULT_REQUEST;
    default_data->tx_polarity = 0;
    default_data->rx_polarity = 0;
    default_data->connected_to_repeater = 0;
    default_data->tx_pam4_precoder = 0;
    default_data->lp_tx_precoder = 0;
    
    for (port_index = 0; port_index < table->keys[0].size; port_index++)
    {
        data = (dnxf_data_port_static_add_port_info_t *) dnxc_data_mgmt_table_data_get(unit, table, port_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    table->values[0].property.name = spn_PORT_INIT_SPEED;
    table->values[0].property.doc =
        "\n"
        "Port data speed.\n"
        "To set different port rate use:\n"
        "port_init_speed_#port#=#speed#\n"
        "Speed value -1 is ALLOWED.\n"
        "If speed is set to be -1 this will boot the device with the port disabled.\n"
        "\n"
    ;
    table->values[0].property.method = dnxc_data_property_method_port_direct_map;
    table->values[0].property.method_str = "port_direct_map";
    table->values[0].property.nof_mapping = 10;
    DNXC_DATA_ALLOC(table->values[0].property.mapping, dnxc_data_property_map_t, table->values[0].property.nof_mapping, "dnxf_data_port_static_add_port_info_t property mapping");

    table->values[0].property.mapping[0].name = "-1";
    table->values[0].property.mapping[0].val = -1;
    table->values[0].property.mapping[1].name = "10312";
    table->values[0].property.mapping[1].val = 10312;
    table->values[0].property.mapping[2].name = "11500";
    table->values[0].property.mapping[2].val = 11500;
    table->values[0].property.mapping[3].name = "20625";
    table->values[0].property.mapping[3].val = 20625;
    table->values[0].property.mapping[4].name = "23000";
    table->values[0].property.mapping[4].val = 23000;
    table->values[0].property.mapping[5].name = "25000";
    table->values[0].property.mapping[5].val = 25000;
    table->values[0].property.mapping[6].name = "25781";
    table->values[0].property.mapping[6].val = 25781;
    table->values[0].property.mapping[7].name = "50000";
    table->values[0].property.mapping[7].val = 50000;
    table->values[0].property.mapping[8].name = "53125";
    table->values[0].property.mapping[8].val = 53125;
    table->values[0].property.mapping[9].name = "56250";
    table->values[0].property.mapping[9].val = 56250;
    
    table->values[1].property.name = spn_PORT_INIT_CL72;
    table->values[1].property.doc =
        "\n"
        "Enable / Disable CL72\n"
        "port_init_cl72_#port# = 0 / 1\n"
        "\n"
    ;
    table->values[1].property.method = dnxc_data_property_method_port_enable;
    table->values[1].property.method_str = "port_enable";
    
    table->values[2].property.name = spn_PORT_FEC;
    table->values[2].property.doc =
        "\n"
        "Specifies the FEC type. The types are as follows:\n"
        "0 - no FEC\n"
        "1 - BASE R - 64/66b KR FEC\n"
        "5 - RS 206 - 64/66b 5T RS FEC\n"
        "6 - RS 108 - 64/66b 5T low latency RS FEC\n"
        "7 - RS 545 - 64/66b 15T RS FEC\n"
        "8 - RS 304 - 64/66b 15T low latency RS FEC\n"
        "\n"
    ;
    table->values[2].property.method = dnxc_data_property_method_port_direct_map;
    table->values[2].property.method_str = "port_direct_map";
    table->values[2].property.nof_mapping = 6;
    DNXC_DATA_ALLOC(table->values[2].property.mapping, dnxc_data_property_map_t, table->values[2].property.nof_mapping, "dnxf_data_port_static_add_port_info_t property mapping");

    table->values[2].property.mapping[0].name = "0";
    table->values[2].property.mapping[0].val = bcmPortPhyFecNone;
    table->values[2].property.mapping[1].name = "1";
    table->values[2].property.mapping[1].val = bcmPortPhyFecBaseR;
    table->values[2].property.mapping[2].name = "5";
    table->values[2].property.mapping[2].val = bcmPortPhyFecRs206;
    table->values[2].property.mapping[3].name = "6";
    table->values[2].property.mapping[3].val = bcmPortPhyFecRs108;
    table->values[2].property.mapping[4].name = "7";
    table->values[2].property.mapping[4].val = bcmPortPhyFecRs545;
    table->values[2].property.mapping[5].name = "8";
    table->values[2].property.mapping[5].val = bcmPortPhyFecRs304;
    
    table->values[3].property.name = spn_PHY_TX_POLARITY_FLIP;
    table->values[3].property.doc =
        "\n"
        "Flips PHY TX polarity if enabled\n"
        "phy_tx_polarity_flip_#port# = 0 / 1\n"
        "\n"
    ;
    table->values[3].property.method = dnxc_data_property_method_port_enable;
    table->values[3].property.method_str = "port_enable";
    
    table->values[4].property.name = spn_PHY_RX_POLARITY_FLIP;
    table->values[4].property.doc =
        "\n"
        "Flips PHY RX polarity if enabled\n"
        "phy_rx_polarity_flip_#port# = 0 / 1\n"
        "\n"
    ;
    table->values[4].property.method = dnxc_data_property_method_port_enable;
    table->values[4].property.method_str = "port_enable";
    
    table->values[5].property.name = spn_REPEATER_LINK_ENABLE;
    table->values[5].property.doc =
        "\n"
        "Shows if a certain FE link is connected to a device in reteamer mode\n"
        "\n"
    ;
    table->values[5].property.method = dnxc_data_property_method_port_enable;
    table->values[5].property.method_str = "port_enable";
    
    table->values[6].property.name = spn_PORT_TX_PAM4_PRECODER;
    table->values[6].property.doc =
        "\n"
        "is the precoding enabled on TX side.\n"
        "port_tx_pam4_precoder_#port#=enable/disable\n"
        "\n"
    ;
    table->values[6].property.method = dnxc_data_property_method_port_direct_map;
    table->values[6].property.method_str = "port_direct_map";
    table->values[6].property.nof_mapping = 2;
    DNXC_DATA_ALLOC(table->values[6].property.mapping, dnxc_data_property_map_t, table->values[6].property.nof_mapping, "dnxf_data_port_static_add_port_info_t property mapping");

    table->values[6].property.mapping[0].name = "enable";
    table->values[6].property.mapping[0].val = 1;
    table->values[6].property.mapping[1].name = "disable";
    table->values[6].property.mapping[1].val = 0;
    table->values[6].property.mapping[1].is_default = 1 ;
    
    table->values[7].property.name = spn_PORT_LP_TX_PRECODER;
    table->values[7].property.doc =
        "\n"
        "has the link partner enabled pre-coding on its TX side.\n"
        "in other words - enable the decoding on my RX side.\n"
        "port_lp_tx_precoder_#port#=enable/disable\n"
        "\n"
    ;
    table->values[7].property.method = dnxc_data_property_method_port_direct_map;
    table->values[7].property.method_str = "port_direct_map";
    table->values[7].property.nof_mapping = 2;
    DNXC_DATA_ALLOC(table->values[7].property.mapping, dnxc_data_property_map_t, table->values[7].property.nof_mapping, "dnxf_data_port_static_add_port_info_t property mapping");

    table->values[7].property.mapping[0].name = "enable";
    table->values[7].property.mapping[0].val = 1;
    table->values[7].property.mapping[1].name = "disable";
    table->values[7].property.mapping[1].val = 0;
    table->values[7].property.mapping[1].is_default = 1 ;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));

    for (port_index = 0; port_index < table->keys[0].size; port_index++)
    {
        data = (dnxf_data_port_static_add_port_info_t *) dnxc_data_mgmt_table_data_get(unit, table, port_index, 0);
        SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &table->values[0].property, port_index, &data->speed));
    }
    for (port_index = 0; port_index < table->keys[0].size; port_index++)
    {
        data = (dnxf_data_port_static_add_port_info_t *) dnxc_data_mgmt_table_data_get(unit, table, port_index, 0);
        SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &table->values[1].property, port_index, &data->link_training));
    }
    for (port_index = 0; port_index < table->keys[0].size; port_index++)
    {
        data = (dnxf_data_port_static_add_port_info_t *) dnxc_data_mgmt_table_data_get(unit, table, port_index, 0);
        SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &table->values[2].property, port_index, &data->fec_type));
    }
    for (port_index = 0; port_index < table->keys[0].size; port_index++)
    {
        data = (dnxf_data_port_static_add_port_info_t *) dnxc_data_mgmt_table_data_get(unit, table, port_index, 0);
        SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &table->values[3].property, port_index, &data->tx_polarity));
    }
    for (port_index = 0; port_index < table->keys[0].size; port_index++)
    {
        data = (dnxf_data_port_static_add_port_info_t *) dnxc_data_mgmt_table_data_get(unit, table, port_index, 0);
        SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &table->values[4].property, port_index, &data->rx_polarity));
    }
    for (port_index = 0; port_index < table->keys[0].size; port_index++)
    {
        data = (dnxf_data_port_static_add_port_info_t *) dnxc_data_mgmt_table_data_get(unit, table, port_index, 0);
        SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &table->values[5].property, port_index, &data->connected_to_repeater));
    }
    for (port_index = 0; port_index < table->keys[0].size; port_index++)
    {
        data = (dnxf_data_port_static_add_port_info_t *) dnxc_data_mgmt_table_data_get(unit, table, port_index, 0);
        SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &table->values[6].property, port_index, &data->tx_pam4_precoder));
    }
    for (port_index = 0; port_index < table->keys[0].size; port_index++)
    {
        data = (dnxf_data_port_static_add_port_info_t *) dnxc_data_mgmt_table_data_get(unit, table, port_index, 0);
        SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &table->values[7].property, port_index, &data->lp_tx_precoder));
    }
    
    table->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
ramon_a0_dnxf_data_port_static_add_serdes_lane_config_set(
    int unit)
{
    int port_index;
    dnxf_data_port_static_add_serdes_lane_config_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnxf_data_module_port;
    int submodule_index = dnxf_data_port_submodule_static_add;
    int table_index = dnxf_data_port_static_add_table_serdes_lane_config;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->flags |= DNXC_DATA_F_INIT_ONLY;

    
    table->keys[0].size = dnxf_data_port.general.nof_links_get(unit);
    table->info_get.key_size[0] = dnxf_data_port.general.nof_links_get(unit);

    
    table->values[0].default_val = "DNXC_PORT_SERDES_LANE_CONFIG_INVALID_VAL";
    table->values[1].default_val = "DNXC_PORT_SERDES_LANE_CONFIG_INVALID_VAL";
    table->values[2].default_val = "DNXC_PORT_SERDES_LANE_CONFIG_INVALID_VAL";
    table->values[3].default_val = "DNXC_PORT_SERDES_LANE_CONFIG_INVALID_VAL";
    table->values[4].default_val = "DNXC_PORT_SERDES_LANE_CONFIG_INVALID_VAL";
    table->values[5].default_val = "DNXC_PORT_SERDES_LANE_CONFIG_INVALID_VAL";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnxf_data_port_static_add_serdes_lane_config_t, (1 * (table->keys[0].size) + 1  ), "data of dnxf_data_port_static_add_table_serdes_lane_config");

    
    default_data = (dnxf_data_port_static_add_serdes_lane_config_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->dfe = DNXC_PORT_SERDES_LANE_CONFIG_INVALID_VAL;
    default_data->media_type = DNXC_PORT_SERDES_LANE_CONFIG_INVALID_VAL;
    default_data->unreliable_los = DNXC_PORT_SERDES_LANE_CONFIG_INVALID_VAL;
    default_data->cl72_auto_polarity_enable = DNXC_PORT_SERDES_LANE_CONFIG_INVALID_VAL;
    default_data->cl72_restart_timeout_enable = DNXC_PORT_SERDES_LANE_CONFIG_INVALID_VAL;
    default_data->channel_mode = DNXC_PORT_SERDES_LANE_CONFIG_INVALID_VAL;
    
    for (port_index = 0; port_index < table->keys[0].size; port_index++)
    {
        data = (dnxf_data_port_static_add_serdes_lane_config_t *) dnxc_data_mgmt_table_data_get(unit, table, port_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    table->values[0].property.name = spn_SERDES_LANE_CONFIG;
    table->values[0].property.doc =
        "\n"
        "DFE filter\n"
        "serdes_lane_config_dfe_#port#=on|off|lp\n"
        "on - dfe is on and lp dfe is off\n"
        "off - both dfe and lp dfe are off\n"
        "lp - both dfe and lp dfe are on\n"
        "\n"
    ;
    table->values[0].property.method = dnxc_data_property_method_port_suffix_direct_map;
    table->values[0].property.method_str = "port_suffix_direct_map";
    table->values[0].property.suffix = "dfe";
    table->values[0].property.nof_mapping = 3;
    DNXC_DATA_ALLOC(table->values[0].property.mapping, dnxc_data_property_map_t, table->values[0].property.nof_mapping, "dnxf_data_port_static_add_serdes_lane_config_t property mapping");

    table->values[0].property.mapping[0].name = "on";
    table->values[0].property.mapping[0].val = soc_dnxc_port_dfe_on;
    table->values[0].property.mapping[1].name = "off";
    table->values[0].property.mapping[1].val = soc_dnxc_port_dfe_off;
    table->values[0].property.mapping[2].name = "lp";
    table->values[0].property.mapping[2].val = soc_dnxc_port_lp_dfe;
    
    table->values[1].property.name = spn_SERDES_LANE_CONFIG;
    table->values[1].property.doc =
        "\n"
        "media type\n"
        "serdes_lane_config_media_type_#port#=backplane|copper|optics\n"
        "\n"
    ;
    table->values[1].property.method = dnxc_data_property_method_port_suffix_direct_map;
    table->values[1].property.method_str = "port_suffix_direct_map";
    table->values[1].property.suffix = "media_type";
    table->values[1].property.nof_mapping = 3;
    DNXC_DATA_ALLOC(table->values[1].property.mapping, dnxc_data_property_map_t, table->values[1].property.nof_mapping, "dnxf_data_port_static_add_serdes_lane_config_t property mapping");

    table->values[1].property.mapping[0].name = "backplane";
    table->values[1].property.mapping[0].val = BCM_PORT_RESOURCE_PHY_LANE_CONFIG_MEDIUM_BACKPLANE;
    table->values[1].property.mapping[1].name = "copper";
    table->values[1].property.mapping[1].val = BCM_PORT_RESOURCE_PHY_LANE_CONFIG_MEDIUM_COPPER_CABLE;
    table->values[1].property.mapping[2].name = "optics";
    table->values[1].property.mapping[2].val = BCM_PORT_RESOURCE_PHY_LANE_CONFIG_MEDIUM_OPTICS;
    
    table->values[2].property.name = spn_SERDES_LANE_CONFIG;
    table->values[2].property.doc =
        "\n"
        "unreliable los\n"
        "serdes_lane_config_unreliable_los_#port#=0|1\n"
        "\n"
    ;
    table->values[2].property.method = dnxc_data_property_method_port_suffix_enable;
    table->values[2].property.method_str = "port_suffix_enable";
    table->values[2].property.suffix = "unreliable_los";
    
    table->values[3].property.name = spn_SERDES_LANE_CONFIG;
    table->values[3].property.doc =
        "\n"
        "enable cl72 auto polarity\n"
        "serdes_lane_config_cl72_auto_polarity_en_#port#=0|1\n"
        "\n"
    ;
    table->values[3].property.method = dnxc_data_property_method_port_suffix_enable;
    table->values[3].property.method_str = "port_suffix_enable";
    table->values[3].property.suffix = "cl72_auto_polarity_en";
    
    table->values[4].property.name = spn_SERDES_LANE_CONFIG;
    table->values[4].property.doc =
        "\n"
        "enable cl72 restart timeout\n"
        "serdes_lane_config_cl72_restart_timeout_en_#port#=0|1\n"
        "\n"
    ;
    table->values[4].property.method = dnxc_data_property_method_port_suffix_enable;
    table->values[4].property.method_str = "port_suffix_enable";
    table->values[4].property.suffix = "cl72_restart_timeout_en";
    
    table->values[5].property.name = spn_SERDES_LANE_CONFIG;
    table->values[5].property.doc =
        "\n"
        "channel mode\n"
        "serdes_lane_config_channel_mode_#port#=force_nr|force_er\n"
        "\n"
    ;
    table->values[5].property.method = dnxc_data_property_method_port_suffix_direct_map;
    table->values[5].property.method_str = "port_suffix_direct_map";
    table->values[5].property.suffix = "channel_mode";
    table->values[5].property.nof_mapping = 2;
    DNXC_DATA_ALLOC(table->values[5].property.mapping, dnxc_data_property_map_t, table->values[5].property.nof_mapping, "dnxf_data_port_static_add_serdes_lane_config_t property mapping");

    table->values[5].property.mapping[0].name = "force_nr";
    table->values[5].property.mapping[0].val = soc_dnxc_port_force_nr;
    table->values[5].property.mapping[1].name = "force_er";
    table->values[5].property.mapping[1].val = soc_dnxc_port_force_er;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));

    for (port_index = 0; port_index < table->keys[0].size; port_index++)
    {
        data = (dnxf_data_port_static_add_serdes_lane_config_t *) dnxc_data_mgmt_table_data_get(unit, table, port_index, 0);
        SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &table->values[0].property, port_index, &data->dfe));
    }
    for (port_index = 0; port_index < table->keys[0].size; port_index++)
    {
        data = (dnxf_data_port_static_add_serdes_lane_config_t *) dnxc_data_mgmt_table_data_get(unit, table, port_index, 0);
        SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &table->values[1].property, port_index, &data->media_type));
    }
    for (port_index = 0; port_index < table->keys[0].size; port_index++)
    {
        data = (dnxf_data_port_static_add_serdes_lane_config_t *) dnxc_data_mgmt_table_data_get(unit, table, port_index, 0);
        SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &table->values[2].property, port_index, &data->unreliable_los));
    }
    for (port_index = 0; port_index < table->keys[0].size; port_index++)
    {
        data = (dnxf_data_port_static_add_serdes_lane_config_t *) dnxc_data_mgmt_table_data_get(unit, table, port_index, 0);
        SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &table->values[3].property, port_index, &data->cl72_auto_polarity_enable));
    }
    for (port_index = 0; port_index < table->keys[0].size; port_index++)
    {
        data = (dnxf_data_port_static_add_serdes_lane_config_t *) dnxc_data_mgmt_table_data_get(unit, table, port_index, 0);
        SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &table->values[4].property, port_index, &data->cl72_restart_timeout_enable));
    }
    for (port_index = 0; port_index < table->keys[0].size; port_index++)
    {
        data = (dnxf_data_port_static_add_serdes_lane_config_t *) dnxc_data_mgmt_table_data_get(unit, table, port_index, 0);
        SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &table->values[5].property, port_index, &data->channel_mode));
    }
    
    table->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
ramon_a0_dnxf_data_port_static_add_serdes_tx_taps_set(
    int unit)
{
    int port_index;
    dnxf_data_port_static_add_serdes_tx_taps_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnxf_data_module_port;
    int submodule_index = dnxf_data_port_submodule_static_add;
    int table_index = dnxf_data_port_static_add_table_serdes_tx_taps;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->flags |= DNXC_DATA_F_INIT_ONLY;

    
    table->keys[0].size = dnxf_data_port.general.nof_links_get(unit);
    table->info_get.key_size[0] = dnxf_data_port.general.nof_links_get(unit);

    
    table->values[0].default_val = "0";
    table->values[1].default_val = "0";
    table->values[2].default_val = "DNXC_PORT_TX_FIR_INVALID_MAIN_TAP";
    table->values[3].default_val = "0";
    table->values[4].default_val = "0";
    table->values[5].default_val = "0";
    table->values[6].default_val = "bcmPortPhyTxTapMode3Tap";
    table->values[7].default_val = "bcmPortPhySignallingModeNRZ";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnxf_data_port_static_add_serdes_tx_taps_t, (1 * (table->keys[0].size) + 1  ), "data of dnxf_data_port_static_add_table_serdes_tx_taps");

    
    default_data = (dnxf_data_port_static_add_serdes_tx_taps_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->pre2 = 0;
    default_data->pre = 0;
    default_data->main = DNXC_PORT_TX_FIR_INVALID_MAIN_TAP;
    default_data->post = 0;
    default_data->post2 = 0;
    default_data->post3 = 0;
    default_data->tx_tap_mode = bcmPortPhyTxTapMode3Tap;
    default_data->signalling_mode = bcmPortPhySignallingModeNRZ;
    
    for (port_index = 0; port_index < table->keys[0].size; port_index++)
    {
        data = (dnxf_data_port_static_add_serdes_tx_taps_t *) dnxc_data_mgmt_table_data_get(unit, table, port_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    table->property.name = "serdes_tx_taps";
    table->property.doc =
        "\n"
        "a set of TX FIR parameters for the port.\n"
        "serdes_tx_taps_#port#=signalling_mode:pre:main:post:pre2:post2:post3\n"
        "signalling_mode can be either pam4 or nrz.\n"
        "[pre2:post2:post3] part is optional:\n"
        "- if it is given - this it 6-tap mode.\n"
        "- if it is not given - this is 3-tap mode.\n"
        "main is set to DNXC_PORT_TX_FIR_INVALID_MAIN_TAP by default to distinguish between whether the SoC property was set or not.\n"
        "\n"
    ;
    table->property.method = dnxc_data_property_method_custom;
    table->property.method_str = "custom";

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));

    for (port_index = 0; port_index < table->keys[0].size; port_index++)
    {
        data = (dnxf_data_port_static_add_serdes_tx_taps_t *) dnxc_data_mgmt_table_data_get(unit, table, port_index, 0);
        SHR_IF_ERR_EXIT(dnxf_data_property_port_static_add_serdes_tx_taps_read(unit, port_index, data));
    }
    
    table->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
ramon_a0_dnxf_data_port_static_add_quad_info_set(
    int unit)
{
    int quad_index;
    dnxf_data_port_static_add_quad_info_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnxf_data_module_port;
    int submodule_index = dnxf_data_port_submodule_static_add;
    int table_index = dnxf_data_port_static_add_table_quad_info;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = dnxf_data_port.general.nof_links_get(unit) / dnxf_data_device.blocks.nof_links_in_fmac_get(unit);
    table->info_get.key_size[0] = dnxf_data_port.general.nof_links_get(unit) / dnxf_data_device.blocks.nof_links_in_fmac_get(unit);

    
    table->values[0].default_val = "1";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnxf_data_port_static_add_quad_info_t, (1 * (table->keys[0].size) + 1  ), "data of dnxf_data_port_static_add_table_quad_info");

    
    default_data = (dnxf_data_port_static_add_quad_info_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->quad_enable = 1;
    
    for (quad_index = 0; quad_index < table->keys[0].size; quad_index++)
    {
        data = (dnxf_data_port_static_add_quad_info_t *) dnxc_data_mgmt_table_data_get(unit, table, quad_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    table->values[0].property.name = spn_SERDES_QRTT_ACTIVE;
    table->values[0].property.doc =
        "\n"
        "Enable / Disable Quad\n"
        "\n"
    ;
    table->values[0].property.method = dnxc_data_property_method_suffix_enable;
    table->values[0].property.method_str = "suffix_enable";
    table->values[0].property.suffix = "";

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));

    for (quad_index = 0; quad_index < table->keys[0].size; quad_index++)
    {
        data = (dnxf_data_port_static_add_quad_info_t *) dnxc_data_mgmt_table_data_get(unit, table, quad_index, 0);
        SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &table->values[0].property, quad_index, &data->quad_enable));
    }
    
    table->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}






static shr_error_e
ramon_a0_dnxf_data_port_lane_map_lane_map_bound_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnxf_data_module_port;
    int submodule_index = dnxf_data_port_submodule_lane_map;
    int define_index = dnxf_data_port_lane_map_define_lane_map_bound;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 4;

    
    define->data = 4;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}



static shr_error_e
ramon_a0_dnxf_data_port_lane_map_info_set(
    int unit)
{
    int lane_index;
    dnxf_data_port_lane_map_info_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnxf_data_module_port;
    int submodule_index = dnxf_data_port_submodule_lane_map;
    int table_index = dnxf_data_port_lane_map_table_info;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->flags |= DNXC_DATA_F_INIT_ONLY;

    
    table->keys[0].size = dnxf_data_port.general.nof_links_get(unit);
    table->info_get.key_size[0] = dnxf_data_port.general.nof_links_get(unit);

    
    table->values[0].default_val = "SOC_DNXF_LANE_MAP_DB_LANE_TO_SERDES_NOT_MAPPED";
    table->values[1].default_val = "SOC_DNXF_LANE_MAP_DB_LANE_TO_SERDES_NOT_MAPPED";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnxf_data_port_lane_map_info_t, (1 * (table->keys[0].size) + 1  ), "data of dnxf_data_port_lane_map_table_info");

    
    default_data = (dnxf_data_port_lane_map_info_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->serdes_rx_id = SOC_DNXF_LANE_MAP_DB_LANE_TO_SERDES_NOT_MAPPED;
    default_data->serdes_tx_id = SOC_DNXF_LANE_MAP_DB_LANE_TO_SERDES_NOT_MAPPED;
    
    for (lane_index = 0; lane_index < table->keys[0].size; lane_index++)
    {
        data = (dnxf_data_port_lane_map_info_t *) dnxc_data_mgmt_table_data_get(unit, table, lane_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    table->property.name = "lane_to_serdes_map_fabric_lane";
    table->property.doc =
        "\n"
        "Shows the DB for the mapping between the lanes and the serdeses.\n"
        "\n"
    ;
    table->property.method = dnxc_data_property_method_custom;
    table->property.method_str = "custom";

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));

    for (lane_index = 0; lane_index < table->keys[0].size; lane_index++)
    {
        data = (dnxf_data_port_lane_map_info_t *) dnxc_data_mgmt_table_data_get(unit, table, lane_index, 0);
        SHR_IF_ERR_EXIT(dnxf_data_property_port_lane_map_info_read(unit, lane_index, data));
    }
    
    table->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}






static shr_error_e
ramon_a0_dnxf_data_port_pll_nof_lcpll_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnxf_data_module_port;
    int submodule_index = dnxf_data_port_submodule_pll;
    int define_index = dnxf_data_port_pll_define_nof_lcpll;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 4;

    
    define->data = 4;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
ramon_a0_dnxf_data_port_pll_nof_port_in_lcpll_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnxf_data_module_port;
    int submodule_index = dnxf_data_port_submodule_pll;
    int define_index = dnxf_data_port_pll_define_nof_port_in_lcpll;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 48;

    
    define->data = 48;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
ramon_a0_dnxf_data_port_pll_nof_plls_per_pm_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnxf_data_module_port;
    int submodule_index = dnxf_data_port_submodule_pll;
    int define_index = dnxf_data_port_pll_define_nof_plls_per_pm;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 2;

    
    define->data = 2;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
ramon_a0_dnxf_data_port_pll_nof_config_words_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnxf_data_module_port;
    int submodule_index = dnxf_data_port_submodule_pll;
    int define_index = dnxf_data_port_pll_define_nof_config_words;
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
ramon_a0_dnxf_data_port_pll_info_set(
    int unit)
{
    int pll_index;
    dnxf_data_port_pll_info_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnxf_data_module_port;
    int submodule_index = dnxf_data_port_submodule_pll;
    int table_index = dnxf_data_port_pll_table_info;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = dnxf_data_port.pll.nof_lcpll_get(unit);
    table->info_get.key_size[0] = dnxf_data_port.pll.nof_lcpll_get(unit);

    
    table->values[0].default_val = "soc_dnxc_init_serdes_ref_clock_312_5";
    table->values[1].default_val = "soc_dnxc_init_serdes_ref_clock_bypass";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnxf_data_port_pll_info_t, (1 * (table->keys[0].size) + 1  ), "data of dnxf_data_port_pll_table_info");

    
    default_data = (dnxf_data_port_pll_info_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->fabric_ref_clk_in = soc_dnxc_init_serdes_ref_clock_312_5;
    default_data->fabric_ref_clk_out = soc_dnxc_init_serdes_ref_clock_bypass;
    
    for (pll_index = 0; pll_index < table->keys[0].size; pll_index++)
    {
        data = (dnxf_data_port_pll_info_t *) dnxc_data_mgmt_table_data_get(unit, table, pll_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    table->values[0].property.name = spn_SERDES_FABRIC_CLK_FREQ;
    table->values[0].property.doc =
        "\n"
        "Configure Input PLL ref clocks per 48-link group.\n"
        "Valid values are 1=156.25Mhz, 3=312.5 MHz.\n"
        "\n"
    ;
    table->values[0].property.method = dnxc_data_property_method_suffix_direct_map;
    table->values[0].property.method_str = "suffix_direct_map";
    table->values[0].property.suffix = "in";
    table->values[0].property.nof_mapping = 2;
    DNXC_DATA_ALLOC(table->values[0].property.mapping, dnxc_data_property_map_t, table->values[0].property.nof_mapping, "dnxf_data_port_pll_info_t property mapping");

    table->values[0].property.mapping[0].name = "1";
    table->values[0].property.mapping[0].val = soc_dnxc_init_serdes_ref_clock_156_25;
    table->values[0].property.mapping[1].name = "3";
    table->values[0].property.mapping[1].val = soc_dnxc_init_serdes_ref_clock_312_5;
    table->values[0].property.mapping[1].is_default = 1 ;
    
    table->values[1].property.name = spn_SERDES_FABRIC_CLK_FREQ;
    table->values[1].property.doc =
        "\n"
        "Configure Output PLL ref clocks per 48-link group.\n"
        "Valid values are 1=156.25Mhz, 3=312.5 MHz or 'bypass'=PLL bypassed(input ref clock == output ref clock)\n"
        "\n"
    ;
    table->values[1].property.method = dnxc_data_property_method_suffix_direct_map;
    table->values[1].property.method_str = "suffix_direct_map";
    table->values[1].property.suffix = "out";
    table->values[1].property.nof_mapping = 3;
    DNXC_DATA_ALLOC(table->values[1].property.mapping, dnxc_data_property_map_t, table->values[1].property.nof_mapping, "dnxf_data_port_pll_info_t property mapping");

    table->values[1].property.mapping[0].name = "1";
    table->values[1].property.mapping[0].val = soc_dnxc_init_serdes_ref_clock_156_25;
    table->values[1].property.mapping[1].name = "3";
    table->values[1].property.mapping[1].val = soc_dnxc_init_serdes_ref_clock_312_5;
    table->values[1].property.mapping[2].name = "bypass";
    table->values[1].property.mapping[2].val = soc_dnxc_init_serdes_ref_clock_bypass;
    table->values[1].property.mapping[2].is_default = 1 ;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));

    for (pll_index = 0; pll_index < table->keys[0].size; pll_index++)
    {
        data = (dnxf_data_port_pll_info_t *) dnxc_data_mgmt_table_data_get(unit, table, pll_index, 0);
        SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &table->values[0].property, pll_index, &data->fabric_ref_clk_in));
    }
    for (pll_index = 0; pll_index < table->keys[0].size; pll_index++)
    {
        data = (dnxf_data_port_pll_info_t *) dnxc_data_mgmt_table_data_get(unit, table, pll_index, 0);
        SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &table->values[1].property, pll_index, &data->fabric_ref_clk_out));
    }
    
    table->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
ramon_a0_dnxf_data_port_pll_config_set(
    int unit)
{
    int fabric_ref_clk_in_index;
    int fabric_ref_clk_out_index;
    dnxf_data_port_pll_config_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnxf_data_module_port;
    int submodule_index = dnxf_data_port_submodule_pll;
    int table_index = dnxf_data_port_pll_table_config;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = soc_dnxc_init_serdes_nof_ref_clocks;
    table->info_get.key_size[0] = soc_dnxc_init_serdes_nof_ref_clocks;
    table->keys[1].size = soc_dnxc_init_serdes_nof_ref_clocks;
    table->info_get.key_size[1] = soc_dnxc_init_serdes_nof_ref_clocks;

    
    table->values[0].default_val = "0,0,0,0,0,0,0,0";
    table->values[1].default_val = "0";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }
    if (table->keys[1].size == 0 || table->info_get.key_size[1] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnxf_data_port_pll_config_t, (1 * (table->keys[0].size) * (table->keys[1].size) + 1  ), "data of dnxf_data_port_pll_table_config");

    
    default_data = (dnxf_data_port_pll_config_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    SHR_RANGE_VERIFY(8, 0, DNXF_DATA_MAX_PORT_PLL_NOF_CONFIG_WORDS, _SHR_E_INTERNAL, "out of bound access to array")
    default_data->data[0] = 0;
    default_data->data[1] = 0;
    default_data->data[2] = 0;
    default_data->data[3] = 0;
    default_data->data[4] = 0;
    default_data->data[5] = 0;
    default_data->data[6] = 0;
    default_data->data[7] = 0;
    default_data->valid = 0;
    
    for (fabric_ref_clk_in_index = 0; fabric_ref_clk_in_index < table->keys[0].size; fabric_ref_clk_in_index++)
    {
        for (fabric_ref_clk_out_index = 0; fabric_ref_clk_out_index < table->keys[1].size; fabric_ref_clk_out_index++)
        {
            data = (dnxf_data_port_pll_config_t *) dnxc_data_mgmt_table_data_get(unit, table, fabric_ref_clk_in_index, fabric_ref_clk_out_index);
            sal_memcpy(data, default_data, table->size_of_values);
        }
    }
    
    if (soc_dnxc_init_serdes_ref_clock_125 < table->keys[0].size && soc_dnxc_init_serdes_ref_clock_156_25 < table->keys[1].size)
    {
        data = (dnxf_data_port_pll_config_t *) dnxc_data_mgmt_table_data_get(unit, table, soc_dnxc_init_serdes_ref_clock_125, soc_dnxc_init_serdes_ref_clock_156_25);
        SHR_RANGE_VERIFY(8, 0, DNXF_DATA_MAX_PORT_PLL_NOF_CONFIG_WORDS, _SHR_E_INTERNAL, "out of bound access to array")
        data->data[0] = 0x00000000;
        data->data[1] = 0x28000000;
        data->data[2] = 0x01c14000;
        data->data[3] = 0x00001428;
        data->data[4] = 0x19000000;
        data->data[5] = 0xa1803e80;
        data->data[6] = 0x64000000;
        data->data[7] = 0x00000020;
        data->valid = 1;
    }
    if (soc_dnxc_init_serdes_ref_clock_125 < table->keys[0].size && soc_dnxc_init_serdes_ref_clock_312_5 < table->keys[1].size)
    {
        data = (dnxf_data_port_pll_config_t *) dnxc_data_mgmt_table_data_get(unit, table, soc_dnxc_init_serdes_ref_clock_125, soc_dnxc_init_serdes_ref_clock_312_5);
        SHR_RANGE_VERIFY(8, 0, DNXF_DATA_MAX_PORT_PLL_NOF_CONFIG_WORDS, _SHR_E_INTERNAL, "out of bound access to array")
        data->data[0] = 0x00000000;
        data->data[1] = 0x14000000;
        data->data[2] = 0x01c14000;
        data->data[3] = 0x00001428;
        data->data[4] = 0x19000000;
        data->data[5] = 0xa1803e80;
        data->data[6] = 0x64000000;
        data->data[7] = 0x00000020;
        data->valid = 1;
    }
    if (soc_dnxc_init_serdes_ref_clock_156_25 < table->keys[0].size && soc_dnxc_init_serdes_ref_clock_156_25 < table->keys[1].size)
    {
        data = (dnxf_data_port_pll_config_t *) dnxc_data_mgmt_table_data_get(unit, table, soc_dnxc_init_serdes_ref_clock_156_25, soc_dnxc_init_serdes_ref_clock_156_25);
        SHR_RANGE_VERIFY(8, 0, DNXF_DATA_MAX_PORT_PLL_NOF_CONFIG_WORDS, _SHR_E_INTERNAL, "out of bound access to array")
        data->data[0] = 0x00000000;
        data->data[1] = 0x28000000;
        data->data[2] = 0x01c14000;
        data->data[3] = 0x00001428;
        data->data[4] = 0x19000000;
        data->data[5] = 0xa1804e00;
        data->data[6] = 0x50000000;
        data->data[7] = 0x00000020;
        data->valid = 1;
    }
    if (soc_dnxc_init_serdes_ref_clock_156_25 < table->keys[0].size && soc_dnxc_init_serdes_ref_clock_312_5 < table->keys[1].size)
    {
        data = (dnxf_data_port_pll_config_t *) dnxc_data_mgmt_table_data_get(unit, table, soc_dnxc_init_serdes_ref_clock_156_25, soc_dnxc_init_serdes_ref_clock_312_5);
        SHR_RANGE_VERIFY(8, 0, DNXF_DATA_MAX_PORT_PLL_NOF_CONFIG_WORDS, _SHR_E_INTERNAL, "out of bound access to array")
        data->data[0] = 0x00000000;
        data->data[1] = 0x14000000;
        data->data[2] = 0x01c14000;
        data->data[3] = 0x00001428;
        data->data[4] = 0x19000000;
        data->data[5] = 0xa1804e00;
        data->data[6] = 0x50000000;
        data->data[7] = 0x00000020;
        data->valid = 1;
    }
    if (soc_dnxc_init_serdes_ref_clock_156_25 < table->keys[0].size && soc_dnxc_init_serdes_ref_clock_bypass < table->keys[1].size)
    {
        data = (dnxf_data_port_pll_config_t *) dnxc_data_mgmt_table_data_get(unit, table, soc_dnxc_init_serdes_ref_clock_156_25, soc_dnxc_init_serdes_ref_clock_bypass);
        SHR_RANGE_VERIFY(8, 0, DNXF_DATA_MAX_PORT_PLL_NOF_CONFIG_WORDS, _SHR_E_INTERNAL, "out of bound access to array")
        data->data[0] = 0x00000000;
        data->data[1] = 0x14000000;
        data->data[2] = 0x01c14000;
        data->data[3] = 0x00001428;
        data->data[4] = 0x59000000;
        data->data[5] = 0xa1804e00;
        data->data[6] = 0x50000000;
        data->data[7] = 0x00780020;
        data->valid = 1;
    }
    if (soc_dnxc_init_serdes_ref_clock_312_5 < table->keys[0].size && soc_dnxc_init_serdes_ref_clock_156_25 < table->keys[1].size)
    {
        data = (dnxf_data_port_pll_config_t *) dnxc_data_mgmt_table_data_get(unit, table, soc_dnxc_init_serdes_ref_clock_312_5, soc_dnxc_init_serdes_ref_clock_156_25);
        SHR_RANGE_VERIFY(8, 0, DNXF_DATA_MAX_PORT_PLL_NOF_CONFIG_WORDS, _SHR_E_INTERNAL, "out of bound access to array")
        data->data[0] = 0x00000000;
        data->data[1] = 0x28000000;
        data->data[2] = 0x01c14000;
        data->data[3] = 0x00001428;
        data->data[4] = 0x19000000;
        data->data[5] = 0xa1804e00;
        data->data[6] = 0x50000000;
        data->data[7] = 0x00000040;
        data->valid = 1;
    }
    if (soc_dnxc_init_serdes_ref_clock_312_5 < table->keys[0].size && soc_dnxc_init_serdes_ref_clock_312_5 < table->keys[1].size)
    {
        data = (dnxf_data_port_pll_config_t *) dnxc_data_mgmt_table_data_get(unit, table, soc_dnxc_init_serdes_ref_clock_312_5, soc_dnxc_init_serdes_ref_clock_312_5);
        SHR_RANGE_VERIFY(8, 0, DNXF_DATA_MAX_PORT_PLL_NOF_CONFIG_WORDS, _SHR_E_INTERNAL, "out of bound access to array")
        data->data[0] = 0x00000000;
        data->data[1] = 0x14000000;
        data->data[2] = 0x01c14000;
        data->data[3] = 0x00001428;
        data->data[4] = 0x19000000;
        data->data[5] = 0xa1804e00;
        data->data[6] = 0x50000000;
        data->data[7] = 0x00000040;
        data->valid = 1;
    }
    if (soc_dnxc_init_serdes_ref_clock_312_5 < table->keys[0].size && soc_dnxc_init_serdes_ref_clock_bypass < table->keys[1].size)
    {
        data = (dnxf_data_port_pll_config_t *) dnxc_data_mgmt_table_data_get(unit, table, soc_dnxc_init_serdes_ref_clock_312_5, soc_dnxc_init_serdes_ref_clock_bypass);
        SHR_RANGE_VERIFY(8, 0, DNXF_DATA_MAX_PORT_PLL_NOF_CONFIG_WORDS, _SHR_E_INTERNAL, "out of bound access to array")
        data->data[0] = 0x00000000;
        data->data[1] = 0x14000000;
        data->data[2] = 0x01c14000;
        data->data[3] = 0x00001428;
        data->data[4] = 0x59000000;
        data->data[5] = 0xa1804e00;
        data->data[6] = 0x50000000;
        data->data[7] = 0x00780040;
        data->valid = 1;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}







static shr_error_e
ramon_a0_dnxf_data_port_synce_cfg_set(
    int unit)
{
    int synce_index_index;
    dnxf_data_port_synce_cfg_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnxf_data_module_port;
    int submodule_index = dnxf_data_port_submodule_synce;
    int table_index = dnxf_data_port_synce_table_cfg;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = 2;
    table->info_get.key_size[0] = 2;

    
    table->values[0].default_val = "SOC_DNXF_PROPERTY_UNAVAIL";
    table->values[1].default_val = "16";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnxf_data_port_synce_cfg_t, (1 * (table->keys[0].size) + 1  ), "data of dnxf_data_port_synce_table_cfg");

    
    default_data = (dnxf_data_port_synce_cfg_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->source_clock_port = SOC_DNXF_PROPERTY_UNAVAIL;
    default_data->source_clock_divider = 16;
    
    for (synce_index_index = 0; synce_index_index < table->keys[0].size; synce_index_index++)
    {
        data = (dnxf_data_port_synce_cfg_t *) dnxc_data_mgmt_table_data_get(unit, table, synce_index_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    table->values[0].property.name = spn_SYNC_ETH_CLK_TO_PORT_ID_CLK;
    table->values[0].property.doc =
        "\n"
        "Specifies which is the source clock port\n"
        "\n"
    ;
    table->values[0].property.method = dnxc_data_property_method_suffix_range_signed;
    table->values[0].property.method_str = "suffix_range_signed";
    table->values[0].property.suffix = "";
    table->values[0].property.range_min = 0;
    table->values[0].property.range_max = 192;
    
    table->values[1].property.name = spn_SYNC_ETH_CLK_DIVIDER;
    table->values[1].property.doc =
        "\n"
        "Specifiers the clock divider\n"
        "\n"
    ;
    table->values[1].property.method = dnxc_data_property_method_suffix_range;
    table->values[1].property.method_str = "suffix_range";
    table->values[1].property.suffix = "";
    table->values[1].property.range_min = 2;
    table->values[1].property.range_max = 16;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));

    for (synce_index_index = 0; synce_index_index < table->keys[0].size; synce_index_index++)
    {
        data = (dnxf_data_port_synce_cfg_t *) dnxc_data_mgmt_table_data_get(unit, table, synce_index_index, 0);
        SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &table->values[0].property, synce_index_index, &data->source_clock_port));
    }
    for (synce_index_index = 0; synce_index_index < table->keys[0].size; synce_index_index++)
    {
        data = (dnxf_data_port_synce_cfg_t *) dnxc_data_mgmt_table_data_get(unit, table, synce_index_index, 0);
        SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &table->values[1].property, synce_index_index, &data->source_clock_divider));
    }
    
    table->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}





static shr_error_e
ramon_a0_dnxf_data_port_retimer_is_supported_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnxf_data_module_port;
    int submodule_index = dnxf_data_port_submodule_retimer;
    int feature_index = dnxf_data_port_retimer_is_supported;
    SHR_FUNC_INIT_VARS(unit);

    feature = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].features[feature_index];
    
    feature->default_data = 1;

    
    feature->data = 1;

    
    feature->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_feature_value_set(unit, feature));

exit:
    SHR_FUNC_EXIT;
}




static shr_error_e
ramon_a0_dnxf_data_port_retimer_links_connection_set(
    int unit)
{
    int link_index;
    dnxf_data_port_retimer_links_connection_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnxf_data_module_port;
    int submodule_index = dnxf_data_port_submodule_retimer;
    int table_index = dnxf_data_port_retimer_table_links_connection;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = dnxf_data_port.general.nof_links_get(unit);
    table->info_get.key_size[0] = dnxf_data_port.general.nof_links_get(unit);

    
    table->values[0].default_val = "-1";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnxf_data_port_retimer_links_connection_t, (1 * (table->keys[0].size) + 1  ), "data of dnxf_data_port_retimer_table_links_connection");

    
    default_data = (dnxf_data_port_retimer_links_connection_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->connected_link = -1;
    
    for (link_index = 0; link_index < table->keys[0].size; link_index++)
    {
        data = (dnxf_data_port_retimer_links_connection_t *) dnxc_data_mgmt_table_data_get(unit, table, link_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (0 < table->keys[0].size)
    {
        data = (dnxf_data_port_retimer_links_connection_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
        data->connected_link = 96;
    }
    if (1 < table->keys[0].size)
    {
        data = (dnxf_data_port_retimer_links_connection_t *) dnxc_data_mgmt_table_data_get(unit, table, 1, 0);
        data->connected_link = 97;
    }
    if (2 < table->keys[0].size)
    {
        data = (dnxf_data_port_retimer_links_connection_t *) dnxc_data_mgmt_table_data_get(unit, table, 2, 0);
        data->connected_link = 98;
    }
    if (3 < table->keys[0].size)
    {
        data = (dnxf_data_port_retimer_links_connection_t *) dnxc_data_mgmt_table_data_get(unit, table, 3, 0);
        data->connected_link = 99;
    }
    if (4 < table->keys[0].size)
    {
        data = (dnxf_data_port_retimer_links_connection_t *) dnxc_data_mgmt_table_data_get(unit, table, 4, 0);
        data->connected_link = 100;
    }
    if (5 < table->keys[0].size)
    {
        data = (dnxf_data_port_retimer_links_connection_t *) dnxc_data_mgmt_table_data_get(unit, table, 5, 0);
        data->connected_link = 101;
    }
    if (6 < table->keys[0].size)
    {
        data = (dnxf_data_port_retimer_links_connection_t *) dnxc_data_mgmt_table_data_get(unit, table, 6, 0);
        data->connected_link = 102;
    }
    if (7 < table->keys[0].size)
    {
        data = (dnxf_data_port_retimer_links_connection_t *) dnxc_data_mgmt_table_data_get(unit, table, 7, 0);
        data->connected_link = 103;
    }
    if (8 < table->keys[0].size)
    {
        data = (dnxf_data_port_retimer_links_connection_t *) dnxc_data_mgmt_table_data_get(unit, table, 8, 0);
        data->connected_link = 152;
    }
    if (9 < table->keys[0].size)
    {
        data = (dnxf_data_port_retimer_links_connection_t *) dnxc_data_mgmt_table_data_get(unit, table, 9, 0);
        data->connected_link = 153;
    }
    if (10 < table->keys[0].size)
    {
        data = (dnxf_data_port_retimer_links_connection_t *) dnxc_data_mgmt_table_data_get(unit, table, 10, 0);
        data->connected_link = 154;
    }
    if (11 < table->keys[0].size)
    {
        data = (dnxf_data_port_retimer_links_connection_t *) dnxc_data_mgmt_table_data_get(unit, table, 11, 0);
        data->connected_link = 155;
    }
    if (12 < table->keys[0].size)
    {
        data = (dnxf_data_port_retimer_links_connection_t *) dnxc_data_mgmt_table_data_get(unit, table, 12, 0);
        data->connected_link = 156;
    }
    if (13 < table->keys[0].size)
    {
        data = (dnxf_data_port_retimer_links_connection_t *) dnxc_data_mgmt_table_data_get(unit, table, 13, 0);
        data->connected_link = 157;
    }
    if (14 < table->keys[0].size)
    {
        data = (dnxf_data_port_retimer_links_connection_t *) dnxc_data_mgmt_table_data_get(unit, table, 14, 0);
        data->connected_link = 158;
    }
    if (15 < table->keys[0].size)
    {
        data = (dnxf_data_port_retimer_links_connection_t *) dnxc_data_mgmt_table_data_get(unit, table, 15, 0);
        data->connected_link = 159;
    }
    if (16 < table->keys[0].size)
    {
        data = (dnxf_data_port_retimer_links_connection_t *) dnxc_data_mgmt_table_data_get(unit, table, 16, 0);
        data->connected_link = 160;
    }
    if (17 < table->keys[0].size)
    {
        data = (dnxf_data_port_retimer_links_connection_t *) dnxc_data_mgmt_table_data_get(unit, table, 17, 0);
        data->connected_link = 161;
    }
    if (18 < table->keys[0].size)
    {
        data = (dnxf_data_port_retimer_links_connection_t *) dnxc_data_mgmt_table_data_get(unit, table, 18, 0);
        data->connected_link = 162;
    }
    if (19 < table->keys[0].size)
    {
        data = (dnxf_data_port_retimer_links_connection_t *) dnxc_data_mgmt_table_data_get(unit, table, 19, 0);
        data->connected_link = 163;
    }
    if (20 < table->keys[0].size)
    {
        data = (dnxf_data_port_retimer_links_connection_t *) dnxc_data_mgmt_table_data_get(unit, table, 20, 0);
        data->connected_link = 164;
    }
    if (21 < table->keys[0].size)
    {
        data = (dnxf_data_port_retimer_links_connection_t *) dnxc_data_mgmt_table_data_get(unit, table, 21, 0);
        data->connected_link = 165;
    }
    if (22 < table->keys[0].size)
    {
        data = (dnxf_data_port_retimer_links_connection_t *) dnxc_data_mgmt_table_data_get(unit, table, 22, 0);
        data->connected_link = 166;
    }
    if (23 < table->keys[0].size)
    {
        data = (dnxf_data_port_retimer_links_connection_t *) dnxc_data_mgmt_table_data_get(unit, table, 23, 0);
        data->connected_link = 167;
    }
    if (24 < table->keys[0].size)
    {
        data = (dnxf_data_port_retimer_links_connection_t *) dnxc_data_mgmt_table_data_get(unit, table, 24, 0);
        data->connected_link = 168;
    }
    if (25 < table->keys[0].size)
    {
        data = (dnxf_data_port_retimer_links_connection_t *) dnxc_data_mgmt_table_data_get(unit, table, 25, 0);
        data->connected_link = 169;
    }
    if (26 < table->keys[0].size)
    {
        data = (dnxf_data_port_retimer_links_connection_t *) dnxc_data_mgmt_table_data_get(unit, table, 26, 0);
        data->connected_link = 170;
    }
    if (27 < table->keys[0].size)
    {
        data = (dnxf_data_port_retimer_links_connection_t *) dnxc_data_mgmt_table_data_get(unit, table, 27, 0);
        data->connected_link = 171;
    }
    if (28 < table->keys[0].size)
    {
        data = (dnxf_data_port_retimer_links_connection_t *) dnxc_data_mgmt_table_data_get(unit, table, 28, 0);
        data->connected_link = 172;
    }
    if (29 < table->keys[0].size)
    {
        data = (dnxf_data_port_retimer_links_connection_t *) dnxc_data_mgmt_table_data_get(unit, table, 29, 0);
        data->connected_link = 173;
    }
    if (30 < table->keys[0].size)
    {
        data = (dnxf_data_port_retimer_links_connection_t *) dnxc_data_mgmt_table_data_get(unit, table, 30, 0);
        data->connected_link = 174;
    }
    if (31 < table->keys[0].size)
    {
        data = (dnxf_data_port_retimer_links_connection_t *) dnxc_data_mgmt_table_data_get(unit, table, 31, 0);
        data->connected_link = 175;
    }
    if (32 < table->keys[0].size)
    {
        data = (dnxf_data_port_retimer_links_connection_t *) dnxc_data_mgmt_table_data_get(unit, table, 32, 0);
        data->connected_link = 176;
    }
    if (33 < table->keys[0].size)
    {
        data = (dnxf_data_port_retimer_links_connection_t *) dnxc_data_mgmt_table_data_get(unit, table, 33, 0);
        data->connected_link = 177;
    }
    if (34 < table->keys[0].size)
    {
        data = (dnxf_data_port_retimer_links_connection_t *) dnxc_data_mgmt_table_data_get(unit, table, 34, 0);
        data->connected_link = 178;
    }
    if (35 < table->keys[0].size)
    {
        data = (dnxf_data_port_retimer_links_connection_t *) dnxc_data_mgmt_table_data_get(unit, table, 35, 0);
        data->connected_link = 179;
    }
    if (36 < table->keys[0].size)
    {
        data = (dnxf_data_port_retimer_links_connection_t *) dnxc_data_mgmt_table_data_get(unit, table, 36, 0);
        data->connected_link = 180;
    }
    if (37 < table->keys[0].size)
    {
        data = (dnxf_data_port_retimer_links_connection_t *) dnxc_data_mgmt_table_data_get(unit, table, 37, 0);
        data->connected_link = 181;
    }
    if (38 < table->keys[0].size)
    {
        data = (dnxf_data_port_retimer_links_connection_t *) dnxc_data_mgmt_table_data_get(unit, table, 38, 0);
        data->connected_link = 182;
    }
    if (39 < table->keys[0].size)
    {
        data = (dnxf_data_port_retimer_links_connection_t *) dnxc_data_mgmt_table_data_get(unit, table, 39, 0);
        data->connected_link = 183;
    }
    if (40 < table->keys[0].size)
    {
        data = (dnxf_data_port_retimer_links_connection_t *) dnxc_data_mgmt_table_data_get(unit, table, 40, 0);
        data->connected_link = 184;
    }
    if (41 < table->keys[0].size)
    {
        data = (dnxf_data_port_retimer_links_connection_t *) dnxc_data_mgmt_table_data_get(unit, table, 41, 0);
        data->connected_link = 185;
    }
    if (42 < table->keys[0].size)
    {
        data = (dnxf_data_port_retimer_links_connection_t *) dnxc_data_mgmt_table_data_get(unit, table, 42, 0);
        data->connected_link = 186;
    }
    if (43 < table->keys[0].size)
    {
        data = (dnxf_data_port_retimer_links_connection_t *) dnxc_data_mgmt_table_data_get(unit, table, 43, 0);
        data->connected_link = 187;
    }
    if (44 < table->keys[0].size)
    {
        data = (dnxf_data_port_retimer_links_connection_t *) dnxc_data_mgmt_table_data_get(unit, table, 44, 0);
        data->connected_link = 188;
    }
    if (45 < table->keys[0].size)
    {
        data = (dnxf_data_port_retimer_links_connection_t *) dnxc_data_mgmt_table_data_get(unit, table, 45, 0);
        data->connected_link = 189;
    }
    if (46 < table->keys[0].size)
    {
        data = (dnxf_data_port_retimer_links_connection_t *) dnxc_data_mgmt_table_data_get(unit, table, 46, 0);
        data->connected_link = 190;
    }
    if (47 < table->keys[0].size)
    {
        data = (dnxf_data_port_retimer_links_connection_t *) dnxc_data_mgmt_table_data_get(unit, table, 47, 0);
        data->connected_link = 191;
    }
    if (48 < table->keys[0].size)
    {
        data = (dnxf_data_port_retimer_links_connection_t *) dnxc_data_mgmt_table_data_get(unit, table, 48, 0);
        data->connected_link = 144;
    }
    if (49 < table->keys[0].size)
    {
        data = (dnxf_data_port_retimer_links_connection_t *) dnxc_data_mgmt_table_data_get(unit, table, 49, 0);
        data->connected_link = 145;
    }
    if (50 < table->keys[0].size)
    {
        data = (dnxf_data_port_retimer_links_connection_t *) dnxc_data_mgmt_table_data_get(unit, table, 50, 0);
        data->connected_link = 146;
    }
    if (51 < table->keys[0].size)
    {
        data = (dnxf_data_port_retimer_links_connection_t *) dnxc_data_mgmt_table_data_get(unit, table, 51, 0);
        data->connected_link = 147;
    }
    if (52 < table->keys[0].size)
    {
        data = (dnxf_data_port_retimer_links_connection_t *) dnxc_data_mgmt_table_data_get(unit, table, 52, 0);
        data->connected_link = 148;
    }
    if (53 < table->keys[0].size)
    {
        data = (dnxf_data_port_retimer_links_connection_t *) dnxc_data_mgmt_table_data_get(unit, table, 53, 0);
        data->connected_link = 149;
    }
    if (54 < table->keys[0].size)
    {
        data = (dnxf_data_port_retimer_links_connection_t *) dnxc_data_mgmt_table_data_get(unit, table, 54, 0);
        data->connected_link = 150;
    }
    if (55 < table->keys[0].size)
    {
        data = (dnxf_data_port_retimer_links_connection_t *) dnxc_data_mgmt_table_data_get(unit, table, 55, 0);
        data->connected_link = 151;
    }
    if (56 < table->keys[0].size)
    {
        data = (dnxf_data_port_retimer_links_connection_t *) dnxc_data_mgmt_table_data_get(unit, table, 56, 0);
        data->connected_link = 104;
    }
    if (57 < table->keys[0].size)
    {
        data = (dnxf_data_port_retimer_links_connection_t *) dnxc_data_mgmt_table_data_get(unit, table, 57, 0);
        data->connected_link = 105;
    }
    if (58 < table->keys[0].size)
    {
        data = (dnxf_data_port_retimer_links_connection_t *) dnxc_data_mgmt_table_data_get(unit, table, 58, 0);
        data->connected_link = 106;
    }
    if (59 < table->keys[0].size)
    {
        data = (dnxf_data_port_retimer_links_connection_t *) dnxc_data_mgmt_table_data_get(unit, table, 59, 0);
        data->connected_link = 107;
    }
    if (60 < table->keys[0].size)
    {
        data = (dnxf_data_port_retimer_links_connection_t *) dnxc_data_mgmt_table_data_get(unit, table, 60, 0);
        data->connected_link = 108;
    }
    if (61 < table->keys[0].size)
    {
        data = (dnxf_data_port_retimer_links_connection_t *) dnxc_data_mgmt_table_data_get(unit, table, 61, 0);
        data->connected_link = 109;
    }
    if (62 < table->keys[0].size)
    {
        data = (dnxf_data_port_retimer_links_connection_t *) dnxc_data_mgmt_table_data_get(unit, table, 62, 0);
        data->connected_link = 110;
    }
    if (63 < table->keys[0].size)
    {
        data = (dnxf_data_port_retimer_links_connection_t *) dnxc_data_mgmt_table_data_get(unit, table, 63, 0);
        data->connected_link = 111;
    }
    if (64 < table->keys[0].size)
    {
        data = (dnxf_data_port_retimer_links_connection_t *) dnxc_data_mgmt_table_data_get(unit, table, 64, 0);
        data->connected_link = 112;
    }
    if (65 < table->keys[0].size)
    {
        data = (dnxf_data_port_retimer_links_connection_t *) dnxc_data_mgmt_table_data_get(unit, table, 65, 0);
        data->connected_link = 113;
    }
    if (66 < table->keys[0].size)
    {
        data = (dnxf_data_port_retimer_links_connection_t *) dnxc_data_mgmt_table_data_get(unit, table, 66, 0);
        data->connected_link = 114;
    }
    if (67 < table->keys[0].size)
    {
        data = (dnxf_data_port_retimer_links_connection_t *) dnxc_data_mgmt_table_data_get(unit, table, 67, 0);
        data->connected_link = 115;
    }
    if (68 < table->keys[0].size)
    {
        data = (dnxf_data_port_retimer_links_connection_t *) dnxc_data_mgmt_table_data_get(unit, table, 68, 0);
        data->connected_link = 116;
    }
    if (69 < table->keys[0].size)
    {
        data = (dnxf_data_port_retimer_links_connection_t *) dnxc_data_mgmt_table_data_get(unit, table, 69, 0);
        data->connected_link = 117;
    }
    if (70 < table->keys[0].size)
    {
        data = (dnxf_data_port_retimer_links_connection_t *) dnxc_data_mgmt_table_data_get(unit, table, 70, 0);
        data->connected_link = 118;
    }
    if (71 < table->keys[0].size)
    {
        data = (dnxf_data_port_retimer_links_connection_t *) dnxc_data_mgmt_table_data_get(unit, table, 71, 0);
        data->connected_link = 119;
    }
    if (72 < table->keys[0].size)
    {
        data = (dnxf_data_port_retimer_links_connection_t *) dnxc_data_mgmt_table_data_get(unit, table, 72, 0);
        data->connected_link = 120;
    }
    if (73 < table->keys[0].size)
    {
        data = (dnxf_data_port_retimer_links_connection_t *) dnxc_data_mgmt_table_data_get(unit, table, 73, 0);
        data->connected_link = 121;
    }
    if (74 < table->keys[0].size)
    {
        data = (dnxf_data_port_retimer_links_connection_t *) dnxc_data_mgmt_table_data_get(unit, table, 74, 0);
        data->connected_link = 122;
    }
    if (75 < table->keys[0].size)
    {
        data = (dnxf_data_port_retimer_links_connection_t *) dnxc_data_mgmt_table_data_get(unit, table, 75, 0);
        data->connected_link = 123;
    }
    if (76 < table->keys[0].size)
    {
        data = (dnxf_data_port_retimer_links_connection_t *) dnxc_data_mgmt_table_data_get(unit, table, 76, 0);
        data->connected_link = 124;
    }
    if (77 < table->keys[0].size)
    {
        data = (dnxf_data_port_retimer_links_connection_t *) dnxc_data_mgmt_table_data_get(unit, table, 77, 0);
        data->connected_link = 125;
    }
    if (78 < table->keys[0].size)
    {
        data = (dnxf_data_port_retimer_links_connection_t *) dnxc_data_mgmt_table_data_get(unit, table, 78, 0);
        data->connected_link = 126;
    }
    if (79 < table->keys[0].size)
    {
        data = (dnxf_data_port_retimer_links_connection_t *) dnxc_data_mgmt_table_data_get(unit, table, 79, 0);
        data->connected_link = 127;
    }
    if (80 < table->keys[0].size)
    {
        data = (dnxf_data_port_retimer_links_connection_t *) dnxc_data_mgmt_table_data_get(unit, table, 80, 0);
        data->connected_link = 128;
    }
    if (81 < table->keys[0].size)
    {
        data = (dnxf_data_port_retimer_links_connection_t *) dnxc_data_mgmt_table_data_get(unit, table, 81, 0);
        data->connected_link = 129;
    }
    if (82 < table->keys[0].size)
    {
        data = (dnxf_data_port_retimer_links_connection_t *) dnxc_data_mgmt_table_data_get(unit, table, 82, 0);
        data->connected_link = 130;
    }
    if (83 < table->keys[0].size)
    {
        data = (dnxf_data_port_retimer_links_connection_t *) dnxc_data_mgmt_table_data_get(unit, table, 83, 0);
        data->connected_link = 131;
    }
    if (84 < table->keys[0].size)
    {
        data = (dnxf_data_port_retimer_links_connection_t *) dnxc_data_mgmt_table_data_get(unit, table, 84, 0);
        data->connected_link = 132;
    }
    if (85 < table->keys[0].size)
    {
        data = (dnxf_data_port_retimer_links_connection_t *) dnxc_data_mgmt_table_data_get(unit, table, 85, 0);
        data->connected_link = 133;
    }
    if (86 < table->keys[0].size)
    {
        data = (dnxf_data_port_retimer_links_connection_t *) dnxc_data_mgmt_table_data_get(unit, table, 86, 0);
        data->connected_link = 134;
    }
    if (87 < table->keys[0].size)
    {
        data = (dnxf_data_port_retimer_links_connection_t *) dnxc_data_mgmt_table_data_get(unit, table, 87, 0);
        data->connected_link = 135;
    }
    if (88 < table->keys[0].size)
    {
        data = (dnxf_data_port_retimer_links_connection_t *) dnxc_data_mgmt_table_data_get(unit, table, 88, 0);
        data->connected_link = 136;
    }
    if (89 < table->keys[0].size)
    {
        data = (dnxf_data_port_retimer_links_connection_t *) dnxc_data_mgmt_table_data_get(unit, table, 89, 0);
        data->connected_link = 137;
    }
    if (90 < table->keys[0].size)
    {
        data = (dnxf_data_port_retimer_links_connection_t *) dnxc_data_mgmt_table_data_get(unit, table, 90, 0);
        data->connected_link = 138;
    }
    if (91 < table->keys[0].size)
    {
        data = (dnxf_data_port_retimer_links_connection_t *) dnxc_data_mgmt_table_data_get(unit, table, 91, 0);
        data->connected_link = 139;
    }
    if (92 < table->keys[0].size)
    {
        data = (dnxf_data_port_retimer_links_connection_t *) dnxc_data_mgmt_table_data_get(unit, table, 92, 0);
        data->connected_link = 140;
    }
    if (93 < table->keys[0].size)
    {
        data = (dnxf_data_port_retimer_links_connection_t *) dnxc_data_mgmt_table_data_get(unit, table, 93, 0);
        data->connected_link = 141;
    }
    if (94 < table->keys[0].size)
    {
        data = (dnxf_data_port_retimer_links_connection_t *) dnxc_data_mgmt_table_data_get(unit, table, 94, 0);
        data->connected_link = 142;
    }
    if (95 < table->keys[0].size)
    {
        data = (dnxf_data_port_retimer_links_connection_t *) dnxc_data_mgmt_table_data_get(unit, table, 95, 0);
        data->connected_link = 143;
    }
    if (96 < table->keys[0].size)
    {
        data = (dnxf_data_port_retimer_links_connection_t *) dnxc_data_mgmt_table_data_get(unit, table, 96, 0);
        data->connected_link = 0;
    }
    if (97 < table->keys[0].size)
    {
        data = (dnxf_data_port_retimer_links_connection_t *) dnxc_data_mgmt_table_data_get(unit, table, 97, 0);
        data->connected_link = 1;
    }
    if (98 < table->keys[0].size)
    {
        data = (dnxf_data_port_retimer_links_connection_t *) dnxc_data_mgmt_table_data_get(unit, table, 98, 0);
        data->connected_link = 2;
    }
    if (99 < table->keys[0].size)
    {
        data = (dnxf_data_port_retimer_links_connection_t *) dnxc_data_mgmt_table_data_get(unit, table, 99, 0);
        data->connected_link = 3;
    }
    if (100 < table->keys[0].size)
    {
        data = (dnxf_data_port_retimer_links_connection_t *) dnxc_data_mgmt_table_data_get(unit, table, 100, 0);
        data->connected_link = 4;
    }
    if (101 < table->keys[0].size)
    {
        data = (dnxf_data_port_retimer_links_connection_t *) dnxc_data_mgmt_table_data_get(unit, table, 101, 0);
        data->connected_link = 5;
    }
    if (102 < table->keys[0].size)
    {
        data = (dnxf_data_port_retimer_links_connection_t *) dnxc_data_mgmt_table_data_get(unit, table, 102, 0);
        data->connected_link = 6;
    }
    if (103 < table->keys[0].size)
    {
        data = (dnxf_data_port_retimer_links_connection_t *) dnxc_data_mgmt_table_data_get(unit, table, 103, 0);
        data->connected_link = 7;
    }
    if (104 < table->keys[0].size)
    {
        data = (dnxf_data_port_retimer_links_connection_t *) dnxc_data_mgmt_table_data_get(unit, table, 104, 0);
        data->connected_link = 56;
    }
    if (105 < table->keys[0].size)
    {
        data = (dnxf_data_port_retimer_links_connection_t *) dnxc_data_mgmt_table_data_get(unit, table, 105, 0);
        data->connected_link = 57;
    }
    if (106 < table->keys[0].size)
    {
        data = (dnxf_data_port_retimer_links_connection_t *) dnxc_data_mgmt_table_data_get(unit, table, 106, 0);
        data->connected_link = 58;
    }
    if (107 < table->keys[0].size)
    {
        data = (dnxf_data_port_retimer_links_connection_t *) dnxc_data_mgmt_table_data_get(unit, table, 107, 0);
        data->connected_link = 59;
    }
    if (108 < table->keys[0].size)
    {
        data = (dnxf_data_port_retimer_links_connection_t *) dnxc_data_mgmt_table_data_get(unit, table, 108, 0);
        data->connected_link = 60;
    }
    if (109 < table->keys[0].size)
    {
        data = (dnxf_data_port_retimer_links_connection_t *) dnxc_data_mgmt_table_data_get(unit, table, 109, 0);
        data->connected_link = 61;
    }
    if (110 < table->keys[0].size)
    {
        data = (dnxf_data_port_retimer_links_connection_t *) dnxc_data_mgmt_table_data_get(unit, table, 110, 0);
        data->connected_link = 62;
    }
    if (111 < table->keys[0].size)
    {
        data = (dnxf_data_port_retimer_links_connection_t *) dnxc_data_mgmt_table_data_get(unit, table, 111, 0);
        data->connected_link = 63;
    }
    if (112 < table->keys[0].size)
    {
        data = (dnxf_data_port_retimer_links_connection_t *) dnxc_data_mgmt_table_data_get(unit, table, 112, 0);
        data->connected_link = 64;
    }
    if (113 < table->keys[0].size)
    {
        data = (dnxf_data_port_retimer_links_connection_t *) dnxc_data_mgmt_table_data_get(unit, table, 113, 0);
        data->connected_link = 65;
    }
    if (114 < table->keys[0].size)
    {
        data = (dnxf_data_port_retimer_links_connection_t *) dnxc_data_mgmt_table_data_get(unit, table, 114, 0);
        data->connected_link = 66;
    }
    if (115 < table->keys[0].size)
    {
        data = (dnxf_data_port_retimer_links_connection_t *) dnxc_data_mgmt_table_data_get(unit, table, 115, 0);
        data->connected_link = 67;
    }
    if (116 < table->keys[0].size)
    {
        data = (dnxf_data_port_retimer_links_connection_t *) dnxc_data_mgmt_table_data_get(unit, table, 116, 0);
        data->connected_link = 68;
    }
    if (117 < table->keys[0].size)
    {
        data = (dnxf_data_port_retimer_links_connection_t *) dnxc_data_mgmt_table_data_get(unit, table, 117, 0);
        data->connected_link = 69;
    }
    if (118 < table->keys[0].size)
    {
        data = (dnxf_data_port_retimer_links_connection_t *) dnxc_data_mgmt_table_data_get(unit, table, 118, 0);
        data->connected_link = 70;
    }
    if (119 < table->keys[0].size)
    {
        data = (dnxf_data_port_retimer_links_connection_t *) dnxc_data_mgmt_table_data_get(unit, table, 119, 0);
        data->connected_link = 71;
    }
    if (120 < table->keys[0].size)
    {
        data = (dnxf_data_port_retimer_links_connection_t *) dnxc_data_mgmt_table_data_get(unit, table, 120, 0);
        data->connected_link = 72;
    }
    if (121 < table->keys[0].size)
    {
        data = (dnxf_data_port_retimer_links_connection_t *) dnxc_data_mgmt_table_data_get(unit, table, 121, 0);
        data->connected_link = 73;
    }
    if (122 < table->keys[0].size)
    {
        data = (dnxf_data_port_retimer_links_connection_t *) dnxc_data_mgmt_table_data_get(unit, table, 122, 0);
        data->connected_link = 74;
    }
    if (123 < table->keys[0].size)
    {
        data = (dnxf_data_port_retimer_links_connection_t *) dnxc_data_mgmt_table_data_get(unit, table, 123, 0);
        data->connected_link = 75;
    }
    if (124 < table->keys[0].size)
    {
        data = (dnxf_data_port_retimer_links_connection_t *) dnxc_data_mgmt_table_data_get(unit, table, 124, 0);
        data->connected_link = 76;
    }
    if (125 < table->keys[0].size)
    {
        data = (dnxf_data_port_retimer_links_connection_t *) dnxc_data_mgmt_table_data_get(unit, table, 125, 0);
        data->connected_link = 77;
    }
    if (126 < table->keys[0].size)
    {
        data = (dnxf_data_port_retimer_links_connection_t *) dnxc_data_mgmt_table_data_get(unit, table, 126, 0);
        data->connected_link = 78;
    }
    if (127 < table->keys[0].size)
    {
        data = (dnxf_data_port_retimer_links_connection_t *) dnxc_data_mgmt_table_data_get(unit, table, 127, 0);
        data->connected_link = 79;
    }
    if (128 < table->keys[0].size)
    {
        data = (dnxf_data_port_retimer_links_connection_t *) dnxc_data_mgmt_table_data_get(unit, table, 128, 0);
        data->connected_link = 80;
    }
    if (129 < table->keys[0].size)
    {
        data = (dnxf_data_port_retimer_links_connection_t *) dnxc_data_mgmt_table_data_get(unit, table, 129, 0);
        data->connected_link = 81;
    }
    if (130 < table->keys[0].size)
    {
        data = (dnxf_data_port_retimer_links_connection_t *) dnxc_data_mgmt_table_data_get(unit, table, 130, 0);
        data->connected_link = 82;
    }
    if (131 < table->keys[0].size)
    {
        data = (dnxf_data_port_retimer_links_connection_t *) dnxc_data_mgmt_table_data_get(unit, table, 131, 0);
        data->connected_link = 83;
    }
    if (132 < table->keys[0].size)
    {
        data = (dnxf_data_port_retimer_links_connection_t *) dnxc_data_mgmt_table_data_get(unit, table, 132, 0);
        data->connected_link = 84;
    }
    if (133 < table->keys[0].size)
    {
        data = (dnxf_data_port_retimer_links_connection_t *) dnxc_data_mgmt_table_data_get(unit, table, 133, 0);
        data->connected_link = 85;
    }
    if (134 < table->keys[0].size)
    {
        data = (dnxf_data_port_retimer_links_connection_t *) dnxc_data_mgmt_table_data_get(unit, table, 134, 0);
        data->connected_link = 86;
    }
    if (135 < table->keys[0].size)
    {
        data = (dnxf_data_port_retimer_links_connection_t *) dnxc_data_mgmt_table_data_get(unit, table, 135, 0);
        data->connected_link = 87;
    }
    if (136 < table->keys[0].size)
    {
        data = (dnxf_data_port_retimer_links_connection_t *) dnxc_data_mgmt_table_data_get(unit, table, 136, 0);
        data->connected_link = 88;
    }
    if (137 < table->keys[0].size)
    {
        data = (dnxf_data_port_retimer_links_connection_t *) dnxc_data_mgmt_table_data_get(unit, table, 137, 0);
        data->connected_link = 89;
    }
    if (138 < table->keys[0].size)
    {
        data = (dnxf_data_port_retimer_links_connection_t *) dnxc_data_mgmt_table_data_get(unit, table, 138, 0);
        data->connected_link = 90;
    }
    if (139 < table->keys[0].size)
    {
        data = (dnxf_data_port_retimer_links_connection_t *) dnxc_data_mgmt_table_data_get(unit, table, 139, 0);
        data->connected_link = 91;
    }
    if (140 < table->keys[0].size)
    {
        data = (dnxf_data_port_retimer_links_connection_t *) dnxc_data_mgmt_table_data_get(unit, table, 140, 0);
        data->connected_link = 92;
    }
    if (141 < table->keys[0].size)
    {
        data = (dnxf_data_port_retimer_links_connection_t *) dnxc_data_mgmt_table_data_get(unit, table, 141, 0);
        data->connected_link = 93;
    }
    if (142 < table->keys[0].size)
    {
        data = (dnxf_data_port_retimer_links_connection_t *) dnxc_data_mgmt_table_data_get(unit, table, 142, 0);
        data->connected_link = 94;
    }
    if (143 < table->keys[0].size)
    {
        data = (dnxf_data_port_retimer_links_connection_t *) dnxc_data_mgmt_table_data_get(unit, table, 143, 0);
        data->connected_link = 95;
    }
    if (144 < table->keys[0].size)
    {
        data = (dnxf_data_port_retimer_links_connection_t *) dnxc_data_mgmt_table_data_get(unit, table, 144, 0);
        data->connected_link = 48;
    }
    if (145 < table->keys[0].size)
    {
        data = (dnxf_data_port_retimer_links_connection_t *) dnxc_data_mgmt_table_data_get(unit, table, 145, 0);
        data->connected_link = 49;
    }
    if (146 < table->keys[0].size)
    {
        data = (dnxf_data_port_retimer_links_connection_t *) dnxc_data_mgmt_table_data_get(unit, table, 146, 0);
        data->connected_link = 50;
    }
    if (147 < table->keys[0].size)
    {
        data = (dnxf_data_port_retimer_links_connection_t *) dnxc_data_mgmt_table_data_get(unit, table, 147, 0);
        data->connected_link = 51;
    }
    if (148 < table->keys[0].size)
    {
        data = (dnxf_data_port_retimer_links_connection_t *) dnxc_data_mgmt_table_data_get(unit, table, 148, 0);
        data->connected_link = 52;
    }
    if (149 < table->keys[0].size)
    {
        data = (dnxf_data_port_retimer_links_connection_t *) dnxc_data_mgmt_table_data_get(unit, table, 149, 0);
        data->connected_link = 53;
    }
    if (150 < table->keys[0].size)
    {
        data = (dnxf_data_port_retimer_links_connection_t *) dnxc_data_mgmt_table_data_get(unit, table, 150, 0);
        data->connected_link = 54;
    }
    if (151 < table->keys[0].size)
    {
        data = (dnxf_data_port_retimer_links_connection_t *) dnxc_data_mgmt_table_data_get(unit, table, 151, 0);
        data->connected_link = 55;
    }
    if (152 < table->keys[0].size)
    {
        data = (dnxf_data_port_retimer_links_connection_t *) dnxc_data_mgmt_table_data_get(unit, table, 152, 0);
        data->connected_link = 8;
    }
    if (153 < table->keys[0].size)
    {
        data = (dnxf_data_port_retimer_links_connection_t *) dnxc_data_mgmt_table_data_get(unit, table, 153, 0);
        data->connected_link = 9;
    }
    if (154 < table->keys[0].size)
    {
        data = (dnxf_data_port_retimer_links_connection_t *) dnxc_data_mgmt_table_data_get(unit, table, 154, 0);
        data->connected_link = 10;
    }
    if (155 < table->keys[0].size)
    {
        data = (dnxf_data_port_retimer_links_connection_t *) dnxc_data_mgmt_table_data_get(unit, table, 155, 0);
        data->connected_link = 11;
    }
    if (156 < table->keys[0].size)
    {
        data = (dnxf_data_port_retimer_links_connection_t *) dnxc_data_mgmt_table_data_get(unit, table, 156, 0);
        data->connected_link = 12;
    }
    if (157 < table->keys[0].size)
    {
        data = (dnxf_data_port_retimer_links_connection_t *) dnxc_data_mgmt_table_data_get(unit, table, 157, 0);
        data->connected_link = 13;
    }
    if (158 < table->keys[0].size)
    {
        data = (dnxf_data_port_retimer_links_connection_t *) dnxc_data_mgmt_table_data_get(unit, table, 158, 0);
        data->connected_link = 14;
    }
    if (159 < table->keys[0].size)
    {
        data = (dnxf_data_port_retimer_links_connection_t *) dnxc_data_mgmt_table_data_get(unit, table, 159, 0);
        data->connected_link = 15;
    }
    if (160 < table->keys[0].size)
    {
        data = (dnxf_data_port_retimer_links_connection_t *) dnxc_data_mgmt_table_data_get(unit, table, 160, 0);
        data->connected_link = 16;
    }
    if (161 < table->keys[0].size)
    {
        data = (dnxf_data_port_retimer_links_connection_t *) dnxc_data_mgmt_table_data_get(unit, table, 161, 0);
        data->connected_link = 17;
    }
    if (162 < table->keys[0].size)
    {
        data = (dnxf_data_port_retimer_links_connection_t *) dnxc_data_mgmt_table_data_get(unit, table, 162, 0);
        data->connected_link = 18;
    }
    if (163 < table->keys[0].size)
    {
        data = (dnxf_data_port_retimer_links_connection_t *) dnxc_data_mgmt_table_data_get(unit, table, 163, 0);
        data->connected_link = 19;
    }
    if (164 < table->keys[0].size)
    {
        data = (dnxf_data_port_retimer_links_connection_t *) dnxc_data_mgmt_table_data_get(unit, table, 164, 0);
        data->connected_link = 20;
    }
    if (165 < table->keys[0].size)
    {
        data = (dnxf_data_port_retimer_links_connection_t *) dnxc_data_mgmt_table_data_get(unit, table, 165, 0);
        data->connected_link = 21;
    }
    if (166 < table->keys[0].size)
    {
        data = (dnxf_data_port_retimer_links_connection_t *) dnxc_data_mgmt_table_data_get(unit, table, 166, 0);
        data->connected_link = 22;
    }
    if (167 < table->keys[0].size)
    {
        data = (dnxf_data_port_retimer_links_connection_t *) dnxc_data_mgmt_table_data_get(unit, table, 167, 0);
        data->connected_link = 23;
    }
    if (168 < table->keys[0].size)
    {
        data = (dnxf_data_port_retimer_links_connection_t *) dnxc_data_mgmt_table_data_get(unit, table, 168, 0);
        data->connected_link = 24;
    }
    if (169 < table->keys[0].size)
    {
        data = (dnxf_data_port_retimer_links_connection_t *) dnxc_data_mgmt_table_data_get(unit, table, 169, 0);
        data->connected_link = 25;
    }
    if (170 < table->keys[0].size)
    {
        data = (dnxf_data_port_retimer_links_connection_t *) dnxc_data_mgmt_table_data_get(unit, table, 170, 0);
        data->connected_link = 26;
    }
    if (171 < table->keys[0].size)
    {
        data = (dnxf_data_port_retimer_links_connection_t *) dnxc_data_mgmt_table_data_get(unit, table, 171, 0);
        data->connected_link = 27;
    }
    if (172 < table->keys[0].size)
    {
        data = (dnxf_data_port_retimer_links_connection_t *) dnxc_data_mgmt_table_data_get(unit, table, 172, 0);
        data->connected_link = 28;
    }
    if (173 < table->keys[0].size)
    {
        data = (dnxf_data_port_retimer_links_connection_t *) dnxc_data_mgmt_table_data_get(unit, table, 173, 0);
        data->connected_link = 29;
    }
    if (174 < table->keys[0].size)
    {
        data = (dnxf_data_port_retimer_links_connection_t *) dnxc_data_mgmt_table_data_get(unit, table, 174, 0);
        data->connected_link = 30;
    }
    if (175 < table->keys[0].size)
    {
        data = (dnxf_data_port_retimer_links_connection_t *) dnxc_data_mgmt_table_data_get(unit, table, 175, 0);
        data->connected_link = 31;
    }
    if (176 < table->keys[0].size)
    {
        data = (dnxf_data_port_retimer_links_connection_t *) dnxc_data_mgmt_table_data_get(unit, table, 176, 0);
        data->connected_link = 32;
    }
    if (177 < table->keys[0].size)
    {
        data = (dnxf_data_port_retimer_links_connection_t *) dnxc_data_mgmt_table_data_get(unit, table, 177, 0);
        data->connected_link = 33;
    }
    if (178 < table->keys[0].size)
    {
        data = (dnxf_data_port_retimer_links_connection_t *) dnxc_data_mgmt_table_data_get(unit, table, 178, 0);
        data->connected_link = 34;
    }
    if (179 < table->keys[0].size)
    {
        data = (dnxf_data_port_retimer_links_connection_t *) dnxc_data_mgmt_table_data_get(unit, table, 179, 0);
        data->connected_link = 35;
    }
    if (180 < table->keys[0].size)
    {
        data = (dnxf_data_port_retimer_links_connection_t *) dnxc_data_mgmt_table_data_get(unit, table, 180, 0);
        data->connected_link = 36;
    }
    if (181 < table->keys[0].size)
    {
        data = (dnxf_data_port_retimer_links_connection_t *) dnxc_data_mgmt_table_data_get(unit, table, 181, 0);
        data->connected_link = 37;
    }
    if (182 < table->keys[0].size)
    {
        data = (dnxf_data_port_retimer_links_connection_t *) dnxc_data_mgmt_table_data_get(unit, table, 182, 0);
        data->connected_link = 38;
    }
    if (183 < table->keys[0].size)
    {
        data = (dnxf_data_port_retimer_links_connection_t *) dnxc_data_mgmt_table_data_get(unit, table, 183, 0);
        data->connected_link = 39;
    }
    if (184 < table->keys[0].size)
    {
        data = (dnxf_data_port_retimer_links_connection_t *) dnxc_data_mgmt_table_data_get(unit, table, 184, 0);
        data->connected_link = 40;
    }
    if (185 < table->keys[0].size)
    {
        data = (dnxf_data_port_retimer_links_connection_t *) dnxc_data_mgmt_table_data_get(unit, table, 185, 0);
        data->connected_link = 41;
    }
    if (186 < table->keys[0].size)
    {
        data = (dnxf_data_port_retimer_links_connection_t *) dnxc_data_mgmt_table_data_get(unit, table, 186, 0);
        data->connected_link = 42;
    }
    if (187 < table->keys[0].size)
    {
        data = (dnxf_data_port_retimer_links_connection_t *) dnxc_data_mgmt_table_data_get(unit, table, 187, 0);
        data->connected_link = 43;
    }
    if (188 < table->keys[0].size)
    {
        data = (dnxf_data_port_retimer_links_connection_t *) dnxc_data_mgmt_table_data_get(unit, table, 188, 0);
        data->connected_link = 44;
    }
    if (189 < table->keys[0].size)
    {
        data = (dnxf_data_port_retimer_links_connection_t *) dnxc_data_mgmt_table_data_get(unit, table, 189, 0);
        data->connected_link = 45;
    }
    if (190 < table->keys[0].size)
    {
        data = (dnxf_data_port_retimer_links_connection_t *) dnxc_data_mgmt_table_data_get(unit, table, 190, 0);
        data->connected_link = 46;
    }
    if (191 < table->keys[0].size)
    {
        data = (dnxf_data_port_retimer_links_connection_t *) dnxc_data_mgmt_table_data_get(unit, table, 191, 0);
        data->connected_link = 47;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}





static shr_error_e
ramon_a0_dnxf_data_port_features_isolation_needed_before_disable_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnxf_data_module_port;
    int submodule_index = dnxf_data_port_submodule_features;
    int feature_index = dnxf_data_port_features_isolation_needed_before_disable;
    SHR_FUNC_INIT_VARS(unit);

    feature = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].features[feature_index];
    
    feature->default_data = 1;

    
    feature->data = 1;

    
    feature->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_feature_value_set(unit, feature));

exit:
    SHR_FUNC_EXIT;
}





shr_error_e
ramon_a0_data_port_attach(
    int unit)
{
    dnxc_data_module_t *module = NULL;
    dnxc_data_submodule_t *submodule = NULL;
    dnxc_data_define_t *define = NULL;
    dnxc_data_feature_t *feature = NULL;
    dnxc_data_table_t *table = NULL;
    int module_index = dnxf_data_module_port;
    int submodule_index = -1, data_index = -1;
    SHR_FUNC_INIT_VARS(unit);

    COMPILER_REFERENCE(define);
    COMPILER_REFERENCE(feature);
    COMPILER_REFERENCE(table);
    COMPILER_REFERENCE(submodule);
    COMPILER_REFERENCE(data_index);
    COMPILER_REFERENCE(submodule_index);
    module = &_dnxc_data[unit].modules[module_index];
    
    submodule_index = dnxf_data_port_submodule_general;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnxf_data_port_general_define_nof_links;
    define = &submodule->defines[data_index];
    define->set = ramon_a0_dnxf_data_port_general_nof_links_set;
    data_index = dnxf_data_port_general_define_nof_pms;
    define = &submodule->defines[data_index];
    define->set = ramon_a0_dnxf_data_port_general_nof_pms_set;
    data_index = dnxf_data_port_general_define_max_bucket_fill_rate;
    define = &submodule->defines[data_index];
    define->set = ramon_a0_dnxf_data_port_general_max_bucket_fill_rate_set;
    data_index = dnxf_data_port_general_define_link_mask;
    define = &submodule->defines[data_index];
    define->set = ramon_a0_dnxf_data_port_general_link_mask_set;
    data_index = dnxf_data_port_general_define_aldwp_max_val;
    define = &submodule->defines[data_index];
    define->set = ramon_a0_dnxf_data_port_general_aldwp_max_val_set;
    data_index = dnxf_data_port_general_define_fmac_clock_khz;
    define = &submodule->defines[data_index];
    define->set = ramon_a0_dnxf_data_port_general_fmac_clock_khz_set;
    data_index = dnxf_data_port_general_define_speed_max;
    define = &submodule->defines[data_index];
    define->set = ramon_a0_dnxf_data_port_general_speed_max_set;
    data_index = dnxf_data_port_general_define_fw_load_method;
    define = &submodule->defines[data_index];
    define->set = ramon_a0_dnxf_data_port_general_fw_load_method_set;
    data_index = dnxf_data_port_general_define_fw_crc_check;
    define = &submodule->defines[data_index];
    define->set = ramon_a0_dnxf_data_port_general_fw_crc_check_set;
    data_index = dnxf_data_port_general_define_fw_load_verify;
    define = &submodule->defines[data_index];
    define->set = ramon_a0_dnxf_data_port_general_fw_load_verify_set;
    data_index = dnxf_data_port_general_define_mac_bucket_fill_rate;
    define = &submodule->defines[data_index];
    define->set = ramon_a0_dnxf_data_port_general_mac_bucket_fill_rate_set;

    

    
    data_index = dnxf_data_port_general_table_supported_phys;
    table = &submodule->tables[data_index];
    table->set = ramon_a0_dnxf_data_port_general_supported_phys_set;
    data_index = dnxf_data_port_general_table_fmac_bus_size;
    table = &submodule->tables[data_index];
    table->set = ramon_a0_dnxf_data_port_general_fmac_bus_size_set;
    
    submodule_index = dnxf_data_port_submodule_stat;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnxf_data_port_stat_define_thread_interval;
    define = &submodule->defines[data_index];
    define->set = ramon_a0_dnxf_data_port_stat_thread_interval_set;
    data_index = dnxf_data_port_stat_define_thread_counter_pri;
    define = &submodule->defines[data_index];
    define->set = ramon_a0_dnxf_data_port_stat_thread_counter_pri_set;
    data_index = dnxf_data_port_stat_define_thread_timeout;
    define = &submodule->defines[data_index];
    define->set = ramon_a0_dnxf_data_port_stat_thread_timeout_set;

    

    
    
    submodule_index = dnxf_data_port_submodule_static_add;
    submodule = &module->submodules[submodule_index];

    

    

    
    data_index = dnxf_data_port_static_add_table_port_info;
    table = &submodule->tables[data_index];
    table->set = ramon_a0_dnxf_data_port_static_add_port_info_set;
    data_index = dnxf_data_port_static_add_table_serdes_lane_config;
    table = &submodule->tables[data_index];
    table->set = ramon_a0_dnxf_data_port_static_add_serdes_lane_config_set;
    data_index = dnxf_data_port_static_add_table_serdes_tx_taps;
    table = &submodule->tables[data_index];
    table->set = ramon_a0_dnxf_data_port_static_add_serdes_tx_taps_set;
    data_index = dnxf_data_port_static_add_table_quad_info;
    table = &submodule->tables[data_index];
    table->set = ramon_a0_dnxf_data_port_static_add_quad_info_set;
    
    submodule_index = dnxf_data_port_submodule_lane_map;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnxf_data_port_lane_map_define_lane_map_bound;
    define = &submodule->defines[data_index];
    define->set = ramon_a0_dnxf_data_port_lane_map_lane_map_bound_set;

    

    
    data_index = dnxf_data_port_lane_map_table_info;
    table = &submodule->tables[data_index];
    table->set = ramon_a0_dnxf_data_port_lane_map_info_set;
    
    submodule_index = dnxf_data_port_submodule_pll;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnxf_data_port_pll_define_nof_lcpll;
    define = &submodule->defines[data_index];
    define->set = ramon_a0_dnxf_data_port_pll_nof_lcpll_set;
    data_index = dnxf_data_port_pll_define_nof_port_in_lcpll;
    define = &submodule->defines[data_index];
    define->set = ramon_a0_dnxf_data_port_pll_nof_port_in_lcpll_set;
    data_index = dnxf_data_port_pll_define_nof_plls_per_pm;
    define = &submodule->defines[data_index];
    define->set = ramon_a0_dnxf_data_port_pll_nof_plls_per_pm_set;
    data_index = dnxf_data_port_pll_define_nof_config_words;
    define = &submodule->defines[data_index];
    define->set = ramon_a0_dnxf_data_port_pll_nof_config_words_set;

    

    
    data_index = dnxf_data_port_pll_table_info;
    table = &submodule->tables[data_index];
    table->set = ramon_a0_dnxf_data_port_pll_info_set;
    data_index = dnxf_data_port_pll_table_config;
    table = &submodule->tables[data_index];
    table->set = ramon_a0_dnxf_data_port_pll_config_set;
    
    submodule_index = dnxf_data_port_submodule_synce;
    submodule = &module->submodules[submodule_index];

    

    

    
    data_index = dnxf_data_port_synce_table_cfg;
    table = &submodule->tables[data_index];
    table->set = ramon_a0_dnxf_data_port_synce_cfg_set;
    
    submodule_index = dnxf_data_port_submodule_retimer;
    submodule = &module->submodules[submodule_index];

    

    
    data_index = dnxf_data_port_retimer_is_supported;
    feature = &submodule->features[data_index];
    feature->set = ramon_a0_dnxf_data_port_retimer_is_supported_set;

    
    data_index = dnxf_data_port_retimer_table_links_connection;
    table = &submodule->tables[data_index];
    table->set = ramon_a0_dnxf_data_port_retimer_links_connection_set;
    
    submodule_index = dnxf_data_port_submodule_features;
    submodule = &module->submodules[submodule_index];

    

    
    data_index = dnxf_data_port_features_isolation_needed_before_disable;
    feature = &submodule->features[data_index];
    feature->set = ramon_a0_dnxf_data_port_features_isolation_needed_before_disable_set;

    

    SHR_FUNC_EXIT;
}
#undef BSL_LOG_MODULE

