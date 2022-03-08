/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 *
 * SOC DNXF PORT
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif

#define BSL_LOG_MODULE BSL_LS_SOCDNX_FABRIC

#ifdef BCM_DNXF_SUPPORT

#include <shared/bsl.h>
#include <shared/fabric.h>
#include <shared/bitop.h>
#include <soc/sand/shrextend/shrextend_debug.h>

#include <soc/defs.h>
#include <soc/error.h>
#include <soc/types.h>

#include <soc/dnxc/error.h>
#include <soc/dnxc/dnxc_defs.h>

#include <soc/dnxf/cmn/dnxf_defs.h>
#include <soc/dnxf/cmn/dnxf_drv.h>
#include <soc/dnxf/cmn/dnxf_fabric.h>
#include <soc/dnxf/cmn/dnxf_port.h>
#include <soc/dnxf/cmn/mbcm.h>
#include <soc/dnxf/cmn/dnxf_cgm.h>
#include <soc/dnxf/ramon/ramon_cgm.h>

#include <soc/dnxf/dnxf_data/auto_generated/dnxf_data.h>
#include <soc/dnxf/dnxf_data/auto_generated/dnxf_data_port.h>
#include <soc/dnxf/dnxf_data/auto_generated/dnxf_data_fabric.h>
#include <soc/dnxf/dnxf_data/auto_generated/dnxf_data_max_fabric.h>

_shr_error_t
soc_dnxf_cgm_pipe_level_type_decode(
    int unit,
    _shr_dnxf_threshold_id_t threshold_id,
    int *level,
    int *pipe)
{
    SHR_FUNC_INIT_VARS(unit);

    if (!_SHR_FABRIC_TH_INDEX_IS_PIPE_LEVEL(threshold_id))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "%d is not a valid threshold_id for this threshold type", threshold_id);
    }

    *level = _SHR_FABRIC_TH_INDEX_PIPE_LEVEL_LEVEL_GET(threshold_id);
    *pipe = _SHR_FABRIC_TH_INDEX_PIPE_LEVEL_PIPE_GET(threshold_id);

exit:
    SHR_FUNC_EXIT;
}

_shr_error_t
soc_dnxf_cgm_pipe_link_type_decode(
    int unit,
    _shr_dnxf_threshold_id_t threshold_id,
    int *link,
    int *pipe)
{
    SHR_FUNC_INIT_VARS(unit);

    if (!_SHR_FABRIC_TH_INDEX_IS_PIPE_LINK(threshold_id))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "%d is not a valid threshold_id for this threshold type", threshold_id);
    }

    *link = _SHR_FABRIC_TH_INDEX_PIPE_LINK_LINK_GET(threshold_id);
    *pipe = _SHR_FABRIC_TH_INDEX_PIPE_LINK_PIPE_GET(threshold_id);

exit:
    SHR_FUNC_EXIT;
}

_shr_error_t
soc_dnxf_cgm_cast_prio_type_decode(
    int unit,
    _shr_dnxf_threshold_id_t threshold_id,
    int *cast,
    int *prio)
{
    SHR_FUNC_INIT_VARS(unit);

    if (!_SHR_FABRIC_TH_INDEX_IS_CAST_PRIO(threshold_id))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "%d is not a valid threshold_id for this threshold type", threshold_id);
    }

    *cast = _SHR_FABRIC_TH_INDEX_CAST_PRIO_CAST_GET(threshold_id);
    *prio = _SHR_FABRIC_TH_INDEX_CAST_PRIO_PRIO_GET(threshold_id);

exit:
    SHR_FUNC_EXIT;
}

