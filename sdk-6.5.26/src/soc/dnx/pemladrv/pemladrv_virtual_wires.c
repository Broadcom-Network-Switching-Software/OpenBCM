
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#define BSL_LOG_MODULE BSL_LS_SOCDNX_INIT_PP

#include <soc/sand/shrextend/shrextend_debug.h>
#include <soc/dnx/pemladrv/pemladrv_virtual_wires.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_pp.h>

shr_error_e
pemladrv_vw_stages_pemla_to_sdk(
    int unit,
    int pemla_stage,
    int* sdk_stage)
{
    int stage_idx;

    SHR_FUNC_INIT_VARS(unit);

    *sdk_stage = DNX_PP_STAGE_INVALID;
    for(stage_idx = 0; stage_idx < DNX_PP_STAGE_NOF; stage_idx++)
    {
        if(pemla_stage == dnx_data_pp.stages.params_get(unit, stage_idx)->chuck_block_id)
        {
            *sdk_stage = stage_idx;
            break;
        }
    }

    SHR_EXIT();

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

    if(dnx_data_pp.stages.params_get(unit, sdk_stage)->is_pem)
    {
        *pemla_stage = dnx_data_pp.stages.params_get(unit, sdk_stage)->chuck_block_id;
    }
    else
    {
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
    int vw_idx, pemla_stage = 0;
    int found_wire = FALSE;

    SHR_FUNC_INIT_VARS(unit);

    virtual_wires = dnx_pemladrv_get_virtual_wires_info(unit);

    if (dnx_data_pp.stages.params_get(unit, sdk_stage)->valid)
    {
        SHR_IF_ERR_EXIT(pemladrv_vw_stages_sdk_to_pemla(unit, sdk_stage, &pemla_stage));

        for(vw_idx = 0; vw_idx < virtual_wires->nof_virtual_wires; vw_idx++)
        {
            if(sal_strncmp(virtual_wires->vw_info_arr[vw_idx].virtual_wire_name, vw_name, sizeof(virtual_wires->vw_info_arr[vw_idx].virtual_wire_name)) == 0)
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
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Invalid stage %s\n", dnx_pp_stage_name(unit, sdk_stage));
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
    int vw_idx, pemla_stage = 0;

    SHR_FUNC_INIT_VARS(unit);

    virtual_wires = dnx_pemladrv_get_virtual_wires_info(unit);
    (*nof_vw) = 0;

    if (dnx_data_pp.stages.params_get(unit, sdk_stage)->valid)
    {
        SHR_IF_ERR_EXIT(pemladrv_vw_stages_sdk_to_pemla(unit, sdk_stage, &pemla_stage));

        for(vw_idx = 0; vw_idx < virtual_wires->nof_virtual_wires; vw_idx++)
        {
            if ((pemla_stage >= virtual_wires->vw_info_arr[vw_idx].start_stage) &&
                (pemla_stage <=  virtual_wires->vw_info_arr[vw_idx].end_stage))
            {
                (*nof_vw)++;
            }
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
    int pemla_stages[DNX_PP_STAGE_NOF] = {0};

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
        if (dnx_data_pp.stages.params_get(unit, sdk_stages[stage_idx])->valid)
        {
            SHR_IF_ERR_EXIT(pemladrv_vw_stages_sdk_to_pemla(unit, sdk_stages[stage_idx], &pemla_stages[stage_idx]));
        }
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
