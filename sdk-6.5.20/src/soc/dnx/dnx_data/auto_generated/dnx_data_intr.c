

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_INTR

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_intr.h>



extern shr_error_e jr2_a0_data_intr_attach(
    int unit);
extern shr_error_e jr2_b0_data_intr_attach(
    int unit);
extern shr_error_e j2c_a0_data_intr_attach(
    int unit);
extern shr_error_e q2a_a0_data_intr_attach(
    int unit);
extern shr_error_e j2p_a0_data_intr_attach(
    int unit);



static shr_error_e
dnx_data_intr_general_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "general";
    submodule_data->doc = "General interrupt information.";
    
    submodule_data->nof_features = _dnx_data_intr_general_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data intr general features");

    
    submodule_data->nof_defines = _dnx_data_intr_general_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data intr general defines");

    submodule_data->defines[dnx_data_intr_general_define_nof_interrupts].name = "nof_interrupts";
    submodule_data->defines[dnx_data_intr_general_define_nof_interrupts].doc = "Number of interrupts in the device.";
    
    submodule_data->defines[dnx_data_intr_general_define_nof_interrupts].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_intr_general_define_polled_irq_mode].name = "polled_irq_mode";
    submodule_data->defines[dnx_data_intr_general_define_polled_irq_mode].doc = "Are interrupts supported polling mode.";
    
    submodule_data->defines[dnx_data_intr_general_define_polled_irq_mode].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_intr_general_define_polled_irq_priority].name = "polled_irq_priority";
    submodule_data->defines[dnx_data_intr_general_define_polled_irq_priority].doc = "The priority of IRQ polling thread.";
    
    submodule_data->defines[dnx_data_intr_general_define_polled_irq_priority].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_intr_general_define_polled_irq_delay].name = "polled_irq_delay";
    submodule_data->defines[dnx_data_intr_general_define_polled_irq_delay].doc = "The minimum delay between IRQ polls.";
    
    submodule_data->defines[dnx_data_intr_general_define_polled_irq_delay].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_intr_general_define_table_dma_enable].name = "table_dma_enable";
    submodule_data->defines[dnx_data_intr_general_define_table_dma_enable].doc = "Enable/disable table DMA operations.";
    
    submodule_data->defines[dnx_data_intr_general_define_table_dma_enable].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_intr_general_define_tdma_timeout_usec].name = "tdma_timeout_usec";
    submodule_data->defines[dnx_data_intr_general_define_tdma_timeout_usec].doc = "Table DMA operation timeout.";
    
    submodule_data->defines[dnx_data_intr_general_define_tdma_timeout_usec].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_intr_general_define_tdma_intr_enable].name = "tdma_intr_enable";
    submodule_data->defines[dnx_data_intr_general_define_tdma_intr_enable].doc = "Table DMA done is interrupt driven or by polling.";
    
    submodule_data->defines[dnx_data_intr_general_define_tdma_intr_enable].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_intr_general_define_tslam_dma_enable].name = "tslam_dma_enable";
    submodule_data->defines[dnx_data_intr_general_define_tslam_dma_enable].doc = "Enable/disable tslam DMA operations.";
    
    submodule_data->defines[dnx_data_intr_general_define_tslam_dma_enable].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_intr_general_define_tslam_timeout_usec].name = "tslam_timeout_usec";
    submodule_data->defines[dnx_data_intr_general_define_tslam_timeout_usec].doc = "Tslam DMA operation timeout.";
    
    submodule_data->defines[dnx_data_intr_general_define_tslam_timeout_usec].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_intr_general_define_tslam_intr_enable].name = "tslam_intr_enable";
    submodule_data->defines[dnx_data_intr_general_define_tslam_intr_enable].doc = "Tslam DMA done is interrupt driven or by polling.";
    
    submodule_data->defines[dnx_data_intr_general_define_tslam_intr_enable].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_intr_general_define_schan_timeout_usec].name = "schan_timeout_usec";
    submodule_data->defines[dnx_data_intr_general_define_schan_timeout_usec].doc = "Schan operation timeout.";
    
    submodule_data->defines[dnx_data_intr_general_define_schan_timeout_usec].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_intr_general_define_schan_intr_enable].name = "schan_intr_enable";
    submodule_data->defines[dnx_data_intr_general_define_schan_intr_enable].doc = "Schan operation done is interrupt driven or by polling.";
    
    submodule_data->defines[dnx_data_intr_general_define_schan_intr_enable].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_intr_general_define_miim_timeout_usec].name = "miim_timeout_usec";
    submodule_data->defines[dnx_data_intr_general_define_miim_timeout_usec].doc = "MIM operation timeout.";
    
    submodule_data->defines[dnx_data_intr_general_define_miim_timeout_usec].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_intr_general_define_miim_intr_enable].name = "miim_intr_enable";
    submodule_data->defines[dnx_data_intr_general_define_miim_intr_enable].doc = "MIM operation done is interrupt driver or by polling.";
    
    submodule_data->defines[dnx_data_intr_general_define_miim_intr_enable].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_intr_general_define_sbus_dma_interval].name = "sbus_dma_interval";
    submodule_data->defines[dnx_data_intr_general_define_sbus_dma_interval].doc = "sbus DMA operation interval.";
    
    submodule_data->defines[dnx_data_intr_general_define_sbus_dma_interval].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_intr_general_define_sbus_dma_intr_enable].name = "sbus_dma_intr_enable";
    submodule_data->defines[dnx_data_intr_general_define_sbus_dma_intr_enable].doc = "sbus DMA interrupt enable.";
    
    submodule_data->defines[dnx_data_intr_general_define_sbus_dma_intr_enable].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_intr_general_define_mem_clear_chunk_size].name = "mem_clear_chunk_size";
    submodule_data->defines[dnx_data_intr_general_define_mem_clear_chunk_size].doc = "sbus DMA chunk size.";
    
    submodule_data->defines[dnx_data_intr_general_define_mem_clear_chunk_size].flags |= DNXC_DATA_F_NUMERIC;

    
    submodule_data->nof_tables = _dnx_data_intr_general_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data intr general tables");

    
    submodule_data->tables[dnx_data_intr_general_table_active_on_init].name = "active_on_init";
    submodule_data->tables[dnx_data_intr_general_table_active_on_init].doc = "interrupt active on init";
    submodule_data->tables[dnx_data_intr_general_table_active_on_init].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_intr_general_table_active_on_init].size_of_values = sizeof(dnx_data_intr_general_active_on_init_t);
    submodule_data->tables[dnx_data_intr_general_table_active_on_init].entry_get = dnx_data_intr_general_active_on_init_entry_str_get;

    
    submodule_data->tables[dnx_data_intr_general_table_active_on_init].nof_keys = 1;
    submodule_data->tables[dnx_data_intr_general_table_active_on_init].keys[0].name = "offset";
    submodule_data->tables[dnx_data_intr_general_table_active_on_init].keys[0].doc = "active interrupt offset";

    
    submodule_data->tables[dnx_data_intr_general_table_active_on_init].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_intr_general_table_active_on_init].values, dnxc_data_value_t, submodule_data->tables[dnx_data_intr_general_table_active_on_init].nof_values, "_dnx_data_intr_general_table_active_on_init table values");
    submodule_data->tables[dnx_data_intr_general_table_active_on_init].values[0].name = "intr";
    submodule_data->tables[dnx_data_intr_general_table_active_on_init].values[0].type = "uint32";
    submodule_data->tables[dnx_data_intr_general_table_active_on_init].values[0].doc = "interrupt ident";
    submodule_data->tables[dnx_data_intr_general_table_active_on_init].values[0].offset = UTILEX_OFFSETOF(dnx_data_intr_general_active_on_init_t, intr);

    
    submodule_data->tables[dnx_data_intr_general_table_disable_on_init].name = "disable_on_init";
    submodule_data->tables[dnx_data_intr_general_table_disable_on_init].doc = "interrupt disabled on init";
    submodule_data->tables[dnx_data_intr_general_table_disable_on_init].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_intr_general_table_disable_on_init].size_of_values = sizeof(dnx_data_intr_general_disable_on_init_t);
    submodule_data->tables[dnx_data_intr_general_table_disable_on_init].entry_get = dnx_data_intr_general_disable_on_init_entry_str_get;

    
    submodule_data->tables[dnx_data_intr_general_table_disable_on_init].nof_keys = 1;
    submodule_data->tables[dnx_data_intr_general_table_disable_on_init].keys[0].name = "offset";
    submodule_data->tables[dnx_data_intr_general_table_disable_on_init].keys[0].doc = "disable interrupt offset";

    
    submodule_data->tables[dnx_data_intr_general_table_disable_on_init].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_intr_general_table_disable_on_init].values, dnxc_data_value_t, submodule_data->tables[dnx_data_intr_general_table_disable_on_init].nof_values, "_dnx_data_intr_general_table_disable_on_init table values");
    submodule_data->tables[dnx_data_intr_general_table_disable_on_init].values[0].name = "intr";
    submodule_data->tables[dnx_data_intr_general_table_disable_on_init].values[0].type = "uint32";
    submodule_data->tables[dnx_data_intr_general_table_disable_on_init].values[0].doc = "interrupt ident";
    submodule_data->tables[dnx_data_intr_general_table_disable_on_init].values[0].offset = UTILEX_OFFSETOF(dnx_data_intr_general_disable_on_init_t, intr);

    
    submodule_data->tables[dnx_data_intr_general_table_disable_print_on_init].name = "disable_print_on_init";
    submodule_data->tables[dnx_data_intr_general_table_disable_print_on_init].doc = "interrupt disable print on init";
    submodule_data->tables[dnx_data_intr_general_table_disable_print_on_init].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_intr_general_table_disable_print_on_init].size_of_values = sizeof(dnx_data_intr_general_disable_print_on_init_t);
    submodule_data->tables[dnx_data_intr_general_table_disable_print_on_init].entry_get = dnx_data_intr_general_disable_print_on_init_entry_str_get;

    
    submodule_data->tables[dnx_data_intr_general_table_disable_print_on_init].nof_keys = 1;
    submodule_data->tables[dnx_data_intr_general_table_disable_print_on_init].keys[0].name = "offset";
    submodule_data->tables[dnx_data_intr_general_table_disable_print_on_init].keys[0].doc = "disable print on init offset";

    
    submodule_data->tables[dnx_data_intr_general_table_disable_print_on_init].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_intr_general_table_disable_print_on_init].values, dnxc_data_value_t, submodule_data->tables[dnx_data_intr_general_table_disable_print_on_init].nof_values, "_dnx_data_intr_general_table_disable_print_on_init table values");
    submodule_data->tables[dnx_data_intr_general_table_disable_print_on_init].values[0].name = "intr";
    submodule_data->tables[dnx_data_intr_general_table_disable_print_on_init].values[0].type = "uint32";
    submodule_data->tables[dnx_data_intr_general_table_disable_print_on_init].values[0].doc = "interrupt ident";
    submodule_data->tables[dnx_data_intr_general_table_disable_print_on_init].values[0].offset = UTILEX_OFFSETOF(dnx_data_intr_general_disable_print_on_init_t, intr);

    
    submodule_data->tables[dnx_data_intr_general_table_block_intr_field_mapping].name = "block_intr_field_mapping";
    submodule_data->tables[dnx_data_intr_general_table_block_intr_field_mapping].doc = "block to intr field mapping";
    submodule_data->tables[dnx_data_intr_general_table_block_intr_field_mapping].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_intr_general_table_block_intr_field_mapping].size_of_values = sizeof(dnx_data_intr_general_block_intr_field_mapping_t);
    submodule_data->tables[dnx_data_intr_general_table_block_intr_field_mapping].entry_get = dnx_data_intr_general_block_intr_field_mapping_entry_str_get;

    
    submodule_data->tables[dnx_data_intr_general_table_block_intr_field_mapping].nof_keys = 1;
    submodule_data->tables[dnx_data_intr_general_table_block_intr_field_mapping].keys[0].name = "offset";
    submodule_data->tables[dnx_data_intr_general_table_block_intr_field_mapping].keys[0].doc = "block to intr field mapping";

    
    submodule_data->tables[dnx_data_intr_general_table_block_intr_field_mapping].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_intr_general_table_block_intr_field_mapping].values, dnxc_data_value_t, submodule_data->tables[dnx_data_intr_general_table_block_intr_field_mapping].nof_values, "_dnx_data_intr_general_table_block_intr_field_mapping table values");
    submodule_data->tables[dnx_data_intr_general_table_block_intr_field_mapping].values[0].name = "intr";
    submodule_data->tables[dnx_data_intr_general_table_block_intr_field_mapping].values[0].type = "uint32";
    submodule_data->tables[dnx_data_intr_general_table_block_intr_field_mapping].values[0].doc = "intr field";
    submodule_data->tables[dnx_data_intr_general_table_block_intr_field_mapping].values[0].offset = UTILEX_OFFSETOF(dnx_data_intr_general_block_intr_field_mapping_t, intr);


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_intr_general_feature_get(
    int unit,
    dnx_data_intr_general_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_intr, dnx_data_intr_submodule_general, feature);
}