static _shr_error_t
soc_dnxf_cgm_threshold_index_get(
    int unit,
    int threshold_id,
    _shr_dnxf_cgm_index_type_t index_type,
    uint32 *start_index,
    uint32 *end_index)
{

    int index_dim[DNXF_DATA_MAX_FABRIC_CONGESTION_NOF_THRESHOLD_INDEX_DIMENSIONS];
    int max_nof_indexes_dim[DNXF_DATA_MAX_FABRIC_CONGESTION_NOF_THRESHOLD_INDEX_DIMENSIONS];
    int i, max_link;
    SHR_FUNC_INIT_VARS(unit);

    switch (index_type)
    {
        case _shr_dnxf_cgm_index_type_pipe:

            max_nof_indexes_dim[0] = 1;
            max_nof_indexes_dim[1] = dnxf_data_fabric.pipes.nof_pipes_get(unit);

            index_dim[0] = 0;
            index_dim[1] = threshold_id;
            break;
        case _shr_dnxf_cgm_index_type_priority:

            max_nof_indexes_dim[0] = 1;
            max_nof_indexes_dim[1] = dnxf_data_fabric.congestion.nof_threshold_priorities_get(unit);

            index_dim[0] = 0;
            index_dim[1] = threshold_id;
            break;
        case _shr_dnxf_cgm_index_type_pipe_level:

            max_nof_indexes_dim[0] = dnxf_data_fabric.congestion.nof_threshold_levels_get(unit);
            max_nof_indexes_dim[1] = dnxf_data_fabric.pipes.nof_pipes_get(unit);

            soc_dnxf_cgm_pipe_level_type_decode(unit, threshold_id, &index_dim[0], &index_dim[1]);
            break;
        case _shr_dnxf_cgm_index_type_pipe_link:

            SHR_IF_ERR_EXIT(soc_dnxf_port_max_port_get(unit, &max_link));

            max_nof_indexes_dim[0] = max_link;
            max_nof_indexes_dim[1] = dnxf_data_fabric.pipes.nof_pipes_get(unit);

            soc_dnxf_cgm_pipe_link_type_decode(unit, threshold_id, &index_dim[0], &index_dim[1]);
            break;
        case _shr_dnxf_cgm_index_type_cast_prio:

            max_nof_indexes_dim[0] = 2;
            max_nof_indexes_dim[1] = dnxf_data_fabric.congestion.nof_threshold_priorities_get(unit);

            soc_dnxf_cgm_cast_prio_type_decode(unit, threshold_id, &index_dim[0], &index_dim[1]);
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Not supported threshold index.");
    }

    for (i = 0; i < dnxf_data_fabric.congestion.nof_threshold_index_dimensions_get(unit); i++)
    {
        start_index[i] = index_dim[i] == -1 ? 0 : index_dim[i];
        end_index[i] = index_dim[i] == -1 ? max_nof_indexes_dim[i] - 1 : index_dim[i];
    }

exit:
    SHR_FUNC_EXIT;
}

