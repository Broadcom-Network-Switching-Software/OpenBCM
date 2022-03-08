
/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 *
 */

#ifndef _DNXC_SW_STATE_PBMP_H

#define _DNXC_SW_STATE_PBMP_H

#include <soc/types.h>
#include <soc/error.h>
#include <shared/bsl.h>
#include <soc/sand/shrextend/shrextend_debug.h>
#include <shared/pbmp.h>

#define DNX_SW_STATE_PBMP_EQ(unit, node_id, path, input_pbmp, result, flags, debug_string)\
do {\
    SHR_IF_ERR_EXIT(dnxc_sw_state_pbmp_eq(unit, node_id, &path, input_pbmp, result, flags, debug_string));\
} while(0)

#define DNX_SW_STATE_PBMP_NEQ(unit, node_id, path, input_pbmp, result, flags, debug_string)\
do {\
    SHR_IF_ERR_EXIT(dnxc_sw_state_pbmp_eq(unit, node_id, &path, input_pbmp, result, flags, debug_string));\
    *result = !(*result);\
} while(0)

#define DNX_SW_STATE_PBMP_MEMBER(unit, node_id, path, input_port, result, flags, debug_string)\
do {\
    SHR_IF_ERR_EXIT(dnxc_sw_state_pbmp_member(unit, node_id, &path, input_port, result, flags, debug_string));\
} while(0)

#define DNX_SW_STATE_PBMP_IS_NULL(unit, node_id, path, result, flags, debug_string)\
do {\
    SHR_IF_ERR_EXIT(dnxc_sw_state_pbmp_is_null(unit, node_id, &path, result, flags, debug_string));\
} while(0)

#define DNX_SW_STATE_PBMP_NOT_NULL(unit, node_id, path, result, flags, debug_string)\
do {\
    SHR_IF_ERR_EXIT(dnxc_sw_state_pbmp_is_null(unit, node_id, &path, result, flags, debug_string));\
    *result = !(*result);\
} while(0)

#define DNX_SW_STATE_PBMP_COUNT(unit, node_id, path, result, flags, debug_string)\
do {\
    SHR_IF_ERR_EXIT(dnxc_sw_state_pbmp_count(unit, node_id, &path, result, flags, debug_string));\
} while(0)

#define DNX_SW_STATE_PBMP_GET(unit, node_id, path, output_pbmp, flags, debug_string)\
do {\
    SHR_IF_ERR_EXIT(dnxc_sw_state_pbmp_get(unit, node_id, &path, output_pbmp, flags, debug_string));\
} while(0)

#define DNX_SW_STATE_PBMP_ASSIGN(unit, node_id, path, input_pbmp, flags, debug_string)\
do {\
    SHR_IF_ERR_EXIT(dnxc_sw_state_pbmp_assign(unit, node_id, &path, input_pbmp, flags, debug_string));\
} while(0)

#define DNX_SW_STATE_PBMP_AND(unit, node_id, path, input_pbmp, flags, debug_string)\
do {\
    SHR_IF_ERR_EXIT(dnxc_sw_state_pbmp_and(unit, node_id, &path, input_pbmp, flags, debug_string));\
} while(0)

#define DNX_SW_STATE_PBMP_OR(unit, node_id, path, input_pbmp, flags, debug_string)\
do {\
    SHR_IF_ERR_EXIT(dnxc_sw_state_pbmp_or(unit, node_id, &path, input_pbmp, flags, debug_string));\
} while(0)

#define DNX_SW_STATE_PBMP_XOR(unit, node_id, path, input_pbmp, flags, debug_string)\
do {\
    SHR_IF_ERR_EXIT(dnxc_sw_state_pbmp_xor(unit, node_id, &path, input_pbmp, flags, debug_string));\
} while(0)

#define DNX_SW_STATE_PBMP_REMOVE(unit, node_id, path, input_pbmp, flags, debug_string)\
do {\
    SHR_IF_ERR_EXIT(dnxc_sw_state_pbmp_remove(unit, node_id, &path, input_pbmp, flags, debug_string));\
} while(0)

#define DNX_SW_STATE_PBMP_NEGATE(unit, node_id, path, input_pbmp, flags, debug_string)\
do {\
    SHR_IF_ERR_EXIT(dnxc_sw_state_pbmp_negate(unit, node_id, &path, input_pbmp, flags, debug_string));\
} while(0)

#define DNX_SW_STATE_PBMP_CLEAR(unit, node_id, path, flags, debug_string)\
do {\
    SHR_IF_ERR_EXIT(dnxc_sw_state_pbmp_clear(unit, node_id, &path, flags, debug_string));\
} while(0)

#define DNX_SW_STATE_PBMP_PORT_SET(unit, node_id, path, input_port, flags, debug_string)\
do {\
    SHR_IF_ERR_EXIT(dnxc_sw_state_pbmp_port_set(unit, node_id, &path, input_port, flags, debug_string));\
} while(0)

#define DNX_SW_STATE_PBMP_PORT_ADD(unit, node_id, path, input_port, flags, debug_string)\
do {\
    SHR_IF_ERR_EXIT(dnxc_sw_state_pbmp_port_add(unit, node_id, &path, input_port, flags, debug_string));\
} while(0)

