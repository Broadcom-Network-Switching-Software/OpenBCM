/*
 *  
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * DCMN CAPTURED BUFFER H
 */
 
#ifndef _SOC_DCMN_CAPTURED_BUFFER_H_
#define _SOC_DCMN_CAPTURED_BUFFER_H_

#include <soc/dcmn/dcmn_fabric_cell.h>
#include <shared/cyclic_buffer.h>

soc_error_t dcmn_captured_buffer_create(int unit, cyclic_buffer_t* captured_cells_buffer, int max_buffered_cells);
soc_error_t dcmn_captured_buffer_destroy(int unit, cyclic_buffer_t* captured_cells_buffer);
soc_error_t dcmn_captured_buffer_add(int unit, cyclic_buffer_t* captured_cells_buffer, const dcmn_captured_cell_t* new_cell);
soc_error_t dcmn_captured_buffer_get(int unit, cyclic_buffer_t* captured_cells_buffer, dcmn_captured_cell_t* received_cell);
soc_error_t dcmn_captured_buffer_is_empty(int unit, const cyclic_buffer_t* captured_cells_buffer, int* is_empty);
soc_error_t dcmn_captured_buffer_is_full(int unit, const cyclic_buffer_t* captured_cells_buffer, int* is_full);
soc_error_t dcmn_captured_buffer_cells_count(int unit, const cyclic_buffer_t* captured_cells_buffer, int* is_empty);

#endif 
