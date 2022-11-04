
/* *INDENT-OFF* */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */
#ifndef _DNX_DATA_INTR_H_

#define _DNX_DATA_INTR_H_

#include <sal/limits.h>
#include <shared/utilex/utilex_framework.h>
#include <soc/property.h>
#include <soc/sand/shrextend/shrextend_debug.h>
#include <soc/dnxc/dnxc_data/dnxc_data_utils.h>
#include <soc/dnxc/dnxc_data/dnxc_data_mgmt.h>
#include <soc/dnxc/intr.h>
#include <soc/dnx/intr/dnx_intr.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_max_intr.h>



#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX family only!"
#endif



shr_error_e dnx_data_if_intr_init(
    int unit);






typedef struct
{
    uint32 intr;
} dnx_data_intr_general_active_on_init_t;


typedef struct
{
    uint32 intr;
} dnx_data_intr_general_disable_on_init_t;


typedef struct
{
    uint32 intr;
} dnx_data_intr_general_disable_print_on_init_t;


typedef struct
{
    uint32 intr;
} dnx_data_intr_general_block_intr_field_mapping_t;


typedef struct
{
    uint32 intr;
} dnx_data_intr_general_skip_clear_on_init_t;


typedef struct
{
    uint32 intr;
} dnx_data_intr_general_force_unmask_on_init_t;


typedef struct
{
    uint32 intr;
} dnx_data_intr_general_skip_clear_on_regress_t;


typedef struct
{
    soc_mem_t core_regs;
    uint32 instance;
} dnx_data_intr_general_framer_module_map_t;


typedef struct
{
    uint32 start;
    uint32 end;
    uint32 vector;
} dnx_data_intr_general_chip_intr_remap_t;


typedef struct
{
    uint32 bit;
} dnx_data_intr_general_chip_intr_enable_t;



typedef enum
{
    dnx_data_intr_general_new_access_support,
    dnx_data_intr_general_soft_init_for_debug,

    
    _dnx_data_intr_general_feature_nof
} dnx_data_intr_general_feature_e;



typedef int(
    *dnx_data_intr_general_feature_get_f) (
    int unit,
    dnx_data_intr_general_feature_e feature);


typedef uint32(
    *dnx_data_intr_general_nof_interrupts_get_f) (
    int unit);


typedef uint32(
    *dnx_data_intr_general_polled_irq_mode_get_f) (
    int unit);


typedef int(
    *dnx_data_intr_general_polled_irq_priority_get_f) (
    int unit);


typedef uint32(
    *dnx_data_intr_general_polled_irq_delay_get_f) (
    int unit);


typedef uint32(
    *dnx_data_intr_general_epni_ebd_interrupt_assert_enable_get_f) (
    int unit);


typedef uint32(
    *dnx_data_intr_general_table_dma_enable_get_f) (
    int unit);


typedef uint32(
    *dnx_data_intr_general_tdma_timeout_usec_get_f) (
    int unit);


typedef uint32(
    *dnx_data_intr_general_tdma_intr_enable_get_f) (
    int unit);


typedef uint32(
    *dnx_data_intr_general_tslam_dma_enable_get_f) (
    int unit);


typedef uint32(
    *dnx_data_intr_general_tslam_timeout_usec_get_f) (
    int unit);


typedef uint32(
    *dnx_data_intr_general_tslam_intr_enable_get_f) (
    int unit);


typedef uint32(
    *dnx_data_intr_general_schan_timeout_usec_get_f) (
    int unit);


typedef uint32(
    *dnx_data_intr_general_schan_intr_enable_get_f) (
    int unit);


typedef uint32(
    *dnx_data_intr_general_miim_timeout_usec_get_f) (
    int unit);


typedef uint32(
    *dnx_data_intr_general_miim_intr_enable_get_f) (
    int unit);


typedef uint32(
    *dnx_data_intr_general_sbus_dma_interval_get_f) (
    int unit);