uint32
dnx_data_intr_general_nof_interrupts_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_intr, dnx_data_intr_submodule_general, dnx_data_intr_general_define_nof_interrupts);
}

uint32
dnx_data_intr_general_polled_irq_mode_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_intr, dnx_data_intr_submodule_general, dnx_data_intr_general_define_polled_irq_mode);
}

int
dnx_data_intr_general_polled_irq_priority_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_intr, dnx_data_intr_submodule_general, dnx_data_intr_general_define_polled_irq_priority);
}

uint32
dnx_data_intr_general_polled_irq_delay_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_intr, dnx_data_intr_submodule_general, dnx_data_intr_general_define_polled_irq_delay);
}

uint32
dnx_data_intr_general_table_dma_enable_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_intr, dnx_data_intr_submodule_general, dnx_data_intr_general_define_table_dma_enable);
}

uint32
dnx_data_intr_general_tdma_timeout_usec_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_intr, dnx_data_intr_submodule_general, dnx_data_intr_general_define_tdma_timeout_usec);
}

uint32
dnx_data_intr_general_tdma_intr_enable_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_intr, dnx_data_intr_submodule_general, dnx_data_intr_general_define_tdma_intr_enable);
}

uint32
dnx_data_intr_general_tslam_dma_enable_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_intr, dnx_data_intr_submodule_general, dnx_data_intr_general_define_tslam_dma_enable);
}