static _shr_error_t
soc_dnxf_cgm_handle_register_structure_1(
    int unit,
    uint32 start_index[DNXF_DATA_MAX_FABRIC_CONGESTION_NOF_THRESHOLD_INDEX_DIMENSIONS],
    uint32 end_index[DNXF_DATA_MAX_FABRIC_CONGESTION_NOF_THRESHOLD_INDEX_DIMENSIONS],
    soc_pbmp_t block_bmp,
    _shr_dnxf_cgm_index_type_t index_type,
    uint32 lr_nlr_offset,
    int profile_id,
    _shr_dnxf_threshold_type_t threshold_type,
    uint32 set_or_get,
    int *threshold_value)
{
    const soc_dnxf_cgm_registers_table_t *threshold_reg_description;
    int i, j, dim0, dim1, max_th_value, threshold_value_tmp, first_block;
    uint32 dimension_offset = 0, reg_offset = 0;
    uint32 nof_regs_to_write_to = 0;
    soc_reg_above_64_val_t reg_above_64_val;
    soc_reg_t reg_name;
    soc_field_t field_name;

    const soc_dnxf_cgm_threshold_to_reg_binding_t *threhold_to_reg_info_binding_table;

    SHR_FUNC_INIT_VARS(unit);

    _SHR_PBMP_FIRST(block_bmp, first_block);

    SHR_IF_ERR_EXIT(MBCM_DNXF_DRIVER_CALL(unit, mbcm_dnxf_cgm_threshold_to_reg_table_get,
                                          (unit, &threhold_to_reg_info_binding_table)));

    threshold_reg_description = threhold_to_reg_info_binding_table[threshold_type].reg_description_ptr;

    nof_regs_to_write_to = threhold_to_reg_info_binding_table[threshold_type].nof_regs_to_write_to;

    for (i = 0; i < nof_regs_to_write_to; i++)
    {

        reg_offset = lr_nlr_offset * nof_regs_to_write_to;

        reg_name = threshold_reg_description[i + reg_offset].register_name;

        switch ((_shr_dnxf_threshold_id_t) dnxf_data_fabric.congestion.
                thresholds_info_get(unit, threshold_type)->index_type)
        {
            case _shr_dnxf_cgm_index_type_pipe:
                dimension_offset = dnxf_data_fabric.pipes.max_nof_pipes_get(unit);
                break;
            case _shr_dnxf_cgm_index_type_pipe_level:

                dimension_offset = dnxf_data_fabric.pipes.max_nof_pipes_get(unit);
                break;
            case _shr_dnxf_cgm_index_type_priority:

                dimension_offset = dnxf_data_fabric.congestion.nof_threshold_priorities_get(unit);
                break;
            case _shr_dnxf_cgm_index_type_cast_prio:

                dimension_offset = dnxf_data_fabric.congestion.nof_threshold_priorities_get(unit);
                break;
            default:
                SHR_ERR_EXIT(_SHR_E_PARAM, "Not supported threhold index.");
        }

        for (dim0 = start_index[0]; dim0 <= end_index[0]; dim0++)
        {
            for (dim1 = start_index[1] + dim0 * dimension_offset; dim1 <= end_index[1] + dim0 * dimension_offset;
                 dim1++)
            {

                field_name = threshold_reg_description[i + reg_offset].field_name_array[dim1];
                if (set_or_get == _SHR_FABRIC_FLAG_SET)
                {

                    threshold_value_tmp = *threshold_value;

                    max_th_value = soc_reg_field_length(unit, reg_name, field_name);
                    max_th_value = ((1 << max_th_value) - 1);

                    if (threshold_value_tmp > max_th_value)
                    {
                        SHR_ERR_EXIT(_SHR_E_PARAM, "Maximum value for this threshold is %d", max_th_value);
                    }

                    if (threshold_value_tmp == -1)
                    {
                        threshold_value_tmp = max_th_value;
                    }

                    SOC_PBMP_ITER(block_bmp, j)
                    {
                        SHR_IF_ERR_EXIT(soc_reg_above_64_get(unit, reg_name, j, profile_id, reg_above_64_val));
                        soc_reg_above_64_field32_set(unit, reg_name, reg_above_64_val, field_name, threshold_value_tmp);
                        SHR_IF_ERR_EXIT(soc_reg_above_64_set(unit, reg_name, j, profile_id, reg_above_64_val));
                    }
                }
                else if (set_or_get == _SHR_FABRIC_FLAG_GET)
                {
                    SHR_IF_ERR_EXIT(soc_reg_above_64_get(unit, reg_name, first_block, profile_id, reg_above_64_val));
                    *threshold_value = soc_reg_above_64_field32_get(unit, reg_name, reg_above_64_val, field_name);

                    SHR_EXIT();

                }
                else
                {
                    SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unsupported handle case!");
                }
            }
        }
    }
exit:
    SHR_FUNC_EXIT;
}