typedef uint32(
    *dnx_data_intr_general_sbus_dma_intr_enable_get_f) (
    int unit);


typedef uint32(
    *dnx_data_intr_general_mem_clear_chunk_size_get_f) (
    int unit);


typedef uint32(
    *dnx_data_intr_general_ser_event_generate_support_get_f) (
    int unit);


typedef uint32(
    *dnx_data_intr_general_ser_reset_cb_en_support_get_f) (
    int unit);


typedef const dnx_data_intr_general_active_on_init_t *(
    *dnx_data_intr_general_active_on_init_get_f) (
    int unit,
    int offset);


typedef const dnx_data_intr_general_disable_on_init_t *(
    *dnx_data_intr_general_disable_on_init_get_f) (
    int unit,
    int offset);


typedef const dnx_data_intr_general_disable_print_on_init_t *(
    *dnx_data_intr_general_disable_print_on_init_get_f) (
    int unit,
    int offset);


typedef const dnx_data_intr_general_block_intr_field_mapping_t *(
    *dnx_data_intr_general_block_intr_field_mapping_get_f) (
    int unit,
    int offset);


typedef const dnx_data_intr_general_skip_clear_on_init_t *(
    *dnx_data_intr_general_skip_clear_on_init_get_f) (
    int unit,
    int offset);


typedef const dnx_data_intr_general_force_unmask_on_init_t *(
    *dnx_data_intr_general_force_unmask_on_init_get_f) (
    int unit,
    int offset);


typedef const dnx_data_intr_general_skip_clear_on_regress_t *(
    *dnx_data_intr_general_skip_clear_on_regress_get_f) (
    int unit,
    int offset);


typedef const dnx_data_intr_general_framer_module_map_t *(
    *dnx_data_intr_general_framer_module_map_get_f) (
    int unit,
    int offset);


typedef const dnx_data_intr_general_chip_intr_remap_t *(
    *dnx_data_intr_general_chip_intr_remap_get_f) (
    int unit,
    int offset);


typedef const dnx_data_intr_general_chip_intr_enable_t *(
    *dnx_data_intr_general_chip_intr_enable_get_f) (
    int unit,
    int offset);



