
/* *INDENT-OFF* */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */
#ifndef _DNXF_DATA_INTR_H_

#define _DNXF_DATA_INTR_H_

#include <sal/limits.h>
#include <shared/utilex/utilex_framework.h>
#include <soc/property.h>
#include <soc/sand/shrextend/shrextend_debug.h>
#include <soc/dnxc/dnxc_data/dnxc_data_utils.h>
#include <soc/dnxc/dnxc_data/dnxc_data_mgmt.h>
#include <soc/dnxc/intr.h>
#include <soc/dnxf/dnxf_data/auto_generated/dnxf_data_max_intr.h>



#ifndef BCM_DNXF_SUPPORT
#error "This file is for use by DNXF family only!"
#endif



shr_error_e dnxf_data_if_intr_init(
    int unit);






typedef struct
{
    uint32 intr;
} dnxf_data_intr_general_active_on_init_t;


typedef struct
{
    uint32 intr;
} dnxf_data_intr_general_disable_on_init_t;


typedef struct
{
    uint32 intr;
} dnxf_data_intr_general_disable_print_on_init_t;


typedef struct
{
    uint32 intr;
} dnxf_data_intr_general_skip_clear_on_regress_t;


typedef struct
{
    uint32 intr;
} dnxf_data_intr_general_block_intr_field_mapping_t;


typedef struct
{
    uint32 start;
    uint32 end;
    uint32 vector;
} dnxf_data_intr_general_chip_intr_remap_t;


typedef struct
{
    uint32 bit;
} dnxf_data_intr_general_chip_intr_enable_t;



typedef enum
{
    dnxf_data_intr_general_new_access_support,
    dnxf_data_intr_general_soft_init_for_debug,

    
    _dnxf_data_intr_general_feature_nof
} dnxf_data_intr_general_feature_e;



typedef int(
    *dnxf_data_intr_general_feature_get_f) (
    int unit,
    dnxf_data_intr_general_feature_e feature);


typedef uint32(
    *dnxf_data_intr_general_nof_interrupts_get_f) (
    int unit);


typedef uint32(
    *dnxf_data_intr_general_polled_irq_mode_get_f) (
    int unit);


typedef uint32(
    *dnxf_data_intr_general_ser_event_generate_support_get_f) (
    int unit);


typedef uint32(
    *dnxf_data_intr_general_ser_reset_cb_en_support_get_f) (
    int unit);


typedef const dnxf_data_intr_general_active_on_init_t *(
    *dnxf_data_intr_general_active_on_init_get_f) (
    int unit,
    int offset);


typedef const dnxf_data_intr_general_disable_on_init_t *(
    *dnxf_data_intr_general_disable_on_init_get_f) (
    int unit,
    int offset);


typedef const dnxf_data_intr_general_disable_print_on_init_t *(
    *dnxf_data_intr_general_disable_print_on_init_get_f) (
    int unit,
    int offset);


typedef const dnxf_data_intr_general_skip_clear_on_regress_t *(
    *dnxf_data_intr_general_skip_clear_on_regress_get_f) (
    int unit,
    int offset);


typedef const dnxf_data_intr_general_block_intr_field_mapping_t *(
    *dnxf_data_intr_general_block_intr_field_mapping_get_f) (
    int unit,
    int offset);


typedef const dnxf_data_intr_general_chip_intr_remap_t *(
    *dnxf_data_intr_general_chip_intr_remap_get_f) (
    int unit,
    int offset);


typedef const dnxf_data_intr_general_chip_intr_enable_t *(
    *dnxf_data_intr_general_chip_intr_enable_get_f) (
    int unit,
    int offset);



typedef struct
{
    
    dnxf_data_intr_general_feature_get_f feature_get;
    
    dnxf_data_intr_general_nof_interrupts_get_f nof_interrupts_get;
    
    dnxf_data_intr_general_polled_irq_mode_get_f polled_irq_mode_get;
    
    dnxf_data_intr_general_ser_event_generate_support_get_f ser_event_generate_support_get;
    
    dnxf_data_intr_general_ser_reset_cb_en_support_get_f ser_reset_cb_en_support_get;
    
    dnxf_data_intr_general_active_on_init_get_f active_on_init_get;
    
    dnxc_data_table_info_get_f active_on_init_info_get;
    
    dnxf_data_intr_general_disable_on_init_get_f disable_on_init_get;
    
    dnxc_data_table_info_get_f disable_on_init_info_get;
    
    dnxf_data_intr_general_disable_print_on_init_get_f disable_print_on_init_get;
    
    dnxc_data_table_info_get_f disable_print_on_init_info_get;
    
    dnxf_data_intr_general_skip_clear_on_regress_get_f skip_clear_on_regress_get;
    
    dnxc_data_table_info_get_f skip_clear_on_regress_info_get;
    
    dnxf_data_intr_general_block_intr_field_mapping_get_f block_intr_field_mapping_get;
    
    dnxc_data_table_info_get_f block_intr_field_mapping_info_get;
    
    dnxf_data_intr_general_chip_intr_remap_get_f chip_intr_remap_get;
    
    dnxc_data_table_info_get_f chip_intr_remap_info_get;
    
    dnxf_data_intr_general_chip_intr_enable_get_f chip_intr_enable_get;
    
    dnxc_data_table_info_get_f chip_intr_enable_info_get;
} dnxf_data_if_intr_general_t;






typedef struct
{
    soc_reg_t reg;
    soc_field_t field;
    dnxc_mem_mask_mode_e mode;
} dnxf_data_intr_ser_mem_mask_t;


typedef struct
{
    soc_mem_t mem;
    int sram_banks_bits;
    int entry_used_bits;
} dnxf_data_intr_ser_xor_mem_t;



typedef enum
{

    
    _dnxf_data_intr_ser_feature_nof
} dnxf_data_intr_ser_feature_e;



typedef int(
    *dnxf_data_intr_ser_feature_get_f) (
    int unit,
    dnxf_data_intr_ser_feature_e feature);


typedef const dnxf_data_intr_ser_mem_mask_t *(
    *dnxf_data_intr_ser_mem_mask_get_f) (
    int unit,
    int index);


typedef const dnxf_data_intr_ser_xor_mem_t *(
    *dnxf_data_intr_ser_xor_mem_get_f) (
    int unit,
    int index);



typedef struct
{
    
    dnxf_data_intr_ser_feature_get_f feature_get;
    
    dnxf_data_intr_ser_mem_mask_get_f mem_mask_get;
    
    dnxc_data_table_info_get_f mem_mask_info_get;
    
    dnxf_data_intr_ser_xor_mem_get_f xor_mem_get;
    
    dnxc_data_table_info_get_f xor_mem_info_get;
} dnxf_data_if_intr_ser_t;





typedef struct
{
    
    dnxf_data_if_intr_general_t general;
    
    dnxf_data_if_intr_ser_t ser;
} dnxf_data_if_intr_t;




extern dnxf_data_if_intr_t dnxf_data_intr;


#endif 

