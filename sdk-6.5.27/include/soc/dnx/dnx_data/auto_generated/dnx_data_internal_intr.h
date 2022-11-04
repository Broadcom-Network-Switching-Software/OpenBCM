
/* *INDENT-OFF* */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */
#ifndef _DNX_DATA_INTERNAL_INTR_H_

#define _DNX_DATA_INTERNAL_INTR_H_

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_intr.h>



#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX family only!"
#endif



typedef enum
{
    dnx_data_intr_submodule_general,
    dnx_data_intr_submodule_ser,

    
    _dnx_data_intr_submodule_nof
} dnx_data_intr_submodule_e;








int dnx_data_intr_general_feature_get(
    int unit,
    dnx_data_intr_general_feature_e feature);



typedef enum
{
    dnx_data_intr_general_define_nof_interrupts,
    dnx_data_intr_general_define_polled_irq_mode,
    dnx_data_intr_general_define_polled_irq_priority,
    dnx_data_intr_general_define_polled_irq_delay,
    dnx_data_intr_general_define_epni_ebd_interrupt_assert_enable,
    dnx_data_intr_general_define_table_dma_enable,
    dnx_data_intr_general_define_tdma_timeout_usec,
    dnx_data_intr_general_define_tdma_intr_enable,
    dnx_data_intr_general_define_tslam_dma_enable,
    dnx_data_intr_general_define_tslam_timeout_usec,
    dnx_data_intr_general_define_tslam_intr_enable,
    dnx_data_intr_general_define_schan_timeout_usec,
    dnx_data_intr_general_define_schan_intr_enable,
    dnx_data_intr_general_define_miim_timeout_usec,
    dnx_data_intr_general_define_miim_intr_enable,
    dnx_data_intr_general_define_sbus_dma_interval,
    dnx_data_intr_general_define_sbus_dma_intr_enable,
    dnx_data_intr_general_define_mem_clear_chunk_size,
    dnx_data_intr_general_define_ser_event_generate_support,
    dnx_data_intr_general_define_ser_reset_cb_en_support,

    
    _dnx_data_intr_general_define_nof
} dnx_data_intr_general_define_e;



uint32 dnx_data_intr_general_nof_interrupts_get(
    int unit);


uint32 dnx_data_intr_general_polled_irq_mode_get(
    int unit);


int dnx_data_intr_general_polled_irq_priority_get(
    int unit);


uint32 dnx_data_intr_general_polled_irq_delay_get(
    int unit);


uint32 dnx_data_intr_general_epni_ebd_interrupt_assert_enable_get(
    int unit);


uint32 dnx_data_intr_general_table_dma_enable_get(
    int unit);


uint32 dnx_data_intr_general_tdma_timeout_usec_get(
    int unit);


uint32 dnx_data_intr_general_tdma_intr_enable_get(
    int unit);


uint32 dnx_data_intr_general_tslam_dma_enable_get(
    int unit);


uint32 dnx_data_intr_general_tslam_timeout_usec_get(
    int unit);


uint32 dnx_data_intr_general_tslam_intr_enable_get(
    int unit);


uint32 dnx_data_intr_general_schan_timeout_usec_get(
    int unit);


uint32 dnx_data_intr_general_schan_intr_enable_get(
    int unit);


uint32 dnx_data_intr_general_miim_timeout_usec_get(
    int unit);


uint32 dnx_data_intr_general_miim_intr_enable_get(
    int unit);


uint32 dnx_data_intr_general_sbus_dma_interval_get(
    int unit);


uint32 dnx_data_intr_general_sbus_dma_intr_enable_get(
    int unit);


uint32 dnx_data_intr_general_mem_clear_chunk_size_get(
    int unit);


uint32 dnx_data_intr_general_ser_event_generate_support_get(
    int unit);


uint32 dnx_data_intr_general_ser_reset_cb_en_support_get(
    int unit);



typedef enum
{
    dnx_data_intr_general_table_active_on_init,
    dnx_data_intr_general_table_disable_on_init,
    dnx_data_intr_general_table_disable_print_on_init,
    dnx_data_intr_general_table_block_intr_field_mapping,
    dnx_data_intr_general_table_skip_clear_on_init,
    dnx_data_intr_general_table_force_unmask_on_init,
    dnx_data_intr_general_table_skip_clear_on_regress,
    dnx_data_intr_general_table_framer_module_map,
    dnx_data_intr_general_table_chip_intr_remap,
    dnx_data_intr_general_table_chip_intr_enable,

    
    _dnx_data_intr_general_table_nof
} dnx_data_intr_general_table_e;



const dnx_data_intr_general_active_on_init_t * dnx_data_intr_general_active_on_init_get(
    int unit,
    int offset);


const dnx_data_intr_general_disable_on_init_t * dnx_data_intr_general_disable_on_init_get(
    int unit,
    int offset);


const dnx_data_intr_general_disable_print_on_init_t * dnx_data_intr_general_disable_print_on_init_get(
    int unit,
    int offset);


const dnx_data_intr_general_block_intr_field_mapping_t * dnx_data_intr_general_block_intr_field_mapping_get(
    int unit,
    int offset);


