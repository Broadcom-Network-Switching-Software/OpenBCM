
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */
#ifndef _PEMLADRV_VIRTUAL_WIRES_H_
#define _PEMLADRV_VIRTUAL_WIRES_H_

#include <soc/sand/shrextend/shrextend_debug.h>
#include <shared/utilex/utilex_str.h>
#include <soc/dnx/pp_stage.h>
#include <soc/dnx/pemladrv/auto_generated/pemladrv.h>

#define PEMLADRV_ITERATOR_BY_STAGE_INIT (-1)
#define PEMLADRV_ITERATOR_BY_STAGE_END  (-2)

shr_error_e pemladrv_vw_wire_get(
    int unit,
    char vw_name[MAX_VW_NAME_LENGTH],
    dnx_pp_stage_e pp_stage,
    VirtualWireInfo ** vw_info);

shr_error_e pemladrv_vw_wire_nof_by_stage(
    int unit,
    dnx_pp_stage_e sdk_stage,
    int *nof_vw);

shr_error_e pemladrv_vw_wire_iterator_by_stage_get_next(
    int unit,
    int nof_stages,
    dnx_pp_stage_e * sdk_stages,
    int *vw_iterator,
    VirtualWireInfo ** vw_info,
    uint8 *stages_not_started);

shr_error_e pemladrv_vw_stages_pemla_to_sdk(
    int unit,
    int pemla_stage,
    int *pp_stage);

shr_error_e pemladrv_vw_stages_sdk_to_pemla(
    int unit,
    int pemla_stage,
    int *pp_stage);

#endif
