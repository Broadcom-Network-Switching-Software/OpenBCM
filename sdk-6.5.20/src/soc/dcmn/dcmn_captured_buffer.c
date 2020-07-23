/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * SOC DCMN CAPTURED BUFFER
 */
 
#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_SOC_FABRIC
#include <shared/bsl.h>
#include <soc/dcmn/error.h>

#include <soc/defs.h>
#include <soc/dcmn/dcmn_error.h>
#include <soc/dcmn/dcmn_captured_buffer.h>


soc_error_t 
dcmn_captured_buffer_create(int unit, cyclic_buffer_t* buffer, int max_buffered_cells)
{
    int rc;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_NULL_CHECK(buffer);

    rc = cyclic_buffer_create(unit, buffer, sizeof(dcmn_captured_cell_t), max_buffered_cells, "captured_buffer"); 
    SOCDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN; 
}

soc_error_t 
dcmn_captured_buffer_destroy(int unit, cyclic_buffer_t* buffer)
{
    int rc;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_NULL_CHECK(buffer);

    rc = cyclic_buffer_destroy(unit, buffer);
    SOCDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN; 
}

soc_error_t 
dcmn_captured_buffer_add(int unit, cyclic_buffer_t* buffer, const dcmn_captured_cell_t* new_cell)
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
dcmn_captured_buffer_get(int unit, cyclic_buffer_t* buffer, dcmn_captured_cell_t* received_cell)
{
    int rc;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_NULL_CHECK(buffer);
    SOCDNX_NULL_CHECK(received_cell);

    rc = cyclic_buffer_get(unit, buffer, (void*)received_cell);
    SOCDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN; 
}

soc_error_t 
dcmn_captured_buffer_is_empty(int unit, const cyclic_buffer_t* buffer, int* is_empty)
{
    int rc;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_NULL_CHECK(buffer);
    SOCDNX_NULL_CHECK(is_empty);

    rc = cyclic_buffer_is_empty(unit, buffer, is_empty);
    SOCDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN; 
}

soc_error_t 
dcmn_captured_buffer_is_full(int unit, const cyclic_buffer_t* buffer, int* is_full)
{
    int rc;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_NULL_CHECK(buffer);
    SOCDNX_NULL_CHECK(is_full);

    rc = cyclic_buffer_is_full(unit, buffer, is_full);
    SOCDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN; 
}

soc_error_t 
dcmn_captured_buffer_cells_count(int unit, const cyclic_buffer_t* buffer, int* count)
{
    int rc;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_NULL_CHECK(buffer);
    SOCDNX_NULL_CHECK(count);

    rc = cyclic_buffer_cells_count(unit, buffer, count);
    SOCDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN; 
}


#undef _ERR_MSG_MODULE_NAME