const dnx_data_intr_general_skip_clear_on_init_t * dnx_data_intr_general_skip_clear_on_init_get(
    int unit,
    int offset);


const dnx_data_intr_general_force_unmask_on_init_t * dnx_data_intr_general_force_unmask_on_init_get(
    int unit,
    int offset);


const dnx_data_intr_general_skip_clear_on_regress_t * dnx_data_intr_general_skip_clear_on_regress_get(
    int unit,
    int offset);


const dnx_data_intr_general_framer_module_map_t * dnx_data_intr_general_framer_module_map_get(
    int unit,
    int offset);


const dnx_data_intr_general_chip_intr_remap_t * dnx_data_intr_general_chip_intr_remap_get(
    int unit,
    int offset);


const dnx_data_intr_general_chip_intr_enable_t * dnx_data_intr_general_chip_intr_enable_get(
    int unit,
    int offset);



shr_error_e dnx_data_intr_general_active_on_init_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_intr_general_disable_on_init_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_intr_general_disable_print_on_init_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_intr_general_block_intr_field_mapping_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_intr_general_skip_clear_on_init_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_intr_general_force_unmask_on_init_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_intr_general_skip_clear_on_regress_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_intr_general_framer_module_map_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_intr_general_chip_intr_remap_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_intr_general_chip_intr_enable_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);



const dnxc_data_table_info_t * dnx_data_intr_general_active_on_init_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_intr_general_disable_on_init_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_intr_general_disable_print_on_init_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_intr_general_block_intr_field_mapping_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_intr_general_skip_clear_on_init_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_intr_general_force_unmask_on_init_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_intr_general_skip_clear_on_regress_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_intr_general_framer_module_map_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_intr_general_chip_intr_remap_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_intr_general_chip_intr_enable_info_get(
    int unit);






int dnx_data_intr_ser_feature_get(
    int unit,
    dnx_data_intr_ser_feature_e feature);



typedef enum
{
    dnx_data_intr_ser_define_tcam_protection_issue,
    dnx_data_intr_ser_define_new_tcam_scan_mode,

    
    _dnx_data_intr_ser_define_nof
} dnx_data_intr_ser_define_e;



uint32 dnx_data_intr_ser_tcam_protection_issue_get(
    int unit);


uint32 dnx_data_intr_ser_new_tcam_scan_mode_get(
    int unit);



typedef enum
{
    dnx_data_intr_ser_table_mem_mask,
    dnx_data_intr_ser_table_xor_mem,
    dnx_data_intr_ser_table_intr_exception,
    dnx_data_intr_ser_table_static_mem,
    dnx_data_intr_ser_table_dynamic_mem,
    dnx_data_intr_ser_table_mdb_mem_map,
    dnx_data_intr_ser_table_kaps_tcam_scan,
    dnx_data_intr_ser_table_severity_mem,
    dnx_data_intr_ser_table_tcam_scan,

    
    _dnx_data_intr_ser_table_nof
} dnx_data_intr_ser_table_e;



const dnx_data_intr_ser_mem_mask_t * dnx_data_intr_ser_mem_mask_get(
    int unit,
    int index);


const dnx_data_intr_ser_xor_mem_t * dnx_data_intr_ser_xor_mem_get(
    int unit,
    int index);


const dnx_data_intr_ser_intr_exception_t * dnx_data_intr_ser_intr_exception_get(
    int unit,
    int index);


const dnx_data_intr_ser_static_mem_t * dnx_data_intr_ser_static_mem_get(
    int unit,
    int index);


const dnx_data_intr_ser_dynamic_mem_t * dnx_data_intr_ser_dynamic_mem_get(
    int unit,
    int index);


const dnx_data_intr_ser_mdb_mem_map_t * dnx_data_intr_ser_mdb_mem_map_get(
    int unit,
    int index);


const dnx_data_intr_ser_kaps_tcam_scan_t * dnx_data_intr_ser_kaps_tcam_scan_get(
    int unit);


const dnx_data_intr_ser_severity_mem_t * dnx_data_intr_ser_severity_mem_get(
    int unit,
    int index);


const dnx_data_intr_ser_tcam_scan_t * dnx_data_intr_ser_tcam_scan_get(
    int unit,
    int index);



shr_error_e dnx_data_intr_ser_mem_mask_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_intr_ser_xor_mem_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_intr_ser_intr_exception_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_intr_ser_static_mem_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_intr_ser_dynamic_mem_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_intr_ser_mdb_mem_map_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_intr_ser_kaps_tcam_scan_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_intr_ser_severity_mem_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_intr_ser_tcam_scan_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);



const dnxc_data_table_info_t * dnx_data_intr_ser_mem_mask_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_intr_ser_xor_mem_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_intr_ser_intr_exception_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_intr_ser_static_mem_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_intr_ser_dynamic_mem_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_intr_ser_mdb_mem_map_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_intr_ser_kaps_tcam_scan_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_intr_ser_severity_mem_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_intr_ser_tcam_scan_info_get(
    int unit);



shr_error_e dnx_data_intr_init(
    int unit,
    dnxc_data_module_t *module_data);



#endif 