uint32
dnx_data_intr_general_tslam_timeout_usec_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_intr, dnx_data_intr_submodule_general, dnx_data_intr_general_define_tslam_timeout_usec);
}

uint32
dnx_data_intr_general_tslam_intr_enable_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_intr, dnx_data_intr_submodule_general, dnx_data_intr_general_define_tslam_intr_enable);
}

uint32
dnx_data_intr_general_schan_timeout_usec_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_intr, dnx_data_intr_submodule_general, dnx_data_intr_general_define_schan_timeout_usec);
}

uint32
dnx_data_intr_general_schan_intr_enable_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_intr, dnx_data_intr_submodule_general, dnx_data_intr_general_define_schan_intr_enable);
}

uint32
dnx_data_intr_general_miim_timeout_usec_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_intr, dnx_data_intr_submodule_general, dnx_data_intr_general_define_miim_timeout_usec);
}

uint32
dnx_data_intr_general_miim_intr_enable_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_intr, dnx_data_intr_submodule_general, dnx_data_intr_general_define_miim_intr_enable);
}

uint32
dnx_data_intr_general_sbus_dma_interval_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_intr, dnx_data_intr_submodule_general, dnx_data_intr_general_define_sbus_dma_interval);
}

uint32
dnx_data_intr_general_sbus_dma_intr_enable_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_intr, dnx_data_intr_submodule_general, dnx_data_intr_general_define_sbus_dma_intr_enable);
}

uint32
dnx_data_intr_general_mem_clear_chunk_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_intr, dnx_data_intr_submodule_general, dnx_data_intr_general_define_mem_clear_chunk_size);
}



const dnx_data_intr_general_active_on_init_t *
dnx_data_intr_general_active_on_init_get(
    int unit,
    int offset)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_intr, dnx_data_intr_submodule_general, dnx_data_intr_general_table_active_on_init);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, offset, 0);
    return (const dnx_data_intr_general_active_on_init_t *) data;

}

