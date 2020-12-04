/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * SOC FE1600 CELLS BUFFER
 */
 
#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_SOC_FABRIC
#include <shared/bsl.h>
#include <soc/dcmn/error.h>

#if defined(BCM_DFE_SUPPORT) || defined (BCM_PETRA_SUPPORT)

#include <soc/defs.h>
#include <soc/error.h>
#include <shared/cyclic_buffer.h>
#include <soc/dcmn/dcmn_cells_buffer.h>


soc_error_t 
dcmn_cells_buffer_create(int unit, cyclic_buffer_t* cells_buffer, int max_buffered_cells)
{
    int rc;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_NULL_CHECK(cells_buffer);

    rc = cyclic_buffer_create(unit, cells_buffer, sizeof(vsc256_sr_cell_t), max_buffered_cells, "cells_buffer->cells"); 
    SOCDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN; 
}

soc_error_t 
dcmn_cells_buffer_destroy(int unit, cyclic_buffer_t* cells_buffer)
{
    int rc;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_NULL_CHECK(cells_buffer);

    rc = cyclic_buffer_destroy(unit, cells_buffer);
    SOCDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN; 
}

soc_error_t 
dcmn_cells_buffer_add(int unit, cyclic_buffer_t* buffer, const vsc256_sr_cell_t* new_cell)
{
    int rc;
    SOCDNX_INIT_FUNC_DEFS;
    
    SOCDNX_NULL_CHECK(buffer);
    SOCDNX_NULL_CHECK(new_cell);

    rc = cyclic_buffer_add(unit, buffer, (const void*)new_cell);
    SOCDNX_IF_ERR_EXIT(rc);
    
exit:
    SOCDNX_FUNC_RETURN; 
}

soc_error_t 
dcmn_cells_buffer_get(int unit, cyclic_buffer_t* cells_buffer, vsc256_sr_cell_t* received_cell)
{
    int rc;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_NULL_CHECK(cells_buffer);
    SOCDNX_NULL_CHECK(received_cell);

    rc = cyclic_buffer_get(unit, cells_buffer, (void*)received_cell);
    SOCDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN; 
}

soc_error_t 
dcmn_cells_buffer_is_empty(int unit, const cyclic_buffer_t* cells_buffer, int* is_empty)
{
    int rc;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_NULL_CHECK(cells_buffer);
    SOCDNX_NULL_CHECK(is_empty);

    rc = cyclic_buffer_is_empty(unit, cells_buffer, is_empty);
    SOCDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN; 
}

soc_error_t 
dcmn_cells_buffer_is_full(int unit, const cyclic_buffer_t* cells_buffer, int* is_full)
{
    int rc;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_NULL_CHECK(cells_buffer);
    SOCDNX_NULL_CHECK(is_full);

    rc = cyclic_buffer_is_full(unit, cells_buffer, is_full);
    SOCDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN; 
}

soc_error_t 
dcmn_cells_buffer_cells_count(int unit, const cyclic_buffer_t* cells_buffer, int* count)
{
    int rc;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_NULL_CHECK(cells_buffer);
    SOCDNX_NULL_CHECK(count);

    rc = cyclic_buffer_cells_count(unit, cells_buffer, count);
    SOCDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN; 
}


#endif 

#undef _ERR_MSG_MODULE_NAME

