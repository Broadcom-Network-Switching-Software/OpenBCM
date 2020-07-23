
/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */


#include <shared/bitop.h>
#include <shared/util.h>
#include <soc/dnxc/swstate/sw_state_defs.h>
#include <soc/dnxc/swstate/types/sw_state_pbmp.h>
#include <soc/dnxc/swstate/dnxc_sw_state.h>
#include <shared/pbmp.h>


#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif

#define BSL_LOG_MODULE BSL_LS_SWSTATEDNX_GENERAL


static int dnxc_sw_state_pbmp_read(
    int unit,
    uint32 module_id,
    dnxc_sw_state_pbmp *pbmp,
    dnxc_sw_state_pbmp *dest,
    uint32 flags,
    char *dbg_string)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    DNX_SW_STATE_MEMREAD(unit, dest, pbmp, 0, sizeof(dnxc_sw_state_pbmp), flags, dbg_string);

    DNXC_SW_STATE_FUNC_RETURN;
}
 

static int dnxc_sw_state_pbmp_write(
    int unit,
    uint32 module_id,
    dnxc_sw_state_pbmp *pbmp,
    dnxc_sw_state_pbmp *src,
    uint32 flags,
    char *dbg_string)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    DNX_SW_STATE_MEMWRITE(unit, module_id, src, pbmp, 0, sizeof(dnxc_sw_state_pbmp), flags, dbg_string);

    DNXC_SW_STATE_FUNC_RETURN;
}

  

int dnxc_sw_state_pbmp_eq(
    int unit,
    uint32 module_id,
    dnxc_sw_state_pbmp *pbmp,
    dnxc_sw_state_pbmp input_bitmap,
    uint8 *result,
    uint32 flags,
    char *dbg_string)
{
    dnxc_sw_state_pbmp tmp_pbmp;

    DNXC_SW_STATE_INIT_FUNC_DEFS;

    SHR_IF_ERR_EXIT(dnxc_sw_state_pbmp_read(unit, module_id, pbmp, &tmp_pbmp, flags, dbg_string));

    *result = (_SHR_PBMP_EQ(tmp_pbmp, input_bitmap) > 0) ? 1: 0;

    DNXC_SW_STATE_FUNC_RETURN;
}


int dnxc_sw_state_pbmp_member(
    int unit,
    uint32 module_id,
    dnxc_sw_state_pbmp *pbmp,
    uint32 input_port,
    uint8 *result,
    uint32 flags,
    char *dbg_string)
{
    dnxc_sw_state_pbmp tmp_pbmp;

    DNXC_SW_STATE_INIT_FUNC_DEFS;

    SHR_IF_ERR_EXIT(dnxc_sw_state_pbmp_read(unit, module_id, pbmp, &tmp_pbmp, flags, dbg_string));

    *result = (_SHR_PBMP_MEMBER(tmp_pbmp, input_port) > 0) ? 1: 0;

    DNXC_SW_STATE_FUNC_RETURN;
}


int dnxc_sw_state_pbmp_is_null(
    int unit,
    uint32 module_id,
    dnxc_sw_state_pbmp *pbmp,
    uint8 *result,
    uint32 flags,
    char *dbg_string)
{
    dnxc_sw_state_pbmp tmp_pbmp;

    DNXC_SW_STATE_INIT_FUNC_DEFS;

    SHR_IF_ERR_EXIT(dnxc_sw_state_pbmp_read(unit, module_id, pbmp, &tmp_pbmp, flags, dbg_string));

    *result = (_SHR_PBMP_IS_NULL(tmp_pbmp) > 0) ? 1: 0;

    DNXC_SW_STATE_FUNC_RETURN;
}