typedef struct
{
    
    dnx_data_intr_general_feature_get_f feature_get;
    
    dnx_data_intr_general_nof_interrupts_get_f nof_interrupts_get;
    
    dnx_data_intr_general_polled_irq_mode_get_f polled_irq_mode_get;
    
    dnx_data_intr_general_polled_irq_priority_get_f polled_irq_priority_get;
    
    dnx_data_intr_general_polled_irq_delay_get_f polled_irq_delay_get;
    
    dnx_data_intr_general_epni_ebd_interrupt_assert_enable_get_f epni_ebd_interrupt_assert_enable_get;
    
    dnx_data_intr_general_table_dma_enable_get_f table_dma_enable_get;
    
    dnx_data_intr_general_tdma_timeout_usec_get_f tdma_timeout_usec_get;
    
    dnx_data_intr_general_tdma_intr_enable_get_f tdma_intr_enable_get;
    
    dnx_data_intr_general_tslam_dma_enable_get_f tslam_dma_enable_get;
    
    dnx_data_intr_general_tslam_timeout_usec_get_f tslam_timeout_usec_get;
    
    dnx_data_intr_general_tslam_intr_enable_get_f tslam_intr_enable_get;
    
    dnx_data_intr_general_schan_timeout_usec_get_f schan_timeout_usec_get;
    
    dnx_data_intr_general_schan_intr_enable_get_f schan_intr_enable_get;
    
    dnx_data_intr_general_miim_timeout_usec_get_f miim_timeout_usec_get;
    
    dnx_data_intr_general_miim_intr_enable_get_f miim_intr_enable_get;
    
    dnx_data_intr_general_sbus_dma_interval_get_f sbus_dma_interval_get;
    
    dnx_data_intr_general_sbus_dma_intr_enable_get_f sbus_dma_intr_enable_get;
    
    dnx_data_intr_general_mem_clear_chunk_size_get_f mem_clear_chunk_size_get;
    
    dnx_data_intr_general_ser_event_generate_support_get_f ser_event_generate_support_get;
    
    dnx_data_intr_general_ser_reset_cb_en_support_get_f ser_reset_cb_en_support_get;
    
    dnx_data_intr_general_active_on_init_get_f active_on_init_get;
    
    dnxc_data_table_info_get_f active_on_init_info_get;
    
    dnx_data_intr_general_disable_on_init_get_f disable_on_init_get;
    
    dnxc_data_table_info_get_f disable_on_init_info_get;
    
    dnx_data_intr_general_disable_print_on_init_get_f disable_print_on_init_get;
    
    dnxc_data_table_info_get_f disable_print_on_init_info_get;
    
    dnx_data_intr_general_block_intr_field_mapping_get_f block_intr_field_mapping_get;
    
    dnxc_data_table_info_get_f block_intr_field_mapping_info_get;
    
    dnx_data_intr_general_skip_clear_on_init_get_f skip_clear_on_init_get;
    
    dnxc_data_table_info_get_f skip_clear_on_init_info_get;
    
    dnx_data_intr_general_force_unmask_on_init_get_f force_unmask_on_init_get;
    
    dnxc_data_table_info_get_f force_unmask_on_init_info_get;
    
    dnx_data_intr_general_skip_clear_on_regress_get_f skip_clear_on_regress_get;
    
    dnxc_data_table_info_get_f skip_clear_on_regress_info_get;
    
    dnx_data_intr_general_framer_module_map_get_f framer_module_map_get;
    
    dnxc_data_table_info_get_f framer_module_map_info_get;
    
    dnx_data_intr_general_chip_intr_remap_get_f chip_intr_remap_get;
    
    dnxc_data_table_info_get_f chip_intr_remap_info_get;
    
    dnx_data_intr_general_chip_intr_enable_get_f chip_intr_enable_get;
    
    dnxc_data_table_info_get_f chip_intr_enable_info_get;
} dnx_data_if_intr_general_t;






typedef struct
{
    soc_reg_t reg;
    soc_field_t field;
    dnxc_mem_mask_mode_e mode;
} dnx_data_intr_ser_mem_mask_t;


typedef struct
{
    soc_mem_t mem;
    int sram_banks_bits;
    int entry_used_bits;
    int flags;
} dnx_data_intr_ser_xor_mem_t;


typedef struct
{
    uint32 intr;
    dnx_intr_exceptional_get_f exception_get_cb;
} dnx_data_intr_ser_intr_exception_t;


typedef struct
{
    soc_mem_t mem;
} dnx_data_intr_ser_static_mem_t;


typedef struct
{
    soc_mem_t mem;
} dnx_data_intr_ser_dynamic_mem_t;


typedef struct
{
    soc_mem_t acc_mem;
    soc_mem_t phy_mem;
} dnx_data_intr_ser_mdb_mem_map_t;


typedef struct
{
    uint32 scan_window;
    uint32 scan_pause;
    uint32 uncorrectable_action;
    uint32 correctable_action;
} dnx_data_intr_ser_kaps_tcam_scan_t;


typedef struct
{
    soc_mem_t mem;
    int severity;
} dnx_data_intr_ser_severity_mem_t;


typedef struct
{
    uint32 scan_cfg_reg;
    uint32 scan_pause_reg;
    uint32 scan_index_reg;
    uint32 tcam_memory;
    uint32 scan_window;
    uint32 dec_action;
    uint32 sec_action;
    uint32 dec_pause;
    uint32 sec_pause;
    uint32 shadowed;
} dnx_data_intr_ser_tcam_scan_t;



typedef enum
{

    
    _dnx_data_intr_ser_feature_nof
} dnx_data_intr_ser_feature_e;