static _shr_error_t
soc_dnxf_cgm_handle_register_structure_2(
    int unit,
    uint32 start_index[DNXF_DATA_MAX_FABRIC_CONGESTION_NOF_THRESHOLD_INDEX_DIMENSIONS],
    uint32 end_index[DNXF_DATA_MAX_FABRIC_CONGESTION_NOF_THRESHOLD_INDEX_DIMENSIONS],
    soc_pbmp_t block_bmp,
    uint32 lr_nlr_offset,
    int profile_id,
    _shr_dnxf_threshold_type_t threshold_type,
    uint32 set_or_get,
    int *threshold_value)
{
    const soc_dnxf_cgm_registers_table_t *threshold_reg_description;
    int j, i, dim0, dim1, max_th_value, first_block;
    soc_reg_above_64_val_t reg_above_64_val;
    soc_reg_t reg_name;
    uint32 reg_offset, nof_regs_to_write_to = 0;
    soc_field_t field_name;

    const soc_dnxf_cgm_threshold_to_reg_binding_t *threhold_to_reg_info_binding_table;

    SHR_FUNC_INIT_VARS(unit);

    _SHR_PBMP_FIRST(block_bmp, first_block);

    SHR_IF_ERR_EXIT(MBCM_DNXF_DRIVER_CALL(unit, mbcm_dnxf_cgm_threshold_to_reg_table_get,
                                          (unit, &threhold_to_reg_info_binding_table)));

    threshold_reg_description = threhold_to_reg_info_binding_table[threshold_type].reg_description_ptr;

    nof_regs_to_write_to = threhold_to_reg_info_binding_table[threshold_type].nof_regs_to_write_to;

    for (i = 0; i < nof_regs_to_write_to; i++)
    {

        reg_offset = lr_nlr_offset * nof_regs_to_write_to;

        for (dim0 = start_index[0]; dim0 <= end_index[0]; dim0++)
        {

            for (dim1 = start_index[1]; dim1 <= end_index[1]; dim1++)
            {

                reg_name = threshold_reg_description[dim1 + 3 * (reg_offset + i)].register_name;

                field_name = threshold_reg_description[dim1 + 3 * (reg_offset + i)].field_name_array[dim0];
                if (set_or_get == _SHR_FABRIC_FLAG_SET)
                {

                    max_th_value = soc_reg_field_length(unit, reg_name, field_name);
                    max_th_value = ((1 << max_th_value) - 1);

                    if (*threshold_value > max_th_value)
                    {
                        SHR_ERR_EXIT(_SHR_E_PARAM, "Maximum value for this threshold is %d", max_th_value);
                    }

                    if (*threshold_value == -1)
                        *threshold_value = max_th_value;

                    SOC_PBMP_ITER(block_bmp, j)
                    {
                        SHR_IF_ERR_EXIT(soc_reg_above_64_get(unit, reg_name, j, profile_id, reg_above_64_val));
                        soc_reg_above_64_field32_set(unit, reg_name, reg_above_64_val, field_name, *threshold_value);
                        SHR_IF_ERR_EXIT(soc_reg_above_64_set(unit, reg_name, j, profile_id, reg_above_64_val));
                    }
                }
                else if (set_or_get == _SHR_FABRIC_FLAG_GET)
                {
                    SHR_IF_ERR_EXIT(soc_reg_above_64_get(unit, reg_name, first_block, profile_id, reg_above_64_val));
                    *threshold_value = soc_reg_above_64_field32_get(unit, reg_name, reg_above_64_val, field_name);

                    SHR_EXIT();
                }
                else
                {
                    SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unsupported handle case!");
                }
            }
        }
    }
exit:
    SHR_FUNC_EXIT;

}

static _shr_error_t
soc_dnxf_cgm_handle_register_structure_3(
    int unit,
    uint32 start_index[DNXF_DATA_MAX_FABRIC_CONGESTION_NOF_THRESHOLD_INDEX_DIMENSIONS],
    uint32 end_index[DNXF_DATA_MAX_FABRIC_CONGESTION_NOF_THRESHOLD_INDEX_DIMENSIONS],
    soc_pbmp_t block_bmp,
    uint32 lr_nlr_offset,
    int profile_id,
    _shr_dnxf_threshold_type_t threshold_type,
    uint32 set_or_get,
    int *threshold_value)
{
    const soc_dnxf_cgm_registers_table_t *threshold_reg_description;
    int j, dim0, dim1, max_th_value, first_block;
    soc_reg_above_64_val_t reg_above_64_val;
    soc_reg_t reg_name;
    uint32 reg_offset, nof_regs_to_write_to = 0;
    soc_field_t field_name;

    const soc_dnxf_cgm_threshold_to_reg_binding_t *threhold_to_reg_info_binding_table;

    SHR_FUNC_INIT_VARS(unit);

    _SHR_PBMP_FIRST(block_bmp, first_block);

    SHR_IF_ERR_EXIT(MBCM_DNXF_DRIVER_CALL(unit, mbcm_dnxf_cgm_threshold_to_reg_table_get,
                                          (unit, &threhold_to_reg_info_binding_table)));

    threshold_reg_description = threhold_to_reg_info_binding_table[threshold_type].reg_description_ptr;

    nof_regs_to_write_to = threhold_to_reg_info_binding_table[threshold_type].nof_regs_to_write_to;

    if (nof_regs_to_write_to > 1)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "CGM register structures don't support handling of more than 1 register");
    }

    reg_offset = profile_id;

    for (dim0 = start_index[0]; dim0 <= end_index[0]; dim0++)
    {
        for (dim1 = start_index[1]; dim1 <= end_index[1]; dim1++)
        {

            reg_name = threshold_reg_description[reg_offset].register_name;

            field_name = threshold_reg_description[reg_offset].field_name_array[dim0];
            if (set_or_get == _SHR_FABRIC_FLAG_SET)
            {

                max_th_value = soc_reg_field_length(unit, reg_name, field_name);
                max_th_value = ((1 << max_th_value) - 1);
                if (*threshold_value > max_th_value)
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM, "Maximum value for this threshold is %d", max_th_value);
                }

                if (*threshold_value == -1)
                    *threshold_value = max_th_value;

                SOC_PBMP_ITER(block_bmp, j)
                {
                    SHR_IF_ERR_EXIT(soc_reg_above_64_get(unit, reg_name, j, dim1, reg_above_64_val));
                    soc_reg_above_64_field32_set(unit, reg_name, reg_above_64_val, field_name, *threshold_value);
                    SHR_IF_ERR_EXIT(soc_reg_above_64_set(unit, reg_name, j, dim1, reg_above_64_val));
                }
            }
            else if (set_or_get == _SHR_FABRIC_FLAG_GET)
            {
                SHR_IF_ERR_EXIT(soc_reg_above_64_get(unit, reg_name, first_block, dim1, reg_above_64_val));
                *threshold_value = soc_reg_above_64_field32_get(unit, reg_name, reg_above_64_val, field_name);

                SHR_EXIT();
            }
            else
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unsupported handle case!");
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