const dnx_data_intr_general_disable_on_init_t *
dnx_data_intr_general_disable_on_init_get(
    int unit,
    int offset)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_intr, dnx_data_intr_submodule_general, dnx_data_intr_general_table_disable_on_init);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, offset, 0);
    return (const dnx_data_intr_general_disable_on_init_t *) data;

}

const dnx_data_intr_general_disable_print_on_init_t *
dnx_data_intr_general_disable_print_on_init_get(
    int unit,
    int offset)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_intr, dnx_data_intr_submodule_general, dnx_data_intr_general_table_disable_print_on_init);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, offset, 0);
    return (const dnx_data_intr_general_disable_print_on_init_t *) data;

}

const dnx_data_intr_general_block_intr_field_mapping_t *
dnx_data_intr_general_block_intr_field_mapping_get(
    int unit,
    int offset)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_intr, dnx_data_intr_submodule_general, dnx_data_intr_general_table_block_intr_field_mapping);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, offset, 0);
    return (const dnx_data_intr_general_block_intr_field_mapping_t *) data;

}


shr_error_e
dnx_data_intr_general_active_on_init_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_intr_general_active_on_init_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_intr, dnx_data_intr_submodule_general, dnx_data_intr_general_table_active_on_init);
    data = (const dnx_data_intr_general_active_on_init_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->intr);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_intr_general_disable_on_init_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_intr_general_disable_on_init_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_intr, dnx_data_intr_submodule_general, dnx_data_intr_general_table_disable_on_init);
    data = (const dnx_data_intr_general_disable_on_init_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->intr);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_intr_general_disable_print_on_init_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_intr_general_disable_print_on_init_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_intr, dnx_data_intr_submodule_general, dnx_data_intr_general_table_disable_print_on_init);
    data = (const dnx_data_intr_general_disable_print_on_init_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->intr);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_intr_general_block_intr_field_mapping_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_intr_general_block_intr_field_mapping_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_intr, dnx_data_intr_submodule_general, dnx_data_intr_general_table_block_intr_field_mapping);
    data = (const dnx_data_intr_general_block_intr_field_mapping_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->intr);
            break;
    }

    SHR_FUNC_EXIT;
}


const dnxc_data_table_info_t *
dnx_data_intr_general_active_on_init_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_intr, dnx_data_intr_submodule_general, dnx_data_intr_general_table_active_on_init);

}

const dnxc_data_table_info_t *
dnx_data_intr_general_disable_on_init_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_intr, dnx_data_intr_submodule_general, dnx_data_intr_general_table_disable_on_init);

}

const dnxc_data_table_info_t *
dnx_data_intr_general_disable_print_on_init_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_intr, dnx_data_intr_submodule_general, dnx_data_intr_general_table_disable_print_on_init);

}

const dnxc_data_table_info_t *
dnx_data_intr_general_block_intr_field_mapping_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_intr, dnx_data_intr_submodule_general, dnx_data_intr_general_table_block_intr_field_mapping);

}






