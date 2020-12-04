/*
 *  
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * DNXC CAPTURED BUFFER H
 */

#ifndef _SOC_DNXC_CAPTURED_BUFFER_H_
#define _SOC_DNXC_CAPTURED_BUFFER_H_

#include <soc/dnxc/dnxc_fabric_cell.h>
#include <shared/cyclic_buffer.h>
#include <shared/shrextend/shrextend_error.h>

shr_error_e dnxc_captured_buffer_create(
    int unit,
    cyclic_buffer_t * captured_cells_buffer,
    int max_buffered_cells);
shr_error_e dnxc_captured_buffer_destroy(
    int unit,
    cyclic_buffer_t * captured_cells_buffer);
shr_error_e dnxc_captured_buffer_add(
    int unit,
    cyclic_buffer_t * captured_cells_buffer,
    const dnxc_captured_cell_t * new_cell);
shr_error_e dnxc_captured_buffer_get(
    int unit,
    cyclic_buffer_t * captured_cells_buffer,
    dnxc_captured_cell_t * received_cell);
shr_error_e dnxc_captured_buffer_is_empty(
    int unit,
    const cyclic_buffer_t * captured_cells_buffer,
    int *is_empty);

#endif