#define DNX_SW_STATE_PBMP_PORT_REMOVE(unit, node_id, path, input_port, flags, debug_string)\
do {\
    SHR_IF_ERR_EXIT(dnxc_sw_state_pbmp_port_remove(unit, node_id, &path, input_port, flags, debug_string));\
} while(0)

#define DNX_SW_STATE_PBMP_PORT_FLIP(unit, node_id, path, input_port, flags, debug_string)\
do {\
    SHR_IF_ERR_EXIT(dnxc_sw_state_pbmp_port_flip(unit, node_id, &path, input_port, flags, debug_string));\
} while(0)

#define DNX_SW_STATE_PBMP_PORTS_RANGE_ADD(unit, node_id, path, input_port, range, flags, debug_string)\
do {\
    SHR_IF_ERR_EXIT(dnxc_sw_state_pbmp_port_range_add(unit, node_id, &path, input_port, range, flags, debug_string));\
} while(0)

#define DNX_SW_STATE_PBMP_FMT(unit, node_id, path, buffer, flags, debug_string)\
do {\
    SHR_IF_ERR_EXIT(dnxc_sw_state_pbmp_fmt(unit, node_id, &path, buffer, flags, debug_string));\
} while(0)

typedef _shr_pbmp_t dnxc_sw_state_pbmp;

int dnxc_sw_state_pbmp_eq(
    int unit,
    uint32 node_id,
    dnxc_sw_state_pbmp * pbmp,
    dnxc_sw_state_pbmp input_bitmap,
    uint8 *result,
    uint32 flags,
    char *dbg_string);

int dnxc_sw_state_pbmp_member(
    int unit,
    uint32 node_id,
    dnxc_sw_state_pbmp * pbmp,
    uint32 input_port,
    uint8 *result,
    uint32 flags,
    char *dbg_string);

int dnxc_sw_state_pbmp_is_null(
    int unit,
    uint32 node_id,
    dnxc_sw_state_pbmp * pbmp,
    uint8 *result,
    uint32 flags,
    char *dbg_string);

int dnxc_sw_state_pbmp_count(
    int unit,
    uint32 node_id,
    dnxc_sw_state_pbmp * pbmp,
    int *result,
    uint32 flags,
    char *dbg_string);

int dnxc_sw_state_pbmp_get(
    int unit,
    uint32 node_id,
    dnxc_sw_state_pbmp * pbmp,
    dnxc_sw_state_pbmp * result,
    uint32 flags,
    char *dbg_string);

int dnxc_sw_state_pbmp_assign(
    int unit,
    uint32 node_id,
    dnxc_sw_state_pbmp * pbmp,
    dnxc_sw_state_pbmp src,
    uint32 flags,
    char *dbg_string);

int dnxc_sw_state_pbmp_and(
    int unit,
    uint32 node_id,
    dnxc_sw_state_pbmp * pbmp,
    dnxc_sw_state_pbmp input_pbmp,
    uint32 flags,
    char *dbg_string);

int dnxc_sw_state_pbmp_or(
    int unit,
    uint32 node_id,
    dnxc_sw_state_pbmp * pbmp,
    dnxc_sw_state_pbmp input_pbmp,
    uint32 flags,
    char *dbg_string);

int dnxc_sw_state_pbmp_xor(
    int unit,
    uint32 node_id,
    dnxc_sw_state_pbmp * pbmp,
    dnxc_sw_state_pbmp input_pbmp,
    uint32 flags,
    char *dbg_string);

int dnxc_sw_state_pbmp_remove(
    int unit,
    uint32 node_id,
    dnxc_sw_state_pbmp * pbmp,
    dnxc_sw_state_pbmp input_pbmp,
    uint32 flags,
    char *dbg_string);

int dnxc_sw_state_pbmp_negate(
    int unit,
    uint32 node_id,
    dnxc_sw_state_pbmp * pbmp,
    dnxc_sw_state_pbmp input_bitmap,
    uint32 flags,
    char *dbg_string);

int dnxc_sw_state_pbmp_clear(
    int unit,
    uint32 node_id,
    dnxc_sw_state_pbmp * pbmp,
    uint32 flags,
    char *dbg_string);

int dnxc_sw_state_pbmp_port_set(
    int unit,
    uint32 node_id,
    dnxc_sw_state_pbmp * pbmp,
    uint32 input_port,
    uint32 flags,
    char *dbg_string);

int dnxc_sw_state_pbmp_port_add(
    int unit,
    uint32 node_id,
    dnxc_sw_state_pbmp * pbmp,
    uint32 input_port,
    uint32 flags,
    char *dbg_string);

int dnxc_sw_state_pbmp_port_remove(
    int unit,
    uint32 node_id,
    dnxc_sw_state_pbmp * pbmp,
    uint32 input_port,
    uint32 flags,
    char *dbg_string);

int dnxc_sw_state_pbmp_port_flip(
    int unit,
    uint32 node_id,
    dnxc_sw_state_pbmp * pbmp,
    uint32 input_port,
    uint32 flags,
    char *dbg_string);

int dnxc_sw_state_pbmp_port_range_add(
    int unit,
    uint32 node_id,
    dnxc_sw_state_pbmp * pbmp,
    uint32 input_port,
    uint32 range,
    uint32 flags,
    char *dbg_string);

int dnxc_sw_state_pbmp_fmt(
    int unit,
    uint32 node_id,
    dnxc_sw_state_pbmp * pbmp,
    char *buffer,
    uint32 flags,
    char *dbg_string);

#endif
