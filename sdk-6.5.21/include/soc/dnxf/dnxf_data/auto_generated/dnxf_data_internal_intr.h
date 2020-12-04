

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifndef _DNXF_DATA_INTERNAL_INTR_H_

#define _DNXF_DATA_INTERNAL_INTR_H_

#include <soc/dnxf/dnxf_data/auto_generated/dnxf_data_internal.h>
#include <soc/dnxf/dnxf_data/auto_generated/dnxf_data_intr.h>



#ifndef BCM_DNXF_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif



typedef enum
{
    dnxf_data_intr_submodule_general,
    dnxf_data_intr_submodule_ser,

    
    _dnxf_data_intr_submodule_nof
} dnxf_data_intr_submodule_e;








int dnxf_data_intr_general_feature_get(
    int unit,
    dnxf_data_intr_general_feature_e feature);



typedef enum
{
    dnxf_data_intr_general_define_nof_interrupts,
    dnxf_data_intr_general_define_polled_irq_mode,

    
    _dnxf_data_intr_general_define_nof
} dnxf_data_intr_general_define_e;



uint32 dnxf_data_intr_general_nof_interrupts_get(
    int unit);


uint32 dnxf_data_intr_general_polled_irq_mode_get(
    int unit);



typedef enum
{
    dnxf_data_intr_general_table_active_on_init,
    dnxf_data_intr_general_table_disable_on_init,
    dnxf_data_intr_general_table_disable_print_on_init,

    
    _dnxf_data_intr_general_table_nof
} dnxf_data_intr_general_table_e;



const dnxf_data_intr_general_active_on_init_t * dnxf_data_intr_general_active_on_init_get(
    int unit,
    int offset);


const dnxf_data_intr_general_disable_on_init_t * dnxf_data_intr_general_disable_on_init_get(
    int unit,
    int offset);


const dnxf_data_intr_general_disable_print_on_init_t * dnxf_data_intr_general_disable_print_on_init_get(
    int unit,
    int offset);



shr_error_e dnxf_data_intr_general_active_on_init_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnxf_data_intr_general_disable_on_init_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnxf_data_intr_general_disable_print_on_init_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);



const dnxc_data_table_info_t * dnxf_data_intr_general_active_on_init_info_get(
    int unit);


const dnxc_data_table_info_t * dnxf_data_intr_general_disable_on_init_info_get(
    int unit);


const dnxc_data_table_info_t * dnxf_data_intr_general_disable_print_on_init_info_get(
    int unit);






int dnxf_data_intr_ser_feature_get(
    int unit,
    dnxf_data_intr_ser_feature_e feature);



typedef enum
{

    
    _dnxf_data_intr_ser_define_nof
} dnxf_data_intr_ser_define_e;




typedef enum
{
    dnxf_data_intr_ser_table_mem_mask,
    dnxf_data_intr_ser_table_xor_mem,

    
    _dnxf_data_intr_ser_table_nof
} dnxf_data_intr_ser_table_e;



const dnxf_data_intr_ser_mem_mask_t * dnxf_data_intr_ser_mem_mask_get(
    int unit,
    int index);


const dnxf_data_intr_ser_xor_mem_t * dnxf_data_intr_ser_xor_mem_get(
    int unit,
    int index);



shr_error_e dnxf_data_intr_ser_mem_mask_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnxf_data_intr_ser_xor_mem_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);



const dnxc_data_table_info_t * dnxf_data_intr_ser_mem_mask_info_get(
    int unit);


const dnxc_data_table_info_t * dnxf_data_intr_ser_xor_mem_info_get(
    int unit);



shr_error_e dnxf_data_intr_init(
    int unit,
    dnxc_data_module_t *module_data);



#endif 