static shr_error_e
dnx_data_intr_ser_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "ser";
    submodule_data->doc = "SER handle information.";
    
    submodule_data->nof_features = _dnx_data_intr_ser_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data intr ser features");

    
    submodule_data->nof_defines = _dnx_data_intr_ser_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data intr ser defines");

    submodule_data->defines[dnx_data_intr_ser_define_tcam_protection_issue].name = "tcam_protection_issue";
    submodule_data->defines[dnx_data_intr_ser_define_tcam_protection_issue].doc = "If there are tcam protect machine issue at device\n";
    
    submodule_data->defines[dnx_data_intr_ser_define_tcam_protection_issue].flags |= DNXC_DATA_F_DEFINE;

    
    submodule_data->nof_tables = _dnx_data_intr_ser_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data intr ser tables");

    
    submodule_data->tables[dnx_data_intr_ser_table_mem_mask].name = "mem_mask";
    submodule_data->tables[dnx_data_intr_ser_table_mem_mask].doc = "memory need masked";
    submodule_data->tables[dnx_data_intr_ser_table_mem_mask].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_intr_ser_table_mem_mask].size_of_values = sizeof(dnx_data_intr_ser_mem_mask_t);
    submodule_data->tables[dnx_data_intr_ser_table_mem_mask].entry_get = dnx_data_intr_ser_mem_mask_entry_str_get;

    
    submodule_data->tables[dnx_data_intr_ser_table_mem_mask].nof_keys = 1;
    submodule_data->tables[dnx_data_intr_ser_table_mem_mask].keys[0].name = "index";
    submodule_data->tables[dnx_data_intr_ser_table_mem_mask].keys[0].doc = "memory masked index";

    
    submodule_data->tables[dnx_data_intr_ser_table_mem_mask].nof_values = 3;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_intr_ser_table_mem_mask].values, dnxc_data_value_t, submodule_data->tables[dnx_data_intr_ser_table_mem_mask].nof_values, "_dnx_data_intr_ser_table_mem_mask table values");
    submodule_data->tables[dnx_data_intr_ser_table_mem_mask].values[0].name = "reg";
    submodule_data->tables[dnx_data_intr_ser_table_mem_mask].values[0].type = "soc_reg_t";
    submodule_data->tables[dnx_data_intr_ser_table_mem_mask].values[0].doc = "register name for  memory mask";
    submodule_data->tables[dnx_data_intr_ser_table_mem_mask].values[0].offset = UTILEX_OFFSETOF(dnx_data_intr_ser_mem_mask_t, reg);
    submodule_data->tables[dnx_data_intr_ser_table_mem_mask].values[1].name = "field";
    submodule_data->tables[dnx_data_intr_ser_table_mem_mask].values[1].type = "soc_field_t";
    submodule_data->tables[dnx_data_intr_ser_table_mem_mask].values[1].doc = "If field defined - will set just the specified field";
    submodule_data->tables[dnx_data_intr_ser_table_mem_mask].values[1].offset = UTILEX_OFFSETOF(dnx_data_intr_ser_mem_mask_t, field);
    submodule_data->tables[dnx_data_intr_ser_table_mem_mask].values[2].name = "mode";
    submodule_data->tables[dnx_data_intr_ser_table_mem_mask].values[2].type = "dnxc_mem_mask_mode_e";
    submodule_data->tables[dnx_data_intr_ser_table_mem_mask].values[2].doc = "Register field should be set value, 0 or 1";
    submodule_data->tables[dnx_data_intr_ser_table_mem_mask].values[2].offset = UTILEX_OFFSETOF(dnx_data_intr_ser_mem_mask_t, mode);

    
    submodule_data->tables[dnx_data_intr_ser_table_xor_mem].name = "xor_mem";
    submodule_data->tables[dnx_data_intr_ser_table_xor_mem].doc = "XOR memory information";
    submodule_data->tables[dnx_data_intr_ser_table_xor_mem].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_intr_ser_table_xor_mem].size_of_values = sizeof(dnx_data_intr_ser_xor_mem_t);
    submodule_data->tables[dnx_data_intr_ser_table_xor_mem].entry_get = dnx_data_intr_ser_xor_mem_entry_str_get;

    
    submodule_data->tables[dnx_data_intr_ser_table_xor_mem].nof_keys = 1;
    submodule_data->tables[dnx_data_intr_ser_table_xor_mem].keys[0].name = "index";
    submodule_data->tables[dnx_data_intr_ser_table_xor_mem].keys[0].doc = "XOR memory index";

    
    submodule_data->tables[dnx_data_intr_ser_table_xor_mem].nof_values = 4;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_intr_ser_table_xor_mem].values, dnxc_data_value_t, submodule_data->tables[dnx_data_intr_ser_table_xor_mem].nof_values, "_dnx_data_intr_ser_table_xor_mem table values");
    submodule_data->tables[dnx_data_intr_ser_table_xor_mem].values[0].name = "mem";
    submodule_data->tables[dnx_data_intr_ser_table_xor_mem].values[0].type = "soc_mem_t";
    submodule_data->tables[dnx_data_intr_ser_table_xor_mem].values[0].doc = "xor memory name";
    submodule_data->tables[dnx_data_intr_ser_table_xor_mem].values[0].offset = UTILEX_OFFSETOF(dnx_data_intr_ser_xor_mem_t, mem);
    submodule_data->tables[dnx_data_intr_ser_table_xor_mem].values[1].name = "sram_banks_bits";
    submodule_data->tables[dnx_data_intr_ser_table_xor_mem].values[1].type = "int";
    submodule_data->tables[dnx_data_intr_ser_table_xor_mem].values[1].doc = "SRAM bank wide bits";
    submodule_data->tables[dnx_data_intr_ser_table_xor_mem].values[1].offset = UTILEX_OFFSETOF(dnx_data_intr_ser_xor_mem_t, sram_banks_bits);
    submodule_data->tables[dnx_data_intr_ser_table_xor_mem].values[2].name = "entry_used_bits";
    submodule_data->tables[dnx_data_intr_ser_table_xor_mem].values[2].type = "int";
    submodule_data->tables[dnx_data_intr_ser_table_xor_mem].values[2].doc = "Entry number address wide for each bank";
    submodule_data->tables[dnx_data_intr_ser_table_xor_mem].values[2].offset = UTILEX_OFFSETOF(dnx_data_intr_ser_xor_mem_t, entry_used_bits);
    submodule_data->tables[dnx_data_intr_ser_table_xor_mem].values[3].name = "flags";
    submodule_data->tables[dnx_data_intr_ser_table_xor_mem].values[3].type = "int";
    submodule_data->tables[dnx_data_intr_ser_table_xor_mem].values[3].doc = "xor memory flags";
    submodule_data->tables[dnx_data_intr_ser_table_xor_mem].values[3].offset = UTILEX_OFFSETOF(dnx_data_intr_ser_xor_mem_t, flags);

    
    submodule_data->tables[dnx_data_intr_ser_table_intr_exception].name = "intr_exception";
    submodule_data->tables[dnx_data_intr_ser_table_intr_exception].doc = "interrupt exceptional";
    submodule_data->tables[dnx_data_intr_ser_table_intr_exception].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_intr_ser_table_intr_exception].size_of_values = sizeof(dnx_data_intr_ser_intr_exception_t);
    submodule_data->tables[dnx_data_intr_ser_table_intr_exception].entry_get = dnx_data_intr_ser_intr_exception_entry_str_get;

    
    submodule_data->tables[dnx_data_intr_ser_table_intr_exception].nof_keys = 1;
    submodule_data->tables[dnx_data_intr_ser_table_intr_exception].keys[0].name = "index";
    submodule_data->tables[dnx_data_intr_ser_table_intr_exception].keys[0].doc = "exceptional_intr index";

    
    submodule_data->tables[dnx_data_intr_ser_table_intr_exception].nof_values = 2;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_intr_ser_table_intr_exception].values, dnxc_data_value_t, submodule_data->tables[dnx_data_intr_ser_table_intr_exception].nof_values, "_dnx_data_intr_ser_table_intr_exception table values");
    submodule_data->tables[dnx_data_intr_ser_table_intr_exception].values[0].name = "intr";
    submodule_data->tables[dnx_data_intr_ser_table_intr_exception].values[0].type = "uint32";
    submodule_data->tables[dnx_data_intr_ser_table_intr_exception].values[0].doc = "interrupt ident";
    submodule_data->tables[dnx_data_intr_ser_table_intr_exception].values[0].offset = UTILEX_OFFSETOF(dnx_data_intr_ser_intr_exception_t, intr);
    submodule_data->tables[dnx_data_intr_ser_table_intr_exception].values[1].name = "exception_get_cb";
    submodule_data->tables[dnx_data_intr_ser_table_intr_exception].values[1].type = "dnx_intr_exceptional_get_f";
    submodule_data->tables[dnx_data_intr_ser_table_intr_exception].values[1].doc = "callback to get if it is exceptional interrupt";
    submodule_data->tables[dnx_data_intr_ser_table_intr_exception].values[1].offset = UTILEX_OFFSETOF(dnx_data_intr_ser_intr_exception_t, exception_get_cb);

    
    submodule_data->tables[dnx_data_intr_ser_table_static_mem].name = "static_mem";
    submodule_data->tables[dnx_data_intr_ser_table_static_mem].doc = "static memory";
    submodule_data->tables[dnx_data_intr_ser_table_static_mem].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_intr_ser_table_static_mem].size_of_values = sizeof(dnx_data_intr_ser_static_mem_t);
    submodule_data->tables[dnx_data_intr_ser_table_static_mem].entry_get = dnx_data_intr_ser_static_mem_entry_str_get;

    
    submodule_data->tables[dnx_data_intr_ser_table_static_mem].nof_keys = 1;
    submodule_data->tables[dnx_data_intr_ser_table_static_mem].keys[0].name = "index";
    submodule_data->tables[dnx_data_intr_ser_table_static_mem].keys[0].doc = "static memory index";

    
    submodule_data->tables[dnx_data_intr_ser_table_static_mem].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_intr_ser_table_static_mem].values, dnxc_data_value_t, submodule_data->tables[dnx_data_intr_ser_table_static_mem].nof_values, "_dnx_data_intr_ser_table_static_mem table values");
    submodule_data->tables[dnx_data_intr_ser_table_static_mem].values[0].name = "mem";
    submodule_data->tables[dnx_data_intr_ser_table_static_mem].values[0].type = "soc_mem_t";
    submodule_data->tables[dnx_data_intr_ser_table_static_mem].values[0].doc = "memory";
    submodule_data->tables[dnx_data_intr_ser_table_static_mem].values[0].offset = UTILEX_OFFSETOF(dnx_data_intr_ser_static_mem_t, mem);

    
    submodule_data->tables[dnx_data_intr_ser_table_mdb_mem_map].name = "mdb_mem_map";
    submodule_data->tables[dnx_data_intr_ser_table_mdb_mem_map].doc = "mdb memory mapping";
    submodule_data->tables[dnx_data_intr_ser_table_mdb_mem_map].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_intr_ser_table_mdb_mem_map].size_of_values = sizeof(dnx_data_intr_ser_mdb_mem_map_t);
    submodule_data->tables[dnx_data_intr_ser_table_mdb_mem_map].entry_get = dnx_data_intr_ser_mdb_mem_map_entry_str_get;

    
    submodule_data->tables[dnx_data_intr_ser_table_mdb_mem_map].nof_keys = 1;
    submodule_data->tables[dnx_data_intr_ser_table_mdb_mem_map].keys[0].name = "index";
    submodule_data->tables[dnx_data_intr_ser_table_mdb_mem_map].keys[0].doc = "mdb memory index";

    
    submodule_data->tables[dnx_data_intr_ser_table_mdb_mem_map].nof_values = 2;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_intr_ser_table_mdb_mem_map].values, dnxc_data_value_t, submodule_data->tables[dnx_data_intr_ser_table_mdb_mem_map].nof_values, "_dnx_data_intr_ser_table_mdb_mem_map table values");
    submodule_data->tables[dnx_data_intr_ser_table_mdb_mem_map].values[0].name = "acc_mem";
    submodule_data->tables[dnx_data_intr_ser_table_mdb_mem_map].values[0].type = "soc_mem_t";
    submodule_data->tables[dnx_data_intr_ser_table_mdb_mem_map].values[0].doc = "acc_mem";
    submodule_data->tables[dnx_data_intr_ser_table_mdb_mem_map].values[0].offset = UTILEX_OFFSETOF(dnx_data_intr_ser_mdb_mem_map_t, acc_mem);
    submodule_data->tables[dnx_data_intr_ser_table_mdb_mem_map].values[1].name = "phy_mem";
    submodule_data->tables[dnx_data_intr_ser_table_mdb_mem_map].values[1].type = "soc_mem_t";
    submodule_data->tables[dnx_data_intr_ser_table_mdb_mem_map].values[1].doc = "phy_mem";
    submodule_data->tables[dnx_data_intr_ser_table_mdb_mem_map].values[1].offset = UTILEX_OFFSETOF(dnx_data_intr_ser_mdb_mem_map_t, phy_mem);

    
    submodule_data->tables[dnx_data_intr_ser_table_kaps_tcam_scan].name = "kaps_tcam_scan";
    submodule_data->tables[dnx_data_intr_ser_table_kaps_tcam_scan].doc = "kaps tcam scan parameter";
    submodule_data->tables[dnx_data_intr_ser_table_kaps_tcam_scan].labels[0] = "j2p_notrev";
    submodule_data->tables[dnx_data_intr_ser_table_kaps_tcam_scan].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_intr_ser_table_kaps_tcam_scan].size_of_values = sizeof(dnx_data_intr_ser_kaps_tcam_scan_t);
    submodule_data->tables[dnx_data_intr_ser_table_kaps_tcam_scan].entry_get = dnx_data_intr_ser_kaps_tcam_scan_entry_str_get;

    
    submodule_data->tables[dnx_data_intr_ser_table_kaps_tcam_scan].nof_keys = 0;

    
    submodule_data->tables[dnx_data_intr_ser_table_kaps_tcam_scan].nof_values = 4;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_intr_ser_table_kaps_tcam_scan].values, dnxc_data_value_t, submodule_data->tables[dnx_data_intr_ser_table_kaps_tcam_scan].nof_values, "_dnx_data_intr_ser_table_kaps_tcam_scan table values");
    submodule_data->tables[dnx_data_intr_ser_table_kaps_tcam_scan].values[0].name = "scan_window";
    submodule_data->tables[dnx_data_intr_ser_table_kaps_tcam_scan].values[0].type = "uint32";
    submodule_data->tables[dnx_data_intr_ser_table_kaps_tcam_scan].values[0].doc = "Kaps tcam scan window count size";
    submodule_data->tables[dnx_data_intr_ser_table_kaps_tcam_scan].values[0].offset = UTILEX_OFFSETOF(dnx_data_intr_ser_kaps_tcam_scan_t, scan_window);
    submodule_data->tables[dnx_data_intr_ser_table_kaps_tcam_scan].values[1].name = "scan_pause";
    submodule_data->tables[dnx_data_intr_ser_table_kaps_tcam_scan].values[1].type = "uint32";
    submodule_data->tables[dnx_data_intr_ser_table_kaps_tcam_scan].values[1].doc = "If pause scan machine";
    submodule_data->tables[dnx_data_intr_ser_table_kaps_tcam_scan].values[1].offset = UTILEX_OFFSETOF(dnx_data_intr_ser_kaps_tcam_scan_t, scan_pause);
    submodule_data->tables[dnx_data_intr_ser_table_kaps_tcam_scan].values[2].name = "uncorrectable_action";
    submodule_data->tables[dnx_data_intr_ser_table_kaps_tcam_scan].values[2].type = "uint32";
    submodule_data->tables[dnx_data_intr_ser_table_kaps_tcam_scan].values[2].doc = "action performed on uncorrectable entries";
    submodule_data->tables[dnx_data_intr_ser_table_kaps_tcam_scan].values[2].offset = UTILEX_OFFSETOF(dnx_data_intr_ser_kaps_tcam_scan_t, uncorrectable_action);
    submodule_data->tables[dnx_data_intr_ser_table_kaps_tcam_scan].values[3].name = "correctable_action";
    submodule_data->tables[dnx_data_intr_ser_table_kaps_tcam_scan].values[3].type = "uint32";
    submodule_data->tables[dnx_data_intr_ser_table_kaps_tcam_scan].values[3].doc = "action performed on correctable entries";
    submodule_data->tables[dnx_data_intr_ser_table_kaps_tcam_scan].values[3].offset = UTILEX_OFFSETOF(dnx_data_intr_ser_kaps_tcam_scan_t, correctable_action);


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_intr_ser_feature_get(
    int unit,
    dnx_data_intr_ser_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_intr, dnx_data_intr_submodule_ser, feature);
}