int dnxc_sw_state_pbmp_count(
    int unit,
    uint32 module_id,
    dnxc_sw_state_pbmp *pbmp,
    int *result,
    uint32 flags,
    char *dbg_string)
{
    dnxc_sw_state_pbmp tmp_pbmp;

    DNXC_SW_STATE_INIT_FUNC_DEFS;

    SHR_IF_ERR_EXIT(dnxc_sw_state_pbmp_read(unit, module_id, pbmp, &tmp_pbmp, flags, dbg_string));

    _SHR_PBMP_COUNT(tmp_pbmp, *result);

    DNXC_SW_STATE_FUNC_RETURN;
}


int dnxc_sw_state_pbmp_get(
    int unit,
    uint32 module_id,
    dnxc_sw_state_pbmp *pbmp,
    dnxc_sw_state_pbmp *dest,
    uint32 flags,
    char *dbg_string)
{
    dnxc_sw_state_pbmp tmp_pbmp;

    DNXC_SW_STATE_INIT_FUNC_DEFS;

    SHR_IF_ERR_EXIT(dnxc_sw_state_pbmp_read(unit, module_id, pbmp, &tmp_pbmp, flags, dbg_string));

    _SHR_PBMP_ASSIGN(*dest, tmp_pbmp);

    DNXC_SW_STATE_FUNC_RETURN;
}


int dnxc_sw_state_pbmp_assign(
    int unit,
    uint32 module_id,
    dnxc_sw_state_pbmp *pbmp,
    dnxc_sw_state_pbmp src,
    uint32 flags,
    char *dbg_string)
{
    dnxc_sw_state_pbmp tmp_pbmp;

    DNXC_SW_STATE_INIT_FUNC_DEFS;

    _SHR_PBMP_ASSIGN(tmp_pbmp, src);

    SHR_IF_ERR_EXIT(dnxc_sw_state_pbmp_write(unit, module_id, pbmp, &tmp_pbmp, flags, dbg_string));

    DNXC_SW_STATE_FUNC_RETURN;
}


int dnxc_sw_state_pbmp_and(
    int unit,
    uint32 module_id,
    dnxc_sw_state_pbmp *pbmp,
    dnxc_sw_state_pbmp input_pbmp,
    uint32 flags,
    char *dbg_string)
{
    dnxc_sw_state_pbmp tmp_pbmp;

    DNXC_SW_STATE_INIT_FUNC_DEFS;

    SHR_IF_ERR_EXIT(dnxc_sw_state_pbmp_read(unit, module_id, pbmp, &tmp_pbmp, flags, dbg_string));

    _SHR_PBMP_AND(tmp_pbmp, input_pbmp);

    SHR_IF_ERR_EXIT(dnxc_sw_state_pbmp_write(unit, module_id, pbmp, &tmp_pbmp, flags, dbg_string));

    DNXC_SW_STATE_FUNC_RETURN;
}


int dnxc_sw_state_pbmp_or(
    int unit,
    uint32 module_id,
    dnxc_sw_state_pbmp *pbmp,
    dnxc_sw_state_pbmp input_pbmp,
    uint32 flags,
    char *dbg_string)
{
    dnxc_sw_state_pbmp tmp_pbmp;

    DNXC_SW_STATE_INIT_FUNC_DEFS;

    SHR_IF_ERR_EXIT(dnxc_sw_state_pbmp_read(unit, module_id, pbmp, &tmp_pbmp, flags, dbg_string));

    _SHR_PBMP_OR(tmp_pbmp, input_pbmp);

    SHR_IF_ERR_EXIT(dnxc_sw_state_pbmp_write(unit, module_id, pbmp, &tmp_pbmp, flags, dbg_string));

    DNXC_SW_STATE_FUNC_RETURN;
}


