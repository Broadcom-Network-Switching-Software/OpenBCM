/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * SOC FE1600 CELLS BUFFER
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif

#define BSL_LOG_MODULE BSL_LS_SOCDNX_FABRIC
#include <shared/bsl.h>
#include <soc/dnxc/error.h>

#if defined(BCM_DNXF_SUPPORT) || defined (BCM_PETRA_SUPPORT)

#include <soc/defs.h>
#include <soc/error.h>
#include <shared/cyclic_buffer.h>
#include <soc/dnxc/dnxc_cells_buffer.h>
#include <shared/shrextend/shrextend_debug.h>

shr_error_e
dnxc_cells_buffer_create(
    int unit,
    cyclic_buffer_t * cells_buffer,
    int max_buffered_cells)
{
    int rc;
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(cells_buffer, _SHR_E_PARAM, "cells_buffer");

    rc = cyclic_buffer_create(unit, cells_buffer, sizeof(dnxc_vsc256_sr_cell_t), max_buffered_cells,
                              "cells_buffer->cells");
    SHR_IF_ERR_EXIT(rc);

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnxc_cells_buffer_destroy(
    int unit,
    cyclic_buffer_t * cells_buffer)
{
    int rc;
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(cells_buffer, _SHR_E_PARAM, "cells_buffer");

    rc = cyclic_buffer_destroy(unit, cells_buffer);
    SHR_IF_ERR_EXIT(rc);

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnxc_cells_buffer_add(
    int unit,
    cyclic_buffer_t * buffer,
    const dnxc_vsc256_sr_cell_t * new_cell)
{
    int rc;
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(buffer, _SHR_E_PARAM, "buffer");
    SHR_NULL_CHECK(new_cell, _SHR_E_PARAM, "new_cell");

    rc = cyclic_buffer_add(unit, buffer, (const void *) new_cell);
    SHR_IF_ERR_EXIT(rc);

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnxc_cells_buffer_get(
    int unit,
    cyclic_buffer_t * cells_buffer,
    dnxc_vsc256_sr_cell_t * received_cell)
{
    int rc;
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(cells_buffer, _SHR_E_PARAM, "cells_buffer");
    SHR_NULL_CHECK(received_cell, _SHR_E_PARAM, "received_cell");

    rc = cyclic_buffer_get(unit, cells_buffer, (void *) received_cell);
    SHR_IF_ERR_EXIT(rc);

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnxc_cells_buffer_is_empty(
    int unit,
    const cyclic_buffer_t * cells_buffer,
    int *is_empty)
{
    int rc;
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(cells_buffer, _SHR_E_PARAM, "cells_buffer");
    SHR_NULL_CHECK(is_empty, _SHR_E_PARAM, "is_empty");

    rc = cyclic_buffer_is_empty(unit, cells_buffer, is_empty);
    SHR_IF_ERR_EXIT(rc);

exit:
    SHR_FUNC_EXIT;
}

#endif 

#undef BSL_LOG_MODULE