uint32
dnx_data_intr_ser_tcam_protection_issue_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_intr, dnx_data_intr_submodule_ser, dnx_data_intr_ser_define_tcam_protection_issue);
}



const dnx_data_intr_ser_mem_mask_t *
dnx_data_intr_ser_mem_mask_get(
    int unit,
    int index)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_intr, dnx_data_intr_submodule_ser, dnx_data_intr_ser_table_mem_mask);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, index, 0);
    return (const dnx_data_intr_ser_mem_mask_t *) data;

}

const dnx_data_intr_ser_xor_mem_t *
dnx_data_intr_ser_xor_mem_get(
    int unit,
    int index)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_intr, dnx_data_intr_submodule_ser, dnx_data_intr_ser_table_xor_mem);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, index, 0);
    return (const dnx_data_intr_ser_xor_mem_t *) data;

}

const dnx_data_intr_ser_intr_exception_t *
dnx_data_intr_ser_intr_exception_get(
    int unit,
    int index)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_intr, dnx_data_intr_submodule_ser, dnx_data_intr_ser_table_intr_exception);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, index, 0);
    return (const dnx_data_intr_ser_intr_exception_t *) data;

}

const dnx_data_intr_ser_static_mem_t *
dnx_data_intr_ser_static_mem_get(
    int unit,
    int index)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_intr, dnx_data_intr_submodule_ser, dnx_data_intr_ser_table_static_mem);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, index, 0);
    return (const dnx_data_intr_ser_static_mem_t *) data;

}