int dnxc_sw_state_pbmp_xor(
    int unit,
    uint32 module_id,
    dnxc_sw_state_pbmp *pbmp,
    dnxc_sw_state_pbmp input_pbmp,
    uint32 flags,
    char *dbg_string)
{
    dnxc_sw_state_pbmp tmp_pbmp;

    DNXC_SW_STATE_INIT_FUNC_DEFS;

    SHR_IF_ERR_EXIT(dnxc_sw_state_pbmp_read(unit, module_id, pbmp, &tmp_pbmp, flags, dbg_string));

    _SHR_PBMP_XOR(tmp_pbmp, input_pbmp);

    SHR_IF_ERR_EXIT(dnxc_sw_state_pbmp_write(unit, module_id, pbmp, &tmp_pbmp, flags, dbg_string));

    DNXC_SW_STATE_FUNC_RETURN;
}


int dnxc_sw_state_pbmp_remove(
    int unit,
    uint32 module_id,
    dnxc_sw_state_pbmp *pbmp,
    dnxc_sw_state_pbmp input_pbmp,
    uint32 flags,
    char *dbg_string)
{
    dnxc_sw_state_pbmp tmp_pbmp;

    DNXC_SW_STATE_INIT_FUNC_DEFS;

    SHR_IF_ERR_EXIT(dnxc_sw_state_pbmp_read(unit, module_id, pbmp, &tmp_pbmp, flags, dbg_string));

    _SHR_PBMP_REMOVE(tmp_pbmp, input_pbmp);

    SHR_IF_ERR_EXIT(dnxc_sw_state_pbmp_write(unit, module_id, pbmp, &tmp_pbmp, flags, dbg_string));

    DNXC_SW_STATE_FUNC_RETURN;
}


int dnxc_sw_state_pbmp_negate(
    int unit,
    uint32 module_id,
    dnxc_sw_state_pbmp *pbmp,
    dnxc_sw_state_pbmp input_bitmap,
    uint32 flags,
    char *dbg_string)
{
    dnxc_sw_state_pbmp tmp_pbmp;

    DNXC_SW_STATE_INIT_FUNC_DEFS;

    SHR_IF_ERR_EXIT(dnxc_sw_state_pbmp_read(unit, module_id, pbmp, &tmp_pbmp, flags, dbg_string));

    _SHR_PBMP_NEGATE(tmp_pbmp, input_bitmap);

    SHR_IF_ERR_EXIT(dnxc_sw_state_pbmp_write(unit, module_id, pbmp, &tmp_pbmp, flags, dbg_string));

    DNXC_SW_STATE_FUNC_RETURN;
}


int dnxc_sw_state_pbmp_clear(
    int unit,
    uint32 module_id,
    dnxc_sw_state_pbmp *pbmp,
    uint32 flags,
    char *dbg_string)
{
    dnxc_sw_state_pbmp tmp_pbmp;

    DNXC_SW_STATE_INIT_FUNC_DEFS;

    _SHR_PBMP_CLEAR(tmp_pbmp);

    SHR_IF_ERR_EXIT(dnxc_sw_state_pbmp_write(unit, module_id, pbmp, &tmp_pbmp, flags, dbg_string));

    DNXC_SW_STATE_FUNC_RETURN;
}


int dnxc_sw_state_pbmp_port_set(
    int unit,
    uint32 module_id,
    dnxc_sw_state_pbmp *pbmp,
    uint32 input_port,
    uint32 flags,
    char *dbg_string)
{
    dnxc_sw_state_pbmp tmp_pbmp;

    DNXC_SW_STATE_INIT_FUNC_DEFS;

    SHR_IF_ERR_EXIT(dnxc_sw_state_pbmp_read(unit, module_id, pbmp, &tmp_pbmp, flags, dbg_string));

    _SHR_PBMP_PORT_SET(tmp_pbmp, input_port);

    SHR_IF_ERR_EXIT(dnxc_sw_state_pbmp_write(unit, module_id, pbmp, &tmp_pbmp, flags, dbg_string));

    DNXC_SW_STATE_FUNC_RETURN;
}