static _shr_error_t
soc_dnxf_cgm_handle_register_structure_4(
    int unit,
    uint32 start_index[DNXF_DATA_MAX_FABRIC_CONGESTION_NOF_THRESHOLD_INDEX_DIMENSIONS],
    uint32 end_index[DNXF_DATA_MAX_FABRIC_CONGESTION_NOF_THRESHOLD_INDEX_DIMENSIONS],
    soc_pbmp_t block_bmp,
    uint32 lr_nlr_offset,
    int profile_id,
    _shr_dnxf_threshold_type_t threshold_type,
    uint32 set_or_get,
    int *threshold_value)
{
    const soc_dnxf_cgm_registers_table_t *threshold_reg_description;
    int j, dim1, max_th_value, first_block;
    soc_reg_above_64_val_t reg_above_64_val;
    soc_reg_t reg_name;
    uint32 reg_offset, nof_regs_to_write_to = 0;
    soc_field_t field_name;

    const soc_dnxf_cgm_threshold_to_reg_binding_t *threhold_to_reg_info_binding_table;

    SHR_FUNC_INIT_VARS(unit);

    _SHR_PBMP_FIRST(block_bmp, first_block);

    SHR_IF_ERR_EXIT(MBCM_DNXF_DRIVER_CALL(unit, mbcm_dnxf_cgm_threshold_to_reg_table_get,
                                          (unit, &threhold_to_reg_info_binding_table)));

    threshold_reg_description = threhold_to_reg_info_binding_table[threshold_type].reg_description_ptr;

    nof_regs_to_write_to = threhold_to_reg_info_binding_table[threshold_type].nof_regs_to_write_to;

    if (nof_regs_to_write_to > 1)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "CGM register structures don't support handling of more than 1 register");
    }

    for (dim1 = start_index[1]; dim1 <= end_index[1]; dim1++)
    {

        reg_offset = dim1;

        reg_name = threshold_reg_description[reg_offset].register_name;

        field_name = threshold_reg_description[reg_offset].field_name_array[profile_id];
        if (set_or_get == _SHR_FABRIC_FLAG_SET)
        {

            max_th_value = soc_reg_field_length(unit, reg_name, field_name);
            max_th_value = ((1 << max_th_value) - 1);
            if (*threshold_value > max_th_value)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Maximum value for this threshold is %d", max_th_value);
            }

            if (*threshold_value == -1)
                *threshold_value = max_th_value;

            SOC_PBMP_ITER(block_bmp, j)
            {
                SHR_IF_ERR_EXIT(soc_reg_above_64_get(unit, reg_name, j, 0, reg_above_64_val));
                soc_reg_above_64_field32_set(unit, reg_name, reg_above_64_val, field_name, *threshold_value);
                SHR_IF_ERR_EXIT(soc_reg_above_64_set(unit, reg_name, j, 0, reg_above_64_val));
            }
        }
        else if (set_or_get == _SHR_FABRIC_FLAG_GET)
        {
            SHR_IF_ERR_EXIT(soc_reg_above_64_get(unit, reg_name, first_block, 0, reg_above_64_val));
            *threshold_value = soc_reg_above_64_field32_get(unit, reg_name, reg_above_64_val, field_name);

            SHR_EXIT();
        }
        else
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unsupported handle case!");
        }
    }