const dnx_data_intr_ser_mdb_mem_map_t *
dnx_data_intr_ser_mdb_mem_map_get(
    int unit,
    int index)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_intr, dnx_data_intr_submodule_ser, dnx_data_intr_ser_table_mdb_mem_map);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, index, 0);
    return (const dnx_data_intr_ser_mdb_mem_map_t *) data;

}

const dnx_data_intr_ser_kaps_tcam_scan_t *
dnx_data_intr_ser_kaps_tcam_scan_get(
    int unit)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_intr, dnx_data_intr_submodule_ser, dnx_data_intr_ser_table_kaps_tcam_scan);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
    return (const dnx_data_intr_ser_kaps_tcam_scan_t *) data;

}


shr_error_e
dnx_data_intr_ser_mem_mask_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_intr_ser_mem_mask_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_intr, dnx_data_intr_submodule_ser, dnx_data_intr_ser_table_mem_mask);
    data = (const dnx_data_intr_ser_mem_mask_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->reg);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->field);
            break;
        case 2:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->mode);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_intr_ser_xor_mem_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_intr_ser_xor_mem_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_intr, dnx_data_intr_submodule_ser, dnx_data_intr_ser_table_xor_mem);
    data = (const dnx_data_intr_ser_xor_mem_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->mem);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->sram_banks_bits);
            break;
        case 2:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->entry_used_bits);
            break;
        case 3:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->flags);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_intr_ser_intr_exception_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_intr_ser_intr_exception_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_intr, dnx_data_intr_submodule_ser, dnx_data_intr_ser_table_intr_exception);
    data = (const dnx_data_intr_ser_intr_exception_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->intr);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%s", data->exception_get_cb == NULL ? "NULL" : "func");
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_intr_ser_static_mem_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_intr_ser_static_mem_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_intr, dnx_data_intr_submodule_ser, dnx_data_intr_ser_table_static_mem);
    data = (const dnx_data_intr_ser_static_mem_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->mem);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_intr_ser_mdb_mem_map_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_intr_ser_mdb_mem_map_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_intr, dnx_data_intr_submodule_ser, dnx_data_intr_ser_table_mdb_mem_map);
    data = (const dnx_data_intr_ser_mdb_mem_map_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->acc_mem);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->phy_mem);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_intr_ser_kaps_tcam_scan_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_intr_ser_kaps_tcam_scan_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_intr, dnx_data_intr_submodule_ser, dnx_data_intr_ser_table_kaps_tcam_scan);
    data = (const dnx_data_intr_ser_kaps_tcam_scan_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, 0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->scan_window);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->scan_pause);
            break;
        case 2:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->uncorrectable_action);
            break;
        case 3:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->correctable_action);
            break;
    }

    SHR_FUNC_EXIT;
}


