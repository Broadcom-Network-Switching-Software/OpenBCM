

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SOCDNX_DIAG

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_graphical.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_property.h>








static shr_error_e
jr2_a0_dnx_data_graphical_diag_num_of_rows_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_graphical;
    int submodule_index = dnx_data_graphical_submodule_diag;
    int define_index = dnx_data_graphical_diag_define_num_of_rows;
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
jr2_a0_dnx_data_graphical_diag_num_of_index_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_graphical;
    int submodule_index = dnx_data_graphical_submodule_diag;
    int define_index = dnx_data_graphical_diag_define_num_of_index;
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
jr2_a0_dnx_data_graphical_diag_num_of_block_entries_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_graphical;
    int submodule_index = dnx_data_graphical_submodule_diag;
    int define_index = dnx_data_graphical_diag_define_num_of_block_entries;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 15;

    
    define->data = 15;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}



static shr_error_e
jr2_a0_dnx_data_graphical_diag_blocks_left_set(
    int unit)
{
    int row_index;
    int index_index;
    dnx_data_graphical_diag_blocks_left_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_graphical;
    int submodule_index = dnx_data_graphical_submodule_diag;
    int table_index = dnx_data_graphical_diag_table_blocks_left;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = dnx_data_graphical.diag.num_of_rows_get(unit);
    table->info_get.key_size[0] = dnx_data_graphical.diag.num_of_rows_get(unit);
    table->keys[1].size = dnx_data_graphical.diag.num_of_index_get(unit);
    table->info_get.key_size[1] = dnx_data_graphical.diag.num_of_index_get(unit);

    
    table->values[0].default_val = "dnx_graphical_block_count";
    table->values[1].default_val = "INVALID";
    table->values[2].default_val = "0";
    table->values[3].default_val = "1";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }
    if (table->keys[1].size == 0 || table->info_get.key_size[1] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_graphical_diag_blocks_left_t, (1 * (table->keys[0].size) * (table->keys[1].size) + 1  ), "data of dnx_data_graphical_diag_table_blocks_left");

    
    default_data = (dnx_data_graphical_diag_blocks_left_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->block = dnx_graphical_block_count;
    default_data->name = "INVALID";
    default_data->valid = 0;
    default_data->per_core = 1;
    
    for (row_index = 0; row_index < table->keys[0].size; row_index++)
    {
        for (index_index = 0; index_index < table->keys[1].size; index_index++)
        {
            data = (dnx_data_graphical_diag_blocks_left_t *) dnxc_data_mgmt_table_data_get(unit, table, row_index, index_index);
            sal_memcpy(data, default_data, table->size_of_values);
        }
    }
    
    if (0 < table->keys[0].size && 0 < table->keys[1].size)
    {
        data = (dnx_data_graphical_diag_blocks_left_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
        data->block = dnx_graphical_nif_rx;
        data->name = "NIF_RX";
        data->valid = 1;
    }
    if (1 < table->keys[0].size && 0 < table->keys[1].size)
    {
        data = (dnx_data_graphical_diag_blocks_left_t *) dnxc_data_mgmt_table_data_get(unit, table, 1, 0);
        data->block = dnx_graphical_ire;
        data->name = "IRE";
        data->valid = 1;
    }
    if (2 < table->keys[0].size && 0 < table->keys[1].size)
    {
        data = (dnx_data_graphical_diag_blocks_left_t *) dnxc_data_mgmt_table_data_get(unit, table, 2, 0);
        data->block = dnx_graphical_spb;
        data->name = "SPB";
        data->valid = 1;
    }
    if (2 < table->keys[0].size && 1 < table->keys[1].size)
    {
        data = (dnx_data_graphical_diag_blocks_left_t *) dnxc_data_mgmt_table_data_get(unit, table, 2, 1);
        data->block = dnx_graphical_ddp;
        data->name = "DDP";
        data->valid = 1;
    }
    if (2 < table->keys[0].size && 2 < table->keys[1].size)
    {
        data = (dnx_data_graphical_diag_blocks_left_t *) dnxc_data_mgmt_table_data_get(unit, table, 2, 2);
        data->block = dnx_graphical_cgm;
        data->name = "CGM";
        data->valid = 1;
    }
    if (2 < table->keys[0].size && 3 < table->keys[1].size)
    {
        data = (dnx_data_graphical_diag_blocks_left_t *) dnxc_data_mgmt_table_data_get(unit, table, 2, 3);
        data->block = dnx_graphical_ipt;
        data->name = "IPT";
        data->valid = 1;
    }
    if (3 < table->keys[0].size && 0 < table->keys[1].size)
    {
        data = (dnx_data_graphical_diag_blocks_left_t *) dnxc_data_mgmt_table_data_get(unit, table, 3, 0);
        data->block = dnx_graphical_fdt;
        data->name = "FDT";
        data->valid = 1;
        data->per_core = 0;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_graphical_diag_blocks_right_set(
    int unit)
{
    int row_index;
    int index_index;
    dnx_data_graphical_diag_blocks_right_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_graphical;
    int submodule_index = dnx_data_graphical_submodule_diag;
    int table_index = dnx_data_graphical_diag_table_blocks_right;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = 4;
    table->info_get.key_size[0] = 4;
    table->keys[1].size = 4;
    table->info_get.key_size[1] = 4;

    
    table->values[0].default_val = "dnx_graphical_block_count";
    table->values[1].default_val = "INVALID";
    table->values[2].default_val = "0";
    table->values[3].default_val = "1";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }
    if (table->keys[1].size == 0 || table->info_get.key_size[1] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_graphical_diag_blocks_right_t, (1 * (table->keys[0].size) * (table->keys[1].size) + 1  ), "data of dnx_data_graphical_diag_table_blocks_right");

    
    default_data = (dnx_data_graphical_diag_blocks_right_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->block = dnx_graphical_block_count;
    default_data->name = "INVALID";
    default_data->valid = 0;
    default_data->per_core = 1;
    
    for (row_index = 0; row_index < table->keys[0].size; row_index++)
    {
        for (index_index = 0; index_index < table->keys[1].size; index_index++)
        {
            data = (dnx_data_graphical_diag_blocks_right_t *) dnxc_data_mgmt_table_data_get(unit, table, row_index, index_index);
            sal_memcpy(data, default_data, table->size_of_values);
        }
    }
    
    if (0 < table->keys[0].size && 0 < table->keys[1].size)
    {
        data = (dnx_data_graphical_diag_blocks_right_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
        data->block = dnx_graphical_nif_tx;
        data->name = "NIF_TX";
        data->valid = 1;
    }
    if (1 < table->keys[0].size && 0 < table->keys[1].size)
    {
        data = (dnx_data_graphical_diag_blocks_right_t *) dnxc_data_mgmt_table_data_get(unit, table, 1, 0);
        data->block = dnx_graphical_epni;
        data->name = "EPNI";
        data->valid = 1;
    }
    if (2 < table->keys[0].size && 0 < table->keys[1].size)
    {
        data = (dnx_data_graphical_diag_blocks_right_t *) dnxc_data_mgmt_table_data_get(unit, table, 2, 0);
        data->block = dnx_graphical_pqp;
        data->name = "PQP";
        data->valid = 1;
    }
    if (2 < table->keys[0].size && 1 < table->keys[1].size)
    {
        data = (dnx_data_graphical_diag_blocks_right_t *) dnxc_data_mgmt_table_data_get(unit, table, 2, 1);
        data->block = dnx_graphical_rqp;
        data->name = "RQP";
        data->valid = 1;
    }
    if (2 < table->keys[0].size && 2 < table->keys[1].size)
    {
        data = (dnx_data_graphical_diag_blocks_right_t *) dnxc_data_mgmt_table_data_get(unit, table, 2, 2);
        data->block = dnx_graphical_fda;
        data->name = "FDA";
        data->valid = 1;
        data->per_core = 0;
    }
    if (3 < table->keys[0].size && 0 < table->keys[1].size)
    {
        data = (dnx_data_graphical_diag_blocks_right_t *) dnxc_data_mgmt_table_data_get(unit, table, 3, 0);
        data->block = dnx_graphical_fdr;
        data->name = "FDR";
        data->valid = 1;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_graphical_diag_counters_set(
    int unit)
{
    int block_index;
    int index_index;
    dnx_data_graphical_diag_counters_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_graphical;
    int submodule_index = dnx_data_graphical_submodule_diag;
    int table_index = dnx_data_graphical_diag_table_counters;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = dnx_graphical_block_count;
    table->info_get.key_size[0] = dnx_graphical_block_count;
    table->keys[1].size = dnx_data_graphical.diag.num_of_block_entries_get(unit);
    table->info_get.key_size[1] = dnx_data_graphical.diag.num_of_block_entries_get(unit);

    
    table->values[0].default_val = "INVALID";
    table->values[1].default_val = "INVALID";
    table->values[2].default_val = "INVALIDr, INVALIDr, INVALIDr, INVALIDr";
    table->values[3].default_val = "INVALIDf, INVALIDf, INVALIDf, INVALIDf";
    table->values[4].default_val = "INVALIDf, INVALIDf, INVALIDf, INVALIDf";
    table->values[5].default_val = "0";
    table->values[6].default_val = "1";
    table->values[7].default_val = "INVALID";
    table->values[8].default_val = "0";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }
    if (table->keys[1].size == 0 || table->info_get.key_size[1] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_graphical_diag_counters_t, (1 * (table->keys[0].size) * (table->keys[1].size) + 1  ), "data of dnx_data_graphical_diag_table_counters");

    
    default_data = (dnx_data_graphical_diag_counters_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->name = "INVALID";
    default_data->short_name = "INVALID";
    SHR_RANGE_VERIFY(4, 0, DNX_DIAG_COUNTER_REG_NOF_REGS, _SHR_E_INTERNAL, "out of bound access to array")
    default_data->regs[0] = INVALIDr;
    default_data->regs[1] = INVALIDr;
    default_data->regs[2] = INVALIDr;
    default_data->regs[3] = INVALIDr;
    SHR_RANGE_VERIFY(4, 0, DNX_DIAG_COUNTER_REG_NOF_REGS, _SHR_E_INTERNAL, "out of bound access to array")
    default_data->flds[0] = INVALIDf;
    default_data->flds[1] = INVALIDf;
    default_data->flds[2] = INVALIDf;
    default_data->flds[3] = INVALIDf;
    SHR_RANGE_VERIFY(4, 0, DNX_DIAG_COUNTER_REG_NOF_REGS, _SHR_E_INTERNAL, "out of bound access to array")
    default_data->overflow_field[0] = INVALIDf;
    default_data->overflow_field[1] = INVALIDf;
    default_data->overflow_field[2] = INVALIDf;
    default_data->overflow_field[3] = INVALIDf;
    default_data->arr_i = 0;
    default_data->fld_num = 1;
    default_data->doc = "INVALID";
    default_data->is_err = 0;
    
    for (block_index = 0; block_index < table->keys[0].size; block_index++)
    {
        for (index_index = 0; index_index < table->keys[1].size; index_index++)
        {
            data = (dnx_data_graphical_diag_counters_t *) dnxc_data_mgmt_table_data_get(unit, table, block_index, index_index);
            sal_memcpy(data, default_data, table->size_of_values);
        }
    }
    
    if (dnx_graphical_nif_rx < table->keys[0].size && 0 < table->keys[1].size)
    {
        data = (dnx_data_graphical_diag_counters_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_graphical_nif_rx, 0);
        data->name = "TOTAL_DROPPED_EOPS";
        data->short_name = "DROPPED_PKTS";
        SHR_RANGE_VERIFY(2, 0, DNX_DIAG_COUNTER_REG_NOF_REGS, _SHR_E_INTERNAL, "out of bound access to array")
        data->regs[0] = CDU_RX_NUM_TOTAL_DROPPED_EOPSr;
        data->regs[1] = CDUM_RX_NUM_TOTAL_DROPPED_EOPSr;
        SHR_RANGE_VERIFY(2, 0, DNX_DIAG_COUNTER_REG_NOF_REGS, _SHR_E_INTERNAL, "out of bound access to array")
        data->flds[0] = RX_NUM_TOTAL_DROPPED_EOPSf;
        data->flds[1] = RX_NUM_TOTAL_DROPPED_EOPSf;
        data->fld_num = 2;
        data->doc = "Number of Packets (EOPs) dropped in all the RX NIF ports";
        data->is_err = 1;
    }
    if (dnx_graphical_nif_rx < table->keys[0].size && 1 < table->keys[1].size)
    {
        data = (dnx_data_graphical_diag_counters_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_graphical_nif_rx, 1);
        data->name = "RX_TOTAL_PACKTS";
        data->short_name = "TTL_PKTS";
        SHR_RANGE_VERIFY(2, 0, DNX_DIAG_COUNTER_REG_NOF_REGS, _SHR_E_INTERNAL, "out of bound access to array")
        data->regs[0] = CDU_RX_TOTAL_PKT_COUNTERr;
        data->regs[1] = CDUM_RX_TOTAL_PKT_COUNTERr;
        SHR_RANGE_VERIFY(2, 0, DNX_DIAG_COUNTER_REG_NOF_REGS, _SHR_E_INTERNAL, "out of bound access to array")
        data->flds[0] = RX_TOTAL_PKT_COUNTERf;
        data->flds[1] = RX_TOTAL_PKT_COUNTERf;
        data->fld_num = 2;
        data->doc = "Number of total packets sent from NIF to IRE";
    }
    if (dnx_graphical_nif_rx < table->keys[0].size && 2 < table->keys[1].size)
    {
        data = (dnx_data_graphical_diag_counters_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_graphical_nif_rx, 2);
        data->name = "RX_TOTAL_BYTES";
        data->short_name = "TTL_BYTS";
        SHR_RANGE_VERIFY(2, 0, DNX_DIAG_COUNTER_REG_NOF_REGS, _SHR_E_INTERNAL, "out of bound access to array")
        data->regs[0] = CDU_RX_TOTAL_BYTE_COUNTERr;
        data->regs[1] = CDUM_RX_TOTAL_BYTE_COUNTERr;
        SHR_RANGE_VERIFY(2, 0, DNX_DIAG_COUNTER_REG_NOF_REGS, _SHR_E_INTERNAL, "out of bound access to array")
        data->flds[0] = RX_TOTAL_BYTE_COUNTERf;
        data->flds[1] = RX_TOTAL_BYTE_COUNTERf;
        data->fld_num = 2;
        data->doc = "Number of total bytes sent from NIF to IRE";
    }
    if (dnx_graphical_ire < table->keys[0].size && 0 < table->keys[1].size)
    {
        data = (dnx_data_graphical_diag_counters_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_graphical_ire, 0);
        data->name = "IN_FDT_CREDITS";
        data->short_name = "FDT_CRDS";
        data->regs[0] = IRE_FDT_INTERFACE_COUNTERr;
        data->flds[0] = FDT_CREDITSf;
        data->doc = "Number of received credit packets";
    }
    if (dnx_graphical_ire < table->keys[0].size && 1 < table->keys[1].size)
    {
        data = (dnx_data_graphical_diag_counters_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_graphical_ire, 1);
        data->name = "IN_CPU_PACKETS";
        data->short_name = "CPU_PKTS";
        data->regs[0] = IRE_CPU_PACKET_COUNTERr;
        data->flds[0] = CPU_PACKET_COUNTERf;
        data->doc = "Number of packets received from the CPU";
    }
    if (dnx_graphical_ire < table->keys[0].size && 2 < table->keys[1].size)
    {
        data = (dnx_data_graphical_diag_counters_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_graphical_ire, 2);
        data->name = "IN_OLP_PACKETS";
        data->short_name = "OLP_PKTS";
        data->regs[0] = IRE_OLP_PACKET_COUNTERr;
        data->flds[0] = OLP_PACKET_COUNTERf;
        data->doc = "Number of packets received from the OLP block (MAC TABLE MANAGMENT)";
    }
    if (dnx_graphical_ire < table->keys[0].size && 3 < table->keys[1].size)
    {
        data = (dnx_data_graphical_diag_counters_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_graphical_ire, 3);
        data->name = "IN_NIF_PACKETS";
        data->short_name = "NIF_PKTS";
        data->regs[0] = IRE_NIF_PACKET_COUNTERr;
        data->flds[0] = NIF_PACKET_COUNTERf;
        data->doc = "Number of packets received from the NIF block (FROM THE NETWORK)";
    }
    if (dnx_graphical_ire < table->keys[0].size && 4 < table->keys[1].size)
    {
        data = (dnx_data_graphical_diag_counters_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_graphical_ire, 4);
        data->name = "IN_OAMP_PACKETS";
        data->short_name = "OAMP_PKTS";
        data->regs[0] = IRE_OAMP_PACKET_COUNTERr;
        data->flds[0] = OAMP_PACKET_COUNTERf;
        data->doc = "Number of packets received from the OAMP block (FROM THE OAM ENGINE)";
    }
    if (dnx_graphical_ire < table->keys[0].size && 5 < table->keys[1].size)
    {
        data = (dnx_data_graphical_diag_counters_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_graphical_ire, 5);
        data->name = "IN_RCYH_PACKETS";
        data->short_name = "RCYH_PKTS";
        data->regs[0] = IRE_RCYH_PACKET_COUNTERr;
        data->flds[0] = RCYH_PACKET_COUNTERf;
        data->doc = "Number of packets received from the Recycle High interface";
    }
    if (dnx_graphical_ire < table->keys[0].size && 6 < table->keys[1].size)
    {
        data = (dnx_data_graphical_diag_counters_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_graphical_ire, 6);
        data->name = "IN_RCYL_PACKETS";
        data->short_name = "RCYL_PKTS";
        data->regs[0] = IRE_RCYL_PACKET_COUNTERr;
        data->flds[0] = RCYL_PACKET_COUNTERf;
        data->doc = "Number of packets received from the Recycle Low interface";
    }
    if (dnx_graphical_ire < table->keys[0].size && 7 < table->keys[1].size)
    {
        data = (dnx_data_graphical_diag_counters_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_graphical_ire, 7);
        data->name = "IN_SAT_PACKETS";
        data->short_name = "SAT_PKTS";
        data->regs[0] = IRE_SAT_PACKET_COUNTERr;
        data->flds[0] = SAT_PACKET_COUNTERf;
        data->doc = "Number of packets received from the SAT block (Service Activation)";
    }
    if (dnx_graphical_ire < table->keys[0].size && 8 < table->keys[1].size)
    {
        data = (dnx_data_graphical_diag_counters_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_graphical_ire, 8);
        data->name = "IN_EVENTOR_PACKETS";
        data->short_name = "EVNTR_PKTS";
        data->regs[0] = IRE_EVENTOR_PACKET_COUNTERr;
        data->flds[0] = EVENTOR_PACKET_COUNTERf;
        data->doc = "Number of packets (SOPs) received from eventor";
    }
    if (dnx_graphical_spb < table->keys[0].size && 0 < table->keys[1].size)
    {
        data = (dnx_data_graphical_diag_counters_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_graphical_spb, 0);
        data->name = "IN_IRE_PACKETS";
        data->short_name = "IRE_PKTS";
        data->regs[0] = SPB_PEC_DEBUG_2r;
        data->flds[0] = IRE_PACKETSf;
        data->doc = "Number of packets from IRE";
    }
    if (dnx_graphical_spb < table->keys[0].size && 1 < table->keys[1].size)
    {
        data = (dnx_data_graphical_diag_counters_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_graphical_spb, 1);
        data->name = "OUT_CGM_REPORTS";
        data->short_name = "CGM_RPRTS";
        data->regs[0] = SPB_BRC_DEBUGr;
        data->flds[0] = CGM_REPORT_COUNTERf;
        data->doc = "number of enq/deq reports sent to the CGM";
    }
    if (dnx_graphical_spb < table->keys[0].size && 2 < table->keys[1].size)
    {
        data = (dnx_data_graphical_diag_counters_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_graphical_spb, 2);
        data->name = "OUT_DDP_PACKETS";
        data->short_name = "DDP_PKTS";
        data->regs[0] = SPB_PTC_S_2D_DEBUGr;
        data->flds[0] = DDP_PACKETSf;
        data->doc = "Number of packets sent from the DRAM to the IPT";
    }
    if (dnx_graphical_spb < table->keys[0].size && 3 < table->keys[1].size)
    {
        data = (dnx_data_graphical_diag_counters_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_graphical_spb, 3);
        data->name = "OUT_IPT_PACKETS";
        data->short_name = "IPT_PKTS";
        data->regs[0] = SPB_PTC_S_2F_DEBUGr;
        data->flds[0] = IPT_PACKETSf;
        data->doc = "Number of packets sent from OCB to IPT";
    }
    if (dnx_graphical_spb < table->keys[0].size && 4 < table->keys[1].size)
    {
        data = (dnx_data_graphical_diag_counters_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_graphical_spb, 4);
        data->name = "CRC_ERROR_PACKETS";
        data->short_name = "CRC_ERRS";
        data->regs[0] = SPB_PTC_S_2F_DEBUGr;
        data->flds[0] = PACKET_CRC_ERRORf;
        data->doc = "Number of packets with CRC error";
        data->is_err = 1;
    }
    if (dnx_graphical_spb < table->keys[0].size && 5 < table->keys[1].size)
    {
        data = (dnx_data_graphical_diag_counters_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_graphical_spb, 5);
        data->name = "OUT_DDP_ERROR_PACKETS";
        data->short_name = "DDP_ERR_PKTS";
        data->regs[0] = SPB_PTC_S_2D_DEBUGr;
        data->flds[0] = DDP_ERROR_PACKETSf;
        data->doc = "Number of packets sent from SPB to DDP with error";
        data->is_err = 1;
    }
    if (dnx_graphical_spb < table->keys[0].size && 6 < table->keys[1].size)
    {
        data = (dnx_data_graphical_diag_counters_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_graphical_spb, 6);
        data->name = "OUT_IPT_ERROR_PACKETS";
        data->short_name = "IPT_ERR_PKTS";
        data->regs[0] = SPB_PTC_S_2F_DEBUGr;
        data->flds[0] = IPT_ERROR_PACKETSf;
        data->doc = "Number of packets sent from SPB to IPT with error";
        data->is_err = 1;
    }
    if (dnx_graphical_spb < table->keys[0].size && 7 < table->keys[1].size)
    {
        data = (dnx_data_graphical_diag_counters_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_graphical_spb, 7);
        data->name = "OUT_S2D_CRC_ERRORS";
        data->short_name = "S2D_CRC_ERR_PKTS";
        data->regs[0] = SPB_PTC_S_2D_DEBUGr;
        data->flds[0] = PACKET_S_2D_CRC_ERRORf;
        data->doc = "Number of packets with CRC error";
        data->is_err = 1;
    }
    if (dnx_graphical_ddp < table->keys[0].size && 0 < table->keys[1].size)
    {
        data = (dnx_data_graphical_diag_counters_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_graphical_ddp, 0);
        data->name = "IN_SPB_GOOD_PACKETS";
        data->short_name = "SPB_OK_PKTS";
        data->regs[0] = DDP_PACKET_ERROR_COUNTERSr;
        data->flds[0] = SPB_GOOD_PACKET_CNTf;
        data->doc = "Number of packets without errors from OCB";
    }
    if (dnx_graphical_ddp < table->keys[0].size && 1 < table->keys[1].size)
    {
        data = (dnx_data_graphical_diag_counters_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_graphical_ddp, 1);
        data->name = "IN_SPB_ERROR_PACKETS";
        data->short_name = "SPB_ERR_PKTS";
        data->regs[0] = DDP_PACKET_ERROR_COUNTERSr;
        data->flds[0] = SPB_PACKET_ERROR_CNTf;
        data->doc = "Number of packets with errors from SPB";
        data->is_err = 1;
    }
    if (dnx_graphical_ddp < table->keys[0].size && 2 < table->keys[1].size)
    {
        data = (dnx_data_graphical_diag_counters_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_graphical_ddp, 2);
        data->name = "IN_ITPP_DISCARD_PACKETS";
        data->short_name = "ITPP_DSCRD_PKTS";
        data->regs[0] = DDP_ITE_DEBUGr;
        data->flds[0] = PACKET_DISCARDf;
        data->doc = "Number of packets with discard request from ITPP";
        data->is_err = 1;
    }
    if (dnx_graphical_ddp < table->keys[0].size && 3 < table->keys[1].size)
    {
        data = (dnx_data_graphical_diag_counters_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_graphical_ddp, 3);
        data->name = "CRC_ERROR_PACKETS";
        data->short_name = "CRC_ERRS";
        data->regs[0] = DDP_ITE_DEBUGr;
        data->flds[0] = PACKET_ERRORf;
        data->doc = "Number of packets with CRC error";
        data->is_err = 1;
    }
    if (dnx_graphical_ddp < table->keys[0].size && 4 < table->keys[1].size)
    {
        data = (dnx_data_graphical_diag_counters_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_graphical_ddp, 4);
        data->name = "DRAM_CRC_FLIPPED_ERROR_PACKETS";
        data->short_name = "DRAM_CRC_FLPD_ERRS";
        data->regs[0] = DDP_PACKET_ERROR_COUNTERSr;
        data->flds[0] = DRAM_PACKET_CRC_FLIPPED_ERROR_CNTf;
        data->doc = "Number of packets with CRC flipped from DRAM (might be error from SPB)";
        data->is_err = 1;
    }
    if (dnx_graphical_ddp < table->keys[0].size && 5 < table->keys[1].size)
    {
        data = (dnx_data_graphical_diag_counters_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_graphical_ddp, 5);
        data->name = "DRAM_CRC_ERROR_PACKETS";
        data->short_name = "DRAM_CRC_ERRS";
        data->regs[0] = DDP_PACKET_ERROR_COUNTERSr;
        data->flds[0] = DRAM_PACKET_CRC_ERROR_CNTf;
        data->doc = "Number of packets with CRC Error (not flipped).";
    }
    if (dnx_graphical_ddp < table->keys[0].size && 6 < table->keys[1].size)
    {
        data = (dnx_data_graphical_diag_counters_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_graphical_ddp, 6);
        data->name = "DRAM_DUMMY_TO_DISCARD_PACKETS";
        data->short_name = "DRAM_DMY_2_DSCRD";
        data->regs[0] = DDP_PACKET_ERROR_COUNTERSr;
        data->flds[0] = DRAM_PACKET_PKUP_DISCARD_CNTf;
        data->doc = "Number of dummy packets generated by the PKUP";
        data->is_err = 1;
    }
    if (dnx_graphical_ddp < table->keys[0].size && 7 < table->keys[1].size)
    {
        data = (dnx_data_graphical_diag_counters_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_graphical_ddp, 7);
        data->name = "DRAM_GOOD_PACKETS";
        data->short_name = "DRAM_OK_PKTS";
        data->regs[0] = DDP_PACKET_ERROR_COUNTERSr;
        data->flds[0] = DRAM_GOOD_PACKET_CNTf;
        data->doc = "Number of packets without errors.";
    }
    if (dnx_graphical_ddp < table->keys[0].size && 8 < table->keys[1].size)
    {
        data = (dnx_data_graphical_diag_counters_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_graphical_ddp, 8);
        data->name = "CPYDAT_ERROR_PACKETS";
        data->short_name = "CPTDAT_CRC_PKTS";
        SHR_RANGE_VERIFY(3, 0, DNX_DIAG_COUNTER_REG_NOF_REGS, _SHR_E_INTERNAL, "out of bound access to array")
        data->regs[0] = DDP_PACKET_ERROR_COUNTERSr;
        data->regs[1] = DDP_PACKET_ERROR_COUNTERSr;
        data->regs[2] = DDP_PACKET_ERROR_COUNTERSr;
        SHR_RANGE_VERIFY(3, 0, DNX_DIAG_COUNTER_REG_NOF_REGS, _SHR_E_INTERNAL, "out of bound access to array")
        data->flds[0] = DRAM_PACKET_CPY_DAT_CRC_ERR_CNTf;
        data->flds[1] = DRAM_PACKET_CPY_DAT_ECC_1B_ERR_CNTf;
        data->flds[2] = DRAM_PACKET_CPY_DAT_ECC_2B_ERR_CNTf;
        data->fld_num = 3;
        data->doc = "Number of packets with pkt-copydat CRC or ECC 1/2B error";
        data->is_err = 1;
    }
    if (dnx_graphical_ddp < table->keys[0].size && 9 < table->keys[1].size)
    {
        data = (dnx_data_graphical_diag_counters_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_graphical_ddp, 9);
        data->name = "OUT_CGM_CPYDAT_REPORTS";
        data->short_name = "CGM_CPTDAT_RPRTS";
        data->regs[0] = DDP_PACKET_ERROR_COUNTERSr;
        data->flds[0] = PKUP_2_CGM_PKT_TX_RPRT_CNTf;
        data->doc = "Number of packets report to Cgm";
    }
    if (dnx_graphical_cgm < table->keys[0].size && 0 < table->keys[1].size)
    {
        data = (dnx_data_graphical_diag_counters_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_graphical_cgm, 0);
        data->name = "QNUM_NOT_VALID_PACKETS(IRPP Drops)";
        data->short_name = "Q_N_VALID_PKTS";
        data->regs[0] = CGM_QNUM_NOT_VALID_PKT_CTRr;
        data->flds[0] = QNUM_NOT_VALID_PKT_CTRf;
        data->doc = "Number of packets dropped due to invalid que.";
        data->is_err = 1;
    }
    if (dnx_graphical_cgm < table->keys[0].size && 1 < table->keys[1].size)
    {
        data = (dnx_data_graphical_diag_counters_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_graphical_cgm, 1);
        data->name = "SRAM_DELETED_PACKETS";
        data->short_name = "SRAM_DEL_PKTS";
        data->regs[0] = CGM_VOQ_SRAM_DEL_PKT_CTRr;
        data->flds[0] = VOQ_SRAM_DEL_PKT_CTRf;
        data->doc = "Number of packets stored in the OCB and dropped, drop reason can be seen at CGM_REJECT_STATUS_BITMAP register";
        data->is_err = 1;
    }
    if (dnx_graphical_cgm < table->keys[0].size && 2 < table->keys[1].size)
    {
        data = (dnx_data_graphical_diag_counters_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_graphical_cgm, 2);
        data->name = "SRAM_DEQUEUE_TO_DRAM_PACKETS";
        data->short_name = "SRAM_DEQ_TO_DRAM_PKTS";
        data->regs[0] = CGM_VOQ_SRAM_DEQ_TO_DRAM_PKT_CTRr;
        data->flds[0] = VOQ_SRAM_DEQ_TO_DRAM_PKT_CTRf;
        data->doc = "Number of packets stored in the OCB, and transfered later on to the DRAM";
    }
    if (dnx_graphical_cgm < table->keys[0].size && 3 < table->keys[1].size)
    {
        data = (dnx_data_graphical_diag_counters_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_graphical_cgm, 3);
        data->name = "SRAM_DEQUEUE_TO_FABRIC_PACKETS";
        data->short_name = "SRAM_DEQ_TO_FAB_PKTS";
        data->regs[0] = CGM_VOQ_SRAM_DEQ_TO_FABRIC_PKT_CTRr;
        data->flds[0] = VOQ_SRAM_DEQ_TO_FABRIC_PKT_CTRf;
        data->doc = "Number of OCB dequeued packets to the Fabric/Local. Note: according to the VOQ_PRG_CTR_QNUM_INDEX and VOQ_PRG_CTR_QNUM_INDEX";
    }
    if (dnx_graphical_cgm < table->keys[0].size && 4 < table->keys[1].size)
    {
        data = (dnx_data_graphical_diag_counters_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_graphical_cgm, 4);
        data->name = "SRAM_ENQUEUE_PACKETS";
        data->short_name = "SRAM_ENQ_PKTS";
        data->regs[0] = CGM_VOQ_SRAM_ENQ_PKT_CTRr;
        data->flds[0] = VOQ_SRAM_ENQ_PKT_CTRf;
        data->doc = "Number of good packets stored in the OCB  (not including rejects). Note: according to the VOQ_PRG_CTR_QNUM_INDEX and VOQ_PRG_CTR_QNUM_INDEX_MASK";
    }
    if (dnx_graphical_cgm < table->keys[0].size && 5 < table->keys[1].size)
    {
        data = (dnx_data_graphical_diag_counters_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_graphical_cgm, 5);
        data->name = "SRAM_ENQUEUE_REJECT_PACKETS";
        data->short_name = "SRAM_ENQ_RJCT_PKTS";
        data->regs[0] = CGM_VOQ_SRAM_ENQ_RJCT_PKT_CTRr;
        data->flds[0] = VOQ_SRAM_ENQ_RJCT_PKT_CTRf;
        data->doc = "Number of packets sent to the OCB and dropped (rejected). Note: according to the VOQ_PRG_CTR_QNUM_INDEX and VOQ_PRG_CTR_QNUM_INDEX_MASK";
        data->is_err = 1;
    }
    if (dnx_graphical_ipt < table->keys[0].size && 0 < table->keys[1].size)
    {
        data = (dnx_data_graphical_diag_counters_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_graphical_ipt, 0);
        data->name = "IN_CGM_COMMANDS";
        data->short_name = "IN_CGM_CMDS";
        data->regs[0] = IPT_PDQ_DEBUGr;
        data->flds[0] = CGM_RXI_PACKET_COUNTf;
        data->doc = "Number of SRAM-packets (TX commands) and DRAM-bundle (SOB) from Cgm, includes SRAM-DEL";
    }
    if (dnx_graphical_ipt < table->keys[0].size && 1 < table->keys[1].size)
    {
        data = (dnx_data_graphical_diag_counters_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_graphical_ipt, 1);
        data->name = "IN_SPB_PACKETS";
        data->short_name = "IN_SPB_PKTS";
        data->regs[0] = IPT_SRAM_RRF_GLB_DEBUGr;
        data->flds[0] = SPB_PACKET_COUNTf;
        data->doc = "Number of packet request from SPB";
    }
    if (dnx_graphical_ipt < table->keys[0].size && 2 < table->keys[1].size)
    {
        data = (dnx_data_graphical_diag_counters_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_graphical_ipt, 2);
        data->name = "IN_DDP_BUFFERS";
        data->short_name = "IN_DDP_BFRS";
        data->regs[0] = IPT_SRAM_RRF_GLB_DEBUGr;
        data->flds[0] = DDP_TX_BUFFER_COUNTERf;
        data->doc = "Number DRAM buffer read to DDP";
    }
    if (dnx_graphical_ipt < table->keys[0].size && 3 < table->keys[1].size)
    {
        data = (dnx_data_graphical_diag_counters_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_graphical_ipt, 3);
        data->name = "IN_ITPP_DISCARD_PACKETS";
        data->short_name = "IN_ITPP_DSCRD_PKTS";
        data->regs[0] = IPT_ITE_DEBUGr;
        data->flds[0] = PACKET_DISCARDf;
        data->doc = "Number of packets with discard request from ITPP";
        data->is_err = 1;
    }
    if (dnx_graphical_fdt < table->keys[0].size && 0 < table->keys[1].size)
    {
        data = (dnx_data_graphical_diag_counters_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_graphical_fdt, 0);
        data->name = "UNREACH_DEST_CELLS";
        data->short_name = "UNREACH_DEST_CELLS";
        data->regs[0] = FDT_UNREACHABLE_DESTINATION_DISCARDED_CELLS_COUNTERr;
        data->flds[0] = UNRCH_DEST_CNT_Nf;
        data->doc = "Counts unreachable cell events for each of the 2 Cores";
        data->is_err = 1;
    }
    if (dnx_graphical_fdt < table->keys[0].size && 1 < table->keys[1].size)
    {
        data = (dnx_data_graphical_diag_counters_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_graphical_fdt, 1);
        data->name = "OUT_FABRIC_CELLS";
        data->short_name = "OUT_FAB_CELLS";
        SHR_RANGE_VERIFY(3, 0, DNX_DIAG_COUNTER_REG_NOF_REGS, _SHR_E_INTERNAL, "out of bound access to array")
        data->regs[0] = FDT_FABRIC_CELLS_CNTr;
        data->regs[1] = FDT_FABRIC_CELLS_CNTr;
        data->regs[2] = FDT_FABRIC_CELLS_CNTr;
        SHR_RANGE_VERIFY(3, 0, DNX_DIAG_COUNTER_REG_NOF_REGS, _SHR_E_INTERNAL, "out of bound access to array")
        data->flds[0] = FABRIC_CELL_CORE_N_CELL_CNTf;
        data->flds[1] = MESH_DEST_1_CORE_N_CELL_CNTf;
        data->flds[2] = MESH_DEST_2_CORE_N_CELL_CNTf;
        data->fld_num = 3;
        data->doc = "fabric cells or mesh dest0/1/2 cells count";
    }
    if (dnx_graphical_fdr < table->keys[0].size && 0 < table->keys[1].size)
    {
        data = (dnx_data_graphical_diag_counters_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_graphical_fdr, 0);
        data->name = "IN_TOTAL_CELLS";
        data->short_name = "IN_TTL_CELLS";
        data->regs[0] = FDR_CELL_IN_CNT_TOTALr;
        data->flds[0] = CELL_IN_CNTf;
        data->overflow_field[0] = CELL_IN_CNT_Of;
        data->doc = "total number of cells";
    }
    if (dnx_graphical_fdr < table->keys[0].size && 1 < table->keys[1].size)
    {
        data = (dnx_data_graphical_diag_counters_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_graphical_fdr, 1);
        data->name = "IN_PIPE0_CELLS";
        data->short_name = "IN_P0_CELLS";
        data->regs[0] = FDR_P_CELL_IN_CNTr;
        data->flds[0] = P_N_CELL_IN_CNTf;
        data->overflow_field[0] = P_N_CELL_IN_CNT_Of;
        data->arr_i = 1;
        data->doc = "FDR total incoming cell counter at the pipe";
    }
    if (dnx_graphical_fdr < table->keys[0].size && 2 < table->keys[1].size)
    {
        data = (dnx_data_graphical_diag_counters_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_graphical_fdr, 2);
        data->name = "IN_PIPE1_CELLS";
        data->short_name = "IN_P1_CELLS";
        data->regs[0] = FDR_P_CELL_IN_CNTr;
        data->flds[0] = P_N_CELL_IN_CNTf;
        data->overflow_field[0] = P_N_CELL_IN_CNT_Of;
        data->arr_i = 2;
        data->doc = "FDR total incoming cell counter at the pipe";
    }
    if (dnx_graphical_fdr < table->keys[0].size && 3 < table->keys[1].size)
    {
        data = (dnx_data_graphical_diag_counters_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_graphical_fdr, 3);
        data->name = "IN_PIPE2_CELLS";
        data->short_name = "IN_P2_CELLS";
        data->regs[0] = FDR_P_CELL_IN_CNTr;
        data->flds[0] = P_N_CELL_IN_CNTf;
        data->overflow_field[0] = P_N_CELL_IN_CNT_Of;
        data->arr_i = 3;
        data->doc = "FDR total incoming cell counter at the pipe";
    }
    if (dnx_graphical_fda < table->keys[0].size && 0 < table->keys[1].size)
    {
        data = (dnx_data_graphical_diag_counters_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_graphical_fda, 0);
        data->name = "FABRIC_CELLS";
        data->short_name = "OUT_RQP_CELLS";
        SHR_RANGE_VERIFY(3, 0, DNX_DIAG_COUNTER_REG_NOF_REGS, _SHR_E_INTERNAL, "out of bound access to array")
        data->regs[0] = FDA_EGQ_CELLS_OUT_CNT_FAB_PIPE_0r;
        data->regs[1] = FDA_EGQ_CELLS_OUT_CNT_FAB_PIPE_1r;
        data->regs[2] = FDA_EGQ_CELLS_OUT_CNT_FAB_PIPE_2r;
        SHR_RANGE_VERIFY(3, 0, DNX_DIAG_COUNTER_REG_NOF_REGS, _SHR_E_INTERNAL, "out of bound access to array")
        data->flds[0] = EGQ_N_CELLS_OUT_CNT_FAB_PIPE_0f;
        data->flds[1] = EGQ_N_CELLS_OUT_CNT_FAB_PIPE_1f;
        data->flds[2] = EGQ_N_CELLS_OUT_CNT_FAB_PIPE_2f;
        SHR_RANGE_VERIFY(3, 0, DNX_DIAG_COUNTER_REG_NOF_REGS, _SHR_E_INTERNAL, "out of bound access to array")
        data->overflow_field[0] = EGQ_N_CELLS_OUT_CNT_FAB_PIPE_0_OVERFLOWf;
        data->overflow_field[1] = EGQ_N_CELLS_OUT_CNT_FAB_PIPE_1_OVERFLOWf;
        data->overflow_field[2] = EGQ_N_CELLS_OUT_CNT_FAB_PIPE_2_OVERFLOWf;
        data->fld_num = 3;
        data->doc = "Cells output counters";
    }
    if (dnx_graphical_fda < table->keys[0].size && 1 < table->keys[1].size)
    {
        data = (dnx_data_graphical_diag_counters_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_graphical_fda, 1);
        data->name = "LOCAL_UNICAST_CELLS";
        data->short_name = "LOCAL_UC_CELLS";
        SHR_RANGE_VERIFY(2, 0, DNX_DIAG_COUNTER_REG_NOF_REGS, _SHR_E_INTERNAL, "out of bound access to array")
        data->regs[0] = FDA_EGQ_CELLS_OUT_CNT_IPT_0r;
        data->regs[1] = FDA_EGQ_CELLS_OUT_CNT_IPT_1r;
        SHR_RANGE_VERIFY(2, 0, DNX_DIAG_COUNTER_REG_NOF_REGS, _SHR_E_INTERNAL, "out of bound access to array")
        data->flds[0] = EGQ_N_CELLS_OUT_CNT_IPT_0f;
        data->flds[1] = EGQ_N_CELLS_OUT_CNT_IPT_1f;
        SHR_RANGE_VERIFY(2, 0, DNX_DIAG_COUNTER_REG_NOF_REGS, _SHR_E_INTERNAL, "out of bound access to array")
        data->overflow_field[0] = EGQ_N_CELLS_OUT_CNT_IPT_0_OVERFLOWf;
        data->overflow_field[1] = EGQ_N_CELLS_OUT_CNT_IPT_1_OVERFLOWf;
        data->fld_num = 2;
        data->doc = "Local UC output cells";
    }
    if (dnx_graphical_fda < table->keys[0].size && 2 < table->keys[1].size)
    {
        data = (dnx_data_graphical_diag_counters_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_graphical_fda, 2);
        data->name = "LOCAL_MULTICAST_CELLS";
        data->short_name = "LOCAL_MC_CELLS";
        SHR_RANGE_VERIFY(2, 0, DNX_DIAG_COUNTER_REG_NOF_REGS, _SHR_E_INTERNAL, "out of bound access to array")
        data->regs[0] = FDA_EGQ_CELLS_OUT_CNT_MESHMC_0r;
        data->regs[1] = FDA_EGQ_CELLS_OUT_CNT_MESHMC_1r;
        SHR_RANGE_VERIFY(2, 0, DNX_DIAG_COUNTER_REG_NOF_REGS, _SHR_E_INTERNAL, "out of bound access to array")
        data->flds[0] = EGQ_N_CELLS_OUT_CNT_MESHMC_0f;
        data->flds[1] = EGQ_N_CELLS_OUT_CNT_MESHMC_1f;
        SHR_RANGE_VERIFY(2, 0, DNX_DIAG_COUNTER_REG_NOF_REGS, _SHR_E_INTERNAL, "out of bound access to array")
        data->overflow_field[0] = EGQ_N_CELLS_OUT_CNT_MESHMC_0_OVERFLOWf;
        data->overflow_field[1] = EGQ_N_CELLS_OUT_CNT_MESHMC_1_OVERFLOWf;
        data->fld_num = 2;
        data->doc = "Mesh MC output cells";
    }
    if (dnx_graphical_rqp < table->keys[0].size && 0 < table->keys[1].size)
    {
        data = (dnx_data_graphical_diag_counters_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_graphical_rqp, 0);
        data->name = "IN_LOCAL0_CELLS";
        data->short_name = "LOCAL0_CELLS";
        data->regs[0] = RQP_PRS_EGQ_COUNTERSr;
        data->flds[0] = EGQ_CELL_0_LOCAL_CNTf;
        data->doc = "Number of Local cells on interface 0";
    }
    if (dnx_graphical_rqp < table->keys[0].size && 1 < table->keys[1].size)
    {
        data = (dnx_data_graphical_diag_counters_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_graphical_rqp, 1);
        data->name = "IN_LOCAL1_CELLS";
        data->short_name = "LOCAL1_CELLS";
        data->regs[0] = RQP_PRS_EGQ_COUNTERSr;
        data->flds[0] = EGQ_CELL_1_LOCAL_CNTf;
        data->doc = "Number of Local cells on interface 1";
    }
    if (dnx_graphical_rqp < table->keys[0].size && 2 < table->keys[1].size)
    {
        data = (dnx_data_graphical_diag_counters_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_graphical_rqp, 2);
        data->name = "IN_TDM_BYPASS_CELLS";
        data->short_name = "TDM_BYPS_CELLS";
        data->regs[0] = RQP_PRS_EGQ_COUNTERSr;
        data->flds[0] = EGQ_TDM_BYPASS_CNTf;
        data->doc = "Number of TDM bypass cells";
    }
    if (dnx_graphical_rqp < table->keys[0].size && 3 < table->keys[1].size)
    {
        data = (dnx_data_graphical_diag_counters_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_graphical_rqp, 3);
        data->name = "IN_FAB0_CELLS";
        data->short_name = "FAB0_CELLS";
        data->regs[0] = RQP_PRS_EGQ_COUNTERSr;
        data->flds[0] = EGQ_CELL_0_FAB_CNTf;
        data->doc = "Number of fabric cells on interface 0";
    }
    if (dnx_graphical_rqp < table->keys[0].size && 4 < table->keys[1].size)
    {
        data = (dnx_data_graphical_diag_counters_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_graphical_rqp, 4);
        data->name = "IN_FAB1_CELLS";
        data->short_name = "FAB1_CELLS";
        data->regs[0] = RQP_PRS_EGQ_COUNTERSr;
        data->flds[0] = EGQ_CELL_0_FAB_CNTf;
        data->doc = "Number of fabric cells on interface 1";
    }
    if (dnx_graphical_rqp < table->keys[0].size && 5 < table->keys[1].size)
    {
        data = (dnx_data_graphical_diag_counters_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_graphical_rqp, 5);
        data->name = "CELL_DECODER_ERROR_CELLS";
        data->short_name = "CDC_ERR_CELLS";
        data->regs[0] = RQP_CELL_DECODER_DEBUG_COUNTERSr;
        data->flds[0] = CDC_ERR_CELL_CNTf;
        data->doc = "Counts the number of cells reach the cell decoder, and Cell decoder decide that it has an error, and the error is not masked (disabled) in the DbgCdcErrMaskDis, and the Source is not masked (disabled) in the DbgSourceMaskDis";
        data->is_err = 1;
    }
    if (dnx_graphical_rqp < table->keys[0].size && 6 < table->keys[1].size)
    {
        data = (dnx_data_graphical_diag_counters_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_graphical_rqp, 6);
        data->name = "EMR_DISCARD_PACKETS";
        data->short_name = "EMR_DSCRD_PKTS";
        data->regs[0] = RQP_EMR_DISCARDS_PACKET_COUNTERr;
        data->flds[0] = EMR_DISCARDS_PACKET_COUNTERf;
        data->doc = "EMR2ERPP discarded packet counter. Counts the number of discarded packets due to reassembly errors or MCDB discard. If bit 32 is set, counter overflowed. If CheckBwToOfp is set, counts OfpToCheckBw packets; otherwise, counts all packets.";
        data->is_err = 1;
    }
    if (dnx_graphical_rqp < table->keys[0].size && 7 < table->keys[1].size)
    {
        data = (dnx_data_graphical_diag_counters_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_graphical_rqp, 7);
        data->name = "EMR_MULTICAST_HIGH";
        data->short_name = "EMR_HMC";
        data->regs[0] = RQP_EMR_MC_HIGH_PACKET_COUNTERr;
        data->flds[0] = EMR_MC_HIGH_PACKET_COUNTERf;
        data->doc = "This counter counts number of multicast high packets discarded because multicast FIFO is full. If bit 32 is set, counter overflowed.";
    }
    if (dnx_graphical_rqp < table->keys[0].size && 8 < table->keys[1].size)
    {
        data = (dnx_data_graphical_diag_counters_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_graphical_rqp, 8);
        data->name = "EMR_MULTICAST_LOW";
        data->short_name = "EMR_LMC";
        data->regs[0] = RQP_EMR_MC_LOW_PACKET_COUNTERr;
        data->flds[0] = EMR_MC_LOW_PACKET_COUNTERf;
        data->doc = "This counter counts number of multicast low packets discarded because multicast FIFO is full. If bit 32 is set, counter overflowed.";
    }
    if (dnx_graphical_rqp < table->keys[0].size && 9 < table->keys[1].size)
    {
        data = (dnx_data_graphical_diag_counters_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_graphical_rqp, 9);
        data->name = "EMR_UNICAST";
        data->short_name = "EMR_UC";
        data->regs[0] = RQP_EMR_UNICAST_PACKET_COUNTERr;
        data->flds[0] = EMR_UNICAST_PACKET_COUNTERf;
        data->doc = "EHP2PQP Unicast packet counter. If bit 32 is set, counter overflowed. If CheckBwToOfp is set, counts OfpToCheckBw packets; otherwise, counts all packets.";
    }
    if (dnx_graphical_rqp < table->keys[0].size && 10 < table->keys[1].size)
    {
        data = (dnx_data_graphical_diag_counters_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_graphical_rqp, 10);
        data->name = "EMR_TDM";
        data->short_name = "EMR_TDM";
        data->regs[0] = RQP_EMR_TDM_PACKET_COUNTERr;
        data->flds[0] = EMR_TDM_PACKET_COUNTERf;
        data->doc = "EHP2PQP Unicast packet counter. If bit 32 is set, counter overflowed. If CheckBwToOfp is set, counts OfpToCheckBw packets; otherwise, counts all packets.";
    }
    if (dnx_graphical_rqp < table->keys[0].size && 11 < table->keys[1].size)
    {
        data = (dnx_data_graphical_diag_counters_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_graphical_rqp, 11);
        data->name = "PRP_MC_DISCARD_PACKETS";
        data->short_name = "PRP_MC_DSCRD_PKTS";
        data->regs[0] = RQP_PRP_DEBUG_COUNTERSr;
        data->flds[0] = PRP_SOP_DISCARD_MC_CNTf;
        data->doc = "Counts the number of multicast SOP discarded (not forward to EHP) in the PRP, mask per reasong is DbgPrpSopDiscardMaskDisCnt.";
        data->is_err = 1;
    }
    if (dnx_graphical_rqp < table->keys[0].size && 12 < table->keys[1].size)
    {
        data = (dnx_data_graphical_diag_counters_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_graphical_rqp, 12);
        data->name = "PRP_UC_DISCARD_PACKETS";
        data->short_name = "PRP_UC_DSCRD_PKTS";
        data->regs[0] = RQP_PRP_DEBUG_COUNTERSr;
        data->flds[0] = PRP_SOP_DISCARD_UC_CNTf;
        data->doc = "Counts the number of unicast SOP discarded (not forward to EHP) in the PRP, mask per reasong is DbgPrpSopDiscardMaskDisCnt.";
        data->is_err = 1;
    }
    if (dnx_graphical_rqp < table->keys[0].size && 13 < table->keys[1].size)
    {
        data = (dnx_data_graphical_diag_counters_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_graphical_rqp, 13);
        data->name = "PRP_TDM_DISCARD_PACKETS";
        data->short_name = "PRP_TDM_DSCRD_PKTS";
        data->regs[0] = RQP_PRP_DEBUG_COUNTERSr;
        data->flds[0] = PRP_SOP_DISCARD_TDM_CNTf;
        data->doc = "Counts the number of TDM SOP discarded (not forward to EHP) in the PRP, mask per reasong is DbgPrpSopDiscardMaskDisCnt.";
        data->is_err = 1;
    }
    if (dnx_graphical_rqp < table->keys[0].size && 14 < table->keys[1].size)
    {
        data = (dnx_data_graphical_diag_counters_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_graphical_rqp, 14);
        data->name = "PRS_DROP_CELL_CNT";
        data->short_name = "PRS_DROP_CELLS";
        data->regs[0] = RQP_PRS_DROP_CELL_CNTr;
        data->flds[0] = PRS_DROP_CELL_CNTf;
        data->doc = "Counts the number of cells dropped.";
        data->is_err = 1;
    }
    if (dnx_graphical_pqp < table->keys[0].size && 0 < table->keys[1].size)
    {
        data = (dnx_data_graphical_diag_counters_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_graphical_pqp, 0);
        data->name = "VISIBILITY_PACKETS";
        data->short_name = "VIS_PKTS";
        data->regs[0] = PQP_VISIBILITY_PACKET_COUNTERr;
        data->flds[0] = VISIBILITY_PACKET_COUNTERf;
        data->doc = "PQP visibility packets counter.  If bit 32 is set, counter overflowed.";
    }
    if (dnx_graphical_pqp < table->keys[0].size && 1 < table->keys[1].size)
    {
        data = (dnx_data_graphical_diag_counters_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_graphical_pqp, 1);
        data->name = "MULTICAST_PACKETS";
        data->short_name = "MC_PKTS";
        data->regs[0] = PQP_PQP_MULTICAST_PACKET_COUNTERr;
        data->flds[0] = PQP_MULTICAST_PACKET_COUNTERf;
        data->doc = "PQP2FQP Multicast  packet counter. If bit 32 is set, counter overflowed. If CheckBwToOfp is set, counts OfpToCheckBw packets; otherwise, counts all packets.";
    }
    if (dnx_graphical_pqp < table->keys[0].size && 2 < table->keys[1].size)
    {
        data = (dnx_data_graphical_diag_counters_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_graphical_pqp, 2);
        data->name = "UNICAST_PACKETS";
        data->short_name = "UC_PKTS";
        data->regs[0] = PQP_PQP_UNICAST_PACKET_COUNTERr;
        data->flds[0] = PQP_UNICAST_PACKET_COUNTERf;
        data->doc = "PQP2FQP Unicast  packet counter. If bit 32 is set, counter overflowed. If CheckBwToOfp is set, counts OfpToCheckBw packets; otherwise, counts all packets.";
    }
    if (dnx_graphical_pqp < table->keys[0].size && 3 < table->keys[1].size)
    {
        data = (dnx_data_graphical_diag_counters_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_graphical_pqp, 3);
        data->name = "MULTICAST_DISCARD_PACKETS";
        data->short_name = "MC_DSCRD_PKTS";
        data->regs[0] = PQP_PQP_DISCARD_MULTICAST_PACKET_COUNTERr;
        data->flds[0] = PQP_DISCARD_MULTICAST_PACKET_COUNTERf;
        data->doc = "PQP discarded Multicast packet counter. If bit 32 is set, counter overflowed.";
        data->is_err = 1;
    }
    if (dnx_graphical_pqp < table->keys[0].size && 4 < table->keys[1].size)
    {
        data = (dnx_data_graphical_diag_counters_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_graphical_pqp, 4);
        data->name = "UNICAST_DISCARD_PACKETS";
        data->short_name = "UC_DSCRD_PKTS";
        data->regs[0] = PQP_PQP_DISCARD_UNICAST_PACKET_COUNTERr;
        data->flds[0] = PQP_DISCARD_UNICAST_PACKET_COUNTERf;
        data->doc = "PQP discarded Unicast packet counter. If bit 32 is set, counter overflowed.";
        data->is_err = 1;
    }
    if (dnx_graphical_epni < table->keys[0].size && 0 < table->keys[1].size)
    {
        data = (dnx_data_graphical_diag_counters_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_graphical_epni, 0);
        data->name = "DISCARD_CNT(ETPP Drops)";
        data->short_name = "DSCRD_TRNS";
        data->regs[0] = EPNI_EPNI_DISC_CNTr;
        data->flds[0] = EPNI_DISC_CNTf;
        data->doc = "counts packets that were discarded by the ETPP, and dropped in the aligner.";
    }
    if (dnx_graphical_epni < table->keys[0].size && 1 < table->keys[1].size)
    {
        data = (dnx_data_graphical_diag_counters_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_graphical_epni, 1);
        data->name = "IFC_TRANSACTIONS";
        data->short_name = "IFC_TRNS";
        data->regs[0] = EPNI_EPNI_IFC_CNTr;
        data->flds[0] = EPNI_IFC_CNTf;
        data->doc = "Counts transactions in EGQ interfaces, according to configuration in EPNI_EPNI_CNT_CFG:If MASK_IFC_TO_CHECK_BW = 0x3f, counting traffic in all queues;If MASK_IFC_TO_CHECK_BW = 0x0, counting traffic according to IFC_TO_CHECK_BW.";
    }
    if (dnx_graphical_epni < table->keys[0].size && 2 < table->keys[1].size)
    {
        data = (dnx_data_graphical_diag_counters_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_graphical_epni, 2);
        data->name = "MIRROR_TRANSACTIONS";
        data->short_name = "MIRROR_TRNS";
        data->regs[0] = EPNI_EPNI_MIRR_CNTr;
        data->flds[0] = EPNI_MIRR_CNTf;
        data->doc = "Counts transactions in egress queues, according to configuration in EPNI_EPNI_CNT_CFG:If MASK_MIRR_PRIO_TO_CHECK_BW = 0x3, counting traffic for both priorities;If MASK_MIRR_PRIO _TO_CHECK_BW = 0x0, counting traffic according to MIRR_PRIO_TO_CHECK_BW.";
    }
    if (dnx_graphical_epni < table->keys[0].size && 3 < table->keys[1].size)
    {
        data = (dnx_data_graphical_diag_counters_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_graphical_epni, 3);
        data->name = "QUEUE_TRANSACTIONS";
        data->short_name = "Q_TRNS";
        data->regs[0] = EPNI_EPNI_Q_CNTr;
        data->flds[0] = EPNI_Q_CNTf;
        data->doc = "Counts transactions in egress queues, according to configuration in EPNI_EPNI_CNT_CFG:If MASK_Q_TO_CHECK_BW = 0x3ff, counting traffic in all queues;If MASK_Q_TO_CHECK_BW = 0x0, counting traffic according to Q_TO_CHECK_BW.";
    }
    if (dnx_graphical_nif_tx < table->keys[0].size && 0 < table->keys[1].size)
    {
        data = (dnx_data_graphical_diag_counters_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_graphical_nif_tx, 0);
        data->name = "TX_TOTAL_PACKTS";
        data->short_name = "TTL_PKTS";
        SHR_RANGE_VERIFY(2, 0, DNX_DIAG_COUNTER_REG_NOF_REGS, _SHR_E_INTERNAL, "out of bound access to array")
        data->regs[0] = CDU_TX_TOTAL_PKT_COUNTERr;
        data->regs[1] = CDUM_TX_TOTAL_PKT_COUNTERr;
        SHR_RANGE_VERIFY(2, 0, DNX_DIAG_COUNTER_REG_NOF_REGS, _SHR_E_INTERNAL, "out of bound access to array")
        data->flds[0] = TX_TOTAL_PKT_COUNTERf;
        data->flds[1] = TX_TOTAL_PKT_COUNTERf;
        data->fld_num = 2;
        data->doc = "Counter for total packets sent from EGQ to NIF";
    }
    if (dnx_graphical_nif_tx < table->keys[0].size && 1 < table->keys[1].size)
    {
        data = (dnx_data_graphical_diag_counters_t *) dnxc_data_mgmt_table_data_get(unit, table, dnx_graphical_nif_tx, 1);
        data->name = "TX_TOTAL_BYTES";
        data->short_name = "TTL_BYTS";
        SHR_RANGE_VERIFY(2, 0, DNX_DIAG_COUNTER_REG_NOF_REGS, _SHR_E_INTERNAL, "out of bound access to array")
        data->regs[0] = CDU_TX_TOTAL_BYTE_COUNTERr;
        data->regs[1] = CDUM_TX_TOTAL_BYTE_COUNTERr;
        SHR_RANGE_VERIFY(2, 0, DNX_DIAG_COUNTER_REG_NOF_REGS, _SHR_E_INTERNAL, "out of bound access to array")
        data->flds[0] = TX_TOTAL_BYTE_COUNTERf;
        data->flds[1] = TX_TOTAL_BYTE_COUNTERf;
        data->fld_num = 2;
        data->doc = "Counter for total bytes sent from EGQ to NIF";
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_graphical_diag_drop_reason_set(
    int unit)
{
    int index_index;
    dnx_data_graphical_diag_drop_reason_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_graphical;
    int submodule_index = dnx_data_graphical_submodule_diag;
    int table_index = dnx_data_graphical_diag_table_drop_reason;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = 4;
    table->info_get.key_size[0] = 4;

    
    table->values[0].default_val = "INVALIDr";
    table->values[1].default_val = "INVALIDf";
    table->values[2].default_val = "NULL";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_graphical_diag_drop_reason_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_graphical_diag_table_drop_reason");

    
    default_data = (dnx_data_graphical_diag_drop_reason_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->regs = INVALIDr;
    default_data->flds = INVALIDf;
    default_data->drop_reason_cb = NULL;
    
    for (index_index = 0; index_index < table->keys[0].size; index_index++)
    {
        data = (dnx_data_graphical_diag_drop_reason_t *) dnxc_data_mgmt_table_data_get(unit, table, index_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (0 < table->keys[0].size)
    {
        data = (dnx_data_graphical_diag_drop_reason_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
        data->regs = CGM_REJECT_STATUS_BITMAPr;
        data->flds = INVALIDf;
        data->drop_reason_cb = dnx_cgm_drop_reason_to_str;
    }
    if (1 < table->keys[0].size)
    {
        data = (dnx_data_graphical_diag_drop_reason_t *) dnxc_data_mgmt_table_data_get(unit, table, 1, 0);
        data->regs = ECGM_RQP_DISCARD_REASONSr;
        data->flds = RQP_DISCARD_REASONSf;
        data->drop_reason_cb = dnx_rqp_drop_reason_to_str;
    }
    if (2 < table->keys[0].size)
    {
        data = (dnx_data_graphical_diag_drop_reason_t *) dnxc_data_mgmt_table_data_get(unit, table, 2, 0);
        data->regs = ECGM_PQP_DISCARD_REASONSr;
        data->flds = PQP_DISCARD_REASONSf;
        data->drop_reason_cb = dnx_pqp_drop_reason_to_str;
    }
    if (3 < table->keys[0].size)
    {
        data = (dnx_data_graphical_diag_drop_reason_t *) dnxc_data_mgmt_table_data_get(unit, table, 3, 0);
        data->regs = PQP_VISIBILITY_PACKET_DEBUGr;
        data->flds = VISIBILITY_ERPP_DROPf;
        data->drop_reason_cb = dnx_erpp_drop_reason_to_str;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}



shr_error_e
jr2_a0_data_graphical_attach(
    int unit)
{
    dnxc_data_module_t *module = NULL;
    dnxc_data_submodule_t *submodule = NULL;
    dnxc_data_define_t *define = NULL;
    dnxc_data_feature_t *feature = NULL;
    dnxc_data_table_t *table = NULL;
    int module_index = dnx_data_module_graphical;
    int submodule_index = -1, data_index = -1;
    SHR_FUNC_INIT_VARS(unit);

    COMPILER_REFERENCE(define);
    COMPILER_REFERENCE(feature);
    COMPILER_REFERENCE(table);
    COMPILER_REFERENCE(submodule);
    COMPILER_REFERENCE(data_index);
    COMPILER_REFERENCE(submodule_index);
    module = &_dnxc_data[unit].modules[module_index];
    
    submodule_index = dnx_data_graphical_submodule_diag;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_graphical_diag_define_num_of_rows;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_graphical_diag_num_of_rows_set;
    data_index = dnx_data_graphical_diag_define_num_of_index;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_graphical_diag_num_of_index_set;
    data_index = dnx_data_graphical_diag_define_num_of_block_entries;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_graphical_diag_num_of_block_entries_set;

    

    
    data_index = dnx_data_graphical_diag_table_blocks_left;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_graphical_diag_blocks_left_set;
    data_index = dnx_data_graphical_diag_table_blocks_right;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_graphical_diag_blocks_right_set;
    data_index = dnx_data_graphical_diag_table_counters;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_graphical_diag_counters_set;
    data_index = dnx_data_graphical_diag_table_drop_reason;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_graphical_diag_drop_reason_set;

    SHR_FUNC_EXIT;
}
#undef BSL_LOG_MODULE