exit:
    SHR_FUNC_EXIT;
}

static _shr_error_t
soc_dnxf_cgm_handle_register_structure_5(
    int unit,
    uint32 start_index[DNXF_DATA_MAX_FABRIC_CONGESTION_NOF_THRESHOLD_INDEX_DIMENSIONS],
    uint32 end_index[DNXF_DATA_MAX_FABRIC_CONGESTION_NOF_THRESHOLD_INDEX_DIMENSIONS],
    soc_pbmp_t block_bmp,
    uint32 lr_nlr_offset,
    int profile_id,
    _shr_dnxf_threshold_type_t threshold_type,
    uint32 set_or_get,
    int *threshold_value)
{
    const soc_dnxf_cgm_registers_table_t *threshold_reg_description;
    int dim0, dim1, max_th_value;
    uint32 reg_val;
    soc_reg_t reg_name;
    uint32 reg_offset, nof_regs_to_write_to = 0;
    soc_field_t field_name;
    int fmac_index, fmac_inner_link;
    const soc_dnxf_cgm_threshold_to_reg_binding_t *threhold_to_reg_info_binding_table;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(MBCM_DNXF_DRIVER_CALL(unit, mbcm_dnxf_cgm_threshold_to_reg_table_get,
                                          (unit, &threhold_to_reg_info_binding_table)));

    threshold_reg_description = threhold_to_reg_info_binding_table[threshold_type].reg_description_ptr;

    nof_regs_to_write_to = threhold_to_reg_info_binding_table[threshold_type].nof_regs_to_write_to;

    if (nof_regs_to_write_to > 1)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "CGM register structures don't support handling of more than 1 register");
    }

    for (dim0 = start_index[0]; dim0 <= end_index[0]; dim0++)
    {

        if (!SOC_PBMP_MEMBER(SOC_INFO(unit).sfi.bitmap, dim0))
        {
            continue;
        }
        SHR_IF_ERR_EXIT(soc_dnxf_drv_link_to_block_mapping(unit, dim0, &fmac_index, &fmac_inner_link, SOC_BLK_FMAC));

        for (dim1 = start_index[1]; dim1 <= end_index[1]; dim1++)
        {

            reg_offset = 0;

            reg_name = threshold_reg_description[reg_offset].register_name;

            field_name = threshold_reg_description[reg_offset].field_name_array[dim1];
            if (set_or_get == _SHR_FABRIC_FLAG_SET)
            {

                max_th_value = soc_reg_field_length(unit, reg_name, field_name);
                max_th_value = ((1 << max_th_value) - 1);
                if (*threshold_value > max_th_value)
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM, "Maximum value for this threshold is %d", max_th_value);
                }

                if (*threshold_value == -1)
                    *threshold_value = max_th_value;

                SHR_IF_ERR_EXIT(soc_reg32_get(unit, reg_name, fmac_index, fmac_inner_link, &reg_val));
                soc_reg_field_set(unit, reg_name, &reg_val, field_name, *threshold_value);
                SHR_IF_ERR_EXIT(soc_reg32_set(unit, reg_name, fmac_index, fmac_inner_link, reg_val));

            }
            else if (set_or_get == _SHR_FABRIC_FLAG_GET)
            {
                SHR_IF_ERR_EXIT(soc_reg32_get(unit, reg_name, fmac_index, fmac_inner_link, &reg_val));
                *threshold_value = soc_reg_field_get(unit, reg_name, reg_val, field_name);
                SHR_EXIT();
            }
            else
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unsupported handle case!");
            }
        }
    }
exit:
    SHR_FUNC_EXIT;
}