const dnxc_data_table_info_t *
dnx_data_intr_ser_mem_mask_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_intr, dnx_data_intr_submodule_ser, dnx_data_intr_ser_table_mem_mask);

}

const dnxc_data_table_info_t *
dnx_data_intr_ser_xor_mem_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_intr, dnx_data_intr_submodule_ser, dnx_data_intr_ser_table_xor_mem);

}

const dnxc_data_table_info_t *
dnx_data_intr_ser_intr_exception_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_intr, dnx_data_intr_submodule_ser, dnx_data_intr_ser_table_intr_exception);

}

const dnxc_data_table_info_t *
dnx_data_intr_ser_static_mem_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_intr, dnx_data_intr_submodule_ser, dnx_data_intr_ser_table_static_mem);

}

const dnxc_data_table_info_t *
dnx_data_intr_ser_mdb_mem_map_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_intr, dnx_data_intr_submodule_ser, dnx_data_intr_ser_table_mdb_mem_map);

}

const dnxc_data_table_info_t *
dnx_data_intr_ser_kaps_tcam_scan_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_intr, dnx_data_intr_submodule_ser, dnx_data_intr_ser_table_kaps_tcam_scan);

}



shr_error_e
dnx_data_intr_init(
    int unit,
    dnxc_data_module_t *module_data)
{
    SHR_FUNC_INIT_VARS(unit);

    
    module_data->name = "intr";
    module_data->nof_submodules = _dnx_data_intr_submodule_nof;
    DNXC_DATA_ALLOC(module_data->submodules, dnxc_data_submodule_t, module_data->nof_submodules, "_dnxc_data intr submodules");

    
    SHR_IF_ERR_EXIT(dnx_data_intr_general_init(unit, &module_data->submodules[dnx_data_intr_submodule_general]));
    SHR_IF_ERR_EXIT(dnx_data_intr_ser_init(unit, &module_data->submodules[dnx_data_intr_submodule_ser]));
    
    if (dnxc_data_mgmt_is_jr2_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_intr_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_jr2_b0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_intr_attach(unit));
        SHR_IF_ERR_EXIT(jr2_b0_data_intr_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_jr2_b1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_intr_attach(unit));
        SHR_IF_ERR_EXIT(jr2_b0_data_intr_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_j2c_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_intr_attach(unit));
        SHR_IF_ERR_EXIT(j2c_a0_data_intr_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_j2c_a1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_intr_attach(unit));
        SHR_IF_ERR_EXIT(j2c_a0_data_intr_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_q2a_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_intr_attach(unit));
        SHR_IF_ERR_EXIT(q2a_a0_data_intr_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_q2a_b0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_intr_attach(unit));
        SHR_IF_ERR_EXIT(q2a_a0_data_intr_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_q2a_b1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_intr_attach(unit));
        SHR_IF_ERR_EXIT(q2a_a0_data_intr_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_j2p_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_intr_attach(unit));
        SHR_IF_ERR_EXIT(j2p_a0_data_intr_attach(unit));
    }
    else
    {
        SHR_EXIT();
    }

exit:
    SHR_FUNC_EXIT;
}
#undef BSL_LOG_MODULE

