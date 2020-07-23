
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifndef _PEMLADRV_VIRTUAL_WIRES_H_
#define _PEMLADRV_VIRTUAL_WIRES_H_

#include <shared/shrextend/shrextend_debug.h>
#include <shared/utilex/utilex_str.h>
#include <soc/dnx/pp_stage.h>
#include <soc/dnx/pemladrv/auto_generated/pemladrv.h>

#define PEMLADRV_ITERATOR_BY_STAGE_INIT (-1)
#define PEMLADRV_ITERATOR_BY_STAGE_END  (-2)

typedef enum
{
    PEMLA_PIPE_STAGE_INGRESS_HARD_LLR_SC = 0,
    PEMLA_PIPE_STAGE_INGRESS_KLEAP_VTA,
    PEMLA_PIPE_STAGE_INGRESS_KLEAP_VTB,
    PEMLA_PIPE_STAGE_INGRESS_KLEAP_VTC,
    PEMLA_PIPE_STAGE_INGRESS_KLEAP_VTD,
    PEMLA_PIPE_STAGE_INGRESS_KLEAP_VTE,
    PEMLA_PIPE_STAGE_INGRESS_KLEAP_FLPA,
    PEMLA_PIPE_STAGE_INGRESS_KLEAP_FLPB,
    PEMLA_PIPE_STAGE_INGRESS_HARD_PMFA_SC,
    PEMLA_PIPE_STAGE_INGRESS_SOFT_PMFA_SOFT,
    PEMLA_PIPE_STAGE_INGRESS_HARD_FER_SC,
    PEMLA_PIPE_STAGE_INGRESS_HARD_PMFB_SC,
    PEMLA_PIPE_STAGE_INGRESS_SOFT_PMFB_SOFT,
    PEMLA_PIPE_STAGE_INGRESS_HARD_LBP_SC,
    PEMLA_PIPE_STAGE_INGRESS_HARD_TM,
    PEMLA_PIPE_STAGE_EGRESS_ERPP_PARSER,
    PEMLA_PIPE_STAGE_EGRESS_ETPPA_HARD_PARSER_AND_PRP_SC,
    PEMLA_PIPE_STAGE_EGRESS_SOFT_PRP_SOFT,
    PEMLA_PIPE_STAGE_EGRESS_EDITOR_TERMINATION,
    PEMLA_PIPE_STAGE_EGRESS_EDITOR_FORWARDING,
    PEMLA_PIPE_STAGE_EGRESS_EDITOR_ENCAPSULATION1,
    PEMLA_PIPE_STAGE_EGRESS_EDITOR_ENCAPSULATION2,
    PEMLA_PIPE_STAGE_EGRESS_EDITOR_ENCAPSULATION3,
    PEMLA_PIPE_STAGE_EGRESS_EDITOR_ENCAPSULATION4,
    PEMLA_PIPE_STAGE_EGRESS_EDITOR_ENCAPSULATION5,
    PEMLA_PIPE_STAGE_EGRESS_EDITOR_TRAP,
    PEMLA_PIPE_STAGE_EGRESS_HARD_BTC_POST_PROCESSING_SC,

    PEMLA_BTC_REF_MODEL,

    PEMLA_UNDEFINED
} PEMLA_STAGES_ENUM_e;

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