_shr_error_t
soc_dnxf_cgm_profile_threshold_set(
    int unit,
    int profile_id,
    _shr_dnxf_threshold_id_t threshold_id,
    _shr_dnxf_threshold_type_t threshold_type,
    uint32 flags,
    int threshold_value)
{
    uint32 start_index[DNXF_DATA_MAX_FABRIC_CONGESTION_NOF_THRESHOLD_INDEX_DIMENSIONS];
    uint32 end_index[DNXF_DATA_MAX_FABRIC_CONGESTION_NOF_THRESHOLD_INDEX_DIMENSIONS];
    int block_id;
    _shr_dnxf_cgm_index_type_t index_type;
    soc_dnxf_cgm_reg_structure_t threshold_handle_case;
    soc_pbmp_t block_bmp;

    uint32 lr_nlr_offset = 0;

    const soc_dnxf_cgm_threshold_to_reg_binding_t *threhold_to_reg_info_binding_table;

    SHR_FUNC_INIT_VARS(unit);

    index_type =
        (_shr_dnxf_threshold_id_t) dnxf_data_fabric.congestion.thresholds_info_get(unit, threshold_type)->index_type;

    SHR_IF_ERR_EXIT(soc_dnxf_cgm_threshold_index_get(unit, threshold_id, index_type, start_index, end_index));

    if (flags & _SHR_FABRIC_TH_FLAG_LR)
    {
        lr_nlr_offset = 1;
    }

    block_id = dnxf_data_fabric.congestion.thresholds_info_get(unit, threshold_type)->is_tx ? SOC_BLK_DTL : SOC_BLK_DCH;
    if (flags & _SHR_FABRIC_TH_FLAG_FE1_ONLY)
    {
        SHR_IF_ERR_EXIT(soc_dnxf_block_instance_bmp_per_device_stage_get
                        (unit, block_id, soc_dnxf_fabric_device_stage_fe1, &block_bmp));
    }
    else if (flags & _SHR_FABRIC_TH_FLAG_FE3_ONLY)
    {
        SHR_IF_ERR_EXIT(soc_dnxf_block_instance_bmp_per_device_stage_get
                        (unit, block_id, soc_dnxf_fabric_device_stage_fe3, &block_bmp));
    }
    else
    {
        SHR_IF_ERR_EXIT(soc_dnxf_block_instance_bmp_per_device_stage_get
                        (unit, block_id, soc_dnxf_fabric_device_stage_fe2, &block_bmp));
    }

    SHR_IF_ERR_EXIT(MBCM_DNXF_DRIVER_CALL(unit, mbcm_dnxf_cgm_threshold_to_reg_table_get,
                                          (unit, &threhold_to_reg_info_binding_table)));

    threshold_handle_case = threhold_to_reg_info_binding_table[threshold_type].th_handle_case;

    switch (threshold_handle_case)
    {
        case soc_dnxf_cgm_reg_structure_1:

            SHR_IF_ERR_EXIT(soc_dnxf_cgm_handle_register_structure_1
                            (unit, start_index, end_index, block_bmp, index_type, lr_nlr_offset,
                             profile_id, threshold_type, _SHR_FABRIC_FLAG_SET, &threshold_value));
            break;
        case soc_dnxf_cgm_reg_structure_2:

            SHR_IF_ERR_EXIT(soc_dnxf_cgm_handle_register_structure_2
                            (unit, start_index, end_index, block_bmp, lr_nlr_offset, profile_id,
                             threshold_type, _SHR_FABRIC_FLAG_SET, &threshold_value));
            break;
        case soc_dnxf_cgm_reg_structure_3:

            SHR_IF_ERR_EXIT(soc_dnxf_cgm_handle_register_structure_3
                            (unit, start_index, end_index, block_bmp, lr_nlr_offset, profile_id,
                             threshold_type, _SHR_FABRIC_FLAG_SET, &threshold_value));
            break;
        case soc_dnxf_cgm_reg_structure_4:

            SHR_IF_ERR_EXIT(soc_dnxf_cgm_handle_register_structure_4
                            (unit, start_index, end_index, block_bmp, lr_nlr_offset, profile_id,
                             threshold_type, _SHR_FABRIC_FLAG_SET, &threshold_value));
            break;
        case soc_dnxf_cgm_reg_structure_5:

            SHR_IF_ERR_EXIT(soc_dnxf_cgm_handle_register_structure_5
                            (unit, start_index, end_index, block_bmp, lr_nlr_offset, profile_id,
                             threshold_type, _SHR_FABRIC_FLAG_SET, &threshold_value));
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Unsupported Type %d", threshold_handle_case);
            break;
    }
exit:

    SHR_FUNC_EXIT;
}