typedef int(
    *dnx_data_intr_ser_feature_get_f) (
    int unit,
    dnx_data_intr_ser_feature_e feature);


typedef uint32(
    *dnx_data_intr_ser_tcam_protection_issue_get_f) (
    int unit);


typedef uint32(
    *dnx_data_intr_ser_new_tcam_scan_mode_get_f) (
    int unit);


typedef const dnx_data_intr_ser_mem_mask_t *(
    *dnx_data_intr_ser_mem_mask_get_f) (
    int unit,
    int index);


typedef const dnx_data_intr_ser_xor_mem_t *(
    *dnx_data_intr_ser_xor_mem_get_f) (
    int unit,
    int index);


typedef const dnx_data_intr_ser_intr_exception_t *(
    *dnx_data_intr_ser_intr_exception_get_f) (
    int unit,
    int index);


typedef const dnx_data_intr_ser_static_mem_t *(
    *dnx_data_intr_ser_static_mem_get_f) (
    int unit,
    int index);


typedef const dnx_data_intr_ser_dynamic_mem_t *(
    *dnx_data_intr_ser_dynamic_mem_get_f) (
    int unit,
    int index);


typedef const dnx_data_intr_ser_mdb_mem_map_t *(
    *dnx_data_intr_ser_mdb_mem_map_get_f) (
    int unit,
    int index);


typedef const dnx_data_intr_ser_kaps_tcam_scan_t *(
    *dnx_data_intr_ser_kaps_tcam_scan_get_f) (
    int unit);


typedef const dnx_data_intr_ser_severity_mem_t *(
    *dnx_data_intr_ser_severity_mem_get_f) (
    int unit,
    int index);


typedef const dnx_data_intr_ser_tcam_scan_t *(
    *dnx_data_intr_ser_tcam_scan_get_f) (
    int unit,
    int index);



typedef struct
{
    
    dnx_data_intr_ser_feature_get_f feature_get;
    
    dnx_data_intr_ser_tcam_protection_issue_get_f tcam_protection_issue_get;
    
    dnx_data_intr_ser_new_tcam_scan_mode_get_f new_tcam_scan_mode_get;
    
    dnx_data_intr_ser_mem_mask_get_f mem_mask_get;
    
    dnxc_data_table_info_get_f mem_mask_info_get;
    
    dnx_data_intr_ser_xor_mem_get_f xor_mem_get;
    
    dnxc_data_table_info_get_f xor_mem_info_get;
    
    dnx_data_intr_ser_intr_exception_get_f intr_exception_get;
    
    dnxc_data_table_info_get_f intr_exception_info_get;
    
    dnx_data_intr_ser_static_mem_get_f static_mem_get;
    
    dnxc_data_table_info_get_f static_mem_info_get;
    
    dnx_data_intr_ser_dynamic_mem_get_f dynamic_mem_get;
    
    dnxc_data_table_info_get_f dynamic_mem_info_get;
    
    dnx_data_intr_ser_mdb_mem_map_get_f mdb_mem_map_get;
    
    dnxc_data_table_info_get_f mdb_mem_map_info_get;
    
    dnx_data_intr_ser_kaps_tcam_scan_get_f kaps_tcam_scan_get;
    
    dnxc_data_table_info_get_f kaps_tcam_scan_info_get;
    
    dnx_data_intr_ser_severity_mem_get_f severity_mem_get;
    
    dnxc_data_table_info_get_f severity_mem_info_get;
    
    dnx_data_intr_ser_tcam_scan_get_f tcam_scan_get;
    
    dnxc_data_table_info_get_f tcam_scan_info_get;
} dnx_data_if_intr_ser_t;





typedef struct
{
    
    dnx_data_if_intr_general_t general;
    
    dnx_data_if_intr_ser_t ser;
} dnx_data_if_intr_t;




extern dnx_data_if_intr_t dnx_data_intr;


#endif 

