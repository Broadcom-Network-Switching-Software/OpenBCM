
/* *INDENT-OFF* */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */
#ifndef _DNX_DATA_INTERNAL_TECHSUPPORT_H_

#define _DNX_DATA_INTERNAL_TECHSUPPORT_H_

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_techsupport.h>



#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX family only!"
#endif



typedef enum
{
    dnx_data_techsupport_submodule_tm,

    
    _dnx_data_techsupport_submodule_nof
} dnx_data_techsupport_submodule_e;








int dnx_data_techsupport_tm_feature_get(
    int unit,
    dnx_data_techsupport_tm_feature_e feature);



typedef enum
{
    dnx_data_techsupport_tm_define_num_of_tables_per_block,
    dnx_data_techsupport_tm_define_num_of_tm_command,

    
    _dnx_data_techsupport_tm_define_nof
} dnx_data_techsupport_tm_define_e;



uint32 dnx_data_techsupport_tm_num_of_tables_per_block_get(
    int unit);


uint32 dnx_data_techsupport_tm_num_of_tm_command_get(
    int unit);



typedef enum
{
    dnx_data_techsupport_tm_table_tm_block,
    dnx_data_techsupport_tm_table_tm_command,
    dnx_data_techsupport_tm_table_tm_mem,

    
    _dnx_data_techsupport_tm_table_nof
} dnx_data_techsupport_tm_table_e;



const dnx_data_techsupport_tm_tm_block_t * dnx_data_techsupport_tm_tm_block_get(
    int unit,
    int block);


const dnx_data_techsupport_tm_tm_command_t * dnx_data_techsupport_tm_tm_command_get(
    int unit,
    int index);


const dnx_data_techsupport_tm_tm_mem_t * dnx_data_techsupport_tm_tm_mem_get(
    int unit,
    int block,
    int index);



shr_error_e dnx_data_techsupport_tm_tm_block_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_techsupport_tm_tm_command_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_techsupport_tm_tm_mem_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);



const dnxc_data_table_info_t * dnx_data_techsupport_tm_tm_block_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_techsupport_tm_tm_command_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_techsupport_tm_tm_mem_info_get(
    int unit);



shr_error_e dnx_data_techsupport_init(
    int unit,
    dnxc_data_module_t *module_data);



#endif 

