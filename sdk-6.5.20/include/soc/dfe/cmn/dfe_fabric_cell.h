/*
* 
* This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
* 
* Copyright 2007-2020 Broadcom Inc. All rights reserved.
*
* This file contains structures and functions declarations for 
* In-band cell configuration and Source Routed Cell.
 */
#ifndef _SOC_DFE_FABRIC_CELL_H
#define _SOC_DFE_FABRIC_CELL_H


#include <soc/error.h>
#include <soc/dfe/cmn/dfe_defs.h>
#include <soc/dcmn/dcmn_defs.h>
#include <soc/dcmn/dcmn_fabric_cell.h>


#define SOC_CELL_FILTER_FLAG_DONT_FORWARD   0x1
#define SOC_CELL_FILTER_FLAG_DONT_TRAP      0x2


int soc_dfe_cell_filter_set(int unit, uint32 flags, uint32 array_size, soc_dcmn_filter_type_t* filter_type_arr, uint32* filter_type_val);
int soc_dfe_cell_filter_clear(int unit);
int soc_dfe_cell_filter_count_get(int unit, int *count);
int soc_dfe_cell_filter_receive(int unit, dcmn_captured_cell_t* data_out);

int soc_dfe_control_cell_filter_set(int unit, uint32 flags, soc_dcmn_control_cell_types_t cell_type, uint32 array_size, soc_dcmn_control_cell_filter_type_t* control_cell_filter_type_arr, uint32* filter_type_val);
int soc_dfe_control_cell_filter_clear(int unit);
int soc_dfe_control_cell_filter_receive(int unit, soc_dcmn_captured_control_cell_t* data_out);

soc_error_t  soc_dfe_fabric_cell_load(int unit);
soc_error_t  soc_dfe_fabric_cell_parse(int unit, soc_dcmn_fabric_cell_entry_t entry, soc_dcmn_fabric_cell_entry_t entry_2, soc_dcmn_fabric_cell_info_t *cell_info, int is_two_parts);
soc_error_t  soc_dfe_fabric_cell_entry_get(int unit, soc_dcmn_fabric_cell_entry_t entry);


typedef enum soc_dfe_cell_filter_mode_e {
    soc_dfe_cell_filter_normal_mode  = 0, 
    soc_dfe_cell_filter_filter_mode  = 1, 
    soc_dfe_cell_filter_capture_mode = 2, 
    soc_dfe_cell_filter_copy_mode    = 3 
} soc_dfe_cell_filter_mode_t;

typedef enum soc_dfe_control_cell_filter_mode_e {
    soc_dfe_control_cell_filter_normal_mode  = 0, 
    soc_dfe_control_cell_filter_copy_mode    = 1, 
    soc_dfe_control_cell_filter_capture_mode = 2, 
    soc_dfe_control_cell_filter_filter_mode  = 3 
    
} soc_dfe_control_cell_filter_mode_t;

#endif  
