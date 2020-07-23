/*
 *  
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * DCMN CELLS BUFFER H
 */
 
#ifndef _SOC_DCMN_CELLS_BUFFER_H_
#define _SOC_DCMN_CELLS_BUFFER_H_

#ifdef BCM_DFE_SUPPORT
#include <soc/dfe/cmn/dfe_drv.h>
#endif 
#include <shared/cyclic_buffer.h>
#include <soc/dcmn/vsc256_fabric_cell.h>

soc_error_t dcmn_cells_buffer_create(int unit, cyclic_buffer_t* cells_buffer, int max_buffered_cells);
soc_error_t dcmn_cells_buffer_destroy(int unit, cyclic_buffer_t* cells_buffer);
soc_error_t dcmn_cells_buffer_add(int unit, cyclic_buffer_t* cells_buffer, const vsc256_sr_cell_t* new_cell);
soc_error_t dcmn_cells_buffer_get(int unit, cyclic_buffer_t* cells_buffer, vsc256_sr_cell_t* received_cell);
soc_error_t dcmn_cells_buffer_is_empty(int unit, const cyclic_buffer_t* cells_buffer, int* is_empty);
soc_error_t dcmn_cells_buffer_is_full(int unit, const cyclic_buffer_t* cells_buffer, int* is_full);
soc_error_t dcmn_cells_buffer_cells_count(int unit, const cyclic_buffer_t* cells_buffer, int* is_empty);

#endif 