int dnxc_sw_state_pbmp_port_add(
    int unit,
    uint32 module_id,
    dnxc_sw_state_pbmp *pbmp,
    uint32 input_port,
    uint32 flags,
    char *dbg_string)
{
    dnxc_sw_state_pbmp tmp_pbmp;

    DNXC_SW_STATE_INIT_FUNC_DEFS;

    SHR_IF_ERR_EXIT(dnxc_sw_state_pbmp_read(unit, module_id, pbmp, &tmp_pbmp, flags, dbg_string));

    _SHR_PBMP_PORT_ADD(tmp_pbmp, input_port);

    SHR_IF_ERR_EXIT(dnxc_sw_state_pbmp_write(unit, module_id, pbmp, &tmp_pbmp, flags, dbg_string));

    DNXC_SW_STATE_FUNC_RETURN;
}


int dnxc_sw_state_pbmp_port_remove(
    int unit,
    uint32 module_id,
    dnxc_sw_state_pbmp *pbmp,
    uint32 input_port,
    uint32 flags,
    char *dbg_string)
{
    dnxc_sw_state_pbmp tmp_pbmp;

    DNXC_SW_STATE_INIT_FUNC_DEFS;

    SHR_IF_ERR_EXIT(dnxc_sw_state_pbmp_read(unit, module_id, pbmp, &tmp_pbmp, flags, dbg_string));

    _SHR_PBMP_PORT_REMOVE(tmp_pbmp, input_port);

    SHR_IF_ERR_EXIT(dnxc_sw_state_pbmp_write(unit, module_id, pbmp, &tmp_pbmp, flags, dbg_string));

    DNXC_SW_STATE_FUNC_RETURN;
}


int dnxc_sw_state_pbmp_port_flip(
    int unit,
    uint32 module_id,
    dnxc_sw_state_pbmp *pbmp,
    uint32 input_port,
    uint32 flags,
    char *dbg_string)
{
    dnxc_sw_state_pbmp tmp_pbmp;

    DNXC_SW_STATE_INIT_FUNC_DEFS;

    SHR_IF_ERR_EXIT(dnxc_sw_state_pbmp_read(unit, module_id, pbmp, &tmp_pbmp, flags, dbg_string));

    _SHR_PBMP_PORT_FLIP(tmp_pbmp, input_port);

    SHR_IF_ERR_EXIT(dnxc_sw_state_pbmp_write(unit, module_id, pbmp, &tmp_pbmp, flags, dbg_string));

    DNXC_SW_STATE_FUNC_RETURN;
}


int dnxc_sw_state_pbmp_port_range_add(
    int unit,
    uint32 module_id,
    dnxc_sw_state_pbmp *pbmp,
    uint32 input_port,
    uint32 range,
    uint32 flags,
    char *dbg_string)
{
    dnxc_sw_state_pbmp tmp_pbmp;

    DNXC_SW_STATE_INIT_FUNC_DEFS;

    SHR_IF_ERR_EXIT(dnxc_sw_state_pbmp_read(unit, module_id, pbmp, &tmp_pbmp, flags, dbg_string));

    _SHR_PBMP_PORTS_RANGE_ADD(tmp_pbmp, input_port, range);

    SHR_IF_ERR_EXIT(dnxc_sw_state_pbmp_write(unit, module_id, pbmp, &tmp_pbmp, flags, dbg_string));

    DNXC_SW_STATE_FUNC_RETURN;
}


int dnxc_sw_state_pbmp_fmt(
    int unit,
    uint32 module_id,
    dnxc_sw_state_pbmp *pbmp,
    char *buffer,
    uint32 flags,
    char *dbg_string)
{
    dnxc_sw_state_pbmp tmp_pbmp;

    DNXC_SW_STATE_INIT_FUNC_DEFS;

    SHR_IF_ERR_EXIT(dnxc_sw_state_pbmp_read(unit, module_id, pbmp, &tmp_pbmp, flags, dbg_string));

    _SHR_PBMP_FMT(tmp_pbmp, buffer);

    DNXC_SW_STATE_FUNC_RETURN;
}

#undef _ERR_MSG_MODULE_NAME