_shr_error_t
soc_dnxf_cgm_profile_threshold_get(
    int unit,
    int profile_id,
    _shr_dnxf_threshold_id_t threshold_id,
    _shr_dnxf_threshold_type_t threshold_type,
    uint32 flags,
    int *threshold_value)
{

    int block_id, rc;
    uint32 start_index[DNXF_DATA_MAX_FABRIC_CONGESTION_NOF_THRESHOLD_INDEX_DIMENSIONS];
    uint32 end_index[DNXF_DATA_MAX_FABRIC_CONGESTION_NOF_THRESHOLD_INDEX_DIMENSIONS];
    _shr_dnxf_cgm_index_type_t index_type;
    soc_pbmp_t block_bmp;
    soc_dnxf_cgm_reg_structure_t threshold_handle_case;

    uint32 lr_nlr_offset = 0;

    const soc_dnxf_cgm_threshold_to_reg_binding_t *threhold_to_reg_info_binding_table;

    SHR_FUNC_INIT_VARS(unit);

    index_type =
        (_shr_dnxf_threshold_id_t) dnxf_data_fabric.congestion.thresholds_info_get(unit, threshold_type)->index_type;

    SHR_IF_ERR_EXIT(soc_dnxf_cgm_threshold_index_get(unit, threshold_id, index_type, start_index, end_index));

    if (flags & _SHR_FABRIC_TH_FLAG_LR)
    {
        lr_nlr_offset = 1;
    }

    block_id = dnxf_data_fabric.congestion.thresholds_info_get(unit, threshold_type)->is_tx ? SOC_BLK_DTL : SOC_BLK_DCH;
    if (flags & _SHR_FABRIC_TH_FLAG_FE1_ONLY)
    {
        SHR_IF_ERR_EXIT(soc_dnxf_block_instance_bmp_per_device_stage_get
                        (unit, block_id, soc_dnxf_fabric_device_stage_fe1, &block_bmp));
    }
    else if (flags & _SHR_FABRIC_TH_FLAG_FE3_ONLY)
    {
        SHR_IF_ERR_EXIT(soc_dnxf_block_instance_bmp_per_device_stage_get
                        (unit, block_id, soc_dnxf_fabric_device_stage_fe3, &block_bmp));
    }
    else
    {
        SHR_IF_ERR_EXIT(soc_dnxf_block_instance_bmp_per_device_stage_get
                        (unit, block_id, soc_dnxf_fabric_device_stage_fe2, &block_bmp));
    }

    rc = MBCM_DNXF_DRIVER_CALL(unit, mbcm_dnxf_cgm_threshold_to_reg_table_get,
                               (unit, &threhold_to_reg_info_binding_table));
    SHR_IF_ERR_EXIT(rc);

    threshold_handle_case = threhold_to_reg_info_binding_table[threshold_type].th_handle_case;

    switch (threshold_handle_case)
    {
        case soc_dnxf_cgm_reg_structure_1:

            SHR_IF_ERR_EXIT(soc_dnxf_cgm_handle_register_structure_1
                            (unit, start_index, end_index, block_bmp, index_type, lr_nlr_offset,
                             profile_id, threshold_type, _SHR_FABRIC_FLAG_GET, threshold_value));
            break;
        case soc_dnxf_cgm_reg_structure_2:
            SHR_IF_ERR_EXIT(soc_dnxf_cgm_handle_register_structure_2
                            (unit, start_index, end_index, block_bmp, lr_nlr_offset, profile_id,
                             threshold_type, _SHR_FABRIC_FLAG_GET, threshold_value));
            break;
        case soc_dnxf_cgm_reg_structure_3:
            SHR_IF_ERR_EXIT(soc_dnxf_cgm_handle_register_structure_3
                            (unit, start_index, end_index, block_bmp, lr_nlr_offset, profile_id,
                             threshold_type, _SHR_FABRIC_FLAG_GET, threshold_value));
            break;
        case soc_dnxf_cgm_reg_structure_4:
            SHR_IF_ERR_EXIT(soc_dnxf_cgm_handle_register_structure_4
                            (unit, start_index, end_index, block_bmp, lr_nlr_offset, profile_id,
                             threshold_type, _SHR_FABRIC_FLAG_GET, threshold_value));
            break;
        case soc_dnxf_cgm_reg_structure_5:

            SHR_IF_ERR_EXIT(soc_dnxf_cgm_handle_register_structure_5
                            (unit, start_index, end_index, block_bmp, lr_nlr_offset, profile_id,
                             threshold_type, _SHR_FABRIC_FLAG_GET, threshold_value));
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Unsupported Type %d", threshold_handle_case);
            break;
    }

exit:
    SHR_FUNC_EXIT;
}

#endif

#undef BSL_LOG_MODULE
