/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * SOC DNXC CAPTURED BUFFER
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif

#define BSL_LOG_MODULE BSL_LS_SOCDNX_FABRIC
#include <shared/bsl.h>
#include <soc/dnxc/error.h>

#include <soc/defs.h>
#include <soc/dnxc/dnxc_error.h>
#include <soc/dnxc/dnxc_captured_buffer.h>
#include <shared/shrextend/shrextend_debug.h>

shr_error_e
dnxc_captured_buffer_create(
    int unit,
    cyclic_buffer_t * buffer,
    int max_buffered_cells)
{
    int rc;
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(buffer, _SHR_E_PARAM, "buffer");

    rc = cyclic_buffer_create(unit, buffer, sizeof(dnxc_captured_cell_t), max_buffered_cells, "captured_buffer");
    SHR_IF_ERR_EXIT(rc);

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnxc_captured_buffer_destroy(
    int unit,
    cyclic_buffer_t * buffer)
{
    int rc;
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(buffer, _SHR_E_PARAM, "buffer");

    rc = cyclic_buffer_destroy(unit, buffer);
    SHR_IF_ERR_EXIT(rc);

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnxc_captured_buffer_add(
    int unit,
    cyclic_buffer_t * buffer,
    const dnxc_captured_cell_t * new_cell)
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
dnxc_captured_buffer_get(
    int unit,
    cyclic_buffer_t * buffer,
    dnxc_captured_cell_t * received_cell)
{
    int rc;
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(buffer, _SHR_E_PARAM, "buffer");
    SHR_NULL_CHECK(received_cell, _SHR_E_PARAM, "received_cell");

    rc = cyclic_buffer_get(unit, buffer, (void *) received_cell);
    SHR_IF_ERR_EXIT(rc);

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnxc_captured_buffer_is_empty(
    int unit,
    const cyclic_buffer_t * buffer,
    int *is_empty)
{
    int rc;
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(buffer, _SHR_E_PARAM, "buffer");
    SHR_NULL_CHECK(is_empty, _SHR_E_PARAM, "is_empty");

    rc = cyclic_buffer_is_empty(unit, buffer, is_empty);
    SHR_IF_ERR_EXIT(rc);

exit:
    SHR_FUNC_EXIT;
}

#undef BSL_LOG_MODULE
