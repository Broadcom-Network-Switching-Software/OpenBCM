
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#define BSL_LOG_MODULE BSL_LS_SOCDNX_INIT_PP

#include <shared/shrextend/shrextend_debug.h>
#include <soc/dnx/pemladrv/pemladrv_virtual_wires.h>

shr_error_e
pemladrv_vw_stages_pemla_to_sdk(
    int unit,
    int pemla_stage,
    int* sdk_stage)
{
    SHR_FUNC_INIT_VARS(unit);

    switch(pemla_stage)
    {
        case PEMLA_PIPE_STAGE_INGRESS_HARD_LLR_SC :
            *sdk_stage = DNX_PP_STAGE_LLR;
            break;
        case PEMLA_PIPE_STAGE_INGRESS_KLEAP_VTA :
            *sdk_stage = DNX_PP_STAGE_VTT1;
            break;
        case PEMLA_PIPE_STAGE_INGRESS_KLEAP_VTB :
            *sdk_stage = DNX_PP_STAGE_VTT2;
            break;
        case PEMLA_PIPE_STAGE_INGRESS_KLEAP_VTC :
            *sdk_stage = DNX_PP_STAGE_VTT3;
            break;
        case PEMLA_PIPE_STAGE_INGRESS_KLEAP_VTD :
            *sdk_stage = DNX_PP_STAGE_VTT4;
            break;
        case PEMLA_PIPE_STAGE_INGRESS_KLEAP_VTE :
            *sdk_stage = DNX_PP_STAGE_VTT5;
            break;
        case PEMLA_PIPE_STAGE_INGRESS_KLEAP_FLPA :
            *sdk_stage = DNX_PP_STAGE_IFWD1;
            break;
        case PEMLA_PIPE_STAGE_INGRESS_KLEAP_FLPB :
            *sdk_stage = DNX_PP_STAGE_IFWD2;
            break;
        case PEMLA_PIPE_STAGE_INGRESS_HARD_PMFA_SC :
            *sdk_stage = DNX_PP_STAGE_IPMF1;
            break;
        case PEMLA_PIPE_STAGE_INGRESS_HARD_FER_SC :
            *sdk_stage = DNX_PP_STAGE_FER;
            break;
        case PEMLA_PIPE_STAGE_INGRESS_HARD_PMFB_SC :
            *sdk_stage = DNX_PP_STAGE_IPMF3;
            break;
        case PEMLA_PIPE_STAGE_INGRESS_HARD_LBP_SC :
            *sdk_stage = DNX_PP_STAGE_LBP;
            break;
        case PEMLA_PIPE_STAGE_EGRESS_ETPPA_HARD_PARSER_AND_PRP_SC :
            *sdk_stage = DNX_PP_STAGE_ETPARSER;
            break;
        case PEMLA_PIPE_STAGE_EGRESS_EDITOR_TERMINATION :
            *sdk_stage = DNX_PP_STAGE_TERM;
            break;
        case PEMLA_PIPE_STAGE_EGRESS_EDITOR_FORWARDING :
            *sdk_stage = DNX_PP_STAGE_EFWD;
            break;
        case PEMLA_PIPE_STAGE_EGRESS_EDITOR_ENCAPSULATION1 :
            *sdk_stage = DNX_PP_STAGE_ENC1;
            break;
        case PEMLA_PIPE_STAGE_EGRESS_EDITOR_ENCAPSULATION2 :
            *sdk_stage = DNX_PP_STAGE_ENC2;
            break;
        case PEMLA_PIPE_STAGE_EGRESS_EDITOR_ENCAPSULATION3 :
            *sdk_stage = DNX_PP_STAGE_ENC3;
            break;
        case PEMLA_PIPE_STAGE_EGRESS_EDITOR_ENCAPSULATION4 :
            *sdk_stage = DNX_PP_STAGE_ENC4;
            break;
        case PEMLA_PIPE_STAGE_EGRESS_EDITOR_ENCAPSULATION5 :
            *sdk_stage = DNX_PP_STAGE_ENC5;
            break;
        case PEMLA_PIPE_STAGE_EGRESS_EDITOR_TRAP :
            *sdk_stage = DNX_PP_STAGE_TRAP;
            break;
        case PEMLA_PIPE_STAGE_EGRESS_HARD_BTC_POST_PROCESSING_SC :
            *sdk_stage = DNX_PP_STAGE_BTC;
            break;
        case PEMLA_PIPE_STAGE_EGRESS_ERPP_PARSER :
        case PEMLA_PIPE_STAGE_INGRESS_HARD_TM :
        case PEMLA_PIPE_STAGE_INGRESS_SOFT_PMFB_SOFT :
        case PEMLA_PIPE_STAGE_INGRESS_SOFT_PMFA_SOFT :
        case PEMLA_PIPE_STAGE_EGRESS_SOFT_PRP_SOFT :
            *sdk_stage = DNX_PP_STAGE_INVALID;
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Invalid pemla stage number (%d)\n", pemla_stage);
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
pemladrv_vw_stages_sdk_to_pemla(
    int unit,
    int sdk_stage,
    int *pemla_stage)
{
    SHR_FUNC_INIT_VARS(unit);

    switch(sdk_stage)
    {
        case DNX_PP_STAGE_LLR :
            *pemla_stage = PEMLA_PIPE_STAGE_INGRESS_HARD_LLR_SC;
            break;
        case DNX_PP_STAGE_VTT1 :
            *pemla_stage = PEMLA_PIPE_STAGE_INGRESS_KLEAP_VTA;
            break;
        case DNX_PP_STAGE_VTT2 :
            *pemla_stage = PEMLA_PIPE_STAGE_INGRESS_KLEAP_VTB;
            break;
        case DNX_PP_STAGE_VTT3 :
            *pemla_stage = PEMLA_PIPE_STAGE_INGRESS_KLEAP_VTC;
            break;
        case DNX_PP_STAGE_VTT4 :
            *pemla_stage = PEMLA_PIPE_STAGE_INGRESS_KLEAP_VTD;
            break;
        case DNX_PP_STAGE_VTT5 :
            *pemla_stage = PEMLA_PIPE_STAGE_INGRESS_KLEAP_VTE;
            break;
        case DNX_PP_STAGE_IFWD1 :
            *pemla_stage = PEMLA_PIPE_STAGE_INGRESS_KLEAP_FLPA;
            break;
        case DNX_PP_STAGE_IFWD2 :
            *pemla_stage = PEMLA_PIPE_STAGE_INGRESS_KLEAP_FLPB;
            break;
        case DNX_PP_STAGE_IPMF1 :
            *pemla_stage = PEMLA_PIPE_STAGE_INGRESS_HARD_PMFA_SC;
            break;
        case DNX_PP_STAGE_FER :
            *pemla_stage = PEMLA_PIPE_STAGE_INGRESS_HARD_FER_SC;
            break;
        case DNX_PP_STAGE_IPMF3 :
            *pemla_stage = PEMLA_PIPE_STAGE_INGRESS_HARD_PMFB_SC;
            break;
        case DNX_PP_STAGE_LBP :
            *pemla_stage = PEMLA_PIPE_STAGE_INGRESS_HARD_LBP_SC;
            break;
        case DNX_PP_STAGE_ETPARSER :
            *pemla_stage = PEMLA_PIPE_STAGE_EGRESS_ETPPA_HARD_PARSER_AND_PRP_SC;
            break;
        case DNX_PP_STAGE_TERM :
            *pemla_stage = PEMLA_PIPE_STAGE_EGRESS_EDITOR_TERMINATION;
            break;
        case DNX_PP_STAGE_EFWD :
            *pemla_stage = PEMLA_PIPE_STAGE_EGRESS_EDITOR_FORWARDING;
            break;
        case DNX_PP_STAGE_ENC1 :
            *pemla_stage = PEMLA_PIPE_STAGE_EGRESS_EDITOR_ENCAPSULATION1;
            break;
        case DNX_PP_STAGE_ENC2 :
            *pemla_stage = PEMLA_PIPE_STAGE_EGRESS_EDITOR_ENCAPSULATION2;
            break;
        case DNX_PP_STAGE_ENC3 :
            *pemla_stage = PEMLA_PIPE_STAGE_EGRESS_EDITOR_ENCAPSULATION3;
            break;
        case DNX_PP_STAGE_ENC4 :
            *pemla_stage = PEMLA_PIPE_STAGE_EGRESS_EDITOR_ENCAPSULATION4;
            break;
        case DNX_PP_STAGE_ENC5 :
            *pemla_stage = PEMLA_PIPE_STAGE_EGRESS_EDITOR_ENCAPSULATION5;
            break;
        case DNX_PP_STAGE_TRAP :
            *pemla_stage = PEMLA_PIPE_STAGE_EGRESS_EDITOR_TRAP;
            break;
        case DNX_PP_STAGE_BTC :
            *pemla_stage = PEMLA_PIPE_STAGE_EGRESS_HARD_BTC_POST_PROCESSING_SC;
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Invalid sdk stage \"%s\" number (%d)\n",
                         dnx_pp_stage_name(unit, sdk_stage), sdk_stage);
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
pemladrv_vw_wire_get(
    int unit,
    char vw_name[MAX_VW_NAME_LENGTH],
    dnx_pp_stage_e sdk_stage,
    VirtualWireInfo** vw_info)
{
    VirtualWiresContainer* virtual_wires;
    int vw_idx, pemla_stage;
    int found_wire = FALSE;

    SHR_FUNC_INIT_VARS(unit);

    virtual_wires = dnx_pemladrv_get_virtual_wires_info(unit);

    SHR_IF_ERR_EXIT(pemladrv_vw_stages_sdk_to_pemla(unit, sdk_stage, &pemla_stage));

    for(vw_idx = 0; vw_idx < virtual_wires->nof_virtual_wires; vw_idx++)
    {
        if(sal_strcmp(virtual_wires->vw_info_arr[vw_idx].virtual_wire_name, vw_name) == 0)
        {
            if ((pemla_stage >= virtual_wires->vw_info_arr[vw_idx].start_stage) &&
                (pemla_stage <=  virtual_wires->vw_info_arr[vw_idx].end_stage))
            {
                *vw_info = &(virtual_wires->vw_info_arr[vw_idx]);
                found_wire = TRUE;
            }
            break;
        }
    }

    if (!found_wire)
    {
        SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Cannot find virtual wire named \"%.*s\".\n", MAX_VW_NAME_LENGTH, vw_name);
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
pemladrv_vw_wire_nof_by_stage(
    int unit,
    dnx_pp_stage_e sdk_stage,
    int *nof_vw)
{
    VirtualWiresContainer* virtual_wires;
    int vw_idx, pemla_stage;

    SHR_FUNC_INIT_VARS(unit);

    virtual_wires = dnx_pemladrv_get_virtual_wires_info(unit);

    SHR_IF_ERR_EXIT(pemladrv_vw_stages_sdk_to_pemla(unit, sdk_stage, &pemla_stage));

    (*nof_vw) = 0;

    for(vw_idx = 0; vw_idx < virtual_wires->nof_virtual_wires; vw_idx++)
    {
        if ((pemla_stage >= virtual_wires->vw_info_arr[vw_idx].start_stage) &&
            (pemla_stage <=  virtual_wires->vw_info_arr[vw_idx].end_stage))
        {
            (*nof_vw)++;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
pemladrv_vw_wire_iterator_by_stage_get_next(
    int unit,
    int nof_stages,
    dnx_pp_stage_e *sdk_stages,
    int *vw_iterator,
    VirtualWireInfo **vw_info,
    uint8 *stages_not_started)
{
    VirtualWiresContainer* virtual_wires;
    int vw_idx, stage_idx;
    int found = FALSE;
    int pemla_stages[DNX_PP_STAGE_NOF];

    SHR_FUNC_INIT_VARS(unit);

    if (nof_stages > DNX_PP_STAGE_NOF)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "There are more PP stages in input (%d) then PP stages (%d).\n",
                     nof_stages, DNX_PP_STAGE_NOF);
    }
    else if (nof_stages <= 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "There must be at least one stage given (%d given).\n", nof_stages);
    }

    if ((*vw_iterator) == PEMLADRV_ITERATOR_BY_STAGE_END)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Iteration ended.\n");
    }
    else if (((*vw_iterator) != PEMLADRV_ITERATOR_BY_STAGE_INIT) && ((*vw_iterator) < 0))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid value for iterator %d.\n", *vw_iterator);
    }

    sal_memset(stages_not_started, 0x0, nof_stages*sizeof(stages_not_started[0]));

    virtual_wires = dnx_pemladrv_get_virtual_wires_info(unit);

    for (stage_idx=0; stage_idx < nof_stages; stage_idx++)
    {
        SHR_IF_ERR_EXIT(pemladrv_vw_stages_sdk_to_pemla(unit, sdk_stages[stage_idx], &pemla_stages[stage_idx]));
    }

    if ((*vw_iterator) == PEMLADRV_ITERATOR_BY_STAGE_INIT)
    {
        vw_idx = 0;
    }
    else
    {
        vw_idx = (*vw_iterator) + 1;
    }

    if (vw_idx < 0)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Invalid iterator, wraps around (%d).\n", *vw_iterator);
    }

    for(; vw_idx < virtual_wires->nof_virtual_wires; vw_idx++)
    {
        for (stage_idx=0; stage_idx < nof_stages; stage_idx++)
        {
            if ((pemla_stages[stage_idx] >= virtual_wires->vw_info_arr[vw_idx].start_stage) &&
                (pemla_stages[stage_idx] <=  virtual_wires->vw_info_arr[vw_idx].end_stage))
            {
                if (found == FALSE)
                {
                    (*vw_iterator) = vw_idx;
                    (*vw_info) = &(virtual_wires->vw_info_arr[vw_idx]);
                    found = TRUE;
                }
                if (pemla_stages[stage_idx] > virtual_wires->vw_info_arr[vw_idx].start_stage)
                {
                    stages_not_started[stage_idx] = TRUE;
                }
            }
        }
        if (found)
        {
            break;
        }
    }
    if (found == FALSE)
    {
        (*vw_iterator) = PEMLADRV_ITERATOR_BY_STAGE_END;
    }

exit:
    SHR_FUNC_EXIT;
}
