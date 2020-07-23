

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_PORT

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_port.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_property.h>
#include <soc/dnxc/dnxc_port.h>
#include <bcm/port.h>
#include <bcm/switch.h>
#include <bcm_int/dnx/algo/port/algo_port_mgmt.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_fabric.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_device.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_nif.h>
#include <bcm_int/dnx/port/imb/imb.h>








static shr_error_e
jr2_a0_dnx_data_port_static_add_default_speed_for_special_if_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_port;
    int submodule_index = dnx_data_port_submodule_static_add;
    int define_index = dnx_data_port_static_add_define_default_speed_for_special_if;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 10000;

    
    define->data = 10000;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->flags |= DNXC_DATA_F_INIT_ONLY;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_port_static_add_fabric_fw_load_method_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_port;
    int submodule_index = dnx_data_port_submodule_static_add;
    int define_index = dnx_data_port_static_add_define_fabric_fw_load_method;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = phymodFirmwareLoadMethodExternal;

    
    define->data = phymodFirmwareLoadMethodExternal;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = "load_firmware_fabric";
    define->property.doc = 
        "\n"
        "0x<verify FW><load method>\n"
        "The method of the firmware load.\n"
        "2 - fast; 1 - MDIO load; 0 - do not load firmware.\n"
        "Byte 0 of the SoC property value.\n"
        "\n"
    ;
    define->property.method = dnxc_data_property_method_custom;
    define->property.method_str = "custom";

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));

    SHR_IF_ERR_EXIT(dnx_data_property_port_static_add_fabric_fw_load_method_read(unit, (uint32 *) &define->data));
    
    define->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_port_static_add_fabric_fw_crc_check_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_port;
    int submodule_index = dnx_data_port_submodule_static_add;
    int define_index = dnx_data_port_static_add_define_fabric_fw_crc_check;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 1;

    
    define->data = 1;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = "load_firmware_fabric";
    define->property.doc = 
        "\n"
        "Enable firmware CRC check.\n"
        "Byte 1: bit 0 of the SoC property value.\n"
        "\n"
    ;
    define->property.method = dnxc_data_property_method_custom;
    define->property.method_str = "custom";

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));

    SHR_IF_ERR_EXIT(dnx_data_property_port_static_add_fabric_fw_crc_check_read(unit, (uint32 *) &define->data));
    
    define->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_port_static_add_fabric_fw_load_verify_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_port;
    int submodule_index = dnx_data_port_submodule_static_add;
    int define_index = dnx_data_port_static_add_define_fabric_fw_load_verify;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0;

    
    define->data = 0;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = "load_firmware_fabric";
    define->property.doc = 
        "\n"
        "Enable firmware verification, byte by byte (time consuming).\n"
        "Byte 1: bit 4 of the SoC property value.\n"
        "\n"
    ;
    define->property.method = dnxc_data_property_method_custom;
    define->property.method_str = "custom";

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));

    SHR_IF_ERR_EXIT(dnx_data_property_port_static_add_fabric_fw_load_verify_read(unit, (uint32 *) &define->data));
    
    define->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_port_static_add_nif_fw_load_method_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_port;
    int submodule_index = dnx_data_port_submodule_static_add;
    int define_index = dnx_data_port_static_add_define_nif_fw_load_method;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = phymodFirmwareLoadMethodExternal;

    
    define->data = phymodFirmwareLoadMethodExternal;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = "load_firmware_nif";
    define->property.doc = 
        "\n"
        "0x<verify FW><load method>\n"
        "The method of the firmware load (first hexadecimal charcter).\n"
        "2 - fast; 1 - MDIO load; 0 - do not load firmware\n"
        "Verify FW (first hexadecimal charcter).\n"
        "1 - enables firmware CRC verfication\n"
        "Setting relevant for NIF side\n"
        "\n"
    ;
    define->property.method = dnxc_data_property_method_custom;
    define->property.method_str = "custom";

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));

    SHR_IF_ERR_EXIT(dnx_data_property_port_static_add_nif_fw_load_method_read(unit, (uint32 *) &define->data));
    
    define->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_port_static_add_nif_fw_crc_check_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_port;
    int submodule_index = dnx_data_port_submodule_static_add;
    int define_index = dnx_data_port_static_add_define_nif_fw_crc_check;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 1;

    
    define->data = 1;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = "load_firmware_nif";
    define->property.doc = 
        "\n"
        "Enable firmware CRC check for NIF side.\n"
        "Byte 1: bit 0 of the SoC property value.\n"
        "\n"
    ;
    define->property.method = dnxc_data_property_method_custom;
    define->property.method_str = "custom";

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));

    SHR_IF_ERR_EXIT(dnx_data_property_port_static_add_nif_fw_crc_check_read(unit, (uint32 *) &define->data));
    
    define->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_port_static_add_nif_fw_load_verify_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_port;
    int submodule_index = dnx_data_port_submodule_static_add;
    int define_index = dnx_data_port_static_add_define_nif_fw_load_verify;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0;

    
    define->data = 0;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = "load_firmware_nif";
    define->property.doc = 
        "\n"
        "Enable firmware verification, byte by byte (time consuming) for NIF side.\n"
        "Byte 1: bit 4 of the SoC property value.\n"
        "\n"
    ;
    define->property.method = dnxc_data_property_method_custom;
    define->property.method_str = "custom";

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));

    SHR_IF_ERR_EXIT(dnx_data_property_port_static_add_nif_fw_load_verify_read(unit, (uint32 *) &define->data));
    
    define->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}



