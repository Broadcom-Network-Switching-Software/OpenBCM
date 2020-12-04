

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifndef _DNX_DATA_INTERNAL_GRAPHICAL_H_

#define _DNX_DATA_INTERNAL_GRAPHICAL_H_

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_graphical.h>



#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif



typedef enum
{
    dnx_data_graphical_submodule_diag,

    
    _dnx_data_graphical_submodule_nof
} dnx_data_graphical_submodule_e;








int dnx_data_graphical_diag_feature_get(
    int unit,
    dnx_data_graphical_diag_feature_e feature);



typedef enum
{
    dnx_data_graphical_diag_define_num_of_rows,
    dnx_data_graphical_diag_define_num_of_index,
    dnx_data_graphical_diag_define_num_of_block_entries,

    
    _dnx_data_graphical_diag_define_nof
} dnx_data_graphical_diag_define_e;



uint32 dnx_data_graphical_diag_num_of_rows_get(
    int unit);


uint32 dnx_data_graphical_diag_num_of_index_get(
    int unit);


uint32 dnx_data_graphical_diag_num_of_block_entries_get(
    int unit);



typedef enum
{
    dnx_data_graphical_diag_table_blocks_left,
    dnx_data_graphical_diag_table_blocks_right,
    dnx_data_graphical_diag_table_counters,
    dnx_data_graphical_diag_table_drop_reason,

    
    _dnx_data_graphical_diag_table_nof
} dnx_data_graphical_diag_table_e;



const dnx_data_graphical_diag_blocks_left_t * dnx_data_graphical_diag_blocks_left_get(
    int unit,
    int row,
    int index);


const dnx_data_graphical_diag_blocks_right_t * dnx_data_graphical_diag_blocks_right_get(
    int unit,
    int row,
    int index);


const dnx_data_graphical_diag_counters_t * dnx_data_graphical_diag_counters_get(
    int unit,
    int block,
    int index);


const dnx_data_graphical_diag_drop_reason_t * dnx_data_graphical_diag_drop_reason_get(
    int unit,
    int index);



shr_error_e dnx_data_graphical_diag_blocks_left_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_graphical_diag_blocks_right_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_graphical_diag_counters_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_graphical_diag_drop_reason_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);



const dnxc_data_table_info_t * dnx_data_graphical_diag_blocks_left_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_graphical_diag_blocks_right_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_graphical_diag_counters_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_graphical_diag_drop_reason_info_get(
    int unit);



shr_error_e dnx_data_graphical_init(
    int unit,
    dnxc_data_module_t *module_data);



#endif 