static shr_error_e
jr2_a0_dnx_data_port_static_add_ucode_port_set(
    int unit)
{
    int port_index;
    dnx_data_port_static_add_ucode_port_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_port;
    int submodule_index = dnx_data_port_submodule_static_add;
    int table_index = dnx_data_port_static_add_table_ucode_port;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->flags |= DNXC_DATA_F_INIT_ONLY;

    
    table->keys[0].size = dnx_data_port.general.fabric_port_base_get(unit);
    table->info_get.key_size[0] = dnx_data_port.general.fabric_port_base_get(unit);

    
    table->values[0].default_val = "BCM_PORT_IF_NULL";
    table->values[1].default_val = "0";
    table->values[2].default_val = "-1";
    table->values[3].default_val = "-1";
    table->values[4].default_val = "-1";
    table->values[5].default_val = "-1";
    table->values[6].default_val = "0";
    table->values[7].default_val = "0";
    table->values[8].default_val = "0";
    table->values[9].default_val = "0";
    table->values[10].default_val = "0";
    table->values[11].default_val = "0";
    table->values[12].default_val = "0";
    table->values[13].default_val = "0";
    table->values[14].default_val = "0";
    table->values[15].default_val = "0";
    table->values[16].default_val = "0";
    table->values[17].default_val = "2";
    table->values[18].default_val = "-1";
    table->values[19].default_val = "2";
    table->values[20].default_val = "-1";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_port_static_add_ucode_port_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_port_static_add_table_ucode_port");

    
    default_data = (dnx_data_port_static_add_ucode_port_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->interface = BCM_PORT_IF_NULL;
    default_data->nof_lanes = 0;
    default_data->interface_offset = -1;
    default_data->core = -1;
    default_data->tm_port = -1;
    default_data->channel = -1;
    default_data->is_stif = 0;
    default_data->is_stif_data = 0;
    default_data->is_kbp = 0;
    default_data->is_cross_connect = 0;
    default_data->is_flexe_phy = 0;
    default_data->is_ingress_interleave = 0;
    default_data->is_egress_interleave = 0;
    default_data->is_tdm = 0;
    default_data->is_if_tdm_only = 0;
    default_data->is_if_tdm_hybrid = 0;
    default_data->sch_priority_propagation_en = 0;
    default_data->num_priorities = 2;
    default_data->base_q_pair = -1;
    default_data->num_sch_priorities = 2;
    default_data->base_hr = -1;
    
    for (port_index = 0; port_index < table->keys[0].size; port_index++)
    {
        data = (dnx_data_port_static_add_ucode_port_t *) dnxc_data_mgmt_table_data_get(unit, table, port_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    table->property.name = "ucode_port, port_priorities, otm_base_q_pair, port_priorities_sch, port_base_hr, ilkn_is_burst_interleaving";
    table->property.doc =
        "\n"
        "1.\n"
        "To map a physical interface and channel to a local logical port and to a core ID and TM port in the core, use the SOC property:\n"
        "ucode_port_#logical_port_num# = #interface_type##interface_id#[.#channel_num#]:core#core-id#.#tm-port#[:stat][:kbp][:tdm][:if_tdm_only][:if_tdm_hybrid], where:\n"
        "*    logical_port_num is the logical port index.\n"
        "*    interface_type is the Physical Interface type.\n"
        "*    interface_id is the index of the specific physical interface, if applicable.\n"
        "*    channel_num is the channel number within the physical interface, if applicable (relevant for channelized interfaces).\n"
        "*    core-id is the core index, 0 or 1.\n"
        "*    tm-port is the TM port of the core (255 is reserved for ERP).\n"
        "*    stat - add ':stat' to use this logical port as statistics interface (see Statistics Interface section for further details)\n"
        "*    stat_data - add ':stat_data' to use this logical port as a mix of statistics interface and data port (see Statistics Interface section for further details)\n"
        "*    kbp - add ':kbp' to use this logical port as KBP\n"
        "*    tdm - add ':tdm' to use this logical port as TDM\n"
        "*    if_tdm_only - add ':if_tdm_only' to use this interface to include TDM ports only\n"
        "*    if_tdm_hybrid - add ':if_tdm_hybrid' to use this interface to include both TDM ports and non TDM ports\n"
        "*    cross_connect - add ':cross_connect' to use this logical port as cross connect\n"
        "*    flexe - add ':flexe' to use this logical port as FlexE physical port\n"
        "*    egr_ilv - add ':egr_ilv' to enable egress interleaving for the interface of that port. relevant for master port.\n"
        "*    sch_priority_propagation - add ':sch_priority_propagation' to enable port priority propagation for the port. All ports of the interface must have the same value of sch_priority_propagation\n"
        "Example: ucode_port_5 = ILKN1.35:core0.2 maps logical port 5 to Interlaken interface number 1 on channel 35, and to core 0 TM port 2.\n"
        "\n"
        "2.\n"
        "To determine the number of OTM queue-pairs for a port, use the following SOC property:\n"
        "port_priorities#port# = 1/2/4/8.\n"
        "\n"
        "3.\n"
        "Usually queue pairs are automatically allocated for each port.\n"
        "A user can choose to manage the queues explicitly by using the SOC property otm_base_q_pair_#port#=0-254\n"
        "('255' value is reserved and cannot be used).\n"
        "This can be useful for example when dynamic port provisioning is required, to optimize queue pair allocation for the more challenging scenario\n"
        "4.\n"
        "To determine the number of HRs for a port, use the following SOC property:\n"
        "port_priorities_sch#port# = 1/2/4/8.\n"
        "\n"
        "5.\n"
        "Usually HRs are automatically allocated for each port.\n"
        "A user can choose to manage the HRs explicitly by using the SOC property port_base_hr_#port#=0-255\n"
        "('255' value is reserved for ERP port).\n"
        "This can be useful for example when dynamic port provisioning is required, to optimize HR allocation for the more challenging scenario\n"
        "\n"
        "6.\n"
        "To set ILKN interface to work in interleaved mode in ingress, use the following SOC property:\n"
        "ilkn_is_burst_interleaving#ILKN_id# = 1.\n"
        "\n"
    ;
    table->property.method = dnxc_data_property_method_custom;
    table->property.method_str = "custom";

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));

    for (port_index = 0; port_index < table->keys[0].size; port_index++)
    {
        data = (dnx_data_port_static_add_ucode_port_t *) dnxc_data_mgmt_table_data_get(unit, table, port_index, 0);
        SHR_IF_ERR_EXIT(dnx_data_property_port_static_add_ucode_port_read(unit, port_index, data));
    }
    
    table->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_port_static_add_speed_set(
    int unit)
{
    int port_index;
    dnx_data_port_static_add_speed_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_port;
    int submodule_index = dnx_data_port_submodule_static_add;
    int table_index = dnx_data_port_static_add_table_speed;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->flags |= DNXC_DATA_F_INIT_ONLY;

    
    table->keys[0].size = dnx_data_port.general.fabric_port_base_get(unit) + dnx_data_fabric.links.nof_links_get(unit);
    table->info_get.key_size[0] = dnx_data_port.general.fabric_port_base_get(unit) + dnx_data_fabric.links.nof_links_get(unit);

    
    table->values[0].default_val = "DNXC_PORT_INVALID_SPEED";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_port_static_add_speed_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_port_static_add_table_speed");

    
    default_data = (dnx_data_port_static_add_speed_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->val = DNXC_PORT_INVALID_SPEED;
    
    for (port_index = 0; port_index < table->keys[0].size; port_index++)
    {
        data = (dnx_data_port_static_add_speed_t *) dnxc_data_mgmt_table_data_get(unit, table, port_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    table->values[0].property.name = spn_PORT_INIT_SPEED;
    table->values[0].property.doc =
        "\n"
        "The default port rate is decided according to interface type.\n"
        "To set different port rate (mbps) use:\n"
        "port_init_speed_<port>=#speed#\n"
        "A list of supported rates per interface type can be found in User Manual.\n"
        "\n"
    ;
    table->values[0].property.method = dnxc_data_property_method_port_range_signed;
    table->values[0].property.method_str = "port_range_signed";
    table->values[0].property.range_min = -1;
    table->values[0].property.range_max = 4000000;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));

    for (port_index = 0; port_index < table->keys[0].size; port_index++)
    {
        data = (dnx_data_port_static_add_speed_t *) dnxc_data_mgmt_table_data_get(unit, table, port_index, 0);
        SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &table->values[0].property, port_index, &data->val));
    }
    
    table->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_port_static_add_ext_stat_speed_set(
    int unit)
{
    int ext_stat_port_index;
    dnx_data_port_static_add_ext_stat_speed_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_port;
    int submodule_index = dnx_data_port_submodule_static_add;
    int table_index = dnx_data_port_static_add_table_ext_stat_speed;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = 4;
    table->info_get.key_size[0] = 4;

    
    table->values[0].default_val = "DNXC_PORT_INVALID_SPEED";
    table->values[1].default_val = "0";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_port_static_add_ext_stat_speed_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_port_static_add_table_ext_stat_speed");

    
    default_data = (dnx_data_port_static_add_ext_stat_speed_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->speed = DNXC_PORT_INVALID_SPEED;
    default_data->nof_lanes = 0;
    
    for (ext_stat_port_index = 0; ext_stat_port_index < table->keys[0].size; ext_stat_port_index++)
    {
        data = (dnx_data_port_static_add_ext_stat_speed_t *) dnxc_data_mgmt_table_data_get(unit, table, ext_stat_port_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    table->property.name = "port_init_speed";
    table->property.doc =
        "\n"
        "ext_stat_speed configure the speed of the external device (kbp) statistic port.\n"
        "The default port rate is invalid. meaning the external kbp port does not exist.\n"
        "In addition, it also determine how many lanes the port has.\n"
        "Use the Soc property ext_stat_speed with suffix ext_stat in order to configure it.\n"
        "Syntax:\n"
        "port_init_speed_ext_stat[ext_stat_port_id]=[speed_MHz].[nof_lanes]lanes\n"
        "Examples:\n"
        "1. port_init_speed_ext_stat0=200000.4lanes - ext stat port_id=0, speed=200GHz, 4 lanes.\n"
        "2. port_init_speed_ext_stat2=100000.2lanes - ext stat port_id=2, speed=100GHz, 2 lanes.\n"
        "\n"
    ;
    table->property.method = dnxc_data_property_method_custom;
    table->property.method_str = "custom";

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));

    for (ext_stat_port_index = 0; ext_stat_port_index < table->keys[0].size; ext_stat_port_index++)
    {
        data = (dnx_data_port_static_add_ext_stat_speed_t *) dnxc_data_mgmt_table_data_get(unit, table, ext_stat_port_index, 0);
        SHR_IF_ERR_EXIT(dnx_data_property_port_static_add_ext_stat_speed_read(unit, ext_stat_port_index, data));
    }
    
    table->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_port_static_add_eth_padding_set(
    int unit)
{
    int port_index;
    dnx_data_port_static_add_eth_padding_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_port;
    int submodule_index = dnx_data_port_submodule_static_add;
    int table_index = dnx_data_port_static_add_table_eth_padding;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->flags |= DNXC_DATA_F_INIT_ONLY;

    
    table->keys[0].size = dnx_data_port.general.fabric_port_base_get(unit);
    table->info_get.key_size[0] = dnx_data_port.general.fabric_port_base_get(unit);

    
    table->values[0].default_val = "0";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_port_static_add_eth_padding_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_port_static_add_table_eth_padding");

    
    default_data = (dnx_data_port_static_add_eth_padding_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->pad_size = 0;
    
    for (port_index = 0; port_index < table->keys[0].size; port_index++)
    {
        data = (dnx_data_port_static_add_eth_padding_t *) dnxc_data_mgmt_table_data_get(unit, table, port_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    table->values[0].property.name = spn_PACKET_PADDING_SIZE;
    table->values[0].property.doc =
        "\n"
        "Configure padding size per eth port (min 64 max 96)\n"
        "\n"
    ;
    table->values[0].property.method = dnxc_data_property_method_port_range_signed;
    table->values[0].property.method_str = "port_range_signed";
    table->values[0].property.range_min = 64;
    table->values[0].property.range_max = 96;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));

    for (port_index = 0; port_index < table->keys[0].size; port_index++)
    {
        data = (dnx_data_port_static_add_eth_padding_t *) dnxc_data_mgmt_table_data_get(unit, table, port_index, 0);
        SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &table->values[0].property, port_index, &data->pad_size));
    }
    
    table->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_port_static_add_link_training_set(
    int unit)
{
    int port_index;
    dnx_data_port_static_add_link_training_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_port;
    int submodule_index = dnx_data_port_submodule_static_add;
    int table_index = dnx_data_port_static_add_table_link_training;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->flags |= DNXC_DATA_F_INIT_ONLY;

    
    table->keys[0].size = dnx_data_port.general.fabric_port_base_get(unit) + dnx_data_fabric.links.nof_links_get(unit);
    table->info_get.key_size[0] = dnx_data_port.general.fabric_port_base_get(unit) + dnx_data_fabric.links.nof_links_get(unit);

    
    table->values[0].default_val = "BCM_PORT_RESOURCE_DEFAULT_REQUEST";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_port_static_add_link_training_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_port_static_add_table_link_training");

    
    default_data = (dnx_data_port_static_add_link_training_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->val = BCM_PORT_RESOURCE_DEFAULT_REQUEST;
    
    for (port_index = 0; port_index < table->keys[0].size; port_index++)
    {
        data = (dnx_data_port_static_add_link_training_t *) dnxc_data_mgmt_table_data_get(unit, table, port_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    table->values[0].property.name = spn_PORT_INIT_CL72;
    table->values[0].property.doc =
        "\n"
        "Enable /Disable CL72\n"
        "port_init_cl72_<port> = 1 / 0\n"
        "\n"
    ;
    table->values[0].property.method = dnxc_data_property_method_port_enable;
    table->values[0].property.method_str = "port_enable";

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));

    for (port_index = 0; port_index < table->keys[0].size; port_index++)
    {
        data = (dnx_data_port_static_add_link_training_t *) dnxc_data_mgmt_table_data_get(unit, table, port_index, 0);
        SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &table->values[0].property, port_index, &data->val));
    }
    
    table->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_port_static_add_ext_stat_link_training_set(
    int unit)
{
    int ext_stat_port_index;
    dnx_data_port_static_add_ext_stat_link_training_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_port;
    int submodule_index = dnx_data_port_submodule_static_add;
    int table_index = dnx_data_port_static_add_table_ext_stat_link_training;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = 4;
    table->info_get.key_size[0] = 4;

    
    table->values[0].default_val = "0";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_port_static_add_ext_stat_link_training_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_port_static_add_table_ext_stat_link_training");

    
    default_data = (dnx_data_port_static_add_ext_stat_link_training_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->val = 0;
    
    for (ext_stat_port_index = 0; ext_stat_port_index < table->keys[0].size; ext_stat_port_index++)
    {
        data = (dnx_data_port_static_add_ext_stat_link_training_t *) dnxc_data_mgmt_table_data_get(unit, table, ext_stat_port_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    table->values[0].property.name = spn_PORT_INIT_CL72;
    table->values[0].property.doc =
        "\n"
        "Enable /Disable link training (CL72) for external device (kbp) statistic port.\n"
        "\n"
    ;
    table->values[0].property.method = dnxc_data_property_method_suffix_enable;
    table->values[0].property.method_str = "suffix_enable";
    table->values[0].property.suffix = "ext_stat";

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));

    for (ext_stat_port_index = 0; ext_stat_port_index < table->keys[0].size; ext_stat_port_index++)
    {
        data = (dnx_data_port_static_add_ext_stat_link_training_t *) dnxc_data_mgmt_table_data_get(unit, table, ext_stat_port_index, 0);
        SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &table->values[0].property, ext_stat_port_index, &data->val));
    }
    
    table->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_port_static_add_fec_type_set(
    int unit)
{
    int port_index;
    dnx_data_port_static_add_fec_type_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_port;
    int submodule_index = dnx_data_port_submodule_static_add;
    int table_index = dnx_data_port_static_add_table_fec_type;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->flags |= DNXC_DATA_F_INIT_ONLY;

    
    table->keys[0].size = dnx_data_port.general.fabric_port_base_get(unit) + dnx_data_fabric.links.nof_links_get(unit);
    table->info_get.key_size[0] = dnx_data_port.general.fabric_port_base_get(unit) + dnx_data_fabric.links.nof_links_get(unit);

    
    table->values[0].default_val = "BCM_PORT_RESOURCE_DEFAULT_REQUEST";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_port_static_add_fec_type_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_port_static_add_table_fec_type");

    
    default_data = (dnx_data_port_static_add_fec_type_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->val = BCM_PORT_RESOURCE_DEFAULT_REQUEST;
    
    for (port_index = 0; port_index < table->keys[0].size; port_index++)
    {
        data = (dnx_data_port_static_add_fec_type_t *) dnxc_data_mgmt_table_data_get(unit, table, port_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    table->values[0].property.name = spn_PORT_FEC;
    table->values[0].property.doc =
        "\n"
        "Specifies the FEC type. The types are as follows:\n"
        "0 - no FEC\n"
        "1 - BASE R - 64/66b KR FEC\n"
        "2 - CL91/RS-FEC\n"
        "3 - Rs544, using 1xN RS FEC architecture\n"
        "4 - RS 272, using 1xN RS FEC architecture\n"
        "5 - RS 206 - 64/66b 5T RS FEC\n"
        "6 - RS 108 - 64/66b 5T low latency RS FEC\n"
        "7 - RS 545 - 64/66b 15T RS FEC\n"
        "8 - RS 304 - 64/66b 15T low latency RS FEC\n"
        "9 - Rs544, using 2xN RS FEC architecture\n"
        "10 - Rs272, using 2xN RS FEC architecture\n"
        "\n"
    ;
    table->values[0].property.method = dnxc_data_property_method_port_direct_map;
    table->values[0].property.method_str = "port_direct_map";
    table->values[0].property.nof_mapping = 11;
    DNXC_DATA_ALLOC(table->values[0].property.mapping, dnxc_data_property_map_t, table->values[0].property.nof_mapping, "dnx_data_port_static_add_fec_type_t property mapping");

    table->values[0].property.mapping[0].name = "0";
    table->values[0].property.mapping[0].val = bcmPortPhyFecNone;
    table->values[0].property.mapping[1].name = "1";
    table->values[0].property.mapping[1].val = bcmPortPhyFecBaseR;
    table->values[0].property.mapping[2].name = "2";
    table->values[0].property.mapping[2].val = bcmPortPhyFecRsFec;
    table->values[0].property.mapping[3].name = "3";
    table->values[0].property.mapping[3].val = bcmPortPhyFecRs544;
    table->values[0].property.mapping[4].name = "4";
    table->values[0].property.mapping[4].val = bcmPortPhyFecRs272;
    table->values[0].property.mapping[5].name = "5";
    table->values[0].property.mapping[5].val = bcmPortPhyFecRs206;
    table->values[0].property.mapping[6].name = "6";
    table->values[0].property.mapping[6].val = bcmPortPhyFecRs108;
    table->values[0].property.mapping[7].name = "7";
    table->values[0].property.mapping[7].val = bcmPortPhyFecRs545;
    table->values[0].property.mapping[8].name = "8";
    table->values[0].property.mapping[8].val = bcmPortPhyFecRs304;
    table->values[0].property.mapping[9].name = "9";
    table->values[0].property.mapping[9].val = bcmPortPhyFecRs544_2xN;
    table->values[0].property.mapping[10].name = "10";
    table->values[0].property.mapping[10].val = bcmPortPhyFecRs272_2xN;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));

    for (port_index = 0; port_index < table->keys[0].size; port_index++)
    {
        data = (dnx_data_port_static_add_fec_type_t *) dnxc_data_mgmt_table_data_get(unit, table, port_index, 0);
        SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &table->values[0].property, port_index, &data->val));
    }
    
    table->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_port_static_add_serdes_lane_config_set(
    int unit)
{
    int port_index;
    dnx_data_port_static_add_serdes_lane_config_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_port;
    int submodule_index = dnx_data_port_submodule_static_add;
    int table_index = dnx_data_port_static_add_table_serdes_lane_config;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->flags |= DNXC_DATA_F_INIT_ONLY;

    
    table->keys[0].size = dnx_data_port.general.fabric_port_base_get(unit) + dnx_data_fabric.links.nof_links_get(unit);
    table->info_get.key_size[0] = dnx_data_port.general.fabric_port_base_get(unit) + dnx_data_fabric.links.nof_links_get(unit);

    
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

    
    DNXC_DATA_ALLOC(table->data, dnx_data_port_static_add_serdes_lane_config_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_port_static_add_table_serdes_lane_config");

    
    default_data = (dnx_data_port_static_add_serdes_lane_config_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->dfe = DNXC_PORT_SERDES_LANE_CONFIG_INVALID_VAL;
    default_data->media_type = DNXC_PORT_SERDES_LANE_CONFIG_INVALID_VAL;
    default_data->unreliable_los = DNXC_PORT_SERDES_LANE_CONFIG_INVALID_VAL;
    default_data->cl72_auto_polarity_enable = DNXC_PORT_SERDES_LANE_CONFIG_INVALID_VAL;
    default_data->cl72_restart_timeout_enable = DNXC_PORT_SERDES_LANE_CONFIG_INVALID_VAL;
    default_data->channel_mode = DNXC_PORT_SERDES_LANE_CONFIG_INVALID_VAL;
    
    for (port_index = 0; port_index < table->keys[0].size; port_index++)
    {
        data = (dnx_data_port_static_add_serdes_lane_config_t *) dnxc_data_mgmt_table_data_get(unit, table, port_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    table->values[0].property.name = spn_SERDES_LANE_CONFIG;
    table->values[0].property.doc =
        "\n"
        "DFE filter\n"
        "serdes_lane_config_dfe_<port>=on|off|lp\n"
        "on - dfe is on and lp dfe is off\n"
        "off - both dfe and lp dfe are off\n"
        "lp - both dfe and lp dfe are on\n"
        "\n"
    ;
    table->values[0].property.method = dnxc_data_property_method_port_suffix_direct_map;
    table->values[0].property.method_str = "port_suffix_direct_map";
    table->values[0].property.suffix = "dfe";
    table->values[0].property.nof_mapping = 3;
    DNXC_DATA_ALLOC(table->values[0].property.mapping, dnxc_data_property_map_t, table->values[0].property.nof_mapping, "dnx_data_port_static_add_serdes_lane_config_t property mapping");

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
        "serdes_lane_config_media_type_<port>=backplane|copper|optics\n"
        "\n"
    ;
    table->values[1].property.method = dnxc_data_property_method_port_suffix_direct_map;
    table->values[1].property.method_str = "port_suffix_direct_map";
    table->values[1].property.suffix = "media_type";
    table->values[1].property.nof_mapping = 3;
    DNXC_DATA_ALLOC(table->values[1].property.mapping, dnxc_data_property_map_t, table->values[1].property.nof_mapping, "dnx_data_port_static_add_serdes_lane_config_t property mapping");

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
        "serdes_lane_config_unreliable_los_<port>=0|1\n"
        "\n"
    ;
    table->values[2].property.method = dnxc_data_property_method_port_suffix_enable;
    table->values[2].property.method_str = "port_suffix_enable";
    table->values[2].property.suffix = "unreliable_los";
    
    table->values[3].property.name = spn_SERDES_LANE_CONFIG;
    table->values[3].property.doc =
        "\n"
        "enable cl72 auto polarity\n"
        "serdes_lane_config_cl72_auto_polarity_en_<port>=0|1\n"
        "\n"
    ;
    table->values[3].property.method = dnxc_data_property_method_port_suffix_enable;
    table->values[3].property.method_str = "port_suffix_enable";
    table->values[3].property.suffix = "cl72_auto_polarity_en";
    
    table->values[4].property.name = spn_SERDES_LANE_CONFIG;
    table->values[4].property.doc =
        "\n"
        "enable cl72 restart timeout\n"
        "serdes_lane_config_cl72_restart_timeout_en_<port>=0|1\n"
        "\n"
    ;
    table->values[4].property.method = dnxc_data_property_method_port_suffix_enable;
    table->values[4].property.method_str = "port_suffix_enable";
    table->values[4].property.suffix = "cl72_restart_timeout_en";
    
    table->values[5].property.name = spn_SERDES_LANE_CONFIG;
    table->values[5].property.doc =
        "\n"
        "channel mode\n"
        "serdes_lane_config_channel_mode_<port>=force_nr|force_er\n"
        "\n"
    ;
    table->values[5].property.method = dnxc_data_property_method_port_suffix_direct_map;
    table->values[5].property.method_str = "port_suffix_direct_map";
    table->values[5].property.suffix = "channel_mode";
    table->values[5].property.nof_mapping = 2;
    DNXC_DATA_ALLOC(table->values[5].property.mapping, dnxc_data_property_map_t, table->values[5].property.nof_mapping, "dnx_data_port_static_add_serdes_lane_config_t property mapping");

    table->values[5].property.mapping[0].name = "force_nr";
    table->values[5].property.mapping[0].val = soc_dnxc_port_force_nr;
    table->values[5].property.mapping[1].name = "force_er";
    table->values[5].property.mapping[1].val = soc_dnxc_port_force_er;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));

    for (port_index = 0; port_index < table->keys[0].size; port_index++)
    {
        data = (dnx_data_port_static_add_serdes_lane_config_t *) dnxc_data_mgmt_table_data_get(unit, table, port_index, 0);
        SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &table->values[0].property, port_index, &data->dfe));
    }
    for (port_index = 0; port_index < table->keys[0].size; port_index++)
    {
        data = (dnx_data_port_static_add_serdes_lane_config_t *) dnxc_data_mgmt_table_data_get(unit, table, port_index, 0);
        SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &table->values[1].property, port_index, &data->media_type));
    }
    for (port_index = 0; port_index < table->keys[0].size; port_index++)
    {
        data = (dnx_data_port_static_add_serdes_lane_config_t *) dnxc_data_mgmt_table_data_get(unit, table, port_index, 0);
        SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &table->values[2].property, port_index, &data->unreliable_los));
    }
    for (port_index = 0; port_index < table->keys[0].size; port_index++)
    {
        data = (dnx_data_port_static_add_serdes_lane_config_t *) dnxc_data_mgmt_table_data_get(unit, table, port_index, 0);
        SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &table->values[3].property, port_index, &data->cl72_auto_polarity_enable));
    }
    for (port_index = 0; port_index < table->keys[0].size; port_index++)
    {
        data = (dnx_data_port_static_add_serdes_lane_config_t *) dnxc_data_mgmt_table_data_get(unit, table, port_index, 0);
        SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &table->values[4].property, port_index, &data->cl72_restart_timeout_enable));
    }
    for (port_index = 0; port_index < table->keys[0].size; port_index++)
    {
        data = (dnx_data_port_static_add_serdes_lane_config_t *) dnxc_data_mgmt_table_data_get(unit, table, port_index, 0);
        SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &table->values[5].property, port_index, &data->channel_mode));
    }
    
    table->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_port_static_add_serdes_tx_taps_set(
    int unit)
{
    int port_index;
    dnx_data_port_static_add_serdes_tx_taps_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_port;
    int submodule_index = dnx_data_port_submodule_static_add;
    int table_index = dnx_data_port_static_add_table_serdes_tx_taps;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->flags |= DNXC_DATA_F_INIT_ONLY;

    
    table->keys[0].size = dnx_data_port.general.fabric_port_base_get(unit) + dnx_data_fabric.links.nof_links_get(unit);
    table->info_get.key_size[0] = dnx_data_port.general.fabric_port_base_get(unit) + dnx_data_fabric.links.nof_links_get(unit);

    
    table->values[0].default_val = "0";
    table->values[1].default_val = "DNXC_PORT_TX_FIR_INVALID_MAIN_TAP";
    table->values[2].default_val = "0";
    table->values[3].default_val = "0";
    table->values[4].default_val = "0";
    table->values[5].default_val = "0";
    table->values[6].default_val = "bcmPortPhyTxTapMode3Tap";
    table->values[7].default_val = "bcmPortPhySignallingModeNRZ";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_port_static_add_serdes_tx_taps_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_port_static_add_table_serdes_tx_taps");

    
    default_data = (dnx_data_port_static_add_serdes_tx_taps_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->pre = 0;
    default_data->main = DNXC_PORT_TX_FIR_INVALID_MAIN_TAP;
    default_data->post = 0;
    default_data->pre2 = 0;
    default_data->post2 = 0;
    default_data->post3 = 0;
    default_data->tx_tap_mode = bcmPortPhyTxTapMode3Tap;
    default_data->signalling_mode = bcmPortPhySignallingModeNRZ;
    
    for (port_index = 0; port_index < table->keys[0].size; port_index++)
    {
        data = (dnx_data_port_static_add_serdes_tx_taps_t *) dnxc_data_mgmt_table_data_get(unit, table, port_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    table->property.name = "serdes_tx_taps";
    table->property.doc =
        "\n"
        "a set of TX FIR parameters for the port.\n"
        "serdes_tx_taps_<port>=signalling_mode:pre:main:post:pre2:post2:post3\n"
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
        data = (dnx_data_port_static_add_serdes_tx_taps_t *) dnxc_data_mgmt_table_data_get(unit, table, port_index, 0);
        SHR_IF_ERR_EXIT(dnx_data_property_port_static_add_serdes_tx_taps_read(unit, port_index, data));
    }
    
    table->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_port_static_add_ext_stat_global_serdes_tx_taps_set(
    int unit)
{
    dnx_data_port_static_add_ext_stat_global_serdes_tx_taps_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_port;
    int submodule_index = dnx_data_port_submodule_static_add;
    int table_index = dnx_data_port_static_add_table_ext_stat_global_serdes_tx_taps;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->values[0].default_val = "0";
    table->values[1].default_val = "DNXC_PORT_TX_FIR_INVALID_MAIN_TAP";
    table->values[2].default_val = "0";
    table->values[3].default_val = "0";
    table->values[4].default_val = "0";
    table->values[5].default_val = "0";
    table->values[6].default_val = "1";
    
    DNXC_DATA_ALLOC(table->data, dnx_data_port_static_add_ext_stat_global_serdes_tx_taps_t, (1 + 1  ), "data of dnx_data_port_static_add_table_ext_stat_global_serdes_tx_taps");

    
    default_data = (dnx_data_port_static_add_ext_stat_global_serdes_tx_taps_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->pre = 0;
    default_data->main = DNXC_PORT_TX_FIR_INVALID_MAIN_TAP;
    default_data->post = 0;
    default_data->pre2 = 0;
    default_data->post2 = 0;
    default_data->post3 = 0;
    default_data->txfir_tap_enable = 1;
    
    data = (dnx_data_port_static_add_ext_stat_global_serdes_tx_taps_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
    sal_memcpy(data, default_data, table->size_of_values);
    
    table->property.name = "serdes_tx_taps";
    table->property.doc =
        "\n"
        "a set of TX FIR parameters for the external stat kbp device.\n"
        "It is configured once for the entire device, as follow:\n"
        "serdes_tx_taps_ext_stat=signalling_mode:pre:main:post:pre2:post2:post3\n"
        "signalling_mode can be either pam4 or nrz.\n"
        "[pre2:post2:post3] are mandatory. (Can be set to zero).\n"
        "main is set to DNXC_PORT_TX_FIR_INVALID_MAIN_TAP by default to distinguish between whether the SoC property was set or not.\n"
        "\n"
    ;
    table->property.method = dnxc_data_property_method_custom;
    table->property.method_str = "custom";

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));

    data = (dnx_data_port_static_add_ext_stat_global_serdes_tx_taps_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
    SHR_IF_ERR_EXIT(dnx_data_property_port_static_add_ext_stat_global_serdes_tx_taps_read(unit, data));
    
    table->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_port_static_add_ext_stat_lane_serdes_tx_taps_set(
    int unit)
{
    int lane_index;
    dnx_data_port_static_add_ext_stat_lane_serdes_tx_taps_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_port;
    int submodule_index = dnx_data_port_submodule_static_add;
    int table_index = dnx_data_port_static_add_table_ext_stat_lane_serdes_tx_taps;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = 32;
    table->info_get.key_size[0] = 32;

    
    table->values[0].default_val = "0";
    table->values[1].default_val = "DNXC_PORT_TX_FIR_INVALID_MAIN_TAP";
    table->values[2].default_val = "0";
    table->values[3].default_val = "0";
    table->values[4].default_val = "0";
    table->values[5].default_val = "0";
    table->values[6].default_val = "1";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_port_static_add_ext_stat_lane_serdes_tx_taps_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_port_static_add_table_ext_stat_lane_serdes_tx_taps");

    
    default_data = (dnx_data_port_static_add_ext_stat_lane_serdes_tx_taps_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->pre = 0;
    default_data->main = DNXC_PORT_TX_FIR_INVALID_MAIN_TAP;
    default_data->post = 0;
    default_data->pre2 = 0;
    default_data->post2 = 0;
    default_data->post3 = 0;
    default_data->txfir_tap_enable = 1;
    
    for (lane_index = 0; lane_index < table->keys[0].size; lane_index++)
    {
        data = (dnx_data_port_static_add_ext_stat_lane_serdes_tx_taps_t *) dnxc_data_mgmt_table_data_get(unit, table, lane_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    table->property.name = "serdes_tx_taps";
    table->property.doc =
        "\n"
        "a set of TX FIR parameters for the external stat kbp device\n"
        "It is configured per stat lane, as follow:\n"
        "serdes_tx_taps_ext_stat_lane<lane_id>=signalling_mode:pre:main:post:pre2:post2:post3\n"
        "signalling_mode can be either pam4 or nrz.\n"
        "[pre2:post2:post3] are mandatory. (Can be set to zero).\n"
        "main is set to DNXC_PORT_TX_FIR_INVALID_MAIN_TAP by default to distinguish between whether the SoC property was set or not.\n"
        "\n"
    ;
    table->property.method = dnxc_data_property_method_custom;
    table->property.method_str = "custom";

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));

    for (lane_index = 0; lane_index < table->keys[0].size; lane_index++)
    {
        data = (dnx_data_port_static_add_ext_stat_lane_serdes_tx_taps_t *) dnxc_data_mgmt_table_data_get(unit, table, lane_index, 0);
        SHR_IF_ERR_EXIT(dnx_data_property_port_static_add_ext_stat_lane_serdes_tx_taps_read(unit, lane_index, data));
    }
    
    table->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_port_static_add_tx_pam4_precoder_set(
    int unit)
{
    int port_index;
    dnx_data_port_static_add_tx_pam4_precoder_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_port;
    int submodule_index = dnx_data_port_submodule_static_add;
    int table_index = dnx_data_port_static_add_table_tx_pam4_precoder;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->flags |= DNXC_DATA_F_INIT_ONLY;

    
    table->keys[0].size = dnx_data_port.general.fabric_port_base_get(unit) + dnx_data_fabric.links.nof_links_get(unit);
    table->info_get.key_size[0] = dnx_data_port.general.fabric_port_base_get(unit) + dnx_data_fabric.links.nof_links_get(unit);

    
    table->values[0].default_val = "0";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_port_static_add_tx_pam4_precoder_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_port_static_add_table_tx_pam4_precoder");

    
    default_data = (dnx_data_port_static_add_tx_pam4_precoder_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->val = 0;
    
    for (port_index = 0; port_index < table->keys[0].size; port_index++)
    {
        data = (dnx_data_port_static_add_tx_pam4_precoder_t *) dnxc_data_mgmt_table_data_get(unit, table, port_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    table->values[0].property.name = spn_PORT_TX_PAM4_PRECODER;
    table->values[0].property.doc =
        "\n"
        "is the precoding enabled on TX side.\n"
        "port_tx_pam4_precoder_<port>=enable/disable\n"
        "\n"
    ;
    table->values[0].property.method = dnxc_data_property_method_port_direct_map;
    table->values[0].property.method_str = "port_direct_map";
    table->values[0].property.nof_mapping = 2;
    DNXC_DATA_ALLOC(table->values[0].property.mapping, dnxc_data_property_map_t, table->values[0].property.nof_mapping, "dnx_data_port_static_add_tx_pam4_precoder_t property mapping");

    table->values[0].property.mapping[0].name = "enable";
    table->values[0].property.mapping[0].val = 1;
    table->values[0].property.mapping[1].name = "disable";
    table->values[0].property.mapping[1].val = 0;
    table->values[0].property.mapping[1].is_default = 1 ;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));

    for (port_index = 0; port_index < table->keys[0].size; port_index++)
    {
        data = (dnx_data_port_static_add_tx_pam4_precoder_t *) dnxc_data_mgmt_table_data_get(unit, table, port_index, 0);
        SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &table->values[0].property, port_index, &data->val));
    }
    
    table->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_port_static_add_lp_tx_precoder_set(
    int unit)
{
    int port_index;
    dnx_data_port_static_add_lp_tx_precoder_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_port;
    int submodule_index = dnx_data_port_submodule_static_add;
    int table_index = dnx_data_port_static_add_table_lp_tx_precoder;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->flags |= DNXC_DATA_F_INIT_ONLY;

    
    table->keys[0].size = dnx_data_port.general.fabric_port_base_get(unit) + dnx_data_fabric.links.nof_links_get(unit);
    table->info_get.key_size[0] = dnx_data_port.general.fabric_port_base_get(unit) + dnx_data_fabric.links.nof_links_get(unit);

    
    table->values[0].default_val = "0";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_port_static_add_lp_tx_precoder_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_port_static_add_table_lp_tx_precoder");

    
    default_data = (dnx_data_port_static_add_lp_tx_precoder_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->val = 0;
    
    for (port_index = 0; port_index < table->keys[0].size; port_index++)
    {
        data = (dnx_data_port_static_add_lp_tx_precoder_t *) dnxc_data_mgmt_table_data_get(unit, table, port_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    table->values[0].property.name = spn_PORT_LP_TX_PRECODER;
    table->values[0].property.doc =
        "\n"
        "has the link partner enabled pre-coding on its TX side.\n"
        "in other words - enable the decoding on my RX side.\n"
        "port_lp_tx_precoder_<port>=enable/disable\n"
        "\n"
    ;
    table->values[0].property.method = dnxc_data_property_method_port_direct_map;
    table->values[0].property.method_str = "port_direct_map";
    table->values[0].property.nof_mapping = 2;
    DNXC_DATA_ALLOC(table->values[0].property.mapping, dnxc_data_property_map_t, table->values[0].property.nof_mapping, "dnx_data_port_static_add_lp_tx_precoder_t property mapping");

    table->values[0].property.mapping[0].name = "enable";
    table->values[0].property.mapping[0].val = 1;
    table->values[0].property.mapping[1].name = "disable";
    table->values[0].property.mapping[1].val = 0;
    table->values[0].property.mapping[1].is_default = 1 ;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));

    for (port_index = 0; port_index < table->keys[0].size; port_index++)
    {
        data = (dnx_data_port_static_add_lp_tx_precoder_t *) dnxc_data_mgmt_table_data_get(unit, table, port_index, 0);
        SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &table->values[0].property, port_index, &data->val));
    }
    
    table->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_port_static_add_fabric_quad_info_set(
    int unit)
{
    int quad_index;
    dnx_data_port_static_add_fabric_quad_info_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_port;
    int submodule_index = dnx_data_port_submodule_static_add;
    int table_index = dnx_data_port_static_add_table_fabric_quad_info;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->flags |= DNXC_DATA_F_INIT_ONLY;

    
    table->keys[0].size = dnx_data_port.general.fabric_port_base_get(unit) / dnx_data_fabric.blocks.nof_links_in_fmac_get(unit);
    table->info_get.key_size[0] = dnx_data_port.general.fabric_port_base_get(unit) / dnx_data_fabric.blocks.nof_links_in_fmac_get(unit);

    
    table->values[0].default_val = "1";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_port_static_add_fabric_quad_info_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_port_static_add_table_fabric_quad_info");

    
    default_data = (dnx_data_port_static_add_fabric_quad_info_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->quad_enable = 1;
    
    for (quad_index = 0; quad_index < table->keys[0].size; quad_index++)
    {
        data = (dnx_data_port_static_add_fabric_quad_info_t *) dnxc_data_mgmt_table_data_get(unit, table, quad_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    table->values[0].property.name = "serdes_qrtt_active";
    table->values[0].property.doc =
        "\n"
        "Enable / Disable Quad.\n"
        "serdes_qrtt_active_<quad_number>=1/0\n"
        "If there is no quad_number suffix, the enable/disable will be done for all quads.\n"
        "Default - all quads are enabled.\n"
        "\n"
    ;
    table->values[0].property.method = dnxc_data_property_method_custom;
    table->values[0].property.method_str = "custom";

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));

    for (quad_index = 0; quad_index < table->keys[0].size; quad_index++)
    {
        data = (dnx_data_port_static_add_fabric_quad_info_t *) dnxc_data_mgmt_table_data_get(unit, table, quad_index, 0);
        SHR_IF_ERR_EXIT(dnx_data_property_port_static_add_fabric_quad_info_quad_enable_read(unit, quad_index, &data->quad_enable));
    }
    
    table->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_port_static_add_header_type_set(
    int unit)
{
    int port_index;
    dnx_data_port_static_add_header_type_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_port;
    int submodule_index = dnx_data_port_submodule_static_add;
    int table_index = dnx_data_port_static_add_table_header_type;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = dnx_data_port.general.fabric_port_base_get(unit);
    table->info_get.key_size[0] = dnx_data_port.general.fabric_port_base_get(unit);

    
    table->values[0].default_val = "BCM_SWITCH_PORT_HEADER_TYPE_ETH";
    table->values[1].default_val = "BCM_SWITCH_PORT_HEADER_TYPE_ETH";
    table->values[2].default_val = "BCM_SWITCH_PORT_HEADER_TYPE_ETH";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_port_static_add_header_type_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_port_static_add_table_header_type");

    
    default_data = (dnx_data_port_static_add_header_type_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->header_type_in = BCM_SWITCH_PORT_HEADER_TYPE_ETH;
    default_data->header_type_out = BCM_SWITCH_PORT_HEADER_TYPE_ETH;
    default_data->header_type = BCM_SWITCH_PORT_HEADER_TYPE_ETH;
    
    for (port_index = 0; port_index < table->keys[0].size; port_index++)
    {
        data = (dnx_data_port_static_add_header_type_t *) dnxc_data_mgmt_table_data_get(unit, table, port_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    table->values[0].property.name = spn_TM_PORT_HEADER_TYPE;
    table->values[0].property.doc =
        "\n"
        "Header type per incoming port.\n"
        "\n"
    ;
    table->values[0].property.method = dnxc_data_property_method_port_suffix_direct_map;
    table->values[0].property.method_str = "port_suffix_direct_map";
    table->values[0].property.suffix = "in";
    table->values[0].property.nof_mapping = 11;
    DNXC_DATA_ALLOC(table->values[0].property.mapping, dnxc_data_property_map_t, table->values[0].property.nof_mapping, "dnx_data_port_static_add_header_type_t property mapping");

    table->values[0].property.mapping[0].name = "ETH";
    table->values[0].property.mapping[0].val = BCM_SWITCH_PORT_HEADER_TYPE_ETH;
    table->values[0].property.mapping[0].is_default = 1 ;
    table->values[0].property.mapping[1].name = "RAW";
    table->values[0].property.mapping[1].val = BCM_SWITCH_PORT_HEADER_TYPE_RAW;
    table->values[0].property.mapping[2].name = "INJECTED_2_PP";
    table->values[0].property.mapping[2].val = BCM_SWITCH_PORT_HEADER_TYPE_INJECTED_2_PP;
    table->values[0].property.mapping[3].name = "INJECTED_2_PP_JR1_MODE";
    table->values[0].property.mapping[3].val = BCM_SWITCH_PORT_HEADER_TYPE_INJECTED_2_PP_JR1_MODE;
    table->values[0].property.mapping[4].name = "INJECTED_2";
    table->values[0].property.mapping[4].val = BCM_SWITCH_PORT_HEADER_TYPE_INJECTED_2;
    table->values[0].property.mapping[5].name = "INJECTED_2_JR1_MODE";
    table->values[0].property.mapping[5].val = BCM_SWITCH_PORT_HEADER_TYPE_INJECTED_2_JR1_MODE;
    table->values[0].property.mapping[6].name = "INJECTED";
    table->values[0].property.mapping[6].val = BCM_SWITCH_PORT_HEADER_TYPE_INJECTED;
    table->values[0].property.mapping[7].name = "TM";
    table->values[0].property.mapping[7].val = BCM_SWITCH_PORT_HEADER_TYPE_TM;
    table->values[0].property.mapping[8].name = "RCH_0";
    table->values[0].property.mapping[8].val = BCM_SWITCH_PORT_HEADER_TYPE_RCH_0;
    table->values[0].property.mapping[9].name = "RCH_1";
    table->values[0].property.mapping[9].val = BCM_SWITCH_PORT_HEADER_TYPE_RCH_1;
    table->values[0].property.mapping[10].name = "STACKING";
    table->values[0].property.mapping[10].val = BCM_SWITCH_PORT_HEADER_TYPE_STACKING;
    
    table->values[1].property.name = spn_TM_PORT_HEADER_TYPE;
    table->values[1].property.doc =
        "\n"
        "Header type per outgoing port.\n"
        "\n"
    ;
    table->values[1].property.method = dnxc_data_property_method_port_suffix_direct_map;
    table->values[1].property.method_str = "port_suffix_direct_map";
    table->values[1].property.suffix = "out";
    table->values[1].property.nof_mapping = 6;
    DNXC_DATA_ALLOC(table->values[1].property.mapping, dnxc_data_property_map_t, table->values[1].property.nof_mapping, "dnx_data_port_static_add_header_type_t property mapping");

    table->values[1].property.mapping[0].name = "ETH";
    table->values[1].property.mapping[0].val = BCM_SWITCH_PORT_HEADER_TYPE_ETH;
    table->values[1].property.mapping[0].is_default = 1 ;
    table->values[1].property.mapping[1].name = "RAW";
    table->values[1].property.mapping[1].val = BCM_SWITCH_PORT_HEADER_TYPE_RAW;
    table->values[1].property.mapping[2].name = "CPU";
    table->values[1].property.mapping[2].val = BCM_SWITCH_PORT_HEADER_TYPE_CPU;
    table->values[1].property.mapping[3].name = "ENCAP_EXTERNAL_CPU";
    table->values[1].property.mapping[3].val = BCM_SWITCH_PORT_HEADER_TYPE_ENCAP_EXTERNAL_CPU;
    table->values[1].property.mapping[4].name = "INJECTED";
    table->values[1].property.mapping[4].val = BCM_SWITCH_PORT_HEADER_TYPE_INJECTED;
    table->values[1].property.mapping[5].name = "STACKING";
    table->values[1].property.mapping[5].val = BCM_SWITCH_PORT_HEADER_TYPE_STACKING;
    
    table->values[2].property.name = spn_TM_PORT_HEADER_TYPE;
    table->values[2].property.doc =
        "\n"
        "Header type per port.\n"
        "\n"
    ;
    table->values[2].property.method = dnxc_data_property_method_port_direct_map;
    table->values[2].property.method_str = "port_direct_map";
    table->values[2].property.nof_mapping = 3;
    DNXC_DATA_ALLOC(table->values[2].property.mapping, dnxc_data_property_map_t, table->values[2].property.nof_mapping, "dnx_data_port_static_add_header_type_t property mapping");

    table->values[2].property.mapping[0].name = "ETH";
    table->values[2].property.mapping[0].val = BCM_SWITCH_PORT_HEADER_TYPE_ETH;
    table->values[2].property.mapping[0].is_default = 1 ;
    table->values[2].property.mapping[1].name = "RAW";
    table->values[2].property.mapping[1].val = BCM_SWITCH_PORT_HEADER_TYPE_RAW;
    table->values[2].property.mapping[2].name = "STACKING";
    table->values[2].property.mapping[2].val = BCM_SWITCH_PORT_HEADER_TYPE_STACKING;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));

    for (port_index = 0; port_index < table->keys[0].size; port_index++)
    {
        data = (dnx_data_port_static_add_header_type_t *) dnxc_data_mgmt_table_data_get(unit, table, port_index, 0);
        SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &table->values[0].property, port_index, &data->header_type_in));
    }
    for (port_index = 0; port_index < table->keys[0].size; port_index++)
    {
        data = (dnx_data_port_static_add_header_type_t *) dnxc_data_mgmt_table_data_get(unit, table, port_index, 0);
        SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &table->values[1].property, port_index, &data->header_type_out));
    }
    for (port_index = 0; port_index < table->keys[0].size; port_index++)
    {
        data = (dnx_data_port_static_add_header_type_t *) dnxc_data_mgmt_table_data_get(unit, table, port_index, 0);
        SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &table->values[2].property, port_index, &data->header_type));
    }
    
    table->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_port_static_add_erp_exist_set(
    int unit)
{
    int core_index;
    dnx_data_port_static_add_erp_exist_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_port;
    int submodule_index = dnx_data_port_submodule_static_add;
    int table_index = dnx_data_port_static_add_table_erp_exist;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = dnx_data_device.general.nof_cores_get(unit);
    table->info_get.key_size[0] = dnx_data_device.general.nof_cores_get(unit);

    
    table->values[0].default_val = "0";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_port_static_add_erp_exist_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_port_static_add_table_erp_exist");

    
    default_data = (dnx_data_port_static_add_erp_exist_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->exist = 0;
    
    for (core_index = 0; core_index < table->keys[0].size; core_index++)
    {
        data = (dnx_data_port_static_add_erp_exist_t *) dnxc_data_mgmt_table_data_get(unit, table, core_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    table->property.name = "ucode_port";
    table->property.doc =
        "\n"
        "Is ERP port defined in config file per core\n"
        "\n"
    ;
    table->property.method = dnxc_data_property_method_custom;
    table->property.method_str = "custom";

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));

    for (core_index = 0; core_index < table->keys[0].size; core_index++)
    {
        data = (dnx_data_port_static_add_erp_exist_t *) dnxc_data_mgmt_table_data_get(unit, table, core_index, 0);
        SHR_IF_ERR_EXIT(dnx_data_property_port_static_add_erp_exist_read(unit, core_index, data));
    }
    
    table->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_port_static_add_base_flexe_instance_set(
    int unit)
{
    int port_index;
    dnx_data_port_static_add_base_flexe_instance_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_port;
    int submodule_index = dnx_data_port_submodule_static_add;
    int table_index = dnx_data_port_static_add_table_base_flexe_instance;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->flags |= DNXC_DATA_F_INIT_ONLY;

    
    table->keys[0].size = dnx_data_port.general.fabric_port_base_get(unit);
    table->info_get.key_size[0] = dnx_data_port.general.fabric_port_base_get(unit);

    
    table->values[0].default_val = "-1";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_port_static_add_base_flexe_instance_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_port_static_add_table_base_flexe_instance");

    
    default_data = (dnx_data_port_static_add_base_flexe_instance_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->val = -1;
    
    for (port_index = 0; port_index < table->keys[0].size; port_index++)
    {
        data = (dnx_data_port_static_add_base_flexe_instance_t *) dnxc_data_mgmt_table_data_get(unit, table, port_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    table->values[0].property.name = spn_PORT_BASE_FLEXE_INSTANCE;
    table->values[0].property.doc =
        "\n"
        "The base FlexE instance ID for FlexE physical port.\n"
        "-1 means the instance ID is allocated by SW.\n"
        "port_base_flexe_instance_<port>=#instance_id#\n"
        "For 50G FlexE PHY, valid values are 0 ~ 7.\n"
        "For 100G FlexE PHY, valid values are 0/2/4/6.\n"
        "For 200G FlexE PHY, valid values are 0/4.\n"
        "For 400G FlexE PHY, valid values are 0.\n"
        "\n"
    ;
    table->values[0].property.method = dnxc_data_property_method_port_range_signed;
    table->values[0].property.method_str = "port_range_signed";
    table->values[0].property.range_min = -1;
    table->values[0].property.range_max = 7;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));

    for (port_index = 0; port_index < table->keys[0].size; port_index++)
    {
        data = (dnx_data_port_static_add_base_flexe_instance_t *) dnxc_data_mgmt_table_data_get(unit, table, port_index, 0);
        table->values[0].property.label = DNXC_DATA_LABEL_FLEXE;
        SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &table->values[0].property, port_index, &data->val));
    }
    
    table->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}





static shr_error_e
jr2_a0_dnx_data_port_general_unrestricted_portmod_pll_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_port;
    int submodule_index = dnx_data_port_submodule_general;
    int feature_index = dnx_data_port_general_unrestricted_portmod_pll;
    SHR_FUNC_INIT_VARS(unit);

    feature = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].features[feature_index];
    
    feature->default_data = 0;

    
    feature->data = 0;

    
    feature->flags |= DNXC_DATA_F_SUPPORTED;
    
    feature->property.name = spn_CUSTOM_FEATURE;
    feature->property.doc = 
        "\n"
        "disable forcing using single PLL\n"
        "is not recommended as allows buggy configuration\n"
        "\n"
    ;
    feature->property.method = dnxc_data_property_method_suffix_enable;
    feature->property.method_str = "suffix_enable";
    feature->property.suffix = "disable_force_single_pll";
    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &feature->property, -1, &feature->data));
    
    feature->flags |= (DNXC_DATA_F_PROPERTY);
    feature->flags |= (DNXC_DATA_F_PROPERTY_INTERNAL);


    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_feature_value_set(unit, feature));

exit:
    SHR_FUNC_EXIT;
}



static shr_error_e
jr2_a0_dnx_data_port_general_erp_tm_port_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_port;
    int submodule_index = dnx_data_port_submodule_general;
    int define_index = dnx_data_port_general_define_erp_tm_port;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 255;

    
    define->data = 255;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_port_general_max_nof_channels_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_port;
    int submodule_index = dnx_data_port_submodule_general;
    int define_index = dnx_data_port_general_define_max_nof_channels;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 256;

    
    define->data = 256;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_port_general_fabric_phys_offset_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_port;
    int submodule_index = dnx_data_port_submodule_general;
    int define_index = dnx_data_port_general_define_fabric_phys_offset;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = dnx_data_nif.phys.nof_phys_get(unit);

    
    define->data = dnx_data_nif.phys.nof_phys_get(unit);

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_port_general_pp_dsp_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_port;
    int submodule_index = dnx_data_port_submodule_general;
    int define_index = dnx_data_port_general_define_pp_dsp_size;
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
jr2_a0_dnx_data_port_general_ingress_vlan_domain_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_port;
    int submodule_index = dnx_data_port_submodule_general;
    int define_index = dnx_data_port_general_define_ingress_vlan_domain_size;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 9;

    
    define->data = 9;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_port_general_vlan_domain_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_port;
    int submodule_index = dnx_data_port_submodule_general;
    int define_index = dnx_data_port_general_define_vlan_domain_size;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 9;

    
    define->data = 9;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_port_general_ffc_instruction_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_port;
    int submodule_index = dnx_data_port_submodule_general;
    int define_index = dnx_data_port_general_define_ffc_instruction_size;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 19;

    
    define->data = 19;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_port_general_max_first_header_size_to_skip_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_port;
    int submodule_index = dnx_data_port_submodule_general;
    int define_index = dnx_data_port_general_define_max_first_header_size_to_skip;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 63;

    
    define->data = 63;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_port_general_prt_ffc_width_start_offset_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_port;
    int submodule_index = dnx_data_port_submodule_general;
    int define_index = dnx_data_port_general_define_prt_ffc_width_start_offset;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 5;

    
    define->data = 5;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_port_general_prt_ffc_start_input_offset_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_port;
    int submodule_index = dnx_data_port_submodule_general;
    int define_index = dnx_data_port_general_define_prt_ffc_start_input_offset;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 10;

    
    define->data = 10;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_port_general_nof_tm_ports_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_port;
    int submodule_index = dnx_data_port_submodule_general;
    int define_index = dnx_data_port_general_define_nof_tm_ports;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 256;

    
    define->data = 256;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_port_general_nof_tm_ports_per_fap_id_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_port;
    int submodule_index = dnx_data_port_submodule_general;
    int define_index = dnx_data_port_general_define_nof_tm_ports_per_fap_id;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 256;

    
    define->data = 256;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_port_general_nof_pp_ports_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_port;
    int submodule_index = dnx_data_port_submodule_general;
    int define_index = dnx_data_port_general_define_nof_pp_ports;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 256;

    
    define->data = 256;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_port_general_reserved_tm_port_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_port;
    int submodule_index = dnx_data_port_submodule_general;
    int define_index = dnx_data_port_general_define_reserved_tm_port;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 254;

    
    define->data = 254;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_port_general_nof_vlan_membership_if_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_port;
    int submodule_index = dnx_data_port_submodule_general;
    int define_index = dnx_data_port_general_define_nof_vlan_membership_if;
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
jr2_a0_dnx_data_port_general_max_vlan_membership_if_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_port;
    int submodule_index = dnx_data_port_submodule_general;
    int define_index = dnx_data_port_general_define_max_vlan_membership_if;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 511;

    
    define->data = 511;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_port_general_fabric_port_base_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_port;
    int submodule_index = dnx_data_port_submodule_general;
    int define_index = dnx_data_port_general_define_fabric_port_base;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 256;

    
    define->data = 256;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = "fabric_logical_port_base";
    define->property.doc = 
        "\n"
        "By default fabric logical ports are 256..(256 + nof links)\n"
        "To extend the logical_port_num range so that ports in range 0...511 can be used, use the following SOC property: fabric_logical_port_base=512.\n"
        "\n"
    ;
    define->property.method = dnxc_data_property_method_custom;
    define->property.method_str = "custom";

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));

    SHR_IF_ERR_EXIT(dnx_data_property_port_general_fabric_port_base_read(unit, (uint32 *) &define->data));
    
    define->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_port_general_pp_port_bits_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_port;
    int submodule_index = dnx_data_port_submodule_general;
    int define_index = dnx_data_port_general_define_pp_port_bits_size;
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
jr2_a0_dnx_data_port_general_nof_port_bits_in_pp_bus_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_port;
    int submodule_index = dnx_data_port_submodule_general;
    int define_index = dnx_data_port_general_define_nof_port_bits_in_pp_bus;
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
jr2_a0_dnx_data_port_general_prt_configuration_ptch2_default_index_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_port;
    int submodule_index = dnx_data_port_submodule_general;
    int define_index = dnx_data_port_general_define_prt_configuration_ptch2_default_index;
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
jr2_a0_dnx_data_port_general_prt_configuration_ptch1_default_index_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_port;
    int submodule_index = dnx_data_port_submodule_general;
    int define_index = dnx_data_port_general_define_prt_configuration_ptch1_default_index;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 1;

    
    define->data = 1;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_port_general_prt_configuration_eth_default_index_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_port;
    int submodule_index = dnx_data_port_submodule_general;
    int define_index = dnx_data_port_general_define_prt_configuration_eth_default_index;
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
jr2_a0_dnx_data_port_general_prt_configuration_set(
    int unit)
{
    int index_index;
    dnx_data_port_general_prt_configuration_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_port;
    int submodule_index = dnx_data_port_submodule_general;
    int table_index = dnx_data_port_general_table_prt_configuration;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = 13;
    table->info_get.key_size[0] = 13;

    
    table->values[0].default_val = "0";
    table->values[1].default_val = "0";
    table->values[2].default_val = "0";
    table->values[3].default_val = "0";
    table->values[4].default_val = "0";
    table->values[5].default_val = "0";
    table->values[6].default_val = "0";
    table->values[7].default_val = "0";
    table->values[8].default_val = "0";
    table->values[9].default_val = "0";
    table->values[10].default_val = "0";
    table->values[11].default_val = "0";
    table->values[12].default_val = "0";
    table->values[13].default_val = "0";
    table->values[14].default_val = "0";
    table->values[15].default_val = "0";
    table->values[16].default_val = "0";
    table->values[17].default_val = "0";
    table->values[18].default_val = "0";
    table->values[19].default_val = "0";
    table->values[20].default_val = "0";
    table->values[21].default_val = "0";
    table->values[22].default_val = "0";
    table->values[23].default_val = "0";
    table->values[24].default_val = "0";
    table->values[25].default_val = "0";
    table->values[26].default_val = "0";
    table->values[27].default_val = "0";
    table->values[28].default_val = "0";
    table->values[29].default_val = "0";
    table->values[30].default_val = "0";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_port_general_prt_configuration_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_port_general_table_prt_configuration");

    
    default_data = (dnx_data_port_general_prt_configuration_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->prt_recycle_profile = 0;
    default_data->port_termination_ptc_profile = 0;
    default_data->prt_qual_mode = 0;
    default_data->layer_offset = 0;
    default_data->context_select = 0;
    default_data->kbr_valid_bitmap_4 = 0;
    default_data->kbr_valid_bitmap_3 = 0;
    default_data->kbr_valid_bitmap_2 = 0;
    default_data->kbr_valid_bitmap_1 = 0;
    default_data->kbr_valid_bitmap_0 = 0;
    default_data->ffc_5_offset = 0;
    default_data->ffc_5_width = 0;
    default_data->ffc_5_input_offset = 0;
    default_data->ffc_4_offset = 0;
    default_data->ffc_4_width = 0;
    default_data->ffc_4_input_offset = 0;
    default_data->ffc_3_offset = 0;
    default_data->ffc_3_width = 0;
    default_data->ffc_3_input_offset = 0;
    default_data->ffc_2_offset = 0;
    default_data->ffc_2_width = 0;
    default_data->ffc_2_input_offset = 0;
    default_data->ffc_1_offset = 0;
    default_data->ffc_1_width = 0;
    default_data->ffc_1_input_offset = 0;
    default_data->ffc_0_offset = 0;
    default_data->ffc_0_width = 0;
    default_data->ffc_0_input_offset = 0;
    default_data->src_sys_port_prt_mode = 0;
    default_data->pp_port_en = 0;
    default_data->tcam_mode_full_en = 0;
    
    for (index_index = 0; index_index < table->keys[0].size; index_index++)
    {
        data = (dnx_data_port_general_prt_configuration_t *) dnxc_data_mgmt_table_data_get(unit, table, index_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (0 < table->keys[0].size)
    {
        data = (dnx_data_port_general_prt_configuration_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
        data->prt_recycle_profile = DBAL_ENUM_FVAL_RECYCLE_CMD_PRT_PROFILE_NONE;
        data->port_termination_ptc_profile = DBAL_ENUM_FVAL_PORT_TERMINATION_PTC_PROFILE_PTCH2;
        data->prt_qual_mode = 1;
        data->layer_offset = 2;
        data->context_select = 1;
        data->kbr_valid_bitmap_4 = 2;
        data->kbr_valid_bitmap_3 = 4;
        data->kbr_valid_bitmap_2 = 0;
        data->kbr_valid_bitmap_1 = 1;
        data->ffc_5_offset = 0;
        data->ffc_5_width = 0;
        data->ffc_5_input_offset = 0;
        data->ffc_4_offset = 0;
        data->ffc_4_width = 0;
        data->ffc_4_input_offset = 0;
        data->ffc_3_offset = 0;
        data->ffc_3_width = 0;
        data->ffc_3_input_offset = 0;
        data->ffc_2_offset = 0;
        data->ffc_2_width = 0;
        data->ffc_2_input_offset = 255;
        data->ffc_1_offset = 0;
        data->ffc_1_width = 3;
        data->ffc_1_input_offset = 252;
        data->ffc_0_offset = 0;
        data->ffc_0_width = 7;
        data->ffc_0_input_offset = 240;
        data->src_sys_port_prt_mode = 0;
        data->pp_port_en = 0;
        data->tcam_mode_full_en = 1;
    }
    if (1 < table->keys[0].size)
    {
        data = (dnx_data_port_general_prt_configuration_t *) dnxc_data_mgmt_table_data_get(unit, table, 1, 0);
        data->prt_recycle_profile = DBAL_ENUM_FVAL_RECYCLE_CMD_PRT_PROFILE_NONE;
        data->port_termination_ptc_profile = DBAL_ENUM_FVAL_PORT_TERMINATION_PTC_PROFILE_PTCH1;
        data->prt_qual_mode = 1;
        data->layer_offset = 3;
        data->context_select = 1;
        data->kbr_valid_bitmap_4 = 2;
        data->kbr_valid_bitmap_3 = 4;
        data->kbr_valid_bitmap_2 = 32;
        data->kbr_valid_bitmap_1 = 9;
        data->kbr_valid_bitmap_0 = 16;
        data->ffc_5_offset = 0;
        data->ffc_5_width = 15;
        data->ffc_5_input_offset = 232;
        data->ffc_4_offset = 2;
        data->ffc_4_width = 0;
        data->ffc_4_input_offset = 248;
        data->ffc_3_offset = 16;
        data->ffc_3_width = 0;
        data->ffc_3_input_offset = 321;
        data->ffc_2_offset = 0;
        data->ffc_2_width = 0;
        data->ffc_2_input_offset = 255;
        data->ffc_1_offset = 0;
        data->ffc_1_width = 3;
        data->ffc_1_input_offset = 252;
        data->ffc_0_offset = 0;
        data->ffc_0_width = 15;
        data->ffc_0_input_offset = 232;
        data->src_sys_port_prt_mode = 2;
        data->pp_port_en = 1;
        data->tcam_mode_full_en = 0;
    }
    if (2 < table->keys[0].size)
    {
        data = (dnx_data_port_general_prt_configuration_t *) dnxc_data_mgmt_table_data_get(unit, table, 2, 0);
        data->prt_recycle_profile = DBAL_ENUM_FVAL_RECYCLE_CMD_PRT_PROFILE_NONE;
        data->port_termination_ptc_profile = DBAL_ENUM_FVAL_PORT_TERMINATION_PTC_PROFILE_ETHERNET;
        data->prt_qual_mode = 0;
        data->layer_offset = 0;
        data->context_select = 0;
        data->kbr_valid_bitmap_4 = 0;
        data->kbr_valid_bitmap_3 = 0;
        data->kbr_valid_bitmap_2 = 16;
        data->kbr_valid_bitmap_1 = 1;
        data->ffc_5_offset = 0;
        data->ffc_5_width = 0;
        data->ffc_5_input_offset = 0;
        data->ffc_4_offset = 0;
        data->ffc_4_width = 15;
        data->ffc_4_input_offset = 292;
        data->ffc_3_offset = 0;
        data->ffc_3_width = 0;
        data->ffc_3_input_offset = 0;
        data->ffc_2_offset = 0;
        data->ffc_2_width = 0;
        data->ffc_2_input_offset = 0;
        data->ffc_1_offset = 0;
        data->ffc_1_width = 0;
        data->ffc_1_input_offset = 0;
        data->ffc_0_offset = 0;
        data->ffc_0_width = 7;
        data->ffc_0_input_offset = 308;
        data->src_sys_port_prt_mode = 2;
        data->pp_port_en = 0;
        data->tcam_mode_full_en = 1;
    }
    if (3 < table->keys[0].size)
    {
        data = (dnx_data_port_general_prt_configuration_t *) dnxc_data_mgmt_table_data_get(unit, table, 3, 0);
        data->prt_recycle_profile = DBAL_ENUM_FVAL_RECYCLE_CMD_PRT_PROFILE_PTCH2;
        data->port_termination_ptc_profile = DBAL_ENUM_FVAL_PORT_TERMINATION_PTC_PROFILE_ETHERNET;
        data->prt_qual_mode = 1;
        data->layer_offset = 2;
        data->context_select = 1;
        data->kbr_valid_bitmap_4 = 2;
        data->kbr_valid_bitmap_3 = 4;
        data->kbr_valid_bitmap_2 = 16;
        data->kbr_valid_bitmap_1 = 1;
        data->ffc_5_offset = 0;
        data->ffc_5_width = 0;
        data->ffc_5_input_offset = 0;
        data->ffc_4_offset = 0;
        data->ffc_4_width = 15;
        data->ffc_4_input_offset = 292;
        data->ffc_3_offset = 0;
        data->ffc_3_width = 0;
        data->ffc_3_input_offset = 0;
        data->ffc_2_offset = 0;
        data->ffc_2_width = 0;
        data->ffc_2_input_offset = 255;
        data->ffc_1_offset = 0;
        data->ffc_1_width = 3;
        data->ffc_1_input_offset = 252;
        data->ffc_0_offset = 0;
        data->ffc_0_width = 7;
        data->ffc_0_input_offset = 240;
        data->src_sys_port_prt_mode = 2;
        data->pp_port_en = 0;
        data->tcam_mode_full_en = 1;
    }
    if (4 < table->keys[0].size)
    {
        data = (dnx_data_port_general_prt_configuration_t *) dnxc_data_mgmt_table_data_get(unit, table, 4, 0);
        data->prt_recycle_profile = DBAL_ENUM_FVAL_RECYCLE_CMD_PRT_PROFILE_802_1BR_TYPE1;
        data->port_termination_ptc_profile = DBAL_ENUM_FVAL_PORT_TERMINATION_PTC_PROFILE_ETHERNET;
        data->prt_qual_mode = 0;
        data->layer_offset = 8;
        data->context_select = 0;
        data->kbr_valid_bitmap_4 = 0;
        data->kbr_valid_bitmap_3 = 0;
        data->kbr_valid_bitmap_2 = 16;
        data->kbr_valid_bitmap_1 = 1;
        data->ffc_5_offset = 0;
        data->ffc_5_width = 0;
        data->ffc_5_input_offset = 0;
        data->ffc_4_offset = 0;
        data->ffc_4_width = 15;
        data->ffc_4_input_offset = 292;
        data->ffc_3_offset = 0;
        data->ffc_3_width = 0;
        data->ffc_3_input_offset = 0;
        data->ffc_2_offset = 0;
        data->ffc_2_width = 0;
        data->ffc_2_input_offset = 0;
        data->ffc_1_offset = 0;
        data->ffc_1_width = 0;
        data->ffc_1_input_offset = 0;
        data->ffc_0_offset = 0;
        data->ffc_0_width = 7;
        data->ffc_0_input_offset = 308;
        data->src_sys_port_prt_mode = 2;
        data->pp_port_en = 0;
        data->tcam_mode_full_en = 1;
    }
    if (5 < table->keys[0].size)
    {
        data = (dnx_data_port_general_prt_configuration_t *) dnxc_data_mgmt_table_data_get(unit, table, 5, 0);
        data->prt_recycle_profile = DBAL_ENUM_FVAL_RECYCLE_CMD_PRT_PROFILE_802_1BR_TYPE2;
        data->port_termination_ptc_profile = DBAL_ENUM_FVAL_PORT_TERMINATION_PTC_PROFILE_ETHERNET;
        data->prt_qual_mode = 0;
        data->layer_offset = 8;
        data->context_select = 0;
        data->kbr_valid_bitmap_4 = 0;
        data->kbr_valid_bitmap_3 = 0;
        data->kbr_valid_bitmap_2 = 16;
        data->kbr_valid_bitmap_1 = 1;
        data->ffc_5_offset = 0;
        data->ffc_5_width = 0;
        data->ffc_5_input_offset = 0;
        data->ffc_4_offset = 0;
        data->ffc_4_width = 15;
        data->ffc_4_input_offset = 292;
        data->ffc_3_offset = 0;
        data->ffc_3_width = 0;
        data->ffc_3_input_offset = 0;
        data->ffc_2_offset = 0;
        data->ffc_2_width = 0;
        data->ffc_2_input_offset = 0;
        data->ffc_1_offset = 0;
        data->ffc_1_width = 0;
        data->ffc_1_input_offset = 0;
        data->ffc_0_offset = 0;
        data->ffc_0_width = 7;
        data->ffc_0_input_offset = 308;
        data->src_sys_port_prt_mode = 2;
        data->pp_port_en = 0;
        data->tcam_mode_full_en = 1;
    }
    if (6 < table->keys[0].size)
    {
        data = (dnx_data_port_general_prt_configuration_t *) dnxc_data_mgmt_table_data_get(unit, table, 6, 0);
        data->prt_recycle_profile = DBAL_ENUM_FVAL_RECYCLE_CMD_PRT_PROFILE_NONE;
        data->port_termination_ptc_profile = DBAL_ENUM_FVAL_PORT_TERMINATION_PTC_PROFILE_RCH;
        data->prt_qual_mode = 0;
        data->layer_offset = 0;
        data->context_select = 1;
        data->kbr_valid_bitmap_4 = 0;
        data->kbr_valid_bitmap_3 = 32;
        data->kbr_valid_bitmap_2 = 4;
        data->kbr_valid_bitmap_1 = 1;
        data->ffc_5_offset = 0;
        data->ffc_5_width = 4;
        data->ffc_5_input_offset = 316;
        data->ffc_4_offset = 0;
        data->ffc_4_width = 0;
        data->ffc_4_input_offset = 0;
        data->ffc_3_offset = 0;
        data->ffc_3_width = 0;
        data->ffc_3_input_offset = 0;
        data->ffc_2_offset = 0;
        data->ffc_2_width = 15;
        data->ffc_2_input_offset = 200;
        data->ffc_1_offset = 0;
        data->ffc_1_width = 0;
        data->ffc_1_input_offset = 0;
        data->ffc_0_offset = 0;
        data->ffc_0_width = 15;
        data->ffc_0_input_offset = 200;
        data->src_sys_port_prt_mode = 2;
        data->pp_port_en = 1;
        data->tcam_mode_full_en = 0;
    }
    if (7 < table->keys[0].size)
    {
        data = (dnx_data_port_general_prt_configuration_t *) dnxc_data_mgmt_table_data_get(unit, table, 7, 0);
        data->prt_recycle_profile = DBAL_ENUM_FVAL_RECYCLE_CMD_PRT_PROFILE_COE;
        data->port_termination_ptc_profile = DBAL_ENUM_FVAL_PORT_TERMINATION_PTC_PROFILE_COE;
        data->prt_qual_mode = 0;
        data->layer_offset = 4;
        data->context_select = 0;
        data->kbr_valid_bitmap_4 = 0;
        data->kbr_valid_bitmap_3 = 0;
        data->kbr_valid_bitmap_2 = 0;
        data->kbr_valid_bitmap_1 = 11;
        data->ffc_5_offset = 0;
        data->ffc_5_width = 0;
        data->ffc_5_input_offset = 0;
        data->ffc_4_offset = 0;
        data->ffc_4_width = 0;
        data->ffc_4_input_offset = 0;
        data->ffc_3_offset = 14;
        data->ffc_3_width = 2;
        data->ffc_3_input_offset = 321;
        data->ffc_2_offset = 0;
        data->ffc_2_width = 0;
        data->ffc_2_input_offset = 0;
        data->ffc_1_offset = 6;
        data->ffc_1_width = 7;
        data->ffc_1_input_offset = 308;
        data->ffc_0_offset = 0;
        data->ffc_0_width = 5;
        data->ffc_0_input_offset = 224;
        data->src_sys_port_prt_mode = 1;
        data->pp_port_en = 1;
        data->tcam_mode_full_en = 0;
    }
    if (8 < table->keys[0].size)
    {
        data = (dnx_data_port_general_prt_configuration_t *) dnxc_data_mgmt_table_data_get(unit, table, 8, 0);
        data->prt_recycle_profile = DBAL_ENUM_FVAL_RECYCLE_CMD_PRT_PROFILE_PTCH2;
        data->port_termination_ptc_profile = DBAL_ENUM_FVAL_PORT_TERMINATION_PTC_PROFILE_RCH;
        data->prt_qual_mode = 1;
        data->layer_offset = 2;
        data->context_select = 1;
        data->kbr_valid_bitmap_4 = 2;
        data->kbr_valid_bitmap_3 = 4;
        data->kbr_valid_bitmap_2 = 0;
        data->kbr_valid_bitmap_1 = 1;
        data->ffc_5_offset = 0;
        data->ffc_5_width = 0;
        data->ffc_5_input_offset = 0;
        data->ffc_4_offset = 0;
        data->ffc_4_width = 0;
        data->ffc_4_input_offset = 0;
        data->ffc_3_offset = 0;
        data->ffc_3_width = 4;
        data->ffc_3_input_offset = 280;
        data->ffc_2_offset = 0;
        data->ffc_2_width = 15;
        data->ffc_2_input_offset = 128;
        data->ffc_1_offset = 0;
        data->ffc_1_width = 3;
        data->ffc_1_input_offset = 252;
        data->ffc_0_offset = 0;
        data->ffc_0_width = 7;
        data->ffc_0_input_offset = 240;
        data->src_sys_port_prt_mode = 0;
        data->pp_port_en = 0;
        data->tcam_mode_full_en = 1;
    }
    if (9 < table->keys[0].size)
    {
        data = (dnx_data_port_general_prt_configuration_t *) dnxc_data_mgmt_table_data_get(unit, table, 9, 0);
        data->prt_recycle_profile = DBAL_ENUM_FVAL_RECYCLE_CMD_PRT_PROFILE_NONE;
        data->port_termination_ptc_profile = DBAL_ENUM_FVAL_PORT_TERMINATION_PTC_PROFILE_PTCH2_JR1;
        data->prt_qual_mode = 1;
        data->layer_offset = 2;
        data->context_select = 1;
        data->kbr_valid_bitmap_4 = 2;
        data->kbr_valid_bitmap_3 = 12;
        data->kbr_valid_bitmap_2 = 0;
        data->kbr_valid_bitmap_1 = 1;
        data->ffc_5_offset = 0;
        data->ffc_5_width = 0;
        data->ffc_5_input_offset = 0;
        data->ffc_4_offset = 0;
        data->ffc_4_width = 0;
        data->ffc_4_input_offset = 0;
        data->ffc_3_offset = 1;
        data->ffc_3_width = 1;
        data->ffc_3_input_offset = 321;
        data->ffc_2_offset = 0;
        data->ffc_2_width = 0;
        data->ffc_2_input_offset = 255;
        data->ffc_1_offset = 0;
        data->ffc_1_width = 3;
        data->ffc_1_input_offset = 252;
        data->ffc_0_offset = 0;
        data->ffc_0_width = 7;
        data->ffc_0_input_offset = 240;
        data->src_sys_port_prt_mode = 0;
        data->pp_port_en = 0;
        data->tcam_mode_full_en = 1;
    }
    if (10 < table->keys[0].size)
    {
        data = (dnx_data_port_general_prt_configuration_t *) dnxc_data_mgmt_table_data_get(unit, table, 10, 0);
        data->prt_recycle_profile = DBAL_ENUM_FVAL_RECYCLE_CMD_PRT_PROFILE_NONE;
        data->port_termination_ptc_profile = DBAL_ENUM_FVAL_PORT_TERMINATION_PTC_PROFILE_ITMH;
        data->prt_qual_mode = 0;
        data->layer_offset = 0;
        data->context_select = 0;
        data->kbr_valid_bitmap_4 = 0;
        data->kbr_valid_bitmap_3 = 0;
        data->kbr_valid_bitmap_2 = 0;
        data->kbr_valid_bitmap_1 = 1;
        data->ffc_5_offset = 0;
        data->ffc_5_width = 0;
        data->ffc_5_input_offset = 0;
        data->ffc_4_offset = 0;
        data->ffc_4_width = 0;
        data->ffc_4_input_offset = 0;
        data->ffc_3_offset = 0;
        data->ffc_3_width = 0;
        data->ffc_3_input_offset = 0;
        data->ffc_2_offset = 0;
        data->ffc_2_width = 0;
        data->ffc_2_input_offset = 0;
        data->ffc_1_offset = 0;
        data->ffc_1_width = 0;
        data->ffc_1_input_offset = 0;
        data->ffc_0_offset = 0;
        data->ffc_0_width = 7;
        data->ffc_0_input_offset = 308;
        data->src_sys_port_prt_mode = 0;
        data->pp_port_en = 0;
        data->tcam_mode_full_en = 1;
    }
    if (11 < table->keys[0].size)
    {
        data = (dnx_data_port_general_prt_configuration_t *) dnxc_data_mgmt_table_data_get(unit, table, 11, 0);
        data->prt_recycle_profile = DBAL_ENUM_FVAL_RECYCLE_CMD_PRT_PROFILE_NONE;
        data->port_termination_ptc_profile = DBAL_ENUM_FVAL_PORT_TERMINATION_PTC_PROFILE_RAW;
        data->prt_qual_mode = 0;
        data->layer_offset = 0;
        data->context_select = 0;
        data->kbr_valid_bitmap_4 = 0;
        data->kbr_valid_bitmap_3 = 0;
        data->kbr_valid_bitmap_2 = 16;
        data->kbr_valid_bitmap_1 = 1;
        data->ffc_5_offset = 0;
        data->ffc_5_width = 0;
        data->ffc_5_input_offset = 0;
        data->ffc_4_offset = 0;
        data->ffc_4_width = 15;
        data->ffc_4_input_offset = 292;
        data->ffc_3_offset = 0;
        data->ffc_3_width = 0;
        data->ffc_3_input_offset = 0;
        data->ffc_2_offset = 0;
        data->ffc_2_width = 0;
        data->ffc_2_input_offset = 0;
        data->ffc_1_offset = 0;
        data->ffc_1_width = 0;
        data->ffc_1_input_offset = 0;
        data->ffc_0_offset = 0;
        data->ffc_0_width = 7;
        data->ffc_0_input_offset = 308;
        data->src_sys_port_prt_mode = 2;
        data->pp_port_en = 0;
        data->tcam_mode_full_en = 1;
    }
    if (12 < table->keys[0].size)
    {
        data = (dnx_data_port_general_prt_configuration_t *) dnxc_data_mgmt_table_data_get(unit, table, 12, 0);
        data->prt_recycle_profile = DBAL_ENUM_FVAL_RECYCLE_CMD_PRT_PROFILE_COE;
        data->port_termination_ptc_profile = DBAL_ENUM_FVAL_PORT_TERMINATION_PTC_PROFILE_PON;
        data->prt_qual_mode = 0;
        data->layer_offset = 4;
        data->context_select = 0;
        data->kbr_valid_bitmap_4 = 0;
        data->kbr_valid_bitmap_3 = 0;
        data->kbr_valid_bitmap_2 = 0;
        data->kbr_valid_bitmap_1 = 7;
        data->ffc_5_offset = 0;
        data->ffc_5_width = 0;
        data->ffc_5_input_offset = 0;
        data->ffc_4_offset = 0;
        data->ffc_4_width = 0;
        data->ffc_4_input_offset = 0;
        data->ffc_3_offset = 0;
        data->ffc_3_width = 0;
        data->ffc_3_input_offset = 0;
        data->ffc_2_offset = 13;
        data->ffc_2_width = 3;
        data->ffc_2_input_offset = 292;
        data->ffc_1_offset = 3;
        data->ffc_1_width = 7;
        data->ffc_1_input_offset = 308;
        data->ffc_0_offset = 0;
        data->ffc_0_width = 2;
        data->ffc_0_input_offset = 236;
        data->src_sys_port_prt_mode = 0;
        data->pp_port_en = 1;
        data->tcam_mode_full_en = 0;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_port_general_first_header_size_set(
    int unit)
{
    int port_index;
    dnx_data_port_general_first_header_size_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_port;
    int submodule_index = dnx_data_port_submodule_general;
    int table_index = dnx_data_port_general_table_first_header_size;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = dnx_data_port.general.fabric_port_base_get(unit);
    table->info_get.key_size[0] = dnx_data_port.general.fabric_port_base_get(unit);

    
    table->values[0].default_val = "0";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_port_general_first_header_size_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_port_general_table_first_header_size");

    
    default_data = (dnx_data_port_general_first_header_size_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->first_header_size = 0;
    
    for (port_index = 0; port_index < table->keys[0].size; port_index++)
    {
        data = (dnx_data_port_general_first_header_size_t *) dnxc_data_mgmt_table_data_get(unit, table, port_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    table->values[0].property.name = spn_FIRST_HEADER_SIZE;
    table->values[0].property.doc =
        "\n"
        "Indicates the size (in Bytes) of a first header to skip before the major header at ingress (e.g. Ethernet).\n"
        "\n"
    ;
    table->values[0].property.method = dnxc_data_property_method_port_range_signed;
    table->values[0].property.method_str = "port_range_signed";
    table->values[0].property.range_min = 0;
    table->values[0].property.range_max = 63;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));

    for (port_index = 0; port_index < table->keys[0].size; port_index++)
    {
        data = (dnx_data_port_general_first_header_size_t *) dnxc_data_mgmt_table_data_get(unit, table, port_index, 0);
        SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &table->values[0].property, port_index, &data->first_header_size));
    }
    
    table->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}






static shr_error_e
jr2_a0_dnx_data_port_egress_nof_ifs_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_port;
    int submodule_index = dnx_data_port_submodule_egress;
    int define_index = dnx_data_port_egress_define_nof_ifs;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 64;

    
    define->data = 64;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}








static shr_error_e
jr2_a0_dnx_data_port_imb_imb_type_info_set(
    int unit)
{
    int type_index;
    dnx_data_port_imb_imb_type_info_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_port;
    int submodule_index = dnx_data_port_submodule_imb;
    int table_index = dnx_data_port_imb_table_imb_type_info;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = imbDispatchTypeCount;
    table->info_get.key_size[0] = imbDispatchTypeCount;

    
    table->values[0].default_val = "0";
    table->values[1].default_val = "0";
    table->values[2].default_val = "0";
    table->values[3].default_val = "0";
    table->values[4].default_val = "0";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_port_imb_imb_type_info_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_port_imb_table_imb_type_info");

    
    default_data = (dnx_data_port_imb_imb_type_info_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->nof_pms = 0;
    default_data->nof_lanes = 0;
    default_data->nof_lanes_in_pm = 0;
    default_data->nof_memory_entries = 0;
    default_data->rate_per_rmc_bit = 0;
    
    for (type_index = 0; type_index < table->keys[0].size; type_index++)
    {
        data = (dnx_data_port_imb_imb_type_info_t *) dnxc_data_mgmt_table_data_get(unit, table, type_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (imbDispatchTypeImb_cdu < table->keys[0].size)
    {
        data = (dnx_data_port_imb_imb_type_info_t *) dnxc_data_mgmt_table_data_get(unit, table, imbDispatchTypeImb_cdu, 0);
        data->nof_pms = dnx_data_nif.eth.nof_pms_in_cdu_get(unit);
        data->nof_lanes = dnx_data_nif.eth.nof_lanes_in_cdu_get(unit);
        data->nof_lanes_in_pm = dnx_data_nif.eth.nof_cdu_lanes_in_pm_get(unit);
        data->nof_memory_entries = dnx_data_nif.eth.cdu_memory_entries_nof_get(unit);
        data->rate_per_rmc_bit = dnx_data_nif.scheduler.rate_per_cdu_rmc_bit_get(unit);
    }
    if (imbDispatchTypeImb_clu < table->keys[0].size)
    {
        data = (dnx_data_port_imb_imb_type_info_t *) dnxc_data_mgmt_table_data_get(unit, table, imbDispatchTypeImb_clu, 0);
        data->nof_pms = dnx_data_nif.eth.nof_pms_in_clu_get(unit);
        data->nof_lanes = dnx_data_nif.eth.nof_lanes_in_clu_get(unit);
        data->nof_lanes_in_pm = dnx_data_nif.eth.nof_clu_lanes_in_pm_get(unit);
        data->nof_memory_entries = dnx_data_nif.eth.clu_memory_entries_nof_get(unit);
        data->rate_per_rmc_bit = dnx_data_nif.scheduler.rate_per_clu_rmc_bit_get(unit);
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}



shr_error_e
jr2_a0_data_port_attach(
    int unit)
{
    dnxc_data_module_t *module = NULL;
    dnxc_data_submodule_t *submodule = NULL;
    dnxc_data_define_t *define = NULL;
    dnxc_data_feature_t *feature = NULL;
    dnxc_data_table_t *table = NULL;
    int module_index = dnx_data_module_port;
    int submodule_index = -1, data_index = -1;
    SHR_FUNC_INIT_VARS(unit);

    COMPILER_REFERENCE(define);
    COMPILER_REFERENCE(feature);
    COMPILER_REFERENCE(table);
    COMPILER_REFERENCE(submodule);
    COMPILER_REFERENCE(data_index);
    COMPILER_REFERENCE(submodule_index);
    module = &_dnxc_data[unit].modules[module_index];
    
    submodule_index = dnx_data_port_submodule_static_add;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_port_static_add_define_default_speed_for_special_if;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_port_static_add_default_speed_for_special_if_set;
    data_index = dnx_data_port_static_add_define_fabric_fw_load_method;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_port_static_add_fabric_fw_load_method_set;
    data_index = dnx_data_port_static_add_define_fabric_fw_crc_check;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_port_static_add_fabric_fw_crc_check_set;
    data_index = dnx_data_port_static_add_define_fabric_fw_load_verify;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_port_static_add_fabric_fw_load_verify_set;
    data_index = dnx_data_port_static_add_define_nif_fw_load_method;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_port_static_add_nif_fw_load_method_set;
    data_index = dnx_data_port_static_add_define_nif_fw_crc_check;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_port_static_add_nif_fw_crc_check_set;
    data_index = dnx_data_port_static_add_define_nif_fw_load_verify;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_port_static_add_nif_fw_load_verify_set;

    

    
    data_index = dnx_data_port_static_add_table_ucode_port;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_port_static_add_ucode_port_set;
    data_index = dnx_data_port_static_add_table_speed;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_port_static_add_speed_set;
    data_index = dnx_data_port_static_add_table_ext_stat_speed;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_port_static_add_ext_stat_speed_set;
    data_index = dnx_data_port_static_add_table_eth_padding;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_port_static_add_eth_padding_set;
    data_index = dnx_data_port_static_add_table_link_training;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_port_static_add_link_training_set;
    data_index = dnx_data_port_static_add_table_ext_stat_link_training;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_port_static_add_ext_stat_link_training_set;
    data_index = dnx_data_port_static_add_table_fec_type;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_port_static_add_fec_type_set;
    data_index = dnx_data_port_static_add_table_serdes_lane_config;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_port_static_add_serdes_lane_config_set;
    data_index = dnx_data_port_static_add_table_serdes_tx_taps;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_port_static_add_serdes_tx_taps_set;
    data_index = dnx_data_port_static_add_table_ext_stat_global_serdes_tx_taps;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_port_static_add_ext_stat_global_serdes_tx_taps_set;
    data_index = dnx_data_port_static_add_table_ext_stat_lane_serdes_tx_taps;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_port_static_add_ext_stat_lane_serdes_tx_taps_set;
    data_index = dnx_data_port_static_add_table_tx_pam4_precoder;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_port_static_add_tx_pam4_precoder_set;
    data_index = dnx_data_port_static_add_table_lp_tx_precoder;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_port_static_add_lp_tx_precoder_set;
    data_index = dnx_data_port_static_add_table_fabric_quad_info;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_port_static_add_fabric_quad_info_set;
    data_index = dnx_data_port_static_add_table_header_type;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_port_static_add_header_type_set;
    data_index = dnx_data_port_static_add_table_erp_exist;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_port_static_add_erp_exist_set;
    data_index = dnx_data_port_static_add_table_base_flexe_instance;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_port_static_add_base_flexe_instance_set;
    
    submodule_index = dnx_data_port_submodule_general;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_port_general_define_erp_tm_port;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_port_general_erp_tm_port_set;
    data_index = dnx_data_port_general_define_max_nof_channels;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_port_general_max_nof_channels_set;
    data_index = dnx_data_port_general_define_fabric_phys_offset;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_port_general_fabric_phys_offset_set;
    data_index = dnx_data_port_general_define_pp_dsp_size;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_port_general_pp_dsp_size_set;
    data_index = dnx_data_port_general_define_ingress_vlan_domain_size;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_port_general_ingress_vlan_domain_size_set;
    data_index = dnx_data_port_general_define_vlan_domain_size;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_port_general_vlan_domain_size_set;
    data_index = dnx_data_port_general_define_ffc_instruction_size;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_port_general_ffc_instruction_size_set;
    data_index = dnx_data_port_general_define_max_first_header_size_to_skip;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_port_general_max_first_header_size_to_skip_set;
    data_index = dnx_data_port_general_define_prt_ffc_width_start_offset;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_port_general_prt_ffc_width_start_offset_set;
    data_index = dnx_data_port_general_define_prt_ffc_start_input_offset;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_port_general_prt_ffc_start_input_offset_set;
    data_index = dnx_data_port_general_define_nof_tm_ports;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_port_general_nof_tm_ports_set;
    data_index = dnx_data_port_general_define_nof_tm_ports_per_fap_id;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_port_general_nof_tm_ports_per_fap_id_set;
    data_index = dnx_data_port_general_define_nof_pp_ports;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_port_general_nof_pp_ports_set;
    data_index = dnx_data_port_general_define_reserved_tm_port;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_port_general_reserved_tm_port_set;
    data_index = dnx_data_port_general_define_nof_vlan_membership_if;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_port_general_nof_vlan_membership_if_set;
    data_index = dnx_data_port_general_define_max_vlan_membership_if;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_port_general_max_vlan_membership_if_set;
    data_index = dnx_data_port_general_define_fabric_port_base;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_port_general_fabric_port_base_set;
    data_index = dnx_data_port_general_define_pp_port_bits_size;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_port_general_pp_port_bits_size_set;
    data_index = dnx_data_port_general_define_nof_port_bits_in_pp_bus;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_port_general_nof_port_bits_in_pp_bus_set;
    data_index = dnx_data_port_general_define_prt_configuration_ptch2_default_index;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_port_general_prt_configuration_ptch2_default_index_set;
    data_index = dnx_data_port_general_define_prt_configuration_ptch1_default_index;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_port_general_prt_configuration_ptch1_default_index_set;
    data_index = dnx_data_port_general_define_prt_configuration_eth_default_index;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_port_general_prt_configuration_eth_default_index_set;

    
    data_index = dnx_data_port_general_unrestricted_portmod_pll;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_port_general_unrestricted_portmod_pll_set;

    
    data_index = dnx_data_port_general_table_prt_configuration;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_port_general_prt_configuration_set;
    data_index = dnx_data_port_general_table_first_header_size;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_port_general_first_header_size_set;
    
    submodule_index = dnx_data_port_submodule_egress;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_port_egress_define_nof_ifs;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_port_egress_nof_ifs_set;

    

    
    
    submodule_index = dnx_data_port_submodule_imb;
    submodule = &module->submodules[submodule_index];

    

    

    
    data_index = dnx_data_port_imb_table_imb_type_info;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_port_imb_imb_type_info_set;

    SHR_FUNC_EXIT;
}
#undef BSL_LOG_MODULE

